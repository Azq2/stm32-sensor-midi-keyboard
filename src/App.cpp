#include "App.h"

#include <algorithm>
#include <climits>

#include "Gpio.h"
#include "utils.h"

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/dwt.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

App::Pin App::m_pins[] = {
	{GPIOB, GPIO13},
	{GPIOB, GPIO14},
	{GPIOB, GPIO15},
	{GPIOB, GPIO12},
	{GPIOA, GPIO8},
	{GPIOB, GPIO8},
	{GPIOB, GPIO9},
	{GPIOB, GPIO7},
	{GPIOB, GPIO3},
	{GPIOA, GPIO1},
	{GPIOB, GPIO1},
	{GPIOA, GPIO0},
	{GPIOB, GPIO4},
	{GPIOB, GPIO0},
	{GPIOA, GPIO7},
	{GPIOA, GPIO2},
	{GPIOA, GPIO4},
	{GPIOB, GPIO5},
	{GPIOB, GPIO11},
	{GPIOA, GPIO5},
	{GPIOA, GPIO15},
	{GPIOA, GPIO3},
	{GPIOA, GPIO6},
	{GPIOB, GPIO6},
};

void App::initHw() {
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	
	Gpio::setAllAnalog();
	
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	
	AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON;
	
	delay_init();
	
	// PA9 & PA10 - UART
	gpio_set_mode(GPIO_BANK_USART1_TX, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIO_BANK_USART1_RX, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
	
	// UART for debug
	rcc_periph_clock_enable(RCC_USART1);
	uart_simple_setup(USART1, 115200, true);
	
	// PC13 - status led
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO13);
	gpio_set_value(GPIOC, GPIO13, true);
}

void App::setPinMode(const Pin &p, PinMode mode) {
	switch (mode) {
		case MODE_UP:
			gpio_set_mode(p.port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, p.pin);
			gpio_set(p.port, p.pin);
		break;
		
		case MODE_DOWN:
			gpio_set_mode(p.port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, p.pin);
			gpio_clear(p.port, p.pin);
		break;
		
		case MODE_INPUT_UP:
			gpio_set_mode(p.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, p.pin);
			gpio_set(p.port, p.pin);
		break;
		
		case MODE_INPUT_DOWN:
			gpio_set_mode(p.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, p.pin);
			gpio_clear(p.port, p.pin);
		break;
	}
}

int App::readPulses(const Pin &p) {
	int start = 0, count = 0;
	for (int i = 0; i < SAMPLES_CNT; i++) {
		// Switch to input
		gpio_clear(p.port, p.pin);
		gpio_set_mode(p.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, p.pin);
		
		// Charge to >1.88V and measure elapsed time
		start = DWT_CYCCNT;
		gpio_set(p.port, p.pin);
		while (!gpio_get(p.port, p.pin));
		count += DWT_CYCCNT - start;
		
		// Complete charge to 3.3V
		gpio_set_mode(p.port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, p.pin);
		
		// Discharge to <1.23V and measure elapsed time
		gpio_set_mode(p.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, p.pin);
		
		start = DWT_CYCCNT;
		gpio_clear(p.port, p.pin);
		while (gpio_get(p.port, p.pin));
		count += DWT_CYCCNT - start;
		
		// Complete discharge to 0V
		gpio_set_mode(p.port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, p.pin);
	}
	return count / SAMPLES_CNT;
}

void App::initSensors() {
	printf("Sensors count: %d\r\n", COUNT_OF(m_pins));
	printf("Init sensors pins...\r\n");
	for (size_t i = 0; i < COUNT_OF(m_pins); i++)
		setPinMode(m_pins[i], MODE_DOWN);
	
	printf("Calibrating...\r\n");
	for (int j = 0; j < CALIBRATION_CYCLES; j++) {
		for (size_t i = 0; i < COUNT_OF(m_pins); i++) 
			m_pins[i].calibrate += readPulses(m_pins[i]);
	}
	
	for (size_t i = 0; i < COUNT_OF(m_pins); i++) {
		m_pins[i].calibrate = m_pins[i].calibrate / CALIBRATION_CYCLES;
		printf("%d - P%c%d = %d \r\n", i, Gpio::bank2name(m_pins[i].port), Gpio::pin2id(m_pins[i].pin), m_pins[i].calibrate);
	}
}

int App::readSensorValue(const Pin &p) {
	int delta = abs(readPulses(p) - p.calibrate);
	return (delta * 10000 / p.calibrate);
}

int App::run() {
	initHw();
	initSensors();
	
	if (DEBUG_SORT_KEYS) {
		bool used_gpios[COUNT_OF(m_pins)] = {false};
		size_t cnt = 0;
		
		printf("Press all keys one by one in right order:\r\n");
		
		while (true) {
			for (size_t i = 0; i < COUNT_OF(m_pins); i++) {
				int value = readSensorValue(m_pins[i]);
				bool pressed = isPressed(i, value);
				
				if (!used_gpios[i] && pressed) {
					printf("{GPIO%c, GPIO%d},\r\n", Gpio::bank2name(m_pins[i].port), Gpio::pin2id(m_pins[i].pin));
					used_gpios[i] = true;
					cnt++;
				}
			}
			
			if (cnt >= COUNT_OF(m_pins)) {
				printf("Done!\r\n");
				while (true);
			}
		}
	} else {
		struct {
			uint32_t time;
			bool state;
		} last_pressed[COUNT_OF(m_pins)] = {};
		int cnt = 0;
		
		printf("Init usb...\r\n");
		m_usb.init();
		
		printf("Polling...\r\n");
		while (true) {
			for (size_t i = 0; i < COUNT_OF(m_pins); i++) {
				m_usb.poll();
				
				int value = readSensorValue(m_pins[i]);
				bool pressed = isPressed(i, value);
				
				if (last_pressed[i].state != pressed) {
					uint32_t elapsed = DWT_CYCCNT - last_pressed[i].time;
					if (elapsed < DEBOUNCE_CYCLES)
						continue;
					
					last_pressed[i].time = DWT_CYCCNT;
					
					if (DEBUG_PRESS_KEYS) {
						printf("%d - P%c%d = %d [%.02f%%], elapsed=%lu\r\n", i, Gpio::bank2name(m_pins[i].port), Gpio::pin2id(m_pins[i].pin), pressed, (float) value / 100, elapsed);
					} else {
						m_usb.sendEvent(NOTE_OFFSET + i, 64, pressed);
					}
					
					if (pressed) {
						cnt++;
						
						if (pressed == 1)
							gpio_clear(GPIOC, GPIO13);
					} else {
						cnt--;
						
						if (!pressed)
							gpio_set(GPIOC, GPIO13);
					}
					
					last_pressed[i].state = pressed;
					cnt++;
				} else {
					last_pressed[i].time = DWT_CYCCNT;
				}
			}
		}
	}
	
	return 0;
}
