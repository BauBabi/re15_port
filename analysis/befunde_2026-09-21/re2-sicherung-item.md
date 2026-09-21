# RE2: Sicherung als Gegenstand? Feuerlöscher-Modell? — Befund mit Belegen

**Datum:** 2026-09-21 · **Quellen:** `info/re2leon/` (RE2-Retail Leon, SLUS-00748) und
`info/Re1.5/` — beide nur gelesen · **Werkzeuge:** `tools/re2_sicherung/`

| Frage | Antwort |
|---|---|
| Hat RE2 eine **Sicherung** als Gegenstand? | **JA, zwei:** Id **76 (0x4C) `Main Fuse`** und Id **77 (0x4D) `Fuse Case`** |
| Für **Leon** erreichbar? | **Ja, beide** — `PL0/RDT/ROOM60D0.RDT` (PL0 = Leon), der einzige Raum des Spiels, der sie führt |
| Hat RE2 ein **Feuerlöscher-3D-Modell**? | **NEIN** — und auch keinen Feuerlöscher-Gegenstand |
| Hat **RE1.5** einen Feuerlöscher? | Gegenstand **ja** (Id 0x31, ROOM1000), **Modell nein** (Prop-Slot 255 = keins) |
| Hat **RE1.5** eine Sicherung? | Name **ja** (Id 0x40 `Fuse`), aber **kein Raum stellt sie auf** (0 von 162 Platzierungen) |

Extrakt: **`extracted_re2_sicherung/`**

---

## 0. Die Sollzahlen — wogegen „es gibt keinen" geprüft ist

| Menge | Sollzahl | Wie abgezählt |
|---|---|---|
| RE2-Item-**Namen** | **140** (Index 0x00..0x8B) | u16-Offsettabelle @`0x8009EBAC`; Index 139 hat den letzten monoton steigenden Offset `0x0651`, ab 140 springt er nach `0x00FC`/`0x0000` und danach ins Unsinnige. Die japanische Tabelle @`0x8009E438` bricht an derselben Stelle. Alle 140 dekodiert: `tabellen/re2_item_names_140.tsv` |
| RE2-Item-**Eigenschaften** | **101** (0x00..0x64) | Records ab `0x800A9E1C`, Schrittweite 8. Die Obergrenze ist belegt, nicht geschätzt: `0x800AA144`/`0x800AA146` werden von **fünf** `lhu`-Instruktionen als eigene Tabelle gelesen (@0x8006D0B0, @0x80072520, @0x800727C8, @0x8007603C, @0x80076224). `0x800AA144 − 0x800A9E1C = 0x328 = 101 × 8` |
| RE2-Inventar-**Symbole** | **106** | `COMMON/DATA/ITEMALL.PIX` = 127200 B ÷ 1200 B je Kachel. Alle 106 gerendert und **angesehen**: `tabellen/re2_alle_106_symbole.png` |
| RE2-**Requisiten-Modelle** | **252** verschiedene in **893** Platzierungen | Modelltabelle `RDT+0x30` × `nOmodel` (`RDT+0x02`) über **alle 250** RDTs, nach MD5 des MD1 entdoppelt. Alle 252 gerendert und **angesehen** |
| RE2-**Dateien** gesamt | **21602** | Zeichensatz-Bytesuche über den kompletten Baum |
| RE1.5-Item-**Namen** | **100** (0x01..0x64) | `DEBUG.BIN` ab 0x4A28, Einträge `0x07 <Text>`, bis `0x4F86` (leer) / `0x4F88` (0xFF-Füllung) |
| RE1.5-**Requisiten-Modelle** | **122** verschiedene in **570** Platzierungen | dieselbe Tabelle über 240 RE1.5-RDTs (34 davon sind 4-Byte-Stummel) |
| RE1.5-**Item-Platzierungen** | **162**, 27 verschiedene Ids | opcode-exakter Walk über 1955 SCD-Blöcke |

---

## 1. Wie die Item-Tabellen gefunden wurden

### 1.1 Vom Namens-Zeichner zur Tabelle

`FUN_800693D0` zeichnet den Item-Namen, `a3` = Item-Id:

