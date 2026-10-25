# =============================================================================
# Runde 41 Spur B "re2gator" — der RE2-Alligator in der ECHTEN exe (ROOM2090 per Sprung).
#
# Dossier analysis/befunde_runde41/B_re2gator.md. Die Unit-Riegel unit_r41_re2gator_* messen die
# Mechanik am Boss-Tick; dieser Riegel prueft, was nur die exe zeigt: Spawn + Boss-Tick im Spiel,
# RE15_GATOR_KANISTER-Haken (Kanister gesetzt, Zeichner laedt das Modell), RE2-Zeilen in gator_boss.log,
# HP-Sockel (kein Tod durch die Zeit), Schuetteln ohne Absturz des Hintergrund-Blits.
#
# Nachbesserung 2 (Abnahme 1, M1/M2): ZWEI Laeufe.
#   Lauf "nische" (Leon nach dem Sprung in der Tuernische, GB_TEST=1): RE2-Intro mit gewaehlter Startlage
#     (gr2_intro_lage, Entscheid B7) - Kopf/Rumpf in keinem Bild in der Wand (Messzeile "INTRO ende ...
#     wand_draussen_max=0"), Kamera-Programm DURCHBRUCH/SEITE/ANGRIFF/FREI und FINISHER (RE2 @0x80100fbc-ff4), danach
#     haelt der Finisher-Schnitt bis zum Ende des Raums (Entscheid B6: kein Kamerawechsel, keine RE1.5-Todeskamera).
#     Gemessen 2026-10-24 (Nachbesserung 2, Lauf A): Intro F6-F280 (Start (-7345,-13277) Gier 576 -> End-Gier 1600
#     auf Leon), Biss F403 -> FINISHER Cut 6, Game Over ~F677 -> EXIT_AT 650 liegt davor.
#   Lauf "dash" (Leon im Suedbecken (-4000,-24000) im Bild der Eintrittskamera Cut 0, Messhaken
#     RE15_R39_MESS_SPIELER_HP=100): der Intro endet im Nordbecken, die Verfolgung kommt durch den Westkanal in Leons
#     Zone -> RE2-Dash (Clip 2 -> 3, gr2_dash_planen: Vordrehung 1139 -> 624 am Suedausgang) mit Putz 6 x 9 um Leon,
#     Kopf/Rumpf im Wasser ("DASH ende ... wand_draussen_max=0"). Gemessen 2026-10-24 (Lauf D2): Dash ab (-6052,-18113),
#     End-Gier 1648 = Peilung zu Leon (Abw. 5), Kopf x min -8642, Putz 54, gezeichnet 27..98 Quads/Bild.
#
# Nachbesserung 3 (Abnahme 2 M1, Orchestrator-Entscheid B9): DRITTER Lauf "natuerlich" = Abnahme-2-Lauf N (ohne GB_TEST,
#   Leon geht aus der Tuernische im Westkanal nach Norden, Aggro bei dist < 9000 zum Lauerplatz). RE2-Form: der Gator ist bis
#   zum Durchbruch UNTERGETAUCHT (Sce_em_set y=+3000 @RDT 0x25A0, Pos_set erst im Intro @SCD 0x0736). Gemessen am NPC-
#   Zeichner (debug.log "[gator-re2] Sicht ...", gator_re2_pc.c): erste Sicht-Zeile "untergetaucht", Auftauchen genau mit dem
#   Intro-Start (Clip 6 Bild 0, = "RE2 AUFTAUCHEN F<n>" in gator_boss.log), erstes gezeichnetes Bild im Intro, kein
#   erneutes Untertauchen, groesster Bildschritt danach <= 340 = groesster authored Wurzelschritt der Clips 6/3/0/1 x Skala
#   2731 (unit_r41_re2gator_lauer: Clip 6 340, 3 72, 0 104, 1 259). Gemessen 2026-10-24 (B41nb3/mess/N): untergetaucht
#   F1-F200, Auftauchen F201 auf (5085,-19880) Clip 6 Bild 0, groesster Schritt 338 (Clip 6 Bild 28); Bilder F1-F201
#   pixelgleich mit einem Lauf ohne Gator (RE15_KEIN_GATOR=1).
#
# Nachbesserung 4 (Abnahme 3 M1/M2): VIERTER Lauf "kanister" (Abnahme-Rezept C3: Leon per Autopilot im Suedbecken, Kanister
#   (-1500,-23400), bis F1400) - der Gator nimmt den Kanister auf und traegt ihn; Messzeile des PC-Zeichners
#   "[gator-re2] Kanister-Sitz ..." (Part6(gezeichnetes Bild) x Lokallage gegen die gezeichnete Kanister-Lage, RE2: Kind der
#   Part-6-Weltmatrix @0x801018d0-e0) muss in JEDEM angehaengten Bild 0 sein. FUENFTER Lauf "belagerung" (Abnahme-Lauf E1: echter
#   Tuerweg 20A0 -> 2090, Leon oben auf der Rampe, GUARD an der Ostwand). In ALLEN Laeufen: keine Zeile "GIER-SPRUNG" in
#   gator_boss.log (Gier-Delta > 0x60 zwischen zwei CHASE/GUARD-Bildern ausser dem RE2-Knick +1024 @0x80100938-48 /
#   @0x80100eb0-c0; RE2 hat in Sub 1/2 keinen Gier-Schreiber). Gemessen 2026-10-24 (B41nb4/mess/C3n, E1n): Kanister angehaengt
#   F1119, "Kanister-Sitz F1388: angehaengt 270 Bilder, Abweichung max 0"; "GIER-WACHE bis F581: 300 Bildpaare CHASE/GUARD, max
#   |Gier-Delta| 64 (ohne +1024), Spruenge > 0x60: 0, Befreiungen 0" (vorher E1 F426/458/490/522/546 +-1944..1984).
#
# Nachbesserung 5 (Abnahme 4 M1): Lauf "belagerung" laeuft bis F890 (nach dem toedlichen GUARD-Hochbiss F627 und dem ganzen
#   RE2-Finisher P1-P3 bis F881) und prueft die Summenzeile "[gator-re2] Maul-Sitz Ende ..." (r41b_maul_sitz): 254 Bilder (14/120/
#   120), Kopf an der Anker-Lage, Leon am gezeichneten Kiefer wie die authored Bahn (<= 8), Kopf/Leon im Bild der Finisher-Kamera
#   (Leon nur oben hinaus beim Wurf), alles im Wasser; Lauf "nische" (CHASE-Tod) prueft dieselbe Messzeile bis EXIT_AT.
#
# Nachbesserung 6 (Abnahme 5 M1-M3): Lauf "belagerung" laeuft durch das Game Over bis in den neu gestarteten Raum 1240
#   (RE15_EXIT_AT=251#1240: der Debug-Sprung @250 setzt den Bildzaehler im ersten 1240 zurueck, also endet der Lauf erst nach dem
#   Game Over). Verlangt: (a) state.log - vom ersten Bild mit Opfer-Modus (gr=1, toedlicher Biss) bis zum letzten Bild vor dem
#   Raumwechsel (Bildzaehler faellt auf 0) steht in JEDEM Bild gr=1 (RE2 P3 @0x801011c4-e4 ohne Ende, Leon-Handler Phase 3
#   @0x80102f24-28 = Spieler +0x1D3 |= 0x80); (b) die Summenzeile "Maul-Sitz Ende" steht im Bild der Game-Over-Uebergabe = letztes
#   2090-Bild, P3 laenger als die alten 120 Bilder, Opfer gehalten und frei von Raum-Objekten in allen Bildern; (c) die Finisher-Kamera
#   ist nicht die Kamera des Biss-Bildes (Abnahme 5 M2). Lauf "nische" prueft (c) ebenso.
#
# Querbezug Runde 41 Spur F "gatorszene" (Nachbesserung 1, Abnahme F 0 Mangel M4; Datei von Spur B, nur die Umgebung der
#   fuenf Laeufe ergaenzt): (a) ROOM2090 ohne Bank 9 Bit 110 startet die Ankunftsszene (Nutzer-Auftrag "neue Cutscene: Wenn man
#   Room 2090 betritt ..."), und das Tor 20A0 -> 2090 ist bis Bit 113 verschlossen (Nachtrag 21) -> alle Laeufe mit
#   RE15_SET_FLAG=9:110,9:113 (Szene gesehen, Tor aufgeschlossen = der Zustand, in dem dieser Riegel den Kampf misst).
#   (b) Nutzer-Punkt "Entferne die Spinnen aus dem ROOM 2090": im Lauf "nische" trafen Leon vorher zwei Spinnen (state.log
#   Abnahme B 6 Lauf A2: hp 100 -> 80 F57 -> 60 F175, dann Gator 10 F290 -> -40 F397 = FRESSEN beim zweiten Biss F403); ohne
#   Spinnen 100 -> 50 F290 -> 0 F397 -> -50 F520 (FRESSEN erst beim dritten Biss, Finisher-Fenster verfehlt). RE15_R41_HP_AT=175:60
#   (Messhaken Spur D, giftkugel_r41.c, kein Spielverhalten) setzt Leons HP im Bild 175 auf denselben Wert 60 wie die zwei
#   Spinnen-Treffer -> beide Gator-Bisse wie vorher. Die uebrigen vier Laeufe halten Leons HP ohnehin per RE15_R39_MESS_SPIELER_HP.
#   (c) Lauf "kanister": ohne die Spinne im Suedbecken (Satz @0x00AC4 (-2300,-23896), am Autopilot-Weg) nimmt der Gator den
#   Kanister spaeter auf (gemessen Nachbesserung F 1: "RE2 KANISTER angehaengt ... F1227", vorher Abnahme B 3 C3n F1119), bis
#   EXIT_AT 1400 nur 150 angehaengte Bilder -> RE15_EXIT_AT 1400 -> 1500 (+100 = Verschiebung 108 aufgerundet auf die naechsten
#   100); die Schranke >= 200 Bilder bei Abweichung 0 bleibt.
#   (d) Orchestrator-Entscheid F-E1 ("die 2090-Ankunftsszene ersetzt das RE2-Intro ... der Riegel prueft den Auftritt ueber F's
#   Szene (Clip-Folge/Schnitte)"): SECHSTER Lauf "szene" ueber die Leiter ohne Bit 110 - kein RE2-Intro, Szenen-Schnitte
#   DURCHBRUCH -> FINISHER -> Ende, Clip-Folge 6/4/5/11/0/10 am state.log. Die fuenf Laeufe oben pruefen B's Kampf (mit dem
#   RE2-Intro beim WIEDER-Betreten nach der Szene, Bits 110/113) unveraendert.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_re2gator.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_re2gator: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_re2gator: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_basis}/nische" "${_basis}/dash" "${_basis}/natuerlich" "${_basis}/kanister" "${_basis}/belagerung")
# Nachbesserung 4 (M2): keine "GIER-SPRUNG"-Zeile in gator_boss.log; liefert die letzte "GIER-WACHE"-Zeile (oder "keine").
function(r41b_gier_wache _dir _lauf _out)
    if(NOT EXISTS "${_dir}/gator_boss.log")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_re2gator: Lauf ${_lauf} ohne gator_boss.log")
    endif()
    file(STRINGS "${_dir}/gator_boss.log" _spr REGEX "^GIER-SPRUNG ")
    if(_spr)
        file(REMOVE "${_exe_kopie}")
        list(GET _spr 0 _s0)
        message(FATAL_ERROR "r41_re2gator: Lauf ${_lauf} - Gier-Sprung in Verfolgung/Belagerung (Abnahme 3 M2): ${_s0}")
    endif()
    file(STRINGS "${_dir}/gator_boss.log" _gw REGEX "^GIER-WACHE ")
    if(_gw)
        list(GET _gw -1 _g)
        set(${_out} "${_g}" PARENT_SCOPE)
    else()
        set(${_out} "keine" PARENT_SCOPE)
    endif()
