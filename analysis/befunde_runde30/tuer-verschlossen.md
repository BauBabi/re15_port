# Runde 30 · Thema A — „Tür verschlossen"-Töne aus RE2

Stand: 2026-09-28 · Phase **ERMITTLUNG, kein Bau** (Umsetzung: Abschnitt UMSETZUNG am Ende) · Basis master 8d83a025 (v0.8.15)
Build der Sonden: `re15_port/build_r30_tuer-verschlossen/` · Ausgaben: `build/r30_tuer-verschlossen/`

Kurzfassung in fünf Sätzen:

1. **RE2 hat genau EINEN „verschlossen"-Satz: Bank 2, Satz 0x16.** Bank 2 ist die *Raumbank*; der
   Satz trägt deshalb je Raum eine andere Welle. Gespielt wird er vom EXE-Tür-Handler
   (@0x80051610 / @0x800516a4) **und** von Raumskripten (z.B. Kartenleser-Tür ROOM2110 @Datei 0x01BBC).
2. Nach Hüllkurven-Messung sind es in Schloss-Räumen **drei Geräusche**: zwei im Polizeirevier
   (0,77 s und 0,51 s) und ein drittes (1,10 s) für die Kartenleser-Tür und alle Türen in
   Kanal/Fabrik/Labor.
3. **RE1.5 ist an verschlossenen Türen stumm** — Handler @0x80043084 / @0x800430bc, 0 von 41
   `Se_on`-Aufrufen der EXE liegen im AOT-Bereich. Einzige Ausnahme im ganzen Spiel: ROOM4000 sub02
   @Datei 0x0142E (ein eigener Piepton vor der Karten-Abfrage).
4. **Der Port ist heute ebenfalls stumm**: an 51 gemessenen Tür-Schloss-Plätzen (und an allen 86
   Plätzen mit Schloss-Wortlaut überhaupt) fällt kein Ton, der nicht auch ohne Tastendruck fiele.
5. ⚠ **ROOM2190 (RE2, Leon A) hat weder Kartenleser- noch Pincode-Tür und keinen Verschlossen-Ton.**
   Die Kartenleser-Tür liegt in **ROOM2110**, das Ziffernfeld in ROOM20B0 (§3.6). Ob der Nutzer
   ROOM2110 meinte, ist NICHT BELEGT.

---

## 1. Auftrag

Wörtlich (AUFTRAG.md Abschnitt A):

> Room 2190 in Resident Evil 2 hat zum Beispiel Beispiele für den Sound von verschlossenen Türen -
> bei Türen mit Kartenleser oder Pincode. Diesen "Tür verschlossen" Sound würde ich gerne für
> unsere Kartenleser Türen, Pincode Türen übernehmen, wenn sie noch verschlossen sind. Des weiteren
> hat Resident Evil 2 auch noch andere verschlossen Sounds für andere Situationen, zum Beispiel für
> Türen die verschlossen sind, und wir den Schlüssel dafür noch nicht besitzen. Ich möchte das du
> ermittelst, was für Arten von "verschlossen" Sound es gibt und die entsprechend auch bei uns
> sauber einbaust an die entsprechenden Stellen.

Einordnung nach `reai-v2-beta-zu-retail`: RE1.5 hat **kein** Ton-System für verschlossene Türen
(§3.1) → unfertig → RE2 Retail ist das Ziel, der Beleg kommt aus RE2.

---

## 2. MESSUNG im Port

### 2.1 Verfahren

Sonde `re15_port/tests/unit/probe_r30_tuer_verschlossen.c`
(Registrierung `tests/unit/probes/r30_tuer-verschlossen.cmake`). Je Raum wird **jeder** nach dem
Aufbau aktive AOT-Platz der Arten TEXT (sce 1) / EVENT (sce 3) / MARKE5 (sce 5) / TÜR (sce 2)
einzeln angefahren:

- frischer Raumaufbau, frische Flags (= frisches Spiel, Schlösser zu),
- Standplatz-Suche: 8 Blickrichtungen × 5 Zielpunkte; genommen wird der erste Stand, den die
  Wandklemme **nicht** verschiebt und dessen 620er-Vorwärtspunkt im Platz liegt
  (dieselbe Rechnung wie `aot_common.c`, Original `ori 0x26c` @0x80042bd0),
- EIN Quadrat-Druck, danach 150 Bilder Protokoll,
- **Kontrolllauf** desselben Platzes ohne Druck (trennt Gegner-Laute vom Tür-Ton).

Gezählt wird an den fünf Bank-Spionen aus `tests/test_support.c` (snd1, snd0, CORE, RE2-Panel,
RE2-Fahrstuhl) und an der Audio-Warteschlange der SCD-VM (`scd_audio_queue_pop`, jedes `Se_on`).

### 2.2 Ergebnis

Lauf über die 98 Räume, die laut Zensus einen Schloss-Text tragen
(`build/r30_tuer-verschlossen/port_messung.txt`, Auswertung `port_messung_auswertung.txt`):

| Größe | Wert |
|---|---|
| Plätze gemessen | 740 |
| davon ohne Standplatz (NICHT gemessen) | 57 |
| Plätze mit Text | 328 |
| Plätze mit Raumwechsel | 211 |
| Plätze, an denen ein Text mit Schloss-Wortlaut aufging (WEITES Muster, schließt Spinde und Autos ein) | 86 |
| … nach Wortlaut | 57 „lock/latched" · 14 Kartenleser/Ausweis · 12 andere Seite · 2 elektronisch · 1 Schlüssel |
| **davon Plätze, an denen eine der 52 Tabellen-Nachrichten aus §5 aufging (STRENGE Formeln, nur Türen)** | **51** (46 verschiedene Stellen; 16 × K, 35 × M) |
| **Plätze mit einem Ton, der im Kontrolllauf NICHT fiel — unter allen 86** | **0** |

Beispielzeilen (gekürzt):

```
ROOM10D0 platz  0 TEXT ev=6  | msg 6 "Communication Room"It's electronically locked.There's a card reader on" | TON: snd1=0 snd0=0 core=0 re2panel=0 re2elev=0 scd_se_on=0 | KONTROLLE: snd1=0 uebrige=0
ROOM11E0 platz  3 TEXT ev=6  | msg 6 "Prisons"It's electronically locked.There's a card reader on the right." | TON: … scd_se_on=0
ROOM1100 platz  3 TEXT ev=0  | msg 0 "It's electronically locked."                     | TON: … scd_se_on=0
ROOM1170 platz  5 TEXT ev=12 | msg 12 "It's locked from the other side."               | TON: … scd_se_on=0
ROOM3010 platz  6 TEXT ev=0  | msg 0 "It's locked.An ID card is required to open it."  | TON: … scd_se_on=0
ROOM4000 platz  6 TEXT ev=0  | msg 0 "The door is locked. The text on the panel reads:…" | TON: … scd_se_on=0
```

Gegenprobe, dass die Sonde Töne überhaupt sieht — der Rollladen-Schalter im selben Raum:

```
ROOM10D0 platz 18 EVENT ev=21 | msg 11 "It's a shutter switch. Will you push it?" | TON: snd1=0 snd0=0 core=1 … scd_se_on=2 (2,0x0c) (2,0x0a)
```

Drei der 86 Plätze zeigten einen snd1-Zähler > 0: ROOM1171 Platz 4 (4 mit Druck, 10 im
Kontrolllauf), ROOM3040 und ROOM3041 Platz 6 (je 1 mit Druck, 2 im Kontrolllauf). Der Kontrolllauf
ohne Druck liegt in allen drei Fällen höher — das sind Gegner-Laute der snd1-Bank, kein Tür-Ton.
Alle übrigen Zähler (snd0, CORE, RE2-Panel, RE2-Fahrstuhl, SCD-Se_on) stehen an allen 86 auf 0.

### 2.3 Was diese Messung NICHT abdeckt

- Nur der Frisch-Zustand. Stellen, die erst nach einem Flag entstehen (6 der 52 Tabellen-Stellen aus
  §5: ROOM11A0/11A1 msg 1, ROOM11B1 msg 12, ROOM30C1 msg 10, ROOM40A0/40A1 msg 3), sind nur statisch
  belegt (§3.1), nicht gefahren.
- 57 Plätze ohne kollisionsfreien Standplatz sind nicht gemessen.
- Gemessen wird an den Spionen der Test-Bibliothek, nicht am Lautsprecher. Für „stumm" reicht das
  (kein Aufruf = kein Ton); für den späteren Hör-Befund nicht.

---

## 3. ORIGINAL-MECHANISMUS

### 3.1 RE1.5 — verschlossene Türen sind stumm

**Handler-Tabelle** `PTR_8007469c` (`re15_disasm.py table 0x8007469c 14`):
`[1] 0x80043084` Text · `[2] 0x800430bc` Tür · `[3] 0x800430f0` Ereignis.

Text-Handler, vollständig:

```
80043084: addiu sp,sp,-24
80043088: addu  v0,a0,zero
8004308c: addu  a0,zero,zero
80043090: ori   a1,zero,0x300
80043098: lhu   a3,2(v0)        ; Pause-Maske
8004309c: lhu   a2,0(v0)        ; Nachrichten-Index
800430a0: jal   0x80027e68      ; Nachricht öffnen — EINZIGER Aufruf
800430a4: sll   a3,a3,16
800430b4: jr    ra
```

Tür-Handler, vollständig — kein Schloss-Test, kein Ton:

