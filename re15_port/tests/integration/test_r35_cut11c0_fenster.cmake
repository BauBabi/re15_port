# =============================================================================
# FENSTER ROOM1120 MIT DER ECHTEN EXE (Runde 35 Spur M).
# Dossier analysis/befunde_runde35/M_cut11c0_fenster.md, Konstanten include/re15_fenster1120.h.
#
# Zwei Laeufe, Spielstand aus probe_r35_fenster karte (ROOM1120, (9,73)=1, Spieler (5000,0,2600) rot
# 0x400 im Cut-1-Gang suedlich des Bands), CONTINUE, Eingabe auf der Spielbild-Zeitachse:
#   A  NUTZERWEG: Leon laeuft nach Norden auf das Fenster zu (U) -> Band -> Ereignis 24: Log zeigt
#      scharf / Kraehe versteckt / Ausloeser / 13 Splitter auf gueltigen Plaetzen / Knall 1+2 / fertig;
#      Framedump VOR dem Ausloesen (Bild 60) und NACH der Zeitlinie (Bild 330): die Lochpixel der
#      Scheiben sind danach dunkler (Schaden), die Kraehe hat das Bild betreten.
#   B  WIEDEREINTRITT: Stand mit (9,79)=1 -> kein Ereignis, Fenster sofort beschaedigt (Bild 60).
#
# RE15_SOFTWARE_RENDER=1 nur fuer die Robustheit des Hakens (Framedump liest den komponierten Frame).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r35_fenster> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r35_fenster: RE15_PC_EXE fehlt: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r35_fenster: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r35_fenster: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
# Eigener exe-Name (Muster test_r34n_b_cursor.cmake): fremde `taskkill /IM re15_pc.exe` treffen uns nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte "${_exe_dir}/re15_pc_r35m_haken_*.exe")
foreach(_k IN LISTS _alte)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r35m_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

# PPM (P6, 320x240) lesen -> Mittel der Leuchtdichte ueber die Lochpixel der Scheiben
# (gen/fenster1120_schaden.inc art 0; Stichprobe: Mitten der beiden Loecher).
function(ppm_luma _datei _out)
    file(READ "${_datei}" _hex HEX)
    # Kopf "P6\n320 240\n255\n" = 15 Byte
    set(_summe 0)
    foreach(_p "187;89" "188;92" "186;95" "200;88" "201;92" "199;95")
        list(GET _p 0 _x)
        list(GET _p 1 _y)
        math(EXPR _o "(15 + (${_y} * 320 + ${_x}) * 3) * 2")
        string(SUBSTRING "${_hex}" ${_o} 6 _rgb)
        string(SUBSTRING "${_rgb}" 0 2 _r)
        string(SUBSTRING "${_rgb}" 2 2 _g)
        string(SUBSTRING "${_rgb}" 4 2 _b)
        math(EXPR _summe "${_summe} + 0x${_r} + 0x${_g} + 0x${_b}")
    endforeach()
    set(${_out} ${_summe} PARENT_SCOPE)
endfunction()

function(fenster_lauf _name _karte_arg _skript _ende _out_log)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" karte "re15_card.mcr" ${_karte_arg}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _ok)
    if(NOT _rvk EQUAL 0)
        message(FATAL_ERROR "r35_fenster[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_ok}")
    endif()
    re15_start_spiel(_rv 200
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=70
        "RE15_INPUT_SCRIPT=${_skript}"
        RE15_FENSTER_LOG=fenster.log
        "RE15_FRAMEDUMP=60-330/270:bild_"
        "RE15_EXIT_AT=${_ende}#1120"
        "${_exe}")
    set(_log "")
    if(EXISTS "${WORKDIR}/fenster.log")
        file(READ "${WORKDIR}/fenster.log" _log)
    endif()
    message(STATUS "r35_fenster[${_name}] exit=${_rv}\n${_log}")
    set(${_out_log} "${_log}" PARENT_SCOPE)
    set(R35_WD "${WORKDIR}" PARENT_SCOPE)
endfunction()

# ---- A: Nutzerweg -------------------------------------------------------------------------------
fenster_lauf(A "" "U3" 330 _log_a)
set(_wd_a "${R35_WD}")
foreach(_muss "ROOM1120 scharf" "Kraehe Slot 4 versteckt" "Ausloeser: Spieler" "Knall 1" "Knall 2"
              "Zeitlinie fertig T+23 Splitter 13 Knalle 2")
    string(FIND "${_log_a}" "${_muss}" _f)
    if(_f LESS 0)
        message(FATAL_ERROR "r35_fenster[A]: '${_muss}' fehlt im Log")
    endif()
endforeach()
string(REGEX MATCHALL "Splitter Bank 0x1[0-3] Platz [0-9]+" _sp "${_log_a}")
list(LENGTH _sp _n_sp)
if(NOT _n_sp EQUAL 13)
    message(FATAL_ERROR "r35_fenster[A]: ${_n_sp} statt 13 Splitter auf gueltigen Plaetzen")
endif()
foreach(_b 000060 000330)
    if(NOT EXISTS "${_wd_a}/bild_${_b}.ppm")
        message(FATAL_ERROR "r35_fenster[A]: Framedump bild_${_b}.ppm fehlt")
    endif()
endforeach()
ppm_luma("${_wd_a}/bild_000060.ppm" _vor)
ppm_luma("${_wd_a}/bild_000330.ppm" _nach)
message(STATUS "r35_fenster[A]: Lochpixel-Leuchtsumme vor ${_vor}, nach ${_nach}")
if(NOT _nach LESS _vor)
    message(FATAL_ERROR "r35_fenster[A]: Scheiben nach dem Ereignis nicht dunkler (vor ${_vor} nach ${_nach})")
endif()

# ---- B: Wiedereintritt mit (9,79)=1 --------------------------------------------------------------
fenster_lauf(B "gebrochen" "W1" 90 _log_b)
set(_wd_b "${R35_WD}")
string(FIND "${_log_b}" "ROOM1120 aus: (9,73)=1 (9,79)=1" _f)
if(_f LESS 0)
    message(FATAL_ERROR "r35_fenster[B]: Wiedereintritt nicht als 'aus' erkannt")
endif()
string(FIND "${_log_b}" "Ausloeser" _f2)
if(NOT _f2 LESS 0)
    message(FATAL_ERROR "r35_fenster[B]: zweites Ereignis beim Wiedereintritt")
endif()
ppm_luma("${_wd_b}/bild_000060.ppm" _b60)
message(STATUS "r35_fenster[B]: Lochpixel-Leuchtsumme ${_b60} (A vor ${_vor})")
if(NOT _b60 LESS _vor)
    message(FATAL_ERROR "r35_fenster[B]: Fenster beim Wiedereintritt nicht beschaedigt")
endif()
file(REMOVE "${_exe}")
message(STATUS "r35_fenster: OK")
