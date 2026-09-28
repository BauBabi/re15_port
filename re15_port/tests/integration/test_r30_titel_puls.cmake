# =============================================================================
# TITEL-PULS-PIN (Runde 30 / Thema D, Nutzer 2026-09-27: "Im Titelbild bei der
# Auswahl New Game, Load Game, Option blinkt der ausgewaehlte Bereich in der
# Frequenz im Vergleich zum Original zu schnell.")
# Dossier: analysis/befunde_runde30/titel-blinken.md (§9 und §10)
#
# Faehrt die ECHTE re15_pc.exe (beschleunigter Renderer, Massstab 1) und liest die
# Messschiene RE15_TITLE_PULSE_LOG. Eine Zeile je Bild des Titels:
#   <zeit_us> <zaehler> <pulswert> <faellig> <phase> <einblende_tick> <einblende_B>
#   <gezeigt> <gezeichnet> <zeile> <zeilen_hash> <zeilen_summe>
# Die ersten sieben Spalten sind der ZUSTAND DER ENGINE (engine/src/title_pulse.c) zu
# Beginn des Bildes. Die letzten fuenf kommen aus dem ZEICHNER (platform/pc/src/render_pc.c,
# re15_render_pc_title_row_probe) fuer DASSELBE Bild, nachdem re15_render_end_frame es
# gezeichnet hat: das Farbbyte der Textur, die fuer die aktive Zeile gezeichnet wurde, und
# FNV-1a-32 / Summe ueber (r>>3, g>>3, b>>3) der ZURUECKGELESENEN Pixel der Zeilenregion
# 256 x 17 (x 0x20 .. 0x11f, y 0x85 .. 0x95: Rechteck bei y + subtraktiver Schatten bei y+1,
# Zeichner FUN_801027a0 @0x80102810-14), gelesen nach dem Menue und vor den Blenden.
#
# Geprueft wird damit am BILD, nicht am Engine-Zustand:
#  (A) ZEICHNER = ENGINE: in jedem gezeigten Bild ist das gezeichnete Farbbyte der aktiven
#      Zeile der Pulswert der Engine (Original: der Zeichner schreibt den Pulswert @0x80102944
#      als R=G=B des Rechtecks, `lhu t0,0x2944(t0)` @0x80102848, `sb` @0x80102850/54/58).
#  (B) DAS ZEILENBILD HAENGT ALLEIN AM PULSWERT: jeder Pulswert ergibt in allen gezeigten
#      Bildern dieselben Pixel. Ein Zeichner mit eigenem Pulsschritt (so bis d98e9639:
#      +2 je Zeichenaufruf in render_pc.c) zeigt je Pulswert der Engine viele Bilder.
#  (C) GLEICH DEM ORIGINAL: fuer 11 Pulswerte liegt die aktive Zeile NEW GAME aus den
#      Bildpuffern von sechs Original-Savestates vor (Tabelle unten; Werkzeug
#      analysis/befunde_runde30/titel-blinken_tools/r30_nb2_orig_zeilen_hash.py). Jedes
#      gezeigte Bild mit einem dieser Pulswerte muss pixelgleich sein (5 Bit je Kanal).
#      Das sichert die Modulation texel * farbe >> 7 mit Saettigung 31 (Befehl 0x66808080
#      @0x80102830-34; psx-spx GPU "Modulation") und die Doppelbelichtung.
#  (D) PULSPERIODE AM BILD: der Abfall des Zeilenbilds auf seine dunkelste Stufe (Pulswert
#      0x86 -> 0x80, Ruecksetzung `ori a0,zero,0x80` @0x80102928) geschieht einmal je
#      Periode. Soll zwischen zwei Abfaellen: 60 Pulsschritte (0x3c @0x80102918) zu je einem
#      Durchgang der Original-Hauptschleife = 2 VBlanks (DAT_800b5456 := 2 @0x8002130c-14,
#      VSync(a0) @0x8002147c-80) bei 59,826 Hz (psx-spx, NTSC non-interlaced)
#      = 2005816,9 us -> 2005817 us. TOLERANZ = gemessene Bilddauer, keine feste Zahl:
#      ein faelliger Schritt wird erst am Anfang des naechsten Bildes ausgefuehrt.
#      |P - 2005817| <= max(Bilddauer an beiden Abfaellen) + 1 us. Die Pulsschritte der
#      Engine je Periode werden aus den BEOBACHTETEN Zaehleraenderungen gezaehlt
#      ((zaehler - zaehler_davor) mod 60), nicht aus der Spalte <faellig>: Soll 60.
#  (E) PULS RUHT IM BESTAETIGUNGS-FADE: 0 Aenderungen von (Zaehler, Pulswert) und 0
#      Aenderungen des Zeilenbilds. Original: der Fade FUN_80102ccc zeichnet ueber
#      FUN_80102a10 (jal @0x80102d10 / @0x80102d60), und FUN_80102a10 (0x80102a10-0x80102a88)
#      ruft FUN_801028ec NICHT.
#  (F) TITEL-EINBLENDE (Engine-Spalte B): erstes Bild mit B = 0 nach 32 Durchgaengen
#      (Schritt -0x400 `ori a1,zero,0xfc00` @0x80102058, Pegel 0x7fff @0x80021718,
#      Farbe = Pegel >> 7 @0x800218d0) = 1069768,96 us -> 1069769 us <= Dauer
#      <= 1069769 us + Bilddauer.
#
# Ablauf: RE15_TITLE_CONFIRM_MS (Testhaken in main.c, ZEIT statt Bildern) bestaetigt
# NEW GAME nach 8,5 s -> Bestaetigungs-Fade -> Player-Select mit RE15_PSELECT_AUTO ->
# RE15_BOOT_EXIT_AT=1 beendet den Prozess am Beginn des Spiel-Boots.
# ⛔ KEIN RE15_SOFTWARE_RENDER: SDLs Software-Renderer kennt die eigene subtraktive
# Mischart (SDL_ComposeCustomBlendMode, render_pc.c s_shadow_blend) nicht, der Schatten
# kommt dort falsch heraus. Gemessen (Dossier §10): mit RE15_SOFTWARE_RENDER=1 zeigte jeder
# der 32 Pulswerte ZWEI verschiedene Zeilenbilder und 0 von 11 Original-Stufen stimmten; mit
# dem beschleunigten Renderer 1 Bild je Pulswert und 11 von 11 gleich dem Original.
# RE15_WINDOW_SCALE=1: Ausgabe 320 x 240 = logische Pixel 1:1 (die Ruecklese tastet sonst
# die Pixelmitte ab).
#
# GEGENPROBEN (Dossier §10): render_pc.c auf d98e9639 (mit nachgeruesteter Ruecklese) -> ROT
# in (A)(B)(C)(D); eigener Pulsschritt je Zeichenaufruf im neuen render_pc.c -> ROT (A)(B)(D);
# Modulation in tmoji_strip auf die alte Abbildung -> ROT (C); main.c mit einem Pulsschritt
# je Bild statt je Durchgang -> ROT (D).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r30_titel_puls.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r30_titel_puls: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r30_titel_puls: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Eigenes Arbeitsverzeichnis je Aufruf (ein Rest des vorigen Laufs haelt sonst debug.log).
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")

