/*
 * RE1.5 Rebuilt — Runde 35 Spur J "affen": ROOM11C0 Gorilla-Boss (Typ 0x27, EM027) + Ada-Szene.
 *
 * Dossier mit allen Messungen und Belegen: analysis/befunde_runde35/J_affen.md.
 *
 * Drei Mechanismen, alle mit Original-Adresse:
 *
 * (1) ADA VERSTECKT SICH / KOMMT ZURUECK (Nutzer-Punkt 1). Das Raumskript sub07 (ROOM11C0.RDT @0x1C62)
 *     setzt Ada VOR dem Lauf zum Streifenwagen `Member_set 12 = 2` (+0x82 := 2, @0x1C66), laeuft
 *     `Plc_dest 05 01 (-18214,-7229)` (@0x1C6A) und senkt sie nach der Ankunft mit `Member_set 01 =
 *     20000` (@0x1C74) unter den Boden; sub03 (@0x1B52) hebt sie mit `Member_set 01 = 0` wieder hoch.
 *     Die NPC-Wurzel klemmt jeden NPC mit FUN_8003b0a4 (@0x8011cc58-68: a1 = Box[6], a2 = Maske 4),
 *     und FUN_8003b0a4 vergleicht das Band der Zelle (floor >> 4, `sra v0,v0,28`-Muster @0x8003ba04,
 *     Resolver `lbu v1,130(a3)` @0x8003b228-3c) mit dem +0x82 der Entity. ROOM11C0 hat keine
 *     Band-2-Zelle -> Ada erreicht ihr Ziel im Wagen. Der Port leitete das NPC-Band aus y ab
 *     (band_from_y(0) = 0) und hielt sie an der Band-0-Zelle x <= -17790 fest (gemessen: Stopp bei
 *     (-16859,-6560), 1511 vor dem Ziel, Ankunftsradius 300 nie erreicht). Fix: enemy_ai_common.c
 *     re15_npc_wall_clamp klemmt mit dem +0x82-Band (re15_affen_npc_band).
 *
 * (2) UEBERZAEHLIGE MESHES (Nutzer-Punkt 3, "komisch beweglicher Teil am Oberkoerper"). EM027 hat 18
 *     Knochen (EMR+4) und 22 Meshes (MD1+8 >> 1). Der Binder FUN_8001e56c setzt die Part-Zahl
 *     +0x83 = Mesh-Zahl, FUN_8001e5b0 legt alle 22 Part-Records an (Stride 0xac, word0 = 1 =
 *     gezeichnet): die rel-Position jedes Parts ist EMR[8 + 6*i] (fuer i >= 18 liest das Original
 *     hinter der Knochentabelle, = die Bytes der Kindtabelle), und Parts >= Knochenzahl bekommen als
 *     Elternmatrix &DAT_80072d4c (EXE-Konstante {4096,0,0, 0,4096,0, 0,0,4096, t=0,0,0}) und KEINEN
 *     Eltern-Record. FUN_8001f3bc posiert nur EMR+4 Knochen; FUN_8001e9ec zeichnet Part i mit
 *     View * (Eltern * Lokal).
 *     (2a) PART 18 = BRUST-/HALSSCHALE: der Gorilla-INIT haengt ihn danach an den RUMPF (Part 1):
 *          `lw v0,392(v0)` @0x80117200 (+0x188 = Part-Records), `sw (v0+236),3204(v0)` @0x80117210-14
 *          (rec18.Elternmatrix = &rec1.Matrix), `sw (v0+172),3240(v0)` @0x80117218-1c (rec18.Eltern-
 *          Record = rec1), rel = (0x66, -810, 0) @0x80117220-30, lokale Rotation = RotMatrix der
 *          Null-Winkel @0x80117234-3c. Savestate (orig_scene/r3 s021): Part 18 traegt die Matrix
 *          von Part 1 und t = T1 + R1*(102,-810,0). Das Mesh verschliesst die Halsoeffnung des
 *          Rumpfes; ohne es sieht man von vorn auf die Innenseite des Rueckens (weisse Platte).
 *     (2b) PARTS 19..21 bleiben beim Binder-Standard: WELTFEST bei (75,1,78)/(0,79,1)/(79,1,80)
 *          (Savestate: Elternmatrix 0x80072d4c), unter dem Boden am Raumursprung.
 *     Der Port (main.c) liess alle vier auf der WURZELPOSE reiten (Becken mit Yaw/Scale/Position)
 *     = der mitbewegte Teil. Roh-Scan aller STAGE*.BIN nach `sw rX,0xc84(rY)` / `sw rX,0xb2c(rY)`:
 *     nur der Gorilla-INIT haengt um; fuer 0x29 (19/18) und 0x30 (17/16) gilt der Binder-Standard.
 *
 * (3) SPRUNG ERST NACH DREI TREFFERN (Nutzer-Punkt 6, NUTZER-VORGABE: die Zahl 3). Original: der
 *     Boden-Flinch (HURT-Spur 0, FUN_8011b018) endet IMMER im Vergeltungs-Sprung `+0x4=1, +0x5=7`
 *     (@0x8011b188-98; +0x5=9 nur bei +0x1e3 != 0 @0x8011b1c8-d8, das in STAGE1.BIN nie gesetzt
 *     wird). Port-Form: ein Trefferzaehler (mag_hit_ctr, INIT-geloescht wie die Nachbarbytes
 *     +0x1e0..+0x1e3 @0x8011707c-ac) zaehlt jeden Flinch-Eintritt (@0x8011b064-70, einer je
 *     Treffer: der +0x93-Latch @0x8001300c sperrt weitere Treffer bis zum Exit @0x8011b178); erst
 *     der dritte Flinch verlaesst die Spur in den Sprung (+0x5=7), die ersten zwei in die Jagd
 *     (+0x5=3 = CHASE A[3]/B[3], derselbe Exit-Schreibsatz +0x4=1/+0x6=0/+0x7=0). Luft-/Sturz-Spuren
 *     (1/2) bleiben byte-true.
 *
 * (4) SPIELER-REAKTION AUF DIE GORILLA-TREFFER (Nutzer-Punkt 4, "im Original zielstrebiger und
 *     aggressiver"). Gemessen (Original-Aufnahme r3 gegen Port-Lauf A3, Leon ohne Eingabe): das
 *     Original toetet Leon 39,4 s nach der Freigabe und haelt ihn dabei auf der Stelle; der Port
 *     trug ihn mit jedem Heavy 4915 Einheiten fort und schob ihn mit jedem Biss in dieselbe
 *     Richtung aus der Reichweite.
 *     (4a) KNOCKDOWN-STOPP = FUN_8001c2dc (EXE): Band = -(y/1800) (@0x8001c2e8-2c), Zellwort der
 *          ersten Zelle des Bandes, deren AABB + Radius den Punkt enthaelt (`jal 0x8003b7f0`
 *          @0x8001c340); `andi v0,v1,0x1` @0x8001c37c -> Flag 1; `andi v0,v1,0x2` @0x8001c390 ->
 *          Flag 0; `andi v0,v1,0x600` @0x8001c3b0 -> != 0: `sb s4,0(s2)` @0x8001c3c0 = Flag 1.
 *          Handler [5] (0x8003644c, Sturz vorwaerts): Flag -> +0x8c = 0 (@0x80036594-b0) VOR dem
 *          Vorschub @0x800365b4; Handler [4] (0x800360e8, Sturz rueckwaerts): Flag NACH dem
 *          Vorschub (@0x800361fc) -> Phase 5 = Slam (@0x80036214-30). Radius = Box[6] = 450
 *          (EXE-Daten 0x80073e94). ROOM11C0: alle SCA-Zellen tragen Bit 0x200 (floor 2/3/0x13) ->
 *          im AABB jeder Zelle rutscht Leon nicht (Savestate s035: +0x8c = 0, +0x9e = 1).
 *     (4b) BISS-RICHTUNG: der Biss schreibt cmd 2 und aca59 = a780(Beisser)+2 selbst
 *          (`jal 0x8001a780` @0x80118488, `addiu v0,v0,2` @0x80118494, `sb` @0x8011849c) — der
 *          Port leitete die Richtung aus dem NAECHSTEN Gegner ab.
 */
