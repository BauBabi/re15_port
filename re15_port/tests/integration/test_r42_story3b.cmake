# =============================================================================
# Runde 42 "story" (v0.8.31), Stufe STAGE3b (30E0, 3070, 3080) - DURCHLAEUFE in der echten exe (RE15_WEGPLAN: Gehen, Blick, Aktionstaste,
# Zielen/Feuern wie der Spieler; Raumwechsel ueber die Tueren des Spiels). Konstanten/Belege include/re15_story3b_r42.h, Code
# engine/src/story3b_r42.c, Birkin-Clipende enemy_ai_common.c re15_birkin_anim_szene. PORT-STORY nach dem Konzept (3.2 L8-L10,
# L-11/L-12, F-08; 5 T-03/T-04, F-09) und dem Nutzerbefund 15.11. (30E0: letzte Marvin-Szene endet zu frueh).
#
# TEIL leon   Zustand = nach dem Wiedersehen in 3060 (Stufe STAGE3a: Bits der Kette integration_r42_story3a_kette + 9:124 Uebergabe,
#             Green Keycard/Super Redhawk/Red Master Keycard im Inventar). Weg 3060 -> 3010 -> Kartentuer -> 30E0 (West-Ring) -> 3070
#             (Birkin-Auftritt, Kampf) -> Zeile msg 6 -> 30E0 (EIN Besuch: Ada, Glasbruch, msg 16/17, Marvin tritt ein, msg 9..13, Ende
#             erst nach Untertitel UND Stimme) -> 3070 (Aufzug mit Ada und Marvin) -> 3080 (Birkins Hand, zwei Stoesse) -> 4000.
#             Mit Ton (SDL_AUDIODRIVER=dummy): die Aufnahmen synchro/STAGE3/room30E0/main*.wav laufen, msg 16/17 und 3070 msg 6 haben
#             keine Aufnahme (Untertitel mit Textdauer). main03/10/11/13 sind NEUAUF (VERTRAG 2.3): bis zur Neuaufnahme spielt die
#             vorhandene Aufnahme unter dem neuen Text weiter, wie 3020 msg 0..2 (Nachbesserung 2, R2-1: keine Stummschaltung). Mit der
#             echten main13.wav (ca. 270 Bilder) wartet die letzte Marvin-Szene nach der Sleep-Kette (152 Bilder) auf Untertitel UND
#             Stimme und endet erst danach (R2-3, Befund 15.11.). Vor 3070 msg 6 poltert es viermal im Lager (B3); nach dem Auftritt
#             kaempft Birkin ohne Szenen-Staerke, der Tod beginnt mit a3 0x200 = Gewicht 3584 (B1).
# TEIL elza   30E1 -> 3071 (Birkin-Auftritt Sub 10, Kampf, Flucht sub17) -> 30E1 (John und Sherry, Original) -> 3071 (Aufzug) -> 3081
#             (Hand) -> 4001. Ausser T-04 im Aufzug darf sich nichts aendern: keine Port-Szene in 30E1/3071.
# Vorgesetzt (der Wegplan-Bot kann es nicht spielen, gemessen Lauf d2: Magazin leer ohne Nachladen, Spieler gefressen): Birkins Tod
# ueber den Messhaken RE15_S3B_BIRKIN_TOT (Leon 600, Elza 60 Kampfbilder - Elza wurde in Lauf elza_vor nach ~600 gefressen; danach Tod wie nach einem Treffer; Zusammenbruch, (5,0x1c) @0x8011a6b8,
# sub01 @0x03496 und alles danach laufen echt) und RE15_R39_MESS_SPIELER_HP=100 (Treffer bis dahin ohne Tod, Muster
# integration_r41_birkin_pistole). 3010-Zombies wie Stufe STAGE3a (Kill-Bits 7:64..7:70).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=<leon|elza> -P test_r42_story3b.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r42_story3b: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r42_story3b: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL leon)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}_${TEIL}")
file(MAKE_DIRECTORY "${WORKDIR}")
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_name "${RE15_PC_EXE}" NAME)
if(_exe_name MATCHES "\\.exe$")
    set(_exe_kopie "${_exe_dir}/re15_pc_r42s3b_${_lauf_id}.exe")
