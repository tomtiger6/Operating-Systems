#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include "utils.h"


using namespace std;





Command::Command(const char* cmd_line):
  m_cmd_line(_trim(string(cmd_line))), m_first_word(m_cmd_line.substr(0, m_cmd_line.find_first_of(" \n"))){}
  /**
  string cmd_s = new string(_trim(string(cmd_line)));
  string firstWord = new string(cmd_s.substr(0, cmd_s.find_first_of(" \n")));
  this -> m_cmd_line = cmd_s;
  this -> m_first_word = firstWord;
  **/

BuiltInCommand::BuiltInCommand(const char* cmd_line)
  :Command(cmd_line){}


RedirectionCommand::RedirectionCommand(const char* cmd_line)
  :Command(cmd_line){}


ChangeDirCommand::ChangeDirCommand(const char* cmd_line, std::string* oldPwd)
  :BuiltInCommand(cmd_line), m_oldPwd(oldPwd){}

GetCurrDirCommand::GetCurrDirCommand(const char* cmd_line)
  :BuiltInCommand(cmd_line){}

ShowPidCommand::ShowPidCommand(const char* cmd_line)
  :BuiltInCommand(cmd_line){}

ChpromptCommand::ChpromptCommand(const char* cmd_line, std::string* prompt)
  :BuiltInCommand(cmd_line), m_prompt(prompt){}

  
void ShowPidCommand::execute(){
  std::cout << "smash pid is "<< getpid()<< std::endl;
}

void ChpromptCommand::execute(){
  char* arr[COMMAND_MAX_ARGS]; 
  int numberOfArgs= _parseCommandLine( (this->m_cmd_line).c_str(),arr);
  if (numberOfArgs==0)
  {
    *(this -> m_prompt)="smash";
    return; 
  }
  else
  {
    *(this -> m_prompt)=arr[1];
  }
  for (size_t i = 0; i < numberOfArgs; i++)
  {
    free(arr[i]);
  }
  
}

void GetCurrDirCommand::execute()
{
  char cwd[COMMAND_ARGS_MAX_LENGTH]; 
  std::cout << getcwd(cwd,COMMAND_ARGS_MAX_LENGTH);
}

void ChangeDirCommand::execute()
{
  char* arr[COMMAND_MAX_ARGS];
  int numberOfArgs= _parseCommandLine( (this->m_cmd_line).c_str(),arr);
  if (numberOfArgs!=1)
  {
    std::cout <<  "smash error: cd: to many arguments";
    return; 
  }
  char cwd[COMMAND_ARGS_MAX_LENGTH];
  if (arr[0]=="-")
  {
    if (m_oldPwd==nullptr)
    {
    std::cout <<  "smash error:cd:OLDPWD not set";
    return;
    }
  chdir((*m_oldPwd).c_str());
  return;
  }
  chdir(arr[1]);
   for (size_t i = 0; i < numberOfArgs; i++)
  {
    free(arr[i]);
  }
  *m_oldPwd=getcwd(cwd,COMMAND_ARGS_MAX_LENGTH);
}







void ExternalCommand::execute(){
  //need normal case
  char* args[4];
	args[0] = (char*)"/bin/bash";
	args[1] = (char*)"-c";
  string temp=this->m_cmd_line;
	args[2] = (char*)(temp.c_str());
	args[3] = NULL;
	_removeBackgroundSign(args[2]);
	execvp(args[0], args);
}