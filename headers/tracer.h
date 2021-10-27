#ifndef HELLTRACER_TRACER_H
#define HELLTRACER_TRACER_H

#include <main_types.h>
#include <fstream>
#include <sys/user.h>
#include <cstdlib>

class HellTracer {
private:
    unsigned long long int fileEntry, effectiveEntry;
    unsigned char replacedFileEntry;
    pid_t pid;
    int memoryFd;
    BinaryParams params;
    bool fixedEntryBreakpoint = false;
    unsigned long long int parseRegValue(Register reg, user_regs_struct regs, unsigned long long int rip);
    int parseMemStr(string mem, unsigned char*& buffer, user_regs_struct regs);
    void writeStepResults(ofstream& outputFile, user_regs_struct regs);
    void convertArgs(vector<string> args, char** execArgs);
    bool readBinaryHeader();
    void fixEntryBreakpoint();
public:
    HellTracer(BinaryParams params) : params(params){};
    void run();
    void handleExit();
};

#endif