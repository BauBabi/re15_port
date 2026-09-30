# Runde 34 (Granaten) — Integration W9: integration_r34_granaten (echte exe)
#
# Dossier: analysis/befunde_runde34_granaten/integration.md (W9), Soll: BAUPLAN.md §1.1-§1.6.
# Registrierung: tests/unit/probes/r34_granaten_exe.cmake (Muster probes/r30_granate.cmake).
#
# WAS GEPRUEFT WIRD (je Lauf eine echte re15_pc.exe, ROOM1140 per RE15_DEBUG_JUMP=1140@250):
#   g09/g0a/g0b x re2/re15  — Leon per RE15_PLAYER_POS so, dass die TIEF geworfene Granate an einem
#                zuvor fressenden (RE2: 0x80103B74 EXEC[8]), dann AUFGESTANDENEN Zombie 0x10 explodiert
#                (Aufstellung je KI-Variante gemessen, integration.md W9). Pruefungen:
#     (a) Abzugsbild A = erstes Bild mit Munition -1 (mg 5 -> 4; Entlade-Handler 0x80033B38/58/78 =
#         nur `jal 0x8004eae4`, `sb v0,0(at)` @0x8004eb60): in Zeile A hat KEIN Gegner HP verloren
#         (Port-Bruecke ENT[9].resolve ist weg, @0x80033b40 nur Munition).
#     (b) genau EIN Wurf: "SPAWN granate" genau einmal, art = 2/3/4 (E1/E3), Spawnbild S = A + 24
#         (TIEF: Clipbild 0x18, @0x80033758).
#     (c) Explosion im Bild X = L + 36 (Liegen setzt den Zuender 42 `ori v0,zero,0x2a` @0x80018474,
#         Routine 31 zaehlt je Bild -1 @0x8001867c-84 und zuendet bei 7 @0x8001856c-70), genau EIN
#         Resolver-Aufruf (`jal 0x80012d60` @0x800185b8) mit Art 2/3/4.
#     (d) Inhalt des Explosionsbilds: art 2 = Licht-Latch (@0x8001857c) + Kind 0x03195000 + SE 0x04080001
#         (@0x800185c0-ec); art 3/4 = Aufschlag-Uebergabe re2_art 2/1 (E8, Op 49/48) und KEINER der
#         HE-Inhalte.
#     (e) Treffer: mindestens ein Gegner verliert HP GENAU in Zeile X (die Zeile wird in main.c HINTER
#         dem ESP-Takt geschrieben, der Resolver laeuft im ESP-Tick von X — @0x8001ce2c nach der
#         Gegner-Schleife @0x8001ce04), steht dort in Zustand 2/3 (+0x04 := 2 / 3 bei HP < 0
#         @0x80012f7c-80013020) und traegt die Reaktionszeile: RE2-KI +0x5 = RE2-Zeile 9/11/10
#         (E6, re2z_row_from_atktype), RE1.5-KI +0x5 = DAT_8006f430[Art] = 9/10/11 (@0x8006f432-34).
#     (f) Reaktion im Bild X+1: die Gegner-Schleife des Folgebilds liest +0x4/+0x5 (@0x8001ce04) und
#         startet den Handler — Clip/Unterzustand in Zeile X+1 anders als in Zeile X.
#     (g) KEIN HAENGER (Original: Reaktionszeile 9 NULL -> `jalr v0` mit v0 = 0 @0x80106c00, Absturz-
#         Dossier §2.7): jeder getoetete Gegner erreicht Zustand 7 (Leiche), und die exe laeuft bis
#         RE15_EXIT_AT weiter (Zeile "[flow] EXIT_AT", exit 0). Spieler-HP im Bild X unveraendert.
#     (h) Toene im Waffen-Log: 6x 0x010A..01 (TIEF: 5 Abpraller + Liegen, @0x800183d0-28 / @0x80018350-58),
#         0x04080001 genau einmal nur bei Art 2 (@0x800185e4-ec), RE2-Aufschlag 0x01130001 / 0x01120001 genau
#         einmal nur bei Art 3 / 4 (E9 -> ARMS10/ARMS11 Satz 10).
#   debug   Item-Debug des Statusschirms (Original FUN_8004a0cc @0x8004a138-35c): Inventar auf, ITEM-
#           Raster, SELECT, 9x R1 (Id 0 -> 9), Kreis/Schliessen = ausgeruestet, werfen. Pruefungen:
#           Menge 255 (@0x8004a1f8/204), Waffenbank W09 nach dem Schliessen, KEIN "[debug-menu] OPEN"
#           nach dem Sprung (Integration W5: FUN_8001443c nur aus der Spielschleife @0x8001c988, die
#           beim Statusschirm geparkt ist @0x8001cb40-48), Wurf S = A + 24, X = L + 36, exe bis EXIT_AT.
#   abzug   Zombie 0x10 in <= 1000 vor Leon im Abzugsbild (Reichweite der alten Bruecke, Waffe 9 =
#           1000): kein HP-Verlust in Zeile A. Faengt die Mutation "ENT[9].resolve wieder 1".
#
# HARNESS (Mess-Umgebung, kein Spielverhalten): Titel-Autostart, Debug-Sprung im Bild 250 (der
# Bildzaehler beginnt nach dem Sprung neu), Eingabe-Skript auf der Spielbild-Achse. Die Startraum-
# Runde ROOM1240 laeuft das Skript bis zum Sprung mit player_mode 2 ab (gemessen: kein Wurf, mg
# bleibt 5) — deshalb die Pruefung "genau ein SPAWN".
# Ein exe-Lauf OHNE Ergebnis (exit=1 = von aussen beendet: fremdes `taskkill /IM re15_pc.exe`;
# debug.log < 10 Zeilen = Startfehler) wird EINMAL wiederholt, nie ein falsches Ergebnis.

