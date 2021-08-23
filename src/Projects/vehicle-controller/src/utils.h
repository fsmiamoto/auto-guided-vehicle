#ifndef __UTILS__H
#define __UTILS__H
#include <stdbool.h>

bool isdigit(char c);

// Push buttons are active in 0
#define ButtonPressed(b) !ButtonRead(b)

#endif