#include "cmsis_os2.h"
#include "constants.h"
#include "driverbuttons.h"
#include "tasks.h"
#include "utils.h"

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