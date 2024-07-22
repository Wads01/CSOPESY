#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Console/ConsoleManager.h"
#include "Processor/Scheduler.h"


int main(){
    ConsoleManager::initialize();
    ConsoleManager* consoleManager = ConsoleManager::getInstance();

    MainConsole mainConsole;
    mainConsole.onEnabled();

    while (consoleManager->isRunning()){
        mainConsole.process();
    }

    ConsoleManager::destroy();
    
    return 0;
}
