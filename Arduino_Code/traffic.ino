#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Ahmed 4G";
const char* password = "0591256436a";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client_Traffic")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void publishState(const char* dir, const char* state, int queue) {
  char topicState[50];
  char topicQueue[50];
  sprintf(topicState, "iot/traffic/%s/state", dir);
  sprintf(topicQueue, "iot/traffic/%s/queue", dir);
  client.publish(topicState, state);
  client.publish(topicQueue, String(queue).c_str());
}


// SENSOR PINS
// WE
#define TRIG_ENTRY_WE 19
#define ECHO_ENTRY_WE 18
#define TRIG_EXIT_WE 5
#define ECHO_EXIT_WE 21

// EW
#define TRIG_ENTRY_EW 2
#define ECHO_ENTRY_EW 4
#define TRIG_EXIT_EW 22
#define ECHO_EXIT_EW 23

// RAFFIC LIGHT PINS
// WE
#define GREEN_WE 14
#define YELLOW_WE 27
#define RED_WE 26

// EW
#define GREEN_EW 25
#define YELLOW_EW 33
#define RED_EW 32

// VARIABLES
int queueWE = 0;
int queueEW = 0;

bool entryWE_Prev = false;
bool exitWE_Prev = false;
bool entryEW_Prev = false;
bool exitEW_Prev = false;

// PHASE STATE
enum Phase { PHASE_GREEN_WE, PHASE_YELLOW_WE, PHASE_GREEN_EW, PHASE_YELLOW_EW };
Phase currentPhase = PHASE_GREEN_WE;

unsigned long phaseStartTime = 0;

const unsigned long BASE_GREEN_DURATION = 5000;
const unsigned long MAX_GREEN_DURATION = 15000;
const unsigned long TIME_PER_CAR = 1000;
const unsigned long YELLOW_DURATION = 2000;

unsigned long greenDurationWE = BASE_GREEN_DURATION;
unsigned long greenDurationEW = BASE_GREEN_DURATION;

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  pinMode(TRIG_ENTRY_WE, OUTPUT); pinMode(ECHO_ENTRY_WE, INPUT);
  pinMode(TRIG_EXIT_WE, OUTPUT);  pinMode(ECHO_EXIT_WE, INPUT);
  pinMode(TRIG_ENTRY_EW, OUTPUT); pinMode(ECHO_ENTRY_EW, INPUT);
  pinMode(TRIG_EXIT_EW, OUTPUT);  pinMode(ECHO_EXIT_EW, INPUT);

  pinMode(GREEN_WE, OUTPUT); pinMode(YELLOW_WE, OUTPUT); pinMode(RED_WE, OUTPUT);
  pinMode(GREEN_EW, OUTPUT); pinMode(YELLOW_EW, OUTPUT); pinMode(RED_EW, OUTPUT);
}

long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 50000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

void handleQueue(bool detectedNow, bool &prevDetected, int &queue, const char* label) {
  if (detectedNow && !prevDetected) {
    queue++;
    Serial.print("ENTER "); Serial.print(label); Serial.print(" → queue++ → "); Serial.println(queue);
    prevDetected = true;
  }
  if (!detectedNow && prevDetected) {
    prevDetected = false;
  }
}

void handleExit(bool detectedNow, bool &prevDetected, int &queue, const char* label) {
  if (detectedNow && !prevDetected) {
    if (queue > 0) queue--;
    Serial.print("EXIT "); Serial.print(label); Serial.print(" → queue-- → "); Serial.println(queue);
    prevDetected = true;
  }
  if (!detectedNow && prevDetected) {
    prevDetected = false;
  }
}

void updateQueues() {
  long entryWE = readDistanceCM(TRIG_ENTRY_WE, ECHO_ENTRY_WE);
  long exitWE  = readDistanceCM(TRIG_EXIT_WE,  ECHO_EXIT_WE);
  handleQueue((entryWE > 0 && entryWE < 8), entryWE_Prev, queueWE, "WE");
  handleExit((exitWE > 0 && exitWE < 8), exitWE_Prev, queueWE, "WE");

  long entryEW = readDistanceCM(TRIG_ENTRY_EW, ECHO_ENTRY_EW);
  long exitEW  = readDistanceCM(TRIG_EXIT_EW,  ECHO_EXIT_EW);
  handleQueue((entryEW > 0 && entryEW < 8), entryEW_Prev, queueEW, "EW");
  handleExit((exitEW > 0 && exitEW < 8), exitEW_Prev, queueEW, "EW");
}

