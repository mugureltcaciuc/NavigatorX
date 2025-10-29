#include <memory>

#include "IFileSystem.h"

class FileManager {
public:
    FileManager(std::unique_ptr<IFileSystem> fs, const std::string& path);
    void run();
    void drawPanel(const std::filesystem::path& leftPath, const std::filesystem::path& rightPath);

    private:
    std::string current_path;
    std::vector<std::filesystem::directory_entry> files;
    std::unique_ptr<IFileSystem> fileSystem;

    void drawMenuBar();
    void drawLeftPanel(const std::filesystem::path& path, std::vector<std::string>& buffer);
    void drawRightPanel(const std::filesystem::path& path, std::vector<std::string>& buffer);
    void refresh_files();

    static std::string get_input_immediate();
};
