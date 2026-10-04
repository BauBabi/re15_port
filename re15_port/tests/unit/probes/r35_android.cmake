# --- Runde 35 (2026-10-03), Spur N "android". Dossier: analysis/befunde_runde35/N_android.md
#     PORT-WAHL-Werkzeuge (Android-Entpacker und Release-Pruefkette), kein Originalverhalten.
#
#   unit_r35_android_abgleich   asset_abgleich.c: Leser-Regeln R1 (kein Segment endet auf .neu) und R2 (kein Pfad
#                               zugleich Datei und Ordner), F-Y1-Proben, re15_abgleich_weg_frei/_leere_eltern auf einem
#                               echten Temp-Ordner (Punkt 2).
#   r35_android_pruefstand      der ECHTE platform/android/jni/android_glue.c + asset_abgleich.c mit Attrappen fuer
#                               SDL/JNI/AAssetManager (r35_android/), unter mingw mit kompat_win.h:
#     unit_r35_android_anzeige  Punkt 1 (E2-1): 11 Displaygroessen x 3 Texte - jede Textzeile und jedes Rechteck im
#                               Bild, keine Ueberlappung, der ganze Text sichtbar
#     unit_r35_android_konflikt Punkt 2 (F-Y4/H8): Update mit Datei<->Ordner-Konflikten wird im SELBEN Start fertig,
#                               Baum = APK, dritter Start schneller Weg; Listen mit R1/R2-Verstoss fail closed
#   unit_r35_android_pruefkette Punkt 3 (F-Y1/F-Y2): release/apk_pruefen.sh + gate_urteil.py + apk_asset_gate.py -
#                               Urteils-Selbsttest (Faelle + Mutanten), Gate-Selbsttest ueber gate_laufen, Negativ-
#                               Kontrollen (leere/kaputte APK, kaputtes umgepinntes Gate, verstuemmeltes Urteil) muessen
#                               ROT werden (r35_android/test_r35_android_pruefkette.sh; nur wenn bash da ist)
add_executable(test_r35_android_abgleich
    test_r35_android_abgleich.c
    ${CMAKE_SOURCE_DIR}/platform/android/jni/asset_abgleich.c)
target_include_directories(test_r35_android_abgleich PRIVATE ${CMAKE_SOURCE_DIR}/platform/android/jni)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r35_android_abgleich)
add_test(NAME unit_r35_android_abgleich COMMAND test_r35_android_abgleich
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/r35_android_abgleich)
set_tests_properties(unit_r35_android_abgleich PROPERTIES TIMEOUT 60)

add_executable(r35_android_pruefstand
    ${CMAKE_CURRENT_SOURCE_DIR}/r35_android/pruefstand_main.c
    ${CMAKE_SOURCE_DIR}/platform/android/jni/android_glue.c
    ${CMAKE_SOURCE_DIR}/platform/android/jni/asset_abgleich.c)
# Attrappen VOR allem anderen (kein echtes SDL), jni/ fuer asset_abgleich.h
target_include_directories(r35_android_pruefstand BEFORE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/r35_android/stub ${CMAKE_SOURCE_DIR}/platform/android/jni)
set_target_properties(r35_android_pruefstand PROPERTIES C_EXTENSIONS ON)   # off64_t (glibc/mingw)
if(WIN32)
    set_source_files_properties(${CMAKE_SOURCE_DIR}/platform/android/jni/android_glue.c
        TARGET_DIRECTORY r35_android_pruefstand
        PROPERTIES COMPILE_OPTIONS "-include;${CMAKE_CURRENT_SOURCE_DIR}/r35_android/kompat_win.h")
else()
    target_compile_definitions(r35_android_pruefstand PRIVATE _GNU_SOURCE=1)
endif()
foreach(_r35n_teil anzeige konflikt)
    add_test(NAME unit_r35_android_${_r35n_teil}
             COMMAND ${CMAKE_COMMAND} -DPRUEFSTAND=$<TARGET_FILE:r35_android_pruefstand>
                     -DARBEIT=${CMAKE_CURRENT_BINARY_DIR}/r35_android_pruefstand -DTEIL=${_r35n_teil}
                     -P ${CMAKE_CURRENT_SOURCE_DIR}/r35_android/test_r35_android_entpacker.cmake)
    set_tests_properties(unit_r35_android_${_r35n_teil} PROPERTIES TIMEOUT 240)
endforeach()
