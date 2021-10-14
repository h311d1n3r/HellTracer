#include <tracer.h>
#include <log.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iomanip>
#include <thread>
#include <chrono>
#include <algorithm>
#include <utils.h>
#include <sys/stat.h>

using namespace std;

vector<string> orderedRegNames({{"rip","rax","rbx","rcx","rdx","rsi","rdi","rbp","rsp","r8","r9","r10","r11","r12","r13","r14","r15"}});

unsigned long long int HellTracer::parseRegValue(Register reg, user_regs_struct regs, unsigned long long int rip) {
    switch (reg) {
        case RIP:
            return rip;
        case RAX:
            return regs.rax;
        case RBX:
            return regs.rbx;
        case RCX:
            return regs.rcx;
        case RDX:
            return regs.rdx;
        case RSI:
            return regs.rsi;
        case RDI:
            return regs.rdi;
        case RBP:
            return regs.rbp;
        case RSP:
            return regs.rsp;
        case R8:
            return regs.r8;
        case R9:
            return regs.r9;
        case R10:
            return regs.r10;
        case R11:
            return regs.r11;
        case R12:
            return regs.r12;
        case R13:
            return regs.r13;
        case R14:
            return regs.r14;
        case R15:
            return regs.r15;
    }
    return 0;
}

int HellTracer::parseMemStr(string mem, unsigned char*& buffer, user_regs_struct regs) {
    mem = trim(mem);
    if(!mem.find_first_of("[")) {
        int lastIndex = mem.find_last_of("]");
        bool end = lastIndex == mem.length()-1;
        string pointerStr = mem.substr(1, lastIndex-1);
        unsigned long long int size = sizeof(unsigned long long int);
        if(pointerStr.find(":") != string::npos) {
            inputToNumber(pointerStr.substr(pointerStr.find(":")+1), size);
            pointerStr = pointerStr.substr(0, pointerStr.find(":"));
        }
        buffer = (unsigned char*) malloc(sizeof(unsigned long long int));
        memset(buffer, 0, sizeof(unsigned long long int));
        this->parseMemStr(pointerStr, buffer, regs);
        unsigned long long int addr = *((unsigned long long int*)buffer);
        buffer = (unsigned char*) malloc(size);
        unsigned long long int readSize = pread(this->memoryFd, buffer, size, addr);
        if(end) return readSize;
        else {
            stringstream pointedValStr;
            pointedValStr << "0x";
            for(int i = readSize-1; i >= 0; i--) pointedValStr << hex << setfill('0') << setw(2) << +buffer[i];
            return parseMemStr(pointedValStr.str()+mem.substr(lastIndex+1), buffer, regs);
        }
    } else {
        string skipped = "";
        while(mem.length() > 0) {
            if(!mem.find("+")) {
                string addStr = mem.substr(1);
                parseMemStr(skipped, buffer, regs);
                unsigned long long int add1 = *((unsigned long long int*) buffer);
                parseMemStr(addStr, buffer, regs);
                unsigned long long int add2 = *((unsigned long long int*) buffer);
                unsigned long long int result = add1 + add2;
                buffer = (unsigned char*) &result;
                return sizeof(result);
            } else if(!mem.find("-")) {
                string subStr = mem.substr(1);
                parseMemStr(skipped, buffer, regs);
                unsigned long long int sub1 = *((unsigned long long int*) buffer);
                parseMemStr(subStr, buffer, regs);
                unsigned long long int sub2 = *((unsigned long long int*) buffer);
                unsigned long long int result = sub1 - sub2;
                buffer = (unsigned char*) &result;
                return sizeof(result);
            } else if(!mem.find("*")) {
                string mulStr = mem.substr(1);
                parseMemStr(skipped, buffer, regs);
                unsigned long long int mul1 = *((unsigned long long int*) buffer);
                parseMemStr(mulStr, buffer, regs);
                unsigned long long int mul2 = *((unsigned long long int*) buffer);
                unsigned long long int result = mul1 * mul2;
                buffer = (unsigned char*) &result;
                return sizeof(result);
            } else if(!mem.find("/")) {
                string divStr = mem.substr(1);
                parseMemStr(skipped, buffer, regs);
                unsigned long long int div1 = *((unsigned long long int*) buffer);
                parseMemStr(divStr, buffer, regs);
                unsigned long long int div2 = *((unsigned long long int*) buffer);
                unsigned long long int result = div1 / div2;
                buffer = (unsigned char*) &result;
                return sizeof(result);
            } else {
                skipped += mem.at(0);
                mem = mem.substr(1);
            }
        }
        if(skipped.length() >= 0) {
            unsigned long long int val;
            bool relativeToEntry = false;
            if(!skipped.find("@")) {
                relativeToEntry = true;
                skipped = skipped.substr(1);
            }
            if(!inputToNumber(skipped, val)) val = parseRegValue(registersFromName[skipped], regs, regs.rip);
            if(relativeToEntry) val = val - this->params.entryAddress + this->effectiveEntry;
            buffer = (unsigned char*) &val;
            return sizeof(val);
        }
    }
    return -1;
}

