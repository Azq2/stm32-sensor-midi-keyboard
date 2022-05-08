PROJECT = app
BUILD_DIR = bin

INCLUDES += -Isrc

CXXFILES += src/main.cpp
CXXFILES += src/utils.cpp
CXXFILES += src/App.cpp
CXXFILES += src/UsbMidi.cpp
CXXFILES += src/UsbMidiDescr.cpp

DEVICE=stm32f103c8t6
OOCD_FILE = board/stm32f1discovery.cfg

OPENCM3_DIR=libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

ifeq ($(BMP_PORT),)
	BMP_PORT_CANDIDATES := $(wildcard /dev/serial/by-id/usb-Black_Sphere_Technologies_Black_Magic_Probe_*-if00)
	ifeq ($(words $(BMP_PORT_CANDIDATES)),1)
		BMP_PORT := $(BMP_PORT_CANDIDATES)
	else
		BMP_PORT = $(error Black Magic Probe gdb serial port not found, please provide the device name via the BMP_PORT variable parameter$(if $(BMP_PORT_CANDIDATES), (found $(BMP_PORT_CANDIDATES))))
	endif
endif

ifeq ($(SERIAL_PORT),)
	SERIAL_PORT_CANDIDATES := $(wildcard /dev/serial/by-id/usb-Black_Sphere_Technologies_Black_Magic_Probe_*-if02)
	ifeq ($(words $(SERIAL_PORT_CANDIDATES)),1)
		SERIAL_PORT := $(SERIAL_PORT_CANDIDATES)
	else
		SERIAL_PORT = $(error Black Magic Probe UART serial port not found, please provide the device name via the SERIAL_PORT variable parameter$(if $(SERIAL_PORT_CANDIDATES), (found $(SERIAL_PORT_CANDIDATES))))
	endif
endif

install:
	@echo -n "RAM: "; size "$(PROJECT).elf" | tail -1 | awk '{print $$2 + $$3}'
	@echo -n "ROM: "; size "$(PROJECT).elf" | tail -1 | awk '{print $$1}'

	@printf "INSTALL $(BMP_PORT) $(PROJECT).elf (flash)\n"
	
	$(GDB) -nx --batch \
		-ex 'target extended-remote $(BMP_PORT)' \
		-x black_magic_probe_flash.scr \
		$(PROJECT).elf

picocom:
	picocom -b115200 "$(SERIAL_PORT)"
