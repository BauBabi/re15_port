# Runde 31 — Tueren: Pruefer 1 (Runde 1)

Zweig `r31/tueren`, Arbeitsbaum `.claude/worktrees/r31_tueren`, Stand HEAD `4a862e96` (Bauer-Commits
`3398f601`, `b2ef2c7c`, `7ce4233f`, `4a862e96` auf Basis `59298905`). Pruefer aendert keinen Spielcode.
Belege `analysis/befunde_runde31/tueren_belege/t4p1_*.jpg`, Rohbilder `build/r31_tueren/t4p1/` (nicht versioniert),
Messwerkzeuge `re15_port/tools/tueren/pruefer1_*` (reine Messung).

## Urteil: haltbar

Der Nutzer sieht beim Spielen an den 83 abgedeckten Tueren (184 Seiten, davon 4 in RE1.5 nie begehbar, s. B1)
die RE2-Sequenz ihres Archivs, mit dem Tonteil dieses Archivs, Griff auf der gemalten Seite; nicht abgedeckte
Tueren, Tor, Intro, Schloss, Laden, Elza-Raeume und Aufzugfahrt laufen wie vorher. Suite 409/409.
Vier Befunde, alle niedrig (Abschnitt "Befunde").

## 1. Selbst gebaut

`bash re15_port/tools/local_build.sh all` (Configure + Build + ctest im eigenen `re15_port/build`):
`=== LOCAL-BUILD-OK (all) — Tests 409/409`, `100% tests passed, 0 tests failed out of 409` (586,8 s).
Die vier Riegel des Bauers gruen: `unit_r31_viereck`, `unit_r31_maschine`, `unit_r31_zuordnung`, `unit_r31_tuer_ton`.

Mit dieser exe `RE15_TUER_SEITE=ALLE RE15_TUER_BOGEN=build/r31_tueren/t4p1/bilder RE15_TUER_SCHNELL=1
RE15_AUDIO_CAP_SYNC=... RE15_SE_DEBUG=1` neu erzeugt: 368 Bilder, **bitgleich** mit den Bauer-Bildern
`build/r31_tueren/t4/bilder` (PIL-Differenz, 0 von 368 abweichend) — der Bogen des Bauers zeigt also HEAD.
`[tuer-seite] 184 Seiten gespielt`, 184 x "Sequenz fertig ... Ton geladen", keine Notiz != 0.

## 2. Griff je Seite (alle 184 Seiten)

Der Bauer-Bogen (Zelle 240x180 fuer das ganze 320x240-Bild, Tuer ~70 px hoch) ist fuer die Griff**form** zu klein.
Eigener Bogen `re15_port/tools/tueren/pruefer1_griffbogen.py` -> `build/r31_tueren/t4p1/griff/g01..g31.png`
(Belege `t4p1_griff_01..31.jpg`): je Seite RE1.5-Ausschnitt (T1, groesster Umriss) + entzerrter Ausschnitt |
Sequenz Anfang | Sequenz Mitte, beide auf die Tuer beschnitten, 300 px hoch. **Alle 31 Boegen angesehen.**

Ergebnis: **keine Seite mit Griff auf der falschen Seite oder in falscher Formfamilie**, soweit der RE1.5-Ausschnitt
den Griff zeigt. Im Einzelnen gesehen:
- DOOR13 (12 Seiten) Knauf links V0 / rechts V1 wie gemalt; DOOR1A (18) Druecker auf Schild, Hebel zeigt zur
  Blattmitte wie gemalt; DOOR19 (12) Drehscheibe unten links V0 / rechts V1; DOOR1C (10) ovaler Griff;
  DOOR15 (8) Griffkasten im Warnstreifen links/rechts; DOOR23 (3) Hebel in der Profilbucht; DOOR24 (4) Druecker
  auf Blech rechts/links; DOOR06 (4) Riegelgriff; DOOR0A (3) Riegel am Querband; DOOR2E (4) Riegel + Lampe;
  DOOR29 (22) Bedientafel links V0 / rechts V1; DOOR26/31 (21) Rad-Lage und Ein-/Zweiteilung wie gemalt (V2/V3
  einteilig, S276/S277 relative Radlage 0,50/0,73 gegen RE1.5 0,48/0,58); DOOR1B/1D Doppeltuer mit Stangen bzw.
  Kasten mittig; DOOR2A Knauf rechts (S321, S323), zweifluegelig V2; DOOR25/27/16/2D/1E ohne Griff.
