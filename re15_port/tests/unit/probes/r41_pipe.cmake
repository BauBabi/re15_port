# --- Runde 41 (2026-11-16), Spur "pipe". Nutzer: "die Pipe [...] ist noch nicht nutzbar. Eigentlich sollte die Pipe so
#     aehnlich funktionieren wie ein Messer, aber mit mehr Reichweite. [...] keine Anzahl, sondern unendlich da sein."
#   unit_r41_pipe   (1) Bank PL00W02/PL04W02 = Messerbank mit Rohr-Netz; (2) Reichweite: Rohr trifft weiter als das
#                   Messer (Messer unveraendert 1400..1500, Kegel @0x8006E5A4), Grenze passt zur Netz-Eichung;
#                   (3) Schaden: Zombie Rohr = Messer = 3 (RE2-Zeile @0x800A412C), Hund 20/5 (@0x8006EBD8/D4); (4) keine Ziffern fuer das Rohr in Gitter, Ausruest-
#                   Feld und Kiste (Gegenprobe Browning); (5) Speichern/Laden. Gegen master ROT in (1)(2)(4).
add_executable(test_pipe_r41 ${CMAKE_CURRENT_LIST_DIR}/../test_pipe_r41.c)
target_link_libraries(test_pipe_r41 PRIVATE re15_engine re15_test_support)
target_include_directories(test_pipe_r41 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_pipe_r41 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_pipe_r41 PRIVATE m)
endif()
add_test(NAME unit_r41_pipe COMMAND test_pipe_r41)
set_tests_properties(unit_r41_pipe PROPERTIES TIMEOUT 300)
