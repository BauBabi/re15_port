/*
 * RE1.5 Rebuilt — HELIPORT ROOM1171, Johns Animationen (Runde 39 Spur K). Nutzer-Befund (AUFTRAG.md, woertlich):
 *   "Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran wink
 *    Animation, die er im Original nicht macht. Auch sonst scheinen die Animationen hier noch nicht ganz zu passen."
 * Dossier mit allen Messungen und Belegen: analysis/befunde_runde39/K_heliport1171.md (Abschnitte 1-6).
 *
 * K1 (enemy_common.c rbj_resolve_slot): RBJ-Kanal-Cache - ein Aktor-Slot OHNE Record belegt keinen Platz.
 *
 * K2 NPC-INIT-POSE (diese Datei). Jede NPC-INIT posiert EINMAL das LOCO-Paar, nicht die eigene Bank:
 *   John 0x49 (STAGE1.BIN): `ori v0,zero,0x2` / `sb v0,148(v1)` (+0x94 = 2) @0x8011de78-7c, `sb zero,149(v0)`
 *   (+0x95 = 0) @0x8011de8c, `sb zero,143(v0)` (+0x8f = 0) @0x8011de9c, `addu a2,zero,zero` @0x8011deb8,
 *   `lw a0,132(v0)` (+0x84) / `lw a1,364(v0)` (+0x16c) / `jal 0x8001f314` / `ori a3,zero,0x200` @0x8011debc-c8.
 *   Gleiches Muster in ALLEN NPC-INITs: STAGE1 @0x8011c808/@0x8011cdd8/@0x8011d3e4/@0x8011d978/@0x8011dec4/
 *   @0x8011e49c, STAGE3 @0x8011c520/@0x8011caf8, STAGE5 @0x8011c1a0/@0x8011c770/@0x8011cd34/@0x8011d30c/@0x8011d978.
 *   f314 posiert dabei +0x95 = 0 (`lbu v0,149(t0)` @0x8001f35c, `sw a2,360(t0)` @0x8001f36c) und schiebt erst danach
 *   vor (@0x8001f618-1c). Die Pose bleibt stehen, bis der naechste anim_set sie ersetzt: die Eskorte-EXEC
 *   (+0x9 = 0, eigene Bank, Stehen @0x8004f384-8c) oder der Executor (state 4). Mit einem Gegner < 15000 laeuft
 *   die Eskorte nur die DECIDE 0x8004f100 (Tabelle @0x8012181c[1], kein anim_set bis `jr ra` @0x8004f1fc) — ROOM1171:
 *   John steht so von F0 bis zum ersten Plc_motion @0x01416.
 *   Port: der Merker gilt ab der INIT bis zur ersten Eskorte-EXEC bzw. zum ersten Executor-Takt; solange ihn ein
 *   Aktor traegt, waehlen re15_actor_uses_loco_bank / re15_actor_uses_own_bank (enemy_ai_common.c, die EINE Regel
 *   fuer Renderer, Treffer-Skelett und Uhr) das Loco-Paar.
 *
 * K3 (enemy_ai_common.c re15_npc_sub_motion): Plc_motion-Saattakt posiert Bild 0 (Phase 0 @0x80050cec-d0c faellt
 *   ohne Sprung in den f314-Rumpf @0x80050d34-4c).
 * K4 (szenenleon_r36.c re15_szl_schub_npc): Schub Phase 5 posiert im Bit-Takt noch Clip 0x12 (anim_set @0x80052af4
 *   vor der Bit-Abfrage @0x80052b44); Clip 0x11 rueckwaerts erst mit Phase 6 @0x80052b98.
 */
#ifndef RE15_HELIPORT1171_R39_H
#define RE15_HELIPORT1171_R39_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_R39K_INIT_CLIP     2      /* +0x94 = 2 @0x8011de78-7c (alle NPC-INITs)                          */
#define RE15_R39K_INIT_BILD     0      /* +0x95 = 0 @0x8011de8c; f314 posiert dieses Bild @0x8001f35c-36c     */
#define RE15_R39K_SCHUB_SAAT    0xFEu  /* K4: Port-Merker in +0x6-Begleitbyte sub_state_3 (Schub-Sub): Phase 6
                                        * setzt Clip 0x11 erst in ihrem ersten Takt (@0x80052b84-98)            */

