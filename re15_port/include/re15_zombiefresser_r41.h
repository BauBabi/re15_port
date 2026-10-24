/*
 * Runde 41 Spur I "zombiefresser" — die Zombies in ROOM2000/2001, ROOM3000/3001, ROOM3010/3011 LIEGEN und stehen
 * auf, wenn man ihnen naeher kommt (RE1.5-Deskriptor 0x84/0x85). Dossier analysis/befunde_runde41/I_zombiefresser.md.
 *
 * NUTZER (analysis/befunde_runde41/AUFTRAG.md, woertlich): "In ROOM 2000 und ROOM 2001 ist die Animation/Aktion der
 * Zombies durchgehend falsch. Sind sind am Boden und fressen, anstatt aufzustehen wenn man ihnen näher kommt. gleiches
 * trifft zu bei den Zombies in STAGE3 in ROOM 3000, 3010 etc. es ist durchgehend falsch."
 *
 * DAS ORIGINAL (RE1.5, selbst disassembliert; STAGE*.BIN roh @0x80100000; der Zombie-Code ist je Stage eine
 * verschobene Kopie: STAGE2 = STAGE1 - 0x16c, STAGE3 = STAGE1 + 0xec):
 *   Wurzel (Registrierung sw handler,0x80072bac+typ*4): STAGE1 0x80100424, STAGE2 0x801002b8, STAGE3 0x80100510.
 *   INIT-Posen-Decoder STAGE2 (STAGE1 @0x80100ca4.., STAGE3 @0x80100d90..):
 *     80100b44  beq (+0x9 & 0x80)==0          ; Tor Bit 0x80
 *     80100b74  sb 0x0c,148  fuer sel {4,7,9} ; 80100bb8  sb 0x0e,148 fuer sel {5,8,0xa}
 *     80100c34  jal 0x8001f314 (+0x170,+0x174,0,0x200) ; Pose = Bank1 Clip 0x0c/0x0e Bild 0 (flach liegend)
 *     80100c7c  sb 0x12,148  fuer sel {4,7,9} ; 80100cc0  sb 0x13,148 fuer sel {5,8,0xa}  (FINAL, ohne f314)
 *     80100d44  sb zero,9    fuer sel 6       ; der ECHTE Fresser verlaesst den Grid-Dispatch (Modus-0-Sub 0xc)
 *   => 0x84/0x85 behalten grid 0x84/0x85 und laufen ueber den Grid-Dispatcher @0x80117910[4]/[5] (STAGE2;
 *      STAGE1 @0x8011f80c, STAGE3 @0x8011d95c): DECIDE 0x80103814 / ANIMATE 0x801038ec (STAGE1 0x80103980 /
 *      0x80103a58, STAGE3 0x80103a6c / 0x80103b44).
 *   WECKER STAGE2 0x80103814:
 *     8010382c  sltiu v0,v0,0xfa0             ; +0x1d0 (2D-Abstand) < 4000
 *     80103838  lbu v0,6 / bne               ; nur bei +0x6 == 0
 *     8010384c  sb 1,6                        ; Phase 1
 *     80103858  andi v0,v0,0xf / 80103860 sh v0,156 ; +0x9c = rand & 0xf (Wartezeit)
 *     80103864..74 andi 3 / jal 0x800453d0 a0=5 ; 1/4: Raum-SE 5
 *   LIEGE-PHASENMASCHINE STAGE2 0x801038ec:
 *     P0 80103940-60  +0x93 |= 1 ; +0x1b8 = 1  ; liegt, Ein-Treffer-Riegel = unschiessbar
 *     P1 80103964-98  was = +0x9c ; +0x9c = was-1 (Delay-Slot 80103974) ; was == 0 -> +0x6 = 2, +0x8f = 0xf
 *     P2 8010399c-cc  jal 0x8001f314 (+0x170,+0x174,0,0x100) ; +0x6 += fertig   ; spielt +0x94 = 0x12 / 0x13
 *     P3 801039d0-0c  sb zero,9 ; sw 0x201,4 ; +0x93 &= 0xfe ; +0x1b8 = 0     ; Kampf (Modus 0, Sub 2)
 *   Bank 1 (CDEMD0.EMS, Typ 0x16, tools/r41_zombiefresser/clip_hoehe.py): Clip 0x12 = 98 Bilder Aufstehen aus dem
 *   LIEGEN (Hoehe f0 -457 -> f97 -2361), Clip 0x13 = 120 Bilder (-590 -> -2384); die Fress-Schleifen sind 0x27/0x28.
 *   DuckStation (stage_saves/mzd_demo/mzd_stage2_room.sav + RAM-Patch, Dossier 2.1b): Ruhe +0x94 = 0x12 / +0x6 = 0 /
 *   +0x93 = 1; nach d < 4000: P2 Clip 0x12 f7/f32/f65 (+0x9c = -1), danach 0x201 / grid 0 / +0x93 = 0.
 *   => ein 0x84/0x85-Zombie FRISST NICHT, er liegt flach und steht auf, sobald Leon naeher als 4000 kommt.
 *
 * WAS DER PORT VORHER TAT: RE2-KI (Standard) schickte sel 4/5 in EXEC[8] FRESSEN (RE2-Fress-Clips 18/19/20, Phase 3
 * 2026-09-19 "4/5/6 = EINE Familie (Fresser)" — das Etikett [5]/[6] = Fresser war falsch, der sel-6-Fresser erreicht
 * den Grid-Dispatch nie, grid = 0 @0x80100d44). RE1.5-KI spielte beim Wecken Clip 0x29 (Aufstehen aus dem KNIEN).
 *
 * ABBILDUNG:
 *   RE2-KI: sel 4/5 unter Bit 0x80 -> der RE2-Liege-Executor EXEC[7] (0x701, +0x10E = 0x4002 Latch, Pose Clip 23/22,
 *     RE2 @0x801037CC-E4; Aufstehen Clip 8/9 @0x80103838-80; 0x101 @0x80103900). Der RE2-Overlay hat fuer den Latch
 *     keinen Naehe-Wecker (einziger Clear @0x80104F0C, Skript-/EXE-seitig) -> PORT-MAPPING des Zeitpunkts auf den
 *     RE1.5-Zwilling: Wecker 0xFA0 + Wartezeit rand&0xf + Latch-Clear am Ende der Wartezeit (= RE1.5 P1 -> P2).
 *     PORT-WAHL Ton: im RE2-Flavor KEIN RE1.5-SE 5 — RE2s eigenes Stoehnen in EXEC[7] P2 (SE 12/10/11,
 *     @0x80103884-D4) faellt beim Aufstehen (Vorbild: Skript-Wecker D15.2 im selben Executor, ebenfalls ohne SE 5).
 *     Unschiessbar bis zum Pose-Ausgang (RE1.5 P0 +0x93 |= 1 bis P3; RE2 +0x1D3 |= 0x80 bis P4 @0x80103914).
 *   RE1.5-KI: Nibble 4/5/6 -> Wecker (wie oben) + die bestehende Liege-Phasenmaschine (enemy_ai_common.c
 *     re15_enemy_ai_lying_phase = FUN_80103a58), die +0x94 = 0x12/0x13 ausspielt.
 */
