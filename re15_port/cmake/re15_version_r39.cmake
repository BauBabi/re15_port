# =============================================================================
# Runde 39 Spur T "titelversion" - RE15_VERSION fuer das Titelbild.
# Dossier analysis/befunde_runde39/T_titelversion.md. Nutzer (Nachtrag 14): "bei dieser und zukuenftig neuen Versionen
# passe mir bitte die Version im Titelscreen an." -> NUTZER-VORGABE (Bau-Umgebung, kein Original-Verhalten).
#
# Quelle der Version, in dieser Reihenfolge:
#   1. -DRE15_VERSION=vX.Y.Z           (Android: app/build.gradle aus -Pre15Version; Hand-Bau)
#   2. Umgebung RE15_VERSION           (release/build_win_cross.sh, build_linux_deck.sh -> Container -> docker_*_build.sh)
#   3. git describe --tags --abbrev=0 --match "v[0-9]*" + " dev"   (lokaler Bau: ehrlich "vX.Y.Z dev")
#   4. "dev"                           (weder Angabe noch git: Titelbild zeigt "Ver.dev")
# Die Definition gilt NUR fuer engine/src/titelversion_r39.c (eine neue Version uebersetzt eine Datei neu).
# Lokal laeuft Configure erneut, sobald sich die Tags aendern (packed-refs / refs/tags als Configure-Abhaengigkeit).
# =============================================================================
include_guard(GLOBAL)

function(re15_version_r39_ermitteln _out)
    set(_v "")
    set(_q "")
    if(DEFINED RE15_VERSION AND NOT "${RE15_VERSION}" STREQUAL "")
        set(_v "${RE15_VERSION}")
        set(_q "CMake-Variable RE15_VERSION")
    elseif(DEFINED ENV{RE15_VERSION} AND NOT "$ENV{RE15_VERSION}" STREQUAL "")
        set(_v "$ENV{RE15_VERSION}")
        set(_q "Umgebung RE15_VERSION")
    else()
        find_package(Git QUIET)
        if(GIT_EXECUTABLE)
            execute_process(COMMAND "${GIT_EXECUTABLE}" describe --tags --abbrev=0 --match "v[0-9]*"
                            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                            OUTPUT_VARIABLE _tag RESULT_VARIABLE _rc ERROR_QUIET
                            OUTPUT_STRIP_TRAILING_WHITESPACE)
            if(_rc EQUAL 0 AND NOT "${_tag}" STREQUAL "")
                set(_v "${_tag} dev")
                set(_q "git describe (letzter Tag) + dev")
            endif()
        endif()
        if("${_v}" STREQUAL "")
            set(_v "dev")
            set(_q "Rueckfall ohne Angabe und ohne git")
        endif()
    endif()
    set(${_out} "${_v}" PARENT_SCOPE)
    set(${_out}_QUELLE "${_q}" PARENT_SCOPE)
endfunction()

# Configure-Abhaengigkeit auf die Tags (nur lokal mit git; Release-Container haben kein .git)
function(re15_version_r39_tag_abhaengigkeit)
    find_package(Git QUIET)
    if(NOT GIT_EXECUTABLE)
        return()
    endif()
    execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse --git-common-dir
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                    OUTPUT_VARIABLE _gd RESULT_VARIABLE _rc ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT _rc EQUAL 0 OR "${_gd}" STREQUAL "")
        return()
    endif()
    if(NOT IS_ABSOLUTE "${_gd}")
        set(_gd "${CMAKE_SOURCE_DIR}/${_gd}")
    endif()
    foreach(_f "${_gd}/packed-refs" "${_gd}/refs/tags")
        if(EXISTS "${_f}")
            set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_f}")
        endif()
    endforeach()
endfunction()

re15_version_r39_ermitteln(RE15_VERSION_TITEL)
if("${RE15_VERSION_TITEL_QUELLE}" MATCHES "^git")
    re15_version_r39_tag_abhaengigkeit()
endif()
message(STATUS "RE15_VERSION (Titelbild): '${RE15_VERSION_TITEL}' (${RE15_VERSION_TITEL_QUELLE})")
set_source_files_properties("${CMAKE_SOURCE_DIR}/engine/src/titelversion_r39.c"
    DIRECTORY "${CMAKE_SOURCE_DIR}/engine"
    PROPERTIES COMPILE_DEFINITIONS "RE15_VERSION=\"${RE15_VERSION_TITEL}\"")
