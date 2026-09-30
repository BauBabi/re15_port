# Gegenpruefung — re_gegner_re2_familie.md (Runde 34, Granaten)

Stand: 2026-09-29, ABGESCHLOSSEN (fortlaufend gespeichert; keine Laufzeitmessung, nur statisch).
Pruefer: Skeptiker-Agent. Methode: jede Zeile der Tabelle "KONSTANTEN FUER DEN BAU" und jede
Mechanismus-Behauptung, die in Code wandern soll, selbst disassembliert (`re2_disasm.py` fuer RE2,
`re15_disasm.py` fuer RE1.5), Bytes selbst gelesen, Offsets NIE selbst gerechnet, Sprungziele selbst
disassembliert. Decompilate nur als Lesehilfe. Ziel: WIDERLEGEN.
Werkzeuge: `analysis/befunde_runde34_granaten/re_gegner_re2_gegen_werkzeug/`,
Ausgaben: `build/r34g_gegner_re2_gegen/` (untracked).
Keine Aenderung unter re15_port/, kein Build, keine git-Schreiboperation.

Urteile: **bestaetigt** / **widerlegt** (mit Korrektur) / **unklar** (mit dem, was fehlt).

## Gliederung

1. Zustellung: Flug (Op 0x0F), Explosion (Op 47), Brand (Op 48), Saeure (Op 49), Nachbrenner (Op 40/19)
2. Applier FUN_800470C0: Liste, Gates, Band, Radius, Schaden, Zustand, Zone, Zeile, Sperre, Richtung
3. Boxtest FUN_80041EF8 (Rechteck-Geometrie)
4. Schadens-Records 0x800A6A88
5. Zombie (EMZ0.BIN): HURT/DEATH-Tabellen, Element-Leiter, DoT
6. Hund (EMD0G_MOD0.BIN), Kraehe, Spinne, Zellenarm
7. RE1.5-Nebenbefund DAT_8006F430 / Weg A
8. Port-Abgleich (Datei:Zeile)
9. Vollstaendigkeit — was der Bau braucht und im Dossier fehlt
10. Urteil

---

## 0. Kernbefunde (vorab, Einzelheiten unten)

* **W1 WIDERLEGT — Brand/Saeure-Box haengt NICHT am Status-Bit 0x80.** Op 48 und Op 49 ueberschreiben
  den Slot-Status (+0x18 := 0x8000 bzw. 0x8403) unmittelbar VOR dem Test `+0x18 & 0x80`. Der Test ist
  immer 0, der ±600-Zweig ist toter Code. Brand- und Saeure-Stoss pruefen IMMER mit {-1200,0,600,300};
  die Bodenflammen entstehen nach JEDEM Brand-Aufschlag. Konstante GL_BOX_GEGNER streichen (§1.3/§1.4).
* **W2 WIDERLEGT — die Element-Leiter im Liegend-HURT des Zombies (@0x80105188-208) ist tot.** Der Zweig
  schreibt `sw 0x00060501,4(s1)` @0x80105184 (+0x5 := 5) direkt vor `lbu v1,5(s1)` @0x80105188. Ein
  liegender Zombie bekommt bei einem ueberlebten GL-Treffer weder Verkohlung noch Aetzung noch den DoT.
  Der Port bildet das bereits richtig ab (`enemy_ai_re2_zombie.c:7052-7063`); das Dossier beschreibt es falsch (§5.2).
* **W3 WIDERLEGT — Hund HURT Zeile 9/10 "so oft / x(4−K) FX 8".** Die Schleife vergleicht den hochgezaehlten
  Zaehler mit dem vom Spawner heruntergezaehlten Budget -> ceil(n/2) Wuerfe. Der Port wirft n (neue
  Port-Abweichung, RNG-Strom, `enemy_ai_re2_dog.c:2053-2066`) (§6.1, §8).
* **Bestaetigt:** Applier (Liste, 4 Gates, Band, Radius, Schaden, Zustand, Zone, Zeile, Sperre, Richtung),
  alle Schadens-Records, Flug/Op 47/Op 40/19/50, alle Zombie-/Hund-/Kraehen-/Spinnen-/Arm-Tabellen, der
  Zombie-DoT (Bytes), RE1.5 DAT_8006F418/F430 und die Port-Zeile-17-Luecke fuer Weg A.
* **Fehlt fuer den Bau (§9):** Art-Zuordnung der vertauschten Ids (RE1.5 0x0A -> RE2-Art 2), Zustellweg-
  Entscheidung (RE1.5-Wurf/Zuender vs. RE2-Flug-Kontakt), Spalte fuer Weg A, Box-Erweiterung ueber den
  Aufruf hinaus, Box-Test auf +0x84/+0x8C, Leerlisten-Waechter, Hund-Immunitaet nach Zeile 10.

---

## 1. Zustellung (RE2-EXE `info/re2leon/PSX.EXE`, selbst disassembliert)

Mitschnitte: `build/r34g_gegner_re2_gegen/dis_8001ed9c.txt` (Flug), `dis_8001f198_op17.txt`,
`dis_80020c3c_op47.txt`, `dis_80020f3c_op48.txt`, `dis_800215c8_op49.txt`, `dis_80020758_op40.txt`,
`dis_8001f2c0_op19.txt`, `dis_80021970_op50.txt`, Opcode-Tabelle `optable_8009d868.txt`,
Voll-Listing `full_re2_exe.txt` (Werkzeug `volldis.py`, nutzt `load()`/`dis_one()` des Skill-Skripts).

### 1.1 Flug Op 0x0F = FUN_8001ED9C

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| Tabellenplatz `@0x8009d8a4` | **bestaetigt** | `table 0x8009d868 96`: `[15] 0x8009d8a4 -> 0x8001ed9c` |
| Box = 8 Byte von @0x80010900 nach sp+32 = {-1400,0,350,250} | **bestaetigt** | `8001edb8: lui a2,0x8001` / `8001edbc: addiu a2,a2,2304`, dann `lwl/lwr v1,3/0(a2)`, `lwl/lwr a0,7/4(a2)`, `swl/swr v1,35/32(sp)`, `swl/swr a0,39/36(sp)` (Rohworte `88c30003 98c30000 88c40007 98c40004 aba30023 bba30020 aba40027 bba40024`); `bytes 0x80010900`: `88 fa 00 00 5e 01 fa 00` = s16 {-1400, 0, 350, 250} |
| Hitcode 0x00030009 + Art(+0x1B, vorzeichenbehaftet `lb`) | **bestaetigt** | `8001ee90: lui s2,0x3` / `8001ee9c: ori s2,s2,0x9` / `8001eed0: lb a3,27(v1)` / `8001eed8: jal 0x800470c0` / `8001eedc: addu a3,a3,s2` (Verzoegerungsslot, laeuft vor dem Sprung) |
| Pruefpunkte y+1000, dann y-1000 | **bestaetigt** | `8001eec8: addiu v0,v0,1000` / `8001eecc: sw v0,20(sp)`; `8001eef8: addiu v0,v0,-2000` / `8001eefc: sw v0,20(sp)`; zweiter `jal 0x800470c0` @0x8001ef08 mit `addu a3,a3,s2` @0x8001ef0c (derselbe Hitcode) |
| Gier = +0x22 | **bestaetigt** | `8001eed4: lh a1,34(v1)`, `8001ef04: lh a1,34(v1)` |
| Treffer (Summe der Rueckgaben != 0) -> Explosion; sonst Lebensdauer +0xB > 0 -> weiter | **bestaetigt** | `8001ef10: addu s0,s0,v0` / `8001ef14: bne s0,zero,0x8001ef38`; `8001ef28: lbu v0,11(t2)` / `8001ef30: bne v0,zero,0x8001ef80`; Abzug je Aufruf `8001ee64: lbu v0,11(v1)` / `8001ee6c: addiu v0,v0,-1` / `8001ee70: sb v0,11(v1)` |
| Explosion: Status +0x18 \|= 0x80, Lage -> 0x800CFB88/8C/90, Dispatch `table[step[2] + Art]` | **bestaetigt** | `8001ef44: lhu v0,24(v1)` / `8001ef4c: ori v0,v0,0x80` / `8001ef50: sh v0,24(v1)`; `sw a0,-1144(at)` / `-1140` / `-1136` @0x8001ef58/64/70; `8001ef74: lbu v0,2(v1)` / `j 0x8001f0e4` -> `8001f0e4: lb v1,27(v1)` / `addu` / `sll 2` / `8001f0fc: lw v0,-10136(at)` (= 0x8009d868) / `8001f104: jalr v0` |

**Ergaenzungen, die das Dossier nicht nennt (fuer den Bau noetig):**
* Der Flug ist **Opcode B**, nicht A: Op 17 (Start) setzt `+0x0 := 22` (`8001f1c4: addiu v0,zero,22` /
  `8001f1c8: sb v0,0(v1)`) und `+0x1 := 15` (`8001f1d4: addiu a2,zero,15` / `8001f1d8: sb a2,1(v0)`),
  Status `+0x18 := 0xB403` (`8001f1e4/e8`), und ruft den Flug einmal direkt (`8001f288: jal 0x8001ed9c`).
* **Lebensdauer +0xB je Art** (in Op 17): `(u16 @0x800CFD06) & 0x1F == 9` (`8001f224: lhu v0,0(a1)` /
  `andi v0,v0,0x1f` / `8001f230: bne v0,v1(=9)`) -> `10 + rand%3` (`jal 0x80015fe8`, Kehrwert 0x55555556
  @0x8001f240-60, `8001f26c: addiu v0,v0,10` / `8001f274: sb v0,11(v1)`); sonst **15**
  (`8001f284: sb a2(=15),11(v0)`). Brand/Saeure: hoechstens 15 Flugaufrufe, dann Explosion mit Status 0x80.
* **Zwei weitere Wege in die Explosion ohne Status 0x80 und ohne 0x800CFB88-Schreiben:**
  (a) `8001ede8: jal 0x800527b4` (x,z) -> Rueckgabe != 0 und < y (`8001ee14: slt v0,v1,v0`) -> FX
  0x1A051C00 (`8001ee1c/20`), `y += 500` (`8001ee34/3c`), Dispatch `table[step[2]+Art]`
  (`8001ee4c: lbu v0,2(v1)` / `j 0x8001f0e4`); (b) Wandkontakt `8001ef84: lw v0,-13368(v0)` (0x800DCBC8)
  != 0 -> Rueckprall-Rechnung, dann `8001f0e0: lbu v0,3(v1)` = step[3] -> `table[step[3]+Art]`.
  Fuer Op 47 folgenlos; fuer Op 48/49 ebenfalls folgenlos, weil deren Status-Weiche tot ist (§1.3/1.4).

