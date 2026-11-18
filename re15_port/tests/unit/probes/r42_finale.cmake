# Runde 42 Strang finale, Stufe 1 (include/re15_finale_r42.h, include/re2_buehne.h, re2_fx.h FUN_8001c8c4/cfbc/cefc).
# Eigene Datei (GLOB in tests/unit/CMakeLists.txt).
#   unit_r42_finale               Buehne (S3), Besitzer-Effekte (S2), Tyrant-Wurzel (S4), Programm sub15 (S6)
#   integration_r42_finale_schuss Leon 5090 mit Pruefhaken RE15_FINALE_WERFER: Abschuss, Zeile, drei Wiederholungen, Einschlag,
#                                 Kill, Abspann 6040, Titel; Bildabstaende gegen die RE2-Zahlen (tests/integration/test_r42_finale_schuss.cmake)
get_filename_component(_r42f_repo_root "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
add_executable(test_r42_finale ${CMAKE_CURRENT_LIST_DIR}/../test_r42_finale.c)
target_link_libraries(test_r42_finale PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_finale PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_finale PRIVATE RE15_REPO_ROOT="${_r42f_repo_root}")
if(NOT WIN32)
    target_link_libraries(test_r42_finale PRIVATE m)
endif()
add_test(NAME unit_r42_finale COMMAND test_r42_finale)
set_tests_properties(unit_r42_finale PROPERTIES TIMEOUT 120)
if(TARGET re15_pc)
    add_test(NAME integration_r42_finale_schuss
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r42_finale_schuss_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_finale_schuss.cmake)
    set_tests_properties(integration_r42_finale_schuss PROPERTIES TIMEOUT 900)
endif()
# Plan S7 (Pruefbericht s1 r1): ohne Bank-9-Bit 129 bleiben sub02 (Abspann-Vorspann) und Tuer 5 (ROOM6040/6041) gesperrt, mit Bit offen.
add_executable(test_r42_finale_abspann ${CMAKE_CURRENT_LIST_DIR}/../test_r42_finale_abspann.c)
target_link_libraries(test_r42_finale_abspann PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_finale_abspann PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    target_link_libraries(test_r42_finale_abspann PRIVATE m)
endif()
add_test(NAME unit_r42_finale_abspann COMMAND test_r42_finale_abspann)
set_tests_properties(unit_r42_finale_abspann PROPERTIES TIMEOUT 120)
# Plan S1 (Pruefbericht s1 r2 R2): R702.ESP, R702_xx.TIM, Raumtonbank R702.EDT/.VH/.VB und R702_XA03_LEON.XAS bytegleich mit ROOM7020.RDT
# bzw. VOICE07.XAS (tests/unit/test_r42_finale_assets.c). Gegenprobe ueber RE15_R42F_RE2=<anderer RE2-Ordner>.
add_executable(test_r42_finale_assets ${CMAKE_CURRENT_LIST_DIR}/../test_r42_finale_assets.c)
target_include_directories(test_r42_finale_assets PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_finale_assets PRIVATE RE15_REPO_ROOT="${_r42f_repo_root}" RE15_ASSETS_PATH=${RE15_ASSETS_PATH})
add_test(NAME unit_r42_finale_assets COMMAND test_r42_finale_assets)
set_tests_properties(unit_r42_finale_assets PROPERTIES TIMEOUT 120)
