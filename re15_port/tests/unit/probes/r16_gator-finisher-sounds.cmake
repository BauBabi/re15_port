# Runde 16 (2026-09-19) — Thema "gator-finisher-sounds": Alligator-Finisher (Leon im Maul,
# Anker vs. Render-Scale), Alligator-Biss-SE (Bank-Latch/ENEMSE-Map), Spinnen-Angriffs-SE.
# Reine Messsonden, kein add_test. Dossier: analysis/befunde_2026-09-19/gator-finisher-sounds.md
foreach(_p probe_r16_gator_finisher_anker probe_r16_gator_se probe_r16_spider_se)
    add_executable(${_p} ${_p}.c)
    target_link_libraries(${_p} PRIVATE re15_engine re15_test_support)
    target_include_directories(${_p} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_compile_definitions(${_p} PRIVATE
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
endforeach()
