SystemFileManagerX is a modular, console-based file manager written in C++17. It separates core logic, file system abstraction, and UI rendering for maintainability and extensibility.

---

## Core Components

### 1. FileManager
- Controls the main loop and user input
- Displays file lists and manages navigation
- Delegates file operations to `IFileSystem`

### 2. IFileSystem (Interface)
- Abstracts platform-specific file operations
- Key methods: `current_path()`, `go_back()`, `open()`, `list_files()`

### 3. WindowsFileSystem
- Implements `IFileSystem` for Windows
- Handles console clearing and file operations

### 4. main.cpp
- Entry point
- Detects platform and initializes appropriate `IFileSystem`
- Starts the application via `FileManager::run()`

---

## Data Flow

1. `main.cpp` initializes `FileManager` with a platform-specific `IFileSystem`
2. `FileManager::run()` handles input and updates UI
3. File operations are delegated to `IFileSystem`
4. UI is refreshed after each operation

---

## Planned Extensions

| Component     | Feature                          |
|---------------|----------------------------------|
| Panel         | Independent left/right navigation|
| InputHandler  | Keyboard abstraction             |
| FileSystem    | Linux/macOS support              |
| UI            | Color output, file highlighting  |

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
# Usage Guide - SystemFileManagerX

SystemFileManagerX is a keyboard-driven console file manager.

## Basic Navigation

1. Launch the application from the terminal.
2. View the current directory and its contents.
3. Use numeric keys + Enter to open a file or folder.
4. Press `Backspace` + Enter to go to the parent directory.
5. Use `Tab` to switch between left and right panels.
6. Press `ESC` to exit.

## Function Key Shortcuts

| Key        | Action                        |
|------------|-------------------------------|
| `F2`       | Rename selected file          |
| `F3`       | View file content             |
| `F4`       | Edit file (opens in Notepad)  |
| `F5`       | Copy file to other panel      |
| `F6`       | Move file to other panel      |
| `F7`       | Create new folder             |
| `F8`       | Delete file or folder         |

---

Last updated: 2025-11-06