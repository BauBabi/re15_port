cmake_minimum_required(VERSION 3.16)
# =============================================================================
# ITEMORTE MIT DER ECHTEN EXE (Runde 38, Spur K). Dossier analysis/befunde_runde38/K_itemorte.md, Konstanten
# include/re15_itemorte_r38.h. NUTZER-VORGABE (AUFTRAG.md Runde 38): Feuerloescher in ROOM1030 an der Stelle von
# fire_extinguisher.bmp (Cut 0, Marke x 38..48 y 111..122) statt ROOM1050; Parking Lot Key / Prison Key gut sichtbar an
# der Stelle von key_location.bmp (ROOM10F0 Cut 5, Marke x 133..143 y 77..85).
#
#   B1/B2/B3  BILD im Nutzer-Cut, je zwei Laeufe, einziger Unterschied das Taken-Bit: der Unterschied (|dR|+|dG|+|dB| > 30)
#             liegt im Fenster um die Marke (+-2 px) und nirgends sonst — Loescher 1030 (Leon), Parking Lot Key 10F0
#             (Leon), Prison Key 10F1 (Elza)
#   B4        Prison Key in der Spiel-Fenstergroesse (RE15_WINDOW_SCALE=3)
#   B3/B4/B5  "GUT SICHTBAR" (Nachbesserung 1, Mangel M1 der Abnahme 0): Prison Key = RE2 Small Key (vorher Cabin Key, ein
#             duenner dunkler Ring: 14 Pixel / Sum|dL| 502 bei Skala 1, 112 / 3193 bei Skala 3). Gemessen werden Pixelzahl
#             UND Kontrastsumme Sum|dL| (probe_r38_itemorte_kontrast) und ihr Verhaeltnis zum Parking Lot Key am selben Ort
#             (B2 Skala 1, B5 Skala 3).
#   A        AUFNAHME 1030 vor dem Pfeiler: Modal -> Ja -> (3,0x85); der dritte Druck an derselben Stelle zeigt den
#             verschobenen Monitor-Satz msg 14 (die genommene Zone wird uebersprungen, @0x80042f50)
#   K1/K2     AUFNAHME an der Pinnwand 10F0 (Leon) / 10F1 (Elza, + Zellen-Flag (4,0xEC)); danach msg 4
#   V1/V2/V3  SICHT: 1030 Cut 10 nach dem Rolltor-Tausch ((4,0x0F), Cut_replace 3->10 @0x01FF7) und 10F0 Cut 7 zeigen
#             KEINEN Unterschied (Regel re15_itemorte_r38_verdeckt), 10F0 Cut 6 zeigt den Schluessel
#   V4/V5     dasselbe bei Elza: 10F1 Cut 7 ohne, Cut 6 mit Prison Key
#   E         Elza: ROOM1031 ohne Loescher; ROOM1091 der leere Loescher am Boden wie Runde 37
#   O         ROOM1050 (Leon): kein Loescher mehr
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r37_feuerloescher_karte> -DRE15_BILD_TOOL=<probe_r34n_e_bild>
#               -DWORKDIR=<dir> -P test_r38_itemorte.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_BILD_TOOL RE15_KONTRAST_TOOL)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r38_itemorte: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_itemorte: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# EIGENER EXE-NAME neben der exe (Asset-Wurzel = exe-Verzeichnis; Muster test_r37_feuerloescher.cmake).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38k_haken_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r38k_haken_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

