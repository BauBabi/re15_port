# =============================================================================
# Runde 41 Spur G "kanister2080", Nachbesserung 1 (Abnahme 0 M2 / Nachtrag 23, Orchestrator-Entscheide FG-E3/FG-E4) - die GANZE
# Folge im ECHTEN Ablauf an der exe:
#   Kampf in ROOM2090/2091 -> Treffer bis zur RE2-Schwelle (Messhaken RE15_K2080_GATOR_HP=24990: EIN echter Pistolentreffer statt
#   43, die Schwelle HP < 25000 @0x80101ebc-dc bleibt) -> Kanister-Szene in 2080/2081 (Marvin/John: Hinknien Loco c11 statt
#   Klettern, Zeile 42, Loslassen) -> Fall in 2090/2091 Cut 12 (FG-E4) -> Kanister liegt an der Aufschlagstelle -> DERSELBE
#   Alligator kaempft weiter (FG-E3: Aktor zurueck in seinen Slot, kein Untertauchen/Auftauchen/Intro) -> er nimmt den Kanister
#   auf (Spur B: Fenster gator_re2_r41.c Stufe 0, "KANISTER Stufe 1" / "angehaengt") -> Schuss -> R1-Tod ("R1 ende (Leiche)").
# OHNE RE15_GB_TEST und OHNE RE15_GATOR_KANISTER (beide ausdruecklich geloescht), Leon UND Elza. Leon beginnt im Westkanal bei
# (-6000,-17500) und schiesst (Zielen/Feuern alle ~4 s), wie ein Spieler, der den Kampf fortsetzt. Gemessen (Wurf-Gier 3072,
# Dossier 8.9): Ausloeser F902 bei (-5790,-6245), Aufschlag (-7515,-12304); nach der Rueckkehr Aufnahme F104, R1 F570, Leiche
# F606. EXIT_AT 1500 Bilder nach der Rueckkehr.
# Nachbesserung 2 (Abnahme 1, N3): VIER Laeufe mit ZWEI weiteren Startlagen - Leon (0,-7000) (Kampf im Nordosten, Abnahme-Lauf
# v2/v4) und Elza (-8000,-16000) (Start West, Abnahme-Lauf v3/v5: vorher keine Aufnahme in 4000 Bildern). Je Lauf: Wahl der
# Wurfrichtung ("Wurfrichtung" = der weitere Landepunkt vom Alligator), "ANSCHWIMMEN an" (RE2-Zielmodus, Header
# RE15_K2080_ANSCHWIMM_AB) und die ganze Folge bis zum R1-Tod in 1500 Bildern nach der Rueckkehr.
#
# Nachbesserung 3 (Abnahme 2 M1-M3, Orchestrator-Entscheide G-E1/G-E2, Nachtrag 28) in denselben vier Laeufen:
#   M1 "Tod im Bild": die R1-Kamerazeile (gator_boss.log, G-E1 gator_re2_r41.c gr2_r1_wahl) meldet Kopf und Kiefer im Bild,
#      im Regions-Viereck und unverdeckt und alle Proben des Todes-Clips sichtbar (statt nur "R1 ende (Leiche)").
#   M2 Blick: "Werfer Cut 7" - der Werfer steht der Kamera zugewandt wie Ada (Versatz -111 +- 12 = RUN-Ankunft d < 300
#      `slti 300` @0x80051764 auf 17061 Kamera-Abstand, frontal >= 4023/4096); die Bahn laeuft in der Wurfrichtung (v_z-Vorzeichen).
#   M3 Explosion: RE15_GATOR_FX_LOG - Raum-ESP ROOM40A0 in 2090/2091 registriert (Banken 0x26/0x08/0x09/0x1A), in den R1-Bildern
#      gezeichnete Quads der Raumbanken UND der Kern-Zeilen 485..498 (> 0), keine unbekannten Ops.
#   M3 Kanister: "Kanister-Scherben" (gator_re2_pc.c) - FUN_80036eb4 (@0x80102348) laesst den Kanister zerplatzen: zehn Bilder
#      (R1 P0 + neun P1-Bilder bis +0x16B = 0 @0x8010255c-78) Scherben mit Skala 180..1530 (Tafel @0x8009DC64 Index 0..9) und der
#      flachen Farbe 0x404040 (@0x80036ecc-d0) statt des intakten Meshes.
#   Laufdauer: seit G-E2 stehen die Haende vor dem der Kamera zugewandten Werfer (Gier 3678 statt 3072), der Kanister landet im
#   Suedbecken bei (-6096,-21600) statt (-6821,-22914). Der passive Schuetze (Zielen/Feuern ohne Laufen) trifft den Alligator dann
#   spaeter - gemessen R1 nach der Rueckkehr: kampf_leon F2290, nordost_leon F1383, kampf_elza F568, west_elza F322 (vorher
#   F826/F899/F826/F814); dazwischen zielt die automatische Wahl auf die Spinnen (0x25) in 2090. Daher 60 Schussbloecke (statt 18,
#   die nach gut 2400 Bildern endeten) und EXIT_AT 3000 Bilder nach der Rueckkehr (statt 1500).
#
# Nachbesserung 4 (Abnahme G 3, Maengel M1/M3):
#   M1 Plattform: FUENFTER Lauf "plattform_leon" (Leon auf der Plattform (0,-16000), Lage des Abnahme-Laufs plat_leon) - dort waehlte
#      die R1-Kamera den laufenden Cut 4 mit nur 6/19 sichtbaren Proben des Todes-Clips; die Rangfolge (gator_re2_r41.c
#      re15_gr2_r1_rang) nimmt jetzt den vollen Todes-Clip vor dem gueltigen Start und vor der Massstab-Grenze.
#   M3 Kopf-Klemme: in jedem Lauf liegt der Kanister beim R1 (debug.log "Kanister Stufe 13 pos=") innerhalb der Aussenwaende
#      x[-8900..7200] z[-27000..-5400] und der Spritzring (Kern-Bank 3, re2fx_pc) wird in den R1-Bildern gezeichnet (vorher im
#      natuerlichen Leon-Verlauf 0 Quads: Kanister bei z -29290 jenseits der Suedwand, FUN_8002c820 @0x80077a30 verwarf alle sechs).
#   Laufdauer: mit der Kopf-Klemme kommt der Alligator an Leon/Elza an der Wand nicht mehr mit dem Kopf durch die Wand heran; der
#   passive Schuetze zielt dann laenger auf die Spinnen (0x25, in diesem Zweig noch in 2090 - Spur F entfernt sie) - gemessen R1
#   nach der Rueckkehr: kampf_elza F3766, west_elza F3274 (EXIT_AT 3000 reichte nicht mehr), kampf_leon F814, nordost_leon F1383
#   (vorher), plattform_leon F1204. Daher EXIT_AT 5000 Bilder nach der Rueckkehr.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_kanister2080_kampf.cmake
# =============================================================================
cmake_minimum_required(VERSION 3.21)
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_kanister2080_kampf: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_kanister2080_kampf: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")
# EIGENER EXE-NAME (Muster test_r34n_b_cursor.cmake): nur die eigene Kopie wird geloescht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r41gk_${_lauf_id}.exe")
if(NOT WIN32)
    set(_exe_kopie "${_exe_dir}/re15_pc_r41gk_${_lauf_id}")
