# =============================================================================
# CUT-BLITZ-RIEGEL (Runde 30 Nachschliff, Spur cut-blitz).
# Dossier: analysis/befunde_runde30/nachschliff-cut-blitz.md
#
# BEFUND: beim Kamerawechsel zeigte der PC-Port EIN Bild lang die Projektion der neuen Kamera
# auf dem Hintergrund der alten (der Apply stand im Zeichenblock NACH dem Hintergrund-Blit).
# Gemessen bei 14 von 14 Wechseln mit 3D: Zonenwechsel zu Fuss und SCD Cut_chg.
#
# ORIGINAL: die Present-Routine FUN_8002137c ruft den Apply FUN_80021bbc (@0x80021558) und
# springt SOFORT hinter Zeichnen und Pufferwechsel (@0x80021560 j LAB_800215fc); der Apply
# schaltet Hintergrund (@0x80021d2c/@0x80021e34), Projektionsabstand (@0x80021e6c) und
# Blickmatrix (@0x80021e8c) in einem Zug. Jedes gezeigte Bild traegt in Hintergrund und
# Projektion denselben Cut.
#
# DIESER RIEGEL faehrt die ECHTE exe dreimal und liest die Messschiene RE15_CUT_SYNC_LOG
# (main.c pc_cut_sync_log: je Bild der Cut des tatsaechlich geblitteten Hintergrunds gegen den
# Cut, dessen Kameramatrix die 3D-Projektion trug, zurueckgerechnet aus cam_view):
#   A  ROOM1150 zu Fuss (Eingabe des Gegenpruefer-Laufs telefon2): Zonenwechsel 0->1 und 1->2
#      und das TELEFON-BILD F330 (SCD Cut_chg(6), der gemeldete Befund: Buch und Karte in
#      Cut-6-Lage auf dem Cut-2-Hintergrund). Nach F330 laeuft kein Spielbild mehr (das
#      Telefon oeffnet sein Fenster, gemessen: keine Zeile F331 in 240 s), deshalb endet A dort.
#   B  ROOM1170 nach dem Sprung: Tuer-Warp, Selbst-Wiedereintritt, SCD Cut_chg(2) mit 3D.
#      Zusaetzlich RE15_NECK_TRACE: im Anforderungsbild F445 (Selbst-Wiedereintritt, Cut 7)
#      steht Elliot ausserhalb des Regions-Vierecks und wird nicht gezeichnet (tris=0). Seine
#      Nacken-FSM muss trotzdem takten - im Original haengt sie am Takt (@0x8011d80c jal
#      FUN_80037358 in FUN_8011d6d4), nicht am Zeichnen (FUN_8001e8c8 @0x8001e9b4 setzt
#      gecullte Entities nur zusammen). Ohne das lag Elliots Kopf F448-F453 einen Schritt
#      zurueck (Gegenpruefer: 8..1020 Pixel).
#   C  NEW GAME, Vorspann-Montage ROOM1240: Feld mfx=<neu>/<vorige>. Das erste Bild muss
#      einblenden (neue Ebene > 0 solange Cut 0 steht) und beim Wechsel 0->1 muss die vorige
#      Ebene ausblenden (vorige > 0 unter Cut 1). Stand der Montage-Schalter hinter dem Apply,
#      verpuffte on_cut(0): beides blieb 0.
# Er verlangt in JEDEM Bild mit 3D (tris > 0): sync=1, oder das Bild liegt unter voller
# Tuerblende (fade0=255 = subtraktives Weiss, das Bild ist schwarz — gemessen: 0 nicht-schwarze
# Pixel im Warp-Bild). Und er verlangt die Wechsel selbst (A >= 2, B >= 1 sichtbare Wechsel mit
# 3D), damit ein verfehlter Weg nicht als gruen durchgeht.
#
# Mutationsproben (Dossier): Apply wieder HINTER den Hintergrund-Blit -> rot (A); Montage-
# Schalter hinter den Apply -> rot (C); gecullte NPCs wieder vor der Pose verwerfen -> rot (B).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r30_cut_blitz.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "cut_blitz: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "cut_blitz: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Wertet cutsync.log eines Laufs aus. _min_wechsel = verlangte Zahl sichtbarer Wechsel.
function(cut_sync_pruefen _name _log _raum _min_wechsel)
    if(NOT EXISTS "${_log}")
        message(FATAL_ERROR "cut_blitz[${_name}]: keine Messschiene ${_log}")
    endif()
    file(STRINGS "${_log}" _zeilen)
    set(_n 0)
    set(_n3d 0)
    set(_fehler "")
    set(_wechsel 0)
    set(_wechsel_txt "")
    set(_vor_raum "")
    set(_vor_view "")
    foreach(_z IN LISTS _zeilen)
        # CMake kennt nur CMAKE_MATCH_1..9 - deshalb nur die sechs gebrauchten Felder fangen.
        if(NOT _z MATCHES "^F([0-9]+) room=([0-9a-f]+) bg=[0-9a-f]+#-?[0-9]+ view=(-?[0-9]+) sync=(-?[0-9]+) req=[0-9]+ shown=-?[0-9]+ fade0=([0-9]+)/[0-9]+ tris=([0-9]+)")
            continue()
        endif()
        set(_f "${CMAKE_MATCH_1}")
        set(_room "${CMAKE_MATCH_2}")
        set(_view "${CMAKE_MATCH_3}")
        set(_sync "${CMAKE_MATCH_4}")
        set(_fade "${CMAKE_MATCH_5}")
        set(_tris "${CMAKE_MATCH_6}")
        math(EXPR _n "${_n} + 1")
        if(_tris GREATER 0)
            math(EXPR _n3d "${_n3d} + 1")
            if(NOT _sync EQUAL 1 AND NOT _fade EQUAL 255)
                string(APPEND _fehler "  ${_z}\n")
            endif()
        endif()
        # sichtbarer Wechsel: gleicher Raum, andere Kamera, 3D im Bild, keine volle Blende
        if(_room STREQUAL _vor_raum AND NOT _view STREQUAL _vor_view AND
           _tris GREATER 0 AND NOT _fade EQUAL 255 AND _room STREQUAL "${_raum}")
            math(EXPR _wechsel "${_wechsel} + 1")
            string(APPEND _wechsel_txt "  ${_z}\n")
        endif()
        set(_vor_raum "${_room}")
        set(_vor_view "${_view}")
    endforeach()
    message(STATUS "cut_blitz[${_name}]: ${_n} Bilder, ${_n3d} mit 3D, "
                   "${_wechsel} sichtbare Wechsel in ROOM${_raum}:\n${_wechsel_txt}")
    if(NOT _fehler STREQUAL "")
        message(FATAL_ERROR
            "cut_blitz[${_name}]: Bilder, deren 3D-Projektion einen ANDEREN Cut traegt als der "
            "Hintergrund (sync != 1 ausserhalb der vollen Tuerblende):\n${_fehler}"
            "Der Kamera-Apply muss VOR dem Hintergrund-Blit laufen (main.c "
            "pc_cam_present_apply; Original FUN_8002137c @0x80021558/@0x80021560).")
    endif()
    if(_wechsel LESS ${_min_wechsel})
        message(FATAL_ERROR
            "cut_blitz[${_name}]: nur ${_wechsel} sichtbare Kamerawechsel in ROOM${_raum}, "
            "verlangt ${_min_wechsel} - der Weg hat die Zonen/den Cut_chg verfehlt, der Riegel "
            "haette nichts geprueft.")
    endif()
