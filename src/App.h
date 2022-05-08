#pragma once

#include <cstdio>

#include "UsbMidi.h"

class App {
	protected:
		// Config here
		constexpr static bool DEBUG_KEYS = false; // true for key press debug
		constexpr static char NOTE_OFFSET = 48; // C2
		constexpr static uint32_t DEBOUNCE_CYCLES = 5000000; // cpu cycles for debounce keys
		constexpr static float SENSOR_KEY_THRESHOLD = 1; // Threshold in % for sensor key "pressed" state
	public:
		struct Pin {
			uint32_t port;
			uint32_t pin;
			int calibrate = 0;
		};

		enum PinMode {
			MODE_INPUT_UP	= 0,
			MODE_INPUT_DOWN	= 1,
			MODE_UP			= 2,
			MODE_DOWN		= 3,
		};
	
	protected:
		static Pin m_pins[];
		UsbMidi m_usb = {};
		
		void initHw();
		void initSensors();
		void setPinMode(const Pin &p, PinMode mode);
		int readPulses(const Pin &p);
		bool isPressed(const Pin &p);
		void calibrateSensors();
	public:
		int run();
};
