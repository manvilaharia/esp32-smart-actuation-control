#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";

WiFiClient espClient;
PubSubClient client(espClient);

// PWM setup
int pins[4] = {3, 5, 6, 7};
int channels[4] = {0, 1, 2, 3};
int duty[4] = {0, 0, 0, 0};
bool ledTimerActive[4] = {false, false, false, false};
unsigned long ledOnTime[4] = {0, 0, 0, 0};
const unsigned long ledOnDuration = 3000;
const unsigned long ledOffDuration = 2000;

// Limit switch
const int limitSwitchPin = 4;
bool lastLimitSwitchState = HIGH;
bool limitSwitchPressed = false;
unsigned long lastLimitPublish = 0;
const unsigned long limitPublishInterval = 1000;

// Thin Potentiometer / Laser Sensor
const int thinPotPin = 1;
const int graph_length = 20;
float voltage = 0;
float filteredVoltage = 0.0;
float filtered_disp = 0.0;
float alpha = 0.4;
bool isFirstReading = true;

// Displacement curve
float calculateY(float x) {
  return -0.08041 * x * x + 1.067 * x + 0.0003;
}

void setup_wifi() {
  Serial.println();
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[WiFi] Connected");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());
}

// MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String msg = String((char*)payload);
  msg.trim();

  Serial.print("[MQTT] Msg [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  for (int i = 0; i < 4; i++) {
    String pwmTopic = "led/pwm" + String(i + 1);
    if (String(topic) == pwmTopic) {
      int val = constrain(msg.toInt(), 0, 4095);
      duty[i] = val;
      ledTimerActive[i] = (val > 0);
      ledOnTime[i] = millis();
      Serial.printf("[PWM] Channel %d (GPIO %d) set to %d\n", i + 1, pins[i], duty[i]);

      String statusTopic = "led/status" + String(i + 1);
      client.publish(statusTopic.c_str(), String(duty[i]).c_str(), true);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("[MQTT] Connecting...");
    if (client.connect("ESPClient")) {
      Serial.println(" connected");
      client.publish("sensor/status", "ESP32 online");

      for (int i = 0; i < 4; i++) {
        client.subscribe(("led/pwm" + String(i + 1)).c_str());
      }
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.println(" – retrying in 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  pinMode(thinPotPin, INPUT);
  pinMode(limitSwitchPin, INPUT_PULLUP);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  for (int i = 0; i < 4; i++) {
    ledcSetup(channels[i], 1000, 12);
    ledcAttachPin(pins[i], channels[i]);
    ledcWrite(channels[i], 0);
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Read limit switch
  bool currentSwitch = digitalRead(limitSwitchPin);
  limitSwitchPressed = (currentSwitch == HIGH);

  if (limitSwitchPressed != lastLimitSwitchState) {
    lastLimitSwitchState = limitSwitchPressed;
    client.publish("limit/switch", limitSwitchPressed ? "PRESSED" : "RELEASED");
  }

  if (millis() - lastLimitPublish >= limitPublishInterval) {
    lastLimitPublish = millis();
    client.publish("limit/switch", limitSwitchPressed ? "PRESSED" : "RELEASED");
  }

  // PWM logic
  for (int i = 0; i < 4; i++) {
    if (limitSwitchPressed && ledTimerActive[i]) {
      unsigned long now = millis();
      unsigned long elapsed = now - ledOnTime[i];

      if (elapsed < ledOnDuration) {
        ledcWrite(channels[i], duty[i]);
      } else if (elapsed < ledOnDuration + ledOffDuration) {
        ledcWrite(channels[i], 0);
      } else {
        ledOnTime[i] = millis();
      }
    } else {
      ledcWrite(channels[i], 0);
    }
  }

  // Read thinpot
  int adcVal = analogRead(thinPotPin);
  voltage = adcVal * (3.3 / 4095.0);
  if (isFirstReading) {
    filteredVoltage = voltage;
    isFirstReading = false;
  } else {
    filteredVoltage = alpha * voltage + (1 - alpha) * filteredVoltage;
  }

  float disp = calculateY(filteredVoltage);
  filtered_disp = alpha * disp + (1 - alpha) * filtered_disp;

  if (filteredVoltage < 0.05) filteredVoltage = 0.0;
  if (filtered_disp < 0.05) filtered_disp = 0.0;

  int pos = map(adcVal, 0, 4095, 0, graph_length);

  // Serial Output
  Serial.println("--------------------------------------------------");
  for (int i = 0; i < 4; i++) {
    Serial.printf("[PWM] CH%d (GPIO %d): %4d\n", i + 1, pins[i], duty[i]);
  }
  Serial.printf("[THINPOT] Raw ADC: %d | Voltage: %.3f V | Filtered: %.3f V\n", adcVal, voltage, filteredVoltage);
  Serial.printf("[DISPLACEMENT] Filtered: %.3f\n", filtered_disp);
  Serial.printf("[LIMIT SWITCH] %s\n", limitSwitchPressed ? "PRESSED" : "RELEASED");

  Serial.print("<");
  for (int i = 0; i < graph_length; i++) {
    Serial.print(i == pos ? "|" : "-");
  }
  Serial.println(">");

  // MQTT Publish
  client.publish("sensor/raw_adc", String(adcVal).c_str(), true);
  client.publish("sensor/voltage", String(filteredVoltage, 3).c_str(), true);
  client.publish("sensor/displacement", String(filtered_disp, 3).c_str(), true);
  client.publish("sensor/thinpot_position", String(pos).c_str(), true);

  delay(300);
}
