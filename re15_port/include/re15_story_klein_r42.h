/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe Kleinkram: T-08 (Ada im Schutzraum 6020) und T-09 (Blue Master Keycard).
 *
 * PORT-STORY nach dem freigegebenen Konzept C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md, Abschnitt 5
 * T-08 [B22] und T-09 [B39]; VERTRAG analysis/befunde_runde42/VERTRAG.md Abschnitt 3. Keine neuen Bank-9-Bits, keine neuen
 * Nachrichten-Ids (Zensus belege/zensus_r42.txt unveraendert).
 *
 * ---- T-08: Ada erscheint im Schutzraum 6020 nur, solange sie lebt ----------------------------------------------------------------
 * ROOM6020 sub00 @0x00B8E `06 00 2e 00` Ifel_ck, @0x00B92 `21 03 4c 01` Ck(3,76)=1, dann zwei Saetze ohne weitere Bedingung:
 *   @0x00B96 `44 00 40 40 00 01 00 ff ..` Sce_em_set Slot 0 Typ 0x40 = Marvin
 *   @0x00BAA `44 01 42 40 00 01 00 ff ..` Sce_em_set Slot 1 Typ 0x42 = Ada
 * (Typen: include/re15_cut10f0.h "Ada Typ 0x42 .., Marvin Typ 0x40"). Adas Tod setzt ROOM50B0 sub02 @0x005A6 `22 03 62 01`
 * Set(3,0x62) - ein Merker in Bank 3 (Spielstand), kein Bank-9-Bit. Zensus aller 240 RDTs (tools/scd_dump_room.py, kk_NOTIZ.md):
 * (3,0x62) kommt NUR in ROOM50B0 vor - Ck @0x004E6 (Ada-Szene noch nicht gelaufen) und Set @0x005A6 (Anfang der Szene, in der Ada
 * stirbt); der Port schreibt das Bit nirgends selbst. Der Mod-Satz @0x00BAA fragt (3,0x62) nicht ab [B22]: wer nach 50B0 zurueck in
 * den Schutzraum geht, findet Ada lebend. PORT-STORY: der Satz @0x00BAA wird verworfen, wenn (3,0x62) = 1 - am selben Tor wie das
 * Kill-Flag (scd_vm.c Sce_em_set, byte-true FUN_800420a0 @0x80042120-38: Record verworfen, kein Aktor). Marvin (@0x00B96) bleibt.
 * Der Schutzraum-Dialog (L18) kommt erst in v0.8.32.
 *
 * ---- T-09: Beschreibung der Blue Master Keycard --------------------------------------------------------------------------------
 * Item 0x47 (Namensbank DEBUG.BIN, Eintrag 0x47 "Blue Master Keycard"; Fundort ROOM4010 main00 Item_aot_set Slot 9 @0x00592). Die
 * ausgelieferte Beschreibung (Bank @0x800c50de, Eintrag 0x47 @0x800c5e53) nennt "Goro Macida", die Notiz in 4010 (msg 39 @0x183D)
 * "Michael". Neutext aus tools/r42_story/texte_klein.py -> gen/story_klein_r42_texte.inc, Form wie das Original (3 Zeilen, 1 Seite).
 * Die Beschreibung haengt am Gegenstand, nicht an der Figur: gilt fuer Leon und Elza.
 */
#ifndef RE15_STORY_KLEIN_R42_H
#define RE15_STORY_KLEIN_R42_H

#include <stdint.h>

#define RE15_SKL_RAUM_SCHUTZRAUM 0x6020u   /* nur Leons Fassung: ROOM6021 setzt Typ 0x49/0x4B (sub00 @0x00B96/@0x00BAA), keine Ada */
#define RE15_SKL_TYP_ADA         0x42u     /* ROOM6020 sub00 @0x00BAA `44 01 42 ..` */
#define RE15_SKL_TOD_BANK        3         /* ROOM50B0 sub02 @0x005A6 `22 03 62 01` Set(3,0x62) */
#define RE15_SKL_TOD_BIT         0x62

/* Sce_em_set-Haken (scd_vm.c): 1 = Satz verwerfen (Ada in 6020 nach ihrem Tod in 50B0). */
int re15_story_klein_r42_spawn_weg(uint16_t room_id, uint8_t type);
/* Beschreibungs-Haken (re15_inv_screen.c re15_inv_desc_entry): Neutext oder NULL. */
const uint8_t *re15_story_klein_r42_desc(uint8_t item_id);
/* Zaehler fuer Tests: [0] Ada verworfen, [1] Ada zugelassen (lebt). */
extern unsigned g_re15_skl_zaehler[2];

#endif
