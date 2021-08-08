/**
 * @file unittest_transport.h
 * Functions used by platformio test framework to communicate
 *
 */

#ifndef UNITEST_TRANSPORT_H
#define UNITEST_TRANSPORT_H



#ifdef __cplusplus
extern "C" {
#endif

void unittest_uart_begin();
void unittest_uart_putchar(char c);
void unittest_uart_flush();
void unittest_uart_end();

#ifdef __cplusplus
}
#endif

#endif  // UNITEST_TRANSPORT_H