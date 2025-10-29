#pragma once
#include <filesystem>
#include <vector>
#include <iomanip>   // for std::setw
#include <filesystem>
#include <iostream>
#include <string>

class IFileSystem {
public:
    virtual ~IFileSystem() = default;
 
    virtual std::string& current_path() const = 0;
    virtual std::string go_back(const std::string& current_path) = 0;
    virtual std::string open(const std::string& selected_path, int index) = 0;
    virtual std::vector<std::filesystem::directory_entry> list_files(const std::string& path) const = 0;
};
