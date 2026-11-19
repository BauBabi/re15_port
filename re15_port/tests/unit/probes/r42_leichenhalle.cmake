# --- Runde 42 Strang "original", Stufe 1: Leichenhalle ROOM1200/1201 - Kuehlfach klappert, Zombie kriecht heraus
#     (Version_40% sub02, include/re15_leichenhalle_r42.h). Riegel nur ueber Alt-Schnittstellen -> Gegenprobe auf aaea5f8c2 rot.
add_executable(test_r42_leichenhalle test_r42_leichenhalle.c)
target_link_libraries(test_r42_leichenhalle PRIVATE re15_engine re15_test_support)
target_include_directories(test_r42_leichenhalle PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r42_leichenhalle PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r42_leichenhalle COMMAND test_r42_leichenhalle)
set_tests_properties(unit_r42_leichenhalle PROPERTIES TIMEOUT 120)
