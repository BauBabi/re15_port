# Runde 35 — Spur B "werfer": UNABHAENGIGE ABNAHME 1

Stand: Zweig `r35/werfer`, HEAD 1e6c4b90 (Nachbesserung 1), Basis master 154a73c1. Datum 2026-10-03.
Pruefer: Abnahme-Agent 1 (kein Code geaendert, nur diese Datei). Massstab: Wortlaut AUFTRAG.md Zeilen 13/14,
VERTRAG.md, CLAUDE.md (RE-Gate), Maengelliste B_abnahme_0.md.

> 1. Einige Waffen, wie die Granatwerfer oder der Raketenwerfer gehen noch nicht
> 2. Andere Waffen wie der Flammenwerfer oder die Colt Python gehen noch nicht richtig.

## 0. Ergebnis

| Punkt / Gate | Urteil |
|---|---|
| 1 Granatwerfer (15/16/17) + Raketenwerfer (18) | **teilweise** (neuer Mangel N1: Geschosse fliegen durch diagonale Zellen) |
| 2 Flammenwerfer (14) + Colt Python (20) | **teilweise** (N1 trifft auch den Strahl; neuer Mangel N2: Python ohne Magnum-Kritklasse) |
| Abnahme-0-Maengel M1..M6 | M1 belegt (angenommen), M2 behoben, M3 behoben, M4 belegt, M5/M6 erledigt |
| Gate Suite | haelt (Dossier + Log 483/483, Code seither unveraendert; eigener Lauf `-R r35_werfer` 5/5) |
| Gate @0x (RE-Gate) | haelt (Stichproben selbst disassembliert, alle zutreffend; keine Rate-Marker) |
| Gate Pfade/Vertrag | haelt |
| Gate Tests | haelt fuer das Gebaute; kein Test fuer N1/N2 |

**bestanden = NEIN.** Die Nachbesserung 1 hat die vier Abnahme-0-Maengel sauber erledigt bzw. belegt (siehe §3).
Zwei NEUE, am gebauten Stand gemessene Maengel verhindern "erfuellt": N1 (Rakete fliegt in ROOM10E0 1900
Einheiten durch eine diagonale Wandzelle; betrifft 11 STAGE1-Raeume, darunter den Affen-Parkplatz 11C0) und N2
(die als "zweiter Magnum-Revolver" erklaerte Python bekommt die Kritklasse des Redhawk nicht — anderer HP-Verlauf
am selben Zombie gemessen, Affen-Abschuss im Sprung fehlt).

## 1. Bau und Suite

* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` ->
  `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. Die exe `re15_port/build/platform/pc/re15_pc.exe`
  (18:03:49) ist der Stand der Quellen; `git diff 6abed8d7 HEAD --stat` = nur B_werfer.md.
* Suite: Dossier §8.9 traegt woertlich `=== LOCAL-BUILD-OK (all) — Tests 483/483`; das Log
  `re15_port/build/mess_r35b_logs/nb1_suite.log` endet mit `100% tests passed, 0 tests failed out of 483` /
  `=== LOCAL-BUILD-OK (all) — Tests 483/483` (N = 483 >= 478) -> nicht selbst wiederholt.
* Eigener Lauf (PowerShell) `ctest --test-dir re15_port/build -R r35_werfer --timeout 600 -V`:
  `unit_r35_werfer` Passed, `unit_r35_werfer_kombi` Passed, `integration_r35_werfer` Passed 146.28 s,
  `integration_r35_werfer_ton` Passed 36.85 s (`Leerschuss hoerbar: erstes abweichendes Bild 331 von 404, 10 abweichende
  Bilder; wf.log ARMS11 satz=1 MIT=1 OHNE=0`), `integration_r35_werfer_elza` Passed 29.97 s (`Elza PL04, W0F 14 Clips,
  10 Runden mit ofs=(120,1200,0), 10 Explosionen`) — `100% tests passed, 0 tests failed out of 5`.

## 2. Messprotokoll (eigene Laeufe)

