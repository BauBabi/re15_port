# Runde 30 — Thema E1: Irons Diary, das DOKUMENT-System

Stand: master 437905cb (v0.8.15). Phase ERMITTLUNG — an `engine/`, `platform/`, `include/`,
`shared_assets/` wurde nichts geändert. Neu sind nur Werkzeuge, eine Messsonde und die
Ausgaben unter `build/r30_irons-diary-dokument/`.

**Fortsetzung (2026-09-28).** Der erste Durchgang brach am Sitzungslimit ab (Entwurf Stand
00:30, auf master 8d83a025). Dieser Durchgang hat den Entwurf per Stichprobe geprüft
(Abschnitt 2.5), die Abnahme am Artefakt nachgetragen (6.5), den Kontaktbogen unter dem
verlangten Namen erzeugt und fünf der neun „nicht belegt"-Punkte geschlossen (Aufrufkette
des Aufhebens, Tasten-Wiederholung, Grund hinter den Sprites, Blende, RDT-Gleichheit).
Zwischen 8d83a025 und 437905cb liegen 83 Dateien Dossiers, Sonden und Werkzeuge;
`git diff --stat 8d83a025 437905cb -- re15_port/engine re15_port/platform re15_port/include
re15_port/shared_assets` ist leer — die Port-Messungen des Entwurfs gelten unverändert.

Schwester-Thema (nicht hier): Welt-Prop auf dem Tresen und das Memory-Card-Item
(`irons-diary-welt`). Berührungspunkt ist allein die **Item-Id des Dokuments** (Abschnitt 5, S1).

Adressen in RAM-Schreibweise. RE2 = `info/re2leon/PSX.EXE` (Datei-Offset = RAM − 0x8000F800).
RE1.5-FILE-Schirm = `info/Re1.5/PSX/BIN/DEBUG.BIN`, Modul lädt @0x800C0000 (Datei-Offset =
RAM − 0x800C0000; md5 von `shared_assets/PSX/BIN/DEBUG.BIN` und `info/Re1.5/PSX/BIN/DEBUG.BIN`
gleich: c2c11aab8a0ae4c0c7fdb56fb33c3a74).

---

## Kurzfassung

1. **Die „vorinstallierten Texte" sind RE1.5-Originaldaten, keine Port-Erfindung.** Die
   FILE-Liste ist in RE1.5 eine STATISCHE Tabelle (Maske u16[3] @0x800c6c98 = `01 00 ff ff ff ff`),
   sie hat im ganzen Auslieferungsstand genau EINEN Leser (@0x800c72e8/f0) und KEINEN Schreiber.
   Gemessen zeigt der Port 21 Namen (1 + 10 + 10). Der Leser zeigt für JEDE Zeile denselben
   7-Seiten-Blob „Operation Report" @0x800ccd34 (fest adressiert @0x800c7614).
2. **RE1.5 hat kein Dokument-Aufheben** — aber den Satz dafür: Prompt-Skript 5
   „The ‹name› has been filed." liegt @0x800c506f und hat keinen einzigen Aufrufer. Das
   System ist unfertig → RE2 ist das Ziel (Beta→Retail).
3. **RE2 öffnet beim Aufheben SOFORT den Leser** (kein „Will you take"), hängt das Dokument an
   eine 24-Byte-Liste @0x800D4B68 (leer = 0xFF, beim Spielstart gefüllt @0x800682dc-f8), zeigt
   nach dem Schließen „The ‹name› has been filed" und räumt erst DANACH Flag, Zone und
   Weltmodell ab (@0x80072b0c-bfc). Die Liste liegt im Speicherblock (Offset 0x6C4 von 0x800D44A4).
4. **Die Töne sind schon da.** RE2 spielt im Leser Bank 4, Sätze 4/5/6/8. `CORE00.EDH` und
   `CORE00.VB` sind zwischen RE1.5 und RE2 **byte-gleich** (md5 9b0e0627… / cdcb61fb…). Zu
   übernehmen sind also nicht Samples, sondern die AUSLÖSE-STELLEN (Tabelle in 3.6).
5. **Die Bild-Ebene des Ports hat drei gemessene Fehler:** schwarzer Kasten um das Buch
   (7410 Texel mit CLUT-Farbe 0x0000 deckend gezeichnet), Titel erscheint zweimal
   (Seitenzuordnung), und der RE1.5-Text „Operation Report" steht mitten im RE2-Blatt.
   Dazu liegt sie an der falschen Stelle (32,−8 statt RE2s 25,30 / 100,60) und auf dem
   falschen Grund: RE2 stellt im Leser ein schwarzes Rechteck 320×240 hinter die Sprites
   (`FUN_8002bda8(2,0)` @0x80071d8c-94 und @0x8006cf78-80), der Port zeigt das blaue Blatt
   des RE1.5-Schirms.
6. **Der Schriftsatz ist aus den Originalseiten gewonnen:** 78 Zeichen, 22 832 Glyphen-Instanzen,
   davon 33 abweichend; ganzzahlige Metrik, die 698 von 1048 Originalzeilen pixelgenau nachsetzt.
   Umlaute und ß sind KONSTRUKTIONEN und so markiert. Prototyp: Titel + 17 Textseiten als PNG
   und als TIM im FILE-Format.
7. **Der Prototyp ist am Artefakt abgenommen** (Fortsetzung): die 17 TIM-Seiten wurden per
   Glyphenvergleich zurückgelesen — 427 von 427 Wörtern des Nutzertexts wortgleich, 8 von 8
   Daten am Kopf einer Seite, 0 Kernpixel ohne Glyphe. Kontaktbogen:
   `build/r30_irons-diary-dokument/kontaktbogen.png`.
8. **Zwei Absprachen sind vor dem Bau nötig** (S0): Thema F plant ebenfalls
   `RE15_SAVE_VERSION 9` mit einem anderen Feld, und die Zone des Schwester-Themas muss die
   Item-Id 0x48 tragen.
9. **Eine Aussage des Entwurfs war falsch und ist berichtigt:** RE2s Blätter-Pfeile sind
   nicht „ohne Farbe". Der Lader verschiebt die CLUT von `ST0.TIM` um 10 Zeilen (Wort 0x0a1b
   @0x80068588); richtig gelesen sind die Pfeile grün (0x1386 / 0x09c3 / 0x0060) und die
   Ende-Marke „EXIT" grau. Der Port-Plan ändert sich dadurch nicht — RE1.5s eigene Pfeile
   sind ebenfalls grün und stehen 2 Pixel daneben.

---

## 1. Symptom / Auftrag

`AUFTRAG.md` Abschnitt E, soweit er das Dokument-System betrifft:

> Der Hintergrund ist FILE08 […]. Der Header soll sein: Irons Diary. […] Zwischen den
> einzelnen Daten soll, wie auch in Resident Evil 2 geblättert werden. Außerdem möchte ich das
> du die Vorinstallierten Texte alle entfernst und die richtigen Sounds für die Textdokumente
> aus Resident Evil 2 übernimmst. Nach dem auflesen und zumachen, soll es vom Schreibtisch
> verschwinden.

---

## 2. MESSUNG im Port

### 2.1 Der FILE-Reiter heute

Echte Framedumps (`RE15_FRAMEDUMP`, beschleunigter Renderer, kein AUTOSHOT/SOFTWARE_RENDER):

| Abzug | Zustand |
|---|---|
| ![Liste](../../build/r30_irons-diary-dokument/port_heute_file_liste_F74.png) | `port_heute_file_liste_F74.png` — Listenseite 0 „Files": Zeile 0 „Chris' Diary", Zeilen 1–9 Unterstriche |
| ![Leser](../../build/r30_irons-diary-dokument/port_heute_leser_F84.png) | `port_heute_leser_F84.png` — Leser Seite 1/7: Titelkarte „Operation Report" |
| ![Leser mit Bild-Ebene](../../build/r30_irons-diary-dokument/port_heute_leser_RE15_DOC8_F84.png) | `port_heute_leser_RE15_DOC8_F84.png` — dasselbe mit `RE15_DOC=8`: RE2-Titel UND RE1.5-Text übereinander, Buch unten links in schwarzem Kasten |

Aufruf (aus `re15_port/build_r30_irons-diary-dokument/platform/pc`, PATH mit
`/c/msys64/mingw64/bin` voran, `MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'`):

```
RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT=mess/title.bmp RE15_PSELECT_AUTO=1 \
RE15_INV_SHOT=mess/x.bmp RE15_INV_FILE_SHOT=1 RE15_FRAMEDUMP="74:mess/file_list_F74.ppm" ./re15_pc.exe
... zusaetzlich RE15_INV_FILE_DOC_SHOT=1 RE15_FRAMEDUMP="84:mess/reader_F84.ppm" [RE15_DOC=8]
```

### 2.2 Womit die Liste gefüllt ist — aus den Bytes gelesen

Werkzeug `analysis/befunde_runde30/r30_diary_port_filelist_dump.py`, Ausgabe
`build/r30_irons-diary-dokument/port_heute_file_liste.txt`:

| Listenseite | Titel (Zeiger u32 @0x800c78e4) | Maske @0x800c6c98 | Basis-Id @0x800c7370 | sichtbar |
|---|---|---|---|---|
| 0 | „Files" (@0x800c78f0) | 0x0001 | 0x48 | Zeile 0 „Chris' Diary"; 0x49 „Operation Report", 0x4a–0x51 „File 3"…„File 10" verdeckt |
| 1 | „S.T.A.R.S. Files" (@0x800c78f6) | 0xffff | 0x52 | Albert Wesker, Jill Valentine, Chris Redfield, Barry Burton, Rebecca Chambers, Brad Vickers, Enrico Marini, E. Dewey/…, J. Frost/…, K. Sullivan/… |
| 2 | „Umbrella Files" (@0x800c7907) | 0xffff | 0x5c | Cerberus, Mutated Baboon, Mutated Alligator, Gargantuagator, Arachnophobia, Giant Spiders, Umbrella File 7/8/9, (leer) |

Rohbytes: Maske `01 00 ff ff ff ff` (Datei 0x06c98), Basis `48 52 5c` (Datei 0x07370),
Titelzeiger `f0 78 0c 80 f6 78 0c 80 07 79 0c 80` (Datei 0x078e4).

Das Dokument @0x800ccd34 beginnt `0e 00 27 00 2d 01 ff 01 fc 02 02 04 1a 05` = 7 Seiten
(u16[0] = 0x0e, `srl t0,t0,1` @0x800c7624); Seite 1 ist die Titelkarte „Operation Report",
Seiten 2–7 der Bericht („September 26th / The Raccoon Police Dept. was unexpectedly attacked
by zombies…" bis „Recorder: David Ford").

### 2.3 Messsonde

`re15_port/tests/unit/probe_r30_irons_diary_dokument.c` (Registrierung
`probes/r30_irons-diary-dokument.cmake`, reine Messung, kein `add_test`). Ausgabe
`build/r30_irons-diary-dokument/probe_messung.txt`:

```
[A] Listenseite 0:  1 Zeilen mit NAMEN,  9 Zeilen mit Unterstrichen
    Listenseite 1: 10 Zeilen mit NAMEN,  0 Zeilen mit Unterstrichen
    Listenseite 2: 10 Zeilen mit NAMEN,  0 Zeilen mit Unterstrichen
    SUMME vorinstallierte Namen: 21
[B] Liste 0/Zeile 0: 394 Ops   Liste 2/Zeile 7: 394 Ops -> BITGLEICH (der Leser kennt die Zeile nicht)
[C] Papier 128x256, H=144, Band 14336 Texel
    Texel mit CLUT-Farbe 0x0000 und Index != 0 (PSX: durchsichtig): 7410
    davon zeichnet der Port DECKEND SCHWARZ:                        7410
    Seitenzahl: Port zaehlt 6, RE2 blaettert 0..max_page = 5 Seiten
    FILE08 title_page gegen p00: 0 abweichende Pixel -> der Port zeigt den Titel ZWEIMAL
[D] Prototyp FILE25: Titelseite 256x144, Papier 128x256, 19 Dateiseiten (Titel + p00..p17)
    title_page gegen p00: 0 abweichende Pixel; Farben ausserhalb FILE08-CLUT[1..6,8]: 0
    FILE25_title_paper.TIM byte-gleich FILE08; Kopf+CLUT+Bildkopf der Textseite byte-gleich FILE08
```

### 2.4 Was die Bild-Ebene heute kann

`re2doc_common.c` lädt `FILE%02d_title_paper/title_page/p%02d_page.TIM` aus
`shared_assets/RE2/FILES/`, liest die Maße aus dem TIM-Kopf und liefert Pixel.
`re15_inv_render_pc_file_image` (inv_render_pc.c) blittet Textseite und Illustration;
`main.c` ruft sie bei offenem Leser (substate 2, item_state 3 oder 4..7) an fester Stelle
`(32,−8)`. Gewählt wird das Dokument nur über die Umgebungsvariable `RE15_DOC` — es gibt
keinen Spielpfad, der `re15_re2doc_select` ruft. Fünf Abweichungen von RE2, alle gemessen:

| # | Port heute | RE2 | Beleg |
|---|---|---|---|
| 1 | Illustration UNTER der Textseite bei (32, −8+H), also unten links | Illustration bei **(100,60)**, Textseite bei **(25,30)**, sie überlappen | 3.4 |
| 2 | Texel mit Index ≠ 0 und CLUT-Farbe 0x0000 deckend schwarz (7410 Stück) | durchsichtig | psx-spx „Texture Color 0000h"; FILE08-CLUT: 62 von 256 Einträgen sind 0x0000, Hintergrund-Index 195 |
| 3 | Leserseite 0 = title_page, 1 = p00, 2 = p01 … | Seite 0 = Titel, Seite p ≥ 1 = p‹p› | 3.3 |
| 4 | RE1.5-Textglyphen werden weiter gezeichnet | — (RE2 hat keinen Zeichenstrom) | Abzug 3 |
| 5 | Grund hinter den Sprites = der blaue RE1.5-FILE-Schirm | Grund **schwarz**, vollflächig | 3.4 „Grund" |

### 2.5 Stichprobe der Fortsetzung (2026-09-28)

Selbst nachgeschlagen, nicht aus dem Entwurf übernommen — alle Werte stimmen:

