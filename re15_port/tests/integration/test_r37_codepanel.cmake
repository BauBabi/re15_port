cmake_minimum_required(VERSION 3.16)
# =============================================================================
# Runde 37 Spur E "codepanel" MIT DER ECHTEN EXE (analysis/befunde_runde37/E_codepanel.md).
#
# Nutzer (AUFTRAG.md #9 + Nachtraege 3/3b): solange zu leuchtet die ROT gemalte Lampe rot; ist der Code richtig, leuchtet
# die andere gemalte Lampe in ihrer Farbe (ROOM10D0 "2F CORRIDOR" = "Korridor 2": blau; SECUGUARD: gruen); die Kamera
# bleibt auf dem Panel, waehrend Piep und Aufschliess-Ton kommen.
#
#   B  ROOM10D0 ECHTER WEG: Blue Keycard im Inventar (RE15_GIVE 0x38) + Taken-Bit (9,52) (sub20 @0x019C0), Kartenleser
#      per RE15_FIRE_AOT=1 (main00 @0x0103E Aot_set Platz 1 sce 3 -> sub20), "Ja" auf msg 7, "You've used the Blue
#      Keycard." (msg 9), Tastenfeld (sub17), Code 4312 per Cursor (D-Pad) + Viereck, "You've opened the lock.",
#      Wegwerf-Abfrage. MUSS: Lampe ZU ab dem Panel, Piep P und Aufschliessen P+15 (auf.log), der angezeigte Cut ist an
#      P+15 noch 11 (cut.log), die Kamera geht erst NACH P+15 zurueck, Lampe OFFEN (blau) ab P+1; Bild 700 (zwischen den
#      Ziffern): rot gemaltes Glas rot-dominant; Bild 900 (Nachricht offen): blau gemaltes Glas blau-dominant.
#   C  ROOM1230 (SECUGUARD, Red Keycard): Tastenfeld per RE15_SUBSTART=17, Ziffern-Riegel (5,13..16) per
#      RE15_SET_FLAG_AT (sub01 @0x00FDA -> sub19). MUSS: Kamera-Halt @0x01462, Piep/Aufschliessen bei Cut 9, Bild 230:
#      rot gemaltes Glas rot-dominant, Bild 300: gruen gemaltes Glas gruen-dominant.
#   D  ROOM10D0 FALSCHER CODE 9999 per Cursor: "Wrong code, try again." (msg 4), KEIN Kamera-Halt, KEIN Ton, nie OFFEN;
#      Bild 500: rot gemaltes Glas rot-dominant.
# Ton ohne Geraet: RE15_AUDIO_CAP_SYNC. Bildpruefung: probe_r37_codepanel ppm (Mittel ueber das gemalte Glas).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PROBE=<probe_r37_codepanel> -DWORKDIR=<dir> -P test_r37_codepanel.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_codepanel: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PROBE OR NOT EXISTS "${RE15_PROBE}")
    message(FATAL_ERROR "r37_codepanel: RE15_PROBE fehlt: '${RE15_PROBE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_codepanel: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r36_tueren.cmake)
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37e_haken_*.exe" "${_exe_dir}/re15_pc_r37e_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe_kopie "${_exe_dir}/re15_pc_r37e_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(e_lauf _name _endbild)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 400
        RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_AUDIO_CAP_SYNC=cap.raw RE15_AUFSCHLIESS_LOG=auf.log RE15_CODEPANEL_LOG=cp.log RE15_CUT_SYNC_LOG=cut.log
        RE15_MSG_LOG=1
        ${ARGN}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r37_codepanel[${_name}]: debug.log fehlt (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "EXIT_AT: Bild ${_endbild}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_codepanel[${_name}]: Endbild ${_endbild} nicht erreicht (exit=${_rv}, ${WORKDIR})")
    endif()
    file(REMOVE "${WORKDIR}/cap.raw")
    foreach(_f auf cp cut)
        set(_t "")
        if(EXISTS "${WORKDIR}/${_f}.log")
            file(READ "${WORKDIR}/${_f}.log" _t)
        endif()
        set(_${_f} "${_t}" PARENT_SCOPE)
    endforeach()
    set(_dbg "${_dbg}" PARENT_SCOPE)
    set(_wd "${WORKDIR}" PARENT_SCOPE)
endfunction()

function(e_muss _name _text _muster)
    string(REGEX MATCH "${_muster}" _m "${_text}")
    if(NOT _m)
        message(FATAL_ERROR "r37_codepanel[${_name}]: erwartet /${_muster}/ - fehlt")
    endif()
endfunction()
function(e_nicht _name _text _muster)
    string(REGEX MATCH "${_muster}" _m "${_text}")
    if(_m)
        message(FATAL_ERROR "r37_codepanel[${_name}]: /${_muster}/ darf nicht vorkommen: '${_m}'")
    endif()
endfunction()

# Mittel ueber eine Flaeche des Framedumps; _kanal (0 r, 1 g, 2 b) muss die beiden anderen um >= 30 uebertreffen.
function(e_dominant _name _ppm _x0 _y0 _x1 _y1 _kanal)
    if(NOT EXISTS "${_ppm}")
        message(FATAL_ERROR "r37_codepanel[${_name}]: Bild fehlt: ${_ppm}")
    endif()
    execute_process(COMMAND "${RE15_PROBE}" ppm "${_ppm}" ${_x0} ${_y0} ${_x1} ${_y1}
                    OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(REGEX MATCH "MITTEL ([0-9]+) ([0-9]+) ([0-9]+)" _m "${_o}")
    if(NOT _m)
        message(FATAL_ERROR "r37_codepanel[${_name}]: ppm-Werkzeug: ${_o}")
    endif()
    set(_c0 ${CMAKE_MATCH_1})
    set(_c1 ${CMAKE_MATCH_2})
    set(_c2 ${CMAKE_MATCH_3})
    set(_d ${_c${_kanal}})
    foreach(_k 0 1 2)
        if(NOT _k EQUAL _kanal)
            math(EXPR _ab "${_d} - ${_c${_k}}")
            if(_ab LESS 30)
                message(FATAL_ERROR "r37_codepanel[${_name}]: Flaeche ${_x0},${_y0}..${_x1},${_y1} Mittel (${_c0},${_c1},${_c2}) - Kanal ${_kanal} nicht dominant")
            endif()
        endif()
    endforeach()
    message(STATUS "r37_codepanel[${_name}]: ${_ppm} (${_x0},${_y0}..${_x1},${_y1}) Mittel (${_c0},${_c1},${_c2}) ok")
endfunction()

# Bild des Pieps (P) und des Aufschliessens aus auf.log; Kamera-Rueckkehr = erstes Bild > P mit anderem Cut in cut.log.
function(e_folge _name _raum _cut _auf _cut_log)
    string(REGEX MATCH "F([0-9]+) raum=${_raum} nachricht=5 art=KARTE_AUF satz=1\\(PIEP\\) bild=\\+0" _m "${_auf}")
    if(NOT _m)
        message(FATAL_ERROR "r37_codepanel[${_name}]: Piep fehlt (auf.log: ${_auf})")
    endif()
    set(_p ${CMAKE_MATCH_1})
    math(EXPR _p15 "${_p} + 15")
    e_muss(${_name} "${_auf}" "F${_p15} raum=${_raum} nachricht=5 art=KARTE_AUF satz=2\\(AUF_E\\) bild=\\+15")
    string(TOLOWER "${_raum}" _rl)
    e_muss(${_name} "${_cut_log}" "F${_p15} room=${_rl} bg=${_rl}#${_cut} ")
    string(REGEX MATCHALL "F[0-9]+ room=${_rl} bg=${_rl}#[0-9]+ " _zeilen "${_cut_log}")
    set(_zurueck -1)
    foreach(_z IN LISTS _zeilen)
        string(REGEX MATCH "F([0-9]+) room=${_rl} bg=${_rl}#([0-9]+) " _mm "${_z}")
        if(CMAKE_MATCH_1 GREATER ${_p} AND NOT CMAKE_MATCH_2 EQUAL ${_cut} AND _zurueck EQUAL -1)
            set(_zurueck ${CMAKE_MATCH_1})
        endif()
    endforeach()
    if(_zurueck EQUAL -1 OR NOT _zurueck GREATER ${_p15})
        message(FATAL_ERROR "r37_codepanel[${_name}]: Kamera zurueck in Bild ${_zurueck}, Aufschliessen ${_p15} - verlaesst das Panel zu frueh")
    endif()
    message(STATUS "r37_codepanel[${_name}]: Piep F${_p}, Aufschliessen F${_p15} bei Cut ${_cut}, Kamera zurueck F${_zurueck}")
endfunction()

# --- B: ROOM10D0 echter Weg ------------------------------------------------------------------------------
# Bildplan (gemessen, analysis/befunde_runde37/E_codepanel.md §4): Viereck alle 10 Bilder 160..480 (msg 7 + Ja,
# msg 9, msg 0; der Cursor steht dabei zwischen den Tasten), dann je Ziffer Cursor (200 Einheiten je Bild, sub02..05)
# + Viereck und 63 Bilder spaeter Viereck (Ziffern-Nachricht zu).
set(_pad "")
foreach(_f RANGE 160 480 10)
    list(APPEND _pad "${_f}:A")
endforeach()
macro(_seg _a _n _k)
    math(EXPR _e "${_a} + ${_n} - 1")
    foreach(_f RANGE ${_a} ${_e})
        list(APPEND _pad "${_f}:${_k}")
    endforeach()
endmacro()
_seg(490 20 L)
_seg(510 4 D)
list(APPEND _pad "516:A" "579:A")                      # "4" (Platz 6), msg 1 zu
_seg(589 26 R)
_seg(615 12 U)
list(APPEND _pad "629:A" "692:A")                      # "3" (Platz 5), msg 2 zu
_seg(702 26 L)
list(APPEND _pad "730:A" "793:A")                      # "1" (Platz 3), msg 3 zu
_seg(803 13 R)
list(APPEND _pad "818:A" "908:A" "960:A" "1010:A")     # "2" (Platz 4); msg 5 zu; Wegwerf-Abfrage Ja; Meldung zu
list(JOIN _pad "," _pad_s)
e_lauf(b 1060 RE15_DEBUG_JUMP=10D0@120 RE15_GIVE=0x38:1 RE15_SET_FLAG=9:52 "RE15_FIRE_AOT=1@150#10D0"
       "RE15_PAD_AT=${_pad_s}" "RE15_EXIT_AT=1060#10D0" "RE15_FRAMEDUMP=700-900/200:fd_")
e_muss(b "${_dbg}" "\\[msgtext\\] room=10d0 id=9 \"You've used the Blue Keycard.\"")
e_muss(b "${_dbg}" "\\[msgtext\\] room=10d0 id=3 \"Enter the fourth number.\"")
e_nicht(b "${_dbg}" "\\[msgtext\\] room=10d0 id=4 ")
e_muss(b "${_cp}" "raum=10D0 lampe ZU: Zeile 0 \\(0 rot, 1 blau, 2 gruen\\) Cut 11")
e_muss(b "${_cp}" "raum=10D0 kamera halten: sub19 @0x0199A")
e_muss(b "${_cp}" "raum=10D0 lampe OFFEN: Zeile 1 \\(0 rot, 1 blau, 2 gruen\\) Cut 11")
e_muss(b "${_cp}" "raum=10D0 kamera zurueck: Toene 2/2")
e_folge(b 10D0 11 "${_auf}" "${_cut}")
# rot gemaltes Glas x 116..127 / y 72..81, blau gemaltes Glas x 164..173 / y 72..81 (ROOM10D11, Dossier §0):
# zu (Bild 700, zwischen den Ziffern) leuchtet die ROTE rot, offen (Bild 900, Nachricht 5 auf dem Panel) die BLAUE blau.
e_dominant(b_zu_rot    "${_wd}/fd_000700.ppm" 116 72 127 81 0)
e_dominant(b_offen_blau "${_wd}/fd_000900.ppm" 164 72 173 81 2)

# --- C: ROOM1230 SECUGUARD -------------------------------------------------------------------------------
e_lauf(c 340 RE15_DEBUG_JUMP=1230@120 "RE15_SUBSTART=17@150#1230" "RE15_PAD_AT=210:A,320:A"
       "RE15_SET_FLAG_AT=5:13,5:14,5:15,5:16@240" "RE15_EXIT_AT=340#1230" "RE15_FRAMEDUMP=230-300/70:fd_")
e_muss(c "${_cp}" "raum=1230 lampe ZU: Zeile 0 \\(0 rot, 1 blau, 2 gruen\\) Cut 9")
e_muss(c "${_cp}" "raum=1230 kamera halten: sub19 @0x01462")
e_muss(c "${_cp}" "raum=1230 lampe OFFEN: Zeile 2 \\(0 rot, 1 blau, 2 gruen\\) Cut 9")
e_muss(c "${_cp}" "raum=1230 kamera zurueck: Toene 2/2")
e_folge(c 1230 9 "${_auf}" "${_cut}")
# rot gemaltes Glas x 122..135, gruen gemaltes x 174..187, y 166..183 (ROOM12309): zu rot, offen gruen
e_dominant(c_zu_rot     "${_wd}/fd_000230.ppm" 122 166 135 183 0)
e_dominant(c_offen_gruen "${_wd}/fd_000300.ppm" 174 166 187 183 1)

# --- D: ROOM10D0 falscher Code 9999 ----------------------------------------------------------------------
set(_pad "210:A")
_seg(220 7 R)
_seg(227 17 D)
list(APPEND _pad "246:A" "309:A" "320:A" "383:A" "394:A" "457:A" "468:A" "531:A")
list(JOIN _pad "," _pad_s)
e_lauf(d 560 RE15_DEBUG_JUMP=10D0@120 "RE15_SUBSTART=17@150#10D0" "RE15_PAD_AT=${_pad_s}" "RE15_EXIT_AT=560#10D0"
       "RE15_FRAMEDUMP=500:fd_000500.ppm")
e_muss(d "${_dbg}" "\\[msgtext\\] room=10d0 id=4 \"Wrong code, try again.\"")
e_nicht(d "${_cp}" "kamera halten")
e_nicht(d "${_cp}" "lampe OFFEN")
e_nicht(d "${_auf}" "PIEP")
e_muss(d "${_cut}" "F5[0-5][0-9] room=10d0 bg=10d0#0 ")
e_dominant(d_zu_rot "${_wd}/fd_000500.ppm" 116 72 127 81 0)

file(REMOVE "${_exe_kopie}")
message(STATUS "r37_codepanel: B (10D0 echter Weg 4312), C (1230 SECUGUARD), D (10D0 falscher Code) bestanden")
