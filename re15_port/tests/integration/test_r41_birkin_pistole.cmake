# =============================================================================
# ENDKAMPF MIT DER STARTWAFFE, Runde 41 Spur R, Abnahme 1 (analysis/befunde_runde41/R_abnahme_1.md M1-M4).
#
# Ein Spieler muss jede Form mit der Pistole durch reinen Dauerbeschuss besiegen koennen, ohne Feuerpause. Vor der
# Nachbesserung ging das nicht: der Flinch setzte +0x93 Bit 0 am Clip-Ende und loeschte es nie (M2, Original setzt je
# Bild @0x8011a39c und loescht @0x8011a3c0), Birkin blieb bei HP 155 unverwundbar; ueber Dauerfeuer startete die
# Mutation endlos neu (M3, Tor @0x801164e8 schliesst @0x80118054).
#
# Je ein Lauf Leon (ROOM5090) und Elza (ROOM5091), kein Messhaken: Debug-Sprung, Tuer 2, laufen, Zielen und Feuer
# gehalten. RE15_GIVE liefert nur die Patronen. Geprueft aus state.log/debug.log:
#   1  Mutation genau einmal (Sub 6 Phase 0 in einem einzigen Bild)
#   1b Runde 41 Spur GR NB3: Form 1 (EM30) startet mit HP 600 = 0x12c @0x8011690c x RE15_BIRKIN_HP_FAKTOR 2 (PORT-WAHL, Nutzer F1)
#   2  Modelltausch (debug.log "Bahnsteig-Form"), danach kommt die Form mit HP 1000 und laeuft (Clip 10, Clip 5); 1000 = RE2-G4-Ctor
#      `addiu v0,zero,1000` @0x80100678 / `sh v0,342(s1)` @0x8010067c (Runde 41 Spur GR NB3, Nutzer F2; vorher 150)
#   3  Die Form nimmt weiter Schaden: HP faellt unter 100
#   4  Rueckstoss-Szene (Clip 8, HP 1), danach Typ 0x36 mit HP 600 (G5)
#   5  Abnahme 2: der G5 faellt unter derselben Pistole (HP < 0), der Spieler lebt dabei noch. Bis zur Nachbesserung frass er den
#      gesunden Spieler bei F2879 kurz vor dem Ziel (Polaritaet `bgtz` @0x801008e0 / @0x80103e80).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_birkin_pistole.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_birkin_pistole: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_birkin_pistole: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41pistole_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41pistole_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Runde 41 Spur GR NB3: die Formen tragen mehr HP (Form 1 600, Riesenform 1000) - bei einem Pistolenschuss alle 22 Bilder
# brauchen sie 27 + 67 statt 14 + 11 Treffer; Endbild und Zeitlimit wachsen mit. Gemessen (Dossier GR_granate.md N3.5): Leon
# und Elza gleich - Mutation F1274, Riesenform F1455, Rueckstoss F3431, G5 F3594, G5 tot F4706. Endbild 5400 = F4706 + ~700.
if(NOT DEFINED _exit_bild)
    set(_exit_bild 5400)
endif()
# Runde 41 Spur GR NB3: ein STEHENDER Skript-Schuetze ohne Ausweichen und Heilen ueberlebt die laengere Form nicht mehr - gemessen
# ohne Haken (Lauf nb3_pistole_ohne_haken, Leon): Form 1 600 -> Mutation F1274, Form F1455 mit 1000, Spieler tot F2100 bei
# Form-HP 700 (Griff Clip 27 F1608 = -35, Hiebe/Biss danach). Wie im Gorilla-Messlauf Runde 39 (ROOM1141) hebt deshalb der
# Messhaken RE15_R39_MESS_SPIELER_HP=100 (engine/src/gegner_r39.c, KEIN Spielverhalten, nur mit Env) die Spieler-HP am Kopf
# jedes KI-Bildes auf 100; negative HP hebt er nicht (gegner_r39.c r39_mess_spieler_hp `pl->hp < 0` -> return). Das Fressen
# des G5 setzt die HP direkt auf -1 (g5_fressen_beginn, enemy_ai_boss_g5.c): der Polaritaetsfehler aus Abnahme 2 (gesunder
# Spieler gefressen) toetet also weiter, Pruefung 5 behaelt ihre Zaehne. Leer setzen (-D_spieler_hp=) = Lauf ohne Haken.
if(NOT DEFINED _spieler_hp)
    set(_spieler_hp "RE15_R39_MESS_SPIELER_HP=100")
