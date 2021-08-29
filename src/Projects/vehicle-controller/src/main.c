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
obstacle_watcher_t obstacle = {.attr = {.name = "Obstacle Watcher"}};
speed_controller_t speed_ctl = {.attr = {.name = "Speed Controller"},
                                .args = {.target_speed = INITIAL_SPEED}};
track_manager_t track = {.attr = {.name = "Track Manager"},
                         .args = {.reference = TRACK_CENTER_REFERENCE,
                                  .period = TRACK_MANAGER_PERIOD}};

void main(void) {
  UARTInit();
  ButtonInit(USW1 | USW2);
  ButtonIntEnable(USW1 | USW2);

  waitForVehicleInit();

  UARTprintf("\r\n\n%s: Autoguided Vehicle Controller\n", name);
  UARTprintf("Version: %s\n\n", version);

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
      osMessageQueueNew(QUEUE_SIZE, sizeof(obstacle_watcher_msg_t), NULL);
  obstacle.tid = osThreadNew(ObstacleWatcher, &obstacle, &obstacle.attr);

  if (osKernelGetState() == osKernelReady)
    osKernelStart();

  // NOT REACHED
  while (1)
    ;
}
