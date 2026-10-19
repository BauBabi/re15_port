# Runde 39 (2026-10-19), Spur T "titelversion" — Riegel, zaehlen zur Suite. Dossier: analysis/befunde_runde39/T_titelversion.md.
#
# Nutzer (AUFTRAG.md, Nachtrag 14): "bei dieser und zukuenftig neuen Versionen passe mir bitte die Version im Titelscreen
# an. Entferne das bestehende." Titelbild DATA/TITLEU.TIM: "Sample" / "Ver.2025.01.25" (929 graue Pixel, x198..283
# y11..33) -> beim Umwandeln im Speicher entfernt (0x8000), "Ver." + RE15_VERSION in der Pixelschrift der alten Zeile
# gezeichnet (engine/src/titelversion_r39.c, Haken render_pc.c re15_render_pc_show_title). RE15_VERSION:
# cmake/re15_version_r39.cmake (-D > Umgebung > letzter Tag + " dev"), Release: release_alles.sh -> build_*.sh.
#
#   unit_r39_titelversion_glyphen   Ausschnitte V e r . 0 1 2 5 bytegleich mit TITLEU.TIM (dokumentierte Stellen);
#                                   alle 16 Glyphen im Raster der alten Zeile (eigenes Flaechenmodell 1,12/1,52/24,88 px),
#                                   die ergaenzten 3 4 6 7 8 9 d v mit hoechstens der Abweichung der Ausschnitte
#   unit_r39_titelversion_text      Version -> Anzeige ("v0.8.27" -> "Ver.0.8.27", Zusatz -> " dev")
#   unit_r39_titelversion_layout    die alte Zeile landet mit den Konstanten an ihren Original-Spalten (+-1), neue
#                                   Zeilen rechtsbuendig an 283,60 px, Abstaende 0,72 / 2,74 px
#   unit_r39_titelversion_maske     929 Pixel, bbox x198..283 y11..33, Ring 608 x 0x8000, Kunst-Staub bleibt
#   unit_r39_titelversion_anwenden  ausserhalb Maske + neuer Kacheln bytegleich, Maske -> 0x8000, neue Zeile exakt,
#                                   TITLEU.TIM auf der Platte bytegleich
#   unit_r39_titelversion_version   einkompilierte Version == die von CMake bestimmte
#   unit_r39_titelversion_cmake     Versionsquelle: -D > Umgebung > git-Tag + " dev" > "dev" (cmake -P)
#   unit_r39_titelversion_huelle    echte build_win_cross.sh / build_linux_deck.sh: docker run -e RE15_VERSION=...
#   unit_r39_titelversion_ablauf    echtes release_alles.sh: windows/linux RE15_VERSION=<neu>, android --version <neu>
#   integration_r39_titelversion_titel  (nur mit re15_pc) echter Titelabzug: debug.log-Zeile mit der CMake-Version,
#                                   ausserhalb des Kastens rohes TITLEU.TIM, "Sample" weg, Abzug == Erwartung pixelgleich
include(re15_version_r39)   # nur die Funktion (das Modul ist include_guard(GLOBAL); engine/CMakeLists.txt laedt es zuerst)
re15_version_r39_ermitteln(_r39t_version)

add_executable(test_r39_titelversion ${CMAKE_CURRENT_LIST_DIR}/../test_r39_titelversion.c)
target_link_libraries(test_r39_titelversion PRIVATE re15_engine)
target_include_directories(test_r39_titelversion PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_r39_titelversion PRIVATE RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    "RE39T_ERWARTET=\"${_r39t_version}\"")
foreach(_teil glyphen text layout maske anwenden version)
    add_test(NAME unit_r39_titelversion_${_teil} COMMAND test_r39_titelversion ${_teil}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(unit_r39_titelversion_${_teil} PROPERTIES TIMEOUT 120)
endforeach()

add_test(NAME unit_r39_titelversion_cmake
         COMMAND "${CMAKE_COMMAND}" -DMODUL=${CMAKE_SOURCE_DIR}/cmake/re15_version_r39.cmake
                 -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}/r39_titelversion_cmake_wd
                 -P ${CMAKE_CURRENT_LIST_DIR}/../r39_titelversion/test_r39_titelversion_cmake.cmake)
set_tests_properties(unit_r39_titelversion_cmake PROPERTIES TIMEOUT 120)

# bash: unter Windows NUR Git-Bash (wie r38_releaseskript.cmake - nie System32\bash.exe/WSL); braucht git und sha256sum.
if(WIN32)
    find_program(R39T_BASH NAMES bash
                 PATHS "C:/Program Files/Git/bin" "C:/Program Files/Git/usr/bin" "C:/msys64/usr/bin" NO_DEFAULT_PATH)
else()
    find_program(R39T_BASH NAMES bash)
endif()
get_filename_component(_r39t_repo "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
if(R39T_BASH AND EXISTS "${_r39t_repo}/release/release_alles.sh" AND EXISTS "${_r39t_repo}/release/build_win_cross.sh")
    foreach(_teil huelle ablauf)
        add_test(NAME unit_r39_titelversion_${_teil}
                 COMMAND "${R39T_BASH}" "${CMAKE_CURRENT_LIST_DIR}/../r39_titelversion/test_r39_titelversion_release.sh"
                         "${_r39t_repo}" "${CMAKE_CURRENT_BINARY_DIR}/r39_titelversion_wd_${_teil}" ${_teil})
        set_tests_properties(unit_r39_titelversion_${_teil} PROPERTIES TIMEOUT 600)
    endforeach()
else()
    message(STATUS "r39_titelversion: bash oder release/*.sh fehlt - unit_r39_titelversion_huelle/_ablauf nicht registriert")
endif()

if(TARGET re15_pc)
    add_test(NAME integration_r39_titelversion_titel
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DPRUEF_TOOL=$<TARGET_FILE:test_r39_titelversion>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r39_titelversion_titel_wd
                     "-DVERSION=${_r39t_version}"
                     -P ${CMAKE_SOURCE_DIR}/tests/integration/test_r39_titelversion_titel.cmake)
    set_tests_properties(integration_r39_titelversion_titel PROPERTIES TIMEOUT 300)
endif()
