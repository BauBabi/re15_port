# Runde 27 (2026-09-26), Thema "ROOM11F0 Schalter-Raetsel": die zehn Schalterwerte des
# Nutzers und der Ton nur beim Schalten.
#
# Nutzer 1: "bei den Schalter raetsel wenn ich den cursor bewege kommt die ganze zeit
# sound. das will ich aber keinen sound. nur beim druecken eines Schalters soll es den
# Schalter sound geben."
# Nutzer 2 (woertliche Vorgabe der Werte, KEIN Original):
#     Schalter :  1    2    3    4    5    6    7    8    9   10
#     Wert     : +20  -20  -10  -30  +20  -40  +20  -50  +30  -60
#     Loesung 1+3+5+7+9 -> 0 -> 20 -> 10 -> 30 -> 50 -> 80; Anzeige = max(0, Summe).
#
# Der Riegel zaehlt alle 1024 Kombinationen selbst durch, liest die RE1.5-Loesungsmenge
# aus den ausgelieferten RDT-Bytes (ROOM11F0.RDT @0x012BE..0x012E2) und misst, dass der
# Bewegungs-Ton weg ist, der INVENTAR-Cursor aber weiter toent (CORE-Satz 4,
# RE1.5 @0x8004a478) und der Schalter-Ton je Betaetigung genau einmal kommt
# (RE2-Raum-SE Gruppe 2 / Index 0x0A, ROOM2130.RDT sub04+0x0082 @Datei 0x01192).
#
# EIGENE Datei (nicht die gemeinsame CMakeLists.txt), s. probes/README.md.
add_executable(r27_panel_schalterwerte ${CMAKE_CURRENT_SOURCE_DIR}/r27_panel_schalterwerte.c)
target_link_libraries(r27_panel_schalterwerte PRIVATE re15_engine re15_test_support)
target_include_directories(r27_panel_schalterwerte PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(r27_panel_schalterwerte PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r27_panel_schalterwerte COMMAND r27_panel_schalterwerte)
set_tests_properties(unit_r27_panel_schalterwerte PROPERTIES TIMEOUT 240)
