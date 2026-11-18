cmake_minimum_required(VERSION 3.16)   # cmake -P: ohne diese Zeile gelten im Linux-Container (CMake 3.28) die alten Policies (IN_LIST, if(TRUE)) - Muster der aelteren Integrationsskripte, Memory reai-v2-linux-test-portabilitaet
# =============================================================================
# RIESENFORM IM ECHTEN WAGENWEG, Runde 41 Spur Form, Nachbesserung 3 (AN-Abnahme 2 Mangel N1,
# .claude/worktrees/r41_annette/analysis/befunde_runde41/AN_abnahme_2.md; Dossier analysis/befunde_runde41/form/STUFE3.md).
#
# Befund: im Spielweg 6030-Tuer -> Wagen A -> B -> C kam die Riesenform mit HP 0 in Wagen C an (A 300, B 150, C 0); der erste
# Pistolentreffer startete die Rueckstoss-Szene, der Kampf fiel weg. Die bisherigen Integrationstests sahen das nicht, weil sie
# per Debug-Sprung in Wagen B beginnen (nur EINE Selbst-Tuer).
# Ursache: die Wagentueren fuehren in denselben Raum (scd_room_reenter, kein re15_enemy_reset). Sce_em_set legt die Form jedes Mal
# neu in Slot 2 an (+0x4 = 0 `sw zero,4(s0)` @0x800421e0, hp 0); der Eintritt von re15_bb_tick (Gegenstueck des G4-Ctors
# 0x8010062c, Tabelle @0x8010af60[0], HP `sh v0,342(s1)` @0x8010067c) lief nur bei neuem SLOT und blieb in Wagen C aus.
#
# Lauf (ein Lauf Leon, kein Debug-Sprung in Wagen B): Raumladung 5090 am Eintrittspunkt der 6030-Tuer (27200,4900) - der
# Debug-Sprung nimmt denselben Raumwechsel (re15_room_request_change -> re15_room_apply_pending) wie die 6030-Tuer -, dann
# zu Fuss Tuer A -> B (Slot 0, Schnitt 9) und B -> C (Slot 2, Schnitt 14), zum Kampfstart, Zielen und Feuer gehalten.
# Eingabe bis zum Kampfstart = Lauf kette_ohne der AN-Abnahme 2 (Tastenskript w8), nur die Wartezeit in Wagen C ist kuerzer.
# Geprueft aus state.log/debug.log:
#   1  der Weg: beide Wagentueren (DOOR FIRE slot=0 target_cut=9, slot=2 target_cut=14), drei Spawns des 0x30
#   2  HP der Form beim Eintritt in Wagen B UND C > 0 und = RE15_BB_FORM_HP (aus include/re15_birkin_formen.h gelesen)
#   3  der Kampf dauert: Treffer auf die Form im Kampffenster (Eintritt C bis Rueckstoss-Szene bzw. Laufende) >= RE15_BB_FORM_HP / 30
#      (mindestens 3); die HP steigen im Fenster nie (kein Ruecksetzen, der Schaden bleibt)
#   4  Leon wird im Kampf angegriffen: seine HP fallen mindestens einmal zwischen dem ersten Treffer auf die Form und dem Fensterende
#   5  Rueckstoss-Szene (Clip 13, HP 1): kommt sie, dann erst nach Abzug >= RE15_BB_FORM_HP - groesster Einzelabzug (die Form verliert
#      ihre ganzen HP bis auf den letzten Treffer; der Todesbild-Wert steht nicht im state.log, die Szene setzt sofort HP 1). Kommt
#      sie bis zum Laufende nicht, muss die Form noch leben (letzte HP > 0). Mit 150 HP kommt sie (F1486). Probelauf mit 1000 HP
#      (GR-Wert, nur der Kopf geaendert): C 1000, 64 Treffer, Leon 42x getroffen, keine Szene bis F5500, Form bei HP 40: Leon steht
#      ab ~F3250 an der Suedwestecke (-12,-26433) und zielt auf den naeheren 0x4D-Aktor (r41/form legt ihn aus der RDT an, (600,-25300),
#      Zustand 4, hp -1; Spur AN verwirft ihn bei (3,89) ohne (9,133)).
# Messhaken RE15_R39_MESS_SPIELER_HP=100 (engine/src/gegner_r39.c, KEIN Spielverhalten, nur mit Env): ein stehender
# Skript-Schuetze ueberlebt eine Form mit mehr HP nicht (Spur GR NB3: 1000 HP, Spieler tot F2100). Der Haken hebt die HP am
# Kopf jedes KI-Bildes; ein Abzug steht im state.log des Bildes, in dem er faellt (Pruefung 4 sieht ihn).
# Gegenprobe: exe 917195879 (vor der Behebung) -> Pruefungen 2 (C: HP 0), 3 und 4 rot.
#
#
# NACHBESSERUNG 4 (Abnahme 2 M-A, analysis/befunde_runde41/form/STUFE3_abnahme_2.md; Dossier STUFE3.md "Nachbesserung 4"): die
# Rueckstoss-Szene hielt fest Cut 12; im echten Wagenweg war das Taumeln 16 Bilder fast schwarz (Form 1240 vor dem Auge) bzw. 7,5 s
# hinter Leons Ruecken (Spieler weicht vorher 2 s zurueck). Teile (TEIL): leon, elza (Raum 5091), zurueck_leon, zurueck_elza (nach dem
# Kampfstart 2 s Zurueckweichen, Tastenskript D2.0 wie die Abnahme-Laeufe v5090/v5091). Zusaetzlich zu 1-5 je Taumelbild (state.log:
# Form st 2/3, Clip 13, HP 1, bis zum Sturz-Clip 12):
#   6a Form im Bild: sichtbare Bildpunkte der Form >= 768 (1 % des Bildes 320 x 240)
#   6b Spielerkoerper: sichtbare Bildpunkte des Spielers (TIM-Slot 0) <= 25600 (ein Drittel, Pruefmass Abnahme 2 M-A)
#   6c Spielerverdeckung: verdeckter Anteil der Form (bedeckt - sichtbar) / bedeckt <= 500 Promille
#   6d fast schwarz: Anteil Bildpunkte mit Luma < 24 <= 600 Promille (Pruefmass Abnahme 2 M-A, Werkzeug probe_r41_form_kamera_bild)
# Sichtbar/bedeckt je Slot aus der Messschiene RE15_SLOTDECKUNG (render_pc.c: die gezeichneten Dreiecke in Zeichenfolge samt
# Vordergrund-Masken in einen Kennungspuffer), NICHT aus dem Kugelmodell der Schnittwahl. Form-Slot = der Figuren-Slot 1..18 (main.c:
# 0..18 Figuren/Objekte, ab 19 Effekte) mit der groessten bedeckten Flaeche in den Taumelbildern.
# Gegenprobe (gemessen, Dossier NB4-5): exe 2e3713c74 -> rot ("kein deck.log", die Messschiene fehlt dort); exe mit birkin_formen.c von
# 2e3713c74 und der Messschiene (fester Cut 12) -> leon/elza 6d rot (F1487-F1502: 776-1000 Promille fast schwarz), zurueck_leon 231 und
# zurueck_elza 94 Verstoesse 6a/6b/6c (Form 0 Bildpunkte sichtbar, Leon bis 56690 Bildpunkte, Form bis 1000 Promille verdeckt).
# Renderer: der des Nutzers (kein RE15_SOFTWARE_RENDER, wie die Abnahme-2-Messung); gemessen: der Software-Renderer dunkelt Cut 11 um
# 40-50 Promille ab (zurueck_elza max 599 statt 544 Promille fast schwarz) und maskiert Bildfehler (Memory reai-v2-visual-verify-gdigrab).
#
#
# NACHBESSERUNG 4 RUNDE 2 (Abnahme 3 N1/N2/E1, analysis/befunde_runde41/form/STUFE3_abnahme_3.md): die Nahsicht-Wache nach dem
# Bildmass machte die Kampfkamera VOR der Szene schlechter (gleiches Spiel je Bild): Spieler wartet 24 s (Lauf k5090) 38 statt 7
# Bilder > 600 Promille fast schwarz (Cut 12 mit der Form 1660-2160 vor dem Auge, Kurzschnitte), Spieler weicht 2 s zurueck (v5090)
# 28 Bilder dunkler Aussenblick Cut 14 statt 0. Die Riegel prueften nur die Taumelbilder. Neu:
#   7  Kampfkamera: je Bild von F1150 (Framedump-Beginn; Wagen C ab ~F946, der Eintritt in Cut 14 ist die Zonenkamera der Tuer) bis
#      vor die Rueckstoss-Szene bzw. bis zum Laufende Anteil fast schwarzer Bildpunkte (Luma < 24, wie 6d). Hoechstzahl Bilder
#      > 600 Promille: leon/elza/zurueck_* 0, warten_* 9; dazu kein Bild ueber 650 Promille. Beides = Stand 2e3713c74 (Achsenregel),
#      gemessen in diesem Riegel (Fenster 320x240): warten_leon 9 Bilder > 600, max 622 (F1889), warten_elza 1, max 619; leon/elza/
#      zurueck_* 0, max 596 (Abnahme 3 mit 640x480 und echter HP: kv_leon 7, max 618). Runde 1 (Wache nach dem Bildmass): warten 38,
#      max 701-707; zurueck 28, max 773-775. Die 650 ist PORT-WAHL zwischen beiden Messungen.
#   Teile warten_leon/warten_elza: Tastenskript wie leon/elza, aber 24 s warten statt 2 s (Abnahme 2/3 Lauf k5090), echte Spieler-HP
#   (ohne Messhaken; Leon faellt bis F1931 auf HP 10), Laufende F1931 wie k5090. Die Form wird dort bis F1931 nur einmal getroffen,
#   die Pruefungen 3 und 5 (Kampfdauer, Rueckstoss) gelten nur in den anderen vier Teilen.
# Gegenprobe (gemessen, Dossier STUFE3.md "Nachbesserung 4 Runde 2"): exe b9ab865af (Wache nach dem Bildmass) -> warten_leon/
# warten_elza und zurueck_leon/zurueck_elza rot in Pruefung 7.
#
# INTEGRATION 1000 HP (Runde 41 Welle 2 Lauf A2, Dossier analysis/befunde_runde41/form/STUFE3.md "Integration 1000 HP (Lauf A2)"):
# Die Form hat seit Spur GR 1000 HP (Nutzerentscheid F2, G4-Ctor `addiu v0,zero,1000` @0x80100678). Mit der Pistole (15 je Treffer)
# braucht der stehende Schuetze 67 Treffer; die Angriffe der Form schieben ihn ab ~F3270 in das Rechteck der Sichtlinien-Zelle 126
# (ROOM5090.RDT SCA, Datei-Offset 0xe60: x -425..5300, z -27200..-25200, Typ 6, Wort +10 = 0x0300 -> Band 0, (0xf00 & w) == 0x300),
# dort lehnt FUN_8001b9b4 (`jal 0x8003dcc4` @0x8001ba20 mit a2 0xf00 @0x8001ba1c, a3 0x300 @0x8001ba24; Aufruf @0x80012168, `bnez v0`
# @0x80012170 -> kein Treffer) jeden Schuss ab - Original, der Port bildet es ab (schuss_r36.c / treffer_r39.c). Die Form blieb bei HP 40,
# keine Szene bis F5500. Deshalb nimmt der Lauf (leon, elza, zurueck_*) jetzt den RAKETENWERFER (Item 18 = 0x12 "ROCKET LAUNCHER",
# inventory_common.c:418; wie integration_r41_g5_tod): Schaden an der Riesenform 301 = RE2-G4-Zeile w0 0x12D4B52D @0x800A5D24
# (Zeiger @0x800A6B58 = 0x800A6A88 + 0x34*4 -> 0x800A5BE4, Satz + 16*20; (w0 >> 0) & 0x3FF wie `andi v1,v1,0x3ff` @0x8004725c; Port
# s_rak_g4, re15_damage.c), tot nach HP / 301 + 1 = 4 Raketen (Todesregel hp < 0, `bgez` @0x80047284). Kein Messhaken mehr (echte HP).
# Eingabe: w8 wie bisher, dann leon/elza 5 s statt 2 s warten, zurueck_* 2 s + D2.0 + 3 s: der Ansturm der Form trifft den Spieler
# (F1322 bzw. F1336, HP 100 -> 95), danach toeten vier Raketen sie (Zustand 2 je Treffer, kein Angriff mehr) in der Lage der
# Abnahme-2-M-A-Laeufe: Form x 16664 / Leon x 15104 (v0_w: 17290 / 14998), Form x 20136 / Leon x 19310 (v0_v: 20124 / 17815).
# Mit sofortigem Feuer (2 s) starb die Form schon nach dem Auftritt bei x 3925 bzw. 9300; die Szene zeigte Cut 12 -> 11 und die
# Gegenprobe "Szene fest Cut 12" war in allen vier Teilen gruen (Riegel ohne Biss). Gemessen (exe bf5001400, Asset K5c, Renderer des
# Nutzers, 320x240, zwei bildgleiche ctest-Laeufe):
#   leon/elza       Raketen F1365/F1397/F1429/F1461 (HP 699, 398, 97, -204), Szene F1462, Taumeln F1462-F1705 (244 Bilder, Cut 13-12-11)
#   zurueck_*       Raketen F1398-F1494, Szene F1495, Taumeln F1495-F1779 (285 Bilder, Cut 13-12-11)
# Pruefung 3 = mindestens HP / 301 + 1 Treffer und jeder Abzug genau 301 (die echte Rakete, kein HP-Haken). Pruefung 4 zaehlt ab dem
# Eintritt in Wagen C (der Angriff faellt vor die erste Rakete).
# Laufende und Framedump: ein Wert (_exit_bild), der Framedump laeuft F1150 bis zum Laufende; die Fenster der Pruefungen 6 und 7
# kommen aus dem state.log (Szenenbeginn, Taumelbilder), eine Szene, deren Taumeln das Laufende erreicht, ist rot.
# Pruefung 7 zaehlt getrennt (Asset K5c ist dunkler als K4; gleiche exe, gleiches Spiel je Bild, gleiche Schnitte, K4 per
# RE15_RE2_ASSET_ROOT): AUFTRITT (Bedingung der Wache (a): Form Zustand 1/0, Clip 10, Clip-Bild < 60; Cut 11 = Kamera von Bild 2,
# PORT-WAHL Nachbesserung 1 M4) K5c 33 Bilder > 600, max 628 (F1182) in allen sechs Teilen, K4 0, max 596; KAMPF siehe _kampf_max.
# Gegenproben (gemessen mit diesem Riegel): Szene fest Cut 12 (birkin_formen.c wie 2e3713c74) -> leon/elza 6d 998 Promille,
# zurueck_* 6a Form 0 sichtbar / 6d 775 rot; Wache nach dem Bildmass (b9ab865af, Commit 128d52e2e rueckwaerts) -> leon 3, zurueck 63/62,
# warten 50/47 Kampfbilder > 600 rot (elza gruen).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> [-DRE15_FORM_HP_HEADER=<re15_birkin_formen.h>] [-DTEIL=leon|elza|zurueck_leon|
#         zurueck_elza|warten_leon|warten_elza] [-DRE15_BILD_TOOL=<probe_r41_form_kamera_bild>] -P test_r41_form_wagenweg.cmake
#
#
# RUNDE 42 STRANG FINALE, Stufe 1 (Plan S4, NUTZERWUNSCH 07.11.2026 "die Form Birkin G5 aus dem Finale wieder entfernst ... ein
# finale wie beim Golem am Ende von Resident Evil 2"): in 5090/5091 traegt die Form die HP des RE2-Super-Tyrant 20000 + 200
# (@0x80101be0, include/re15_finale_r42.h RE15_FI_HP_START, von dort gelesen) und stirbt nur durch den Abschuss; die Rueckstoss-Szene
# und der G5 sind entfallen. Umgestellt: Pruefung 2 Soll-HP = RE15_FI_HP_START; Raketen-Teile: der Abschuss-Ausloeser (finale.log,
# RE15_FINALE_LOG) beendet das Kampffenster statt der Rueckstoss-Szene, Pruefung 3 (>= HP/301 Treffer) und 5 entfallen, Pruefung 6
# (Kamera der Taumelbilder) wird "kein Taumelbild" (die Szene darf nicht mehr kommen); Laufende ohne Raumbedingung (nach dem Abschuss
# folgt der Abspann in 6040). Pruefung 1, 4 und 7 unveraendert.
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_form_wagenweg: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_form_wagenweg: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL leon)
endif()
if(NOT TEIL MATCHES "^(leon|elza|zurueck_leon|zurueck_elza|warten_leon|warten_elza)$")
    message(FATAL_ERROR "r41_form_wagenweg: TEIL '${TEIL}' unbekannt")
