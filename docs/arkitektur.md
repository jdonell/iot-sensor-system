# Arkitektur

## Diagram

```
┌──────────────────┐
│ ESP32-C6         │
│ + potentiometer  │
└────────┬─────────┘
         │ MQTT, port 1883, JSON
         │ topic: byggnad/rum-a/temp
         ▼
┌──────────────────┐
│ Mosquitto-broker │
│ (inloggning)     │
└────────┬─────────┘
         │ MQTT, port 1883
         ▼
┌──────────────────┐
│ Python-tjänst    │
│ (app.py)         │
└────────┬─────────┘
         │ HTTP/REST, port 5000, JSON
         ▼
┌──────────────────┐
│ Klient (curl,    │
│ webbläsare, app) │
└──────────────────┘
```

## Komponenter och ansvar

| Komponent | Ansvar | Plats i repot |
|---|---|---|
| ESP32-C6 | Läser mätvärdet, bygger JSON och publicerar till brokern var 5:e sekund | `src/esp32/` |
| Mosquitto-broker | Tar emot meddelanden och skickar dem vidare till prenumeranter | `src/broker/` |
| Python-tjänst | Prenumererar, validerar JSON, sparar senaste värdet, loggar och tillhandahåller API:et | `src/server/` |
| Klient | Hämtar data via API:et | – |

## Nätverk

| Vad | Värde |
|---|---|
| Brokerns adress (från ESP32) | `192.168.0.127` (datorns IP i hemnätverket) |
| Brokerns adress (från Python-tjänsten) | `localhost` |
| MQTT-port | `1883` |
| API-port | `5000` |
| Topic | `byggnad/rum-a/temp` |

## Datakontrakt

Varje mätvärde skickas som JSON:

```json
{
  "sensorId": "room-a-temp-01",
  "timestamp": "2026-10-07T15:30:00+0200",
  "value": 21.7,
  "unit": "C"
}
```

| Fält | Typ | Beskrivning |
|---|---|---|
| `sensorId` | text | Sensorns unika namn |
| `timestamp` | text | Tidpunkt för mätningen (ISO 8601), hämtad via NTP |
| `value` | tal | Uppmätt temperatur |
| `unit` | text | Enhet, `C` för Celsius |

Python-tjänsten kräver fälten `sensorId`, `value` och `unit`, och att `value` är ett tal mellan -40 och 85.

## Återanslutning och kommunikationsfel

- **ESP32:** om Wi-Fi tappas försöker kortet ansluta igen. Om brokern inte svarar görs ett nytt försök var 5:e sekund.
- **Python-tjänsten:** återansluter automatiskt till brokern med 1–30 sekunders väntetid mellan försöken, och prenumererar igen i `on_connect`.
- **Ogiltiga meddelanden** fångas med `try`/`except` och räknas som valideringsfel, utan att tjänsten kraschar.