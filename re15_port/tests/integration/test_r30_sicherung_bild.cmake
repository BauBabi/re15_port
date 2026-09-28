# =============================================================================
# SICHERUNG-BILD-PIN (Runde 30, Thema H "andere Sicherung"; Nachbesserung nach dem
# Gegenpruefer).
#
# BEFUND DES GEGENPRUEFERS (Mutationsprobe M2): Das PC-Spiel liest ITEM/ITPS.ITP und
# DATA/ITEMALL.PIX ueber VIER Stellen im Plattformcode,
#     platform/pc/main.c             vor re15_itemall_set_pix / re15_itps_set_data
#     platform/pc/src/inv_render_pc.c   s_itemall / s_itps (Statusschirm)
# Ohne den Einsetz-Aufruf an diesen vier Stellen blieben unit_r30_sicherung_bild und
# integration_r30_sicherung_laden GRUEN, obwohl das Aufnahme-Modal wieder das alte
# Bild ("Fuse Case", RE2-Block 77) zeigte: Modal F240 wich in 24583 Punkten (960x720)
# vom Bau ab und nur in 4158 vom Bestand. Der Bild-Riegel prueft nur die faulen Lader
# der Engine, die das PC-Spiel nie erreicht (main.c setzt die Puffer vorher).
#
# DIESER RIEGEL faehrt die ECHTE exe: Spielstand in ROOM1150 mit der Sicherung in
# Inventarplatz 0 (probe_r30_sicherung_karte ... fach0), CONTINUE, dann der
# Inventar-Abnahmehaken RE15_INV_CHECK_SHOT (main.c: SQUARE F31 Reiter ITEM, SQUARE F45
# Raster-Bestaetigung, LEFT F54 CHECK, SQUARE F56 Ausfuehren; Foto-Upload F64; Abzug und
# Ende F140). Das debug.log MUSS tragen:
#   1. [sicherung] Ladestelle main.c: Modal-Bild 0x40 weicht in 0 von 8064 Punkten ab,
#      Icon-Tile 0x40 in 0 von 1200 Bytes
#        -> main.c hat an BEIDEN Stellen eingesetzt; gelesen ueber re15_itps_pixel (den
#           Leser des Modals) und re15_itemall_tile_raw
#   2. [inv] Sicherung im Statusschirm: Icon-Tile 0x40 weicht in 0 von 1200 Bytes ab,
#      Bild-Block 0x40 in 0 von 12288 Bytes
#        -> inv_render_pc.c hat an BEIDEN Stellen eingesetzt
#   3. [inv] CHECK-Foto Item 0x40: crect (0,489) prect (832,256) 56x72 -> Foto und CLUT geladen
#        -> der CHECK-Vorgang auf der Sicherung laedt das Foto in das Fenster des
#           RE1.5-Schirms. Mit dem AUSGELIEFERTEN Block stuende hier crect (0,480)
#           prect (0,0) -> "Foto NICHT geladen" (Foto-Lader DEBUG.BIN @0x800c0258:
#           LoadImage der eingebetteten Rechtecke @0x800c0280 / @0x800c02a0).
#
# ER DISKRIMINIERT (gemessen, Dossier analysis/befunde_runde30/sicherung.md UMSETZUNG):
# fehlt einer der vier Einsetz-Aufrufe, steht in Zeile 1 bzw. 2 statt der 0 die
# Abweichung des ausgelieferten Stands (Modal-Leser 2212 Punkte, Tile 389, Block 8904
# Bytes), und Zeile 3 meldet "Foto NICHT geladen".
#
# ⛔ Geprueft wird das LOG, kein Bild. Die Zeilen lesen aber genau die Puffer, aus denen
# gezeichnet wird. Die Sichtabnahme (Modal, Raster, CHECK-Fotofeld) laeuft ueber
# RE15_FRAMEDUMP mit dem beschleunigten Renderer (Dossier, UMSETZUNG).
# RE15_SOFTWARE_RENDER=1 dient hier nur der Robustheit des Testhakens.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe> -DWORKDIR=<dir>
#               -P test_r30_sicherung_bild.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "sicherung_bild: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "sicherung_bild: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "sicherung_bild: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# Eigenes Arbeitsverzeichnis je Aufruf (ein Rest des vorigen Aufrufs haelt sonst
# debug.log gesperrt).
string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${WORKDIR}")

execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "1150" "fach0"
                WORKING_DIRECTORY "${WORKDIR}"
                TIMEOUT 60
                RESULT_VARIABLE _rvk
                OUTPUT_VARIABLE _outk)
if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
    message(FATAL_ERROR "sicherung_bild: Kartenwerkzeug exit=${_rvk}\n${_outk}")
endif()

re15_start_spiel(_rv 180
    RE15_NO_INTRO=1
    RE15_NOAUDIO=1
    RE15_WINDOW_SCALE=3
    RE15_SOFTWARE_RENDER=1
    RE15_CONTINUE_TEST=1
    RE15_CARD_AUTO=1
    RE15_CARD_SLOT=0
    RE15_INV_SHOT=inv.bmp        # Schirm auf in F30, Abzug + Ende in F140
    RE15_INV_CHECK_SHOT=1        # CHECK auf Inventarplatz 0 = die Sicherung
    "${RE15_PC_EXE}")

if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "sicherung_bild: kein debug.log (exit=${_rv})")
endif()
# debug.log enthaelt auch Binaer-Bytes -> binaer-sicher als HEX lesen und suchen
file(READ "${WORKDIR}/debug.log" _lh HEX)

function(_hex_von _text _out)
    string(HEX "${_text}" _h)
    string(TOLOWER "${_h}" _h)
    set(${_out} "${_h}" PARENT_SCOPE)
endfunction()

function(_verlange _text _meldung)
    _hex_von("${_text}" _h)
    string(FIND "${_lh}" "${_h}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "sicherung_bild: ${_meldung}\n  erwartet im debug.log: '${_text}'\n"
                            "  (exit=${_rv}, Arbeitsverzeichnis ${WORKDIR})")
    endif()
endfunction()

_verlange("CONTINUE: resumed in room 1150"
          "der Lauf hat den Spielstand in ROOM1150 nicht geladen")
_verlange("[inv] acceptance shot"
          "Bild 140 wurde nicht erreicht (kein Abzug) - der Lauf ist vorher abgerissen, ueber "
          "die Sicherung sagt er nichts")
_verlange("[sicherung] Ladestelle main.c: Modal-Bild 0x40 weicht in 0 von 8064 Punkten ab, Icon-Tile 0x40 in 0 von 1200 Bytes"
          "main.c hat das Rohr-Bild / Rohr-Icon NICHT in den geladenen Puffer eingesetzt - "
          "das Aufnahme-Modal zeigt wieder den ausgelieferten 'Fuse Case' "
          "(re15_sicherung_bild_einsetzen vor re15_itps_set_data, "
          "re15_sicherung_icon_einsetzen vor re15_itemall_set_pix)")
_verlange("[inv] Sicherung im Statusschirm: Icon-Tile 0x40 weicht in 0 von 1200 Bytes ab, Bild-Block 0x40 in 0 von 12288 Bytes"
          "inv_render_pc.c hat Icon/Bild NICHT in die Puffer des Statusschirms eingesetzt - "
          "Raster und CHECK zeigen einen anderen Gegenstand als das Modal")
_verlange("[inv] CHECK-Foto Item 0x40: crect (0,489) prect (832,256) 56x72 -> Foto und CLUT geladen"
          "der CHECK auf der Sicherung laedt kein Foto in das Fenster (832,256) / CLUT (0,489) "
          "des RE1.5-Schirms - das Fotofeld bleibt leer")

message(STATUS "sicherung_bild: OK - Modal, Raster und CHECK lesen an allen vier "
               "PC-Ladestellen das Rohr; das CHECK-Foto der Sicherung wird geladen")
