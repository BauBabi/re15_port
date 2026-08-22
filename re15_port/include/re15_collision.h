/*
 * RE1.5 Rebuilt — player vs room SCA collision (2026-06-04).
 *
 * The room's collision is the SCA block (re15_rdt_t.sca): shaped rectangles
 * defining the walkable space. The player must stay within the union of the
 * walkable entries — this stops the player from "flying" off the rooftop /
 * through the boundary. Mirrors RE1.5 FUN_8003b0a4 / RE2 FUN_8004fba0 (per-
 * shape test + position clamp); this first pass treats every entry as its
 * bounding rectangle (covers type 1 rect; per-shape circle/diagonal/slope
 * refinement is a follow-up) and slides along walls.
 */
#ifndef RE15_COLLISION_H
#define RE15_COLLISION_H

#include <stdint.h>
#include "re15_rdt.h"

/* 1 if (x,z) is inside the walkable SCA geometry (or no SCA data → 1). */
int  re15_collision_on_floor(const re15_rdt_t *rdt, int32_t x, int32_t z);

/* Constrain a move from (old_x,old_z) to (*x,*z): if the target leaves the
 * walkable floor, slide along whichever axis stays on it; if neither does,
 * keep the old position. */
void re15_collision_constrain(const re15_rdt_t *rdt,
                              int32_t old_x, int32_t old_z,
                              int32_t *x, int32_t *z);

/* ENEMY wall clamp (byte-true FUN_8003b0a4 with the enemy's own args): radius = the enemy's box
 * radius (hit_radius_min), solid-mask = entity+0x1d7 (Zombie @0x80100624 / ZGirl @0x8010aac8 lesen
 * das Feld — 4 aufrecht, 8 kriechend; ALLE anderen Aufrufer uebergeben hart 4), band = from
 * enemy_y (+0x82). */
void re15_collision_constrain_enemy(const re15_rdt_t *rdt,
                                    int32_t old_x, int32_t old_z,
                                    int32_t *x, int32_t *z,
                                    int32_t radius, int32_t enemy_y, uint32_t mask);

/* Derselbe Klemmpfad MIT der byte-true Kontakt-Rueckschreibung, die FUN_8003b0a4 nebenbei macht:
 *
 *   contact   -> entity+0x90 : `&= 0xf0` beim Eintritt (@0x8003b1d0-dc),
 *                              `= (FUN_8001bf04(...)>>4) + 8 + (cell.u1 & 3)` je Treffer (@0x8003b4c0-dc)
 *   cell_attr -> entity+0x1b4: `= 0` beim Eintritt (@0x8003b1ec), `= &cell` je Treffer (@0x8003b4ec);
 *                der Port legt direkt `*(u16*)(cell+0x0a)` ab (der einzige gelesene Wert,
 *                Hund @0x8010e29c/@0x8010e670).
 * Beide Zeiger duerfen NULL sein (dann verhaelt sich der Aufruf wie re15_collision_constrain_enemy).
 *
 * Rueckgabewert = der Rueckgabewert von FUN_8003b0a4 (v0): 1, sobald MINDESTENS EINE Zelle die
 * Broadphase bestanden hat (`uVar14 = 1` im Treffer-Zweig, unabhaengig davon, ob der Push die
 * Position bewegt hat), sonst 0. Das ist das +0x1d6 des Maggot-Roots (`sh v0,470(v1)` @0x80116e84). */
int  re15_collision_constrain_contact(const re15_rdt_t *rdt,
                                      int32_t old_x, int32_t old_z,
                                      int32_t *x, int32_t *z,
                                      int32_t radius, int32_t enemy_y, uint32_t mask,
                                      uint8_t *contact, uint16_t *cell_attr);

/* Object (Obj_model_set prop) push-out: keep the player out of every SOLID prop's
 * authored collision box (FUN_8002cabc). Call AFTER re15_collision_constrain in the
 * player-move pass (walls first, then objects). Reads g_scd.props directly. */
