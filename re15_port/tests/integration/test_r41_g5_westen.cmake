# =============================================================================
# G5 UND ARME SCHIEBEN DEN SPIELER NICHT IN DIE WAND. Runde 41 Spur R, Abnahme 3 (analysis/befunde_runde41/R_abnahme_3.md N1, N2).
#
# Vorher: Leon im Westen des Wagens, ohne Feuer. Der Koerper drueckte ihn gegen die Diagonale #30 (ROOM5090), der Handler LAB_8003cb9c lief bei
# 740 Einheiten Tiefe ueber (`mult v1,a2` @0x8003cf88) und warf ihn hinein, von dort in Box 29 und Box 22 (z -27668). Dort wirkte keine Eingabe
# mehr, der G5 traf ihn nicht und die Pistole nicht den G5: Softlock (Laeufe z_w4, p_falle). Spieler und Arm: Box 29, 388 Bilder (z_fern).
#
# Original: RE2-Klemme FUN_8003567c, Bytes am Kopf von re15_collision_constrain_re2. Kein Messhaken: Rakete bis zum G5, dann die Eingaben der Abnahme.
# Geprueft aus state.log, an JEDEM Bildende:
#   1  Der Spieler-Mittelpunkt liegt in keiner der Zellen #22, #23, #28, #29, #30
#   2  Zum Schluss wirkt die Eingabe: ab dem Ende der G5-Szene (state.log pm=2 -> pm!=2 nach F1500, g5auftritt_r41.c @0x01983/@0x01987)
#      bewegt sich der Spieler um mindestens 300 Einheiten.
# Abnahme G5 1 M11: die G5-Szene sperrt den Spieler seit Runde 41 Vorlauf + 1162 Bilder (RE2 room7040 sub06); "ab F1600" lag mitten in der
# Szene und mass die Versetzung 6500 -> 13873 statt der Eingabe. Die Tasten kommen darum 41 s (1230 Bilder) spaeter, gemessen wird ab dem
# Szenenende.
#
# INTEGRATION RUNDE 41 (GR + G5A, gemessen im Integrationsbaum): mit den GR-HP (Riesenform 1000 = RE2-G4-Ctor `addiu v0,zero,1000`
# @0x80100678) trifft die Rakete (w18 -> Art 9, DAT_8006f418[9] = 1000 `e8 03` @0x8006f42a) die Riesenform auf GENAU 0 (F592 HP 1000,
# F604 HP 0), 0 ist nicht tot (Todesregel hp < 0); MA7.2 endete bei F604, die 0-HP-Form toetete den Spieler (Leon F1411, Elza F1506),
# Game over -> Neustart (Bildzaehler 1672 -> 0), kein G5, Zellen-/Weg-Masse unsinnig. Deshalb MA9 (Feuer bis F658): die naechste
# Rakete bei ~F636 toetet die 0-HP-Form -> Rueckstoss F638 -> G5 F737, Armierung F792, Szenenende F1954 (integration_r41_g5_tod,
# gleiche Vorgeschichte A0.1,W1,U8.5,MA9). Die Tasten ab W56.5 liegen damit weiter hinter dem Szenenende.
# RUNDE 41 SPUR WS: Rakete RE2 je Typ (Form 1 G1 200 @0x800A58B0, Riesenform G4 301 @0x800A5D24): die Riesenform braucht 4 Raketen
# (gemessen F666 699, F698 398, F730 97, F761 -204; G5 F866, Kopf von integration_r41_g5_tod). MA9 endete vor der ersten -> MA13;
# alle Tasten danach ruecken mit dem Feuerende um 4 s (120 Bilder) nach, der G5 um 129 Bilder (F737 -> F866).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_g5_westen.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_g5_westen: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_g5_westen: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41westen_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41westen_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_f "")
function(ein_lauf _name _raum _skript)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_extra "")
    if(_raum STREQUAL "5091")
        set(_extra "RE15_TITLE_SHOT_CHAR=1")
    endif()
    re15_start_spiel(_rv 480
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=500,-9175,2048,0"
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=${_skript}"
        RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log RE15_GIVE=18:200 RE15_EQUIP=18
        "RE15_EXIT_AT=3540#${_raum}"
        "${_exe_kopie}")
    set(_g "")
    if(NOT EXISTS "${WORKDIR}/state.log")
        set(_f "${_f}\n ${_name}: kein Protokoll (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "PL\\(")
    set(_n 0)
    set(_wand 0)
    set(_erste "")
    set(_minx 999999)
    set(_maxx -999999)
    set(_minz 999999)
    set(_maxz -999999)
    set(_raum_an 0)
    set(_szene 0)
    set(_ende "")
    foreach(_z IN LISTS _zeilen)
        if(NOT _z MATCHES "^F([0-9]+) .*PL\\((-?[0-9]+),(-?[0-9]+),rot=")
            continue()
        endif()
        set(_b "${CMAKE_MATCH_1}")
        set(_x "${CMAKE_MATCH_2}")
        set(_zz "${CMAKE_MATCH_3}")
        set(_pm "")
        if(_z MATCHES " pm=([0-9]+) ")
            set(_pm "${CMAKE_MATCH_1}")
        endif()
        if(_b GREATER_EQUAL 1500 AND _pm STREQUAL "2")
            set(_szene 1)
        elseif(_szene AND NOT _ende AND NOT _pm STREQUAL "2")
            set(_ende "${_b}")
        endif()
        # erst ab dem Sprung in den Raum (Spieler auf dem Bahnsteig)
        if(_zz GREATER -30000 AND _zz LESS -17000)
            set(_raum_an 1)
        endif()
        if(NOT _raum_an)
            continue()
        endif()
        math(EXPR _n "${_n} + 1")
        set(_fest "")
        if(_zz LESS -26884 AND _x GREATER -5673 AND _x LESS 28067)
            set(_fest "#22")
        elseif(_zz GREATER_EQUAL -19870 AND _x GREATER -5673 AND _x LESS 28057)
            set(_fest "#23")
        elseif(_x GREATER_EQUAL 4050 AND _x LESS 20225 AND _zz GREATER_EQUAL -21700 AND _zz LESS -19700)
            set(_fest "#28")
        elseif(_x GREATER_EQUAL 5300 AND _x LESS 20250 AND _zz GREATER_EQUAL -27200 AND _zz LESS -25200)
            set(_fest "#29")
        elseif(_x GREATER_EQUAL -425 AND _x LESS 5300 AND _zz GREATER_EQUAL -27200 AND _zz LESS -25200)
            # #30 Typ 6: fest unter der Linie (-425,-27200)-(5300,-25200)
            math(EXPR _l "(${_zz} + 27200) * 5725")
            math(EXPR _r "(${_x} + 425) * 2000")
            if(_l LESS _r)
                set(_fest "#30")
            endif()
        endif()
        if(_fest)
            math(EXPR _wand "${_wand} + 1")
            if(NOT _erste)
                set(_erste "F${_b} (${_x},${_zz}) ${_fest}")
            endif()
        endif()
        if(_ende AND _b GREATER_EQUAL _ende)
            if(_x LESS _minx)
                set(_minx "${_x}")
            endif()
            if(_x GREATER _maxx)
                set(_maxx "${_x}")
            endif()
            if(_zz LESS _minz)
                set(_minz "${_zz}")
            endif()
            if(_zz GREATER _maxz)
                set(_maxz "${_zz}")
            endif()
        endif()
    endforeach()
    if(_n LESS 1500)
        string(APPEND _g "\n ${_name}: nur ${_n} Bilder im Raum")
    endif()
    if(_wand GREATER 0)
        string(APPEND _g "\n ${_name}: ${_wand} Bildenden in fester Zelle, erstes ${_erste}")
    endif()
    if(NOT _ende)
        string(APPEND _g "\n ${_name}: kein Ende der G5-Szene im Protokoll (pm=2 -> !=2 nach F1500)")
        set(_weg 0)
    else()
        math(EXPR _weg "${_maxx} - ${_minx} + ${_maxz} - ${_minz}")
    endif()
    if(_weg LESS 300)
        string(APPEND _g "\n ${_name}: Eingabe wirkt nicht mehr, Weg ab Szenenende F${_ende} nur ${_weg}")
    endif()
    message(STATUS "r41_g5_westen ${_name}: ${_n} Bilder, in fester Zelle ${_wand}, Szenenende F${_ende}, Weg danach ${_weg}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

# Eingaben der Abnahme: z_w4 (Leon im Westen ohne Feuer) und z_fern (Suedwand im Armbereich), danach laufen, drehen, rennen
# INTEGRATION RUNDE 41 WELLE 2 (Stufe 3 der Form-Spur, gemessen im Integrationsbaum 2a0fe07bc, Lauf lauf_5b9f175d leon_w4):
# die Riesenform ist ab der Raumladung da und faellt den vier Raketen frueher, G5-Vorlauf F652, Armierung F707, Szenenende F1869
# (vorher G5 F866 -> Armierung F921 -> Szenenende F2083). Die Tasten ab W56.5 (= F2656 bei Skriptstart F100) lagen damit 787
# statt 573 Bilder hinter dem Szenenende; der G5 biss den stehenden Spieler bei F2220/2358/2474/2592/2708 (100 -> 0) und
# toetete ihn F2731 (Neustart, Bildzaehler 2992 -> 0; das Zellenmass sah danach den Spawn (500,-9175) als #23). Die Wartezeit
# rueckt um die gemessenen 214 Bilder (7,1 s) vor: W49.4 = F2443, wieder 573 Bilder nach dem Szenenende wie in Welle 1.
set(_tasten ",W49.4,U2,W1,D2,W1,L0.5,U2,R1.1,XU2,W10")
ein_lauf(leon_w4 5090 "A0.1,W1,U8.5,MA13,W4.2,XU1.9${_tasten}")
ein_lauf(elza_w4 5091 "A0.1,W1,U8.5,MA13,W4.2,XU2.2${_tasten}")
ein_lauf(leon_fern 5090 "A0.1,W1,U8.5,MA13,W4.2,R1.08,XU2.4${_tasten}")
file(REMOVE "${_exe_kopie}")

if(_f)
    message(FATAL_ERROR "r41_g5_westen: FEHLER${_f}")
endif()
message(STATUS "r41_g5_westen: alle Pruefungen gruen")
