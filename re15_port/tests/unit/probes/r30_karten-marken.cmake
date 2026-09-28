# Runde 30 (2026-09-27), Thema F "karten-marken".
# Dossier: analysis/befunde_runde30/karten-marken.md
#
# probe_r30_karten-marken = MESS-WERKZEUG (kein add_test): Zensus der Besucht-Bits,
#   Marken mit fremder zid, Innenwaende gegen die Kachel, Speichern/Laden-Rundlauf.
#   Bindet engine/src ein, um DIESELBEN Tabellen zu lesen, die die Engine fuehrt.
add_executable(probe_r30_karten-marken
    ${CMAKE_CURRENT_SOURCE_DIR}/probe_r30_karten-marken.c)
target_link_libraries(probe_r30_karten-marken PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_karten-marken PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(probe_r30_karten-marken PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# Fortsetzung (dritter Agent): Rundlauf je Ort, Nutzer-Karte, Marken-Zensus.
add_executable(probe_r30_karten-marken_rundlauf
    ${CMAKE_CURRENT_SOURCE_DIR}/probe_r30_karten-marken_rundlauf.c)
target_link_libraries(probe_r30_karten-marken_rundlauf PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_karten-marken_rundlauf PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(probe_r30_karten-marken_rundlauf PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
