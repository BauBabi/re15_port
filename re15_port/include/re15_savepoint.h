/*
 * re15_savepoint.h — the in-room PHONE save-points (FE-4 Phase 3).
 *
 * RE1.5's save-points are telephones: examining one runs a Message_on showing
 * "It's a phone. You can save your progress with this. Save is not available in
 * this preview." (the prototype disabled the actual save). Per the port mandate
 * these become working save-points (RE2's typewriter role). A save-point is
 * identified byte-true by the (room, message-id) pair of that phone message —
 * message-id == the room's main<NN>.msg index (voice/text both key off it).
 *
 * The 14 phone locations (RE-verified: every room whose message data contains
 * "You can save your progress"), Leon room + Elza mirror (odd id):
 *   STAGE1 1070/1071 main20 · 1120/1121 main06 · 1150/1151 main01
 *   STAGE2 2010/2011 main03
 *   STAGE3 30A0/30A1 main01 · 30B0/30B1 main01
 *   STAGE4 4010 main43 · 4011 main07
 */
#ifndef RE15_SAVEPOINT_H
#define RE15_SAVEPOINT_H

#include <stdint.h>

/* 1 if a Message_on of `msg_id` in `room_id` is a phone save-point message. */
int re15_savepoint_is(unsigned room_id, uint8_t msg_id);

/* Set by the SCD Message_on handler when a save-point phone is examined; the
 * platform polls it to open the save flow (gated on the MEMORY CARD item), then
 * clears it. One-shot request, like the boot-movie gate. */
int  re15_savepoint_pending(void);
void re15_savepoint_set_pending(int on);

/* Clear the pending request + latched cut. Called on a room change so a stale
 * pending (or cut) from the previous room can't leak into the newly-entered room.
 * NOTE: there is deliberately NO post-save re-examine cooldown — the examine fires
 * only on a fresh action-button EDGE (a held button can't re-fire), so every
 * DELIBERATE press re-opens the menu (RE2 typewriter behaviour) with no dead period.
 * An earlier 90-frame cooldown swallowed clicks made within ~3s of closing the menu
 * ("sometimes it opens, sometimes not"); it was a workaround for the message-clear
 * flicker that no longer exists (save phones now skip the flavor dialog entirely). */
void re15_savepoint_reset(void);

/* The camera cut latched at the EXAMINE action (in the AOT scan, BEFORE the phone's
 * SCD sub runs its Cut_chg to the interaction close-up). The save block stores THIS
 * — the stable gameplay framing — not the transient close-up that is active a couple
 * of frames later when the save-point handler samples the live cut. Byte-true: the
 * original loads the gameplay perspective, not the desk close-up. -1 = unset. */
void re15_savepoint_set_cut(int cut);   /* latch at the examine action fire */
int  re15_savepoint_saved_cut(void);    /* the latched gameplay cut, or -1 */

/* ORTSNAMEN-INDEX des Save-Punkts — UEBERNAHME DES VORPROJEKT-PATCHES (Nutzer-Entscheidung
 * 2026-08-08, analysis/bug_save_room_name_recheck.md §3): Der Original-Resolver FUN_80026e4c
 * @0x80026e4c ist ein return-0-Stub (Datei 0x1764c: jr ra / addu v0,zero,zero); die
 * Anzeige-Maschinerie existiert komplett (Slot-Zeile sysmes 0x1a+idx @0x80026818/20,
 * BIOS-Kartentitel SJIS-Tabelle 0x13*idx @0x80026f0c-28). Der Patch des Vorprojekts
 * (reAi/scripts/patch_save_final.py Block [Z]) fuellt den Index ueber game_state[3]
 * = 0x800B0FBF (Kartenblock-Offset +0x203) mit genau ZWEI Werten:
 *   0 = Schreibmaschine ROOM1150/1151 (SCD_SAVE_RET @0x800708c0 `sb zero`)
 *   1 = Telefon        ROOM1070      (AOT_TYPE1_HOOK @0x8007087c `sb v0=1`)
 * Alle uebrigen Save-Raeume hat der Patch NIE definiert -> 0 (= Stub-Verhalten).
 * Latch: beim Pending-Set per Registry-Lookup des ausloesenden Raums; reset() wischt auf 0. */
void    re15_savepoint_latch_loc(unsigned room_id);  /* Registry-Lookup -> Latch */
uint8_t re15_savepoint_loc(void);                    /* der gelatchte Index (0/1) */

#endif /* RE15_SAVEPOINT_H */
