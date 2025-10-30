// #define _HAS_CXX17 1
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>

#include <windows.h>
#include <conio.h>
#include <thread>
#include <chrono>
#include <atomic>

#include "FileSystemFactory.h"
#include "FileManager.h"
#include "WindowsFileSystem.h"
#include "Globals.h"

#pragma comment(lib,"User32.lib")

void monitor_escape_key() {
    while(!done.load()){
        if(GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            esc_pressed.store(true);
            break;
        }
        // Avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void monitor_tab_key() {
    while (!done.load()) {
        if (GetAsyncKeyState(VK_TAB) & 0x8000) {
            if (!tab_pressed.load()) { // prevent spamming
                tab_pressed.store(true);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


using namespace std;


int main() {
    std::thread esc_thread(monitor_escape_key);
    std::thread tab_thread(monitor_tab_key);

    std::string start_path = std::filesystem::current_path().string();

    auto fileSystem = FileSystemFactory::create(start_path);
    FileManager manager(std::move(fileSystem), start_path);
    std::cout << "Running (press ESC to quit)...\n" << std::flush;
    while (!esc_pressed.load())
    {
        manager.run();

        if (tab_pressed.load())
        {
            manager.isLeftPanelActive = !manager.isLeftPanelActive;
            tab_pressed.store(false); // reset the flag
        }

    }

    // signal monitor thread to end if it's still looping
    done.store(true);

    if(esc_thread.joinable()) {
        esc_thread.join();
    }

    if(tab_thread.joinable()) {
        tab_thread.join();
    }

    return 0;
}
