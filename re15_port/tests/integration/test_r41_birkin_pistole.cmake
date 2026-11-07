# =============================================================================
# ENDKAMPF MIT DER STARTWAFFE, Runde 41 Spur R, Abnahme 1 (analysis/befunde_runde41/R_abnahme_1.md M1-M4).
#
# Ein Spieler muss jede Form mit der Pistole durch reinen Dauerbeschuss besiegen koennen, ohne Feuerpause. Vor der
# Nachbesserung ging das nicht: der Flinch setzte +0x93 Bit 0 am Clip-Ende und loeschte es nie (M2, Original setzt je
# Bild @0x8011a39c und loescht @0x8011a3c0), Birkin blieb bei HP 155 unverwundbar; ueber Dauerfeuer startete die
# Mutation endlos neu (M3, Tor @0x801164e8 schliesst @0x80118054).
#
# Je ein Lauf Leon (ROOM5090) und Elza (ROOM5091), seit Runde 41 Spur GR NB3 mit dem Messhaken RE15_R39_MESS_SPIELER_HP (unten):
# Debug-Sprung, Tuer 2, laufen, Zielen und Feuer gehalten. RE15_GIVE liefert nur die Patronen. Geprueft aus state.log/debug.log:
#   1  Stufe 3 (analysis/befunde_runde41/form/STUFE3.md): KEINE Mutation mehr in 5090 (Sub 6 nie), die Riesenform ist ab
#      der Raumladung da
#   1b Runde 41 Spur GR NB3: Form 1 (EM30) startet mit HP 600 = 0x12c @0x8011690c x RE15_BIRKIN_HP_FAKTOR 2 (PORT-WAHL, Nutzer F1)
#   2  debug.log "Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips" (BAHNSTEIG_FORM.EMD), die Form kommt mit HP 1000 und laeuft
#      (Clip 10, Clip 5); 1000 = RE2-G4-Ctor `addiu v0,zero,1000` @0x80100678 / `sh v0,342(s1)` @0x8010067c (Runde 41 Spur GR
#      NB3, Nutzer F2; vorher 150)
#   3  Die Form nimmt weiter Schaden: HP faellt unter 100
#   4  Rueckstoss-Szene (Clip 13 Taumeln, Nachbesserung 1; HP 1), Sturz aus dem Wagen (Clip 12, Stufe 3), danach Typ 0x36 mit HP 600 (G5)
#   5  Abnahme 2: der G5 faellt unter derselben Pistole (HP < 0), der Spieler lebt dabei noch. Bis zur Nachbesserung frass er den
#      gesunden Spieler bei F2879 kurz vor dem Ziel (Polaritaet `bgtz` @0x801008e0 / @0x80103e80).
#   5b G5-Rueckzug (Integration Runde 41, Gegenpruefung Welle 1 B1): vom Ende der G5-Szene (state.log pm 2 -> nicht 2 nach dem
#      G5-Auftritt; g5auftritt_r41.c Szenenende @0x0197C Plc_ret, Freigabe @0x01983) bis zum Bild des G5-Todes trifft den Spieler
#      nichts: keine Spieler-HP unter der des Vorbildes und keine unter der am Szenenende. Pruefung 5 allein sieht den Biss mit dem
#      Messhaken nicht mehr (Abschnitt GR NB3 unten); 5b prueft, was der G5A-Rueckzug leisten soll.
#
# G5-AUFTRITT NACHBESSERUNG 2 (analysis/befunde_runde41/G5_auftritt.md Abschnitt 8): nach der G5-Szene geht der Spieler ZURUECK
# (Taste unten, 12 s ab F2548 in G5As Zeitachse, im Integrationsstand ab F4540, s. u.; wirkt erst mit der Freigabe am Szenenende)
# und schiesst dann weiter. Der Test fuehrte den Spieler bis
# Nachbesserung 1 im G5-Kampf STEHEND - das trug nur, weil ihn das Szenenende 715 neben der RE2-Linie liess (z -22855 statt Pos_set
# -23570 @0x0196C `32 00 02 3a 00 00 ee a3`) und die Arme ihn danach aus dem Bissbogen schoben. Auf RE2s Linie beisst der G5 jeden
# Spieler, der innerhalb 6000 vor ihm steht (`sltiu 0x1770` @0x80103e08, Bogen 128 @0x80103e2c-3c, 40 Schaden a0 @0x80104118), und
# frisst ihn bei HP 0 (@0x80103e78-ec4) - Halbierung (Dossier 8.1): jede Kombination der NB1-Aenderungen ausser "alle aus" endet so.
# Der Zug bringt den G5 nur bis X < 12000 (@0x80100fd0-e4), der Biss-Vorstoss nur bis 12000 (@0x80104060-94): wer hinter X 18000
# steht, wird nie gebissen. Der Spieltest misst also weiter "Pistole besiegt den G5", mit dem Abstand, den RE2 dafuer vorsieht.
#
# INTEGRATION RUNDE 41 (GR + G5A, gemessen im Integrationsbaum mit Dauerfeuer ohne Rueckzug, Leon 5090 = Elza 5091): mit den
# GR-HP (Form 1 600, Riesenform 1000) liegt die G5-Szene 1992 Bilder spaeter als in G5A NB2: Mutation F1274, Rueckstoss F3431,
# Vorlauf F3550, Armierung F3605 (55 Vorlaufbilder), Szenenende F4767 (Bild 1162, Spieler x 13873 wie in NB2). Der Rueckzug
# (12 s Taste unten) beginnt deshalb bei F4540 = F2548 + 1992 (34 x MA4 + W2.4 statt 18 x MA4) und wirkt wie in NB2 genau
# 133 Bilder nach dem Szenenende (F4767..F4900). Endbild 6500 (G5A: G5 tot 867 Bilder nach dem Rueckzugsende -> ~F5770, + ~700).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r41_birkin_pistole.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r41_birkin_pistole: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r41_birkin_pistole: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name neben der exe (Asset-Wurzel = exe-Verzeichnis): fremde Kills treffen ihn nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r41pistole_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r41pistole_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# Runde 41 Spur GR NB3: die Formen tragen mehr HP (Form 1 600, Riesenform 1000) - bei einem Pistolenschuss alle 22 Bilder
# brauchen sie 27 + 67 statt 14 + 11 Treffer; Endbild und Zeitlimit wachsen mit. Gemessen (Dossier GR_granate.md N3.5): Leon
# und Elza gleich - Mutation F1274, Riesenform F1455, Rueckstoss F3431, G5 F3594, G5 tot F4706 (alte Szene). Integration Runde 41
# mit der G5A-Szene (1162 Bilder ab Armierung F3605) und Rueckzug: Szenenende F4767, Rueckzug bis F4900, G5 tot ~F5770 -> Endbild 6500.
if(NOT DEFINED _exit_bild)
    set(_exit_bild 6500)