# Soll-Werte (Herleitung im Kopf): 60 x 2 / 59,826 s und 32 x 2 / 59,826 s, aufgerundet auf us.
set(_soll_periode_us 2005817)
set(_soll_einblende_us 1069769)
set(_confirm_ms 8500)          # Testparameter: 8,5 s Titel = 4 Abfaelle / 3 volle Perioden
set(_min_perioden 2)

# (C) ORIGINAL: Pulswert -> FNV-1a-32 der aktiven Zeile NEW GAME (256 x 17 Pixel, 5 Bit je
# Kanal) aus dem Bildpuffer des Original-Savestates (stage_saves/, VRAM-Zeilen 133..149 bzw.
# 373..389, Spalten 32..287). Welcher Pulswert ein Puffer zeigt, bestimmt der Nachbau des
# Zeichners (0 von 4352 Pixeln abweichend fuer genau diesen Wert); der Hash ist aus dem
# ORIGINAL-VRAM gerechnet. Alle sechs Savestates laufen auf der Auslieferungs-EXE
# (@0x80026e4c nicht gepatcht). 0xbc kommt in boot_52 (y0) und nav_down1 (y0) gleich vor.
set(_orig_tab
    "136:452e4901"   # 0x88  boot_48.sav   Puffer y=0
    "138:c2713d44"   # 0x8a  boot_48.sav   Puffer y=240
    "156:74c7e050"   # 0x9c  boot_40.sav   Puffer y=0
    "158:5018e0cb"   # 0x9e  boot_40.sav   Puffer y=240
    "162:b7245ff0"   # 0xa2  mzd_title.sav Puffer y=240
    "164:0b2c163f"   # 0xa4  mzd_title.sav Puffer y=0
    "172:1a2d63c7"   # 0xac  boot_44.sav   Puffer y=0
    "174:0ee16d0d"   # 0xae  boot_44.sav   Puffer y=240
    "186:cb4d35aa"   # 0xba  boot_52.sav   Puffer y=240
    "188:7841f327"   # 0xbc  boot_52.sav   Puffer y=0 (= nav_down1.sav Puffer y=0)
    "190:409b0bdb")  # 0xbe  nav_down1.sav Puffer y=240
