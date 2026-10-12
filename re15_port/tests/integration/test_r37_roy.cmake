cmake_minimum_required(VERSION 3.16)
# test_r37_roy.cmake — INTEGRATION Spur K (Runde 37) "roy": Elzas Roy an der ECHTEN exe.
#   cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_elza_karte> -DRE15_SPUR_TOOL=<test_r37_roy>
#         -DWORKDIR=<dir> -DTEIL=<szene1011|szene1021|leon1170> -P test_r37_roy.cmake
#
#   szene1011  ECHTER Weg: Elza-Spielstand ROOM1021 vor Tuer-Slot 1, CONTINUE, Tuer -> ROOM1011 (Roy-Szene sub02 laeuft
#              von selbst). Roy kommt aus EMD/CDEMD1.EMS Sektor 1639 (FUN_80022300 @0x800223ac), KEIN Elliot-Modell,
#              Zeichenweg = Gegner-Bank (nicht Elliot), RE15_ANIM_TRACE: RBJ-Record 1 (Clips 5/6/7/9/10 = 28/78/30/100/60),
#              Einmal-Clips halten, Ohnmacht Clip 9 endet auf Bild 99 und bleibt.
#   szene1021  Elza-Neuspiel, Flags (4,0x60)/(3,0xC9) (ROOM1211 sub02), Raum 1021, Szene ueber AOT-Ereignis 2:
#              Roy aus CDEMD1, Clip 23 (30), Gang Clip 5 (61, eigene Bank), Sturz Clip 7 (113) haelt Bild 112.
#   leon1170   Gegenprobe Leon: ROOM1170 -> Elliot wie bisher (ELLIOT.* geladen, Zeichenweg "elliot"), kein Roy-Lader.
set(_tag "r37_roy[${TEIL}]")
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_SPUR_TOOL WORKDIR TEIL)
    if(NOT ${_v})
        message(FATAL_ERROR "${_tag}: ${_v} fehlt")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_dir "${WORKDIR}/lauf_${TEIL}_${_lauf_id}")
file(MAKE_DIRECTORY "${_dir}")
# Messkopie unter eigenem Namen neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r37roy_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

macro(fail)
    file(REMOVE "${_exe}")
    message(FATAL_ERROR "${_tag}: ${ARGN} (${_dir})")
endmacro()

