# --- Runde 34a (2026-09-30), N1 "Geraete-Entpacker update-sicher" (Android).
#     Dossier: analysis/befunde_runde34_android/android_entpacker_n1.md
#     Der Test uebersetzt GENAU die ausgelieferte Datei platform/android/jni/asset_abgleich.c
#     (reines C, ohne SDL/Android - dieselbe Quelle linkt jni/CMakeLists.txt in libmain.so):
#     SHA-256 gegen FIPS-180-2-Vektoren, Asset-Liste Format v2 (Kopfzeile, Zeilen, Pfadregeln,
#     v1 abgelehnt, fail closed) und der Abgleich gegen "zuletzt entpackt" (Befund N1a: gleiche
#     Groesse, andere Summe -> GEAENDERT). PORT-WAHL, kein Originalverhalten. ---
add_executable(test_r34a_asset_abgleich
    test_r34a_asset_abgleich.c
    ${CMAKE_SOURCE_DIR}/platform/android/jni/asset_abgleich.c)
target_include_directories(test_r34a_asset_abgleich PRIVATE ${CMAKE_SOURCE_DIR}/platform/android/jni)
add_test(NAME unit_r34a_asset_abgleich COMMAND test_r34a_asset_abgleich)
set_tests_properties(unit_r34a_asset_abgleich PROPERTIES TIMEOUT 60)
