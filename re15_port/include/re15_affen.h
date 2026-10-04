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
 *     +0x1e0..+0x1e3 @0x8011707c-ac) zaehlt jeden Eintritt in EINE der drei Treffer-Spuren (Spur 0
 *     Boden-Flinch @0x8011b064-70, Spur 1 Luft-Treffer @0x8011b238-54, Spur 2 Sturz @0x8011b44c-68;
 *     einer je Treffer: der +0x93-Latch @0x8001300c sperrt weitere Treffer bis zum Exit); erst der
 *     dritte Treffer verlaesst seine Spur in den Sprung (+0x5=7), die ersten zwei in die Jagd (+0x5=3 =
 *     CHASE A[3]/B[3], derselbe Exit-Schreibsatz +0x4=1/+0x6=0/+0x7=0). Das Original springt in ALLEN
 *     drei Spuren nach jedem Treffer (Exits @0x8011b188-98 / @0x8011b3bc-ec / @0x8011b6b4-e8, s. (3)
 *     unten bei re15_affen_treffer_zaehlen); der Zaehler gilt seit 30484afa in allen dreien.
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
#include "re15_enemy.h"   /* re15_enemy_bank_t (15) */

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

/* (3) Trefferzaehler: der Eintritt in JEDE der drei Treffer-Spuren zaehlt (Spur 0 Boden-Flinch
 *     @0x8011b064-70, Spur 1 Luft-Treffer 7/8/13/21 @0x8011b238-54, Spur 2 Sturz 9..11/15..18
 *     @0x8011b44c-68 — Schrotflinte/Magnum landen in 1/2); jeder Exit entscheidet 7 (Sprung) / 3 (Jagd).
 *     Original: alle drei Exits schreiben +0x4=1/+0x5=7/+0x6=0/+0x7=0 (Spur 0 @0x8011b188-98, Spur 1
 *     @0x8011b3bc-ec, Spur 2 @0x8011b6b4-e8, selbst disassembliert). NUTZER-VORGABE "nicht nach jedem
 *     Schuss" gilt fuer alle Waffen (Abnahme 0 M3: mit der Schrotflinte 6 Treffer = 6 Spruenge). */
void     re15_affen_treffer_zaehlen(re15_actor_t *e);
uint8_t  re15_affen_sprung_oder_jagd(re15_actor_t *e);   /* Spur 1/2-Exit: 7 beim 3. Treffer, sonst 3 */
uint8_t  re15_affen_flinch_exit_sub(re15_actor_t *e);    /* Spur 0-Exit: 9 bei +0x1e3, sonst wie oben */

/* (4a) Stopp-Flag der Knockdown-Sonde FUN_8001c2dc (`sb s4/zero,0(s2)`): 1 = der Punkt liegt
 *      (mit Radius) im AABB einer Zelle seines Bandes, deren Wort Bit 0x1 oder (ohne Bit 0x2)
 *      Bit 0x600 traegt. */
#define RE15_KD_SONDE_RADIUS  450         /* Spieler-Box[6]: EXE-Daten 0x80073e94+6, `lhu a1,6(v0)` @0x80036590 / @0x80036210 */
int      re15_affen_kd_sonde(const re15_rdt_t *rdt, int32_t x, int32_t y, int32_t z, int32_t r);

/* (4b) Flinch-Clip des Bisses aus der Blickrichtung des BEISSERS: a780 = ((Spieler.rot_y -
 *      Gegner.rot_y + 0x400) & 0xfff) < 0x800 (@0x8001a788-a4) -> aca59 = a780+2 -> Handler [2]
 *      Clip 8 (frontal) / [3] Clip 9 (von hinten). */
uint8_t  re15_affen_biss_clip(const re15_actor_t *e, const re15_actor_t *pl);

/* (2c) Welttransformation eines Parts ohne Knochen fuer den Zeichner (main.c-Haken): wie (2b), aber
 *      NICHT fuer die G5-Kinder 0x36/0x37 (eigene Regel), NICHT fuer RE2-Banken (re2_rig) und NICHT fuer
 *      den vom Gorilla-INIT umgehaengten Part 18 (2a). Rueckgabe 1 = rot/trans ueberschrieben. */
