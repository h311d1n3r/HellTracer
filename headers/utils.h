#ifndef HELLTRACER_UTILS_H
#define HELLTRACER_UTILS_H
#include <string>
#include <vector>
using namespace std;

void split(const string &str, char c, vector<string> &elements);
bool inputToNumber(string input, unsigned long long int& number);
string toUppercase(string input);

#endif