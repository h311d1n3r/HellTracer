#ifndef HELLTRACER_LOG_H
#define HELLTRACER_LOG_H

#include <string>
#include <sstream>

using namespace std;

/**
 * @brief An enumeration of the different log levels available in the application
 */
enum LogLevel {
    SUCCESS,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

/**
 * @brief This class is used to print formatted log messages
 */
class Logger {
private:
    /**
     * @brief The unique instance of the Logger class as required in every implementation of singleton pattern
     */
    static Logger* instance;
public:
    /**
     * @brief Prints a formatted message
     * @param level The log level
     * @param message The original text to format and print
     * @param bold True if the font of the message must be bold, false otherwise
     * @param endLine True if a carriage return must be added at the end of the message, false otherwise
     * @param prefixSymbol True if a symbol representing the log level must be printed at the start of the message, false otherwise
     */
    void log(LogLevel level, string message, bool bold, bool endLine, bool prefixSymbol);
    /**
     * @brief Prints a formatted message
     * @param level The log level
     * @param message The original text to format and print
     * @param bold True if the font of the message must be bold, false otherwise
     * @param endLine True if a carriage return must be added at the end of the message, false otherwise
     */
    void log(LogLevel level, string message, bool bold, bool endLine) {log(level, message, bold, endLine, true);};
    /**
     * @brief Prints a formatted message
     * @param level The log level
     * @param message The original text to format and print
     * @param bold True if the font of the message must be bold, false otherwise
     */
    void log(LogLevel level, string message, bool bold) {log(level, message, bold, true, true);};
    /**
     * @brief Prints a formatted message
     * @param level The log level
     * @param message The original text to format and print
     */
    void log(LogLevel level, string message) {log(level, message, false, true, true);};
    /**
     * @brief Returns the unique instance of the Logger class as required in every implementation of singleton pattern
     * @return The Logger instance
     */
    static Logger getLogger();
};

#endif