else()
    set(_exe_kopie "${_exe_dir}/re15_pc_r42s3b_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_fehler "")
macro(fail _t)
    list(APPEND _fehler "${_t}")
    message(STATUS "FAIL r42_story3b[${TEIL}]: ${_t}")
endmacro()
macro(ok _t)
    message(STATUS "ok   r42_story3b[${TEIL}]: ${_t}")
endmacro()
macro(pruefe _text _muster)
    string(FIND "${_dbg}" "${_muster}" _p)
    if(_p LESS 0)
        fail("${_text} (fehlt: '${_muster}')")
    else()
        ok("${_text}")
    endif()
endmacro()
macro(pruefe_nicht _text _muster)
    string(FIND "${_dbg}" "${_muster}" _p)
    if(_p LESS 0)
        ok("${_text}")
    else()
        fail("${_text} (gefunden: '${_muster}')")
    endif()
endmacro()
macro(pruefe_re _text _regex)
    if(_dbg MATCHES "${_regex}")
        ok("${_text}")
    else()
        fail("${_text} (fehlt: /${_regex}/)")
    endif()
endmacro()
function(_fertig)
    file(REMOVE "${_exe_kopie}")
    list(LENGTH _fehler _nf)
    if(_nf GREATER 0)
        message(FATAL_ERROR "r42_story3b[${TEIL}]: ${_nf} Fehler (${WORKDIR})")
    endif()
    message(STATUS "r42_story3b[${TEIL}]: alle Pruefungen ok (${WORKDIR})")
endfunction()

# Pose-Messschiene (skeleton_common.c re15_pose_messzeile): Zeilen "F.. R.. s.. t.. m.. af.. kf.. fr.. ra.. wp.." je gezeichneter Pose.
# Prueft das Wechselbild eines Clips: Bild fc-1 von <clip_a> gezeichnet, im Folgebild <clip_b> Bild 1 mit Rate <rate>.
function(_wechsel _datei _typ _clip_a _letztes _clip_b _rate _erg)
    file(STRINGS "${_datei}" _z REGEX " t${_typ} m(${_clip_a}|${_clip_b}) ")
    set(_alt "")
    set(_n 0)
    foreach(_l ${_z})
        if(_alt MATCHES " m${_clip_a} af${_letztes} " AND _l MATCHES " m${_clip_b} af1 .* ra${_rate} ")
            math(EXPR _n "${_n} + 1")
        endif()
        set(_alt "${_l}")
    endforeach()
    set(${_erg} ${_n} PARENT_SCOPE)
endfunction()
function(_umlauf _datei _typ _clip_a _clip_b _erg)
    file(STRINGS "${_datei}" _z REGEX " t${_typ} m(${_clip_a}|${_clip_b}) ")
    set(_alt "")
    set(_n 0)
    foreach(_l ${_z})
        if(_alt MATCHES " m${_clip_a} af0 kf" AND _l MATCHES " m${_clip_b} af1 ")
            math(EXPR _n "${_n} + 1")
        endif()
        set(_alt "${_l}")
    endforeach()
    set(${_erg} ${_n} PARENT_SCOPE)
endfunction()

set(_basis RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_EQUIP=3
           RE15_S3B_BIRKIN_TOT=600 RE15_R39_MESS_SPIELER_HP=100 RE15_WEGPLAN_LOG=wegplan.txt RE15_STATE_LOG=state.log
           RE15_POSE_LOG=pose.txt RE15_STIMME_LOG=1 RE15_SE_DEBUG=1)

# 3070: Birkin-Szene (Aot Platz 2 @0x033E2), Kampf bis (bank,bit), Rueckweg nach 30E0 nordlich am Block SCA 3 vorbei (dort liegt
# Birkin). Wege aus der SCA (Planer scratchpad story/s3a/weg.py, Band 0, Radius 450).
set(_weg_3070_kampf "WARTE 30
GEHE -11955 -14390 250
GEHE -11055 -9440 250
GEHE -11055 -5540 250
GEHE -20355 -5390 250
GEHE -25155 -8690 250
BLICK 2048
AKTION
JAGE @BANK@ @BIT@ 3000 3000 9000
WARTE 300
GEHE -22005 -3440 250
GEHE -16005 -3440 250
GEHE -11205 -5390 250
GEHE -11055 -5690 250
GEHE -11055 -9740 250
GEHE -12405 -17840 250
GEHE -12400 -18250 100
BLICK 1024
WARTE 10
AKTION
")
# 30E0/30E1 West-Ring -> Tuer Platz 1 (main00 @0x00A12 -> 3070), dann 3070 -> Aufzug (Aot Platz 3 @0x03464) -> 3080 -> 4000.
set(_weg_zur_3070 "GEHE -25700 -22300 250
BLICK 3072
WARTE 10
AKTION
")
set(_weg_aufzug "WARTE 30
GEHE -11055 -8840 250
GEHE -11055 -5540 250
GEHE -11205 -5390 250
GEHE -19605 -4940 250
AKTION
")

if(TEIL STREQUAL "leon")
    string(REPLACE "@BANK@ @BIT@" "4 194" _kampf "${_weg_3070_kampf}")
    file(WRITE "${WORKDIR}/plan.txt" "RAUM 3060
WARTE 30
GEHE -26606 -22432 250
GEHE -26900 -22400 650
BLICK 2048
AKTION
RAUM 3010
WARTE 30
RAEUME 6000 2500
GEHE -19040 9394 250
RAEUME 6000 2500
GEHE -21890 3844 250
RAEUME 6000 2500
GEHE -22040 -16706 250
RAEUME 6000 2500
GEHE -21500 -16900 100
RAEUME 6000 2500
BLICK 1024
AKTION
WARTE 90
GEHE -25976 -19836 250
RAEUME 6000 2500
GEHE -26750 -19800 800
RAEUME 6000 2500
BLICK 2048
TIPPE 3 60 600
TIPPE 9 255 200
RAUM 30E0
WARTE 30
${_weg_zur_3070}RAUM 3070
${_kampf}RAUM 30E0
TIPPE 3 180 3000 1
TIPPE 9 255 900 1
${_weg_zur_3070}RAUM 3070
${_weg_aufzug}RAUM 3080
WARTE 30
RAUM 4000
WARTE 60
ENDE
")
    re15_start_spiel(_rv 2400 ${_basis} SDL_AUDIODRIVER=dummy RE15_GOTO_ROOM=3060 "RE15_GIVE=21:200,54:1,7:6,70:1"
        "RE15_SET_FLAG=9:90,9:111,9:112,9:113,9:118,9:119,3:245,3:206,3:151,9:121,9:120,7:64,7:65,7:66,7:67,7:68,7:69,7:70,7:113,7:114,3:176,3:179,9:124,9:85,9:35,9:180,9:92,9:123,3:59,4:196,3:160,7:80,7:81,7:82,3:82,3:93,3:177"
        "RE15_WEGPLAN=${WORKDIR}/plan.txt" "RE15_EXIT_AT=60#4000" "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        fail("kein debug.log (exit=${_rv})")
        _fertig()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    # Weg
    pruefe("Kartentuer 3010 -> 30E0 (West-Ring)" "room=3010 id=1 ")
    pruefe("3070: Birkin-Szene (sub02)" "[msg] room=3070")
    # L-11 / F-08
    pruefe("F-08: 3070 nach Birkins Tod msg 6" "ROOM3070: (4,194) im Besuch gesetzt (sub01 @0x03496, Birkins Tod) -> msg 6")
    pruefe("F-08: Wortlaut msg 6" "[msgtext] room=3070 id=6 \"Leon: That came from the storage room... Ada?\"")
    # NB1 B3: vier Holzschlaege (Port-Bank 0x3E Satz 0) zwischen dem Start der Zeile und msg 6
    string(FIND "${_dbg}" "ROOM3070: (4,194) im Besuch gesetzt" _p0)
    string(FIND "${_dbg}" "[msgtext] room=3070 id=6 " _p6)
    if(_p0 GREATER -1 AND _p6 GREATER _p0)
        math(EXPR _l "${_p6} - ${_p0}")
        string(SUBSTRING "${_dbg}" ${_p0} ${_l} _vor6)
        string(REGEX MATCHALL "RE2 DOOR0F Klopfen" _kl "${_vor6}")
        list(LENGTH _kl _nkl)
        if(_nkl EQUAL 4)
            ok("B3: vier Holzschlaege aus dem Lager vor msg 6")
        else()
            fail("B3: ${_nkl} Holzschlaege vor msg 6 (soll 4)")
        endif()
    else()
        fail("B3: Zeilenstart/msg 6 nicht gefunden")
    endif()
    # L8-L10 ein Besuch
    pruefe("L10: 30E0 in einem Besuch (Programm)" "ROOM30E0: ein Besuch = sub02 + Glasbruch + Zeilen 16/17 + Marvin tritt ein + sub03 + Warten")
    pruefe_re("L8: Zeilenfolge 0..8, 16, 17, 9..13 in 30E0"
        "id=8 savepoint=0.*id=16 savepoint=0.*id=17 savepoint=0.*id=9 savepoint=0.*id=10 savepoint=0.*id=11 savepoint=0.*id=12 savepoint=0.*id=13 savepoint=0")
    pruefe("L8: msg 16" "[msgtext] room=30e0 id=16 \"Leon: That came from the lobby... Marvin!\"")
    pruefe("L10: msg 17" "[msgtext] room=30e0 id=17 \"Ada: Someone's coming.\"")
    pruefe("L8: msg 10 Neutext" "[msgtext] room=30e0 id=10 \"Leon: That's a bite... They got you?\"")
    pruefe("L8: msg 11 Neutext" "[msgtext] room=30e0 id=11 \"Marvin: The lobby's gone...! They smashed through the front glass and got inside!\"")
    pruefe("L8: msg 13 Neutext" "[msgtext] room=30e0 id=13 \"Marvin: There's more of them than I thought... If they've come this far, the city's finished.\"")
    pruefe("L7: msg 3 (Stufe STAGE3a)" "[msgtext] room=30e0 id=3 \"Leon: He's back in the lobby. Some dogs got his leg... he can barely walk.\"")
    pruefe("Ton: msg 16 ohne Aufnahme -> Untertitel mit Textdauer" "ROOM30E0 msg 16: keine Aufnahme")
    # Nachbesserung 2 (R2-1): die vorhandenen Aufnahmen spielen unter dem neuen Text (keine Stummschaltung)
    pruefe_nicht("R2-1: keine Stummschaltung alter Aufnahmen" "ist noch die alte Aufnahme")
    foreach(_m 3 10 11 13)
        pruefe("R2-1: msg ${_m} spielt die Aufnahme" "[voice] Start ROOM30E0 Id ${_m} ")
    endforeach()
    pruefe("R2-1: main13.wav geladen" "[voice] clip loaded: main13.wav")
    # Nutzerbefund 15.11. / R2-3 (streng): nach der Sleep-Kette von msg 13 laeuft main13.wav noch; die Szene wartet (> 0 Bilder),
    # das Warten endet erst mit Zeile UND Stimme, das Szenenende (Riegel (2,7) los) kommt danach.
    set(_r_kette "ROOM30E0: Sleep-Kette nach msg 13 durch [(]Bild ([0-9]+) der Szene[)] - Zeile aktiv [01], Stimme 1 [(]Rest ([1-9][0-9]*) Bilder[)]")
    set(_r_ende  "ROOM30E0: Zeile und Stimme zu Ende [(]Bild ([0-9]+) der Szene, ([1-9][0-9]*) Bilder gewartet[)]")
    set(_r_fertig "ROOM30E0: Szene fertig nach ([0-9]+) Bildern [(]Riegel [(]2,7[)] los[)], davon ([1-9][0-9]*) Bilder auf Zeile/Stimme gewartet")
    if(_dbg MATCHES "${_r_kette}")
        set(_k_bild ${CMAKE_MATCH_1})
        set(_k_rest ${CMAKE_MATCH_2})
        ok("R2-3: nach der Sleep-Kette (Szenenbild ${_k_bild}) laeuft main13.wav noch (Rest ${_k_rest} Bilder)")
    else()
        fail("R2-3: Sleep-Kette nach msg 13 mit laufender Stimme (Rest > 0) fehlt (/${_r_kette}/)")
    endif()
    if(_dbg MATCHES "${_r_ende}")
        set(_e_bild ${CMAKE_MATCH_1})
        set(_e_gew ${CMAKE_MATCH_2})
        ok("R2-3: Zeile und Stimme zu Ende bei Szenenbild ${_e_bild}, ${_e_gew} Bilder gewartet")
    else()
        fail("R2-3: Ende des Wartens (Zeile und Stimme zu Ende, > 0 Bilder) fehlt (/${_r_ende}/)")
    endif()
    if(_dbg MATCHES "${_r_fertig}")
        set(_f_bild ${CMAKE_MATCH_1})
        set(_f_gew ${CMAKE_MATCH_2})
        ok("R2-3: Szene fertig nach ${_f_bild} Bildern, ${_f_gew} gewartet")
    else()
        fail("R2-3: Szenenende mit > 0 Wartebildern fehlt (/${_r_fertig}/)")
    endif()
    if(DEFINED _k_bild AND DEFINED _e_bild AND DEFINED _f_bild)
        if(_e_bild GREATER _k_bild AND _f_bild GREATER _e_bild AND _f_gew EQUAL _e_gew)
            ok("R2-3: Reihenfolge Sleep-Kette ${_k_bild} < Stimmende ${_e_bild} < Szenenende ${_f_bild}")
        else()
            fail("R2-3: Reihenfolge falsch (Sleep-Kette ${_k_bild}, Stimmende ${_e_bild}, Szenenende ${_f_bild}, gewartet ${_e_gew}/${_f_gew})")
        endif()
    endif()
    # weiter wie im Original
    pruefe("3070 Aufzug (sub03, msg 0)" "room=3070 id=0 ")
    pruefe("3080 erreicht" "PC loaded room3080.rdt")
    pruefe("4000 erreicht" "EXIT_AT: Bild 60 in Raum 4000")
    # T-03 / T-04 an der Pose-Messschiene
    if(EXISTS "${WORKDIR}/pose.txt")
        _wechsel("${WORKDIR}/pose.txt" "30" "16" "58" "13" "256" _n)
        if(_n GREATER 0)
            ok("T-03: 3070 Wechselbild Clip 0x10 Bild 58 (letztes) -> Clip 0xd Bild 1 mit Rate 0x100")
        else()
            fail("T-03: Wechsel Clip 16 af58 -> Clip 13 af1 ra256 fehlt")
        endif()
        _umlauf("${WORKDIR}/pose.txt" "30" "16" "13" _u)
        if(_u EQUAL 0)
            ok("T-03: kein Umlaufbild (Clip 0x10 Bild 0) vor Clip 0xd")
        else()
            fail("T-03: Umlaufbild Clip 16 af0 vor Clip 13 (${_u}x)")
        endif()
        # NB1 B1: nach dem Auftritt (Clips 0x10/0xd) kaempft Birkin in 3070 ohne Szenen-Staerke (Rate 0 wie vor T-03); Tod Clip 9
        # beginnt mit +0x8f 7 (@0x8011a654) x a3 0x200 (@0x8011a764) = wp 3584
        file(STRINGS "${WORKDIR}/pose.txt" _k3070 REGEX " R3070 s[0-9]+ t30 m([0-9]|1[0-24578-9]|[2-9][0-9]) .* ra256 ")
        list(LENGTH _k3070 _nk)
        if(_nk EQUAL 0)
            ok("B1: 3070-Kampf ohne stehengebliebene Rate 0x100")
        else()
            list(GET _k3070 0 _k0)
            fail("B1: ${_nk} Kampfposen mit Rate 0x100 (erste: ${_k0})")
        endif()
        file(STRINGS "${WORKDIR}/pose.txt" _tod REGEX " R3070 s[0-9]+ t30 m9 af0 kf[0-9]+ fr7 ra512 wp3584 ")
        list(LENGTH _tod _nt)
        if(_nt GREATER 0)
            ok("B1: Tod Clip 9 mit Startgewicht 7 x 0x200 = 3584")
        else()
            fail("B1: Tod Clip 9 fr7 ra512 wp3584 fehlt")
        endif()
        _wechsel("${WORKDIR}/pose.txt" "36" "4" "9" "4" "1024" _n4)
        if(_n4 EQUAL 1)
            ok("T-04: 3080 zweiter Stoss aus dem gehaltenen letzten Bild (Clip 4 af9 -> af1, Rate 0x400), einmal")
        else()
            fail("T-04: Uebergang Clip 4 af9 -> af1 ra1024 ${_n4}x (soll 1)")
        endif()
    else()
        fail("keine pose.txt")
    endif()
    _fertig()
elseif(TEIL STREQUAL "elza")
    string(REPLACE "@BANK@ @BIT@" "3 181" _kampf "${_weg_3070_kampf}")
    file(WRITE "${WORKDIR}/plan.txt" "RAUM 30E1
WARTE 30
${_weg_zur_3070}RAUM 3071
${_kampf}RAUM 30E1
TIPPE 3 180 3000 1
TIPPE 9 255 900 1
${_weg_zur_3070}RAUM 3071
${_weg_aufzug}RAUM 3081
WARTE 30
RAUM 4001
WARTE 60
ENDE
")
    re15_start_spiel(_rv 2400 ${_basis} RE15_S3B_BIRKIN_TOT=60 RE15_NOAUDIO=1 RE15_GOTO_ROOM=30E1 RE15_PSELECT_AUTO_SWITCH=1 "RE15_GIVE=21:200"
        "RE15_WEGPLAN=${WORKDIR}/plan.txt" "RE15_EXIT_AT=60#4001" "${_exe_kopie}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        fail("kein debug.log (exit=${_rv})")
        _fertig()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    pruefe("3071 erreicht" "PC loaded room3071.rdt")
    pruefe("30E1: Johns und Sherrys Szene (Original sub02)" "[msg] room=30e1 id=0 ")
    pruefe_nicht("30E1: kein Port-Programm" "ROOM30E0: ein Besuch")
    if(_dbg MATCHES "\\[story3b\\] F[0-9]+ ROOM30E1|ROOM3071: \\(4,194\\)")
        fail("30E1/3071: Port-Story-Zeilen in Elzas Raeumen")
    else()
        ok("30E1/3071: keine Port-Story (nur der Messhaken)")
    endif()
    pruefe("3081 erreicht" "PC loaded room3081.rdt")
    pruefe("4001 erreicht" "EXIT_AT: Bild 60 in Raum 4001")
    _fertig()
else()
    fail("unbekannter TEIL ${TEIL}")
    _fertig()
endif()