foreach(_e IN LISTS _orig_tab)
    string(REPLACE ":" ";" _kv "${_e}")
    list(GET _kv 0 _k)
    list(GET _kv 1 _v)
    set(_orig_${_k} "${_v}")
    set(_orig_gesehen_${_k} 0)
endforeach()

re15_start_spiel(_rv 180
    RE15_NO_INTRO=1
    RE15_NOAUDIO=1
    RE15_WINDOW_SCALE=1
    RE15_TITLE_PULSE_LOG=puls.txt
    RE15_TITLE_CONFIRM_MS=${_confirm_ms}
    RE15_PSELECT_AUTO=1
    RE15_BOOT_EXIT_AT=1
    "${RE15_PC_EXE}")

if(NOT EXISTS "${WORKDIR}/puls.txt")
    message(FATAL_ERROR "r30_titel_puls: kein Pulsprotokoll (RE15_TITLE_PULSE_LOG) geschrieben "
                        "(exit=${_rv}) — die exe kennt die Messschiene nicht oder kam nie in den Titel")
endif()

file(STRINGS "${WORKDIR}/puls.txt" _zeilen
     REGEX "^[0-9]+ [0-9]+ [0-9]+ [0-9]+ [0-9]+ [0-9]+ -?[0-9]+ [01] -?[0-9]+ -?[0-9]+ [0-9a-f]+ [0-9]+$")
file(STRINGS "${WORKDIR}/puls.txt" _alle)
list(LENGTH _zeilen _n_zeilen)
list(LENGTH _alle _n_alle)
if(_n_zeilen LESS 10 OR NOT _n_zeilen EQUAL _n_alle)
    message(FATAL_ERROR "r30_titel_puls: ${_n_zeilen} von ${_n_alle} Zeilen im Format mit 12 Spalten "
                        "(exit=${_rv}) — die exe schreibt die Zeichner-Spalten nicht")
endif()

set(_prev_ph "")
set(_prev_t 0)
set(_prev_ctr 0)
set(_prev_val 0)
set(_prev_hash "")
set(_prev_sum 0)
set(_prev_shown 0)
set(_n_titel 0)
set(_n_fade 0)
set(_n_ungezeigt 0)
set(_n_ungezeigt_fade 0)
set(_bad_val 0)
set(_zeichner_abw 0)         # (A)
set(_zeichner_txt "")
set(_bild_konflikt 0)        # (B)
set(_bild_txt "")
set(_n_bild_werte 0)
set(_orig_gleich 0)          # (C)
set(_orig_abw 0)
set(_orig_txt "")
set(_n_zeile_fremd 0)
set(_smin 999999999)         # (D) dunkelste Stufe des Zeilenbilds (erster Durchgang ueber das Protokoll)
set(_fade_aend 0)            # (E)
set(_fade_bild_aend 0)
set(_seg0_t "")              # (F) erstes Bild des ERSTEN Titel-Abschnitts
set(_einbl_us "")
set(_einbl_dt 0)
set(_dt_min 999999999)
set(_dt_max 0)
set(_first_seg_done 0)

