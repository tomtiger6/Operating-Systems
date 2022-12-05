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
  std::string cmd_s = _trim(string(cmd_line));
  std::string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
  firstWord = firstWord.substr(0,firstWord.find_last_of('&'));
  std::string cmd_line_noamps = cmd_line;
  cmd_line_noamps = cmd_line_noamps.substr(0,cmd_line_noamps.find_last_of('&'));
  const char * cmd_line_noamp = cmd_line_noamps.c_str();
  
  if (firstWord.compare("chprompt") == 0) {
    return new ChpromptCommand(cmd_line_noamp, &(this -> m_command_prompt));
  }
  if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line_noamp);
  }
  if (firstWord.compare("pwd") == 0) {
    return new GetCurrDirCommand(cmd_line_noamp);
  }
  if (firstWord.compare("cd") == 0) {
    return new ChangeDirCommand(cmd_line_noamp, &(this -> m_oldPwd));
  }
  if (firstWord.compare("jobs") == 0) {
    return new JobsCommand(cmd_line_noamp, &(this -> m_jobs));
  }
  if (firstWord.compare("fg") == 0) {
    return new ForegroundCommand(cmd_line_noamp, this);
  }
  if (firstWord.compare("bg") == 0) {
    return new BackgroundCommand(cmd_line_noamp, this);
  }
  if (firstWord.compare("quit") == 0) {
    return new QuitCommand(cmd_line_noamp, &(this -> m_jobs));
  }
  if (firstWord.compare("kill") == 0) {
    return new KillCommand(cmd_line_noamp, &(this -> m_jobs));
  }
  return new ExternalCommand(cmd_line, &(this -> m_jobs));
} 




void SmallShell::executeCommand(const char *cmd_line) 
{//NEED TO DO SPECIAL
  int std_out_copy= 12;
  int fd=-1;
  if (*cmd_line == '\0'){
    return;
  }
  std::string orig = cmd_line;
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
    const char * redirect_cmd=cmd_s.substr(0,redirect_pos).c_str();
    //for debuging std::cout << "cmd is "<< redirect_cmd << endl;
    std_out_copy= dup(1);
    if (redirect_pos+1==redirect_pos_sec) 
    {//there is '>>' in the cmd line should append
      fd=open(dest.c_str(), O_RDWR | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); //should fiind put what access mode the file should have.
    }
    else if (redirect_pos==redirect_pos_sec)
    {//else there is only '>' in the cmd line should overwrite.
      FILE* my_file=fopen(dest.c_str(), "w");
      fd=fileno(my_file);
      //fd=open(dest.c_str(),  O_RDWR | O_CREAT , S_IRUSR | S_IWUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); //should fiind put what access mode the file should have.
    }
    dup2(fd,1);
    SmallShell& bobby = SmallShell::getInstance();
    bobby.executeCommand(redirect_cmd);
    close(fd);
    dup2(std_out_copy,1);
    close(std_out_copy);
    return;
  }
  size_t pipe_pos=cmd_s.find_first_of('|');
  if(pipe_pos!=string::npos)
  {//found '|' in cmd_line
    if (pipe_pos+1>cmd_s.length())
    {
    //last char is '|' which should be an error  

    }
    string cmd_end=cmd_s.substr(pipe_pos+1,cmd_s.length());
    string cmd_start=cmd_s.substr(0,pipe_pos);
    int pipe_arr[2];
    pipe(pipe_arr);
    pid_t pid =fork();
    if (pid==0)
    {//son
      setpgrp();
      if (cmd_end[0]=='&')
      {
        cmd_end=cmd_end.substr(1);
      }
      dup2(pipe_arr[0],STDIN_FILENO);
      close(pipe_arr[1]);
      close(pipe_arr[0]);
      std_out_copy= dup(STDIN_FILENO);
      SmallShell& bobby = SmallShell::getInstance();
      bobby.executeCommand((cmd_end + " " + read_stdin()).c_str());
      exit(0);
    }
    else 
    {//father
      int std_to_work_with=STDOUT_FILENO;
      if(cmd_end[0]=='&')
      {
        std_to_work_with= STDERR_FILENO;
      }
      close(pipe_arr[0]);
      std_out_copy= dup(std_to_work_with);
      dup2(pipe_arr[1],std_to_work_with);
      close(pipe_arr[1]);
      SmallShell& bobby = SmallShell::getInstance();
      bobby.executeCommand(cmd_start.c_str());
      dup2(std_out_copy,std_to_work_with);
      close(std_out_copy);
      waitpid(pid,NULL,0);
      return;
    }
  }  
  //-------end of special commands----------- 


  if (isBuiltIn(firstWord)){//BUILT-IN
    Command* cmd = this -> CreateCommand(cmd_line);
    cmd -> execute();
    delete cmd;
  } else  {//EXTERNAL // or timeout
    bool is_background = _isBackgroundComamnd(cmd_line);
    pid_t son_pid = fork();
    if (son_pid){//dad
      if (is_background){
        (this -> m_jobs).addJob(orig, son_pid);
      } else  {
        this -> m_is_foreground_in_list = false;
        this -> m_current_foreground_pid = son_pid;
        this -> m_current_foreground_cmd = orig;
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


  // if (fd!=-1)
  // {
  //   close(fd);
  //   dup2(std_out_copy,1);
  //   close(std_out_copy);
  // }
}




