# Runde 37 — Spur I feuerloescher — UNABHAENGIGE ABNAHME 1 (2026-10-12)

Baum `.claude/worktrees/r37_feuerloescher` (Zweig `r37/feuerloescher`, Basis master 64ba31e0, Kopf 969ac9c9).
Massgeblich: `AUFTRAG.md` Nachtrag 4 (+ 4b, 4c), `VERTRAG.md`. Geprueft wird der Stand NACH Nachbesserung 1
(Commits 4b1b1ec2..969ac9c9) — alle Punkte neu an der GEBAUTEN exe gemessen, nicht nur M1/M2.

**Ergebnis: BESTANDEN.** M1 (Item-Box-Weg) und M2 (Abschnittsverweis) aus Abnahme 0 sind behoben; alle Punkte des
Nutzers (Nachtrag 4, 4b, 4c) sind an der gebauten exe neu gemessen erfuellt; RE-Gate, Vertrag und Tests halten.

Hinweis zur Ausloesung: Der Auftrag, der diesen Workflow angestossen hat (Nachtrag 3b, Leuchtfarbe der Code-Panels:
"hintergrund rot - rot leuchten - hintergrund blau - blau leuchten"), gehoert laut AUFTRAG.md (Nachtrag 3b) und
VERTRAG.md Abschnitt 0 zur Spur E codepanel. Spur I ist dafuer nicht zustaendig; geprueft wird hier nur, dass Spur I
keine Panel-Datei anfasst (Abschnitt 2.3).

## 0. Werkzeug der Abnahme
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`, `... build` ->
  `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. exe `re15_pc.exe` 14:35:46 juenger als die letzte Code-Aenderung
  `feuerloescher_r37.c` 14:35:43; Arbeitsbaum sauber (`git status --short` nur dieser Bericht); die neue Messzeile
  `kein Loescher (Leon hat ihn in die Item-Box gelegt ...)` steht in der exe (`grep -c` = 1).
* Messkopie `re15_port/build/platform/pc/re15_pc_r37i_abn1_mess.exe` (eigener Name, nie `taskkill /IM`, nach der
  Abnahme geloescht). Spielstand mit dem Kartenwerkzeug `probe_r37_feuerloescher_karte` (gelesen: schreibt Lage/Flags/
  Inventar/Kiste ueber `re15_savedata_capture`; `kiste:` belegt den naechsten freien Box-Platz ab 2), Start
  `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0` (Titel -> LOAD GAME), Eingabe `RE15_INPUT_SCRIPT` mit
  `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60` (A = Viereck/Aktion, X = Kreuz/Abbruch, S = Start,
  L/R drehen, U vor; input_pc.c `script_bit_for`), Ende `RE15_EXIT_AT=<bild>[#<raum>]` (main.c: Ende am Bild, mit
  `#raum` nur in diesem Raum), Logs `debug.log` (`RE15_MSG_LOG=1`), `RE15_STATE_LOG`, `RE15_DISCARD_LOG`,
  `RE15_FLAG_TRACE`; Bilder `RE15_FRAMEDUMP` mit `RE15_WINDOW_SCALE=1` (PPM, 2x vergroessert nach PNG).
  `RE15_BOX_TEST=1` setzt EINMAL vor der Hauptschleife `re15_itembox_set_pending(1)` (main.c:5051) = derselbe
  Box-Schirm wie die Box-Zone. Lauf-Skript `lauf.sh` + Rohdaten im Scratchpad der Abnahme (`iabn1/<lauf>/`).
* Belegbilder (2x vergroessert) im Baum unter `re15_port/build/abnahme_i1/` (git-ignoriert, NICHT committet); im
  Text nur mit Dateinamen genannt.
* Beide Messkopien (`re15_pc_r37i_abn1_mess.exe`, `re15_pc_r37i_abn1_v0824.exe`) nach den Laeufen geloescht (vorher
  `tasklist`: kein laufender Prozess). Kein Code im Baum geaendert; committet wird nur dieser Bericht.
* ⛔ Messfalle (gemessen, Lauf a1): `RE15_FRAMEDUMP=<bild>:<datei>` schreibt bei JEDEM Erreichen des Bilds — nach dem
  Tuer-Weg zaehlt `g_engine.frame_count` im Zielraum von 0, das Bild F130 von ROOM1050 wurde vom Bild F130 in ROOM1090
  ueberschrieben. Box-Bilder daher in einem eigenen Lauf mit Ende vor der Tuer (a1v).

## 1. Messprotokoll je Nutzer-Punkt

### 1.1 Nachbesserung 1 / M1 — "it can only lay there - if leon ... select 'yes'" (Item-Box-Weg) -> BEHOBEN
Alle Laeufe mit `lauf.sh <name> <ende> <skript> <karte...> [-- ENV]` (Abschnitt 0), Karte jeweils im NEIN-Zustand
(Feuer aus (3,0x81), Loescher NICHT weggeworfen).
* **a1 (= L8 der Abnahme 0, Box-Schirm + TUER-Weg):**
  `lauf.sh a1 "200#1090" "W1,A0.1,W0.7,A0.1,W1,X0.1,W0.7,X0.1,W2,A0.5,W6" 1050 19850 -22300 1024 cut:6 flag:9:0x86 flag:3:0x85 flag:3:0x80 flag:3:0x81 leer give:0x31:1 -- RE15_BOX_TEST=1`
  ```
  [save] CONTINUE: resumed in room 1050 (hp=100)
  [feuerloescher] ROOM1050: kein Loescher (schon genommen, (9,0x86)=1)
  [feuerloescher] Name Item 0x31: Empty Fire Extinguisher
  [tuer] Sequenz Archiv 2 DOOR1D Port-Archiv P1DK Variante 2 Bit7 0 Tuer 29 Seite S022 T014 Spender 07 (17 Skripte)
  [room] PC loaded room1090.rdt (336440 bytes)
  [feuerloescher] ROOM1090: kein Loescher (Leon hat ihn in die Item-Box gelegt (Item 0x31 in der Kiste, Platz 2))
  [flow] EXIT_AT: Bild 200 in Raum 1090 erreicht -> exit
  ```
  KEINE Zeile `liegt leer am Boden`, KEIN `oid=0x05`. Derselbe Eingang lieferte am Stand 52062d05 (Abnahme 0, L8)
  `ROOM1090: Loescher liegt leer am Boden ...` — der Befund ist an der exe weg.
  **a1v** (dieselbe Eingabe, Ende `135#1050`, `RE15_FRAMEDUMP=80-130/50:fd_`): F80 Box-Schirm, der Loescher steht in der
  ITEM LIST ("Empty Fire Extinguisher"); F130 nach dem Tausch steht "Empty Fire Extinguisher" in der ITEM BOX, die
  Item-Liste ist leer (Bilder `a1v_box_vorher_inventar_F80.png`, `a1v_box_nachher_loescher_in_box_F130.png`).
