#include "SmallShell.h"
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include "utils.h"



// TODO: Add your implementation for classes in Commands.h 

SmallShell::SmallShell() : m_oldPwd(), m_command_prompt("smash"), m_jobs(), m_current_foreground_pid(0), m_current_foreground_cmd(),
m_is_foreground_in_list(false), m_current_foreground_job_id(0){}
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
  if (firstWord.compare("fg") == 0) {
    return new ForegroundCommand(cmd_line, this);
  }
  if (firstWord.compare("bg") == 0) {
    return new BackgroundCommand(cmd_line, this);
  }
  if (firstWord.compare("quit") == 0) {
    return new QuitCommand(cmd_line, &(this -> m_jobs));
  }
  if (firstWord.compare("kill") == 0) {
    return new KillCommand(cmd_line, &(this -> m_jobs));
  }
  return new ExternalCommand(cmd_line, &(this -> m_jobs));
} 




void SmallShell::executeCommand(const char *cmd_line) 
{//NEED TO DO SPECIAL
  int std_out_copy= 10;
  int fd=-1;
  if (*cmd_line == '\r' || *cmd_line == '\n'){
    std::cout << std::endl;
      return;
  }
  if (*cmd_line == '\0'){
    exit(0);
  }
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
  size_t redirect_pos=cmd_s.find_first_of('>');
  //-------start of special commands----------- 
  if (redirect_pos!=string::npos)
  {//found '>' in cmd_line
    size_t redirect_pos_sec=cmd_s.find_last_of('>');
    if (redirect_pos_sec+1>cmd_s.length())
    {
      //last char is '>' which should be an error  
    }
    string dest=_trim( cmd_s.substr(redirect_pos_sec+1));
    cmd_line=cmd_s.substr(0,redirect_pos).c_str();
    std_out_copy= dup(1);
    if (redirect_pos+1==redirect_pos_sec) 
    {//there is '>>' in the cmd line should append
      fd=open(dest.c_str(), O_RDWR |O_APPEND  |O_CREAT);
    }
    else if (redirect_pos==redirect_pos_sec)
    {//else there is only '>' in the cmd line should overwrite.
      fd=open(dest.c_str(), O_RDWR |O_CREAT);
    }
    dup2(fd,1);
  }
  size_t pipe_pos=cmd_s.find_first_of('|');
  if(pipe_pos!=string::npos)
  {//found '|' in cmd_line
    if (pipe_pos+1>cmd_s.length())
    {
    //last char is '|' which should be an error  
    //int arr[2];
    //pipe(arr);
    //fork;

    }
    string cmd_start=cmd_s.substr(pipe_pos+1,cmd_s.length());
    string cmd_end=cmd_s.substr(0,pipe_pos);
    int pipe_arr[2];
    pipe(pipe_arr);
    if (fork()==0)
    {//son
      close(pipe_arr[0]);
    }
    else 
    {//father
      close(pipe_arr[1]);
    }
  }  
  //-------end of special commands----------- 
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
        this -> m_is_foreground_in_list = false;
        this -> m_current_foreground_pid = son_pid;
        this -> m_current_foreground_cmd = cmd_line;
        waitpid(son_pid, NULL, WUNTRACED);//might need to pause son until we get here. SO if a ctr-c or ctr-z is sent then fg_pid is updated.
        this -> m_current_foreground_pid = 0;
      }
    } else  {//forked process
      setpgrp();
      Command* cmd = this -> CreateCommand(cmd_line);
      cmd -> execute();
      delete cmd;
      exit(0);
    }
  }
  if (fd!=-1)
  {
    close(fd);
    dup2(std_out_copy,1);
    close(std_out_copy);
  }
}




