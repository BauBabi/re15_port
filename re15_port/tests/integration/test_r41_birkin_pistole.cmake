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
#   2  Modelltausch (debug.log "Bahnsteig-Form"), danach kommt die Form mit HP 150 und laeuft (Clip 10, Clip 5)
#   3  Die Form nimmt weiter Schaden: HP faellt unter 100
#   4  Rueckstoss-Szene (Clip 8, HP 1), danach Typ 0x36 mit HP 600 (G5)
#   5  Abnahme 2: der G5 faellt unter derselben Pistole (HP < 0), der Spieler lebt dabei noch. Bis zur Nachbesserung frass er den
#      gesunden Spieler bei F2879 kurz vor dem Ziel (Polaritaet `bgtz` @0x801008e0 / @0x80103e80).
#
# G5-AUFTRITT NACHBESSERUNG 2 (analysis/befunde_runde41/G5_auftritt.md Abschnitt 8): nach der G5-Szene geht der Spieler ZURUECK
# (Taste unten, 12 s ab F2548; wirkt erst mit der Freigabe am Szenenende) und schiesst dann weiter. Der Test fuehrte den Spieler bis
# Nachbesserung 1 im G5-Kampf STEHEND - das trug nur, weil ihn das Szenenende 715 neben der RE2-Linie liess (z -22855 statt Pos_set
# -23570 @0x0196C `32 00 02 3a 00 00 ee a3`) und die Arme ihn danach aus dem Bissbogen schoben. Auf RE2s Linie beisst der G5 jeden
# Spieler, der innerhalb 6000 vor ihm steht (`sltiu 0x1770` @0x80103e08, Bogen 128 @0x80103e2c-3c, 40 Schaden a0 @0x80104118), und
# frisst ihn bei HP 0 (@0x80103e78-ec4) - Halbierung (Dossier 8.1): jede Kombination der NB1-Aenderungen ausser "alle aus" endet so.
# Der Zug bringt den G5 nur bis X < 12000 (@0x80100fd0-e4), der Biss-Vorstoss nur bis 12000 (@0x80104060-94): wer hinter X 18000
# steht, wird nie gebissen. Der Spieltest misst also weiter "Pistole besiegt den G5", mit dem Abstand, den RE2 dafuer vorsieht.
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

set(_f "")
function(ein_lauf _name _raum)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_extra "")
    if(_raum STREQUAL "5091")
        set(_extra "RE15_TITLE_SHOT_CHAR=1")
    endif()
    # Zielen + Feuer gehalten (R1 + Quadrat) bis F2548, dann 12 s zurueck (D), danach wieder Zielen + Feuer; die Pistole schiesst
    # alle 22 Bilder. Gemessen (Nachbesserung 2): Szenenende F2775, Spieler 13873 -> 23113, G5 600 -> 15 bis F3774 ohne Treffer am
    # Spieler, 40 Schuesse a 15.
    set(_skript "A0.1,W1,U8.5")
    foreach(_i RANGE 1 18)
        string(APPEND _skript ",MA4")
    endforeach()
    string(APPEND _skript ",D12")
    # Nachbesserung G5 3: nach jedem 4-s-Abschnitt R1 kurz los (W0.2), damit das naechste Heben neu zielt. Die Zielhilfe laeuft nur im
    # Heben (Latch FUN_8003703c rundum, Radius 0x7530 @0x80032fac, Drehung FUN_8001a8f8 0xC8 @0x80032fe0-ec, player_common.c RAISE).
    # Gemessen (Lauf mit RE2-Vorkampf-Puls, eine Zufallsziehung @0x80104544 mehr): ein Arm peitscht den Spieler bei x 23113 (F3576 HP
    # 45 -> 38, F3618 -> 31), der Blick dreht 2055 -> 640; bei durchgehend gehaltenem R1 zielte er danach bis F5400 ins Leere, G5 blieb
    # bei 150 HP. NB2 8.9 hatte genau diese Annahme ("kein Treffer bei 23113") als ungemessen offen gelassen.
    foreach(_i RANGE 1 60)
        string(APPEND _skript ",MA4,W0.2")
    endforeach()
    re15_start_spiel(_rv 420
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=500,-9175,2048,0"
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=${_skript}"
        RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log "RE15_GIVE=3:400,21:250" RE15_EQUIP=3
        "RE15_EXIT_AT=5400#${_raum}"   # G5-Auftritt NB2: Szene 1162 + Vorlauf + Rueckzug, G5 stirbt ~F3800 (vorher 4200)
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
    set(_form_min 9999)
    set(_rueck 0)
    set(_g5 0)
    set(_war_mut 0)
    set(_clip10 0)
    set(_clip5 0)
    set(_g5_tot 0)
    set(_pl_hp_tod -999)
    foreach(_z IN LISTS _zeilen)
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
                set(_g5 1)
            elseif(_g5 AND _hp LESS 0 AND NOT _g5_tot)
                set(_g5_tot 1)
                # Spieler-HP im selben Bild (state.log PL(x,z,rot=..,hp=..)): er muss den Tod des G5 erleben
                if(_z MATCHES "PL\\(-?[0-9]+,-?[0-9]+,rot=[0-9]+,hp=(-?[0-9]+)\\)")
                    set(_pl_hp_tod "${CMAKE_MATCH_1}")
                endif()
            endif()
            continue()
        endif()
        if(_st EQUAL 1 AND _s1 EQUAL 6 AND _s2 EQUAL 0)
            math(EXPR _mut "${_mut} + 1")
        endif()
        if(_st EQUAL 1 AND _s1 EQUAL 6)
            set(_war_mut 1)
        elseif(_war_mut EQUAL 1 AND _st EQUAL 1 AND _s1 EQUAL 0 AND _mo EQUAL 10)
            set(_clip10 1)
            if(_form_hp LESS 0)
                set(_form_hp "${_hp}")
            endif()
        endif()
        if(_clip10 AND _mo EQUAL 5)
            set(_clip5 1)
        endif()
        if(_clip10 AND _st EQUAL 1 AND _hp LESS _form_min)
            set(_form_min "${_hp}")
        endif()
        if(_clip10 AND _st EQUAL 2 AND _mo EQUAL 8 AND _hp EQUAL 1)
            set(_rueck 1)
        endif()
    endforeach()
    if(NOT _mut EQUAL 1)
        string(APPEND _g "\n ${_name}: Mutation ${_mut}-mal gestartet, erwartet 1")
    endif()
    if(NOT _form_hp EQUAL 150)
        string(APPEND _g "\n ${_name}: Form kommt mit HP ${_form_hp}, erwartet 150 (Ctor-Teil, 0x96 @0x80118094)")
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
        string(APPEND _g "\n ${_name}: G5 faellt unter der Pistole nicht bis F5400")
    elseif(NOT _pl_hp_tod GREATER 0)
        string(APPEND _g "\n ${_name}: Spieler-HP ${_pl_hp_tod} im Bild, in dem der G5 faellt (der Spieler muss leben)")
    endif()
    message(STATUS "r41_birkin_pistole ${_name}: Mutationen ${_mut}, Form-HP ${_form_hp} bis ${_form_min}, Rueckstoss ${_rueck}, G5 ${_g5}, G5 tot ${_g5_tot}, Spieler-HP dabei ${_pl_hp_tod}")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

ein_lauf(leon 5090)
ein_lauf(elza 5091)
file(REMOVE "${_exe_kopie}")

if(_f)
    message(FATAL_ERROR "r41_birkin_pistole: FEHLER${_f}")
endif()
message(STATUS "r41_birkin_pistole: alle Pruefungen gruen")
