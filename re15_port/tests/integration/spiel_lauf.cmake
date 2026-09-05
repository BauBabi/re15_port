# ⛔ SPIEL AUS EINEM TESTHAKEN STARTEN - OHNE "cmake -E env".
#
# BEFUND 2026-09-05, gemessen: integration_relatch_pin lief in die Zeitueberschreitung,
# obwohl das Spiel seine Arbeit fertig hatte. Die letzte Zeile im debug.log war
# "[flow] BOOT_EXIT_AT: boot #3 reached -> exit" - danach ruft der Haken
# re15_testhaken_ende(), also fclose(stderr) + _exit(0).
#
# Drei Messungen, gleiche Umgebung, gleiches Arbeitsverzeichnis:
#   1. exe direkt aus der Shell            -> exit=0 nach 23/24/26 s (3 von 3)
#   2. execute_process + "cmake -E env"    -> "Process terminated due to timeout" (120 s)
#   3. execute_process + set(ENV{...})     -> rv=0 nach 62 s
#
# Der Unterschied ist allein der Zwischenprozess: execute_process wartet, bis die
# Ausgabe-Roehren geschlossen sind, und "cmake -E env" haelt sie ueber das Ende des
# Kindes hinaus offen. Seit das Spiel im Testhaken mit _exit(0) endet (2026-09-04,
# gegen eine andere Flatterhaftigkeit - s. Kopf von re15_testhaken_ende in main.c)
# laeuft kein atexit-Abbau mehr, und der Wrapper haengt sichtbar.
#
# Diese Fassung setzt die Umgebung mit set(ENV{...}) und startet die exe direkt.
# Aufruf wie bisher: KEY=WERT-Paare, danach die exe.
macro(re15_start_spiel _erg _timeout)
    set(_re15_cmd "")
    set(_re15_env "")
    foreach(_a ${ARGN})
        if(_a MATCHES "^([A-Za-z_][A-Za-z0-9_]*)=(.*)$")
            set(ENV{${CMAKE_MATCH_1}} "${CMAKE_MATCH_2}")
            list(APPEND _re15_env "${CMAKE_MATCH_1}")
        else()
            list(APPEND _re15_cmd "${_a}")
        endif()
    endforeach()
    execute_process(COMMAND ${_re15_cmd}
                    WORKING_DIRECTORY "${WORKDIR}"
                    TIMEOUT ${_timeout}
                    RESULT_VARIABLE ${_erg})
    foreach(_v ${_re15_env})
        unset(ENV{${_v}})
    endforeach()
endmacro()
