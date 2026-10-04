# =============================================================================
# RUNDE 35 SPUR I — ENTLADEN-PIN (echte exe). Dossier analysis/befunde_runde35/I_entladen.md.
#
# NUTZER: "Nachdem ich gestorben bin und new game mache habe ich teilweise noch PRIs von meinen
# Spielstand davor ... Wenn man tot ist, aber auch wenn man den Raum wechselt sollen saemtliche
# Assets von den Raeumen davor entladen sein."
#
# Messschiene: RE15_ENTLADEN_LOG (platform/pc/src/entladen_pc.c). Jede Grenze (raum / spielstart /
# spielende) schreibt
#   EREIGNIS <grenze> ... | belegt <fach>=n ...   Belegung DIREKT nach dem Entladen (muss 0 sein)
#   SUMME seit=<grenze> bilder=.. bilder_mit_masken=.. bilder_fremd_belegt=.. ...
#        bilder_fremde_masken_gezeichnet=..       Bilanz der Strecke bis zur naechsten Grenze
# und BILD-Zeilen nur, wenn ein Bild einen FREMDEN Eintrag (aeltere Generation) belegt oder zeichnet.
#
# Laeufe (je eigene exe-Kopie, eigenes Arbeitsverzeichnis):
#   A  Tod -> NEW GAME: ROOM1020 (24 Masken in Cut 0) betreten, sterben, neues Spiel, wieder sterben.
#      VORHER gemessen: 23/23 Titelbilder zeichneten die 24 ROOM1020-Masken, im neuen Spiel 301/301
#      Bilder mit 12 fremden TIM-Slots + der ROOM1020-RDT.
#   B  Raumwechsel ROOM1020 -> ROOM1030. VORHER: 4 fremde TIM-Slots (12/26/27/47) in jedem Bild.
#   C  Tod -> LOAD im Todesraum (gleicher Raum, gleicher Cut): die Masken muessen im zweiten Spiel
#      WIEDER gezeichnet werden (PRI-Riegel je Generation; ohne ihn fehlen sie bis zum Cut-Wechsel).
#   D  Tod in ROOM1170 -> NEW GAME -> Montage -> echte Tuer nach ROOM1170: die Cinematic-Bank 1170
#      muss im zweiten Spiel NEU gebunden werden (Original `jal 0x8001b3f8` @0x80039a08 bei jedem
#      Raumladen). GEGENPROBE gemessen (Dossier M3): ohne den Riegel-Fix fehlt die Zeile.
#      Nachbesserung 1 (M2): Elliot (Typ 0x47) ist ein Raum-Modell (Sce_em_set `jal 0x80022300`
#      @0x80042328 in die Arena) — er darf in ROOM1240 NICHT resident sein, wird beim Spawn in 1170
#      geladen (debug.log "[elliot] PL05 loaded ... (Raum 1170, Spawn 0x47)") und faellt am Tod
#      (VORHER spielende: figur=1 + TIM-Slot 1 belegt, EREIGNIS: alles 0).
#   E  Nachbesserung 1 (M1): Raum-Stimmen. Intro ROOM1240 (main00..05) -> Montage-Tuer -> ROOM1170
#      mit Ton (SDL_AUDIODRIVER=dummy). VORHER raum (1240) muss Clips belegt haben (sonst misst der
#      Lauf nichts), EREIGNIS raum: stimme=0. VORHER Abnahme 0: 6 Clips ~3,2 MB blieben resident.
#
# Nachbesserung 2 (Abnahme 1, M1 + H1-H3) — Fach rbj (Raum-Animationsbank, Original RDT+0x5C @0x8001b404,
# RDT in der Arena @0x800397e8, Reset @0x80039738), bg_prev (Montage-Schnappschuss), re2ton (RE2-Raumbank):
#   F  Intro -> 1170 -> echte Tuer 4 -> 1130 (240 Bilder/s): VORHER raum 1170 rbj=1 + rbj_datei 1170/55060,
#      EREIGNIS 0, 100 Bilder ROOM1130 ohne fremde Belegung; VORHER raum 1240 bg_prev=1 (H1).
#   D  (erweitert) Tod in 1170 mit geladener RBJ-Datei: VORHER spielende rbj=1 + rbj_datei 1170.
#   G  Leihe Spur K: ROOM10F0 (Block von ROOM11B0) -> 1030: VORHER raum 10F0 rbj=1, EREIGNIS 0.
#   H  Boot-Weg: Karte ROOM1170 + CONTINUE: VORHER raum 1170 rbj=1 + rbj_datei 1170 (Boot-Puffer).
#   I  RE2-Raumbank TUERSE (Dummy-Ton): verschlossene Tuer 1170 (AOT 5) -> 1130: re2ton>=1 -> 0.
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r35_entladen_karte> -DWORKDIR=<dir>
#               [-DTEIL=A|B|C|D|alle] -P test_r35_entladen.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r35_entladen: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r35_entladen: WORKDIR fehlt")
endif()
if(NOT TEIL)
    set(TEIL alle)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigene exe-Kopie: fremde Kills (local_build.sh anderer Baeume, Nutzer) treffen diesen Lauf nicht.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
