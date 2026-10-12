# Runde 37 Spur C "fahrstuhl" (analysis/befunde_runde37/C_fahrstuhl.md). GEPINNT -> add_test je Teil von probe_r37_fahrstuhl:
#   archiv    Auftrag #6: Port-Archive P27F (Flurseite) / P27C (Kabinenseite) = RE2-Fahrstuhltuer DOOR27 (Tonteil + SCD
#             bytegleich) mit NEUEM MD1: zwei glatte Fluegel 1860 x 6600, gerade Fuge (Gegenprobe DOOR27: verzahnt);
#             Maschine Bild fuer Bild = DOOR27 V0 (2240 je Fluegel); Vorderseite Eckfarbe 73, gezeigt = gemalt
#             (Zeilenprofil der entzerrten RE1.5-Fluegel ROOM10C00/ROOM11200 bzw. ROOM10800, +-6), grau.
#   einstieg  echter Durchgang ROOM1040/10C0/1120 -> Kabine ROOM1080: genau eine Anfrage P27F V0 (S013/S040/S057).
#   ausstieg  Auftrag #7: echter Skriptweg der Kabine (Knopf = Evt_exec sub07/08/09, Fahrt, Plc_dest, Aot_on) mit dem
#             player_mode-Proxy von main.c: ROOM1080/1081 -> P27C V0 S027/S028/S029, ROOM4020/4021 -> DOOR27 V0
#             S221/S222/S223; beim Feuern player_mode 2; Gegenprobe normale Tuer in player_mode 2 ohne Sequenz.
add_executable(probe_r37_fahrstuhl probe_r37_fahrstuhl.c)
target_link_libraries(probe_r37_fahrstuhl PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r37_fahrstuhl PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(probe_r37_fahrstuhl PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    RE15_ASSET_SHARED_DIR="${CMAKE_SOURCE_DIR}/shared_assets")
add_test(NAME unit_r37_fahrstuhl_archiv   COMMAND probe_r37_fahrstuhl archiv)
add_test(NAME unit_r37_fahrstuhl_einstieg COMMAND probe_r37_fahrstuhl einstieg)
add_test(NAME unit_r37_fahrstuhl_ausstieg COMMAND probe_r37_fahrstuhl ausstieg)
set_tests_properties(unit_r37_fahrstuhl_archiv unit_r37_fahrstuhl_einstieg unit_r37_fahrstuhl_ausstieg
                     PROPERTIES TIMEOUT 300)
if(NOT WIN32)
    target_link_libraries(probe_r37_fahrstuhl PRIVATE m)
endif()

# integration_r37_fahrstuhl: echte exe (Tuersequenz-Laeufer, Ton) - tests/integration/test_r37_fahrstuhl.cmake
if(TARGET re15_pc)
    add_test(NAME integration_r37_fahrstuhl
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_fahrstuhl_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_fahrstuhl.cmake)
    set_tests_properties(integration_r37_fahrstuhl PROPERTIES TIMEOUT 900)
endif()

# Nachbesserung 1 (AUFTRAG.md Nachtrag 5, probe_r37_fahrstuhl_n1.c):
#   lastenaufzug  5a: Ausstieg Lastenaufzug ROOM3080/3081 -> ROOM4000/4001 = Port-Archiv P2DS Variante 5 (RE2 ROOM6170
#                 @0x035A2 DOOR2D V5 = hinab), echter Skriptweg aller vier Fahrten, Mechanik V5 hinab / V4 hinauf.
#   lift2030      5b: RE2-Ton der STAGE2-Hubplattformen ROOM2030/2031 (RE2 ROOM6030 Satz 0x11/0x0B, LIFT6030.VBS), echter
#                 Weg (Generator, Aktionstaste) hinauf/hinab, Lift A/B, Leon/Elza; Gegenprobe ohne Strom stumm.
add_executable(probe_r37_fahrstuhl_n1 probe_r37_fahrstuhl_n1.c)
target_link_libraries(probe_r37_fahrstuhl_n1 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r37_fahrstuhl_n1 PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(probe_r37_fahrstuhl_n1 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    RE15_ASSET_SHARED_DIR="${CMAKE_SOURCE_DIR}/shared_assets")
add_test(NAME unit_r37_fahrstuhl_lastenaufzug COMMAND probe_r37_fahrstuhl_n1 lastenaufzug)
add_test(NAME unit_r37_fahrstuhl_lift2030     COMMAND probe_r37_fahrstuhl_n1 lift2030)
set_tests_properties(unit_r37_fahrstuhl_lastenaufzug unit_r37_fahrstuhl_lift2030 PROPERTIES TIMEOUT 300)
if(NOT WIN32)
    target_link_libraries(probe_r37_fahrstuhl_n1 PRIVATE m)
endif()

# integration_r37_fahrstuhl_n1: echte exe - 3080 -> 4000 mit P2DS V5, Lift-Ton ROOM2030 (Aufnahme mit/ohne Aktionstaste,
# Auswertung probe_r37_fahrstuhl_n1 kapsel) - tests/integration/test_r37_fahrstuhl_n1.cmake
if(TARGET re15_pc)
    add_test(NAME integration_r37_fahrstuhl_n1
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_PROBE_N1=$<TARGET_FILE:probe_r37_fahrstuhl_n1>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r37_fahrstuhl_n1_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r37_fahrstuhl_n1.cmake)
    set_tests_properties(integration_r37_fahrstuhl_n1 PROPERTIES TIMEOUT 900)
endif()
