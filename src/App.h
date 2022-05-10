#pragma once

#include <cstdio>

#include "UsbMidi.h"

class App {
	protected:
		// Config here
		constexpr static char NOTE_OFFSET					= 48; // C3
		constexpr static uint32_t DEBOUNCE_CYCLES			= 1000000;
		constexpr static bool DEBUG_SORT_KEYS				= false;
		constexpr static bool DEBUG_PRESS_KEYS				= false;
		constexpr static float SENSOR_WHITE_KEY_THRESHOLD	= 1000;
		constexpr static float SENSOR_BLACK_KEY_THRESHOLD	= 1300;
		constexpr static int CALIBRATION_CYCLES				= 100;
		constexpr static int SAMPLES_CNT					= 8;
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
		int readPulses(const Pin &p);
		int readSensorValue(const Pin &p);
		
		inline bool isBlackKey(int i) {
			static const bool values[] = {
				false, true, false, true, false,
				false, true, false, true, false, true, false,
			};
			return values[(i % 12)];
		}
		
		inline bool isPressed(int key, int value = -1) {
			if (isBlackKey(key)) {
				return value > SENSOR_BLACK_KEY_THRESHOLD;
			} else {
				return value > SENSOR_WHITE_KEY_THRESHOLD;
			}
		}
		
		void calibrateSensors();
	public:
		int run();
};
