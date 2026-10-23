//212-Ким-задача о пирамидах с МПК (Клиент)

#include "func.h"
#include <iostream>
#include <string>

//212-Ким-главная функция (клиентский интерфейс)
int main() {
    Control control;
    
    std::cout << "=== Client-Server App Initialized ===\n";
    std::cout << "Available commands:\n";
    std::cout << "  load     - load data from data.txt\n";
    std::cout << "  generate - generate 2D gaussian clouds\n";
    std::cout << "  cluster  - run graph wave clustering\n";
    std::cout << "  mst      - build Minimum Spanning Tree\n";
    std::cout << "  exit     - quit the program\n\n";
    std::cout << "(Visualization runs automatically after each command)\n\n";

    std::string cmd;
    while (true) {
        std::cout << "cmd> ";
        if (!(std::cin >> cmd)) break;
        
        if (cmd == "exit" || cmd == "quit") {
            control.executeCommand("exit");
            break;
        }
        
        control.executeCommand(cmd);
    }

    return 0;
}