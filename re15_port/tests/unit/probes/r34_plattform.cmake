# Runde 34 (Granaten), Spur C (Plattform) — analysis/befunde_runde34_granaten/bau_c.md
#
# ZWEI BETRIEBSARTEN IN EINER DATEI (Dateibesitz Spur C = genau diese Datei + probe_r34_plattform*.c):
#   * Configure: tests/unit/CMakeLists.txt bindet probes/*.cmake per GLOB ein -> unten werden die
#     Sonden und der exe-Pin registriert.
#   * Skript:    cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P <diese Datei> (CMAKE_SCRIPT_MODE_FILE
#     gesetzt) -> der Block direkt hierunter faehrt den exe-Pin integration_r34_plattform_takt und
#     kehrt mit return() zurueck, bevor die Registrierung (add_executable ist im Skriptmodus
#     verboten) erreicht wird.
#
#   unit_r34_plattform   was ohne Fenster pruefbar ist (platform/pc/src/fx_plattform_pc.c, OHNE SDL):
#                        C1 Takt (ESP-Tick hinter dem Spielschritt, Freigabe/Pause), C4 Haken-Bindung +
#                        Ton-Weiche (FUN_80045024 / RE2 0x0113/0x0112 -> ARMS10/11 Satz 10, EDH-Bytes),
#                        C2 TEX.TIM-Effektseiten (Paletten 481/483/492, Seitenpixel == gepinnte
#                        file-route-Blaetter) und Zeichen-Helfer (Sichtbarkeit, wpos, CLUT/TPAGE, defW/H),
#                        C3 Licht-Latch gegen K2 LICHT_LESER, C8 Harness-Parse, C7 RE2-Part-Tinte.
#                        Rueckgabe 0 = gruen, sonst Nummer der ersten verletzten Pruefung.
#   unit_r34_plattform_ton   (s.u.) echtes audio_pc.c.
#   integration_r34_plattform_takt   exe-Pin fuer die LAGE des ESP-Takts in main.c (s.u.).

