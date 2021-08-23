#include "constants.h"
#include "types.h"
#include "uart.h"
#include "utils.h"
#include "utils/uartstdio.h"


void UARTReader(void *arg) {
  uart_reader_t *r = (uart_reader_t *)arg;
  printThreadInit(r->tid);

  char c, buffer[BUFFER_SIZE];
  int i;

  for (;;) {
    if (UARTPeek('L') && UARTPeek('r') && UARTPeek('f')) {
      i = 0;

      while (!isdigit((c = UARTgetc())))
        ; // Skip until first digit

      // TODO: Check for index boundary
      while (1) {
        c = UARTgetc();
        if (!isdigit(c) && c != '.')
          break;
        buffer[i++] = c;
      }

      buffer[i] = '\0';

      UARTprintf("\nreceived: %s\n", buffer);
    }
  }
}