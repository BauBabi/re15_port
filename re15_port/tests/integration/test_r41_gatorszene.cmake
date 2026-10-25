# =============================================================================
# Runde 41 Spur F "gatorszene" — die 2090-Ankunftsszene in der ECHTEN exe (Leiter 2080 -> 2090, Nachtrag 21).
#
# Dossier analysis/befunde_runde41/F_gatorszene.md. Die Unit-Riegel unit_r41_gatorszene_* pruefen die Mechanik ueber die
# Engine-Wege; dieser Riegel prueft den Weg, den der Spieler geht: Debug-Sprung nach 2080 (Original-Rechner (3,245) gesetzt,
# Leiter frei), Leiter-Tuer Slot 1 @0x00BC2 -> 2090 (-8200,0,-25550), Szene bis zum Kampf. Verlangt in debug.log:
#   - "[gatorszene] ROOM2090 Slot 1 (Tor) gesperrt" (Nachtrag 21),
#   - die Folge-Zeilen in dieser Reihenfolge: A (Ankunft), P 1, T (Knall), P 2 + M 8 (2080, acht Zombies), P 3, S, G 1, G 2,
#     K 13, G 3, G 4, V, G 5, G 6, G 7, B 1 (Musik-Weiche 0xFF3C),
#   - keine Spinne 0x25 in 2090 ("Spinne 0x25 ... verworfen" zweimal je 2090-Besuch),
#   - Nachtrag 20 am state.log (RE15_STATE_LOG): die acht Zombies Typ 0x16 der 2080-Einstellung stehen oestlich der Band-8-
#     Absatzwand x -16650 (Spur G: SCA @0x05C4) in JEDEM Bild und LAUFEN (mindestens 4 von 8 mehr als 300 Einheiten zwischen
#     dem ersten und dem letzten Bild der Einstellung; gemessen Lauf lad4: 8 von 8).
# TEIL=leon | elza (Elza: Spieler-Auswahl Auto + Wechsel, Raeume 2081/2091).
#
# Nachbesserung 1 (Abnahme F 0):
#   M1 leon/elza: Folge-Zeile "C 0" (Kamera-Uebergabe auf den Eintritts-Cut 0 der Leiter, ROOM2080 Slot 1 @0x00BC2; Form ROOM1150
#      sub08 Cut_chg @0x012C4 vor Plc_ret @0x012E0) mit "Spielfigur im Bild", und state.log: in den Bildern F405-F479 des letzten
#      2090-Besuchs (nur dieser Besuch erreicht F400) steht JEDES Bild auf cam=0 (vorher F405-F699 cam=1, Leon hinter der Kamera).
#   M2 tod/tod_elza: Todesrolle (RE15_GB_TEST=3) zweimal - mit Schluessel und mit (9,112) vorab (kein Schluessel) -, Bilder
#      F480-F598 je 2 (RE15_FRAMEDUMP, WINDOW_SCALE=1); Auswerter `test_r41_gatorszene bilddiff` (RE15_TOOL): Schluessel in JEDEM
#      Bild sichtbar (Unterschied > 0), Puls max/min >= 1,40 ohne Plateau; debug.log "Schluessel-Lage: Kamera <c> (laufende)"
#      mit Blink-Spitze <= 0x80.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=leon|elza -P test_r41_gatorszene.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_gatorszene: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_gatorszene: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL leon)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_dir "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_dir}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r41f_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(WORKDIR "${_dir}")

