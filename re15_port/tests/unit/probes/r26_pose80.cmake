# --- Runde 26 (2026-09-26), Thema hunde-posenriegel:
#     Misst auf dem ECHTEN Pfad (game_step + Zielen + Feuern), ob +0x1D3 Bit 0x80
#     (Original-Gate `lbu v0,467(s0)` @0x80047138 / `bne v0,zero` @0x80047140, unmaskiert)
#     je Typ 0x20/0x21/0x25/0x26 wieder 0 wird. Reine Messsonde, kein add_test. ---
add_executable(probe_r26_pose80 probe_r26_pose80.c)
target_link_libraries(probe_r26_pose80 PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r26_pose80 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r26_pose80 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
