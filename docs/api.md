# API-dokumentation

Python-tjänsten (`src/server/app.py`) tillhandahåller ett REST-API på port `5000`.

Bas-URL: `http://localhost:5000` (från samma dator) eller `http://192.168.0.127:5000` (från andra enheter i nätverket).

## Autentisering

Endpointen för mätvärden kräver en API-nyckel i headern `X-API-Key`. Nyckeln konfigureras i `src/server/credentials.py`, som inte versionshanteras.

---

## GET /api/sensors/latest

Returnerar det senaste giltiga mätvärdet.

**Parametrar:** inga.

**Headers:**

| Header | Krävs | Beskrivning |
|---|---|---|
| `X-API-Key` | Ja | API-nyckeln |

**Exempel på anrop:**
```
curl.exe -H "X-API-Key: <din-nyckel>" http://localhost:5000/api/sensors/latest
```

**Svar vid lyckat anrop (200 OK):**
```json
{"sensorId": "room-a-temp-01", "timestamp": "2026-10-07T15:30:00+0200", "unit": "C", "value": 21.7}
```

**Felsvar:**

| Statuskod | När | Svar |
|---|---|---|
| `401 Unauthorized` | API-nyckeln saknas eller är fel | `{"error": "unauthorized", "message": "Ogiltig eller saknad API-nyckel"}` |
| `404 Not Found` | Inget giltigt mätvärde har tagits emot än | `{"error": "not_found", "message": "Inget mätvärde har tagits emot än"}` |

---

## GET /health

Visar systemets status. Används för övervakning och kräver ingen API-nyckel.

**Parametrar:** inga.

**Exempel på anrop:**
```
curl.exe http://localhost:5000/health
```
eller öppna adressen i en webbläsare.

**Svar (200 OK):**
```json
{
  "status": "ok",
  "mqtt_connected": true,
  "messages_received": 3,
  "validation_errors": 1,
  "seconds_since_last_message": 23.8
}
```

| Fält | Beskrivning |
|---|---|
| `status` | Alltid `ok` om API:et svarar |
| `mqtt_connected` | `true` om tjänsten är ansluten till brokern |
| `messages_received` | Antal mottagna MQTT-meddelanden, även ogiltiga |
| `validation_errors` | Antal meddelanden som inte följde datakontraktet |
| `seconds_since_last_message` | Sekunder sedan senaste meddelandet, `null` om inget har kommit |

## Felkontrakt

Alla felsvar har samma format:
```json
{"error": "<felkod>", "message": "<beskrivning på svenska>"}
```