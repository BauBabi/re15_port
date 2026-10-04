# Spur J (Runde 35) — ROOM11C0: Ada versteckt sich / kommt zurueck, Gorilla-Boss (0x27).
# Dossier analysis/befunde_runde35/J_affen.md, Code include/re15_affen.h + engine/src/affen_11c0.c
# (Haken je 1-2 Zeilen: enemy_ai_common.c NPC-Klemmband +0x82, Treffer-Zaehler Spur 0/1/2, LEAP ohne 245d8,
# +0x8f-Abbau, Fuss-Sperre als Abfrage, Pin-Anker, Pin-Variante vor dem Yaw-Latch, Spawn-Wurzel 0x27;
# game_step_common.c Knockdown-Sonde; main.c Part 18 am Rumpf / Parts ohne Knochen weltfest; emd_common.c
# EMR-Rohdaten am Skelett). Die Riegel takt/griff/schrot messen gegen GDB-Einzelbild-Spuren des Originals.
#
# RIEGEL: Teile teile / band / ada / sprung / flug / wagen / brust / kdsonde / biss / frac / anker / takt / griff / npcband / schrot /
# wand / szene / finisher / zonensprung / kette (Beschreibung im Kopf von test_r35_affen.c; Nachbesserung 6: finisher = cmd-6-Hook
# 0x8011c414, zonensprung = Blind-Zonen-Schleife gegen die GDB-Spur des Originals, kette = Elternketten gegen FUN_8011bf50/c024).
add_executable(test_r35_affen ${CMAKE_CURRENT_SOURCE_DIR}/test_r35_affen.c)
target_link_libraries(test_r35_affen PRIVATE re15_engine re15_test_support)
target_include_directories(test_r35_affen PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r35_affen PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
foreach(_teil teile band ada sprung flug wagen brust kdsonde biss frac anker takt griff npcband schrot wand szene finisher zonensprung kette)
    add_test(NAME unit_r35_affen_${_teil} COMMAND test_r35_affen ${_teil})
    set_tests_properties(unit_r35_affen_${_teil} PROPERTIES TIMEOUT 240)
endforeach()
