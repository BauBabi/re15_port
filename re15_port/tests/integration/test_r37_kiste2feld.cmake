cmake_minimum_required(VERSION 3.16)
# =============================================================================
# 2-FELD-SYMBOLE MIT DER ECHTEN EXE (Runde 37 Spur R "kiste2feld", Orchestrator-Auftrag R zu Nachtrag 9 des Nutzers).
# Dossier analysis/befunde_runde37/R_kiste2feld.md.
#
#   Nachtrag 9: "In der Kiste muessen die Icons offenbar so weit wie moeglich rechts liegen, damit sie nicht vom Text
#   ueberdeckt werden." — Auftrag R: die 2-Feld-Symbole (49 px, Waffen) VOLLSTAENDIG wie RE2 zeichnen (RE2: POLY_FT4
#   u 40..120 ueber die Kacheln Id + Id+0x56, @0x800717b4/@0x8007183c/@0x80071848, FUN_80069c40 @0x80069cd8-d5c);
#   dieselbe Kachel-Logik im Inventar-Gitter und im Ausruest-Kasten.
#
# Echter Weg: Spielstand (probe_r37_kiste2feld_karte) vor der Kiste, CONTINUE, Viereck F40 am Kisten-AOT, Viereck F90
# = Box-Seite; RUNTER F100/F115, HOCH F130/F145/F160 (Kiste Ring 0..5 = 0x0e..0x13, 6 = 0x0B, 63 = 0x15). Bilder per
# RE15_FRAMEDUMP (RE15_WINDOW_SCALE=1). probe_r37_kiste2feld_bild vergleicht PIXELGENAU:
#   kiste    F99 (Zeilen = Platz 0..4), F110 (1..5), F125 (2..6), F170 (63, 0..3): alle sechs 2-Feld-Ids + zwei schmale
#            gegen das 80x30-Breitbild bzw. die 40x30-Kachel mit der PSX-GPU-Abtastung; 2-Feld: keine Spalten 40..48
#            gleich Spalte 39 (der alte Fehler);
#   gitter   Inventar-Gitter im Kisten-Schirm (F99, Cursor auf Zelle 0) und im Statusschirm (Taste S in F40, Bild
#            F80): Kopfzellen 0/2/5/8 (jede Spalte des Symbol-Speichers) links = Spalten 0..39, Zelle k+1 = 40..79;
#   ausruest Ausruest-Kasten (MC51 ausgeruestet, Kopf in Zelle 5 = Speicher-Spalte 2) bei x 0x82 / y 0x3a.
# Laeufe: Leon ROOM1150 (Kiste), Elza ROOM1151 (Kiste), Leon ROOM1150 Statusschirm, Leon ROOM1150 Statusschirm mit
# ausgeruesteter MC51.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DRE15_KARTE_TOOL=<probe_r37_kiste2feld_karte> -DRE15_BILD_TOOL=<probe_r37_kiste2feld_bild>
#               -DRE15_ASSETS=<shared_assets/PSX> -DWORKDIR=<dir> -P test_r37_kiste2feld.cmake
# =============================================================================
foreach(_v RE15_PC_EXE RE15_KARTE_TOOL RE15_BILD_TOOL RE15_ASSETS)
    if(NOT ${_v} OR NOT EXISTS "${${_v}}")
        message(FATAL_ERROR "r37_kiste2feld: ${_v} fehlt/existiert nicht: '${${_v}}'")
    endif()
endforeach()
if(NOT WORKDIR)
    message(FATAL_ERROR "r37_kiste2feld: WORKDIR fehlt")
endif()

include("${CMAKE_CURRENT_LIST_DIR}/spiel_lauf.cmake")

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf_id)
set(_basis "${WORKDIR}/lauf_${_lauf_id}")

# ⛔ EIGENER EXE-NAME (Muster test_r34n_b_cursor.cmake): ein Bau in einem anderen Baum darf diese exe nicht beenden.
get_filename_component(_exe_dir "${RE15_PC_EXE}" DIRECTORY)
get_filename_component(_exe_ext "${RE15_PC_EXE}" EXT)
file(GLOB _alte_kopien "${_exe_dir}/re15_pc_r37k2f_t_*")
foreach(_k IN LISTS _alte_kopien)
    file(REMOVE "${_k}")
endforeach()
set(_exe_kopie "${_exe_dir}/re15_pc_r37k2f_t_${_lauf_id}${_exe_ext}")
file(COPY_FILE "${RE15_PC_EXE}" "${_exe_kopie}")
set(RE15_PC_EXE "${_exe_kopie}")

