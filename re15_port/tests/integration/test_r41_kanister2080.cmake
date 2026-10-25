# =============================================================================
# Runde 41 Spur G "kanister2080" — die echte exe (Dossier analysis/befunde_runde41/G_kanister2080.md).
#
# Lauf "ankunft"  (Leon) echter Tuerweg 2090 -> 2080 (ROOM2090 Tuer Slot 0 @0x0996, DOOR16) mit Bit 110 (Spur F: 2090-Szene
#                 gesehen) und (3,245), Ton gemischt (RE15_AUDIO_CAP_SYNC, RE15_SE_DEBUG): beim ersten Betreten ERST der Knall
#                 (Nachtrag 17: Knall-Bank Satz 0 = RE2 ROOM1060 Se_on 0x0a @0x0F48, zwei Lagen Tone 8/9 center 91 11025 Hz
#                 direkt hinter der Ausloese-Zeile), DANN die 8 Zombies (Sce_em_set Typ 0x16 Etage 8, y -14400), Bit 114.
# Lauf "ebene"    (Leon) Nachtrag 20: Leon oben am Treppenabsatz (-16000,-16700, Band 8, Blick nach Westen), die Zombies laufen
#                 auf ihn zu, er steigt die Treppe @0x0BE2 hinab ([stair] START dir=DOWN 8 -> 5) und wartet unten: in JEDER
#                 Messzeile "Ebene" stehen alle Zombies auf y -14400 und keiner westlich der Absatzwand @0x05C4 (x -16650);
#                 die Verfolgung reicht bis an den Absatz (x <= -15000), Leon steht danach unten (y -9000).
# Lauf "kanister" (Leon) ROOM2080 OHNE Bit 110 in Cut 5 (= Gas_Can.bmp): kein Zombie, der Kanister steht an der Reling - die
#                 Messzeile des Zeichners traegt Lage (-6745,-14400,-17625) und den Bildkasten in Cut 5, der die Markenmitte
#                 (167,156) enthaelt und vom Fuss (y ~155) bis ueber y 80 reicht.
# Lauf "szene"    (Leon) ROOM2090 mitten im Kampf (RE15_GB_TEST=5), Bit 110, Messhaken RE15_K2080_GATOR_HP=24990 (Schwelle
#                 HP < 25000 mit EINEM echten Pistolentreffer statt 43): Ausloeser -> ROOM2080 (Marvin 0x40, 8 Zombies) ->
#                 Tuer-Ton (DOOR06 Satz 1: se=1 vag 2 center 79 16397 Hz) -> 8 Kopfschuesse mit 8 x "Kopf ab" (RE2
#                 FUN_801092C4) -> Aufheben -> Wurf (RE2 ROOM7020 sub13) -> Aufschlag (KANISTER2080.VBS Satz 0: center 95
#                 8193 Hz) -> Rueckkehr ROOM2090 (Tot-Bits 8 von 8) -> Kanister im Becken; Schnitte in 2080 in der Folge
#                 2 -> 5 -> 7 (state.log cam=), Nachricht 42 im Bild (state.log msg id=42), keiner der Zombies auf der Treppe.
# Lauf "elza"     dasselbe mit Elza (Spielerwahl RE15_PSELECT_AUTO_SWITCH): ROOM2091 -> ROOM2081, John 0x49.
# Nachbesserung 1: Aufheben = Hinknien Loco c11 (ROOM11B0 sub06 @0x01650, Plc_motion(1,11) zweimal, kein Loco c2 = Klettern, kein
#                 Eigen c0 = RUN-Gait); Orchestrator-Entscheide FG-E3/FG-E4: der Fall laeuft in ROOM2090/2091 Cut 12 (state.log
#                 cam=12 nach dem Wurf), der Kanister liegt an der Aufschlagstelle. RE15_GB_TEST=5 dient hier nur als schneller
#                 Ausloeser der Szene; den weiteren Kampf OHNE Testhaken misst integration_r41_kanister2080_kampf.
# Nachbesserung 2: N1 die Zeile 42 steht ihre VOLLE Zeit (state.log msg a=1 id=42 >= 97 Bilder, Messzeile "Zeile 42 stand"), das
#                 Loslassen kommt erst danach (RE2 Wsleep/Wsleeping @0x079A/@0x079B: "Wsleeping frei" vor "Wurf: Start"); N2 die
#                 Gier des Werfers bleibt in Cut 5 vom Hinknien bis zum Schnitt gleich, im ersten Bild von Cut 7 steht die
#                 Wurf-Gier (RE2 Dir_set @0x0748 nach Cut_chg 7 @0x0726); N3 Zeile "Wurfrichtung" mit der Wahl des Landepunkts.
# Nachbesserung 3: G-E2 im ersten Bild von Cut 7 steht die der Kamera zugewandte Gier (Zeile "Werfer Cut 7", Versatz wie Ada
#                 -111 +- 12, frontal >= 4023/4096), die Wurfrichtung bleibt die Wahl des Landepunkts; Nachtrag 28 die Raum-ESP
#                 ROOM40A0 wird in 2090/2091 umgelegt wie FUN_8001bd38 (7 Zeilen "Raumbank") und registriert, beim Wechsel nach
#                 2080/2081 wieder ausgetragen (GLAS1090.ESP).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_kanister2080.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_kanister2080: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_kanister2080: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
# EIGENER EXE-NAME (Muster test_r34n_b_cursor.cmake): nur die eigene Kopie wird geloescht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r41g_${_lauf_id}.exe")
if(NOT WIN32)
    set(_exe_kopie "${_exe_dir}/re15_pc_r41g_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(r41g_fehler _text)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_kanister2080: ${_text}")
