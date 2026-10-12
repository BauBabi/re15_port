# Runde 37 — Spur G "beretta": UNABHAENGIGE ABNAHME 0 (2026-10-12)

Baum `.claude/worktrees/r37_beretta`, Zweig `r37/beretta`, Kopf 4a5571df (Basis = merge-base f1e019ca; master ist
inzwischen weiter, Spur-Diff daher ueber `git diff master...HEAD`). Eigener Bau: `local_build.sh configure` + `build`
-> `LOCAL-BUILD-OK`, `ninja: no work to do` (exe 16:08:31, alle Quellen der Spur aelter: beretta_r37.c 15:28,
re15_beretta_r37.h 15:59, game_step_common.c 15:28, re15_damage.c 15:28; Baum sauber bis auf unversioniertes
`mess_r37g/`). Gemessen wird diese exe; Messkopie `re15_abn0g.exe` (eigener Name), Logs im Scratchpad der Abnahme
(Skript `abn_run.sh` = Aufbau wie `G_beretta_mess/run_mess.sh`, Auswertung mit `G_beretta_mess/auswertung.py`).

## Ergebnis: NICHT BESTANDEN
| Pruefung | Urteil |
|---|---|
| Punkt 1 (Beretta M93R: jeder Schuss des Feuerstosses trifft) | **teilweise** — Zombies (Leon/Elza, beide KI-Modi), Spinne/Hund (RE2-Pause 0) und alle Typen unter RE1.5-KI: drei Treffer E/E+3/E+6. Unter der STANDARD-KI (RE2) treffen die Gegner mit RE1.5-Gehirn weiter nur EINMAL (an der exe gemessen 0x2b ROOM5040 und Kakerlake 0x29 ROOM3040; Gorilla 0x27, Birkin 0x30, Alligator 0x23 gleiche Code-Lage, Sonde 1f); dort schadet der Stoss jetzt sogar weniger als vorher (gemessen 0x2b 20 -> 10, 0x29 25 -> 14) |
| RE-Gate (Konstanten mit @0x, 7 Stellen selbst disassembliert) | haelt |
| Vertrag (Pfade, Ressourcen, Haken, Tests nur ueber probes/) | haelt (Testluecke s. M3) |
| Gezielte Tests (eigener Lauf) | 47/47 Unit (Regex des Bau-Agenten ohne integration_*), 43/43 Gegner-Typ-Tests, 5/5 exe-Integration (inkl. integration_r37_beretta) — alle gruen, keiner deckt M1/M2 ab |

Maengel: M1 (Standard-KI, RE1.5-Gehirn-Gegner 1 Treffer), M2 (Schadens-Regression dieser Gegner), M3 (Testluecke),
M4 (Dossier/Header falsch) — Abschnitt 4. Die M93R/Glock ist im Spiel ueber das Original-Item-Debug des Inventars
(menu_common.c:1267ff) fuer jeden Raum erhaeltlich; die betroffene Klasse ist also im Spiel erreichbar.

## Punkt 1 (Nachtrag, Wortlaut)
> bei der Beretta M93R muss - wenn sie schon mehrfach schießt, das auch gelten das die Gegner mehrfach getroffen werden.
> Werden sie nur ein einziges mal getroffen bei 3 Schuss, ist es nur Schussverschwendung.

### 1a. Messung ROOM1140, Leon, Ziel Zombie Platz 5 (Typ 0x11) — eigene Laeufe
Befehl je Lauf: `RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS=200,-18200,1024 RE15_GIVE=<w>:15 RE15_EQUIP=<w>
RE15_INPUT_SCRIPT=M2.0,MA0.1,M3.0,W2 (BASIS=spiel, START=1) RE15_STATE_LOG/WAFFEN_LOG/FX_LOG RE15_EXIT_AT=260#1140`,
`RE15_AI_FLAVOR=re2|re15` bzw. ohne (Standard). Alle Laeufe `exit=0`, `[flow] EXIT_AT: Bild 260 in Raum 1140`.

| Lauf | Munition | HP-Abzuege Platz 5 | Muendung (wf SPAWN id=2 sub=0) | Blut id=0 Alter 1 |
|---|---|---|---|---|
| l_w5_re2 (M93R, RE2) | F61 15 -> 12 | F61 -16, F64 -16, F67 -16 (250 -> 202) | F61, F64, F67 | F61, F64, F67 |
| l_w5_default (M93R, ohne RE15_AI_FLAVOR) | F61 15 -> 12 | F61/F64/F67 je -16 | F61, F64, F67 | F61, F64, F67 |
| l_w6_re2 (Glock 18, RE2) | F61 15 -> 12 | F61/F64/F67 je -16 | F61, F64, F67 | F61, F64, F67 |
| l_w5_re15 (M93R, RE1.5-KI) | F61 15 -> 12 | F61/F64/F67 je -16 (st=2 ss1=5 je Treffer) | F61, F64, F67 | F61, F64, F67 |
| l_w6_re15 (Glock 18, RE1.5-KI) | F61 15 -> 12 | F61/F64/F67 je -16 (ss1=6) | F61, F64, F67 | F61, F64, F67 |
| l_w3_re2 (Browning, Gegenprobe) | F61 15 -> 14 | F61 -16 (einer) | F61 | F61 |
=> Gegen den Zombie trifft jede der drei Kugeln (Bild E, E+3, E+6), Treffer-Reaktion und Blut je Kugel, im Takt der drei
Muendungsblitze; Browning unveraendert. Deckt sich mit Dossier §4.

