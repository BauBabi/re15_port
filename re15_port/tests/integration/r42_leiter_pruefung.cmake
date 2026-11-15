# =============================================================================
# Runde 42 "story" (v0.8.31), Stufe STAGE1 - gemeinsamer Teil fuer integration_r42_story1_* und Glied 2 von
# integration_r42_story2_kette/_kette_elza: Weg ab der 11Ax-Ankunft aus 207x (Tuer 207x Slot 4 -> (6500,0,-3200)) durch die Szene vor
# der Leiter bis 300x, und die Pruefung des Logs. Belege include/re15_story1_r42.h, Code engine/src/story1_r42.c.
#
#   r42_leiter_plan(<out> <S>)   Wegplan-Text ab "RAUM 11A<S>": unterer Gang -> Kante (Aktion, hinauf) -> Szene -> FLAG 9 120 ->
#                                hinab (Aktion an der Kante, Blick 1024) -> unten angekommen -> wieder hinauf (keine zweite Szene)
#                                -> Abschnitt -> Leiter (Slot 1 @0x00F7C, Aktion) -> "RAUM 300<S>", WARTE 1500
#   r42_leiter_pruefen(<dbg> <wp> <S> <uhr> <out>)   Pruefungen (Aufrufer stellt _fehler bereit); <out> = Kurzbericht
# =============================================================================
function(r42_leiter_plan _out _s)
    set(${_out} "RAUM 11A${_s}
WARTE 30
GEHE 300 -1500 150
BLICK 3072
WARTE 5
GEHE 300 -750 150
BLICK 3072
WARTE 5
AKTION
WARTE 1700
FLAG 9 120
GEHE 300 300 150
BLICK 1024
WARTE 10
AKTION
WARTE 120
GEHE 300 -1500 150
BLICK 3072
WARTE 5
GEHE 300 -750 150
BLICK 3072
WARTE 5
AKTION
WARTE 150
GEHE 300 1600 150
GEHE 300 14000 150
GEHE 0 20000 150
GEHE 0 24200 150
BLICK 3072
WARTE 10
AKTION
RAUM 300${_s}
WARTE 1500
" PARENT_SCOPE)
endfunction()

function(_r42l_muss _text _wort _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        _fehler("[leiter]: ${_was} - '${_wort}' fehlt")
    endif()
endfunction()
function(_r42l_nicht _text _wort _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(NOT _p LESS 0)
        _fehler("[leiter]: ${_was} - '${_wort}' steht im Log")
    endif()
endfunction()
# Folge der Zeilen eines Raums (Ids in der Reihenfolge des Logs), nur Ids >= <von> und <= <bis>
function(_r42l_folge _text _raum _von _bis _out)
    string(REGEX MATCHALL "\\[msgtext\\] room=${_raum} id=[0-9]+" _m "${_text}")
    set(_ids "")
    foreach(_z IN LISTS _m)
        string(REGEX REPLACE ".* id=" "" _i "${_z}")
        if(_i GREATER_EQUAL ${_von} AND _i LESS_EQUAL ${_bis})
            list(APPEND _ids ${_i})
        endif()
    endforeach()
    string(REPLACE ";" "," _ids "${_ids}")
    set(${_out} "${_ids}" PARENT_SCOPE)
endfunction()

function(r42_leiter_pruefen _dbg _wp _s _uhr _out)
    string(TOLOWER "11a${_s}" _k11)
    if(_s EQUAL 0)
        set(_szene "Szene L3 (Marvin)")
        set(_zeilen "3,4,5,6")
    elseif(_uhr)
        set(_szene "Szene E2 (John, Sherry) mit Uhr (6b)")
        set(_zeilen "3,4,5,6,7,8,9,10")
    else()
        set(_szene "Szene E2 (John, Sherry) ohne Uhr")
        set(_zeilen "3,4,5,10")
    endif()
    # ---- 11Ax: Szene vor der Leiter ---------------------------------------------------------------------------------------
    _r42l_muss("${_dbg}" "loaded room${_k11}.rdt" "11A${_s} erreicht")
    _r42l_muss("${_dbg}" "verworfen - " "K-07: Mod-Begleiter in 11A${_s} verworfen")
    _r42l_muss("${_dbg}" "Spieler betritt den Abschnitt bei" "Ausloesung nach dem Hinaufsteigen (Zone SCA @0x0830/@0x0824, RVD @0x02D4)")
    _r42l_muss("${_dbg}" "-> ${_szene}" "Fassung der Szene")
    string(REGEX MATCHALL "Spieler betritt den Abschnitt" _m "${_dbg}")
    list(LENGTH _m _n)
    if(NOT _n EQUAL 1)
        _fehler("[leiter]: Szene ${_n} mal ausgeloest (soll 1: nach dem zweiten Hinaufsteigen keine zweite)")
    endif()
    _r42l_folge("${_dbg}" "${_k11}" 3 10 _ist)
    if(NOT _ist STREQUAL "${_zeilen}")
        _fehler("[leiter]: Zeilen ${_ist} (soll ${_zeilen})")
    endif()
    if(_s EQUAL 1 AND _uhr)
        _r42l_muss("${_dbg}" "Taschenuhr (Item 0x3F" "6b/F-26: Uhr an Sherry uebergeben")
    else()
        _r42l_nicht("${_dbg}" "an Sherry uebergeben" "keine Rueckgabe")
    endif()
    # Abgang (Nachbesserung 1, Pruefer B1/B2): unten an die Kante, Abblende, im Schwarz oben gesetzt (Kamera 2), dort Richtung
    # Leiter, erst in Kamera 3 (hinter dem Pfeiler) entfernt; die Kantensonde trifft nie (re15_story1_r42.h ABGANG)
    _r42l_muss("${_dbg}" "Abblende (Zeitsprung, PORT-WAHL" "Abblende (Zeitsprung)")
    _r42l_muss("${_dbg}" "Abblende schwarz" "Schwarz")
    _r42l_muss("${_dbg}" "verlaesst den unteren Gang (schwarz)" "Begleiter verlassen im Schwarz den unteren Gang")
    _r42l_muss("${_dbg}" "verlaesst den Raum Richtung Leiter (Cut 3, hinter dem Pfeiler)" "Begleiter erst in Kamera 3 entfernt")
    _r42l_muss("${_dbg}" "Einblende fertig" "Einblende")
    _r42l_nicht("${_dbg}" "WARNUNG NPC" "Kanten-Sub eines Begleiters (Sonde FUN_8002d7d8)")
    string(REGEX MATCH "Einblende nach ([0-9]+) Bildern Schwarz, Kamera ([0-9]+)" _ein "${_dbg}")
    if(NOT _ein)
        _fehler("[leiter]: keine Einblende-Zeile")
    endif()
    if(CMAKE_MATCH_1 LESS 30 OR NOT CMAKE_MATCH_2 EQUAL 2)
        _fehler("[leiter]: Schwarz ${CMAKE_MATCH_1} Bilder / Kamera ${CMAKE_MATCH_2} (soll >= 30, Kamera 2)")
    endif()
    set(_schwarz ${CMAKE_MATCH_1})
    string(REGEX MATCHALL "verlaesst den Raum Richtung Leiter" _m "${_dbg}")
    list(LENGTH _m _nweg)
    if(_s EQUAL 0)
        set(_nweg_soll 1)
    else()
        set(_nweg_soll 2)
    endif()
    if(NOT _nweg EQUAL _nweg_soll)
        _fehler("[leiter]: ${_nweg} Begleiter oben entfernt (soll ${_nweg_soll})")
    endif()
    string(REGEX MATCHALL "Ankunft (Begleiter|Sherry) bei \\([-0-9]+,-180[01],[0-9]+\\) Band 1" _oben "${_dbg}")
    list(LENGTH _oben _noben)
    if(_noben LESS _nweg_soll)
        _fehler("[leiter]: ${_noben} Ankuenfte oben im Abschnitt (soll ${_nweg_soll})")
    endif()
    _r42l_muss("${_wp}" "FLAG (9,120) = 1" "Bit 120 = 1")
    string(REGEX MATCHALL "spieler=\\([0-9-]+,-1[45][0-9][0-9]\\)[^\n]*beendet: angekommen" _unten "${_wp}")
    list(LENGTH _unten _nu)
    if(_nu LESS 2)
        _fehler("[leiter]: nach der Szene nicht wieder im unteren Gang (${_nu} Ankuenfte bei z -1500, soll >= 2)")
    endif()
    _r42l_muss("${_dbg}" "[aot] DOOR FIRE slot=1 rect=(0,24950,hw=650,hh=450)" "Leiter Slot 1 @0x00F7C (x -650..650 z 24500..25400)")
    # ---- 300x unveraendert (Stand vor der Stufe, fa2d88eb1, dieselbe Route) ------------------------------------------------
    string(FIND "${_dbg}" "loaded room300${_s}.rdt" _p3)
    if(_p3 LESS 0)
        _fehler("[leiter]: 300${_s} nicht erreicht")
    endif()
    string(SUBSTRING "${_dbg}" ${_p3} -1 _d3)
    _r42l_folge("${_d3}" "300${_s}" 0 99 _ist3)
    if(_s EQUAL 0)
        set(_soll3 "2,3,4,5,6,7,8")
        _r42l_muss("${_d3}" "[spawn-diag] Sce_em_set type=0x40 behavior=0x40 slot=0 pos=(-24458,0,-16261) dir=-248" "3000: Marvin wie vorher")
    else()
        set(_soll3 "2,3,4,5,6,7,8,9")
        _r42l_muss("${_d3}" "[spawn-diag] Sce_em_set type=0x49 behavior=0x40 slot=0 pos=(-23363,0,-14359) dir=640" "3001: John wie vorher")
        _r42l_muss("${_d3}" "[spawn-diag] Sce_em_set type=0x4B behavior=0x40 slot=1 pos=(-23600,0,-15799) dir=352" "3001: Sherry wie vorher")
    endif()
    string(FIND "${_ist3}" "${_soll3}" _pf)
    if(NOT _pf EQUAL 0)
        _fehler("[leiter]: 300${_s} Zeilen ${_ist3} (soll beginnen mit ${_soll3}, Stand vorher)")
    endif()
    set(${_out} "${_szene}, Zeilen ${_ist}, einmal, Schwarz ${_schwarz} Bilder, ${_noben} oben angekommen, Bit 120, 300${_s} unveraendert (${_ist3})" PARENT_SCOPE)
endfunction()
