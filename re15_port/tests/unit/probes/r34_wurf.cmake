# Runde 34 (Granaten) — Spur A: Wurf, Flug, Abprall, Zuender, Explosion, Item-Debug.
#
# Dossier: analysis/befunde_runde34_granaten/bau_a.md (Bauplan BAUPLAN.md §3.1 A9)
#
#   unit_r34_wurf   echte Engine (CORE00.ESP als globale Bank, re15_esp_fx_tick, re15_resolve_attack):
#                   Zeitlinien MITTE/HOCH/TIEF/vergiftet exakt nach BAUPLAN §1.1, SE-Folge, genau EIN
#                   Resolver-Aufruf im Zuender-7-Bild, Kinder 0x03195000/0x030B5400/0x030B5800, Latch,
#                   Gier 1024, Ziel-Dummy 0x27 + Spieler-Eigenschaden, 0x0A/0x0B mit Aufschlag-Spion,
#                   Pool voll, Raumwechsel, Negativ-Kontrollen; Spielschritt (kein Schaden im Abzugsbild,
#                   R1 los, Drehen) und Item-Debug des Statusschirms.
add_executable(probe_r34_wurf ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_wurf.c)
target_link_libraries(probe_r34_wurf PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34_wurf PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34_wurf PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
# libm unter Linux nicht implizit (Muster probes/r30_granate.cmake)
if(NOT WIN32)
    target_link_libraries(probe_r34_wurf PRIVATE m)
endif()
add_test(NAME unit_r34_wurf COMMAND probe_r34_wurf)
set_tests_properties(unit_r34_wurf PROPERTIES TIMEOUT 120)
