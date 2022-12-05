#include "utils.h"
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"

using namespace std;




const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

string getFirstWord(string word)
{
  return word.substr(0, word.find_first_of(" \n"));   
}

string _ltrim(const std::string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
  return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

bool isBuiltIn (const std::string my_str)
{
   std::string my_str_2 = my_str.substr(0,my_str.find_last_of('&'));
   const char * word = my_str_2.c_str();
    if ((strcmp(word,"chprompt")==0) || (strcmp(word,"showpid")==0) || (strcmp(word,"pwd")==0) || (strcmp(word,"cd")==0) || 
    (strcmp(word,"jobs")==0) || (strcmp(word,"fg")==0) || (strcmp(word,"bg")==0) || (strcmp(word,"quit")==0) || (strcmp(word,"kill")==0)
    || (strcmp(word,"fare")==0)) {
        return true;
    }
    return false;
}


bool is_number(const std::string& stringy)
{
  std::string::const_iterator iter = stringy.begin();

  if (iter == stringy.end() || (((*iter) != '-') && (!std::isdigit(*iter))) ){
    return false;
  }
  iter++;
  while(iter != stringy.end() && std::isdigit(*iter)){
    iter++;
  }
  return ((!stringy.empty()) && (iter == stringy.end()));
}


bool is_pos_number(const std::string& stringy)
{
  std::string::const_iterator iter = stringy.begin();

  while(iter != stringy.end() && std::isdigit(*iter)){
    iter++;
  }
  return ((!stringy.empty()) && (iter == stringy.end()));
}




bool is_dashed_number(const std::string& stringy){
  std::string::const_iterator iter = stringy.begin();
  if (iter == stringy.end() || (*iter) != '-'){
    return false;
  }
  return is_number(stringy.substr(1));
}

string read_stdin ()
{
  string ret;
  char read_buff[COMMAND_ARGS_MAX_LENGTH];
  while (read(STDIN_FILENO,read_buff,COMMAND_ARGS_MAX_LENGTH)>0)
  {
    ret.append(read_buff);
  }
  return ret;
}