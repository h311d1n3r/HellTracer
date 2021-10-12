#include <iostream>
#include <filesystem>
#include <string>
#include <main_types.h>
#include <log.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <tracer.h>
#include <utils.h>

using namespace std;

const string TOOL_TITLE = "  _  _     _ _ _____                    \n"
                          " | || |___| | |_   _| _ __ _ __ ___ _ _ \n"
                          " | __ / -_) | | | || '_/ _` / _/ -_) '_|\n"
                          " |_||_\\___|_|_| |_||_| \\__,_\\__\\___|_|  \n"
                          "                                        ";
const string VERSION = "1.0";
const string AUTHOR = "h311d1n3r";

BinaryParams params;
map<string, Register> registersFromName = {
        {"rax", Register::RAX},
        {"rbx", Register::RBX},
        {"rcx", Register::RCX},
        {"rdx", Register::RDX},
        {"rsi", Register::RSI},
        {"rdi", Register::RDI},
        {"rbp", Register::RBP},
        {"rsp", Register::RSP},
        {"r8", Register::R8},
        {"r9", Register::R9},
        {"r10", Register::R10},
        {"r11", Register::R11},
        {"r12", Register::R12},
        {"r13", Register::R13},
        {"r14", Register::R14},
        {"r15", Register::R15},
        {"rip", Register::RIP},
        {"all", Register::ALL}
};

void printHelpMessage() {
    Logger::getLogger().log(LogLevel::INFO, TOOL_TITLE, false, true, false);
    stringstream version;
    version << " Version: " << VERSION;
    Logger::getLogger().log(LogLevel::INFO, version.str(), false, true, false);
    stringstream author;
    author << " Author: " << AUTHOR;
    Logger::getLogger().log(LogLevel::INFO, author.str(), false, false, false);
    Logger::getLogger().log(LogLevel::INFO, "________________________________________\n", false, true, false);
    Logger::getLogger().log(LogLevel::INFO, "Syntax: \033[1;33mhelltracer binary [-param value] [--flag]\n", false, true, false);
    Logger::getLogger().log(LogLevel::INFO, "Parameters:", false, true, false);
    stringstream paramsList;
    paramsList << "\033[1;33m   entry\033[0;36m -> Specifies the program entry point. e.g: 'entry 0x401000'" << endl;
    paramsList << "\033[1;33m   start\033[0;36m -> Specifies the program start address for tracing. e.g: 'entry 0x4010f0'" << endl;
    paramsList << "\033[1;33m   end\033[0;36m -> Specifies the program end address for tracing. e.g: 'entry 0x40115e'" << endl;
    paramsList << "\033[1;33m   mem\033[0;36m -> Turns on tracing of the specified memory range. e.g: 'mem 0x7f5310:0x7f531e', 'mem rsp+4:rsp+12', 'mem ascii=rbp-0x20:rbp-0x1c'" << endl;
    paramsList << "\033[1;33m   args\033[0;36m -> Specifies the arguments to be passed to the traced binary. e.g: 'args \"./name.bin hello world\"'" << endl;
    paramsList << "\033[1;33m   output\033[0;36m -> Specifies the output file (.csv). e.g: 'output out.csv'" << endl;
    Logger::getLogger().log(LogLevel::INFO, paramsList.str(), false, true, false);
    Logger::getLogger().log(LogLevel::INFO, "Flags:", false, true, false);
    stringstream flagsList;
    flagsList << "\033[1;33m   help\033[0;36m -> Displays this message." << endl;
    flagsList << "\033[1;33m   reg_name\033[0;36m -> Turns on tracing of register reg_name. e.g: 'rip', 'rcx', 'rsp', 'all'" << endl;
    Logger::getLogger().log(LogLevel::INFO, flagsList.str(), false, true, false);
}

