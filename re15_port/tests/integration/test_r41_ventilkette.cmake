# =============================================================================
# VENTILKETTE MIT DER ECHTEN EXE (Runde 41 Spur C): der echte Weg 2030 -> Ost-Tuer -> ROOM20B0 und die Leiter 2000.
#
# Dossier analysis/befunde_runde41/C_ventilkette.md, Konstanten include/re15_ventilkette_r41.h. Der Unit-Riegel
# unit_r41_ventilkette misst die Mechanik Takt fuer Takt (auch Zaehler-Ende -> Flut -> Tod); dieser Riegel misst, was
# nur die exe zeigt: den Eingabepfad (Aktionstaste an den Ventilen, Ja/Nein, Warnung), den Zaehlerstart im Spiel-Takt,
# die umgelenkte Ost-Tuer (echter Tuer-Feuerpfad re15_aot_fire_slot -> Raumlader) und den Raumwechsel nach ROOM20B0
# mit dem Setzen der Bits 103 + 100; dazu die Leiter in 2000 als Sperrtext (kein Raumwechsel nach 2050).
#
#   A  ROOM2030 per Debug-Sprung, RE15_PLAYER_POS (-2650,7280) Blick +z Band 1 = am Sued-Landeplatz vor Ventil 09, mit
#      gesenkter Bruecke + Sicherung im Ventilkasten 2010 (Bits 101/102 + (9,125) = Bedingung der Nachtraege 9/16; (3,108)
#      dazu, weil die Sicherung vorher aufgehoben wurde). Nachbesserung 5: nach jedem Ja die RE2-Dreh-Animation (RE2 ROOM1080
#      sub08, 345 Takte) -> Wartezeit 450 statt 90; Zaehlerstart ~F4500, Ost-Tuer F4700. Nachbesserung 4
#      (Nachtrag 13): die richtige Folge 09/07/02/01 zu Fuss (RE15_WEGPLAN, Plan _ventile unten: Sued, Sued, Steg, Nord, Nord),
#      nach der vierten Drehung "RICHTIG" -> Ereignis 26 -> Warnung 46 -> Marker -> Zaehler. F4700 RE15_FIRE_AOT=0 (die
#      umgelenkte Ost-Tuer feuert ueber den echten Tuerpfad) -> ROOM20B0 geladen, "Zaehler lief beim Verlassen" (Bits 103 +
#      100), Ende 60 Bilder nach der Ankunft in 20B0 (der Bildzaehler g_engine.frame_count beginnt je Raum neu - Wegplan-Log
#      und RE15_EXIT_AT "#raum" zaehlen je Raum; die Framedumps F4440/F4560/F4680 liegen darum in 2030 und werden in 20B0
#      nicht ueberschrieben). Gemessen 2026-10-24 (Messlauf nb5 v2bc, Plan wie _ventile):
#      die Wegplan-WARTE ruht waehrend der Animation (player_mode 2: je Ja 795 statt 450 Bilder), Warnung zu F4483, Countdown
#      sichtbar ab F4500 (Bildausschnitt x 230..286 y 20..36 Mittel Rot 68 statt 15 davor).
#   B  ROOM2000 per Debug-Sprung + RE15_PLAYER_POS vor die Leiter (-26000,10300, Blick -x; Vorwaertspunkt 620 -> x -26620
#      liegt im Leiter-Rechteck x -27600..-26600 @0x0169E, bei -25900 nicht - gemessen 22:12): F330 Aktion ->
#      Sperrtext 40, KEIN Raumwechsel nach ROOM2050 bis zum Ende (F520).
#   C  Die Flut am Spiel (gemessen 2026-10-23 22:00, Dossier §4 "Flut am Spiel", C_belege/flut_echt_sheet.png; seit
#      Nachbesserung 4 ausgeloest ueber die richtige Folge zu Fuss): Start wie A, Plan _ventile + Flucht ueber Steg/Treppe auf den
#      Ring nach (6550,-200), dort stehen bleiben; die drei Spinnen als Spielzustand "schon getoetet" ueber ihre Kill-Bits
#      (Sce_em_set pc[7] = 0x9b/0x9c/0x9d @0x01C18/@0x01C2C/@0x01C40, EM-Status-Gate FUN_800420a0 @0x80042120-38, Zone 7 =
#      STAGE2) - kein Haken, kein Spielverhalten. Warnung zu -> Sleep 1800 -> Flut (Cut 4, Pad-Sperre), Wasser 50 je Takt ab
#      y 0: Fuesse (-5400) -> EIN Spritzer (Wasser-Schritt-Effekt 0x06050D00 @0x800456b8), -6300 = 900 ueber den Fuessen ->
#      hp -1 + blaue Blende (fade-log kick ch=0 value=0x5000), Todeskette. Riegel: Zaehler-Ende 1800 +- 3 Bilder nach dem
#      Schliessen, Tod 125 +- 2 Bilder nach dem Flutbeginn, Blende, Wasser -6300 / Spieler -5400.
#      Nachbesserung 1 (Abnahme 0, M4): C laeuft im BESCHLEUNIGTEN Renderer (direct3d/opengl, wie beim Nutzer), weil SDLs
#      Software-Renderer die subtraktive Blende ABR 2 nicht kann. Die Farbe der Bildmitte wird an Framedumps je 6 Bilder im
#      Fenster F6300..F6594 gemessen (das Todesbild haengt vom Weg ab; Nachbesserung 4 F3650, Nachbesserung 5 F6408): der letzte Dump <= Tod - 4 und der erste >= Tod + 5
#      (Werkzeug test_r41_ventilkette ppm): vorher Rot und Gruen >= 15 (unverschleiert), nachher Rot und Gruen <= 4 und
#      Blau >= 20 (Rot+Gruen abgezogen, Blau bleibt).
#
#   D  Nachtraege 9/16: Ventil 09 (Slot 19, Ereignis 22) ohne die Sicherung im Ventilkasten 2010 ((9,125) = 0; Bits 101/102
#      gesetzt) -> Port-Programm 2030_ventile_vorher, Text 49 = Original "It's locked." (ROOM20A0 msg 1 @0x203C), KEIN
#      Zaehler, KEINE Flut.
#
#   E  Abnahme 4 Mangel 2 (Nachbesserung 5): Start wie A, Plan _ventile, nach dem Schliessen der Warnung zurueck ueber den
#      Steg an das Plakat (Sued-Landeplatz, Slot 21, Ereignis 23) und Aktion -> waehrend des Zaehlers oeffnet das Plakat
#      NICHT ("Plakat gesperrt"); stehen bleiben -> Flut -> die Pad-Sperre der Flut (ROOM2030 sub02 @0x01E30 Form) bleibt bis
#      zum Ertrinken: zwischen "FLUT: Wasser steigt" und "ERTRUNKEN" kein "letterbox closed -> gameplay" (der Fehlerfall der
#      Abnahme 4, Lauf pk: F3555 mitten in der Flut).
#
# Die Spinnen (main00 Switch auf den Eintritts-Cut, Case 0 @0x01C12) stehen am Ring; A, C, D und E laufen ohne sie (Kill-Bits).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_R41C_WERKZEUG=<test_r41_ventilkette> -DWORKDIR=<dir> -P test_r41_ventilkette.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_ventilkette: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_R41C_WERKZEUG OR NOT EXISTS "${RE15_R41C_WERKZEUG}")
    message(FATAL_ERROR "r41_ventilkette: RE15_R41C_WERKZEUG (test_r41_ventilkette, ppm-Mittel) fehlt: '${RE15_R41C_WERKZEUG}'")