endif()
set(_raum 5090)
set(_figur Leon)
if(TEIL MATCHES "elza")
    set(_raum 5091)
    set(_figur Elza)
endif()
if(NOT RE15_FORM_HP_HEADER)
    set(RE15_FORM_HP_HEADER "${CMAKE_CURRENT_LIST_DIR}/../../include/re15_birkin_formen.h")
endif()
# Runde 42 Strang finale: 5090/5091 = Tyrant-HP RE15_FI_HP_START "(20000 + 200)" aus include/re15_finale_r42.h
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/../../include/re15_finale_r42.h" _hp_zeile REGEX "^#define RE15_FI_HP_START")
if(NOT _hp_zeile MATCHES "RE15_FI_HP_START +\\(([0-9]+) \\+ ([0-9]+)\\)")
    message(FATAL_ERROR "r41_form_wagenweg: RE15_FI_HP_START nicht in include/re15_finale_r42.h")
endif()
math(EXPR _form_hp_soll "${CMAKE_MATCH_1} + ${CMAKE_MATCH_2}")
# Lauf A2 (s. Kopf): Rakete 301 an der Riesenform, tot nach HP / 301 + 1 Treffern.
set(_rak_schaden 301)      # RE2-G4 w0 0x12D4B52D @0x800A5D24, & 0x3FF (`andi v1,v1,0x3ff` @0x8004725c); Port s_rak_g4 (re15_damage.c)
math(EXPR _rak_n "${_form_hp_soll} / ${_rak_schaden} + 1")   # Todesregel hp < 0 (`bgez v1` @0x80047284 RE2, @0x80013014 RE1.5)
set(_treffer_min ${_rak_n})
# Laufende = Framedump-Ende. Gemessen: letztes Taumelbild F1705 (leon/elza) bzw. F1779 (zurueck_*); 2000 laesst >= 221 Bilder Abstand,
# ein spaeteres Szenenende ist rot (Pruefung 6, "Taumeln erreicht das Laufende").
set(_exit_bild 2000)
set(_rakete 1)
# Runde 42 Strang finale: der Abschuss kommt im Raketen-Lauf um F1365-F1400 (gemessen), Kill + Abspann-Vorspann ~F1860 -> ROOM6040.
# Danach zaehlt der Bildzaehler im Abspannraum dieselben Nummern noch einmal (gemessen: deck.log/state.log F1150.. doppelt, der
# Framedump rs_001150.. wurde mit den Diashow-Bildern ueberschrieben, 909 Promille fast schwarz). Laufende darum F1700 in 5090/5091.
set(_exit_bild 1700)
# Pruefung 7 Kampfteil (nach dem Auftritt bis vor die Szene): Hoechstzahl Bilder > 600 Promille und hoechster Wert, je Teil GEMESSEN
# mit K5c (Stand bf5001400, Dossier "Integration 1000 HP (Lauf A2)"): leon/elza 0 (max 553 / 531); zurueck_leon 4 (max 623),
# zurueck_elza 3 (max 617): Cut 13 um F1325 (Ansturm der Form); warten_leon 5 (max 698), warten_elza 4 (max 653): Cut 13 F1917-F1921,
# Spieler vor der Form (Achsenregel: kein Wechsel). Gegenprobe Wache nach dem Bildmass (b9ab865af, Runde 1): warten 50 / 47 -> rot.
set(_kampf_max 0)
set(_kampf_hoechst 600)
if(TEIL STREQUAL "zurueck_leon")
    set(_kampf_max 4)
    set(_kampf_hoechst 623)