```
800693EC: sll   v0,s4,3          ; id * 8
800693FC: lui   at,0x800B
80069400: addu  at,at,v0
80069404: lbu   v0,-25059(at)    ; 0x800B0000-25059 = 0x800A9E1D + id*8
8006940C: andi  s1,v0,0xF        ; Varianten-Nibble
8006943C: addiu a3,s1,129        ; Variante -> Namens-Index 129+Nibble
80069450: jal   0x80031070       ; Text-Renderer
```

`FUN_80030B9C` löst den Zeiger auf:

```
80030BD0: lbu   s0,-25059(at)    ; 0x800A9E1D + id*8  = Varianten-Nibble
80030C24: lhu   v1,-5204(at)     ; 0x8009EBAC + id*2  = Offsettabelle EN
80030C2C: addiu v0,v0,-6832      ; 0x8009E550         = Textbasis EN
80030C00: lhu   v1,-4946(at)     ; 0x8009ECAE = dieselbe Tabelle ab Index 129
80030C74: lhu   v1,-7112(at)     ; 0x8009E438         = Offsettabelle JP
80030C7C: addiu v0,v0,-8388      ; 0x8009DF3C         = Textbasis JP
```

`0x80031130: addiu v0,zero,247` ⇒ **0xF7 = Stringende**.

### 1.2 Die Record-Lage — erst falsch, dann gemessen

Mein erster Ansatz legte den Record auf `0x800A9E18` und las `+0` als Zeiger. Das ist
**falsch**: die Kombinations-Prüfung @`0x800695F8` liest denselben Record anders:

```
80069600: lbu a0,-25057(at)      ; 0x800A9E1F + id*8 = Anzahl der Rezepte
80069618: lw  v1,-25056(at)      ; 0x800A9E20 + id*8 = Zeiger auf die Rezeptliste
80069620: lbu v0,0(v1)           ; Rezept +0 = Partner-Item
80069638: addiu v1,v1,4          ; Rezept-Schrittweite 4
80069644: lbu v0,2(v1)           ; Rezept +2 = Ergebnis-Item
```

Ein `lw` muss 4-byte-ausgerichtet sein. Damit liegt die Basis zwingend auf
**`0x800A9E1C`**, Schrittweite 8:

```
+0 u8  max      Stapel-/Magazingrenze
+1 u8  variant  Varianten-Nibble (Namenstausch nach dem Untersuchen)
+2 u8  class    Klassenbyte
+3 u8  n_mix    Anzahl Kombinations-Rezepte
+4 u32 mix_ptr  Zeiger auf die Rezeptliste
```

**Gegenprobe, die die Lage beweist** — mit dieser Basis fällt die komplette
Kräutermischung von RE2 korrekt heraus, ohne dass ich etwas dafür getan hätte:

| Item | max | Rezepte |
|---|---|---|
| 2 Hand Gun | **18** | +20⇒2, +32⇒4 |
| 5 Magnum | **8** | +22⇒5, +33⇒6 |
| 7 Shotgun | **5** | +21⇒7, +34⇒8 |
| 8 C. Shotgun | **7** | +21⇒7 |
| 38 Green Herb | 1 | +39⇒42, +38⇒41, +40⇒43, +41⇒44, +43⇒45 |
| 39 Red Herb | 1 | +38⇒42, +43⇒46 |
| 40 Blue Herb | 1 | +38⇒43, +42⇒46, +41⇒45 |

Magazingrößen und Mischgraph sind die aus dem Spiel bekannten. Mit der falschen
Basis kam an diesen Stellen Müll heraus.

### 1.3 Der Zeichensatz — ebenfalls erst falsch

Buchstaben: `0x1D..0x36` = `A..Z`, `0x3D..0x56` = `a..z`, `0x00` = Leerzeichen
(verifiziert an `Knife`, `Hand Gun`, `Magnum`, `Shotgun`).