| # | Aussage des Entwurfs | nachgeschlagen | Ergebnis |
|---|---|---|---|
| 1 | Dokument-Zweig `sltiu v0,a3,0x68` | `info/re2leon/PSX.EXE` @0x80071bbc = `2ce20068`, @0x80071bc0 = `1040004f` (`beq v0,zero,0x80071d00`) | stimmt |
| 2 | Dokument-Nr = Id − 104 | @0x80071d00 `0c01a4b7` (`jal 0x800692dc`), @0x80071d04 `24e4ff98` (`addiu a0,a3,-104`) | stimmt |
| 3 | Liste 24 Plätze @0x800D4B68, leer 0xFF | @0x800692e0 `240600ff`, @0x800692e8 `24634b68`, @0x80069308 `2ca20018` | stimmt |
| 4 | Ton beim Öffnen Bank 4 / Satz 8 | @0x80071df0 `3c040408` (`lui a0,0x408`), @0x80071df4 `0c016e8a`; **Sprungziel** @0x8005ba30 `srl t1,a0,24` (Bank), @0x8005ba7c-80 `srl v0,a0,16` / `andi s7,v0,0xff` (Satz) | stimmt |
| 5 | RE1.5-Maske `01 00 ff ff ff ff` | `info/Re1.5/PSX/BIN/DEBUG.BIN` Datei 0x06c98 | stimmt |
| 6 | RE1.5-Basis `48 52 5c`, Titelzeiger | Datei 0x07370, Datei 0x078e4 = `f0 78 0c 80 f6 78 0c 80 07 79 0c 80` | stimmt |
| 7 | RE1.5-Leser adressiert den Blob fest | @0x800c7610-14 `3c09800d` / `2529cd34`; Blob Datei 0x0cd34 = `0e 00 27 00 2d 01 …` | stimmt |
| 8 | RE1.5 „has been filed." | Datei 0x0506f = `30 44 41 00 05 01 06 00 05 00 08 44 3d 4f 00 3e 41 41 4a 00 42 45 48 41 40 57 01 00` | stimmt |
| 9 | CORE00 byte-gleich | md5 EDH 9b0e0627500b50eaca5f8bc4124635d9, VB cdcb61fb58d9ebfcf3352757674f7a6e in beiden Bäumen | stimmt |
| 10 | DEBUG.BIN beider Bäume gleich | md5 c2c11aab8a0ae4c0c7fdb56fb33c3a74, 262 144 B | stimmt |
| 11 | Textseiten tragen keine STP-Texel | Index-Histogramm über 191 Seiten: nur 0–6, 8 und EIN Texel 15; STP-Farben liegen auf 7, 9–13 | stimmt |

Eine Aussage des Entwurfs war zu weit gefasst und ist in 3.4 berichtigt: „kein STP-Bit in
Gebrauch" gilt für die Textseiten aller Dokumente und für die Illustration von FILE08,
**nicht** für alle Illustrationen — FILE03/04/09 (6356 Texel) und FILE15/16/17 (26 176 /
28 678 / 26 176 Texel) tragen STP-Farben und werden von RE2 halbdurchsichtig gezeichnet.
Für das Diary (Illustration = FILE08) folgenlos.

---

## 3. ORIGINAL-MECHANISMUS

### 3.1 RE1.5: die Liste ist statisch, der Leser kennt kein Dokument

```
DEBUG.BIN  Zeilen-Zeichner
800c72e4  lui   t1,0x800c
800c72e8  addiu t1,t1,27800        ; 0x800c6c98  Maskentabelle
800c72ec  addu  t1,t1,t0           ; + Listenseite*2
800c72f0  lhu   s3,0(t1)           ; Maske
800c7300  ori   s2,zero,0x35       ; y der ersten Zeile
800c7304  andi  t0,s3,0x1
800c7308  bne   t0,zero,0x800c7320 ; Bit gesetzt -> Name, sonst Unterstriche

DEBUG.BIN  Leser
800c7610  lui   t1,0x800d
800c7614  addiu t1,t1,-13004       ; 0x800ccd34  DER Dokument-Blob, fest
800c7618  lhu   t0,0(t1)
800c7624  srl   t0,t0,1            ; Seitenzahl = u16[0] / 2 = 7
```

Adress-Suche (`r30_mips_dis.py --find-addr`) über PSX.EXE, DEBUG.BIN, STAGE1–6.BIN, TITLE.BIN:

* 0x800c6c98 (Maske): genau 1 Treffer, der Leser @0x800c72e8. **Kein Schreiber.**
* 0x800ccd34 (Blob): 4 Treffer, alle in DEBUG.BIN (@0x800c7128, @0x800c7544, @0x800c7614,
  @0x800c7768), alle lesend, alle ohne Index nach Zeile/Listenseite.

### 3.2 RE1.5: Item-Ids enden bei 0x47, FILE-Namen beginnen bei 0x48

| Beleg | Wert |
|---|---|
| `sltiu v0,v0,0x48` @0x8004a350, `ori v0,zero,0x47` @0x8004a358, `sb v0,0(v1)` @0x8004a35c (Status-Schirm klemmt die Id auf 0x47) | höchste Item-Id 0x47 |
| `DATA/ITEMALL.PIX` 86 400 B / 1200 B je Kachel | 72 Kacheln = Ids 0x00…0x47 |
| `ITEM/ITPS.ITP` 884 736 B / 0x3000 | 72 Bilder = Ids 0x00…0x47 |
| Basis-Id der ersten FILE-Zeile, u8 @0x800c7370 | 0x48 |
| Zensus `r30_diary_item_id_zensus.py`: 240 Räume, 164 `Item_aot_set` | höchste platzierte Id 0x47, **0 Platzierungen ≥ 0x48** |
| einziges `sltiu/slti …,0x48` in PSX.EXE | @0x8004a350 (die Klemme oben) — **kein Dokument-Zweig im Aufnahme-Pfad** |

RE2 zum Vergleich: genau ein `sltiu …,0x68` in der EXE, @0x80071bbc — das ist der Dokument-Zweig.

### 3.3 RE2: Liste, Aufheben, Seitenlader

**Liste** — 24 Byte @0x800D4B68, leer = 0xFF, Reihenfolge = Aufhebe-Reihenfolge.

```
Spielstart
800682dc  addiu a1,zero,24
800682e0  addiu v0,zero,255
800682e4  addiu a1,a1,-1
800682e8  lui   at,0x800d
800682ec  addu  at,at,a1
800682f0  sb    v0,19304(at)       ; 0x800d4b68 + a1 = 0xFF
800682f4  bne   a1,zero,0x800682e8
800682f8  addiu a1,a1,-1
(Rookie-Modus setzt danach Platz 0 = 23: addiu v0,zero,23 @0x8006836c, sb @0x80068374)

Anhaengen FUN_800692dc
800692e0  addiu a2,zero,255
800692e8  addiu v1,v1,19304        ; 0x800d4b68
800692ec  lbu   v0,0(v1)
800692f4  bne   v0,a2,0x80069304
80069300  sb    a0,0(v1)           ; erster freier Platz bekommt die Dokument-Nr
80069308  sltiu v0,a1,0x18         ; 24 Plaetze
```

**Speicherstand.** Das Karten-Overlay `COMMON/BIN/MEM_CARD.BIN` lädt @**0x801BFA18**
(gemessen: 19 der 31 `jal`-Ziele im Overlay treffen mit dieser Basis einen Funktions-Prolog,
die nächstbeste Basis 4). ⛔ `analysis/itembox_re2/re2-box-speicher.md` rechnet mit 0x801C0000;
alle dortigen Overlay-Adressen liegen um 0x5E8 zu hoch.

```
Speichern
801c0c00  lui   s6,0x800d
801c0c04  addiu s6,s6,18116        ; 0x800d46c4
801c0c48  addiu s0,s6,-544         ; s0 = 0x800d44a4 = Blockbasis
801c0c60  addu  a0,s0,zero
801c0c70  addiu v0,zero,2048
801c0c74  sw    v0,16(sp)          ; 5. Argument = 0x800 Byte
801c0c94  jal   0x801c3678
  801c3688  addu s2,a0,zero        ; s2 = Blockbasis
  801c3680  lw   s4,608(sp)        ; s4 = 0x800
  801c3784  addu a0,s1,zero        ; fd
  801c3788  addu a1,s2,zero        ; Puffer = 0x800d44a4
  801c378c  jal  0x800957f4        ; BIOS B0:0x35 write (Stub: addiu t1,zero,53 @0x800957fc)
  801c3790  addu a2,s4,zero        ; 0x800
Laden
801c0de8  lui   s0,0x800d
801c0dec  addiu s0,s0,17572        ; 0x800d44a4
801c0df8  jal   0x80010778         ; Wortkopie (lw/sw-Schleife)
801c0dfc  addiu a2,zero,1944       ; 0x798 Byte
```

0x800D4B68 − 0x800D44A4 = **0x6C4**; 0x6C4 + 0x18 = 0x6DC < 0x798. Die FILE-Liste wird also
gespeichert UND zurückgeladen.

