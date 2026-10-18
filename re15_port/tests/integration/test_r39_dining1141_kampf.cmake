cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 39 SPUR J "dining1141", NACHBESSERUNG 2 (Abnahme 1 Mangel M1, Nutzer-Nachtrag 4 Punkt 3: "John sollte auch ab und an
# mal auf die Gorillas schiessen in ROOM 1141 und dann ihre Aufmerksamkeit erhalten"). Echte exe, echter Weg mit Elza
# (1131 -> 1141 wie integration_r39_dining1141_d), Pistole im Inventar, Gorillas LEBEND (kein RE15_R38F_GORILLAS_STARR); Elza
# geht nach der Szene neben John (Lage L3 der Abnahme 1) und wehrt sich nicht - die Gorillas packen sie, John schiesst.
# Dossier analysis/befunde_runde39/J_dining1141.md Abschnitt 9, Code engine/src/dining_r39.c, Konstanten include/re15_dining_r39.h.
# Geprueft (debug.log, state.txt, RE15_R39J_LOG):
#   (a) Treffer auch, waehrend Elza im Griff ist (state.txt gr=1), auf den Gorilla, der sie NICHT haelt; Fehlschuesse nur
#       "im Angriff" (Greifer/Biss/Sprung) oder "tot", nie wegen des Opferzustands
#   (b) kein Fehlschuss "nicht im Kegel" (John fuehrt die Waffe nach, gun sub5 @0x80033f94)
#   (c) die Mehrheit der Treffer-Salven bringt eine sichtbare Zuwendung ("John zugewandt", Kegel 0x180 @0x80117a60-74)
#   John im Bild: wenigstens ein Schuss bei Kamera Cut 0 (zeigt Johns Ecke, Projektion (122,90..117), Dossier 9.1)
#   Querungs-Wache (Spur E) bleibt wirksam: Elza und Gorillas nie hinter einer Wand
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> [-DAUSWERTEN=<lauf>] -P test_r39_dining1141_kampf.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r39_dining1141_kampf: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_dining1141_kampf: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_lauf "${WORKDIR}/lauf_${_lauf_id}")
if(AUSWERTEN)
    set(_lauf "${AUSWERTEN}")
endif()

# Eigene exe-Kopie mit Praefix JE TEST (re15_pc_r39j_k_), nur die eigenen alten Kopien weg.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39j_k_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39j_k_${_lauf_id}${_exe_ext}")
if(NOT AUSWERTEN)
    file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
endif()

set(_fehler "")
macro(pruef_wahr _bed _text)
    if(${_bed})
        message(STATUS "ok   ${_text}")
    else()
        list(APPEND _fehler "${_text}")
        message(STATUS "FAIL ${_text}")
    endif()
endmacro()

