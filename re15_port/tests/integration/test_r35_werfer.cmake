# Runde 35 Spur B — Integration mit der ECHTEN exe: Granatwerfer 15/16/17, Raketenwerfer 18,
# Flammenwerfer 14, Colt Python 20 (Beta -> Retail, RE2-Mechanik).
#
# Dossier: analysis/befunde_runde35/B_werfer.md §4/§5. Registrierung: tests/unit/probes/r35_werfer.cmake.
# Harness wie test_r34_granaten.cmake: ROOM1140 per RE15_DEBUG_JUMP=1140@250, Leon (-1676,-18070)
# Blick 1076 (die Zombies 0x10 bei (-1800,-19600)/(-1800,-21600) stehen vor ihm), RE15_GIVE/RE15_EQUIP,
# Eingabe "M0.6,MA1.5,M1.0,W4" (Heben, Abzug 45 Bilder halten, Halten, Warten), Logs wf.log (Waffen),
# state.log (Gegner), debug.log (Ablauf). Je Lauf wird gemessen:
#   15  Rueckstossbild 1: "RE2SPAWN a0=01002000" + 5 x "RE2SPAWN a0=020c0a00" (RE2 @0x80044b44),
#       Explosions-Ton "re2fx code=0x01110001 -> ARMS0F Satz 10" (Op 47 Sub 12), Knall 0x01000001,
#       Gegner-HP faellt, Reaktionszeile ss1=9 (RE2-KI), kein Haenger (EXIT_AT).
#   16  "RE2SPAWN a0=020c1000" + "03081200", Ton 0x01130001 (Op 49), ss1=11.
#   17  Ton 0x01120001 (Op 48), ss1=10.
#   18  "RE2SPAWN a0=020d1000" + 030a1a00 + 030a1500, Ton 0x01140001 -> RE2 ARMS11 Satz 20 (Op 47
#       Sub 13), Gegner-HP faellt um 900 (RE2-Zeile 17), ss1=17.
#   14  >= 6 x "RE2SPAWN a0=031d1200" (je 3. Bild), Toene "re2arms ARMS10 satz=0" und "satz=11",
#       Gegner-HP faellt um 15 je Treffer (RE2-Zeile 16), ss1=16; Fuel 6 -> 0 nach 24 Bildern.
#   20  "SPAWN id=2 sub=0 scale=0xe00" (Redhawk-Muendung 0x02000E00) + "id=3 sub=0 scale=0x1000",
#       "SE  arms_rec=0", Gegner-HP faellt um 900 (RE2-Zeile 5), ss1=5.
# Ein Lauf ohne Ergebnis (exit=1 = von aussen beendet; debug.log < 10 Zeilen) wird EINMAL wiederholt.

cmake_minimum_required(VERSION 3.16)
set(_tag "r35_werfer")
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

# Ein exe-Lauf fuer Waffe _id; Wiederholung genau einmal bei exit=1 / Startfehler.
function(r35_exe _lauf _dir _id)
    file(MAKE_DIRECTORY "${_dir}")
    set(WORKDIR "${_dir}")
    set(_env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
             RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1140@250 "RE15_PLAYER_POS=-1676,-18070,1076"
             RE15_AI_FLAVOR=re2 "RE15_GIVE=${_id}:6" "RE15_EQUIP=${_id}"
             RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=M0.6,MA1.5,M1.0,W4"
             RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log "RE15_EXIT_AT=200#1140")
    re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
    if(NOT _rv EQUAL 0)
        set(_z "")
        if(EXISTS "${_dir}/debug.log")
            file(STRINGS "${_dir}/debug.log" _z)
        endif()
        list(LENGTH _z _n)
        # exit 1/-1 = von aussen beendet (fremdes taskkill, ein parallel laufender local_build.sh
        # beendet die exe seines Bauverzeichnisses) -> genau EIN Wiederholungsversuch.
        if(_n LESS 10 OR "${_rv}" STREQUAL "1" OR "${_rv}" STREQUAL "-1")
            message(STATUS "${_tag} [${_lauf}]: Lauf ohne Ergebnis (exit=${_rv}, debug.log ${_n} Zeilen) -> EIN Wiederholungsversuch")
            file(REMOVE "${_dir}/debug.log" "${_dir}/state.log" "${_dir}/wf.log")
            re15_start_spiel(_rv 240 ${_env} "${RE15_PC_EXE}")
        endif()
    endif()
    if(NOT _rv EQUAL 0)
        r35_fehler("${_lauf}" "re15_pc.exe exit=${_rv} (erwartet 0 bis RE15_EXIT_AT — Haenger/Absturz?), ${_dir}")
    endif()
    foreach(_d debug.log state.log wf.log)
        if(NOT EXISTS "${_dir}/${_d}")
            r35_fehler("${_lauf}" "kein ${_d} in ${_dir}")
        endif()
    endforeach()
    file(STRINGS "${_dir}/debug.log" _ex REGEX "\\[flow\\] EXIT_AT: ")
    if(NOT _ex)
        r35_fehler("${_lauf}" "RE15_EXIT_AT nicht erreicht (keine Zeile '[flow] EXIT_AT') — Haenger?")
    endif()
