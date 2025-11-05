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

#pragma comment(lib, "User32.lib")

using namespace std;

const int panelWidth = 50;
const int separatorWidth = 2;
const std::string panelSeparator = " | ";

FileManager::FileManager(std::unique_ptr<IFileSystem> fileSystem, const std::string &path)
    : fileSystem(std::move(fileSystem)), left_path(path),
      right_path(std::filesystem::path(path).parent_path().string())
{
    refresh_files();
}
void FileManager::refresh_files()
{
    // system("cls"); // Use 'clear' on Linux/macOS
    left_files.clear();
    right_files.clear();

    for (const auto &entry : std::filesystem::directory_iterator(left_path))
    {
        left_files.push_back(entry);
    }

    for (const auto &entry : std::filesystem::directory_iterator(right_path))
    {
        right_files.push_back(entry);
    }
}

// Reads input immediately, returns on Enter or Escape
std::string FileManager::read_user_command()
{
    std::string buffer;

    while (!esc_pressed.load())
    {
        if (_kbhit())
        {
            int ch = _getch();

            // Handle extended keys (e.g., function keys)
            if (ch == 0 || ch == 224)
            {
                int ext = _getch();
                switch (ext)
                {
                case 60:
                    return "__F2__"; // Rename
                case 61:
                    return "__F3__"; // View
                case 62:
                    return "__F4__"; // Edit
                case 63:
                    return "__F5__"; // Copy
                case 64:
                    return "__F6__"; // Move
                case 65:
                    return "__F7__"; // New Folder
                case 66:
                    return "__F8__"; // Delete
                case 72:
                    return "__UP__"; // Arrow Up
                case 80:
                    return "__DOWN__"; // Arrow Down
                default:
                    break;
                }
            }

            switch (ch)
            {
            case 27: // ESC
                std::cout << "\nEscape pressed. Exiting...\n";
                return "__ESC__";

            case 13: // Enter
                std::cout << "\n";
                return buffer;

            case 8: // Backspace
                if (!buffer.empty())
                {
                    buffer.pop_back();
                    std::cout << "\b \b";
                }
                break;

            case 9: // TAB
                tab_pressed.store(true);
                return "__TAB__";

            default:
                buffer.push_back(static_cast<char>(ch));
                break;
            }
        }
    }

    return buffer;
}

void FileManager::run()
{
    clear_screen();
    refresh_files();
    drawPanel(left_path, right_path);

    std::string input = read_user_command();

    if (process_navigation_input(input))
    {
        return;
    }

    process_file_selection_input(input);
    refresh_files();
}

void FileManager::drawMenuBar()
{
    std::vector<std::string> menuItems = {
        "F2 Rename", "F3 View", "F4 Edit", "F5 Copy", "F6 Move", "F7 NewFolder", "F8 Delete", "TAB SwitchPanel", "BS FolderUp", "ESC Quit"};

    std::cout << "+" << std::string(128, '-') << "+" << std::endl;
    for (const auto &item : menuItems)
    {
        std::cout << "|  " << std::setw(8) << std::left << item;
    }
    std::cout << std::endl;
    std::cout << "+" << std::string(128, '-') << "+" << std::endl;
}

