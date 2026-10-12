cmake_minimum_required(VERSION 3.16)
# =============================================================================
# KISTE MIT DER ECHTEN EXE (Runde 37 Spur M, Nachtrag 9 des Nutzers). Dossier analysis/befunde_runde37/M_kiste.md.
#
#   "In der Kiste muessen die Icons offenbar so weit wie moeglich rechts liegen, damit sie nicht vom Text
#    ueberdeckt werden. [...] beim scrollen/bewegen [...] Der Kasten muss layertechnisch ueber den Items liegen."
#
# Drei Laeufe auf dem echten Weg: Spielstand (probe_r37_kiste_karte) vor der Kiste, CONTINUE, Viereck in F40 oeffnet
# die Kiste ueber das Kisten-AOT (Vorwaertspunkt 620 im Rechteck, aot_common.c), Viereck F90 = Box-Seite, RUNTER F100,
# HOCH F120. Anzeige 1 Bild versetzt: Gleiten F102..F107 / F122..F127, Commit F108 / F128. RE15_FRAMEDUMP jedes Bild
# F95..F130 bei RE15_WINDOW_SCALE=1 (320x240). probe_r37_kiste_bild prueft:
#   - jedes Bild: Panel-Pixel ausserhalb des Listenfensters bitgleich BOXPANEL.BIN (kein Zeilen-Pixel ueber dem
#     Kasten, Rahmen unveraendert; RE2: Rahmen OT-Eintrag 2 ueber Symbolen/Namen 6 und Ziffern 3);
#   - Ruhebild F99: Symbol je Zeile buendig am Fensterrand (x 204-Breite), Spalte davor reiner Listengrund;
#   - F107 / F127: die einlaufende Zeile (RE2 scroll+5 / scroll-1) ist zu sehen.
# Raeume: Leon ROOM1150, Elza ROOM1151 (Kisten-AOT slot 5 bytegleich main00 @0x00DCE), Leon ROOM2010 (slot 5 main00
# @0x007A8, Rechteck x 3650..5250 z -6600..-5600).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r37_kiste_karte> -DRE15_BILD_TOOL=<probe_r37_kiste_bild>
#               -DRE15_BOXPANEL=<BOXPANEL.BIN> -DWORKDIR=<dir> -P test_r37_kiste.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_BILD_TOOL RE15_BOXPANEL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r37_kiste: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_kiste: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# ⛔ EIGENER EXE-NAME (Muster test_r34n_b_cursor.cmake): ein Bau in einem anderen Baum darf diese exe nicht beenden.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37kiste_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37kiste_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

function(kiste_lauf _name _raum _x _z _gier _arten)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(GLOB _alt "${WORKDIR}/f*.ppm")
    if(_alt)
        file(REMOVE ${_alt})
    endif()
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" "${_x}" "${_z}" "${_gier}"
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r37_kiste[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        "RE15_PAD_AT=40:A,90:A,100:D,120:U"
        "RE15_FRAMEDUMP=95-130/1:f"
        "RE15_EXIT_AT=131#${_raum}"
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r37_kiste[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_kiste[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r37_kiste[${_name}]: Endbild nicht erreicht - Lauf vorher abgerissen "
                            "(exit=${_rv}, ${WORKDIR})")
    endif()
    set(_bilder "ruhe:f000099.ppm")
    foreach(_f RANGE 100 129)
        if(_f EQUAL 107)
            list(APPEND _bilder "RE:f000${_f}.ppm")
        elseif(_f EQUAL 127)
            list(APPEND _bilder "HE:f000${_f}.ppm")
        else()
            list(APPEND _bilder "f000${_f}.ppm")
        endif()
    endforeach()
    execute_process(COMMAND "${RE15_BILD_TOOL}" "${RE15_BOXPANEL}" "${_arten}" ${_bilder}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 120
                    RESULT_VARIABLE _rvb OUTPUT_VARIABLE _outb)
    file(WRITE "${WORKDIR}/kiste_bild.txt" "${_outb}")
    if(NOT _rvb EQUAL 0)
        message(FATAL_ERROR "r37_kiste[${_name}]: Bildpruefung exit=${_rvb} (${WORKDIR}/kiste_bild.txt)\n${_outb}")
    endif()
    string(REGEX MATCHALL "Rahmen ok" _ok "${_outb}")
    list(LENGTH _ok _nok)
    if(NOT _nok EQUAL 31)
        message(FATAL_ERROR "r37_kiste[${_name}]: ${_nok}/31 Bilder mit unversehrtem Rahmen\n${_outb}")
    endif()
    message(STATUS "r37_kiste[${_name}] ROOM${_raum}: 31 Bilder Rahmen unversehrt, Symbole buendig rechts, "
                   "einlaufende Zeile oben/unten sichtbar")
endfunction()

kiste_lauf(leon   1150 -25650 -12100 0 SBSSS)
kiste_lauf(elza   1151 -25650 -12100 0 SBSSS)
kiste_lauf(stage2 2010 3050   -6100  0 SBSSS)

file(REMOVE "${_exe_kopie}")
message(STATUS "r37_kiste: OK")
