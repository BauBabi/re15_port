/*
 * RE1.5 Rebuilt — RE2-ZELLENARM (RE2 Typ 0x2D "Zombie-Arme im Fenster", ROOM2050) als
 * Gehirn fuer die ROOM1210-Gitterhaende (RE1.5-Typ 0x1A) unter dem RE2-Flavor.
 *
 * Runde 16 / Phase 2 (2026-09-19), Dossier analysis/befunde_2026-09-19/arme-1210-re2.md.
 * Alle Original-Adressen beziehen sich auf das RE2-Retail-Overlay CDEMD0_EM2D_ai1.BIN
 * (CDEMD0.EMS Sektor 0x9B0, 0x1528 B, geladen @0x80100000; Volltext
 * analysis/befunde_2026-09-19/arme-1210-re2_em2d_ai1.dis) bzw. auf info/re2leon/PSX.EXE.
 * Definition + Belege: engine/src/enemy_ai_re2_zellenarm.c.
 */
#ifndef RE15_ENEMY_AI_RE2_ZELLENARM_H
#define RE15_ENEMY_AI_RE2_ZELLENARM_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy.h"     /* re15_enemy_bank_t (Runde 42 Stufe 2: Modell-Anmeldung) */
#include "re15_skeleton.h"  /* re15_skel_pose_t (Runde 42 Stufe 2 Runde 2: Zeichen-Lage) */

/* 1 = dieser Aktor gehoert dem RE2-Zellenarm-Gehirn (Typ 0x1A UND RE2-Flavor fuer 0x1A). */
int  re15_re2arm_owns(const re15_actor_t *e);

/* Der ganze Tick (Root @0x80100018 + Routinen-Tabelle @0x80101424). Rueckgabe 1 = uebernommen,
 * 0 = nicht mein Aktor (der Aufrufer faehrt dann den RE1.5-Writher). */
int  re15_re2arm_tick(int slot);

/* ENEMSE-Wiedergabe (PC registriert; engine bleibt link-sauber). bank_fn wird SOFORT mit der
 * Bank des Arms gerufen, se_fn spaeter mit (id, flag2000=1) — Belege an der Definition. */
void re15_re2arm_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank));

/* Zeichner-Seite: Bitmaske der Parts (Bone-Index), die NICHT gezeichnet werden — die sieben
 * Bones des inaktiven Arms (INIT `sw zero,0(part)` @0x80100240 / @0x801002F8). Der ganze
 * Aktor ist ausserdem ueber e->no_draw verborgen, solange Entity-Bit 2 gesetzt ist
 * (0x80101164 mit a2 = 0). Mit dem RE1.5-Modell EM1A (Runde 42 Stufe 2, Runde 3): 0 = alle vier Teile (im Original
 * stehen die Zombies sichtbar hinter dem Gitter; die Maske 0x0003 aus Runde 2 beruhte auf einer falschen Aussage). */
uint16_t re15_re2arm_part_hide_mask(const re15_actor_t *e);

/* Mess-/Test-Auskunft: Heimat (+0x218 Yaw, +0x21C x, +0x220 z) und der globale Griff-
 * Cooldown 0x800CFBF4 (== g_re2_room_gflags, EIN Wort mit Hund/Kraehe). */
void re15_re2arm_home(int slot, int16_t *yaw, int32_t *x, int32_t *z);
int  re15_re2arm_holder_slot(void);      /* -1 = kein Arm haelt den Spieler */
/* Runde 35 Spur H, Nachbesserung 2: Hand der GEMISCHTEN Parts (+0x14E-Ueberblendung wie 0x80029614,
 * = Pin-Quelle @0x80100C18-38) und die reine Keyframe-Hand derselben Parts-Pose; 1 = Parts gueltig. */
int  re15_re2arm_hand_parts(int slot, int32_t gemischt[3], int32_t rein[3]);
int  re15_re2arm_hand_bone(const re15_actor_t *e);   /* EM2D: 3 (Arm A) / 10 (Arm B); EM1A: 2 / 3 */