endfunction()

# ---- A: ROOM1150 zu Fuss, Zonenwechsel F143 0->1 / F239 1->2, Telefon F330 ------------------
set(WORKDIR "${_basis}_a")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rva 240
    RE15_NO_INTRO=1
    RE15_NOAUDIO=1
    RE15_TITLE_SHOT=title.bmp
    RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=1150@240
    "RE15_INPUT_SCRIPT=R0.6333,W0.2,U2.4333,W0.2,L0.2667,W0.2,U2.8,W0.5,R0.3333,W0.5,A0.04,W3"
    RE15_INPUT_SCRIPT_START=330
    RE15_CUT_SYNC_LOG=cutsync.log
    "RE15_EXIT_AT=330#1150"
    "${RE15_PC_EXE}")
message(STATUS "cut_blitz[A]: exit=${_rva}")
cut_sync_pruefen(A "${WORKDIR}/cutsync.log" "1150" 2)
# Das Telefon-Bild selbst: SCD Cut_chg(6) ist angefordert (req=6), das Bild traegt 3D, und
# Hintergrund wie Projektion stehen noch im Cut 2 (der Apply folgt erst im naechsten Durchlauf;
# Original @0x80021558 am Ende des Anforderungsdurchlaufs, dessen OT verworfen wird).
file(STRINGS "${WORKDIR}/cutsync.log" _tel REGEX "^F330 room=1150 ")
if(NOT _tel MATCHES "bg=1150#2 view=2 sync=1 req=6 .* tris=[1-9]")
    message(FATAL_ERROR
        "cut_blitz[A]: Telefon-Bild F330 nicht wie verlangt (bg 1150#2, view 2, sync 1, req 6, "
        "3D im Bild) - gelesen: '${_tel}'. Leer = der Weg hat das Telefon verfehlt; view=6 = "
        "der gemeldete Blitz (Cut-6-Projektion auf Cut-2-Hintergrund).")
endif()
message(STATUS "cut_blitz[A]: Telefon ${_tel}")

