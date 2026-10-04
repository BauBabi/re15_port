# =============================================================================
# FLEISCH-BROCKEN NACH DEM REDHAWK-TREFFER MIT DER ECHTEN EXE (Runde 35, Spur D "redhawk").
#
# Nutzer (AUFTRAG.md Z.17): "Wenn ich mit der Super Redhawk auf die Hunde schiesse bleiben die
# Fleisch Effekte die sich rausloesen permanent da in loop."
# Dossier analysis/befunde_runde35/D_redhawk.md; Mechanik-Riegel unit_r35_redhawk (Takt fuer Takt
# gegen Routine 36 @0x800187c4 / Routine 37 @0x8001885c / Anim-Terminator @0x8001a40c).
#
# Dieser Haken prueft, was nur die exe zeigt: der ganze Spielweg (Eingabe R1 + SQUARE, Feuer-FSM,
# Treffer, RE2-Hunde-KI, ESP-Takt der Plattform fx_plattform_pc.c) und die Zahl der lebenden ESP-
# Plaetze je Bild (RE15_STATE_LOG Feld "fx=" = re15_esp_fx_count()).
#
# Lauf (gemessen 2026-10-04, 15 s): ROOM11D0 per RE15_DEBUG_JUMP, freier Hunde-Satz (Flag 3:152,
# probe_r34_reaktion.c:426), RE2-KI, Super Redhawk (Item 7) ausgeruestet, Leon (-7000,-15900) Blick 0,
# Eingabe ab Spielbild 18: Zielen + Feuern im Wechsel. Gemessen: Hund 1 stirbt durch Waffe 7
# (st=3 ss1=7) in Bild 112, fx steigt auf 59 (Bild 119) und
#   VORHER (Stand 154a73c1, Haken in re15_esp.c ausgeschaltet): fx bleibt ab Bild 133 bis Laufende
#          auf 6 stehen = die 6 Raum-Id-7-Brocken (Dossier M2/N5);
#   NACHHER: fx = 0 ab Bild 168.
# Pruefung: (1) Vorbedingung Redhawk-Tod eines Hundes, (2) danach entstehen Effekte, (3) innerhalb von
# 90 Bildern nach dem Tod gibt es ein Bild mit fx = 0 (vorher NIE: die 6 Brocken stehen), (4) im
# Endbild fx = 0.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r35_redhawk.cmake
# =============================================================================
set(_tag "r35_redhawk")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis), Muster test_r34n_b_cursor.cmake:44-58.
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r35d_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r35d_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

file(MAKE_DIRECTORY "${WORKDIR}")
file(REMOVE "${WORKDIR}/state.log" "${WORKDIR}/debug.log")
re15_start_spiel(_rv 180
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
    RE15_DEBUG_JUMP=11D0@250 RE15_SET_FLAG=3:152 RE15_AI_FLAVOR=re2
    RE15_GIVE=7:30 RE15_EQUIP=7 RE15_PLAYER_POS=-7000,-15900,0
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=18
    "RE15_INPUT_SCRIPT=M0.3,MA0.1,M0.6,MA0.1,M0.6,MA0.1,M0.6,MA0.1,M0.6,MA0.1,M0.6,MA0.1,M1,W40"
    RE15_STATE_LOG=state.log
    "RE15_EXIT_AT=230#11D0"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")
if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
    message(FATAL_ERROR "${_tag}: debug.log/state.log fehlt (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/debug.log" _dbg)
string(FIND "${_dbg}" "EXIT_AT: Bild 230 in Raum 11d0" _p)
if(_p LESS 0)
    message(FATAL_ERROR "${_tag}: Endbild 230 in ROOM11D0 nicht erreicht (exit=${_rv}, ${WORKDIR})")
endif()

# Zeilen der LETZTEN Sitzung (der Sprung setzt den Bildzaehler auf 0 zurueck).
file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX "^F[0-9]+ ")
set(_sitzung "")
foreach(_z IN LISTS _zeilen)
    if(_z MATCHES "^F0 ")
        set(_sitzung "")
    endif()
    list(APPEND _sitzung "${_z}")
endforeach()

set(_tod -1)
set(_fx_max 0)
set(_null_nach_tod -1)
set(_fx_ende -1)
set(_f_ende -1)
foreach(_z IN LISTS _sitzung)
    if(NOT _z MATCHES "^F([0-9]+) .* fx=([0-9]+) ")
        continue()
    endif()
    set(_f "${CMAKE_MATCH_1}")
    set(_fx "${CMAKE_MATCH_2}")
    set(_f_ende "${_f}")
    set(_fx_ende "${_fx}")
    if(_tod LESS 0 AND _z MATCHES "\\[[0-9]+ t=20 st=3 ss1=7 ")
        set(_tod "${_f}")
    endif()
    if(_tod GREATER_EQUAL 0)
        if(_fx GREATER _fx_max)
            set(_fx_max "${_fx}")
        endif()
        math(EXPR _frist "${_tod} + 90")
        if(_null_nach_tod LESS 0 AND _fx EQUAL 0 AND _f GREATER _tod AND _f LESS_EQUAL _frist)
            set(_null_nach_tod "${_f}")
        endif()
    endif()
endforeach()

if(_tod LESS 0)
    message(FATAL_ERROR "${_tag}: AUFSTELLUNG - kein Hund starb durch Waffe 7 (st=3 ss1=7) im Lauf "
                        "(KI-Wege geaendert?), ${WORKDIR}")
endif()
if(_fx_max LESS 6)
    message(FATAL_ERROR "${_tag}: nach dem Redhawk-Tod (Bild ${_tod}) zu wenige Effekte: fx max ${_fx_max} "
                        "(Router 0x80104610 wirft 6 Brocken + Blut), ${WORKDIR}")
endif()
if(_null_nach_tod LESS 0)
    message(FATAL_ERROR "${_tag}: NUTZER-BEFUND - bis 90 Bilder nach dem Redhawk-Tod (Bild ${_tod}) kein Bild "
                        "mit fx = 0: die Fleisch-Brocken laufen weiter (vorher stand fx dauerhaft auf 6), "
                        "${WORKDIR}")
endif()
if(NOT _fx_ende EQUAL 0)
    message(FATAL_ERROR "${_tag}: Endbild ${_f_ende}: fx = ${_fx_ende} (erwartet 0), ${WORKDIR}")
endif()
message(STATUS "${_tag}: Redhawk-Tod Bild ${_tod}, fx max ${_fx_max}, fx = 0 ab Bild ${_null_nach_tod} "
               "(<= Tod + 90), Endbild ${_f_ende} fx ${_fx_ende} - ok")
