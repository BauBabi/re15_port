# =============================================================================
# SZENE ROOM10F0 MIT DER ECHTEN EXE (Runde 35, Spur K): der Abnahmeweg des Auftrags — Spielstand +
# CONTINUE in ROOM10D0 vor der Tuer zum Communication Room, Aktionstaste, echter Eintritt durch die
# Tuer, Szene laeuft im Zielraum GENAU EINMAL (zweiter Lauf mit gesetztem Flag (9,71): keine Szene).
#
# Dossier analysis/befunde_runde35/K_cut10f0.md, Konstanten include/re15_cut10f0.h. Die Unit-Riegel
# unit_r35_cut10f0_* pruefen die Mechanik Bild fuer Bild in der VM; dieser Riegel prueft, was nur die
# exe zeigt: Lade-Weg, Tuersequenz -> Raumwechsel, Gestenblock-Leihe (cut10f0_pc.c), Port-Bank-Tuerknall,
# Kartenhinweis-Kette ueber den Statusschirm (menu_common.c) und die Raummusik (audio_pc.c).
#
# Laeufe (Karte aus probe_r35_cut10f0_karte, Spieler (1900,0,-7000) Gierung 2048 vor der Tuer, CONTINUE,
# Eingabeskript auf der Spielbild-Zeitachse ab Bild 60):
#   A  ROOM10D0 -> Tuer -> ROOM10F0: "[cut10f0] ROOM10F0: Szene gestartet", Animationsblock von ROOM11B0
#      geliehen, Nachrichten 6..23 in dieser Reihenfolge, 2x Se_on Bank 14 (0x0E) Satz 1, "Szene zu Ende",
#      Hinweis 1 -> 2 (Folge) -> schliessen, danach Cut 2 (Leon im Bild).
#      NACHBESSERUNG 1 (Mangel 2): RE15_SET_FLAG_AT setzt (9,73) "Irons-Todesszene gesehen" MITTEN in einer
#      laufenden Szene (Stellvertreter fuer die 1150-Montage der Spur L) -> MAIN01 beginnt trotzdem erst
#      NACH dem Szenen-Ende ("MAIN01-Fenster auf", "entry=FF01")
#   B  wie A mit (9,71)=1 im Spielstand: kein "[cut10f0]", keine Nachricht 6..23, kein geliehener Block
#   C  CONTINUE direkt IN ROOM10F0 mit ausstehender Szene (Boot-Weg main.c): Szene + Leihe; ohne (9,73)
#      kein MAIN01
#   D  MAIN01 VON RAUM ZU RAUM: Stand in ROOM10F0 mit (9,71)=1 und (9,73)=1, Blick zur Tuer -> nach dem Laden
#      stoesst das erste Spielbild die Raummusik an ("room=0F entry=FF01"; der Boot-BGM-Aufruf lief vor dem
#      Restore), Aktionstaste -> Tuer -> ROOM10D0: "room=0D entry=FF01 ... [unveraendert, laeuft durch]"
#   E  ENDE AM PARKPLATZ: Stand in ROOM11B0 vor der Tuer nach ROOM11C0, Fenster offen -> "room=1B entry=FF01",
#      Tuer -> "room=1C entry=FF56" (eigene Musik des Parkplatzes), die Ankunftsszene setzt (4,64) ->
#      "MAIN01-Fenster zu"
#   F  MAIN01 ERST NACH DER MONTAGE (Mangel 2): Stand in ROOM1150 mit (9,71)=1 OHNE (9,73) -> Latch (9,72),
#      Raummusik = Tabelle ("room=15 entry=FF1E", kein FF01); bei Bild 60 setzt RE15_SET_FLAG_AT (9,73)
#      (keine Szene laeuft) -> "MAIN01-Fenster auf", "room=15 entry=FF01"
#   G  RAUMSKRIPT STOPPT DEN MAIN-KANAL (Mangel 1): Stand im Flur ROOM1180 (Strom an) vor der Tuer zum Zwinger,
#      alle fuenf Gegner des Zwingers tot, Fenster offen -> Tuer -> ROOM11D0: "room=1D entry=FF01 ... laeuft
#      durch", sub01 @0x01710 `54 00 02 00 00 00` (Stop an Slot 0) wird NICHT auf MAIN01 angewandt
#
# RE15_SOFTWARE_RENDER=1 dient nur der Robustheit des Testhakens; geprueft wird das LOG (debug.log).
# SDL_AUDIODRIVER=dummy, damit die BGM-Weiche auch ohne Audio-Endpunkt laeuft und "[bgm] ... entry=FF01"
# geschrieben wird (ohne Geraet kehrt re15_audio_start_room_bgm vorher zurueck).
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r35_cut10f0_karte> -DWORKDIR=<dir>
#               -P test_r35_cut10f0.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r35_cut10f0: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
    message(FATAL_ERROR "r35_cut10f0: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r35_cut10f0: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake): local_build.sh eines parallel bauenden Baums darf
# diesen Lauf nicht treffen.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r35k_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r35k_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

# szene_lauf(<name> <karten-argumente> <raum> <endbild> <out> [KEY=WERT ...])  — die Zusatz-Umgebung gilt nur
# fuer diesen Lauf (re15_start_spiel setzt sie danach zurueck).
function(szene_lauf _name _karte _raum _ende _out_debug)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" ${_karte}
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r35_cut10f0[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    #  F60 Aktionstaste an der Tuer (Tuersequenz, Raumwechsel), danach nur warten
    re15_start_spiel(_rv 600
        RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=A0.2,W5"
        RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_CAM_TRACE=1 RE15_BGM_CTL_DEBUG=1
        "RE15_EXIT_AT=${_ende}"
        ${ARGN}
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r35_cut10f0[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    string(FIND "${_dbg}" "CONTINUE: resumed in room ${_raum}" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r35_cut10f0[${_name}]: Spielstand in ROOM${_raum} nicht geladen (exit=${_rv}, ${WORKDIR})")
    endif()
    string(FIND "${_dbg}" "EXIT_AT: Bild" _p)
    if(_p LESS 0)
        message(FATAL_ERROR "r35_cut10f0[${_name}]: Endbild ${_ende} nicht erreicht - Tuer nicht genommen oder "
                            "Lauf abgerissen (exit=${_rv}, ${WORKDIR})")
    endif()
    set(${_out_debug} "${_dbg}" PARENT_SCOPE)
endfunction()

function(_pos _text _wort _out)
    string(FIND "${_text}" "${_wort}" _p)
    set(${_out} ${_p} PARENT_SCOPE)
endfunction()

# --- A: erster Eintritt -> Szene; (9,73) faellt MITTEN in die Szene -> MAIN01 erst nach ihrem Ende --------
# Endbild 3200: Szene bis ~F2730 (Zeilentakt 110), Hinweiskette bis ~F2990
szene_lauf(a "" 10d0 "3200#10f0" _dbg "RE15_SET_FLAG_AT=9:73@300")
_pos("${_dbg}" "[cut10f0] ROOM10F0: Szene gestartet" _ps)
if(_ps LESS 0)
    message(FATAL_ERROR "r35_cut10f0[A]: Szene nach dem Tuereintritt nicht gestartet")
endif()
_pos("${_dbg}" "Animationsblock von ROOM11B0 geliehen" _pr)
if(_pr LESS 0)
    message(FATAL_ERROR "r35_cut10f0[A]: Gestenblock von ROOM11B0 nicht geliehen (cut10f0_pc.c)")
endif()
# Nachrichten 6..23 in dieser Reihenfolge
set(_vorher -1)
foreach(_id RANGE 6 23)
    _pos("${_dbg}" "[msg] room=10f0 id=${_id} " _pm)
    if(_pm LESS 0)
        message(FATAL_ERROR "r35_cut10f0[A]: Nachricht ${_id} nie angezeigt")
    endif()
    if(NOT _pm GREATER _vorher)
        message(FATAL_ERROR "r35_cut10f0[A]: Nachricht ${_id} vor ihrer Vorgaengerin")
    endif()
    set(_vorher ${_pm})
endforeach()
string(REGEX MATCHALL "\\[se\\] SCD Se_on: bank=14 id=1" _knall "${_dbg}")
list(LENGTH _knall _nk)
if(NOT _nk EQUAL 2)
    message(FATAL_ERROR "r35_cut10f0[A]: Tuerknall (Se_on Bank 0x0E Satz 1) ${_nk}x statt 2x")
endif()
_pos("${_dbg}" "[cut10f0] Szene zu Ende" _pe)
if(_pe LESS 0)
    message(FATAL_ERROR "r35_cut10f0[A]: Szene nicht zu Ende gelaufen (Faden haengt)")
endif()
_pos("${_dbg}" "Folge-Hinweis 1 -> 2" _pf)
_pos("${_dbg}" "[hint] F" _ph)
if(_pf LESS 0 OR _ph LESS 0)
    message(FATAL_ERROR "r35_cut10f0[A]: Kartenhinweis-Kette ROOM11C0 -> ROOM1150 nicht gelaufen")
endif()
string(REGEX MATCH "\\[hint\\] F[0-9]+ schliessen" _hs "${_dbg}")
if(NOT _hs)
    message(FATAL_ERROR "r35_cut10f0[A]: Hinweis-Schirm hat sich nicht geschlossen")
endif()
# Mangel 2: (9,73) kam waehrend der Szene; MAIN01 (entry=FF01) und "Fenster auf" erst NACH dem Szenen-Ende
_pos("${_dbg}" "[setflag-at] Frame" _p73)
_pos("${_dbg}" "[cut10f0] MAIN01-Fenster auf in ROOM10F0" _pauf)
_pos("${_dbg}" "entry=FF01 -> MAIN01" _pb)
if(_p73 LESS 0 OR NOT _p73 LESS _pe)
    message(FATAL_ERROR "r35_cut10f0[A]: (9,73) wurde nicht WAEHREND der Szene gesetzt (setflag ${_p73}, Ende ${_pe})")
endif()
if(_pauf LESS 0 OR _pb LESS 0)
    message(FATAL_ERROR "r35_cut10f0[A]: nach Szene + (9,73) kein MAIN01 (Fenster ${_pauf}, entry=FF01 ${_pb})")
endif()
if(NOT _pauf GREATER _pe OR NOT _pb GREATER _pe)
    message(FATAL_ERROR "r35_cut10f0[A]: MAIN01 begann VOR dem Szenen-Ende (Fenster ${_pauf}, FF01 ${_pb}, Ende ${_pe})")
endif()
# Ende: Kamera wieder auf Cut 2 (Leon im Bild), nicht auf Cut 0 der Abgangsszene (letzte cam-trace-Zeile)
string(REGEX MATCHALL "\\[cam-trace\\] F[0-9]+ room=10f0 [^\n]*" _cts "${_dbg}")
list(LENGTH _cts _ncts)
if(_ncts EQUAL 0)
    message(FATAL_ERROR "r35_cut10f0[A]: keine cam-trace-Zeile in ROOM10F0")
endif()
list(GET _cts -1 _letzt)
string(REGEX MATCH "shown\\(s_last\\)=([0-9]+)" _m "${_letzt}")
if(NOT "${CMAKE_MATCH_1}" STREQUAL "2")
    message(FATAL_ERROR "r35_cut10f0[A]: nach der Szene steht die Kamera nicht auf Cut 2 (${_letzt})")
endif()
# Tuerknall wirklich auf die RE2-Tuerbank geleitet (audio_pc.c, nicht "VERWORFEN")
string(REGEX MATCHALL "\\[se\\] Se_on bank=14 id=1 -> RE2-Tuerbank" _tb "${_dbg}")
list(LENGTH _tb _ntb)
if(NOT _ntb EQUAL 2)
    message(FATAL_ERROR "r35_cut10f0[A]: Port-Bank 0x0E ${_ntb}x auf die RE2-Tuerbank geleitet statt 2x")
endif()

# --- B: Szene schon gesehen -> keine Szene -----------------------------------------------------------
szene_lauf(b "gesehen" 10d0 "300#10f0" _dbg)
_pos("${_dbg}" "[cut10f0]" _ps)
_pos("${_dbg}" "[msg] room=10f0 id=6 " _pm)
_pos("${_dbg}" "Animationsblock von ROOM11B0 geliehen" _pr)
if(NOT (_ps LESS 0 AND _pm LESS 0 AND _pr LESS 0))
    message(FATAL_ERROR "r35_cut10f0[B]: mit (9,71)=1 lief die Szene trotzdem (cut10f0 ${_ps}, msg ${_pm}, rbj ${_pr})")
endif()

# --- C: CONTINUE direkt IN ROOM10F0 mit ausstehender Szene (alter Spielstand) -> Lade-Weg (main.c Boot) ---
szene_lauf(c "in10f0" 10f0 "400#10f0" _dbg)
_pos("${_dbg}" "[cut10f0] ROOM10F0: Szene gestartet" _ps)
_pos("${_dbg}" "Animationsblock von ROOM11B0 geliehen" _pr)
_pos("${_dbg}" "[msg] room=10f0 id=6 " _pm)
if(_ps LESS 0 OR _pr LESS 0 OR _pm LESS 0)
    message(FATAL_ERROR "r35_cut10f0[C]: CONTINUE in ROOM10F0: Szene ${_ps}, Leihe ${_pr}, msg 6 ${_pm} (Boot-Weg)")
endif()
_pos("${_dbg}" "entry=FF01" _pb)
if(NOT _pb LESS 0)
    message(FATAL_ERROR "r35_cut10f0[C]: MAIN01 schon in der 10F0-Szene (ohne (9,73))")
endif()

# --- D: MAIN01 von Raum zu Raum (Laden im Fenster, dann durch die Tuer nach ROOM10D0) ---------------------
szene_lauf(d "gesehen;montage;raus" 10f0 "200#10d0" _dbg)
_pos("${_dbg}" "[cut10f0] ROOM10F0: Szene gestartet" _ps)
if(NOT _ps LESS 0)
    message(FATAL_ERROR "r35_cut10f0[D]: Szene lief trotz (9,71)=1 noch einmal")
endif()
_pos("${_dbg}" "CONTINUE: resumed in room 10f0" _pl)
_pos("${_dbg}" "[cut10f0] Raummusik-Anstoss in ROOM10F0: Soll MAIN01" _pa)
_pos("${_dbg}" "[bgm] stage=0 room=0F entry=FF01 -> MAIN01" _pb1)
_pos("${_dbg}" "[bgm] stage=0 room=0D entry=FF01 -> MAIN01" _pb2)
if(_pa LESS 0 OR _pb1 LESS 0 OR NOT _pb1 GREATER _pl)
    message(FATAL_ERROR "r35_cut10f0[D]: nach dem Laden in ROOM10F0 kein MAIN01 (Anstoss ${_pa}, FF01 ${_pb1}, Laden ${_pl})")
endif()
if(_pb2 LESS 0 OR NOT _pb2 GREATER _pb1)
    message(FATAL_ERROR "r35_cut10f0[D]: nach der Tuer in ROOM10D0 kein MAIN01 (entry=FF01)")
endif()
string(REGEX MATCH "\\[bgm\\] stage=0 room=0D entry=FF01[^\n]*laeuft durch" _durch "${_dbg}")
if(NOT _durch)
    message(FATAL_ERROR "r35_cut10f0[D]: MAIN01 wurde beim Raumwechsel neu gestartet statt durchzulaufen")
endif()
string(REGEX MATCH "\\[bgm\\] stage=0 room=0D entry=FF1F" _tab "${_dbg}")
if(_tab)
    message(FATAL_ERROR "r35_cut10f0[D]: ROOM10D0 spielt seine Tabellenmusik (FF1F) statt MAIN01")
endif()

# --- E: Ende am Parkplatz (ROOM11B0 -> Tuer -> ROOM11C0) ----------------------------------------------------
szene_lauf(e "gesehen;montage;vor11c0" 11b0 "100#11c0" _dbg)
_pos("${_dbg}" "CONTINUE: resumed in room 11b0" _pl)
_pos("${_dbg}" "[bgm] stage=0 room=1B entry=FF01 -> MAIN01" _pb1)
_pos("${_dbg}" "[bgm] stage=0 room=1C entry=FF56" _pb2)
if(_pb1 LESS 0 OR NOT _pb1 GREATER _pl)
    message(FATAL_ERROR "r35_cut10f0[E]: in ROOM11B0 (vor dem Parkplatz) kein MAIN01")
endif()
if(_pb2 LESS 0 OR NOT _pb2 GREATER _pb1)
    message(FATAL_ERROR "r35_cut10f0[E]: im Parkplatz ROOM11C0 nicht dessen eigene Musik (entry=FF56)")
endif()
string(REGEX MATCH "\\[bgm\\] stage=0 room=1C entry=FF01" _falsch "${_dbg}")
if(_falsch)
    message(FATAL_ERROR "r35_cut10f0[E]: MAIN01 laeuft im Parkplatz weiter")
endif()
# "Parkplatz erreicht" = die Ankunftsszene ROOM11C0 sub02 setzt (4,64) -> das Fenster schliesst
_pos("${_dbg}" "[cut10f0] MAIN01-Fenster zu in ROOM11C0" _pz)
if(_pz LESS 0 OR NOT _pz GREATER _pb2)
    message(FATAL_ERROR "r35_cut10f0[E]: im Parkplatz schliesst das MAIN01-Fenster nicht ((4,64) der Ankunftsszene)")
endif()
string(REGEX MATCH "MAIN01-Fenster zu in ROOM11C0[^\n]*Parkplatz erreicht \\(4,64\\)=1" _p464 "${_dbg}")
if(NOT _p464)
    message(FATAL_ERROR "r35_cut10f0[E]: Fenster schloss nicht ueber (4,64)")
endif()

# --- F: MAIN01 erst NACH der Montage (Mangel 2) + Karten-Latch (9,72) -------------------------------------
# Integration K+L (r35/integration): Mit (9,71)=1 und (3,94)=1 startet beim Betreten von ROOM1150 die
# Irons-Todesszene von Spur L (irons_tod_1150.c, Ereignis 21) — (9,73) setzt dann die Szene selbst, erst
# nach Montage und Rueckkehr (Lauf m3 des Bau-Agenten K, Dossier §9.7). Der fruehere Harness-Schalter
# RE15_SET_FLAG_AT=9:73@60 traf mitten in die laufende Szene und war nur im Zweig K allein sinnvoll.
# F1: ROOM1150 vor der Todesszene -> Latch (9,72), Tabellenmusik FF1E, Szene startet, KEIN MAIN01.
szene_lauf(f "gesehen;in1150" 1150 "150#1150" _dbg)
_pos("${_dbg}" "[cut10f0] ROOM1150 nach der Szene betreten: (9,72)=1" _pk)
if(_pk LESS 0)
    message(FATAL_ERROR "r35_cut10f0[F]: Raumaufbau ROOM1150 mit (9,71)=1 setzt den Latch (9,72) nicht")
endif()
_pos("${_dbg}" "[bgm] stage=0 room=15 entry=FF1E" _ptab)
if(_ptab LESS 0)
    message(FATAL_ERROR "r35_cut10f0[F]: ROOM1150 nach der 10F0-Szene spielt nicht seine Tabellenmusik (entry=FF1E)")
endif()
_pos("${_dbg}" "[irons-tod] ROOM1150 Zustand" _pit)
string(FIND "${_dbg}" "Szene startet (Programm 0)" _pst)
if(_pit LESS 0 OR _pst LESS 0)
    message(FATAL_ERROR "r35_cut10f0[F]: Irons-Todesszene (Spur L) startet in ROOM1150 nicht")
endif()
_pos("${_dbg}" "[cut10f0] MAIN01-Fenster auf in ROOM1150" _pauf)
_pos("${_dbg}" "[bgm] stage=0 room=15 entry=FF01 -> MAIN01" _pb1)
if(NOT _pauf LESS 0 OR NOT _pb1 LESS 0)
    message(FATAL_ERROR "r35_cut10f0[F]: MAIN01 schon waehrend der Irons-Todesszene (Fenster ${_pauf}, FF01 ${_pb1})")
endif()
# F2: (9,73)=1 (Todesszene gesehen) -> in ROOM1150 oeffnet das Fenster nach dem ersten VM-Lauf, MAIN01.
szene_lauf(f2 "gesehen;in1150;montage" 1150 "150#1150" _dbg)
_pos("${_dbg}" "[cut10f0] MAIN01-Fenster auf in ROOM1150" _pauf)
_pos("${_dbg}" "[bgm] stage=0 room=15 entry=FF01 -> MAIN01" _pb1)
if(_pauf LESS 0 OR _pb1 LESS 0)
    message(FATAL_ERROR "r35_cut10f0[F2]: mit (9,73)=1 kein MAIN01 in ROOM1150 (Fenster ${_pauf}, FF01 ${_pb1})")
endif()
string(FIND "${_dbg}" "Szene startet (Programm 0)" _pst)
if(NOT _pst LESS 0)
    message(FATAL_ERROR "r35_cut10f0[F2]: Irons-Todesszene startet trotz (9,73)=1 erneut")
endif()

# --- G: ein Raumskript stoppt den MAIN-Kanal (Mangel 1) -----------------------------------------------------
# Flur ROOM1180 (Strom an) -> Tuer -> Zwinger ROOM11D0, alle fuenf Gegner tot: MAIN01 laeuft durch die Tuer,
# sub01 schickt im ersten Spielbild den Stop an Slot 0 — er darf MAIN01 nicht treffen.
szene_lauf(g "gesehen;montage;vor11d0" 1180 "200#11d0" _dbg)
_pos("${_dbg}" "[bgm] stage=0 room=18 entry=FF01 -> MAIN01" _pb1)
if(_pb1 LESS 0)
    message(FATAL_ERROR "r35_cut10f0[G]: im Flur ROOM1180 kein MAIN01 nach dem Laden")
endif()
string(REGEX MATCH "\\[bgm\\] stage=0 room=1D entry=FF01[^\n]*laeuft durch" _durch "${_dbg}")
_pos("${_dbg}" "[bgm] stage=0 room=1D entry=FF01" _pb2)
if(NOT _durch OR _pb2 LESS 0)
    message(FATAL_ERROR "r35_cut10f0[G]: MAIN01 laeuft nicht durch die Tuer in den Zwinger ROOM11D0")
endif()
string(SUBSTRING "${_dbg}" ${_pb2} -1 _rest)
_pos("${_rest}" "Sce_bgm_control slot=0 op=2 im MAIN01-Fenster NICHT angewandt" _pn)
if(_pn LESS 0)
    message(FATAL_ERROR "r35_cut10f0[G]: ROOM11D0 sub01 @0x01710 (Sce_bgm_control Slot 0 op 2) kam nicht an der Weiche an")
endif()
string(REGEX MATCH "Sce_bgm_control slot=0 op=[0-9]+ \\(part=" _stop "${_rest}")
if(_stop)
    message(FATAL_ERROR "r35_cut10f0[G]: ein Skript-Befehl an den MAIN-Slot wurde im Zwinger auf MAIN01 ANGEWANDT (${_stop})")
endif()

file(REMOVE "${_exe_kopie}")
message(STATUS "r35_cut10f0: OK - Eintritt durch die Tuer, Szene genau einmal, Hinweiskette, MAIN01 erst nach "
               "(9,73) und nach dem Szenen-Ende, von Raum zu Raum bis zum Parkplatz ((4,64)), Raumskript-Stop "
               "im Fenster nicht angewandt, Karten-Latch ROOM1150, Boot-Weg")
