# =============================================================================
# TITEL-PULS-PIN (Runde 30 / Thema D, Nutzer 2026-09-27: "Im Titelbild bei der
# Auswahl New Game, Load Game, Option blinkt der ausgewaehlte Bereich in der
# Frequenz im Vergleich zum Original zu schnell.")
# Dossier: analysis/befunde_runde30/titel-blinken.md
#
# Faehrt die ECHTE re15_pc.exe und misst ueber die Messschiene
# RE15_TITLE_PULSE_LOG (eine Zeile je Bild des Titels:
#   "<zeit_us> <zaehler> <pulswert> <faellige_durchgaenge> <phase> <einblende_tick> <einblende_B>",
#  phase 0 = Titel-Schleife, 1 = Bestaetigungs-Fade), was das Spiel TUT:
#
#  (1) PULSPERIODE in der Titel-Schleife. Soll: 60 Pulsschritte (0x3c @0x80102918,
#      TITLE.BIN FUN_801028ec) zu je einem Durchgang der Original-Hauptschleife =
#      2 VBlanks (DAT_800b5456 := 2 @0x8002130c-14, VSync(a0) @0x8002147c-80) bei
#      59,826 Hz (psx-spx, NTSC non-interlaced) = 2005816,9 us. Kleinste ganze Laufzeit
#      mit 60 Durchgaengen: 2005817 us.
#      TOLERANZ = gemessene Bilddauer, keine feste Zahl: ein faelliger Schritt wird erst am
#      Anfang des naechsten Bildes ausgefuehrt, also zwischen 0 und 1 Bilddauer spaeter.
#      Je Periode gilt |P - 2005817| <= max(Bilddauer an beiden Grenzen) + 1 us.
#  (2) PULS IM BESTAETIGUNGS-FADE: 0 Aenderungen von (Zaehler, Pulswert) ab dem
#      Bestaetigen. Original: der Fade FUN_80102ccc zeichnet ueber FUN_80102a10
#      (jal @0x80102d10 / @0x80102d60), und FUN_80102a10 (0x80102a10-0x80102a88) ruft
#      FUN_801028ec NICHT.
#  (3) TITEL-EINBLENDE: erstes Bild mit B = 0 nach 32 Durchgaengen (Schritt -0x400
#      `ori a1,zero,0xfc00` @0x80102058, Pegel 0x7fff @0x80021718, Farbe = Pegel >> 7
#      @0x800218d0) = 1069768,96 us nach dem ersten Bild -> 1069769 us <= Dauer
#      <= 1069769 us + Bilddauer.
#
# Ablauf: RE15_TITLE_CONFIRM_MS (Testhaken in main.c, ZEIT statt Bildern) bestaetigt
# NEW GAME nach 8,5 s -> Bestaetigungs-Fade -> Player-Select mit RE15_PSELECT_AUTO ->
# RE15_BOOT_EXIT_AT=1 beendet den Prozess am Beginn des Spiel-Boots.
# RE15_SOFTWARE_RENDER=1 (kein VSync): fuer diese ZEITmessung zulaessig und gewollt —
# die Schleife laeuft dann mit freier Bildrate, der Puls muss trotzdem 2006 ms halten.
# Ein BILD-Beweis ist das nicht (dafuer gdigrab, Dossier §8.4 b/e).
#
# GEGENPROBE (Dossier titel-blinken.md, UMSETZUNG Nachbesserung): am Ausgangsstand
# master d98e9639 — dort mit derselben Messschiene nachgeruestet — ist dieser Test ROT
# (Periode = 60 Bilder statt 2006 ms, Puls laeuft im Fade weiter, Einblende zu kurz).
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
set(_confirm_ms 8500)          # Testparameter: 8,5 s Titel = 3 volle Perioden (Grenzen ~2,0/4,0/6,0/8,0 s)
set(_min_perioden 2)