endif()
if(NOT RE15_R41C_RE2TEX OR NOT EXISTS "${RE15_R41C_RE2TEX}")
    message(FATAL_ERROR "r41_ventilkette: RE15_R41C_RE2TEX (shared_assets/RE2/TEX.TIM, Schrift des RE2-Countdowns) fehlt: '${RE15_R41C_RE2TEX}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_ventilkette: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r34n_b_cursor.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41c_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r41c_haken_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r41c_haken_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(_muss _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r41_ventilkette[${_name}]: ${_was} — '${_wort}' fehlt im debug.log")
    endif()
endfunction()
function(_darf_nicht _text _wort _name _was)
    string(FIND "${_text}" "${_wort}" _p)
    if(NOT _p LESS 0)
        message(FATAL_ERROR "r41_ventilkette[${_name}]: ${_was} — '${_wort}' steht im debug.log")
    endif()
endfunction()

# Plan der richtigen Folge 09/07/02/01 (Kopf re15_ventilkette_r41.h "Ventil-RAEDER"; Spieler z 7280 Blick +z vor der
# Suedwand, z -7880 Blick -z vor der Nordwand; Steg ueber x -1050). Endet nach dem Wegdruecken der Warnung.
set(_ventile "RAUM 2030
WARTE 30
GEHE -2650 7280 100
BLICK 3072
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE 2600 7280 100
BLICK 3072
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE -1050 6900 100
GEHE -1050 6000
GEHE -1050 -6400
GEHE -1050 -7400 100
GEHE 150 -7880 100
BLICK 1024
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
GEHE -1350 -7880 100
BLICK 1024
WARTE 10
AKTION
WARTE 150
AKTION
WARTE 450
AKTION
WARTE 30
")

# ---- A: 2030 Ventile (richtige Folge zu Fuss) -> Zaehler -> Ost-Tuer -> 20B0 --------------------------------------
set(WORKDIR "${_basis}_a")
file(MAKE_DIRECTORY "${WORKDIR}")
file(WRITE "${WORKDIR}/plan.txt" "${_ventile}ENDE\n")
re15_start_spiel(_rva 600
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=2030@250
    "RE15_PLAYER_POS=-2650,7280,3072,1"
    "RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102,3:108,9:125"
    "RE15_WEGPLAN=${WORKDIR}/plan.txt" RE15_WEGPLAN_LOG=wegplan.txt
    RE15_MSG_LOG=1
    "RE15_FIRE_AOT=0@4700#2030"
    "RE15_FRAMEDUMP=4440-4680/120:${WORKDIR}/f_"
    "RE15_EXIT_AT=60#20B0"
    "${RE15_PC_EXE}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r41_ventilkette[a]: kein debug.log (exit=${_rva}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
_muss("${_dbg}" "JUMP -> 203 TREATMENT POOL (ROOM2030)" a "Sprung nach 2030")
_darf_nicht("${_dbg}" "[ventilkette41] ROOM2030: Bruecke obj 0 oben" a "mit Bit 102 steht die Bruecke unten")
_muss("${_dbg}" "[ventilkette41] ROOM2030 Ereignis 22 -> Port-Programm 2030_ventil_09" a "Aktion an Ventil 09 startet seine Frage")
_muss("${_dbg}" "[msg] room=2030 id=55" a "Frage 55 (Ventil 09)")
_muss("${_dbg}" "[ventilkette41] Folge 09 07 02 01 -> RICHTIG: Warnung, Zaehler, Flut" a "richtige Folge nach der vierten Drehung")
_muss("${_dbg}" "[ventilkette41] ROOM2030 Ereignis 26 -> Port-Programm 2030_ventile_flut" a "Ereignis 26 vom C-Takt")
_muss("${_dbg}" "[msg] room=2030 id=46" a "Warnung 46")
_muss("${_dbg}" "[msgtext] room=2030 id=46 \"WARNING: The water will rise in one minute. Please leave the room immediately!\"" a "Warnung 46 im Wortlaut des Nachtrags 19")
_muss("${_dbg}" "Plc_motion(entity=0, motion=17, flags=0x00)" a "RE2-Dreh-Animation (Clip 17, RE2 ROOM1080 sub08) beim Drehen")
_muss("${_dbg}" "[ventilkette41] Zaehler laeuft: 1800 Takte, Ost-Tuer -> 20B0, Wasser geparkt" a "Zaehler im Spiel-Takt")
_muss("${_dbg}" "[ventilkette41] ROOM2030: Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)" a "Ost-Tuer umgelenkt")
_muss("${_dbg}" "[fire-aot] slot=0" a "Ost-Tuer gefeuert (echter Tuerpfad)")
_muss("${_dbg}" "[esp] room 20B0:" a "ROOM20B0 geladen (Umlenkung wirksam)")
_muss("${_dbg}" "[ventilkette41] Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100" a "Flut gelaufen + Leiter frei")
_darf_nicht("${_dbg}" "[esp] room 2070:" a "ohne Umlenkung waere die Ost-Tuer nach ROOM2070 gegangen")
_muss("${_dbg}" "EXIT_AT: Bild" a "Endbild in ROOM20B0 erreicht")
# Countdown der 60 s (Nachtrag 18 Punkt 4) in der Form der RE2-Anzeige (Nachbesserung 6, Abnahme 5 Mangel 2, Nachtrag 25 (p);
# include/re15_ventilkette_r41.h "Countdown"): Bildvergleich RE2 <-> Port. Das Werkzeug "re2cd" baut RE2s Zeichner FUN_800318a4
# EIGENSTAENDIG aus der RE2-TEX.TIM nach (14x14-Felder uv (c%18*14, c/18*14), x 104 + 14 i, y 32, CLUT 480 + 2*Farbe) und
# verlangt, dass JEDES deckende Texel des Kandidaten im Framedump steht. F4440 (Warnung noch offen) = kein Treffer; F4560/F4680
# = Treffer in Farbe 2 (Rot, unter 60 s @0x800260ec) mit genau dem Text, den RE2s Zaehler fuer dieses Bild zeigt: Bild k =
# Framedump - erstes Countdown-Bild (Log "Countdown RE2-Form Bild 0 ... F<n>"), Sekunde 59 - k/30, dann 9 - (k%30)/3 und
# 8 - 3*(k%3) (@0x800260b8..@0x800260d8, @0x80026188, @0x8002634c..@0x80026360). Gemessen 2026-10-25 (Messlauf nb6
# al_flucht_leon, beschleunigt): erstes Bild F4484, F4500 " 0^59=45", F4520 " 0^58=78", F4600 " 0^56=12", F5000 " 0^42=78".
string(REGEX MATCH "Countdown RE2-Form Bild 0 \" 0\\^59=98\" Farbe 2 \\(x 104 y 32\\) F([0-9]+)" _cd0 "${_dbg}")
if(NOT _cd0)
    message(FATAL_ERROR "r41_ventilkette[a]: kein erstes Countdown-Bild \" 0^59=98\" (Farbe 2, x 104 y 32) im debug.log")
endif()
set(_cd_f0 "${CMAKE_MATCH_1}")
function(_re2cd _bild _soll_text)
    set(_ppm "${WORKDIR}/f_00${_bild}.ppm")
    if(NOT EXISTS "${_ppm}")
        message(FATAL_ERROR "r41_ventilkette[a]: Framedump F${_bild} fehlt (${_ppm})")
    endif()
    execute_process(COMMAND "${RE15_R41C_WERKZEUG}" re2cd "${_ppm}" "${RE15_R41C_RE2TEX}" OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    if(NOT _r EQUAL 0)
        message(FATAL_ERROR "r41_ventilkette[a]: re2cd F${_bild}: ${_o}")
    endif()
    string(STRIP "${_o}" _o)
    message(STATUS "r41_ventilkette[a]: F${_bild}: ${_o}")
    if(_soll_text STREQUAL "")
        if(NOT _o MATCHES "^RE2CD KEIN TREFFER")
            message(FATAL_ERROR "r41_ventilkette[a]: vor dem Zaehler (F${_bild}) steht schon ein Countdown im Bild: ${_o}")
        endif()
    else()
        string(REPLACE "^" "\\^" _soll_re "${_soll_text}")
        if(NOT _o MATCHES "^RE2CD TREFFER \"${_soll_re}\" FARBE 2 PIXEL [0-9]+")
            message(FATAL_ERROR "r41_ventilkette[a]: F${_bild} zeigt nicht den RE2-Countdown \"${_soll_text}\" in Farbe 2 (${_o})")
        endif()
    endif()
endfunction()
function(_cd_soll _bild _out)
    math(EXPR _k "${_bild} - ${_cd_f0}")
    math(EXPR _sek "59 - ${_k} / 30")
    math(EXPR _z "9 - (${_k} % 30) / 3")
    math(EXPR _e "8 - 3 * (${_k} % 3)")
    if(_sek LESS 10)
        set(_sek "0${_sek}")
    endif()
    set(${_out} " 0^${_sek}=${_z}${_e}" PARENT_SCOPE)
endfunction()
_re2cd(4440 "")
_cd_soll(4560 _t1)
_cd_soll(4680 _t2)
_re2cd(4560 "${_t1}")
_re2cd(4680 "${_t2}")
message(STATUS "r41_ventilkette[a]: RE2-Countdown erstes Bild F${_cd_f0}, F4560 \"${_t1}\", F4680 \"${_t2}\" (Bildvergleich RE2-Zeichner <-> Port gleich)")

# ---- B: 2000 Leiter = Sperrtext, kein Raumwechsel ------------------------------------------------------------
set(WORKDIR "${_basis}_b")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rvb 240
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=2000@240
    "RE15_PLAYER_POS=-26000,10300,2048"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330
    "RE15_INPUT_SCRIPT=A0.1,W2,A0.1,W3"
    RE15_MSG_LOG=1
    "RE15_FRAMEDUMP=360:${WORKDIR}/f2000_360.ppm"
    "RE15_EXIT_AT=520#2000"
    "${RE15_PC_EXE}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r41_ventilkette[b]: kein debug.log (exit=${_rvb}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbgb)
_muss("${_dbgb}" "[ventilkette41] ROOM2000: Leiter Slot 4 (@0x0169E) -> Sperrtext 40" b "Leiter als Sperrtext angelegt")
_muss("${_dbgb}" "[msgtext] room=2000 id=40" b "Aktion an der Leiter zeigt Text 40 (sce-1-Weg loggt [msgtext])")
_darf_nicht("${_dbgb}" "[esp] room 2050:" b "die Leiter darf nicht nach ROOM2050 fuehren")
_darf_nicht("${_dbgb}" "[aot] DOOR FIRE slot=4" b "Slot 4 darf keine Tuer mehr sein")
_muss("${_dbgb}" "EXIT_AT: Bild" b "Endbild in ROOM2000 erreicht")

# ---- C: 2030 Zaehler abgelaufen -> Flut ueber die Ringebene -> Ertrinken --------------------------------------
set(WORKDIR "${_basis}_c")
file(MAKE_DIRECTORY "${WORKDIR}")
file(WRITE "${WORKDIR}/plan.txt" "${_ventile}GEHE -1900 -7700 80
BLICK 2048
WARTE 20
AKTION
WARTE 300
GEHE -6800 -9675
GEHE 6575 -9675
GEHE 6550 -200
BLICK 3072
ENDE
")
re15_start_spiel(_rvc 720
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=2030@250
    "RE15_PLAYER_POS=-2650,7280,3072,1"
    "RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102,3:108,9:125"
    "RE15_WEGPLAN=${WORKDIR}/plan.txt" RE15_WEGPLAN_LOG=wegplan.txt
    RE15_MSG_LOG=1 RE15_FADE_LOG=1 RE15_STATE_LOG=state.txt
    "RE15_FRAMEDUMP=6300-6594/6:${WORKDIR}/f_"
    "RE15_EXIT_AT=6600#2030"
    "${RE15_PC_EXE}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r41_ventilkette[c]: kein debug.log (exit=${_rvc}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbgc)
_muss("${_dbgc}" "ABR2 (B-F) ok" c "Renderer mit subtraktiver Blende (ABR 2) - der Software-Renderer zeichnet die Ertrink-Blende deckend gelb (Nachbesserung 1, M4)")
_muss("${_dbgc}" "[ventilkette41] Folge 09 07 02 01 -> RICHTIG: Warnung, Zaehler, Flut" c "richtige Folge zu Fuss")
_muss("${_dbgc}" "[ventilkette41] ROOM2030 Ereignis 26 -> Port-Programm 2030_ventile_flut" c "Programm gestartet")
_muss("${_dbgc}" "[msg] room=2030 id=46" c "Warnung 46 (Ja gewaehlt)")
_muss("${_dbgc}" "[ventilkette41] Zaehler laeuft: 1800 Takte, Ost-Tuer -> 20B0, Wasser geparkt" c "Zaehler gestartet")
_muss("${_dbgc}" "[ventilkette41] FLUT: Wasser steigt" c "Zaehler abgelaufen -> Flut")
_muss("${_dbgc}" "room=2030 CUT 0 -> 4" c "Flut unter Cut 4 (Draufsicht)")
_muss("${_dbgc}" "[ventilkette41] ERTRUNKEN: Wasser y -6300, Spieler y -5400 -> hp -1, blaue Blende" c "Ertrinken 900 ueber den Fuessen auf dem Ring")
string(FIND "${_dbgc}" "[ventilkette41] SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y -5400)" _p_spritz)
string(FIND "${_dbgc}" "[ventilkette41] ERTRUNKEN:" _p_tot)
if(_p_spritz LESS 0 OR _p_tot LESS 0 OR NOT _p_spritz LESS _p_tot)
    message(FATAL_ERROR "r41_ventilkette[c]: kein Spritzer beim Erreichen der Fuesse vor dem Ertrinken (Spritzer ${_p_spritz}, tot ${_p_tot})")
endif()
string(REGEX MATCHALL "SPRITZER: Wasser y" _spritz_alle "${_dbgc}")
list(LENGTH _spritz_alle _n_spritz)
if(NOT _n_spritz EQUAL 1)
    message(FATAL_ERROR "r41_ventilkette[c]: ${_n_spritz} Spritzer-Ereignisse statt genau einem (Ereignis, keine Rate)")
endif()
_muss("${_dbgc}" "kick ch=0 value=0x5000" c "blaue Blende Kanal 0 (level 0x5000)")
_darf_nicht("${_dbgc}" "[esp] room 2070:" c "kein Raumwechsel waehrend der Flut")
_muss("${_dbgc}" "EXIT_AT: Bild" c "Endbild erreicht")
# Zeiten aus dem Zustandslog (Bildnummern des Spiels, RE15_STATE_LOG): Schliessen der Warnung = letzter Wechsel der
# Pause-Maske FFFF0007 -> 00000007 vor der Flut; Flut = erstes Bild mit cam=4; Tod = erstes Bild mit hp=-1.
if(NOT EXISTS "${WORKDIR}/state.txt")
    message(FATAL_ERROR "r41_ventilkette[c]: state.txt fehlt")
endif()
file(STRINGS "${WORKDIR}/state.txt" _st REGEX "^F[0-9]+ pad=")
set(_f_zu -1)
set(_f_flut -1)
set(_f_tot -1)
set(_pf_vorher "")
foreach(_z IN LISTS _st)
    string(REGEX MATCH "^F([0-9]+) " _m "${_z}")
    set(_f "${CMAKE_MATCH_1}")
    string(REGEX MATCH "pf=([0-9A-F]+)" _m2 "${_z}")
    set(_pf "${CMAKE_MATCH_1}")
    if(_f_flut LESS 0 AND _pf_vorher STREQUAL "FFFF0007" AND _pf STREQUAL "00000007")
        set(_f_zu "${_f}")
    endif()
    set(_pf_vorher "${_pf}")
    if(_f_flut LESS 0 AND _z MATCHES " cam=4 ")
        set(_f_flut "${_f}")
    endif()
    if(_f_tot LESS 0 AND _z MATCHES "hp=-1\\)")
        set(_f_tot "${_f}")
    endif()
endforeach()
message(STATUS "r41_ventilkette[c]: Warnung zu F${_f_zu}, Flut F${_f_flut}, tot F${_f_tot}")
if(_f_zu LESS 0 OR _f_flut LESS 0 OR _f_tot LESS 0)
    message(FATAL_ERROR "r41_ventilkette[c]: Bildnummern nicht gefunden (zu ${_f_zu}, Flut ${_f_flut}, tot ${_f_tot})")
endif()
math(EXPR _d_zaehler "${_f_flut} - ${_f_zu}")
math(EXPR _d_tod "${_f_tot} - ${_f_flut}")
if(_d_zaehler LESS 1798 OR _d_zaehler GREATER 1804)
    message(FATAL_ERROR "r41_ventilkette[c]: Zaehler ${_d_zaehler} Bilder (erwartet 1800 +- 3: Sleep 1800 '09 0a 08 07', 60 s bei 30 Takten)")
endif()
if(_d_tod LESS 123 OR _d_tod GREATER 127)
    message(FATAL_ERROR "r41_ventilkette[c]: Tod ${_d_tod} Bilder nach dem Flutbeginn (erwartet 125 +- 2: 126 Schritte x 50 = 6300 = -5400 - 900, der erste Schritt im Marker-Takt)")
endif()
# Farbe der Ertrink-Blende (M4): Mittel der Bildmitte 4 Bilder vor / nach dem Todesbild (Framedumps je 2 Bilder).
function(_mittel _bild _out)
    set(_ppm "${WORKDIR}/f_00${_bild}.ppm")
    if(NOT EXISTS "${_ppm}")
        message(FATAL_ERROR "r41_ventilkette[c]: Framedump F${_bild} fehlt (${_ppm})")
    endif()
    execute_process(COMMAND "${RE15_R41C_WERKZEUG}" ppm "${_ppm}" 120 80 200 160 OUTPUT_VARIABLE _o RESULT_VARIABLE _r)
    string(REGEX MATCH "MITTEL ([0-9]+) ([0-9]+) ([0-9]+)" _m "${_o}")
    if(NOT _m)
        message(FATAL_ERROR "r41_ventilkette[c]: ppm-Werkzeug F${_bild}: ${_o}")
    endif()
    set(${_out} "${CMAKE_MATCH_1};${CMAKE_MATCH_2};${CMAKE_MATCH_3}" PARENT_SCOPE)
endfunction()
# Dumps je 6 Bilder ab F6300: der letzte <= Tod - 4 und der erste >= Tod + 5. Nachbesserung 5: vier Drehungen mit der RE2-
# Animation (je 345 Takte) verschieben alles um ~2760 Bilder. Gemessen 2026-10-25 00:24 (Riegel-Lauf lauf_da6c18d5_c):
# Warnung zu F4482, Flut F6283 (+1801), Tod F6408 (+125); vorher (Nachbesserung 4, lauf_a8685a18_c) Tod F3650, Titel nach
# F3911 (Tod + 261) -> Schranken wie damals Tod - 60 .. Tod + 180, Ende F6600 = obere Schranke + 12 (vor dem Titel).
if(_f_tot LESS 6348 OR _f_tot GREATER 6588)
    message(FATAL_ERROR "r41_ventilkette[c]: Todesbild F${_f_tot} ausserhalb F6348..F6588 (Dump-Fenster F6300..F6594, Ende F6600 vor dem Titel; Weg oder Zaehler veraendert?)")
endif()
math(EXPR _bild_vor "((${_f_tot} - 4 - 6300) / 6) * 6 + 6300")
math(EXPR _bild_nach "((${_f_tot} + 5 - 6300 + 5) / 6) * 6 + 6300")
_mittel(${_bild_vor} _rgb_vor)
_mittel(${_bild_nach} _rgb_nach)
list(GET _rgb_vor 0 _vr)
list(GET _rgb_vor 1 _vg)
list(GET _rgb_nach 0 _nr)
list(GET _rgb_nach 1 _ng)
list(GET _rgb_nach 2 _nb)
message(STATUS "r41_ventilkette[c]: Blende Bildmitte F${_bild_vor} (${_rgb_vor}) -> F${_bild_nach} (${_rgb_nach})")
if(_vr LESS 15 OR _vg LESS 15)
    message(FATAL_ERROR "r41_ventilkette[c]: vor dem Tod (F${_bild_vor}) ist die Bildmitte schon dunkel (${_rgb_vor}) - Gegenprobe ungueltig")
endif()
if(_nr GREATER 4 OR _ng GREATER 4 OR _nb LESS 20)
    message(FATAL_ERROR "r41_ventilkette[c]: Ertrink-Blende F${_bild_nach} nicht blau (${_rgb_nach}; erwartet Rot/Gruen <= 4, Blau >= 20 - ABR 2 Maske Rot+Gruen @0x174a)")
endif()

# ---- D: Ventile ohne Sicherung im Ventilkasten 2010 (Nachtraege 9/16) ---------------------------------------------------
set(WORKDIR "${_basis}_d")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rvd 240
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=2030@240
    "RE15_PLAYER_POS=-2650,7280,3072,1"
    "RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102"
    "RE15_FIRE_AOT=19@330#2030"
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=450
    "RE15_INPUT_SCRIPT=A0.1,W4,A0.1,W4,A0.1,W4"
    RE15_MSG_LOG=1
    "RE15_EXIT_AT=900#2030"
    "${RE15_PC_EXE}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r41_ventilkette[d]: kein debug.log (exit=${_rvd}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbgd)
_muss("${_dbgd}" "[fire-aot] slot=19" d "Ventil 09 (Slot 19, Ereignis 22) gefeuert")
_muss("${_dbgd}" "[ventilkette41] ROOM2030 Ereignis 22 -> Port-Programm 2030_ventile_vorher" d "ohne Sicherung kein Ventilraetsel")
_muss("${_dbgd}" "[msgtext] room=2030 id=49 \"It's locked.\"" d "Text 49 = Original ROOM20A0 msg 1 @0x203C (Nachtrag 16 Folge 4)")
_darf_nicht("${_dbgd}" "Port-Programm 2030_ventil_09" d "nicht die Ventil-Frage")
_darf_nicht("${_dbgd}" "Zaehler laeuft" d "kein Zaehler ohne Sicherung")
_darf_nicht("${_dbgd}" "[msg] room=2030 id=46" d "keine Warnung ohne Sicherung")
_muss("${_dbgd}" "EXIT_AT: Bild" d "Endbild erreicht")

# ---- E: Plakat waehrend des Zaehlers gesperrt, die Flut-Sperre bleibt bis zum Tod (Abnahme 4 Mangel 2) -------------
# Ende F6450: Warnung zu ~F4482 und Flut ~F6283 wie C (gleicher Plan bis zur Warnung), Tod auf dem Landeplatz (y -1800) ~61
# Bilder nach dem Flutbeginn (Abnahme 4 Lauf pk: Flut F3524 -> ERTRUNKEN F3585), Titel erst ~260 Bilder nach dem Tod.
set(WORKDIR "${_basis}_e")
file(MAKE_DIRECTORY "${WORKDIR}")
file(WRITE "${WORKDIR}/plan.txt" "${_ventile}FLAG 5 29
GEHE -1050 -7400 100
GEHE -1050 -6400
GEHE -1050 6000
GEHE -1050 6900 100
GEHE -3650 7280 100
BLICK 3072
WARTE 10
AKTION
WARTE 60
FLAG 5 28
ENDE
")
re15_start_spiel(_rve 720
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=2030@250
    "RE15_PLAYER_POS=-2650,7280,3072,1"
    "RE15_SET_FLAG=7:155,7:156,7:157,9:101,9:102,3:108,9:125"
    "RE15_WEGPLAN=${WORKDIR}/plan.txt" RE15_WEGPLAN_LOG=wegplan.txt
    RE15_MSG_LOG=1
    "RE15_EXIT_AT=6450#2030"
    "${RE15_PC_EXE}")
if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r41_ventilkette[e]: kein debug.log (exit=${_rve}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbge)
set(_wpe "")
if(EXISTS "${WORKDIR}/wegplan.txt")
    file(READ "${WORKDIR}/wegplan.txt" _wpe)
endif()
_muss("${_dbge}" "[ventilkette41] Zaehler laeuft: 1800 Takte" e "Zaehler gestartet")
_muss("${_dbge}" "Plakat gesperrt (Zaehler/Flut laufen, ihre Pad-Sperre bleibt)" e "Aktion am Plakat waehrend des Zaehlers")
_darf_nicht("${_dbge}" "Port-Programm 2030_plakat" e "waehrend des Zaehlers oeffnet das Plakat nicht")
_muss("${_wpe}" "FLAG (5,28) = 0" e "Plakat-Marker (5,0x1C) bleibt 0")
string(FIND "${_dbge}" "[ventilkette41] FLUT: Wasser steigt" _p_flut_e)
if(_p_flut_e LESS 0)
    message(FATAL_ERROR "r41_ventilkette[e]: keine Flut")
endif()
string(SUBSTRING "${_dbge}" ${_p_flut_e} -1 _nach_flut_e)
string(FIND "${_nach_flut_e}" "[ventilkette41] ERTRUNKEN:" _p_tot_e)
if(_p_tot_e LESS 0)
    message(FATAL_ERROR "r41_ventilkette[e]: nach der Flut nicht ertrunken")
endif()
string(SUBSTRING "${_nach_flut_e}" 0 ${_p_tot_e} _flut_bis_tot_e)
string(FIND "${_flut_bis_tot_e}" "letterbox closed -> gameplay" _p_frei_e)
if(NOT _p_frei_e LESS 0)
    message(FATAL_ERROR "r41_ventilkette[e]: zwischen Flutbeginn und Tod gibt das Spiel den Spieler frei (Pad-Sperre der Flut aufgehoben, Abnahme 4 Mangel 2)")
endif()
_muss("${_dbge}" "EXIT_AT: Bild" e "Endbild erreicht")

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_ventilkette: E (Plakat waehrend des Zaehlers gesperrt, Flut-Sperre bis zum Tod) bestanden")
message(STATUS "r41_ventilkette: A (2030 richtige Folge 09/07/02/01 zu Fuss mit RE2-Dreh-Animation -> Warnung 46 (Nachtrag 19) -> Zaehler -> Ost-Tuer -> 20B0, Bits 103+100), B (Leiter 2000 = Text 40), C (Zaehler ${_d_zaehler} -> Flut -> tot nach ${_d_tod}, Blende ${_rgb_vor} -> ${_rgb_nach}) und D (ohne Kasten-Sicherung Text 49 \"It's locked.\", kein Zaehler) bestanden")
