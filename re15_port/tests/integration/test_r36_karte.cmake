# Runde 36 Spur M "karte" — ECHTER LAUF der echten exe je Nutzer-Punkt: Spielstand (Slot 0) vor
# einer Tuer schreiben (probe_r36_karte karte), Titel -> LOAD GAME (RE15_CONTINUE_TEST +
# RE15_CARD_AUTO), Aktionstaste an der Tuer (RE15_INPUT_SCRIPT "W1,A0.3,W1,A0.3", Muster
# test_r35_karte.cmake Lauf E), im Zielraum Statusschirm -> MAP (RE15_INV_OPEN_AT=40#<raum>),
# Abzug des Inventar-Framebuffers (RE15_INV_FB_SHOT, RE15_WINDOW_SCALE=1) und Auswertung des
# BILDES (probe_r36_karte farben / marker):
#   A  1230 -> Tuer -> 10A0 (B1, Band 4): Treppensprossen in Blatt 0 rect 6 (P1) und die
#      11B0/11C0-Tuer WAAGERECHT auf y100 (P4)
#   B  1230 -> Tuer -> 11D0: Spieler-Marker direkt unter der Tuer, nicht in der Mitte (P2)
#   C  1230 -> Haupthallen-Tuer -> 1190: rot NUR in rect 1, rect 2 ungezeichnet (P3)
#   D  1230 -> Ostkammer-Tuer -> 1190: rot NUR in rect 2, rect 1 ungezeichnet (P3)
#   E  1100 -> Tuer -> 1110: Evidence Room rot, die kleinen Raeume UNGEZEICHNET (P5)
#   F  Spielstand nach der Fahrt 1F -> 3F und zu Fuss auf 2F -> MAP in 10C0: die 2F-Kabine
#      ist UNGEZEICHNET (P6); H dieselbe Fahrt, MAP in 1120: 3F-Kabine besucht (Ankunft);
#   G  1040 -> Tuer -> Kabine: 1F-Kabine rot (Gegenprobe Weg)
#   I  (Nachbesserung 1, Abnahme 0 M1) Debug-Menue-Sprung "108 ELEVATOR" aus ROOM1100 ohne Etagen-
#      Bits: Kabine nur auf 1F (Blatt des Original-Seiten-Setzers), 2F-Kasten leer, 3F nicht anwaehlbar
# Dossier: analysis/befunde_runde36/M_karte.md
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PROBE=<probe_r36_karte> -DWORKDIR=<dir> -P test_r36_karte.cmake
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_karte: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PROBE OR NOT EXISTS "${RE15_PROBE}")
    message(FATAL_ERROR "r36_karte: RE15_PROBE fehlt: '${RE15_PROBE}'")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
# Eigener exe-Name (Muster test_r34n_b_cursor.cmake): fremde Kills treffen den Lauf nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36m_karte_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r36m_karte_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_fehler "")