Ziffern hatte ich zuerst auf `0x13..0x1C` gelegt. **Falsch.** Item 37 heißt
`Chemical AC-W24` und liest sich dort `… 3B 33 0E 10` — `0x0E`/`0x10` müssen `2`/`4`
sein, also **`'0' = 0x0C`**. Gegenprobe: Namen 108/113 werden damit zu
`Operation report 1` / `Operation report 2` statt `report {0D}` / `{0E}`.
`0x3B = '-'` (`AC-W24`, `G-virus`, `P-epsilon report`),
`0x3A = '''` (`CHRIS's diary`), `0x01 = '.'` (`C. Hand Gun`, `G. Launcher`).

---

## 2. Der Fund

### 2.1 Die beiden Namen

```
Index 76 (0x4C) @0x8009E896   29 3d 45 4a 00 22 51 4f 41   "Main Fuse"
Index 77 (0x4D) @0x8009E8A0   22 51 4f 41 00 1f 3d 4f 41   "Fuse Case"
```

Japanisch: `0x8009E1CE` (`c8 a8 d4 c1 ee 0a 3b dc`) und `0x8009E1D7`
(`c1 ee 0a 3b dc af 3b b3`).

Es sind die **einzigen zwei** von 140 Namen mit „Fuse". Eine Bytesuche nach der
kodierten Folge über alle 21602 Dateien findet `Fuse`/`fuse` nur in der EXE
(genau diese beiden Namen) und in den Raumtexten von ROOM6080, ROOM60D0 und ROOM6110.

### 2.2 Die Eigenschafts-Records

```
Id 76 @0x800AA07C   01 00 04 00 e8 9c 0a 80
Id 77 @0x800AA084   01 00 04 00 e8 9c 0a 80
```

Beide: **max = 1** (ein Feld, nicht stapelbar), Varianten-Nibble 0 (kein Namenstausch),
Klassenbyte 0x04 (wie alle normalen Schlüsselgegenstände), **0 Rezepte**
(Zeiger `0x800A9CE8` = die leere Liste). Eine Gegenprobe über **alle 80** Rezepte des
Spiels findet Id 76/77 weder als Partner noch als Ergebnis — die beiden lassen sich
in RE2 mit nichts kombinieren.

### 2.3 Die Symbole

`COMMON/DATA/ITEMALL.PIX`, Kachel-Index **= Item-Id**, je 1200 Byte:
Id 76 ab Datei-`0x16440`, Id 77 ab `0x168F0`.

Format **40 × 30, 8 bpp**, eine 256-Farben-CLUT bei VRAM y=496. Das ist gemessen,
nicht übernommen: der Zeichner setzt das Zielrechteck selbst —
`80069C74: addiu v0,zero,20 / 80069C78: sh v0,20(sp)` (Breite 20 VRAM-Worte) und
`80069C7C: addiu v0,zero,30 / 80069C84: sh v0,22(sp)` (Höhe 30). 20 Worte sind bei
8 bpp 40 Pixel, und 40×30 = 1200 = genau die Schrittweite des Symbol-Caches
(`&DAT_8019C000 + id*0x4B0`). ⛔ Der TIM-Kopf, den ein fremder Extraktor um die
Rohkacheln gelegt hat (`COMMON/DATA/ITEMALL/ITEMALL_*.TIM`), behauptet 4 bpp — das
ist mit `w=20, h=30` und 1200 Byte unvereinbar, und mit 4 bpp entstehen sichtbare
Streifen im Abstand von einem Pixel. **Der Kopf ist falsch, die PIX ist die Quelle.**

Die Zuordnung Kachel↔Id ist zusätzlich visuell abgenommen: im Kontaktbogen aller 106
Kacheln sitzt an 1 das Messer, an 2/3/4 die Pistolen, an 7 die Schrotflinte, an 12 die
Armbrust, an 30 das Farbband, an 38–46 die Kräuter, an 47 das Feuerzeug, an 96 die
MO-Disk — jedes Bild passt zu seinem Namen.

### 2.4 Wo Leon sie bekommt — ROOM60D0, und sonst nirgends

