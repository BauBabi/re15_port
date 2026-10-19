cmake_minimum_required(VERSION 3.16)
# =============================================================================
# TITELBILD DER ECHTEN EXE (Runde 39, Spur T "titelversion").
#
# Dossier analysis/befunde_runde39/T_titelversion.md. Nutzer (AUFTRAG.md, Nachtrag 14): "bei dieser und zukuenftig
# neuen Versionen passe mir bitte die Version im Titelscreen an. Entferne das bestehende."
#
# Lauf: Titelabzug RE15_TITLE_SHOT bei Bild 600 (RE15_TITLE_SHOT_AF=600: die Einblende dauert 32 Titel-Ticks = 1,07 s,
# bei VSync bis 560 Hz sicher vorbei), Fensterskala 1 (320x240), ohne Intro/Ton,
# Ende per RE15_INV_SHOT (Bild 34 im Spiel). Danach:
#   - debug.log traegt "[titel_r39] alte Beschriftung entfernt: 929 Pixel; neu "<Anzeige>" (RE15_VERSION "<VERSION>")"
#     -> der Haken in render_pc.c lief, mit der von CMake bestimmten Version
#   - test_r39_titelversion pruefen <titel.bmp>: ausserhalb des Kastens x188..295 y4..35 (ohne Menue) = rohes
#     TITLEU.TIM; Zeile "Sample" weg (Maske schwarz, Rest unveraendert); Abzug == Erwartung pixelgleich
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DPRUEF_TOOL=<test_r39_titelversion> -DWORKDIR=<dir> "-DVERSION=<RE15_VERSION>"
#         -P <diese Datei>
# =============================================================================
set(_tag "r39_titelversion_titel")
foreach(_v RE15_PC_EXE PRUEF_TOOL WORKDIR)
    if(NOT ${_v})
        message(FATAL_ERROR "${_tag}: ${_v} fehlt")
    endif()
endforeach()
if(NOT EXISTS "${RE15_PC_EXE}" OR NOT EXISTS "${PRUEF_TOOL}")
    message(FATAL_ERROR "${_tag}: exe/Pruefwerkzeug fehlt ('${RE15_PC_EXE}' / '${PRUEF_TOOL}')")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Ohne echte GPU (Linux-Release-Container: Videotreiber offscreen, Mesa-GL, keine Bildrate) ist der Bildvergleich nicht
# aussagekraeftig (gemessen 2026-10-19: Titel bei Bild 150 noch in der Einblende, 23325 Abweichungen ausserhalb des
# Kastens) - er wird SICHTBAR uebersprungen wie in test_r30_titel_puls.cmake; die Engine-Pruefungen (Protokollzeile mit
# Version und 929 Pixeln) bleiben Pflicht. RE15_OHNE_GPU liest die exe nicht - es wird VOR dem Leeren gelesen.
set(_ohne_gpu 0)
if(DEFINED ENV{RE15_OHNE_GPU} AND NOT "$ENV{RE15_OHNE_GPU}" STREQUAL "" AND NOT "$ENV{RE15_OHNE_GPU}" STREQUAL "0")
    set(_ohne_gpu 1)
endif()

# Keine RE15_*-Variablen des Aufrufers erben (VERTRAG Runde 39 Abschnitt 2).
execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _umg)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _umg_namen "${_umg}")
foreach(_n ${_umg_namen})
    string(REGEX REPLACE "[\n=]" "" _n "${_n}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
# Messkopie unter eigenem Namen neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake),
# Praefix je Test und Lauf - kein Glob, der Kopien eines Geschwister-Tests loescht. Endung wie die Quelle.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r39t_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

re15_start_spiel(_rv 180
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=titel.bmp RE15_TITLE_SHOT_AF=600 RE15_INV_SHOT=ende34.bmp
    "${_exe}")
file(REMOVE "${_exe}")
if(NOT EXISTS "${WORKDIR}/titel.bmp")
    message(FATAL_ERROR "${_tag}: kein Titelabzug (exit=${_rv}, ${WORKDIR})")
endif()

file(READ "${WORKDIR}/debug.log" _dbg)
string(REGEX MATCH "\\[titel_r39\\] alte Beschriftung entfernt: ([0-9]+) Pixel; neu \"([^\"]*)\" \\(RE15_VERSION \"([^\"]*)\"\\)([^\n]*)" _zeile "${_dbg}")
if(NOT _zeile)
    message(FATAL_ERROR "${_tag}: Zeile '[titel_r39] ...' fehlt im debug.log - der Haken in render_pc.c lief nicht (${WORKDIR})")
endif()
set(_entfernt "${CMAKE_MATCH_1}")
set(_anzeige "${CMAKE_MATCH_2}")
set(_version "${CMAKE_MATCH_3}")
set(_rest "${CMAKE_MATCH_4}")
message(STATUS "${_tag}: ${_zeile}")
if(NOT _entfernt EQUAL 929)
    message(FATAL_ERROR "${_tag}: ${_entfernt} statt 929 Pixel entfernt")
endif()
if(NOT "${_version}" STREQUAL "${VERSION}")
    message(FATAL_ERROR "${_tag}: exe traegt RE15_VERSION '${_version}', CMake bestimmte '${VERSION}'")
endif()
if(NOT "${_anzeige}" MATCHES "^Ver\\." OR "${_rest}" MATCHES "NICHT GEZEICHNET")
    message(FATAL_ERROR "${_tag}: neue Zeile nicht gezeichnet ('${_anzeige}'${_rest})")
endif()

if(_ohne_gpu)
    message(STATUS "${_tag}: SKIP Bildvergleich (RE15_OHNE_GPU gesetzt - keine echte GPU); Protokollzeile geprueft: "
                   "'${_anzeige}', RE15_VERSION '${_version}', ${_entfernt} Pixel entfernt")
    file(REMOVE_RECURSE "${WORKDIR}")
    return()
endif()
execute_process(COMMAND "${PRUEF_TOOL}" pruefen "${WORKDIR}/titel.bmp"
                RESULT_VARIABLE _prv OUTPUT_VARIABLE _po ERROR_VARIABLE _pe TIMEOUT 120)
message(STATUS "${_po}")
string(REGEX MATCH "text=([^\n]*)" _m "${_po}")
if(NOT "${CMAKE_MATCH_1}" STREQUAL "${_anzeige}")
    message(FATAL_ERROR "${_tag}: Pruefwerkzeug erwartet '${CMAKE_MATCH_1}', exe zeigt '${_anzeige}'")
endif()
if(NOT _prv EQUAL 0)
    message(FATAL_ERROR "${_tag}: Titelabzug weicht ab (rc=${_prv}) ${_pe}")
endif()
file(REMOVE_RECURSE "${WORKDIR}")
message(STATUS "${_tag}: OK - '${_anzeige}' im Titelbild, alte Beschriftung weg, Rest pixelgleich")
