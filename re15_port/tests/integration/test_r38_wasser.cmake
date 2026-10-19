# Runde 38 Spur N "wasser" — Integration mit der ECHTEN exe: der Wasser-Effekt (Bild + Ton), wenn Leon oder Elza durch
# flaches Wasser gehen (Nutzer: "In Room 1260 und folgende fehlt noch der Wasser Effekt, wenn sich Leon oder Elza im
# flachen Wasser bewegen."). Dossier analysis/befunde_runde38/N_wasser.md; Registrierung tests/unit/probes/r38_wasser.cmake.
#
# Laeufe (Debug-Menue-Sprung wie der Nutzer-Weg ins Raum-Innere, Eingabe-Skript auf der Spielbild-Zeitachse):
#   leon1260  Leon, ROOM1260, (-8600,-7900) Band 1 Blick 0, "W0.5,U1.5,W2": geht ueber Beton (Schritt F83) in den
#             FLR-Fleck P0 (Typ 0x97) an die Wand. Erwartet (Original-GDB DuckStation, Dossier B9):
#               RE15_WASSER_LOG  ein Schritt  zweig=boden code=0x06050d00 part=4 anker=(-7555,-1873,-7385) edt=0x17
#                                und davor    zweig=trocken edt=0x1d
#               RE15_FX_LOG      der Zeichner zeichnet id=6 sub=5 (>= 13 Bilder je Spritzer)
#               debug.log        [foot] SE ... type=23 vag=7 (Wasserschritt) und type=29 vag=1 (Beton)
#   elza1261  Elza (Spielerwahl), ROOM1261, gleiche Lage/Eingabe: zweig=boden code=0x06050d00, id=6 sub=5, vag=7.
#   leon2090  Leon, ROOM2090 (Becken, sce-8-Stempel -1620), (-8438,-25550) Band 0 Blick 0, "U1.6,W1" ab Bild 30
#             (der Alligator greift ab ~F52 an, der erste Schritt liegt davor):
#               zweig=stempel code=0x06032000 part=6 stempel=-1620 edt=0x1a, [foot] type=26 vag=3 (Watschritt).
#   leon10b0_tuer  echter Weg: Sprung nach 10B0, durch die Tuer nach 1260 (Ankunft vor Fleck P5), Wasserschritt + Spritzer.
#   (elza10b1_tuer entfallen: der Kanaldeckel ROOM10B1 ist fuer Elza seit Runde 39 Spur P immer zu, Nutzer-Nachtrag 12)
# v0.8.25 (2077bb8e) faellt hier durch: kein RE15_WASSER_LOG, kein id=6 im FX_LOG, im Becken Betonschritt vag=1.
# Die exe laeuft als Messkopie re15_pc_r38n_haken_<id> neben der Original-exe (Muster test_r34n_b_cursor.cmake).

cmake_minimum_required(VERSION 3.16)
set(_tag "r38_wasser")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_wurzel}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38n_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r38n_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(r38n_fehler _lauf _text)
    message(FATAL_ERROR "${_tag} [${_lauf}]: ${_text}")
endfunction()

