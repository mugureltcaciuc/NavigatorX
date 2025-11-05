#include <memory>

#include "IFileSystem.h"

class FileManager
{
public:
    FileManager(std::unique_ptr<IFileSystem> fs, const std::string &path);
    void run();
    void drawPanel(const std::filesystem::path &leftPath, const std::filesystem::path &rightPath);
    bool isLeftPanelActive;
    int selectedLeftIndex = 0;  // Tracks which file is selected
    int selectedRightIndex = 0; // Tracks which file is selected

private:
    std::string left_path;
    std::string right_path;

    std::vector<std::filesystem::directory_entry> left_files;
    std::vector<std::filesystem::directory_entry> right_files;

    std::unique_ptr<IFileSystem> fileSystem;

    void drawMenuBar();
    void drawLeftRightPanels(const std::filesystem::path &leftPath, const std::filesystem::path &rightPath, std::vector<std::string> &buffer);

    // Helper functions
    void refresh_files();
    void clear_screen();
    int selectedIndex() const;
    bool process_navigation_input(const std::string &input);
    int process_file_selection_input(const std::string &input);
    std::string formatEntry(size_t index, const std::string &name, const std::string &type,
                            const std::string &sizeStr, const std::string &timeStr, bool isSelected);
    void print_line_with_dual_selection(const std::string &line, bool isLeftSelected, bool isRightSelected, size_t totalWidth);
    std::string read_user_command();
};