cmake_minimum_required(VERSION 3.16)
set(_tag "r34_granaten")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

# ---- Aufstellungen (gemessen, integration.md W9) --------------------------------------------------
# RE2-KI: Leon (-1676,-18070) Blick 1076 -> die TIEF-Granate liegt bei (-2199,-20548); Zombie 3 (0x10,
#         HP 80) steht dort nach dem Aufstehen im Bild X in 771. RE1.5-KI: Leon (-4311,-19289) Blick 0 ->
#         Granate bei (-1856,-19902); Zombie 2 (0x10, HP 75) in 513. Beide KI-Varianten laufen nach dem
#         Aufwecken (Abstand < 4000, RE1.5-Fresser-Tor, enemy_ai_re2_zombie.c re2z_exec_feeding) unter-
#         schiedlich — deshalb zwei Aufstellungen. Faellt die Vorbedingung "mindestens ein Treffer",
#         meldet der Test das als AUFSTELLUNG (KI-Wege geaendert), nicht als Granatenfehler.
set(_pos_re2  "-1676,-18070,1076")
set(_pos_re15 "-4311,-19289,0")
set(_skript_wurf "MD0.6,MDA0.2,MD2.5,W5")   # ab Spielbild 1: Heben + TIEF, Abzug, Halten

# Nur fuer Mutationsproben: -DR34_NUR="g9_re2;abzug" faehrt nur diese Laeufe (Standard: alle).
function(r34_laeuft _lauf _aus)
    set(${_aus} 1 PARENT_SCOPE)
    if(DEFINED R34_NUR AND NOT "${R34_NUR}" STREQUAL "")
        list(FIND R34_NUR "${_lauf}" _i)
        if(_i LESS 0)
            set(${_aus} 0 PARENT_SCOPE)
        endif()
    endif()
endfunction()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_wurzel}")

# ---------------------------------------------------------------------------------------------------
# Hilfen
# ---------------------------------------------------------------------------------------------------
function(r34_fehler _lauf _text)
    message(FATAL_ERROR "${_tag} [${_lauf}]: ${_text}")
endfunction()

# Ein exe-Lauf; ARGN = KEY=WERT ... (Umgebung). Wiederholung genau einmal bei exit=1 / Startfehler.
function(r34_exe _lauf _dir _timeout)
    file(MAKE_DIRECTORY "${_dir}")
    set(WORKDIR "${_dir}")
    re15_start_spiel(_rv ${_timeout} ${ARGN} "${RE15_PC_EXE}")
    if(NOT _rv EQUAL 0)
        set(_z "")
        if(EXISTS "${_dir}/debug.log")
            file(STRINGS "${_dir}/debug.log" _z)
        endif()
        list(LENGTH _z _n)
        if(_n LESS 10 OR "${_rv}" STREQUAL "1")
            message(STATUS "${_tag} [${_lauf}]: Lauf ohne Ergebnis (exit=${_rv}, debug.log ${_n} Zeilen) "
                           "-> EIN Wiederholungsversuch")
            file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/gr.log" "${_dir}/wf.log")
            re15_start_spiel(_rv ${_timeout} ${ARGN} "${RE15_PC_EXE}")
        endif()
    endif()
    if(NOT _rv EQUAL 0)
        r34_fehler("${_lauf}" "re15_pc.exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT — Haenger/Absturz?), ${_dir}")
    endif()
    foreach(_d debug.log state.log gr.log)
        if(NOT EXISTS "${_dir}/${_d}")
            r34_fehler("${_lauf}" "kein ${_d} in ${_dir}")
        endif()
    endforeach()
    file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
    if(NOT _ex)
        r34_fehler("${_lauf}" "RE15_EXIT_AT nicht erreicht (keine Zeile '[flow] EXIT_AT') — Haenger?")
    endif()
endfunction()

# Zustandslog NACH dem Sprung: der Bildzaehler faellt beim Raumwechsel auf 1 zurueck; alle Zeilen ab
# dem ersten Rueckgang. Setzt im Aufrufer _Z_<F> (erste Zeile je Bild) und _ZMAX.
macro(r34_zustand _datei)
    # Zeilen des vorigen Laufs verwerfen (die _Z_<F> leben im Aufrufer-Scope weiter).
    if(DEFINED _ZMAX)
        foreach(_f RANGE 0 ${_ZMAX})
            unset(_Z_${_f})
        endforeach()
    endif()
    file(STRINGS "${_datei}" _zl)
    set(_vorher -1)
    set(_nach 0)
    set(_ZMAX 0)
    foreach(_l IN LISTS _zl)
        if(_l MATCHES "^F([0-9]+) ")
            set(_f "${CMAKE_MATCH_1}")
            if(NOT _nach AND _vorher GREATER _f)
                set(_nach 1)
            endif()
            if(_nach AND NOT DEFINED _Z_${_f})
                set(_Z_${_f} "${_l}")
                if(_f GREATER _ZMAX)
                    set(_ZMAX ${_f})
                endif()
            endif()
            set(_vorher ${_f})
        endif()
    endforeach()
    if(NOT _nach)
        message(FATAL_ERROR "${_tag}: ${_datei}: kein Raumwechsel im Zustandslog gefunden")
    endif()
endmacro()

