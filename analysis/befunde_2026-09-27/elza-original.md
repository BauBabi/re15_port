# Elza — der Original-Mechanismus (Ermittlung, kein Bau)

Stand 2026-09-27. Alle Zahlen selbst disassembliert aus `info/Re1.5/PSX.EXE`
(t_addr 0x80010000, Text ab Datei-0x800) und den Overlays unter
`info/Re1.5/PSX/BIN/`. Decompilate dienten nur als Wegweiser, nie als Beleg —
an zwei Stellen war das Decompilat nachweislich falsch (§1.3, §2 Fussnote).

⛔ **Overlay-Ladeadressen:** STAGE*.BIN und TITLE.BIN laden roh @0x80100000,
**DEBUG.BIN aber @0x800c0000**. Der Skript-Default ist 0x80100000; alle
DEBUG.BIN-Adressen in diesem Dossier sind auf 0x800c0000 korrigiert. Beleg:
der Code an DEBUG-Datei-Offset 0xac referenziert `0x800c00d4` als *eigene* Daten
(`lui t0,0x800c / addiu t0,t0,212`), und die EXE springt ihn als `jal 0x800c00a8`
@0x80031680 an.

---

## KERNBEFUND IN EINEM SATZ

Das Original trägt den Charakter als **PLD-Index im niederen Nibble von `0x800aca5c`**
(Leon = 0 = PL00, Elza = **4** = PL04 — **nicht** 1) und spiegelt ihn als **Bit 31 des
Wortes `0x800aca3c`**. Die Raum-Id wird **nicht** umgerechnet: der RDT-CD-Dateiindex ist
`RaumTabelle[stage][raum] + Elza-Bit` — **zwei Instruktionen**, `srl a0,a0,31`
@0x800397e4 und `addu a0,a0,v0` @0x800397ec. Leon- und Elza-RDT liegen als
Nachbarn im CD-Dateiverzeichnis.

---

## 1. WO SPEICHERT DAS ORIGINAL DIE CHARAKTERWAHL?

### 1.1 Das Feld: `0x800aca5c`, ein BYTE

Zugriff immer `lbu`/`sb`, nie `lhu`/`lw`
(`80026f4c: lbu v0,-13732(v0)`). Vollständiger Xref-Scan (eigener
lui/addiu-Registerverfolger über PSX.EXE **und alle 8 Overlays**).
**Es gibt genau fünf Schreiber:**

| Adresse | Binär | Instruktion | Bedeutung |
|---|---|---|---|
| `0x801016ac` | TITLE.BIN | `sb v0,-13732(at)` | Charakterwahl, `v0 = [menu+916] << 2` |
| `0x801024c0` | TITLE.BIN | `sb zero,-13732(at)` | Zweig Leon → 0 |
| `0x801024d4` | TITLE.BIN | `sb v0,-13732(at)` (`ori v0,zero,0x4` @0x801024cc) | Zweig Elza → **4** |
| `0x8001d4f4` | PSX.EXE | `sb v0,-13732(at)` | aus Save-Header, `lbu v0,4030(v0) ; 0x800b0fbe` @0x8001d4c8 |
| `0x8003978c` | PSX.EXE | `sb v0,0(s0)`, s0 = 0x800aca5c | Modellwechsel, niederes Nibble := `[0x800b0ff0]` |

### 1.2 Die WERTE sind 0 und 4, nicht 0 und 1

```
80101684: lui  v0,0x8010
80101688: lw   v0,9920(v0)          ; 0x801026c0 = Menü-Struktur
8010169c: lbu  v0,916(v0)           ; Cursor der Charakterwahl
801016a4: sll  v0,v0,2              ; <<2  ->  0 oder 4
801016ac: sb   v0,-13732(at)        ; 0x800aca5c
```
```
801024a4: bne  a1,zero,0x801024cc   ; a1 = Cursor (lbu a1,0(s0) @0x80102488)
801024c0: sb   zero,-13732(at)      ; Leon -> 0
801024cc: ori  v0,zero,0x4
801024d4: sb   v0,-13732(at)        ; Elza -> 4
```

