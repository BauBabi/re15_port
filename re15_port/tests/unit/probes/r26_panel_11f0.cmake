# Runde 26 (2026-09-26), Thema "ROOM11F0 Panel": Klick-Ton, roter Leistungs-Zeiger,
# versetzter Cursor-Schatten.
#
# Nutzer: "Der Bewegungs- und Bestaetigungssound des Cursors bei Raetseln etc. ist
# grundlegend falsch. Der Klick Sound soll so sein, wie bei Resident Evil 2 Room
# 2130/213 ... Ausserdem moechte ich in ROOM 11F0 so einen roten Cursor mit der
# Bewegungslogik ... Nach der Eingabe der korrekten Position, muss der Cursor auf 80
# stehen. Ausserdem hat der Cursor immer so einen verzerrten/versetzten Schatten."
#
# Belege mit Adresse: include/re15_panel_zeiger.h (RE2 ROOM2130.RDT sub04-Offsets + die
# selbst vermessene Skalen-Eichung von ROOM11F0 Cut 10) und include/re15_audio.h
# (warum RE1.5 hier unfertig ist: 0 Se_on im ganzen ROOM11F0-SCD, snd0-EDT @Datei 0x03794
# auf 0x0A/0x0C leer).
#
# EIGENE Datei (nicht die gemeinsame CMakeLists.txt), s. probes/README.md.
add_executable(r26_panel_11f0 ${CMAKE_CURRENT_SOURCE_DIR}/r26_panel_11f0.c)
target_link_libraries(r26_panel_11f0 PRIVATE re15_engine re15_test_support)
target_include_directories(r26_panel_11f0 PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(r26_panel_11f0 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r26_panel_11f0 COMMAND r26_panel_11f0)
set_tests_properties(unit_r26_panel_11f0 PROPERTIES TIMEOUT 240)
