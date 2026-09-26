# Runde 27 (2026-09-27), Thema "ada-cutscene-1050".
# Dossier: analysis/befunde_2026-09-27/ada-cutscene-1050.md
#
# PIN unit_r27_ada_cutscene_1050: faehrt ROOM1050s Cutscene-Start (Flag(3,0x6e) +
#   scd_room_reenter, danach SCD-VM -> Entity-Schleife in der Original-Reihenfolge
#   @0x8001cdec/@0x8001ce04) und verlangt den byte-true Posen-Strom der NPC-Executor-Subs:
#   jeder Clip POSIERT zuerst Bild 0 und endet auf Bild fc-1.
#   Original: anim_set FUN_8001f314 posiert das aktuelle +0x95 (@0x8001f35c/@0x8001f36c)
#   und schiebt es erst am Ende des Keyframe-Integrators vor (@0x8001f618-1c), mit
#   Rueckgabe 1 erst NACH dem letzten Bild (@0x8001f624-3c).
#   Gegenprobe am alten Stand (re15_npc_anim schob VOR dem Posieren vor): M1/M2/M4 = 1
#   statt 0 und M3 = 0 statt 15 -> vier rote Pruefungen, Exit 1.
add_executable(test_r27_ada_cutscene_1050 test_r27_ada_cutscene_1050.c)
target_link_libraries(test_r27_ada_cutscene_1050 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r27_ada_cutscene_1050 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r27_ada_cutscene_1050 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r27_ada_cutscene_1050 COMMAND test_r27_ada_cutscene_1050)
