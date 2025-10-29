#include "FileSystemFactory.h"
#include "IFileSystem.h"
#include "Globals.h"

// Include platform-specific file system implementations
#ifdef _WIN32
    #include "WindowsFileSystem.h"
#else
    #include "LinuxFileSystem.h"
#endif

std::unique_ptr<IFileSystem> FileSystemFactory::create(std::string& path) {
#ifdef _WIN32
    return std::make_unique<WindowsFileSystem>(path);
#else
    return std::make_unique<LinuxFileSystem>(path);
#endif
}

