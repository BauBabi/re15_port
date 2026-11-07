# -*- coding: utf-8 -*-
"""NB3-Bau: Riegel-Registrierung (probes/r41_gatorfix.cmake) und exe-Riegel (integration/test_r41_gatorfix.cmake)."""
import io, sys
def patch(P, reps):
    s = io.open(P, encoding="utf-8", newline="").read()
    crlf = "\r\n" in s
    s = s.replace("\r\n", "\n")
    for old, new in reps:
        c = s.count(old)
        if c != 1:
            print("FUNDSTELLE %d statt 1 in %s:\n%s" % (c, P, old[:300])); sys.exit(1)
        s = s.replace(old, new)
    io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
    print("OK", P, "crlf=%d" % crlf)

patch(r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/tests/unit/probes/r41_gatorfix.cmake", [
("""#   unit_r41_gatorfix_re2(_elza)     seit NB2: Kopf-Riegel (Mesh 5/6/7) Stand f942f56c0 gegen neu; Kopf + Kanister in Rampe/Plattform nur dokumentiert
foreach(_teil montage montage_elza marvin marvin_elza schluessel schluessel_elza klemm klemm_elza fuzz fuzz_elza haken haken_elza re2 re2_elza re2_gegen re2_gegen_elza gang gang_elza v4 v4_elza)""",
"""#   unit_r41_gatorfix_re2(_elza)     seit NB2: Kopf-Riegel (Mesh 5/6/7) Stand f942f56c0 gegen neu; Kopf + Kanister in Rampe/Plattform nur dokumentiert
#   unit_r41_gatorfix_nb3(_elza)     G2-RE2 NB3 (Nutzerentscheid 2026-11-06, Messblatt G2_MESSBLATT_NB3.md): (A1) 18 Rampen-Lagen oben x 3600 Bilder, mit und ohne
#                                     Kanister: erster Treffer <= 600 Bilder und laengste Pause ohne Treffer <= 600, Rammen >= 1; (B1) 11 Wasser-Lagen vor der
#                                     Suedkante: Haenge-Fenster 0, (B3) ohne Fortschritt <= 450, Pause ohne Treffer <= 600; mit = ohne; Gegenproben Stand
#                                     a661eb8aa (Pruefhaken nb3_alt): R2318/-16300 ohne Treffer, W4200/-18800/c2 Haenge-Fenster, alte gang-Lagen weichen ab
#   unit_r41_gatorfix_gang(_elza)    seit NB3: Lagen ausserhalb der Baender (Westkanal, Nordbecken 2500 vor der Kante, Suedbecken 2500 vor der Plattform),
#                                     alter Stand = Pruefhaken rw_alt + nb3_alt
foreach(_teil montage montage_elza marvin marvin_elza schluessel schluessel_elza klemm klemm_elza fuzz fuzz_elza haken haken_elza re2 re2_elza re2_gegen re2_gegen_elza gang gang_elza v4 v4_elza nb3 nb3_elza)"""),
("""    foreach(_teil gang_montage gang_montage_elza gang_szene gang_szene_elza frei frei_elza rampe rampe_elza
                  abn2_leiterecke abn2_wand_elza abn2_nordbecken abn2_rampe_west abn2_rampe_west_elza abn2_ecken abn2_wand)   # NB1/NB2""",
"""    foreach(_teil gang_montage gang_montage_elza gang_szene gang_szene_elza frei frei_elza rampe rampe_elza
                  abn2_leiterecke abn2_wand_elza abn2_nordbecken abn2_rampe_west abn2_rampe_west_elza abn2_ecken abn2_wand   # NB1/NB2
                  rampe_oben rampe_oben_elza)   # G2-RE2 NB3 (Vorgabe A): Figur oben auf der Rampe, obenlog; rampe/rampe_elza/abn2_wand_elza: dazu haenglog (Vorgabe B)"""),
])

