#include <filesystem>

#include "WindowsFileSystem.h"


std::string WindowsFileSystem::go_back(const std::string &selected_path)
{
    // const auto& selected = files[0];
    // auto absolute_path = std::filesystem::absolute(selected.path());
    auto absolute_path = std::filesystem::absolute(selected_path);
    auto current_absolute_path = std::filesystem::absolute(absolute_path.parent_path());
    auto parent_absolute_path = std::filesystem::absolute(current_absolute_path.parent_path());
    auto current_path = parent_absolute_path.string();
    return current_path;
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
