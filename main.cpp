//212-Ким-задача конусы с волной, с МПК и k-means

#include "func.h"
#include <iostream>
#include <string>

int main() {
    Control control;
    
    std::cout << "=== Client-Server App Initialized ===\n";
    std::cout << "Commands:\n";
    std::cout << "  load         - load data from data.txt\n";
    std::cout << "  generate     - generate 2D gaussian clouds\n";
    std::cout << "  cluster <T>  - run wave clustering (T - optional threshold)\n";
    std::cout << "  kmeans <K>   - run k-means clustering (K - number of clusters)\n";
    std::cout << "  mst          - build Minimum Spanning Tree\n";
    std::cout << "  exit         - quit\n\n";

    std::string cmd;
    while (true) {
        std::cout << "cmd> ";
        if (!(std::cin >> cmd)) break;
        
        if (cmd == "exit" || cmd == "quit") {
            control.executeCommand("exit");
            break;
        }
        
        float val = 0.0f;
        int k = 0;

        if (cmd == "cluster") {
            if (std::cin.peek() != '\n' && std::cin.peek() != '\r') {
                std::cin >> val;
            }
        } else if (cmd == "kmeans") {
            std::cin >> k;
            if (k <= 0) {
                std::cerr << "Please provide a valid number of clusters (k > 0).\n";
                continue;
            }
        }
        
        control.executeCommand(cmd, val, k);
    }

    return 0;
}