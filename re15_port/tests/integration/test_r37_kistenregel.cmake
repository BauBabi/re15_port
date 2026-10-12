cmake_minimum_required(VERSION 3.16)
# =============================================================================
# KISTENREGEL MIT DER ECHTEN EXE (Runde 37, Spur Q). Dossier analysis/befunde_runde37/Q_kistenregel.md, Regel
# include/re15_kistenregel_r37.h. Die Unit-Riegel unit_r37_kistenregel_* pruefen die Mechanik am Engine-Code; dieser
# Riegel prueft den Nutzerweg an der exe: Spielstand (Kartenwerkzeug probe_r37_feuerloescher_karte, `kiste:` legt den
# Gegenstand in die ITEM BOX, `give:` ins Inventar) -> CONTINUE -> Druck auf den Platz -> Nachrichten im debug.log.
#
#   A  1090 Loescher in der KISTE, Leon am Feuer (Spielstand wie Lauf I der Spur I): Aktion -> NUR msg 7 "I must
#      hurry up ...", KEINE Frage msg 8, kein Loeschen (msg 9)                         (Auftrag Q, erster Fall)
#   B  1090 Loescher im INVENTAR (Gegenprobe, dieselbe Eingabe): msg 7, Frage msg 8, Ja -> msg 9 wie bisher
#   C  10D0 Kartenleser, Blue Keycard in der KISTE (Aktion per RE15_FIRE_AOT=1): msg 7 -> Ja -> msg 8 "You have not
#      the Blue Keycard to operate it." statt msg 9;  D  dasselbe mit der Karte im INVENTAR -> msg 9
#   E  Elza 11E1 Kartenleser, Yellow Keycard in der KISTE -> msg 8
#   F  Elza 1211 Zelle, Prison Key in der KISTE -> Platz 8 = msg 10 "It's locked.It seems that there is someone
#      inside..." (keine Befreiung)
#   G  3010 Tuer, Green Keycard in der KISTE -> msg 0 "It's locked.An ID card is required to open it." statt msg 1
#   H  4030 Hauptsicherung, Spark Plug in der KISTE -> msg 0 "Main Fuse ..." (Raumtext statt Marker)
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r37_feuerloescher_karte> -DWORKDIR=<dir>
#               -P test_r37_kistenregel.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r37_kistenregel: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r37_kistenregel: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_kistenregel: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# EIGENER EXE-NAME neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake): ein paralleler
# Bau beendet sonst per `taskkill /IM re15_pc.exe` jede re15_pc.exe der Maschine.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37q_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37q_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

