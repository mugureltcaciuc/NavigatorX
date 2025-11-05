#include <atomic>
#include <chrono>
#include <conio.h>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>

#include "FileSystemFactory.h"
#include "FileManager.h"
#include "WindowsFileSystem.h"
#include "Globals.h"

#pragma comment(lib, "User32.lib")

using namespace std;

void monitor_escape_key(std::atomic<bool>& flag) {
    bool pressed_once = false;
    while (!done.load()) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            if (!pressed_once) {
                flag.store(true);
                pressed_once = true;
                while (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        } else {
            pressed_once = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void monitor_down_key(std::atomic<bool>& flag) {
    bool pressed_once = false;
    while (!done.load()) {
        if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
            if (!pressed_once) {
                flag.store(true);
                pressed_once = true;
                while (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        } else {
            pressed_once = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void monitor_up_key(std::atomic<bool>& flag) {
    bool pressed_once = false;
    while (!done.load()) {
        if (GetAsyncKeyState(VK_UP) & 0x8000) {
            if (!pressed_once) {
                flag.store(true);
                pressed_once = true;
                while (GetAsyncKeyState(VK_UP) & 0x8000) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        } else {
            pressed_once = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


int main()
{
    // Start key monitoring threads
    std::thread esc_thread(monitor_escape_key, std::ref(esc_pressed));
    std::thread down_thread(monitor_down_key, std::ref(down_pressed));
    std::thread up_thread(monitor_up_key, std::ref(up_pressed));

    tab_pressed.store(false);

    std::string start_path = std::filesystem::current_path().string();
    auto fileSystem = FileSystemFactory::create(start_path, start_path);
    FileManager manager(std::move(fileSystem), start_path);
    manager.isLeftPanelActive = true;

    std::cout << "Running (press ESC to quit)...\n"
              << std::flush;

    while (!esc_pressed.load())
    {
        manager.run();

        if (tab_pressed.load())
        {
            manager.isLeftPanelActive = !manager.isLeftPanelActive;
            if (manager.isLeftPanelActive)
            {
                manager.selectedLeftIndex = 0;
            }
            else
            {
                manager.selectedRightIndex = 0;
            }

            tab_pressed.store(false);
        }

        if (down_pressed.load())
        {
            if (manager.isLeftPanelActive)
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
            if (manager.isLeftPanelActive)
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
    for (auto &t : {&esc_thread, &down_thread, &up_thread})
    {
        if (t->joinable())
            t->join();
    }

    return 0;
}
