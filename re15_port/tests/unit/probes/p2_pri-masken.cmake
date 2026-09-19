# Sonden und Pins p2 / pri-masken (2026-09-19, Phase 2 des Befunds pri-masken-audit.md):
# Vordergrund-Masken STAGE1 ausschliesslich aus den Freistellungen des Nutzers.

# Reine Messsonde: Original-Atlanten ueber den Engine-Dekoder ablegen (Kalibrierung).
add_executable(probe_p2_atlas_dump probe_p2_atlas_dump.c)
target_link_libraries(probe_p2_atlas_dump PRIVATE re15_engine re15_test_support)
target_include_directories(probe_p2_atlas_dump PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_p2_atlas_dump PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# Pin: Hash-Riegel der Nutzer-Freistellungen (Liste pri_stage1_hashes.inc, erzeugt von
# tools/maske/pri_hashes.py).
add_executable(test_pri_hashes test_pri_hashes.c)
target_include_directories(test_pri_hashes PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(test_pri_hashes PRIVATE RE15_PORT_SRC_DIR="${CMAKE_SOURCE_DIR}")
add_test(NAME unit_pri_hashes COMMAND test_pri_hashes)
set_tests_properties(unit_pri_hashes PROPERTIES TIMEOUT 60)

# Pin: Engine-Deckung == Ziel-Silhouette (PBM) bitgenau, ueber die echten Leser.
add_executable(test_pri_silhouette test_pri_silhouette.c)
target_link_libraries(test_pri_silhouette PRIVATE re15_engine re15_test_support)
target_include_directories(test_pri_silhouette PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_pri_silhouette PRIVATE RE15_PORT_SRC_DIR="${CMAKE_SOURCE_DIR}")
add_test(NAME unit_pri_silhouette COMMAND test_pri_silhouette)
set_tests_properties(unit_pri_silhouette PROPERTIES TIMEOUT 120)

# Reine Messsonde: BEGEHBARE Standplaetze ueber den Spieler-Klemmpfad (nicht den
# Containment-Scan re15_collision_on_floor, der die Zell-INNENRAEUME liefert).
add_executable(probe_p2_floor_dump probe_p2_floor_dump.c)
target_link_libraries(probe_p2_floor_dump PRIVATE re15_engine re15_test_support)
target_include_directories(probe_p2_floor_dump PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_p2_floor_dump PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
