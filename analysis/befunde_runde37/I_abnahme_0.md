# Runde 37 — Spur I feuerloescher — UNABHAENGIGE ABNAHME 0 (2026-10-12)

Baum `.claude/worktrees/r37_feuerloescher` (Zweig `r37/feuerloescher`, Basis master 64ba31e0, Kopf 52062d05).
Massgeblich: `AUFTRAG.md` Nachtrag 4 (+ 4b, 4c), `VERTRAG.md`. Geprueft am GEBAUTEN Stand dieses Baums
(`local_build.sh configure` + `build`: `LOCAL-BUILD-OK`, `ninja: no work to do`; exe 13:14:43 juenger als die
letzte Code-Aenderung `feuerloescher_r37.c` 13:13:32; alle `[feuerloescher]`-Logzeilen in der exe nachgewiesen).

**Ergebnis: NICHT BESTANDEN** — ein gemessener Mangel gegen den Wortlaut von Nachtrag 4b (M1: der leere Loescher
liegt in ROOM1090 auch OHNE "Ja", sobald Leon ihn in die Item-Box legt, und ist dann doppelt da). Alles andere ist
an der exe gemessen erfuellt.

Hinweis zur Ausloesung: Der Auftrag, der diesen Workflow angestossen hat (Nachtrag 3b, Leuchtfarbe der Code-Panels),
gehoert zur Spur E codepanel. Spur I hat keine Panel-Datei angefasst (`git diff master...HEAD --name-only`: keine
`panel_lampen_pc.c`, keine Code-Panel-Datei) — fuer 3b ist diese Abnahme nicht zustaendig.

## 0. Werkzeug der Abnahme
* Messkopie der exe `re15_port/build/platform/pc/re15_pc_r37i_abn_mess.exe` (eigener Name, nie `taskkill /IM`;
  nach der Abnahme geloescht), Spielstand mit dem Kartenwerkzeug `probe_r37_feuerloescher_karte` (gelesen: schreibt
  nur Lage/Flags/Inventar ueber `re15_savedata_capture`), Start `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
  RE15_CARD_SLOT=0`, Eingabe `RE15_INPUT_SCRIPT` (Basis Spielbild, Start 60), Logs `debug.log`, `RE15_STATE_LOG`,
  `RE15_DISCARD_LOG`, `RE15_FLAG_TRACE`, Bilder `RE15_FRAMEDUMP` mit `RE15_WINDOW_SCALE=1`. Lauf-Skript `lauf.sh`
  + Rohdaten im Scratchpad der Abnahme (`iabn/<lauf>/`); Belegbilder (2x vergroessert) im Baum unter
  `re15_port/build/abnahme_i0/` (git-ignoriert, NICHT committet).
* ⛔ Messfalle (gemessen, Lauf L1): beim TUER-Weg setzt `g_engine.frame_count` im Zielraum auf 0 — ein Skript mit
  `RE15_INPUT_SCRIPT_BASIS=spiel` laeuft im neuen Raum VON VORN. Die Selbst-Tuer in ROOM1090 (Aot_on 3 @0x02726)
  setzt den Zaehler NICHT zurueck (Lauf L2: `[scd F403] Cut_chg(5)` nach dem Neuladen). Raumgenaues Ende daher mit
  `RE15_EXIT_AT=<bild>#<raum>`.

## 1. Messprotokoll je Nutzer-Punkt