- Griff-Tausch: S058/S060/S061/S065 langer Messing-Stangengriff (DOOR04) auf der gemalten Seite; S042 Druecker
  (DOOR07) rechts — Form stimmt, Erscheinung s. Befund B2.
- Seiten ohne Griff im RE1.5-Bild (unsichtbar/flach/Kante: S094, S111, S115, S116, S161, S227, S242, S255,
  S291, S309, S314, S320 u. a.) folgen der Regel der Stufe 3 (Komplement/Gegenseite) — nicht nachpruefbar, nicht
  neu verhandelt.
- Schieberichtung (der Bogen zeigt sie NICHT: bei DOOR19/DOOR2A ist "Mitte" = "Anfang", weil der Bogenhaken nur
  Drehung bzw. Lage y/z misst): per `RE15_TUER_SERIE` nachgeholt, `t4p1_schiebe.jpg` + `t4p1_ablauf_s052.jpg`:
  S052 DOOR19 V0 schiebt nach rechts, S051 DOOR19 V1 nach links, S280 DOOR2A V0 nach rechts, S321 DOOR2A V1 nach
  links — jeweils vom Griff weg, wie T2 Anhang A1.
- DOOR2D-Richtung (S311 V4 hinauf / S288 V5 hinab) selbst gemessen, Serie je Bild, roter Schachtrahmen
  (R>120, G<60) Zeilenspanne: S311 Bild 120 (0,106) -> 125 (0,195) -> 130 (0,239) -> 135 (81,239) -> 140 (201,239)
  = kommt oben herein und laeuft nach unten = Kamera steigt; S288 Bild 85 (158,239) -> 90 (20,239) -> 95 (0,237)
  -> 100 (0,162) -> 105 (0,64) = von unten nach oben = Kamera sinkt. Bestaetigt die Belegung im Bau-Dossier 1.3.
  `t4p1_S311_reihe.jpg`, `t4p1_S288_reihe.jpg`.

## 3. Echtlaeufe (echte exe, beschleunigter Renderer, RE15_FRAMEDUMP + RE15_TUER_SERIE, Ton per RE15_AUDIO_CAP_SYNC + RE15_SE_DEBUG)

Anders als der Bauer (RE15_FIRE_AOT) durch den **echten Scan**: `pruefer1_lauf.sh` = Titel-Vorlauf,
`RE15_DEBUG_JUMP`, Standplatz `RE15_PLAYER_POS` so, dass der 620er-Vorwaertspunkt (FUN_80042bac @0x80042bd0)
auf der Tuermitte liegt (`pruefer1_standplatz.py`), dann QUADRAT per `RE15_PAD_AT` (Band-Gate, Viereck-Trefftest
und Zwischensequenz-Sperre laufen also mit). Fenster war 960x720 (Framedump 960x720, Boegen verkleinert).

