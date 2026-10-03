# Runde 35 Spur G "karte" — ECHTER LAUF der echten exe: Spielstand (Slot 0) im Raum schreiben,
# Titel -> LOAD GAME (RE15_CONTINUE_TEST + RE15_CARD_AUTO), Statusschirm -> MAP (RE15_INV_OPEN_AT),
# Abzug des Inventar-Framebuffers (RE15_INV_FB_SHOT) und Auswertung des BILDES (probe_r35_karte bild):
# rote Fuellung (aktueller Raum) NUR im erwarteten Kasten, gelbe RE2-Tuerbalken darin.
#   A  ROOM1230 (Ankunft aus 11D0)           -> B1-Blatt, Gang rect 0 (120,60)-(175,131)
#   B  ROOM11F0 (Ankunft aus 11E0)           -> B2-Blatt, rect 1 (100,101)-(139,156)
#   C  ROOM1200 (Ankunft aus 11E0)           -> B2-Blatt, rect 2 (141,101)-(172,140)
#   D  ROOM1210 (Ankunft aus 11E0)           -> B2-Blatt, rect 3 (187,70)-(250,149), 6 Tueren gelb
#   E  ROOM1120 -> Tuer -> ROOM1080 (Fahrstuhl von 3F aus betreten) -> 3F-Blatt, Kabine
#      rect 0 (127,137)-(142,152)  [Nutzerweg: Aktionstaste an der Fahrstuhltuer]
# Dossier: analysis/befunde_runde35/G_karte.md
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PROBE=<probe_r35_karte> -DWORKDIR=<dir> -P test_r35_karte.cmake
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r35_karte: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PROBE OR NOT EXISTS "${RE15_PROBE}")
    message(FATAL_ERROR "r35_karte: RE15_PROBE fehlt: '${RE15_PROBE}'")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
# Eigener exe-Name (Muster test_r34n_b_cursor.cmake): fremde Kills treffen den Lauf nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r35g_karte_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r35g_karte_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_fehler "")
function(karte_lauf _name _raum _kartenargs _skript _oeffnen_raum _kasten _min_gelb)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log" "${WORKDIR}/karte.bmp")
    execute_process(COMMAND "${RE15_PROBE}" karte "re15_card.mcr" ${_kartenargs}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r35_karte[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    set(_skriptenv "")
    if(NOT "${_skript}" STREQUAL "")
        set(_skriptenv RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
                       "RE15_INPUT_SCRIPT=${_skript}")
    endif()
    re15_start_spiel(_rv 240
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        ${_skriptenv}
        "RE15_INV_OPEN_AT=40#${_oeffnen_raum}"
        RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=60
        "RE15_EXIT_AT=200#${_oeffnen_raum}"
        "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/karte.bmp")
        message(FATAL_ERROR "r35_karte[${_name}]: kein Kartenabzug (exit=${_rv}, ${WORKDIR})")
    endif()
    execute_process(COMMAND "${RE15_PROBE}" bild "${WORKDIR}/karte.bmp" ${_kasten} ${_min_gelb}
                    RESULT_VARIABLE _rvb OUTPUT_VARIABLE _outb)
    message(STATUS "r35_karte[${_name}]: ${_outb}")
    if(NOT _rvb EQUAL 0)
        set(_fehler "${_fehler} ${_name}" PARENT_SCOPE)
    endif()
endfunction()

# Besucht-Orte fuer die Blaetter (wie beim Durchlaufen): B2 = 11E0 + 11F0 + 1200 + 1210,
# B1 = 1190 + 11B0 + 11C0 + 11D0 + 1230.
set(_b2 besucht:11E0:-24707:-9442 besucht:11F0:250:250 besucht:1200:-20154:-25245
        besucht:1210:-26400:-2200)
set(_b1 besucht:1190:3174:-11182 besucht:11B0:-26483:-18094 besucht:11D0:-4729:-16018
        besucht:1230:-4729:-16018)
karte_lauf(A 1230 "1230;-4729;-16018;0;${_b1}" "" 1230 "120;60;175;131" 0)
karte_lauf(B 11F0 "11F0;250;250;0;${_b2}" "" 11F0 "100;101;139;156" 0)
karte_lauf(C 1200 "1200;-20154;-25245;0;${_b2}" "" 1200 "141;101;172;140" 0)
karte_lauf(D 1210 "1210;-26400;-2200;0;${_b2}" "" 1210 "187;70;250;149" 30)
# E: vor der Fahrstuhltuer von ROOM1120 (Tuer @0xCB6 r(300,5900,2000,1000)), Blick -z
# (Yaw 1024 = (cos,-sin) = -z, player_common.c:1292), Aktionstaste -> ROOM1080.
karte_lauf(E 1120 "1120;1300;6600;1024;besucht:1120:1300:7300" "W1,A0.3" 1080
           "127;137;142;152" 0)

file(REMOVE "${_exe_kopie}")
if(NOT "${_fehler}" STREQUAL "")
    message(FATAL_ERROR "r35_karte: Bildpruefung ROT bei:${_fehler}")
endif()
message(STATUS "r35_karte: alle Laeufe OK")
