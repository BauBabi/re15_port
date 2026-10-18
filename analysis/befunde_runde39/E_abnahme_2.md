# Runde 39 — Spur E "gegner": UNABHAENGIGE ABNAHME 2 (2026-10-18, nach Nachbesserung 2)

Pruefer: Abnahme-Agent (unabhaengig vom Bau-Agenten). Baum `.claude/worktrees/r39_gegner`, Zweig `r39/gegner`, HEAD b963e6bb,
Merge-Basis mit master 22abaed9 (master = 37011982, nur Doku dazu). Die Punkte 1 (11C0), 2, 3, 4 sind seit Welle 2 in master
(82fc487d); der Zweig traegt darueber nur Nachbesserung 2 (`git diff 22abaed9 HEAD`: 10 Dateien, +1152/-42; Code nur
gegner_r39.c, re15_gegner_r39.h, enemy_ai_common.c (4 Kommentarzeilen), Tests). Gewertet werden trotzdem ALLE vier Punkte am
gebauten Stand dieses Zweigs.

**Kurzurteil**: Punkte 1-4 **erfuellt** (alle selbst an der gebauten exe gemessen, Punkt 1 jetzt in BEIDEN Gorilla-Raeumen 11C0/1141 und mit Vorher-Lauf; Punkte 2-4 je mit Vorher-Lauf der v0.8.26-exe); Gates Vertrag/Tests/RE-Gate halten; Maengel: keine; Hinweise H1-H6 (§8), darunter der vorbestehende Wand-Durchstoss in 1141 (H1, an der master-exe mit Energie 180 reproduziert). **bestanden = true.**

Bericht wurde fortlaufend geschrieben (Abschnitte in Messreihenfolge).

## 0. Bau
- `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do`,
  `LOCAL-BUILD-OK (build)`. exe `re15_port/build/platform/pc/re15_pc.exe` 18:21:19 (juenger als die letzte Quelle
  re15_gegner_r39.h 18:21:07; b963e6bb aenderte danach nur Dossier/Kopfkommentar/cmake-Kommentar). Die exe traegt die neue
  Logzeile `affen init slot=%d raum=%04x typ=%02x` (strings).
- Messkopien: `re15_pc_eabn2_<tag>.exe` neben der exe (Asset-Wurzel = exe-Ordner + Vorfahren), Arbeitsordner
  `scratchpad/E_abn2/<tag>`, Start mit `env -i` (keine geerbten RE15_*), nach dem Lauf geloescht; ein haengender Lauf wurde
  ueber seine eigene PID beendet (kein `taskkill /IM`).
- Vergleichs-exe (nur gelesen, kopiert nach `scratchpad/E_abn2/exe/`): Hauptbaum-exe 14:51 = master 82fc487d (Stand VOR
  Nachbesserung 2: Regel nur in 11C0) und `release/pkg-win/re15_port_v0.8.26/re15_pc.exe` (Stand VOR Spur E).

## 1. Punkt 1 — Gorillas: doppelte Energie, Flinch erst nach 3 Handfeuerwaffen-Treffern (seit Nachtrag 3: ALLE 0x27)

Wortlaut: "Die Gorillas beim Parking Lot können ruhig doppelt so viel Energie haben. Außerdem sollten sie erst die flinch
Animation machen, wenn sie von der Energie her 3x mit der Handfeuerwaffe getroffen wurden, nicht bei jeden Schuss." +
Nachtrag 3 (2026-10-18 14:25): "Zu 2. ja, wir machen das Gorilla Verhalten generell so."

### 1.1 Zensus selbst (alle 240 RDTs, `scd_dump_room.py`, Sce_em_set mit Typ-Byte 0x27)
Nur ROOM11C0 (@0x01784/@0x01798 grid 0x30 Szene, @0x017E8/@0x017FC grid 0x10 Kampf) und ROOM1141 (@0x00B1A/@0x00B2E grid
0x30, @0x00B54/@0x00B68 grid 0x10) — deckungsgleich mit dem Dossier. Die Regel `re15_r39_affen_regel` = Haken an && Typ 0x27
(kein Raum) deckt damit alle Gorillas beider Szenarien.

