# =============================================================================
# RIEGEL ROOM1171 MIT DER ECHTEN EXE (Runde 39 Spur K "heliport1171", Nutzer: "Wenn John in Room 1171 sagt - "Elza
# hurry up" macht er so eine komische Kletter Animation statt der ran wink Animation ... Auch sonst scheinen die
# Animationen hier noch nicht ganz zu passen.")
#
# Dossier analysis/befunde_runde39/K_heliport1171.md. Gemessen wird der POSE-STROM (RE15_ANIM_TRACE, geschrieben in
# anim_select_common.c an der einzigen Stelle, an der der Renderer eine Pose aufloest): Spalten Bild, Aktor, Typ, motion,
# Clip, Bildzahl des GERENDERTEN Clips, ... Johns (Typ 0x49 = 73) Sub-0-Gesten muessen aus RBJ rec1 kommen (Marker-
# Binder FUN_8001b3f8, +0x180/+0x184 @0x8001b4a0/@0x8001b4bc; Sub 0 @0x80050d40/48):
#   Plc_motion(0,2,0) @0x014CE ("Elza, hurry up!", Heran-Winken) = RBJ rec1 Clip 2 = 96 Bilder — NICHT Bank 0 Clip 2
#   = 50 Bilder (Kletter-Clip, +0x94 = 2 @0x800526a0); Plc_motion(0,0,0) @0x01416/@0x0141E/@0x01526/@0x0152E = RBJ rec1
#   Clip 0 = 20 Bilder — NICHT Bank 0 Clip 0 = 34; Plc_motion(2,6,0) @0x0142A/@0x014C2 = eigene Bank Clip 6 = 10 Bilder.
# Gegenprobe vor dem Fix (1038dc15, build_bisect): motion 2 rendert 50 Bilder, motion 0 rendert 34 -> ROT.
#
#   weg    Spielstand ROOM1131 (Elza, (3,0xC2)=1 wie nach ROOM1141 sub02 @0x00C20) vor der Flurtuer Slot 3, Aktionstaste
#          -> ROOM1171 (Cut 8, kein Ereignis), Selbst-Tuer Slot 5 (@0x012F0) per RE15_FIRE_AOT -> Szenario 0 -> sub07
#          -> sub02 -> ROOM10B1 (Weg wie test_r38_heliport1171 weg_re2)
#   laden  Spielstand direkt in ROOM1171 + CONTINUE
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_elza_karte> -DWORKDIR=<dir> -DTEIL=weg|laden
#               -P test_r39_heliport1171.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.16)
set(_tag "r39_heliport1171[${TEIL}]")
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL WORKDIR TEIL)
    if(NOT ${_v})
        message(FATAL_ERROR "${_tag}: ${_v} fehlt")
    endif()
endforeach()
if(NOT EXISTS "${RE15_PC_EXE}" OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "${_tag}: exe/Kartenwerkzeug fehlt ('${RE15_PC_EXE}' / '${RE15_KARTE_TOOL}')")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Keine RE15_*-Variablen des Aufrufers erben (VERTRAG §2): alles, was die Umgebung traegt, wird vorher geloescht.
execute_process(COMMAND "${CMAKE_COMMAND}" -E environment OUTPUT_VARIABLE _envdump)
string(REGEX MATCHALL "(^|\n)RE15_[A-Za-z0-9_]*=" _geerbt "${_envdump}")
foreach(_g ${_geerbt})
    string(REGEX REPLACE "^\n?(RE15_[A-Za-z0-9_]*)=$" "\\1" _n "${_g}")
    unset(ENV{${_n}})
endforeach()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/${TEIL}_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
# Messkopie unter eigenem Namen neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake), Praefix
# je Test und Lauf — kein Glob, der Kopien eines Geschwister-Tests loescht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r39k_${TEIL}_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

macro(fail)
    file(REMOVE "${_exe}")
    message(FATAL_ERROR "${_tag}: ${ARGN} (${WORKDIR})")
