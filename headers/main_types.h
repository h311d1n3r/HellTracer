#ifndef HELLTRACER_MAIN_TYPES_H
#define HELLTRACER_MAIN_TYPES_H

#include <string>
#include <map>
#include <vector>

using namespace std;

#if __x86_64__
/**
 * @brief An enumeration of all registers present in the architecture
 */
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
#else
enum Register {
    EAX,
    EBX,
    ECX,
    EDX,
    ESI,
    EDI,
    EBP,
    ESP,
    EIP,
    ALL
};
#endif

/**
 * @brief Contains Register fields as values and their string representations as keys
 */
extern map<string, Register> registersFromName;

/**
 * @brief Stores options relative to a targeted binary
 */
struct BinaryParams {
    /**
     * @brief The path to the binary file
     */
    string binaryPath;
    /**
     * @brief The parameters to be passed when executing the binary
     */
    vector<string> binaryArgs;
    /**
     * @brief The path of the resulting CSV file
     */
    string outputPath;
    /**
     * @brief The entry point address to adjust all addresses on
     */
    unsigned long long int entryAddress;
    /**
     * @brief Addresses greater than this one will be written in the output CSV file
     */
    unsigned long long int startAddress;
    /**
     * @brief Addresses smaller than this one will be written in the output CSV file
     */
    unsigned long long int endAddress;
    /**
     * @brief The registers which values must be written on each line of the output CSV file
     */
    vector<Register> trackedRegisters;
    /**
     * @brief The memory regions (address + length) which values must be written on each line of the output CSV file
     */
    vector<string> trackedMemoryRegions;
};

#endif