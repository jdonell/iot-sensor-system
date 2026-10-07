# Testprotokoll

## Hur jag testade

Jag testade hela systemet från början till slut, alltså att ett meddelande går hela vägen från avsändaren till API:et. Allt kördes på min Windows-dator.

ESP32:n kunde jag inte använda i testerna, eftersom jag inte lyckades ladda upp kod till kortet (se test 15). I stället använde jag verktyget `mosquitto_pub`, som skickar meddelanden till brokern på samma sätt som ESP32:n gör.

**Testfiler jag använde:**
- `tests/giltigt-meddelande.json`: ett korrekt meddelande
- `tests/ogiltigt-meddelande.json`: ett trasigt meddelande, där den sista `}` saknas

## Testerna

### Brokern

| Nr | Vad jag testade | Vad jag gjorde | Vad jag förväntade mig | Gick det? |
|---|---|---|---|---|
| 1 | Brokern startar med min inställningsfil | Startade Mosquitto med `-c src/broker/mosquitto.conf` | Det står `Config loaded` och `running` | Ja |
| 2 | Brokern tar emot anslutningar från nätverket | Körde `netstat -ano \| findstr 1883`, som visar vilka program som använder port 1883 | Det står `LISTENING` på port 1883 | Ja |
| 3 | Man kommer inte in utan lösenord | Försökte ansluta utan användarnamn och lösenord | Brokern säger `not authorised` | Ja |
| 4 | Man kommer inte in med fel lösenord | Försökte ansluta med fel lösenord | Brokern säger `not authorised` | Ja |
| 5 | Man kommer in med rätt lösenord | Anslöt med rätt användarnamn och lösenord | Brokern släpper in | Ja |

### Python-tjänsten

| Nr | Vad jag testade | Vad jag gjorde | Vad jag förväntade mig | Gick det? |
|---|---|---|---|---|
| 6 | Tjänsten ansluter till brokern | Startade `python src/server/app.py` | Det står `Ansluten till brokern` | Ja |
| 7 | Tjänsten tar emot ett korrekt meddelande | Skickade `giltigt-meddelande.json` | Det står `Mätvärde: 21.7 C från room-a-temp-01` i loggen | Ja |
| 8 | Tjänsten klarar ett trasigt meddelande | Skickade `ogiltigt-meddelande.json` | Det står en varning i loggen, felräknaren går upp, och programmet fortsätter köra | Ja |

### API:et

| Nr | Vad jag testade | Vad jag gjorde | Vad jag förväntade mig | Gick det? |
|---|---|---|---|---|
| 9 | Övervakningen visar hur systemet mår | Öppnade `http://localhost:5000/health` i webbläsaren | Det står `mqtt_connected: true` och hur många meddelanden som kommit | Ja |
| 10 | Man får inte data utan API-nyckel | Öppnade `/api/sensors/latest` i webbläsaren, utan nyckel | API:et svarar `401`, som betyder "inte behörig" | Ja |
| 11 | Man får data med rätt API-nyckel | Använde `curl` och skickade med nyckeln | Jag får tillbaka mätvärdet 21.7 | Ja |

### Om brokern försvinner

| Nr | Vad jag testade | Vad jag gjorde | Vad jag förväntade mig | Gick det? |
|---|---|---|---|---|
| 12 | Tjänsten märker att brokern är borta | Stängde av brokern med `taskkill` | `/health` visar `mqtt_connected: false`, men API:et svarar fortfarande | Ja |
| 13 | Tjänsten ansluter igen av sig själv | Startade brokern igen | Tjänsten ansluter igen utan att jag gör något | Ja, efter 5 sekunder |

### ESP32

| Nr | Vad jag testade | Vad jag gjorde | Vad jag förväntade mig | Gick det? |
|---|---|---|---|---|
| 14 | ESP32-koden går att bygga | Klickade på Build i PlatformIO | Det står `SUCCESS` | Ja |
| 15 | ESP32:n skickar mätvärden hela vägen till API:et | Försökte ladda upp koden till kortet | Mätvärden från ESP32:n syns i API:et | Nej: uppladdningen misslyckades med felet `No serial data received` |

## Sammanfattning

14 av 15 tester gick igenom. Hela vägen från brokern till API:et fungerar: inloggning, API-nyckel, kontroll av meddelanden och återanslutning. Det som saknas är att köra koden på det riktiga ESP32-kortet (test 15).