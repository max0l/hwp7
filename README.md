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
0b0000  | Daten: 00
0b0001  | nicht benutzt
0b0010  | nicht benutzt
0b0011  | Daten: 11
0b0100  | nicht benutzt
0b0101  | Daten: 01
0b0110  | Daten: 10
0b0111  | Nicht benutzt
0b1000  | Startzeichen
0b1001  | Paketnummer (Ein Paket = 8 Byte)
0b1010  | Doppelte zeichen zwischenzeichen
0b1011  | OK Steuerzeichen
0b1100  | 
0b1101  |
0b1110  | Stopzeichen
0b1111  | Übertragung Ende