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
 *     +0x83 = Mesh-Zahl, FUN_8001e5b0 legt alle 22 Part-Records an: die rel-Position jedes Parts ist
 *     EMR[8 + 6*i] (fuer i >= 18 liest das Original hinter der Knochentabelle, = die Bytes der
 *     Kindtabelle), und Parts >= Knochenzahl bekommen als Elternmatrix &DAT_80072d4c (EXE-Konstante
 *     {4096,0,0, 0,4096,0, 0,0,4096, t=0,0,0}) und KEINEN Eltern-Record. FUN_8001f3bc posiert nur
 *     EMR+4 Knochen; FUN_8001e9ec zeichnet Part i mit View * (Eltern * Lokal) -> die vier Parts
 *     stehen WELTFEST bei (3,72,3)/(75,1,78)/(0,79,1)/(79,1,80), unbeteiligt an Lage, Drehung und
 *     Skalierung des Gorillas. Der Port (main.c) liess sie auf der WURZELPOSE reiten (Becken mit
 *     Yaw/Scale/Position) = der mitbewegte Teil. re15_affen_surplus_part_world liefert die
 *     Original-Welttransformation. Gilt generisch fuer jeden RE1.5-Binder mit mehr Meshes als
 *     Knochen (CDEMD0.EMS: 0x27 22/18, 0x29 19/18, 0x30 17/16).
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
 */
#ifndef RE15_AFFEN_H
#define RE15_AFFEN_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_emd.h"

/* NUTZER-VORGABE (AUFTRAG.md Runde 35): "Ich moechte das die Monkeys erst springen, wenn sie 3x
 * getroffen wurden, nicht nach jeden Schuss." */
#define RE15_AFFEN_TREFFER_BIS_SPRUNG   3

/* (1) Band der NPC-Wandklemme = Zustandsbyte +0x82 (FUN_8003b0a4 @0x8003b228-3c). */
int      re15_affen_npc_band(const re15_actor_t *e);

/* (2) Welttransformation eines Parts ohne Knochen (part >= bone_count) nach FUN_8001e5b0:
 *     rot := Identitaet (Q12), trans := EMR[8 + 6*part] (s16 x,y,z), kein Entity-Anteil.
 *     Rueckgabe 1 = angewendet, 0 = Part hat einen Knochen oder die EMR-Bytes fehlen. */
int      re15_affen_surplus_part_world(const re15_emd_skeleton_t *sk, int part,
                                       int32_t rot[9], int32_t trans[3]);

/* (3) Trefferzaehler: Eintritt in den Boden-Flinch zaehlt; der Exit entscheidet 7 (Sprung) / 3 (Jagd). */
void     re15_affen_treffer_zaehlen(re15_actor_t *e);
uint8_t  re15_affen_flinch_exit_sub(re15_actor_t *e);

#endif /* RE15_AFFEN_H */