**Das `<<2` ist der Beweis:** der Wert ist kein 0/1-Schalter, sondern der **PLD-Index**.
`FUN_800314b0` (Spielermodell laden) liest ihn @0x800314d4 und indiziert damit

```
read 0x80073f70 16 x u16 = [60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75]
```

16 Einträge = PL00..PL0F, CD-Dateiindex 60..75. Index 0 → 60 = **PL00.PLD** (Leon),
Index 4 → 64 = **PL04.PLD** (Elza).

**Die Datenlage bestätigt es unabhängig:** `info/Re1.5/PSX/PLD/` enthält genau **zwei**
PLD mit vollständigem Waffensatz W00..W14 — **PL00** und **PL04**. Alle anderen
(PL01,02,03,05..0E) sind waffenlose Modelle; PL0F hat genau eine PLW.

**Der Diskriminator ist Bit 2** (`& 4`), und die Datentabellen zeigen es wörtlich.
Die fünf 16×2-Byte-Tabellen ab 0x80073ea4 (Index = `aca5c*2`) gruppieren so:

```
0x80073ea4:  4b30 4b30 4b30 4b30 | 4630 4630 4630 4630 | 4b30 4b30 4b30 4b30 | 4630 4630 4630 3230
             ^--- Index 0..3 ----^ ^--- Index 4..7 ----^ ^--- 8..11 ---------^ ^--- 12..15 -------^
             Leon-Gruppe           Elza-Gruppe           Leon-Gruppe           Elza-Gruppe
```
Der Wechsel liegt exakt auf Bit 2. `& 4` ist also die Original-Abfrage, und sie ist
korrekt — im Port ist sie nur **unerreichbar**, weil `re15_gameflow.c:38`
`character = 0|1` setzt und `(0|1) & 4 == 0` ist. Die Portabbildung muss **0 und 4**
lauten.

### 1.3 Das Nibble-Paar — Decompilat vs. echte Instruktionen

`RE_15_Quellcode_V2/FUN_800396fc.c:15` behauptet
`if ((DAT_800aca5c & 0xf) != 0) { DAT_800aca5c = DAT_800aca5c & 0xf0; … }`.
**Falsch.** Die Instruktionen:

```
80039758: lui  s0,0x800b
8003975c: addiu s0,s0,-13732        ; s0 = 0x800aca5c
80039760: lbu  a0,0(s0)
80039768: lh   v1,4080(v1)          ; 0x800b0ff0 = ANGEFORDERTER PL-Index
8003976c: andi v0,a0,0xf            ; aktueller PL-Index
80039770: beq  v0,v1,0x80039790     ; gleich -> nichts tun
80039774: andi v0,a0,0xf0           ; hohes Nibble bleibt erhalten
8003977c: lbu  v1,4080(v1)
80039784: or   v0,v0,v1
80039788: jal  0x800314b0           ; Spielermodell neu laden
8003978c: sb   v0,0(s0)             ; 0x800aca5c = (alt & 0xf0) | neu
```

Also: **niederes Nibble = aktuell geladener PL-Index**, `0x800b0ff0` (u16) =
angeforderter PL-Index; das hohe Nibble ist **nicht** der Charakter. Gefüllt wird
`0x800b0ff0` beim Sitzungsstart aus `0x800aca5c`: `8001d558: sh a0,4080(at)`
mit `a0 = lbu 0x800aca5c` @0x8001d51c.

### 1.4 Der Zwilling: Bit 31 von `0x800aca3c`

Beide Setz-Pfade spiegeln die Wahl sofort ins Wort `0x800aca3c`:

TITLE.BIN
```
801016bc: beq  v0,zero,0x801016d4   ; v0 = 0x800aca5c
801016c0: lui  v1,0x8000            ; Elza:  Maske 0x80000000
801016d0: or   v0,v0,v1             ;        0x800aca3c |= 0x80000000
801016d4: lui  v1,0x7fff            ; Leon:  Maske 0x7fffffff
801024b8: ori  v1,v1,0xffff
801024c8: and  v0,v0,v1             ;        0x800aca3c &= 0x7fffffff
801024ec: sw   v0,-13764(at)        ; 0x800aca3c
```
PSX.EXE, Sitzungsstart, gleiche Bauform: `lw 0x800aca3c` @0x8001d514,
`lui v1,0x8000` @0x8001d520, `and v0,v0,v1` @0x8001d53c, `sw` @0x8001d544.