# Gegnerfelder einer Zustandszeile -> <p>_SLOTS und <p>_<slot>_<FELD> (T ST SS1 SS2 SS3 MO AF D HP).
function(r34_gegner _zeile _p)
    string(REGEX MATCHALL "\\[[0-9]+ t=[0-9a-f]+ st=[0-9]+ ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=[0-9]+ af=[0-9]+ stun=-?[0-9]+ d=[0-9]+ @\\(-?[0-9]+,-?[0-9]+,r-?[0-9]+\\)\\] hp=-?[0-9]+"
           _alle "${_zeile}")
    set(_slots "")
    foreach(_g IN LISTS _alle)
        # CMake fuehrt nur CMAKE_MATCH_0..9 -> die HP zuerst mit einem eigenen Ausdruck.
        if(NOT _g MATCHES "\\] hp=(-?[0-9]+)$")
            continue()
        endif()
        set(_hp "${CMAKE_MATCH_1}")
        if(NOT _g MATCHES "^\\[([0-9]+) t=([0-9a-f]+) st=([0-9]+) ss1=([0-9]+) ss2=([0-9]+) ss3=([0-9]+) g=[0-9a-f]+ mo=([0-9]+) af=([0-9]+) stun=-?[0-9]+ d=([0-9]+) @")
            continue()
        endif()
        set(_s "${CMAKE_MATCH_1}")
        list(APPEND _slots "${_s}")
        set(${_p}_${_s}_T   "${CMAKE_MATCH_2}"  PARENT_SCOPE)
        set(${_p}_${_s}_ST  "${CMAKE_MATCH_3}"  PARENT_SCOPE)
        set(${_p}_${_s}_SS1 "${CMAKE_MATCH_4}"  PARENT_SCOPE)
        set(${_p}_${_s}_SS2 "${CMAKE_MATCH_5}"  PARENT_SCOPE)
        set(${_p}_${_s}_SS3 "${CMAKE_MATCH_6}"  PARENT_SCOPE)
        set(${_p}_${_s}_MO  "${CMAKE_MATCH_7}"  PARENT_SCOPE)
        set(${_p}_${_s}_AF  "${CMAKE_MATCH_8}"  PARENT_SCOPE)
        set(${_p}_${_s}_D   "${CMAKE_MATCH_9}"  PARENT_SCOPE)
        set(${_p}_${_s}_HP  "${_hp}"            PARENT_SCOPE)
    endforeach()
    set(${_p}_SLOTS "${_slots}" PARENT_SCOPE)
endfunction()