* **a3 (Kiste im Spielstand + TUER-Weg, ohne Box-Schirm):**
  `lauf.sh a3 "200#1090" "W1,A0.5,W12" 1050 19850 -22300 1024 cut:6 flag:9:0x86 flag:3:0x85 flag:3:0x80 flag:3:0x81 leer kiste:0x31:1`
  -> `[tuer] Sequenz ... DOOR1D ...`, `[room] PC loaded room1090.rdt`,
  `[feuerloescher] ROOM1090: kein Loescher (Leon hat ihn in die Item-Box gelegt (Item 0x31 in der Kiste, Platz 2))`;
  kein `oid=0x05`.
* **a4 (ECHTE Box ROOM1150 + Sprung, = L8r/N):**
  `lauf.sh a4 "120#1090" "W1,A0.1,W3,A0.1,W0.7,A0.1,W1,X0.1,W0.7,X0.1,W3" 1150 -23000 -12100 2048 flag:9:0x86 flag:3:0x85 flag:3:0x80 flag:3:0x81 flag:7:0xf8 flag:7:0xf9 flag:7:0xfa flag:7:0xfb leer give:0x31:1 -- RE15_DEBUG_JUMP=1090@330 RE15_PLAYER_POS=2500,-2400,3072,1`
  ```
  [msg] room=1150 id=3 ITEMBOX (box screen, message suppressed)
  [feuerloescher] Name Item 0x31: Empty Fire Extinguisher
  [debug-menu] AUTO-JUMP -> ROOM1090 (Frame 330)
  [feuerloescher] ROOM1090: kein Loescher (Leon hat ihn in die Item-Box gelegt (Item 0x31 in der Kiste, Platz 2))
  ```
  Bild `a4_1150_echte_box_F210.png`: echte Box, "Empty Fire Extinguisher" in der ITEM BOX, Item-Liste leer.
* **a5 (HERAUSNEHMEN, Gegenrichtung des Tauschs):** Karte wie a3 (`kiste:0x31:1`, Inventar leer) + `RE15_BOX_TEST=1`,
  Skript wie a1 (leerer Inventar-Platz 0 <-> Box-Platz 2) -> `[feuerloescher] ROOM1090: kein Loescher (Leon traegt ihn
  noch (Item 0x31 im Inventar))` — der Loescher kommt zurueck ins Inventar, es liegt weiter keiner.
* **a6 (Spielstand/CONTINUE in ROOM1090 mit dem Loescher in der Kiste, = O):** Karte 1090 (2500,-1800,-2400) rot 3072
  Cut 6, Zustand nach der Rettung (9,0x86)(3,0x85)(3,0x80)(3,0x81)(3,0xBB)(3,0x6E), Zombies 0xF8..0xFB erledigt, Inventar
  leer, `kiste:0x31:1`, `W2`, Ende 100, `RE15_FRAMEDUMP=60:fd_F60.ppm` -> `ROOM1090: kein Loescher (Leon hat ihn in die
  Item-Box gelegt (Item 0x31 in der Kiste, Platz 2))`, kein `oid=0x05`; Gegenstueck **a7** (gleiche Karte ohne Kiste =
  Ja-Zustand) -> `Loescher liegt leer am Boden` + `[prop-render] pi=4 oid=0x05 pos=(2500,-1800,-1300) rot=(0,3250,0)`.
  Bild `a6_a7_1090_cut6_kiste_vs_ja.png` (links a6: Boden vor der Wagentuer leer; rechts a7: der Loescher liegt).
* Zusammenfassung der Marker (Zaehlung `grep -c` in debug.log):
  ```
  a1   oid05=0 liegt=0 weggeworfen=0 kiste=1 traegt=0
  a3   oid05=0 liegt=0 weggeworfen=0 kiste=1 traegt=0
  a4   oid05=0 liegt=0 weggeworfen=0 kiste=1 traegt=0
  a5   oid05=0 liegt=0 weggeworfen=0 kiste=0 traegt=1
  a6   oid05=0 liegt=0 weggeworfen=0 kiste=1 traegt=0
  a7   oid05=1 liegt=1 weggeworfen=0 kiste=0 traegt=0
  b1   oid05=1 liegt=1 weggeworfen=1 kiste=0 traegt=1
  b2   oid05=0 liegt=0 weggeworfen=0 kiste=0 traegt=1
  b3   oid05=0 liegt=1 weggeworfen=0 kiste=0 traegt=0   (Ankunfts-Cut 3 an der Tuer zeigt den Platz nicht -> kein Zeichnen)
  g1   oid05=0 liegt=0 weggeworfen=0 kiste=1 traegt=0
  ```
