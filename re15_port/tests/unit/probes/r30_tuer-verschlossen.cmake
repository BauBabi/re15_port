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
