#include "cmsis_os2.h" // CMSIS-RTOS
#include <stdbool.h>
#include <stdio.h>

#include "uart.h"
#include "utils/uartstdio.h"

osThreadId_t thread1_id, thread2_id;
osMutexId_t uart_id;

const osThreadAttr_t thread1_attr = {.name = "Thread 1"};

void myKernelInfo(void) {
  osVersion_t osv;
  char infobuf[16];
  if (osKernelGetInfo(&osv, infobuf, sizeof(infobuf)) == osOK) {
    UARTprintf("Kernel Information: %s\n", infobuf);
    UARTprintf("Kernel Version    : %d\n", osv.kernel);
    UARTprintf("Kernel API Version: %d\n\n", osv.api);
    UARTFlushTx(false);
  }
}

void myKernelState(void) {
  UARTprintf("Kernel State: ");
  switch (osKernelGetState()) {
  case osKernelInactive:
    UARTprintf("Inactive\n\n");
    break;
  case osKernelReady:
    UARTprintf("Ready\n\n");
    break;
  case osKernelRunning:
    UARTprintf("Running\n\n");
    break;
  case osKernelLocked:
    UARTprintf("Locked\n\n");
    break;
  case osKernelError:
    UARTprintf("Error\n\n");
    break;
  }
  UARTFlushTx(false);
}

void myThreadState(osThreadId_t thread_id) {
  osThreadState_t state;
  state = osThreadGetState(thread_id);
  switch (state) {
  case osThreadInactive:
    UARTprintf("Inactive\n");
    break;
  case osThreadReady:
    UARTprintf("Ready\n");
    break;
  case osThreadRunning:
    UARTprintf("Running\n");
    break;
  case osThreadBlocked:
    UARTprintf("Blocked\n");
    break;
  case osThreadTerminated:
    UARTprintf("Terminated\n");
    break;
  case osThreadError:
    UARTprintf("Error\n");
    break;
  }
}

void myThreadInfo(void) {
  osThreadId_t threads[8];
  uint32_t number = osThreadEnumerate(threads, sizeof(threads));

  UARTprintf("Number of active threads: %d\n", number);
  for (int n = 0; n < number; n++) {
    UARTprintf("  %s (priority %d) - ", osThreadGetName(threads[n]),
               osThreadGetPriority(threads[n]));
    myThreadState(threads[n]);
  } // for
  UARTprintf("\n");
  UARTFlushTx(false);
}

__NO_RETURN void osRtxIdleThread(void *argument) {
  (void)argument;

  for (;;) {
    // UARTprintf("Idle thread\n");
    asm("wfi");
  }
}

__NO_RETURN void thread1(void *arg) {
  for (;;) {

    osMutexAcquire(uart_id, osWaitForever);
    // UARTprintf("A1.5;");
    osMutexRelease(uart_id);

    for (int d = 0; d < 10000; d++)
      ;

    osMutexAcquire(uart_id, osWaitForever);
    // UARTprintf("S;");
    osMutexRelease(uart_id);

    osDelay(1000);
  }
}

void main(void) {
  UARTInit();
  myKernelInfo();
  myKernelState();

  if (osKernelGetState() == osKernelInactive)
    osKernelInitialize();

  myKernelState();

  thread1_id = osThreadNew(thread1, NULL, &thread1_attr);
  uart_id = osMutexNew(NULL);

  if (osKernelGetState() == osKernelReady)
    osKernelStart();

  // NOT REACHED
  while (1)
    ;
}
