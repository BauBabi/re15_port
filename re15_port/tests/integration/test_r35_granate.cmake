# Runde 35 Spur A — integration_r35_granate (echte exe). Dossier: analysis/befunde_runde35/A_granate.md
# Registrierung: tests/unit/probes/r35_granate.cmake. Harness-Muster: tests/integration/test_r34_granaten.cmake.
#
# LAEUFE (je eine echte re15_pc.exe, Titel-Autostart, RE15_DEBUG_JUMP, Eingabe-Skript auf der Spielbild-Achse):
#   wand    ROOM1140, Leon (-4311,-19289) Blick 0, MITTE-Wurf (Skript M0.6,MA0.2,M2.5): die Granate erreicht die
#           solide SCA-Zelle [x8350 z-20750 w4100 d1150] (gemessen W1/N1b). Pruefungen:
#           (a) gr.log traegt GENAU EINE Zeile "EV wand wpos=(x,y,z) -> rueckzug (x',y',z')": x >= 8350 (in der
#               Zelle), x' < 8350 (RE2-Rueckzug @0x8001ef90-0cc), (b) der Resolver-Aufruf (Explosion) hat denselben
#               Tick T wie die Wandzeile (RE2 @0x8001f0e0-104: im selben Bild), (c) kein Granatenbild mit x > 8350
#               nach der Wandzeile (kein Durchflug), (d) Explosions-SE "SE  re2fx code=0x01110001 -> ARMS0F Satz 10"
#               genau einmal, KEIN "SE  esp code=0x04080001", (e) exe laeuft bis RE15_EXIT_AT.
#   zombie  ROOM1140, RE2-KI, Leon (-1676,-18070) Blick 1076, TIEF-Wurf (Aufstellung integration_r34_granaten g9_re2):
#           (a) Explosion im Bild L + 36 (Zeitzuender RE1.5 @0x80018474/@0x8001856c unveraendert — kein Flugkontakt),
#           (b) resolver-Zeile "r=2000 eingriffe=N" mit N >= 2 (vorher 1: Reichweite 900), (c) jeder getroffene
#               Zombie 0x10 traegt im Explosionsbild ss1=9 und im Folgebild NICHT Clip 1 (0x80108530 Sturz, vorher),
#               sondern Clip 2 (re2z_death_rip Phase 0, clip2[] @0x80108C24-30) und wird weggeschleudert (Lage
#               aendert sich um > 600 in 10 Bildern), erreicht Zustand 7 (Leiche) bis Bild X+80,
#           (d) RE2-SE genau einmal, (e) exe bis EXIT_AT.
#   gator   ROOM2090 (Gator-Boss, Modul enemy_ai_boss_gator.c), RE15_FORCE_EXPLOSION=2@<bild>:<slot> am Alligator:
#           HP faellt um 1000 (GB_HP 3000 -> 2000, Art 2 @0x8006f41c, 0x23 ohne RE2-Modell), Zustand 2 im
#           Explosionsbild, exe laeuft bis EXIT_AT (kein Haenger), RE15_FORCE_EXPLOSION-Zeile "Treffer=1".
#   birkin  ROOM5090 (G5 EM36, Modul enemy_ai_boss_g5.c), ARMIERT ueber RE15_DEBUG_SUB=4@255 (sub04 = Kampfstart:
#           grid 0x33 -> 0x13, Lage (-9000,-23400); Vorspann bis ~Bild 346, danach kriecht er), RE15_FORCE_EXPLOSION
#           im Bild 400 am kriechenden Boss: (a) vor dem Treffer armiert (g=13), (b) HP faellt um 80 (E16
#           @0x800A5F7C), (c) REAKTION: 40 Bilder spaeter laeuft sein Clip weiter (af anders) und er hat sich
#           bewegt (> 200) — kein Haenger. (Abnahme 0, Mangel 5: der alte Pin traf den geparkten Boss, grid 0x33.)
#   duenn_a / duenn_b  ROOM1220 (Zellentrakt), HOCH-Wurf (380 je Bild @0x80018494) gegen die 275 dicke Zellenfront
#           x[-21825..-21550] aus zwei Wurfphasen (Leon x -19533 / -19433; Abnahme 0, Laeufe t1220h1/t1220h2):
#           a = ein Bildpunkt faellt IN die Zelle, b = die Bildpunkte -21503 / -21873 liegen davor und DAHINTER.
#           Beide: genau eine Wandzeile, Rueckzug x' > -21550 (Flurseite), kein Granatenbild mit x <= -21550,
#           Explosion im Wandtick; b zusaetzlich: der Wandpunkt liegt hinter der Zelle (x < -21825) = der Fall,
#           den der Punkttest je Bild verfehlte (vorher: Explosion bei x -25811 in der Zelle).
#   raute   ROOM11C0 nach der Ada-Szene (Skriptstart 1075; Abnahme 0, Lauf t11c0c), MITTE-Wurf aus der Hand
#           (-6720,-13266): der Punkt liegt im Rechteck der Raute x[-15400..4799] z[-13266..4921], ausserhalb der
#           Raute. Vorher: Wandzeile im Wurfbild (0 Flugbilder). Jetzt: >= 5 Flugbilder vor der ersten Wandzeile.
# Boss-Slots nach RE15_DEBUG_JUMP (gemessen r35a_mess/b1_2090 + b2_5090, Bild 100): ROOM2090 Alligator = Slot 15
# (t=23, HP 3000, Lauerstellung), ROOM5090 Birkin = Slot 2 (t=36, HP 600, grid 0x33 vor dem Kampfstart).
# Ein Lauf OHNE Ergebnis (exit=1 = von aussen beendet; debug.log < 10 Zeilen = Startfehler) wird EINMAL wiederholt.