### 1.2 Explosion Explosiv, Op 47 = 0x80020C3C

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| Phasen-Sprungtabelle @0x80010928, Phase 0 = 0x80020cd0 | **bestaetigt** | `80020ca4: lhu v1,18(v0)` / `sltiu v0,v1,0x5` / `80020cc0: lw v0,2344(at)` (0x80010928) / `jr v0`; `bytes 0x80010928`: `d0 0c 02 80 7c 0e 02 80 90 0e 02 80 a4 0e 02 80 b8 0e 02 80` |
| `step[1] := 47` @0x80020d14-18 | **bestaetigt** | `80020d14: addiu v0,zero,47` / `80020d18: sb v0,1(v1)`; dazu `+0x18 := 0x8400` (`80020d00/04`), `+0x0 := 0` (`80020d08`) |
| Gate Unter-Id +0x1E == 12, sonst nur SE 0x01140001 | **bestaetigt** | `80020d30: lbu v1,30(v1)` / `80020d34: addiu v0,zero,12` / `80020d38: bne v1,v0,0x80020dc0` / Slot `80020d3c: lui a0,0x114` (laeuft in beiden Wegen) / `80020dc0: ori a0,a0,0x1` / `80020dc4: jal 0x8005ba28` |
| SE 0x01110001, Hitcode 0x10020009 | **bestaetigt** | `80020d40: lui a0,0x111` / `80020d44: ori a0,a0,0x1` / `80020d48: jal 0x8005ba28`; `80020d54: lui a3,0x1002` / `80020d58: ori a3,a3,0x9` |
| Box = sp+32 + (+0x1E*8 - 96) = Box[0] | **bestaetigt** | `80020d68: lbu a2,30(v0)` / `sll a2,a2,3` / `addiu a2,a2,-96` / `80020d7c: addu a2,s0(=sp+32),a2` (Slot); 16 Byte von @0x80010918 (`80020c58/5c: lui a2,0x8001 / addiu a2,a2,2328`, lwl/lwr-Kopie nach sp+32..47) |
| Box[0] = {-2000,0,1000,500}, Box[1] = {-1000,0,500,250} | **bestaetigt** | `bytes 0x80010918`: `30 f8 00 00 e8 03 f4 01` · `18 fc 00 00 f4 01 fa 00` |
| Pruefhoehe y, dann y+900 | **bestaetigt** | `80020cdc-cfc` Punkt = (+0x34,+0x36,+0x38) nach sp+16..24; `80020d98: addiu v0,v0,900` / `sw v0,20(sp)` / zweiter `jal 0x800470c0` @0x80020db0 |

Ergaenzung: die Rueckgaben beider Aufrufe werden verworfen. Die Radius-Erweiterung der Box (§2) bleibt
nach einem Treffer im Puffer sp+32 stehen und gilt fuer den ZWEITEN Aufruf (y+900) mit (§2, Befund E2).

### 1.3 Brand, Op 48 = 0x80020F3C — Status-Weiche WIDERLEGT (toter Zweig)

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| SE 0x01120001 | **bestaetigt** | `80020fd4: lui a0,0x112` / `80021028: ori a0,a0,0x1` / `8002102c: jal 0x8005ba28` (a1 = Slot+0x60, `80021030`) |
| Boxen @0x8001093c {-1200,0,600,300} und {-600,0,300,150} | **bestaetigt (Werte)** | `80020f54/58: lui a1,0x8001 / addiu a1,a1,2364`, 16-Byte-Kopie nach sp+32; `bytes 0x8001093c`: `50 fb 00 00 58 02 2c 01 a8 fd 00 00 2c 01 96 00` |
| Aufschlagpunkt -> +0x60/64/68 | **bestaetigt** | `80020fe8-f0: lh v1/a1/a2,52/54/56(a3)` -> `80021000: sw v1,96(a3)` / `80021014: sw a1,100(a3)` / `80021018: sw a2,104(a3)` |
| **Weiche Status-Bit 0x80: 0 -> Box ±1200 + Bodenflammen, 1 -> Box ±600** | **WIDERLEGT** | Op 48 UEBERSCHREIBT den Status VOR dem Test: `80020ff4: ori v0,zero,0x8000` (Rohwort `34028000`) / `80020ff8: sh v0,24(a3)` (`a4e20018`, a3 = laufender Slot aus `80020f40: lw a3,-13360(a3)`, danach unveraendert). Der Test liest `80021040: lhu v0,24(v1)` (`94620018`, v1 = derselbe Slot aus 0x800DCBD0) / `80021048: andi v0,v0,0x80` -> 0x8000 & 0x80 = **0, immer**. Dazwischen liegt nur `jal 0x8005ba28` (SE-Spieler; schreibt in seinen Kanal-Satz, `8005bc30`/`8005bd1c: sw v0,24(v1)` mit v1 = Kanal, kein Zugriff auf 0x800DCBD0). Einziger Sprung nach 0x800214e8 im ganzen EXE-Listing ist `8002104c: bne` -> **der ±600-Zweig @0x800214e8-574 ist toter Code.** |
| Aufruf 1 und 2 am selben Punkt (Slot+0x60), +1800 nur auf die Kopie sp+20 | **bestaetigt** | `80021050: addiu a0,v1,96` (Slot des `bne`, laeuft immer) / `80021060: jal`; `80021074: lw v0,20(sp)` / `80021080: addiu v0,v0,1800` / `80021084: sw v0,20(sp)` / `8002108c: jal` mit `80021090: addiu a0,a0,96`. Praezisierung: sp+16..24 enthaelt den ALTEN Inhalt von +0x60..+0x68 (`80020fc4-fe4: lw v0,96/100/104(a3)` -> `sw 16/20/24(sp)`, VOR dem Ueberschreiben), nicht den Aufschlagpunkt; er wandert am Ende (+3600 auf y) zurueck (`80021578-a0`). Fuer den Schaden folgenlos. |
| Bodenflammen 0x0505xxxx "nur im Wand/Boden-Zweig" | **widerlegt (Bedingung)** | Der Zweig ist der einzige erreichbare -> **nach JEDEM Brand-Aufschlag 3 Bodenflammen** (`80021114: lui v0,0x505`, `jal 0x8001cbe8` @0x8002116c / 0x800212c0 / 0x80021418, Gier +0 / +400 / -400 plus Zufall). Jede bekommt **+0x4A := 1** (`8002121c: sh s1(=1),-29382(at)` = Pool 0x800D8CF0 + 0x4A; `8002135c` / `800214b4: sh v0(=1),-29382(at)`) -> Op-19-Gate `+0x4A != 0` erfuellt. |

**Korrektur fuer den Bau:** Brand-Stoss = 2 Aufrufe Hitcode 0x0002000A am Aufschlagpunkt mit Box
{-1200,0,600,300} (±1200 ohne Radius), IMMER; danach 3 Bodenflammen. Box {-600,0,300,150} @0x80010944
wird von Op 48 nie benutzt.

### 1.4 Saeure, Op 49 = 0x800215C8 — Status-Weiche WIDERLEGT (toter Zweig)

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| Phase-0-Sprungtabelle @0x80010950[0] = 0x80021678 | **bestaetigt** | `80021668: lw v0,2384(at)` (0x80010950) / `jr v0`; `bytes 0x80010950`: `78 16 02 80 …` |
| SE 0x01130001 | **bestaetigt** | `80021678: lui a0,0x113` / `8002167c: ori a0,a0,0x1` / `800216ac: jal 0x8005ba28` (a1 = sp+16) |
| Hitcode 0x1002000B, Pruefhoehe y und y+1800 | **bestaetigt** | `800216e4: lui a3,0x1002` / `800216f0: ori a3,a3,0xb` (Slot) / `800216ec: jal`; `8002170c: addiu v0,v0,1800` / `80021710: sw v0,20(sp)` / `80021718: jal` mit `8002171c: ori a3,a3,0xb`; Punkt = (+0x34,+0x36,+0x38) @0x80021628-48 |
| **Weiche Status-Bit 0x80: 1 -> sp+40 (±600), 0 -> sp+32 (±1200)** | **WIDERLEGT** | `80021694: ori v0,zero,0x8403` (`34028403`) / `80021698: sh v0,24(v1)` (`a4620018`) setzt den Status VOR dem Test; `800216c0: lhu v0,24(v1)` / `800216c8: andi v0,v0,0x80` -> 0x8403 & 0x80 = **0, immer** -> `800216cc: bne` nie genommen -> `800216d4: j 0x800216e0` / `800216d8: addiu s0,sp,32`. `800216dc: addiu s0,sp,40` ist nur Sprungziel dieses `bne` -> **tot**. |

**Korrektur fuer den Bau:** Saeure-Stoss = 2 Aufrufe Hitcode 0x1002000B, Box {-1200,0,600,300} (±1200),
IMMER. Die Konstante GL_BOX_GEGNER ist fuer Brand UND Saeure zu streichen.

### 1.5 Nachbrenner Op 40 = 0x80020758, Op 19 = 0x8001F2C0, Op 50 = 0x80021970

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| Tabellenplatz @0x8009d908 | **bestaetigt** | `[40] 0x8009d908 -> 0x80020758` |
| Box @0x80010910 {-600,0,300,150} (Kopie nach sp+32, kein Zeiger auf die Daten) | **bestaetigt** | `80020768/6c: lui a2,0x8001 / addiu a2,a2,2320`, lwl/lwr-Kopie nach sp+32..39, `800207c0: addiu a2,sp,32` (Slot); `bytes 0x80010910`: `a8 fd 00 00 2c 01 96 00` |
| Hitcode 0x2002000A, Pruefhoehe y-100 | **bestaetigt** | `80020794: lui a3,0x2002` / `800207a0: ori a3,a3,0xa`; `800207a4: addiu v0,v0,-100` / `800207a8: sw v0,20(sp)` |
| Treffer -> Op 50 | **bestaetigt** | `800207c4: beq v0,zero,0x800207d4` / `800207cc: jal 0x80021970` |
| Op 50: step[2] := 64, acc.x := 0, step[3] := 0, vel.x := 0, "Status" &= ~2 | **bestaetigt** (Praezisierung) | `80021978/7c: addiu v0,zero,64 / sb v0,2(v1)`; `8002198c: sb zero,8(v0)`; `80021990: sb zero,3(v0)`; `8002199c: sh zero,12(v0)`; `800219a0: lhu v0,18(v1)` / `800219a8: andi v0,v0,0xfffd` / `800219b0: sh v0,18(v1)` = **Step-Status +0x12** (nicht Slot-Status +0x18) |
| Op 19: +0x4A != 0, step[0x16] >= 16, X-Aspekt > 0x1000 -> Op 40; step[0x16]++ | **bestaetigt** | `8001f2d0: lh v0,74(v1)` / `8001f2d8: beq v0,zero,0x8001f32c`; `8001f2e0: lhu v0,22(v1)` / `sltiu v0,v0,0x10` / `bne -> 8001f310`; `8001f2f4: lhu v0,4(v1)` / `sltiu v0,v0,0x1001` / `bne -> 8001f310`; `8001f308: jal 0x80020758`; `8001f31c-28: lhu/addiu 1/sh 22(v1)`. Praezisierung: das `++` laeuft nur bei `+0x4A != 0` (der `beq` @0x8001f2d8 springt darueber). |

