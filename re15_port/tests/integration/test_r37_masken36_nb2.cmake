cmake_minimum_required(VERSION 3.16)
# =============================================================================
# SPUR P "masken36", NACHBESSERUNG 2 (Runde 37, Abnahme 1 M1/M2) MIT DER ECHTEN EXE.
#
# Dossier analysis/befunde_runde37/P_masken36.md, Abschnitt "Nachbesserung 2" (NB2.1, NB2.2).
#
# M1 - ROOM3050/3051 Cut 15 (Monitor-Nahaufnahme der Kran-Konsole, sub10 +0x2596 Cut_chg 15): bis Nachbesserung 1
#      uebermalten die 36 Generator-Masken den dunkelgruenen Innenrand des Ziel-Cursors (Objekt 3; Abnahme 1: 60 Punkte,
#      Leon und Elza). Das Original zeichnet dort KEINE Maske (Kamera-Eintrag +0x25C -> +0x84C = FF FF FF FF,
#      FUN_800392d4 @0x80039324 lw / @0x8003932c bne -1 / @0x80039338 sb zero). Jetzt sind alle 36 Rechtecke aus.
#      Aufbau der Abnahme + RE15_GIVE=0x46:1: seit master v0.8.25 (Runde 37 Spur Q, kistenregel_r37.c K8) setzt
#      ROOM3050 main00 @0x01F90 Ck(3,0xB2) das Cursor-Objekt nur mit der Red Master Keycard (Item 0x46) im Inventar.
#        K_A  mit Masken, Karte          K_B  ohne Masken (RE15_NO_PRI=1), Karte
#        K_C  ohne Masken, OHNE Karte = dasselbe Bild ohne Cursor
#      probe_r37_masken36_ppm(K_A, K_B, K_C) im ganzen Bild: S = Cursor-Punkte (|B - C| > 24), H = davon uebermalt.
#      Verlangt: Log "Cut_chg(15)" + "[pri] cut=15 pri_offset=0x84C masks=0", S > 30 (der Cursor ist WIRKLICH im Bild -
#      ohne Karte waere "A == B" leer wahr), H == 0, und A/B im ganzen Bild ohne einen Punkt > 24 (probe(A, B, A)).
#      Leon (ROOM3050) und Elza (RE15_TITLE_SHOT_CHAR=1, ROOM3051).
# M2 - Kopfoberkante: der Zufallsweg der Abnahme 1 in ROOM30A0 Cut 0 (Leon VOR der Buero-Trennwand); bis Nachbesserung 1
#      schnitt der obere Rahmen-Balken (Maske 20) in Bild 380/400 4 bzw. 7 Punkte vom Haar ab, weil das Figurmodell die
#      Kopfoberkante (2833..3000 von 3000) nie abtastete. Jetzt ist das Raster pixelgenau.
#        W_A mit Masken, W_B ohne Masken (gleicher Weg, Laeufe deterministisch), W_C Zwangs-Cut 0 ohne Figur.
#      Verlangt: Bild 380 und 400 je S > 50 und H == 0.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PPM_TOOL=<probe_r37_masken36_ppm> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_masken36_nb2: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PPM_TOOL OR NOT EXISTS "${RE15_PPM_TOOL}")
    message(FATAL_ERROR "r37_masken36_nb2: RE15_PPM_TOOL fehlt: '${RE15_PPM_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_masken36_nb2: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; local_build.sh killt re15_pc.exe). Endung wie die Quelle.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37pnb2_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37pnb2_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(_basis_wd "${WORKDIR}")

# lauf(<name> <ziel_raum> <framedump> <exit_bild> ENV...)
function(lauf _name _zr _fd _eb)
    set(WORKDIR "${_basis_wd}/lauf_${_lauf_id}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_AUDIODRIVER=dummy
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        "RE15_FRAMEDUMP=${_fd}" "RE15_EXIT_AT=${_eb}#${_zr}"
        ${ARGN}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r37_masken36_nb2[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    set(_wd_${_name} "${WORKDIR}" PARENT_SCOPE)
endfunction()

# ---- M1: Cut 15 -------------------------------------------------------------------------------------------------------
set(_druck "")
foreach(_b 420 480 540 600 660 720 780 840 900 960)
    math(EXPR _b2 "${_b} + 2")
    list(APPEND _druck "square@${_b}" "square@${_b2}")
endforeach()
string(REPLACE ";" "," _druck "${_druck}")
set(_k15 "RE15_SET_FLAG=3:178" "RE15_DEBUG_JUMP=3050@250" "RE15_PLAYER_POS=-24757,-15900,3072" "RE15_FORCE_EVENT=10@300"
         "RE15_PRESS=${_druck}")
lauf(k_a 3050 900:f.ppm 900 ${_k15} "RE15_GIVE=0x46:1")
lauf(k_b 3050 900:f.ppm 900 ${_k15} "RE15_GIVE=0x46:1" RE15_NO_PRI=1)
lauf(k_c 3050 900:f.ppm 900 ${_k15} RE15_NO_PRI=1)
lauf(ek_a 3051 900:f.ppm 900 ${_k15} "RE15_GIVE=0x46:1" RE15_TITLE_SHOT_CHAR=1)
lauf(ek_b 3051 900:f.ppm 900 ${_k15} "RE15_GIVE=0x46:1" RE15_TITLE_SHOT_CHAR=1 RE15_NO_PRI=1)
lauf(ek_c 3051 900:f.ppm 900 ${_k15} RE15_TITLE_SHOT_CHAR=1 RE15_NO_PRI=1)

# ---- M2: Zufallsweg ROOM30A0 (Abnahme 1, Seed 37014) ------------------------------------------------------------------
set(_weg "RE15_DEBUG_JUMP=30A0@250" "RE15_INPUT_SCRIPT_START=520"
         "RE15_INPUT_SCRIPT=R0.69,U1.12,R0.23,U1.15,R0.19,U0.98,R0.44,U0.88,L0.14,U1.02,L0.16,U2.18,R0.67,U1.89")
lauf(w_a 30A0 380-400/20:f 400 ${_weg})
lauf(w_b 30A0 380-400/20:f 400 ${_weg} RE15_NO_PRI=1)
lauf(w_c 30A0 120:f.ppm 120 "RE15_DEBUG_JUMP=30A0@250" "RE15_PLAYER_POS=-19800,19763,0" "RE15_FORCE_CUT=0" RE15_NO_PRI=1)
file(REMOVE "${_exe_kopie}")

function(bild_da _datei)
    if(NOT EXISTS "${_datei}")
        message(FATAL_ERROR "r37_masken36_nb2: Framedump fehlt: ${_datei}")
    endif()
endfunction()
function(sh_messen _out_s _out_h _a _b _c)
    bild_da("${_a}")
    bild_da("${_b}")
    bild_da("${_c}")
    execute_process(COMMAND "${RE15_PPM_TOOL}" "${_a}" "${_b}" "${_c}" 0 0 320 240 24
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o TIMEOUT 60)
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r37_masken36_nb2: PPM-Werkzeug exit=${_rv}: ${_o}")
    endif()
    string(REGEX MATCH "S=([0-9]+) H=([0-9]+)" _m "${_o}")
    set(${_out_s} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    set(${_out_h} "${CMAKE_MATCH_2}" PARENT_SCOPE)
endfunction()

# M1 Log: Cut 15 gezeigt, keine Maske (die R15M-Sektion ist nach dem Ausschluss leer = Original-NULL-Sektion)
foreach(_n k_a ek_a)
    file(READ "${_wd_${_n}}/debug.log" _log)
    if(NOT _log MATCHES "Cut_chg\\(15\\)")
        message(FATAL_ERROR "r37_masken36_nb2[${_n}]: Cut 15 nicht erreicht (Aufbau der Abnahme 1 kommt nicht an)")
    endif()
    if(NOT _log MATCHES "Item 0x46 im Inventar")
        message(FATAL_ERROR "r37_masken36_nb2[${_n}]: Kistenregel K8 sieht die Red Master Keycard nicht - kein Cursor-Objekt")
    endif()
    string(REGEX MATCH "\\[pri\\] cut=15 pri_offset=0x84C masks=([0-9]+) fg_atlas=([0-9])" _m "${_log}")
    if(NOT _m OR NOT CMAKE_MATCH_1 EQUAL 0)
        message(FATAL_ERROR "r37_masken36_nb2[${_n}]: Cut 15 zeichnet Masken: '${_m}' (Soll masks=0 wie die Original-NULL-Sektion)")
    endif()
    message(STATUS "r37_masken36_nb2[${_n}]: ${_m}")
endforeach()
sh_messen(_ks _kh "${_wd_k_a}/f.ppm" "${_wd_k_b}/f.ppm" "${_wd_k_c}/f.ppm")
sh_messen(_kd _kd2 "${_wd_k_a}/f.ppm" "${_wd_k_b}/f.ppm" "${_wd_k_a}/f.ppm")
sh_messen(_eks _ekh "${_wd_ek_a}/f.ppm" "${_wd_ek_b}/f.ppm" "${_wd_ek_c}/f.ppm")
sh_messen(_ekd _ekd2 "${_wd_ek_a}/f.ppm" "${_wd_ek_b}/f.ppm" "${_wd_ek_a}/f.ppm")
message(STATUS "r37_masken36_nb2 M1 Cut 15: Leon Cursor-Punkte ${_ks}, uebermalt ${_kh}, A/B verschieden ${_kd} | "
               "Elza Cursor-Punkte ${_eks}, uebermalt ${_ekh}, A/B verschieden ${_ekd} (Nachbesserung 1: 60 von 62 Innenrand)")
if(NOT _ks GREATER 30 OR NOT _eks GREATER 30)
    message(FATAL_ERROR "r37_masken36_nb2: M1 - kein Cursor im Bild (Leon ${_ks}, Elza ${_eks}) - Vergleich waere leer")
endif()
if(NOT _kh EQUAL 0 OR NOT _ekh EQUAL 0 OR NOT _kd EQUAL 0 OR NOT _ekd EQUAL 0)
    message(FATAL_ERROR "r37_masken36_nb2: M1 - Masken uebermalen in Cut 15 (Cursor Leon ${_kh} / Elza ${_ekh}, Bild ${_kd} / ${_ekd})")
endif()

# M2 Log + Zaehlung
file(READ "${_wd_w_a}/debug.log" _wl)
if(NOT _wl MATCHES "\\[pri\\] cut=0 pri_offset=0x3C0 masks=[1-9][0-9]* fg_atlas=1")
    message(FATAL_ERROR "r37_masken36_nb2[w_a]: ROOM30A0 Cut 0 ohne gezeichnete Masken")
endif()
if(NOT _wl MATCHES "\\[walk\\] F390 cut=0 ")
    message(FATAL_ERROR "r37_masken36_nb2[w_a]: der Zufallsweg steht in Bild 390 nicht in Cut 0")
endif()
foreach(_bild 380 400)
    sh_messen(_ws _wh "${_wd_w_a}/f000${_bild}.ppm" "${_wd_w_b}/f000${_bild}.ppm" "${_wd_w_c}/f.ppm")
    message(STATUS "r37_masken36_nb2 M2 ROOM30A0 Cut 0 Bild ${_bild}: Figurpunkte ${_ws}, uebermalt ${_wh} "
                   "(Nachbesserung 1: 380 -> 4 von 328, 400 -> 7 von 355)")
    if(NOT _ws GREATER 50)
        message(FATAL_ERROR "r37_masken36_nb2: M2 - Leon in Bild ${_bild} nicht zu sehen (${_ws})")
    endif()
    if(NOT _wh EQUAL 0)
        message(FATAL_ERROR "r37_masken36_nb2: M2 - Bild ${_bild}: ${_wh} Figurpunkte VOR der Trennwand uebermalt")
    endif()
endforeach()
message(STATUS "r37_masken36_nb2: OK")