cmake_minimum_required(VERSION 3.16)
set(_tag "r35_granate")
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "${_tag}: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "${_tag}: WORKDIR fehlt")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_wurzel "${WORKDIR}/lauf_${_lauf_id}")
file(MAKE_DIRECTORY "${_wurzel}")

function(r35_fehler _lauf _text)
    message(FATAL_ERROR "${_tag} [${_lauf}]: ${_text}")
endfunction()

function(r35_exe _lauf _dir _timeout)
    file(MAKE_DIRECTORY "${_dir}")
    set(WORKDIR "${_dir}")
    re15_start_spiel(_rv ${_timeout} ${ARGN} "${RE15_PC_EXE}")
    if(NOT _rv EQUAL 0)
        set(_z "")
        if(EXISTS "${_dir}/debug.log")
            file(STRINGS "${_dir}/debug.log" _z)
        endif()
        list(LENGTH _z _n)
        # SDL-Assertion WIN_AddDisplay (SDL_windowsmodes.c: Display-Topologie aendert sich waehrend des Laufs, z.B. durch
        # fremde Fenster paralleler Baeume): ohne SDL_ASSERT=always_ignore blockiert ihr Dialog die exe bis zum Timeout
        # (gemessen Nachbesserung 1, Lauf raute: Stillstand bei Bild 779, debug.log traegt die Assertion). Umgebungs-
        # ereignis, kein Spielbefund -> ebenfalls EIN Wiederholungsversuch.
        set(_sdl 0)
        foreach(_zz IN LISTS _z)
            if(_zz MATCHES "Assertion failure at WIN_AddDisplay")
                set(_sdl 1)
            endif()
        endforeach()
        if(_n LESS 10 OR "${_rv}" STREQUAL "1" OR _sdl)
            message(STATUS "${_tag} [${_lauf}]: Lauf ohne Ergebnis (exit=${_rv}, debug.log ${_n} Zeilen, SDL-Assertion ${_sdl}) -> EIN Wiederholungsversuch")
            file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/gr.log" "${_dir}/wf.log")
            re15_start_spiel(_rv ${_timeout} ${ARGN} "${RE15_PC_EXE}")
        endif()
    endif()
    if(NOT _rv EQUAL 0)
        r35_fehler("${_lauf}" "re15_pc.exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT — Haenger/Absturz?), ${_dir}")
    endif()
    file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
    if(NOT _ex)
        r35_fehler("${_lauf}" "RE15_EXIT_AT nicht erreicht (keine Zeile '[flow] EXIT_AT') — Haenger?")
    endif()
endfunction()

# Zustandslog NACH dem Sprung (Bildzaehler faellt beim Raumwechsel zurueck): _Z_<F> = letzte Zeile je Bild.
macro(r35_zustand _datei)
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
            if(_nach)
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

# Felder eines Gegners <slot> aus einer Zustandszeile: <p>_T/ST/SS1/SS2/MO/X/Z/HP (leer = nicht enthalten).
function(r35_gegner _zeile _slot _p)
    set(${_p}_HP "" PARENT_SCOPE)
    if(_zeile MATCHES "\\[${_slot} t=([0-9a-f]+) st=([0-9]+) ss1=([0-9]+) ss2=([0-9]+) ss3=[0-9]+ g=[0-9a-f]+ mo=([0-9]+) af=[0-9]+ stun=-?[0-9]+ d=[0-9]+ @\\((-?[0-9]+),(-?[0-9]+),r-?[0-9]+\\)\\] hp=(-?[0-9]+)")
        set(${_p}_T   "${CMAKE_MATCH_1}" PARENT_SCOPE)
        set(${_p}_ST  "${CMAKE_MATCH_2}" PARENT_SCOPE)
        set(${_p}_SS1 "${CMAKE_MATCH_3}" PARENT_SCOPE)
        set(${_p}_SS2 "${CMAKE_MATCH_4}" PARENT_SCOPE)
        set(${_p}_MO  "${CMAKE_MATCH_5}" PARENT_SCOPE)
        set(${_p}_X   "${CMAKE_MATCH_6}" PARENT_SCOPE)
        set(${_p}_Z   "${CMAKE_MATCH_7}" PARENT_SCOPE)
        set(${_p}_HP  "${CMAKE_MATCH_8}" PARENT_SCOPE)
    endif()
