#include "types.h"
#include "uart.h"
#include "utils.h"
#include "utils/uartstdio.h"
#include "constants.h"

void UARTWriter(void *arg) {
  uart_writer_t *w = (uart_writer_t *)arg;
  uart_writer_msg_t msg;

  osMessageQueueId_t queue_id = w->args.qid;

  printThreadInit(w->tid);

  for (;;) {
    osMessageQueueGet(queue_id, &msg, MSG_PRIO, osWaitForever);
    UARTprintf("\n%s", msg.content);
  }
}