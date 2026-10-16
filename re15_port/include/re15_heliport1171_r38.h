/*
 * RE1.5 Rebuilt — HELIPORT ROOM1171 (Runde 38 Spur I). Nutzer-Befund (AUFTRAG.md, woertlich):
 *   "Elza stuck in ihrer Cutscene im ROOM 1170."
 * Dossier mit allen Messungen und Belegen: analysis/befunde_runde38/I_heliport1171.md.
 *
 * WAS DAS ORIGINAL TUT (selbst disassembliert, STAGE1.BIN roh @0x80100000, PSX.EXE):
 *   SZENE    ROOM1171.RDT sub07 @0x016C4 Ck(3,0xC2)==1 -> Sce_em_set John 0x49 @0x016C8 + ZWEI Kraehen
 *            Typ 0x21 beh 0x40 @0x016DC/@0x016F0 + Evt_exec sub02 @0x01704. sub02 wartet @0x013F4-0x013FE
 *            `While Ck(5,0x1C)==0`. Die Kraehen-INIT @0x80112408 schickt grid&0x40 in Zustand 4 (FLIGHT-2).
 *   GRIFF    FLIGHT-2 HOVER 0x80115130: Kontakt (+0x1d0 @0x80115254) und aca58 != 5 (@0x80115268-70) ->
 *            acbfc = Kraehe @0x8011527c, aca5a = 0 @0x80115288, acbcc/acbd0 = Kraehe+0x178/+0x17c
 *            @0x80115290/@0x801152b4, aca58 = 5 @0x801152a0, aca59 = 1 @0x801152ac (RUECKgriff); immer
 *            `jal 0x80115d74(2)` @0x801152b8 und SET(0x800b1028, 0x1c) `jal 0x8004ef90` @0x801152c4 =
 *            Spiel-Flag (5,0x1C) (Flag-Tabelle @0x80074664[5] = 0x800b1028). KEIN HP-Write.
 *   WURF     Kraehen-Wurzel @0x801120a0-e8: TEST(0x800b1028,0x1d) && aca58==5 && aca5a<3 -> aca5a = 3.
 *            Die Szene setzt (5,0x1D) @0x01404, 60 Bilder nach dem Griff.
 *   OPFER    Hook A [0x21] -> Tabelle @0x8012123c[aca59=1] = 0x80115b80, Phasen @0x80100304:
 *            P0 @0x80115bb8 +0x8f=7 @0x80115bd0, Clip 0 @0x80115bec, Bild 0 @0x80115bf4, aca5a++ @0x80115c00,
 *               Se_on(0x03020001) @0x80115c04, faellt in P1. KEIN +0x93, KEIN aca3c (anders als vorn).
 *            P1 @0x80115c0c f314 (Paar C) @0x80115c20; Umlauf -> Se_on @0x80115c3c, Clip 1 @0x80115c54,
 *               Bild 0 @0x80115c5c, aca5a++ @0x80115c68.
 *            P2 @0x80115c74 f314 @0x80115c88 (Halte-Schleife), Umlauf -> Se_on @0x80115ca4.
 *            P3 @0x80115cb4 Clip 2 @0x80115cc4, Bild 0 @0x80115ccc, aca5a++ @0x80115cd8, faellt in P4.
 *            P4 @0x80115cdc f314 @0x80115cf0; Umlauf -> aca58 = 4 @0x80115d04, aca59 = 6 @0x80115d10,
 *               `sh zero` aca5a @0x80115d18 = Kommando 4 / Modus 6 (0x800517f0: PLW-Clip 1, dann Clip 2 als
 *               Schleife; Ende = naechster Plc-Befehl des Skripts, ROOM1171 @0x014E2 Plc_dest Modus 5).
 *   SE       Se_on 0x03020001: FUN_80045024 Bank `srl v1,a0,24` @0x80045028 = 3 -> Tabelle @0x80010e70[3] =
 *            0x800450f8 (`sltiu v0,s4,0x19`, EDT = [0x800ac778]+0x14 @0x80045104-10 = Raum-snd1), Satz
 *            `srl v0,a0,16; andi 0xff` @0x80045078-7c = 2 -> im Port re15_audio_room_se(2).
 *
 * WAS IM PORT FEHLTE (gemessen, Dossier Abschnitt 1): (a) das Kraehen-Flagwort war ein PRIVATES
 *   s_crow_gflags statt Bank 5 -> (5,0x1C) wurde nie gesetzt, (5,0x1D)/(5,0x1F) des Skripts kamen nie an;
 *   (b) im Standard-KI-Modus (RE2) uebernahm das RE2-Kraehenhirn die Ereignis-Kraehen (EMOVL21_S0.BIN ruft
 *   keinen RE2-Flag-Helfer 0x8007730c/0x80077334/0x80077360) -> sie kamen nie; (c) der Rueckgriff zog 8 HP ab
 *   (re15_crow_hit_player, im Original kein HP-Write) und hatte keine Opfer-Animation, keinen Wurf.
 *
 * PORT-WAHL (belegte Form): die Ereignis-Kraehe (Typ 0x21, +0x9 & 0x40 — game-weit NUR ROOM1171 sub07,
 *   RDT-Zensus) laeuft in JEDEM KI-Modus ueber das RE1.5-Hirn und die RE1.5-Bank (Muster "0x26 nach
 *   HERKUNFT", main.c / enemy_ai_re2_dog.c). RE2 hat kein Gegenstueck; die Szene ist in RE1.5 fertig.
 */