### 1b. Messung Elza ROOM11E1 (Spielerwahl, RE15_PSELECT_AUTO) — eigene Laeufe
`ELZA=1 RE15_DEBUG_JUMP=11E0@250 RE15_PLAYER_POS=-10500,-11000,1024 RE15_INPUT_SCRIPT=M3.0,MA0.1,M3.0,W2 EXIT=220#11E1`;
debug.log `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `loaded room11e1.rdt`.
| Lauf | Munition | Treffer |
|---|---|---|
| e_w5_default (M93R, Standard-KI) | F91 15 -> 12 | Platz 1: F91 -16, F94 -16, F97 -16 (58 -> 10) |
| e_w5_re15 (M93R, RE1.5-KI) | F91 15 -> 12 | F91 Platz 1 -16, F94 Platz 2 -16, F97 Platz 3 -16 (der erste faellt, die naechste Kugel nimmt den naechsten) |
| e_w3_default (Browning) | F91 15 -> 14 | Platz 1 F91 -16 (einer) |

### 1c. Messung NICHT-Zombie-Gegner unter der STANDARD-KI (RE2-Flavor) — ⛔ Befund
Code-Lage (gelesen): `re15_ai_re2_for_type()` liefert im RE2-Flavor fuer JEDEN Typ 1 (enemy_ai_re2_zombie.c:173-174
`case RE15_AI_FLAVOR_RE2: return 1;`). `riegel_loesen()` (beretta_r37.c:89) ueberspringt damit im Standard-Modus ALLE
Gegner. Der RE2-Pausenfilter, der dort den Riegel +0x93 Bit 0 freigeben soll, laeuft aber nur fuer die Zombie-Familie
(`re15_re2z_hit_filter_apply`) und die Typen 0x20/0x21/0x25/0x26 (`re15_re2_pause_filter_apply`, re15_damage.c:3925;
Aufruf game_step_common.c:2384-2395). Fuer die Typen mit RE1.5-Gehirn und RE1.5-Schadenszeile — 0x23 Alligator,
0x27 Gorilla, 0x29 Kakerlake, 0x2b ("tyrant"), 0x30/0x36 Birkin (re15_damage.c:752-768) — gibt im Standard-Modus nur
deren eigene Hurt-Kette den Riegel frei (z.B. `e->hit_react &= ~1u` am Ende des HURT 0x80114770, enemy_ai_common.c:13849,
nachdem Phase 0 `+0x93 |= 2` gesetzt hat @0x801148a4 = enemy_ai_common.c:13839; Kakerlake `hit_react = 0` @0x801149ac = enemy_ai_common.c:11482), also NACH Kugel 2/3.

Messung ROOM5040 (B-2 CORRIDOR), Ziel Platz 1 Typ 0x2b (hp 121) bei (-14493,-14685), Leon (-14493,-13185) Blick -z:
`JUMP=5040 RE15_PLAYER_POS=-14493,-13185,1024 RE15_GIVE=<w>:15 RE15_EQUIP=<w> RE15_INPUT_SCRIPT=M2.0,MA0.1,M3.0,W2
EXIT=200#5040`. "basis" = exe des Hauptbaums (gebaut 2026-10-12 09:02:00; letzter Engine-Commit davor, danach bis
f1e019ca nur Test-Commits 6fe54ff3/7f0fa8bb -> Engine-Stand = Basis der Spur), als Kopie `re15_abn0g_basis.exe`.
| Lauf | exe | KI | Munition | Treffer auf 0x2b | Muendung |
|---|---|---|---|---|---|
| basis_t2b_w5_def | Basis (vorher) | Standard (RE2) | F61 15 -> 12 | F61 -20 (EINER, Spalte 5) | F61/F64/F67 |
| basis_t2b_w5_re15 | Basis (vorher) | RE1.5 | F61 15 -> 12 | F61 -20 (EINER) | F61/F64/F67 |
| **t2b_w5_def_a** | **Spur (nachher)** | **Standard (RE2)** | F61 15 -> 12 | **F61 -10 (EINER)** — F64/F67 nichts, HURT 2/5 bis F70 | F61/F64/F67 |
| t2b_w5_def_b (Leon (-12993,-14685) Blick -x) | Spur | Standard (RE2) | F61 15 -> 12 | **F61 -10 (EINER)** | F61/F64/F67 |
| t2b_w5_re15_a | Spur | RE1.5 | F61 15 -> 12 | F61/F64/F67 je -10 (drei) | F61/F64/F67 |
| t2b_w3_def_a (Browning) | Spur | Standard | F61 15 -> 14 | F61 -10 (einer) | F61 |
=> Im Standard-Modus trifft der Feuerstoss diesen Gegner weiterhin nur EINMAL (3 Patronen, 3 Blitze, 1 Treffer = genau
die "Schussverschwendung" des Wortlauts) — und weil der Schaden je Kugel jetzt Spalte 3 ist, schadet der ganze Stoss
nur noch so viel wie EIN Browning-Schuss (-10 statt vorher -20). Fuer diese Gegnerklasse ist die Spur im Standard-Modus
eine REGRESSION gegenueber der Basis. Nur mit RE15_AI_FLAVOR=re15 (nicht Standard) treffen alle drei Kugeln.