exe-Kopien: `re15_port/build/platform/pc/re15_abnB1.exe` (cmp-gleich re15_pc.exe und re15_r35nb1.exe) und
`re15_abnB1_alt.exe` (cmp-gleich re15_r35b.exe = Stand vor Nachbesserung 1). Kein taskkill. Skripte und Laeufe im
Sitzungs-Scratchpad `.../scratchpad/abnB1/` (`run_real.sh`, `run_gen.sh`, `run_gen_alt.sh`, `ana.py` HP-Verlauf aus
state.log, `aud_diff.py` Mischer-Vergleich je Spielbild, `census.py` SCA-Zensus, `sca.py`), Laeufe unter `runs/`.
Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
RE15_DEBUG_JUMP=<raum>@250 RE15_INPUT_SCRIPT_BASIS=spiel RE15_STATE_LOG RE15_WAFFEN_LOG RE15_WPN_DBG=1
RE15_EXIT_AT=<bild>#<raum>`, Vorgabe-KI (kein RE15_AI_FLAVOR). Die Laeufe 2.1-2.5 entstanden in dieser Abnahme vor
einer Unterbrechung (18:37-18:42) mit derselben exe-Kopie, 2.6/2.7 danach (21:3x); alle exit 0.

### 2.1 Echter Weg (Statusschirm -> Item-Debug SELECT + N x R1 -> schliessen -> zielen -> feuern), ROOM1000
Leon (21850,-13400) Blick -x, Skript `S0.1,W2,A0.1,W1,E0.1,W0.3,(M0.1,W0.2) x N,X0.1,W1,S0.1,W3,M0.6,MA1.5,M1.0,W4`.
* 15: `W-bank -> W0F (Clips 11, ...)`, 12 x RE2SPAWN (2 Schuss), Explosionen `0x01110001 -> ARMS0F Satz 10 @(16657,-2083,-13029)` u.a.
* 16: `0x01130001 -> ARMS10 Satz 10 @(17071,-2094,-13030)`; 17: `0x01120001 -> ARMS11 Satz 10 @(17071,-2094,-13030)`.
* 18: `W-bank -> W12`, zwei Raketen, `0x01140001 -> RE2 ARMS11 Satz 20 @(16882,-2728,-12963)` / `@(16855,-2621,-13025)`.
* 14: `W-bank -> W0E`, 15 Strahl-Spawns, `re2arms ARMS10 satz=0` (3x) / `satz=11` (2x).
* 20: `W-bank -> W14`, `SE arms_rec=0 bank=20` (2x).
* Bilder `abnB1/w18_mont.png` (zwei Raketen, Rauchspur, Feuerball an der Spindwand), `abnB1/w15_w14_mont.png`
  (fuenf Runden + Feuerbaelle; durchgehender Flammenstrahl), `abnB1/w20_mont.png`.

### 2.2 Schaden ROOM1140 (Leon (-1676,-18070) Blick 1076, `RE15_GIVE=<id>:6`, Skript `M0.6,MA1.5,M1.0,W4`)
`ana.py`: 15 Platz 2 `F21 hp-150 ss1=9`, Platz 3 `F21 30 -> F57 -170`; 16 `F21 -150 ss1=11`; 17 `F21 -150 ss1=10`;
18 `F26 -850 ss1=17`, P3 `F60 -820`; 14 P2 `35/20/5/-10 ss1=16`, P3 sechs Schritte zu 15; 20 `F19 -850 ss1=5`, P3 `F44 -820`.
= Abnahme 0 §2.2, keine Regression durch die Nachbesserung.

### 2.3 Abnahme-0-M2 Leerschuss Rakete (ROOM1000, `RE15_GIVE=<id>:1`, `RE15_AUDIO_CAP_SYNC=cap.raw`)
MIT `M0.6,MA0.3,M1.6,MA0.2,M2.0,W1` gegen OHNE `...,M0.2,...`: wf.log MIT `F76 pad=8800 ... mag=0` /
`SE  re2arms ARMS11 satz=1`, OHNE ohne diese Zeile. `aud_diff.py`: Rakete `Bilder 404 abweichend 10 erste [331]`,
Bild 331 `mit 33 ohne 17`, 332 `32/15`, 336 `33/24`; GL-Gegenprobe 5 abweichende Bilder (331 `19/17`, 332 `31/15`).
-> Der Leerschuss der Rakete ist jetzt am Mischer da (Abnahme 0: 0 abweichende Bilder).

