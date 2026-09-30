#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"



float tempReading (); 
void setup() {
  // Öppna kanalen till serial monitor.
  Serial.begin(115200);

  // Börja ansluta till Wi-Fi, en gång.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Vänta tills anslutningen är klar.
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // När vi kommer hit är ESP32:n ansluten.
  Serial.printf("\nESP32 ansluten till Wi-Fi!\n");
  Serial.print("IP-adress: ");
  Serial.println(WiFi.localIP());
}

void loop() {
 

  float temp = tempReading(); 
   
  Serial.printf("{\"sensorId\": \"room-a-temp-01\", \"value\": %.1f, \"unit\": \"C\"}\n", temp);
  
  delay(5000); 
}


float tempReading (){
  float randomNumber = random(180, 260) / 10.0;
  return randomNumber;
  // random(180, 260) ger ett heltal mellan 180 och 259.
  // Delat med 10.0 blir det ett decimaltal mellan 18.0 och 25.9.
}
