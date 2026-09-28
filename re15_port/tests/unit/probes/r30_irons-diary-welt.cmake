# Runde 30 (2026-09-27), Thema E2 "Irons Diary: Welt-Prop auf dem Schreibtisch + Memory Card".
# REINE MESS-Sonde (kein add_test): Kameratabelle/Sichtmatrix ROOM1150 mit den Engine-
# Funktionen, Projektion/Rueckprojektion der beiden Nutzer-Marken, Prop-Pool nach dem
# Hochfahren. Dossier: analysis/befunde_runde30/irons-diary-welt.md
add_executable(probe_r30_irons-diary-welt "probe_r30_irons-diary-welt.c")
target_link_libraries(probe_r30_irons-diary-welt PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_irons-diary-welt PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_irons-diary-welt PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