function(r34_feld _zeile _rx _aus)
    set(${_aus} "" PARENT_SCOPE)
    if(_zeile MATCHES "${_rx}")
        set(${_aus} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    endif()
endfunction()

# Abzugsbild: erstes Nachsprung-Bild, in dem die Munition des ausgeruesteten Platzes (mg=) faellt.
function(r34_abzug _aus)
    set(${_aus} "" PARENT_SCOPE)
    set(_alt "")
    foreach(_f RANGE 1 ${_ZMAX})
        if(NOT DEFINED _Z_${_f})
            continue()
        endif()
        r34_feld("${_Z_${_f}}" " mg=(-?[0-9]+) " _mg)
        if(NOT "${_alt}" STREQUAL "" AND NOT "${_mg}" STREQUAL "" AND _mg LESS _alt)
            set(${_aus} ${_f} PARENT_SCOPE)
            return()
        endif()
        set(_alt "${_mg}")
    endforeach()
endfunction()

# Granatenlog: Spawn (Bild, Art), Liegen- und Explosionsbild (ueber die Platz-Zeilen T -> F), P.
function(r34_granatenlog _datei _p)
    file(STRINGS "${_datei}" _gl)
    set(_spawn_n 0)
    set(_res_n 0)
    set(_tl "")
    set(_tx "")
    foreach(_l IN LISTS _gl)
        if(_l MATCHES "^F=([0-9]+) SPAWN granate art=([0-9]+) ")
            math(EXPR _spawn_n "${_spawn_n} + 1")
            set(${_p}_S "${CMAKE_MATCH_1}" PARENT_SCOPE)
            set(${_p}_ART "${CMAKE_MATCH_2}" PARENT_SCOPE)
        elseif(_l MATCHES "^T=([0-9]+) F=([0-9]+) slot=")
            if(NOT DEFINED _tf_${CMAKE_MATCH_1})
                set(_tf_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}")
            endif()
        elseif(_l MATCHES "^T=([0-9]+) EV se code=[0-9a-f]+ pos=.* liegen$")
            if("${_tl}" STREQUAL "")
                set(_tl "${CMAKE_MATCH_1}")
            endif()
        elseif(_l MATCHES "^T=([0-9]+) EV resolver art=([0-9]+) P=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)")
            math(EXPR _res_n "${_res_n} + 1")
            set(_tx "${CMAKE_MATCH_1}")
            set(${_p}_RART "${CMAKE_MATCH_2}" PARENT_SCOPE)
            set(${_p}_PX "${CMAKE_MATCH_3}" PARENT_SCOPE)
            set(${_p}_PY "${CMAKE_MATCH_4}" PARENT_SCOPE)
            set(${_p}_PZ "${CMAKE_MATCH_5}" PARENT_SCOPE)
        endif()
    endforeach()
    set(${_p}_SPAWN_N ${_spawn_n} PARENT_SCOPE)
    set(${_p}_RES_N ${_res_n} PARENT_SCOPE)
    set(${_p}_L "" PARENT_SCOPE)
    set(${_p}_X "" PARENT_SCOPE)
    if(NOT "${_tl}" STREQUAL "" AND DEFINED _tf_${_tl})
        set(${_p}_L "${_tf_${_tl}}" PARENT_SCOPE)
    endif()
    if(NOT "${_tx}" STREQUAL "" AND DEFINED _tf_${_tx})
        set(${_p}_X "${_tf_${_tx}}" PARENT_SCOPE)
    endif()
    # Ereignisse des Explosions-Ticks
    set(_ev "")
    if(NOT "${_tx}" STREQUAL "")
        foreach(_l IN LISTS _gl)
            if(_l MATCHES "^T=${_tx} EV (.*)$")
                list(APPEND _ev "${CMAKE_MATCH_1}")
            endif()
        endforeach()
    endif()
    set(${_p}_EV "${_ev}" PARENT_SCOPE)
endfunction()

# (a) kein HP-Verlust im Abzugsbild; Rueckgabe-Text leer = gut.
function(r34_pruef_abzug _za _zv _aus)
    set(${_aus} "" PARENT_SCOPE)
    r34_gegner("${_za}" _A)
    r34_gegner("${_zv}" _V)
    foreach(_s IN LISTS _A_SLOTS)
        if(DEFINED _V_${_s}_HP AND _A_${_s}_HP LESS _V_${_s}_HP)
            set(${_aus} "Gegner ${_s} (t=${_A_${_s}_T}) verliert im Abzugsbild HP ${_V_${_s}_HP} -> ${_A_${_s}_HP} (Sofort-Bruecke ENT[9].resolve? @0x80033b40 = nur Munition)" PARENT_SCOPE)
            return()
        endif()
    endforeach()
endfunction()

# Slots, deren HP von Zeile _zv zu Zeile _zx faellt.
function(r34_hp_verlust _zv _zx _aus)
    r34_gegner("${_zv}" _V)
    r34_gegner("${_zx}" _X)
    set(_l "")
    foreach(_s IN LISTS _X_SLOTS)
        if(DEFINED _V_${_s}_HP AND _X_${_s}_HP LESS _V_${_s}_HP)
            list(APPEND _l ${_s})
        endif()
    endforeach()
    set(${_aus} "${_l}" PARENT_SCOPE)
endfunction()

# (e)(f) Treffer im Bild X, Reaktion X+1; Rueckgabe: Liste getroffener Slots, Fehlertext.
function(r34_pruef_treffer _zv _zx _zn _zeile_soll _aus_slots _aus_fehler)
    set(${_aus_fehler} "" PARENT_SCOPE)
    set(${_aus_slots} "" PARENT_SCOPE)
    r34_gegner("${_zv}" _V)
    r34_gegner("${_zx}" _X)
    r34_gegner("${_zn}" _N)
    set(_treffer "")
    foreach(_s IN LISTS _X_SLOTS)
        if(DEFINED _V_${_s}_HP AND _X_${_s}_HP LESS _V_${_s}_HP)
            list(APPEND _treffer ${_s})
            if(NOT (_X_${_s}_ST EQUAL 2 OR _X_${_s}_ST EQUAL 3))
                set(${_aus_fehler} "Gegner ${_s}: HP gefallen, aber Zustand ${_X_${_s}_ST} statt 2/3 (+0x04 := 2/3 @0x80012f7c-80013020)" PARENT_SCOPE)
                return()
            endif()
            if(NOT _X_${_s}_SS1 EQUAL _zeile_soll)
                set(${_aus_fehler} "Gegner ${_s}: Reaktionszeile +0x5 = ${_X_${_s}_SS1}, erwartet ${_zeile_soll}" PARENT_SCOPE)
                return()
            endif()
            if(NOT DEFINED _N_${_s}_MO)
                set(${_aus_fehler} "Gegner ${_s}: fehlt in Zeile X+1" PARENT_SCOPE)
                return()
            endif()
            if(_N_${_s}_MO EQUAL _X_${_s}_MO AND _N_${_s}_SS2 EQUAL _X_${_s}_SS2 AND _N_${_s}_SS3 EQUAL _X_${_s}_SS3)
                set(${_aus_fehler} "Gegner ${_s}: keine Reaktion im Bild X+1 (Clip ${_X_${_s}_MO}, +0x6 ${_X_${_s}_SS2}, +0x7 ${_X_${_s}_SS3} unveraendert)" PARENT_SCOPE)
                return()
            endif()
        endif()
    endforeach()
    set(${_aus_slots} "${_treffer}" PARENT_SCOPE)
endfunction()

# (h) TOENE des Wurfs aus dem Waffen-Log (RE15_WAFFEN_LOG, Zeilen "SE  esp code=..." bzw. "SE  re2fx code=...";
#     Weiche fx_plattform_pc.c, Spur C4): Abprall/Liegen `jal 0x80045024` mit 0x010A0001 | (n<<8) @0x800183d0-28 /
#     @0x80018350-58 (TIEF: Zaehler 5 -> 5 Abpraller + Liegen = 6 Toene 0x010a....), Explosion 0x04080001 @0x800185e4-ec
#     nur Art 2, RE2-Aufschlag Saeure 0x01130001 (RE2 @0x80021678-7c) / Brand 0x01120001 (RE2 @0x80020fd4/0x80021028)
#     -> ARMS10/ARMS11 Satz 10 (E9) nur Art 3/4. Rueckgabe: Fehlertext oder leer.
function(r34_pruef_toene _datei _art _aus)
    set(${_aus} "" PARENT_SCOPE)
    file(STRINGS "${_datei}" _w REGEX "SE  (esp|re2fx) code=")
    set(_n010a 0)
    set(_n0408 0)
    set(_n0113 0)
    set(_n0112 0)
    foreach(_l IN LISTS _w)
        if(_l MATCHES "SE  esp code=0x010a0[0-4]01 ")
            math(EXPR _n010a "${_n010a} + 1")
        elseif(_l MATCHES "SE  esp code=0x04080001 ")
            math(EXPR _n0408 "${_n0408} + 1")
        elseif(_l MATCHES "SE  re2fx code=0x01130001 ")
            math(EXPR _n0113 "${_n0113} + 1")
        elseif(_l MATCHES "SE  re2fx code=0x01120001 ")
            math(EXPR _n0112 "${_n0112} + 1")
        endif()
    endforeach()
    set(_soll0408 0)
    set(_soll0113 0)
    set(_soll0112 0)
    if(_art EQUAL 2)
        set(_soll0408 1)
    elseif(_art EQUAL 3)
        set(_soll0113 1)
    else()
        set(_soll0112 1)
    endif()
    if(NOT _n010a EQUAL 6 OR NOT _n0408 EQUAL _soll0408 OR NOT _n0113 EQUAL _soll0113 OR NOT _n0112 EQUAL _soll0112)
        set(${_aus} "Toene: ${_n010a}x 0x010a....(Soll 6), ${_n0408}x 0x04080001 (Soll ${_soll0408}), ${_n0113}x 0x01130001 (Soll ${_soll0113}), ${_n0112}x 0x01120001 (Soll ${_soll0112})" PARENT_SCOPE)
    endif()
endfunction()

# ---------------------------------------------------------------------------------------------------
# NEGATIV-KONTROLLE der Auswerter (vor jedem exe-Lauf): erfundene Zeilen, die fallen MUESSEN.
# ---------------------------------------------------------------------------------------------------
set(_nk_v "F18 pad=0840 PL(-2651,-19600,rot=4095,hp=100) pst=1 mg=5 [2 t=10 st=1 ss1=8 ss2=4 ss3=0 g=00 mo=21 af=9 stun=6 d=849 @(-1800,-19600,r512)] hp=50")
set(_nk_a "F19 pad=8840 PL(-2651,-19600,rot=4095,hp=100) pst=1 mg=4 [2 t=10 st=2 ss1=17 ss2=0 ss3=0 g=00 mo=21 af=10 stun=6 d=849 @(-1800,-19600,r512)] hp=40")
r34_pruef_abzug("${_nk_a}" "${_nk_v}" _nk1)
r34_pruef_abzug("${_nk_v}" "${_nk_v}" _nk2)
set(_nk_x "F119 PL(-4311,-19289,rot=79,hp=100) [2 t=10 st=3 ss1=9 ss2=1 ss3=0 g=00 mo=1 af=80 stun=18 d=2048 @(-2303,-19650,r1555)] hp=-125")
set(_nk_xv "F118 PL(-4311,-19289,rot=79,hp=100) [2 t=10 st=1 ss1=19 ss2=1 ss3=0 g=00 mo=1 af=79 stun=18 d=2048 @(-2285,-19645,r1566)] hp=75")
set(_nk_n_ok "F120 PL(-4311,-19289,rot=79,hp=100) [2 t=10 st=3 ss1=9 ss2=1 ss3=1 g=00 mo=11 af=1 stun=18 d=2039 @(-2303,-19650,r1555)] hp=-125")
set(_nk_n_ohne "F120 PL(-4311,-19289,rot=79,hp=100) [2 t=10 st=3 ss1=9 ss2=1 ss3=0 g=00 mo=1 af=81 stun=18 d=2039 @(-2303,-19650,r1555)] hp=-125")
r34_pruef_treffer("${_nk_xv}" "${_nk_x}" "${_nk_n_ok}" 9 _nk_s3 _nk3)
r34_pruef_treffer("${_nk_xv}" "${_nk_x}" "${_nk_n_ohne}" 9 _nk_s4 _nk4)
r34_pruef_treffer("${_nk_xv}" "${_nk_x}" "${_nk_n_ok}" 11 _nk_s5 _nk5)
r34_pruef_treffer("${_nk_xv}" "${_nk_xv}" "${_nk_xv}" 9 _nk_s6 _nk6)
if("${_nk1}" STREQUAL "" OR NOT "${_nk2}" STREQUAL "" OR NOT "${_nk3}" STREQUAL "" OR NOT "${_nk_s3}" STREQUAL "2"
   OR "${_nk4}" STREQUAL "" OR "${_nk5}" STREQUAL "" OR NOT "${_nk_s6}" STREQUAL "")
    message(FATAL_ERROR "${_tag}: Auswerter defekt (Abzug-Schaden '${_nk1}', Abzug-ok '${_nk2}', "
                        "Treffer-ok '${_nk3}' Slots '${_nk_s3}', ohne Reaktion '${_nk4}', falsche Zeile '${_nk5}', "
                        "ohne Treffer Slots '${_nk_s6}')")
endif()
message(STATUS "${_tag}: Negativ-Kontrolle der Auswerter ok (Abzug-Schaden, fehlende Reaktion, falsche Zeile fallen)")

# ---------------------------------------------------------------------------------------------------
# Laeufe g09/g0a/g0b x re2/re15
# ---------------------------------------------------------------------------------------------------
foreach(_ki re2 re15)
    foreach(_id 9 10 11)
        math(EXPR _art "${_id} - 7")                       # 9/10/11 -> Art 2/3/4 (E3, explizit)
        if(_ki STREQUAL "re2")
            set(_pos "${_pos_re2}")
            # E6: RE2-Zeile = re2z_row_from_atktype[Art] = 9 (HE) / 11 (Saeure) / 10 (Brand)
            if(_art EQUAL 2)
                set(_zeile 9)
            elseif(_art EQUAL 3)
                set(_zeile 11)
            else()
                set(_zeile 10)
            endif()
        else()
            set(_pos "${_pos_re15}")
            # DAT_8006f430[Art]: 9/10/11 (@0x8006f432-34)
            math(EXPR _zeile "${_art} + 7")
        endif()
        set(_lauf "g${_id}_${_ki}")
        r34_laeuft("${_lauf}" _an)
        if(NOT _an)
            continue()
        endif()
        set(_dir "${_wurzel}/${_lauf}")
        r34_exe("${_lauf}" "${_dir}" 240
            RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
            RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1140@250 "RE15_PLAYER_POS=${_pos}"
            RE15_AI_FLAVOR=${_ki} RE15_GIVE=${_id}:5 RE15_EQUIP=${_id}
            RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_skript_wurf}"
            RE15_STATE_LOG=state.log RE15_GRANATE_LOG=gr.log RE15_WAFFEN_LOG=wf.log
            "RE15_EXIT_AT=220#1140")
        # Zustandslog
        r34_zustand("${_dir}/state.log")
        r34_abzug(_A)
        if("${_A}" STREQUAL "")
            r34_fehler("${_lauf}" "kein Abzug gefunden (Munition faellt nie)")
        endif()
        r34_granatenlog("${_dir}/gr.log" _G)
        if(NOT _G_SPAWN_N EQUAL 1)
            r34_fehler("${_lauf}" "${_G_SPAWN_N} Granaten-Spawns, erwartet genau 1")
        endif()
        if(NOT _G_ART EQUAL _art)
            r34_fehler("${_lauf}" "Spawn-Art ${_G_ART}, erwartet ${_art} (E1/E3)")
        endif()
        math(EXPR _s_soll "${_A} + 24")
        if(NOT _G_S EQUAL _s_soll)
            r34_fehler("${_lauf}" "Spawnbild ${_G_S}, erwartet A + 24 = ${_s_soll} (TIEF Clipbild 0x18 @0x80033758)")
        endif()
        if("${_G_L}" STREQUAL "" OR "${_G_X}" STREQUAL "")
            r34_fehler("${_lauf}" "kein Liegen (${_G_L}) oder keine Explosion (${_G_X}) im Granatenlog")
        endif()
        math(EXPR _x_soll "${_G_L} + 36")
        if(NOT _G_X EQUAL _x_soll)
            r34_fehler("${_lauf}" "Explosion im Bild ${_G_X}, erwartet L + 36 = ${_x_soll} (Zuender 42 @0x80018474 -> 7 @0x8001856c)")
        endif()
        if(NOT _G_RES_N EQUAL 1 OR NOT _G_RART EQUAL _art)
            r34_fehler("${_lauf}" "${_G_RES_N} Resolver-Aufrufe mit Art ${_G_RART}, erwartet genau 1 mit Art ${_art} (@0x800185b8)")
        endif()
        # (d) Inhalt des Explosionsbilds
        set(_he 0)
        set(_aufs "")
        foreach(_e IN LISTS _G_EV)
            if(_e STREQUAL "latch" OR _e MATCHES "^kind code=03195000 " OR _e MATCHES "^se code=04080001 ")
                math(EXPR _he "${_he} + 1")
            elseif(_e MATCHES "^aufschlag re2_art=([0-9]+) ")
                set(_aufs "${CMAKE_MATCH_1}")
            endif()
        endforeach()
        if(_art EQUAL 2)
            if(NOT _he EQUAL 3 OR NOT "${_aufs}" STREQUAL "")
                r34_fehler("${_lauf}" "HE-Explosionsbild: ${_he}/3 HE-Inhalte (Latch/0x03195000/0x04080001), Aufschlag '${_aufs}' (erwartet keiner)")
            endif()
        else()
            math(EXPR _re2_soll "5 - ${_art}")             # Art 3 -> 2 (Saeure, Op 49), Art 4 -> 1 (Brand, Op 48)
            if(NOT _he EQUAL 0 OR NOT "${_aufs}" STREQUAL "${_re2_soll}")
                r34_fehler("${_lauf}" "Aufschlagbild: ${_he} HE-Inhalte (erwartet 0), re2_art '${_aufs}' (erwartet ${_re2_soll}, E8)")
            endif()
        endif()
        # (h) Toene des Wurfs
        r34_pruef_toene("${_dir}/wf.log" ${_art} _ft)
        if(NOT "${_ft}" STREQUAL "")
            r34_fehler("${_lauf}" "${_ft}")
        endif()
        # (a) Abzugsbild ohne Schaden
        math(EXPR _av "${_A} - 1")
        r34_pruef_abzug("${_Z_${_A}}" "${_Z_${_av}}" _fa)
        if(NOT "${_fa}" STREQUAL "")
            r34_fehler("${_lauf}" "${_fa}")
        endif()
        # (e)(f) Treffer im Bild X, Reaktion X+1
        math(EXPR _xv "${_G_X} - 1")
        math(EXPR _xn "${_G_X} + 1")
        math(EXPR _xvv "${_G_X} - 2")
        r34_hp_verlust("${_Z_${_xvv}}" "${_Z_${_xv}}" _vorher_slots)
        if(NOT "${_vorher_slots}" STREQUAL "")
            r34_fehler("${_lauf}" "HP-Verlust schon im Bild X-1 = ${_xv} (Slots ${_vorher_slots}) — Schaden nicht im Explosionsbild")
        endif()
        r34_pruef_treffer("${_Z_${_xv}}" "${_Z_${_G_X}}" "${_Z_${_xn}}" ${_zeile} _hit _fh)
        if(NOT "${_fh}" STREQUAL "")
            r34_fehler("${_lauf}" "Explosionsbild ${_G_X}: ${_fh}")
        endif()
        if("${_hit}" STREQUAL "")
            r34_fehler("${_lauf}" "AUFSTELLUNG: kein Gegner verliert im Explosionsbild ${_G_X} HP (P=(${_G_PX},${_G_PY},${_G_PZ})) — die KI-Wege haben sich geaendert, Aufstellung neu messen (integration.md W9)")
        endif()
        r34_feld("${_Z_${_xv}}" "PL\\(-?[0-9]+,-?[0-9]+,rot=-?[0-9]+,hp=(-?[0-9]+)\\)" _plv)
        r34_feld("${_Z_${_G_X}}" "PL\\(-?[0-9]+,-?[0-9]+,rot=-?[0-9]+,hp=(-?[0-9]+)\\)" _plx)
        # Der Spieler liegt ausserhalb von R = 450 + 500 (BAUPLAN 1.5): ein Treffer kostete 1000
        # (DAT_8006f418[Art], @0x8006f41c-20). Zombie-Bisse im selben Bild (10..30) bleiben erlaubt.
        math(EXPR _pld "${_plx} - ${_plv}")
        if(_pld LESS -500)
            r34_fehler("${_lauf}" "Spieler-HP im Explosionsbild ${_plv} -> ${_plx} — die Explosion traf Leon (P liegt > 950 entfernt, BAUPLAN 1.5)")
        endif()
        # (g) kein Haenger: jeder Getoetete erreicht Zustand 7 bis zum Laufende
        r34_gegner("${_Z_${_G_X}}" _GX)
        set(_tote "")
        foreach(_s IN LISTS _hit)
            if(_GX_${_s}_HP LESS 0)
                list(APPEND _tote ${_s})
            endif()
        endforeach()
        foreach(_s IN LISTS _tote)
            set(_leiche "")
            foreach(_f RANGE ${_xn} ${_ZMAX})
                if(DEFINED _Z_${_f})
                    r34_gegner("${_Z_${_f}}" _E)
                    if(DEFINED _E_${_s}_ST AND _E_${_s}_ST EQUAL 7)
                        set(_leiche ${_f})
                        break()
                    endif()
                endif()
            endforeach()
            if("${_leiche}" STREQUAL "")
                r34_fehler("${_lauf}" "Gegner ${_s} (HP ${_GX_${_s}_HP}) erreicht bis Bild ${_ZMAX} keinen Zustand 7 (Leiche) — HAENGER in der Reaktion")
            endif()
            message(STATUS "${_tag} [${_lauf}]: Gegner ${_s} t=${_GX_${_s}_T} HP ${_GX_${_s}_HP}, Zeile ${_GX_${_s}_SS1}, Leiche ab Bild ${_leiche}")
        endforeach()
        message(STATUS "${_tag} [${_lauf}]: ok — A ${_A}, S ${_G_S}, L ${_G_L}, X ${_G_X}, P (${_G_PX},${_G_PY},${_G_PZ}), getroffen: ${_hit}, getoetet: ${_tote}")
    endforeach()
