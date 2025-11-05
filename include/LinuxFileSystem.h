#pragma once

#include "IFileSystem.h"
#include <string>
#include <filesystem>

class LinuxFileSystem : public IFileSystem {
public:
    LinuxFileSystem(const std::string& left_path, const std::string& right_path);

    bool copy_file(const std::filesystem::path& source, const std::filesystem::path& destination) override;
    bool move_file(const std::filesystem::path& source, const std::filesystem::path& destination) override;
    bool delete_file(const std::filesystem::path& target) override;
    bool create_folder(const std::filesystem::path& base, const std::string& folder_name) override;

private:
    std::string left_path_;
    std::string right_path_;
};