### 2.4 Abnahme-0-M1 Tisch und neue Kreis-Zellen
* Tisch ROOM1140, Leon (200,-10300) Blick 1024, `M0.6,MA0.3,M1.6,W2`: 18 `0x01140001 @(-174,-2530,-10668)`; 15 fuenf
  Explosionen `@(-102,-2101,-11760)` .. `@(-165,-1801,-11315)`; alle fuenf Zombies HP gleich (unveraendert wie
  Abnahme 0 — jetzt als RE1.5-Schusslinien-Regel belegt, §5.1 Stichprobe 1).
* Kreis ROOM1000, Leon (-1327,-1000) Blick 1024: NEU `RE2FLUG welt=(-1716,-2575,-2390)`, `(-1726,-2607,-3157)`, dann
  `0x01140001 @(-1723,-2596,-2901)`; ALT (re15_abnB1_alt.exe) weiter `(-1737,-2640,-3923) fuss=3`, `(-1748,-2673,-4690)`,
  Explosion erst `@(-1744,-2662,-4435)` = Wand. Kreis-Zellen sperren jetzt.

### 2.5 Abnahme-0-M4 Fresser ROOM1140 (`M0.6,MA1.5,M1.0,W4`)
* Tuer (-7600,-17600): GL 15 zehn Explosionen (u.a. `@(-928,-1892,-20295)`, `@(-2449,-63,-20814)`), alle fuenf Plaetze
  HP unveraendert (`ss1=8`). Python 20 vom selben Punkt: Platz 2 `F19 hp-850`, Platz 5 `F44 -650`.
* Mitte (-5000,-19600): GL Platz 2/3 `F23 -150/-120 ss1=9`, Plaetze 4/5 bleiben 250; Rakete Platz 2/3 `F27 -850/-820`,
  zweite Rakete fliegt an Platz 5 vorbei `@(7673,-2769,-19978)`. = Dossier §8.4, RE2-Gates selbst disassembliert (§5.1).

### 2.6 NEU — Rakete und Strahl durch eine diagonale Zelle (ROOM10E0, Typ 5)
SCA ROOM10E0.RDT Zelle 21 @RDT 0x758 `22 0b 74 0e 8c f1 88 fa 05 ff 00 03` = {w 2850, d 3700, x -3700, z -1400,
**Typ 5**, u0 ff, u1 00, Wort+10 0x0300 = Band 0, Klasse 3 — dieselbe Klasse wie der ROOM1140-Tisch}; dahinter
Rechteck Zelle 9 {x -5000..-600, z 2300..3100}. Fest ist nach push_diag5 (re15_collision.c:530-532, LAB_8003c734)
der Bereich `LINE < ZTERM`, LINE = 3700*(x+3700)/2850, ZTERM = z+1400.
* Lauf `diag_10e0b_w18`: Leon (-3000,-3000) Blick 3072 (+z), `RE15_GIVE=18:4`, `M0.6,MA0.3,M1.6,W2`, Auto-Zielen auf
  die Fresser hinter Zelle 9 (Gier 3137). wf.log:
  ```
  RE2FLUG ... welt=(-2302,-2640,-134) kontakt=0 fuss=3
  RE2FLUG ... welt=(-2215,-2673,627) kontakt=0 fuss=3      LINE 1928 < ZTERM 2027  -> IM festen Dreieck
  RE2FLUG ... welt=(-2128,-2706,1389) kontakt=0 fuss=3     LINE 2041 < ZTERM 2789  -> IM festen Dreieck
  RE2FLUG ... welt=(-2041,-2739,2150) kontakt=0 fuss=3     LINE 2154 < ZTERM 3550  -> IM festen Dreieck
  SE  re2fx code=0x01140001 -> RE2 ARMS11 Satz 20 @(-2070,-2728,1897)
  ```
  Die Rakete fliegt drei Bilder (~1900 Einheiten) durch den festen Teil der Diagonalzelle und explodiert erst nach
  dem Kontakt mit dem Rechteck Zelle 9 (Rueckprall). (Erster Versuch `diag_10e0_w18` von (2000,1000): Auto-Zielen
  drehte auf die Fresser, Explosion am Startpunkt — nicht gewertet.)