⛔ **Das vorhandene Decompilat taugt hier nicht.**
`info/re2leon/PL0/RDT/room60D0/scd/sub02.c` enthält wörtlich
`Item_aot_set_4p(/* WIP */);` und gibt für den Satz davor `sat=0, x=125, z=125,
w=256, d=256` aus — unplausibel. Ein Zensus über die `.c`-Dateien findet deshalb nur
den `Main Fuse` und **verliert den `Fuse Case` ganz**. Ich habe daher die rohen
`.scd`-Blöcke opcode-exakt gelaufen.

**Opcode-Längen**: 126 der 143 Handler ab `0x800A74C8` geben ihren PC-Vorschub selbst
her (`addiu rX,rPC,N` / `sw rX,28(a0)`, `tools/re2_sicherung/re2_scd_lens.py`). Die
restlichen 11 sind einzeln am Handler disassembliert bzw. aus der Referenz ergänzt
und im Override-Block von `re2_scd_walk.py` mit Begründung notiert, z.B.
`0x07 Else_ck` (@0x80053964 `lhu v0,2(v1)` ⇒ u16-Blocklänge bei +2, Record 4 B),
`0x17 Goto` (@0x8005415C `lh t0,4(a1)` ⇒ 6 B), `0x18 Gosub` (@0x800541A8
`addiu v1,v1,2` ⇒ 2 B).

**Feldlage von `Item_aot_set` (0x4E, 22 B)** aus `FUN_80054CD4`:

```
80054CF4: lhu   a1,18(s0)     ; +18 = flag  (geht als Bit in den Flag-Test FUN_80077360)
80054CF8: lbu   s2,20(s0)     ; +20 = md1   (Prop-Slot, Schranke < 0x20)
80054CFC: addiu v0,s0,22      ; Recordlänge 22
80054DD4: lbu   s1,21(s0)     ; +21 = action
```
⇒ `+0 op, +1 aot, +2 sce, +3 sat, +4 nFloor, +5 super, +6 x, +8 z, +10 w, +12 d,
+14 i_item, +16 n_item, +18 flag, +20 md1, +21 action`.

**`Item_aot_set_4p` (0x69, 30 B)** aus `FUN_80055008`: `lhu a1,26(s0)` (flag),
`lbu s2,28(s0)` (md1), `addiu v0,s0,30`. Die vier Eckpunkte (16 B) ersetzen x/z/w/d
(8 B), alles danach schiebt sich um 8 ⇒ `+22 i_item, +24 n_item`. Der Handler setzt
außerdem selbst `80055034: ori v0,v0,0x80 / sb v0,3(s0)` — **sat |= 0x80** markiert
den 4-Punkt-Typ.

**Die beiden Records**, `PL0/RDT/room60D0/scd/sub02.scd` (Walk: `ok`, 19 Records,
kein Desync), identisch auch in `ROOM60D0.RDT` bei `0x0008E6` / `0x0008FC`:

```
+0x0000  4E 01 02 00 00 00 00 7D 00 7D 01 00 01 00 4C 00 01 00 18 00 03 00
         aot=1 sce=2 sat=0  x=32000 z=32000 w=1 d=1   item=76 n=1 flag=24 md1=3 act=0
+0x0016  69 02 02 31 00 00 41 B9 CE C8 A1 BD 7A CC BF C2 D0 C6 47 BD FC C2
         4D 00 01 00 3D 00 01 00
         aot=2 sce=2 sat=49 4 Eckpunkte           item=77 n=1 flag=61 md1=1 act=0
```

* **`Fuse Case` (77)** ist eine ganz gewöhnliche Bodenaufnahme: `sat=49` wie 256 von
  269 Item-AOTs des Spiels, vier echte Weltkoordinaten. Hinlaufen, Aktion drücken.
