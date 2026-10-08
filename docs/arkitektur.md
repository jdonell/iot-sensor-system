# Arkitektur

Den här filen beskriver hur systemet är uppbyggt och hur delarna pratar med varandra.

## Översikt

(Skriv här med egna ord: vad mäter systemet, vart skickas värdet, och vem kan läsa det till slut?)

## Hur delarna hänger ihop

Systemet har fyra delar. Ett mätvärde går genom dem i den här ordningen:

1. ESP32-kortet läser ett värde från en potentiometer och gör om det till en temperatur.
2. ESP32-kortet skickar värdet till brokern med MQTT.
3. Brokern skickar värdet vidare till Python-tjänsten, också med MQTT.
4. Python-tjänsten sparar värdet. Andra program kan sedan fråga efter det via API:et, med HTTP.

Kort sagt:

```
ESP32  ->  Broker  ->  Python-tjänst  ->  API
```

## Vad varje del gör

- ESP32-kortet: läser mätvärdet, gör om det till JSON och skickar det till brokern var femte sekund. Koden finns i mappen src/esp32.
- Brokern (Mosquitto): tar emot meddelanden och skickar dem vidare till alla som har bett om dem. Inställningarna finns i mappen src/broker.
- Python-tjänsten: tar emot meddelandena, kontrollerar att de är rätt ifyllda, sparar det senaste värdet och skriver vad som händer i en logg. Koden finns i mappen src/server.
- API:et: är en del av Python-tjänsten. Andra program kan fråga det efter det senaste mätvärdet.

## Adresser och portar

- ESP32-kortet når brokern på datorns IP-adress, 192.168.0.127.
- Python-tjänsten når brokern på localhost, eftersom de körs på samma dator.
- MQTT använder port 1883.
- API:et använder port 5000.
- Mätvärdena skickas på topicen byggnad/rum-a/temp.

## Hur delarna pratar med varandra

(Skriv här med egna ord: förklara publish och subscribe. Vem skickar, vem tar emot, och vad gör brokern i mitten?)

## Varför MQTT

(Skriv här med egna ord: varför passar MQTT för att skicka mätvärden från en liten ESP32? Jämför gärna med HTTP.)

## Hur ett meddelande ser ut

Varje mätvärde skickas som JSON. Ett meddelande ser ut så här:

```json
{
  "sensorId": "room-a-temp-01",
  "timestamp": "2026-10-07T15:30:00+0200",
  "value": 21.7,
  "unit": "C"
}
```

Det här betyder fälten:

- sensorId är sensorns namn, så att man vet vilken sensor värdet kommer från.
- timestamp är tiden då mätningen gjordes. ESP32-kortet hämtar tiden från internet.
- value är temperaturen.
- unit är enheten. C betyder Celsius.

Python-tjänsten kräver att sensorId, value och unit finns med. Den kräver också att value är ett tal mellan -40 och 85. Annars sparas inte meddelandet.

## Om något går fel med anslutningen

- Om ESP32-kortet tappar Wi-Fi försöker det ansluta igen.
- Om ESP32-kortet inte når brokern försöker det igen var femte sekund.
- Om Python-tjänsten tappar brokern försöker den ansluta igen av sig själv. När den har anslutit ber den om meddelandena på nytt.
- Om ett meddelande är trasigt kraschar inte Python-tjänsten. Den skriver felet i loggen och hoppar över meddelandet.

## Begränsningar och förbättringar

- Just nu skickas alla meddelanden som vanlig text över nätverket, även lösenorden. Någon som tjuvlyssnar på nätverket kan läsa dem. Med kryptering, som heter TLS, blir texten oläslig för alla utom mottagaren.

- Både ESP32:n och Python-tjänsten loggar in med användarnamnet esp32 och samma lösenord. Om lösenordet läcker kan någon låtsas vara vilken del av systemet som helst. Bättre vore att varje del har sitt eget.
