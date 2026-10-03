# Runde 35 — Spur B "werfer": UNABHAENGIGE ABNAHME 2

Stand: Zweig `r35/werfer`, HEAD 9c791e47 (Nachbesserung 2), Basis master 154a73c1. Datum 2026-10-03.
Pruefer: Abnahme-Agent 2 (kein Code geaendert, nur diese Datei). Massstab: Wortlaut AUFTRAG.md Zeilen 13/14,
VERTRAG.md, CLAUDE.md (RE-Gate), Maengelliste B_abnahme_1.md (N1, N2).

> 1. Einige Waffen, wie die Granatwerfer oder der Raketenwerfer gehen noch nicht
> 2. Andere Waffen wie der Flammenwerfer oder die Colt Python gehen noch nicht richtig.

## 0. Ergebnis

| Punkt / Gate | Urteil |
|---|---|
| 1 Granatwerfer (15/16/17) + Raketenwerfer (18) | **erfuellt** (N1 behoben, an drei Zellformen an der exe gemessen) |
| 2 Flammenwerfer (14) + Colt Python (20) | **erfuellt** (N1 auch fuer den Strahl, N2 behoben; zwei kosmetische OFFEN-Punkte s. §6) |
| Abnahme-1-Maengel | N1 behoben (Typ 5 ROOM10E0, Typ 4 ROOM1050, Durchtunneln ROOM1000 selbst gemessen), N2 behoben (hp -1 wie Redhawk) |
| Gate Suite | haelt (Dossier §9.9 und Log `nb2_suite2.log`: 484/484 am selben Code; eigener Lauf der 10 betroffenen Tests 10/10) |
| Gate @0x (RE-Gate) | haelt (9 Stellen selbst disassembliert, alle zutreffend; Typ 4 zusaetzlich selbst nachgerechnet; keine Rate-Marker) |
| Gate Pfade/Vertrag | haelt |
| Gate Tests | haelt (N1: unit 120-129/140-149 + integration_r35_werfer_form; N2: unit 130-134 + _form; eigene Gegenprobe rot) |

**bestanden = JA.** Beide Maengel der Abnahme 1 sind behoben und am gebauten Stand nachgemessen, nicht nur am
Rezept des Bau-Agenten: zusaetzlich zu dessen ROOM10E0-Bahn an einer zweiten Zellform (Dreieck Typ 4, ROOM1050), am
duennen Kreis (ROOM1000) und mit der alten exe als Gegenprobe. Keine Regression auf den Abnahme-1-Bahnen.

## 1. Bau und Suite

* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` ->
  `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. Die exe `re15_port/build/platform/pc/re15_pc.exe` ist der Stand
  der Quellen; `cmp re15_pc.exe re15_r35nb2b.exe` = gleich.
* `git diff 2cc3862c HEAD --stat` = nur `analysis/befunde_runde35/B_werfer.md` (+11): der Code am Suite-Lauf 2 ist der
  Endstand. Dossier §9.9 traegt woertlich `=== LOCAL-BUILD-OK (all) — Tests 484/484`; das Log
  `re15_port/build/mess_r35b_logs/nb2_suite2.log` endet mit `test OK — 484/484 bestanden` /
  `=== LOCAL-BUILD-OK (all) — Tests 484/484` (N = 484 >= 478). Nicht selbst wiederholt.
* Eigener Lauf (Git-Bash, PATH msys64 zuerst) `ctest --test-dir re15_port/build -R "r35_werfer|re2_hp_model|re2_weapon_rows|
  re2_zombie_teardeath|re2_baby_spider_dmg" --timeout 900 -V`: `100% tests passed, 0 tests failed out of 10` —
  unit_re2_weapon_rows, unit_re2_baby_spider_dmg, unit_re2_hp_model, unit_re2_zombie_teardeath, unit_r35_werfer
  (Teil I: `Rakete ab (-2477,-1657) Richtung (465,4069): Explosion nach 4 Bildern @(-2331,-385)`, `GL-Runde x -1200 ...
  @(-1200,1500)`, `Rakete ab (-13000,-12500) Richtung (4096,0): ... @(-7112,-12500)`, `Rakete ab (-2117,-3290) ...
  @(-2117,-3802)`, `... mit Schuetze: Explosion nach 1 Bildern @(-4750,-13576)`, ok 120-129, 140-149, 130-134),
  unit_r35_werfer_kombi, integration_r35_werfer 134.18 s (`[w20]: ... Zombie 2 HP 50 -> -1 (ss1=5)`), _ton 28.47 s,
  _elza 30.03 s, _form 26.09 s (`Rakete 10E0 explodiert vor der Hypotenuse (... @(-2331,-2629,-388)), 3 Flug-Lagen
  ausserhalb des festen Dreiecks; Python-Treffer Platz 2 -> hp -1 (Kritklasse)`). Log
  `<S>/ctest_r35.log`.

