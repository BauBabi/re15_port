# Runde 42 Strang "namen", Stufe 6: der ECHTE Lader (probe_r42_namen_altlader) aus einer ERSATZ-WURZEL.
# Die Probe liegt als Kopie in ${WURZEL}; daneben ${WURZEL}/synchro/... mit vier ersetzten Dateien. Der Basis-Wurzelsucher
# (platform/pc/src/asset_root_pc.c rr_init) nimmt das exe-Verzeichnis zuerst, alle uebrigen Dateien kommen aus dem Repo.
# synchro/ im Repo wird nur gelesen.
#   6030 main02  = alte Aufnahme + leerer RIFF-Abschnitt (gleicher Ton, 8 Byte laenger)      -> spielt
#   3020 main00  = alte Aufnahme, letztes Bit gekippt (gleiche Groesse)                      -> spielt
#   30E0 main13  = alte Aufnahme + leerer RIFF-Abschnitt                                     -> spielt
#   6030 main22  = eine ANDERE Aufnahme unter diesem Namen (Kopie von room6030/main27.wav)   -> spielt
# Die elf uebrigen Saetze der Liste bleiben stumm, alle anderen Dateien der vier Ordner spielen.
cmake_minimum_required(VERSION 3.21)
foreach(_v PROBE WERKZEUG SYNCHRO WURZEL)
    if(NOT ${_v})
        message(FATAL_ERROR "r42_namen_altlader_ersatz: ${_v} fehlt")
    endif()
endforeach()
file(MAKE_DIRECTORY "${WURZEL}/synchro/STAGE6/room6030" "${WURZEL}/synchro/STAGE3/room3020" "${WURZEL}/synchro/STAGE3/room30E0")
get_filename_component(_name "${PROBE}" NAME)
file(COPY_FILE "${PROBE}" "${WURZEL}/${_name}")
function(_bau _art _rel)
    execute_process(COMMAND "${WERKZEUG}" ${_art} "${SYNCHRO}/${_rel}" "${WURZEL}/synchro/${_rel}" RESULT_VARIABLE _rv)
    if(NOT _rv EQUAL 0)
        message(FATAL_ERROR "r42_namen_altlader_ersatz: Werkzeug ${_art} ${_rel} -> ${_rv}")
    endif()
endfunction()
_bau(anhang "STAGE6/room6030/main02.wav")
_bau(bit    "STAGE3/room3020/main00.wav")
_bau(anhang "STAGE3/room30E0/main13.wav")
file(COPY_FILE "${SYNCHRO}/STAGE6/room6030/main27.wav" "${WURZEL}/synchro/STAGE6/room6030/main22.wav")
execute_process(COMMAND "${WURZEL}/${_name}" ersatz "${WURZEL}" 4 WORKING_DIRECTORY "${WURZEL}" RESULT_VARIABLE _rv)
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "r42_namen_altlader_ersatz: Probe meldet ${_rv}")
endif()