### 1.5 Persistenz im Speicherstand

```
80026f4c: lbu v0,-13732(v0)   ; 0x800aca5c
80026f6c: sb  v0,4030(at)     ; 0x800b0fbe  = Save-Header-Byte
```
Rückweg beim Laden: `8001d4c8 lbu v0,0x800b0fbe` → `8001d4f4 sb v0,0x800aca5c`.

⛔ **Eine Unstimmigkeit im ORIGINAL, die ich NICHT wegvermute:** der
Speicherkarten-Titel testet **Bit 0** desselben Bytes —
```
80026e58: lbu  v0,4030(v0)          ; 0x800b0fbe
80026e70: andi v0,v0,0x1
80026e74: beq  v0,zero,0x80026e8c   ; ==0 -> Leon-Template 0x800107f8
80026e80: addiu a1,a1,1996          ; !=0 -> Elza-Template 0x800107cc
```
Nimmt `0x800aca5c` nur 0 oder 4 an, ist Bit 0 immer 0 und das Elza-Template wird nie
erreicht. Einen Schreiber, der Bit 0 setzt, gibt es nicht — der Xref-Scan über EXE und
alle acht Overlays listet ausschliesslich die fünf Schreiber aus §1.1. **Ich fülle diese
Lücke nicht.** Der Port (`re15_mc_title.c:38`) benutzt seinen eigenen 0/1-Wert und trifft
damit das, was an dieser Stelle offenbar gemeint war.

---

## 2. WIE ENTSTEHT DIE RAUM-ID? — Das Herzstück

**Es gibt keine Id-Arithmetik und keinen Dateinamen.** Ein String-Scan über die
gesamte PSX.EXE findet kein `ROOM…`/`.RDT`/`%04x`-Dateinamen-Muster. Geladen wird
über einen **CD-Dateiindex**. Die Elza-Variante entsteht durch **+1 auf diesen Index**:

```
; FUN_800396fc — Raum laden
800397a8: lh   v0,4064(v0)          ; 0x800b0fe0 = Stage
800397b0: lh   v1,4066(v1)          ; 0x800b0fe2 = Raum
800397b8: lw   a0,-13764(a0)        ; 0x800aca3c  (Bit31 = Elza)
800397c4: sll  v0,v0,2              ; Stage*4
800397cc: addiu at,at,17292         ; 0x8007438c = Stage-Zeigertabelle
800397d0: addu at,at,v0
800397d4: lw   v0,0(at)             ; -> Raumtabelle dieser Stage
800397d8: sll  v1,v1,1              ; Raum*2
800397dc: addu v1,v1,v0
800397e0: lhu  v0,0(v1)             ; Basis-Dateiindex (Leon)
800397e4: srl  a0,a0,31             ; a0 = Elza-Bit, 0 oder 1
800397e8: jal  0x80013b60           ; CD-Laden nach Index
800397ec: addu a0,a0,v0             ; INDEX = Basis + Elza
```

`0x800397e4` und `0x800397ec` sind die **gesamte** Charakter-Logik der Raumwahl.
(Fussnote: das Decompilat rendert das als `- ((int)DAT_800aca3c >> 0x1f)`. Die
Instruktion ist `srl` — logisch, Ergebnis 0/1 — und `addu`, nicht `subu`.)

Stage-Zeigertabelle @0x8007438c, sechs Einträge:

| Stage | Raumtabelle | Einträge |
|---|---|---|
| 0 | 0x8007429c | 40 |
| 1 | 0x800742ec | 16 |
| 2 | 0x8007430c | 16 |
| 3 | 0x8007432c | 16 |
| 4 | 0x8007434c | 24 |
| 5 | 0x8007437c | 8 |