endif()
# Runde 41 Spur GR NB3: ein STEHENDER Skript-Schuetze ohne Ausweichen und Heilen ueberlebt die laengere Form nicht mehr - gemessen
# ohne Haken (Lauf nb3_pistole_ohne_haken, Leon): Form 1 600 -> Mutation F1274, Form F1455 mit 1000, Spieler tot F2100 bei
# Form-HP 700 (Griff Clip 27 F1608 = -35, Hiebe/Biss danach). Wie im Gorilla-Messlauf Runde 39 (ROOM1141) hebt deshalb der
# Messhaken RE15_R39_MESS_SPIELER_HP=100 (engine/src/gegner_r39.c, KEIN Spielverhalten, nur mit Env) die Spieler-HP am Kopf
# jedes KI-Bildes auf 100; negative HP hebt er nicht (gegner_r39.c r39_mess_spieler_hp `pl->hp < 0` -> return). Das Fressen
# des G5 setzt die HP direkt auf -1 (g5_fressen_beginn, enemy_ai_boss_g5.c): der Polaritaetsfehler aus Abnahme 2 (gesunder
# Spieler gefressen) toetet also weiter. Leer setzen (-D_spieler_hp=) = Lauf ohne Haken.
# GEGENPRUEFUNG WELLE 1 B1: mehr als den Polaritaetsfehler sieht Pruefung 5 mit dem Haken nicht. Ein Biss nimmt im Port 20 (RE2 EM36
# `addiu a0,zero,40` @0x80104118, halbiert ueber RE15_G5_HP in enemy_ai_boss_g5.c: RE2 200 HP, Port 100), gefressen wird nur bei
# Spieler-HP <= 0 (`lh v0,-690(v0)` = 0x800cfd4e @0x80103e78, `bgtz v0` @0x80103e80); der Haken stellt vor jedem KI-Bild 100 her,
# also endet jede Biss-Folge bei 80 und der Hauptfall des G5A-Rueckzugs (gebissen bis 0, gefressen) kann nicht eintreten.
# Pruefung 5b misst deshalb den Treffer selbst: state.log schreibt das Bildende NACH re15_game_step (main.c, PARITY STATE-LOG), der
# Haken laeuft am KOPF von re15_enemy_ai_run_all - ein Abzug steht also im Bild des Treffers im Protokoll (gemessen lauf_38551e8c:
# 23 Bilder HP 90/50 vor dem Szenenende, im Folgebild wieder 100).
if(NOT DEFINED _spieler_hp)
    set(_spieler_hp "RE15_R39_MESS_SPIELER_HP=100")
