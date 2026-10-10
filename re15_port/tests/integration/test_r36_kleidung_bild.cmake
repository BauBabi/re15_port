# =============================================================================
# BLUTIGE/ZERRISSENE KLEIDUNG IM BILD (Runde 36, Spur B "kleidung").
#
# Dossier analysis/befunde_runde36/B_kleidung.md §5/§6. Der Unit-Riegel unit_r36_kleidung_griff_*
# misst die MECHANIK (ueberlebter Griff -> Wund-Tabelle). Dieser Riegel misst, was nur die exe zeigt:
# die Wund-Tabelle eines Spielstands (v9 wounds[8][2], re15_savedata.c:212) kommt ueber CONTINUE
# (re15_wound_load -> main.c Blut-Decal-Sync -> render_pc.c re15_render_pc_wound_blit) auf der
# Spieler-Textur an, fuer Leon (PL00, LUT-Zeile 0 @0x80074208) UND Elza (PL04, LUT-Zeile 4
# @0x80074248 — vorher nahm der Port Zeile 1).
#
# Laeufe (ROOM1130/1131 Cut 2, Leon/Elza bei (-1024,0,-3336) rot 856, hp 20 = Danger, Bild 150):
#   LA  Leon ohne Wunden            LA2 dasselbe noch einmal (Determinismus: 0 Pixel Unterschied)
#   LB  Leon alle 8 Panels Stufe 2  LC  Leon Panels 5/7 Stufe 1 (= drei ueberlebte Griffe von vorn,
#                                       Helper 0x8010a1cc (0,10)(5,50)(7,50): 150 >= 120)
#   EA  Elza ohne Wunden            EB  Elza alle 8 Panels Stufe 2
# Mass: Pixel-Differenz der Framedumps (RE15_WINDOW_SCALE=1, 320x240) — alle Abweichungen liegen im
# Figurenkasten x177..247 / y81..205 (befund.log: kasten=x186..228,y137..214, scr=(207,211)),
# Schwellen = halbe gemessene Werte (gemessen 2026-10-10: LA/LB 737, LA/LC 250, EA/EB 458).
#
# SDL_ASSERT=always_ignore (Nachbesserung 1, M2): SDL-eigener Assertion-Handler, kein Spielverhalten —
# in einer RDP-Sitzung blieb jede exe an WIN_AddDisplay (SDL_windowsmodes.c:380) stehen (B_abnahme_0.md §5).
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<test_r36_kleidung> -DWORKDIR=<dir> -P <diese Datei>
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r36_kleidung_bild: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r36_kleidung_bild: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r36_kleidung_bild: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake: local_build.sh raeumt re15_pc.exe-Prozesse ab).
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r36b_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe "${_exe_dir}/re15_pc_r36b_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe}")

# Ein Lauf: Karte schreiben (Slot 0), CONTINUE, Framedump Bild 150, Ende Bild 160.
function(kleid_lauf _name _raum _char _wunden _out_bild)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" karte "re15_card.mcr" "${_raum}" "${_char}"
                            -1024 0 -3336 856 20 "${_wunden}" 2
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60 RESULT_VARIABLE _rvk OUTPUT_VARIABLE _ok)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r36_kleidung_bild[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_ok}")
    endif()
    re15_start_spiel(_rv 200
        SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        "RE15_FRAMEDUMP=150:bild.ppm"
        "RE15_EXIT_AT=160#${_raum}"
        "${_exe}")
    if(NOT EXISTS "${WORKDIR}/bild.ppm")
        message(FATAL_ERROR "r36_kleidung_bild[${_name}]: Framedump fehlt (exit=${_rv})")
    endif()
    message(STATUS "r36_kleidung_bild[${_name}] exit=${_rv} ${_ok}")
    set(${_out_bild} "${WORKDIR}/bild.ppm" PARENT_SCOPE)
endfunction()

# Vergleich ueber das Werkzeug (teil bild): min_diff 0 = bitgleich, sonst >= min_diff und alles im Kasten.
function(kleid_vergleich _name _a _b _min)
    execute_process(COMMAND "${RE15_KARTE_TOOL}" bild "${_a}" "${_b}" "${_min}" 177 81 247 205
                    TIMEOUT 60 RESULT_VARIABLE _rv OUTPUT_VARIABLE _out)
    message(STATUS "r36_kleidung_bild[${_name}] ${_out}")
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r36_kleidung_bild[${_name}]: Bildvergleich fehlgeschlagen (exit=${_rv})\n${_out}")
    endif()
endfunction()

kleid_lauf(LA  1130 0 "-"       _la)
kleid_lauf(LA2 1130 0 "-"       _la2)
kleid_lauf(LB  1130 0 "alle:2"  _lb)
kleid_lauf(LC  1130 0 "5:1,7:1" _lc)
kleid_lauf(EA  1131 4 "-"       _ea)
kleid_lauf(EB  1131 4 "alle:2"  _eb)

kleid_vergleich("Determinismus LA/LA2" "${_la}" "${_la2}" 0)
kleid_vergleich("Leon Fine/alle Panels Stufe 2" "${_la}" "${_lb}" 350)
kleid_vergleich("Leon Fine/drei Griffe von vorn (5,7 Stufe 1)" "${_la}" "${_lc}" 120)
kleid_vergleich("Elza Fine/alle Panels Stufe 2" "${_ea}" "${_eb}" 220)

file(REMOVE "${_exe}")
message(STATUS "r36_kleidung_bild: OK — Wunden der Spielstaende erscheinen auf Leons und Elzas Kleidung")