* **b3 (Gegenprobe JA-Zustand, TUER-Weg):** Karte 1050 vor der Notausgangs-Tuer, (9,0x86)(3,0x85)(3,0x80)(3,0x81)(3,0xBB)
  (3,0x6E), Inventar leer, Kiste leer, Skript L3 der Abnahme 0 -> `[msgtext] room=1050 id=8 "Leon: Hey, wait!"`
  (Ada-Szene), Tuer, `[feuerloescher] ROOM1090: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250,
  Zone Slot 6 Untersuchen msg 29` — das Tor sperrt NUR mit dem Loescher in Inventar/Kiste.
* Beobachtung ohne Mangel: In der Box-Zeile laeuft "Empty Fire Extinguisher" in das Symbol (a1v F130); der laengste
  Original-Name "Minidisc Player w/ Disc" tut dasselbe (Abnahme 0, `l8m_box_vergleich_minidisc_F130.png`) —
  vorhandenes Box-Layout.

### 1.2 Punkt 3 + Nachtrag 4b — "bei Leon nach der Nutzung erstmalig auftauchen" / "only ... if ... 'yes'" -> ERFUELLT
* **e1 (vor der Nutzung, echter Weg):** Karte 1050 (17200,-22500) rot 1024 Cut 6, Spielbeginn-Flags, Skript L1 der Abnahme 0
  (`W1,A0.1,W3.5,A0.1,W3,L0.3667,W0.3,U1.2,W0.3,R0.3667,W0.3,A0.5,W10`, Ende `140#1090`):
  ```
  [feuerloescher] ROOM1050: Loescher an der Wand obj 5 bei (17200,-1150,-23775) rot_y 0, Zone Slot 13 Aufnahme Item 0x31 Bit (9,0x86)
  [prop-render] pi=2 oid=0x05 pos=(17200,-1150,-23775) rot=(0,0,0) meshes=1
  [feuerloescher] Aufnahme Item 0x31 (9,0x86) -> Set(3,0x85)=1 wie ROOM1000 sub01 @0x00D0C
  [tuer] Sequenz Archiv 2 DOOR1D ...
  [room] PC loaded room1090.rdt (336440 bytes)
  [feuerloescher] ROOM1090: kein Loescher (Feuer brennt noch, (3,0x81)=0)
  ```
  Bild `e1_1050_aufnahme_modal_F150.png`: Aufnahme-Modal "Will you take the ..." mit dem ITPS-Bild.
* **b1 (Nutzung + JA):** Karte 1090 (0,-1800,250) rot 0 Cut 13, (9,0x86)(3,0x85)(3,0x80), Einsatzausruestung + Loescher,
  Skript `W1` + 40 x `,A0.1,W1.9`, Ende 1500, `RE15_DISCARD_LOG=discard.log RE15_FLAG_TRACE=1`:
  ```
  [feuerloescher] ROOM1090: kein Loescher (Feuer brennt noch, (3,0x81)=0)
  [msgtext] room=1090 id=7 "I must hurry up and get something to put out this fire to save that woman!"
  [msgtext] room=1090 id=8 "Will you use the Fire Extinguisher?"
  [feuerloescher] ROOM1090: kein Loescher (Leon traegt ihn noch (Item 0x31 im Inventar))
  [msgtext] room=1090 id=9 "You've used the Fire Extinguisher."
  [feuerloescher] Loescher weggeworfen -> liegt in ROOM1090
  [feuerloescher] ROOM1090: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250, Zone Slot 6 Untersuchen msg 29
  [prop-render] pi=4 oid=0x05 pos=(2500,-1800,-1300) rot=(0,3250,0) meshes=1
  [msgtext] room=1090 id=3 "Leon: There! Are you okay?"   ... id=4, id=5, id=6
  ```
  discard.log: `F511 ... abfrage=1 frage=8 gegenstand=0x31 ... gefragt=1`, `F594 ... text=45/45`, `F630 ... abfrage=0 ...
  gefragt=1 weg=1`. Bilder `b1_1090_wegwerfabfrage_F600.png` ("You don't need this item any more. Discard it? ▸Yes No"),
  `b1_1090_cut7_liegt_nach_ja_F780.png` (Cut 7 der Rettungsszene, der Loescher liegt links unten vor dem Reifen).
* **b2 (Nutzung + NEIN):** wie b1 mit `leer give:0x31:1`, 10. Druck = X (Skript L4c der Abnahme 0): KEIN `weggeworfen`,
  KEIN `liegt leer am Boden`, Rettungsszene id=3..6 laeuft; discard.log `F629 ... abfrage=1 ... text=45/45 ... weg=0`,
  `F630 abfrage=0 ... gefragt=1 weg=0`.
* **b3 (Wiederbetreten ueber die TUER nach JA)** und **a7 (Spielstand/CONTINUE nach JA)**: `Loescher liegt leer am Boden`
  (b3 nach der Ada-Szene 1050 "Leon: Hey, wait!"), a7 zusaetzlich `[prop-render] pi=4 oid=0x05 pos=(2500,-1800,-1300)`.
  Bild `a6_a7_1090_cut6_kiste_vs_ja.png`: links Kiste (a6, Boden vor der Wagentuer leer), rechts Ja (a7, der Loescher
  liegt vor Leons Fuessen).
* **f3 (Untersuchen nach JA):** Karte 1090 (2500,-1800,-2400) rot 3072 Cut 6, Ja-Zustand, Zombies erledigt, `W1,A0.1,W4`
  -> `Loescher liegt leer am Boden ...`, `[prop-render] pi=4 oid=0x05 ...`, `[msgtext] room=1090 id=29 "It's an empty
  fire extinguisher."`; Bild `f3_1090_leon_msg29_nach_ja_F150.png`.
