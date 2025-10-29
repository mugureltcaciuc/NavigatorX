#pragma once
#include <memory>
#include "IFileSystem.h"  // Needed for unique_ptr<IFileSystem>

class FileSystemFactory {
public:
    static std::unique_ptr<IFileSystem> create(std::string& path);
};