```
800430bc: ori v0,zero,0x1
800430c4: sw  a0,-13912(at)     ; 0x800ac9a8 Tür-Nutzlast
800430d4: sb  v0,21337(at)      ; 0x800b5359 Übergang = 1
800430d8: lw  v0,0(a0)  / 800430dc: lui v1,0xff00 / 800430e0: or / 800430e4: sw   ; Pause
800430e8: jr  ra
```

**Se_on-Zensus der EXE** (`r30_tuer_re2_seon_zensus.py 0x80045024 info/Re1.5/PSX.EXE`,
Wort `0x0c011409`): 41 Aufrufstellen. Zwischen `0x80041730` (der SCD-Opcode Se_on selbst) und
`0x8004a158` (Inventar) liegt **keine** — der gesamte AOT-Scan `FUN_80042bac` samt allen 14 Handlern
(0x8004305c … 0x800435cc) ruft den SE-Spieler nie. → `re15_seon_zensus_exe.txt`

**Schloss = Datenwahl**, kein Schlüssel-Feld: derselbe Platz bekommt `Aot_set sce=1` (Text) oder
`Door_aot_set sce=2` (Tür). 36 solche Zwillinge über 240 RDTs (`re15_tuer_zensus.txt` §4; Herleitung
`analysis/door_lock_1170.md` §3).

