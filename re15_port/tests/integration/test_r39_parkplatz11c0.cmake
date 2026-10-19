cmake_minimum_required(VERSION 3.16)
# =============================================================================
# RUNDE 39 SPUR O "parkplatz11c0" MIT DER ECHTEN EXE. Dossier analysis/befunde_runde39/O_parkplatz11c0.md, Konstanten
# include/re15_parkplatz_r39.h. Die Unit-Riegel unit_r39_parkplatz11c0_* messen die Mechanik am Engine-Code; dieser
# Riegel prueft den Nutzerweg an der exe (Speicherkarte + CONTINUE in ROOM11B0, danach NUR echte Tueren per RE15_WEGPLAN):
#
#   K  LEON 11B0 -> Parkplatz-Tuer -> 11C0: Ada-Szene (msg 0..6) laeuft, Programm SZENE sperrt die Tuer; im Kampf (Gorillas
#      leben, per RE15_R39O_GORILLAS_STARR eingefroren, damit Leon zu Fuss lebend ankommt — Lauf vor1 im Dossier) zur Tuer:
#      Text msg 40, KEIN Raumwechsel, ohne Stimme (Nachtrag 13); danach laeuft Leon weiter
#      (steuerbar); Kampfende (RE15_R39O_GORILLAS_TOT: genau die Kill-Bits (7,0x60)/(7,0x61) des Gorilla-Todes) ->
#      Programm ENDE, msg 7, die Ende-Szene geht selbst durch die Tuer -> ROOM11B0. Bilder per RE15_FRAMEDUMP.
#   W  LEON Wiedereintritt im Kampfzustand ((4,64)=1, (3,67)=0 — z. B. ein Spielstand von vor dieser Runde, als die Tuer im
#      Kampf noch ging): Tuer Slot 0 ist ab dem Eintritt der Text; Druck -> msg 40, kein Raumwechsel.
#   N  LEON nach dem Kampf ((4,64)=1, (3,67)=1): Tuer ist die Original-Tuer, Druck -> ROOM11B0, kein Eingriff.
#   E  ELZA in 11C1 (Hunde, kein Begleiter): Tuer bleibt Tuer, Druck -> zurueck nach ROOM11B1 (die fuenf Hunde nimmt
#      WEGPLAN GEGNERWEG heraus, damit kein Biss den Druck schluckt — die Tuer haengt an keinem Gegner). Start per Spielstand
#      IN 11C1: die Gegenrichtung 11B1 -> 11C1 ist seit Runde 39 Nachtrag 12c fuer Elza zu (tuersperre_r39.c).
#   I  LEON (4,64)=0, (3,67)=1 (Karte des Spur-A-Riegels integration_r39_stimmen_11b0; Abnahme 0 M1): keine Sperre -
#      die Freigabe sub01 @0x0182E Ck(3,67)==0 griffe nie. i1 Aot_on(0) in Bild 60 der Szene -> ROOM11B0; i2 Szene
#      durch, zu Fuss zur Tuer, Druck -> ROOM11B0.
# Geprueft werden debug.log, state.txt und wegplan_log.txt.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_schluessel_karte> -DWORKDIR=<dir> -DTEIL=K|W|N|E|I
#         -P test_r39_parkplatz11c0.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r39_parkplatz11c0: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r39_parkplatz11c0: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_parkplatz11c0: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL "K")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# EIGENER EXE-NAME neben der exe (Asset-Wurzel = exe-Verzeichnis), Praefix NUR dieses Teils (Muster r38_dining1141).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r39o_${TEIL}_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r39o_${TEIL}_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# keine Umgebung des Aufrufers erben
foreach(_v RE15_AI_FLAVOR RE15_FRAMEDUMP RE15_INPUT_SCRIPT RE15_INPUT_SCRIPT_START RE15_PLAYER_POS RE15_DEBUG_JUMP
           RE15_SET_FLAG RE15_SET_FLAG_AT RE15_R39O_GORILLAS_STARR RE15_R39O_GORILLAS_TOT RE15_R38F_GORILLAS_TOT
           RE15_R38F_GORILLAS_STARR RE15_NOAUDIO RE15_PSELECT_AUTO RE15_PSELECT_AUTO_SWITCH RE15_EXIT_AT)
    unset(ENV{${_v}})
