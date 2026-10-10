/*
 * re15_schuss_r36.h — Runde 36 Spur C: der allgemeine Schusspfad (alle 22 Waffen des
 * Spieler-Resolvers FUN_80011f50).
 *
 * Dossier: analysis/befunde_runde36/C_schuss.md.
 *
 * (1) SICHTLINIE (RE1.5, Auslieferungsstand): FUN_80011f50 prueft NACH der Kandidatenschleife
 *     und VOR der Typ-Pruefung/dem +0x93-Latch die Sichtlinie vom Spieler zum GEWINNER:
 *         80012160: beq s1,zero,0x8001253c      ; kein Gewinner -> 0
 *         80012164: addiu s0,s1,52              ; s0 = Gewinner + 0x34 (Lage)
 *         80012168: jal 0x8001b9b4              ; FUN_8001b9b4(&Lage)
 *         8001216c: addu a0,s0,zero
 *         80012170: bne v0,zero,0x80012540      ; blockiert -> return 0 (kein 2. Kandidat)
 *         80012174: addu v0,zero,zero
 *     FUN_8001b9b4: delta = Ziel - DAT_800ac784(+0x34/+0x38/+0x3c) @0x8001b9d8-ba10, dann fuer die
 *     Regionen 3..0 `ori a2,zero,0xf00` / `jal 0x8003dcc4` / `ori a3,zero,0x300` @0x8001ba1c-24,
 *     ODER-verknuepft @0x8001ba28. DAT_800ac784 ist beim Spielerschuss der Spieler (FUN_80031c44).
 *     FUN_8003dcc4 filtert Zellen auf Band == Spieler+0x82 (`lbu v1,130(cur)` @0x8003de60, `sra
 *     v0,v0,28` @0x8003de68) und Klasse 3 (`and 0xf00` / `bne 0x300` @0x8003de7c-94) und testet den
 *     Strahl (alle Koordinaten /0x12) nur in x/z gegen beide Zelldiagonalen — keine Hoehe. Der
 *     Konferenztisch ROOM1140 (SCA-Zelle 6, `01 ff 00 03` @RDT 0x5d0) sperrt deshalb jede Waffe,
 *     auch das Messer (die Tabelle @0x8006e548 schickt w1/w2 durch dieselbe Funktion).
 *     Port: re15_los_ray_blocked (enemy_ai_common.c, byte-true FUN_8003dcc4) mit dem SPIELER als
 *     Strahl-Ursprung (sein `floor` = +0x82) und dem Gewinner als Ziel, Regionen 3..0.
 *
 * (2) FRESSER-SPERRE (RE2 Retail, weil RE1.5 die Fresser-Zustandsmaschine nicht fertig hat):
 *     FUN_800410CC (alle Schusswaffen, `jal 0x800410CC` @0x80043AFC) wirft in der Kandidaten-
 *     schleife `lbu v0,467(s2)` / `bne v0,zero,0x80041300` @0x80041270-78 (+0x1D3 != 0) und
 *     `lhu v0,270(s2)` / `andi v0,v0,0xc000` / `bne` @0x80041290-9c (+0x10E) heraus; EXEC[8] P0
 *     setzt +0x1D3 |= 0x80 @0x80103c04-14 (EMZ0.BIN), solange der Zombie frisst. Im Port fuehrt
 *     re15_re2z_hit_filter_apply (enemy_ai_re2_zombie.c) diese Gates ueber +0x93 Bit 0; die
 *     v0.3.5-Ausnahme fuer die Spawn-Pose gilt seit Runde 36 nicht mehr fuer EXEC[8] (Nutzer:
 *     "Im Original geht beides nicht -> angehen").
 */
#ifndef RE15_SCHUSS_R36_H
#define RE15_SCHUSS_R36_H

#include <stdint.h>
#include "re15_actor.h"

/* FUN_8003dcc4-Zwilling (enemy_ai_common.c): Strahl von `e` (Ursprung, Band = e->floor) nach
 * `ziel` gegen die Klasse-3-Zellen der SCA-Region `step` (0..3); re2_maske 0 = ungefiltert.
 * Runde 36 Spur C: fuer den Spieler-Schuss exportiert (vorher static). */
int re15_los_ray_blocked(const re15_actor_t *e, const re15_actor_t *ziel, int step,
                         unsigned re2_maske);

/* FUN_8001b9b4-Zwilling fuer den Spieler-Schuss: 1 = Sichtlinie Spieler -> Ziel durch eine
 * Klasse-3-Zelle im Band des Spielers blockiert (Regionen 3..0, @0x8001ba1c-28), sonst 0.
 * Ohne geladene SCA (g_room_rdt_ok == 0) immer 0 — wie `sltu fp,v0` @0x8003dd94 bei leerer Region. */
int re15_schuss_r36_sichtlinie_blockiert(const re15_actor_t *pl, const re15_actor_t *ziel);

/* Mess-Zaehler (nur fuer Sonden): wie oft die Sichtlinie seit dem letzten Reset einen Gewinner
 * verworfen hat (`bne v0,zero,0x80012540` @0x80012170). */
int  re15_schuss_r36_sichtlinie_verworfen(void);
void re15_schuss_r36_sichtlinie_reset(void);

#endif /* RE15_SCHUSS_R36_H */