endfunction()

# Granatenlog: T->F je Platzzeile, Wandzeile, Resolver-Zeile, Liegen-Tick, Explosions-Tick.
function(r35_granatenlog _datei _p)
    file(STRINGS "${_datei}" _gl)
    set(_wand_n 0)
    set(_res_n 0)
    set(_tl "")
    set(_tx "")
    set(_twand "")
    set(${_p}_WAND_X "" PARENT_SCOPE)
    set(${_p}_WAND_XR "" PARENT_SCOPE)
    set(${_p}_EINGRIFFE "" PARENT_SCOPE)
    set(${_p}_XMAX_NACH_WAND "" PARENT_SCOPE)
    set(${_p}_WAND_YR "" PARENT_SCOPE)
    set(${_p}_PY "" PARENT_SCOPE)
    set(_xmax_nach "")
    set(_xmin "")
    set(_flug_n 0)
    foreach(_l IN LISTS _gl)
        if(_l MATCHES "^T=([0-9]+) F=([0-9]+) slot=[0-9]+ .* wpos=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)")
            if(NOT DEFINED _tf_${CMAKE_MATCH_1})
                set(_tf_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}")
            endif()
            if("${_xmin}" STREQUAL "" OR CMAKE_MATCH_3 LESS _xmin)
                set(_xmin "${CMAKE_MATCH_3}")
            endif()
            if("${_twand}" STREQUAL "")
                math(EXPR _flug_n "${_flug_n} + 1")
            endif()
            if(NOT "${_twand}" STREQUAL "" AND _xmax_nach STREQUAL "")
                set(_xmax_nach "${CMAKE_MATCH_3}")
            elseif(NOT "${_twand}" STREQUAL "" AND CMAKE_MATCH_3 GREATER _xmax_nach)
                set(_xmax_nach "${CMAKE_MATCH_3}")
            endif()
        elseif(_l MATCHES "^T=([0-9]+) EV wand wpos=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\) -> rueckzug \\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)")
            math(EXPR _wand_n "${_wand_n} + 1")
            set(_twand "${CMAKE_MATCH_1}")
            set(${_p}_WAND_X  "${CMAKE_MATCH_2}" PARENT_SCOPE)
            set(${_p}_WAND_XR "${CMAKE_MATCH_5}" PARENT_SCOPE)
            set(${_p}_WAND_YR "${CMAKE_MATCH_6}" PARENT_SCOPE)
        elseif(_l MATCHES "^T=([0-9]+) EV se code=[0-9a-f]+ pos=.* liegen$")
            if("${_tl}" STREQUAL "")
                set(_tl "${CMAKE_MATCH_1}")
            endif()
        elseif(_l MATCHES "^T=([0-9]+) EV resolver art=([0-9]+) P=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\) r=([0-9]+) eingriffe=([0-9]+)")
            math(EXPR _res_n "${_res_n} + 1")
            set(_tx "${CMAKE_MATCH_1}")
            set(${_p}_R "${CMAKE_MATCH_6}" PARENT_SCOPE)
            set(${_p}_PY "${CMAKE_MATCH_4}" PARENT_SCOPE)
            set(${_p}_EINGRIFFE "${CMAKE_MATCH_7}" PARENT_SCOPE)
        endif()
    endforeach()
    set(${_p}_WAND_N ${_wand_n} PARENT_SCOPE)
    set(${_p}_XMIN "${_xmin}" PARENT_SCOPE)
    set(${_p}_FLUG_N ${_flug_n} PARENT_SCOPE)
    set(${_p}_RES_N ${_res_n} PARENT_SCOPE)
    set(${_p}_TWAND "${_twand}" PARENT_SCOPE)
    set(${_p}_TX "${_tx}" PARENT_SCOPE)
    set(${_p}_XMAX_NACH_WAND "${_xmax_nach}" PARENT_SCOPE)
    set(${_p}_L "" PARENT_SCOPE)
    set(${_p}_X "" PARENT_SCOPE)
    if(NOT "${_tl}" STREQUAL "" AND DEFINED _tf_${_tl})
        set(${_p}_L "${_tf_${_tl}}" PARENT_SCOPE)
    endif()
    if(NOT "${_tx}" STREQUAL "" AND DEFINED _tf_${_tx})
        set(${_p}_X "${_tf_${_tx}}" PARENT_SCOPE)
    endif()
