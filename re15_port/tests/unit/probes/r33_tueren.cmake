# Runde 33 / Thema T - restliche Tueren (analysis/befunde_runde33/tueren_rest_plan.md, tueren_rest_pilot.md).
# GEPINNT -> add_test je Teil von probe_r33_tueren:
#   archive    Port-Archive shared_assets/RE15DOOR (tools/tueren/tuer_archiv_bauen.py): Groesse/FNV wie
#              gen/re15_tuer_eigen.inc, Tonteil + Kopf/SCD/MD1 bytegleich dem RE2-Basis-Archiv, TIM 8 bit
#              128x256 mit einer 256er-CLUT; jede benutzte Variante laeuft Bild fuer Bild wie das Basis-Archiv.
#   zuordnung  Port-Zeilen gegen die echten Door_aot_set; G1-Tuer ROOM1000 -> ROOM1050 stellt im
#              Spielschritt die Anfrage mit P07G (Basis DOOR07).
add_executable(probe_r33_tueren probe_r33_tueren.c)
target_link_libraries(probe_r33_tueren PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r33_tueren PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(probe_r33_tueren PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    RE15_ASSET_SHARED_DIR="${CMAKE_SOURCE_DIR}/shared_assets")
add_test(NAME unit_r33_tueren_archive   COMMAND probe_r33_tueren archive)
add_test(NAME unit_r33_tueren_zuordnung COMMAND probe_r33_tueren zuordnung)
set_tests_properties(unit_r33_tueren_archive unit_r33_tueren_zuordnung PROPERTIES TIMEOUT 120)
