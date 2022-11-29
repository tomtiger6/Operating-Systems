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



// TODO: Add your implementation for classes in Commands.h 

SmallShell::SmallShell() :m_command_prompt("smash"), m_oldPwd(nullptr), m_jobs(){
// TODO: add your implementation
}

SmallShell::~SmallShell() {
// TODO: add your implementation
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line, bool* to_execute) {

  *to_execute = true;
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
  bool is_background = _isBackgroundCommand(cmd_line);

  if (firstWord.compare("chprompt") == 0) {
    return new ChpromptCommand(cmd_line, &(this -> m_command_prompt));
  }
  if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  if (firstWord.compare("pwd") == 0) {
    return new GetCurrDirCommand(cmd_line);
  }
  if (firstWord.compare("cd") == 0) {
    return new ChangeDirCommand(cmd_line, &(this -> m_oldPwd));
  }
  if (firstWord.compare("jobs") == 0) {
    return new JobsCommand(cmd_line, &(this -> m_jobs));
  }
  

  int flag = fork();
  *to_execute = !flag;
  if (*to_execute){
    return new ExternalCommand(cmd_line, &(this -> m_jobs));
  } else  {
    if (is_background){
      this -> m_jobs -> addJob(cmd_line, flag);
    } else  {
      waitpid(flag);
    }
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
  bool to_execute = true;
  Command* cmd = this -> CreateCommand(cmd_line, &to_execute);
  if (to_execute){
    cmd -> execute();
  }

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
	args[2] = (char*)cmd_line.c_str();
	args[3] = NULL;
	//setpgrp();
	args[2] = _removeBackgroundSign(args[2]).c_str();
	execvp(args[0], args);

}