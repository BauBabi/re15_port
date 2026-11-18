# Runde 42 Strang rest, Stufe 6 - vier Vordergrund-Masken aus RE2 (STAGE4-6, gleiches Hintergrundbild). Kopf von
# tests/unit/test_r42_pri_re2.c, Daten tools/maske/re2_uebernahme_r42.py, Lader bg_pc.c re15_pri_re2_zusatz.
#   unit_r42_pri_re2_daten      Zusatzmasken == RE2-Masken (src/Groesse gleich, dst + Bildversatz, Tiefe ceil(d/2)-1), Atlas im RE2-Layout,
#                               Leon == Elza, Basis-Sektion des Cuts wie erwartet
#   unit_r42_pri_re2_farbe      Atlasfarbe == RE1.5-Bild an der Bildstelle (Bildversatz ROOM5100 K7 noetig)
#   unit_r42_pri_re2_anhaengen  Kette des Zeichners: Basis unveraendert vorn, Zusatz dahinter, srcY + Atlas-Versatz
#   integration_r42_pri_re2     echte exe, Leon und Elza: Figur HINTER dem Gegenstand verdeckt (>= Haelfte), Figur DAVOR 0 Punkte uebermalt
#                               (Szenen tests/integration/test_r42_pri_re2_szenen.cmake, Laeufer test_r41_masken36_stage2.cmake)
add_executable(test_r42_pri_re2 ${CMAKE_CURRENT_LIST_DIR}/../test_r42_pri_re2.c)
target_link_libraries(test_r42_pri_re2 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_pri_re2 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_pri_re2 PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
                                                  RE15_RE2_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon")
if(NOT WIN32)
    target_link_libraries(test_r42_pri_re2 PRIVATE m)
endif()
foreach(_teil daten farbe anhaengen)
    add_test(NAME unit_r42_pri_re2_${_teil} COMMAND test_r42_pri_re2 ${_teil} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_pri_re2_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
if(TARGET re15_pc)
    add_test(NAME integration_r42_pri_re2
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PPM_TOOL=$<TARGET_FILE:probe_r37_masken36_ppm>
                     -DRE15_BG_TOOL=$<TARGET_FILE:probe_bg_dump>
                     -DCD_ROOT=${CMAKE_SOURCE_DIR}/shared_assets/PSX
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r42_pri_re2_wd
                     -DSZENEN=${CMAKE_SOURCE_DIR}/tests/integration/test_r42_pri_re2_szenen.cmake
                     -DPRAEFIX=r42pre2
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_masken36_stage2.cmake)
    set_tests_properties(integration_r42_pri_re2 PROPERTIES TIMEOUT 2400)
endif()