endfunction()
# EIGENER EXE-NAME (Muster test_r34n_b_cursor.cmake): parallele Bauten beenden re15_pc.exe per Namen.
# Nachbesserung 1 (Abnahme 0, M5): KEIN file(GLOB)+REMOVE fremder re15_pc_r41b_*.exe mehr - nur die eigene Kopie.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r41b_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Nachbesserung 5 (Abnahme 4 M1): Messzeile "[gator-re2] Maul-Sitz ..." des PC-Zeichners (gator_re2_pc.c) - je Finisher-Bild
# Kopf (Bone 6) der gezeichneten Pose gegen die Anker-Lage (Abweichung), Abstand Leon <-> gezeichneter Kiefer (Part 7) gegen die
# authored Bahn (Kiefer-Abstand), Kopf/Leon im Bild der gezeichneten Kamera (Leon darf nur oben hinaus: Wurf P2), beide im Wasser.
# _ende = 1: die Summenzeile "Maul-Sitz Ende" (Finisher bis P3-Ende gelaufen) wird verlangt, sonst gilt die letzte Zeile.
function(r41b_maul_sitz _dir _lauf _ende _out)
    if(_ende)
        file(STRINGS "${_dir}/debug.log" _ms REGEX "\\[gator-re2\\] Maul-Sitz Ende F[0-9]+: ")
    else()
        file(STRINGS "${_dir}/debug.log" _ms REGEX "\\[gator-re2\\] Maul-Sitz F[0-9]+: ")
    endif()
    if(NOT _ms)
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_re2gator: Lauf ${_lauf} - keine Messzeile Maul-Sitz (Summenzeile verlangt: ${_ende}) - Finisher nicht gelaufen bzw. nicht bis P3-Ende")
    endif()
    list(GET _ms -1 _l)
    if(NOT _l MATCHES "Bilder ([0-9]+) \\(P1 ([0-9]+) P2 ([0-9]+) P3 ([0-9]+)\\), Abweichung max (-?[0-9]+) \\(F[0-9]+\\), Kiefer-Abstand max (-?[0-9]+) ")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_re2gator: Lauf ${_lauf} - Maul-Sitz-Zeile unlesbar: ${_l}")
    endif()
    set(_n ${CMAKE_MATCH_1})
    set(_p1 ${CMAKE_MATCH_2})
    set(_p2 ${CMAKE_MATCH_3})
    set(_p3 ${CMAKE_MATCH_4})
    set(_abw ${CMAKE_MATCH_5})
    set(_kie ${CMAKE_MATCH_6})
    if(NOT _l MATCHES "Kopf im Bild ([0-9]+)/[0-9]+, Leon im Bild ([0-9]+)/[0-9]+ \\+ oben hinaus ([0-9]+) \\(Cut (-?[0-9]+)\\), im Wasser ([0-9]+)/[0-9]+")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_re2gator: Lauf ${_lauf} - Maul-Sitz-Zeile unlesbar: ${_l}")
    endif()
    set(_kopf ${CMAKE_MATCH_1})
    set(_leon ${CMAKE_MATCH_2})
    set(_oben ${CMAKE_MATCH_3})
    set(_cut ${CMAKE_MATCH_4})
    set(_nass ${CMAKE_MATCH_5})
    math(EXPR _leon_sicht "${_leon} + ${_oben}")
    set(_fehl "")
    # Nachbesserung 6: Opfer gehalten (+0x1D3 & 0x80) und frei von Raum-Objekten in jedem Bild
    if(NOT _l MATCHES "Opfer gehalten ([0-9]+)/[0-9]+, Opfer frei ([0-9]+)/[0-9]+")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_re2gator: Lauf ${_lauf} - Maul-Sitz-Zeile ohne 'Opfer gehalten/frei' (Nachbesserung 6): ${_l}")
    endif()
    set(_gehalten ${CMAKE_MATCH_1})
    set(_frei ${CMAKE_MATCH_2})
    if(NOT _gehalten EQUAL _n OR NOT _frei EQUAL _n)
        string(APPEND _fehl " Opfer gehalten ${_gehalten}/${_n}, frei von Raum-Objekten ${_frei}/${_n};")
    endif()
    # P1 = Clip-4-Bild 1..14 bis zum Anker (@0x801010fc-104), P2 = Clip 5 sf 1..120, P3 = Clip 11 ohne Ende (RE2 @0x801011c4-e4)
    if(NOT _p1 EQUAL 14)
        string(APPEND _fehl " P1 ${_p1} statt 14;")
    endif()
    set(_ende_bild -1)
    if(_ende)
        if(NOT _p2 EQUAL 120 OR _p3 LESS 121)
            string(APPEND _fehl " P2/P3 ${_p2}/${_p3} statt 120/>120 (P3 endet nicht mehr nach 120 Bildern);")
        endif()
        if(NOT _l MATCHES "letztes Finisher-Bild F([0-9]+) \\(Game-Over-Uebergabe\\)")
            string(APPEND _fehl " Summenzeile nicht im Bild der Game-Over-Uebergabe;")
        else()
            set(_ende_bild ${CMAKE_MATCH_1})
        endif()
    endif()
    if(_abw GREATER 4 OR _abw LESS 0)
        string(APPEND _fehl " Kopf ${_abw} neben der Anker-Lage;")
    endif()
    if(_kie GREATER 8 OR _kie LESS 0)
        string(APPEND _fehl " Leon ${_kie} neben dem gezeichneten Kiefer (authored Bahn);")
    endif()
    if(NOT _kopf EQUAL _n OR NOT _leon_sicht EQUAL _n OR NOT _nass EQUAL _n)
        string(APPEND _fehl " Kopf im Bild ${_kopf}/${_n}, Leon im Bild/oben ${_leon_sicht}/${_n}, im Wasser ${_nass}/${_n};")
    endif()
    if(_cut LESS 0)
        string(APPEND _fehl " keine Kamera;")
    endif()
    if(NOT "${_fehl}" STREQUAL "")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_re2gator: Lauf ${_lauf} - Finisher: Leon nicht im Maul wie authored (Abnahme 4 M1):${_fehl} Zeile: ${_l}")
    endif()
    set(${_out} "Maul-Sitz ${_n} Bilder (P1 ${_p1} P2 ${_p2} P3 ${_p3}) Cut ${_cut}, Kopf ${_abw}, Kiefer-Abstand ${_kie}, Kopf/Leon im Bild ${_kopf}/${_leon}+${_oben} oben, Wasser ${_nass}, gehalten ${_gehalten}, frei ${_frei}" PARENT_SCOPE)
    set(r41b_ende_bild ${_ende_bild} PARENT_SCOPE)
