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
# (6) TON MITTE/RAND F317-F343 (Bogen h74-h100, Nutzer 15.11. "komische braune Streifen an den Raendern", Entscheid des Orchestrators: einheitlich
#     auch ueber die Breite): Querprofile senkrecht zu jedem sichtbaren Glied, dEab (a*b* ohne L* = ohne Schattierung) Mitte gegen Rand je
#     Seite hoechstens 6,5 in jedem gemessenen Bild (mindestens 10 Bilder). Gemessen 320x240 (scratchpad schwanz_r9/r3): Asset a48e2ce5
#     (graue Flanke) 7,7-11,3 in 27 von 27 Bildern ROT, Asset 92a58f26 (Runde 8f, Flanke im Bauchfeld C1) 3,7-5,3; 6,5 liegt dazwischen.
#     Original-Video (nur Richter, 640x480): Mittel 2,7, hoechstens 4,6.
#
# Runde 4 (Pruefer r9 Runde 3, scratchpad schwanz_r9/pruefung_r3/PRUEFUNG.md: h58-h61 / h71 Schwanz fast unsichtbar, h15/h16 rosa
# Wurzelstueck an der linken Taille zwischen Rumpf und Arm, h17 / h29-h35 Stueck vor Schritt und Oberschenkel - kein Riegel sah es):
# (3) jetzt F258-F316 (h15-h73) statt F301-F316, gleiche Grenzen (mehr als 20 Punkte UND mehr als 20 %).
# (5b) TAILLE LINKS F255-F267 (h12-h24): hoechstens 10 Schwanzpunkte (320x240) je Bild NEBEN der Silhouette ueber dem Guertel (oberste
#     Jeanszeile) rechts der Koerpermitte (Bild rechts = Birkins linke Taille zwischen Rumpf und Arm). Original h16-h24: der Schwanz kommt
#     links aus der Koerpermitte; vorher (d8ef12963) lief dort h12-h16 ein rosa Streifen bis 18 Zeilen (320x240) ueber dem Guertel.
# (7) MINDEST-SICHTBARKEIT F301-F314 (h58-h71): je Bild mindestens 100 Schwanzpunkte (320x240). 100 = knapp die Haelfte der kleinsten
#     Video-Schwanzflaeche der Strecke (h58: Videolinie (405,322) -> (418,372) 51,7 Videopunkte = 25 Punkte lang x Schwanzbreite 9 = 225).
# Messwerte und Gegenproben: scratchpad schwanz_r9/r4 (BERICHT_r4.md).
#
# Runde 5 (Pruefer r9 Runde 4, scratchpad schwanz_r9/pruefung_r4/PRUEFUNG.md: h30-h34 und h72 ein kurzer rosa Klumpen unter dem Schritt, h30/h31
# geknickt 107 / 84 Grad in 960x720 - (4) sah es bei 320x240 nicht, eine Mindestlaenge fehlte; dazu Nutzer 2026-11-15 zur Farbe "die Oberseite des
# Schwanzes wieder eher rot als dunkel" / "NUR DIE UNTERSEITE WAR KOMISCH MIT DEN ABSCHNITTEN!!!!"):
# (4) zaehlt auch kleine sichtbare Stuecke (Glied >= 3 Punkte, Schwanzpunkt an 30 %, 50 % ODER 70 %).
# (6) "Ton Mitte/Rand im Bogen" (d8ef12963) ist ERSETZT durch die Farbregel 8g (Asset a02a6df3, r41/sichel_v8 83350f359):
#   (6a) Spiel-Asset EM30_HANGAR.EMD sha256 == Pin release/em30_hangar.sha256 == 8g a02a6df3.
#   Drei weitere Laeufe mit ausgetauschtem Schwanz-Asset (RE15_CD_ROOT-Ueberlagerung, sonst alles gleich): C8 = Asset 8c d31f9534 (Video 28,
#   SF_belege/v8/bewegung/), MK = Klassen-Marker, NAHT = Naht-Marker (beide aus dem Spiel-Asset 8g mit runde8g/werkzeug/marker.py, abgelegt
#   unter SF_belege/v8/schwanz_r9_r5/). Auswerter probe_r41_schwanz_farbe:
#   (6b) Oberseite/Flanke/Nadel im Bogen F317-F343 (darin F336 = Nutzerbild 3) bildpunktgleich zu 8c: hoechstens 10 Promille anders.
#   (6c) Unterseite ohne Abschnitte F255-F350: dE76 Mittel Bauch gegen Bauch-Fuge hoechstens 8,0.
#   (6d) hoechstens 100 Schwanzpunkte mit Naht-Texel u 54-55 in F255-F350 (bis zur v0.8.31-Basis 0, s. "(6d) und der Adapter").
#   Gemessen (320x240, scratchpad schwanz_r9/r5): 8g 0,00 % / 3,54 / 0; Gegenproben 8f 92a58f26 (b) 98,07 % ROT, 8c (c) 18,33 ROT, 8d 547acc80
#   (d) 5172 ROT (und (b) 61,62 %).
#   (6d) UND DER ADAPTER (v0.8.31-Basis Runde 2, scratchpad v0831_basis/r2, BERICHT_r2.md). Ab 17.11. 18:50 war (6d) mit 35 Punkten rot, auf allen
#   Staenden (r42/story, r42/hangar_aussen fe5436f55, release_0830 ec0d39bb9 mit dem alten Ausloeser F255-F350, und die exe f7057a708, die am
#   16.11. 18:38 hier 0 gemessen hatte - dieselbe Datei, dieselben Assets). Die Szene selbst ist bitgleich: N-, T- und NAHT-Laeufe h12-h107 der
#   exe mit altem Ausloeser (release) und der exe mit neuem (r42/story) sind Bild fuer Bild gleich. Geaendert hat sich die Sitzung: bis 17.11.
#   17:51 lokale Konsole, seit 18:12 Fernsitzung bzw. getrennt (LocalSessionManager Ereignis 24/25); SDL nimmt fuer Direct3D 11 den DXGI-Adapter 0,
#   in dieser Sitzung "NVIDIA GeForce RTX 4060 Laptop GPU" ohne Ausgang (gemessen mit EnumAdapters1). Mechanismus (RE15_TRILOG an den Naht-
#   Punkten): die 8g-Flaechen beruehren die Naht-Spalte nur mit Ecken und Kanten (z. B. uv (49,57) (54,100) (55,55), (43,55) (49,80) (55,80));
#   ob ein Bildpunkt-Mittelpunkt dort Texel 54/55 trifft, entscheidet die Rasterung der GPU. Auf dem NVIDIA-Weg sind es 1-6 Punkte je Bild
#   (F74-F85, F91-F104, F115/F116 = h60-h71, h77-h90, h101/h102), einzelne dunkle Punkte bzw. kurze 1-Punkt-Linien. Gegenproben auf demselben
#   Adapter: 8c 4490, 8d 5174 (je Bild bis 150 / 172), auf der Konsole 4489 / 5172. Grenze 100 = knapp das Dreifache des hoechsten 8g-Werts
#   (35), Faktor 45 unter der kleinsten Gegenprobe; sie trennt "Naht-Texel nur an Flaechenraendern" von "Flaechen liegen auf der Naht".
# (8) MINDESTLAENGE F272-F278 (h29-h35) und F315 (h72): je Bild 0 Schwanzpunkte (verdeckt) oder ein Stueck mindestens so lang wie die Linie des
#   Original-Videos und mindestens zwei Schwanzbreiten (18 Punkte 320x240). Gemessen: Runde 4 (47eab4cdb) ROT h30-h34 (9,8-22,1 gegen
#   20,0-25,0) und (4) h30-h32 (107 / 84 / 74 Grad); Runde 5 h30-h34 29,8-31,3, h35 / h72 verdeckt.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_PRUEF_TOOL=<probe_r41_schwanz_zusammen> -DWORKDIR=<dir> -P test_r41_schwanz_zusammen.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "schwanz_zusammen: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_PRUEF_TOOL OR NOT EXISTS "${RE15_PRUEF_TOOL}")
    message(FATAL_ERROR "schwanz_zusammen: RE15_PRUEF_TOOL fehlt: '${RE15_PRUEF_TOOL}'")
