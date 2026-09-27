# Runde 30 / Welle 2: die Trefferboxen +0x98 / +0x9E aller RE2-Typen — und der dritte Fahrstuhl

Nutzer-Auftrag von heute, wörtlich:

> „Ja, na klar, fahrstuhl sound muss überall bei fahrstühlen rein. Der fehlt weil REsident
> Evil 1.5 eine 40% Beta ist und unvollständig. Und die Trefferboxen wenn die fehlen müssen
> natürlich auch ermttelt und übernommen werden. Das soll alles sauber Resident Evil 2
> entsprechen."

Beides ist gebaut. Die Zahlen unten sind gemessen, nicht modelliert; jede Konstante trägt
ihre Adresse.

---

## 1. Der Vollscan, selbst gefahren

`sh rt,152(rs)` / `sh rt,158(rs)` über die fünf Gegner-Overlays in
`info/re2leon/COMMON/BIN/` (alle laden roh @`0x80100000`) — **48 Stores**:

| Datei | Stores |
|---|---|
| `EMZ0.BIN` (Zombie 0x10) | 26 |
| `EMD0G_MOD0.BIN` (Hund 0x20) | 10 |
| `EMOVL21_S0.BIN` (Krähe 0x21) | 6 |
| `EMS25.BIN` (Spinne 0x25) | 4 |
| `EMS26.BIN` (Baby 0x26) | 2 |

Zwölf davon gehen über `lw <reg>,408(base)` (= +0x198, Teile-Pool, Stride 172 = 0xAC) auf ein
**Modell-Teil**, wo +0x98 ein WINKEL ist — die sind ausdrücklich **nicht** auf `re2_hit_box_set`
gebogen worden. Bleiben 36 SELF-Stores = 18 Paare.

⛔ **Der `sh`-Scan allein ist unvollständig.** Die Spinne setzt ihre Box als **Blockkopie mit
`sw`** (acht Wörter nach +0x84..+0xA0). Ein zweiter Scan über `sw rt,132..160(rs)` findet in
`EMS25.BIN` **neun** solche Kopien aus **vier** Tabellen (Bytes selbst gelesen):

| Tabelle | XZ (+0x90/+0x92) | +0x98 | +0x9A | +0x9C | +0x9E |
|---|---|---|---|---|---|
| @`0x801063A0` | 800 / 800 | −1400 | 1000 | 1000 | 1400 |
| @`0x801063C0` | 30 / 30 | −100 | 1000 | 1000 | 1400 |
| @`0x801063E0` | 30 / 30 | 0 | 1000 | 1000 | 1400 |
| @`0x80106400` | 5 / 5 | −1 | 5 | 5 | 1 |

Kopierstellen: @`0x801003C0`(A) @`0x801004EC`(C) @`0x80101AD4`(B) @`0x80101D44`(C)
@`0x80102594`(B) @`0x80102718`(A) @`0x80103690`(A) @`0x80104378`(A) und @`0x80105E88`(D) —
letztere schreibt in das **frisch gespawnte Baby**, nicht in `self`.

Damit ist die Welle-1-Zeile „0x25 Spinne 0/1400" **berichtigt**: der Boden-Spawn trägt
**−1400/1400**, nicht 0/1400. 0/1400 ist der Decken- bzw. Wandwert.

---

## 2. Was jetzt im Port steht

### 0x10 Zombie (`enemy_ai_re2_zombie.c`, neuer Helfer `re2z_hitbox`)

