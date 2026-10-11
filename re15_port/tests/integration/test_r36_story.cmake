# =============================================================================
# STORY LEON/ELZA MIT DER ECHTEN EXE (Runde 36, Spur P): der Abnahmeweg — Spielstand + CONTINUE (Boot-Weg
# main.c, geht NICHT durch scd_room_reenter) und echter Tuer-Eintritt per Aktionstaste (Tuer-Weg
# room_common.c: scd_room_reenter Schritt 7, Nachrichtenblock Schritt 11). Leon-Laeufe (xxx0) und Elza-Laeufe
# (xxx1, Figur 4 im Spielstand). Geprueft wird das LOG (debug.log): "[story36] ..." (Installer) und der
# Messhaken RE15_MSG_LOG "[msgtext] room=.. id=.. "<Wortlaut>"" (je geoeffnetem Text, re15_story_r36.h).
#
# Dossier analysis/befunde_runde36/P_story.md, Konstanten include/re15_story_r36.h, Karten
# probe_r36_story_karte (Staende dort beschrieben). Die Unit-Riegel unit_r36_story_* pruefen die Mechanik
# (Prop/Platz/Text) Schritt fuer Schritt; dieser Riegel prueft, was nur die exe zeigt: Lade-Weg, Tuer-Weg,
# Spielerposition/Kollision/Aktions-Scan der echten Spielschleife und den angezeigten Wortlaut.
#
# TEIL=leon:
#   P1a vor1020   ROOM1010 -> Tuer -> ROOM1020 ohne Strom: "Roy noch nicht da"; RE15_FIRE_AOT Slot 5 -> KEIN Text
#   P1b roy strom CONTINUE in ROOM1020 mit (4,0xF3): "Roy liegt da"; Aktionstaste vor Roy -> msg 0 Roy-Text
#   P2a zelle     CONTINUE in ROOM1210: Aktionstaste im Gang vor der Zelle -> msg 20 "The cell door is open..."
#   P2b vor1210   ROOM11E0 -> Tuer -> ROOM1210: Text-Platz Slot 9 angelegt; RE15_FIRE_AOT Slot 9 -> msg 20
#   P4a vor6030   ROOM6000 -> Tuer -> ROOM6030 (Cut 6), 3 s vorwaerts in die Marke: Wagen-Szene msg 0..8 mit
#                 neuem msg 1/2, kein "Where's Elza"
#   P4b zug_leon  CONTINUE in ROOM6030 (Cut 0): Annette-Szene ZUERST, msg 12 neu, kein "Keeping the monsters"
#   P4c annette   CONTINUE in ROOM6000: Treffen mit Annette msg 0..3 mit neuem msg 0/3
#   P4d ada       CONTINUE in ROOM50B0: Adas Szene, msg 1 neu
# TEIL=elza:
#   P3a schuppen  CONTINUE in ROOM1091 (Elza): Aktionstaste vor dem Schuppen -> msg 20 "It's a burnt-out shed..."
#   P3b vor1091   ROOM1051 -> Tuer -> ROOM1091 (Elza): Text-Platz Slot 5 angelegt; RE15_FIRE_AOT Slot 5 -> msg 20
#   P4e vor6031   ROOM6001 -> Tuer -> ROOM6031 (Cut 6), Drehung + 4 s vorwaerts: Wagen-Szene msg 0..2 mit neuem 0/1
#   P4f marvin    CONTINUE in ROOM30C1 mit (3,0x39): Marvin-Szene msg 1..8 mit neuem msg 3/4/7
#
# RE15_SOFTWARE_RENDER=1 dient nur der Robustheit des Testhakens; SDL_AUDIODRIVER=dummy.
# Die Zeitachse des Eingabeskripts (Basis "spiel") beginnt in JEDEM Raum neu (gemessen: "[input-script]
# Tick 0 -> F60" nach dem Tuer-Eintritt erneut) — die Tuer-Laeufe enden deshalb mit langem Warten, und der
# zweite Druck bei F60 im Zielraum trifft dort keinen Platz (Spawn-Vorwaertspunkte geprueft, Dossier).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_story_karte> -DWORKDIR=<dir> -DTEIL=leon|elza
#               -P test_r36_story.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_story: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r36_story: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_story: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL leon)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake): local_build.sh eines parallel bauenden Baums darf
# diesen Lauf nicht treffen.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36p_${TEIL}_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r36p_${TEIL}_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