void switchToWE() {
  digitalWrite(GREEN_WE, HIGH); digitalWrite(YELLOW_WE, LOW); digitalWrite(RED_WE, LOW);
  digitalWrite(GREEN_EW, LOW);  digitalWrite(YELLOW_EW, LOW); digitalWrite(RED_EW, HIGH);
}

void switchToEW() {
  digitalWrite(GREEN_WE, LOW);  digitalWrite(YELLOW_WE, LOW); digitalWrite(RED_WE, HIGH);
  digitalWrite(GREEN_EW, HIGH); digitalWrite(YELLOW_EW, LOW); digitalWrite(RED_EW, LOW);
}

void switchToYellowWE() {
  digitalWrite(GREEN_WE, LOW);  digitalWrite(YELLOW_WE, HIGH); digitalWrite(RED_WE, LOW);
  digitalWrite(GREEN_EW, LOW);  digitalWrite(YELLOW_EW, LOW);  digitalWrite(RED_EW, HIGH);
}

void switchToYellowEW() {
  digitalWrite(GREEN_WE, LOW);  digitalWrite(YELLOW_WE, LOW);  digitalWrite(RED_WE, HIGH);
  digitalWrite(GREEN_EW, LOW);  digitalWrite(YELLOW_EW, HIGH); digitalWrite(RED_EW, LOW);
}

void loop() {
  updateQueues();

  // MQTT HANDLING
  if (!client.connected()) reconnect();
  client.loop();

  // MQTT HANDLING
  if (!client.connected()) reconnect();
  client.loop();

  if (currentPhase == PHASE_GREEN_WE) {
    publishState("WE", "GREEN", queueWE);
    publishState("EW", "RED", queueEW);
  }
  if (currentPhase == PHASE_YELLOW_WE) {
    publishState("WE", "YELLOW", queueWE);
    publishState("EW", "RED", queueEW);
  }
  if (currentPhase == PHASE_GREEN_EW) {
    publishState("EW", "GREEN", queueEW);
    publishState("WE", "RED", queueWE);
  }
  if (currentPhase == PHASE_YELLOW_EW) {
    publishState("EW", "YELLOW", queueEW);
    publishState("WE", "RED", queueWE);
  }

  Serial.print("queueWE: "); Serial.print(queueWE);
  Serial.print(" | queueEW: "); Serial.println(queueEW);

  greenDurationWE = BASE_GREEN_DURATION + queueWE * TIME_PER_CAR;
  if (greenDurationWE > MAX_GREEN_DURATION) greenDurationWE = MAX_GREEN_DURATION;

  greenDurationEW = BASE_GREEN_DURATION + queueEW * TIME_PER_CAR;
  if (greenDurationEW > MAX_GREEN_DURATION) greenDurationEW = MAX_GREEN_DURATION;

  unsigned long now = millis();
  unsigned long elapsed = now - phaseStartTime;

  switch (currentPhase) {
    case PHASE_GREEN_WE:
      switchToWE();
      if (elapsed >= greenDurationWE) {
        if (queueEW > 0) {
          currentPhase = PHASE_YELLOW_WE;
          phaseStartTime = now;
        }
      }
      break;
    case PHASE_YELLOW_WE:
      switchToYellowWE();
      if (elapsed >= YELLOW_DURATION) {
        currentPhase = PHASE_GREEN_EW;
        phaseStartTime = now;
      }
      break;
    case PHASE_GREEN_EW:
      switchToEW();
      if (elapsed >= greenDurationEW) {
        if (queueWE > 0) {
          currentPhase = PHASE_YELLOW_EW;
          phaseStartTime = now;
        }
      }
      break;
    case PHASE_YELLOW_EW:
      switchToYellowEW();
      if (elapsed >= YELLOW_DURATION) {
        currentPhase = PHASE_GREEN_WE;
        phaseStartTime = now;
      }
      break;
  }
  delay(200);
}