re15_start_spiel(_rv 180
    RE15_NO_INTRO=1
    RE15_NOAUDIO=1
    RE15_SOFTWARE_RENDER=1
    RE15_TITLE_PULSE_LOG=puls.txt
    RE15_TITLE_CONFIRM_MS=${_confirm_ms}
    RE15_PSELECT_AUTO=1
    RE15_BOOT_EXIT_AT=1
    "${RE15_PC_EXE}")

if(NOT EXISTS "${WORKDIR}/puls.txt")
    message(FATAL_ERROR "r30_titel_puls: kein Pulsprotokoll (RE15_TITLE_PULSE_LOG) geschrieben "
                        "(exit=${_rv}) — die exe kennt die Messschiene nicht oder kam nie in den Titel")
endif()

file(STRINGS "${WORKDIR}/puls.txt" _zeilen REGEX "^[0-9]+ [0-9]+ [0-9]+ [0-9]+ [0-9]+ [0-9]+ -?[0-9]+$")
list(LENGTH _zeilen _n_zeilen)
if(_n_zeilen LESS 10)
    message(FATAL_ERROR "r30_titel_puls: nur ${_n_zeilen} Zeilen im Pulsprotokoll (exit=${_rv})")
endif()

set(_prev_ph "")
set(_prev_t 0)
set(_prev_ctr 0)
set(_prev_val 0)
set(_wrap_t "")          # Zeit der letzten auswertbaren Ruecksetzung im laufenden Titel-Abschnitt
set(_wrap_dt 0)
set(_steps 0)            # Pulsschritte seit dieser Ruecksetzung
set(_n_titel 0)
set(_n_fade 0)
set(_fade_aend 0)
set(_n_per 0)
set(_n_per_aus 0)
set(_per_txt "")
set(_bad_val 0)
set(_seg0_t "")          # erstes Bild des ERSTEN Titel-Abschnitts (Einblende)
set(_einbl_us "")
set(_einbl_dt 0)
set(_dt_min 999999999)
set(_dt_max 0)
set(_first_seg_done 0)

foreach(_z IN LISTS _zeilen)
    string(REPLACE " " ";" _f "${_z}")
    list(GET _f 0 _t)
    list(GET _f 1 _ctr)
    list(GET _f 2 _val)
    list(GET _f 3 _due)
    list(GET _f 4 _ph)
    list(GET _f 6 _b)

    if(_ph EQUAL 0)
        math(EXPR _n_titel "${_n_titel} + 1")
        if(_val LESS 128 OR _val GREATER 190)
            math(EXPR _bad_val "${_bad_val} + 1")
        endif()
        if("${_prev_ph}" STREQUAL "0")
            math(EXPR _dt "${_t} - ${_prev_t}")
            if(_dt LESS _dt_min)
                set(_dt_min ${_dt})
            endif()
            if(_dt GREATER _dt_max)
                set(_dt_max ${_dt})
            endif()
            # --- (3) Einblende: erstes Bild mit B = 0 im ersten Titel-Abschnitt
            if(NOT _first_seg_done AND "${_einbl_us}" STREQUAL "" AND _b EQUAL 0)
                math(EXPR _einbl_us "${_t} - ${_seg0_t}")
                set(_einbl_dt ${_dt})
            endif()
            # --- (1) Pulsperiode
            if(_due GREATER 0)
                math(EXPR _steps "${_steps} + ${_due}")
                math(EXPR _sum "${_prev_ctr} + ${_due}")
                if(_sum GREATER_EQUAL 60)                 # Zaehler lief in diesem Bild durch 0
                    if(_due EQUAL 1)
                        if(NOT "${_wrap_t}" STREQUAL "")
                            math(EXPR _p "${_t} - ${_wrap_t}")
                            set(_tol ${_dt})
                            if(_wrap_dt GREATER _tol)
                                set(_tol ${_wrap_dt})
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
                            string(APPEND _per_txt "\n    Periode ${_n_per}: ${_p} us, ${_steps} Schritte, "
                                                   "Abweichung ${_abw} us, Toleranz (Bilddauer) ${_tol} us -> ${_urteil}")
                        endif()
                        set(_wrap_t ${_t})
                        set(_wrap_dt ${_dt})
                    else()
                        set(_wrap_t "")                   # mehrere Schritte im Grenzbild: nicht auswertbar
                    endif()
                    set(_steps 0)
                endif()
            endif()
        else()
            # Beginn eines Titel-Abschnitts (Eintritt oder Rueckkehr): Durchgang 0
            if("${_seg0_t}" STREQUAL "")
                set(_seg0_t ${_t})
            elseif(NOT "${_prev_ph}" STREQUAL "")
                set(_first_seg_done 1)
            endif()
            set(_wrap_t "")
            set(_steps 0)
        endif()
    else()
        # --- (2) Bestaetigungs-Fade: (Zaehler, Pulswert) gegen das Bild davor
        math(EXPR _n_fade "${_n_fade} + 1")
        if(NOT "${_prev_ph}" STREQUAL "")
            if(NOT _ctr EQUAL _prev_ctr OR NOT _val EQUAL _prev_val)
                math(EXPR _fade_aend "${_fade_aend} + 1")
            endif()
        endif()
        set(_first_seg_done 1)
    endif()
    set(_prev_ph ${_ph})
    set(_prev_t ${_t})
    set(_prev_ctr ${_ctr})
    set(_prev_val ${_val})
