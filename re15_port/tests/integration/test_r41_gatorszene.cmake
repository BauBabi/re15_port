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
#      je 2 (RE15_FRAMEDUMP, WINDOW_SCALE=1); Auswerter `test_r41_gatorszene bilddiff` (RE15_TOOL): Schluessel in JEDEM
#      Bild sichtbar (Unterschied > 0), Puls max/min >= 1,40 ohne Plateau; Blink-Spitze <= 0x80.
# Nachbesserung 2 (Abnahme F 1, M2 "Water Key nach dem Tod nur 4-6 Pixel, als Schluessel nicht erkennbar"):
#   tod/tod_elza (Todesrolle) und NEU tod_r1/tod_r1_elza (RE2-R1-Kanistertod mit NATUERLICHER Kamera, Rezept K3 der Abnahme: Start
#   (-1000,-25550) Blick Ost ueber den RVD-Streifen 0 -> 1 (RVD @0x00234 x -500..500), Kanister-Testhaken der Spur B, 12 Schuesse).
#   Verlangt: debug.log "Schluessel-Lage: Kamera K (groesstes Bild, laufende L)" mit Quadrat 180 x H / Tiefe >= 10,4 Bildpunkte
#   (H = 26684 >> 7 = 208, RID @0x060 aller 13 Cuts; 10,4 = Schluessel der Ankunftseinstellung, abgenommen), "Schluessel-
#   Einstellung: Cut K (vorher Cut L) F<s>" und "... Ende: Cut L F<e>" mit e - s >= 90 (Halt GSZ_LEICHE_BILDER) in BEIDEN Laeufen
#   gleich; state.log cam=K in F<s+1>..F<e-3> und cam=L in F<e+1>..F<e+20> (Cut_old FUN_8004032c @0x8004033c/@0x80040364);
#   bilddiff in den Bildern der Einstellung: jedes Bild mit Schluessel, Puls >= 1,40, Kasten mindestens 10 x 9 Bildpunkte (= der
#   Ankunfts-Schluessel ~10,4 x 9,5, gatorszene_r41.c GSZ_KEY_AUF_*; Abnahme 1 mass 4 x 6 bzw. 6 x 4).
# Nachbesserung 3 (Abnahme F 2, M1 "Zwischenbild der Zonenkamera zwischen Explosionsschnitt und Schluessel-Einstellung"):
#   NEU tod_explosion/tod_explosion_elza (Rezept KW/KWE der Abnahme 2: natuerliche Leiter-Kamera Cut 0 nach dem Sprung, Kanister-
#   Testhaken der Spur B, Schuesse ab F151). B's R1 P0 schneidet auf die Explosions-Kamera E (Kopf in Cut 0 nicht im Bild, RE2
#   +0x218 |= 0x40 @0x80102248); am Clip-9-Ende (RE2 @0x8010249c) schnitte B auf den Ausgangs-Cut X zurueck (@0x801024a4-ec), das
#   Tot-Flag kaeme zwei Takte spaeter (@0x8010265c-64). Der Schluessel-Schnitt ersetzt den Rueckschnitt im selben Takt (gatorszene_r41.c
#   key_rueckschnitt_pruefen). Verlangt: gator_boss.log "KAMERA R1 EXPLOSION ... -> Cut E F<p>" (E >= 0), debug.log "Schluessel-
#   Schnitt statt R1-Rueckschnitt: Explosions-Cut E -> Ausgangs-Cut X F<r>", "Schluessel-Einstellung: Cut K (vorher Cut X) F<r>",
#   "... Ende: Cut X F<e>" (e - r 90..94), E/K/X paarweise verschieden, "R1 ende (Leiche)" und keine "KAMERA R1"-Zeile nach F<r>;
#   state.log cam=E in F<p>..F<r-1> und cam=K in F<r>..F<e-3> (vorher cam 0 in F196-F197); Bilder je Bild ueber den Auswerter
#   `test_r41_gatorszene kamfolge` (Referenzen E = Mitte der Explosions-Einstellung, K = F<r+45>, X = F<e+15>): Folge E+K+ ueber
#   F<p+5>..F<r+10> (vorher fd 197/198 = X).
#
# Nachbesserung 4 (Abnahme F 3, M1 = Nachtrag 29 "Die Zombie Leiche inklusive der Kette ... muss weiter weg sein, damit man am
#   Start des Kampfes eine Chance hat"): leon/elza und NEU flucht/flucht_elza (derselbe Leiterweg, Spielfigur rennt ab dem
#   Kampfbeginn nach Osten: RE15_INPUT_SCRIPT=W13.5,UX4.6 - gehalten ab F406, die Szene haelt die Figur bis zum Kampfbeginn).
#   Verlangt: Abstandsquadrat Gator-Wurzel <-> Spielfigur im Bild der Folge C >= RE2-Intro-Ende (Auswerter `test_r41_gatorszene
#   re2soll`, aus den RE2-Daten gerechnet), in den RE2-Bildern danach kein Biss (Clip 4) und kein Treffer; leon/elza zusaetzlich
#   cam=0 in F<c>..F<c+74> (vorher fest F405-F479), flucht zusaetzlich Spielfigur >= 5000 oestlich der Kampfbeginn-Lage.
# Nachbesserung 5 (Abnahme F 4, M1-M4 = Orchestrator-Entscheid F-E2 "Kampfbeginn = RE2 ... Sollwert RE2 mit oberer UND unterer
#   Grenze aus dem RE2-Beleg"): RE2-SOLLBAND aus dem Auswerter `test_r41_gatorszene re2soll` (tools/r41_gatorszene/probe_re2_band_n5.c):
#   Abstand Intro-Ende (Cut 1 + 150) .. fruehester Kampfbeginn fuer Leon (sub03 Plc_ret @SCD 0x02B6 nach Cut 1 + 113), Kontakt
#   121 .. 158 Bilder. GLEICHES MASS auf beiden Seiten: "erster Kontakt oder Angriff" = das erste Bild nach dem Kampfbeginn mit
#   Kontakt-Mass (debug.log "[gatorszene] Kontakt F..: +N", ein Knochen erreicht die Spielfigur laengs der Schwimmrichtung - RE2
#   Gier 2048 = "vorderster Knochen auf Leons x"), Biss (Clip 4) oder Treffer (HP faellt); RE2 greift in Sub 1/2 nicht an (Sub 1
#   Main @0x80100c34-40, Sub 2 Main @0x80100cf8-d04, FRESSEN nur aus Sub 7 @0x801012e0-e4). leon/elza (Leon steht): Abstand im Band
#   und erster Kontakt/Angriff im Band; Beckenzone Gator = Spielfigur = 0 (RE2 @0x80100990-ec, Zeile "Kampfbeginn F..: ... Zone").
#   flucht/flucht_elza: die Spielfigur rennt ab dem Kampfbeginn nach NORDEN durch den Westkanal (der Gator kommt jetzt von Osten aus
#   dem Suedbecken): RE15_INPUT_SCRIPT=W10,L0.6,UX5 (Linksdrehung F301-F318 auf Gier -1728, Rennen ab F319; gemessen F41n5 N_LW_test);
#   Abstand im Band, kein Kontakt/Angriff vor dem RE2-Wert, Spielfigur >= 5000 nach Norden in den RE2-Bildern.
# FG-E7 (AUFTRAG.md 2026-10-26 01:19): Todeslage ohne Kopf-Klemme; die K3-Wandlage (4430,-26160) ist OFFEN (Release Notes 'Ab Samstag', Behebung Option (a))
#   Befund der Integration F+G+M (tests:FGM): in den Rezepten K3/KW stirbt der Alligator SOFORT an der GB_TEST=5-Marke (4997,-20318)
#   (Gier 1089 = Blick nach Sueden; der Kanister-Testhaken (4430,-26160) liegt 840 vor der Suedwand im Aufnahme-Fenster). Der
#   R1-Todes-Clip traegt ihn ~4950 vorwaerts, der Kopf ragt ueber die Suedwand z -27000, Spur G's Kopf-Klemme (enemy_ai_boss_gator.c
#   Haken (2b), gator_re2_r41.c re15_gr2_kopf_klemme) schiebt den Anker (gator_boss.log "KOPF-KLEMME an F62" bzw. "F176", groesster
#   Ueberstand 67): Leiche (3413,-23942) statt (3413,-25001), Schluessel-Lage Kamera 1 Tiefe 5918 = Quadrat 6,3 < 10,4 Bildpunkte
#   (tod_r1) bzw. K = E = 1 (tod_explosion).
#   Neue Todeslagen (an der exe gemessen, Integrationsbaum e10a32c1; gator_boss.log ohne jede KOPF-KLEMME-Zeile; Leon und Elza
#   bitgleich): Kanister-Testhaken und GB_TEST=5 bleiben, geschossen wird erst NACH dem Aggro der Marke (enemy_ai_boss_gator.c
#   GBP_LURK mit GB_TEST=5: `if (++s_t5 >= 900) g->aggro = 1`) - dann dreht der Alligator auf die Spielfigur und schwimmt los:
#   - tod_r1/_elza (K3-Start (-1000,-25550) Blick Ost, U1.2 ueber den RVD-Streifen 0 -> 1 @0x00234): Gier 1435 auf Leon, Schuss
#     F931 -> R1 bei (5073,-20271) "Sperre ohne Schnitt" in Cut 1 (natuerliche Kamera), Leiche (1320,-23508), Schluessel-Lage
#     Kamera 0 (laufende 1) Tiefe 2304 = Quadrat 16,2 Bildpunkte, Einstellung Cut 0 F969-F1061, zurueck Cut 1 (ebenso gemessen mit
#     Schuss F916 und F955; ab F960 beisst der Alligator).
#   - tod_explosion/_elza (Spieler im RVD-Streifen 0 -> 7 @0x00248 bei (-7000,-19000) Blick Nord = laufender Cut 7): Gier ~2005 nach
#     Westen durch den Suedkanal, Schuss F1156 -> R1 bei (352,-22265), Explosions-Cut E 0, Leiche (-4582,-21497), Schluessel-Schnitt
#     F1192 statt B's Rueckschnitt auf den Ausgangs-Cut X 7, Schluessel-Kamera K 1 Tiefe 2443 = Quadrat 15,3 Bildpunkte, Einstellung
#     F1192-F1284 (E/K/X verschieden gemessen fuer Schuss F1111..F1186; F1066: K = E = 0, F1201: K = X = 7). Am Leiter-Cut 0 (Rezept
#     KW) lag jede gemessene Lage ohne Kopf-Klemme im Suedkanal mit K = X = 0 (Schuss F1045..F1180).
#   Vorbedingung im Riegel (r41f_fge7_ohne_klemme): keine KOPF-KLEMME-Zeile zwischen "R1 start" und "R1 ende (Leiche)", keine beim
#   R1-Start offene Klemme; tod_r1 zusaetzlich R1 ohne Explosionsschnitt. Schwellen unveraendert (Quadrat >= 10,4 Bildpunkte,
#   E/K/X paarweise verschieden).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -DTEIL=leon|elza|flucht|flucht_elza -P test_r41_gatorszene.cmake
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