* **`Main Fuse` (76)** steht auf dem **Parkplatz**: `x=z=32000`, Box 1×1, `sat=0`.
  Das ist ein wiederkehrendes Autoren-Muster (auch `H. Gun Bullets` in room1090 sub03
  und `Joint S/N Plug` in room7010 sub02 liegen so). Er wird **per Skript übergeben**:

  `room60D0/scd/sub05.scd`
  ```
  +0x0012  06 00 08 00     Ifel_ck
  +0x0016  21 22 18 00     Ck(0x22, 0x18, 0)      ; 0x18 = 24 = genau das flag des Records
  +0x001A  47 01           Aot_on(aot 1)
  +0x001C  08 00           Endif
  ```

  `Aot_on` (`FUN_800551C8`) ruft den sce-Handler **sofort** auf:
  ```
  800551E0: lbu v0,1(v0)         ; Pool-Record +1 = sat
  80055208: andi v0,v0,0x80
  8005520C: bne  -> a0 = rec+20  ; 4-Punkt-Form
  80055214:        a0 = rec+12   ; Kurzform   (= Record+14 = i_item)
  8005521C: lbu v0,0(v1)         ; sce
  8005522C: lw  v0,29636(at)     ; Tabelle 0x800A73C4
  80055234: jalr v0
  ```
  und `sce = 2` ist der **Aufnahme-Handler** `0x80051884`: er legt die Id nach
  `0x800E873F`/`0x800D4231` und schaltet mit `800518F8: sb v1,23552(at)`
  (`DAT_800D5C00 = 2`) auf den **Item-Get-Bildschirm**.

  Kurz: Leon bekommt den `Main Fuse` nicht vom Boden, sondern über ein Skript-Ereignis
  in ROOM60D0 — gegated durch dasselbe Flag 24, das den Record als „schon genommen"
  markiert.

**Dass es sonst nirgends vorkommt**, ist doppelt geprüft:
1. Der opcode-exakte Walk über **alle 2553** SCD-Blöcke findet 269 Item-AOTs mit
   79 verschiedenen Ids; 76 und 77 je genau einmal, beide in room60D0/sub02.
   45 Blöcke (1,8 %) laufen aus dem Tritt, dort könnte der Walk etwas übersehen.
2. Deshalb zusätzlich eine **Gegenprobe an jeder Byteposition** aller 250 RDT-Container
   und aller 2553 SCD-Blöcke (`anchor_scan.py`), die die Opcode-Struktur ignoriert und
   deshalb nichts übersehen *kann*: **47 Rohtreffer**, davon **4 plausibel** — und diese
   vier sind **dieselben zwei Records**, einmal im Container `ROOM60D0.RDT`
   (`+0x0008E6`, `+0x0008FC`) und einmal im daraus geschnittenen Block `sub02.scd`
   (`+0x0000`, `+0x0016`). Die übrigen 43 sind Textblöcke (`4e 00 4e 00 41 00 …`) mit
   `n_item=78`, `md1=75` und ähnlichem Unsinn.

### 2.5 Die Weltmodelle

Die Bodenobjekte sind **Raum-Requisiten**, keine globalen Item-Modelle: das Feld
`md1` des Item-AOT indiziert die `Obj_model_set`-Slots desselben Raums. Die
Modelltabelle steht bei `RDT+0x30`, `2 × nOmodel` u32-Offsets, abwechselnd TIM und MD1
(`nOmodel` = `RDT+0x02`; im Port als `rdt_common.c:51-79`).

ROOM60D0 hat `nOmodel = 8`, Tabelle @`0x124`:

| Slot | MD1 | Größe | TIM | Wer benutzt ihn |
|---|---|---|---|---|
| 1 | `0x000ED8` | 1356 B | `0x01F384`, 8bpp 128×192 | **`Fuse Case` (Item 77, md1=1)** |
| 3 | `0x002280` | 1988 B | `0x01F384` | **`Main Fuse` (Item 76, md1=3)** |
| 7 | `0x008C10` | 2180 B | `0x02DBC4`, 8bpp 128×64 | `F. Aid Spray` (Item 35, md1=7) |

**Slot 7 ist die unabhängige Kontrolle:** der Item-AOT bei `+0x01A4` trägt `item=35`
(`F. Aid Spray`) und `md1=7`, und Slot 7 rendert unverkennbar eine Erste-Hilfe-Sprühdose
mit grünem Kreuz. Damit ist bewiesen, dass `md1` den Prop-Slot indiziert — nicht bloß
behauptet.