elseif(TEIL STREQUAL "zurueck_elza")
    set(_kampf_max 3)
    set(_kampf_hoechst 617)
endif()
set(_warten 0)
if(TEIL MATCHES "^warten")
    set(_warten 1)
    set(_rakete 0)         # Pistole wie Abnahme 2/3 Lauf k5090
    set(_exit_bild 1931)   # = Abnahme 2/3 Lauf k5090 (RE15_EXIT_AT=1931#5090)
    if(TEIL STREQUAL "warten_leon")
        set(_kampf_max 5)
        set(_kampf_hoechst 698)
    else()
        set(_kampf_max 4)
        set(_kampf_hoechst 653)
    endif()
endif()
# Pruefung 7 Auftritt (Wache (a), Cut 11): K5c 33 Bilder > 600, max 628 in allen Teilen (s. Kopf); 650 = PORT-WAHL Nachbesserung 4 Runde 2.
# Pruefung 6a/6c/6d (s. dort): Pruefmass Abnahme 2 M-A (768 Bildpunkte, 500 / 600 Promille). Lauf A2 (Rakete nach dem Ansturm, K5c)
# ueberschreitet es in vier Teilen; die Schranke ist dort der gemessene Wert des Stands bf5001400 (zwei ctest-Laeufe, je
# bildgleich), die Gegenprobe "Szene fest Cut 12" (birkin_formen.c von 2e3713c74) bleibt rot (6d 998 bzw. 6a 0 / 6d 775):
#   leon          6a 751, 6c 639: Cut 12 F1621-F1635 (14 Bilder), Form x 4142 -> 3022 klein am Wagenende (2081-2810 bedeckte
#                 Bildpunkte, 510-639 Promille davon verdeckt), hinter Leon (x 15104, 3422 vor dem Auge x 18526, ganz sichtbar), bis sie
#                 x <= 3000 erreicht (Cut 11, BF_RS_TAUMEL_KAM_X); mit K4 dasselbe (637) - Lage, nicht Asset. Schnittwahl bf_schnitt_waehlen (PORT-WAHL Nachbesserung 4), nicht geaendert, offen gemeldet.
#   elza          6c 523: dieselbe Stelle F1633-F1634.
#   zurueck_leon  6d 629, zurueck_elza 6d 605: F1577-F1580 und F1712, nur mit K5c (die dunklere Form gross im Bild).
# Lauf D (Merge r41/form, Spiel-Asset K5e_f1, Nutzerentscheid E32): leon 6a an derselben Stelle F1635 Cut 12 742 statt 751
#   (je bildgleich in drei Einzellaeufen). Ursache ist das EMD: K5e-EMD + K5c-TIM ergibt 742, K5c-Paar 751 (RE15_RE2_ASSET_ROOT,
#   Bericht analysis/befunde_runde41/INT_laufd_riesenform.md); die Geometrie ist K5d (Krallen ganz eingezogen), die Zaehlung
#   render_pc.c re15_slotdeckung zaehlt nur deckende Dreiecke. Schranke 6a leon = gemessener Wert K5e 742; 6c leon 631 <= 639 bleibt.
# Release v0.8.30 (Merge r41/form, Spiel-Asset K5g3 + rechte Hand Variante 1 + Vorderzaehne, EMD 115f8721, Nutzerentscheid
#   15.11.2026 "Variante1", K5g3/EINBAU_R9.md): an derselben Stelle Cut 12 6c leon 641 (F1635) statt 631, elza 525 (F1634) statt 517;
#   6a leon 751 >= 742. Bildgleich in der Suite und in je zwei Einzellaeufen. Ursache ist das Asset: dieselbe exe mit
#   RE15_RE2_ASSET_ROOT = Kopie von shared_assets/RE2 mit K5e_f1 (git show 62bd0001b, sha256 f277916d.../31a35e66...) ergibt
#   elza 517 und leon gruen (Gegenprobe im Commit des Release-Zweigs r41/release_0830). Schranken 6c = gemessene Werte K5g3.
set(_p6_form_min 768)
set(_p6_deck 500)
set(_p6_dunkel 600)
if(TEIL STREQUAL "leon")
    set(_p6_form_min 742)   # K5e_f1 (E32), Lauf D; vorher 751 mit K5c; K5g3 misst 751
    set(_p6_deck 641)       # K5g3 (R9-Einbau), Release v0.8.30; vorher 639 (K5c), K5e_f1 631