`<S>` = Sitzungs-Scratchpad `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abnB2`.

## 2. Messprotokoll (eigene Laeufe)

exe-Kopien (kein taskkill): `re15_port/build/platform/pc/re15_abnB2.exe` (cmp-gleich re15_pc.exe, Stand HEAD) und als
Gegenprobe `re15_abnB1.exe` (cmp-gleich `re15_r35nb2_alt.exe` = Stand vor Nachbesserung 2). Skripte `<S>/run_b2.sh`
(Rezepte d18/d14/t7/t20/p7/p20 woertlich aus Dossier §9.1/run_nb2.sh, dazu eigene Messorte), `<S>/run_real.sh`,
`<S>/run_nl.sh`; Auswertung `<S>/tiefe_b2.py` (Tiefe senkrecht zur Hypotenuse je Partikel), `<S>/dreieck4_b2.py`,
`<S>/form_b2.py` (unabhaengige Gleitkomma-Nachrechnung des Formtests), `<S>/ana.py` (HP-Verlauf aus state.log).
Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_DEBUG_JUMP=<raum>@250 RE15_PLAYER_POS RE15_GIVE RE15_EQUIP
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_STATE_LOG RE15_WAFFEN_LOG RE15_WPN_DBG=1 RE15_EXIT_AT=<bild>#<raum>`,
Vorgabe-KI. Alle Laeufe exit 0. Neu-Laeufe `<S>/neu/`, Gegenprobe `<S>/alt/`.

Lese-Hinweis (selbst ermittelt): die RE2FLUG-Zeile wird in `re15_werfer_tick` VOR dem RE2-FX-Tick des Bildes
geschrieben (werfer_r35.c:144-163 `flug_log`), zeigt also die Lage des VORIGEN Ticks. Der Tick des Bildes rechnet
die naechste Lage und testet die Strecke dorthin; bei Kontakt Rueckprall `lokal -= vel1 + vel2/3` (re2_fx.c:1180-1192,
@0x8001f874-9c0) — deshalb liegt die Explosion ~1,33 Schritte vor der Kontaktlage. Beispiel d18 unten: Kontakt bei
(-2215,627), Explosion (-2331,-388) = 627 - 1015.

### 2.1 N1 — ROOM10E0 Zelle 21 (Typ 5), Rezept Abnahme 1 §2.6 (Leon (-3000,-3000) Blick 3072)
* Rakete `d18` (`18:4`, `M0.6,MA0.3,M1.6,W2`):
  ```
  NEU  RE2FLUG ... welt=(-2477,-2575,-1657) / (-2389,-2607,-896) / (-2302,-2640,-134)
       SE  re2fx code=0x01140001 -> RE2 ARMS11 Satz 20 @(-2331,-2629,-388)
  ALT  ... (-2302,-2640,-134), (-2215,-2673,627), (-2128,-2706,1389), (-2041,-2739,2150)
       SE  re2fx code=0x01140001 -> RE2 ARMS11 Satz 20 @(-2070,-2728,1897)
  ```
  `dreieck.py` (Werkzeug des Dossiers, gelesen): NEU `Plaetze 3, im festen Dreieck 0, z max -134`; ALT `Plaetze 6, im
  festen Dreieck 3, z max 2150`. = Dossier §9.1/§9.4 reproduziert.
* GL `d15` (eigener Lauf, `15:6`): NEU fuenf Explosionen @(-2554,-551), (-2355,-253), (-2542,-243), (-2101,-11),
  (-1661,1004) — alle im LEEREN Teil des Zellrechtecks VOR der Hypotenuse (z_h bei diesen x: 88/346/103/676/1247);
  `Plaetze 24, im festen Dreieck 0, z max 1137`. ALT vier Explosionen direkt an der Muendung @(-2481,-1325) ..
  (-2438,-1377) = Fehlkontakt im LEEREN Teil des Rechtecks (z_h dort ~180), die fuenfte Runde fliegt ins Dreieck und
  explodiert @(-2172,1528) (z_h 584 -> 944 tief im festen Teil); `Plaetze 10, im festen Dreieck 3, z max 1726`.
  Beide Fehler der alten Fassung (Durchflug + Rechteck-Fehlkontakt) sind weg.
