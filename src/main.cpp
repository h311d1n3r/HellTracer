#include <iostream>
#include <filesystem>
#include <string>
#include <main_types.h>
#include <log.h>
#include <string.h>

using namespace std;

const string TOOL_TITLE = "  _  _     _ _ _____                    \n"
                          " | || |___| | |_   _| _ __ _ __ ___ _ _ \n"
                          " | __ / -_) | | | || '_/ _` / _/ -_) '_|\n"
                          " |_||_\\___|_|_| |_||_| \\__,_\\__\\___|_|  \n"
                          "                                        ";
const string VERSION = "1.0";
const string AUTHOR = "h311d1n3r";

BinaryParams params;

bool inputToNumber(string input, unsigned long long int& number) {
    size_t inputEnd;
    int base = 10;
    if(input.find("0x") == 0) {
        base = 16;
        input = input.substr(2);
    } else if(input.find("a") != string::npos || input.find("b") != string::npos || input.find("c") != string::npos
        || input.find("d") != string::npos || input.find("e") != string::npos || input.find("f") != string::npos) base = 16;
    number = stoull(input, &inputEnd, base);
    return inputEnd;
}

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
    paramsList << "\033[1;33m   mem\033[0;36m -> Turns on tracing of the specified memory range. e.g: 'mem 0x7f5310-0x0x7f531e', 'mem rsp+4-rsp+12'" << endl;
    Logger::getLogger().log(LogLevel::INFO, paramsList.str(), false, true, false);
    Logger::getLogger().log(LogLevel::INFO, "Flags:", false, true, false);
    stringstream flagsList;
    flagsList << "\033[1;33m   help\033[0;36m -> Displays this message." << endl;
    flagsList << "\033[1;33m   reg_name\033[0;36m -> Turns on tracing of register reg_name. e.g: 'rip', 'rcx', 'rsp'" << endl;
    Logger::getLogger().log(LogLevel::INFO, flagsList.str(), false, true, false);
}

bool analyseParam(string param, string val) {
    if(param.find("--") == 0) {
        string paramName = param.substr(2);
        if(!paramName.compare("help")) printHelpMessage();
    } else if(param.find("-") == 0) {
        string paramName = param.substr(1);
        if(!val.length()) {
            stringstream msg;
            msg << "Parameter \033[;37m" << paramName << "\033[;31m requires a value !" << endl;
            Logger::getLogger().log(LogLevel::FATAL, msg.str());
            return false;
        }
        if(!paramName.compare("entry")) {
            unsigned long long int entryAddress = 0;
            if(inputToNumber(val, entryAddress)) {
                params.entryAddress = entryAddress;
            } else {
                stringstream msg;
                msg << "Specified entry address \033[;37m" << val << "\033[;31m is not a number !" << endl;
                Logger::getLogger().log(LogLevel::FATAL, msg.str());
                return false;
            }
        } else if(!paramName.compare("start")) {
            unsigned long long int startAddress = 0;
            if(inputToNumber(val, startAddress)) {
                params.startAddress = startAddress;
            } else {
                stringstream msg;
                msg << "Specified start address \033[;37m" << val << "\033[;31m is not a number !" << endl;
                Logger::getLogger().log(LogLevel::FATAL, msg.str());
                return false;
            }
        } else if(!paramName.compare("end")) {
            unsigned long long int endAddress = 0;
            if(inputToNumber(val, endAddress)) {
                params.endAddress = endAddress;
            } else {
                stringstream msg;
                msg << "Specified end address \033[;37m" << val << "\033[;31m is not a number !" << endl;
                Logger::getLogger().log(LogLevel::FATAL, msg.str());
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if(argc > 1) {
        if(!strcmp(argv[1], "--help")) {
            printHelpMessage();
            return 0;
        }
        char* binaryName = argv[1];
        string binaryPath = filesystem::current_path().u8string() + "/" + binaryName;
        if(filesystem::exists(binaryPath)) {
            for (int i = 2; i < argc; i++) {
                if(!analyseParam(argv[i], i+1 < argc ? argv[i+1] : "")) return 0;
            }

        } else {
            stringstream msg;
            msg << "File \033[;37m" << binaryPath << "\033[;31m does not exist !" << endl;
            Logger::getLogger().log(LogLevel::FATAL, msg.str());
        }
    } else {
        Logger::getLogger().log(LogLevel::FATAL, "You need to specify more parameters -> \033[;37mhelltracer binary\n");
    }
    return 0;
}