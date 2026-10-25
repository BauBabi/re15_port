# Runde 41 (2026-10-24), Spur M "masken36" — Riegel, zaehlen zur Suite. Dossier: analysis/befunde_runde41/M_masken36.md.
#
# Punkt 1 (AUFTRAG.md): "Die von dir erstellten Masken fuer STAGE3-6 sind relativ unbrauchbar, weil die ueberall Loecher
# aufweisen." -> "danach kannst du deine Masken schliessen in der naechsten Version."
#   unit_r41_masken36_loecher  Mechanik des Schliessens an allen 216 Generator-Sektionen (test_r41_masken36.c): eigener Atlas,
#                             den der Lader nimmt; offene Kerben (Kuenstler-Atlas deckend + farbgleich direkt an der Kante
#                             eines alten Rechtecks, nicht gezeichnet) NACHHER <= 30 % von VORHER (gemessen nach NB3 14,3 %, NB2 16,0 %, NB1 18,4 %, vor NB1 25,1 %), Gegenprobe VORHER > 10000; Blit.
add_executable(test_r41_masken36 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_masken36.c)
target_link_libraries(test_r41_masken36 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_masken36 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_masken36 PRIVATE RE15_PORT_SRC_DIR="${CMAKE_SOURCE_DIR}")
if(NOT WIN32)
    target_link_libraries(test_r41_masken36 PRIVATE m)
