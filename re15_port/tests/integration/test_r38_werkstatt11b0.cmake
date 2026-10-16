# =============================================================================
# RUNDE 38 SPUR C "werkstatt11b0" MIT DER ECHTEN EXE (Dossier analysis/befunde_runde38/C_werkstatt11b0.md).
# Die Unit-Riegel unit_r38_werkstatt11b0_* messen Bytes, Takt und Bankwahl in der VM, unit_r38_werkstatt11b0_ton den
# Mischer; dieser Haken prueft dieselbe Szene im Spiel (Debug-Sprung ROOM11B0, Strom an, 11C0 erledigt):
#
#   A  RE15_MSG_LOG/RE15_SE_DEBUG/RE15_ANIM_TRACE/RE15_AUDIO_CAP_SYNC, Ende Bild 2200 in ROOM11B0:
#      P1  "[msgtext] room=11b0 id=20 \"Marvin: But, hold on...\""
#      P2  "[schub-r38] Ergaenzungsbank ROOM1171 snd0 geladen", ACHT "[schub-r38] F<n> ROOM11B0 snd0 Satz 7 leer ... ->
#          ROOM1171 snd0 Satz 7", jeder vom Mischer gekeyt ("[se] Stimme: se=7 layer=0 vag=3 ... -> SE-Stimme 0");
#          Takt F1 F1 F1+25 F1+25 F1+50 F1+50 (Bild 1 von Clip 0x12 @0x80052ac0, Leon + Marvin) + Phase 7 (2 Bilder)
#          und Marvins Pose in F1 = Clip 18 (0x12) Bild 1 auf der Loco-Bank (anim.txt)
#      P3  "[w11b0-r38] F<n7> Aktor 1 Typ 0x40: Schub-Sub Phase 7"; anim.txt Bild n7 = Clip 17 Bild 0 Loco (gehaltene
#          Pose), Bild n7+1 = Clip 1 Bild 0 Bank 1; KEINE Marvin-Zeile mit Clip 17/18 (10/25 Bilder) ohne Loco-Bank
#   B  wie A, aber RE15_W11B0_SCHUB_STUMM=1 (Mess-Haken: Ergaenzung aus = Stand vor Runde 38), Ende Bild 1800:
#      Ton-Mitschnitt A gegen B: bis zum ersten Schiebe-Ton Byte fuer Byte gleich, in den 4 Bildern danach verschieden
#      (Abbildung Spielbild -> Mitschnitt: Bloecke = Dateigroesse/5880, Versatz = Bloecke - Endbild, in A und B gleich)
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_r38_werkstatt11b0.cmake
# =============================================================================
if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "r38_werkstatt11b0: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r38_werkstatt11b0: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# Eigener exe-Name (Muster test_r34n_b_cursor.cmake): ein parallel bauender Baum darf diesen Lauf nicht treffen.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r38c_haken_*.exe")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r38c_haken_${_lauf_id}.exe")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")

set(_ende_a 2200)
set(_ende_b 1800)
set(_gemeinsam RE15_NO_INTRO=1 SDL_AUDIODRIVER=dummy RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=11b0@250 "RE15_SET_FLAG=3:0x43,3:0x82,4:0xf3" RE15_MSG_LOG=1 RE15_SE_DEBUG=1)

# --- A ---------------------------------------------------------------------------------------------------
set(WORKDIR "${_basis}_a")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 500 ${_gemeinsam}
    "RE15_ANIM_TRACE=${WORKDIR}/anim.txt" "RE15_AUDIO_CAP_SYNC=${WORKDIR}/cap.raw" "RE15_EXIT_AT=${_ende_a}#11b0"
    "${_exe_kopie}")
set(_wd_a "${WORKDIR}")
if(NOT EXISTS "${_wd_a}/debug.log")
    message(FATAL_ERROR "r38_werkstatt11b0[A]: kein debug.log (exit=${_rv}, ${_wd_a})")
