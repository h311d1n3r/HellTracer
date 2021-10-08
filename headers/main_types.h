#pragma once

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
    r14,
    R15,
    RIP,
    ALL
};

struct BinaryParams {
    unsigned long long int entryAddress;
    unsigned long long int startAddress;
    unsigned long long int endAddress;
    Register* trackedRegisters;
    unsigned long long int* trackedMemoryRegions = nullptr;
};