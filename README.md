# Summary

This project is a write up of a gift from a friend for my birthday which consists of a GPS tracker
an a traffic light of LED's that change to green the closer to my house you get.

It uses an Atmega88-20pu and 16mhz crystal resulting in what I understand to be an Arduino on a breadboard
with a few minor differences due to the difference in chip specifications.

The key differences between the Atmega88 and atmega328 found on typical Arduino devices are;

* Lower Flash: 8192 vs 32768
* Less SRAM: 1024 vs 2048
* Smaller EEPROM: 512 vs 1024
* No temp or pico power options

Consult the respective datasheets for more detailed comparisons

## Bill of Materials

For this project you will require the following components, however you should be able to easily
replicate the design using a typical Arduino device without too much hassle.

* AtMega88-20PU DIP
* 1x 10uf Capacitor
* 2x 27pf Capacitors
* 1x 16mhz crystal
* 3x coloured LED's (Red, Yellow, Green)
* 3x 470ohm resistors
* 1x 10k ohm resitor
* 1x gy-gps6mv2 GPS module
* 2x AA batteries
* 1x 30x12 breadboard with power lines (Give or take)

# Build Instructions

## Flash the chip

To push the code onto the Atmega88 its easiest to use an Arduino in ISP (In System Programmer) mode.

Specific instructions are not provided as I haven't tried it yet - my mate did the initial programming.

## Schematic

<Image Goes here>

## Wire up the Atmega88


1. Place the atmega88 in the middle of your breadboard
2. Place the 16mhz crystal across XTL1 and XTL2
3. Place the 27pf capacitors from each leg of the crystal to ground
4. Place jumpers between each of the chips ground pins and the ground plane
6. Place the 10k resistor between the positive plane and the reset pin of the atmega88
7. Place the 10uf capacitor between the positive and ground planes - ensure you check the polarity
8. Place jumpers between vcc, vRef and AVcc
9. Place jumps from the positive plane to the vcc pin

This should leave your breadboard with an Arduino-ish clone with nothing really wired up to do anything
apart from start if you apply 3v to the power lines.

## Add the GPS and LED's

1. Add a 470ohm resitor and the green led between Digital output 5 and the ground pin
2. Add another 470ohm resitor and the yellow led between Digital output 6 and the ground pin
3. Add the last 470ohm resitor and the red led between Digital output 7 and the ground pin
4. Attach the GPS module to the breadboard and connect:
    1. GND to the ground plane
    2. The TX from the module to the RXD input on the Atmega88
    3. VCC to the power plane