* Lauf `diag_10e0b_w14` (Flammenwerfer, `14:100`, `M0.6,MA1.5,M1.6,W2`): 360 Strahl-Plaetze, **47 davon im festen
  Dreieck** (z.B. `(-2310,-1938,501)`, `(-2224,-2024,540)`), der Strahl endet erst an seiner Reichweite (z max 769).
* Ursache: `wandzelle_im_band` (re2_fx.c, Runde-35-Block) prueft ueber der Bandhoehe nur Typ 1 und Typ 3
  (`if (typ != 1u && typ != 3u) continue;`); die Formen 2/4..9 werden uebersprungen (Dossier OFFEN 18, dort mit
  "kommen in ROOM1000/1060/1140 nicht vor" begruendet).
* Zensus aller RDTs (`census.py`, solide Zellen, Bit 1 = 0): Typ 1 17538, Typ 3 1578, **Typ 2/4/5/6/7/8/9 =
  344/228/228/180/200/20/26**. STAGE1-Raeume mit solchen Zellen: 1010 (2), 1050 (4, 5), 1090 (4), 10B0 (5), 10D0 (4-7),
  10E0 (5, 7), 1150 (6, 9), 1190 (7, 9), **11C0 (2, 4, 6, 8 — u.a. Zelle 5 {x -15400..4799, z -13266..4921, Typ 2},
  der Affen-Parkplatz)**, 11D0 (5), 11F0 (7), je mit Raumvariante x1.
* Gegen die eigene Begruendung der Spur: die RE1.5-Schusslinie FUN_8003dcc4 (Beleg der Spur fuer M1) liest je Zelle
  nur Wort+0/+2/+4/+6/+10 und rechnet mit den Ecken x/18, z/18, (x+w)/18, (z+d)/18 (`lhu t1,-6(s7)` / `lhu a0,0(fp)` /
  `lhu t0,-4(s7)` / `lhu v0,-8(s7)` @0x8003ddc8-e8, `sh` @0x8003de28-58; bis @0x8003df18 kein Lesen des Typ-Bytes +8):
  Zelle 21 (Klasse 3, Band 0) sperrt dort die Schusslinie. Die Werfer-Geschosse lassen sie durch.

### 2.7 NEU — Colt Python ohne die Kritklasse des Redhawk (ROOM1140, Leon (200,-10300) Blick 1024)
`run_gen.sh ... tisch_w7 1140 "200,-10300,1024" "7:6" 7 "M0.6,MA0.3,M1.6,W2" 1 150` gegen den Lauf `tisch_w20`
(gleich, `20:6`/20). state.log Platz 5 (Typ 0x11, Fresser, d=9272), Bild 19:
```
w7 : [5 t=11 st=3 ss1=5 ... d=9272 @(200,-19600,r1536)] hp=-1
w20: [5 t=11 st=3 ss1=5 ... d=9272 @(200,-19600,r1536)] hp=-650
```
Grund (RE1.5, selbst disassembliert): `ori v0,zero,0x8 / bne v1,v0` + `sltiu v0,v0,0xbb8` + `ori v0,zero,0x7 / bne`
@0x80012380-a4 und `ori v0,v0,0x40 / sb v0,147(s1)` @0x800123b4-b8 (Waffe 7, oder 8 unter 3000 -> +0x93 |= 0x40);
`andi v0,v0,0x40 / beq` + `sltiu v0,v0,0x20` + `addiu v0,zero,-1 / sh v0,154(s1)` @0x800124fc-1c (Typ < 0x20 -> HP -1).
Port: re15_damage.c:2665 und :2740 nur `weapon_id == 7 || (weapon_id == 8 && best_dist < 3000u)`; die Python-Weiche
re15_damage.c:2695 setzt nur `dmg = dmg_row[7]`. Leser des Bits 0x40 im Port: Gorilla/Affe 0x27
enemy_ai_common.c:9157 und :9206 (im Sprung abgeschossen, @0x80118b14-74) und :9248 (Pin-Abbruch @0x8011a898-8fc).
Der Redhawk holt einen springenden Affen herunter bzw. bricht dessen Griff, die Python nicht. Die Spur erklaert die
Python in §3.5 Punkt 1 ausdruecklich zum "zweiten Magnum-Revolver" mit allen Folgen (Entladung, Spalte 7, RE2-Zeile 5,
Munition, Tester) — die Kritklasse fehlt dort ohne Begruendung.

