# =============================================================================
# Runde 39 Spur J "dining1141", Nachbesserung 3 (Abnahme 2 Hinweis H4; VERTRAG Abschnitt 2: Riegel parallel- und
# Linux-fest, "keine Umgebungsvariablen des Aufrufers erben"): vor dem Spielstart JEDE RE15_*-Variable aus der Umgebung
# des Aufrufers (ctest, Shell, ein Messlauf mit RE15_FRAMEDUMP/RE15_INPUT_SCRIPT/RE15_R38F_GORILLAS_TOT ...) entfernen.
# Danach gelten nur die Werte, die der Riegel selbst an re15_start_spiel (spiel_lauf.cmake) uebergibt.
# Die Namen kommen aus "cmake -E environment" (die ganze Umgebung), nicht aus einer festen Liste - eine spaeter
# hinzugekommene Mess-Variable kann so nicht durchsickern. Eingebunden von test_r39_dining1141.cmake,
# test_r39_dining1141_kampf.cmake, test_r39_dining1141_zielen.cmake und test_r38_dining1141.cmake.
# =============================================================================
macro(r39j_umgebung_leeren)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E environment
                    OUTPUT_VARIABLE _r39j_umg RESULT_VARIABLE _r39j_umg_rv)
    if(NOT _r39j_umg_rv EQUAL 0)
        message(FATAL_ERROR "r39j_umgebung_leeren: cmake -E environment rv=${_r39j_umg_rv}")
    endif()
    string(REGEX MATCHALL "RE15_[A-Za-z0-9_]*=" _r39j_umg_namen "${_r39j_umg}")
    set(_r39j_umg_weg "")
    foreach(_r39j_n IN LISTS _r39j_umg_namen)
        string(REGEX REPLACE "=$" "" _r39j_n "${_r39j_n}")
        if(DEFINED ENV{${_r39j_n}})
            unset(ENV{${_r39j_n}})
            list(APPEND _r39j_umg_weg "${_r39j_n}")
        endif()
    endforeach()
    list(LENGTH _r39j_umg_weg _r39j_umg_k)
    message(STATUS "r39j: ${_r39j_umg_k} RE15_*-Variablen des Aufrufers entfernt: ${_r39j_umg_weg}")
endmacro()
