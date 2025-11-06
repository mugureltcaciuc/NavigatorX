# Build Instructions - SystemFileManagerX

## Requirements
- C++17 compiler (GCC, Clang, or MSVC)
- CMake 3.10 or later
- Git
---

## Windows (MSVC)

### Prerequisites:

- Visual Studio with Desktop Development with C++ workload
- C++17 support
- Git

### Installation:

- Open x64 Native Tools Command Prompt for VS (from Start menu).
- Clone the repository: git clone https://github.com/mugureltcaciuc/SystemFileManagerX.git cd SystemFileManagerX
- Build: Compile the main source files using MSVC:
    cl /EHsc /std:c++17 /FeSystemFileManagerX.exe src\main.cpp src\FileManager.cpp src\WindowsFileSystem.cpp
- Run: SystemFileManagerX.exe

##  Linux (g++)

### Prerequisites:

- g++ (GNU C++ compiler)
- Git

### Installation:

- Install g++ if not already installed: sudo apt update sudo apt install g++
- Clone the repository: git clone https://github.com/mugureltcaciuc/SystemFileManagerX.git cd SystemFileManagerX
- Build: Compile the source files:
        g++ -std=c++17 -o SystemFileManagerX src/main.cpp src/FileManager.cpp src/LinuxFileSystem.cpp
- Run: ./SystemFileManagerX

## Notes:

Ensure your terminal supports UTF-8 for proper display.
On Windows, use the Developer Command Prompt to ensure MSVC environment variables are set.
On Linux, make sure you have permission to access the directories you navigate.