void HellTracer::writeStepResults(ofstream& outputFile, user_regs_struct regs) {
    unsigned long long int rip = this->params.entryAddress ? (this->params.entryAddress + regs.rip - this->effectiveEntry) : regs.rip;
    if((!params.startAddress || params.startAddress <= rip) && (!params.endAddress || params.endAddress >= rip)) {
        for (string regName: orderedRegNames) {
            if (params.trackedRegisters[0] == Register::ALL ||
                find(params.trackedRegisters.begin(), params.trackedRegisters.end(), registersFromName[regName]) !=
                params.trackedRegisters.end()) {
                unsigned long long int regValue = parseRegValue(registersFromName[regName], regs, rip);
                outputFile << "=\"0x" << hex << setfill('0') << setw(8) << regValue << "\",";
            }
        }
        for(string mem : this->params.trackedMemoryRegions) {
            string subMem = mem;
            bool printAscii = false;
            if(!mem.find("ascii=")) {
                subMem = mem.substr(6);
                printAscii = true;
            }
            unsigned char* buffer;
            int read = this->parseMemStr(subMem, buffer, regs);
            outputFile << "=\"";
            for(int i = 0; i < read; i++) {
                if(printAscii && buffer[i] >= 0x20 && buffer[i] <= 0x7e) outputFile << buffer[i];
                else outputFile << hex << setfill('0') << setw(2) << +buffer[i];
            }
            outputFile << "\",";
        }
        outputFile << endl;
    }
}

bool HellTracer::readBinaryHeader() {
    ifstream binaryRead(this->params.binaryPath, ifstream::binary);
    if(binaryRead) {
        binaryRead.seekg(0, binaryRead.end);
        int len = binaryRead.tellg();
        binaryRead.seekg(0, binaryRead.beg);
        if(len >= 0x20) {
            char* idField = (char*) malloc(0x20);
            binaryRead.read(idField, 0x20);
            if(idField[0] == 0x7f && idField[1] == 'E' && idField[2] == 'L' && idField[3] == 'F') {
                if(idField[4] == 2) {
                    if(idField[5] == 1) {
                        if(idField[7] == 0 || idField[7] == 3) {
                            this->fileEntry = *((unsigned long long int*)(idField + 0x18));
                            binaryRead.seekg(this->fileEntry, binaryRead.beg);
                            binaryRead.read((char*)&this->replacedFileEntry, 1);
                            binaryRead.close();
                            ofstream binaryWrite;
                            binaryWrite.open(this->params.binaryPath, ofstream::binary | ofstream::out | ofstream::in);
                            binaryWrite.seekp(this->fileEntry, binaryWrite.beg);
                            const char breakpoint = 0xCC;
                            binaryWrite.write(&breakpoint, 1);
                            stringstream msg;
                            msg << "Replaced byte 0x" << hex << setfill('0') << setw(2) << +this->replacedFileEntry << " at address 0x" << hex << setfill('0') << setw(8) << +this->fileEntry << " (entry point) with byte 0xcc (breakpoint).";
                            Logger::getLogger().log(LogLevel::INFO, msg.str());
                            binaryWrite.close();
                            return true;
                        } else Logger::getLogger().log(LogLevel::FATAL, "The specified file can't be run on a Linux architecture.");
                    } else Logger::getLogger().log(LogLevel::FATAL, "HellTracer currently handles little endian files only.");
                } else Logger::getLogger().log(LogLevel::FATAL, "HellTracer currently handles x64 files only.");
            } else Logger::getLogger().log(LogLevel::FATAL, "The specified binary is not a ELF file.");
        } else {
            stringstream error;
            error << "The specified binary file is only " << len << " bytes long.";
            Logger::getLogger().log(LogLevel::FATAL, error.str());
        }
        binaryRead.close();
    } else Logger::getLogger().log(LogLevel::FATAL, "An error occured while trying to open the specified binary file.");
    return false;
}

