# IoT: Smart Parking System


---

## Table of Contents

- [Idea Summary](#idea-summary)
- [Tools and Technologies](#tools-and-technologies)
- [How To](#how-to)
- [Additional References](#additional-references)
- [Author Info](#author-info)

---


### Idea Summary

This project utilizes mulitple Heltec LoRa development boards and a GPS module<sup>1</sup> to develop a smart parking system for cities, driven by the need to address parking scarcity in busy areas. The increasing adoption of smart parking systems is expected in the near future<sup>2</sup>. In an ideal world, this approach would enable users to exchange information on parking spot availability through the MQTT network protocol (TCP/IP under the hood), resulting in reduced driving time and decreased accident risks. In our use case, the MQTT publish-subcribe paradigm comprises of a Broker, Device Under Test (DUT), and Inspector, where the Broker serves as the access point, the DUT stores the data, and the Inspector manages button input and coordinates. The overall notion is to allow them to send (publish) and receive (subscribe) a topic (category) saved as a UTF-8 string that can provide geographical details setting the spots in a lot, and  which parking spots are taken and which are available: all with button-presses.

---

### Tools and Technologies

#### Technologies
Install the PlatformIO IDE Extension and follow the instructions for: 

> VS Code → https://platformio.org/install/ide?install=vscode

> Atom → https://docs.platformio.org/en/latest/integration/ide/atom.html#installation


#### Tools
Using this

---

### How To

Once you have that set up, you'll need to create a new project in PlatformIO. Give it a name, select the board as "Heltec WiFi LoRa 32 (V3)", keep the Arduino Framework, and your customary file location. 

From there on a bunch of stuff in the environment will load in and be generated. In one of the files called "platform.ini" do the following:

- Ensure that the default `monitor_speed` of 9600 is equal 115200 which is the fastest supported baud rate by Software Serial as of this post.
- Add this necessary library dependency, if it isn't already there, with this:

```
lib_deps =
  https://github.com/hsaturn/TinyMqtt.git#0.9.12
  hsaturn/TinyConsole@0.4.2
```

- Attach the antenna of your LoRa devices, configure them to be connected to your computing machines (mainly PCs/laptops) with your USB (Type-C) cable of choice, and be within range of an MQTT server, 

---

### Additional References

1. [91tuvtrO2jL.pdf](https://github.com/Honestabe9/IoTSmartParkingSystem/files/11425854/91tuvtrO2jL.pdf)
2. Chang, B. (2022, March 1). Smart parking system using IoT - Mokosmart.com. MOKOSmart #1 Smart Device Solution in China. https://www.mokosmart.com/smart-parking-system-using-iot/



---

### Author Info

- UVA Students: Abel Ressom, James Zhu, and Josh Lee
