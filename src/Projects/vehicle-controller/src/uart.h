#ifndef __UART__H__
#define __UART__H__

#define UART_BAUD_RATE 115200

void UARTInit(void);
void UART0_Handler(void);
void UARTFlush(void);

void UARTPrintAndFlush(const char *fmt, ...);

extern void UARTStdioIntHandler(void);
#endif