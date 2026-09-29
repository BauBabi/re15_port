# Runde 33, Thema K — die Karte nach dem Irons-Hinweis: Etage 2F waehlbar, Zielraum markiert.
# Eigene Datei, damit parallel arbeitende Agenten nicht dieselbe CMakeLists editieren.
# Dossier: analysis/befunde_runde33/karte_zielraum.md
add_executable(test_r33_karte ${CMAKE_CURRENT_SOURCE_DIR}/test_r33_karte.c)
target_link_libraries(test_r33_karte PRIVATE re15_engine re15_test_support)
target_include_directories(test_r33_karte PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r33_karte PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
# Modus "messen" ist die Messsonde (kein Test). Riegel:
foreach(_r33k etage markierung speicher)
    add_test(NAME unit_r33_karte_${_r33k} COMMAND test_r33_karte ${_r33k})
    set_tests_properties(unit_r33_karte_${_r33k} PROPERTIES TIMEOUT 60 SKIP_RETURN_CODE 77)
endforeach()
