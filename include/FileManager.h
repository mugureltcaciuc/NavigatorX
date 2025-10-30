#include <memory>

#include "IFileSystem.h"

class FileManager {
public:
    FileManager(std::unique_ptr<IFileSystem> fs, const std::string& path);
    void run();
    void drawPanel(const std::filesystem::path& leftPath, const std::filesystem::path& rightPath);
    bool isLeftPanelActive = true;
    int selectedLeftIndex = 0; // Tracks which file is selected
    int selectedRightIndex = 0; // Tracks which file is selected

private:
    std::string current_path;
    std::filesystem::path left_path;
    std::filesystem::path right_path;

    std::vector<std::filesystem::directory_entry> files;
    std::unique_ptr<IFileSystem> fileSystem;

    void drawMenuBar();
    void drawLeftRightPanels(const std::filesystem::path& leftPath, const std::filesystem::path& rightPath, std::vector<std::string>& buffer);

    // Helper functions
    void refresh_files();
    void clear_screen();
    void update_panel_paths();
    bool handle_special_input(const std::string& input);
    int handle_index_input(const std::string& input);
    std::string formatEntry(size_t index, const std::string& name, const std::string& type,
                                        const std::string& sizeStr, const std::string& timeStr, bool isSelected);

    static std::string get_input_immediate();
};
