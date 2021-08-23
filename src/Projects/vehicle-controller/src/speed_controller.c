#include "constants.h"
#include "tasks.h"

void printThreadInit(osThreadId_t tid);

static void stopAccelerating(void *arg) {
  if (speed_ctl.args.target_speed != 0) {
    uart_writer_msg_t msg = {.content = "A0;"};
    osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, NO_WAIT);
  }
  speed_ctl.isAccelerating = false;
}

void SpeedController(void *arg) {
  speed_controller_t *s = (speed_controller_t *)arg;
  button_event_t event;
  uart_writer_msg_t msg;

  osMessageQueueId_t queue_id = s->args.qid;
  osTimerId_t timer = osTimerNew(stopAccelerating, osTimerOnce, NULL, NULL);

  printThreadInit(s->tid);

  for (;;) {
    osMessageQueueGet(queue_id, &event, NULL, osWaitForever);
    if (event == SW2_PRESSED) {
      s->args.target_speed = 0;
      msg.content = ";S;";
      osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, osWaitForever);
    } else {
      // Ignore if would exceed speed limit
      if (s->args.target_speed + SPEED_INCR > MAX_SPEED)
        continue;

      // Already accelerating, ignore
      if (s->isAccelerating)
        continue;

      s->isAccelerating = true;
      s->args.target_speed += SPEED_INCR;
      msg.content = ";A1;";
      osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, osWaitForever);
      osTimerStart(timer, SPEED_INCR * SECOND);
    }
  }
}