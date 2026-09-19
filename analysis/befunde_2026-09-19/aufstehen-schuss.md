# BEFUND aufstehen-schuss — Treffer WAEHREND des Boden-Aufstehers (RE2-Zombie)

Nutzer 2026-09-19: „Wenn die Zombies im Prozess sind wieder aufzustehen, laesst man sie mit
Schuss wieder hin fallen, und dann probieren sie wieder aufzustehen usw. Das ist im Original
Resident Evil 2 anders — dort reagieren sie zwar auf Schuesse, stehen aber weiter auf."

Quellen dieser Runde (alles selbst disassembliert, nicht aus Port-Kommentaren abgeschrieben):
`info/re2leon/COMMON/BIN/EMZ0.BIN` (RAW @0x80100000) via
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`, `info/re2leon/PSX.EXE` (Hitscan
FUN_800410CC, AoE-Applier FUN_800470C0), Tabelle @0x8010C940 eigener Dump. Port:
`re15_port/engine/src/enemy_ai_re2_zombie.c`, `re15_port/engine/src/re15_damage.c`.
Sonde: `re15_port/tests/unit/probe_r16_aufstehen_schuss.c`
(CMake `re15_port/tests/unit/probes/r16_aufstehen-schuss.cmake`, Build
`re15_port/build_r16_aufstehen-schuss`).

---

## 0. Kurzfassung

Der Port schickt jeden Treffer waehrend des Boden-Aufstehers (EXEC[5] P6/P7, Clip 8/9) ueber die
Liege-Route `0x60501` zurueck in P6 — der Aufsteh-Clip startet bei Bild 0, der Zombie sinkt
sichtbar in die Liegepose zurueck und steht erneut auf (gemessen: 3 Treffer = 3 Neustarts).
Das Original prueft in der HURT-Wurzel VOR Flinch-Tor und Liege-Route `+0x21A & 0x10`
(@0x80105014-1C, gesetzt in EXEC[5]-P6 @0x80103588 und EXEC[8]-P3 @0x80103D00) und ruft dann
FUN_80107A78: Blut, Oberkoerper-Twist ueber die Part-Matrizen, der laufende Clip wird NICHT
gewechselt (nur `+0x14E = 15` @0x80107B3C), er laeuft in P1..P3 zu Ende, und der Exit
@0x80107E70-ECC stellt das in +0x22C gesicherte Zustandswort (Schnappschuss +0x228 vom
Tick-Ende @0x8010061C-28) mit `+0x6 += 1` wieder her — also P7+1 = P8 = `0x101`, er steht.
Der Port muss FUN_80107A78 als eigenen Handler samt +0x228/+0x22C-Schnappschuss nachbauen und
in `re2z_hurt` zwischen Kriecher-Zweig und Flinch-Tor einhaengen; derselbe Weg repariert den
Fresser-Aufsteher (EXEC[8] P3/P4), bei dem der Port heute den Haupt-Treffer spielt und das
Bit 0x10 klebrig stehen laesst.

---

## 1. Reproduktion/Messung

Sonde `probe_r16_aufstehen_schuss` (ROOM1140.RDT, RE2-Flavor, geladene RE2-Baenke EM010/EM011/
EM016 aus `shared_assets/RE2/CDEMD0.EMS`, echter Weg `scd_vm_tick + re15_game_step`, Pistole
= RE1.5-Waffe 3 -> RE2-Zeile 3, Schuss ueber Pad R1/SQUARE). Zombie slot 2 (Typ 0x10, Fresser
grid 0x86), RE2-INIT laeuft 3 Bilder, dann Zustandswort `0x501` (= der Flinch-Commit
@0x801050A4-AC) -> EXEC[5]. Ausgabe: `probe_out_full.txt` (Scratchpad), Auszug:

### 1.1 Sturzkette bis zum Aufsteher (Teil A, Zustandswechsel)

```
f0    st=1/5/1 mo=0x01 fr=10 | 21A=0x0202 1D3=0x80 223=19        <- P0 done: Clip 1 ab Bild 10, 21A|=0x202, 1D3|=0x80
f49   st=1/5/2 mo=0x01 fr=59
f50   st=1/5/3 mo=0x17 fr=0  | 21A=0x0202 1D3=0x00               <- P2: Liege-Clip 0x17, 1D3 &= 0x7f (treffbar)
f149  st=1/5/6 mo=0x17 fr=9  | 21A=0x0202                        <- Liegezeit abgelaufen
f149  st=1/5/7 mo=0x08 fr=0  | 21A=0x0012                        <- P6: Clip 8, 21A |= 0x10, &= ~0x200
```

### 1.2 Treffer WAEHREND des Aufstehens (Teil B, P7, Clip 8 ab Bild 12; drei Schuesse im Abstand 24)

```
PRE  f161 st=1/5/7 mo=0x08 fr=12 | 21A=0x0012 1D3=0x00 223=19 222=0 16B=0 1D2=0 | hp=200
RST6 f161 st=1/5/6 mo=0x08 fr=13 | 21A=0x0012 1D3=0x03 223=19 222=0 16B=1 1D2=0 | hp=184 react=0x81 hnd=-
     f162 st=1/5/7 mo=0x08 fr=0  |                                                <- Clip 8 startet NEU bei Bild 0
     f163 st=1/5/7 mo=0x08 fr=1
     ...
