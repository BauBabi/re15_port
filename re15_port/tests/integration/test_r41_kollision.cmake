cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 41 SPUR H "kollision" MIT DER ECHTEN EXE. Dossier analysis/befunde_runde41/H_kollision.md.
# Nutzer: "Die NPCs z.B. Ada und Marvin koennen ineinander rein laufen, das ist falsch, die sind beide fest."
#         "In ROOM 2050 lassen sich die Kisten in einander schieben, das ist falsch"
#
# Teile (-DTEIL=...):
#   kiste         ROOM2050 (Leon), Wegplan RE15_WEGPLAN (dachkiste_r36.c): obj 2 (-1256,0,-64) nach Osten gegen obj 3
#                 (4248,0,1400). Muss: SCHUB-GESPERRT an obj 2 (push.txt) mit OBJEKT-GESPERRT "gegen ... obj=3"
#                 (kollision.txt = Durchlauf @0x8002c03c-c0a8, FUN_8002cdf4 @0x8002c078); kein angenommenes Ziel x >= 2448
#                 (|dX| <= 1800 = hx+hx); obj 3 bleibt auf dem Record. Vor Runde 41 gemessen: obj 2 bis x 4506 IN obj 3.
#   raetsel_leon  ROOM2050: obj 2 -> Kerbe slot 5, obj 3 -> Kerbe slot 4, obj 1 -> Kerbe slot 3 (Aot slot 3/4/5 @0x01CAA/
#                 @0x01CBE/@0x01CD2) -> Flags (5,0)/(5,1)/(5,2) = 1 (sub01 @0x0203C/@0x02096/@0x020F0...) — das Raetsel
#                 bleibt mit der Sperre loesbar; Endlagen paarweise |dZ| > 1800.
#   raetsel_elza  dasselbe in ROOM2051 mit Elza (RE15_PSELECT_AUTO_SWITCH=1).
#   npc3070       ROOM3070 Leon, (4,215)/(4,236)/(3,180): Ada (0x42) + Marvin (0x40) als Eskorte (sub00 @0x0343C/
#                 @0x03450) — Leon steht 25 s, geht, steht, geht; Abstand der beiden je Bild (RE15_STATE_LOG ->
#                 probe_r41_kollision_abstand): Kontakt (< 1000) ja, kein Bild < 850, min >= 880, Ende >= 895.
#                 Original (H_belege/orig_3070_eskorte, DuckStation-GDB): min. 901 ueber 187 Proben; Port vorher: 3.
#   npc3001       ROOM3001 Elza, (4,198)/(4,199): John (0x49) + Sherry (0x4b) als Eskorte (sub00 @0x015A2/@0x015B6),
#                 Radien-Summe 750: Kontakt (< 850) ja, kein Bild < 700, min >= 730, Ende >= 745. Port vorher: 4.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_ABSTAND_TOOL=<probe_r41_kollision_abstand> -DWORKDIR=<dir> -DTEIL=<teil>
#         -P test_r41_kollision.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_kollision: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_kollision: WORKDIR fehlt")
endif()
if(NOT TEIL)
    message(FATAL_ERROR "r41_kollision: TEIL fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_basis}")

# EIGENER EXE-NAME neben der exe (Asset-Wurzel = exe-Verzeichnis), Praefix NUR dieses Teils.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41h_${TEIL}_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r41h_${TEIL}_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

# keine Umgebung des Aufrufers erben
foreach(_v RE15_AI_FLAVOR RE15_FRAMEDUMP RE15_INPUT_SCRIPT RE15_INPUT_SCRIPT_START RE15_INPUT_SCRIPT_BASIS
           RE15_PLAYER_POS RE15_DEBUG_JUMP RE15_SET_FLAG RE15_SET_FLAG_AT RE15_NOAUDIO RE15_PSELECT_AUTO
           RE15_PSELECT_AUTO_SWITCH RE15_EXIT_AT RE15_WEGPLAN RE15_WEGPLAN_LOG RE15_PUSH_LOG RE15_OBJFALL_LOG
           RE15_KOLLISION_LOG RE15_KOLLISION_SETZE RE15_STATE_LOG RE15_CONTINUE_TEST RE15_CARD_AUTO)
    unset(ENV{${_v}})
endforeach()

function(_fehler _was)
    file(REMOVE "${_exe}")
    message(FATAL_ERROR "r41_kollision[${TEIL}]: ${_was}")
endfunction()

set(_grund RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
           RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1)

