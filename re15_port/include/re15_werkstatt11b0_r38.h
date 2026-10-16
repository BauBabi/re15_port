/* re15_werkstatt11b0_r38.h — Runde 38 Spur C "werkstatt11b0" (analysis/befunde_runde38/AUFTRAG.md Z.16/Z.34/Z.35,
 * Dossier analysis/befunde_runde38/C_werkstatt11b0.md). Die ROOM11B0-Szene selbst (Schub-Sub 0x13, Erweiterung mit
 * msg 20..22) stammt aus Runde 36 Spur J (re15_szenenleon_r36.h); diese Datei traegt nur die zwei Korrekturen:
 *
 * (2) SCHIEBE-TON beim gemeinsamen Schieben des Containers.
 *     Der Schub-Sub spielt im Original Bank 2 Satz 7 bei Bild 1 jedes Clip-0x12-Umlaufs (@0x80052ac0 `lbu v1,149(v0)`
 *     == 1 -> @0x80052acc `lui a0,0x207` / @0x80052ad8 `jal 0x80045024`) und in Phase 7 (@0x80052bc8/@0x80052bcc) —
 *     derselbe Satz wie Leons Kistenschieben (@0x80035a18 `lui a0,0x207` / @0x80035a1c `jal 0x80045024`). Bank 2 =
 *     Raum-snd0 (FUN_80045024 Tabelle @0x80010e70[2] -> @0x80045130..@0x80045144: RDT+0x08 + Satz*4). ROOM11B0 hat
 *     dort NICHTS: Satz 7 @0xE4A0 = `00 00 00 00` (VH ps=1 ts=4 vs=3) — die Szene ist im Original stumm, die Raumbank
 *     ist unfertig. Der einzige andere Raum mit Plc_dest 0x13 ist ROOM1171 (@0x01566/@0x01572); dessen snd0-Satz 7
 *     @0x6EF8 = `00 00 45 10` (Prog 0, Ton 4, Prio 5, Stimme 0) -> Ton 4 @0x77FC (vol 110, center 87) -> VAG 4
 *     @0xA8BC, 6688 B, sha1 f343933e. Bytegleich (Satz, Ton, VAG) in ROOM1090 @0x118EC/@0x121F0/@0x152B0 — dem Raum,
 *     in dem Leon die Kiste ALLEIN schiebt — sowie 1091/1170/3080. Das Schiebe-System ist in RE1.5 also fertig; nur
 *     11B0s Raumbank fehlt der Satz. Ergaenzung: in ROOM11B0 spielt Satz 7 aus der snd0-Bank von ROOM1171 (zur
 *     Laufzeit aus shared_assets/PSX/STAGE1/ROOM1171.RDT geschnitten, keine neue Datei), Takt unveraendert = Original.
 *
 * (3) "FALL-ANIMATION" Marvins nach dem Schieben = falsche Bank im UEBERGABEBILD Sub 0x13 -> Sub 6.
 *     Phase 6 @0x80052b84 posiert Clip 0x11 rueckwaerts (anim_set(+0x84,+0x16c,1,0x200) @0x80052b98), das letzte
 *     posierte Bild ist Index 0; FUN_8001f3bc setzt am Clip-Ende +0x95 = 0 (@0x8001f63c `sb zero,149(v1)`).
 *     Phase 7 @0x80052bc0..@0x80052bf4 ruft KEIN anim_set (nur Se_on @0x80052bcc, +0x5=6 @0x80052be0, +0x6=0
 *     @0x80052bf0) — die Pose im Arbeitsblock Aktor+0x188 (einziger Schreiber anim_set -> FUN_8001f3bc) bleibt
 *     stehen, und erst Sub 6 [1] @0x80051878 posiert im Folgebild Bank 1 Clip 1 (Blend +0x8f=7 aus genau dieser
 *     Pose). Der Port renderte im Uebergabebild (Sub 6, +0x6=0, Clip noch 0x11) nach der Sub-6-Regel aus Bank 1;
 *     Clip 17 >= 14 Clips -> Rueckfall Containerbank Clip 17 VORWAERTS Bild 9 = die tiefste Schiebehaltung, danach
 *     7 Bilder Blend daraus = der gemeldete "Fall". Ebenso im Uebergabebild Phase 1 -> 2: Phase 1 posiert Bank 1
 *     Clip 5 (@0x8005294c) VOR `+0x6 = 2` (@0x800529a0-b0), der Port waehlte schon die Loco-Bank.
 *     Regel: die Bank folgt dem Clip, den der Sub zuletzt posiert hat (Clip 5 -> +0x170 Bank 1, Clips 0x11/0x12 ->
 *     +0x84 Loco), und im Uebergabebild bleibt die Pose von Phase 6 (Loco Clip 0x11 Bild 0) stehen.
 */
