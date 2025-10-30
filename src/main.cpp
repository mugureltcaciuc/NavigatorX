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

using namespace std;

void monitor_key(int vk_code, std::atomic<bool>& flag) {
    while (!done.load()) {
        if (GetAsyncKeyState(vk_code) & 0x8000) {
            if (!flag.load()) {
                flag.store(true);
                while (GetAsyncKeyState(vk_code) & 0x8000) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    // Start key monitoring threads
    std::thread esc_thread(monitor_key, VK_ESCAPE, std::ref(esc_pressed));
    std::thread tab_thread(monitor_key, VK_TAB, std::ref(tab_pressed));
    std::thread down_thread(monitor_key, VK_DOWN, std::ref(down_pressed));
    std::thread up_thread(monitor_key, VK_UP, std::ref(up_pressed));

    tab_pressed.store(false);


    std::string start_path = std::filesystem::current_path().string();
    auto fileSystem = FileSystemFactory::create(start_path);
    FileManager manager(std::move(fileSystem), start_path);

    std::cout << "Running (press ESC to quit)...\n" << std::flush;

    while (!esc_pressed.load()) {
        manager.run();

        if (tab_pressed.load()) {
            manager.isLeftPanelActive = !manager.isLeftPanelActive;
            if(manager.isLeftPanelActive) {
                manager.selectedLeftIndex  = 0;
            }
            else {
                manager.selectedRightIndex  = 0;
            }

            tab_pressed.store(false);
        }

        if (down_pressed.load())
        {
            if(manager.isLeftPanelActive)
            {
                manager.selectedLeftIndex++;
            }
            else
            {
                manager.selectedRightIndex++;
            }
            down_pressed.store(false);
        }

        if (up_pressed.load())
        {
            if(manager.isLeftPanelActive)
            {
                if (manager.selectedLeftIndex > 0)
                {
                    manager.selectedLeftIndex--;
                }
            }
            else
            {
                if (manager.selectedRightIndex > 0)
                {
                    manager.selectedRightIndex--;
                }
            }

            up_pressed.store(false);
        }
    }

    done.store(true);

    // Join threads
    for (auto& t : { &esc_thread, &tab_thread, &down_thread, &up_thread }) {
        if (t->joinable()) t->join();
    }

    return 0;
}