| # | Adresse | Wert | Ort im Port |
|---|---|---|---|
| 1 | @0x8010095C/64 | −1500/1500 | `re2z_init`, INIT-Joinpunkt |
| 2 | @0x80100B18/20 | −350/350 | `re15_re2z_enter_crawler` |
| 3 | @0x80100BCC/D4 | −350/350 | **OPEN** — Spawn-Zweig 0x0F01 hat keinen Port-Zwilling |
| 4 | @0x80103464/6C | −350/350 | EXEC[5] P2 (Aufschlag) |
| 5 | @0x8010366C-94 | **Rampe** | EXEC[5] P7 |
| 6 | @0x80103714/20 | −1500/1500 | EXEC[5] P8 |
| 7 | @0x80104A20/28 | −1500/1500 | Reset |
| 8 | @0x80106B2C/34 | −350/350 | Kriecher-Eintritt aus der Treffer-Reaktion |
| 9 | @0x80107810/18 | −350/350 | Knockdown-P2 |
| 10 | @0x80107E90/98 | −1500/1500 | Aufsteher-EXIT |
| 11 | @0x8010899C/A4 | −350/350 | **OPEN** — Todes-Wiederbelebung nicht gebaut |

Die **Rampe** ist die Gegenstelle, die die Kriecher-Box aus der Sackgasse holt — selbst
disassembliert:

```
8010366c: lh    v0,152(s2)
80103678: slti  v0,v0,-1499
8010367c: bne   v0,zero,0x80103698     ; b < -1499 -> fertig
80103688: addiu v1,v1,-10
8010368c: sh    v1,152(s2)             ; b -= 10
80103690: addiu v0,v0,10
80103694: sh    v0,158(s2)             ; h += 10
```

### 0x21 Krähe (`enemy_ai_re2_crow.c`)

* INIT −350/530 @`0x801003C4`/`DC`
* **Neu-Berechner jedes Bild**, vor dem Zustands-Verteiler:
  `lw v0,496(s0)` @`0x801001EC` / `sltiu v0,v0,0x384` @`0x801001F4` →
  `+0x98 = -350` @`0x80100204` wenn +0x1F0 < 900, sonst `+0x98 = 0` @`0x80100208`.
* ⛔ **Berichtigung:** der Port schrieb +0x98 bisher in `target_z`. Das ist der
  **Plc_neck-Look-at-Kanal** (SCD-Id 0x22), nicht die Trefferbox — der Store hat also einen
  fremden Kanal überschrieben. Der Test `test_re2_crow_ai` prüft jetzt `re2_hit_b98`/`re2_hit_h9e`.

### 0x25 Spinne (`enemy_ai_re2_spider.c`)

INIT −1400/1400 (Tabelle A, @`0x801003BC-400`); Deckenzweig `sh zero,152(s2)` @`0x8010049C`
→ 0/1400; Wandzweig Tabelle C @`0x801004EC-52C` → 0/1400; Deckensprung @`0x80102718-6C`
→ −1400/1400 gefolgt von `sh zero,152(s0)` @`0x8010276C` → 0/1400.
Fünf weitere Blockkopien sind als Fehlstelle mit Adresse benannt.

### 0x26 Baby (`enemy_ai_re2_spider.c`, `re2sb_init`)

−10/+10 @`0x80100168-78` — der einzige Schreiber im ganzen Overlay (2 von 2 Treffern).
Alle sechs Maße sind 10 = ein Würfel von 20 Einheiten Kantenlänge.

---

## 3. Das fünfte Tor — je Typ entschieden, nicht pauschal

Das Tor @`0x8004716C-A4` lautet
`DURCH ⟺ (uint32)(eY + b + 100 + h − MündungY) < (uint32)(2*(h+100))`.
Die Mündungshöhe liefert `re15_player_muzzle_world` (Bone 11 der Kette @`0x80042E60-94`).

Gemessen mit `probe_r31_boxen` auf dem echten Weg (`re15_game_step` + Pad, echte RDTs,
RE2-Bank `shared_assets/RE2/CDEMD0.EMS`), **900 Bilder Dauerbeschuss je Typ, drei Läufe**:

