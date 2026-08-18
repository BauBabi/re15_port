# =============================================================================
# Unit-Tests — Einzelne Parser/Module isoliert testen
# =============================================================================
# Jede Testdatei wird als eigenständiges Executable gebaut und bei CTest
# registriert. Alle Tests linken gegen re15_engine (statische Lib).
# =============================================================================

# --- RDT-Parser Unit-Test (Requirement 10.1, 10.8, 12.1) ---
add_executable(test_rdt_parser test_rdt_parser.c)
target_link_libraries(test_rdt_parser PRIVATE re15_engine re15_test_support)
target_include_directories(test_rdt_parser PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_rdt_parser COMMAND test_rdt_parser)
set_tests_properties(unit_rdt_parser PROPERTIES TIMEOUT 30)

# --- SCA-Parser Unit-Test ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_sca_parse.c")
    add_executable(test_sca_parser test_sca_parse.c)
    target_link_libraries(test_sca_parser PRIVATE re15_engine re15_test_support)
    target_include_directories(test_sca_parser PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME unit_sca_parser COMMAND test_sca_parser)
    set_tests_properties(unit_sca_parser PROPERTIES TIMEOUT 30)
endif()

# --- SCD-Opcodes Unit-Test (Requirement 4.6, 12.4) ---
add_executable(test_scd_opcodes test_scd_opcodes.c)
target_link_libraries(test_scd_opcodes PRIVATE re15_engine re15_test_support)
target_include_directories(test_scd_opcodes PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_scd_opcodes COMMAND test_scd_opcodes)
set_tests_properties(unit_scd_opcodes PROPERTIES TIMEOUT 30)

# --- Fixkomma-Mathematik Unit-Test (Requirement 5.3, 11.2) ---
add_executable(test_math_common test_math_common.c)
target_link_libraries(test_math_common PRIVATE re15_engine re15_test_support)
target_include_directories(test_math_common PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_math_common PRIVATE m)
add_test(NAME unit_math_common COMMAND test_math_common)
set_tests_properties(unit_math_common PROPERTIES TIMEOUT 30)

# --- Room-Load Unit-Test (Requirement 10.1, 12.2) ---
add_executable(test_room_load test_room_load.c)
target_link_libraries(test_room_load PRIVATE re15_engine re15_test_support)
target_include_directories(test_room_load PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_room_load COMMAND test_room_load)
set_tests_properties(unit_room_load PROPERTIES TIMEOUT 30)

# --- Member_set/cmp Unit-Test (#11/#12 — byte-true RE1.5 id-Tabelle FUN_8004116c/FUN_80041358) ---
add_executable(test_member test_member.c)
target_link_libraries(test_member PRIVATE re15_engine re15_test_support)
target_include_directories(test_member PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_member COMMAND test_member)
set_tests_properties(unit_member PROPERTIES TIMEOUT 30)

# --- Player-Damage Unit-Test (#13 — byte-true Player-Branch von FUN_80012d60) ---
add_executable(test_damage test_damage.c)
target_link_libraries(test_damage PRIVATE re15_engine re15_test_support)
target_include_directories(test_damage PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_damage COMMAND test_damage)
set_tests_properties(unit_damage PROPERTIES TIMEOUT 30)

# --- Enemy-AI-FSM Unit-Test (Phase 2 — byte-true FUN_8011d6d4/d84c/d9f4 + 80101b64/c7c/de4) ---
add_executable(test_enemy_ai test_enemy_ai.c)
target_link_libraries(test_enemy_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_enemy_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_enemy_ai COMMAND test_enemy_ai)
set_tests_properties(unit_enemy_ai PROPERTIES TIMEOUT 30)

# --- ROOM1140 briefing-zombie spawn (Phase 8.2 — byte-true Sce_em_set roster via real RDT) ---
add_executable(test_room1140_spawn test_room1140_spawn.c)
target_link_libraries(test_room1140_spawn PRIVATE re15_engine re15_test_support)
target_include_directories(test_room1140_spawn PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_room1140_spawn PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_room1140_spawn COMMAND test_room1140_spawn)
set_tests_properties(unit_room1140_spawn PROPERTIES TIMEOUT 30)

# --- ROOM1140 live-AI game_step wiring (Phase 8.6 — re15_enemy_ai_run_all type-gated, real RDT) ---
add_executable(test_room1140_combat test_room1140_combat.c)
target_link_libraries(test_room1140_combat PRIVATE re15_engine re15_test_support)
target_include_directories(test_room1140_combat PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_room1140_combat PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_room1140_combat COMMAND test_room1140_combat)
set_tests_properties(unit_room1140_combat PROPERTIES TIMEOUT 30)

# --- Dog (type 0x20) chase/bite AI — Wave 1 port probe (re15_dog_ai_tick in enemy_ai_common.c) ---
add_executable(test_dog_ai test_dog_ai.c)
target_link_libraries(test_dog_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_dog_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_dog_ai COMMAND test_dog_ai)
set_tests_properties(unit_dog_ai PROPERTIES TIMEOUT 30)

# --- Crow (type 0x21) 3D-flight AI — Wave 1 port probe (re15_crow_ai_tick in enemy_ai_common.c) ---
add_executable(test_crow_ai test_crow_ai.c)
target_link_libraries(test_crow_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_crow_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_crow_ai COMMAND test_crow_ai)
set_tests_properties(unit_crow_ai PROPERTIES TIMEOUT 30)

# --- Spider-Baby (type 0x26) stationary web-spitter AI — Wave 1 (re15_spider_ai_tick) ---
add_executable(test_spider_ai test_spider_ai.c)
target_link_libraries(test_spider_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_spider_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_spider_ai COMMAND test_spider_ai)
set_tests_properties(unit_spider_ai PROPERTIES TIMEOUT 30)

# --- Maggots (type 0x27) large moving ground creature AI — Wave 1 (re15_maggot_ai_tick) ---
add_executable(test_maggot_ai test_maggot_ai.c)
target_link_libraries(test_maggot_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_maggot_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_maggot_ai PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_maggot_ai COMMAND test_maggot_ai)
set_tests_properties(unit_maggot_ai PROPERTIES TIMEOUT 30)

add_executable(test_adult_spider_ai test_adult_spider_ai.c)
target_link_libraries(test_adult_spider_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_adult_spider_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_adult_spider_ai COMMAND test_adult_spider_ai)
set_tests_properties(unit_adult_spider_ai PROPERTIES TIMEOUT 30)

add_executable(test_cockroach_ai test_cockroach_ai.c)
target_link_libraries(test_cockroach_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_cockroach_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_cockroach_ai COMMAND test_cockroach_ai)
set_tests_properties(unit_cockroach_ai PROPERTIES TIMEOUT 30)

# --- NPC (Chief Irons, type 0x40) invulnerable cutscene actor AI — Wave 1 (re15_npc_ai_tick) ---
add_executable(test_npc_ai test_npc_ai.c)
target_link_libraries(test_npc_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_npc_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_npc_ai COMMAND test_npc_ai)
set_tests_properties(unit_npc_ai PROPERTIES TIMEOUT 30)

# --- NPC SCD-owns-animation yield — regression guard for 4cf6bead (byte-true gate FUN_8011c654) ---
add_executable(test_npc_scd_pose test_npc_scd_pose.c)
target_link_libraries(test_npc_scd_pose PRIVATE re15_engine re15_test_support)
target_include_directories(test_npc_scd_pose PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_npc_scd_pose COMMAND test_npc_scd_pose)
set_tests_properties(unit_npc_scd_pose PROPERTIES TIMEOUT 30)

# --- Zombie Girl (type 0x13, EM013) nav-pathing zombie variant AI — Wave 1 (re15_zgirl_ai_tick) ---
add_executable(test_zgirl_ai test_zgirl_ai.c)
target_link_libraries(test_zgirl_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_zgirl_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_zgirl_ai COMMAND test_zgirl_ai)
set_tests_properties(unit_zgirl_ai PROPERTIES TIMEOUT 30)

# --- EMS enemy-model archive index (re15_ems.c — load enemy EMDs from CDEMD*.EMS) ---
add_executable(test_ems test_ems.c)
target_link_libraries(test_ems PRIVATE re15_engine re15_test_support)
target_include_directories(test_ems PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_ems PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_ems COMMAND test_ems)
set_tests_properties(unit_ems PROPERTIES TIMEOUT 30)

# --- ESP effect-sprite section parse (Phase ESP-A — re15_esp.c, byte-true vs real ROOM1140.RDT) ---
add_executable(test_esp_parse test_esp_parse.c)
target_link_libraries(test_esp_parse PRIVATE re15_engine re15_test_support)
target_include_directories(test_esp_parse PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_esp_parse PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_esp_parse COMMAND test_esp_parse)
set_tests_properties(unit_esp_parse PROPERTIES TIMEOUT 30)

# --- Audio VAB-Parsing Unit-Test (Requirement 6.6, 12.1) ---
add_executable(test_audio_vab test_audio_vab.c)
target_include_directories(test_audio_vab PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_audio_vab COMMAND test_audio_vab)
set_tests_properties(unit_audio_vab PROPERTIES TIMEOUT 30)

# --- Inventory / weapon-select menu LOGIC (Phase 8.20 — menu_common.c) ---
add_executable(test_weapon_select test_weapon_select.c)
target_link_libraries(test_weapon_select PRIVATE re15_engine re15_test_support)
target_include_directories(test_weapon_select PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_weapon_select COMMAND test_weapon_select)
set_tests_properties(unit_weapon_select PROPERTIES TIMEOUT 30)

# --- Wave 2: the byte-true status-screen FSM (open stages / tab FSM / ITEM slides ±14 /
#     grid nav w/ auto-repeat 15/4 / command stage / close commit) — shots/inv_wave2_spec.md ---
add_executable(test_inv_fsm test_inv_fsm.c)
target_link_libraries(test_inv_fsm PRIVATE re15_engine re15_test_support)
target_include_directories(test_inv_fsm PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_inv_fsm COMMAND test_inv_fsm)
set_tests_properties(unit_inv_fsm PROPERTIES TIMEOUT 30)

# --- Inventory item ICON decode (Phase 8.22 — item_icon_common.c, byte-true ITEMALL.PIX) ---
add_executable(test_item_icon test_item_icon.c)
target_link_libraries(test_item_icon PRIVATE re15_engine re15_test_support)
target_include_directories(test_item_icon PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_item_icon COMMAND test_item_icon)
set_tests_properties(unit_item_icon PROPERTIES TIMEOUT 30)