RST6 f185 st=1/5/6 mo=0x08 fr=23 | hp=168 16B=1                                  <- zweiter Treffer: wieder P6
     f186 st=1/5/7 mo=0x08 fr=0
RST6 f209 st=1/5/6 mo=0x08 fr=23 | hp=152                                        <- dritter Treffer: wieder P6
UP!! f290 st=1/1/0 mo=0x08 fr=79                                                 <- erst 81 Bilder nach dem letzten Treffer steht er
SUMME: 3 Treffer, Aufsteher-Neustarts P7->P6 = 3, Neu-Stuerze ->P0 = 0, aufgestanden (5->1) = 1
```

Identisch fuer Zielen TIEF (D-Pad unten) und fuer Treffer ab Bild 40 des Clips (3/3 Neustarts).
`hnd=-` = KEINE Zelle der 2D-Tabelle wurde dispatcht: der Treffer nimmt die Liege-Route
`0x60501` (Zustandswort 1/5/6), P6 setzt Clip 8 mit Startbild 0 neu (`re2z_clip(..., 0, 0xF,
0x100, 0)` enemy_ai_re2_zombie.c:2138) und stempelt `+0x16B = 1` (Blut). Der Aufsteher faengt
jedes Mal bei der Liegepose an — das ist das „laesst man sie wieder hinfallen" des Nutzers.
Die Flinch-Schwelle (`223=19` > 0) verhindert den 0x501-Neusturz; der 0x60501-Weg ist der
gemessene Taeter. Zone: `1D2=0` in allen Faellen (Port-Regel `liegt = +0x21A & 2`, s. §3).

### 1.3 Treffer im LIEGEN (Teil C, P3, Clip 0x17) — Vergleich

```
Zielen EBEN: 0 Treffer (Elevations-Band verwirft den Liegenden; eigenes Thema liegende-zombies)
Zielen TIEF: HIT! f72 st=1/5/6 mo=0x17 fr=9 | 21A=0x0202 16B=1 hp=184   <- Liege-Route 0x60501 -> P6: steht sofort auf
             RST6 f96 st=1/5/6 mo=0x08 fr=23 | hp=168                    <- Treffer waehrend des Aufstehens: Neustart
