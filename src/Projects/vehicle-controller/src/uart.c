#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "system_TM4C1294.h"
#include "uart.h"
#include "utils/uartstdio.h"

void UARTInit(void) {
  // Enable UART0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    ;

  // Initialize the UART for console I/O.
  UARTStdioConfig(0, UART_BAUD_RATE, SystemCoreClock);

  // Enable the GPIO Peripheral used by the UART.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    ;

  // Configure GPIO Pins for UART mode.
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  
  UARTFlushRx();
  UARTFlushTx(true);
}

void UART0_Handler(void) { UARTStdioIntHandler(); }

bool UARTCharsAvailable(void) { return UARTCharsAvail(UART0_BASE); }

// Print and then flush
// TODO: Debug
void UARTPrintAndFlush(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  UARTprintf(fmt, args);
  UARTFlush();
  va_end(args);
}

// Flush the Transmmit Buffer
void UARTFlush() { UARTFlushTx(false); }