endfunction()
# Nachbesserung 6 (Abnahme 5 M2): die Finisher-Kamera ist nicht die Kamera des Biss-Bildes - gator_boss.log "RE2 FINISHER Buehne ...
# NB6: laufende Kamera a/b ausgeschlossen" + "RE2 KAMERA FINISHER ... -> Cut c ... (Buehne)": c != a und c != b.
function(r41b_kamera_wechsel _dir _lauf _out)
    file(STRINGS "${_dir}/gator_boss.log" _bu REGEX "^RE2 FINISHER Buehne .*laufende Kamera (-?[0-9]+)/(-?[0-9]+) ausgeschlossen")
    file(STRINGS "${_dir}/gator_boss.log" _ka REGEX "^RE2 KAMERA FINISHER FINISHER RE2-Cut -?[0-9]+ -> Cut (-?[0-9]+) .*\\(Buehne\\)")
    if(NOT _bu OR NOT _ka)
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_re2gator: Lauf ${_lauf} - keine Buehne/Finisher-Kamera in gator_boss.log ('${_bu}' / '${_ka}')")
    endif()
    list(GET _bu -1 _b)
    string(REGEX MATCH "laufende Kamera (-?[0-9]+)/(-?[0-9]+) ausgeschlossen" _m "${_b}")
    set(_la ${CMAKE_MATCH_1})
    set(_lb ${CMAKE_MATCH_2})
    list(GET _ka -1 _k)
    string(REGEX MATCH "-> Cut (-?[0-9]+) " _m "${_k}")
    set(_c ${CMAKE_MATCH_1})
    if(_c EQUAL _la OR _c EQUAL _lb)
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_re2gator: Lauf ${_lauf} - Finisher-Kamera Cut ${_c} = laufende Kamera ${_la}/${_lb}: kein Schnitt, Umsetzen sichtbar (Abnahme 5 M2)")
    endif()
    set(${_out} "Finisher Cut ${_c} (Biss-Bild ${_la}/${_lb})" PARENT_SCOPE)
endfunction()

