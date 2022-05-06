#!/bin/bash

# Force enable LTO for prevent perfomance loss because libopencm3 authors dont use inline functions for own strange reasons.
LTO_PLUGIN=$(arm-none-eabi-gcc --print-file-name=liblto_plugin.so)

make -C libopencm3/ TARGETS=stm32/f1 \
	CFLAGS="-flto" \
	LDFLAGS="-flto" \
	ARFLAGS="rcs --plugin '$LTO_PLUGIN'" $@
