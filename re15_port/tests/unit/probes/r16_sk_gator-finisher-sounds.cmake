# Runde 16 (2026-09-19) — SKEPTIKER-Gegensonde zum Thema "gator-finisher-sounds".
# Reine Messsonde, kein add_test. Bericht: analysis/befunde_2026-09-19/gator-finisher-sounds.skeptiker.md
foreach(_p probe_r16_sk_gator_finisher_sounds)
    add_executable(${_p} ${_p}.c)
    target_link_libraries(${_p} PRIVATE re15_engine re15_test_support)
    target_include_directories(${_p} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_compile_definitions(${_p} PRIVATE
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
endforeach()
