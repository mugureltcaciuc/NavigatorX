#pragma once
#include <filesystem>
#include <vector>
#include <iomanip>   // for std::setw
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    virtual std::vector<std::filesystem::directory_entry>
    list_files(const std::string& path) = 0;

    virtual std::string go_back(const std::string& current_path) = 0;
    virtual void open(const std::filesystem::directory_entry& entry) = 0;
};
