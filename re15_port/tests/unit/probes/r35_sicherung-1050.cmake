# Riegel: das herausgenommene Sicherungs-Raetsel in ROOM1050.
#
# Der Haken BAUT das Raetsel nicht — er nagelt fest, was gemessen ist, und sichert,
# dass der Raum bis zu einer belegten Wiederherstellung loesbar bleibt.
# Dossier: analysis/befunde_2026-09-27/sicherung-verdrahtung.md
#
# Zwei Haelften, beide muessen stimmen:
#   (A) HERKUNFTSMARKE auf den AUSGELIEFERTEN Bytes — Cut-Paar 7/8 als "zwei Zustaende
#       eines Blicks" (pri 0x518/0x51C), das fertige Schwesterraetsel ROOM2030 -> ROOM2060
#       (Set(3,108,1) @0x01FF2 / Cut_replace 5,11 + 6,10 @0x016C2/@0x016C5), die verwaiste
#       ROOM1050-Nachricht 2 und die 0 Ausgabe-Records fuer Item 0x40.
#   (B) DURCHSPIELBARKEIT, live gefahren: Raum hochfahren, auf AOT-Slot 7 stellen,
#       Aktion, Ja -> flag(3,121)==1 und SCA-Zelle 19 in allen fuenf Partitionen frei.
#
# GEGENPROBE, dass (B) wirklich misst und nicht nur sich selbst — sie laeuft IM Haken
# mit und ist gemessen, nicht behauptet:
#   * derselbe Aufbau mit flag(3,121)=1 laesst sub00 @0x00C1E den ELSE-Zweig nehmen,
#     Slot 7 wird nicht installiert -> der Haken meldet "Slot 7 bleibt WEG (act=0)".
#     Genau diese Form haette ein Raum, dessen Schalter hinter einer Sicherungs-
#     Bedingung liegt — der Haken sieht sie also.
#   * VORZUSTAND wird mitgeprueft: Zelle 19 ist vor dem Druck in allen fuenf
#     Partitionen SOLIDE (u0=0xFF, floor=3) und danach frei (u0=0, floor=0). Ohne die
#     erste Haelfte waere "frei" wertlos.
#   * Der Gegenprobe-Lauf arbeitet auf einer KOPIE der RDT-Bytes, weil Sca_id_set die
#     SCA im Puffer bleibend umschreibt (op_sca_id_set, scd_vm.c) — sonst haette er den
#     Vorzustand des Hauptlaufs zerstoert.
add_executable(test_room1050_sicherung test_room1050_sicherung.c)
target_link_libraries(test_room1050_sicherung PRIVATE re15_engine re15_test_support)
target_include_directories(test_room1050_sicherung PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(test_room1050_sicherung PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

add_test(NAME unit_room1050_sicherung COMMAND test_room1050_sicherung)
set_tests_properties(unit_room1050_sicherung PROPERTIES TIMEOUT 30)