#ifndef RE15_ZOMBIEFRESSER_R41_H
#define RE15_ZOMBIEFRESSER_R41_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_R41_WECK_DIST     0xFA0u  /* sltiu v0,v0,0xfa0 @0x8010382c (STAGE2), @0x80103998 (STAGE1), @0x80103a84 (STAGE3) */
#define RE15_R41_WARTE_MASKE   0xFu    /* andi v0,v0,0xf / sh v0,156 @0x80103858-60 (STAGE2) */
#define RE15_R41_MOAN_MASKE    3u      /* andi v0,v0,0x3 @0x80103864, jal 0x800453d0 a0=5 @0x80103870-74 */
#define RE15_R41_MOAN_SE       5       /* ori a0,zero,0x5 @0x80103874 (Raum-SE 5) */
#define RE15_R41_UNSCHARF      (-1)    /* Port-Sentinel fuer +0x9c: Wecker noch nicht ausgeloest (das Original
                                        * haelt diese Information in +0x6 == 0 @0x80103838-40; im RE2-Executor
                                        * gehoert +0x6 aber EXEC[7]). Nach der Wartezeit steht +0x9c im Original
                                        * auf 0xffff (Post-Dekrement @0x80103974) = derselbe Wert. */

/* 1 = Sce_em_set-Deskriptor (+0x9) ist ein RE1.5-Liegender mit Naehe-Wecker: Bit 0x80 und sel 4/5
 * (Tor @0x80100b44, sel {4,7,9}/{5,8,0xa} @0x80100b4c-bb8; Dispatcher [4]/[5] @0x80117920/24). */
int  re15_r41_liegewecker_deskriptor(uint8_t beh);

/* 1 = dieser Aktor ist ein solcher Liegender UND liegt noch (grid traegt den Deskriptor; P4/P3 loeschen ihn). */
int  re15_r41_liegewecker_liegt(const re15_actor_t *e);

/* RE2-INIT-Haken (re2z_init, Liege-Zweig): Wecker unscharf. */
void re15_r41_liegewecker_init(re15_actor_t *e);

/* RE2-EXEC[7]-Haken (re2z_exec_lying, VOR dem Port-Naehe-Wecker 0xBB8): 1 = der Aktor gehoert diesem Wecker
 * (dann darf der 0xBB8-Wecker NICHT greifen), 0 = nicht zustaendig. */
int  re15_r41_liegewecker_re2_tick(re15_actor_t *e, const re15_actor_t *pl);

