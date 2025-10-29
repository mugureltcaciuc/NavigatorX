#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>

#include <windows.h>
#include <conio.h>
#include <thread>
#include <chrono>
#include <atomic>

#include "FileManager.h"
#include "WindowsFileSystem.h"
#include "Globals.h"


#pragma comment(lib,"User32.lib")

using namespace std;


FileManager::FileManager(std::unique_ptr<IFileSystem> fileSystem, const std::string& path)
    : fileSystem(std::move(fileSystem)), current_path(path) {
    refresh_files();
}
void FileManager::refresh_files() {
    system("cls"); // Use 'clear' on Linux/macOS
    files.clear();
     for(const auto& entry : std::filesystem::directory_iterator(current_path)) {
         files.push_back(entry);
     }
}

// Reads input immediately, returns on Enter or Escape
std::string FileManager::get_input_immediate() {
    std::string buffer;

     while(!esc_pressed.load()) {
        if (_kbhit()) {
            int ch = _getch();

            if (ch == 27) {  // ESC
                std::cout << "\nEscape pressed. Exiting...\n";
                return "__ESC__";  // special marker, or you can throw/exit
            }
            else if (ch == 13) { // Enter
                std::cout << "\n";
                return buffer;    // return collected input
            }
            else if (ch == 8) {  // Backspace
                if (!buffer.empty()) {
                    buffer.pop_back();
                    std::cout << "\b \b"; // erase last char on console
                }
            }
            else {
                buffer.push_back(static_cast<char>(ch));
                std::cout << static_cast<char>(ch); // echo
            }
        }
    }
    std::cout << "do something";
    return buffer;
}

void FileManager::run() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    const int totalWidth = 100;
    const int panelWidth = totalWidth / 2 - 1;

    refresh_files();
    std::cout << "Current path (left and right): " << current_path << std::endl;
    std::cout << std::string(totalWidth, '=') << std::endl;

    // Draw both panels line by line
    size_t maxRows = std::max<size_t>(files.size(), 15);
    for (size_t i = 0; i < maxRows; ++i) {
        std::string left, right;

        // Left panel (your current directory)
        if (i < files.size()) {
            left = "[" + std::to_string(i) + "] " + files[i].path().filename().string();
            if (files[i].is_directory())
                left += " [DIR]";
        }

        // Right panel (for now, show same directory)
        if (i < files.size()) {
            right = "[" + std::to_string(i) + "] " + files[i].path().filename().string();
            if (files[i].is_directory())
                right += " [DIR]";
        }

        // Format and align columns
        if (left.size() > static_cast<size_t>(panelWidth - 2))
            left = left.substr(0, panelWidth - 5) + "...";
        if (right.size() > static_cast<size_t>(panelWidth - 2))
            right = right.substr(0, panelWidth - 5) + "...";

        std::cout << std::left << std::setw(panelWidth) << left << " |   "
                  << std::left << std::setw(panelWidth) << right << "\n";
    }

    std::cout << std::string(totalWidth, '=') << std::endl;
    std::cout << "[ GO UP ] [<- Backspace + Enter]" << std::endl;

    // Input handling (your existing logic)
    std::cout << std::endl << "Select file number: ";
    std::string input = FileManager::get_input_immediate();

    // Check for Backspace
    // Check for Backspace (go back if pressed with no text)
    if (input.empty()) {
        // Backspace pressed at empty input will be handled inside get_input_immediate
        // but we can check again here for safety
        const auto& selected = files[0];
        auto selected_path = selected.path();
        current_path = fileSystem->go_back(selected_path.string());
        refresh_files();

        return;
    }

    
    try {
        int index = std::stoi(input);
        if (index >= 0 && index < static_cast<int>(files.size())) {
            const auto& selected = files[0];
            auto selected_path = selected.path();

            std::filesystem::path fullPath(selected_path);

            // Get the parent path (i.e., remove .git)
            std::filesystem::path directoryPath = fullPath.parent_path();
            current_path = fileSystem->open(directoryPath.string(), index);
        } else {
            std::cout << "Invalid index." << std::endl;
        }
    } catch (...) {
        done.store(true);
        esc_pressed.store(true);
        std::cout << "Invalid input." << std::endl;
    }
}

// Helper function to draw one panel (used for both left and right)
void drawPanel(const std::vector<std::filesystem::directory_entry>& files, int panelWidth, bool isLeft) {
    const size_t maxRows = 15;
    for (size_t i = 0; i < maxRows; ++i) {
        if (i < files.size()) {
            std::string name = "[" + std::to_string(i) + "] " + files[i].path().filename().string();
            if (files[i].is_directory())
                name += " [DIR]";

            if (name.size() > static_cast<size_t>(panelWidth - 2))
                name = name.substr(0, panelWidth - 5) + "...";

            std::cout << std::left << std::setw(panelWidth) << name;
        } else {
            std::cout << std::setw(panelWidth) << " ";
        }

        if (isLeft)
            std::cout << "│"; // separator between panels

        std::cout << "\n";
    }
}