Zweiter Typ, ROOM3040 (FACTORY LINE A), Ziel Platz 2 Typ 0x29 Kakerlake (hp 99) bei (-13500,-11800), Leon
(-13500,-10300) Blick -z, sonst wie oben (`JUMP=3040 EXIT=200#3040`):
| Lauf | exe | KI | Munition | Treffer auf 0x29 |
|---|---|---|---|---|
| basis_roach_w5_def | Basis (vorher) | Standard | F61 15 -> 12 | F61 -25 (EINER, Spalte 5) |
| **roach_w5_def** | **Spur** | **Standard** | F61 15 -> 12 | **F61 -14 (EINER)**, F64/F67 nichts, HURT 2/5 bis F73 |
| roach_w5_re15 | Spur | RE1.5 | F61 15 -> 12 | F61/F64/F67 je -14 (drei) |
=> Dieselbe Klasse: im Standard-Modus 1 Treffer je Stoss, Schaden je Stoss 25 -> 14 (RE1.5-Zeile 0x29 @0x8006e0d0 +
0x29*0x58: Spalte 3 = 14, Spalte 5 = 25, selbst aus PSX.EXE gelesen).

### 1d. Krähe / Hund / Spinne unter RE2 (Code gelesen, nicht eigens an der exe gemessen)
RE2-Pause Zeile 4 (`s_re2_stun_*[5]`, re15_damage.c:568-581): Zombie 1, Hund 0, Spinne 0, Kraehe 15. Die Kraehe hat
im RE2-Modell 10 HP (`case 0x21: return 10;` re15_damage.c:956, @0x80100324/348) und nimmt je Kugel 15
(`s_re2_wpn_dmg_crow` w5 = 15) — sie stirbt an Kugel 1, die Pausen-Ausnahme des Dossiers (§7) laeuft also ins Leere
(kein verschenkter Treffer an einer lebenden Kraehe). Hund/Spinne: Pause 0 -> jede Kugel trifft. Pruefung "Spalte 5
== Spalte 3" der RE2-Zeilen des Ports (selbst ausgelesen): Zombie 16/16, Zombie 0x16 11/11, Hund 18/18, Kraehe 15/15,
Babyspinne 17/17 — die Damage-Zeile ist dort wirklich ein No-op.

### 1e. Bedienvarianten und Restmunition (Spur-exe, Standard-KI, ROOM1140 Zombie)
| Lauf | Eingabe | Ergebnis |
|---|---|---|
| l_w5_def_loslassen | `M2.0,MA0.1,W3` (R1 ab F64 los) | F61/F64/F67 je -16 — Rueckstoss-Abbruch frisst keine Kugel |
| l_w5_def_halten | `M2.0,MA2.0,M1.0,W2` (Abzug gehalten) | Stoesse F61, F82, F103 je drei Treffer (+0/+3/+6), Munition 15 -> 12 -> 9 -> 6 |
| l_w5_def_mag2 | RE15_GIVE=5:2 | Munition 2 -> 0, Treffer F61, F64 (zwei), Blitze F61/F64/F67 |
| l_w5_def_mag1 | RE15_GIVE=5:1 | Munition 1 -> 0, Treffer F61 (einer), Blitze F61/F64/F67 |

### 1f. Deterministische Sonde (Mechanik ohne exe) — Quelltext im Anhang A
Aufbau wie `test_r37_beretta.c` (ROOM1140.RDT, Leon (200,-18200) Blick -z, Ziel (200,-19600), HP 2000), je Bild erst
die RE2-Filter wie game_step_common.c:2384-2395, dann `re15_beretta_r37_tick_ex(1)`; KEINE Gegner-KI-Ticks.
Bau: `gcc -std=c11 -DRE15_ASSETS_PATH=<baum>/re15_port/shared_assets/PSX -DRE15_PLATFORM_PC=1 -I<baum>/re15_port/include
abn_sonde_g.c <build>/engine/libre15_engine.a <build>/tests/libre15_test_support.a -lm -lkernel32 ...` (Linkzeile von
test_r37_beretta aus build.ninja). Ergebnis:
```
RE2   Typ 0x23: Treffer 1 (Bilder 0)     Stoss -7      RE1.5 Typ 0x23: Treffer 3 (0,3,6) Stoss -21
RE2   Typ 0x25: Treffer 3 (Bilder 0,3,6) Stoss -51     RE1.5 Typ 0x25: Treffer 3          Stoss -30
RE2   Typ 0x27: Treffer 1 (Bilder 0)     Stoss -12     RE1.5 Typ 0x27: Treffer 3          Stoss -36
RE2   Typ 0x29: Treffer 1 (Bilder 0)     Stoss -14     RE1.5 Typ 0x29: Treffer 3          Stoss -42
RE2   Typ 0x2b: Treffer 1 (Bilder 0)     Stoss -10     RE1.5 Typ 0x2b: Treffer 3          Stoss -30
RE2   Typ 0x30: Treffer 1 (Bilder 0)     Stoss -15     RE1.5 Typ 0x30: Treffer 3          Stoss -45
```
(Zombie 0x10 und Kraehe 0x21 zeigen in der Sonde unter RE2 ebenfalls 1 Treffer, weil ihre RE2-Pause erst im KI-Tick
abgebaut wird — an der exe trifft der Zombie dreimal (1a); die Kraehe stirbt an Kugel 1 (1d). Hund 0x20 wird mit
geradem Zielen in dieser Sonde nicht getroffen — kein Befund der Spur.) Fuer 0x23/0x27/0x30 ist das Ergebnis der
Sonde dieselbe Code-Lage wie fuer die an der exe gemessenen 0x29/0x2b (kein Filter gibt den Riegel frei,
`riegel_loesen` uebersprungen); an der exe einzeln gemessen sind nur 0x2b und 0x29.

