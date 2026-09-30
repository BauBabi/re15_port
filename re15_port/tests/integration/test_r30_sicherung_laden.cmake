# =============================================================================
# SICHERUNG-LADE-PIN (Runde 30, Thema H; Nutzer: "in ROOM 1150 im Modell das
# hochgeht ist es nicht sichtbar").
#
# ⛔ SEIT RUNDE 34 NACHT (Spur B, include/re15_hebetisch_cursor.h): RE15_FIRE_AOT=1 (re15_aot_fire_slot ->
# scd_event_fire direkt) prueft den HARNESS-Weg OHNE Hebetisch-Cursor — die Fahrt laeuft sofort. Der
# Spieler hat diesen Weg nicht mehr: die Aktionstaste am Tisch bringt erst den Cursor (Kuppel-Klick ->
# Fahrt). Den Spielerweg bis zu den Aufnahmen prueft integration_r34n_b_cursor.
#
# GEMESSEN (echte exe, vor dem Bau, Spielstand in ROOM1150, RE15_FIRE_AOT=1@90):
# der Lade-Lauf war ueber alle 25 Stichbilder F90..F330 PIXELGLEICH mit dem Lauf
# ohne Sicherung. Im debug.log standen nach
#     [save] CONTINUE: resumed in room 1150 (hp=100)
# nur `[prop-render] pi=0`, `pi=1`, `pi=2` — KEINE Zeile `pi=4`, kein Modal.
# Am Tuerweg lag die Sicherung da (`[prop-render] pi=4 oid=0x04`).
#
# URSACHE: re15_sicherung_install hatte genau eine Aufrufstelle (scd_room_reenter);
# der Boot-/CONTINUE-Weg in platform/pc/main.c geht dort nicht durch.
#
# ORIGINAL: es gibt nur EINEN Raumlader. FUN_800396fc hat genau zwei Aufrufer,
#     8001d5ac: jal 0x800396fc      ; Session-Start / LOAD
#     8001d988: jal 0x800396fc      ; Tuer
# und ruft selbst @0x80039a00 `jal 0x8003ef6c` die SCD-Raum-Init.
#
# DER RIEGEL, zwei Laeufe mit der ECHTEN exe:
#   A  Spielstand in ROOM1150, Sicherung NICHT genommen -> CONTINUE, Hebetisch
#      ausloesen (RE15_FIRE_AOT=1@90#1150), Ende bei Bild 280 (RE15_EXIT_AT).
#      Das debug.log MUSS tragen:
#         CONTINUE: resumed
#         [sicherung] Boot-Weg: Prop obj_id=4 im Pool
#         [prop-render] pi=4 oid=0x04
#         [sicherung] Modal Item 0x40 zeichnet ... Bild weicht in 0 von 8064 Punkten ab
#   B  derselbe Lauf mit GENOMMENER Sicherung (Flag (9,53) im Spielstand).
#      Das debug.log darf KEINE der drei Sicherungs-Zeilen tragen — das Flag
#      wird vor dem Anlegen zurueckgeladen.
#
# WAS DIE ZEILEN BELEGEN — und was NICHT (Gegenpruefer, Runde 30):
#   * Pool-Zeile: re15_sicherung_install lief am Lade-Weg.
#   * [prop-render] pi=4: das Prop wird dem Zeichenweg uebergeben. Die Zeile steht
#     EINMAL je Prozess, beim ERSTEN Zeichnen — je nach Kamera schon an der Parklage
#     vor dem Ausloesen (Nutzerkarte: y=-21386, vor [fire-aot]). Ueber die
#     Hebetisch-Szene sagt sie NICHTS.
#   * Modal-Zeile: DAS ist die Pruefung IN der Szene. re15_sicherung_tick macht das
#     Modal nur auf, wenn die Sicherung angelegt ist UND die Plattform in der RUHE OBEN
#     steht (Runde 31: sub04 im Sleep 30 @0x101A, include/re15_hebetisch.h), also die
#     Hubfahrt von sub04 zu Ende ist (gemessen: Modal auf in Bild 247, zeichnet in Bild 248,
#     Hebetisch y=-1205; bis Runde 30 Bild 226 bei y=-1105 mitten im Hub). Die Zahl 0 sagt,
#     dass das Modal das Rohr-Bild
#     zeichnet (Leser re15_itps_pixel). Ob die Sicherung im Fach SICHTBAR ist, belegt
#     allein die Framedump-Abnahme (analysis/befunde_runde30/sicherung.md, UMSETZUNG).
#
# Er DISKRIMINIERT: ohne den Aufruf am Lade-Weg fehlen in Lauf A alle drei Zeilen —
# genau der gemessene Vorher-Stand; ohne das Einsetzen des Bildes in main.c meldet
# die Modal-Zeile statt 0 die Abweichung des ausgelieferten Blocks (2212 Punkte).
#
# RE15_SOFTWARE_RENDER=1 dient hier nur der Robustheit des Testhakens; geprueft
# wird das LOG, kein Bild.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe> -DWORKDIR=<dir>
#               -P test_r30_sicherung_laden.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "sicherung_laden: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "sicherung_laden: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "sicherung_laden: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Eigenes Arbeitsverzeichnis je Aufruf (ein Rest des vorigen Aufrufs haelt sonst
# debug.log gesperrt — dieselbe Begruendung wie im Westen-Pin).
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# "CONTINUE: resumed"           = 434f4e54494e55453a20726573756d6564
# "[sicherung] Boot-Weg: Prop"  = 5b736963686572756e675d20426f6f742d5765673a2050726f70
# "[prop-render] pi=4 oid=0x04" = 5b70726f702d72656e6465725d2070693d34206f69643d30783034
# "EXIT_AT: Bild"               = 455849545f41543a2042696c64
set(_hex_continue "434f4e54494e55453a20726573756d6564")
set(_hex_boot     "5b736963686572756e675d20426f6f742d5765673a2050726f70")
set(_hex_render   "5b70726f702d72656e6465725d2070693d34206f69643d30783034")
set(_hex_exit     "455849545f41543a2042696c64")
# "[sicherung] Modal Item 0x40 zeichnet" und " Bild weicht in 0 von 8064 Punkten ab"
string(HEX "[sicherung] Modal Item 0x40 zeichnet" _hex_modal)
string(HEX "Bild weicht in 0 von 8064 Punkten ab" _hex_modal_rohr)
string(TOLOWER "${_hex_modal}" _hex_modal)
string(TOLOWER "${_hex_modal_rohr}" _hex_modal_rohr)

