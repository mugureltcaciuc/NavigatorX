# NavigatorX

**NavigatorX** is a modern C++17 console-based File Manager for Windows and Linux.
It provides a fast, keyboard-driven interface for navigating directories, managing files, and exploring your system in a dual-pane layout.
---
##  Features

- **Written in C++17** using only the standard library
- **Dual-panel interface** (left and right panels)
- **File and directory listing** with clear `[DIR]` markers
-  **Navigation shortcuts**
   - **Backspace + Enter** --> go up one directory
   - **ESC** --> exit
   - **Numeric selection** --> open a file or enter a directory
-  **Automatic screen refresh** when changing directories
-  **Cross-platform** (Windows, Linux, macOS – uses `std::filesystem`)

---

##   Technologies

- **Language:** C++17
- **Standard library:** `<filesystem>`, `<thread>`, `<iomanip>`, `<iostream>`
- **Compiler support:** GCC, Clang, MSVC

---

## 🖥️ Usage

1. **Clone the repository**
   ```bash
   git clone https://github.com/mugureltcaciuc/NavigatorX.git
   cd NavigatorX

2. **Build the project**
    g++ -std=c++17 -O2 -o NavigatorX src/*.cpp


3. **Run the File Manager**
   ```bash
   ./NavigatorX

4. **Controls**
    Key / Command      |  Action
    Number + Enter     |  Open file / enter directory
    Backspace + Enter  |  Go up one directory
    ESC                |  Exit the application
    Enter              |  Confirm input

5. **Project Structure**
    NavigatorX/
    |-- src/
    |   |-- main.cpp
    |   |-- FileManager.cpp
    |   |-- FileManager.h
    |   |-- ...
    |-- README.md
    |-- LICENSE

6. **Planned Improvements**

     File operations (copy, move, delete)
     Color themes for directory panels
     File preview mode
     Configurable key bindings
     Command-line integration
     Persistent settings and bookmarks

7.  **License**

    This project is licensed under the MIT License.
    See the LICENSE file for details.