### 1.2 ROOM11C0 (Leon), Kampf-Layout — Gegenprobe zum Stand der Abnahme 1
Messkopie `p1_11c0_kampf`: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp
RE15_TITLE_SHOT_AF=2 RE15_SET_FLAG=4:0x40 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-6729,-12800,1486 RE15_EQUIP=3
RE15_INPUT_SCRIPT=M1.5,MA0.1,M0.6,...(14 Paare ausgeschrieben) RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=290
RE15_STATE_LOG=state.log RE15_R39_LOG=1 RE15_EXIT_AT=900` -> exit 0 (39 s).
- gegner_r39.log: `affen init slot=2/3 raum=11c0 typ=27 hp=360 (tabelle 180 @0x8011f514, regel 1)`; `F335 hp=348 akku=12 ->
  OHNE ZUCKEN`, `F379 hp=336 akku=24 -> OHNE ZUCKEN`, `F440 hp=324 akku=36 -> FLINCH (Schwelle 36)`, `F462 hp=312 akku=12 -> OHNE`.
- state.log (eigene Auswertung `gor.py`): F335 360->348 `st=1/5/1/0 mo=18`, F379 348->336 `st=1/3/1/0 mo=5` (kein Clip 7),
  **F440 336->324 `st=2/3/1/1 mo=7` = einziger Flinch-Beginn, Ende F452 -> `st=1/7`**, F462 324->312 `st=1/7 mo=20` ohne Clip 7.
  Bildgleich mit Abnahme 1 und dem Bau-Agenten (F335/F379/F440/F462): der Wechsel Raum-Tor -> Typ-Regel aendert 11C0 nicht.

### 1.3 ROOM11C0 (Leon) ueber den SZENEN-Weg (neu in dieser Abnahme)
Messkopie `p1_11c0_szene`: Debug-Sprung 11C0@240 mit Leon an (-22604,14455) (Aufbau integration_r39_gegner_schatten, OHNE
Flag 4/0x40) -> Gorilla-Szene (beide 0x27 im Spawn-Bild INIT mit 360), Steuerung frei ab F1085; ab Spielbild 1090 Pistole
(`RE15_EQUIP=3`, Skript `M0.5` + 18 x (`W0.1,M0.6` + 6 x `MA0.1,M0.3`)). gegner_r39.log, alle Treffer-Zeilen:
G2 `F1126 348 akku 12 OHNE`, `F1150 336 akku 24 OHNE`, `F1174 324 akku 36 FLINCH`, `F1243 312 akku 12 OHNE`, `F1312 300 akku 24
OHNE`, `F1429 288 akku 36 FLINCH`, `F1511 276 akku 12 OHNE`, `F1534 264 akku 24 OHNE`, `F1894 252 akku 36 FLINCH`; G3 `F1219 348
akku 12 OHNE`, `F1267 336 akku 24 OHNE`, `F1837 324 akku 36 FLINCH`. state.log: Clip-7-Beginne in Zustand 2 genau F1174, F1429,
F1837, F1894 (sonst keiner). Leon stirbt F2294 (nur 15 Schuss, das Skript laedt nicht nach — Messaufbau, kein Befund); danach
CONTINUE-Schleife ohne EXIT_AT -> Messkopie ueber ihre eigene PID beendet.

### 1.4 ROOM1141 (Elza) — der neue Teil der Nachbesserung 2, echter Weg 1131 -> Tuer -> 1141
Aufbau = integration_r39_gegner_1141 (Wegplan 1131 Schluessel + Tuer -> 1141 Phase H -> Gemaelde -> BEFREIUNG; `RE15_SET_FLAG=
3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78 RE15_GIVE=0x43:1,0x04:99,0x15:60 RE15_EQUIP=4 RE15_DEBUG_JUMP=1131@gp RE15_PLAYER_POS=
-1300,-13900,0 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, Schuetze `W10` + 80 x (`W0.1,M0.6` + 6 x `MA0.1,M0.3`) ab
Spielbild 2760, `RE15_EXIT_AT=4500`).
- **integration_r39_gegner_1141 selbst gefahren**: Passed 268,3 s, `r39_gegner_1141: OK — INIT 360 x 4, 20 Treffer-Zeilen
  (FLINCH 6 nur bei akku >= 36, OHNE ZUCKEN 14), Flinch-Beginne im state.txt 6` (Lauf lauf_c35e5638).
- **eigener Lauf OHNE Messhaken (`n1141_ohne`, kein RE15_R39_MESS_SPIELER_HP)**: exit 0, EXIT_AT 4500 erreicht. `affen init
  slot=2/3 raum=1141 typ=27 hp=360 (... regel 1)` je zweimal (Raumaufbau + BEFREIUNG). 19 Treffer-Zeilen, FLINCH 6 (alle akku 36),
  OHNE 13 (akku 12/24), regelwidrig 0. G2: F3081 OHNE 12 / F3105 OHNE 24 / **F3129 FLINCH 36** / F3198 / F3420 / **F3466** / F3651 /
  F3675 / **F3732** / F3942 / F4011 / **F4059** / F4128 / F4152 / **F4233**; G3: F3513 / F3756 / **F3780** / F4433. state.txt: Clip-7-
  Beginne in Zustand 2 genau F3129, F3466, F3732, F3780, F4059, F4233 (= die FLINCH-Zeilen). Die stehende Elza stirbt F4439
  (hp 4 -> -2). Die 19 Treffer-Zeilen sind identisch mit denen des Riegel-Laufs (mit Messhaken; der hat dazu nur F4476, nach
  Elzas Tod im Lauf ohne Haken) — die Gorilla-Seite haengt also nicht am Messhaken.
- **Vorher-Lauf mit der Hauptbaum-exe 14:51 (`v1141_master`, master 82fc487d = Regel nur in 11C0), gleicher Aufbau**: `affen init
  slot=2/3 raum=1141 hp=180` (x4); dieselben Schussbilder, aber **jeder Treffer flincht**: F3081 180->168 Clip 7, F3105 168->156
  Clip 7, F3129 156->144 Clip 7, F3198, F3222, F3303, F3360 (je Clip 7). = der Zustand, den Nachtrag 3 beseitigen will.
- Unit selbst (Ausgabe gelesen): `unit_r39_gegner_1141_hp1141` vorher 180/180, nachher 360/360, Regel in 11C0/1141/2000 an, 0x29
  aus; `_zucken1141` (echte P228-Schuesse) vorher Clip 7 = 1/1/1, nachher 0/0/1, 360 -> 324, Akku 12/24; `_ablauf1141` vorher 16
  Treffer / 15 Flinches je Gorilla, nachher 31 / 10, Kill-Flags (7,0xCB)/(7,0xCC) Bild 1858, (4,0xF2) = 0 Bild 1859, John geparkt
  Bild 2026, Dachtuer -> 0x17; `_affe29` 0x29 Bild fuer Bild gleich.
