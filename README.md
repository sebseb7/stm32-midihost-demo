stm32-midihost-demo
===================

this demoes how to use midi host on stm32f4 mcus.

In this demo I use the stm32f4-discovery board with a KORG nanoKONTROL2 connected using an USB OTG adapter.

Note: before you can control the LEDs of the nanoKONTROL2 with midi messages you have to change "LED MODE" to "external" usign the KORG Kontrol Editor. (controlling the LEDs does NOT work on the older nanoKONTROL, only the newer nanoKONTROL2 has this function)

this demo flashes the upper left LED of the nanoKONTROL2 an echoes back all midi messages (which toggles the LEDs under the buttons on the nanoKONTROL2)
