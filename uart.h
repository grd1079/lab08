#ifndef _UART_H
#define _UART_H

#include "MK64F12.h"
#include <stdint.h>

#define RED_LED		(1 << 22)
#define BLUE_LED 	(1 << 21)
#define GREEN_LED	(1 << 26)
#define CAMERA_CLK (1 << 9)
#define CAMERA_SI (1 << 23)

void LED_Init(void);
void uart0_put(char *ptr_str), uart3_put(char *ptr_str);
void uart_init(void);
uint8_t uart0_getchar(void), uart3_getchar(void);
void uart0_putchar(char ch), uart3_putchar(char ch);
void uart0_putNumU(int i);

#endif /* UART_H */
