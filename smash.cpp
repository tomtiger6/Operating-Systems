#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"
#include <string>
#include "SmallShell.h"
#define MAX_COMMAND 81


int main(int argc, char* argv[]) {
    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }


    //TODO: setup sig alarm handler

    SmallShell& smash = SmallShell::getInstance();
    while(true) {
        std::cout << smash.m_command_prompt << "> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        if(std::cin.eof()){
            return 0;
        }
        smash.executeCommand(cmd_line.c_str());
    }
    return 0;
}

