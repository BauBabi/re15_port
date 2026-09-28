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

# ---- RIEGEL (Bau-Agent Runde 30) -------------------------------------------------
# unit_map_marke_zid: jede Marke haengt an ihrer EIGENEN Zone (keine Rueckfall-zid 0,
#   kein fremdes Rechteck, keine Marke ohne Traeger). Liest die Tabellen der Engine.
add_executable(test_map_marke_zid
    ${CMAKE_CURRENT_SOURCE_DIR}/test_map_marke_zid.c)
target_link_libraries(test_map_marke_zid PRIVATE re15_engine re15_test_support)
target_include_directories(test_map_marke_zid PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
add_test(NAME unit_map_marke_zid COMMAND test_map_marke_zid)
set_tests_properties(unit_map_marke_zid PROPERTIES TIMEOUT 30)

# unit_r30_karte_nutzerstand: die UNVERAENDERTE Speicherkarte des Nutzers vom 2026-09-27
#   (Platz 2, Alt-Stand v8) laden und die Op-Liste des echten Zeichners pruefen - kein
#   RE2-Blau, Schema-Fuellung halbtransparent in Palettenfarbe, keine Dach-Wandzeile,
#   keine Marke ohne Traeger, die vier verlorenen Orte wieder gezeichnet.
add_executable(test_r30_karte_nutzerstand
    ${CMAKE_CURRENT_SOURCE_DIR}/test_r30_karte_nutzerstand.c)
target_link_libraries(test_r30_karte_nutzerstand PRIVATE re15_engine re15_test_support)
target_include_directories(test_r30_karte_nutzerstand PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r30_karte_nutzerstand PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_R30_NUTZERKARTE="${CMAKE_CURRENT_SOURCE_DIR}/probes/r30_karte_nutzer_2026-09-27.mcr")
add_test(NAME unit_r30_karte_nutzerstand COMMAND test_r30_karte_nutzerstand)
set_tests_properties(unit_r30_karte_nutzerstand PROPERTIES TIMEOUT 60)

# unit_map_speichern_laden: Rundlauf capture -> Karte -> load -> restore je Ort (102),
#   Laden in laufender Sitzung (D2), Alt-Staende v6/v7/v8 (D3, Hebung), Save-Vertrag v9.
add_executable(test_map_speichern_laden
    ${CMAKE_CURRENT_SOURCE_DIR}/test_map_speichern_laden.c)
target_link_libraries(test_map_speichern_laden PRIVATE re15_engine re15_test_support)
target_include_directories(test_map_speichern_laden PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
add_test(NAME unit_map_speichern_laden COMMAND test_map_speichern_laden)
set_tests_properties(unit_map_speichern_laden PROPERTIES TIMEOUT 120)
