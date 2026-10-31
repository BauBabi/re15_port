# =============================================================================
# SPUR N "nachtrag30" MIT DER ECHTEN EXE (Runde 41, AUFTRAG.md Nachtraege 30, 30b, 30d, 30e, 34 (N-E2), 36, 37). Dossier analysis/befunde_runde41/N_nachtrag30.md.
#   TEIL=panel   FIGUR leon/elza: Pult -> Frage 40 -> Panel -> A nach B, B nach C, C nach A -> 80 -> Text 43 -> Ja -> Fahrt unter Cut 4 mit den Toenen RE2 ROOM4080
#                Satz 0x0F / 0x10 -> Text 44. Bild: Fenster von Tank B vor und nach dem Umfuellen.
#   TEIL=schrank FIGUR leon/elza: 2010 Cut 3: Aktion -> Tuer klappt auf (Bilder geschlossen / offen), Text 40; mit (3,108) Ja -> Cut 8 + Cursor wartet (BSS-Vergleich), SQUARE neben der Zone nichts,
#                Cursor im Bild an der Stelle wie ROOM2060, rechts + SQUARE -> Cut 9 + Bit 125 + Text 42 -> Standbild der Ventile; START im Cursor-Schritt oeffnet nichts; zu Fuss nach Cut 4: keine Tuer im Bild.
#   TEIL=wasser  FIGUR leon/elza: 2000 Cut 10 + Cut 9: Wasserlauf an der Leiter und auf dem Weg (Bildvergleich gegen den Lauf mit Bit 100), der Weg LAEUFT (Werkzeug fluss: Verschiebung waechst mit dem
#                Abstand der Bilder, Gegenlauf mit Bit 100 steht), Weg durch die echte Tuer 2010 -> 2000, Leiter weiter gesperrt, mit Bit 100 nichts.
#   TEIL=laden   FIGUR leon/elza: der echte Weg: Spielstand + CONTINUE (Wasserlauf 2000, Schrank 2010, Panel 2030 samt Bruecke) und Ring-Tuer 2070 -> 2030 zu Fuss zum Pult -> Panel geloest.
#   TEIL=ton     Mitschnitt-Zwillinge RE15_AUDIO_CAP_SYNC: Bruecke (Stummel BRUECKE4080.*) und Sirene (Stummel SOUND/SUB_34.BGM): vorher bitgleich, waehrend RMS, danach bitgleich
#   TEIL=laut    volle Mischung volume=100 gegen 50 und 10 (N-E2): Tuersequenz DOOR26 + Sprung ins Wasser 2090, jeder Abtastwert == (v100 * q) >> 15
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_N30_WERKZEUG=<test_r41_nachtrag30> -DRE15_R41C_WERKZEUG=<test_r41_ventilkette> -DRE15_R41C_PSX=<RE15_ASSETS_PATH>
#         -DTEIL=<teil> -DFIGUR=<leon|elza> -DWORKDIR=<dir> -P test_r41_nachtrag30.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
foreach(_v RE15_PC_EXE RE15_N30_WERKZEUG RE15_R41C_WERKZEUG RE15_R41C_PSX)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r41_nachtrag30: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR OR NOT TEIL)
    message(FATAL_ERROR "r41_nachtrag30: WORKDIR/TEIL fehlt")
endif()
if(NOT FIGUR)
    set(FIGUR leon)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_basis}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte "${_exe_dir}/re15_pc_r41n_${TEIL}_${FIGUR}_*")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41n_${TEIL}_${FIGUR}_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(_fehler _t)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_nachtrag30[${TEIL} ${FIGUR}]${_t}")
