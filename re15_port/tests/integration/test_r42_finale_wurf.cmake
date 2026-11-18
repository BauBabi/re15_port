cmake_minimum_required(VERSION 3.16)   # cmake -P: Policies wie die uebrigen Integrationsskripte (Linux-Container CMake 3.28)
# =============================================================================
# FINALE 5090/5091: DIE WURFSZENE. Runde 42 Strang finale, Stufe 3 (Plan S5, K-13; include/re15_finale_wurf_r42.h), Runde 4.
#
# NUTZERWUNSCH 07.11.2026: "... Das jemand Leon den Raketenwerfer zuwirft ...". NUTZERENTSCHEID 17.11.2026: "Ok, passt für mich. Aber
# ein Spruch wäre schon gut beim zuwerfen, so etwas wie: Leon, send him to hell, oder Leon, let's finish this oder sowas."
# NUTZER 18.11.2026: "... würde ich John eher am Eingang stehen lassen ..." / "John kann danach gerne wieder raus laufen. ..."
#
# Ein Lauf je Figur (TEIL leon = ROOM5090, elza = ROOM5091): Debug-Sprung nach Wagen B, durch die Tuer nach Wagen C bis zum
# Kampfbeginn (Weg wie integration_r42_finale_schuss), dann OHNE Waffe/Messhaken warten. Pruefhaken RE15_FINALE_UHR=60 kuerzt nur die
# Uhr (Spielwert 1800 prueft unit_r42_finale). KEIN Haken stellt den Spieler um: der Werfer landet neben ihm, die Aufnahme laeuft ueber
# den echten Item-Weg (Taste, Ja) von dort, wo er steht. Geprueft aus finale.log (Takte = Spieltakt des Finale-Moduls) gegen RE2
# ROOM7020 sub12/sub13 und den RE1.5-Wurf:
#   W = "Bild 0" ein Takt nach dem Szenenstart: John erscheint an der Ankunft (24800,-23550) (Tuer Slot 2 @0x0108E), Schnitt 13;
#   Loslassen im Clip-Bild 22 (@0x800336fc) bei W+53 (Sleep 1 + 30 @0x0187C/@0x01892, dann 22 Bilder Wurf), Zuruf Id 3 im Bild danach;
#   Landung nach 28 Flugbildern (RE2-Flugdauer) mit Se_on Raumbank 0x0E im selben Takt (@0x019DE-E2), hoechstens 1700 vom Spieler
#   (Spieler im RE2-Aufnahme-Rechteck 3400x2500);
#   Schnitt auf die Kamera des Spielers waehrend des Flugs, sobald der Werfer eine Werferlaenge (1771) von der Hand weg ist; Reaktion 16
#   Takte nach der Landung (Do/Edwhile + Sleep 15 @0x018C4-D0);
#   Runde 3: John LAEUFT zur Tuer (Plc_dest Modus 5) und kommt innerhalb von 10..60 Takten an, Tuerton der Tuersequenz (DOOR2A V2,
#   Eintrag 0); wie RE2 erst der Schnitt zurueck (@0x01948), dann das Parken (@0x01958): John wird NIE in einem Bild geparkt, in dem
#   der Schnitt 13 (oder eine Kamera, die ihn sieht) noch gilt - Riegel gegen Rot 1 aus s3 r2 ("verschwindet vor der Tuer");
#   Aufnahme-AOT Slot 6 Gegenstand 0x12 Anzahl 2 Taken-Bit 137, nach dem Aufheben Zellen 0/1 = 0x12 mit Anzahl 2.
#   Runde 4: die Form ruht waehrend der ganzen Szene SICHTBAR (keine Zeile "Form gezeichnet 0"; RE2 @0x80026590-9c blendet nicht aus).
#   debug.log: Schnitte 13 (W) / 13 (Reaktion) / zurueck auf die gemerkte Kamera (Ende), John aus JOHN_49_WURF.EMD mit 26 Clips, der
#   Werfer in der Hand im Schnitt 13, im Flug im Schnitt 13 und in der Kamera des Spielers, am Boden in der Kamera des Spielers.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=leon|elza -P test_r42_finale_wurf.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_finale_wurf: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_finale_wurf: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL leon)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r42wurf_${TEIL}_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_raum 5090)
set(_figur_env "")
if(TEIL STREQUAL "elza")
    set(_raum 5091)
    set(_figur_env RE15_TITLE_SHOT_CHAR=1)
endif()
re15_start_spiel(_rv 500
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_figur_env}
    RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=500,-9175,2048,0"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=A0.1,W1,U8.5,W7.3,A0.1,W3.5,A0.1,W4"
    RE15_FINALE_LOG=1 RE15_FINALE_UHR=60 "RE15_EXIT_AT=1000"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")

