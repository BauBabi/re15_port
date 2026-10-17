# Runde 38 Spur D "zopf" — Integration mit der ECHTEN exe: Elzas Zopf wird gezeichnet und pendelt.
# Dossier analysis/befunde_runde38/D_zopf.md (Belege FUN_80024c30 / FUN_80024e40, Init @0x80031a78-80031bf4).
# Registrierung: tests/unit/probes/r38_zopf.cmake.
#
# Aufbau (Messung des Dossiers §4): Elza (RE15_PSELECT_AUTO), RE15_DEBUG_JUMP 1120 -> ROOM1121, Elza (2500,10000)
# Blick 2048 = mit dem Ruecken zur Kamera (Cut 0), Eingabe "W1,R1.0,W4" (1 s stehen, 1 s Tank-Drehung, 4 s stehen).
#   elza      RE15_ZOPF_LOG + Bild 50 (waehrend der Drehung):
#             spur  -> test_r38_zopf spur: s0 dreht sich, Zopf schlaegt seitlich aus, schwingt durch, kommt zur Ruhe
#   elza_aus  dieselbe Fahrt mit der Mess-Sonde RE15_ZOPF_SONDE_AUS=1 (Zopf gerechnet, NICHT gezeichnet), Bild 50:
#             pixel -> test_r38_zopf pixel: >= 25 Pixel verschieden, alle am Hinterkopf-Fenster
#   leon      Gegenprobe Leon (ROOM1120, gleiche Stelle): kein Zopf-Schritt, Leon-Mesh 17 Teile
#   elza_lauf Gehen + Rennen (Dossier §4.4): Elza (150,9550) Blick 2048 mit der Eingabe der ORIGINAL-Messung
#             (D_belege/orig_zopf_gdb.py: stehen, R 29 Bilder, stehen, U 40 Bilder), danach Kehrt + Rennen 14 Bilder:
#             lauf   -> test_r38_zopf lauf: Ausschlag nach hinten beim Gehen je Glied wie im Original (Referenz
#                       r38_zopf_orig.inc, Schranke = Abweichung allein durch die gleichmaessige Port-Schrittweite),
#                       Rennen reisst weiter, nach dem Halt Schwung durch die Ruhelage und Ausschwingen
#             huelle -> test_r38_zopf huelle (nur Ausgabe): Zopf-Netz gegen den Figurkasten der Runde-37-Spur P
# Die exe laeuft als Messkopie re15_pc_r38d_haken_<id> neben der Original-exe (Muster test_r34n_b_cursor.cmake).
# Ein Lauf ohne Ergebnis (exit=1/-1 = von aussen beendet; debug.log < 10 Zeilen) wird EINMAL wiederholt.

cmake_minimum_required(VERSION 3.16)
set(_tag "r38_zopf")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_ZOPF_TOOL OR NOT EXISTS "${RE15_ZOPF_TOOL}")
    message(FATAL_ERROR "${_tag}: RE15_ZOPF_TOOL fehlt: '${RE15_ZOPF_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_wurzel}")

get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" LAST_EXT)          # ".exe" unter Windows, leer unter Linux
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38d_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r38d_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(r38_fehler _lauf _text)
    message(FATAL_ERROR "${_tag} [${_lauf}]: ${_text}")
endfunction()

