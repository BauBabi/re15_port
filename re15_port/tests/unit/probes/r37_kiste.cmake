# --- Runde 37 (2026-10-12), Spur M "kiste". Dossier: analysis/befunde_runde37/M_kiste.md
#     Nutzer (Nachtrag 9): Symbole in der Kiste so weit rechts wie moeglich, kein Name ueberdeckt sie;
#     beim Scrollen liegt der Kasten (Rahmen) layertechnisch UEBER den Items.

# Kartenwerkzeug (kein eigener Test): Spielstand vor der Kiste, Kiste mit den laengsten Namen gefuellt.
add_executable(probe_r37_kiste_karte ${CMAKE_CURRENT_LIST_DIR}/../probe_r37_kiste_karte.c)
target_link_libraries(probe_r37_kiste_karte PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r37_kiste_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(probe_r37_kiste_karte PRIVATE m)
endif()
