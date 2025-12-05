# Cooperative Thermal Alert Network (IoT)

### B31OT – Internet of Things

**ESP32 | ESP-NOW | MQTT | Node-RED | Docker**

## Overview

The **Cooperative Thermal Alert Network** is an energy-efficient wireless monitoring system built using multiple ESP32 sensor nodes operating in a **Gateway-Centric Star Topology**. Each node gathers temperature and humidity data, which is transmitted to a central gateway using the low-power **ESP-NOW** protocol. The gateway evaluates the readings, determines whether alerts must be triggered, sends LED color commands back to the nodes, and forwards all data to the cloud using **MQTT**.

Real-time visualization and historical data storage are provided through **Node-RED** and **Docker**.

---

## Hardware Used

* ESP32 WROOM (×3)
* DHT11 Temperature & Humidity Sensor
* RGB NeoPixel LEDs (WS2812)
* USB/Micro-USB power supply

---

## Technologies

* **Local Communication:** ESP-NOW
* **Cloud Uplink:** MQTT (HiveMQ Cloud)
* **Dashboard:** Node-RED
* **Storage:** Docker containers

---

## Key Features

### Centralized Gateway Logic

* Sensor nodes only measure and send environmental readings.
* Threshold checks and alert decisions occur on the gateway.
* Gateway broadcasts LED instructions (RED / BLUE / GREEN) to all nodes.

### Energy-Efficient Sensor Nodes

* Nodes do not enable Wi-Fi (≈150 mA).
* Communication uses ESP-NOW (~40 mA for milliseconds).
* Nodes enter deep-sleep after transmission for optimal battery life.

### Cooperative LED Alerts

The gateway enforces network-wide visual status:

* Temperature **> 28°C** → LEDs **RED**
* Temperature **< 18°C** → LEDs **BLUE**
* Otherwise → LEDs **GREEN**

### MQTT Cloud Integration

* Gateway publishes sensor readings as JSON messages.
* Node-RED subscribes to MQTT topics for real-time visualization.

### Dockerized Data Storage

* Historical sensor data stored in Docker containers.
* Easy to deploy, persistent, and portable.

---

## System Architecture

### 1. Data Acquisition (Sensor Nodes)

* Node wakes up.
* Reads DHT11 values (temperature & humidity).
* Sends raw data via ESP-NOW to the gateway.
* Returns to deep sleep.

### 2. ESP-NOW Transmission

* Lightweight, low-power communication.
* No Wi-Fi connection required.
* Sends data packets: `(Temperature, Humidity, NodeID)`.

### 3. Gateway Logic

Upon receiving node data:

1. Parses sensor values.
2. Applies threshold rules:

   * `> 28°C` → LED_RED
   * `< 18°C` → LED_BLUE
   * else → LED_GREEN
3. Broadcasts LED command to all nodes via ESP-NOW.
4. Sends reading to the cloud through MQTT using JSON encoding.

### 4. Cloud Uplink (MQTT)

Example JSON payload:

```json
{
  "node": "NodeA",
  "temperature": 29.4,
  "humidity": 62,
  "alert": "RED"
}
```

### 5. Visualization (Node-RED)

Node-RED dashboards include:

* Live sensor readings
* LED status indicators
* Real-time temperature/humidity charts
* Alert history
* Network status panels

---

## Workflow Summary

1. Node wakes → reads DHT11
2. Sends data to gateway using ESP-NOW
3. Gateway evaluates thresholds
4. Gateway sends LED color instructions
5. Nodes update LEDs and sleep
6. Gateway uploads data via MQTT
7. Node-RED displays everything in real time

---

## Team Members

* V Anantha Shankar
* Sarjjana Venkataraman
* Chinyere Uwa
* Axel Biegalski

---

## Future Improvements

In the system we have, a node identifies a high temperature and sends a red LED message, and the Gateway instantly acts upon this message and sends another message to the rest of the nodes to change their colour of LED too. Since not all nodes wake up after the same time the ESP-NOW messages are not always instantaneous; when the other nodes update their colour a few seconds later. With this short delay, some packets can be lost but this does not have an impact on the speed with which the LED status changes. Notably, it does not have any impact on the real sensor readings, all the information is still recorded as designed and stored within our Docker-based system via the MQTT connection of our Gateway.

In the future we can be able to make the system more responsive by including a simple confirmation or retransmission mechanism to ensure that the Gateway is aware that every node was able to receive the LED update. This would assist the network to remain in full sync in case some of the messages are lost. The other thing that would be helpful in improving this was synchronization of the nodes or introduction of minor reliability improvements, like the presence of a check before resuming the sleep of new instructions. Such modifications would make the entire system more real-time, as well as help minimize delay and enhance stability.


## OUTPUT

![IoT img1](https://github.com/user-attachments/assets/8ec7df37-28af-43a9-be82-d5f6e5c95281)




