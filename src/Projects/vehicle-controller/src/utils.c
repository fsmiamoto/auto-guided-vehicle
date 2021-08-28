#include "utils.h"
#include "cmsis_os2.h"
#include "constants.h"
#include "tasks.h"
#include "types.h"
#include "uart.h"
#include "utils/uartstdio.h"

bool isdigit(char c) { return c >= '0' && c <= '9'; }

void printThreadInit(osThreadId_t tid) {
  const char *name = osThreadGetName(tid);
  UARTprintf("%s: initialized\n", name);
}

void waitForVehicleInit(void) {
  UARTprintf(";R;\r");
  UARTFlush();

  // FIXME: Actually wait for 'inicio' message here, this is ugly
  for (int i = 0; i < 10000000; i++)
    ;
}