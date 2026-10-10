# Runde 36 / Spur K "szene1150" — Irons-Todesszene / Montage (Dossier analysis/befunde_runde36/K_szene1150.md).
# Code: engine/src/irons_tod_1150.c, include/re15_irons_tod.h, engine/src/gen/knall_bank.inc (tools/r35_l/knall_cut.py).
#
#   test_r36_szene1150   RIEGEL (Teil ueber das Argument):
#     unit_r36_szene1150_knall_re2   K1: Knall-Bank Satz 0 = RE2 ROOM1060 Se_on 0x0a (sub05 @0x00F48), Welle bytegleich
#                                    aus info/re2leon/PL0/RDT/ROOM1060.RDT @0x066B8, zwei Lagen; nicht mehr DOOR04 Door_exit
#     unit_r36_szene1150_folge       K3: ganze Kette ueber die echten Tuer-Anforderungen, je Montage-Schritt
#                                    Knall < Spawn < Kamera (re15_irons_tod_folge), Zombies nach dem Raumaufbau schon da,
#                                    keiner erscheint auf der zeigenden Kamera; Varianten 1140/1070 leer, Tor schon offen
#     unit_r36_szene1150_dialog11c0  K2: Ada und Marvin einander zugewandt (Koerper re15_ai_arc_test Kegel 0x60
#                                    @0x80076c41, Kopf Plc_neck Modus 1 auf den Partner) je Zeile 10..14
add_executable(test_r36_szene1150 ${CMAKE_CURRENT_SOURCE_DIR}/test_r36_szene1150.c)
target_link_libraries(test_r36_szene1150 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r36_szene1150 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r36_szene1150 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_RE2_RDT_DIR="${CMAKE_SOURCE_DIR}/../info/re2leon/PL0/RDT")
foreach(_teil knall_re2 folge dialog11c0)
    add_test(NAME unit_r36_szene1150_${_teil} COMMAND test_r36_szene1150 ${_teil})
    set_tests_properties(unit_r36_szene1150_${_teil} PROPERTIES TIMEOUT 300)
endforeach()
