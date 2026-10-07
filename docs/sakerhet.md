# Säkerhet

## Vad kan gå fel? (risker)

| # | Risk | Vad kan hända |
|---|---|---|
| 1 | En främling ansluter till brokern | Hen kan skicka falska mätvärden eller läsa alla meddelanden. |
| 2 | Lösenord hamnar på GitHub | Vem som helst på internet kan se mina lösenord. |
| 3 | En främling hämtar data från API:et | Mätvärdena hamnar hos någon som inte ska ha dem. |
| 4 | Ett trasigt eller falskt meddelande kommer in | Programmet kan krascha eller spara konstiga värden. |
| 5 | Någon tjuvlyssnar på nätverket | Meddelanden och lösenord skickas som vanlig text och kan läsas. |

## Vad jag har gjort för att skydda systemet

### 1. Brokern kräver inloggning (skyddar mot risk 1)

Brokern är postkontoret i systemet. Nu släpper den bara in den som har rätt användarnamn och lösenord. I filen `src/broker/mosquitto.conf` står:
```
allow_anonymous false
password_file C:\iot-sensor-system\src\broker\passwd
```
- `allow_anonymous false` betyder: ingen får komma in utan att logga in.
- `password_file` talar om var lösenorden finns.

Lösenordet sparas inte som vanlig text. Det sparas som en **hash**, en kod som är omöjlig att räkna tillbaka till lösenordet. Även om någon ser filen kan hen inte läsa lösenordet.

Så här testade jag det:

| Test | Resultat |
|---|---|
| Ansluta utan lösenord | Nekad: `not authorised` |
| Ansluta med fel lösenord | Nekad: `not authorised` |
| Ansluta med rätt lösenord | Insläppt |

### 2. Lösenord läggs aldrig på GitHub (skyddar mot risk 2)

Alla lösenord ligger i egna filer. Filen `.gitignore` säger åt git att låtsas att de filerna inte finns, så de laddas aldrig upp.

| Fil | Vad den innehåller |
|---|---|
| `src/esp32/include/secrets.h` | Wi-Fi-lösenordet och brokerns inloggning, för ESP32:n |
| `src/server/credentials.py` | Brokerns inloggning och API-nyckeln, för Python-tjänsten |
| `src/broker/passwd` | Brokerns lösenordsfil |

Jag lade till filerna i `.gitignore` **innan** jag skapade dem. Då kunde de aldrig råka följa med. Före varje uppladdning kontrollerade jag med `git status -u` att de inte var med.

### 3. API:et kräver en nyckel (skyddar mot risk 3)

En **API-nyckel** fungerar som ett lösenord för API:et. Den som vill hämta mätvärden måste skicka med nyckeln. Utan nyckel svarar API:et `401 Unauthorized`, som betyder "inte behörig".

Jag testade det på två sätt:
- **I webbläsaren**, utan nyckel: nekad.
- **Med kommandot `curl`**, med rätt nyckel: fick mätvärdet.

### 4. Varje meddelande kontrolleras (skyddar mot risk 4)

Python-tjänsten kontrollerar alla meddelanden innan den sparar dem:
- Är det korrekt JSON?
- Finns fälten `sensorId`, `value` och `unit`?
- Är `value` ett tal mellan -40 och 85? En temperatur på 500 grader är till exempel inte rimlig.

Om något är fel sparas meddelandet inte. Felet skrivs i loggen och räknas i `/health`. Jag testade det med filen `tests/ogiltigt-meddelande.json`.

## Hur lösenord och nycklar hanteras

- **Wi-Fi-lösenordet** finns bara i `secrets.h`. Själva koden i `main.cpp` använder bara namnet `WIFI_PASSWORD`, aldrig det riktiga lösenordet.
- **Brokerns lösenord** finns i `secrets.h` och `credentials.py`, och som hash i `passwd`.
- **API-nyckeln** finns bara i `credentials.py`.
- Den som laddar ner projektet måste skapa de här filerna själv, med egna lösenord. Hur man gör står i README.

## Vad som fortfarande inte är säkert

| Svaghet | Varför det är ett problem | Hur det skulle kunna lösas |
|---|---|---|
| Ingen kryptering | Allt skickas som vanlig text och kan läsas av någon som tjuvlyssnar (risk 5). | Använda **TLS**, som krypterar trafiken. Det är samma sak som ger hänglåset i webbläsaren. För MQTT används då port 8883. |
| Alla använder samma inloggning | Om lösenordet läcker kan någon låtsas vara vilken sensor som helst. | Ge varje sensor ett eget användarnamn. |
| Alla får skicka på alla topics | En inloggad enhet kan skicka och läsa överallt. | Lägga in **behörighetslistor** i Mosquitto, så att varje sensor bara får skicka på sin egen topic. |
| `/health` är öppen | Vem som helst i nätverket kan se hur systemet mår. | Kräva API-nyckel även där. |
| Brokern tar emot från alla nätverk | Den lyssnar på alla nätverk datorn är ansluten till. | Lägga in brandväggsregler som bara släpper in enheter från hemnätverket. |

