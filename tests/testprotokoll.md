# Testprotokoll

## Vad är ett testprotokoll?

Ett testprotokoll är en lista över allt jag har testat, och om det fungerade.

## Hur jag testade

Jag testade allt på min dator. Jag kunde inte använda ESP32-kortet, eftersom jag inte lyckades skicka koden till det. I stället använde jag ett program som heter mosquitto_pub. Det skickar meddelanden till brokern på samma sätt som ESP32:n gör.

Jag använde två testfiler:
- giltigt-meddelande.json är ett rätt meddelande.
- ogiltigt-meddelande.json är ett trasigt meddelande.

## Testerna

### Test av brokern

**Test 1: Brokern startar**
Jag gjorde: startade brokern med min inställningsfil.
Jag väntade mig: att det skulle stå running.
Det fungerade: ja.

**Test 2: Brokern lyssnar på nätverket**
Jag gjorde: körde kommandot netstat, som visar vilka program som använder port 1883.
Jag väntade mig: att det skulle stå LISTENING.
Det fungerade: ja.

**Test 3: Man kommer inte in utan lösenord**
Jag gjorde: försökte koppla in mig utan lösenord.
Jag väntade mig: att brokern skulle säga not authorised.
Det fungerade: ja.

**Test 4: Man kommer inte in med fel lösenord**
Jag gjorde: försökte koppla in mig med fel lösenord.
Jag väntade mig: att brokern skulle säga not authorised.
Det fungerade: ja.

**Test 5: Man kommer in med rätt lösenord**
Jag gjorde: kopplade in mig med rätt lösenord.
Jag väntade mig: att brokern skulle släppa in mig.
Det fungerade: ja.

### Test av Python-tjänsten

**Test 6: Python-tjänsten kopplar in sig på brokern**
Jag gjorde: startade Python-tjänsten.
Jag väntade mig: att det skulle stå Ansluten till brokern.
Det fungerade: ja.

**Test 7: Ett rätt meddelande kommer fram**
Jag gjorde: skickade giltigt-meddelande.json.
Jag väntade mig: att det skulle stå Mätvärde: 21.7 C i loggen.
Det fungerade: ja.

**Test 8: Ett trasigt meddelande kraschar inte programmet**
Jag gjorde: skickade ogiltigt-meddelande.json.
Jag väntade mig: en varning i loggen, och att programmet fortsatte köra.
Det fungerade: ja.

### Test av API:et

**Test 9: Sidan /health visar hur systemet mår**
Jag gjorde: öppnade http://localhost:5000/health i webbläsaren.
Jag väntade mig: att det skulle stå mqtt_connected: true.
Det fungerade: ja.

**Test 10: Man får inget utan API-nyckel**
Jag gjorde: öppnade /api/sensors/latest i webbläsaren, utan nyckel.
Jag väntade mig: felkod 401, som betyder "du får inte komma in".
Det fungerade: ja.

**Test 11: Man får mätvärdet med rätt API-nyckel**
Jag gjorde: frågade API:et med curl och rätt nyckel.
Jag väntade mig: att få tillbaka 21.7.
Det fungerade: ja.

### Test när brokern försvinner

**Test 12: Python-tjänsten märker att brokern är borta**
Jag gjorde: stängde av brokern med taskkill.
Jag väntade mig: att /health skulle visa mqtt_connected: false.
Det fungerade: ja.

**Test 13: Python-tjänsten kopplar in sig igen av sig själv**
Jag gjorde: startade brokern igen.
Jag väntade mig: att Python-tjänsten skulle koppla in sig igen utan hjälp.
Det fungerade: ja, efter 5 sekunder.

### Test av ESP32

**Test 14: ESP32-koden går att bygga**
Jag gjorde: klickade på Build i PlatformIO.
Jag väntade mig: att det skulle stå SUCCESS.
Det fungerade: ja.

**Test 15: ESP32:n skickar mätvärden hela vägen**
Jag gjorde: försökte skicka koden till kortet.
Jag väntade mig: att mätvärden från kortet skulle synas i API:et.
Det fungerade: nej. Jag fick felet No serial data received och kunde inte skicka koden till kortet.

## Sammanfattning

14 av 15 tester fungerade. Allt från brokern till API:et fungerar.