* Flamme `d14` (`14:100`, `M0.6,MA1.5,M1.6,W2`), `tiefe_b2.py`:
  NEU `Partikel 15, Lagen im Dreieck 143, Partikel im Dreieck 13, max Tiefe je Partikel [81 .. 139], Schritt ab einer
  Lage im Dreieck: Mittel 16, max 20; z max 898`;
  ALT `Partikel 15, Lagen im Dreieck 47, Partikel im Dreieck 12, max Tiefe [6 .. 332], Schritt Mittel 92, max 120; z max 769`.
  Die hoehere Lagenzahl (143) sind die Nachbrenn-Bilder an der Kontaktstelle (Schritt <= 20), nicht ein tieferer Flug
  (Tiefe 332 -> 139). RE2-Nachbrennen selbst disassembliert (§5.1 Nr. 8).

### 2.2 N1 — ROOM1050 Zelle 20 (Typ 4) — eigener Messort, NICHT im Rezept des Bau-Agenten
SCA ROOM1050 Zelle 20 = {w 2100, d 3250, x 22450, z -19150, Typ 4, u0 ff, u1 00, floor 03}; Typ 4 = rechter Winkel bei
(x+w, z+d), fest wenn z >= -15900 - 3250*(x-22450)/2100 (selbst aus LAB_8003beb0 hergeleitet, §5.1 Nr. 7). Raum ohne
Gegner (kein Auto-Zielen: `PL(19000,-20000,rot=3768)` bleibt). Leon (19000,-20000) Blick 3768:
* Rakete `r4_1050` (`18:4`):
  ```
  NEU  ... (22433,-2673,-18595), (23110,-2706,-18236)  SE ... 0x01140001 ... @(22884,-2695,-18356)
  ALT  ... (23110,-18236), (23786,-2739,-17877), (24463,-2772,-17518)  SE ... @(24238,-2761,-17637)
  ```
  `dreieck4_b2.py`: NEU `Lagen 5, im festen Dreieck 0`; ALT `Lagen 7, im festen Dreieck 2 [(23786,-17877),
  (24463,-17518)]` — die alte Rakete fliegt durch das Dreieck bis zur Wand Zelle 3 (x 24950). Neu: Kontakt auf der
  Strecke (23110,-18236) -> (23786,-17877) (Endpunkt 91 hinter der Hypotenuse), Rueckprall, Explosion davor.
  `form_b2.py` (unabhaengig, Gleitkomma): die Strecke davor (22433,-18595)->(23110,-18236) beruehrt KEINE Zellform —
  passt.
* GL `g4_1050` (`15:6`): NEU `Lagen 35, im festen Dreieck 0`, Explosionen u.a. @(23201,-18123), (23147,-17278) vor der
  Kante; ALT `Lagen 31, im festen Dreieck 2 [(23901,-17741), (24427,-17455)]`, Explosion @(24252,-17550) im Dreieck.
* Flamme `f4_1050` (Leon (21000,-19000)): NEU `Lagen im Dreieck 130, max Tiefe je Partikel [37 .. 222], Schritt danach
  Mittel 15 max 21`; ALT `106, max Tiefe [37 .. 838], Schritt Mittel 72 max 140`. Erste Lage im Dreieck bei beiden
  6..73 tief (Schritt hinein 20 bzw. 149/150) — der Strahl haelt an der ersten Lage und kriecht dort mit 20 je Bild
  (RE2 Op 70, kein Rueckprall). Das ist der Dossier-Punkt NEU 20; hier gemessen bis 222 tief (ROOM10E0: 139).
* Bilder `<S>/r4_1050_mont.png`, `<S>/d18_neu_mont.png` / `d18_alt_mont.png`: in beiden Raeumen zeigt der aktive Kameraschnitt
  die Flugbahn NICHT (Waschraum- bzw. Tresen-Ansicht) — als Bildbeleg unbrauchbar, Beleg sind die wf.log-Lagen.

