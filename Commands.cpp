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
#include "AlarmList.h"

using namespace std;

#define freeAndRet(arr,num) for(int i=0;i<num;i++){free(arr[i]);} return



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

BackgroundCommand::BackgroundCommand(const char* cmd_line, SmallShell* smash): BuiltInCommand(cmd_line), m_smash(smash){}

QuitCommand::QuitCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line), m_jobs(jobs){}

KillCommand::KillCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line), m_jobs(jobs){}

FareCommand::FareCommand(const char* cmd_line): BuiltInCommand(cmd_line){}

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
    std::cerr <<  "smash error: cd: too many arguments" << std::endl;
    freeAndRet(arr,numberOfArgs); 
  }
  char cwd[COMMAND_ARGS_MAX_LENGTH];
  if (!strcmp ( arr[1],"-"))
  {
    if (m_oldPwd->empty()){
      std::cerr <<  "smash error: cd: OLDPWD not set" << std::endl;
      freeAndRet(arr,numberOfArgs);
    }
    string temp = getcwd(cwd,COMMAND_ARGS_MAX_LENGTH);
    if (chdir(m_oldPwd->c_str())){
      perror("smash error: chdir failed");
    } else  {
      *m_oldPwd = temp;
    }
    freeAndRet(arr,numberOfArgs);
  }
  string temp = getcwd(cwd,COMMAND_ARGS_MAX_LENGTH);
  if (chdir(arr[1])){
    perror("smash error: chdir failed");
  } else  {
    *m_oldPwd = temp;
  }
  freeAndRet(arr,numberOfArgs);
}

void JobsCommand::execute() 
{
  (*(this -> m_jobs)).printJobsList();
}

void ForegroundCommand::execute(){
  char* arr[COMMAND_MAX_ARGS];
  int numberOfArgs= _parseCommandLine((this->m_cmd_line).c_str(),arr);
  if (numberOfArgs == 1 && ((this -> m_smash) -> m_jobs.getLastJob(NULL) == nullptr)){//no args and empty list
    std::cerr << "smash error: fg: jobs list is empty" << std::endl;
  } else if (numberOfArgs > 2){//too many args
    std::cerr << "smash error: fg: invalid arguments" << std::endl;
  } else  {//one argument or no arguments but with a last job
    JobsList::JobEntry* job = (this -> m_smash) -> m_jobs.getLastJob(NULL);
    if (numberOfArgs == 2){// there is an argument, needs to be checked
      std::string stringed_id = string(arr[1]);
      if (!is_number(stringed_id)){//args isn't a number
        std::cerr << "smash error: fg: invalid arguments" << std::endl;
        for (int i = 0; i < numberOfArgs; i++){free(arr[i]);} return;
      } 
      int id = stoi(stringed_id);
      job = (this -> m_smash)-> m_jobs.getJobById(id);
      if (job == nullptr){
        std::cerr << "smash error: fg: job-id "<< id <<" does not exist" << std::endl;
        for (int i = 0; i < numberOfArgs; i++){free(arr[i]);} return;
      }
    }
    std::cout << job -> m_cmd_line << " : " << job -> m_process_id<< std::endl;
    this -> m_smash -> m_is_foreground_in_list = true;
    this -> m_smash -> m_current_foreground_job_id = job -> m_job_id;
    this -> m_smash -> m_current_foreground_pid = job -> m_process_id;
    this -> m_smash -> m_current_foreground_cmd = job -> m_cmd_line;
    if (kill (job -> m_process_id, SIGCONT)){
      perror("smash error: kill failed");
    }
    waitpid(job -> m_process_id, NULL, WUNTRACED);
    this -> m_smash -> m_current_foreground_pid = 0;
  }
  for (int i = 0; i < numberOfArgs; i++){
    free(arr[i]);
  }
}

