# Riegel: die Charakterwahl fuehrt in Elzas Zweig (Runde 35).
#
# Dossier: analysis/befunde_2026-09-27/elza-zweig.md
# Vorarbeit: elza-original.md (Ermittlung) / elza-portzustand.md (Messung).
#
# Fuenf Haelften, alle gegen ihren Original-Beleg:
#   A  character ist der PLD-INDEX 0/4 (@0x801016a4 sll 2, @0x801024c0 / @0x801024d4),
#      nicht 0/1.
#   B  der &4-Test des Originals (@0x80104008) schlaegt fuer Elza an und fuer Leon
#      nicht — mit Gegenprobe, dass der ALTE Wert 1 auf Leons Seite faellt.
#   C  die Variantenregel @0x800397e4 srl 31 / @0x800397ec addu: Basis unveraendert,
#      niederste Hex-Ziffer = Spieler.
#   D  der Startraum liegt in den AUSGELIEFERTEN Bytes — der Haken liest
#      ROOM1240.RDT@0x0531 (= 0x17 ROOM1170) und ROOM1241.RDT@0x0531 (= 0x03
#      ROOM1031) aus der echten RDT. Faellt die Belegstelle weg, faellt der Haken.
#   E  Tuer-Zielformel bleibt in der Variante, und ein Save/Load-Rundlauf bringt
#      Elza samt angefordertem Modell-Index (work_vars[0x10] = DAT_800B0FF0)
#      zurueck — ohne den letzten Punkt haette der erste Raumwechsel nach dem
#      Laden PL00 nachgeladen (@0x80039770 beq / @0x80039788 jal 0x800314b0).
add_executable(test_elza_zweig test_elza_zweig.c)
target_link_libraries(test_elza_zweig PRIVATE re15_engine re15_test_support)
target_include_directories(test_elza_zweig PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_elza_zweig PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

add_test(NAME unit_elza_zweig COMMAND test_elza_zweig)
set_tests_properties(unit_elza_zweig PROPERTIES TIMEOUT 30)