# lauf(<name> <karten-argumente (;-Liste)> <raum> <skript> <ende> <out> [KEY=WERT ...])
function(lauf _name _karte _raum _skript _ende _out_debug)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_karte}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r36_story[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=${_skript}"
        RE15_MSG_LOG=1
        "RE15_EXIT_AT=${_ende}"
        ${ARGN}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r36_story[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r36_story[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r36_story[${_name}]: Endbild ${_ende} nicht erreicht - Tuer nicht genommen oder "
                            "Lauf abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    message(STATUS "r36_story[${_name}]: Lauf ok (${WORKDIR})")
    set(${_out_debug} "${_dbg}" PARENT_SCOPE)
endfunction()

function(muss _name _text _wort)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r36_story[${_name}]: erwartet, aber fehlt: ${_wort}")
    endif()
    message(STATUS "r36_story[${_name}]: ok   ${_wort}")
endfunction()

function(darf_nicht _name _text _wort)
    string(FIND "${_text}" "${_wort}" _p)
    if(NOT _p LESS 0)
        message(FATAL_ERROR "r36_story[${_name}]: darf nicht vorkommen: ${_wort}")
    endif()
    message(STATUS "r36_story[${_name}]: ok   nicht: ${_wort}")
endfunction()

if(TEIL STREQUAL "leon")
    # --- P1a: Tuer ROOM1010 -> ROOM1020 OHNE Strom -------------------------------------------------------
    lauf(p1a "vor1020" 1010 "A0.3,W30" "400#1020" _d "RE15_FIRE_AOT=5@200#1020")
    muss(p1a "${_d}" "[story36] ROOM1020: Strom (4,0xF3)=0 -> Roy noch nicht da")
    muss(p1a "${_d}" "[fire-aot] slot=5")
    darf_nicht(p1a "${_d}" "[msgtext] room=1020 id=0 ")

    # --- P1b: CONTINUE in ROOM1020 MIT Strom, Aktionstaste vor Roy ---------------------------------------
    lauf(p1b "roy;strom" 1020 "A0.3,W30" "600#1020" _d)
    muss(p1b "${_d}" "[story36] ROOM1020: Strom (4,0xF3)=1 -> Roy liegt da")
    muss(p1b "${_d}" "[msgtext] room=1020 id=0 \"It's the RPD officer, Roy.He appears to have died recently.\"")

    # --- P2a: CONTINUE in ROOM1210, Aktionstaste vor der Zelle -------------------------------------------
    lauf(p2a "zelle" 1210 "A0.3,W30" "400#1210" _d)
    muss(p2a "${_d}" "[story36] ROOM1210: Text-Platz Slot 9")
    muss(p2a "${_d}" "[msgtext] room=1210 id=20 \"The cell door is open...Whoever was locked up here is gone.\"")

    # --- P2b: Tuer ROOM11E0 -> ROOM1210, Platz feuern ---------------------------------------------------
    lauf(p2b "vor1210" 11e0 "A0.3,W30" "400#1210" _d "RE15_FIRE_AOT=9@200#1210")
    muss(p2b "${_d}" "[story36] ROOM1210: Text-Platz Slot 9")
    muss(p2b "${_d}" "[msgtext] room=1210 id=20 \"The cell door is open...Whoever was locked up here is gone.\"")

    # --- P4a: Tuer ROOM6000 -> ROOM6030 (Cut 6), vorwaerts in die Marke: Wagen-Szene ----------------------
    lauf(p4a "vor6030" 6000 "A0.3,W8,U3,W40" "2600#6030" _d)
    muss(p4a "${_d}" "[story36] ROOM6030: 3 Nachrichten umgeschrieben")
    muss(p4a "${_d}" "[msgtext] room=6030 id=1 \"Leon: Is anyone else with you?\"")
    muss(p4a "${_d}" "[msgtext] room=6030 id=2 \"Sherry: Elza is out there fighting the monsters!\"")
    muss(p4a "${_d}" "[msgtext] room=6030 id=8 \"Leon: I see. Thanks.\"")
    darf_nicht(p4a "${_d}" "Where's Elza")

    # --- P4b: CONTINUE in ROOM6030 (Cut 0): Annette-Szene ZUERST ------------------------------------------
    lauf(p4b "zug_leon" 6030 "W30" "1500#6030" _d)
    muss(p4b "${_d}" "[msgtext] room=6030 id=11 \"Annette: Where's that biker girl?\"")
    muss(p4b "${_d}" "[msgtext] room=6030 id=12 \"Leon: I've never met her. Can you start this thing?\"")
    darf_nicht(p4b "${_d}" "Keeping the monsters at bay")

    # --- P4c: CONTINUE in ROOM6000: Treffen mit Annette --------------------------------------------------
    lauf(p4c "annette" 6000 "W30" "1500#6000" _d)
    muss(p4c "${_d}" "[msgtext] room=6000 id=0 \"Annette: You must be that cop. Your friend is already here.\"")
    muss(p4c "${_d}" "[msgtext] room=6000 id=3 \"Annette: Come on. Let's join the others and get out of here.\"")
    darf_nicht(p4c "${_d}" "Your friends")

    # --- P4d: CONTINUE in ROOM50B0: Adas Szene ------------------------------------------------------------
    lauf(p4d "ada" 50b0 "W30" "1500#50b0" _d)
    muss(p4d "${_d}" "[msgtext] room=50b0 id=1 \"Ada: Some other survivors are down there with her too.\"")
    darf_nicht(p4d "${_d}" "Your other friends")
elseif(TEIL STREQUAL "elza")
    # --- P3a: CONTINUE in ROOM1091 (Elza), Aktionstaste vor dem Schuppen --------------------------------
    lauf(p3a "schuppen" 1091 "A0.3,W30" "400#1091" _d)
    muss(p3a "${_d}" "[story36] ROOM1091: Text-Platz Slot 5")
    muss(p3a "${_d}" "[msgtext] room=1091 id=20 \"It's a burnt-out shed.Someone must have put out the fire.\"")

    # --- P3b: Tuer ROOM1051 -> ROOM1091 (Elza), Platz feuern ---------------------------------------------
    lauf(p3b "vor1091" 1051 "A0.3,W30" "400#1091" _d "RE15_FIRE_AOT=5@200#1091")
    muss(p3b "${_d}" "[story36] ROOM1091: Text-Platz Slot 5")
    muss(p3b "${_d}" "[msgtext] room=1091 id=20 \"It's a burnt-out shed.Someone must have put out the fire.\"")

    # --- P4e: Tuer ROOM6001 -> ROOM6031 (Cut 6), Drehung nach -X und vorwaerts in die Marke: Wagen-Szene ----
    #     Elzas Marke liegt westlicher als Leons: ROOM6031 main00 @0x00CC6 Rechteck (-25130,-25270,1660,5200)
    #     gegen ROOM6030 (-19730,-25570,1600,6300); R0.45 dreht von 1100 auf ~2444 (gemessen), U4 laeuft hinein.
    lauf(p4e "vor6031" 6001 "A0.3,W8,R0.45,U4,W40" "1800#6031" _d)
    muss(p4e "${_d}" "[story36] ROOM6031: 2 Nachrichten umgeschrieben")
    muss(p4e "${_d}" "[msgtext] room=6031 id=0 \"Elza: Sherry! Is anyone else here?\"")
    muss(p4e "${_d}" "[msgtext] room=6031 id=1 \"Sherry: A policeman! He's fighting off monsters. He wants us to start the train.\"")
    muss(p4e "${_d}" "[msgtext] room=6031 id=2 \"Elza: I'll do it!\"")
    darf_nicht(p4e "${_d}" "Where's Leon")

    # --- P4f: CONTINUE in ROOM30C1 mit (3,0x39): Marvin-Szene ---------------------------------------------
    lauf(p4f "marvin" 30c1 "W60" "2600#30c1" _d)
    muss(p4f "${_d}" "[msgtext] room=30c1 id=2 \"Officer: I'm Marvin Branagh, RPD. I'm here with Leon Kennedy.\"")
    muss(p4f "${_d}" "[msgtext] room=30c1 id=3 \"Elza: Leon? I don't know him. I never ran into him at the RPD.\"")
    muss(p4f "${_d}" "[msgtext] room=30c1 id=4 \"Marvin: He's a rookie, and he's helping me and Ada escape the city.\"")
    muss(p4f "${_d}" "[msgtext] room=30c1 id=7 \"Elza: Will you tell them that me and John and Sherry are here and we're all right?\"")
    muss(p4f "${_d}" "[msgtext] room=30c1 id=8 \"Marvin: Will do. See you soon!\"")
    darf_nicht(p4f "${_d}" "separated")
    darf_nicht(p4f "${_d}" "How is he")
else()
    message(FATAL_ERROR "r36_story: TEIL '${TEIL}' unbekannt (leon|elza)")
endif()

file(REMOVE "${_exe_kopie}")
message(STATUS "r36_story[${TEIL}]: alle Laeufe bestanden")
