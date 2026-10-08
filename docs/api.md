# API

## Vad är ett API?

Ett API är ett sätt för program att prata med varandra.

I det här projektet kan andra program fråga API:et: "Vad är temperaturen just nu?" API:et svarar med det senaste mätvärdet.

Man kan tänka på API:et som en reception. Man ställer en fråga, och receptionen svarar.

## Var finns API:et?

API:et körs på min dator. Man når det på den här adressen:

```
http://localhost:5000
```

Localhost betyder "den här datorn". 5000 är porten, alltså vilken dörr på datorn man knackar på.

## API:et har två adresser

### Adress 1: Hämta temperaturen

```
http://localhost:5000/api/sensors/latest
```

Här får man det senaste mätvärdet.

Men man måste ha en API-nyckel. En API-nyckel är som ett lösenord. Utan nyckeln får man inget svar.

Så här frågar man, med ett program som heter curl. Curl skickar en fråga till API:et från terminalen:

```
curl.exe -H "X-API-Key: din-nyckel" http://localhost:5000/api/sensors/latest
```

Om nyckeln är rätt får man svaret:

```
{"sensorId": "room-a-temp-01", "unit": "C", "value": 21.7}
```

Det betyder: sensorn room-a-temp-01 mätte 21,7 grader Celsius.

Om något är fel får man ett felmeddelande i stället:

- **401** betyder: fel eller ingen API-nyckel. Du får inte komma in.
- **404** betyder: det finns inget mätvärde än. Inget meddelande har kommit.

### Adress 2: Se om systemet mår bra

```
http://localhost:5000/health
```

Här ser man om allt fungerar. Man behöver ingen nyckel. Man kan öppna adressen direkt i webbläsaren.

Svaret ser ut så här:

```
{"status": "ok", "mqtt_connected": true, "messages_received": 3, "validation_errors": 1, "seconds_since_last_message": 23.8}
```

Det betyder:

- **status: ok** betyder att API:et fungerar.
- **mqtt_connected: true** betyder att Python-tjänsten är kopplad till brokern. Om det står false är den inte kopplad.
- **messages_received: 3** betyder att 3 meddelanden har kommit.
- **validation_errors: 1** betyder att 1 av meddelandena var trasigt.
- **seconds_since_last_message: 23.8** betyder att det senaste meddelandet kom för 23,8 sekunder sedan.

## Var finns API-nyckeln?

API-nyckeln står i filen src/server/credentials.py. Den filen laddas aldrig upp till GitHub, eftersom nyckeln är hemlig.