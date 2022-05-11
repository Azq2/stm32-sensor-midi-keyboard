# stm32-sensor-midi-keyboard

# How that's works???

I don't know, really...

# Pinout

|Pin|Note|
|---|---|
|PB13|C3|
|PB14|C3#|
|PB15|D3|
|PB12|D3#|
|PA8|E3|
|PB8|F3|
|PB9|F3#|
|PB7|G3|
|PB3|G3#|
|PA1|A3|
|PB1|A3#|
|PA0|B3|
|PB4|C4|
|PB0|C4#|
|PA7|D4|
|PA2|D4#|
|PA4|E4|
|PB5|F4|
|PB11|F4#|
|PA5|G4|
|PA15|G4#|
|PA3|A4|
|PA6|A4#|
|PB6|B4|
|PB10|C4|

But you can connect key in any random order and just generate new `App::m_pins` using `DEBUG_SORT_KEYS=true` mode.

That's exactly what I did 🤦‍♂️