endforeach()

# ---------------------------------------------------------------------------------------------------
# Lauf "debug": Item-Debug des Statusschirms -> Granate 0x09 -> Wurf
# ---------------------------------------------------------------------------------------------------
set(_sk "S0.1,W2,A0.1,W1,E0.1,W0.3")
foreach(_i RANGE 1 9)
    set(_sk "${_sk},M0.1,W0.2")                         # R1 = Id + 1 (@0x8004a238-260)
endforeach()
set(_sk "${_sk},X0.1,W1,S0.1,W3,${_skript_wurf}")
set(_lauf "debug")
r34_laeuft("${_lauf}" _an_debug)
set(_dir "${_wurzel}/${_lauf}")
if(_an_debug)
# Skriptbeginn 260 > Sprungbild 250: die Startraum-Runde erreicht das Skript nie (kein SELECT dort);
# Leon am Sprungpunkt der Tuer (-7600,-17600), alle Fresser > 4000 entfernt -> keiner wacht auf.
r34_exe("${_lauf}" "${_dir}" 300
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1140@250 RE15_AI_FLAVOR=re2
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=260 "RE15_INPUT_SCRIPT=${_sk}"
    RE15_STATE_LOG=state.log RE15_GRANATE_LOG=gr.log RE15_WAFFEN_LOG=wf.log
    "RE15_FRAMEDUMP=692-693/1:f_" "RE15_EXIT_AT=760#1140")
