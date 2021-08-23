#include "cmsis_os2.h"
#include "constants.h"
#include "tasks.h"
#include "utils.h"
#include "utils/ustdlib.h"


void TrackManager(void *arg) {
  track_manager_t *t = (track_manager_t *)arg;
  uart_writer_msg_t readingRequest = {.content = ";Prf;"};
  uart_writer_msg_t turnRequest;
  track_manager_msg_t reading;

  char buffer[32];
  printThreadInit(t->tid);

  for (;;) {
    osDelay(1000);
    osMessageQueuePut(writer.args.qid, &readingRequest, MSG_PRIO,
                      osWaitForever);
    osMessageQueueGet(t->args.qid, &reading, MSG_PRIO, osWaitForever);

    double turn = t->args.gain * (t->args.reference - reading.sensor_reading);
    usnprintf(buffer, 32, ";V%d;", turn);
    turnRequest.content = buffer;

    osMessageQueuePut(writer.args.qid, &turnRequest, MSG_PRIO, osWaitForever);
  }
}