## 2. Applier FUN_800470C0 (Mitschnitt `dis_800470c0_applier.txt`, 420 Instruktionen bis `jr ra` @0x8004765c)

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| Liste 0x800CFE1C .. *(0x800CE334) | **bestaetigt** | `800470f8: lui s7,0x800d` / `800470fc: addiu s7,s7,-15896` (= 0x800CC1E8) / `80047118: addiu s2,s7,15412` (= 0x800CFE1C; das Werkzeug annotiert faelschlich 0x800d3c34) / `8004711c: lw s0,0(s2)`; Ende `80047410: lw v0,8524(s7)` (= 0x800CE334) / `80047418: bne s2,v0,0x8004711c`. Raum-Init setzt Ende = Anfang: `80049ed4: addiu v0,s0,15412` / `80049edc: sw v0,-7372(at)` |
| Gate 1 Wort0 & 1 | **bestaetigt** | `80047124: lw v0,0(s0)` / `8004712c: andi v0,v0,0x1` / `80047130: beq v0,zero,0x8004740c` |
| Gate 2 +0x1D3 != 0 (ganzes Byte) | **bestaetigt** | `80047138: lbu v0,467(s0)` / `80047140: bne v0,zero,0x8004740c` |
| Gate 3 HP (+0x156, s16) < 0 | **bestaetigt** | `80047148: lh v0,342(s0)` / `80047150: bltz v0,0x8004740c` |
| Gate 4 +0x10E & 0xC000 | **bestaetigt** | `80047158: lhu v0,270(s0)` / `80047160: andi v0,v0,0xc000` / `80047164: bne` |
| +0x1D0 &= 0xFF00 je Kandidat VOR dem Band | **bestaetigt** | `8004716c: lhu v1,464(s0)` / `80047178: andi v1,v1,0xff00` / `80047184: sh v1,464(s0)` |
| Band `(Y + h98 + 100 + d9e - P.y) <u 2(d9e+100)` | **bestaetigt** | `80047170: lh a0,152(s0)` (h s16) / `80047174: lw v0,60(s0)` (Y s32) / `8004717c: addu` / `80047180: addiu v0,v0,100` / `80047188: lhu v1,158(s0)` (d u16) / `8004718c: lw a0,4(s4)` (P.y) / `80047190: addu` / `80047194: subu` / `80047198: addiu v1,v1,100` / `8004719c: sll v1,v1,1` / `800471a0: sltu` / `800471a4: beq v0,zero,0x8004740c`. P.y in (Y+h-d-100, Y+h+d+100] |
| Radius: p3 += (s16)+0x1EE >> 2, p2 += dto. | **bestaetigt** | `800471bc: lhu v1,494(s0)` / `sll v1,v1,16` / `sra v1,v1,18` / `800471d0: sh v0,6(s3)`; zweimal fuer p2: `800471ec: sh v0,4(s3)` (Verzoegerungsslot des `jal 0x80041ef8`) |
| Ruecknahme nur im Nicht-Treffer-Zweig | **bestaetigt** | `800471f0: beq v0,zero,0x800473dc`; Ruecknahme `800473dc-408` (`subu` + `sh 6(s3)`/`sh 4(s3)`); Treffer-Weg endet mit `j 0x8004740c` (@0x800473d4) bzw. `beq ... 0x8004740c` (@0x800473c0) |
| Treffer: +0x1D0 \|= 1 | **bestaetigt** | `800471f8: lhu v0,464(s0)` / `80047200: ori v0,v0,0x1` / `80047204: sh v0,464(s1)` |
| Modus: Hitcode & 0x10000 = alle, sonst erster | **bestaetigt** | `80047208: lui v0,0x1` / `8004720c: and v0,s5,v0` / `80047210: beq v0,zero,0x80047434` (Einzelzweig bricht die Schleife sofort ab) |
| Schaden = (w0 >> 10K) & 0x3FF, Record 0x800A6A88[Typ] + (Zeile-1)*20 | **bestaetigt** (Praezisierung) | `80047218: lbu v0,8(s1)` / `8004722c: lw a1,27272(at)` (0x800A6A88) / `80047230-3c: sll 2 / addu / sll 2 / addiu -20` / `80047240: addu a1,a1,v0` / `80047244-50: 10K` / `80047254: srlv` / `8004725c: andi v1,v1,0x3ff` / `80047260: subu` / `80047268: sh v0,342(s1)`. Praezisierung: der Zeilenindex ist **Hitcode & 0xFFFF** (`80047214: andi v1,s5,0xffff`, Verzoegerungsslot), nicht & 0xFF — fuer alle GL-Hitcodes gleich |
| +0x1FC := altes Wort ausser 0xC02 (Einzel: 0xC02/0xC03) | **bestaetigt** | `80047264: lw v1,4(s1)` / `8004726c: addiu v0,zero,3074` / `80047270: beq` / `80047278: sw v1,508(s1)`; Einzel `8004748c: addiu v0,a0,-3074` / `80047490: sltiu v0,v0,0x2` / `8004749c: sw a0,508(s1)` |
| +0x4-WORT 2, bei HP < 0 3 | **bestaetigt** | `80047280: addiu v0,zero,2` / `80047284: bgez v1,0x80047294` / `80047288: sw v0,4(s1)` (Slot, laeuft immer) / `8004728c/90: addiu v0,zero,3 / sw v0,4(s1)`. HP == 0 -> 2. `sw` = Wort -> +0x5/+0x6/+0x7 = 0 |
| Zone: 1; 0x20000 && Y+h/2 < P.y -> 0; Wort0 & 0x10000000 && P.y < Y+3h/2 -> 2 | **bestaetigt** (Praezisierung) | `80047294/98: addiu v0,zero,1 / sb v0,466(s1)`; `8004729c-a4: lui 0x2 / and / beq -> 0x80047314`; `800472b8: sra a0,v0,17` (h/2) / `800472c8: slt` / `800472d4: sb zero,466(s1)`; `800472dc: lui v1,0x1000` / `800472e4: beq` / `800472e8: sll v1,a0,1` / `80047300: slt v1,v1,v0` / `8004730c: sb v0(=2),466(s1)`. Praezisierung: die Kopf-Regel liegt INNERHALB des 0x20000-Zweigs (alle GL-Hitcodes haben 0x20000) |
| +0x5 = Hitcode low byte; +0x1D2 += 3K | **bestaetigt** | `80047324: sb s5,5(s1)`; `80047310: sll v1,s6,1` / `80047320: addu v1,v1,s6` / `80047328: addu v0,v0,v1` / `80047330: sb v0,466(s1)` |
| Sperre +0x1D3 = (alt & 0x80) \| ((w1 >> 9) & 0x7F) | **bestaetigt** | `8004731c: lbu a0,467(s1)` / `8004732c: andi a0,a0,0x80` / `80047334: sb a0,467(s1)` / `80047338: lw v0,4(a1)` (w1) / `80047340: srl v0,v0,9` / `80047344: andi v0,v0,0x7f` / `80047348: or` / `8004734c: sb a0,467(s1)` |
| Richtung: a = Peilung(P->G) - Yaw(+0x76), Bits 0x20/0x40/0x80 | **bestaetigt** | `80047350: lw a0,0(s4)` / `80047354: lw a1,8(s4)` / `80047314: lw a2,56(s1)` (+0x38) / `8004733c: lw a3,64(s1)` (+0x40) / `80047358: jal 0x800154ac`; FUN_800154AC: `800154c8: subu s0,a2,a0` (dx = G-P) / `800154dc: subu a0,a3,a1` (dz = G-P), alle vier Argumente auf s16 gekuerzt (`sll 16 / sra 16` @0x800154b4-d8); `80047360: lh v1,118(s1)` / `80047368: subu a0,v0,v1` / `8004736c-78: +1024 / &0xfff / slti 2048` -> 0x20; `8004737c/94-98: +1536 / slti 1024` -> 0x40; `800473a0/b8-bc: -512 / slti 1024` -> 0x80 |

**Ergaenzungen / neue Befunde zum Applier:**
* **E1 — Leerlisten-Waechter (fehlt im Dossier).** `800470c0: lui v0,0x800d` / `800470c4: lbu v0,-1037(v0)`
  (= 0x800CFBF3, Gegnerzahl) / `8004710c: beq v0,zero,0x8004762c` -> Rueckgabe 0 ohne Listenlauf. Die
  Schleife ist eine do-while (Rumpf vor dem Endtest @0x80047418). 0x800CFBF3 = Gegnerzahl: Raum-Init
  `80049ee4: sb zero,-1037(at)`, Abmelden `8001b250-70: lbu/addiu -1/sb` auf 0x800CFBF3.
* **E2 — Box-Erweiterung ueberlebt den AUFRUF.** Die Aufrufer uebergeben einen Stapelpuffer (sp+32), den
  sie fuer den ZWEITEN Aufruf derselben Op wiederverwenden (Flug y-1000 @0x8001eee8, Op 47 y+900
  @0x80020db4, Op 48 Aufruf 2 @0x80021068, Op 49 Aufruf 2 @0x800216f8). Nach einem Treffer bleibt
  p2/p3 um r/4 des getroffenen Gegners vergroessert — im Alle-Modus (Schleife laeuft weiter) UND im
  Einzelmodus (`80047210: beq -> 0x80047434` verlaesst die Schleife ohne Ruecknahme). Der zweite Aufruf
  prueft also mit einer um r_A erweiterten Box. Erst die naechste Op-Ausfuehrung kopiert frisch aus den
  Daten (lwl/lwr-Kopie im Prolog jeder Op). Das Dossier nennt nur den Alle-Modus innerhalb EINES Aufrufs.
* **E3 — Box-Test prueft den Trefferkasten-Mittelpunkt +0x84/+0x8C, nicht +0x38/+0x40.**
  `800471b4: addiu a2,s0,132` -> FUN_80041EF8 liest `lw 0(s2)` / `lw 8(s2)` (@0x80042044-8c) = +0x84/+0x8C.
  Schreiber FUN_80036E30: `80036e84: lw v0,56(a2)` / `80036e90: addu v0,v0,v1(=lh +0xA0)` /
  `80036e94: sw v0,132(a2)`; `+0x8C = +0x40 + lh +0xA2` (@0x80036e9c-b0); +0xA0/+0xA2 = +0x94/+0x96, bei
  Gier & 0x400 vertauscht (@0x80036e34-74). Zombie: +0x94 = +0x96 = 0 im INIT (`80100990/94: sh zero,148/150`)
  -> dort gleich; **Hund dagegen +0x94 = 500** (EMD0G_MOD0.BIN `80100284: addiu v0,zero,500` /
  `80100288: sh v0,148(s0)`, dazu +0x98 = -1000 @0x8010028c/94, +0x1EE = 600 @0x80100290/0x801002c4) ->
  beim Hund liegt der gepruefte Punkt 500 neben +0x38/+0x40 (x oder z je nach Gier-Quadrant). Das Band nimmt Y = +0x3C (`lw v0,60(s0)`), die Richtung
  +0x38/+0x40.
