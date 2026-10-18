/*
 * RE1.5 Rebuilt — Runde 39 Spur E "gegner": Parking-Lot-Gorillas (Energie x2, Flinch erst nach 3 Pistolen-
 * Treffern), kein paralleler Hundebiss (RE2-Ein-Angreifer-Riegel), liegende Zombies kriechen (RE1.5-Wecker),
 * Gorilla-/Affen-Schatten auf der Boden-Referenz. Dossier: analysis/befunde_runde39/E_gegner.md.
 *
 * (1) GORILLA 0x27 in ROOM11C0 ("PARKING LOT", debug_jump_table.h; Sce_em_set 0x27 @0x01784/0x01798 Szene,
 *     @0x017E8/0x017FC Kampf). ROOM1141 traegt dieselben 0x27 — Nutzer: nur der Parking Lot.
 *     HP: INIT FUN_80116f50 liest die Zeile Typ 0x27 der HP-Tabelle (`jal 0x8001af20` @0x801170cc, Basis 0x8011f034
 *     @0x801170d4-d8, `andi 0xf` @0x801170e4, `sh v0,154(a1)` = +0x9a @0x80117104) = @0x8011f514: 16 x 180.
 *     NUTZER-VORGABE "doppelt so viel Energie" -> 2 x 180 = 360.
 *     Flinch: der Resolver schreibt +0x4 = 2 (HURT) bei JEDEM Treffer (@0x80012520); Spur 0 FUN_8011b018 spielt
 *     Clip 7 (`ori v1,zero,0x7 / sb v1,148(v0)` @0x8011b08c-90) und endet mit +0x93 = 0 (@0x8011b178), +0x4 = 1
 *     (@0x8011b188), +0x5 = 7 (@0x8011b194-98). Eine Schadens-Schwelle hat das Original nicht.
 *     NUTZER-VORGABE "erst die flinch Animation, wenn sie von der Energie her 3x mit der Handfeuerwaffe getroffen
 *     wurden": Schwelle = 3 x Pistolen-Schaden am Gorilla = 3 x 12 (@0x8006ee44 = 0x8006e0d0 + 0x27*0x58 + 3*4,
 *     Waffe 3 = Pistole, DAT_8006f430[0] = 3) = 36 Energie seit dem letzten Flinch. Darunter: Treffer ohne Zucken
 *     (Blut + Se 3 wie der Flinch-Eintritt @0x8011b0bc-138; der Original-Luftschuss ohne Flinch macht ebenfalls nur
 *     Blut @0x80118b78-bd0), das Gehirn laeuft mit den Unterzustaenden des letzten Ticks weiter, +0x93 = 0 wie
 *     der Flinch-Exit (@0x8011b178; sonst bliebe der Einmal-Riegel @0x8001300c zu). Jeder Treffer zaehlt fuer den
 *     Vergeltungs-Sprung der Runde 35 (3 Treffer -> Sprung, re15_affen_treffer_zaehlen) weiter wie bisher.
 * (2) HUND (RE2-Modul EMD0G_MOD0.BIN): Boden-Biss-Spielerhaken 0x801051BC schreibt PL +0x4 = 2 / +0x5 = dir+2
 *     (@0x801051e0-ec) und PL+0x1D3 |= 0x80 (`ori v1,v1,0x80` @0x801051f8 / `sb v1,467(a0)` @0x80105200);
 *     Kontakt-Tor `lbu v0,467(s1)` @0x80104e5c / `andi 0x80` @0x80104e64 / `bne` @0x80104e68 und Flug-Tor
 *     @0x80104d84-94 (-4) lassen keinen zweiten Hund an den beanspruchten Spieler. Freigabe durch den SPIELER:
 *     HURT [2]/[3] 0x8003f778 Phase 2 `sw a0(=1),4(s0)` @0x8003f8b0 / `sb zero,467(s0)` @0x8003f8b4 (ebenso
 *     [0]/[1] @0x8003f748, [4] @0x8003fa4c, [5] @0x8003fe68/@0x8003fec4). Port-Zwilling: der NORMAL-Prolog loescht
 *     +0x93 Bit 1 (@0x80031964) im Bild nach dem Flinch-Exit (state 1 @0x80035c80/db8) bzw. kd_ende.
 * (3) KRIECHER 0x81 (ROOM1010/1011 Typ 0x10, ROOM1220/1221 Typ 0x16): RE1.5 DECIDE[5] FUN_8010466C Block B
 *     `sltiu 0xbb8` @0x80104718 (dist < 3000) + arc 0x200 @0x8010472C-34 -> Wort 1 (@0x80104738/@0x80104748) =
 *     Kriech-Lokomotion. RE2 (EMZ0.BIN) kennt fuer den wartenden Kriecher (0x201 @0x80100B74, EXEC[2] @0x80103B48
 *     ohne Ausgang) nur den Griff-Wecker DECIDE[2] @0x80103A70. NUTZER-VORGABE "kriechen wie im Original":
 *     RE1.5-Wecker als zweiter Ausgang des RE2-WAIT, nach dem byte-true RE2-Griff-Entscheid.
 * (4) SCHATTEN: Gorilla-Wurzel FUN_80116db8 `lh a1,442(v0)` @0x80116f30 (+0x1ba Boden-Referenz) / `jal 0x8001b064`
 *     @0x80116f34; kleiner Affe 0x29 STAGE3.BIN @0x80110c78/@0x80110c7c; FUN_8001b064 `local_88.t[1] = param_2`
 *     = Quad-Hoehe. Port (main.c) nahm npc->y (Koerper). +0x1ba = Sce_em_set-Saat -(pc[4]*1800) @0x800421d4-
 *     0x80042210 (scd_vm.c dog_floor_y), bleibt waehrend der Ballistik (c1a4 landet auf +0x1ba).
 */
