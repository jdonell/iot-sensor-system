# IoT Sensor System

## Vad är det här?

Det här projektet mäter temperaturen i ett rum. Andra program kan sedan fråga efter temperaturen.

Det fungerar som ett postsystem:

1. ESP32-kortet mäter temperaturen och skickar den som ett meddelande.
2. Brokern är som ett postkontor. Den tar emot meddelandet och skickar det vidare.
3. Python-tjänsten är som en sekreterare. Den tar emot meddelandet, kollar att det är rätt och sparar det.
4. API:et är som en reception. Andra program kan fråga där vad temperaturen är.

```
ESP32  ->  Broker  ->  Python-tjänst  ->  API
```

## Mapparna

- docs: beskrivningar av projektet
- src/esp32: koden till ESP32-kortet
- src/broker: inställningar för brokern
- src/server: Python-tjänsten och API:et
- tests: tester och testmeddelanden

## Det här behöver du

Program:
- VS Code med tillägget PlatformIO
- Mosquitto, som är brokern
- Python 3.11 eller nyare
- Git

Saker:
- Ett ESP32-C6-DevKitM-1-kort
- En potentiometer, alltså ett vred, kopplad till pinne 4 på kortet

## Steg 1: Ladda ner och installera

Ladda ner projektet. Skriv i terminalen:

```
git clone https://github.com/jdonell/iot-sensor-system.git
cd iot-sensor-system
```

Installera det som Python behöver:

```
python -m pip install -r src/server/requirements.txt
```

Installera Mosquitto från mosquitto.org/download. Ta bort krysset vid Service under installationen.

## Steg 2: Skapa lösenordsfilerna

Lösenord ska aldrig ligga på GitHub. Därför måste du skapa tre filer själv.

Fil 1, brokerns lösenord. Skriv i terminalen:

```
& "C:\Program Files\Mosquitto\mosquitto_passwd.exe" -c src\broker\passwd esp32
```

Skriv ett lösenord två gånger. Tecknen syns inte när du skriver. Det är meningen.

Fil 2 heter src/server/credentials.py. Skriv det här i den:

```
MQTT_USER = "esp32"
MQTT_PASSWORD = "lösenordet från fil 1"
API_KEY = "en nyckel du hittar på"
```

Fil 3 heter src/esp32/include/secrets.h. Skriv det här i den:

```
#pragma once

#define WIFI_SSID     "namnet på ditt wifi"
#define WIFI_PASSWORD "lösenordet till ditt wifi"

#define MQTT_HOST     "din dators IP-adress"
#define MQTT_USER     "esp32"
#define MQTT_PASSWORD "lösenordet från fil 1"
```

Din dators IP-adress hittar du genom att skriva ipconfig i terminalen. ESP32-kortet och datorn måste vara på samma wifi.

## Steg 3: Starta allt

Starta delarna i den här ordningen. Varje del behöver ett eget terminalfönster.

Starta brokern:

```
& "C:\Program Files\Mosquitto\mosquitto.exe" -c src\broker\mosquitto.conf -v
```

Vänta tills det står running.

Starta Python-tjänsten i ett nytt fönster:

```
python src\server\app.py
```

Vänta tills det står Ansluten till brokern.

Starta ESP32-kortet: öppna mappen src/esp32 i VS Code och klicka på Upload längst ner.

## Steg 4: Kolla att det fungerar

Skicka ett testmeddelande. Byt ut lösenord mot ditt lösenord:

```
& "C:\Program Files\Mosquitto\mosquitto_pub.exe" -h localhost -t "byggnad/rum-a/temp" -u esp32 -P lösenord -f tests\giltigt-meddelande.json
```

Öppna sedan http://localhost:5000/health i webbläsaren. Där ser du hur många meddelanden som har kommit.

Fråga API:et efter temperaturen. Byt ut din-nyckel mot din API-nyckel:

```
curl.exe -H "X-API-Key: din-nyckel" http://localhost:5000/api/sensors/latest
```

Du ska få tillbaka temperaturen.

## Om något inte fungerar

- Det står ConnectionRefusedError: brokern är inte igång. Starta den igen.
- Det står not authorised: fel användarnamn eller lösenord.
- Det står Invalid input: filen har fel sorts radbrytning. Klicka på CRLF längst ner i VS Code och välj LF.
- Det står konstiga tecken i Serial Monitor: fel hastighet. Det ska stå monitor_speed = 115200 i platformio.ini.
- API:et svarar 401: API-nyckeln saknas eller är fel.

Fler fel finns i docs/felsokning.md.

## Det här fungerar inte än

- ESP32-kortet är inte testat på riktigt. Koden fungerar att bygga, men jag kunde inte skicka den till kortet. Därför testade jag med mosquitto_pub i stället.
- Jag använder ett vred i stället för en temperatursensor. Vredets läge görs om till en temperatur mellan 18 och 26 grader.
- Inget är krypterat. Allt skickas som vanlig text.
- Det senaste värdet sparas bara medan programmet kör. Om programmet startas om är det borta.
- Sökvägen till lösenordsfilen i mosquitto.conf är skriven för min dator.