set(_fehler "")
macro(pruef _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(_p LESS 0)
        list(APPEND _fehler "${_name}: fehlt '${_muster}'")
        message(STATUS "FAIL ${_name}: fehlt '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: '${_muster}'")
    endif()
endmacro()
macro(pruef_nicht _name _text _muster)
    string(FIND "${${_text}}" "${_muster}" _p)
    if(NOT _p LESS 0)
        list(APPEND _fehler "${_name}: unerwartet '${_muster}'")
        message(STATUS "FAIL ${_name}: unerwartet '${_muster}'")
    else()
        message(STATUS "ok   ${_name}: nicht da '${_muster}'")
    endif()
endmacro()

# Ein Lauf: Karte schreiben, exe starten (CONTINUE), debug.log lesen. _extra = ;-Liste KEY=WERT.
function(k_lauf _name _raum _kartenargs _skript _ende _extra _out)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_kartenargs}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r38_itemorte[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 400
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
        "RE15_INPUT_SCRIPT=${_skript}"
        RE15_MSG_LOG=1
        "RE15_EXIT_AT=${_ende}"
        ${_extra}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r38_itemorte[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(TOLOWER "${_raum}" _raum_kl)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum_kl}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r38_itemorte[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r38_itemorte[${_name}]: Endbild nicht erreicht - Lauf vorher abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    message(STATUS "r38_itemorte[${_name}]: exit=${_rv}, ${WORKDIR}")
    set(${_out} "${_dbg}" PARENT_SCOPE)
endfunction()

# Bildvergleich zweier Laeufe (Bild 100): Pixel im Fenster >= _min, ausserhalb == 0 bzw. (bei _min 0) im Fenster == 0.
function(k_bild _name _mit _ohne _x0 _y0 _x1 _y1 _min)
    set(_a "${_basis}_${_mit}/fd_100.ppm")
    set(_b "${_basis}_${_ohne}/fd_100.ppm")
    execute_process(COMMAND "${RE15_BILD_TOOL}" "${_a}" "${_b}" ${_x0} ${_y0} ${_x1} ${_y1} 0
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _out)
    string(STRIP "${_out}" _out)
    message(STATUS "r38_itemorte[${_name}]: ${_out}")
    if(NOT _out MATCHES "FENSTER [^:]*: ([0-9]+) Pixel" )
        set(_fehler "${_fehler};${_name}: Bildwerkzeug-Ausgabe unlesbar '${_out}'" PARENT_SCOPE)
        return()
    endif()
    set(_innen "${CMAKE_MATCH_1}")
    string(REGEX MATCH "AUSSEN: ([0-9]+)" _m "${_out}")
    set(_aussen "${CMAKE_MATCH_1}")
    if(_min EQUAL 0)
        if(NOT _innen EQUAL 0)
            set(_fehler "${_fehler};${_name}: ${_innen} Pixel gezeichnet (Soll 0, verdeckt)" PARENT_SCOPE)
            message(STATUS "FAIL ${_name}: ${_innen} Pixel (Soll 0)")
        else()
            message(STATUS "ok   ${_name}: 0 Pixel (nicht gezeichnet)")
        endif()
    else()
        if(_innen LESS _min OR NOT _aussen EQUAL 0)
            set(_fehler "${_fehler};${_name}: Fenster ${_innen} (Soll >= ${_min}), aussen ${_aussen} (Soll 0)" PARENT_SCOPE)
            message(STATUS "FAIL ${_name}: Fenster ${_innen} (Soll >= ${_min}), aussen ${_aussen}")
        else()
            message(STATUS "ok   ${_name}: Fenster ${_innen} >= ${_min}, aussen 0")
        endif()
    endif()
endfunction()

# Kontrast zweier Laeufe (Bild 100): Pixel im Fenster >= _min_px, Sum|dL| >= _min_dl, ausserhalb 0; Sum|dL| -> _out.
function(k_kontrast _name _mit _ohne _x0 _y0 _x1 _y1 _min_px _min_dl _out)
    set(_a "${_basis}_${_mit}/fd_100.ppm")
    set(_b "${_basis}_${_ohne}/fd_100.ppm")
    execute_process(COMMAND "${RE15_KONTRAST_TOOL}" "${_a}" "${_b}" ${_x0} ${_y0} ${_x1} ${_y1}
                    RESULT_VARIABLE _rv OUTPUT_VARIABLE _o)
    string(STRIP "${_o}" _o)
    message(STATUS "r38_itemorte[${_name}]: ${_o}")
    if(NOT _o MATCHES "FENSTER [^:]*: ([0-9]+) Pixel, SUMDL ([0-9]+),.*AUSSEN: ([0-9]+)")
        set(_fehler "${_fehler};${_name}: Kontrastwerkzeug-Ausgabe unlesbar '${_o}'" PARENT_SCOPE)
        set(${_out} 0 PARENT_SCOPE)
        return()
    endif()
    set(_px "${CMAKE_MATCH_1}")
    set(_dl "${CMAKE_MATCH_2}")
    set(_au "${CMAKE_MATCH_3}")
    if(_px LESS _min_px OR _dl LESS _min_dl OR NOT _au EQUAL 0)
        set(_fehler "${_fehler};${_name}: ${_px} Pixel (Soll >= ${_min_px}), Sum|dL| ${_dl} (Soll >= ${_min_dl}), aussen ${_au} (Soll 0)" PARENT_SCOPE)
        message(STATUS "FAIL ${_name}: ${_px} Pixel (>= ${_min_px}), Sum|dL| ${_dl} (>= ${_min_dl}), aussen ${_au}")
    else()
        message(STATUS "ok   ${_name}: ${_px} Pixel >= ${_min_px}, Sum|dL| ${_dl} >= ${_min_dl}, aussen 0")
    endif()
    set(${_out} "${_dl}" PARENT_SCOPE)
endfunction()
# Verhaeltnis zweier Kontrastsummen in Prozent: 100 * _zaehler >= _min_proz * _nenner (Nenner 0 = Fehler).
function(k_verhaeltnis _name _zaehler _nenner _min_proz)
    if("${_zaehler}" STREQUAL "" OR "${_nenner}" STREQUAL "")
        set(_fehler "${_fehler};${_name}: Kontrastsumme fehlt ('${_zaehler}' / '${_nenner}')" PARENT_SCOPE)
        message(STATUS "FAIL ${_name}: Kontrastsumme fehlt")
        return()
    endif()
    math(EXPR _l "100 * ${_zaehler}")
    math(EXPR _r "${_min_proz} * ${_nenner}")
    if(_nenner EQUAL 0 OR _l LESS _r)
        set(_fehler "${_fehler};${_name}: Prison Key ${_zaehler} gegen Parking Lot Key ${_nenner} (Soll >= ${_min_proz} %)" PARENT_SCOPE)
        message(STATUS "FAIL ${_name}: Sum|dL| Prison ${_zaehler} / Parking ${_nenner} < ${_min_proz} %")
    else()
        message(STATUS "ok   ${_name}: Sum|dL| Prison ${_zaehler} / Parking ${_nenner} >= ${_min_proz} %")
    endif()
endfunction()

set(_fd "RE15_FRAMEDUMP=100:fd_100.ppm")

# --- B1: Loescher ROOM1030 Cut 0 (Nutzer-Cut), Leon steht rechts im Bild (210 ; 154) -----------------------------------
k_lauf(b1_mit  1030 "1030;-11500;-19500;2048;cut:0" "W3" 120 "${_fd}" _b1)
pruef(B1 _b1 "[itemorte] ROOM1030: Slot 14 (msg 14) -> Slot 20")
pruef(B1 _b1 "[feuerloescher] ROOM1030: Loescher an der Wand obj 5 bei (-2813,-2122,-17904) rot_y 3072, Zone Slot 14")
pruef(B1 _b1 "oid=0x05 pos=(-2813,-2122,-17904) rot=(0,3072,0)")
k_lauf(b1_ohne 1030 "1030;-11500;-19500;2048;cut:0;flag:9:0x86;flag:3:0x85" "W3" 120 "${_fd}" _b1o)
pruef(B1 _b1o "[feuerloescher] ROOM1030: kein Loescher (schon genommen, (9,0x86)=1)")
k_bild(B1 b1_mit b1_ohne 36 109 50 124 40)

# --- B2/B3: Schluessel an der Pinnwand ROOM10F0 (Leon) / ROOM10F1 (Elza), Cut 5 ----------------------------------------
k_lauf(b2_mit  10F0 "10F0;0;4000;1024;cut:5;flag:9:71;flag:3:94" "W3" 120 "${_fd}" _b2)
pruef(B2 _b2 "[schluessel] ROOM10F0: PARKING LOT KEY (Item 0x45, Bit (9,82)) Prop obj 4 bei (1358,-2495,-3400) rot (3072,0,2048), Zone Slot 8")
pruef(B2 _b2 "oid=0x04 pos=(1358,-2495,-3400) rot=(3072,0,2048)")
k_lauf(b2_ohne 10F0 "10F0;0;4000;1024;cut:5;flag:9:71;flag:3:94;flag:9:82" "W3" 120 "${_fd}" _b2o)
k_bild(B2 b2_mit b2_ohne 131 75 145 87 40)
# Kontrast des Parking Lot Key (abgenommener Stand, Abnahme 0: 55 Pixel, Sum|dL| ~3430) = Bezug fuer B3.
k_kontrast(B2K b2_mit b2_ohne 131 75 145 87 40 2700 _dl_park1)
k_lauf(b3_mit  10F1 "10F1;0;4000;1024;cut:5;elza" "W3" 120 "${_fd}" _b3)
pruef(B3 _b3 "[schluessel] ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83)) Prop obj 4 bei (1358,-2495,-3400) rot (1024,0,0), Zone Slot 8")
k_lauf(b3_ohne 10F1 "10F1;0;4000;1024;cut:5;elza;flag:9:83" "W3" 120 "${_fd}" _b3o)
k_bild(B3 b3_mit b3_ohne 131 75 145 87 22)
# B3K "gut sichtbar" bei Skala 1 (Nachbesserung 1): RE2 Small Key (ROOM10F0.RDT MD1 @0x001650, flach @0x00706) gemessen
# 2026-10-16 27 Pixel, Sum|dL| 1627 (Cabin Key vorher 14 / 502), Parking Lot Key 55 / 3428 -> 47 %.
k_kontrast(B3K b3_mit b3_ohne 131 75 145 87 22 1300 _dl_prison1)
k_verhaeltnis(B3V "${_dl_prison1}" "${_dl_park1}" 38)
# B4: der Prison Key in der Fenstergroesse, in der gespielt wird — RE15_WINDOW_SCALE=3 (960x720, 3D in Fensteraufloesung,
# render_pc.c Fenster = groesste ganze Skala <= 90 % des Bildschirms). Fenster = B3-Fenster x 3. Gemessen 2026-10-16
# (Small Key): 217 Pixel, Sum|dL| 11899, Huelle x 397..432 y 236..250, aussen 0 (Cabin Key vorher 112 / 3193).
k_lauf(b4_mit  10F1 "10F1;0;4000;1024;cut:5;elza" "W3" 120 "${_fd};RE15_WINDOW_SCALE=3" _b4)
k_lauf(b4_ohne 10F1 "10F1;0;4000;1024;cut:5;elza;flag:9:83" "W3" 120 "${_fd};RE15_WINDOW_SCALE=3" _b4o)
k_bild(B4 b4_mit b4_ohne 393 225 437 263 180)
k_kontrast(B4K b4_mit b4_ohne 393 225 437 263 180 9500 _dl_prison3)
# B5: Parking Lot Key bei Skala 3 = Bezug fuer B4 (gemessen 491 Pixel, Sum|dL| 28852; Prison Key 11899 -> 41 %).
k_lauf(b5_mit  10F0 "10F0;0;4000;1024;cut:5;flag:9:71;flag:3:94" "W3" 120 "${_fd};RE15_WINDOW_SCALE=3" _b5)
k_lauf(b5_ohne 10F0 "10F0;0;4000;1024;cut:5;flag:9:71;flag:3:94;flag:9:82" "W3" 120 "${_fd};RE15_WINDOW_SCALE=3" _b5o)
k_kontrast(B5K b5_mit b5_ohne 393 225 437 263 400 23000 _dl_park3)
k_verhaeltnis(B4V "${_dl_prison3}" "${_dl_park3}" 33)

# --- A: Aufnahme ROOM1030 vor dem Pfeiler; dritter Druck = Monitor-Satz aus Slot 20 -------------------------------------
# Skript (30 Hz ab Spielbild 60): A@90 Modal, A@198 Ja, A@321 an derselben Stelle.
k_lauf(a 1030 "1030;-4300;-17904;0;cut:0" "W1,A0.1,W3.5,A0.1,W4,A0.1,W6" 520 "" _a)
pruef(A _a "[feuerloescher] Aufnahme Item 0x31 (9,0x86) -> Set(3,0x85)=1")
pruef(A _a "[msgtext] room=1030 id=14 \"Seems like even the emergency broadcast has gone offline...\"")

# --- K1/K2: Aufnahme an der Pinnwand; danach Satz msg 4 aus Slot 15 -----------------------------------------------------
k_lauf(k1 10F0 "10F0;1300;-2900;1024;cut:5;flag:9:71;flag:3:94" "W1,A0.1,W3.5,A0.1,W4,A0.1,W6" 520 "" _k1)
pruef(K1 _k1 "[schluessel] PARKING LOT KEY genommen: Item 0x45, Bit (9,82)")
pruef(K1 _k1 "[msgtext] room=10f0 id=4 \"Several files from past cases.Nothing useful here.\"")
pruef_nicht(K1 _k1 "Flag (4,0xEC) = 1")
k_lauf(k2 10F1 "10F1;1300;-2900;1024;cut:5;elza" "W1,A0.1,W3.5,A0.1,W4,A0.1,W6" 520 "" _k2)
pruef(K2 _k2 "[schluessel] PRISON KEY genommen: Item 0x42, Bit (9,83)")
pruef(K2 _k2 "Prison Key genommen -> Flag (4,0xEC) = 1")
pruef(K2 _k2 "[msgtext] room=10f1 id=4 \"Several files from past cases.Nothing useful here.\"")

# --- V1-V3: Sicht in den anderen Cuts (RE15_FORCE_CUT) -------------------------------------------------------------------
k_lauf(v1_mit  1030 "1030;-11500;-19500;2048;cut:0;flag:4:0x0f" "W3" 120 "${_fd};RE15_FORCE_CUT=10" _v1)
k_lauf(v1_ohne 1030 "1030;-11500;-19500;2048;cut:0;flag:4:0x0f;flag:9:0x86;flag:3:0x85" "W3" 120 "${_fd};RE15_FORCE_CUT=10" _v1o)
pruef(V1 _v1 "[feuerloescher] ROOM1030: Loescher an der Wand obj 5")
k_bild(V1 v1_mit v1_ohne 0 0 319 239 0)
k_lauf(v2_mit  10F0 "10F0;0;4000;1024;cut:5;flag:9:71;flag:3:94" "W3" 120 "${_fd};RE15_FORCE_CUT=7" _v2)
k_lauf(v2_ohne 10F0 "10F0;0;4000;1024;cut:5;flag:9:71;flag:3:94;flag:9:82" "W3" 120 "${_fd};RE15_FORCE_CUT=7" _v2o)
k_bild(V2 v2_mit v2_ohne 0 0 319 239 0)
k_lauf(v3_mit  10F0 "10F0;0;4000;1024;cut:5;flag:9:71;flag:3:94" "W3" 120 "${_fd};RE15_FORCE_CUT=6" _v3)
k_lauf(v3_ohne 10F0 "10F0;0;4000;1024;cut:5;flag:9:71;flag:3:94;flag:9:82" "W3" 120 "${_fd};RE15_FORCE_CUT=6" _v3o)
k_bild(V3 v3_mit v3_ohne 76 62 103 81 50)
# V4/V5: dieselbe Sicht bei Elza (ROOM10F1, Prison Key) — Cut 7 (Buecherregal, keine Masken) nicht gezeichnet, Cut 6
# (zweite Kamera auf die Pinnwand) gezeichnet. Gemessen 2026-10-16: Cut 7 0 Pixel, Cut 6 mit dem Small Key (Nachbesserung 1)
# 56 Pixel x 81..98 y 69..75, Sum|dL| 2690 (Cabin Key vorher 29 Pixel).
k_lauf(v4_mit  10F1 "10F1;0;4000;1024;cut:5;elza" "W3" 120 "${_fd};RE15_FORCE_CUT=7" _v4)
k_lauf(v4_ohne 10F1 "10F1;0;4000;1024;cut:5;elza;flag:9:83" "W3" 120 "${_fd};RE15_FORCE_CUT=7" _v4o)
pruef(V4 _v4 "[schluessel] ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83)) Prop obj 4")
k_bild(V4 v4_mit v4_ohne 0 0 319 239 0)
k_lauf(v5_mit  10F1 "10F1;0;4000;1024;cut:5;elza" "W3" 120 "${_fd};RE15_FORCE_CUT=6" _v5)
k_lauf(v5_ohne 10F1 "10F1;0;4000;1024;cut:5;elza;flag:9:83" "W3" 120 "${_fd};RE15_FORCE_CUT=6" _v5o)
k_bild(V5 v5_mit v5_ohne 76 62 103 81 45)

# --- E: Elza ROOM1031 (kein Loescher) und ROOM1091 (der leere am Boden, Runde 37) ----------------------------------------
k_lauf(e1 1031 "1031;-11500;-19500;2048;cut:0;elza" "W2" 90 "" _e1)
pruef_nicht(E _e1 "[feuerloescher] ROOM1031")
pruef_nicht(E _e1 "oid=0x05")
pruef_nicht(E _e1 "[itemorte] ROOM1031")
k_lauf(e2 1091 "1091;1880;-1300;0;y:-1800;cut:6;elza;flag:7:0xf8;flag:7:0xf9;flag:7:0xfa;flag:7:0xfb" "W2" 90 "" _e2)
pruef(E _e2 "[feuerloescher] ROOM1091: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300)")

# --- O: ROOM1050 (Leon) ohne Loescher --------------------------------------------------------------------------------------
k_lauf(o 1050 "1050;18900;-20800;0;cut:6" "W2" 90 "" _o)
pruef_nicht(O _o "[feuerloescher] ROOM1050")
pruef_nicht(O _o "oid=0x05")

file(REMOVE "${_exe_kopie}")
if(_fehler)
    string(REPLACE ";" "\n  " _f "${_fehler}")
    message(FATAL_ERROR "r38_itemorte: ${_f}")
endif()
message(STATUS "r38_itemorte: alle Laeufe gruen")