# ---------------------------------------------------------------- Lauf 1: Tuernische (Intro, Kamera, Finisher)
set(_dir "${_basis}/nische")
set(WORKDIR "${_dir}")
# Umgebung wie test_r35_granate.cmake (_env_basis): Titelbild + Auto-Start (RE15_TITLE_SHOT_AF=2), sonst
# bleibt die exe im Titel stehen und der Sprung @250 greift nie (gemessen 2026-10-23: 7 debug-Zeilen in 90 s).
re15_start_spiel(_rv 300
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
    SDL_ASSERT=always_ignore
    RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1
    RE15_SET_FLAG=9:110,9:113   # Querbezug Runde 41 Spur F (Kopf: Szene gesehen + Tor offen)
    RE15_R41_HP_AT=175:60       # Querbezug Runde 41 Spur F: Leons HP wie mit den zwei Spinnen-Treffern (Kopf)
    "RE15_GATOR_KANISTER=-6000,-15000"
    RE15_STATE_LOG=state.log
    "RE15_EXIT_AT=650#2090"
    "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf nische exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
if(NOT _ex)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf nische RE15_EXIT_AT nicht erreicht (keine Zeile '[flow] EXIT_AT')")
endif()
file(STRINGS "${_dir}/debug.log" _kan REGEX "\\[gator-re2\\] Kanister-Modell geladen")
if(NOT _kan)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Kanister-Modell nicht geladen (KANISTER40A0.MD1/.TIM) - debug.log ohne '[gator-re2] Kanister-Modell geladen'")
endif()
file(STRINGS "${_dir}/gator_boss.log" _gb)
set(_gesetzt 0)
set(_re2 0)
set(_intro 0)
set(_intro_ende 0)
set(_intro_lage 0)
set(_intro_wand -1)
set(_fin_cut -1)
set(_fin_halt 0)
set(_fin_halt_falsch "")
foreach(_l IN LISTS _gb)
    if(_l MATCHES "^RE2 KANISTER gesetzt")
        set(_gesetzt 1)
    endif()
    if(_l MATCHES "^RE2 ")
        math(EXPR _re2 "${_re2} + 1")
    endif()
    if(_l MATCHES "^RE2 INTRO Lage \\(Pos_set-Zwilling")
        set(_intro_lage 1)
    endif()
    if(_l MATCHES "^RE2 INTRO start")
        set(_intro 1)
    endif()
    if(_l MATCHES "^RE2 INTRO ende .* wand_draussen_max=([0-9]+)")
        set(_intro_ende 1)
        set(_intro_wand ${CMAKE_MATCH_1})
    endif()
    if(_l MATCHES "^RE2 KAMERA FINISHER FINISHER RE2-Cut -?[0-9]+ -> Cut (-?[0-9]+) ")
        set(_fin_cut ${CMAKE_MATCH_1})
    endif()
    if(_l MATCHES "^RE2 KAMERA FINISHER Halt Cut (-?[0-9]+) F")
        math(EXPR _fin_halt "${_fin_halt} + 1")
        if(NOT "${CMAKE_MATCH_1}" STREQUAL "${_fin_cut}")
            set(_fin_halt_falsch "${_l}")
        endif()
    endif()
endforeach()
if(NOT _gesetzt)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: RE15_GATOR_KANISTER ohne 'RE2 KANISTER gesetzt' in gator_boss.log")
endif()
if(NOT _intro OR NOT _intro_ende OR NOT _intro_lage)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: RE2-Intro (Sub 0 @0x8010086c-948) nicht mit gewaehlter Startlage gezuendet/beendet (Lage ${_intro_lage}, start ${_intro}, ende ${_intro_ende})")
endif()
if(NOT _intro_wand EQUAL 0)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Intro - Rumpf/Kopf ${_intro_wand} ausserhalb des Wassers (Abnahme 1 M2, Entscheid B7)")
endif()
# Nachtrag 4 (RE2-Kameraschnitte): das Kamera-Programm im ECHTEN Ablauf. RE2-Form (Dossier §9.1): Intro DURCHBRUCH bei
# Clip-6-Bild 0 (ROOM40A0 sub02 Cut 10), SEITE bei Bild 70 (@0x80100894-a4), ANGRIFF am Clip-6-Ende Bild 124
# (@0x801008bc-c8), frei am Clip-3-Ende Bild 149 (@0x801008f8-918); FINISHER bei FRESSEN P0 Clip 4 Bild 0
# (@0x80100fbc-ff4). Jeder Schnitt muss im Zustandslog desselben Bildes als cam=<Cut> stehen.
set(_kam_folge "")
set(_kam_frames "")
set(_fin_bild -1)
foreach(_l IN LISTS _gb)
    if(_l MATCHES "^RE2 KAMERA (INTRO|R1|FINISHER) ([A-Z]+) RE2-Cut -?[0-9]+ -> Cut (-?[0-9]+) F([0-9]+) clip=([0-9]+) bild=([0-9]+)")
        list(APPEND _kam_folge "${CMAKE_MATCH_2}:${CMAKE_MATCH_5}/${CMAKE_MATCH_6}")
        if(${CMAKE_MATCH_3} LESS 0)
            file(REMOVE "${_exe_kopie}")
            message(FATAL_ERROR "r41_re2gator: Kamera ${CMAKE_MATCH_2} ohne Zonenkamera (Cut ${CMAKE_MATCH_3}): ${_l}")
        endif()
        list(APPEND _kam_frames "${CMAKE_MATCH_4}=${CMAKE_MATCH_3}")
        if("${CMAKE_MATCH_2}" STREQUAL "FINISHER")
            set(_fin_bild ${CMAKE_MATCH_4})
        endif()
    elseif(_l MATCHES "^RE2 KAMERA INTRO frei .* clip=([0-9]+) bild=([0-9]+)")
        list(APPEND _kam_folge "FREI:${CMAKE_MATCH_1}/${CMAKE_MATCH_2}")
    endif()
endforeach()
set(_kam_soll "DURCHBRUCH:6/0;SEITE:6/70;ANGRIFF:6/124;FREI:3/149;FINISHER:4/0")
if(NOT "${_kam_folge}" STREQUAL "${_kam_soll}")
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Kamera-Folge '${_kam_folge}' statt '${_kam_soll}' (Nachtrag 4, Dossier §9)")
endif()
# Entscheid B6 (Abnahme 1 M1 b): der Finisher-Schnitt steht bis zum Ende des Raums - je 20 Bilder eine Halt-Zeile mit
# demselben Cut, und KEIN cam= im Zustandslog weicht ab dem Finisher-Bild davon ab (keine RE1.5-Todeskamera).
if(_fin_halt LESS 10 OR NOT "${_fin_halt_falsch}" STREQUAL "")
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Finisher-Halt ${_fin_halt} Zeilen (erwartet >= 10 bis EXIT_AT 650), abweichend: '${_fin_halt_falsch}'")
endif()
file(STRINGS "${_dir}/state.log" _zl)
set(_mo6 0)
set(_mo3 0)
set(_gator 0)
set(_hp_min 99999)
set(_raum 0)
set(_fin_cam_bilder 0)
set(_fin_cam_falsch "")
foreach(_l IN LISTS _zl)
    if(_l MATCHES "t=23 ")
        set(_raum 1)
    endif()
    if(_raum AND _l MATCHES "^F([0-9]+) .* cam=([0-9]+) ")
        set(_f ${CMAKE_MATCH_1})
        set(_c ${CMAKE_MATCH_2})
        foreach(_kf IN LISTS _kam_frames)
            if("${_kf}" STREQUAL "${_f}=${_c}")
                list(REMOVE_ITEM _kam_frames "${_kf}")
            endif()
        endforeach()
        if(_fin_bild GREATER 0 AND _f GREATER_EQUAL _fin_bild)
            math(EXPR _fin_cam_bilder "${_fin_cam_bilder} + 1")
            if(NOT "${_c}" STREQUAL "${_fin_cut}")
                set(_fin_cam_falsch "F${_f} cam=${_c}")
            endif()
        endif()
    endif()
    if(_l MATCHES "\\[([0-9]+) t=23 st=[0-9]+ ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) af=[0-9]+ [^]]*\\] hp=(-?[0-9]+)")
        set(_gator 1)
        if("${CMAKE_MATCH_2}" STREQUAL "6")
            math(EXPR _mo6 "${_mo6} + 1")
        elseif("${CMAKE_MATCH_2}" STREQUAL "3")
            math(EXPR _mo3 "${_mo3} + 1")
        endif()
        # hp=0 steht nur im Spawn-Bild VOR dem Boss-INIT (main.c spawnt mit 0, der erste Boss-Tick schreibt 25300)
        if(${CMAKE_MATCH_3} GREATER 0 AND ${CMAKE_MATCH_3} LESS _hp_min)
            set(_hp_min ${CMAKE_MATCH_3})
        endif()
    endif()
endforeach()
if(NOT _gator)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: kein Alligator (t=23) im Zustandslog")
endif()
if(NOT _kam_frames STREQUAL "")
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Schnitt nicht im Zustandslog desselben Bildes (Bild=Cut): ${_kam_frames}")
endif()
if(_fin_cam_bilder LESS 100 OR NOT "${_fin_cam_falsch}" STREQUAL "")
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: nach dem Finisher-Schnitt (F${_fin_bild} Cut ${_fin_cut}) ${_fin_cam_bilder} Zustandsbilder, Kamerawechsel: '${_fin_cam_falsch}' (Entscheid B6)")
endif()
if(_mo6 LESS 120 OR _mo3 LESS 145)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Intro-Clip-Bilder Clip 6 ${_mo6} / Clip 3 ${_mo3} (erwartet 125 / 150)")
endif()
if(_hp_min LESS 25000)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: HP unter dem RE2-Sockel (${_hp_min} < 25000) ohne Beschuss")
endif()
r41b_maul_sitz("${_basis}/nische" "nische" 0 _ms1)   # Nachbesserung 5: CHASE-Tod, Finisher bis EXIT_AT (P3 nicht ganz)
r41b_kamera_wechsel("${_basis}/nische" "nische" _kw1)   # Nachbesserung 6 (M2)
r41b_gier_wache("${_basis}/nische" "nische" _gw1)
set(_ms1 "${_ms1}, ${_kw1}")
set(_status1 "nische: ${_ms1}, RE2-Zeilen ${_re2}, Intro Lage/Start/Ende ${_intro_lage}/${_intro}/${_intro_ende} wand ${_intro_wand}, Clip 6/3 ${_mo6}/${_mo3}, Kamera ${_kam_folge}, Finisher Cut ${_fin_cut} ab F${_fin_bild} gehalten ${_fin_cam_bilder} Bilder / ${_fin_halt} Halt-Zeilen, HP min ${_hp_min}, kein Gier-Sprung")

# ---------------------------------------------------------------- Lauf 3: natuerlicher Zugang (B9: untergetaucht)
set(_dir "${_basis}/natuerlich")
set(WORKDIR "${_dir}")
re15_start_spiel(_rv 300
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
    SDL_ASSERT=always_ignore
    RE15_DEBUG_JUMP=2090@250
    RE15_SET_FLAG=9:110,9:113   # Querbezug Runde 41 Spur F (Kopf: Szene gesehen + Tor offen)
    "RE15_PLAYER_POS=-8200,-25550,3072" RE15_R39_MESS_SPIELER_HP=100
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W2,U12,W40"
    "RE15_EXIT_AT=520#2090"
    "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf natuerlich exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
file(STRINGS "${_dir}/debug.log" _sicht REGEX "\\[gator-re2\\] Sicht ")
set(_s_erst "")
set(_s_auf -1)
set(_s_auf_clip "")
set(_s_gez -1)
set(_s_gez_clip "")
set(_s_wieder 0)
set(_s_sprung -1)
foreach(_l IN LISTS _sicht)
    if(_l MATCHES "Sicht F([0-9]+) (untergetaucht|aufgetaucht) pos=\\(.*\\) clip=([0-9]+) bild=([0-9]+)")
        if("${_s_erst}" STREQUAL "")
            set(_s_erst "${CMAKE_MATCH_2}")
        endif()
        if("${CMAKE_MATCH_2}" STREQUAL "aufgetaucht" AND _s_auf LESS 0)
            set(_s_auf ${CMAKE_MATCH_1})
            set(_s_auf_clip "${CMAKE_MATCH_3}/${CMAKE_MATCH_4}")
        elseif("${CMAKE_MATCH_2}" STREQUAL "untergetaucht" AND _s_auf GREATER 0)
            set(_s_wieder 1)
        endif()
    elseif(_l MATCHES "Sicht erstes Bild F([0-9]+) gezeichnet .* clip=([0-9]+) bild=([0-9]+)")
        set(_s_gez ${CMAKE_MATCH_1})
        set(_s_gez_clip "${CMAKE_MATCH_2}")
    elseif(_l MATCHES "Sicht Lagesprung max ([0-9]+) F")
        set(_s_sprung ${CMAKE_MATCH_1})
    endif()
endforeach()
file(STRINGS "${_dir}/gator_boss.log" _gb3)
set(_g_auf -1)
set(_g_lage "")
foreach(_l IN LISTS _gb3)
    if(_l MATCHES "^RE2 AUFTAUCHEN F([0-9]+) ")
        set(_g_auf ${CMAKE_MATCH_1})
    endif()
    if(_l MATCHES "^RE2 INTRO Lage \\(Pos_set-Zwilling.*von \\((-?[0-9]+),(-?[0-9]+)\\)")
        set(_g_lage "${CMAKE_MATCH_1},${CMAKE_MATCH_2}")
    endif()
endforeach()
if(NOT "${_s_erst}" STREQUAL "untergetaucht" OR _s_auf LESS 1 OR NOT "${_s_auf_clip}" STREQUAL "6/0")
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf natuerlich - vor dem Durchbruch nicht untergetaucht bzw. Auftauchen nicht mit Clip 6 Bild 0 "
                        "(erste Sicht '${_s_erst}', aufgetaucht F${_s_auf} Clip/Bild ${_s_auf_clip}; RE2 Sce_em_set y=+3000 @RDT 0x25A0, "
                        "Sub 0 P1 Clip 6 @0x801008ac-b8)")
endif()
if(NOT _g_auf EQUAL _s_auf OR "${_g_lage}" STREQUAL "")
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf natuerlich - Auftauchen F${_s_auf} (Zeichner) != RE2 AUFTAUCHEN F${_g_auf} (Boss) oder keine "
                        "Intro-Lage (Pos_set-Zwilling) '${_g_lage}'")
