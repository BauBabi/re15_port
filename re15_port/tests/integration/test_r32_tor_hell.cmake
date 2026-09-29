# =============================================================================
# TOR-HELLIGKEIT (Runde 32, Nutzer 2026-09-29: "Das von uns erstellte Tor in ROOM 1170 sieht gut
# aus, aber ist zu dunkel.") Dossier: analysis/befunde_runde32/tor_helligkeit.md
#
# Faehrt die ECHTE re15_pc.exe (beschleunigter Renderer; KEIN RE15_SOFTWARE_RENDER) mit dem
# Pruefhaken RE15_TUER_TEST=1 (Tor Variante 1) und RE15_TUER_BOGEN=<dir>: die Szene legt
# S000_anfang.ppm ab (Bild 20 nach dem Einblenden; im Standbild 16..129 ist jedes Bild
# pixelgleich mit Bild 100, gemessen). RE15_WINDOW_SCALE=1 -> 320 x 240 = logische Pixel.
# Pruefer: probe_r32_tor bild <ppm> (Schwellen und Herleitung im Kopf von probe_r32_tor.c).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DPRUEFER=<probe_r32_tor> -DWORKDIR=<dir> -P test_r32_tor_hell.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r32_tor_hell: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT PRUEFER OR NOT EXISTS "${PRUEFER}")
    message(FATAL_ERROR "r32_tor_hell: PRUEFER fehlt/existiert nicht: '${PRUEFER}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r32_tor_hell: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}/bogen")

re15_start_spiel(_rv 90
    RE15_NOAUDIO=1
    RE15_WINDOW_SCALE=1
    RE15_TUER_TEST=1
    RE15_TUER_SCHNELL=1
    RE15_TUER_BOGEN=${WORKDIR}/bogen
    "${RE15_PC_EXE}")

set(_bild "${WORKDIR}/bogen/S000_anfang.ppm")
if(NOT EXISTS "${_bild}")
    message(FATAL_ERROR "r32_tor_hell: kein Bogenbild ${_bild} (exit=${_rv})")
endif()
execute_process(COMMAND "${PRUEFER}" bild "${_bild}"
                RESULT_VARIABLE _pr OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
message(STATUS "${_out}${_err}")
if(NOT _pr EQUAL 0)
    message(FATAL_ERROR "r32_tor_hell: Pruefer meldet Fehler (exit=${_pr})")
endif()
file(REMOVE_RECURSE "${WORKDIR}")
