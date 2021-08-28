#include <stdlib.h>

#include "constants.h"
#include "tasks.h"
#include "uart.h"
#include "utils.h"
#include "utils/ustdlib.h"
#include <stdio.h>

#define Kp 6
#define Ki 1
#define Kd 10

void TrackManager(void *arg) {
  track_manager_t *t = (track_manager_t *)arg;
  uart_writer_msg_t readingRequest = {.content = ";Prf;"};
  uart_writer_msg_t turnRequest;
  track_manager_msg_t reading;

  float turn, error, last_error;
  float abs_error, ref = 0.0, last_ref = 0.0;
  float derivative, integral = 0.0;

  char buffer[BUFFER_SIZE];
  printThreadInit(t->tid);

  for (;;) {
    osDelay(TRACK_MANAGER_PERIOD);

    // Skip if vehicle stopped
    if (speed_ctl.args.target_speed == 0)
      continue;

    osMessageQueuePut(writer.args.qid, &readingRequest, MSG_PRIO,
                      osWaitForever);
    osMessageQueueGet(t->args.qid, &reading, MSG_PRIO, osWaitForever);

    error = t->args.reference - reading.rf_reading;
    abs_error = error > 0 ? error : -error;

    if (abs_error < 0.01) {
      error = 0;
    }

    // PID Control
    derivative = error - last_error;
    integral += error;
    ref = Kp * error + Ki * integral + Kd * derivative;

    turn = ref - last_ref;

    sprintf(buffer, ";V%f;", turn);
    turnRequest.content = buffer;
    osMessageQueuePut(writer.args.qid, &turnRequest, MSG_PRIO, osWaitForever);

    last_error = error;
    last_ref = ref;
  }
}