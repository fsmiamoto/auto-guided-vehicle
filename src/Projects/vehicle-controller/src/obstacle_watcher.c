#include "constants.h"
#include "tasks.h"
#include "utils.h"

#define TIMEOUT 200

static void returnToCenter(void *arg) {
  track.args.reference = TRACK_CENTER_REFERENCE;
}

void ObstacleWatcher(void *arg) {
  obstacle_watcher_t *o = (obstacle_watcher_t *)arg;
  obstacle_watcher_msg_t reading;
  uart_writer_msg_t readingRequest = {.content = ";Pu;"};
  
  
  osTimerId_t timer = osTimerNew(returnToCenter, osTimerOnce, NULL, NULL);

  printThreadInit(o->tid);

  for (;;) {
    osDelay(OBSTACLE_WATCHER_PERIOD);

    osMessageQueuePut(writer.args.qid, &readingRequest, MSG_PRIO,
                      osWaitForever);
    osStatus_t status =
        osMessageQueueGet(o->args.qid, &reading, MSG_PRIO, TIMEOUT);
    if (status != osOK) {
      continue;
    }

    if(reading.sensor_reading < 0) continue;
    
    if (reading.sensor_reading <= OBSTACLE_WARNING_DISTANCE && track.args.reference != TRACK_LEFT_REFERENCE) {
      track.args.reference = TRACK_LEFT_REFERENCE;
      osTimerStart(timer,
                   SECOND*OBSTACLE_WARNING_DISTANCE / speed_ctl.args.target_speed);
    }
  }
}