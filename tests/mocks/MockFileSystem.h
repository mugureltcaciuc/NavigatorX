#pragma once
#include "IFileSystem.h"
#include <gmock/gmock.h>

class MockFileSystem : public IFileSystem {
public:
    MOCK_METHOD(bool, copy_file, (const std::filesystem::path&, const std::filesystem::path&), (override));
    MOCK_METHOD(bool, move_file, (const std::filesystem::path&, const std::filesystem::path&), (override));
    MOCK_METHOD(bool, delete_file, (const std::filesystem::path&), (override));
    MOCK_METHOD(bool, create_folder, (const std::filesystem::path&, const std::string&), (override));
};