set(_fehler "")
macro(pruef _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(_p LESS 0)
        list(APPEND _fehler "${_name}: fehlt '${_muster}'")
        message(STATUS "FAIL ${_name}: fehlt '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: '${_muster}'")
    endif()
endmacro()
macro(pruef_nicht _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(NOT _p LESS 0)
        list(APPEND _fehler "${_name}: unerwartet '${_muster}'")
        message(STATUS "FAIL ${_name}: unerwartet '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: nicht da '${_muster}'")
    endif()
endmacro()

# Ein Lauf: Karte schreiben, exe starten, debug.log lesen (Muster test_r37_feuerloescher.cmake fl_lauf).
function(kr_lauf _name _raum _kartenargs _skript _ende _extra _out)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_kartenargs}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r37_kistenregel[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 400
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
        "RE15_INPUT_SCRIPT=${_skript}"
        RE15_MSG_LOG=1
        "RE15_EXIT_AT=${_ende}"
        ${_extra}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r37_kistenregel[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_kistenregel[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_kistenregel[${_name}]: Endbild nicht erreicht - Lauf vorher abgerissen "
                            "(exit=${_rv}, ${WORKDIR})")
    endif()
    message(STATUS "r37_kistenregel[${_name}]: exit=${_rv}, ${WORKDIR}")
    set(${_out} "${_dbg}" PARENT_SCOPE)
endfunction()

# Takte (30 Hz ab Spielbild 60): A@30 Aktion am Feuer (msg 7), A@93 weiter (msg 8 = Frage), A@291 Ja, A@474 weiter
# (Lauf I der Spur I, test_r37_feuerloescher.cmake). Ende Bild 800: Loeschen + msg 9 liegen bei Inventar davor.
set(_feuer "W1,A0.1,W2,A0.1,W6.5,A0.1,W6,A0.1,W8")
set(_k1 "1090;0;250;0;y:-1800;cut:13;flag:9:0x86;flag:3:0x85;flag:3:0x80;leer")
# Druck per RE15_FIRE_AOT=<slot>@70#<raum> (re15_aot_fire_slot = Pfad des Aktionsdrucks), A je 2 s: weiter / Ja.
set(_ja "W3,A0.1,W2,A0.1,W2,A0.1,W2,A0.1,W2,A0.1,W4")

# --- A/B: 1090 -------------------------------------------------------------------------------------------------
kr_lauf(a 1090 "${_k1};kiste:0x31:1" "${_feuer}" 800 "" _a)
pruef(A _a "[kistenregel] ROOM1090 @0x0230A Ck(3,0x85) K1: Item 0x31 NICHT im Inventar (Item-Box Platz 2)")
pruef(A _a "id=7 \"I must hurry up and get something to put out this fire to save that woman!\"")
pruef_nicht(A _a "id=8 \"Will you use the Fire Extinguisher?\"")
pruef_nicht(A _a "id=9 \"You've used the Fire Extinguisher.\"")
kr_lauf(b 1090 "${_k1};give:0x31:1" "${_feuer}" 800 "" _b)
pruef(B _b "[kistenregel] ROOM1090 @0x0230A Ck(3,0x85) K1: Item 0x31 im Inventar -> wie Original")
pruef(B _b "id=8 \"Will you use the Fire Extinguisher?\"")
pruef(B _b "id=9 \"You've used the Fire Extinguisher.\"")

# --- C/D: Kartenleser 10D0 (Leon) ------------------------------------------------------------------------------
kr_lauf(c 10d0 "10D0;800;-5000;2048;flag:9:0x34;leer;kiste:0x38:1" "${_ja}" 700 "RE15_FIRE_AOT=1@70#10D0" _c)
pruef(C _c "[kistenregel] ROOM10D0 @0x019C0 Ck(9,0x34) K2: Item 0x38 NICHT im Inventar (Item-Box Platz 2)")
pruef(C _c "id=8 \"You have not the Blue Keycard to operate it.\"")
pruef_nicht(C _c "id=9 \"You've used the Blue Keycard.\"")
kr_lauf(d 10d0 "10D0;800;-5000;2048;flag:9:0x34;leer;give:0x38:1" "${_ja}" 700 "RE15_FIRE_AOT=1@70#10D0" _d)
pruef(D _d "id=9 \"You've used the Blue Keycard.\"")
pruef_nicht(D _d "id=8 \"You have not the Blue Keycard to operate it.\"")

# --- E: Kartenleser 11E1 (Elza) --------------------------------------------------------------------------------
kr_lauf(e 11e1 "11E1;11050;4250;0;elza;flag:9:0x8a;leer;kiste:0x39:1" "${_ja}" 700 "RE15_FIRE_AOT=4@70#11E1" _e)
pruef(E _e "id=8 \"You have not the Yellow Keycard to operate it.\"")
pruef_nicht(E _e "id=9 \"You've used the Yellow Keycard.\"")

# --- F: Zelle 1211 (Elza), Prison Key in der Kiste ----------------------------------------------------------------
kr_lauf(f 1211 "1211;-21500;-21600;0;elza;flag:4:0xec;leer;kiste:0x42:1" "${_ja}" 400 "RE15_FIRE_AOT=8@70#1211" _f)
pruef(F _f "[kistenregel] ROOM1211 @0x01DEA Ck(4,0xEC) K11: Item 0x42 NICHT im Inventar (Item-Box Platz 2)")
pruef(F _f "id=10 \"It's locked.It seems that there is someone inside...\"")
pruef_nicht(F _f "id=0 \"Elza: Hey! You must be John right? Get up now!\"")

# --- G: Tuer 3010, Green Keycard in der Kiste ---------------------------------------------------------------------
kr_lauf(g 3010 "3010;-26750;-19800;0;y:-1800;flag:9:0x55;flag:3:0x3b;leer;kiste:0x36:1" "${_ja}" 400
        "RE15_FIRE_AOT=6@70#3010" _g)
pruef(G _g "id=0 \"It's locked.An ID card is required to open it.\"")
pruef_nicht(G _g "id=1 \"You've used the Green Keycard.\"")

# --- H: Hauptsicherung 4030, Spark Plug in der Kiste -----------------------------------------------------------
kr_lauf(h 4030 "4030;-22960;-22960;0;flag:9:0x56;flag:3:0x57;leer;kiste:0x41:1" "${_ja}" 400
        "RE15_FIRE_AOT=6@70#4030" _h)
pruef(H _h "[kistenregel] ROOM4030 @0x004FA Ck(3,0x57) K10: Item 0x41 NICHT im Inventar (Item-Box Platz 2)")
pruef(H _h "id=0 \"Main Fuse\"")

file(REMOVE "${_exe_kopie}")
if(_fehler)
    string(REPLACE ";" "\n  " _f "${_fehler}")
    message(FATAL_ERROR "r37_kistenregel: ${_f}")
endif()
message(STATUS "r37_kistenregel: alle Laeufe gruen")
