# Felsökning

## Vad är felsökning?

Felsökning betyder att hitta och laga fel. För att testa mitt system gjorde jag två fel med flit. Sedan tittade jag på om systemet klarade dem.

För varje fel skriver jag sex saker:

1. Vad jag såg
2. Hur jag hittade felet
3. Vilka verktyg jag använde
4. Varför felet hände
5. Vad jag gjorde
6. Hur jag visste att det fungerade igen

## Fel 1: Ett trasigt meddelande

Jag skickade ett meddelande som var fel skrivet. Den sista klammerparentesen } saknades. Det är som en blankett som inte är färdigifylld. Meddelandet ligger i filen tests/ogiltigt-meddelande.json.

**1. Vad jag såg**
Inget nytt mätvärde kom fram.

**2. Hur jag hittade felet**
Python-tjänsten skrev en varning. Och på sidan /health stod det att ett meddelande var trasigt.

**3. Vilka verktyg jag använde**
- Python-tjänstens logg. Loggen är texten som programmet skriver i terminalen. Där stod det: WARNING Ogiltig JSON mottagen.
- Sidan /health i webbläsaren. Där stod det: validation_errors: 1. Det betyder att ett meddelande var trasigt.

**4. Varför felet hände**
Meddelandet var inte rätt skrivet. Python kunde inte läsa det.

**5. Vad jag gjorde**
Jag hade redan skrivit koden så att den klarar trasiga meddelanden. Koden försöker läsa meddelandet. Om det inte går, skriver den en varning och hoppar över meddelandet. Den kraschar inte.

**6. Hur jag visste att det fungerade**
Programmet fortsatte att köra. När jag skickade ett rätt meddelande efteråt kom det fram som vanligt.

## Fel 2: Brokern försvinner

Brokern är som ett postkontor som skickar vidare meddelandena. Jag stängde av brokern medan Python-tjänsten var kopplad till den.

**1. Vad jag såg**
På sidan /health stod det: mqtt_connected: false. Det betyder att Python-tjänsten inte längre var kopplad till brokern.

**2. Hur jag hittade felet**
Jag använde ett kommando som heter netstat. Det visar vilka program som använder en viss port. Brokern använder port 1883.

**3. Vilka verktyg jag använde**
- Kommandot netstat. Det visade att brokern hade nummer 10668.
- Kommandot taskkill. Det stänger av ett program. Jag stängde av brokern med: taskkill /PID 10668 /F
- Python-tjänstens logg. Där stod det: Tappade anslutningen till brokern. Försöker igen automatiskt.
- Sidan /health. Där stod det: mqtt_connected: false.

**4. Varför felet hände**
Brokern var avstängd. Python-tjänsten hade ingen att prata med.

**5. Vad jag gjorde**
Jag startade brokern igen. Python-tjänsten är byggd för att själv försöka koppla in sig igen. Den behöver ingen hjälp.

**6. Hur jag visste att det fungerade**
Fem sekunder efter att jag startade brokern kopplade Python-tjänsten in sig igen, helt av sig själv. Det syntes i brokerns logg. På sidan /health stod det mqtt_connected: true igen.

## Andra fel som hände medan jag arbetade

**Fel lösenord**
Jag såg: not authorised. Det betyder "du får inte komma in".
Varför: lösenordet i min kod var inte samma som brokerns lösenord.
Vad jag gjorde: jag skrev in samma lösenord på båda ställena.

**Konstig radbrytning**
Jag såg: Invalid input, när jag skickade en fil.
Varför: filen var sparad på Windows-sättet, med en annan sorts radbrytning.
Vad jag gjorde: jag sparade filen på ett annat sätt, som heter LF.

**Konstiga tecken på skärmen**
Jag såg: konstiga tecken i stället för text.
Varför: ESP32:n och datorn pratade olika snabbt.
Vad jag gjorde: jag ställde in samma hastighet på båda, 115200.

**Kortet startade om hela tiden**
Jag såg: Brownout detector was triggered.
Varför: kortet fick inte tillräckligt med ström när Wi-Fi startade.
Vad jag gjorde: jag bytte till ett annat kort, ESP32-C6.

## Vad jag lärde mig

- jag fick ConnectionRefusedError flera gånger, fast brokern hade visat running. Det berodde på att den stängdes av i en av alla terminaler. Det löstes först när jag körde netstat och såg att inget lyssnade på port 1883.

- Fel lösenord. Det såg ut som att koden var fel, men det var bara en bokstav som skilde.