* **E4 — Rueckgabewert:** Alle-Modus gibt den LETZTEN getroffenen Gegner zurueck (`8004742c: j 0x80047630` /
  `addu v0,s1,zero`), Einzelmodus den ersten; die Aufrufer werten nur != 0 aus.
* **E5 — Takt der Sperre = 15 Bilder bestaetigt:** die Gegnerschleife der Hauptschleife
  (`800267c0 .. 80026930: addiu s2,s2,4` / `lw v0,8524(s3)` / `bne`) laeuft VOR der FX-Pumpe
  (`80026980: jal 0x8001d300`). Stempel 15 im Bild N, Dekrement in N+1..N+15, in N+15 wieder trefferbar.

## 3. Boxtest FUN_80041EF8 (Mitschnitt `dis_80041ef8_box.txt`)

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| Ecke = P + R(Yaw)*(p0, 0, -p1-4*p3) | **bestaetigt** | Uebersetzung sp+84/88/92: `80041f28: lh v0,0(s1)` / `80041f34: sw v0,84(sp)` (p0), `80041f30: sw zero,88(sp)`, `80041f38: lh v1,2(s1)` / `80041f3c: lh a3,6(s1)` / `80041f68: subu v1,zero,v1` / `80041f6c: sll a3,a3,2` / `80041f70: subu v1,v1,a3` / `80041f78: sw v1,92(sp)`; Matrix sp+64 = Einheitsmatrix (`sw 4096/0/4096/0/4096` @0x80041f4c-5c); R aus SVECTOR (0, yaw, 0) (`80041f44: sh a1,162(sp)` / `80041f74: jal 0x8008e1f4`, a1 = sp+128) mit Translation (P.x, 0, P.z) (`80041f94/90/a4: sw 148/152/156(sp)`); Verkettung `80041fa0: jal 0x8002ce94` -> Translation sp+116/124 |
| Kanten R*(p2,0,0) und R*(0,0,2*p3) | **bestaetigt** | `80041fcc: lh v0,4(s1)` / `80041fdc: sw v0,16(sp)` / `80041fd4: sw zero,20(sp)` / `80041fd8: jal 0x8008dba4` (SVECTOR-Ausgabe sp+24); `80041fe8: lh v0,6(s1)` / `80041ff4: sll v0,v0,1` / `80041ffc: sw v0,20(sp)` / `80041ff0: sw zero,16(sp)` / `80041ff8: jal 0x8008dba4` (Ausgabe sp+32). Die Folgelesungen `lh 24`, `lhu 28`, `lh 32`, `lhu 36` (@0x8004200c-1c) belegen SVECTOR-Ausgaben (s16) |
| Viertel-Raster | **bestaetigt** | Ecke `80041fbc/c0: sra 2`; Ziel `80042054/5c/78/80/ac/b8: sra 2` auf `lw 8(s2)` / `lw 0(s2)` |
| Test = zwei Vorzeichenwechsel | **bestaetigt** (eigene Herleitung) | `800420e4: jal 0x8008d6c4` (Matrixprodukt, Ergebnis sp+64); `800420ec-fc: lh 64 / lh 66 / andi 0x8000 / beq -> 0` = e1*(G-C) und e1*(G-C-e1) verschiedenen Vorzeichens; `80042104-18: lh 70 / lh 74 / andi 0x8000 / xor / sltu` = dasselbe fuer e2 -> Parallelogramm C + a*e1 + b*e2, 0 <= a,b < 1 |
| Weltmass x in [p0, p0+4*p2+r], z in [-4*p3-r, 4*p3+r] (p1 = 0) | **bestaetigt** | folgt aus Ecke/Kanten (Kante e1 = 4*p2 Weltmass, e2 = 8*p3), Radius-Erweiterung p2/p3 += r/4. Achtung: in x waechst die Box NUR nach +x (p0 bleibt), in z symmetrisch |

## 4. Schadens-Records 0x800A6A88 (Werkzeug `records.py`, Ausgabe `records.txt`)

Zeigertabelle selbst gedumpt (`table 0x800a6a88 0x40`, `rec_ptr_800a6a88.txt`): [0x10..0x14] = 0x800A412C,
[0x15..0x17] = 0x800A42A8, [0x18..0x1F] = 0x800A412C, [0x20] = 0x800A4424, [0x21] = 0x800A45A0,
[0x25] = [0x26] = 0x800A4B90, [0x2C] = 0x800A412C, [0x2D] = 0x800A5180 — **alle bestaetigt**.

| Zeile (Dossier §1.3 / KONSTANTEN) | Urteil | Eigene Bytes (w0 w1) -> K0/K1/K2, Sperre |
|---|---|---|
| Zombie 0x10/0x11/0x13 Zeile 9/10/11 @0x800A41CC/E0/F4 | **bestaetigt** | `c8 c8 a0 00 0a 1e 8f 07` -> 200/50/10, 15 · `c8 c8 50 00 0a 1e 8f 07` -> 200/50/5, 15 · `c8 c8 a0 00 …` -> 200/50/10, 15 |
| 0x16 @0x800A4348/5C/70 | **bestaetigt** | `50 c8 a0 00` -> 80/50/10 · `50 c8 50 00` -> 80/50/5 · `c8 c8 a0 00` -> 200/50/10; w1 0x078F1E0A -> 15 |
| Hund @0x800A44C4/D8/EC | **bestaetigt** | `2c c9 a0 00` -> 300/50/10 · `2c c9 50 00` -> 300/50/5 · `2c c9 a0 00` -> 300/50/10; 15 |
| Kraehe @0x800A4640/54/68 | **bestaetigt** | `3c f0 f0 00` x3 -> 60/60/15; 15 |
| Spinne @0x800A4C30/44/58 | **bestaetigt** | `3c f0 40 01 b4 1f 8f 07` -> 60/60/20 · `82 08 52 00 68 1f 8f 07` -> 130/130/5 · `5a f0 a0 00 68 1f 8f 07` -> 90/60/10; w1 0x078F1FB4 / 0x078F1F68 -> 15 |
| Zellenarm 0x2D @0x800A5220/34/48 | **bestaetigt** | `3c f0 40 01` -> 60/60/20 · `3c f0 a0 00` -> 60/60/10 · `3c f0 a0 00` -> 60/60/10; 15 |
| Zombie w1 & 7 = 2 (Reserve-Kosten Zeile 9/10/11) | **bestaetigt** | w1 = 0x078F1E0A -> & 7 = 2, (>>3)&7 = 1, (>>6)&7 = 0 |
| GL-Applier zieht KEINE Reserve ab, Hitscan-Applier schon (@0x80041954-88) | **bestaetigt** | Store-Liste FUN_800470C0 vollstaendig (§2): nur +0x1D0/+0x156/+0x1FC/+0x4/+0x1D2/+0x5/+0x1D3. FUN_800410CC: `8004196c: lbu v0,338(t0)` / `80041970: andi v1,v1,0x7` / `80041974: subu` / `80041978: sb v0,338(t0)` / Klemme `80041980-88` (dazu +0x151 @0x80041928-44) |

Zusatz: Unter-Id +0x1E = volles SUB-Byte des Spawn-Worts: FUN_8001BF10 `8001bf1c: srl v0,a0,16` /
`8001bf20: andi t5,v0,0xff` / `8001bfb0: sll v0,t5,16` / `8001bfb4: addu v0,v0,t6(=BANK)` /
`8001bfc0: sw v0,28(t0)` -> +0x1C = Bank, +0x1E = SUB; 0x020C0A00 / 0x020C1000 -> +0x1E = 0x0C = 12
(**bestaetigt**, GL_EXPL_GATE erfuellt). +0x1B wird beim Spawn genullt (`8001bfbc: sb zero,27(t0)`).

## 5. Zombie-Familie (EMZ0.BIN == EMOVL10_S0.BIN, md5 7f7a39e6… beide; Listing `full_EMZ0.BIN.txt`)

### 5.1 Tabellen

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| HURT-Basis 0x8010C940, Index Zeile*36 + Spalte*4 | **bestaetigt** | `801053e0: lbu v1,5(a0)` / `801053e4: lui a2,0x8011` / `801053e8: addiu a2,a2,-14016` / `801053ec-f4: *9*4` / `801053f8: lbu v1,466(a0)` / `80105400: sll v1,v1,2` / `80105408: lw v0,0(v1)` / `80105410: jalr v0` |
| DEATH-Basis 0x8010CC24 | **bestaetigt** | `801084e4: lbu v1,5(a0)` / `801084e8: lui a2,0x8011` / `801084ec: addiu a2,a2,-13276` / … / `80108514: jalr v0` |
| HURT Zeile 9 {7438,5BC0,5BC0,5438x6}; 10/11 {5BC0x3,5438x6} | **bestaetigt** | `table 0x8010CA84/0x8010CAA8/0x8010CACC 9 --bin EMZ0.BIN` (`zombie_tabs.txt`) |
| DEATH Zeile 9 {7438,8BEC,8BEC,8530x6}; 10/11 8530x9; Zeile 17 {7438,8BEC,0,7438,9610,0,7438,8530,0} | **bestaetigt** | `table 0x8010CD68/0x8010CD8C/0x8010CDB0/0x8010CE88 9 --bin EMZ0.BIN` |
| Kopf-Spalte beim stehenden Zombie unerreichbar | **bestaetigt** (eigener Beleg statt Port-Zitat) | einziger Wort0-Zonen-Schreiber im Overlay: INIT `80100984: lui v1,0xc00` / `80100988: or` / `80100998: sw v0,0(s2)` (+ Loeschungen mit 0xF3FF…); alle `lui …,0x1000` in EMZ0 (@0x801002e8/0x801067e4/0x801077c0/0x8010895c) testen das GLOBALE Wort 0x800CFBD8, nicht Wort0. Ein EXE-Schreiber von Bit 0x10000000 in Gegner-Wort0 ist nicht ausgeschlossen (nicht gescannt) |
| Steh-Box +0x98 = -1500, +0x9E = 1500, +0x1EE = 500 | **bestaetigt** | `80100958: addiu v0,zero,-1500` / `8010095c: sh v0,152(s2)`; `80100960: addiu v0,zero,1500` / `80100964: sh v0,158(s2)`; `8010096c: addiu v1,zero,500` / `80100980: sh v1,494(s2)` (das Dossier nennt fuer ±1500 nur Port-Zeilen) |
| HP-Tabelle 50..128, Brad 0x11 = 250 | **bestaetigt** | `read 0x8010c670 16 --w 2`: 80,94,128,75,60,95,58,75,50,83,79,66,80,65,82,65 (Leser `80100708: lhu v0,-14736(at)`); `801008c8: addiu v1,zero,250` / `801008cc: sh v1,342(s2)` |

