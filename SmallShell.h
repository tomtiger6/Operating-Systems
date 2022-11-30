#include "Jobs.h"
#include "Commands.h"



class SmallShell {
 private:
 
  // TODO: Add your data members
  SmallShell();
 public:
  std::string m_oldPwd ;
  std::string m_command_prompt ;
  JobsList m_jobs;
  pid_t m_current_foreground_pid;
  std::string m_current_foreground_cmd;

  SmallShell();
  SmallShell::~SmallShell()  = default;
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