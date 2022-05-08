#pragma once

#include <cstdio>

#include <libopencm3/usb/usbd.h>

class UsbMidi {
	protected:
		UsbMidi(const UsbMidi &);
		void operator=(const UsbMidi &);
		
		static UsbMidi *m_instance;
		
		usbd_device *m_usbd = nullptr;
		
		static const struct usb_device_descriptor m_dev;
		static const struct usb_config_descriptor m_config;
		static const uint8_t m_sysex_identity[];
		
		static char m_serial_number[25];
		static const char *m_usb_strings[];
		uint8_t m_usbd_control_buffer[128];
		
		void triggerRescanUsb();
		
		static void usbSetConfigCallback(usbd_device *usbd_dev, uint16_t wValue);
		static void usbDataRxCallback(usbd_device *usbd_dev, uint8_t ep);
	public:
		UsbMidi() {
			m_instance = this;
		}
		
		~UsbMidi() {
			m_instance = nullptr;
		}
		
		void init();
		void poll();
		void sendEvent(char note, char velocity, bool pressed);
};