#ifndef RE15_HELIPORT1171_R38_H
#define RE15_HELIPORT1171_R38_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_R38I_KRAEHE_TYP     0x21u  /* ROOM1171 @0x016DC/@0x016F0 pc[2] `21`                           */
#define RE15_R38I_EREIGNIS_BIT   0x40u  /* pc[3] = beh `40` -> +0x9; INIT @0x80112408 (grid&0x40 -> Zust. 4)  */
#define RE15_R38I_ZBANK          5      /* Flag-Tabelle @0x80074664[5] = 0x800b1028                         */
#define RE15_R38I_BIT_GRIFF      0x1Cu  /* SET @0x801152c4 (a1 = 0x1c @0x801152c8); Ck @0x013F8              */
#define RE15_R38I_BIT_WURF       0x1Du  /* TEST @0x801120a8 (a1 = 0x1d @0x801120ac); Set @0x01404             */
#define RE15_R38I_SE_SATZ        2      /* Se_on 0x03020001 Satz `srl a0,16; andi 0xff` @0x80045078-7c       */
#define RE15_R38I_BLEND          7      /* +0x8f = 7 @0x80115bd0                                            */
#define RE15_R38I_CLIP_INTRO     0      /* @0x80115bec `sb zero,acae8`                                      */
#define RE15_R38I_CLIP_HALTEN    1      /* @0x80115c54 `sb v0(=1),acae8`                                    */
#define RE15_R38I_CLIP_WURF      2      /* @0x80115cc4 `sb v0(=2),acae8`                                    */
#define RE15_R38I_PHASE_WURF     3      /* Wurzel @0x801120e4-e8 `sb v0(=3),aca5a`                          */
/* Laengen der drei Opfer-Clips (Paar C = EM021 dir[5]) AUS ELZAS ARCHIV: der Modell-Lader FUN_80022300 nimmt
 * bei gesetztem Elza-Bit EMD/CDEMD1.EMS (`bltz` @0x80022334 -> @0x800223ac), und die Szene gibt es nur in
 * ROOM1171. CDEMD1 EM021 dir[5] = 14 / 31 / 30 Bilder (Leons CDEMD0: 14 / 36 / 20); der Riegel
 * test_r38_heliport1171 banken liest beide Archive nach. Eingebettet wie s_crow_clip_len im Kraehenhirn, damit
 * die Maschine auch ohne geladene Bank die echten Laengen hat; mit Bank gelten die Bank-Laengen. */
#define RE15_R38I_LEN_INTRO      14
#define RE15_R38I_LEN_HALTEN     31
#define RE15_R38I_LEN_WURF       30

/* 1 = Ereignis-Kraehe (Typ 0x21 mit +0x9 & 0x40). */
int  re15_r38i_ereigniskraehe(const re15_actor_t *e);

/* HOVER-Kontakt 0x80115248-b4: der Rueckgriff (aca58=5/aca59=1), nur wenn kein Griff laeuft. */
void re15_r38i_rueckgriff(re15_actor_t *kraehe, re15_actor_t *pl);

/* Kraehen-Wurzel @0x801120a0-e8: (5,0x1D) && Griff && Phase < 3 -> Phase 3. */
void re15_r38i_wurfstoss(void);

/* Hook A [0x21][aca59=1] = 0x80115b80 im Spieler-Takt (game_step nach re15_player_victim_tick). */
void re15_r38i_rueckgriff_tick(void);

/* Raum-Init: Kommandowort weg (Raumlader FUN_800314b0 `sw zero,0x800aca58` @0x80031518). */
void re15_r38i_install(uint16_t room);

/* Mess-API (Riegel): Phase -1 = aus, 0..4 = aca5a; Zahl der Rueckgriffe; Zahl der Wuerfe (Modus 6). */
int  re15_r38i_phase(void);
int  re15_r38i_griffe(void);
int  re15_r38i_wuerfe(void);

#endif
