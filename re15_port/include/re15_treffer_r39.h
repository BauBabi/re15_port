/*
 * re15_treffer_r39.h - Runde 39 Spur D "treffer": Sichtlinie des Spieler-Schusses und der Zielhilfe.
 *
 * Dossier: analysis/befunde_runde39/D_treffer.md (RE-Belege B1-B5, Messungen, Bilder D_belege/zellen).
 *
 * NUTZER (AUFTRAG.md): "Zombies sind ueber den Tisch hinweg nicht treffbar. Das darf so nicht sein" /
 * "Das Auto Aim darf nicht gelten fuer Gegner die der Spieler sonst garnicht sehen koennte, zum Beispiel wenn
 * sie um die Ecke sind und nicht mal ansatzweise in Leons Blickfeld mit Wand davor."
 *
 * (1) SICHTLINIE DES SCHUSSES. RE1.5 (Auslieferung): FUN_80011f50 -> `jal 0x8001b9b4` @0x80012168 auf den
 *     Gewinner; FUN_8001b9b4 zieht je Region 3..0 FUN_8003dcc4(delta, region, 0xf00, 0x300) (`ori a2,zero,0xf00`
 *     @0x8001ba1c, `ori a3,zero,0x300` @0x8001ba24, Schleife `ori s0,zero,0x3` @0x8001ba08 / `addiu s0,s0,-1`
 *     @0x8001ba38). FUN_8003dcc4 nimmt nur Zellen mit Band == Spieler+0x82 und `(w10 & 0xf00) == 0x300`, d.h.
 *     Art-Nibble (floor & 0x0f) == 3, und testet den Strahl 2D (/0x12) gegen beide Zelldiagonalen - keine Hoehe.
 *     Das Art-Nibble ist ein Designer-Attribut je Zelle: Bit 2 (0x400) markiert Schraegen (FUN_8001bafc
 *     FUN_8003dcc4(..,0x400,0x400)), Bit 0+1 (0x300) "sperrt Sicht und Schuss". In ROOM1020 tragen die Pulte Art 2
 *     (durchlaessig), in ROOM1140 ist JEDE Zelle Art 3 - auch der Konferenztisch [6] 12100x6100 @(-4650,-17450)
 *     (unfertiger Beta-Datenstand).
 *     RE2 Retail (Ziel, wo RE1.5 unfertig ist): FUN_80050858(A, B, Flagmaske, Hoehentest) sperrt NUR Zellen,
 *     deren id-Wort die Maske traegt: `lhu v1,8(t1)` @0x800508bc / `and v0,v1,fp` @0x800508c4 /
 *     `beq v0,zero,0x800508b0` @0x800508c8 (naechste Zelle). Der Schuss-Applier FUN_800410CC ruft sie je
 *     Kandidat mit `addiu a2,zero,128` @0x800412e0 / `addiu a3,zero,1` @0x800412ec / `jal` @0x800412f0;
 *     ~16 % der RE2-Zellen tragen 0x80 nicht (Moebel; eigener Dump aller PL0/RDT, Hoehe fast immer 1800).
 *     PORT: der RE1.5-Strahl bleibt (Form byte-true), aber MOEBEL-Zellen sind durchlaessig - die Liste
 *     gen/treffer_r39_moebel.inc ist die RE2-Flag-Praxis auf den RE1.5-Daten, Inhalt je Raum aus den
 *     Hintergrundbildern gemessen (tools/r39_treffer_inseln_bild.py, D_belege/zellen/ROOM*_inseln_*.png).
 *
 * (2) ZIELHILFE. RE1.5 FUN_8003703c @0x8003703c-171a0 waehlt nur nach Abstand (SquareRoot0 @0x800370f8,
 *     Eimer +0x9A<0 @0x80037120 / +0x09&0x80 @0x80037124, versteckt +0x09&0x60 @0x8003710c) - KEINE Sichtlinie.
 *     RE2 FUN_80042144 prueft je Kandidat `jal 0x80050858` @0x800422a8 (a0 = Spieler-Teil +0x1C1 +0x5c, a1 =
 *     (Gegner+0x84, Teil-y, Gegner+0x8C), 0x80, 1) und ueberspringt Verdeckte (`bne v0,zero,0x800422f4`
 *     @0x800422b0); der naechste SICHTBARE gewinnt. Beta -> Retail: der Port prueft dieselbe Sichtlinie wie der
 *     Schuss je Kandidat (re15_treffer_r39_zielhilfe_verdeckt, Haken in re15_player_aim_target).
 */
#ifndef RE15_TREFFER_R39_H
#define RE15_TREFFER_R39_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_rdt.h"

typedef struct {
    uint16_t    room;      /* Raum-Id (Leon xxx0 / Elza xxx1)                      */
    uint8_t     idx;       /* Zellenindex in Partition 0 (nur Doku)                 */
    int16_t     x, z;      /* Zellenursprung                                         */
    uint16_t    w, d;      /* Breite, Tiefe                                          */
    uint8_t     type;      /* Form (RE1.5 type-Byte)                                 */
    uint8_t     band;      /* floor >> 4                                             */
    const char *was;       /* gemessene Bedeutung (Tisch, Bett, ...)                 */
} re15_treffer_r39_moebel_t;

/* 1 = diese Art-3-Zelle des Raums `room` ist ein Moebelstueck (Zuordnung ueber Raum + x/z/w/d). */
int re15_treffer_r39_zelle_moebel(unsigned room, const re15_sca_entry_t *c);

/* FUN_8001b9b4-Zwilling mit Moebel-Ausnahme: 1 = Strahl von `von` (Band = von->floor) nach `nach` kreuzt eine
 * sperrende Art-3-Zelle (Regionen 3..0 @0x8001ba08-38, Zelltest FUN_8003dcc4 = re15_r36_los_zelle_kreuzt). */
int re15_treffer_r39_los_blockiert(const re15_actor_t *von, const re15_actor_t *nach);

/* RE2-FUN_80042144-Kandidatenbedingung (@0x800422a8-b0) auf dem RE1.5-Strahl: 1 = Kandidat `e` ist fuer die
 * Zielhilfe verdeckt (ueberspringen). */
int re15_treffer_r39_zielhilfe_verdeckt(const re15_actor_t *pl, const re15_actor_t *e);

/* Messschiene RE15_TREFFER_LOG: Gewinner der Zielhilfe (Slot, -1 = keiner); Haken am Ende von re15_player_aim_target. */
void re15_treffer_r39_log_ziel(int slot);

/* Die Moebel-Tabelle (fuer den Riegel unit_r39_treffer_moebel). */
const re15_treffer_r39_moebel_t *re15_treffer_r39_moebel_tabelle(int *n);

/* ---- Messung / Gegenprobe (nur Sonden, kein Spielverhalten) ---------------------------------------------- */
int      re15_treffer_r39_los_letzte_zelle(void);   /* Index (g_room_rdt.sca) der sperrenden Zelle, -1 = frei */
void     re15_treffer_r39_moebel_aus(int aus);       /* 1 = Moebel-Liste ignorieren (= alter Stand, Art 3 sperrt alles) */
void     re15_treffer_r39_zielhilfe_aus(int aus);    /* 1 = Zielhilfe ohne Sichtlinie (= alter Stand FUN_8003703c)      */
unsigned re15_treffer_r39_zaehler(int welcher);      /* 0 Zielhilfe verdeckt, 1 Schuss/Strahl gesperrt, 2 Moebel uebersprungen */
void     re15_treffer_r39_zaehler_reset(void);

#endif