if(TEIL MATCHES "elza$")
    set(_r2080 2081)
    set(_r2090 2091)
    set(_wahl RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
else()
    set(_r2080 2080)
    set(_r2090 2090)
    set(_wahl RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
endif()

# ---- Nachbesserung 1 (M2): Schluessel nach dem Tod sichtbar und pulsierend (Todesrolle, Leon / Elza) -------------------
if(TEIL MATCHES "^tod")
    if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - RE15_TOOL (test_r41_gatorszene) fehlt: '${RE15_TOOL}'")
    endif()
    # GB_TEST=3: Tod nach 300 Boss-Bildern + Todesrolle Clip 7 (165 Bilder) -> Leiche (Zustand 7) und Schluessel ab ~F470
    # (gemessen Nachbesserung F 1, Lauf T2n: "Folge L ROOM2090 F470"). Eine Blink-Periode (0x3c = 60 Bilder) ab F480.
    foreach(_v mit ohne)
        set(WORKDIR "${_dir}/${_v}")
        file(MAKE_DIRECTORY "${WORKDIR}")
        if(_v STREQUAL "ohne")
            set(_bits "RE15_SET_FLAG=9:110,9:112")
        else()
            set(_bits "RE15_SET_FLAG=9:110")
        endif()
        re15_start_spiel(_rv 300
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
            ${_wahl}
            ${_bits}
            RE15_DEBUG_JUMP=${_r2090}@250 RE15_GB_TEST=3 RE15_R39_MESS_SPIELER_HP=100
            RE15_STATE_LOG=state.log
            "RE15_EXIT_AT=600#${_r2090}"
            "RE15_FRAMEDUMP=480-598/2:fd_"
            "${_exe_kopie}")
        if(NOT _rv EQUAL 0)
            file(REMOVE "${_exe_kopie}")
            message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL}/${_v} exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${WORKDIR}")
        endif()
    endforeach()
    file(REMOVE "${_exe_kopie}")
    file(STRINGS "${_dir}/mit/debug.log" _lage REGEX "\\[gatorszene\\] Schluessel-Lage: Kamera [0-9]+ ")
    if(NOT _lage)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - keine Zeile 'Schluessel-Lage' (Alligator nicht tot?), ${_dir}/mit")
    endif()
    list(GET _lage 0 _lage0)
    if(NOT _lage0 MATCHES "Kamera ([0-9]+) \\(laufende\\).*Blink-Spitze ([0-9]+) ")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Schluessel nicht fuer die laufende Kamera gelegt: ${_lage0}")
    endif()
    set(_kam "${CMAKE_MATCH_1}")
    if(CMAKE_MATCH_2 GREATER 128)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Blink-Spitze ${CMAKE_MATCH_2} > 0x80 (Kappung, Plateau): ${_lage0}")
    endif()
    file(STRINGS "${_dir}/mit/state.log" _sl REGEX "^F5[0-9][0-9] ")
    foreach(_z IN LISTS _sl)
        if(NOT _z MATCHES " cam=${_kam} ")
            message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Kamera wechselte nach der Lage (soll cam=${_kam}): ${_z}")
        endif()
    endforeach()
    execute_process(COMMAND "${RE15_TOOL}" bilddiff "${_dir}/mit" "${_dir}/ohne" 480 598 2
                    RESULT_VARIABLE _br OUTPUT_VARIABLE _bo ERROR_VARIABLE _be)
    string(REGEX MATCH "bilddiff: [^\n]*" _bz "${_bo}")
    if(NOT _br EQUAL 0)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Schluessel nicht sichtbar/pulsierend: ${_bz}\n${_bo}")
    endif()
    message(STATUS "r41_gatorszene: Lauf ${TEIL} - ${_lage0}; Kamera ${_kam} F500-F599 unveraendert; ${_bz}")
    return()
endif()

# Bild 600 im DRITTEN 2090-Besuch: der Bildzaehler beginnt je Raum neu; der erste 2090-Besuch (Programm 1) endet nach ~140
# Bildern, der 2080-Besuch nach ~150, der Gator-Ablauf ist ~F416 vorbei (gemessen 2026-10-24, Laeufe s4/lad2).
re15_start_spiel(_rv 480
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
    ${_wahl}
    RE15_SET_FLAG=3:245
    RE15_STATE_LOG=state.log
    RE15_DEBUG_JUMP=2080@250
    "RE15_FIRE_AOT=1@40#${_r2080}"
    "RE15_EXIT_AT=600#${_r2090}"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
if(NOT EXISTS "${_dir}/debug.log")
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} ohne debug.log (${_dir})")
endif()
file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: Bild [0-9]+ in Raum ${_r2090}")
if(NOT _ex)
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} endete nicht im dritten ${_r2090}-Besuch (kein EXIT_AT), ${_dir}")
endif()
file(STRINGS "${_dir}/debug.log" _tor REGEX "\\[gatorszene\\] ROOM${_r2090} Slot 1 \\(Tor\\) gesperrt")
if(NOT _tor)
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Tor ${_r2090} Slot 1 nicht gesperrt (Nachtrag 21)")
endif()
file(STRINGS "${_dir}/debug.log" _sp REGEX "\\[gatorszene\\] ROOM${_r2090}: Spinne 0x25 bei")
list(LENGTH _sp _n_sp)
if(_n_sp LESS 2)
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Spinnen in ${_r2090} nicht verworfen (${_n_sp} Zeilen)")
endif()

# Folge-Zeilen in Reihenfolge
file(STRINGS "${_dir}/debug.log" _fz REGEX "\\[gatorszene\\] Folge ")
set(_soll "A;P 1;T;P 2;M 8;P 3;S;G 1;G 2;K 13;G 3;G 4;V;G 5;G 6;C 0;G 7;B 1")
set(_i 0)
list(LENGTH _fz _n_fz)
foreach(_s IN LISTS _soll)
    string(REPLACE " " ";" _teile "${_s}")
    list(GET _teile 0 _art)
    list(LENGTH _teile _nt)
    set(_gefunden 0)
    while(_i LESS _n_fz)
        list(GET _fz ${_i} _z)
        math(EXPR _i "${_i} + 1")
        if(_nt GREATER 1)
            list(GET _teile 1 _wert)
            if(_z MATCHES "Folge ${_art} ROOM[0-9A-F]+ F[0-9]+ wert=${_wert}/")
                set(_gefunden 1)
                break()
            endif()
        elseif(_z MATCHES "Folge ${_art} ")
            set(_gefunden 1)
            break()
        endif()
    endwhile()
    if(NOT _gefunden)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Folge-Zeile '${_s}' fehlt oder steht nicht in der Reihenfolge (${_dir}/debug.log)")
    endif()