| Typ | TOR AUS | TOR SCHARF | KONTROLLE (Box 0/0) | Urteil |
|---|---|---|---|---|
| 0x10 Zombie | 45 Treffer | **45** | 0 | **SCHARF** |
| 0x20 Hund | 41 | **12** | 0 | SCHARF (schon Welle 1; die Absenkung IST der Befund) |
| 0x25 Spinne | 82 | **82** | 0 | **SCHARF** |
| 0x21 Krähe | 41 | **0** | 0 | unscharf — wäre dauerhaft untreffbar |
| 0x26 Baby | 41 | **0** | 0 | unscharf — wäre dauerhaft untreffbar |

**Die KONTROLLE ist der Riegel gegen eine lügende Sonde.** Mit künstlicher Box 0/0 (Fenster
[−100,100)) fällt jeder Typ auf 0 Treffer — das Tor wird also wirklich gemessen und die
Gleichheit „AUS == SCHARF" ist kein Artefakt eines toten Pfades.

Mündungshöhe im Port: **Hgun = eY − MündungY = 1665..1671**, unbeweglich, weil der Port die
senkrechte Zielpose (RE2-Bone-Kette 0→9→10→11 @`0x80042E64/74/84/94`) nicht führt. Daraus die
Fenster: Zombie stehend [−100, 3100) ✓, Zombie Kriecher [−100, 800) ✗, Hund stehend
[−100, 2100) ✓, Hund liegend [−100, 1100) ✗, Spinne Boden [−100, 2900) ✓, Spinne Decke/Wand
[−1500, 1500) ✗, Krähe [−280, 980) bzw. [−630, 630) ✗, Baby [−100, 120) ✗.

### Warum der Zombie-Kriecher trotzdem gebaut ist

Weil die Gegenstelle **gemessen erreichbar** ist. Ein Lauf über 900 Bilder in ROOM1140:

```
STEHEND vor dem Treffer:  Tor DURCH 298/298
KLEIN ab Bild 298:        125 Bilder klein, Tor DURCH 35/125
WIEDER GROSS bei Bild 423 = 125 Bilder nach dem Niederschlag
                          (61 gezählte Rampen-Schritte @0x8010366C-94)
DANACH:                   Tor DURCH 60/60
```

Und in echten Treffern gerechnet ändert das scharfe Tor beim Zombie **nichts** (45 = 45).

### Die benannte Restgefahr bei 0x25

Die **Decken-/Wandspinne** trägt +0x98 = 0 und fällt rechnerisch aus dem Fenster. Kein
ausgeliefertes RE1.5-Zimmer spawnt eine solche Spinne — gemessen 5 von 5
(ROOM2030/2050/2060/2070/20A0, alle Modus 0, alle −1400/1400, 0 Räume mit Unterkante 0).
Der Versuch, sie im Port durch einen erzwungenen Re-INIT herbeizuführen, war ein **FEHLLAUF**
(die Box blieb −1400/1400) und ist in der Sondenausgabe als solcher protokolliert — er sagt
über die Deckenspinne **nichts**.

---

## 4. Der dritte Fahrstuhl

RE1.5 hat **drei** Fahrstuhlkabinen = sechs RDTs:

| Kabine | Räume | Fahrten je Datei | Gestalt | vorher |
|---|---|---|---|---|
| ELEVATOR (STAGE1) | ROOM1080/1081 | 3 | SIG1 (32 B) | Ton spielte |
| **WAREHOUSE LIFT (STAGE3)** | **ROOM3080/3081** | **2** | **SIG2 (28 B)** | **stumm** |
| A-2 ELEVATOR (STAGE4) | ROOM4020/4021 | 3 | SIG1 | Ton spielte |

Die WAREHOUSE LIFT fällt durch SIG1, weil sie ein anderes Fahrskript hat. Bytes selbst gelesen
(`ROOM3080.RDT` @`0x09FE`):

