#include <Arduino.h>

// put function declarations here:


void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("hey from esp32");  //motsvarar printf("text\n"); 

  delay(1000);  // Väntar i 1000 millisekunder, alltså en sekund. det motsvarar sleep()  	
}  

// put function definitions here:

