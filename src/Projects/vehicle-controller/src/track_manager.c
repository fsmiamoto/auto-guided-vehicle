#include <stdlib.h>

#include "constants.h"
#include "tasks.h"
#include "uart.h"
#include "utils.h"
#include "utils/ustdlib.h"
#include <stdio.h>

#define Kp 2.5
#define Ki 1.0
#define Kd 4.0

void TrackManager(void *arg) {
  track_manager_t *t = (track_manager_t *)arg;
  uart_writer_msg_t readingRequest = {.content = ";Prf;"};
  uart_writer_msg_t turnRequest;
  track_manager_msg_t reading;

  float turn, error, last_error;
  float steering = 0.0, last_steering = 0.0;
  float derivative, integral = 0.0;

  uint32_t now, last_read_tick = osKernelGetTickCount();

  char buffer[BUFFER_SIZE];
  printThreadInit(t->tid);

  for (;;) {
    osDelay(TRACK_MANAGER_PERIOD);

    // Skip if vehicle stopped
    if (speed_ctl.args.target_speed == 0)
      continue;

    osMessageQueuePut(writer.args.qid, &readingRequest, MSG_PRIO,
                      osWaitForever);

    osStatus_t status = osMessageQueueGet(t->args.qid, &reading, MSG_PRIO,
                                          TRACK_MANAGER_READ_TIMEOUT);
    if (status != osOK) {
      continue;
    }

    now = osKernelGetTickCount();
    error = t->args.reference - reading.rf_reading;

    if ((error > 0 ? error : -error) < 0.005) {
      error = 0.0;
    }

    // PID Control
    derivative = error - last_error / (float)(now - last_read_tick);
    integral += error;

    steering = Kp * error + Ki * integral + Kd * derivative;

    turn = steering - last_steering;

    sprintf(buffer, ";V%f;", turn);
    turnRequest.content = buffer;
    osMessageQueuePut(writer.args.qid, &turnRequest, MSG_PRIO, osWaitForever);

    last_error = error;
    last_steering = steering;
    last_read_tick = now;
  }
}