endmacro()

set(_weg "")
if(TEIL STREQUAL "weg")
    set(_karte 1131 2700 15300 1024 3:194)
    set(_weg RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=30 RE15_INPUT_SCRIPT=A0.5 "RE15_FIRE_AOT=5@60#1171")
elseif(TEIL STREQUAL "laden")
    set(_karte 1171 2300 14365 1024 3:194)
else()
    fail("unbekannter TEIL")
endif()

execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_karte}
                WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
    fail("Kartenwerkzeug exit=${_rvk}: ${_outk}")
endif()

re15_start_spiel(_rv 600
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
    RE15_EVT_TRACE=1 RE15_MSG_LOG=1 RE15_ANIM_TRACE=anim.txt
    "RE15_EXIT_AT=60#10b1"
    ${_weg}
    "${_exe}")
file(REMOVE "${_exe}")
if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/anim.txt")
    message(FATAL_ERROR "${_tag}: kein debug.log/anim.txt (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)

function(_muss _was _warum)
    string(FIND "${_dbg}" "${_was}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "${_tag}: fehlt '${_was}' - ${_warum} (exit=${_rv}, ${WORKDIR})")
    endif()
endfunction()
if(_weg)
    _muss("DOOR FIRE slot=3" "Flurtuer ROOM1131 Slot 3 (@0x008EE) nicht benutzt")
    _muss("DOOR FIRE slot=5" "Selbst-Tuer Slot 5 (@0x012F0) nicht gefeuert")
endif()
_muss("room=1171 Evt_exec sub=2" "sub02 (Evt_exec @0x01704) nicht gestartet")
string(FIND "${_dbg}" "[msg] room=1171 id=0 " _m0)
string(FIND "${_dbg}" "[msg] room=1171 id=1 " _m1)
if(_m0 LESS 0 OR _m1 LESS _m0)
    message(FATAL_ERROR "${_tag}: msg 0 'Elza, hurry up!' / msg 1 nicht in Folge (${_m0}/${_m1}) (${WORKDIR})")
endif()
_muss("PC loaded room10b1.rdt" "Szene nicht bis zum Abstieg nach ROOM10B1 gelaufen")
_muss("EXIT_AT: Bild 60 in Raum 10b1" "Lauf vor dem Endbild abgerissen")

# Pose-Strom: nur Johns (73) und die Kraehen-Zeilen (33); Abschnitt = Bildzaehler faellt (Raumwechsel). Der Szenen-
# Abschnitt ist der erste, in dem John UND Kraehen gerendert werden (ROOM1171 Szenario 0; ROOM10B1 hat keine Kraehen,
# ROOM1171 Cut 8 und ROOM1131 keinen John).
file(STRINGS "${WORKDIR}/anim.txt" _zz REGEX "^[0-9]+ [0-9]+ (73|33) ")
set(_abschn 0)
set(_vor -1)
set(_szene -1)
set(_john_in "")
set(_kraehe_in "")
foreach(_z IN LISTS _zz)
    if(NOT _z MATCHES "^([0-9]+) [0-9]+ ([0-9]+) ")
        continue()
    endif()
    set(_f ${CMAKE_MATCH_1})
    set(_t ${CMAKE_MATCH_2})
    if(_f LESS _vor)
        math(EXPR _abschn "${_abschn} + 1")
    endif()
    set(_vor ${_f})
    if(_t EQUAL 73)
        list(APPEND _john_in ${_abschn})
    else()
        list(APPEND _kraehe_in ${_abschn})
    endif()
endforeach()
list(REMOVE_DUPLICATES _john_in)
list(REMOVE_DUPLICATES _kraehe_in)
foreach(_a IN LISTS _john_in)
    list(FIND _kraehe_in ${_a} _k)
    if(_k GREATER -1 AND _szene LESS 0)
        set(_szene ${_a})
    endif()
endforeach()
if(_szene LESS 0)
    fail("kein Abschnitt mit John (73) und Kraehen (33) im Pose-Strom - Szene nicht gerendert")
endif()

# Zaehlen im Szenen-Abschnitt: Spalten Bild Aktor Typ motion Clip fc cur slot ...
set(_abschn 0)
set(_vor -1)
set(_winken 0)
set(_klettern 0)
set(_c0_rbj 0)
set(_c0_bank0 0)
set(_c6_eigen 0)
foreach(_z IN LISTS _zz)
    if(NOT _z MATCHES "^([0-9]+) [0-9]+ ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ")
        continue()
    endif()
    set(_f ${CMAKE_MATCH_1})
    if(_f LESS _vor)
        math(EXPR _abschn "${_abschn} + 1")
    endif()
    set(_vor ${_f})
    if(NOT _abschn EQUAL _szene OR NOT CMAKE_MATCH_2 EQUAL 73)
        continue()
    endif()
    set(_mo ${CMAKE_MATCH_3})
    set(_cl ${CMAKE_MATCH_4})
    set(_fc ${CMAKE_MATCH_5})
    if(_mo EQUAL 2 AND _cl EQUAL 2 AND _fc EQUAL 96)
        math(EXPR _winken "${_winken} + 1")
    endif()
    if(_mo EQUAL 2 AND _cl EQUAL 2 AND _fc EQUAL 50)
        math(EXPR _klettern "${_klettern} + 1")
    endif()
    if(_mo EQUAL 0 AND _cl EQUAL 0 AND _fc EQUAL 20)
        math(EXPR _c0_rbj "${_c0_rbj} + 1")
    endif()
    if(_mo EQUAL 0 AND _cl EQUAL 0 AND _fc EQUAL 34)
        math(EXPR _c0_bank0 "${_c0_bank0} + 1")
    endif()
    if(_mo EQUAL 6 AND _cl EQUAL 6 AND _fc EQUAL 10)
        math(EXPR _c6_eigen "${_c6_eigen} + 1")
    endif()
endforeach()
message(STATUS "${_tag}: Szenen-Abschnitt ${_szene}: Heran-Winken (Clip 2/96) ${_winken} Bilder, Kletter-Clip (2/50) "
               "${_klettern}, RBJ-Clip 0 (20) ${_c0_rbj}, Bank-0-Clip 0 (34) ${_c0_bank0}, eigener Clip 6 (10) ${_c6_eigen}")
# Plc_motion(0,2,0) @0x014CE bis zum naechsten Befehl (Plc_dest Modus 9 @0x014F2 hinter Johns Ankunft): >= 96 Bilder
if(_winken LESS 96 OR NOT _klettern EQUAL 0)
    message(FATAL_ERROR "${_tag}: 'Elza, hurry up!' rendert nicht RBJ rec1 Clip 2 (96 Bilder: ${_winken}) bzw. noch den "
        "Bank-0-Kletter-Clip (50 Bilder: ${_klettern}) (${WORKDIR})")
endif()
# Plc_motion(0,0,0) viermal (40 + 20 + 60 + 30 Bilder Schlaf, @0x0141A/@0x01426/@0x0152A/@0x01536)
if(_c0_rbj LESS 140 OR NOT _c0_bank0 EQUAL 0)
    message(FATAL_ERROR "${_tag}: Plc_motion(0,0,0) rendert nicht RBJ rec1 Clip 0 (20 Bilder: ${_c0_rbj}) bzw. noch "
        "Bank 0 Clip 0 (34 Bilder: ${_c0_bank0}) (${WORKDIR})")
endif()
if(_c6_eigen LESS 150)
    message(FATAL_ERROR "${_tag}: Plc_motion(2,6,0) rendert nicht die eigene Bank Clip 6 (10 Bilder: ${_c6_eigen}) (${WORKDIR})")
endif()
message(STATUS "${_tag}: OK ${WORKDIR}")