* **Wiederholung a1r** (a1 identisch nochmal, unter Last): dieselben Zeilen, `Platz 2`, kein `oid=0x05` — reproduzierbar.
* **Nein + Kiste:** Abschnitt 1.1 (a1, a3, a4, a6) — kein Prop.

### 1.3 Punkt 1 — "ein Modell des Feuerloeschers" -> ERFUELLT
* Generator unabhaengig nachgefahren: Kopie von `tools/r37_feuerloescher/*.py`, `tools/r36_p/*.py`, `include/font_width.h`,
  `ITEM/ITPS.ITP`, `STAGE4/ROOM4050.RDT` in einen leeren Scratchpad-Baum (`iabn1/gen/`), `python3
  re15_port/tools/r37_feuerloescher/feuerloescher_modell.py` -> `Laenge/Durchmesser 2.346`, `R = 110`,
  `Hoehe 792 (0.47 m)`, `Faktor 0.899`, `MD1 stehend 7940 B, liegend 7940 B, TIM 16928 B`; `cmp` gegen
  `engine/src/gen/feuerloescher_r37.inc`: **BYTEGLEICH** (sha256 `d55976f9...d2f581` beide). Die Eingaben
  `ITPS.ITP`, `ROOM4050.RDT`, `ROOM1000/1050/1090/1091.RDT` in shared_assets sind bytegleich mit `info/Re1.5/PSX/`.
  Generator und `.inc` seit Abnahme 0 unveraendert (`git diff 52062d05..HEAD` leer).
* An der exe gezeichnet: Wand ROOM1050 Cut 6 (c1, `c1_1050_cut6_wand_F120.png` + Zoom `c1_1050_wand_zoom.png`: roter
  Loescher mit Etikett und Hebel rechts der EXIT-Doppeltuer), liegend ROOM1090 Cut 6 (a7) und Cut 7 (b1 F780), ROOM1091
  Cut 6 (c2). Kein Texturfehler sichtbar.

### 1.4 Punkt 2 — "bei elza einen Leeren Feuerloescher am Boden" -> ERFUELLT
* **d1 (TUER-Weg 1051 -> 1091):** `lauf.sh d1 "150#1091" "W1,A0.5,W6" 1051 19850 -22300 1024 cut:6 elza` ->
  `[tuer] Sequenz ... DOOR1D ...`, `[room] PC loaded room1091.rdt (282404 bytes)`,
  `[feuerloescher] ROOM1091: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250, Zone Slot 3 Untersuchen msg 29`;
  keine `[feuerloescher] ROOM1051`-Zeile (Elza bekommt keinen Wand-Loescher).
* **c2 (Untersuchen):** Karte 1091 (1880,-1800,-1300) rot 0 Cut 6, Elza, Zombies 0xF8..0xFB erledigt, `W1.5,A0.1,W5` ->
  `[prop-render] pi=4 oid=0x05 pos=(2500,-1800,-1300) rot=(0,3250,0)`, `[msgtext] room=1091 id=29 "It's an empty fire
  extinguisher."`; Bilder `c2_1091_elza_liegt_F80.png` (Loescher vor Elza an der offenen Wagentuer), `c2_1091_elza_msg29_F140.png` (Text).

### 1.5 Punkt 4 — Spind -> "woanders platzieren" -> ERFUELLT
* Neuer Fundort ROOM1050 Wand rechts der Notausgangs-Tuer: e1 (Aufnahme ueber Eingaben, Set(3,0x85), Tuer -> 1090), c1 (Bild).
* **m1 (Wand nach der Aufnahme):** Karte 1050 (17200,-22500) rot 1024 Cut 6, `W1,A0.1,W3.5,A0.1,W3,R0.5,W0.5,U2,W1`,
  Ende 430 -> `Aufnahme Item 0x31 (9,0x86) -> Set(3,0x85)=1 ...`; Bild `m1_1050_wand_vor_nach_aufnahme_F60_F420.png`
  (links F60 Leon vor der Wand, rechts F420 nach der Aufnahme: die Wand rechts der Tuer ist leer).
* **e3 (TUER-Weg 1050 -> 1000):** `[tuer] Sequenz Archiv 2 DOOR07 ...`, `[room] PC loaded room1000.rdt`,
  `[feuerloescher] ROOM1000: Spind Slot 3 (Item 0x31 @0x00C24) -> Raumtext msg 0`.
* **e2 (Spind untersuchen):** Karte 1000 (20900,-1100) rot 3072, `W1,A0.1,W4` -> `[msgtext] room=1000 id=0 "Nothing
  unusual."`, keine Aufnahme; Bild `e2_1000_spind_nothing_unusual_F140.png`.
* Der Fundort selbst ist PORT-WAHL (Dossier 3.2/7, Entscheidung beim Nutzer).

### 1.6 Nachtrag 4c — NEIN -> "Empty Fire Extinguisher" im Inventar -> ERFUELLT
* **b2 (echter NEIN-Weg, danach Statusschirm -> ITEM -> CHECK):** `[feuerloescher] Name Item 0x31: Empty Fire
  Extinguisher`, `[inv] CHECK-Foto Item 0x31 ...`, `[feuerloescher] CHECK-Text Item 0x31: An empty fire extinguisher.`;
  Bilder `b2_status_empty_fire_extinguisher_F1560.png`, `b2_check_an_empty_fire_extinguisher_F1790.png`, Uebersicht `b2_nein_status_check_uebersicht.png`.
* **f1 / f2 (Spielstand laden):** CONTINUE 1090 mit (3,0x81) + Loescher -> Statusschirm "Empty Fire Extinguisher"; ohne
  (3,0x81) (vor dem Loeschen) -> "Fire Extinguisher" (Bild `f1_f2_name_nach_vs_vor_dem_loeschen.png`, links/rechts).