Stage 0, `read 0x8007429c 40 --w 2`:
`[681, 684, 687, 690, 693, 696, 699, 702, 705, … , 798]` — **Schrittweite 3**.
Drei Dateien je Raum, genau wie `info/Re1.5/PSX/STAGE1/` es zeigt:
`ROOM105.BSS`, `ROOM1050.RDT`, `ROOM1051.RDT`. Der Tabellenwert zeigt auf die
Leon-RDT, `+1` ist die Elza-RDT.

**Damit ist der Dateiname aufgelöst:** `ROOM<stage+1><raum:2 hex><spieler>.RDT`.
ROOM**1**·**05**·**0** = Stage-Index 0, Raum 0x05, Spieler 0 (Leon);
ROOM**1**·**05**·**1** = derselbe Raum, Spieler 1 (Elza). 40 Räume 0x00..0x27
in Stage 0 = 80 RDT — genau der 40/40-Schnitt aus der Aufgabenstellung.

**Die Tür liefert nur Stage und Raum, nie den Charakter.** FUN_8001d600:
`_DAT_800b0fe2 = *(byte*)(DAT_800ac9a8 + 9)` (Zielraum aus dem Tür-Record),
`DAT_800b0fe0 = *(byte*)(DAT_800ac9a8 + 4)` (Ziel-Stage); Schreiber
`sh v0,0x800b0fe2` @0x8001d660 und `sh v0,0x800b0fe0` @0x8001d808 / @0x8001d97c.
**Für den Port heisst das: EINE Türtabelle, EIN Raum-Bezeichner — der Spieler-Digit
wird erst beim Dateizugriff angehängt.** Kein Spiegel-Datensatz, keine zweite
Raumliste.

---

## 3. WO IST DER EINSTIEG?

Sitzungsstart ist `FUN_8001d22c` (Prolog `addiu sp,sp,-24` @0x8001d22c); sie endet mit
`jal 0x800314b0` @0x8001d5a4 (Spielermodell) und `jal 0x800396fc` @0x8001d5ac (Raum).

**Die Weiche Neues Spiel / Laden:**
```
8001d284: lw   v0,-13768(v0)        ; 0x800aca38
8001d288: lui  v1,0x2               ; Maske 0x20000
8001d28c: and  v0,v0,v1
8001d290: beq  v0,zero,0x8001d49c   ; Bit AUS -> LADEN (Position aus Save-Header
                                    ;   0x800b0fc2, lhu @0x8001d4a0)
```
Bit 0x20000 setzt TITLE.BIN im Charakterwahl-Bildschirm:
`lui a0,0x2` @0x80102474, `or v0,v0,a0` @0x80102490, `sw v0,0x800aca38` @0x80102498 —
unmittelbar vor den Charakter-Stores @0x801024c0/@0x801024d4.

**Die Charakter-Weiche des neuen Spiels:**
```
8001d29c: lw   v0,-13764(v0)        ; 0x800aca3c
8001d2a4: bltz v0,0x8001d324        ; Bit31 gesetzt = ELZA
```

| | Leon (Fall-through @0x8001d2a8) | Elza (@0x8001d324) |
|---|---|---|
| Raum `0x800b0fe2` | `ori v0,zero,0x17` @0x8001d2a8, `sh` @0x8001d2b0 | `ori v0,zero,0x3` @0x8001d324, `sh` @0x8001d32c |
| Stage `0x800b0fe0` | `sh zero` @0x8001d310 | `sh zero` @0x8001d368 |
| X | 0x952 = 2386 @0x8001d2b4/c0 | −8888 @0x8001d330/3c |
| Y `0x800aca8c` | −7200 @0x8001d308/18 | 0 @0x8001d388 |
| Z | 0x35b1 = 13745 @0x8001d2cc/d8 | −12989 @0x8001d348/54 |
| Winkel `0x800acabe` | −2904 @0x8001d2e4/ec | −2960 @0x8001d360/70 |
| Etage `+0x82` | 4 @0x8001d2f0/f8 | 0 @0x8001d378 |

**→ Leon startet in Raum 0x17 = ROOM1170, Elza in Raum 0x03 = ROOM1031. Beide Stage 0.**

