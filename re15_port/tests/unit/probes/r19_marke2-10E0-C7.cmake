# Riegel r19 / Marke 2 (2026-09-21) — ROOM10E0 Cut 7 darf die Figur nicht ZERREISSEN.
# Dossier: analysis/befunde_2026-09-21/pri-runde19/marke2-10E0-C7.md
# Ursache war das UMSCHLIESSENDE RECHTECK einer Typ-5-Diagonalzelle im Tiefen-Raycast
# (SCA-Eintrag 21 @ROOM10E0.RDT 0x758, type 0x05; re15_collision.c push_diag5
# @LAB_8003c734). Der Riegel prueft das ERGEBNIS in den ausgelieferten Maskendateien,
# nicht das Werkzeug — er faellt also auch, wenn die Dateien jemand anders neu baut.
add_executable(test_r19_zerreiss_10e0c7 test_r19_zerreiss_10e0c7.c)
target_link_libraries(test_r19_zerreiss_10e0c7 PRIVATE re15_engine re15_test_support)
target_include_directories(test_r19_zerreiss_10e0c7 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r19_zerreiss_10e0c7 PRIVATE
    RE15_PORT_SRC_DIR="${CMAKE_SOURCE_DIR}")
add_test(NAME unit_r19_zerreiss_10e0c7 COMMAND test_r19_zerreiss_10e0c7)
