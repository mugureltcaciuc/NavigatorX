// Globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <atomic>

extern std::atomic<bool> esc_pressed;
extern std::atomic<bool> tab_pressed;
extern std::atomic<bool> down_pressed;
extern std::atomic<bool> up_pressed;
extern std::atomic<bool> done;

#endif // GLOBALS_H