**Unabhängige Gegenprobe aus den Daten.** Direkt danach überschreibt dieselbe Funktion
die Position aus einer Tabelle @0x800c263c (in DEBUG.BIN), Indexformel aus den
Instruktionen 0x8001d39c–0x8001d3c8: `offset = (637*stage + 13*raum) * 2`, also ein
26-Byte-Satz je Raum. Die Sätze enthalten X (s16), Z (s16), Etage (u8) **und den
Klartext-Raumnamen**. Gelesen aus `info/Re1.5/PSX/BIN/DEBUG.BIN` (Basis 0x800c0000):

```
raum 0x03  ROOM103x   X= -8200  Z=-22500  fl=0   "LOBBY"
raum 0x17  ROOM117x   X=  3940  Z= 14167  fl=4   "HELIPORT"
raum 0x24  ROOM124x   X=-26214  Z= -3861  fl=0   "OPENING"
```

Die **Etagen** stimmen exakt mit den fest verdrahteten Konstanten überein (4 bzw. 0),
die X/Z liegen in derselben Ecke des Raums. Und die Namen passen zur Erzählung:
**Leon kommt auf dem HELIPORT an, Elza startet in der LOBBY.** Die Stage-Schrittweite
637 ist mitgeprüft — Raum 0 der Stages 0..5 heisst "BATH-LOCKERS", "L TUNNEL",
"FACT. ENTRANCE", "LAB ENTRANCE", "A-2 ENTRANCE", "GATE PLATFORM".

⛔ **Zum Port:** `RE15_NEWGAME_ROOM = 0x1240` (`re15_gameflow.c:16`) ist Raum 0x24 =
**"OPENING"** — weder Leons noch Elzas Original-Einstieg. Das ist eine
Port-Entscheidung (Vorspann-Raum), keine Ableitung aus dieser Funktion. Wer die
Charakterwahl byte-true verdrahtet, muss diese Konstante mit angehen.

⛔ **Offen und nicht geraten:** die Positions-Überschreibung aus 0x800c263c läuft in
diesen Instruktionen **unbedingt** für beide Zweige. 0x800c263c liegt ausserhalb des
EXE-Abbilds (t_size 0xaf000 → EXE endet 0x800bf000) und gehört DEBUG.BIN. Ob im
Auslieferungsstand DEBUG.BIN zu diesem Zeitpunkt resident ist, habe ich **nicht**
gemessen. Der **Raum** ist davon unberührt — der steht in beiden Fällen fest.

---

## 4. WAS HÄNGT NOCH AM CHARAKTER? — Bauliste für die zweite Welle

Alle Lesestellen von `0x800aca5c` aus dem Vollscan, nach Wirkung sortiert.