### 1.5 Voll-Ablauf ROOM1141 an der exe — Variante v11 des Bau-Agenten selbst nachgefahren
Aufbau wie §1.4 + `RE15_R39_MESS_SPIELER_HP=100`, Skript = `scratchpad/e_nb2c/schuss_v11.txt` des Bau-Agenten (`W11` + 80 x
(`W0.1,M0.6` + 6 x `MA0.1,M0.3`), kopiert), `RE15_INPUT_SCRIPT_START=2760`, `RE15_EXIT_AT=11500` (Lauf `n1141_v11`).
- gegner_r39.log (eigene Zaehlung): Platz 2 = 20 x OHNE + 10 x FLINCH, Platz 3 = 20 x OHNE + 10 x FLINCH, regelwidrig 0; je letzte
  Zeile hp=0 (F6507 / F5670) — der 31. Treffer toetet: state.txt **G3 F5739 `st 1 -> 3` hp -12, G2 F6576 `st 1 -> 3` hp -12**
  (F5809 / F6646 -> 7). **John (Platz 1, Typ 0x49) setzt sich F6697 in Bewegung und endet geparkt (-30000,-30000)**. Elza bleibt
  per Messhaken bei 100 (jeder Ausgleich <= 12). Bildgleich mit den Angaben des Bau-Agenten (G3 F5739, G2 F6576; seine Zeile
  "F6678 los" = Szenenbeginn pm=2, die Positionsaenderung misst mein Auswerter ab F6697).
- Damit ist der Voll-Ablauf "Kampf mit 2 x 360 -> Tod beider Gorillas -> John geht" an der exe belegt (mit Messhaken, weil eine
  stehende Skript-Elza ohne Ausweichen den Kampf nicht ueberlebt — `n1141_ohne`: Tod F4439 nach 19 Treffern).

- Urteil Punkt 1: **erfuellt** — an der exe in BEIDEN Gorilla-Raeumen (11C0 Leon: Kampf-Layout + Szenen-Weg; 1141 Elza: echter
  Tuer-Weg, mit und ohne Messhaken) INIT 360 = 2 x 180 und Clip 7 nur bei akku >= 36 (= jeder dritte 12er-Treffer); vorher
  (master-exe) in 1141 180 und Clip 7 bei jedem Treffer. Den Voll-Ablauf (Tod beider Gorillas -> John) s. 1.5.

## 2. Punkt 2 — kein paralleler Hundebiss (RE2-Hundemodul)

Wortlaut: "Wenn ich einmal von einem Hund gerade angegriffen/gebissen werde, soll ich nicht parallel noch von anderen Hunden
angegriffen/gebissen werden können, wenn ich mich nicht wehren kann, so zum Beispiel bei Room 1090."
Code seit Abnahme 1 unveraendert (in master; `git diff 22abaed9 HEAD` beruehrt enemy_ai_re2_dog.c nicht, gegner_r39.c Teil (2)
nur um den Messhaken-Aufruf am Kopf von `re15_r39_hund_riegel_tick` ergaenzt, ohne Env wirkungslos: `n = -1` -> return).

- Zensus selbst (`scd_dump_room.py`): ROOM1090/1091 tragen KEINE Hunde (0x10/0x11 Zombies, 0x26 Feuer, 0x42 NPC); die Hunde-
  Szene (3 x 0x20 grid 0x40 + 3 x grid 0x00) liegt in ROOM1190/1191 — die Zuordnung "Room 1090" = 1190 (wie Spur B) stimmt.
- **integration_r39_gegner_hunde1190 selbst gefahren**: Passed 42,3 s, `4 Bisse (F685/F720/F753/F833 je -20), alle Abstaende >= 24
  Bilder; Riegel gesetzt 4 x / frei 4 x` (lauf_e749c6a9). Eigene Auswertung (`hunde.py`): im Biss-Bild je genau EIN Hund in Sub 3/3
  (F685 H1 d=949, F720 H2 d=2039, F753 H3 d=949, F833 H3 d=1286); gegner_r39.log `GESETZT F685 / FREI F708`, `F720 / F743`,
  `F753 / F776`, `F833 / F856` = je 23 Bilder (22 Bilder Flinch + Exit-Bild). Bildgleich mit Abnahme 1.
- **Spielszene vorher/nachher ROOM11C1 (Elza, fuenf Hunde 0x20)**, Karte `probe_r36_elza_karte re15_card.mcr 11C1 -22604 14455 0`,
  beide Laeufe `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
  RE15_CARD_SLOT=0 RE15_STATE_LOG=state.log RE15_EXIT_AT=1150` (debug.log `Spieler-Familie PL04 (character=4, Elza-Bit=1)`):
  - **vorher = v0.8.26-exe** (`p2_11c1_v0826`): Bisse F779 / F862 / F906 / F947 / **F1008 (20 -> 0, H4 Sub 3/3) und F1019 (0 -> -20,
    H4 UND H5 in Sub 3/3, d=1305 / 951)** — 11 Bilder nach dem ersten, mitten in Elzas Reaktion = der Nutzer-Befund als Spielszene.
  - **nachher = exe dieses Zweigs** (`p2_11c1_nachher`): bildgleich bis F1008; in F1008 stehen H2 und H5 in Sub 8/1 (abgewiesen)
    statt im Biss; Riegel `GESETZT F1008 / FREI F1031`; **naechster Biss F1040** (H1 Sub 3/1) = 32 Bilder spaeter, nach dem Ende
    der Reaktion. Alle Riegel-Paare 23 Bilder (F779/802, F862/885, F906/929, F947/970, F1008/1031). Abstaende 83/44/41/61/32.
  - Identisch mit Abnahme 1 §2.2/2.3 (deterministisch).