endfunction()

# Toene: RE2-Explosion genau einmal, RE1.5-CORE-Explosion nie.
function(r35_pruef_toene _datei _lauf)
    file(STRINGS "${_datei}" _w REGEX "SE  (esp|re2fx) code=")
    set(_n_re2 0)
    set(_n_alt 0)
    foreach(_l IN LISTS _w)
        if(_l MATCHES "SE  re2fx code=0x01110001 -> ARMS0F Satz 10")
            math(EXPR _n_re2 "${_n_re2} + 1")
        elseif(_l MATCHES "SE  esp code=0x04080001 ")
            math(EXPR _n_alt "${_n_alt} + 1")
        endif()
    endforeach()
    if(NOT _n_re2 EQUAL 1 OR NOT _n_alt EQUAL 0)
        r35_fehler("${_lauf}" "Toene: ${_n_re2}x RE2 0x01110001 -> ARMS0F Satz 10 (Soll 1), ${_n_alt}x RE1.5 0x04080001 (Soll 0)")
    endif()
endfunction()

# ---------------------------------------------------------------------------------------------------
# NEGATIV-KONTROLLE der Auswerter: erfundene Zeilen, die fallen MUESSEN.
# ---------------------------------------------------------------------------------------------------
r35_gegner("F120 PL(-1676,-18070,rot=1076,hp=100) [3 t=10 st=3 ss1=9 ss2=1 ss3=0 g=00 mo=2 af=15 stun=6 d=2660 @(-1509,-21329,r3037)] hp=-120" 3 _nk)
if(NOT "${_nk_HP}" STREQUAL "-120" OR NOT "${_nk_MO}" STREQUAL "2" OR NOT "${_nk_SS1}" STREQUAL "9" OR NOT "${_nk_Z}" STREQUAL "-21329")
    message(FATAL_ERROR "${_tag}: Auswerter r35_gegner defekt (hp '${_nk_HP}' mo '${_nk_MO}' ss1 '${_nk_SS1}' z '${_nk_Z}')")
endif()
r35_gegner("F120 PL(...) [4 t=10 st=1 ...] hp=80" 3 _nk2)
if(NOT "${_nk2_HP}" STREQUAL "")
    message(FATAL_ERROR "${_tag}: Auswerter r35_gegner findet einen fremden Slot")
endif()
message(STATUS "${_tag}: Negativ-Kontrolle der Auswerter ok")

# Nur fuer Mutationsproben: -DR35_NUR="wand;zombie" faehrt nur diese Laeufe.
function(r35_laeuft _lauf _aus)
    set(${_aus} 1 PARENT_SCOPE)
    if(DEFINED R35_NUR AND NOT "${R35_NUR}" STREQUAL "")
        list(FIND R35_NUR "${_lauf}" _i)
        if(_i LESS 0)
            set(${_aus} 0 PARENT_SCOPE)
        endif()
    endif()
endfunction()

# SDL_ASSERT=always_ignore: SDL-eigene Umgebungsvariable (SDL_assert.c) — eine SDL-Assertion oeffnet sonst einen Dialog,
# der die exe anhaelt (s. r35_exe). Kein Spielschalter.
set(_env_basis RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
               RE15_STATE_LOG=state.log RE15_GRANATE_LOG=gr.log RE15_WAFFEN_LOG=wf.log SDL_ASSERT=always_ignore)