endif()
# Patronen (GR NB3): die Menge ist ein Byte (re15_inventory.h `uint8_t qty`, main.c RE15_GIVE `(uint8_t)qt`) - das fruehere
# 3:400 lud 144 (400 & 0xff); der Skript-Schuetze laedt nicht nach (Kasten 21 bleibt liegen). Mit 144 war die Waffe gemessen
# bei F4356 leer (G5 HP 330, Lauf mit Haken). 3:255 = das Byte-Maximum.
set(_f "")
function(ein_lauf _name _raum)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_extra "")
    if(_raum STREQUAL "5091")
        set(_extra "RE15_TITLE_SHOT_CHAR=1")
    endif()
    # Zielen + Feuer gehalten (R1 + Quadrat), 100 x 4 s; die Pistole schiesst damit alle 22 Bilder.
    set(_skript "A0.1,W1,U8.5")
    foreach(_i RANGE 1 100)
        string(APPEND _skript ",MA4")
    endforeach()
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=500,-9175,2048,0"
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=${_skript}"
        RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log "RE15_GIVE=3:255,21:250" RE15_EQUIP=3
        ${_spieler_hp}
        "RE15_EXIT_AT=${_exit_bild}#${_raum}"
        "${_exe_kopie}")
    set(_g "")
    if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
        set(_f "${_f}\n ${_name}: kein Protokoll (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    if(NOT _dbg MATCHES "Bahnsteig-Form: 27 Meshes")
        string(APPEND _g "\n ${_name}: kein Modelltausch (debug.log)")
    endif()
    # Abnahme 3 N3: der NPC-Zweig des Zeichners verwirft die Rueckseiten der Birkin-Formen (FUN_80027bec, re15_nclip_r41_weg).
    if(NOT _dbg MATCHES "nclip-r41")
        string(APPEND _g "\n ${_name}: Zeichner ruft den Rueckseiten-Verwurf nicht (N3)")
    endif()
    file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX " t=3[06] st=")
    set(_mut 0)
    set(_form_hp -1)
    set(_form1_max -1)
    set(_bild_mut -1)
    set(_bild_form -1)
    set(_bild_rueck -1)
    set(_bild_g5 -1)
    set(_bild_g5_tot -1)
    set(_mg_start "")
    set(_mg_g5_tot "?")
    set(_form_min 9999)
    set(_rueck 0)
    set(_g5 0)
    set(_war_mut 0)
    set(_clip10 0)
    set(_clip5 0)
    set(_g5_tot 0)
    foreach(_z IN LISTS _zeilen)
        string(REGEX MATCH "^F([0-9]+) .* mg=(-?[0-9]+) " _fm "${_z}")
        set(_bild "${CMAKE_MATCH_1}")
        set(_mg_jetzt "${CMAKE_MATCH_2}")
        if(_mg_start STREQUAL "")
            set(_mg_start "${_mg_jetzt}")
        endif()
        string(REGEX MATCH "t=(3[06]) st=([0-9]+) ss1=([0-9]+) ss2=([0-9]+) ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) [^]]*\\] hp=(-?[0-9]+)" _m "${_z}")
        if(NOT _m)
            continue()
        endif()
        set(_typ "${CMAKE_MATCH_1}")
        set(_st "${CMAKE_MATCH_2}")
        set(_s1 "${CMAKE_MATCH_3}")
        set(_s2 "${CMAKE_MATCH_4}")
        set(_mo "${CMAKE_MATCH_5}")
        set(_hp "${CMAKE_MATCH_6}")
        if(_typ STREQUAL "36")
            if(_hp GREATER 0)
                if(NOT _g5)
                    set(_bild_g5 "${_bild}")
                endif()
                set(_g5 1)
            elseif(_g5 AND _hp LESS 0)
                if(NOT _g5_tot)
                    set(_bild_g5_tot "${_bild}")
                    set(_mg_g5_tot "${_mg_jetzt}")
                endif()
                set(_g5_tot 1)
            endif()
            continue()
        endif()
        if(NOT _war_mut AND _hp GREATER _form1_max)
            set(_form1_max "${_hp}")
        endif()
        if(_st EQUAL 1 AND _s1 EQUAL 6 AND _s2 EQUAL 0)
            math(EXPR _mut "${_mut} + 1")
            if(_bild_mut LESS 0)
                set(_bild_mut "${_bild}")
            endif()
        endif()
        if(_st EQUAL 1 AND _s1 EQUAL 6)
            set(_war_mut 1)
        elseif(_war_mut EQUAL 1 AND _st EQUAL 1 AND _s1 EQUAL 0 AND _mo EQUAL 10)
            set(_clip10 1)
            if(_form_hp LESS 0)
                set(_form_hp "${_hp}")
                set(_bild_form "${_bild}")
            endif()
        endif()
        if(_clip10 AND _mo EQUAL 5)
            set(_clip5 1)
        endif()
        if(_clip10 AND _st EQUAL 1 AND _hp LESS _form_min)
            set(_form_min "${_hp}")
        endif()
        if(_clip10 AND _st EQUAL 2 AND _mo EQUAL 8 AND _hp EQUAL 1)
            if(NOT _rueck)
                set(_bild_rueck "${_bild}")
            endif()
            set(_rueck 1)
        endif()
    endforeach()
    if(NOT _mut EQUAL 1)
        string(APPEND _g "\n ${_name}: Mutation ${_mut}-mal gestartet, erwartet 1")
    endif()
    if(NOT _form1_max EQUAL 600)
        string(APPEND _g "\n ${_name}: Form 1 hat hoechstens HP ${_form1_max}, erwartet 600 (0x12c @0x8011690c x 2, GR NB3)")
    endif()
    if(NOT _form_hp EQUAL 1000)
        string(APPEND _g "\n ${_name}: Form kommt mit HP ${_form_hp}, erwartet 1000 (G4-Ctor @0x8010067c, GR NB3)")
    endif()
    if(NOT _clip5)
        string(APPEND _g "\n ${_name}: die Form laeuft nicht (kein Clip 5 nach dem Auftritt)")
    endif()
    if(_form_min GREATER_EQUAL 100)
        string(APPEND _g "\n ${_name}: Form nimmt keinen Schaden (HP-Minimum ${_form_min})")
    endif()
    if(NOT _rueck)
        string(APPEND _g "\n ${_name}: keine Rueckstoss-Szene")
    endif()
    if(NOT _g5)
        string(APPEND _g "\n ${_name}: kein G5 (Typ 0x36)")
    endif()
    if(NOT _g5_tot)
        string(APPEND _g "\n ${_name}: G5 faellt unter der Pistole nicht bis F${_exit_bild}")
    endif()
    # Patronen am Ende: Magazin (mg=) und Vorrat (Zeile "Patronen" fehlt im Log; Vorrat = RE15_GIVE 21:250 minus Nachladen).
    file(STRINGS "${WORKDIR}/state.log" _letzte REGEX "^F[0-9]+ .* mg=")
    list(LENGTH _letzte _nl)
    set(_mg "?")
    if(_nl GREATER 0)
        math(EXPR _il "${_nl} - 1")
        list(GET _letzte ${_il} _lz)
        if(_lz MATCHES " mg=(-?[0-9]+) ")
            set(_mg "${CMAKE_MATCH_1}")
        endif()
    endif()
    message(STATUS "r41_birkin_pistole ${_name}: Mutationen ${_mut} (F${_bild_mut}), Form 1 HP max ${_form1_max}, Form-HP ${_form_hp} "
                   "(F${_bild_form}) bis ${_form_min}, Rueckstoss ${_rueck} (F${_bild_rueck}), G5 ${_g5} (F${_bild_g5}), G5 tot ${_g5_tot} "
                   "(F${_bild_g5_tot}), Patronen in der Waffe ${_mg_start} -> ${_mg_g5_tot} beim G5-Tod, am Ende ${_mg}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

ein_lauf(leon 5090)
ein_lauf(elza 5091)
file(REMOVE "${_exe_kopie}")

if(_f)
    message(FATAL_ERROR "r41_birkin_pistole: FEHLER${_f}")
endif()
message(STATUS "r41_birkin_pistole: alle Pruefungen gruen")
