# 🚦 Smart Traffic and Congestion Control System (IoT)

![ESP32](https://img.shields.io/badge/Board-ESP32-blue?logo=espressif)
![Arduino IDE](https://img.shields.io/badge/IDE-Arduino-green?logo=arduino)
![IoT](https://img.shields.io/badge/IoT-Smart%20Traffic-orange?logo=internet-of-things)
![MQTT](https://img.shields.io/badge/Protocol-MQTT-purple?logo=eclipse-mosquitto)
![HTML](https://img.shields.io/badge/Frontend-HTML5-red?logo=html5)
![Bootstrap](https://img.shields.io/badge/UI-Bootstrap-blueviolet?logo=bootstrap)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

---

## 🧠 Overview

Traffic congestion is a growing challenge in modern cities. Traditional traffic lights operate on fixed timers that do not adapt to real-time traffic conditions—leading to wasted time, fuel, and efficiency.  

This project implements an **IoT-based Smart Traffic Control System** using:

- **ESP32** microcontroller  
- **Ultrasonic sensors** to detect vehicle queues  
- **LED traffic lights** for simulation  
- **MQTT** for real-time communication  
- **Web dashboard** for live monitoring

The system dynamically adjusts green light durations based on actual queue sizes, reducing congestion and improving traffic flow.

---

## ⚙️ Hardware & Software Components

| Component | Purpose |
|-----------|---------|
| ESP32-WROOM-32S | Microcontroller for traffic logic and MQTT communication |
| Ultrasonic Sensors (HC-SR04) | Detect vehicle entry & exit |
| LEDs (Red, Yellow, Green) | Simulated traffic lights |
| Breadboard + Jumper Wires | Circuit assembly |
| Wi-Fi Network | MQTT connectivity |
| HiveMQ Public Broker | MQTT publish/subscribe communication |
| Arduino IDE | ESP32 code development |
| HTML, CSS, JavaScript, Bootstrap, MQTT.js | Real-time web dashboard |

---

## 🏗 System Design

The project simulates a **two-way traffic intersection**:

- **West → East (WE)**  
- **East → West (EW)**  

Each direction has:

- 1 Entry Sensor  
- 1 Exit Sensor  
- 3 Traffic LEDs (Red, Yellow, Green)  
- A queue counter (tracked in ESP32 memory)

**Workflow:**

1. Sensors detect vehicles entering/exiting.  
2. Queue size is updated in real-time.  
3. ESP32 calculates which direction should get green light based on queue comparison.  
4. Green duration = Base (5s) + (2s × Number of Cars).  
5. Traffic light states and queue values are published via MQTT.  
6. Dashboard updates live with traffic states.

---

## 🔧 How It Works

### 1️⃣ Queue Detection
- Entry sensor detects incoming cars → queue increases.  
- Exit sensor detects leaving cars → queue decreases.

### 2️⃣ Adaptive Timing
- Green light duration is calculated dynamically:  

```
Green Duration = BASE_GREEN_DURATION + (Queue Size × TIME_PER_CAR)
```

- Base Duration = 5 seconds  
- Time per Car = 2 seconds  

### 3️⃣ Traffic Light Logic
- **Sequence:** Green → Yellow (2s) → Red → Switch direction  
- Alternates based on real-time queue sizes

### 4️⃣ MQTT Communication
- ESP32 publishes real-time data to topics:  

```
iot/traffic/WE/state  → Current state of WE direction
iot/traffic/EW/state  → Current state of EW direction
iot/traffic/WE/queue  → Queue count of WE direction
iot/traffic/EW/queue  → Queue count of EW direction
```

### 5️⃣ Dashboard
- Built with HTML, CSS, JavaScript, and Bootstrap  
- Displays traffic light states as colored circles  
- Shows live vehicle queue counts for each lane  

---

## 🚀 Setup Instructions

### 1️⃣ Hardware
1. Connect ESP32 to ultrasonic sensors and LEDs as per the circuit diagram (see `/Docs/`).  
2. Upload the Arduino code (`Arduino_Code/traffic.ino`) via Arduino IDE.

### 2️⃣ MQTT Setup
1. Ensure ESP32 is connected to Wi-Fi.  
2. Uses HiveMQ public broker: `broker.hivemq.com` (WSS, port 8884).  

### 3️⃣ Dashboard
1. Open `/Dashboard/index.html` in a modern browser.  
2. Dashboard connects automatically to MQTT and displays real-time traffic states.  

---

## 📊 Traffic Phase Logic

```
PHASE_GREEN_WE → PHASE_YELLOW_WE → PHASE_GREEN_EW → PHASE_YELLOW_EW → repeat
```

- Phase transitions only occur if the opposing lane has a non-zero queue.  
- Prevents unnecessary switching when one lane has no traffic.

---

## 👤 Author

**Ahmed Bin Halabi**  
Software Engineering Student — Alfaisal University  

[GitHub](https://github.com/Ahmed-BinHelabi) | [LinkedIn](https://www.linkedin.com/in/ahmed-bin-halabi-a78127253/)
