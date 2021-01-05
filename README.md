# Simple Arduino Chess Clock

When I thought of doing a Chess Clock with Arduino, the goal was to build using simple programming without use class and work with the AVR register. The base used was [Arduino Reference](https://www.arduino.cc/reference/en/) The most difficult thing was to manipulate the timer using only the Arduino millis (). The idea is the project can be useful by the beginner students of the Arduino.

Video Demostration: [https://www.youtube.com/watch?v=boUblF9qTB8](https://www.youtube.com/watch?v=boUblF9qTB8)

<img src="https://github.com/naldin/ArduinoChessClock/raw/master/IMG_0230.JPG" />

## Features

* Complete timer adjust using keypad to hour, minutes and seconds, of 1 sec up to 10 hours
* Storage last adjust in eeprom
* Time control by Sudden Death or Increment of up to 99 seg
* Pause timer using any keypad key and release using game buttoon
* Sound to check that game button was pressed and when game is over

## Parts

* Arduino Uno or any other
* 2 Push button R13-502
* Buzzer
* For LCD shield board, can be used [LCD Keypad Shield](https://www.dfrobot.com/wiki/index.php/Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009)) or DIY using:
  * LCD 16x2
  * 6 Tactile Push Button Switch
  * Universal Circuit Board
  * Row Pin Header

## Circuit

The most complicated circuit is the LCD Keypad Shield, if this piece is bought ready the rest is so simple.

<img src="https://github.com/naldin/ArduinoChessClock/raw/master/schematics.png" />
<img src="https://github.com/naldin/ArduinoChessClock/raw/master/schematic_Keypad_Shield.png" />

## 3D parts

Arduino Stand Definitive Edition for Arduino Pro Mini: [https://www.thingiverse.com/thing:4710255](https://www.thingiverse.com/thing:4710255)

<img src="https://github.com/naldin/ArduinoChessClock/raw/master/IMG_2802.JPG" />
<img src="https://github.com/naldin/ArduinoChessClock/raw/master/IMG_2802.JPG" />
<img src="https://github.com/naldin/ArduinoChessClock/raw/master/IMG_2802.JPG" />
<img src="https://github.com/naldin/ArduinoChessClock/raw/master/IMG_2802.JPG" />

Arduino Stand for Arduino Uno: [https://www.thingiverse.com/thing:3043109](https://www.thingiverse.com/thing:3043109)

Bauhaus Chess Set: [https://www.thingiverse.com/thing:2161468](https://www.thingiverse.com/thing:2161468)

<img src="https://github.com/naldin/ArduinoChessClock/raw/master/IMG_0231.JPG" />
<img src="https://github.com/naldin/ArduinoChessClock/raw/master/IMG_0233.JPG" />
<img src="https://github.com/naldin/ArduinoChessClock/raw/master/IMG_0229.JPG" />
