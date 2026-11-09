cmake_minimum_required(VERSION 3.16)   # cmake -P: ohne diese Zeile gelten im Linux-Container (CMake 3.28) die alten Policies (IN_LIST, if(TRUE)) - Muster der aelteren Integrationsskripte, Memory reai-v2-linux-test-portabilitaet
# =============================================================================
# Runde 41 Spur AN, Nachbesserung 5 (Abnahme 3 A2): BEGLEITER IN DER SCHUSSLINIE, echter Raum mit der echten exe.
#
# Nutzerentscheid 2026-11-03: "Mache die RE 2 Variante, das der Schuss durch begleiter durch geht." -> RE2-Retail-Regel statt
# RE1.5 @0x80012178-84. Ein lebender Begleiter (NPC 0x40..0x4D, hp -1) ist kein Kandidat des Waffen-Resolvers; der Schuss geht
# durch ihn hindurch auf den Gegner dahinter, der Begleiter nimmt keinen Schaden und geht nicht in Zustand 3. RE2 filtert ueber
# HP, nicht ueber den Typ: FUN_800410CC `lh v0,342(s2)` @0x80041280 / `bltz v0,0x80041300` @0x80041288 (Begruendung
# re15_damage.c re15_player_weapon_fire).
# Zwei falsche Staende, die dieser Test rot meldet (Gegenproben, Dossier N5.2):
#   NB4 (RE1.5-Typpruefung `sltiu v0,v0,0x40` @0x80012180 auf den Gewinner): der Begleiter vorn im Keil schluckt jeden Schuss,
#       der Gegner wird in "davor" nie getroffen.
#   vor NB4 (re15_damage.c aus b646c72a6^): der erste Schuss zieht dem Begleiter Schaden ab (hp -1 -> -6) und setzt ihn in
#       Zustand 3 (Tod).
#
# Raum ROOM10D0 (Leon), Spielstand neu: sub00 @0x0123A Ck(4,247)=0 setzt
#   Marvin   Sce_em_set @0x01256 `44 00 40 40 .. 8f 16 .. 49 5c .. 76 04` = Typ 0x40, (5775, 23625), Gier 1142 -> Aktor 1
#   Zombie   Sce_em_set @0x0126A `44 01 10 0e .. f6 16 .. 5e 64 .. e8 04` = Typ 0x10, Deskriptor 0x0E (schlafend liegend,
#            wacht bei Spielerabstand < 0xBB8 auf, enemy_ai_common.c @0x80100f64-fd4)                          -> Aktor 2
# Der Zombie wird ueber den Messhaken RE15_GEGNER_POS festgehalten (2950 vor Leon, also wach, aber ausser Griffweite);
# Marvin steht unberuehrt an seiner Skriptlage. Pistole (Waffe 3, Keil-Halbbreite Radius + 1000, s_player_wpn_reach[3]),
# Zielen + Feuer 6 s gehalten (R1 + Quadrat).
#   davor     Leon (3800,23625) Gier 0 (+x): Marvin liegt 1975 vor Leon genau auf der Linie zum Zombie (6750,23625).
#             Erwartet: Schuesse fallen (Magazin), der Zombie wird durch Marvin hindurch mindestens zweimal getroffen, Marvin
#             bleibt in Zustand und hp wie vorher (kein Zustand 3, hp -1).
#   seitlich  Leon (3800,25500), Zombie (6750,25500): Marvin 1875 seitlich der Linie, ausserhalb des Keils (450 + 1000).
#             Erwartet: der Zombie wird mindestens zweimal getroffen, Marvin bleibt unveraendert.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_begleiter_schusslinie.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_begleiter_schusslinie: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_begleiter_schusslinie: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")
# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41begleiter_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41begleiter_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_f "")
function(ein_lauf _name _z)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
        RE15_DEBUG_JUMP=10D0@250 "RE15_PLAYER_POS=3800,${_z},0,0" "RE15_GEGNER_POS=2,6750,${_z},2048@1#10D0"
        RE15_EQUIP=3 "RE15_GIVE=3:100"
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=W6,MA6,W1"
        RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log
        "RE15_EXIT_AT=520#10D0"
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/state.log" OR NOT EXISTS "${WORKDIR}/debug.log")
        set(_f "${_f}\n ${_name}: kein Protokoll (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    set(_g "")
    if(NOT _dbg MATCHES "RE15_GEGNER_POS: Akteur 2 -> \\(6750,0,${_z}\\)")
        string(APPEND _g "\n ${_name}: Zombie nicht festgehalten (RE15_GEGNER_POS)")
    endif()
    file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "^F[0-9]+ ")
    set(_mg0 -1)
    set(_mg_end -1)
    set(_basis 0)          # 1 = Ausgangswerte vor dem ersten Schuss gelesen
    set(_m_st "")
    set(_m_hp "")
    set(_z_hp "")
    set(_z_hp_letzt "")
    set(_z_treffer 0)
    set(_m_fehl "")
    set(_z_fehl "")
    set(_m_lage "")
    set(_vorher "")
    foreach(_z IN LISTS _zeilen)
        string(REGEX MATCH "^F([0-9]+) .* mg=(-?[0-9]+) " _m "${_z}")
        if(NOT _m)
            continue()
        endif()
        set(_bild "${CMAKE_MATCH_1}")
        set(_mg "${CMAKE_MATCH_2}")
        if(_mg LESS 0)
            continue()
        endif()
        string(REGEX MATCH "\\[1 t=40 st=([0-9]+) [^]]*@\\((-?[0-9]+),(-?[0-9]+),r-?[0-9]+\\)\\] hp=(-?[0-9]+)" _mm "${_z}")
        set(_mst "${CMAKE_MATCH_1}")
        set(_mx "${CMAKE_MATCH_2}")
        set(_mz "${CMAKE_MATCH_3}")
        set(_mhp "${CMAKE_MATCH_4}")
        string(REGEX MATCH "\\[2 t=10 st=([0-9]+) [^]]*\\] hp=(-?[0-9]+)" _zm "${_z}")
        set(_zhp "${CMAKE_MATCH_2}")
        if(_mg0 LESS 0)
            set(_mg0 "${_mg}")
        endif()
        if(NOT _basis)
            if(_mg EQUAL _mg0)
                # letzte Zeile vor dem ersten Schuss = Ausgangslage
                set(_vorher "${_mm}|${_zm}|${_mst}|${_mhp}|${_zhp}|${_mx}|${_mz}")
                continue()
            endif()
            string(REPLACE "|" ";" _v "${_vorher}")
            list(LENGTH _v _nv)
            if(_nv LESS 7)
                string(APPEND _g "\n ${_name}: keine Zeile vor dem ersten Schuss")
                break()
            endif()
            list(GET _v 0 _vm)
            list(GET _v 1 _vz)
            list(GET _v 2 _m_st)
            list(GET _v 3 _m_hp)
            list(GET _v 4 _z_hp)
            list(GET _v 5 _m_x)
            list(GET _v 6 _m_z)
            if(NOT _vm OR NOT _vz)
                string(APPEND _g "\n ${_name}: Marvin (Aktor 1, Typ 0x40) oder Zombie (Aktor 2, Typ 0x10) fehlt vor dem ersten Schuss")
                break()
            endif()
            set(_m_lage "(${_m_x},${_m_z})")
            set(_z_hp_letzt "${_z_hp}")
            set(_basis 1)
        endif()
        set(_mg_end "${_mg}")
        if(NOT _mm)
            if(NOT _m_fehl)
                set(_m_fehl "F${_bild}: Marvin nicht mehr aktiv")
            endif()
        elseif(NOT _mst STREQUAL _m_st OR NOT _mhp STREQUAL _m_hp)
            if(NOT _m_fehl)
                set(_m_fehl "F${_bild}: Marvin st ${_m_st} -> ${_mst}, hp ${_m_hp} -> ${_mhp}")
            endif()
        endif()
        if(_zm AND _zhp LESS _z_hp_letzt)
            math(EXPR _z_treffer "${_z_treffer} + 1")
            if(NOT _z_fehl)
                set(_z_fehl "F${_bild}: Zombie hp ${_z_hp_letzt} -> ${_zhp}")
            endif()
        endif()
        if(_zm)
            set(_z_hp_letzt "${_zhp}")
        endif()
    endforeach()
    if(NOT _basis)
        string(APPEND _g "\n ${_name}: kein Schuss gefallen (Magazin ${_mg0} unveraendert)")
    else()
        math(EXPR _schuesse "${_mg0} - ${_mg_end}")
        if(_schuesse LESS 5)
            string(APPEND _g "\n ${_name}: nur ${_schuesse} Schuesse (Magazin ${_mg0} -> ${_mg_end}), erwartet >= 5")
        endif()
        if(NOT _m_lage STREQUAL "(5775,23625)")
            string(APPEND _g "\n ${_name}: Marvin nicht an der Skriptlage (5775,23625) @0x01256, sondern ${_m_lage}")
        endif()
        if(_m_st EQUAL 3 OR NOT _m_hp EQUAL -1)
            string(APPEND _g "\n ${_name}: Marvin schon vor dem ersten Schuss tot/verletzt (st ${_m_st}, hp ${_m_hp})")
        endif()
        if(_m_fehl)
            string(APPEND _g "\n ${_name}: Begleiter veraendert (darf weder Schaden nehmen noch in Zustand 3 gehen): ${_m_fehl}")
        endif()
        if(_z_treffer LESS 2)
            if(_name STREQUAL "davor")
                string(APPEND _g "\n ${_name}: Zombie hinter Marvin nur ${_z_treffer}x getroffen, erwartet >= 2 - der Schuss muss durch den Begleiter gehen (RE2 @0x80041280-88, Nutzerentscheid 2026-11-03; geschluckt = RE1.5 @0x80012178-84)")
            else()
                string(APPEND _g "\n ${_name}: Zombie nur ${_z_treffer}x getroffen, erwartet >= 2 (Marvin ausserhalb des Keils)")
            endif()
        endif()
        message(STATUS "r41_begleiter_schusslinie ${_name}: ${_schuesse} Schuesse, Zombie ${_z_hp} -> ${_z_hp_letzt} (${_z_treffer} Treffer, erster ${_z_fehl}), Marvin st ${_m_st} hp ${_m_hp} ${_m_lage}")
    endif()
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

ein_lauf(davor 23625)
ein_lauf(seitlich 25500)
file(REMOVE "${_exe_kopie}")

if(_f)
    message(FATAL_ERROR "r41_begleiter_schusslinie:${_f}\n(Laeufe unter ${_wurzel})")
endif()
message(STATUS "r41_begleiter_schusslinie: davor und seitlich getroffen, Begleiter unveraendert (${_wurzel})")
