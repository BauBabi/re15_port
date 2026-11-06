# Runde 41 Spur G2 "gatorfix" - Riegel und Werkzeuge. Dossier: analysis/befunde_runde41/G2_gatorfix.md.
add_executable(test_r41_gatorfix ${CMAKE_CURRENT_LIST_DIR}/../test_r41_gatorfix.c)
target_link_libraries(test_r41_gatorfix PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_gatorfix PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(test_r41_gatorfix PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r41_gatorfix PRIVATE m)
endif()
#   (NB4 M4: im Rollfenster ohne Ausnahme Kiefer- + waagerechter Rahmen, je Bild <= erreichbares Minimum + 4, ausserhalb 0; Gegenprobe Stand vor NB4)
#   unit_r41_gatorfix_maul(_elza)  Punkt 1: in allen Haltebildern (Koppelbild + 26 .. Wurf, Fang .. weg) kein Leichen-Scheitel ueber dem Schaedel
#                                  (Part 7) oder unter dem Unterkiefer (Part 6), Laengsfenster +-150; Gegenprobe Leiche wie vor G2 (Pruefhaken)
#   unit_r41_gatorfix_wurf(_elza)  Punkt 2: Wurf Clip 5 Bild 57, hochgeworfen (>= H/2 ueber der Maullage), Fang Bild 75 im Maul (<= 40), weg Bild 77
#                                  samt Schluessel; Gegenprobe Leiche wie vor G2: kein Wurf
foreach(_teil maul maul_elza wurf wurf_elza)
    add_test(NAME unit_r41_gatorfix_${_teil} COMMAND test_r41_gatorfix ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_gatorfix_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
#   unit_r41_gatorfix_montage(_elza)  Punkt 3: fuenf Zombies in der Montage, Blick zur Leiter (Gang: integration_r41_gatorfix_gang_montage*)
#   unit_r41_gatorfix_marvin(_elza)   Punkte 4 + 5: drei Zombies, Blick zur Leiter, alle erschossen <= 90 Bilder, beim Tod >= 2000 von der Leiter;
#                                     Musik-Eintrag 0xFF3C in 2080 waehrend der Szene und zurueck; Gegenprobe 2080 ohne Szene = Raumtabelle
#   unit_r41_gatorfix_schluessel(_elza)  Punkt 6: gezeichnetes Modell 270 statt 180 (RE2-MD1 bytegleich), Bildkasten Ankunft Cut 0 und nach dem Tod
#                                     >= 1,8 x die Flaeche mit dem RE2-Modell (Gegenprobe im selben Lauf)
#   unit_r41_gatorfix_klemm(_elza)   Punkt 7: mit dem Kanister im Maul greift die Kopf-Klemme am Steuerziel nicht (Lage der Nutzer-Marke 1 + drei Waende);
#                                     Gegenprobe Ziel wie vor G2 (Pruefhaken re15_gr2_kk_ziel_alt_setzen): Ueberstand 340 = Festdruecken
#   integration_r41_gatorfix_gang_*   exe: Gang zur Leiter (Montage: alle fuenf >= 1000 naeher; Szene: naeher beim Tod, >= 2000 Rest, letzter Schuss <= F90)
#   unit_r41_gatorfix_fuzz(_elza)   NB1 (M1/M3): Fuzz ueber 26 Spielerlagen (Waende, Ecken, Plattform, Rampe, Leiter) mit dem Kanister im Maul -
#                                     Wandreiben <= 30 Bilder, Stillstand hoechstens wie vor G2; Gegenprobe G2 vor NB1 (Pruefhaken): Wandreiben > 30
#   unit_r41_gatorfix_kamera(_elza)  NB4 Punkt 2: mit Kanister - je Lage (Rampe, Leiter; die Plattform ist nicht betretbar) und Kamera der Figur (RVD-Zonen
#                                     entlang des Wegs von Sueden/Norden) kreist er zu >= 70 % im Bild, wo die Kamera sichtbares Wasser zeigt; Gegenprobe NB3
#   unit_r41_gatorfix_haken(_elza)   NB5 (Abnahme 3, Haken): nach dem Raumaufbau des naechsten Raums staucht der Leichen-Haken keinen Prop mehr;
#                                     Gegenprobe Pruefhaken _fit_reset_alt_setzen(1): Haken bleibt an
#   unit_r41_gatorfix_bahn(_elza)    NB5 (Abnahme 3 M2a/M2b): Belagerung mit Kanister je Rampenlage und Kamera - die GEFAHRENE Bahn folgt dem Plan (Abstand,
#                                     Umlaufzeit, Schwerpunkt) und der Alligator ist im Bild erkennbar (Silhouette >= 50 % der Verfolgung an der Leiter in Cut 0);
#                                     Gegenprobe Stand NB4 (Kreis R 2000, gespiegelte Karotte): rot
#   unit_r41_gatorfix_re2(_elza)     G2-RE2 (Neuaufbau ab NB5, Dossier "G2-RE2"): echter Boss-Takt mit dem Kanister, 13 Lagen (Abnahme-4-Haenger, Rampe,
#                                     Leiter, Wege) x 1800 Bilder, Stand NB5 (Pruefhaken re15_gr2_rw_alt_setzen) gegen neu: Haengen, Drehen, Kopf/Kanister in
#                                     Rampe/Plattform/vor der Kamera, Rammen, Tauch-Obergrenze, rueckwaerts, Gang = authored RE2-Vortrieb; Gegenproben NB5 rot
#   unit_r41_gatorfix_re2_gegen(_elza) Gegenproben der Bausteine: RE1.5-Tempo (Gang weicht ab), ohne Rammen (0 Rammen)
#   unit_r41_gatorfix_gang(_elza)    ohne Kanister Bild fuer Bild gleich NB5; Gegenprobe RE1.5-Tempo auch ohne Kanister
#   (bahn/kamera = NB5-Belagerung mit dem Kanister: entfallen, mit dem Kanister ueberquert er die Rampe - Werkzeuge bleiben)
#   unit_r41_gatorfix_v4(_elza)      G2-RE2 NB2 "Vorschlag 4": (a) Bewegung mit Kanister Bild fuer Bild = ohne Kanister (13 Lagen, Gegenprobe Kanister-Huelle aktiv =
#                                     Stand f942f56c0: rot), (b) Kanister = Part 6 der gezeichneten Pose x Lokallage in jedem Bild (Gegenprobe ein Bild alte Pose: rot)
#   unit_r41_gatorfix_re2(_elza)     seit NB2: Kopf-Riegel (Mesh 5/6/7) Stand f942f56c0 gegen neu; Kopf + Kanister in Rampe/Plattform nur dokumentiert
foreach(_teil montage montage_elza marvin marvin_elza schluessel schluessel_elza klemm klemm_elza fuzz fuzz_elza haken haken_elza re2 re2_elza re2_gegen re2_gegen_elza gang gang_elza v4 v4_elza)
    add_test(NAME unit_r41_gatorfix_${_teil} COMMAND test_r41_gatorfix ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_gatorfix_${_teil} PROPERTIES TIMEOUT 600)
endforeach()
if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_gatorfix.cmake)
    foreach(_teil gang_montage gang_montage_elza gang_szene gang_szene_elza frei frei_elza rampe rampe_elza
                  abn2_leiterecke abn2_wand_elza abn2_nordbecken abn2_rampe_west abn2_rampe_west_elza abn2_ecken abn2_wand)   # NB1/NB2
        add_test(NAME integration_r41_gatorfix_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_TOOL=$<TARGET_FILE:test_r41_gatorfix>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_gatorfix_${_teil}_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_gatorfix.cmake)
        set_tests_properties(integration_r41_gatorfix_${_teil} PROPERTIES TIMEOUT 2400)   # G2-RE2 NB2: frei/rampe/abn2_* laufen zweimal (mit / ohne Kanister, tragvergleich)
    endforeach()
endif()