# Ein Lauf im Arbeitsordner; ein Lauf ohne debug.log bekommt EINEN Wiederholungsversuch (Fenster-Haken unter Last).
macro(_lauf _timeout)
    set(WORKDIR "${_basis}")
    re15_start_spiel(_rv ${_timeout} ${ARGN} "${_exe}")
    if("${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1" OR NOT EXISTS "${_basis}/debug.log")
        message(STATUS "r41_kollision[${TEIL}]: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
        file(REMOVE "${_basis}/debug.log" "${_basis}/push.txt" "${_basis}/kollision.txt" "${_basis}/state.txt"
                    "${_basis}/wegplan_log.txt" "${_basis}/objfall.txt")
        re15_start_spiel(_rv ${_timeout} ${ARGN} "${_exe}")
    endif()
    if(NOT EXISTS "${_basis}/debug.log")
        _fehler("kein debug.log (exit=${_rv}, ${_basis})")
    endif()
    file(READ "${_basis}/debug.log" _dbg)
endmacro()

macro(_lese _datei _var)
    set(${_var} "")
    if(EXISTS "${_basis}/${_datei}")
        file(READ "${_basis}/${_datei}" ${_var})
    endif()
endmacro()

macro(_muss _text _var _was)
    string(FIND "${${_var}}" "${_text}" _pos)
    if(_pos LESS 0)
        _fehler("${_was} — fehlt: '${_text}'")
    endif()
endmacro()

# Abstandsauswertung eines RE15_STATE_LOG: setzt _min/_kontakt/_unter/_ende/_paar.
macro(_abstand _ab _summe)
    if(NOT RE15_ABSTAND_TOOL OR NOT EXISTS "${RE15_ABSTAND_TOOL}")
        _fehler("RE15_ABSTAND_TOOL fehlt: '${RE15_ABSTAND_TOOL}'")
    endif()
    execute_process(COMMAND "${RE15_ABSTAND_TOOL}" "${_basis}/state.txt" ${_ab} ${_summe}
                    RESULT_VARIABLE _rva OUTPUT_VARIABLE _aus TIMEOUT 120)
    if(NOT _rva EQUAL 0)
        _fehler("Auswerter exit=${_rva}: ${_aus}")
    endif()
    string(REGEX MATCH "PAARBILDER ([0-9]+)" _m "${_aus}")
    set(_paar "${CMAKE_MATCH_1}")
    string(REGEX MATCH "MIN (-?[0-9]+) F(-?[0-9]+)" _m "${_aus}")
    set(_min "${CMAKE_MATCH_1}")
    set(_min_f "${CMAKE_MATCH_2}")
    string(REGEX MATCH "KONTAKT ([0-9]+)" _m "${_aus}")
    set(_kontakt "${CMAKE_MATCH_1}")
    string(REGEX MATCH "UNTER ([0-9]+)" _m "${_aus}")
    set(_unter "${CMAKE_MATCH_1}")
    string(REGEX MATCH "ENDE (-?[0-9]+)" _m "${_aus}")
    set(_ende "${CMAKE_MATCH_1}")
    if("${_paar}" STREQUAL "" OR "${_min}" STREQUAL "" OR "${_ende}" STREQUAL "")
        _fehler("Auswerter-Ausgabe unvollstaendig: ${_aus}")
    endif()
endmacro()

if(TEIL STREQUAL "kiste")
    file(WRITE "${_basis}/wegplan.txt" "OBJ 2
RAUM 2050
GEGNERWEG
WARTE 30
SCHIEB 0 X >= 4500 1200
WARTE 15
SCHIEB 0 X >= 4500 200
WARTE 15
SCHIEB 0 X >= 4500 200
WARTE 10
ENDE
")
    _lauf(400 ${_grund} RE15_PUSH_LOG=push.txt RE15_OBJFALL_LOG=objfall.txt RE15_KOLLISION_LOG=kollision.txt
          "RE15_WEGPLAN=${_basis}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt RE15_DEBUG_JUMP=2050@250
          "RE15_PLAYER_POS=-2700,-64,0,0" RE15_EXIT_AT=1100)
    _muss("loaded room2050.rdt" _dbg "ROOM2050 nicht geladen")
    _lese(push.txt _push)
    _lese(kollision.txt _kol)
    _lese(objfall.txt _fall)
    string(REGEX MATCHALL "room=2050 p=[0-9]+ obj=2 SCHUB-GESPERRT[^\n]*" _ges "${_push}")
    list(LENGTH _ges _n_ges)
    if(_n_ges LESS 2)
        _fehler("nur ${_n_ges} gesperrte Schuebe an obj 2 (erwartet >= 2, je SCHIEB-Runde einer)")
    endif()
    string(REGEX MATCHALL "OBJEKT-GESPERRT p=[0-9]+ obj=2 ziel=[(][-0-9]+,[-0-9]+[)] gegen p=[0-9]+ obj=3 lage=[(]4248,0,1400[)][^\n]*" _obj "${_kol}")
    list(LENGTH _obj _n_obj)
    if(_n_obj LESS 2)
        _fehler("keine Objekt-Sperre obj 2 gegen obj 3 in kollision.txt (FUN_8002cdf4 @0x8002c078): '${_kol}'")
    endif()
    string(REGEX MATCHALL "obj=2 SCHUB-OK[^\n]*ziel=[(][-0-9]+," _oks "${_push}")
    set(_xmax -99999)
    foreach(_z IN LISTS _oks)
        string(REGEX MATCH "ziel=[(]([-0-9]+)," _m "${_z}")
        if(CMAKE_MATCH_1 GREATER _xmax)
            set(_xmax ${CMAKE_MATCH_1})
        endif()
    endforeach()
    list(LENGTH _oks _n_ok)
    if(_n_ok LESS 100)
        _fehler("nur ${_n_ok} angenommene Schuebe — der Weg zur zweiten Kiste wurde nicht gefahren")
    endif()
    if(NOT _xmax LESS 2448)
        _fehler("angenommenes Ziel x ${_xmax} >= 2448: obj 2 steckt in obj 3 (|dX| <= 1800)")
    endif()
    string(REGEX MATCHALL "room=2050 spieler=[^\n]*" _lagen "${_fall}")
    list(LENGTH _lagen _n_lagen)
    if(_n_lagen LESS 1)
        _fehler("keine Lagezeile in objfall.txt")
    endif()
    list(GET _lagen -1 _letzte)
    string(FIND "${_letzte}" "(4248,0,1400) b0" _p3)
    if(_p3 LESS 0)
        _fehler("obj 3 nicht mehr auf dem Record (4248,0,1400): '${_letzte}'")
    endif()
    message(STATUS "r41_kollision[kiste]: ${_n_ok} angenommene Schuebe (max. Ziel x ${_xmax} < 2448), ${_n_ges} gesperrt, "
                   "${_n_obj} Objekt-Sperren gegen obj 3; Ende '${_letzte}'")
elseif(TEIL STREQUAL "raetsel_leon" OR TEIL STREQUAL "raetsel_elza")
    if(TEIL STREQUAL "raetsel_leon")
        set(_raum 2050)
        set(_wahl "")
    else()
        set(_raum 2051)
        set(_wahl RE15_PSELECT_AUTO_SWITCH=1)
    endif()
    # Weg (H_belege/raetsel_leon, an der exe gemessen): obj 2 nach Westen in Kerbe 5 (z -64); obj 3 nach Norden bis
    # z <= -1880, dann nach Westen in Kerbe 4; obj 1 nach Sueden bis z >= -3740 (Fenster (-3750,-3687): r0[0] @0x00910
    # sperrt die Kiste bei z < -3750, obj 3 bei |dZ| <= 1800), dann nach Westen in Kerbe 3.
    file(WRITE "${_basis}/wegplan.txt" "OBJ 2
RAUM ${_raum}
GEGNERWEG
WARTE 30
SCHIEB 2048 X <= -6600 1500
WARTE 20
GEHE 3000 -64
GEHE 5700 -64
GEHE 5700 2800
GEHE 4248 2800 100
OBJ 3
SCHIEB 1024 Z <= -1880 1500
WARTE 20
GEHE 5648 -530
GEHE 5648 -1880 100
SCHIEB 2048 X <= -6600 1800
WARTE 20
GEHE -4000 -3000
GEHE 3500 -3000
GEHE 3500 -6952
GEHE 2052 -6952 100
OBJ 1
SCHIEB 3072 Z >= -3740 1200
WARTE 20
GEHE 2052 -5100
GEHE 3600 -5100
GEHE 3450 -3700 100
SCHIEB 2048 X <= -6600 1800
WARTE 30
FLAG 5 0
FLAG 5 1
FLAG 5 2
WARTE 30
ENDE
")
    _lauf(800 ${_grund} ${_wahl} RE15_PUSH_LOG=push.txt RE15_OBJFALL_LOG=objfall.txt RE15_KOLLISION_LOG=kollision.txt
          "RE15_WEGPLAN=${_basis}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt RE15_DEBUG_JUMP=${_raum}@250
          "RE15_PLAYER_POS=144,-64,2048,0" RE15_EXIT_AT=3000)
    _muss("loaded room${_raum}.rdt" _dbg "ROOM${_raum} nicht geladen")
    if(TEIL STREQUAL "raetsel_elza")
        _muss("Spieler-Familie PL04" _dbg "Elza nicht gewaehlt (RE15_PSELECT_AUTO_SWITCH)")
    endif()
    _lese(wegplan_log.txt _wp)
    _lese(objfall.txt _fall)
    foreach(_b 0 1 2)
        _muss("FLAG (5,${_b}) = 1" _wp "Kerbe nicht belegt: Flag (5,${_b}) nicht gesetzt (sub01 Member_cmp 15)")
    endforeach()
    string(REGEX MATCHALL "room=${_raum} spieler=[^\n]*" _lagen "${_fall}")
    list(LENGTH _lagen _n_lagen)
    if(_n_lagen LESS 1)
        _fehler("keine Lagezeile in objfall.txt")
    endif()
    list(GET _lagen -1 _letzte)
    # Endlagen: p2 = obj 1, p3 = obj 2, p4 = obj 3 (Installationsreihenfolge sub00), Band 0, x in der Kerbenspalte.
    set(_zs "")
    foreach(_p 2 3 4)
        string(REGEX MATCH "p${_p} [(](-?[0-9]+),0,(-?[0-9]+)[)] b0" _m "${_letzte}")
        if("${_m}" STREQUAL "")
            _fehler("Kiste p${_p} nicht auf Band 0 gefunden: '${_letzte}'")
        endif()
        if(CMAKE_MATCH_1 GREATER -5550)
            _fehler("Kiste p${_p} nicht in der Kerbenspalte (x ${CMAKE_MATCH_1} > -5550): '${_letzte}'")
        endif()
        list(APPEND _zs ${CMAKE_MATCH_2})
    endforeach()
    list(GET _zs 0 _z1)
    list(GET _zs 1 _z2)
    list(GET _zs 2 _z3)
    math(EXPR _d13 "(${_z3}) - (${_z1})")
    math(EXPR _d32 "(${_z2}) - (${_z3})")
    if(NOT _d13 GREATER 1800 OR NOT _d32 GREATER 1800)
        _fehler("Kisten zu dicht (dZ obj1/obj3 ${_d13}, obj3/obj2 ${_d32}, beide muessen > 1800 sein): '${_letzte}'")
    endif()
    message(STATUS "r41_kollision[${TEIL}]: Raetsel geloest, (5,0..2) = 1, Endlage '${_letzte}' (dZ ${_d13} / ${_d32})")
elseif(TEIL STREQUAL "npc3070" OR TEIL STREQUAL "npc3001")
    if(TEIL STREQUAL "npc3070")
        set(_raum 3070)
        set(_wahl "")
        set(_flags "RE15_SET_FLAG=4:215,4:236,3:180")
        set(_summe 900)
        set(_min_soll 880)
        set(_ende_soll 895)
    else()
        set(_raum 3001)
        set(_wahl RE15_PSELECT_AUTO_SWITCH=1)
        set(_flags "RE15_SET_FLAG=4:198,4:199")
        set(_summe 750)
        set(_min_soll 730)
        set(_ende_soll 745)
    endif()
    _lauf(600 ${_grund} ${_wahl} ${_flags} RE15_DEBUG_JUMP=${_raum}@250 RE15_STATE_LOG=state.txt
          RE15_KOLLISION_LOG=kollision.txt RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1
          "RE15_INPUT_SCRIPT=W25,U1.5,W12,U1.5,W12" "RE15_EXIT_AT=1700#${_raum}")
    _muss("loaded room${_raum}.rdt" _dbg "ROOM${_raum} nicht geladen")
    if(TEIL STREQUAL "npc3001")
        _muss("Spieler-Familie PL04" _dbg "Elza nicht gewaehlt (RE15_PSELECT_AUTO_SWITCH)")
    endif()
    _abstand(30 ${_summe})
    if(_paar LESS 1500)
        _fehler("nur ${_paar} Bilder mit beiden NPC (Eskorte nicht gespawnt?)")
    endif()
    if(_kontakt LESS 1)
        _fehler("die beiden NPC kommen sich nie nahe (Kontakt 0) — Messung waere vakuant")
    endif()
    if(_unter GREATER 0)
        _fehler("${_unter} Bilder mit Abstand < Summe - 50 (ineinander gelaufen), min ${_min} bei F${_min_f}")
    endif()
    if(_min LESS ${_min_soll})
        _fehler("min. Abstand ${_min} (F${_min_f}) < ${_min_soll}")
    endif()
    if(_ende LESS ${_ende_soll})
        _fehler("Abstand am Ende ${_ende} < ${_ende_soll}")
    endif()
    _lese(kollision.txt _kol)
    string(FIND "${_kol}" "NPC-SCHUB" _ps)
    if(_ps LESS 0)
        _fehler("kein NPC-SCHUB in kollision.txt (Wurzel-Schwanz @0x8011cc3c/@0x8011cc44 nicht gelaufen)")
    endif()
    message(STATUS "r41_kollision[${TEIL}]: ${_paar} Paarbilder, min ${_min} (F${_min_f}), Kontakt ${_kontakt}, "
                   "unter Summe-50: ${_unter}, Ende ${_ende} (Radien-Summe ${_summe})")
else()
    _fehler("unbekannter TEIL")
endif()
file(REMOVE "${_exe}")
