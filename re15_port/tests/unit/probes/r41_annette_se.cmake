# Runde 41 Spur AN, Nachbesserung 1: RE2-Toene der Annette-Szene - Mini-Bank shared_assets/RE2/ANNETTE_SE.VBS
# (tools/r41_annette/an_se_cut.py, Spieler platform/pc/src/annette_se_pc.c, Belege analysis/befunde_runde41/AN_belege/an_se.md).
#   bank   Groesse/FNV, VH, 4 Wellen, Satznummern = RE15_AN_SE_*
#   toene  EDT/Ton-Bytes je Satz = RE2-Quelle ausser Lage (Programm 0, Ton-Lage, VAG-Index), Tonhoehen 0x5F3/0x400/0x1C4/0x23A
#   lagen  Lagen 1/2/1/1 (STURZ zweilagig wie ROOM6141 Satz 0x0A), Wellen dekodierbar, Laenge bis zum Endblock, nicht stumm
add_executable(probe_r41_annette_se ${CMAKE_CURRENT_LIST_DIR}/../probe_r41_annette_se.c)
target_link_libraries(probe_r41_annette_se PRIVATE re15_engine)
target_include_directories(probe_r41_annette_se PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r41_annette_se PRIVATE
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
if(NOT WIN32)
    target_link_libraries(probe_r41_annette_se PRIVATE m)
endif()
foreach(_teil bank toene lagen)
    add_test(NAME unit_r41_annette_se_${_teil} COMMAND probe_r41_annette_se ${_teil})
    set_tests_properties(unit_r41_annette_se_${_teil} PROPERTIES TIMEOUT 120)
endforeach()
