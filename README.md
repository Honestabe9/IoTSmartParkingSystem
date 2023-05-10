# IoT: Smart Parking System


---

## Table of Contents

- [Idea Summary](#idea-summary)
- [Tools and Technologies](#tools-and-technologies)
- [How To](#how-to)
- [Additional References](#additional-references)
- [License](#license)
- [Authors Info](#authors-info)

---


### Idea Summary

This project utilizes mulitple Heltec LoRa development boards and a GPS module<sup>1</sup> to develop a smart parking system for cities, driven by the need to address parking scarcity in busy areas. The increasing adoption of smart parking systems is expected in the near future<sup>2</sup>. In an ideal world, this approach would enable users to exchange information on parking spot availability through the MQTT network protocol (TCP/IP under the hood), resulting in reduced driving time and decreased accident risks. In our use case, the MQTT publish-subcribe paradigm comprises of a Broker, Device Under Test (DUT), and Inspector, where the Broker serves as the access point, the DUT stores the data, and the Inspector manages button input and coordinates. The overall notion is to allow them to send (publish) and receive (subscribe) a topic (category) saved as a UTF-8 string that can provide geographical details setting the spots in a lot, and  which parking spots are taken and which are available: all with button-presses. 

If you want an overview of this, check out the [DemoSlides](https://github.com/Honestabe9/IoTSmartParkingSystem/blob/main/Group8Final/DemoSlides/IoT%20Smart%20Parking%20System%20-%20Demo.pdf).

---

### Tools and Technologies

#### Technologies
Install the PlatformIO IDE Extension and follow the instructions for: 

* VS Code → https://platformio.org/install/ide?install=vscode;

* Atom → https://docs.platformio.org/en/latest/integration/ide/atom.html#installation.


#### Tools
Keep the following libraries from [Mikal Hart](https://github.com/mikalhart):

* TinyMQTT → https://github.com/hsaturn/TinyMqtt;

* TinyGPSPlus → https://github.com/mikalhart/TinyGPSPlus;

* SoftwareSerial Library → https://docs.arduino.cc/learn/built-in-libraries/software-serial.

---

### How To

Once your technologies and tools set up, you'll need to create a new project in PlatformIO. Give it a name, select the board as "Heltec WiFi LoRa 32 (V3)", keep the Arduino Framework, and your customary file location. 

From then on a bunch of stuff in the environment will load in and be generated. In one of the files called "platform.ini"—which runs on the ESP32 platform—ensure that the default `monitor_speed` of 9600 is equal 115200 which is the fastest supported baud rate by Software Serial as of this post. Then, add this necessary library dependency, if it isn't already there, with this:

```C
lib_deps =
  https://github.com/hsaturn/TinyMqtt.git#0.9.12
  hsaturn/TinyConsole@0.4.2
```

As this is finished, attach the antenna of your LoRa devices, configure them to be connected to your computing machines (mainly PCs/laptops) with your USB (Type-C) cable of choice, and be within range of an accessible MQTT server.

Hook up the main GT-U7 module to its receiver and wire that to the Heltec LoRa according to the correct pins from the rough wiring diagram (in the demo slides). You can reference Heltec Automation's [pin map](https://github.com/Honestabe9/IoTSmartParkingSystem/blob/main/Group8Final/FinalWiFi/HTIT-WB32LA(F)_V3.png) and [schematic](https://github.com/Honestabe9/IoTSmartParkingSystem/blob/main/Group8Final/FinalWiFi/HTIT-WB32LA(F)_V3_Schematic_Diagram.pdf) of the device to double-check connections.

Afterward, once you get all of that situated, get build and upload your code onto the LoRa. Arrange your port and serial monitor to work accordingly, and get running!

> Side Note - Other than that use the `PRG` button to program which ought to gather the dimensions (hardcoded or not) for parking spots and/or determine if they are occupied/unoccupied in the lot of choice. Hit `RST` to reset and re-run/refresh the monitor from whatever has been collected.

---

### Additional References

1. [91tuvtrO2jL.pdf](https://github.com/Honestabe9/IoTSmartParkingSystem/files/11425854/91tuvtrO2jL.pdf)
2. Chang, B. (2022, March 1). Smart parking system using IoT - Mokosmart.com. MOKOSmart #1 Smart Device Solution in China. https://www.mokosmart.com/smart-parking-system-using-iot/



---

### License
[MIT License](https://github.com/Honestabe9/IoTSmartParkingSystem/blob/main/LICENSE)






---

### Authors Info

- UVA Students: Abel Ressom, James Zhu, and Josh Lee
