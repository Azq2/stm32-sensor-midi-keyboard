# stm32-sensor-midi-keyboard

# How that works? 🤔

All piano keys is a capacitive touch sensors.
The microcontroller continuously measure needed time for change pin state from 0 to 1 and and back
That's have little lag, because any gpio pin have parasitic capacitance.
And when finger touches pin there is a current leak happens and time to charge/discharge changes.

The algorithm is simple:
1. Change pin to input with internal pull-up
2. Measure time until pin have `1` state
3. Change pin to output=1 for complete charge parasitic capacitor
4. Change pin to input with internal pull-down
5. Measure time until pin have `0` state
6. Change pin to output=0 for complete discharge parasitic capacitor

Any deviations in these measured times is meaning for key press.

# Shematic

![Schematic](docs/schematic.png)

Current pinout which use in the code:

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