**Aufrufkette des Aufhebens** (Fortsetzung; schließt den früheren offenen Punkt „wer ruft
`FUN_80071ba0`"). Es gibt keinen `jal 0x80071ba0`; die Funktion steht in einer Tabelle:

```
Zonen-Verteiler, Tabelle @0x800a73c4 (Basis-Bezug @0x8005111c), Eintrag 2 = Item-Zone
800a73cc: 80051884

FUN_80051884 (Handler der Item-Zone; a0 = Zonen-Datensatz. Ein Dokument ist fuer den
              Verteiler eine gewoehnliche Item-Zone - die Ausloesebedingung ist dieselbe)
80051884  lbu   a1,0(a0)            ; Item-Id aus dem Datensatz
800518a0  sb    a1,-30913(at)       ; 0x800e873f  Name fuer die Meldung
800518a8  sb    a1,16945(at)        ; 0x800d4231  aufzuhebende Item-Id
800518b0  sw    v1,-6696(at)        ; 0x800ce5d8 = die ausloesende Zone (v1 = [0x800ead94])
800518cc  lbu   v0,7(a0)
800518d4  andi  v0,v0,0x1
800518d8  bne   v0,zero,0x80051924  ; Bit 0 gesetzt: Umweg, s. unten
800518f0  addiu v1,zero,2
800518f8  sb    v1,23552(at)        ; 0x800d5c00 = 2  Status-Schirm-Art "Aufheben"
800518fc  addiu v1,zero,1
80051904  sb    v1,-3256(at)        ; 0x800df348 = 1  Status-Schirm anfordern
80051908  ori   a0,a0,0x8000
80051918  sw    a0,-1064(at)        ; 0x800cfbd8 |= 0x8000
80051924  addiu v1,zero,6
8005192c  sb    v1,-1027(at)        ; Umweg: 0x800cfbfd = 6, 0x800cfbfe = 0 (@0x80051934)
  Dieselben drei Schreiber (Art 2, Anforderung 1, Bit 0x8000) stehen ein zweites Mal
  @0x8003e254-84. Dass der Umweg dort ankommt, ist NICHT verfolgt (Abschnitt 8).

Status-Aufgabe FUN_800689bc, Verteiler
80068c6c  addiu s0,s0,-27796        ; 0x800a936c  Tabelle [Art][Phase], 3 Zeiger je Art
80068c70  lbu   v1,16(s2)           ; Art   = 0x800d5c00
80068c78..80                        ; Art * 12
80068c84  lbu   v1,0(s2)            ; Phase = 0x800d5bf0
80068c8c  sll   v1,v1,2
80068c94  lw    v0,0(v1)
80068c9c  jalr  v0
  Tabelle @0x800a936c: Art 0 = 8006a574/8006a774/80068cd4, Art 1 = 8006f900/8006fb64/80068cd4,
  Art 2 = **80071ba0**/80071e14/80068cd4, Art 3 = 8006efd8/8006f164/80068cd4,
  Art 4 = 8006f6a8/8006f8b4/80068cd4
```

In `FUN_80051884` steht kein `jal 0x8002fe38` — zwischen dem Betreten der Zone und dem
Dokument-Zweig liegt keine Abfrage. Damit ist belegt: **Aufheben öffnet den Leser sofort.**

**Aufheben** — `FUN_80071ba0`, Dokument-Zweig (Listing `re2_dis_80071ba0_aufnahme.txt`):

```
80071ba4  lbu   a3,16945(a3)       ; 0x800d4231 = aufzuhebende Item-Id
80071bbc  sltiu v0,a3,0x68
80071bc0  beq   v0,zero,0x80071d00 ; Id >= 0x68 -> Dokument
80071d00  jal   0x800692dc
80071d04  addiu a0,a3,-104         ; Dokument-Nr = Id - 0x68
80071d08  lui   a1,0x8007
80071d0c  addiu a1,a1,-11196       ; 0x8006d444 = Seitenlader
80071d10  srl   v1,v0,3            ; Platz/8 -> Reihe
80071d14  andi  v0,v0,0x7          ; Platz&7 -> Spalte
80071d1c  sb    v0,23554(at)       ; 0x800d5c02 Spalte
80071d28  sb    v1,23553(at)       ; 0x800d5c01 Reihe
80071d30  sb    zero,23555(at)     ; 0x800d5c03 Seite = 0
80071d40  sb    zero,23557(at)     ; 0x800d5c05 = 0  (Titel-Slot laden)
80071d44  jal   0x80031f6c         ; Ladeaufgabe 2 starten
80071d4c..5c                       ; warten, bis Aufgabe 2 fertig
80071d6c  addiu v0,zero,2327       ; 0x917 -> Illustration 0x801a0000
80071d70  jal   0x80076a40
80071d80  addiu v0,zero,2583       ; 0xa17 -> Textseite 0x801a8220
80071d84  jal   0x80076a40
80071d98  jal   0x80075fd0         ; Leser-Sprites anlegen
80071da0..ac a0=512 a1=-6144 a2=7 a3=1
80071db0  addiu v0,zero,312
80071db4  sh    v0,92(s1)          ; 0x800d5c4c: Textseite startet bei x = 312
80071dbc  sb    v0,0(s1)           ; 0x800d5bf0 = 1
80071dc0  addiu v0,zero,4
80071dc4  sb    v0,1(s1)           ; 0x800d5bf1 = 4  (Modus: Dokument-Leser)
80071dc8  addiu v0,zero,3
80071dcc  jal   0x8002c1a0         ; Blende
80071dd0  sb    v0,2(s1)           ; 0x800d5bf2 = 3  (Zustand: hereinfahren)
80071dd4..ec                       ; zeichnen (jal 0x800761b8), bis die Blende steht
80071df0  lui   a0,0x408
80071df4  jal   0x8005ba28         ; TON Bank 4 / Satz 8
```

Zwischen 0x80071d00 und 0x80071df8 steht **kein** `jal 0x8002fe38` (Nachrichten-Öffner): es
gibt für Dokumente keine „Will you take"-Abfrage. Die `jal`-Ziele dort sind vollständig:
0x800692dc, 0x80031f6c, 0x80031f94, 0x80032138, 0x80076a40 (2×), 0x8002bda8, 0x80075fd0,
0x8002c1a0, 0x800761b8, 0x8002c350, 0x8005ba28.

**Seitenlader** `0x8006d444`:

```
8006d46c  lbu   v0,19304(at)       ; Dokument-Nr aus der Liste
8006d474  lbu   v1,23557(v1)       ; 0x800d5c05
8006d480  lbu   a0,-25904(at)      ; 0x800a9ad0[doc] = erster Slot
8006d484  beq   v1,zero,0x8006d49c ; 5c05 == 0 -> Titel-Slot
8006d488  addiu v0,a0,1
8006d490  lbu   v1,23555(v1)       ; Seite
8006d498  addu  a0,v0,v1           ; sonst Slot = erster + 1 + Seite
8006d49c  addiu v0,zero,166        ; CD-Datei 166 = FILES.TIM
```

Folge: Seite 0 beim Öffnen = Titel-Slot (Illustration + Titelseite); beim ZURÜCKblättern auf
Seite 0 = Slot erster+1 = `p00`. Genau deshalb ist `p00` bei allen 25 Dokumenten
byte-gleich der Titelseite. Seite p ≥ 1 = `p‹p›`. Letzte Seite = `max_page`
(u16 @0x800AA144 + doc*4; FILE08: `04 00 70 00` → max_page 4, y_off 112, H 144).

### 3.4 RE2: der Leser — Zustände, Tasten, Lage

Aufnahme-Leser = Modus 4 der Tabelle @0x800a9c9c → `0x8007274c`, Sprungtabelle @0x80011dbc
(7 Einträge: 0x8007279c, 0x80072918, 0x80072990 ×2, 0x80072a34 ×2, 0x80072b0c).
Listing `re2_dis_8007274c_leser_aufnahme.txt`.

| Zustand | Bedeutung | Belege |
|---|---|---|
| 3 | Textseite fährt von x=312 herein | Tabelle rückwärts: `addiu v0,v0,-25369` @0x80072a1c (0x800a9ce7 − Zähler), `subu` @0x80072a30 |
| 0 | Lesen | max_page `lhu a0,-24252(at)` @0x800727c8 |
| 2 → 3 | vorwärts blättern: hinaus nach links, laden, herein von rechts | `sltiu v0,v0,0xb` @0x8007299c; Tabelle `lbu v1,-25380(at)` @0x80072a0c; laden `jal 0x80032138` @0x800729b8, `jal 0x80076a40` @0x800729d4; x=312 @0x800729dc-e0 |
| 4 → 5 | rückwärts blättern, gespiegelt | x = −262 `addiu v0,zero,-262` @0x80072a88; `addu` @0x80072af4 |
| 1 | Ende-Marke scharf (nach RECHTS auf der letzten Seite) | `addiu v0,zero,1` @0x800728a8, `sb` @0x800728ac |
| 6 | Schließen: Meldung abwarten, dann abräumen | 3.5 |

Fahrkurve u8[12] @0x800a9cdc = `01 02 04 06 08 0a 10 20 30 40 60 70` (dieselben 12 Byte liegen
noch einmal @0x800a9ac4 für den FILE-Schirm). 11 Schritte: hinaus Σ(Index 1..11) = 398,
herein Σ(Index 10..0) = 287 → 312 − 287 = **25** = Ruhelage.

Tasten. Steuerkreuz aus dem GEHALTENEN Rohwort 0x800CE304 (`lhu v1,8476(s2)` @0x8007286c,
s2 = 0x800cc1e8; Schreiber `sh a2,-7420(at)` @0x800393a4 mit a2 = aktuelles Pad-Wort):
RECHTS 0x2000 (@0x8007287c), LINKS 0x8000 (@0x800728c4). Bestätigen/Abbrechen aus dem
ENTPRELLTEN, über die Tastenbelegung gemappten Wort 0x800CE310 (`lw v0,8488(s2)`):
Abbrechen = Bit 13 (`andi v0,v0,0x2000` @0x80072828), im Ende-Zustand Bestätigen ODER
Abbrechen (`andi v0,v0,0x3000` @0x80072954). Belegungstabelle u16[16] @0x800a26a0,
Konfiguration 0: Index 12 = 0x40 (Kreuz), Index 13 = 0x10 (Dreieck).

**Tasten-Wiederholung** (Fortsetzung; schließt den früheren offenen Punkt „Gatter 0x800cfb74
Bit 31"). Das Steuerkreuz wirkt nur in Bildern, in denen Bit 31 von 0x800CFB74 steht
(`bgez v0,0x80072afc` @0x80072864, im Ende-Zustand @0x80072924). Das Bit ist der
Wiederhol-Puls der Pad-Routine:

```
80039364..70  v1 = (alt ^ neu) & neu            ; frisch gedrueckt (0x800ce300 / 0x800ce2fc)
8003937c  lw    a0,-1004(a0)       ; 0x800dfc14  Maske der wiederholenden Tasten
800393b0  and   v1,v1,a0
800393b4  beq   v1,zero,0x800393cc
800393c0  lbu   v0,-1000(v0)       ; 0x800dfc18  frisch gedrueckt -> Zaehler = Anlaufzeit
800393d0  lbu   v1,9988(v1)        ; 0x800a2704  Zaehler
800393d8  beq   v1,zero,0x80039414 ; abgelaufen -> Zaehler = 0x800dfc19 (Folgezeit), Puls
800393dc  and   v0,a2,a0           ; gehalten & Maske
800393e4  addiu v0,v1,-1           ; -> Zaehler - 1
800393f0..80039408                 ; 0x800cfb74 &= 0x7fffffff   (kein Puls)
80039430..38                       ; 0x800cfb74 |= 0x80000000   (Puls)

Status-Schirm setzt die Werte beim Eintritt
800689ec  ori   a0,zero,0xf01c     ; Maske: Steuerkreuz 0xf000 + 0x1c
800689f0  jal   0x80039464         ; sw a0 -> 0x800dfc14, sh a1 -> 0x800dfc18
800689f4  addiu a1,zero,1546       ; 0x060a: Anlaufzeit 0x0a = 10, Folgezeit 0x06 = 6
```

Gehaltenes RECHTS blättert also weiter: Puls im Bild des Drückens, dann nach 10
heruntergezählten Bildern, danach alle 6 + 1 Bilder. Während die Seite fährt
(Zustände 2–5), liest der Leser keine Tasten.

**Grund** (Fortsetzung; schließt den früheren offenen Punkt „was steht hinter den Sprites").
Beide Leser schalten den Bildgrund auf ein vollflächiges schwarzes Rechteck:

```
Aufnahme-Leser                         FILE-Schirm, Leser oeffnet        ... schliesst
80071d8c  addiu a0,zero,2              8006cf78  addiu a0,zero,2         8006cfe0  addiu a0,zero,2
80071d90  jal   0x8002bda8             8006cf7c  jal   0x8002bda8        8006cfe4  lui   a1,0x20
80071d94  addu  a1,zero,zero           8006cf80  addu  a1,zero,zero      8006cfe8  jal   0x8002bda8
                                                                         8006cfec  ori   a1,a1,0x808
FUN_8002bda8(a0 = Art, a1 = Farbe)
8002bdac  sb    a0,-684(at)        ; 0x800dfd54 = Art
8002bdb4  bne   a0,v0(=2),0x8002bdec
8002bdc0  lui   a2,0xf0 ; 8002bdc4 ori a2,a2,0x140
8002bdd0  sw    a1,140(v1)         ; Farbe          (je Zeichenpuffer, 2 Stueck, Schritt 152)
8002bdd4  sw    zero,144(v1)       ; x = 0, y = 0
8002bdd8  sw    a2,148(v1)         ; w = 0x140 = 320, h = 0xf0 = 240
Bildwechsel
8002bd0c  lbu   v1,628(s1)         ; s1 = 0x800dfae0 -> 0x800dfd54
8002bd14  bne   v1,v0(=2),0x8002bd3c
8002bd20  jal   0x8008fb34         ; SetTile (Laenge 3, Code 0x60 @0x8008fb34-44)
8002bd24  addiu a0,a0,136
8002bd34  jal   0x8008f918         ; AddPrim in die Ordnungstabelle
```

Farbe 0 = schwarz im Leser; 0x200808 (r 8, g 8, b 32) ist der Grund des Status-Schirms, auf
den der FILE-Schirm beim Schließen zurückstellt. Vor dem Öffnen fahren im FILE-Schirm die
Tafeln 14 Bilder lang hinaus (`sltiu v0,v0,0xe` @0x8006cf08) — im Leser steht außer Grund,
Illustration, Textseite und Pfeilen nichts auf dem Schirm.

**Blende** `FUN_8002c1a0(512, −6144, 7, 1)` @0x80071da0-cc (schließt den früheren offenen
Punkt): `a0 >> 8` = 2 ist die Mischart (@0x8002c1a8, `sb a2,4(s0)` @0x8002c1e8), `a0 & 0xff`
= 0 der Blendenplatz, a2 = 7 setzt r, g, b auf 255 (@0x8002c1dc-0x8002c230), a1 < 0 setzt
den Startpegel 0x8000 + a1 = 0x6800 (@0x8002c278-90). `FUN_8002c350` liefert das
Vorzeichenbit des Pegels (@0x8002c36c-74); die Schleife @0x80071dd4-ec läuft, bis es steht.
Das ist dieselbe Aufblende, mit der RE1.5 seinen Status-Schirm öffnet
(`FUN_800217b0(0x200,−0x1800,7,0)`, im Port der Blendenkanal 0) — nur das vierte Argument
ist 1 statt 0 (`sb a3,8(s0)` @0x8002c1f0; Bedeutung nicht verfolgt).

**Texturseiten und Lader** (Fortsetzung; schließt den früheren offenen Punkt „Inhalt der
DR_MODE-Primitive"). Der TIM-Lader `0x80076a40` legt Bild und CLUT nach dem Wort 0x800CFBF0:

```
80076a64  lbu   v1,-1040(v1)       ; 0x800cfbf0 = Seiten-Nr
80076a6c  sll   v0,v1,6            ; x = Nr * 64
80076a70  sltiu v1,v1,0x10
80076a80  addiu v0,v0,-1024        ; Nr >= 16: x -= 1024
80076a9c..a8                       ; y = (Nr >= 16) ? 256 : 0
80076b00  lbu   v0,-1039(v0)       ; 0x800cfbf1 = CLUT-Zeile
80076b08  addiu v0,v0,480
80076b0c  sh    v0,2(v1)           ; CLUT-Rechteck y = 480 + Zeile (x bleibt das des TIM)
```

| Aufruf | Wort | Bild nach | CLUT nach |
|---|---|---|---|
| Illustration, `addiu v0,zero,2327` @0x80071d6c | 0x0917 | (448,256) | (0,489) |
| Textseite, `addiu v0,zero,2583` @0x80071d80 | 0x0a17 | (448,256) — überschreibt die Zeilen 0…H−1 der Illustration | (0,490) |
| `ST0.TIM` Blatt 2 (Pfeile, „EXIT"), `addiu v0,zero,2587` @0x80068588 | 0x0a1b | (704,256) | (256,**490**…510) |

Genau deshalb tastet RE2 die Illustration ab v = H ab: im VRAM liegt über ihren ersten H
Zeilen die Textseite. Die drei Zeichenmodi setzt der Status-Schirm beim Eintritt mit
`SetDrawMode` (`0x800912ac`, Länge 2 @0x800912bc-d8):

| Primitiv (je Puffer +12) | tpage | bedeutet | Beleg |
|---|---|---|---|
| 0x800D6BA8 — vor der Illustration | 151 = 0x97 | 8bpp, (448,256), Mischart 0 | @0x8006872c-58 |
| 0x800D6C08 — vor der Textseite | 23 = 0x17 | 4bpp, (448,256), Mischart 0 | @0x8006878c-b8 |
| 0x800D6C20 — vor Pfeilen und Ende-Marke | 27 = 0x1b | 4bpp, (704,256), Mischart 0 | @0x800687bc-e8 |

Die Sprites tragen Code 0x66 (halbdurchsichtig + Textur). Gemischt werden auf der PSX nur
Texel, deren CLUT-Farbe das STP-Bit trägt. Die Textseiten ALLER Dokumente benutzen nur die
Indizes 0–6 und 8 (Histogramm über 191 Seiten: 0 = 6 858 778, 1…6 = 453 590, 8 = 781 327,
15 = 1 Texel), die STP-Farben 0x8000 liegen auf 7 und 9–13 → **kein Texel der Textseiten
wird gemischt.** Die Illustration von FILE08 trägt 0 STP-Texel (25 842 von 32 768 Texeln
sind 0x0000 = durchsichtig). Sechs andere Illustrationen tragen STP-Texel (FILE03/04/09,
FILE15/16/17) — für das Diary folgenlos.

Lage und Zeichenreihenfolge — `FUN_80075fd0` (Anlegen) und `FUN_800724b4` (Zeichnen):

```
FUN_80075fd0
8007603c  lhu   s3,-24250(at)      ; y_off
80076044  subu  s5,s1,s3           ; H = 256 - y_off
8007604c  addiu a1,zero,490        ; Textseite: CLUT (0,490)
80076050  addiu v0,zero,102        ; 0x66 SPRT
8007605c..64 sb s4(=128)           ; r=g=b=0x80
80076068  sb zero,-2(s0) ; 8007606c sb zero,-1(s0)   ; u=0 v=0
80076070  sh s1,2(s0)   ; 80076078 sh s5,4(s0)       ; w=256 h=H
800760ac  addiu a1,zero,489        ; Illustration: CLUT (0,489)
800760c0..c8 sb s1(=128)           ; r=g=b=0x80
800760d0  sb v0,-1(s0)             ; v = (-y_off)&0xFF = H
800760d4  sh s1,2(s0)   ; 800760dc sh s3,4(s0)       ; w=128 h=y_off
80076170  addiu v0,zero,25 ; 80076178 sh v0,23628(at) ; 0x800d5c4c = 25
8007617c  addiu v0,zero,30 ; 80076184 sh v0,23630(at) ; 0x800d5c4e = 30

FUN_800724b4
80072538  lhu v0,23628(v0) ; 80072540 sh v0,8(s0)    ; Textseite x
80072548  lhu v0,23630(v0) ; 80072554 sh v0,10(s0)   ; Textseite y
80072550  jal 0x8008f918                             ; AddPrim(Textseite)
80072584  addiu v0,zero,100 ; 80072588 sh v0,8(s0)   ; Illustration x = 100
8007258c  addiu v0,zero,60  ; 80072594 sh v0,10(s0)  ; Illustration y = 60
80072590  jal 0x8008f918                             ; AddPrim(Illustration)
```

`AddPrim` hängt vorn an → gezeichnet wird zuerst die Illustration, dann die Textseite darüber.
Die Illustration wird **auf jeder Seite** gezeichnet (kein Zustands-Gatter um den Aufruf
@0x80072590) und steht fest bei (100,60) — sie fährt beim Blättern NICHT mit.
Helligkeit: r=g=b=0x80 (neutral), und die FILE08-Illustrations-CLUT trägt in keinem ihrer
256 Einträge das STP-Bit → volle Deckung, keine Mischung.

Pfeile und Ende-Marke (alle bei y = 110):

| Element | Lage | Textur | Belege |
|---|---|---|---|
| Pfeil rechts | x = 282 + 3·Blink | u=42 v=12, 12×13, CLUT (256,492) | @0x80072630-70 |
| Ende-Marke „EXIT" | x = 280 | u=56 v=12, 42×14, CLUT (256,490); Helligkeit 48, im Zustand 1 128 | @0x800725c4-0x80072624, @0x80072678-84 |
| Pfeil links | x = 12 − 3·Blink, nur Zustand 0 und Seite ≠ 0 | u=28 v=12 | @0x800726b8-fc |

Die Marke steht im zweiten TIM von `COMMON/DATA/ST0.TIM` (@Datei 0x10820, 256×72 4bpp).

⛔ **Berichtigung der Fortsetzung.** Der erste Durchgang las die CLUT an ihrer Datei-Lage
(256,480) und fand für die Pfeil-Indizes 5/6/7 nur 0x0000 („Farben nicht belegt"). Der Lader
verschiebt den Block aber um 10 Zeilen (Wort 0x0a1b @0x80068588, Tabelle oben): VRAM-Zeile
490 + k ist Datei-Zeile k. Die Abzüge `re2_ST0_blatt2_clut490.png` / `…492.png` des ersten
Durchgangs zeigen deshalb die FALSCHEN Zeilen (Datei-Zeilen 10 und 12) und sind überholt.

| Sprite | CLUT | Datei-Zeile | Farben (Index → 15-Bit-Wert) |
|---|---|---|---|
| Pfeile | (256,492) | 2 | 5 → 0x1386 (r 6, g 28, b 4 von 31), 6 → 0x09c3 (3,14,2), 7 → 0x0060 (0,3,0) = **grün** |
| Ende-Marke | (256,490) | 0 | 5 → 0x56b5 (21,21,21), 7 → 0x39ce, 9 → 0x2529, 12 → 0x0c63 = **grau** |

Werkzeug `analysis/befunde_runde30/r30_diary_re2_pfeile.py`, Ausgabe
`re2_pfeile_und_endemarke.txt` und

![Pfeile und Ende-Marke](../../build/r30_irons-diary-dokument/re2_pfeile_und_endemarke.png)

RE1.5s eigener Leser-Pfeil ist ebenfalls grün und steht fast an derselben Stelle
(`emit_file_arrows`: x = 0x11c + Wippe = 284, y = 0x70 = 112, 16×16, uv(0x70,0x48); RE2:
x = 282 + 3·Blink, y = 110, 12×13). Die beiden Spiele unterscheiden sich hier um 2 Pixel.

### 3.5 RE2: Schließen — Meldung, dann Abräumen

```
Zustand 0, Abbrechen
80072830  addiu v0,zero,6  ; 80072834 sb v0,2(s0)    ; Zustand 6
80072838  lui a0,0xaf ; 8007283c ori a0,a0,0x10      ; Lage x=0x10 y=0xaf
80072840  ori a1,zero,0xe400
80072844  addiu a2,zero,10                           ; Meldung 10
80072848  jal 0x8002fe38
80072854  lui a0,0x405 -> 80072980 jal 0x8005ba28    ; TON Satz 5

Zustand 6
80072b10  lbu v0,-30916(v0)        ; 0x800e873c Nachrichten-Status
80072b18  andi v0,v0,0x80
80072b1c  bne v0,zero,0x80072c00   ; Meldung steht noch -> warten
80072b28  lw  v1,-6696(v1)         ; 0x800ce5d8 = die ausloesende Zone
80072b40  sb  zero,0(v1)           ; Zone aus
80072b8c  jal 0x8007730c           ; Aufgenommen-Flag setzen
80072b94  lbu a3,6(s1)             ; Modell-Platz der Platzierung
80072b9c  beq a3,v0(=255),...      ; 255 = kein Weltmodell
80072ba0..ac                       ; a3 * 0x1f8
80072bb0  sw  zero,16700(v0)       ; 0x800d0324 + a3*0x1f8 = 0  -> WELTMODELL WEG
80072bf0  lui a0,0x405 ; 80072bf4 jal 0x8005ba28     ; TON Satz 5
80072bfc  sb  zero,1(s0)           ; Modus 0 = zurueck ins Spiel
```

Meldung 10 (lateinische Bank, Basis 0x800A075C + u16[0x800A1E7C + 10·2]) @0x800A08B6:
`30 44 41 00 f9 01 f8 00 f9 00 fc 44 3d 4f 00 3e 41 41 4a 00 42 45 48 41 40 01 fe`
= „The ‹Farbe 1›‹NAME›‹Farbe 0› / has been filed".

**RE1.5 trägt denselben Satz**, Prompt-Skript 5 @0x800c506f:
`30 44 41 00 05 01 06 00 05 00 08 44 3d 4f 00 3e 41 41 4a 00 42 45 48 41 40 57 01 00`
= „The ‹Farbe 1›‹NAME›‹Farbe 0› / has been filed." — aber alle sechs `jal 0x80027e68` in
EXE/DEBUG/STAGE1–6 rufen mit a1=0x100 und a2 ∈ {0,1}, mit a1=0x300 (Raum-Meldungen,
andere Bank) oder a1=0x8400 (Beschreibungs-Bank 0x800c50de). Skript 5 ist nie verdrahtet.
Fortsetzung: die Suche ist auf Sprünge und Zeiger erweitert — in `PSX.EXE`, `DEBUG.BIN`,
`STAGE1`–`6.BIN` und `TITLE.BIN` steht kein `j 0x80027e68` und kein Datenwort 0x80027e68;
es bleibt bei den sechs `jal`. Die Lage der Meldung ist für die ganze Bank 0x100 in der
Funktion selbst festgelegt (`ori v0,zero,0x22` @0x80027eec → 0x800B8534, `ori v0,zero,0xb4`
@0x80027f14 → 0x800B8536), also (34,180) für jeden Aufrufer.
Ausgabe: `build/r30_irons-diary-dokument/meldung_has_been_filed.txt`.

### 3.6 Die Töne

Bank und Satz stecken im Aufrufwort: `FUN_8005ba28(a0)` nimmt `a0 >> 24` als Bank und
`(a0 >> 16) & 0xff` als Satz. Bank 4 = CORE.

| Datei | RE1.5 | RE2 |
|---|---|---|
| `SOUND/CORE00.EDH` md5 | 9b0e0627500b50eaca5f8bc4124635d9 | 9b0e0627500b50eaca5f8bc4124635d9 |
| `SOUND/CORE00.VB` md5 | cdcb61fb58d9ebfcf3352757674f7a6e | cdcb61fb58d9ebfcf3352757674f7a6e |

Sätze 4/5/6/8/10 sind darüber hinaus in allen Spiel-Bänken beider Spiele wellen- und
parametergleich (`core_baenke_vergleich.txt`; Ausnahmen: RE1.5 CORE0E, die Titel-Bänke
CORE10–13/15, in denen Satz 8 LEER ist). **Es gibt nichts zu importieren.**

Sample-Quelle je Satz (Fortsetzung, aus `CORE00.EDH`/`CORE00.VB` gelesen — 16 Sätze zu 4 Byte
ab Datei 0, `pBAV` @Datei 0x40, 12 Wellen; in RE2 und RE1.5 Wert für Wert gleich):

| Satz | EDH-Satz | Programm / Ton | Welle | `CORE00.VB` | md5 der Welle |
|---|---|---|---|---|---|
| 4 (bewegen, Ende erreicht) | @0x10 = `00 00 53 00` | 0 / 5 | 10 | @0x09130, 1360 B | 23d65788a0d2e2252927b41740137b4a |
| 5 (abbrechen, schließen) | @0x14 = `00 00 63 01` | 0 / 6 | 7 | @0x07560, 2944 B | c5cffbc9809e3ef89a36e5988561af16 |
| 6 (bestätigen) | @0x18 = `00 00 73 01` | 0 / 7 | 9 | @0x085e0, 2896 B | 59b5f14e45ff950309239f641be43eec |
| 8 (Seite, Dokument öffnet) | @0x20 = `00 00 93 00` | 0 / 9 | 6 | @0x05e50, 5904 B | b2de140fbc682ce0f1a54ab37877b3b1 |

| Ereignis | RE1.5 = Port heute | RE2 Aufnahme-Leser | RE2 FILE-Schirm |
|---|---|---|---|
| Dokument öffnet sich | Satz 6 @0x800c704c-54 (nur der Listen-Klick) | **Satz 8** @0x80071df0-f4, nach der Blende | Satz 6 @0x8006ce9c (Klick), dann **Satz 8** @0x8006cf58-70 |
| blättern vor/zurück | Satz 8 @0x800c71d4-dc / @0x800c7260-68 | Satz 8 @0x800728ec-f4 | Satz 8 @0x8006d1e0-e8 |
| RECHTS auf der letzten Seite | Satz 4 @0x800c71ec-f4 | Satz 4 @0x800728b0-b8 | Satz 4 @0x8006d1a4-ac |
| LINKS aus der Ende-Stellung | Satz 4 @0x800c722c-34 | **stumm** (@0x80072940-48, kein `jal`) | **stumm** (@0x8006d288-9c) |
| Abbrechen beim Lesen | Satz 5 @0x800c7170-78 | Satz 5 @0x80072854 | Satz 5 @0x8006d148 |
| Bestätigen in der Ende-Stellung | Satz 5 (fällt in @0x800c7170) | **Satz 6** @0x8007297c-84 | **Satz 6** @0x8006d248 |
| Abbrechen in der Ende-Stellung | Satz 5 @0x800c7170-78 (KREUZ wird vor der Seite geprüft, @0x800c712c-34) | **Satz 6** — dieselbe Abfrage `andi v0,v0,0x3000` @0x80072954 | **Satz 6** — `andi v0,v0,0x3000` @0x8006d214 |
| Meldung weg, zurück ins Spiel | — | Satz 5 @0x80072bf0-f8 | — |

### 3.7 Der FILE-Schirm von RE2 (nur zur Einordnung)

`0x8006c6e4`, 19 Zustände (Tabelle @0x80011c30): ein 3D-Karussell, 3 Reihen × 8 Spalten =
24 Plätze (`Reihe*8 + Spalte`). Leere Plätze lassen sich nicht öffnen
(`lbu v1,19304(at)` @0x8006ce68, `addiu v0,zero,255` @0x8006ce6c, `beq v1,v0` @0x8006ce70).
Der Name des Dokuments wird bei (16,203) gedruckt (@0x8006cec4-ec, Id = Dokument + 104).

---

## 4. URSACHE

| Symptom | Ursache |
|---|---|
| „Vorinstallierte Texte" | Der Port bildet RE1.5 byte-treu ab, und RE1.5 zeichnet die Liste aus einer festen Maske (3.1). Die 21 Namen sind RE1-Reste im Beta-Datenbestand. |
| Jede Zeile zeigt denselben Text | Der RE1.5-Leser adressiert den Blob fest (@0x800c7614); die gewählte Zeile geht nicht ein (Sonde B: bitgleich). |
| Aufheben eines Dokuments gibt es nicht | RE1.5 hat keinen Dokument-Zweig (3.2); im Port landet jede Item-Zone im Item-Modal `re15_item_modal_start`. |
| Schwarzer Kasten um das Buch | `re15_re2doc_pixel` prüft `idx == 0`, die PSX prüft die FARBE 0x0000. FILE08 malt den Hintergrund mit Index 195 → 0x0000. |
| Titel zweimal | `main.c` rechnet `pg = file_reader_page − 1` und `tile_name` bildet Seite 0 auf `p00` ab; RE2 lädt `p00` nur beim Zurückblättern (3.3). |
| Doppelter Text | `emit_file_reader` zeichnet den RE1.5-Blob unabhängig davon, ob ein Bild-Dokument gewählt ist. |
| Blauer Grund hinter dem Dokument | Die Bild-Ebene wird NACH der Anzeigeliste des RE1.5-Schirms gezeichnet (`main.c:4838-4863`) und bringt keinen eigenen Grund mit; RE2 setzt ein schwarzes Rechteck (3.4 „Grund"). |
| „Falsche" Töne | Die Wellen sind richtig. Es fehlen RE2s Ton beim Öffnen (Satz 8) und die Meldung samt Schlusston; zwei Stellen tragen einen anderen Satz (Tabelle 3.6). |

---

## 5. UMSETZUNGSPLAN für den Bau-Agenten

Grundsatz: **Was RE1.5 vollständig hat, bleibt RE1.5** — der Listen-Schirm (Zeilen, Titel,
Hervorhebung) und der Leser-Automat mit seiner Blätter-Animation (Zustände 3..7, Treiber
0x800c77bc, 28 px je Bild über 10 Bilder, Pfeile, Fußzeile). **Was RE1.5 nicht hat, kommt
aus RE2** — die dynamische Liste, die Zuordnung Zeile→Dokument, das Aufheben mit
Sofort-Leser, die Meldung, die zwei Sprites mit Lage und schwarzem Grund, RE2s Ton-Stellen.

Zeilennummern des Ports gelten für master 437905cb.

### S0 — Zwei Absprachen VOR dem ersten Edit

1. **Speicher-Version.** Thema F (`karten-marken.md` Abschnitt 5, Schritt 4 Punkt 3) hebt ebenfalls auf
   `RE15_SAVE_VERSION 9` und hängt `uint8_t visited_floor[16]` an. Beide Felder gehören in
   EINEN Versionssprung. Reihenfolge im Datensatz: erst `visited_floor[16]`, dann
   `files[24]`, dann `checksum` (gemessen mit einem Übersetzungslauf gegen `include/re15_savedata.h`:
   `sizeof(re15_savedata_t)` = 904, `offsetof(checksum)` = 900, Version 8; mit beiden
   Feldern 944 und Prüfwort bei 940, nur mit `files[24]` 928 und 924). Werden die
   Themen getrennt gebaut, nimmt das zweite v10 — zwei verschiedene Layouts unter derselben
   Nummer 9 dürfen nicht entstehen.
2. **Item-Id 0x48.** Das Schwester-Thema `irons-diary-welt` legt die Zone auf den Tresen;
   sie muss `item_type = 0x48` tragen (sein Beleg-Auszug `build/r30_irons-diary-welt/
   memorycard_belege.txt` führt 0x48 bereits als „Chris' Diary"). Die Memory Card ist Item
   0x21 und geht den gewöhnlichen Item-Weg.

### S1 — Liste und Dokument-Tabelle (neu: `include/re15_files.h`, `engine/src/re15_files.c`)

| Konstante | Wert | Beleg |
|---|---|---|
| Plätze | 24 | `sltiu v0,a1,0x18` @0x80069308 |
| leer | 0xFF | `addiu a2,zero,255` @0x800692e0; Füllen @0x800682dc-f8 |
| Anhängen | erster freier Platz, Rückgabe = Platz | @0x800692ec-0x80069314 |
| erste Dokument-Item-Id | 0x48 | RE1.5: u8 @0x800c7370 = 0x48, Klemme @0x8004a350-5c, 72 Kacheln; RE2-Vorbild `sltiu v0,a3,0x68` @0x80071bbc, `addiu a0,a3,-104` @0x80071d04 |

* `re15_files_reset()` beim Spielstart, `re15_files_add(doc)` (liefert den Platz),
  `re15_files_get(platz)`, `re15_files_export/import(uint8_t[24])`.
* Dokument-Tabelle, Eintrag 0 = **Irons Diary**:

  | Feld | Wert | Herkunft |
  |---|---|---|
  | Item-Id | 0x48 | oben |
  | Bild-Satz | 25 (`FILE25_*`) | erster freier Satz hinter RE2s 25 Dokumenten (`RE2_FILES_DOC_COUNT 25`, `gen/re2_files_toc.inc`) |
  | max_page | 17 | Prototyp, letzte Zeile von `FILE25_satz.txt`; entspricht RE2s u16 @0x800AA144 + doc·4 |
  | Seitenhöhe H | 144 | TIM-Kopf; entspricht FILE08 (`04 00 70 00` → y_off 112) |
  | Listenname | `25 4e 4b 4a 4f 00 20 45 3d 4e 55 07` = „Irons Diary" | Kodierung Code = ASCII − 0x24, Leerzeichen 0x00, Ende 0x07; belegt an „Chris' Diary" @0x800c4e04 = `1f 44 4e 45 4f 3a 00 20 45 3d 4e 55 07` |

  max_page steht AUSDRÜCKLICH in der Tabelle und wird nicht aus der Zahl der Dateien
  gezählt: `re15_re2doc_page_count` zählt bei RE2s Dokumenten 9, 23 und 24 mehr Dateien, als
  ihr max_page erlaubt (7 Dateien bei max_page 4; 16 bei max_page 2). Riegel: `p17` ist
  vorhanden, `p18` nicht.

### S2 — Speicherstand (`include/re15_savedata.h`, `engine/src/re15_savedata.c`)

Neues Feld `uint8_t files[24]` vor `checksum` (Lage gegen Thema F: S0.1). Hebe-Kette wie
bei v5/v6: ein älterer Stand lädt mit 24 × 0xFF. Beleg, dass RE2 die Liste speichert: 3.3
(Offset 0x6C4 im Block 0x800D44A4; geschrieben 0x800 Byte @0x801c0c70-94, geladen 0x798
Byte @0x801c0dec-fc).

### S3 — Liste dynamisch (`engine/src/re15_inv_screen.c:1259` `emit_file_list`, `engine/src/menu_common.c:1528` `file_mode`)

* Zeile r der Listenseite p zeigt Platz `p*10 + r`; belegt → Name aus der Dokument-Tabelle
  (a3 = 0 wie @0x800c7320-28), leer oder ≥ 24 → Unterstriche (a3 = 0x30 wie @0x800c7310-1c).
  `re15_inv_file_mask`, `re15_inv_file_rowbase` und `re15_inv_file_name_*` werden nicht
  mehr gelesen (die erzeugte `gen/inv_file_doc.inc` bleibt als Archiv der Originalbytes).
  Damit sind alle 21 vorinstallierten Namen und der Blob „Operation Report" aus dem Spiel.
* Alle drei Listenseiten tragen Titel 0 „Files" (@0x800c78f0). ⛔ PORT-ENTSCHEIDUNG: die
  Titel „S.T.A.R.S. Files"/„Umbrella Files" gehören zu den vorinstallierten RE1-Inhalten;
  RE2 kennt keine Kategorien. Die Seiten-Navigation (LINKS/RECHTS, 3 Seiten, Umlauf) bleibt
  byte-treu RE1.5.
* Zeilenwahl: VIERECK auf einem LEEREN Platz tut nichts, kein Ton
  (`lbu v1,19304(at)` @0x8006ce68, `beq v1,v0,0x8006cea0` @0x8006ce70 — der Zweig überspringt
  Anlegen UND Ton). RE1.5 prüft hier nichts, weil seine Liste fest war
  (`menu_common.c:1583-89`).
* Beim Öffnen: `re15_re2doc_select(bildsatz)`, beim Schließen `re15_re2doc_select(-1)`.
  Die Umgebungsvariable `RE15_DOC` (`platform/pc/main.c:4847-52`) bleibt als Ansehhilfe.

### S4 — Leser für Bild-Dokumente

| Stelle | Änderung | Beleg |
|---|---|---|
| `menu_common.c:1633` `int end = 7` | `end = max_page + 1` des gewählten Dokuments (Diary: 18) | RE1.5 liest die Zahl aus dem Dokument (@0x800c7124-30); RE2 `lhu a0,-24252(at)` @0x800727c8 |
| `re15_inv_screen.c:1217` `emit_file_reader` | bei Bild-Dokument KEINE Textglyphen; Fußzeile `Seite+1 / end` bleibt (RE1.5 @0x800c7744, y = 0xd2) | RE2-Seiten sind Bilder, kein Zeichenstrom |
| `re15_inv_screen.c:1233` `emit_file_arrows`: `end = RE15_INV_FILEDOC_PAGES` | `end` des Dokuments | @0x800c7544-50 |
| `re15_inv_screen.c:1665-1673` (FILE-Block in `re15_inv_screen_build`) | bei Bild-Dokument im Leser (Zustand 3..7) enthält die Anzeigeliste NUR Pfeile und Fußzeile und kehrt dann zurück — kein Rahmen, keine Tafeln, kein Hintergrundblatt (Muster: früher Rücksprung `build_box_mode` `:1635`) | RE2: Grund = schwarzes Rechteck, sonst nichts (3.4 „Grund") |
| `platform/pc/main.c:4861` `pg = file_reader_page − 1` | Seite 0 → Titelseite, Seite p ≥ 1 → `p‹p›`, Ende-Stellung (Seite = end) → letzte Seite | @0x8006d484-98; Klemme RE1.5 @0x800c7628-34 |
| `re2doc_common.c:143` `re15_re2doc_page_count` | entfällt für den Leser (S1: max_page aus der Tabelle); `p00` ist die Titel-Dublette | 3.3 |
| `re2doc_common.c:170` `if (idx == 0) return 0` | durchsichtig ist die FARBE 0x0000, nicht der Index 0 | psx-spx; Sonde C: 7410 Texel |
| `inv_render_pc.c:1166` `re15_inv_render_pc_file_image` + Aufruf `main.c:4863` | Reihenfolge: (1) **schwarzes Rechteck 320×240**, (2) Illustration bei **(100,60)**, 128 × (256−H), abgetastet ab v = H, (3) Textseite bei **(25,30)**, (4) erst DANACH die Anzeigeliste mit Pfeilen und Fußzeile | (1) @0x80071d8c-94, @0x8002bdc0-d8; (2) @0x80072584-94, @0x800760b8-dc; (3) @0x80076170-84 |
| Blättern (Zustände 4..7) | Textseite x = 25 + (`file_text_x` − 0x28); die Illustration bleibt stehen | Ruhelage RE1.5 a0 = 0x28 @0x800c6f94; RE2 Illustration fest (3.4) |
| Öffnen (aus der Liste UND beim Aufheben) | Einstieg über Zustand 7: `file_text_x = 320`, `file_anim_phase = 0`, Seite 0 — die Titelseite fährt von rechts herein | RE1.5-Zustand 7 @0x800c7868-78; RE2 x = 312 @0x8006cf68-74 (Liste) und @0x80071db0-b4 (Aufheben) |

RE2s eigene Fahrkurve (@0x800a9cdc) wird NICHT übernommen — RE1.5 hat das Blättern vollständig.
⛔ PORT-ENTSCHEIDUNG: RE2s 14 Bilder Tafel-Ausfahrt vor dem Öffnen aus der Liste
(`sltiu v0,v0,0xe` @0x8006cf08) entfallen; RE1.5s FILE-Schirm hat keine Tafeln, die ausfahren.

### S5 — Aufheben (`engine/src/aot_common.c`, `engine/src/menu_common.c`, `platform/pc/main.c`)

1. **Weiche.** Beide Item-Zweige rufen heute `re15_item_modal_start`
   (`aot_common.c:706` Sofortzündung, `aot_common.c:1374` Scan). Davor:
   `item_type >= 0x48` → Dokument-Pfad. Das Item-Modal darf für 0x48 nie starten — seine
   Bildquellen enden bei 0x47 (`ITPS.ITP` 72 Bilder, `ITEMALL.PIX` 72 Kacheln).
2. **Anforderung** nach dem Muster `re15_menu_request_box` (`menu_common.c:1835`):
   `re15_menu_request_doc(doc, taken_bit, aot_slot, taken_prop)` setzt `s_doc_target`,
   `s_latch = 1`, `s_stage = 1` und merkt sich die drei Abräum-Werte. Das ist der Weg, den
   RE2 geht: die Item-Zone fordert den Status-Schirm in der Art „Aufheben" an
   (@0x800518f0-0x80051918), eine Abfrage gibt es nicht.
3. **Eintritt** in der Phase 0 neben dem Box-Zweig (`menu_common.c:1699`):
   `re15_files_add(doc)`, `re15_re2doc_select(bildsatz)`, `s_substate = 2`,
   `item_state = 7`, `file_text_x = 320`, `file_anim_phase = 0`, `file_reader_page = 0`.
   Die Phase 0 wartet die Aufblende ab (`menu_common.c:1703`); im ersten Bild der Phase 1
   spielt Satz 8 (@0x80071dec-f4: erst Blende, dann Ton, dann fährt die Seite).
4. **Schließen** aus dem Aufnahme-Leser geht NICHT in die Liste (`item_state = 1`), sondern:
   * Meldung „The ‹Name› has been filed." = RE1.5-Prompt-Skript 5 @0x800c506f. Der Port
     führt es schon: `prompt_key_to_script` Schlüssel 7 → Skript [5]
     (`item_prompt_common.c:59`), bisher ohne Aufrufer. Lage (0x22, 0xb4) — RE1.5 setzt sie
     für die ganze Bank 0x100 fest in `FUN_80027e68` (`ori v0,zero,0x22` @0x80027eec,
     `ori v0,zero,0xb4` @0x80027f14), also unabhängig vom Aufrufer. Schreibmaschine und
     Bestätigen wie beim Skript „can't carry" (`item_modal_common.c:286-291`).
   * Der Name kommt aus der Dokument-Tabelle, NICHT aus der RE1.5-Namensbank: unter 0x48
     stünde dort „Chris' Diary" (@0x800c4e04). Laufzeit-Ersetzung am Namens-Opcode `06 00`
     in `re15_item_prompt_walk`, die erzeugte Tabelle bleibt byte-gleich.
   * Erst wenn die Meldung weg ist (RE2 wartet auf Bit 0x80 von 0x800E873C,
     @0x80072b10-1c): Zone inaktiv (@0x80072b40), Aufgenommen-Flag (@0x80072b8c),
     Weltmodell weg (@0x80072bb0), Ton Satz 5 (@0x80072bf0-f8), zurück ins Spiel
     (@0x80072bfc). Die drei Abräum-Schritte gibt es im Port schon im Item-Modal Zustand 7
     (`item_modal_common.c:340` `re15_game_flag_set(9,…)`, `:342` `slots[].active = 0`,
     `:360` `scd_prop_hide_by_obj_id`).
5. Aus der FILE-Liste geöffnet schließt der Leser wie bisher in die Liste, ohne Meldung.

### S6 — Töne (`engine/src/menu_common.c`)

Nur die fett gesetzten Zellen der Tabelle 3.6 ändern sich:

| Stelle im Port | heute | neu | Beleg |
|---|---|---|---|
| Leser öffnet aus der Liste (`:1584`) | `se4(6)` | `se4(6)`, im Folgebild `se4(8)` mit dem Hereinfahren — **in der Nachbesserung ersetzt: `se4(8)` 15 Bilder nach `se4(6)`, RE2s Zustand 11 (10.6)** | @0x8006ce9c, @0x8006cf58-70; Abstand `sltiu v0,v0,0xe` @0x8006cf08 |
| Leser öffnet beim Aufheben | — | `se4(8)` nach der Aufblende | @0x80071df0-f4 |
| LINKS aus der Ende-Stellung (`:1647`) | `se4(4)` | stumm | @0x80072940-48 |
| VIERECK in der Ende-Stellung (`:1641`) | `se4(5)` | `se4(6)` | @0x8007297c-84 |
| KREUZ in der Ende-Stellung (`:1637`, Seite = end) | `se4(5)` | `se4(6)` | `andi v0,v0,0x3000` @0x80072954, @0x8006d214 |
| KREUZ beim Lesen (`:1637`, Seite < end) | `se4(5)` | unverändert | @0x80072854, @0x8006d148 |
| RECHTS in der Ende-Stellung (`:1662`) | `se4(5)` + schließen | unverändert RE1.5 | @0x800c71ac; RE2 kennt den Fall nicht (Zustand 1 liest RECHTS nicht) |
| Meldung weg (nur Aufnahme) | — | `se4(5)` | @0x80072bf0-f8 |

Alle Sätze liegen in Bank 4 = `CORE00`, in beiden Spielen byte-gleich — es wird keine Welle
kopiert.

### S7 — Bilddaten

`build/r30_irons-diary-dokument/FILE25_*.TIM` (20 Dateien: `title_paper`, `title_page`,
`p00`…`p17`) nach `re15_port/shared_assets/RE2/FILES/`. Erzeugung jederzeit wiederholbar
und deterministisch (zweiter Lauf mit anderem Titel: alle 17 Textseiten und die
Illustration byte-gleich, es weichen nur `title_page` und `p00` ab):

```
python re15_port/tools/re2_doc_satz.py atlas --out build/r30_irons-diary-dokument
python re15_port/tools/re2_doc_satz.py satz  --out build/r30_irons-diary-dokument \
   --font build/r30_irons-diary-dokument/re2_doc_font.json \
   --text build/r30_irons-diary-dokument/nutzertext.txt --titel "IRONS DIARY" --doc 25 --vorlage 8
python analysis/befunde_runde30/r30_diary_satz_pruefung.py      # Abnahme am Artefakt (6.5)
python analysis/befunde_runde30/r30_diary_kontaktbogen.py       # kontaktbogen.png
```

Paket: `FILE25_*` muss unter `RE2/FILES/` neben der exe liegen (Suchpfade
`re2doc_common.c:103-104`). Android: die neue `engine/src/re15_files.c` erreicht
`libmain.so` nur nach frischem Configure (`app/.cxx` friert die GLOB-Liste ein).

### S8 — Riegel und Messung

* Die Sonde `probe_r30_irons_diary_dokument` wird zum Riegel: A = 0 Namen bei leerer Liste und
  genau 1 nach `re15_files_add(0)`; B = verschiedene Ops für zwei verschiedene Dokumente;
  C = 0 deckend gezeichnete 0x0000-Texel, Seitenzahl = max_page + 1; D unverändert.
* `test_inv_fsm.c` prüft die FILE-Töne schon über `g_test_core_se_last` — dort die Zeilen aus S6.
* Speicherstand: Rundlauf der neuen Version und Hebung des Vorgängers (Liste 24 × 0xFF).
* Aufheben: Zone mit `item_type 0x48` zünden → `re15_item_modal` bleibt in Zustand 0,
  Menü öffnet in substate 2 / item_state 7, Liste trägt Platz 0 = Dokument 0; nach
  Schließen + Bestätigen der Meldung: Flag gesetzt, Zone inaktiv, Prop versteckt — in DIESER
  Reihenfolge und erst NACH der Meldung.
* Bild: `RE15_FRAMEDUMP` des Lesers gegen `FILE25_p‹NN›_page_schirm.png` — verglichen werden
  die Pixel, an denen Textseite oder Illustration sichtbar sind, und der Grund muss außerhalb
  von Pfeilen und Fußzeile (0,0,0) sein.
* Satz: `r30_diary_satz_pruefung.py` gegen die nach `shared_assets` kopierten Dateien
  (Verzeichnis als Argument) — ERGEBNIS-Zeile „ALLE PRUEFUNGEN BESTANDEN".

---

## 6. Schriftsatz und Prototyp

Werkzeug `re15_port/tools/re2_doc_satz.py`, Bericht `atlas_bericht.txt`, Schrift `re2_doc_font.json`.

### 6.1 Gemessen

| Größe | Wert | Beleg |
|---|---|---|
| Zeilenraster | 16 px; H = 144 → 9 Zeilen | Bänder FILE08 p01: 4–11, 20–30, 35–45, … |
| Versalhöhe / x-Höhe | Zeilen +4…+11 / +7…+11 | Glyphen-Bitmaps |
| Papier | Index 0 (durchsichtig) | FILE08 p04: 36 864 von 36 864 Texeln Index 0 |
| Glyphenkern | Indizes 1–6 = 0x677b 0x5f39 0x4eb5 0x4231 0x35ce 0x294a | CLUT[0..8] auf allen 191 Seiten gleich |
| Kontur | Index 8 = 0x1063, 8er-Nachbarschaft des Kerns | 781 561 Soll-Texel, 277 abweichend, 44 Nichtnull ohne Kern |
| Glyphen | 78 Zeichen, 22 832 Instanzen, 33 tragen nicht die häufigste Bitmap | 1094 von 1130 Zeilen benutzt, 36 verworfen |
| Titelseiten | dieselben Glyphen: 437 von 437 pixelgleich | alle 25 Titel in Versalien, waagerecht mittig (x-Mitte 127,0–127,5) |
| Vorschub | ganzzahlig: die meisten Zeichen 9; i 5; J 6; l, j, I 7; Komma, Punkt, Apostroph 6; Leerzeichen 6 | je Zeichen mit Zählung im Bericht |
| Folgen | „.." Abstand 4 (38 von 38), „!!" Abstand 6 (2 von 2) | — |
| Gegenprobe | **698 von 1048** linksbündigen Originalzeilen pixelgenau nachgesetzt | — |
| Seitenrand FILE08 | p01: 12, p02: 10, p03: 10 → gewählt **10** | Modus der Stiftlage am Zeilenanfang |
| größtes Kernpixel-x FILE08 | 243 / 246 / 249 → Grenze **249** | — |
| Titel FILE08 | Rasterzeile 4 (y 64–79), mittig | Band y 67–75 |

![Glyphen-Atlas](../../build/r30_irons-diary-dokument/glyphen_atlas.png)

Rot unterstrichen = Konstruktion.

### 6.2 Konstruktionen (NICHT aus dem Original)

| Zeichen | gebaut aus | Änderung |
|---|---|---|
| ä | a | zwei Punkte, Index 1 (wie der i-Punkt), Zeile 5, Spalten 1 und 4 |
| ö | o | Zeile 5, Spalten 1 und 3 |
| ü | u | Zeile 5, Spalten 2 und 4 |
| Ä Ö Ü | A O U | Zeile 2, Spalten 2 und 4 |
| ß | Versal B | Zeilen 4, 7, 11: Kopf gerundet, Steg und Fuß vom Stamm gelöst |

Metrik jeweils die des Grundzeichens.

### 6.3 Satzregeln des Werkzeugs (keine Messung)

* Je Datum eine neue Seite; die Datumszeile steht in Zeile 0, der Text folgt DIREKT darunter
  (so FILE08 p01 „June 8th" und FILE16 p01). Die Leerzeile, die im Nutzertext zwischen Datum
  und Eintrag steht, entfällt. Leerzeilen INNERHALB eines Eintrags bleiben.
* Längere Einträge laufen auf Folgeseiten weiter; keine Leerzeile am Kopf einer Folgeseite.
* Keine Silbentrennung; Umbruch nur an Leerzeichen. Eine alleinstehende Punktfolge beginnt
  keine Zeile.
* Wortlaut und Schreibung des Nutzertexts unverändert (Kleinschreibung, „ausgesand", „such").

### 6.4 Ergebnis

Titel „IRONS DIARY" (x 83…171, Mitte 127,0) + **17 Textseiten**, `max_page` = 17, H = 144.
Seitenaufteilung mit jeder Zeile: `build/r30_irons-diary-dokument/FILE25_satz.txt`.

| Datum | Seiten |
|---|---|
| 18. September 1998 | p01 |
| 19. September | p02–p03 |
| 20. September | p04–p05 |
| 21. September | p06–p07 |
| 22. September | p08–p09 |
| 26. September | p10–p11 |
| 27. September | p12–p14 |
| 28. September | p15–p17 |

Seiten auf neutralem Grund:

![Kontaktbogen lesbar](../../build/r30_irons-diary-dokument/FILE25_kontaktbogen_lesbar.png)

Vorschau im RE2-Schirmlayout (Illustration bei 100,60, Textseite bei 25,30, Grund schwarz):

![Kontaktbogen Schirm](../../build/r30_irons-diary-dokument/FILE25_kontaktbogen_schirm.png)

Dateien: `FILE25_title_paper.TIM` (33 312 B, byte-gleich FILE08), `FILE25_title_page.TIM`,
`FILE25_p00_page.TIM` (= Titelseite), `FILE25_p01_page.TIM` … `FILE25_p17_page.TIM`
(je 18 496 B; Kopf, CLUT und Bildkopf byte-gleich FILE08). Noch NICHT nach `shared_assets`.

### 6.5 Abnahme am Artefakt (Fortsetzung)

Geprüft werden die geschriebenen TIM-Dateien, nicht das Protokoll des Satzwerkzeugs.
Werkzeug `analysis/befunde_runde30/r30_diary_satz_pruefung.py`, Ausgabe
`build/r30_irons-diary-dokument/satz_pruefung.txt`:

```
[1] Nutzertext AUFTRAG.md Abschnitt E gegen nutzertext.txt        ZEICHENGLEICH
[2] Ruecklesen von 17 Textseiten (Glyphenvergleich, Kern-Indizes 1..6)
    Kernpixel ohne Glyphe: 0   Abstaende ausserhalb der Metrik: 0
[3] Wortfolge: Nutzertext 427 Woerter, zurueckgelesen 427 Woerter  WORTGLEICH
[4] Daten im Nutzertext: 8   Seiten mit Datum am Kopf: 8   (p01 p02 p04 p06 p08 p10 p12 p15)
    Datum NICHT am Seitenkopf: 0
[5] Textseiten mit abweichender Groesse oder abweichendem Kopf: 0 von 17
    title_page / p00: Kopf+CLUT+Bildkopf byte-gleich FILE08; title_paper byte-gleich FILE08
    Titelseite zurueckgelesen: Rasterzeile 4 = 'IRONS DIARY'
ERGEBNIS: ALLE PRUEFUNGEN BESTANDEN
```

„Wortgleich" heißt: jedes Wort mit seiner Groß-/Kleinschreibung und seinen Satzzeichen, in
der Reihenfolge des Nutzers — auch „polizeikräfte", „ausgesand", „such", „aschpfahl", „daß".
Das Rücklesen erkennt die Umlaute und das ß als eigene Glyphen; kein Kernpixel blieb ohne
Zuordnung.

**Kontaktbogen** aller 18 Leserseiten (Titel + p01…p17), aus den TIM gelesen — oben auf
neutralem Grund, unten im RE2-Schirmlayout: `build/r30_irons-diary-dokument/kontaktbogen.png`
(2078 × 3580; Werkzeug `analysis/befunde_runde30/r30_diary_kontaktbogen.py`).

![Kontaktbogen](../../build/r30_irons-diary-dokument/kontaktbogen.png)

Gegenüberstellung Original gegen Prototyp (oben FILE08 p01, unten FILE25 p01, je 2×):

![Vergleich](../../build/r30_irons-diary-dokument/vergleich_FILE08_p01_gegen_FILE25_p01.png)

**Zweite Titelfassung.** `build/r30_irons-diary-dokument/variante_titel_gemischt/` trägt
denselben Satz mit dem Titel „Irons Diary" in gemischter Schreibung (x 84…170, Mitte 127,0).
Gegen die Hauptfassung weichen nur `FILE25_title_page.TIM` und `FILE25_p00_page.TIM` ab.

---

## 7. Risiken und offene Fragen

1. **Item-Id 0x48 muss mit dem Schwester-Thema übereinstimmen** (S0.2). Die Zone auf dem
   Tresen trägt diese Id; weicht sie ab, greift der Dokument-Zweig nicht.
2. **Speicher-Version** (S0.1). `karten-marken.md` plant dieselbe Nummer 9 mit einem anderen
   Feld. Belegt, nicht vermutet: dort Abschnitt 5, Schritt 4 Punkt 3, „`RE15_SAVE_VERSION 9`,
   Feld `uint8_t visited_floor[16]` direkt vor `checksum`".
3. **Titel in Versalien.** Der Nutzer schrieb „Irons Diary"; alle 25 RE2-Titelseiten sind
   Versalien (25 von 25 Transkriptionen, 437 von 437 Titel-Glyphen), die Listennamen gemischt
   („Chief's diary" @0x8009EAB1). Hauptfassung: Titelseite „IRONS DIARY", Listenname
   „Irons Diary". Die gemischte Titelseite liegt fertig daneben (6.5).
4. **17 Seiten, fünf davon mit 1–3 Zeilen** (p03: 2, p05: 3, p07: 1, p11: 3, p14: 1). Folge
   der 9 Zeilen je Seite; H = 176 (11 Zeilen) scheidet aus, weil die FILE08-Illustration die
   Zeilen 144–255 belegt. RE2 selbst führt Seiten dieser Art: FILE08 p04 ist ganz leer
   (36 864 von 36 864 Texeln Index 0) und wird trotzdem geblättert (max_page 4).
5. **Lesbarkeit.** RE2 zeichnet die Illustration in voller Helligkeit unter den Text; die
   Kontur macht ihn lesbar. Aus Code und CLUT belegt, an keinem laufenden RE2 nachgemessen.
6. **Schwarzer Grund im RE1.5-Schirm.** Der Leser für Bild-Dokumente zeigt nicht mehr das
   blaue Blatt des RE1.5-FILE-Schirms. Das folgt RE2 (3.4 „Grund"); wer den Schirm kennt,
   sieht einen Wechsel zwischen Liste (blau) und Leser (schwarz).
7. **Bestehende Wachen** (`test_inv_fsm.c`, `test_re2doc_bildebene.c`, die Zensus-Prüfung in
   `tools/gen_inv_file_doc.py`) halten den heutigen Stand fest und müssen mitgezogen werden.
8. **Android** friert die GLOB-Liste ein: die neue `engine/src/re15_files.c` fehlt sonst still
   in `libmain.so`.
9. **Paket:** `FILE25_*` muss im Paket unter `RE2/FILES/` liegen; der Lader sucht relativ zur exe.
10. **PSX-Ziel:** die Bild-Ebene ist heute nur in `platform/pc/` gebaut
    (`inv_render_pc.c`); das Dokument-System bleibt auf der PSX ohne Bild, bis dort ein
    Gegenstück steht.

---

## 8. Ausdrücklich NICHT belegt

Geschlossen in der Fortsetzung (standen im Entwurf hier): Farben der Pfeile und Inhalt der
DR_MODE-Primitive (3.4 „Texturseiten und Lader"), Grund hinter den Sprites (3.4 „Grund"),
Parameter der Blende (3.4 „Blende"), Gatter Bit 31 (3.4 „Tasten-Wiederholung"), Aufrufer
von `FUN_80071ba0` (3.3 „Aufrufkette"), Gleichheit der RDT-Bäume (240 von 240 `*.RDT` unter
`info/Re1.5/PSX` und `re15_port/shared_assets/PSX` md5-gleich).

Offen bleibt:

1. **Kein Bild vom laufenden RE2.** Lage, Reihenfolge, Helligkeit und Grund des Lesers sind
   aus Code, CLUT und Texeln belegt; ein RE2-Savestate oder -Framedump, an dem sich das
   Schirmbild nachmessen ließe, liegt nicht im Repo.
2. **Das vierte Argument der Blende** (1 im Leser, 0 in RE1.5s Status-Schirm;
   `sb a3,8(s0)` @0x8002c1f0). Der Port benutzt seine vorhandene Status-Aufblende.
3. **Der Umweg der Item-Zone** bei gesetztem Bit 0 von Byte +7 (0x800cfbfd = 6,
   @0x80051924-34): was die Zahl steuert und dass der Weg bei den Schreibern @0x8003e254-84
   ankommt, ist nicht verfolgt. Für den Port folgenlos — seine Item-Zone zündet auf ihrem
   eigenen, schon gebauten Weg.
4. **Warum 350 der 1048 Originalzeilen um 1–3 px von der Metrik abweichen.** Die Abweichungen
   sitzen überwiegend an Leerzeichen (5–9 statt 6) und als einzelne −1 zwischen Buchstaben.
   Ob Blocksatz, Handarbeit oder ein Satzprogramm dahintersteht, ist nicht ermittelt.
5. **Linkslage von K** (ein einziger Beleg, hinter einem Leerzeichen) — 0 gesetzt in Analogie
   zu den 19 anderen 7 breiten Versalien. **Vorschub von ?** (steht im Original nur vor
   Zeilenende) — 9 in Analogie; im Diary folgenlos, das ? steht am Absatzende.
6. **Umlaute und ß** sind Konstruktionen (6.2). RE2 (US) führt keine; eine deutsche
   RE2-Fassung, an der sie sich messen ließen, liegt nicht im Repo.
7. ~~**Tonabstand beim Öffnen aus der Liste.**~~ **Geschlossen in der Nachbesserung (10.6):**
   RE2s Zustand 11 selbst disassembliert — Satz 8 fällt frühestens 15 Bilder nach Satz 6
   (`sltiu v0,v0,0xe` @0x8006cf08 plus Ladeprüfung @0x8006cf14); der Port lädt synchron und
   hält genau 15 Bilder, im laufenden Spiel gemessen (Satz 6 in F540, Satz 8 in F555).

---

## 9. Artefakte

| Pfad | Inhalt |
|---|---|
| `re15_port/tools/re2_doc_satz.py` | Atlas + Schriftsatz + TIM-Ausgabe |
| `re15_port/tests/unit/probe_r30_irons_diary_dokument.c`, `probes/r30_irons-diary-dokument.cmake` | Messsonde |
| `analysis/befunde_runde30/r30_mips_dis.py` | Disassembler für beliebige Ladeadresse + Adress-Suche |
| `analysis/befunde_runde30/r30_diary_port_filelist_dump.py` | FILE-Liste und Blob aus DEBUG.BIN |
| `analysis/befunde_runde30/r30_diary_meldung_filed.py` | „has been filed" in beiden Spielen + Aufrufer-Zensus |
| `analysis/befunde_runde30/r30_diary_core_baenke.py` | Vergleich der CORE-Bänke |
| `analysis/befunde_runde30/r30_diary_item_id_zensus.py` | platzierte Item-Ids aller Räume |
| `analysis/befunde_runde30/r30_diary_satz_pruefung.py` | Fortsetzung: Abnahme am Artefakt (Rücklesen der TIM) |
| `analysis/befunde_runde30/r30_diary_kontaktbogen.py` | Fortsetzung: Kontaktbogen aus den TIM |
| `analysis/befunde_runde30/r30_diary_re2_pfeile.py` | Fortsetzung: Pfeile und Ende-Marke mit verschobener CLUT |
| `build/r30_irons-diary-dokument/re2_dis_*.txt` | 12 Disassembly-Listings |
| `build/r30_irons-diary-dokument/FILE25_*` | Prototyp: 20 TIM, PNG, `_lesbar`, `_schirm`, `FILE25_satz.txt` |
| `build/r30_irons-diary-dokument/kontaktbogen.png` | Kontaktbogen aller 18 Leserseiten |
| `build/r30_irons-diary-dokument/satz_pruefung.txt` | Ergebnis der Abnahme |
| `build/r30_irons-diary-dokument/variante_titel_gemischt/` | zweite Titelfassung „Irons Diary" |
| `build/r30_irons-diary-dokument/re2_pfeile_und_endemarke.{png,txt}` | RE2-Pfeile, richtig eingefärbt |
| `build/r30_irons-diary-dokument/vergleich_FILE08_p01_gegen_FILE25_p01.png` | Original gegen Prototyp |
| `build/r30_irons-diary-dokument/port_heute_*` | Abzüge und Dekodierung des heutigen Stands |
| `build/r30_irons-diary-dokument/re2_ST0_blatt2_clut49{0,2}.png` | ⛔ überholt (falsche CLUT-Zeilen, s. 3.4) |

---

## 10. UMSETZUNG (Bau, Zweig worktree-wf_b4b268f3-d12-4)

Stand: aufgesetzt auf master d98e9639. Commits: 946c1978 (S7 Seitenbilder), 27857b80 (WIP des
ersten Bau-Agenten, ungeprüft gesichert), 1c9b8768, 9b1a83b1, 0031e8c9, 4a6d947b und dieser
Nachtrag (Fortsetzung). Die Fortsetzung hat den WIP gebaut (0 Fehler) und die Suite gefahren,
BEVOR sie weiterschrieb: 358 von 360 grün, rot nur `unit_inv_fsm` und `unit_r26_inventar` (beide
hielten den alten Stand fest). Jetzt **362 von 362** (master 360 + `unit_r30_irons_diary_dokument`
+ `unit_r30_irons_diary_ablauf`).

### 10.1 Ausgangszustand — selbst nachgemessen

Eigener Bau von master d98e9639 (Sparse-Arbeitsbaum im Notizverzeichnis), die Sonde von master
unverändert gefahren (im Arbeitsbaum:
`build/r30_irons-diary-dokument/umsetzung/sonde_ausgangszustand_master_d98e9639.txt`):

| Größe | master d98e9639 | jetzt |
|---|---|---|
| vorinstallierte Namen in der FILE-Liste | 21 (1 + 10 + 10) | 0 bei leerer Liste, 1 nach `re15_files_add(0)` |
| Leser, Liste 0/Zeile 0 gegen Liste 2/Zeile 7 | 394 Ops, BITGLEICH | Bild-Dokument: 5 Ops (Fußzeile 4 + Pfeil 1), zwei Dokumente verschieden |
| CLUT-Farbe 0x0000 bei Index ≠ 0, deckend gezeichnet | 7410 von 7410 | 0 von 7410 |
| Seitenzahl | Port 6 (FILE08), Titel zweimal | Diary 18 = max_page 17 + 1; Seite 0 = Titelseite, p ≥ 1 = p‹p› |
| `sizeof(re15_savedata_t)` / Prüfwort / Version | 904 / @900 / 8 | 944 / @940 / 9 |

Tragende Adressen vor dem ersten Edit selbst disassembliert (Sprungziel, richtige Datei):
RE2 `FUN_800692dc` @0x800692dc-0x80069318 (Rückgabe = Platz; voll → 0 aus `sltiu` @0x80069308),
@0x80071bbc/@0x80071d04, @0x80071d8c-94, @0x80071df0-f4, @0x80072830-54, @0x800728a8-f4,
@0x80072918-88, @0x80072b0c-bfc, @0x80076170-84, @0x80072584-94, @0x8002bda8-dd8,
@0x8006ce68-9c, @0x8006cf58-70; RE1.5 DEBUG.BIN Maske/Basis/Titel/Skript 5/„Chris' Diary"/Blob
(Dateioffsets wie 2.5), @0x800c7610-24, @0x800c7868-80, @0x800c6f94, @0x800c704c-74;
RE1.5 PSX.EXE `FUN_80027e68` Bank 0x100 → (0x22, 0xb4) @0x80027eec/@0x80027f14, Schreibmaschine
@0x800281a0-38. Alle Werte wie im Plan.

### 10.2 Gebaut, je Plan-Schritt

| Schritt | Stand | Wo |
|---|---|---|
| S0.1 Speicher-Version | erledigt: Vertrag v9 wortgleich (Felder, Namen, Feld-Kommentare, Hebung in EINEM Schritt, fremdes Feld mit `/* R30-VERTRAG: fremdes Feld */` in Erfassung, Hebung, Wiederherstellung) | `include/re15_savedata.h`, `engine/src/re15_savedata.c` |
| S0.2 Item-Id 0x48 | erledigt: `RE15_FILES_FIRST_ITEM_ID 0x48` | `include/re15_files.h` |
| S1 Liste + Tabelle | erledigt | `include/re15_files.h`, `engine/src/re15_files.c`; Rücksetzen beim neuen Spiel `re15_gameflow.c` |
| S2 Speicherstand | erledigt; Hebung v7/v8 → v9 (v7 verwirft wie bisher die Besucht-Bits) | `re15_savedata.c` |
| S3 Liste dynamisch | erledigt; alle drei Listenseiten „Files"; leerer Platz öffnet nicht, kein Ton | `re15_inv_screen.c` `emit_file_list`, `menu_common.c` `file_mode` |
| S4 Leser für Bild-Dokumente | erledigt; Unterlage schwarz → Illustration (100,60) → Textseite (25,30) → RE1.5-Pfeile/Fußzeile; Durchsicht = FARBE 0x0000 | `re15_inv_screen.c`, `inv_render_pc.c`, `re2doc_common.c`, `main.c` |
| S5 Aufheben | erledigt; Weiche in BEIDEN Item-Zweigen VOR `re15_item_modal_start` | `aot_common.c` `aot_item_dokument`, `menu_common.c` |
| S6 Töne | erledigt (Tabelle 3.6, fett gesetzte Zellen) | `menu_common.c` |
| S7 Bilddaten | erledigt (946c1978), 20 TIM byte-gleich der Bau-Ausgabe (md5 je Datei) | `shared_assets/RE2/FILES/FILE25_*` |
| S8 Riegel | erledigt | s. 10.4 |

**SCHNITTSTELLE für die Folgespur irons-diary-welt** (so gebaut):

```c
void re15_menu_request_doc(int doc, int taken_bit, int aot_slot, int obj_id);   /* re15_menu.h */
```

fordert den Leser an (doc = Nummer der Dokument-Tabelle, 0 = Irons Diary = Item-Id 0x48). Der
Leser öffnet ohne Abfrage auf der Titelseite; das Dokument hängt da schon an der Liste. Nach dem
Schließen steht „The Irons Diary has been filed." (RE1.5-Skript 5 @0x800c506f, Lage (0x22,0xb4));
ERST nach dem Bestätigen, im selben Bild und in RE2s Reihenfolge: AOT-Slot `aot_slot` inaktiv
(@0x80072b40), Flag (9, `taken_bit`) gesetzt (@0x80072b8c), Prop `obj_id` ausgeblendet
(`scd_prop_hide_by_obj_id`, @0x80072bb0), Ton Satz 5 (@0x80072bf0-f8), dann schließt das Menü.
`taken_bit <= 0` = kein Flag, `aot_slot < 0` = keine Zone, `obj_id < 0` oder 0xFF = kein Modell.
Im Aufnahme-Pfad (`aot_common.c`, Sofortzündung `re15_aot_fire_slot` UND Scan `re15_aot_scan`)
zweigt jede Item-Zone mit `item_type >= 0x48` VOR `re15_item_modal_start` dorthin ab; für eine
Id ≥ 0x48 ohne Tabelleneintrag geschieht nichts (das Item-Modal startet für ≥ 0x48 nie).
Messen ohne Welt-Prop: `RE15_DOC_REQUEST="<bild>[:<doc>[:<taken_bit>[:<aot_slot>[:<obj_id>]]]]"`
(dazu `RE15_PAD_AT`, `RE15_DOC_LOG`, `RE15_DOC_EXIT_AT`; alle nur Messhaken in `main.c`, die
Ausgabe landet in `debug.log` neben der exe).
Hinweis an die Folgespur: RE2 räumt bei Bit 0x80 von Platzierungs-Byte 7 zusätzlich an das
Weltmodell gebundene Effekte ab (`FUN_8001cefc(5, …)` @0x80072bb4-bec, Tabelle 0x800d8cf0,
0x60 × 0x7c, Löschen @0x8001cf14-4c); die Port-Schnittstelle trägt dieses Byte nicht. Hängt
irons-diary-welt dem Prop einen Effekt an, muss der dort mit weg.

### 10.3 Abweichungen vom Plan und Berichtigungen des WIP

1. **Abräum-Reihenfolge.** Der WIP setzte Flag, Zone, Weltmodell. Selbst disassembliert ist RE2s
   Folge Zone (@0x80072b40, Verzögerungsplatz), Flag (`jal 0x8007730c` @0x80072b8c), Weltmodell
   (@0x80072bb0). Berichtigt (4a6d947b); gemessen über die neue Messschiene
   `re15_menu_doc_trace_folge`.
2. **Riegel B der Sonde** verglich den alten Textleser auf Seite 0 mit der auf Seite 1
   gemessenen 394 (die Sonde von master misst mit `file_reader_page = 1`). Jetzt Seite 1; 394.
3. **Port-Wahlen** jetzt wörtlich „Port-Wahl, keine Original-Adresse" mit Messung: Bild-Satz 25,
   max_page 17 (am Satz gemessen), Titel 0 auf allen Listenseiten, Platz = Seite·10 + Zeile,
   EIN Bild zwischen Satz 6 und Satz 8 beim Öffnen aus der Liste. (Die letzte war NICHT
   gemessen — die Gegenprüfung hat das zu Recht beanstandet; in der Nachbesserung durch RE2s
   belegte 15 Bilder ersetzt, 10.6 Punkt 3.)
4. **Bestehende Wachen mitgezogen** (Grund je Stelle im Test): `test_inv_fsm.c` Welle F (Liste
   mit Irons Diary auf Platz 0, 11 statt 12 Glyphen; Seite 1 leer mit „Files"; der
   '&'-Digraph-Beleg der RE1-Namen ist über die Liste nicht mehr erreichbar; leerer Platz
   öffnet nicht; Öffnen über Zustand 7 mit SE 6 dann SE 8; Ende-Stellung 18; LINKS aus der
   Ende-Stellung stumm; VIERECK/KREUZ in der Ende-Stellung SE 6), `probe_r26_inventar.c` B4
   (Dokument-Id 0x48 ausgenommen, neu B5: Skript 5 mit Tabellennamen), `test_re2doc_bildebene.c`
   (Satz 25 gegen die Port-Tabelle), `tools/gen_inv_file_doc.py` (nur Modul-Kommentar; Zensus
   erneut gefahren, `gen/inv_file_doc.inc` byte-gleich).
5. **Fußzeile im Intro-Raum halb verdeckt.** In ROOM1240 (Startraum der Messläufe, Kinematik)
   schneidet die Letterbox die Fußzeile „1/18" bei y 216 ab — das tat sie auf master beim
   Textleser genauso (`port_heute_leser_F84.png`). In ROOM1100 steht sie ganz. Nicht geändert.

### 10.4 Abnahme — gemessen

| Messung | Soll | Ist |
|---|---|---|
| Sonde A: Namen bei leerer Liste / nach `re15_files_add(0)` | 0 / 1 | 0 / 1 (Name 11 Glyphen, alle Seiten „Files") |
| Sonde A: Archiv der Originalmaske | 21 | 21, Basis 48 52 5c, 0x48 = „Chris' Diary" |
| Sonde C: deckend gezeichnete 0x0000-Texel FILE08 / FILE25 / 19 Diary-Seitendateien | 0 | 0 / 0 / 0 (bei 118 977 sichtbaren) |
| Sonde C: Seitenzahl | 18 | 18; p17 da, p18 nicht |
| Speicherstand: sizeof / visited_floor / files / checksum | 944 / 900 / 916 / 940 | 944 / 900 / 916 / 940 |
| Rundlauf über .mcr | Platz 0 = Dokument 0 | Platz 0 = 0, 1 belegt |
| Hebung v8 → v9 an der Speicherkarte des Nutzers (`nutzer_marken/re15_card_nutzer_2026-09-27.mcr`) | alle Plätze v9, 24 × 0xFF | 4 von 4 (roh v8), Prüfwort neu (visited_floor prüft der Riegel seit der Nachbesserung NICHT mehr — fremdes Feld, 10.6 Punkt 1) |
| Hebung v7 → v9; verfälschter v8-/v9-Block | gehoben / abgewiesen | gehoben (Besucht-Bits verworfen) / abgewiesen |
| Reihenfolge beim Aufheben (Folgenummer) | anhängen < schließen < Meldung weg < Zone < Flag < Weltmodell | 1, 2, 3, 4, 5, 6; Bild 1 / 42 / 132 / 132 / 132 / 132 |
| Item-Modal während des ganzen Aufhebens | nie aktiv | nie aktiv |
| Töne beim Aufheben bis zum Lesen | nur Satz 8, nach der Blende | nur Satz 8, im ersten Fahrbild (Bild 18 nach der Anforderung) |
| Framedump Leser gegen `FILE25_*_page_schirm.png`, sichtbare Referenzpixel gleich (5 Bit) | alle, außer unter RE1.5s Pfeilen | Titel 7069/7069, p01 13377/13385, p02 14427/14440, p03 8833/8833, p04 13791/13798; alle 28 Abweichungen unter dem linken Pfeil (x 34–35, y 112–127) |
| Grund außerhalb Pfeilen und Fußzeile | (0,0,0) | 0 nicht-schwarze Pixel in allen fünf Abzügen |
| Leser aus der Liste geöffnet (ROOM1100, Titel) | wie beim Aufheben | 7069/7069, 0 fremde Pixel außerhalb Pfeil/Fußzeile |
| `r30_diary_satz_pruefung.py` gegen die KOPIERTEN Dateien | ALLE PRÜFUNGEN BESTANDEN | bestanden: 427 von 427 Wörtern, 8 Daten am Seitenkopf, 0 Kernpixel ohne Glyphe |
| FILE-Reiter mit leerer Liste (Framedump F74) | keine vorinstallierten Namen | 10 Unterstrich-Zeilen, Titel „Files"; VIERECK auf leerer Zeile 0 (F84): Liste bleibt, Zustand 1 |
| FILE-Reiter nach dem Aufheben (ROOM1100, F2050) | „Irons Diary" auf Zeile 0 | „Irons Diary", Zeilen 1–9 Unterstriche |

Abzüge (Framedump, beschleunigter Renderer, 960×720) im Arbeitsbaum unter
`build/r30_irons-diary-dokument/umsetzung/`: `leser_aufheben_titel_F74.png`,
`leser_aufheben_p01_F99.png` … `leser_aufheben_p04_F174.png`, `meldung_filed_F249.png`,
`file_liste_leer_F74.png`, `file_liste_leer_viereck_F84.png`,
`file_liste_nach_aufheben_R1100_F2050.png`, `leser_aus_liste_titel_R1100_F2080.png`,
Vergleichswerkzeug `vergleich.py` mit Ausgabe `vergleich_framedump_gegen_schirm.txt`,
`satz_pruefung_kopierte_dateien.txt`.

### 10.5 Nicht gemessen / offen

1. **Kein Bild vom laufenden RE2** (wie Abschnitt 8.1); verglichen wurde gegen die
   RE2-Schirmvorschau aus Code und Texeln.
2. **Hörprobe** der Töne: gemessen ist, WELCHER Satz an welcher Stelle ausgelöst wird
   (`g_test_core_se_last`), nicht der Klang (alle Läufe mit `RE15_NOAUDIO`).
3. **Welt-Weg** über eine echte Item-Zone im Raum: das Prop auf dem Tresen baut die Folgespur
   irons-diary-welt; gemessen ist die Zone im Riegel (`re15_aot_fire_slot` mit `item_type 0x48`)
   und die Anforderung per `RE15_DOC_REQUEST` im laufenden Spiel.
4. **PSX-Ziel und Android** nicht gebaut. Android: `engine/src/re15_files.c` ist neu — `app/.cxx`
   friert die GLOB-Liste ein, vor dem Paket frisch konfigurieren.
5. **Paket:** `FILE25_*` muss unter `RE2/FILES/` neben der exe liegen.
6. **Linker RE1.5-Pfeil über der RE2-Textspalte** (offene Abweichung, in der Nachbesserung
   gemessen, nicht geändert): steht die Wippe auf 0, liegt der Pfeil bei x 20–35
   (`0x14 - off`, 16 breit, RE1.5 @0x800c7554-70), die RE2-Textseite beginnt bei x 25 und
   trägt ab x 34 Glyphen. Framedump gegen `FILE25_p01/p02/p04_page_schirm.png`: 8 / 13 / 7
   abweichende Pixel, alle bei x 34–35, y 112–127; bei Wippe 4 (x 16–31) keine. RE2s eigener
   Pfeil (x = 12 − 3·Blink, y 110, @0x800726b8-d4) erreicht die Spalte nicht. Nicht
   umgebaut, weil der Port die Pfeile bewusst aus RE1.5 nimmt (Grundsatz am Leser,
   `menu_common.c` „was RE1.5 vollständig hat, bleibt RE1.5") und RE2s Pfeil-Grafik aus
   seinem Status-Schirm nicht im Asset-Baum liegt; eine Mischung „RE1.5-Pfeil an RE2-Lage"
   wäre eine neue Erfindung.

### 10.6 Nachbesserung nach der Gegenprüfung

Die Gegenprüfung urteilte MÄNGEL (1 erheblich, 4 gering). Stand je Mangel, jeweils mit
eigenem Commit:

**1. Speicher-Riegel nagelte das FREMDE Feld fest (erheblich) — behoben.**
`test_r30_irons_diary_ablauf.c` S3 (Hebung an der Nutzer-Karte) und S5 (Erfassung) prüften
`visited_floor == 0`. Gemessen von der Gegenprüfung: mit der Hebung von karten-marken trägt
die Nutzer-Karte dort 1/2/4/2 Etagen-Bits in den Plätzen 0/1/2/4 — der Riegel wäre nach dem
Zusammenführen rot geworden. Jetzt prüft er nur noch Version 9, `files` 24 × 0xFF und das
Prüfwort; der Kopf des Tests sagt, warum `visited_floor` nirgends geprüft wird. Ist: Hebung
4 von 4.

Dabei gefunden (eigene Messung, `git merge-tree --write-tree worktree-wf_b4b268f3-d12-3
HEAD`): `re15_savedata_capture` führt git OHNE Konflikt zusammen, und die Zeile
`memset(out->visited_floor, 0, …) /* R30-VERTRAG: fremdes Feld */` dieser Spur landete
HINTER `re15_map_visited_floor_export` von karten-marken — sie hätte die Etagen-Bits nach dem
Export stillschweigend genullt. Die Zeile steht jetzt direkt nach `memset(out, 0, …)` vor
jedem Export; der Probelauf zeigt danach die richtige Folge (Null, Export der Etagen-Bits,
`memset(files, 0xFF)` von karten-marken, `re15_files_export` dieser Spur).

**ZUSAMMENFÜHREN mit karten-marken (d12-3)** — Konflikte laut Probelauf nur in
`re15_savedata.h` (Kommentar an `RE15_SAVE_VERSION`: eine Seite nehmen) und in
`re15_savedata.c`, vier Stücke:

1. Hebung v7/v8, Kommentar: eine Seite.
2. Hebung v7/v8, Rumpf: die Seite von karten-marken nehmen (v7 leert `visited`,
   `re15_map_visited_floor_heben(sd->visited, sd->visited_floor)`); das nachfolgende
   `memset(sd->files, 0xFF, …)` steht außerhalb des Konflikts und bleibt.
3. Hebung v2..v6: die Seite von karten-marken nehmen (kein `memcpy(visited)`, Nebenbefund D3;
   enthält schon `memset(sd->files, 0xFF, …)`).
4. `re15_savedata_restore`: BEIDES — von karten-marken `re15_map_visited_import(in->visited)`
   und `re15_map_visited_floor_import(in->visited_floor)` (ohne die alte Versionsabfrage),
   von dieser Spur `re15_files_import(in->files)` samt RE2-Beleg-Kommentar.

Danach die Zeilen mit `R30-VERTRAG: fremdes Feld` löschen, auch die beiden `memset` in
`re15_savedata_capture` (sie schaden in der neuen Reihenfolge nicht mehr, sind aber toter
Code). Prüfen: `unit_r30_irons_diary_ablauf` (S1–S4) und `test_map_speichern_laden` beide
grün.

**2. Scan-Weiche ungesichert (gering) — behoben.** Neuer Teil B in
`test_r30_irons_diary_ablauf.c`: Item-Zone 0x48 über `re15_aot_scan` mit Aktionstaste — der
Weg des Spielers (RE1.5 Druck-Scan FUN_80042bac, Handler[9] @0x80043328; RE2 FUN_80051884
@0x800518f0-f8). B1 ohne Taste nichts; B2 Leser angefordert, Item-Modal aus, Zone/Flag/
Weltmodell unverändert; B3 voller Lauf bis Zone aus, Flag (9,0x34), Weltmodell aus; B4
zweiter Druck nichts; B5 Gegenprobe Id 0x47 über denselben Scan startet das Item-Modal.
Negativ-Kontrolle N7 (Scan-Weiche in `aot_common.c` auskommentiert, neu gebaut): vorher
0 FAIL, jetzt **6 FAIL**; wiederhergestellt 0 FAIL.

**3. Ein Bild zwischen Satz 6 und Satz 8 unbelegt (gering) — an RE2 angeglichen.**
Selbst disassembliert (`info/re2leon/PSX.EXE`, Schirm mit Sprungtabelle @0x8006c754,
Zustand 2(s2) = 0x800d5bf2, Zähler 3(s2) = 0x800d5bf3):

```
Zustand 10 (Zeilenwahl), belegter Platz, Bild N
  8006ce74  addiu v0,zero,11   / 8006ce80 sb v0,2(s2)     -> Zustand 11
  8006ce90  jal   0x80031f6c   (a0 = 2, a1 = 0x8006d444)  Seitenlader als Task 2
  8006ce9c  lui   a0,0x406     / 8006cebc jal 0x8005ba28  Satz 6
Zustand 11 @0x8006cefc, je Bild
  8006cefc  lbu   v0,3(s2)
  8006cf04  addiu v1,v0,1      / 8006cf10 sb v1,3(s2)
  8006cf08  sltiu v0,v0,0xe                               alter Wert < 14 ->
  8006cf0c  bne   v0,zero,0x8006cf8c                        Tafeln fahren aus
  8006cf14  jal   0x80032138   (a0 = 2)                   Task-Status, lhu @0x80032144
  8006cf1c  beq   v0,zero,0x8006cf2c                      fertig -> weiter
  8006cf28  sb    v0(=14),3(s2)                           laedt noch -> naechstes Bild
  8006cf58  lui   a0,0x408                                Satz 8
  8006cf60  addiu v0,zero,16   / 8006cf64 sb v0,2(s2)     -> Zustand 16 (Leser)
  8006cf68  addiu v0,zero,312  / 8006cf74 sh v0,92(s2)    Textseite x = 312
  8006cf6c  sb    zero,3(s2)
  8006cf70  jal   0x8005ba28                              Satz 8, SELBES Bild
```

Zustand 10 wird nur mit Zähler 0 betreten (`sb zero,3(s2)` @0x8006cdd4 und @0x8006d000 —
die einzigen Schreiber von Zustand 10 sind @0x8006cdcc/@0x8006cff8), und weder Zustand 10
noch seine Callees (FUN_8006d550, 0x80075fd0, 0x80031f6c, 0x800693d0) schreiben 0x800d5bf3
(alle Schreiber im RE2-Dump `ghidra_re2_Leon.txt` durchgesehen). Also: Satz 8 frühestens
**15 Bilder** nach Satz 6 (Bilder N+1…N+14 mit altem Wert 0…13, Bild N+15 Ladeprüfung),
länger nur, solange die CD liest. Der Port lädt synchron (`re15_re2doc_select`), die
Ladeprüfung fällt sofort in den Fertig-Zweig: genau 15 Bilder. Gebaut in `menu_common.c`
`file_open_wait_tick` (`FILE_OPEN_WAIT_BILDER 14` @0x8006cf08); Satz 8 und das Öffnen im
SELBEN Bild (@0x8006cf58-74). Nicht übernommen: die Tafel-Bewegung @0x8006cf8c-cfc4 —
RE1.5s FILE-Schirm hat keine Tafeln; die Liste steht die 14 Bilder still, Eingaben liest
Zustand 11 keine. `test_inv_fsm.c` F6/F10 mitgezogen (Klick SE 6 sofort; 14 Wartebilder,
UNTEN wirkungslos, kein Ton; 15. Bild Leser offen + SE 8; dann 10 Fahrbilder).
Negativ-Kontrolle N8 (`FILE_OPEN_WAIT_BILDER 0`): **6 FAIL**.

Im laufenden Spiel gemessen (Nutzer-Karte Platz 0 → ROOM1150, `SDL_AUDIODRIVER=dummy
RE15_SE_DEBUG=1 RE15_DOC_LOG=1`, Aufheben per `RE15_DOC_REQUEST=260`, dann Menü, FILE,
VIERECK, VIERECK in F540): **Satz 6 in Bild 540, Satz 8 in Bild 555**, erster Fahrschritt
(x 292) in Bild 556, Zustand 3 ab Bild 567. Die Framedumps F535–F555 sind untereinander
pixelgleich (die Liste mit „Irons Diary", Zeile 0 hervorgehoben); der Titel nach dem Öffnen
(F570) gegen `FILE25_title_page_schirm.png`: **7069 von 7069** sichtbaren Pixeln gleich,
0 fremde Pixel außerhalb Pfeil/Fußzeile.

**4. Pfeil und '&'-Digraph (optional).**

- '&'-Digraph — **wieder unter Riegel**: neue Messschiene `re15_inv_screen_text_probe`
  (Glyphen-Drucker FUN_80028ec4 an freier Stelle, kein Spielpfad); `test_inv_fsm.c` F4b
  druckt den Archiv-Namen 0x59 an der alten Stelle y 0xc5: **8 Glyphen** rutschen auf
  y 0xd5 (@0x800131c0-c4, @0x80028fe8); Gegenprobe 0x58 ohne Digraph: 0.
- Pfeil — **nicht geändert**, als offene Abweichung in 10.5 Punkt 6 geführt (Messung und
  Grund dort).

**5. Riegel B der Sonde verglich nur die Fußzeile (gering) — verschärft.** Zusätzlich die
Bild-Ebene: `re15_inv_file_bild_lage` liefert Satz 25 / Seite −1 (Irons Diary) und Satz 8 /
Seite −1 (FILE08); die beiden Titel-Textseiten (je 256 × 144) unterscheiden sich in **1156
von 1396** sichtbaren Pixeln.

**Abnahme der Nachbesserung** (alle Läufe mit dem Bau dieses Zweigs; Abzüge und Protokolle
im Arbeitsbaum unter `build/r30_irons-diary-dokument/nachbesserung/`):

| Messung | Soll | Ist |
|---|---|---|
| Sonde A: Namen bei leerer Liste / nach `re15_files_add(0)` | 0 / 1 | 0 / 1 |
| Sonde C: deckend gezeichnete 0x0000-Texel (Titel + 18 Seitendateien) | 0 | 0 bei 118 977 sichtbaren |
| Sonde C: Seitenzahl | 18 | 18 |
| Speicherstand: Layout / Rundlauf / Hebung Nutzer-Karte / v7 / verfälscht | 944·900·916·940 / Platz 0 = 0 / 4 von 4 / gehoben / abgewiesen | wie Soll |
| Reihenfolge beim Aufheben (Sofortzündung, Teil A) | 1…6 | 1, 2, 3, 4, 5, 6 |
| Aufheben über den Scan (Teil B) | Leser statt Item-Modal, danach abgeräumt | B0–B5 grün; N7 → 6 FAIL |
| Satz 6 → Satz 8 beim Öffnen aus der Liste | ≥ 15 Bilder (RE2), Port synchron 15 | 15 (F540 → F555), Riegel F6; N8 → 6 FAIL |
| Framedump Leser beim Aufheben (`lauf1_aufheben`, F74/99/124/149/174) gegen `FILE25_*_schirm.png` | alle außer unter dem linken Pfeil | Titel 7069/7069, p01 13377/13385, p02 14427/14440, p03 8833/8833, p04 13791/13798; alle Abweichungen x 34–35, y 112–127 |
| Framedump Leser aus der Liste (`lauf2_liste_oeffnen`, F570) | Titel gleich | 7069/7069 |
| FILE-Reiter leer, Nutzer-Karte (`lauf3_liste_leer_nutzerkarte`, F145) | keine vorinstallierten Namen | 10 Unterstrich-Zeilen, Titel „Files", `liste0 = 255` |
| `r30_diary_satz_pruefung.py` gegen die nach `shared_assets/RE2/FILES` kopierten Dateien | bestanden | ALLE PRÜFUNGEN BESTANDEN, 427 von 427 Wörtern, 8 Daten am Seitenkopf |
| '&'-Digraph (F4b) | Umbruch vorhanden | 8 Glyphen bei y 0xd5, Gegenprobe 0 |
| Suite | alle grün | 362 von 362 (Endstand, Commit des Abschlusses) |

Nicht gemessen: der Klang (Hörprobe) — gemessen ist, WELCHER Satz in WELCHEM Bild ausgelöst
wird; ob sich Satz 6 und Satz 8 im Abstand von 15 Bildern noch berühren, hängt an der Länge
von Satz 6 und ist nicht vermessen (RE2 hat denselben Mindestabstand). PSX- und Android-Ziel
nicht gebaut (Android: `re15_files.c` neu → frisch konfigurieren). Der zusammengeführte Stand
mit karten-marken ist NICHT gebaut; der Probelauf `git merge-tree` liefert nur die
Konfliktliste oben.