endfunction()

# Die Zeile DIREKT hinter der (jeder) Ausloese-Zeile des Moduls muss die "[se] Stimme:"-Zeile des erwarteten Satzes sein
# (audio_pc.c se_play_layers, RE15_SE_DEBUG=1; gleicher Aufruf, gleicher Strom). Gezaehlt wird, wie oft das zutrifft.
function(r41g_ton_folgt _datei _ausl _folge _soll _was)
    file(STRINGS "${_datei}" _z REGEX "(${_ausl})|(\\[se\\] Stimme: )")
    set(_n 0)
    set(_warte 0)
    foreach(_l IN LISTS _z)
        if(_warte)
            if(_l MATCHES "${_folge}")
                math(EXPR _n "${_n} + 1")
            endif()
            set(_warte 0)
        endif()
        if(_l MATCHES "${_ausl}")
            set(_warte 1)
        endif()
    endforeach()
    if(NOT _n EQUAL ${_soll})
        r41g_fehler("${_was}: ${_n} x '${_folge}' direkt hinter '${_ausl}' statt ${_soll}")
    endif()
endfunction()

set(_basisenv RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore)
set(_tonenv RE15_NO_INTRO=1 RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore)
set(_leon RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
set(_elza RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
set(_ebene_ok "auf der Treppe \\(x < -16650\\) 0, nicht auf y -14400: 0 ")

# ---------------------------------------------------------------- ankunft (echter Tuerweg 2090 -> 2080, Ton gemischt)
set(WORKDIR "${_basis}/ankunft")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 300 ${_tonenv} ${_leon}
    RE15_DEBUG_JUMP=2090@250 "RE15_SET_FLAG=9:110,3:245" RE15_KEIN_GATOR=1 "RE15_PLAYER_POS=-8900,-25244,2048"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W1.0,A0.2,W6.0"
    RE15_SPAWN_DIAG=1 "RE15_EXIT_AT=90#2080" "${_exe_kopie}")
file(REMOVE "${WORKDIR}/cap.raw")
if(NOT _rv EQUAL 0)
    r41g_fehler("Lauf ankunft exe exit=${_rv}")
endif()
file(STRINGS "${WORKDIR}/debug.log" _td REGEX "\\[tuer\\] Sequenz Archiv [0-9]+ DOOR16 ")
if(NOT _td)
    r41g_fehler("Lauf ankunft: kein Tuerweg 2090 -> 2080 (Leiter DOOR16, Tuer Slot 0 @ROOM2090 0x0996)")
endif()
file(STRINGS "${WORKDIR}/debug.log" _sp REGEX "\\[k2080\\] F[0-9]+ ROOM2080 nach Bit 110: 8 lebende Zombies \\(Tot-Bits 236\\.\\.243\\)")
if(NOT _sp)
    r41g_fehler("Lauf ankunft: keine Zeile 'ROOM2080 nach Bit 110: 8 lebende Zombies'")
endif()
file(STRINGS "${WORKDIR}/debug.log" _kn REGEX
     "\\[k2080\\] F[0-9]+ Knall \\(Knall-Bank Satz 0 = RE2 ROOM1060 Se_on 0x0a @0x0F48, Tuerknall der Irons-/Elza-Szene\\) - erstes Betreten nach Bit 110, Bit 114 = 1")
if(NOT _kn)
    r41g_fehler("Lauf ankunft: kein Knall beim ersten Betreten")
endif()
# beide Lagen des Knalls (Tone 8/9 fein 0/1, center 91, 11025 Hz = RE2 ROOM1060 VAG 5) direkt hinter der Ausloese-Zeile
set(_ausl "\\[k2080\\] F[0-9]+ Knall: Knall-Bank 19976 B in der Tuerbank, Satz 0 ")
r41g_ton_folgt("${WORKDIR}/debug.log" "${_ausl}"
    "\\[se\\] Stimme: se=0 layer=0 vag=0 note=67 fine=0 center=91 vol=127 pitch=0x400 \\(11025 Hz\\)" 1 "Lauf ankunft Knall Lage 0")
file(STRINGS "${WORKDIR}/debug.log" _kl REGEX "\\[se\\] Stimme: se=0 layer=1 vag=0 note=67 fine=1 center=91 vol=127 pitch=0x400 \\(11025 Hz\\)")
if(NOT _kl)
    r41g_fehler("Lauf ankunft: zweite Lage des Knalls fehlt")
endif()
# ERST der Knall, DANN die Zombies (Reihenfolge im Strom)
file(STRINGS "${WORKDIR}/debug.log" _rf REGEX "(${_ausl})|(\\[spawn-diag\\] Sce_em_set type=0x16)")
list(GET _rf 0 _erste)
if(NOT _erste MATCHES "Knall: Knall-Bank")
    r41g_fehler("Lauf ankunft: Zombies vor dem Knall (${_erste})")
endif()
file(STRINGS "${WORKDIR}/debug.log" _em REGEX "\\[spawn-diag\\] Sce_em_set type=0x16 behavior=0x02 slot=[0-7] pos=\\(-?[0-9]+,-14400,-?[0-9]+\\)")
list(LENGTH _em _n_em)
if(NOT _n_em EQUAL 8)
    r41g_fehler("Lauf ankunft: ${_n_em} Zombie-Spawns auf Band 8 statt 8")
endif()
message(STATUS "r41_kanister2080 ankunft: Tuerweg 2090 -> 2080, Knall (2 Lagen) vor 8 Zombies Typ 0x16 auf y -14400")

# ---------------------------------------------------------------- ebene (Nachtrag 20: oberhalb der Treppe, kein Abstieg)
set(WORKDIR "${_basis}/ebene")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 300 ${_basisenv} ${_leon}
    RE15_DEBUG_JUMP=2080@250 "RE15_SET_FLAG=9:110,3:245" "RE15_PLAYER_POS=-16000,-16700,2048,8"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W2.0,A0.2,W20.0" RE15_R39_MESS_SPIELER_HP=100
    RE15_STAIR_DBG=1 "RE15_EXIT_AT=720#2080" "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    r41g_fehler("Lauf ebene exe exit=${_rv}")
endif()
file(STRINGS "${WORKDIR}/stair_dbg.log" _ab REGEX "\\[stair\\] START dir=DOWN cur_band=8 target_band=5")
if(NOT _ab)
    r41g_fehler("Lauf ebene: kein Abstieg ueber die Treppe @0x0BE2 (Band 8 -> 5)")
endif()
file(STRINGS "${WORKDIR}/debug.log" _eb REGEX "\\[k2080\\] F[0-9]+ Ebene: ")
file(STRINGS "${WORKDIR}/debug.log" _eb_ok REGEX "\\[k2080\\] F[0-9]+ Ebene: 8 Zombies x -?[0-9]+\\.\\.-?[0-9]+ y -14400\\.\\.-14400, ${_ebene_ok}")
list(LENGTH _eb _n_eb)
list(LENGTH _eb_ok _n_eb_ok)
if(_n_eb LESS 20 OR NOT _n_eb EQUAL _n_eb_ok)
    r41g_fehler("Lauf ebene: ${_n_eb_ok} von ${_n_eb} Ebene-Zeilen ohne Zombie auf der Treppe / ausserhalb y -14400 (mind. 20)")
endif()
set(_xmin 0)
set(_unten 0)
foreach(_l IN LISTS _eb)
    if(_l MATCHES "Zombies x (-?[0-9]+)\\.\\.")
        if(CMAKE_MATCH_1 LESS _xmin)
            set(_xmin ${CMAKE_MATCH_1})
        endif()
    endif()
    if(_l MATCHES "Spieler \\(-2[0-9][0-9][0-9][0-9],-9000,")
        math(EXPR _unten "${_unten} + 1")
    endif()
endforeach()
if(_xmin GREATER -15000 OR _unten LESS 10)
    r41g_fehler("Lauf ebene: Verfolgung bis x ${_xmin} (soll <= -15000), Leon unten in ${_unten} Zeilen (soll >= 10)")
endif()
message(STATUS "r41_kanister2080 ebene: Abstieg 8 -> 5, ${_n_eb} Ebene-Zeilen ohne Zombie auf der Treppe, Verfolgung bis x ${_xmin}")

# ---------------------------------------------------------------- kanister
set(WORKDIR "${_basis}/kanister")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 300 ${_basisenv} ${_leon}
    RE15_DEBUG_JUMP=2080@250 "RE15_PLAYER_POS=-12000,-24000,0,8" RE15_FORCE_CUT=5 RE15_SPAWN_DIAG=1
    "RE15_EXIT_AT=40#2080" "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    r41g_fehler("Lauf kanister exe exit=${_rv}")
endif()
file(STRINGS "${WORKDIR}/debug.log" _em REGEX "\\[spawn-diag\\] Sce_em_set type=0x16")
if(_em)
    r41g_fehler("Lauf kanister: Zombies ohne Bit 110")
endif()
file(STRINGS "${WORKDIR}/debug.log" _kz REGEX "\\[k2080\\] F[0-9]+ Kanister pos=\\(-6745,-14400,-17625\\) Cut 5 Kasten=")
if(NOT _kz)
    r41g_fehler("Lauf kanister: keine Zeichner-Zeile des Kanisters in Cut 5")
endif()
list(GET _kz -1 _k)
if(NOT _k MATCHES "Kasten=\\((-?[0-9]+)\\.\\.(-?[0-9]+),(-?[0-9]+)\\.\\.(-?[0-9]+)\\) Dreiecke ([0-9]+)")
    r41g_fehler("Lauf kanister: Zeile unlesbar: ${_k}")
endif()
set(_x0 ${CMAKE_MATCH_1})
set(_x1 ${CMAKE_MATCH_2})
set(_y0 ${CMAKE_MATCH_3})
set(_y1 ${CMAKE_MATCH_4})
set(_tri ${CMAKE_MATCH_5})
if(_x0 GREATER 167 OR _x1 LESS 167 OR _y1 LESS 150 OR _y0 GREATER 80 OR _tri LESS 30)
    r41g_fehler("Lauf kanister: Kasten ${_x0}..${_x1},${_y0}..${_y1} (${_tri} Dreiecke) deckt die Marke (167,156) nicht vom Fuss bis y 80")
endif()
message(STATUS "r41_kanister2080 kanister: Cut 5 Kasten ${_x0}..${_x1},${_y0}..${_y1}, ${_tri} Dreiecke, keine Zombies")

# ---------------------------------------------------------------- szene (Leon) / elza
foreach(_wer leon elza)
    set(WORKDIR "${_basis}/szene_${_wer}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    if(_wer STREQUAL "leon")
        set(_wahl ${_leon})
        set(_r80 2080)
        set(_r90 2090)
        set(_npc "Marvin 0x40")
        set(_env ${_tonenv})
    else()
        set(_wahl ${_elza})
        set(_r80 2081)
        set(_r90 2091)
        set(_npc "John 0x49")
        set(_env ${_basisenv})
    endif()
    re15_start_spiel(_rv 600 ${_env} ${_wahl}
        RE15_DEBUG_JUMP=2090@250 "RE15_SET_FLAG=9:110,3:245" RE15_GB_TEST=5 "RE15_PLAYER_POS=6500,-25000,3072"
        "RE15_GIVE=3:15" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W2.0,M0.3,MA0.1,M0.3,MA0.1,W1.0"
        RE15_STATE_LOG=state.log "RE15_EXIT_AT=120#${_r90}" RE15_GATOR_FX_LOG=1 "${_exe_kopie}")
    file(REMOVE "${WORKDIR}/cap.raw")
    if(NOT _rv EQUAL 0)
        r41g_fehler("Lauf szene_${_wer} exe exit=${_rv}")
    endif()
    if(_wer STREQUAL "leon")
        r41g_ton_folgt("${WORKDIR}/debug.log" "\\[k2080\\] F[0-9]+ Tuer-Ton: DOOR06-Tonteil 19608 B in der Tuerbank, Satz 1 "
            "\\[se\\] Stimme: se=1 layer=0 vag=2 note=61 fine=57 center=79 vol=127 pitch=0x5f3 \\(16397 Hz\\)" 1 "Lauf szene_leon Tuer-Ton")
        r41g_ton_folgt("${WORKDIR}/debug.log" "\\[k2080\\] F[0-9]+ Aufschlag-Ton KANISTER2080\\.VBS Satz 0"
            "\\[se\\] Stimme: se=0 layer=0 vag=0 note=65 fine=57 center=95 vol=127 pitch=0x2f9 \\(8193 Hz\\)" 1 "Lauf szene_leon Aufschlag")
    endif()
    foreach(_muster
            "AUSLOESER geschwaecht .*-> ROOM${_r80}"
            "ROOM${_r80} SZENE \\(${_npc}, 8 lebende Zombies"
            "Tuer-Ton DOOR06 Satz 1 \\(Door_exit @0x800141f0\\) - Szene"
            "Kanister aufgehoben \\(Loco c11 Bild 24, Hinknien ROOM11B0 sub06 @0x01650\\)"
            "Wurf: Start .* -> Landung \\(-?[0-9]+,-?[0-9]+,-?[0-9]+\\) nach [0-9]+ Bildern"
            "Aufschlag im Wasser"
            "ROOM${_r90} RUECKKEHR: .*Zone-7-Tot-Bits 236\\.\\.243 gesetzt: 8 von 8"
            "ROOM${_r90} Kanister im Becken \\(-?[0-9]+,-1200,-?[0-9]+\\) = Aufschlagstelle")
        file(STRINGS "${WORKDIR}/debug.log" _z REGEX "\\[k2080\\] F[0-9]+ ${_muster}")
        if(NOT _z)
            r41g_fehler("Lauf szene_${_wer}: keine Zeile '${_muster}'")
        endif()
    endforeach()
    # Nachbesserung 1 (M1, Nachtrag 24b): Hinknien + Aufstehen = Plc_motion(1,11) zweimal, kein Loco c2 (Klettern) und kein
    # Eigen c0 (RUN-Gait) im Szenen-Programm
    file(STRINGS "${WORKDIR}/debug.log" _kn REGEX "\\[scd\\] Plc_motion\\(entity=1, motion=11, ")
    list(LENGTH _kn _n_kn)
    file(STRINGS "${WORKDIR}/debug.log" _kl REGEX "\\[scd\\] Plc_motion\\(entity=1, motion=2, |\\[scd\\] Plc_motion\\(entity=2, motion=0, ")
    if(NOT _n_kn EQUAL 2 OR _kl)
        r41g_fehler("Lauf szene_${_wer}: ${_n_kn} x Plc_motion(1,11) statt 2 bzw. Kletter-/Lauf-Clip im Programm (${_kl})")
    endif()
    file(STRINGS "${WORKDIR}/debug.log" _ka REGEX "\\[k2080\\] F[0-9]+ Kopf ab: Slot [0-9]+ \\(Part 8 Flagwort 0x4[A-F]")
    list(LENGTH _ka _n_ka)
    file(STRINGS "${WORKDIR}/debug.log" _sch REGEX "\\[k2080\\] F[0-9]+ Schuss [0-9]+: Treffer Slot [0-9]+ .*hp=-1 state=3 zeile=5")
    list(LENGTH _sch _n_sch)
    if(NOT _n_ka EQUAL 8 OR NOT _n_sch EQUAL 8)
        r41g_fehler("Lauf szene_${_wer}: ${_n_sch} Kopfschuesse (zeile 5) und ${_n_ka} x Kopf ab statt je 8")
    endif()
    file(STRINGS "${WORKDIR}/debug.log" _eb REGEX "\\[k2080\\] F[0-9]+ Ebene: ")
    file(STRINGS "${WORKDIR}/debug.log" _eb_ok REGEX "\\[k2080\\] F[0-9]+ Ebene: [0-9]+ Zombies x -?[0-9]+\\.\\.-?[0-9]+ y -14400\\.\\.-14400, ${_ebene_ok}")
    list(LENGTH _eb _n_eb)
    list(LENGTH _eb_ok _n_eb_ok)
    if(_n_eb EQUAL 0 OR NOT _n_eb EQUAL _n_eb_ok)
        r41g_fehler("Lauf szene_${_wer}: ${_n_eb_ok} von ${_n_eb} Ebene-Zeilen ohne Zombie auf der Treppe / ausserhalb y -14400")
    endif()
    # Schnittfolge in 2080 (Spieler auf dem Parkplatz -23200,-16700): erste Bilder je Cut
    file(STRINGS "${WORKDIR}/state.log" _st REGEX "PL\\(-23200,-16700,")
    set(_folge "")
    set(_letzt "")
    set(_msg42 0)
    foreach(_l IN LISTS _st)
        if(_l MATCHES " cam=([0-9]+) ")
            set(_c ${CMAKE_MATCH_1})
            if(NOT _c STREQUAL _letzt)
                string(APPEND _folge "${_c} ")
                set(_letzt ${_c})
            endif()
        endif()
        if(_l MATCHES "id=42 ")
            set(_msg42 1)
        endif()
    endforeach()
    if(NOT _folge MATCHES "^2 5 7 ")
        r41g_fehler("Lauf szene_${_wer}: Schnittfolge in ${_r80} '${_folge}' statt '2 5 7'")
    endif()
    if(NOT _msg42)
        r41g_fehler("Lauf szene_${_wer}: Nachricht 42 nie im Bild (state.log msg id=42)")
    endif()
    # ---- Nachbesserung 2 (N1): volle Standzeit der Zeile 42, Loslassen erst danach ----
    file(STRINGS "${WORKDIR}/state.log" _z42 REGEX "PL\\(-23200,-16700,.* msg\\(a=1 fsm=[0-9]+ id=42 ")
    list(LENGTH _z42 _n_z42)
    # Volle Standzeit = re15_msg_compute_duration des Textes (Leon/Elza je 99 Bilder, Abnahme 0 p3r_leon F569..F667 und unit texte);
    # 97 laesst zwei Bilder fuer die Lage des ersten/letzten Bildes im Protokoll. Vorher (Abnahme 1): 52.
    if(_n_z42 LESS 97)
        r41g_fehler("Lauf szene_${_wer}: Zeile 42 nur ${_n_z42} Bilder im Bild statt ihrer vollen Zeit (N1)")
    endif()
    file(STRINGS "${WORKDIR}/debug.log" _wf REGEX "\\[k2080\\] F[0-9]+ (Zeile 42 stand F[0-9]+\\.\\.F[0-9]+ \\([0-9]+ Bilder\\)|Wsleeping frei|Wurf: Start)")
    set(_wfolge "")
    foreach(_l IN LISTS _wf)
        if(_l MATCHES "Zeile 42 stand")
            string(APPEND _wfolge "Z")
        elseif(_l MATCHES "Wsleeping frei")
            string(APPEND _wfolge "W")
        elseif(_l MATCHES "Wurf: Start")
            string(APPEND _wfolge "R")
        endif()
    endforeach()
    if(NOT _wfolge MATCHES "^ZW+R$" AND NOT _wfolge MATCHES "^ZWR")
        r41g_fehler("Lauf szene_${_wer}: Folge Zeilenende/Wsleeping/Loslassen '${_wfolge}' statt 'ZWR' (N1, RE2 @0x079A/@0x079B)")
    endif()
    # ---- Nachtrag 26 (RE2-Kippen): nach dem Griff kippt der Kanister ueber 23 Bilder (Bild 1 Bruch 0 ... Bild 24 Bruch 4096) ----
    file(STRINGS "${WORKDIR}/debug.log" _kp REGEX "\\[k2080\\] F[0-9]+ (Kanister aufgehoben|Kippen Bild [0-9]+: Bruch [0-9]+/4096)")
    set(_kfolge "")
    foreach(_l IN LISTS _kp)
        if(_l MATCHES "Kanister aufgehoben")
            string(APPEND _kfolge "A ")
        elseif(_l MATCHES "Kippen Bild ([0-9]+): Bruch ([0-9]+)/4096")
            string(APPEND _kfolge "${CMAKE_MATCH_1}:${CMAKE_MATCH_2} ")
        endif()
    endforeach()
    if(NOT _kfolge MATCHES "^A 1:0 6:240 12:1056 18:2448 24:4096 $")
        r41g_fehler("Lauf szene_${_wer}: Kipp-Folge '${_kfolge}' statt 'A 1:0 6:240 12:1056 18:2448 24:4096' (Nachtrag 26, ROOM40A0 sub10)")
    endif()
    # ---- Nachbesserung 2 (N2): Gier des Werfers (Slot 9) in Cut 5 vom Hinknien bis zum Schnitt gleich, in Cut 7 die Wurf-Gier ----
    file(STRINGS "${WORKDIR}/debug.log" _wr REGEX "\\[k2080\\] F[0-9]+ Wurfrichtung [0-9]+ ")
    if(NOT _wr)
        r41g_fehler("Lauf szene_${_wer}: keine Zeile 'Wurfrichtung' (N3)")
    endif()
    list(GET _wr 0 _wrl)
    string(REGEX MATCH "Wurfrichtung ([0-9]+) " _m "${_wrl}")
    set(_wgier ${CMAKE_MATCH_1})
    set(_knien -1)
    set(_sprung "")
    set(_cut7 -1)
    foreach(_l IN LISTS _st)
        if(NOT _l MATCHES " cam=([0-9]+) ")
            continue()
        endif()
        set(_c ${CMAKE_MATCH_1})
        if(NOT _l MATCHES "\\[9 t=(40|49) [^]]* mo=([0-9]+) af=[0-9]+ [^]]*@\\(-?[0-9]+,-?[0-9]+,r([0-9]+)\\)\\]")
            continue()
        endif()
        set(_mo ${CMAKE_MATCH_2})
        set(_r ${CMAKE_MATCH_3})
        if(_c EQUAL 5 AND _knien EQUAL -1 AND _mo EQUAL 11)
            set(_knien ${_r})
        elseif(_c EQUAL 5 AND NOT _knien EQUAL -1 AND NOT _r EQUAL _knien)
            set(_sprung "${_sprung} ${_r}")
        elseif(_c EQUAL 7 AND _cut7 EQUAL -1)
            set(_cut7 ${_r})
        endif()
    endforeach()
    if(_knien EQUAL -1 OR NOT _sprung STREQUAL "")
        r41g_fehler("Lauf szene_${_wer}: Gier in Cut 5 ab dem Hinknien ${_knien}, abweichend:${_sprung} (N2: kein Drehen im Griffbild)")
    endif()
    # Nachbesserung 3 (G-E2): im ersten Bild von Cut 7 die der Kamera zugewandte Gier (Zeile "Werfer Cut 7"), nicht die Wurfrichtung
    file(STRINGS "${WORKDIR}/debug.log" _bl REGEX "\\[k2080\\] F[0-9]+ Werfer Cut 7: Gier [0-9]+, Gier zur Kamera [0-9]+, Versatz -?[0-9]+, frontal [0-9]+/4096")
    if(NOT _bl)
        r41g_fehler("Lauf szene_${_wer}: keine Zeile 'Werfer Cut 7' (G-E2)")
    endif()
    list(GET _bl 0 _bll)
    string(REGEX MATCH "Werfer Cut 7: Gier ([0-9]+), Gier zur Kamera [0-9]+, Versatz (-?[0-9]+), frontal ([0-9]+)/4096" _m "${_bll}")
    set(_wg ${CMAKE_MATCH_1})
    set(_vers ${CMAKE_MATCH_2})
    set(_fr ${CMAKE_MATCH_3})
    math(EXPR _vabw "${_vers} + 111")
    if(NOT _cut7 EQUAL _wg OR _vabw LESS -12 OR _vabw GREATER 12 OR _fr LESS 4023)
        r41g_fehler("Lauf szene_${_wer}: erstes Bild Cut 7 Gier ${_cut7}, Werfer ${_wg}, Versatz ${_vers}, frontal ${_fr} (G-E2: wie Ada -111 / 4034)")
    endif()
    # Nachtrag 28: Raum-ESP ROOM40A0 umgelegt (FUN_8001bd38), in ${_r90} registriert, beim Wechsel nach ${_r80} ausgetragen
    set(_umlage "0x16: TIM 32 Zeilen -> TPage 0x0F y 0, CLUT-Zeilen 480\\.\\.483" "0x19: TIM 80 Zeilen -> TPage 0x0F y 32, CLUT-Zeilen 484\\.\\.484"
                "0x08: TIM 72 Zeilen -> TPage 0x0F y 112, CLUT-Zeilen 485\\.\\.485" "0x09: TIM 32 Zeilen -> TPage 0x0F y 184, CLUT-Zeilen 486\\.\\.487"
                "0x26: TIM 88 Zeilen -> TPage 0x0E y 0, CLUT-Zeilen 488\\.\\.491" "0x1A: TIM 80 Zeilen -> TPage 0x0E y 88, CLUT-Zeilen 492\\.\\.494"
                "0x18: TIM 48 Zeilen -> TPage 0x0E y 168, CLUT-Zeilen 495\\.\\.495")
    foreach(_u IN LISTS _umlage)
        file(STRINGS "${WORKDIR}/debug.log" _z REGEX "\\[gator-fx\\] Raumbank ${_u} \\(FUN_8001bd38\\)")
        if(NOT _z)
            r41g_fehler("Lauf szene_${_wer}: Umlage der Raumbank '${_u}' fehlt (FUN_8001bd38)")
        endif()
    endforeach()
    file(STRINGS "${WORKDIR}/debug.log" _rg REGEX "\\[gator-fx\\] F[0-9]+ ROOM${_r90}: Raum-ESP ROOM40A0 -> re2fx_register_raum rc=0 ")
    file(STRINGS "${WORKDIR}/debug.log" _ag REGEX "\\[gator-fx\\] F[0-9]+ ROOM${_r80}: Raum-ESP ROOM40A0 ausgetragen, GLAS1090\\.ESP wieder registriert")
    list(LENGTH _rg _n_rg)
    if(_n_rg LESS 2 OR NOT _ag)
        r41g_fehler("Lauf szene_${_wer}: Raum-ESP ${_n_rg}x in ${_r90} registriert (2 Besuche erwartet), Austragen in ${_r80}: '${_ag}'")
    endif()
    # FG-E4: der Fall laeuft in ${_r90} Cut 12; nach dem Aufschlag + Sleep 15 der Schnitt zurueck (Spieler wieder auf seiner Lage
    # beim Ausloesen 6500,-25000)
    file(STRINGS "${WORKDIR}/state.log" _st90 REGEX "PL\\(6500,-25000,")
    set(_c90 "")
    set(_letzt "")
    foreach(_l IN LISTS _st90)
        if(_l MATCHES " cam=([0-9]+) ")
            if(NOT CMAKE_MATCH_1 STREQUAL _letzt)
                string(APPEND _c90 "${CMAKE_MATCH_1} ")
                set(_letzt ${CMAKE_MATCH_1})
            endif()
        endif()
    endforeach()
    if(NOT _c90 MATCHES " 12 ")
        r41g_fehler("Lauf szene_${_wer}: kein Cut 12 in ${_r90} (Folge '${_c90}')")
    endif()
    message(STATUS "r41_kanister2080 szene_${_wer}: ${_npc}, 8 Kopfschuesse, Schnitte '${_folge}', Nachricht 42, Kanister im Becken")
endforeach()

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_kanister2080: alle Laeufe gruen")
