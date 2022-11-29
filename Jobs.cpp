#include "command.h"




void JobsList::addJob(const std::string cmd_line, pid_t process_id, bool isStopped = false){
    if (this -> m_jobs.size() == 0){
      m_jobs.push_back(JobEntry(cmd,_line, process_id, 1, isStopped));
    } else  {
      int max_job_id = (this -> m_jobs.back()).m_job_id ;
      m_jobs.push_back(JobEntry(cmd,_line, process_id, max_job_id + 1, isStopped));
    }
  }

void JobsList::printJobsList(){
    std::vector<JobEntry>::iterator iter;
    for (iter= m_jobs.begin() ; iter <= m_jobs.end(); iter++){
      std::cout << *iter << std::endl;
    }
}


JobsList::JobEntry * JobsList::getJobById(int jobId){
    std::vector<JobEntry>::iterator iter;
    for (iter = m_jobs.begin() ; iter <= m_jobs.end(); iter++){
      if ((*iter).m_job_id == jobID){
        return &(*iter);
      }
    }
    return nullptr;
}