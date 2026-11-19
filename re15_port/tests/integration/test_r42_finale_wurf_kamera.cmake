cmake_minimum_required(VERSION 3.16)   # cmake -P: Policies wie die uebrigen Integrationsskripte (Linux-Container CMake 3.28)
# =============================================================================
# FINALE 5091: DIE WURFSZENE HAELT IHRE KAMERA, AUCH WENN DER SPIELER AM KAMERA-UEBERGANG STEHT.
# Runde 42 Strang finale, Stufe 4 Runde 2 (Pruefbericht s4 r1, Rot R1).
#
# NUTZER 18.11.2026: "John kann danach gerne wieder raus laufen. Sollte er auch, aber das geht schneller vom Eingang aus." und die
# Vorgabe der Runde "Die Kamera sieht John wie in RE2 frontal und hell". Im Elza-Durchlauf s4 r1 war John nie im Bild: die Szene
# schnitt per Cut_chg(13) auf Johns Kamera, im selben Bild schaltete die Nahsicht-Wache der Riesenform (birkin_formen.c
# bf_nahsicht_wache, PORT-WAHL ohne Gegenstueck im Original) auf ihre Gegenkamera 12 zurueck. Gemessen (Messschiene
# RE15_KAMERAHALT_LOG, Lauf s4r2/lauf/orig_mess): "Wache: Form-Mitte x 10864, Spieler x 16904, angezeigt 13 -> Gegenkamera 12" in
# den Bildern W und Reaktion. Im Original schneidet waehrend der Szene nur ihr Programm (RE2 ROOM7020 sub12 Cut_chg @0x0184E /
# @0x018FC / @0x01948; Zonen aus ueber Bit 0x100 @0x800402d4, Scan-Gate @0x8001cce0-e4).
#
# Ablauf (Wegplan wie ein Spieler, Elza): Debug-Sprung nach Wagen A von 5091 mit (3,89) und Bank-9-Bit 130, durch Wagen B nach C,
# an der Kampfstelle x 8150 stehen und mit der Pistole (RE15_GIVE 3:15, 21:50) auf die Form feuern. Ein Rueckstoss der Form wirft
# Elza nach x 16904 - genau auf den Uebergang der Kameras 12/13; danach "genug Schaden" und die Szene. VORBEDINGUNG (sonst rot, damit
# der Riegel nicht still an der falschen Stelle prueft): Spieler beim Szenenstart bei 16000 <= x <= 17500.
# Geprueft aus debug.log ([kamerahalt] je Bild, [scd] Cut_chg, [bf-kamera]) und finale.log:
#   - jedes Bild von W (Cut_chg(13)) bis vor den Flugschnitt auf die Kamera des Spielers zeigt Kamera 13 (cam=13);
#   - jedes Bild von der Reaktion (Cut_chg(13)) bis vor das Szenenende zeigt Kamera 13;
#   - keine Zeile "[bf-kamera] ... Wache" zwischen Szenenstart und Szenenende.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r42_finale_wurf_kamera.cmake
#         nur auswerten (vorhandener Lauf-Ordner mit finale.log/debug.log): cmake -DAUSWERTEN=<lauf-ordner> -P ...
# =============================================================================
if(AUSWERTEN)
    set(RE15_PC_EXE "${CMAKE_CURRENT_LIST_FILE}")
    set(WORKDIR "${AUSWERTEN}")
endif()
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_finale_wurf_kamera: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_finale_wurf_kamera: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

set(_rv "-")
if(NOT AUSWERTEN)
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r42wurfkam_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