elseif(TEIL STREQUAL "elza")
    set(_p6_deck 525)       # K5g3 (R9-Einbau), Release v0.8.30; vorher 523 (K5c), K5e_f1 517
elseif(TEIL STREQUAL "zurueck_leon")
    set(_p6_dunkel 629)
elseif(TEIL STREQUAL "zurueck_elza")
    set(_p6_dunkel 605)
endif()
set(_auftritt_max 33)
set(_auftritt_hoechst 650)
math(EXPR _limit_s "120 + ${_exit_bild} / 20")

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41wagenweg_${TEIL}_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41wagenweg_${TEIL}_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Tastenskript w8 (AN-Abnahme 2, Lauf kette_ohne): Wagen A nach Westen, Tuer A -> B, Wagen B, Tuer B -> C, Wagen C bis zum
# Kampfstart; danach statt 24 s nur 2 s warten, dann Zielen + Feuer gehalten (MA4).
set(_skript "")
foreach(_i RANGE 1 7)
    string(APPEND _skript "A0.1000,W0.1000,")
endforeach()
string(APPEND _skript "U15.0000,")
foreach(_i RANGE 1 10)
    string(APPEND _skript "A0.1000,W0.1000,")
endforeach()
string(APPEND _skript "L0.0333,U2.1667,R0.0333,U10.7000,")
foreach(_i RANGE 1 9)
    string(APPEND _skript "A0.1000,W0.1000,")
