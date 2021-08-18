#ifndef __UART__H__
#define __UART__H__

#define UART_BAUD_RATE 115200

void UARTInit(void);
void UART0_Handler(void);

extern void UARTStdioIntHandler(void);
#endif