# ---- Nachbesserung 4 (Abnahme F 3 M1 = Nachtrag 29 "Die Zombie Leiche inklusive der Kette ... muss weiter weg sein, damit man am
# Start des Kampfes eine Chance hat"): Kampfbeginn weit genug von der Leiter. Sollwert = Ende des RE2-Alligator-Intros in ROOM40A0,
# vom Auswerter `test_r41_gatorszene re2soll` aus den RE2-Daten gerechnet (wie tools/r41_gatorszene/probe_re2_intro_ende.c):
# Abstandsquadrat Gator-Wurzel <-> Leon (Pos_set @SCD 0x0736, Clip 6/3 @0x801008ac-948, Plc_dest @SCD 0x0286) und die Bilder, bis
# der vorderste Knochen des stehenden Leons x erreicht (Sperre +0x238 = 110 @0x8010091c-20, Hysterese @0x80100b5c-64/@0x80100b88).
function(r41f_re2soll out_ab2 out_bilder out_ab2_max out_bilder_max)
    execute_process(COMMAND "${RE15_TOOL}" re2soll RESULT_VARIABLE _r OUTPUT_VARIABLE _o ERROR_VARIABLE _e)
    string(REGEX MATCH "re2soll: abstand=([0-9]+) abstand2=([0-9]+) bilder=([0-9]+) abstand_max=([0-9]+) abstand2_max=([0-9]+) bilder_max=([0-9]+)"
           _m "${_o}")
    if(NOT _r EQUAL 0 OR NOT _m)
        message(FATAL_ERROR "r41_gatorszene: re2soll fehlgeschlagen (rc ${_r}): ${_o}")
    endif()
    set(${out_ab2} ${CMAKE_MATCH_2} PARENT_SCOPE)
    set(${out_bilder} ${CMAKE_MATCH_3} PARENT_SCOPE)
    set(${out_ab2_max} ${CMAKE_MATCH_5} PARENT_SCOPE)
    set(${out_bilder_max} ${CMAKE_MATCH_6} PARENT_SCOPE)