void BackgroundCommand::execute(){
  char* arr[COMMAND_MAX_ARGS];
  int numberOfArgs= _parseCommandLine((this->m_cmd_line).c_str(),arr);
  if (numberOfArgs == 1 && ((this -> m_smash) -> m_jobs.getLastStoppedJob(NULL) == nullptr)){//no args and no stopped jobs
    std::cerr << "smash error: bg: there is no stopped jobs to resume" << std::endl;
  } else if (numberOfArgs > 2){//too many args
    std::cerr << "smash error: bg: invalid arguments" << std::endl;
  } else  {//one argument or no arguments but with a last stopped job
    JobsList::JobEntry* job = (this -> m_smash) -> m_jobs.getLastStoppedJob(NULL);
    if (numberOfArgs == 2){// there is an argument, needs to be checked
      std::string stringed_id = string(arr[1]);
      if (!is_number(stringed_id)){//arg isn't a number
        std::cerr << "smash error: fg: invalid arguments" << std::endl;
        for (int i = 0; i < numberOfArgs; i++){free(arr[i]);} return;
      } 
      int id = stoi(stringed_id);
      job = (this -> m_smash)-> m_jobs.getJobById(id);
      if (job == nullptr){
        std::cerr << "smash error: bg: job-id "<< id <<" does not exist" << std::endl;
        for (int i = 0; i < numberOfArgs; i++){free(arr[i]);} return;
      } else if (job -> m_is_stopped == false){
        std::cerr << "smash error: bg: job-id " << job -> m_job_id  << " is already running in the background" << std::endl;
        for (int i = 0; i < numberOfArgs; i++){free(arr[i]);} return;
      }
    }
    std::cout << job -> m_cmd_line << " : " << job -> m_process_id<< std::endl;
    if (kill (job -> m_process_id, SIGCONT)){
      perror("smash error: kill failed");
    }
    job -> m_is_stopped = false;
  }
  for (int i = 0; i < numberOfArgs; i++){
    free(arr[i]);
  }
}

void QuitCommand::execute(){
  char* arr[COMMAND_MAX_ARGS];
  int numberOfArgs= _parseCommandLine((this->m_cmd_line).c_str(),arr);
  if ((numberOfArgs > 1) && (!strcmp(arr[1], "kill"))){
    this -> m_jobs -> killAllJobsAndPrint();
  }
  for (int i = 0; i < numberOfArgs; i++){
    free(arr[i]);
  }
  exit(0);
}

void KillCommand::execute(){
  char* arr[COMMAND_MAX_ARGS];
  int numberOfArgs= _parseCommandLine((this->m_cmd_line).c_str(),arr);
  if (numberOfArgs != 3){//wrong number of args
    std::cerr << "smash error: kill: invalid arguments" << std::endl;
  } else if (!((is_dashed_number(string(arr[1])))&& (is_number(string(arr[2]))))){//args are not numbas
    std::cerr << "smash error: kill: invalid arguments" << std::endl;
  } else  if( this -> m_jobs -> getJobById(stoi(string(arr[2]))) == nullptr ){//invalid 
    std::cerr << "smash error: kill: job-id " << string(arr[2])   << " does not exist" << std::endl;
  } else  {
    int sig = stoi(string(arr[1]).substr(1));
    pid_t receiver = (this -> m_jobs -> getJobById(stoi(string(arr[2])))) -> m_process_id;
    if (sig < 1 || sig > 31){
      std::cerr << "smash error: kill: invalid arguments" << std::endl;
    } else if (kill(receiver ,  sig)){
      perror("smash error: kill failed");
    } else  {
      std::cout << "signal number "    <<  sig<<" was sent to pid "<< receiver <<std::endl;
    }
  }
  for (int i = 0; i < numberOfArgs; i++){
    free(arr[i]);
  }
}






void ExternalCommand::execute(){
  string str_cmd_line = this -> m_cmd_line;
  char * char_cmd_line = (char*)(str_cmd_line.c_str());
  _removeBackgroundSign(char_cmd_line);
  if (str_cmd_line.find('*')!= string::npos || str_cmd_line.find('?')!= string::npos)
  {
    //complex command
    char* args[4];
    args[0] = (char*)"/bin/bash";
    args[1] = (char*)"-c";
  
    args[2] = char_cmd_line;
    args[3] = NULL;
    _removeBackgroundSign(args[2]);
    if(execvp(args[0], args)== -1) 
    {
      //need to change to correct error handling
      std::cerr << "smash error: " << char_cmd_line << ": invalid arguments" << std::endl;

    }
  }
  else
  {
    char* args[COMMAND_MAX_ARGS];
    int numberOfArgs= _parseCommandLine( char_cmd_line,args);
    for (int i = numberOfArgs; i < COMMAND_MAX_ARGS; i++)
    {
      args[i]=NULL;
    }
    if(execvp(args[0], args)==-1)
    {
      //need to change to correct error handling
      std::cerr << "smash error: " << char_cmd_line << ": invalid arguments" << std::endl;
    }
    for (int j = 0; j < numberOfArgs; j++)
    {
      free(args[j]);
    }
  }

  
}


void FareCommand::execute{



  
}