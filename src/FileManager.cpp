#include <algorithm>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>

#include <iomanip>        // For formatting output (e.g., std::setprecision)
#include <sstream>        // For building formatted strings
#include <ctime>          // For std::localtime and std::strftime

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
    : fileSystem(std::move(fileSystem)), current_path(path), left_path(std::filesystem::path(path)),
    right_path(std::filesystem::path(path).parent_path()) {
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
            else if (ch == 9) {  // TAB
                return "__TAB__";
            }
            else {
                buffer.push_back(static_cast<char>(ch));
                std::cout << static_cast<char>(ch); // echo
            }
        }
    }
    return buffer;
}
void FileManager::run() {
    clear_screen();
    refresh_files();

    std::cout << "Current path (left and right): " << current_path << std::endl;

    update_panel_paths();
    drawPanel(left_path, right_path, isLeftPanelActive);

    std::cout << "\nSelect file number: ";
    std::string input = get_input_immediate();

    if (handle_special_input(input)) {
        return;
    }

    handle_index_input(input);
    refresh_files();
}

void FileManager::drawMenuBar() 
{
    std::vector<std::string> menuItems = {
        "1 View", "2 Edit", "3 Copy", "4 Move", "5 NewFolder", "7 Delete", "TAB SwitchPanel", "BS FolderUp", "ESC Quit"
    };

    std::cout << "+--------------------------------------------------------------------------------------------------+" << std::endl;
    for (const auto& item : menuItems)
    {
        std::cout << "|  " << std::setw(8) << std::left << item;
    }
    std::cout << std::endl;
    std::cout << "+--------------------------------------------------------------------------------------------------+" << std::endl;
}

std::string FileManager::formatEntry(size_t index, const std::string& name, const std::string& type,
                                     const std::string& sizeStr, const std::string& timeStr, bool isSelected) {
    std::ostringstream line;
    std::string indexStr = isSelected ? "[* " + std::to_string(index) + "]" : "[" + std::to_string(index) + "]";
    line << std::left
         << std::setw(6) << indexStr
         << std::setw(25) << name
         << std::setw(7) << type
         << std::setw(10) << sizeStr
         << std::setw(17) << timeStr;
    return line.str();
}

void FileManager::drawLeftPanel(const std::filesystem::path& path, std::vector<std::string>& buffer, bool isLeftPanelActive) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        buffer.push_back("[Invalid path]");
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string name = entry.path().filename().string();
        std::string type = entry.is_directory() ? "[DIR]" : "";

        std::string sizeStr = "";
        std::string timeStr = "";

        if (entry.is_regular_file()) {
            auto fsize = entry.file_size();
            double sizeMB = static_cast<double>(fsize) / (1024 * 1024);
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << sizeMB << " MB";
            sizeStr = oss.str();

            auto ftime = entry.last_write_time();
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now()
            );
            std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
            std::tm* tm = std::localtime(&cftime);
            char timeBuf[20];
            std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M", tm);
            timeStr = timeBuf;
        }

        size_t index = buffer.size();
        bool isSelected = (index == selectedLeftIndex);
        buffer.push_back(formatEntry(index, name, type, sizeStr, timeStr, isSelected));
        if (buffer.size() >= 20) break;
    }
}

void FileManager::drawRightPanel(const std::filesystem::path& path, std::vector<std::string>& buffer, bool isRightPanelActive) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        buffer.push_back("[Invalid path]");
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string name = entry.path().filename().string();
        std::string type = entry.is_directory() ? "[DIR]" : "";

        std::string sizeStr = "";
        std::string timeStr = "";

        if (entry.is_regular_file()) {
            auto fsize = entry.file_size();
            double sizeMB = static_cast<double>(fsize) / (1024 * 1024);
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << sizeMB << " MB";
            sizeStr = oss.str();

            auto ftime = entry.last_write_time();
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now()
            );
            std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
            std::tm* tm = std::localtime(&cftime);
            char timeBuf[20];
            std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M", tm);
            timeStr = timeBuf;
        }

        size_t index = buffer.size();
        bool isSelected = (index == selectedRightIndex);
        buffer.push_back(formatEntry(index, name, type, sizeStr, timeStr, isSelected));
        if (buffer.size() >= 20) break;
    }
}

// Helper function to draw one panel (used for both left and right)
void FileManager::drawPanel(const std::filesystem::path& leftPath, const std::filesystem::path& rightPath,  bool isLeftActive)
{
    drawMenuBar();

    std::vector<std::string> leftBuffer;
    std::vector<std::string> rightBuffer;

    drawLeftPanel(leftPath, leftBuffer, isLeftActive);
    drawRightPanel(rightPath, rightBuffer, !isLeftActive);

    auto left_size = leftBuffer.size();
    auto right_size =  rightBuffer.size();
    size_t maxLines = max(left_size, right_size);

    std::string leftLabel  = isLeftActive ? ">>> LEFT PANEL <<<"  : "  Left Panel  ";
    std::string rightLabel = !isLeftActive ? ">>> RIGHT PANEL <<<" : "  Right Panel ";

    std::cout << std::left << std::setw(40) << leftLabel << " | " << std::setw(40) << rightLabel << std::endl;
    std::cout << std::string(98, '-')    << std::endl;

    for (size_t i = 0; i < maxLines; ++i) {
        std::string left = i < leftBuffer.size() ? leftBuffer[i] : "";
        std::string right = i < rightBuffer.size() ? rightBuffer[i] : "";
        std::cout << std::setw(40) << left << " | " << std::setw(40) << right << std::endl;
    }

    std::cout << std::string(98, '-') << std::endl;
}

void FileManager::clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void FileManager::update_panel_paths() {
    if (isLeftPanelActive) {
        left_path = std::filesystem::path(current_path);
    } else {
        right_path = std::filesystem::path(current_path);
    }
}

bool FileManager::handle_special_input(const std::string& input) {
    if (input.empty()) {
        current_path = fileSystem->go_back(files[0].path().string());
        refresh_files();
        return true;
    }

    if (input == "\t" || input == "__TAB__") {
        current_path = files[0].path().parent_path().string();
        return false;
    }

    return false;
}

int FileManager::handle_index_input(const std::string& input) {
    int index = 0;
    try {
        index = std::stoi(input);
        if (index >= 0 && index < static_cast<int>(files.size())) {
            std::filesystem::path directoryPath = files[0].path().parent_path();
            current_path = fileSystem->open(directoryPath.string(), index);
        } else {
            // std::cout << "Invalid index." << std::endl;
        }
    } catch (...) {
        // done.store(true);
        // esc_pressed.store(true);
        // std::cout << "Invalid input." << std::endl;
    }
    return index;
}