endfunction()
# Auswertung des LETZTEN Raum-Abschnitts von state.log (der Bildzaehler beginnt je Raum mit F0 neu) ab dem Kampfbeginn F<c>
# (Folge C): Abstandsquadrat Gator-Wurzel <-> Spielfigur in F<c> >= RE2; in F<c+1>..F<c+bilder> kein Biss (Gator Clip 4 = Biss
# des Boss-Moduls) und kein Treffer (HP faellt); mit_cam: F<c>..F<c+74> cam=0 (Nachbesserung 1 M1, Fassung 1: F405-F479).
# Ergebnis: ab2 (Abstandsquadrat), x0/xb (Spieler-x in F<c> / F<c+bilder>), erster Biss/Treffer im Lauf (-1 = keiner).
function(r41f_kampf dir c re2_ab2 re2_bilder re2_ab2_max re2_bilder_max mit_cam out_ab2 out_x0 out_xb out_biss out_treffer
                    out_z0 out_zb out_k)
    file(STRINGS "${dir}/state.log" _alle REGEX "^F[0-9]+ ")
    set(_ab "")
    foreach(_z IN LISTS _alle)
        if(_z MATCHES "^F0 ")
            set(_ab "")
        endif()
        list(APPEND _ab "${_z}")
    endforeach()
    math(EXPR _c_ende "${c} + 74")
    math(EXPR _b_ende "${c} + ${re2_bilder}")
    set(_ab2 -1)
    set(_x0 0)
    set(_xb -999999)
    set(_z0 0)
    set(_zb -999999)
    set(_hp0 -1)
    set(_biss -1)
    set(_treffer -1)
    set(_n_cam 0)
    set(_n_b 0)
    foreach(_z IN LISTS _ab)
        string(REGEX MATCH "^F([0-9]+) " _m "${_z}")
        set(_f ${CMAKE_MATCH_1})
        if(_f LESS c)
            continue()
        endif()
        if(NOT _z MATCHES "PL\\((-?[0-9]+),(-?[0-9]+),rot=-?[0-9]+,hp=(-?[0-9]+)\\)")
            continue()
        endif()
        set(_px ${CMAKE_MATCH_1})
        set(_pz ${CMAKE_MATCH_2})
        set(_hp ${CMAKE_MATCH_3})
        set(_mo -1)
        if(_z MATCHES " t=23 st=[0-9]+ ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) af=[0-9]+ [^@]*@\\((-?[0-9]+),(-?[0-9]+),r")
            set(_mo ${CMAKE_MATCH_1})
            set(_gx ${CMAKE_MATCH_2})
            set(_gz ${CMAKE_MATCH_3})
        endif()
        if(_f EQUAL c)
            if(_mo EQUAL -1)
                message(FATAL_ERROR "r41_gatorszene: kein Alligator (t=23) im state.log-Bild F${c}: ${_z}")
            endif()
            math(EXPR _dx "${_gx} - ${_px}")
            math(EXPR _dz "${_gz} - ${_pz}")
            math(EXPR _ab2 "${_dx} * ${_dx} + ${_dz} * ${_dz}")
            set(_hp0 ${_hp})
            set(_x0 ${_px})
            set(_z0 ${_pz})
        endif()
        if(mit_cam AND _f LESS_EQUAL _c_ende)
            math(EXPR _n_cam "${_n_cam} + 1")
            if(NOT _z MATCHES " cam=0 ")
                message(FATAL_ERROR "r41_gatorszene: Kampfanfang F${c}-F${_c_ende} nicht auf Cut 0 (Nachbesserung 1 M1): ${_z}")
            endif()
        endif()
        if(_f GREATER c)
            if(_biss EQUAL -1 AND _mo EQUAL 4)
                set(_biss ${_f})
            endif()
            if(_treffer EQUAL -1 AND _hp LESS _hp0)
                set(_treffer ${_f})
            endif()
            if(_f LESS_EQUAL _b_ende)
                math(EXPR _n_b "${_n_b} + 1")
                if(_f EQUAL _b_ende)
                    set(_xb ${_px})
                    set(_zb ${_pz})
                endif()
            endif()
        endif()
    endforeach()
    if(_ab2 LESS 0)
        message(FATAL_ERROR "r41_gatorszene: Kampfbeginn F${c} nicht im letzten state.log-Abschnitt (${dir})")
    endif()
    if(mit_cam AND _n_cam LESS 70)
        message(FATAL_ERROR "r41_gatorszene: nur ${_n_cam} state.log-Bilder F${c}-F${_c_ende}")
    endif()
    if(_n_b LESS re2_bilder)
        message(FATAL_ERROR "r41_gatorszene: nur ${_n_b} state.log-Bilder nach dem Kampfbeginn F${c} (verlangt ${re2_bilder}, EXIT_AT zu frueh?)")
    endif()
    if(_ab2 LESS re2_ab2 OR _ab2 GREATER re2_ab2_max)
        message(FATAL_ERROR "r41_gatorszene: Abstandsquadrat Gator-Wurzel <-> Spielfigur am Kampfbeginn F${c} ${_ab2} nicht im RE2-Band "
                            "${re2_ab2}..${re2_ab2_max} (Abnahme 3: 6162^2 = 37970244 zu nah, Abnahme 4: 17539^2 = 307616521 zu weit)")
    endif()
    # Kampfbeginn-Zeile: Beckenzone Gator = Spielfigur = 0 (RE2-Zonenbeziehung am Intro-Ende, @0x80100990-ec)
    file(STRINGS "${dir}/debug.log" _kb REGEX "\\[gatorszene\\] Kampfbeginn F${c}: .* Zone ([0-9]+), Spielfigur .* Zone ([0-9]+), Abstand")
    if(NOT _kb)
        message(FATAL_ERROR "r41_gatorszene: keine Zeile '[gatorszene] Kampfbeginn F${c}' in ${dir}/debug.log")
    endif()
    list(GET _kb -1 _kbz)
    string(REGEX MATCH "Zone ([0-9]+), Spielfigur .* Zone ([0-9]+), Abstand" _m "${_kbz}")
    if(NOT CMAKE_MATCH_1 EQUAL 0 OR NOT CMAKE_MATCH_2 EQUAL 0)
        message(FATAL_ERROR "r41_gatorszene: Beckenzone am Kampfbeginn Gator ${CMAKE_MATCH_1} / Spielfigur ${CMAKE_MATCH_2} (RE2: beide 0, "
                            "Abnahme 4: Gator 1 -> 300 Bilder Dash): ${_kbz}")
    endif()
    # gleiches Mass: erster Kontakt oder Angriff (Kontakt-Zeile, Biss, Treffer) in Bildern nach dem Kampfbeginn
    set(_k -1)
    file(STRINGS "${dir}/debug.log" _ko REGEX "\\[gatorszene\\] Kontakt F[0-9]+: \\+[0-9]+ Bilder nach dem Kampfbeginn")
    if(_ko)
        list(GET _ko -1 _koz)
        string(REGEX MATCH "\\+([0-9]+) Bilder" _m "${_koz}")
        set(_k ${CMAKE_MATCH_1})
    endif()
    foreach(_ev ${_biss} ${_treffer})
        if(NOT _ev EQUAL -1)
            math(EXPR _d "${_ev} - ${c}")
            if(_k EQUAL -1 OR _d LESS _k)
                set(_k ${_d})
            endif()
        endif()
    endforeach()
    if(NOT _k EQUAL -1 AND _k LESS re2_bilder)
        message(FATAL_ERROR "r41_gatorszene: erster Kontakt/Angriff nach ${_k} Bildern < RE2 ${re2_bilder} (Biss F${_biss} / Treffer "
                            "F${_treffer}, Kampfbeginn F${c}; Abnahme 3: Biss nach 54/26 Bildern)")
    endif()
    if(mit_cam AND (_k EQUAL -1 OR _k GREATER re2_bilder_max))
        message(FATAL_ERROR "r41_gatorszene: Leon steht - erster Kontakt/Angriff nach ${_k} Bildern nicht im RE2-Band "
                            "${re2_bilder}..${re2_bilder_max} (Abnahme 4: Biss nach 470 Bildern)")
    endif()
    set(${out_ab2} ${_ab2} PARENT_SCOPE)
    set(${out_x0} ${_x0} PARENT_SCOPE)
    set(${out_xb} ${_xb} PARENT_SCOPE)
    set(${out_biss} ${_biss} PARENT_SCOPE)
    set(${out_treffer} ${_treffer} PARENT_SCOPE)
    set(${out_z0} ${_z0} PARENT_SCOPE)
    set(${out_zb} ${_zb} PARENT_SCOPE)
    set(${out_k} ${_k} PARENT_SCOPE)
