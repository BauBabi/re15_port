# Runde 41 (2026-10-30), Spur P "alligator2" - Riegel. Dossier: analysis/befunde_runde41/P_alligator2.md.
#   Punkte: Nachtrag 31 Punkt 2 (Kampfbeginn nicht vor der Linse), FG-E7 Option (a) (Schluessel-Lagewahl sucht weiter), Lagespruenge
#   mit dem Kanister im Maul (Kopf-Klemme <-> Koerper-Klemme), Riegel Uebergabe Ankunftsszene 2090 -> Knall + 8 Zombies in 2080.
add_executable(test_r41_alligator2 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_alligator2.c)
target_link_libraries(test_r41_alligator2 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_alligator2 PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/..)
target_compile_definitions(test_r41_alligator2 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(test_r41_alligator2 PRIVATE m)
endif()
foreach(_teil kamera kamera_elza schluessel tragen)
    add_test(NAME unit_r41_alligator2_${_teil} COMMAND test_r41_alligator2 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r41_alligator2_${_teil} PROPERTIES TIMEOUT 600)
endforeach()

if(TARGET re15_pc AND EXISTS ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_alligator2.cmake)
    foreach(_teil kamera kamera_elza uebergabe uebergabe_elza k3 k3_elza)
        add_test(NAME integration_r41_alligator2_${_teil}
                 COMMAND "${CMAKE_COMMAND}"
                         -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                         -DRE15_TOOL=$<TARGET_FILE:test_r41_alligator2>
                         -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r41_alligator2_${_teil}_wd
                         -DTEIL=${_teil}
                         -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r41_alligator2.cmake)
        set_tests_properties(integration_r41_alligator2_${_teil} PROPERTIES TIMEOUT 900)
    endforeach()
endif()
