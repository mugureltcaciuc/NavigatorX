#include "LinuxFileSystem.h"
#include <filesystem>
#include <iostream>

LinuxFileSystem::LinuxFileSystem(const std::string& left_path, const std::string& right_path)
    : left_path_(left_path), right_path_(right_path) {}

bool LinuxFileSystem::copy_file(const std::filesystem::path& source, const std::filesystem::path& destination) {
    try {
        std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Copy failed: " << e.what() << std::endl;
        return false;
    }
}

bool LinuxFileSystem::move_file(const std::filesystem::path& source, const std::filesystem::path& destination) {
    try {
        std::filesystem::rename(source, destination);
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Move failed: " << e.what() << std::endl;
        return false;
    }
}

bool LinuxFileSystem::delete_file(const std::filesystem::path& target) {
    try {
        return std::filesystem::remove(target);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Delete failed: " << e.what() << std::endl;
        return false;
    }
}

bool LinuxFileSystem::create_folder(const std::filesystem::path& base, const std::string& folder_name) {
    try {
        return std::filesystem::create_directory(base / folder_name);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Create folder failed: " << e.what() << std::endl;
        return false;
    }
}
