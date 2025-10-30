#include <algorithm>
#include <atomic>
#include <chrono>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>

#include "FileManager.h"
#include "WindowsFileSystem.h"
#include "Globals.h"


#pragma comment(lib,"User32.lib")

using namespace std;


const int panelWidth = 50;
const int separatorWidth = 2;
const std::string panelSeparator = " | ";


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
    drawPanel(left_path, right_path);

    // std::cout << "\nSelect file number: ";
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

    std::cout << "+" << std::string(128, '-') << "+" << std::endl;
    for (const auto& item : menuItems)
    {
        std::cout << "|  " << std::setw(8) << std::left << item;
    }
    std::cout << std::endl;
    std::cout << "+" << std::string(128, '-') << "+" << std::endl;
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

void FileManager::drawLeftRightPanels(const std::filesystem::path& leftPath,
                                      const std::filesystem::path& rightPath,
                                      std::vector<std::string>& buffer) 
{
    const int indexWidth = 6;     // e.g. "[* 0]"
    const int nameWidth = 25;
    const int typeWidth = 6;      // "[DIR]" or empty
    const int sizeWidth = 10;     // e.g. " 20.7 MB"
    const int timeWidth = 16;     // e.g. "2025-10-30 19:24"
    const std::string panelSeparator = " | ";

    const int panelWidth = indexWidth + nameWidth + typeWidth + sizeWidth + timeWidth;

    std::string leftHeader  = isLeftPanelActive ? "+------------------ >>> LEFT PANEL <<< --------------------+"  : "+------------------ Left Panel --------------------+  ";
    std::string rightHeader = !isLeftPanelActive ? "+------------------ >>> RIGHT PANEL <<< --------------------+" : "+------------------ Right Panel --------------------+ ";

    std::ostringstream headerLine;
    headerLine << std::left << std::setw(panelWidth) << leftHeader
               << panelSeparator
               << std::left << std::setw(panelWidth) << rightHeader;
    buffer.push_back(headerLine.str());

    auto formatEntry = [&](const std::filesystem::directory_entry& entry,
                           size_t index,
                           bool isPanelActive,
                           int selectedIndex) -> std::string {
        std::string name = entry.path().filename().string();
        std::string type = entry.is_directory() ? "[DIR]" : "";
        std::string sizeStr, timeStr;

        if (entry.is_regular_file()) {
            double sizeMB = static_cast<double>(entry.file_size()) / (1024 * 1024);
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

        std::ostringstream line;
        line << std::left << std::setw(indexWidth)
             << (isPanelActive && index == selectedIndex ? "[* " + std::to_string(index) + "]" : "[  " + std::to_string(index) + "]")
             << std::left << std::setw(nameWidth) << name
             << std::left << std::setw(typeWidth) << type
             << std::left << std::setw(sizeWidth) << sizeStr
             << std::left << std::setw(timeWidth) << timeStr;

        return line.str();
    };

    std::vector<std::string> leftEntries;
    std::vector<std::string> rightEntries;

    if (std::filesystem::exists(leftPath) && std::filesystem::is_directory(leftPath)) {
        size_t index = 0;
        for (const auto& entry : std::filesystem::directory_iterator(leftPath)) {
            leftEntries.push_back(formatEntry(entry, index++, isLeftPanelActive, selectedLeftIndex));
            if (leftEntries.size() >= 20) break;
        }
    } else {
        leftEntries.push_back("[Invalid path]");
    }

    if (std::filesystem::exists(rightPath) && std::filesystem::is_directory(rightPath)) {
        size_t index = 0;
        for (const auto& entry : std::filesystem::directory_iterator(rightPath)) {
            rightEntries.push_back(formatEntry(entry, index++, !isLeftPanelActive, selectedRightIndex));
            if (rightEntries.size() >= 20) break;
        }
    } else {
        rightEntries.push_back("[Invalid path]");
    }

    size_t maxLines = max(leftEntries.size(), rightEntries.size());
    for (size_t i = 0; i < maxLines; ++i) {
        std::string left = i < leftEntries.size() ? leftEntries[i] : std::string(panelWidth, ' ');
        std::string right = i < rightEntries.size() ? rightEntries[i] : std::string(panelWidth, ' ');
        std::ostringstream line;
        line << left << panelSeparator << right;
        buffer.push_back(line.str());
    }
}

// Helper function to draw one panel (used for both left and right)
void FileManager::drawPanel(const std::filesystem::path& leftPath, const std::filesystem::path& rightPath)
{
    std::cout << std::string(130, '-') << std::endl;
    std::vector<std::string> buffer;
    drawLeftRightPanels(leftPath, rightPath, buffer);

    for (const auto& line : buffer) {
        std::cout << line << std::endl;
    }

    drawMenuBar();
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
            // Do nothing;
        }
    } catch (...) {
        // done.store(true);
        // esc_pressed.store(true);
        // Do nothing;
    }
    return index;
}