Nebenbefund am selben Lauf: Redhawk UND Python treffen den fressenden Platz 5 ueber den Konferenztisch (Dossier OFFEN
16/17, allgemeiner Schuss-Pfad, nicht Python-spezifisch — nicht gewertet, §6).

## 3. Urteil je Punkt

### Punkt 1 — Granatwerfer / Raketenwerfer: TEILWEISE
Erfuellt (gemessen 2.1-2.5): Ausruesten ueber den echten Weg, Feuern, RE2-Geschosse, waagerechte Bahn, Explosion an
Rechteck- und jetzt auch Kreis-Zellen, Ton (auch der Leerschuss der Rakete), Schaden an stehenden/aufgewachten Zombies,
Munition/Nachladen, Elza (Test gruen). Die Abnahme-0-Punkte sind erledigt:
* M1 Tisch: belegt — die RE1.5-Schusslinie sperrt an Klasse-3-Zellen des Bandes ohne Hoehe (§5.1, Stichprobe 1);
  die Werfer folgen derselben Regel wie die Handgranate (Spur A). Angenommen.
* M2 Leerschuss: behoben und am Mischer gemessen (2.3).
* M3 Adress-Zitate: behoben (`grep 80074dd4|de0|dec|df8|e04|e10` in re15_port: 0 Treffer; `bytes 0x80074e5c` =
  `06 00 00 00 88 4c 07 80 03 00 00 00`).
* M4 Fresser: RE2-treu belegt (§5.1, Stichprobe 3), gemessen wie im Dossier (2.5).
Nicht erfuellt: **N1** — die Geschosse fliegen durch Zellen der Formen 2/4..9 (gemessen ROOM10E0, 11 STAGE1-Raeume
betroffen, darunter 11C0), dazu das von der Spur selbst gemessene Durchtunneln duenner Zellen (OFFEN 19: Sehne 552 <
Schritt 767, Kreis ROOM1000 von (-1700,-1000) aus).

### Punkt 2 — Flammenwerfer / Colt Python: TEILWEISE
Flammenwerfer: Strahl, Takt, Fuel, Toene, Schaden, Tisch-Regel (wie M1) erfuellt; N1 laesst den Strahl 47 Plaetze tief
in die Diagonalzelle ROOM10E0 laufen (2.6).
Colt Python: Feuer, Blitz/Rauch, Schaden 900 (RE2-Zeile 5), Nachladen erfuellt; **N2** — die erklaerte
Magnum-Zuordnung ist unvollstaendig (Kritklasse +0x93 |= 0x40 / HP -1 fehlt, 2.7).

## 4. Maengel (nummeriert, nachpruefbar)

**N1 — Werfer-Geschosse und Flammenstrahl ignorieren ueber der Bandhoehe alle Zellformen ausser Rechteck (1) und Kreis
(3); die Rakete fliegt durch diagonale Waende.**
Messung 2.6: ROOM10E0 Zelle 21 (Typ 5, @RDT 0x758), drei Flugbilder im festen Dreieck, Explosion erst 1900 Einheiten
spaeter am Rechteck dahinter; Flammenstrahl 47 Plaetze im Dreieck. Betroffen laut Zensus 1226 Zellen in allen Stages,
in STAGE1 die Raeume 1010, 1050, 1090, 10B0, 10D0, 10E0, 1150, 1190, 11C0, 11D0, 11F0 (+x1). Code:
re2_fx.c `wandzelle_im_band` (`if (typ != 1u && typ != 3u) continue;`). Die Spur fuehrt das als OFFEN 18 "bei der
Zusammenfuehrung mit Spur A vereinen" — VERTRAG §2 Regel 4 laesst OFFEN zu, aber am Wortlaut "Raketenwerfer geht"
gemessen ist ein Geschoss, das in 11 Raeumen durch Waende fliegt, nicht fertig. Die eigene Belegkette der Spur (RE1.5-
Schusslinie FUN_8003dcc4 sperrt an jeder Klasse-3-Zelle des Bandes ueber deren Ecken, kein Typ-Lesen) verlangt, dass
auch Zelle 21 sperrt. Erwartet: alle soliden Zellformen 1..9 des Bandes pruefen (formgenau ueber push_diag2..7 bzw.
Spur As Formtest oder belegt ueber die Rechteckecken wie FUN_8003dcc4), dazu der Strecken- statt Punkttest gegen das
Durchtunneln (OFFEN 19); Test: Rakete in ROOM10E0 von (-3000,-3000) Blick 3072 explodiert vor z 2300 (bzw. in 11C0 an
Zelle 5), Gegenprobe mit dem alten Filter rot.

