# =============================================================================
# FENSTER ROOM1120 MIT DER ECHTEN EXE (Runde 35 Spur M).
# Dossier analysis/befunde_runde35/M_cut11c0_fenster.md, Konstanten include/re15_fenster1120.h.
#
# Zwei Laeufe, Spielstand aus probe_r35_fenster karte (ROOM1120 Cut 1, (9,73)=1, Spieler (5000,0,2600)
# rot 0xC00 = Blick nach Norden im Cut-1-Gang suedlich des Bands), CONTINUE, Eingabe auf der
# Spielbild-Zeitachse ab Bild 70:
#   A  NUTZERWEG: Leon laeuft 1,3 s nach Norden auf das Fenster zu (U1.3) -> Band -> Ereignis 24: Log zeigt
#      scharf / Kraehe versteckt / Ausloeser / 13 Splitter auf gueltigen Plaetzen / Knall 1+2 / fertig.
#      Framedumps 60..330 alle 30 Bilder: Bild 60 = VOR dem Ausloesen; in mindestens einem spaeteren Bild
#      (>= 150, Splitter gelandet) sind die LOCHpixel dunkler UND die BRUCHKANTEN heller als vorher
#      (beides zusammen kann weder die dunkle Kraehe noch ein additiver Splitter allein erzeugen).
#   B  WIEDEREINTRITT: Stand mit (9,79)=1 -> kein Ereignis, Fenster ab Bild 60 beschaedigt (keine
#      Kraehe, keine Splitter im Bild — der reine Hintergrund-Schaden).
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

# LOCH = Lochpixel (art 0, a 40), KANTE = Bruchkanten (art 1, a 136/158) aus gen/fenster1120_schaden.inc.
set(R35_LOCH  "187|89" "188|92" "186|95" "200|88" "201|92" "199|95")
set(R35_KANTE "185|83" "186|84" "183|86" "204|84" "200|85" "196|86" "201|86")
# PPM P6 320x240, Kopf "P6\n320 240\n255\n" = 15 Byte (gemessen 50 36 0a 33 32 30 20 32 34 30 0a 32 35 35 0a).
function(ppm_summe _datei _liste _out)
    file(READ "${_datei}" _hex HEX)
    set(_summe 0)
    foreach(_q ${${_liste}})
        string(REPLACE "|" ";" _p "${_q}")
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
        "RE15_FRAMEDUMP=60-330/30:bild_"
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
fenster_lauf(A "" "U1.3" 330 _log_a)
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
if(NOT EXISTS "${_wd_a}/bild_000060.ppm")
    message(FATAL_ERROR "r35_fenster[A]: Framedump bild_000060.ppm fehlt")
endif()
ppm_summe("${_wd_a}/bild_000060.ppm" R35_LOCH  _loch_vor)
ppm_summe("${_wd_a}/bild_000060.ppm" R35_KANTE _kante_vor)
set(_treffer "")
foreach(_b 150 180 210 240 270 300 330)
    set(_d "${_wd_a}/bild_000${_b}.ppm")
    if(NOT EXISTS "${_d}")
        message(FATAL_ERROR "r35_fenster[A]: Framedump bild_000${_b}.ppm fehlt")
    endif()
    ppm_summe("${_d}" R35_LOCH  _l)
    ppm_summe("${_d}" R35_KANTE _k)
    message(STATUS "r35_fenster[A]: Bild ${_b}: Loch ${_l} (vor ${_loch_vor}), Kante ${_k} (vor ${_kante_vor})")
    if(_l LESS _loch_vor AND _k GREATER _kante_vor)
        list(APPEND _treffer ${_b})
    endif()
endforeach()
if(NOT _treffer)
    message(FATAL_ERROR "r35_fenster[A]: in keinem Bild nach dem Ereignis Loch dunkler UND Kante heller")
endif()
message(STATUS "r35_fenster[A]: Schaden belegt in Bild(ern) ${_treffer}")

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
ppm_summe("${_wd_b}/bild_000060.ppm" R35_LOCH  _bl)
ppm_summe("${_wd_b}/bild_000060.ppm" R35_KANTE _bk)
message(STATUS "r35_fenster[B]: Loch ${_bl} (A vor ${_loch_vor}), Kante ${_bk} (A vor ${_kante_vor})")
if(NOT (_bl LESS _loch_vor AND _bk GREATER _kante_vor))
    message(FATAL_ERROR "r35_fenster[B]: Fenster beim Wiedereintritt nicht beschaedigt")
endif()
file(REMOVE "${_exe}")
message(STATUS "r35_fenster: OK")
