#pragma once
#include "IFileSystem.h"

class WindowsFileSystem : public IFileSystem
{
public:
    WindowsFileSystem(std::string& startPath);

    std::string& current_path() const override;
    std::string go_back(const std::string& current_path) override;
    std::string open(const std::string& selected_path, int index) override;

    std::vector<std::filesystem::directory_entry> list_files(const std::string& path) const override;

private:
    std::string& _currentPath;
};
