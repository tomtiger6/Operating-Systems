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

SmallShell::SmallShell() : m_oldPwd(nullptr), m_command_prompt("smash"), m_jobs(), m_current_foreground_pid(0), m_current_foreground_cmd(NULL){
// TODO: add your implementation
}



/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line, bool* to_execute) {

  *to_execute = true;
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
  bool is_background = _isBackgroundComamnd(cmd_line);
  pid_t son_pid = fork();
  *to_execute = !son_pid;

  if (*to_execute){//new command
    setpgrp();
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
  
  else  {//dad shell
    if (is_background && (!(shouldNotIgnoreAmpersand(firstWord))))
    {
      (this -> m_jobs).addJob(cmd_line, son_pid);
    }
     else  
    {
      this -> m_current_foreground_pid = son_pid;
      this -> m_current_foreground_cmd = cmd_line;
      waitpid(son_pid, NULL, WUNTRACED);
      this -> m_current_foreground_pid = 0;
      this -> m_current_foreground_cmd = string(NULL);
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
    delete cmd;
  }

    // TODO: Add your implementation here
    // for example:
    // Command* cmd = CreateCommand(cmd_line);
    // cmd->execute();
    // Please note that you must fork smash process for some commands (e.g., external commands....)
}




