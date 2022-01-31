#ifndef HELLTRACER_TRACER_H
#define HELLTRACER_TRACER_H

#include <main_types.h>
#include <fstream>
#include <sys/user.h>
#include <cstdlib>

/**
 * @brief Organizes debugging of the targeted binary and writes results into the CSV file
 */
class HellTracer {
private:
    /**
     * @brief Target static entry point, read from the ELF header
     */
    unsigned long long int fileEntry;
    /**
     * @brief Target dynamic entry point, read when execution starts
     */
    unsigned long long int effectiveEntry;
    /**
     * @brief Value of entry point byte, replaced by a breakpoint in order to read effective entry
     */
    unsigned char replacedFileEntry;
    /**
     * @brief Process ID of the target
     */
    pid_t pid;
    /**
     * @brief Handles memory file of the target
     */
    int memoryFd;
    /**
     * @brief Options relative to the targeted binary
     */
    BinaryParams params;
    /**
     * @brief True if the entry point byte was set back to its original value, false otherwise
     */
    bool fixedEntryBreakpoint = false;
    /**
     * @brief Retrieves the value of a register from a Register field
     * @param reg The register field
     * @param regs The struct to extract the register from
     * @param ip The instruction pointer
     * @return The register value
     * @sa parseMemStr(), writeStepResults()
     */
    unsigned long long int parseRegValue(Register reg, user_regs_struct regs, unsigned long long int ip);
    /**
     * @brief Parses a string and retrieves the bytes at the specified memory location
     * @param mem The string to be parsed
     * @param buffer The buffer used to store the extracted bytes
     * @param regs The struct that contains the current state registers
     * @return If the function succeeds, returns the amount of read bytes, otherwise returns -1
     * @sa parseRegValue(), writeStepResults()
     */
    int parseMemStr(string mem, unsigned char*& buffer, user_regs_struct regs);
    /**
     * @brief Writes the line of current step in the output CSV file
     * @param outputFile A handle to the output file
     * @param regs The struct that contains the current state registers
     * @sa run()
     */
    void writeStepResults(ofstream& outputFile, user_regs_struct regs);
    /**
     * @brief Converts program parameters from a vector<string> into a char**
     * @param args The input parameters
     * @param execArgs The output parameters
     * @sa run()
     */
    void convertArgs(vector<string> args, char** execArgs);
    /**
     * @brief Extracts data and makes some checks relative to the ELF header of the target
     * @return True if the specified file can be targeted by the tool, false otherwise
     * @sa run()
     */
    bool readBinaryHeader();
    /**
     * @brief Sets back the replaced breakpoint byte
     * @sa handleExit()
     */
    void fixEntryBreakpoint();
public:
    /**
     * @brief Basic constructor for HellTracer instances
     * @param params Options relative to the targeted binary
     */
    HellTracer(BinaryParams params) : params(params){};
    /**
     * @brief Starts debugging the target
     */
    void run();
    /**
     * @brief Called when the user exits the program and at the end of execution
     * @sa fixEntryBreakpoint(), run()
     */
    void handleExit();
};

#endif