void re15_collision_objects(int32_t *x, int32_t *z);

/* FUN_8002cfd4 @0x8002cfd4 — die EINGRENZUNG des Spielers auf den (um sein halbes
 * Eigenmass geschrumpften) XZ-Kasten eines Objekts. Das ist der Mechanismus, der ihn
 * OBEN auf der Kiste haelt: statt der Ausschiebung laeuft er fuer DAS Objekt, auf dem
 * der Spieler steht (@0x8002bef8 bne / @0x8002bf04 jal / @0x8002bf0c j). Der
 * Kletter-FSM ruft ihn zusaetzlich @0x800382dc (Ende Aufstieg) und @0x80038604
 * (Landung auf einem Objekt). Schreibt *x/*z (im Original `sw 0x34/0x3c(a3)`). */
void re15_collision_prop_contain(int prop_idx, int32_t *x, int32_t *z);

/* FUN_8002cabc mit a2 = 1 — der SCHIEBE-Modus: dieselbe Achsen-Wahl wie oben, aber das
 * OBJEKT weicht aus statt des Aktors (@0x8002cd44 `sw v0,52(t0)` / @0x8002cdac `sw v0,60(t0)`;
 * mode 0 schreibt stattdessen 52/60(a3) = den Aktor). Setzt *ox/*oz auf die neue Objektmitte
 * und liefert 1, wenn verschoben wurde (das `addiu s0,s0,1` @0x8002cd4c/@0x8002cdb4, dessen
 * Summe FUN_8002cabc @0x8002cde0 zurueckgibt). prop_idx = Index in g_scd.props. */
int  re15_collision_push_prop(int prop_idx, int32_t actor_x, int32_t actor_z,
                              int32_t *ox, int32_t *oz);

/* FUN_8003b558(obj, 2) — die Gueltigkeitspruefung nach dem Schieben: liegt die (um `r`
 * aufgeblasene) Objekt-Grundflaeche ueber einer SOLIDEN SCA-Zelle des Bandes? Reiner TEST,
 * bewegt nichts. `mask` = das Solid-Klassen-Bit (a1); der Objekt-Aufrufer uebergibt 2.
 * Rueckgabe 1 = geblockt (= Original-Rueckgabe != 0 -> @0x8002bfb8 Ruecksetzung). */
int  re15_collision_box_blocked(const re15_rdt_t *rdt, int32_t x, int32_t z,
                                int band, int32_t r, unsigned mask);

/* Reset the tracked floor band (call on room change so the previous room's band
 * doesn't leak into the new room). */
void re15_collision_reset_band(void);

/* Set the floor band explicitly (doors carry it in the spawn Y). */
void re15_collision_set_band(int band);
/* Convert a spawn Y to a floor band, as the original does: -(Y / 0x708). */
int  re15_collision_band_from_y(int32_t y);
/* Current tracked floor band (diagnostic). */
int  re15_collision_debug_band(void);
/* Set the band from the player Y only if it isn't set yet (initial spawn). */
void re15_collision_ensure_band(int32_t y);

/* Report the lowest and highest floor bands (= floor>>4) present among the
 * room's SCA cells. Used by the stair traversal to pick the descent/ascent
 * target (the original walks bands down/up to the next populated floor —
 * FUN_8001c2dc). Either out-pointer may be NULL. If there is no SCA data both
 * are left untouched / set to the current band. */
void re15_collision_band_range(const re15_rdt_t *rdt, int *min_band, int *max_band);

/* The next SCA band strictly BELOW / ABOVE `cur` (the next floor down/up a stair
 * leads to — the original descends ONE band-level per stair, data2=2). Returns
 * `cur` unchanged when there is no lower/higher band. */
int  re15_collision_next_band_below(const re15_rdt_t *rdt, int cur);
int  re15_collision_next_band_above(const re15_rdt_t *rdt, int cur);