file(STRINGS "${_dir}/debug.log" _dl)
set(_nach 0)
set(_menue 0)
set(_w09 0)
foreach(_l IN LISTS _dl)
    if(_l MATCHES "\\[debug-menu\\] JUMP -> ")
        set(_nach 1)
    elseif(_nach AND _l MATCHES "\\[debug-menu\\] OPEN")
        set(_menue 1)
    elseif(_nach AND _l MATCHES "\\[equip\\] W-bank -> W09")
        set(_w09 1)
    endif()
endforeach()
if(_menue)
    r34_fehler("${_lauf}" "SELECT im ITEM-Raster oeffnete das UTILITY/DEBUG-MENU (W5: FUN_8001443c nur aus der Spielschleife @0x8001c988)")
endif()
if(NOT _w09)
    r34_fehler("${_lauf}" "nach dem Schliessen keine Waffenbank W09 (Item-Debug hat die Granate nicht ausgeruestet)")
endif()
r34_zustand("${_dir}/state.log")
set(_m255 0)
foreach(_f RANGE 1 ${_ZMAX})
    if(DEFINED _Z_${_f} AND _Z_${_f} MATCHES " mg=255 ")
        set(_m255 1)
        break()
    endif()
endforeach()
if(NOT _m255)
    r34_fehler("${_lauf}" "Menge 255 nie gesehen (@0x8004a1f8/204)")
