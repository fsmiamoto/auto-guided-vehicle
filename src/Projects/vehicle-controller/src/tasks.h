#ifndef __TASKS__H
#define __TASKS__H
#include "types.h"
extern uart_writer_t writer;
extern uart_reader_t reader;
extern speed_controller_t speed_ctl;
extern track_manager_t track;
extern obstacle_watcher_t obstacle;

void SpeedController(void *arg);
void UARTReader(void *arg);
void UARTWriter(void *arg);
void TrackManager(void *arg);
void ObstacleWatcher(void *arg);

#endif