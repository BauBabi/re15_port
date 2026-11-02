# =============================================================================
# test_r41_gatorfix.cmake - Runde 41 Spur G2 "gatorfix", exe-Riegel (Dossier analysis/befunde_runde41/G2_gatorfix.md).
#
# TEIL=gang_montage|gang_montage_elza  Punkt 3: Leiter 2080 -> 2090-Ankunftsszene -> Montage 2080 (Programm 2) -> zurueck. state.log: die fuenf
#     Zombies der Montage laufen in den Montage-Bildern (Spieler an der Leiter-Ankunft PL(-9950,-25700)) je >= 1000 naeher an die Leiter
#     (Auswerter test_r41_gatorfix ganglog montage). Vorher (Bau 789f6e681, Lauf v_ank_leon): 8 Zombies, 4 davon >= 1000, kleinster Weg -452.
# TEIL=gang_szene|gang_szene_elza  Punkt 4: Kampf 2090 mit dem Ausloese-Messhaken RE15_K2080_GATOR_HP=24990 (ein echter Treffer) -> Marvin/John-
#     Szene in 2080/2081. state.log: drei Zombies, jeder naeher an der Leiter, wenn er stirbt, und noch >= 2000 entfernt (ganglog szene); debug.log:
#     letzter Schuss <= 90 Bilder nach dem Szenenbeginn. Vorher: der Spieler stand am Treppenfuss, 8 stehende Zombies, letzter Schuss F351.
# TEIL=frei|frei_elza  Nachbesserung 1 (M1, E1): Kampf 2090 wie die Abnahme 1 (frei_leon/frei_elza: Ausloeseschuss, dann Zufallsweg im Tank-Steuer,
#     walk.py Saat 1/2, dieselbe Eingabe), 8400 Bilder Rueckkehr-Besuch, kein Messhaken fuer die Lage. Auswerter test_r41_gatorfix traglog: mit dem Kanister
#     im Maul (NB2, Mass M5 der Abnahme 2) kein Zeitfenster von 2 s, in dem die Wurzel nicht 300 vom Fensteranfang wegkommt (einzige Ausnahme: Fenster mit
#     einem Biss am Spieler), Wandreiben <= 30 Bilder, Aufnahme <= 900 Bilder nach dem Aufschlag (E1 "spaetestens nach rund 30 s").
# TEIL=abn2_*  NB2: die Laeufe der Abnahme 2 mit ihrer Eingabe (Leiter-Ecke, Wand-Tour Elza mit NW-Ecke/Ost-Nische, Nordbecken, Rampe West Leon/Elza,
#     Ecken-Tour, Wand-Tour Leon), gleiches Mass. Gegenprobe = die Abnahme-Laeufe (Bau e7d0510c7): 969 / ... Fenster (traglog im Dossier). Gegenprobe = Abnahme-Laeufe (Bau 867c95480):
#     Wandreiben 345 / 192 Bilder, Anteil 26 % / 25 %, Aufnahme 3619 Bilder (Leon).
# TEIL=rampe|rampe_elza  Nachbesserung 1 (M3): wie frei, Rueckkehr mit dem Messhaken RE15_K2080_RUECK_LAGE an die Suedkante der Rampe (Blick Nord), die
#     Figur klettert auf die Rampe (Kletterzellen Bodentypwort Bit 0, nur die Rampe) und bleibt oben; climb.log sub=9, Belagerung (Phase 5) mit dem Kanister,
#     Stillstand fern <= 60 Bilder am Stueck, Wandreiben <= 30.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_TOOL=<test_r41_gatorfix> -DWORKDIR=<dir> -DTEIL=<teil> -P test_r41_gatorfix.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_gatorfix: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
    message(FATAL_ERROR "r41_gatorfix: RE15_TOOL fehlt/existiert nicht: '${RE15_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_gatorfix: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_dir "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_dir}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r41g2_${_lauf_id}.exe")