endfunction()
function(_muss _text _wort _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        _fehler(": ${_was} — '${_wort}' fehlt im debug.log")
    endif()
endfunction()
function(_darf_nicht _text _wort _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(NOT _p LESS 0)
        _fehler(": ${_was} — '${_wort}' steht im debug.log")
    endif()
endfunction()
function(_danach _text _w1 _w2 _was)
    string(FIND "${_text}" "${_w1}" _p1)
    if(_p1 LESS 0)
        _fehler(": ${_was} — '${_w1}' fehlt")
    endif()
    string(SUBSTRING "${_text}" ${_p1} -1 _r)
    string(FIND "${_r}" "${_w2}" _p2)
    if(_p2 LESS 0)
        _fehler(": ${_was} — '${_w2}' steht nicht nach '${_w1}'")
    endif()
endfunction()

if("${FIGUR}" STREQUAL "elza")
    set(_env RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
    set(S 1)
else()
    set(_env RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
    set(S 0)
endif()

macro(_lauf _name _timeout)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv ${_timeout}
        SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 ${_env}
        ${ARGN}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        _fehler(": [${_name}] kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    if("${FIGUR}" STREQUAL "elza")
        _muss("${_dbg}" "Spieler-Familie PL04" "Elza gewaehlt")
    endif()
endmacro()

function(_wert _werkzeug _re _out)
    execute_process(COMMAND "${RE15_N30_WERKZEUG}" ${_werkzeug} OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(STRIP "${_o}" _o)
    if(NOT _r EQUAL 0 OR NOT _o MATCHES "${_re}")
        _fehler(": Werkzeug ${_werkzeug}: ${_o}")
    endif()
    set(${_out} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

# ---- panel -------------------------------------------------------------------------------------------------------------------
if("${TEIL}" STREQUAL "panel")
    # Takte aus dem Messlauf 2026-10-30 (Leon, Kalibrierlauf pcal): Pult F330, Frage 40 Ja F474 = Panel offen, Text 41 (2 Seiten, je Seite zwei Tastendruecke) zu F696.
    # Cursor von (160,119), 2,515 Bildpunkte je Bild (ROOM2050 @0x023C2): L1.2 -> x 69 (Tank A), R1.0 -> x 144 (Tank B), R0.6 -> x 190 (Tank C), L1.6 -> x 69.
    set(_skript "A0.1,W1.5,A0.1,W1,A0.1,W2,A0.1,W1.5,A0.1,W1.5,A0.1,W2,A0.1,W2,A0.1,W2,L1.2,W0.3,A0.1,W0.3,R1.0,W0.3,A0.1,W1.5,A0.1,W0.3,R0.6,W0.3,A0.1,W1.5,A0.1,W0.3,L1.6,W0.3,A0.1,W3,A0.1,W1,A0.1,W2,A0.1,W2,A0.1,W2,A0.1,W6")
    _lauf(p 300 RE15_NOAUDIO=1 RE15_SE_DEBUG=1 RE15_SOFTWARE_RENDER=1
          RE15_DEBUG_JUMP=2030@250 "RE15_PLAYER_POS=350,-9500,1024,3" "RE15_SET_FLAG=7:155,7:156,7:157"
          RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=${_skript}"
          "RE15_FRAMEDUMP=780-900/120:f_" "RE15_EXIT_AT=1420#203${S}")
    _muss("${_dbg}" "[ventilkette41] ROOM203${S} Ereignis 24 -> Port-Programm 2030_panel_ein" "Pult ohne Druck oeffnet das Panel")
    _muss("${_dbg}" "[msgtext] room=203${S} id=40 \"A control panel for the bridge. Will you operate the panel?\"" "Frage 40")
    _danach("${_dbg}" "[panel2030] offen: A 100 B 0 C 0" "[msgtext] room=203${S} id=41 \"The pressure gauge reads 100.Pick a tank to drain, then a tank to fill.\"" "Text 41 im Panel")
    _danach("${_dbg}" "[panel2030] Tank A: Quelle" "[panel2030] Tank B: umgefuellt -> A 50 B 50 C 0" "A nach B")
    _danach("${_dbg}" "[panel2030] Tank B: Quelle" "[panel2030] Tank C: umgefuellt -> A 50 B 20 C 30" "B nach C")
    _danach("${_dbg}" "[panel2030] Tank C: Quelle" "[panel2030] Tank A: umgefuellt -> A 80 B 20 C 0" "C nach A")
    _danach("${_dbg}" "[panel2030] Anzeige 80 erreicht" "[panel2030] GELOEST: A 80 B 20 C 0" "Ruhe 30 Bilder")
    _danach("${_dbg}" "Panel 2030 GELOEST" "[ventilkette41] ROOM203${S} Ereignis 24 -> Port-Programm 2030_pult_senken" "Pult fragt")
    _danach("${_dbg}" "Port-Programm 2030_pult_senken" "[msgtext] room=203${S} id=43 \"The pressure gauge reads 80. Lower the bridge?\"" "bei 80 folgt Text 43")
    _danach("${_dbg}" "id=43 " "Cut_chg(4)" "Bruecke faehrt unter Cut 4")
    _danach("${_dbg}" "Cut_chg(4)" "Runde 41 Spur N: Bruecke BRUECKE4080 Satz 0x0F" "Motor mit der Fahrt")
    _danach("${_dbg}" "Satz 0x0F" "Runde 41 Spur N: Bruecke BRUECKE4080 Satz 0x10" "Halt am Ende")
    _danach("${_dbg}" "obj=0x00 m=1" "Runde 41 Spur N: Bruecke BRUECKE4080 Satz 0x10" "Halt mit der Endlage")
    _muss("${_dbg}" "[msgtext] room=203${S} id=44 \"The bridge is down.\"" "Pult danach: Text 44")
    _darf_nicht("${_dbg}" "2030_v13" "Ventil 13 entfaellt")
    set(_a "${WORKDIR}/f_000780.ppm")
    set(_b "${WORKDIR}/f_000900.ppm")
    foreach(_x a b)
        if(NOT EXISTS "${_${_x}}")
            _fehler(": Framedump ${_${_x}} fehlt")
        endif()
    endforeach()
    _wert("bild;${_a};142;92;153;116" "^BILD ([0-9]+)" _b_leer)
    _wert("bild;${_b};142;92;153;116" "^BILD ([0-9]+)" _b_voll)
    message(STATUS "r41_nachtrag30[panel ${FIGUR}]: Fenster Tank B leer ${_b_leer}, voll ${_b_voll}")
    math(EXPR _d "${_b_voll} - ${_b_leer}")
    if(_d LESS 60)
        _fehler(": Tank B fuellt sich im Bild nicht (leer ${_b_leer}, voll ${_b_voll})")
    endif()
    # Abnahme 0, Mangel 3: START bei offenem Panel oeffnet nichts (die Vollbild-Ebene liegt ueber dem Inventar), die Tasten bleiben im Panel, CROSS verlaesst es
    _lauf(st 300 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_DEBUG_JUMP=2030@250 "RE15_PLAYER_POS=350,-9500,1024,3" "RE15_SET_FLAG=7:155,7:156,7:157"
          RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W1.5,A0.1,W1,A0.1,W2,A0.1,W1.5,A0.1,W1.5,A0.1,W2,A0.1,W2,A0.1,W2,S0.1,W2,L1.2,W0.3,S0.1,W2,X0.1,W2,S0.1,W2" "RE15_EXIT_AT=1000#203${S}")
    _danach("${_dbg}" "[panel2030] offen: A 100 B 0 C 0" "[panel2030] Steuerkreuz 0x8" "Tasten bleiben im Panel")
    _danach("${_dbg}" "[panel2030] Steuerkreuz 0x8" "[panel2030] Abbruch" "CROSS verlaesst das Panel")
    string(FIND "${_dbg}" "[panel2030] Abbruch" _p)
    string(SUBSTRING "${_dbg}" 0 ${_p} _vor)
    _darf_nicht("${_vor}" "Inventar-Ton Satz 6" "START oeffnet bei offenem Panel nichts")
    string(SUBSTRING "${_dbg}" ${_p} -1 _nach)
    _muss("${_nach}" "Inventar-Ton Satz 6" "nach dem Panel oeffnet START wieder")
    message(STATUS "r41_nachtrag30[panel ${FIGUR}]: A nach B, B nach C, C nach A = 80 -> Text 43 -> Bruecke mit RE2-Ton")
endif()

# ---- schrank ------------------------------------------------------------------------------------------------------------------
if("${TEIL}" STREQUAL "schrank")
    # (3,108) = 0: Klick klappt auf, danach der Text wie Spur C. Spieler (4300,-2770) Blick +x -> Vorwaertspunkt im Platz, die Figur verdeckt in Cut 3 die Tuer nicht.
    _lauf(a 300 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_DEBUG_JUMP=2010@250 "RE15_PLAYER_POS=4300,-2770,0,0" RE15_FORCE_CUT=3
          RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1" "RE15_FRAMEDUMP=326-366/40:f_" "RE15_EXIT_AT=380#201${S}")
    _danach("${_dbg}" "Port-Programm 2010_kasten_leer" "[schrank2010] Tuer klappt auf" "Klick klappt auf")
    _danach("${_dbg}" "[schrank2010] Tuer klappt auf" "[schrank2010] Tuer auf (rot_y 1024)" "90 Grad")
    set(_zu "${WORKDIR}/f_000326.ppm")
    set(_auf "${WORKDIR}/f_000366.ppm")
    if(NOT EXISTS "${_zu}" OR NOT EXISTS "${_auf}")
        _fehler(": Framedumps fehlen (${_zu})")
    endif()
    _wert("diff;${_zu};${_auf};224;30;258;200" "^DIFF ([0-9]+)" _d)
    _wert("diff;${_zu};${_auf};20;30;120;170" "^DIFF ([0-9]+)" _g)   # ueber dem Text (y >= 186)
    message(STATUS "r41_nachtrag30[schrank ${FIGUR}]: Schrankflaeche geschlossen <-> offen ${_d}, Gegenprobe links ${_g} (Zehntel Graustufen)")
    if(_d LESS 150 OR NOT _g EQUAL 0)
        _fehler(": Aufklappen im Bild nicht belegt (Flaeche ${_d}, Gegenprobe ${_g})")
    endif()
    # Ja-Zweig: Tasten wie k1 in test_r41_ventilkette_weg.cmake. Cursor-Schritt wie ROOM2060 sub18/sub19 (Dossier Nachbesserung 1): F640 Cut 8 + Cursor, F650 SQUARE neben der Zone und F660 START tun nichts,
    # F680 Cursor unveraendert, F700 rechts = Zone 7, F710 SQUARE -> F730 Cut 9 + Text 42, F790 Text zu, Standbild.
    _lauf(b 300 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_FLAG_CENSUS=1 RE15_SE_DEBUG=1 RE15_DEBUG_JUMP=2010@250 "RE15_PLAYER_POS=4950,-2770,0,0" RE15_SET_FLAG=3:108
          "RE15_PRESS=square@300,square@368,square@418,square@468,square@538,square@618,square@650,start@660,right@700,square@710,square@790,square@870"
          "RE15_FRAMEDUMP=640-730/10:f_" "RE15_EXIT_AT=960#201${S}")
    _danach("${_dbg}" "[schrank2010] Tuer auf" "[msgtext] room=201${S} id=41 \"Will you use the Fuse?\"" "Frage nach dem Aufklappen")
    _danach("${_dbg}" "[schrank2010] Nahansicht ROOM2060 Cut 8, Cursor obj 4 bei (19571,-22542)" "[schrank2010] Nahansicht ROOM2060 Cut 9, Cursor (19771,-22542) auf Zone 7" "Cut 8, dann Cut 9")
    _darf_nicht("${_dbg}" "Inventar-Ton Satz 6" "START im Cursor-Schritt oeffnet nichts")
    _danach("${_dbg}" "[schrank2010] Nahansicht ROOM2060 Cut 9" "WRITE Set  zone=9 idx=125 op=1" "Bit 125 hinter dem Cursor-Schritt")
    _danach("${_dbg}" "zone=9 idx=125 op=1" "[msgtext] room=201${S} id=42 \"You've used the Fuse.\"" "Text 42 auf Cut 9")
    _danach("${_dbg}" "id=42 \"You've used the Fuse.\"" "[ventilkette41] Standbild der Ventile offen" "Standbild wie Spur C")
    foreach(_paar "8;640" "8;680" "9;730")
        list(GET _paar 0 _c)
        list(GET _paar 1 _f)
        execute_process(COMMAND "${RE15_R41C_WERKZEUG}" bssdiff "${RE15_R41C_PSX}/BSS/ROOM2060/BG0${_c}.BSS" "${WORKDIR}/f_000${_f}.ppm" 0 0 320 170 OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
        string(REGEX MATCH "ABWEICHUNG ([0-9]+) GLEICH ([0-9]+)" _m "${_o}")
        if(NOT _m OR CMAKE_MATCH_2 LESS 95)
            _fehler(": F${_f} ist nicht ROOM2060 Cut ${_c}: ${_o}")
        endif()
        message(STATUS "r41_nachtrag30[schrank ${FIGUR}]: F${_f} = ROOM2060 Cut ${_c}, ${CMAKE_MATCH_2} % gleich")
    endforeach()
    # Der Cursor steht im Bild: Stempel an der Startlage (Rahmen x 147..171, y 108..130), gegen ROOM2060 nach links verschoben ist die Flaeche leer
    _wert("diff;${WORKDIR}/f_000640.ppm;${WORKDIR}/f_000730.ppm;147;108;172;131" "^DIFF ([0-9]+)" _z)
    _wert("diff;${WORKDIR}/f_000640.ppm;${WORKDIR}/f_000680.ppm;147;108;172;131" "^DIFF ([0-9]+)" _s)
    message(STATUS "r41_nachtrag30[schrank ${FIGUR}]: Cursor F640 gegen Cut 9 ${_z}, gegen F680 (SQUARE neben der Zone, START) ${_s}")
    if(_z LESS 100 OR NOT _s EQUAL 0)
        _fehler(": Cursor im Bild nicht belegt (${_z}, ${_s})")
    endif()
    # Abnahme 0, Mangel 4: zu Fuss nach Cut 4 steht die offene Tuer nicht im Bild (Plan wie Lauf s_geh2); Spalten 0..27 = Hintergrund BG04 (Lauf s_geh3 100 %, mit Tuer s_geh2 29 %), Figur steht weiter rechts
    file(WRITE "${_basis}_plan_c4.txt" "RAUM 201${S}
WARTE 30
GEHE 4950 -2770 100
BLICK 0
WARTE 10
AKTION
WARTE 60
AKTION
WARTE 30
AKTION
WARTE 30
AKTION
WARTE 30
GEHE 3500 -1500 100
WARTE 30
GEHE 3000 -5000 100
WARTE 30
GEHE -1000 -5000 100
WARTE 90
ENDE
")
    _lauf(c4 300 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_DEBUG_JUMP=2010@250 "RE15_WEGPLAN=${_basis}_plan_c4.txt" RE15_WEGPLAN_LOG=wegplan.txt "RE15_FRAMEDUMP=700:f.ppm" "RE15_EXIT_AT=720#201${S}")
    _muss("${_dbg}" "[schrank2010] Tuer auf (rot_y 1024)" "Tuer offen")
    string(FIND "${_dbg}" "[pri] cut=" _q REVERSE)
    string(SUBSTRING "${_dbg}" ${_q} 12 _z)
    if(NOT "${_z}" STREQUAL "[pri] cut=4 ")
        _fehler(": Plan endet nicht in Cut 4 ('${_z}')")
    endif()
    execute_process(COMMAND "${RE15_R41C_WERKZEUG}" bssdiff "${RE15_R41C_PSX}/BSS/ROOM2010/BG04.BSS" "${WORKDIR}/f.ppm" 0 0 28 240 OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(REGEX MATCH "ABWEICHUNG ([0-9]+) GLEICH ([0-9]+)" _m "${_o}")
    message(STATUS "r41_nachtrag30[schrank ${FIGUR}]: Cut 4 Spalten 0..27 gegen BG04: ${_o}")
    if(NOT _m OR CMAKE_MATCH_2 LESS 95)
        _fehler(": offene Tuer steht in Cut 4 im Bild: ${_o}")
    endif()
    # Klick von Norden: der Platz reicht bis unter Cut 4 (Lauf wk): Cut_chg 3, die Tuer klappt im Bild auf, Cut_old zurueck nach Cut 4
    file(WRITE "${_basis}_plan_n.txt" "RAUM 201${S}
WARTE 30
GEHE 3000 -5000 100
GEHE 4950 -4500 100
GEHE 4950 -3950 100
BLICK 3072
WARTE 10
AKTION
WARTE 60
AKTION
WARTE 30
AKTION
WARTE 30
AKTION
WARTE 60
ENDE
")
    _lauf(n 300 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_DEBUG_JUMP=2010@250 "RE15_WEGPLAN=${_basis}_plan_n.txt" RE15_WEGPLAN_LOG=wegplan.txt "RE15_EXIT_AT=520#201${S}")
    string(FIND "${_dbg}" "Port-Programm 2010_kasten_leer" _p)
    if(_p LESS 0)
        _fehler(": Klick von Norden traf den Schrank nicht")
    endif()
    string(SUBSTRING "${_dbg}" 0 ${_p} _vor)
    string(FIND "${_vor}" "[pri] cut=" _q REVERSE)
    string(SUBSTRING "${_vor}" ${_q} 12 _z)
    if(NOT "${_z}" STREQUAL "[pri] cut=4 ")
        _fehler(": Klick von Norden: Kamera vor dem Klick '${_z}' statt Cut 4")
    endif()
    _danach("${_dbg}" "Port-Programm 2010_kasten_leer" "Cut_chg(3)" "Klick von Norden schaltet auf Cut 3")
    _danach("${_dbg}" "Cut_chg(3)" "[pri] cut=3" "Cut 3 im Bild")
    _danach("${_dbg}" "[pri] cut=3" "[schrank2010] Tuer auf (rot_y 1024)" "Aufklappen unter Cut 3")
    _danach("${_dbg}" "[schrank2010] Tuer auf (rot_y 1024)" "[pri] cut=4" "Cut_old zurueck nach Cut 4")
endif()

# ---- wasser -------------------------------------------------------------------------------------------------------------------
if("${TEIL}" STREQUAL "wasser")
    set(_ort "RE15_PLAYER_POS=-26300,6500,1024,1")
    # Abnahme 0, Mangel 2: Flaechen und Schwellen aus den Laeufen y10/y9 (Nachbesserung 1, Dossier): Cut 10 Weg x 72..124, y 146..179, 2,5 Bildpunkte je Bild; Cut 9 Weg x 90..149, y 120..149, 2,3 je Bild.
    function(_fluss _ordner _x0 _y0 _x1 _y1 _was _laeuft)
        execute_process(COMMAND "${RE15_N30_WERKZEUG}" fluss "${_ordner}" 430 461 ${_x0} ${_y0} ${_x1} ${_y1} OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
        string(STRIP "${_o}" _o)
        message(STATUS "r41_nachtrag30[wasser ${FIGUR}]: ${_was}: ${_o}")
        if(NOT _r EQUAL 0 OR NOT _o MATCHES "^FLUSS (-?[0-9]+) (-?[0-9]+) (-?[0-9]+) (-?[0-9]+) (-?[0-9]+) (-?[0-9]+) (-?[0-9]+) (-?[0-9]+) (-?[0-9]+) AENDERUNG")
            _fehler(": fluss ${_was}: ${_o}")
        endif()
        set(_s1 ${CMAKE_MATCH_1})
        set(_k1 ${CMAKE_MATCH_2})
        set(_n1 ${CMAKE_MATCH_3})
        set(_s2 ${CMAKE_MATCH_4})
        set(_s3 ${CMAKE_MATCH_7})
        if(NOT _o MATCHES "AENDERUNG ([0-9]+) VON ([0-9]+)")
            _fehler(": fluss ${_was}: ${_o}")
        endif()
        set(_a ${CMAKE_MATCH_1})
        math(EXPR _viertel "${CMAKE_MATCH_2} / 6")
        if(_laeuft)
            if(_s1 LESS 1 OR NOT _s2 GREATER _s1 OR NOT _s3 GREATER _s2 OR NOT _k1 GREATER _n1 OR _a LESS _viertel)
                _fehler(": ${_was}: es laeuft nichts nach rechts (${_o})")
            endif()
        else()
            if(_a GREATER 0)
                _fehler(": ${_was}: Gegenprobe bewegt sich (${_o})")
            endif()
        endif()
    endfunction()
    _lauf(an 300 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_DEBUG_JUMP=2000@250 ${_ort} RE15_FORCE_CUT=10 "RE15_FRAMEDUMP=430-461/1:f_" "RE15_EXIT_AT=465#200${S}")
    _muss("${_dbg}" "[wasserlauf41] ROOM200${S}: 6 Strahlen, Lauf ueber den Weg Effekt 0x0F" "Wasserlauf an")
    set(_an "${WORKDIR}/f_000430.ppm")
    _fluss("${WORKDIR}" 72 146 125 180 "Cut 10 Weg" 1)
    _lauf(an9 300 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_DEBUG_JUMP=2000@250 ${_ort} RE15_FORCE_CUT=9 "RE15_FRAMEDUMP=430-461/1:f_" "RE15_EXIT_AT=465#200${S}")
    _fluss("${WORKDIR}" 90 120 150 150 "Cut 9 Weg" 1)
    _lauf(aus 300 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_DEBUG_JUMP=2000@250 ${_ort} RE15_FORCE_CUT=10 "RE15_SET_FLAG=9:100" "RE15_FRAMEDUMP=430-461/1:f_" "RE15_EXIT_AT=465#200${S}")
    _darf_nicht("${_dbg}" "[wasserlauf41]" "mit Bit 100 kein Wasserlauf")
    set(_aus "${WORKDIR}/f_000430.ppm")
    _fluss("${WORKDIR}" 72 146 125 180 "Cut 10 Weg mit Bit 100" 0)
    foreach(_d "${_an}" "${_aus}")
        if(NOT EXISTS "${_d}")
            _fehler(": Framedump ${_d} fehlt")
        endif()
    endforeach()
    # Leiter, Weg, Gegenprobe (rechte Bildhaelfte, dort kein Wasserlauf): Regionen aus den Kalibrierlaeufen 2026-10-31, siehe Dossier §7
    _wert("diff;${_an};${_aus};20;60;80;170" "^DIFF ([0-9]+)" _l)
    _wert("diff;${_an};${_aus};76;146;118;168" "^DIFF ([0-9]+)" _w)
    _wert("diff;${_an};${_aus};230;40;310;200" "^DIFF ([0-9]+)" _g)
    message(STATUS "r41_nachtrag30[wasser ${FIGUR}]: Leiter ${_l}, Weg ${_w}, Gegenprobe ${_g} (Zehntel Graustufen)")
    if(_l LESS 60 OR _w LESS 60 OR _g GREATER 12)
        _fehler(": Wasserlauf im Bild nicht belegt (Leiter ${_l}, Weg ${_w}, Gegenprobe ${_g})")
    endif()
    # echter Weg: Tuer 2010 -> 2000 (Prop-Lader beim Raumwechsel, Modell der nassen Spur)
    _lauf(tuer 300 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_DEBUG_JUMP=2010@250 "RE15_FIRE_AOT=0@300#201${S}" RE15_FORCE_CUT=10 "RE15_FRAMEDUMP=200:f.ppm" "RE15_EXIT_AT=210#200${S}")
    _muss("${_dbg}" "loaded room200${S}.rdt" "Tuer 2010 -> 2000")
    _muss("${_dbg}" "[wasserlauf41] ROOM200${S}: 6 Strahlen, Lauf ueber den Weg Effekt 0x0F" "Wasserlauf nach der Tuer")
    _wert("diff;${WORKDIR}/f.ppm;${_aus};76;146;118;168" "^DIFF ([0-9]+)" _w)
    message(STATUS "r41_nachtrag30[wasser ${FIGUR}]: nach der Tuer Weg ${_w}")
    if(_w LESS 60)
        _fehler(": Lauf fehlt nach der Tuer (${_w})")
    endif()
    _lauf(leiter 300 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_DEBUG_JUMP=2000@250 "RE15_PLAYER_POS=-26000,10300,2048"
          RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W2,A0.1,W3" "RE15_EXIT_AT=520#200${S}")
    _muss("${_dbg}" "[msgtext] room=200${S} id=40 \"I cannot climb up the ladder because of the water pressure.\"" "Leiter weiter gesperrt")
    _darf_nicht("${_dbg}" "loaded room205" "die Leiter fuehrt nicht nach 2050")
endif()

# ---- laden --------------------------------------------------------------------------------------------------------------------
if("${TEIL}" STREQUAL "laden")
    # Lade-Weg: Spielstand vom Werkzeug "karte" (Muster probe_r36_elza_karte.c, mit y: das Band folgt beim Laden aus y), danach RE15_CONTINUE_TEST + RE15_CARD_AUTO.
    macro(_karte_lauf _name _timeout _karte)
        set(WORKDIR "${_basis}_${_name}")
        file(MAKE_DIRECTORY "${WORKDIR}")
        execute_process(COMMAND "${RE15_N30_WERKZEUG}" karte re15_card.mcr ${_karte} WORKING_DIRECTORY "${WORKDIR}" OUTPUT_VARIABLE _ok RESULT_VARIABLE _rk)
        if(NOT _rk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
            _fehler(": [${_name}] Karte: ${_ok}")
        endif()
        re15_start_spiel(_rv ${_timeout} SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
            RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 ${ARGN} "${_exe_kopie}")
        if(NOT EXISTS "${WORKDIR}/debug.log")
            _fehler(": [${_name}] kein debug.log (exit=${_rv})")
        endif()
        file(READ "${WORKDIR}/debug.log" _dbg)
        _muss("${_dbg}" "[save] CONTINUE: resumed" "${_name}: Spielstand geladen")
        if("${FIGUR}" STREQUAL "elza")
            _muss("${_dbg}" "Spieler-Familie PL04" "Elza geladen")
        endif()
    endmacro()
    set(_spinnen "7:155;7:156;7:157")
    # Wasserlauf: Modell und Strahlen auch am Lade-Weg, Gegenlauf mit Bit 100
    _karte_lauf(wan 300 "200${S};-26300;-1800;6500;1024" RE15_FORCE_CUT=10 RE15_FRAMEDUMP=200:f.ppm "RE15_EXIT_AT=210#200${S}")
    _muss("${_dbg}" "[wasserlauf41] ROOM200${S}: 6 Strahlen, Lauf ueber den Weg Effekt 0x0F" "Wasserlauf nach CONTINUE")
    set(_an "${WORKDIR}/f.ppm")
    _karte_lauf(waus 300 "200${S};-26300;-1800;6500;1024;9:100" RE15_FORCE_CUT=10 RE15_FRAMEDUMP=200:f.ppm "RE15_EXIT_AT=210#200${S}")
    _darf_nicht("${_dbg}" "[wasserlauf41]" "mit Bit 100 kein Wasserlauf")
    _wert("diff;${_an};${WORKDIR}/f.ppm;20;60;80;170" "^DIFF ([0-9]+)" _l)
    _wert("diff;${_an};${WORKDIR}/f.ppm;76;146;118;168" "^DIFF ([0-9]+)" _w)
    _wert("diff;${_an};${WORKDIR}/f.ppm;230;40;310;200" "^DIFF ([0-9]+)" _g)
    message(STATUS "r41_nachtrag30[laden ${FIGUR}]: Wasserlauf nach CONTINUE: Leiter ${_l}, Weg ${_w}, Gegenprobe ${_g}")
    if(_l LESS 60 OR _w LESS 60 OR _g GREATER 12)
        _fehler(": Wasserlauf nach CONTINUE nicht im Bild (Leiter ${_l}, Weg ${_w}, Gegenprobe ${_g})")
    endif()
    # Schrank nach CONTINUE
    _karte_lauf(s 300 "201${S};4300;0;-2770;0" RE15_FORCE_CUT=3 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=120 RE15_INPUT_SCRIPT=A0.1 RE15_FRAMEDUMP=116-156/40:f_ "RE15_EXIT_AT=170#201${S}")
    _danach("${_dbg}" "Port-Programm 2010_kasten_leer" "[schrank2010] Tuer auf (rot_y 1024)" "Klick klappt auf")
    _wert("diff;${WORKDIR}/f_000116.ppm;${WORKDIR}/f_000156.ppm;224;30;258;200" "^DIFF ([0-9]+)" _d)
    message(STATUS "r41_nachtrag30[laden ${FIGUR}]: Schrank nach CONTINUE: Flaeche ${_d}")
    if(_d LESS 150)
        _fehler(": Schrank klappt nach CONTINUE nicht sichtbar auf (${_d})")
    endif()
    # Schrank zu Fuss: quer durch den Raum, die Kamera schaltet selbst auf Cut 3 (der Riegel oben erzwingt den Cut, weil die Figur dort sonst die Flaeche verdeckt)
    file(WRITE "${_basis}_plan_s.txt" "RAUM 201${S}
WARTE 30
GEHE 4950 -2770 100
BLICK 0
WARTE 10
AKTION
WARTE 60
ENDE
")
    _karte_lauf(sw 300 "201${S};-4600;0;-4950;0" "RE15_WEGPLAN=${_basis}_plan_s.txt" RE15_WEGPLAN_LOG=wegplan.txt "RE15_EXIT_AT=420#201${S}")
    string(FIND "${_dbg}" "[schrank2010] Tuer klappt auf" _p)
    if(_p LESS 0)
        _fehler(": zu Fuss: kein Aufklappen")
    endif()
    string(SUBSTRING "${_dbg}" 0 ${_p} _vor)
    string(FIND "${_vor}" "[pri] cut=" _q REVERSE)
    string(SUBSTRING "${_vor}" ${_q} 12 _z)
    if(NOT "${_z}" STREQUAL "[pri] cut=3 ")
        _fehler(": zu Fuss: Kamera beim Aufklappen '${_z}' statt Cut 3")
    endif()
    # Panel: das Skript wie im Teil panel, 210 Bilder frueher
    set(_skript "A0.1,W1.5,A0.1,W1,A0.1,W2,A0.1,W1.5,A0.1,W1.5,A0.1,W2,A0.1,W2,A0.1,W2,L1.2,W0.3,A0.1,W0.3,R1.0,W0.3,A0.1,W1.5,A0.1,W0.3,R0.6,W0.3,A0.1,W1.5,A0.1,W0.3,L1.6,W0.3,A0.1,W3,A0.1,W1,A0.1,W2,A0.1,W2,A0.1,W2,A0.1,W6")
    _karte_lauf(p 300 "203${S};350;-5400;-9500;1024;${_spinnen}" RE15_SE_DEBUG=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=120 "RE15_INPUT_SCRIPT=${_skript}" "RE15_EXIT_AT=1210#203${S}")
    _danach("${_dbg}" "[panel2030] offen: A 100 B 0 C 0" "[panel2030] GELOEST: A 80 B 20 C 0" "Panel nach CONTINUE geloest")
    _danach("${_dbg}" "[panel2030] GELOEST" "Bruecke BRUECKE4080 Satz 0x10" "Fahrt nach dem Raetsel")
    # Tuer: 2070 Slot 1 @0x00A5A = Ring-Tuer (Ankunft (6550,-5400,-100) nfloor 3), zu Fuss zum Pult (Wegplan), ab dem Pult das Skript. Der Plan endet in Bild ~344 (Messlauf kt, Leon).
    file(WRITE "${_basis}_plan.txt" "RAUM 203${S}
WARTE 30
GEHE 6575 -9675
GEHE 350 -9675
BLICK 1024
WARTE 10
ENDE
")
    _karte_lauf(t 300 "207${S};0;0;0;0;${_spinnen}" "RE15_FIRE_AOT=1@60#207${S}" "RE15_WEGPLAN=${_basis}_plan.txt" RE15_WEGPLAN_LOG=wegplan.txt RE15_SE_DEBUG=1
                RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=400 "RE15_INPUT_SCRIPT=${_skript}" "RE15_EXIT_AT=1490#203${S}")
    _muss("${_dbg}" "[aot] DOOR FIRE slot=1" "Ring-Tuer")
    _muss("${_dbg}" "loaded room203${S}.rdt" "Ring-Tuer -> 2030")
    _danach("${_dbg}" "[panel2030] offen: A 100 B 0 C 0" "[panel2030] GELOEST: A 80 B 20 C 0" "Panel nach der Tuer geloest")
    _danach("${_dbg}" "[panel2030] GELOEST" "Bruecke BRUECKE4080 Satz 0x10" "Fahrt nach dem Raetsel")
    message(STATUS "r41_nachtrag30[laden ${FIGUR}]: Panel nach CONTINUE und nach der Ring-Tuer geloest, Bruecke mit RE2-Ton")
endif()

# ---- ton ----------------------------------------------------------------------------------------------------------------------
if("${TEIL}" STREQUAL "ton")
    # Zwilling: Stummel der RE2-Dateien, alles andere bleibt (asset_root_pc.c re15_pc_read_re2: erst RE15_RE2_ASSET_ROOT, dann <shared>/RE2)
    set(_ohne "${_basis}/re2_ohne")
    file(MAKE_DIRECTORY "${_ohne}/SOUND")
    foreach(_d BRUECKE4080.EDT BRUECKE4080.VH BRUECKE4080.VB SOUND/SUB_34.BGM)
        file(WRITE "${_ohne}/${_d}" "0000")
    endforeach()
    # Bruecke: Bit 101 gesetzt -> Pult -> Frage 43 -> Ja (Tasten wie die Frage 40 im Teil panel) -> Fahrt
    macro(_bruecke _name)
        _lauf(${_name} 500 RE15_SOFTWARE_RENDER=1 RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1
              RE15_DEBUG_JUMP=2030@250 "RE15_PLAYER_POS=350,-9500,1024,3" "RE15_SET_FLAG=7:155,7:156,7:157,9:101"
              RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330 "RE15_INPUT_SCRIPT=A0.1,W1.5,A0.1,W1,A0.1,W2,A0.1,W6"
              ${ARGN} "RE15_EXIT_AT=800#2030")
    endmacro()
    _bruecke(ba)
    string(REGEX MATCH "Satz 0x0F \\(RE2 ROOM4080 sub13\\) F[0-9]+ capTick ([0-9]+)" _m "${_dbg}")
    set(_t_an "${CMAKE_MATCH_1}")
    string(REGEX MATCH "Satz 0x10 \\(RE2 ROOM4080 sub13\\) F[0-9]+ capTick ([0-9]+)" _m "${_dbg}")
    set(_t_aus "${CMAKE_MATCH_1}")
    if("${_t_an}" STREQUAL "" OR "${_t_aus}" STREQUAL "")
        _fehler(": Bruecken-Ton fehlt im Log")
    endif()
    set(_a_cap "${WORKDIR}/cap.raw")
    _bruecke(bb "RE15_RE2_ASSET_ROOT=${_ohne}")
    _muss("${_dbg}" "Bank BRUECKE4080 (RE2 ROOM4080 snd0) FEHLT -> stumm" "Zwilling ohne Bank")
    math(EXPR _d "${_t_aus} - ${_t_an}")
    message(STATUS "r41_nachtrag30[ton]: Bruecke capTick ${_t_an} .. ${_t_aus} (${_d} Bilder)")
    if(NOT _d EQUAL 60)
        _fehler(": Motor -> Halt ${_d} Bilder statt 60 (Fahrt-Schleife des Moduls Ventilkette)")
    endif()
    execute_process(COMMAND "${RE15_R41C_WERKZEUG}" capdiff "${_a_cap}" "${WORKDIR}/cap.raw" ${_t_an} ${_t_aus} OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(STRIP "${_o}" _o)
    message(STATUS "r41_nachtrag30[ton]: Bruecke A-B: ${_o}")
    if(NOT _o MATCHES "^CAPDIFF VOR ([0-9]+) WAEHREND ([0-9]+)\\.[0-9] NACH ([0-9]+) ")
        _fehler(": capdiff: ${_o}")
    endif()
    if(NOT CMAKE_MATCH_1 EQUAL 0 OR CMAKE_MATCH_2 LESS 300 OR NOT CMAKE_MATCH_3 EQUAL 0)
        _fehler(": Bruecken-Ton nicht belegt: ${_o}")
    endif()
    # Der Motor laeuft bis zum Halt (Dauerschleife in RE2, Stimme 6) und nicht darueber hinaus: letzte 10 Bilder der Fahrt hoerbar
    math(EXPR _t10 "${_t_aus} - 10")
    execute_process(COMMAND "${RE15_R41C_WERKZEUG}" capdiff "${_a_cap}" "${WORKDIR}/cap.raw" ${_t10} ${_t_aus} OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(STRIP "${_o}" _o)
    message(STATUS "r41_nachtrag30[ton]: letzte 10 Bilder vor dem Halt: ${_o}")
    if(NOT _o MATCHES "^CAPDIFF VOR [0-9]+ WAEHREND ([0-9]+)\\.[0-9] " OR CMAKE_MATCH_1 LESS 300)
        _fehler(": Motor bricht vor dem Halt ab: ${_o}")
    endif()
    # Sirene: (5,0x1D) = Zaehler (nach dem Warntext, Programm 2030_ventile), die ganze Minute (Schleife 14,3 s mehrfach, "durchgehend"), danach die Ost-Tuer: der Raum wird verlassen.
    # Ende mit der Flut: integration_r41_ventilkette_alarm.
    macro(_sirene _name)
        _lauf(${_name} 500 RE15_SOFTWARE_RENDER=1 RE15_AUDIO_CAP_SYNC=cap.raw
              RE15_DEBUG_JUMP=2030@250 "RE15_PLAYER_POS=6550,-100,0,3" "RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102"
              "RE15_SET_FLAG_AT=5:29@420" "RE15_FIRE_AOT=0@2300#2030" ${ARGN} "RE15_EXIT_AT=200#20B0")
    endmacro()
    _sirene(sa)
    _muss("${_dbg}" "[ventilkette41] Zaehler laeuft: 1800 Takte" "Zaehler")
    _danach("${_dbg}" "Sirene ROOM6000 (RE2 SUB_34) AN" "Sirene ROOM6000 (RE2 SUB_34) AUS" "Sirene aus beim Verlassen")
    set(_a_cap "${WORKDIR}/cap.raw")
    string(REGEX MATCH "Sirene ROOM6000 \\(RE2 SUB_34\\) AN, ROOM2030, capTick ([0-9]+)" _m "${_dbg}")
    set(_t_an "${CMAKE_MATCH_1}")
    string(REGEX MATCH "Sirene ROOM6000 \\(RE2 SUB_34\\) AUS, ROOM[0-9A-F]+, capTick ([0-9]+)" _m "${_dbg}")
    set(_t_aus "${CMAKE_MATCH_1}")
    _sirene(sb "RE15_RE2_ASSET_ROOT=${_ohne}")
    _muss("${_dbg}" "Sirene ROOM6000 (RE2 SUB_34) NICHT geladen" "Zwilling ohne SUB_34")
    execute_process(COMMAND "${RE15_R41C_WERKZEUG}" capdiff "${_a_cap}" "${WORKDIR}/cap.raw" ${_t_an} ${_t_aus} OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(STRIP "${_o}" _o)
    message(STATUS "r41_nachtrag30[ton]: Sirene capTick ${_t_an} .. ${_t_aus} A-B: ${_o}")
    if(NOT _o MATCHES "^CAPDIFF VOR ([0-9]+) WAEHREND ([0-9]+)\\.[0-9] NACH ([0-9]+) ")
        _fehler(": capdiff: ${_o}")
    endif()
    set(_vor "${CMAKE_MATCH_1}")
    set(_w "${CMAKE_MATCH_2}")
    set(_n "${CMAKE_MATCH_3}")
    if(NOT _vor EQUAL 0 OR _w LESS 300 OR NOT _n EQUAL 0)
        _fehler(": Sirene: vor ${_vor} (0), waehrend ${_w} (>= 300), nach ${_n} (0)")
    endif()
    math(EXPR _d "${_t_aus} - ${_t_an}")
    if(_d LESS 1800)
        _fehler(": Sirene nur ${_d} Takte vermessen, verlangt die ganze Minute (1800)")
    endif()
    execute_process(COMMAND "${RE15_N30_WERKZEUG}" luecke "${_a_cap}" "${WORKDIR}/cap.raw" ${_t_an} ${_t_aus} 100 OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(STRIP "${_o}" _o)
    message(STATUS "r41_nachtrag30[ton]: Sirene durchgehend: ${_o}")
    if(NOT _o MATCHES "^LUECKE 0 TAKTE")
        _fehler(": Sirene setzt aus: ${_o}")
    endif()
endif()

# ---- laut ---------------------------------------------------------------------------------------------------------------------
if("${TEIL}" STREQUAL "laut")
    # 20A0 -> Tuer Slot 0 (Tuersequenz DOOR26 mit Schliesston; Tor offen (9,113), Szene gesehen (9,110)) -> 2090, Sprung ins Becken (Absprung y -1800, Landung y 0): die Toene aus
    # Nachtrag 30b. Eine Endstufe am Ende der Mischung: jeder Abtastwert folgt.
    macro(_mix _name _v)
        file(MAKE_DIRECTORY "${_basis}_${_name}")
        file(WRITE "${_basis}_${_name}/opt.cfg" "volume=${_v}\nsound=stereo\n")
        _lauf(${_name} 300 RE15_DEBUG_JUMP=20A0@240 "RE15_PLAYER_POS=4950,5000,2048,2" RE15_SET_FLAG=9:113,9:110 "RE15_OPTIONS_FILE=${_basis}_${_name}/opt.cfg" RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1
              "RE15_INPUT_SCRIPT=W9,A0.2,W4,R0.37,W0.3,U0.7,W0.2,A0.2,W0.3,A0.2,W0.3,A0.2,W0.3,A0.2,W4" RE15_INPUT_SCRIPT_START=0 "RE15_EXIT_AT=420#2090")
    endmacro()
    _mix(v100 100)
    _muss("${_dbg}" "[tuer] Bild 60 Se_on Tonkopf-Eintrag 0" "Tuersequenz gelaufen")
    _muss("${_dbg}" "[tuer] Sequenz fertig: 286 Bilder + 1 Warten, 1 Se_on, Schliesston 1" "Tuersequenz mit Schliesston")
    if(NOT _dbg MATCHES "F210 cut=3 mo=[0-9]+ af=[0-9]+ frz=0 . pl pos=.6363,0,-14032.")
        _fehler(": Sprung ins Becken: Landung (6363,0,-14032) fehlt im Log")
    endif()
    set(_c100 "${WORKDIR}/cap.raw")
    _mix(v50 50)
    set(_c50 "${WORKDIR}/cap.raw")
    _mix(v10 10)
    set(_c10 "${WORKDIR}/cap.raw")
    foreach(_paar "_c50;0x2000" "_c10;327")
        list(GET _paar 0 _n)
        list(GET _paar 1 _q)
        execute_process(COMMAND "${RE15_N30_WERKZEUG}" endstufe "${_c100}" "${${_n}}" ${_q} OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
        string(STRIP "${_o}" _o)
        message(STATUS "r41_nachtrag30[laut]: ${_n} q ${_q}: ${_o}")
        if(NOT _r EQUAL 0 OR NOT _o MATCHES "falsch 0 von ")
            _fehler(": Endstufe ${_n}: ${_o}")
        endif()
        if(_o MATCHES "laut 0,")
            _fehler(": Mitschnitt ohne Ton: ${_o}")
        endif()
    endforeach()
endif()

file(REMOVE "${_exe_kopie}")