```
22 01 1c 01   Set   bank1 bit28 = 1     <- Puls 1 = Fahrt   (Se_on id 0x11)
09 0a 3c 00   Sleep 60
22 01 1c 00   Set   bank1 bit28 = 0
22 01 1d 01   Set   bank1 bit29 = 1     <- Puls 2 = Ankunft (Se_on id 0x12)
09 0a 3c 00   Sleep 60
22 01 1d 00   Set   bank1 bit29 = 0
22 01 1c 01   Set   bank1 bit28 = 1
```

Schlafzeiten 60/60 statt 8/90/8/20, zwei Bits statt einem.

**Eindeutigkeit gemessen** (der Generator bricht ab, wenn eine Signatur außerhalb ihrer
Dateien trifft): SIG1 12 Treffer in 4 Dateien, SIG2 4 Treffer in 2 Dateien
(ROOM3080 @0x09FE/@0x0BDA, ROOM3081 @0x07FE/@0x0842), **0 Fehltreffer über alle 240 RDTs**.
Der Anker bleibt datengetrieben — keine Raumnummer im Code.

Der Ton fehlt auch dort im Original: ROOM3080 hat zwar drei `Se_on` (`36 02 0a ..` = Bank 2
Id 0x0A) @`0x0095C`/@`0x00970`/@`0x00984`, aber rund 0xA0 Byte **vor** der Fahrt, in einem
anderen Abschnitt. Wozu sie gehören, ist nicht bestimmt.

Die frühere Notiz „offene Nutzer-Frage ROOM4020" ist gestrichen: der Nutzer will den Ton
ausdrücklich überall.

Nebenbefund zum Anker: bank1 bit0x1C/0x1D ist die Fahrstuhl-**Beleuchtung**, nicht
„Fahrstuhl fährt" — ROOM6030 verschränkt dieselben Pulse mit `Cut_chg` 4/5
(@`0x01214`-`0x01250`) und hat @`0x01004` eine Blink-Schleife. Die Signaturen sind also der
Flacker-Rhythmus einer Fahrt: gemessen eindeutig, aber keine Semantik.

---

## 5. Fehlstellen, benannt statt gebogen

1. **Zombie @0x80100BCC/D4 und @0x8010899C/A4** — kein Port-Zwilling (Spawn-Zweig 0x0F01,
   Todes-Wiederbelebung). Adressen stehen im Quelltext.
2. **Spinne: fünf der neun Blockkopien** (@0x80101AD4, @0x80101D44, @0x80102594, @0x80103690,
   @0x80104378) brauchen je eine eigene Zustands-Zuordnung.
3. **word0-Bits von `FUN_80104088`** (Maske 0xE7FFFFFF @0x80104090-B4 + 0x04000000
   @0x801040AC im a1==0-Zweig, 0x0C000000 @0x801040CC im a1!=0-Zweig) bleiben unumgesetzt —
   der Port führt diese beiden word0-Bits nicht, und im Zensus ist kein Leser aufgetaucht.
   Dieselben 0x0C000000 kommen beim Zombie @0x80103730 und @0x80107EA8 vor.
4. **Der Seiteneffekt des Tores** (`andi v1,v1,0xff00` @0x80047178 + `sh v1,464(s0)`
   @0x80047184 löscht das untere Byte von +0x1D0 bei JEDEM Kandidaten) bleibt unumgesetzt —
   der Port führt +0x1D0 nicht.
5. **Die senkrechte Zielpose** (RE2-Bone-Kette 0→9→10→11 @0x80042E64/74/84/94) fehlt weiter.
   Sie ist die Bedingung dafür, dass Krähe und Baby-Spinne je gatet werden können.
6. **Die 0x26-Spur**: dass RE2 Baby-Spinnen über den radialen Pfad @0x800477CC tötet
   (nur XZ, kein Y: `lw 0(s5)/lw 56(s0)` @0x80047764-68, `jal 0x8008D2F4` @0x80047794,
   `sltu v0,v0,a3` @0x800477A4), ist weiterhin eine Spur, keine Feststellung — wer diesen
   Pfad ruft, ist nicht aufgelöst.
