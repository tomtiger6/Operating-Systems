#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include "utils.h"
#include "Jobs.h"
#include "SmallShell.h"

using namespace std;





Command::Command(const char* cmd_line):
m_cmd_line(_trim(string(cmd_line))), m_first_word(m_cmd_line.substr(0, m_cmd_line.find_first_of(" \n"))){}

//RedirectionCommand::RedirectionCommand(const char* cmd_line) :Command(cmd_line){}

BuiltInCommand::BuiltInCommand(const char* cmd_line) :Command(cmd_line){}

ChangeDirCommand::ChangeDirCommand(const char* cmd_line, std::string* oldPwd) :BuiltInCommand(cmd_line), m_oldPwd(oldPwd){}

GetCurrDirCommand::GetCurrDirCommand(const char* cmd_line): BuiltInCommand(cmd_line){}

ShowPidCommand::ShowPidCommand(const char* cmd_line): BuiltInCommand(cmd_line){}

ChpromptCommand::ChpromptCommand(const char* cmd_line, std::string* prompt): BuiltInCommand(cmd_line), m_prompt(prompt){}

JobsCommand::JobsCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line), m_jobs(jobs){}

ForegroundCommand::ForegroundCommand(const char* cmd_line, SmallShell* smash): BuiltInCommand(cmd_line), m_smash(smash){}

void ShowPidCommand::execute(){
  std::cout << "smash pid is "<< getpid()<< std::endl;
}

void ChpromptCommand::execute(){
  char* arr[COMMAND_MAX_ARGS]; 
  int numberOfArgs= _parseCommandLine( (this->m_cmd_line).c_str(),arr);
  if (numberOfArgs==1){
    *(this -> m_prompt) = string("smash");
  } else  {
    *(this -> m_prompt) = string(arr[1]);
  }
  for (int i = 0; i < numberOfArgs; i++)
  {
    free(arr[i]);
  }
}

void GetCurrDirCommand::execute()
{
  char cwd[COMMAND_ARGS_MAX_LENGTH]; 
  std::cout << getcwd(cwd,COMMAND_ARGS_MAX_LENGTH) << std::endl;
}

void ChangeDirCommand::execute()
{
  char* arr[COMMAND_MAX_ARGS];
  int numberOfArgs= _parseCommandLine( (this->m_cmd_line).c_str(),arr);
  if (numberOfArgs > 2)
  {
    std::cout <<  "smash error: cd: to many arguments" << std::endl;
    return; 
  }
  char cwd[COMMAND_ARGS_MAX_LENGTH];
  if (!strcmp ( arr[0],"-"))
  {
    if (m_oldPwd->empty())
    {
    std::cout <<  "smash error:cd:OLDPWD not set" << std::endl;
    return;
    }
  *m_oldPwd = getcwd(cwd,COMMAND_ARGS_MAX_LENGTH);
  chdir(m_oldPwd->c_str());
  return;
  }
  *m_oldPwd = getcwd(cwd,COMMAND_ARGS_MAX_LENGTH);
  chdir(arr[1]);
   for (int i = 0; i < numberOfArgs; i++)
  {
    free(arr[i]);
  }
}//need to add error handlinf for chdir failure

void JobsCommand::execute() 
{
  (*(this -> m_jobs)).removeFinishedJobs();
  (*(this -> m_jobs)).printJobsList();
}

void ForegroundCommand::execute(){

  char* arr[COMMAND_MAX_ARGS];
  int numberOfArgs= _parseCommandLine((this->m_cmd_line).c_str(),arr);
  if (numberOfArgs == 1){//no args
    std::cout << "smash error: fg: jobs list is empty" << std::endl;
  } else if (numberOfArgs > 2){//too many args
    std::cout << "smash error: fg: invalid arguments" << std::endl;
  } else  {//one argument
    std::string stringed_id = string(arr[1]);
    int id = stoi(stringed_id);
    if (!is_number(stringed_id)){//args isn't a number
      std::cout << "smash error: fg: invalid arguments" << std::endl;
    } else  if ((this -> m_smash) -> m_jobs.getJobById(id) == nullptr){
      std::cout << "smash error: fg: job-id "<< id <<" does not exist" << std::endl;
    } else  {
      JobsList::JobEntry* job = (this -> m_smash)-> m_jobs.getJobById(id);
      std::cout << job -> m_cmd_line << " : " << job -> m_process_id<< std::endl;
      kill (job -> m_process_id, SIGCONT);
      this -> m_smash -> m_is_foreground_in_list = true;
      this -> m_smash -> m_current_foreground_job_id = job -> m_job_id;
      this -> m_smash -> m_current_foreground_pid = job -> m_process_id;
      this -> m_smash -> m_current_foreground_cmd = job -> m_cmd_line;
      waitpid(job -> m_process_id, NULL, WUNTRACED);
      this -> m_smash -> m_current_foreground_pid = 0;
    }
  }
  for (int i = 0; i < numberOfArgs; i++){
    free(arr[i]);
  }
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