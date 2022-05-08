#include "App.h"

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
	
	// Setup systick
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_set_reload(rcc_ahb_frequency / 1000);
	systick_interrupt_enable();
	
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
	uint32_t start = dwt_read_cycle_counter();
	for (int i = 0; i < 8; i++) {
		setPinMode(p, MODE_INPUT_UP);
		while (!gpio_get(p.port, p.pin));
		setPinMode(p, MODE_UP);
		
		setPinMode(p, MODE_INPUT_DOWN);
		while (gpio_get(p.port, p.pin));
		setPinMode(p, MODE_DOWN);
	}
	return dwt_read_cycle_counter() - start;
}

void App::initSensors() {
	printf("Sensors count: %d\r\n", COUNT_OF(m_pins));
	printf("Init sensors pins...\r\n");
	for (size_t i = 0; i < COUNT_OF(m_pins); i++)
		setPinMode(m_pins[i], MODE_DOWN);
	
	printf("Calibrating...\r\n");
	for (size_t i = 0; i < COUNT_OF(m_pins); i++)
		m_pins[i].calibrate = readPulses(m_pins[i]);
}

bool App::isPressed(const Pin &p) {
	int cnt = readPulses(p);
	float pct = 100 - (float) p.calibrate / (float) cnt * 100;
	return pct > 1;
}

int App::run() {
	initHw();
	initSensors();
	
	if (DEBUG_KEYS) {
		bool used_gpios[COUNT_OF(m_pins)] = {false};
		size_t cnt = 0;
		
		printf("Press all keys one by one in right order:\r\n");
		
		while (true) {
			for (size_t i = 0; i < COUNT_OF(m_pins); i++) {
				if (!used_gpios[i] && isPressed(m_pins[i])) {
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
				
				if (dwt_read_cycle_counter() - last_pressed[i].time < DEBOUNCE_CYCLES)
					continue;
				
				bool pressed = isPressed(m_pins[i]);
				if (last_pressed[i].state != pressed) {
				//	printf("%d - P%c%d = %d [delta=%d]\r\n", cnt, Gpio::bank2name(m_pins[i].port), Gpio::pin2id(m_pins[i].pin), pressed, dwt_read_cycle_counter() - last_pressed[i].time);
					
					m_usb.sendEvent(NOTE_OFFSET + i, 64, pressed);
					
					if (pressed) {
						cnt++;
						
						if (pressed == 1)
							gpio_clear(GPIOC, GPIO13);
					} else {
						cnt--;
						
						if (!pressed)
							gpio_set(GPIOC, GPIO13);
					}
					
					last_pressed[i].time = dwt_read_cycle_counter();
					last_pressed[i].state = pressed;
					cnt++;
				}
			}
		}
	}
	
	return 0;
}