/* K2: Merker setzen (NPC-INIT, enemy_ai_common.c re15_npc_ai_tick Fall 0) bzw. loesen (erster anim_set auf einem
 * anderen Kanal: Eskorte-EXEC, Executor-Takt). */
void re15_r39k_init_pose_setzen(int slot, uint8_t typ);
void re15_r39k_init_pose_loesen(int slot);
/* 1 = der Aktor zeigt noch die INIT-Pose (Loco-Paar Clip 2 Bild 0). */
int  re15_r39k_init_pose(const re15_actor_t *a);
/* Raumaufbau (scd_room_setup.c, Installer-Block): alle Merker loeschen. */
void re15_r39k_install(uint16_t room);

/* NACHBESSERUNG 1 — SPIELER-SUB 6 (Event-Reach 0x800517f0, Spieler-Tabelle @0x80073e30[6] = NPC-Tabelle @0x80076ca0[6]).
 * M1 Saattakt: Phase 0 `sb v1(=1),6(a0)` @0x80051844, `sb v1,148(v0)` (+0x94 = 1) @0x80051854, `sb zero,149(v0)`
 *    (+0x95 = 0) @0x80051864, `sb v0(=7),143(v1)` (+0x8f = 7) @0x80051874 faellt OHNE Sprung in den Phase-1-Rumpf
 *    @0x80051878: `jal 0x8001f314` (+0x170/+0x174, a2 = 0) @0x8005188c posiert Bild 0 (f314 posiert +0x95
 *    @0x8001f35c-36c, Vorschub erst @0x8001f618-1c). Clip 1 = Bild 0..15 (16 Takte); im Takt, der Bild 15 posiert,
 *    liefert f314 1 -> +0x6 = 2 (`addu v1,v1,v0` / `sb v1,6(a0)` @0x800518a8-b0); Phase 2 @0x800518b4 setzt +0x94 = 2
 *    @0x800518c8 / +0x8f = 7 @0x800518d8 und posiert im selben Takt Clip 2 Bild 0 (f314 @0x800518f0).
 * M1/M3a Startbild: setzt der SPIELER-DISPATCHER selbst +0x5 = 6 (Plc_dest-Ankunft: Gang `sb v0(=6),-13735(at)`
 *    @0x80030cf0 hinter dessen f314 @0x80030c78, Drehen @0x800313ec vor dessen f314 @0x80031494, Lauf @0x80030f44,
 *    rueckwaerts @0x800311a8/@0x80031318; Schub Phase 7 @0x80052be0), laeuft Sub 6 erst im NAECHSTEN Bild: der
 *    Dispatcher FUN_80031c44 (@0x8001ce0c) dispatcht +0x5 einmal je Bild (@0x8003068c-ac). Davor laeuft in jedem Bild
 *    das SCD (`jal 0x8003f038` @0x8001cdec -> scd_execute_scripts @0x8003f088); erfuellt dort die Warteschleife
 *    (While @0x8003f6f4 kehrt mit 1 zurueck @0x8003f840, Evt_end im Gosub mit 1 @0x8003f22c) und der naechste
 *    Plc_dest setzt +0x4/+0x5/+0x6 neu (@0x80041c14-1c), kommt Sub 6 gar nicht an die Reihe — kein Clip-1-Bild.
 *    Setzt dagegen das SCD selbst Modus 6 (Plc_dest @0x80041c18 vor dem Dispatcher), posiert Phase 0 im selben Bild. */
void re15_r39k_reach_saat(int naechstes_bild);   /* Sub 6 beginnt: 0 = Phase 0 in diesem Bild, 1 = erst im naechsten */
void re15_r39k_reach_aus(void);                  /* Sub 6 ersetzt (Plc-Befehl, Kommando-Reset)                       */
int  re15_r39k_reach_wartet(void);               /* 1 = +0x5 = 6 steht, Sub 6 lief noch nicht (Dispatcher-Bild)       */
/* Im Event-Reach-Takt (game_step_common.c) VOR dem Vorschub: 1 = Takt verbraucht (Warten bzw. Phase 0 = Bild 0). */
int  re15_r39k_reach_takt(re15_actor_t *pl);

#endif /* RE15_HELIPORT1171_R39_H */