# ---------------------------------------------------------------------------------------------------
# Lauf "wand"
# ---------------------------------------------------------------------------------------------------
r35_laeuft(wand _an)
if(_an)
    set(_dir "${_wurzel}/wand")
    r35_exe(wand "${_dir}" 240 ${_env_basis}
        RE15_DEBUG_JUMP=1140@250 "RE15_PLAYER_POS=-4311,-19289,0" RE15_AI_FLAVOR=re2 RE15_GIVE=9:5 RE15_EQUIP=9
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=M0.6,MA0.2,M2.5,W5"
        "RE15_EXIT_AT=260#1140")
    r35_granatenlog("${_dir}/gr.log" _G)
    if(NOT _G_WAND_N EQUAL 1)
        r35_fehler(wand "${_G_WAND_N} Wandzeilen im Granatenlog, erwartet genau 1 (Zelle x8350 z-20750 w4100 d1150)")
    endif()
    if(NOT _G_WAND_X GREATER_EQUAL 8350 OR NOT _G_WAND_XR LESS 8350)
        r35_fehler(wand "Wand bei x=${_G_WAND_X} (soll >= 8350, in der Zelle), Rueckzug x'=${_G_WAND_XR} (soll < 8350, RE2 @0x8001ef90-0cc)")
    endif()
    if(NOT _G_RES_N EQUAL 1 OR NOT "${_G_TX}" STREQUAL "${_G_TWAND}")
        r35_fehler(wand "${_G_RES_N} Resolver-Aufrufe, Explosions-Tick ${_G_TX} vs Wand-Tick ${_G_TWAND} (soll 1, gleicher Tick @0x8001f0e0-104)")
    endif()
    if(NOT "${_G_XMAX_NACH_WAND}" STREQUAL "" AND _G_XMAX_NACH_WAND GREATER 8349)
        r35_fehler(wand "Granate nach der Wand bis x=${_G_XMAX_NACH_WAND} (Durchflug; soll < 8350)")
    endif()
    if(NOT "${_G_R}" STREQUAL "2000")
        r35_fehler(wand "Resolver-Reichweite r=${_G_R} (soll 2000, RE2 Box @0x80010918)")
    endif()
    if(NOT "${_G_PY}" STREQUAL "${_G_WAND_YR}")
        r35_fehler(wand "Explosionspunkt P.y=${_G_PY}, Rueckzugspunkt y=${_G_WAND_YR} (soll gleich: RE2 Op 47 liest die Lage ohne Versatz @0x80020cdc-fc)")
    endif()
    r35_pruef_toene("${_dir}/wf.log" wand)
    message(STATUS "${_tag} [wand]: ok — Wand x=${_G_WAND_X} -> ${_G_WAND_XR}, Explosion Tick ${_G_TX}")
endif()

# ---------------------------------------------------------------------------------------------------
# Lauf "zombie"
# ---------------------------------------------------------------------------------------------------
r35_laeuft(zombie _an)
if(_an)
    set(_dir "${_wurzel}/zombie")
    r35_exe(zombie "${_dir}" 240 ${_env_basis}
        RE15_DEBUG_JUMP=1140@250 "RE15_PLAYER_POS=-1676,-18070,1076" RE15_AI_FLAVOR=re2 RE15_GIVE=9:5 RE15_EQUIP=9
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=MD0.6,MDA0.2,MD2.5,W5"
        "RE15_EXIT_AT=260#1140")
    r35_granatenlog("${_dir}/gr.log" _G)
    if("${_G_L}" STREQUAL "" OR "${_G_X}" STREQUAL "")
        r35_fehler(zombie "kein Liegen (${_G_L}) oder keine Explosion (${_G_X}) im Granatenlog")
    endif()
    math(EXPR _x_soll "${_G_L} + 36")
    if(NOT _G_X EQUAL _x_soll OR NOT _G_WAND_N EQUAL 0)
        r35_fehler(zombie "Explosion im Bild ${_G_X}, erwartet L + 36 = ${_x_soll} (Zeitzuender @0x80018474/@0x8001856c), Wandzeilen ${_G_WAND_N} (0)")
    endif()
    if(NOT _G_EINGRIFFE GREATER_EQUAL 2)
        r35_fehler(zombie "Explosion traf ${_G_EINGRIFFE} Gegner (soll >= 2 mit Reichweite 2000; vorher 1 mit 900)")
    endif()
    r35_zustand("${_dir}/state.log")
    math(EXPR _xn "${_G_X} + 1")
    math(EXPR _x10 "${_G_X} + 10")
    math(EXPR _x80 "${_G_X} + 80")
    math(EXPR _xv "${_G_X} - 1")
    set(_getroffen 0)
    set(_gore 0)
    set(_leichen 0)
    foreach(_s RANGE 1 15)
        r35_gegner("${_Z_${_xv}}" ${_s} _V)
        r35_gegner("${_Z_${_G_X}}" ${_s} _X)
        if("${_V_HP}" STREQUAL "" OR "${_X_HP}" STREQUAL "")
            continue()
        endif()
        if(NOT _X_HP LESS _V_HP OR NOT "${_X_T}" STREQUAL "10")
            continue()
        endif()
        math(EXPR _getroffen "${_getroffen} + 1")
        if(NOT _X_SS1 EQUAL 9 OR NOT _X_ST EQUAL 3)
            r35_fehler(zombie "Zombie ${_s}: +0x5 ${_X_SS1} Zustand ${_X_ST} im Explosionsbild (soll 9 / 3)")
        endif()
        r35_gegner("${_Z_${_xn}}" ${_s} _N)
        r35_gegner("${_Z_${_x10}}" ${_s} _N10)
        r35_gegner("${_Z_${_x80}}" ${_s} _N80)
        if("${_N_MO}" STREQUAL "1")
            r35_fehler(zombie "Zombie ${_s}: Clip 1 im Bild X+1 = Sturz-Tod 0x80108530 (Spalte 3, vorher) statt Zerreissen 0x80108BEC")
        endif()
        if(NOT "${_N10_Z}" STREQUAL "" AND NOT "${_X_Z}" STREQUAL "")
            math(EXPR _dz "${_N10_Z} - ${_X_Z}")
            math(EXPR _dx "${_N10_X} - ${_X_X}")
            if(_dz LESS 0)
                math(EXPR _dz "0 - ${_dz}")
            endif()
            if(_dx LESS 0)
                math(EXPR _dx "0 - ${_dx}")
            endif()
            math(EXPR _d "${_dx} + ${_dz}")
            if(_d GREATER 600)
                math(EXPR _gore "${_gore} + 1")
            endif()
        endif()
        if("${_N80_ST}" STREQUAL "7")
            math(EXPR _leichen "${_leichen} + 1")
        endif()
    endforeach()
    if(_getroffen LESS 2)
        r35_fehler(zombie "nur ${_getroffen} Zombies 0x10 verlieren im Explosionsbild HP (soll >= 2)")
    endif()
    if(NOT _gore EQUAL _getroffen)
        r35_fehler(zombie "${_gore}/${_getroffen} getroffene Zombies weggeschleudert (> 600 in 10 Bildern, re2z_death_rip)")
    endif()
    if(NOT _leichen EQUAL _getroffen)
        r35_fehler(zombie "${_leichen}/${_getroffen} getroffene Zombies in Zustand 7 (Leiche) bis Bild X+80 — Wiederbelebung?")
    endif()
    r35_pruef_toene("${_dir}/wf.log" zombie)
    message(STATUS "${_tag} [zombie]: ok — X=${_G_X}, ${_getroffen} Zombies zerrissen, ${_G_EINGRIFFE} Eingriffe")
