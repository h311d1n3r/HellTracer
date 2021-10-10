#ifndef HELLTRACER_TRACER_H
#define HELLTRACER_TRACER_H

#include <main_types.h>
#include <fstream>
#include <sys/user.h>

class HellTracer {
private:
    unsigned long long int fileEntry, effectiveEntry;
    unsigned char replacedFileEntry;
    pid_t pid;
    int memoryFd;
    BinaryParams params;
    unsigned long long int parseRegValue(Register reg, user_regs_struct regs, unsigned long long int rip);
    void writeStepResults(ofstream& outputFile, user_regs_struct regs);
    void convertArgs(vector<string> args, char** execArgs);
    bool readBinaryHeader();
public:
    HellTracer(BinaryParams params) : params(params){};
    void run();
};

#endif