**N2 — Colt Python: die erklaerte PORT-WAHL "zweiter Magnum-Revolver" uebernimmt die Kritklasse des Redhawk nicht.**
Messung 2.7: gleicher Lauf, gleicher Zombie, Bild 19: Redhawk `hp=-1`, Python `hp=-650`. RE1.5 @0x80012380-b8 setzt
+0x93 |= 0x40 fuer Waffe 7 (und 8 < 3000), @0x800124fc-1c HP := -1 fuer Typ < 0x20; Port re15_damage.c:2665/:2740 nur
fuer 7/8. Folge im Spiel: der Affe (Typ 0x27, enemy_ai_common.c:9157/9206/9248 = @0x80118b14-74 / @0x8011a898-8fc)
wird von der Python weder im Sprung abgeschossen noch aus dem Griff geholt, vom Redhawk schon. Erwartet: w20 in die
Kritklasse aufnehmen (als Teil der PORT-WAHL in §3.5 benannt, mit Test: Python-Treffer setzt +0x93 Bit 0x40, Typ < 0x20
-> HP -1) ODER den Ausschluss mit Beleg begruenden.

## 5. Gates

### 5.1 RE-Gate (@0x) — Stichproben SELBST disassembliert
1. RE1.5 Schusslinie (Beleg fuer M1): `jal 0x8001b9b4` / `addu a0,s0,zero` (s0 = Ziel+52) @0x80012168-6c, `bne v0,zero,
   0x80012540` / `addu v0,zero,zero` @0x80012170-74; FUN_8001b9b4 Delta Ziel - `*(0x800ac784)`+0x34/38/3c, Schleife s0 =
   3..0 `ori a2,zero,0xf00 / jal 0x8003dcc4 / ori a3,zero,0x300` @0x8001ba1c-24; FUN_8003dcc4 `lhu a0,0(s7)` /
   `lbu v1,130(v1)` / `sll v0,a0,16 / sra v0,v0,28 / bne` @0x8003de5c-6c, `and v1,t5,a0` ... `bne v1,v0` @0x8003de7c-94,
   danach `jal 0x800663ac` (x/z-Kreuzprodukt) @0x8003defc — kein y. **Stimmt.**
2. RE1.5 Kreis FUN_8003d6a8: `lhu a2,0(a0) / srl a2,a2,1` @0x8003d6cc-d4, Mitte x+r/z+r, `jal 0x80065f60` @0x8003d724,
   `subu s0,s0,v1 / bgtz` @0x8003d730-34 (= pen > 0); Verteiler `sw v0,10340(at)` = 0x800b2864 <- 0x8003d6a8 @0x8003af1c-24
   (Index 3). **Stimmt**, Port `(cr - rr) - dist >= 1` gleichwertig.
3. RE2 Leerzweig: @0x8004382c `jal 0x80069f54`, @0x80043844-54 Flanke `lw 0x800ce310 / andi 0x40`, @0x8004385c
   `jal 0x8006a23c`, @0x80043864 `beq v0,zero,0x80043894` / `lui a0,0x101`, @0x80043878-80 `addiu -9 / sltiu 0x3 / bne`,
   @0x80043894-9c `ori a0,a0,0x1 / jal 0x8005ba28 / addiu a1,s0,56`; 0x8006a23c: `lbu v1,23546(v1)` / `addiu v0,zero,17` /
   `bne` / `j 0x8006a2a4` / `addu v0,zero,zero` @0x8006a284-9c. **Stimmt.** EDH: RE2 ARMS11 `0000 1436 0000 5416 ffff`,
   RE1.5 ARMS12 `0000 1320 ffff ffff`, Satz 10 @0x28 `0000 3320`; shared_assets/RE2/SOUND/ARMS11.EDH cmp-gleich RE2-Disc.