endif()
r34_abzug(_A)
r34_granatenlog("${_dir}/gr.log" _G)
if("${_A}" STREQUAL "" OR NOT _G_SPAWN_N EQUAL 1 OR NOT _G_ART EQUAL 2)
    r34_fehler("${_lauf}" "Abzug '${_A}', ${_G_SPAWN_N} Spawns, Art '${_G_ART}' (erwartet Abzug, 1 Spawn, Art 2)")
endif()
r34_pruef_toene("${_dir}/wf.log" 2 _ft)
if(NOT "${_ft}" STREQUAL "")
    r34_fehler("${_lauf}" "${_ft}")
endif()
math(EXPR _s_soll "${_A} + 24")
math(EXPR _x_soll "${_G_L} + 36")
if(NOT _G_S EQUAL _s_soll OR NOT _G_X EQUAL _x_soll OR NOT _G_RES_N EQUAL 1)
    r34_fehler("${_lauf}" "S ${_G_S} (Soll ${_s_soll}), X ${_G_X} (Soll ${_x_soll}), ${_G_RES_N} Resolver-Aufrufe")
endif()
# (W8) Helligkeit des HE-Feuerballs im Explosionsbild: Kind 0x03195000 (Routine 10: Flags 0x13 = ABE,
# TPAGE-ABR 0 = 0.5*B + 0.5*F), Palette 483 Index 1 = (248,248,248) (DATA/TEX.TIM). Das Original moduliert
# mit der Primitivfarbe 0x80 = x 1.0 (FUN_800537e4 @0x800537ec/@0x800538fc-908; FUN_800534c4 schreibt nur
# das Code-Byte @0x8005369c) -> wirksamer Beitrag 2*out - B bis ~248; mit Farbe 128 als SDL-Faktor ~124.
# Bildausschnitt: der Feuerball liegt in diesem Lauf (Tuer-Sprungpunkt, Kamera 0, Fenster x1) bei
# x 95..155 / y 85..140, links neben Leon (dessen Licht-Latch-Farbe bleibt ausserhalb).
if(RE15_PPM_TOOL AND EXISTS "${RE15_PPM_TOOL}")
    math(EXPR _xm1 "${_G_X} - 1")
    # Framedump-Namen wie main.c: "<praefix>%06ld.ppm"
    foreach(_v _xm1 _G_X)
        set(_z "000000${${_v}}")
        string(LENGTH "${_z}" _zl)
        math(EXPR _za "${_zl} - 6")
        string(SUBSTRING "${_z}" ${_za} 6 _pad${_v})
    endforeach()
    set(_pa "${_dir}/f_${_pad_xm1}.ppm")
    set(_pb "${_dir}/f_${_pad_G_X}.ppm")
    if(NOT EXISTS "${_pa}" OR NOT EXISTS "${_pb}")
        r34_fehler("${_lauf}" "Framedumps ${_pa} / ${_pb} fehlen (RE15_FRAMEDUMP 692-693, X ${_G_X})")
    endif()
    execute_process(COMMAND "${RE15_PPM_TOOL}" "${_pa}" "${_pb}" 95 85 155 140 0
                    OUTPUT_VARIABLE _hell RESULT_VARIABLE _hrv OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT _hrv EQUAL 0 OR NOT _hell MATCHES "^max=(-?[0-9]+),(-?[0-9]+),(-?[0-9]+) n=([0-9]+)$")
        r34_fehler("${_lauf}" "Helligkeitsmessung unbrauchbar (${_hrv}: '${_hell}')")
    endif()
    if(CMAKE_MATCH_1 LESS 240 OR CMAKE_MATCH_2 LESS 240 OR CMAKE_MATCH_3 LESS 240 OR CMAKE_MATCH_4 LESS 200)
        r34_fehler("${_lauf}" "Feuerball im Explosionsbild zu dunkel: Beitrag ${CMAKE_MATCH_1}/${CMAKE_MATCH_2}/${CMAKE_MATCH_3} (Soll ~248 = Texel x 0x80/0x80), ${CMAKE_MATCH_4} Pixel — Primitivfarbe 128 als SDL-Faktor? (W8)")
    endif()
    message(STATUS "${_tag} [${_lauf}]: Feuerball-Beitrag ${_hell} (Soll ~248, W8)")