endforeach()

string(CONCAT _bericht
    "r30_titel_puls: ${_n_zeilen} Bilder (Titel ${_n_titel}, Fade ${_n_fade}), "
    "Bilddauer im Titel ${_dt_min} ... ${_dt_max} us, exit=${_rv}"
    "\n  (1) Pulsperiode, Soll ${_soll_periode_us} us +/- Bilddauer:${_per_txt}"
    "\n  (2) Pulsaenderungen im Bestaetigungs-Fade: ${_fade_aend} (Soll 0)"
    "\n  (3) Einblende bis B = 0: ${_einbl_us} us (Soll ${_soll_einblende_us} us + 0 ... ${_einbl_dt} us Bilddauer)")

set(_fehler "")
if(_n_per LESS _min_perioden)
    string(APPEND _fehler "\n  nur ${_n_per} auswertbare Pulsperioden (mindestens ${_min_perioden})")
endif()
if(_n_per_aus GREATER 0)
    string(APPEND _fehler "\n  ${_n_per_aus} von ${_n_per} Pulsperioden ausserhalb 2005817 us +/- Bilddauer "
                          "bzw. nicht 60 Schritte — der Puls haengt nicht an 2 VBlanks je Schritt")
endif()
if(_bad_val GREATER 0)
    string(APPEND _fehler "\n  ${_bad_val} Pulswerte ausserhalb 0x80 ... 0xBE")
endif()
if(_n_fade LESS 1)
    string(APPEND _fehler "\n  kein Bild des Bestaetigungs-Fades im Protokoll — RE15_TITLE_CONFIRM_MS griff nicht")
endif()
if(_fade_aend GREATER 0)
    string(APPEND _fehler "\n  der Puls laeuft im Bestaetigungs-Fade weiter (${_fade_aend} Aenderungen; "
                          "Original: FUN_80102a10 ruft FUN_801028ec nicht)")
endif()
if("${_einbl_us}" STREQUAL "")
    string(APPEND _fehler "\n  die Titel-Einblende erreicht im ersten Titel-Abschnitt nie B = 0")
else()
    math(EXPR _einbl_hi "${_soll_einblende_us} + ${_einbl_dt}")
    if(_einbl_us LESS _soll_einblende_us OR _einbl_us GREATER _einbl_hi)
        string(APPEND _fehler "\n  Einblende ${_einbl_us} us statt ${_soll_einblende_us} ... ${_einbl_hi} us "
                              "(32 Durchgaenge zu 2 VBlanks)")
    endif()
endif()

if(NOT "${_fehler}" STREQUAL "")
    message(FATAL_ERROR "${_bericht}\nROT:${_fehler}")
endif()
message(STATUS "${_bericht}\nOK — Titel-Puls 2006 ms, ruht im Fade, Einblende 32 Durchgaenge")