```

Der Liege-Treffer -> sofortiges Aufstehen mit Blut ist byte-true (Original @0x80105168-8C,
§2.4). Der zweite Treffer (Aufsteher) zeigt denselben Defekt wie 1.2.

### 1.4 Kontrolle stehend (Teil D)

```
HIT! f12  st=2/3/1 mo=0x03 | 223=0 222=1 16B=26 1D2=1 hnd=MAIN5438     <- Haupt-Treffer
FALL f36  st=1/5/0 | 223=24 222=1                                        <- zweiter Treffer: Flinch 0x501 (res<=0, 222==1)
RST6 f156 / RST6 f180 (Treffer im Aufsteher: Neustarts)   UP!! f261
```

### 1.5 Fresser-Aufsteher EXEC[8] P3/P4 (Teil E) — dasselbe Bit, anderer Fehlausgang

```
PRE  f15 st=1/8/4 mo=0x15 fr=11 | 21A=0x0010 223=19
HIT! f15 st=2/3/1 mo=0x00 fr=0  | 21A=0x0010 223=4 16B=24 hnd=MAIN5438  <- Haupt-Treffer statt FUN_80107A78
HIT! f39 st=2/3/1              | 21A=0x0010 223=-11
Ende f182 st=1/3/3 mo=0x0C     | 21A=0x0010                              <- Bit 0x10 bleibt fuer immer stehen
```

Der Aufsteh-Clip 0x15 wird abgebrochen (Clip 0 des Haupt-Treffers), und `+0x21A & 0x10` bleibt
klebrig (kein Loescher ausser EXEC[8]-P4-Clip-Ende @0x80103D84-8C und dem Exit von
FUN_80107A78 @0x80107EA0-A4). Folge im Port: der spaetere Tod dieses Zombies nimmt DEATH-Zweig 2
(`re2z_death_lying`, enemy_ai_re2_zombie.c:7169) statt des Stand-Tods — ein Folgedefekt
derselben Luecke (nicht separat gemessen, Ableitung aus dem stehenden Bit).

---

## 2. Original-Mechanismus

### 2.1 HURT-Wurzel FUN_80104F40 — die Reihenfolge der Weichen (EMZ0.BIN, Offset = addr − 0x80100000)

```
80104fe0: lhu  a0,270(s1)           ; +0x10E
80104fe8: andi v0,a0,0x1
80104fec: beq  v0,zero,0x80105014   ; Bit 0 (Kriecher) -> 1D-Tabelle 0x8010CBE8[+0x5], j 0x8010540c
80105014: lhu  v0,538(s1)           ; +0x21A
8010501c: andi v0,v0,0x10
80105020: beq  v0,zero,0x8010503c   ; Bit 0x10 NICHT gesetzt -> weiter zum Flinch-Tor
80105024: andi v0,a0,0x40           ; (Delay-Slot: +0x10E & 0x40 fuer das Flinch-Tor)
80105028: addu a0,s1,zero
8010502c: jal  0x80107a78           ; ** AUFSTEHER-TREFFER FUN_80107A78 **
80105030: addu a2,s3,zero
80105034: j    0x80105418           ; Epilog — Flinch-Tor, Liege-Route, Zerleger, 2D-Dispatch laufen NICHT
8010503c: beq  v0,zero,0x8010505c   ; ab hier Flinch-Tor (Phase 0, +0x223 <= thr, +0x222==1 || +0x5==1) -> sw 0x501 @0x801050A4-AC
80105168: lhu  v1,538(s1) / andi 0x2 / beq -> 0x80105288   ; Liege-Route: +0x21A & 2 -> 0x60501 @0x8010517C-84, +0x16B=1 @0x8010518C
801053e0-410: 2D-Dispatch 0x8010C940[+0x5*36 + +0x1D2*4], jalr
```

Der 0x10-Zweig steht VOR Flinch-Tor und Liege-Route. Wer das Bit traegt, erreicht weder `0x501`
noch `0x60501`.

### 2.2 Produzenten/Loescher von +0x21A Bit 0x10 (Vollscan aller `538(` -Stores in EMZ0.BIN)

| Adresse | Instruktion | Bedeutung |
|---|---|---|
| @0x80103578-8C | `lhu v0,538 / ori v0,v0,0x10 / sh v0,538` | EXEC[5] P6 (Knockdown-Aufsteher, Clip 8/9 `sw a0,332` @0x801035A8) |
| @0x80103D00-10 | `ori 0x10 / sh 538` | EXEC[8] P3 (Fresser-Aufsteher Clip 0x15) |
| @0x801036B0-BC | `andi v0,v0,0xffef / sh` | EXEC[5] P7, nur wenn Advance „fertig" (`beq v0,zero,0x801036d0` @0x801036A8) |
| @0x80103D84-8C | `andi 0xffef` | EXEC[8] P4 Clip-Ende |
| @0x80107EA0-A4 | `andi v1,v1,0xffed / sh v1,538` | Exit von FUN_80107A78 (loescht 0x10 UND 0x2) |
| @0x80103F7C-90 | `andi 0xffef / ori 0x8` | EXEC[9] P0 (Stoss-Taumel setzt 0x8, loescht 0x10) |
| @0x8010087C / @0x801049DC | Vollaustrag | INIT |

Waehrend des gesamten Aufstehens (P6 bis P7-Clip-Ende, Clip 8/9 = 80 Bilder) ist Bit 0x10
gesetzt; Bit 0x2 ebenfalls (Loescher erst @0x801036C8-CC). Der Zombie ist dabei TREFFBAR:
`+0x1D3` wurde in P2 auf `& 0x7f` gesetzt (@0x80103484-90), P6/P7 schreiben +0x1D3 nicht; der
Hitscan-Filter verlangt nur `+0x1D3 == 0` (`lbu v0,467(s2) / bne v0,zero` @0x80041270-78),
`+0x10E & 0xC000 == 0` (@0x80041290-9C), HP >= 0, word0 & 1. Kein word0-0x08000000-Gate im
Hitscan (die 0x0C000000-Bits, die P0 @0x801032B8-FC loescht/setzt, sind Teile-Masken der
Zonenrechnung, nicht Treffbarkeit).

### 2.3 FUN_80107A78 — der Aufsteher-Treffer-Handler (0x80107A78..0x80107EEC)

Phasen-Dispatch @0x80107AA4-E8 ueber +0x6: 0 -> P0 @0x80107AEC (faellt in P1 durch), 1 -> P1
@0x80107D00, 2 -> P2 @0x80107DB8, 3 -> P3 @0x80107E58.

**P0 @0x80107AEC-CFC:**
```
80107aec: lhu  v1,464(s1)           ; +0x1D0 Trefferrichtung
80107af0: sb   v0,6(s1)             ; +0x6 = 1
80107af8: sh   zero,344(s1)         ; +0x158 = 0   (Twist-Zaehler)
80107afc: sh   v0,346(s1)           ; +0x15A = 1   (Twist-Vorzeichen)
80107b00: andi v1,v1,0x20
80107b04: beq  v1,zero,0x80107b14
80107b08: sb   zero,362(s1)         ; +0x16A = 0 (Delay-Slot, immer)
80107b0c: addiu v0,zero,-1
80107b10: sh   v0,346(s1)           ; +0x1D0 & 0x20 (Treffer von vorn, Hitscan @0x80041A0C-2C) -> +0x15A = -1
80107b14-30: Blut-FX Id 6096, Offset {0,300,0}, Anker +0x198+1448, a1 = +0x76 (jal 0x8001bf10)
80107b34: lw   v1,552(s1)           ; +0x228 = Zustandswort-Schnappschuss vom Tick-Ende (s. 2.5)
80107b38: addiu v0,zero,15
80107b3c: sb   v0,334(s1)           ; ** +0x14E = 15: NUR das Blend-Feld des Clip-Worts; +0x14C (Clip) und +0x14D (Bild) bleiben **
80107b40: andi v0,v1,0xff
80107b44: bne  v0,s0,0x80107b80     ; (+0x228 & 0xff) != 1 -> +0x22C NICHT ueberschreiben (Re-Hit waehrend Zustand 2)
80107b4c: sw   v1,556(s1)           ; +0x22C = +0x228  (das Rueckkehr-Wort)
80107b48-7c: (+0x228 & 0x00ff0000) == 2 -> jal 0x8002fa00  — unerfuellbar (lui 0xff maskiert Bits 16..23, Vergleich mit 2), toter Zweig
80107b80-bc: HP < 0 -> HP = 0, +0x16A = 1, Blut 0x31F40, +0x14E = 15
80107bc0-dc: SE 12, wenn +0x239 == 0; +0x239 = 150
80107be0-cfc: Zeilen-Sonderfaelle ueber +0x5: 10 (Flammen, +0x1D2 < 3 -> Brand 0x80106128, +0x21A|=0x800), +0x16B = 24 (Standard),
              16 (+0x23A-Zaehler, Brand ab 9, +0x16B = 2), 15/18 (+0x16B = 2), 11 (Saeure 0x80106310), 14 (Funken 0x80106510)
```
Es gibt in P0 KEINEN Store auf +0x14C/+0x14D und KEINEN Store auf +0x4: der Aufsteh-Clip laeuft
ungebrochen weiter.

**P1 @0x80107D00-DB4 (Twist auf, Clip laeuft):**
```
80107d0c: jal  0x8002959c / a3 = 256 ; Advance des LAUFENDEN Clips (8/9 bzw. 0x15)
80107d14: bne  v0,zero,0x80107e70   ; Clip fertig -> EXIT
80107d1c-54: vec.z = -((+0x158 * (s8)+0x16B) << 3) * +0x15A   (`mult/sll 3/subu/mult`, sh t0,28(sp))
80107d50: jal  0x8008e1f4           ; RotMatrix(vec, m)
80107d5c: jal  0x8008d934           ; MulMatrix(part0+24, m)          — Part 0 nach-multipliziert
80107d70: jal  0x8008e1b4           ; TransposeMatrix
80107d7c: jal  0x8008da44           ; MulMatrix2(mT, part1+24)        — Part 1 gegenlaeufig
80107d84-a0: +0x158 += 1; wenn alter Wert < 3 -> return
80107da4-b0: sonst +0x6 = 2, +0x158 = 16
```
Das ist exakt die Bone-Injektion des Haupt-Handlers P1 @0x801057A4-E8 (im Port `re2z_lean_pair`,
enemy_ai_re2_zombie.c:3912, Beleg-Block re15_actor.h:311-327), nur mit Zaehler 0..3.

**P2 @0x80107DB8-E54 (Twist ab):** Advance(256), fertig -> EXIT; sonst vec.z =
−(+0x158 * +0x16B) * +0x15A (OHNE `<<3`, @0x80107DD4-F0), dieselben vier Matrix-Calls, `+0x158 -= 1`
(Delay-Slot-Store @0x80107E48, Test auf ALT != 0), bei ALT == 0 -> +0x6 = 3.

**P3 @0x80107E58-6C:** nur Advance(256); fertig -> EXIT, sonst return.

**EXIT @0x80107E70-ECC:**
```
80107e70: lw   a0,556(s1)           ; +0x22C
80107e78-98: Hitbox 500/500/500/500/-1500/1500 (+0x9A/9C/90/92/98/9E)
80107ea0: andi v1,v1,0xffed / sh v1,538   ; +0x21A &= ~0x12  (Bit 0x10 UND Bit 0x2 weg)
80107ea8-b0: word0 |= 0x0C000000
80107eb8: sw   a0,4(s1)             ; ** Zustandswort = +0x22C **  (z.B. 0x00070501)
80107ebc: lbu  v1,6(s1)             ; liest +0x6 des WIEDERHERGESTELLTEN Worts (= 7)
80107ec0: andi v0,v0,0xdfff / sh 270  ; +0x10E &= ~0x2000
80107ec4: addiu v1,v1,1
80107ecc: sb   v1,6(s1)             ; +0x6 = 8  -> EXEC[5] P8 -> `sw 0x101` @0x801036F4-F8: er STEHT
```
Fuer den Fresser-Aufsteher: +0x22C = 0x00040801 -> +0x6 = 5 -> EXEC[8] P5 `0x101` @0x80103D90-94.

### 2.4 Liege-Route (zum Vergleich, byte-true im Port)

@0x80105168-8C: `+0x21A & 2` -> `lui v1,0x6 / ori 0x501 / sw v1,4` = `0x60501`, `sb 1,363` (+0x16B = 1)
-> EXEC[5] P6 @0x80103568: Clip 8/9 mit Startbild 0 (`sw a0,332` @0x801035A8, a0 = 0x000F0008/9),
Blut 8000 nur bei +0x16B == 1 (@0x801035B0-DC). Diese Route ist NUR fuer den Liegenden (P2..P5,
Bit 0x10 noch nicht gesetzt) erreichbar.

### 2.5 Der Schnappschuss +0x228 (Per-Frame-Wurzel des Zombies, 0x80100xxx)

```
801004e0: lw   v0,-14288(at)        ; 0x8010C830[+0x4] — Zustands-Dispatch
801004e8: jalr v0
...
8010061c: lw   v0,4(s0)             ; Zustandswort NACH dem Dispatch dieses Ticks
80100624: jal  0x80016028
80100628: sw   v0,552(s0)           ; +0x228 = Zustandswort (Delay-Slot) — am TICK-ENDE
```
Der Hitscan stempelt `+0x4 = 2` erst danach (Spieler-Tick, `sw v0,4(t0)` @0x800418EC), der
naechste Zombie-Tick sieht in +0x228 das Wort VOR dem Treffer (0x00070501). Ein zweiter Treffer,
waehrend FUN_80107A78 laeuft (Bit 0x10 ist bis zum Exit gesetzt), findet +0x228 = 0x0001000x
(Zustand 2) vor: `(+0x228 & 0xff) != 1` -> +0x22C bleibt 0x00070501 — deshalb die Pruefung
@0x80107B40-44. Einziger Leser von +0x22C im ganzen Overlay ist der Exit @0x80107E70 (eigener
Vollscan `556(`).

### 2.6 Trefferpause dazwischen

Hitscan-Stempel `+0x1D3 = (alt & 0x80) | ((Schadenszeile+4 >> (9 + 7*Klammer)) & 0x7f)`
@0x80041ABC-AE8; Dekrement der Low-7 im Zombie-Tick @0x80100484-98
(`andi 0x7f / beq zero / addiu -1 / sb 467`). Der Port fuehrt das bereits
(`re15_re2_stun_frames`, re15_damage.c:1738; gemessen `1D3=0x03` nach jedem Pistolentreffer).

---

## 3. Port-Ist

**`re2z_hurt` (enemy_ai_re2_zombie.c:6553-6663):** Stempel-Zwilling -> `re2z_grab_abort` ->
Kriecher-Zweig (`f10e & 1`) -> **hier fehlt der 0x10-Zweig** (Kommentar :6575-6577: „OPEN
(unveraendert): +0x21A & 0x10 -> Kriecher-Umbau FUN_80107A78 … ohne Produzenten fuer +0x21A Bit
0x10 ist er im Port unerreichbar") -> Flinch-Tor (:6584-6597) -> Liege-Route (:6601-6624) ->
Zerleger -> 2D-Dispatch. Die Kommentar-Begruendung ist falsch: der Port SETZT das Bit selbst in
EXEC[5]-P6 (:2132 `re2z_flags21a |= 0x10u`, @0x8010358C) und EXEC[8]-P3 (:2554, @0x80103D00-10);
die Bezeichnung „Kriecher-Umbau" ist eine unbelegte Deutung — FUN_80107A78 ist der
Aufsteher-Treffer (§2.3).

**Was der Treffer im Port deshalb tut:**
- Knockdown-Aufsteher (Bit 0x2 noch gesetzt, Loescher erst P7-Clip-Ende :2165): Flinch-Tor faellt
  durch (`res223 = 19 > 0`), Liege-Route greift -> `0x60501` (:6602), `gaitrow = 1` (:6603) ->
  `re2z_exec_knockdown` case 6 (:2128-2158): `re2z_clip(e, param_clips[4+side], 0, 0xF, 0x100, 0)`
  = Clip 8/9 **Startbild 0** -> die gemessene Neustart-Schleife (§1.2). Bei erschoepftem
  +0x223 (nach mehreren Haupt-Treffern) wuerde stattdessen das Flinch-Tor `0x501` (:6588)
  zuenden — ein echter Neusturz aus der Liegepose (in Teil D nicht erreicht, weil 0x501 selbst
  +0x223 auf 16..31 setzt).
- Fresser-Aufsteher (nur Bit 0x10, kein Bit 0x2): Flinch-Tor faellt durch, Liege-Route nicht,
  2D-Dispatch -> `re2z_hit_main` (:4910), Clip 0 ersetzt den Aufsteh-Clip 0x15, Bit 0x10 bleibt
  fuer immer stehen (§1.5).

**Stempel `re15_re2_stamp_hit` (re15_damage.c:1961-2007):** Spalte
`hits1d2 = (liegt || elev < 0) ? 0 : 1` mit `liegt = (f10e & 1) || (flags21a & 2)` (:1998-1999)
-> waehrend des Knockdown-Aufstehers immer Spalte 0 (BEINE), waehrend des Fresser-Aufstehers 1.
Fuer den Fix ist die Spalte irrelevant: FUN_80107A78 liest +0x1D2 nur in der Zeile-10-Sonderregel
(`sltiu +0x1D2 < 3` @0x80107C04-10). Die Zeile `+0x5` (Pistole = 3) fuehrt in FUN_80107A78 nur
zu `+0x16B = 24` (@0x80107BEC/C38), ohne Sonderfall.

**Fehlende Felder:** Der Port hat kein +0x228/+0x22C (re15_actor.h kennt nur `re2d_offx228` des
Hundes mit anderer Bedeutung); `re2z_prev_sub` (:8005) ist nur der +0x5-Schnappschuss.
Vorhanden und wiederverwendbar: `re2z_hitdir1d0` (+0x1D0, Bit 0x20 = von vorn), `re2_lean[3]` +
`re2z_lean_pair` (:3912, die Bone-Injektion; pro Tick geloescht :7786-7787), `re2z_gore_fx_ex`,
`re2z_se`, `re2z_cd239`, `re2z_gore_burn/acid/spark`, `re2z_burn23a`, `re2z_gaitrow` (+0x16B),
`re2z_t158/t15a/dir16a`.

**Tick-Ende:** `re15_re2z_tick` (:7772-8159) — der Dispatch `switch (e->state)` (:7998-8013), danach
`re15_re2z_lower_body_tick` und `re15_re2z_hit_filter_apply` (:8158). Ein Schnappschuss des
Zustandsworts am Tick-Ende existiert nicht.

---

## 4. Fix-Plan (Phase 2)

Datei: `re15_port/engine/src/enemy_ai_re2_zombie.c`, Header `re15_port/include/re15_actor.h`.

1. **Felder** (re15_actor.h, RE2-Zombie-Block): `uint32_t re2z_word228;` (+0x228
   Zustandswort-Schnappschuss, Produzent @0x8010061C-28) und `uint32_t re2z_word22c;` (+0x22C
   Rueckkehr-Wort, Produzent @0x80107B4C, Konsument @0x80107E70). Init 0 (INIT-Vollaustrag
   @0x8010087C deckt die +0x21A-Bits; +0x228/+0x22C werden im Original nicht explizit genullt —
   `re15_actor_init` nullt die Struktur, das genuegt).

2. **Schnappschuss** in `re15_re2z_tick` direkt nach dem `switch (e->state)` (:8013, vor
   `re15_re2z_lower_body_tick`): `e->re2z_word228 = ((uint32_t)e->sub_state_2 << 16) |
   ((uint32_t)e->sub_state_1 << 8) | e->state;` — Byte-Layout wie `re15_ai_set_state_word`
   (+0x4 = state, +0x5 = sub1, +0x6 = sub2). Beleg `lw v0,4(s0) / sw v0,552(s0)` @0x8010061C-28.
   Reihenfolge-Sicherheit: der Port stempelt den Treffer im selben `game_step` VOR dem Zombie-Tick
   (gemessen §1.2: HIT und Zustandswechsel im selben Bild) — der Schnappschuss des Vor-Ticks ist
   dann wie im Original das Wort vor dem Treffer.

3. **Neuer Handler `re2z_getup_hurt(e, pl)`** (= FUN_80107A78) mit den vier Phasen aus §2.3:
   - P0 (`sub_state_2 == 0`): `sub_state_2 = 1`; `t158 = 0`; `t15a = (hitdir1d0 & 0x20) ? -1 : 1`
     (@0x80107AF8-B10); `dir16a = 0`; Blut `re2z_gore_fx_ex(e, part, 6096, rot_y, 0, 300, 0)`
     (@0x80107B14-30; Anker +0x198+1448 = derselbe Teil, den der Flinch-Blutzweig
     `addiu a2,a2,244` NICHT nutzt — im Port ueber den vorhandenen Positions-Zwilling
     der 1448-Anker, sonst Part 0 mit Vermerk); `anim_frac = 15` OHNE `re2z_clip()` (@0x80107B3C —
     Clip und Bild bleiben); `if ((word228 & 0xff) == 1) word22c = word228;` (@0x80107B40-4C);
     HP<0-Zweig (@0x80107B80-BC) mitnehmen (HP = 0, dir16a = 1, Blut 8000|0x30000, frac 15);
     SE 12 mit `cd239`-Gate (@0x80107BC0-DC); Zeilen-Sonderfaelle (@0x80107BE0-CFC): Default
     `gaitrow = 24`; Zeile 10: Brand wenn `!(f10e & 0x80) && hits1d2 < 3` + `flags21a |= 0x800`;
     Zeile 16: Brand ab `burn23a >= 9`, `gaitrow = 2`, `burn23a++`; Zeilen 15/18: `gaitrow = 2`;
     Zeile 11: `re2z_gore_acid` wenn `hits1d2 < 3`; Zeile 14: `re2z_gore_spark`. Dann FALLTHRU P1.
   - P1: `done = re2z_clip_done(e)` (Advance des laufenden Clips, a3 = 256 @0x80107D10) -> bei
     done EXIT; sonst `re2z_lean_pair(e, 0, 0, -(((t158 * (int8_t)gaitrow) << 3) * t15a))`
     (@0x80107D1C-80, dieselbe Injektion wie Haupt-P1 :3912ff); `t158++`, war alt >= 3 ->
     `sub_state_2 = 2; t158 = 16` (@0x80107D84-B0).
   - P2: Advance; done -> EXIT; sonst `re2z_lean_pair(e, 0, 0, -((t158 * (int8_t)gaitrow) * t15a))`
     (@0x80107DD4-F0, ohne `<<3`); Delay-Slot-Dekrement: `alt = t158; t158 = alt-1; if (alt == 0)
     sub_state_2 = 3` (@0x80107E38-54).
   - P3: Advance; done -> EXIT (@0x80107E58-6C).
   - EXIT (@0x80107E70-ECC): `flags21a &= ~0x12`; `re15_ai_set_state_word(e, word22c)`;
     `sub_state_2++` (liest das wiederhergestellte +0x6); `f10e &= ~0x2000`; word0-Bits
     0x0C000000 und Hitbox-Reset sind Praesentation/Kollision ohne Port-Zwilling (Vermerk).
     Wichtig: KEIN `re2z_hit_latch_release`-Ersatz erfinden — der Original-Exit schreibt +0x1D3
     nicht; P8 von EXEC[5] (`andi 0x7f` @0x80103718-28) bzw. P5 von EXEC[8] (@0x80103D98)
     erledigen das im Folgetick, genau wie im Port heute (:2200, :2574).
   - Der Advance in P1..P3 muss den laufenden Clip 8/9 bzw. 0x15 mit Rate 256 weiterzaehlen —
     im Port ist das der normale `anim_frame`-Vortrieb des Renderers/Advance (gleiche Mechanik,
     mit der EXEC[5] P7 `re2z_clip_done` fuehrt, :2162).

4. **Einhaengen in `re2z_hurt`** (:6575, zwischen Kriecher-Zweig und Flinch-Tor):
   `if (e->re2z_flags21a & 0x10u) { re2z_getup_hurt(e, pl); return; }` (@0x80105014-38, `j
   0x80105418` = Epilog, kein Dispatch). Den irrefuehrenden OPEN-Kommentar :6575-6577 durch die
   Belege aus §2.2/2.3 ersetzen.

5. **Zonen-Stempel bleibt** (re15_damage.c:1998-1999) — fuer diesen Handler ohne Wirkung; die
   Spalte 0 waehrend des Aufstehers ist ohnehin nur eine Port-Bruecke (Kommentar :1978-1986).

6. **Reihenfolge der Umsetzung:** (1) Felder, (2) Schnappschuss, (3) Handler ohne Twist
   (Zustandslogik), Pin gruen, (4) Twist ueber `re2z_lean_pair`, (5) Kommentar-Korrektur. Kein
   Eingriff in EXEC[5]/EXEC[8]/Liege-Route/Flinch-Tor — die sind byte-true.

7. **Risiken:**
   - Zweiter Treffer waehrend `re2z_getup_hurt` laeuft: Stempel nullt +0x6 -> P0 erneut; +0x22C
     bleibt durch die `& 0xff == 1`-Pruefung erhalten (§2.5). Pin dafuer vorsehen.
   - Toedlicher Treffer im Aufsteher: Hitscan stempelt Zustand 3 -> `re2z_death` Zweig 2
     (`flags21a & 0x10` -> `re2z_death_lying`, :7169) — unveraendert, aber nun ohne das klebrige
     Bit aus §1.5 (das Bit ist beim Fresser nur noch waehrend des echten Aufstehens gesetzt).
   - `re2z_lean_pair` wird zu Tick-Beginn geloescht (:7786-7787) — die Injektion muss in JEDEM
     P1/P2-Tick neu bestellt werden (so wie Haupt-P1), sonst kein sichtbarer Twist.
   - `word22c == 0` (kein gueltiger Schnappschuss, z.B. Zombie direkt in Zustand 2 gesetzt):
     Original wuerde `sw 0 -> +0x4` = Zustand 0 = INIT schreiben. Port: gleich verhalten
     (INIT-Wiederholung ist im Original beobachtbar), nicht kaschieren; im Pin ausschliessen.

8. **Absicherung:**
   - Sonde `probe_r16_aufstehen_schuss` erneut: Erwartung Teil B `Aufsteher-Neustarts = 0`,
     `Neu-Stuerze = 0`, `aufgestanden = 1`, Clip 8 laeuft von Bild 12 ohne Ruecksprung auf 0 bis
     Bild 79 durch, `hnd=-`, nach dem Treffer `st=2/3/1..3` mit `mo=0x08` (Clip unveraendert),
     dann `st=1/5/8 -> 1/1/0` innerhalb von zwei Bildern nach Clip-Ende; Teil E `21A` verliert
     Bit 0x10 spaetestens am Clip-Ende von 0x15, Ende NICHT im Haupt-Treffer.
   - Neuer ctest-Pin (z.B. `test_re2z_getup_hurt.c`): Direktaufruf-Szenario Zustand 1/5/7 mit
     Clip 8 Bild 12, Stempel (Zeile 3, +0x6 = 0, hitdir 0x20), 100 Ticks: Invarianten
     `motion == 8` durchgehend, `anim_frame` monoton, Ende `state == 1 && sub_state_1 == 1`,
     `flags21a & 0x12 == 0`, `f10e & 0x2000 == 0`; zweiter Stempel bei Bild 30 -> `word22c`
     unveraendert 0x00070501; Fresser-Variante 1/8/4 Clip 0x15 -> Ende ueber P5.
   - Bestehende Pins, die den 0x60501-Weg messen (Liege-Treffer -> Aufstehen, Teil C bleibt
     byte-true) und `test_re2_gore` (0-RNG-Wuerfe der stummen Zerleger-Leiter) muessen gruen
     bleiben; 224/224 via `local_build.sh`.

---

## 5. Offen / nicht belegt

- Anker-Teil des P0-Bluts (+0x198+1448, @0x80107B30) und des Flinch-Bluts (+0x198+244) sind
  Modell-Pool-Offsets; welcher Part-Index das ist, wurde nicht aufgeloest (Port-FX ist ohnehin
  positionslos/Part-basiert). Kein Einfluss auf den Zustandsdefekt.
- `jal 0x8002fa00` @0x80107B78 ist im Original unerreichbar (§2.3, Maske 0x00ff0000 gegen 2); die
  Absicht des Compilats (vermutlich „Phase == 2") ist nicht rekonstruierbar — nicht portieren.
- `jal 0x80016028` @0x80100624 (Tick-Ende, neben dem Schnappschuss) und `0x8010bcd0` @0x80100614
  wurden nicht identifiziert; fuer den Schnappschuss-Zeitpunkt reicht die Position nach dem
  Dispatch @0x801004E8.
- Der Liegende ist im Port mit ebenem Zielen nicht treffbar (§1.3, 0 Treffer) — eigenes Thema
  (liegende-zombies), hier nicht untersucht.
- Nicht dynamisch gegen einen RE2-Savestate verifiziert (kein RE2-DuckStation-Lauf in dieser
  Runde); die Kette ist statisch vollstaendig belegt (Weiche @0x80105014-38, Bit-Produzenten,
  Handler, Schnappschuss, einziger +0x22C-Leser).
