# Aurora Ring

[![Star this project](http://githubbadges.com/star.svg?user=kulaone&repo=aurora_ring&style=flat&color=fff&background=007ec)](https://github.com/kulaone/aurora_ring)
> A LED ring alerts system of when the aurora (northern lights) might be visible from the UK

This project is base on API by [AuroraWatch UK][1]

## Hardware:
1. [WeMos D1 Mini][2] — A tiny ESP8266-based board that run Arduino code, and has built-in WiFi support
2. A RGB LED Ring, I recommend using the [24 NeoPixel LED][3]

## Software:
1. [WebSockets][4] - WebSocket Server and Client for Arduino (v2.0.2 or newer)
2. [ADAFRUIT_NeoPixel][5] - Controlling single-wire-based LED pixels
3. [SimpleTimer][6] - A simple library to launch timed actions

## Installation

### Arduino Environment Configuration
Make sure you have Arduino 1.6.7 or newer. You can download the latest version from [www.arduino.cc][7].
You will need to set up your Arduino environment for ESP8266 development, I use the [AdaFruit tutorial][8].
From the Arduino IDE, install the WebSockets and AdaFruit NeoPixel libraries (*Sketch> Include Library> Manage Libraries*)

### Connecting the NeoPixel Ring to the WeMos D1 Mini
Connect the NeoPixel Ring to your WeMos D1 Mini as follows:
* NeoPixel VCC ↔ WeMos 5V
* NeoPixel GND ↔ WeMos G
* NeoPixel IN / DI ↔ WeMos D2 pin

### Setting up the code
Update aurara_ring.ino with your Wi-Fi network id and password.
If you are not using ring with 24 LEDs, update NUM_PIXELS with the number of LEDs
```c++
const char* networks[][2] = {
        { "ssid", "password"}
    };

const int NUM_PIXELS 24
```
If you need to set more the one network, set it like this:
```c++
 const char* networks[][2] = {
         { "ssid1", "password1"},
         { "ssid2", "password2"},
         { "ssid3", "password3" }
     };
```

### Testing the hardware
For testing the ring, change *testRingMode* to true.
```c++
const boolean testRingMode = true;
```

The projecr on [instructables][10]

For this project I was inspired by Uri Shaked article ["How to Connect Your T-Shirt to Slack Using Arduino"][9]

[1]:http://aurorawatch.lancs.ac.uk/
[2]:http://www.wemos.cc/Products/d1_mini.html
[3]:http://amzn.to/2hyJPYR
[4]:https://github.com/Links2004/arduinoWebSockets
[5]:https://github.com/adafruit/Adafruit_NeoPixel
[6]:http://playground.arduino.cc/Code/SimpleTimer
[7]:https://www.arduino.cc/en/Main/Software
[8]:https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide#install-the-esp8266-board-package
[9]:https://medium.com/@urish/how-to-connect-your-t-shirt-to-slack-using-arduino-90761201d70f#.7f5ov1hdz
[10]:http://www.instructables.com/id/Aurora-Alerts-Ring/