- Unit selbst gruen: unit_r39_gegner_hund_riegel, unit_r39_gegner_hund_re15 (RE1.5-Flavor nativ), alle unit_*dog* / *hund* der
  Gruppe (s. §7).
- Urteil Punkt 2: **erfuellt** (Mechanismus PL+0x1D3 & 0x80 nach RE2, an der exe als Spielszene vorher/nachher belegt).

## 3. Punkt 3 — liegende Zombies kriechen (ROOM1010/1011, ROOM1220/1221)

Wortlaut: "Die Zombies in ROOM 1220/1221 und in ROOM 1010 die am Boden liegen kriechen im Original. Das sollen sie bei uns auch."
Code seit Abnahme 1 unveraendert (in master). RE-Beleg selbst disassembliert (STAGE1.BIN `dis 0x80104700 22`): `lw v0,464(v0)`
(+0x1d0 Distanz) @0x80104710, **`sltiu v0,v0,0xbb8` @0x80104718**, `beq` @0x8010471c, `jal 0x8001a9cc` (arc_test) @0x8010472c mit
**`ori a1,zero,0x200` @0x80104730**, `bne` @0x80104734, **`ori v0,zero,0x1` @0x80104738 / `sw v0,4(v1)` @0x80104748** = Zustandswort 1
(Kriech-Lokomotion) — genau `re15_r39_kriecher_weckt` (dist < 0xbb8, arc 0x200) + `re15_ai_set_state_word(e, 1u)`.

Alle vier Raeume ueber den ECHTEN Tuer-Weg (Tuer-AOT des Nachbarraums, `RE15_FIRE_AOT` = `re15_aot_fire_slot`, derselbe Pfad wie
das Hineinlaufen), danach geht der Spieler per `RE15_WEGPLAN` (`GEHE`) auf die Liegenden zu. Tuer-Zensus selbst: ROOM1020/1021
Door 0 @0x01C82 -> Raum 0x01 Cut 4 (Ziel (3650,-3950)); ROOM1210/1211 Door 1 @0x01CE6 -> Raum 0x22 Cut 0 (Ziel (-22400,-6500),
rot 2048) = sub00 `Switch` Case 0 @0x00F24 mit den beiden 0x81-Saetzen @0x00F2A/@0x00F3E. Gemeinsame Env: `RE15_NO_INTRO=1
RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_STATE_LOG=state.log RE15_R39_LOG=1`; Wegplan `RAUM <r> / WARTE 60 /
GEHE 2500 -2700 150` (1010/1011) bzw. `GEHE -23500 -7700 150` (1220/1221) `/ WARTE 400`, `RE15_EXIT_AT=600#<r>`.

| Lauf | Weg | gemessen (state.log / gegner_r39.log, `kriech.py`) |
|---|---|---|
| `p3_1010_leon` | `RE15_DEBUG_JUMP=1020@240 RE15_FIRE_AOT=0@60#1020` | F6 beide 0x10 g=81 `st=1 ss1=2 mo=23` (liegend) bei (950,-1700) d=3510 / (-50,-3800) d=3700; **F104 `kriecher weckt slot=3 dist=2985`**, **F111 `slot=4 dist=2996`** -> `ss1=0 mo=5`; K3 kriecht 742 Einheiten bis zum Griff F126 (`ss1=1 mo=14`), Leon hp 100->95 F137 ->90 ->85; K4 kriecht 2293 Einheiten bis zum Griff F270, hp 80 F281 |
| `p3_1220_leon` | `RE15_DEBUG_JUMP=1210@240 RE15_FIRE_AOT=1@60#1210` | F6 beide 0x16 g=81 liegend (-25398,-6739) d=3007 / (-23500,-9700) d=3376; **F98 `slot=1 dist=2941`**, **F103 `slot=2 dist=2996`**; K2 kriecht 803 bis zum Griff F133, hp 95 F144 ...; K1 kriecht 1360 bis zum Griff F276 |
| `p3_1011_elza` | `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=1021@gp RE15_FIRE_AOT=0@60#1021` (`[elzaems] Typ 0x10 aus EMD/CDEMD1.EMS`) | bildgleich mit Leon 1010: F104/F111 Wecker (2985/2996), Griff F126, Elza hp 95 F137 |
| `p3_1221_elza` | wie oben, `RE15_DEBUG_JUMP=1211@gp RE15_FIRE_AOT=1@60#1211` (`[elzaems] Typ 0x16`) | bildgleich mit Leon 1220: F98/F103 Wecker (2941/2996), Griff F133, Elza hp 95 F144 |
| `p3_1010_v0826` (VORHER, v0.8.26-exe, gleicher Weg/Wegplan) | wie p3_1010_leon | beide liegen 600 Bilder (`st=1 ss1=2 mo=23`, Weg 0) = der Nutzer-Befund |
| `p3_1220_v0826` (VORHER) | wie p3_1220_leon | beide liegen 600 Bilder, Weg 0 |

- Unit selbst gruen: unit_r39_gegner_kriecher_1010 / _1220 (RE2: vorher Weg 0, nachher Wort 1), _1010_re15 / _1220_re15 (Block B
  nativ), unit_re2_crawler, unit_re2z_crawl_hurt_reentry, unit_re2z_crawler_spawn_wait, unit_1030_crawl_live, unit_crawl_toggle,
  unit_r36_blut_pin_kriecher*, unit_r38_griff_kriecher2_1220.
- Urteil Punkt 3: **erfuellt** (alle vier Raeume, beide Figuren, ueber die Tuer; vorher/nachher an der exe).

## 4. Punkt 4 — Gorilla-Schatten beim Sprung am Boden

