#ifndef SMALL_SHELL_H_
#define SMALL_SHELL_H_


#include "Jobs.h"
#include "Commands.h"



class SmallShell {
 private:
 
  // TODO: Add your data members
 public:
  std::string m_oldPwd ;
  std::string m_command_prompt ;
  JobsList m_jobs;
  pid_t m_current_foreground_pid;
  std::string m_current_foreground_cmd;

  SmallShell();
  ~SmallShell()  = default;
  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
};
#endif //SMALL_SHELL_H
