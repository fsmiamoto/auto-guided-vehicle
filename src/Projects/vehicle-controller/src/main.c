#include "cmsis_os2.h" // CMSIS-RTOS
#include <stdbool.h>
#include <stdio.h>

#include "driverleds.h"
#include "uart.h"
#include "utils/uartstdio.h"

#define UART_WRITER_QUEUE_SIZE 8

#define DEBOUNCE_TICKS 300U

// Push buttons are active in 0
#define ButtonPressed(b) !ButtonRead(b)

typedef enum {
  SW1_PRESSED,
  SW2_PRESSED,
} button_event_t;

typedef struct {
  char *content;
  uint8_t size;
} uart_writer_msg_t;

typedef struct {
  osMessageQueueId_t qid;
} uart_writer_args_t;

typedef struct {
  osThreadAttr_t attr;
  osThreadId_t tid;
  uart_writer_args_t args;
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
  uart_writer_t *w = (uart_writer_t *)arg;
  uart_writer_msg_t msg;
  osStatus_t status;
  const char *name = osThreadGetName(w->tid);
  osMessageQueueId_t queue_id = w->args.qid;

  UARTprintf("%s: initialized;\n", name);
  UARTFlush();

  for (;;) {
    UARTprintf("%s: waiting for msg", name);
    osMessageQueueGet(queue_id, &msg, NULL, osWaitForever);
    UARTprintf("%s\n", msg.content);
  }
}

void main(void) {
  UARTInit();
  initMessage();

  if (osKernelGetState() == osKernelInactive)
    osKernelInitialize();

  writer.args.qid =
      osMessageQueueNew(UART_WRITER_QUEUE_SIZE, sizeof(button_event_t), NULL);
  writer.tid = osThreadNew(UARTWriter, &writer, &writer.attr);

  if (osKernelGetState() == osKernelReady)
    osKernelStart();

  // NOT REACHED
  while (1)
    ;
}