endif()
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

function(r41gk_fehler _text)
    file(REMOVE "${_exe_kopie}")
    message(FATAL_ERROR "r41_kanister2080_kampf: ${_text}")
endfunction()

# Testhaken, die den echten Ablauf verdecken wuerden, NICHT vom Aufrufer erben (Abnahme 0 M2/M4)
unset(ENV{RE15_GB_TEST})
unset(ENV{RE15_GATOR_KANISTER})
unset(ENV{RE15_KEIN_GATOR})
unset(ENV{RE15_K2080_WURF_GIER})

set(_basisenv RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore)
set(_leon RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2)
set(_elza RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1)
# Zielen + Feuern (R1 / R1+Quadrat) nach 6 s alle ~4 s, 60 Bloecke (Nachbesserung 3: reicht ueber EXIT_AT 3000 hinaus)
set(_schiess "W6.0")
foreach(_i RANGE 1 60)
    string(APPEND _schiess ",M0.6,MA0.1,M0.3,MA0.1,W3.0")
endforeach()

foreach(_lauf "leon|-6000,-17500,3072|kampf" "elza|-6000,-17500,3072|kampf" "leon|0,-7000,2048|nordost" "elza|-8000,-16000,1024|west"
              "leon|0,-16000,3072|plattform")
    string(REPLACE "|" ";" _lt "${_lauf}")
    list(GET _lt 0 _wer)
    list(GET _lt 1 _start)
    list(GET _lt 2 _lname)
    set(WORKDIR "${_basis}/${_lname}_${_wer}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    if(_wer STREQUAL "leon")
        set(_wahl ${_leon})
        set(_r80 2080)
        set(_r90 2090)
        set(_npc "Marvin 0x40")
    else()
        set(_wahl ${_elza})
        set(_r80 2081)
        set(_r90 2091)
        set(_npc "John 0x49")
    endif()
    re15_start_spiel(_rv 900 ${_basisenv} ${_wahl}
        RE15_DEBUG_JUMP=2090@250 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:60" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100
        RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_schiess}"
        RE15_STATE_LOG=state.log "RE15_PLAYER_POS=${_start}" "RE15_EXIT_AT=5000#${_r90}" RE15_GATOR_FX_LOG=1 "${_exe_kopie}")
    if(NOT _rv EQUAL 0)
        r41gk_fehler("Lauf ${_lname}_${_wer} exe exit=${_rv}")
    endif()
    # ---- Szene (2080/2081) und Rueckweg ----
    foreach(_muster
            "FG-E3: Alligator gesichert Slot [0-9]+ "
            "AUSLOESER geschwaecht .*-> ROOM${_r80}"
            "ROOM${_r80} SZENE \\(${_npc}, 8 lebende Zombies"
            "Kanister aufgehoben \\(Loco c11 Bild 24, Hinknien ROOM11B0 sub06 @0x01650\\)"
            "Wurf: Start .* -> Landung \\(-?[0-9]+,-?[0-9]+,-?[0-9]+\\) nach [0-9]+ Bildern \\(Wasser 2090 y -1620\\)"
            "FG-E3: ROOM${_r90} derselbe Alligator in Slot [0-9]+ "
            "ROOM${_r90} RUECKKEHR: .*Zone-7-Tot-Bits 236\\.\\.243 gesetzt: 8 von 8"
            "Aufschlag im Wasser bei "
            "ROOM${_r90} Kanister im Becken \\(-?[0-9]+,-1200,-?[0-9]+\\) = Aufschlagstelle"
            "Wurfrichtung (3072|1024) \\((Nord|Sued)becken, Alligator beim Ausloesen"
            "ANSCHWIMMEN an: Gator Slot [0-9]+ "
            "HALS zum Kanister: Part 5 "
            "HALS zurueck \\(Akku -> 0\\)")
        file(STRINGS "${WORKDIR}/debug.log" _z REGEX "\\[k2080\\] F[0-9]+ ${_muster}")
        if(NOT _z)
            r41gk_fehler("Lauf ${_lname}_${_wer}: keine Zeile '${_muster}'")
        endif()
    endforeach()
    # Nachtrag 24b: Hinknien/Aufstehen (Plc_motion(1,11) zweimal), kein Klettern (1,2) und kein RUN-Gait als "Tragen" (2,0)
    file(STRINGS "${WORKDIR}/debug.log" _kn REGEX "\\[scd\\] Plc_motion\\(entity=1, motion=11, ")
    list(LENGTH _kn _n_kn)
    file(STRINGS "${WORKDIR}/debug.log" _kl REGEX "\\[scd\\] Plc_motion\\(entity=1, motion=2, |\\[scd\\] Plc_motion\\(entity=2, motion=0, ")
    if(NOT _n_kn EQUAL 2 OR _kl)
        r41gk_fehler("Lauf ${_lname}_${_wer}: ${_n_kn} x Plc_motion(1,11) statt 2 bzw. Kletter-/Lauf-Clip im Szenen-Programm (${_kl})")
    endif()
    # FG-E4: der Fall laeuft in Cut 12
    file(STRINGS "${WORKDIR}/state.log" _c12 REGEX " cam=12 ")
    if(NOT _c12)
        r41gk_fehler("Lauf ${_lname}_${_wer}: Cut 12 nie im Bild (FG-E4)")
    endif()
    # ---- weiterer Kampf nach der Rueckkehr (gator_boss.log, Reihenfolge ab "KANISTER gesetzt") ----
    file(STRINGS "${WORKDIR}/gator_boss.log" _gb REGEX
         "GATOR VERSCHWUNDEN|RE2 AUFTAUCHEN|INTRO ende|KANISTER gesetzt|KANISTER Stufe 1 |KANISTER angehaengt|R1 start|R1 ende \\(Leiche\\)")
    set(_stand 0)
    set(_aufnahme "")
    foreach(_l IN LISTS _gb)
        if(_stand GREATER 0 AND _l MATCHES "GATOR VERSCHWUNDEN|RE2 AUFTAUCHEN|INTRO ende")
            r41gk_fehler("Lauf ${_lname}_${_wer}: nach der Szene ein neuer Alligator/Intro ('${_l}') - FG-E3 verlangt denselben")
        endif()
        if(_stand EQUAL 0 AND _l MATCHES "KANISTER gesetzt")
            set(_stand 1)
        elseif(_stand EQUAL 1 AND _l MATCHES "KANISTER Stufe 1 ")
            set(_stand 2)
            set(_aufnahme "${_l}")
        elseif(_stand EQUAL 2 AND _l MATCHES "KANISTER angehaengt")
            set(_stand 3)
        elseif(_stand EQUAL 3 AND _l MATCHES "R1 start")
            set(_stand 4)
        elseif(_stand EQUAL 4 AND _l MATCHES "R1 ende \\(Leiche\\)")
            set(_stand 5)
        endif()
    endforeach()
    set(_schritte "Kanister im Becken (KANISTER gesetzt)" "Aufnahme (KANISTER Stufe 1)" "angehaengt" "R1 start" "R1 ende (Leiche)")
    if(NOT _stand EQUAL 5)
        list(GET _schritte ${_stand} _fehlt)
        r41gk_fehler("Lauf ${_lname}_${_wer}: Folge nach der Szene bricht ab vor '${_fehlt}' (Stand ${_stand} von 5)")
    endif()
    # ---- Nachbesserung 3, M1 (G-E1): der R1-Tod im Bild ----
    file(STRINGS "${WORKDIR}/gator_boss.log" _r1k REGEX "KAMERA R1 (EXPLOSION RE2-Cut 5 -> Cut [0-9]+|Sperre ohne Schnitt).*G-E1")
    if(NOT _r1k)
        r41gk_fehler("Lauf ${_lname}_${_wer}: keine R1-Kamerazeile mit G-E1 (gator_boss.log)")
    endif()
    list(GET _r1k -1 _r1l)
    if(NOT _r1l MATCHES "Kopf ok, Kiefer ok, Todes-Clip ([0-9]+)/([0-9]+) Proben")
        r41gk_fehler("Lauf ${_lname}_${_wer}: Kopf/Kiefer im R1-Schnitt nicht sichtbar: '${_r1l}'")
    endif()
    if(NOT CMAKE_MATCH_1 EQUAL CMAKE_MATCH_2 OR CMAKE_MATCH_2 LESS 10)
        r41gk_fehler("Lauf ${_lname}_${_wer}: Todes-Clip nur ${CMAKE_MATCH_1} von ${CMAKE_MATCH_2} Proben im Bild: '${_r1l}'")
    endif()
    # ---- Nachbesserung 3, M2 (G-E2): Werfer der Kamera zugewandt, Bahn in der Wurfrichtung ----
    file(STRINGS "${WORKDIR}/debug.log" _bl REGEX "\\[k2080\\] F[0-9]+ Werfer Cut 7: Gier [0-9]+, Gier zur Kamera [0-9]+, Versatz -?[0-9]+, frontal [0-9]+/4096")
    if(NOT _bl)
        r41gk_fehler("Lauf ${_lname}_${_wer}: keine Zeile 'Werfer Cut 7' (M2)")
    endif()
    list(GET _bl 0 _bll)
    string(REGEX MATCH "Versatz (-?[0-9]+), frontal ([0-9]+)/4096" _m "${_bll}")
    set(_vers ${CMAKE_MATCH_1})
    set(_fr ${CMAKE_MATCH_2})
    math(EXPR _vabw "${_vers} + 111")
    if(_vabw LESS -12 OR _vabw GREATER 12 OR _fr LESS 4023)
        r41gk_fehler("Lauf ${_lname}_${_wer}: Werfer nicht wie Ada der Kamera zugewandt (Versatz ${_vers}, frontal ${_fr}; RE2 -111 / 4034)")
    endif()
    file(STRINGS "${WORKDIR}/debug.log" _wb REGEX "\\[k2080\\] F[0-9]+ Wurf: Start .* v=\\(-?[0-9]+,-?[0-9]+,-?[0-9]+\\) .* Wurfrichtung (1024|3072)")
    if(NOT _wb)
        r41gk_fehler("Lauf ${_lname}_${_wer}: keine Wurf-Zeile mit Wurfrichtung (M2)")
    endif()
    list(GET _wb 0 _wbl)
    string(REGEX MATCH "v=\\(-?[0-9]+,-?[0-9]+,(-?[0-9]+)\\) .* Wurfrichtung ([0-9]+)" _m "${_wbl}")
    if((CMAKE_MATCH_2 EQUAL 1024 AND NOT CMAKE_MATCH_1 LESS 0) OR (CMAKE_MATCH_2 EQUAL 3072 AND NOT CMAKE_MATCH_1 GREATER 0))
        r41gk_fehler("Lauf ${_lname}_${_wer}: Bahn nicht in der Wurfrichtung (v_z ${CMAKE_MATCH_1}, Wurfrichtung ${CMAKE_MATCH_2}): '${_wbl}'")
    endif()
    # ---- Nachbesserung 3, M3 (Nachtrag 28): Raum-ESP ROOM40A0 registriert, Explosions-Quads gezeichnet ----
    file(STRINGS "${WORKDIR}/debug.log" _rg REGEX "\\[gator-fx\\] F[0-9]+ ROOM${_r90}: Raum-ESP ROOM40A0 -> re2fx_register_raum rc=0 \\(Banken 0x26 1, 0x08 1, 0x09 1, 0x1A 1\\)")
    if(NOT _rg)
        r41gk_fehler("Lauf ${_lname}_${_wer}: Raum-ESP ROOM40A0 in ${_r90} nicht registriert (M3)")
    endif()
    file(STRINGS "${WORKDIR}/debug.log" _fx REGEX "\\[gator-fx\\] F[0-9]+ ROOM${_r90} R1 1 reg40A0 1 ")
    set(_fx_raum 0)
    set(_fx_kern 0)
    set(_fx_unb 0)
    foreach(_l IN LISTS _fx)
        if(_l MATCHES "gezeichnet G [0-9]+ \\(kern485 ([0-9]+) raum ([0-9]+)\\) re2fx_pc [0-9]+ \\| op_unbekannt ([0-9]+)")
            if(CMAKE_MATCH_1 GREATER _fx_kern)
                set(_fx_kern ${CMAKE_MATCH_1})
            endif()
            if(CMAKE_MATCH_2 GREATER _fx_raum)
                set(_fx_raum ${CMAKE_MATCH_2})
            endif()
            if(CMAKE_MATCH_3 GREATER _fx_unb)
                set(_fx_unb ${CMAKE_MATCH_3})
            endif()
        endif()
    endforeach()
    list(LENGTH _fx _n_fx)
    if(_n_fx EQUAL 0 OR _fx_raum EQUAL 0 OR _fx_kern EQUAL 0 OR NOT _fx_unb EQUAL 0)
        r41gk_fehler("Lauf ${_lname}_${_wer}: Explosion in ${_n_fx} R1-Bildern: hoechstens ${_fx_raum} Raum-Quads, ${_fx_kern} Kern-485-Quads, ${_fx_unb} unbekannte Ops (M3)")
    endif()
    # ---- Nachbesserung 3, M3: der Kanister zerplatzt (FUN_80036eb4 -> Objekt-Zeichner FUN_800363f0 -> FUN_8002d3c8/d718) ----
    file(STRINGS "${WORKDIR}/debug.log" _sch REGEX "\\[gator-re2\\] Kanister-Scherben F[0-9]+: Bild [0-9]+ Skala [0-9]+ Farbe 0x404040, ")
    set(_sch_ist "")
    foreach(_l IN LISTS _sch)
        string(REGEX MATCH "Skala ([0-9]+)" _m "${_l}")
        list(APPEND _sch_ist ${CMAKE_MATCH_1})
    endforeach()
    if(NOT "${_sch_ist}" STREQUAL "180;354;522;684;840;990;1134;1272;1404;1530")
        r41gk_fehler("Lauf ${_lname}_${_wer}: Kanister-Scherben '${_sch_ist}' statt zehn Bilder 180..1530 (Tafel @0x8009DC64, M3)")
    endif()
    # ---- Nachbesserung 4, M3: Kanister beim R1 innerhalb der Aussenwaende (Kopf-Klemme), Spritzring gezeichnet ----
    file(STRINGS "${WORKDIR}/debug.log" _k13 REGEX "\\[gator-re2\\] Kanister Stufe 13 pos=\\(-?[0-9]+,-?[0-9]+,-?[0-9]+\\)")
    if(NOT _k13)
        r41gk_fehler("Lauf ${_lname}_${_wer}: keine Zeile 'Kanister Stufe 13 pos=' (M3)")
    endif()
    list(GET _k13 -1 _k13l)
    string(REGEX MATCH "pos=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)" _m "${_k13l}")
    set(_kx ${CMAKE_MATCH_1})
    set(_kz ${CMAKE_MATCH_3})
    if(_kx LESS -8900 OR _kx GREATER 7200 OR _kz LESS -27000 OR _kz GREATER -5400)
        r41gk_fehler("Lauf ${_lname}_${_wer}: Kanister beim R1 (${_kx},${_kz}) ausserhalb der Aussenwaende x[-8900..7200] z[-27000..-5400] (M3)")
    endif()
    set(_ring 0)
    foreach(_l IN LISTS _fx)
        if(_l MATCHES "re2fx_pc ([0-9]+) \\| op_unbekannt" AND CMAKE_MATCH_1 GREATER _ring)
            set(_ring ${CMAKE_MATCH_1})
        endif()
    endforeach()
    if(_ring EQUAL 0)
        r41gk_fehler("Lauf ${_lname}_${_wer}: Spritzring (re2fx_pc) in keinem R1-Bild gezeichnet (M3)")
    endif()
    message(STATUS "r41_kanister2080_kampf ${_lname}_${_wer} (Start ${_start}): Szene (${_npc}, Hinknien) -> Fall Cut 12 -> derselbe Alligator -> ${_aufnahme} -> R1-Tod; R1-Kamera '${_r1l}'; Werfer Versatz ${_vers} frontal ${_fr}; Explosion bis ${_fx_raum} Raum- + ${_fx_kern} Kern- + ${_ring} Spritzring-Quads in ${_n_fx} R1-Bildern; Kanister beim R1 (${_kx},${_kz}); Kanister-Scherben ${_sch_ist}")
endforeach()

file(REMOVE "${_exe_kopie}")
message(STATUS "r41_kanister2080_kampf: Leon und Elza, vier Startlagen (mit Plattform) gruen")
