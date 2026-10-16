# --- Runde 38 (2026-10-16), Spur V "releaseskript". Dossier: analysis/befunde_runde38/V_releaseskript.md
#     Nutzer: "eine einfache Möglichkeit - über Skript oder sowas - die Ports für alle Systeme zu bauen - der neuen
#     Version eine neue Versionsnummer - minor - zukommen zu lassen und die alte ins Archiv zu bringen."
#     Bau-Umgebung (release/release_alles.sh, release_alles_lib.sh, docker_bereit.sh, release_alles.cmd), kein
#     Spielverhalten. Die Tests fahren das ECHTE release_alles.sh in einem Wegwerf-Repo; nur die Schritt-Skripte
#     (Bau, Paket, Docker, Python) sind Attrappen (r38_releaseskript/attrappen/), weil ein echter Lauf Stunden dauert.
#
#   unit_r38_releaseskript_version    Versionslogik: Folgeversion (Z+1, bei 99 Uebertrag wie v0.7.99 -> v0.8.0;
#                                     v0.99.99 -> keine automatische), versionCode build.gradle:43-49 streng steigend,
#                                     hoechster Tag numerisch mit Fremd-Tags, Gueltigkeit, Volume-Namen
#   unit_r38_releaseskript_bausteine  SHA256SUMS mit CRLF (frischer Checkout), Archiv erst .unvollstaendig dann fertig,
#                                     vorhandener Ordner bleibt, Frische-Pruefung (altes/leeres/fehlendes Artefakt)
#   unit_r38_releaseskript_trocken    --dry-run: Version + jeder Schritt + Befehle, schreibt NICHTS, baut nichts;
#                                     Versionswahl aus Tags, --version-Grenzen, --tag ohne Notes, --ab, Docker, Baum
#   unit_r38_releaseskript_ablauf     ganzer Lauf: Reihenfolge, Version an Android + Paket, alte Version ins Archiv,
#                                     neue ins Archiv (sha256sum -c), alte APK aus release/, kein Commit ohne --tag;
#                                     --ab abschluss --tag: Commit + annotierter Tag + Push (lokales origin)
#   unit_r38_releaseskript_fehler     simulierte Schritt-Fehler brechen ab (auch durch die tee-Pipe), spaetere
#                                     Schritte laufen nicht; altes Artefakt, fehlende Marke, fehlender Android-Satz,
#                                     Archiv-Konflikt, kaputtes Altarchiv, schmutziger Baum, Sperre, Docker, Platz,
#                                     Wiederaufnahme --ab
#   unit_r38_releaseskript_cmd        (nur Windows) release_alles.cmd: Git-Bash, Argumente + Rueckgabe durchgereicht,
#                                     Doppelklick-Weg mit Antwort N baut nichts
# bash: unter Windows NUR Git-Bash (wie r35_android.cmake - nie System32\bash.exe/WSL); braucht git und sha256sum.
set(_r38v_release "${CMAKE_SOURCE_DIR}/../release")
if(WIN32)
    find_program(R38_RELEASESKRIPT_BASH NAMES bash
                 PATHS "C:/Program Files/Git/bin" "C:/Program Files/Git/usr/bin" "C:/msys64/usr/bin" NO_DEFAULT_PATH)
else()
    find_program(R38_RELEASESKRIPT_BASH NAMES bash)
endif()
if(R38_RELEASESKRIPT_BASH AND EXISTS "${_r38v_release}/release_alles.sh" AND EXISTS "${_r38v_release}/release_alles_lib.sh")
    get_filename_component(_r38v_repo "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
    set(_r38v_teile version bausteine trocken ablauf fehler)
    if(WIN32)
        list(APPEND _r38v_teile cmd)
    endif()
    foreach(_r38v_teil ${_r38v_teile})
        add_test(NAME unit_r38_releaseskript_${_r38v_teil}
                 COMMAND "${R38_RELEASESKRIPT_BASH}"
                         "${CMAKE_CURRENT_SOURCE_DIR}/r38_releaseskript/test_r38_releaseskript.sh"
                         "${_r38v_repo}" "${CMAKE_CURRENT_BINARY_DIR}/r38_releaseskript_wd_${_r38v_teil}" ${_r38v_teil})
        set_tests_properties(unit_r38_releaseskript_${_r38v_teil} PROPERTIES TIMEOUT 900)
    endforeach()
else()
    message(STATUS "r38_releaseskript: bash oder release/release_alles.sh fehlt - unit_r38_releaseskript_* nicht registriert")
endif()
