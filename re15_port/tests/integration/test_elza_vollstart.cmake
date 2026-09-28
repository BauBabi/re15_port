# =============================================================================
# RIEGEL "ELZA-VOLLSTART" (Runde 30, Thema G). Uebernommen aus der Vorlage
# analysis/befunde_runde30/r30_elza_vollstart_riegel.cmake; angemeldet in
# tests/unit/probes/r30_elza-intro.cmake als integration_elza_vollstart (TIMEOUT 240).
# Gebaut wird gegen: P1 main.c Vorlauf (3,193) auf RE15_ROOM_BASE(boot_room) (ROOM1031.RDT
# main00 @0x0204E / sub12 @0x02976), P2 main.c work_vars[0x10] nach scd_vm_init
# (@0x8001d51c / @0x8001d558 / @0x80039768), P3 aot_common.c Selbst-Tuer mit Variante
# (@0x800397e4 / @0x800397ec / @0x8001d988).
#
# Nutzer-Befund (AUFTRAG.md Abschnitt G): Lobby-Bild im Vorspann, Abbruch spielt die
# Montage noch einmal, in der Lobby steht Leon statt Elza, ihre Szene startet nicht.
#
# Faehrt die ECHTE re15_pc.exe: Titel -> NEW GAME -> Charakterwahl RECHTS (= Elza)
# -> Vorspann-Montage ROOM1241 -> ABBRUCH im ersten Tastenfenster nach Bild 352
# (RE15_PRESS square@352..364; das Fenster steht in ROOM1241.RDT sub02: Set(2,7,0)
# @0x058A / Sleep 2 / Set(2,7,1) @0x0592, gemessen Bild 361..363) -> ROOM1031.
#
# Geprueft wird das debug.log:
#   (a) KEIN LOBBY-BILD VOR DEM UEBERGANG: der erste Hintergrund, der nach dem Laden
#       von ROOM1031 geladen wird, ist Cut 13 (BG13 = das schwarze Standbild, byte-gleich
#       mit ROOM1170 BG07 und ROOM1240 BG00, sha256 44cafcd7ff20b961...), nicht Cut 0.
#   (b) ABBRUCH -> LOBBY OHNE NEUSTART: ROOM1241 wird kein zweites Mal geladen; in
#       ROOM1031 startet der Erzaehler sub15 (Evt_exec, ROOM1031.RDT @0x020A2), nicht
#       sub12 (@0x02072, der Rueckweg in die Montage).
#   (c) PL04 IN ROOM1031 UND DIE SZENE LAEUFT: keine "[pld] Spielermodell -> PLD/PL00.PLD"-
#       Zeile; nach der Selbst-Tuer Slot 19 (@0x02082) startet sub13 (Evt_exec @0x020B2)
#       und die Szene endet ("letterbox closed -> gameplay").
#
# Das Spiel hat keinen Ausstieg nach Gesamtbildern (g_engine.frame_count springt bei
# jedem Raumwechsel auf 0), deshalb endet der Lauf ueber die Zeitschranke; ein
# Zeitablauf ist hier der ERWARTETE Ausgang, geprueft wird allein das Protokoll.
#
# Aufruf:
#   cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<ordner> [-DRE15_ASSET_ROOT=<PSX-Baum>]
#         [-DLAUFZEIT=100] [-DNUR_PRUEFEN=1] -P r30_elza_vollstart_riegel.cmake
# NUR_PRUEFEN=1 startet das Spiel nicht, sondern wertet ein vorhandenes WORKDIR/debug.log aus.
# =============================================================================
if(NOT NUR_PRUEFEN AND (NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}"))
    message(FATAL_ERROR "elza_vollstart: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "elza_vollstart: WORKDIR fehlt")
endif()
if(NOT LAUFZEIT)
    set(LAUFZEIT 100)   # gemessen: Start ~25 s + 363 + 445 + 570 Bilder bei 30/s = ~71 s
endif()

if(NOT NUR_PRUEFEN)
file(MAKE_DIRECTORY "${WORKDIR}")
file(REMOVE "${WORKDIR}/debug.log")

set(ENV{RE15_NOAUDIO} 1)
set(ENV{RE15_NO_INTRO} 1)
set(ENV{RE15_PSELECT_AUTO} 1)
set(ENV{RE15_PSELECT_AUTO_SWITCH} 1)
set(ENV{RE15_FADE_LOG} 1)
set(ENV{RE15_EVT_TRACE} 1)
set(ENV{RE15_INPUT_SCRIPT} "W2,S1,W900")
set(ENV{RE15_INPUT_SCRIPT_START} 30)
set(ENV{RE15_PRESS} "square@352,square@353,square@354,square@355,square@356,square@357,square@358,square@359,square@360,square@361,square@362,square@363,square@364")
if(RE15_ASSET_ROOT)
    set(ENV{RE15_ASSET_ROOT} "${RE15_ASSET_ROOT}")
    set(ENV{RE15_CD_ROOT}    "${RE15_ASSET_ROOT}")
endif()

execute_process(COMMAND "${RE15_PC_EXE}"
                WORKING_DIRECTORY "${WORKDIR}"
                TIMEOUT ${LAUFZEIT}
                RESULT_VARIABLE rv)
message(STATUS "elza_vollstart: Lauf beendet mit '${rv}' (Zeitablauf ist erwartet)")
endif()   # NOT NUR_PRUEFEN

if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "elza_vollstart: kein debug.log in ${WORKDIR}")
endif()
file(STRINGS "${WORKDIR}/debug.log" zeilen
     REGEX "pselect done|Spieler-Familie|PC loaded room|Spielermodell -> |Evt_exec sub=|DOOR FIRE slot=|bg-log. F[0-9]+ load room|letterbox closed")
# KEINE Muster mit eckigen Klammern: CMakes Regex liest "[pld]" als ZEICHENKLASSE. Gemessen am
# ersten Wurf dieses Skripts: das Muster fuer die pld-Zeile traf "[window] windowed 960x720"
# (ein 'd' vor einem Leerzeichen) und verfehlte die eigentliche Zeile
# "[pld] Spielermodell -> PLD/PL00.PLD" - der Riegel meldete PL00-Tausch='' , obwohl die
# Zeile im Protokoll stand (debug.log Zeile 457). Deshalb hier nur klammerfreie Muster.

set(fehler "")
set(n_1241 0)
set(n_1031 0)
set(erster_bg_1031 "")
set(hat_sub12 0)
set(hat_sub15 0)
set(hat_sub13 0)
set(hat_tuer19 0)
set(hat_ende 0)
set(hat_elza 0)
set(hat_pl04 0)
set(pl00_tausch "")
set(in_1031 0)
foreach(z IN LISTS zeilen)
    if(z MATCHES "pselect done ch=1")
        set(hat_elza 1)
    endif()
    if(z MATCHES "Spieler-Familie PL04")
        set(hat_pl04 1)
    endif()
    if(z MATCHES "PC loaded room1241")
        math(EXPR n_1241 "${n_1241}+1")
        set(in_1031 0)
    endif()
    if(z MATCHES "PC loaded room1031")
        math(EXPR n_1031 "${n_1031}+1")
        set(in_1031 1)
    endif()
    if(in_1031 AND erster_bg_1031 STREQUAL "" AND z MATCHES "load room1031#([0-9]+)")
        set(erster_bg_1031 "${CMAKE_MATCH_1}")
    endif()
    if(z MATCHES "Spielermodell -> PLD/PL00")
        set(pl00_tausch "${z}")
    endif()
    if(z MATCHES "room=1031 Evt_exec sub=12 ")
        set(hat_sub12 1)
    endif()
    if(z MATCHES "room=1031 Evt_exec sub=15 ")
        set(hat_sub15 1)
    endif()
    if(z MATCHES "DOOR FIRE slot=19 ")
        set(hat_tuer19 1)
    endif()
    if(hat_tuer19 AND z MATCHES "room=1031 Evt_exec sub=13 ")
        set(hat_sub13 1)
    endif()
    if(hat_sub13 AND z MATCHES "letterbox closed")
        set(hat_ende 1)
    endif()
endforeach()

message(STATUS "elza_vollstart: MESSWERTE  elza=${hat_elza} pl04=${hat_pl04}  "
               "geladen room1031=${n_1031}x room1241(nach Start)=${n_1241}x  "
               "erster BG in 1031=#${erster_bg_1031}  sub12=${hat_sub12} sub15=${hat_sub15}  "
               "Tuer19=${hat_tuer19} sub13=${hat_sub13} Szenenende=${hat_ende}  "
               "PL00-Tausch='${pl00_tausch}'")

if(NOT hat_elza)
    list(APPEND fehler "Vorbedingung: die Charakterwahl lieferte nicht ch=1 (Elza)")
endif()
if(NOT hat_pl04)
    list(APPEND fehler "Vorbedingung: beim Start wurde nicht die Familie PL04 geladen")
endif()
if(n_1031 EQUAL 0)
    list(APPEND fehler "Vorbedingung: ROOM1031 wurde nie geladen (Abbruch nicht gegriffen?)")
endif()
# (a)
if(NOT erster_bg_1031 STREQUAL "13")
    list(APPEND fehler "(a) LOBBY-BILD IM VORSPANN: erster Hintergrund in ROOM1031 ist Cut #${erster_bg_1031}, erwartet 13 (schwarzes Erzaehler-Standbild)")
endif()
# (b)
if(n_1241 GREATER 0)
    list(APPEND fehler "(b) MONTAGE SPIELT NOCH EINMAL: ROOM1241 wurde nach dem Start ${n_1241}x neu geladen")
endif()
if(hat_sub12)
    list(APPEND fehler "(b) ROOM1031 nahm den Erstbesuchs-Zweig (Evt_exec sub12 = Rueckweg in die Montage)")
endif()
if(NOT hat_sub15)
    list(APPEND fehler "(b) der Erzaehler sub15 ist in ROOM1031 nicht gestartet")
endif()
# (c)
if(NOT pl00_tausch STREQUAL "")
    list(APPEND fehler "(c) LEON STATT ELZA: ${pl00_tausch}")
endif()
if(NOT hat_tuer19)
    list(APPEND fehler "(c) die Selbst-Tuer Slot 19 hat nicht gefeuert")
endif()
if(NOT hat_sub13)
    list(APPEND fehler "(c) ELZAS SZENE STARTET NICHT: nach der Selbst-Tuer kein Evt_exec sub13")
endif()
if(NOT hat_ende)
    list(APPEND fehler "(c) die Szene ist nicht bis zur Spielfreigabe durchgelaufen")
endif()

if(fehler)
    foreach(f IN LISTS fehler)
        message(STATUS "elza_vollstart: FEHLER  ${f}")
    endforeach()
    list(LENGTH fehler nf)
    message(FATAL_ERROR "elza_vollstart: ${nf} Fehler")
endif()
message(STATUS "elza_vollstart OK")
