# Spur J (Runde 35) — ROOM11C0: Ada versteckt sich / kommt zurueck, Gorilla-Boss (0x27).
# Dossier analysis/befunde_runde35/J_affen.md, Code include/re15_affen.h + engine/src/affen_11c0.c
# (Haken: enemy_ai_common.c NPC-Klemmband +0x82, Flinch-Zaehler, LEAP ohne 245d8; main.c Parts ohne
# Knochen weltfest; emd_common.c EMR-Rohdaten am Skelett).
#
# RIEGEL: Teile teile / band / ada / sprung / flug / wagen / brust / kdsonde / biss / frac / anker / takt / griff / npcband (Beschreibung im Kopf von
# test_r35_affen.c).
add_executable(test_r35_affen ${CMAKE_CURRENT_SOURCE_DIR}/test_r35_affen.c)
target_link_libraries(test_r35_affen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_affen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r35_affen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil teile band ada sprung flug wagen brust kdsonde biss frac anker takt griff npcband)
    add_test(NAME unit_r35_affen_${_teil} COMMAND test_r35_affen ${_teil})
    set_tests_properties(unit_r35_affen_${_teil} PROPERTIES TIMEOUT 240)
endforeach()
