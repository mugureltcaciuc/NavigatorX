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

using namespace std;
namespace fs = std::filesystem;



int main() {
    std::thread esc_thread(monitor_escape_key);

    auto fs = FileSystemFactory::create();
    FileManager manager(std::move(fs), ".");
    std::cout << "Running (press ESC to quit)...\n" << std::flush;
    while (!esc_pressed.load())
    {
        manager.run();
    }

    // signal monitor thread to end if it's still looping
    done.store(true);

    if(esc_thread.joinable()) {
        esc_thread.join();
    }

    return 0;
}