# Ein Lauf: Karte schreiben, exe starten, Bilder pruefen. Weitere Argumente = Pruefungen "modus|arg1|arg2|..."
# (| als Trenner); _ok_soll = Zahl der erwarteten "-> ok"-Zeilen aller Pruefungen.
function(k2f_lauf _name _raum _belegung _pad _dump _exit _ok_soll)
    set(WORKDIR "${_basis}_${_name}")
    file(MAKE_DIRECTORY "${WORKDIR}")
    file(GLOB _alt "${WORKDIR}/f*.ppm")
    if(_alt)
        file(REMOVE ${_alt})
    endif()
    file(REMOVE "${WORKDIR}/re15_card.mcr" "${WORKDIR}/debug.log")
    execute_process(COMMAND "${RE15_KARTE_TOOL}" "re15_card.mcr" "${_raum}" "-25650" "-12100" "0" "${_belegung}"
                    WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                    RESULT_VARIABLE _rvk OUTPUT_VARIABLE _outk)
    if(NOT _rvk EQUAL 0 OR NOT EXISTS "${WORKDIR}/re15_card.mcr")
        message(FATAL_ERROR "r37_kiste2feld[${_name}]: Kartenwerkzeug exit=${_rvk}\n${_outk}")
    endif()
    re15_start_spiel(_rv 300
        RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
        "RE15_PAD_AT=${_pad}"
        "RE15_FRAMEDUMP=${_dump}"
        "RE15_EXIT_AT=${_exit}#${_raum}"
        "${RE15_PC_EXE}")
    if(NOT EXISTS "${WORKDIR}/debug.log")
        message(FATAL_ERROR "r37_kiste2feld[${_name}]: kein debug.log (exit=${_rv}, ${WORKDIR})")
    endif()
    file(READ "${WORKDIR}/debug.log" _dbg)
    foreach(_z "CONTINUE: resumed in room ${_raum}" "EXIT_AT: Bild")
        string(FIND "${_dbg}" "${_z}" _p)
        if(_p LESS 0)
            message(FATAL_ERROR "r37_kiste2feld[${_name}]: '${_z}' fehlt im debug.log (exit=${_rv}, ${WORKDIR})")
        endif()
    endforeach()
    set(_alles "")
    foreach(_pr IN LISTS ARGN)
        string(REPLACE "|" ";" _args "${_pr}")
        execute_process(COMMAND "${RE15_BILD_TOOL}" "${RE15_ASSETS}" ${_args}
                        WORKING_DIRECTORY "${WORKDIR}" TIMEOUT 60
                        RESULT_VARIABLE _rvb OUTPUT_VARIABLE _outb)
        string(APPEND _alles "${_outb}")
        if(NOT _rvb EQUAL 0)
            file(WRITE "${WORKDIR}/k2f_bild.txt" "${_alles}")
            message(FATAL_ERROR "r37_kiste2feld[${_name}]: Bildpruefung '${_pr}' exit=${_rvb}\n${_outb}")
        endif()
    endforeach()
    file(WRITE "${WORKDIR}/k2f_bild.txt" "${_alles}")
    string(REGEX MATCHALL "-> ok" _ok "${_alles}")
    list(LENGTH _ok _nok)
    if(NOT _nok EQUAL _ok_soll)
        message(FATAL_ERROR "r37_kiste2feld[${_name}]: ${_nok}/${_ok_soll} Zeilen/Zellen ok\n${_alles}")
    endif()
    message(STATUS "r37_kiste2feld[${_name}] ROOM${_raum}: ${_nok} Symbole/Haelften pixelgleich")
    file(GLOB _ppm "${WORKDIR}/f*.ppm")
    if(_ppm)
        file(REMOVE ${_ppm})   # 230 KB je Bild; Ergebnis steht in k2f_bild.txt
    endif()
endfunction()

set(_inv "0:0f:6,2:12:4,5:13:30,8:0e:100")

# Kisten-Liste Leon: 4 Bilder x 5 Zeilen (alle sechs 2-Feld-Ids + 0x0B + 0x15) + Gitter im Kisten-Schirm (6 Haelften,
# Zelle 0/1 unter dem Cursor).
k2f_lauf(kiste 1150 alle "40:A,90:A,100:D,115:D,130:U,145:U,160:U" "95-175/1:f" 176 26
    "kiste|f000099.ppm|0e,0f,10,11,12|1,2,3,4,5"
    "kiste|f000110.ppm|0f,10,11,12,13|2,3,4,5,6"
    "kiste|f000125.ppm|10,11,12,13,0b|3,4,5,6,3"
    "kiste|f000170.ppm|15,0e,0f,10,11|100,1,2,3,4"
    "gitter|f000099.ppm|215|26|${_inv}|cursor=0")
# Kisten-Liste Elza (ROOM1151, Kisten-AOT bytegleich, Spur M).
k2f_lauf(elza 1151 alle "40:A,90:A,100:D" "95-112/1:f" 113 16
    "kiste|f000099.ppm|0e,0f,10,11,12|1,2,3,4,5"
    "kiste|f000110.ppm|0f,10,11,12,13|2,3,4,5,6"
    "gitter|f000099.ppm|215|26|${_inv}|cursor=0")
# Statusschirm: alle vier Kopfzellen (8 Haelften).
k2f_lauf(status 1150 alle "40:S" "75-85/1:f" 86 8
    "gitter|f000080.ppm|215|26|${_inv}")
# Statusschirm mit ausgeruesteter MC51 (Kopf Zelle 5 = Speicher-Spalte 2): Ausruest-Kasten (2) + Gitter (8).
k2f_lauf(ausruest 1150 ausr "40:S" "75-85/1:f" 86 10
    "ausruest|f000080.ppm|130|58|13|30"
    "gitter|f000080.ppm|215|26|${_inv}")

file(REMOVE "${_exe_kopie}")
message(STATUS "r37_kiste2feld: OK")
