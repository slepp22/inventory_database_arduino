# Inventory Database Part : Arduino
<img src="https://github.com/slepp22/inventory_database_arduino/blob/main/logo.png?raw=true" alt="Logo" width="30%" />


This project is a collaboration between Centria and Hochschule Darmstadt to develop a Locker/Renting/Charging System for universities. This repository contains the Arduino part, responsible for controlling the lockers. The Arduino communic


## Introduction

This project is a collaboration between Centria and Hochschule Darmstadt to develop 
a Locker/Renting/Charging System for universities. 
This repository contains the Arduino part, responsible for controlling the lockers.
The Arduino communicates with the backend via FastAPI, which in turn interacts
with a frontend for booking purposes.

The other repository can be found here :
-  Frontend: https://github.com/kekkeller/inventory_database_frontend/
-  Backend: https://github.com/slepp22/inventory_database_backend



## Features

- Pin Pad for access control
- Servo Motor for opening lock
- Door sensor, to check door state
- Display LCD, 16x2, with backlight, to communicate with user
- Arduino Power Meter to see current wattage
- Data of Wattage is visualized with ThingSpeak https://thingspeak.com/channels/2530450
  <img src="https://de.mathworks.com/help/thingspeak/collectagriculturaldataoverthethingsnetworktsexample_11_de.png" align = right alt="Logo" width="20%" />
## Installation

1. Install Arduino IDE 
   - Version used in project 2.3.2 
   - Download Link : https://www.arduino.cc/en/software
2. Install libaries used in project
   - Servo by Michael Margolis, Arduino 1.2.1 (Servo Motor)
   - Grove - LCD RGB Backlight by Seeed Studio 1.0.0 (Display LCD)
   - ArduinoJson by Benoit Blanchon 7.0.4
   - WifiNINA by Arduino 1.8.14
   - Keypad by Mark Stanley, Alexander Brevig 3.1.1 (Keypad)
3. Setup Arduino (Arduino UNO WiFi Rev.2)
    //TODO add electronics here

## Usage
In the Wifi Section of the code you have to add your wifi and password, which is needed to connect to the backend, to get a pin code.
```python
//WIFI SECTION
#include <WiFiNINA.h>
char ssid[] = "LorimIpsum";
char wifi_password[] = "lockerHardware";
```

In the Current Sensor Section of the code you have to set the voltage value, which is provided in your country.
If you unsure about the voltage value you can check here : https://www.rei.com/learn/expert-advice/world-electricity-guide.html#:~:text=Plugs%20and%20Electrical%20Information
```python
//CURRENT SENSOR SECTION
int voltage = 220; //Voltage of Power Grid
```

<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQeIBbxDL8IrOPwuaB3jFH2bx_lcdh7UUzGndN6Kd6m&s" alt="Alt text" style="width: 300px;" align="left">
<!-- Insert a blank line here -->
<img src="https://www.tha.de/Binaries/Binary19462/Logo-Centria.webp" alt="Alt text" style="width: 300px;" align="center">