endif()
if(_s_gez LESS _s_auf OR NOT "${_s_gez_clip}" STREQUAL "6" OR _s_wieder)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf natuerlich - erstes gezeichnetes Bild F${_s_gez} (Clip ${_s_gez_clip}) vor/ausserhalb des "
                        "Durchbruchs F${_s_auf} oder erneut untergetaucht (${_s_wieder})")
endif()
if(_s_sprung LESS 1 OR _s_sprung GREATER 340)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf natuerlich - sichtbarer Lagesprung ${_s_sprung} > 340 (groesster authored Wurzelschritt "
                        "Clip 6/3/0/1 x 2731/4096, unit_r41_re2gator_lauer)")
endif()
r41b_gier_wache("${_dir}" "natuerlich" _gw3)
set(_status3 "natuerlich: untergetaucht bis F${_s_auf} (Lauerplatz ${_g_lage}), Auftauchen Clip/Bild ${_s_auf_clip}, erstes Bild F${_s_gez} Clip ${_s_gez_clip}, groesster Bildschritt ${_s_sprung}, kein Gier-Sprung")

# ---------------------------------------------------------------- Lauf 4: Kanister getragen (Nachbesserung 4, M1 + M2)
set(_dir "${_basis}/kanister")
set(WORKDIR "${_dir}")
re15_start_spiel(_rv 300
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
    SDL_ASSERT=always_ignore
    RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1
    RE15_SET_FLAG=9:110,9:113   # Querbezug Runde 41 Spur F (Kopf: Szene gesehen + Tor offen)
    "RE15_PLAYER_POS=2500,-25000,2048" RE15_R39_MESS_SPIELER_HP=100
    "RE15_GATOR_KANISTER=-1500,-23400"
    "RE15_AUTOPILOT=xz:5500,-25500\;xz:5500,-20500\;xz:-1000,-21000\;xz:-1000,-25500\;xz:5500,-25500\;xz:5500,-20500\;xz:-1000,-21000\;noact"
    RE15_AUTOPILOT_ROOM=2090
    "RE15_EXIT_AT=1500#2090"   # Querbezug Runde 41 Spur F (Kopf): ohne Spinnen Aufnahme F1227 statt F1119 -> Fenster +100
    "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf kanister exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
file(STRINGS "${_dir}/gator_boss.log" _anh REGEX "^RE2 KANISTER angehaengt ")
file(STRINGS "${_dir}/debug.log" _sitz REGEX "\\[gator-re2\\] Kanister-Sitz F[0-9]+: angehaengt ")
if(NOT _anh OR NOT _sitz)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf kanister - Kanister nicht aufgenommen/angehaengt (angehaengt '${_anh}', Sitz-Zeilen '${_sitz}')")
endif()
list(GET _sitz -1 _sitz_l)
if(NOT _sitz_l MATCHES "angehaengt ([0-9]+) Bilder, Abweichung max ([0-9]+) ")
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf kanister - Kanister-Sitz-Zeile unlesbar: ${_sitz_l}")
endif()
set(_sitz_n "${CMAKE_MATCH_1}")
set(_sitz_max "${CMAKE_MATCH_2}")
if(_sitz_n LESS 200 OR NOT _sitz_max EQUAL 0)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf kanister - Kanister ${_sitz_max} neben Part6(gezeichnet) x Lokallage bzw. nur ${_sitz_n} angehaengte "
                        "Bilder (Abnahme 3 M1: Kind der Part-6-Weltmatrix @0x801018d0-e0, >= 200 Bilder verlangt)")
