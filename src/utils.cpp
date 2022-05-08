#include "utils.h"

#include <cstdio>
#include <cerrno>
#include <unistd.h>

#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/dwt.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

static uint32_t uart_for_prinf = 0;

void uart_simple_setup(uint32_t usart, uint32_t baudrate, bool use_for_printf) {
	usart_set_baudrate(usart, baudrate);
	usart_set_databits(usart, 8);
	usart_set_stopbits(usart, USART_STOPBITS_1);
	usart_set_parity(usart, USART_PARITY_NONE);
	usart_set_flow_control(usart, USART_FLOWCONTROL_NONE);
	usart_set_mode(usart, USART_MODE_TX_RX);
	usart_enable(usart);
	
	if (use_for_printf)
		uart_for_prinf = usart;
}

void delay_init(void) {
	dwt_enable_cycle_counter();
}

void delay_us(uint32_t us) {
	uint32_t cycles = dwt_read_cycle_counter() + (rcc_ahb_frequency / 1000000) * us;
	while (dwt_read_cycle_counter() <= cycles);
}

void delay_ms(uint32_t us) {
	uint32_t cycles = dwt_read_cycle_counter() + (rcc_ahb_frequency / 1000) * us;
	while (dwt_read_cycle_counter() <= cycles);
}

extern "C" int _write(int file, char *ptr, int len) {
	int i;
	if (uart_for_prinf && (file == STDOUT_FILENO || file == STDERR_FILENO)) {
		for (i = 0; i < len; ++i)
			usart_send_blocking(USART1, ptr[i]);
		return i;
	}
	errno = EIO;
	return -1;
}