Gerendert (MD1-Kopf `length/unknown/object_count`, Mesh-Köpfe à 56 B nach
`re15_port/include/re15_md1.h`) zeigt **Slot 3 eine Patronensicherung**: zylindrischer
Körper, an beiden Enden ein geriffelter Gewindeansatz — dieselbe Form wie das
Inventarsymbol. **Slot 1** ist die passende **hohle Fassung**, ebenfalls deckungsgleich
mit ihrem Symbol. Beides liegt als `.md1`, `.tim`, `.obj` und Vorschau-PNG im Extrakt.

---

## 3. Feuerlöscher

### 3.1 RE2 — nichts, in keiner der vier Mengen

| Geprüft | Umfang | Treffer |
|---|---|---|
| Item-Namen | alle **140** | 0 |
| Inventar-Symbole | alle **106**, einzeln angesehen | 0 |
| Requisiten-Modelle | alle **252** verschiedenen, einzeln angesehen | 0 |
| Zeichensatz-Bytesuche `extinguisher` / `extinguish` | alle **21602** Dateien | **0** |

Die Bytesuche ist als Methode abgesichert: dieselbe Suche nach `fuse` liefert 24
Treffer, darunter die beiden Namen in der EXE und die Raumtexte von ROOM6080,
ROOM60D0 und ROOM6110 — sie findet also, was da ist.

Die Suche nach `fire` (14 Treffer) landet nur in Hintergrundbildern und in den
Raumtexten von ROOM2060 und ROOM20D0; nichts davon ist ein Gegenstand.

**Nächste Verwandte in RE2**, falls eine Basis gebraucht wird
(Symbole liegen einzeln in `tabellen/alle_106_symbole/`):

| Id | Name | Form |
|---|---|---|
| 35 | `F. Aid Spray` | Sprühdose mit grünem Kreuz — **hat als einziges ein 3D-Modell** (ROOM60D0 Slot 7) |
| 23 | `Fuel` | roter Kanister mit Stutzen |
| 36 | `Antivirus bomb` | Behälterkiste |

### 3.2 RE1.5 — Gegenstand ja, Modell nein

`info/Re1.5/PSX/BIN/DEBUG.BIN` trägt ab `0x4A28` die Namensliste als Folge
`0x07 <Text>`. **Id = Platz + 1**; das ist an zwei im Port bereits gemessenen Punkten
verankert (`re15_port/engine/src/scd_vm.c:3829-3831` zitiert
„ROOM1050.RDT @0xb9a = handgun ammo (0x15)" und „ROOM1000 @0xc24 = 0x31 x1";
`test_item_name_census.c:42` definiert `ITEM_FIRE_EXTINGUISHER 0x31`):

```
Id 0x15 @DEBUG.BIN+0x4B28   "H. Gun Bullets"
Id 0x31 @DEBUG.BIN+0x4CDC   "Fire Extinguisher"
Id 0x40 @DEBUG.BIN+0x4D8E   "Fuse"
Id 0x41 @DEBUG.BIN+0x4D93   "Spark Plug"
```

Opcode-exakter Zensus über alle 240 RE1.5-RDTs (1955 SCD-Blöcke, Längentabelle des
Ports, 28 Blöcke = 1,4 % abgebrochen): **162 Item-Platzierungen, 27 verschiedene Ids.**

* `Fire Extinguisher` (0x31): **genau eine** Platzierung —
  `ROOM1000 sub00 +0x0C24`:
  ```
  50 03 09 31 00 00 14 50 E0 FC 20 03 20 03 31 00 01 00 86 00 FF 00
  aot=3 sce=9 sat=0x31 x=20500 z=-800 w=800 d=800 item=0x31 n=1 tk=134 prop=0xFF
  ```
  **`prop = 0xFF` = kein Requisiten-Modell.** Das Objekt am Boden ist reines
  Hintergrundbild.
* `Fuse` (0x40): **null** Platzierungen. Der Name existiert, der Gegenstand wird nie
  ausgelegt.
* Unter den **122** verschiedenen RE1.5-Requisiten-Modellen (570 Platzierungen,
  alle angesehen) ist ebenfalls kein Feuerlöscher.

---

## 4. Was der Port davon benutzen kann

