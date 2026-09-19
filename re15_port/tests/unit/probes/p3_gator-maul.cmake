# Phase 3 (2026-09-19) — Thema "gator-maul": Leons Groesse im 2/3-Maul (Punkt 1) und
# die Trefferfenster des Lunge-Clips (Punkt 2, Dossier gator-finisher-sounds.md §4 B2).
# Reine Messsonden, kein add_test.
foreach(_p probe_p3_gator_maul probe_p3_gator_lunge)
    add_executable(${_p} ${_p}.c)
    target_link_libraries(${_p} PRIVATE re15_engine re15_test_support)
    target_include_directories(${_p} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_compile_definitions(${_p} PRIVATE
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
endforeach()