endif()
add_test(NAME unit_r41_masken36_loecher COMMAND test_r41_masken36 loecher WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_loecher PROPERTIES TIMEOUT 600)
# Punkt 2 (AUFTRAG.md): "Ich habe diverse neue PRIs fuer STAGE2 erstellt. die sollen alle sauber eingebaut werden!"
#   unit_r41_masken36_kurbel  ROOM2020 Cut 3 an der Kurbel (Pos_set @0x0A8C (3650,-1800,-2925)): die Sichtlinie Kamera ->
#                             Leon kreuzt die solide Zelle der Suedreling (Leon steht HINTER ihr), das Original hat in Cut 3
#                             keine Masken (NULL-Sektion), die Reling-Maske verdeckt die Beine nach dem Original-Urteil.
add_test(NAME unit_r41_masken36_kurbel COMMAND test_r41_masken36 kurbel WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
#   integration_r41_masken36_stage2  echte exe (Leon und Elza): ROOM2020 Cut 3 an der Kurbel - die Reling-Maske verdeckt die
#                             Beine (Leon steht HINTER der Reling); an der strengsten VOR-Stelle 0 Figurpunkte uebermalt,
#                             an der HINTER-Stelle >= Haelfte verdeckt (11A0, 2030, 2040, 2050, 2080); C = probe_bg_dump.
if(TARGET re15_pc)
    add_test(NAME integration_r41_masken36_stage2
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                     -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_masken36_s2_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
    set_tests_properties(integration_r41_masken36_stage2 PROPERTIES TIMEOUT 2400)
endif()

# Nachbesserung 1 (Abnahme 0, Mangel 1): ausgeschlossene Generator-Objekte der Stages 3..6 mit der Tiefe ihrer Kollisionszelle
#   integration_r41_masken36_zelle  echte exe (Leon): je Objekt eine HINTER-Stelle (die wieder aufgenommenen Rechtecke verdecken
#                             Leon: H >= Haelfte bzw. H > 0) und eine DAVOR-Stelle (Leon ueberlappt sie, 0 Punkte uebermalt);
#                             Szenen test_r41_masken36_zelle_szenen.cmake, Laeufer wie integration_r41_masken36_stage2.
if(TARGET re15_pc)
    add_test(NAME integration_r41_masken36_zelle
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                     -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_masken36_zelle_wd
                     -DSZENEN=${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_zelle_szenen.cmake
                     -DPRAEFIX=r41m3
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
    set_tests_properties(integration_r41_masken36_zelle PROPERTIES TIMEOUT 2400)
endif()

# Nachbesserung 2 (Abnahme 1, Mangel 1, Weg (a)): Generator-Rechtecke mit Zellen-Modell (tests/unit/gen_r41_masken36_zellen.inc,
# tools/maske/zellen3d_r41.py) - der Ausschluss urteilt fuer sie in 3D (Koerper der Modellzelle vor dem Figurpunkt).
#   unit_r41_masken36_zellen3d  jede Modell-Zeile byte-genau im RDT, MSK-Tiefe == Zeile, Engine zeichnet sie; Ausschluss MIT 3D-Urteil
#                             == Engine-Tabelle in jedem Modell-Cut, Gegenprobe OHNE 3D-Urteil: >= 127 Rechtecke fielen weg (gemessen NB3 127, NB2 79),
#                             alle mit Modell-Zeile. Modelle auch aus der Zuordnung von Hand (tools/maske/zuordnung_r41.json, 12 Objekte).
add_test(NAME unit_r41_masken36_zellen3d COMMAND test_r37_masken36 zellen3d WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_zellen3d PROPERTIES TIMEOUT 1500)
#   integration_r41_masken36_zellen3d  echte exe: die in NB2 gezeichneten Objekte (ROOM6000 Lok/Traeger, 3040 C0, X-Kiste 5020 C6,
#                             Treppenraum 5020 C0, Trennwand 30A0 C1, Kistenstapel 5090 C13)
#                             verdecken Leon HINTER ihnen (H >= Haelfte bzw. H > 0) und uebermalen ihn DAVOR nicht (H == 0);
#                             Szenen test_r41_masken36_zellen3d_szenen.cmake, Laeufer wie integration_r41_masken36_stage2.
#                             NB3: die NB2-"DAVOR"-Plaetze 6000 C5 im Gang hinter der Ecke des Wandblocks Zelle 12 @0x007EC sind
#                             HINTER-Stellen (Art M, Beleg in der Szenendatei); DAVOR Cut 5 = Abnahme-2-Stelle (12233,-9984) Band 2.
if(TARGET re15_pc)
    add_test(NAME integration_r41_masken36_zellen3d
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                     -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_masken36_zellen3d_wd
                     -DSZENEN=${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_zellen3d_szenen.cmake
                     -DPRAEFIX=r41m4
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
    set_tests_properties(integration_r41_masken36_zellen3d PROPERTIES TIMEOUT 2400)
endif()

# Nachbesserung 3 (Abnahme 2, Mangel 1 "zu ferne Tiefe", Mangel 2 Restliste/OFFEN 9): Koerper in Teilrechtecke (tools/maske/kacheln_r41.py,
# Koerper-Form tools/maske/koerper_r41.py = Formen des Resolvers inkl. Raute Typ 2 push_diag2, Zuordnung von Hand tools/maske/zuordnung_r41.json).
#   unit_r41_masken36_fern  Mass "zu fern" (test_r37_masken36 fern) ueber alle Generator-Sektionen mit Modell der Stages 3..6, Leon und Elza:
#                             erreichbare Figurpunkte HINTER dem Koerper einer gezeichneten Modell-Maske, die keine gezeichnete Maske verdeckt
#                             (re15_pri_mask_occludes) - gemessen nach NB3 40 276 (vor NB3 1 360 606 = Gegenprobe), je Stage verlangt
#                             Stage 3 == 0 (vorher 56 346, darunter die Trennwand 30A0), Stage 4 <= 72 (4080 C7, zwei Plaetze an der
#                             Zellkante), Stage 5 <= 270 (5090 C11, Randfall an der Dreieckszelle), Stage 6 <= 39 934 (alles ROOM6000 Band 0
#                             = Gleisbett unter dem Bahnsteig, OFFEN 10).
# NB4: die 39 934 Punkte der Stage 6 liegen auf Band 0 im Gleisbett ROOM6000 - Saaten aus Tueren mit falsch gelesenem Band (AOT-Scan
# @0x80042cac..0x80042ccc gated auf pc[4]); mit der Korrektur (test_r37_masken36 --saat-tuer) waeren es 0. Vorgabe bleibt Stand NB3
# (Entscheid M-E1, Dossier NB4 Mangel 2 (a)).
add_test(NAME unit_r41_masken36_fern COMMAND test_r37_masken36 fern 40276 0 72 270 39934 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_fern PROPERTIES TIMEOUT 1500)
#   integration_r41_masken36_fern  echte exe (Leon und Elza): hinter der Buero-Trennwand ROOM30A0 Cut 0/1 an den Spielstellen der Abnahme 2
#                             im Fenster der Trennwand KEIN Figurpunkt im Endbild sichtbar (Art Z, V == 0, probe_r41_masken36_sicht mit
#                             Masken-Bild ohne Figur), davor 0 uebermalt (Art V); Szenen test_r41_masken36_fern_szenen.cmake.
add_executable(probe_r41_masken36_sicht ${CMAKE_CURRENT_LIST_DIR}/../probe_r41_masken36_sicht.c)
if(TARGET re15_pc)
    add_test(NAME integration_r41_masken36_fern
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DRE15_SICHT_TOOL=$<TARGET_FILE:probe_r41_masken36_sicht>
                     -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                     -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_masken36_fern_wd
                     -DSZENEN=${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_fern_szenen.cmake
                     -DPRAEFIX=r41m5
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
    set_tests_properties(integration_r41_masken36_fern PROPERTIES TIMEOUT 2400)
endif()

# Nachbesserung 4 (Abnahme 3, Mangel 1 "Ost-Stirn der Trennwand 30A0/30A1 Cut 0 ohne Maske"):
#   unit_r41_masken36_stirn  die Stirnseite IM BILD (Kuenstler-Ausschnitt des SLD-Atlas, Kante in der Ebene x = -19975 bei z 14834..14904)
#                             ist gezeichnet (0 offene Kerben im Fenster x 282..319) und KEINE Maske liegt ueber der Zellenflaeche
#                             jenseits der Kante (z > 14950 bis 15375, Zelle 6 @RDT+0x0045C) - dort zeigt das Bild den Boden hinter der
#                             Figur; Szenen z30A0_*/v30A0_*/v30A1_* im Riegel integration_r41_masken36_fern.
add_test(NAME unit_r41_masken36_stirn COMMAND test_r41_masken36 stirn WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_stirn PROPERTIES TIMEOUT 600)

# Nachbesserung 4 (Abnahme 3, Mangel 3): der gemeinsame Laeufer test_r41_masken36_stage2.cmake (stage2/_zelle/_zellen3d/_fern) wertet
# nur noch Laeufe aus, deren debug.log nach dem letzten Laden des Zielraums den Framedump und "[flow] EXIT_AT: Bild 120 in Raum <zr>"
# zeigt (RE15_FRAMEDUMP=120 schreibt sonst schon im Boot-Raum; gemessen Abnahme 3: d4040_09 verglich das Boot-Bild), sonst bis zu
# 3 Versuche, dann Fehler (TIMEOUT der vier Riegel deshalb 2400 s).
#   integration_r41_masken36_laufpruefung  Gegenprobe an der echten exe: Lauf mit RE15_EXIT_AT=60 im Zielraum (f.ppm = Boot-Bild 120 liegt
#                             da) wird abgelehnt, der volle Lauf derselben Szene angenommen ("Gegenprobe OK" Pflicht; Laeufer -DGEGENPROBE=1).
if(TARGET re15_pc)
    add_test(NAME integration_r41_masken36_laufpruefung
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DRE15_SICHT_TOOL=$<TARGET_FILE:probe_r41_masken36_sicht>
                     -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                     -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_masken36_laufpruefung_wd
                     -DSZENEN=${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_fern_szenen.cmake
                     -DPRAEFIX=r41m6
                     -DGEGENPROBE=1
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
    set_tests_properties(integration_r41_masken36_laufpruefung PROPERTIES TIMEOUT 900 PASS_REGULAR_EXPRESSION "Gegenprobe OK")
endif()

# Nachbesserung 5 (Abnahme 4, Mangel 1 = Entscheid M-E2 "Teile des Original-Vordergrundatlas einsetzen, sofern je Teil Lage
# (Bildvergleich) und Tiefe (Zelle mit RDT-Offset) belegt sind"): tools/maske/platzieren_r41.py (Regel 5/6 NB5), Tabelle
# tests/unit/gen_r41_masken36_platziert.inc, Modell-Zeilen in tests/unit/gen_r41_masken36_zellen.inc.
#   unit_r41_masken36_platziert  je Sektion: Rechteckzahl erstes + n, Leon/Elza bytegleich; neue Rechtecke = Hintergrund an der Zielstelle
#                             (Blit); jedes Teil liegt dort, wo der Original-Atlas (Spiel-Entpacker re15_sld_*) es hat (>= 95 % der Texel
#                             farbgleich); gezeichnet (nach dem Ausschluss) mindestens der gemessene Endstand; die Gelaender der Restliste
#                             (30E0 C6..C9, 3050 C6, 3040 C3) sind dabei.
add_test(NAME unit_r41_masken36_platziert COMMAND test_r41_masken36 platziert WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_platziert PROPERTIES TIMEOUT 900)
#   unit_r41_masken36_teile  Mechanik "Tiefe aus der Zelle": jedes gezeichnete neue Rechteck hat eine Modell-Zeile (Zelle + 12 SCA-Bytes am
#                             RDT-Offset, Tiefe == Zeile); an keinem deckenden Punkt liegt der Eintritt des Sehstrahls in den Koerper hinter
#                             der Rechteck-Tiefe (unabhaengig in C nachgerechnet: Halbebenen der Resolver-Form, bucket = (1023*vz)>>16,
#                             @0x8002565c / ZSF3 341 @0x80066c70 / Masken-OT @0x80039650) - keine Figur VOR dem Gegenstand wird verdeckt.
add_test(NAME unit_r41_masken36_teile COMMAND test_r37_masken36 teile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_teile PROPERTIES TIMEOUT 900)
#   integration_r41_masken36_teile_a..d  echte exe: je Kamera mit eingesetzten Original-Atlas-Teilen eine HINTER-Stelle (Art M: die neuen
#                             Rechtecke verdecken die Figur, H > 0) und eine DAVOR-Stelle (Art V: Figur ueberlappt die neuen Teile davor,
#                             0 Punkte uebermalt), Leon; Elza je Raum die staerkste HINTER-/DAVOR-Stelle. Stellen = test_r37_masken36 neustellen
#                             (dieselben Stehplaetze/Figur/Tabelle wie der Ausschluss). Gruppen: a = ROOM3040, b = 3050 + 4040, c = 30A0/30E0/
#                             Stage 5, d = Stage 6; Szenen test_r41_masken36_teile_szenen_<g>.cmake, Laeufer wie integration_r41_masken36_stage2.
if(TARGET re15_pc)
    foreach(_g a b c d)
        add_test(NAME integration_r41_masken36_teile_${_g}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                         -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                         -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_masken36_teile_${_g}_wd
                         -DSZENEN=${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_teile_szenen_${_g}.cmake
                         -DPRAEFIX=r41m8${_g}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
        set_tests_properties(integration_r41_masken36_teile_${_g} PROPERTIES TIMEOUT 2400)
    endforeach()
endif()

# Nachbesserung 5 (Abnahme 4, Mangel 2 "roter Ordner ROOM30A0/30A1 Cut 0 ohne Maske"): der Ordner ist KEIN Teil des Original-Vordergrundatlas
# (der Kuenstler-Ausschnitt deckt den gruenen Ordner daneben, den roten nicht; das Original hat in 30A0/30A1 gar keine Masken) -> nach M-E2 /
# M-E1 keine automatische Maske, Freistellung des Nutzers (Dossier "Fuer den Nutzer: Freistellungen").
#   unit_r41_masken36_ordner  pri_offset Cut 0 -> ff ff ff ff + Kopfbyte 7 == 0 (beide Varianten); Atlas (Spiel-Entpacker) an der Verschiebung
#                             des gezeichneten Rechtecks unter dem gruenen Ordner: gruen >= 91 Bildpunkte farbgleich, rot 0; kein eingesetztes
#                             Teil auf dem roten Fenster; gezeichnet dort 0 (Leon und Elza).
add_test(NAME unit_r41_masken36_ordner COMMAND test_r41_masken36 ordner WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r41_masken36_ordner PROPERTIES TIMEOUT 600)