endif()
if(NOT RE15_FARB_TOOL OR NOT EXISTS "${RE15_FARB_TOOL}")
    message(FATAL_ERROR "schwanz_zusammen: RE15_FARB_TOOL fehlt: '${RE15_FARB_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "schwanz_zusammen: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# (6a) Spiel-Asset = Pin = 8g (Runde 5)
set(_asset "${CMAKE_CURRENT_LIST_DIR}/../../shared_assets/PSX/EMD/EM30_HANGAR.EMD")
set(_pin_datei "${CMAKE_CURRENT_LIST_DIR}/../../../release/em30_hangar.sha256")
set(_sha_8g "a02a6df3472b99597ad520f7c76bcff62f3665ca01c495358ff5aac3c31cc744")
file(SHA256 "${_asset}" _sha)
file(STRINGS "${_pin_datei}" _pin LIMIT_COUNT 1)
string(REGEX MATCH "^[0-9a-f]+" _pin "${_pin}")
if(NOT _sha STREQUAL _pin OR NOT _sha STREQUAL _sha_8g)
    message(FATAL_ERROR "schwanz_zusammen (6a): Spiel-Asset ${_sha}, Pin ${_pin}, verlangt 8g ${_sha_8g}")
endif()
message(STATUS "schwanz_zusammen (6a): Spiel-Asset = Pin = 8g a02a6df3")
set(_belege "${CMAKE_CURRENT_LIST_DIR}/../../../analysis/befunde_runde41/SF_belege/v8")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
set(_f0 255)
set(_f1 350)

# _asset_ueber: leer = Spiel-Asset, sonst EMD-Datei, die per RE15_CD_ROOT-Ueberlagerung (<lauf>/cd/EMD/EM30_HANGAR.EMD) das Schwanz-Asset ersetzt
function(szene_lauf _name _maske _asset_ueber _out_dir)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}/fd")
    set(_menv "RE15_SCHWANZ_LOG=schwanz.log")
    if(_maske)
        set(_menv "RE15_SCHWANZ_MASKE=${_maske}")
    endif()
    if(_asset_ueber)
        if(NOT EXISTS "${_asset_ueber}")
            message(FATAL_ERROR "schwanz_zusammen[${_name}]: Ueberlagerungs-Asset fehlt: ${_asset_ueber}")
        endif()
        file(MAKE_DIRECTORY "${WORKDIR}/cd/EMD")
        configure_file("${_asset_ueber}" "${WORKDIR}/cd/EMD/EM30_HANGAR.EMD" COPYONLY)
        list(APPEND _menv "RE15_CD_ROOT=${WORKDIR}/cd")
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

