# RDT-Analyse-Skill

Analysiert RDT-Dateien (Room Definition Table) aus Resident Evil 1.5 und gibt pro Sektion eine strukturierte Übersicht mit Name, Offset, Größe und Einträge-Anzahl aus.

## Kontext laden

Lade folgende Referenzdokumentation vor der Analyse:
- `RE15_KNOWLEDGE.md` §1.1 (RDT Format — Adresstabelle, Header-Felder, Sektionstypen)
- `re15_port/include/re15_rdt.h` (Struct-Definition mit 21 Sektions-Pointern)
- `re15_port/engine/src/rdt_common.c` (In-Place-Parser-Implementierung)

## Eingabe

- Pfad zu einer `.RDT`-Datei (z.B. `STAGE1/ROOM1170.RDT`)
- Alternativ: Stage + Room-Hex + Player (z.B. Stage 1, Room 17, Player 0)

## Analyse-Schritte

### 1. Header parsen (Offset 0x00–0x07)

Lese die 8 Header-Bytes und extrahiere:

| Byte | Feld         | Bedeutung                        |
|------|--------------|----------------------------------|
| 0x00 | nSprite      | Anzahl Sprite-Definitionen       |
| 0x01 | nCut         | Anzahl Kamera-Winkel (BSS-Chunks)|
| 0x02 | nOmodel      | Anzahl Objekt-Modelle            |
| 0x03 | nItem        | Anzahl Items im Raum             |
| 0x04 | nDoor        | Anzahl Tür-Definitionen          |
| 0x05 | nRoom_at     | Anzahl Raum-Attribute            |
| 0x06 | Reverb_lv    | Reverb-Level (Audio)             |
| 0x07 | Reserved     | Nicht verwendet                  |

### 2. Adresstabelle parsen (Offset 0x08–0x5C, 21 Einträge)

Lese 21 × 4 Bytes (Little-Endian u32) und erzeuge die Sektions-Übersicht:

| Index | Offset | Sektion      | Beschreibung                       |
|-------|--------|--------------|------------------------------------|
| 0     | 0x08   | snd0_edt     | Sound-Bank 0 EDT                   |
| 1     | 0x0C   | snd0_vh      | Sound-Bank 0 VH (VAG Header)       |
| 2     | 0x10   | snd0_vb      | Sound-Bank 0 VB (VAG Body)         |
| 3     | 0x14   | snd1_edt     | Sound-Bank 1 EDT                   |
| 4     | 0x18   | snd1_vh      | Sound-Bank 1 VH                    |
| 5     | 0x1C   | snd1_vb      | Sound-Bank 1 VB                    |
| 6     | 0x20   | collision    | SCA-Kollisionsdaten                |
| 7     | 0x24   | camera       | RID-Kameradefinitionen             |
| 8     | 0x28   | zone         | RVD-Trigger-Zonen                  |
| 9     | 0x2C   | light        | Beleuchtungsdaten                  |
| 10    | 0x30   | md1_ptr      | Modell-Pointer-Tabelle             |
| 11    | 0x34   | floor        | Bodensound-Regionen                |
| 12    | 0x38   | block        | (Unused — nicht parsen)            |
| 13    | 0x3C   | message      | MSG-Texttabelle                    |
| 14    | 0x40   | main_scd     | Haupt-SCD-Skript                   |
| 15    | 0x44   | sub_scd      | Sub-SCD-Skripte                    |
| 16    | 0x48   | extra_scd    | Extra-SCD (unused in RE1.5)        |
| 17    | 0x4C   | effect       | Effekt-Sprites (ESP)               |
| 18    | 0x54   | esp_tim      | ESP-Texturen                       |
| 19    | 0x58   | model_tim    | Modell-Texturen                    |
| 20    | 0x5C   | animation    | Animationsdaten                    |

### 3. Sektionsgrößen berechnen

Für jede Sektion mit Offset ≠ 0x00000000:
- **Größe** = Differenz zum nächsten nicht-Null-Offset (sortiert) bzw. Dateigröße für die letzte Sektion
- **Einträge-Anzahl** (wo berechenbar):
  - `collision`: Erste 4 Bytes der Sektion = Anzahl SCA-Einträge (u16 count + u16 floor)
  - `camera`: Berechne `size / sizeof(RID_entry)` (je 16 Bytes pro Kamera)
  - `zone`: Berechne aus RVD-Header (erste 2 Bytes = count)
  - `main_scd` / `sub_scd`: Pointer-Tabelle am Anfang → Anzahl Subs = erste u16 / 2
  - `message`: Erste 2 Bytes = Anzahl Nachrichten

