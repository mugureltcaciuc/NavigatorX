// Globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <atomic>

extern std::atomic<bool> done;
extern std::atomic<bool> esc_pressed;
extern std::atomic<bool> tab_pressed;

#endif // GLOBALS_H
