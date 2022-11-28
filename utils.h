#ifndef UTILS_H_
#define UTILS_H_

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include "smash.cpp"

using namespace std;

string getFirstWord(string word);
int stringToArray(string word, string  *arr);



#endif //UTILS_H_
