#include "cmsis_os2.h" // CMSIS-RTOS
#include <stdbool.h>
#include <stdio.h>

#include "uart.h"
#include "utils/uartstdio.h"

typedef struct {
  osThreadAttr_t attr;
  osThreadId_t tid;
} uart_writer_t;

const char *name = "Osewa";
const char *version = "0.0.1";

uart_writer_t writer = {.attr = {.name = "UART Writer"}};

void initMessage(void) {
  UARTprintf("%s: Autoguided Vehicle Controller\n", name);
  UARTprintf("Version: %s\n", version);
  UARTFlush();
}

void UARTWriter(void *arg) {
  uart_writer_t *args = (uart_writer_t *)arg;
  UARTprintf("%s: initialized;\n", osThreadGetName(args->tid));
  UARTFlush();

  for (;;) {
    UARTprintf("A1.5;\n");
    UARTFlush();
    osDelay(1000);
    UARTprintf("A0;\n");
    UARTFlush();
    osDelay(5000);
    UARTprintf("S;\n");
    UARTFlush();
    osDelay(10000);
  }
}

void main(void) {
  UARTInit();
  initMessage();

  if (osKernelGetState() == osKernelInactive)
    osKernelInitialize();

  writer.tid = osThreadNew(UARTWriter, &writer, &writer.attr);

  if (osKernelGetState() == osKernelReady)
    osKernelStart();

  // NOT REACHED
  while (1)
    ;
}
