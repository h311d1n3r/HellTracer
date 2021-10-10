#ifndef HELLTRACER_MAIN_TYPES_H
#define HELLTRACER_MAIN_TYPES_H

#include <string>
#include <map>
#include <vector>

using namespace std;

enum Register {
    RAX,
    RBX,
    RCX,
    RDX,
    RSI,
    RDI,
    RBP,
    RSP,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    RIP,
    ALL
};

extern map<string, Register> registersFromName;

struct BinaryParams {
    string binaryPath;
    vector<string> binaryArgs;
    string outputPath;
    unsigned long long int entryAddress;
    unsigned long long int startAddress;
    unsigned long long int endAddress;
    vector<Register> trackedRegisters;
    vector<string> trackedMemoryRegions;
};

#endif