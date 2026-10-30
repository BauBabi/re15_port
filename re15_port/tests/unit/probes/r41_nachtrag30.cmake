# Runde 41 Spur N "nachtrag30" — Riegel, zaehlen zur Suite. Dossier: analysis/befunde_runde41/N_nachtrag30.md.
#   unit_r41_nachtrag30            Mechanik: Panel-Regel + Vollsuche + Weg im Raum (Leon/Elza), Bruecken-Toene, Schrank 2010/2011, Wasserlauf 2000/2001, Sirene, Notiz + Loesung
#   integration_r41_nachtrag30_*   echte exe: panel (Leon/Elza), schrank (Leon/Elza), wasser (Leon/Elza), ton (Sirene + Bruecke, Mitschnitt-Zwilling), laut (volle Mischung 100 gegen 50)
add_executable(test_r41_nachtrag30 ${CMAKE_CURRENT_LIST_DIR}/../test_r41_nachtrag30.c)
target_link_libraries(test_r41_nachtrag30 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r41_nachtrag30 PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r41_nachtrag30 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    RE15_N30_FILE31_P01=0xDD14987Du
    RE15_N30_FILE31_P02=0x4087837Fu)
if(NOT WIN32)
    target_link_libraries(test_r41_nachtrag30 PRIVATE m)
endif()
add_test(NAME unit_r41_nachtrag30 COMMAND test_r41_nachtrag30)
set_tests_properties(unit_r41_nachtrag30 PROPERTIES TIMEOUT 300)