function(sicherung_lauf _name _karten_arg _out_hex)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")

    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "1150" ${_karten_arg}
                    WORKING_DIRECTORY "${WORKDIR}"
                    TIMEOUT 60
                    RESULT_VARIABLE _rvk
                    OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0)
        message(FATAL_ERROR "sicherung_laden[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    if(NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "sicherung_laden[${_name}]: keine Karte geschrieben")
    endif()

    re15_start_spiel(_rv 180
        RE15_NO_INTRO=1
        RE15_NOAUDIO=1
        RE15_SOFTWARE_RENDER=1
        RE15_CONTINUE_TEST=1
        RE15_CARD_AUTO=1
        RE15_CARD_SLOT=0
        "RE15_FIRE_AOT=1@90#1150"    # Hebetisch ausloesen: Aot slot 1 -> sub04
        "RE15_EXIT_AT=280#1150"      # Prozessende am Bild (Modal zeichnet ab Bild 248, Runde 31)
        "${RE15_PC_EXE}")

    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "sicherung_laden[${_name}]: kein debug.log (exit=${_rv})")
    endif()
    # debug.log enthaelt auch Binaer-Bytes -> binaer-sicher als HEX lesen und suchen
    file(READ "${WORKDIR}/debug.log" _lh HEX)

    string(FIND "${_lh}" "${_hex_continue}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "sicherung_laden[${_name}]: der Lauf hat gar nicht geladen "
                            "(keine CONTINUE-Zeile im debug.log, exit=${_rv})")
    endif()
    string(FIND "${_lh}" "${_hex_exit}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "sicherung_laden[${_name}]: Bild 280 in ROOM1150 wurde nicht "
                            "erreicht (keine EXIT_AT-Zeile, exit=${_rv}) — der Lauf ist "
                            "vorher abgerissen, ueber die Sicherung sagt er nichts")
    endif()
    set(${_out_hex} "${_lh}" PARENT_SCOPE)
