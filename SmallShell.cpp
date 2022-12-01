#include "SmallShell.h"
#include <vector>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include "utils.h"



// TODO: Add your implementation for classes in Commands.h 

SmallShell::SmallShell() : m_oldPwd(), m_command_prompt("smash"), m_jobs(), m_current_foreground_pid(0), m_current_foreground_cmd(){}
// TODO: add your implementation




/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
  
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
  return new ExternalCommand(cmd_line, &(this -> m_jobs));
} 




void SmallShell::executeCommand(const char *cmd_line) {//NEED TO DO SPECIAL
  if (*cmd_line == '\r' || *cmd_line == '\n'){
    std::cout << std::endl;
      return;
  }
  if (*cmd_line == '\0'){
    exit(0);
  }
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
  if (isBuiltIn(firstWord)){//BUILT-IN
    Command* cmd = this -> CreateCommand(cmd_line);
    cmd -> execute();
    delete cmd;
  } else  {//EXTERNAL
    bool is_background = _isBackgroundComamnd(cmd_line);
    pid_t son_pid = fork();
    if (son_pid){//dad
      if (is_background){
        (this -> m_jobs).addJob(cmd_line, son_pid);
      } else  {
        this -> m_current_foreground_pid = son_pid;
        this -> m_current_foreground_cmd = cmd_line;
        waitpid(son_pid, NULL, WUNTRACED);
        this -> m_current_foreground_pid = 0;
        this -> m_current_foreground_cmd = "";
      }
    } else  {//forked process
      setpgrp();
      Command* cmd = this -> CreateCommand(cmd_line);
      cmd -> execute();
      delete cmd;
      exit(0);
    }
  }
}




