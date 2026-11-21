# integration_r42_fabrikplan_d0_bild — Strang fabrikplan Stufe 4 (21.11.2026): der Fabrikplan in ROOM30D0 (Eingangshalle nach der
# Lobby-Szene) ist in jeder Kamera, in der er ganz im Bild liegt, auch WIRKLICH zu sehen - nicht verdeckt (Pruefer Stufe 1 gelb 3:
# "nur Ecken im Bild" reicht nicht). Muster integration_r36_kartenitems_bild: je Kamera zwei Laeufe der ECHTEN exe am Lade-Weg
# (Speicherkarte aus probe_r35_inhalt_karte: Raum 30D0, Leon abseits bei (-24000,-27000), Lobby-Szene (3,0x47) = Set @0x00CB8 in
# ROOM30E0 sub02, die acht Zombies der Halle per Kill-Flag 7:146..153 erledigt = Sce_em_set pc[7] 0x92..0x99 @0x0130A..@0x01396,
# Gate FUN_800420a0 @0x80042120-38), RE15_FORCE_CUT, Framedump Bild 100 (320 x 240): einmal OHNE Bit 35 (Plan haengt), einmal MIT
# (kein Plan). probe_r34n_e_bild zaehlt die verschiedenen Bildpunkte im Fenster um das projizierte Plakat-Viereck.
# Soll je Kamera: (a) der Lauf ohne Bit legt die Karte an, der mit Bit nicht ("[kartenitems] ROOM30D0: Karte");
#                 (b) im Fenster mindestens 90 % der Viereck-Flaeche verschieden (= sichtbar, nicht verdeckt);
#                 (c) ausserhalb des Fensters 0 (der Unterschied IST das Plakat).
# Viereck-Flaeche = Projektion der Plakatflaeche x -8680..-6520, y -2720..-1280, z 1100 (karten_export.py Zeile 30D0) mit der
# RDT-Kamera (probe_r34n_e_dokumente kamera 30D0), gemessen 2026-11-21 (BERICHT_s4_r1.md): Cut 1 181, Cut 2 192, Cut 6 8289,
# Cut 8 1109, Cut 10 659, Cut 11 222, Cut 13 179 Bildpunkte; verschieden (Schwelle |dR|+|dG|+|dB| > 30): 177/201/8331/1149/645/
# 232/191, aussen je 0. Die uebrigen Kameras (0, 3, 4, 5, 7, 9, 12, 14, 15) zeigen das Plakat nicht ganz (Ecke ausserhalb des
# Bilds oder hinter der Kamera) und haben 0 Unterschied.
# Gegenprobe: auf 974e4b9b3 (Stand vor Stufe 4, keine Zeile 30D0) ist jeder Fall rot (keine Karte, 0 Bildpunkte).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r35_inhalt_karte> -DRE15_BILD_TOOL=<probe_r34n_e_bild>
#               -DWORKDIR=<dir> -P test_r42_fabrikplan_d0_bild.cmake

foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_BILD_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "fabrikplan_d0_bild: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "fabrikplan_d0_bild: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
set(_flags flag:3:71 flag:7:146 flag:7:147 flag:7:148 flag:7:149 flag:7:150 flag:7:151 flag:7:152 flag:7:153)

function(lauf _name _cut _bit)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" 30D0 -24000 -27000 0 ${_flags} ${_bit}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "fabrikplan_d0_bild[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 240 RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1
                     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_FORCE_CUT=${_cut}
                     RE15_FRAMEDUMP=100:raum.ppm "RE15_EXIT_AT=101#30D0" "${RE15_PC_EXE}")
    set(_log "")
    if(EXISTS "${WORKDIR}/debug.log")
        file(READ "${WORKDIR}/debug.log" _log)
    endif()
    string(FIND "${_log}" "CONTINUE: resumed in room 30d0" _pc)
    if(_pc LESS 0 OR NOT EXISTS "${WORKDIR}/raum.ppm")
        message(FATAL_ERROR "fabrikplan_d0_bild[${_name}]: Spielstand nicht geladen oder kein Framedump (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_log}" "[kartenitems] ROOM30D0: Karte" _pk)
    set(LAUF_DIR "${WORKDIR}" PARENT_SCOPE)
    set(LAUF_KARTE "${_pk}" PARENT_SCOPE)
endfunction()

# cut | fenster x0 y0 x1 y1 (Huelle des Vierecks +-3) | flaeche
set(_faelle
    "1|29|138|52|157|181"
    "2|135|80|157|98|192"
    "6|0|48|112|155|8289"
    "8|185|105|232|139|1109"
    "10|236|99|271|129|659"
    "11|282|91|303|114|222"
    "13|216|152|235|187|179")

set(_fehler "")
foreach(_f ${_faelle})
    string(REPLACE "|" ";" _t "${_f}")
    list(GET _t 0 _cut)
    list(GET _t 1 _x0)
    list(GET _t 2 _y0)
    list(GET _t 3 _x1)
    list(GET _t 4 _y1)
    list(GET _t 5 _fl)
    math(EXPR _min "${_fl} * 9 / 10")
    lauf(c${_cut}_mit ${_cut} "")
    set(_dir_mit "${LAUF_DIR}")
    set(_k_mit "${LAUF_KARTE}")
    lauf(c${_cut}_ohne ${_cut} "bit:35")
    set(_dir_ohne "${LAUF_DIR}")
    set(_k_ohne "${LAUF_KARTE}")
    if(_k_mit LESS 0)
        string(APPEND _fehler "Cut ${_cut}: ohne Bit 35/139 keine Karte in ROOM30D0\n")
    endif()
    if(NOT _k_ohne LESS 0)
        string(APPEND _fehler "Cut ${_cut}: Karte trotz Bit 35 angelegt\n")
    endif()
    execute_process(COMMAND "${RE15_BILD_TOOL}" "${_dir_mit}/raum.ppm" "${_dir_ohne}/raum.ppm" ${_x0} ${_y0} ${_x1} ${_y1} ${_min}
                    TIMEOUT 60 RESULT_VARIABLE _rvb OUTPUT_VARIABLE _outb)
    string(STRIP "${_outb}" _outb)
    message(STATUS "fabrikplan_d0_bild Cut ${_cut} (Viereck ${_fl} px, Soll >= ${_min}): ${_outb}")
    if(NOT _rvb EQUAL 0)
        string(APPEND _fehler "Cut ${_cut}: Plakat nicht (ganz) sichtbar: ${_outb}\n")
    endif()
    if(NOT _outb MATCHES "AUSSEN: 0$")
        string(APPEND _fehler "Cut ${_cut}: Unterschied ausserhalb des Plakat-Fensters: ${_outb}\n")
    endif()
endforeach()

if(_fehler)
    message(FATAL_ERROR "fabrikplan_d0_bild:\n${_fehler}")
endif()
message(STATUS "fabrikplan_d0_bild: OK")
