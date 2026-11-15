# =============================================================================
# SCHWANZ ZUSAMMENHAENGEND (Runde 41 Schwanz r9, Zweig r41/sichel_szene) - Riegel gegen den frei haengenden Schwanz der Sichel-Form.
#
# Nutzer 2026-11-15 zu den Videos 30_sichel_schwanz_einheitlich (Standbilder birkin_fehler/): "in den Szenen teilweise so aus als taucht
# der schwanz jetzt garnicht mehr auf, oder ist nicht mehr sauber connected" und "vorher WAR er ja sauber connected".
# Ursache (Messlauf scratchpad schwanz_r9, BERICHT.md): seit c94847a31 zeichnete die Hangar-Szene die Glieder Wurzel -> Video-Austritt
# mit Querschnitt 0 (re15_schwanz_r41_verdeckt). Der Austritt ist ein Punkt des 80-%-Videos; der Port-Koerper deckt ihn nicht ab. Das
# sichtbare Stueck begann deshalb h25-h29 frei in der Luft (bis 104 Punkte neben dem Koerper in 960x720) und h60-h70 als Schnittkante
# vor der Huefte. Seit r9 werden alle Glieder gezeichnet und der Koerper verdeckt sie.
#
# Der Riegel faehrt die ECHTE exe dreimal durch die Hangar-Szene (Weg wie scratchpad vorschau_v8_szene/gd_v8.sh: Debug-Sprung 50A0,
# Affen per Kill-Flag aus, Eingabeskript nach Sueden -> Szene) mit Framedump F255-F350 (Szenentakt h12-h107), Fenster 320x240:
#   N normal, T RE15_SCHWANZ_MASKE=schwanz, L RE15_SCHWANZ_MASKE=figuren (Messhaken main.c, nur Messung).
# (1) probe_r41_schwanz_zusammen verlangt: in keinem Bild ein Schwanzteil (N != T) ohne Figurpunkt (T != L: Birkin oder Spider-Man) im
# Abstand <= 2 Punkte; dazu Schwanz in mindestens 60 Bildern (gemessen nach r9: 74 von 96; vor r9 ebenso - die Probe ist also nicht leer).
# Gegenprobe (vor r9, Querschnitt 0, sonst gleiche exe): (1) rot F268-F273 (h25-h30), (2) rot (verdeckt 3..7). Dazu (2) unten.
#
# Runde 2 (Pruefer r9 Runde 1, scratchpad schwanz_r9/pruefung/PRUEFUNG.md: der Riegel war ein Pixelnaehe-Mass und blieb gruen bei einem
# Knick wie ein Ellbogen h26-h30 und einem Schwanzstueck VOR dem rechten Oberschenkel h58-h60 / h71-h72). Vierter Lauf
#   B RE15_SCHWANZ_MASKE=birkin (Koerperpunkte = T != B), dazu im Auswerter:
# (3) VOR DEM KOERPER F301-F316 (h58-h73): kein Bild mit mehr als 20 Schwanzpunkten UND mehr als 20 % des sichtbaren Schwanzes ueber der
#     Birkin. Gemessen (scratchpad schwanz_r9/r2, 320x240): vorher 93a512aef h58/h59/h60/h71/h72 155/195, 173/288, 112/371, 234/317,
#     46/104 (30-79 %); nachher groesster Wert h62 60/595 (10 %, Nadel ueber dem Schienbein am Boden - im Video ebenso am Bein).
# (4) SICHTBARER KNICK F255-F350: an keinem Gelenk mit beiden Gliedern sichtbar ein Bildwinkel ueber 70 Grad. Vorher 93a512aef h26-h30
#     95/90/87/139/154 Grad (Gelenk 6); nachher groesster Wert 59 Grad (h21, unveraendert seit dem Stand der Videos 27/28: Austritt an der
#     Huefte h18-h22). 70 liegt zwischen beiden.
# Gegenproben (gleicher Auswerter auf eigenen Laeufen, scratchpad schwanz_r9/r2): 93a512aef ROT (3) und (4); Verhalten 7fd592184 (Querschnitt
# 0 der verdeckten Glieder) ROT (1), (3) und (4).
#
# Runde 3 (Pruefer r9 Runde 2, scratchpad schwanz_r9/pruefung_r2/PRUEFUNG.md: "rosa Wurzelstueck an der rechten Flanke zwischen Bauch und Arm
# bis ueber den Guertel" h59-h70 - der Schwanz kam seitlich aus der Taille statt in Hueft-Hoehe hinter der Huefte hervor, Areas3.png h64;
# (3) sah es nicht, weil der Streifen NEBEN der Silhouette lag):
# (5) UEBER DER AUSTRITTSZEILE F301-F315 (h58-h72): je Bild hoechstens 10 Schwanzpunkte (320x240) ueber der Zeile Austritt des
#     Original-Videos - 4,5 (halbe Schwanzbreite), auch neben der Silhouette (Auswerter: k_aus_video = tabelle_v8.json "austritt").
#     Gemessen (scratchpad schwanz_r9/r3): 4f2b7a354 38-190 je Bild h58-h70 (14 Fehlerbilder), Runde 3 hoechstens 3.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PRUEF_TOOL=<probe_r41_schwanz_zusammen> -DWORKDIR=<dir> -P test_r41_schwanz_zusammen.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "schwanz_zusammen: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PRUEF_TOOL OR NOT EXISTS "${RE15_PRUEF_TOOL}")
    message(FATAL_ERROR "schwanz_zusammen: RE15_PRUEF_TOOL fehlt: '${RE15_PRUEF_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "schwanz_zusammen: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
set(_f0 255)
set(_f1 350)

function(szene_lauf _name _maske _out_dir)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}/fd")
    set(_menv "RE15_SCHWANZ_LOG=schwanz.log")
    if(_maske)
        set(_menv "RE15_SCHWANZ_MASKE=${_maske}")
    endif()
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1
        RE15_NOAUDIO=1
        RE15_TITLE_SHOT=title.bmp
        RE15_TITLE_SHOT_AF=2
        RE15_WINDOW_SCALE=1
        RE15_HANGAR_LOG=1
        RE15_SET_FLAG=8:42,8:43
        RE15_DEBUG_JUMP=50A0@20
        RE15_PLAYER_POS=-12406,4381,1024
        "RE15_INPUT_SCRIPT=W0.5,UX3.6,W0.3,L0.27,W0.3,UX2.5,W12,UX1.5,W10"
        RE15_EXIT_AT=360
        "RE15_FRAMEDUMP=${_f0}-${_f1}/1:fd/f"
        ${_menv}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "schwanz_zusammen[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(STRINGS "${WORKDIR}/debug.log" _h26 REGEX "\\[hangar-takt\\] h26 F")
    if(NOT _h26)
        message(FATAL_ERROR "schwanz_zusammen[${_name}]: Hangar-Szene nicht erreicht (keine Zeile [hangar-takt] h26, exit=${_rv}, ${WORKDIR})")
    endif()
    if(NOT EXISTS "${WORKDIR}/fd/f000${_f1}.ppm")
        message(FATAL_ERROR "schwanz_zusammen[${_name}]: Framedump F${_f1} fehlt (exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_out_dir} "${WORKDIR}/fd" PARENT_SCOPE)
endfunction()

szene_lauf(n "" _dn)
# (2) Kein Glied wird kuenstlich unsichtbar gemacht: der Messhaken schreibt je gezeichnetem Bild die Zahl der Glieder ab der Wurzel ohne
# Querschnitt ("verdeckt N"). Vor r9 stand dort h16-h35 / h58-h73 3..6 (sichtbares Stueck mit Schnittkante, frei oder VOR der Huefte) und
# h36-h57 / h74-h75 7 (Schwanz ganz weg). Verlangt: in der Szene F${_f0}-F${_f1} ueberall 0 - was hinter dem Koerper liegt, verdeckt der Koerper.
file(STRINGS "${_dn}/../schwanz.log" _sl REGEX "^\\[schwanz-bild\\] F")
set(_zeilen 0)
set(_ohne "")
foreach(_z ${_sl})
    if(_z MATCHES "^\\[schwanz-bild\\] F([0-9]+) verdeckt ([0-9]+)")
        if(CMAKE_MATCH_1 GREATER_EQUAL ${_f0} AND CMAKE_MATCH_1 LESS_EQUAL ${_f1})
            math(EXPR _zeilen "${_zeilen} + 1")
            if(NOT CMAKE_MATCH_2 EQUAL 0)
                list(APPEND _ohne "F${CMAKE_MATCH_1}:${CMAKE_MATCH_2}")
            endif()
        endif()
    endif()
endforeach()
if(_zeilen LESS 90)
    message(FATAL_ERROR "schwanz_zusammen: nur ${_zeilen} Zeilen [schwanz-bild] in F${_f0}-F${_f1} (Messhaken RE15_SCHWANZ_LOG?)")
endif()
if(_ohne)
    message(FATAL_ERROR "schwanz_zusammen: Glieder ohne Querschnitt (Bild:Zahl) ${_ohne}")
endif()
message(STATUS "schwanz_zusammen: ${_zeilen} Bilder, kein Glied ohne Querschnitt")
szene_lauf(t "schwanz" _dt)
szene_lauf(l "figuren" _dl)
szene_lauf(b "birkin" _db)

execute_process(COMMAND "${RE15_PRUEF_TOOL}" "${_dn}" "${_dt}" "${_dl}" ${_f0} ${_f1} 2 3
                        "${_db}" "${_dn}/../schwanz.log" 70 301 316 200 20 301 315 10
                TIMEOUT 120
                RESULT_VARIABLE _rv
                OUTPUT_VARIABLE _out)
message("${_out}")
string(REGEX MATCH "mit Schwanz ([0-9]+)" _m "${_out}")
set(_mit "${CMAKE_MATCH_1}")
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "schwanz_zusammen: Schwanzteil haengt frei in der Luft, liegt vor dem Koerper, knickt sichtbar oder kommt ueber der Austrittszeile aus der Taille (exit=${_rv}) - Dumps ${_basis}_*")
endif()
if(NOT _mit OR _mit LESS 60)
    message(FATAL_ERROR "schwanz_zusammen: Schwanz nur in '${_mit}' von 96 Bildern sichtbar (Soll >= 60) - Messhaken oder Szene kaputt")
endif()
file(REMOVE_RECURSE "${_basis}_n" "${_basis}_t" "${_basis}_l" "${_basis}_b")
message(STATUS "schwanz_zusammen: OK (Schwanz in ${_mit} Bildern, kein frei haengender Teil, nicht vor dem Koerper, kein sichtbarer Knick)")
