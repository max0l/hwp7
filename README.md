# HWP7

## C++ programm zu versenden von Daten über das B15F Experminatal Board mittels Full-Duplex

### Überlegung

- Man hat 8 Leitungen, 4 für das senden 4 für das Empfangen
    - Beim Senden: 0b1234
    1 Bit: Daten oder Steuersignal -> 0b10xx (Kein Paritätsbit?)
    2 Bit: Paritätsbit (Gerade anzahl Einsen = 0, ungerade = 1)
    3: Daten
    4: Daten

### Abbildungstabelle
 Symbol | Beschreibung |
--------|--------------|
0b0**0**00  | Daten: 00
0b0**0**01  | nicht benutzt
0b0010  | nicht benutzt
0b0011  | Daten: 11
0b0100  | nicht benutzt
0b0**1**01  | Daten: 01
0b0**1**10  | Daten: 10
0b0111  | Nicht benutzt
0b**1**000  | Startzeichen
0b**1**001  | Paketnummer (Ein Paket = 8 Byte)
0b**1**010  | Doppelte zeichen zwischenzeichen
0b**1**011  | OK Steuerzeichen
0b**1**100  | 
0b**1**101  |
0b**1**110  | Stopzeichen
0b**1**111  | Übertragung Ende

##
Tisch 5: schwarz rechts
Tisch 6: schwarz links
wenn man davor sitzt