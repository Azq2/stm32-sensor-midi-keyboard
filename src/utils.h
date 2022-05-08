#pragma once

#include <cstdint>
#include <libopencm3/stm32/gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __WFI() __asm__ volatile ("wfi")
#define __WFE() __asm__ volatile ("wfe")
#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))

__attribute__((always_inline)) static inline void __enable_irq(void) {
	__asm__ volatile ("cpsie i" : : : "memory");
}

__attribute__((always_inline)) static inline uint32_t __disable_irq(void) {
	uint32_t result;
	__asm__ volatile ("MRS %0, primask\ncpsid i" : "=r" (result));
	return result;
}

inline void gpio_set_value(uint32_t port, uint16_t pins, bool value) {
	value ? gpio_set(port, pins) : gpio_clear(port, pins);
}

void uart_simple_setup(uint32_t usart, uint32_t baudrate, bool printf);

void delay_init(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t us);

#ifdef __cplusplus
}
#endif
