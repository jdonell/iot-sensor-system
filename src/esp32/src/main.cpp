#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include "secrets.h"

// ---------- Inställningar ----------
const int   MQTT_PORT  = 1883;
const char* MQTT_TOPIC = "byggnad/rum-a/temp";
const char* SENSOR_ID  = "room-a-temp-01";
const int   SENSOR_PIN = 4;                    // potentiometern kopplas till GPIO4
const unsigned long SEND_INTERVAL_MS = 5000;   // skicka var femte sekund

WiFiClient   wifiClient;        // nätverksanslutningen
PubSubClient mqtt(wifiClient);  // MQTT-klienten, som använder nätverksanslutningen

unsigned long lastSend = 0;         // när vi senast skickade
unsigned long lastMqttAttempt = 0;  // när vi senast försökte ansluta till brokern
unsigned long sentCount = 0;        // övervakning: antal skickade meddelanden
unsigned long failCount = 0;        // övervakning: antal misslyckade sändningar

// Läser potentiometern och gör om värdet till en temperatur mellan 18 och 26 grader.
float readTemperature() {
  int raw = analogRead(SENSOR_PIN);       // ger ett tal mellan 0 och 4095
  return 18.0 + (raw / 4095.0) * 8.0;     // 0 blir 18.0, 4095 blir 26.0
}

void connectWiFi() {
  Serial.printf("[WiFi] Ansluter till %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.printf("\n[WiFi] Ansluten, IP-adress: %s\n", WiFi.localIP().toString().c_str());
}

void connectMqtt() {
  Serial.printf("[MQTT] Ansluter till %s:%d ... ", MQTT_HOST, MQTT_PORT);
  // Varje klient behöver ett unikt namn. Vi använder kortets unika chip-ID.
  String clientId = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
  if (mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
    Serial.println("ansluten");
  } else {
    // Felkoder: -2 = brokern nås inte, 5 = fel användarnamn eller lösenord
    Serial.printf("misslyckades, kod %d. Försöker igen om 5 s\n", mqtt.state());
  }
}

// Ger aktuell tid som text, till exempel 2026-10-07T15:30:00+0200.
// Om klockan inte är hämtad från internet än blir texten tom.
String isoTimestamp() {
  struct tm t;
  if (!getLocalTime(&t, 0)) return "";
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%z", &t);
  return String(buf);
}

void sendReading() {
  float temp = readTemperature();
  String ts = isoTimestamp();

  // Bygg JSON-meddelandet enligt datakontraktet och spara det i en variabel.
  char payload[160];
  snprintf(payload, sizeof(payload),
           "{\"sensorId\":\"%s\",\"timestamp\":\"%s\",\"value\":%.1f,\"unit\":\"C\"}",
           SENSOR_ID, ts.c_str(), temp);

  if (mqtt.publish(MQTT_TOPIC, payload)) {
    sentCount++;
    Serial.printf("[MQTT] Skickat: %s\n", payload);
  } else {
    failCount++;
    Serial.println("[MQTT] Sändningen misslyckades");
  }
  Serial.printf("[Status] skickade=%lu, misslyckade=%lu\n", sentCount, failCount);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n[Start] IoT-sensorn startar");

  connectWiFi();
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org");  // hämta svensk tid från internet
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
}

void loop() {
  // 1. Om Wi-Fi har tappats: försök igen och hoppa över resten av varvet.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Anslutningen tappades, försöker igen...");
    WiFi.reconnect();
    delay(2000);
    return;
  }

  // 2. Om brokern inte är ansluten: försök igen, men högst var femte sekund.
  if (!mqtt.connected()) {
    if (millis() - lastMqttAttempt >= 5000) {
      lastMqttAttempt = millis();
      connectMqtt();
    }
    return;
  }

  // 3. Allt är anslutet: låt MQTT-biblioteket sköta sitt, och skicka var femte sekund.
  mqtt.loop();
  if (millis() - lastSend >= SEND_INTERVAL_MS) {
    lastSend = millis();
    sendReading();
  }
}