#ifndef UTILS_H_
#define UTILS_H_

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"

using namespace std;

string getFirstWord(string word);

string _ltrim(const std::string& s);

string _rtrim(const std::string& s);

string _trim(const std::string& s);

int _parseCommandLine(const char* cmd_line, char** args);

bool _isBackgroundComamnd(const char* cmd_line);

void _removeBackgroundSign(char* cmd_line);

bool shouldNotIgnoreAmpersand (string my_str);

#endif //UTILS_H_
