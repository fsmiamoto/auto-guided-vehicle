#ifndef __UART__H__
#define __UART__H__
#include <stdbool.h>

#define UART_BAUD_RATE 115200

void UARTInit(void);
void UART0_Handler(void);
void UARTFlush(void);
bool UARTCharsAvailable(void);

void UARTPrintAndFlush(const char *fmt, ...);

extern void UARTStdioIntHandler(void);
#endif