Wortlaut (Nachtrag 2026-10-18): "Die Gorillas haben beim springen direkt ihren Schatten unter dem beim." Code seit Abnahme 1
unveraendert (main.c-Haken `re15_r39_schatten_y` -> +0x1ba fuer 0x27/0x29).
- **integration_r39_gegner_schatten selbst gefahren**: Passed 58,7 s, `39 Sprung-Bilder (Sub 7/8), 428 Luft-Bilder mit Quad-Y ==
  Boden-Y (+0x1ba @0x80116f30), Szenen-Gorillas 404 Quad-Zeilen (404 auf -3600; Cut 13 keine, Cut 12 115 Wagen-Zeilen)`.
- **Bildvergleich mit dem BESCHLEUNIGTEN Renderer** (kein RE15_SOFTWARE_RENDER; debug.log `SDL-Renderer direct3d`; RE15_FRAMEDUMP
  liest den fertig komponierten Backbuffer vor dem Present, main.c:11368 / render_pc.c:1693): Debug-Sprung 11C0@240, Leon an
  (-22604,14455), `RE15_FRAMEDUMP=1350-1395/5:fd_`, je einmal mit der v0.8.26-exe (`p4_sprung_v0826`) und dieser exe
  (`p4_sprung_nachher`). gegner_r39.log nachher: F1362-F1385 Gorilla Platz 3 `st=1/7`, `koerper_y` -720 .. -4680 .. -720,
  `boden_y=0 quad_y=0` (24 Zeilen). Bilder `scratchpad/E_abn2/p4_sprung_zoom.png` (Ausschnitt x40-280/y0-200, 3x):
  **vorher F1365/F1370/F1375 haengt eine dunkle Schatten-Ellipse in der Luft unter dem springenden Gorilla (F1375 deutlich oben
  im Bild ueber dem Wagen); nachher liegt der Blob auf dem Asphalt (F1365/F1370 unten links am Boden), in der Luft keiner.**
- Weitere Laeufe: in jedem eigenen Gorilla-Lauf gilt quad_y == boden_y fuer jede `schatten`-Zeile (1141 ohne Messhaken: 276 Zeilen,
  172 davon mit Koerper ueber dem Boden in Sub 7/8/15; 11C0-Szenenweg: 552 Zeilen, 154 in der Luft) — 0 Abweichungen.
- Urteil Punkt 4: **erfuellt**.

## 5. RE-Gate (Nachbesserung 2) — Stichproben SELBST disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`)

| Behauptung (Diff/Dossier) | selbst gelesen | Ergebnis |
|---|---|---|
| Typ 0x27 -> Wurzel 0x80116db8 nur ueber die STAGE1-Registrierung @0x8011e900-0c (RE15_R39_AFFEN_TYP) | STAGE1 `dis 0x8011e8f0 10`: `lui v0,0x8011` @0x8011e900, `addiu v0,v0,28088` (= 0x80116db8) @0x8011e904, `lui at,0x8007` / `sw v0,11336(at)` (= 0x80072c48 = 0x80072bac + 0x27*4) @0x8011e90c | stimmt |
| HP-Zeile 0x27 @0x8011f514 = 16 x 180, Saat ohne hp-Bedingung `sh v0,154(a1)` @0x80117104 | `read 0x8011f514 16 --w 2` = 16 x 180; `dis 0x801170c8 18`: rng `jal 0x8001af20` @0x801170cc, Basis 0x8011f034 @0x801170d4-d8, `andi 0xf` @0x801170e4, `lbu v1,8(a1)` / `sll v1,v1,5` (Typ*0x20) @0x801170e8-f0, `lhu` @0x801170fc, `sh v0,154(a1)` @0x80117104 | stimmt |
| Pistole 12 @0x8006ee44, SIG P228 (Waffe 4) 12 @0x8006ee48 | PSX.EXE `read 0x8006ee38 22 --w 2 --stride 4` = `[0, 7, 30, 12, 12, 28, ...]` (Index 3 @0x8006ee44 = 12, Index 4 @0x8006ee48 = 12); Port re15_damage.c:431 `s_wpn_dmg_maggot[22]` identisch; Item 0x04 = "SIG P228" (inventory_common.c:415) | stimmt |
| Waffe 4 flincht auf Spur 0 (Tabelle @0x801214a8) | `read 0x801214a8 22 --w 4`: Index 0-6, 12, 14, 19, 20 = 0x8011b018; 7/8/13/21 = 0x8011b1ec; 9-11/15-18 = 0x8011b400 | stimmt |
| Tod erst bei hp < 0 (`bgez` @0x80012528) | PSX.EXE `dis 0x80012500 12` + `dis 0x80012530 8`: `lh v1,154(s1)` @0x80012520, `ori v0,zero,0x2` @0x80012524, `bgez v1,0x80012420` @0x80012528, Delay `sb v0,4(s1)` @0x8001252c; Fallthrough `ori v0,zero,0x3` / `j` / `sb v0,4(s1)` @0x80012530-38 | stimmt (360/12 + 1 = 31 Treffer) |
| Messhaken: Finisher nur bei player.hp < 50 (`slti` @0x80118cf4); Gorilla-Abzuege -6 @0x80118468, -12 @0x801187bc | STAGE1 `dis 0x80118cd0 36`: `lbu v1,149(v0)` == 0x13 @0x80118cd4-dc, `lh v0,0(a2)` player.hp @0x80118cec, `slti v0,v0,50` @0x80118cf4, `beq` @0x80118cf8, hit_react @0x80118d04-0c, `jal 0x8001a804` @0x80118d18, +0x1dc @0x80118d34-3c, Sub 8 `sb v0,5(v1)` @0x80118d44; `addiu v0,v0,-6` @0x80118468, `addiu v0,v0,-12` @0x801187bc | stimmt |
| Blut + Se 3 beim Treffer ohne Zucken = Flinch-Eintritt @0x8011b0bc-138 | `dis 0x8011b05c 40` + `dis 0x8011b0fc 20`: +0x93 \|= 2 @0x8011b064-70, Clip 7 `ori v1,zero,0x7 / sb v1,148(v0)` @0x8011b08c-90, Blutvektor-Tabelle 0x80121388 @0x8011b0d4-110, `jal 0x80019700` @0x8011b12c, `jal 0x800453d0 / ori a0,zero,0x3` @0x8011b134-38 | stimmt |
| Befund E: Koerper-Stoss @0x8002b22c-260, naehere Kante FUN_8003bca8 @0x8003bcec-cfc | PSX.EXE `dis 0x8002b1d0 38`: SquareRoot0 @0x8002b1d8, pen `subu s0,s0,a2` @0x8002b1e4 / `blez` @0x8002b1e8, Hoehenband @0x8002b208-228, `mult s1,s0` / `addiu a2,a2,1` / `div v0,a2` @0x8002b22c-238; `dis 0x8003bca8 30`: t1 = Ostkante+18 @0x8003bce0, a0 = Westkante-18 @0x8003bce8, `subu v0,zero,a0` @0x8003bcec / `slt v0,v0,t1` @0x8003bcf0 / `beq` @0x8003bcf4 / `addu t1,a0,zero` @0x8003bcfc | stimmt |
| Punkt 3 (unveraendert): Block B @0x80104718-48 | s. §3 | stimmt |

