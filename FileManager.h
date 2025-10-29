#include <memory>

#include "IFileSystem.h"

class FileManager {
public:
    FileManager(std::unique_ptr<IFileSystem> fs, const std::string& path);
    void run();

private:
    std::string current_path;
    std::vector<std::filesystem::directory_entry> files;
    std::unique_ptr<IFileSystem> fileSystem;

    void drawPanel(const std::vector<std::filesystem::directory_entry>& files, int panelWidth, bool isLeft);
    void refresh_files();

    static std::string get_input_immediate();  // Can be separated later
};
