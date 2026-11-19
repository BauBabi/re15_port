# Runde 42 Strang "namen", Stufe 6 - ERSATZ-WURZEL fuer Durchlaeufe mit einer "neuen" Aufnahme, ohne synchro/ anzufassen.
#
# Seit Stufe 6 bleibt eine veraltete Aufnahme (alter Satz unter neuem Untertitel, Abdruck des Tags v0.8.30) stumm
# (include/re15_altaufnahme_r42.h). Ein Durchlauf, der eine dieser Zeilen SPRECHEN hoeren muss, legt die exe-Kopie in eine eigene
# Wurzel und daneben synchro/<Pfad> mit einer ersetzten Datei: gleicher Ton, anderer Abdruck (leerer RIFF-Abschnitt angehaengt,
# tests/unit/werkzeug_r42_altaufnahme_ersatz.c). Der Basis-Wurzelsucher (platform/pc/src/asset_root_pc.c rr_init) nimmt das
# exe-Verzeichnis zuerst; alle uebrigen Dateien kommen wie immer aus dem Repo.
#
#   re15_altaufnahme_ersatzwurzel(<out_exe> <exe> <wurzel> <werkzeug> <synchro> <Pfad relativ zu synchro/>...)
#     <out_exe> = Pfad der exe-Kopie in <wurzel> (der Aufrufer loescht sie am Ende wie bisher seine Kopie).
function(re15_altaufnahme_ersatzwurzel _out _exe _wurzel _werkzeug _synchro)
    if(NOT _werkzeug OR NOT EXISTS "${_werkzeug}")
        message(FATAL_ERROR "altaufnahme_ersatz: Werkzeug fehlt ('${_werkzeug}') - Aufruf mit -DRE15_ALT_WERKZEUG=<werkzeug_r42_altaufnahme_ersatz>")
    endif()
    if(NOT _synchro OR NOT IS_DIRECTORY "${_synchro}")
        message(FATAL_ERROR "altaufnahme_ersatz: synchro fehlt ('${_synchro}') - Aufruf mit -DRE15_SYNCHRO=<Repo>/synchro")
    endif()
    file(MAKE_DIRECTORY "${_wurzel}")
    get_filename_component(_name "${_exe}" NAME)
    file(COPY_FILE "${_exe}" "${_wurzel}/${_name}")
    foreach(_rel ${ARGN})
        get_filename_component(_dir "${_wurzel}/synchro/${_rel}" DIRECTORY)
        file(MAKE_DIRECTORY "${_dir}")
        execute_process(COMMAND "${_werkzeug}" anhang "${_synchro}/${_rel}" "${_wurzel}/synchro/${_rel}" RESULT_VARIABLE _rv)
        if(NOT _rv EQUAL 0)
            message(FATAL_ERROR "altaufnahme_ersatz: ${_rel} -> ${_rv}")
        endif()
    endforeach()
    set(${_out} "${_wurzel}/${_name}" PARENT_SCOPE)
endfunction()

# re15_altaufnahme_pruefe(<synchro> <Raum 4 Hex> <Nummer>): Zeile einer der 13 Saetze im debug.log des Laufs pruefen, je nach Stand
# der Datei im Baum (tests/integration/altaufnahmen_r42.cmake re15_altaufnahme_stand):
#   ALT   -> der Lader meldet "ist noch die alte Aufnahme", KEINE Stimme startet (stumm, Textdauer)
#   NEU   -> die neue Aufnahme des Nutzers spielt
#   FEHLT -> "keine Aufnahme" (Untertitel mit Textdauer)
# Benutzt die Makros pruefe / pruefe_re / pruefe_nicht und den Logtext _dbg des Aufrufers.
macro(re15_altaufnahme_pruefe _ra_synchro _ra_raum _ra_nr)
    re15_altaufnahme_stand(_ra_st "${_ra_synchro}" ${_ra_raum} ${_ra_nr})
    if(_ra_st STREQUAL "ALT")
        pruefe_re("Ton: ${_ra_raum} msg ${_ra_nr} traegt noch die alte Aufnahme -> stumm (Strang namen Stufe 6)"
                  "ROOM${_ra_raum} msg ${_ra_nr}: [^\n]*ist noch die alte Aufnahme")
        pruefe_nicht("Ton: ${_ra_raum} msg ${_ra_nr} startet keine Stimme" "[voice] Start ROOM${_ra_raum} Id ${_ra_nr} ")
    elseif(_ra_st STREQUAL "NEU")
        pruefe("Ton: ${_ra_raum} msg ${_ra_nr} hat eine neue Aufnahme -> spielt" "[voice] Start ROOM${_ra_raum} Id ${_ra_nr} ")
    elseif(_ra_st STREQUAL "FEHLT")
        pruefe("Ton: ${_ra_raum} msg ${_ra_nr} ohne Datei -> Untertitel mit Textdauer" "ROOM${_ra_raum} msg ${_ra_nr}: keine Aufnahme")
    else()
        fail("Ton: ${_ra_raum} msg ${_ra_nr} steht nicht in der Liste der veralteten Aufnahmen")
    endif()
endmacro()
