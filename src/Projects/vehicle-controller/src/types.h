#ifndef __TYPES__H
#define __TYPES__H
#include "cmsis_os2.h"
#include <stdbool.h>

typedef enum {
  SW1_PRESSED,
  SW2_PRESSED,
} button_event_t;

typedef struct {
  char *content;
} uart_writer_msg_t;

typedef struct {
  osMessageQueueId_t qid;
} uart_writer_args_t;

typedef struct {
  osThreadAttr_t attr;
  osThreadId_t tid;
  uart_writer_args_t args;
} uart_writer_t;

typedef struct {
  osThreadAttr_t attr;
  osThreadId_t tid;
} uart_reader_t;

typedef struct {
  osMessageQueueId_t qid;
  int16_t target_speed;
} speed_controller_args_t;

typedef struct {
  osThreadAttr_t attr;
  osThreadId_t tid;
  bool isAccelerating;
  bool isBraking;
  speed_controller_args_t args;
} speed_controller_t;

typedef struct {
  osMessageQueueId_t qid;
  double reference;
  double gain;
} track_manager_args_t;

typedef struct {
  osThreadAttr_t attr;
  osThreadId_t tid;
  track_manager_args_t args;
} track_manager_t;

typedef struct {
  double sensor_reading;
} track_manager_msg_t;

typedef struct {
  osMessageQueueId_t qid;
} obstacle_watcher_args_t;

typedef struct {
  osThreadAttr_t attr;
  osThreadId_t tid;
  obstacle_watcher_args_t args;
} obstacle_watcher_t;

typedef struct {
  double sensor_reading;
} obstacle_watcher_msg_t;

#endif