/*
 * RE1.5 Rebuilt — Beta-Birkin-Formen: Hangar-Form (ROOM50A0/50A1) und Vierbein-Form (ROOM5090/5091).
 *
 * ⛔ PORT-ERGAENZUNG, KEIN AUSLIEFERUNGSSTAND. Nutzerauftrag 2026-10-30 nach dem Videovergleich
 * analysis/video_80prozent/BEFUND.md (G1 + Vierbein-Screenshot V2 1:56). Beide Formen und beide Auftritte
 * gibt es nur im 80%-Material; jede Zahl, die hier NICHT mit @0x… belegt ist, ist aus dem Video gemessen
 * (Rueckprojektion durch die RID-Kamera) oder Gestaltung, und als solche markiert.
 *
 * Modelle: EMD/EM30_HANGAR.EMD, EMD/EM30_BAHNSTEIG.EMD (re15_port/tools/birkin_formen.py), gewaehlt vom
 * PC-Lader nach Raum (re15_bf_emd_datei). Clips, Skelett-Hierarchie und KI sind die von EM30 / Typ 0x30.
 *
 *   HANGAR (50A0/50A1, Cut 12, V1 4:36-4:40): Birkin packt einen anspringenden Spider-Man (Typ 0x2B), stemmt
 *     ihn ueber den Kopf und schmettert ihn vor sich auf den Boden (Birkin-Clip 0x0B = GRAB-THROW, Wurf bei
 *     Bild 0x2C wie im Original @0x80117d30), dann greift er den Spieler an (Uebergabe an die 0x30-KI).
 *   BAHNSTEIG (5090/5091): Phase 1 = Vierbein-Form mit der RE1.5-0x30-KI (Auslieferungsspawn sub00 @0x0124A,
 *     Kampfstart sub04 Pos_set @0x12FE / Member_set(0x0c,0x13) @0x130A). Wo das Original nach dem Tod in
 *     Phase 3 auf den Raum-Morph wartet (grid&0xf==2, @0x8011a6c0 — in 5090 nie gesetzt), wechselt der Port
 *     in den RE2-G5 (Typ 0x36, enemy_ai_boss_g5.c). RE15_ENDKAMPF_NUR_G5=1 stellt den alten Ablauf her
 *     (0x30 -> 0x36 schon beim Spawn).
 */
#ifndef RE15_BIRKIN_FORMEN_H
#define RE15_BIRKIN_FORMEN_H

#include <stdint.h>

/* Raumabhaengiges EMD fuer `type` (relativ zum CD-Wurzelverzeichnis) oder NULL = Auslieferungsmodell.
 * RE15_BIRKIN_FORM=original|hangar|bahnsteig erzwingt fuer 0x30 eine Form (Testen). Typ 0x29 in
 * ROOM4080/4081 -> EMD/EM29_ROT.EMD (roter Affe, Video 1 4:11-4:19). */
const char *re15_bf_emd_datei(uint8_t type);

int  re15_bf_bahnsteig_aktiv(void);      /* ROOM5090/5091 und nicht RE15_ENDKAMPF_NUR_G5 */
int  re15_bf_hangar_aktiv(void);         /* ROOM50A0/50A1 und nicht RE15_KEIN_HANGAR_BIRKIN */

/* Birkin (0x30) in 5090 nach Todesphase 2 an den G5 uebergeben (Typwechsel 0x30 -> 0x36). */
void re15_bf_morph_zu_g5(int slot);

/* Je KI-Tick vor der Gegner-Schleife: setzt im Hangar Birkin + Spider-Man und spielt die Wurfszene. */
void re15_bf_raumtick(void);
/* 1 = dieser Slot wird gerade von der Szene gefuehrt (KI-Tick ueberspringen). */
int  re15_bf_szene_besitzt(int slot);
void re15_bf_reset(void);                /* Raumwechsel */

/* Diagnose fuer Tests: Szenenphase (0 = wartet, 1 = Sprung, 2 = Griff, 3 = Sturz, 4 = liegt, 5 = vorbei). */
int  re15_bf_hangar_phase(void);
int  re15_bf_hangar_slot(int birkin);    /* 1 = Birkin-Slot, 0 = Spider-Man-Slot, -1 = keiner */

#endif