#ifndef RE15_AFFEN_H
#define RE15_AFFEN_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_rdt.h"

/* NUTZER-VORGABE (AUFTRAG.md Runde 35): "Ich moechte das die Monkeys erst springen, wenn sie 3x
 * getroffen wurden, nicht nach jeden Schuss." */
#define RE15_AFFEN_TREFFER_BIS_SPRUNG   3

/* (1) Band der NPC-Wandklemme = Zustandsbyte +0x82 (FUN_8003b0a4 @0x8003b228-3c). */
int      re15_affen_npc_band(const re15_actor_t *e);

/* (2a) Part, den der INIT an einen Knochen haengt: Gorilla 0x27 Part 18 -> Knochen 1,
 *      rel (102,-810,0) (@0x80117200-3c). Schreibt die Pose (Rotation = Elternknochen,
 *      t = T_eltern + R_eltern * rel) nach *out; Rueckgabe 1 = angehaengt, 0 = kein solcher Part. */
#define RE15_AFFEN_BRUST_PART     18      /* 3204 = 18*0xac + 0x6c @0x80117214 */
#define RE15_AFFEN_BRUST_ELTERN    1      /* v0+236 = 1*0xac + 0x40 @0x80117210 */
#define RE15_AFFEN_BRUST_REL_X   0x66     /* @0x80117220-24 */
#define RE15_AFFEN_BRUST_REL_Y  (-810)    /* @0x80117228-2c */
#define RE15_AFFEN_BRUST_REL_Z     0      /* @0x80117230 */
int      re15_affen_part_attach(uint8_t type, int part, const re15_skel_pose_t *poses,
                                int bone_count, re15_skel_pose_t *out);

