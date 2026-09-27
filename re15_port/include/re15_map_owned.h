#ifndef RE15_MAP_OWNED_H
#define RE15_MAP_OWNED_H
/*=========================================================================
 * KARTEN-BESITZ — "habe ich den Plan dieses Blattes gefunden?"
 *
 * VORBILD RE2-RETAIL (RE1.5 hat das System NICHT — Begruendung unten).
 * RE2 fuehrt dafuer die SCD-Flag-Bank 33 = 0x800D4924 (Bank-Zeigertabelle
 * 0x800A78C8, Index 33). Bit-Nummer = Karten-Id 0..0x13 aus dem Karten-Record
 * 0x800AAA3D + id*8, gelesen `lbu a1,-21955(at)` @0x8006E660 direkt vor
 * `addiu a0,a0,18724` (= 0x800D4924) @0x8006E668 und dem Bit-Test
 * `jal 0x80077360` @0x8006E66C. Gesetzt wird das Bit NICHT von der EXE,
 * sondern vom Raum-Skript (SCD-Opcode 0x22 `Set`, Handler 0x800543B4).
 *
 * WARUM RE1.5 HIER NICHT MASSGEBLICH IST: RE1.5 kennt weder ein Karten-Item
 * (vollstaendige Namenstabelle, 102 Eintraege, Offsettabelle @0x800C495C /
 * Blob @0x800C4A28 in DEBUG.BIN — kein Eintrag enthaelt "Map") noch ein
 * Besitz-Gatter im Kartenzeichner: dessen Schleife 0x80047128..0x800471FC
 * (Rueckwaertssprung `bne v0,zero,0x80047128` @0x800471FC) liest ausschliess-
 * lich Rechteckfelder per `lhu` und ruft keinen Bit-Test auf. RE1.5 malt also
 * immer ALLE Rechtecke der Seite. Das Vier-Zustands-System stammt deshalb
 * vollstaendig aus RE2.
 *
 * WAS RE1.5 SEHR WOHL HAT: eine echte FUNDSTELLE. Siehe s_map_funde in
 * re15_map_owned.c — ROOM5030/5031 setzen beim Untersuchen des Wandplans
 * Flag(3,115). Der Besitz wird deshalb NICHT in einer eigenen Bank gefuehrt,
 * sondern aus diesen Spiel-Flags abgeleitet; damit liegt er automatisch im
 * Spielstand (re15_savedata.c: `memcpy(out->flags, g_game.flags, ...)`) und
 * ALTE STAENDE BLEIBEN OHNE VERSIONS-BUMP LADBAR.
 *=======================================================================*/
#include <stdint.h>

/* 1 = der Plan dieses Kartenblattes wurde im Spiel gefunden.
 * Port-Gegenstueck zum Bank-33-Bit-Test @0x8006E66C. */
int re15_map_owned_page(unsigned page);

/* Die ganze Bank als Bitmaske, Bit = Seiten-Id — das Port-Gegenstueck zu
 * RE2s Bank 33 (0x800D4924). Fuer Sonden/Riegel. */
uint32_t re15_map_owned_bits(void);

/* Anzahl der Zeilen in der Fundstellen-Tabelle (Sonden/Riegel). */
int re15_map_fund_count(void);
/* Zeile i: welcher Raum, welches Flag, welches Blatt. 0 = kein solcher Index. */
int re15_map_fund_get(int i, unsigned *room, int *bank, int *bit, unsigned *page);

#endif /* RE15_MAP_OWNED_H */