**Skript-Zensus** (240 RDTs, 34 davon 4-Byte-Platzhalter; `r30_re15_tuer_zensus.py`):
284 `Se_on` in 40 Räumen auf nur 11 verschiedenen (Bank,Satz)-Paaren, häufigste (2,0x0a)×56
(2,0x0d)×48 (2,0x0e)×44; **(2,0x16) kommt in keinem RE1.5-Skript vor**. 77 Skriptblöcke öffnen
eine Nachricht, die auf das WEITE Suchmuster passt (lock, card, code, key, open, shut, …);
18 davon enthalten ein `Se_on` — 16 sind Rollladen-Schalter („shutter switch"), einer der
Riegelschlüssel-Einsatz (ROOM3091 sub07), und genau **einer** ist ein Schloss:

```
ROOM4000.RDT sub02 @Datei 0x01426
  0x01426 Ifel_ck   06 00 2c 00
  0x0142A Ck        21 03 20 00            ; flag(3,32) == 0
  0x0142E Se_on     36 02 0f 00 03 00 00 00 00 00 00 00   ; Bank 2, Satz 0x0F
  0x0143A Message_on 2b 00 ff ff           ; "The door is locked. The text on the panel reads: …"
  0x01440 Message_on 2b 01 ff ff           ; "Will you use the Blue Master Keycard?"
```

Satz 0x0F in ROOM4000: EDT @Datei 0x161DC `00 00 c5 10`, Tone @0x16BC0 vol 80, VAG 7 @0x1D9E0,
**384 Byte**. Das ist RE1.5s einziger eigener Schloss-Ton und bleibt unangetastet (§5, Bedingung C).

**RE1.5-Raumbänke können den RE2-Satz nicht liefern** (`r30_re15_bank_zensus.py`,
`re15_bank_zensus.txt`): alle 206 Bänke haben 32 Sätze (0x00–0x1F); 0x25/0x26 existieren nicht.
Satz 0x16 ist in 36 Räumen belegt (STAGE2/5/6 und ROOM1260/1261), wird aber von keinem Skript
gerufen; wozu er dient, ist NICHT BELEGT.

**Und die RE2-Wellen stecken in keiner RE1.5-Bank** (`r30_re15_gegen_re2_wellen.py`,
`re15_gegen_re2_wellen.txt`): 92 verschiedene Wellen über alle Sätze aller Leon-Raumbänke, jede
gegen die RE2-Familien A/B/E/P gemessen — **0 Treffer** (gleiche Dauer ±5 % und r ≥ 0,90).
Höchster Wert r = 0,87 bei 0,058 s gegen 0,51 s Dauer. Die Töne müssen also importiert werden.

### 3.2 RE2 — der SE-Spieler und die Bank 2

`FUN_8005ba28` (RE2 `PSX.EXE`, md5 09a9b642…, t_addr 0x80010000):

```
8005ba30: srl  t1,a0,24          ; BANK
8005ba64: lb   v0,19528(at)      ; 0x800d4c48[bank] = VAB-Handle
8005ba6c: beq  v0,v1,0x8005bd38  ; -1 -> stumm
8005ba7c: srl  v0,a0,16 / 8005ba80: andi s7,v0,0xff   ; SATZ
8005ba8c: lw   a1,-17544(at)     ; 0x800dbb78[bank*4] = EDT-Zeiger
8005ba98: lw   v0,0(a1)
8005baa0: beq  v0,v1,0x8005bd38  ; Satz == 0xFFFFFFFF -> stumm
8005bac8: lw   a2,30112(at)      ; 0x800d75a0[bank*4] = VH-Zeiger
```

Bank 2 ist die **Raumbank** — Lader `FUN_80059e54`:

```
80059ee8: lui a2,0x800d / 80059eec: lw a2,-7388(a2)   ; 0x800ce324 = RDT-Kopf
80059f1c: lw  a1,8(a2)            ; RDT+0x08 = EDT
80059f44: sw  a1,-17536(at)       ; 0x800dbb80 = EDT-Zeiger der Bank 2
80059f5c: lw  a0,12(a2)           ; RDT+0x0C = VH
80059f70: sw  a0,30120(at)        ; 0x800d75a8 = VH-Zeiger der Bank 2
```

Jede RE2-Raumbank hat 48 Sätze (239 von 250 RDTs; 11 ohne Bank).

Der SCD-Opcode `Se_on` 0x36 (Tabelle @0x800a74c8, Eintrag @0x800a75a0 = `0x80056428`) benutzt
dieselbe Nummerierung:

```
80056444: lh  a1,4(s0)   ; Bezugsobjekt
80056448: lbu a3,1(s0)   ; Bank
8005644c: lh  a0,2(s0)   ; Satz | Flags<<8
80056518: sll v1,a3,24 / 80056520: andi v0,a0,0xff / 80056524: sll v0,v0,16
80056530: jal 0x8005ba28
8005653c: addiu v1,s0,12 ; Satzlänge 12
```

### 3.3 RE2 — der Tür-Handler `0x80051514`

AOT-Handler-Tabelle `PTR_800a73c4` (Aufruf @0x80051494–9c `lw v0,0(v0)` / `addiu a0,s0,12` /
`jalr v0`): `[1] 0x80051514` Tür · `[4] 0x80051948` Text · `[5] 0x80051980` Ereignis ·
`[13] 0x80052044` Schublade.

Satzlage: `Door_aot_se` 0x3B @0x80054be4 legt `pc+2` ab und schiebt um 32 (@0x80054c40); die
Nutzlast ist Satz+12, also `key_id` = Nutzlast+15, `key_type` = Nutzlast+16.

```
800515a8: lbu  a1,15(s2)            ; key_id
800515b0: andi v0,a1,0x80
800515b4: beq  v0,zero,0x800516c8   ; Bit 7 aus -> Tür geht auf
800515c0: jal  0x80077360           ; Flag (key_id & 0x3f) schon gesetzt?
800515c8: bne  v0,zero,0x800516cc   ;   ja -> Tür geht auf
800515d0: lbu  s0,16(s2)            ; key_type
800515d4: addiu v0,zero,254 / 800515d8: bne s0,v0,0x80051608
  800515e8: addiu a2,zero,10 / 800515ec: jal 0x8002fe38   ; "You unlocked it."
  800515f4: lui a0,0x226   / 800515f8: jal 0x8005ba28     ; Se_on(2,0x26)
  80051600: j 0x80051688                                  ; Flag setzen
80051608: bne s0,v0(=255),0x80051628
  8005160c: lui a0,0x216   / 80051610: jal 0x8005ba28     ; Se_on(2,0x16)   <<< VERSCHLOSSEN
  80051620: j 0x800516b8   / 80051624: addiu a2,zero,11   ; "It's locked from inside."
80051628: jal  0x800696cc           ; Gegenstand key_type im Inventar suchen
80051634: bltz s1,0x800516a0
  80051640: addiu a2,zero,5 / 8005164c: jal 0x8002fe38    ; "You have used the <Name>."
  80051654: lui a0,0x225   / 80051658: jal 0x8005ba28     ; Se_on(2,0x25)
  80051664..70: Fortsetzung 0x80051718 einhängen (Wegwerf-Abfrage)
800516a0: lui a0,0x216   / 800516a4: jal 0x8005ba28       ; Se_on(2,0x16)   <<< VERSCHLOSSEN
800516b4: addiu a2,s0,-76 / 800516b8: jal 0x8002fe38      ; Text = SET-A[key_type-76]
```

Beide 0x16-Aufrufe tragen `a1 = 0` (@0x80051614 / @0x800516a8 `addu a1,zero,zero`) und Flag-Byte 0:
**nicht positional**, Ton und Text im selben Bild.

Sprungziele selbst disassembliert: `0x80077360` = Flag-Test (`srl v1,a1,5` … `and v0,v1,v0`),
`0x8007730c` = Flag-Setzen, `0x800696cc` = Inventarsuche (Schleife über 0x800d4a3c, Schritt 4).

Die Texte (SET-A, Tabelle @0x8009f368, Texte @0x8009efcc; `r30_re2_text.py`):

| Index | key_type | Text |
|---|---|---|
| 10 | 0xFE | You unlocked it. |
| 11 | 0xFF | It's locked from inside. |
| 12 | 0x58 | It's locked. |
| 13–16 | 0x59–0x5C | It's locked. A spade / diamond / heart / club is etched under the key hole. |
| 19 | 0x5F | It's locked. The door is marked "Power Room". |
| 21 | 0x61 | The door is opened with a card key. |
| 22 | 0x62 | It's locked. It's a keyhole for the Master Key. |
| 23 | 0x63 | It's locked. |
| 9 | — | This key is useless now. Discard? (Fortsetzung 0x80051718, **ohne** Se_on) |

**Zensus der verschlossenen Tür-AOTs** (`r30_re2_tuer_zensus.py`, `re2_tuer_zensus.txt`):
574 Tür-AOTs in 250 RDTs, **24** mit `key_id & 0x80` in 21 Räumen, alle in 1xxx–7xxx
(23 × Opcode 0x3B, 1 × 0x68 = ROOM60C0 @Datei 0x00F52).
key_type: 0x58×1, 0x59×2, 0x5A×2, 0x5B×1, 0x5C×3, 0x5F×1, 0x61×2, 0x62×1, 0x63×1, 0xFE×5, 0xFF×5.
Gegenprobe per Rohmuster: 23 von 23 `0x3B`-Sätzen decken sich (0 nur-roh, 0 nur-Walker).

### 3.4 RE2 — Raumskripte spielen denselben Satz

Die Kartenleser-Tür zur Waffenkammer, `ROOM2110.RDT` (Hintergrund Cut 12 zeigt Tür „ARMS STORAGE"
mit Lesegerät: `build/r30_tuer-verschlossen/re2_ROOM211_hintergruende.png`):

```
sub03 @0x017FA  Ck(4,0x0e)==0
      @0x017FE  Aot_set  aot 5 sce 5 -> sub10           ; das Lesegerät
      @0x01826  Aot_reset 46 08 05 31 ff 00 18 09 00 00 ; die TÜR (aot 8) -> Ereignis sub09
sub09 @0x01BAE  Message_on 2b 00 00 00 ff ff            ; msg 0 "Weapon Storage"
      @0x01BB6  Message_on 2b 00 06 00 ff ff            ; msg 6 "It's electronically locked.
                                                        ;        There's a card reader on the left."
      @0x01BBC  Se_on 36 02 16 00 00 00 44 a4 f8 f8 93 cc   ; Bank 2, Satz 0x16   <<<
sub10 @0x01BDE/@0x01BEA  Message_on msg 2 / msg 1       ; "The power to the card reader is on/off"
                                                        ;  -> KEIN Se_on
sub08 @0x01B4C/5C/90     Se_on 0x0c, 0x0d, 0x0b         ; Karte angenommen (3 Töne)
```

**Zensus aller Skript-`Se_on` auf Bank 2 Satz 0x16** (`r30_re2_lockscript_zensus.py`,
`re2_lockscript_zensus.txt`): 17 Aufrufe. Gegenprobe Rohmuster `36 02 {16|25|26}`: 19 von 19
decken sich.

| Raum / Block | @Datei | Text im selben Block |
|---|---|---|
| 1050 sub02 | 0x013E4 | It's locked… |
| 10C0 sub02 | 0x01ADA | The knob turns, but the door won't budge. It seems to be sealed from the other side. |
| 1100 sub05 | 0x0304E | It's electronically locked. The door reads: "Library." |
| 2000 sub09 / sub10 | 0x02CAC / 0x02CCA | It's electronically locked. |
| 20A0 sub02 / sub03 | 0x00B56 / 0x00B6A | It's locked… |
| **2110 sub09** | **0x01BBC** | **It's electronically locked. There's a card reader on the left.** |
| 4010 sub06 | 0x02804 | The warehouse door is locked. |
| 40A0 sub30 | 0x033C4 | It's electronically locked and will not open! |
| 5090 sub19 | 0x0339A | It's locked from the inside. |
| 6170 sub23 | 0x0463A | The door has been completely sealed due to the explosion. |
| 7020 sub06 | 0x0159E | The door won't open until the power is restored! |
| 1120 sub36/sub43, 3010 sub30, 7030 sub06 | — | kein Schloss-Text im Block — Zweck NICHT BELEGT |

**In RE2 STUMM** (für die Abgrenzung wichtig):

| Fall | Beleg |
|---|---|
| Reiner Text-Platz „The door won't open!" u.ä. | Text-Handler @0x80051948 ruft nur `jal 0x8002fe38` @0x80051968; 47 solche Plätze (Liste §3 der Zensus-Datei) |
| Kaputtes Schloss „The lock is broken and can't be opened." | ROOM2090 sub00 @0x00E7C `Aot_reset … sce 4` = Text-Platz |
| Lesegerät selbst „The power to the card reader is off." | ROOM2110 sub10, kein Se_on im Block |
| **Ziffernfeld, falscher Code** | ROOM20B0 sub02: je Ziffer sub03 @0x033A2 `Se_on(2,0x0a)`; richtig @0x03356 `Se_on(2,0x0b)`; der Else-Zweig ab @0x0336E enthält **kein** Se_on |
| Schreibtisch „The desk is locked." | Schubladen-Handler @0x80052174–7c `addiu a1,zero,256` / `j 0x800522a8` / `addiu a2,zero,2`, kein Se_on auf diesem Weg |
| Wegwerf-Abfrage „This key is useless now." | Fortsetzung 0x80051718…0x80051880 enthält kein `jal 0x8005ba28` |

### 3.5 RE2 — welche Wellen hinter Satz 0x16 liegen

Zensus `r30_re2_bank2_zensus.py` (`re2_bank2_zensus_16_25_26.txt`): Satz 0x16 belegt in 50 Räumen,
0x25 in 27, 0x26 in 11. Für die 31 Leon-A-Räume sind es 12 verschiedene Dateien.

Hüllkurven-Vergleich `r30_re2_wellen_familien.py` (10-ms-Betragsmittel, Pearson ≥ 0,90 bei
gleicher Dauer ±5 %; Abspielrate nach dem Port-Weg `re15_vab_note2pitch2`):

| Familie | Wellen (sha1, Bytes) | Dauer | Räume |
|---|---|---|---|
| **A** | cf1414572aea (7184) | 0,77–0,82 s | 1010 10C0 1100 1140 2000 2010 20A0 20B0 20F0 |
| **B** | 60e753ac5e56 (3232) | 0,51 s | 1050 1160 2020 2040 2070 20C0 |
| **E** | be2f6ea9caaa (10272), c8f2c228c6b9 (5152), 119e2ab24f8c (7088), 453a9195db3b (5152), b463b0854ee0 (3872), 11bf15000312 (7088) — paarweise r = 0,94…1,00 | 1,10–1,12 s | **2110** 4010 40A0 5090 6030 6060 60B0 60C0 6170 7000 7030 7040 |
| P | adc3364d41dd, ea19d086e3cb (je 6144), r = 0,97 | 1,31 s | 40E0 7020 |
| — | 770559359c40 (17632, 3,04 s) ROOM1120; c9efe1b034cc (9248) ROOM3010 | | kein Schloss-Kontext |

Familie E ist dasselbe Geräusch in sechs Abtastraten; die beste Fassung ist die aus ROOM2110
(16397 Hz). A und B korrelieren nicht (r = −0,07) — zwei verschiedene Geräusche.

**Eine Regel „Türart → Welle" gibt es in RE2 nicht.** Gemessen: die Tür ROOM2000↔ROOM2020
(beide `dtex` 13) klingt von der einen Seite nach A, von der anderen nach B. Die Welle ist je
**Raum** gesetzt. Ableitbar ist nur das Gebietsmuster: A/B ausschließlich in Stage 1–2
(Polizeirevier), E/P in allen Schloss-Räumen der Stages 4–7 **und** in ROOM2110.

WAV-Abzüge zum Anhören: `build/r30_tuer-verschlossen/wav/` (22 Dateien), Messwerte
`re2_wellen_messung.txt`. Das sind Messwerte, kein Hörbefund.

### 3.6 ROOM2190 — was dort wirklich ist

`info/re2leon/PL0/RDT/ROOM2190.RDT` (138460 B), Skript vollständig gelaufen
(`re2_room2190_scd.txt`, 22 Blöcke; Gegenprobe: alle 22 Blöcke bytegleich zu den extrahierten
`.scd`). Hintergründe `re2_ROOM219_hintergruende.png`: Zellentrakt-Gang mit Gitterzellen.

| Platz | Satz | Befund |
|---|---|---|
| aot 0 → ROOM2160, aot 2 → ROOM21A0, aot 3 → ROOM3010 | @0x0095A / @0x0097A / @0x009BA | `key_id` = 0x00, **unverschlossen** |
| aot 3 (bei flag(1,6)) | @0x009A2 `Aot_set … sce 4`, msg 4 | „The door won't open!" — Text-Platz, stumm |
| aot 10 | @0x00B22 `Aot_set … sce 4`, msg 2 | „A control panel to release the lock. The lock appears to be open." — stumm |
| Raumbank Satz 0x16 | @Datei 0x0AAD8 | `ff ff ff ff` = leer |
| alle 11 `Se_on` des Raums | Sätze 0x0a/0x0b/0x0c/0x0d/0x1d/0x1f | liegen in den Zwischensequenzen sub08/sub10/sub12/sub17 |

Kartenleser-Tür = **ROOM2110** (§3.4). Ziffernfeld = **ROOM20B0** msg 13–16
(„A numerical key pad. First digit number?").

### 3.7 Die Arten — Ergebnis

| Art | RE2-Auslöser | Satz | Welle |
|---|---|---|---|
| Schlüssel fehlt | EXE @0x800516a4 | 2 / 0x16 | je Raum: A, B oder E |
| Von der anderen Seite / von innen verriegelt | EXE @0x80051610; Skript 5090 | 2 / 0x16 | je Raum |
| Elektronisch verriegelt / Kartenleser | Skript 1100, 2000, **2110**, 40A0; EXE key_type 0x61 | 2 / 0x16 | 2110/40A0/60C0: E · 1100/2000: A |
| Versiegelt / ohne Strom | Skript 10C0, 6170, 7020 | 2 / 0x16 | A · E · P |
| *Aufgeschlossen: Schlüssel benutzt* | EXE @0x80051658 | 2 / 0x25 | je Raum (7 Wellen) |
| *Aufgeschlossen: von dieser Seite entriegelt* | EXE @0x800515f8 | 2 / 0x26 | d1d99d659a37 (9 von 11 Räumen; ROOM6090 54f7c4ad37c5; ROOM1130 nicht auflösbar) |
| *Sonderschloss* | Skript ROOM2080 sub13 @0x0174A | 2 / 0x27 | d3caa2f5cfc9 |

Die kursiven drei sind **keine** Verschlossen-Töne und nicht beauftragt; sie stehen hier, weil sie
im selben Handler liegen.

---

## 4. URSACHE

Der Port ist stumm, weil er RE1.5 byte-true folgt und RE1.5 hier nichts hat:

- Text-Platz → `re15_scd_show_message()` (`scd_vm.c:1446`) → `re15_dialog_open_mask()`; kein
  Audio-Aufruf. Das entspricht exakt `LAB_80043084`.
- Skript-Weg → `op_message_on()` (`scd_vm.c:1573`); Töne nur, wo das Skript selbst ein `Se_on` trägt.
- Ein Schlüssel-Feld wie RE2s `key_id`/`key_type` gibt es im RE1.5-Tür-Satz nicht; der
  RE2-Handler lässt sich deshalb nicht 1:1 einhängen. Der Einhängepunkt im Port ist das
  **Öffnen der Schloss-Nachricht** — in RE2 fallen Ton und Text im selben Bild (§3.3, §3.4).

---

## 5. UMSETZUNGSPLAN für den Bau-Agenten

Zwei Werkzeuge liegen fertig vor und schreiben im Normalfall nur nach `build/`; mit `--install`
schreiben sie an den Auslieferungsort.

### Schritt 1 — Bank schneiden

```
python re15_port/tools/re2_door_se_cut.py --install
  -> re15_port/shared_assets/RE2/TUERSE.VBS          (23832 B, md5 8ff0c00c17017dcb15b68711411f4e52)
  -> re15_port/engine/src/gen/re2_door_bank.inc
```

| Bank-Satz | RE2-Quelle | EDT-Satz @Datei | Tone @Datei | VAG @Datei | Bytes | sha1 |
|---|---|---|---|---|---|---|
| 0 `RE2_DOOR_SE_ZU_A` | ROOM1140 Satz 0x16 | 0x01448 `00 00 74 16` | 0x01DB0 | 0x02870 | 7184 | cf1414572aea7ba1f3364129a5021b8fc8111b4b |
| 1 `RE2_DOOR_SE_ZU_B` | ROOM1050 Satz 0x16 | 0x019FC `00 00 84 16` | 0x02384 | 0x074A4 | 3232 | 60e753ac5e5697bae5b2f3caa4d3eb1dc1308708 |
| 2 `RE2_DOOR_SE_ZU_E` | ROOM2110 Satz 0x16 | 0x02EFC `00 00 74 00` | 0x03864 | 0x07054 | 10272 | be2f6ea9caaa6a9996a73c5a3f3fb0baf6b56aa3 |

Satzformat wie `ELEVSE.VBS`: `[SE-Map 0x20][VH 3104][Trailer 8][VBD 20688]`, `edt_size` 3144.
Geändert werden beim Schnitt nur der VAG-Index im Tone (+0x16) und das Tone-Nibble im EDT-Byte 2;
alles andere ist bytegleich. Das Werkzeug bricht ab, wenn ein Offset, ein Rohsatz oder ein sha1
abweicht.

**Schon geprüft** (Sonde `probe_r30_tuerse_bank`, Ausgabe `tuerse_bank_sonde.txt`) — die Bank lädt
mit dem vorhandenen Port-Code (`re15_vab_parse` / `re15_edt_decode` /
`re15_edt_resolve_layers_ex` / `re15_vag_adpcm_decode`):

| Satz | Stimme | Tone | pitch | Rate | Samples | Dauer |
|---|---|---|---|---|---|---|
| 0 | 6 (prio 4) | vol 110 center 85 shift 42 note 66 | 0x589 | 15256 Hz | 12544 | 0,822 s |
| 1 | 6 (prio 4) | vol 105 center 91 shift 0 note 67 | 0x400 | 11025 Hz | 5628 | 0,510 s |
| 2 | −16 (Direktzweig) | vol 127 center 84 shift 57 note 66 | 0x5f3 | 16397 Hz | 17948 | 1,095 s |

### Schritt 2 — Stellentabelle erzeugen

```
python re15_port/tools/gen_lock_se_sites.py --install
  -> re15_port/engine/src/gen/lock_se_sites.inc      (52 Stellen: 17 x K, 35 x M; 66 verworfen)
```

Aufnahmebedingungen (im Generator-Kopf ausgeschrieben): **A** wörtliche Schloss-Formel ·
**B** Nachricht im Raum erreichbar (Text-AOT oder `Message_on`) · **C** Skript-Wege mit eigenem
RE1.5-`Se_on` werden nicht aufgenommen (ROOM4000 sub02). Jede Zeile trägt den Datei-Offset der
Nachricht und der AOT-/Skript-Sätze. Vorabzug: `build/r30_tuer-verschlossen/lock_se_sites.inc`.

Art **K** (17): „…It's electronically locked…" in ROOM10D0/10D1 msg 6, 11E0/11E1 msg 6, 1230/1231
msg 6, 1180/1181 msg 0, 1100/1101 msg 0, 4080/4081 msg 0; „…An ID card is required…" in
ROOM3010/3011 msg 0, 4000 msg 0 (nur Text-Weg), 40A0/40A1 msg 3.
Art **M** (35): „It's locked.", „It's locked from the other side.", „The door is locked.",
Schutzraum-, Feuer- und Riegel-Texte (vollständige Liste in der `.inc`).

Dass die Text-Plätze ohne Zwilling wirklich Türen sind, ist am Hintergrundbild geprüft
(`r30_re15_platz_auf_bild.py`, `build/r30_tuer-verschlossen/plaetze/*.png`): ROOM10D0 Platz 0
(Tür „radio room" mit Lesegerät), ROOM10F0 Platz 9, ROOM1100 Platz 3, ROOM1180 Platz 5 (Tür neben
Ziffernfeld), ROOM2020 Platz 5, ROOM3020 Platz 1 und ROOM30C0 Platz 2 (Glastür), ROOM4000 Platz 6,
ROOM5120 Platz 4. **ROOM20A0 Platz 8 ist im Bild nicht eindeutig** (§7).

### Schritt 3 — Engine

Neu `re15_port/include/re15_lock_se.h` + `re15_port/engine/src/lock_se_common.c`:

```c
#define RE15_LOCK_WEG_AOT    1   /* Text-Platz sce 1  (RE2-Gegenstück: EXE @0x80051610/@0x800516a4) */
#define RE15_LOCK_WEG_SKRIPT 2   /* Message_on        (RE2-Gegenstück: ROOM2110 sub09 @0x01BBC)      */
void re15_lock_se_notice(unsigned room_id, uint8_t msg_id, int weg);
void re15_door_bank_rec(re15_door_bank_rec_t *out);      /* Muster re15_elev_bank_rec */
extern unsigned g_re15_lock_se_zaehler;                  /* Messhaken */
extern int      g_re15_lock_se_letzter;
```

`re15_lock_se_notice` sucht (Raum, Nachricht) in `re15_lock_se_sites[]`, prüft `wege & weg` und
ruft `re15_audio_re2_door_se()`. Zuordnung Art → Satz: **K → `RE2_DOOR_SE_ZU_E`**,
**M → `RE2_DOOR_SE_ZU_A`** (Vorschlag, siehe §6 Punkt 2). Nicht positional, wie
@0x80051614 / @0x800516a8.

### Schritt 4 — Einhängepunkte (`re15_port/engine/src/scd_vm.c`)

| Stelle | wo genau | Aufruf |
|---|---|---|
| `re15_scd_show_message()` Z. 1446 ff. | **nach** den beiden Rückkehr-Zweigen Save-Telefon (Z. 1461) und Item-Box (Z. 1473), unmittelbar vor `re15_dialog_open_mask` (Z. 1487) | `re15_lock_se_notice(g_current_room_id, index, RE15_LOCK_WEG_AOT)` |
| `op_message_on()` | neben `re15_discard_besitz_vor_nachricht` (Z. 1750) — das ist die Stelle des tatsächlichen Öffnens, **hinter** dem Stimmen-Warte-Gate, das mit `return 2` wiederholt | `re15_lock_se_notice(g_current_room_id, t->pc[1], RE15_LOCK_WEG_SKRIPT)` |

⛔ Nicht früher in `op_message_on` einhängen: der Opcode wird bei laufender Stimme mehrfach
betreten, der Ton fiele dann je Bild.

### Schritt 5 — Audio

| Datei | Änderung |
|---|---|
| `re15_port/include/re15_audio.h` | `void re15_audio_re2_door_se(int se_id);` mit Belegblock (dieses Dossier §3.3/§3.4) |
| `re15_port/platform/pc/src/audio_pc.c` | `load_re2_door_se_pc()` + `re15_audio_re2_door_se()` als Kopie des Fahrstuhl-Slots Z. 1129–1197, Datei `TUERSE.VBS`, Größen aus `re15_door_bank_rec()` |
| `re15_port/platform/psx/src/audio_psx.c` | Folge-Stub wie Z. 858 |
| `re15_port/tests/test_support.c` | Spion `g_test_door_se_last` / `g_test_door_se_count` |

### Schritt 6 — Riegel

1. `unit_r30_tuerse_bank`: aus `probe_r30_tuerse_bank.c` — Soll je Satz: Bytes 7184/3232/10272,
   pitch 0x589/0x400/0x5f3, Stimme 6/6/−16.
2. `unit_r30_tuer_verschlossen`: aus `probe_r30_tuer_verschlossen.c`, um den neuen Spion erweitert.
   Soll im Frisch-Zustand über dieselben 98 Räume: **51 Plätze** öffnen eine Tabellen-Nachricht
   (46 verschiedene Stellen; 16 × K, 35 × M) → dort genau 1 Aufruf mit dem Satz der Art; an
   **allen übrigen** Plätzen 0 Aufrufe. Pflicht-Einzelfälle:
   ROOM10D0 Platz 0 → ZU_E · ROOM1170 Platz 5 → ZU_A · ROOM1070 Platz 4 (Spinde) → 0 ·
   ROOM10D0 Platz 17 (Wegweiser) → 0 · ROOM10D0 Platz 14 (offene Tür) → Raumwechsel, 0.
3. Gegenprobe ROOM4000: `sub02` direkt starten (wie `probe_r22_codepanel_reihenfolge`) → SCD-Se_on
   (2,0x0f) = 1, RE2-Tür-Ton = 0.
4. Tabellen-Volllauf: für alle 52 Zeilen `re15_lock_se_notice` mit jedem gesetzten Weg → 1 Aufruf;
   mit dem nicht gesetzten Weg → 0.

### Schritt 7 — Abnahme im Spiel

Env-gegatetes Datei-Log (die GUI-exe hat kein stderr), z.B. `RE15_TUERSE_LOG=<pfad>`: eine Zeile je
Auslösung mit Raum, Nachricht, Art, Satz. Hörprobe durch den Nutzer an ROOM10D0 (Funkraum-Tür) und
ROOM1170 (Tür am Treppenende).

### Schritt 8 — Paket

`TUERSE.VBS` muss in allen drei Paketen liegen. Android: `lock_se_common.c` ist eine neue
Engine-Quelle — der eingefrorene Configure unter `app/.cxx` kennt sie nicht
(Memory `reai-v2-android-glob-cache`); vor dem Bau verwerfen, danach `re15_assets.txt` auf
`RE2/TUERSE.VBS` prüfen.

---

## 6. Risiken und offene Fragen

1. **ROOM2190 ≠ Kartenleser-Raum.** Der Plan nimmt die Welle aus ROOM2110, weil das die einzige
   Kartenleser-Tür in Leon A ist. Meinte der Nutzer ein anderes Geräusch aus ROOM2190, wäre es einer
   der Zwischensequenz-Sätze 0x0a–0x0e — dann bitte benennen.
2. **Welche Welle für mechanisch verschlossene Türen?** RE2 gibt keine Regel (§3.5). Vorschlag A
   (9 Räume, 0,77 s), Alternative B (6 Räume, 0,51 s). Beide liegen in der Bank; der Wechsel ist eine
   Zeile. Zum Anhören: `wav/re2_1140_16_schluessel_fehlt.wav` gegen
   `wav/re2_1050_16_schluessel_fehlt.wav`.
3. **Gebietsmuster.** RE2 nimmt in Kanal/Fabrik/Labor ausnahmslos Familie E, auch für mechanische
   Schlösser. Soll der Port das nachbilden, bekämen die M-Stellen der RE1.5-Stages 2/4/5/6 ebenfalls
   ZU_E. Welche RE1.5-Stage welchem RE2-Gebiet entspricht, ist aus Bytes nicht ableitbar.
4. **Lesegerät ohne Karte / falscher Code** („You have not the Blue Keycard…", „Wrong code, try
   again.") sind **nicht** in der Tabelle: RE2 ist an beiden Gegenstücken stumm (§3.4).
5. **Stimmen-Kollision.** ZU_A/ZU_B laufen über SE-Stimme 6 mit Priorität 4 und unterliegen dem
   Prioritäts-Gate `FUN_80045a18`; ein lauterer Raum-SE auf derselben Stimme kann den Tür-Ton
   verdrängen. Das ist Original-Verhalten der Stimmen-Maschine, aber im Port nicht gemessen.
6. **Wortlaut-Klassifikation.** Die Tabelle hängt an 11 wörtlichen Formeln. Das ist eine
   Port-Entscheidung, kein Original-Mechanismus; die 66 verworfenen Nachrichten stehen mit Grund
   in der `.inc`.
7. **Aufschließ-Töne** (Satz 0x25 / 0x26) sind vorbereitet (`re2_door_se_cut.py
   --mit-aufschliessen`), aber nicht beauftragt und nicht eingeplant. Der natürliche Einhängepunkt
   wäre die vorhandene Wegwerf-Tabelle `gen/discard_sites.inc`.

---

## 7. Ausdrücklich NICHT belegt

- Ob der Nutzer mit „Room 2190" ROOM2110 meinte.
- Wie die Wellen **klingen**. Dauer, Rate und Hüllkurve sind gemessen; „Rütteln", „Summen" o.ä.
  wären Hörurteile, die ich nicht fällen kann.
- Eine RE2-Regel, wann ein Raum Welle A und wann B bekommt.
- Der Zweck der vier Skript-`Se_on(2,0x16)` ohne Schloss-Text (ROOM1120 sub36/sub43, ROOM3010 sub30,
  ROOM7030 sub06).
- Ob ROOM20A0 Platz 8 (msg 1 „It's locked.") eine Tür ist — das projizierte Rechteck liegt im Bild
  vor einem Sims, nicht erkennbar an einem Türblatt. Ebenso ungeprüft am Bild: ROOM11B1 Platz 8,
  ROOM1211 Platz 8, ROOM4001 Platz 8 (Abzüge liegen vor, nicht ausgewertet).
- Das Verhalten im echten RE2 zur Laufzeit. Alles hier ist statisch aus EXE und RDTs gelesen; kein
  Emulator-Lauf.
- 27 von 1961 RE2-Skriptblöcken brechen im Walker mitten im Block ab, 45 weitere im Schwanz hinter
  dem letzten `Evt_end`; gelaufen sind 75,8 % der Bytes. Die zwei Rohmuster-Gegenproben (§3.3, §3.4)
  fanden außerhalb des Gelaufenen **nichts**, sie decken aber nur genau diese zwei Muster ab.
- Die Claire-Disc (PL1) liegt nicht im Repo.

---

## Werkzeuge und Artefakte

| Pfad | Zweck |
|---|---|
| `re15_port/tools/re2_door_se_cut.py` | Bank schneiden (mit Selbsttest) |
| `re15_port/tools/gen_lock_se_sites.py` | Stellentabelle erzeugen |
| `re15_port/tests/unit/probe_r30_tuer_verschlossen.c` | Port-Messung Text + Ton |
| `re15_port/tests/unit/probe_r30_tuerse_bank.c` | Bank mit Port-Code laden |
| `re15_port/tests/unit/probes/r30_tuer-verschlossen.cmake` | Registrierung beider Sonden |
| `analysis/befunde_runde30/tools/r30_tuer_re2_seon_zensus.py` | alle `jal Se_on` einer EXE mit a0 |
| `analysis/befunde_runde30/tools/r30_re2_scd.py` · `r30_re2_msg.py` · `r30_re2_text.py` | RE2-Skript / -Texte aus der RDT |
| `analysis/befunde_runde30/tools/r30_re2_roombank.py` · `r30_re2_bank2_zensus.py` | RE2-Raumbank |
| `analysis/befunde_runde30/tools/r30_re2_tuer_zensus.py` · `r30_re2_lockscript_zensus.py` | RE2-Zensus Türen / Skripte |
| `analysis/befunde_runde30/tools/r30_re2_wellen_wav.py` · `r30_re2_wellen_familien.py` | WAV-Abzug, Hüllkurven |
| `analysis/befunde_runde30/tools/r30_re15_tuer_zensus.py` · `r30_re15_bank_zensus.py` | RE1.5-Zensus |
| `analysis/befunde_runde30/tools/r30_re15_gegen_re2_wellen.py` | jede RE1.5-Bankwelle gegen die RE2-Familien |
| `analysis/befunde_runde30/tools/r30_re15_platz_auf_bild.py` | AOT-Rechteck auf den Hintergrund |
| `analysis/befunde_runde30/tools/r30_port_messung_auswertung.py` | Auswertung der Port-Messung |
| `build/r30_tuer-verschlossen/` | alle Ausgaben, `bank/`, `wav/`, `plaetze/` |

---

## UMSETZUNG (Bau-Agent, 2026-09-28, Zweig `r30/tuer-verschlossen`)

Gebaut sind die Schritte 1–7 aus §5. Schritt 8 (Paket) macht der Zusammenführer.

### Vorab selbst nachgeprüft (bevor eine Zeile Code entstand)

| Beleg | Binärdatei | Ergebnis |
|---|---|---|
| Handler-Tabelle `0x8007469c` → `[1] 0x80043084`, `[2] 0x800430bc` | RE1.5 `info/Re1.5/PSX.EXE` (md5 b55fdaa5…) | bestätigt; Text-Handler einziger Aufruf `@0x800430a0 jal 0x80027e68`; Tür-Handler ohne Schloss-Test, ohne SE |
| Tür-Handler `PTR_800a73c4[1] = 0x80051514`, `@0x8005160c lui a0,0x216` (Delay-Slot) / `@0x80051610 jal 0x8005ba28` / `@0x80051614 addu a1,zero,zero`; `@0x800516a0` / `@0x800516a4` / `@0x800516a8` gleich; Text `@0x800516b8 jal 0x8002fe38` | RE2 `info/re2leon/PSX.EXE` (md5 09a9b642…) | bestätigt |
| Sprungziel `0x8005ba28` = SE-Spieler: `@0x8005ba30 srl t1,a0,24` (Bank), `@0x8005ba7c/80` Satz, `@0x8005ba8c lw a1,-17544(at)` EDT-Zeiger `0x800dbb78[bank]`; Bank 2 = `[RDT+8]` (`@0x80059f1c lw a1,8(a2)` / `@0x80059f44 sw → 0x800dbb80`) | RE2 | bestätigt |
| ROOM2110 `@0x01BB6 2b 00 06 00 ff ff` / `@0x01BBC 36 02 16 00 00 00 44 a4 f8 f8 93 cc`; EDT-Satz 0x16 `@0x02EFC 00 00 74 00`; ROOM1140 `@0x01448 00 00 74 16` | RE2 RDTs | bestätigt |
| ROOM4000 sub02 `@0x01426` Ifel_ck … `@0x0142E 36 02 0f 00` … `@0x0143A 2b 00` | RE1.5 `shared_assets/PSX/STAGE4/ROOM4000.RDT` | bestätigt; sub02 beginnt genau bei `0x01426` |
| Ausgangszustand mit der Sonde | Port, Baum vor Schritt 3 | 740 Plätze / 57 ohne Standplatz / 328 Text / 211 Raumwechsel / 81 mit irgendeinem Ton — **jede Zeile bytegleich** zu `build/r30_tuer-verschlossen/port_messung.txt` |

### Gebaut

| Schritt | Datei(en) | Stand |
|---|---|---|
| 1 Bank | `shared_assets/RE2/TUERSE.VBS` (23832 B, md5 `8ff0c00c17017dcb15b68711411f4e52`), `engine/src/gen/re2_door_bank.inc` | Werkzeug-Selbsttest grün, md5 = Vorabzug |
| 2 Tabelle | `engine/src/gen/lock_se_sites.inc` (52 Stellen, 17 K / 35 M, 66 verworfen) | bytegleich zum Vorabzug |
| 3 Modul | `include/re15_lock_se.h`, `engine/src/lock_se_common.c` | Zuordnung Art → Satz als **zwei Defines an einer Stelle** (`RE15_LOCK_SE_SATZ_K` = `RE2_DOOR_SE_ZU_E`, `RE15_LOCK_SE_SATZ_M` = `RE2_DOOR_SE_ZU_A`), beide im Kommentar als „Port-Wahl, keine Original-Adresse" mit der Hüllkurven-Messung (A 9 / B 6 / E 12 Räume) |
| 4 Einhängen | `engine/src/scd_vm.c` | `re15_scd_show_message`: hinter Save-Telefon/Item-Box, unmittelbar vor `re15_dialog_open_mask`. `op_message_on`: neben `re15_discard_besitz_vor_nachricht`, also **hinter** der Stimmen-Warte-Schranke |
| 5 Audio | `include/re15_audio.h`, `platform/pc/src/audio_pc.c` (Slot 5b, Kopie des Fahrstuhl-Slots), `platform/psx/src/audio_psx.c` (Stub), `tests/test_support.c` (Spion `g_test_door_se_*`) | |
| 6 Riegel | `tests/unit/test_r30_tuer.c`, angemeldet in `tests/unit/probes/r30_tuer-verschlossen.cmake` | vier ctest-Einträge, s. u. |
| 7 Log | `RE15_TUERSE_LOG=<pfad>`: `F<bild> raum=… nachricht=… art=K/M weg=AOT/SKRIPT satz=n(ZU_x) nr=n` | Echtlauf s. u. |

Die Sonde `probe_r30_tuer_verschlossen` druckt jetzt zusätzlich am Zeilenende
`TUERTON=n satz=s KONTROLLE=k`; alle übrigen Spalten sind unverändert.

### Gemessen — Soll gegen Ist

| Riegel | Soll | Ist |
|---|---|---|
| (a) `unit_r30_tuerse_bank` | 7184 / 3232 / 10272 B, pitch 0x589 / 0x400 / 0x5f3, Stimme 6 / 6 / −16 | genau so; dazu Rohsatz `00 00 04 16` / `00 00 14 16` / `00 00 24 00`, Tone vol110 c85 s42 n66 / vol105 c91 s0 n67 / vol127 c84 s57 n66, 12544 / 5628 / 17948 Samples (0,822 / 0,510 / 1,095 s), Sätze 3–7 leer |
| (b) `unit_r30_tuer_verschlossen` | 98 Räume, 51 Plätze mit Tabellen-Nachricht (46 Stellen, 16 K / 35 M) je 1 Aufruf mit dem Satz der Art, sonst 0 | 740 Plätze (57 ohne Standplatz, 1 mit schon offenem Text); **51** Plätze (**46** Stellen, **16** K → Satz 2, **35** M → Satz 0) je genau 1 Aufruf; 689 übrige 0; Kontrolllauf ohne Druck an allen 740 Plätzen 0 |
| (b) Pflichtfälle | ROOM10D0 P0 → ZU_E · ROOM1170 P5 → ZU_A · ROOM1070 P4 → 0 · ROOM10D0 P17 → 0 · ROOM10D0 P14 → Raumwechsel, 0 | alle fünf erfüllt |
| (c) `unit_r30_tuer_r4000` | sub02 direkt: SCD-Se_on (2,0x0f) = 1, RE2-Tür-Ton = 0 | Se_on(2,0x0f) 1 ×, msg 0 geht auf, Tür-Ton 0 × |
| (d) `unit_r30_tuer_tabelle` | alle 52 Zeilen: gesetzter Weg → 1 Aufruf, nicht gesetzter → 0 | 52 Zeilen (K 17 / M 35), 53 Aufrufe (eine Zeile trägt beide Wege: ROOM3061 msg 0), jeder mit dem Satz der Art; nicht gesetzte Wege 0; jede Nachricht im ausgelieferten RDT ein Schloss-Text (`lock`/`latched`) und **keine** Ja/Nein-Abfrage (die parkt in `op_message_on` vor dem Einhängepunkt); 6 Gegenproben stumm (Spinde, Kartenleser, falscher/richtiger Code, Wegweiser, ROOM4090-Feld) |
| Bestand | alle Spalten außer dem neuen Tür-Ton unverändert | Sonde vorher gegen nachher über 740 Plätze: `diff --strip-trailing-cr` = **0 Zeilen** (snd1/snd0/CORE/RE2-Panel/RE2-Fahrstuhl/SCD-Se_on, Texte, Standplätze) |
| Gegenprobe Riegel | ein falsch verdrahteter Satz muss auffallen | M testweise auf ZU_B: `tabelle` 37 Fehler, `raeume` 36 Fehler |
| Suite | 360 (master) + neue | **364 / 364** grün |

### Echtlauf (echte `re15_pc.exe`, `analysis/befunde_runde30/tools/r30_tuer_echtlauf.sh`)

Weg: Titel → `RE15_DEBUG_JUMP` (Original-UTILITY-MENU-Pfad) → Eingabeskript (0,7 s links = rot −2016, QUADRAT).
Die Aufnahme läuft über `RE15_AUDIO_CAP_SYNC` (bildgetaktet), einmal mit und einmal ohne QUADRAT;
`analysis/befunde_runde30/tools/r30_tuer_audio_diff.py` vergleicht die beiden.

| Stelle | Log | Audio: Differenz mit/ohne Druck |
|---|---|---|
| ROOM10D0 Platz 0 (K), Spawn (1700,−7000) | `F228 raum=10D0 nachricht=6 art=K weg=AOT satz=2(ZU_E) nr=1`; Text offen ab F228 = **selbes Bild** | genau **ein** Abschnitt, 1,091 s, L == R (nicht positional); = Welle E mit **r = 0,9988**, Pegel 0,2498; außerhalb 0 Stereo-Frames verschieden |
| ROOM20A0 Platz 8 (M), Spawn (−2800,−22200) | `F228 … nachricht=1 art=M satz=0(ZU_A) nr=1`, `F474 … nr=2` (der zweite Druck öffnet den Text erneut → zweiter Ton) | Tonbeginn gegen A r = 0,8368 (gegen B 0,0487, gegen E 0,2646); auf den Samples, die zwischen Ton 1 und Ton 2 gleich sind (55 %), r = 0,9531. Die übrigen Unterschiede sind Raum-SEs, die der Text-Freeze zeitlich verschiebt (6-Bild-Ereignisse ab Tick 392, in beiden Läufen gleich gemustert) — kein Tür-Ton |

ROOM1170 Platz 5 ist im Echtlauf **nicht** gefahren: der Debug-Sprung spielt die Eintritts-Sequenz, danach steht
Leon bei (−31000,31000), rund 58000 Einheiten und eine Treppe vom Platz entfernt. Der Fall ist im Riegel (b) belegt.
Ein Hörbefund ist das alles nicht — gemessen ist, dass die richtige Welle zur richtigen Zeit im Mix liegt.

### Zum Anhören

`analysis/befunde_runde30/tuer-verschlossen_wav/`:
`welle_A_ZU_A_mechanisch_re2_1140_16.wav` (verdrahtet für Art M) ·
`welle_E_ZU_E_kartenleser_re2_2110_16.wav` (verdrahtet für Art K) ·
`welle_B_ZU_B_alternative_re2_1050_16.wav` (liegt als Satz 1 in `TUERSE.VBS`, nicht verdrahtet).
Tausch nach dem Anhören: in `engine/src/lock_se_common.c` die Zeile `RE15_LOCK_SE_SATZ_M` (bzw. `_K`) und im
Riegel `tests/unit/test_r30_tuer.c` die gleichnamige `SOLL_SATZ_*`-Zeile — der Riegel pinnt die Wahl absichtlich.

### ZUSATZ — Pincode-Türen

`analysis/befunde_runde30/tools/r30_tuer_pincode_zensus.py` (alle 206 RDTs mit Inhalt, Wortlaut
digit/code/keypanel/number): RE1.5 hat **drei Pincode-Türen** (Karte + vier Ziffern, „Enter the first number."
… „Wrong code, try again."). Ihre Verschlossen-Meldungen stehen **alle** als Art K in der Tabelle:

| Raum | Nachricht | Datei-Offset | Text |
|---|---|---|---|
| ROOM10D0 / 10D1 | msg 6 | `@0x01F3B` / `@0x01B77` | "Communication Room" It's electronically locked. There's a card reader on the right. |
| ROOM11E0 / 11E1 | msg 6 | `@0x021D9` | "Prisons" It's electronically locked. … |
| ROOM1230 / 1231 | msg 6 | `@0x015BF` | "Weapon Storage" It's electronically locked. … |

Das Ziffernfeld in ROOM4090/4091 (msg 5 `@0x00D03` „A numerical keypanel. An ID card is required to operate it.",
msg 6 „… I could use the machine …") bedient eine Maschine, keine Tür. **Es fehlt keine Pincode-Tür.**
Die Falscheingabe am Feld („Wrong code, try again.", msg 4) bleibt stumm wie in RE2 (ROOM20B0-Else-Zweig ohne Se_on, §3.4).

### Abweichungen vom Plan

1. **Riegel als EIN Programm mit vier ctest-Einträgen** (`test_r30_tuer <teil>`), statt zwei getrennter
   Programme. Grund: eine Verlinkung statt vier; die Namen `unit_r30_tuerse_bank` und
   `unit_r30_tuer_verschlossen` sind wie im Plan, dazu `unit_r30_tuer_r4000` und `unit_r30_tuer_tabelle`.
2. **Riegel (d) prüft zusätzlich**, dass jede Tabellen-Nachricht im RDT ein Schloss-Text und keine
   Ja/Nein-Abfrage ist — eine Abfrage parkt in `op_message_on` vor dem Einhängepunkt, dort fiele nie ein Ton.
3. **Das Datei-Log trägt die Bildnummer**, damit der Ton in der Audio-Aufnahme auffindbar ist.
4. **Keine Konstante ohne Beleg eingebaut.** Die beiden Zuordnungs-Defines sind die einzigen Wahlwerte;
   sie sind als Port-Wahl gekennzeichnet.

### Offen / nicht gemessen

- Die sechs flag-abhängigen Stellen (ROOM11A0/11A1 msg 1, ROOM11B1 msg 12, ROOM30C1 msg 10,
  ROOM40A0/40A1 msg 3) sind nur durch den Tabellen-Volllauf (d) belegt, nicht im Raum gefahren.
- ROOM1170 Platz 5 nicht im Echtlauf (s. o.), nur im Riegel.
- Stimmen-Kollision (§6 Punkt 5): ZU_A läuft über SE-Stimme 6 mit Priorität 4 und das Gate
  `FUN_80045a18`; ob ein Raum-SE auf derselben Stimme den Tür-Ton verdrängt, ist im Port nicht gemessen.
- PSX: nur Stub (wie Fahrstuhl/Panel).
- Paket (Schritt 8): `lock_se_common.c` ist eine neue Engine-Quelle → der eingefrorene Android-Configure
  unter `app/.cxx` muss verworfen werden; `RE2/TUERSE.VBS` in allen drei Paketen prüfen.

### Nachbesserung nach der Gegenprüfung (2026-09-28)

Der Gegenprüfer meldete drei Mängel (1 erheblich, 2 gering) und vier Konstanten ohne Beleg.
Jede seiner Byte-Angaben habe ich vor dem ersten Edit selbst nachgelesen.

#### Mangel 1 (erheblich): ROOM5080/5081 msg 2 fehlte — behoben, mit der Welle aus RE2 selbst

| Beleg | Datei | Bytes |
|---|---|---|
| RE1.5 legt den Text auf den Tür-Platz | `ROOM5080.RDT` sub02 @Datei 0x007C2 (5081: 0x007BA) | `46 00 01 31 02 00 ff ff 00 00` = Aot_reset Platz 0, sce 1, msg 2 |
| Platz 0 ist die Tür | `ROOM5080.RDT` main00 @Datei 0x006FA | `3b 00 02 31 …` Door_aot_set |
| Text | `ROOM5080.RDT` msg 2 @Datei 0x00996 (5081: 0x0098E) | „The door won't open until / the power is restored!" |
| RE2 hat **denselben Raum** | `ROOM7020.RDT` msg 0–3 | dieselben vier Generator-Texte; msg 2 @Datei 0x01F69 wörtlich gleich |
| RE2 spielt dazu einen Ton | `ROOM7020.RDT` sub06 @Datei 0x01598 / 0x0159E | `2b 00 02 00 ff ff` Message_on 2, `36 02 16 00 00 00 00 00 00 00 00 00` Se_on(2,0x16) |
| welche Welle | `ROOM7020.RDT` EDT @0x05CE8, Tone @0x06730, VAG 18 @0x1DD50 | `00 00 e3 00`, vol127 center102 shift57 note72, 6144 B, sha1 ea19d086e3cb (Familie P) |

**Abweichung vom Vorschlag des Gegenprüfers.** Er schlug Art K / ZU_E vor, weil P nicht in der Bank lag.
Für genau diesen Text gibt es aber einen direkten RE2-Beleg: RE2 spielt in diesem Raum die Welle P.
ZU_E wäre dagegen eine Port-Wahl gewesen. Deshalb habe ich die Bank um diesen einen Satz erweitert:

- `re2_door_se_cut.py`: Satz 3 **ZU_P** aus ROOM7020 (Selbsttest gegen alle vier Datei-Offsets). `TUERSE.VBS`
  wächst von 23832 auf **29976 B**, md5 **777e1f43e65f3c68941a635d12886a63**. Die Sätze 0–2 bleiben bytegleich.
- `gen_lock_se_sites.py`: neue Art **S** (ohne Strom) mit **einer** wörtlichen Formel
  `^The door won't open until the power is restored!$`. Die Tabelle hat jetzt **54 Zeilen (K 17 / M 35 / S 2)**.
  Der Diff gegen die alte Tabelle besteht genau aus den zwei neuen Zeilen, beide über den Weg AOT.
- `lock_se_common.c`: `RE15_LOCK_SE_SATZ_S = RE2_DOOR_SE_ZU_P`, mit dem vollständigen Beleg im Kommentar,
  ausdrücklich **keine** Port-Wahl.

**Vollständigkeit gemessen**, damit keine weitere Stelle fehlt. Es fanden sich keine weiteren Stellen, die aufzunehmen wären.
- Suche über alle Tür-Zwilling-Texte ohne Tabellenzeile (Gegenprüfer-Skript `twins.py`): Übrig bleibt nur ROOM4070/4071
  „The door won't open!". RE2 ist dort stumm: `PTR_800a73c4[4]` = 0x80051948 ruft nur @0x80051968 `jal 0x8002fe38`
  (selbst disassembliert).
- Wortsuche über alle 206 RDTs (seal/budge/inside/power/won't open/jam/block/broken/stuck/shut) gegen die
  17 RE2-Skript-`Se_on(2,0x16)`:
  - Rolltor „The shutter is tightly sealed in place." (ROOM11B0/11B1): RE2 legt seine Rolltor-Texte auf stumme
    Text-Plätze (ROOM60B0 sub00 @0x012C0, ROOM60C0 sub02 @0x01084, ROOM6160 sub00 @0x01EFC).
  - Tür mit kaputtem Knauf ROOM11D0/11D1 msg 1: RE2 hat dazu „The lock is broken and can't be opened.", ebenfalls
    ein stummer Text-Platz (ROOM2090 sub00 @0x00E7C).
  - Sonst nur Nicht-Türen: Aufzüge, Schalter, Geräte, Wasser, Kisten, ein Behälter, ein Spind.

**Tonhöhe belegt** (Konstante ohne Beleg Nr. 4). RE2 rechnet mit **demselben** `note2pitch2` wie RE1.5:
- Se_on 0x8005ba28 legt note = Tone[+6] ab (@0x8005bbec `lbu v0,6(s0)` / @0x8005bbf4 `sh v0,0xa(v1)`), ebenso
  fine = Tone[+5] (@0x8005bbf8 / @0x8005bc00 `sh v0,0xc(v1)`).
- Aufrufkette: FUN_8005c5e4 @0x8005c788 `jal 0x8007fdc8`, dann @0x80080110 `jal 0x80083010`
  (a0 = note @0x800800fc, a1 = fine @0x80080114). Das gilt, solange Tone[+0x16] ≠ 0xff (@0x8007ffa0 / @0x800800f0).
- 0x80083010 entspricht RE1.5 @0x80056b2c Befehl für Befehl (Diff: nur Adressen, ein `ori`/`addiu`, ein Register).
  Die 12×16-Tabelle RE2 @0x800aba40 ist **bytegleich** mit RE1.5 @0x80077520 (384 B, md5 6b1d457a486b2a3a5c17e9a040a00c12).

Damit gilt `re15_vab_note2pitch2` für alle vier Sätze. Die pitch-Werte im Riegel tragen diese Adressen.

#### Mangel 3 (gering): Riegel schützte die Lage hinter der Stimmen-Schranke nicht — behoben

Neuer Riegel-Teil **(f) `unit_r30_tuer_stimme`** mit den echten Bytes ROOM3091 @Datei 0x0159A `2b 06 ff ff`
(Message_on 6, Tabellenzeile Art M, Weg SKRIPT). Die Stimme läuft 40 Bilder lang (`g_re15_voice_laeuft`,
`g_re15_voice_restbilder` zählt je Bild herunter). Soll: während des Parkens 0 Töne, nach der Freigabe genau 1.

**Negativkontrolle wie beim Gegenprüfer:** Ich habe den Haken an den Anfang von `op_message_on` verschoben.
Dann parken 40 Bilder mit **40 Tönen**, insgesamt fallen **41**, und (f) ist **ROT**. Mit zurückgesetztem Code ist (f) wieder grün.

Zusätzlich neuer Teil **(e) `unit_r30_tuer_strom`**:
- Frisch ist Platz 0 in ROOM5080/5081 die Tür (Raumwechsel, 0 Töne).
- Danach laufen die **echten** zehn Aot_reset-Bytes aus sub02 als Thread. Platz 0 ist dann ein Text-Platz mit msg 2.
- Ohne Druck fallen 0 Töne, mit Druck geht msg 2 auf und es fällt **genau 1 × ZU_P**.

Negativkontrollen:
- Ohne die S-Zeilen geht msg 2 auf, aber es fallen **0 Töne**, (e) ist ROT. Das ist der gemeldete Mangel, reproduziert.
- Mit S = ZU_E fällt Satz 2 statt 3, (e) und (d) sind ROT.

#### Mangel 2 (gering): ROOM1100/1101 „It's electronically locked." (Sprachgerät-Tür) — NICHT geändert, Frage an den Nutzer

Die Angaben des Gegenprüfers habe ich nachgelesen:
- RE2 1100 sub05 @Datei 0x0304E und 2000 sub09/sub10 @Datei 0x02CAC/0x02CCA sind jeweils `36 02 16 00 …`.
- Der Raumbank-Satz 0x16 löst in 1100 und 2000 auf **cf1414572aea** auf (Welle A), in 2110 auf **be2f6ea9caaa** (Welle E).
- RE2s eigene „electronically locked"-Türen **ohne** Kartenleser klingen im Revier also nach **A**.

Der Port folgt der Vorgabe „Art K (Kartenleser/elektronisch) → E". Deshalb bekommt 1100/1101 heute E.
Die Tür ist weder Kartenleser noch Pincode. Sowohl die Worte des Nutzers als auch die RE2-Bytes sprächen für A.
Weil das eine Wahl gegen die Vorgabe wäre, habe ich es **nicht** eigenmächtig geändert.

**Frage an den Nutzer beim Anhören:** Soll die Sprachgerät-Tür 1100/1101 E (wie der Kartenleser) oder A
(wie RE2s elektronisch verriegelte Revier-Türen) bekommen?

Umsetzung von A: eine Formel `^It's electronically locked\.$` → Art M **vor** die K-Formeln in
`gen_lock_se_sites.py`, dann die Tabelle neu erzeugen und im Riegel die K/M-Zahlen anpassen (K 15, M 37).
Andere Zeilen trifft das nicht: 4080/4081 tragen den Raumnamen vor dem Text, die Kartenleser-Türen den Zusatz „There's a card reader…".

#### Die übrigen „Konstanten ohne Beleg"

| Konstante | Stand |
|---|---|
| `RE15_LOCK_SE_SATZ_K = ZU_E` | unverändert Vorgabe, im Code wörtlich „Port-Wahl, keine Original-Adresse" + Messung (s. Mangel 2) |
| `RE15_LOCK_SE_SATZ_M = ZU_A` | unverändert, ebenso gekennzeichnet (A 9 Revier-Räume gegen B 6) |
| `FORMELN` | im Generator-Kopf jetzt wörtlich „Port-Wahl, keine Original-Adresse", dazu die Vollständigkeitsmessung von oben. Belegt ist je Art die **Welle**, bei S zusätzlich der Wortlaut |
| pitch 0x589/0x400/0x5f3 (+ neu 0x2f9) | **belegt**, RE2-Kette oben |

#### Gemessen — Soll gegen Ist (Nachbesserung)

| Riegel / Messung | Soll | Ist |
|---|---|---|
| (a) `unit_r30_tuerse_bank` | 29976 B, 4 Sätze, Satz 3 = `00 00 33 00`, pitch 0x2f9, 10724 Samples, Sätze 4–7 leer | genau so; Satz 3 Stimme −16 prio 3 vol127 center102 shift57 note72, 6144 B, 8193 Hz, **1,309 s**; Sätze 0–2 unverändert (0x589 / 0x400 / 0x5f3) |
| (b) `unit_r30_tuer_verschlossen` | unverändert 51 Plätze (K 16 / M 35), Art S im Frisch-Zustand 0 | 740 Plätze, 57 ohne Standplatz, **51** (K 16 / M 35 / **S 0**), 46 Stellen, 689 übrige 0 — PASS |
| (c) `unit_r30_tuer_r4000` | Se_on(2,0x0f) 1 ×, Tür-Ton 0 | 1 × / 0 — PASS |
| (d) `unit_r30_tuer_tabelle` | 54 Zeilen (17 / 35 / 2), gesetzter Weg 1, sonst 0 | **54** Zeilen, **55** Aufrufe, je Satz der Art; **9** Gegenproben stumm (neu: ROOM4070 msg 0, ROOM11B0 msg 14, ROOM5080 msg 3) — PASS |
| (e) `unit_r30_tuer_strom` (neu) | frisch Raumwechsel + 0; nach Aot_reset ohne Druck 0, mit Druck msg 2 + 1 × ZU_P | 5080: 1/0 · 0 · msg 2, 1 × Satz 3; 5081 ebenso — PASS |
| (f) `unit_r30_tuer_stimme` (neu) | geparkt 0, danach genau 1 × ZU_A | 40 Bilder 0 Töne, Text zu; danach **1** Ton, Satz 0, msg 6 offen — PASS |
| Negativkontrollen | jede muss ROT werden | Haken vorn → (f) 40/41 ROT · ohne S-Zeilen → (e) 0 Töne ROT, (d) ROT · S = ZU_E → (e)+(d) ROT |
| Bestand | Sonde über die 98 Räume unverändert | `probe_r30_tuer_verschlossen` gegen den Gegenprüfer-Lauf `probe_nachher.txt`: **0 Zeilen** Unterschied (740 Plätze, 81 mit Ton) |
| Suite | 364 + 2 neue | **366 / 366** grün |

#### Nicht gemessen

- **Echtlauf ROOM5080 msg 2 in der echten `re15_pc.exe`: nicht erreicht.**
  - Debug-Sprung, Weg zum Generator und dessen Text-Folge klappen: Leon steht bei (−9950,−18200) und läuft nach
    (−26483,−18200). Zwei Texte sind offen, F247–F499 und F501–F835 (sub02 öffnet msg 0 und msg 1;
    das Log führt die Nummer nicht).
  - Danach kam Leon in vier Anläufen nicht zurück an die Tür:
    - Auf der Linie z ≈ −18150 bleibt er bei x ≈ −25440 hängen. Auf dem Hinweg lief er dort durch.
    - In zwei Läufen bewegte ihn nach einer Wandberührung der folgende Laufbefehl nicht mehr.
      In einem Lauf wanderte er ohne Eingabe langsam nach −x.
  - Das fällt in die Generator-Folge von sub02 (Plc_dest @Datei 0x007F4 Ziel (−18100,−17800), Do/Edwhile
    @0x007FC auf Bit (5,32), Plc_ret @0x00818). Das ist nicht Teil dieser Spur und nicht untersucht.
    Skript und Logs: `build/r30_echt_5080_*` (nicht committet).
  - Für den Weg AOT ist dieselbe Einhängestelle im Echtlauf belegt (ROOM10D0 / ROOM20A0, oben).
  - Satz 3 läuft über denselben Slot `se_play_layers` wie Satz 2. Ein Hörbefund ist das nicht.
- Ob der Nutzer mit „ROOM2190" ROOM2110 meinte (RE2 ROOM2190 Satz 0x16 ist leer, §3.6): weiter offen.

#### Paket (Schritt 8, für den Zusammenführer)

- `RE2/TUERSE.VBS` ist jetzt **29976 B**, md5 **777e1f43e65f3c68941a635d12886a63** (nicht mehr 23832 B / 8ff0c00c…).
  Die Datei in allen drei Paketen prüfen.
- `lock_se_common.c` ist neu. Deshalb den Android-Cache `app/.cxx` verwerfen.
- Suite-Zahl: +6 Einträge aus `probes/r30_tuer-verschlossen.cmake` (vorher 4).

#### Zum Anhören (ergänzt)

`welle_P_ZU_P_ohne_strom_re2_7020_16.wav`: RE2 ROOM7020 Satz 0x16, 8193 Hz, 1,309 s. Das ist der Ton an der
Tür ohne Strom, RE2s eigene Wahl für genau diesen Text.
