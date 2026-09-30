# =============================================================================
# LEUCHTSCHRIFT-RIEGEL ROOM1150/1151 (Runde 34 Nacht, Spur G2, Dossier G2_schrift1150.md §9).
#
# Nutzer: "in Irons Office room 1150 blinkt die Schrift eigentlich im Hintergrund."
# Original (PSX.EXE, Dossier §3): sub05 schaltet mit Opcode 0x45 (Tabelle @0x800745bc ->
# 0x800428d4 -> FUN_800396a8) die sprite.pri-Gruppen 6..11 von Cut 2 im Wechsel aus/an, je Sleep 20
# (ROOM1150 @0x010C8/@0x010DE); FUN_80039590 zeichnet nur Records mit Byte0 & 1 (@0x800395f0-f4);
# jeder Cut-Apply baut alle Masken neu an (FUN_800392d4 @0x80021c28).
#
# DER RIEGEL faehrt die ECHTE exe am Lade-Weg (CONTINUE, Karte probe_r34n_g_karte mit camera_cut 2),
# OHNE RE15_FORCE_CUT (setzt main.c pc_cam_present_apply JEDES Bild pending -> Aufbau jedes Bild ->
# Blinken unsichtbar, Gegenpruefung Auflage 4), beschleunigter Renderer, RE15_FRAMEDUMP:
#   R  ROOM1150, RE15_NO_PRI=1, EIN Bild = Hintergrund pur = AUS-Referenz im Schrift-Rechteck
#   A  ROOM1150, Bilder F100..F200 + RE15_MG_LOG
#   B  ROOM1151 (Elza, gleiche BSS ROOM115.BSS, gleiche sprite.pri @0x0066C), wie A
# probe_r34n_g_schrift_eval verlangt je Lauf: jedes Bild zeigt den Zustand, den Aufbau/Opcode 0x45
# im Log vorgeben (AN = Buchstaben-Masken, AUS = Referenz), alle AN-Bilder bitgleich, mindestens drei
# vollstaendige Laeufe, jeder genau 20 Bilder.
#
# ER DISKRIMINIERT (Mutationsproben Dossier §9.3): Filter in render_pc.c entfernt -> alle Bilder AN
# -> rot; Aufbau-Haken in main.c entfernt -> kein "Aufbau Cut 2" im Log, Zahl 0 -> rot; Opcode-Zeile
# in scd_vm.c entfernt -> keine Umschaltung mit Treffer -> rot.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r34n_g_karte>
#               -DRE15_EVAL_TOOL=<probe_r34n_g_schrift_eval> -DWORKDIR=<dir>
#               -P test_r34n_g_schrift1150.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_EVAL_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "schrift1150: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "schrift1150: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Alte Laufordner dieses Riegels weg: ein ROT abgebrochener Lauf laesst seine Bilder liegen
# (~200 MB je Lauf, gemessen 1,1 GB nach den Mutationsproben).
file(GLOB _alte_laeufe "${WORKDIR}/lauf_*")
if(_alte_laeufe)
    file(REMOVE_RECURSE ${_alte_laeufe})
endif()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

function(schrift_lauf _name _raum _out_dir)
    set(ARGS_EXTRA ${ARGN})
    set(WORKDIR "${_basis}_${_name}")
    file(REMOVE_RECURSE "${WORKDIR}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" 2
                    WORKING_DIRECTORY "${WORKDIR}"
                    TIMEOUT 60
                    RESULT_VARIABLE _rvk
                    OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "schrift1150[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1
        RE15_NOAUDIO=1
        RE15_CONTINUE_TEST=1
        RE15_CARD_AUTO=1
        RE15_CARD_SLOT=0
        RE15_WINDOW_SCALE=3
        RE15_MG_LOG=mg.log
        ${ARGS_EXTRA}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "schrift1150[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    file(READ "${WORKDIR}/debug.log" _log)
    string(FIND "${_log}" "CONTINUE: resumed in room ${_raum}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "schrift1150[${_name}]: der Lauf hat nicht in ROOM${_raum} geladen "
                            "(exit=${_rv})")
    endif()
    string(FIND "${_log}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "schrift1150[${_name}]: Endbild nicht erreicht (exit=${_rv})")
    endif()
    string(FIND "${_log}" "[pri] cut=2 pri_offset=0x66C masks=54" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "schrift1150[${_name}]: Cut 2 nicht gezeigt (keine [pri]-Zeile cut=2 "
                            "pri_offset=0x66C masks=54)")
    endif()
    set(${_out_dir} "${WORKDIR}" PARENT_SCOPE)
endfunction()

# R: Referenz ohne Masken
schrift_lauf(ref 1150 _dir_ref RE15_NO_PRI=1 "RE15_FRAMEDUMP=150:ref.ppm" "RE15_EXIT_AT=150#1150")
if(NOT EXISTS "${_dir_ref}/ref.ppm")
    message(FATAL_ERROR "schrift1150[ref]: kein Referenzbild ref.ppm")
endif()

function(schrift_auswerten _name _dir _raum _bis)
    execute_process(COMMAND "${RE15_EVAL_TOOL}" "${_dir_ref}/ref.ppm" "${_dir}/f" 100 ${_bis}
                            "${_dir}/mg.log" ${_raum} ${ARGN}
                    TIMEOUT 120
                    RESULT_VARIABLE _rve
                    OUTPUT_VARIABLE _oute)
    message(STATUS "schrift1150[${_name}]:\n${_oute}")
    if(NOT _rve EQUAL 0)
        message(FATAL_ERROR "schrift1150[${_name}]: die Leuchtschrift blinkt nicht wie im Original "
                            "(Auswerter exit=${_rve}) — Dossier G2_schrift1150.md §9")
    endif()
    # Bilder wieder weg (je Lauf ~200 MB), Log und Auswertung bleiben.
    file(GLOB _ppm "${_dir}/f*.ppm")
    if(_ppm)
        file(REMOVE ${_ppm})
    endif()
endfunction()

# A/B: Lade-Weg ohne Cut-Ereignis -> genau EIN Aufbau (Auflage 6).
foreach(_raum 1150 1151)
    schrift_lauf(r${_raum} ${_raum} _dir "RE15_FRAMEDUMP=100-200/1:f" "RE15_EXIT_AT=200#${_raum}")
    schrift_auswerten(${_raum} "${_dir}" ${_raum} 200 --aufbau 1)
endforeach()

# C: Statusschirm in einer AUS-Phase (Auflagen 5/6): START bei F125 (:= 0 war F119), zu nach 36
# Bildern. Original: Dirty := 2 (@0x800466fc) -> KEIN Aufbau, der Zustand bleibt AUS, das naechste
# := 1 kommt 20 SCD-Takte nach F119 (Laeufer steht im Menue @0x8003f040-4c). Gemessen: Menue
# F126..F176, := 1 bei F191. Bilder F126..F180 zeigen den Schirm und seine Blenden.
schrift_lauf(inv1150 1150 _dir "RE15_FRAMEDUMP=100-260/1:f" "RE15_EXIT_AT=260#1150"
             RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=125 "RE15_INPUT_SCRIPT=S0.1,W1.2,S0.1")
schrift_auswerten(inv1150 "${_dir}" 1150 260 --aufbau 1 --menue 126-180)

message(STATUS "schrift1150: OK — ROOM1150 und ROOM1151 Cut 2: HEAVEN blinkt 20/20 Bilder wie sub05 "
               "(Opcode 0x45) es vorgibt, am Lade-Weg der echten exe")