* In der Kiste traegt er denselben Namen (a1v, a4).

### 1.7 M2 — Abschnittsverweis -> BEHOBEN
`grep -n "Abschnitt 3\.[0-9]" engine/src/feuerloescher_r37.c` -> nur Zeile 99 `Dossier Abschnitt 3.4 + Nachbesserung 1`;
im Dossier steht die Tor-Regel in 3.4.

### 1.8 Beobachtung des Bau-Agenten nachgemessen (kein Spur-I-Mangel)
**g1:** Karte 1090 am Feuer, (9,0x86)(3,0x85)(3,0x80), Inventar leer, `kiste:0x31:1`, Skript wie b1 -> msg 7, msg 8
"Will you use the Fire Extinguisher?", Ja, `ROOM1090: kein Loescher (Leon hat ihn in die Item-Box gelegt ...)`, msg 9,
Rettungsszene id=3..6; discard.log bis F1500 `gefragt=0 weg=0` (keine Wegwerf-Abfrage). Bestaetigt: Leon loescht auch
mit dem Loescher in der Kiste. Ursache ist die ORIGINAL-Regel ROOM1090 sub00 @0x02306 `06 00 24 00 21 03 85 00`
(nur Ck(3,0x85), selbst gelesen), die es schon vor Spur I gab ((3,0x85) setzte vorher ROOM1000 sub01 @0x00D0C beim
Spind — gleiche Lage); Spur I hat daran nichts geaendert. **Gegenprobe v0.8.24** (Hauptbaum-exe 09:02, ohne
`[feuerloescher]`-Code, Kopie `re15_pc_r37i_abn1_v0824.exe`, dieselbe Karte, dieselbe Eingabe, Lauf g1m): msg 7, msg 8,
msg 9, id=3..6, discard.log `F1500 ... gefragt=0 weg=0` — dasselbe Verhalten VOR Spur I. Folge fuer die Nutzer-Punkte: kein Doppel, kein Prop ohne
Ja (4b gewahrt), der Loescher bleibt "Empty Fire Extinguisher" (4c sinngemaess, in der Kiste). Ob das Loeschen den
Loescher im Inventar verlangen soll, ist eine Orchestrator-/Nutzerfrage ausserhalb dieser Spur.

## 2. Gates

