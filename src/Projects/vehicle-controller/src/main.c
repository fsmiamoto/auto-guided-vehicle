#include "cmsis_os2.h"
#include <stdbool.h>
#include <stdio.h>

#include "driverbuttons.h"
#include "uart.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#include "types.h"

#define QUEUE_SIZE 10

#define SECOND 1000

#define NO_WAIT 0U
#define MSG_PRIO 0U

#define DEBOUNCE_TICKS 300U

#define SPEED_INCR 5 // m/s
#define MAX_SPEED 10 // m/s

// Push buttons are active in 0
#define ButtonPressed(b) !ButtonRead(b)

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

void stopAccelerating(void *arg) {
  if (speed_ctl.args.target_speed != 0) {
    uart_writer_msg_t msg = {.content = "A0;"};
    osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, NO_WAIT);
    speed_ctl.isAccelerating = false;
  }
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
      msg.content = "S;";
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
      msg.content = "A1;";
      osMessageQueuePut(writer.args.qid, &msg, MSG_PRIO, osWaitForever);
      osTimerStart(timer, SPEED_INCR * SECOND);
    }
  }
}

void TrackManager(void *arg) {
  track_manager_t *t = (track_manager_t *)arg;
  uart_writer_msg_t msg = {.content = "Prf;"};
  track_manager_msg_t reading;

  printThreadInit(t->tid);

  for (;;) {
    osDelay(100);
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

bool isdigit(char c) { return c >= '0' && c <= '9'; }

void UARTReader(void *arg) {
  uart_reader_t *r = (uart_reader_t *)arg;
  printThreadInit(r->tid);

  char c, buffer[32];
  int i;

  for (;;) {
    if (!UARTCharsAvailable()) {
      continue;
    }

    if (UARTPeek('L') && UARTPeek('.')) {
      i = 0;

      while ((c = UARTgetc()) != '-')
        ; // Skip until -

      while (1) {
        c = UARTgetc();
        if (!isdigit(c) && c != '.')
          break;
        buffer[i++] = c;
      }

      buffer[i] = '\0';
      double d = ustrtof(buffer, NULL);

      UARTprintf("\nreading: %d\n", d);
    }
  }
}

void waitForVehicle(void) {
  UARTprintf("R;");
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