bool analyseParam(string param, string val) {
    if(!param.find("--")) {
        string paramName = param.substr(2);
        if(!paramName.compare("help")) printHelpMessage();
        if(registersFromName.count(paramName)) {
            if(!paramName.compare("all")) params.trackedRegisters.clear();
            params.trackedRegisters.push_back(registersFromName[paramName]);
        }
    } else if(!param.find("-")) {
        string paramName = param.substr(1);
        if(!val.length()) {
            stringstream msg;
            msg << "Parameter \033[;37m" << paramName << "\033[;31m requires a value !";
            Logger::getLogger().log(LogLevel::FATAL, msg.str());
            return false;
        }
        if(!paramName.compare("entry")) {
            unsigned long long int entryAddress = 0;
            if(inputToNumber(val, entryAddress)) {
                params.entryAddress = entryAddress;
            } else {
                stringstream msg;
                msg << "Specified entry address \033[;37m" << val << "\033[;31m is not a number !";
                Logger::getLogger().log(LogLevel::FATAL, msg.str());
                return false;
            }
        } else if(!paramName.compare("start")) {
            unsigned long long int startAddress = 0;
            if(inputToNumber(val, startAddress)) {
                params.startAddress = startAddress;
            } else {
                stringstream msg;
                msg << "Specified start address \033[;37m" << val << "\033[;31m is not a number !";
                Logger::getLogger().log(LogLevel::FATAL, msg.str());
                return false;
            }
        } else if(!paramName.compare("end")) {
            unsigned long long int endAddress = 0;
            if(inputToNumber(val, endAddress)) {
                params.endAddress = endAddress;
            } else {
                stringstream msg;
                msg << "Specified end address \033[;37m" << val << "\033[;31m is not a number !";
                Logger::getLogger().log(LogLevel::FATAL, msg.str());
                return false;
            }
        } else if(!paramName.compare("mem")) {
            string subVal = val;
            if(!val.find("ascii=")) subVal = val.substr(6);
            int sep = subVal.find(":");
            if(sep != string::npos && sep != subVal.length()-1) {
                string val1 = subVal.substr(0,sep);
                string val2 = subVal.substr(sep+1);
                sep = val1.find("+");
                unsigned long long int number = 0;
                if(sep != string::npos && sep != val1.length()-1) {
                    string regName = val1.substr(0,sep);
                    string off = val1.substr(sep+1);
                    if(!registersFromName.count(regName)) {
                        Logger::getLogger().log(LogLevel::FATAL, "Parameter mem : Register "+regName+" does not exist.");
                        return false;
                    }
                    else if(!inputToNumber(off, number)) {
                        Logger::getLogger().log(LogLevel::FATAL, "Parameter mem : "+off+" is not a number.");
                        return false;
                    }
                } else {
                    sep = val1.find("-");
                    if(sep != string::npos && sep != val1.length()-1) {
                        string regName = val1.substr(0,sep);
                        string off = val1.substr(sep+1);
                        if(!registersFromName.count(regName)) {
                            Logger::getLogger().log(LogLevel::FATAL, "Parameter mem : Register "+regName+" does not exist.");
                            return false;
                        }
                        else if(!inputToNumber(off, number)) {
                            Logger::getLogger().log(LogLevel::FATAL, "Parameter mem : "+off+" is not a number.");
                            return false;
                        }
                    } else if(!inputToNumber(val1, number)) {
                        Logger::getLogger().log(LogLevel::FATAL, "Parameter mem : "+val1+" is not a number.");
                        return false;
                    }
                }
                sep = val2.find("+");
                if(sep != string::npos && sep != val2.length()-1) {
                    string regName = val2.substr(0,sep);
                    string off = val2.substr(sep+1);
                    if(!registersFromName.count(regName)) {
                        Logger::getLogger().log(LogLevel::FATAL, "Parameter mem : Register "+regName+" does not exist.");
                        return false;
                    }
                    else if(!inputToNumber(off, number)) {
                        Logger::getLogger().log(LogLevel::FATAL, "Parameter mem : "+off+" is not a number.");
                        return false;
                    }
                } else {
                    sep = val2.find("-");
                    if(sep != string::npos && sep != val2.length()-1) {
                        string regName = val2.substr(0,sep);
                        string off = val2.substr(sep+1);
                        if(!registersFromName.count(regName)) {
                            Logger::getLogger().log(LogLevel::FATAL, "Parameter mem : Register "+regName+" does not exist.");
                            return false;
                        }
                        else if(!inputToNumber(off, number)) {
                            Logger::getLogger().log(LogLevel::FATAL, "Parameter mem : "+off+" is not a number.");
                            return false;
                        }
                    } else if(!inputToNumber(val2, number)) {
                        Logger::getLogger().log(LogLevel::FATAL, "Parameter mem : "+val2+" is not a number.");
                        return false;
                    }
                }
            } else {
                Logger::getLogger().log(LogLevel::FATAL, "Parameter mem requires value to be like 'start:end'");
                return false;
            }
            params.trackedMemoryRegions.push_back(val);
        } else if(!paramName.compare("args")) {
            vector<string> args;
            split(val, ' ', args);
            params.binaryArgs = args;
        } else if(!paramName.compare("output")) params.outputPath = (val[0] == '/' ? "" : filesystem::current_path().u8string()) + "/" + val;
    }
    return true;
}

bool checkSudo() {
    uid_t effectiveUID = geteuid();
    return !effectiveUID;
}

int main(int argc, char* argv[]) {
    if(argc > 1) {
        if(!strcmp(argv[1], "--help")) {
            printHelpMessage();
            return 0;
        }
        char* binaryName = argv[1];
        string binaryPath = (binaryName[0] == '/' ? "" : filesystem::current_path().u8string()) + "/" + binaryName;
        if(filesystem::exists(binaryPath)) {
            params.binaryPath = binaryPath;
            for (int i = 2; i < argc; i++) {
                if(!analyseParam(argv[i], i+1 < argc ? argv[i+1] : "")) return 0;
            }
            if(!params.trackedRegisters.size() && !params.trackedMemoryRegions.size()) {
                params.trackedRegisters.push_back(Register::ALL);
                Logger::getLogger().log(LogLevel::ERROR, "No registers or memory regions have been specified for tracking. All registers will be tracked by default.");
            }
            if(!params.outputPath.length()) {
                params.outputPath = filesystem::current_path().u8string() + "/out.csv";
                Logger::getLogger().log(LogLevel::INFO, "No output file has been specified. Results will be written to \033[;37m"+params.outputPath);
            }
            if(!params.binaryArgs.size()) params.binaryArgs.push_back(params.binaryPath);
            if(!checkSudo()) Logger::getLogger().log(LogLevel::WARNING, "Running the tool without root rights (sudo) may cause issues.");
            HellTracer tracer(params);
            tracer.run();
        } else {
            stringstream msg;
            msg << "File \033[;37m" << binaryPath << "\033[;31m does not exist !";
            Logger::getLogger().log(LogLevel::FATAL, msg.str());
        }
    } else {
        Logger::getLogger().log(LogLevel::FATAL, "You need to specify more parameters -> \033[;37mhelltracer binary");
    }
    return 0;
}