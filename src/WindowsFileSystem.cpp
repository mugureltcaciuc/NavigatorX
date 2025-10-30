#include <filesystem>
#include <iostream>

#include "WindowsFileSystem.h"

using namespace std;

WindowsFileSystem::WindowsFileSystem(std::string &startPath)
    : _currentPath(startPath), _initialized(false) {}

void WindowsFileSystem::initialize()
{
    if (!_initialized)
    {
        _files = list_files(_currentPath);
        _initialized = true;
    }
}

std::string &WindowsFileSystem::current_path() const
{
    return _currentPath;
}

std::string WindowsFileSystem::open(const std::string &selected_path, int index)
{
    auto files = WindowsFileSystem::list_files(selected_path);
    const auto &selected = files[index];
    auto current_path = selected.path().string();
    if (!selected.is_directory())
    {
        std::string command = "\"" + selected.path().string() + "\"";
        system(command.c_str());
        current_path = std::filesystem::path(selected).parent_path().parent_path().string();
    }
    else
    {
        system("cls"); // Use 'clear' on Linux/macOS
        files.clear();

        for (const auto &entry : std::filesystem::directory_iterator(current_path,
                  std::filesystem::directory_options::skip_permission_denied))
        {
            files.emplace_back(entry);
        }
    }
    return current_path;
}

std::vector<std::filesystem::directory_entry> WindowsFileSystem::list_files(const std::string &path) const
{
    vector<std::filesystem::directory_entry> files;
    try
    {
        for (const auto &entry : std::filesystem::directory_iterator(path))
            files.emplace_back(entry);
    }
    catch (const std::exception &e)
    {
        cerr << "Error reading directory: " << e.what() << endl;
    }
    return files;
}

std::string WindowsFileSystem::go_back(const std::string &selected_path)
{
    auto absolute_path = std::filesystem::absolute(selected_path);
    auto current_absolute_path = std::filesystem::absolute(absolute_path.parent_path());
    auto parent_absolute_path = std::filesystem::absolute(current_absolute_path.parent_path());
    auto current_path = parent_absolute_path.string();
    return current_path;
}