endif()
r41b_gier_wache("${_dir}" "kanister" _gw4)
set(_status4 "kanister: ${_sitz_n} angehaengte Bilder, Abweichung max ${_sitz_max}, ${_gw4}")

# ---------------------------------------------------------------- Lauf 5: Plattform-Belagerung ueber den echten Tuerweg (E1)
set(_dir "${_basis}/belagerung")
set(WORKDIR "${_dir}")
re15_start_spiel(_rv 300
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
    SDL_ASSERT=always_ignore
    RE15_DEBUG_JUMP=20A0@250 "RE15_FIRE_AOT=0@40#20A0" RE15_GB_TEST=1
    RE15_SET_FLAG=9:110,9:113   # Querbezug Runde 41 Spur F (Kopf: Szene gesehen + Tor offen)
    RE15_STATE_LOG=state.log
    "RE15_EXIT_AT=251#1240"
    "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf belagerung exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
r41b_gier_wache("${_dir}" "belagerung" _gw5)
if(NOT _gw5 MATCHES "^GIER-WACHE bis F[0-9]+: ([0-9]+) Bildpaare CHASE/GUARD, max \\|Gier-Delta\\| ([0-9]+) \\(ohne \\+1024\\), Spruenge > 0x60: 0, Befreiungen 0")
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf belagerung - keine 300 Bildpaare CHASE/GUARD ohne Sprung/Befreiung: '${_gw5}'")
endif()
file(STRINGS "${_dir}/gator_boss.log" _guard REGEX "^GUARD dist=")
list(LENGTH _guard _guard_n)
if(_guard_n LESS 5)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf belagerung - Belagerung (GUARD) nicht erreicht (${_guard_n} GUARD-Zeilen)")
endif()
# Nachbesserung 5 (Abnahme 4 M1): der dritte GUARD-Hochbiss (F627) toetet Leon; der RE2-Finisher laeuft bis zum P3-Ende (F881)
# vor dem Raumwechsel nach dem Game Over (~F900) - Leon liegt in JEDEM P1-P3-Bild wie authored am gezeichneten Kiefer, im Bild
# der Finisher-Kamera und im Wasser (vorher: Maul leer, Leon 2300-5300 hinter der Ostwand, Abnahme 4 §3.4).
r41b_maul_sitz("${_dir}" "belagerung" 1 _ms5)
r41b_kamera_wechsel("${_dir}" "belagerung" _kw5)   # Nachbesserung 6 (M2)
# Nachbesserung 6 (M1): Opfer-Modus gr=1 in JEDEM Bild vom toedlichen Biss bis zum letzten 2090-Bild vor dem Raumwechsel (Bildzaehler
# faellt nach dem Game Over auf 0), und dieses letzte Bild ist das der Summenzeile (Game-Over-Uebergabe).
file(STRINGS "${_dir}/state.log" _zl5)
set(_gr_an -1)
set(_gr_bilder 0)
set(_gr_los "")
set(_letzt -1)
set(_vor -1)
foreach(_l IN LISTS _zl5)
    if(NOT _l MATCHES "^F([0-9]+) .* gr=([01])")
        continue()
    endif()
    set(_f ${CMAKE_MATCH_1})
    set(_g ${CMAKE_MATCH_2})
    if(_gr_an GREATER_EQUAL 0 AND _f LESS _vor)
        break()                                     # Raumwechsel nach dem Game Over: neuer Bildzaehler
    endif()
    set(_vor ${_f})
    if(_gr_an LESS 0 AND _g EQUAL 1)
        set(_gr_an ${_f})
    endif()
    if(_gr_an GREATER_EQUAL 0)
        math(EXPR _gr_bilder "${_gr_bilder} + 1")
        set(_letzt ${_f})
        if(_g EQUAL 0 AND "${_gr_los}" STREQUAL "")
            set(_gr_los "F${_f}")
        endif()
    endif()
endforeach()
if(_gr_an LESS 0 OR NOT "${_gr_los}" STREQUAL "" OR _gr_bilder LESS 250)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf belagerung - Opfer nicht bis zum Raumwechsel gehalten (Abnahme 5 M1): gr=1 ab F${_gr_an}, ${_gr_bilder} Bilder, erstes gr=0 '${_gr_los}', letztes 2090-Bild F${_letzt}")
endif()
if(NOT _letzt EQUAL r41b_ende_bild)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf belagerung - Summenzeile Maul-Sitz Ende F${r41b_ende_bild} ist nicht das letzte 2090-Bild F${_letzt}")
endif()
set(_status5 "belagerung: ${_gw5}, GUARD-Zeilen ${_guard_n}, ${_ms5}, ${_kw5}, Opfer gehalten F${_gr_an}-F${_letzt} (${_gr_bilder} Bilder bis zum Raumwechsel)")

# ---------------------------------------------------------------- Lauf 2: Leon im Suedbecken (Dash, Putz)
set(_dir "${_basis}/dash")
set(WORKDIR "${_dir}")
re15_start_spiel(_rv 300
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
    SDL_ASSERT=always_ignore
    RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1
    RE15_SET_FLAG=9:110,9:113   # Querbezug Runde 41 Spur F (Kopf: Szene gesehen + Tor offen)
    "RE15_PLAYER_POS=-4000,-24000,2048" RE15_R39_MESS_SPIELER_HP=100
    RE15_STATE_LOG=state.log
    "RE15_EXIT_AT=1100#2090"
    "${_exe_kopie}")
file(REMOVE "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "r41_re2gator: Lauf dash exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
if(NOT _ex)
    message(FATAL_ERROR "r41_re2gator: Lauf dash RE15_EXIT_AT nicht erreicht (keine Zeile '[flow] EXIT_AT')")
endif()
file(STRINGS "${_dir}/gator_boss.log" _gb)
set(_dash 0)
set(_dash_ende 0)
set(_dash_wand -1)
set(_putz_runden 0)
set(_putz_gesamt 0)
set(_putz_im_becken 1)
set(_um_x "")
foreach(_l IN LISTS _gb)
    if(_l MATCHES "^RE2 DASH start .* putz_um=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)")
        set(_dash 1)
        set(_um_x ${CMAKE_MATCH_1})
        set(_um_z ${CMAKE_MATCH_3})
    endif()
    if(_l MATCHES "^RE2 DASH ende .* wand_draussen_max=([0-9]+)")
        set(_dash_ende 1)
        if(${CMAKE_MATCH_1} GREATER _dash_wand)
            set(_dash_wand ${CMAKE_MATCH_1})
        endif()
    endif()
    # Nachbesserung 1 (M2): je Durchlauf eine Zeile; Lage um Leon muss IM Becken liegen (x -8900..7200, z -27000..-5400)
    if(_l MATCHES "^RE2 PUTZ runde ([0-9]+)/6 spawns=([0-9]+) gesamt=([0-9]+) um=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)")
        math(EXPR _putz_runden "${_putz_runden} + 1")
        set(_putz_gesamt ${CMAKE_MATCH_3})
        if(${CMAKE_MATCH_4} LESS -8900 OR ${CMAKE_MATCH_4} GREATER 7200 OR ${CMAKE_MATCH_6} LESS -27000 OR ${CMAKE_MATCH_6} GREATER -5400)
            set(_putz_im_becken 0)
        endif()
    endif()
endforeach()
if(NOT _dash OR NOT _dash_ende)
    message(FATAL_ERROR "r41_re2gator: RE2-Dash (Zonenwechsel -> Clip 2/3 @0x80100dac-e28) nicht gezuendet/beendet (start ${_dash}, ende ${_dash_ende})")
endif()
if(NOT _dash_wand EQUAL 0)
    message(FATAL_ERROR "r41_re2gator: Dash - Rumpf/Kopf ${_dash_wand} ausserhalb des Wassers (Abnahme 1 M2)")
endif()
if(NOT _putz_runden EQUAL 6 OR NOT _putz_gesamt EQUAL 54)
    message(FATAL_ERROR "r41_re2gator: Putz ${_putz_runden} Durchlaeufe / ${_putz_gesamt} Spawns statt 6 / 54 (ROOM40A0 sub13/sub14)")
endif()
if(NOT _putz_im_becken)
    message(FATAL_ERROR "r41_re2gator: Putz-Lage ausserhalb des Beckens (putz_x/y/z nicht um Leon)")
endif()
# Fortsetzung 3 (M2): die WIRKSAME Lage der Putz-Teilchen im Zeichner (debug.log "[gator-re2] Putz-Sicht", gator_re2_pc.c)
file(STRINGS "${_dir}/debug.log" _ps REGEX "\\[gator-re2\\] Putz-Sicht F")
set(_ps_gez 0)
set(_ps_n 0)
foreach(_l IN LISTS _ps)
    if(_l MATCHES "gezeichnet_vorbild=([0-9]+) .* welt=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)")
        math(EXPR _ps_n "${_ps_n} + 1")
        if(${CMAKE_MATCH_1} GREATER _ps_gez)
            set(_ps_gez ${CMAKE_MATCH_1})
        endif()
        math(EXPR _ddx "${CMAKE_MATCH_2} - (${_um_x})")
        math(EXPR _ddz "${CMAKE_MATCH_4} - (${_um_z})")
        if(_ddx GREATER 3000 OR _ddx LESS -3000 OR _ddz GREATER 3000 OR _ddz LESS -3000)
            message(FATAL_ERROR "r41_re2gator: Putz-Teilchen nicht um Leon (${_um_x},${_um_z}): ${_l}")
        endif()
    endif()
endforeach()
if(_ps_n EQUAL 0 OR _ps_gez EQUAL 0)
    message(FATAL_ERROR "r41_re2gator: Putz nie gezeichnet (Putz-Sicht-Zeilen ${_ps_n}, groesste Quad-Zahl ${_ps_gez})")
endif()
file(STRINGS "${_dir}/state.log" _zl)
set(_mo2 0)
set(_mo3 0)
foreach(_l IN LISTS _zl)
    if(_l MATCHES "\\[([0-9]+) t=23 st=[0-9]+ ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) af=[0-9]+ ")
        if("${CMAKE_MATCH_2}" STREQUAL "2")
            math(EXPR _mo2 "${_mo2} + 1")
        elseif("${CMAKE_MATCH_2}" STREQUAL "3")
            math(EXPR _mo3 "${_mo3} + 1")
        endif()
    endif()
endforeach()
# Clip 2 = 150 Bilder (EDD), Clip 3 = 2 x 150 (Intro + Dash); Toleranz fuer den Bildzaehler-Start
if(_mo2 LESS 145 OR _mo3 LESS 290)
    message(FATAL_ERROR "r41_re2gator: Lauf dash Clip-Bilder Clip 2 ${_mo2} / Clip 3 ${_mo3} (erwartet 150 / 300)")
endif()
r41b_gier_wache("${_basis}/dash" "dash" _gw2)

# ---------------------------------------------------------------- Lauf 6: Auftritt ueber die 2090-Ankunftsszene (Spur F)
# Querbezug Runde 41 Spur F, Orchestrator-Entscheid F-E1 (Abnahme F 0, M4): "die 2090-Ankunftsszene ersetzt das RE2-Intro
# (Nachtrag 6) -> der Riegel prueft den Auftritt ueber F's Szene (Clip-Folge/Schnitte), B's uebrige Pruefungen bleiben
# unveraendert gruen". Echter Weg wie integration_r41_gatorszene_leon: Rechner (3,245), Leiter ROOM2080 Slot 1 @0x00BC2 -> 2090,
# ohne Bit 110. Verlangt im dritten 2090-Besuch (P3): (a) KEIN RE2-Intro ("RE2 INTRO start" fehlt - die Szene ersetzt Sub 0
# @0x8010086c-948 beim ersten Betreten); (b) die Szenen-Schnitte der Kamera-API dieser Spur: DURCHBRUCH (RE2-Cut 10) vor
# FINISHER (RE2-Cut 7, Fressen) und "KAMERA SZENE ... Ende"; (c) die Clip-Folge des Alligators im state.log (t=23 mo=): 6
# (Durchbruch @0x801008ac-b8) -> 4 (FRESSEN P1 @0x801010ec-f4) -> 5 (P2 @0x80101180-88) -> 11 (P3 @0x801011d8-e0) -> 0
# (Wende, Blend 32 @0x80100c38) -> 10 (Schrei R4 @0x80102c40-48); (d) keine Zeile "GIER-SPRUNG".
set(_dir "${_basis}/szene")
file(MAKE_DIRECTORY "${_dir}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")   # die Kopie entfernte der Lauf dash
set(WORKDIR "${_dir}")
re15_start_spiel(_rv 300
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
    SDL_ASSERT=always_ignore
    RE15_SET_FLAG=3:245
    RE15_DEBUG_JUMP=2080@250 "RE15_FIRE_AOT=1@40#2080"
    RE15_STATE_LOG=state.log
    "RE15_EXIT_AT=420#2090"
    "${_exe_kopie}")
if(NOT _rv EQUAL 0)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf szene exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
endif()
file(STRINGS "${_dir}/gator_boss.log" _sz_intro REGEX "^RE2 INTRO start")
if(_sz_intro)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf szene - RE2-Intro trotz Ankunftsszene gestartet: ${_sz_intro}")
endif()
file(STRINGS "${_dir}/gator_boss.log" _sz_kam REGEX "^RE2 KAMERA SZENE ")
set(_sz_durch -1)
set(_sz_fin -1)
set(_sz_ende 0)
foreach(_l IN LISTS _sz_kam)
    if(_l MATCHES "^RE2 KAMERA SZENE DURCHBRUCH RE2-Cut 10 -> Cut ([0-9]+) ")
        set(_sz_durch ${CMAKE_MATCH_1})
    elseif(_l MATCHES "^RE2 KAMERA SZENE FINISHER RE2-Cut 7 -> Cut ([0-9]+) " AND NOT _sz_durch EQUAL -1)
        set(_sz_fin ${CMAKE_MATCH_1})
    elseif(_l MATCHES "Ende" AND NOT _sz_fin EQUAL -1)
        set(_sz_ende 1)
    endif()
endforeach()
if(_sz_durch EQUAL -1 OR _sz_fin EQUAL -1 OR NOT _sz_ende)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf szene - Schnitte DURCHBRUCH ${_sz_durch} / FINISHER ${_sz_fin} / Ende ${_sz_ende} fehlen")
endif()
file(STRINGS "${_dir}/state.log" _sz_zl REGEX " t=23 ")
set(_sz_folge "")
set(_sz_letzt -1)
foreach(_l IN LISTS _sz_zl)
    if(_l MATCHES " t=23 st=[0-9]+ ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) af=")
        if(NOT CMAKE_MATCH_1 EQUAL _sz_letzt)
            list(APPEND _sz_folge ${CMAKE_MATCH_1})
            set(_sz_letzt ${CMAKE_MATCH_1})
        endif()
    endif()
endforeach()
set(_sz_soll 6 4 5 11 0 10)
set(_sz_i 0)
list(LENGTH _sz_soll _sz_n)
foreach(_m IN LISTS _sz_folge)
    if(_sz_i LESS _sz_n)
        list(GET _sz_soll ${_sz_i} _sz_w)
        if(_m EQUAL _sz_w)
            math(EXPR _sz_i "${_sz_i} + 1")
        endif()
    endif()
endforeach()
if(_sz_i LESS _sz_n)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_re2gator: Lauf szene - Clip-Folge 6/4/5/11/0/10 nicht vollstaendig (erreicht ${_sz_i} von ${_sz_n}), "
                        "state.log mo-Folge '${_sz_folge}'")
endif()
r41b_gier_wache("${_dir}" "szene" _gw6)
set(_status6 "szene: kein RE2-Intro, Schnitte DURCHBRUCH Cut ${_sz_durch} -> FINISHER Cut ${_sz_fin} -> Ende, Clip-Folge 6/4/5/11/0/10, ${_gw6}")
file(REMOVE "${_exe_kopie}")
message(STATUS "r41_re2gator: ok — ${_status1} | ${_status3} | ${_status4} | ${_status5} | dash: DASH ${_dash}/${_dash_ende} wand ${_dash_wand}, Putz ${_putz_runden} x 9 = ${_putz_gesamt}, gezeichnet bis ${_ps_gez} Quads/Bild in ${_ps_n} Messbildern, Clip 2/3 ${_mo2}/${_mo3}, ${_gw2} | ${_status6}")