# Ein Lauf: Spielstand schreiben, laden, ggf. Tuer, MAP im Zielraum, Abzug nach WORKDIR/karte.bmp.
function(karte_lauf _name _kartenargs _skript _oeffnen_raum)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log" "${WORKDIR}/karte.bmp")
    execute_process(COMMAND "${RE15_PROBE}" karte "re15_card.mcr" ${_kartenargs}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r36_karte[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    set(_skriptenv "")
    if(NOT "${_skript}" STREQUAL "")
        set(_skriptenv RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
                       "RE15_INPUT_SCRIPT=${_skript}")
    endif()
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        ${_skriptenv}
        "RE15_INV_OPEN_AT=40#${_oeffnen_raum}"
        RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=60
        "RE15_EXIT_AT=200#${_oeffnen_raum}"
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/karte.bmp")
        message(STATUS "r36_karte[${_name}] FAIL: kein Kartenabzug (exit=${_rv}, ${WORKDIR})")
        set(_fehler "${_fehler} ${_name}-ohne-Abzug" PARENT_SCOPE)
    endif()
endfunction()

# Farbzaehlung im Kasten: setzt <praefix>_rot/_besucht/_gelb/_sprosse/_wand im Aufrufer.
function(farben _name _praefix _x0 _y0 _x1 _y1)
    if(NOT EXISTS "${_basis}_${_name}/karte.bmp")
        foreach(_k rot besucht gelb sprosse wand)
            set(${_praefix}_${_k} -1 PARENT_SCOPE)
        endforeach()
        return()
    endif()
    execute_process(COMMAND "${RE15_PROBE}" farben "${_basis}_${_name}/karte.bmp" ${_x0} ${_y0} ${_x1} ${_y1}
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _out)
    if(NOT _rv EQUAL 0 OR NOT _out MATCHES "rot=([0-9]+) besucht=([0-9]+) gelb=([0-9]+) sprosse=([0-9]+) wand=([0-9]+)")
        message(FATAL_ERROR "r36_karte[${_name}]: farben ${_x0},${_y0}-${_x1},${_y1}: ${_out}")
    endif()
    set(${_praefix}_rot ${CMAKE_MATCH_1} PARENT_SCOPE)
    set(${_praefix}_besucht ${CMAKE_MATCH_2} PARENT_SCOPE)
    set(${_praefix}_gelb ${CMAKE_MATCH_3} PARENT_SCOPE)
    set(${_praefix}_sprosse ${CMAKE_MATCH_4} PARENT_SCOPE)
    set(${_praefix}_wand ${CMAKE_MATCH_5} PARENT_SCOPE)
    string(STRIP "${_out}" _out)
    message(STATUS "r36_karte[${_name}] ${_praefix}: ${_out}")
endfunction()

# pruefe(<lauf> <wert> <OP> <soll> <text>) - <wert> ist ein Variablenname oder eine Zahl
macro(pruefe _name _a _op _b _text)
    if(${_a} ${_op} ${_b})
        message(STATUS "r36_karte[${_name}] PASS: ${_text}")
    else()
        message(STATUS "r36_karte[${_name}] FAIL: ${_text}")
        set(_fehler "${_fehler} ${_name}")
    endif()
endmacro()

# ---- A: P1 Treppen 10A0 B1 + P4 Tuer 11B0/11C0 --------------------------------------------
# Spieler in ROOM1230 vor der 10A0-Tuer (slot 1 @0x00B34, Trigger (-1381,-8494) 2409x1444) am
# Rueck-Spawn (-358,-6536), Blick -z (Yaw 1024 = (cos,-sin) = -z, player_common.c). 11B0/11C0
# besucht (ihre Ankunftspunkte), damit die Tuer-Marke 11B0<->11C0 auf B1 gezeichnet wird.
karte_lauf(A "1230;-358;0;-6536;1024;besucht:11B0:-26483:-18094;besucht:11C0:-25552:-25331;besucht:1230:-358:-6536"
           "W1,A0.3,W1,A0.3" 10A0)
# Runde 39 Spur G: B1 ist Schema-Zeichnung (re15_map_zones.h B1-Kopf) - der 10A0-Absatz liegt auf (141..158,153..168)
# (synth 10), die Tuer 11B0/11C0 auf der gemeinsamen Wandlinie y94 bei x161..165 (Marke (163,94)).
farben(A tr 141 153 158 168)
pruefe(A tr_sprosse GREATER_EQUAL 25 "B1: Treppensprossen im 10A0-Absatz (Schema-Zeichnung) - ${tr_sprosse} Pixel")
farben(A tw 161 94 165 94)
pruefe(A tw_gelb EQUAL 5 "B1: Tuer 11B0/11C0 als waagerechter Balken x161..165 auf der Wandlinie y94 - ${tw_gelb} von 5")
farben(A to 163 90 163 93)
farben(A tu 163 95 163 98)
math(EXPR _quer "${to_gelb} + ${tu_gelb}")
pruefe(A _quer EQUAL 0 "B1: KEIN senkrechter Balken quer durch die Wand bei x163 - ${_quer}")

# ---- B: P2 Marker ROOM11D0 an der Tuer ------------------------------------------------------
# ROOM1230 vor der 11D0-Tuer (slot 0 @0x00B12, Trigger (-6041,-17561) 2409x1444; nach Ck(4,243)
# @0x00AEA fuehrt sie nach 11D0), Rueck-Spawn (-4729,-16018), Blick -z.
karte_lauf(B "1230;-4729;0;-16018;1024;besucht:1230:-4729:-16018;flag:4:243"
           "W1,A0.3,W1,A0.3" 11D0)
execute_process(COMMAND "${RE15_PROBE}" marker "${_basis}_B/karte.bmp" 108 160 150 190
                RESULT_VARIABLE _rvm OUTPUT_VARIABLE _outm)
string(STRIP "${_outm}" _outm)
message(STATUS "r36_karte[B] ${_outm}")
if(_rvm EQUAL 0 AND _outm MATCHES "^MARKER ([0-9]+) ([0-9]+)")
    set(_my ${CMAKE_MATCH_2})
    # Runde 39 Spur G: Tuermarke Gang<->11D0 (136,173) auf der Oberkante der Zwinger-Zeichnung; Ring-Mitte =
    # Marker - 1 (TEX.TIM @0x14910).
    pruefe(B _my LESS_EQUAL 178 "11D0: Marker-Ring y ${_my} direkt unter der Tuer y173")
else()
    pruefe(B 0 EQUAL 1 "11D0: Marker im Abzug gefunden")
endif()

# Kasten der Haupthalle (rect 1, gemalt x76..143 y76..118) OHNE die Zeilen y114..118: dort liegt
# der untere Ring-Teil der Gang-Kachel (rect 0) darueber (erster Lauf 2026-10-11: 22 "besucht"-Punkte
# des Gangs in y115..117 bei x>=120 - der Gang ist im Lauf D besucht).
# ---- C: P3 1190 Haupthalle --------------------------------------------------------------------
# ROOM1230 vor der Haupthallen-Tuer (slot 2 @0x00B54, Trigger (-9266,8888) 2297x934), Rueck-Spawn
# (-8133,10226), Blick -z.
karte_lauf(C "1230;-8133;0;10226;1024;besucht:1230:-8133:10226" "W1,A0.3,W1,A0.3" 1190)
# Runde 39 Spur G: Halle = Schema-Zeichnung (69..132,115..151), Ostkammer (132..150,109..140), je Innenraum gezaehlt.
farben(C h 70 116 131 150)
farben(C o 133 110 149 139)
math(EXPR _o_alles "${o_rot} + ${o_besucht}")
pruefe(C h_rot GREATER_EQUAL 200 "1190 Haupthalle: rot in der Hallen-Zeichnung - ${h_rot}")
pruefe(C _o_alles EQUAL 0 "1190 Haupthalle: Ostkammer ungezeichnet - ${_o_alles}")

# ---- D: P3 1190 Ostkammer ---------------------------------------------------------------------
# ROOM1230 vor der Ostkammer-Tuer (slot 4 @0x00CE6, Trigger (-1052,11694) 2297x934; Else-Zweig
# von Ck(3,0x89) @0x00B98 -> flag 3:137), Rueck-Spawn (142,13001), Blick -z. Im selben Zweig setzt
# main00 zwei Hunde (Sce_em_set @0x00D08 / @0x00D1C, Kill-Flag pc[7] = 0x88 / 0x89 in Zone 7,
# re15_em_status_zone) - erster Lauf 2026-10-11: sie rissen den Spieler vor der Tuer um. Als
# erledigt gesetzt (flag 7:136 / 7:137), wie nach dem Kampf.
karte_lauf(D "1230;142;0;13001;1024;besucht:1230:142:13001;flag:3:137;flag:7:136;flag:7:137"
           "W1,A0.3,W1,A0.3" 1190)
farben(D o 133 110 149 139)
farben(D h 70 116 131 150)
math(EXPR _h_alles "${h_rot} + ${h_besucht}")
pruefe(D o_rot GREATER_EQUAL 100 "1190 Ostkammer: rot in der Ostkammer-Zeichnung - ${o_rot}")
pruefe(D _h_alles EQUAL 0 "1190 Ostkammer: Haupthalle ungezeichnet - ${_h_alles}")

# ---- E: P5 ROOM1110 Teilbereiche -------------------------------------------------------------
# ROOM1100 vor der 1110-Tuer (slot 1 @0x009BA, Trigger (-27330,-11736) 900x1600), Rueck-Spawn
# (-26130,-11036), Blick -x (Yaw 2048).
# Die Tuer steht erst nach Ck(3,0x67) @0x00AC8 und Ck(4,0xE8) @0x00B06 (ROOM1100 sub00, sonst
# Aot_reset slot 1 sce 0 @0x00ACC/@0x00B0A); die fuenf Zombies des Raums (Sce_em_set @0x00A5E..
# 0x00AAE, Kill-Flags pc[7] 0x55..0x59 Zone 7) als erledigt gesetzt.
karte_lauf(E "1100;-26130;0;-11036;2048;besucht:1100:-26130:-11036;flag:3:103;flag:4:232;flag:7:85;flag:7:86;flag:7:87;flag:7:88;flag:7:89"
           "W1,A0.3,W1,A0.3" 1110)
farben(E t0 147 115 186 143)
farben(E t1 190 123 211 132)
farben(E t2 190 135 211 144)
math(EXPR _t12 "${t1_rot} + ${t1_besucht} + ${t2_rot} + ${t2_besucht}")
pruefe(E t0_rot GREATER_EQUAL 200 "1110: Evidence Room rot - ${t0_rot}")
pruefe(E _t12 EQUAL 0 "1110: die kleinen Tuerbereiche ungezeichnet (vorher besucht) - ${_t12}")

# ---- F: P6 Fahrstuhl -- Fahrt 1F -> 3F, dann zu Fuss auf 2F ------------------------------------
# Reihenfolge wie im Spiel: ROOM1040 (1F), Kabine (aus 1F betreten), ROOM1120 (3F, mit dem Fahrstuhl
# angekommen; Kabinen-Tuer 2 @0x04C2 -> Spawn (1300,0,7300)), dann ROOM10C0 (2F, nicht mit dem
# Fahrstuhl; Spawn aus ROOM10D0 (-6700,0,-2100)). MAP in ROOM10C0 = Blatt 3: die 2F-Kabine (Ersatz-
# rect 4 (109,134,16,16), gemalter Innenraum x110..117 y135..142) war nie besucht.
karte_lauf(F "10C0;-6700;0;-2100;1024;besucht:1040:-21786:-10500;besucht:1080:-13650:-900;besucht:1120:1300:7300;besucht:10C0:-6700:-2100"
           "" 10C0)
farben(F k 110 135 117 142)
math(EXPR _k_alles "${k_rot} + ${k_besucht}")
pruefe(F _k_alles EQUAL 0 "2F-Kabine (nie mit dem Fahrstuhl auf 2F) ungezeichnet (vorher besucht) - ${_k_alles}")

# ---- H: P6 Gegenstueck -- mit dem Fahrstuhl auf 3F angekommen ----------------------------------
# Derselbe Weg bis ROOM1120; MAP dort = Blatt 4: die 3F-Kabine (rect 0 (127,137), gemalter Innenraum
# x128..135 y138..145) IST besucht - man kam mit ihr an.
karte_lauf(H "1120;1300;0;7300;1024;besucht:1040:-21786:-10500;besucht:1080:-13650:-900;besucht:1120:1300:7300"
           "" 1120)
farben(H k 128 138 135 145)
pruefe(H k_besucht GREATER_EQUAL 10 "3F-Kabine nach Ankunft mit dem Fahrstuhl besucht - ${k_besucht}")

# ---- G: Gegenprobe echter Weg in die Kabine (1F) ----------------------------------------------
# ROOM1040 in der Fahrstuhltuer (slot 0 @0x01096), Blick +z (Yaw 3072), Reservestrom (4,243).
karte_lauf(G "1040;-21786;0;-10500;3072;besucht:1040:-21786:-10500;flag:4:243" "W1,A0.3,W1,A0.3" 1080)
farben(G k 110 135 117 142)
pruefe(G k_rot GREATER_EQUAL 10 "Kabine aus 1F betreten: 1F-Kabine rot - ${k_rot}")

# ---- I: P6 Nachbesserung 1 (Abnahme 0 M1) -- Debug-Menue-Sprung "108 ELEVATOR" ---------------
# Spielstand ROOM1100 (2F-Gang, kein Etagenraum der Kabine) OHNE Bank-3-Bit 54/55/56, LOAD, dann
# RE15_DEBUG_JUMP=1080@gp (= Quadrat in der JUMP-Zeile, Spawn aus dem DEBUG.BIN-Satz), MAP in der
# Kabine. I0 Blatt 1F, I1 einmal HOCH (2F), I2 zweimal HOCH. Vorher (5064c3e8): 2F- und 3F-Kabine
# besucht=64, I2 zeigte das Blatt 3F mit der Kabine als einzigem Kasten. Soll: Kabine nur auf dem
# Blatt des Original-Seiten-Setzers (Blatt 2, `ori v0,zero,0x2` @0x8004b684 -> `sb` @0x8004b88c),
# 2F-Kasten leer, 3F nicht anwaehlbar (I2 == I1).
function(debug_lauf _name _pad)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log" "${WORKDIR}/karte.bmp")
    execute_process(COMMAND "${RE15_PROBE}" karte "re15_card.mcr" 1100 -26130 0 -11036 2048
                            "besucht:1100:-26130:-11036"
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r36_karte[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    set(_padenv "")
    if(NOT "${_pad}" STREQUAL "")
        set(_padenv "RE15_PAD_AT=${_pad}")
    endif()
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        "RE15_DEBUG_JUMP=1080@gp" ${_padenv}
        "RE15_INV_OPEN_AT=40#1080" RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=85
        "RE15_EXIT_AT=170#1080"
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/karte.bmp")
        message(STATUS "r36_karte[${_name}] FAIL: kein Kartenabzug (exit=${_rv}, ${WORKDIR})")
        set(_fehler "${_fehler} ${_name}-ohne-Abzug" PARENT_SCOPE)
    endif()
endfunction()
debug_lauf(I0 "")
debug_lauf(I1 "100:U")
debug_lauf(I2 "100:U,108:U")
farben(I0 k 110 135 117 142)
pruefe(I0 k_rot GREATER_EQUAL 10 "Debug-Sprung in die Kabine: 1F-Kabine (Blatt 2) rot - ${k_rot}")
farben(I1 k 109 134 118 143)
math(EXPR _i1 "${k_rot} + ${k_besucht}")
pruefe(I1 _i1 EQUAL 0 "Debug-Sprung: 2F-Kabine ungezeichnet (vorher besucht=64) - ${_i1}")
farben(I2 k 127 137 136 146)
math(EXPR _i2 "${k_rot} + ${k_besucht}")
pruefe(I2 _i2 EQUAL 0 "Debug-Sprung: kein Kabinen-Kasten an der 3F-Stelle (vorher besucht=64) - ${_i2}")
if(EXISTS "${_basis}_I1/karte.bmp" AND EXISTS "${_basis}_I2/karte.bmp")
    execute_process(COMMAND "${RE15_PROBE}" gleich "${_basis}_I1/karte.bmp" "${_basis}_I2/karte.bmp"
                    RESULT_VARIABLE _rvg OUTPUT_VARIABLE _outg)
    string(STRIP "${_outg}" _outg)
    message(STATUS "r36_karte[I2] ${_outg}")
    if(_rvg EQUAL 0 AND _outg MATCHES "^GLEICH ([0-9]+)")
        set(_gl ${CMAKE_MATCH_1})
        pruefe(I2 _gl EQUAL 0 "Debug-Sprung: zweites HOCH bleibt auf 2F, Blatt 3F nicht anwaehlbar - ${_gl} Pixel anders")
    else()
        pruefe(I2 0 EQUAL 1 "Debug-Sprung: Abzuege I1/I2 vergleichbar")
    endif()
endif()

file(REMOVE "${_exe_kopie}")
if(NOT "${_fehler}" STREQUAL "")
    message(FATAL_ERROR "r36_karte: Bildpruefung ROT bei:${_fehler}")
endif()
message(STATUS "r36_karte: alle Laeufe OK")