### 5.2 Element-Leiter — ein Standort WIDERLEGT

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| **Zeilen 9/10/11 im "HURT-Liegend-Zweig" @0x80105188-208** (samt dem Beispiel-Listing im Dossier und "Brand und Saeure loesen im … Liegen Verkohlung bzw. Aetzung aus") | **WIDERLEGT — toter Code** | Der Zweig schreibt das Zustandswort UNMITTELBAR vor der Leiter: `8010517c: lui v1,0x6` / `80105180: ori v1,v1,0x501` / `80105184: sw v1,4(s1)` (Rohwort `ae230004`) -> Little Endian +0x4 = 0x01, **+0x5 = 0x05**, +0x6 = 0x06. Die Leiter liest danach `80105188: lbu v1,5(s1)` = 5 -> `bne 10` (@0x80105194), `bne 11` (@0x801051d0), `bne 9` (@0x801051e8) nie gleich. Einziger Weg nach 0x80105188..0x8010520c ist das Durchfallen ab 0x80105184 (Voll-Listing: kein Sprungziel dort). Der Port weiss das bereits und bildet es stumm nach (`enemy_ai_re2_zombie.c:7052-7063`, "TOTER ZWEIG IM ORIGINAL"). Folge: ein LIEGENDER Zombie, der einen GL-Treffer ueberlebt, bekommt weder Verkohlung noch Aetzung noch Russ noch `+0x21A \|= 0x800` (kein DoT). |
| Leiter in DEATH-Liegend @0x80108444-4B8 | **bestaetigt** | kein Wort-/+0x5-Store zwischen DEATH-Wurzel 0x80108250 und `80108444: lbu v1,5(s0)`; Zeile 10 -> `jal 0x80106128` @0x80108468, 11 -> `jal 0x80106310` @0x80108480, 9/17 -> `jal 0x8010640c` @0x801084b4 |
| Leiter in STAGGER P0 @0x80105DC4-F18 (Zeilen 10/11) | **bestaetigt** | kein +0x5-Store in 0x80105BC0..0x80105DC4 (nur `80105c48: sb v0,6(s4)`); `80105dc4: lbu v1,5(s4)`; Zeile 10 + nicht verkohlt -> `80105de8: jal 0x80106128` + `80105df0-fc: lhu 538 / ori 0x800 / sh` ; Zeile 11 -> falls `+0x21A & 0x1000 == 0` und `(rand & 1) == 0` Bein wegaetzen (@0x80105e24-f10), dann `80105f14: jal 0x80106310` |
| Leitern DEATH-MAIN 0x80108530 @0x801086E4-7C0, Kriecher-Tod 0x80108A14 @0x80108B00-B88, Aufsteh-Tod 0x801099E4 @0x80109B90-C54, Kriecher-Treffer 0x80107888 @0x80107960-9B0, Aufsteh-Treffer 0x80107A78 @0x80107BE0-CE0, ZERREISSEN @0x80108CC0-CEC | **bestaetigt** | je Funktion vom Prolog bis zur Leiter kein `sw …,4(base)` / `sb …,5(base)` (Scan der Bereiche). Kriecher-Tabellen `0x8010CBE8[+0x5]` -> 0x80107888 und `0x8010CECC[+0x5]` -> 0x80108A14 fuer Zeilen 1..18 (`table`) |
| MAIN 0x80105438 und Knockdown 0x80107438 ohne 9/10/11-Leiter | **bestaetigt** | MAIN vergleicht +0x5 nur mit 16 (@0x80105510-20, @0x80105640-44, @0x8010585c-68) und 14 (@0x80105724-28, @0x80105870) |
| **Korrigierte Liste der LEBENDEN `+0x21A \|= 0x800`-Setzer** (fuer den DoT) | **Ergaenzung** | `ori v0,v0,0x800` im Overlay: @0x801051c0 und @0x8010526c (toter Liegend-Zweig), @0x80105560 (MAIN P0, nur Zeile 16), **@0x80105df8 (STAGGER P0, Zeile 10)**, @0x80107994 / @0x80107a14 (Kriecher-Treffer), @0x80107c28 / @0x80107c80 (Aufsteh-Treffer); dazu FUN_80106310 (Aetzung, `80106334: ori v0,v0,0x1800`). Keine Maske loescht 0x800/0x1000 wieder (kein `andi …,0xf7ff/0xefff/0xe7ff` im Overlay) -> der DoT laeuft bis zum Tod. |

### 5.3 DoT (Brand/Saeure ueber Zeit) — **bestaetigt**, zwei Praezisierungen

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| Gate (+0x10E & 0x80 oder +0x21A & 0x1000), (+0x236 & 7) == 0, +0x21A & 0x800 | **bestaetigt** | `80101dc0: lhu v0,270(s1)` / `80101dc8: andi v0,v0,0x80` / `80101dcc: bne -> 80101de8`; `80101dd4: lhu v0,538(s1)` / `80101ddc: andi v0,v0,0x1000` / `80101de0: beq -> 80101ec4`; `80101de8: lhu v0,566(s1)` / `80101df0: andi v0,v0,0x7` / `80101df4: bne -> 80101e94`; `80101dfc: lhu v0,538(s1)` / `80101e04: andi v0,v0,0x800` / `80101e08: beq -> 80101e94` |
| HP -1, Gier += 64 - ((r1 >> (r2&3)) & 1)*128 | **bestaetigt** | `80101e10/18: jal 0x80015fe8` x2 / `80101e20: andi v0,v0,0x3` / `80101e24: srav s0,s0,v0` (Rohwort 0x00508007) / `80101e28: andi s0,s0,0x1` / `80101e2c: sll s0,s0,7` / `80101e38: addiu v1,v1,64` / `80101e3c: subu v1,v1,s0` / `80101e40: addiu v0,v0,-1` / `80101e44: sh v0,342(s1)` / `80101e50: sh v1,118(s1)` (Slot) |
| Tod: Wort 0x0A03 bzw. 0x0B03, +0x1D2 = 4, +0x1D3 \|= 0x80, +0x21A \|= 0x2000 | **bestaetigt** | `80101e48: sll v0,v0,16` / `80101e4c: bgez`; `80101e58: addiu v1,zero,2563` / `80101e64: sw v1,4(s1)` (Slot, beide Wege) / `80101e68/6c: addiu v0,zero,2819 / sw`; `80101e70/74: addiu v0,zero,4 / sb v0,466(s1)`; `80101e80: ori v0,v0,0x80` / `80101e88: sb v0,467(s1)`; `80101e84: ori v1,v1,0x2000` / `80101e90: sh v1,538(s1)` |
| Saeure-Zucken je Bild: Gier += 64 - (rand >> 1) | **bestaetigt** + Praezisierung | `80101e94-a0: lhu 538 / andi 0x1000 / beq`; `80101ea8: jal 0x80015fe8` / `80101eb4: sra v0,v0,1` / `80101eb8: addiu v1,v1,64` / `80101ebc: subu` / `80101ec0: sh v1,118(s1)`. **Praezisierung:** FUN_80015FE8 liefert 0..255 (`80016014: andi v0,v0,0xff`), also rand>>1 in 0..127 -> Zucken in [-63, +64] |
| Zaehler +0x236: +1 je Wurzel-Aufruf, INIT 0 | **bestaetigt** | `801004f8: lhu v0,566(s0)` / `80100504: addiu v0,v0,1` / `80100508: sh v0,566(s0)` (nach dem Zustands-`jalr` @0x801004e8); `801008ac: sh zero,566(s2)`; einzige weitere Leser @0x80101de8 und @0x8010249c |
| Zweiter Block "Anrempel-Executor" @0x80102474-54C | **bestaetigt (Bytes), Benennung WIDERLEGT** | Bytes wie oben (`8010249c: lhu v0,566(s0)`, `801024c4-d0: HP -1`, `801024dc: addiu v1,zero,2563`, `801024f4: addiu v0,zero,2819`, Saeure-Zucken `80102520-4c`). Die Funktion ist EXEC[2] = 0x80102260 (Executor-Tabelle `8010c8d0: 80101a40` / `8010c8d4: 80102260`), laut Port-Befund (`enemy_ai_re2_zombie.c:1542ff`, gemessen) der **zweite Gang ("Arme oben")**, kein Anrempeln. Also: DoT tickt in BEIDEN Gang-Executoren EXEC[1]/EXEC[2], sonst nirgends. |
| Tod-Zelle [10][4] bzw. [11][4] = 0x80108530; Sturz liest 0x2000 -> Seite 0 | **bestaetigt** | Zeile-10/11-DEATH-Dump Spalte 4 = 0x80108530; `801085a4: jal 0x80015fe8` / `801085ac: lhu v1,538(s1)` / `801085b4: andi v1,v1,0x2000` / `801085bc: sb v0,362(s1)` (Slot, rand&1) / `801085c0: sb zero,362(s1)` |
| Leichen-Farbausblender @0x8010A810-868 | **bestaetigt** + Praezisierung | `8010a810: lhu v0,270(s0)` / `andi 0x80` / `beq`; Zaehler `8010a80c-20: lhu 346 / +1 / sh 346`; **Stopp**, wenn Part 0 Farb-Byte == 16 (`8010a82c: lbu v1,112(a2)` / `8010a830: addiu v0,zero,16` / `8010a834: beq`); `(+0x15A+1) & 3 == 0` (`8010a83c`); **15 Parts** (`8010a838: addiu a0,zero,15`) je `+= 0xFFFEFEFF` (`8010a848/4c`, = -0x010101) |

### 5.4 ZERREISSEN / Wegschleudern / Sturz

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| Phasentabelle @0x8010012C | **bestaetigt** | `80108c54: lbu v1,6(s3)` / `80108c70: lw v0,300(at)` / `jr v0`; `table 0x8010012c 7`: P0 = 0x80108c80 |
| Russ nur Zeile 9 (und 17) | **bestaetigt** | `80108cc0: lbu v1,5(s3)` / `beq 9` / `bne 17` / `80108cd8-e4: +0x10E & 0x80` / `80108cec: jal 0x8010640c` |
| Wegschleudern nur Zeile 9, "3 von 4" | **bestaetigt** | `80108cf4/fc: jal 0x80015fe8` x2 / `80108d04: andi v0,v0,0x7` / `80108d08: srav s0,s0,v0` / `80108d0c: andi s0,s0,0x3` / `80108d10: beq s0,zero -> 80108d44` / `80108d18: lbu v1,5(s3)` / `80108d20: bne v1,9` / `80108d28: sb v0(=1),561(s3)` / `80108d34: jal 0x80109610`. Zusatz: +0x231 == 1 beim Eintritt -> sofort Wegschleudern (`80108c34-44`); die Seite fuer Clip 4/3 kommt aus `+0x1D0 & 0x20` (`80108d44-54: lhu 464 / andi 0x20 / srl 5 / sb 363`) — also aus den Richtungsbits des Appliers |
| Sturz-Tod Clip {1,2}[Seite], Rate 15; Seite = rand&1, bei +0x21A & 0x2000 = 0 | **bestaetigt** | `8010855c/60: sb 1,16(sp) / sb 2,17(sp)`; `8010860c: lui v1,0xf`; Seite s. 5.3 |