file(WRITE "${WORKDIR}/wegplan.txt" "RAUM 5091
WARTE 30
TASTE 0x8 3 45
TASTE 0x10 3 15
TASTE 0x8000 3 45
TASTE 0x8000 3 40
TASTE 0x8000 3 90
TASTE 0x8 3 45
GEHE 20000 5000 250
GEHE 300 5000 200
BLICK 2048
AKTION
WARTE 60
GEHE 19500 -9250 200
GEHE 13800 -9250 200
GEHE 12000 -10300 250
GEHE 3000 -10300 250
GEHE 350 -9175 150
BLICK 2048
AKTION
WARTE 60
GEHE 8150 -23550 250
JAGE 5 25 4000 99999 9000
WARTE 30
ENDE
")
re15_start_spiel(_rv 1500
    RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 "RE15_GIVE=3:15,21:50"
    "RE15_DEBUG_JUMP=5091@250" "RE15_PLAYER_POS=27200,4900,2048,0" "RE15_SET_FLAG=3:89,9:130"
    "RE15_WEGPLAN=${WORKDIR}/wegplan.txt" RE15_WEGPLAN_LOG=wegplan_log.txt
    RE15_FINALE_LOG=1 RE15_KAMERAHALT_LOG=1 RE15_BOOT_EXIT_AT=2 "RE15_EXIT_AT=2400"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")
endif()

if(NOT EXISTS "${WORKDIR}/finale.log" OR NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "r42_finale_wurf_kamera: finale.log/debug.log fehlt (exit=${_rv}, ${WORKDIR})")
endif()
file(READ "${WORKDIR}/finale.log" _log)
file(STRINGS "${WORKDIR}/debug.log" _dbg REGEX "^\\[(kamerahalt|scd F[0-9]+\\] Cut_chg|bf-kamera|finale\\] T[0-9]+ F[0-9]+ Wurf: (Szene startet|Ende))")
function(bild_von _muster _var)
    if(_log MATCHES "\\[finale\\] T[0-9]+ F([0-9]+) ${_muster}")
        set(${_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    else()
        set(${_var} "" PARENT_SCOPE)
    endif()
endfunction()
bild_von("Wurf: Szene startet" _S)
bild_von("Wurf: Bild 0: John erscheint" _W)
bild_von("Wurf: Schnitt auf die Kamera des Spielers" _SK)
bild_von("Wurf: Reaktion" _R)
bild_von("Wurf: Ende" _E)
set(_f "")
if(NOT _S OR NOT _W OR NOT _SK OR NOT _R OR NOT _E)
    message(FATAL_ERROR "r42_finale_wurf_kamera: Szene unvollstaendig - Start '${_S}' / Bild 0 '${_W}' / Flugschnitt '${_SK}' / "
                        "Reaktion '${_R}' / Ende '${_E}' (exit=${_rv}, ${WORKDIR})\n${_log}")
endif()
# Vorbedingung: Spieler am Uebergang 12/13
if(NOT _log MATCHES "Wurf: Leon [(](-?[0-9]+),(-?[0-9]+)[)] Gier")
    message(FATAL_ERROR "r42_finale_wurf_kamera: Spielerlage beim Szenenstart fehlt\n${_log}")
endif()
set(_px "${CMAKE_MATCH_1}")
if(_px LESS 16000 OR _px GREATER 17500)
    message(FATAL_ERROR "r42_finale_wurf_kamera: VORBEDINGUNG verfehlt - Spieler beim Szenenstart bei x ${_px}, nicht am Kamera-Uebergang "
                        "12/13 (16000..17500). Der Riegel prueft hier nichts; Wegplan/Kampfverlauf hat sich geaendert.\n${_log}")
endif()
if(NOT _log MATCHES "Kamera vorher 12")
    string(APPEND _f "\n Kamera vor der Szene nicht 12 (Vorbedingung: Spieler sieht sich in Kamera 12)")
endif()
# Kamera je Bild aus [kamerahalt]
set(_n13a 0)
set(_n13b 0)
set(_in_szene 0)
foreach(_z IN LISTS _dbg)
    if(_z MATCHES "^\\[kamerahalt\\] F([0-9]+) raum=5091 [^\n]* cam=([0-9]+) ")
        set(_fr "${CMAKE_MATCH_1}")
        set(_cam "${CMAKE_MATCH_2}")
        if(_fr GREATER _W AND _fr LESS _SK)
            if(_cam EQUAL 13)
                math(EXPR _n13a "${_n13a} + 1")
            else()
                string(APPEND _f "\n Bild F${_fr} (Wurf, vor dem Flugschnitt F${_SK}) zeigt Kamera ${_cam} statt 13")
            endif()
        endif()
        if(_fr GREATER _R AND _fr LESS _E)
            if(_cam EQUAL 13)
                math(EXPR _n13b "${_n13b} + 1")
            else()
                string(APPEND _f "\n Bild F${_fr} (Reaktion/Abgang, vor dem Ende F${_E}) zeigt Kamera ${_cam} statt 13")
            endif()
        endif()
    elseif(_z MATCHES "Wurf: Szene startet")
        set(_in_szene 1)
    elseif(_z MATCHES "Wurf: Ende")
        set(_in_szene 0)
    elseif(_z MATCHES "^\\[bf-kamera\\] F[0-9]+ Wache")
        # Reihenfolge im debug.log statt Bildnummer: ein "Wache aus" im Startbild VOR dem Szenenstart gehoert zum Kampf
        # (gemessen F2123: die Form im Treffer, Wache gibt ab, erst danach startet die Szene).
        if(_in_szene)
            string(APPEND _f "\n Nahsicht-Wache schaltet in der Szene: ${_z}")
        endif()
    endif()
endforeach()
math(EXPR _soll_a "${_SK} - ${_W} - 1")
math(EXPR _soll_b "${_E} - ${_R} - 1")
if(NOT _n13a EQUAL _soll_a OR NOT _n13b EQUAL _soll_b)
    string(APPEND _f "\n Kamera 13 in ${_n13a}/${_soll_a} Bildern (W..Flugschnitt) und ${_n13b}/${_soll_b} Bildern (Reaktion..Ende)")
endif()
if(_f)
    string(REPLACE ";" "\n" _dbg_txt "${_dbg}")
    message(FATAL_ERROR "r42_finale_wurf_kamera: FEHLER (Spieler x ${_px}, W F${_W}, Flugschnitt F${_SK}, Reaktion F${_R}, Ende F${_E})${_f}\n"
                        "--- finale.log ---\n${_log}")
endif()
message(STATUS "r42_finale_wurf_kamera: OK (Spieler x ${_px}; Kamera 13 in ${_n13a} Bildern W F${_W}..F${_SK} und ${_n13b} Bildern "
               "Reaktion F${_R}..Ende F${_E}; keine Wache in der Szene)")