### 2.1 Bau und Tests
* `local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `local_build.sh build` -> `ninja: no work to do.`,
  `LOCAL-BUILD-OK (build)`.
* Spur-Riegel direkt (Arbeitsordner im Scratchpad): `test_r37_feuerloescher.exe kiste` -> `16 Pruefungen, 0 Fehler`;
  ohne Argument (alle Teile in einem Prozess) -> `92 Pruefungen, 0 Fehler`.
* **Mutationsprobe der Abnahme (selbst gebaut, nichts im Baum geaendert):** Kopie von `feuerloescher_r37.c` im Scratchpad
  (`iabn1/mut/`), Tor-Zeile ohne `&& in_der_kiste(RE15_FL37_ITEM) < 0`, mit den Flags aus `build.ninja` uebersetzt
  (`gcc -DRE15_ASSETS_PATH=... -DRE15_PLATFORM_PC=1 -I include -I engine/src -std=c11`) und VOR `libre15_engine.a` mit
  dem gebauten Test-Objekt gelinkt: `test_mut.exe kiste` -> `16 Pruefungen, 3 Fehler`
  (`FAIL: K Tor zu, solange der Loescher in der Kiste liegt`, `FAIL: K Wiederbetreten mit dem Loescher in der Kiste:
  kein Prop obj 5, kein Platz msg 29 (kein Doppel)`, `FAIL: K geladen + Raumstart: keiner liegt`); Kontrolle mit der
  unveraenderten Kopie auf demselben Weg -> `16 Pruefungen, 0 Fehler`. Der neue Riegel misst den Befund M1.
* Gezielte Gruppe (`ctest -R`, `-j 3`, `--timeout 600`, unter Last paralleler Agenten; derselbe Ausdruck wie Abnahme 0
  und Bau-Agent, 290 Tests), Ausdruck woertlich:
  `^(unit_r37_feuerloescher|integration_r37_feuerloescher|unit_aot_edge|unit_scd_aot_opcodes|unit_savedata|unit_item_modal|unit_item_stack_press|unit_aot_sce_census|unit_sicherung_1150|unit_re2_room1090_ab|unit_1090_|probe_1090_|unit_plc_back_yaw_1090|unit_map_1090|unit_ada_eintritt_1050|unit_r19_marke3_1050_c1_tiefe|unit_r21_discard_wegwerfen|unit_r27_ada_cutscene_1050|unit_r30_|unit_r34n_|unit_r35_|unit_room1050_sicherung|unit_r36_|unit_inv_fsm|unit_item_icon|unit_inv_grant|unit_room1150_itembox|unit_itembox_transfer|unit_itembox_screen|unit_debug_menu|unit_pri_inventar_riegel|unit_r26_inventar|unit_msg_dialog|unit_action_msg_gate|unit_text_freeze|integration_item_name_census|integration_flag_census|integration_r36_inventar|integration_r36_schluessel|integration_r35_inventar1050|integration_r36_story_leon|integration_r36_story_elza|integration_r34n_e_dokumente_bild|integration_r36_kartenitems_bild)`
  Zwischenstand: `integration_r37_feuerloescher` **Passed 439.01 sec** (Laeufe A..O dieses Laufs `lauf_e0267629_*`:
  M/N/O je `kein Loescher (Leon hat ihn in die Item-Box gelegt (Item 0x31 in der Kiste, Platz 2))`, A `weggeworfen` +
  `liegt leer am Boden`, I ohne `weggeworfen`, J/K Name/CHECK, L ohne Ersatz-Name); `integration_r36_story_leon/elza`,
  `integration_r36_inventar`, `integration_r36_schluessel(_befreiung)`, `integration_r35_inventar1050`,
  `integration_r34n_e_dokumente_bild`, `integration_r36_kartenitems_bild` Passed.
  `unit_r35_android_bash_mutanten` **Failed 1447.24 sec**: `[FEHLER] AEQUIVALENT-Eintrag passt zu keinem gleichwertigen
  Mutanten mehr: A (( rc == 0 )): == -> <=`, Ursache in der Ausgabe: `[erkannt] Zeile 317: A (( rc == 0 )): == -> <=
  <- Zeitlimit` (Mutantenlauf ueber 300 s, `bash_urteil_mutanten.py:323`, `Laufzeit 1445 s, 10 parallel`) — ein
  Last-Zeitlimit. Der Test mutiert `release/apk_pruefen.sh` gegen `r35_android/urteil_kontrollen.sh`; Spur I beruehrt
  keine seiner Eingaben (`git diff master...HEAD --name-only | grep -E "release/|r35_android|platform/android"` leer).
  **Endergebnis der Gruppe:** `99% tests passed, 1 tests failed out of 290` (`Total Test time (real) = 1555.61 sec`),
  `The following tests FAILED: 480 - unit_r35_android_bash_mutanten (Failed)`. Alle zehn Spur-Riegel gruen:
  `integration_r37_feuerloescher` 439.01 s, `unit_r37_feuerloescher_{kiste,raum,loeschen,spielstand,leer,aufnahme,
  modell,spind,elza}` Passed. **Einzel-Nachlauf** des roten Tests (`ctest -R '^unit_r35_android_bash_mutanten$'
  --timeout 600`): `1/1 Test #480: unit_r35_android_bash_mutanten ...   Passed  945.74 sec`, `100% tests passed, 0 tests
  failed out of 1` — der rote Gruppenlauf war das Last-Zeitlimit, keine Regression. Damit 290/290 gruen.

### 2.2 RE-Gate (Stichproben SELBST disassembliert, richtige Binaerdatei)
* RE1.5 `info/Re1.5/PSX.EXE`, `re15_disasm.py dis 0x8004dfec 22` (FUN_8004dfec, Inventar-Suche):
  `8004dff0 lbu v0,4028(v0)` (0x800b0fbc Zaehler), `8004e014 addiu at,at,4268` (0x800b10ac Basis),
  `8004e01c lbu v0,0(at)`, `8004e024 bne v0,a0,0x8004e034` — sucht nur das Inventar-Feld, wie zitiert.
* RE2 `info/re2leon/PSX.EXE`, `re2_disasm.py dis 0x800703b8 30` / `dis 0x800706a8 30` (FUN_800703b8, Kiste):
  `800703cc lbu v1,23572(v1)` (0x800d5c14 Scroll), `800703d8 addiu s4,s4,-15896` (0x800cc1e8),
  `80070400 lbu v0,-30636(at)` -> `80070408 sb v0,16(sp)`, `8007042c addiu v1,v1,2`, `80070440 andi s5,v1,0x3f`,
  `800706b8 lbu v0,-30592(at)`, `800706c8 sb v0,-30636(at)`, `80070700 lbu v0,16(sp)`, `80070710 sb v0,-30592(at)`.
  0x800cc1e8 + 0x10000 - 30636 = 0x800d4a3c (Inventar), -30592 = 0x800d4a68 (Box): TAUSCH, wie zitiert.
* RE2 Wegwerf-Ja, `dis 0x80051750 26`: Basis `8005172c addiu s0,s0,-15896` (0x800cc1e8), `80051774 sb zero,-30636(at)`,
  `80051794 sb zero,-30634(at)`, `80051798 jal 0x80069714`; das Box-Feld (-30592) wird in diesem Zweig nicht beschrieben.
* RE1.5 Kombinieren/Heilen: `kombi_zensus.py info/Re1.5/PSX.EXE` (selbst ausgefuehrt) -> `Zeile 0x31 @0x80074FF4: bytes
  01 00 00 00 88 4c 07 80 00 00 00 00 count=0`, `Paare mit 0x31 ...: KEINE`; `re15_disasm.py bytes 0x80074ff4 12` gleich.
  Heil-Gatter: `8004aa84 lbu v1,0(at)` (Inventar[Cursor], Basis 0x800b10ac), `8004aa94 beq ... 0x8004ab48` mit
  Verzoegerung `8004aa98 addiu v0,v1,-34`, `8004ab48 sltiu v0,v0,0xe`, `8004ab4c beq v0,zero,0x8004ab6c`,
  `8004ab6c ori v0,zero,0x6` (cant-use) — Id 0x31 (0x31-0x22 = 15 >= 14) geht nach "cant-use", wie zitiert.
* RDT-Bytes (shared_assets == `info/Re1.5/PSX/`, `cmp`): ROOM1090 @0x0271E `22 03 81 01`, @0x02722 `22 03 84 01`,
  @0x024CE `22 03 84 00`, @0x02306 `06 00 24 00 21 03 85 00`; ROOM1000 @0x00C24 `50 03 09 31 00 00 14 50 e0 fc 20 03
  20 03 31 00 01 00 86 00 ff 00`, @0x00D0C `22 03 85 01`; ROOM1050 @0x00B7A `3b 05 02 31 00 00 b4 46 a4 a2 ac 0d e8 03
  8c d8 f8 f8 68 10 00 0c 00 09 03 01` (Slot 5, Rechteck (18100,-23900) 3500x1000, Ziel (-10100,-1800,4200) rot 3072,
  Raum 0x09 Cut 3). Alles deckt sich mit den Zitaten.
* Census der Port-Wege, die 0x31 aus dem Inventar nehmen (selbst nachgezogen, `grep re15_inv_remove_slot|re15_inv_init|
  re15_itembox_init|re15_itembox_import`): Wegwerf-Ja `item_discard_common.c:353`; `menu_common.c:712` (Heilen, s.o.);
  `inventar_r36.c:78/114` nur `re15_inventar_r36_leer_weg_id` = Karte 0x21 + Granaten mit Menge 0; `rolltor_1050.c:204`
  nur im eigenen Programm `k_mit`; `messer_rueckfall.c` nur Messer 0x01; `scd_vm.c:536` / `main.c:4419` Neues Spiel
  (Flags werden mitgenullt); `main.c:5452/5664` nur hinter `RE15_INV_MIX_SHOT` / `RE15_ITEM_USE_TEST`; Laden
  `re15_savedata.c:294` (Kiste reist mit). Einzige Wegwerf-Stelle fuer 0x31: `gen/discard_sites.inc:44`
  `{ 0x1090, 9, 0x31, 0, 0 }`. => "(3,0x81) und 0x31 weder im Inventar noch in der Kiste" <=> Wegwerf-Ja. Die
  Herleitung des Bau-Agenten haelt.
* Diff-Suche (`git diff master...HEAD -- re15_port`, nur neue Zeilen) nach deferred/tunable/interim/for now/faithful/
  plausib/TODO/FIXME/getenv: **keine Treffer**. Nachbesserung 1 fuehrt keine neue Verhaltenskonstante ein; die
  Box-Platz-Formel des Kartenwerkzeugs `(k + 2) & (RE15_BOX_SLOTS - 1)` traegt @0x8007042c / @0x80070440 (geprueft).
  Ohne Original-Beleg bleiben nur die als PORT-WAHL / NUTZER-VORGABE gekennzeichneten Werte (Lagen `k_lagen`, Masse,
  Texte msg 29 / CHECK-Text / Spindtext, Name "Empty Fire Extinguisher").
* Erklaert der Fix den Befund? Ja: der Befund M1 war "Tor offen, weil nur `g_inv` gefragt"; mit der Kisten-Pruefung
  meldet dieselbe Eingabe (a1 = L8) die neue Zeile `kein Loescher (Leon hat ihn in die Item-Box gelegt ...)`, und die
  Mutationsprobe zeigt, dass genau diese Zeile des Tors den Unterschied macht.

### 2.3 Vertrag / Pfade
* `git diff master...HEAD --name-only` (ausser analysis/): `engine/src/feuerloescher_r37.c`, `engine/src/gen/
  feuerloescher_r37.inc`, `include/re15_feuerloescher_r37.h`, Haken in `item_discard_common.c` (+2),
  `item_modal_common.c` (+2), `item_prompt_common.c` (+2), `re15_inv_screen.c` (+4), `scd_room_setup.c` (+2),
  `platform/pc/main.c` (+8), Tests `tests/unit/test_r37_feuerloescher.c`, `tests/unit/probe_r37_feuerloescher_karte.c`,
  `tests/unit/probes/r37_feuerloescher.cmake`, `tests/integration/test_r37_feuerloescher.cmake`
  (`cmake_minimum_required(VERSION 3.16)` vorhanden, kein popen in den C-Tests), Werkzeuge `tools/r37_feuerloescher/*`.
  **Keine** release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists, **keine Panel-Datei**
  (panel_lampen_pc.c / Code-Panel-Module unberuehrt — Nachtrag 3b ist Spur E).
* Nachbesserung 1 (`git diff 09094721..HEAD --stat`): nur `feuerloescher_r37.c` (+31/-8), `re15_feuerloescher_r37.h` (+5/-3,
  Kommentar), Test/Probe/Integration der Spur, `tools/r37_feuerloescher/kombi_zensus.py`, Dossier, 3 Belegbilder —
  keine gemeinsame Datei beruehrt.
* Ressourcen: keine neuen Bank-9-Bits (nur Original-Bits (9,0x86), (3,0x85), (3,0x81); `test_r36_kartenbits.c`
  unberuehrt), Nachricht 29 (VERTRAG 1.2: 29..31) in 1090/1091, Prop obj 5 (in 1050 nur Dokument obj 2, in 1090/1091
  Port-Prop obj 4; `grep` der Port-Prop-Ids: kein weiterer Nutzer in diesen Raeumen), Slots 13 (1050), 6 (1090),
  3 (1091), 3 (1000, Umwidmung nach dem Init-Lauf). Installer direkt nach `re15_inhalt_r35_install` (begruendet,
  Riegel unit_r34n_e_dokumente) — Merge-Stelle fuer die Integration.

### 2.4 Nebenwirkungen
Elza 1051 ohne Wand-Loescher (d1), Elza 1091 liegt immer (c2/d1), Ada-Szene 1050 laeuft (b3), Rettungsszene laeuft nach
Ja, nach Nein und mit dem Loescher in der Kiste (b1/b2/g1), Herausnehmen aus der Kiste gibt ihn ins Inventar zurueck
(a5), Spind 1000 per Tuer-Weg umgewidmet (e3). In jedem anderen Raum tut der Installer nichts (`index_des_raums` -1,
Prop-Lader `obj_id` -1).

### 2.5 Belegbilder des Bau-Agenten
`I_belege/nb1_vorher_box_F130.png` und `nb1_nachher_1150_echte_box_F210.png` sind BYTEGLEICH (sha256 `c0914782...`).
Kein Kopierfehler: der Box-Schirm ist raumunabhaengig und deterministisch — meine eigenen Bilder a1v F130 (Box-Schirm
ROOM1050) und a4 F210 (echte Box ROOM1150) sind ebenfalls bytegleich (sha256 `9644f62d...`, Pixel-Differenz leer).
Den Unterschied vorher/nachher zeigt nur das Log bzw. das Bodenbild `nb1_nachher_1090_cut6_kiste_vs_ja.png` (deckt sich
mit a6/a7).

### 2.6 Hinweise fuer die Integration (kein Mangel)
* Merge-Stellen: `scd_room_setup.c` / `platform/pc/main.c` je eine Zeile direkt NACH `re15_inhalt_r35_install` (vor
  `re15_dokumente_install`, Grund Riegel unit_r34n_e_dokumente); `item_modal_common.c`, `item_discard_common.c`,
  `item_prompt_common.c`, `re15_inv_screen.c` je Include + Zeile direkt neben den Runde-36-Spur-Q-Haken.
* `engine/src/feuerloescher_r37.c` ist eine GLOB-Quelle -> Android `app/.cxx` neu konfigurieren.
* Spur D (itemband) fuehrt ein Band-Gate fuer Item-AOTs ein: die neue Aufnahme-Zone ROOM1050 Slot 13 traegt Band 0 wie
  die Notausgangs-Tuer daneben (@0x00B7A pc[4] = 0x00) — nach dem Merge `unit_r37_feuerloescher_aufnahme` und
  `integration_r37_feuerloescher` (Lauf A) mitfahren.

## 3. Maengel

**Keine.** M1 (Abnahme 0) und M2 sind an der exe bzw. im Code behoben (1.1, 1.7); alle Nutzer-Punkte erneut am gebauten
Stand gemessen erfuellt (1.2-1.6).

Beobachtungen ohne Mangel (nicht von Spur I verursacht oder Nutzer-/Orchestrator-Entscheidung):
1. **Loeschen mit dem Loescher in der Kiste** (1.8, Lauf g1): Leon loescht auch, wenn der Loescher vor dem Loeschen in
   der Item-Box liegt; es kommt keine Wegwerf-Abfrage, am Boden liegt keiner, der Loescher bleibt in der Kiste als
   "Empty Fire Extinguisher". Ursache ROOM1090 sub00 @0x02306 `06 00 24 00 21 03 85 00` (nur (3,0x85)); identisch auf
   v0.8.24 (Lauf g1m). Ob das Loeschen den Loescher im Inventar verlangen soll, entscheidet Orchestrator/Nutzer.
2. **Box-Zeile:** "Empty Fire Extinguisher" laeuft in das Item-Symbol (a1v F130) — wie der laengste Original-Name
   "Minidisc Player w/ Disc" (Abnahme 0, l8m); vorhandenes Box-Layout.
3. **`unit_r35_android_bash_mutanten`** rot in der Gruppe durch ein Last-Zeitlimit eines Mutanten (2.1); Spur I
   beruehrt keine Eingabe dieses Tests; einzeln nachgefahren gruen (945.74 s).
4. Die Belegbilder `nb1_vorher_box_F130.png` / `nb1_nachher_1150_echte_box_F210.png` sind bytegleich — erklaert durch den
   deterministischen Box-Schirm (2.5), kein Kopierfehler.
5. Offen fuer den Nutzer (Dossier Abschnitt 7, unveraendert): Fundort (ROOM1050 rechts der Notausgangs-Tuer), Texte
   msg 29 / CHECK-Text, Groesse 0,47 m, Spindtext "Nothing unusual.".

## 4. Urteil
| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Modell des Feuerloeschers | erfuellt | Generator unabhaengig bytegleich nachgefahren; gezeichnet 1050 Wand (c1), 1090 (a7, b1 F780, f3), 1091 (c2) |
| 2 Elza 1091 leerer Loescher am Boden | erfuellt | d1 Tuer-Weg 1051->1091, c2 Bild + msg 29 |
| 3 Leon 1090 erst nach der Nutzung + Nachtrag 4b (nur nach Ja) | erfuellt | e1 vorher nichts, b1 Ja -> liegt, b2 Nein -> nichts, b3/a7 Tuer/Spielstand nach Ja -> liegt, a1/a3/a4/a6 Nein+Kiste -> nichts, a5 Herausnehmen -> nichts |
| 4 Spind ROOM1000 -> anderer Ort | erfuellt | e1 Aufnahme an der Wand ROOM1050 + Tuer, e3 Tuer-Weg 1000, e2 "Nothing unusual." |
| Nachtrag 4c "Empty Fire Extinguisher" | erfuellt | b2 echter Nein-Weg (Name + CHECK), f1/f2 nach Laden / vor dem Loeschen |
| M1 (Abnahme 0) Item-Box-Weg | behoben | a1 = L8 jetzt `kein Loescher (... Kiste ...)`; Mutationsprobe 3 Fehler ohne Kisten-Pruefung |
| M2 (Abnahme 0) Abschnittsverweis | behoben | nur noch "Abschnitt 3.4 + Nachbesserung 1" |
| RE-Gate | erfuellt | FUN_8004dfec, FUN_800703b8, Wegwerf-Ja @0x80051774, Kombi-Zeile @0x80074FF4, Heil-Gatter @0x8004ab48, RDT-Bytes selbst geprueft |
| Vertrag/Pfade | erfuellt | 2.3, keine Panel-Datei, keine verbotenen Pfade, keine neuen Bits |
| Tests | erfuellt | 290er-Gruppe: 289 gruen + `unit_r35_android_bash_mutanten` einzeln gruen (Gruppen-Rot = Last-Zeitlimit, Spur I beruehrt keine Eingabe); alle 10 Spur-Riegel gruen; Mutationsprobe: `kiste` erkennt M1 (3 Fehler) |

**bestanden = true** — alle Nutzer-Punkte erfuellt, M1/M2 behoben, alle Gates halten.