szene_lauf(n "" "" _dn)
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
szene_lauf(t "schwanz" "" _dt)
szene_lauf(l "figuren" "" _dl)
szene_lauf(b "birkin" "" _db)
szene_lauf(c8 "" "${_belege}/bewegung/EM30_HANGAR_d31f9534.EMD" _dc8)
szene_lauf(mk "" "${_belege}/schwanz_r9_r5/EM30_HANGAR_a02a6df3_marker_klasse.EMD" _dmk)
szene_lauf(naht "" "${_belege}/schwanz_r9_r5/EM30_HANGAR_a02a6df3_marker_naht.EMD" _dnaht)

execute_process(COMMAND "${RE15_PRUEF_TOOL}" "${_dn}" "${_dt}" "${_dl}" ${_f0} ${_f1} 2 3
                        "${_db}" "${_dn}/../schwanz.log" 70 258 316 200 20 301 315 10 272 278 100 255 267 301 314 100 315
                TIMEOUT 120
                RESULT_VARIABLE _rv
                OUTPUT_VARIABLE _out)
message("${_out}")
string(REGEX MATCH "mit Schwanz ([0-9]+)" _m "${_out}")
set(_mit "${CMAKE_MATCH_1}")
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "schwanz_zusammen: Schwanzteil haengt frei in der Luft, liegt vor dem Koerper, knickt sichtbar, kommt ueber der Austrittszeile aus der Taille oder ist ein Klumpen (exit=${_rv}) - Dumps ${_basis}_*")
endif()
execute_process(COMMAND "${RE15_FARB_TOOL}" "${_dn}" "${_dt}" "${_dc8}" "${_dmk}" "${_dnaht}" ${_f0} ${_f1} 317 343 10 80 100
                TIMEOUT 120
                RESULT_VARIABLE _rvf
                OUTPUT_VARIABLE _outf)
message("${_outf}")
if(NOT _rvf EQUAL 0)
    message(FATAL_ERROR "schwanz_zusammen (6): Farbe nicht 8g - Oberseite anders als 8c, Unterseite mit Abschnitten oder Naht-Texel sichtbar (exit=${_rvf}) - Dumps ${_basis}_*")
endif()
if(NOT _mit OR _mit LESS 60)
    message(FATAL_ERROR "schwanz_zusammen: Schwanz nur in '${_mit}' von 96 Bildern sichtbar (Soll >= 60) - Messhaken oder Szene kaputt")
endif()
file(REMOVE_RECURSE "${_basis}_n" "${_basis}_t" "${_basis}_l" "${_basis}_b" "${_basis}_c8" "${_basis}_mk" "${_basis}_naht")
message(STATUS "schwanz_zusammen: OK (Schwanz in ${_mit} Bildern, kein frei haengender Teil, nicht vor dem Koerper, kein sichtbarer Knick, kein Klumpen, Farbe 8g)")