# ---- B: ROOM1170 nach dem Sprung, SCD Cut_chg(2) mit 3D (F479/F480), Nacken gecullt ---------
set(WORKDIR "${_basis}_b")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rvb 240
    RE15_NO_INTRO=1
    RE15_NOAUDIO=1
    RE15_TITLE_SHOT=title.bmp
    RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=1170@240
    RE15_CUT_SYNC_LOG=cutsync.log
    RE15_NECK_TRACE=neck.log
    "RE15_EXIT_AT=490#1170"
    "${RE15_PC_EXE}")
message(STATUS "cut_blitz[B]: exit=${_rvb}")
cut_sync_pruefen(B "${WORKDIR}/cutsync.log" "1170" 1)
# Nacken ohne Zeichnen: mindestens ein Bild in ROOM1170 ohne 3D (tris=0), in dem die
# Nacken-FSM eines NPC trotzdem getaktet hat.
if(NOT EXISTS "${WORKDIR}/neck.log")
    message(FATAL_ERROR "cut_blitz[B]: keine Nacken-Messzeile ${WORKDIR}/neck.log")
endif()
file(STRINGS "${WORKDIR}/neck.log" _nz REGEX "^F[0-9]+ slot=")
file(STRINGS "${WORKDIR}/cutsync.log" _leer REGEX "^F[0-9]+ room=1170 .* tris=0 ")
set(_leer_bilder "")
foreach(_z IN LISTS _leer)
    if(_z MATCHES "^F([0-9]+) ")
        list(APPEND _leer_bilder "${CMAKE_MATCH_1}")
    endif()
endforeach()
set(_takt_ohne_bild "")
foreach(_z IN LISTS _nz)
    if(_z MATCHES "^F([0-9]+) slot=")
        list(FIND _leer_bilder "${CMAKE_MATCH_1}" _i)
        if(_i GREATER -1)
            string(APPEND _takt_ohne_bild "  ${_z}\n")
        endif()
    endif()
endforeach()
list(LENGTH _nz _nn)
message(STATUS "cut_blitz[B]: ${_nn} Nacken-Takte, davon in Bildern ohne 3D:\n${_takt_ohne_bild}")
if(_takt_ohne_bild STREQUAL "")
    message(FATAL_ERROR
        "cut_blitz[B]: kein Nacken-Takt in einem Bild ohne 3D. Ein NPC ausserhalb des "
        "Regions-Vierecks (Elliot im Anforderungsbild F445) muss weiter posiert werden: "
        "Original-Takt @0x8011d80c jal FUN_80037358 haengt nicht am Zeichnen, FUN_8001e8c8 "
        "setzt gecullte Entities nur zusammen (@0x8001e9b4 FUN_8001ef54). main.c "
        "npc_region_culled.")
endif()

# ---- C: NEW GAME, Vorspann-Montage ROOM1240 (Montage-Schalter vor dem Apply) ----------------
set(WORKDIR "${_basis}_c")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rvc 240
    RE15_NOAUDIO=1
    RE15_TITLE_SHOT=title.bmp
    RE15_TITLE_SHOT_AF=2
    RE15_CUT_SYNC_LOG=cutsync.log
    "RE15_EXIT_AT=200#1240"
    "${RE15_PC_EXE}")
message(STATUS "cut_blitz[C]: exit=${_rvc}")
file(STRINGS "${WORKDIR}/cutsync.log" _mz REGEX "^F[0-9]+ room=1240 ")
set(_ein 0)
set(_aus 0)
set(_nm 0)
foreach(_z IN LISTS _mz)
    if(NOT _z MATCHES " shown=([0-9]+) .* mfx=(-?[0-9]+)/(-?[0-9]+)")
        continue()
    endif()
    math(EXPR _nm "${_nm} + 1")
    if(CMAKE_MATCH_1 EQUAL 0 AND CMAKE_MATCH_2 GREATER 0)
        math(EXPR _ein "${_ein} + 1")
    endif()
    if(CMAKE_MATCH_1 EQUAL 1 AND CMAKE_MATCH_3 GREATER 0)
        math(EXPR _aus "${_aus} + 1")
    endif()
endforeach()
message(STATUS "cut_blitz[C]: ${_nm} Montagebilder, erstes Bild eingeblendet in ${_ein}, "
               "Ausblend-Ebene unter Cut 1 in ${_aus}")
if(_nm LESS 150)
    message(FATAL_ERROR "cut_blitz[C]: nur ${_nm} Montagebilder mit mfx-Feld - Vorspann verfehlt?")
endif()
if(_ein EQUAL 0 OR _aus EQUAL 0)
    message(FATAL_ERROR
        "cut_blitz[C]: Montage verpufft (erstes Bild ein: ${_ein}, Ausblenden unter Cut 1: "
        "${_aus}). re15_montage_fx_set_active muss VOR pc_cam_present_apply laufen, sonst "
        "kehrt re15_montage_fx_on_cut(0) sofort zurueck.")
endif()

message(STATUS "cut_blitz: OK - Hintergrund und Projektion tragen in jedem Bild denselben Cut")