### 4. Ausgabe-Format

Pro Sektion eine Zeile:

```
[Index] Sektion       Offset      Größe       Einträge  Status
──────────────────────────────────────────────────────────────────
[0]     snd0_edt      0x00012A40  4.2 KB      —         OK
[6]     collision     0x00000F60  2.1 KB      34        OK
[7]     camera        0x000017A0  256 B       16        OK
[12]    block         0x00000000  —           —         Nicht vorhanden
[14]    main_scd      0x0000A200  1.8 KB      3 Subs    OK
```

## Fehlerbehandlung

- **Null-Offset (0x00000000)**: Sektion als "Nicht vorhanden" kennzeichnen, weiter mit nächster Sektion
- **Offset überschreitet Dateigröße**: Sektion als "FEHLERHAFT" kennzeichnen mit Meldung `Offset 0xXXXXXXXX > Dateigröße (N Bytes)`, restliche Sektionen weiterverarbeiten
- **Datei zu klein (< 0x60 Bytes)**: Fehlermeldung "Ungültiger RDT-Header — Datei zu klein" ausgeben, keine Sektionsanalyse möglich
- **Unlesbare Sektion (Parse-Fehler bei Einträge-Zählung)**: Sektion mit `Einträge: ?` und Status "Parse-Fehler" kennzeichnen, restliche Sektionen trotzdem verarbeiten

## Beispiel-Aufruf

> Analysiere die RDT-Datei ROOM1170.RDT

Ergebnis:
```
RDT-Analyse: ROOM1170.RDT (Stage 1, Room 0x17, Player Leon)
Dateigröße: 287.4 KB
═══════════════════════════════════════════════════════════════

Header:
  nSprite=2, nCut=16, nOmodel=3, nItem=1, nDoor=4, nRoom_at=6, Reverb=2

Sektionen:
[Index] Sektion       Offset      Größe       Einträge  Status
──────────────────────────────────────────────────────────────────
[0]     snd0_edt      0x00045E00  128 B       —         OK
[1]     snd0_vh       0x00045E80  2.0 KB      —         OK
[2]     snd0_vb       0x00046680  32.5 KB     —         OK
[3]     snd1_edt      0x00000000  —           —         Nicht vorhanden
[4]     snd1_vh       0x00000000  —           —         Nicht vorhanden
[5]     snd1_vb       0x00000000  —           —         Nicht vorhanden
[6]     collision     0x00000F60  2.1 KB      34        OK
[7]     camera        0x000017A0  256 B       16        OK
[8]     zone          0x000018A0  448 B       14        OK
[9]     light         0x00001A60  192 B       —         OK
[10]    md1_ptr       0x00001B20  48 B        3         OK
[11]    floor         0x00001B50  320 B       —         OK
[12]    block         0x00000000  —           —         Nicht vorhanden
[13]    message       0x00001C90  512 B       8         OK
[14]    main_scd      0x00001E90  1.8 KB      3 Subs    OK
[15]    sub_scd       0x00002590  4.2 KB      12 Subs   OK
[16]    extra_scd     0x00000000  —           —         Nicht vorhanden
[17]    effect        0x000035E0  640 B       —         OK
[18]    esp_tim       0x00003860  8.0 KB      —         OK
[19]    model_tim     0x00005860  128.0 KB    —         OK
[20]    animation     0x00025860  131.0 KB    —         OK
═══════════════════════════════════════════════════════════════
Zusammenfassung: 16/21 Sektionen vorhanden, 0 fehlerhaft
```

## Weiterführende Referenzen

- Java-Parser: `src/main/java/de/re15/extractors/RDTExtractor.java:37-120`
- Sub-Parser: `CollisionParser.java`, `CameraParser.java`, `ZoneRvdParser.java`, `MSGParser.java`
- C-Engine-Parser: `re15_port/engine/src/rdt_common.c`
- Wissensbasis: `RE15_KNOWLEDGE.md` §1.1
