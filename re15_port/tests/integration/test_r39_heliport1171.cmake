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

# Zaehlen im Szenen-Abschnitt. Spalten (anim_select_common.c): Bild Aktor Typ motion Clip fc cur slot Uhr loco rueck alt.
# CMake kennt nur 9 Gruppen: gefangen werden Bild(1) Typ(2) motion(3) Clip(4) fc(5) slot(6) rueck(7).
set(_abschn 0)
set(_vor -1)
set(_winken 0)
set(_klettern 0)
set(_c0_rbj 0)
set(_c0_bank0 0)
set(_c6_eigen 0)
set(_nach_plc 0)          # 1 ab dem ersten Plc_motion (RBJ Clip 0, 20 Bilder) = Ende der INIT-Pose
set(_init_loco 0)         # K2: Loco Clip 2 (50) Bild 0 vor dem ersten Plc_motion
set(_init_eigen 0)        # K2: eigene Clip 2 (52) vor dem ersten Plc_motion (alter Fehler)
set(_lauf_key "")
set(_saat_n 0)            # K3: Laeufe der Plc_motion-Clips (Clip 0/20, 6/10, 2/96)
set(_saat_falsch "")
set(_letzt18 0)           # K4: vorige Zeile war Clip 18 (25)
set(_rueck17_vor18 -1)    # K4: Clip 17 rueckwaerts beginnt direkt nach Clip 18
set(_rueck17_n 0)
set(_rueck17_slots "")
set(_rueck17_zu 0)
foreach(_z IN LISTS _zz)
    if(NOT _z MATCHES "^([0-9]+) [0-9]+ ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) [0-9]+ ([0-9]+) [0-9]+ [0-9]+ ([0-9]+)")
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
    set(_sl ${CMAKE_MATCH_6})
    set(_rw ${CMAKE_MATCH_7})
    if(_mo EQUAL 0 AND _cl EQUAL 0 AND _fc EQUAL 20)
        set(_nach_plc 1)
    endif()
    if(NOT _nach_plc)
        if(_mo EQUAL 2 AND _cl EQUAL 2 AND _fc EQUAL 50 AND _sl EQUAL 0)
            math(EXPR _init_loco "${_init_loco} + 1")
        endif()
        if(_mo EQUAL 2 AND _cl EQUAL 2 AND _fc EQUAL 52)
            math(EXPR _init_eigen "${_init_eigen} + 1")
        endif()
    endif()
    if(_mo EQUAL 2 AND _cl EQUAL 2 AND _fc EQUAL 96)
        math(EXPR _winken "${_winken} + 1")
    endif()
    if(_nach_plc AND _mo EQUAL 2 AND _cl EQUAL 2 AND _fc EQUAL 50)
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
    # K3: erstes Bild jedes Laufs der Plc_motion-Clips = Saattakt -> Bild 0 bzw. fc-1 (Phase 0 @0x80050cec-d0c faellt
    # in f314 @0x80050d4c; f314 posiert +0x95 @0x8001f35c-36c, Vorschub erst @0x8001f618-1c).
    set(_key "${_mo}/${_cl}/${_fc}/${_rw}")
    if(NOT _key STREQUAL _lauf_key)
        if((_mo EQUAL 0 AND _cl EQUAL 0 AND _fc EQUAL 20) OR (_mo EQUAL 6 AND _cl EQUAL 6 AND _fc EQUAL 10) OR
           (_mo EQUAL 2 AND _cl EQUAL 2 AND _fc EQUAL 96))
            math(EXPR _saat_n "${_saat_n} + 1")
            if(_rw EQUAL 1)
                math(EXPR _soll "${_fc} - 1")
            else()
                set(_soll 0)
            endif()
            if(NOT _sl EQUAL _soll)
                list(APPEND _saat_falsch "F${_f}:${_key}:Bild${_sl}")
            endif()
        endif()
        # K4: Beginn von Clip 17 rueckwaerts (Schub Phase 6)
        if(_mo EQUAL 17 AND _cl EQUAL 17 AND _fc EQUAL 10 AND _rw EQUAL 1 AND _rueck17_vor18 LESS 0)
            set(_rueck17_vor18 ${_letzt18})
            set(_rueck17_zu 1)
        elseif(_rueck17_zu)
            set(_rueck17_zu 0)
        endif()
    endif()
    set(_lauf_key "${_key}")
    if(_rueck17_zu)
        math(EXPR _rueck17_n "${_rueck17_n} + 1")
        list(APPEND _rueck17_slots ${_sl})
    endif()
    if(_mo EQUAL 18 AND _cl EQUAL 18 AND _fc EQUAL 25)
        set(_letzt18 1)
    else()
        set(_letzt18 0)
    endif()
endforeach()
string(REPLACE ";" "," _rueck17_txt "${_rueck17_slots}")
message(STATUS "${_tag}: Szenen-Abschnitt ${_szene}: INIT-Pose Loco 2/50 Bild 0 ${_init_loco}, eigene 2/52 vorher "
               "${_init_eigen}; Heran-Winken (Clip 2/96) ${_winken} Bilder, Kletter-Clip (2/50 nach Plc_motion) "
               "${_klettern}, RBJ-Clip 0 (20) ${_c0_rbj}, Bank-0-Clip 0 (34) ${_c0_bank0}, eigener Clip 6 (10) ${_c6_eigen}; "
               "Saattakte ${_saat_n} (falsch: ${_saat_falsch}); Schub Clip 17 rueckw. nach Clip 18: ${_rueck17_vor18}, "
               "${_rueck17_n} Bilder [${_rueck17_txt}]")
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
# K2: INIT-Pose = Loco-Paar Clip 2 Bild 0 (+0x94 = 2 @0x8011de7c, +0x95 = 0 @0x8011de8c, f314(+0x84,+0x16c)
# @0x8011debc-c4) bis zum ersten Plc_motion @0x01416 (Laden: F0-F220).
if(_init_loco LESS 150 OR NOT _init_eigen EQUAL 0)
    message(FATAL_ERROR "${_tag}: INIT-Pose nicht Loco Clip 2 Bild 0 (${_init_loco} Bilder) bzw. noch eigene Bank Clip 2 "
        "(${_init_eigen} Bilder) vor dem ersten Plc_motion (${WORKDIR})")
endif()
# K3: alle sieben Plc_motion-Laeufe beginnen mit dem Saat-Bild
if(_saat_n LESS 7 OR _saat_falsch)
    message(FATAL_ERROR "${_tag}: Plc_motion-Saattakt nicht Bild 0 / fc-1 (${_saat_n} Laeufe, falsch: ${_saat_falsch}) (${WORKDIR})")
endif()
# K4: Clip 17 rueckwaerts folgt direkt auf Clip 18 und laeuft 9..0 (10 Bilder)
if(NOT _rueck17_vor18 EQUAL 1 OR NOT _rueck17_txt STREQUAL "9,8,7,6,5,4,3,2,1,0")
    message(FATAL_ERROR "${_tag}: Schub Phase 5 -> 6: Clip 17 rueckwaerts nicht direkt nach Clip 18 bzw. nicht 9..0 "
        "(${_rueck17_vor18}, [${_rueck17_txt}]) (${WORKDIR})")
endif()
message(STATUS "${_tag}: OK ${WORKDIR}")