endfunction()
function(r41f_folge_c dir out_c)
    file(STRINGS "${dir}/debug.log" _fc REGEX "\\[gatorszene\\] Folge C ROOM[0-9A-F]+ F[0-9]+ ")
    if(NOT _fc)
        message(FATAL_ERROR "r41_gatorszene: keine Folge-Zeile C (Kampfbeginn) in ${dir}/debug.log")
    endif()
    list(GET _fc -1 _l)
    string(REGEX MATCH "Folge C ROOM[0-9A-F]+ F([0-9]+) " _m "${_l}")
    set(${out_c} ${CMAKE_MATCH_1} PARENT_SCOPE)
endfunction()
# FG-E7 (AUFTRAG.md 2026-10-26 01:19): Vorbedingung der Todeslage in gator_boss.log (re15_gr2_log): zwischen "R1 start" und "R1 ende
# (Leiche)" keine Zeile der Kopf-Klemme ("KOPF-KLEMME an/aus F..", gator_re2_r41.c re15_gr2_kopf_klemme), beim R1-Start keine offene
# Klemme, genau ein R1. ohne_schnitt = 1: R1 mit der natuerlichen Kamera ("KAMERA R1 Sperre ohne Schnitt", kein "KAMERA R1 EXPLOSION").
function(r41f_fge7_ohne_klemme dir teil ohne_schnitt)
    if(NOT EXISTS "${dir}/gator_boss.log")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${teil} - kein gator_boss.log (FG-E7-Vorbedingung), ${dir}")
    endif()
    file(STRINGS "${dir}/gator_boss.log" _kk
         REGEX "R1 start pos=|R1 ende \\(Leiche\\)|KOPF-KLEMME (an|aus) F|KAMERA R1 (EXPLOSION|Sperre ohne Schnitt)")
    set(_offen 0)
    set(_offen_z "")
    set(_im_r1 0)
    set(_n_r1 0)
    set(_sperre 0)
    set(_expl 0)
    foreach(_l IN LISTS _kk)
        if(_l MATCHES "R1 start pos=")
            if(_offen)
                message(FATAL_ERROR "r41_gatorszene: Lauf ${teil} - FG-E7-Vorbedingung verletzt: Kopf-Klemme beim R1-Start offen "
                                    "('${_offen_z}'), ${dir}")
            endif()
            set(_im_r1 1)
            math(EXPR _n_r1 "${_n_r1} + 1")
        elseif(_l MATCHES "R1 ende \\(Leiche\\)")
            set(_im_r1 0)
        elseif(_l MATCHES "KOPF-KLEMME (an|aus) F")
            if(_im_r1)
                message(FATAL_ERROR "r41_gatorszene: Lauf ${teil} - FG-E7-Vorbedingung verletzt: Kopf-Klemme im R1 ('${_l}'), ${dir}")
            endif()
            if(_l MATCHES "KOPF-KLEMME an F")
                set(_offen 1)
                set(_offen_z "${_l}")
            else()
                set(_offen 0)
            endif()
        elseif(_l MATCHES "KAMERA R1 Sperre ohne Schnitt")
            set(_sperre 1)
        elseif(_l MATCHES "KAMERA R1 EXPLOSION")
            set(_expl 1)
        endif()
    endforeach()
    if(NOT _n_r1 EQUAL 1)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${teil} - ${_n_r1} R1-Starts in gator_boss.log (erwartet 1), ${dir}")
    endif()
    if(ohne_schnitt AND (NOT _sperre OR _expl))
        message(FATAL_ERROR "r41_gatorszene: Lauf ${teil} - R1 nicht mit der natuerlichen Kamera (Sperre ohne Schnitt ${_sperre}, "
                            "Explosionsschnitt ${_expl}), ${dir}")
    endif()
endfunction()


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