int      re15_affen_teil_weltfest(uint8_t type, const re15_emd_skeleton_t *sk, int part,
                                  int32_t rot[9], int32_t trans[3]);

/* (4c) CROSSFADE +0x8f: anim_set FUN_8001f314 (a3 = 0x200 an jeder Gorilla-Site, z.B. `jal 0x8001f314`
 *      @0x80117d6c) zieht +0x8f je Aufruf um 1 (`lbu v0,143(v1)` @0x8001f5a8, `addiu v0,v0,-1` @0x8001f5b0,
 *      `sb v0,143(v1)` @0x8001f5b4; nur bei +0x8f != 0, `bne v1,zero` @0x8001f540). Der Port baute ihn nie
 *      ab (gemessen affen_fuss.log frac=7 in 849/887 Bildern; Original r3: +0x8f = 0 in 55/66 Proben).
 *      Haken: enemy_ai_common.c re15_maggot_anim (1 Zeile).
 *
 * (4d) FUSS-SPERRE ALS ABFRAGE: re15_maggot_footlock posiert zwei Keyframes nur, um den Locator-Versatz zu
 *      lesen. g_anim_pose_actor zeigt dabei noch auf den zuletzt GEZEICHNETEN Aktor (gemessen Slot 3, frac 7)
 *      -> beide Posen wurden gegen DESSEN Vor-Pose gemischt; Folge +52..-83 statt +8..+125 je Bild, der
 *      Gorilla kroch auf der Stelle. Abfrage = ohne Pose-Aktor und ohne Tween (Muster
 *      re15_enemy_bone_world_pos); der Zustand wird danach wiederhergestellt. Rueckgabe = die von
 *      re15_skel_compute_pose. Mess-Schiene RE15_AFFEN_FUSS=1 -> affen_fuss.log (re15_affen_fuss_log). */
int      re15_affen_pose_abfrage(const re15_emd_skeleton_t *sk, int kf, re15_skel_pose_t *pose);
void     re15_affen_fuss_log(int slot, int clip, int bild, int bone, int kf_n, int kf_p,
                             int32_t dx, int32_t dz, int16_t rot_y);

/* (4e) LEAP-FLUG OHNE 245d8: jeder Pfad der Phase 2 (0x80118b14) springt in den Epilog 0x80118dc4 —
 *      Landung `j 0x80118dc4` @0x80118cc0, Bild != 0x13 `bne ..,0x80118dc4` @0x80118cdc, Finisher-Gates
 *      @0x80118cf8/d0c/d20/d3c, Commit `j 0x80118dc4` @0x80118d6c. `jal 0x800245d8` @0x80118dbc erreichen nur
 *      Phase 0/1 (Anlauf) und Phase 3 (`beq v0,zero,0x80118dbc` @0x80118d84). Port vorher: c1a4 + 245d8(0)
 *      je Flugbild = doppelte Flugweite (Lauf B: 12700 in 40 Bildern). Haken: Zeile entfernt.
 *
 * (4f) SPAWN-WURZELAUFRUF: Sce_em_set ruft die Typ-Wurzel einmal mit geloeschtem Bit 0x20
 *      (`andi v0,v1,0xdf` @0x8004256c, `jalr 0x80072bac[typ]` @0x8004259c, Bit zurueck @0x80042604-08) —
 *      auch fuer den Gorilla: sein INIT (HP 180, Scale 0x1b33 @0x80117148, Flag 0x800, Zustand 1) laeuft im
 *      Spawn-Bild, auch fuer die eingefrorenen Szenen-Records (ROOM11C0 grid 0x30; Original-Savestate
 *      t=6.11: st=1, +0x166=0x1b33, flags 0x801). Haken: enemy_ai_common.c re15_enemy_spawn_root. */

