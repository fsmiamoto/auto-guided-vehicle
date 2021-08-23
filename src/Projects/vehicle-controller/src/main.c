#include <stdbool.h>
#include <stdio.h>

#include "cmsis_os2.h"
#include "driverbuttons.h"
#include "uart.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#include "constants.h"
#include "tasks.h"
#include "types.h"
#include "utils.h"

const char *name = "Osewa";
const char *version = "0.0.1";

uart_writer_t writer = {.attr = {.name = "UART Writer"}};
uart_reader_t reader = {.attr = {.name = "UART Reader"}};
speed_controller_t speed_ctl = {.attr = {.name = "Speed Controller"},
                                .args = {.target_speed = 0}};
track_manager_t track = {.attr = {.name = "Track Manager"}};
obstacle_watcher_t obstacle = {.attr = {.name = "Obstacle Watcher"}};

void initMessage(void) {
  UARTprintf("\r\n%s: Autoguided Vehicle Controller\n", name);
  UARTprintf("Version: %s\n", version);
  UARTFlush();
}

void printThreadInit(osThreadId_t tid) {
  const char *name = osThreadGetName(tid);
  UARTprintf("%s: initialized\n", name);
  UARTFlush();
}

void UARTWriter(void *arg) {
  uart_writer_t *w = (uart_writer_t *)arg;
  uart_writer_msg_t msg;

  osMessageQueueId_t queue_id = w->args.qid;

  printThreadInit(w->tid);

  for (;;) {
    osMessageQueueGet(queue_id, &msg, NULL, osWaitForever);
    UARTprintf("%s\n", msg.content);
  }
}

void TrackManager(void *arg) {
  track_manager_t *t = (track_manager_t *)arg;
  uart_writer_msg_t msg = {.content = ";Prf;"};
  track_manager_msg_t reading;

  printThreadInit(t->tid);

  for (;;) {
    osDelay(1000);
    osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, osWaitForever);
    osMessageQueueGet(t->args.qid, &reading, MSG_PRIO, osWaitForever);
  }
}

void ObstacleWatcher(void *arg) {
  obstacle_watcher_t *o = (obstacle_watcher_t *)arg;
  obstacle_watcher_msg_t reading;

  printThreadInit(o->tid);

  for (;;) {
    osMessageQueueGet(o->args.qid, &reading, MSG_PRIO, osWaitForever);
  }
}

void UARTReader(void *arg) {
  uart_reader_t *r = (uart_reader_t *)arg;
  printThreadInit(r->tid);

  char c, buffer[32];
  int i;

  for (;;) {
    if (UARTPeek('L') && UARTPeek('.')) {
      i = 0;

      while (!isdigit((c = UARTgetc())))
        ; // Skip until first digit

      // TODO: Check for index boundary
      while (1) {
        c = UARTgetc();
        if (!isdigit(c) && c != '.')
          break;
        buffer[i++] = c;
      }

      buffer[i] = '\0';
    }
  }
}

void waitForVehicle(void) {
  UARTprintf(";R;\r");
  UARTFlush();
  // TODO: Actually wait for 'inicio' message here.
}

void main(void) {
  UARTInit();
  ButtonInit(USW1 | USW2);
  ButtonIntEnable(USW1 | USW2);
  initMessage();

  waitForVehicle();

  if (osKernelGetState() == osKernelInactive)
    osKernelInitialize();

  writer.args.qid =
      osMessageQueueNew(QUEUE_SIZE, sizeof(uart_writer_msg_t), NULL);
  writer.tid = osThreadNew(UARTWriter, &writer, &writer.attr);

  reader.tid = osThreadNew(UARTReader, &reader, &reader.attr);

  speed_ctl.args.qid =
      osMessageQueueNew(QUEUE_SIZE, sizeof(button_event_t), NULL);
  speed_ctl.tid = osThreadNew(SpeedController, &speed_ctl, &speed_ctl.attr);

  track.args.qid =
      osMessageQueueNew(QUEUE_SIZE, sizeof(track_manager_msg_t), NULL);
  track.tid = osThreadNew(TrackManager, &track, &track.attr);

  obstacle.args.qid =
      osMessageQueueNew(QUEUE_SIZE, sizeof(obstacle_watcher_t), NULL);
  obstacle.tid = osThreadNew(ObstacleWatcher, &obstacle, &obstacle.attr);

  if (osKernelGetState() == osKernelReady)
    osKernelStart();

  // NOT REACHED
  while (1)
    ;
}