endif()

# ---------------------------------------------------------------------------------------------------
# Laeufe "gator" (ROOM2090) und "birkin" (ROOM5090) — RE15_FORCE_EXPLOSION am Boss (Mess-Haken, Integration
# W6: Explosion so, als laege die Granate am Gegner <slot>; seit Runde 35 dieselbe Zustellung wie Routine 31).
# ---------------------------------------------------------------------------------------------------
function(r35_boss _lauf _raum _slot _typ _bild _dmg)
    set(_dir "${_wurzel}/${_lauf}")
    math(EXPR _exit "${_bild} + 40")
    r35_exe(${_lauf} "${_dir}" 240 ${_env_basis}
        RE15_DEBUG_JUMP=${_raum}@250 "RE15_FORCE_EXPLOSION=2@${_bild}:${_slot}" "RE15_EXIT_AT=${_exit}#${_raum}")
    r35_zustand("${_dir}/state.log")
    math(EXPR _bv "${_bild} - 1")
    r35_gegner("${_Z_${_bv}}" ${_slot} _V)
    r35_gegner("${_Z_${_bild}}" ${_slot} _X)
    if("${_V_HP}" STREQUAL "" OR "${_X_HP}" STREQUAL "")
        r35_fehler(${_lauf} "Boss-Slot ${_slot} fehlt im Zustandslog (Bild ${_bv}/${_bild})")
    endif()
    if(NOT "${_X_T}" STREQUAL "${_typ}")
        r35_fehler(${_lauf} "Slot ${_slot} ist Typ ${_X_T}, erwartet ${_typ}")
    endif()
    math(EXPR _hp_soll "${_V_HP} - ${_dmg}")
    if(NOT _X_HP EQUAL _hp_soll)
        r35_fehler(${_lauf} "HP ${_V_HP} -> ${_X_HP} im Bild ${_bild} (soll ${_hp_soll}: Schaden ${_dmg})")
    endif()
    file(STRINGS "${_dir}/wf.log" _fe REGEX "FORCE_EXPLOSION|AUFSCHLAG")
    set(${_lauf}_HPV "${_V_HP}" PARENT_SCOPE)
    set(${_lauf}_HPX "${_X_HP}" PARENT_SCOPE)
    message(STATUS "${_tag} [${_lauf}]: ok — ${_typ} Slot ${_slot}: HP ${_V_HP} -> ${_X_HP}, Zustand ${_X_ST}, exe bis EXIT_AT")
endfunction()

r35_laeuft(gator _an)
if(_an)
    # Gator-Boss 0x23 in ROOM2090: GB_HP 3000 (enemy_ai_boss_gator.c, DESIGN), RE1.5 Art 2 = 1000 (@0x8006f41c).
    r35_boss(gator 2090 15 23 60 1000)