# ---- Nachbesserung 3 (M1): kein Bild des Zonen-Cuts zwischen Explosionsschnitt und Schluessel-Einstellung (Leon / Elza) -------
if(TEIL MATCHES "^tod_explosion")
    if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - RE15_TOOL (test_r41_gatorszene) fehlt: '${RE15_TOOL}'")
    endif()
    # FG-E7 (AUFTRAG.md 2026-10-26 01:19): Todeslage ohne Kopf-Klemme; die K3-Wandlage (4430,-26160) ist OFFEN (Release Notes 'Ab Samstag', Behebung Option (a))
    # Rezept (Kopf des Skripts, Abschnitt FG-E7): Spieler im RVD-Streifen 0 -> 7 (@0x00248) bei (-7000,-19000) Blick Nord = laufender
    # Cut 7 statt Leiter-Cut 0 (dort lag jede Lage ohne Kopf-Klemme im Suedkanal mit K = X = 0); erster Schuss nach dem Aggro der
    # GB_TEST=5-Marke: W38.2 = 1146 Bilder warten, Zielen F1147-F1155, Feuer F1156 -> R1 F1156, Rueckschnitt-Ersatz F1192, Ende F1284.
    # Vorher (Rezept KW/KWE der Abnahme 2): Leiter-Spawn, W5, R1 F160 an der Marke, EXIT_AT 330, Bilder 150-330.
    # 12 Schuesse ausgeschrieben (input_pc.c script_parse_once kennt keine Wiederholung, Abnahme F 2 H1)
    set(_schuss "W38.2")
    foreach(_i RANGE 1 12)
        string(APPEND _schuss ",M0.3,MA0.1")
    endforeach()
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
        ${_wahl}
        RE15_SET_FLAG=9:110
        RE15_DEBUG_JUMP=${_r2090}@250
        RE15_STATE_LOG=state.log
        "RE15_PLAYER_POS=-7000,-19000,3072,0"
        RE15_R39_MESS_SPIELER_HP=100 RE15_GB_TEST=5 "RE15_GATOR_KANISTER=4430,-26160" "RE15_GIVE=3:15,21:50" RE15_EQUIP=3
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_schuss}"
        "RE15_EXIT_AT=1330#${_r2090}" "RE15_FRAMEDUMP=1146-1330/1:fd_"
        "${_exe_kopie}")
    file(REMOVE "${_exe_kopie}")
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${_dir}")
    endif()
    if(NOT EXISTS "${_dir}/gator_boss.log" OR NOT EXISTS "${_dir}/debug.log" OR NOT EXISTS "${_dir}/state.log")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} ohne gator_boss.log/debug.log/state.log (${_dir})")
    endif()
    # die Spielfigur des Szenarios (Elza = Familie PL04, Leon = PL00; main.c "[pl] Spieler-Familie")
    if(TEIL MATCHES "elza$")
        set(_fam PL04)
    else()
        set(_fam PL00)
    endif()
    file(STRINGS "${_dir}/debug.log" _famz REGEX "\\[pl\\] Spieler-Familie ${_fam} ")
    if(NOT _famz)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Spielfigur nicht ${_fam} (Szenario falsch), ${_dir}")
    endif()
    # B's R1 mit Explosionsschnitt (der Fall dieses Riegels)
    file(STRINGS "${_dir}/gator_boss.log" _gb REGEX "KAMERA R1 |R1 ende \\(Leiche\\)")
    set(_ecut -1)
    set(_p -1)
    set(_tot 0)
    foreach(_l IN LISTS _gb)
        if(_l MATCHES "KAMERA R1 EXPLOSION RE2-Cut -?[0-9]+ -> Cut (-?[0-9]+) F([0-9]+)")
            set(_ecut "${CMAKE_MATCH_1}")
            set(_p "${CMAKE_MATCH_2}")
        elseif(_l MATCHES "R1 ende \\(Leiche\\)")
            set(_tot 1)
        endif()
    endforeach()
    if(_ecut LESS 0 OR _p LESS 0 OR NOT _tot)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - kein R1 mit Explosionsschnitt bis zur Leiche (Cut ${_ecut} F${_p}, Leiche ${_tot}), ${_dir}")
    endif()
    r41f_fge7_ohne_klemme("${_dir}" "${TEIL}" 0)   # FG-E7: Todeslage ohne Kopf-Klemme (Vorbedingung, s. Kopf)
    file(STRINGS "${_dir}/debug.log" _rs REGEX "\\[gatorszene\\] Schluessel-Schnitt statt R1-Rueckschnitt: ")
    file(STRINGS "${_dir}/debug.log" _e REGEX "\\[gatorszene\\] Schluessel-Einstellung: Cut [0-9]+ ")
    file(STRINGS "${_dir}/debug.log" _o REGEX "\\[gatorszene\\] Schluessel-Einstellung Ende: Cut [0-9]+ ")
    if(NOT _rs OR NOT _e OR NOT _o)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Rueckschnitt-Ersatz '${_rs}' / Einstellung '${_e}' / Ende '${_o}' fehlt (Abnahme 2 M1), ${_dir}")
    endif()
    list(GET _rs 0 _rs0)
    list(GET _e 0 _e0)
    list(GET _o 0 _o0)
    if(NOT _rs0 MATCHES "Explosions-Cut ([0-9]+) -> Ausgangs-Cut ([0-9]+) F([0-9]+)")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Zeile unlesbar: ${_rs0}")
    endif()
    set(_rexpl "${CMAKE_MATCH_1}")
    set(_x "${CMAKE_MATCH_2}")
    set(_r "${CMAKE_MATCH_3}")
    if(NOT _e0 MATCHES "Cut ([0-9]+) \\(vorher Cut ([0-9]+)\\) F([0-9]+)")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Einstellungs-Zeile unlesbar: ${_e0}")
    endif()
    set(_k "${CMAKE_MATCH_1}")
    set(_ev "${CMAKE_MATCH_2}")
    set(_s "${CMAKE_MATCH_3}")
    if(NOT _o0 MATCHES "Ende: Cut ([0-9]+) F([0-9]+)")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Ende-Zeile unlesbar: ${_o0}")
    endif()
    set(_oc "${CMAKE_MATCH_1}")
    set(_ee "${CMAKE_MATCH_2}")
    if(NOT _rexpl EQUAL _ecut OR NOT _ev EQUAL _x OR NOT _s EQUAL _r OR NOT _oc EQUAL _x)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Folge passt nicht: Explosion Cut ${_ecut} (Ersatz-Zeile ${_rexpl}), Einstellung F${_s} "
                            "(Ersatz F${_r}), vorher ${_ev} / zurueck ${_oc} (soll Ausgangs-Cut ${_x})")
    endif()
    if(_ecut EQUAL _k OR _k EQUAL _x OR _ecut EQUAL _x)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Kameras nicht verschieden (E ${_ecut}, K ${_k}, X ${_x}): der Riegel misst so keinen Schnitt")
    endif()
    math(EXPR _halt "${_ee} - ${_s}")
    if(_halt LESS 90 OR _halt GREATER 94)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Einstellung ${_halt} Bilder (soll 90 = GSZ_LEICHE_BILDER + Faden-Ende)")
    endif()
    foreach(_l IN LISTS _gb)
        if(_l MATCHES "KAMERA R1 .* F([0-9]+)")
            if(CMAKE_MATCH_1 GREATER _r)
                message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - B's R1-Kamera nach dem Schluessel-Schnitt F${_r} noch aktiv: ${_l}")
            endif()
        endif()
    endforeach()
    # state.log: letzter Raum-Abschnitt (Bildzaehler beginnt je Raum neu)
    file(STRINGS "${_dir}/state.log" _sl REGEX "^F[0-9]+ ")
    set(_letzt "")
    set(_vor -1)
    foreach(_z IN LISTS _sl)
        if(_z MATCHES "^F([0-9]+) ")
            if(CMAKE_MATCH_1 LESS _vor)
                set(_letzt "")
            endif()
            set(_vor "${CMAKE_MATCH_1}")
            list(APPEND _letzt "${_z}")
        endif()
    endforeach()
    math(EXPR _r1 "${_r} - 1")
    math(EXPR _k1 "${_ee} - 3")
    set(_n_e 0)
    set(_n_k 0)
    foreach(_z IN LISTS _letzt)
        if(NOT _z MATCHES "^F([0-9]+) ")
            continue()
        endif()
        set(_f "${CMAKE_MATCH_1}")
        if(_f GREATER_EQUAL _p AND _f LESS_EQUAL _r1)
            if(NOT _z MATCHES " cam=${_ecut} ")
                message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - in der Explosions-Einstellung nicht Cut ${_ecut}: ${_z}")
            endif()
            math(EXPR _n_e "${_n_e} + 1")
        elseif(_f GREATER_EQUAL _r AND _f LESS_EQUAL _k1)
            if(NOT _z MATCHES " cam=${_k} ")
                message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Zwischenbild: nach dem Explosionsschnitt nicht Cut ${_k} (Abnahme 2 M1): ${_z}")
            endif()
            math(EXPR _n_k "${_n_k} + 1")
        endif()
    endforeach()
    math(EXPR _soll_e "${_r} - ${_p}")
    if(NOT _n_e EQUAL _soll_e OR _n_k LESS 80)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - state.log unvollstaendig (${_n_e}/${_soll_e} Explosions-, ${_n_k} Einstellungsbilder)")
    endif()
    # Bilder: Kamera je Bild ueber Referenzbilder (keine Schwelle), Folge E+K+
    math(EXPR _b0 "${_p} + 5")
    math(EXPR _b1 "${_r} + 10")
    math(EXPR _re "(${_p} + ${_r}) / 2")
    math(EXPR _rk "${_r} + 45")
    math(EXPR _rx "${_ee} + 15")
    execute_process(COMMAND "${RE15_TOOL}" kamfolge "${_dir}" ${_b0} ${_b1} ${_re} ${_rk} ${_rx}
                    RESULT_VARIABLE _kr OUTPUT_VARIABLE _ko ERROR_VARIABLE _ke)
    string(REGEX MATCH "kamfolge: [^\n]*" _kz "${_ko}")
    if(NOT _kr EQUAL 0)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Bildfolge nicht Explosion -> Schluessel (Abnahme 2 M1): ${_kz}\n${_ko}")
    endif()
    file(GLOB _fd "${_dir}/fd_*.ppm")
    if(_fd)
        file(REMOVE ${_fd})
    endif()
    message(STATUS "r41_gatorszene: Lauf ${TEIL} - Explosion Cut ${_ecut} F${_p}-F${_r1}, Schluessel-Einstellung Cut ${_k} F${_r}-F${_ee} "
                   "(${_halt} Bilder, statt B's Rueckschnitt auf Cut ${_x}), danach Cut ${_x}; state.log ${_n_e}/${_n_k}; ${_kz}")
    return()
