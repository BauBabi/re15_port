# Runde 26 — Inventar & Abfragen: Bestaetigungston, Wegwerf-Text, Munition stapeln.
#
# TEIL A  Bestaetigungston beim Aufnehmen-Ja. BELEGTE RE2-Angleichung, keine RE1.5-Zeile:
#         RE1.5 ist an dieser Stelle stumm (kein `jal 0x80045024` im Aufnahme-Teilbaum ab
#         FUN_8001db28; LAB_80021f6c hat genau einen jal, 0x8004ef90 @0x80021fd8).
#         RE2s Ja/Nein-Box FUN_80030844 toent: `andi v0,s1,0x20` @0x80030948 /
#         `bne` @0x8003094c / `lui a0,0x406` @0x80030950 / `jal 0x8005ba28` @0x80030954;
#         Nein `lui a0,0x405` @0x8003093c. Die Freigabe traegt RE2s AUFNAHME-Abfrage
#         (SET-B Idx 0, Flagbyte `FB 80`): `ori a1,zero,0xe400` @0x80071ff8 ->
#         `jal 0x8002fe38` @0x80072000, Bit 0x2000 umgeht Gate A @0x80030910.
#         Satznummern sind RE1.5-eigen: Ja `lui a0,0x406` @0x8004a51c,
#         Nein `lui a0,0x405` @0x8004a660.
#         ⛔ BEWUSST NICHT AUSGEWEITET (je mit Beleg, s. probe_r26_inventar.c):
#         can't-carry-Zweig (RE2 SET-B Idx 1 hat keine Auswahlbox), Wegwerf-Abfrage
#         (RE2 SET-A Idx 9, `FB 00`, a1=0x100 @0x80051834 = stumm) und die allgemeine
#         SCD-Ja/Nein-Abfrage (RE2: 47 von 122 Raum-Boxen toenen — keine Regel).
#
# TEIL B  Wegwerf-Text "item" statt "key" fuer Nicht-Schluessel. ⛔ NUTZER-ENTSCHEIDUNG,
#         NICHT byte-true: der Glyphenlauf "Discard" kommt im ganzen ausgelieferten
#         RE1.5-Baum genau einmal vor (DEBUG.BIN @0x800c50ad), RE2 hat ebenfalls nur eine
#         Wegwerf-Zeile (@0x8009f160) und sie sagt "key". Der Riegel vergleicht gegen die
#         DATEN: Referenz-Walker ueber gen/item_prompt_data.inc (== DEBUG.BIN @0x800c4fc6).
#         Anker: Blob 0xd9..0xdb == "key" (@0x800c509f), Blob 0x48..0x4b == "item"
#         (@0x800c500e). Zensus ueber alle 102 ausgelieferten Namen.
#
# TEIL C  Munition gleicher Sorte stapeln. ⛔ NUTZER-ENTSCHEIDUNG: weder RE1.5 noch RE2
#         stapelt beim Aufnehmen (RE1.5 ein Insert FUN_8004dc4c @0x8004dc4c, ein Aufrufer
#         @0x8001e0c4, `sb s1,0(at)` @0x8004def4 statt Addition; RE2 jal 0x80069adc).
#         Arithmetik byte-true aus der Menue-Zusammenlegung FUN_8004e054 geborgt:
#         Summe @0x8004e13c/@0x8004e14c/@0x8004e160, cap `lbu` @0x8004e338 auf
#         0x80074da8 + Id*12, `sltu` @0x8004e340, Ziel = cap @0x8004e3f4 /
#         Rest = Summe-cap @0x8004e410, sonst Ziel = Summe @0x8004e398-3a8.
#         Das Munitions-Fenster ist das byte-true Id-Fenster 0x15..0x21
#         (`sltiu v0,v0,0x15` @0x80047d54 / `sltiu v0,v0,0x22` @0x80049124) GESCHNITTEN
#         mit "hat einen eigenen Kombinations-Satz" (Feld +4 derselben Tabelle, Leser des
#         Originals @0x8004e9d8): 0x15..0x20 tragen eigene Saetze, NUR 0x21 (Memory Card)
#         zeigt auf den Null-Satz @0x80074c88 und faellt damit datengetrieben heraus.
#         ⛔ DER DUPLIKATIONS-RIEGEL (C1c/C2c/C4a/C5a/C5b/C6): Gesamtmenge VOR und NACH
#         der Aufnahme, ueber alle vier Zweige — voller Stapel, Teilstapel mit Ueberlauf,
#         kein vorhandener Platz, kein freier Platz.
#
# RUECKBAU-MESSUNG am ALTEN Stand (jede Aenderung einzeln zurueckgenommen, gebaut,
# gemessen, wiederhergestellt) — s. das Ergebnisfeld der Runde.
#
# EIGENE Datei (nicht die gemeinsame CMakeLists.txt), s. probes/README.md.
add_executable(probe_r26_inventar
    ${CMAKE_CURRENT_SOURCE_DIR}/probe_r26_inventar.c)
target_link_libraries(probe_r26_inventar PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r26_inventar PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/src)
target_compile_definitions(probe_r26_inventar PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r26_inventar COMMAND probe_r26_inventar)
set_tests_properties(unit_r26_inventar PROPERTIES TIMEOUT 120)