Umfang der betroffenen Klasse (eigener Zensus der Sce_em_set-Saetze 0x44, Typ auf +2, RE1.5-Satz 20 B, LE-Lage +8..+13,
ueber alle `shared_assets/PSX/STAGE*/ROOM*.RDT`): Gorilla 0x27 in 11C0/1141 (8 Saetze), Kakerlake 0x29 in 15 Raeumen
(3040/3041/4000/4040/4041/4080/4081/5000/5001/5031/50A0/50A1/50B0/5100/5101, 32 Saetze), Typ 0x2b in 9 Raeumen
(4001/5040/5041/5070/5071/5110/5111/5130/5131, 28 Saetze), Birkin 0x30 in 6 Raeumen (3070/3071/5080/5081/50E0/50F1; die Saetze in 5090/5091 werden zum
G5-Modul 0x36 umgetypt, scd_vm.c op_sce_em_set — eigenes RE2-Modul, hier nicht geprueft). Das ist ein grosser Teil der Gegner der Stages 3-5 — dort bleibt der Feuerstoss im
Standard-Modus "Schussverschwendung".

### 1g. Urteil Punkt 1: **teilweise**
Erfuellt fuer Zombies (Leon und Elza, beide KI-Modi, an der exe gemessen), Spinne (RE2-Pause 0, Sonde 3 Treffer),
Hund (RE2-Pause 0, nur Code gelesen) und alle Typen unter RE1.5-KI (exe 0x2b/0x29, Sonde alle);
die Kraehe stirbt an Kugel 1. NICHT erfuellt fuer die Gegner mit RE1.5-Gehirn unter der STANDARD-KI (RE2-Flavor):
gemessen 0x2b (ROOM5040) und 0x29 (ROOM3040) je Stoss 1 Treffer bei 3 Patronen — und durch die Schadensspalte 3
schadet der Stoss dort weniger als vor der Spur (0x2b 20 -> 10, 0x29 25 -> 14, Basis-exe gegen Spur-exe gemessen).
Der Wortlaut ("die Gegner mehrfach getroffen ... sonst nur Schussverschwendung") gilt fuer alle Gegner.

