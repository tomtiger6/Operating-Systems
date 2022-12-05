#include "Commands.h"
#include "Jobs.h"


JobsList::JobEntry::JobEntry(const std::string cmd_line, pid_t process_id, int job_id, bool is_stopped): 
   m_job_id(job_id), m_cmd_line(cmd_line), m_process_id(process_id), m_starting_time(time(NULL)), m_is_stopped(is_stopped){}

void JobsList::addJob(const std::string cmd_line, pid_t process_id, bool isStopped){
    this -> removeFinishedJobs();
    if (this -> m_jobs.size() == 0){
      m_jobs.push_back(JobEntry(cmd_line, process_id, 1, isStopped));
    } else  {
      int max_job_id = (this -> m_jobs.back()).m_job_id ;
      m_jobs.push_back(JobEntry(cmd_line, process_id, max_job_id + 1, isStopped));
    }
  }

void JobsList::printJobsList(){
  this -> removeFinishedJobs();
  if (this -> m_jobs.size() != 0){
    std::vector<JobEntry>::iterator iter;
    for (iter= m_jobs.begin() ; iter < m_jobs.end(); iter++){
      std::cout << *iter << std::endl;
    }
  }
}


JobsList::JobEntry * JobsList::getJobById(int jobId){
  this -> removeFinishedJobs();
    std::vector<JobEntry>::iterator iter;
    for (iter = m_jobs.begin() ; iter < m_jobs.end(); iter++){
      if ((*iter).m_job_id == jobId){
        return &(*iter);
      }
    }
    return nullptr;
}



void JobsList::removeJobById(int jobId){
  this -> removeFinishedJobs();
  std::vector<JobEntry>::iterator iter;
    for (iter = m_jobs.begin() ; iter < m_jobs.end(); iter++){
      if ((*iter).m_job_id == jobId){
        this -> m_jobs.erase(iter);
        break;
      }
    }

}

JobsList::JobEntry * JobsList::getLastJob(int* lastJobId){
  this -> removeFinishedJobs();
  if (this -> m_jobs.size() == 0){
    return nullptr;
  }
  if (lastJobId != NULL){
  *lastJobId = ((this -> m_jobs).back()).m_job_id;
  }
  return &((this -> m_jobs).back());
}


JobsList::JobEntry * JobsList::getLastStoppedJob(int *jobId){
  this -> removeFinishedJobs();
  JobEntry* last = nullptr;
  std::vector<JobEntry>::iterator iter;
  for (iter = m_jobs.begin() ; iter < m_jobs.end(); iter++){
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
  std::vector<JobEntry>::iterator iter= m_jobs.begin();

  while (iter < m_jobs.end()){
    if (waitpid((*iter).m_process_id, NULL, WNOHANG)){
      iter = m_jobs.erase(iter);
    } else  {
      iter++;
    }
  }

  for (iter= m_jobs.begin() ; iter < m_jobs.end(); iter++){
    if (  ((*iter).m_is_stopped == false)  &&  (waitpid((*iter).m_process_id, NULL, WUNTRACED | WNOHANG))){// *iter got a sigstop
      (*iter).m_is_stopped = true;
    } else if (((*iter).m_is_stopped == true)  && waitpid((*iter).m_process_id, NULL, WCONTINUED | WNOHANG)){// *iter got a sigcont
      (*iter).m_is_stopped = false;
    }
  }
}


void JobsList::rePushJob(int job_id){
  std::vector<JobEntry>::iterator iter;
  for (iter= m_jobs.begin() ; iter < m_jobs.end(); iter++){
    if ((*iter).m_job_id == job_id){
      (*iter).m_is_stopped = true;
      (*iter).m_starting_time = time(NULL);
      break;
    }
  }
}

void JobsList::killAllJobsAndPrint(){
  this -> removeFinishedJobs();
  std::cout << "smash: sending SIGKILL signal to " << this -> m_jobs.size()  <<" jobs:" <<std::endl;
  std::vector<JobEntry>::iterator iter;
  for (iter = m_jobs.begin() ; iter < m_jobs.end(); iter++){
    if (kill((*iter).m_process_id, SIGKILL)){
      perror("smash error: kill failed");
    } else  {
      std::cout << (*iter).m_process_id << ": " << (*iter).m_cmd_line << std::endl;
    }
  }
}

JobsList::JobEntry* JobsList::getJobByProcess(pid_t process_id){
  this -> removeFinishedJobs();
  std::vector<JobEntry>::iterator iter;
  for (iter = m_jobs.begin() ; iter < m_jobs.end(); iter++){
    if (((*iter).m_process_id) == process_id){
      return &(*iter);
    } 
  }
  return nullptr;
}




std::ostream& operator<<(std::ostream& os, const JobsList::JobEntry& job)
{
    os << "[" << job.m_job_id << "] " << job.m_cmd_line << " : " << job.m_process_id 
    << " " << difftime(time(NULL), job.m_starting_time) << " secs";
    if (job.m_is_stopped){
      os << " (stopped)";
    }
    return os;
}