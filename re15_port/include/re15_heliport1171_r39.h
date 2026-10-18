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
void re15_r39k_reach_bildanfang(void);           /* Walker-Takt des naechsten Bildes (Spieler nicht angehalten)       */
/* Im Event-Reach-Takt (game_step_common.c) VOR dem Vorschub: 1 = Takt verbraucht (Warten bzw. Phase 0 = Bild 0). */
int  re15_r39k_reach_takt(re15_actor_t *pl);

/* NACHBESSERUNG 1 — M3(b) KRAEHEN (Typ 0x21, RE1.5-Hirn: Ereignis-Kraehen ROOM1171 in jedem KI-Modus). Das Original
 * fuehrt +0x94 (Clip) / +0x95 (NAECHSTES Bild) / +0x8f (Blende); GEZEICHNET wird, was FUN_8001f314 zuletzt posiert hat:
 *   Clip-Setzer 0x80115d94: `sb a0,148(v0)` (+0x94) @0x80115da0, `sb zero,149(v0)` (+0x95 = 0) @0x80115db0,
 *     `sb v0(=7),143(v1)` (+0x8f) @0x80115dc4 — OHNE zu posieren.
 *   f314 posiert (+0x94, +0x95, +0x8f) (`lbu v0,149(t0)` @0x8001f35c, `sw a2,360(t0)` @0x8001f36c) und schiebt erst
 *     danach vor (+0x95++ @0x8001f618-1c, +0x8f-- @0x8001f5b0-b4, Umlauf `sb zero,149` @0x8001f63c + Rueckgabe 1).
 *   Die Kraehen-Handler lesen +0x95 VOR ihrem f314 (ASCEND `lbu v1,149(a0)` @0x80115078 vor f314 @0x801150f0, Flug
 *     Sub 5 @0x80112cac vor @0x80112d1c, Sub 4 @0x80112b10) = das Bild, das dieser Takt posieren WIRD.
 *   Tod: Landung setzt Clip 0x0a @0x801148f4 OHNE f314 im selben Takt (die Fall-Pose bleibt), FINISH f314
 *     @0x8011493c posiert Bild 0..11 bei Timer 11..0 (@0x80114950-60), Zustand 7 @0x80114978 im Takt von Bild 11;
 *     Zustand 7 0x801157e8 ruft kein f314 -> die Leiche haelt Bild 11.
 * Der Port renderte (motion, anim_frame, anim_frac) NACH dem Vorschub = immer ein Bild voraus (Gemessen ROOM1171:
 * ORIENT 1..7,0 statt 0..7; Leiche Clip 0x0a Bild 12 statt 11; Landebild schon Clip 0x0a Bild 0). Jetzt: diese Tabelle
 * haelt +0x94/+0x95/+0x8f des Originals, die Aktorfelder das zuletzt POSIERTE (= gezeichnete) Bild. */
void re15_r39k_kraehe_clip(re15_actor_t *e, uint8_t clip);                    /* 0x80115d94 */
int  re15_r39k_kraehe_anim(re15_actor_t *e, const uint8_t *laengen, int n);  /* f314: posieren, dann vorschieben */
int  re15_r39k_kraehe_bild(const re15_actor_t *e);                           /* +0x95 (naechstes Bild)          */

/* NACHBESSERUNG 2 — N1 Kraehen-Wurzel 0x80112020: nach dem Zustand (`jalr` @0x801121b4) laeuft OHNE Sprung der Nachlauf
 * Kasten-Versatz @0x801121c4, Koerper-Schub @0x801121d4 (+0x1d0 @0x801121e8) und der WAND-PASS FUN_8003b0a4 @0x80112204
 * (&+0x34, Radius Kasten[3] = 200 @0x80112200, Maske 4 @0x801121fc; +0x1d1 = Rueckgabe @0x80112218) — fuer JEDEN Zustand
 * (Tabelle @0x8012111c: INIT 0x8011224c, DEATH 0x801146d0, FLIGHT-2 0x80114e54, 0x801157e8, Stubs 0x80114e4c).
 * Band = +0x82 der Wurzel (@0x80112154, VOR dem Zustand); Wort0 Bit 3 ueberspringt den Pass (@0x8003b12c-40). Der Port
 * hatte ihn nur in Zustand 1. alt_x/alt_z = Lage vor dem Zustand (Rechteck-Schub; Spawn-Lage +0x40/+0x44 @0x800421a0-b8).
 * Gemessen ROOM1171: Kraehe 3 spawnt in der Westwand-Zelle und wird im INIT-Takt hinausgeschoben (Dossier 13.3). */
void re15_r39k_kraehe_wandpass(re15_actor_t *e, int32_t alt_x, int32_t alt_z);

#endif /* RE15_HELIPORT1171_R39_H */
