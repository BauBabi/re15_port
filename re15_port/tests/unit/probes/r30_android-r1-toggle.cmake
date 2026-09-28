# --- Runde 30 (2026-09-27), Thema C "Android: R1 als Umschalter".
#     Dossier: analysis/befunde_runde30/android-r1-toggle.md
#     ERMITTLUNGS-SONDE, noch KEIN add_test: die Umschalt-Logik liegt als VORSCHLAG in
#     analysis/befunde_runde30/android-r1-toggle/touch_r1_toggle.h. Der Bau-Agent legt sie nach
#     platform/pc/src/touch_r1_toggle_pc.h, stellt den Include-Pfad unten um und schaltet dann
#     add_test scharf (siehe Dossier, Abschnitt 5).
#     Die Sonde faehrt den echten Spielschritt dreifach (HALTEN / RASTE / NAIV) und faellt mit
#     exit 1, sobald die RASTE einen Sollwert verfehlt. ---
add_executable(probe_r30_android_r1_toggle probe_r30_android_r1_toggle.c)
target_link_libraries(probe_r30_android_r1_toggle PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_android_r1_toggle PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../analysis/befunde_runde30/android-r1-toggle)
target_compile_definitions(probe_r30_android_r1_toggle PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