## 6. Hund, Kraehe, Spinne, Zellenarm

### 6.1 Hund 0x20 (EMD0G_MOD0.BIN, `full_EMD0G_MOD0.BIN.txt`)

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| DEATH-Wurzel 0x801040DC -> 0x801055CC[+0x5]; [9] = 0x80104610, [10]/[11] = Router 0x80104118 | **bestaetigt** | `801040e4: lbu v0,5(a0)` / `801040f8: lw v0,21964(at)` / `80104100: jalr`; `table 0x801055CC`: [9] 0x80104610, [10]/[11] 0x80104118 |
| Router: +0x6 == 0 -> 0x80105618[+0x5] ([10] 0x80104774, [11] 0x8010481C), sonst 0x80105668[+0x6] | **bestaetigt** | `80104120: lbu v0,6(a0)` / `80104128: bne` / `80104144: lw v0,22040(at)` / `80104158: lw v0,22120(at)` |
| Zeile-9-Phasen @0x80105688, P0 = 0x80104694 | **bestaetigt** | `80104634: lw v0,22152(at)`; `table 0x80105688`: [0] 0x80104694 |
| Zeile 9 P0: K >= 1 -> nur Kern; sonst +0x231 := 1, Kern, Teile-Wurf, FX 7, +0x21F := 18 | **bestaetigt** + Ergaenzung | `801046a8-c4: lbu 5 / ==9 / lbu 466 / sltiu 3`; `801046e4: jal 0x80104178` / `801046e8: sb s1(=1),561(s0)` (Slot) / `801046ec: jal 0x80104440`; **Ergaenzung:** vor dem FX-7-Wurf wird das Budget auf 1 gesetzt (`80104708: sb s1(=1),543(s0)`, Slot des `jal rand`), Part = rand & 0xF, Wurf nur bei Part-Flags & 0x4A == 0 (`8010473c/40`), danach `80104754/58: +0x21F := 18` |
| Kern 0x80104178 = Todesschrei SE 7, stumm bei +0x231 | **bestaetigt** | `801041b8: lbu v0,561(s2)` / `801041c0: bne -> skip` / `801041c8: addiu a0,zero,7` / `801041cc: jal 0x8005bd6c` |
| Teile-Wurf 0x80104440: 7 Parts {2,3,4,7,8,9,10}, Flags \|= 0x4A, +0x9C 800, +0x9A -150, +0x9E 10, +0xA4 -100, Farbe 0x00101040 | **bestaetigt** + Ergaenzung | `bytes 0x80105680`: `02 03 04 07 08 09 0a`; `801044a4: ori v0,v0,0x4a`; `801044b8: sh t2(=800),156`; `801044bc: sh t1(=-150),154`; `801044c0: sh t0(=10),158`; `801044c4: sh a3(=-100),164`; `801044c8: sw a2(=0x00101040),112`. **Ergaenzung:** zusaetzlich `+0xA0 := 0` (`801044b4`), Part `+0x98 := Hund-Gier +0x76` (`801044ac`/`801044cc`), Hund `+0x1C0 \|= 1` (`80104458-64`) |
| Blut je Bild in 0x80104610 | **bestaetigt** + Ergaenzung | nach dem Phasen-`jalr` jedes Bild: `jal 0x80105070` (Part 3, FX 0) @0x8010464c, (Part 2, FX 0) @0x8010465c, (Part 2, FX 1 + rand&1) @0x80104678; Budget-begrenzt (+0x21F, Spawner `80105090: lbu v0,543(a3)` / `beq`, `8010518c-98: -1`) |
| DEATH Zeile 10: Gate +0x1D2 < 3 oder Zeile 16; +0x21F := 6; 6x FX 7; 17 Parts 0x00202020 | **bestaetigt** | `80104784: jal 0x80104178`; `8010478c-a8`; `801047b0: sb v0(=6),543`; Schleife `801047b8-d4` (s0 < 6, `jal 0x80105070` a2 = 7); `801047d8/e0: lui a0,0x20 / ori a0,a0,0x2020`; `801047f4: sw a0,112(v0)` / `801047f8: sltiu v0,s0,0x11` |
| DEATH Zeile 11: Gate +0x1D2 < 3; 17 Parts 0x3F2F; +0x21F := 2; FX 9 @rand&7, FX 10 @(rand&7)\|8 | **bestaetigt** | `80104830-3c`; `80104844: addiu a1,zero,16175` / `80104858: sw a1,112(v0)` / `8010485c: sltiu v0,a0,0x11`; `80104870: sb v0(=2),543(s0)`; `80104878-80: andi 7 / a2 = 9`; `80104890-9c: andi 7 / ori 8 / a2 = 10` |
| HURT-Tabelle @0x80105538 [9] 0x80103CE4, [10] 0x80103D9C, [11] 0x80103E60 | **bestaetigt** | HURT-Wurzel `801032b0-bc: +0x223 & 0x80 -> 0x801038c0` sonst `801032c4: lbu v0,5(a0)` / `801032d8: lw v0,21816(at)` |
| HURT Zeile 9/10: "+0x21F = 2 bzw. 4 - +0x1D2/3, **so oft** FX 8" / Tabelle §4 "**FX 8 x(4-K)**" | **WIDERLEGT (Anzahl)** | Schleife `80103d38: jal rand` / `80103d3c: addiu s0,s0,1` (Slot) / `80103d48: jal 0x80105070` / `80103d50: lbu v0,543(s1)` / `80103d58: sltu v0,s0,v0` / `bne`: der Spawner zieht +0x21F je Wurf ab (`8010518c-98`), der Zaehler s0 waechst -> Anzahl = **ceil(n/2)**, n = Startwert (n = 1 -> 1, 2 -> 1, 3 -> 2, 4 -> 2). Zeile 10: K0 -> 2, K1 -> 2, K2 -> 1 (erreichbar ist beim Hund nur K2 = 1 Wurf, K0/K1 toeten bzw. gibt es fuer Zeile 10 nicht). Zeile 9: K1 -> 1 Wurf. |
| HURT Zeile 10: +0x5 := 1, **+0x1D3 \|= 0x80** | **bestaetigt** + Folgerung | `80103e28: lbu v0,467(a0)` / `80103e30: sb v1(=1),5(a0)` / `80103e34: ori v0,v0,0x80` / `80103e3c: sb v0,467(a0)`. **Folgerung (fehlt im Dossier):** Bit 0x80 macht Gate 2 dauerhaft zu -> der Hund ist fuer JEDEN Applier-Treffer immun, bis ein `+0x1D3 &= 0x7F` laeuft (Hund-Overlay: @0x80100600, 0x80100890, 0x80100c00, 0x80102e3c, 0x80102fa0, 0x801030ac, 0x8010327c, 0x80103710) |
| HURT Zeile 11: 1 Part (rand&0xF) Farbe 0x3F2F, +0x21F := 1, FX 9 | **bestaetigt** | `80103e80: jal rand` / `80103e8c: andi a1,v0,0xf` / `80103ebc: addiu v1,zero,16175` / `80103ec0: sw v1,112(v0)` / `80103ec4: jal 0x80105070` / `80103ec8: sb s0(=1),543(s1)` (Slot) / `80103edc: sb s0(=1),5(a0)` |
| FX-Tabelle @0x801056AC, FX 7/8/9/10 | **bestaetigt** | `bytes 0x801056ac 84`: FX7 @0x801056d6 `85 03 00 00 00 10`, FX8 @0x801056dc `85 04 00 00 00 10`, FX9 @0x801056e2 `84 0f 00 00 00 14`, FX10 @0x801056e8 `84 0f 00 00 00 0c`; Spawner a1 = Part, a2 = FX (`801050a8: andi a0,a2,0xff` -> Tabelle; `801050dc: andi a0,s2,0x7f` -> Part) |

### 6.2 Kraehe 0x21 (EMOVL21_S0.BIN)

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| Zustandstabelle @0x80104908 [2] = [3] = 0x801028BC | **bestaetigt** | `table 0x80104908 6` |
| Dispatch 0x80104A18[+0x5]; 9/10/11 (und 5/6/17) -> 0x80102CA0 | **bestaetigt** | `801028f4: lbu v0,5(a0)` / `80102908: lw v0,18968(at)` / `80102910: jalr`; `table 0x80104A18 20`: [5],[6],[9],[10],[11],[17] = 0x80102ca0 |
| Sperre-Dekrement @0x80100160-74 | **bestaetigt** | `80100160: lbu v1,467(s0)` / `80100168: andi v0,v1,0x7f` / `8010016c: beq` / `80100170: addiu v0,v1,-1` / `80100174: sb v0,467(s0)` |

### 6.3 Spinne 0x25 / Baby 0x26 (EMS25.BIN)

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| HURT 0x80106518[+0x5] (@0x80102CD8), DEATH 0x801065A8[+0x5] (@0x80103CE0-FC) | **bestaetigt** | `80102cd8: lbu v0,5(a0)` / `80102cec: lw v0,25880(at)`; `80103ce0: lbu v0,5(s0)` / `80103cf4: lw v0,26024(at)` / `80103cfc: jalr`; Tabellen [9..11] = 0x80103800/0x801038e4/0x801039ac bzw. 0x8010493c/0x80104a5c/0x80104b88 |
| HURT 9: +0x5 := 7, 0x80102FDC | **bestaetigt** | `8010380c: addiu v0,zero,7` / `80103818: jal 0x80102fdc` / `8010381c: sb v0,5(s1)` (Slot) |
| HURT 11: Bein abschiessen | **bestaetigt** + Ergaenzung | `801039cc: jal rand` / `801039d4: andi a1,v0,0x7`; **Gates, die das Dossier nicht nennt:** nur bei `+0x221 >= 3` (`801039dc-e8: lbu 545 / sltiu 3 / bne -> 0x80103a98`) und wenn Bein k noch da ist (`801039f0-a00: lbu 544 / srlv / andi 1 / bne`); dann `80103a10-18: sllv / or / sb 544`, `80103a38/3c: -1 / sb 545`; Part (2k+4): Farbe 0x00101F3F (`80103a08/50/54`), Flags \|= 0x1062 (`80103a58/5c`); FX (2k+3, 7) immer (`80103a9c-a4`) |
| DEATH 9: Babys (rand&3)+6 | **bestaetigt** | `80104590: jal rand` / `8010459c: addiu a1,zero,8194` (0x2002) / `801045a0: andi v0,v0,0x3` / `801045a4: jal 0x80105d38` / `801045a8: addiu a2,v0,6` |
| DEATH 10: 20 Parts 0x00202F2F | **bestaetigt** | `80104b44: lui a1,0x20` / `80104b48: jal 0x8010609c` / `80104b4c: ori a1,a1,0x2f2f`; FUN_8010609C: Schleife `801060a4-bc` bis `sltiu v0,a2,0x14` (20), Stride `addiu v1,v1,172`, `sw a1,112(v0)` |
| DEATH 11: 20 Parts 0x00101F3F, Part 19 fliegt (Flags \|= 0x10, +0x9C/9D/9E = 100/100/90), FX (19,7)(0,6)(1,6), +0x239 := 1 | **bestaetigt** + Ergaenzung | `80104bc0/c4: lui a1,0x10 / ori a1,a1,0x1f3f`; `80104bcc/d0: sh 90,3426(v0)`; `80104be4/e8: sb 100,3424/3425`; `80104bec: ori v1,v1,0x10`; `80104bfc-c24: FX`; `80104c2c: sb v0(=1),569(s0)`. **Gate (fehlt):** nur bei `+0x6 == 0` UND `+0x224 == 0` (`80104ba4-bc`), sonst direkt generischer Tod 0x80103D30; Part 19 `+0x98 = +0x9A = 0` (`80104bdc/e0`) |

