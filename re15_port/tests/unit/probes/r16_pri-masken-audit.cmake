# Sonden r16 / pri-masken-audit (2026-09-19) — Vordergrund-Masken STAGE1 gegen die
# Freistellungen des Nutzers. Reine Messsonde, kein add_test.
add_executable(probe_r16_pri_masken_audit probe_r16_pri_masken_audit.c)
target_link_libraries(probe_r16_pri_masken_audit PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_pri_masken_audit PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_pri_masken_audit PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