| Lauf | Tuer / Seite | Archiv | Log | Bild |
|---|---|---|---|---|
| s043 | ROOM10D0 -> 10E0, S043 (Doppeltuer) | DOOR1B V3 | Slot 14 per Scan, Se_on Bild 90 (10400 Hz), Schliesston 11025 Hz, `room10e0.rdt`; danach UP 200..252 -> 29 Schrittgeraeusche, Spieler geht | alter Raum dunkelt ab, beide Fluegel schwingen weg, 10E0 blendet ein (F0 schwarz, F4 dunkel, F8 hell), Spieler vor der Rueckkehrtuer, laeuft los (`t4p1_ablauf_s043.jpg`) |
| s225 | ROOM4030 -> 4040, S225 **Viereck** | DOOR25 V0 | Slot 1 rect=(-25410,-25245,1220,1315) per Viereck-Trefftest, Se_on 110, Door_exit | Schacht dunkelt ab, Hubtuer faehrt hoch, 4040 blendet ein, Spieler auf dem Laufsteg an der Tuer (`t4p1_ablauf_s225.jpg`) |
| s290 | ROOM50C0 -> 4020 **Aufzugskabine** (Strom-Flag 3:78, `Ck(3,0x4e)` vor dem Satz ROOM50C0 @0x92E) | DOOR27 V0 | Se_on 70 (7041 Hz), Door_exit, `room4020.rdt` | SUB FLOOR 03 dunkelt ab, Schiebetuer, Kabine blendet ein (`t4p1_ablauf_s290.jpg`) |
| s290f | dasselbe + Fahrt (`RE15_FORCE_EVENT=8@200` = Panel-Evt_exec sub08) | - | Fahr-SE 17/18, Null-Rechteck-Slot 1 -> `room5000.rdt`, **kein** `[tuer]` | Panel "02", Fahrt, 5000 blendet mit Balken ein, Spieler vor dem Aufzug (`t4p1_ablauf_s290_fahrt.jpg`) |
| s052 | ROOM1110 Slot 1, S052 **Selbst-Tuer** | DOOR19 V0 | Se_on 60 (9226 Hz), 301 Bilder + 11 Warten | Tuer schiebt nach rechts, danach Cut 7, Spieler hinter der Tuer (`t4p1_ablauf_s052.jpg`, `_nach.jpg`) |
| s159 | ROOM2070 -> 11A0, S159 **einseitig** (Flag 3:151) | DOOR0A V1 | Se_on 100 (11025 Hz), Door_exit | Gittertuer schwingt, 11A0 blendet ein, Spieler neben der anders gemalten Gegenseite (`t4p1_ablauf_s159.jpg`) |
| s159_auf | dasselbe ohne Flag: QUADRAT F150, Meldung wegdruecken, QUADRAT F330 | DOOR0A V1 | `[msg] room=2070 id=0` "You've opened the lock.", dann Slot 4 -> Sequenz -> 11A0 | Aufschliessen ohne Sequenz, danach Tuer mit Sequenz |
| laden | Spielstand ROOM1150 (`probe_r30_granate_karte`), `RE15_CONTINUE_TEST`, S065 | DOOR09 V0 + Griff DOOR04 | `Griff-Tausch ... Spender-Griff geladen`, Se_on 70, `room1130.rdt` | Stangengriff links, 1130 blendet ein (`t4p1_ablauf_laden.jpg`) |
| elza_s016 | Elza (PL04, Elza-Bit 1), ROOM**1041** Slot 3 -> **1021** | DOOR13 V1 | Schluessel Raum 0x1041 + Rechteck greift | Elza im Flur, Tuer, 1021 blendet ein (`t4p1_ablauf_elza_s016.jpg`) |
| tor2 | ROOM1170 Tor Slot 0 nach dem Intro | Archiv 1 (Tor) | `Sequenz Archiv 1 DOOR2E`, Se_on 100, Szenario-Wiedereintritt | Torsequenz wie v0.8.16 (`t4p1_ablauf_tor2.jpg`) |
| s226 | ROOM4030 Viereck S226 (nicht abgedeckt, "keine Tuer") | - | kein `[tuer]`, `room4080.rdt` | RE1.5-Blende (`ablauf_s226`, Rohbilder) |
| s001 | ROOM1000 -> 1050 S001 (nicht abgedeckt) | - | kein `[tuer]`, `room1050.rdt` | RE1.5-Blende (`t4p1_ablauf_s001.jpg`) |

- **Se_on-Bilder** aller Laeufe = Tabelle T2 Abschnitt 4 (1B 90, 25 110, 27 70, 19 60, 0A 100, 09 70, 13 60, Tor 100).
- **Intro ROOM1240 -> 1170**: in s043 und tor2 feuern `slot=0 rect=(0,0,0,0)` und in 1170 `slot=3 rect=(0,0,0,0)`
  ohne `[tuer]` — keine Sequenz.
- **Zwischensequenz**: Sprung nach ROOM4000 startet die Eintrittsszene, die S216 per Skript feuert ->
  `[aot] Tuer S216 in einer Zwischensequenz gefeuert -> RE1.5-Uebergang ohne Sequenz` (PORT-WAHL des Bauers, deckt
  sich mit T2 6.3 "Cutscene-Uebergaenge nicht anfassen").