endfunction()

# --- Lauf A: Sicherung liegt noch im Tisch ------------------------------------
sicherung_lauf(a "" _log_a)
string(FIND "${_log_a}" "${_hex_boot}" _pos)
if(_pos LESS 0)
    message(FATAL_ERROR
        "sicherung_laden[A]: nach dem Laden in ROOM1150 liegt die Sicherung NICHT im "
        "Prop-Pool — re15_sicherung_install fehlt am Boot-/CONTINUE-Weg "
        "(Original: FUN_800396fc hat zwei Aufrufer, @0x8001d5ac LOAD und @0x8001d988 Tuer).")
endif()
string(FIND "${_log_a}" "${_hex_render}" _pos)
if(_pos LESS 0)
    message(FATAL_ERROR
        "sicherung_laden[A]: die Sicherung liegt im Pool, wird dem Zeichenweg aber nie "
        "uebergeben (keine Zeile '[prop-render] pi=4 oid=0x04' = kein erstes Zeichnen im "
        "ganzen Lauf).")
endif()
string(FIND "${_log_a}" "${_hex_modal}" _pos)
if(_pos LESS 0)
    message(FATAL_ERROR
        "sicherung_laden[A]: in der Hebetisch-Szene geht das Aufnahme-Modal der Sicherung "
        "bis Bild 280 NICHT auf (keine Zeile '[sicherung] Modal Item 0x40 zeichnet') - "
        "entweder ist die Hubfahrt von sub04 nicht gelaufen oder die Sicherung fehlt.")
endif()
# die Zahl muss IN der Modal-Zeile stehen, nicht irgendwo im Log
string(SUBSTRING "${_log_a}" ${_pos} 400 _modal_zeile)
string(FIND "${_modal_zeile}" "${_hex_modal_rohr}" _p2)
if(_p2 LESS 0)
    message(FATAL_ERROR
        "sicherung_laden[A]: das Aufnahme-Modal zeichnet NICHT das Rohr-Bild (die Modal-Zeile "
        "meldet eine Abweichung vom eingesetzten Block) - main.c setzt das Bild nicht ein.")
endif()

# --- Lauf B: Sicherung schon genommen -----------------------------------------
sicherung_lauf(b "genommen" _log_b)
string(FIND "${_log_b}" "${_hex_boot}" _pos)
if(NOT _pos LESS 0)
    message(FATAL_ERROR
        "sicherung_laden[B]: die Sicherung ist im Spielstand GENOMMEN (Flag (9,53)), liegt "
        "nach dem Laden aber wieder im Pool — das Anlegen laeuft vor dem Restore der Flags.")
endif()
string(FIND "${_log_b}" "${_hex_render}" _pos)
if(NOT _pos LESS 0)
    message(FATAL_ERROR
        "sicherung_laden[B]: die genommene Sicherung wird nach dem Laden gezeichnet.")
endif()
string(FIND "${_log_b}" "${_hex_modal}" _pos)
if(NOT _pos LESS 0)
    message(FATAL_ERROR
        "sicherung_laden[B]: fuer die genommene Sicherung geht das Aufnahme-Modal noch einmal auf.")
endif()

message(STATUS "sicherung_laden: OK — am Lade-Weg liegt die Sicherung im Pool, geht an den "
               "Zeichenweg, und in der Hebetisch-Szene oeffnet das Modal mit dem Rohr-Bild; "
               "eine genommene Sicherung bleibt weg")