## 2. RE-Gate
Selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py` des Hauptbaums,
Binaerdateien `info/Re1.5/PSX.EXE` bzw. `info/re2leon/PSX.EXE`; Sprungziele mit-disassembliert):
1. RE1.5 Entlade-Tabelle `table 0x80074100`: [5] 0x80074114 -> 0x800338a8, [6] 0x80074118 -> 0x800338a8 (Browning
   [3]/[4] -> 0x800337bc). Handler `dis 0x800338a8`: drei `jal 0x80019700` (a0 = 0x02020800 / 0x03020c00 /
   0x04020800), `lbu a0,-13731(a0)` (0x800aca5d) / `lw a1,124(v0)` / **`jal 0x80011f50` @0x8003396c (einmal)**,
   **`jal 0x8004eae4` @0x80033974 / 0x8003397c / 0x80033984**, `jr ra` @0x8003399c. Bestaetigt.
2. Sub-2-Latch `dis 0x80033460`: `lbu v0,0(v1)` (0x800aca5b) / `bne v0,zero,0x80033508` @0x80033470-78, `sb v0` (=1)
   @0x80033480, `jalr v0` ueber 0x80074100+id*4 @0x800334e0; zweiter Resolve nur fuer Waffe 8 (`ori v0,zero,0x8` /
   `bne` @0x80033510-14, `jal 0x80011f50` @0x80033554). Bestaetigt.
3. Riegel `dis 0x800124c0`: `lbu v0,147(s1)` / **`ori v0,v0,0x1` @0x800124F0** / `sb v0,147(s1)` @0x800124f8;
   Schaden `lhu v1,0(v1)` aus Basis + Typ*0x58 + Waffe*4, `subu` von +0x9a; `bgez` -> +0x4 = 2 sonst 3
   @0x80012520-38. `dis 0x800123e8`: `andi v0,v1,0x1` / `beq` / **`ori v0,v1,0x2` @0x8001240c** / `sb` /
   `jal 0x80011f50` @0x80012418 (Rueckgriff). Bestaetigt.
4. RE2 Custom Handgun: `table 0x800A6F38` [4] 0x800a6f48 -> 0x8004879c; `table 0x800A7098` [2] -> 0x800487d8.
   `dis 0x800488d8`: `beq v1,zero,0x80048958` @0x80048910, `addiu v0,zero,3` / `beq v1,v0` @0x80048914-18,
   `addiu v0,zero,6` / `bne v1,v0,0x800489c4` @0x8004891c-20, `andi v0,s2,0x40` @0x80048928, `jal 0x80069f54`
   @0x80048938 / `bne v0,zero,0x80048958` @0x80048940, Kugel: `jal 0x80041098` @0x80048990, `jal 0x8006a0cc`
   @0x800489a0, `jal 0x80040350` @0x800489b0, `jal 0x80043d30` @0x800489b8; Resolver `jal 0x800410cc` @0x80048a04.
   Bildzaehler `dis 0x80029b10`: `lbu v0,333(s2)` / `addiu v0,v0,1` / `sb v0,333(s2)` @0x80029b28-34,
   `sb zero,333(s2)` @0x80029b48. Bestaetigt.
5. RE2-Zeilen `bytes 0x800A412C`: @0x800A4154 `10 3c e0 00 | 14 0a 85 02`, @0x800A4168 `10 3c e0 00 | 14 02 81 00`
   -> Schaden 16/15/14 gleich, Pause (w1>>9)&0x7f = 5 bzw. 1. Bestaetigt.
6. CORE00.ESP (shared_assets == info/Re1.5, `cmp`): Effekt 2 Sub 2 Zeilen @0x1188/0x11B0/0x11D8 = A 15, +0x0E 0/2/2,
   +0x16/+0x17 = 0/2, +0x26 1/1/0 (ebenso Effekt 3 @0x44C/474/49C, Effekt 4 @0x1988/19B0/19D8). Routine 15
   `dis 0x80017ac8`: `lhu v0,14(a3)` / `beq v0,zero,0x80017b10` @0x80017aec-f4, `addiu v0,v0,-1` / `sh` @0x80017b04-0c,
   Kind `jal 0x800199d4` @0x80017b38, `lhu v0,38(v1)` != 0 -> `jal 0x800174e4` @0x80017b4c-5c. Bestaetigt.
7. RE1.5-Schadenstabelle aus PSX.EXE (u16 @0x8006e0d0 + Typ*0x58 + w*4): 0x10 = 5/15, 0x27 = 12/28, 0x29 = 14/25,
   0x2b = 10/20, 0x30 = 15/28, 0x23 = 7/21 (Spalte 3 / Spalte 5). Bestaetigt.
Guess-Tells in den HINZUGEFUEGTEN Zeilen (`git diff master...HEAD -- re15_port | grep '^+'` auf deferred|tunable|interim|
for now|faithful|plausib|TODO|FIXME|approx|getenv): **keine**. Konstanten: `BERETTA_KUGEL_ABSTAND 3` (@0x80048914/1c,
ESP 0x11B0/0x11D8 + Routine 15), `BERETTA_KUGELN 3` (@0x80033974/7c/84), Waffen 5/6 (@0x80074114/18), Riegel-Bits 0|1
(@0x800124F0 / @0x8001240c), HP<0 (@0x80012520-2c), Schaden `dmg_row[3]` (NUTZER-VORGABE + RE2 @0x800A4154/68) — alle
belegt bzw. gekennzeichnet. `& 0x10` ist die Port-Kodierung von `s_aim_recoil` (player_common.c:520), keine
Original-Konstante. **RE-Gate: haelt.**
Erklaert der Fix den Befund? Der Vorher-Befund (1 Treffer je Stoss) ist durch den EINEN Resolve @0x8003396c erklaert
(Original-GDB-Log `G_beretta_mess/orig_w5/log.txt`: Resolver 1x in B41, HP 105 -> 90, Kinder B41/B44/B47, +0x93 = 01
bis B50). Der Fix behebt ihn aber nur dort, wo der Riegel vor Kugel 2/3 frei wird; die Freigabe haengt am falschen
Kriterium (`re15_ai_re2_for_type` statt "fuer diesen Aktor laeuft ein RE2-Pausenfilter") — s. Mangel M1.

## 3. Vertrag (VERTRAG.md)
* Pfade (`git diff master...HEAD --name-only`, master ist seit der Basis nur um Doku/Orchestrator-Commits weiter):
  analysis/befunde_runde37/G_beretta*, engine/src/beretta_r37.c (neu, 122 Z.), include/re15_beretta_r37.h (neu, 82 Z.),
  engine/src/game_step_common.c (+8/-3: Include, Tick-Zeile, Rueckgaben merken, Feuerstoss-Aufruf; Kommentar
  "Runde 37 Spur G"), engine/src/re15_damage.c (+6: 1 Codezeile + Kommentar), tests/unit/probes/r37_beretta.cmake,
  tests/unit/test_r37_beretta.c, tests/integration/test_r37_beretta.cmake (neu), tests/unit/test_re2_hp_model.c (+5/-1).
  Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists. game_step_common.c steht in §1.4 nicht
  woertlich bei G, traegt aber den Entlade-Block (= Schuss-/Waffen-Code) — kleine Haken, haelt.
* Bank-9-Bits / Nachrichten-Ids / Ereignisse / Slots: keine belegt (Diff-Suche). Haelt.
* Neue Logik in neuen Dateien (beretta_r37.c/.h). Haelt.
* Tests nur ueber `probes/r37_beretta.cmake`; Integration mit `cmake_minimum_required(VERSION 3.16)`; `file(COPY_FILE)`
  wie in test_r34n_b_cursor.cmake u.a. etabliert; Unit-Test ohne POSIX-Aufrufe. Haelt.
* `test_re2_hp_model.c`: Erwartung fuer 5/6 auf Spalte 3 umgestellt — Folge der gewollten Schadensaenderung, begruendet.
  Die Umstellung entfernt aber zugleich die einzige Wache, die den Schaden 5/6 gegen die RE1.5-Zeilen pinnte; die
  Regression aus 1c bleibt dadurch von der ganzen Suite unbemerkt (s. M3).
* Tests je Punkt: vorhanden (unit_r37_beretta, integration_r37_beretta) und messend — aber nur Zombie/Hund unter
  RE1.5-KI bzw. Zombie unter RE2; die Klasse "RE1.5-Gehirn unter Standard-KI" fehlt (M3).

## 4. Maengel (nummeriert, nachpruefbar)

**M1 — Standard-KI: der Feuerstoss trifft Gegner mit RE1.5-Gehirn weiterhin nur EINMAL (Wortlaut nicht erfuellt).**
Gemessen an der Spur-exe ohne RE15_AI_FLAVOR (= RE2, enemy_ai_re2_zombie.c:105): ROOM5040 Typ 0x2b 1 Treffer (F61 -10),
ROOM3040 Typ 0x29 1 Treffer (F61 -14), jeweils Munition 15 -> 12 und Blitze F61/F64/F67 (1c). Dieselben Szenen mit
RE15_AI_FLAVOR=re15: drei Treffer. Ursache: `riegel_loesen()` ueberspringt jeden Aktor mit
`re15_ai_re2_for_type(type)` (beretta_r37.c:89) — im RE2-Flavor ist das fuer JEDEN Typ wahr (enemy_ai_re2_zombie.c:174).
Die als Ersatz genannte RE2-Trefferpause (+0x1D3) wird aber nur fuer die Zombie-Familie und 0x20/0x21/0x25/0x26
nachgerechnet (re15_damage.c:3925, game_step_common.c:2384-2395); 0x23/0x27/0x29/0x2b/0x30 behalten den RE1.5-Riegel
+0x93 bis zum Ende ihrer Hurt-Kette (Tyrant-Zweig enemy_ai_common.c:13839/13849, Kakerlake :11482). Betroffen: Gorilla
(11C0/1141), Kakerlaken (15 Raeume), 0x2b (9 Raeume), Birkin 0x30 (6 Raeume, ohne das G5-Modul 5090/5091). Nachpruefung: Laeufe t2b_w5_def_a /
roach_w5_def (1c) bzw. Sonde 1f (RE2-Zeilen 0x23/0x27/0x29/0x2b/0x30 = "Treffer 1"). Das Kriterium fuer "RE2-Pause
regelt selbst" muss das sein, was wirklich laeuft (Aktor im RE2-Pausenfilter: `re15_re2z_owns_type` bzw. 0x20/0x21/
0x25/0x26 mit RE2-Gehirn), nicht der Flavor.

**M2 — Regression: im Standard-Modus schadet der Feuerstoss diesen Gegnern WENIGER als vor der Spur.**
Basis-exe gegen Spur-exe, gleicher Aufbau, Standard-KI: 0x2b 20 -> 10, 0x29 25 -> 14 (1c). Ursache: Schaden je Kugel
= Spalte 3 (re15_damage.c, `if (weapon_id == 5 || weapon_id == 6) dmg = dmg_row[3];`) bei weiterhin nur EINER Kugel
(M1). Nach Tabelle (PSX.EXE @0x8006e0d0) ebenso Gorilla 28 -> 12, Birkin 28 -> 15, Alligator 21 -> 7. Damit ist die
"Schussverschwendung" fuer diese Gegner groesser als vorher (3 Patronen = 1 Browning-Schuss). Behebt sich mit M1,
muss aber dort mitgemessen werden (Stoss = 3 x Spalte 3 erst, wenn alle drei Kugeln treffen).

**M3 — Testluecke: kein Riegel deckt "RE1.5-Gehirn unter Standard-KI" ab.**
`unit_r37_beretta` setzt durchgehend RE1.5-Flavor (Teil A-H, `re15_ai_flavor_set(RE15_AI_FLAVOR_RE15)`), Schaden C nur
Zombie/Hund; `integration_r37_beretta` misst nur Zombies (ROOM1140/11E1). Die 43 vorhandenen Gegner-Tests
(unit_maggot_ai, unit_cockroach_ai, unit_tyrant_ai, unit_birkin_ai, unit_alligator_ai, unit_r35_affen_*, ...) sind mit
dem Defekt gruen (eigener Lauf 43/43). Gebraucht: ein Riegel mit RE2-Flavor gegen mindestens 0x29/0x2b (Sonde 1f
zeigt den Aufbau; mit KI-Ticks oder an der exe wie 1c), Erwartung drei Treffer E/E+3/E+6 und Stoss = 3 x Spalte 3.

**M4 — Dossier/Header beschreiben die Lage falsch (Nutzerinformation irrefuehrend).**
G_beretta.md §7 "Gegen Gegner mit RE1.5-Schadenszeile (Gorilla, Kakerlake, Alligator, Tyrant, Birkin ...) ist der
Stoss damit 3 x Browning" — im Standard-Modus gemessen 1 x Browning (M1/M2). re15_beretta_r37.h ("Gegner mit
RE2-Trefferpause (+0x1D3 ...) behalten ihre RE2-Regel") und der Kommentar beretta_r37.c:89 setzen "RE2-Flavor" mit
"hat RE2-Pausenfilter" gleich. Nach der Behebung von M1 richtigstellen.

Kein Mangel (geprueft): RE-Belege (Abschnitt 2), Kugel-Takt E/E+3/E+6 synchron zu den Muendungsblitzen, Patronen je
Kugel (2 -> 2 Treffer, 1 -> 1), Rueckstoss-Abbruch frisst keine Kugel, gehaltener Abzug (Folgestoesse je 3 Treffer),
Browning/andere Waffen unveraendert, Elza, RE2-Zeilen-No-op (Spalte 5 == 3), Kraehe (stirbt an Kugel 1), Vertrag.

## Anhang A — Sonde `abn_sonde_g.c` (Abschnitt 1f)
```c
/* Abnahme 0 Spur G — Sonde: Feuerstoss 5 gegen je einen Gegner-Typ unter RE2- und RE1.5-Flavor.
 * Aufbau wie tests/unit/test_r37_beretta.c (ROOM1140.RDT, Leon (200,-18200) Blick -z, Ziel (200,-19600)).
 * Je Bild wie game_step: erst die RE2-Filter (game_step_common.c:2384-2395), dann re15_beretta_r37_tick_ex(1).
 * KEINE Gegner-KI-Ticks (wie der Unit-Test der Spur). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_inventory.h"
#include "re15_ai_flavor.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_beretta_r37.h"
#define XS_(x) #x
#define XS(x) XS_(x)
extern re15_actor_t g_actors[];
extern int re15_re2z_owns_type(unsigned t);
extern void re15_re2z_hit_filter_apply(int slot);
extern void re15_re2_pause_filter_apply(int slot);
static void filter(void){ for (int s=1;s<RE15_ACTOR_MAX;s++) if (g_actors[s].active && re15_ai_re2_for_type(g_actors[s].type)) {
    if (re15_re2z_owns_type(g_actors[s].type)) re15_re2z_hit_filter_apply(s); else re15_re2_pause_filter_apply(s);} }
int main(void){
    char p[1024]; snprintf(p,sizeof p,"%s/STAGE1/ROOM1140.RDT",XS(RE15_ASSETS_PATH));
    FILE*f=fopen(p,"rb"); if(!f){printf("RDT fehlt\n");return 1;} fseek(f,0,SEEK_END); long n=ftell(f); fseek(f,0,SEEK_SET);
    uint8_t*rdt=malloc(n); fread(rdt,1,n,f); fclose(f); memset(&g_room_rdt,0,sizeof g_room_rdt);
    g_room_rdt_ok=(re15_rdt_parse(rdt,(size_t)n,&g_room_rdt)==0 && g_room_rdt.sca_count>0);
    static const uint8_t T[]={0x10,0x20,0x21,0x23,0x25,0x27,0x29,0x2b,0x30};
    for (int fl=0; fl<2; fl++){
      re15_ai_flavor_set(fl? RE15_AI_FLAVOR_RE15 : RE15_AI_FLAVOR_RE2);
      for (unsigned k=0;k<sizeof T;k++){
        re15_beretta_r37_reset();
        re15_actor_t*pl=&g_actors[RE15_ACTOR_SLOT_PLAYER]; memset(pl,0,sizeof*pl);
        pl->active=1; pl->hp=100; pl->state=1; pl->x=200; pl->z=-18200; pl->rot_y=1024; re15_player_apply_hitbox(pl);
        for(int s=1;s<RE15_ACTOR_MAX;s++) memset(&g_actors[s],0,sizeof g_actors[s]);
        re15_actor_t*e=&g_actors[1]; e->active=1; e->type=T[k]; e->x=200; e->z=-19600; e->hp=2000; e->state=1;
        re15_enemy_apply_hitbox(e,T[k]); e->grid_id=0; e->aim_band=2;
        re15_inv_init(); re15_inv_grant(5,15); re15_player_set_equipped_weapon(5);
        int r1=re15_player_weapon_fire(5); int hp1=e->hp; re15_beretta_r37_feuerstoss(5,3,r1);
        int treffer=(r1!=0); char bild[64]="0"; 
        for(int t=1;t<=8;t++){ filter(); int r=re15_beretta_r37_tick_ex(1); if(r==2){treffer++; char b[8]; snprintf(b,sizeof b,",%d",t); strcat(bild,b);} }
        printf("%s Typ 0x%02x: Kugel1 r=%d dHP1=%d | Treffer %d (Bilder %s) | HP 2000 -> %d (Stoss %d)\n",
               fl?"RE1.5":"RE2  ",T[k],r1,hp1-2000,treffer,r1?bild:"-",e->hp,e->hp-2000);
      }
    }
    return 0;
}
```

## Anhang B — Messskript `abn_run.sh` (Abschnitte 1a-1e; Auswertung: `G_beretta_mess/auswertung.py <lauf>`)
```bash
#!/bin/bash
# Abnahme 0 Spur G — eigener Messlauf an der echten exe (Messkopie re15_abn0g.exe neben der exe).
# Aufruf: [ELZA=1] [JUMP=xxxx] [EXIT=...] bash abn_run.sh <tag> <waffe> <flavor> <pos x,z,rot> <input> [extra env ...]
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r37_beretta"
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
TAG="$1"; WAFFE="$2"; FLAVOR="$3"; POS="$4"; INPUT="$5"; shift 5
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="$EXE_DIR/${EXE_NAME:-re15_abn0g.exe}"; SRC="${EXE_SRC:-$EXE_DIR/re15_pc.exe}"
cmp -s "$SRC" "$EXE" || cp -f "$SRC" "$EXE"
WD="$SP/mess/$TAG"
rm -rf "$WD"; mkdir -p "$WD"
if [ "${ELZA:-0}" = "1" ]; then
    START="RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1"
else
    START="RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2"
fi
FLV=""
[ "$FLAVOR" != "default" ] && FLV="RE15_AI_FLAVOR=$FLAVOR"
( cd "$WD" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 $START $FLV \
    RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP="${JUMP:-1140}@250" RE15_PLAYER_POS="$POS" \
    RE15_GIVE="$WAFFE:${MAG:-15}" RE15_EQUIP="$WAFFE" \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT="$INPUT" \
    RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_FX_LOG=fx.log RE15_EXIT_AT="${EXIT:-260#1140}" \
    "$@" timeout 240 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
cat "$WD/exit.txt"
```
Aufrufe: z.B. `bash abn_run.sh l_w5_re2 5 re2 200,-18200,1024 M2.0,MA0.1,M3.0,W2`; Elza `ELZA=1 JUMP=11E0 EXIT=220#11E1 bash abn_run.sh e_w5_default 5 default -10500,-11000,1024 M3.0,MA0.1,M3.0,W2`;
0x2b `JUMP=5040 EXIT=200#5040 bash abn_run.sh t2b_w5_def_a 5 default -14493,-13185,1024 M2.0,MA0.1,M3.0,W2`; Kakerlake `JUMP=3040 EXIT=200#3040 ... roach_w5_def 5 default -13500,-10300,1024 ...`;
Basis-exe: zusaetzlich `EXE_SRC=<hauptbaum>/re15_port/build/platform/pc/re15_pc.exe EXE_NAME=re15_abn0g_basis.exe`; Restmunition `MAG=2` / `MAG=1`.

## 5. Gezielte Tests (eigener Lauf im Spurbaum, keine volle Suite)
* Regex des Bau-Agenten ohne die exe-Laeufe: `ctest --test-dir re15_port/build -R "^(unit_r37_beretta|unit_r36_schuss|...|
  unit_r35_affen_schrot)$" --timeout 600 --output-on-failure` -> **100% tests passed, 0 tests failed out of 47** (3,92 s).
* Gegnertypen, deren Schadenszeile die Spur beruehrt: `-R "^(unit_maggot_ai|unit_cockroach_ai|unit_birkin_ai|
  unit_alligator_ai|unit_tyrant_ai|unit_maggot_bone_square|unit_re2_dog_hp_reroll|unit_maggot_zone_leap|unit_gator_sweep|
  unit_gator_fress|probe_p2_birkin_g5|probe_p3_birkin_rest|unit_r17_birkin_1zu1_pin|unit_r18_birkin_restluecken_pin|
  unit_r20_birkin_push_pin|unit_r30_birkin_frost|unit_r30_n_room5080_birkin_wand|r30_hund_hitbox_stauchung|
  r31_trefferboxen|unit_r34n_f_leiche_latch|unit_r35_affen_.*|unit_r36_zombieki_affen.*|unit_pri_inventar_riegel)$"`
  -> **100% tests passed, 0 tests failed out of 43** (34,43 s) — gruen MIT dem Defekt aus M1/M2 (keiner prueft den
  Feuerstoss gegen diese Typen).
* exe-Laeufe: `-R "^(integration_r37_beretta|integration_r36_schuss|integration_r36_elza_schuss|integration_r35_werfer|
  integration_r36_granate)$" --timeout 900` -> **100% tests passed, 0 tests failed out of 5** (551,10 s; r35_werfer
  145,82 s, r36_elza_schuss 44,78 s, r36_granate 104,12 s, r36_schuss 131,01 s, r37_beretta 125,32 s). r37_beretta:
  leon_re2_w5 / leon_re2_w6 / leon_re15_w5 je "F61 Munition -3, Treffer 0:5:-16;3:5:-16;6:5:-16", leon_re2_w3
  "Browning F61 Munition -1, Treffer 0:5:-16", elza_re2_w5 "F91 Munition -3, Treffer 0:1:-16;3:1:-16;6:1:-16".
* Echter Weg (Tuer statt Sprung): nicht anwendbar — Waffenmechanik, raumunabhaengig (Raumlader des Debug-Sprungs ist
  derselbe); die M93R erhaelt der Spieler ueber das Item-Debug des Inventars.
* Messkopien `re15_abn0g.exe` / `re15_abn0g_basis.exe` nach den Laeufen geloescht (kein taskkill, alle Laeufe endeten
  ueber RE15_EXIT_AT mit exit=0).

## 6. Fuer den Bau-Agenten (Nachbesserung)
1. M1: Freigabe-Kriterium in `riegel_loesen()` auf "fuer diesen Aktor laeuft ein RE2-Pausenfilter" umstellen
   (Zombie-Familie `re15_re2z_owns_type` bzw. 0x20/0x21/0x25/0x26-Baby mit RE2-Gehirn); alle anderen Aktoren — auch im
   RE2-Flavor — wie im RE1.5-Modus behandeln. Danach an der exe 0x2b (ROOM5040) und 0x29 (ROOM3040) mit Standard-KI
   messen: drei Treffer E/E+3/E+6.
2. M2: mit 1. mitmessen — Stoss = 3 x Spalte 3 (0x2b 30, 0x29 42 statt Basis 20/25); im Dossier fuer den Nutzer
   ausweisen.
3. M3: Riegel mit Standard-/RE2-Flavor gegen mindestens 0x29 und 0x2b (Unit mit Filter-/KI-Tick oder exe-Lauf).
4. M4: Dossier §7 und Header/Kommentar richtigstellen.
