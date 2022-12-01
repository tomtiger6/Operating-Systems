#include "Commands.h"
#include "Jobs.h"


JobsList::JobEntry::JobEntry(const std::string cmd_line, pid_t process_id, int job_id, bool is_stopped): 
   m_job_id(job_id), m_cmd_line(cmd_line), m_process_id(process_id), m_starting_time(time(NULL)), m_is_stopped(is_stopped){}

void JobsList::addJob(const std::string cmd_line, pid_t process_id, bool isStopped){
    if (this -> m_jobs.size() == 0){
      m_jobs.push_back(JobEntry(cmd_line, process_id, 1, isStopped));
    } else  {
      int max_job_id = (this -> m_jobs.back()).m_job_id ;
      m_jobs.push_back(JobEntry(cmd_line, process_id, max_job_id + 1, isStopped));
    }
  }

void JobsList::printJobsList(){
    this -> removeFinishedJobs();
    std::vector<JobEntry>::iterator iter;
    for (iter= m_jobs.begin() ; iter <= m_jobs.end(); iter++){
      std::cout << *iter << std::endl;
    }
}


JobsList::JobEntry * JobsList::getJobById(int jobId){
    std::vector<JobEntry>::iterator iter;
    for (iter = m_jobs.begin() ; iter <= m_jobs.end(); iter++){
      if ((*iter).m_job_id == jobId){
        return &(*iter);
      }
    }
    return nullptr;
}



void JobsList::removeJobById(int jobId){
  std::vector<JobEntry>::iterator iter;
    for (iter = m_jobs.begin() ; iter <= m_jobs.end(); iter++){
      if ((*iter).m_job_id == jobId){
        this -> m_jobs.erase(iter);
      }
    }

}

JobsList::JobEntry * JobsList::getLastJob(int* lastJobId){
  if (this -> m_jobs.size() == 0){
    return nullptr;
  }
  if (lastJobId != NULL){
  *lastJobId = ((this -> m_jobs).back()).m_job_id;
  }
  return &((this -> m_jobs).back());
}


JobsList::JobEntry * JobsList::getLastStoppedJob(int *jobId){
  JobEntry* last = nullptr;
  std::vector<JobEntry>::iterator iter;
  for (iter = m_jobs.begin() ; iter <= m_jobs.end(); iter++){
    if ((*iter).m_is_stopped){
      last = &(*iter);
    }
  }
  if ((last != nullptr) && (jobId != NULL)){
    *jobId = last -> m_job_id;
  }
  return last;
}


void JobsList::removeFinishedJobs(){
  bool again = true;
  int result = 0;
  std::vector<JobEntry>::iterator iter;
  while (again){
    for (iter = m_jobs.begin() ; iter <= m_jobs.end(); iter++){
        waitpid((*iter).m_process_id, &result, WNOHANG);
        return WIFEXITED(result){
          this -> m_jobs.erase(iter);
          again = true;
          break;
        }
      }
    again = false;
  }
}




void JobsList::killAllJobs(){
  //USE SIGNALS
}





















std::ostream& operator<<(std::ostream& os, const JobsList::JobEntry& job)
{
    os << "[" << job.m_job_id << "] " << job.m_cmd_line << " : " << job.m_process_id 
    << " " << difftime(time(NULL), job.m_starting_time);
    if (job.m_is_stopped){
      os << " (stopped)";
    }
    return os;
}