**Für eine Sicherung** ist alles da: Symbol (40×30, 8 bpp + CLUT), beide Namen als
UTF-8 und als Originalbytes, Eigenschaftsrecord, und — der eigentliche Gewinn —
**zwei fertige 3D-Modelle** (Sicherung + Fassung) mit gemeinsamer Textur. Der Port
liest Requisiten-Modelle bereits aus dem RDT (`pc_load_room_prop_set`), das Format ist
identisch. Nötig wäre:

1. Eine RE1.5-Item-Id vergeben. **Id 0x40 `Fuse` liegt frei** — Name existiert schon
   in `DEBUG.BIN`, kein Raum benutzt sie. Das ist die saubere Variante, weil dann
   nichts erfunden wird.
2. MD1 + TIM aus `item_076_main_fuse/` als Requisite in das Ziel-RDT einspeisen (oder
   portseitig laden) und mit einem `Item_aot_set` daneben verdrahten.
3. Das Symbol in die Symbolbank hängen. ⛔ Hier klemmt es portseitig:
   `re15_port/engine/src/item_icon_common.c` hat nur die Paletten `clut_idx` 2 und 6
   für Ids `0x00..0x18`. Für alles darüber gibt es noch keine gemessene Palette.

**Für einen Feuerlöscher** gibt es nichts zu übernehmen — weder aus RE2 noch aus
RE1.5. Der Gegenstand `Fire Extinguisher` (RE1.5 Id 0x31) existiert samt Text und
Symbolplatz, aber ein 3D-Modell hat ihn nie begleitet; im Original sieht man ihn nur
als Teil des Hintergrundbildes von ROOM1000.

---

## 5. Offen

* Das **RE1.5-Symbol** zu Id 0x31 liegt vermutlich auf Kachel 49 von
  `info/Re1.5/PSX/DATA/ITEMALL.PIX` (72 Kacheln à 40×30), und die Form dort passt
  (länglicher Körper mit abgewinkeltem Ansatz). ⛔ **Bewiesen ist das nicht**: der Port
  belegt `tile == id` nur für Ids < 0x15. Für den echten Beleg fehlt die
  Upload-Reihenfolge aus dem Framebuffer, so wie sie für 0x15→37 schon gemessen wurde.
* Die **CLUT der RE1.5-Symbole** ab Id 0x19 ist nicht gemessen
  (`item_icon_common.c:48`).
* **45 RE2-** und **28 RE1.5-SCD-Blöcke** laufen im Walk aus dem Tritt. Für die
  Sicherungs-Frage ist das durch die Byteposition-Gegenprobe abgedeckt; für andere
  Fragen wären die fehlenden Opcode-Längen noch nachzutragen.
* Der `Fuse Case` (77) hat in RE2 **kein** Kombinations-Rezept mit dem `Main Fuse` (76)
  — geprüft über alle 80 Rezepte. Wie die beiden im Spiel zusammenwirken, steckt also
  im Raumskript, nicht in der Item-Tabelle; das habe ich nicht weiterverfolgt.

---

## 6. Werkzeuge

Alle unter `tools/re2_sicherung/`, alle nur lesend auf `info/`:

| Datei | Zweck |
|---|---|
| `re2_items.py` | RE2-Namens- und Eigenschaftstabellen |
| `re2_scd_lens.py` | Opcode-Längen aus den 143 Handlern ziehen |
| `re2_scd_walk.py` | opcode-exakter Walk + Item-Zensus |
| `anchor_scan.py` | Gegenprobe an jeder Byteposition |
| `charset_grep.py` | Wort im Spiel-Zeichensatz kodiert bytesuchen |
| `msg_decode.py` | Raumtexte lesen |
| `itemall_extract.py`, `clut_probe.py` | Inventarsymbole |
| `rdt_props.py`, `dump_props.py`, `prop_census.py` | Requisiten-Modelle |
| `md1_view.py`, `multi_view.py` | MD1 parsen, als OBJ schreiben, texturiert rendern |
| `re15_items.py`, `re15_item_census.py`, `re15_icon.py` | die RE1.5-Seite |
| `xref_scan.py` | Zugriffe auf einen Adressbereich in der EXE finden |
| `export.py` | baut `extracted_re2_sicherung/` |