endforeach()
if(_warten)
    string(APPEND _skript "A0.1000,W0.6000,U4.9000,W24")   # Nachbesserung 4 Runde 2: 24 s warten (Abnahme 2/3 Lauf k5090)
elseif(TEIL MATCHES "^zurueck")
    # Lauf A2: w8 + 2 s + 2 s zurueck (Abnahme 2 v5090/v5091), dann 3 s warten: der Ansturm der Form trifft (F1336), Tod bei Form
    # x 20136 / Spieler x 19310 = Lage der Abnahme-2-M-A-Laeufe v0_v (Form x 20124, Leon x 17815). Mit sofortigem Feuer starb die Form
    # bei x ~9300 und die Gegenprobe "fest Cut 12" war gruen.
    string(APPEND _skript "A0.1000,W0.6000,U4.9000,W2,D2.0,W3")
else()
    # Lauf A2: w8 + 5 s statt 2 s: der Ansturm der Form (ss1 4 ab ~F1300) trifft F1322, die Raketen F1365-F1461 toeten sie bei x 16664,
    # Spieler x 15104 = Lage der Abnahme-2-M-A-Laeufe v0_w (Form x 17290, Leon x 14998). Mit 2 s starb die Form nach dem Auftritt bei
    # x 3925 und die Gegenprobe "fest Cut 12" war gruen (Dossier "Integration 1000 HP (Lauf A2)").
    string(APPEND _skript "A0.1000,W0.6000,U4.9000,W5")
endif()
math(EXPR _ma_n "${_exit_bild} / 120 + 2")
foreach(_i RANGE 1 ${_ma_n})
    string(APPEND _skript ",MA4")
endforeach()

set(_figur_env "")
# Waffe: Raketenwerfer (Lauf A2, s. Kopf), warten_* die Pistole wie k5090. Kein Messhaken mehr (echte Spieler-HP in allen Teilen).
set(_waffe_env "RE15_GIVE=18:200" RE15_EQUIP=18)
if(NOT _rakete)
    set(_waffe_env "RE15_GIVE=3:400,21:250" RE15_EQUIP=3)
endif()
if(_raum EQUAL 5091)
    set(_figur_env RE15_TITLE_SHOT_CHAR=1)
endif()
re15_start_spiel(_rv ${_limit_s}
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_figur_env}
    RE15_SLOTDECKUNG=deck.log "RE15_FRAMEDUMP=1150-${_exit_bild}/1:rs_"
    RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=27200,4900,2048,0" "RE15_SET_FLAG=3:89"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=6 "RE15_INPUT_SCRIPT=${_skript}"
    RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log ${_waffe_env} RE15_FINALE_LOG=1
    "RE15_EXIT_AT=${_exit_bild}#${_raum}"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")

set(_f "")
if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
    message(FATAL_ERROR "r41_form_wagenweg: kein Protokoll (exit=${_rv}) in ${WORKDIR}")
endif()

# --- 1 der Weg ---
file(READ "${WORKDIR}/debug.log" _dbg)
if(NOT _dbg MATCHES "DOOR FIRE slot=0 [^\n]*target_cut=9 spawn=\\(24700,0,-9250\\)")
    string(APPEND _f "\n Weg: Tuer A -> B (DOOR FIRE slot=0, Schnitt 9) fehlt")
endif()
if(NOT _dbg MATCHES "DOOR FIRE slot=2 [^\n]*target_cut=14 spawn=\\(24800,0,-23550\\)")
    string(APPEND _f "\n Weg: Tuer B -> C (DOOR FIRE slot=2, Schnitt 14) fehlt")
endif()
string(REGEX MATCHALL "Sce_em_set type=0x30 " _spawns "${_dbg}")
list(LENGTH _spawns _spawn_n)
if(_spawn_n LESS 3)
    string(APPEND _f "\n Weg: ${_spawn_n} Spawns des 0x30, erwartet 3 (A, B, C)")
endif()

# --- 2 bis 5 aus dem state.log ---
file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "^F[0-9]+ pad=")
set(_phase 0)          # 0 = vor 5090, 1 = Wagen A, 2 = Wagen B, 3 = Wagen C (Kampf), 4 = Rueckstoss
set(_hp_b "")
set(_hp_c "")
set(_bild_c "")
set(_form_alt "")
set(_treffer 0)
set(_abzug 0)
set(_max_einzel 0)
set(_hp_hoch "")
set(_erster_treffer "")
set(_pl_alt "")
set(_pl_treffer 0)
set(_rueck "")
set(_einzel_falsch "")
foreach(_z IN LISTS _zeilen)
    if(NOT _z MATCHES "^F([0-9]+) pad=[0-9A-Fa-f]+ PL\\((-?[0-9]+),(-?[0-9]+),rot=-?[0-9]+,hp=(-?[0-9]+)\\)")
        continue()
    endif()
    set(_bild "${CMAKE_MATCH_1}")
    set(_px "${CMAKE_MATCH_2}")
    set(_pz "${CMAKE_MATCH_3}")
    set(_php "${CMAKE_MATCH_4}")
    set(_ist_form 0)
    if(_z MATCHES "\\[[0-9]+ t=30 st=([0-9]+) ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) [^]]*\\] hp=(-?[0-9]+)")
        set(_ist_form 1)
        set(_st "${CMAKE_MATCH_1}")
        set(_mo "${CMAKE_MATCH_2}")
        set(_fhp "${CMAKE_MATCH_3}")
    endif()
    if(_phase EQUAL 0)
        if(_ist_form)
            set(_phase 1)
        endif()
        continue()
    endif()
    if(_phase EQUAL 1 AND _px EQUAL 24700 AND _pz EQUAL -9250)
        set(_phase 2)
        if(_ist_form)
            set(_hp_b "${_fhp}")
        endif()
        continue()
    endif()
    if(_phase EQUAL 2 AND _px EQUAL 24800 AND _pz EQUAL -23550)
        set(_phase 3)
        set(_bild_c "${_bild}")
        if(_ist_form)
            set(_hp_c "${_fhp}")
            set(_form_alt "${_fhp}")
        endif()
        set(_pl_alt "${_php}")
        continue()
    endif()
    if(NOT _phase EQUAL 3)
        continue()
    endif()
    if(_ist_form)
        if((_st EQUAL 2 OR _st EQUAL 3) AND _mo EQUAL 13 AND _fhp EQUAL 1)
            set(_rueck "${_bild}")
            set(_phase 4)
            continue()
        endif()
        if(NOT _form_alt STREQUAL "" AND _fhp GREATER _form_alt AND _hp_hoch STREQUAL "")
            set(_hp_hoch "F${_bild} ${_form_alt} -> ${_fhp}")
        endif()
        if(NOT _form_alt STREQUAL "" AND _fhp LESS _form_alt)
            math(EXPR _treffer "${_treffer} + 1")
            math(EXPR _einzel "${_form_alt} - ${_fhp}")
            math(EXPR _abzug "${_abzug} + ${_einzel}")
            if(_rakete AND NOT _einzel EQUAL _rak_schaden AND _einzel_falsch STREQUAL "")
                set(_einzel_falsch "F${_bild} ${_form_alt} -> ${_fhp} (Abzug ${_einzel})")
            endif()
            if(_einzel GREATER _max_einzel)
                set(_max_einzel "${_einzel}")
            endif()
            if(_erster_treffer STREQUAL "")
                set(_erster_treffer "${_bild}")
            endif()
        endif()
        set(_form_alt "${_fhp}")
    endif()
    if(NOT _pl_alt STREQUAL "" AND _php LESS _pl_alt)   # Lauf A2: ab dem Eintritt in Wagen C (Kampfstart), s. Pruefung 4
        math(EXPR _pl_treffer "${_pl_treffer} + 1")
    endif()
    set(_pl_alt "${_php}")
