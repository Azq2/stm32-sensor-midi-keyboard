#pragma once

#include <cstdio>

#include "UsbMidi.h"

class App {
	// Config here
	protected:
		// Debug mode
		constexpr static bool DEBUG_SORT_KEYS				= false; // for generate new m_pins struct
		constexpr static bool DEBUG_PRESS_KEYS				= false; // for debug key pressing (to show threshold and time delta)
		
		// MIDI settings
		constexpr static char NOTE_OFFSET					= 48; // note for first key (C3)
		constexpr static char KEY_VELOCITY					= 64; // normal velocity
		
		// Key scanning
		constexpr static uint32_t DEBOUNCE_CYCLES			= 1000000;
		constexpr static float SENSOR_WHITE_KEY_THRESHOLD	= 1000; // threshold for white keys (0 - 100_00)
		constexpr static float SENSOR_BLACK_KEY_THRESHOLD	= 1300; // threshold for black keys (0 - 100_00)
		constexpr static int CALIBRATION_CYCLES				= 100;
		constexpr static int SAMPLES_CNT					= 8;
	public:
		struct Pin {
			uint32_t port;
			uint32_t pin;
			int calibrate = 0;
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
