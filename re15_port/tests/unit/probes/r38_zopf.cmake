# Runde 38 Spur D "zopf" — Elzas Zopf wie im Original RE1.5 (FUN_80024c30 / FUN_80024e40; Init Kommando 0
# @0x800318f8, Aufruf @0x80031d98 hinter `andi v0,v0,0x4` @0x80031d8c). Dieselbe Routine fuehrt RE2 fuer Claire
# (FUN_8001844c / FUN_80018728). Dossier analysis/befunde_runde38/D_zopf.md.
#
# RIEGEL (Unit, misst die Mechanik): libgte init ruhe drehung laufen freeze — und
# `test_r38_zopf spur <zopf.log>` / `pixel <mit.ppm> <ohne.ppm> x0 y0 x1 y1` fuer die Integration.
add_executable(test_r38_zopf ${CMAKE_CURRENT_LIST_DIR}/../test_r38_zopf.c)
target_link_libraries(test_r38_zopf PRIVATE re15_engine re15_test_support)
target_include_directories(test_r38_zopf PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r38_zopf PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT MSVC)
    target_link_libraries(test_r38_zopf PRIVATE m)
endif()
foreach(_teil libgte init ruhe drehung laufen freeze)
    add_test(NAME unit_r38_zopf_${_teil} COMMAND test_r38_zopf ${_teil})
    set_tests_properties(unit_r38_zopf_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

# INTEGRATION (echte exe): Elza ROOM1121 mit dem Ruecken zur Kamera, Tank-Drehung — Winkelverlauf (RE15_ZOPF_LOG),
# Pixel am Hinterkopf (Bild mit Zopf gegen die Mess-Sonde RE15_ZOPF_SONDE_AUS), Gegenprobe Leon ohne Zopf.
add_test(NAME integration_r38_zopf
    COMMAND ${CMAKE_COMMAND}
        -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
        -DRE15_ZOPF_TOOL=$<TARGET_FILE:test_r38_zopf>
        -DWORKDIR=${CMAKE_BINARY_DIR}/r38_zopf_wd
        -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r38_zopf.cmake)
set_tests_properties(integration_r38_zopf PROPERTIES TIMEOUT 900)