| # | Was | Beleg | Leon → Elza |
|---|---|---|---|
| 1 | **Spielermodell PLD** | FUN_800314b0, `lbu` @0x800314d4 → Tabelle 0x80073f70[PL] | CD-Index 60 (PL00) → 64 (PL04) |
| 2 | **Waffenmodell PLW** | FUN_80036b68, `lbu` @0x80036df8, Tabelle 0x800741e8 + PL*2, plus Waffen-Id `0x800aca5d` @0x80036e00 | Basis 76 → 97 (PL0F: 118). Deckt sich mit PL00W00..W14 (76..96), PL04W00..W14 (97..117), PL0FW00 (118) |
| 3 | **Startwaffe** | EXE `jal 0x800c00a8` @0x80031680 → DEBUG.BIN `lbu v0,[0x800c00d4 + PL]` @0x800c00bc → `sb v0,0x800aca5d` @0x80031690. Tabelle 0x800c00d4 = `01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 00` | beide 1; nur PL0F = 0 |
| 4 | **Waffen-Reichweite/-Werte** | FUN_80011f50: `andi v1,v1,0x4` @0x80012088, `sltu` @0x8001208c, Zeilenabstand 0x58 @0x80012090-a0, Basis 0x8006e5a0 @0x80012084 | ⛔ **beide Zeilen sind BYTE-IDENTISCH** (0x8006e5a0 und 0x8006e5f8, je 0x58 Byte, gemessen) — der Mechanismus existiert, im Auslieferungsstand ohne Unterschied |
| 5 | **Waffen-/Ziel-Geometrie (5 Tabellen)** | Lesestellen 0x80032348, 0x80032388, 0x800324bc, 0x80032630, 0x80032670, 0x800327d0, 0x800329b0, 0x800329f0, 0x80032bf4, 0x80032c34, 0x80032d44 — je `sll v0,v0,1` + Tabellenbasis | 0x80073ea4: 75→70 · 0x80073ec4: 70→65 · 0x80073ee4: gleich · 0x80073f04: 60→55 · 0x80073f24: 200→210 (2. Byte 48/48/96/48/72 konstant). Verrechnet mit `0x800acabe` (Spielerwinkel) |
| 6 | **Effekt-Ankerpunkt** | 0x80034864 / 0x80034974 / 0x80034ab0 / 0x80034b30: `andi t0,t0,0x4` → `0x46 - 10*e` @0x80034884-88 und `0x41a - 30*e` @0x8003489c-a0 | 70 → 60 und 1050 → 1020 |
| 7 | **Kopf-/Nacken-Feder (Plc_neck)** | FUN_80031c44: `lbu` @0x80031d84, `andi v0,v0,0x4` @0x80031d8c, `beq` @0x80031d90, `jal 0x80024c30` @0x80031d98 auf `0x800aca54` | **nur Elza.** (FUN_80024c30 = Neck-Damped-Spring, `RE15_FUN_CATALOG.md:44`, dort bereits mit demselben Gate `DAT_800aca5c&0x4` notiert) |
| 8 | **Effekt-/Sprite-Satz beim Laden** | `lbu` @0x80045e20, `sltiu v0,v0,0x4` @0x80045e28, `beq` @0x80045e2c | PL<4: Satz 0x80074bb8 = `{01,03,15}`; sonst 0x80074bc4 = `{01}`. Ablage 1200 Byte je Eintrag (`*5*15*16` @0x80045e54-64) in den 0x8010xxxx-Bereich |
| 9 | **Gegner-/Overlay-Reaktion auf Entity-Typ 0x19** | `lbu` + `andi 0x4` + `bne`: STAGE1 @0x80104000/08/0c · STAGE2 @0x80103e94/9c/a0 · STAGE3 @0x801040ec · STAGE4 @0x80103fb4 · STAGE5 @0x80104134 · STAGE6 **keiner** | Leon-Zweig: `sb 1,440(entity)` @0x80104030. Elza-Zweig @0x801040c4: `[entity+472] \|= 2` @0x801040e8, `jal 0x800453d0(9)` @0x801040ec, RNG-Bit @0x801040fc → ggf. `jal 0x800453d0(5)` |
| 10 | **Speicherkarten-Blocktitel** | 0x800b0fbe Bit 0 @0x80026e70; Templates 0x800107cc (Elza) / 0x800107f8 (Leon) | siehe die Einschränkung in §1.5 |
| 11 | **Startraum + Startpose** | FUN_8001d22c @0x8001d2a4 ff. | §3 |
| 12 | **RDT-Auswahl** | @0x800397e4 / @0x800397ec | §2 |
| 13 | **Modellwechsel zur Laufzeit** | `0x800b0ff0` (angeforderter PL) ↔ `0x800aca5c` niederes Nibble, @0x80039758-8c; gefüllt @0x8001d558 | trägt Cutscene-Modelle (PL0F u.a.), nicht nur Leon/Elza |
| 14 | **PL0F-Sonderfälle** | `aca5c == 0xf`: @0x80032584-8c, @0x80032890-98, @0x80038188-90 | eigener Zweig, weder Leon noch Elza |

