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
#include "Globals.h"


#pragma comment(lib,"User32.lib")

using namespace std;
namespace fs = std::filesystem;


FileManager::FileManager(std::unique_ptr<IFileSystem> fs, const std::string& path)
    : fs(std::move(fs)), current_path(path) {
    refresh_files();
}
void FileManager::refresh_files() {
    system("cls"); // Use 'clear' on Linux/macOS
    files.clear();
     for(const auto& entry : fs::directory_iterator(current_path)) {
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
    system("cls"); // Use 'clear' on Linux/macOS

    std::cout << "Current path: " << current_path << std::endl;
    for(size_t i=0; i < files.size(); ++i) {
        std::cout << "[" << i << "]" << files[i].path().filename().string();
        if(files[i].is_directory()) {
            std::cout << " [DIR]";
        }
        std::cout << std::endl;
    }

    std::cout << "[" << files.size() << "] [UP DIR]" << std::endl;

    std::cout << std::endl << "Select file number: ";
    std::string input =  FileManager::get_input_immediate();

    try {
        int index = std::stoi(input);
        if(index >= 0 && index < static_cast<int>(files.size())) {
            open_selected(index);
        }
        else if(index == static_cast<int>(files.size())) {
            go_back();
        }
        else {
            std::cout << "Invalid index." << std::endl;
        }
    } catch (...) {
        done.store(true);
        esc_pressed.store(true);
        // go_back();
        std::cout << "Invalid input." << std::endl;
    }
}

void FileManager::open_selected(int index) {
    std::cout << "Opening files in path: " << current_path << "\n";
    const auto& selected = files[index];
    if(selected.is_directory()) {
        current_path = selected.path().string();
        refresh_files();
    } else {
        std::string command = "\"" + selected.path().string() + "\"";
        system(command.c_str());
    }
}

void FileManager::go_back() {
    const auto& selected = files[0];
    auto absolute_path = std::filesystem::absolute(selected.path());
    auto current_absolute_path = std::filesystem::absolute(absolute_path.parent_path());
    auto parent_absolute_path = std::filesystem::absolute(current_absolute_path.parent_path());
    current_path = parent_absolute_path.string();
    std::cout << "Full path: " << current_path << '\n';
    refresh_files();
}
