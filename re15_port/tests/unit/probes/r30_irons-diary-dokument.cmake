# --- Runde 30 (2026-09-27), Thema irons-diary-dokument: das DOKUMENT-System
#     (FILE-Liste, Leser, Bild-Ebene, Irons Diary = FILE25).
#     Dossier: analysis/befunde_runde30/irons-diary-dokument.md
#
# unit_r30_irons_diary_dokument = RIEGEL (seit dem Bau; vorher reine Messsonde):
#       A  FILE-Liste dynamisch: 0 Namen bei leerer Liste, 1 nach re15_files_add(0)
#          (RE2 24 Plaetze @0x800D4B68, FUN_800692dc); die 21 Originalnamen (Maske
#          @0x800c6c98) bleiben als ARCHIV zaehlbar, das Spiel zeigt sie nicht,
#       B  Leser: Bild-Dokument ohne Zeichenstrom, verschiedene Dokumente ergeben
#          verschiedene Anzeigelisten (vorher fest @0x800c7614),
#       C  Durchsicht = CLUT-FARBE 0x0000 (0 deckende Texel statt 7410), Seitenzahl 18,
#       D  die nach shared_assets/RE2/FILES kopierten FILE25_*,
#       E  Seite -> Datei (RE2 Seitenlader 0x8006d444), x-Lage (RE2 25 @0x80076170).
# unit_r30_irons_diary_ablauf = RIEGEL: Speicherstand v9 (Rundlauf, Hebung an der
#       Speicherkarte des Nutzers) und die Aufhebe-Reihenfolge (RE2 FUN_80071ba0,
#       Zustand 6 @0x80072b0c-bfc).
add_executable(probe_r30_irons_diary_dokument probe_r30_irons_diary_dokument.c)
target_link_libraries(probe_r30_irons_diary_dokument PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_irons_diary_dokument PRIVATE
    ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(probe_r30_irons_diary_dokument PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
add_test(NAME unit_r30_irons_diary_dokument COMMAND probe_r30_irons_diary_dokument)
set_tests_properties(unit_r30_irons_diary_dokument PROPERTIES TIMEOUT 120)

add_executable(test_r30_irons_diary_ablauf test_r30_irons_diary_ablauf.c)
target_link_libraries(test_r30_irons_diary_ablauf PRIVATE re15_engine re15_test_support)
target_include_directories(test_r30_irons_diary_ablauf PRIVATE
    ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(test_r30_irons_diary_ablauf PRIVATE
    R30_NUTZER_KARTE="${CMAKE_SOURCE_DIR}/../analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr")
add_test(NAME unit_r30_irons_diary_ablauf COMMAND test_r30_irons_diary_ablauf
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(unit_r30_irons_diary_ablauf PROPERTIES TIMEOUT 120)