/* (5) PIN-LATCH-ANKER: der Pin-Latch (@0x8011abe8) ruft `jal 0x8001ac38` @0x8011ac18 mit a0 = Spieler.
 *     FUN_8001ac38: Anker des GREIFERS = Lage - rot(off[kf des laufenden Clips/Bildes]) (@0x8001ac6c-ad18),
 *     dann KOPIE an den Spieler (`sh v0,160(s2)` @0x8001ad30, `sh v0,162(s2)` @0x8001ad48). Der Port
 *     kommentierte den Aufruf nur: Leons Anker blieb (0,0) -> Opfer-Platzierung am Raumursprung (Lauf C1
 *     F447). Zusaetzlich: Bezugspunkt der Port-Wandklemme re15_victim_place = Standpunkt beim Zupacken. */
void     re15_affen_pin_anker(re15_actor_t *e, re15_actor_t *pl);

/* (6) GORILLA-WURF = Opfer-Handler 0x8011c118 (Phasentabelle @0x80100404) + SPIELER-SCHWANZ FUN_80031c44
 *     (Nachbesserung 2, A2; Original-GDB jnb2/g_wer.txt: Haltepunkte an jeder Station des Spieler-Ticks).
 *     (6a) P1 (0x8011c228) ruft anim_set (`jal 0x8001f314` @0x8011c23c) und setzt P2 (`sb 2,aca5a` @0x8011c25c),
 *          sobald +0x95 danach 0xb ist (`ori v0,zero,0xb` / `bne` @0x8011c24c-50). P2 (0x8011c268) platziert mit
 *          dem +0x95 VOR seinem anim_set (`sltiu v0,v0,0x25` @0x8011c278, `jal 0x8001ad68` @0x8011c294, anim_set
 *          @0x8011c2b0) -> die Opfer-Bilder 0x0b..0x24 werden platziert (GDB: erste Platzierung mit +0x95 = 0x0b,
 *          letzte mit 0x24). Der Port zaehlt vor dem Platzieren hoch -> Fenster [0x0b, 0x25).
 *     (6b) RUECK-Variante (aca59 != 0, `beq v1,zero` @0x8011c208): P0 setzt +0x95 = 0xc (@0x8011c214-1c) und
 *          springt OHNE anim_set und OHNE Platzierung ans Ende (`j 0x8011c3b8` @0x8011c220); 0xc wird im FOLGE-
 *          Bild platziert. Port: Seed 0x0b (wird vor dem Platzieren auf 0x0c gezaehlt), Fenster ab 0x0b + 1.
 *     (6c) P3 (0x8011c2e8) Clip 0x10 VORWAERTS (a2 = 0 im Delay-Slot @0x8011c318), P5 (0x8011c31c) Clip 0xb
 *          RUECKWAERTS (`ori a2,zero,0x1` @0x8011c348 -> FUN_8001f314 spiegelt den Cursor @0x8001f34c-54), beide aus
 *          der COMMON-Bank (`lw a0,-13608(a0)` = DAT_800acad8 @0x8011c350, `lw a1,-13376(a1)` = DAT_800acbc0
 *          @0x8011c358) wie die Knockdown-Handler; P7 (0x8011c384): aca58 = 1, +0x93 = 0, aca3c &= ~0xc0.
 *     (6d) REIHENFOLGE: der Handler laeuft als Kommando 5 IM Spieler-Tick (`jalr v0` @0x80031cb4), danach
 *          Koerper-Schub (`jal 0x8002b544` @0x80031cbc) und Wandklemme (`jal 0x8003b0a4` @0x80031d70, Bezug =
 *          Spiegel +0x40/+0x44 = Lage am Ende des Vorbilds, @0x8003b4ac). GDB: Bild 0x23 Platzierung
 *          (-4334,-11423) -> Klemme (-4588,-11243); Bild 0x24 (-5402,-10631) -> (-5381,-10551); danach schiebt
 *          die Klemme Leon ~100/Bild aus der Zelle (auch nach der Freigabe). Der Port klemmte die Platzierung
 *          stattdessen auf den "letzten begehbaren Standpunkt" (re15_victim_place, fuer 0x27 seit 2026-08-29)
 *          und rief die Wandklemme VOR der Platzierung. Jetzt fuer den Greifer 0x27: Platzierung ohne
 *          Ersatzklemme, danach re15_player_body_and_walls mit dem Bezug vom Bildanfang. Riegel `wand`:
 *          re15_collision_constrain = FUN_8003b0a4 in 168/168 Original-Bildern bitgleich. */