### 2.3 N1 — Durchtunneln, ROOM1000 Kreis-Zelle 15 (Mitte (-1700,-4350), r 500), Leon (-1728,-1000) Blick 1024
```
NEU  (-2117,-2390), (-2127,-3157), (-2138,-2640,-3923)  SE ... 0x01140001 @(-2134,-2629,-3668)
ALT  ... (-2138,-3923), (-2149,-2673,-4690)  SE ... @(-2145,-2662,-4435)
```
Bei x -2145 liegt die Sehne bei z -4578..-4122; die Lagen -3923 und -4690 liegen beide ausserhalb, die Strecke
schneidet sie. Neu: Explosion vor dem Kreis; alt: die Rakete springt ueber den Kreis (Kontakt erst an der Wand
dahinter, Rueckprall-Explosion zufaellig im Kreis). = Abnahme-1 OFFEN 19, behoben.

### 2.4 N2 — Python gegen Redhawk, ROOM1140 (Rezepte t7/t20/p7/p20 des Dossiers)
`ana.py` (HP-Verlauf je Platz):
```
NEU t7  (Redhawk, Tisch (200,-10300) Blick 1024): slot 5 typ 11 ... F19:hp-1(st3,ss1=5)
NEU t20 (Python, gleich):                         slot 5 typ 11 ... F19:hp-1(st3,ss1=5)
ALT t20 (Python):                                 slot 5 typ 11 ... F19:hp-650(st3,ss1=5) F70:hp-1(st7,ss1=8)
NEU p7  (Redhawk, (-1676,-18070) Blick 1076):     slot 2 F19:hp-1(st3,ss1=5), slot 3 F44:hp-1(st3,ss1=5)
NEU p20 (Python, gleich):                         slot 2 F19:hp-1(st3,ss1=5), slot 3 F44:hp-1(st3,ss1=5)
ALT p20 (Python):                                 slot 2 F19:hp-850 ..., slot 3 F44:hp-820 ...
```
Python und Redhawk verhalten sich am selben Zombie bildgleich (Platz, Bild, Zustand, Reaktionszeile). Affe (Typ 0x27):
unit_r35_werfer 133/134 (`w20 gegen Typ 0x27: +0x93 Bit 0x40 gesetzt, hp > 0`); die Leser des Bits im Port lesen nur
das Bit (enemy_ai_common.c:9157/9206/9248, kein Waffenvergleich) — nicht live im 11C0 gemessen (dort laeuft beim
Sprung die Ada-Szene, s. 2.7).

### 2.5 Regression — echter Weg (Statusschirm -> Item-Debug SELECT + N x R1 -> zielen -> feuern), ROOM1000
`run_real.sh <S>/real 14 15 16 17 18 20` (Rezept Abnahme 1 §2.1): Baenke `W-bank -> W0E` (14), `W0F` (15/16/17), `W12` (18),
`W14` (20); 14: 15 Strahl-Spawns, 360 Flug-Lagen, `re2arms ARMS10 satz=0` x3 / `satz=11` x2; 15: 10 Explosionen
`0x01110001` — **zeilengleich** mit Abnahme 1 (`diff` der Explosionszeilen: identisch); 16 `0x01130001 @(17071,-2094,-13030)`,
17 `0x01120001 @(17071,-2094,-13030)`, 18 `0x01140001 @(16882,-2728,-12963)` / `@(16855,-2621,-13025)` = Abnahme 1;
20 `arms_rec=0 bank=20` x2. Tisch ROOM1140 (Abnahme 1 §2.4): Rakete `@(-174,-2530,-10668)`, GL fuenf Explosionen
`@(-102,-2101,-11760)` .. `@(-165,-1801,-11315)`, alle HP unveraendert — wie Abnahme 1 (Tisch-Regel, OFFEN 9).

### 2.6 Nachladen Python gegen Redhawk (ROOM1000, Rezept Dossier §4.2, `<S>/run_nl.sh`)
`20:1,23:8` bzw. `7:1,23:8`, Skript `M0.6,MA0.3,M1.6,MA0.2,M2.6,...`: Magazin w20 `mg=1 -> 0 -> 6 -> 5`, w7 dasselbe;
`SE arms_rec=3 bank=20` bzw. `bank=7` (Nachlade-Ton). Die Python laedt nach und feuert weiter. Der Schnelllader-Wurf
0x04060800 (player_common.c:1213-1221, @0x80033e34-88) bleibt Redhawk-eigen (Dossier NEU 21) — im Bild
`<S>/nl_mont.png` (Leon klein im Hintergrund) nicht unterscheidbar.

