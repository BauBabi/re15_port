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
#   P4a vor6030   ROOM6000 -> Tuer -> ROOM6030 (Cut 6), 3 s vorwaerts in die Marke: Wagen-Szene; seit Runde 41 ST80-T
#                 das Programm k_s80_6030_sub02 mit 13 Zeilen (msg 0,1,21,2,3,22,4,5,6,7,23,24,8), kein "Where's Elza"
#   P4b zug_leon  CONTINUE in ROOM6030 (Cut 0), Wagen-Szene NICHT gelaufen ((3,22)=0): Fuehrerstand VOR dem Wagen.
#                 Seit Runde 41 ST80 Nachbesserung 1 (Abnahme 1 M1) die Fassung k_s80_6030_sub08_vorne: msg 11, 31
#                 "Everyone we could find. Can you start this thing?", 13 - KEINE Sherry-Zeile (msg 12/25/26/40 nie),
#                 kein "biker girl", kein "Keeping the monsters". GEGENPROBE: Stand ad8532cd4 zeigt hier msg 12/25 -> rot.
#   P4h zug_wagen CONTINUE in ROOM6030 (Cut 0) mit (3,22)=1 (Wagen zuerst, sub02 @0x00D7A): Fuehrerstand wie bisher
#                 (k_s80_6030_sub08): msg 11, 12, 25, 26 vor "Certainly.", kein msg 31/40
#   P4i nachtrag  CONTINUE in ROOM6030 (Cut 0) mit (3,22)=1, (4,23)=1 und (9,128)=1 (vorne zuerst, danach Wagen):
#                 sub07 @0x0110C -> Nachtrag k_s80_6030_sub09_nachtrag: msg 40, 25; kein msg 11/13/31 (nichts doppelt)
#   P4c annette   CONTINUE in ROOM6000: Treffen mit Annette msg 0..3 mit neuem msg 0/3
#   P4d ada       CONTINUE in ROOM50B0: Adas Szene, msg 1 neu
# TEIL=zug (Runde 41 ST80 Nachbesserung 1; normale Eingaben, kein CONTINUE im Fuehrerstand, Weg = Abnahme 1 Lauf kabine5):
#   P4j vorne_weg ROOM6000 -> Tuer -> ROOM6030 (Cut 6) -> Gang -> Treppe (main00 @0x00CB2) -> Tuer Slot 1 (@0x00C52) ->
#                 Fuehrerstand VOR dem Wagen (11, 31, 13..19) -> Tuer zurueck -> Treppe -> Marke Slot 5 (@0x00CC6) ->
#                 Wagen (13 Zeilen, "Who are you two?" vor Johns Vorstellung) -> zurueck nach vorne -> Nachtrag (40, 25).
#                 Reihenfolge geprueft, jede Zeile genau einmal, kein msg 12/26.
#   P4k wagen_weg ROOM6000 -> ROOM6030 -> Marke (P4a-Weg) -> Wagen -> Gang/Treppe/Tuer -> Fuehrerstand wie bisher
#                 (11, 12, 25, 26, 13..19), kein msg 31/40.
# TEIL=torweg_f|torweg_w|torweg_r (Runde 41 Integration A2, Gegenpruefung Welle 2 Befund 2): DURCHGEHENDE Laeufe erster Besuch
#   6030 -> 6000 Tor -> zweiter Besuch 6030, Leon, Karte "vor6030 tor_bereit" ((3,48)/(3,49) Bahnsteig-Kampf 5080 vorbei, (3,239)).
#   Je Raum ein Abschnitt des Eingabeskripts ('|', input_pc.c), RE15_FIRE_AOT der Reihe nach (main.c): Ausgang Slot 2 (main00
#   @0x00C72) -> 6000, Torschalter Slot 5 (main00 @0x00E66 sce 3 -> sub07: msg 5, Ja, @0x011B4 Set(3,89)), Tuer Slot 3 (@0x00DDA)
#   -> 6030 Cut 6. Im 6030 Wege der P4j/P4k; im zweiten Besuch Gang -> Treppe -> Fuehrerstand, Tuer hinaus und wieder hinein.
#   torweg_f  vorne zuerst -> Wagen -> OHNE Rueckkehr -> Tor -> Fuehrerstand: Nachtrag msg 40, 25 (k_s80_6030_sub05_nachtrag_tor,
#             sub07 @0x01132) vor dem Zugstart der Spur AN (msg 27..30) beim naechsten Betreten; (9,128) 1 -> 0, (9,133) 0 -> 1.
#             GEGENPROBE Stand dabac8d22 (gemessen, Lauf weg_F_vor): Zugstart 27..30 ohne 40/25, (9,128) bleibt 1 -> rot.
#   torweg_w  Wagen zuerst -> Fuehrerstand (msg 11, 12, 25, 26) -> Tor -> Fuehrerstand: Zugstart sofort, kein Nachtrag.
#   torweg_r  vorne -> Wagen -> zurueck nach vorne (Nachtrag sub09) -> Tor -> Fuehrerstand: Zugstart sofort, kein zweiter Nachtrag.
#   Jede Zeile genau einmal; der Wagen-Eintritt nach dem Tor zeigt die Tuer nach 5090 noch zu (Spur AN k_tuer), keine Wagen-Szene
#   (Torschalter @0x011C0 Set(3,22)). Schranken = Bilder der Messlaeufe weg_*_nach (Ausgang F5100/F5000/F6800, Tor F100, Tuer F900).
# TEIL=elza:
#   P3a schuppen  CONTINUE in ROOM1091 (Elza): Aktionstaste vor dem Schuppen -> msg 20 "It's a burnt-out shed..."
#   P3b vor1091   ROOM1051 -> Tuer -> ROOM1091 (Elza): Text-Platz Slot 5 angelegt; RE15_FIRE_AOT Slot 5 -> msg 20
#   P4e vor6031   ROOM6001 -> Tuer -> ROOM6031 (Cut 6), Drehung + 4 s vorwaerts: Wagen-Szene msg 0..2 mit neuem 0/1
#   P4f marvin    CONTINUE in ROOM30C1 mit (3,0x39): Marvin-Szene, seit Runde 41 ST80-T kurze Fassung msg 1..5
#                 (k_s80_30c1_sub03), msg 6..8 nie
#   P4g john30e1  CONTINUE in ROOM30E1 (Cut 0) mit (3,0xB5): Elza trifft John, msg 2/3 nach A1 (Runde 41 ST80-T)
#
# RE15_SOFTWARE_RENDER=1 dient nur der Robustheit des Testhakens; SDL_AUDIODRIVER=dummy.
# Die Zeitachse des Eingabeskripts (Basis "spiel") beginnt in JEDEM Raum neu (gemessen: "[input-script]
# Tick 0 -> F60" nach dem Tuer-Eintritt erneut) — die Tuer-Laeufe enden deshalb mit langem Warten, und der
# zweite Druck bei F60 im Zielraum trifft dort keinen Platz (Spawn-Vorwaertspunkte geprueft, Dossier).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_story_karte> -DWORKDIR=<dir> -DTEIL=leon|elza|zug
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

