#ifndef UTILS_H_
#define UTILS_H_

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>


using namespace std;

string getFirstWord(string word);

string _ltrim(const std::string& s);

string _rtrim(const std::string& s);

string _trim(const std::string& s);

int _parseCommandLine(const char* cmd_line, char** args);

bool _isBackgroundComamnd(const char* cmd_line);

void _removeBackgroundSign(char* cmd_line);

bool isBuiltIn (const string my_str);

bool is_number(const std::string& stringy);

bool is_dashed_number(const std::string& stringy);

bool is_pos_number(const std::string& stringy);

string read_stdin ();


#endif //UTILS_H_
