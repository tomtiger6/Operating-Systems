#ifndef JOBS_H_
#define JOBS_H_

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>




//Need to add mechanism to delete finished jobs
class JobsList {
 public:

  class JobEntry {

   int m_job_id;
   std::string m_cmd_line;
   pid_t m_process_id;
   time_t m_starting_time;
   bool m_is_stopped;

   public:
   
   friend std::ostream& operator<<(std::ostream& os, const JobEntry& entry); 
   JobEntry(const std::string cmd_line, pid_t process_id, int job_id, bool is_stopped);
   ~JobEntry() = default ;
 
   friend class JobsList;
  };


  std::vector<JobEntry> m_jobs;
  // TODO: Add your data members
  public:
  JobsList() :m_jobs(){}
  ~JobsList() = default;
  
  void addJob(const std::string cmd_line, pid_t process_id, bool isStopped = false);
  void printJobsList();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  void removeFinishedJobs();


  void killAllJobs();
  // TODO: Add extra methods or modify exisitng ones as needed
};


#endif /*JOBS_H_*/