# Energy meter - specifically for Solar Panels - for solarbildung.org
Current is sensed via transformer TA12-100 (100:1) and fed to ADC on an ESP32 board.
Sensor values are stored locally on LittleFS, and sent to cloud via Wifi and UMTS modem stick.

![PVmeter](https://github.com/hierle/pvmeter/blob/main/misc/pvmeter1.png?raw=true)<br>
![PVmeter](https://github.com/hierle/pvmeter/blob/main/misc/pvmeter2.png?raw=true)<br>

Contents:
- ./PCB/ :                  kicad PCB files
- ./cloud/ :                sample CGI script for data collection
- ./misc/ :                 images ...
- ./pvmeter/ :              pvmeter sketch (Arduino IDE)

Compiled and flashed with ArduinoIDE with ESP32 extensions


Features:
- using Lolin S2 mini based on ESP32S2
- transformer TA12-100 rated for 5A max, up to ~1kW
- real time clock DS3231 (I2C) ensures accurate timestamps for data logging
- TFT display via SPI can display status, current energy flow, ...
- USB UMTS modem stick provides reliable data upload even w/o Wifi avail
- USB command line interface for debugging and development

BOM (aliexpress):

| Count | Part number        | Description             | Usage                    |
|-------|--------------------|-------------------------|----------------------|
| 1     | Lolin-S2-miniNI    | CPU                     | CPU                  |
|       |                    |                         |                      |


S2 mini documentation:

https://www.wemos.cc/en/latest/s2/s2_mini.html


How to compile:

in short:<br>
- install ArduinoIDE from arduino.cc
- install the ESP32 Boards from Boards and Library Manager
- select "Lolin S2 mini" board
- on Mac you might need to install pyserial: sudo pip3 install pyserial
- on Linux you might need to install python3-pyserial
- connect the board
- select the corresponding serial port
- open the sketch folder
- adjust config.h and serialnr.h
- compile and flash


For configuration adjust config.h and serialnr.h

config.h:
- WIFI_SSID / WIFI_PASS: credentials for USB UMTS modem
- CLOUD_SERVER: server for data upload
- UPDATE_PERIOD: how often to update TFT display
- SENSOR_PERIOD: how often to measure and upload sensor data
- SENSOR_THRETHOLD: cut off noise below this value
- see also the comments for each entry in config.h

serialnr.h:
- SERIALNUMBER: serial number for specific device (who is sending the data?)

Additional notes:
- you will need a *DATA* USB-C cable, a charging only cable will not work, as it's missing the data pins ;-)


Todos:
- many :)


Have fun!