### 6.4 Zellenarm (RE2 0x2D, `build/extracted/re2_ems/CDEMD0_EM2D_ai1.BIN`, RE_OVERLAY_DIR)

| Behauptung | Urteil | Eigener Beleg |
|---|---|---|
| HURT 0x80100F50 / DEATH 0x80100FE0 ueber +0x1D2 -> 0x801014CC / 0x801014F0 | **bestaetigt** | `80100f58: lbu v0,466(a0)` / `80100f6c: lw v0,5324(at)`; `80100fe8: lbu v0,466(a0)` / `80100ffc: lw v0,5360(at)`; Tabellen: Spalten 0,1,3,4,6,7 -> 0x80100f8c bzw. 0x8010101c, Spalten 2/5/8 = 0 |
| INIT Wort0 \|= 0x0C000000, +0x1EE = 800 | **bestaetigt** | `80100338: addiu v1,zero,800` / `8010034c: sh v1,494(s0)`; `80100350: lui v1,0xc00` / `8010036c: or` / `80100374: sw v0,0(s0)` |
| Kein Sperren-Dekrement; "nur sb zero,467 im INIT und ori 0x80" | **bestaetigt (kein Dekrement)**, Aufzaehlung unvollstaendig | weitere Freigaben `801005b8: sb zero,467(s1)` und `8010061c: sb zero,467(s1)` (Zustandsmaschine), weitere `ori 0x80` @0x80100708, 0x80100df0, 0x80100e54. Ergebnis wie im Dossier: gesperrt bis ein Zustand freigibt |

## 7. RE1.5-Nebenbefund FUN_80012D60 (Gegner-Zweig) / Weg A

| Behauptung | Urteil | Eigener Beleg (`re15_disasm.py dis 0x80012f10 72`, `re15_dis_80012f10.txt`) |
|---|---|---|
| +0x7 := 0, +0x6 := 1, +0x5 := DAT_8006F430[Art], HP(+0x9A) -= DAT_8006F418[Art], +0x93 \|= 1, +0x4 := 2/3 | **bestaetigt** | `80012fd0: ori v0,zero,0x1` / `80012fd4: sb zero,7(s1)` / `80012fd8: sb v0,6(s1)` / `80012fdc/e0: lui at,0x8007 / addiu at,at,-3024` / `80012fe4: addu at,at,s0` / `80012fe8: lbu v0,0(at)` / `80012ff0: sb v0,5(s1)` / `80012ff4: lhu a0,0(s3)` (s3 = 0x8006F418 + 2*Art, `80012f14-20`) / `80012ffc: subu` / `80013000: sh v1,154(s1)` / `80013008/0c: ori 0x1 / sb 147` / `80013010-20: sb 2/3,4(s1)` (Byte-Store) |
| Bytes 0x8006F418 / 0x8006F430 | **bestaetigt** | `0a 00 14 00 e8 03 e8 03 e8 03 32 00 64 00 c8 00 2c 01 e8 03` (Art 2 = 1000) · `03 03 09 0a 0b 0e 0f 10 11 12 14` |
| +0x5 = RE1.5-Waffen-Id; Art 2 -> 9, 3 -> 10, 4 -> 11 | **bestaetigt** | wie oben; Port `re2z_row_from_weapon` (`enemy_ai_re2_zombie.c:3829-3834`): [9] = 9, [10] = 11, [11] = 10 |
| Port Weg A liefert Zeile 17 statt 9 | **bestaetigt** | `re15_enemy_take_damage` -> `re15_re2_stamp_hit(e, 1, type)` (`re15_damage.c:2981`) -> `re15_re2z_row_for_atktype` -> `re2z_row_from_atktype[11] = {1,1,17,17,17,9,9,10,11,17,1}` (`enemy_ai_re2_zombie.c:3847`), [2] = 17 |
| Einordnung "Id 9 in RE1.5 fertig" | **unklar / einschraenken** | Die ZUSTELLUNG (Routine 31 -> FUN_80012D60 Art 2) laeuft; die RE1.5-REAKTION auf `+0x5 = 9, +0x6 = 1` ist im Auslieferungsstand NICHT fertig: Schwester-Dossier `re_absturz_original.md` §2.5/2.7 misst den Haenger an `80106c00: jalr v0` mit v0 = 0 aus der STAGE1-Todestabelle 0x8011FEAC [Zeile 9][Spalte 1] = 0. Fuer die RE2-KI-Typen folgenlos (deren Reaktion ist RE2), aber die Aussage "fertig" gilt nur fuer die Zustellung; die Schadenszahl 1000 ist deshalb kein "Original-Verhalten, das je ohne Absturz erreicht wurde" (Auftrag Punkt 3). |

## 8. Port-Abgleich (Stichproben der Datei:Zeile-Angaben, nur gelesen)

| Dossier-Aussage | Urteil | Eigene Lesung |
|---|---|---|
| `game_step_common.c:1775` `ENT[9] = {1,1,1,0}` (resolve = 1), `:1779-1780` 10/11 resolve = 0, Aufruf `:1808-1809` | **bestaetigt** | Zeilen 1775/1779/1780/1808-1809 wie zitiert |
| `re15_damage.c:2648-2649` Sperre-Stempel, `:2653-2666` Klammer aus Sonde, `:2667` +0x5 = weapon_id, `:2683` +0x93 \|= 1, `:2700` +0x4 = 2/3 | **bestaetigt** | gelesen (2643-2700) |
| `re15_damage.c:2934-2940` Spalte aus Zielhoehe; `:2959-2992` Weg A; `:2973` 1000 flach; `:2976` react_table; `:2981` Stempel mit row_src = 1 | **bestaetigt** | gelesen (2925-2992) |
| `re15_damage.c:1005-1008` Reichweite [9] = 1000; `:394-396` Zombie-Zeile [9] = 100; `:751` Standardzweig | **bestaetigt** | gelesen |
| `enemy_ai_re2_zombie.c:3829-3835` / `:3847` Zeilen-Tabellen | **bestaetigt** | [9]=9, [10]=11, [11]=10; atktype [2]=17 |
| `enemy_ai_re2_zombie.c:6611-6621` Richtung aus Spieler-Peilung, `:6623-6632` Reserve-Abzug, `:7004` Aufruf | **bestaetigt** | `re2z_stamp_hit_row` nimmt `pl` (Spieler) als Quelle; Abzug je Treffer |
| `enemy_ai_re2_zombie.c:1493-1494` "edge-fall" Fehldeutung, `:8062-8066` kein Feld +0x236 | **bestaetigt** | Kommentar "WALK edge-fall death commits 0xA03/0xB03 … OPEN"; INIT-Block nennt +0x236 "GAR KEIN Feld" |
| "Reaktionsseite komplett portiert … Element-Leiter in allen Handlern, die sie im Original tragen" | **bestaetigt, aber Dossier-Begruendung falsch** | Der Port fuehrt den Liegend-Zweig bewusst als TOT (`:7052-7063`) — das Dossier dagegen fuehrt ihn als lebende Leiter (§5.2 hier). Der Port ist richtig, das Dossier nicht. |
| `enemy_ai_re2_dog.c:2037-2070` HURT 9/10/11 "portiert" | **teilweise WIDERLEGT (neue Port-Abweichung)** | `:2053-2056` und `:2061-2066`: `for (int i = 0; i < n; i++) re2d_fx(…, 8)` mit Budget n -> **n** FX 8. Original: **ceil(n/2)** (§6.1). Erreichbar mit GL: Hund + Nachbrenner (Zeile 10, K2, n = 2) -> Original 1 Wurf, Port 2 Wuerfe; dazu je Wurf zwei RNG-Zuege (Part `re15_re2_rand()` + Streuung in `re2d_fx` @0x801050D4) -> der RNG-Strom laeuft auseinander. Beim Hund mit Explosion K1 (Zeile 9, n = 1) gleich. |
| `enemy_ai_re2_dog.c:2152-2156` DEATH 10/11 auf den Kern gefallen; `:2159-2161` Teile-Wurf OPEN | **bestaetigt** | Kommentar "{10,16} -> 0x80104774, {11} -> 0x8010481C — alle UNPORTIERT"; Teile-Wurf "Render-seitig OPEN" |
| `enemy_ai_re2_spider.c:2280/2289` Farben OPEN | **bestaetigt** | "FUN_8010609C(self, 0x00202F2F) … OPEN" / "(self, 0x00101F3F) … OPEN"; Zeile-11-Tod traegt die Gates `+0x6 == 0 && +0x224 == 0` (`:2285`ff) |

## 9. Vollstaendigkeit — was der Bau braucht und im Dossier fehlt oder nur behauptet ist

1. **Box-Weiche Brand/Saeure (W1)** — Konstanten GL_BOX_WAND (Bedingung "Bit 0x80 = 0") und GL_BOX_GEGNER
   sind falsch; richtig: IMMER {-1200,0,600,300}; Bodenflammen nach jedem Brand-Aufschlag.
2. **Art-Zuordnung RE1.5 -> RE2 fehlt in der Konstanten-Tabelle.** GL_HITCODE_FLUG "0x30009 + Art (0 HE,
   1 Brand, 2 Saeure)" ist RE2-Art = RE2-Item-Id − 9 (`8001f1b4: addiu v0,v0,-9`). Die RE1.5-Ids sind
   VERTAUSCHT (0x0A = Acid, 0x0B = Incendiary; Namen Runde 30 §1): RE1.5 0x0A -> Art 2 (Op 49, Zeile 11),
   0x0B -> Art 1 (Op 48, Zeile 10). "Art = RE1.5-Id − 9" waere falsch. (Das Schwester-Dossier
   `re_saeure_brand.md` §3.1 nennt die Zuordnung; hier fehlt sie.)