endif()
file(READ "${_wd_a}/debug.log" _dbg)
string(FIND "${_dbg}" "[flow] EXIT_AT: Bild ${_ende_a} in Raum 11b0" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r38_werkstatt11b0[A]: Lauf endete nicht in Bild ${_ende_a} (exit=${_rv})")
endif()

# P1 -------------------------------------------------------------------------------------------------------
string(FIND "${_dbg}" "[msgtext] room=11b0 id=20 \"Marvin: But, hold on...\"" _p)
if(_p LESS 0)
    string(REGEX MATCH "\\[msgtext\\] room=11b0 id=20 [^\n]*" _ist "${_dbg}")
    message(FATAL_ERROR "r38_werkstatt11b0[P1]: msg 20 nicht \"Marvin: But, hold on...\" (ist: '${_ist}')")
endif()
message(STATUS "r38_werkstatt11b0[P1]: [msgtext] room=11b0 id=20 \"Marvin: But, hold on...\"")

# P2 -------------------------------------------------------------------------------------------------------
string(FIND "${_dbg}" "[schub-r38] Ergaenzungsbank ROOM1171 snd0 geladen" _p)
if(_p LESS 0)
    message(FATAL_ERROR "r38_werkstatt11b0[P2]: Ergaenzungsbank ROOM1171 nicht geladen")
endif()
string(REGEX MATCHALL "\\[schub-r38\\] F[0-9]+ ROOM11B0 snd0 Satz 7 leer [^\n]*-> ROOM1171 snd0 Satz 7[^\n]*\n\\[se\\] Stimme: se=7 layer=0 vag=3 [^\n]*-> SE-Stimme 0"
       _toene "${_dbg}")
list(LENGTH _toene _n)
if(NOT _n EQUAL 8)
    message(FATAL_ERROR "r38_werkstatt11b0[P2]: ${_n} gekeyte Schiebe-Toene statt 8")
endif()
set(_bilder "")
foreach(_t IN LISTS _toene)
    string(REGEX MATCH "\\[schub-r38\\] F([0-9]+) " _m "${_t}")
    list(APPEND _bilder "${CMAKE_MATCH_1}")
endforeach()
list(GET _bilder 0 _f1)
list(GET _bilder 1 _f1b)
list(GET _bilder 2 _f2)
list(GET _bilder 3 _f2b)
list(GET _bilder 4 _f3)
list(GET _bilder 5 _f3b)
list(GET _bilder 6 _f7a)
list(GET _bilder 7 _f7b)
math(EXPR _d12 "${_f2} - ${_f1}")
math(EXPR _d23 "${_f3} - ${_f2}")
math(EXPR _d7 "${_f7b} - ${_f7a}")
if(NOT _f1 EQUAL _f1b OR NOT _f2 EQUAL _f2b OR NOT _f3 EQUAL _f3b OR NOT _d12 EQUAL 25 OR NOT _d23 EQUAL 25
   OR NOT _f7a GREATER _f3 OR _d7 LESS 0 OR _d7 GREATER 2)
    message(FATAL_ERROR "r38_werkstatt11b0[P2]: Takt falsch: ${_bilder}")
endif()
file(STRINGS "${_wd_a}/anim.txt" _z REGEX "^${_f1} 1 64 18 18 25 1 1 25 1 0$")
if(NOT _z)
    file(STRINGS "${_wd_a}/anim.txt" _z REGEX "^${_f1} 1 ")
    message(FATAL_ERROR "r38_werkstatt11b0[P2]: Marvin in F${_f1} nicht bei Clip 0x12 Bild 1 (Loco): '${_z}'")
endif()
message(STATUS "r38_werkstatt11b0[P2]: 8 Schiebe-Toene (ROOM1171 snd0 Satz 7, Stimme 0) in Bild ${_bilder}; F${_f1} Marvin Clip 0x12 Bild 1")

# P3 -------------------------------------------------------------------------------------------------------
string(REGEX MATCH "\\[w11b0-r38\\] F([0-9]+) Aktor 1 Typ 0x40: Schub-Sub Phase 7" _m "${_dbg}")
if(NOT _m)
    message(FATAL_ERROR "r38_werkstatt11b0[P3]: keine Uebergabe Phase 7 fuer Marvin im debug.log")
endif()
set(_f7 "${CMAKE_MATCH_1}")
math(EXPR _f7n "${_f7} + 1")
file(STRINGS "${_wd_a}/anim.txt" _z7 REGEX "^${_f7} 1 64 ")
file(STRINGS "${_wd_a}/anim.txt" _z8 REGEX "^${_f7n} 1 64 ")
if(NOT _z7 STREQUAL "${_f7} 1 64 17 17 10 0 0 10 1 0")
    message(FATAL_ERROR "r38_werkstatt11b0[P3]: Uebergabebild F${_f7} nicht 'Clip 17 Bild 0 Loco': '${_z7}'")
endif()
if(NOT _z8 STREQUAL "${_f7n} 1 64 1 1 16 0 0 16 0 0")
    message(FATAL_ERROR "r38_werkstatt11b0[P3]: Folgebild F${_f7n} nicht 'Bank 1 Clip 1 Bild 0': '${_z8}'")
endif()
file(STRINGS "${_wd_a}/anim.txt" _falsch REGEX "^[0-9]+ 1 64 1[78] 1[78] (10|25) [0-9]+ [0-9]+ [0-9]+ 0 ")
if(_falsch)
    list(GET _falsch 0 _f0)
    message(FATAL_ERROR "r38_werkstatt11b0[P3]: Marvin Clip 0x11/0x12 ohne Loco-Bank (Rueckfall): '${_f0}'")
endif()
message(STATUS "r38_werkstatt11b0[P3]: F${_f7} '${_z7}' (gehaltene Pose), F${_f7n} '${_z8}', kein Rueckfall")

# --- B: Mess-Haken aus = Stand vor Runde 38 -------------------------------------------------------------------
set(WORKDIR "${_basis}_b")
file(MAKE_DIRECTORY "${WORKDIR}")
re15_start_spiel(_rv 400 ${_gemeinsam} RE15_W11B0_SCHUB_STUMM=1
    "RE15_AUDIO_CAP_SYNC=${WORKDIR}/cap.raw" "RE15_EXIT_AT=${_ende_b}#11b0"
    "${_exe_kopie}")
set(_wd_b "${WORKDIR}")
if(NOT EXISTS "${_wd_b}/debug.log" OR NOT EXISTS "${_wd_b}/cap.raw")
    message(FATAL_ERROR "r38_werkstatt11b0[B]: kein debug.log/cap.raw (exit=${_rv}, ${_wd_b})")
endif()
file(READ "${_wd_b}/debug.log" _dbg_b)
string(FIND "${_dbg_b}" "[schub-r38] F" _p)
if(NOT _p LESS 0)
    message(FATAL_ERROR "r38_werkstatt11b0[B]: Mess-Haken RE15_W11B0_SCHUB_STUMM wirkte nicht")
endif()
file(SIZE "${_wd_a}/cap.raw" _sa)
file(SIZE "${_wd_b}/cap.raw" _sb)
math(EXPR _ba "${_sa} / 5880")
math(EXPR _bb "${_sb} / 5880")
math(EXPR _off_a "${_ba} - ${_ende_a}")
math(EXPR _off_b "${_bb} - ${_ende_b}")
if(NOT _off_a EQUAL _off_b)
    message(FATAL_ERROR "r38_werkstatt11b0[B]: Mitschnitte nicht deckungsgleich (Versatz A ${_off_a}, B ${_off_b})")
endif()
# vor dem ersten Ton: 20 Bilder gleich; danach (der Ton wird beim naechsten Pumpen gekeyt): 4 Bilder verschieden
math(EXPR _vor_o "(${_f1} + ${_off_a} - 19) * 5880")
math(EXPR _nach_o "(${_f1} + ${_off_a} + 1) * 5880")
file(READ "${_wd_a}/cap.raw" _va OFFSET ${_vor_o} LIMIT 117600 HEX)
file(READ "${_wd_b}/cap.raw" _vb OFFSET ${_vor_o} LIMIT 117600 HEX)
file(READ "${_wd_a}/cap.raw" _na OFFSET ${_nach_o} LIMIT 23520 HEX)
file(READ "${_wd_b}/cap.raw" _nb OFFSET ${_nach_o} LIMIT 23520 HEX)
string(LENGTH "${_na}" _lna)
if(NOT _va STREQUAL _vb)
    message(FATAL_ERROR "r38_werkstatt11b0[B]: Mitschnitte schon VOR dem ersten Schiebe-Ton verschieden")
endif()
if(_na STREQUAL _nb OR _lna LESS 47040)
    message(FATAL_ERROR "r38_werkstatt11b0[B]: kein Schiebe-Ton im Mitschnitt nach F${_f1} (A == B)")
endif()
message(STATUS "r38_werkstatt11b0[B]: Mitschnitt A (mit Ton) und B (ohne) bis F${_f1} gleich (Versatz ${_off_a} Bloecke), F${_f1}+1..+4 verschieden")
file(REMOVE "${_exe_kopie}")
message(STATUS "r38_werkstatt11b0: P1 Text, P2 Schiebe-Ton (Takt + Mitschnitt), P3 Uebergabebild — in Ordnung")
