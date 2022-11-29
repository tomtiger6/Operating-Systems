#include "command.h"
#include "Job.h"


JobsList::JobEntry::JobEntry(const std::string cmd_line, pid_t process_id, int job_id, bool is_stopped): 
   m_job_id(job_id), m_cmd_line(cmd_line), m_process_id(process_id), m_starting_time(time(NULL)), m_is_stopped(is_stopped){}

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




































ostream& operator<<(ostream& os, const JobsList::JobEntry& job)
{
    os << "[" << job.m_job_id << "] " << job.m_cmd_line << " : " << job.m_process_id 
    << " " << difftime(time(NULL), job.m_starting_time);
    if (job.m_is_stopped){
      os << " (stopped)";
    }
    return os;
}