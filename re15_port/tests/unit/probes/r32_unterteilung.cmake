# Runde 32 - Tueren-Unterteilung (analysis/befunde_runde32/tueren_unterteilung.md).
# GEPINNT -> add_test je Teil von probe_r32_unterteilung:
#   referenz  re15_door_divide_gt3 gegen die ORIGINAL-Befehle von DivideGT3 0x8008ebf4 (+ RotAverageNclip3,
#             ReadSZfifo3, RCpolyGT3A, Paketausgabe 0x8008f288), ausgefuehrt von einem Mini-R3000 aus
#             info/re2leon/PSX.EXE: DOOR13-Blatt in den echten Sequenzmatrizen + 3000 Zufallsdreiecke.
#   tor       Flag-Tor am echten Zeichenpfad re15_door_mesh_zeichnen: 0x0aa0 teilt, 0x0a80 (Tor) nicht.
add_executable(probe_r32_unterteilung probe_r32_unterteilung.c)
target_link_libraries(probe_r32_unterteilung PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r32_unterteilung PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(probe_r32_unterteilung PRIVATE
    RE15_RE2_EXE_PATH="${CMAKE_SOURCE_DIR}/../info/re2leon/PSX.EXE"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r32_unterteilung_referenz COMMAND probe_r32_unterteilung referenz)
add_test(NAME unit_r32_unterteilung_tor      COMMAND probe_r32_unterteilung tor)
set_tests_properties(unit_r32_unterteilung_referenz unit_r32_unterteilung_tor PROPERTIES TIMEOUT 120)