endforeach()

set(_fehler "")
macro(pruef _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(_p LESS 0)
        list(APPEND _fehler "${_name}: fehlt '${_muster}'")
        message(STATUS "FAIL ${_name}: fehlt '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: '${_muster}'")
    endif()
endmacro()
macro(pruef_nicht _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(NOT _p LESS 0)
        list(APPEND _fehler "${_name}: unerwartet '${_muster}'")
        message(STATUS "FAIL ${_name}: unerwartet '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: nicht da '${_muster}'")
    endif()
endmacro()
macro(pruef_wahr _name _bed _text)
    if(${_bed})
        message(STATUS "ok   ${_name}: ${_text}")
    else()
        list(APPEND _fehler "${_name}: ${_text}")
        message(STATUS "FAIL ${_name}: ${_text}")
    endif()
endmacro()

# Ein Lauf: Karte schreiben (probe_r36_schluessel_karte), Wegplan schreiben, exe starten, Dateien lesen.
function(lauf _name _karte _plan _extra _dbg_out _st_out _wp_out)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_karte}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r39_parkplatz11c0[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    file(WRITE "${WORKDIR}/wegplan.txt" "${_plan}")
    re15_start_spiel(_rv 1500
        RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_AUDIO_CAP_SYNC=cap.raw RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_MSG_LOG=1 RE15_EVT_TRACE=1 RE15_STATE_LOG=state.txt
        "RE15_WEGPLAN=${WORKDIR}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt
        ${_extra}
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r39_parkplatz11c0[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r39_parkplatz11c0[${_name}]: Endbild nicht erreicht - Lauf haengt oder riss ab (exit=${_rv}, ${WORKDIR})")
    endif()
    set(_st "")
    if(EXISTS "${WORKDIR}/state.txt")
        file(STRINGS "${WORKDIR}/state.txt" _st_zeilen REGEX "msg\\(a=1 fsm=1 id=40 ")
        list(LENGTH _st_zeilen _n40)
        set(_st "bilder_mit_msg40=${_n40}")
    endif()
    set(_wp "")
    if(EXISTS "${WORKDIR}/wegplan_log.txt")
        file(READ "${WORKDIR}/wegplan_log.txt" _wp)
    endif()
    message(STATUS "r39_parkplatz11c0[${_name}]: exit=${_rv}, ${WORKDIR}")
    set(${_dbg_out} "${_dbg}" PARENT_SCOPE)
    set(${_st_out} "${_st}" PARENT_SCOPE)
    set(${_wp_out} "${_wp}" PARENT_SCOPE)
endfunction()

# Vor der Parkplatz-Tuer in ROOM11B0 (Rechteck x -26730..-24230, z -29940..-26440), Blick -z: Strom (4,0xF3), Sherry-
# Szene erledigt (3,0x82), Tuer aufgeschlossen (9,93) — Muster integration_r36_schluessel. Spawn in 11C0 = ROOM11B0
# main00 @0x00FAA (-25279,0,17268) Cut 0 (im Tuer-Rechteck von 11C0 main00 @0x01712).
set(_karte_11b0 11B0 -25480 -26000 1024 flag:4:0xF3 flag:3:0x82 flag:9:93)

# --- K: Kampf am echten Weg ------------------------------------------------------------------------------------
# Weg zurueck zur Tuer ueber die Plc_dest-Punkte der Szene (sub02 @0x0185E..@0x0187C): (-15893,-6270), (-15893,12430);
# vor der Tuer (-25100,16600) Blick +Z (Vorwaerts-Punkt im Rechteck). Drei Drucke: Text auf, weiter, zu (gemessen
# Lauf nach1: msg 40 offen F1742..F1867). Dann weg von der Tuer (-20000,12000) = steuerbar. Kampfende nach 1100 freien
# Kampfbildern (Messhaken; gemessen: Text F1741, Weg fertig F2050, Tod F2207).
if(TEIL STREQUAL "K")
    lauf(k "${_karte_11b0}" "RAUM 11B0
WARTE 20
AKTION
RAUM 11C0
AOTS
FLAG 4 64
FLAG 3 67
GEHE -15893 -6270 300
GEHE -15893 12430 300
GEHE -25100 16600 150
BLICK 3072
AOTS
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AOTS
GEHE -20000 12000 300
WARTE 400
RAUM 11B0
WARTE 30
ENDE
" "RE15_R39O_GORILLAS_STARR=1;RE15_R39O_GORILLAS_TOT=1100;RE15_EXIT_AT=200#11b0;RE15_FRAMEDUMP=1745-1865/30:fd_" _d _st _wp)
    pruef(K _d "CONTINUE: resumed in room 11b0")
    pruef(K _d "DOOR FIRE slot=1 rect=(-25480,-28190")
    pruef(K _d "loaded room11c0.rdt")
    pruef(K _d "ROOM11C0 sub01 @0x01824 Evt_exec sub02 -> Programm SZENE")
    pruef(K _d "[msg] room=11c0 id=5 ")
    pruef(K _d "[msg] room=11c0 id=6 ")
    pruef(K _d "letterbox closed -> gameplay")
    pruef(K _wp "AOT slot 0 typ 5 ev 40 flags 0x31 band 0 mitte (-25100,17250)")
    pruef_nicht(K _wp "AOT slot 0 typ 1 ")
    pruef(K _wp "FLAG (4,64) = 1")
    pruef(K _wp "FLAG (3,67) = 0")
    pruef(K _d "msg 40")
    pruef_nicht(K _d "[voice] ROOM11C0 msg 40:")              # Nachtrag 13: einfacher Raumtext ohne Stimme
    if(_st MATCHES "bilder_mit_msg40=([0-9]+)")
        set(_n "${CMAKE_MATCH_1}")
        set(_b FALSE)
        if(_n GREATER 20)
            set(_b TRUE)
        endif()
        pruef_wahr(K _b "Text msg 40 stand ${_n} Bilder offen (Zustands-Log msg(a=1 fsm=1 id=40))")
    endif()
    # Waehrend des Kampfes kein Durchgang: die einzige DOOR FIRE slot=0 kommt NACH dem Kampfende.
    string(FIND "${_d}" "MESSHAKEN RE15_R39O_GORILLAS_TOT" _p_tot)
    string(FIND "${_d}" "DOOR FIRE slot=0" _p_tuer)
    string(FIND "${_d}" "[msgtext] room=11c0 id=40 " _p_text)   # der Text selbst (RE15_MSG_LOG; Nachtrag 13: kein Stimmen-Log mehr)
    set(_b FALSE)
    if(_p_tot GREATER 0 AND _p_tuer GREATER _p_tot AND _p_text GREATER 0 AND _p_text LESS _p_tot)
        set(_b TRUE)
    endif()
    pruef_wahr(K _b "Text im Kampf (Pos ${_p_text}) vor dem Kampfende (Pos ${_p_tot}), Tuer erst danach (Pos ${_p_tuer})")
    string(FIND "${_wp}" "Befehl 19 beendet: angekommen" _p_weg)
    set(_b FALSE)
    if(NOT _p_weg LESS 0)
        set(_b TRUE)
    endif()
    pruef_wahr(K _b "nach dem Text laeuft Leon von der Tuer weg (GEHE (-20000,12000) angekommen)")
    pruef(K _d "ROOM11C0 sub01 @0x01842 Evt_exec sub03 -> Programm ENDE")
    pruef(K _d "[msg] room=11c0 id=7 ")
    pruef(K _d "DOOR FIRE slot=0 rect=(-25100,17250,hw=2000,hh=1350) target_cut=1 spawn=(-25552,0,-25331)")
    pruef(K _d "loaded room11b0.rdt")
    file(GLOB _bilder "${_basis}_k/fd_*")
    list(LENGTH _bilder _nb)
    set(_b FALSE)
    if(_nb GREATER 0)
        set(_b TRUE)
    endif()
    pruef_wahr(K _b "${_nb} Framedumps des offenen Textes (fd_*)")
endif()

# --- W: Wiedereintritt im Kampfzustand ------------------------------------------------------------------------------
# Spawn (-25279,17268) liegt im Tuer-Rechteck; Blick +Z -> Vorwaerts-Punkt (-25279,17888) im Rechteck (z <= 18600).
if(TEIL STREQUAL "W")
    lauf(w "${_karte_11b0};flag:4:64" "RAUM 11B0
WARTE 20
AKTION
RAUM 11C0
AOTS
FLAG 4 64
FLAG 3 67
BLICK 3072
AKTION
WARTE 60
AKTION
WARTE 60
AKTION
WARTE 60
AOTS
ENDE
" "RE15_R39O_GORILLAS_STARR=1;RE15_EXIT_AT=420" _d _st _wp)
    pruef(W _d "loaded room11c0.rdt")
    pruef(W _d "ROOM11C0: Kampf laeuft beim Eintritt ((4,64)=1, (3,67)=0) -> Tuer Slot 0 = Text msg 40")
    pruef_nicht(W _d "Programm SZENE")
    pruef(W _wp "AOT slot 0 typ 5 ev 40 flags 0x31")
    pruef_nicht(W _wp "AOT slot 0 typ 1 ")
    pruef(W _d "[msgtext] room=11c0 id=40 ")
    pruef_nicht(W _d "DOOR FIRE slot=0")
    pruef(W _d "EXIT_AT: Bild 420 in Raum 11c0")
endif()

# --- N: nach dem Kampf ((4,64)=1, (3,67)=1): die Tuer ist wieder die Original-Tuer ----------------------------------
if(TEIL STREQUAL "N")
    lauf(n "${_karte_11b0};flag:4:64;flag:3:67" "RAUM 11B0
WARTE 20
AKTION
RAUM 11C0
AOTS
FLAG 3 67
BLICK 3072
AKTION
RAUM 11B0
WARTE 30
ENDE
" "RE15_EXIT_AT=200#11b0" _d _st _wp)
    pruef(N _d "loaded room11c0.rdt")
    pruef(N _wp "AOT slot 0 typ 1 ev 0 flags 0x31 band 0 mitte (-25100,17250)")
    pruef(N _wp "FLAG (3,67) = 1")
    pruef_nicht(N _d "[parkplatz]")
    pruef(N _d "DOOR FIRE slot=0 rect=(-25100,17250,hw=2000,hh=1350) target_cut=1 spawn=(-25552,0,-25331)")
    pruef(N _d "EXIT_AT: Bild 200 in Raum 11b0")
endif()

# --- E: Elza ROOM11C1 -> 11B1 (Ankunftslage aus ROOM11B1 main00 @0x00F9C: (-25279,17268), Blick 0x11FB & 0xFFF) --------
if(TEIL STREQUAL "E")
    lauf(e "11C1;-25279;17268;507;elza;flag:4:0xF3;flag:4:64" "RAUM 11C1
AOTS
GEGNERWEG
BLICK 3072
AKTION
RAUM 11B1
WARTE 30
ENDE
" "RE15_EXIT_AT=200#11b1" _d _st _wp)
    pruef(E _wp "AOT slot 0 typ 1 ev 0 flags 0x31 band 0 mitte (-25100,17250)")
    pruef_nicht(E _d "[parkplatz]")
    pruef(E _d "DOOR FIRE slot=0 rect=(-25100,17250")
    pruef(E _d "loaded room11b1.rdt")
    pruef(E _d "EXIT_AT: Bild 200 in Raum 11b1")
endif()

# --- I: (4,64)=0, (3,67)=1 (Abnahme 0 M1) ----------------------------------------------------------------------------
# Flag-Zustand der Karte von integration_r39_stimmen_11b0 (Spur A, test_r39_stimmen.cmake:97 `3:0x43 3:0x82 4:0xf3`, Lage
# (-25100,15000) Blick 1024): die Ende-Szene gilt als gelaufen, die Ada-Szene nicht. sub01 @0x01824 startet sub02 wie im
# Original; die einzige Freigabe sub01 @0x0182E Ck(3,67)==0 -> @0x01842 griffe nie -> KEINE Sperre (parkplatz_r39.c).
#   i1  Weg des Spur-A-Riegels: Aot_on(0) in Bild 60 der Szene (RE15_FIRE_AOT=0@60#11c0) -> Tuer -> ROOM11B0
#   i2  Szene bis zur Rueckgabe der Steuerung, Gegner weg, zu Fuss zur Tuer (Wege wie K), Druck -> Tuer -> ROOM11B0.
#       Am CONTINUE-Weg kuerzt der vorbestehende Boot-Haken "door-3 self-reentry FALLBACK" (platform/pc/main.c, Frame-
#       Cap Bild 900; nicht Spur O) die Szene: er laeuft main00+sub00 neu, mit (4,64)=1/(3,67)=1 nimmt sub00 den
#       Nach-Kampf-Zweig (@0x017C8/@0x017D0, keine Gegner). Die Pruefungen gelten mit und ohne diesen Haken.
if(TEIL STREQUAL "I")
    set(_karte_i 11C0 -25100 15000 1024 flag:3:67 flag:3:0x82 flag:4:0xF3)
    lauf(i1 "${_karte_i}" "RAUM 11B0
WARTE 30
ENDE
" "RE15_FIRE_AOT=0@60#11c0;RE15_EXIT_AT=200#11b0" _d _st _wp)
    pruef(I1 _d "CONTINUE: resumed in room 11c0")
    pruef(I1 _d "ROOM11C0 sub01 @0x01824 Evt_exec sub02 bei (3,67)=1 -> keine Sperre")
    pruef(I1 _d "room=11c0 Evt_exec sub=2 ")
    pruef_nicht(I1 _d "Programm SZENE")
    pruef(I1 _d "[fire-aot] slot=0 at F60")
    pruef(I1 _d "DOOR FIRE slot=0 rect=(-25100,17250,hw=2000,hh=1350) target_cut=1 spawn=(-25552,0,-25331)")
    pruef(I1 _d "loaded room11b0.rdt")
    pruef_nicht(I1 _d "[msgtext] room=11c0 id=40 ")
    pruef(I1 _d "EXIT_AT: Bild 200 in Raum 11b0")
    lauf(i2 "${_karte_i}" "RAUM 11C0
AOTS
FLAG 4 64
FLAG 3 67
GEGNERWEG
GEHE -15893 -6270 300
GEHE -15893 12430 300
GEHE -25100 16600 150
BLICK 3072
AOTS
AKTION
RAUM 11B0
WARTE 30
ENDE
" "RE15_EXIT_AT=200#11b0" _d _st _wp)
    pruef(I2 _d "ROOM11C0 sub01 @0x01824 Evt_exec sub02 bei (3,67)=1 -> keine Sperre")
    pruef_nicht(I2 _d "Programm SZENE")
    pruef(I2 _d "letterbox closed -> gameplay")
    pruef(I2 _wp "AOT slot 0 typ 1 ev 0 flags 0x31 band 0 mitte (-25100,17250)")
    pruef_nicht(I2 _wp "AOT slot 0 typ 5 ")
    pruef(I2 _wp "FLAG (4,64) = 1")
    pruef(I2 _wp "FLAG (3,67) = 1")
    pruef(I2 _d "DOOR FIRE slot=0 rect=(-25100,17250,hw=2000,hh=1350) target_cut=1 spawn=(-25552,0,-25331)")
    pruef(I2 _d "loaded room11b0.rdt")
    pruef_nicht(I2 _d "[msgtext] room=11c0 id=40 ")
    pruef(I2 _d "EXIT_AT: Bild 200 in Raum 11b0")
endif()

file(REMOVE "${_exe_kopie}")
if(_fehler)
    string(REPLACE ";" "\n  " _f "${_fehler}")
    message(FATAL_ERROR "r39_parkplatz11c0[${TEIL}]: ${_f}")
endif()
message(STATUS "r39_parkplatz11c0[${TEIL}]: alle Pruefungen gruen (${_basis})")