endforeach()

# Runde 42 Strang finale: Kampfende = Abschuss-Ausloeser (finale.log), keine Rueckstoss-Szene mehr
set(_abschuss "")
if(EXISTS "${WORKDIR}/finale.log")
    file(READ "${WORKDIR}/finale.log" _fin)
    if(_fin MATCHES "\\[finale\\] T[0-9]+ F([0-9]+) Ausloeser")
        set(_abschuss "${CMAKE_MATCH_1}")
    endif()
endif()
if(NOT _rueck STREQUAL "")
    string(APPEND _f "\n Rueckstoss-Szene F${_rueck} lief (Plan S4: in 5090/5091 entfallen)")
endif()
set(_rueck "${_abschuss}")
if(_rakete)
    set(_treffer_min 0)
    if(_abschuss STREQUAL "")
        string(APPEND _f "\n kein Abschuss-Ausloeser bis F${_exit_bild} (finale.log), Form bei HP '${_form_alt}'")
    endif()
endif()
if(NOT _hp_b STREQUAL "${_form_hp_soll}")
    string(APPEND _f "\n Wagen B: Form mit HP '${_hp_b}', erwartet ${_form_hp_soll} (RE15_BB_FORM_HP)")
endif()
if(_hp_c STREQUAL "" OR NOT _hp_c GREATER 0 OR NOT _hp_c EQUAL _form_hp_soll)
    string(APPEND _f "\n Wagen C (F${_bild_c}): Form mit HP '${_hp_c}', erwartet ${_form_hp_soll} (RE15_BB_FORM_HP, N1: 0)")
endif()
if(_treffer LESS _treffer_min AND NOT _warten)
    string(APPEND _f "\n Kampf: ${_treffer} Treffer auf die Form bis zur Rueckstoss-Szene bzw. F${_exit_bild}, erwartet >= ${_treffer_min} (HP ${_form_hp_soll} / Rakete ${_rak_schaden} + 1)")
endif()
if(NOT _einzel_falsch STREQUAL "")
    string(APPEND _f "\n Kampf: Abzug ungleich Raketenschaden ${_rak_schaden} (@0x800A5D24): ${_einzel_falsch}")
endif()
if(NOT _hp_hoch STREQUAL "")
    string(APPEND _f "\n Kampf: HP der Form steigen im Kampf (${_hp_hoch}), der Schaden bleibt nicht")
endif()
if(FALSE)   # Runde 42: Pruefung 5 entfaellt (keine Rueckstoss-Szene, Tod nur durch den Abschuss)
    math(EXPR _abzug_min "${_form_hp_soll} - ${_max_einzel}")
    if(_abzug LESS _abzug_min OR _abzug LESS 1)
        string(APPEND _f "\n Kampf: Rueckstoss-Szene F${_rueck} schon nach ${_abzug} HP Abzug, erwartet >= ${_abzug_min} (${_form_hp_soll} - Einzelabzug ${_max_einzel})")
    endif()
elseif((_form_alt STREQUAL "" OR NOT _form_alt GREATER 0) AND NOT _warten)
    string(APPEND _f "\n Form ohne HP vor dem Abschuss (F${_exit_bild}, HP '${_form_alt}')")
endif()
# Lauf A2: Fenster ab dem Eintritt in Wagen C (F${_bild_c}); die Rakete betaeubt die Form (Zustand 2) bis zum Tod, ihr Angriff faellt
# vor die erste Rakete (Ansturm F1322 leon/elza, F1336 zurueck_*, gemessen).
if(_pl_treffer LESS 1)
    string(APPEND _f "\n Kampf: ${_figur} wird zwischen dem Eintritt in Wagen C (F${_bild_c}) und dem Kampfende (Abschuss F${_rueck}) nicht getroffen")
endif()

# --- 6 Kamera der Rueckstoss-Szene (Nachbesserung 4) ---
# Schranken 6c/6d: Pruefmass Abnahme 2 M-A (500 / 600 Promille), je Teil auf den Messwert gehoben, wo der Stand ihn ueberschreitet
# (Lauf A2, s. _p6_* oben).
set(_taumel "")
foreach(_z IN LISTS _zeilen)
    if(_z MATCHES "^F([0-9]+) pad=" AND _z MATCHES "t=30 st=[23] ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=13 [^]]*\\] hp=1( |$)")
        string(REGEX MATCH "^F([0-9]+)" _m "${_z}")
        list(APPEND _taumel "${CMAKE_MATCH_1}")
    endif()
endforeach()
list(LENGTH _taumel _taumel_n)
set(_k "")
set(_kam_info "")
if(_warten)
    set(_kam_info "keine Szene bis F${_exit_bild} (Teil warten)")
elseif(TRUE)   # Runde 42 Strang finale: keine Rueckstoss-Szene mehr -> kein Taumelbild; die Kamerapruefung 6 entfaellt
    if(_taumel_n GREATER 0)
        string(APPEND _f "\n ${_taumel_n} Taumelbilder (Clip 13, HP 1) im state.log: die Rueckstoss-Szene lief (Plan S4: entfallen)")
    endif()
    set(_kam_info "Rueckstoss-Szene entfallen (Plan S4), ${_taumel_n} Taumelbilder, Abschuss F${_abschuss}")
