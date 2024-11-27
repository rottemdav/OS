// smash.cpp

/*=============================================================================
* Includes, usings
=============================================================================*/
#include <iostream>
#include <string>
#include "commands.h"
#include "signals.h"

/*=============================================================================
* Main Function
=============================================================================*/
int main(int argc, char* argv[]) {
    std::string line;
    std::string cmd;

    while (true) {
        std::cout << "smash > ";
        std::getline(std::cin, line);
        
        // Copy input to cmd (optional, you could work directly with line)
        cmd = line;

        // Execute command
        // Example placeholder: processCommand(cmd);
        // Replace with actual function call from "commands.h"

        // Clear buffers for next command (not necessary with std::string)
        line.clear();
        cmd.clear();
    }

    return 0;
}
