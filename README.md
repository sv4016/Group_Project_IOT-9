**Cooperative Thermal Alert Network (IoT)**

**Course: B31OT - Internet of Things**

Hardware: ESP32 WROOM (x3), DHT11, RGB LEDs (NeoPixel)

Protocol: ESP-NOW (Local) & MQTT (Cloud)

Backend: Node-RED (Visualization) & Docker (Storage)

**Team Members:**
V Anantha Shankar, SARJJANA Venkataraman, Chinyere Uwa, Axel Biegalski

**Overview**

This project deploys a wireless sensor network based on energy-efficient monitoring of the environmental conditions through a Gateway-Centric Star Topology.

In contrast to a mesh network where nodes communicate to one another our Central Gateway is the smart coordinator. It gathers information of the local nodes through ESP-NOW, ascertains whether an alert is necessary and sends instruction back to the nodes to activate their LEDs.

More importantly, the Gateway is the only one that is linked to the Internet. It connects the local low-power network to the HiveMQ Cloud, where the information is visualised and stored with the help of Node-RED and Docker.


**Key Features**

Centralized Control: There is logic centralized in the Gateway. When the heat at Node A is received to be high, the Gateway receives this and orders Node B (and Node A) to become Red.

Optimal Energy Saving: Nodes do not switch on Wi-Fi (consumes approximately 150mA). They rely on ESP-NOW (~40mA) only during milliseconds and then fall asleep which increases battery life considerably.

Dockerized Storage: The data storage is managed through Docker containers such that historical sensor data is stored in the safest way possible.

Node-RED Visualization: Node-RED is a dashboard application that subscribes to the MQTT messages being sent by the Gateway to update live gauges and status. In this project, Node-RED is also deployed on a local docker container and bind MQTT broker to plot real time charts and create management/monitoring dashboards.

**System Architecture**

Data Acquisition: The Nodes are woken up and they read temperature/humidity of the DHT11 sensor.

Transmission (ESP-NOW): The raw data (Temp, Humidity, NodeID) are transmitted instantly by the Nodes to the Gateway with low power ESP-NOW protocol. There is no check of threshold at nodes.

**Gateway Logic:**

It gets to the Gateway where the packet is received.

It checks the value:

And when temperature is > 28degC: Gateway sends an ESP-NOW command: "Set LEDs RED".

In case of less than 18degC: Gateway sends ESP-NOW command: " Set LEDs BLUE".

Else: Gateway broadcasts: "LEDs GREEN Set.

This command is sent to nodes and they change their visual status.

Cloud Uplink (MQTT): The Gateway encodes the sensor data in the form of JSON and posts it to HiveMQ Cloud through Wi-Fi.