endif()

# ---- Nachbesserung 1/2 (M2): Schluessel nach dem Tod sichtbar und pulsierend (Todesrolle / R1, Leon / Elza) ----------------
if(TEIL MATCHES "^tod")
    if(NOT RE15_TOOL OR NOT EXISTS "${RE15_TOOL}")
        file(REMOVE "${_exe_kopie}")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - RE15_TOOL (test_r41_gatorszene) fehlt: '${RE15_TOOL}'")
    endif()
    if(TEIL MATCHES "^tod_r1")
        # FG-E7 (AUFTRAG.md 2026-10-26 01:19): Todeslage ohne Kopf-Klemme; die K3-Wandlage (4430,-26160) ist OFFEN (Release Notes 'Ab Samstag', Behebung Option (a))
        # Rezept K3 (Abnahme F 1, natuerliche Kamera) mit dem ersten Schuss NACH dem Aggro der GB_TEST=5-Marke (Kopf des Skripts,
        # Abschnitt FG-E7): U1.2 (F1-F36, RVD-Streifen 0 -> 1), W29.5 = 885 Bilder warten, Zielen F922-F930, Feuer F931 -> R1 F931
        # "Sperre ohne Schnitt" in Cut 1, Einstellung Cut 0 F969-F1061, zurueck Cut 1. Vorher: "U1.2,(M0.3,MA0.1)x12" (Feuer F46 an
        # der Marke; "x12" = 12 s Kreuz), R1 ende F84, Einstellung F84-F176 (Nachbesserung F 2, Laeufe K4m/KEm), EXIT_AT 300.
        set(_schuss_r1 "U1.2,W29.5")
        foreach(_i RANGE 1 12)
            string(APPEND _schuss_r1 ",M0.3,MA0.1")
        endforeach()
        set(_lauf RE15_GB_TEST=5 "RE15_PLAYER_POS=-1000,-25550,0,0" "RE15_GATOR_KANISTER=4430,-26160" "RE15_GIVE=3:15,21:50"
                  RE15_EQUIP=3 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_schuss_r1}"
                  "RE15_EXIT_AT=1200#${_r2090}" "RE15_FRAMEDUMP=940-1100/2:fd_")
    else()
        # GB_TEST=3: Tod nach 300 Boss-Bildern + Todesrolle Clip 7 (165 Bilder) -> Leiche (Zustand 7) und Schluessel F469,
        # Einstellung F469-F561 (gemessen Nachbesserung F 2, Laeufe T2m/TEm).
        set(_lauf RE15_GB_TEST=3 RE15_R39_MESS_SPIELER_HP=100 "RE15_EXIT_AT=660#${_r2090}" "RE15_FRAMEDUMP=440-660/2:fd_")
    endif()
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
            RE15_DEBUG_JUMP=${_r2090}@250 ${_lauf}
            RE15_STATE_LOG=state.log
            "${_exe_kopie}")
        if(NOT _rv EQUAL 0)
            file(REMOVE "${_exe_kopie}")
            message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL}/${_v} exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT), ${WORKDIR}")
        endif()
    endforeach()
    file(REMOVE "${_exe_kopie}")
    if(TEIL MATCHES "^tod_r1")   # FG-E7: Todeslage ohne Kopf-Klemme, R1 mit der natuerlichen Kamera (Vorbedingung, s. Kopf)
        foreach(_v mit ohne)
            r41f_fge7_ohne_klemme("${_dir}/${_v}" "${TEIL}/${_v}" 1)
        endforeach()
    endif()
    file(STRINGS "${_dir}/mit/debug.log" _lage REGEX "\\[gatorszene\\] Schluessel-Lage: Kamera [0-9]+ ")
    if(NOT _lage)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - keine Zeile 'Schluessel-Lage' (Alligator nicht tot?), ${_dir}/mit")
    endif()
    list(GET _lage 0 _lage0)
    if(NOT _lage0 MATCHES "Kamera ([0-9]+) \\(groesstes Bild, laufende ([0-9]+)\\), Lage [(][-0-9,]+[)] Tiefe ([0-9]+),.*Blink-Spitze ([0-9]+) ")
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Schluessel nicht fuer die Kamera mit dem groessten Bild gelegt: ${_lage0}")
    endif()
    set(_kam "${CMAKE_MATCH_1}")
    set(_lauf_cut "${CMAKE_MATCH_2}")
    set(_tiefe "${CMAKE_MATCH_3}")
    if(CMAKE_MATCH_4 GREATER 128)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Blink-Spitze ${CMAKE_MATCH_4} > 0x80 (Kappung, Plateau): ${_lage0}")
    endif()
    math(EXPR _quad10 "1800 * 208 / ${_tiefe}")      # Quadrat 180 x H / Tiefe, x10 (H = fov 26684 >> 7, RID @0x060)
    if(_quad10 LESS 104)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Schluessel-Quadrat ${_quad10}/10 Bildpunkte < 10,4 (Ankunfts-Schluessel): ${_lage0}")
    endif()
    foreach(_v mit ohne)
        file(STRINGS "${_dir}/${_v}/debug.log" _e REGEX "\\[gatorszene\\] Schluessel-Einstellung: Cut [0-9]+ ")
        file(STRINGS "${_dir}/${_v}/debug.log" _o REGEX "\\[gatorszene\\] Schluessel-Einstellung Ende: Cut [0-9]+ ")
        if(NOT _e OR NOT _o)
            message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL}/${_v} - Einstellung auf den Schluessel fehlt (Beginn '${_e}', Ende '${_o}')")
        endif()
        list(GET _e 0 _e0)
        list(GET _o 0 _o0)
        if(NOT _e0 MATCHES "Cut ([0-9]+) \\(vorher Cut ([0-9]+)\\) F([0-9]+)")
            message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL}/${_v} - Einstellungs-Zeile unlesbar: ${_e0}")
        endif()
        set(_ek_${_v} "${CMAKE_MATCH_1}")
        set(_ev_${_v} "${CMAKE_MATCH_2}")
        set(_es_${_v} "${CMAKE_MATCH_3}")
        if(NOT _o0 MATCHES "Ende: Cut ([0-9]+) F([0-9]+)")
            message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL}/${_v} - Ende-Zeile unlesbar: ${_o0}")
        endif()
        set(_oc_${_v} "${CMAKE_MATCH_1}")
        set(_ee_${_v} "${CMAKE_MATCH_2}")
    endforeach()
    if(NOT _ek_mit EQUAL _kam OR NOT _ev_mit EQUAL _lauf_cut OR NOT _oc_mit EQUAL _lauf_cut)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Einstellung Cut ${_ek_mit} (soll ${_kam}), vorher ${_ev_mit} / zurueck ${_oc_mit} (soll ${_lauf_cut})")
    endif()
    if(NOT _ek_ohne EQUAL _ek_mit OR NOT _es_ohne EQUAL _es_mit OR NOT _ee_ohne EQUAL _ee_mit)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Zwilling ohne Schluessel schneidet anders (Cut ${_ek_ohne} F${_es_ohne}-F${_ee_ohne} gegen Cut ${_ek_mit} F${_es_mit}-F${_ee_mit})")
    endif()
    math(EXPR _halt "${_ee_mit} - ${_es_mit}")
    if(_halt LESS 90 OR _halt GREATER 94)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Einstellung ${_halt} Bilder (soll 90 = GSZ_LEICHE_BILDER + Faden-Ende)")
    endif()
    # state.log: Kamera K in der Einstellung, danach wieder L (Cut_old)
    math(EXPR _k0 "${_es_mit} + 1")
    math(EXPR _k1 "${_ee_mit} - 3")
    math(EXPR _z0 "${_ee_mit} + 1")
    math(EXPR _z1 "${_ee_mit} + 20")
    file(STRINGS "${_dir}/mit/state.log" _sl REGEX "^F[0-9]+ ")
    # Der Bildzaehler beginnt je Raum neu (Startraum bis zum Sprung bei Bild 250, dann 2090): nur der LETZTE Abschnitt zaehlt.
    set(_letzt "")
    set(_vor -1)
    foreach(_z IN LISTS _sl)
        if(_z MATCHES "^F([0-9]+) ")
            if(CMAKE_MATCH_1 LESS _vor)
                set(_letzt "")
            endif()
            set(_vor "${CMAKE_MATCH_1}")
            list(APPEND _letzt "${_z}")
        endif()
    endforeach()
    set(_n_k 0)
    set(_n_z 0)
    foreach(_z IN LISTS _letzt)
        if(NOT _z MATCHES "^F([0-9]+) ")
            continue()
        endif()
        set(_f "${CMAKE_MATCH_1}")
        if(_f GREATER_EQUAL _k0 AND _f LESS_EQUAL _k1)
            if(NOT _z MATCHES " cam=${_kam} ")
                message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - in der Einstellung nicht Cut ${_kam}: ${_z}")
            endif()
            math(EXPR _n_k "${_n_k} + 1")
        elseif(_f GREATER_EQUAL _z0 AND _f LESS_EQUAL _z1)
            if(NOT _z MATCHES " cam=${_lauf_cut} ")
                message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - nach der Einstellung nicht zurueck auf Cut ${_lauf_cut}: ${_z}")
            endif()
            math(EXPR _n_z "${_n_z} + 1")
        endif()
    endforeach()
    if(_n_k LESS 80 OR _n_z LESS 15)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - state.log zu kurz (${_n_k} Einstellungs-, ${_n_z} Folgebilder)")
    endif()
    # Bilder der Einstellung (gerade Nummern, RE15_FRAMEDUMP je 2): ab s+2, bis e-4
    math(EXPR _b0 "${_es_mit} + 2")
    math(EXPR _b0_rest "${_b0} % 2")
    if(_b0_rest)
        math(EXPR _b0 "${_b0} + 1")
    endif()
    math(EXPR _b1 "${_ee_mit} - 4")
    math(EXPR _b1_rest "${_b1} % 2")
    if(_b1_rest)
        math(EXPR _b1 "${_b1} - 1")
    endif()
    execute_process(COMMAND "${RE15_TOOL}" bilddiff "${_dir}/mit" "${_dir}/ohne" ${_b0} ${_b1} 2 10 9
                    RESULT_VARIABLE _br OUTPUT_VARIABLE _bo ERROR_VARIABLE _be)
    string(REGEX MATCH "bilddiff: [^\n]*" _bz "${_bo}")
    if(NOT _br EQUAL 0)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Schluessel in der Einstellung nicht sichtbar/pulsierend/zu klein: ${_bz}\n${_bo}")
    endif()
    message(STATUS "r41_gatorszene: Lauf ${TEIL} - ${_lage0}; Einstellung Cut ${_kam} F${_es_mit}-F${_ee_mit} (${_halt} Bilder), "
                   "danach Cut ${_lauf_cut}; Quadrat ${_quad10}/10 Bildpunkte; ${_bz}")
    return()
