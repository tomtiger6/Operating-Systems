#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include "SmallShell.h"
using namespace std;


#define SIG__KILL 9
#define SIG__STOP 19

void ctrlZHandler(int sig_num) {
	std::cout <<  "smash: got ctrl-Z" << std::endl;
  SmallShell& bobby = SmallShell::getInstance();
  pid_t process = bobby.m_current_foreground_pid;
  if (process){
    bobby.m_jobs.addJob(bobby.m_current_foreground_cmd, process, true);
    kill (process, SIG__STOP);
  }
  std::cout << "smash: process "  <<  process <<" was stopped" << std::endl;
}

void ctrlCHandler(int sig_num) {
  std::cout <<  "smash: got ctrl-C" << std::endl;
  SmallShell& bobby = SmallShell::getInstance();
  pid_t process = bobby.m_current_foreground_pid;
  if (process){
    kill (process, SIG__KILL);
  }
  std::cout << "smash: process " << process << " was killed"  <<std::endl ;
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

