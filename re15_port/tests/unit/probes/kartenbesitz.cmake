# RE2-KARTENSYSTEM: Karten-BESITZ (Runde 27).
# Eigene Datei, damit parallel arbeitende Agenten nicht dieselbe CMakeLists editieren.
#
# RE15_RE2_ST0_TIM zeigt auf RE2s Original-Palette. Der Riegel liest die drei
# Halbwoerter dort selbst nach, statt seine eigenen Konstanten zu bestaetigen;
# fehlt die Datei, meldet er UEBERSPRUNGEN statt falsch-gruen zu sein.
add_executable(test_karte_besitz test_karte_besitz.c)
target_link_libraries(test_karte_besitz PRIVATE re15_engine re15_test_support)
target_include_directories(test_karte_besitz PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_karte_besitz PRIVATE
    RE15_RE2_ST0_TIM="${CMAKE_SOURCE_DIR}/../info/re2leon/COMMON/DATA/ST0.TIM")
add_test(NAME unit_karte_besitz COMMAND test_karte_besitz)
