# Smart Actuation and Sensor Monitoring

Embedded systems work completed during my internship at CSIR-AMPRI, focusing on sensor integration, real-time monitoring, wireless communication and hardware development using the ESP32-S3 XIAO.

## Overview

The work explored an ESP32-based sensing and monitoring platform using a ThinPot displacement sensor, MQTT communication, a browser-based dashboard and a custom PCB. Additional experiments involved load cell calibration, spring characterisation and data acquisition from a soft robotic gripper.

## Work Areas

### Displacement Sensing and Calibration

Integrated a ThinPot linear potentiometer with the ESP32-S3 XIAO to measure displacement through analog voltage readings. Collected reference measurements and developed a voltage-to-displacement calibration model.

A laser displacement sensor was used to validate ThinPot readings and examine measurement deviations.

### MQTT Communication and Monitoring

Developed an MQTT-based communication workflow using the ESP32, a local broker and a browser-based interface.

The system supported sensor data monitoring and remote control commands, including PWM control of GPIO outputs. Python was used for data logging and visualisation.

### Load Cell and Spring Characterisation

Conducted separate experiments involving load cell voltage calibration and spring force-displacement measurements. Applied Hooke's law to estimate the spring constant.

An LVDT and a Zaber actuator were also used in a separate spring displacement profiling experiment.

### PCB Design and Hardware Integration

Designed a custom PCB using EasyEDA Pro to organise the ESP32-based sensor interface, power connections and peripheral wiring. The board was fabricated and soldered as part of the hardware development process.

### Soft Robotic Gripper Data Acquisition

Collected and analysed deformation and displacement data from a soft robotic gripper to understand its response under different operating conditions. The control strategies used to operate the gripper were external to this work.

## Technical Tools

- ESP32-S3 XIAO
- Embedded C/C++ and Arduino framework
- MQTT and PubSubClient
- HTML, JavaScript and MQTT.js
- Python for data analysis and visualisation
- EasyEDA Pro
- ThinPot, load cell, LVDT and laser displacement sensing
- Ubuntu Linux

## Outcome

The work covered sensor interfacing, displacement calibration, MQTT-based monitoring, PWM control, PCB development and experimental data analysis. It provided practical experience in embedded system design and hardware-software integration.