endif()

# Bild 600 im DRITTEN 2090-Besuch: der Bildzaehler beginnt je Raum neu; der erste 2090-Besuch (Programm 1) endet nach ~140
# Bildern, der 2080-Besuch nach ~150, der Gator-Ablauf ist ~F416 vorbei (gemessen 2026-10-24, Laeufe s4/lad2).
set(_flucht "")
if(TEIL MATCHES "^flucht")
    # Nachbesserung 5: der Gator beginnt den Kampf im Ostteil des Suedbeckens (Zone 0, gatorszene_r41.c) - weglaufen = nach NORDEN
    # durch den Westkanal: Linksdrehung 0,6 s ab F301 (Kampfbeginn F297, die Szene haelt die Figur bis dahin: Set(2,7,1) / (1,27,1),
    # ROOM1150 sub08 @0x01114/@0x01118), dann Rennen (UP + CROSS) 5 s (gemessen F41n5 N_LW_test: Gier -1728, x -8432, z -25550 ->
    # -16150 in den RE2-Bildern). Nachbesserung 4: W13.5,UX4.6 nach Osten (Kampfbeginn F422).
    set(_flucht RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W10,L0.6,UX5")
endif()
re15_start_spiel(_rv 480
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore
    ${_wahl}
    RE15_SET_FLAG=3:245
    RE15_STATE_LOG=state.log
    RE15_DEBUG_JUMP=2080@250
    "RE15_FIRE_AOT=1@40#${_r2080}"
    "RE15_EXIT_AT=600#${_r2090}"
    ${_flucht}
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
# F<c>..F<c+74> des letzten 2090-Besuchs cam=0 (Abnahme 0: cam=1, Leon hinter der Kamera). Nachbesserung 4: F<c> = Folge C (die
# Wende auf Leon ist laenger, Kampfbeginn F423 statt F405) statt fest F405-F479; dazu Abstand und Bilder bis zum ersten Angriff.
file(STRINGS "${_dir}/debug.log" _ueb REGEX "\\[gatorszene\\] Kamera-Uebergabe: Cut 0 \\(Eintritts-Cut\\), Spielfigur im Bild")
if(NOT _ueb)
    message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - keine Kamera-Uebergabe auf Cut 0 mit Spielfigur im Bild (M1)")
endif()
r41f_folge_c("${_dir}" _c)
r41f_re2soll(_re2_ab2 _re2_bilder _re2_ab2_max _re2_bilder_max)
if(TEIL MATCHES "^flucht")
    set(_mit_cam 0)
else()
    set(_mit_cam 1)
endif()
r41f_kampf("${_dir}" ${_c} ${_re2_ab2} ${_re2_bilder} ${_re2_ab2_max} ${_re2_bilder_max} ${_mit_cam} _ab2 _x0 _xb _biss _treffer _z0 _zb _k)
if(TEIL MATCHES "^flucht")
    math(EXPR _weg "${_zb} - ${_z0}")
    if(_weg LESS 5000)
        message(FATAL_ERROR "r41_gatorszene: Lauf ${TEIL} - Spielfigur nach ${_re2_bilder} Bildern nur ${_weg} nach Norden gelaufen (z ${_z0} -> ${_zb})")
    endif()
    message(STATUS "r41_gatorszene: Lauf ${TEIL} - Kampfbeginn F${_c}, Abstandsquadrat ${_ab2} im RE2-Band ${_re2_ab2}..${_re2_ab2_max}, "
                   "weggelaufen ${_weg} nach Norden (z ${_z0} -> ${_zb} in ${_re2_bilder} Bildern), erster Kontakt/Angriff nach ${_k} "
                   "Bildern >= RE2 ${_re2_bilder} (Biss F${_biss}, Treffer F${_treffer}; -1 = keiner bis EXIT_AT)")
    return()
endif()
set(_status_kampf "Kampfbeginn F${_c}, Abstandsquadrat ${_ab2} im RE2-Band ${_re2_ab2}..${_re2_ab2_max}, Zone 0/0, cam=0 F${_c}-F${_c}+74, erster Kontakt/Angriff nach ${_k} Bildern im RE2-Band ${_re2_bilder}..${_re2_bilder_max} (Biss F${_biss}, Treffer F${_treffer})")
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
               "2080: ${_n_zl} Bilder mit Zombies, keiner westlich x -16650, ${_bewegt} von 8 gelaufen; ${_status_kampf}")
