### Smart Traffic and Congestion Control System (IoT)

##### Overview

Traffic congestion is a growing challenge in modern cities. Traditional traffic light systems rely on fixed timers that do not adapt to real-time conditions—leading to wasted time, fuel, and efficiency.

This project presents an IoT-based Smart Traffic Control System using an ESP32, ultrasonic sensors, and LED traffic lights. The system dynamically adjusts green light duration based on actual queue sizes, communicates through MQTT, and provides a real-time dashboard for monitoring.

##### Hardware & Software Components

- ESP32-WROOM-32S → Microcontroller for traffic logic + MQTT communication

- Ultrasonic Sensors (HC-SR04) → Detect vehicle entry & exit

- LEDs (Red, Yellow, Green) → Simulated traffic lights

- Breadboard + Jumper Wires → Circuit assembly

- Wi-Fi Network → For MQTT connectivity

- HiveMQ Public Broker → MQTT publish/subscribe communication

- Arduino IDE → For ESP32 code development

- HTML, JavaScript, Bootstrap, MQTT.js → Real-time dashboard

##### System Design

The project simulates a two-way traffic intersection:

- West → East (WE)
- East → West (EW)

Each direction has:

- 1 Entry Sensor
- 1 Exit Sensor
- 3 Traffic LEDs (Red, Yellow, Green)
- A Queue Counter (tracked in ESP32 memory)

System Workflow:

1. Sensors detect vehicles entering/exiting.
2. Queue size is updated in real-time.
3. ESP32 decides which direction gets green based on queue comparison.
4. Green duration = Base (5s) + (2s × Number of Cars).
5. Traffic light states + queue values are published via MQTT.
6. Dashboard updates live with traffic states.

##### How It Works

- Queue Detection

* Entry sensor detects approaching cars → queue increases.
* Exit sensor detects leaving cars → queue decreases.

- Adaptive Timing

* Green duration is calculated dynamically:
  Green Time = BASE_GREEN_DURATION + (Queue Size × TIME_PER_CAR)
* Base Duration = 5 seconds
* Time per Car = 2 seconds

- Traffic Light Logic

* Green → Yellow (2s) → Red → Switch direction
* Alternates if both queues are equal.

- MQTT Communication

* Publishes real-time states & queues to topics:
* - iot/traffic/WE/state
* - iot/traffic/EW/state
* - iot/traffic/WE/queue
* - iot/traffic/EW/queue

- Dashboard

* Built with HTML, CSS , JavaScript
* Displays traffic light state (colored circle) & live queue count.

##### Setup Instructions

1. Hardware

- Connect ESP32 with sensors & LEDs as per the circuit diagram (see /Docs/ folder).
- Upload the Arduino code (Arduino_Code/traffic.ino) using Arduino IDE.

2. MQTT Setup

- Ensure ESP32 is connected to Wi-Fi.
- Uses HiveMQ public broker (broker.hivemq.com) with WSS on port 8884.

3. Dashboard

- Open /Dashboard/index.html in a browser.
- Connects automatically to MQTT & displays real-time updates.
