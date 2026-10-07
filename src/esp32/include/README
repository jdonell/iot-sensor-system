# IoT Sensor System

## Vad är det här?

Det här projektet mäter temperaturen i ett rum och visar den för andra program.

Det fungerar ungefär som ett postsystem:

1. **ESP32:n** är en liten dator med en sensor. Den mäter temperaturen och skickar den som ett meddelande.
2. **Brokern** är som ett postkontor. Den tar emot meddelandet och skickar det vidare.
3. **Python-tjänsten** är som en sekreterare. Den tar emot meddelandet, kontrollerar att det är rätt ifyllt och sparar det senaste värdet.
4. **API:et** är som en reception. Andra program kan fråga där och få reda på temperaturen.

```
ESP32  -->  Broker  -->  Python-tjänst  -->  API
```

Mer om hur delarna hänger ihop finns i [docs/arkitektur.md](docs/arkitektur.md).

## Vad finns i mapparna?

```
iot-sensor-system/
├── README.md             Den här filen
├── docs/                 Beskrivningar av projektet
├── src/
│   ├── esp32/            Koden till ESP32:n
│   ├── broker/           Inställningar för brokern
│   └── server/           Python-tjänsten och API:et
└── tests/                Tester och testmeddelanden
```

## Vad behöver jag?

**Program:**
- **VS Code** med tillägget **PlatformIO**, för att skicka koden till ESP32:n
- **Mosquitto**, som är brokern
- **Python** version 3.11 eller nyare
- **Git**, för att ladda ner projektet

**Saker:**
- Ett ESP32-C6-DevKitM-1-kort
- En potentiometer (ett vred), kopplad till pinne 4 på kortet

## Steg 1: Ladda ner och installera

**1. Ladda ner projektet.** Skriv i terminalen:
```
git clone https://github.com/jdonell/iot-sensor-system.git
cd iot-sensor-system
```

**2. Installera det Python behöver.** Skriv:
```
python -m pip install -r src/server/requirements.txt
```
Det installerar två bibliotek: `paho-mqtt`, som pratar med brokern, och `flask`, som gör API:et.

**3. Installera Mosquitto** från mosquitto.org/download. Under installationen: ta bort krysset vid **Service**.

## Steg 2: Skapa lösenordsfilerna

Lösenord ska aldrig ligga på GitHub. Därför måste du skapa tre filer själv.

**Fil 1: Brokerns lösenord.** Skriv i terminalen:
```
& "C:\Program Files\Mosquitto\mosquitto_passwd.exe" -c src\broker\passwd esp32
```
Du får skriva ett lösenord två gånger. Tecknen syns inte när du skriver, det är meningen. Kom ihåg lösenordet, du behöver det igen.

**Fil 2: `src/server/credentials.py`.** Skapa filen och skriv i den:
```python
MQTT_USER = "esp32"
MQTT_PASSWORD = "lösenordet-från-fil-1"
API_KEY = "hitta-på-en-nyckel"
```
API-nyckeln är ett lösenord för API:et. Du kan låta Python hitta på en åt dig:
```
python -c "import secrets; print(secrets.token_hex(16))"
```

**Fil 3: `src/esp32/include/secrets.h`.** Skapa filen och skriv i den:
```cpp
#pragma once

#define WIFI_SSID     "namnet-på-ditt-wifi"
#define WIFI_PASSWORD "lösenordet-till-ditt-wifi"

#define MQTT_HOST     "din-dators-ip-adress"
#define MQTT_USER     "esp32"
#define MQTT_PASSWORD "lösenordet-från-fil-1"
```
- Din dators IP-adress hittar du genom att skriva `ipconfig` i terminalen. Leta efter raden **IPv4-adress**.
- ESP32:n och datorn måste vara på **samma Wi-Fi**.
- Wi-Fi-nätverket måste vara **2,4 GHz**. ESP32:n klarar inte 5 GHz.

## Steg 3: Starta systemet

Starta delarna **i den här ordningen**. Varje del behöver ett **eget terminalfönster**, och fönstret måste vara öppet så länge delen ska köra.

**1. Starta brokern:**
```
& "C:\Program Files\Mosquitto\mosquitto.exe" -c src\broker\mosquitto.conf -v
```
Vänta tills det står `running`. Om Windows frågar om brandväggen: välj **Privata nätverk** och klicka **Tillåt**.

**2. Starta Python-tjänsten**, i ett nytt fönster:
```
python src\server\app.py
```
Vänta tills det står `Ansluten till brokern`.

**3. Starta ESP32:n.** Öppna mappen `src/esp32` i VS Code. Klicka på pilen **→** (Upload) längst ner. Om det fastnar på `Connecting...`, håll inne knappen **BOOT** på kortet. Klicka sedan på **kontakten** (Serial Monitor) för att se vad kortet gör.

## Steg 4: Kontrollera att det fungerar

**Skicka ett testmeddelande** (om du inte har en ESP32). Byt `<lösenord>` mot lösenordet från fil 1:
```
& "C:\Program Files\Mosquitto\mosquitto_pub.exe" -h localhost -t "byggnad/rum-a/temp" -u esp32 -P <lösenord> -f tests\giltigt-meddelande.json
```

**Titta om det kom fram:**
- I Python-tjänstens fönster ska det stå `Mätvärde: 21.7 C från room-a-temp-01`.
- Öppna `http://localhost:5000/health` i webbläsaren. Där ser du hur systemet mår och hur många meddelanden som kommit.

**Fråga API:et efter temperaturen.** Byt `<din-nyckel>` mot din API-nyckel:
```
curl.exe -H "X-API-Key: <din-nyckel>" http://localhost:5000/api/sensors/latest
```
Du ska få tillbaka temperaturen. Mer om API:et finns i [docs/api.md](docs/api.md).

## Om något inte fungerar

| Det här händer | Det betyder oftast |
|---|---|
| `ConnectionRefusedError` eller "måldatorn nekade det" | Brokern är inte igång. Starta den igen. |
| `not authorised` | Fel användarnamn eller lösenord. |
| `Invalid input` när du skickar en fil | Filen har fel sorts radbrytning. Klicka på **CRLF** längst ner i VS Code och välj **LF**. |
| Konstiga tecken i Serial Monitor | Fel hastighet. Det ska stå `monitor_speed = 115200` i `platformio.ini`. |
| `401` från API:et | API-nyckeln saknas eller är fel. |

Fler fel och hur de löstes finns i [docs/felsokning.md](docs/felsokning.md).

## Det här fungerar inte än

- **ESP32:n är inte testad på riktigt.** Koden går att bygga, men jag lyckades inte ladda upp den till kortet (felet `No serial data received`). Därför har jag testat med `mosquitto_pub` i stället. Se test 15 i [tests/testprotokoll.md](tests/testprotokoll.md).
- **En potentiometer används i stället för en temperatursensor.** Vredets läge räknas om till en temperatur mellan 18 och 26 grader.
- **Ingen kryptering.** Data och lösenord skickas som vanlig text. Ett riktigt system skulle använda TLS.
- **Det senaste värdet sparas bara i minnet.** Det försvinner om Python-tjänsten startas om.
- **Flasks server är gjord för att testa**, inte för många användare samtidigt.
- **Sökvägen till lösenordsfilen** i `mosquitto.conf` är skriven för min dator och måste ändras på en annan.

Mer om säkerheten finns i [docs/sakerhet.md](docs/sakerhet.md).