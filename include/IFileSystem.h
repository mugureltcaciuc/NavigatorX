#pragma once
#include <filesystem>
#include <vector>
#include <iomanip> // for std::setw
#include <filesystem>
#include <iostream>
#include <string>

class IFileSystem
{
public:
    virtual ~IFileSystem() = default;

    virtual std::string &get_left_path() = 0;
    virtual std::string &get_right_path() = 0;
    virtual std::string go_back(const std::string &current_path) = 0;
    virtual std::string open(const std::string &selected_path, int index) = 0;
    virtual std::vector<std::filesystem::directory_entry> list_files(const std::string &path) const = 0;

    virtual bool rename_file(const std::filesystem::path& original_path, const std::string& new_name) = 0;
    virtual bool view_file_content(const std::filesystem::path& file_path) = 0;
    virtual bool edit_file_content(const std::filesystem::path& file_path) = 0;
    virtual bool copy_file(const std::filesystem::path& from, const std::filesystem::path& to) = 0;
    virtual bool move_file(const std::filesystem::path& from, const std::filesystem::path& to) = 0;
    virtual bool delete_file(const std::filesystem::path& target) = 0;
    virtual bool create_folder(const std::filesystem::path& parent, const std::string& name) = 0;
};
