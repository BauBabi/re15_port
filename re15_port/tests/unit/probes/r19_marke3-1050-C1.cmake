# Runde 19, Nutzer-Marke 3 (befund.log Zeile 13287) — ROOM1050 Cut 1:
# Leons Bein hing vor der Schreibtischplatte, obwohl er hinter dem Tisch steht.
# Ursache war die TIEFE (Spaltenregel las die Plattenkante als Bodenkontakt), nicht
# die Freistellung. Der Riegel haelt die geschriebenen Maskentiefen gegen die
# fernste Ecke der SCA-Sperrzelle, die der Gegenstand IST — beide Groessen aus den
# ausgelieferten Dateien, keine aus dem Bauwerkzeug.
# Dossier: analysis/befunde_2026-09-21/pri-runde19/marke3-1050-C1.md
# ABDECKUNG: 1 der 77 geschriebenen Cuts, 1 der 4 Marken dieser Runde.
add_executable(r19_marke3_1050_c1_tiefe ${CMAKE_CURRENT_SOURCE_DIR}/r19_marke3_1050_c1_tiefe.c)
target_link_libraries(r19_marke3_1050_c1_tiefe PRIVATE re15_engine re15_test_support)
target_include_directories(r19_marke3_1050_c1_tiefe PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(r19_marke3_1050_c1_tiefe PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r19_marke3_1050_c1_tiefe COMMAND r19_marke3_1050_c1_tiefe)
set_tests_properties(unit_r19_marke3_1050_c1_tiefe PROPERTIES TIMEOUT 30)
