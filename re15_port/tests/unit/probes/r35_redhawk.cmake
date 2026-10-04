# Runde 35 Spur D "redhawk" — Fleisch-Effekte der Hunde (Super Redhawk) laufen ewig.
#
# Dossier: analysis/befunde_runde35/D_redhawk.md.
#
# MESS-WERKZEUG (kein add_test): probe_r35_redhawk zensus = Routinen-Zensus aller Raum-ESP-Baenke.
add_executable(probe_r35_redhawk ${CMAKE_CURRENT_LIST_DIR}/../test_r35_redhawk.c)
target_link_libraries(probe_r35_redhawk PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r35_redhawk PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r35_redhawk PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
if(NOT WIN32)
    target_link_libraries(probe_r35_redhawk PRIVATE m)
endif()