- **Schliesston nicht abgeschnitten** (`pruefer1_schliesston.py`): Referenz = zwei Sequenzen S043+S046 hintereinander
  (Schliesston klingt dort ungestoert, Ticks 170..208, ~38 Ticks = 1,3 s); gegen den Echtlauf s043b per
  Kreuzkorrelation ausgerichtet (Versatz 274 Ticks). Nach Sequenzende (= Raumwechsel nach Tick 170) folgt der
  Echtlauf der Referenz weiter: Korrelation je Tick 0,70 / 0,90 / 0,68 / 0,64 / 0,65 / 0,67 / 0,51 ... 0,43 bis Tick
  191, danach geht der abklingende Ton im Raum-BGM unter. Der Ton laeuft also ueber `re15_audio_load_room_banks`
  hinweg (Gegenstueck RE2 @0x800597a4..0x80059818, selbst nachgelesen, s. 5).
- **Kein Haenger**: jeder Lauf erreicht sein `RE15_EXIT_AT`; die zwei Zuglaeufe ohne Feuer (S314/S322) lagen in
  Annette-Eintrittsszenen (Bild F100/F300 zeigt den Dialog), kein Tuerfehler.

## 4. Stichproben Bytes/Adressen

- Tabelle @0x8009a520 (RE2 PSX.EXE, Dateioffset 0x8AD20 = 0x8009a520 - 0x80010000 + 0x800): Bytes von DOOR00..02,
  DOOR13 (`a8 3d 04 95 08 00 00 00 96 c1 00 00`) und DOOR25 (`d8 44 fc 85 09 00 00 00 18 22 00 00`) gleich
  `gen/re2_tuer_tabelle.inc`; DOOR13 8*0x800 + 0x9504 = 54532 = Dateigroesse. sha1 DOOR13/DOOR2D in
  `shared_assets/RE2/DOOR` = `info/re2leon/COMMON/DOOR` (unveraendert).
- RE2 selbst disassembliert (`re2_disasm.py`): Member_set `80055c14 lw a0,340(s0)`, `80055c18 lbu a1,1(v0)`,
  `80055c1c lh a2,2(v0)`, `80055c30 addiu v0,v0,4`; Setter-Delay `80055d30 sw a2,56(a0)` / `d38 sw a2,60` /
  `d40 sw a2,64`; 0x8A/0x8B/0x8C `80059378 addiu v1,v1,6`, `800593c8 addiu v1,v1,6`, `8005941c addiu v1,v1,8`;
  Raumbank-Key-Off `800597ac addiu s1,zero,24`, `800597bc/c0` s3 = 0xfffebbbf, `800597c8/cc` s2 = 0x2980e,
  `80059800 addu`, `80059804 sltu`, `80059808 bne`, `80059810 jal 0x80079498` — alles wie im Code zitiert.
- RE1.5 (`re15_disasm.py`): FUN_80014368 @0x80014368..0x8001442c Befehl fuer Befehl gegen
  `re15_aot_point_in_quad_fun80014368` verglichen (vier Tests, Operandenreihenfolge der vier `slt`, Rueckgabe 1 nur
  bei `beq` @0x80014428 mit Delay `ori v0,zero,1`) — stimmt; Scan-Wahl `80042f04 andi v0,v1,0x80`,
  `80042f10 jal 0x80014368`; Nutzlast `80042f90 addiu a0,s0,20`; Installer `80040630 addiu v0,v1,40` / `80040634
  addiu v0,v1,32` — stimmt.
- **Zaehlung**: `gen/tuer_zuordnung.inc` 368 Zeilen = 184 Seiten x xxx0/xxx1; gegen `zuordnung.json` je Seite
  Archiv, Variante, Spender, Raumdateien und Band: **184/184, 0 Abweichungen, 0 weggelassen** (S311 gebaut, s. 2).
  86 Tuer-Ids = 83 abgedeckte + 3 einseitige. Schluessel-Kollision abgedeckt/nicht abgedeckt (gleiche Raumdatei +
  Rechteck/Viereck + Band ueber alle 324 Zensus-Seiten): 0.

## Befunde

