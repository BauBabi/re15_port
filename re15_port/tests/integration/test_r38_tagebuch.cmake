cmake_minimum_required(VERSION 3.16)
# test_r38_tagebuch.cmake — INTEGRATION Runde 38 Spur G "tagebuch" an der ECHTEN exe (Messkopie unter eigenem Namen).
#   cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r36_elza_karte> -DRE15_BILD_TOOL=<probe_r38_tagebuch_bild>
#         -DWORKDIR=<dir> -DTEIL=<lesen|karte|leon> -P test_r38_tagebuch.cmake
#
#   lesen  Elza-Spielstand ROOM1011 NACH dem Dialog ((4,0xF1)/(4,0xFA)/(3,0x74) = sub02 @0x00AFE-B06), CONTINUE,
#          Elza vor Roy, VIERECK: "[tagebuch] ... erstes Mal: ablegen", KEIN "[msgtext] room=1011 id=18";
#          RECHTS bis zur Ende-Stellung: auf der letzten Seite steht "0513" in den 6 Farben der Textmaler-Zeile 482
#          (87 Pixel, nur im Code-Kasten); VIERECK + Bestaetigen -> Bit 68; am Evt_end die Kette "Eintrag 3 (ROOM10F1
#          Blatt 3 Rechteck 9) -> Folge 4 (ROOM1211 Blatt 1 Rechteck 3)", der Hinweis-Schirm blinkt erst die 2F-Kachel,
#          dann (Folge) die B2-Kachel (Bildfolge), dann zu.
#   karte  Spielstand mit gelesenem Tagebuch ((9,68)): normale Karte (Statusschirm -> MAP), HOCH -> 2F: Kachel 3/9
#          blinkt; RUNTER, RUNTER -> B2: Kachel 1/3 blinkt (Blaetter ohne Besuch waehlbar).
#   leon   Gegenprobe Leon ROOM1010, gleiche Flags, gleicher Druck: keine "[tagebuch]"-Zeile, kein Hinweis.
set(_tag "r38_tagebuch[${TEIL}]")
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_BILD_TOOL WORKDIR TEIL)
    if(NOT ${_v})
        message(FATAL_ERROR "${_tag}: ${_v} fehlt")
    endif()
