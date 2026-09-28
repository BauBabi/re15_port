# Runde 30 / Thema H — MESS-VARIANTE des Spiels fuer die Sicherung im Hebetisch.
#
# Dossier: analysis/befunde_runde30/sicherung.md
#
# ⛔ STANDARD = AUS. Ohne -DRE15_R30_SICHERUNG_VARIANTE=ON legt diese Datei NICHTS an — weder
# ein Ziel noch einen Test. Sie kostet die uebrigen Build-Verzeichnisse (re15_port/build und die
# der parallel arbeitenden Agenten) also keine Sekunde und kann dort nichts brechen.
#
# Mit dem Schalter entsteht `re15_pc_r30_sicherung`: dasselbe Spiel wie `re15_pc` (dieselben
# Quellen, Definitionen, Include-Pfade, Link-Optionen — alles vom Ziel re15_pc ABGELESEN, nicht
# abgeschrieben), nur dass die vier Symbole aus engine/src/sicherung_1150.c durch
# tests/unit/probe_r30_sicherung_variante.c ersetzt sind. Das Objekt steht vor dem statischen
# Archiv re15_engine, das Archivmitglied wird deshalb nicht gezogen (kein Doppelsymbol).
# engine/, platform/ und include/ bleiben unveraendert.
#
#   cmake -S re15_port -B re15_port/build_r30_sicherung ... -DRE15_R30_SICHERUNG_VARIANTE=ON
#   cmake --build re15_port/build_r30_sicherung --target re15_pc_r30_sicherung
option(RE15_R30_SICHERUNG_VARIANTE
       "Runde 30: Mess-Variante des Spiels mit einstellbarem Sitz/Modell der Sicherung" OFF)

if(RE15_R30_SICHERUNG_VARIANTE AND TARGET re15_pc)
    get_target_property(_r30s_src_dir re15_pc SOURCE_DIR)
    get_target_property(_r30s_srcs    re15_pc SOURCES)
    set(_r30s_abs "")
    foreach(_s IN LISTS _r30s_srcs)
        if(IS_ABSOLUTE "${_s}")
            list(APPEND _r30s_abs "${_s}")
        else()
            list(APPEND _r30s_abs "${_r30s_src_dir}/${_s}")
        endif()
    endforeach()

    add_executable(re15_pc_r30_sicherung
        ${_r30s_abs}
        ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_sicherung_variante.c)

    foreach(_p COMPILE_DEFINITIONS INCLUDE_DIRECTORIES LINK_LIBRARIES LINK_OPTIONS COMPILE_OPTIONS)
        get_target_property(_v re15_pc ${_p})
        if(_v)
            set_target_properties(re15_pc_r30_sicherung PROPERTIES ${_p} "${_v}")
        endif()
    endforeach()
    get_target_property(_r30s_win re15_pc WIN32_EXECUTABLE)
    if(_r30s_win)
        set_target_properties(re15_pc_r30_sicherung PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
    # gen/sicherung_prop.inc liegt unter engine/src
    target_include_directories(re15_pc_r30_sicherung PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/engine/src
        ${_r30s_src_dir}/src)
endif()

# Fortsetzungs-Agent: reine MESS-Sonde (druckt den Zeitplan von sub04 in ROOM1150 UND 1151).
# Ebenfalls nur mit dem Schalter, KEIN add_test — die Suite-Zahl bleibt unberuehrt.
if(RE15_R30_SICHERUNG_VARIANTE)
    add_executable(probe_r30_sicherung_fahrt
        ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_sicherung_fahrt.c)
    target_link_libraries(probe_r30_sicherung_fahrt PRIVATE re15_engine re15_test_support)
    target_include_directories(probe_r30_sicherung_fahrt PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()

# =============================================================================
# BAU (Runde 30, Thema H) — die Riegel zum Umbau. Anders als die Mess-Variante oben sind
# sie IMMER an (kein Schalter) und zaehlen zur Suite.
#
#   unit_r30_sicherung_modell   Welt-Modell: bytegleich mit soll/sicherung_zord_normal.md1,
#                               40 Vierecke in Z-Ordnung (FUN_800256b0, Primitiv 0x3C),
#                               160 Eck-Normalen nach aussen
#   unit_r30_sicherung_sitz     Sitz/Drehung gegen den Fachboden aus ROOM1150.RDT UND
#                               ROOM1151.RDT (Prop 0, Vierecke 79-81), Deckelweg 150
#   unit_r30_sicherung_bild     Item-Bild/Icon: bytegleich mit soll/weg2n_*; Einsetzen
#                               veraendert nur 0xC0000..0xC2FFF bzw. 0x12C00..0x130AF
#   integration_r30_sicherung_laden   echte exe, CONTINUE in ROOM1150: die Sicherung liegt
#                               im Pool und wird gezeichnet ([prop-render] pi=4 oid=0x04)
# =============================================================================
add_executable(probe_r30_sicherung_modell
    ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_sicherung_modell.c)
target_link_libraries(probe_r30_sicherung_modell PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_sicherung_modell PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_sicherung_modell PRIVATE
    RE15_R30_SOLL_DIR=${CMAKE_SOURCE_DIR}/../analysis/befunde_runde30/sicherung_werkzeug/soll)
add_test(NAME unit_r30_sicherung_modell COMMAND probe_r30_sicherung_modell)
set_tests_properties(unit_r30_sicherung_modell PROPERTIES TIMEOUT 30)

add_executable(probe_r30_sicherung_sitz
    ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_sicherung_sitz.c)
target_link_libraries(probe_r30_sicherung_sitz PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_sicherung_sitz PRIVATE ${CMAKE_SOURCE_DIR}/include)
# RE15_ASSETS_PATH kommt als PUBLIC-Definition von re15_engine (engine/CMakeLists.txt)
add_test(NAME unit_r30_sicherung_sitz COMMAND probe_r30_sicherung_sitz)
set_tests_properties(unit_r30_sicherung_sitz PROPERTIES TIMEOUT 30)

add_executable(probe_r30_sicherung_bild
    ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_sicherung_bild.c)
target_link_libraries(probe_r30_sicherung_bild PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_sicherung_bild PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_r30_sicherung_bild COMMAND probe_r30_sicherung_bild)
set_tests_properties(unit_r30_sicherung_bild PROPERTIES TIMEOUT 30)

# MESS-WERKZEUG (kein add_test): schreibt die Speicherkarte fuer den Lade-Pin.
add_executable(probe_r30_sicherung_karte
    ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_sicherung_karte.c)
target_link_libraries(probe_r30_sicherung_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_sicherung_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(TARGET re15_pc)
    add_test(NAME integration_r30_sicherung_laden
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r30_sicherung_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r30_sicherung_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r30_sicherung_laden.cmake)
    set_tests_properties(integration_r30_sicherung_laden PROPERTIES TIMEOUT 240)
endif()
