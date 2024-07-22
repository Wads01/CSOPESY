@echo off

rem Compile all .cpp files
g++ -std=c++20 -Wall -c main.cpp -o main.o
g++ -std=c++20 -Wall -c UI/UI_Manager.cpp -o UI_Manager.o
g++ -std=c++20 -Wall -c Processor/CommandProcessor.cpp -o CommandProcessor.o
g++ -std=c++20 -Wall -c Processor/Process.cpp -o Process.o
g++ -std=c++20 -Wall -c Processor/Scheduler.cpp -o Scheduler.o
g++ -std=c++20 -Wall -c Console/ConsoleManager.cpp -o ConsoleManager.o
g++ -std=c++20 -Wall -c Console/BaseScreen.cpp -o BaseScreen.o
g++ -std=c++20 -Wall -c Console/AConsole.cpp -o AConsole.o
g++ -std=c++20 -Wall -c Console/Consoles/MainConsole.cpp -o MainConsole.o
g++ -std=c++20 -Wall -c Console/Consoles/MarqueeConsole.cpp -o MarqueeConsole.o
g++ -std=c++20 -Wall -c Console/Consoles/ProcessConsole.cpp -o ProcessConsole.o
g++ -std=c++20 -Wall -c Command/ICommand.cpp -o ICommand.o
g++ -std=c++20 -Wall -c Command/PrintCommand.cpp -o PrintCommand.o
g++ -std=c++20 -Wall -c Resource/ResourceEmulator.cpp -o ResourceEmulator.o
g++ -std=c++20 -Wall -c Memory/Memory.cpp -o Memory.o
g++ -std=c++20 -Wall -c Memory/IMemoryAllocator.cpp -o IMemoryAllocator.o
g++ -std=c++20 -Wall -c Memory/FlatMemoryAllocator.cpp -o FlatMemoryAllocator.o
g++ -std=c++20 -Wall -c Memory/PagingMemoryAllocator.cpp -o PagingMemoryAllocator.o



rem Link object files into executable
g++ main.o UI_Manager.o CommandProcessor.o Process.o Scheduler.o ConsoleManager.o BaseScreen.o AConsole.o MainConsole.o MarqueeConsole.o ProcessConsole.o ICommand.o PrintCommand.o ResourceEmulator.o Memory.o IMemoryAllocator.o FlatMemoryAllocator.o PagingMemoryAllocator.o -o OS_EMULATOR.exe

rem Delete all .o files
del *.o

rem Delete all .txt files in the z_memLogs_z folder
del /q z_memLogs_z\*.txt

rem run the executable
OS_EMULATOR.exe