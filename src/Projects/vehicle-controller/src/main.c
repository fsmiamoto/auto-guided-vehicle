#include "cmsis_os2.h" // CMSIS-RTOS
#include <stdbool.h>
#include <stdio.h>

#include "driverbuttons.h"
#include "uart.h"
#include "utils/uartstdio.h"

#define UART_WRITER_QUEUE_SIZE 8

#define NO_WAIT 0U
#define MSG_PRIO 0U

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
  const char *name = osThreadGetName(w->tid);
  osMessageQueueId_t queue_id = w->args.qid;

  UARTprintf("%s: initialized;\n", name);
  UARTFlush();

  for (;;) {
    UARTprintf("%s: waiting for msg\n", name);
    osMessageQueueGet(queue_id, &msg, NULL, osWaitForever);
    UARTprintf("%s\n", msg.content);
  }
}
void GPIOJ_Handler(void) {
  // Used for debouncing
  static uint32_t tick_last_msg_sw1, tick_last_msg_sw2;

  ButtonIntClear(USW1 | USW2);

  if (ButtonPressed(USW1)) {
    if ((osKernelGetTickCount() - tick_last_msg_sw1) < DEBOUNCE_TICKS)
      return;

    uart_writer_msg_t msg = {.content = "A1.0;", .size = 5};
    osStatus_t status =
        osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, NO_WAIT);
    if (status == osOK)
      tick_last_msg_sw1 = osKernelGetTickCount();
  }

  if (ButtonPressed(USW2)) {
    if ((osKernelGetTickCount() - tick_last_msg_sw2) < DEBOUNCE_TICKS)
      return;

    uart_writer_msg_t msg = {.content = "S;", .size = 2};
    osStatus_t s = osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, NO_WAIT);
    if (s == osOK)
      tick_last_msg_sw2 = osKernelGetTickCount();
  }
}

void main(void) {
  UARTInit();
  ButtonInit(USW1 | USW2);
  ButtonIntEnable(USW1 | USW2);
  initMessage();

  if (osKernelGetState() == osKernelInactive)
    osKernelInitialize();

  writer.args.qid = osMessageQueueNew(UART_WRITER_QUEUE_SIZE,
                                      sizeof(uart_writer_msg_t), NULL);
  writer.tid = osThreadNew(UARTWriter, &writer, &writer.attr);

  if (osKernelGetState() == osKernelReady)
    osKernelStart();

  // NOT REACHED
  while (1)
    ;
}
