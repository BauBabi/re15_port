# --- Runde 26 (2026-09-26), Thema eintritt-pose: die erste Animation wiederholt sich,
#     wenn nach dem Raumwechsel eine Cutscene startet (Leon ROOM1170 nach der Tuer,
#     Leon/Ada ROOM1050). Ursache: die Raum-Eintritts-Pose (Sentinel 210 = W-Bank-Clip 1)
#     wurde als DAUER-Motion geparkt und ueber cur % frame_count zyklisch gespielt.
#     Dossier: analysis/befunde_2026-09-26/cutscene-erste-anim-wiederholt.md
#     RIEGEL (add_test): die Sonde faellt mit exit 1, sobald motion 210 laenger als ein
#     Bild steht, anim_frame darauf ueber 0 laeuft, es Neustarts gibt oder die Uebergabe
#     an den Idle-FSM Fall 0 (motion 200, +0x94 = 3 @0x80032088) nicht in F1 faellt.
#     Original-Beleg: cmd-0-Handler @0x800318f8 laeuft genau einmal (@0x8003192c) und setzt
#     die Pose hart auf Bild 0 (@0x80031c10-c24). ---
add_executable(probe_eintritt_pose_einmal probe_eintritt_pose_einmal.c)
target_link_libraries(probe_eintritt_pose_einmal PRIVATE re15_engine re15_test_support)
target_include_directories(probe_eintritt_pose_einmal PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_eintritt_pose_einmal PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME eintritt_pose_einmal COMMAND probe_eintritt_pose_einmal)
set_tests_properties(eintritt_pose_einmal PROPERTIES TIMEOUT 60)
