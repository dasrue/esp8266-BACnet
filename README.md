# ESP8266 BACnet

ESP8266 BACnet is a port of the open-source [BACnet Stack] by Steve Karg to the [ESP8266] WiFi microcontroller by [Espressif Systems].
These microcontrollers are very inexpensive, designed for IoT and home automation uses. This port brings the [BACnet] commercial building automation and control protocol to the ESP8266, allowing for cheap, easily deployable building automation devices and sensors.

# Working Features
- Device discovery (receive whois, send iam)
- Read property
- Write property
- Binary output object
- Storing and retreiving WiFi and BACnet information in the Flash memory
- Interfacing with hardware
    - Analogue inputs to HTU21D Temp/Humidity sensor

# Features Still in Progress
- Change of value subscribe and notify
- Webserver/App interface for setting up WiFi settings initially
- Get proper broadcast address (eg 192.168.1.255 instead of 255.255.255.255)
- Interfacing with hardware
    - Binary inputs and outputs to GPIO
    - Analogue inputs to sensors such as DHT22
    - Analogue input to ADC input
    - Analogue outputs to PWM output
- Extra BACnet objects
    - Network (wifi configuration)

# How to Build
Step 1:	Install the [esp-open-sdk]. Instructions are avaliable on the github page

Step 2:	Clone this repository.

Step 3:	Edit the makefile to reflect your ESP8266 SDK install location.

Step 4:	Run Make to make the current test application (as of this commit it is a HTU21D temperature/humidity sensor monitor)

Step 5: Run "Make Flash" to flash the firmware to a connected ESP8266, using the default serial port (/dev/ttyUSB0 on Ubuntu)


License
----
[BACnet Stack] original and modified files:     eCos (GPL-like) license

Files created by me, for use in this port:      MIT license

In general, the files within the src/, include/, and Demos/ directories are licensed under MIT. Files within the BACnet-Stack/ directory are under the eCos license


[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)

   [BACnet Stack]: <http://bacnet.sourceforge.net/>
   [Espressif Systems]: <https://espressif.com/>
   [ESP8266]: <https://en.wikipedia.org/wiki/ESP8266>
   [BACnet]: <http://www.bacnet.org/>
   [esp-open-sdk]: <https://github.com/pfalcon/esp-open-sdk>