# Wegplan = integration_r39_dining1141_d bis zum Kampf, dann neben John (Lage L3 der Abnahme 1) und stehen bleiben.
set(_plan "RAUM 1131
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
BLICK 0
AKTION
RAUM 1141
WARTE 420
GEHE -7600 -12000 150
GEHE -8150 -10250 80
BLICK 2048
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 200
GEHE -7350 -8500 150
GEHE -7350 -7600 150
GEHE -7250 -1000 80
BLICK 3072
AKTION
WARTE 40
AKTION
WARTE 40
AKTION
WARTE 60
GEHE -5800 -20500 150
WARTE 2400
ENDE
")

if(NOT AUSWERTEN)
    file(MAKE_DIRECTORY "${_lauf}")
    file(WRITE "${_lauf}/wegplan.txt" "${_plan}")
    set(WORKDIR "${_lauf}")
    re15_start_spiel(_rv 3000
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
        RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_STATE_LOG=state.txt RE15_R39J_LOG=r39.txt
        "RE15_WEGPLAN=${_lauf}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt
        "RE15_EXIT_AT=5500#1141"
        "RE15_SET_FLAG=3:0x5F,4:0xF0,4:0xF1,4:0xEC,3:0x78" "RE15_GIVE=0x43:1,0x03:15,0x15:50" "RE15_DEBUG_JUMP=1131@gp"
        "RE15_PLAYER_POS=-1300,-13900,0" "RE15_R38F_GORILLAS_TOT=2400"
        "${_exe_kopie}")
    file(REMOVE "${_exe_kopie}")
    message(STATUS "r39_dining1141_kampf: exit=${_rv}, ${_lauf}")
endif()

if(NOT EXISTS "${_lauf}/debug.log" OR NOT EXISTS "${_lauf}/state.txt" OR NOT EXISTS "${_lauf}/r39.txt")
    message(FATAL_ERROR "r39_dining1141_kampf: debug.log/state.txt/r39.txt fehlt (${_lauf})")
endif()

# Schuesse aus debug.log: "[dining39] F<n> John schiesst (Schuss <k>) auf Gorilla Slot <s>: Treffer... | daneben - <Grund>"
file(STRINGS "${_lauf}/debug.log" _schuss_zeilen REGEX "John schiesst \\(Schuss")
file(STRINGS "${_lauf}/debug.log" _zu_zeilen REGEX "\\] F[0-9]+ Gorilla Slot [0-9]+ John zugewandt")
list(LENGTH _schuss_zeilen _n_schuss)
set(_n_treffer 0)
set(_n_kegel 0)
set(_n_sonst 0)
set(_treffer_f "")
foreach(_z IN LISTS _schuss_zeilen)
    if(_z MATCHES "\\] F([0-9]+) John schiesst \\(Schuss [0-9]+\\) auf Gorilla Slot (-?[0-9]+): (.*)$")
        set(_f ${CMAKE_MATCH_1})
        set(_e "${CMAKE_MATCH_3}")
        if(_e MATCHES "^Treffer")
            math(EXPR _n_treffer "${_n_treffer} + 1")
            list(APPEND _treffer_f ${_f})
        elseif(_e MATCHES "nicht im Kegel")
            math(EXPR _n_kegel "${_n_kegel} + 1")
        elseif(NOT _e MATCHES "im Angriff" AND NOT _e MATCHES "tot")
            math(EXPR _n_sonst "${_n_sonst} + 1")
        endif()
    endif()
endforeach()
message(STATUS "     ${_n_schuss} Schuesse, ${_n_treffer} Treffer, ${_n_kegel} ausser Kegel, ${_n_sonst} sonstige Fehlschuesse")
set(_b_viel FALSE)
if(_n_schuss GREATER 7)
    set(_b_viel TRUE)
endif()
pruef_wahr(_b_viel "N4.3: John schiesst ab und an (${_n_schuss} Schuesse in rund 1700 Kampfbildern, zwei je Salve)")

# (a) Treffer mit Elza im Griff: state.txt-Zeile des Schussbilds traegt gr=1
set(_n_griff 0)
foreach(_f IN LISTS _treffer_f)
    file(STRINGS "${_lauf}/state.txt" _sz REGEX "^F${_f} ")
    if(_sz MATCHES " gr=1")
        math(EXPR _n_griff "${_n_griff} + 1")
    endif()
endforeach()
set(_b_a FALSE)
if(_n_griff GREATER 0 AND _n_sonst EQUAL 0)
    set(_b_a TRUE)
endif()
pruef_wahr(_b_a "NB2 (a): ${_n_griff} Treffer, waehrend Elza im Griff ist (gr=1); kein Fehlschuss wegen des Opferzustands (${_n_sonst})")
# (b) John fuehrt die Waffe nach: kein Fehlschuss ausserhalb des Kegels 0xC8
set(_b_b FALSE)
if(_n_kegel EQUAL 0 AND _n_treffer GREATER 3)
    set(_b_b TRUE)
endif()
pruef_wahr(_b_b "NB2 (b): kein Schuss ausserhalb des Kegels (${_n_kegel}), ${_n_treffer} Treffer")
# (c) Zuwendung: je Treffer-Salve (Treffer innerhalb von 40 Bildern = eine Salve) eine Zeile "John zugewandt" bis 180 Bilder danach
set(_salven "")
set(_letzte -1000)
foreach(_f IN LISTS _treffer_f)
    math(EXPR _d "${_f} - ${_letzte}")
    if(_d GREATER 40)
        list(APPEND _salven ${_f})
    endif()
    set(_letzte ${_f})
endforeach()
list(LENGTH _salven _n_salven)
set(_n_zu 0)
foreach(_s IN LISTS _salven)
    math(EXPR _bis "${_s} + 180")
    set(_hat FALSE)
    foreach(_z IN LISTS _zu_zeilen)
        if(_z MATCHES "\\] F([0-9]+) Gorilla Slot")
            if(CMAKE_MATCH_1 GREATER_EQUAL _s AND CMAKE_MATCH_1 LESS_EQUAL _bis)
                set(_hat TRUE)
            endif()
        endif()
    endforeach()
    if(_hat)
        math(EXPR _n_zu "${_n_zu} + 1")
    endif()
endforeach()
math(EXPR _haelfte "${_n_salven} / 2")
set(_b_c FALSE)
if(_n_salven GREATER 2 AND _n_zu GREATER _haelfte)
    set(_b_c TRUE)
endif()
pruef_wahr(_b_c "NB2 (c): ${_n_zu} von ${_n_salven} Treffer-Salven mit sichtbarer Zuwendung (Gorilla dreht sich John zu)")

# John im Bild + Querungs-Wache aus RE15_R39J_LOG
file(STRINGS "${_lauf}/r39.txt" _zeilen)
set(_cut0_schuss 0)
set(_wand 0)
set(_wand_f "")
foreach(_z IN LISTS _zeilen)
    if(NOT _z MATCHES "^F([0-9]+) cam=[0-9]+ anz=([0-9]+) ")
        continue()
    endif()
    set(_f ${CMAKE_MATCH_1})
    set(_anz ${CMAKE_MATCH_2})
    list(FIND _treffer_f ${_f} _ti)
    if(_ti GREATER -1 AND _anz EQUAL 0)
        math(EXPR _cut0_schuss "${_cut0_schuss} + 1")
    endif()
    # Raumgrenzen (SCA ROOM1141: Westwand x -10600..-8600, Nordwand z -250.., Suedwand z -25600..-23600, Ostrand x 14700..):
    # Elza nie jenseits; ein Gorilla (Radius 1618) nie naeher als -7100 an der Westwand / -22100 an der Suedwand
    if(_z MATCHES " elza=\\((-?[0-9]+),(-?[0-9]+)\\) ")
        if(CMAKE_MATCH_1 LESS -8600 OR CMAKE_MATCH_2 LESS -23600)
            math(EXPR _wand "${_wand} + 1")
            set(_wand_f "F${_f} Elza")
        endif()
    endif()
    if(_z MATCHES " g=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+),-?[0-9]+\\)/\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+),-?[0-9]+\\) ")
        foreach(_gx ${CMAKE_MATCH_1} ${CMAKE_MATCH_4})
            if(_gx LESS -7100 AND NOT _gx EQUAL 0)
                math(EXPR _wand "${_wand} + 1")
                set(_wand_f "F${_f} Gorilla x ${_gx}")
            endif()
        endforeach()
        foreach(_gz ${CMAKE_MATCH_2} ${CMAKE_MATCH_5})
            if(_gz LESS -22100 AND NOT _gz EQUAL 0)
                math(EXPR _wand "${_wand} + 1")
                set(_wand_f "F${_f} Gorilla z ${_gz}")
            endif()
        endforeach()
    endif()
endforeach()
set(_b_bild FALSE)
if(_cut0_schuss GREATER 0)
    set(_b_bild TRUE)
endif()
pruef_wahr(_b_bild "NB2: John beim Schiessen im Bild - ${_cut0_schuss} Treffer-Schuesse bei Kamera Cut 0 (zeigt Johns Ecke)")
set(_b_wand FALSE)
if(_wand EQUAL 0)
    set(_b_wand TRUE)
endif()
pruef_wahr(_b_wand "Querungs-Wache (Spur E): niemand hinter einer Wand (${_wand} Bilder ${_wand_f})")

list(LENGTH _fehler _nf)
if(_nf GREATER 0)
    message(FATAL_ERROR "r39_dining1141_kampf: ${_nf} Fehler: ${_fehler} (${_lauf})")
endif()
message(STATUS "r39_dining1141_kampf: alle Pruefungen gruen (${_lauf})")
