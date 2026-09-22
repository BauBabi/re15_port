# --- Runde 21 (2026-09-22), Thema tentakel-schub: Spur A zum Nutzer-Befund "von birkin
#     rausgeschoben ausserhalb des begehbaren Bereiches" (ROOM5090, Marke F962).
#     Misst die KIND-KOLLISION der vier Tentakel (enemy_ai_tentakel_g5.c:808,
#     Original 0x80104F64 + FUN_80034D0C) getrennt vom Koerper-Schub des Bosses.
#     Reine Diagnose-Sonde, KEIN add_test. Braucht shared_assets/RE2/CDEMD0.EMS. ---
add_executable(probe_r21_tentakel_schub probe_r21_tentakel_schub.c)
target_link_libraries(probe_r21_tentakel_schub PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r21_tentakel_schub PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r21_tentakel_schub PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
