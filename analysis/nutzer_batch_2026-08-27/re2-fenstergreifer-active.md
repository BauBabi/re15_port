# RE2-Zombie ACTIVE — vollstaendige Kartierung der Dispatch-Tabelle @0x8010C854

**Stand:** 2026-08-27 (in Arbeit, inkrementell) · **Ziel:** den ortsgebundenen „Haende durch das
Fenster/Gitter"-Zweig in RE2 finden oder sein Fehlen belegen.
**Binary:** `info/re2leon/COMMON/BIN/EMZ0.BIN`, roh geladen @`0x80100000` (kein 0x800-Header),
Datei-Offset = `addr - 0x80100000`. Groesse 53068 B → letzte Adresse 0x8010CF4B.

> Jede Zahl unten ist mit `@0x…`-Adresse + Instruktion belegt. Wo etwas NICHT gefunden ist,
> steht das ausdruecklich als **NICHT GEFUNDEN** samt der probierten Wege.

---

## 0. Vorbemerkung: drei Dateinamen, EINE Datei

```
$ sha1sum info/re2leon/COMMON/BIN/*.BIN
2d8885fc05e5729d9beb9e8bf8be9c2a3c32a891 *EMOVL10_S0.BIN
2d8885fc05e5729d9beb9e8bf8be9c2a3c32a891 *EMZ0.BIN
2d8885fc05e5729d9beb9e8bf8be9c2a3c32a891 *EMZ0_d1.BIN
```
`EMOVL10_S0.BIN`, `EMZ0.BIN` und `EMZ0_d1.BIN` sind **byte-identisch**. Es gibt also nur EIN
Zombie-Overlay. (`EMOVL10_S1.BIN` ist eine eigene Datei, sha1 `71d0db…`.)

---

## 1. ⛔ KORREKTUR: Es sind NICHT „52 Handler / 38 unerforschte Zweige"

Der Auftrag ging (wie das Vor-Dossier) davon aus, `0x8010C854` sei eine flache Tabelle mit 52
Eintraegen und 38 unerforschten Verhaltensfunktionen. **Das ist ein Layout-Irrtum: drei
verschiedene Tabellen liegen luekenlos hintereinander und ueberlappen im Dump.**

### 1.1 Der ACTIVE-Dispatcher @0x8010114C (Tabelle 1)

```
8010114c: addiu sp,sp,-24
80101150: sw   ra,16(sp)
80101154: lhu  v0,270(a0)      ; +0x10E
8010115c: andi v0,v0,0x3f
80101160: sll  v0,v0,2
80101164: lui  at,0x8011
8010116c: lw   v0,-14252(at)   ; = 0x8010C854 + idx*4
80101174: jalr v0
```
Tabelle 1 = `0x8010C854`, **14 Eintraege** (Index 0..13), alternierend:

| idx | Ziel | Bedeutung |
|----|------|-----------|
| 0,2,4,6,8,10,12 (gerade) | `0x8010118C` | Zombie **aufrecht** |
| 1,3,5,7,9,11,13 (ungerade) | `0x80101210` | Zombie **kriechend** |

Ausser Bit0 unterscheidet `+0x10E & 0x3f` in ACTIVE **nichts** — alle geraden Indizes zeigen auf
dieselbe Funktion, alle ungeraden ebenso (`table 0x8010c854 64 --bin EMZ0.BIN`).

### 1.2 `0x8010118C` (aufrecht) ist selbst ein Sub-Dispatcher — Tabellen 2 und 3

