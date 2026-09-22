# Runde 22 (2026-09-22), Thema "discard-codepanel": woran erkennt man, dass der
# Zugriffscode einmal richtig eingegeben wurde?
# Dossier: analysis/befunde_2026-09-22/discard-codepanel-was-markiert-den-code.md
# Reine Messsonde (kein add_test) — sie fuehrt die ausgelieferten SCD-Bytes von
# ROOM10D0/ROOM1100 im Port-VM aus und protokolliert die Reihenfolge der Message_on.
add_executable(probe_r22_codepanel_reihenfolge probe_r22_codepanel_reihenfolge.c)
target_link_libraries(probe_r22_codepanel_reihenfolge PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r22_codepanel_reihenfolge PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r22_codepanel_reihenfolge PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
