#include <gtest/gtest.h>
#include "WindowsFileSystem.h"
#include <filesystem>
#include <fstream>
#include <memory>

class WindowsFileSystemTest : public ::testing::Test
{
protected:
    std::unique_ptr<WindowsFileSystem> fs;

    std::filesystem::path test_dir = "test_dir";
    std::filesystem::path test_file = test_dir / "file.txt";
    std::filesystem::path copy_file = test_dir / "copy.txt";

    void SetUp() override
    {
        std::filesystem::create_directory(test_dir);
        std::ofstream(test_file) << "Hello, world!";
        std::string dummy_path = test_dir.string();
        fs = std::make_unique<WindowsFileSystem>(dummy_path, dummy_path);
    }

    void TearDown() override
    {
        std::filesystem::remove_all(test_dir);
        fs.reset();
    }
};

TEST_F(WindowsFileSystemTest, CopyFileSuccess)
{
    EXPECT_TRUE(fs->copy_file(test_file, copy_file));
    EXPECT_TRUE(std::filesystem::exists(copy_file));
}

TEST_F(WindowsFileSystemTest, MoveFileSuccess)
{
    EXPECT_TRUE(fs->move_file(test_file, copy_file));
    EXPECT_TRUE(std::filesystem::exists(copy_file));
    EXPECT_FALSE(std::filesystem::exists(test_file));
}

TEST_F(WindowsFileSystemTest, DeleteFileSuccess)
{
    EXPECT_TRUE(fs->delete_file(test_file));
    EXPECT_FALSE(std::filesystem::exists(test_file));
}

TEST_F(WindowsFileSystemTest, CreateFolderSuccess)
{
    std::string folder_name = "new_folder";
    EXPECT_TRUE(fs->create_folder(test_dir, folder_name));
    EXPECT_TRUE(std::filesystem::exists(test_dir / folder_name));
}
