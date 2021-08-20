#include "cmsis_os2.h" // CMSIS-RTOS
#include <stdbool.h>
#include <stdio.h>

#include "driverbuttons.h"
#include "uart.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#define QUEUE_SIZE 10

#define SECOND 1000

#define NO_WAIT 0U
#define MSG_PRIO 0U

#define DEBOUNCE_TICKS 300U

#define TARGET_SPEED 5 // m/s

// Push buttons are active in 0
#define ButtonPressed(b) !ButtonRead(b)

typedef enum {
  SW1_PRESSED,
  SW2_PRESSED,
} button_event_t;

typedef struct {
  char *content;
} uart_writer_msg_t;

typedef struct {
  osMessageQueueId_t qid;
} uart_writer_args_t;

typedef struct {
  osThreadAttr_t attr;
  osThreadId_t tid;
  uart_writer_args_t args;
} uart_writer_t;

typedef struct {
  osThreadAttr_t attr;
  osThreadId_t tid;
} uart_reader_t;

typedef struct {
  osMessageQueueId_t qid;
  int16_t target_speed;
} speed_controller_args_t;

typedef struct {
  osThreadAttr_t attr;
  osThreadId_t tid;
  speed_controller_args_t args;
} speed_controller_t;

const char *name = "Osewa";
const char *version = "0.0.1";

uart_writer_t writer = {.attr = {.name = "UART Writer"}};
uart_reader_t reader = {.attr = {.name = "UART Reader"}};
speed_controller_t speed_ctl = {.attr = {.name = "Speed Controller"},
                                .args = {.target_speed = TARGET_SPEED}};

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
    osMessageQueueGet(queue_id, &msg, NULL, osWaitForever);
    UARTprintf("%s\n", msg.content);
  }
}

void stopAccelarating(void *arg) {
  uart_writer_msg_t msg = {.content = "A0;"};
  osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, NO_WAIT);
}

void SpeedController(void *arg) {
  speed_controller_t *s = (speed_controller_t *)arg;
  button_event_t event;
  uart_writer_msg_t msg;

  const char *name = osThreadGetName(s->tid);
  osMessageQueueId_t queue_id = s->args.qid;

  osTimerId_t timer = osTimerNew(stopAccelarating, osTimerOnce, NULL, NULL);

  UARTprintf("%s: initialized;\n", name);
  UARTFlush();

  for (;;) {
    osMessageQueueGet(queue_id, &event, NULL, osWaitForever);
    if (event == SW2_PRESSED) {
      s->args.target_speed = 0;
      msg.content = "S;";
      osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, osWaitForever);
    } else {
      msg.content = "A1;";
      osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, osWaitForever);
      osTimerStart(timer, s->args.target_speed * SECOND);
    }
  }
}

void GPIOJ_Handler(void) {
  // Used for debouncing
  static uint32_t tick_last_msg_sw1, tick_last_msg_sw2;

  ButtonIntClear(USW1 | USW2);

  if (ButtonPressed(USW1)) {
    if ((osKernelGetTickCount() - tick_last_msg_sw1) < DEBOUNCE_TICKS)
      return;

    button_event_t event = SW1_PRESSED;
    osStatus_t status =
        osMessageQueuePut(speed_ctl.args.qid, &event, MSG_PRIO, NO_WAIT);
    if (status == osOK)
      tick_last_msg_sw1 = osKernelGetTickCount();
  }

  if (ButtonPressed(USW2)) {
    if ((osKernelGetTickCount() - tick_last_msg_sw2) < DEBOUNCE_TICKS)
      return;

    button_event_t event = SW2_PRESSED;
    osStatus_t status =
        osMessageQueuePut(speed_ctl.args.qid, &event, MSG_PRIO, NO_WAIT);
    if (status == osOK)
      tick_last_msg_sw2 = osKernelGetTickCount();
  }
}

void UARTReader(void *arg) {
  char buffer[32];
  char c;
  uint8_t i;
  for (;;) {
    if (UARTPeek(';')) {
      i = 0;
      while ((c = UARTgetc()) != ';') {
        buffer[i++] = c;
      }
      buffer[i] = '\0';
      float reading = ustrtof(buffer, NULL);
      UARTprintf("\nreading: %d\n", reading);
    }
  }
}

void main(void) {
  UARTInit();
  ButtonInit(USW1 | USW2);
  ButtonIntEnable(USW1 | USW2);
  initMessage();

  if (osKernelGetState() == osKernelInactive)
    osKernelInitialize();

  writer.args.qid =
      osMessageQueueNew(QUEUE_SIZE, sizeof(uart_writer_msg_t), NULL);
  writer.tid = osThreadNew(UARTWriter, &writer, &writer.attr);

  reader.tid = osThreadNew(UARTReader, &reader, &reader.attr);

  speed_ctl.args.qid =
      osMessageQueueNew(QUEUE_SIZE, sizeof(button_event_t), NULL);
  speed_ctl.tid = osThreadNew(SpeedController, &speed_ctl, &speed_ctl.attr);

  if (osKernelGetState() == osKernelReady)
    osKernelStart();

  // NOT REACHED
  while (1)
    ;
}