endforeach()
# Nachbesserung 1 (M1): Kamera-Uebergabe am Szenenende auf den Eintritts-Cut 0, Spielfigur im Bild; im ganzen Kampfanfang
# F405-F479 des letzten 2090-Besuchs (nur er erreicht Bild 400) cam=0 (Abnahme 0: cam=1, Leon hinter der Kamera).
file(STRINGS "${_dir}/debug.log" _ueb REGEX "\\[gatorszene\\] Kamera-Uebergabe: Cut 0 \\(Eintritts-Cut\\), Spielfigur im Bild")
if(NOT _ueb)
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - keine Kamera-Uebergabe auf Cut 0 mit Spielfigur im Bild (M1)")
endif()
file(STRINGS "${_dir}/state.log" _ks REGEX "^F4(0[5-9]|[1-7][0-9]) ")
list(LENGTH _ks _n_ks)
if(_n_ks LESS 70)
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - nur ${_n_ks} state.log-Bilder F405-F479")
endif()
foreach(_z IN LISTS _ks)
    if(NOT _z MATCHES " cam=0 ")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Kampfanfang nicht auf Cut 0 (M1): ${_z}")
    endif()
endforeach()
# Nachtrag 20: Zombies Typ 0x16 (state.log " t=16 ") oben und laufend. Die Treffer beginnen OHNE die oeffnende "[" des
# Satzes: ein Listen-Element mit offener eckiger Klammer laesst CMake die folgenden ";" nicht trennen (sonst 1 statt 8).
if(NOT EXISTS "${_dir}/state.log")
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} ohne state.log (${_dir})")
endif()
file(STRINGS "${_dir}/state.log" _zl REGEX " t=16 ")
list(LENGTH _zl _n_zl)
if(_n_zl LESS 30)
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - nur ${_n_zl} state.log-Bilder mit Zombies Typ 0x16 (2080-Einstellung fehlt?)")
endif()
set(_west 0)
foreach(_z IN LISTS _zl)
    string(REGEX MATCHALL "[0-9]+ t=16 [^@]*@\\((-?[0-9]+)," _ms "${_z}")
    foreach(_m IN LISTS _ms)
        string(REGEX MATCH "@\\((-?[0-9]+)," _x "${_m}")
        set(_xv ${CMAKE_MATCH_1})
        if(_xv LESS_EQUAL -16650)
            math(EXPR _west "${_west} + 1")
        endif()
    endforeach()
endforeach()
if(_west GREATER 0)
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - ${_west} Zombie-Lagen westlich der Absatzwand x -16650 (Nachtrag 20: oben bleiben)")
endif()
list(GET _zl 0 _erst)
list(GET _zl -1 _letzt)
string(REGEX MATCHALL "[0-9]+ t=16 [^@]*@\\(-?[0-9]+,-?[0-9]+," _m0 "${_erst}")
string(REGEX MATCHALL "[0-9]+ t=16 [^@]*@\\(-?[0-9]+,-?[0-9]+," _m1 "${_letzt}")
set(_bewegt 0)
foreach(_a IN LISTS _m0)
    string(REGEX MATCH "^([0-9]+) t=16 .*@\\((-?[0-9]+),(-?[0-9]+)," _x "${_a}")
    set(_s ${CMAKE_MATCH_1})
    set(_ax ${CMAKE_MATCH_2})
    set(_az ${CMAKE_MATCH_3})
    foreach(_b IN LISTS _m1)
        string(REGEX MATCH "^([0-9]+) t=16 .*@\\((-?[0-9]+),(-?[0-9]+)," _y "${_b}")
        if(CMAKE_MATCH_1 STREQUAL _s)
            math(EXPR _dx "${CMAKE_MATCH_2} - ${_ax}")
            math(EXPR _dz "${CMAKE_MATCH_3} - ${_az}")
            if(_dx LESS 0)
                math(EXPR _dx "0 - ${_dx}")
            endif()
            if(_dz LESS 0)
                math(EXPR _dz "0 - ${_dz}")
            endif()
            math(EXPR _d "${_dx} + ${_dz}")
            if(_d GREATER 300)
                math(EXPR _bewegt "${_bewegt} + 1")
            endif()
        endif()
    endforeach()
endforeach()
if(_bewegt LESS 4)
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - nur ${_bewegt} von 8 Zombies gelaufen (Nachtrag 20: sie sollen oben laufen)")
endif()
message(STATUS "r41_gatorszene: Lauf ${TEIL} - Tor gesperrt, ${_n_sp} Spinnen-Saetze verworfen, Folge ${_soll} vollstaendig, "
               "2080: ${_n_zl} Bilder mit Zombies, keiner westlich x -16650, ${_bewegt} von 8 gelaufen")