### 2.7 Versuche ohne Wertung
* ROOM11C0 (Raute Typ 2, Leon (-13000,-12500) Blick 0): beim Sprung laeuft die Ada-Szene (`pf=01000007`, Leon wird zu
  (-15893,12133) gefuehrt), kein Schuss. Die Raute ist nur im Unit-Test (124/125) belegt.
* ROOM1190 (Kapseln Typ 9), ROOM10D0 (Dreiecksring), ROOM10E0 Typ-7-Zelle 22: das Auto-Zielen dreht Leon auf einen
  Gegner (rot 2048 -> 529, 3072 -> 3937, 1536 -> 3383); die Bahn trifft die gewuenschte Zelle nicht. Die Formen 6..9
  sind nur ueber unit_r35_werfer 140-148 (Form gegen den Port-Zwilling des RE1.5-Handlers) belegt.

## 3. Urteil je Punkt

### Punkt 1 — Granatwerfer / Raketenwerfer: ERFUELLT
Ausruesten ueber den echten Weg, Feuern, RE2-Geschosse, Explosion an Rechteck-, Kreis- und jetzt auch an Dreieckszellen
(Typ 5 ROOM10E0, Typ 4 ROOM1050 an der exe; Typ 2/6..9 im Unit-Test), kein Ueberspringen duenner Zellen (ROOM1000),
kein Fehlkontakt mehr im leeren Teil einer Schraegzelle (GL ROOM10E0: vier Runden explodierten vorher an der Muendung),
Toene, Schaden, Munition/Nachladen, Elza (Test gruen) — keine Regression auf den Bahnen der Abnahme 1 (2.5).
Abnahme-1-N1 ist behoben; die Belegkette (Formtabelle, Typ 5/2/4, RE2-Formtest, vorige Weltlage) ist selbst
disassembliert (§5.1).

### Punkt 2 — Flammenwerfer / Colt Python: ERFUELLT
Flammenwerfer: Strahl haelt an Schraegzellen (Tiefe 332 -> 139 in ROOM10E0, 838 -> 222 in ROOM1050), danach
RE2-Nachbrennen 20 je Bild (@0x8002351c-38 selbst gelesen); Rest wie Abnahme 1 (Takt, Fuel, Toene, Schaden).
Colt Python: Feuer, Blitz/Rauch, Knall, Schaden, Nachladen wie bisher; Abnahme-1-N2 behoben — der Python-Treffer
toetet Typ < 0x20 wie der Redhawk (hp -1, bildgleich an Platz 5/2/3, 2.4); das Bit 0x40 fuer den Affen ist gesetzt
(Unit 133/134).

## 4. Maengel

Keine blockierenden Maengel. (Nicht gewertete Hinweise in §6.)

## 5. Gates

### 5.1 RE-Gate (@0x) — Stichproben SELBST disassembliert (re15_disasm.py / re2_disasm.py)
1. RE1.5 Formverteiler FUN_8003aea0: `addiu v0,v0,-17240` (0x8003bca8) / `sw v0,10332(at)` (0x800b285c) @0x8003aefc-af04;
   [2] 0x8003d00c @0x8003af14, [3] 0x8003d6a8 @0x8003af24, [4] 0x8003beb0 @0x8003af34, [5] 0x8003c734 @0x8003af44,
   [6] 0x8003cb9c @0x8003af54, [7] 0x8003c2cc @0x8003af64, [8] 0x8003d7e8 @0x8003af74, [9] 0x8003d930 @0x8003af84,
   `jr ra` @0x8003af88. **Stimmt.**
2. Typ 5 LAB_8003c734: `lhu v0,2(t3)` / `lhu v1,6(t3)` ... `subu a1,s0,t4` / `subu t8,s3,a1` (t8 = d+r) @0x8003c764-788,
   `subu v0,t9,t5` / `mult t8,v0` (px - x) @0x8003c7b4-b8, `subu t6,v0,t5` (w+r) / `div a2,t6` @0x8003c7dc-e0, dann
   `subu a1,t7,a1` (pz - (z-r)) / `slt v0,s2,a1` / `beq v0,zero,0x8003cb68` @0x8003c82c-834 = fest bei LINE < pz-(z-r).
   Port case 5 Ecken (x,z),(x+w,z+d),(x,z+d) **stimmt**.
