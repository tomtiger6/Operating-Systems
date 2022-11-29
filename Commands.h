#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command {
  

// TODO: Add your data members
  
 public:
  std::string m_cmd_line;
  std::string m_first_word;
  int m_process_id;
  Command(const char* cmd_line, int process_id);
  virtual ~Command() = default;
  virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {

  public:
  BuiltInCommand(const char* cmd_line, int process_id);
  virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
 public:
  
  ExternalCommand(const char* cmd_line, int process_id, JobsList* jobs);
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const char* cmd_line, int process_id);
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
  std::string* m_oldPwd;
  ChangeDirCommand(const char* cmd_line, int process_id, std::string* oldPwd);
  virtual ~ChangeDirCommand() {}
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line, int process_id);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line, int process_id);
  virtual ~ShowPidCommand() {}
  void execute() override;
};


class ChpromptCommand : public BuiltInCommand {
 public:
 
  std::string* m_prompt;
  ChpromptCommand(const char* cmd_line, int process_id, std::string* prompt);
  virtual ~ChpromptCommand() {}
  void execute() override;
};


class JobsList;
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members
public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};


class JobsList {
 public:

  class JobEntry {
  int m_job_id;
   Command* m_cmd;
   time_t m_starting_time;
   bool m_is_stopped;
   public:
   
   friend std::ostream& operator<<(std::ostream& os, const JobEntry& entry); 
   JobEntry(Command* cmd, int job_id, bool is_stopped): 
   m_job_id(job_id), m_cmd(cmd), m_starting_time(time(NULL)), m_is_stopped(is_stopped){}
   ~JobEntry() = default ;
   friend class JobsList;
  };


 std::vector<JobEntry> m_jobs;
 // TODO: Add your data members
 public:
  JobsList() :m_jobs(){}
  ~JobsList() = default;
  
  void addJob(Command* cmd, bool isStopped = false){
    if (this -> m_jobs.size() == 0){
      m_jobs.push_back(JobEntry(cmd, 1, isStopped));
    } else  {
      int max_job_id = (this -> m_jobs.back()).m_job_id ;
      m_jobs.push_back(JobEntry(cmd, max_job_id + 1, isStopped));
    }
  }

  void printJobsList(){
    std::vector<JobEntry>::iterator iter;
    for (iter= m_jobs.begin() ; iter <= m_jobs.end(); iter++){
      std::cout << *iter << std::endl;

    }
  }


  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
  JobsList* m_jobs;
 // TODO: Add your data members
 public:
  JobsCommand(const char* cmd_line, int process_id, JobsList* jobs): BuiltInCommand(cmd_line, process_id), m_jobs(jobs){}
  virtual ~JobsCommand() = default;
  void execute() override{
    m_jobs -> printJobsList();
  }
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class TimeoutCommand : public BuiltInCommand {
/* Optional */
// TODO: Add your data members
 public:
  explicit TimeoutCommand(const char* cmd_line);
  virtual ~TimeoutCommand() {}
  void execute() override;
};

class FareCommand : public BuiltInCommand {
  /* Optional */
  // TODO: Add your data members
 public:
  FareCommand(const char* cmd_line);
  virtual ~FareCommand() {}
  void execute() override;
};

class SetcoreCommand : public BuiltInCommand {
  /* Optional */
  // TODO: Add your data members
 public:
  SetcoreCommand(const char* cmd_line);
  virtual ~SetcoreCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
  /* Bonus */
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line, JobsList* jobs);
  virtual ~KillCommand() {}
  void execute() override;
};

class SmallShell {
 private:
 
  // TODO: Add your data members
  SmallShell();
 public:
  std::string m_oldPwd ;
  std::string m_command_prompt ;
  JobsList m_jobs;


  Command *CreateCommand(const char* cmd_line, bool* to_execute);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();
  void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_