std::string FileManager::formatEntry(size_t index, const std::string &name, const std::string &type,
                                     const std::string &sizeStr, const std::string &timeStr, bool isSelected)
{
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

void FileManager::drawLeftRightPanels(const std::filesystem::path &leftPath,
                                      const std::filesystem::path &rightPath,
                                      std::vector<std::string> &buffer)
{
    const int indexWidth = 6; // e.g. "[* 0]"
    const int nameWidth = 25;
    const int typeWidth = 6;  // "[DIR]" or empty
    const int sizeWidth = 10; // e.g. " 20.7 MB"
    const int timeWidth = 16; // e.g. "2025-10-30 19:24"
    const std::string panelSeparator = " | ";

    const int panelWidth = indexWidth + nameWidth + typeWidth + sizeWidth + timeWidth;

    std::string leftHeader = isLeftPanelActive ? "+------------------ >>> LEFT PANEL <<< --------------------+" : "+------------------ Left Panel --------------------+  ";
    std::string rightHeader = !isLeftPanelActive ? "+------------------ >>> RIGHT PANEL <<< --------------------+" : "+------------------ Right Panel --------------------+ ";

    std::ostringstream headerLine;
    headerLine << std::left << std::setw(panelWidth) << leftHeader
               << panelSeparator
               << std::left << std::setw(panelWidth) << rightHeader;
    buffer.push_back(headerLine.str());

    auto formatEntry = [&](const std::filesystem::directory_entry &entry,
                           size_t index,
                           bool isPanelActive,
                           int selectedIndex) -> std::string
    {
        std::string name = entry.path().filename().string();
        std::string type = entry.is_directory() ? "[DIR]" : "";
        std::string sizeStr, timeStr;

        if (entry.is_regular_file())
        {
            double sizeMB = static_cast<double>(entry.file_size()) / (1024 * 1024);
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << sizeMB << " MB";
            sizeStr = oss.str();

            auto ftime = entry.last_write_time();
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
            std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
            std::tm *tm = std::localtime(&cftime);
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

    if (std::filesystem::exists(leftPath) && std::filesystem::is_directory(leftPath))
    {
        size_t index = 0;
        for (const auto &entry : std::filesystem::directory_iterator(leftPath))
        {
            leftEntries.push_back(formatEntry(entry, index++, isLeftPanelActive, selectedLeftIndex));
            if (leftEntries.size() >= 20)
                break;
        }
    }
    else
    {
        leftEntries.push_back("[Invalid path]");
    }

    if (std::filesystem::exists(rightPath) && std::filesystem::is_directory(rightPath))
    {
        size_t index = 0;
        for (const auto &entry : std::filesystem::directory_iterator(rightPath))
        {
            rightEntries.push_back(formatEntry(entry, index++, !isLeftPanelActive, selectedRightIndex));
            if (rightEntries.size() >= 20)
                break;
        }
    }
    else
    {
        rightEntries.push_back("[Invalid path]");
    }

    size_t maxLines = max(leftEntries.size(), rightEntries.size());
    for (size_t i = 0; i < maxLines; ++i)
    {
        std::string left = i < leftEntries.size() ? leftEntries[i] : std::string(panelWidth, ' ');
        std::string right = i < rightEntries.size() ? rightEntries[i] : std::string(panelWidth, ' ');
        std::ostringstream line;
        line << left << panelSeparator << right;
        buffer.push_back(line.str());
    }
}

// Helper function to draw one panel (used for both left and right)
void FileManager::drawPanel(const std::filesystem::path &leftPath, const std::filesystem::path &rightPath)
{
    std::cout << "Left path: " << left_path << "\\*.*" << "  |  Right path: " << right_path << "\\*.*" << std::endl;
    std::cout << std::string(130, '-') << std::endl;

    std::vector<std::string> buffer;
    drawLeftRightPanels(leftPath, rightPath, buffer);

    for (const auto &line : buffer)
    {
        std::cout << line << std::endl;
    }

    drawMenuBar();
}

void FileManager::clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

bool FileManager::process_navigation_input(const std::string &input)
{
    auto &activeFiles = isLeftPanelActive ? left_files : right_files;
    auto &activePath = isLeftPanelActive ? left_path : right_path;

    if (input.empty())
    {
        if (isLeftPanelActive)
        {
            left_path = fileSystem->go_back(left_files[0].path().string());
        }
        else
        {
            right_path = fileSystem->go_back(right_files[0].path().string());
        }

        refresh_files();
        return true;
    }

    if (input == "\t" || input == "__TAB__")
    {
        isLeftPanelActive = !isLeftPanelActive;
        if (isLeftPanelActive)
        {
            selectedLeftIndex = 0;
        }
        else
        {
            selectedRightIndex = 0;
        }
        return false;
    }

    if (input == "__F2__") // Rename
    {
        const auto &activeFiles = isLeftPanelActive ? left_files : right_files;
        if (!activeFiles.empty())
        {
            std::filesystem::path original = activeFiles[selectedIndex()].path();
            std::cout << "\nEnter new name for " << original.filename() << ": ";
            std::string new_name;
            std::getline(std::cin, new_name);
            if (!new_name.empty())
            {
                fileSystem->rename_file(original, new_name);
                refresh_files();
            }
        }
        return true;
    }

    if (input == "__F3__") // View
    {
        const auto &activeFiles = isLeftPanelActive ? left_files : right_files;
        if (!activeFiles.empty())
        {
            std::filesystem::path file = activeFiles[selectedIndex()].path();
            fileSystem->view_file_content(file);
        }
        return true;
    }

    if (input == "__F4__") // Edit
    {
        const auto &activeFiles = isLeftPanelActive ? left_files : right_files;
        if (!activeFiles.empty())
        {
            std::filesystem::path file = activeFiles[selectedIndex()].path();
            fileSystem->edit_file_content(file);
        }
        return true;
    }

    if (input == "__F5__") // Copy
    {
        if (activeFiles.empty())
            return true;
        std::filesystem::path source = activeFiles[selectedIndex()].path();
        std::filesystem::path targetDir = isLeftPanelActive ? right_path : left_path;
        std::filesystem::path target = targetDir / source.filename();
        fileSystem->copy_file(source, target);
        refresh_files();
        return true;
    }

    if (input == "__F6__") // Move
    {
        if (activeFiles.empty())
            return true;
        std::filesystem::path source = activeFiles[selectedIndex()].path();
        std::filesystem::path targetDir = isLeftPanelActive ? right_path : left_path;
        std::filesystem::path target = targetDir / source.filename();
        fileSystem->move_file(source, target);
        refresh_files();
        return true;
    }

    if (input == "__F7__") // New Folder
    {
        std::string folderName;
        std::cout << "\nEnter new folder name: ";
        std::getline(std::cin, folderName);
        if (!folderName.empty())
            fileSystem->create_folder(activePath, folderName);
        refresh_files();
        return true;
    }

    if (input == "__F8__") // Delete
    {
        if (activeFiles.empty())
            return true;
        std::filesystem::path target = activeFiles[selectedIndex()].path();
        std::cout << "\nDelete " << target.filename() << "? [y/N]: ";
        char confirm;
        std::cin >> confirm;
        if (confirm == 'y' || confirm == 'Y')
            fileSystem->delete_file(target);
        refresh_files();
        return true;
    }

    return false;
}

int FileManager::selectedIndex() const
{
    return isLeftPanelActive ? selectedLeftIndex : selectedRightIndex;
}

int FileManager::process_file_selection_input(const std::string &input)
{
    int index = 0;
    try
    {
        index = std::stoi(input);

        if (isLeftPanelActive)
        {
            if (index >= 0 && index < static_cast<int>(left_files.size()))
            {
                std::filesystem::path directoryPath = left_files[0].path().parent_path();
                left_path = fileSystem->open(directoryPath.string(), index);
            }
            else
            {
                // Do nothing;
            }
        }
        else
        {
            if (index >= 0 && index < static_cast<int>(right_files.size()))
            {
                std::filesystem::path directoryPath = right_files[0].path().parent_path();
                right_path = fileSystem->open(directoryPath.string(), index);
            }
            else
            {
                // Do nothing;
            }
        }
    }
    catch (...)
    {
        // done.store(true);
        // esc_pressed.store(true);
        // Do nothing;
    }
    return index;
}
