# Runde 19b, Nutzer-Marke 4 (befund.log Zeile 13944, Bild befund_10D0_F3843_marke1.bmp)
# — ROOM10D0 Cut 7, der Klappstuhl.
#
# Der Riegel haelt die MODELLKLASSE fest: der Stuhl wird aus EINEM Standpunkt
# modelliert, seine Tiefe haengt nur von der Bildzeile ab, und kein Punkt liegt hinter
# seinem eigenen Fussabdruck (Schranke aus dem Kamerasatz von ROOM10D0.RDT, nicht aus
# einer an der Marke gemessenen Zahl). Die GEGENPROBE rechnet der Test selbst: die
# Spaltenregel, aus demselben Kamerasatz und derselben Silhouette, verletzt beide
# Eigenschaften — sonst stuende der Riegel wirkungslos gruen.
#
# ABDECKUNG: 1 der 77 geschriebenen Cuts, darin 1 von 3 Objekten, 2677 Texel.
# Dossier: analysis/befunde_2026-09-21/pri-runde19b-marke4.md
# EIGENE Datei (nicht die gemeinsame CMakeLists.txt), s. probes/README.md.
add_executable(r19b_marke4_10d0_c7_zeilenmodell
    ${CMAKE_CURRENT_SOURCE_DIR}/r19b_marke4_10d0_c7_zeilenmodell.c)
target_link_libraries(r19b_marke4_10d0_c7_zeilenmodell PRIVATE re15_engine re15_test_support)
target_include_directories(r19b_marke4_10d0_c7_zeilenmodell PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(r19b_marke4_10d0_c7_zeilenmodell PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_PROBE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/probes")
add_test(NAME unit_r19b_marke4_zeilenmodell COMMAND r19b_marke4_10d0_c7_zeilenmodell)
set_tests_properties(unit_r19b_marke4_zeilenmodell PROPERTIES TIMEOUT 30)