```
801011a8: lbu  v0,5(s0)          ; +0x5 = SUBSTATE
801011b0: sll  v0,v0,2
801011bc: lw   v0,-14196(at)     ; = 0x8010C88C + sub*4   (Tabelle 2 = "Phase A")
801011c4: jalr v0
801011cc: addu a0,s0,zero
801011d0: lbu  v0,5(s0)          ; +0x5 erneut lesen (Phase A darf es geaendert haben)
801011e4: lw   v0,-14132(at)     ; = 0x8010C8CC + sub*4   (Tabelle 3 = "Phase B")
801011ec: jalr v0
```
Also **zwei** Aufrufe pro Frame: erst `TabA[sub]` („entscheiden"), dann — mit dem eventuell
frisch geschriebenen `+0x5` — `TabB[sub]` („ausfuehren"). Beide Tabellen haben 16 Eintraege
(sub 0..15), denn `0x8010C88C + 16*4 = 0x8010C8CC` und `0x8010C8CC + 16*4 = 0x8010C90C`.

### 1.3 `0x80101210` (kriechend) — Tabellen 4 und 5

```
8010122c: lbu  v0,5(s0)
80101240: lw   v0,-14068(at)     ; = 0x8010C90C + sub*4   (Kriech-Phase A)
80101248: jalr v0
80101254: lbu  v0,5(s0)
80101268: lw   v0,-14056(at)     ; = 0x8010C918 + sub*4   (Kriech-Phase B)
80101270: jalr v0
```
`0x8010C918 - 0x8010C90C = 0xC` → Kriech-Tabellen haben je **3** Eintraege (sub 0..2).
Ab `0x8010C924` beginnen Daten (`0x803200be, 0x80400096, …` — keine gueltigen Code-Zeiger).

### 1.4 Damit loest sich die Zahl 52/38 auf

`14 (Tab1) + 16 (Tab2) + 16 (Tab3) + 3 (Tab4) + 3 (Tab5) = 52` Worte — genau die 52, die als
„Handler" gezaehlt wurden. Die 38 „verschiedenen Funktionen" sind: 2 Sub-Dispatcher +
36 Substate-Handler. Von den 36 sind **13 reine `jr ra`-Stubs**:

```
801025e4: jr ra / nop     80103170: jr ra / nop     80103178: jr ra / nop
80103180: jr ra / nop     80103778: jr ra / nop     8010394c: jr ra / nop
80103b6c: jr ra / nop     80103e40: jr ra / nop     80104174: jr ra / nop
80104394: jr ra / nop     80104920: jr ra / nop     80104d6c: jr ra / nop
80104e4c: jr ra / nop
```
Es bleiben **23 echte Verhaltensfunktionen**, nicht 38.

---

## 2. Die Entity-Felder, auf die es ankommt (alle belegt)

| Feld | Bedeutung | Beleg |
|---|---|---|
| `+0x04` | Hauptzustand (Index in `0x8010C830`) | `sw v0,4(s0)` mit 0x…01 überall; Tabelle `0x8010C830` |
| `+0x05` | Substate (Index in Tab 2/3 bzw. 4/5) | `801011a8: lbu v0,5(s0)` |
| `+0x06` | Sub-Substate (jede Substate-Funktion hat eine eigene Sprungtabelle) | z.B. `8010379c: lbu v1,6(s0)` |
| `+0x08` | Gegner-Typ (aus `Sce_em_set` pc[3]) | `8005733c: sb v0,8(s0)` |
| `+0x38/+0x40` | X / Z (s32) | `800572dc: sw v0,56(s0)` / `8005730c: sw v0,64(s0)` |
| `+0x76` | Yaw (0..4095) | `80057330: sh v0,118(s0)` |
| `+0x106` | Etage/Floor | `80057390: sb v0,262(s0)` |
| `+0x10E` | **Spawn-/Verhaltens-Wort** (siehe §3) | `80057354: sh v0,270(s0)` |
| `+0x14C` | Clip-Nr (Wort: [0]=Clip, [1]=Frame, [2]=Flags) | `sw v0,332(s0)` |
| `+0x14D` | aktueller Anim-Frame | `801028a0: lbu v1,333(s1)` |
| `+0x156` | HP (s16); `-1` = unverwundbar | `80100a40: sh v0,342(s2)` mit `v0=-1`; Spieler-HP `0x800CFD4E` |
| `+0x1F0` | **Distanz zum Spieler**, jeden Frame neu | `800265dc: sw v0,496(s0)` nach `jal 0x8008d2f4` (sqrt) über `(x-px)²+(z-pz)²` |
| `+0x239` | Stöhn-Cooldown in Frames | `801038d0/4: addiu v0,zero,150 / sb v0,569(s0)` |

**Der Spieler ist die Entity @`0x800CFBF8`.** Beleg: `80102664-8: lui s3,0x800d / addiu s3,s3,-1032` → `0x800CFBF8`; und
die beiden Globalen, die als „playerX/playerZ" an `atan2` gehen, sind `0x800CFC30 = 0x800CFBF8+0x38`
und `0x800CFC38 = 0x800CFBF8+0x40` — exakt die X/Z-Felder derselben Struktur.
Spieler-HP = `0x800CFD4E` = `player+0x156` (`FUN_800401d4` @`0x80040248-5C`).

**Die EXE-Bibliotheksfunktionen, die im Zombie-Overlay aufgerufen werden** (alle über
`RE2_Quellcode_V2/FUN_*.c` bzw. eigene Disasm identifiziert):

| Adresse | Was sie tut | Beleg |
|---|---|---|
| `0x80015FE8` | `rand()` (LCG auf `DAT_800CE318`) | `RE2_Quellcode_V2/FUN_80015fe8.c` |
| `0x800154AC` | `atan2(x1,z1,x2,z2)` → 0..4095 | Aufrufer `FUN_80015614` |
| `0x80015614` | Dreh-Schritt zum Ziel; **0 = bereits ausgerichtet** (`if (2*step<=delta) …`) | `FUN_80015614.c` |
| `0x80015558` | dreht `+0x76` um max. `a3` zum Ziel | `FUN_80015558.c` |
| `0x80015758` | **Kegel-Test**: liegt B innerhalb ±`a3` um Winkel `a2`? 0 = ja | `0x800157AC-B8`: `andi 0xfff / sltu / xori 1` |
| `0x800152C8` | **FORTBEWEGUNG**: `+0x38/+0x40 += rot(+0x144)` | `FUN_800152c8.c` |
| `0x80015E7C` | **Root-Motion**: Anim-Translation auf die Position anwenden | `FUN_80015e7c.c` |
| `0x80016200` | mit `a1!=0`: `+0x144 = sqrt(dx²+dz²)`; **mit `a1==0`: zieht die Root-Translation wieder ab** (= verankern) | `FUN_80016200.c` |
| `0x8002959C` | Anim-Zeiger setzen / Frame weiterschalten, Rückgabe != 0 = Clip zu Ende | `FUN_8002959c.c` |
| `0x80015B94` | spielt einen Clip auf der **übergebenen** Entity (wird mit dem SPIELER aufgerufen) | Disasm `0x80015B94`, lädt `0x800CE330` |
| `0x80015CB8` | dito für die eigene Entity | Disasm `0x80015CB8` |
| `0x8005BD6C` | **`se_play(id, entity)`** — Sound-Effekt | `FUN_8005bd6c.c` |
| `0x8001BF10` | positionierte SPU-Stimme belegen (Voice 0xA003) | `FUN_8001bf10.c` |
| `0x800401D4` | **`player_damage(dmg, mode)`** — zieht von `0x800CFD4E` ab | `FUN_800401d4.c` |

---

## 3. `+0x10E` — wer setzt es, was bedeutet es

### 3.1 Der EINZIGE Schreiber ausserhalb des Overlays: `Sce_em_set`

```
8005714c:  <Funktionsanfang>  ← Eintrag 0x44 der SCD-Opcode-Tabelle 0x800A74C8
80057170: lw   v0,28(s5)          ; SCD-pc
80057178: sw   v0,23528(at)       ; -> 0x800D5BE8 (Zeiger auf den Opcode-Record)
…
80057344: lw   v1,23528(v1)       ; = 0x800D5BE8
8005734c: lhu  v0,4(v1)           ; ****  pc[4..5]  ****
80057354: sh   v0,270(s0)         ; ****  -> entity+0x10E  ****
…
800576e4: addiu v1,v1,22          ; Record-Laenge = 22 Bytes
800576e8: sw   v1,28(s5)
```
* Die SCD-Opcode-Dispatch-Tabelle liegt @`0x800A74C8` (`80053f84: lw v0,29896(at)` mit `at=0x800A0000`).
  `(0x800A75D8 - 0x800A74C8)/4 = 0x44` → **`Sce_em_set` = Opcode `0x44`, Record-Laenge 22 Byte** (RE1.5: 20).
* Record-Felder (aus derselben Funktion): `pc[3]`→`+0x08` Typ (`8005733c`), `pc[4..5]`→`+0x10E` (`80057354`),
  `pc[6]`→`+0x106` Floor (`80057390`), `pc[10..11]`→X (`800572DC`), `pc[12..13]`→Y (`800572F4`),
  `pc[14..15]`→Z (`8010730C`), `pc[16..17]`→Yaw (`80057330`).

### 3.2 ⛔ Die RE2-STAGE-Overlays fassen `+0x10E` NICHT an

Exhaustiver Scan aller `sh/sb/sw/lhu/lbu/lh` mit Immediate `0x10e`/`0x10f` ueber **alle** Dateien in
`info/re2leon/COMMON/BIN/` und die `PSX.EXE`:

```
STAGE1.BIN … STAGE7.BIN :  0 Treffer
EMZ0.BIN                : 66 Treffer (alle im Zombie-Code selbst)
PSX.EXE                 : u.a. 80057354 sh v0,0x10e(s0)   <- Sce_em_set
```
**Kein Raum-Overlay schreibt `+0x10E`.** Der Wert kommt ausschliesslich aus den 22-Byte-`Sce_em_set`-
Records der RDT-Skripte (bzw. wird vom Zombie-Overlay selbst umgeschrieben, §3.4).

### 3.3 Bit-Bedeutung von `+0x10E`

| Bits | Bedeutung | Beleg |
|---|---|---|
| `0x003F` | ACTIVE-Tabellenindex (nur Bit0 zaehlt: 0=aufrecht, 1=kriechend) **und** INIT-Spawn-Variante | `8010115c: andi v0,v0,0x3f`; `801009c4 ff` |
| `0x2000` | Kriech-/Liegend-Flag | gesetzt `80102db8: ori v0,v0,0x2000`, `80103308`, `801040b8`, `80104468`; geloescht `80103744: andi v0,v0,0xdfff` |
| `0x4000` | **ROOT-LOCK: „bleib in diesem Substate stehen"** | geprueft `80103824` (Sub 7) und `80103c9c` (Sub 8); gesetzt `80101a14`, `80102234`, `80103a4c`; geloescht `80104f0c: andi v1,v1,0xbfff` (HURT) |

Zusaetzlich setzt der Frame-Prolog bei gesetztem ROOT-Bit ein Laufzeit-Flag:
```
8010050c: lhu  v0,270(s0)
80100514: andi v0,v0,0x4000
80100518: bne  v0,zero,0x80100534
80100534: ori  v0,v1,0x2
80100538: sb   v0,448(s0)        ; +0x1C0 |= 2
```

### 3.4 INIT (`0x8010065C`) — die Spawn-Varianten, die `+0x10E & 0x3f` auswaehlt

| `&0x3f` | Startzustand `+0x4/+0x5` | Clip `+0x14C` | Sonstiges | Adresse |
|---|---|---|---|---|
| (Default) | — | 1 | | `801009d4: sw a1,332(s2)` (a1=1) |
| 2 | `01 / 07` | 22 | `+0x21A \|= 4` | `801009e8`, `801009dc` |
| 3 | `01 / 00` | 1 | | `80100b8c: sw a1,4(s2)` |
| 4 | `01 / 07` | 23 | | `80100a10`, `80100a08` |
| **5** | `01 / 07` | 22 | **`+0x156 = -1` (unverwundbar)**, **`+0x10E := 0x4002`**, `+0x21A\|=4`, `(+0x198)[0x560] \|= 0x4000` | `80100a30`, `80100a34-38`, `80100a3c-40` |
| 6 | `01 / 01` | 1 | | `80100abc` |
| **7** | `01 / 07` | 23 | **`+0x156 = -1`**, **`+0x10E := 0x4004`**, `(+0x198)[0x560]\|=0x4000` | `80100a84`, `80100a88-8c`, `80100a90-94` |
| 8 | `01 / 08` | 18 | | `80100adc`, `80100ad4` |
| 1 | `01 / 02` | 23 | | `80100b74` |
| 10 | `01 / 0F` | 0 | `+0x1D3\|=0x80`, `+0x21A=(&~4)\|2`, `+0x0\|=8` | `80100bdc`, `80100c00` |
| **jedes ungerade** | (Hitbox 200/±350) | 23 | `+0x10E \|= 0x2000`, `+0x156 /= 2`, Flags `&0xF3FFFFFF \| 0x04000000` | `80100ae8-80100b5c` |

`0x4002 = 16386` (`80100a34: addiu v0,zero,16386`), `0x4004 = 16388` (`80100a88: addiu v0,zero,16388`),
`-1` (`80100a3c: addiu v0,zero,-1` → `sh v0,342(s2)`).

⇒ **`Sce_em_set` mit `+0x10E & 0x3f ∈ {5,7}` erzeugt einen unverwundbaren, dauerhaft am Fleck
verankerten Zombie in Substate 7.**

### 3.5 Weitere gemessene `+0x10E`-Bits

| Bit | Bedeutung | Beleg |
|---|---|---|
| `0x40` | **kein Koerperteil-/Kopf-Schaden** (Zerstueckelung uebersprungen) | `801055dc: lhu v0,270(s1)` / `801055e4: andi v0,v0,0x40` / `801055e8: bne v0,zero,0x80105624` |
| `0x80` | Blut-/Gore-Tropfroutine `0x80106128` laeuft jeden Frame | `801010c0: andi v0,v0,0x80` -> `801010cc: jal 0x80106128`; die Routine setzt das Bit selbst (`80106144: ori v0,v0,0x80`) und spielt die Voices `0x05032710` / `0x05031388` |

---

## 4. Die vollstaendige Substate-Karte (aufrecht, Tabellen 2/3)

Phase A = Entscheider, Phase B = Ausfuehrer; beide werden pro Frame nacheinander gerufen.

| Sub | Phase A | Phase B | Was es ist | Clip `+0x14C` | Fortbewegung? |
|---|---|---|---|---|---|
| 0 | `0x80101294` | `0x801013F4` | Idle/Beobachten -> wechselt nach 1 oder 12 | 3 (`80101470`) | B: ja (`8010161c`) |
| 1 | `0x80101714` | `0x80101A40` | **Verfolgen (gehen)** — enthaelt das Greif-Gate | 1 (`80101a8c`) | B: ja (`80101d60`) |
| 2 | `0x80101F7C` | `0x80102260` | Verfolgen, zweite Variante | 1 (`801022ac`) | B: ja (`8010246c`) |
| 3 | *Stub* `0x801025E4` | `0x801025EC` | **GRAB/BISS — der EINZIGE Schadenszweig** | 0x0B / 0x0E | erst ab `+0x6`=7 (`80102ca8`) |
| 4 | *Stub* `0x80103170` | *Stub* `0x80103178` | **leer** — beide `jr ra` | — | — |
| 5 | *Stub* `0x80103180` | `0x80103188` | Umwerfen/Aufstehen; setzt/loescht Bit `0x2000` | 0x0C, 3 | nein |
| 6 | *Stub* `0x8010394C` | `0x80103954` | **Toetungs-Finisher** (Anim auf dem SPIELER), dann -> Sub 8 mit ROOT | 0x18 (`801039c8`) | nein |
| **7** | *Stub* `0x80103778` | `0x80103780` | **ORTSFEST stehend, stoehnend** | 22/23, dann 8/9 | **nein** |
| **8** | *Stub* `0x80103B6C` | `0x80103B74` | **ORTSFEST kniend/fressend**, dreht sich langsam zum Spieler | 18/19/20, 21 | **nein**, aktiv verankert |
| 9 | *Stub* `0x80103E40` | `0x80103E48` | Sturz/Aufprall (setzt `0x2000`) | 2 (`80103f30`) | ja (`80103fbc`) |
| 10 | *Stub* `0x80104174` | `0x8010417C` | Aufmerksam-Werden: dreht nach `+0x1D4 & 0xfff`, wartet `rand&0x7f + 60` Frames, dann -> Sub 1 | aus `+0x218` | nein |
| 11 | *Stub* `0x80104394` | `0x8010439C` | Zu-Boden-Gehen -> setzt low6 = 1 (Krabbler) | `0x0002000A` | ja (`80104504`) |
| 12 | `0x8010460C` | `0x80104748` | Drehen am Ort; **zweites Greif-Gate** | 0x1B (`801047c0`) | B: ja (`801048e4`) |
| 13 | *Stub* `0x80104920` | `0x80104928` | Model-Tint-Routine (`jal 0x80017054`) | 1 (`80104a7c`) | nein |
| 14 | *Stub* `0x80104D6C` | `0x80104D74` | kurze Anim + `se_play(5)` (`80104e00`) | `0x000F0011` | nein |
| 15 | *Stub* `0x80104E4C` | `0x80104E54` | Spawn-Auftritt (INIT-Variante 10) | — | ja (`80104f24`) |

**Kriech-Tabellen (Tab 4 `0x8010C90C` / Tab 5 `0x8010C918`, je 3 Eintraege):**

| Sub | A | B | Was |
|---|---|---|---|
| 0 | `0x80102EE4` | `0x80103024` | Kriechen vorwaerts (`8010314c`), Clip `0x000F0005` |
| 1 | *Stub* `0x801025E4` | `0x801025EC` | **derselbe GRAB wie aufrecht Sub 3** |
| 2 | `0x80103A70` | `0x80103B48` | Idle liegend, Clip 0x17 (`80103b60`); A kann ROOT setzen (`80103a4c`) |

**Nicht erreichbar:** fuer Substate **4** (Doppel-Stub), **13** und **14** habe ich im gesamten
Overlay keinen Schreiber gefunden (Scan aller `sw rX,4(rY)` mit konstantem rX und aller
`sb rX,5(rY)` ueber `0x80100000..0x8010CF4C`). Substate 4 tut ohnehin nichts.

### 4.1 Sub 0 — der Entscheider `0x80101294` (byte-true)

```
801012c0: lw    s2,496(s0)         ; s2 = Distanz zum Spieler (+0x1F0)
801012c4: jal   0x80015614         ; turnstep(player, a3=1024)  -> s1
801012e4: jal   0x80015614         ; turnstep(player, a3=800)   -> v1
801012f0: sltiu v0,s2,0x1388       ; < 5000 ?
80101300: bne   s1,zero,0x80101324 ; nur wenn AUSGERICHTET (+/-1024)
80101310: andi  v0,v0,0x800        ; +0x154 & 0x800
8010131c: sw    v0,4(s0)  (v0=257) ; -> {state 1, sub 1}
80101320: sltiu v0,s2,0xbb8        ; < 3000
80101374: addiu v0,zero,3073
80101378: sw    v0,4(s0)           ; -> {state 1, sub 12}
8010137c: sltiu v0,s2,0x7d0        ; < 2000
801013cc: addiu v0,zero,3073
801013d0: sw    v0,4(s0)           ; -> {state 1, sub 12}
```
Schwellen: **5000 / 3000 / 2000** (`0x1388 / 0xBB8 / 0x7D0`) in derselben Einheit wie `+0x1F0`.

### 4.2 Das GREIF-GATE — Sub 1 (`0x801018E8`) und Sub 12 (`0x8010463C`)

```
801018e8: lbu   v1,8(s1)           ; s1 = Spieler 0x800CFBF8 ; +0x8
801018f4: sltiu v0,s2,0x4b0        ; Distanz < 1200
80101900: lbu   v0,467(s1)         ; Spieler +0x1D3
80101908: andi  v0,v0,0x80         ;   schon gepackt -> nein
80101914: lbu   v1,262(s0)         ; Zombie-Floor
80101918: lbu   v0,262(s1)         ; Spieler-Floor  -> muessen gleich sein
80101928: lhu   v0,538(s0)         ; +0x21A Bit 0x20 sperrt die LINKE Variante
80101940: lh    a2,118(s0)         ; Zombie-Yaw
80101948: jal   0x80015758         ; Kegel: Spieler in +/-256 um (Yaw + 256)?
8010194c:   addiu a2,a2,256
80101954: addiu v0,zero,769
80101958: sw    v0,4(s0)           ; -> {state 1, sub 3} = GRAB
80101968: sb    v0,467(s1)         ; Spieler +0x1D3 |= 0x80
```
und spiegelbildlich `8010197c..801019ac` mit `a2 = Yaw - 256`, gesperrt durch `+0x21A & 0x40`.
Sub 12 hat dasselbe Gate mit **Distanz < 1300** (`8010463c: sltiu v0,a0,0x514`), zusaetzlich
`+0x14D < 11` (`80104668: sltiu v0,v0,0xb`) und Floor-Vergleich gegen `0x800CFCFE` (= Spieler+0x106).

`FUN_80015758(pA,pB,winkel,tol)` liefert **0**, wenn B in +/-`tol` um `winkel` liegt
(`800157ac: andi v0,v0,0xfff` / `800157b4: sltu v0,v0,s0` mit `s0 = 2*tol` / `800157b8: xori v0,v0,1`).
`tol = 256` von 4096 = **+/-22,5 Grad**, Mitte 22,5 Grad links bzw. rechts der Blickachse.

### 4.3 Sub 3 — der GRAB (`0x801025EC`), 10 Sub-Substates ueber `0x8010001C`

Varianten-Index `s5`:
```
8010266c: lhu   v0,270(s1)
80102674: andi  s5,v0,0x1      ; Kriech-Bit
80102670: lbu   v1,8(s1)       ; Gegner-Typ
8010267c: beq   v1,v0,…        ; v0 = 23 (0x17)
80102688: bne   v1,v0,…        ; v0 = 17 (0x11)
80102690: addiu s5,s5,2        ; Typ 0x17 / 0x11 -> Variante +2
```
Datentabellen (`bytes 0x80100000 80 --bin EMZ0.BIN`; Datei-Offset = Adresse - 0x80100000):
```
8010000c: 0b 0b 0e 0e 0b 0b 0e 0e   ; Clip je Variante: s5 gerade -> 0x0B, ungerade -> 0x0E
80100014: 10 14 01 05 10 1e 01 0a   ; (Trigger-Frame, SCHADEN) je Variante:
                                    ;   s5=0 -> Frame 0x10, dmg 20
                                    ;   s5=1 -> Frame 0x01, dmg  5
                                    ;   s5=2 -> Frame 0x10, dmg 30
                                    ;   s5=3 -> Frame 0x01, dmg 10
```
Auslesen: `801026c4-cc: sll v0,s5,1 / addu v0,sp,v0 / lbu v1,16(v0)` (Clip),
`80102898-a4: addiu v0,sp,64 / addu s0,v0,v1 / lbu v0,0(s0)` + `801028f4: lbu a0,1(s0)`.

Ausloesen des Bisses:
```
801028a0: lbu v1,333(s1)      ; aktueller Anim-Frame (+0x14D)
801028a4: lbu v0,0(s0)        ; Trigger-Frame aus 0x80100014
801028ac: bne v1,v0,Ende
801028e8: addiu a0,zero,3
801028ec: jal 0x8005bd6c      ; se_play(3, zombie)
801028f4: lbu a0,1(s0)        ; Schadenswert
801028f8: jal 0x800401d4      ; player_damage(dmg, s5&1)
801028fc:   andi a1,s5,0x1
```
Bindung des Spielers an den Zombie (s3 = `0x800CFBF8`):
```
80102710: sw s1,436(s3)   ; Spieler+0x1B4 = Zeiger auf den Zombie
80102728: sw v0,4(s3)     ; Spieler+0x4 = {state 5, sub = same_facing()}
80102744: sb v0,5(s3)     ; +2 falls s5&1
80102750: sw v0,392(s3)   ; Spieler+0x188 <- Zombie+0x188
80102760: sb v0,467(s3)   ; Spieler+0x1D3 |= 0x80
80102788: sw v0,0(s3)     ; Spieler+0x0 |= 0x1000
```
Sounds in diesem Zweig: `se_play` **10/11** (`801027ac/b0`, per `rand&1`), **3** (`801028e8`),
**8** (`80102b58`), **12** (`80102ddc`), **4** (`80102ea4`).

### 4.4 Sub 6 — Finisher; und wie ein Zombie DAUERHAFT verankert wird

```
801039b0: ori v0,v0,0x18        ; v0 = 0x000F0018 -> Clip 0x18, +0x14E = 0x0F
801039d0: jal 0x80015b94        ; a0 = 0x800CFBF8 -> spielt die Anim auf dem SPIELER
801039e8/ec: a0 = 11 bzw. 10 (rand&1)
801039f0: jal 0x8005bd6c        ; Stoehnen
801039a4: addiu v1,zero,2049    ; = 0x0801 -> {state 1, sub 8}
80103a44: lhu v0,270(s0)
80103a48: sw  v1,4(s0)
80103a4c: ori v0,v0,0x4000      ; ROOT-LOCK setzen
80103a50: sh  v0,270(s0)
```
Ebenso in Sub 1 und Sub 2, wenn der Spieler **tot** ist:
```
801019e8: lh    v1,342(s1)      ; Spieler-HP (0x800CFD4E)
801019ec: addiu v0,zero,-32768
801019f0: bne   v1,v0,Ende      ; nur wenn HP == 0x8000 (Tod-Marker)
801019fc: sltiu v0,s2,0x3e8     ; Distanz < 1000
80101a04: lui   v1,0x6 ; ori v1,v1,0x801   ; 0x00060801 -> {state 1, sub 8, +0x6 = 6}
80101a14: ori   v0,v0,0x4000    ; ROOT-LOCK
```
(identisch `0x80102220..38` in Sub 2.)

---

## 5. ⭐ DER FUND: die beiden ORTSFESTEN Zweige (Substate 7 und 8)

Beide sind genau das, was gesucht war: **kein `pos_advance`, kein Root-Motion-Vorwaerts, aber
Anim + Stoehnen + langsames Zuwenden zum Spieler — und sie verlassen ihren Zustand NIE, wenn
Bit `0x4000` von `+0x10E` gesetzt ist.** Ihre Phase-A-Eintraege sind Stubs, d.h. sie haben
ueberhaupt keinen Entscheider, der sie wegschalten koennte.

### 5.1 Substate 7 `0x80103780` — stehend, ortsfest, stoehnend

Sub-Substate-Sprungtabelle `0x80100094` (5 Eintraege):
`801037CC, 8010381C, 80103838, 801038D8, 80103900`.

```
; +0x6 = 0  (Einstieg)
801037cc: lhu v0,538(s0)      ; +0x21A & 4 ?
801037d4: andi v0,v0,0x4
801037d8: beq  v0,zero,0x801037e8
801037dc: sw   v1,332(s0)     ; v1 = 23  -> Clip 23
801037e0: addiu v0,zero,22
801037e4: sw   v0,332(s0)     ; sonst    -> Clip 22
801037f4: jal  0x8002959c     ; Anim setzen
801037fc: addiu v0,zero,1
80103800: sb   v0,6(s0)       ; +0x6 = 1
80103804: lbu  v0,467(s0) ; ori 0x80 ; sb -> +0x1D3 |= 0x80
80103808: lbu  v1,448(s0) ; ori 0x01 ; sb -> +0x1C0 |= 0x01
8010381c: lhu  v0,270(s0)
80103824: andi v0,v0,0x4000
80103828: bne  v0,zero,0x80103930   ; **ROOT gesetzt -> +0x6 BLEIBT 1, fuer immer**
8010382c: addiu v0,zero,2
80103830: j    0x80103930
80103834: sb   v0,6(s0)             ; sonst +0x6 = 2 (weiter)

; +0x6 = 2  (nur ohne ROOT erreichbar): zufaelliger Stoehn-Loop
80103838: jal 0x80015fe8            ; rand
80103840: lui  v1,0xf ; ori v1,v1,0x8   ; 0x000F0008 -> Clip 8
80103848: andi v0,v0,0xf ; sll v0,v0,8  ; Startframe = rand & 0x0F
80103858: andi a0,a0,0x4                ; +0x21A & 4 ?
8010386c: lui  v1,0xf ; ori v1,v1,0x9   ; dann Clip 9 statt 8
80103884: lbu  v1,569(s0)               ; Stoehn-Cooldown +0x239
80103890: sb   v0,6(s0)   (v0=3)
801038a8/b0/c4: a0 = 12 / 10 / 11       ; zwei rand()-Wuerfe waehlen die Stimme
801038c8: jal 0x8005bd6c                ; se_play(id, zombie)
801038d0: addiu v0,zero,150
801038d4: sb   v0,569(s0)               ; Cooldown = 150 Frames

; +0x6 = 4: Ausstieg
80103900: addiu v1,zero,257 ; sw v1,4(s0)   ; -> {state 1, sub 1} (verfolgen)
80103914: andi v0,v0,0x7f  ; +0x1D3 &= ~0x80
80103920: andi v1,v1,0xfe  ; +0x1C0 &= ~0x01
80103928: or   v0,v0,a0    ; a0 = 0x0C000000 -> Flags +0x0 |= 0x0C000000
```
Fortbewegung: **keine** — weder `0x800152C8` noch `0x80015E7C` liegen im Bereich
`0x80103780..0x8010394B` (Scan aller `jal`-Ziele im Overlay, §7.2).

### 5.2 Substate 8 `0x80103B74` — kniend/fressend, aktiv am Fleck verankert

Sub-Substate-Sprungtabelle `0x801000B4` (9 Eintraege):
`80103BE8, 80103C18, 80103CB0, 80103CD8, 80103D60, 80103D90, 80103DB4, 80103DD8, 80103E20`.
Clip-Tabelle `0x801000A8`: `12 13 14 12 13 14 12 13` (= Clips 18/19/20, per `rand&7`).

```
; +0x6 = 0
80103bec: jal 0x80015fe8
80103bf4: andi v0,v0,0x7 ; addu v0,sp,v0 ; lbu v0,16(v0)  ; Clip aus 0x801000A8
80103bfc: lui  a0,0x7    ; addu v0,v0,a0                   ; +0x14E = 7
80103c10: sw   v0,332(s1)
80103c24: jal  0x8002959c
80103c38: bne  a0,zero,0x80103c50        ; a0 = +0x14E
80103c44: addu a1,zero,zero              ; a1 = 0
80103c48: jal  0x80016200                ; **Root-Motion RUECKGAENGIG -> Position eingefroren**
80103c4c: addiu a2,zero,1
80103c50: lw   v0,376(s1) ; lw v0,0(v0)  ; Anim-Frame-Flags (+0x178)
80103c5c: lui  v1,0x1 ; and v0,v0,v1     ; Flag-Bit 0x00010000
80103c70: sh   v0,24(sp)  (v0=200)       ; Voice-Volumen/Abstand
80103c80: addiu a0,zero,5072             ; = 0x13D0
80103c8c: jal  0x8001bf10                ; positionierte Voice
80103c94: lhu  v0,270(s1)
80103c9c: andi v0,v0,0x4000
80103ca0: bne  v0,zero,0x80103e24        ; **ROOT -> +0x6 bleibt 1, fuer immer**
80103ca4: addiu v0,zero,3
80103cac: sb   v0,6(s1)                  ; sonst +0x6 = 3

; +0x6 = 3: aufrichten + stoehnen
80103cd8: lui v1,0xf ; ori v1,v1,0x15 ; sw v1,332(s1)   ; Clip 21, +0x14E = 0x0F
80103cf0: sb  v1,564(s1)  (v1=2)
80103d08: ori v0,v0,0x10  ; sh -> +0x21A |= 0x10
80103d38/4c: a0 = 11 / 10
80103d50: jal 0x8005bd6c                                 ; Stoehnen
80103d58: addiu v0,zero,150 ; sb v0,569(s1)              ; Cooldown 150

; +0x6 = 5: Ausstieg  -> {state 1, sub 1}
80103d90: addiu v0,zero,257 ; sw v0,4(s1)

; +0x6 = 6 / 7: Clip 21 mit zufaelligem Startframe, danach LANGSAM zum Spieler drehen
80103db4: addiu v0,zero,7 ; sb v0,6(s1)
80103dc0: lui v1,0xf ; ori v1,v1,0x15 ; addu v0,v0,v1 ; sw v0,332(s1)  ; Clip 21, Frame rand&7
80103ddc: lw  a1,-976(0x800d0000)   ; = 0x800CFC30 playerX
80103de4: lw  a2,-968(0x800d0000)   ; = 0x800CFC38 playerZ
80103dec: jal 0x80015558
80103df0:   addiu a3,zero,16        ; **Drehschritt 16/4096 pro Frame = ca. 1,4 Grad**
80103e04: jal 0x8002959c            ; a3 = 0x00010100
```
Fortbewegung: **keine** — und zusaetzlich wird die Root-Translation der Anim jeden Frame wieder
abgezogen (`80103c48: jal 0x80016200` mit `a1 = 0`; `FUN_80016200` subtrahiert dann
`+0x38 -= dx`, `+0x40 -= dz`, s. `RE2_Quellcode_V2/FUN_80016200.c`). Der Zombie **kann sich
gar nicht** von der Stelle bewegen, auch wenn die Animation Translation enthaelt.

---

## 6. Welche RE2-Raeume das tatsaechlich benutzen (RDT-Belege)

`Sce_em_set` = Opcode **0x44**, **22 Byte** (Beleg §3.1). Feldlage im Record:
`[0]=0x44 [1]=? [2]=Slot [3]=Typ [4..5]=+0x10E [6]=Floor [7..9]=? [10..11]=X [12..13]=Y
[14..15]=Z [16..17]=Yaw [18..21]=?`.
Gescannt wurde der Block zwischen Header-Offset `@0x48` und `@0x50` aller 495 RDTs unter
`info/re2leon/PL0/RDT/`. (Hinweis: das ist ein **validierter Mustersuchlauf**, kein vollstaendiger
SCD-Walk — die Kontrollfluss-Opcodes sind nicht ausdekodiert. Die unten zitierten Treffer sind
jeweils mit Datei-Byte-Offset und Roh-Bytes belegt und stehen in 22-Byte-Ketten, also echt.)

### 6.1 `ROOM1130.RDT` — fuenf verankerte Zombies in Substate 8

```
file 0x001212: 44 00 01 1f 08 40 00 05 03 52 55 a2 00 00 fb af 71 0b 00 00 00 00
file 0x001228: 44 00 02 1f 08 40 00 05 03 53 66 a8 00 00 8b b9 51 06 00 00 00 00
file 0x00123e: 44 00 03 1f 08 40 00 06 03 71 02 9b 00 00 aa b5 79 00 00 00 00 00
file 0x0013f2: 44 00 05 1f 08 40 00 06 03 d3 37 a6 00 00 ef b1 b1 0a 00 00 00 00
```
`+0x10E = 0x4008` = ROOT(0x4000) + Variante 8 -> INIT setzt `{state 1, sub 8}`, Clip 18
(`80100ad4/dc`), und Substate 8 bleibt wegen `80103c9c` fuer immer bei `+0x6 = 1`.
Ein zweiter Block derselben Datei setzt dieselben Zombies mit `+0x10E = 0x0006` bzw. `0x0000`
(also OHNE ROOT) — offensichtlich der Szenario-/Schwierigkeits-Alternativpfad.

### 6.2 `ROOM1190.RDT` — vier verankerte, nicht zerstueckelbare Zombies

```
file 0x000dae: 44 00 01 1f 48 40 00 07 01 a4 6f 29 00 00 a0 a6 f0 06 00 00 00 00
file 0x000dc4: 44 00 02 1f 48 40 00 29 00 a5 48 23 00 00 5c ab 60 04 00 00 00 00
file 0x000dda: 44 00 03 1f 48 40 00 07 02 a6 7d 1b 00 00 36 a9 f0 00 00 00 00 00
file 0x000df0: 44 00 04 1f 48 40 00 29 00 a7 5f 22 00 00 9a a1 70 0e 00 00 00 00
```
`+0x10E = 0x4048` = ROOT + Bit `0x40` (kein Koerperteil-Schaden, §3.5) + Variante 8.
Und ein Slot mit `0x4002` (`file 0x000c5e`) = ROOT + Variante 2 -> **Substate 7**.

### 6.3 `ROOM2150.RDT` — Zombie-Typ 0x12 als reine Kulisse

```
file 0x0012d6: 44 00 01 12 02 40 00 03 00 …   ; +0x10E = 0x4002 -> ROOT, Sub 7, Clip 22
file 0x0012ec: 44 00 02 12 04 40 00 04 00 …   ; +0x10E = 0x4004 -> ROOT, Sub 7, Clip 23
file 0x0013f4: 44 00 00 12 0a 40 00 03 00 …   ; +0x10E = 0x400a -> ROOT, Variante 10
```
Weitere Raeume mit demselben Muster (aus dem Gesamtlauf): `ROOM4050`, `ROOMD050` (`0x4002`/`0x4004`),
`ROOM1030` (`0x5002` = ROOT + 0x1000 + Variante 2), `ROOM2100`/`ROOM1140`/`ROOM2050`
(Typ 0x10 mit `0x4002`/`0x4006`/`0x400a`), `ROOM1090` (Typ 0x21 mit `0x4000`/`0x4002`),
`ROOMA1A0`, `ROOM5020`, `ROOM20B0`.

### 6.4 ⛔ Was das NICHT ist

Es gibt **kein** Raum-Skript und **keinen** `+0x10E`-Wert, der einen *ortsfesten* Zombie
*schadensfaehig* macht. Die Werte `0x4002 / 0x4004 / 0x4008 / 0x4048 / 0x400a` fuehren
ausnahmslos in Substate 7 oder 8, und deren Phase-A-Eintraege sind Stubs.

---

## 7. Ergebnis

### 7.1 GEFUNDEN: der ortsgebundene Zweig

**RE2 hat sehr wohl eine ortsgebundene Zombie-KI.** Sie besteht aus zwei Substates, die

* sich **nicht** fortbewegen (kein `0x800152C8`, kein `0x80015E7C` in ihrem Adressbereich),
* im Fall von Substate 8 die Root-Translation der Animation sogar **aktiv zurueckrechnen**
  (`80103c48: jal 0x80016200` mit `a1 = 0`),
* per `rand()` **stoehnen** (`se_play` 10/11/12, Cooldown 150 Frames in `+0x239`),
* sich (Substate 8, `+0x6 = 7`) mit **16/4096 pro Frame** langsam zum Spieler drehen,
* und — bei gesetztem `+0x10E & 0x4000` — ihren Zustand **nie** verlassen
  (`80103828` bzw. `80103ca0`), weil ihre Phase-A-Entscheider reine `jr ra`-Stubs sind
  (`0x80103778`, `0x80103B6C`).

Aktiviert wird das **ausschliesslich** ueber das `Sce_em_set`-Feld `pc[4..5]` -> `+0x10E`
(`80057354`), mit den in echten RE2-Raeumen belegten Werten `0x4002 / 0x4004 / 0x4008 /
0x4048 / 0x400a`. Mit `pc[4..5] & 0x3f ∈ {5,7}` kommen zusaetzlich **HP = -1** (unverwundbar,
`80100a3c-40` / `80100a90-94`) und ein erzwungenes `+0x10E := 0x4002` bzw. `0x4004` dazu.

### 7.2 NICHT GEFUNDEN: ein ortsgebundener Zweig, der GREIFT und SCHADEN macht

* Im gesamten Zombie-Overlay gibt es **genau einen** Aufruf von `player_damage`:
  ```
  801028f8: jal 0x800401d4      (einziger Treffer im Bereich 0x80100000..0x8010CF4C)
  ```
  Er liegt in Substate 3 (`0x801025EC`).
* Substate 3 wird **nur** aus Substate 1 (`80101958`, `8010199c`), Substate 2 (`80102178`,
  `801021bc`) und Substate 12 (`801046d8`, `8010471c`) heraus gesetzt — alle drei sind
  Fortbewegungs-Substates (`80101d60`, `8010246c`, `801048e4` = `MOVE_FORWARD`).
* Substate 7 und 8 koennen Substate 3 **nicht** erreichen: ihre Phase A ist ein Stub, ihre
  Phase B schreibt `+0x4/+0x5` nur nach `{1,1}` (`8010390c`, `80103d94`).
* Es gibt keinen `+0x10E`-Wert 0..63, der einen verankerten Zombie schadensfaehig macht — die
  ACTIVE-Tabelle unterscheidet ueber `+0x10E & 0x3f` **nur** aufrecht/kriechend
  (`8010115c: andi v0,v0,0x3f` und die 14 Eintraege ab `0x8010C854`).

**Fazit:** Die RE2-Fensterszene, an die sich der Nutzer erinnert, ist im Zombie-Overlay als
**verankerter, stoehnender, den Spieler anschauender Zombie** implementiert — aber der
Greif-Angriff daraus existiert nicht. Wer die Szene im Port haben will, kombiniert die
byte-true belegte Verankerung (Substate 7/8, `+0x10E |= 0x4000`) mit dem byte-true belegten
Greif-Gate aus §4.2 — das ist dann eine **Nachruestung nach RE2-Vorbild**, kein RE2-1:1-Port,
und muss so etikettiert werden.

### 7.3 Ausdruecklich ausgeschlossen (mit Beleg)

| Hypothese | Widerlegt durch |
|---|---|
| „`0x8010C854` hat 52 Handler / 38 unerforschte Verhaltensfunktionen" | Drei ueberlappende Tabellen, 13 der 36 Substate-Eintraege sind `jr ra`-Stubs (§1) |
| „Ein Raum-Overlay (STAGE1..7.BIN) setzt den Zweig" | 0 Treffer auf Immediate `0x10e`/`0x10f` in allen sieben Dateien (§3.2) |
| „`+0x10E & 0x3f` waehlt 14 verschiedene Verhalten" | Alle geraden Indizes -> `0x8010118C`, alle ungeraden -> `0x80101210` (`table 0x8010c854 64`) |
| „Der Greifer sitzt in einem der hohen Indizes 14..51" | Diese Worte sind Teil der Substate-Tabellen 2..5, keine ACTIVE-Eintraege (§1.2/1.3) |
| „EMZ0_d1 / EMOVL10_S0 sind andere Zombie-Varianten" | sha1-identisch mit EMZ0.BIN (§0) |
| „Es gibt einen zweiten Schadensaufruf" | genau ein `jal 0x800401d4` im Overlay (§7.2) |

### 7.4 Offen / NICHT ermittelt

* **Welche Pose die Clips 8, 9, 18, 19, 20, 21, 22, 23 zeigen** — dafuer muesste die RE2-EMD/EDD
  des Zombies gerendert und katalogisiert werden. Ich habe die Clip-NUMMERN belegt, nicht ihr
  Aussehen. **NICHT GEFUNDEN** — naechster Weg: `info/re2leon/COMMON/BIN/EM_TYPE20.EMD` +
  `EM_TYPE20.TIM` mit dem vorhandenen `EmdGltfExtractor` durchjagen und die Clips rendern.
* **Welcher physische RE2-Raum `ROOM1130` / `ROOM1190` ist** — ich habe die RDT-Daten, aber keine
  belegte Raum-Namen-Zuordnung. **NICHT GEFUNDEN**, und ich rate sie nicht.
* **Bedeutung von `+0x154 & 0x800`** (Gate in Sub 0/1/2 vor jedem Zustandswechsel) und von
  `0x800CFBF6 & 0x15 / & 0x17` (Spieler-Statuswort) — **NICHT GEFUNDEN**.
  Naechster Weg: Schreiber von `0x800CFBF6` in der RE2-EXE suchen (Immediate-Scan wie §3.2).
* **Sub-Substates 4, 13, 14** haben keinen gefundenen Schreiber (§4) — moeglicherweise nur ueber
  die Master-Handler `0x8010A440` / `0x80109CFC` erreichbar, die ich nicht vollstaendig
  disassembliert habe.
* Der vollstaendige **SCD-Walk** fuer RE2 fehlt (Kontrollfluss-Opcodes `0x01,0x03,0x06,0x07,
  0x09,0x0c,0x0f..0x13,0x17..0x1a,0x1d,0x28,0x2d,0x31..0x33,0x36,0x37,0x39,0x40,0x46,0x48,
  0x49,0x4d,0x4e,0x53,0x69..0x6b,0x72,0x74,0x7c..0x7f,0x88,0x8d` liefern ihre Laenge nicht ueber
  das ausgewertete `addiu rX,rX,N / sw rX,28(rY)`-Muster). Der Raum-Scan in §6 ist deshalb ein
  validierter Mustersuchlauf, kein Walk.

---

## 8. Bauplan fuer den Port (NACHRUESTUNG nach RE2-Vorbild, nicht 1:1)

Alles hier stammt 1:1 aus den oben belegten RE2-Adressen; die **Kombination** (Verankerung +
Greifen) ist die Nachruestung und muss als solche im Code-Kommentar stehen.

1. **Verankerung** (RE2 `+0x10E & 0x4000`, `80103824` / `80103c9c`): ein Gegner-Flag „ortsfest".
   Solange gesetzt, keine Zustandswechsel-Entscheidung und kein Root-Motion-Vorwaerts;
   pro Frame die Anim-Translation wieder abziehen (RE2 `0x80016200(a1=0)` @`80103c48`).
2. **Idle-Pose + Stoehnen** (RE2 Sub 7 `0x80103780`): Grund-Clip 22 bzw. 23 je Flag `+0x21A & 4`
   (`801037d4-e4`); danach Clip 8 bzw. 9 mit **Startframe `rand() & 0x0F`** (`80103848`,
   `80103874`); Stimme per zwei `rand()&1`-Wuerfen aus **{10, 11, 12}** (`801038a8/b0/c4`);
   **Cooldown 150 Frames** in `+0x239` (`801038d0`).
3. **Zuwenden** (RE2 Sub 8 `+0x6=7`, `80103dec-f0`): `turn_toward(playerX, playerZ, step = 16)`,
   also **16/4096 pro Frame**.
4. **Greif-Gate** (RE2 Sub 1 `0x801018E8`, wenn der Nutzer das Greifen will):
   * Distanz `< 1200` (`801018f4: sltiu v0,s2,0x4b0`) — fuer ein Fenster ggf. kleiner waehlen,
     dann ist die Zahl aber **gewaehlt**, nicht belegt: als solche kennzeichnen.
   * Spieler nicht schon gepackt (`+0x1D3 & 0x80`, `80101908`).
   * gleiche Etage (`80101914/18`).
   * Kegel **+/-22,5 Grad** um `Yaw +/- 256` (`80101948` mit `a3 = 256`), Seite per
     `+0x21A & 0x20` / `& 0x40` sperrbar.
5. **Biss/Schaden** (RE2 Sub 3, §4.3): Trigger auf Anim-Frame **0x10** (stehend) bzw. **0x01**
   (liegend), Schaden **20 / 5 / 30 / 10** je Variante (`0x80100014`), `se_play(3)` beim Treffer
   (`801028e8`), Spieler an den Greifer binden (`80102710`, `80102728`, `80102760`).

---

## 9. Reproduktion

```bash
cd C:/workspace/git/reAi_v2
D=.claude/skills/re15-psx-disasm/scripts/re2_disasm.py     # bereits auf RE2 konfiguriert
python $D table 0x8010c830 12 --bin EMZ0.BIN     # Master-Zustandstabelle
python $D table 0x8010c854 64 --bin EMZ0.BIN     # ACTIVE + die drei Substate-Tabellen
python $D dis   0x8010114c 20 --bin EMZ0.BIN     # ACTIVE-Dispatcher
python $D dis   0x8010118c 40 --bin EMZ0.BIN     # aufrecht: Sub-Dispatcher (Phase A/B)
python $D dis   0x80103780 80 --bin EMZ0.BIN     # Substate 7 (ortsfest)
python $D dis   0x80103b74 180 --bin EMZ0.BIN    # Substate 8 (ortsfest, verankert)
python $D dis   0x801025ec 200 --bin EMZ0.BIN    # Substate 3 (GRAB)
python $D bytes 0x80100000 80 --bin EMZ0.BIN     # Clip-/Schadens-Tabellen
python $D dis   0x8005714c 200                   # Sce_em_set (EXE)
python $D table 0x800a74c8 143                   # SCD-Opcode-Dispatch (0x44 = Sce_em_set)
```
`re2_disasm.py` beherrscht `lwl/lwr/swl/swr` (op 0x22/0x26/0x2a/0x2e) noch nicht — die tauchen in
`0x801025EC` und `0x80103B74` als `.word …(op22)` auf; es sind die 8-Byte-Kopien der Clip-/
Schadenstabellen von `0x8010000C` / `0x80100014` / `0x801000A8` auf den Stack.

---

## Verifikation (unabhaengig nachdisassembliert)

Datum: 2026-08-25. Alle Adressen mit einem eigenen Mini-Decoder (importiert `dis_one()` aus
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`) direkt aus
`info/re2leon/COMMON/BIN/EMZ0.BIN` (RAW @`0x80100000`, 0xCF4C Bytes) und
`info/re2leon/PSX.EXE` (`t_addr`=`0x80010000`, text @Datei `0x800`) nachdisassembliert.
Gesamturteil: **TEILWEISE** — Ergebnis (1) traegt vollstaendig, Ergebnis (2) traegt im Kern
(Verankerung + Stoehnen), aber die zentrale **Negativ**-Behauptung ("kein Pfad zum Greifen")
ist **widerlegt**.

### A. BESTAETIGT (byte-genau nachgeprueft)

* **Tabellen-Layout 14/16/16/3/3 = 52.** `8010116c: lw v0,-14252(at)` mit `lui at,0x8011`
  -> `0x8010C854`; `801011bc: -14196` -> `0x8010C88C`; `801011e4: -14132` -> `0x8010C8CC`;
  `80101240: -14068` -> `0x8010C90C`; `80101268: -14056` -> `0x8010C918`. Tab1 alterniert
  exakt 14x `8010118c/80101210`, ab Wort 14 beginnt Tab2. Ab `0x8010C924` (Wort 52) steht
  Nicht-Code (`803200be 80400096 ...`).
* **13 Stubs / 23 echte Funktionen.** Die 38 Tab2..Tab5-Eintraege ergeben 36 eindeutige
  Adressen (`0x801025e4`/`0x801025ec` doppelt); genau 13 davon beginnen mit `03e00008 jr ra`:
  `801025e4, 80103170, 80103178, 80103180, 80103778, 8010394c, 80103b6c, 80103e40, 80104174,
  80104394, 80104920, 80104d6c, 80104e4c`. Rest = 23. Insbesondere sind die Phase-A-Eintraege
  von Substate 7 (`0x80103778`) und 8 (`0x80103b6c`) tatsaechlich Stubs.
* **Uebergeordnete Ebene (vom Dossier nicht erwaehnt, stuetzt es aber):**
  `801004e0: lw v0,-14288(at)` -> Tabelle `0x8010C830[9]`, Index = entity`+0x4`:
  `[0]=8010065c INIT, [1]=8010114c ACTIVE, [2]=80104f40, [3]=80108250, [4]=80065c88 (EXE),
  [7]=8010a440, [8]=80109cfc`. `0x8010C854` schliesst luekenlos daran an.
* **Substate 7 = `0x80103780`**: 5-Fall-Sprungtabelle `0x80100094`
  (`801037bc: lw v0,148(at)`) = `801037cc, 8010381c, 80103838, 801038d8, 80103900`.
  Clip 23 (`801037dc`) bzw. 22 (`801037e4`) je `+0x21A&4`; Clip `0x000F0008|((rand&0xf)<<8)`
  (`80103840-60`) bzw. `...0009` (`8010386c-80`); `se_play`-Args 12/10/11 an
  `801038ac/801038c0/801038c4` (**nicht** `801038a8/b0/c4` wie zitiert — dort stehen
  `j` bzw. `jal`); Cooldown `addiu v0,zero,150 / sb v0,569(s0)` `801038d0/d4`. Der Cooldown
  `+0x239` wird pro Frame im Entity-Treiber dekrementiert (`8010045c-8010046c`).
* **`0x80103824` Verankerung.** Fall 1 der Sub-Tabelle ist `0x8010381c`, und der Fall besteht
  NUR aus `lhu +0x10E / andi 0x4000 / bne -> Epilog` bzw. `+0x6:=2`. Bei gesetztem Bit bleibt
  `+0x6` also tatsaechlich **fuer immer 1** — fuer Substate 7 exakt wie behauptet.
* **Substate 8 = `0x80103B74`**: 9-Fall-Tabelle `0x801000B4` (`80103bd8: lw v0,180(at)`)
  = `80103be8, 80103c18, 80103cb0, 80103cd8, 80103d60, 80103d90, 80103db4, 80103dd8, 80103e20`.
  `0x80103B98-BB4` sind `lwl/lwr/swl/swr` (op 0x22/0x26/0x2a/0x2e) — die 8-Byte-Kopie von
  `0x801000A8` (`12 13 14 12 13 14 12 13`) auf `sp+0x10`. `80103c48: jal 0x80016200` mit
  `a1=0`, `a2=1` — und `FUN_80016200` subtrahiert bei `param_2==0` tatsaechlich von `+0x38/+0x40`.
  (Nebenbedingung, im Dossier nicht genannt: die Verankerung laeuft nur bei `+0x14E == 0`,
  `80103c30/38: lbu a0,334(s1) / bne a0,zero,0x80103c50`.)
* **Keine Fortbewegung in 7/8.** Vollstaendiger `jal`-Scan beider Adressbereiche: nur
  `80015fe8 (rand), 8002959c (anim), 8005bd6c (se), 80016200 (anchor), 80015558 (turn),
  8001bf10 (fx)`. `0x800152C8` (= `+0x38/+0x40 += rot(+0x144)`) und `0x80015E7C`
  (= Root-Delta nach `+0x144`) kommen nicht vor; `FUN_80029614` (via `8002959c`) schreibt
  weder `+0x38` noch `+0x40`, sondern nur die Bone-Matrizen und `+0x14D`.
* **`jal 0x800401d4` genau 1x.** Exhaustiver Scan `0x80100000..0x8010CF4C`: einziger Treffer
  `801028f8`. `FUN_800401d4 @0x80040248: lhu v0,342(a2)` mit `a2=0x800CFBF8` -> `0x800CFD4E`.
* **Grab-Tabellen byte-genau.** Rohbytes: `0x8010000C: 0b 0b 0e 0e 0b 0b 0e 0e`;
  `0x80100014: 10 14 01 05 10 1e 01 0a`; `0x801000A8: 12 13 14 12 13 14 12 13`.
  Index `2*s5` (`80102888: sll v1,s5,1`), `s5 = +0x10E&1 (+2 falls Typ 0x17/0x11)`
  (`8010266c-90`), Trigger-Frame `lbu 0(s0)` vs `+0x14D`, Schaden `lbu 1(s0)` (`801028f4`).
  Sprungtabelle des Grabs `0x8010001C` mit 10 Eintraegen (`801026b0: lw v0,28(at)`) bestaetigt.
  Spieler-Bindung `80102710: sw s1,436(s3)`, `80102728: sw v0,4(s3)`,
  `80102760: sb v0,467(s3)` mit `s3 = 0x800CFBF8` (`80102664-68`) bestaetigt.
* **Kegeltest `0x80015758`** exakt wie zitiert (Dossier-Tippfehler: `8005779c` -> `8001579c`);
  Rueckgabe 0 = innerhalb +/-tol, `tol=256/4096 = 22,5 Grad`.
* **Sce_em_set.** `80057170-78` legt den SCD-pc in `0x800D5BE8` ab; `80057344/4c/54` liest
  `pc[4..5]` und schreibt `sh v0,270(s0)`; `800576e4: addiu v1,v1,22`. Dispatch-Tabelle
  `0x800A74C8` (`80053f84: lw v0,29896(at)`), Eintrag `0x800A75D8 = 0x8005714C` -> Opcode
  **0x44**. Tabelle hat 0x8F Eintraege (0x00..0x8E), ab `0x8F` Daten.
* **Spawn-Varianten.** `801009e8` (Var 2 -> `0x0701`), `80100a0c` (Var 4), `80100a2c/34/38/3c/40`
  (Var 5: `0x0701`, `+0x10E:=16386`, `+0x156:=-1`), `80100a80/88/8c/90/94` (Var 7: `16388`, HP=-1),
  `80100ad4/d8/dc` (Var 8: Clip 18, `+0x4:=2049=0x0801`). Alles exakt.
* **Distanz `+0x1F0`.** `800265a4-d8` (dx=`+0x38`, dz=`+0x40`, `jal 0x8008d2f4`),
  `800265e0: sw v0,496(s0)`. Exakt.
* **RDT-Bytes.** `ROOM1130.RDT@0x001212` und `ROOM1190.RDT@0x000dae` / `@0x000c5e` stimmen
  Byte fuer Byte mit den zitierten 22 Bytes ueberein (`0x4008`, `0x4048`, `0x4002`).
* **STAGE1..7.BIN (RE2): 0 Zugriffe auf `+0x10E`** — eigener Immediate-Scan
  (op 0x20/21/23/24/25/28/29/2b, imm 270/271) bestaetigt 0 in allen sieben; EMZ0 hat 81.
* **`EMZ0.BIN` = `EMOVL10_S0.BIN` = `EMZ0_d1.BIN`**, sha1 `2d8885fc05e5729d9beb9e8bf8be9c2a3c32a891`,
  je 53068 Bytes. (`EMOVL10_S1.BIN` ist eine ANDERE Datei, sha1 `71d0db3c...`.)

### B. WIDERLEGT

**B1 — `Sce_em_set` (0x44) ist NICHT der einzige Schreiber von `+0x10E` ausserhalb des Overlays.**
Zwei weitere Schreiber in `PSX.EXE`, beide ueber die SCD-Opcode-Tabelle erreichbar:

* **SCD-Opcode `0x8E` @`0x80057714`** (Tabelleneintrag `0x800A7700`, Index
  `(0x800A7700-0x800A74C8)/4 = 0x8E`) — ein Klon von `Sce_em_set`, Satzlaenge **24** Byte
  (`80057d78: addiu v1,v1,24`), schreibt dieselben `pc[4..5]`:
  `800579d8: lw v1,23528(v1) / 800579e0: lhu v0,4(v1) / 800579e8: sh v0,270(s0)`.
  (In den 250 RE2-RDTs findet ein Scan keinen `0x8E`-Satz mit `0x4xxx` — der Pfad ist
  vorhanden, aber ungenutzt. Die Behauptung "der EINZIGE Schreiber" bleibt trotzdem falsch.)
* **SCD-Opcode `0x42` @`0x80056F9C`** (`0x800A75D0`) — `lw a1,340(a0)` holt die Entity, dann
  `80056fb8: lhu v0,270(a1) / 80056fc0: andi v0,v0,0xfff / 80056fc4: sh v0,270(a1)` und
  `80056fc8: lhu v0,270(a1) / 80056fd0: andi v0,v0,0xafff / 80056fd4: sh v0,270(a1)` —
  `0xafff` **loescht Bit 0x4000**. Zusaetzlich `80056fb4: sw v1(=1),4(a1)` -> Zustand ACTIVE,
  Substate 0. Opcode 0x42 ist also der **Ent**anker-Befehl.
* Im Overlay selbst wird das Bit ebenfalls geloescht: `80104f0c: andi v1,v1,0xbfff /
  80104f10: sh v1,270(s0)` (Substate-15-Phase-B `0x80104E54`, das direkt danach
  `80104f24: jal 0x800152c8` = Fortbewegung aufruft).

**B2 — Substate 3 wird an ACHT Stellen gesetzt, nicht an sechs.**
Exhaustiver Scan aller `sw rt,4(rs)` mit vorangehendem `addiu rt,zero,0x0301`:
`80101958, 8010199c, 80102178, 801021bc, 801046d8, 8010471c` (die sechs des Dossiers)
**plus `80105a10` und `80105a4c`**. Letztere liegen in `0x80105438` — und das ist kein
Fortbewegungs-Substate, sondern ein **Schadens-Reaktions**-Handler.

**B3 — Der ortsgebundene Zweig HAT einen Pfad zum Greifen: ueber den Schadenszustand.**
Der `0x4000`-Test existiert im gesamten Overlay an genau drei Stellen (`andi ...,0x4000`-Scan):
`80100514` (Treiber, setzt nur `+0x1C0|=2`), `80103824` (Sub 7), `80103c9c` (Sub 8).
**In keinem Schadens-Handler.** Der Schadenszustand (`+0x4 = 2` -> `0x80104F40`) dispatcht ueber
eine 2-D-Tabelle:

```
801053e0: lbu v1,5(a0)              ; Substate
801053e4: lui  a2,0x8011
801053e8: addiu a2,a2,-14016        ; = 0x8010C940
801053ec: sll  v0,v1,3
801053f0: addu v0,v0,v1             ; v0 = 9*Substate
801053f4: sll  v0,v0,2
801053f8: lbu  v1,466(a0)           ; +0x1D2 (Trefferart)
801053fc: addu v0,v0,a2
80105400: sll  v1,v1,2
80105404: addu v1,v1,v0
80105408: lw   v0,0(v1)
80105410: jalr v0
```

-> `handler = *(0x8010C940 + 36*substate + 4*(+0x1D2))`. Damit:

* **Substate 7** (Zeile `0x8010CA3C`): `80107438, 801066fc, 0, 80107438, 80105bc0, 0,
  80107438, 80105438, 0` — bei `+0x1D2 == 7` also **`0x80105438`**, die Funktion mit den
  Grab-Sprungstellen `80105a10`/`80105a4c`.
* **Substate 8** (Zeile `0x8010CA60`): `80107438, 801066fc, 0, 80107438, 80105bc0, 0,
  80107438, 80105bc0, 0`.

Und alle diese Handler schreiben `+0x4` mit einem **neuen Substate**:
`801060f8: sw 0x0201` (in `0x80105bc0`), `8010784c: sw 0x0201` und `801077e4: sw 0x7`
(in `0x80107438`), `80107408/80107418: sw 0x0101/0x0201` (`0x8010703c`),
`801081c0/80108228: sw 0x0201/0x0b01` (`0x80107ef0`), sowie in `0x80105438`
`80105a10/80105a4c: sw 0x0301` (GRAB), `80105ad4: 0x0b01`, `80105b10: 0x0c01`,
`80105b2c: 0x0201`, `80105b40/80105b50: 0x0101`.
Ein beschossener verankerter Zombie landet also in Substate 1/2/11/12 — und Substate 1/2/12
tragen die vom Dossier selbst dokumentierten Greif-Gates. Die Aussage "sie koennen ihren
Zustand gar nicht wechseln, sobald `+0x10E & 0x4000` gesetzt ist" gilt **nur innerhalb des
ACTIVE-Automaten**. Nur die Varianten 5 und 7 (HP = -1) sind davon ausgenommen — ROOM1130
(`0x4008`) und ROOM1190 (`0x4048`) sind Variante 8 mit **normalen HP**.
Zusatz: `0x80105438` enthaelt ein **drittes/viertes Greif-Gate** — `80105980: sltiu v0,s3,0x514`
(1300), Kegel `80105a00: jal 0x80015758` mit `a3 = 288` (**nicht 256**, also +/-25,3 Grad) und
`80105a38` mit `a2 = yaw-256`. Auch das fehlt im Dossier.

**B4 — "Sub 8 dreht sich mit 16/4096 pro Frame zum Spieler" gilt im verankerten Zustand NICHT.**
`80103dec: jal 0x80015558 / 80103df0: addiu a3,zero,16` liegt in Sub-Substate **7**
(`0x801000B4[7] = 0x80103DD8`), erreicht nur ueber Sub-Substate 6 (`0x80103DB4`, setzt
`+0x6:=7` an `80103dbc`). Ein Scan aller `sb rt,6(rs)` im Bereich `0x80103B74..0x80103E40`
findet die Werte `1, 3, 1, 4, 5, 7, 0` und `+0x6 += ret` — **niemals 6**. Der verankerte
Einstieg ist `+0x4 = 0x0801` -> `+0x6 = 0` -> Fall 0 setzt `+0x6 = 1`; danach haelt der
`0x4000`-Test bei `80103c9c`. Das im Fazit versprochene "Anschauen" ist damit **nicht belegt**.

**B5 — "bleibt fuer immer bei `+0x6=1`" ist fuer Substate 8 ungenau.**
Fall 1 (`0x80103C18`) rechnet `80103c2c-3c: lbu v1,6(s1) / addu v1,v1,v0 / sb v1,6(s1)` mit
`v0` = Rueckgabe von `0x8002959c` (`FUN_80029614` liefert `1` am Clip-Ende). Fall 2
(`0x80103CB0`) setzt `+0x6` wieder auf 1 und den Clip auf `Tab+0x300`. Der verankerte Zombie
zykelt also **1 <-> 2**, er steht nicht auf 1 fest. (Fuer Substate 7 stimmt die Aussage.)

**B6 — "ROOM1130 5x `0x4008`" widerspricht der eigenen Beleglage.**
Das Dossier listet in seinem eigenen Adress-Eintrag **acht** Offsets
(`0x001212, 0x001228, 0x00123e, 0x00139a, 0x0013b0, 0x0013c6, 0x0013dc, 0x0013f2`); ein
unabhaengiger Scan aller 250 RDTs findet **neun** `0x44`-Saetze mit `0x4008` in ROOM1130.
Die Prosa ("fuenf verankerten Zombies") ist durch nichts gedeckt.

### C. UNBELEGT / ZU WEIT INTERPRETIERT (nicht widerlegt, aber nicht bewiesen)

* **`0x801055E4` = "kein Koerperteil-/Kopf-Schaden, Zerstueckelung wird uebersprungen".**
  Die Instruktionen stimmen (`801055dc lhu +0x10E / 801055e4 andi 0x40 / 801055e8 bne
  -> 0x80105624`), aber uebersprungen wird nachweislich nur der **Nachlade-Zweig** des
  Trefferzaehlers: `801055f0 bgtz` (Zaehler noch > 0?), `801055fc/80105604 lh +0x156 / slti 81`
  (HP >= 81?), `80105610-20 rand&0xf + 16 -> +0x223`. Der Zaehler-Abzug `sb a0,547(s1)` steht
  im **Delay-Slot** von `801055e8` und laeuft immer. Das Etikett "Zerstueckelung" ist eine
  Deutung ohne Beleg.
* **Greif-Gate unvollstaendig spezifiziert.** Gate 1 verlangt zusaetzlich `+0x21A & 0x20 == 0`
  (`80101928-34`); Gate 2 verlangt zusaetzlich `+0x6 == 3` (`8010462c`) und Spielertyp
  `!= 0x0F` (`8010468c-9c: lbu 0x800CFC00 / xori 0xf`). Wer daraus eine Nachruestung baut,
  bekommt sonst ein zu grosszuegiges Gate.
* **"Tab1 hat 14 Eintraege"** ist eine Layout-Ableitung, keine Code-Schranke: `8010115c:
  andi v0,v0,0x3f` hat **keinen** Bound-Check, Index 14..51 laeuft in Tab2..Tab5 hinein.
  Fuer alle in RE2-RDTs belegten `+0x10E`-Werte (`&0x3f` = 2/4/8/10) ist das folgenlos.