endif()
# Felder g (grid) und af eines Gegners <slot> (CMake kennt nur 9 Fanggruppen -> eigener Auswerter).
function(r35_gegner_g _zeile _slot _p)
    set(${_p}_G "" PARENT_SCOPE)
    set(${_p}_AF "" PARENT_SCOPE)
    if(_zeile MATCHES "\\[${_slot} t=[0-9a-f]+ st=[0-9]+ ss1=[0-9]+ ss2=[0-9]+ ss3=[0-9]+ g=([0-9a-f]+) mo=[0-9]+ af=([0-9]+) ")
        set(${_p}_G  "${CMAKE_MATCH_1}" PARENT_SCOPE)
        set(${_p}_AF "${CMAKE_MATCH_2}" PARENT_SCOPE)
    endif()
endfunction()

r35_laeuft(birkin _an)
if(_an)
    # G5 EM36 in ROOM5090: HP 600 (@0x801003fc), Granate = RE2-Record Zeile 9 K0 = 80 (E16 @0x800A5F7C).
    # ARMIERT: sub04 (Kampfstart) ueber RE15_DEBUG_SUB=4@255 -> grid 0x13, Vorspann bis ~Bild 346, danach kriecht
    # der Boss (gemessen nb1/birkin_c: F380 x -8808, F400 x -7373). Explosion im Bild 400.
    set(_dir "${_wurzel}/birkin")
    r35_exe(birkin "${_dir}" 300 ${_env_basis}
        RE15_DEBUG_JUMP=5090@250 RE15_DEBUG_SUB=4@255 "RE15_FORCE_EXPLOSION=2@400:2" "RE15_EXIT_AT=460#5090")
    r35_zustand("${_dir}/state.log")
    r35_gegner("${_Z_399}" 2 _V)
    r35_gegner("${_Z_400}" 2 _X)
    r35_gegner("${_Z_440}" 2 _N)
    r35_gegner_g("${_Z_399}" 2 _VG)
    r35_gegner_g("${_Z_400}" 2 _XG)
    r35_gegner_g("${_Z_440}" 2 _NG)
    if("${_V_HP}" STREQUAL "" OR "${_X_HP}" STREQUAL "" OR "${_N_HP}" STREQUAL "")
        r35_fehler(birkin "Boss-Slot 2 fehlt im Zustandslog (Bild 399/400/440)")
    endif()
    if(NOT "${_X_T}" STREQUAL "36" OR NOT "${_VG_G}" STREQUAL "13")
        r35_fehler(birkin "Slot 2: Typ ${_X_T} grid ${_VG_G} im Bild 399 (soll 36 / 13 = ARMIERT ueber sub04; 33 = geparkt)")
    endif()
    math(EXPR _hp_soll "${_V_HP} - 80")
    if(NOT _X_HP EQUAL _hp_soll OR NOT _N_HP EQUAL _hp_soll)
        r35_fehler(birkin "HP ${_V_HP} -> ${_X_HP} im Bild 400, ${_N_HP} im Bild 440 (soll ${_hp_soll}: Schaden 80 @0x800A5F7C, genau einmal)")
    endif()
    math(EXPR _dx "${_N_X} - ${_X_X}")
    math(EXPR _dz "${_N_Z} - ${_X_Z}")
    if(_dx LESS 0)
        math(EXPR _dx "0 - ${_dx}")
    endif()
    if(_dz LESS 0)
        math(EXPR _dz "0 - ${_dz}")
    endif()
    math(EXPR _d "${_dx} + ${_dz}")
    if(NOT _d GREATER 200 OR "${_NG_AF}" STREQUAL "${_XG_AF}")
        r35_fehler(birkin "Reaktion: 40 Bilder nach dem Treffer Weg ${_d} (soll > 200), af ${_XG_AF} -> ${_NG_AF} (soll anders) — Boss haengt")
    endif()
    message(STATUS "${_tag} [birkin]: ok — 36 Slot 2 armiert (g=${_VG_G}): HP ${_V_HP} -> ${_X_HP}, danach Weg ${_d}, af ${_XG_AF} -> ${_NG_AF}, exe bis EXIT_AT")
endif()