endif()
# Gegenprobe B1 (Messlauf, nicht der Riegel): -D_rueck_ab=<n> = Rueckzug nach n statt 34 Abschnitten MA4 (34 = F4540; jeder Abschnitt
# 120 Bilder), -D_rueck_s=0 = kein Rueckzug, -D_laeufe=leon = nur der Leon-Lauf. Gemessen 2026-11-07 (Leon, Integrationsstand 9867e3913,
# je eine Kopie der exe): ohne Rueckzug 8 Treffer nach dem Szenenende F4767 (erster F5105 100 -> 80 bei x 14386, kleinster x-Abstand
# zum G5 4548 < 6000 = `sltiu v0,v0,0x1770` @0x80103e08), G5 tot F5876; Rueckzug 1200 Bilder spaeter (-D_rueck_ab=44) 8 Treffer, erster
# F5105, G5 tot F6219; Rueckzug auf G5As alter Zeitachse (-D_rueck_ab=18, liegt in der Formphase) 7 Treffer nach dem Szenenende F5310.
# In allen drei Laeufen ist 5b rot und Pruefung 5 gruen ("Spieler-HP dabei 100"). Mit Rueckzug (Standard): 0 Treffer, x-Abstand >= 10725.
if(NOT DEFINED _rueck_ab)
    set(_rueck_ab 34)
endif()
if(NOT DEFINED _rueck_s)
    set(_rueck_s 12)
endif()
if(NOT DEFINED _laeufe)
    set(_laeufe leon elza)