endfunction()

# Zaehlt Zeilen in einer Datei, die auf _rx passen.
function(r35_zaehle _datei _rx _aus)
    file(STRINGS "${_datei}" _l REGEX "${_rx}")
    list(LENGTH _l _n)
    set(${_aus} ${_n} PARENT_SCOPE)
endfunction()

# Gegner-HP NACH dem Sprung: groesster (= Ausgangs-) und kleinster HP-Wert sowie die Reaktionszeile
# (ss1) des ersten HP-Verlusts des Platzes _slot (Zeilen "F<n> ... [<slot> t=.. st=.. ss1=.. ... hp=..]";
# nach dem Raumwechsel faellt der Bildzaehler auf 1 zurueck — nur Zeilen ab dem ersten Rueckgang zaehlen;
# die ersten Bilder nach dem Sprung tragen hp=0, bevor das Raumskript die Gegner setzt — deshalb MAX).
function(r35_gegner _datei _slot _hp0 _hpmin _ss1 _bild)
    file(STRINGS "${_datei}" _zl)
    set(_vorher -1)
    set(_nach 0)
    set(_min 100000)
    set(_start -100000)
    set(_zeile "")
    set(_bildnr "")
    foreach(_l IN LISTS _zl)
        if(_l MATCHES "^F([0-9]+) ")
            set(_f "${CMAKE_MATCH_1}")
            if(NOT _nach AND _vorher GREATER _f)
                set(_nach 1)
            endif()
            set(_vorher ${_f})
            if(NOT _nach)
                continue()
            endif()
            if(_l MATCHES "\\[${_slot} t=[0-9a-f]+ st=([0-9]+) ss1=([0-9]+) ss2=[0-9]+ ss3=[0-9]+ g=[0-9a-f]+ mo=[0-9]+ af=[0-9]+ stun=-?[0-9]+ d=[0-9]+ @\\(-?[0-9]+,-?[0-9]+,r-?[0-9]+\\)\\] hp=(-?[0-9]+)")
                set(_st "${CMAKE_MATCH_1}")
                set(_s1 "${CMAKE_MATCH_2}")
                set(_hp "${CMAKE_MATCH_3}")
                if(_hp GREATER _start)
                    set(_start ${_hp})
                endif()
                if(_hp LESS _min)
                    set(_min ${_hp})
                    if("${_zeile}" STREQUAL "" AND _start GREATER 0 AND _hp LESS _start)
                        set(_zeile "${_s1}")
                        set(_bildnr "${_f}")
                    endif()
                endif()
            endif()
        endif()
    endforeach()
    set(${_hp0} ${_start} PARENT_SCOPE)
    set(${_hpmin} ${_min} PARENT_SCOPE)
    set(${_ss1} "${_zeile}" PARENT_SCOPE)
    set(${_bild} "${_bildnr}" PARENT_SCOPE)
endfunction()

# ---------------------------------------------------------------------------------------------------
# 15 Granatwerfer Explosiv
# ---------------------------------------------------------------------------------------------------
set(_d "${_wurzel}/w15")
r35_exe("w15" "${_d}" 15)
r35_zaehle("${_d}/wf.log" "RE2SPAWN a0=01002000" _n_mz)
r35_zaehle("${_d}/wf.log" "RE2SPAWN a0=020c0a00" _n_rd)
r35_zaehle("${_d}/wf.log" "SE  re2fx code=0x01110001 -> ARMS0F Satz 10" _n_ex)
r35_zaehle("${_d}/wf.log" "SE  re2fx code=0x01000001 -> ARMS Satz 0" _n_kn)
if(_n_mz LESS 1 OR _n_rd LESS 5)
    r35_fehler("w15" "RE2-Spawns fehlen: Muendung ${_n_mz} (erwartet >= 1), Runden ${_n_rd} (erwartet >= 5, 5 x 0x020C0A00 @0x80044be8-e74)")
endif()
if(_n_ex LESS 1 OR _n_kn LESS 1)
    r35_fehler("w15" "Toene fehlen: Explosion 0x01110001 ${_n_ex}, Knall 0x01000001 ${_n_kn}")
endif()
r35_gegner("${_d}/state.log" 2 _hp0 _hpmin _ss1 _bild)
if(_hpmin GREATER_EQUAL _hp0)
    r35_fehler("w15" "Zombie Platz 2 verliert keine HP (${_hp0} -> ${_hpmin}) — AUFSTELLUNG oder Explosion ohne Schaden")
