# Runde 36 (2026-10-12), Nachtrag zu Spur N, Zweig r36/kartenbits — die Lageplan-Bits der Spur N lagen auf
# Bank-9-Bits ORIGINALER Item-Saetze (90 = ROOM2090/2091 Item 0x3F, 91 = ROOM2040/2041 Item 0x32).
# Dossier: analysis/befunde_runde36/N_kartenitems.md Abschnitt 9.
#
#   unit_r36_kartenbits   Z  Zensus aller ausgelieferten RDTs (Laengen = scd_opcode_size_at): Bank-9-Bits des
#                            Originals aus Item_aot_set (u16 pc[18] @0x80040680 / pc[26] @0x80040670), Ck(9,n)
#                            @0x8003fd2c, Set(9,n) @0x8003fe04, FLAG_CHG(9,n) @0x80043140; 0x58/0x59 mit Bank 9
#                            verboten. Gegenprobe: 206 RDTs, 0 Desync, 40674 Opcodes, 164 Item_aot_set, 85 Bits,
#                            bekannte Bits 85/86/90/91/92/52/136/138/21 gefunden.
#                         P  explizite Liste aller Port-Bank-9-Bits (Makros der Koepfe + live k_r36k_lage):
#                            keines im Original-Satz; Gegenprobe alte Spur-N-Belegung 87..91 -> genau {90, 91}.
#                         V  Plan ueber das echte Port-Programm (Ereignis 28, Ja) nehmen -> Item_aot_set in
#                            ROOM2040/2041 @0x01364 und ROOM2090/2091 @0x00A12 bleibt scharf (Taken-Test
#                            @0x800406dc); Gegenprobe Original-Bit -> still; Rueckrichtung kein Besitz.
#                         S  Alt-Spielstaende: Nutzerkarte 2026-09-27 (Fixture r30) ohne 77/78; v0.8.23-Stand (v9)
#                            mit genommenen Items 0x3F/0x32 ueber die Speicherkarte -> kein Plan besessen, Plaene da.
add_executable(test_r36_kartenbits ${CMAKE_CURRENT_LIST_DIR}/../test_r36_kartenbits.c)
target_link_libraries(test_r36_kartenbits PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_kartenbits PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_kartenbits PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_R36KB_NUTZERKARTE="${CMAKE_CURRENT_SOURCE_DIR}/probes/r30_karte_nutzer_2026-09-27.mcr")
if(NOT WIN32)
    target_link_libraries(test_r36_kartenbits PRIVATE m)
endif()
add_test(NAME unit_r36_kartenbits COMMAND test_r36_kartenbits
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r36_kartenbits PROPERTIES TIMEOUT 300)
