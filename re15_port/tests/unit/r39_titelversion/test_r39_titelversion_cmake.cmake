cmake_minimum_required(VERSION 3.16)
# =============================================================================
# Riegel Runde 39 Spur T "titelversion": Versionsquelle re15_port/cmake/re15_version_r39.cmake.
# Dossier analysis/befunde_runde39/T_titelversion.md. Reihenfolge: -DRE15_VERSION > Umgebung RE15_VERSION >
# git describe --tags --abbrev=0 --match "v[0-9]*" + " dev" > "dev".
# Aufruf: cmake -DMODUL=<re15_version_r39.cmake> -DWORKDIR=<dir> -P <diese Datei>
# Keine Umgebung des Aufrufers erben: RE15_VERSION / GIT_CEILING_DIRECTORIES werden je Fall selbst gesetzt.
# =============================================================================
if(NOT MODUL OR NOT EXISTS "${MODUL}")
    message(FATAL_ERROR "r39_titelversion_cmake: MODUL fehlt: '${MODUL}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "r39_titelversion_cmake: WORKDIR fehlt")
endif()
include("${MODUL}")
unset(ENV{RE15_VERSION})
unset(ENV{GIT_CEILING_DIRECTORIES})

set(_fehler 0)
set(_n 0)
macro(pruef _name _ist _soll)
    math(EXPR _n "${_n} + 1")
    if("${_ist}" STREQUAL "${_soll}")
        message(STATUS "  ok   ${_name}: '${_ist}'")
    else()
        message(STATUS "  FEHL ${_name}: '${_ist}' (Soll '${_soll}')")
        math(EXPR _fehler "${_fehler} + 1")
    endif()
endmacro()

string(RANDOM LENGTH 8 ALPHABET "0123456789abcdef" _lauf)
set(_wd "${WORKDIR}/lauf_${_lauf}")
file(REMOVE_RECURSE "${_wd}")
file(MAKE_DIRECTORY "${_wd}/repo" "${_wd}/leer")

# 1) -DRE15_VERSION
set(RE15_VERSION "v9.9.9")
re15_version_r39_ermitteln(_v)
pruef("-DRE15_VERSION=v9.9.9" "${_v}" "v9.9.9")
pruef("Quelle" "${_v_QUELLE}" "CMake-Variable RE15_VERSION")

# 2) Umgebung (Release-Container)
unset(RE15_VERSION)
set(ENV{RE15_VERSION} "v8.8.8")
re15_version_r39_ermitteln(_v)
pruef("Umgebung RE15_VERSION=v8.8.8" "${_v}" "v8.8.8")

# 3) -D schlaegt die Umgebung; leeres -D zaehlt nicht
set(RE15_VERSION "v9.9.9")
re15_version_r39_ermitteln(_v)
pruef("-D v9.9.9 gegen Umgebung v8.8.8" "${_v}" "v9.9.9")
set(RE15_VERSION "")
re15_version_r39_ermitteln(_v)
pruef("leeres -D, Umgebung v8.8.8" "${_v}" "v8.8.8")
unset(RE15_VERSION)
unset(ENV{RE15_VERSION})

# 4) git: letzter Tag vX.Y.Z + " dev" (ein spaeterer Fremd-Tag zaehlt nicht)
find_package(Git QUIET)
if(NOT GIT_EXECUTABLE)
    message(FATAL_ERROR "r39_titelversion_cmake: git fehlt")
endif()
set(_g "${GIT_EXECUTABLE}" -c user.name=r39t -c user.email=r39t@localhost -c commit.gpgsign=false -c tag.gpgsign=false)
foreach(_cmd "init;-q" "commit;-q;--allow-empty;-m;eins" "tag;v1.2.3" "commit;-q;--allow-empty;-m;zwei" "tag;fremd-tag")
    execute_process(COMMAND ${_g} ${_cmd} WORKING_DIRECTORY "${_wd}/repo" RESULT_VARIABLE _rc OUTPUT_QUIET ERROR_QUIET)
    if(NOT _rc EQUAL 0)
        message(FATAL_ERROR "r39_titelversion_cmake: git ${_cmd} rc=${_rc}")
    endif()
endforeach()
set(RE15_VERSION_R39_GITDIR "${_wd}/repo")
re15_version_r39_ermitteln(_v)
pruef("git-Baum mit v1.2.3 (danach fremd-tag)" "${_v}" "v1.2.3 dev")
execute_process(COMMAND ${_g} tag v1.2.4 WORKING_DIRECTORY "${_wd}/repo" OUTPUT_QUIET ERROR_QUIET)
re15_version_r39_ermitteln(_v)
pruef("neuer Tag v1.2.4 auf HEAD" "${_v}" "v1.2.4 dev")

# 5) weder Angabe noch git -> "dev"
set(ENV{GIT_CEILING_DIRECTORIES} "${_wd}")
set(RE15_VERSION_R39_GITDIR "${_wd}/leer")
re15_version_r39_ermitteln(_v)
pruef("ohne Angabe und ohne git" "${_v}" "dev")
unset(ENV{GIT_CEILING_DIRECTORIES})

file(REMOVE_RECURSE "${_wd}")
math(EXPR _gut "${_n} - ${_fehler}")
message(STATUS "== r39_titelversion cmake: ${_gut}/${_n} Pruefungen gruen ==")
if(NOT _fehler EQUAL 0)
    message(FATAL_ERROR "r39_titelversion_cmake: ${_fehler} Pruefung(en) rot")
endif()
