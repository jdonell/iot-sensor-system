# Felsökning

För att testa att systemet tål problem orsakade jag två fel med flit. För varje fel beskriver jag vad jag såg, hur jag hittade felet, vad som orsakade det, vad jag gjorde och hur jag kontrollerade att det fungerade igen.

---

## Fel 1: Trasigt JSON-meddelande

**Vad jag gjorde:** Jag skickade ett meddelande där den sista klammerparentesen `}` saknades. Meddelandet ligger i filen `tests/ogiltigt-meddelande.json`.

**1. Vad jag såg**
Inget nytt mätvärde dök upp. API:et visade fortfarande det gamla värdet.

**2. Hur jag hittade felet**
Python-tjänsten skrev en varning i sin logg, och räknaren för felaktiga meddelanden i `/health` gick upp med ett.

**3. Verktyg jag använde**
- **Python-tjänstens logg** (texten som skrivs ut i terminalen):
```
  WARNING Ogiltig JSON mottagen: {"sensorId": "room-a-temp-01", "value": 21.7, "unit": "C"
```
- **Adressen `/health`** i webbläsaren, som visade `"validation_errors": 1`.
- **Brokerns logg**, som visade att meddelandet var 60 bytes stort i stället för 65. Det saknades alltså några tecken.

**4. Vad som orsakade felet**
JSON måste följa bestämda regler, ungefär som en blankett. Eftersom `}` saknades kunde Python inte läsa meddelandet.

**5. Vad jag gjorde**
Koden som läser JSON ligger inuti `try` och `except`. Det betyder: *försök läsa meddelandet, och om det inte går, fånga felet i stället för att krascha*. Felet skrivs i loggen, räknas, och meddelandet hoppas över.

**6. Hur jag kontrollerade att det fungerade**
Programmet kraschade inte. `/health` svarade fortfarande, och när jag sedan skickade ett korrekt meddelande togs det emot som vanligt.

---

## Fel 2: Brokern försvinner

**Vad jag gjorde:** Jag stängde av brokern (programmet Mosquitto) medan Python-tjänsten var ansluten till den.

**1. Vad jag såg**
`/health` visade `"mqtt_connected": false`, alltså att tjänsten inte längre var ansluten till brokern.

**2. Hur jag hittade felet**
Jag använde kommandot `netstat` för att se vilket program som använde port 1883, som är MQTT:s port. Sedan stängde jag av det programmet.

**3. Verktyg jag använde**
- **`netstat -ano | findstr 1883`** visar vilka program som använder port 1883. Talet längst till höger är programmets ID-nummer:
```
  TCP  0.0.0.0:1883     0.0.0.0:0     LISTENING    10668   ← brokern
  TCP  [::1]:59362      [::1]:1883    ESTABLISHED  9008    ← Python-tjänsten
```
- **`taskkill /PID 10668 /F`** stänger av programmet med ID 10668, alltså brokern.
- **Python-tjänstens logg:** `WARNING Tappade anslutningen till brokern ... Försöker igen automatiskt.`
- **`/health`:** `"mqtt_connected": false`

**4. Vad som orsakade felet**
När brokern var avstängd lyssnade inget program på port 1883, så Python-tjänsten hade inget att ansluta till.

**5. Vad jag gjorde**
Jag startade brokern igen. Python-tjänsten är byggd för att själv försöka ansluta igen, med en väntetid på 1–30 sekunder mellan försöken. När den ansluter igen körs funktionen `on_connect`, och där prenumererar den på topicen på nytt. Annars hade den varit ansluten men inte fått några meddelanden.

**6. Hur jag kontrollerade att det fungerade**
I brokerns logg syns att Python-tjänsten anslöt och prenumererade igen bara fem sekunder efter att brokern startats, helt av sig själv:
```
1791375154: mosquitto version 2.1.2 running
1791375159: New client connected ... u'esp32'
1791375159: Received SUBSCRIBE ... byggnad/rum-a/temp
```
Därefter visade `/health` `"mqtt_connected": true` igen.

---

## Andra fel jag stötte på under arbetet

| Fel | Vad jag såg | Varför det hände | Vad jag gjorde |
|---|---|---|---|
| Fel lösenord | `Connection Refused: not authorised`. Brokern svarade med kod 5, som betyder "inte behörig". | Lösenordet i `credentials.py` var inte samma som i brokerns lösenordsfil. | Jag gjorde lösenorden lika. |
| Fel radbrytning | `mosquitto_pub` sa `Invalid input` när jag skickade en fil. | Filen hade Windows-radbrytningar (CRLF) i stället för LF. | Jag sparade filen med LF, och lade till `.gitattributes` så att JSON-filer alltid får LF. |
| Fel hastighet i serial monitor | Konstiga tecken som `������` i stället för text. | Serial monitor läste med hastigheten 9600 medan ESP32:n skickade med 115200. | Jag ändrade `monitor_speed` till 115200. |
| Kortet startade om hela tiden | Meddelandet `Brownout detector was triggered` när Wi-Fi slogs på. | USB-strömmen räckte inte när Wi-Fi-radion startade. | Jag bytte till ett ESP32-C6-DevKitM-1. |

