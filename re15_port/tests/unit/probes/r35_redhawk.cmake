# Runde 35 Spur D "redhawk" — Fleisch-Effekte der Hunde (Super Redhawk) laufen ewig.
#
# Dossier: analysis/befunde_runde35/D_redhawk.md.
#
# MESS-WERKZEUG (kein add_test): probe_r35_redhawk zensus = Routinen-Zensus aller Raum-ESP-Baenke.
add_executable(probe_r35_redhawk ${CMAKE_CURRENT_LIST_DIR}/../test_r35_redhawk.c)
target_link_libraries(probe_r35_redhawk PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r35_redhawk PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r35_redhawk PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r35_redhawk PRIVATE m)
endif()

# unit_r35_redhawk   PIN 1: Super Redhawk (Waffe 7) toetet den Hund in ROOM11D0 -> 6 Raum-Id-7-Brocken;
#                    jeder landet (Routine B 36 @0x800187c4 -> 37), schliesst ab (Routine 37 @0x8001885c:
#                    Flags := row[0x0e], Anim := row[0x16], Vorschub @0x800188a0) und endet am Anim-
#                    Terminator (@0x8001a40c); 90 Bilder nach dem Schuss 0 lebende Brocken (vorher 6/6).
#                    PIN 2: Pistole bis zum Tod (HURT-Brocken FX 1/2 @0x80102838-48) -> 0 Brocken.
#                    PIN 3: alle Raeume mit Raum-Id 7, jeder Brocken-Sub gespawnt -> 0 Haenger.
add_test(NAME unit_r35_redhawk COMMAND probe_r35_redhawk pin)
set_tests_properties(unit_r35_redhawk PROPERTIES TIMEOUT 300)