macro(lauf _timeout)
    set(WORKDIR "${_dir}")
    re15_start_spiel(_rv ${_timeout} ${ARGN} "${_exe}")
    if("${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1" OR NOT EXISTS "${_dir}/debug.log")
        message(STATUS "${_tag}: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
        file(REMOVE "${_dir}/debug.log" "${_dir}/anim.log")
        re15_start_spiel(_rv ${_timeout} ${ARGN} "${_exe}")
    endif()
    if(NOT EXISTS "${_dir}/debug.log")
        fail("kein debug.log (exit=${_rv})")
    endif()
endmacro()

# Roy-Lader + Zeichenweg (debug.log): genau die Zeile des Elza-Archivs, kein Elliot-Modell, Gegner-Bank.
macro(roy_geladen)
    file(STRINGS "${_dir}/debug.log" _roy REGEX "\\[roy\\] Typ 0x47 aus EMD/CDEMD1.EMS Sektor 1639 \\(142272 B\\)")
    file(STRINGS "${_dir}/debug.log" _ell REGEX "\\[elliot\\] PL05 loaded")
    file(STRINGS "${_dir}/debug.log" _diag REGEX "\\[enemy-diag\\] actor[0-9]+ type=0x47 model=")
    list(LENGTH _roy _nroy)
    list(LENGTH _ell _nell)
    if(_nroy LESS 1)
        fail("Roy nicht aus CDEMD1.EMS Sektor 1639 geladen (FUN_80022300 @0x800223ac)")
    endif()
    if(_nell GREATER 0)
        fail("Elliot-Modell in einem Elza-Raum geladen: ${_ell}")
    endif()
    if(NOT "${_diag}" MATCHES "model=enemy-bank")
        fail("Roy zeichnet nicht ueber die Gegner-Bank: '${_diag}'")
    endif()
endmacro()

macro(spur_pruefen _raum)
    execute_process(COMMAND "${RE15_SPUR_TOOL}" spur "${_dir}/anim.log" ${_raum}
                    RESULT_VARIABLE _rvs OUTPUT_VARIABLE _outs ERROR_VARIABLE _errs)
    message(STATUS "${_tag}:\n${_outs}")
    if(NOT _rvs EQUAL 0)
        fail("Spur-Auswertung exit=${_rvs}")
    endif()
endmacro()

set(_basis RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_NOAUDIO=1 RE15_ENEMY_DIAG=1)

if(TEIL STREQUAL "szene1011")
    # (3,0xC9) = 1: ROOM1021 ohne Zombies (main00 @0x01E68 Ck(3,0xC9)==0) -> Roy ist der erste Aktor 1 des Laufs und
    # bekommt die (je Aktor-Index einmalige) RE15_ENEMY_DIAG-Zeile in ROOM1011.
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" 1021 -4000 -18000 2695 3:0xC9
                    WORKING_DIRECTORY "${_dir}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0)
        fail("Kartenwerkzeug exit=${_rvk}: ${_outk}")
    endif()
    lauf(900 ${_basis} RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_ANIM_TRACE=anim.log
         "RE15_FIRE_AOT=1@40#1021" "RE15_EXIT_AT=2300#1011")
    file(STRINGS "${_dir}/debug.log" _ein REGEX "PC loaded room1011")
    if(NOT _ein)
        fail("ROOM1011 nicht ueber die Tuer betreten")
    endif()
    roy_geladen()
    spur_pruefen(1011)
elseif(TEIL STREQUAL "szene1021")
    lauf(900 ${_basis} RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
         "RE15_SET_FLAG=4:0x60,3:0xC9" RE15_GOTO_ROOM=1021 "RE15_FORCE_EVENT=2@40" RE15_ANIM_TRACE=anim.log
         "RE15_EXIT_AT=1100#1021")
    roy_geladen()
    spur_pruefen(1021)
elseif(TEIL STREQUAL "leon1170")
    # ECHTER Weg Leon: Neuspiel -> Montage ROOM1240 -> Tuer -> ROOM1170 (Erzaehler sub11 -> Selbst-Tuer -> sub15 Elliot).
    # (RE15_GOTO_ROOM=1170 ueberspringt die Montage; dort setzt kein sub11 das Elliot-Gate (4,0xF2) — gemessen 2500 Bilder.)
    lauf(900 ${_basis} RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_ANIM_TRACE=anim.log "RE15_EXIT_AT=1200#1170")
    file(STRINGS "${_dir}/debug.log" _ell REGEX "\\[elliot\\] PL05 loaded: .*\\(Raum 1170, Spawn 0x47\\)")
    file(STRINGS "${_dir}/debug.log" _roy REGEX "\\[roy\\]")
    file(STRINGS "${_dir}/debug.log" _diag REGEX "\\[enemy-diag\\] actor[0-9]+ type=0x47 model=")
    if(NOT _ell)
        fail("Elliot (Leon, ROOM1170) nicht wie bisher aus ELLIOT.* geladen")
    endif()
    if(_roy)
        fail("Roy-Lader griff bei Leon: ${_roy}")
    endif()
    if(NOT "${_diag}" MATCHES "model=elliot")
        fail("Elliot zeichnet nicht ueber den Elliot-Zweig: '${_diag}'")
    endif()
    spur_pruefen(1170)
    message(STATUS "${_tag}: Elliot unveraendert (${_ell}; ${_diag})")
else()
    fail("unbekannter TEIL")
endif()
file(REMOVE "${_exe}")
message(STATUS "${_tag}: OK")
