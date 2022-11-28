#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include "utils.cpp"

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

// TODO: Add your implementation for classes in Commands.h 

SmallShell::SmallShell() {
// TODO: add your implementation
}

SmallShell::~SmallShell() {
// TODO: add your implementation
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

  if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  
  if (firstWord.compare("chprompt") == 0) {
    return new ShowPidCommand(cmd_line);
  }
	// For example:
/*
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

  if (firstWord.compare("pwd") == 0) {
    return new GetCurrDirCommand(cmd_line);
  }
  else if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  else if ...
  .....
  else {
    return new ExternalCommand(cmd_line);
  }
  */
  return nullptr;
}

void SmallShell::executeCommand(const char *cmd_line) {

	
if (*cmd_line == '\r' || *cmd_line == '\n'){
	std::cout << std::endl;
   	return;
}
if (*cmd_line == '\0'){
	exit(0);
}

  Command* cmd = this -> CreateCommand(cmd_line/*, getpid()*/);
  cmd -> execute();

  
  // TODO: Add your implementation here
  // for example:
  // Command* cmd = CreateCommand(cmd_line);
  // cmd->execute();
  // Please note that you must fork smash process for some commands (e.g., external commands....)
}































Command::Command(const char* cmd_line):
  m_cmd_line(_trim(string(cmd_line))), m_first_word(m_cmd_line.substr(0, m_cmd_line.find_first_of(" \n"))){}

  /**
  string cmd_s = new string(_trim(string(cmd_line)));
  string firstWord = new string(cmd_s.substr(0, cmd_s.find_first_of(" \n")));
  this -> m_cmd_line = cmd_s;
  this -> m_first_word = firstWord;
}*****/

BuiltInCommand::BuiltInCommand(const char* cmd_line)
  :Command(cmd_line){}


RedirectionCommand::RedirectionCommand(const char* cmd_line)
  :Command(cmd_line){}


ChangeDirCommand::ChangeDirCommand(const char* cmd_line)
  :BuiltInCommand(cmd_line){}

GetCurrDirCommand::GetCurrDirCommand(const char* cmd_line)
  :BuiltInCommand(cmd_line){}

ShowPidCommand::ShowPidCommand(const char* cmd_line)
  :BuiltInCommand(cmd_line){}

ChpromptCommand::ChpromptCommand(const char* cmd_line)
  :BuiltInCommand(cmd_line){}

  
void ShowPidCommand::execute(){
  std::cout << "smash pid is "<< getpid()<< std::endl;
}

void ChpromptCommand::execute(std::string* prompt){
  char* arr[COMMAND_MAX_ARGS]; 
  int numberOfArgs= _parseCommandLine( (this->m_cmd_line).c_str(),arr);
  if (numberOfArgs!=1)
  {
    std::cout <<  "smash error:>\""<< this->m_first_word<<"\"";
    return; 
  }
  *prompt = arr[0];
}

void GetCurrDirCommand::execute()
{
  char* arr[COMMAND_MAX_ARGS];
  int numberOfArgs= _parseCommandLine( (this->m_cmd_line).c_str(),arr);
  if (numberOfArgs!=0)
  {
    std::cout <<  "smash error:>\""<< this->m_first_word<<"\"";
    return; 
  }
  char cwd[COMMAND_ARGS_MAX_LENGTH];
  std::cout << getcwd(cwd,COMMAND_ARGS_MAX_LENGTH);
}

void ChangeDirCommand::execute(char** oldPwd)
{
  char* arr[COMMAND_MAX_ARGS];
  int numberOfArgs= _parseCommandLine( (this->m_cmd_line).c_str(),arr);
  if (numberOfArgs!=1)
  {
    std::cout <<  "smash error:>\""<< this->m_first_word<<"\"";
    return; 
  }
  char cwd[COMMAND_ARGS_MAX_LENGTH];
  if (arr[0]=="-")
  {
    if (oldPwd==nullptr)
    {
    std::cout <<  "smash error:cd:OLDPWD not set";
    return;
    }
  chdir(*oldPwd);
  return;
  }
  *oldPwd=getcwd(cwd,COMMAND_ARGS_MAX_LENGTH);
  chdir(arr[0]);
}





ostream& operator<<(ostream& os, const JobsList::JobEntry& job)
{
    os << "[" << job.m_job_id << "] " << (job.m_cmd)->m_cmd_line << " " << (job.m_cmd)->m_process_id 
    << " " << difftime(time(NULL), job.m_starting_time);
    return os;
}