- Kennzeichnung: Faktor 2 (`RE15_R39_AFFEN_HP_FAKTOR`) und 3 Treffer (`RE15_R39_ZUCK_TREFFER`) als NUTZER-VORGABE; der Akku-Bezug
  (`s_hp_ref`/`s_ref_ok`, Neusetzen bei HP ueber dem Bezug) als PORT-WAHL der Buchfuehrung — im Spiel setzt ihn der INIT
  (`re15_r39_affen_hp_init`), der Neusetz-Zweig greift nur fuer Aktoren ohne INIT (Sonden). Jede neue Konstante traegt @0x....
- Tabu-Woerter (deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/hack) im Code-Diff und in gegner_r39.c,
  re15_gegner_r39.h, test_r39_gegner*.c/.cmake: keine.
- Neuer Env-Schalter: nur `RE15_R39_MESS_SPIELER_HP` (Messhaken, ohne Env `n = -1` -> sofort return, kein Spielverhalten; nicht
  als Abschluss eines Punktes benutzt — die Regel ist ohne ihn an der exe gemessen, §1.4 `n1141_ohne`).
- Erklaert der Fix den Befund? Ja: vorher (master-exe) in 1141 `hp=180` + Clip 7 bei jedem Treffer, weil `re15_r39_affen_parkplatz`
  am Raum 0x11C0 hing; nachher `regel 1` fuer jeden Typ 0x27, gleiche Schussbilder, Clip 7 nur bei akku >= 36.

## 6. Nebenwirkung "Befund E" (Wand-Durchstoss in ROOM1141) — selbst gemessen: VORBESTEHEND, nicht von dieser Spur
Der Bau-Agent meldet als OFFEN, dass im 1141-Kampf ein Gorilla (Versuch 5) bzw. Elza (Versuch 6) durch eine 2000 dicke Wand
geschoben werden kann, und dass die doppelte Energie es "wahrscheinlicher" mache. Eigene Laeufe mit demselben Skript-Schuetzen,
Auswerter `aussen.py` (Spieler x < -8600 oder z < -23600 bzw. Gorilla jenseits der Innenkanten = ausserhalb des Raums):

| Lauf | exe / Energie | ausserhalb |
|---|---|---|
| `v1141_master` | Hauptbaum-exe 14:51 (master 82fc487d), 1141-Gorillas **180**, Flinch je Treffer | **Elza ab F3504**: F3502 (-8132,-9269) -> F3504 (-11068,-9837) -> F3505 (-11068,-11367), bleibt bis F4500 draussen; Elza im Opfer-Pin `gr=1 mo=1`, G2 in Sub 15 Phase 3 `mo=28` bei (-6974..-6982,-9440..-9633) |
| `v1141_v0826` | v0.8.26-exe, **180** | niemand (anderer Kampfverlauf: erster Treffer F3174) |
| `n1141_ohne` | diese exe, **360**, ohne Messhaken | niemand (Elza stirbt F4439) |
| integration_r39_gegner_1141 (lauf_c35e5638) | diese exe, **360** | niemand bis F4500 |
| `n1141_v11` | diese exe, **360**, Messhaken | niemand bis F11500 |

- -8132 = Innenkante der Westwand fuer den Spieler (-8600 + 450 + 18), -11068 = Aussenkante (-10600 - 450 - 18): derselbe
  Naehere-Kante-Ausstoss FUN_8003bca8 (@0x8003bcec-cfc, selbst gelesen, §5), den der Bau-Agent fuer Versuch 5/6 beschreibt; der
  Sprung ueber die Wandmitte (-9600) passiert im Opfer-Pin (Runde-35-Pin, `gr=1`), nicht in einem Code-Pfad dieser Spur.