Ebenfalls Charakter-abhängig, aber **Zweck nicht bewiesen** (Adresse belegt, Deutung
offen — ich schreibe sie hier hin, statt sie zu raten):
`0x8003165c`, `0x800316dc`, `0x800319d8/0x800319f0` (`sltiu v0,v0,0x2` — PL 0/1),
`0x80031a80/0x80031a98` (`-4`, `sltiu 0x2` — PL 4/5), `0x80037c8c/0x80037cb8`
(Tabellen 0x80074208/0x80074209, Schrittweite 0x10 — Wund-/Blut-Decals,
FUN_80037c1c), DEBUG.BIN `0x800c01d0`, `0x800c4618`, `0x800c4730`
(`0x800c4618` ist eine Kopie von #6).

---

## 5. RE2-GEGENPROBE

RE2 löst dieselbe Aufgabe **mit derselben Tabellenarchitektur, aber einem anderen
Diskriminator** (`info/re2leon/PSX.EXE`, t_addr 0x80010000, t_size 0xf0800):

```
8004a15c: sll  v1,v1,2              ; Stage*4
8004a168: lw   v1,29200(at)         ; 0x800a7210 = Stage-Zeigertabelle
8004a16c: sll  v0,v0,1              ; Raum*2
8004a174: lhu  a0,0(v0)             ; Basis-Dateiindex
8004a180: addiu a0,a0,125           ; +0x7d   <-- der EINE Unterschied
…
8004a1b8: lhu  a0,0(v1)             ; der Zweig OHNE Aufschlag
```

Gleiche Bauform wie RE1.5 (`0x8007438c` / `srl 31` / `addu`), aber:

* der Aufschlag ist **+125**, nicht +1, und die Weiche ist `DAT_800d44a0`
  (`FUN_80049e48`, Zeilen 92–97) — **nicht** der Spieler;
* **Leon und Claire liegen in getrennten Verzeichnissen**: `info/re2leon/PL0/RDT/`
  enthält 495 RDT, und **jede einzige** endet auf Spieler-Digit `0`. Kein `…1.RDT`.
  Der Charakter ist also die Disc/das Verzeichnis (PL0 vs. PL1), nicht der Index;
* **Szenario A/B steckt in der Stage-Ziffer**: die Stages laufen 1..7 **und** A..G,
  also `stage + 9` (ROOM1000…ROOM70xx vs. ROOMA000…ROOMG0xx).

**Architektur-Fazit:** RE2 trennt die Charaktere über die Datenträger-Struktur, RE1.5
über benachbarte Dateiindizes in *einem* Verzeichnis. RE1.5s Lösung ist die
kompaktere und ist im Port mit **einer** Zeile abbildbar: Raum-Id bleibt
charakterfrei, der Spieler-Digit kommt erst beim Öffnen der Datei dazu.

---

## 6. WAS DER PORT DAFÜR BRAUCHT (keine Umsetzung, nur die Folgerung)

1. `g_gameflow.character` muss **0 oder 4** führen (nicht 0/1), sonst bleiben
   `enemy_ai_common.c:4230` und `enemy_ai_re2_zombie.c:1849` tot.
2. Ein Elza-Bit im Flag-Wort (Zwilling von `0x800aca3c` Bit 31) — oder direkt aus
   `character & 4` abgeleitet.
3. Der RDT-Öffner hängt den Spieler-Digit an: `ROOM%X%02X%d.RDT` mit
   `d = (character & 4) ? 1 : 0`. Türziele und Speicherpunkte bleiben
   charakterfrei — sie tragen im Original nur Stage und Raum.
4. `RE15_NEWGAME_ROOM` wird charakterabhängig: Leon 0x17 → ROOM1170,
   Elza 0x03 → ROOM1031, beide Stage 0, mit den Startposen aus §3.
5. `re15_savepoint.c:21` („1071 = Elza-Spiegel des Telefons", als `[PORT-S]`
   markiert) kann entfallen: es gibt keinen separaten Spiegel-Eintrag, sondern
   denselben Raum mit Spieler-Digit 1.

---

## 7. WAS ICH NICHT BELEGEN KONNTE

* Wer im Original Bit 0 von `0x800b0fbe` setzt, damit der Elza-Kartentitel
  @0x800107cc greift. Keiner der fünf Schreiber tut es (§1.5).
* Ob DEBUG.BIN beim Neues-Spiel-Einstieg resident ist und die Startposition aus
  0x800c263c damit die fest verdrahtete überschreibt (§3). Der **Raum** ist davon
  unabhängig.
* Der Zweck der in §4 unten aufgeführten Lesestellen. Adressen stehen da, Deutung
  nicht.
