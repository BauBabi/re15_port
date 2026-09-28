# Runde 30 (2026-09-27), Thema "tuer-verschlossen": was tut der Port HEUTE an einer
# verschlossenen Tuer / einem Kartenleser / einem Code-Feld - Text und Ton?
# Dossier: analysis/befunde_runde30/tuer-verschlossen.md
# Reine Messsonde (kein add_test): sie faehrt die ausgelieferten SCD-Bytes im Port-VM,
# drueckt an JEDEM aktiven Text-/Event-/Tuer-Platz einmal QUADRAT und protokolliert
# Nachricht + jeden SE-Aufruf (fuenf Bank-Spione aus tests/test_support.c + SCD-Se_on).
add_executable(probe_r30_tuer_verschlossen probe_r30_tuer_verschlossen.c)
target_link_libraries(probe_r30_tuer_verschlossen PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_tuer_verschlossen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_tuer_verschlossen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")

# Zweite Messsonde: laedt die von tools/re2_door_se_cut.py geschnittene Mini-Bank TUERSE.VBS
# mit demselben VAB-Code wie ELEVSE.VBS und druckt je Satz Welle/Tonhoehe/Dauer.
add_executable(probe_r30_tuerse_bank probe_r30_tuerse_bank.c)
target_link_libraries(probe_r30_tuerse_bank PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_tuerse_bank PRIVATE ${CMAKE_SOURCE_DIR}/include)

# ---- BAU (Runde 30): RIEGEL fuer den "Tuer verschlossen"-Ton (RE2-ERGAENZUNG) ----
# Ein Programm, vier Teile (Dossier §5 Schritt 6): (a) Bank-Bytes/pitch, (b) 98 Raeume im
# Frisch-Zustand (51 Plaetze je 1 Ton mit dem Satz der Art, sonst 0, fuenf Pflichtfaelle),
# (c) ROOM4000 sub02 behaelt RE1.5s eigenes Se_on(2,0x0f), (d) Tabellen-Volllauf 52 Zeilen.
add_executable(test_r30_tuer ${CMAKE_CURRENT_SOURCE_DIR}/test_r30_tuer.c)
target_link_libraries(test_r30_tuer PRIVATE re15_engine re15_test_support)
target_include_directories(test_r30_tuer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r30_tuer PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r30_tuerse_bank        COMMAND test_r30_tuer bank)
add_test(NAME unit_r30_tuer_tabelle       COMMAND test_r30_tuer tabelle)
add_test(NAME unit_r30_tuer_r4000         COMMAND test_r30_tuer r4000)
add_test(NAME unit_r30_tuer_verschlossen  COMMAND test_r30_tuer raeume)
set_tests_properties(unit_r30_tuerse_bank unit_r30_tuer_tabelle unit_r30_tuer_r4000
    PROPERTIES TIMEOUT 60)
set_tests_properties(unit_r30_tuer_verschlossen PROPERTIES TIMEOUT 240)
