#ifndef __CONSTANTS__H
#define __CONSTANTS__H

#define QUEUE_SIZE 10
#define BUFFER_SIZE 32

#define SECOND 1000

#define NO_WAIT 0U
#define MSG_PRIO 0U

#define DEBOUNCE_TICKS 300U

#define INITIAL_SPEED 0 // m/s
#define SPEED_INCR 5    // m/s
#define MAX_SPEED 15    // m/s

#define UART_READER_DEBOUNCE 100 // ticks

#define TRACK_MANAGER_PERIOD 10        // ticks
#define TRACK_MANAGER_READ_TIMEOUT 200 // ticks
#define TRACK_CENTER_REFERENCE -0.0358
#define TRACK_LEFT_REFERENCE -2.5

#define OBSTACLE_WARNING_DISTANCE 50          // m
#define OBSTACLE_WATCHER_READ_TIMEOUT 200     // ticks
#define OBSTACLE_WATCHER_PERIOD osWaitForever // ticks

#endif