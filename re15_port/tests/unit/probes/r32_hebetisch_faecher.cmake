# Runde 32 / H — Hebetisch in Irons' Buero (ROOM1150/1151): Granate und Sicherung liegen in den
# UNTEREN Faechern von Prop 0 (Granate links = Fach A z 96..861, Sicherung rechts = Fach B
# z 950..1715), nicht mehr oben in der Kuppel.
#
# Dossier: analysis/befunde_runde32/hebetisch_faecher.md, Konstanten include/re15_granate.h,
# include/re15_sicherung.h (PORT-WAHL, Herleitung aus den MD1-Bytes von Prop 0).
#
#   unit_r32_hebetisch_faecher   Faecher aus den RDT-Bytes (1150/1151); Sitz im Pool; beide liegen
#                                auf ihrem Fachboden, ganz im Fach, kein Durchstoss, Fach sonst leer;
#                                Sichtlinien von Cut 4 durch die eigene Oeffnung (Ruhe oben und Hub);
#                                Schirm: Granate links, Sicherung rechts der Trennwand (Rechnung —
#                                gemessen im Framedump, Dossier §4)
add_executable(probe_r32_hebetisch_faecher ${CMAKE_CURRENT_LIST_DIR}/../probe_r32_hebetisch_faecher.c)
target_link_libraries(probe_r32_hebetisch_faecher PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r32_hebetisch_faecher PRIVATE ${CMAKE_SOURCE_DIR}/include)
# fabsf aus <math.h>: unter Linux liegt libm nicht implizit dabei
if(NOT WIN32)
    target_link_libraries(probe_r32_hebetisch_faecher PRIVATE m)
endif()
add_test(NAME unit_r32_hebetisch_faecher COMMAND probe_r32_hebetisch_faecher)
set_tests_properties(unit_r32_hebetisch_faecher PROPERTIES TIMEOUT 120)