# --- Zombie AWARE-WALK Pose-Oracle (P1 regression: em10 loco clip2/kf97 == live PSX hunch) ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_zombie_pose.c")
    add_executable(test_zombie_pose test_zombie_pose.c)
    target_link_libraries(test_zombie_pose PRIVATE re15_engine re15_test_support)
    target_include_directories(test_zombie_pose PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME unit_zombie_pose COMMAND test_zombie_pose WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_tests_properties(unit_zombie_pose PROPERTIES TIMEOUT 30)
endif()

add_executable(test_birkin_ai test_birkin_ai.c)
target_link_libraries(test_birkin_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_birkin_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_birkin_ai COMMAND test_birkin_ai)
set_tests_properties(unit_birkin_ai PROPERTIES TIMEOUT 30)

add_executable(test_writher_ai test_writher_ai.c)
target_link_libraries(test_writher_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_writher_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_writher_ai COMMAND test_writher_ai)
set_tests_properties(unit_writher_ai PROPERTIES TIMEOUT 30)

add_executable(test_alligator_ai test_alligator_ai.c)
target_link_libraries(test_alligator_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_alligator_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_alligator_ai COMMAND test_alligator_ai)
set_tests_properties(unit_alligator_ai PROPERTIES TIMEOUT 30)

add_executable(test_fx_emitter_ai test_fx_emitter_ai.c)
target_link_libraries(test_fx_emitter_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_fx_emitter_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_fx_emitter_ai COMMAND test_fx_emitter_ai)
set_tests_properties(unit_fx_emitter_ai PROPERTIES TIMEOUT 30)

add_executable(test_tyrant_ai test_tyrant_ai.c)
target_link_libraries(test_tyrant_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_tyrant_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_tyrant_ai COMMAND test_tyrant_ai)
set_tests_properties(unit_tyrant_ai PROPERTIES TIMEOUT 30)

add_executable(test_ivy_ai test_ivy_ai.c)
target_link_libraries(test_ivy_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_ivy_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_ivy_ai COMMAND test_ivy_ai)
set_tests_properties(unit_ivy_ai PROPERTIES TIMEOUT 30)

add_executable(test_enemy_shootable test_enemy_shootable.c)
target_link_libraries(test_enemy_shootable PRIVATE re15_engine re15_test_support)
target_include_directories(test_enemy_shootable PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_enemy_shootable COMMAND test_enemy_shootable)
set_tests_properties(unit_enemy_shootable PROPERTIES TIMEOUT 30)

add_executable(test_aot_edge test_aot_edge.c)
target_link_libraries(test_aot_edge PRIVATE re15_engine re15_test_support)
target_include_directories(test_aot_edge PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_aot_edge COMMAND test_aot_edge)
set_tests_properties(unit_aot_edge PROPERTIES TIMEOUT 30)

add_executable(test_enemy_body_push test_enemy_body_push.c)
target_link_libraries(test_enemy_body_push PRIVATE re15_engine re15_test_support)
target_include_directories(test_enemy_body_push PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_enemy_body_push PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_enemy_body_push COMMAND test_enemy_body_push)
set_tests_properties(unit_enemy_body_push PROPERTIES TIMEOUT 30)

# --- SCD AOT opcodes (Cut_chg/Aot_set/Sce_em_set/Item_aot_set/Door_aot_set) — was orphaned (never
#     registered); re-added, incl. the Item_aot_set LE type/amount regression (audit wf_6fb46a73) ---
add_executable(test_scd_aot_opcodes test_scd_aot_opcodes.c)
target_link_libraries(test_scd_aot_opcodes PRIVATE re15_engine re15_test_support)
target_include_directories(test_scd_aot_opcodes PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_scd_aot_opcodes COMMAND test_scd_aot_opcodes)
set_tests_properties(unit_scd_aot_opcodes PROPERTIES TIMEOUT 30)

# --- Byte-true SCA diagonal/slope push-out types 2/4/5/6/7 (was DEFERRED — walk through walls) ---
add_executable(test_slope_collision test_slope_collision.c)
target_link_libraries(test_slope_collision PRIVATE re15_engine re15_test_support)
target_include_directories(test_slope_collision PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_slope_collision COMMAND test_slope_collision)
set_tests_properties(unit_slope_collision PROPERTIES TIMEOUT 30)

# --- Byte-true auto-aim latch is ALL-AROUND (FUN_8003703c has no angle gate; the front-arc gate was a divergence) ---
add_executable(test_aim_all_around test_aim_all_around.c)
target_link_libraries(test_aim_all_around PRIVATE re15_engine re15_test_support)
target_include_directories(test_aim_all_around PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_aim_all_around COMMAND test_aim_all_around)
set_tests_properties(unit_aim_all_around PROPERTIES TIMEOUT 30)

# --- Regression: aim LOWER exit must not flash the injured-idle sentinel (the "land" bug) ---
add_executable(test_aim_lower_exit test_aim_lower_exit.c)
target_link_libraries(test_aim_lower_exit PRIVATE re15_engine re15_test_support)
target_include_directories(test_aim_lower_exit PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_aim_lower_exit COMMAND test_aim_lower_exit)
set_tests_properties(unit_aim_lower_exit PROPERTIES TIMEOUT 30)

# --- Byte-true player HURT reaction: directional clip 0x8/0x9 + backward knockback (FUN_80035af0) ---
add_executable(test_hit_reaction test_hit_reaction.c)
target_link_libraries(test_hit_reaction PRIVATE re15_engine re15_test_support)
target_include_directories(test_hit_reaction PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_hit_reaction COMMAND test_hit_reaction)
set_tests_properties(unit_hit_reaction PROPERTIES TIMEOUT 30)

# --- Byte-true SCD Se_on VAB-bank selector (FUN_80045024: arg>>24 picks the bank; was ignored) ---
add_executable(test_se_bank_routing test_se_bank_routing.c)
target_include_directories(test_se_bank_routing PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_se_bank_routing COMMAND test_se_bank_routing)
set_tests_properties(unit_se_bank_routing PROPERTIES TIMEOUT 30)

# --- Byte-true SCD For2 (0x1B): For loop with a work-variable count (was unmapped -> op_unknown) ---
add_executable(test_scd_for2 test_scd_for2.c)
target_link_libraries(test_scd_for2 PRIVATE re15_engine re15_test_support)
target_include_directories(test_scd_for2 PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_scd_for2 COMMAND test_scd_for2)
set_tests_properties(unit_scd_for2 PROPERTIES TIMEOUT 30)

# --- Byte-true stair TURN-to-face heading settle (LAB_80037fd8: clip-5 turn preamble, was instant snap) ---

# --- Byte-true bleed/poison roll consumes TWO RNG draws (was short-circuited by C && -> shared RNG desync) ---
add_executable(test_bleed_rng test_bleed_rng.c)
target_link_libraries(test_bleed_rng PRIVATE re15_engine re15_test_support)
target_include_directories(test_bleed_rng PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_bleed_rng COMMAND test_bleed_rng)
set_tests_properties(unit_bleed_rng PROPERTIES TIMEOUT 30)

# --- Zombie knockdown GET-UP clip must advance, not freeze (audit ANIM-1: dual-use clip 0x12 pin) ---
add_executable(test_getup_advance test_getup_advance.c)
target_link_libraries(test_getup_advance PRIVATE re15_engine re15_test_support)
target_include_directories(test_getup_advance PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_getup_advance COMMAND test_getup_advance)
set_tests_properties(unit_getup_advance PROPERTIES TIMEOUT 30)

# --- RVD CAM_SWITCH scan picks the FIRST table zone on overlap (audit rvd-scan-order: reversed scan) ---
add_executable(test_rvd_scan_order test_rvd_scan_order.c)
target_link_libraries(test_rvd_scan_order PRIVATE re15_engine re15_test_support)
target_include_directories(test_rvd_scan_order PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_rvd_scan_order COMMAND test_rvd_scan_order)
set_tests_properties(unit_rvd_scan_order PROPERTIES TIMEOUT 30)

# --- Byte-true SCD Dir_set (0x33): 3 axes to the work entity, ignores pc[1] (was player-only rot_y) ---
add_executable(test_scd_dir_set test_scd_dir_set.c)
target_link_libraries(test_scd_dir_set PRIVATE re15_engine re15_test_support)
target_include_directories(test_scd_dir_set PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_scd_dir_set COMMAND test_scd_dir_set)
set_tests_properties(unit_scd_dir_set PROPERTIES TIMEOUT 30)

# --- Byte-true enemy-status kill-flag persistence: killed enemies don't respawn on room re-entry ---
add_executable(test_em_status_persist test_em_status_persist.c)
target_link_libraries(test_em_status_persist PRIVATE re15_engine re15_test_support)
target_include_directories(test_em_status_persist PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_em_status_persist COMMAND test_em_status_persist)
set_tests_properties(unit_em_status_persist PROPERTIES TIMEOUT 30)

# --- Byte-true per-actor SCA wall clamp: enemy solid-mask 4 + own radius (was player 1/450 for all) ---
add_executable(test_collision_layers test_collision_layers.c)
target_link_libraries(test_collision_layers PRIVATE re15_engine re15_test_support)
target_include_directories(test_collision_layers PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_collision_layers COMMAND test_collision_layers)
set_tests_properties(unit_collision_layers PROPERTIES TIMEOUT 30)

# --- Byte-true pose-then-advance ordering + 0x8000 marker skip (skeleton/anim audit wf_8a6bd306) ---
add_executable(test_anim_pose_order test_anim_pose_order.c)
target_link_libraries(test_anim_pose_order PRIVATE re15_engine re15_test_support)
target_include_directories(test_anim_pose_order PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_anim_pose_order COMMAND test_anim_pose_order)
set_tests_properties(unit_anim_pose_order PROPERTIES TIMEOUT 30)

# --- Byte-true ADPCM END-flag/shift-clamp + EDT layering/water mask (audio audit wf_1db9c802) ---
add_executable(test_audio_decode test_audio_decode.c)
target_link_libraries(test_audio_decode PRIVATE re15_engine re15_test_support)
target_include_directories(test_audio_decode PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_audio_decode COMMAND test_audio_decode)
set_tests_properties(unit_audio_decode PROPERTIES TIMEOUT 30)

add_executable(test_xa_decode test_xa_decode.c)
target_link_libraries(test_xa_decode PRIVATE re15_engine re15_test_support)
target_include_directories(test_xa_decode PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_xa_decode COMMAND test_xa_decode)
set_tests_properties(unit_xa_decode PROPERTIES TIMEOUT 30)

add_executable(test_savedata test_savedata.c)
target_link_libraries(test_savedata PRIVATE re15_engine re15_test_support)
target_include_directories(test_savedata PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_savedata COMMAND test_savedata)
set_tests_properties(unit_savedata PROPERTIES TIMEOUT 30)

add_executable(test_savepoint test_savepoint.c)
target_link_libraries(test_savepoint PRIVATE re15_engine re15_test_support)
target_include_directories(test_savepoint PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_savepoint COMMAND test_savepoint)
set_tests_properties(unit_savepoint PROPERTIES TIMEOUT 30)

# --- Byte-true ACTION-Zustands-Gate (Examine/Tuer nur cmd-1-Substates 0-4, DECIDE-Tabelle
#     @0x80073fb0) + Live-FF-Kadenz durch die echte game_step-Pad-Verkabelung (Reports A+B) ---
add_executable(test_action_msg_gate test_action_msg_gate.c)
target_link_libraries(test_action_msg_gate PRIVATE re15_engine re15_test_support)
target_include_directories(test_action_msg_gate PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_action_msg_gate PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_action_msg_gate COMMAND test_action_msg_gate)
set_tests_properties(unit_action_msg_gate PROPERTIES TIMEOUT 30)

# --- Byte-true dialog FSM buttons + cadence (msg audit wf_6aad95ad) ---
add_executable(test_msg_dialog test_msg_dialog.c)
target_link_libraries(test_msg_dialog PRIVATE re15_engine re15_test_support)
target_include_directories(test_msg_dialog PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_msg_dialog COMMAND test_msg_dialog)
set_tests_properties(unit_msg_dialog PROPERTIES TIMEOUT 30)


# --- Byte-true SCD 0x59 indexed flag write + 0x57 fade step (were mis-named Xa_on/Sce_bgmtbl_set) ---
add_executable(test_scd_flag_fade test_scd_flag_fade.c)
target_link_libraries(test_scd_flag_fade PRIVATE re15_engine re15_test_support)
target_include_directories(test_scd_flag_fade PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_scd_flag_fade COMMAND test_scd_flag_fade)
set_tests_properties(unit_scd_flag_fade PROPERTIES TIMEOUT 30)

# --- Flag store -> door/event gate (roadmap S1-4 / PROG-2) ---
add_executable(test_flag_gate test_flag_gate.c)
target_link_libraries(test_flag_gate PRIVATE re15_engine re15_test_support)
target_include_directories(test_flag_gate PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_flag_gate COMMAND test_flag_gate)
set_tests_properties(unit_flag_gate PROPERTIES TIMEOUT 30)

add_executable(test_scd_key_ck test_scd_key_ck.c)
target_link_libraries(test_scd_key_ck PRIVATE re15_engine re15_test_support)
target_include_directories(test_scd_key_ck PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_scd_key_ck COMMAND test_scd_key_ck)
set_tests_properties(unit_scd_key_ck PROPERTIES TIMEOUT 30)

# --- Byte-true RE1.5 camera LookAt (audit #3 — integer SquareRoot0 + division, no float/rounding) ---
add_executable(test_camera_view test_camera_view.c)
target_link_libraries(test_camera_view PRIVATE re15_engine re15_test_support)
target_include_directories(test_camera_view PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_camera_view COMMAND test_camera_view)
set_tests_properties(unit_camera_view PROPERTIES TIMEOUT 30)

# --- Byte-true RE1.5 RotMatrix (audit #1 — per-product truncation + negate-before-shift, pixel-shift) ---
add_executable(test_rotmatrix test_rotmatrix.c)
target_link_libraries(test_rotmatrix PRIVATE re15_engine re15_test_support)
target_include_directories(test_rotmatrix PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_rotmatrix COMMAND test_rotmatrix)
set_tests_properties(unit_rotmatrix PROPERTIES TIMEOUT 30)

# --- Byte-true world-item pickup INSERT (no auto-stack, wide-weapon front-shift; audit wf_6eea7fa1) ---
add_executable(test_inv_grant test_inv_grant.c)
target_link_libraries(test_inv_grant PRIVATE re15_engine re15_test_support)
target_include_directories(test_inv_grant PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_inv_grant COMMAND test_inv_grant)
set_tests_properties(unit_inv_grant PROPERTIES TIMEOUT 30)

# --- Byte-true PsyQ BIOS catan CORDIC (0x800658fc) + walker bearing (FUN_8001a6d4; was a LUT approx) ---
add_executable(test_catan test_catan.c)
target_link_libraries(test_catan PRIVATE re15_engine re15_test_support)
target_include_directories(test_catan PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_catan COMMAND test_catan)
set_tests_properties(unit_catan PROPERTIES TIMEOUT 30)

# --- Byte-true PsyQ BIOS SquareRoot0 table replica (0x80065f60 — the engine's ONLY sqrt, NOT floor) ---
add_executable(test_squareroot0 test_squareroot0.c)
target_link_libraries(test_squareroot0 PRIVATE re15_engine re15_test_support)
target_include_directories(test_squareroot0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_squareroot0 COMMAND test_squareroot0)
set_tests_properties(unit_squareroot0 PROPERTIES TIMEOUT 30)

# --- Melee reach through the body-push standoff (audit wf_555f18eb Part B — the "harmless enemy" bug) ---
add_executable(test_enemy_attack_reach test_enemy_attack_reach.c)
target_link_libraries(test_enemy_attack_reach PRIVATE re15_engine re15_test_support)
target_include_directories(test_enemy_attack_reach PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_enemy_attack_reach PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_enemy_attack_reach COMMAND test_enemy_attack_reach)
set_tests_properties(unit_enemy_attack_reach PROPERTIES TIMEOUT 30)

# --- Byte-true inventory item-USE (heal) flow (wf_13443da6): "Will you use the X?" -> heal + consume ---
add_executable(test_item_use test_item_use.c)
target_link_libraries(test_item_use PRIVATE re15_engine re15_test_support)
target_include_directories(test_item_use PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_item_use COMMAND test_item_use)
set_tests_properties(unit_item_use PROPERTIES TIMEOUT 30)

# --- Byte-true item-get "You got X" pickup MODAL (FUN_8001db28): zoom/flip FSM + deferred grant (U11) ---
add_executable(test_item_modal test_item_modal.c)
target_link_libraries(test_item_modal PRIVATE re15_engine re15_test_support)
target_include_directories(test_item_modal PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_item_modal COMMAND test_item_modal)
set_tests_properties(unit_item_modal PROPERTIES TIMEOUT 30)

# --- ROOM1150 item box = message-only AOT (inv_plan gap 18, wave 6): slot-5 Aot_set -> sub07 msg 3 ---
add_executable(test_room1150_itembox test_room1150_itembox.c)
target_link_libraries(test_room1150_itembox PRIVATE re15_engine re15_test_support)
target_include_directories(test_room1150_itembox PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_room1150_itembox PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_room1150_itembox COMMAND test_room1150_itembox)
set_tests_properties(unit_room1150_itembox PROPERTIES TIMEOUT 30)

# --- ITEM BOX (RE1.5-hybrid): RE2 FUN_800703b8 transfer engine, table-driven over the
#     14-quirk catalog (shots/itembox_spec.md §3/§7) + savedata v4 box round-trip/upgrade ---
add_executable(test_itembox_transfer test_itembox_transfer.c)
target_link_libraries(test_itembox_transfer PRIVATE re15_engine re15_test_support)
target_include_directories(test_itembox_transfer PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_itembox_transfer COMMAND test_itembox_transfer)
set_tests_properties(unit_itembox_transfer PROPERTIES TIMEOUT 30)

# --- ITEM BOX screen FSM (menu substate 4): side split / EXIT row / page flip / swap /
#     reject state-5 wait / shared close phase (shots/itembox_spec.md §2.3-2.4 + §6) ---
add_executable(test_itembox_screen test_itembox_screen.c)
target_link_libraries(test_itembox_screen PRIVATE re15_engine re15_test_support)
target_include_directories(test_itembox_screen PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_itembox_screen COMMAND test_itembox_screen)
set_tests_properties(unit_itembox_screen PROPERTIES TIMEOUT 30)

# --- Wave-6 finding 4: VIRTUAL pad-word builder (FUN_80030444, preset-0 table @0x80073dbc) ---
add_executable(test_pad_remap test_pad_remap.c)
target_link_libraries(test_pad_remap PRIVATE re15_engine re15_test_support)
target_include_directories(test_pad_remap PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_pad_remap COMMAND test_pad_remap)
set_tests_properties(unit_pad_remap PROPERTIES TIMEOUT 30)

# --- AOT sce-dispatch census fixes (aot_sce_census d7376834): sce-0 inert installs +
#     Aot_reset full retype + Aot_on fire-now + sce-5 object/enemy-pool marker latch ---
add_executable(test_aot_sce_census test_aot_sce_census.c)
target_link_libraries(test_aot_sce_census PRIVATE re15_engine re15_test_support)
target_include_directories(test_aot_sce_census PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_aot_sce_census COMMAND test_aot_sce_census)
set_tests_properties(unit_aot_sce_census PROPERTIES TIMEOUT 30)

# --- RE2-Zombie-KI (OPTIONS -> AI -> RE2) Unit-Test ---
add_executable(test_re2_zombie_ai test_re2_zombie_ai.c)
target_link_libraries(test_re2_zombie_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_re2_zombie_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_re2_zombie_ai COMMAND test_re2_zombie_ai)
set_tests_properties(unit_re2_zombie_ai PROPERTIES TIMEOUT 30)

# --- WELLE B: RE2-Zombie-Brain A/B im echten ROOM1140 (headless, deterministisch) ---
add_executable(test_re2_room1140_ab test_re2_room1140_ab.c)
target_link_libraries(test_re2_room1140_ab PRIVATE re15_engine re15_test_support)
target_include_directories(test_re2_room1140_ab PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_re2_room1140_ab PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_re2_room1140_ab COMMAND test_re2_room1140_ab)
set_tests_properties(unit_re2_room1140_ab PROPERTIES TIMEOUT 60)

# --- WELLE C: RE2-Cerberus-KI (OPTIONS -> AI -> RE2) Unit-Test ---
add_executable(test_re2_dog_ai test_re2_dog_ai.c)
target_link_libraries(test_re2_dog_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_re2_dog_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_re2_dog_ai COMMAND test_re2_dog_ai)
set_tests_properties(unit_re2_dog_ai PROPERTIES TIMEOUT 30)

# --- WELLE C: RE2-Cerberus-Brain A/B im echten ROOM1190 (headless, deterministisch) ---
add_executable(test_re2_room1190_ab test_re2_room1190_ab.c)
target_link_libraries(test_re2_room1190_ab PRIVATE re15_engine re15_test_support)
target_include_directories(test_re2_room1190_ab PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_re2_room1190_ab PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_re2_room1190_ab COMMAND test_re2_room1190_ab)
set_tests_properties(unit_re2_room1190_ab PROPERTIES TIMEOUT 60)

# --- WELLE D: RE2-Kraehen-KI (OPTIONS -> AI -> RE2) Unit-Test ---
add_executable(test_re2_crow_ai test_re2_crow_ai.c)
target_link_libraries(test_re2_crow_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_re2_crow_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_re2_crow_ai COMMAND test_re2_crow_ai)
set_tests_properties(unit_re2_crow_ai PROPERTIES TIMEOUT 30)

# --- WELLE D: RE2-Kraehen-Brain A/B im echten ROOM10C0 (headless, deterministisch) ---
add_executable(test_re2_room10c0_ab test_re2_room10c0_ab.c)
target_link_libraries(test_re2_room10c0_ab PRIVATE re15_engine re15_test_support)
target_include_directories(test_re2_room10c0_ab PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_re2_room10c0_ab PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_re2_room10c0_ab COMMAND test_re2_room10c0_ab)
set_tests_properties(unit_re2_room10c0_ab PROPERTIES TIMEOUT 60)

# --- Blut-Decal-Wund-Akkumulator (Dossier analysis/blood_decals.md BD-3/BD-5) ---
add_executable(test_wound_accumulator test_wound_accumulator.c)
target_link_libraries(test_wound_accumulator PRIVATE re15_engine re15_test_support)
target_include_directories(test_wound_accumulator PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_wound_accumulator COMMAND test_wound_accumulator)
set_tests_properties(unit_wound_accumulator PROPERTIES TIMEOUT 30)

# --- SE-Tone-Pitch note2pitch2 (Dossier analysis/rolltor_sound.md D1) ---
add_executable(test_se_pitch test_se_pitch.c)
target_link_libraries(test_se_pitch PRIVATE re15_engine re15_test_support)
target_include_directories(test_se_pitch PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_se_pitch COMMAND test_se_pitch)
set_tests_properties(unit_se_pitch PROPERTIES TIMEOUT 30)

# --- Kraehen-KI ROOM1170 (Dossier analysis/crow_1170.md D1/D5) ---
add_executable(test_crow_fixes test_crow_fixes.c)
target_link_libraries(test_crow_fixes PRIVATE re15_engine re15_test_support)
target_include_directories(test_crow_fixes PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_crow_fixes COMMAND test_crow_fixes)
set_tests_properties(unit_crow_fixes PROPERTIES TIMEOUT 30)

# --- Zombie-Hit-Reaktion ROOM1140 (Dossier analysis/zombie_hit_1140.md D1/D2/D5/D6) ---
add_executable(test_zombie_hit_fixes test_zombie_hit_fixes.c)
target_link_libraries(test_zombie_hit_fixes PRIVATE re15_engine re15_test_support)
target_include_directories(test_zombie_hit_fixes PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_zombie_hit_fixes COMMAND test_zombie_hit_fixes)
set_tests_properties(unit_zombie_hit_fixes PROPERTIES TIMEOUT 30)

# --- DIAGNOSE-Probe (kein ctest-Eintrag): Biss-/Release-/Devour-Blut-Spawns ---
add_executable(probe_bite_fx probe_bite_fx.c)
target_link_libraries(probe_bite_fx PRIVATE re15_engine re15_test_support)
target_include_directories(probe_bite_fx PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest-Eintrag): ROOM1170-Kraehen grid/armed/Attack-Erreichbarkeit ---
add_executable(probe_crow_1170 probe_crow_1170.c)
target_link_libraries(probe_crow_1170 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_crow_1170 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest-Eintrag): Zombie-Treffer-Timeline ---
add_executable(probe_zombie_hit probe_zombie_hit.c)
target_link_libraries(probe_zombie_hit PRIVATE re15_engine re15_test_support)
target_include_directories(probe_zombie_hit PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest-Eintrag): ROOM10A0-Treppen-Richtungswahl ---
add_executable(probe_stair_10a0 probe_stair_10a0.c)
target_link_libraries(probe_stair_10a0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_stair_10a0 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest-Eintrag): ROOM1060 letzte Treppe Kamera-Sweep ---
add_executable(probe_stair_cam_1060 probe_stair_cam_1060.c)
target_link_libraries(probe_stair_cam_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_stair_cam_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest-Eintrag): Treppenhaus-Kamera-Kette (10A0/1060) ---
add_executable(probe_stair_cam_chain probe_stair_cam_chain.c)
target_link_libraries(probe_stair_cam_chain PRIVATE re15_engine re15_test_support)
target_include_directories(probe_stair_cam_chain PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest-Eintrag): ROOM10A0-Treppenhaus Kamera-Umschaltung ---
add_executable(probe_stair_cam_10a0 probe_stair_cam_10a0.c)
target_link_libraries(probe_stair_cam_10a0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_stair_cam_10a0 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- ROOM1170 Elliot-Intro-Run: Sonde + ctest-PIN (Gleit-Report 2026-08-09; f314-per-Tick
# --- im Walk-Sub @0x80051630/@0x8005171c, Speed 210 @0x80076c8e, Sub-6-Handoff @0x80051794) ---
add_executable(probe_elliot_1170_run probe_elliot_1170_run.c)
target_link_libraries(probe_elliot_1170_run PRIVATE re15_engine re15_test_support)
target_include_directories(probe_elliot_1170_run PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_elliot_1170_run COMMAND probe_elliot_1170_run)
set_tests_properties(unit_elliot_1170_run PROPERTIES TIMEOUT 30)

# --- DIAGNOSE-Probe (kein ctest-Eintrag): ROOM10D0 Marvin-Cutscene (analysis/marvin_10d0.md) ---
add_executable(probe_marvin_10d0 probe_marvin_10d0.c)
target_link_libraries(probe_marvin_10d0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_marvin_10d0 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest-Eintrag): ROOM10D0 Liegend-Zombie behavior 0x0e (analysis/zombie_lyer_10d0.md) ---
add_executable(probe_zlyer_10d0 probe_zlyer_10d0.c)
target_link_libraries(probe_zlyer_10d0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_zlyer_10d0 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- REGRESSION (ctest): ROOM1170-Treppen unter der per-Record-Richtungswahl ---
add_executable(probe_stair_1170 probe_stair_1170.c)
target_link_libraries(probe_stair_1170 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_stair_1170 PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_stair_1170_regression COMMAND probe_stair_1170)

# --- ORIGINAL-Debug-Menue (UTILITY MENU, PSX.EXE @0x80014444) ---
add_executable(test_debug_menu test_debug_menu.c)
target_link_libraries(test_debug_menu PRIVATE re15_engine re15_test_support)
target_include_directories(test_debug_menu PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_debug_menu PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_debug_menu COMMAND test_debug_menu)
set_tests_properties(unit_debug_menu PROPERTIES TIMEOUT 30)

# Diagnose-Sonde (kein ctest): Leons Flinch beim Kraehen-Dive (analysis/crow_victim_anim.md)
add_executable(probe_crow_flinch probe_crow_flinch.c)
target_link_libraries(probe_crow_flinch PRIVATE re15_engine re15_test_support)
target_include_directories(probe_crow_flinch PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Regression: Kraehen-Corpse Anim-Halt (Freeze Frame 12) + Blutlache +10/Tick x51 + GIB-Wipe
# (analysis/crow_death_pool.md, Corpse-Maschine 0x801157e8/@0x80121234)
add_executable(probe_crow_death probe_crow_death.c)
target_link_libraries(probe_crow_death PRIVATE re15_engine re15_test_support)
target_include_directories(probe_crow_death PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_crow_death_corpse COMMAND probe_crow_death)
set_tests_properties(unit_crow_death_corpse PROPERTIES TIMEOUT 30)

# --- DIAGNOSE-Probe (kein ctest): ROOM10D0 Marvin-Cutscene Head-Look (analysis/cutscene_headlook.md) ---
add_executable(probe_headlook_10d0 probe_headlook_10d0.c)
target_link_libraries(probe_headlook_10d0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_headlook_10d0 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest): Save-Zaehler erbt bei NEW GAME den Karten-Stand (analysis/save_counter.md) ---
add_executable(probe_save_counter probe_save_counter.c)
target_link_libraries(probe_save_counter PRIVATE re15_engine re15_test_support)
target_include_directories(probe_save_counter PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest): Save-Slot-Ortsname "Irons' Office" (analysis/bug_save_room_name.md) ---
add_executable(probe_save_room_name probe_save_room_name.c)
target_link_libraries(probe_save_room_name PRIVATE re15_engine re15_test_support)
target_include_directories(probe_save_room_name PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest): Verletzungszustand (HP/Blut-Decals) im Savegame (analysis/save_injured_state.md) ---
add_executable(probe_save_injured probe_save_injured.c)
target_link_libraries(probe_save_injured PRIVATE re15_engine re15_test_support)
target_include_directories(probe_save_injured PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Regression: ROOM1170 Tuer-Slot-5-Lock — Ifel/Else-Zwilling sce1-MESSAGE/sce2-DOOR auf Ck(4,0xc3)
# ("It's locked from the other side", analysis/door_lock_1170.md)
add_executable(probe_door_1170 probe_door_1170.c)
target_link_libraries(probe_door_1170 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_door_1170 PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_door_1170_lock COMMAND probe_door_1170)
set_tests_properties(unit_door_1170_lock PROPERTIES TIMEOUT 30)

# Regression: ROOM10D0 Marvin SPAWN-/Start-Animation (Nutzer-Report #3) — die Executor-Kanal-
# Bank ist +0x170/+0x174 = EMD dir[4]/dir[3] BANK 1 (FUN_80022300 @0x800224b8/c8), gemessen
# gegen Original-Savestate r10d0_walk1 (Clip1 16f -> Clip2 52f). analysis/marvin_spawn_anim.md
add_executable(probe_marvin_spawn probe_marvin_spawn.c)
target_link_libraries(probe_marvin_spawn PRIVATE re15_engine re15_test_support)
target_include_directories(probe_marvin_spawn PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_marvin_spawn_bank COMMAND probe_marvin_spawn)
set_tests_properties(unit_marvin_spawn_bank PROPERTIES TIMEOUT 30)

# Regression: ROOM10D0-Zombie nach Raum-Verlassen/Wiederbetreten (analysis/zombie_10d0_reentry.md)
# Szenario A = ohne Cutscene (Erst-Zweig erneut, Liegend-Schlaefer weckt); Szenario B = Ck(4,247)=1
# (ELSE-Zweig beh=0x02: stehender Schlaefer MUSS engagen — Nibble-2-Dispatch f960[0]=FUN_80101c7c)
add_executable(probe_zreentry_10d0 probe_zreentry_10d0.c)
target_link_libraries(probe_zreentry_10d0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_zreentry_10d0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_zombie_10d0_reentry_a COMMAND probe_zreentry_10d0 A)
set_tests_properties(unit_zombie_10d0_reentry_a PROPERTIES TIMEOUT 30)
add_test(NAME unit_zombie_10d0_reentry_b COMMAND probe_zreentry_10d0 B)
set_tests_properties(unit_zombie_10d0_reentry_b PROPERTIES TIMEOUT 30)

# DIAGNOSE-Probe (kein ctest): EM040-Bank-1-Gait-Inhalt (Keyframe-Varianz Clip 5/0 — "Marvin schwebt")
add_executable(probe_em40_gait probe_em40_gait.c)
target_link_libraries(probe_em40_gait PRIVATE re15_engine re15_test_support)
target_include_directories(probe_em40_gait PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Regression: Marvin-Render-Pose — Bank-1-Gaits zyklieren in der GEPOSTEN Ausgabe
# (compute_actor_kf + skel_compute_pose; Nutzer-Report #5 "schwebt am Anfang")
add_executable(probe_marvin_render_pose probe_marvin_render_pose.c)
target_link_libraries(probe_marvin_render_pose PRIVATE re15_engine re15_test_support)
target_include_directories(probe_marvin_render_pose PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_marvin_render_pose COMMAND probe_marvin_render_pose)
set_tests_properties(unit_marvin_render_pose PROPERTIES TIMEOUT 30)

# --- DIAGNOSE-Probe (kein ctest): ROOM1040 "shutter switch"-Prompt beim Betreten (BUG 3) ---
add_executable(probe_room1040_switch probe_room1040_switch.c)
target_link_libraries(probe_room1040_switch PRIVATE re15_engine re15_test_support)
target_include_directories(probe_room1040_switch PRIVATE ${CMAKE_SOURCE_DIR}/include)

# DIAGNOSE (kein ctest): ROOM1070 Zombie-Wake (Nutzer-Report BUG 5) — analysis/bug_room1070_wake.md
add_executable(probe_room1070_wake probe_room1070_wake.c)
target_link_libraries(probe_room1070_wake PRIVATE re15_engine re15_test_support)
target_include_directories(probe_room1070_wake PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- ROOM1030 Cutscene-Messsonde (Bug-Report 2026-08: "Cutscene triggert komisch,
#     keine Kontrolle zurueck"). Reine MESSUNG, kein ctest-Gate. ---
add_executable(probe_cut_1030 probe_cut_1030.c)
target_link_libraries(probe_cut_1030 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_cut_1030 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE D: Kamera-Zustand beim Raum-(Re)Load (Bug-Report 2026-08-06:
#     "ROOM1030 erneut betreten -> Kamera auf den Zombies"). Reine MESSUNG. ---
add_executable(probe_camload_1030 probe_camload_1030.c)
target_link_libraries(probe_camload_1030 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_camload_1030 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# DIAGNOSE (kein ctest): "nach dem Tod laden -> stirbt sofort wieder" (Nutzer-Report 2026-08-05)
# analysis/bug_load_after_death.md — der Devour-Kommando-Zustand ueberlebt Tod->Title->LOAD.
add_executable(probe_load_after_death probe_load_after_death.c)
target_link_libraries(probe_load_after_death PRIVATE re15_engine re15_test_support)
target_include_directories(probe_load_after_death PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_load_after_death PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

# ==== REGRESSIONS-GATES fuer die Fehlerliste vom 2026-08-05 ====
# Die drei Sonden oben sind nach dem Fix keine reinen Messungen mehr, sondern pruefen das
# wiederhergestellte Original-Verhalten und liefern bei Rueckfall einen Fehlercode.
add_test(NAME unit_load_after_death COMMAND probe_load_after_death)
set_tests_properties(unit_load_after_death PROPERTIES TIMEOUT 30)

add_test(NAME unit_room1070_wake COMMAND probe_room1070_wake)
set_tests_properties(unit_room1070_wake PROPERTIES TIMEOUT 30)

# Der volle Nutzer-Ablauf: Schalter druecken (Prompt MUSS kommen) -> Raum neu betreten
# (Prompt darf NICHT kommen, Kontrolle muss beim Spieler bleiben).
add_test(NAME unit_room1040_switch COMMAND probe_room1040_switch 1040 -24000 16500 0 1)
set_tests_properties(unit_room1040_switch PROPERTIES TIMEOUT 30)

# --- MESSSONDE (kein ctest): ROOM1030 Kamera beim WIEDERbetreten (Nutzer-Report 2026-08-06).
#     Faehrt den echten Tuer-Pfad re15_room_apply_pending 1030 -> 1040 -> 1030. ---
add_executable(probe_cam_1030_reentry probe_cam_1030_reentry.c)
target_link_libraries(probe_cam_1030_reentry PRIVATE re15_engine re15_test_support)
target_include_directories(probe_cam_1030_reentry PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- MESSSONDE (kein ctest, A1 2026-08-08): "komische Animation nach Tuerwechsel nach
#     Zombie-Kampf" — echter Tuer-Pfad 1170 -> 1140 (Kampf-Variante A/B/C) -> Tuer raus,
#     Spieler-Felder der ersten 120 Ticks im Zielraum loggen. ---
add_executable(probe_hitdoor_entry_anim probe_hitdoor_entry_anim.c)
target_link_libraries(probe_hitdoor_entry_anim PRIVATE re15_engine re15_test_support)
target_include_directories(probe_hitdoor_entry_anim PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- MESSSONDE (kein ctest): Cut_replace-Mechanismus in ROOM1030 (Lane B, 2026-08-06).
#     Misst den Tail von op_cut_replace beim Wiederbetreten ueber alle Vorraum-Cuts. ---
add_executable(probe_cut_replace_1030 probe_cut_replace_1030.c)
target_link_libraries(probe_cut_replace_1030 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_cut_replace_1030 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- MESSSONDE (kein ctest): RVD-Auswahl PORT vs ORIGINAL FUN_80014230 (Kamera-Schwenk) ---
add_executable(probe_rvd_diff_10a0 probe_rvd_diff_10a0.c)
target_link_libraries(probe_rvd_diff_10a0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_rvd_diff_10a0 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- MESSSONDE (kein ctest): Kamera-Verlauf beim Treppen-Abstieg ROOM10A0 ---
#     (Ziel probe_stair_cam_10a0 ist bereits weiter oben definiert — Doppel-
#      Definition entfernt, sie liess CMake mit "target already exists" abbrechen.)

# --- MESSSONDE (kein ctest): Treppen-Abstieg ROOM10A0 tick-genau (Eject-Schrittweite) ---
add_executable(probe_stair_eject_10a0 probe_stair_eject_10a0.c)
target_link_libraries(probe_stair_eject_10a0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_stair_eject_10a0 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Regressions-Gate: ROOM1030 beim Wiederbetreten — der Tuer-Eintritts-Cut muss sich gegen jeden
# Rest-Cut des Vorraums durchsetzen (Nutzer-Report 2026-08-06, Ursache: Schwanz von op_cut_replace).
add_test(NAME unit_cam_1030_reentry COMMAND probe_cam_1030_reentry)
set_tests_properties(unit_cam_1030_reentry PROPERTIES TIMEOUT 30)

# --- LANE D MESSSONDE (kein ctest): Pro-Frame-Trajektorie des ROOM1060-Abstiegs ---
#     Findet den exakten Bruchpunkt der RVD-Zustandsmaschine (FUN_80014230 @0x80014230).
add_executable(probe_lane_d_traj1060 probe_lane_d_traj1060.c)
target_link_libraries(probe_lane_d_traj1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_lane_d_traj1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE-E MESSSONDE (kein ctest): Original-Quirk oder Port-Fehler? ROOM1060-Kette
#     mit Segment-Skip-Detektor (RVD-Scan ist ein PUNKT-Test @0x80014298). ---
add_executable(probe_lanee_chain_1060 probe_lanee_chain_1060.c)
target_link_libraries(probe_lanee_chain_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_lanee_chain_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE A MESSSONDE (kein ctest): ROOM1060 Zonen-/Geometrie-Karte + Abstiegs-Weg ---
add_executable(probe_lanea_1060_zonepath probe_lanea_1060_zonepath.c)
target_link_libraries(probe_lanea_1060_zonepath PRIVATE re15_engine re15_test_support)
target_include_directories(probe_lanea_1060_zonepath PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE-F MESSSONDE (kein ctest): Etage/Band/Hoehe im Kamera-Zonen-Scan (ROOM1060)
#     Prueft das Floor-Byte rec[1] (@0x80014274/0x80014280), die XZ-Ueberlappung der
#     Zonen verschiedener Cuts und den idealen vs. Port-Abstiegs-Sweep. ---
add_executable(probe_laneF_floor_1060 probe_laneF_floor_1060.c)
target_link_libraries(probe_laneF_floor_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_laneF_floor_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- DIAGNOSE-Probe (kein ctest-Eintrag): LANE B Treppen-FSM Frame-fuer-Frame ---
add_executable(probe_lb_stairfsm probe_lb_stairfsm.c)
target_link_libraries(probe_lb_stairfsm PRIVATE re15_engine re15_test_support)
target_include_directories(probe_lb_stairfsm PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE-E MESSSONDE (kein ctest): ist die Rueckschalt-Zone im Original ueberhaupt begehbar? ---
add_executable(probe_lanee_reach_1060 probe_lanee_reach_1060.c)
target_link_libraries(probe_lanee_reach_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_lanee_reach_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE A MESSSONDE (kein ctest): ROOM1060 Marge der Kamera-Zonen am Treppenfuss ---
add_executable(probe_lanea_1060_margin probe_lanea_1060_margin.c)
target_link_libraries(probe_lanea_1060_margin PRIVATE re15_engine re15_test_support)
target_include_directories(probe_lanea_1060_margin PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE C MESSSONDE (kein ctest): laeuft der RVD-/Cut-Zonen-Scan waehrend der Treppe? ---
add_executable(probe_lanec_camscan_1060 probe_lanec_camscan_1060.c)
target_link_libraries(probe_lanec_camscan_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_lanec_camscan_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE-F MESSSONDE (kein ctest): RVD-Floor-Gate vs. Treppen, game-weit ---
add_executable(probe_laneF_floorgate_rooms probe_laneF_floorgate_rooms.c)
target_link_libraries(probe_laneF_floorgate_rooms PRIVATE re15_engine re15_test_support)
target_include_directories(probe_laneF_floorgate_rooms PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE-E MESSSONDE (kein ctest): Trefferspanne der Treppen-Kamerazonen (Gait vs. Port-Eject) ---
add_executable(probe_lanee_margin_1060 probe_lanee_margin_1060.c)
target_link_libraries(probe_lanee_margin_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_lanee_margin_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE-F MESSSONDE (kein ctest): Zonen-Skip durch diskrete Abtastung (ROOM1060) ---
add_executable(probe_laneF_gap_1060 probe_laneF_gap_1060.c)
target_link_libraries(probe_laneF_gap_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_laneF_gap_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE-E MESSSONDE (kein ctest): game-weiter Zensus, wo der Port-Finalize-Versatz RVD-Zonen ueberspringt ---
add_executable(probe_lanee_eject_census probe_lanee_eject_census.c)
target_link_libraries(probe_lanee_eject_census PRIVATE re15_engine re15_test_support)
target_include_directories(probe_lanee_eject_census PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(probe_adv_stairband_1060 probe_adv_stairband_1060.c)
target_link_libraries(probe_adv_stairband_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_adv_stairband_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- ADVERSARIALE GEGENPROBE Lane C (kein ctest): Cut-Kette ROOM1060 mit KOPF-Triggern ---
add_executable(probe_advc_chain1060 probe_advc_chain1060.c)
target_link_libraries(probe_advc_chain1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_advc_chain1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(probe_adv_ejectcam_1060 probe_adv_ejectcam_1060.c)
target_link_libraries(probe_adv_ejectcam_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_adv_ejectcam_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# ADV-F adversarial cross-check (Diagnose, kein ctest)
add_executable(probe_advF_ejectcam_1060 probe_advF_ejectcam_1060.c)
target_link_libraries(probe_advF_ejectcam_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_advF_ejectcam_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# ADV-Z adversarial causality probe (Diagnose, kein ctest)
add_executable(probe_advz_cause_1060 probe_advz_cause_1060.c)
target_link_libraries(probe_advz_cause_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_advz_cause_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# REF-A adversarial gait-causality probe (Diagnose, kein ctest)
add_executable(probe_refA_gaitcause_1060 probe_refA_gaitcause_1060.c)
target_link_libraries(probe_refA_gaitcause_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_refA_gaitcause_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Hardware-Wahrheit: Port-Treppenstrecke gegen 4 DuckStation-Messungen (ORIGINAL-EXE, ROOM1060).
add_executable(probe_stair_hwtruth_1060 probe_stair_hwtruth_1060.c)
target_link_libraries(probe_stair_hwtruth_1060 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_stair_hwtruth_1060 PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_stair_hwtruth_1060 COMMAND probe_stair_hwtruth_1060)
set_tests_properties(unit_stair_hwtruth_1060 PROPERTIES TIMEOUT 30)

# --- LANE B (Bug-Report 2026-08-06 ROOM1030): "Zombies kriechen nicht unter dem Tor
#     durch" + "Dutzende statt SECHS". Reine MESSUNG, kein ctest-Gate. ---
add_executable(probe_laneb_1030_horde probe_laneb_1030_horde.c)
target_link_libraries(probe_laneb_1030_horde PRIVATE re15_engine re15_test_support)
target_include_directories(probe_laneb_1030_horde PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE E (Bug-Report 2026-08-06 ROOM1030): SICHTBARKEIT — wer wird ueberhaupt
#     gezeichnet? Region-Quad-Cull (FUN_8001e8c8/FUN_80014368) + Spawn-Roster.
#     Reine MESSUNG, kein ctest-Gate. ---
add_executable(probe_laneE_1030_visible probe_laneE_1030_visible.c)
target_link_libraries(probe_laneE_1030_visible PRIVATE re15_engine re15_test_support)
target_include_directories(probe_laneE_1030_visible PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE D: ROOM1030 Spawn-/Sichtbarkeits-Messung (Nutzer-Report 2026-08-06:
#     "nur SECHS Zombies im Original / Dutzende bei uns" + "Zombies kriechen unter dem Tor
#     durch"). Reine MESSUNG, kein ctest-Gate. ---
add_executable(probe_laned_1030_spawn probe_laned_1030_spawn.c)
target_link_libraries(probe_laned_1030_spawn PRIVATE re15_engine re15_test_support)
target_include_directories(probe_laned_1030_spawn PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- LANE C: ROOM1030 Tor + Einlauf-Choreografie (Bug-Report 2026-08-06:
#     "Zombies kriechen nicht unter dem Tor durch" / "Dutzende statt sechs").
#     Reine MESSUNG, kein ctest-Gate. ---
# Zensus: weicht die Objekt-Notch-Record-Auswahl (Port: type==sce5) irgendwo von der
# Original-Auswahl (rec[1]: Pool-Bit 0x04 + AUTO + Testbit) ab?
add_executable(probe_objnotch_census probe_objnotch_census.c)
target_link_libraries(probe_objnotch_census PRIVATE re15_engine re15_test_support)
target_include_directories(probe_objnotch_census PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Glied-1-Nachweis: stempelt der Port entity+0x0B wie das Original?
# Soll (am ORIGINAL per PCSX-Redux gemessen): Aktor in AOT-Slot 5 -> 5, in Slot 6 -> 6.
add_executable(probe_lanea_1030_cap probe_lanea_1030_cap.c)
target_link_libraries(probe_lanea_1030_cap PRIVATE re15_engine re15_test_support)
target_include_directories(probe_lanea_1030_cap PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(probe_gate_1030 probe_gate_1030.c)
target_link_libraries(probe_gate_1030 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_gate_1030 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- FAHRSTUHL ROOM1080 (ctest-Pin): Etagen-Wahl (Panel sub06 -> Cursor-Notch 4/5/6 ->
#     sub07/08/09 -> Aot_on 0/1/2) muss pro Etage das RDT-Soll-Ziel liefern:
#     slot0 -> ROOM1040 (main00 @0x482), slot1 -> ROOM10C0 (@0x4A2), slot2 -> ROOM1120 (@0x4C2). ---
add_executable(probe_elevator_1080 probe_elevator_1080.c)
target_link_libraries(probe_elevator_1080 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_elevator_1080 PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME probe_elevator_1080 COMMAND probe_elevator_1080)

# --- REGRESSION (ctest): ROOM11F0 Generator-Raum — 12 Obj_model_set-Props (10 Schalter
#     id 0x02..0x0B, Maschine id 0x00, Tuer-Prop id 0x01), Notch-Stempel, Toggle-Subs,
#     Win-Pattern. Nutzer-Report "6 Schalter fehlen" = PC-Loader/Draw-Cap 6 (main.c);
#     Original hat KEIN Cap: FUN_800436a8 laeuft bis nOmodel (@0x80043758/0x800437ac),
#     LAB_80040914 indexiert den Pool per obj_id (@0x8004093c-58). ---
add_executable(probe_gen_11f0 probe_gen_11f0.c)
target_link_libraries(probe_gen_11f0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_gen_11f0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_gen_11f0_switches COMMAND probe_gen_11f0)
set_tests_properties(unit_gen_11f0_switches PROPERTIES TIMEOUT 30)

# --- ADVERSARIALE GEGENPROBE zu Lane "behavior-0d" (ROOM1030). Reine MESSUNG, kein ctest-Gate. ---
add_executable(probe_adv_1030_verify probe_adv_1030_verify.c)
target_link_libraries(probe_adv_1030_verify PRIVATE re15_engine re15_test_support)
target_include_directories(probe_adv_1030_verify PRIVATE ${CMAKE_SOURCE_DIR}/include)

# ROOM1030 darf nur 6 Zombies instanziieren (Save(0x12,6) + Cap-Gate @0x80042224).
add_executable(probe_1030_spawncap probe_1030_spawncap.c)
target_link_libraries(probe_1030_spawncap PRIVATE re15_engine re15_test_support)
target_include_directories(probe_1030_spawncap PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_1030_spawncap COMMAND probe_1030_spawncap)
set_tests_properties(unit_1030_spawncap PROPERTIES TIMEOUT 30)

# --- LANE E (2026-08-07): TOR-MASKE ROOM1030 — warum kommen nur Kriecher durch?
#     Misst Sca_id_set(rgn2/3,idx6,0xF7) am Port + die (Maske & u0)-Arithmetik von
#     @0x8003b254/58. Reine MESSUNG, kein ctest-Gate. ---
add_executable(probe_laneE_gatemask probe_laneE_gatemask.c)
target_link_libraries(probe_laneE_gatemask PRIVATE re15_engine re15_test_support)
target_include_directories(probe_laneE_gatemask PRIVATE ${CMAKE_SOURCE_DIR}/include)

# ROOM1030 Kriechtor-Maschine: Sub-Modus-0x10-Toggle (FUN_80104f80) + Grid-Wurzel 1
# (FUN_80101708) + Kriech-Erstframe (FUN_801036dc) — Zustandsmaschinen-Gate mit Mock-Bank
# (Cliplaengen 98/99; Hand-Lock darf 0-Delta liefern).
add_executable(test_crawl_toggle test_crawl_toggle.c)
target_link_libraries(test_crawl_toggle PRIVATE re15_engine re15_test_support)
target_include_directories(test_crawl_toggle PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_crawl_toggle COMMAND test_crawl_toggle)
set_tests_properties(unit_crawl_toggle PROPERTIES TIMEOUT 30)

# Kriech-VORTRIEB mit der ECHTEN CDEMD0.EMS-Bank (Hand-Lock FUN_80109470, 5x CompMatrix):
# Betrag wird geloggt, NICHT geprueft (Soll erst per Savestate C). Reine MESSUNG, kein ctest.
add_executable(probe_crawl_commit probe_crawl_commit.c)
target_link_libraries(probe_crawl_commit PRIVATE re15_engine re15_test_support)
target_include_directories(probe_crawl_commit PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Dog-Treffer-Trias (Nutzer-Report 2026-08-08): (1) Blut-Spawn @HURT-Entry (0x2000, Bone 1
# @0x80110a84-aa4), (2) Rueckstoss-Hop sub 7 (Clip 0x13, +0x8c=0x14, 245d8(0x800)
# @0x8010f070/f120), (3) Corpse haelt LETZTEN Collapse-Keyframe (f314-Wrap FUN_8001f3bc,
# kein f314 im Corpse-Root 0x80111774). Bank-echt (EM020 aus CDEMD0.EMS).
add_executable(probe_dog_hit probe_dog_hit.c)
target_link_libraries(probe_dog_hit PRIVATE re15_engine re15_test_support)
target_include_directories(probe_dog_hit PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_dog_hit COMMAND probe_dog_hit)
set_tests_properties(unit_dog_hit PROPERTIES TIMEOUT 30)

# Dog-FINISHER (Nutzer-Report 2026-08-08): Grab sub 9 -> Feed-Timeout -> sub 0xb FUN_801100b4
# (Dog-Clip 0x1b, cmd 6 @0x80110138, Victim-Bank dog+0x178/0x17c @0x80110160/70) -> Hook B
# LAB_80111cb0/Maschine B 0x80111cf0: LEON-Kollaps = Dog-BANK2-Clip 4 (@0x80111d44), Blut 0x2000
# @Frames 0x29/0x3a (Bone 8), Se_on(0x4030001) @0x3a, 45630(2,0) @0x4f, Wunden 0/5/7
# (@0x80111e78/84/90) + cmd 7 (@0x80111ea0) am Clip-Ende. Bank-echt (EM020 aus CDEMD0.EMS).
add_executable(probe_dog_devour probe_dog_devour.c)
target_link_libraries(probe_dog_devour PRIVATE re15_engine re15_test_support)
target_include_directories(probe_dog_devour PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_dog_devour COMMAND probe_dog_devour)
set_tests_properties(unit_dog_devour PROPERTIES TIMEOUT 30)

# ROOM11E0 STROM-/Funken-Effekt (Nutzer-Report 2026-08-08, WRENCH-Reparatur-Stelle):
# sub27 (Datei 0x20DC) loopt `Sce_espr_on id=0x11 sub=0 cat=0 @(-12200,-2400,-8000)
# scale16=0x1000` + Sleep(20) solange flag(4,36)==0; sub21 setzt flag(4,36)=1 (@+0x086A).
# Row-Maschine: Effekt 0x11 sub0 = 1 Stream/2 Rows @0x46F8, row0 A=10 -> Routine 10
# @0x800176b0: flags:=0x13 (ABE), TPAGE|=0x20 (ABR1 additiv), Advance. Spawner-Kette
# FUN_80041864 (@0x8004194c packt scale16 aus pc[6..7]) -> FUN_80019700 (IMMER Row-Block).
add_executable(probe_espr_11e0 probe_espr_11e0.c)
target_link_libraries(probe_espr_11e0 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_espr_11e0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_espr_11e0 COMMAND probe_espr_11e0)
set_tests_properties(unit_espr_11e0 PROPERTIES TIMEOUT 30)

# --- RE2-Modus WELLE A: CDEMD0.EMS-TOC (EXE @0x8009adf4) + RE2-EMD-Splitter gegen die
#     echten shared_assets/RE2-Bytes (EM010: dir@0x23DAC/8, Paar1 8 Clips/65, MD1 nObj 34) ---
add_executable(test_re2_ems test_re2_ems.c)
target_link_libraries(test_re2_ems PRIVATE re15_engine re15_test_support)
target_include_directories(test_re2_ems PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_re2_ems PRIVATE
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_re2_ems COMMAND test_re2_ems)
set_tests_properties(unit_re2_ems PROPERTIES TIMEOUT 30)

# --- RE2-Modus WELLE A: ENEMSE.VBS-Bank-TOC (EXE @0x800a7b1c, Loader FUN_8005a09c) +
#     byte-true SE-Map-Dekodierung (FUN_8005bd6c) an echten Bank-0/11-Bytes ---
add_executable(test_re2_enemse test_re2_enemse.c)
target_link_libraries(test_re2_enemse PRIVATE re15_engine re15_test_support)
target_include_directories(test_re2_enemse PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_re2_enemse PRIVATE
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_re2_enemse COMMAND test_re2_enemse)
set_tests_properties(unit_re2_enemse PROPERTIES TIMEOUT 30)

# --- Batch A1 (2026-08-17): NECK/HEAD-LOOK — Auto-Look-Scan FUN_8003703c (Wurzel-Gate +
#     Ausschluss +0x9&0x60 + K1/K2/K3) und die Neck-FSM-Basis FUN_80037358 (Klemme VOR
#     dem Keyframe-Abzug @0x80037598/@0x800375a4-d0, Release-Pitch 0 ABSOLUT
#     @0x80037554-60). Belege: analysis-Specs lane17_NECK / lane17_CUTSCENE. ---
add_executable(test_neck_headlook test_neck_headlook.c)
target_link_libraries(test_neck_headlook PRIVATE re15_engine re15_test_support)
target_include_directories(test_neck_headlook PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_neck_headlook COMMAND test_neck_headlook)
set_tests_properties(unit_neck_headlook PROPERTIES TIMEOUT 30)

# --- Batch B1 (2026-08-17): Gegner-/NPC-KI-Cluster. Pins:
#     T1 Phase-1-Body auf dem Align-Tick der Walk-Subs (Sub 4 @0x80051230 -> Body @0x80051234,
#        Exit @0x800512cc; Sub 5 @0x8005156c -> @0x800515a0, Exit @0x80051638)
#     T2 Idle-Clip 0 LOOPT (f314 unbedingt @0x80101da8, Wrap `sb zero,149` @0x8001f63c)
#     T3 RE2-Frame-Wort-SFX nur in EXEC[1]/EXEC[2] (jal-Scan EMOVL10_S0.BIN: @0x80101d34/@0x80102454)
#     T4 ROOM1100 sel-7 liegt (RE1.5-Posenpaket-Familie {4,7,9} @0x80100dc0-e8)
#     T5 Skript-Wecker Nibble 9/10 (`sb a2,9(a0)` @0x800411f8) -> RE2 EXEC[9] 0x901 @0x80103E48
#     T3b LIEGEN (EXEC[7]) + AUFSTEHEN (EXEC[9]) ohne Frame-Wort-SE — der woertlich gemeldete
#        Teil von Report 13, den T3 nicht erreicht
#
# ⛔ EIN add_test PRO PIN (Review-Fund test-validity 2026-08-17): vorher lief das ganze Binary
#    unter EINEM Eintrag (`add_test(NAME unit_lane_b1_ai COMMAND test_lane_b1_ai)`) und
#    aggregierte alle Pins in einen Exit-Code. Solange EIN Pin rot war (T1), war das ctest-
#    Verdikt fuer die anderen bedeutungslos — die einzigen Pins der Nutzer-Reports 12/13/15
#    konnten unbemerkt rot werden, und genau das war eingetreten (T4/T5 rot, Suite meldete
#    unveraendert "unit_lane_b1_ai (Failed)"). Jeder Pin hat jetzt sein eigenes CI-Signal.
add_executable(test_lane_b1_ai test_lane_b1_ai.c)
target_link_libraries(test_lane_b1_ai PRIVATE re15_engine re15_test_support)
target_include_directories(test_lane_b1_ai PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_lane_b1_t1_aligntick  COMMAND test_lane_b1_ai t1)   # Reports 1 + 11
add_test(NAME unit_lane_b1_t2_idleloop   COMMAND test_lane_b1_ai t2)   # Report 12
add_test(NAME unit_lane_b1_t3_sfxgate    COMMAND test_lane_b1_ai t3)   # Report 13 (Walk/Knockdown)
add_test(NAME unit_lane_b1_t3b_getupsfx  COMMAND test_lane_b1_ai t3b)  # Report 13 (Liegen/Get-up)
add_test(NAME unit_lane_b1_t45_room1100  COMMAND test_lane_b1_ai t45)  # Report 15
set_tests_properties(unit_lane_b1_t1_aligntick unit_lane_b1_t2_idleloop
                     unit_lane_b1_t3_sfxgate unit_lane_b1_t3b_getupsfx
                     unit_lane_b1_t45_room1100 PROPERTIES TIMEOUT 90)

# --- Report 15 (ASSET-HAELFTE): die 13 BSS-Hintergruende von ROOM1100 waren ROOM1170-Kopien
#     (git HEAD nachgerechnet: alle 13 byte-identisch mit BSS/ROOM1170/). Neu geschnitten aus
#     STAGE1/ROOM110.BSS (851968 B = 13 x 65536). Der Fix lag in KEINEM Test — dieser Daten-Pin
#     prueft den GANZEN BSS-Baum (156 Raeume / 1688 Chunk-Dateien) gegen seine Stage-Container. ---
#     Bewusst OHNE re15_engine: reiner Datei-Vergleich, kein Netz, kein Emulator, keine Engine.
add_executable(test_bss_room_source test_bss_room_source.c)
target_compile_definitions(test_bss_room_source PRIVATE RE15_ASSETS_PATH=${RE15_ASSETS_PATH})
add_test(NAME unit_bss_room_source COMMAND test_bss_room_source)
set_tests_properties(unit_bss_room_source PROPERTIES TIMEOUT 120)

# --- Batch C (2026-08-17): GLOBALER TEXT-FREEZE (Nutzer-Report 16). Pins:
#     (1) sce-1-Examine-AOT traegt die Pause-Maske im Payload u16@+2 (@0x80043098)
#     (2) offener Examine-Text (0xffff0000) -> Spieler/KI/Anim/SCD/Pad stehen
#         (@0x80031c78 / @0x8010043c / @0x80019e40 / @0x8003f04c / @0x80030514)
#     (3) Maske 0 (Caption, 420 von 698 Message_on) friert NICHTS ein — Regressionsschutz
#         fuer die Kino-Captions ROOM1170/ROOM1240 (alle 22 tragen 0x0000)
#     (4) Close = Snapshot-Restore (@0x800285a4/@0x800286cc/@0x8002871c), auch ueber
#         einem bestehenden Item-Modal-Freeze
#     (5) Open-Guard (@0x80027e74) schuetzt den Snapshot vor dem Re-Exec
#     (6) Raumwechsel nullt g_pauseflags (@0x8001ca44/@0x8001caec)
add_executable(test_text_freeze test_text_freeze.c)
target_link_libraries(test_text_freeze PRIVATE re15_engine re15_test_support)
target_include_directories(test_text_freeze PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_text_freeze PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_text_freeze COMMAND test_text_freeze)
set_tests_properties(unit_text_freeze PROPERTIES TIMEOUT 60)

# --- Cluster 4 (2026-08-17): Raum-Transitions-Praesentation + Spieler-Kommandowort.
#     (a) Der Self-Reenter (Narrator -> Cinematic, ROOM1170) lief ohne Tuer-Praesentation:
#         harter Schnitt auf den bereits sprintenden Elliot. FUN_8001d600 verzweigt ALLEIN
#         am Tuer-Record-Zeiger (@0x8001d600/@0x8001d618) — ein Selbst-Raum-Wechsel nimmt
#         also dieselbe Praesentation. Transitions-FSM FUN_8001c958, Tabelle @0x8001069c;
#         State 4 @0x8001cc34-5c setzt g_pauseflags |= 0xff000000 (lui 0xff00 + or) und
#         State-Byte 5 @0x8001cc50 -> Spieler/KI/Anim/SCD/Pad stehen die ganze Einblendung.
#     (b) Spieler-Kommandowort +0x4: Plc_motion @0x80041ba4-b0 / Plc_dest @0x80041c14
#         schreiben 4, Plc_ret @0x80041f8c-9c schreibt 1 und NICHT +0x1b8 — darum fuehrt
#         das Original ab cmd 1 jeden Frame 0x12 nach. Ersetzt den frueheren 0x6C-Proxy.
add_executable(test_room_transition_cmd test_room_transition_cmd.c)
target_link_libraries(test_room_transition_cmd PRIVATE re15_engine re15_test_support)
target_include_directories(test_room_transition_cmd PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_room_transition_cmd COMMAND test_room_transition_cmd)
set_tests_properties(unit_room_transition_cmd PROPERTIES TIMEOUT 30)

# --- Cluster 3 (2026-08-17): RE2-Frame-Wort-SE-Kadenz. Beide Original-Gates:
#     Sub-Gate (+0x10E & 0x80) || (+0x21A & 0x8000)  @0x80101cd8-cf8 / @0x801023f8-418
#     Drittel-Takt  +0x14D % 3 == 2                  @0x80101d00-2c  / @0x80102420-4c
#     (+0x14D IST das Frame-Byte des Clip-Wortes @+0x14C, Produzent = die geteilte
#      EXE-Anim-Advance jal 0x8002959c direkt davor @0x80101cd0 / @0x801023f0.)
add_executable(test_re2_sfx_cadence test_re2_sfx_cadence.c)
target_link_libraries(test_re2_sfx_cadence PRIVATE re15_engine re15_test_support)
target_include_directories(test_re2_sfx_cadence PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_re2_sfx_cadence PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_re2_sfx_cadence COMMAND test_re2_sfx_cadence)
set_tests_properties(unit_re2_sfx_cadence PROPERTIES TIMEOUT 90)

# --- Cluster 3 (2026-08-17): ROTOR-BGM-PIN (Nutzer-Report 3, "Rotoren-Sound fehlt im
#     ROOM1170-Intro"). Der Port hatte auf dem BGM-SUB-Layer eine ERFUNDENE
#     Positions-Skalierung (FUN_80045a64 hat in der ganzen EXE nur 4 Caller, alle im
#     SE-Pfad, keiner bei BGM) plus ein player_mode-Mute. Beides daempfte den Rotor nicht
#     nur, es hob auch das Skript-Stop Sce_bgm_control(1,2) jeden Frame wieder auf.
#     Der Pin rendert die Mix-Kette deterministisch (RE15_AUDIO_CAP_SYNC: 1470 Stereo-
#     Frames je Spielframe, ohne SDL-Geraet) und misst den isolierten Rotor-Layer:
#     ON-Fenster rms>0 mit Blade-Pass ~10.9 Hz, OFF-Fenster still, kein Pegelunterschied
#     zwischen ferner und naher Kameraposition. Gegen den alten Stand fallen 4 der 5 Pins.
#     Braucht audio_pc.c + SDL2 direkt (NICHT re15_test_support, dessen Audio-Stubs
#     genau die geprueften Funktionen ersetzen wuerden).
if(TARGET SDL2::SDL2-static)
    add_executable(test_rotor_bgm_pin
        test_rotor_bgm_pin.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/audio_pc.c)
    target_link_libraries(test_rotor_bgm_pin PRIVATE re15_engine SDL2::SDL2-static)
    target_include_directories(test_rotor_bgm_pin PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/platform/pc/src)
    target_compile_definitions(test_rotor_bgm_pin PRIVATE
        RE15_PLATFORM_PC
        RE15_ASSET_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_CD_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
    add_test(NAME unit_rotor_bgm_pin COMMAND test_rotor_bgm_pin)
    set_tests_properties(unit_rotor_bgm_pin PROPERTIES TIMEOUT 120)
endif()

# --- DIAGNOSE (kein ctest): Nutzer-Report "Irons schaut im Original etwas diagonaler".
#     Misst Irons' (NPC 0x45) Neck-Zustand + die GERENDERTE Kopf-Orientierung in ROOM1150.
add_executable(probe_irons_neck_1150 probe_irons_neck_1150.c)
target_link_libraries(probe_irons_neck_1150 PRIVATE re15_engine re15_test_support m)
target_include_directories(probe_irons_neck_1150 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- MESSPROBE ROOM1140 Re-Entry nach "in die Menge laufen" (Nutzer-Report 2026-08-17) ---
add_executable(probe_1140_reentry probe_1140_reentry.c)
target_link_libraries(probe_1140_reentry PRIVATE re15_engine re15_test_support)
target_include_directories(probe_1140_reentry PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- PIN (2026-08-17): Nutzer-Report "Irons schaut im Original etwas diagonaler in Irons'
#     Office". Sce_em_set ruft den Typ-Root INLINE beim Spawn mit state 0 (`sw zero,4(s0)`
#     @0x800421e0 ... `jalr v0` @0x8004259c) -> der NPC-INIT 0x8011d2b8 LAEUFT immer und
#     setzt +0x1b8=0 (Entity-Tracking) / +0x1b9=8 / part8 Steps 0x40-0x30 / Klemmen
#     0x2c8-0x138. Der Port fuehrte den INIT nur als state-0-Dispatch aus, den der SCD mit
#     Plc_motion (state=4 @0x80041bb0) ueberholte -> Neck-FSM fuer Irons komplett aus.
#     Der Pin nagelt die GERENDERTE Kopf-Orientierung (Yaw UND Pitch) fest.
add_executable(test_irons_neck_1150 test_irons_neck_1150.c)
target_link_libraries(test_irons_neck_1150 PRIVATE re15_engine re15_test_support m)
target_include_directories(test_irons_neck_1150 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_irons_neck_1150 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_irons_neck_1150 COMMAND test_irons_neck_1150)
set_tests_properties(unit_irons_neck_1150 PROPERTIES TIMEOUT 90)

# --- MESSPROBE ROOM10D0 Knockdown/Getup-SEs (Nutzer-Report 2026-08-17) ---
add_executable(probe_10d0_knockse probe_10d0_knockse.c)
target_link_libraries(probe_10d0_knockse PRIVATE re15_engine re15_test_support)
target_include_directories(probe_10d0_knockse PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- Bank-Regel + Frame-Wort-SEs des STAGE1-Zombies (Nutzer-Reports 2026-08-17 A+B) ---
add_executable(test_zombie_bank_sfx test_zombie_bank_sfx.c)
target_link_libraries(test_zombie_bank_sfx PRIVATE re15_engine re15_test_support)
target_include_directories(test_zombie_bank_sfx PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_zombie_bank_rule  COMMAND test_zombie_bank_sfx bank)
add_test(NAME unit_zombie_idle_clip  COMMAND test_zombie_bank_sfx idle)
add_test(NAME unit_zombie_step_sfx   COMMAND test_zombie_bank_sfx step)
add_test(NAME unit_zombie_knock_sfx  COMMAND test_zombie_bank_sfx knock)
set_tests_properties(unit_zombie_bank_rule unit_zombie_idle_clip
                     unit_zombie_step_sfx unit_zombie_knock_sfx PROPERTIES TIMEOUT 60)

# --- A/B-MESSPROBE ROOM10D0-Liegender vs. ROOM1140-Stehender (Nutzer-Report 2026-08-17 #3) ---
add_executable(probe_ab_se_10d0_1140 probe_ab_se_10d0_1140.c)
target_link_libraries(probe_ab_se_10d0_1140 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_ab_se_10d0_1140 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- A/B-PIN ROOM10D0-Liegender vs. ROOM1140-Stehender (Nutzer-Report 2026-08-17 #3) ---
add_executable(test_zombie_se_ab_10d0 test_zombie_se_ab_10d0.c)
target_link_libraries(test_zombie_se_ab_10d0 PRIVATE re15_engine re15_test_support)
target_include_directories(test_zombie_se_ab_10d0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_zombie_se_ab_10d0 COMMAND test_zombie_se_ab_10d0)
set_tests_properties(unit_zombie_se_ab_10d0 PROPERTIES TIMEOUT 90)

# --- PIN auf die SE-STIMMEN-MASCHINE (Nutzer-Report 2026-08-17 #4) ---
# FUN_800453d0 byte3&0x1f-0x10 = feste SPU-Stimme, FUN_80045a18 = Prioritaets-Gate,
# FUN_800458d4 = Frame-Pumpe (Stimme v == SPU 16+v). Prueft berechnete Werte, kein Klang.
add_executable(test_se_voice_machine test_se_voice_machine.c)
target_link_libraries(test_se_voice_machine PRIVATE re15_engine re15_test_support)
target_include_directories(test_se_voice_machine PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_se_voice_machine COMMAND test_se_voice_machine)
set_tests_properties(unit_se_voice_machine PROPERTIES TIMEOUT 60)

# --- PIN auf die a3-Semantik von anim_set (FUN_8001f314) — Nutzer-Report 2026-08-17 #3 ---
# Widerlegt+friert ein: a3 ist KEINE Frame-Rate (Vorschub ist immer +1 @0x8001f610-3c),
# sondern der Crossfade-Gewichtsschritt (@0x8001f474 mult mit +0x8f).
add_executable(test_anim_rate test_anim_rate.c)
target_link_libraries(test_anim_rate PRIVATE re15_engine re15_test_support)
target_include_directories(test_anim_rate PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_anim_rate COMMAND test_anim_rate)
set_tests_properties(unit_anim_rate PROPERTIES TIMEOUT 60)

# --- PIN Rueckwaerts-Wiedergabe (anim_set a2) + Frame-Wort-SE-Dekoder lesen denselben Slot ---
# Byte-true: +0x168 (@0x8001f36c) ist der EINZIGE Frame-Zeiger; FUN_8001b38c liest ihn (@0x8001b3a4).
add_executable(test_anim_reverse_se test_anim_reverse_se.c)
target_link_libraries(test_anim_reverse_se PRIVATE re15_engine re15_test_support)
target_include_directories(test_anim_reverse_se PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_anim_reverse_se COMMAND test_anim_reverse_se)
set_tests_properties(unit_anim_reverse_se PROPERTIES TIMEOUT 60)

# --- PIN Front/Back-Latch (+0x93 Bit 0x80) + Knockdown-Clipwahl (Nutzer-Report 2026-08-17 #3) ---
# Belegt: Latch wirkt NUR im Tod (13-2*Bit @0x80106ca0-ac); der Knockdown nimmt
# unbedingt Clip 0x0B (@0x80105190-94) -> kann die gemeldete SE-Differenz nicht erklaeren.
add_executable(test_knockdown_frontback test_knockdown_frontback.c)
target_link_libraries(test_knockdown_frontback PRIVATE re15_engine re15_test_support)
target_include_directories(test_knockdown_frontback PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_knockdown_frontback COMMAND test_knockdown_frontback)
set_tests_properties(unit_knockdown_frontback PROPERTIES TIMEOUT 60)

# --- DIAGNOSE: Schuss -> Taumel -> Sturz, Frame-Sequenz (Nutzer-Report nach v0.2.4) ---
add_executable(probe_stagger_fall_1140 probe_stagger_fall_1140.c)
target_link_libraries(probe_stagger_fall_1140 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_stagger_fall_1140 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- RE2-Modus: der ECHTE Spielweg (pc_enemy_load-Spiegel inkl. RE2-Bank aus CDEMD0.EMS,
#     AI-Ticks in game_step-Reihenfolge, Render-Auswahl, Schuss). Nutzer-Report 2026-08-17. ---
add_executable(probe_re2_livepath test_re2_livepath.c)
target_link_libraries(probe_re2_livepath PRIVATE re15_engine re15_test_support)
target_include_directories(probe_re2_livepath PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_re2_livepath PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_re2_livepath COMMAND probe_re2_livepath)
set_tests_properties(unit_re2_livepath PROPERTIES TIMEOUT 90)

# --- RE2-Modus: byte-true ENEMSE-Bank-Wahl FUN_80052b38 + Raum-kind-Paar @0x8005728c-b8 ---
add_executable(test_re2_enemse_bank test_re2_enemse_bank.c)
target_link_libraries(test_re2_enemse_bank PRIVATE re15_engine re15_test_support)
target_include_directories(test_re2_enemse_bank PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_re2_enemse_bank COMMAND test_re2_enemse_bank)
set_tests_properties(unit_re2_enemse_bank PROPERTIES TIMEOUT 30)

# --- Schuss -> Taumel -> Sturz: Uebergabetick-Bank + volle Sturz-Frame-Folge ---
add_executable(test_knockdown_handoff test_knockdown_handoff.c)
target_link_libraries(test_knockdown_handoff PRIVATE re15_engine re15_test_support)
target_include_directories(test_knockdown_handoff PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_knockdown_handoff_bank COMMAND test_knockdown_handoff bank)
add_test(NAME unit_knockdown_fall_seq     COMMAND test_knockdown_handoff fall)
set_tests_properties(unit_knockdown_handoff_bank unit_knockdown_fall_seq PROPERTIES TIMEOUT 60)

# --- NEGATIV-PIN Aufsteh-Grunzer nach Knockdown (Nutzer-Report 2026-08-18) ---
add_executable(test_zombie_getup_se test_zombie_getup_se.c)
target_link_libraries(test_zombie_getup_se PRIVATE re15_engine re15_test_support)
target_include_directories(test_zombie_getup_se PRIVATE ${CMAKE_SOURCE_DIR}/include)
add_test(NAME unit_zombie_getup_se COMMAND test_zombie_getup_se)
set_tests_properties(unit_zombie_getup_se PROPERTIES TIMEOUT 120)