# _start: "leon" (Titelbild-Weg) oder "elza" (Spielerwahl -> Elza)
function(r38n_exe _lauf _start _raum _pos _input _beginn _exit)
    set(_dir "${_wurzel}/${_lauf}")
    file(MAKE_DIRECTORY "${_dir}")
    set(WORKDIR "${_dir}")
    if(_exit MATCHES "#")
        set(_ex "${_exit}")              # Ende in einem anderen Raum (Weg durch die Tuer)
    else()
        set(_ex "${_exit}#${_raum}")
    endif()
    if(_start STREQUAL "elza")
        set(_st RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
    else()
        set(_st RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
    endif()
    set(_env RE15_NO_INTRO=1 ${_st} RE15_WINDOW_SCALE=1 RE15_AUDIO_CAP_SYNC=cap.raw RE15_FOOT_DEBUG=1
             "RE15_DEBUG_JUMP=${_raum}@250"
             RE15_STATE_LOG=state.log RE15_WASSER_LOG=wasser.log RE15_FX_LOG=fx.log "RE15_EXIT_AT=${_ex}")
    if(NOT _pos STREQUAL "-")            # "-" = Raum-Skript bestimmt die Lage (kein RE15_PLAYER_POS)
        list(APPEND _env "RE15_PLAYER_POS=${_pos}")
    endif()
    if(NOT _input STREQUAL "-")          # "-" = keine Eingabe (Skriptlauf)
        list(APPEND _env RE15_INPUT_SCRIPT_BASIS=spiel "RE15_INPUT_SCRIPT_START=${_beginn}" "RE15_INPUT_SCRIPT=${_input}")
    endif()
    re15_start_spiel(_rv 300 ${_env} "${RE15_PC_EXE}")
    set(_z "")
    if(EXISTS "${_dir}/debug.log")
        file(STRINGS "${_dir}/debug.log" _z)
    endif()
    list(LENGTH _z _n)
    if(NOT _rv EQUAL 0 AND (_n LESS 10 OR "${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1"))
        message(STATUS "${_tag} [${_lauf}]: Lauf ohne Ergebnis (exit=${_rv}, debug.log ${_n} Zeilen) -> EIN Wiederholungsversuch")
        file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/wasser.log" "${_dir}/fx.log" "${_dir}/cap.raw")
        re15_start_spiel(_rv 300 ${_env} "${RE15_PC_EXE}")
    endif()
    if(NOT _rv EQUAL 0)
        r38n_fehler("${_lauf}" "re15_pc exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
    endif()
    file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
    if(NOT _ex)
        r38n_fehler("${_lauf}" "RE15_EXIT_AT nicht erreicht — Haenger? ${_dir}")
    endif()
    foreach(_d wasser.log fx.log)
        if(NOT EXISTS "${_dir}/${_d}")
            r38n_fehler("${_lauf}" "kein ${_d} (RE15_WASSER_LOG/RE15_FX_LOG) in ${_dir} — Wasser-Schritt fehlt")
        endif()
    endforeach()
endfunction()

# Zeile im Log mit allen Teilmustern (Literal-Teilstrings)
function(r38n_zeile _lauf _datei _muster_liste _beschreibung)
    file(STRINGS "${_datei}" _zl)
    foreach(_l IN LISTS _zl)
        set(_alle 1)
        foreach(_m IN LISTS _muster_liste)
            string(FIND "${_l}" "${_m}" _p)
            if(_p LESS 0)
                set(_alle 0)
                break()
            endif()
        endforeach()
        if(_alle)
            message(STATUS "${_tag} [${_lauf}]: ok ${_beschreibung}: ${_l}")
            return()
        endif()
    endforeach()
    r38n_fehler("${_lauf}" "${_beschreibung} fehlt in ${_datei} (Muster: ${_muster_liste})")
endfunction()

function(r38n_fx_bilder _lauf _datei _min)
    file(STRINGS "${_datei}" _zl REGEX "^id=6 sub=5 ")
    list(LENGTH _zl _n)
    if(_n LESS _min)
        r38n_fehler("${_lauf}" "Zeichner zeichnete ${_n} Bilder id=6 sub=5 (erwartet >= ${_min}) — kein Spritzer sichtbar")
    endif()
    message(STATUS "${_tag} [${_lauf}]: ok Zeichner id=6 sub=5 in ${_n} Bildern")
endfunction()

# ---- Leon ROOM1260 ---------------------------------------------------------------------------------------------
r38n_exe(leon1260 leon 1260 "-8600,-7900,0,1" "W0.5,U1.5,W2" 60 140)
set(_d "${_wurzel}/leon1260")
r38n_zeile(leon1260 "${_d}/wasser.log" "room=1260;zweig=boden;code=0x06050d00;part=4;anker=(-7555,-1873,-7385);edt=0x17"
           "Wasserschritt (Effekt 6 Sub 5 am Original-Anker, Tonsatz 0x17)")
r38n_zeile(leon1260 "${_d}/wasser.log" "room=1260;zweig=trocken;edt=0x1d" "Betonschritt ohne Effekt")
r38n_fx_bilder(leon1260 "${_d}/fx.log" 13)
r38n_zeile(leon1260 "${_d}/debug.log" "[foot] SE foot=4 type=23 vag=7" "Wasser-Schritt-Ton (EDT 0x17 -> VAG 7)")
r38n_zeile(leon1260 "${_d}/debug.log" "type=29 vag=1" "Beton-Schritt-Ton (EDT 0x1d -> VAG 1)")

# ---- Elza ROOM1261 ---------------------------------------------------------------------------------------------
r38n_exe(elza1261 elza 1261 "-8600,-7900,0,1" "W0.5,U1.5,W2" 60 140)
set(_d "${_wurzel}/elza1261")
r38n_zeile(elza1261 "${_d}/debug.log" "Elza-Bit=1" "Spielerin Elza (PL04)")
r38n_zeile(elza1261 "${_d}/wasser.log" "room=1261;zweig=boden;code=0x06050d00;part=4;edt=0x17"
           "Wasserschritt Elza (Effekt 6 Sub 5, Tonsatz 0x17)")
r38n_fx_bilder(elza1261 "${_d}/fx.log" 13)
r38n_zeile(elza1261 "${_d}/debug.log" "type=23 vag=7" "Wasser-Schritt-Ton Elza (VAG 7)")

# ---- Leon ROOM2090 (sce-8-Stempel) -----------------------------------------------------------------------------
r38n_exe(leon2090 leon 2090 "-8438,-25550,0,0" "U1.6,W1" 30 80)
set(_d "${_wurzel}/leon2090")
r38n_zeile(leon2090 "${_d}/wasser.log" "room=2090;stempel=-1620;zweig=stempel;code=0x06032000;part=6;edt=0x1a"
           "Watschritt im Becken (Effekt 6 Sub 3 an Part a1-1, Watsatz 0x1a)")
r38n_zeile(leon2090 "${_d}/debug.log" "type=26 vag=3" "Wat-Schritt-Ton (EDT 0x1a -> VAG 3)")

# ---- Leon durch die Tuer ROOM10B0 -> ROOM1260 (echter Weg, kein Sprung in 1260) -----------------------------------
# 10B0 FRONT EXTERIOR: vor die Tuer (Satz main00 @0x16f2, Ziel Stage 0 Raum 0x26, Ankunft (-20250,-1800,-6700) Blick 1024,
# Cut 15), Aktionstaste, Raumwechsel; das Skript laeuft auf der Spielbild-Achse im neuen Raum erneut an -> Leon geht nach
# -z durch den Fleck P5 (x -20800..-19500, z -9550..-7350).
r38n_exe(leon10b0_tuer leon 10B0 "-21370,3000,3072,0" "U0.6,A0.2,U3" 30 "150#1260")
set(_d "${_wurzel}/leon10b0_tuer")
r38n_zeile(leon10b0_tuer "${_d}/debug.log" "JUMP -> 10b" "Sprung nur nach 10B0")
r38n_zeile(leon10b0_tuer "${_d}/wasser.log" "room=10B0;zweig=trocken" "Schritt vor der Tuer (10B0)")
r38n_zeile(leon10b0_tuer "${_d}/wasser.log" "room=1260;zweig=boden;code=0x06050d00;edt=0x17"
           "nach der Tuer: Wasserschritt im Fleck P5 von ROOM1260")
r38n_fx_bilder(leon10b0_tuer "${_d}/fx.log" 13)

# ---- Elza durch die Tuer ROOM10B1 -> ROOM1261: ENTFALLEN (Runde 39 Spur P, Nachbesserung 1) ----------------------------
# Nutzer-Nachtrag 12 (analysis/befunde_runde39/AUFTRAG.md): "in Elzas Szenario die Tür im ROOM 10B1 ausnahmslos IMMER
# geschlossen ... auch wenn sie nach Sherry sucht". Der Kanaldeckel ROOM10B1 Slot 1 (@0x16f2) ist fuer Elza seitdem ein
# Text (include/re15_tuersperre_r39.h); diesen Weg gibt es im Spiel nicht mehr. Der Deckel selbst ist in
# integration_r39_johnruf1041_tuer_t10b1v/_t10b1s/_t10b1n gesichert. Elzas Wasserschritt im Fleck der Kanalgaenge bleibt
# durch elza1261 belegt (gleiche Lage/Eingabe wie leon1260: zweig=boden, edt 0x17, VAG 7), der Tuerweg in den Fleck P5
# durch leon10b0_tuer (Leons Deckel ROOM10B0 ist offen).

# ---- Skriptlauf ROOM20B0 (Leon) / ROOM20B1 (Elza): Flut-Szene, Plc_dest (Lauf 3) ---------------------------------
# Raum-Skript laesst die Figur auf Band 1 entlang z -6355 nach -x rennen (200/Bild). Die Skript-Bewegung
# (re15_actor_step_all_walkers) laeuft im PC-Hauptlauf VOR re15_game_step; der Schritt prueft Boden und Anker an der
# GEZEICHNETEN Lage (Original: Schritt vor pos_advance, Plc-Lauf `jal 0x80045630` @0x80030e90 vor `jal 0x800245d8`
# @0x80030eb0). Erwartet je Schritt zustand=gezeichnet, lage = vor + (200,0) (Walker schon weiter), und fuer Leon
# Anker relativ zur Lage = Original-Lauf-Anker (gdb_1260_lauf.txt, Blick 0: Bild 0 (520,-91,-56), Bild 12
# (-804,-1075,-196)) um 180 Grad gedreht (Blick 2047): (-520,-91,56) / (804,-1075,196), je +-3.
function(r38n_skriptlauf _lauf _raum _soll_rel)
    set(_d "${_wurzel}/${_lauf}")
    file(STRINGS "${_d}/wasser.log" _zl REGEX "room=${_raum} schritt")
    list(LENGTH _zl _n)
    if(_n LESS 5)
        r38n_fehler("${_lauf}" "nur ${_n} Schritte in ${_raum} (Flut-Szene nicht gelaufen?) ${_d}")
    endif()
    set(_boden 0)
    set(_walker 0)
    set(_geprueft 0)
    foreach(_l IN LISTS _zl)
        if(NOT _l MATCHES "zustand=gezeichnet")
            r38n_fehler("${_lauf}" "Schritt nicht am gezeichneten Bild: ${_l}")
        endif()
        if(NOT _l MATCHES "lage=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)")
            r38n_fehler("${_lauf}" "keine lage in: ${_l}")
        endif()
        set(_lx ${CMAKE_MATCH_1})
        set(_ly ${CMAKE_MATCH_2})
        set(_lz ${CMAKE_MATCH_3})
        if(_l MATCHES "vor=\\((-?[0-9]+),(-?[0-9]+)\\)")
            math(EXPR _dv "${_lx} - ${CMAKE_MATCH_1}")
            if(_dv EQUAL 200)
                math(EXPR _walker "${_walker} + 1")
            endif()
        endif()
        string(REGEX MATCH " bild=([0-9]+) " _mb "${_l}")
        set(_bild "${CMAKE_MATCH_1}")
        if(_l MATCHES "zweig=boden")
            math(EXPR _boden "${_boden} + 1")
            # Anker-Vergleich erst ab dem 2. Laufzyklus (Bild >= 22): der erste Schritt nach dem Losrennen traegt die
            # Ueberblend-Pose des Clip-Wechsels; der Original-Bezug (gdb_1260_lauf.txt) ist der eingeschwungene Lauf.
            if(_soll_rel AND _bild GREATER_EQUAL 22)
                string(REGEX MATCH "anker=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)" _m "${_l}")
                math(EXPR _rx "${CMAKE_MATCH_1} - ${_lx}")
                math(EXPR _ry "${CMAKE_MATCH_2} - ${_ly}")
                math(EXPR _rz "${CMAKE_MATCH_3} - ${_lz}")
                set(_treffer 0)
                foreach(_s IN LISTS _soll_rel)
                    string(REPLACE ":" ";" _sv "${_s}")
                    list(GET _sv 0 _sx)
                    list(GET _sv 1 _sy)
                    list(GET _sv 2 _sz)
                    math(EXPR _ex "${_rx} - (${_sx})")
                    math(EXPR _ey "${_ry} - (${_sy})")
                    math(EXPR _ez "${_rz} - (${_sz})")
                    if(_ex GREATER_EQUAL -3 AND _ex LESS_EQUAL 3 AND _ey GREATER_EQUAL -3 AND _ey LESS_EQUAL 3
                       AND _ez GREATER_EQUAL -3 AND _ez LESS_EQUAL 3)
                        set(_treffer 1)
                    endif()
                endforeach()
                if(NOT _treffer)
                    r38n_fehler("${_lauf}" "Anker relativ (${_rx},${_ry},${_rz}) passt zu keinem Original-Wert (${_soll_rel}): ${_l}")
                endif()
                message(STATUS "${_tag} [${_lauf}]: ok Anker relativ (${_rx},${_ry},${_rz}) = Original: ${_l}")
                math(EXPR _geprueft "${_geprueft} + 1")
            endif()
        endif()
    endforeach()
    if(_boden LESS 2)
        r38n_fehler("${_lauf}" "nur ${_boden} Wasserschritte in den Flecken von ${_raum}")
    endif()
    if(_walker LESS 3)
        r38n_fehler("${_lauf}" "lage = vor + 200 nur ${_walker}x — Skript-Bewegung lag nicht vor dem Spielschritt?")
    endif()
    if(_soll_rel AND _geprueft LESS 2)
        r38n_fehler("${_lauf}" "nur ${_geprueft} Wasserschritte ab Bild 22 mit Original-Anker verglichen")
    endif()
    message(STATUS "${_tag} [${_lauf}]: ok ${_n} Schritte am gezeichneten Bild, ${_boden} im Wasser, Walker-Vorlauf ${_walker}x")
endfunction()

r38n_exe(leon20b0_skript leon 20B0 "-" "-" 30 200)
r38n_skriptlauf(leon20b0_skript 20B0 "-520:-91:56;804:-1075:196")
r38n_zeile(leon20b0_skript "${_wurzel}/leon20b0_skript/wasser.log" "room=20B0;boden=0x17;zweig=trocken;edt=0x17"
           "Fleck 0x17 ohne Bit 0x80: Wasser-Ton ohne Effekt (Original-Daten)")
r38n_exe(elza20b1_skript elza 20B1 "-" "-" 30 200)
r38n_zeile(elza20b1_skript "${_wurzel}/elza20b1_skript/debug.log" "Elza-Bit=1" "Spielerin Elza (PL04)")
r38n_skriptlauf(elza20b1_skript 20B1 "")

file(REMOVE "${_exe_kopie}")
message(STATUS "${_tag}: OK — Leon/Elza Wasserschritt mit Effekt 6 Sub 5 + VAG 7, Becken Effekt 6 Sub 3 + VAG 3, Tuerwege 10B0 -> 1260 / 10B1 -> 1261, Skriptlauf 20B0/20B1")