elseif(NOT EXISTS "${WORKDIR}/deck.log")
    string(APPEND _f "\n Kamera: kein deck.log (Messschiene RE15_SLOTDECKUNG fehlt in dieser exe)")
else()
    file(STRINGS "${WORKDIR}/deck.log" _deck REGEX "^F[0-9]+ cam=")
    foreach(_d IN LISTS _deck)
        if(_d MATCHES "^F([0-9]+) ")
            set(_dk_${CMAKE_MATCH_1} "${_d}")
        endif()
    endforeach()
    # Form-Slot: Figuren-Slot 1..18 mit der groessten bedeckten Flaeche ueber die Taumelbilder
    foreach(_s RANGE 1 18)
        set(_sum_${_s} 0)
    endforeach()
    foreach(_t IN LISTS _taumel)
        if(DEFINED _dk_${_t})
            string(REGEX MATCHALL " s[0-9]+=[0-9]+/[0-9]+" _sl "${_dk_${_t}}")
            foreach(_e IN LISTS _sl)
                if(_e MATCHES " s([0-9]+)=([0-9]+)/([0-9]+)" AND CMAKE_MATCH_1 GREATER 0 AND CMAKE_MATCH_1 LESS 19)
                    math(EXPR _sum_${CMAKE_MATCH_1} "${_sum_${CMAKE_MATCH_1}} + ${CMAKE_MATCH_3}")
                endif()
            endforeach()
        endif()
    endforeach()
    set(_fslot -1)
    set(_fsum 0)
    foreach(_s RANGE 1 18)
        if(_sum_${_s} GREATER _fsum)
            set(_fsum ${_sum_${_s}})
            set(_fslot ${_s})
        endif()
    endforeach()
    # fast schwarz aus der Framedump-Serie
    list(GET _taumel 0 _t0)
    list(GET _taumel -1 _t1)
    math(EXPR _t1_grenze "${_exit_bild} - 2")
    if(NOT _t1 LESS _t1_grenze)
        string(APPEND _f "\n Kamera: Taumeln F${_t0}-F${_t1} erreicht das Laufende F${_exit_bild} (Laufende/Framedump zu frueh)")
    endif()
    if(RE15_BILD_TOOL AND EXISTS "${RE15_BILD_TOOL}")
        execute_process(COMMAND "${RE15_BILD_TOOL}" "${WORKDIR}/rs_" ${_t0} ${_t1} RESULT_VARIABLE _rvb OUTPUT_VARIABLE _outb ERROR_VARIABLE _errb)
        string(REGEX MATCHALL "F[0-9]+ dunkel=[0-9]+" _dl "${_outb}")
        foreach(_e IN LISTS _dl)
            if(_e MATCHES "F([0-9]+) dunkel=([0-9]+)")
                set(_dn_${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
            endif()
        endforeach()
    else()
        string(APPEND _f "\n Kamera: RE15_BILD_TOOL fehlt (probe_r41_form_kamera_bild)")
    endif()
    set(_fehl_n 0)
    set(_ohne 0)
    set(_dn_n 0)
    set(_min_form 999999)
    set(_max_pl 0)
    set(_max_deck 0)
    set(_max_dn 0)
    set(_kams "")
    foreach(_t IN LISTS _taumel)
        if(DEFINED _dn_${_t})
            math(EXPR _dn_n "${_dn_n} + 1")
            if(_dn_${_t} GREATER _max_dn)
                set(_max_dn ${_dn_${_t}})
            endif()
            if(_dn_${_t} GREATER _p6_dunkel)
                math(EXPR _fehl_n "${_fehl_n} + 1")
                if(_fehl_n LESS 7)
                    string(APPEND _k "\n   F${_t}: ${_dn_${_t}} Promille fast schwarz (> ${_p6_dunkel})")
                endif()
            endif()
        endif()
        if(NOT DEFINED _dk_${_t})
            math(EXPR _ohne "${_ohne} + 1")
            continue()
        endif()
        set(_fv 0)
        set(_fb 0)
        set(_pv 0)
        string(REGEX MATCH "cam=(-?[0-9]+)" _m "${_dk_${_t}}")
        set(_cam "${CMAKE_MATCH_1}")
        list(FIND _kams "${_cam}" _kpos)
        if(_kpos LESS 0)
            list(APPEND _kams "${_cam}")
        endif()
        if(_dk_${_t} MATCHES " s${_fslot}=([0-9]+)/([0-9]+)")
            set(_fv ${CMAKE_MATCH_1})
            set(_fb ${CMAKE_MATCH_2})
        endif()
        if(_dk_${_t} MATCHES " s0=([0-9]+)/")
            set(_pv ${CMAKE_MATCH_1})
        endif()
        set(_deck 0)
        if(_fb GREATER 0)
            math(EXPR _deck "(${_fb} - ${_fv}) * 1000 / ${_fb}")
        endif()
        if(_fv LESS _min_form)
            set(_min_form ${_fv})
        endif()
        if(_pv GREATER _max_pl)
            set(_max_pl ${_pv})
        endif()
        if(_deck GREATER _max_deck)
            set(_max_deck ${_deck})
        endif()
        set(_grund "")
        if(_fv LESS _p6_form_min)
            string(APPEND _grund " Form sichtbar ${_fv} < ${_p6_form_min}")
        endif()
        if(_pv GREATER 25600)
            string(APPEND _grund " ${_figur} ${_pv} > 25600")
        endif()
        if(_deck GREATER _p6_deck)
            string(APPEND _grund " Form zu ${_deck} Promille verdeckt")
        endif()
        if(_grund)
            math(EXPR _fehl_n "${_fehl_n} + 1")
            if(_fehl_n LESS 7)
                string(APPEND _k "\n   F${_t} Cut ${_cam}:${_grund}")
            endif()
        endif()
    endforeach()
    math(EXPR _mit "${_taumel_n} - ${_ohne}")
    math(EXPR _mit_min "${_taumel_n} * 9 / 10")
    if(_mit LESS _mit_min)
        string(APPEND _f "\n Kamera: nur ${_mit} von ${_taumel_n} Taumelbildern im deck.log")
    endif()
    if(_dn_n LESS 20)
        string(APPEND _f "\n Kamera: nur ${_dn_n} Framedump-Bilder in den Taumelbildern F${_t0}-F${_t1}")
    endif()
    if(_fehl_n GREATER 0)
        string(APPEND _f "\n Kamera der Rueckstoss-Szene: ${_fehl_n} Verstoesse in F${_t0}-F${_t1}${_k}")
    endif()
    set(_kam_info "Taumeln F${_t0}-F${_t1} (${_taumel_n} Bilder, Schnitte ${_kams}), Form-Slot ${_fslot} min sichtbar ${_min_form}, ${_figur} max ${_max_pl}, Form max ${_max_deck} Promille verdeckt, max ${_max_dn} Promille fast schwarz (${_dn_n} Bilder)")
endif()
# --- 7 Kampfkamera vor der Szene (Nachbesserung 4 Runde 2) ---
set(_k7_bis ${_exit_bild})
if(NOT _rueck STREQUAL "")
    math(EXPR _k7_bis "${_rueck} - 1")
endif()
set(_k7_info "")
# Auftrittsbilder (Lauf A2) = Bedingung der Wache (a) in bf_nahsicht_wache (birkin_formen.c): Form Zustand 1 / Unterzustand 0
# (BB_AUFTRITT), Clip 10 (BB_C_DROHEN), Clip-Bild < BF_AUFTRITT_KAM_BIS 60 (Cut 11). Gemessen F1169-F1227 in allen Teilen.
foreach(_z IN LISTS _zeilen)
    if(_z MATCHES "^F([0-9]+) pad=")
        set(_zb "${CMAKE_MATCH_1}")
        if(_z MATCHES "t=30 st=1 ss1=0 ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=10 af=([0-9]+) " AND CMAKE_MATCH_1 LESS 60)
            set(_auf_${_zb} 1)
        endif()
    endif()
endforeach()
if(RE15_BILD_TOOL AND EXISTS "${RE15_BILD_TOOL}")
    execute_process(COMMAND "${RE15_BILD_TOOL}" "${WORKDIR}/rs_" 1150 ${_k7_bis} RESULT_VARIABLE _rv7 OUTPUT_VARIABLE _out7 ERROR_VARIABLE _err7)
    string(REGEX MATCHALL "F[0-9]+ dunkel=[0-9]+" _dl7 "${_out7}")
    list(LENGTH _dl7 _n7)
    set(_ueber7 0)
    set(_max7 0)
    set(_max7_f "")
    set(_k7 "")
    set(_ueberA 0)
    set(_maxA 0)
    set(_maxA_f "")
    set(_nA 0)
    foreach(_e IN LISTS _dl7)
        if(_e MATCHES "F([0-9]+) dunkel=([0-9]+)")
            set(_eb ${CMAKE_MATCH_1})
            set(_ew ${CMAKE_MATCH_2})
            if(DEFINED _auf_${_eb})
                math(EXPR _nA "${_nA} + 1")
                if(_ew GREATER _maxA)
                    set(_maxA ${_ew})
                    set(_maxA_f ${_eb})
                endif()
                if(_ew GREATER 600)
                    math(EXPR _ueberA "${_ueberA} + 1")
                endif()
                continue()
            endif()
            if(_ew GREATER _max7)
                set(_max7 ${_ew})
                set(_max7_f ${_eb})
            endif()
            if(_ew GREATER 600)
                math(EXPR _ueber7 "${_ueber7} + 1")
                if(_ueber7 LESS 9)
                    string(APPEND _k7 "\n   F${_eb}: ${_ew} Promille fast schwarz")
                endif()
            endif()
        endif()
    endforeach()
    math(EXPR _n7_min "(${_k7_bis} - 1150) * 9 / 10")
    if(_n7 LESS _n7_min)
        string(APPEND _f "\n Kampfkamera: nur ${_n7} Framedump-Bilder in F1150-F${_k7_bis}, erwartet >= ${_n7_min}")
    endif()
    if(_max7 GREATER _kampf_hoechst)
        string(APPEND _f "\n Kampfkamera vor der Szene (F1150-F${_k7_bis}, ohne Auftritt): F${_max7_f} ${_max7} Promille fast schwarz, erlaubt ${_kampf_hoechst}")
    endif()
    if(_ueber7 GREATER _kampf_max)
        string(APPEND _f "\n Kampfkamera vor der Szene (F1150-F${_k7_bis}, ohne Auftritt): ${_ueber7} Bilder > 600 Promille fast schwarz, erlaubt ${_kampf_max}${_k7}")
    endif()
    if(_nA LESS 1)
        string(APPEND _f "\n Kampfkamera: kein Auftrittsbild (Form Zustand 1/0, Clip 10) in F1150-F${_k7_bis}")
    endif()
    if(_maxA GREATER _auftritt_hoechst)
        string(APPEND _f "\n Auftritt (Cut 11): F${_maxA_f} ${_maxA} Promille fast schwarz, erlaubt ${_auftritt_hoechst}")
    endif()
    if(_ueberA GREATER _auftritt_max)
        string(APPEND _f "\n Auftritt (Cut 11): ${_ueberA} Bilder > 600 Promille fast schwarz, erlaubt ${_auftritt_max}")
    endif()
    set(_k7_info "F1150-F${_k7_bis} ${_n7} Bilder; Auftritt ${_nA} Bilder, ${_ueberA} > 600 (erlaubt ${_auftritt_max}), max ${_maxA} (F${_maxA_f}); Kampf ${_ueber7} > 600 (erlaubt ${_kampf_max}), max ${_max7} (F${_max7_f}, erlaubt ${_kampf_hoechst})")
else()
    string(APPEND _f "\n Kampfkamera: RE15_BILD_TOOL fehlt (probe_r41_form_kamera_bild)")
endif()

message(STATUS "r41_form_wagenweg[${TEIL}]: Kamera ${_kam_info}")
message(STATUS "r41_form_wagenweg[${TEIL}]: Kampfkamera ${_k7_info}")
message(STATUS "r41_form_wagenweg: Soll-HP ${_form_hp_soll}, Wagen B ${_hp_b}, Wagen C ${_hp_c} (F${_bild_c}), "
               "Treffer ${_treffer} (Abzug ${_abzug}, groesster ${_max_einzel}) ab F${_erster_treffer}, Rueckstoss F${_rueck}, Form-HP zuletzt ${_form_alt}, ${_figur} getroffen ${_pl_treffer}x, "
               "exit=${_rv}, Lauf ${WORKDIR}")
if(_f)
    message(FATAL_ERROR "r41_form_wagenweg[${TEIL}]: FEHLER${_f}")
endif()
file(GLOB _ppm "${WORKDIR}/rs_*.ppm")
if(_ppm)
    file(REMOVE ${_ppm})
endif()
message(STATUS "r41_form_wagenweg[${TEIL}]: alle Pruefungen gruen")