int      re15_player_victim_gorilla(void);   /* 1 = Opfer-Handler des Gorillas (0x27) besitzt Leon (P0-P6) */
/* (6e) 1 = dieser Gorilla haelt Leon (sub 15 Phase 3/4: Wort-Bit 0x1000 gesetzt @0x8011ac34-38, geloescht
 *      @0x8011ad8c-94) -> das Paar ist vom Koerper-Schub FUN_8002aec4 ausgenommen (`andi 0x1000` @0x8002af14). */
int      re15_affen_griff_paar(const re15_actor_t *e);

/* (7) RNG des Originals: FUN_8001af20 hasht das a0-Register des Aufrufers (Dossier A1). Wo a0 an einer
 *     Ziehstelle nachweislich ein fester Wert ist, zieht der Port mit DIESEM Wert statt der xorshift-Ersatzquelle:
 *     B[4] (Heavy-Anlauf, `jal 0x8001af20` @0x80118164 / @0x8011817c): a0 = Entity+0x34 (der Zeiger, mit dem
 *     A[4] die Zonen-Abfrage FUN_8003b93c ruft) — GDB jnb2/g_rng.txt: 21/21 Ziehungen ra 0x8011816c mit
 *     a0 = 0x800ad048 (e2+0x34), danach a0 = 0xa0e8 (verkettet) -> +0x8c 188 / +0x9e 73 je Bild wie gemessen. */
uint8_t  re15_affen_rng_a0(uint32_t *a0);
uint32_t re15_affen_psx_entity(const re15_actor_t *e);
/* (7b) B[3] CHASE (`jal 0x8001af20` @0x80117ce0 Eintritt / @0x80117d1c je Bild): a0 = Rest aus A[3] (s. affen_11c0.c);
 *      B[0] Leerlauf-Timer (@0x80117594): a0 = Entity-Zeiger (GDB vs9117: 0x800ad014 bei cur = e2). */
uint32_t re15_affen_a804_a0(const re15_actor_t *e, const re15_actor_t *ziel, int32_t r);
uint32_t re15_affen_b3_a0(const re15_actor_t *e, const re15_actor_t *pl);
uint32_t re15_affen_b0_a0(const re15_actor_t *e, const re15_actor_t *pl);   /* +0x1dc != 0 -> Abstand^2, sonst Entity */
/* (7c) B[7] Absprung (`jal 0x8001af20` @0x80118a3c): a0 = g_entity(cur) per `lw a0,-14460(a0)` @0x80118a24. */

/* (8) RITT-PLATZIERUNG DES GREIFERS (Nachbesserung 3, M2). Der Pin-Latch (Phase 2, 0x8011abe8) dreht den Gorilla
 *     nach dem Anker per `jal 0x8001a8f8` (a0 = Spieler+0x34 @0x8011ac60, a1 = 0x800 @0x8011acb0 -> Yaw := Peilung,
 *     FUN_8001a8f8 `slt` @0x8001a974 / `sh a0,106(v1)` @0x8001a984) und faellt OHNE Sprung in Phase 3 (0x8011acb4):
 *     `jal 0x8001ad68` @0x8011accc mit a0 = g_entity (@0x8011acc0), a1 = +0x84, a2 = +0x16c -> der GORILLA SELBST
 *     steht jedes Bild auf Anker + rot(Versatz des laufenden Clip-0x1c-Bildes) (FUN_8001ad68 @0x8001adf4-ae18),
 *     erst danach anim_set (@0x8011ace8). Im Original rueckt e1 so ~800 auf Leon vor und schiebt e2 weg (Abstand
 *     e1-e2 = 3200 in T256-T264, jnb1/g_griff.txt). latch = 1: Phase-2-Bild (Yaw-Fang + Platzierung). */