endif()
# Patronen (GR NB3): die Menge ist ein Byte (re15_inventory.h `uint8_t qty`, main.c RE15_GIVE `(uint8_t)qt`) - das fruehere
# 3:400 lud 144 (400 & 0xff); der Skript-Schuetze laedt nicht nach (Kasten 21 bleibt liegen). Mit 144 war die Waffe gemessen
# bei F4356 leer (G5 HP 330, Lauf mit Haken). 3:255 = das Byte-Maximum.
set(_f "")
function(ein_lauf _name _raum)
    set(WORKDIR "${_wurzel}/${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_extra "")
    if(_raum STREQUAL "5091")
        set(_extra "RE15_TITLE_SHOT_CHAR=1")
    endif()
    # Zielen + Feuer gehalten (R1 + Quadrat) bis F4540, dann 12 s zurueck (D), danach wieder Zielen + Feuer; die Pistole schiesst
    # alle 22 Bilder. Gemessen (G5A Nachbesserung 2, Form-HP 150): Szenenende F2775, Spieler 13873 -> 23113, G5 600 -> 15 bis F3774
    # ohne Treffer am Spieler, 40 Schuesse a 15. Integration Runde 41 (GR-HP): Szenenende F4767, Rueckzug ab F4540 = F388 + 34 x 120
    # + 72 (W2.4). In der Rueckstoss-Szene (ab F3431) feuert der Spieler selbst weiter (re15_bf_pad); ab dem Vorlauf F3550 sperrt
    # die G5-Szene die Eingabe (sub06 @0x01778 Set(2,7,1), g5auftritt_r41.c), die Schritte bis zum Szenenende F4767 wirken nicht.
    set(_skript "A0.1,W1,U8.5")
    foreach(_i RANGE 1 ${_rueck_ab})
        string(APPEND _skript ",MA4")
    endforeach()
    string(APPEND _skript ",W2.4")
    if(_rueck_s GREATER 0)
        string(APPEND _skript ",D${_rueck_s}")
    endif()
    # Nachbesserung G5 3: nach jedem 4-s-Abschnitt R1 kurz los (W0.2), damit das naechste Heben neu zielt. Die Zielhilfe laeuft nur im
    # Heben (Latch FUN_8003703c rundum, Radius 0x7530 @0x80032fac, Drehung FUN_8001a8f8 0xC8 @0x80032fe0-ec, player_common.c RAISE).
    # Gemessen (Lauf mit RE2-Vorkampf-Puls, eine Zufallsziehung @0x80104544 mehr): ein Arm peitscht den Spieler bei x 23113 (F3576 HP
    # 45 -> 38, F3618 -> 31), der Blick dreht 2055 -> 640; bei durchgehend gehaltenem R1 zielte er danach bis F5400 ins Leere, G5 blieb
    # bei 150 HP. NB2 8.9 hatte genau diese Annahme ("kein Treffer bei 23113") als ungemessen offen gelassen.
    foreach(_i RANGE 1 60)
        string(APPEND _skript ",MA4,W0.2")
    endforeach()
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 ${_extra}
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_PLAYER_POS=500,-9175,2048,0"
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=${_skript}"
        RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log "RE15_GIVE=3:255,21:250" RE15_EQUIP=3
        ${_spieler_hp}
        "RE15_EXIT_AT=${_exit_bild}#${_raum}"
        "${_exe_kopie}")
    set(_g "")
    if(NOT EXISTS "${WORKDIR}/debug.log" OR NOT EXISTS "${WORKDIR}/state.log")
        set(_f "${_f}\n ${_name}: kein Protokoll (exit=${_rv})" PARENT_SCOPE)
        return()
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    if(NOT _dbg MATCHES "Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips")
        string(APPEND _g "\n ${_name}: Riesenform nicht geladen (debug.log, BAHNSTEIG_FORM.EMD)")
    endif()
    # Abnahme 3 N3: der NPC-Zweig des Zeichners verwirft die Rueckseiten der Birkin-Formen (FUN_80027bec, re15_nclip_r41_weg).
    if(NOT _dbg MATCHES "nclip-r41")
        string(APPEND _g "\n ${_name}: Zeichner ruft den Rueckseiten-Verwurf nicht (N3)")
    endif()
    file(STRINGS "${WORKDIR}/state.log" _zeilen REGEX " t=3[06] st=")
    set(_mut 0)
    set(_form_hp -1)
    set(_form1_max -1)
    set(_bild_mut -1)
    set(_bild_form -1)
    set(_bild_rueck -1)
    set(_bild_g5 -1)
    set(_bild_g5_tot -1)
    set(_mg_start "")
    set(_mg_g5_tot "?")
    set(_form_min 9999)
    set(_rueck 0)
    set(_g5 0)
    set(_war_mut 0)
    set(_clip10 0)
    set(_clip5 0)
    set(_g5_tot 0)
    set(_pl_hp_tod -999)
    # 5b: Spieler vom Szenenende bis zum G5-Tod
    set(_pm_vor "")
    set(_hp_vor "")
    set(_bild_ende -1)
    set(_x_ende "?")
    set(_hp_ende "")
    set(_n_fenster 0)
    set(_treffer 0)
    set(_treffer_erst "")
    set(_hp_min_fenster 9999)
    set(_dx_min "?")
    set(_x_g5tod "?")
    set(_sturz 0)
    foreach(_z IN LISTS _zeilen)
        string(REGEX MATCH "^F([0-9]+) .* mg=(-?[0-9]+) " _fm "${_z}")
        set(_bild "${CMAKE_MATCH_1}")
        set(_mg_jetzt "${CMAKE_MATCH_2}")
        if(_mg_start STREQUAL "")
            set(_mg_start "${_mg_jetzt}")
        endif()
        # 5b: Spieler je Bild (state.log PL(x,z,rot,hp), pm = g_scd.player_mode; die G5-Szene setzt am Ende pm 1, g5auftritt_r41.c)
        set(_pl_x "")
        set(_pl_hp "")
        if(_z MATCHES "PL\\((-?[0-9]+),-?[0-9]+,rot=[0-9]+,hp=(-?[0-9]+)\\)")
            set(_pl_x "${CMAKE_MATCH_1}")
            set(_pl_hp "${CMAKE_MATCH_2}")
        endif()
        set(_pm "")
        if(_z MATCHES " pm=([0-9]+) ")
            set(_pm "${CMAKE_MATCH_1}")
        endif()
        if(_g5 AND _bild_ende LESS 0 AND _pm_vor STREQUAL "2" AND NOT _pm STREQUAL "2")
            set(_bild_ende "${_bild}")
            set(_x_ende "${_pl_x}")
            set(_hp_ende "${_pl_hp}")
        endif()
        # Fenster: Szenenende bis einschliesslich Todesbild (_g5_tot setzt erst der Typ-0x36-Zweig unten)
        if(_bild_ende GREATER_EQUAL 0 AND NOT _g5_tot AND NOT _pl_hp STREQUAL "")
            math(EXPR _n_fenster "${_n_fenster} + 1")
            if(_pl_hp LESS _hp_min_fenster)
                set(_hp_min_fenster "${_pl_hp}")
            endif()
            if(NOT _hp_vor STREQUAL "" AND _pl_hp LESS _hp_vor)
                math(EXPR _treffer "${_treffer} + 1")
                if(_treffer_erst STREQUAL "")
                    set(_treffer_erst "F${_bild} HP ${_hp_vor} -> ${_pl_hp}, Spieler x ${_pl_x}")
                endif()
            endif()
            if(_z MATCHES "t=36 [^]]*@\\((-?[0-9]+),")
                math(EXPR _dx "${_pl_x} - (${CMAKE_MATCH_1})")
                if(_dx_min STREQUAL "?" OR _dx LESS _dx_min)
                    set(_dx_min "${_dx}")
                endif()
            endif()
            set(_x_g5tod "${_pl_x}")
        endif()
        set(_pm_vor "${_pm}")
        set(_hp_vor "${_pl_hp}")
        string(REGEX MATCH "t=(3[06]) st=([0-9]+) ss1=([0-9]+) ss2=([0-9]+) ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) [^]]*\\] hp=(-?[0-9]+)" _m "${_z}")
        if(NOT _m)
            continue()
        endif()
        set(_typ "${CMAKE_MATCH_1}")
        set(_st "${CMAKE_MATCH_2}")
        set(_s1 "${CMAKE_MATCH_3}")
        set(_s2 "${CMAKE_MATCH_4}")
        set(_mo "${CMAKE_MATCH_5}")
        set(_hp "${CMAKE_MATCH_6}")
        if(_typ STREQUAL "36")
            if(_hp GREATER 0)
                if(NOT _g5)
                    set(_bild_g5 "${_bild}")
                endif()
                set(_g5 1)
            elseif(_g5 AND _hp LESS 0 AND NOT _g5_tot)
                set(_bild_g5_tot "${_bild}")
                set(_mg_g5_tot "${_mg_jetzt}")
                set(_g5_tot 1)
                # Spieler-HP im selben Bild (state.log PL(x,z,rot=..,hp=..)): er muss den Tod des G5 erleben
                if(_z MATCHES "PL\\(-?[0-9]+,-?[0-9]+,rot=[0-9]+,hp=(-?[0-9]+)\\)")
                    set(_pl_hp_tod "${CMAKE_MATCH_1}")
                endif()
            endif()
            continue()
        endif()
        if(NOT _war_mut AND _hp GREATER _form1_max)
            set(_form1_max "${_hp}")
        endif()
        if(_st EQUAL 1 AND _s1 EQUAL 6 AND _s2 EQUAL 0)
            math(EXPR _mut "${_mut} + 1")
            if(_bild_mut LESS 0)
                set(_bild_mut "${_bild}")
            endif()
        endif()
        if(_st EQUAL 1 AND _s1 EQUAL 6)
            set(_war_mut 1)
        elseif(_st EQUAL 1 AND _s1 EQUAL 0 AND _mo EQUAL 10)
            set(_clip10 1)
            if(_form_hp LESS 0)
                set(_form_hp "${_hp}")
                set(_bild_form "${_bild}")
            endif()
        endif()
        if(_clip10 AND _mo EQUAL 5)
            set(_clip5 1)
        endif()
        if(_clip10 AND _st EQUAL 1 AND _hp LESS _form_min)
            set(_form_min "${_hp}")
        endif()
        # Nachbesserung 1 M5: die Riesenform taumelt in Phase 1 (Clip 13), Clip 8 nur noch am Platz an der Sturzgrenze
        if(_clip10 AND _st EQUAL 2 AND (_mo EQUAL 13 OR _mo EQUAL 8) AND _hp EQUAL 1)
            if(NOT _rueck)
                set(_bild_rueck "${_bild}")
            endif()
            set(_rueck 1)
        endif()
        if(_rueck AND _st EQUAL 2 AND _mo EQUAL 12 AND _hp EQUAL 1)
            set(_sturz 1)
        endif()
    endforeach()
    if(NOT _mut EQUAL 0)
        string(APPEND _g "\n ${_name}: Mutation ${_mut}-mal gestartet, erwartet 0 (Riesenform ab der Raumladung)")
    endif()
    if(NOT _form1_max EQUAL 600)
        string(APPEND _g "\n ${_name}: Form 1 hat hoechstens HP ${_form1_max}, erwartet 600 (0x12c @0x8011690c x 2, GR NB3)")
    endif()
    if(NOT _form_hp EQUAL 1000)
        string(APPEND _g "\n ${_name}: Form kommt mit HP ${_form_hp}, erwartet 1000 (G4-Ctor @0x8010067c, GR NB3)")
    endif()
    if(NOT _clip5)
        string(APPEND _g "\n ${_name}: die Form laeuft nicht (kein Clip 5 nach dem Auftritt)")
    endif()
    if(_form_min GREATER_EQUAL 100)
        string(APPEND _g "\n ${_name}: Form nimmt keinen Schaden (HP-Minimum ${_form_min})")
    endif()
    if(NOT _rueck)
        string(APPEND _g "\n ${_name}: keine Rueckstoss-Szene")
    endif()
    if(NOT _sturz)
        string(APPEND _g "\n ${_name}: kein Sturz aus dem Wagen (Clip 12)")
    endif()
    if(NOT _g5)
        string(APPEND _g "\n ${_name}: kein G5 (Typ 0x36)")
    endif()
    if(NOT _g5_tot)
        string(APPEND _g "\n ${_name}: G5 faellt unter der Pistole nicht bis F${_exit_bild}")
    elseif(NOT _pl_hp_tod GREATER 0)
        string(APPEND _g "\n ${_name}: Spieler-HP ${_pl_hp_tod} im Bild, in dem der G5 faellt (der Spieler muss leben)")
    endif()
    # 5b (Gegenpruefung Welle 1 B1): der Rueckzug haelt den Spieler aus dem Biss - kein Treffer vom Szenenende bis zum G5-Tod
    if(_bild_ende LESS 0)
        string(APPEND _g "\n ${_name}: kein Ende der G5-Szene im Protokoll (pm 2 -> nicht 2 nach dem G5-Auftritt), 5b ungeprueft")
    elseif(_n_fenster LESS 2)
        string(APPEND _g "\n ${_name}: ${_n_fenster} Bilder zwischen Szenenende F${_bild_ende} und G5-Tod, 5b ungeprueft")
    elseif(_treffer GREATER 0 OR _hp_min_fenster LESS _hp_ende)
        string(APPEND _g "\n ${_name}: ${_treffer} Treffer am Spieler zwischen Szenenende F${_bild_ende} und G5-Tod (erster ${_treffer_erst}; "
                         "kleinste HP ${_hp_min_fenster}, am Szenenende ${_hp_ende}; kleinster x-Abstand zum G5 ${_dx_min}) - der Rueckzug "
                         "haelt ihn nicht aus dem Biss (5b)")
    endif()
    # Patronen am Ende: Magazin (mg=) und Vorrat (Zeile "Patronen" fehlt im Log; Vorrat = RE15_GIVE 21:250 minus Nachladen).
    file(STRINGS "${WORKDIR}/state.log" _letzte REGEX "^F[0-9]+ .* mg=")
    list(LENGTH _letzte _nl)
    set(_mg "?")
    if(_nl GREATER 0)
        math(EXPR _il "${_nl} - 1")
        list(GET _letzte ${_il} _lz)
        if(_lz MATCHES " mg=(-?[0-9]+) ")
            set(_mg "${CMAKE_MATCH_1}")
        endif()
    endif()
    message(STATUS "r41_birkin_pistole ${_name}: Mutationen ${_mut} (F${_bild_mut}), Form 1 HP max ${_form1_max}, Form-HP ${_form_hp} "
                   "(F${_bild_form}) bis ${_form_min}, Rueckstoss ${_rueck} (F${_bild_rueck}), Sturz ${_sturz}, G5 ${_g5} (F${_bild_g5}), G5 tot ${_g5_tot} "
                   "(F${_bild_g5_tot}), Spieler-HP dabei ${_pl_hp_tod}, Patronen in der Waffe ${_mg_start} -> ${_mg_g5_tot} beim G5-Tod, am Ende ${_mg}; "
                   "5b: Szenenende F${_bild_ende} x ${_x_ende} HP ${_hp_ende}, ${_n_fenster} Bilder bis zum G5-Tod, Treffer ${_treffer}, "
                   "kleinste HP ${_hp_min_fenster}, Spieler x beim G5-Tod ${_x_g5tod}, kleinster x-Abstand zum G5 ${_dx_min} "
                   "(Rueckzug nach ${_rueck_ab} x MA4, D${_rueck_s})")
    set(_f "${_f}${_g}" PARENT_SCOPE)
endfunction()

list(FIND _laeufe leon _i_leon)
list(FIND _laeufe elza _i_elza)
if(_i_leon GREATER -1)
    ein_lauf(leon 5090)
endif()
if(_i_elza GREATER -1)
    ein_lauf(elza 5091)
endif()
file(REMOVE "${_exe_kopie}")

if(_f)
    message(FATAL_ERROR "r41_birkin_pistole: FEHLER${_f}")
endif()
message(STATUS "r41_birkin_pistole: alle Pruefungen gruen")