4. RE2 Fresser-Sperre: EMZ0.BIN `lbu v1,467(s1)` @0x80103c04, `ori v1,v1,0x80` @0x80103c0c, `sb v1,467(s1)` @0x80103c14;
   Applier `lbu v0,467(s0) / bne v0,zero,0x8004740c` @0x80047138-40, `lhu v0,270(s0) / andi 0xc000 / bne` @0x80047158-64;
   Resolver `lbu v0,467(s2) / bne v0,zero,0x80041300` @0x80041270-78, `lhu 270 / andi 0xc000 / bne` @0x80041290-9c. **Stimmt.**
5. ROOM1140 Zelle 6 @RDT 0x5d0 `44 2f d4 17 d6 ed d6 bb 01 ff 00 03`. **Stimmt.**
Rate-Marker im Code-Diff (deferred/tunable/interim/for now/faithful/plausib/TODO/vermutlich/getenv): 0 Treffer.
Erklaert der Fix den Befund? M2 ja (ARMS12 Satz 1 = ff -> stumm; RE2-Satz 1 belegt -> hoerbar, MIT/OHNE gemessen);
Kreis ja (alt durch, neu Explosion, Unit-Gegenprobe FAIL 113); M1/M4 ohne Codeaenderung, Beleg traegt.

### 5.2 Vertrag / Pfade
`git diff master --name-only`: engine/src (enemy_ai_re2_zombie.c, game_step_common.c, inventory_common.c, menu_common.c,
player_common.c, re15_damage.c, re2_fx.c, werfer_r35.c neu), include (re15_werfer.h neu, re2_fx.h), platform/pc (main.c,
audio_pc.c, fx_plattform_pc.c), shared_assets/RE2/SOUND/ARMS10/11 (neu, erlaubt), tests (eigene + vier nachgezogene
Alt-Pins + test_support.c), Dossiers. Kein release/, platform/android/, shared_assets/PSX/, keine
tests/unit/CMakeLists.txt / tests/integration/CMakeLists.txt. Nachbesserung 1 in gemeinsamen Dateien: game_step_common.c
+1 Zeile. Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots, Ereignisse. Haelt.

### 5.3 Tests
Messend fuer das Gebaute: unit_r35_werfer (Teil H 100-113 mit echter RDT, Gegenprobe ohne Typ 3 rot), unit_r35_werfer_kombi,
integration_r35_werfer (9 exe-Laeufe), _ton (Mischer, Gegenprobe alte exe rot), _elza. Luecken: kein Test schiesst auf
eine Zelle der Formen 2/4..9 (N1), keiner vergleicht Python und Redhawk im Trefferzweig (N2).

## 6. Sonstige Feststellungen (nicht gewertet)
* OFFEN 16/17 sind allgemein: Redhawk (w7) und Python treffen den fressenden Zombie Platz 5 ueber den Tisch ROOM1140
  (2.7, Bild 19) — RE1.5 sperrt dort die Schusslinie (5.1 Nr. 1), RE2 den Fresser (5.1 Nr. 4). Fuer den Spieler wirkt das
  wie "Revolver treffen, Werfer nicht". Gehoert in den allgemeinen Schuss-Pfad (Orchestrator), nicht in Spur B.
* RE2-Weckausloeser der Fresser = +0x1D4 Bit 0 (Leser EMZ0 `lhu v0,468(s0) / andi 0x1` @0x80104e9c-a4, Aufstehen mit
  `andi 0x7f` auf +0x1D3 @0x80104ef0-04); Schreiber u.a. die Member-Set-Sprungtabelle `sh a2,468(a0)` @0x80055d90 (SCD).
  Der Port weckt nach RE1.5-Abstand 0xFA0 (gekennzeichnetes PORT-MAPPING, nicht Spur B).
* Zusammenfuehrung: re2_fx.c, re15_damage.c, enemy_ai_re2_zombie.c auch bei Spur A/D; N1 ist dort mit Spur As Formtest
  zu loesen, falls die Spur ihn uebernimmt.
