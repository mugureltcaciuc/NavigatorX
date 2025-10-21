#include <filesystem>

#include "WindowsFileSystem.h"


std::string WindowsFileSystem::go_back(const std::string &current_path)
{
    //TODO
    return std::string();
}

void WindowsFileSystem::open(const std::filesystem::directory_entry &entry)
{
    //TODO
}

std::vector<std::filesystem::directory_entry> WindowsFileSystem::list_files(const std::string& path) {
    std::vector<std::filesystem::directory_entry> entries;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        entries.push_back(entry);
    }
    return entries;
}
