# =============================================================================
# Runde 39 Spur R "hund1121" — ECHTE exe, Elza-Spiel: in der Panikphase ((9,97) = 1) springt in ROOM1121 ein Hund durch
# das Fenster, durch das bei Leon in ROOM1120 die Kraehe hereinfliegt. Dossier analysis/befunde_runde39/R_hund1121.md.
# Registrierung: tests/unit/probes/r39_hund1121.cmake.
#
# Weg (VERTRAG: Story-/Raum-Punkte ueber den echten Weg): Elza-Spielstand in ROOM1121 am Ankunftsplatz der Tuer 0 aus
# ROOM1061 ((150,0,9550) Gier 0 = ROOM1061 main00 @0x00D12 Door_aot_set Ziel), CONTINUE, dann laeuft sie wie beim Durchlauf
# nach Osten, dreht nach Sueden und rennt den Gang hinunter durchs Band (Kamera Cut 0 -> Cut 1 ueber die RVD).
#   A  (9,97) = 1          : Log-Kette (scharf, Ausloeser, Hund (5400,-1827,12100), Sub 14, Freigabe, Glas, Wand zu,
#                            Knall 1/2, fertig Sub 2), Spawn-Diagnose Typ 0x20; Mitschnitt: Spitze am Knall >= 3 x Median
#                            und > Spitze von B im selben Fenster; Bild: im Wartebild weicht die rechte Scheibe von B ab.
#   B  ohne (9,97)         : kein Hund, Log "aus: (9,97)=0".
#   C  (9,97) + (7,0xAE)   : schon gesprungen -> kein Hund.
#   D  Leon ROOM1120       : (9,97) + (9,73): Leons Kraehen-Ereignis wie Runde 35, kein Hund.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_WERKZEUG=<test_r39_hund1121> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
cmake_minimum_required(VERSION 3.16)
set(_tag "r39_hund1121")
foreach(_v RE15_PC_EXE RE15_WERKZEUG WORKDIR)
    if(NOT ${_v})
        message(FATAL_ERROR "${_tag}: ${_v} fehlt")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Keine Umgebungsvariablen des Aufrufers erben (VERTRAG §2): jedes RE15_* aus der Umgebung entfernen.
execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _umg)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _umg_namen "${_umg}")
foreach(_n ${_umg_namen})
    string(REGEX REPLACE "[\n=]" "" _n "${_n}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_basis}")
# Eigene Messkopie neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake), Praefix je Test.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r39r_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

macro(fail)
    file(REMOVE "${_exe}")
    message(FATAL_ERROR "${_tag}: ${ARGN} (${_basis})")
endmacro()

