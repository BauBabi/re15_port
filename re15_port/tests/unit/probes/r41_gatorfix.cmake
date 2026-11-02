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
foreach(_teil montage montage_elza marvin marvin_elza schluessel schluessel_elza klemm klemm_elza fuzz fuzz_elza)
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
        set_tests_properties(integration_r41_gatorfix_${_teil} PROPERTIES TIMEOUT 900)
    endforeach()
endif()
