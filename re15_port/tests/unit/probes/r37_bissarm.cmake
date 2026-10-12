# Spur J (Runde 37) — "bissarm": beim Zombie-Biss fehlt teilweise der dranhaengende Arm.
# Dossier analysis/befunde_runde37/J_bissarm.md.
add_executable(test_r37_bissarm ${CMAKE_CURRENT_SOURCE_DIR}/test_r37_bissarm.c)
target_link_libraries(test_r37_bissarm PRIVATE re15_engine re15_test_support)
target_include_directories(test_r37_bissarm PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r37_bissarm PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