if(CMAKE_SCRIPT_MODE_FILE)
    # =============================================================================================
    # integration_r34_plattform_takt — exe-Pin C1 (E10), Nachbesserung M2 der Gegenpruefung
    # (analysis/befunde_runde34_granaten/bau_c.gegenpruefung.md §8 M2).
    #
    # WAS GEPINNT WIRD: der ESP-Tick (re15_pc_fx_takt -> re15_esp_fx_tick + re2fx_tick) laeuft in
    # platform/pc/main.c HINTER re15_game_step und VOR dem Zeichnen desselben Bilds. Die Unit-Sonde
    # unit_r34_plattform (T1-T6) prueft nur den Wrapper; wo main.c ihn aufruft, prueft nur dieser
    # Pin. Ein Zurueckschieben vor den Spielschritt (z.B. bei einer Merge-Aufloesung) oder das
    # Entfernen des Aufrufs (dann tickt erst der Rueckfall hinter dem Zeichenblock) wird rot.
    #
    # ORIGINAL (RE1.5 PSX.EXE, Hauptlauf FUN_8001c6e8, selbst disassembliert `dis 0x8001cdd0 60`):
    #   8001ce04  jal 0x8001a50c    Gegner
    #   8001ce0c  jal 0x80031c44    Spieler inkl. Waffen-FSM -> spawnt Muendung id 2 sub 0 (Schussbild)
    #   8001ce2c  jal 0x80019e20    ESP-Tick -> derselbe Platz tickt noch IM Spawnbild
    #   8001ce34  jal 0x8001db28    Item-Modal
    # Erster Tick des Muendungs-Platzes (CORE00.ESP id 2 sub 0 Strom 0 Zeile 0 @0x0FE0:
    # A=8, Byte +0x0e = 0x93, Zeile 1 @0x1008 A=9):
    #   Routine 8 (Tabelle 0x80071d40[8] = 0x800175dc): `lbu v0,14(v1)` @0x800175ec /
    #   `sb v0,108(v1)` @0x800175f4 -> Flags := row[0x0e] = 0x93; `jal 0x800174e4` @0x8001763c = Vorschub.
    #   Bild-Zeitgeber FUN_80019e20 (Timer 0 -> Satzindex +1): frame 0 -> 1.
    # Spawner-Flags vor jedem Tick = 0x03 (re15_esp.c, Spawner @0x800197b4-d0).
    #
    # ERWARTUNG im Spawnbild (FX-Log: eine Zeile je GEZEICHNETEM Platz, "F=" = Spielbild):
    #   erste Zeile "id=2 sub=0 ..." hat F = Spawnbild (aus dem Waffen-Log: F-Zeile vor
    #   "SPAWN id=2 sub=0"), frame=1 und fl=93; KEINE id-2-sub-0-Zeile des Spawnbilds traegt fl=03.
    # Gemessen (bau_c.md §C1, Gegenpruefung §2.1, Lauf build/r34g_c/gp_c1_hg): nachher
    #   "id=2 sub=0 eidx=3 frame=1 ... A=9 ... fl=93 art=0 F=360"; vorher (Takt vor dem Schritt)
    #   "frame=0 ... fl=03" im Bild 360.
    # HARNESS (Mess-Umgebung, kein Spielverhalten): Titel-Autostart, Debug-Sprung ROOM1140 im Bild
    # 250, Browning (Item 3, 15 Schuss), Skript ab Spielbild 300: 1 s warten, 1 s zielen (R1),
    # 0.2 s zielen+feuern -> Schuss im Bild 360; Ende im Bild 400.
    # =============================================================================================
    if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
        message(FATAL_ERROR "r34_plattform_takt: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
    endif()
    if(NOT WORKDIR)
        message(FATAL_ERROR "r34_plattform_takt: WORKDIR fehlt")
    endif()
    include("${CMAKE_CURRENT_LIST_DIR}/../../integration/spiel_lauf.cmake")

    # Pruefung EINER FX-Zeile gegen das Spawnbild. Ergebnis: _ok (1/0) + _warum.
    function(r34_takt_zeile _zeile _spawn _ok _warum)
        set(${_ok} 0 PARENT_SCOPE)
        if(NOT _zeile MATCHES "^id=2 sub=0 ")
            set(${_warum} "keine Zeile 'id=2 sub=0'" PARENT_SCOPE)
            return()
        endif()
        if(NOT _zeile MATCHES " frame=([0-9]+) ")
            set(${_warum} "kein Feld frame= in '${_zeile}'" PARENT_SCOPE)
            return()
        endif()
        set(_frame "${CMAKE_MATCH_1}")
        if(NOT _zeile MATCHES " fl=([0-9a-f][0-9a-f]) ")
            set(${_warum} "kein Feld fl= in '${_zeile}'" PARENT_SCOPE)
            return()
        endif()
        set(_fl "${CMAKE_MATCH_1}")
        if(NOT _zeile MATCHES " F=([0-9]+)$")
            set(${_warum} "kein Feld F= am Zeilenende in '${_zeile}'" PARENT_SCOPE)
            return()
        endif()
        set(_f "${CMAKE_MATCH_1}")
        if(NOT _f EQUAL _spawn)
            string(CONCAT _t "erste Muendungszeile im Bild ${_f}, Spawnbild ist ${_spawn} "
                             "(der Platz wurde im Spawnbild nicht gezeichnet)")
            set(${_warum} "${_t}" PARENT_SCOPE)
            return()
        endif()
        if(NOT _frame EQUAL 1 OR NOT _fl STREQUAL "93")
            string(CONCAT _t "Spawnbild ${_spawn}: frame=${_frame} fl=${_fl}, erwartet frame=1 fl=93 "
                             "(ungetickt = frame=0 fl=03: der ESP-Takt lief NICHT zwischen "
                             "re15_game_step und dem Zeichnen — Original @0x8001ce0c < @0x8001ce2c)")
            set(${_warum} "${_t}" PARENT_SCOPE)
            return()
        endif()
        set(${_ok} 1 PARENT_SCOPE)
        set(${_warum} "frame=1 fl=93 im Spawnbild ${_spawn}" PARENT_SCOPE)
    endfunction()

    # NEGATIV-KONTROLLE des Auswerters (vor jedem exe-Lauf): die Zeile des alten Stands (bau_c.md §C1,
    # "vorher F360") und eine Zeile aus dem Folgebild MUESSEN verworfen werden, die gemessene Zeile
    # des neuen Stands MUSS gelten — sonst waere der Pin blind.
    r34_takt_zeile("id=2 sub=0 eidx=3 frame=0 w(-6440,-2639,-18171) phys=1 xlat=(0,0,0) drift=(0,20,4) slot=51 q=2405 wpos=(0,0,0) A=8 B=0 zuender=0 zaehler=0 fl=03 art=0 F=360" 360 _nk1 _nk1w)
    r34_takt_zeile("id=2 sub=0 eidx=3 frame=1 w(-6440,-2639,-18171) phys=1 xlat=(0,20,4) drift=(0,20,4) slot=51 q=2405 wpos=(0,0,0) A=9 B=0 zuender=0 zaehler=0 fl=93 art=0 F=361" 360 _nk2 _nk2w)
    r34_takt_zeile("id=2 sub=0 eidx=3 frame=1 w(-6440,-2639,-18171) phys=1 xlat=(0,20,4) drift=(0,20,4) slot=51 q=2405 wpos=(0,0,0) A=9 B=0 zuender=0 zaehler=0 fl=93 art=0 F=360" 360 _pk _pkw)
    if(_nk1 OR _nk2 OR NOT _pk)
        message(FATAL_ERROR "r34_plattform_takt: Auswerter defekt (alt=${_nk1} '${_nk1w}', "
                            "Folgebild=${_nk2} '${_nk2w}', neu=${_pk} '${_pkw}')")
    endif()

    string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
    set(WORKDIR "${WORKDIR}/lauf_${_lauf_id}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    set(_r34_env
        RE15_NO_INTRO=1
        RE15_NOAUDIO=1
        RE15_TITLE_SHOT=title.bmp
        RE15_TITLE_SHOT_AF=2
        RE15_WINDOW_SCALE=1
        RE15_DEBUG_JUMP=1140@250
        RE15_GIVE=3:15
        RE15_EQUIP=3
        RE15_INPUT_SCRIPT_BASIS=spiel
        RE15_INPUT_SCRIPT_START=300
        "RE15_INPUT_SCRIPT=W1,M1,MA0.2,M2.5,W4"
        RE15_WAFFEN_LOG=wf.log
        RE15_FX_LOG=fx.log
        "RE15_EXIT_AT=400#1140")
    re15_start_spiel(_rv 180 ${_r34_env} "${RE15_PC_EXE}")
    # EIN Wiederholungsversuch NUR fuer einen Lauf OHNE Ergebnis, nie fuer ein falsches Ergebnis
    # (ausgewertet wird nur ein Lauf mit exit 0):
    #   (a) Startfehler: debug.log < 10 Zeilen (Muster und Begruendung:
    #       tests/integration/test_relatch_pin.cmake);
    #   (b) exit=1 = Signatur eines VON AUSSEN beendeten Prozesses: der Port kennt Code 1 nur aus
    #       der Renderer-Initialisierung (render_pc.c exit(1), vor "[window]"), ein Absturz liefert
    #       0xc0000005. Gemessen 2026-09-30 (bau_c.md NACHBESSERUNG M2): tools/local_build.sh
    #       setzt PATH ohne WindowsPowerShell, faellt deshalb auf "taskkill //F //IM re15_pc.exe"
    #       zurueck und beendet bei JEDEM build einer Parallel-Sitzung alle re15_pc.exe der
    #       Maschine — erster Lauf dieses Pins: exit=1 bei Spielbild 17 nach dem Sprung, Waffen-Log
    #       mitten in der Zeile abgerissen.
    # Ein deterministischer Fehler faellt auch im zweiten Lauf -> rot.
    if(NOT _rv EQUAL 0)
        set(_zeilen "")
        if(EXISTS "${WORKDIR}/debug.log")
            file(STRINGS "${WORKDIR}/debug.log" _zeilen)
        endif()
        list(LENGTH _zeilen _n)
        message(STATUS "r34_plattform_takt: exit=${_rv}, debug.log hat ${_n} Zeilen (letzte 5):")
        set(_i 0)
        math(EXPR _ab "${_n} - 5")
        foreach(_z IN LISTS _zeilen)
            if(_i GREATER_EQUAL _ab)
                message(STATUS "   | ${_z}")
            endif()
            math(EXPR _i "${_i} + 1")
        endforeach()
        if(_n LESS 10 OR "${_rv}" STREQUAL "1")
            message(STATUS "r34_plattform_takt: Lauf ohne Ergebnis (exit=${_rv}) -> EIN "
                           "Wiederholungsversuch")
            file(REMOVE "${WORKDIR}/debug.log" "${WORKDIR}/fx.log" "${WORKDIR}/wf.log")
            re15_start_spiel(_rv 180 ${_r34_env} "${RE15_PC_EXE}")
        endif()
    endif()
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r34_plattform_takt: re15_pc.exe exit=${_rv} (erwartet 0, ${WORKDIR})")
    endif()
    foreach(_d debug.log wf.log fx.log)
        if(NOT EXISTS "${WORKDIR}/${_d}")
            message(FATAL_ERROR "r34_plattform_takt: kein ${_d} in ${WORKDIR}")
        endif()
    endforeach()

    # Spawnbild = die F-Zeile des Waffen-Logs vor dem ersten "SPAWN id=2 sub=0"
    file(STRINGS "${WORKDIR}/wf.log" _wf REGEX "^F[0-9]+ pad=|SPAWN id=2 sub=0 ")
    set(_spawn "")
    set(_letztes "")
    foreach(_z IN LISTS _wf)
        if(_z MATCHES "^F([0-9]+) pad=")
            set(_letztes "${CMAKE_MATCH_1}")
        elseif(_z MATCHES "SPAWN id=2 sub=0 ")
            set(_spawn "${_letztes}")
            break()
        endif()
    endforeach()
    if(_spawn STREQUAL "")
        message(FATAL_ERROR "r34_plattform_takt: kein Schuss — keine Zeile 'SPAWN id=2 sub=0' im "
                            "Waffen-Log (${WORKDIR}/wf.log)")
    endif()

    file(STRINGS "${WORKDIR}/fx.log" _fx REGEX "^id=2 sub=0 ")
    list(LENGTH _fx _nfx)
    if(_nfx EQUAL 0)
        message(FATAL_ERROR "r34_plattform_takt: Schuss im Bild ${_spawn}, aber keine gezeichnete "
                            "Muendung im FX-Log (${WORKDIR}/fx.log)")
    endif()
    list(GET _fx 0 _erste)
    r34_takt_zeile("${_erste}" "${_spawn}" _ok _warum)
    if(NOT _ok)
        message(FATAL_ERROR "r34_plattform_takt: ${_warum}\n  Zeile: ${_erste}\n  (${WORKDIR})")
    endif()
    # Reihenfolge-unabhaengig: im Spawnbild traegt KEIN Muendungsplatz die Spawner-Flags 0x03
    set(_n_spawnbild 0)
    foreach(_z IN LISTS _fx)
        if(_z MATCHES " F=${_spawn}$")
            math(EXPR _n_spawnbild "${_n_spawnbild} + 1")
            if(_z MATCHES " fl=03 ")
                message(FATAL_ERROR "r34_plattform_takt: Muendungsplatz im Spawnbild ${_spawn} "
                                    "ungetickt (fl=03): ${_z}")
            endif()
        endif()
    endforeach()
    message(STATUS "r34_plattform_takt: OK — ${_warum}; ${_n_spawnbild} Muendungszeile(n) im "
                   "Spawnbild, alle getickt (${WORKDIR})")
    return()
endif()

add_executable(probe_r34_plattform
    ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_plattform.c
    ${CMAKE_SOURCE_DIR}/platform/pc/src/fx_plattform_pc.c)
target_link_libraries(probe_r34_plattform PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34_plattform PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/platform/pc/src)
target_compile_definitions(probe_r34_plattform PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
# libm (Runde-30-Befund probes/r30_irons-diary-welt.cmake): unter Linux nicht implizit
if(NOT WIN32)
    target_link_libraries(probe_r34_plattform PRIVATE m)
endif()
add_test(NAME unit_r34_plattform COMMAND probe_r34_plattform)
set_tests_properties(unit_r34_plattform PROPERTIES TIMEOUT 120)

#   unit_r34_plattform_ton   dieselbe Ton-Weiche mit dem ECHTEN audio_pc.c (RE15_AUDIO_CAP_SYNC, kein
#                        SDL-Geraet, Muster test_r31_tuer_ton): ARMS10/ARMS11 Satz 10 (Zusatzbaenke, E9)
#                        und die ESP-Codes 0x010A0601 (ARMS09 Satz 0x0A) / 0x04080001 (CORE Satz 8)
#                        KLINGEN (PCM-Energie), leere Saetze / Bank 0 bleiben stumm.
if(TARGET SDL2::SDL2-static)
    add_executable(probe_r34_plattform_ton
        ${CMAKE_CURRENT_LIST_DIR}/../probe_r34_plattform_ton.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/fx_plattform_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/audio_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/asset_root_pc.c
        ${CMAKE_SOURCE_DIR}/platform/pc/src/skeleton_trig_pc.c)
    target_link_libraries(probe_r34_plattform_ton PRIVATE re15_engine SDL2::SDL2-static)
    target_include_directories(probe_r34_plattform_ton PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/platform/pc/src)
    target_compile_definitions(probe_r34_plattform_ton PRIVATE
        RE15_PLATFORM_PC
        RE15_ASSET_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_CD_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
        RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
    if(NOT WIN32)
        target_link_libraries(probe_r34_plattform_ton PRIVATE m)
    endif()
    add_test(NAME unit_r34_plattform_ton COMMAND probe_r34_plattform_ton)
    set_tests_properties(unit_r34_plattform_ton PROPERTIES TIMEOUT 120
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

#   integration_r34_plattform_takt   echte exe: der ESP-Takt laeuft in main.c HINTER re15_game_step und
#                        VOR dem Zeichnen (@0x8001ce0c Spieler < @0x8001ce2c ESP-Tick) — die Muendung
#                        steht im Schussbild getickt im FX-Log (frame=1 fl=93). Skript = diese Datei.
if(TARGET re15_pc)
    add_test(NAME integration_r34_plattform_takt
             COMMAND "${CMAKE_COMMAND}"
                     -DRE15_PC_EXE=$<TARGET_FILE:re15_pc>
                     -DWORKDIR=${CMAKE_BINARY_DIR}/tests/integration/r34_plattform_takt_wd
                     -P ${CMAKE_CURRENT_LIST_FILE})
    set_tests_properties(integration_r34_plattform_takt PROPERTIES TIMEOUT 420)
endif()