# ---- erster Durchgang: (A)(B)(C)(E)(F), Bilddauern, dunkelste Stufe -------------------------
foreach(_z IN LISTS _zeilen)
    string(REPLACE " " ";" _f "${_z}")
    list(GET _f 0 _t)
    list(GET _f 1 _ctr)
    list(GET _f 2 _val)
    list(GET _f 4 _ph)
    list(GET _f 6 _b)
    list(GET _f 7 _shown)
    list(GET _f 8 _drawn)
    list(GET _f 9 _row)
    list(GET _f 10 _hash)
    list(GET _f 11 _sum)

    if(_shown EQUAL 1)
        # --- (A) Zeichner = Engine
        if(NOT _drawn EQUAL _val)
            math(EXPR _zeichner_abw "${_zeichner_abw} + 1")
            if(_zeichner_abw LESS 4)
                string(APPEND _zeichner_txt "\n    t=${_t}: Engine ${_val}, gezeichnet ${_drawn}")
            endif()
        endif()
        # --- (B) ein Zeilenbild je Pulswert
        if(NOT DEFINED _bild_${_val})
            set(_bild_${_val} "${_hash}")
            math(EXPR _n_bild_werte "${_n_bild_werte} + 1")
        elseif(NOT "${_bild_${_val}}" STREQUAL "${_hash}")
            math(EXPR _bild_konflikt "${_bild_konflikt} + 1")
            if(_bild_konflikt LESS 4)
                string(APPEND _bild_txt "\n    Pulswert ${_val}: Bild ${_hash} statt ${_bild_${_val}} (t=${_t})")
            endif()
        endif()
        # --- (C) Original
        if(_row EQUAL 0)
            if(DEFINED _orig_${_val})
                math(EXPR _orig_gesehen_${_val} "${_orig_gesehen_${_val}} + 1")
                if("${_hash}" STREQUAL "${_orig_${_val}}")
                    math(EXPR _orig_gleich "${_orig_gleich} + 1")
                else()
                    math(EXPR _orig_abw "${_orig_abw} + 1")
                    if(_orig_abw LESS 4)
                        string(APPEND _orig_txt "\n    Pulswert ${_val}: Port ${_hash}, Original ${_orig_${_val}} (t=${_t})")
                    endif()
                endif()
            endif()
        else()
            math(EXPR _n_zeile_fremd "${_n_zeile_fremd} + 1")
        endif()
    endif()

    if(_ph EQUAL 0)
        math(EXPR _n_titel "${_n_titel} + 1")
        if(_val LESS 128 OR _val GREATER 190)
            math(EXPR _bad_val "${_bad_val} + 1")
        endif()
        if(_shown EQUAL 0)
            math(EXPR _n_ungezeigt "${_n_ungezeigt} + 1")
        elseif(_sum LESS _smin)
            set(_smin ${_sum})
        endif()
        if("${_prev_ph}" STREQUAL "0")
            math(EXPR _dt "${_t} - ${_prev_t}")
            if(_dt LESS _dt_min)
                set(_dt_min ${_dt})
            endif()
            if(_dt GREATER _dt_max)
                set(_dt_max ${_dt})
            endif()
            # --- (F) Einblende: erstes Bild mit B = 0 im ersten Titel-Abschnitt
            if(NOT _first_seg_done AND "${_einbl_us}" STREQUAL "" AND _b EQUAL 0)
                math(EXPR _einbl_us "${_t} - ${_seg0_t}")
                set(_einbl_dt ${_dt})
            endif()
        else()
            if("${_seg0_t}" STREQUAL "")
                set(_seg0_t ${_t})
            elseif(NOT "${_prev_ph}" STREQUAL "")
                set(_first_seg_done 1)
            endif()
        endif()
    else()
        # --- (E) Bestaetigungs-Fade: gegen das Bild davor (auch gegen das Bestaetigungsbild)
        math(EXPR _n_fade "${_n_fade} + 1")
        if(_shown EQUAL 0)
            math(EXPR _n_ungezeigt_fade "${_n_ungezeigt_fade} + 1")
        endif()
        if(NOT "${_prev_ph}" STREQUAL "")
            if(NOT _ctr EQUAL _prev_ctr OR NOT _val EQUAL _prev_val)
                math(EXPR _fade_aend "${_fade_aend} + 1")
            endif()
            if("${_prev_ph}" STREQUAL "1" AND _shown EQUAL 1 AND _prev_shown EQUAL 1
               AND NOT "${_hash}" STREQUAL "${_prev_hash}")
                math(EXPR _fade_bild_aend "${_fade_bild_aend} + 1")
            endif()
        endif()
        set(_first_seg_done 1)
    endif()
    set(_prev_ph ${_ph})
    set(_prev_t ${_t})
    set(_prev_ctr ${_ctr})
    set(_prev_val ${_val})
    set(_prev_hash "${_hash}")
    set(_prev_shown ${_shown})
endforeach()