#ifndef RE15_GEGNER_R39_H
#define RE15_GEGNER_R39_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_R39_PARKPLATZ_RAUM      0x11C0u   /* "PARKING LOT" (debug_jump_table.h), beide 0x27-Varianten */
#define RE15_R39_AFFEN_HP_TABELLE    180       /* @0x8011f514 (Zeile 0x27, 16 Spalten gleich), Saat @0x80117104 */
#define RE15_R39_AFFEN_HP_FAKTOR     2         /* NUTZER-VORGABE "doppelt so viel Energie" */
#define RE15_R39_PISTOLE_SCHADEN     12        /* @0x8006ee44 (Zeile 0x27 @0x8006ee38, Waffe 3 = Pistole) */
#define RE15_R39_ZUCK_TREFFER        3         /* NUTZER-VORGABE "3x mit der Handfeuerwaffe" */
#define RE15_R39_ZUCK_SCHWELLE       (RE15_R39_ZUCK_TREFFER * RE15_R39_PISTOLE_SCHADEN)   /* 36 Energie */
#define RE15_R39_KRIECH_WECK_DIST    0xbb8u    /* sltiu 0xbb8 @0x80104718 */
#define RE15_R39_KRIECH_WECK_ARC     0x200     /* arc_test a3 = 0x200 @0x8010472C-34 */

/* NUR Sonden/Tests (Gegenprobe "vorher" im selben Lauf): 0 = alle Haken dieser Spur sind durchsichtig. */
void    re15_r39_gegner_enable(int an);

/* (1) Gorilla ROOM11C0 */
int     re15_r39_affen_parkplatz(const re15_actor_t *e);
int16_t re15_r39_affen_hp_init(const re15_actor_t *e, int16_t tabelle);   /* INIT: 2 x Tabellenwert, Akku-Bezug */
void    re15_r39_affen_merken(const re15_actor_t *e);                     /* Tick-Ende in Zustand 1: +0x5/+0x6/+0x7 */
int     re15_r39_affen_zucken_unterdruecken(re15_actor_t *e);             /* HURT-Eintritt: 1 = kein Flinch */
int     re15_r39_affen_akku(const re15_actor_t *e);                       /* Sonde: Energie seit dem letzten Flinch */

/* (2) Hund (RE2-Modul) */
int     re15_r39_hund_riegel(const re15_actor_t *pl);                     /* PL+0x1D3 & 0x80 @0x80104e5c-68 */
void    re15_r39_hund_riegel_setzen(re15_actor_t *pl);                    /* Boden-Biss @0x801051f8-0x80105200 */
void    re15_r39_hund_riegel_tick(void);                                  /* je Bild: Freigabe wie @0x8003f8b4 */
int     re15_r39_hund_riegel_vom_hund(void);                              /* Sonde: Riegel-Besitzer = Hund */

/* (3) Kriecher */
int     re15_r39_kriecher_weckt(const re15_actor_t *e, const re15_actor_t *pl);   /* RE1.5 Block B @0x80104718-48 */

/* (4) Schatten */
int     re15_r39_schatten_y(const re15_actor_t *npc, int32_t *y);        /* 0x27/0x29: Quad-Y = +0x1ba */

/* Mess-Schiene (kein Spielverhalten): RE15_R39_LOG=1 -> gegner_r39.log im Arbeitsordner. */
void    re15_r39_log(const char *fmt, ...);

#endif /* RE15_GEGNER_R39_H */
