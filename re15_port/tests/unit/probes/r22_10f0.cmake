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

# ⛔ RIEGEL (Runde 22, Nutzer-Marke befund_10F0_F335_marke1.bmp). Erkennungsmass
# "UNBELEGTE VERDECKUNG": kein Figurpunkt darf von einem Maskentexel verdeckt werden,
# der ausserhalb des BELEGT-Feldes des Winkels liegt — also in der Silhouette einer
# Moebelzelle MIT Freistellung, aber ausserhalb dieser Freistellung. Schranke 0, kein
# freier Parameter. Geprueft ueber die Nutzer-Marke, alle protokollierten Bilder aus
# befund.log und alle begehbaren Standplaetze beider Winkel (Klemmpfad, 200er-Raster).
# GEGENPROBE (Dossier analysis/befunde_2026-09-21/10f0-umsetzung.md §6): mit dem
# Auslieferungsstand vom 2026-09-21 faellt der Riegel — C4 MARKE 154, PFAD 22 Bilder,
# STAND 152 Plaetze; C5 STAND 516 Plaetze.
add_test(NAME unit_r22_10f0_figur
         COMMAND probe_r22_10f0_figur riegel
                 ${CMAKE_SOURCE_DIR}/../analysis/befunde_2026-09-21/10f0-quader-silhouette/messung/pfad_10f0_aus_befundlog.txt)
set_tests_properties(unit_r22_10f0_figur PROPERTIES TIMEOUT 300)
