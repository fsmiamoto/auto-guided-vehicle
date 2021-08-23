#include "utils.h"
#include "uart.h"
#include "utils/uartstdio.h"


bool isdigit(char c) { return c >= '0' && c <= '9'; }

void printThreadInit(osThreadId_t tid) {
  const char *name = osThreadGetName(tid);
  UARTprintf("%s: initialized\n", name);
  UARTFlush();
}