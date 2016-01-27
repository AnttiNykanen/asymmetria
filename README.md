# ESP8266-based LED dimmer #

This dimmer designed to fit inside a Lival Asymmetria desk lamp. The
plan is to use [LED strips like these][ledstrips] to replace the stock
fluorescent bulb, but other kinds of LEDs should work too, as long as
you adjust the shunt resistor accordingly and don't exceed the 3.3V
regulator's ratings.

![pcb photo](./doc/pcb/asymmetria_v1.0_photo_front_small.jpg)

## Contents of this directory ##

  * [hardware](./hardware/): Hardware design files
  * [firmware](./firmware/): Firmware source code
  * [doc](./doc/): Documentation
  
## License ##

  * Hardware:
    [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)
  * Firmware:
    * Files in the user subdirectory:
      [ISC](https://opensource.org/licenses/ISC)
    * Everything else: whatever the files say
 
## The Fine Print ##

Note that owning and/or using homemade radio transmitters, which this thing is, might not be legal for everyone everywhere.

[ledstrips]: http://www.aliexpress.com/item/Ultra-thin-5W-LED-COB-60-Chip-Led-Driving-DRL-Daytime-Running-Light-Lamp-Bar-Strip/32303226004.html