### Punkt 4 — "den FEuerloescher im Spind bei Leon ... woanders platzieren" -> ERFUELLT
**L1 (echter Weg: Aufnahme an der Wand, Tuer, ROOM1090):**
`lauf.sh l1 760 "W1,A0.1,W3.5,A0.1,W3,L0.3667,W0.3,U1.2,W0.3,R0.3667,W0.3,A0.5,W10" 1050 17200 -22500 1024 cut:6`
(Leon, Spielbeginn-Flags, Einsatzausruestung, vor der Wand rechts der Notausgangs-Tuer, Blick Norden):
```
[save] CONTINUE: resumed in room 1050 (hp=100)
[feuerloescher] ROOM1050: Loescher an der Wand obj 5 bei (17200,-1150,-23775) rot_y 0, Zone Slot 13 Aufnahme Item 0x31 Bit (9,0x86)
[prop-render] pi=2 oid=0x05 pos=(17200,-1150,-23775) rot=(0,0,0) meshes=1
[feuerloescher] Aufnahme Item 0x31 (9,0x86) -> Set(3,0x85)=1 wie ROOM1000 sub01 @0x00D0C
[tuer] Sequenz Archiv 2 DOOR1D Port-Archiv P1DK Variante 2 Bit7 0 Tuer 29 Seite S022 T014 Spender 07 (17 Skripte)
[room] PC loaded room1090.rdt (336440 bytes)
[feuerloescher] ROOM1090: kein Loescher (Feuer brennt noch, (3,0x81)=0)
```
State-Log: Aufnahme-Druck F90, Ja F198, dann 36 Bilder nach Osten bis (19864,-22392), nach Norden gedreht, Aktion
F376 an Slot 5 -> Tuersequenz -> ROOM1090 (-10100,-1800,4268). Bilder `m1_1050_aufnahme_modal_F150.png` ("Will you
take the ..." mit ITPS-Bild), `m1_1050_wand_leer_nach_aufnahme_F420.png` (Wand danach leer).
**WA (Leon laeuft bis an die Wand):** `lauf.sh m2 260 "W1,U1.5,W0.5,A0.1,W3.5,A0.1,W2" 1050 17200 -21000 1024 cut:6`
-> Leon steht an der Kollisionskante (17200,-22909), die Aufnahme greift dort trotzdem (`Aufnahme Item 0x31 ...
Set(3,0x85)=1`).
**E2 (Bild):** `lauf.sh e2 130 "W2" 1050 15500 -21000 0 cut:6` -> `e2_1050_cut6_wand_loescher_F120.png`,
`e2_1050_wand_zoom.png`: roter Loescher mit Etikett und Hebel rechts der EXIT-Doppeltuer, ca. 14 Bildpunkte hoch
(320x240).
**L7a (Tuer-Weg in die Umkleide):** `lauf.sh l7a "120#1000" "W1,A0.5,W6" 1050 14700 -13500 2048 cut:4` ->
`[tuer] Sequenz Archiv 2 DOOR07 ...`, `[room] PC loaded room1000.rdt`,
`[feuerloescher] ROOM1000: Spind Slot 3 (Item 0x31 @0x00C24) -> Raumtext msg 0`.
**L7b (Spind untersuchen):** `lauf.sh l7b 260 "W1,A0.1,W4" 1000 20900 -1100 3072` ->
`[msgtext] room=1000 id=0 "Nothing unusual."`, keine Aufnahme; Bild `l7_1000_spind_nothing_unusual_F140.png`.
Story-Folge: (3,0x85) wird bei der Aufnahme gesetzt (Log oben); ROOM1090 legt damit das Loesch-Ereignis an (L2:
msg 7/8 -> Ja -> Loeschen). Der Fundort ist PORT-WAHL (Dossier Abschnitt 7, Nutzer entscheidet).

### Punkt 1 — "ein Modell des Feuerloeschers" -> ERFUELLT
* Gebaut (PORT-WAHL, weder RE1.5 noch RE2 haben eins). Generator unabhaengig nachgefahren: Kopie von
  `tools/r37_feuerloescher/*.py` + `tools/r36_p/texte_bauen.py` + `ITPS.ITP` + `ROOM4050.RDT` + `font_width.h` in einen
  leeren Scratchpad-Baum, `python3 feuerloescher_modell.py` -> `Laenge/Durchmesser 2.346`, `R = 110`,
  `Hoehe 792 (0.47 m)`, `Weiss vorher 730, Referenz ROOM4050 656 -> Faktor 0.899`,
  `MD1 stehend 7940 B, liegend 7940 B, TIM 16928 B`; **`cmp` gegen `engine/src/gen/feuerloescher_r37.inc`:
  BYTEGLEICH**. Quellen geprueft: ITPS.ITP Block 0x31 @0x093000 beginnt mit TIM-Kopf `10 00 00 00 09 00 00 00`,
  ROOM4050.RDT @0x2DDA8 ebenso (`10 00 00 00 09 00 00 00 0c 02 ...`).
* An der exe gezeichnet: Wand ROOM1050 Cut 6 (E2), liegend ROOM1090 Cut 5/6/7 (F2 `f2_1090_cut5_zoom.png`, F3
  `f3_1090_leon_cut6_msg29_nach_ja.png`, L2 `l2_1090_cut7_liegt_zoom.png`), ROOM1091 Cut 6 (L6b
  `l6_1091_elza_cut6_liegt_F80.png`). Textur (Rot, Etikett, Hebel) korrekt, kein Texturfehler sichtbar.

### Punkt 2 — "bei elza einen Leeren Feuerloescher am Boden" -> ERFUELLT
**L6a (Tuer-Weg 1051 -> 1091):** `lauf.sh l6a "150#1091" "W1,A0.5,W6" 1051 19850 -22300 1024 cut:6 elza` ->
```
[save] CONTINUE: resumed in room 1051 (hp=100)
[room] PC loaded room1091.rdt (282404 bytes)
[feuerloescher] ROOM1091: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250, Zone Slot 3 Untersuchen msg 29
```
(keine `[feuerloescher] ROOM1051`-Zeile: Elza bekommt keinen Wand-Loescher.) **L6b (Untersuchen):**
`lauf.sh l6b 300 "W1.5,A0.1,W5" 1091 1880 -1300 0 y:-1800 cut:6 elza flag:7:0xf8 flag:7:0xf9 flag:7:0xfa flag:7:0xfb`
-> `[prop-render] pi=4 oid=0x05 pos=(2500,-1800,-1300)`, `[msgtext] room=1091 id=29 "It's an empty fire extinguisher."`;
Bilder `l6_1091_elza_cut6_liegt_F80.png` (Loescher vor der offenen Wagentuer), `l6_1091_elza_msg29_F140.png`.

### Punkt 3 + Nachtrag 4b — "bei Leon nach der Nutzung erstmalig auftauchen" / "it can only lay there - if leon when he is asked to discard it select 'yes'" -> TEILWEISE (Mangel M1)
**L2 (Loeschen + JA):** Spielstand am Feuer mit Loescher, Hilferuf vorbei (ohne (3,0x80) laeuft erst die Rufszene
msg 0..2 und Leon steht danach ausserhalb des Loesch-Vierecks, gemessen im Vorlauf):
`lauf.sh l2 1500 "W1" + 40x",A0.1,W1.9" 1090 0 250 0 y:-1800 cut:13 flag:9:0x86 flag:3:0x85 flag:3:0x80 give:0x31:1`
(+ `RE15_FLAG_TRACE=1 RE15_DISCARD_LOG=discard.log`):
```
[feuerloescher] ROOM1090: kein Loescher (Feuer brennt noch, (3,0x81)=0)
[msgtext] room=1090 id=7 "I must hurry up and get something to put out this fire to save that woman!"
[msgtext] room=1090 id=8 "Will you use the Fire Extinguisher?"
[feuerloescher] ROOM1090: kein Loescher (Leon traegt ihn noch (Item 0x31 im Inventar))
[msgtext] room=1090 id=9 "You've used the Fire Extinguisher."
[feuerloescher] Loescher weggeworfen -> liegt in ROOM1090
[feuerloescher] ROOM1090: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250, Zone Slot 6 Untersuchen msg 29
[prop-render] pi=4 oid=0x05 pos=(2500,-1800,-1300) rot=(0,3250,0) meshes=1
[msgtext] room=1090 id=3 "Leon: There! Are you okay?"   ... bis id=6
```
discard.log: Abfrage voll getippt ab F600 (`frage=8 gegenstand=0x31 wahl=0 text=45/45`), Ja F630. Bilder
`l2_1090_wegwerfabfrage_F600.png`, `l2_1090_cut7_liegt_nach_ja_F780.png` / `l2_1090_cut7_liegt_zoom.png` (liegt vor
dem Reifen; waehrend der Szene vom unteren Letterbox-Balken angeschnitten).
**L3 (Wiederbetreten ueber die TUER nach Ja):** Spielstand ROOM1050 vor der Notausgangs-Tuer mit dem Zustand nach L2
(9,0x86)(3,0x85)(3,0x80)(3,0x81)(3,0xBB)(3,0x6E), kein Loescher im Inventar:
`lauf.sh l3 "150#1090" "W1,A0.5,W2,L0.5667,W0.3,U0.1,W0.3,A0.5,W6" 1050 19850 -22300 1024 cut:6 <Flags>` ->
`[msgtext] room=1050 id=8 "Leon: Hey, wait!"` (Ada-Szene 1050 laeuft), Tuer, `[room] PC loaded room1090.rdt`,
`[feuerloescher] ROOM1090: Loescher liegt leer am Boden obj 5 ... Zone Slot 6 Untersuchen msg 29`.
**F2/F3 (Bild + Untersuchen nach Ja):** `lauf.sh f3 220 "W1,A0.1,W4" 1090 2500 -2400 3072 y:-1800 cut:6 <Flags>
flag:7:0xf8..0xfb` -> `[msgtext] room=1090 id=29 "It's an empty fire extinguisher."`, Bild
`f3_1090_leon_cut6_msg29_nach_ja.png`. (Ohne die vier Zombie-Status-Bits packt ein Nach-Rettungs-Zombie (sub00
@0x023A0..) Leon sofort — Spielstand-Artefakt der Messung, kein Spur-I-Thema.)
**L4 (Loeschen + NEIN):** wie L2, 10. Druck = X -> KEIN `weggeworfen`, KEIN `liegt leer am Boden`, Rettungsszene
laeuft (`id=3 ... id=6`); discard.log F630 `abfrage=0 ... gefragt=1 weg=0`.
**L5 (Wiederbetreten ueber die TUER nach Nein):** wie L3 + `give:0x31:1` ->
`[feuerloescher] ROOM1090: kein Loescher (Leon traegt ihn noch (Item 0x31 im Inventar))`.
**L8 / L8v (NEIN, dann Item-Box, dann Tuer) — MANGEL M1:**
`lauf.sh l8 "200#1090" "W1,A0.1,W0.7,A0.1,W1,X0.1,W0.7,X0.1,W2,A0.5,W6" 1050 19850 -22300 1024 cut:6 flag:9:0x86 flag:3:0x85 flag:3:0x80 flag:3:0x81 leer give:0x31:1`
mit `RE15_BOX_TEST=1` (oeffnet denselben Box-Schirm wie die Box-Zone ROOM1150 msg 3, `re15_itembox.c`):
```
[save] CONTINUE: resumed in room 1050 (hp=100)
[feuerloescher] ROOM1050: kein Loescher (schon genommen, (9,0x86)=1)
[feuerloescher] Name Item 0x31: Empty Fire Extinguisher
[tuer] Sequenz Archiv 2 DOOR1D ...
[room] PC loaded room1090.rdt (336440 bytes)
[feuerloescher] ROOM1090: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250, Zone Slot 6 Untersuchen msg 29
```
L8v (dieselbe Eingabe, Ende vor der Tuer `230#1050`): `l8_box_vorher_inventar_F80.png` (Loescher im Inventar),
`l8_box_nachher_loescher_in_box_F130.png` ("Empty Fire Extinguisher" liegt in der ITEM BOX, Inventar leer).
**L8r (dieselbe Einlagerung an der ECHTEN Box):** `lauf.sh l8r 330 "W1,A0.1,W3,A0.1,W0.7,A0.1,W2" 1150 -23000 -12100 2048
flag:9:0x86 flag:3:0x85 flag:3:0x80 flag:3:0x81 leer give:0x31:1` -> `[msg] room=1150 id=3 ITEMBOX (box screen, message
suppressed)`, `[feuerloescher] Name Item 0x31: Empty Fire Extinguisher`; Bild `l8r_1150_echte_box_loescher_eingelagert.png`
("Empty Fire Extinguisher" in der ITEM BOX, Inventar leer) — die Box in Leons ROOM1150 nimmt den Loescher an,
derselbe Zustand wie in L8.
=> Der Loescher liegt am Boden von ROOM1090 UND in der Item-Box, obwohl Leon an der Abfrage NEIN gesagt hat.
Ursache im Code: `feuerloescher_r37.c:83-84` `tor_offen` = `(3,0x81) && re15_inv_find_item(0x31) < 0`;
`re15_inv_find_item` (`inventory_common.c:147-152`) sucht nur `g_inv.slots[0..RE15_INV_MAX_SLOTS)`, die Box
(`g_itembox.slots[64]`, `re15_itembox.h`) nicht. "Nicht im Inventar" ist also kein Beleg fuer "Ja gesagt".

### Nachtrag 4c — NEIN -> "Empty Fire Extinguisher" im Inventar -> ERFUELLT
**L4b/L4c (echter NEIN-Weg, dann Statusschirm -> ITEM -> CHECK, nur ueber Eingaben):** wie L4 mit
`leer give:0x31:1`, danach `S0.1,W1.5,A0.1,W1.5,A0.1,W1.5,L0.1,W1,A0.1`:
`[feuerloescher] Name Item 0x31: Empty Fire Extinguisher`, `[inv] CHECK-Foto Item 0x31 ...`,
`[feuerloescher] CHECK-Text Item 0x31: An empty fire extinguisher.`; Bilder `l4_status_empty_fire_extinguisher.png`
(Name unter der Liste), `l4_check_an_empty_fire_extinguisher.png` (ITPS-Foto + "An empty fire extinguisher.").
Vor dem Loeschen bleibt der Originalname (L2/L4: "Will you use the Fire Extinguisher?").
**L9 (Spielstand laden):** CONTINUE ROOM1090 mit (3,0x81) + Loescher -> `Name Item 0x31: Empty Fire Extinguisher`,
Bild `l9_status_nach_laden_empty.png`.
Glyphen geprueft: DEBUG.BIN @0x4BA7 `21 49 4c 50 55 00 23 ...` = "Empty Grenade Shells", @0x4CDC
`22 45 4e 41 00 21 54 50 45 4a 43 51 45 4f 44 41 4e 07` = "Fire Extinguisher" — `k_name_leer` ist genau
"Empty " + Name 0x31 + 0x07.
Beobachtung ohne Mangel: in der Item-Box-Zeile laeuft der Name in das Symbol (`l8_box_nachher_...`); der laengste
Originalname "Minidisc Player w/ Disc" (Item 0x44) tut dasselbe (`l8m_box_vergleich_minidisc_F130.png`) — vorhandenes
Box-Layout, nicht von Spur I verursacht.

## 2. Gates

### 2.1 Bau und Tests
* `local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `local_build.sh build` -> `LOCAL-BUILD-OK (build)`.
* Gezielt (`ctest -R`, `-j 3`, `--timeout 600`, unter Last paralleler Agenten), Ausdruck woertlich:
  `^(unit_r37_feuerloescher|integration_r37_feuerloescher|unit_aot_edge|unit_scd_aot_opcodes|unit_savedata|unit_item_modal|unit_item_stack_press|unit_aot_sce_census|unit_sicherung_1150|unit_re2_room1090_ab|unit_1090_|probe_1090_|unit_plc_back_yaw_1090|unit_map_1090|unit_ada_eintritt_1050|unit_r19_marke3_1050_c1_tiefe|unit_r21_discard_wegwerfen|unit_r27_ada_cutscene_1050|unit_r30_|unit_r34n_|unit_r35_|unit_room1050_sicherung|unit_r36_|unit_inv_fsm|unit_item_icon|unit_inv_grant|unit_room1150_itembox|unit_itembox_transfer|unit_itembox_screen|unit_debug_menu|unit_pri_inventar_riegel|unit_r26_inventar|unit_msg_dialog|unit_action_msg_gate|unit_text_freeze|integration_item_name_census|integration_flag_census|integration_r36_inventar|integration_r36_schluessel|integration_r35_inventar1050|integration_r36_story_leon|integration_r36_story_elza|integration_r34n_e_dokumente_bild|integration_r36_kartenitems_bild)`
  -> **`100% tests passed, 0 tests failed out of 289`** (1054 s), darin alle 8 `unit_r37_feuerloescher_*` und
  `integration_r37_feuerloescher` (289 s). Breiter als der Lauf des Bau-Agenten (alle `unit_r34n_`/`unit_r35_`/
  `unit_r36_`, dazu `integration_flag_census`, `integration_r36_kartenitems_bild`). Keine Regression -> keine
  Gegenprobe mit v0.8.21 noetig.
* Luecke: kein Riegel fuer den Box-Weg (M1).

### 2.2 RE-Gate (Stichproben SELBST disassembliert, richtige Binaerdatei)
`re15_disasm.py dis` gegen `info/Re1.5/PSX.EXE`:
* Item_aot_set: `80040664 andi v0,v0,0x80`, `80040680 lhu a1,18(a2)`, `80040684 lbu s1,20(a2)`,
  `80040688 addiu v0,a2,22`, `800406d8 lw a0,18056(a0)` (=0x80074688), `800406dc jal 0x8004efe4`,
  `800406e4 beq v0,zero,0x8004071c`, `800406f4 sb zero,0(v0)`, Pool `0x800b3f98` + tk_prop*148,
  `80040718 sw v1,0(at)` mit v1 = 0x80000000 — wie im Dossier 2.1.
* FUN_8002c18c: `8002c2ec lw v0,12(s1)`, `8002c2f4 andi v0,v0,0x10`, `8002c2f8 beq v0,zero,0x8002c360`,
  `8002c320 jal 0x80022f0c` — Licht-Gate wie zitiert (Attribut 0 = NCCT-Zweig).
* Obj_model_set: `80040990 lhu v0,6(a2)`, `80040998 ori v0,v0,0x1`, `800409a8 lh v0,8(a2)`, `800409b0 sw v0,12(a1)`.
* FUN_80042bac: `80042bd0 ori v0,zero,0x26c` (620 voraus).
RDT-/Datei-Bytes direkt gelesen: ROOM1090 @0x0271E `22 03 81 01`, @0x02722 `22 03 84 01`, @0x024CE `22 03 84 00`
(`scd_dump_room.py`: `=== sub03 [file 0x24CE..]` = erster Befehl der Rettungsszene), @0x02306 `06 00 24 00 21 03 85 00`,
@0x02336 Slot 2 sce 3, @0x02554 Member_set (2602,-1800,-2233), Sce_em_set @0x02214 (2052,-1334) / @0x02228
(2375,-2333); ROOM1000 @0x00C24 `50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 31 00 01 00 86 00 ff 00`,
@0x00CFC..0x00D0C `... 21 09 86 01 ... 21 03 85 00 22 03 85 01`, @0x00CAA Slot 6 msg 0 Pause `ff ff`, @0x00D22 msg 0
"Nothing unusual."; ROOM1001 @0x00C24 Item 0x15 x30 Bit 0x86; ROOM1050 @0x00B7A Slot 5 Rechteck (18100,-23900)
3500x1000, Ziel (-10100,-1800,4200); ROOM1010 @0x00930 `2d 00 00 00 01 00 0a 00 00 00`. Alles deckt sich mit den Zitaten.
* Abweichung vom VERTRAG 1.2 ("Boden-Loescher bei Leon haengt an (3,0x84)") ist BELEGT begruendet: (3,0x84) loescht
  sub03 @0x024CE als ersten Befehl; dauerhaft ist (3,0x81) @0x0271E.
* Ohne Original-Beleg sind nur als PORT-WAHL / NUTZER-VORGABE gekennzeichnete Werte (Lagen `k_lagen`,
  Zonengroessen, Massstab, Texte msg 29 / CHECK-Text, Spindtext). Suche nach deferred/tunable/interim/for now/
  faithful/plausibel/TODO im Diff: keine Treffer in neuen Zeilen; kein `getenv` im Engine-/Plattform-Diff.

### 2.3 Vertrag / Pfade
* Geaenderte Pfade (`git diff master...HEAD --name-only`): `analysis/befunde_runde37/*`, neue Dateien
  `engine/src/feuerloescher_r37.c`, `include/re15_feuerloescher_r37.h`, `engine/src/gen/feuerloescher_r37.inc`,
  `tools/r37_feuerloescher/*`, `tests/unit/test_r37_feuerloescher.c`, `tests/unit/probe_r37_feuerloescher_karte.c`,
  `tests/unit/probes/r37_feuerloescher.cmake`, `tests/integration/test_r37_feuerloescher.cmake`
  (`cmake_minimum_required(VERSION 3.16)` vorhanden); Haken in `item_discard_common.c` (+2), `item_modal_common.c` (+2),
  `item_prompt_common.c` (+2), `re15_inv_screen.c` (+4), `scd_room_setup.c` (+2), `platform/pc/main.c` (+8, Muster der
  r35/r36-Lader). Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists.
* Ressourcen: Nachricht 29 (VERTRAG 1.2: 29..31) in 1090/1091 — kollisionsfrei (1091 Port-msg 20 der Runde 36;
  msg 29 der Runde 36 liegt in ROOM11B0); keine neuen Bank-9-Bits; Slots 13 (1050: Original belegt nur 0..10, Port 4
  und 15), 6 (1090), 3 (1091), 3 (1000, Umwidmung nach dem Init-Lauf); Prop obj 5 in 1050/1090/1091 frei (nOmodel
  2/4/4, Port-Props obj 2 bzw. obj 4). Installer bewusst VOR `re15_dokumente_install` statt am Blockende (Grund
  belegt: Riegel unit_r34n_e_dokumente); auf `r37/integration` stehen `re15_inhalt_r35_install` /
  `re15_dokumente_install` weiter direkt untereinander (Merge-Stelle bekannt).

### 2.4 Nebenwirkungen
Elza 1001 Spind unveraendert (Riegel R 1001), 1051 ohne Wand-Loescher (L6a), Ada-Szene 1050 laeuft (L3),
Rettungsszene laeuft nach Ja und nach Nein (L2/L4), Dokument 1050 / Schrot 1090 unberuehrt (Riegel gruen),
Aufnahme bei vollem Inventar: Haken sitzt hinter dem Voll-Zweig (`item_modal_common.c` Zustand 7 -> 8), setzt
(3,0x85) also nur bei erfolgreicher Aufnahme.

## 3. Maengel

**M1 (blockierend, Nachtrag 4b + Punkt 3):** Der leere Loescher erscheint in ROOM1090 auch OHNE "Ja": NEIN an der
Wegwerf-Abfrage, Loescher in die Item-Box legen, ROOM1090 betreten -> `[feuerloescher] ROOM1090: Loescher liegt leer
am Boden ...`, gleichzeitig "Empty Fire Extinguisher" in der Box (Laeufe L8/L8v/L8r, Bilder
`l8_box_vorher_inventar_F80.png`, `l8_box_nachher_loescher_in_box_F130.png`,
`l8r_1150_echte_box_loescher_eingelagert.png`). Widerspricht dem Wortlaut "it can only
lay there - if leon when he is asked to discard it select 'yes'" und der Lesart "Nein -> kein Prop ... auch nicht nach
Wiederbetreten"; es entsteht genau das Doppel, das `tor_offen` vermeiden soll. Ursache `feuerloescher_r37.c:83-84`
(nur `g_inv` gefragt). Die Item-Box in Leons ROOM1150 (msg 3) nimmt den Loescher an (L8r), ROOM1090 ist ueber die
Notausgangs-Tuer ROOM1050 Slot 5 erreichbar (L1/L3/L5/L8) — ein normaler Spielweg (nutzlose Gegenstaende landen in
der Box).
Abhilfe (Wahl des Bau-Agenten): das Tor zusaetzlich gegen `g_itembox.slots[]` pruefen (kein neues Bit; die Box ist
der einzige weitere Ort — Heil-Verbrauch `menu_common.c` nur Ids 0x22..0x2F, `inventar_r36.c` nur 0x21/Granaten,
`rolltor_1050.c` nur sein Teil), ODER das Ja selbst dauerhaft festhalten (Bit nur nach Zuteilung durch den
Orchestrator; VERTRAG 1.1 teilt 96/97 an Spur F). Dazu ein Riegel fuer den Box-Weg (unit: Loescher in `g_itembox`,
(3,0x81) gesetzt, Raumstart 1090 -> kein Prop; exe-Lauf wie L8 -> keine Zeile "liegt leer am Boden").

**M2 (klein, Doku):** `feuerloescher_r37.c:82` verweist fuer die Tor-Regel auf "Dossier Abschnitt 3.3" — dort steht der
Spind; die Regel steht in Abschnitt 3.4. Verweis berichtigen.

## 4. Urteil
| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Modell | erfuellt | Generator byte-gleich nachgefahren; gezeichnet 1050/1090/1091 (E2, F2, F3, L2, L6b) |
| 2 Elza 1091 liegt | erfuellt | L6a Tuer-Weg, L6b msg 29 + Bild |
| 3 Leon nach Nutzung (+4b nur nach Ja) | teilweise | Ja/Nein/Tuer-Weg korrekt (L2, L3, L4, L5); Box-Weg liegt ohne Ja (M1) |
| 4 Spind -> anderer Ort | erfuellt | L1 echter Weg Wand -> Tuer -> 1090, WA, L7a/L7b Spind "Nothing unusual." |
| 4c "Empty Fire Extinguisher" | erfuellt | L4b/L4c echter NEIN-Weg, L9 nach Laden |
| RE-Gate | erfuellt | 4 EXE-Stellen + RDT-/DEBUG.BIN-Bytes selbst geprueft |
| Vertrag/Pfade | erfuellt | Abschnitt 2.3 |
| Tests | teilweise | 289/289 gruen, aber kein Riegel fuer den Box-Weg |

**bestanden = false** (M1).
