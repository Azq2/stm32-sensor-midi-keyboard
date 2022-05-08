#include "UsbMidi.h"

#include "utils.h"

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/audio.h>
#include <libopencm3/usb/midi.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/desig.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

UsbMidi *UsbMidi::m_instance = nullptr;

const struct usb_device_descriptor UsbMidi::m_dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,    /* was 0x0110 in Table B-1 example descriptor */
	.bDeviceClass = 0,   /* device defined at interface level */
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x16c0,
	.idProduct = 0x5e4,
	.bcdDevice = 0x0100,
	.iManufacturer = 1,  /* index to string desc */
	.iProduct = 2,       /* index to string desc */
	.iSerialNumber = 3,  /* index to string desc */
	.bNumConfigurations = 1,
};

const uint8_t UsbMidi::m_sysex_identity[] = {
	0x04,	/* USB Framing (3 byte SysEx) */
	0xf0,	/* SysEx start */
	0x7e,	/* non-realtime */
	0x00,	/* Channel 0 */
	0x04,	/* USB Framing (3 byte SysEx) */
	0x7d,	/* Educational/prototype manufacturer ID */
	0x66,	/* Family code (byte 1) */
	0x66,	/* Family code (byte 2) */
	0x04,	/* USB Framing (3 byte SysEx) */
	0x51,	/* Model number (byte 1) */
	0x19,	/* Model number (byte 2) */
	0x00,	/* Version number (byte 1) */
	0x04,	/* USB Framing (3 byte SysEx) */
	0x00,	/* Version number (byte 2) */
	0x01,	/* Version number (byte 3) */
	0x00,	/* Version number (byte 4) */
	0x05,	/* USB Framing (1 byte SysEx) */
	0xf7,	/* SysEx end */
	0x00,	/* Padding */
	0x00,	/* Padding */
};

char UsbMidi::m_serial_number[25] = {};
const char *UsbMidi::m_usb_strings[] = { "libopencm3.org", "MIDI demo", UsbMidi::m_serial_number };

void UsbMidi::init() {
	desig_get_unique_id_as_string(m_serial_number, sizeof(m_serial_number));
	
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO12 | GPIO11);
	
	triggerRescanUsb();
	
	m_usbd = usbd_init(&st_usbfs_v1_usb_driver, &m_dev, &m_config, m_usb_strings, 3, m_usbd_control_buffer, sizeof(m_usbd_control_buffer));
	usbd_register_set_config_callback(m_usbd, usbSetConfigCallback);
}

void UsbMidi::usbSetConfigCallback(usbd_device *usbd_dev, uint16_t wValue) {
	(void) wValue;
	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, usbDataRxCallback);
	usbd_ep_setup(usbd_dev, 0x81, USB_ENDPOINT_ATTR_BULK, 64, NULL);
}

void UsbMidi::usbDataRxCallback(usbd_device *usbd_dev, uint8_t ep) {
	(void) ep;
	
	char buf[64];
	int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);
	if (len) {
		while (usbd_ep_write_packet(usbd_dev, 0x81, m_sysex_identity, sizeof(m_sysex_identity)) == 0);
	}
}

void UsbMidi::sendEvent(char note, char velocity, bool pressed) {
	char buf[4] = {
		(char) (pressed ? 0x09 : 0x08),
		(char) (pressed ? 0x90 : 0x80),
		note,
		velocity
	};
	while (usbd_ep_write_packet(m_usbd, 0x81, buf, sizeof(buf)) == 0);
}

void UsbMidi::poll() {
	usbd_poll(m_usbd);
}

void UsbMidi::triggerRescanUsb() {
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
	gpio_clear(GPIOA, GPIO12);
	delay_ms(2);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO12);
}