patch(r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/tests/integration/test_r41_gatorfix.cmake", [
("""# TEIL=rampe|rampe_elza  Nachbesserung 1 (M3): wie frei, Rueckkehr mit dem Messhaken RE15_K2080_RUECK_LAGE an die Suedkante der Rampe (Blick Nord), die
#     Figur klettert auf die Rampe (Kletterzellen Bodentypwort Bit 0, nur die Rampe) und bleibt oben; climb.log sub=9, Belagerung (Phase 5) mit dem Kanister,
#     Stillstand fern <= 60 Bilder am Stueck, Wandreiben <= 30.""",
"""# TEIL=rampe|rampe_elza  Nachbesserung 1 (M3): wie frei, Rueckkehr mit dem Messhaken RE15_K2080_RUECK_LAGE an die Suedkante der Rampe (Blick Nord), die
#     Figur klettert auf die Rampe (Kletterzellen Bodentypwort Bit 0, nur die Rampe) und bleibt oben; climb.log sub=9, Belagerung (Phase 5) mit dem Kanister,
#     Stillstand fern <= 60 Bilder am Stueck, Wandreiben <= 30.
#     G2-RE2 NB3 (Vorgabe B, Messblatt G2_MESSBLATT_NB3.md Riegel B2/B3): rampe/rampe_elza/abn2_wand_elza mit und ohne Kanister - haenglog: Haenge-Fenster
#     (RW-Mass) 0, laengste Zeit ohne Fortschritt <= 450 Bilder; mit Kanister 0 Zeilen HAENGT / RUECKWAERTS an / TAUCHEN an (kein Rueckfall noetig).
# TEIL=rampe_oben|rampe_oben_elza  G2-RE2 NB3 (Vorgabe A, Riegel A2): Eingabe der Abnahme-4-Laeufe west4_leon / west5_elza bis zum Klettern (A0.2 bei ~F639),
#     dann W2.0,U1.2 auf die Rampe und W170 stehen (RE15_K2080_RUECK_LAGE 4200,-18800 / 3600,-18750, Cut 2), 4400 Bilder, mit und ohne Kanister - obenlog:
#     erster Treffer oben <= 600 Bilder nach dem ersten Oben-Bild, keine Pause ohne Treffer > 600, solange die Figur oben ist (Rammen, CROSS-Biss, Hochbiss)."""),
("""    set(_extra "")
    set(_ende 8400)
    if(TEIL MATCHES "^rampe")
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W22.0,A0.3,W2.0,A0.3,W2.0,XU0.8,W120")""",
"""    set(_extra "")
    set(_ende 8400)
    if(TEIL MATCHES "^rampe_oben")   # G2-RE2 NB3 (Vorgabe A): Messblatt NB3 1.3 / batch3.sh oben_leon|oben_elza
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W14.0,U0.1,A0.2,W2.0,U1.2,W170")
        if(TEIL MATCHES "elza$")
            set(_extra "RE15_K2080_RUECK_LAGE=3600,-18750,3072,2" RE15_CLIMB_TRACE=climb.log)
        else()
            set(_extra "RE15_K2080_RUECK_LAGE=4200,-18800,3072,2" RE15_CLIMB_TRACE=climb.log)
        endif()
        set(_ende 4400)
    elseif(TEIL MATCHES "^rampe")
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W22.0,A0.3,W2.0,A0.3,W2.0,XU0.8,W120")"""),
("""        set(_tz "${_tz}; Klettern sub=9 ${_kln} Zeilen, Ueberquerung mit Kanister ${_g5n} Bilder, Rammen ${_rmn}")
    endif()
    message(STATUS "r41_gatorfix ${TEIL}: ${_tz}")
    return()""",
"""        set(_tz "${_tz}; Klettern sub=9 ${_kln} Zeilen, Ueberquerung mit Kanister ${_g5n} Bilder, Rammen ${_rmn}")
    endif()
    # G2-RE2 NB3 (Vorgabe B, Riegel B2/B3): rampe / rampe_elza / abn2_wand_elza - haenglog mit (0 Rueckfall-Zeilen) und ohne Kanister: Haenge-Fenster 0, ohne Fortschritt <= 450
    if(TEIL MATCHES "^(rampe|rampe_elza|abn2_wand_elza)$")
        execute_process(COMMAND "${RE15_TOOL}" haenglog "${_dir}" mit RESULT_VARIABLE _hr OUTPUT_VARIABLE _ho ERROR_VARIABLE _he)
        string(REGEX MATCH "haenglog: [^\\n]*" _hz "${_ho}")
        execute_process(COMMAND "${RE15_TOOL}" haenglog "${_dir}_ohne" RESULT_VARIABLE _hr2 OUTPUT_VARIABLE _ho2 ERROR_VARIABLE _he2)
        string(REGEX MATCH "haenglog: [^\\n]*" _hz2 "${_ho2}")
        set(_tz "${_tz}; NB3 mit Kanister: ${_hz}; ohne Kanister: ${_hz2}")
        if(NOT _hr EQUAL 0 OR NOT _hr2 EQUAL 0)
            g2_fehler("NB3 Vorgabe B verfehlt (Haenge-Fenster / ohne Fortschritt / Rueckfall): ${_tz}")
        endif()
    endif()
    # G2-RE2 NB3 (Vorgabe A, Riegel A2): rampe_oben(_elza) - obenlog mit und ohne Kanister: erster Treffer oben <= 600, keine Pause > 600
    if(TEIL MATCHES "^rampe_oben")
        execute_process(COMMAND "${RE15_TOOL}" obenlog "${_dir}" RESULT_VARIABLE _or OUTPUT_VARIABLE _oo ERROR_VARIABLE _oe)
        string(REGEX MATCH "obenlog: [^\\n]*" _oz "${_oo}")
        execute_process(COMMAND "${RE15_TOOL}" obenlog "${_dir}_ohne" RESULT_VARIABLE _or2 OUTPUT_VARIABLE _oo2 ERROR_VARIABLE _oe2)
        string(REGEX MATCH "obenlog: [^\\n]*" _oz2 "${_oo2}")
        set(_tz "${_tz}; NB3 mit Kanister: ${_oz}; ohne Kanister: ${_oz2}")
        if(NOT _or EQUAL 0 OR NOT _or2 EQUAL 0)
            g2_fehler("NB3 Vorgabe A verfehlt (Figur oben sicher): ${_tz}")
        endif()
    endif()
    message(STATUS "r41_gatorfix ${TEIL}: ${_tz}")
    return()"""),
])