void HellTracer::convertArgs(vector<string> args, char** execArgs) {
    int argsAmount = 0;
    for (int i = 0; i < args.size(); i++) {
        execArgs[argsAmount++] = strdup(args[i].c_str());
    }
    execArgs[argsAmount++] = NULL;
}

void HellTracer::run() {
    if(!this->readBinaryHeader()) return;
    ofstream outputFile;
    outputFile.open(this->params.outputPath, ofstream::out);
    int outputNameIndex = this->params.binaryPath.find_last_of("/");
    if(outputNameIndex == string::npos) outputNameIndex = 0;
    else outputNameIndex++;
    outputFile << "Execution of " << this->params.binaryPath.substr(outputNameIndex) << endl;
    for(string regName : orderedRegNames) {
        if(params.trackedRegisters[0] == Register::ALL || find(params.trackedRegisters.begin(), params.trackedRegisters.end(), registersFromName[regName]) != params.trackedRegisters.end()) {
            outputFile << toUppercase(regName) << ",";
        }
    }
    for(string mem : this->params.trackedMemoryRegions) {
        string upper = toUppercase(mem);
        upper = trim(upper);
        for(int i = 0; i < upper.length()-1; i++) if(upper[i] == '0' && upper[i+1] == 'X') upper[i+1] = 'x';
        outputFile << upper << ",";
    }
    outputFile << endl;
    this->pid = fork();
    if(!this->pid) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        char* execArgs[1024];
        convertArgs(params.binaryArgs, execArgs);
        execv(this->params.binaryPath.c_str(), execArgs);
    }
    waitpid(this->pid, NULL, 0);
    ptrace(PTRACE_CONT, pid, NULL, NULL);
    //breakpoint set in binary
    waitpid(this->pid, NULL, 0);
    char file[64];
    sprintf(file, "/proc/%ld/mem", (long)this->pid);
    this->memoryFd = open(file, O_RDWR);
    if(this->memoryFd != -1) {
        user_regs_struct regs;
        Logger::getLogger().log(LogLevel::SUCCESS, "Target is running and ready to be traced !");
        unsigned char* buf = (unsigned char*) malloc(10);
        pread(this->memoryFd, buf, 10, regs.rip);
        ptrace(PTRACE_GETREGS,this->pid,NULL,&regs);
        regs.rip--;
        this->effectiveEntry = regs.rip;
        pwrite(this->memoryFd, &this->replacedFileEntry, 1, this->effectiveEntry);
        pread(this->memoryFd, buf, 10, regs.rip);
        ptrace(PTRACE_SETREGS,this->pid,NULL,&regs);
        pid_t alive = this->pid;
        while(alive == this->pid) {
            this->writeStepResults(outputFile, regs);
            ptrace(PTRACE_SINGLESTEP, this->pid, NULL, NULL);
            alive = waitpid(this->pid, NULL, 0);
            ptrace(PTRACE_GETREGS,this->pid,NULL,&regs);
        }
        close(this->memoryFd);
    } else Logger::getLogger().log(LogLevel::FATAL, "Unable to access memory of running target. Missing rights ?");
    ofstream binaryWrite;
    do {
        binaryWrite.open(this->params.binaryPath, ofstream::binary | ofstream::out | ofstream::in);
        this_thread::sleep_for(std::chrono::milliseconds(10));
    } while(!binaryWrite.is_open());
    binaryWrite.seekp(this->fileEntry, binaryWrite.beg);
    binaryWrite.write((char*)&this->replacedFileEntry, 1);
    binaryWrite.close();
    Logger::getLogger().log(LogLevel::INFO, "Restored breakpoint byte.");
    outputFile.close();
    chmod(this->params.outputPath.c_str(), S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH);
}