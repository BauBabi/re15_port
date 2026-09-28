# Runde 31 / H — Hebetisch in Irons' Buero (ROOM1150/1151): Granate LINKS, Sicherung RECHTS,
# Aufnahme erst in der RUHE OBEN von sub04 ([Sleep 30 @0x101A, For-Abfahrt @0x1042)).
#
# Dossier: analysis/befunde_runde31/hebetisch.md, Konstanten include/re15_sicherung.h,
# include/re15_granate.h, include/re15_hebetisch.h.
#
#   unit_r31_hebetisch   Ruhe-Fenster aus den RDT-Bytes (1150/1151, Negativ-Kontrolle 1140);
#                        Sitze gegen Fachboden, geschlossene Kuppel, offene Deckel, Achteck,
#                        gegenseitiges Durchdringen; links/rechts in Cut 4 (Rechnung — gemessen im
#                        Framedump, Dossier §1.3); Aufnahme im ersten Ruhebild, jede Aufnahme bei
#                        y=-1205, 40 Ruhebilder ohne Aufnahme (Sleep 30 + Sleep 10), Yes/Yes
add_executable(probe_r31_hebetisch ${CMAKE_CURRENT_LIST_DIR}/../probe_r31_hebetisch.c)
target_link_libraries(probe_r31_hebetisch PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r31_hebetisch PRIVATE ${CMAKE_SOURCE_DIR}/include)
# sqrtf/hypotf/lroundf aus <math.h>: unter Linux liegt libm nicht implizit dabei
if(NOT WIN32)
    target_link_libraries(probe_r31_hebetisch PRIVATE m)
endif()
add_test(NAME unit_r31_hebetisch COMMAND probe_r31_hebetisch)
set_tests_properties(unit_r31_hebetisch PROPERTIES TIMEOUT 120)