- **Folgerung**: Der Durchstoss tritt mit der UNVERAENDERTEN Energie 180 auf (master-exe, bildgenau reproduzierbar mit dem Aufbau
  §1.4 + `scratchpad/E_abn2/skript_1141.txt`), ist also kein Fehler der Nachbesserung 2. Er ist ein Softlock-Risiko des 1141-Kampfes
  (Elza ausserhalb = weder Kampfende noch Dachschloss erreichbar) und gehoert zu Spur J (1141-Umbau, Welle 3) bzw. in den NACHLAUF
  (Schreiber der Pin-Platzierung ueber die Wandmitte bestimmen; RE2-Gegenstueck des Wandausstosses lesen). -> Hinweis H1, kein
  Mangel dieser Spur.

## 7. Vertrag, Pfade, Tests
- Pfade (`git diff 22abaed9 HEAD --name-only`, nur Nachbesserung 2): analysis/befunde_runde39/E_gegner.md, engine/src/enemy_ai_common.c
  (4 Kommentarzeilen der vorhandenen Haken), engine/src/gegner_r39.c, include/re15_gegner_r39.h, tests/integration/
  test_r39_gegner_1141.cmake (neu), tests/unit/probes/r39_gegner.cmake (Sonden-je-Thema), tests/unit/test_maggot_ai.c (+2: include
  + `re15_r39_gegner_enable(0)`, Kommentar "Runde 39 Spur E"), test_r35_affen.c (Kommentar), test_r39_gegner.c (Teil affen_hp auf
  "generell"), test_r39_gegner_1141.c (neu). Ueber alle Spur-E-Dateien (inkl. der in master liegenden): keine release/,
  platform/android/, shared_assets/PSX/, keine CMakeLists.txt; die Haken in den gemeinsamen Dateien (enemy_ai_common.c 5 + include,
  enemy_ai_re2_dog.c 3 + include, enemy_ai_re2_zombie.c 1 + 2 Kommentarzeilen + include, main.c 1 + include) tragen je
  "Runde 39 Spur E". Bank-9-Bits / Nachrichten-Ids / Slots / Ereignisse: keine belegt. `git status` vor dem Bericht sauber.
- test_maggot_ai.c / test_r35_affen.c schalten die Spur-Haken ab (`re15_r39_gegner_enable(0)`), weil sie den ORIGINAL-Mechanismus
  (Saat 180, Flinch je Treffer) gegen Bytes/Savestates messen; die Nutzer-Vorgabe misst test_r39_gegner / test_r39_gegner_1141 je
  vorher/nachher im selben Lauf. Kein Original-Riegel wurde dadurch "gruen gemacht", der die Vorgabe pruefen sollte.
- Neue Tests parallel-/Linux-fest: eigener Laufordner `r39_gegner_1141_wd/lauf_<zufall>`, Messkopie `re15_pc_r39e_1141_<zufall>`,
  geerbte RE15_* der Liste ungesetzt, `cmake_minimum_required(VERSION 3.16)`, keine .exe-/Backslash-Pfade (`LAST_EXT`), `m` fuer
  Nicht-WIN32 in probes/r39_gegner.cmake.
- Je Punkt messende Riegel: P1 unit_r39_gegner_affen_hp/_zucken/_schuss, unit_r39_gegner_1141_hp1141/_zucken1141/_ablauf1141/_affe29,
  integration_r39_gegner_1141; P2 unit_r39_gegner_hund_riegel/_hund_re15, integration_r39_gegner_hunde1190; P3 unit_r39_gegner_
  kriecher_1010/_1220/_1010_re15/_1220_re15; P4 unit_r39_gegner_schatten, integration_r39_gegner_schatten.
- **Selbst gefahren (Spurbaum, gezielt, keine volle Suite)**:
  - Unit: `ctest -j3 -R` (Ausdruck `scratchpad/E_abn2/regex_unit.txt`: alle unit_* zu maggot|affen|gorilla|1141|11c0|r39_gegner|
    dining|elzaems|szene11c0|griff|hund|dog|kriech|crawl|schatten|body_push + die Liste des Bau-Agenten) -> **`100% tests passed,
    0 tests failed out of 149`** (13 s).
  - Integration: `-R "^integration_r39_gegner_1141$"` -> Passed 268,3 s; `-j1 -R "^integration_r39_gegner_(hunde1190|schatten)$"` ->
    `100% ... out of 2` (42,3 s / 58,7 s); `-j2 -R "^integration_(r38_dining1141_.*|r37_elzaems_elza1141|r37_szene11c0|r38_affen3|
    r38_affen3_decke|r38_affen3_tor)$"` -> **`100% tests passed, 0 tests failed out of 12`** (dining1141 a 293 s, d 316 s, s 612 s,
    r 210 s, w 210 s, z 117 s, l 41 s; szene11c0 120 s; elzaems_elza1141 12 s; affen3 84 s, _decke 93 s, _tor 90 s).
  - Zusammen 149 Unit + 15 Integration gruen; kein Fenster-Haken musste nachgefahren werden.

## 8. Hinweise (keine Maengel gegen den Wortlaut)
- **H1 (Befund E, vorbestehend, §6)**: Wand-Durchstoss im 1141-Kampf ist mit Energie 180 an der master-exe reproduzierbar (Elza
  ab F3504 hinter der Westwand). Softlock-Risiko des Raums; Zustaendigkeit Spur J (1141-Umbau) bzw. NACHLAUF: Schreiber, der den
  Opfer-Pin (Runde 35, `re15_affen_pin_anker` / Opfer-Anker, `gr=1`) ueber die Wandmitte setzt, und das RE2-Verhalten des
  Wandausstosses bestimmen. Die doppelte Energie verlaengert den Kampf (mehr Pin-Gelegenheiten), erzeugt den Fehler aber nicht.
