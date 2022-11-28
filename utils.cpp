#include "utils.h"
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


string getFirstWord(string word)
{
  return word.substr(0, word.find_first_of(" \n"));   
}
//get a line a parsh into words in an array. each enyery in the array will be one of the words in the line and the return value is the number of words.
int stringToArray(string line, string  *arr) 
{
    int firstWordEndingPos= line.find_first_of(" \n");
    int index=0;
    string currWord=getFirstWord(line);
    string restWord=line.substr(firstWordEndingPos,line.length());
    arr[0]=currWord;
    while (currWord.length()!=0)
    {
         firstWordEndingPos= restWord.find_first_of(" \n");
         index++;
         currWord=getFirstWord(restWord);
         restWord=word.substr(firstWordEndingPos,restWord.length());
         arr[index]=currWord;
    }
    return index;
}

