# ORM_Project
GRID Management TCP aplikacija – README

1. Opis projekta
Ovaj projekat implementira TCP klijent-server aplikaciju za upravljanje modelom
razvodnog postrojenja. Server sadrži analogne i digitalne module, dok klijent
omogućava izlistavanje modula i promenu njihovih vrednosti ili stanja putem
tekstualnog interfejsa.

------------------------------------------------------------

2. Kompajliranje programa
Za kompajliranje projekta potrebno je:

1) Otvoriti terminal u direktorijumu gde se nalazi projekat.
2) Pokrenuti sledeću komandu:

   make

Ovom komandom se kompajliraju fajlovi server.c i client.c i generišu izvršni
fajlovi:
- server
- client

------------------------------------------------------------

3. Pokretanje aplikacija

1) Pokretanje servera:
   
   ./server [broj_analognih_modula] [broj_digitalnih_modula]

   Ako se argumenti ne proslede, server automatski koristi podrazumevane
   vrednosti:
   - 10 analognih modula
   - 10 digitalnih modula

2) Pokretanje klijenta (u drugom terminalu):

   ./client

------------------------------------------------------------

4. Korišćenje aplikacije
Nakon pokretanja klijenta, prikazuje se tekstualni meni sa sledećim opcijama:

1) Izlistaj analogne module
2) Izlistaj digitalne module
3) Promeni analogni modul
4) Promeni digitalni modul
5) Prekini komunikaciju

Klijent šalje zahteve serveru, a server odgovara odgovarajućim podacima ili
potvrdom izvršene komande.

------------------------------------------------------------

5. Napomene
- Server i klijent komuniciraju preko IP adrese 127.0.0.1 i porta 12345.
- Komunikacija se ostvaruje korišćenjem TCP protokola, koji obezbeđuje pouzdanu
  i dvosmernu razmenu podataka.
- Preporučuje se korišćenje opcije 5 u meniju klijenta za pravilno zatvaranje
  konekcije.

------------------------------------------------------------
