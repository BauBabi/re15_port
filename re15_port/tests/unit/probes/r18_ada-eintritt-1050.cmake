# Runde 18 (2026-09-21), Thema "ada-eintritt-1050".
# Dossier: analysis/befunde_2026-09-21/ada-eintritt-1050.md
#
# PIN unit_ada_eintritt_1050: faehrt den ROOM1050-Eintritt (Flag(3,0x6e) + scd_room_reenter)
#   und verlangt, dass Ada (Typ 0x42) in der Tuer-Blende in ihrer INIT-Pose Clip 2 steht statt
#   im Laufzyklus Clip 0. Original: State-0-INIT der Wurzel 0x8011cb70 @0x8011cd8c/@0x8011cd90
#   (`ori v0,zero,0x2` / `sb v0,148(v1)`); Sce_em_set FUN_800420a0 schreibt +0x94 nie und der
#   Raumwechsel FUN_8001a4c0 @0x8001a4e8 nullt am Entity nur +0x00.
#   Gegenprobe: mit dem alten Platzhalter (return 0) faellt der Riegel rot.
add_executable(test_r18_ada_eintritt_1050 test_r18_ada_eintritt_1050.c)
target_link_libraries(test_r18_ada_eintritt_1050 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r18_ada_eintritt_1050 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r18_ada_eintritt_1050 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_ada_eintritt_1050 COMMAND test_r18_ada_eintritt_1050)
