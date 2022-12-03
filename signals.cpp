#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include "SmallShell.h"
#include "Jobs.h"
using namespace std;


#define SIG__KILL 9
#define SIG__STOP 19

void ctrlZHandler(int sig_num) {
	std::cout <<  "smash: got ctrl-Z" << std::endl;
  SmallShell& bobby = SmallShell::getInstance();
  pid_t process = bobby.m_current_foreground_pid;
  if (process){
    if (bobby.m_is_foreground_in_list){
      bobby.m_jobs.rePushJob(bobby.m_current_foreground_job_id);
    } else  {
      bobby.m_jobs.addJob(bobby.m_current_foreground_cmd, process, true);
    }
    if (kill (process, SIG__STOP)){
      perror("smash error: kill failed");
    } else  {
      std::cout << "smash: process "  <<  process <<" was stopped" << std::endl;
    }
  }
}

void ctrlCHandler(int sig_num) {
  std::cout <<  "smash: got ctrl-C" << std::endl;
  SmallShell& bobby = SmallShell::getInstance();
  pid_t process = bobby.m_current_foreground_pid;
  if (process){
    if (kill (process, SIG__KILL)){
      perror("smash error: kill failed");
    } else  {
      std::cout << "smash: process " << process << " was killed"  <<std::endl ;
    }
  } 
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}
