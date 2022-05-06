#include "utils.h"

#include <cstdio>

#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

static void main_hw_setup(void) {
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	
	// Setup systick
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_set_reload(rcc_ahb_frequency / 1000);
	systick_interrupt_enable();
}

static void debug_setup(void) {
	// UART for debug
	rcc_periph_clock_enable(RCC_USART1);
	uart_simple_setup(USART1, 115200, true);
	
	// PA9 & PA10 - UART
	gpio_set_mode(GPIO_BANK_USART1_TX, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIO_BANK_USART1_RX, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
	
	// PC13 - status led
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO13);
	gpio_set_value(GPIOC, GPIO13, true);
}

void sys_tick_handler(void) {
	
}

int main(void) {
	gpio_set_all_analog();
	delay_init();
	main_hw_setup();
	debug_setup();
	
	while (true) {
		printf("xuj\r\n");
	}
	
	return 0;
}
