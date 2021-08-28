#include <stdio.h>

#include "constants.h"
#include "tasks.h"
#include "types.h"
#include "uart.h"
#include "utils.h"
#include "utils/uartstdio.h"

#include "driverlib/uart.h"

static float toFloat(char *s) {
  float result;
  sscanf(s, "%f", &result);
  return result;
}

static bool isValid(char c) { return isdigit(c) || c == '.' || c == '-'; }

void UARTReader(void *arg) {
  uart_reader_t *r = (uart_reader_t *)arg;
  printThreadInit(r->tid);

  track_manager_msg_t track_msg;
  obstacle_watcher_msg_t obstacle_msg;

  char c, buffer[BUFFER_SIZE];
  int i, bytesAvailable;

  for (;;) {
    bytesAvailable = UARTRxBytesAvail();
    osDelay(UART_READER_DEBOUNCE);

    if (bytesAvailable == 0 || bytesAvailable != UARTRxBytesAvail()) {
      continue;
    }

    // Message received
    i = 0;

    while ((c = UARTgetc()) != 'L')
      ; // Skip until 'L'

    c = UARTgetc();
    switch (c) {
    case 'r':
      // RF sensor reading - Lrf-?[0-9]+.[0-9]+
      UARTgetc(); // ignore 'f'

      while (UARTRxBytesAvail() && isValid(c = UARTgetc())) {
        buffer[i++] = c;
      }
      buffer[i] = '\0';

      track_msg.rf_reading = toFloat(buffer);
      osMessageQueuePut(track.args.qid, &track_msg, MSG_PRIO, osWaitForever);
      break;
    case 'u':
      // Ultrasonic sensor reading - Lu-?[0-9]+.[0-9]+
      while (UARTRxBytesAvail() && isValid(c = UARTgetc())) {
        buffer[i++] = c;
      }
      buffer[i] = '\0';
      obstacle_msg.sensor_reading = toFloat(buffer);
      osMessageQueuePut(obstacle.args.qid, &obstacle_msg, MSG_PRIO,
                        osWaitForever);
      break;
    default:
      // Discard
      while (UARTRxBytesAvail() && isValid(c = UARTgetc()))
        ;
    }
  }
}