#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>
#include <Windows.h>

#include "UI_Manager.h"

using namespace std;

UI_Manager* UI_Manager::sharedInstance = nullptr;
int UI_Manager::pid_generate = -1;

UI_Manager& UI_Manager::getInstance(){
    if (!sharedInstance)
        initialize();

    return *sharedInstance;
}

void UI_Manager::initialize(){
    if (!sharedInstance)
        sharedInstance = new UI_Manager();
}

void UI_Manager::destroy(){
    if (sharedInstance){
        delete sharedInstance;
        sharedInstance = nullptr;
    }
}

void UI_Manager::clear(){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void UI_Manager::printMainHeader(){
    cout <<
R"(
  /$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$  /$$$$$$$$  /$$$$$$  /$$     /$$
 /$$__  $$ /$$__  $$ /$$__  $$| $$__  $$| $$_____/ /$$__  $$|  $$   /$$/
| $$  \__/| $$  \__/| $$  \ $$| $$  \ $$| $$      | $$  \__/ \  $$ /$$/ 
| $$      |  $$$$$$ | $$  | $$| $$$$$$$/| $$$$$   |  $$$$$$   \  $$$$/  
| $$       \____  $$| $$  | $$| $$____/ | $$__/    \____  $$   \  $$/   
| $$    $$ /$$  \ $$| $$  | $$| $$      | $$       /$$  \ $$    | $$    
|  $$$$$$/|  $$$$$$/|  $$$$$$/| $$      | $$$$$$$$|  $$$$$$/    | $$    
 \______/  \______/  \______/ |__/      |________/ \______/     |__/  
)";
}

void UI_Manager::printMainWelcome(){
    cout <<
R"(
Hello! Welcome to CSOPESY commandline!
Type 'help' to list down all possible commands.
Type 'exit' to quit, 'clear' to clear the screen.
)";
}

void UI_Manager::printMainCommandList(){
    cout <<
R"(
List of all possible commands.
'exit'                          ->      Terminates the program.
'clear'                         ->      Clears the screen except the header.
'marquee'                       ->      Displays a marquee animation with keyboard polling.
'screen'                        ->      Displays additional info about the main console.
'screen -s <name>'              ->      Creates a screen.
'screen -r <name>'              ->      Loads selected screen.
'screen -d <name>'              ->      Deletes selected screen.
'screen -ls'                    ->      List all screens.
'scheduler-test'                ->      Runs the scheduler; outputs sample processes.
'scheduler-stop'                ->      Stops the scheduler on generating running processes.
'report-util'                   ->      Logs the 'screen -ls' command in a text file.
'process-smi'                   ->      View of the memory allocation and utilization of processors
'view-config'                   ->      Views the configuration of the scheduler and memory.
'vmstat'                        ->      More detailed view on the paging allocator.
)";
}

void UI_Manager::printMessage(const std::string &message){
    cout << message << endl;
}

std::string UI_Manager::generateTimestamp(){
  auto now = std::chrono::system_clock::now();
  auto now_c = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&now_c), "%m/%d/%Y, %I:%M:%S %p");
  return ss.str();
}

void UI_Manager::setCursorPosition(int x, int y){
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void UI_Manager::getConsoleSize(int &width, int &height){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)){
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    else{
        std::cout << "Error in getting console size" << std::endl;
        return;
    }
}

void UI_Manager::setTextColor(int color, const std::string &text){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    std::cout << text;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int UI_Manager::generatePID(){
    return ++pid_generate; 
}