#include "constants.h"
#include "tasks.h"
#include "utils.h"

static void returnToCenter(void *arg) {
  track.args.reference = TRACK_CENTER_REFERENCE;
}

void ObstacleWatcher(void *arg) {
  obstacle_watcher_t *o = (obstacle_watcher_t *)arg;
  obstacle_watcher_msg_t reading;

  osTimerId_t timer = osTimerNew(returnToCenter, osTimerOnce, NULL, NULL);

  printThreadInit(o->tid);

  for (;;) {
    osMessageQueueGet(o->args.qid, &reading, MSG_PRIO, osWaitForever);
    track.args.reference = TRACK_LEFT_REFERENCE;
    osTimerStart(timer,
                 OBSTACLE_WARNING_DISTANCE / speed_ctl.args.target_speed);
  }
}