endif()
if(NOT "${_ss1}" STREQUAL "9")
    r35_fehler("w15" "Reaktionszeile des Treffers ist ${_ss1}, erwartet 9 (RE2 GL Explosiv; Op 47 Hitcode 0x10020009)")
endif()
message(STATUS "${_tag} [w15]: Muendung ${_n_mz}, Runden ${_n_rd}, Explosionen ${_n_ex}, Zombie 2 HP ${_hp0} -> ${_hpmin} (Bild ${_bild}, ss1=${_ss1})")

# ---------------------------------------------------------------------------------------------------
# 16 Saeure / 17 Brand
# ---------------------------------------------------------------------------------------------------
foreach(_p "16;020c1000;0x01130001;ARMS10 Satz 10;11" "17;020c1000;0x01120001;ARMS11 Satz 10;10")
    list(GET _p 0 _id)
    list(GET _p 1 _code)
    list(GET _p 2 _se)
    list(GET _p 3 _bank)
    list(GET _p 4 _row)
    set(_d "${_wurzel}/w${_id}")
    r35_exe("w${_id}" "${_d}" ${_id})
    r35_zaehle("${_d}/wf.log" "RE2SPAWN a0=${_code}" _n_rd)
    r35_zaehle("${_d}/wf.log" "RE2SPAWN a0=03081200" _n_ra)
    r35_zaehle("${_d}/wf.log" "SE  re2fx code=${_se} -> ${_bank}" _n_se)
    if(_n_rd LESS 1 OR _n_ra LESS 1)
        r35_fehler("w${_id}" "RE2-Spawns fehlen: Runde ${_n_rd}, Rauch 0x03081200 ${_n_ra} (@0x80044f9c-c8)")
    endif()
    if(_n_se LESS 1)
        r35_fehler("w${_id}" "Aufschlag-Ton ${_se} -> ${_bank} fehlt")
    endif()
    r35_gegner("${_d}/state.log" 2 _hp0 _hpmin _ss1 _bild)
    if(_hpmin GREATER_EQUAL _hp0)
        r35_fehler("w${_id}" "Zombie Platz 2 verliert keine HP (${_hp0} -> ${_hpmin})")
    endif()
    if(NOT "${_ss1}" STREQUAL "${_row}")
        r35_fehler("w${_id}" "Reaktionszeile ${_ss1}, erwartet ${_row}")
    endif()
    message(STATUS "${_tag} [w${_id}]: Runde ${_n_rd}, Ton ${_n_se}, Zombie 2 HP ${_hp0} -> ${_hpmin} (ss1=${_ss1})")
endforeach()

# ---------------------------------------------------------------------------------------------------
# 18 Raketenwerfer
# ---------------------------------------------------------------------------------------------------
set(_d "${_wurzel}/w18")
r35_exe("w18" "${_d}" 18)
r35_zaehle("${_d}/wf.log" "RE2SPAWN a0=020d1000" _n_rk)
r35_zaehle("${_d}/wf.log" "RE2SPAWN a0=030a1a00" _n_r1)
r35_zaehle("${_d}/wf.log" "RE2SPAWN a0=030a1500 a1=0 ofs=\\(300,-900,0\\)" _n_r2)
r35_zaehle("${_d}/wf.log" "SE  re2fx code=0x01140001 -> RE2 ARMS11 Satz 20" _n_ex)
r35_zaehle("${_d}/wf.log" "SE  re2fx code=0x01110001" _n_falsch)
if(_n_rk LESS 1 OR _n_r1 LESS 1 OR _n_r2 LESS 1)
    r35_fehler("w18" "RE2-Spawns fehlen: Rakete ${_n_rk}, Rauch ${_n_r1}/${_n_r2} (@0x800455d8-62c)")
endif()
if(_n_ex LESS 1 OR _n_falsch GREATER 0)
    r35_fehler("w18" "Explosions-Ton: 0x01140001 ${_n_ex} (erwartet >= 1), 0x01110001 ${_n_falsch} (erwartet 0, Sub 13 @0x80020d38)")
endif()
r35_gegner("${_d}/state.log" 2 _hp0 _hpmin _ss1 _bild)
math(EXPR _dmg "${_hp0} - ${_hpmin}")
if(NOT _dmg EQUAL 900)
    r35_fehler("w18" "Raketen-Schaden am Zombie 2 = ${_dmg}, erwartet 900 (RE2-Zeile 17 @0x800A426C)")
endif()
if(NOT "${_ss1}" STREQUAL "17")
    r35_fehler("w18" "Reaktionszeile ${_ss1}, erwartet 17")
