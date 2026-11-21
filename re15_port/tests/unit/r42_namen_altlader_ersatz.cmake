# Runde 42 Strang "namen", Stufe 6: der ECHTE Lader (probe_r42_namen_altlader) aus einer ERSATZ-WURZEL.
# Die Probe liegt als Kopie in ${WURZEL}; daneben ${WURZEL}/synchro/... mit vier ersetzten Dateien. Der Basis-Wurzelsucher
# (platform/pc/src/asset_root_pc.c rr_init) nimmt das exe-Verzeichnis zuerst, alle uebrigen Dateien kommen aus dem Repo.
# synchro/ im Repo wird nur gelesen.
#   6030 main02  = alte Aufnahme + leerer RIFF-Abschnitt (gleicher Ton, 8 Byte laenger)      -> spielt
#   3020 main00  = alte Aufnahme, letztes Bit gekippt (gleiche Groesse)                      -> spielt
#   30E0 main13  = alte Aufnahme + leerer RIFF-Abschnitt                                     -> spielt
#   6030 main22  = eine ANDERE Aufnahme unter diesem Namen (Kopie von room6030/main27.wav)   -> spielt
# Die neun uebrigen Saetze der Liste (13, Stufe 7) bleiben stumm, alle anderen Dateien der vier Ordner spielen - auch 3020 main02
# und 6030 main21 (fast woertlich, nicht in der Liste).
# Strang fabrikplan Stufe 2 (20.11.2026, Commit ce5ec405f): im Bestand liegen alle 13 Saetze neu aufgenommen - die vier Ersatz-
# Dateien entstehen aus den NEUEN Aufnahmen, die neun uebrigen spielen ebenfalls (keine traegt mehr den Abdruck des Tags; die
# Probe verlangt das jetzt, Pruefung "Bestand 20.11.2026").
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
# Strang fabrikplan Stufe 2: seit alle 13 Saetze neu aufgenommen sind, traegt keine Datei im Bestand mehr den Abdruck des Tags. Damit
# die Stumm-Regel weiter am echten Lader gemessen wird, legt der Lauf die ALTE Aufnahme von 6031 msg 2 in die Ersatz-Wurzel: der
# Nutzer hat sie nach synchro/unused/STAGE6/room6031/main02.wav gelegt (Commit ce5ec405f; bytegleich mit dem Stand des Tags v0.8.30,
# Abdruck-Pruefung unit_r42_namen_altaufnahme "ARCHIV"). Diese eine Datei muss stumm bleiben.
set(_alt_quelle "${SYNCHRO}/unused/STAGE6/room6031/main02.wav")
if(NOT EXISTS "${_alt_quelle}")
    message(FATAL_ERROR "r42_namen_altlader_ersatz: alte Aufnahme ${_alt_quelle} fehlt - ohne sie misst der Lauf die Stumm-Regel nicht")
endif()
file(MAKE_DIRECTORY "${WURZEL}/synchro/STAGE6/room6031")
file(COPY_FILE "${_alt_quelle}" "${WURZEL}/synchro/STAGE6/room6031/main02.wav")
execute_process(COMMAND "${WURZEL}/${_name}" ersatz "${WURZEL}" 4 1 WORKING_DIRECTORY "${WURZEL}" RESULT_VARIABLE _rv)
if(NOT _rv EQUAL 0)
    message(FATAL_ERROR "r42_namen_altlader_ersatz: Probe meldet ${_rv}")
endif()