3. Typ 2 LAB_8003d00c: `lhu t1,0(s0)` / `lhu v1,4(s0)` / `lhu t0,2(s0)` / `lhu a0,6(s0)` @0x8003d070-7c, `addu s7,t1,v1` /
   `addu s6,t0,a0` @0x8003d088-8c, `srl a1,a1,1` / `addu v1,v1,a1` / `srl a2,a2,1` / `addu a0,a0,a2` @0x8003d090-9c. **Stimmt.**
4. RE2 Formtest FUN_8004fba0: `lhu v1,8(s2)` / `andi v1,v1,0xf` / `sltiu v0,v1,0xe` / `beq v0,zero,0x8004ffb0` / `sll v0,v1,2` /
   `lw v0,4356(at)` / `jr v0` @0x8004fe2c-54; Tabelle 0x80011104 gelesen: [0] 0x8004ffb0, [1..8] 0x8004fe5c, fe7c, fe9c,
   febc, fedc, fefc, ff1c, ff3c, [9] 0x8004ffb0. **Stimmt** (Rechteck [0]/[9] = nur Vortest, [1..8] eigene Formen).
5. RE2 Weltlage FUN_8001d894: `lw v1,52(a2)` @0x8001d954, `lhu a0,56(a2)` @0x8001d95c, `sw v1,60(a2)` @0x8001d964,
   `sh a0,64(a2)` @0x8001d96c (Delay-Slot des `beq` @0x8001d968, laeuft immer). Reihenfolge FUN_8001d68c: Op A `jalr`
   @0x8001d6c0, `jal 0x8001d894` @0x8001d6c8, Op B ueber 0x8009d868 `jalr` @0x8001d6f8. Spawner `sw zero,52/60/68/72(t0)`
   @0x8001ccfc-d08. Port re2_fx.c:298-299 kopiert genauso. **Stimmt.**
6. RE1.5 Kritklasse FUN_80011f50: `lbu v0,147(s1)` / `andi v0,v0,0x1` / `sb` @0x80012370-7c; `ori v0,zero,0x8` / `bne v1,v0,
   0x800123a4` / `ori v0,zero,0x7` @0x80012380-88; `lw v0,-2592(v0)` (0x8008f5e0) / `sltiu v0,v0,0xbb8` @0x80012390-98;
   `ori v0,v0,0x40` / `sb v0,147(s1)` @0x800123b4-b8; `andi v0,v0,0x40` / `beq v0,zero,0x80012520` @0x800124fc-500,
   `lbu v0,8(s1)` / `sltiu v0,v0,0x20` / `beq` / `addiu v0,zero,-1` / `sh v0,154(s1)` @0x80012508-1c. Zwischen 0x800123bc und
   0x800124f8 schreiben nur `ori 0x80` @0x800123f4, @0x80012410 und `ori 0x1` @0x800124f0 auf +0x93 — das Bit 0x40 wird
   nicht beruehrt; "HP -1 am Bit" ist also gleichwertig zum Original. **Stimmt.**
7. Typ 4 LAB_8003beb0 (ueber das Dossier hinaus, das 4/6/7 aus den Port-Zwillingen uebernimmt): `andi t3,a3,0xffff` /
   `subu v1,zero,t3` @0x8003beb8-c4, `lhu t9,2(a0)` @0x8003beec, `subu v1,v1,t9` (-(d+r)) @0x8003befc, `subu v0,s1,v0`
   (px-(x-r)) / `mult` @0x8003bf14-18, `addu t5,v1,t3` (w+r) / `div` @0x8003bf28-2c, `addu t4,t9,t8` (z+d) / `subu fp,s0,t4`
   / `slt v0,s6,fp` / `beq v0,zero,0x8003c298` @0x8003bf80-8c = fest bei pz > (z+d) - (d+r)(px-x+r)/(w+r): rechter Winkel
   bei (x+w, z+d). Port case 4 (x,z+d),(x+w,z),(x+w,z+d) **stimmt**; an der exe in ROOM1050 bestaetigt (2.2).