set(_weg "XU1.5,R0.35,XU2.5")
# Lauf <name> <ton 0/1> <karte-args...>: Spielstand schreiben, CONTINUE, Weg fahren.
function(lauf _name _ton)
    set(WORKDIR "${_basis}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_WERKZEUG}" karte "re15_card.mcr" ${ARGN}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _ok)
    if(NOT _rvk EQUAL 0)
        file(REMOVE "${_exe}")
        message(FATAL_ERROR "${_tag}[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_ok}")
    endif()
    list(GET ARGN 0 _raum)
    set(_audio RE15_NOAUDIO=1)
    if(_ton)
        set(_audio RE15_AUDIO_CAP_SYNC=cap.raw "RE15_FRAMEDUMP=140-200/1:bild_")
    endif()
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 ${_audio}
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=70 "RE15_INPUT_SCRIPT=${_weg}"
        RE15_SPAWN_DIAG=1 RE15_HUND1121_LOG=hund.log RE15_FENSTER_LOG=fenster.log
        "RE15_EXIT_AT=260#${_raum}"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(STATUS "${_tag}[${_name}]: kein debug.log (exit=${_rv}) -> EIN Wiederholungsversuch")
        re15_start_spiel(_rv 300
            RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 ${_audio}
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
            RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=70 "RE15_INPUT_SCRIPT=${_weg}"
            RE15_SPAWN_DIAG=1 RE15_HUND1121_LOG=hund.log RE15_FENSTER_LOG=fenster.log
            "RE15_EXIT_AT=260#${_raum}"
            "${_exe}")
    endif()
    if(NOT EXISTS "${WORKDIR}/debug.log")
        file(REMOVE "${_exe}")
        message(FATAL_ERROR "${_tag}[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    set(_h "")
    if(EXISTS "${WORKDIR}/hund.log")
        file(READ "${WORKDIR}/hund.log" _h)
    endif()
    file(STRINGS "${WORKDIR}/debug.log" _sp REGEX "\\[spawn-diag\\] Sce_em_set type=0x20 ")
    file(STRINGS "${WORKDIR}/debug.log" _cont REGEX "CONTINUE: resumed in room")
    message(STATUS "${_tag}[${_name}] exit=${_rv} ${_cont}\n${_h}")
    set(R39R_H "${_h}" PARENT_SCOPE)
    set(R39R_SP "${_sp}" PARENT_SCOPE)
    set(R39R_CONT "${_cont}" PARENT_SCOPE)
endfunction()

# ---- A: Panikphase, echter Weg -------------------------------------------------------------------
lauf(A 1 1121 150 9550 0 9:97)
if(NOT R39R_CONT MATCHES "room 1121")
    fail("A: Spielstand ROOM1121 nicht geladen")
endif()
foreach(_muss "ROOM1121 scharf: Slot 4 Band x 3500..6650 z 4300..6400" "Wand Band 1 Region 0..4 Index 15: gefunden 1"
              "Ausloeser: Spieler" "Hund Slot 5 gesetzt bei (5400,-1827,12100) RE2-Hirn 1"
              "RE2: Sub 14 Phase 0" "Freigabe (RE2=1), Takt 2, Hund (5400,-1827,12100)" "13 Splitter"
              "Wand Band 1 wieder zu" "Knall 1:" "3 Bilder nach dem Glas" "Knall 2:" "8 Bilder nach dem Glas"
              "Zustand 1 Sub 2")
    string(FIND "${R39R_H}" "${_muss}" _f)
    if(_f LESS 0)
        fail("A: '${_muss}' fehlt in hund.log")
    endif()
endforeach()
if(NOT R39R_SP MATCHES "type=0x20 behavior=0x00 slot=4 pos=\\(5400,-1827,12100\\) dir=1048")
    fail("A: Spawn-Diagnose ohne den Hund (5400,-1827,12100) dir 1048: '${R39R_SP}'")
endif()
string(REGEX MATCH "F([0-9]+) \\[hund1121\\] Ausloeser: Spieler \\(([-0-9]+),([-0-9]+)\\)" _m "${R39R_H}")
set(_f_ausl ${CMAKE_MATCH_1})
set(_ausl_z ${CMAKE_MATCH_3})
if(_ausl_z LESS 4300 OR _ausl_z GREATER 6400)
    fail("A: Ausloeser ausserhalb des Bands (z ${_ausl_z})")
endif()
string(REGEX MATCH "F([0-9]+) \\[hund1121\\] Freigabe" _m "${R39R_H}")
set(_f_frei ${CMAKE_MATCH_1})
string(REGEX MATCH "F([0-9]+) \\[hund1121\\] Glas: Hund bei \\(([-0-9]+),([-0-9]+),([-0-9]+)\\)" _m "${R39R_H}")
set(_f_glas ${CMAKE_MATCH_1})
set(_glas_y ${CMAKE_MATCH_3})
set(_glas_z ${CMAKE_MATCH_4})
string(REGEX MATCH "F([0-9]+) \\[hund1121\\] Knall 1" _m "${R39R_H}")
set(_f_k1 ${CMAKE_MATCH_1})
string(REGEX MATCH "F([0-9]+) \\[hund1121\\] Knall 2" _m "${R39R_H}")
set(_f_k2 ${CMAKE_MATCH_1})
# Glasbruch in Scheibenhoehe (Fuesse des Hundes zwischen Rahmen-Unterkante -1827 und Scheiben-Oberkante -3049).
if(_glas_z GREATER 11200 OR _glas_y GREATER -1827 OR _glas_y LESS -3049)
    fail("A: Glas bei y ${_glas_y} z ${_glas_z} (soll z <= 11200, y -3049..-1827)")
endif()
message(STATUS "${_tag}[A]: Ausloeser F${_f_ausl}, Freigabe F${_f_frei}, Glas F${_f_glas}, Knall F${_f_k1}/F${_f_k2}")

# ---- B: ohne Panik --------------------------------------------------------------------------------
lauf(B 1 1121 150 9550 0)
string(FIND "${R39R_H}" "ROOM1121 aus: (9,97)=0" _f)
if(_f LESS 0 OR R39R_SP)
    fail("B: ohne (9,97) gab es ein Ereignis/einen Hund")
endif()

# Mitschnitt: Knall 1 und 2 (je 6 Bilder ab dem Log-Bild) in A gegen den Median und gegen B im selben Fenster.
foreach(_l A B)
    execute_process(COMMAND "${RE15_WERKZEUG}" mitschnitt "${_basis}/${_l}/cap.raw" 260 ${_f_k1} 6 ${_f_k2} 6
                    RESULT_VARIABLE _rva OUTPUT_VARIABLE _outa)
    if(NOT _rva EQUAL 0)
        fail("${_l}: Mitschnitt-Auswertung exit=${_rva}: ${_outa}")
    endif()
    message(STATUS "${_tag}[${_l}] Mitschnitt: ${_outa}")
    string(REGEX MATCHALL "spitze [0-9]+" _sp_${_l} "${_outa}")
    string(REGEX MATCH "median ([0-9]+)" _mm "${_outa}")
    set(_median_${_l} ${CMAKE_MATCH_1})
endforeach()
list(LENGTH _sp_A _nsp)
if(NOT _nsp EQUAL 2)
    fail("Mitschnitt A: keine zwei Fenster")
endif()
foreach(_k 0 1)
    list(GET _sp_A ${_k} _a)
    list(GET _sp_B ${_k} _b)
    string(REPLACE "spitze " "" _a "${_a}")
    string(REPLACE "spitze " "" _b "${_b}")
    math(EXPR _min "${_median_A} * 3")
    if(_a LESS _min OR NOT _a GREATER _b)
        fail("Mitschnitt: Knall ${_k}: Spitze A ${_a} (Median ${_median_A}, B ${_b}) — soll >= 3 x Median und > B")
    endif()
endforeach()

# Bild: im Wartebild (Freigabe-Bild) steht der Hund in der rechten Scheibe — dort weicht A von B ab, im linken Fluegel nicht
# (Bildlage der Scheiben Runde 35 M §1: x 182..208 / y 83..101; rechte Scheibe x 196..207).
math(EXPR _bf "${_f_frei}")
foreach(_dd A B)
    if(NOT EXISTS "${_basis}/${_dd}/bild_000${_bf}.ppm")
        fail("Framedump bild_000${_bf}.ppm fehlt in ${_dd}")
    endif()
endforeach()
execute_process(COMMAND "${RE15_WERKZEUG}" bild "${_basis}/A/bild_000${_bf}.ppm" "${_basis}/B/bild_000${_bf}.ppm" 196 80 207 101
                OUTPUT_VARIABLE _ob)
execute_process(COMMAND "${RE15_WERKZEUG}" bild "${_basis}/A/bild_000${_bf}.ppm" "${_basis}/B/bild_000${_bf}.ppm" 183 84 193 100
                OUTPUT_VARIABLE _ob2)
message(STATUS "${_tag}: Bild F${_bf} rechte Scheibe: ${_ob}  linke Scheibe: ${_ob2}")
string(REGEX MATCH "verschieden ([0-9]+)" _m "${_ob}")
set(_vr ${CMAKE_MATCH_1})
string(REGEX MATCH "verschieden ([0-9]+)" _m "${_ob2}")
set(_vl ${CMAKE_MATCH_1})
if(_vr LESS 25 OR NOT _vl LESS 5)
    fail("Bild F${_bf}: rechte Scheibe ${_vr} verschiedene Pixel (soll >= 25), linke ${_vl} (soll < 5)")
endif()

# ---- C: schon gesprungen --------------------------------------------------------------------------
lauf(C 0 1121 150 9550 0 9:97 7:0xAE)
string(FIND "${R39R_H}" "ROOM1121 aus: (9,97)=1 (7,0xAE)=1" _f)
if(_f LESS 0 OR R39R_SP)
    fail("C: mit (7,0xAE)=1 gab es ein Ereignis/einen Hund")
endif()

# ---- D: Leon ROOM1120 ------------------------------------------------------------------------------
lauf(D 0 1120 150 9550 0 9:97 9:73)
if(NOT R39R_CONT MATCHES "room 1120")
    fail("D: Spielstand ROOM1120 nicht geladen")
endif()
if(R39R_SP OR NOT "${R39R_H}" STREQUAL "")
    fail("D: Leon ROOM1120 mit Hund/Hunde-Log: '${R39R_H}'")
endif()
file(READ "${_basis}/D/fenster.log" _fl)
string(FIND "${_fl}" "ROOM1120 scharf" _f1)
string(FIND "${_fl}" "Ausloeser" _f2)
if(_f1 LESS 0 OR _f2 LESS 0)
    fail("D: Leons Kraehen-Ereignis fehlt:\n${_fl}")
endif()

file(REMOVE "${_exe}")
file(REMOVE_RECURSE "${_basis}")
message(STATUS "${_tag}: OK")
