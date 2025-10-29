#include <algorithm>
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

    refresh_files();
    std::cout << "Current path (left and right): " << current_path << std::endl;

    std::filesystem::path left_path(current_path);
    this->drawPanel(left_path, left_path);

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

void FileManager::drawMenuBar() {
    std::vector<std::string> menuItems = {
        "Help", "Menu", "View", "Edit", "Copy", "Mkdir", "Delete", "Back", "Quit"
    };

    std::cout << "+--------------------------------------------------------------------------------------------------+" << std::endl;
    for (const auto& item : menuItems)
    {
        std::cout << "|  " << std::setw(8) << std::left << item;
    }
    std::cout << std::endl;
    std::cout << "+--------------------------------------------------------------------------------------------------+" << std::endl;
}

void FileManager::drawLeftPanel(const std::filesystem::path& path, std::vector<std::string>& buffer) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        buffer.push_back("[Invalid path]");
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string name = entry.path().filename().string();
         std::string type = entry.is_directory()
            ? "[" + std::to_string(buffer.size()) + " - DIR] "
            : "[" + std::to_string(buffer.size()) + "] ";
        buffer.push_back(type + name);
        if (buffer.size() >= 20) break;
    }
}

void FileManager::drawRightPanel(const std::filesystem::path& path, std::vector<std::string>& buffer) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        buffer.push_back("[Invalid path]");
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string name = entry.path().filename().string();
        std::string type = entry.is_directory()
            ? "[" + std::to_string(buffer.size()) + " - DIR] "
            : "[" + std::to_string(buffer.size()) + "] ";

        buffer.push_back(type + name);
        if (buffer.size() >= 20) break;
    }
}

// Helper function to draw one panel (used for both left and right)
void FileManager::drawPanel(const std::filesystem::path& leftPath, const std::filesystem::path& rightPath)
{
    drawMenuBar();

    std::vector<std::string> leftBuffer;
    std::vector<std::string> rightBuffer;

    drawLeftPanel(leftPath, leftBuffer);
    drawRightPanel(rightPath, rightBuffer);

    auto left_size = leftBuffer.size();
    auto right_size =  rightBuffer.size();
    size_t maxLines = max(left_size, right_size);

    std::cout << std::left << std::setw(40) << "Left Panel" << " | " << std::setw(40) << "Right Panel" << std::endl;
    std::cout << std::string(98, '-') << std::endl;

    for (size_t i = 0; i < maxLines; ++i) {
        std::string left = i < leftBuffer.size() ? leftBuffer[i] : "";
        std::string right = i < rightBuffer.size() ? rightBuffer[i] : "";
        std::cout << std::setw(40) << left << " | " << std::setw(40) << right << std::endl;
    }

    std::cout << std::string(98, '-') << std::endl;
}
