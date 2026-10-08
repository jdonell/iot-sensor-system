# Säkerhet

## Vad handlar den här filen om?

Säkerhet betyder att skydda systemet så att ingen obehörig kan läsa, ändra eller förstöra något. Här skriver jag vad som kan gå fel, och vad jag har gjort för att skydda systemet.

## Vad kan gå fel?

1. **En främling kopplar in sig på brokern.** Då kan hen skicka falska mätvärden eller läsa alla meddelanden.
2. **Mina lösenord hamnar på GitHub.** Då kan vem som helst på internet se dem.
3. **En främling hämtar mätvärden från API:et.** Då får någon data som hen inte ska ha.
4. **Ett trasigt eller falskt meddelande kommer in.** Då kan programmet krascha eller spara konstiga värden.
5. **Någon tjuvlyssnar på nätverket.** Allt skickas som vanlig text, så det kan läsas.

## Vad jag har gjort

### Skydd 1: Brokern kräver lösenord

Brokern är som ett postkontor. Nu släpper den bara in den som har rätt användarnamn och lösenord.

Lösenordet sparas inte som vanlig text. Det sparas som en hash. En hash är en kod som man inte kan räkna tillbaka till lösenordet. Även om någon ser filen kan hen inte läsa lösenordet.

Jag testade det på tre sätt:
- Utan lösenord: jag kom inte in.
- Med fel lösenord: jag kom inte in.
- Med rätt lösenord: jag kom in.

### Skydd 2: Lösenorden laddas aldrig upp till GitHub

Alla lösenord ligger i egna filer:
- secrets.h har Wi-Fi-lösenordet och brokerns lösenord, för ESP32:n.
- credentials.py har brokerns lösenord och API-nyckeln, för Python-tjänsten.
- passwd är brokerns lösenordsfil.

En fil som heter .gitignore säger åt git att aldrig ladda upp de här filerna. Jag lade till dem i .gitignore innan jag skapade dem. Då kunde de aldrig följa med av misstag.

### Skydd 3: API:et kräver en nyckel

En API-nyckel är som ett lösenord för API:et. Utan rätt nyckel får man inget mätvärde. API:et svarar bara 401, som betyder "du får inte komma in".

Jag testade det:
- I webbläsaren, utan nyckel: jag fick inget mätvärde.
- Med curl och rätt nyckel: jag fick mätvärdet.

### Skydd 4: Varje meddelande kontrolleras

Python-tjänsten kontrollerar varje meddelande innan det sparas:
- Är meddelandet rätt skrivet?
- Finns allt med som ska finnas?
- Är temperaturen rimlig, mellan -40 och 85 grader?

Om något är fel sparas inte meddelandet. Programmet skriver en varning och fortsätter. Jag testade det med ett trasigt meddelande.

## Vad som fortfarande inte är säkert

- **Ingen kryptering.** Allt skickas som vanlig text och kan läsas av någon som tjuvlyssnar. Det skulle kunna lösas med TLS, som gör texten oläslig för andra.
- **Alla använder samma lösenord.** Om lösenordet läcker kan någon låtsas vara vilken del som helst. Det skulle kunna lösas genom att varje del får ett eget lösenord.
- **Alla får skicka överallt.** Den som är inloggad kan skicka på alla topics. Det skulle kunna lösas genom att varje sensor bara får skicka på sin egen topic.
- **Sidan /health är öppen.** Vem som helst i nätverket kan se hur systemet mår. Det skulle kunna lösas genom att kräva en nyckel även där.

## Egna tankar

- Till exempel att man lägger till filen i .gitignore innan man skapar den, eller att git kommer ihåg allt, så ett lösenord som laddats upp en gång är svårt att få bort.