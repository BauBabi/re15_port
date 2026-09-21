# Runde 22, Nutzer-Marke ROOM10F0 Cut 4 (befund.log Zeile 15602,
# Bild befund_10F0_F335_marke1.bmp): "Leon ist da grossteils transparent".
#
# Die Sonde misst die WIRKLICHE Figur (PL00.MD1 + PL00.EMR posiert, mit dem
# Renderer-Transform aus main.c projiziert und gerastert) gegen die TEXELGENAUE
# Maskendeckung des Winkels — statt gegen den Rechteck-Kasten, mit dem alle
# bisherigen PRI-Messungen gerechnet haben.
#
# EIGENE Datei (nicht die gemeinsame CMakeLists.txt), s. probes/README.md.
add_executable(probe_r22_10f0_figur
    ${CMAKE_CURRENT_SOURCE_DIR}/probe_r22_10f0_figur.c)
target_link_libraries(probe_r22_10f0_figur PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r22_10f0_figur PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r22_10f0_figur PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
