#pragma once
#include <main_types.h>

class HellTracer {
private:
    BinaryParams params;
public:
    HellTracer(BinaryParams params) : this.params = params;
    void run();
};