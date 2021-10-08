#pragma once
#include <string>
#include <sstream>

using namespace std;

enum LogLevel {
    SUCCESS,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class Logger {
private:
    static Logger* instance;
public:
    static void log(LogLevel level, string message, bool bold, bool endLine, bool prefixSymbol);
    static void log(LogLevel level, string message, bool bold, bool endLine) {log(level, message, bold, endLine, true);};
    static void log(LogLevel level, string message, bool bold) {log(level, message, bold, true, true);};
    static void log(LogLevel level, string message) {log(level, message, false, true, true);};
    static Logger getLogger();
};