# Runde 30 / Thema H — MESS-VARIANTE des Spiels fuer die Sicherung im Hebetisch.
#
# Dossier: analysis/befunde_runde30/sicherung.md
#
# ⛔ STANDARD = AUS. Ohne -DRE15_R30_SICHERUNG_VARIANTE=ON legt diese Datei NICHTS an — weder
# ein Ziel noch einen Test. Sie kostet die uebrigen Build-Verzeichnisse (re15_port/build und die
# der parallel arbeitenden Agenten) also keine Sekunde und kann dort nichts brechen.
#
# Mit dem Schalter entsteht `re15_pc_r30_sicherung`: dasselbe Spiel wie `re15_pc` (dieselben
# Quellen, Definitionen, Include-Pfade, Link-Optionen — alles vom Ziel re15_pc ABGELESEN, nicht
# abgeschrieben), nur dass die vier Symbole aus engine/src/sicherung_1150.c durch
# tests/unit/probe_r30_sicherung_variante.c ersetzt sind. Das Objekt steht vor dem statischen
# Archiv re15_engine, das Archivmitglied wird deshalb nicht gezogen (kein Doppelsymbol).
# engine/, platform/ und include/ bleiben unveraendert.
#
#   cmake -S re15_port -B re15_port/build_r30_sicherung ... -DRE15_R30_SICHERUNG_VARIANTE=ON
#   cmake --build re15_port/build_r30_sicherung --target re15_pc_r30_sicherung
option(RE15_R30_SICHERUNG_VARIANTE
       "Runde 30: Mess-Variante des Spiels mit einstellbarem Sitz/Modell der Sicherung" OFF)

if(RE15_R30_SICHERUNG_VARIANTE AND TARGET re15_pc)
    get_target_property(_r30s_src_dir re15_pc SOURCE_DIR)
    get_target_property(_r30s_srcs    re15_pc SOURCES)
    set(_r30s_abs "")
    foreach(_s IN LISTS _r30s_srcs)
        if(IS_ABSOLUTE "${_s}")
            list(APPEND _r30s_abs "${_s}")
        else()
            list(APPEND _r30s_abs "${_r30s_src_dir}/${_s}")
        endif()
    endforeach()

    add_executable(re15_pc_r30_sicherung
        ${_r30s_abs}
        ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_sicherung_variante.c)

    foreach(_p COMPILE_DEFINITIONS INCLUDE_DIRECTORIES LINK_LIBRARIES LINK_OPTIONS COMPILE_OPTIONS)
        get_target_property(_v re15_pc ${_p})
        if(_v)
            set_target_properties(re15_pc_r30_sicherung PROPERTIES ${_p} "${_v}")
        endif()
    endforeach()
    get_target_property(_r30s_win re15_pc WIN32_EXECUTABLE)
    if(_r30s_win)
        set_target_properties(re15_pc_r30_sicherung PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
    # gen/sicherung_prop.inc liegt unter engine/src
    target_include_directories(re15_pc_r30_sicherung PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/engine/src
        ${_r30s_src_dir}/src)
endif()

# Fortsetzungs-Agent: reine MESS-Sonde (druckt den Zeitplan von sub04 in ROOM1150 UND 1151).
# Ebenfalls nur mit dem Schalter, KEIN add_test — die Suite-Zahl bleibt unberuehrt.
if(RE15_R30_SICHERUNG_VARIANTE)
    add_executable(probe_r30_sicherung_fahrt
        ${CMAKE_CURRENT_LIST_DIR}/../probe_r30_sicherung_fahrt.c)
    target_link_libraries(probe_r30_sicherung_fahrt PRIVATE re15_engine re15_test_support)
    target_include_directories(probe_r30_sicherung_fahrt PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()