/* Containment floor query (FUN_8003b7f0 / FUN_8001c6e8): the BAND of the SCA
 * floor cell that CONTAINS (x,z), scanning DOWN from `start_band` to 0 (the
 * original returns the first walkable band found below the player). Returns the
 * band, or -1 if (x,z) is over no cell at any band <= start_band. The stair uses
 * this on a forward probe point to detect ARRIVAL on the destination platform
 * (the byte-true end condition: player.Y == floor-probe-ahead). */
int  re15_collision_floor_band_at(const re15_rdt_t *rdt,
                                  int32_t x, int32_t z, int start_band);

/* FUN_8003b7f0 @0x8003b7f0 mit EXPLIZITEM Band und Radius — liefert das ATTRIBUT-WORT
 * der SCA-Zelle unter (x,z), also `*(u16*)(zelle+0x0a)` = `u1 | (floor<<8)`, bzw. 0,
 * wenn keine Zelle des Bandes den Punkt enthaelt. Byte-true:
 *   @0x8003b848-50 Quadrant via FUN_8003b068(vec, tmp, hdr[0], hdr[1])
 *   @0x8003b874-84 [start,end) = hdr[4+q*4] .. hdr[8+q*4], Zellen-Stride 12
 *   @0x8003b8a0-b0 Band-Gate `((u16)zelle[0x0a] << 16 >> 28) == (band & 0xff)`
 *   @0x8003b8b8-f8 `(u32)(x - (zelle.x - r)) < (u32)(zelle.w + 2r)` je Achse
 *   @0x8003b900-04 Treffer -> Rueckgabe = das rohe u16 @zelle+0x0a
 * (`re15_collision_on_floor` ist derselbe Scan mit dem GETRACKTEN Band und
 *  boolescher Rueckgabe; hier braucht der Kletter-Test die Attribut-Bits 0/1/0xe.) */
uint16_t re15_collision_floor_typeword(const re15_rdt_t *rdt, int32_t x, int32_t z,
                                       int band, int32_t r);

/* FUN_8001c6e8 @0x8001c6e8 (Decompilat RE_15_Quellcode_V2/FUN_8001c6e8.c) — die
 * BODEN-HOEHE unter (x,z): scannt die Baender `start_band-1 .. 0` und liefert
 *   -(band+1) * 0x708   fuer die erste Zelle, die (x,z) enthaelt, deren Attributwort
 *                       `(s16)zelle[0x0a] & 0xf002 == band<<12` erfuellt (Band gleich UND
 *                       u1-Bit1 frei) und deren `(s16)zelle[0x08] & mask` gesetzt ist.
 * VOR jedem Band-Durchlauf laeuft (solange `mask & 0x10000` frei ist) der Objekt-Pass:
 * fuer jedes aktive Objekt `FUN_8002da4c(p, obj, (s16)((r - sign(r))>>1), band)` ->
 * Treffer liefert `(s16)(obj.y - 2*box.hy)` = die OBERKANTE des Objekts.
 * Rueckgabe 0, wenn nichts gefunden. */
int16_t re15_collision_room_coll(const re15_rdt_t *rdt, int32_t x, int32_t z,
                                 int32_t r, int start_band, uint32_t mask);

/* FUN_8002da4c @0x8002da4c — Punkt gegen die (um `margin` aufgeblasene) Box eines
 * Obj_model_set-Props, mit Band-Gate. 1 = drin. */
int  re15_collision_prop_box_hit(int prop_idx, int32_t x, int32_t z,
                                 int32_t margin, int band);

/* The centroid (average cell-centre) of all SCA cells of `band` — used by the
 * stair as the destination platform's walkable centre to auto-walk Leon to
 * (mirrors the original deriving the descent target from the stair geometry).
 * For a perimeter-ring platform this is the walkable interior. Returns 1 and
 * fills *cx,*cz on success; 0 if the band has no cells. */
int  re15_collision_band_centroid(const re15_rdt_t *rdt, int band,
                                  int32_t *cx, int32_t *cz);

#endif /* RE15_COLLISION_H */