void     re15_affen_ritt_platz(re15_actor_t *e, const re15_actor_t *pl, int latch);

/* (9) FUSS-SPERRE MIT DER POOL-POSE: anim_set FUN_8001f314 holt das Bildwort zu +0x95 (`lbu v0,149(t0)` @0x8001f344/
 *     @0x8001f35c, `sw a2,360(t0)` @0x8001f36c), FUN_8001f3bc schreibt DESSEN Pose in den Pool +0x188 (`lw s1,392(v1)`
 *     @0x8001f40c, RotMatrix je Record, Stride 172) und zaehlt +0x95 ERST DANACH hoch (`lbu`/`sb v0,149(v1)`
 *     @0x8001f610-1c, Wrap `sb zero,149(v1)` @0x8001f63c). FUN_8011bf50 kettet +0x20 mit genau diesen Pool-Matrizen
 *     (`jal 0x80022da0` @0x8011bf80-c4) und zieht `m.t - rec[84]` ab (@0x8011bfd4-c008) -> die Fusssperre bewegt mit
 *     der Pose des Bildes VOR dem Vorschub. Der Port nahm das Bild danach: am Ende des Brustschlags (Clip 3, Bild 69 ->
 *     Wrap 0) rechnete er kf 74 gegen kf 143 = ~1900 Einheiten Sprung; im Original rechnet bf50 dort Bild 69 gegen 68
 *     (g_griff F370 -> F371: (4,-4)) und das Folgebild verlaesst Sub 2 ohne bf50. Haken: re15_maggot_anim merkt das
 *     Bild vor dem Vorschub, re15_maggot_footlock posiert es (je 1 Zeile). */

/* (10) A/B IM SELBEN TICK (Nachbesserung 4, N2). Der Brain-Rumpf 0x80117254 ruft A[+0x5] (Tabelle 0x801213e8,
 *      `lbu v0,5(v0)` @0x80117324, `jalr` @0x80117344), liest +0x5 NEU (`lbu v0,5(v0)` @0x80117358) und ruft
 *      B[+0x5] (Tabelle 0x80121428, `jalr` @0x80117378) im SELBEN Tick; +0x1dc-- erst danach (@0x801173f8-40c).
 *      Wechselt A[3] auf den Biss, laeuft B[5] (Clip 0x12 + anim_set) sofort. Der Port brach nach dem A-Entscheid
 *      ab und fuehrte B erst im Folgetick aus: +1 Tick je A-Wechsel (2 je Biss-Zyklus). Haken in enemy_ai_common.c:
 *      die A-Wechsel (A[0], A[1], A[3], A[4]) springen nach `ab_b` (enemy_ai_common.c, Hinweis H2 Abnahme 4); dort laeuft der Sub-Schalter ein zweites
 *      Mal NUR mit dem B-Teil (A[3]/A[4]/A[15] werden im B-Lauf uebersprungen; A[2]/A[5..8] sind `jr ra`). */

/* (11) TREFFERPUNKT DER ANGRIFFE = GEZEICHNETE KNOCHENMATRIX (Nachbesserung 4, N2). FUN_8001bff8 (PSX.EXE)
 *      komponiert die Knochenmatrix a0 mit einem Versatz a1 (Identitaet 0x80072d4c, t := a1 @0x8001c058-7c,
 *      `jal 0x80022da0` @0x8001c078) und prueft das Quadrat um den Spieler (@0x8001c080-c0). a0 = Pool-Record
 *      + 0x40 (B[5] `addiu a0,s2,1612` = Record 9 @0x801183c0; B[6]/B[8] 1096/1784 = Records 6/10 @0x801186f4-f8 /
 *      @0x801190e0-e4; Sub 15 924 = Record 5 @0x8011ab68). Record + 0x40 schreibt NUR der Zeichner (FUN_8001e9ec /
 *      FUN_8001ef54 `FUN_80022da0(rec[0x1b], rec+0x18, rec+0x40)`, aus FUN_8001e8c8, Zeichen-Schleife @0x8001d108
 *      NACH den KI-Ticks) -> Pose des im Vortick gezeichneten Bildes an Lage/Yaw vom Ende des Vorticks.
 *      Versatz: B[5] vx := 0x64 (`ori v0,zero,0x64` / `sw v0,16(sp)` @0x80118380-84), alle anderen (0,0,0)
 *      aus 0x80072d60. Merken am Anfang des Gorilla-Ticks (= Zeichenstand: die Spieler-Schiebung bewegt nur den
 *      Spieler, re15_body_push(const pusher, ..., gepusht)); Pose = (Clip, Bild) des letzten anim_set. */
