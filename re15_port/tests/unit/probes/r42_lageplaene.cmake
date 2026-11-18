# Runde 42 Strang lageplaene, Stufe 1 - STAGE1-Lageplaene in der RE2-Form (EIN Revierplan ROOM1030, EIN Kellerplan
# ROOM1200; die Etagenplaene 1140/10D0/11D0 sind entfernt). Belege: include/re15_kartenitems_r36.h (RUNDE 42).
#   unit_r42_lageplaene_tabelle / _revier / _keller / _weg / _texte / _alt   (tests/unit/test_r42_lageplaene.c)
# Gegenprobe: auf aaea5f8c2 (fuenf Etagenplaene) sind alle sechs Teile rot.
add_executable(test_r42_lageplaene ${CMAKE_CURRENT_LIST_DIR}/../test_r42_lageplaene.c)
target_link_libraries(test_r42_lageplaene PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_lageplaene PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_lageplaene PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_lageplaene PRIVATE m)
endif()
foreach(_teil tabelle revier keller weg texte alt)
    add_test(NAME unit_r42_lageplaene_${_teil} COMMAND test_r42_lageplaene ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_lageplaene_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

# Runde 42 Strang lageplaene, Stufe 2 - EIN Plan je Gebiet STAGE2..6 wie RE2 (Kanalisation 2000/2001 Bit 34, Fabrik nur 30C1
# Bit 35, U-Bahn 6000/6001 Bit 37; 4020 = r42/rest, 5030 = Original). Belege: include/re15_kartenitems_r36.h (RUNDE 42 STUFE 2).
#   unit_r42_lageplaene2_tabelle / _kanal / _fabrik / _ubahn / _texte / _sicht   (tests/unit/test_r42_lageplaene2.c)
# Gegenprobe: auf aaea5f8c2 (keine Plaene ausserhalb STAGE1) sind alle sechs Teile rot.
add_executable(test_r42_lageplaene2 ${CMAKE_CURRENT_LIST_DIR}/../test_r42_lageplaene2.c)
target_link_libraries(test_r42_lageplaene2 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_lageplaene2 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_lageplaene2 PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_lageplaene2 PRIVATE m)
endif()
foreach(_teil tabelle kanal fabrik ubahn texte sicht)
    add_test(NAME unit_r42_lageplaene2_${_teil} COMMAND test_r42_lageplaene2 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_lageplaene2_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

#   integration_r42_lageplaene2_weg   echte exe: Leon 2000, Elza 30C1, Leon 6000 gehen zum Plakat, Aktion -> Ereignis 28 (kein
#                                     Klettern), Ja -> Meldung 31. Gegenprobe auf aaea5f8c2 rot (kein Plakat).
if(TARGET re15_pc)
    add_test(NAME integration_r42_lageplaene2_weg
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_schluessel_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r42_lageplaene2_weg_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_lageplaene2_weg.cmake)
    set_tests_properties(integration_r42_lageplaene2_weg PROPERTIES TIMEOUT 900)
endif()

# Runde 42 Strang lageplaene_nach (Nacharbeit): Dach (Blatt 5, Bit 42), Revierplan an der Tuerkamera 2 der Lobby (s = 2),
# Fabrikplan s = 2. Belege: include/re15_kartenitems_r36.h (RUNDE 42 NACHARBEIT).
#   unit_r42_lageplaene3_dach / _revier / _druck / _fabrik   (tests/unit/test_r42_lageplaene3.c)
# Gegenprobe: auf aaea5f8c2 und auf bce9c91a8 (Stand vor der Nacharbeit) sind alle vier Teile rot.
add_executable(test_r42_lageplaene3 ${CMAKE_CURRENT_LIST_DIR}/../test_r42_lageplaene3.c)
target_link_libraries(test_r42_lageplaene3 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_lageplaene3 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_lageplaene3 PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r42_lageplaene3 PRIVATE m)
endif()
foreach(_teil dach revier druck fabrik)
    add_test(NAME unit_r42_lageplaene3_${_teil} COMMAND test_r42_lageplaene3 ${_teil}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r42_lageplaene3_${_teil} PROPERTIES TIMEOUT 300)
endforeach()

#   integration_r42_lageplaene3_weg   echte exe: Leon 1030 (aus Kamera 4) und Elza 1031 (vom Spielbeginn) gehen zur
#                                     Ausgangstuer nach 1050; Kamera 2 schaltet VOR der Aufnahme; Aktion im Streifen
#                                     Bank/Tuer -> Ereignis 28 (kein Klettern), Ja -> Meldung 31. Gegenprobe auf bce9c91a8
#                                     und aaea5f8c2 rot (dort haengt der Plan an der Rueckwand).
if(TARGET re15_pc)
    add_test(NAME integration_r42_lageplaene3_weg
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_schluessel_karte>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r42_lageplaene3_weg_wd
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r42_lageplaene3_weg.cmake)
    set_tests_properties(integration_r42_lageplaene3_weg PROPERTIES TIMEOUT 900)
endif()
