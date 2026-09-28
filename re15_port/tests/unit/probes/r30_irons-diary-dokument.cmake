# --- Runde 30 (2026-09-27), Thema irons-diary-dokument: das DOKUMENT-System
#     (FILE-Liste, Leser, Bild-Ebene, Prototyp FILE25 "Irons Diary").
#     Dossier: analysis/befunde_runde30/irons-diary-dokument.md
#     Reine Messsonde (KEIN add_test). Sie misst den heutigen Stand:
#       A  wie viele Listenzeilen einen vorinstallierten Namen zeigen
#          (DEBUG.BIN Maske @0x800c6c98, Basis-Id @0x800c7370),
#       B  dass der Leser die gewaehlte Zeile nicht kennt (Blob @0x800ccd34,
#          fest adressiert @0x800c7614),
#       C  den Durchsicht-Fehler der Bild-Ebene (CLUT-Farbe 0x0000 bei Index != 0)
#          und die Seitenzaehlung gegen RE2s max_page (@0x800AA144),
#       D  den Prototyp FILE25 aus build/r30_irons-diary-dokument/
#          (re15_port/tools/re2_doc_satz.py). ---
add_executable(probe_r30_irons_diary_dokument probe_r30_irons_diary_dokument.c)
target_link_libraries(probe_r30_irons_diary_dokument PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_irons_diary_dokument PRIVATE
    ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(probe_r30_irons_diary_dokument PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2"
    R30_PROTOTYP_DIR="${CMAKE_SOURCE_DIR}/../build/r30_irons-diary-dokument")
