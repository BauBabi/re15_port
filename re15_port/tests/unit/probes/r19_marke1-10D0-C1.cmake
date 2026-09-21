# Riegel Runde 19 / Marke 1 (2026-09-21) — ROOM10D0 Cut 1: die Tischplatte des hinteren
# Klapptischs muss VOR dem Spieler an der Nutzer-Marke F162 (3575,0,-4568) liegen.
# Messzahlen und Herleitung: analysis/befunde_2026-09-21/pri-runde19/marke1-10D0-C1.md
# EIGENE Datei (nicht die gemeinsame CMakeLists.txt), damit parallele Agenten sich nicht
# in die Quere kommen — s. probes/README.md.
add_executable(test_r19_marke1_10d0_c1 test_r19_marke1_10d0_c1.c)
target_link_libraries(test_r19_marke1_10d0_c1 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r19_marke1_10d0_c1 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r19_marke1_10d0_c1 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME r19_marke1_10d0_c1 COMMAND test_r19_marke1_10d0_c1)