endforeach()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_dir "${WORKDIR}/lauf_${TEIL}_${_lauf_id}")
file(MAKE_DIRECTORY "${_dir}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
set(_exe "${_exe_dir}/re15_pc_r38tagebuch_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

macro(fail)
    file(REMOVE "${_exe}")
    message(FATAL_ERROR "${_tag}: ${ARGN} (${_dir})")
endmacro()

macro(karte)
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${ARGN}
                    WORKING_DIRECTORY "${_dir}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0)
        fail("Kartenwerkzeug exit=${_rvk}: ${_outk}")
    endif()
endmacro()

macro(lauf _timeout)
    set(WORKDIR "${_dir}")
    re15_start_spiel(_rv ${_timeout} ${ARGN} "${_exe}")
    if("${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1" OR NOT EXISTS "${_dir}/debug.log")
        message(STATUS "${_tag}: Lauf ohne Ergebnis (exit=${_rv}) -> EIN Wiederholungsversuch")
        file(GLOB _alt "${_dir}/*.ppm")
        file(REMOVE "${_dir}/debug.log" ${_alt})
        re15_start_spiel(_rv ${_timeout} ${ARGN} "${_exe}")
    endif()
    if(NOT EXISTS "${_dir}/debug.log")
        fail("kein debug.log (exit=${_rv})")
    endif()
endmacro()

macro(bild_pruefen)
    execute_process(COMMAND "${RE15_BILD_TOOL}" ${ARGN} RESULT_VARIABLE _rvb OUTPUT_VARIABLE _outb ERROR_VARIABLE _errb)
    message(STATUS "${_tag}: ${_outb}")
    if(NOT _rvb EQUAL 0)
        fail("Bildpruefung ${ARGV0} exit=${_rvb}")
    endif()
endmacro()

# Elza vor dem liegenden Roy, Blick +z (Gier 3072): Vorwaertspunkt (x + 620 cos, z - 620 sin; @0x80042bd0) liegt im
# Rechteck von Slot 5 (x -2400..-1400, z 6950..7950, sub00 @0x00A52).
set(_stand 1011 -1900 6830 3072)
set(_basis RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_NOAUDIO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0)

if(TEIL STREQUAL "lesen")
    karte(${_stand} 4:0xF1 4:0xFA 3:0x74)
    # Viereck @60 = Untersuchen; Leser: RECHTS x6 bis zur Ende-Stellung, Viereck schliesst, Viereck bestaetigt "filed".
    lauf(900 ${_basis} RE15_MSG_LOG=1
         "RE15_PRESS=square@60,right@200,right@230,right@260,right@290,right@320,right@350,square@380,square@480"
         "RE15_FRAMEDUMP=360-1100/5:f_" "RE15_EXIT_AT=1100#1011")
    file(STRINGS "${_dir}/debug.log" _neu REGEX "\\[tagebuch\\] ROOM1011 msg 18 -> Roys Diary \\(Dokument 5, Bildsatz 30, erstes Mal: ablegen\\)")
    file(STRINGS "${_dir}/debug.log" _m18 REGEX "\\[msgtext\\] room=1011 id=18")
    file(STRINGS "${_dir}/debug.log" _zu REGEX "\\[tagebuch\\] Leser zu: Bit \\(9,68\\)=1, Kartenkette nach dem Evt_end")
    file(STRINGS "${_dir}/debug.log" _kette REGEX "\\[tagebuch\\] Evt_end erreicht: Kartenkette Eintrag 3 \\(ROOM10F1 Blatt 3 Rechteck 9\\) -> Folge 4 \\(ROOM1211 Blatt 1 Rechteck 3\\)")
    file(STRINGS "${_dir}/debug.log" _folge REGEX "\\[hint\\] F[0-9]+ Folge-Hinweis 3 -> 4")
    file(STRINGS "${_dir}/debug.log" _schl REGEX "\\[hint\\] F[0-9]+ schliessen")
    if(NOT _neu)
        fail("Roy untersuchen oeffnete nicht Roys Diary (erstes Mal)")
    endif()
    if(_m18)
        fail("die Code-Nachricht msg 18 erschien trotzdem: ${_m18}")
    endif()
    if(NOT _zu OR NOT _kette OR NOT _folge OR NOT _schl)
        fail("Ablage/Kette fehlt: zu='${_zu}' kette='${_kette}' folge='${_folge}' schliessen='${_schl}'")
    endif()
    # Ende-Stellung (RECHTS @350, Schliessen @380): Bild 360 zeigt die letzte Seite. Satz: p05 Zeile 5 Kern-x 11..42,
    # y 80..95; Textseite bei (25,30) (@0x80076170-84) -> Kasten x 36..67, y 110..125; 87 Kernpixel (Satzbericht).
    bild_pruefen(gruen "${_dir}/f_000360.ppm" 36 110 67 125 87)
    # Hinweis-Schirm: erst die 2F-Kachel (Blatt 3 Rechteck 9), dann die B2-Kachel (Blatt 1 Rechteck 3).
    bild_pruefen(blink "${_dir}/f_" 480 1100 5 3 9 200)
    bild_pruefen(blink "${_dir}/f_" 480 1100 5 1 3 200)
    message(STATUS "${_tag}: ${_kette}")
elseif(TEIL STREQUAL "karte")
    karte(${_stand} 4:0xF1 4:0xFA 3:0x74 9:68)
    # Statusschirm -> MAP nach 40 spielbaren Bildern (RE15_INV_OPEN_AT, main.c); HOCH -> 2F, RUNTER x2 -> B2 (1F -> B1
    # unbekannt, uebersprungen, menu_common.c Etagenwahl).
    lauf(900 ${_basis} "RE15_INV_OPEN_AT=40#1011" "RE15_PRESS=up@150,down@260,down@290"
         "RE15_FRAMEDUMP=150-420/4:f_" "RE15_EXIT_AT=430#1011")
    bild_pruefen(blink "${_dir}/f_" 154 258 4 3 9 200)
    bild_pruefen(blink "${_dir}/f_" 298 420 4 1 3 200)
elseif(TEIL STREQUAL "leon")
    karte(1010 -1900 6830 3072 4:0xF1 4:0xFA 3:0x74)
    lauf(900 ${_basis} RE15_MSG_LOG=1 "RE15_PRESS=square@60" "RE15_EXIT_AT=500#1010")
    file(STRINGS "${_dir}/debug.log" _tb REGEX "\\[tagebuch\\]")
    file(STRINGS "${_dir}/debug.log" _hint REGEX "\\[hint\\] F[0-9]+ begin")
    file(STRINGS "${_dir}/debug.log" _ein REGEX "\\[save\\] CONTINUE: resumed in room 1010")
    if(NOT _ein)
        fail("ROOM1010 nicht geladen")
    endif()
    if(_tb OR _hint)
        fail("Leon: Tagebuch/Hinweis griff: '${_tb}' '${_hint}'")
    endif()
else()
    fail("unbekannter TEIL")
endif()
file(REMOVE "${_exe}")
message(STATUS "${_tag}: OK")
