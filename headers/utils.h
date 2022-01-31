#ifndef HELLTRACER_UTILS_H
#define HELLTRACER_UTILS_H
#include <string>
#include <vector>
using namespace std;

/**
 * @brief Splits a string into a string vector, cutting at a given character
 * @param str String to be splitted
 * @param c Character to cut at
 * @param elements Resulting string vector
 */
void split(const string &str, char c, vector<string> &elements);
/**
 * @brief Removes spaces from a string
 * @param str String to be trimmed
 * @return The string without spaces
 */
string trim(string str);
/**
 * @brief Converts a string representing a number (decimal or hexadecimal) into an unsigned long long int
 * @param input The string to be converted in a number
 * @param number The converted number
 * @return True if a number could be extracted, false otherwise
 */
bool inputToNumber(string input, unsigned long long int& number);
/**
 * @brief Converts a string into its uppercase version
 * @param input The string to be converted
 * @return The converted string
 */
string toUppercase(string input);
/**
 * @brief Converts a string into its lowercase version
 * @param input The string to be converted
 * @return The converted string
 */
string toLowercase(string input);

#endif