set(_f "")
if(NOT EXISTS "${WORKDIR}/finale.log" OR NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r42_finale_wurf_${TEIL}: finale.log/debug.log fehlt (exit=${_rv})")
endif()
file(READ "${WORKDIR}/finale.log" _log)
file(READ "${WORKDIR}/debug.log" _dbg)
function(takt_von _muster _var)
    if(_log MATCHES "\\[finale\\] T([0-9]+) F[0-9]+ ${_muster}")
        set(${_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    else()
        set(${_var} "" PARENT_SCOPE)
    endif()
endfunction()
takt_von("Wurf: Szene startet" _S)
takt_von("Wurf: Bild 0: John erscheint [(]Aktor [0-9]+[)] an [(]24800,-23550[)]" _W)
if(NOT _S OR NOT _W)
    message(FATAL_ERROR "r42_finale_wurf_${TEIL}: keine Wurfszene / kein Bild 0 im finale.log (exit=${_rv})\n${_log}")
endif()
math(EXPR _ws "${_W} - ${_S}")
if(NOT _ws EQUAL 1)
    string(APPEND _f "\n Bild 0 ${_ws} Takte nach dem Szenenstart statt 1 (VM laeuft vor dem Spielzug)")
endif()
foreach(_paar "Wurf: Loslassen im Clip-Bild 22 bei W[+]53:53" "Wurf: Zuruf an [(]Id 3[)] bei W[+]54:54")
    string(REGEX MATCH "^(.*):([0-9]+)$" _x "${_paar}")
    set(_m "${CMAKE_MATCH_1}")
    set(_soll "${CMAKE_MATCH_2}")
    takt_von("${_m}" _t)
    if(NOT _t)
        string(APPEND _f "\n '${_m}' fehlt")
    else()
        math(EXPR _ist "${_t} - ${_W}")
        if(NOT _ist EQUAL _soll)
            string(APPEND _f "\n '${_m}' bei W+${_ist} statt W+${_soll}")
        endif()
    endif()
endforeach()
takt_von("Wurf: Loslassen" _LOS)
takt_von("Wurf: Landung nach 28 Flugbildern" _L)
takt_von("Se_on Raumbank R702 Satz 0x0E [(]PC-Haken 1[)]" _SE)
takt_von("Wurf: Schnitt auf die Kamera des Spielers" _SK)
takt_von("Wurf: Reaktion" _R)
takt_von("Wurf: John an der Tuer" _J)
takt_von("Wurf: John geparkt" _P)
takt_von("Wurf: Ende" _E)
if(NOT _LOS OR NOT _L OR NOT _SE OR NOT _SK OR NOT _R OR NOT _J OR NOT _E OR NOT _P)
    string(APPEND _f "\n Loslassen '${_LOS}' / Landung 28 '${_L}' / Se_on 0x0E '${_SE}' / Schnitt Spieler '${_SK}' / Reaktion '${_R}' / John an der Tuer '${_J}' / Ende '${_E}' / John geparkt '${_P}' fehlt")
else()
    math(EXPR _dl "${_L} - ${_LOS}")
    if(NOT _dl EQUAL 28)
        string(APPEND _f "\n Landung ${_dl} Takte nach dem Loslassen statt 28")
    endif()
    if(NOT _SE EQUAL _L)
        string(APPEND _f "\n Landeton T${_SE} nicht im Landetakt T${_L} (@0x019E2)")
    endif()
    if(_SK LESS_EQUAL _LOS OR _SK GREATER _L)
        string(APPEND _f "\n Schnitt auf den Spieler T${_SK} nicht im Flug (T${_LOS}..T${_L})")
    endif()
    math(EXPR _dr "${_R} - ${_L}")
    if(NOT _dr EQUAL 16)
        string(APPEND _f "\n Reaktion ${_dr} Takte nach der Landung statt 16 (Do/Edwhile + Sleep 15)")
    endif()
    math(EXPR _dj "${_J} - ${_R}")
    if(_dj GREATER 60 OR _dj LESS 10)
        string(APPEND _f "\n John nach ${_dj} Takten an der Tuer (erwartet 10..60: umdrehen, laufen)")
    endif()
    math(EXPR _de "${_E} - ${_J}")
    if(_de LESS 0 OR _de GREATER 1)
        string(APPEND _f "\n Ende ${_de} Takte nach Johns Ankunft an der Tuer statt 0..1")
    endif()
    if(_P LESS _E)
        string(APPEND _f "\n John geparkt T${_P} VOR dem Schnitt zurueck T${_E} (RE2: Cut_chg @0x01948 vor Pos_set @0x01958)")
    endif()
    if(NOT _log MATCHES "Wurf: John ausser Bild der Kamera [0-9]+ (nach dem Schnitt zurueck|- jetzt geparkt)")
        string(APPEND _f "\n John nicht ausser Bild geparkt")
    endif()
endif()
if(NOT _log MATCHES "Wurf: Schnitt: Werfer ([0-9]+) von der Loslasslage [(]Werferlaenge 1771[)]")
    string(APPEND _f "\n Schnitt im Flug nicht nach einer Werferlaenge")
elseif(CMAKE_MATCH_1 LESS 1771)
    string(APPEND _f "\n Schnitt im Flug schon bei ${CMAKE_MATCH_1} < 1771")
endif()
if(NOT _log MATCHES "Wurf: Tuerton: Archiv 2 DOOR2A Variante 2" OR NOT _log MATCHES "Wurf: Tuerton: Seite S[0-9]+, Se_on Tuerbank Eintrag 0")
    string(APPEND _f "\n Tuerton beim Abgang (DOOR2A V2, Eintrag 0) fehlt")
endif()
if(NOT _dbg MATCHES "Werfer-Modell PLD/PL00W12.PLW geladen: Waffenband 38 von 72 Dreiecken")
    string(APPEND _f "\n Werfer nicht als PL00W12-Waffenband geladen")
endif()
if(_log MATCHES "Abgang per Notbremse")
    string(APPEND _f "\n John ging per Notbremse ab")
endif()
if(_log MATCHES "Landung: Abstand zum Spieler ([0-9]+)")
    if(CMAKE_MATCH_1 GREATER 1700)
        string(APPEND _f "\n Werfer landet ${CMAKE_MATCH_1} vom Spieler (> 1700, Spieler ausserhalb des Aufnahme-Rechtecks)")
    endif()
else()
    string(APPEND _f "\n Abstand Landung - Spieler fehlt")
endif()
foreach(_m "Aufnahme-AOT Slot 6: Typ 2" "Aufnahme-AOT: Gegenstand 0x12 Anzahl 2 Taken-Bit 137" "aufgehoben: Zellen 0/1 = 0x12"
           "aufgehoben: Anzahl 2/2")
    if(NOT _log MATCHES "${_m}")
        string(APPEND _f "\n '${_m}' fehlt (Aufnahme)")
    endif()
endforeach()
# debug.log: Schnitte, Modell, Werfer gezeichnet
if(NOT _dbg MATCHES "\\[scd F${_W}\\] Cut_chg\\(13\\)")
    string(APPEND _f "\n Cut_chg(13) nicht im Bild 0 (F${_W})")
endif()
if(_R AND NOT _dbg MATCHES "\\[scd F${_R}\\] Cut_chg\\(13\\)")
    string(APPEND _f "\n Cut_chg(13) nicht im Reaktionsbild F${_R}")
endif()
set(_vorher "")
if(_log MATCHES "Kamera vorher ([0-9]+)")
    set(_vorher ${CMAKE_MATCH_1})
    if(_E AND NOT _dbg MATCHES "\\[scd F${_E}\\] Cut_chg\\(${_vorher}\\)")
        string(APPEND _f "\n Rueckschnitt auf ${_vorher} nicht im Endbild F${_E}")
    endif()
endif()
if(NOT _dbg MATCHES "EM49: Ersatzmodell RE2/JOHN_49_WURF.EMD \\(Raum ${_raum}\\)" OR NOT _dbg MATCHES "EM49 loaded: [0-9]+ meshes, 15 bones, 26 clips")
    string(APPEND _f "\n John nicht aus JOHN_49_WURF.EMD (26 Clips)")
endif()
foreach(_zc "1:13" "2:13" "2:${_vorher}" "3:${_vorher}")
    string(REPLACE ":" ";" _zl "${_zc}")
    list(GET _zl 0 _z)
    list(GET _zl 1 _c)
    if(NOT _dbg MATCHES "Werfer gezeichnet: Zustand ${_z} Lage [^\n]* Cut ${_c} Flaechen [1-9]")
        string(APPEND _f "\n Werfer im Zustand ${_z} nicht im Schnitt ${_c} gezeichnet")
    endif()
endforeach()
# Runde 4 (Pruefbericht s3 r3, Rot): die Form RUHT waehrend der Szene und bleibt SICHTBAR - RE2 @0x80026590-0x8002659c (+0x10E &
# 0x8000) ueberspringt nur den Typ-Handler (jalr @0x800265f4), blendet nicht aus. Runde 3 setzte no_draw = 1: die Form verschwand im
# Bild der Spielerkamera (F406) und stand ab F522 wieder da.
if(NOT _log MATCHES "Wurf: John-Gier [0-9]+ [(]Peilung zum Spieler[)], Form ruht sichtbar")
    string(APPEND _f "
 Form nicht als 'ruht sichtbar' gemeldet (Runde 3: unsichtbar)")
endif()
if(NOT _log MATCHES "Wurf: Form gezeichnet 1 bei W[+]")
    string(APPEND _f "
 Messzeile 'Form gezeichnet 1' fehlt")
endif()
if(_log MATCHES "Wurf: Form gezeichnet 0")
    string(APPEND _f "
 Form waehrend der Szene nicht gezeichnet (verschwindet sichtbar, Rot s3 r3)")
endif()
if(_log MATCHES "Ausloeser: Abschuss")
    string(APPEND _f "\n Abschuss ausgeloest (ohne Werfer in der Hand darf das nicht sein)")
endif()
if(_f)
    message(FATAL_ERROR "r42_finale_wurf_${TEIL}: FEHLER${_f}\n--- finale.log ---\n${_log}")
endif()
message(STATUS "r42_finale_wurf_${TEIL}: OK (W ${_W}, Loslassen T${_LOS}, Schnitt T${_SK}, Landung T${_L}, Reaktion T${_R}, John an der Tuer T${_J}, Ende T${_E}, geparkt T${_P})")
