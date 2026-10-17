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
#   elza10b1_tuer  echter Weg mit Elza: 10B1 -> Tuer -> 1261, Wasserschritt + Spritzer + VAG 7 im Fleck P5, Beton ohne Effekt.
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
             "RE15_DEBUG_JUMP=${_raum}@250" "RE15_PLAYER_POS=${_pos}"
             RE15_INPUT_SCRIPT_BASIS=spiel "RE15_INPUT_SCRIPT_START=${_beginn}" "RE15_INPUT_SCRIPT=${_input}"
             RE15_STATE_LOG=state.log RE15_WASSER_LOG=wasser.log RE15_FX_LOG=fx.log "RE15_EXIT_AT=${_ex}")
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

# ---- Elza durch die Tuer ROOM10B1 -> ROOM1261 (echter Weg mit Elza, Lauf 3 2026-10-17) -----------------------------
# Elza-RDT = Leon-RDT + 1 (@0x800397e4/ec): dieselbe Tuer fuehrt Elza nach 1261, Ankunft vor Fleck P5. Gemessen:
# F39 room=1261 zweig=boden part=7 edt=0x17, F63 dito, danach Beton 0x1d; [foot] type=23 vag=7 / type=29 vag=1.
r38n_exe(elza10b1_tuer elza 10B1 "-21370,3000,3072,0" "U0.6,A0.2,U3" 30 "150#1261")
set(_d "${_wurzel}/elza10b1_tuer")
r38n_zeile(elza10b1_tuer "${_d}/debug.log" "Elza-Bit=1" "Spielerin Elza (PL04)")
r38n_zeile(elza10b1_tuer "${_d}/debug.log" "JUMP -> 10b" "Sprung nur nach 10B1")
r38n_zeile(elza10b1_tuer "${_d}/wasser.log" "room=10B1;zweig=trocken" "Schritt vor der Tuer (10B1)")
r38n_zeile(elza10b1_tuer "${_d}/wasser.log" "room=1261;zweig=boden;code=0x06050d00;edt=0x17"
           "nach der Tuer: Wasserschritt Elza im Fleck P5 von ROOM1261")
r38n_zeile(elza10b1_tuer "${_d}/wasser.log" "room=1261;zweig=trocken;edt=0x1d" "Elza auf Beton in 1261: kein Effekt")
r38n_fx_bilder(elza10b1_tuer "${_d}/fx.log" 13)
r38n_zeile(elza10b1_tuer "${_d}/debug.log" "type=23 vag=7" "Wasser-Schritt-Ton Elza nach der Tuer (VAG 7)")

file(REMOVE "${_exe_kopie}")
message(STATUS "${_tag}: OK — Leon/Elza Wasserschritt mit Effekt 6 Sub 5 + VAG 7, Becken Effekt 6 Sub 3 + VAG 3, Tuerwege 10B0 -> 1260 / 10B1 -> 1261")