8. RE2 Op 70 Kontakt: `jal 0x8004fba0` (a1 2, a2 8192, a3 0) @0x80023434-40; Kontaktzweig `sb zero,8` / `sh zero,12` /
   `sb zero,9` @0x800234c8-d8, `addiu v0,zero,12` / `sb v0,33` @0x800234e4-e8, `addiu v0,zero,20` / `sh v0,14(v1)`
   @0x8002351c-20, `addiu v0,zero,6` / `sb v0,2` @0x80023524-28, `sb 1,3` @0x80023534-38 — kein Lage-Ruecksetzen. NEU 20
   ist RE2-treu. **Stimmt.**
9. Aufrufer FUN_8004fba0: Op 15 `jal 0x8004fba0` @0x8001eea0, Op 24 `addiu a1,zero,2` / `addiu a2,zero,8192` / `jal` /
   `addu a3,zero,zero` @0x8001f808-14. **Stimmt.**

Rate-Marker im Code-Diff gegen master (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vermutlich|
geschaetzt|approx|getenv` in `+`-Zeilen): **0 Treffer**. PORT-WAHLen gekennzeichnet: w20 in der Kritklasse
(re15_damage.c:2665-2669), Schuetze -> Muendung (re2_fx.c Kommentar werfer_boden), Flaeche ohne Radius-Aufschlag 2
(werfer_r35.c Kopf). Commit-Messages 9250a21f und 9c791e47 tragen die Adressen.

Erklaert der Fix den Befund? Ja: N1-Vorbedingung "Filter `typ != 1 && typ != 3` + Punkttest" stand im alten Code
(Abnahme 1 §4) und ist im Diff ersetzt; die alte exe zeigt an allen drei Messorten genau den vorhergesagten Durchflug
bzw. Rechteck-Fehlkontakt, die neue nicht (2.1-2.3). N2-Vorbedingung "Kritklasse nur 7/8" stand in re15_damage.c und
ist ersetzt; ALT t20/p20 -650/-850, NEU -1 (2.4).

### 5.2 Vertrag / Pfade
`git diff master --name-only`: engine/src (enemy_ai_re2_zombie.c, game_step_common.c, inventory_common.c, menu_common.c,
player_common.c, re15_damage.c, re2_fx.c, werfer_r35.c neu), include (re15_werfer.h neu, re2_fx.h), platform/pc (main.c,
audio_pc.c, fx_plattform_pc.c), shared_assets/RE2/SOUND/ARMS10/11 (neu, erlaubt), tests (eigene + nachgezogene Alt-Pins
+ test_support.c), Dossiers. Suche nach release/, platform/android/, shared_assets/PSX/, tests/unit/CMakeLists.txt,
tests/integration/CMakeLists.txt: **0 Treffer**. Nachbesserung 2 beruehrt keine Datei der VERTRAG-Liste gemeinsamer
Dateien (nur re2_fx.c, re15_damage.c, eigene Dateien, Tests). Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots, Ereignisse.
Nachgezogene Pins geprueft: test_re2_hp_model `R35_KRIT(w)` = 7 oder 20 an den Crit-Stellen; test_re2_weapon_rows
Negativ-Kontrolle w20 erwartet jetzt DEATH; integration_r35_werfer [w20] erwartet hp -1 statt Schaden 900 — alles
direkte Folgen von N2, keine Abschwaechung anderer Waffen. Die Schadenszuordnung der Python bleibt gepinnt, wo die
Kritklasse nicht greift: test_re2_hp_model fuer Typen >= 0x20 (`2000 - row[R35_W(w)]`, Spalte 7), test_re2_baby_spider_dmg
(RE2-Zeile 5 -> 130), unit_r35_werfer 81 (Spalte 7 >= 200 -> Tod); den exakten Wert 900 am Zombie sieht kein Test mehr
(am Zustand durch hp -1 verdeckt). **Haelt.**

### 5.3 Tests
Messend fuer jeden Punkt: N1 unit_r35_werfer 120-129 (echte RDTs ROOM10E0/11C0/1000/1140), 140-149 (Form je Typ 1..9
gegen `re15_collision_constrain_contact_band` + Filter), integration_r35_werfer_form [rakete10e0] (exe, z < 528 und keine
Flug-Lage im Dreieck); N2 unit 130-134, integration_r35_werfer_form [python1140]. Eigene GEGENPROBE: `cmake
-DRE15_PC_EXE=.../re15_abnB1.exe -DWORKDIR=<S>/form_gegen_wd -P tests/integration/test_r35_werfer_form.cmake` ->
`r35_werfer_form [rakete10e0]: Explosion nicht VOR der Hypotenuse der Zelle 21 (z soll -3000..527): ' SE re2fx
code=0x01140001 -> RE2 ARMS11 Satz 20 @(-2070,-2728,1897)'` (rot). Unit-Gegenprobe des Bau-Agenten im Log
`re15_port/build/mess_r35b_logs/nb2_gegenprobe_unit.txt` gelesen: `test_r35_werfer: 7 FAILURES` (122/123/125/128/129/
131/134). **Haelt.**

## 6. Sonstige Feststellungen (nicht gewertet)
* **NEU 20 (Flamme kriecht nach Kontakt):** gemessen bis 222 tief in ROOM1050 (139 in ROOM10E0). RE2 Op 70 setzt bei
  Kontakt nur das Nachbrennen ohne Ruecksetzen (§5.1 Nr. 8) — RE2-treu; ein Ruecksetzen auf +0x3C/+0x40 waere eine
  PORT-WAHL ohne Vorbild. Bei Bedarf mit einer Sicht-Messung am Hintergrund entscheiden.
* **NEU 21 (Python ohne Schnelllader-Wurf beim Nachladen):** kosmetisch, Nachladen selbst funktioniert (2.6). Der
  Dossier-Satz "Falls die PORT-WAHL ... auch das Nachlade-Bild umfassen soll" ist eine offene Wahl; die PORT-WAHL
  "zweiter Magnum-Revolver" (§3.5) spricht dafuer, w20 im Zweig player_common.c:1215 aufzunehmen. Beim Zusammenfuehren
  entscheiden, nicht dem Nutzer als Frage vorlegen.
* Der Streckentest selbst ist eine Abweichung von RE2 (Punkttest je Bild). Im Code steht das ausdruecklich
  ("statt nur des Punktes (RE2 FUN_8004fba0 je Bild)", werfer_r35.c Kopf; "RE2 testet nur den Punkt je Bild",
  re2_fx.c), aber ohne das woertliche Etikett PORT-WAHL — der Grund (Abnahme-1 OFFEN 19, Nutzer "nicht durch die Wand")
  steht im Dossier §9.3.
* OFFEN 16/17 (allgemeiner Schuss-Pfad): Redhawk UND jetzt auch die Python toeten den fressenden Platz 5 ueber den
  Konferenztisch ROOM1140 (2.4 t7/t20, d=9272). RE1.5 sperrt dort die Schusslinie (FUN_8001b9b4, Abnahme 1 §5.1 Nr. 1).
  Wie in Abnahme 1: allgemeiner Resolver, nicht Spur B — aber durch N2 jetzt mit sofortigem Tod sichtbarer.
* Zusammenfuehrung: `re15_werfer_zelle_strecke`/`re15_werfer_band_strecke` (werfer_r35.c) und Spur As Formtest
  (granate_r35.c) sind dieselbe Flaechentabelle in zwei Kopien; re2_fx.c / re15_damage.c fassen auch andere Spuren an.
* game_step_common.c traegt seit dem ersten Abschluss ~60 Zeilen Spur-B-Aenderung (Entlade-Tabelle 15..18/20, Flammen-
  und Fuel-Zweig, Nachlade-Gate) — mehr als "1-5 Zeilen", aber in bestehenden Tabellen/Zweigen derselben Waffen; von
  Abnahme 0/1 angenommen, in Nachbesserung 2 unveraendert.
* `s_r35_schuetze` wird bei `re2fx_reset` nicht geloescht; ein nicht verbrauchter Eintrag wirkte nur, wenn derselbe Platz
  spaeter ohne `spawn()` ein Geschoss mit Op 15/24/70 bekaeme. Einziger Aufrufer von `re2fx_spawn_sofort` ausserhalb
  re2_fx.c ist werfer_r35.c:117 (grep); der interne Bank-2-Erzeuger re2_fx.c:1066 (Aufschlag-Platz 0x020C1000) setzt
  Op B = 47 + Art, laeuft also nicht ueber werfer_boden — derzeit ohne Folge.