/* Runde 42 Strang "original" Stufe 2 (NUTZERWUNSCH 18.11.2026: "...du solltest EIGENTLICH NUR die funktionalitaet aus
 * Resident Evil 2 übernehmen, nicht das Modell. Das Modell darf schon noch das aus Resident Evil 1.5 sein"):
 * Der Lader (platform/pc/main.c) laedt fuer Typ 0x1A das RE1.5-Modell EM1A als Zeichen-Bank und meldet es hier an.
 * `em2d_logik` ist die RE2-Bank EM2D (CDEMD0.EMS kind 0x2D), nur noch als Takt (Cliplaengen Paar 1) und als Quelle
 * fuer Leons Opfer-Clips (Paar 3); NULL = ohne RE2-Archiv (Takt dann auf den EM1A-Laengen). Belege am Modul. */
void re15_re2arm_modell_einrichten(re15_enemy_bank_t *anzeige, const re15_enemy_bank_t *em2d_logik);
/* 1 = dieser Arm zeichnet das RE1.5-Modell EM1A (4 Knochen), 0 = RE2-EM2D (15 Knochen). */
int  re15_re2arm_modell_re15(const re15_actor_t *e);
/* RE2-Clip (0..5, CDEMD0_EM2D_ai1.BIN) -> EM1A-Clip (0..3); -1 ausserhalb. */
int  re15_re2arm_em1a_clip(int re2_clip);
/* RE2-Takt des Arms (Logik-Clip/-Bild), Test-/Mess-Auskunft. 1 = Slot gueltig. */
int  re15_re2arm_takt(int slot, int *re2_clip, int *re2_bild);
/* Runde 3: Zeichen-Lage des EM1A (Belege am Modul, Block MODELL: WAS DAS ORIGINAL ZEIGT / KOERPER-WEG / ZEICHNEN).
 * Der Koerper (Rumpf, Kopf, zweiter Arm) steht an der RE1.5-Stelle (Record-Lage + B[1]-Weg ab dem Ausloeser), der RE2-Arm
 * sitzt an dessen Schultergelenk und zeigt mit dem Handgelenk auf die RE2-Hand (gestreckt, wenn er zu kurz ist).
 * zeichen_teil: 1 = rot/trans (Welt) gesetzt, 0 = nicht dieser Weg, -1 = Teil nicht zeichnen; streck = Faktor Q12.
 * koerper_lage: 1 = Koerper wird gezeichnet, p = Welt-Lage. koerper_info: Mess-/Test-Auskunft (bild -1 = vor dem Ausloeser). */
int  re15_re2arm_zeichen_teil(const re15_actor_t *e, int teil, const re15_skel_pose_t *poses, int n,
                              int32_t rot[9], int32_t trans[3], int32_t *streck);
int  re15_re2arm_koerper_lage(const re15_actor_t *e, int32_t p[3]);
int  re15_re2arm_koerper_info(int slot, int *bild, int *clip, int *frame, int32_t rec[3]);
/* Runde 35 Spur H, Nachbesserung 3: RE2-Koerper-Push FUN_80034D0C(Arm, Spieler) mit dem Arm-Segment
 * r 800 / Halbhoehe 500 (@0x80100328-64) gegen das Spieler-Segment r 450 / Y -1530 / 1530 — aus dem
 * Spieler-Pass (FUN_800355C4 @0x80026628). 1 = geschoben. */
int  re15_re2arm_body_push_player(re15_actor_t *e, re15_actor_t *pl);
/* 1 genau einmal nach einem Pin B4 P0 (@0x80100C18-38) in diesem Bild — der Spieler-Pass schiebt im
 * Original im SELBEN Bild nach dem Pin. */
int  re15_re2arm_take_pin_bild(void);
/* Runde 35 Spur H NB4: RE2-Zielwahl fuer Leons Blick (FUN_8003DB38 @0x8003c1b4), jedes Bild aus dem
 * Spieler-Pass; verbraucht nur im Opfer-Zustand des RE2-Arms. look_debug: Test-/Mess-Auskunft. */
void re15_re2arm_player_look(re15_actor_t *pl);
typedef struct { uint8_t aktiv; uint16_t f10e; uint8_t sicht_frei; int32_t x, z; } re2look_kand_t;
int  re15_re2arm_look_waehle(const re15_actor_t *pl, int n, const re2look_kand_t *k);   /* -1 = SELBST */
void re15_re2arm_look_debug(int set_cd, int set_ziel, int *cd, int *ziel);

#endif /* RE15_ENEMY_AI_RE2_ZELLENARM_H */
