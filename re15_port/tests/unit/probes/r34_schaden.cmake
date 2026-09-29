# Runde 34 (Granaten), Spur B — Schaden und Gegnerreaktion aller Typen.
#
# Dossier: analysis/befunde_runde34_granaten/bau_b.md, Abnahme BAUPLAN.md §3.2.
#
#   unit_r34_schaden     Resolver-Tore (Gate B @0x80012f54-60, NPC-Ausschluss E7), RE2-Stempel
#                        der Explosion (E6), RE2-GL-Applier FUN_800470C0-Zwilling (B4),
#                        Trefferkasten-Versatz FUN_8002b498 (B2)
#   unit_r34_reaktion    Gegnerreaktion je Typ + Zensus "kein Tabellensprung auf NULL"
add_executable(probe_r34_schaden ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_schaden.c)
target_link_libraries(probe_r34_schaden PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34_schaden PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34_schaden PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r34_schaden PRIVATE m)
endif()
add_test(NAME unit_r34_schaden COMMAND probe_r34_schaden alle)
set_tests_properties(unit_r34_schaden PROPERTIES TIMEOUT 120)
