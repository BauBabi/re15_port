# Runde 41 (2026-10-24), Spur J "stage3szenen_a" - Orchestrator-Entscheid A-E5: RBJ-Marker-Binder byte-true FUN_8001b3f8.
# Eigene Datei (nicht r41_stage3szenen_a.cmake), damit Spur K den Commit "rbj-binder" unveraendert per cherry-pick
# uebernehmen kann, ohne dass spaetere J-Riegel bei der Integration add/add-Konflikte erzeugen.
# Dossier analysis/befunde_runde41/J_stage3szenen_a.md S0, Code engine/src/rbj_binder_r41.c, include/re15_rbj_binder_r41.h.
#
#   unit_r41_stage3szenen_a_rbj_mechanik  synthetische Marker: a3 kumulativ (@0x8001b4c4), Spieler = letzter Bit-0-Record
#   unit_r41_stage3szenen_a_rbj_zensus    alle RDTs: 47 mit RBJ, 12 abweichend, Sollwerte je Raum
#   unit_r41_stage3szenen_a_rbj_kanal     Port-Weg re15_actor_rbj_anim / re15_apply_room_cinematic an 3060/3091/2000/1040/3061
add_executable(test_r41_stage3szenen_a_rbj ${CMAKE_CURRENT_LIST_DIR}/../test_r41_stage3szenen_a_rbj.c)
target_link_libraries(test_r41_stage3szenen_a_rbj PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_stage3szenen_a_rbj PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r41_stage3szenen_a_rbj PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r41_stage3szenen_a_rbj PRIVATE m)
endif()
foreach(_teil mechanik zensus kanal)
    add_test(NAME unit_r41_stage3szenen_a_rbj_${_teil} COMMAND test_r41_stage3szenen_a_rbj ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_stage3szenen_a_rbj_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
