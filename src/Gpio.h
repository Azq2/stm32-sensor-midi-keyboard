#pragma once
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

namespace Gpio {
	constexpr uint8_t pin2id(uint16_t pin) {
		switch (pin) {
			case GPIO0:		return 0;
			case GPIO1:		return 1;
			case GPIO2:		return 2;
			case GPIO3:		return 3;
			case GPIO4:		return 4;
			case GPIO5:		return 5;
			case GPIO6:		return 6;
			case GPIO7:		return 7;
			case GPIO8:		return 8;
			case GPIO9:		return 9;
			case GPIO10:	return 10;
			case GPIO11:	return 11;
			case GPIO12:	return 12;
			case GPIO13:	return 13;
			case GPIO14:	return 14;
			case GPIO15:	return 15;
		}
		return 0;
	}
	
	constexpr uint16_t id2pin(uint8_t pin) {
		return 1 << pin;
	}
	
	constexpr uint8_t bank2id(uint32_t port) {
		// On stm32 all gpio registers are ordered and same indented
		return (port - GPIO_PORT_A_BASE) / 0x0400;
	}
	
	constexpr uint32_t id2bank(uint8_t id) {
		// On stm32 all gpio registers are ordered and same indented
		return GPIO_PORT_A_BASE + (id * 0x0400);
	}
	
	constexpr char bank2name(uint32_t bank) {
		// On stm32 all gpio registers are ordered and same indented
		return 'A' + bank2id(bank);
	}
	
	// Reduce power consumption
	inline void setAllAnalog() {
		rcc_periph_clock_enable(RCC_GPIOA);
		rcc_periph_clock_enable(RCC_GPIOB);
		rcc_periph_clock_enable(RCC_GPIOC);
		rcc_periph_clock_enable(RCC_GPIOD);
		
		gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO_ALL);
		gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO_ALL);
		gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO_ALL);
		gpio_set_mode(GPIOD, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO_ALL);
		
		rcc_periph_clock_disable(RCC_GPIOA);
		rcc_periph_clock_disable(RCC_GPIOB);
		rcc_periph_clock_disable(RCC_GPIOC);
		rcc_periph_clock_disable(RCC_GPIOD);
	}
}
