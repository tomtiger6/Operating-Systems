#ifndef ALARM_LIST_H_
#define ALARM_LIST_H_


#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>



class AlarmList{

  public:
  class timed_command{
    pid_t m_process_id;
    time_t m_expiration;
    public:
    timed_command(pid_t process_id, time_t expiration): m_process_id(process_id), m_expiration(expiration){}
    friend class AlarmList;
  };



  std::vector<timed_command> m_list;


  void handleAlarm();

  void addAlarm(pid_t process_id, time_t duration);
};




#endif /*ALARM_LIST_H_*/