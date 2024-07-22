#pragma once
#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <Windows.h>

#include "../UI/UI_Manager.h"
#include "AConsole.h"
#include "BaseScreen.h"
#include "Consoles/MainConsole.h"
#include "Consoles/MarqueeConsole.h"
#include "Consoles/ProcessConsole.h"

const std::string MAIN_CONSOLE = "MAIN_CONSOLE";
const std::string MARQUEE_CONSOLE = "MARQUEE_CONSOLE";
const std::string PROCESS_CONSOLE = "PROCESS_CONSOLE";

class ConsoleManager{
public:
    typedef std::unordered_map<std::string, std::shared_ptr<AConsole>> ConsoleTable;

    static ConsoleManager* getInstance();
    static void initialize();
    static void destroy();

    void drawConsole() const;
    void process() const;

    void switchConsole(const std::string &consoleName);
    bool registerScreen(std::shared_ptr<BaseScreen> screenReference);
    void unregisterScreen(const std::string &screenName);

    void listScreens() const;
    void listScreens(std::ofstream& logfile) const;
    void getPreviousConsole();
    void exit();
    bool isRunning() const;

    HANDLE getConsoleHandle() const;

private:
    ConsoleManager();
    ConsoleManager(const ConsoleManager&) = delete;
    ConsoleManager& operator=(const ConsoleManager&) = delete;
    static ConsoleManager* sharedInstance;

    ConsoleTable consoleTable;
    std::shared_ptr<AConsole> currConsole;
    std::shared_ptr<AConsole> prevConsole;

    HANDLE consoleHandle;
    bool running = true;
};

#endif