# ---- zweiter Durchgang: (D) Periode aus den BEOBACHTETEN Bildabfaellen ------------------------
set(_prev_ph "")
set(_prev_t 0)
set(_prev_ctr 0)
set(_prev_sum -1)
set(_ev_t "")                # Zeit des letzten Abfalls im laufenden Titel-Abschnitt
set(_ev_dt 0)
set(_steps 0)                # Engine-Pulsschritte seit diesem Abfall (aus Zaehleraenderungen)
set(_n_ev 0)
set(_n_per 0)
set(_n_per_aus 0)
set(_per_txt "")
set(_ev_ohne_reset 0)
foreach(_z IN LISTS _zeilen)
    string(REPLACE " " ";" _f "${_z}")
    list(GET _f 0 _t)
    list(GET _f 1 _ctr)
    list(GET _f 2 _val)
    list(GET _f 4 _ph)
    list(GET _f 7 _shown)
    list(GET _f 11 _sum)
    if(_ph EQUAL 0 AND "${_prev_ph}" STREQUAL "0")
        math(EXPR _dt "${_t} - ${_prev_t}")
        math(EXPR _d "(${_ctr} - ${_prev_ctr} + 60) % 60")
        math(EXPR _steps "${_steps} + ${_d}")
        if(_shown EQUAL 1 AND _prev_sum GREATER_EQUAL 0
           AND _sum EQUAL _smin AND NOT _prev_sum EQUAL _smin)
            math(EXPR _n_ev "${_n_ev} + 1")
            if(_val GREATER 132)            # 0x84: 0x80/0x82/0x84 ergeben dasselbe Bild
                math(EXPR _ev_ohne_reset "${_ev_ohne_reset} + 1")
            endif()
            if(NOT "${_ev_t}" STREQUAL "")
                math(EXPR _p "${_t} - ${_ev_t}")
                set(_tol ${_dt})
                if(_ev_dt GREATER _tol)
                    set(_tol ${_ev_dt})
                endif()
                math(EXPR _tol "${_tol} + 1")
                math(EXPR _abw "${_p} - ${_soll_periode_us}")
                if(_abw LESS 0)
                    math(EXPR _abw "0 - ${_abw}")
                endif()
                math(EXPR _n_per "${_n_per} + 1")
                set(_urteil "ok")
                if(_abw GREATER _tol OR NOT _steps EQUAL 60)
                    math(EXPR _n_per_aus "${_n_per_aus} + 1")
                    set(_urteil "AUSSERHALB")
                endif()
                if(_n_per LESS 13)
                    string(APPEND _per_txt "\n    Periode ${_n_per}: ${_p} us, ${_steps} Engine-Schritte, "
                                           "Abweichung ${_abw} us, Toleranz (Bilddauer) ${_tol} us -> ${_urteil}")
                endif()
            endif()
            set(_ev_t ${_t})
            set(_ev_dt ${_dt})
            set(_steps 0)
        endif()
    elseif(_ph EQUAL 0)
        set(_ev_t "")                  # Beginn eines Titel-Abschnitts
        set(_steps 0)
    endif()
    set(_prev_ph ${_ph})
    set(_prev_t ${_t})
    set(_prev_ctr ${_ctr})
    if(_shown EQUAL 1)
        set(_prev_sum ${_sum})
    else()
        set(_prev_sum -1)
    endif()
endforeach()

set(_orig_nicht_gesehen "")
foreach(_e IN LISTS _orig_tab)
    string(REPLACE ":" ";" _kv "${_e}")
    list(GET _kv 0 _k)
    if(_orig_gesehen_${_k} EQUAL 0)
        list(APPEND _orig_nicht_gesehen ${_k})
    endif()
endforeach()

string(CONCAT _bericht
    "r30_titel_puls: ${_n_zeilen} Bilder (Titel ${_n_titel}, Fade ${_n_fade}), "
    "Bilddauer im Titel ${_dt_min} ... ${_dt_max} us, exit=${_rv}"
    "\n  (A) gezeichnetes Farbbyte != Pulswert der Engine: ${_zeichner_abw} Bilder (Soll 0)${_zeichner_txt}"
    "\n  (B) Pulswerte mit mehr als einem Zeilenbild: ${_bild_konflikt} (Soll 0), "
    "verschiedene Pulswerte gezeigt: ${_n_bild_werte}${_bild_txt}"
    "\n  (C) gleich dem Original-Bildpuffer: ${_orig_gleich} Bilder, abweichend ${_orig_abw} (Soll 0)${_orig_txt}"
    "\n  (D) Periode am Bild (Abfall auf Summe ${_smin}), Soll ${_soll_periode_us} us +/- Bilddauer, "
    "60 Engine-Schritte: ${_n_ev} Abfaelle${_per_txt}"
    "\n  (E) im Bestaetigungs-Fade: Pulsaenderungen der Engine ${_fade_aend}, Aenderungen des Zeilenbilds "
    "${_fade_bild_aend} (Soll 0 / 0)"
    "\n  (F) Einblende bis B = 0: ${_einbl_us} us (Soll ${_soll_einblende_us} us + 0 ... ${_einbl_dt} us Bilddauer)")