**B1 (niedrig) — 4 der 184 gebauten Seiten sind in RE1.5 nie begehbar.** S081 (ROOM1180/1181 Slot 4), S276 (ROOM5070/5071
Slot 1), S277 (Slot 2), S304 (ROOM5120/5121 Slot 2): jeder Door_aot_set dieser Seiten hat sce 0 (Zensus-Rohbytes
`3b 04 00 31`, `3b 01 00 31`, `3b 02 00 31`, `3b 02 00 31`), und in keiner der sieben Raumdateien steht ein
Aot_reset(slot, 2, ...) (Suche nach `46 <slot> 02`, leer); sce 0 ist inert (op_aot_reset-Kommentar, LAB_80040738).
Die Sequenz dieser Seiten kann der Nutzer nie sehen; die Tueren bleiben ueber ihre Gegenseiten abgedeckt (83 bleibt).
Dazu je eine Variante inert: S069 nur fuer Leon (ROOM1170 Slot 1 sce 0), S216 nur fuer Elza (ROOM4001 sce 0).
Wirkung: Die Nutzer-Aussage "184 Tuerseiten spielen beim Durchgehen die RE2-Sequenz" gilt fuer 180 Seiten; kein
Verhaltensfehler.

**B2 (niedrig) — Griff-Tausch S042 (T027, DOOR13 V1 <- DOOR07): der Druecker ist kaum erkennbar.** Im Anfangsbild ein
duennes dunkles olivfarbenes Band (~18 x 2 px bei 320x240), keine Rosette; das RE1.5-Bild zeigt einen hellen,
verchromten Druecker mit Rosette, zur Blattmitte zeigend. Seite (rechts) und Formfamilie (Druecker) stimmen, beim
Aufziehen kippt er. `t4p1_s042_griff_zoom.jpg`. PORT-WAHL des Bauers (Spender-Mesh + Spender-TIM Platz 25).

**B3 (niedrig, nicht gegen RE2 geprueft) — DOOR29 V1: Schildfragment.** In der Oeffnungsphase aller 9 V1-Seiten (S227
S231 S234 S238 S241 S242 S250 S258 S297) steht ueber dem Schild "SHAFT TYPE-M" ein gespiegeltes Schriftstueck, der
Warnaufkleber erscheint doppelt; V0 zeigt das nicht (`t4p1_door29_mitte.jpg`, links V1 S234, rechts V0 S229). Die
T2-Python-Uebersicht (`build/r31_tueren/t2/bilder/DOOR29_v1.png`, Z-Puffer) zeigt im Bild 0 ebenfalls ein
gespiegeltes Schild — vermutlich Modelleigenschaft (beidseitiges Schild) unter der RE2-OT-Sortierung, also womoeglich
originalgetreu. Offen, bis eine RE2-Aufnahme von DOOR29 V1 vorliegt.

**B4 (niedrig) — Belegfuehrung des Bauers traegt zwei Aussagen nicht.** (a) Bau-Dossier 6 "Oeffnungsrichtung ...
Schiebetueren 19/2A in ihre Richtung" am Kontaktbogen abgelesen — dort ist "Mitte" bei DOOR19/DOOR2A bildgleich mit
"Anfang" (Bogenhaken misst nur Drehung / Lage y/z), und die 240x180-Zellen zeigen die Griffform nicht. (b) Die fuenf
Echtlaeufe liefen ueber `RE15_FIRE_AOT`, also am Scan (Band-Gate, Viereck-Trefftest, Zwischensequenz-Sperre)
vorbei. Beides hier nachgeholt (Abschnitte 2 und 3), Ergebnis ohne Fehler.

## Log

- 2026-09-29: Bau 409/409; Bogen-Neuerzeugung bitgleich; 31 Griffboegen angesehen; DOOR2D-Richtung gemessen;
  Echtlaeufe s043, s043b, s225, s226, s290, s290f, s052, s159, s159_zu, s159_auf, laden, elza_s016, tor2, s001
  (+ Sonden elev_a, s314, s322, s087, tor, AOT-Dumps 4050/1110/6000/4000/50C0/2070/1170); Schliesston-Korrelation;
  Stichproben RE2/RE1.5 disassembliert; Zaehlung 184/184.