if(NOT WIN32)
    set(_exe_kopie "${_exe_dir}/re15_pc_r41g2_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(WORKDIR "${_dir}")

function(g2_fehler _text)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_gatorfix: Lauf ${TEIL} - ${_text} (${_dir})")
endfunction()

foreach(_v RE15_GB_TEST RE15_GATOR_KANISTER RE15_KEIN_GATOR RE15_K2080_WURF_GIER RE15_K2080_GATOR_HP RE15_SET_FLAG RE15_FIRE_AOT RE15_G2_WURF_CUT
           RE15_K2080_RUECK_KANISTER RE15_K2080_RUECK_LAGE)
    unset(ENV{${_v}})
endforeach()

if(TEIL MATCHES "elza$")
    set(_r2080 2081)
    set(_r2090 2091)
    set(_wahl RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
else()
    set(_r2080 2080)
    set(_r2090 2090)
    set(_wahl RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
endif()

if(TEIL MATCHES "^gang_montage")
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 ${_wahl}
        RE15_SET_FLAG=3:245 RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=${_r2080}@250 "RE15_FIRE_AOT=1@40#${_r2080}"
        "RE15_EXIT_AT=150#${_r2080}" "${_exe_kopie}")   # Montage-Besuch F1..F156 (der erste 2080-Besuch endet mit der Leiter bei ~F45)
elseif(TEIL MATCHES "^gang_szene")
    set(_schuss "W6.0")
    foreach(_i RANGE 1 20)
        string(APPEND _schuss ",M0.6,MA0.1,M0.3,MA0.1,W3.0")
    endforeach()
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 ${_wahl}
        RE15_DEBUG_JUMP=${_r2090}@250 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:60" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_schuss}" RE15_STATE_LOG=state.log "RE15_PLAYER_POS=-6000,-17500,3072"
        "RE15_EXIT_AT=200#${_r2080}" "${_exe_kopie}")
elseif(TEIL MATCHES "^(frei|rampe|abn2_)")
    if(TEIL MATCHES "elza$")
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W1.0,W1.433,XU1.200,XUR0.867,XU1.100,R0.633,XUR1.300,XU1.667,XU1.067,R0.833,XU3.200,XU1.433,XU1.433,XU2.267,XU2.300,R0.767,R0.767,XU3.467,W0.667,XUR1.467,XU1.700,R0.667,R0.433,R0.533,XU1.800,L0.800,R0.433,W1.200,R0.433,XU2.267,W1.133,XU3.067,L0.900,XU2.433,XU2.467,L0.733,XU1.667,XUL1.133,L0.233,XU1.633,XU1.300,XU1.100,XUR0.900,XUL1.600,L0.467,XU3.467,XU1.100,XU2.533,L0.400,R0.500,XUL1.867,XUL1.200,XU2.667,XU1.267,XU3.200,XU2.467,XU2.300,XU3.400,XU2.500,XU1.033,L0.667,XU1.333,XU2.133,XU3.300,L0.900,XU2.100,XU2.467,R0.367,XU2.300,XU1.100,XU1.267,L0.267,XU2.867,XU1.067,R0.433,L0.667,XUL1.600,XU2.267,L0.833,W1.067,W1.167,L0.567,W1.100,XU3.467,XU1.033,L0.267,XU2.933,XUR0.600,R0.333,L0.200,XU1.767,L0.733,XU2.433,XU2.167,XUR1.833,XU1.300,R0.633,W1.200,XU2.633,XUR0.933,W0.633,XU2.200,XUR1.900,XUR1.100,XU2.500,L0.600,XUL0.600,XU2.367,XUL1.700,XUL0.700,XU1.567,R0.500,L0.300,W1.467,XU1.833,XU2.300,XU1.400,W0.733,XUR1.667,L0.533,R0.833,XU2.267,R0.367,R0.700,R0.700,XUL1.633,XUL1.300,XU3.033,XU3.200,XUR1.200,R0.667,XUL1.100,XU2.433,XU1.533,W0.900,L0.400,W1.067,L0.200,R0.367,R0.700,W1.267,XU3.267,L0.733,R0.533,XUR0.533,XU2.900,XU2.867,XUR0.867,L0.233,R0.667,XU1.700,L0.467,R0.600,XU2.700,L0.533,R0.267,XUR1.000,W0.800,L0.567,XU2.500,XUL1.700,R0.233,W1.067,L0.867,XU3.033,XU1.900,XU1.200,L0.533,XUR1.833,W0.933,R0.433,XU1.600,XU2.700,XU1.900,XUL0.900,R0.467,XUR0.933,R0.733,XUR1.300,L0.467,XU1.433,W0.867,L0.433,XU3.433,XU1.433,XU3.200,R0.200,XU3.067,R0.533,XUR0.967,W0.600,XU1.367")
    else()
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W1.0,XU3.133,XUR0.667,XUR1.467,XUL1.233,XUR1.133,XUL1.533,XU3.000,L0.833,XU1.067,R0.667,W1.233,R0.867,R0.367,XU2.133,XU1.067,XUL0.767,W1.367,XU1.833,R0.567,XUL0.967,R0.800,L0.533,R0.500,XU2.367,R0.267,L0.733,R0.467,R0.233,R0.667,XU1.567,XU1.500,W1.033,XU1.867,XUR1.200,XU2.367,W0.500,XUL1.267,R0.867,R0.600,XU2.267,R0.500,XU2.367,XUR1.200,XU1.567,XU2.467,XUR0.533,W0.567,XUR0.533,XUL0.900,XUL1.000,XU1.400,L0.667,R0.533,XU2.200,XU1.967,XU1.467,XU3.233,L0.867,XU3.433,XU2.000,XU3.400,XU2.267,XU3.067,W1.133,R0.367,R0.667,XUR0.600,XU3.433,XUR0.600,XU3.300,XU1.400,L0.200,XUL1.400,XU3.267,L0.467,XU1.500,R0.600,R0.400,R0.633,XU1.700,XU3.133,XUL1.000,XU2.700,XUR1.867,XUR1.667,L0.700,XU1.433,XUL0.900,XU2.267,XU1.833,XU1.600,W1.267,R0.300,L0.433,XU1.967,W0.633,XU1.300,R0.267,L0.867,XU1.733,L0.533,XU2.967,XUL1.433,XU2.033,XUL0.767,XUR0.733,L0.300,XUR1.867,XU3.433,XUR1.333,R0.267,L0.233,XUR1.600,XU1.967,XU1.167,XU2.500,R0.367,R0.667,XU2.367,XU1.600,XU1.700,W0.933,R0.867,XU1.767,XU1.800,XUR0.867,XU2.533,XUL0.867,XU1.600,XU2.367,XU1.200,R0.733,XU2.167,XUL1.267,XUL1.267,L0.733,W1.333,XU1.267,R0.300,XU2.367,R0.767,L0.333,XU1.400,L0.367,XU3.400,XUR1.300,XUR0.500,XUL0.900,XU2.600,XU2.433,XU1.900,XU1.300,XU3.067,XU2.000,L0.533,XU1.800,XUR1.900,L0.367,R0.867,R0.600,XUR1.967,XU3.300,XUR0.733,XUL0.967,R0.300,R0.600,XUR0.767,XU1.533,L0.533,XU2.600,XU3.100,L0.567,XU1.633,XU2.833,XU3.433,W1.133,W1.200,XU3.133,XU3.133,W1.400,XU3.267,XU2.567,L0.300,R0.833,R0.367,L0.867,L0.233,W0.700")
    endif()
    set(_extra "")
    set(_ende 8400)
    if(TEIL MATCHES "^rampe")
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W22.0,A0.3,W2.0,A0.3,W2.0,XU0.8,W120")
        set(_extra "RE15_K2080_RUECK_LAGE=3000,-18800,3072" RE15_CLIMB_TRACE=climb.log)
        set(_ende 3600)
    elseif(TEIL STREQUAL "abn2_leiterecke")   # Abnahme 2 Lauf rampe_nat_leon
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W3.5,L0.515,XU1.17,W0.3,R0.356,XU1.45,W0.3,R0.356,XU1.0,W200")
        set(_ende 8000)
        set(_extra)
    elseif(TEIL STREQUAL "abn2_wand_elza")   # Abnahme 2 Lauf w_elza
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W1.0,D0.500,W1.200,U0.400,W1.300,D0.500,W1.033,U0.400,W2.233,XU8.867,W7.367,D0.500,W1.367,U0.400,W0.933,L0.200,W1.700,D0.500,W1.200,U0.400,W0.933,XU7.667,W9.500,U0.400,W2.233,D0.500,W2.067,R0.200,W2.200,U0.400,W1.267,L0.533,XU3.467,W3.067,L0.367,XU8.267,W17.467,R0.367,XU7.733,W12.033,XU8.400,W6.100,UL2.000,W1.067,L0.367,XU6.367,W17.867,UR1.100,W1.933,R0.700,XU1.800,W3.600,XU7.467,W8.833,U0.400,W0.900,R0.200,W1.033,U0.400,W2.000,D0.500,W0.933,R0.200,W1.000,L0.367,XU6.300,W16.567,XU6.633,W12.700,XU6.433,W6.400,UR2.933,W2.167,R0.200,W1.667,U0.400,W1.600,R0.200,W1.300,L0.200,W1.300,D0.500,W0.833,R0.367,XU6.067,W1.600")
        set(_ende 8400)
        set(_extra)
    elseif(TEIL STREQUAL "abn2_nordbecken")   # Abnahme 2 Lauf plat_leon
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W3.0,A0.3,W1.2,A0.3,W1.2,A0.3,W1.2,A0.3,W1.2,A0.3,W2.0,R0.356,XU2.0,W15,U1.5,W25,L0.356,U2.0,W25,R0.711,U3.5,W30,L0.356,U1.0,W40,R0.711,U2.5,W30,L0.711,U1.0,W40")
        set(_ende 7300)
        set(_extra "RE15_K2080_RUECK_LAGE=5200,-13800,1024")
    elseif(TEIL STREQUAL "abn2_rampe_west")   # Abnahme 2 Lauf plat7_leon
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W14.0,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,W200")
        set(_ende 8000)
        set(_extra "RE15_K2080_RUECK_LAGE=4200,-18700,3072")
    elseif(TEIL STREQUAL "abn2_rampe_west_elza")   # Abnahme 2 Lauf plat8_elza
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W1.0,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,U0.1,A0.2,W0.3,D0.1,W0.3,W200")
        set(_ende 8100)
        set(_extra "RE15_K2080_RUECK_LAGE=2500,-18700,3072")
    elseif(TEIL STREQUAL "abn2_ecken")   # Abnahme 2 Lauf ecken_leon
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W3.0,L0.515,XU4.0,W2.0,R0.356,XU5.0,W40,R0.356,XU2.5,W30,L0.711,XU1.0,W5,L0.356,XU2.0,W25,L0.356,XU4.0,W8,L0.356,XU3.0,W35,R0.711,XU5.0,W30,L0.356,XU3.0,W30")
        set(_ende 7500)
        set(_extra)
    elseif(TEIL STREQUAL "abn2_wand")   # Abnahme 2 Lauf w_leon
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W1.0,R0.367,XU8.967,W17.033,L0.367,XU7.667,W11.833,D0.500,W1.933,U0.400,W1.533,XU6.467,W11.800,XU7.967,W10.967,XU8.367,W6.267,UL1.133,W1.733,R0.367,XU6.367,W17.133,R0.700,XU1.600,W2.967,L0.533,XU2.667,W3.300,L0.200,W1.467,U0.400,W1.933,L0.367,XU8.567,W10.300,XU6.233,W13.567,L0.367,XU7.633,W16.800,R0.533,XU1.867,W4.433,D0.500,W2.333,R0.200,W1.900,U0.400,W2.433,D0.500,W1.167,L0.200,W2.033,R0.200,W2.133,U0.400,W2.433,XU6.633,W11.500,XU8.700,W4.300")
        set(_ende 8400)
        set(_extra)
    endif()
    re15_start_spiel(_rv 900
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 ${_wahl}
        RE15_DEBUG_JUMP=${_r2090}@250 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:1" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_weg}" RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1
        "RE15_PLAYER_POS=-6000,-17500,3072" ${_extra} "RE15_EXIT_AT=${_ende}#${_r2090}" "${_exe_kopie}")
    file(REMOVE "${_exe_kopie}")
    if(NOT _rv EQUAL 0)
        g2_fehler("exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT)")
    endif()
    execute_process(COMMAND "${RE15_TOOL}" traglog "${_dir}" RESULT_VARIABLE _tr OUTPUT_VARIABLE _to ERROR_VARIABLE _te)
    string(REGEX MATCH "traglog: [^\n]*" _tz "${_to}")
    if(NOT _tr EQUAL 0 OR NOT _tz MATCHES "angehaengt F[0-9]+ \\(([0-9]+) Bilder")
        g2_fehler("kein Tragen: ${_tz}")
    endif()
    set(_auf ${CMAKE_MATCH_1})
    string(REGEX MATCH "2-s-Fenster ohne Fortschritt ([0-9]+)" _m "${_tz}")
    set(_fenster ${CMAKE_MATCH_1})
    string(REGEX MATCH "laengstes Wandreiben ([0-9]+) Bilder" _m "${_tz}")
    set(_rub ${CMAKE_MATCH_1})
    if("${_rub}" STREQUAL "" OR _rub GREATER 30)
        g2_fehler("Wandreiben ${_rub} Bilder > 30 (1 s): ${_tz}")
    endif()
    if("${_fenster}" STREQUAL "" OR NOT _fenster EQUAL 0)
        g2_fehler("${_fenster} Zeitfenster von 2 s ohne Fortschritt (NB2 M5, ausser Biss am Spieler): ${_tz}")
    endif()
    if(TEIL MATCHES "^frei")
        if(_auf GREATER 900)
            g2_fehler("Aufnahme ${_auf} Bilder nach dem Aufschlag > 900 (E1, rund 30 s): ${_tz}")
        endif()
    elseif(TEIL MATCHES "^rampe")
        file(STRINGS "${_dir}/climb.log" _kl REGEX " room=${_r2090} .* sub=9 ")
        if(NOT _kl)
            g2_fehler("kein Klettern auf die Rampe (climb.log sub=9)")
        endif()
        file(STRINGS "${_dir}/gator_trag.log" _g5 REGEX "^TRAG F[0-9]+ ph=5 ")
        list(LENGTH _g5 _g5n)
        if(_g5n LESS 600)
            g2_fehler("nur ${_g5n} Trag-Bilder Belagerung (Phase 5) - Figur nicht oben?")
        endif()
        set(_tz "${_tz}; Belagerung mit Kanister ${_g5n} Bilder")
    endif()
    message(STATUS "r41_gatorfix ${TEIL}: ${_tz}")
    return()
else()
    g2_fehler("unbekannter TEIL")
endif()
file(REMOVE "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    g2_fehler("exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT)")
endif()
if(TEIL MATCHES "^gang_montage")
    set(_modus montage)
else()
    set(_modus szene)
endif()
execute_process(COMMAND "${RE15_TOOL}" ganglog "${_dir}/state.log" ${_modus} RESULT_VARIABLE _gr OUTPUT_VARIABLE _go ERROR_VARIABLE _ge)
string(REGEX MATCH "ganglog [a-z]+: [0-9]+ Bilder an der Leiter[^\n]*" _gz "${_go}")
if(NOT _gr EQUAL 0)
    g2_fehler("Gang zur Leiter: ${_gz}")
endif()
if(_modus STREQUAL "szene")
    file(STRINGS "${_dir}/debug.log" _sl REGEX "\\[k2080\\] F[0-9]+ Schuss [0-9]+: Treffer")
    list(LENGTH _sl _n)
    if(NOT _n EQUAL 3)
        g2_fehler("${_n} Treffer statt 3")
    endif()
    list(GET _sl 2 _l3)
    string(REGEX MATCH "F([0-9]+) Schuss" _m "${_l3}")
    if(CMAKE_MATCH_1 GREATER 90)
        g2_fehler("letzter Schuss F${CMAKE_MATCH_1} > 90 Bilder nach dem Szenenbeginn")
    endif()
    message(STATUS "r41_gatorfix ${TEIL}: ${_gz}; letzter Schuss F${CMAKE_MATCH_1}")
else()
    message(STATUS "r41_gatorfix ${TEIL}: ${_gz}")
endif()