endif()
message(STATUS "${_tag} [${_lauf}]: ok — Item-Debug: Menge 255, W09 ausgeruestet, kein Debug-Menue; A ${_A}, S ${_G_S}, L ${_G_L}, X ${_G_X}")
endif()

# ---------------------------------------------------------------------------------------------------
# Lauf "abzug": Zombie in <= 1000 vor Leon im Abzugsbild — die alte Sofort-Bruecke haette ihn getroffen
# ---------------------------------------------------------------------------------------------------
set(_lauf "abzug")
r34_laeuft("${_lauf}" _an_abzug)
set(_dir "${_wurzel}/${_lauf}")
if(_an_abzug)
r34_exe("${_lauf}" "${_dir}" 200
    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1140@250 "RE15_PLAYER_POS=-2600,-19600,0"
    RE15_AI_FLAVOR=re2 RE15_GIVE=9:5 RE15_EQUIP=9
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=${_skript_wurf}"
    RE15_STATE_LOG=state.log RE15_GRANATE_LOG=gr.log RE15_WAFFEN_LOG=wf.log
    "RE15_EXIT_AT=60#1140")
r34_zustand("${_dir}/state.log")
r34_abzug(_A)
if("${_A}" STREQUAL "")
    r34_fehler("${_lauf}" "kein Abzug gefunden")
endif()
math(EXPR _av "${_A} - 1")
r34_gegner("${_Z_${_A}}" _AB)
set(_nah "")
foreach(_s IN LISTS _AB_SLOTS)
    if(_AB_${_s}_D LESS_EQUAL 1000 AND NOT _AB_${_s}_D EQUAL 0)
        set(_nah ${_s})
    endif()
endforeach()
if("${_nah}" STREQUAL "")
    r34_fehler("${_lauf}" "AUFSTELLUNG: kein Gegner in <= 1000 im Abzugsbild ${_A} — die Pruefung haette keine Zaehne")
endif()
r34_pruef_abzug("${_Z_${_A}}" "${_Z_${_av}}" _fa)
if(NOT "${_fa}" STREQUAL "")
    r34_fehler("${_lauf}" "${_fa}")
endif()
message(STATUS "${_tag} [${_lauf}]: ok — Abzugsbild ${_A}, Gegner ${_nah} in ${_AB_${_nah}_D} vor Leon, kein HP-Verlust")
endif()
message(STATUS "${_tag}: ALLE LAEUFE GRUEN (${_wurzel})")
