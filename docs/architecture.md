# Architecture Overview - SystemFileManagerX

SystemFileManagerX is a console-based file manager for Windows and Linux, written in C++17.
Its architecture is modular, with clear separation between core logic, file system abstraction, and user interface rendering.

---

## Core Components

### 1. FileManager
Purpose: Central controller of the application.

Responsibilities:
- Runs the main program loop (run).
- Handles user input and calls appropriate file system operations.
- Displays file lists and manages navigation between directories.

Collaborates with:
- IFileSystem for file operations.
- Console UI for displaying panels and paths.

---

### 2. IFileSystem (Interface)
Purpose: Defines a platform-independent interface for file operations.

Key Methods:
- current_path() – returns current directory path.
- go_back() – navigates to the parent directory.
- open() – opens a file or enters a folder.
- list_files() – retrieves directory contents.

Implemented by:
- WindowsFileSystem (for Windows)
- FileSystem (for general or Unix-based systems)

---

### 3. WindowsFileSystem
Purpose: Windows-specific implementation of IFileSystem.

Responsibilities:
- Manages navigation using the Windows file system.
- Implements platform-specific logic such as clearing the console screen.

Implements: All virtual functions from IFileSystem.

---

### 4. main.cpp
Purpose: Application entry point.

Responsibilities:
- Detects the platform using preprocessor conditions.
- Creates the appropriate IFileSystem implementation.
- Initializes FileManager with it.
- Calls FileManager::run() to start the program.

---

## Data Flow Overview

1. Program start:
   main.cpp creates WindowsFileSystem and passes it to FileManager.

2. Main loop (FileManager::run):
   - Displays current directory.
   - Waits for user input (for example, number, Backspace, ESC).
   - Calls corresponding methods on IFileSystem.

3. File operations:
   - IFileSystem executes platform-specific logic (for example, open folder, go back).
   - Returns updated path and directory entries to FileManager.

4. UI update:
   - FileManager refreshes and reprints the directory listing.

---

## Planned Extensions

Component          | Planned Feature
------------------ | -------------------------------------
Panel              | Independent left and right navigation panels
InputHandler       | Keyboard input abstraction
FileSystem         | Linux and macOS support
UI                 | Colorized output and file highlighting

---

## Summary

SystemFileManagerX follows a clean and extensible architecture:

main.cpp
   |
   v
FileManager
   |
   v
IFileSystem ---> WindowsFileSystem / FileSystem

This separation ensures that:
- The user interface logic is independent of the file system implementation.
- Adding new features such as colors, dual panels, or cross-platform support is straightforward.
- Testing is simpler, since FileManager can use mock IFileSystem implementations.

---

Last updated: 2025-10-15