void     re15_affen_zeichen_merk(const re15_actor_t *e);
int      re15_affen_trefferpunkt(const re15_actor_t *e, int bone, int32_t out[3]);

/* (12) POOL-UEBERBLENDUNG IN DER FUSS-SPERRE (Nachbesserung 4, N2; schliesst OFFEN 3/4 von Nachbesserung 3).
 *      FUN_8001f3bc schreibt die Pool-Winkel NICHT als reinen Keyframe: bei +0x8f != 0 mischt es die gespeicherten
 *      Winkel (Record+0x60) mit dem Keyframe (FUN_80020510(r, kf, r, 0x1000 - 0x200*frac), Decompilat Z. 77-88) und die
 *      Wurzel per GPF12/GPL12 (Z. 40-61), +0x8f-- erst danach (Z. 78). FUN_8011bf50/c024 ketten +0x20 mit DIESEN
 *      Pool-Matrizen (@0x8011bf80-c4) und ziehen die Welt-t ab, die der Zeichner im Vortick in Record+0x40 schrieb
 *      (`lw a0,84(s0)` @0x8011bfd8, `lw a0,92(s0)` @0x8011bff8). +0x20 traegt dabei die Yaw des Zeichners (RotMatrix
 *      nur in FUN_8001e8c8) und als t die laufende Lage (+0x34..+0x3c): neue Lage = Zeichenlage - R*S*(jetzt - zeichen).
 *      Der Port posierte ungemischt (Keyframe Bild gegen Bild-1), mit der frisch gesteuerten Yaw und ueberging das
 *      Bild jedes Clip-Wechsels (s_prev_clip) — gemessen: nach dem Biss-Ende (Clip 0x12 -> 5) kriecht e2 im Original
 *      sofort weiter ((-9087) -> (-9142) -> (-9223)), im Port stand er und lief dann rueckwaerts (+9, +19), Ruhelage
 *      40-73 daneben -> Treffer ein Fenster-Bild frueher (Zyklus 103 statt 104). Haken: re15_maggot_anim ruft
 *      re15_affen_pool_anim (VOR Vorschub und Abbau), re15_maggot_footlock zuerst re15_affen_fusssperre. */
void     re15_affen_pool_anim(re15_actor_t *e);
int      re15_affen_fusssperre(re15_actor_t *e, int bone);

/* (13) KETTE AUF DER GTE (Nachbesserung 5, P1). Zeichner (FUN_8001ef54/FUN_8001e9ec), Fuss-Sperre (FUN_8011bf50/c024)
 *      und Trefferpunkt (FUN_8001bff8) rechnen die Knochen-Weltlage als CompMatrix-Kette FUN_80022da0, die ENTITY-MATRIX
 *      +0x20 = RotMatrix(+0x68) * ScaleMatrix(+0x166) als ERSTER Faktor (FUN_8001e8c8), dann Record+0x18 von der Wurzel
 *      abwaerts. Jede Stufe rundet auf der GTE: R' ueber MVMVA 0x4a49e012 (sf=1, IR 16 Bit gesaettigt) @0x80022df0/
 *      @0x80022e38/@0x80022e84, t' = (TR<<12 + R*V0) >> 12 ueber MVMVA 0x4a480012 @0x80022eec, V0 = 16-Bit-Haelften
 *      (`lhu`/`lwc2` @0x80022ecc-e0). Der Port rechnete bis Nachbesserung 4 die Kette im Objektraum (ohne +0x20), bildete
 *      die Differenz und drehte/skalierte zuletzt — je Tick 1-4 Einheiten neben dem Original (GDB jnb5/g_stufe, Fuss-
 *      Sperre F196 e1 (-39,75) gegen Port (-37,75)). Dazu die Pool-Ueberblendung mit zwei getrennten >> 12: Wurzel
 *      `gpf12`/`gpl12` (FUN_8001f3bc Z. 40-61), Winkel FUN_80020510 -> LoadAverageShort12 `gpf12_b`/`gpl12_b`.
 *      re15_affen_kette_test: die Glieder, die affen_kette rechnet (affen_glieder) — Riegel `kette` prueft sie gegen die
 *      fest verdrahteten Ketten von FUN_8011bf50 (@0x8011bf80-c4) und FUN_8011c024 (@0x8011c054-b8) (Nachbesserung 6, M4). */