# _start: "elza" (Spielerwahl -> Elza, Raumvariante 11x1) oder "leon"; _extra = weitere KEY=WERT-Paare
function(r38_exe _lauf _dir _start _exit _extra)
    file(MAKE_DIRECTORY "${_dir}")
    set(WORKDIR "${_dir}")
    if(_start STREQUAL "elza")
        set(_st RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
    else()
        set(_st RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
    endif()
    set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 ${_st} RE15_WINDOW_SCALE=1 "RE15_DEBUG_JUMP=1120@250"
             "RE15_PLAYER_POS=2500,10000,2048" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=30
             "RE15_INPUT_SCRIPT=W1,R1.0,W4" "RE15_EXIT_AT=${_exit}" ${_extra})
    re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
    if(NOT _rv EQUAL 0)
        set(_z "")
        if(EXISTS "${_dir}/debug.log")
            file(STRINGS "${_dir}/debug.log" _z)
        endif()
        list(LENGTH _z _n)
        if(_n LESS 10 OR "${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1")
            message(STATUS "${_tag} [${_lauf}]: Lauf ohne Ergebnis (exit=${_rv}, debug.log ${_n} Zeilen) -> EIN Wiederholungsversuch")
            file(REMOVE "${_dir}/debug.log" "${_dir}/zopf.log")
            re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
        endif()
    endif()
    if(NOT _rv EQUAL 0)
        r38_fehler("${_lauf}" "re15_pc.exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
    endif()
    if(NOT EXISTS "${_dir}/debug.log")
        r38_fehler("${_lauf}" "kein debug.log in ${_dir}")
    endif()
    file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
    if(NOT _ex)
        r38_fehler("${_lauf}" "RE15_EXIT_AT nicht erreicht (keine Zeile '[flow] EXIT_AT') — Haenger?")
    endif()
endfunction()

# ---- Elza: Zopf pendelt (Winkelverlauf) + Bild 50 mit Zopf
set(_d "${_wurzel}/elza")
r38_exe("elza" "${_d}" elza "220#1121" "RE15_ZOPF_LOG=zopf.log;RE15_FRAMEDUMP=50:mit.ppm")
if(NOT EXISTS "${_d}/zopf.log" OR NOT EXISTS "${_d}/mit.ppm")
    r38_fehler("elza" "zopf.log / mit.ppm fehlt in ${_d}")
endif()
file(STRINGS "${_d}/debug.log" _md REGEX "\\[md1\\] loaded test\\.md1: 21 meshes")
if(NOT _md)
    r38_fehler("elza" "Elzas Spielermodell hat nicht 21 Teile (PL04) — '[md1] loaded test.md1: 21 meshes' fehlt")
endif()
execute_process(COMMAND "${RE15_ZOPF_TOOL}" spur "${_d}/zopf.log" RESULT_VARIABLE _rs OUTPUT_VARIABLE _os ERROR_VARIABLE _os)
message(STATUS "${_os}")
if(NOT _rs EQUAL 0)
    r38_fehler("elza" "Winkelverlauf des Zopfs falsch (test_r38_zopf spur)")
endif()

# ---- Elza mit Mess-Sonde: Zopf NICHT gezeichnet, sonst dasselbe Bild -> Pixel am Hinterkopf
set(_d2 "${_wurzel}/elza_aus")
r38_exe("elza_aus" "${_d2}" elza "60#1121" "RE15_ZOPF_SONDE_AUS=1;RE15_FRAMEDUMP=50:ohne.ppm")
if(NOT EXISTS "${_d2}/ohne.ppm")
    r38_fehler("elza_aus" "ohne.ppm fehlt in ${_d2}")
endif()
execute_process(COMMAND "${RE15_ZOPF_TOOL}" pixel "${_d}/mit.ppm" "${_d2}/ohne.ppm" 195 75 255 135
                RESULT_VARIABLE _rp OUTPUT_VARIABLE _op ERROR_VARIABLE _op)
message(STATUS "${_op}")
if(NOT _rp EQUAL 0)
    r38_fehler("pixel" "Zopf-Pixel am Hinterkopf fehlen oder liegen ausserhalb des Fensters (test_r38_zopf pixel)")
endif()

# ---- Leon: unveraendert, kein Zopf
set(_d3 "${_wurzel}/leon")
r38_exe("leon" "${_d3}" leon "60#1120" "RE15_ZOPF_LOG=zopf.log")
file(STRINGS "${_d3}/debug.log" _ml REGEX "\\[md1\\] loaded test\\.md1: 17 meshes")
if(NOT _ml)
    r38_fehler("leon" "Leons Spielermodell hat nicht 17 Teile (PL00)")
endif()
set(_zl "")
if(EXISTS "${_d3}/zopf.log")
    file(STRINGS "${_d3}/zopf.log" _zl REGEX "^Z[0-9]+ ")
endif()
list(LENGTH _zl _nz)
if(NOT _nz EQUAL 0)
    r38_fehler("leon" "Leon rechnet einen Zopf (${_nz} Schritte) — darf er nicht (Modell-Bit 2 @0x80031d8c)")
endif()
# ---- Elza: Gehen + Rennen, Eingabe der Original-Messung (Dossier §4.4)
set(_d4 "${_wurzel}/elza_lauf")
r38_exe("elza_lauf" "${_d4}" elza "560#1121"
        "RE15_ZOPF_LOG=zopf.log;RE15_PLAYER_POS=150,9550,2048;RE15_INPUT_SCRIPT=W4.5,R0.9667,W3.0333,U1.3333,W2.0,R0.7,W1.0,XU0.4667,W3.0")
if(NOT EXISTS "${_d4}/zopf.log")
    r38_fehler("elza_lauf" "zopf.log fehlt in ${_d4}")
endif()
execute_process(COMMAND "${RE15_ZOPF_TOOL}" lauf "${_d4}/zopf.log" RESULT_VARIABLE _rl OUTPUT_VARIABLE _ol ERROR_VARIABLE _ol)
message(STATUS "${_ol}")
if(NOT _rl EQUAL 0)
    r38_fehler("elza_lauf" "Zopf beim Gehen/Rennen nicht wie im Original (test_r38_zopf lauf)")
endif()
execute_process(COMMAND "${RE15_ZOPF_TOOL}" huelle "${_d4}/zopf.log" OUTPUT_VARIABLE _oh ERROR_VARIABLE _oh)
message(STATUS "${_oh}")
message(STATUS "${_tag}: ALLE TEILE OK (elza spur, pixel, leon ohne Zopf, elza_lauf) in ${_wurzel}")
