#pragma once
#include "IFileSystem.h"

class WindowsFileSystem : public IFileSystem
{
public:
    WindowsFileSystem(std::string &left_path, std::string &right_path);
    void initialize();

    std::string &get_left_path() override;
    std::string &get_right_path() override;
    std::string go_back(const std::string &current_path) override;
    std::string open(const std::string &selected_path, int index) override;
    std::vector<std::filesystem::directory_entry> list_files(const std::string &path) const override;

    std::string _left_path;
    std::string _right_path;

private:
    std::vector<std::filesystem::directory_entry> _left_files;
    std::vector<std::filesystem::directory_entry> _right_files;

    bool _initialized;
};
