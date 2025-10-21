#include "IFileSystem.h"

class WindowsFileSystem : public IFileSystem {
public:
    std::vector<std::filesystem::directory_entry> list_files(const std::string& path) override;
    std::string go_back(const std::string& current_path) override;
    void open(const std::filesystem::directory_entry& entry) override;
};