set(_fehler "")
if(NOT _n_ungezeigt EQUAL 1 OR _n_ungezeigt_fade GREATER 0)
    string(APPEND _fehler "\n  ${_n_ungezeigt} Titelbilder / ${_n_ungezeigt_fade} Fade-Bilder ohne Ruecklese "
                          "(Soll 1 / 0: nur das Bestaetigungsbild zeigt niemand) — Renderer ohne Ruecklese?")
endif()
if(_zeichner_abw GREATER 0)
    string(APPEND _fehler "\n  (A) der Zeichner zeichnet die aktive Zeile mit einem anderen Pulswert als die Engine")
endif()
if(_bild_konflikt GREATER 0)
    string(APPEND _fehler "\n  (B) das Zeilenbild haengt nicht allein am Pulswert der Engine")
endif()
if(_orig_abw GREATER 0)
    string(APPEND _fehler "\n  (C) ${_orig_abw} Bilder weichen vom Original-Bildpuffer ab (Modulation / Mischung)")
endif()
if(NOT "${_orig_nicht_gesehen}" STREQUAL "")
    string(APPEND _fehler "\n  (C) Original-Stufen nie gezeigt: ${_orig_nicht_gesehen}")
endif()
if(_n_zeile_fremd GREATER 0)
    string(APPEND _fehler "\n  ${_n_zeile_fremd} Bilder mit einer anderen aktiven Zeile als NEW GAME")
endif()
if(_n_per LESS _min_perioden)
    string(APPEND _fehler "\n  (D) nur ${_n_per} auswertbare Pulsperioden am Bild (mindestens ${_min_perioden})")
endif()
if(_n_per_aus GREATER 0)
    string(APPEND _fehler "\n  (D) ${_n_per_aus} von ${_n_per} Perioden am Bild ausserhalb 2005817 us +/- Bilddauer "
                          "bzw. nicht 60 Engine-Schritte — der Puls auf dem Bildschirm haengt nicht an 2 VBlanks je Schritt")
endif()
if(_ev_ohne_reset GREATER 0)
    string(APPEND _fehler "\n  (D) ${_ev_ohne_reset} Bildabfaelle ohne Ruecksetzung der Engine (Pulswert > 0x84)")
endif()
if(_bad_val GREATER 0)
    string(APPEND _fehler "\n  ${_bad_val} Pulswerte ausserhalb 0x80 ... 0xBE")
endif()
if(_n_fade LESS 1)
    string(APPEND _fehler "\n  (E) kein Bild des Bestaetigungs-Fades im Protokoll — RE15_TITLE_CONFIRM_MS griff nicht")
endif()
if(_fade_aend GREATER 0 OR _fade_bild_aend GREATER 0)
    string(APPEND _fehler "\n  (E) der Puls laeuft im Bestaetigungs-Fade weiter (Engine ${_fade_aend}, Bild ${_fade_bild_aend}; "
                          "Original: FUN_80102a10 ruft FUN_801028ec nicht)")
endif()
if("${_einbl_us}" STREQUAL "")
    string(APPEND _fehler "\n  (F) die Titel-Einblende erreicht im ersten Titel-Abschnitt nie B = 0")
else()
    math(EXPR _einbl_hi "${_soll_einblende_us} + ${_einbl_dt}")
    if(_einbl_us LESS _soll_einblende_us OR _einbl_us GREATER _einbl_hi)
        string(APPEND _fehler "\n  (F) Einblende ${_einbl_us} us statt ${_soll_einblende_us} ... ${_einbl_hi} us "
                              "(32 Durchgaenge zu 2 VBlanks)")
    endif()
endif()

if(NOT "${_fehler}" STREQUAL "")
    message(FATAL_ERROR "${_bericht}\nROT:${_fehler}")
endif()
message(STATUS "${_bericht}\nOK — die Zeile auf dem Bild pulst mit 2006 ms, gleich dem Original, ruht im Fade")