#ifndef RE15_WERKSTATT11B0_R38_H
#define RE15_WERKSTATT11B0_R38_H

#include <stdint.h>
#include "re15_actor.h"

/* ---- (2) Schiebe-Ton -------------------------------------------------------------------------------------- */
#define RE15_W11B0_RAUM             0x11B0u  /* Leon; Satz 7 @0xE4A0 `00 00 00 00`                                */
#define RE15_W11B0_SCHUB_SATZ       7        /* 0x02070000 = Bank 2 Satz 7 (@0x80052acc/@0x80052bc8, @0x80035a18)  */
#define RE15_W11B0_QUELLE_RAUM      0x1171u  /* einziger anderer Raum mit Plc_dest 0x13 (@0x01566/@0x01572)        */
#define RE15_W11B0_QUELLE_EDT_OFF   0x6EDCu  /* ROOM1171.RDT Kopfwort +0x08 (snd0-EDT)                              */
#define RE15_W11B0_QUELLE_VH_OFF    0x6F5Cu  /* Kopfwort +0x0C (snd0-VH, "pBAV", ps=1 ts=8 vs=6)                    */
#define RE15_W11B0_QUELLE_VB_OFF    0x87FCu  /* Kopfwort +0x10 (snd0-VB)                                            */
#define RE15_W11B0_QUELLE_VAG       4        /* Satz 7 `00 00 45 10` -> Prog 0 Ton 4 @0x77FC -> VAG 4 @0xA8BC 6688 B */
#define RE15_W11B0_QUELLE_VAG_LEN   6688

/* Plattform-Haken: audio_pc.c re15_audio_room_se_snd0 fragt ihn zuerst; Rueckgabe 1 = der Satz wurde aus der
 * Ergaenzungsbank gespielt. PC: platform/pc/src/werkstatt11b0_pc.c (re15_pc_w11b0_init, main.c); PSX und
 * Unit-Tests ohne Plattform: NULL = Original-Verhalten (leerer Satz bleibt stumm). */
extern int (*re15_w11b0_snd0_hook)(int se_id);

/* Reine Entscheidung (Riegel-Zugriff, ohne Audio): 1 = in Raum room_id fehlt Satz se_id in der Raumbank (EDT-Satz
 * mit Byte 2 und 3 = 0, re15_edt_decode "empty") UND die Ergaenzung ist zustaendig (ROOM11B0, Satz 7). */
int re15_w11b0_ergaenzung_noetig(unsigned room_id, const uint8_t *raum_edt, int raum_edt_size, int se_id);

/* ---- (3) Bank im Uebergabebild -------------------------------------------------------------------------------
 * Aus re15_szl_schub_npc_kanal (szenenleon_r36.c) gerufen: -1 = keine Aussage (alte Regel), sonst 0/1/2 wie dort
 * (1 = Loco +0x84/+0x16c, 2 = Bank 1 +0x170/+0x174). */
int  re15_w11b0_schub_kanal(const re15_actor_t *e);
/* Phase 7 des NPC-Schub-Subs (re15_szl_schub_npc default-Zweig): +0x95 = 0 wie @0x8001f63c und die Pose von
 * Phase 6 (Loco Clip 0x11 Bild 0) bis zum ersten anim_set des Sub 6 halten. */
void re15_w11b0_schub_uebergabe(re15_actor_t *e);
/* Riegel: 1 = Aktor slot haelt gerade die Uebergabe-Pose. */
int  re15_w11b0_halte_aktiv(int slot);

#endif