3. **Welcher Zustellweg fuer welche Granate?** Das Dossier ist in sich uneinheitlich: §0 sagt "RE1.5 fuer die
   Zustellung" (Id 9), Bauhinweis 2 verlangt "Ereignisse je Art exakt nach §1.1/§1.2" inkl. Flug-Kontakt je
   Bild und Op 47. Die RE2-Flug-Kontakte (0x3000x, Box HINTER dem Pruefpunkt, Explosion beim ersten Kontakt)
   gehoeren zu einem geradeaus fliegenden Werfer-Geschoss; das Schwester-Dossier legt fuer alle drei Granaten
   den RE1.5-Wurf + Zuender (Explosion im Zuenderbild 7) fest. Beides zugleich ist kein Originalverhalten.
   Dazu: das RE2-Explosiv-Geschoss sind 5 Teilgeschosse (Handler 0x80044B44, fuenf `jal 0x8001bf10` mit
   0x020C0A00 @0x80044bf8/c78/cfc/d80/e04) — fuer eine Handgranate unbrauchbar. Entscheidung vor dem Bau.
4. **Spalte fuer Weg A nicht hergeleitet.** §3.3 rechnet die Zone nur fuer die RE2-Flug-Pruefpunkte. Fuer
   den RE1.5-Resolver-Weg (Punkt = Lage mit y−500, `800185a0-ac`) waere mit der RE2-Zonenregel eine am Boden
   liegende Granate (y ≈ Fuss-Y) P.y = Y−500 > Y−750 -> **Zone 0 (Beine) -> DEATH [9][0] = 0x80107438
   (Knockdown-Tod), nicht ZERREISSEN**. Ob die RE2-Regel auf den RE1.5-Punkt angewandt wird, ist eine
   Port-Zuordnung und muss im Bau benannt werden.
5. **Box-Erweiterung ueberlebt den Aufruf (E2)**, **Box-Test auf +0x84/+0x8C (E3)**, **Leerlisten-Waechter
   0x800CFBF3 (E1)**, **Takt 15 durch Reihenfolge Gegnerschleife vor FX-Pumpe (E5)** — fuer byte-true noetig,
   im Dossier nicht genannt.
6. **Flug-Lebensdauer und Nicht-Treffer-Explosionen:** 10..12 Aufrufe (Explosiv) bzw. 15 (Brand/Saeure),
   Wasser-Weg und Wand-Weg (§1.1 Ergaenzungen) — gehoert zur Zustellung, fehlt hier (ggf. im Schwester-Dossier).
7. **Liegende Zombies bekommen keinen Element-Effekt (W2)** — der Bau darf die Leiter im Liegend-HURT NICHT
   "reparieren" (der Port hat es richtig).
8. **Hund: Budget-Schleife ceil(n/2) (W3)** und **Immunitaet nach HURT Zeile 10 (+0x1D3 \|= 0x80)** — beides
   fehlt; die Port-Schleife ist abweichend.
9. **Hund Zeile-9-Tod: Budget := 1 vor dem FX-7-Wurf, Teile-Wurf zusaetzlich +0xA0 := 0, Part-+0x98 := Gier,
   Hund-+0x1C0 \|= 1; Blut je Bild = FX 0 (Part 3), FX 0 (Part 2), FX 1+rand&1 (Part 2)** — im Dossier nur
   "FX an Part 3 und 2".
10. **Spinne: Gates** HURT 11 (`+0x221 >= 3`, Bein k noch da) und DEATH 11 (`+0x6 == 0 && +0x224 == 0`) —
    fehlen im Dossier (der Port hat sie).
11. **Leichen-Ausblender:** Stopp bei Part-0-Farbbyte 16 und genau 15 Parts — fehlt.
12. **RNG:** FUN_80015FE8 liefert 0..255; alle "rand"-Formeln des Dossiers (Zucken, Babys (rand&3)+6, Wegschleudern
    3/4) haengen am RE2-RNG-Strom — der Port muss die Zuege in derselben Reihenfolge ziehen.
13. **Einordnung Id 9:** "fertig" gilt nur fuer die Zustellung; die RE1.5-Reaktion ist im Auslieferungsstand
    NULL (Haenger, `re_absturz_original.md` §2.7). Fuer RE2-KI-Typen folgenlos, fuer die Schadenszahl-Entscheidung
    (1000 flach vs. RE2-Record) aber ein Argument: 1000 wurde im Original nie ohne Absturz erreicht.
14. **O4 (Lage der Flug-Box)** — statisch weiter gestuetzt: FUN_800154AC liefert fuer R(θ)·(1,0,0) die Peilung θ
    (dx>0,dz=0 -> 0 @0x80015534-3c; dx=0,dz<0 -> 1024 @0x800154b0; dz>0 -> 3072 @0x800154e8), dieselbe
    Rotation 0x8008E1F4 baut FUN_800152C8 (Bewegung um +0x144) und FUN_80041EF8. Lokal +x = Blickrichtung,
    sofern "Gier = Blickpeilung" (gestuetzt durch die Port-Messung Clip 4 = Front / 3 = Ruecken bei
    `+0x1D0 & 0x20`). Box x in [-1400, +r] liegt damit hinter dem Pruefpunkt (Richtung Werfer). Rest-Annahme:
    diese Blickkonvention — Laufzeitmessung bleibt der Abschluss.

## 10. Urteil je Zeile der Tabelle "KONSTANTEN FUER DEN BAU"

| Name | Urteil | Korrektur / Anmerkung |
|---|---|---|
| GL_HITCODE_FLUG | bestaetigt | + Art = RE2-Item − 9; RE1.5 0x0A -> Art 2, 0x0B -> Art 1 (§9.2) |
| GL_FLUG_Y1 / Y2 | bestaetigt | — |
| GL_FLUG_BOX | bestaetigt | x-Erweiterung nur nach +x (p0 fest) |
| GL_HITCODE_EXPLOSION | bestaetigt | — |
| GL_EXPL_GATE | bestaetigt | +0x1E = volles SUB-Byte (FUN_8001BF10 @0x8001bfb0-c0) |
| GL_EXPL_Y | bestaetigt | — |
| GL_EXPL_BOX | bestaetigt | — |
| GL_HITCODE_BRAND | bestaetigt (Wert) | zweites Adresspaar @0x800214F0-FC liegt im TOTEN Zweig -> streichen |
| GL_BRAND_PUNKT | bestaetigt | @0x80021518/@0x80021528 im toten Zweig -> streichen; sp+20 haelt das ALTE +0x64 |
| GL_HITCODE_SAEURE | bestaetigt | — |
| GL_SAEURE_Y | bestaetigt | — |
| GL_BOX_WAND | **widerlegt (Bedingung)** | Wert richtig; gilt IMMER (Status wird vor dem Test auf 0x8000 bzw. 0x8403 gesetzt, @0x80020ff4-f8 / @0x80021694-98) |
| GL_BOX_GEGNER | **widerlegt** | nie benutzt (toter Zweig in Op 48 @0x800214e8 und Op 49 @0x800216dc) -> streichen |
| GL_HITCODE_NACHBRENNER | bestaetigt | — |
| GL_NACHBR_BOX / Y | bestaetigt | "Status &= ~2" = Step-Status +0x12 |
| SE_EXPLOSION / BRAND / SAEURE | bestaetigt | — |
| APPLIER_LISTE | bestaetigt | + Leerlisten-Waechter 0x800CFBF3 (@0x800470c4 / @0x8004710c) |
| APPLIER_GATES | bestaetigt | — |
| APPLIER_BAND | bestaetigt | — |
| APPLIER_RADIUS | bestaetigt | + Erweiterung ueberlebt auch den AUFRUF (Einzel- und Alle-Modus), zweiter Aufruf derselben Op sieht sie |
| APPLIER_SCHADEN | bestaetigt | Zeilenindex = Hitcode & 0xFFFF (@0x80047214) |
| APPLIER_ZUSTAND | bestaetigt | — |
| APPLIER_ZONE | bestaetigt | Kopf-Regel liegt im 0x20000-Zweig |
| APPLIER_ZEILE | bestaetigt | — |
| APPLIER_SPERRE | bestaetigt | + Kraehe @0x80100160-74; Takt 15 (Gegnerschleife vor FX-Pumpe) |
| APPLIER_RICHTUNG | bestaetigt | Winkelfunktion kuerzt alle Koordinaten auf s16 |
| SCHADEN_ZOMBIE / ZOMBIE16 / HUND / KRAEHE / SPINNE / ARM | bestaetigt | alle Bytes selbst gelesen (`records.txt`) |
| ZOMBIE_HURT_TAB | bestaetigt | — |
| ZOMBIE_DEATH_TAB | bestaetigt | — |
| ZOMBIE_DOT_TAKT | bestaetigt | zweiter Standort ist EXEC[2] (zweiter Gang), nicht "Anrempeln" |
| ZOMBIE_DOT_ZAEHLER | bestaetigt | — |
| ZOMBIE_DOT_TOD | bestaetigt | — |
| ZOMBIE_DOT_ZUCKEN | bestaetigt | RNG 0..255 -> Saeure-Zucken [-63,+64] |
| ZOMBIE_ELEMENT | **widerlegt (Beleg-Stelle)** | die zitierte HURT-Stelle @0x80105188-208 ist TOT (+0x5 = 5 nach `sw 0x00060501` @0x80105184). Lebende HURT-Stellen: STAGGER P0 @0x80105DC4-F18, Kriecher-Treffer @0x80107960-9B0, Aufsteh-Treffer @0x80107BE0-CE0 |
| HUND_TOD_BRAND | bestaetigt | — |
| HUND_TOD_SAEURE | bestaetigt | — |
| HUND_TOD_HE | bestaetigt | + Budget 1 vor FX 7 (@0x80104708), +0xA0 := 0, Part-+0x98 := Gier, +0x1C0 \|= 1 |
| HUND_HURT_SAEURE | bestaetigt | — |
| HUND_FX_TAB | bestaetigt | — |
| SPINNE_TOD_FARBE | bestaetigt | + Gate Zeile 11: +0x6 == 0 && +0x224 == 0 (@0x80104ba4-bc) |
| RE15_ART_ZU_WAFFE | bestaetigt | — |

Mechanismus-Aussagen ausserhalb der Tabelle: widerlegt sind (a) die Status-Weiche Brand/Saeure (§1.3/1.4),
(b) die Leiter im Liegend-HURT und die Aussage "im Liegen Verkohlung/Aetzung + DoT" (§5.2),
(c) "FX 8 so oft / x(4−K)" beim Hund (§6.1); Benennung "Anrempel-Executor" falsch (§5.3).
Alles uebrige in §1/§2/§5/§6/§7 ist mit eigenen Bytes bestaetigt.

**Fazit:** Das Dossier ist in der Applier-Mechanik, den Records, den Zombie-/Hund-/Kraehen-/Spinnen-Tabellen
und dem neu gefundenen Zombie-DoT byte-genau. Drei Behauptungen, die in Code wandern wuerden, sind
widerlegt (tote Box-Weiche Brand/Saeure, tote Liegend-Leiter, Hund-FX-Zaehlung); vor dem Bau zu klaeren
sind die Zustellweg-Frage (RE1.5-Wurf/Zuender vs. RE2-Flug-Kontakt, §9.3), die Art-Zuordnung der
vertauschten Ids (§9.2) und die Spalte fuer Weg A (§9.4).
