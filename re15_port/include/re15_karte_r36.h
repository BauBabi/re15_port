/* re15_karte_r36.h — Runde 36 Spur M "karte": Besucht-Latch eines TEILBEREICHS.
 *
 * Nutzer 2026-10-10: "Die kleinen Tuerbereiche von ROOM 1110 werden schon als besucht angezeigt,
 * obwohl ich dort noch garnicht drin war". Ein Rechteck mit Teilbereichen (s_map_teile in
 * engine/src/re15_map_zones.h; ROOM1110 = Evidence Room + zwei kleine Raeume hinter Selbst-Tueren)
 * erbte fuer JEDEN Teil das Zonen-Bit des ganzen Raums: wer den Evidence Room betrat, sah die
 * beiden kleinen Raeume als besucht.
 *
 * VORBILD RE2 (selbst disassembliert, info/re2leon/PSX.EXE): das Besucht-Bit wird beim BETRETEN
 * gesetzt - der Raumaufbau FUN_80049e48 ruft als letzten Schritt `jal 0x8006931c` @0x8004a39c,
 * und FUN_8006931c setzt Bank 9 (0x800d490c, `addiu a0,s0,238` @0x80069390) Bit Stage-Basis+Raum
 * (`jal 0x8007730c` @0x80069394). Der Zeichner FUN_8006e120 prueft es JE KACHEL (`lbu a1,12(s1)`
 * @0x8006e67c / @0x8006e730, Test Bank 9 @0x8006e688 / @0x8006e73c, ohne Karte nicht besuchte
 * Kachel uebersprungen `beq v0,zero,0x8006e768` @0x8006e744). Eine Selbst-Tuer ist in RE2 ein
 * Raumwechsel - die kleinen Raeume waeren dort eigene Raeume mit eigenem Bit.
 * PORT-UEBERTRAGUNG: jeder Teil bekommt ein eigenes Bit, gesetzt, solange der Spieler in seiner
 * Weltbox steht (dieselbe Pruefung, die ihn AKTUELL macht). Ausnahme EINGANG: der Teil, in dem
 * der einzige Tuer-Spawn von AUSSEN liegt, gilt mit dem Zonen-Bit als besucht - niemand kommt
 * anders in die Zone (1110: nur ROOM1100 @0x009BA -> (-2400,-5000) im Evidence Room; 5090: nur
 * ROOM6030 @0x00C92 -> (27200,4900) in Teil 1). Das haelt auch alte Spielstaende richtig, die
 * die Teil-Bits noch nicht kennen.
 *
 * SPEICHER: die Teil-Bits liegen im Etagen-Feld visited_floor[16] (Spielstand v9, 128 Bits) ab
 * Bit RE15_KARTE_R36_TEIL_BASIS; die Etagen-Bits (s_etage_bit, re15_map_zones.c) belegen 0..26
 * und duerfen bis 95 wachsen. Kein neues Speicherformat. */
#ifndef RE15_KARTE_R36_H
#define RE15_KARTE_R36_H

#define RE15_KARTE_R36_TEIL_BASIS 96   /* erstes Teil-Bit in visited_floor[] */

/* Bit des Teils (page, rect, Ausschnitt-Ursprung tx/ty) im Etagen-Feld; -1 = der Teil hat keins
 * (dann gilt wie bisher das Zonen-Bit). *eingang = 1: Eingangsteil (s.o.). */
int re15_karte_r36_teil_bit(int page, int rect, int tx, int ty, int *eingang);

/* Fuer die Riegel: Zeilen der Tabelle. */
int re15_karte_r36_teil_bit_count(void);

#endif
