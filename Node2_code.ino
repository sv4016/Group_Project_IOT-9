#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

// -------- Node Settings --------
#define NODE_ID 2
const char* nodeName = "node2";

// -------- Pins --------
#define DHTPIN     5
#define DHTTYPE    DHT11
#define LED_PIN    4
#define NUMPIXELS  1

DHT dht(DHTPIN, DHTTYPE);
Adafruit_NeoPixel led(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// -------- WiFi --------
const char* ssid     = "Test";
const char* password = "12345678";

// -------- HiveMQ TLS --------
const char* mqttServer = "d946ca5ed00846249842517aab740e61.s1.eu.hivemq.cloud";
const int   mqttPort   = 8883;
const char* mqttUser   = "Sarjjana";
const char* mqttPass   = "Qwerty123";

// -------- Topics --------
const char* dataTopic  = "greenhouse/node2/data";
const char* alertTopic = "greenhouse/alerts";

// -------- TLS Client --------
WiFiClientSecure secureClient;
PubSubClient mqtt(secureClient);

// -------- ALERT OVERRIDE --------
bool alertActive = false;
unsigned long alertUntil = 0;

// -------- LED helper --------
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  led.setPixelColor(0, led.Color(r, g, b));
  led.show();
}

// -------- WiFi --------
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nConnected ✔");
}

// -------- MQTT --------
void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Node2 connecting to HiveMQ... ");

    String clientId = "Node2_";
    clientId += WiFi.macAddress();
    clientId.replace(":", "");

    if (mqtt.connect(clientId.c_str(), mqttUser, mqttPass)) {
      Serial.println("✔ Connected");
      mqtt.subscribe(alertTopic);
    }
    else {
      Serial.print("FAILED, state=");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

// -------- MQTT CALLBACK --------
void mqttCallback(char* topic, byte* payload, unsigned int len) {
  String msg = "";
  for(int i=0;i<len;i++) msg += (char)payload[i];

  Serial.print("ALERT RECEIVED → ");
  Serial.println(msg);

  if (msg == "HOT") {
    setColor(255,0,0);
    alertActive = true;
    alertUntil = millis() + 5000;
  }
  else if (msg == "COLD") {
    setColor(0,0,255);
    alertActive = true;
    alertUntil = millis() + 5000;
  }
  else {
    alertActive = false;
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  led.begin();
  led.clear();
  led.show();

  connectWiFi();

  secureClient.setInsecure();
  mqtt.setServer(mqttServer, mqttPort);
  mqtt.setCallback(mqttCallback);
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Sensor error");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Humidity: ");
  Serial.println(h);

  if (alertActive && millis() < alertUntil) {
    Serial.println("⚠ OVERRIDE ACTIVE - ignoring local temp");
    delay(2000);
    return;
  }

  alertActive = false;

  String status;

  if (t > 28) {
    status = "Hot";
    setColor(255, 0, 0);
    mqtt.publish(alertTopic, "HOT");
  }
  else if (t < 18) {
    status = "Cold
    setColor(0, 0, 255);
    mqtt.publish(alertTopic, "COLD");
  }
  else {
    status = "Optimal";
    setColor(0, 255, 0);
    mqtt.publish(alertTopic, "NORMAL");
  }

  StaticJsonDocument<256> doc;
  doc["node"]        = NODE_ID;
  doc["name"]        = nodeName;
  doc["temperature"] = t;
  doc["humidity"]    = h;
  doc["status"]      = status;

  char buffer[256];
  size_t len = serializeJson(doc, buffer);
  mqtt.publish(dataTopic, buffer, len);

  delay(5000);
}