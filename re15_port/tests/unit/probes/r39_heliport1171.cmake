# Runde 39 (2026-10-18), Spur K "heliport1171" — Johns Animationen in Elzas Dachszene ROOM1171.
# NUTZER: "Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran wink
# Animation, die er im Original nicht macht. Auch sonst scheinen die Animationen hier noch nicht ganz zu passen."
# Ursache: lazy RBJ-Kanal-Cache (engine/src/enemy_common.c, 2 Plaetze) von den Ereignis-Kraehen belegt -> John posierte
# seine Sub-0-Gesten aus Bank 0 (Clip 2 = Kletter-Clip +0x94 = 2 @0x800526a0) statt aus RBJ rec1 (FUN_8001b3f8 bindet
# jedes Marker-Bit ohne Grenze @0x8001b470-c4; Sub 0 spielt +0x180/+0x184 @0x80050d40/48).
# Dossier analysis/befunde_runde39/K_heliport1171.md.
#
# RIEGEL (Unit, echte RDTs/Archive, Bilder in main.c-Reihenfolge + RBJ-Abfrage des Renderers):
#   unit_r39_heliport1171_banken    Johns Baenke + ROOM1171-RBJ (rec1 Marker 0x2 {20,30,96})
#   unit_r39_heliport1171_binder    Slots ohne Record verdraengen den Record nicht (1171 John, 10D0 Marvin, 1170 Elliot aus)
#   unit_r39_heliport1171_szene     Johns Kanal + Clip-Laenge je Plc_motion-Schritt = Original, "Elza, hurry up!" = 96
#   unit_r39_heliport1171_leon1170  Gegenprobe Leon ROOM1170: keine Bindung ueber den Cache (wie vorher)
add_executable(test_r39_heliport1171 ${CMAKE_CURRENT_LIST_DIR}/../test_r39_heliport1171.c)
target_link_libraries(test_r39_heliport1171 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r39_heliport1171 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_heliport1171 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(test_r39_heliport1171 PRIVATE m)
endif()
foreach(_teil banken binder szene leon1170)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r39_heliport1171_${_teil}_wd)
    add_test(NAME unit_r39_heliport1171_${_teil} COMMAND test_r39_heliport1171 ${_teil})
    set_tests_properties(unit_r39_heliport1171_${_teil} PROPERTIES TIMEOUT 300
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r39_heliport1171_${_teil}_wd)
endforeach()

# INTEGRATION (echte exe, Elza): der Pose-Strom RE15_ANIM_TRACE (anim_select_common.c = die Stelle, an der der Renderer
# die Pose aufloest) traegt die Bildzahl des GERENDERTEN Clips. Johns Gesten muessen aus RBJ rec1 kommen.
#   weg    echter Weg: Spielstand ROOM1131 (Elza, (3,0xC2)=1) vor der Flurtuer Slot 3, Aktionstaste -> ROOM1171, Selbst-Tuer
#          Slot 5 (RE15_FIRE_AOT, derselbe Tuerpfad wie die Aktionstaste) -> Szene -> ROOM10B1
#   laden  Spielstand direkt in ROOM1171 + CONTINUE (Lade-Weg)
foreach(_teil weg laden)
    add_test(NAME integration_r39_heliport1171_${_teil}
        COMMAND ${CMAKE_COMMAND}
            -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
            -DRE15_KARTE_TOOL=$<TARGET_FILE:probe_r36_elza_karte>
            -DWORKDIR=${CMAKE_BINARY_DIR}/r39_heliport1171_wd
            -DTEIL=${_teil}
            -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_heliport1171.cmake)
    set_tests_properties(integration_r39_heliport1171_${_teil} PROPERTIES TIMEOUT 900)
endforeach()
