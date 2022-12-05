#include "AlarmList.h"
#include "Jobs.h"
#include "SmallShell.h"
void AlarmList::handleAlarm(){
    std::vector<timed_command>::iterator iter = m_list.begin();
    pid_t process_id = (*iter).m_process_id;
    m_list.erase(iter);
    alarm((*(m_list.begin())).m_expiration - time(NULL));
    JobsList::JobEntry* job = SmallShell::getInstance().m_jobs.getJobByProcess(process_id);
    if (job != nullptr){//process didn't finish
      if (kill(process_id, SIGKILL)){
        perror("smash error: kill failed");
    } else  {
      std::cout << "smash: "   << job -> m_cmd_line <<  " timed out!" << std::endl;
    }
    }
  }

  void AlarmList::addAlarm(pid_t process_id, time_t duration){
  timed_command cmd(process_id, time(NULL)+duration);
  bool added = false;
  std::vector<timed_command>::iterator iter = m_list.begin();
  for (iter = m_list.begin(); iter <m_list.end(); iter++){
    if ((*iter).m_expiration > cmd.m_expiration){
      m_list.insert(iter, cmd);
      added = true;
      break;
    }
  }
  if (!added){
    m_list.push_back(cmd);
  }
  alarm((*(m_list.begin())).m_expiration - time(NULL));
  }