# Zeitgrenze je Lauf (s); die durchgehenden torweg-Laeufe dauern allein 390-470 s (weg_*_nach).
set(_zeit 600)
if(TEIL MATCHES "^torweg_")
    set(_zeit 1100)
endif()

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
    re15_start_spiel(_rv ${_zeit}
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

# Runde 41 ST80 Nachbesserung 1: Reihenfolge (erstes Vorkommen von _a vor dem ersten von _b) und Anzahl.
function(vor _name _text _a _b)
    string(FIND "${_text}" "${_a}" _pa)
    string(FIND "${_text}" "${_b}" _pb)
    if(_pa LESS 0 OR _pb LESS 0 OR NOT _pa LESS _pb)
        message(FATAL_ERROR "r36_story[${_name}]: Reihenfolge falsch: '${_a}' (@${_pa}) muss vor '${_b}' (@${_pb}) stehen")
    endif()
    message(STATUS "r36_story[${_name}]: ok   '${_a}' vor '${_b}'")
endfunction()

function(genau_einmal _name _text _wort)
    string(REPLACE "${_wort}" "\n@@TREFFER@@\n" _t "${_text}")
    string(REGEX MATCHALL "@@TREFFER@@" _m "${_t}")
    list(LENGTH _m _n)
    if(NOT _n EQUAL 1)
        message(FATAL_ERROR "r36_story[${_name}]: ${_n}x statt genau einmal: ${_wort}")
    endif()
    message(STATUS "r36_story[${_name}]: ok   genau einmal: ${_wort}")
endfunction()

# Sherry darf vor dem Wagengespraech in keiner angezeigten Zeile stehen.
function(keine_sherry_zeile _name _text)
    string(REGEX MATCH "\\[msgtext\\][^\n]*(Sherry|daughter|little girl)" _m "${_text}")
    if(_m)
        message(FATAL_ERROR "r36_story[${_name}]: Sherry-Zeile vor dem Wagen: ${_m}")
    endif()
    message(STATUS "r36_story[${_name}]: ok   keine Sherry-Zeile")
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
    lauf(p4a "vor6030" 6000 "A0.3,W8,U3,W40" "3600#6030" _d)
    # 15 = 13 (ST80-T) + msg 31/40 (ST80 Nachbesserung 1)
    muss(p4a "${_d}" "[story36] ROOM6030: 15 Nachrichten umgeschrieben/neu")
    muss(p4a "${_d}" "[story80] ROOM6030 Evt_exec sub02 -> Programm")
    muss(p4a "${_d}" "[msgtext] room=6030 id=1 \"Leon: Who are you two?\"")
    muss(p4a "${_d}" "[msgtext] room=6030 id=21 \"John: Name's John. This is Sherry. We made it down here with a girl named Elza.\"")
    muss(p4a "${_d}" "[msgtext] room=6030 id=2 \"Sherry: She's out there fighting the monsters!\"")
    muss(p4a "${_d}" "[msgtext] room=6030 id=22 \"John: She said she'd catch up with us.\"")
    muss(p4a "${_d}" "[msgtext] room=6030 id=23 \"Sherry: A lab coat...? Is it Mom?!\"")
    muss(p4a "${_d}" "[msgtext] room=6030 id=24 \"John: Easy, kid. Stay with me.\"")
    muss(p4a "${_d}" "[msgtext] room=6030 id=8 \"Leon: I'll find out. Stay here.\"")
    darf_nicht(p4a "${_d}" "Where's Elza")
    darf_nicht(p4a "${_d}" "I see. Thanks.")

    # --- P4b: CONTINUE in ROOM6030 (Cut 0), (3,22)=0: Fuehrerstand VOR dem Wagen (ST80 Nachbesserung 1) ------
    lauf(p4b "zug_leon" 6030 "W30" "1500#6030" _d)
    muss(p4b "${_d}" "[story80] ROOM6030 Evt_exec sub08 -> Programm (576 B): Fuehrerstand vor dem Wagen")
    muss(p4b "${_d}" "[msgtext] room=6030 id=11 \"Annette: Is everyone aboard?\"")
    muss(p4b "${_d}" "[msgtext] room=6030 id=31 \"Leon: Everyone we could find. Can you start this thing?\"")
    muss(p4b "${_d}" "[msgtext] room=6030 id=13 \"Annette: Certainly.\"")
    vor(p4b "${_d}" "[msgtext] room=6030 id=11 " "[msgtext] room=6030 id=31 ")
    vor(p4b "${_d}" "[msgtext] room=6030 id=31 " "[msgtext] room=6030 id=13 ")
    keine_sherry_zeile(p4b "${_d}")
    darf_nicht(p4b "${_d}" "[msgtext] room=6030 id=12 ")
    darf_nicht(p4b "${_d}" "[msgtext] room=6030 id=25 ")
    darf_nicht(p4b "${_d}" "[msgtext] room=6030 id=26 ")
    darf_nicht(p4b "${_d}" "[msgtext] room=6030 id=40 ")
    darf_nicht(p4b "${_d}" "biker girl")
    darf_nicht(p4b "${_d}" "Keeping the monsters at bay")

    # --- P4h: CONTINUE in ROOM6030 (Cut 0), (3,22)=1: Fuehrerstand NACH dem Wagen wie bisher ----------------
    lauf(p4h "zug_leon;wagen" 6030 "W30" "1500#6030" _d)
    muss(p4h "${_d}" "[story80] ROOM6030 Evt_exec sub08 -> Programm (644 B): Fuehrerstand msg 11,12,25,26")
    muss(p4h "${_d}" "[msgtext] room=6030 id=11 \"Annette: Is everyone aboard?\"")
    muss(p4h "${_d}" "[msgtext] room=6030 id=12 \"Leon: Everyone we could find. There's a little girl in the back... Sherry.\"")
    muss(p4h "${_d}" "[msgtext] room=6030 id=25 \"Annette: Sherry...? My daughter is on this train? ...Thank God.\"")
    muss(p4h "${_d}" "[msgtext] room=6030 id=26 \"Leon: Can you start this thing?\"")
    muss(p4h "${_d}" "[msgtext] room=6030 id=13 \"Annette: Certainly.\"")
    vor(p4h "${_d}" "[msgtext] room=6030 id=12 " "[msgtext] room=6030 id=25 ")
    vor(p4h "${_d}" "[msgtext] room=6030 id=26 " "[msgtext] room=6030 id=13 ")
    darf_nicht(p4h "${_d}" "[msgtext] room=6030 id=31 ")
    darf_nicht(p4h "${_d}" "[msgtext] room=6030 id=40 ")
    darf_nicht(p4h "${_d}" "biker girl")

    # --- P4i: CONTINUE in ROOM6030 (Cut 0) nach "vorne zuerst" und Wagen: Nachtrag msg 40, 25 ---------------
    lauf(p4i "zug_leon;wagen;nachtrag" 6030 "W30" "1500#6030" _d)
    muss(p4i "${_d}" "[story80] ROOM6030 Evt_exec sub09 -> Programm (238 B): Fuehrerstand nach dem Wagen, Nachtrag")
    muss(p4i "${_d}" "[msgtext] room=6030 id=40 \"Leon: There's a little girl in the back... Sherry.\"")
    muss(p4i "${_d}" "[msgtext] room=6030 id=25 \"Annette: Sherry...? My daughter is on this train? ...Thank God.\"")
    vor(p4i "${_d}" "[msgtext] room=6030 id=40 " "[msgtext] room=6030 id=25 ")
    darf_nicht(p4i "${_d}" "[msgtext] room=6030 id=11 ")
    darf_nicht(p4i "${_d}" "[msgtext] room=6030 id=13 ")
    darf_nicht(p4i "${_d}" "[msgtext] room=6030 id=31 ")

    # --- P4c: CONTINUE in ROOM6000: Treffen mit Annette --------------------------------------------------
    lauf(p4c "annette" 6000 "W30" "1500#6000" _d)
    muss(p4c "${_d}" "[msgtext] room=6000 id=0 \"Annette: You must be that cop. Your friend is already here.\"")
    muss(p4c "${_d}" "[msgtext] room=6000 id=3 \"Annette: Come on. Let's join the others and get out of here.\"")
    darf_nicht(p4c "${_d}" "Your friends")

    # --- P4d: CONTINUE in ROOM50B0: Adas Szene ------------------------------------------------------------
    lauf(p4d "ada" 50b0 "W30" "1500#50b0" _d)
    muss(p4d "${_d}" "[msgtext] room=50b0 id=1 \"Ada: Some other survivors are down there with her too.\"")
    darf_nicht(p4d "${_d}" "Your other friends")
elseif(TEIL STREQUAL "zug")
    # --- P4j: Weg der Abnahme 1 (Lauf kabine5) als Eingabe, plus Rueckweg nach vorne ------------------------
    #     6000 Tuer (A0.3) -> 6030 Cut 6 bei (-17970,-20380); Gang nach Suedost bis (-13307,-22758); Treppe (A0.3);
    #     Tuer Slot 1 (A0.3) -> Fuehrerstand; W68 Szene; Tuer zurueck (A0.3), Treppe (A0.3), XU3 in die Marke;
    #     W75 Wagen-Szene. Rueckweg (gemessen, ST80 NB1): L0.40 dreht von 1045 auf -107, XU3 laeuft am Gang-Rand
    #     (SCA #3 / #2) nach (-13307,-22758), R0.07, Treppe, XU1, R0.07, Tuer Slot 1 -> Nachtrag ab F5838.
    lauf(p4j "vor6030" 6000
         "A0.3,W8,L0.07,XU0.5,W0.5,L0.38,XU2,R0.12,A0.3,W6,XU1,R0.07,A0.3,W68,A0.3,W11,XU1,A0.3,W2,XU3,W75,L0.40,XU3,W0.5,R0.07,A0.3,W6,XU1,R0.07,A0.3,W30"
         "6800#6030" _d)
    muss(p4j "${_d}" "[story80] ROOM6030 Evt_exec sub08 -> Programm (576 B): Fuehrerstand vor dem Wagen")
    muss(p4j "${_d}" "[story80] ROOM6030 Evt_exec sub02 -> Programm")
    muss(p4j "${_d}" "[story80] ROOM6030 Evt_exec sub09 -> Programm (238 B): Fuehrerstand nach dem Wagen, Nachtrag")
    vor(p4j "${_d}" "[msgtext] room=6030 id=11 " "[msgtext] room=6030 id=31 ")
    vor(p4j "${_d}" "[msgtext] room=6030 id=31 " "[msgtext] room=6030 id=13 ")
    vor(p4j "${_d}" "[msgtext] room=6030 id=19 " "[msgtext] room=6030 id=0 ")
    vor(p4j "${_d}" "[msgtext] room=6030 id=1 \"Leon: Who are you two?\"" "[msgtext] room=6030 id=21 ")
    vor(p4j "${_d}" "[msgtext] room=6030 id=21 " "[msgtext] room=6030 id=40 ")
    vor(p4j "${_d}" "[msgtext] room=6030 id=8 " "[msgtext] room=6030 id=40 ")
    vor(p4j "${_d}" "[msgtext] room=6030 id=40 " "[msgtext] room=6030 id=25 ")
    # bis zur Vorstellung im Wagen (msg 21) keine Sherry-Zeile
    string(FIND "${_d}" "[msgtext] room=6030 id=21 " _p21)
    string(SUBSTRING "${_d}" 0 ${_p21} _vor_wagen)
    keine_sherry_zeile(p4j "${_vor_wagen}")
    foreach(_id 11 31 13 14 15 16 17 18 19 0 1 21 2 3 22 4 5 6 7 23 24 8 40 25)
        genau_einmal(p4j "${_d}" "[msgtext] room=6030 id=${_id} ")
    endforeach()
    darf_nicht(p4j "${_d}" "[msgtext] room=6030 id=12 ")
    darf_nicht(p4j "${_d}" "[msgtext] room=6030 id=26 ")

    # --- P4k: Wagen zuerst (P4a-Weg), danach derselbe Rueckweg nach vorne: Fuehrerstand wie bisher ----------
    lauf(p4k "vor6030" 6000 "A0.3,W8,U3,W62,L0.40,XU3,W0.5,R0.07,A0.3,W6,XU1,R0.07,A0.3,W68" "4800#6030" _d)
    muss(p4k "${_d}" "[story80] ROOM6030 Evt_exec sub02 -> Programm")
    muss(p4k "${_d}" "[story80] ROOM6030 Evt_exec sub08 -> Programm (644 B): Fuehrerstand msg 11,12,25,26")
    vor(p4k "${_d}" "[msgtext] room=6030 id=8 " "[msgtext] room=6030 id=11 ")
    vor(p4k "${_d}" "[msgtext] room=6030 id=11 " "[msgtext] room=6030 id=12 ")
    vor(p4k "${_d}" "[msgtext] room=6030 id=12 " "[msgtext] room=6030 id=25 ")
    vor(p4k "${_d}" "[msgtext] room=6030 id=25 " "[msgtext] room=6030 id=26 ")
    vor(p4k "${_d}" "[msgtext] room=6030 id=26 " "[msgtext] room=6030 id=13 ")
    foreach(_id 0 1 21 2 3 22 4 5 6 7 23 24 8 11 12 25 26 13 14 15 16 17 18 19)
        genau_einmal(p4k "${_d}" "[msgtext] room=6030 id=${_id} ")
    endforeach()
    darf_nicht(p4k "${_d}" "[msgtext] room=6030 id=31 ")
    darf_nicht(p4k "${_d}" "[msgtext] room=6030 id=40 ")
    darf_nicht(p4k "${_d}" "Evt_exec sub09 -> Programm")
elseif(TEIL MATCHES "^torweg_(f|w|r)$")
    set(_weg "${CMAKE_MATCH_1}")
    set(_vorne "A0.3,W8,L0.07,XU0.5,W0.5,L0.38,XU2,R0.12,A0.3,W6,XU1,R0.07,A0.3")
    if(_weg STREQUAL "f")
        set(_seg1 "${_vorne},W68,A0.3,W11,XU1,A0.3,W2,XU3,W75")
        set(_aus 5100)
    elseif(_weg STREQUAL "w")
        set(_seg1 "A0.3,W8,U3,W62,L0.40,XU3,W0.5,R0.07,A0.3,W6,XU1,R0.07,A0.3,W68,A0.3,W11")
        set(_aus 5000)
    else()
        set(_seg1 "${_vorne},W68,A0.3,W11,XU1,A0.3,W2,XU3,W75,L0.40,XU3,W0.5,R0.07,A0.3,W6,XU1,R0.07,A0.3,W20,A0.3,W11")
        set(_aus 6800)
    endif()
    set(_n "torweg_${_weg}")
    lauf(${_n} "vor6030;tor_bereit" 6000
         "A0.3,W8|${_seg1}|W5,A0.2,W1,A0.2,W1,A0.2,W30|${_vorne},W45,A0.3,W11,R0.71,A0.3,W65"
         "4300#6030!3" _d
         "RE15_FIRE_AOT=2@${_aus}#6030,5@100#6000,3@900#6000"
         "RE15_FLAG_WATCH=3:22,4:23,3:89,9:128,9:133")
    muss(${_n} "${_d}" "[input-script] Abschnitt 3 ab Raum 6030")
    muss(${_n} "${_d}" "[msgtext] room=6000 id=5 \"A device to open the tunnel gate.Will you turn on the switch?\"")
    muss(${_n} "${_d}" "ROOM6000 (3,89) 0 -> 1")
    muss(${_n} "${_d}" "[annette] ROOM6030 Wagen (Cut 6): Zugstart nicht gelaufen")
    muss(${_n} "${_d}" "[story80] ROOM6030 Evt_exec sub02 -> Programm")
    genau_einmal(${_n} "${_d}" "[story80] ROOM6030 Evt_exec sub02 -> Programm")
    muss(${_n} "${_d}" "ROOM6030 Zugstart-Szene statt sub05")
    genau_einmal(${_n} "${_d}" "ROOM6030 Zugstart-Szene statt sub05")
    muss(${_n} "${_d}" "ROOM6030 (9,133) 0 -> 1")
    vor(${_n} "${_d}" "ROOM6000 (3,89) 0 -> 1" "ROOM6030 Zugstart-Szene statt sub05")
    vor(${_n} "${_d}" "[msgtext] room=6030 id=25 " "[msgtext] room=6030 id=28 ")
    vor(${_n} "${_d}" "[msgtext] room=6030 id=19 " "[msgtext] room=6030 id=27 ")
    vor(${_n} "${_d}" "[msgtext] room=6030 id=8 " "[msgtext] room=6030 id=27 ")
    vor(${_n} "${_d}" "[msgtext] room=6030 id=27 " "[msgtext] room=6030 id=28 ")
    vor(${_n} "${_d}" "[msgtext] room=6030 id=28 " "[msgtext] room=6030 id=30 ")
    if(_weg STREQUAL "w")
        set(_ids 0 1 21 2 3 22 4 5 6 7 23 24 8 11 12 25 26 13 14 15 16 17 18 19 27 28 29 30)
        muss(${_n} "${_d}" "[story80] ROOM6030 Evt_exec sub08 -> Programm (644 B)")
        darf_nicht(${_n} "${_d}" "[msgtext] room=6030 id=31 ")
        darf_nicht(${_n} "${_d}" "[msgtext] room=6030 id=40 ")
    else()
        set(_ids 11 31 13 14 15 16 17 18 19 0 1 21 2 3 22 4 5 6 7 23 24 8 40 25 27 28 29 30)
        muss(${_n} "${_d}" "[story80] ROOM6030 Evt_exec sub08 -> Programm (576 B)")
        muss(${_n} "${_d}" "ROOM6030 (9,128) 1 -> 0")
        vor(${_n} "${_d}" "[msgtext] room=6030 id=8 " "[msgtext] room=6030 id=40 ")
        vor(${_n} "${_d}" "[msgtext] room=6030 id=40 " "[msgtext] room=6030 id=25 ")
        darf_nicht(${_n} "${_d}" "[msgtext] room=6030 id=12 ")
        darf_nicht(${_n} "${_d}" "[msgtext] room=6030 id=26 ")
    endif()
    if(_weg STREQUAL "f")
        # Nachtrag nach dem Tor (sub07 @0x01132), dann der Zugstart beim naechsten Betreten
        muss(${_n} "${_d}" "[story80] ROOM6030 Evt_exec sub05 -> Programm (262 B)")
        muss(${_n} "${_d}" "[annette] ROOM6030 Zugstart wartet: Sherry-Nachtrag offen")
        darf_nicht(${_n} "${_d}" "Evt_exec sub09 -> Programm")
        vor(${_n} "${_d}" "ROOM6000 (3,89) 0 -> 1" "[msgtext] room=6030 id=40 ")
        vor(${_n} "${_d}" "[story80] ROOM6030 Evt_exec sub05 -> Programm" "ROOM6030 Zugstart-Szene statt sub05")
        vor(${_n} "${_d}" "ROOM6030 (9,128) 1 -> 0" "ROOM6030 (9,133) 0 -> 1")
    else()
        darf_nicht(${_n} "${_d}" "[story80] ROOM6030 Evt_exec sub05 -> Programm")
        darf_nicht(${_n} "${_d}" "Zugstart wartet")
        if(_weg STREQUAL "r")
            muss(${_n} "${_d}" "[story80] ROOM6030 Evt_exec sub09 -> Programm (238 B)")
            vor(${_n} "${_d}" "ROOM6030 (9,128) 1 -> 0" "ROOM6000 (3,89) 0 -> 1")
        endif()
    endif()
    foreach(_id ${_ids})
        genau_einmal(${_n} "${_d}" "[msgtext] room=6030 id=${_id} ")
    endforeach()
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
    muss(p4e "${_d}" "[story36] ROOM6031: 2 Nachrichten umgeschrieben/neu")
    muss(p4e "${_d}" "[msgtext] room=6031 id=0 \"Elza: Sherry! Is anyone else here?\"")
    muss(p4e "${_d}" "[msgtext] room=6031 id=1 \"Sherry: A policeman! He's fighting off monsters. He wants us to start the train.\"")
    muss(p4e "${_d}" "[msgtext] room=6031 id=2 \"Elza: I'll do it!\"")
    darf_nicht(p4e "${_d}" "Where's Leon")

    # --- P4f: CONTINUE in ROOM30C1 mit (3,0x39): Marvin-Szene ---------------------------------------------
    lauf(p4f "marvin" 30c1 "W60" "2600#30c1" _d)
    muss(p4f "${_d}" "[story80] ROOM30C1 Evt_exec sub03 -> Programm")
    muss(p4f "${_d}" "[msgtext] room=30c1 id=1 \"Elza: Who are you?\"")
    muss(p4f "${_d}" "[msgtext] room=30c1 id=2 \"Officer: Marvin Branagh, RPD.\"")
    muss(p4f "${_d}" "[msgtext] room=30c1 id=3 \"Elza: Are there any others still alive?\"")
    muss(p4f "${_d}" "[msgtext] room=30c1 id=4 \"Marvin: A few. There are other survivors out there.\"")
    muss(p4f "${_d}" "[msgtext] room=30c1 id=5 \"Elza: Then I hope they make it out.\"")
    darf_nicht(p4f "${_d}" "[msgtext] room=30c1 id=6 ")
    darf_nicht(p4f "${_d}" "[msgtext] room=30c1 id=7 ")
    darf_nicht(p4f "${_d}" "[msgtext] room=30c1 id=8 ")
    darf_nicht(p4f "${_d}" "Leon Kennedy")
    darf_nicht(p4f "${_d}" "separated")

    # --- P4g: CONTINUE in ROOM30E1 (Cut 0) mit (3,0xB5): Elza trifft John (Runde 41 ST80-T, A1) ------------
    lauf(p4g "john30e1;cut=0" 30e1 "W30" "1500#30e1" _d)
    muss(p4g "${_d}" "[msgtext] room=30e1 id=2 \"John: This is bad... those monsters have us surrounded!\"")
    muss(p4g "${_d}" "[msgtext] room=30e1 id=3 \"John: ...Damn it, there's no place around here to hide...\"")
    darf_nicht(p4g "${_d}" "bit me")
    darf_nicht(p4g "${_d}" "medical supplies")
else()
    message(FATAL_ERROR "r36_story: TEIL '${TEIL}' unbekannt (leon|elza|zug|torweg_f|torweg_w|torweg_r)")
endif()

file(REMOVE "${_exe_kopie}")
message(STATUS "r36_story[${TEIL}]: alle Laeufe bestanden")