int      re15_affen_kette_test(int bone, int out[RE15_EMD_MAX_BONES]);

/* (15) GORILLA-FINISHER (Nachbesserung 6, M1). B[8] (Treffer im Sprung, player.hp -= 600 @0x801191a8-ac) schreibt das
 *      Wort aca58 := 6 (@0x801191c4-cc, aca59 = aca5a = 0), acbcc/acbd0 := Gorilla+0x178/+0x17c (Opfer-Bank,
 *      @0x801191e0-9204). Der cmd-6-Hook des Typs 0x27 ist 0x8011c3d4 (Registrierung @0x8011eab8-c8, Dispatch
 *      0x800368c0 `lw -514` @0x8003692c) und verteilt ueber aca59 (@0x8011c3d8) auf die Tabelle 0x80121580 =
 *      {0x8011c414, 0x8011c414}. 0x8011c414:
 *        aca5a 0 (@0x8011c460-d4): aca5a := 1, +0x8f := 7 (acae3), Clip acae8 := 0, Bild acae9 := 0, Blut FUN_80019700(0x2000,
 *                 +0x6a, Part 8, Null-Versatz 0x80121570) @0x8011c4a0, Se_on(0x04030001) = CORE 3 @0x8011c4b8,
 *                 aca3c |= 0xc0; faellt ohne Sprung in den Zweig 1 (dasselbe Bild).
 *        aca5a 1 (@0x8011c4d8-554): bei Bild 0x3c (@0x8011c4e0) FUN_80045630(2,0,0) @0x8011c4f0 + Blut @0x8011c518;
 *                 f314(acbcc, acbd0, 0, 0x200) @0x8011c534; aca5a += Clip-Ende (@0x8011c548-50).
 *        aca5a 2 (@0x8011c55c-84): Wunden FUN_80037edc (0,0xa) (5,0x32) (7,0x32); Wort aca58 := 7 (Leiche).
 *      KEIN 0x8001ad68 und kein Write auf +0x34/+0x3c/+0x6a: Leon bleibt stehen. Der Port fuhr den Finisher bis
 *      Nachbesserung 5 durch den Wurf-Hook 0x8011c118 (cmd 5, Platzierung relativ zum alten Rear-up-Anker) -> Sprung
 *      um 14000 Einheiten (Abnahme 5, w3y F2814). Haken: re15_player_victim_devour (Typ 0x27 -> start) und
 *      re15_player_victim_tick (Zustand 2 + Greifer 0x27 -> tick), enemy_ai_common.c. Ereignis-Protokoll fuer den
 *      Riegel: RE15_AFFEN_FIN_* | Bild. */
#define RE15_AFFEN_FIN_LOG_N    8
#define RE15_AFFEN_FIN_EINTRITT 0x1000
#define RE15_AFFEN_FIN_FALL     0x2000
#define RE15_AFFEN_FIN_TOD      0x4000
void     re15_affen_finisher_start(void);
void     re15_affen_finisher_tick(re15_actor_t *pl, const re15_enemy_bank_t *vb);
int      re15_affen_finisher_aktiv(void);
int      re15_affen_finisher_ereignisse(int *out, int max);

#endif /* RE15_AFFEN_H */