# ---------------------------------------------------------------------------------------------------
# Laeufe "duenn_a" / "duenn_b" — ROOM1220, duenne Zellenfront x[-21825..-21550], HOCH, zwei Wurfphasen
# ---------------------------------------------------------------------------------------------------
function(r35_duenn _lauf _px)
    set(_dir "${_wurzel}/${_lauf}")
    r35_exe(${_lauf} "${_dir}" 240 ${_env_basis}
        RE15_DEBUG_JUMP=1220@250 "RE15_PLAYER_POS=${_px},-9700,2048" RE15_GIVE=9:5 RE15_EQUIP=9
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=MU0.6,MUA0.2,MU2.5,W5"
        "RE15_EXIT_AT=260#1220")
    r35_granatenlog("${_dir}/gr.log" _G)
    if(NOT _G_WAND_N EQUAL 1)
        r35_fehler(${_lauf} "${_G_WAND_N} Wandzeilen im Granatenlog, erwartet genau 1 (Zellenfront x[-21825..-21550])")
    endif()
    if(NOT _G_WAND_XR GREATER -21550)
        r35_fehler(${_lauf} "Rueckzug x'=${_G_WAND_XR} liegt nicht auf der Flurseite der Zellenfront (soll > -21550)")
    endif()
    if("${_G_XMIN}" STREQUAL "" OR NOT _G_XMIN GREATER -21550)
        r35_fehler(${_lauf} "Granatenbild mit x=${_G_XMIN} hinter/in der Zellenfront (soll > -21550: kein Durchflug)")
    endif()
    if(NOT _G_RES_N EQUAL 1 OR NOT "${_G_TX}" STREQUAL "${_G_TWAND}")
        r35_fehler(${_lauf} "${_G_RES_N} Resolver-Aufrufe, Explosions-Tick ${_G_TX} vs Wand-Tick ${_G_TWAND} (soll 1, gleicher Tick)")
    endif()
    set(${_lauf}_WAND_X "${_G_WAND_X}" PARENT_SCOPE)
    message(STATUS "${_tag} [${_lauf}]: ok — Wandpunkt x=${_G_WAND_X} -> Rueckzug ${_G_WAND_XR}, kleinstes Granaten-x ${_G_XMIN}")
endfunction()

r35_laeuft(duenn_a _an)
if(_an)
    r35_duenn(duenn_a -19533)
    if(NOT duenn_a_WAND_X LESS -21550 OR NOT duenn_a_WAND_X GREATER -21826)
        r35_fehler(duenn_a "Phase a: Wandpunkt x=${duenn_a_WAND_X} sollte IN der Zelle liegen (-21825..-21550) — Aufstellung verschoben?")
    endif()
endif()
r35_laeuft(duenn_b _an)
if(_an)
    r35_duenn(duenn_b -19433)
    if(NOT duenn_b_WAND_X LESS -21825)
        r35_fehler(duenn_b "Phase b: Wandpunkt x=${duenn_b_WAND_X} sollte HINTER der Zelle liegen (< -21825: der Fall, den der Punkttest verfehlt) — Aufstellung verschoben?")
    endif()
endif()

# ---------------------------------------------------------------------------------------------------
# Lauf "raute" — ROOM11C0, Wurf aus dem Begrenzungsrechteck einer Raute (Zellform statt Rechteck)
# ---------------------------------------------------------------------------------------------------
r35_laeuft(raute _an)
if(_an)
    set(_dir "${_wurzel}/raute")
    r35_exe(raute "${_dir}" 400 ${_env_basis}
        RE15_DEBUG_JUMP=11C0@250 RE15_GIVE=9:5 RE15_EQUIP=9
        RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1075 "RE15_INPUT_SCRIPT=M0.6,MA0.2,M2.5,W5"
        "RE15_EXIT_AT=1300#11C0")
    file(STRINGS "${_dir}/gr.log" _sp REGEX "SPAWN granate art=2 .* anker=")
    if(NOT _sp)
        r35_fehler(raute "kein Wurf im Granatenlog (SPAWN fehlt) — Skriptstart 1075 liegt nicht mehr hinter der Ada-Szene?")
    endif()
    list(GET _sp 0 _sp0)
    if(NOT _sp0 MATCHES "anker=\\((-?[0-9]+),(-?[0-9]+),(-?[0-9]+)\\)")
        r35_fehler(raute "SPAWN-Zeile ohne Anker: ${_sp0}")
    endif()
    set(_ax "${CMAKE_MATCH_1}")
    set(_az "${CMAKE_MATCH_3}")
    r35_granatenlog("${_dir}/gr.log" _G)
    if(_G_FLUG_N LESS 5)
        r35_fehler(raute "nur ${_G_FLUG_N} Flugbilder vor der ersten Wandzeile (Hand (${_ax},${_az}); soll >= 5 — vorher 0: Explosion im Wurfbild im Rechteck der Raute)")
    endif()
    if(NOT _G_RES_N EQUAL 1)
        r35_fehler(raute "${_G_RES_N} Explosionen (soll genau 1 bis Bild 1300)")
    endif()
    message(STATUS "${_tag} [raute]: ok — Hand (${_ax},${_az}), ${_G_FLUG_N} Flugbilder, Wandzeilen ${_G_WAND_N}, Explosion Tick ${_G_TX}")
endif()