/* RE1.5-KI-Haken (enemy_ai_common.c, Grid-Nibble 4/5/6 mit +0x5 == 0): der DECIDE-Teil (Wecker). Danach laeuft
 * im Aufrufer die Liege-Phasenmaschine (ANIMATE). 1 = zustaendig, 0 = nicht. */
int  re15_r41_liegewecker_re15_decide(re15_actor_t *e);

/* NACHBESSERUNG 1 (Abnahme 0, M1) — die LIEGEPOSE VOR DEM WECKEN (RE1.5-KI; gilt fuer 0x84/0x85 UND 0x87/0x88).
 * Das Original posiert das Skelett nur in anim_set (FUN_8001f314); +0x94 ist nur der Clip fuer den NAECHSTEN Aufruf.
 * Der INIT-Decoder (STAGE2; STAGE1/STAGE3 identisch verschoben) posiert die Liege-Pose und schreibt erst DANACH den
 * Aufsteh-Clip:
 *   80100b70/74  ori v0,zero,0xc / sb v0,148      sel {4,7,9}   -> +0x94 = 0x0c
 *   80100bb4/b8  ori v0,zero,0xe / sb v0,148      sel {5,8,0xa} -> +0x94 = 0x0e
 *   80100c0c  sb zero,149 ; 80100c1c  sb zero,143 -> +0x95 = 0, +0x8f = 0 (FUN_8001f3bc setzt den Keyframe DIREKT)
 *   80100c34  jal 0x8001f314 (a0 = +0x170, a1 = +0x174, a2 = 0 @0x80100c28, a3 = 0x200 @0x80100c38)
 *             (STAGE1 @0x80100da0, STAGE3 @0x80100e8c)
 *   80100c7c / 80100cc0  sb 0x12 / 0x13,148       -> FINAL +0x94 OHNE anim_set
 * P0 (@0x80103940-60) und P1 (@0x80103964-98) rufen kein anim_set; der erste nach der INIT ist P2 @0x801039a8 (mit
 * +0x8f = 0xf @0x80103998: Ueberblendung aus der gehaltenen Pose). Script-geweckte 0x89/0x8a springen direkt nach P2
 * (@0x80103a1c). Bank 1 (CDEMD0.EMS dir[3]/dir[4], alle Zombie-Typen 0x10/0x11/0x12/0x16/0x18 wortgleich): Clip 0x0c
 * n=1 Bild 0 = 0x0000012d, Clip 0x0e n=1 Bild 0 = 0x00000149 (Clip 0x12 f0 = 0x190, Clip 0x13 f0 = 0x1c2).
 * Der Port posiert jedes Bild aus (motion, anim_frame) = (+0x94, +0x95); diese Funktion liefert dem Renderer
 * (anim_select_common.c, Gegnerzweig) den Clip, den das Skelett TATSAECHLICH traegt, solange er von +0x94 abweicht:
 * 0x0c (sel 4/7/9) bzw. 0x0e (sel 5/8/0xa), sonst -1. +0x94 (motion) bleibt RAM-treu 0x12/0x13. */
#define RE15_R41_INIT_POSE_A   0x0Cu   /* ori v0,zero,0xc / sb v0,148(v1) @0x80100b70-74 (STAGE2), sel {4,7,9} */
#define RE15_R41_INIT_POSE_B   0x0Eu   /* ori v0,zero,0xe / sb v0,148(v1) @0x80100bb4-b8 (STAGE2), sel {5,8,0xa} */
int  re15_r41_liege_skelett_clip(const re15_actor_t *a);

/* NACHBESSERUNG 1 (M1b) — der ERSTE P2-Takt. P1 setzt +0x6 = 2 (@0x80103984) und +0x8f = 0xf (@0x80103998, Delay-Slot
 * von `j 0x80103a10` @0x80103994) und kehrt OHNE anim_set zurueck; P2 ist ein eigener Fall der +0x6-Weiche
 * (@0x80103928 -> 0x8010399c), sein anim_set (@0x801039a8, a3 = 0x100) laeuft erst im NAECHSTEN Takt und posiert dort
 * Bild 0 (Gewicht 0xf00). Der Port-Vorschub (player_common.c) liefe aber schon in diesem naechsten Takt vor -> P2 zeigte
 * Bild 1 / +0x8f 14 und endete einen Takt zu frueh. Abhilfe im Port-Vokabular: motion_init_delay = 1 ("Saat-Takt posiert
 * Bild 0", re15_npc_seed_clip / game_step_common.c), NUR beim Uebergang P1 -> P2 (ph_vor = +0x6 vor dem ANIMATE == 1).
 * Die Skript-Weckung 9/10 (DECIDE @0x801039fc setzt +0x6 = 2, ANIMATE im selben Takt) bleibt unberuehrt. */
void re15_r41_liege_p2_saat(re15_actor_t *e, uint8_t ph_vor);

/* Schalter fuer Tests (Standard AN). */
void re15_r41_liegewecker_enable(int an);
int  re15_r41_liegewecker_enabled(void);

#endif /* RE15_ZOMBIEFRESSER_R41_H */