endif()
message(STATUS "${_tag} [w18]: Rakete ${_n_rk}, Ton ${_n_ex}, Zombie 2 HP ${_hp0} -> ${_hpmin} (ss1=${_ss1})")

# ---------------------------------------------------------------------------------------------------
# 14 Flammenwerfer
# ---------------------------------------------------------------------------------------------------
set(_d "${_wurzel}/w14")
r35_exe("w14" "${_d}" 14)
r35_zaehle("${_d}/wf.log" "RE2SPAWN a0=031d1200 a1=[-0-9]+ ofs=\\(150,1200,0\\)" _n_fl)
r35_zaehle("${_d}/wf.log" "SE  re2arms ARMS10 satz=0" _n_s0)
r35_zaehle("${_d}/wf.log" "SE  re2arms ARMS10 satz=11" _n_s11)
if(_n_fl LESS 6)
    r35_fehler("w14" "Flammenstrahl-Spawns ${_n_fl} (erwartet >= 6 bei 6 Fuel = 24 Bilder / 3)")
endif()
if(_n_s0 LESS 1 OR _n_s11 LESS 1)
    r35_fehler("w14" "RE2-Toene fehlen: Satz 0 ${_n_s0}, Satz 11 ${_n_s11} (@0x80045534-6c)")
endif()
r35_gegner("${_d}/state.log" 2 _hp0 _hpmin _ss1 _bild)
if(_hpmin GREATER_EQUAL _hp0)
    r35_fehler("w14" "Zombie Platz 2 verliert keine HP durch den Strahl (${_hp0} -> ${_hpmin})")
endif()
if(NOT "${_ss1}" STREQUAL "16")
    r35_fehler("w14" "Reaktionszeile ${_ss1}, erwartet 16 (Op 70 Hitcode 0x20010)")
endif()
# Fuel: mag=1 -> mag=0 im Waffen-Log (2 je 8 Bilder: 6 Fuel = 24 Bilder nach dem Schleifenstart)
file(STRINGS "${_d}/wf.log" _mag0 REGEX "^F[0-9]+ pad=[0-9a-f]+ w=14 .* mag=0 ")
list(LENGTH _mag0 _n_mag0)
if(_n_mag0 LESS 1)
    r35_fehler("w14" "Fuel wird nie leer (RE2 FUN_8006a0cc Id 16: 2 je 8 Bilder)")
endif()
message(STATUS "${_tag} [w14]: Strahl ${_n_fl}, Toene ${_n_s0}/${_n_s11}, Zombie 2 HP ${_hp0} -> ${_hpmin} (ss1=${_ss1})")

# ---------------------------------------------------------------------------------------------------
# 20 Colt Python
# ---------------------------------------------------------------------------------------------------
set(_d "${_wurzel}/w20")
r35_exe("w20" "${_d}" 20)
r35_zaehle("${_d}/wf.log" "SPAWN id=2 sub=0 scale=0xe00" _n_mz)
r35_zaehle("${_d}/wf.log" "SPAWN id=3 sub=0 scale=0x1000" _n_ra)
r35_zaehle("${_d}/wf.log" "SPAWN id=4 sub=0" _n_hu)
r35_zaehle("${_d}/wf.log" "SE  arms_rec=0 bank=20" _n_kn)
if(_n_mz LESS 1 OR _n_ra LESS 1)
    r35_fehler("w20" "Revolver-Entladung fehlt: Muendung 0x02000E00 ${_n_mz}, Rauch 0x03001000 ${_n_ra} (0x800339A4)")
endif()
if(_n_hu GREATER 0)
    r35_fehler("w20" "Huelse gespawnt (${_n_hu}) — der Revolver-Handler 0x800339A4 hat keine")
endif()
if(_n_kn LESS 1)
    r35_fehler("w20" "Knall ARMS14 Satz 0 fehlt")
endif()
r35_gegner("${_d}/state.log" 2 _hp0 _hpmin _ss1 _bild)
math(EXPR _dmg "${_hp0} - ${_hpmin}")
if(NOT _dmg EQUAL 900)
    r35_fehler("w20" "Python-Schaden am Zombie 2 = ${_dmg}, erwartet 900 (Spalte 7 -> RE2-Zeile 5 @0x800A417C)")
endif()
if(NOT "${_ss1}" STREQUAL "5")
    r35_fehler("w20" "Reaktionszeile ${_ss1}, erwartet 5 (Magnum)")
endif()
message(STATUS "${_tag} [w20]: Muendung ${_n_mz}, Rauch ${_n_ra}, Knall ${_n_kn}, Zombie 2 HP ${_hp0} -> ${_hpmin} (ss1=${_ss1})")

message(STATUS "${_tag}: alle sechs Waffen gemessen — OK")