- **H2 (Integration)**: `RE15_R39_LOG` ist bei Spur E ein Schalter, bei Spur F (zustand_r39.c:58) ein Dateiname — in jedem
  Messordner dieser Abnahme lag deshalb eine Datei `1` (Spur-F-Log). Harmlos, bei der Integration einen Namen trennen.
- **H3 (Integration)**: integration_r39_gegner_1141 haengt am Runde-38-F-Ablauf von 1141 (Wegplan bis zum Gemaelde, Kampfbeginn
  ~F2760, EXIT_AT 4500 mit ~600 Bildern Reserve). Baut Spur J die Szene um, ist der Wegplan/EXIT_AT nachzuziehen.
- **H4 (fuer den Nutzer)**: die Schwelle ist energie-basiert ("von der Energie her"): Browning HP / SIG P228 (je 12) zucken beim
  3. Treffer; Beretta M93R / Glock 18 (je 28, @0x8006ee4c / @0x8006ee50) beim 2.; Messer (7) beim 6.; Schrotflinte (38) und
  Magnum (50) sofort. Das Dossier nennt Messer/Schrot/Magnum, nicht die beiden staerkeren Handfeuerwaffen.
- **H5 (vorbestehend, wie Abnahme 1 H1)**: Groesse des Gorilla-Schatten-Quads (Original +0xbc/+0xbe = 1700/1800) gegen Port
  500/600 — nicht Teil des Nutzer-Punkts (Lage am Boden ist erfuellt).
- **H6 (Messaufbau)**: ein stehender Skript-Schuetze ueberlebt den 1141-Kampf gegen 2 x 360 nicht (Tod F4439 nach 19 Treffern);
  der Voll-Ablauf an der exe braucht deshalb den Messhaken. Fuer den Spieler (der ausweicht) ist das kein Befund; Munition liegt
  laut Dossier-Zensus reichlich auf Elzas Weg (nicht selbst nachgezaehlt).

## 9. Maengel
Keine.

## 10. Ergebnis

| Punkt | Urteil | Kern des Belegs (selbst gemessen) |
|---|---|---|
| 1 Gorillas HP x2 + Flinch erst nach 3 Handfeuerwaffen-Treffern (alle 0x27, Nachtrag 3) | **erfuellt** | 11C0 Kampf-Layout F335/F379 ohne Clip 7, F440 FLINCH akku 36 (360 -> 324); 11C0 Szenen-Weg 12 Treffer, Clip 7 nur F1174/F1429/F1837/F1894; 1141 Elza (Tuer-Weg) ohne Messhaken 19 Treffer, FLINCH nur bei akku 36, Clip-7-Beginne == FLINCH-Zeilen; vorher (master-exe) 1141 hp 180 + Clip 7 bei jedem Treffer; v11 nachgefahren: je Gorilla 30 Treffer-Zeilen (10 FLINCH), Tod mit dem 31., John geht |
| 2 kein paralleler Hundebiss | **erfuellt** | 11C1 Elza (5 Hunde): v0.8.26 Biss F1008 + F1019 (11 Bilder, in der Reaktion), diese exe F1008 -> Riegel FREI F1031 -> naechster Biss F1040; 1190: 4 Bisse, Riegel je 23 Bilder |
| 3 liegende Zombies kriechen (1010/1011, 1220/1221) | **erfuellt** | alle vier Raeume ueber die Tuer, Leon und Elza: Wecker bei dist 2985/2996 bzw. 2941/2996, Kriechen bis zum Griff, HP-Abzug; v0.8.26 auf demselben Weg: liegen 600 Bilder |
| 4 Gorilla-Schatten beim Sprung am Boden | **erfuellt** | beschleunigter Renderer, Framedump F1365-F1375: vorher Ellipse in der Luft, nachher am Boden; gegner_r39.log quad_y == boden_y in allen Laeufen (0 Abweichungen) |

Gates: Vertrag/Pfade OK, Tests OK (149 Unit + 15 Integration selbst gefahren), RE-Gate OK (9 Stichproben der Nachbesserung 2 +
Block B selbst disassembliert, alle stimmen). **bestanden = true.**

## 11. Pruefprotokoll (Werkzeuge, Ablage)
- Disasm: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (STAGE1.BIN / PSX.EXE), Zensus `re15_port/tools/scd_dump_room.py`
  (alle 240 RDTs; `scratchpad/E_abn2/zensus27.txt`).
- Laeufe: `scratchpad/E_abn2/run.sh` (Messkopie `re15_pc_eabn2_<tag>.exe`, `env -i`, Kopie danach geloescht), Laufordner
  `scratchpad/E_abn2/{p1_11c0_kampf, p1_11c0_szene, n1141_ohne, v1141_master, v1141_v0826, n1141_v11, p2_11c1_nachher,
  p2_11c1_v0826, p3_1010_leon, p3_1220_leon, p3_1011_elza, p3_1221_elza, p3_1010_v0826, p3_1220_v0826, p4_sprung_nachher,
  p4_sprung_v0826}`; Auswerter `gor.py`, `hunde.py`, `kriech.py`, `elzapos.py`, `aussen.py`; Bilder `p4_sprung_vergleich.png`,
  `p4_sprung_zoom.png`; Vergleichs-exe `scratchpad/E_abn2/exe/` (aus dem Hauptbaum nur kopiert).
- Ein Lauf (`p1_11c0_szene`) lief nach Leons Tod in die CONTINUE-Schleife ohne EXIT_AT und wurde ueber die eigene PID 36904
  beendet (`Stop-Process -Id`), kein `taskkill /IM`.