set(_exe_kopie "${_exe_dir}/re15_pc_r35_entladen_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

# --- Hilfen -------------------------------------------------------------------------------------
function(entladen_lauf _name _timeout)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/entladen.log")
    if(_name STREQUAL "e" OR _name STREQUAL "i")   # mit Ton: Dummy-Treiber (kein Geraet noetig), Stimmen werden dekodiert
        set(_ton SDL_AUDIODRIVER=dummy)
    else()
        set(_ton RE15_NOAUDIO=1)
    endif()
    re15_start_spiel(_rv ${_timeout} RE15_NO_INTRO=1 ${_ton}
                     "RE15_ENTLADEN_LOG=${WORKDIR}/entladen.log" ${ARGN} "${_exe_kopie}")
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r35_entladen[${_name}]: exe exit=${_rv}")
    endif()
    if(NOT EXISTS "${WORKDIR}/entladen.log")
        message(FATAL_ERROR "r35_entladen[${_name}]: kein entladen.log")
    endif()
endfunction()

# Prueft das Log eines Laufs: Pflicht-Grenzen vorhanden, jede EREIGNIS-Zeile ohne Belegung, jede
# SUMME ohne fremde Bilder, keine BILD-Zeile.
function(entladen_pruefen _name _min_raum _min_start _min_ende)
    set(_log "${_basis}_${_name}/entladen.log")
    file(STRINGS "${_log}" _ereig REGEX "^EREIGNIS ")
    file(STRINGS "${_log}" _summe REGEX "^SUMME ")
    file(STRINGS "${_log}" _bild  REGEX "^BILD ")
    foreach(_g raum spielstart spielende)
        file(STRINGS "${_log}" _z REGEX "^EREIGNIS ${_g} ")
        list(LENGTH _z _n)
        if(_g STREQUAL "raum")
            set(_min ${_min_raum})
        elseif(_g STREQUAL "spielstart")
            set(_min ${_min_start})
        else()
            set(_min ${_min_ende})
        endif()
        if(_n LESS _min)
            message(FATAL_ERROR "r35_entladen[${_name}]: nur ${_n} Grenze(n) '${_g}' (erwartet >= ${_min})")
        endif()
    endforeach()
    foreach(_z IN LISTS _ereig)
        string(REGEX MATCH "\\| belegt [^|]*" _bel "${_z}")
        if(_bel MATCHES "=[1-9]")
            message(FATAL_ERROR "r35_entladen[${_name}]: nach dem Entladen noch belegt (Arena-Reset "
                                "@0x80039738 / @0x8001d5a0 laesst nichts uebrig): '${_z}'")
        endif()
    endforeach()
    foreach(_z IN LISTS _summe)
        if(NOT _z MATCHES "bilder_fremd_belegt=0 " OR NOT _z MATCHES "bilder_fremde_masken_gezeichnet=0 ")
            message(FATAL_ERROR "r35_entladen[${_name}]: fremde Eintraege eines Raums/Spielstands davor: '${_z}'")
        endif()
    endforeach()
    list(LENGTH _bild _nb)
    if(_nb GREATER 0)
        list(GET _bild 0 _b0)
        message(FATAL_ERROR "r35_entladen[${_name}]: ${_nb} BILD-Zeile(n) mit fremder Belegung, erste: '${_b0}'")
    endif()
endfunction()

# Liefert bilder_mit_masken der n-ten (0-basiert) SUMME-Zeile mit dem Anlass _seit.
function(summe_masken _name _seit _index _out)
    file(STRINGS "${_basis}_${_name}/entladen.log" _z REGEX "^SUMME seit=${_seit} ")
    list(LENGTH _z _n)
    if(_index GREATER_EQUAL _n)
        message(FATAL_ERROR "r35_entladen[${_name}]: keine ${_index}. SUMME seit=${_seit} (nur ${_n})")
    endif()
    list(GET _z ${_index} _zeile)
    string(REGEX MATCH "bilder_mit_masken=([0-9]+)" _m "${_zeile}")
    set(${_out} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

# --- A: Tod -> NEW GAME -------------------------------------------------------------------------
if(TEIL STREQUAL "A" OR TEIL STREQUAL "alle")
    entladen_lauf(a 240 RE15_TITLE_SHOT=t.bmp RE15_DEBUG_JUMP=1020@5 RE15_KILL_AT=40 RE15_BOOT_EXIT_AT=3)
    entladen_pruefen(a 1 3 2)
    # Die eigenen Masken des Raums bleiben: ROOM1020 Cut 0 zeichnet seine 24 Masken weiter.
    summe_masken(a raum 0 _m1020)
    if(_m1020 LESS 1)
        message(FATAL_ERROR "r35_entladen[a]: ROOM1020 zeichnete keine eigenen Masken (bilder_mit_masken=${_m1020})")
    endif()
    # Titel zwischen Tod und neuem Spiel: keine Maske (Original zeichnet nur @0x8001ce54).
    summe_masken(a spielende 0 _mtitel)
    if(NOT _mtitel EQUAL 0)
        message(FATAL_ERROR "r35_entladen[a]: ${_mtitel} Titelbilder nach dem Tod mit Raum-Masken")
    endif()
    message(STATUS "r35_entladen[A] OK: Tod -> NEW GAME, 0 fremde Eintraege, ROOM1020-Masken ${_m1020} Bilder")
endif()

# --- B: Raumwechsel 1020 -> 1030 ----------------------------------------------------------------
if(TEIL STREQUAL "B" OR TEIL STREQUAL "alle")
    entladen_lauf(b 240 RE15_TITLE_SHOT=t.bmp RE15_DEBUG_JUMP=1020@5 RE15_GOTO_ROOM=1030
                        RE15_KILL_AT=150 RE15_BOOT_EXIT_AT=2)
    entladen_pruefen(b 2 1 1)
    file(STRINGS "${_basis}_b/debug.log" _r1030 REGEX "PC loaded room1030")
    if(NOT _r1030)
        message(FATAL_ERROR "r35_entladen[b]: ROOM1030 wurde nicht betreten")
    endif()
    summe_masken(b raum 1 _m1030)
    if(_m1030 LESS 1)
        message(FATAL_ERROR "r35_entladen[b]: ROOM1030 zeichnete keine eigenen Masken (bilder_mit_masken=${_m1030})")
    endif()
    message(STATUS "r35_entladen[B] OK: Raumwechsel 1020 -> 1030, 0 fremde Eintraege, ROOM1030-Masken ${_m1030} Bilder")
endif()

# --- C: Tod -> LOAD im Todesraum (gleicher Raum + Cut) --------------------------------------------
if(TEIL STREQUAL "C" OR TEIL STREQUAL "alle")
    if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
        message(FATAL_ERROR "r35_entladen[c]: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
    endif()
    file(MAKE_DIRECTORY "${_basis}_c")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" 1020 -26000 0 -8700 0
                    WORKING_DIRECTORY "${_basis}_c" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0)
        message(FATAL_ERROR "r35_entladen[c]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    entladen_lauf(c 240 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
                        RE15_KILL_AT=60 RE15_BOOT_EXIT_AT=3)
    entladen_pruefen(c 0 3 2)
    # Beide Spiele (Lade-Weg) beginnen in ROOM1020 Cut 0 und muessen ihre Masken zeichnen.
    summe_masken(c spielstart 0 _ms1)
    summe_masken(c spielstart 1 _ms2)
    if(_ms1 LESS 1 OR _ms2 LESS 1)
        message(FATAL_ERROR "r35_entladen[c]: Masken nach dem Laden fehlen (Spiel 1: ${_ms1}, Spiel 2 "
                            "nach Tod im selben Raum/Cut: ${_ms2} Bilder) — PRI-Riegel muss nach dem "
                            "Entladen neu ableiten")
    endif()
    # Nachbesserung 1 (M2): die Boot-Lader-Slots 2/3 ("Heli/Pilot" = Objekte 2/5 der BOOT-RDT, hier
    # ROOM1020) sind Raum-Bytes (RDT ab Arena-Basis @0x800397e8) — gezaehlt und am Tod entladen.
    file(STRINGS "${_basis}_c/entladen.log" _vende REGEX "^VORHER spielende ")
    list(LENGTH _vende _nvende)
    if(_nvende LESS 1)
        message(FATAL_ERROR "r35_entladen[c]: keine Grenze spielende")
    endif()
    list(GET _vende 0 _ve0)
    if(NOT "${_ve0} " MATCHES "tim_slots 2 3 ")
        message(FATAL_ERROR "r35_entladen[c]: Slots 2/3 (Objekte 2/5 der Boot-RDT ROOM1020) nicht als "
                            "Raum-Slots belegt/gezaehlt: '${_ve0}'")
    endif()
    message(STATUS "r35_entladen[C] OK: Tod -> LOAD im Todesraum, Masken ${_ms1}/${_ms2} Bilder, 0 fremd, Slots 2/3 entladen")
endif()

# --- D: Cinematic-Bank ueber den Tod ------------------------------------------------------------
if(TEIL STREQUAL "D" OR TEIL STREQUAL "alle")
    # 240 Bilder/s Wandzeit (Montage-Tuer in Spiel 2 gemessen bei Bild 2842; Tod danach bei 3200).
    entladen_lauf(d 400 RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_GOTO_ROOM=1170
                        RE15_KILL_AT=3200 RE15_BOOT_EXIT_AT=3)
    entladen_pruefen(d 2 3 2)
    file(STRINGS "${_basis}_d/debug.log" _bank REGEX "\\[rbj\\] room 1170 cinematic overlay")
    file(STRINGS "${_basis}_d/debug.log" _ein  REGEX "PC loaded room1170")
    list(LENGTH _bank _nbank)
    list(LENGTH _ein _nein)
    if(_nein LESS 2)
        message(FATAL_ERROR "r35_entladen[d]: ROOM1170 nur ${_nein}x betreten (erwartet: Spiel 1 GOTO + Spiel 2 Tuer)")
    endif()
    if(NOT _nbank EQUAL _nein)
        message(FATAL_ERROR "r35_entladen[d]: ROOM1170 ${_nein}x betreten, Cinematic-Bank nur ${_nbank}x gebunden "
                            "— Original bindet bei JEDEM Raumladen neu (jal 0x8001b3f8 @0x80039a08)")
    endif()
    # Nachbesserung 1 (M2): Elliot nur im Raum, in dem er gesetzt wird.
    file(STRINGS "${_basis}_d/entladen.log" _v1240 REGEX "^VORHER raum gen=[0-9]+ raum=1240 ")
    foreach(_z IN LISTS _v1240)
        string(REGEX MATCH "[|] belegt [^|]*" _bel "${_z}")
        string(REGEX MATCH "[|] tim_slots[^|]*" _sl "${_z} ")
        if(NOT _bel MATCHES " figur=0" OR _sl MATCHES " 1 ")
            message(FATAL_ERROR "r35_entladen[d]: Elliot in ROOM1240 resident (Original: Modell erst mit "
                                "Sce_em_set, jal 0x80022300 @0x80042328): '${_z}'")
        endif()
    endforeach()
    file(STRINGS "${_basis}_d/debug.log" _el REGEX "elliot. PL05 loaded")
    list(LENGTH _el _nel)
    if(_nel LESS 1)
        message(FATAL_ERROR "r35_entladen[d]: Elliot nie geladen (kein '[elliot] PL05 loaded')")
    endif()
    foreach(_z IN LISTS _el)
        if(NOT _z MATCHES "Raum 1170, Spawn 0x47")
            message(FATAL_ERROR "r35_entladen[d]: Elliot ausserhalb seines Spawns geladen: '${_z}'")
        endif()
    endforeach()
    file(STRINGS "${_basis}_d/entladen.log" _vende REGEX "^VORHER spielende ")
    set(_elliot_am_tod 0)
    foreach(_z IN LISTS _vende)
        string(REGEX MATCH "[|] tim_slots[^|]*" _sl "${_z} ")
        if(_z MATCHES " figur=1 " AND _sl MATCHES " 1 ")
            set(_elliot_am_tod 1)
        endif()
    endforeach()
    if(NOT _elliot_am_tod)
        message(FATAL_ERROR "r35_entladen[d]: kein Tod mit geladenem Elliot (figur=1 + TIM-Slot 1) — "
                            "der Lauf misst das Entladen nicht")
    endif()
    # Nachbesserung 2 (M1): der Tod in 1170 trifft eine GELADENE RBJ-Datei (Vorbedingung) — das Ereignis
    # (entladen_pruefen) laesst nichts uebrig.
    set(_rbj_am_tod 0)
    foreach(_z IN LISTS _vende)
        if(_z MATCHES " raum=1170 " AND _z MATCHES " rbj=1 " AND _z MATCHES "rbj_datei raum=1170 bytes=55060 ")
            set(_rbj_am_tod 1)
        endif()
    endforeach()
    if(NOT _rbj_am_tod)
        message(FATAL_ERROR "r35_entladen[d]: kein Tod in 1170 mit geladener RBJ-Datei (rbj=1, rbj_datei 1170) — "
                            "der Lauf misst Mangel 1 (Abnahme 1) nicht")
    endif()
    message(STATUS "r35_entladen[D] OK: Cinematic-Bank 1170 in beiden Spielen gebunden (${_nbank}/${_nein}), "
                   "Elliot ${_nel}x beim Spawn geladen, am Tod entladen, in 1240 nie resident")
endif()

# --- E: Raum-Stimmen (Nachbesserung 1, M1) ------------------------------------------------------
if(TEIL STREQUAL "E" OR TEIL STREQUAL "alle")
    entladen_lauf(e 300 RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60 "RE15_EXIT_AT=60#1170")
    entladen_pruefen(e 1 1 0)
    file(STRINGS "${_basis}_e/debug.log" _clips REGEX "voice. clip loaded")
    list(LENGTH _clips _nclips)
    file(STRINGS "${_basis}_e/entladen.log" _v REGEX "^VORHER raum gen=[0-9]+ raum=1240 ")
    list(LENGTH _v _nv)
    if(_nv LESS 1)
        message(FATAL_ERROR "r35_entladen[e]: keine Grenze 1240 -> 1170")
    endif()
    list(GET _v 0 _v0)
    string(REGEX MATCH " stimme=([0-9]+)" _m "${_v0}")
    set(_nst "${CMAKE_MATCH_1}")
    if(_nclips LESS 1 OR NOT _nst OR _nst LESS 1)
        message(FATAL_ERROR "r35_entladen[e]: vor der Grenze keine Stimme geladen (Clips ${_nclips}, "
                            "Fach stimme='${_nst}') — der Lauf misst das Entladen nicht: '${_v0}'")
    endif()
    message(STATUS "r35_entladen[E] OK: ${_nst} Raum-Stimmen aus ROOM1240 an der Grenze entladen, 0 fremd")
endif()

# Nachbesserung 2: erste VORHER-Zeile <grenze> im Raum <raum> des Laufs <name> nach <out>.
function(vorher_zeile _name _grenze _raum _out)
    file(STRINGS "${_basis}_${_name}/entladen.log" _v REGEX "^VORHER ${_grenze} gen=[0-9]+ raum=${_raum} ")
    list(LENGTH _v _nv)
    if(_nv LESS 1)
        message(FATAL_ERROR "r35_entladen[${_name}]: keine Grenze '${_grenze}' aus ROOM${_raum}")
    endif()
    list(GET _v 0 _v0)
    set(${_out} "${_v0} " PARENT_SCOPE)
endfunction()

# --- F: Raum-Animationsbank ueber eine echte Tuer (Nachbesserung 2, M1 + H1) ----------------------
if(TEIL STREQUAL "F" OR TEIL STREQUAL "alle")
    entladen_lauf(f 300 RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60
                        "RE15_FIRE_AOT=4@3000#1170" "RE15_EXIT_AT=100#1130")
    entladen_pruefen(f 2 1 0)
    file(STRINGS "${_basis}_f/debug.log" _r1130 REGEX "room 1130 has no RBJ")
    if(NOT _r1130)
        message(FATAL_ERROR "r35_entladen[f]: ROOM1130 (Raum ohne Animationsblock) nicht ueber die Tuer betreten")
    endif()
    vorher_zeile(f raum 1170 _v1170)
    if(NOT _v1170 MATCHES " rbj=1 " OR NOT _v1170 MATCHES "rbj_datei raum=1170 bytes=55060 ")
        message(FATAL_ERROR "r35_entladen[f]: an der Tuer 1170 -> 1130 keine geladene RBJ-Datei (Vorbedingung): '${_v1170}'")
    endif()
    vorher_zeile(f raum 1240 _v1240)
    if(NOT _v1240 MATCHES " bg_prev=1 ")
        message(FATAL_ERROR "r35_entladen[f]: Montage-Schnappschuss in ROOM1240 nicht belegt (Vorbedingung H1): '${_v1240}'")
    endif()
    message(STATUS "r35_entladen[F] OK: RBJ/ROOM1170.RBJ (55060 B) an der Tuer 1170 -> 1130 entladen, bg_prev an der Tuer 1240 -> 1170")
endif()

# --- G: Leihe Spur K (ROOM10F0 <- ROOM11B0) ---------------------------------------------------------
if(TEIL STREQUAL "G" OR TEIL STREQUAL "alle")
    entladen_lauf(g 240 RE15_TITLE_SHOT=t.bmp RE15_DEBUG_JUMP=10F0@5 RE15_GOTO_ROOM=1030 "RE15_EXIT_AT=100#1030")
    entladen_pruefen(g 2 1 0)
    file(STRINGS "${_basis}_g/debug.log" _leih REGEX "Animationsblock von ROOM11B0 geliehen")
    if(NOT _leih)
        message(FATAL_ERROR "r35_entladen[g]: keine Leihe in ROOM10F0 — der Lauf misst die Leihe nicht")
    endif()
    vorher_zeile(g raum 10F0 _v10f0)
    if(NOT _v10f0 MATCHES " rbj=1 " OR _v10f0 MATCHES "rbj_datei")
        message(FATAL_ERROR "r35_entladen[g]: Leihe an der Grenze 10F0 -> 1030 nicht als rbj gezaehlt: '${_v10f0}'")
    endif()
    message(STATUS "r35_entladen[G] OK: geliehener Block (ROOM11B0) an der Grenze 10F0 -> 1030 entladen")
endif()

# --- H: Boot-Weg (Spielstand in ROOM1170, CONTINUE) -------------------------------------------------
if(TEIL STREQUAL "H" OR TEIL STREQUAL "alle")
    if(NOT RE15_KARTE_TOOL OR NOT EXISTS "${RE15_KARTE_TOOL}")
        message(FATAL_ERROR "r35_entladen[h]: RE15_KARTE_TOOL fehlt: '${RE15_KARTE_TOOL}'")
    endif()
    file(MAKE_DIRECTORY "${_basis}_h")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" 1170
                    WORKING_DIRECTORY "${_basis}_h" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0)
        message(FATAL_ERROR "r35_entladen[h]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    entladen_lauf(h 240 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
                        RE15_KILL_AT=60 RE15_BOOT_EXIT_AT=2)
    entladen_pruefen(h 1 2 1)
    file(STRINGS "${_basis}_h/debug.log" _boot REGEX "loading cinematic bank: RBJ/ROOM1170.RBJ .55060 bytes")
    if(NOT _boot)
        message(FATAL_ERROR "r35_entladen[h]: Boot-Weg lud RBJ/ROOM1170.RBJ nicht — der Lauf misst den Boot-Puffer nicht")
    endif()
    vorher_zeile(h raum 1170 _vh)
    if(NOT _vh MATCHES " rbj=1 " OR NOT _vh MATCHES "rbj_datei raum=1170 bytes=55060 ")
        message(FATAL_ERROR "r35_entladen[h]: Boot-Puffer RBJ/ROOM1170.RBJ an der ersten Grenze nicht gezaehlt: '${_vh}'")
    endif()
    message(STATUS "r35_entladen[H] OK: Boot-Puffer RBJ/ROOM1170.RBJ an der ersten Grenze entladen")
endif()

# --- I: RE2-Raumbank-Ergaenzung TUERSE (Dummy-Ton) ---------------------------------------------------
if(TEIL STREQUAL "I" OR TEIL STREQUAL "alle")
    entladen_lauf(i 240 RE15_TITLE_SHOT=t.bmp RE15_DEBUG_JUMP=1170@5 "RE15_FIRE_AOT=5@10#1170"
                        RE15_GOTO_ROOM=1130 "RE15_EXIT_AT=60#1130" "RE15_TUERSE_LOG=${_basis}_i/tuerse.log")
    entladen_pruefen(i 2 1 0)
    file(STRINGS "${_basis}_i/tuerse.log" _tz REGEX "raum=1170 .*satz=0")
    if(NOT _tz)
        message(FATAL_ERROR "r35_entladen[i]: kein 'verschlossen'-Ton in ROOM1170 — der Lauf misst die Raumbank nicht")
    endif()
    vorher_zeile(i raum 1170 _vi)
    string(REGEX MATCH " re2ton=([0-9]+)" _m "${_vi}")
    if(NOT CMAKE_MATCH_1 OR CMAKE_MATCH_1 LESS 1)
        message(FATAL_ERROR "r35_entladen[i]: TUERSE vor der Grenze nicht geladen/gezaehlt: '${_vi}'")
    endif()
    message(STATUS "r35_entladen[I] OK: RE2-Raumbank (TUERSE) an der Grenze 1170 -> 1130 entladen")
endif()


file(REMOVE "${_exe_kopie}")
message(STATUS "r35_entladen OK (${TEIL})")
