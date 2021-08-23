#ifndef __UTILS__H
#define __UTILS__H
#include "cmsis_os2.h"
#include <stdbool.h>

bool isdigit(char c);

void printThreadInit(osThreadId_t tid);

// Push buttons are active in 0
#define ButtonPressed(b) !ButtonRead(b)

#endif