/* (2b) Welttransformation eines Parts ohne Knochen (part >= bone_count) nach FUN_8001e5b0:
 *     rot := Identitaet (Q12), trans := EMR[8 + 6*part] (s16 x,y,z), kein Entity-Anteil.
 *     Rueckgabe 1 = angewendet, 0 = Part hat einen Knochen oder die EMR-Bytes fehlen. */
int      re15_affen_surplus_part_world(const re15_emd_skeleton_t *sk, int part,
                                       int32_t rot[9], int32_t trans[3]);

/* (3) Trefferzaehler: Eintritt in den Boden-Flinch zaehlt; der Exit entscheidet 7 (Sprung) / 3 (Jagd). */
void     re15_affen_treffer_zaehlen(re15_actor_t *e);
uint8_t  re15_affen_flinch_exit_sub(re15_actor_t *e);

/* (4a) Stopp-Flag der Knockdown-Sonde FUN_8001c2dc (`sb s4/zero,0(s2)`): 1 = der Punkt liegt
 *      (mit Radius) im AABB einer Zelle seines Bandes, deren Wort Bit 0x1 oder (ohne Bit 0x2)
 *      Bit 0x600 traegt. */
#define RE15_KD_SONDE_RADIUS  450         /* Spieler-Box[6]: EXE-Daten 0x80073e94+6, `lhu a1,6(v0)` @0x80036590 / @0x80036210 */
int      re15_affen_kd_sonde(const re15_rdt_t *rdt, int32_t x, int32_t y, int32_t z, int32_t r);

/* (4b) Flinch-Clip des Bisses aus der Blickrichtung des BEISSERS: a780 = ((Spieler.rot_y -
 *      Gegner.rot_y + 0x400) & 0xfff) < 0x800 (@0x8001a788-a4) -> aca59 = a780+2 -> Handler [2]
 *      Clip 8 (frontal) / [3] Clip 9 (von hinten). */
uint8_t  re15_affen_biss_clip(const re15_actor_t *e, const re15_actor_t *pl);

#endif /* RE15_AFFEN_H */
