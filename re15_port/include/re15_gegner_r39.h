/*
 * RE1.5 Rebuilt — Runde 39 Spur E "gegner": Gorillas 0x27 (Energie x2, Flinch erst nach 3 Pistolen-Treffern; seit
 * Nachbesserung 2 fuer ALLE Gorillas), kein paralleler Hundebiss (RE2-Ein-Angreifer-Riegel), liegende Zombies kriechen
 * (RE1.5-Wecker), Gorilla-/Affen-Schatten auf der Boden-Referenz. Dossier: analysis/befunde_runde39/E_gegner.md.
 *
 * (1) GORILLA Typ 0x27 (STAGE1-Registrierung `lui v0,0x8011 / addiu v0,v0,28088` = Wurzel 0x80116db8 @0x8011e900-04,
 *     `sw v0,11336(at)` = Dispatch-Platz 0x80072c48 = 0x80072bac + 0x27*4 @0x8011e90c). Zensus Sce_em_set Typ 0x27 in
 *     allen 240 RDTs beider Szenarien (scd_dump_room.py; Abbrueche nur in 34 Stummel-RDTs zu 4 Byte): ROOM11C0 (Leon,
 *     "PARKING LOT": Szene @0x01784/@0x01798 grid 0x30, Kampf @0x017E8/@0x017FC grid 0x10) und ROOM1141 (Elza, Briefing
 *     Room: Szene @0x00B1A/@0x00B2E grid 0x30, Kampf @0x00B54/@0x00B68 grid 0x10; der Port-Ablauf BEFREIUNG setzt
 *     dieselben Saetze, gen/dining_r38_programme.inc). NUTZER-VORGABE (AUFTRAG.md Nachtrag 3, 2026-10-18: "Zu 2. ja,
 *     wir machen das Gorilla Verhalten generell so."): die Regel gilt fuer jeden Typ 0x27, in jedem Raum — kein Raum-Tor.
 *     Die kleinen Affen 0x29 (Stage 3-5, eigene Wurzel 0x80110b00, affen3_r38.c) bleiben unberuehrt.
 *     HP: INIT FUN_80116f50 liest die Zeile Typ 0x27 der HP-Tabelle (`jal 0x8001af20` @0x801170cc, Basis 0x8011f034
 *     @0x801170d4-d8, `andi 0xf` @0x801170e4, `sh v0,154(a1)` = +0x9a @0x80117104, ohne hp-Bedingung — der einzige
 *     Zweig des INIT ist grid&1 @0x80116f80) = @0x8011f514: 16 x 180.
 *     NUTZER-VORGABE "doppelt so viel Energie" -> 2 x 180 = 360.
 *     Flinch: der Resolver schreibt +0x4 = 2 (HURT) bei JEDEM Treffer (@0x80012520); Spur 0 FUN_8011b018 spielt
 *     Clip 7 (`ori v1,zero,0x7 / sb v1,148(v0)` @0x8011b08c-90) und endet mit +0x93 = 0 (@0x8011b178), +0x4 = 1
 *     (@0x8011b188), +0x5 = 7 (@0x8011b194-98). Eine Schadens-Schwelle hat das Original nicht.
 *     NUTZER-VORGABE "erst die flinch Animation, wenn sie von der Energie her 3x mit der Handfeuerwaffe getroffen
 *     wurden": Schwelle = 3 x Pistolen-Schaden am Gorilla = 3 x 12 (@0x8006ee44 = 0x8006e0d0 + 0x27*0x58 + 3*4,
 *     Waffe 3 = Pistole, DAT_8006f430[0] = 3; Elzas SIG P228 = Waffe 4 = ebenfalls 12 @0x8006ee48) = 36 Energie seit
 *     dem letzten Flinch. Darunter: Treffer ohne Zucken
 *     (Blut + Se 3 wie der Flinch-Eintritt @0x8011b0bc-138; der Original-Luftschuss ohne Flinch macht ebenfalls nur
 *     Blut @0x80118b78-bd0), das Gehirn laeuft mit den Unterzustaenden des letzten Ticks weiter, +0x93 = 0 wie
 *     der Flinch-Exit (@0x8011b178; sonst bliebe der Einmal-Riegel @0x8001300c zu). Jeder Treffer zaehlt fuer den
 *     Vergeltungs-Sprung der Runde 35 (3 Treffer -> Sprung, re15_affen_treffer_zaehlen) weiter wie bisher.
 *     Bezug des Akkus (PORT-WAHL der Buchfuehrung): HP beim INIT bzw. beim letzten Flinch; ein Aktor, dessen HP ueber
 *     dem Bezug liegt (Platz neu belegt ohne INIT), setzt den Bezug am Tick-Ende neu (HP steigen beim Gorilla nie).
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
 *     Der Schwanz laeuft auf JEDEM Pfad der Wurzel (Root-Skip `andi 0x20 / bne` @0x80116df4-f8 springt nur ueber
 *     Gehirn-Dispatch + Push-out auf 0x80116e88, VOR den Schwanz @0x80116f20-38; STAGE3 `bne` @0x80110b40 -> 0x80110bd0,
 *     Schwanz @0x80110c68-80): auch die Szenen-Gorillas (grid 0x30, geparkt -20000 / im Wagen -2500; INIT im Spawn-Bild
 *     @0x8004256c-70/@0x8004259c/@0x80042604-08) geben ihren Quad auf +0x1ba = -3600 mit Ecken 1700/1800 aus. Sichtbar
 *     wird er nur mit (x,z) im Region-Viereck des Cuts (FUN_8001b064: `lw a1,DAT_800ac790` @0x8001b180 / `jal 0x80014368`
 *     @0x8001b224 / `beq` @0x8001b22c) - Original-RAM: Cuts 0/1/3/4/13 nein, Cut 12 / 5 ja (dort in der Klappenoeffnung
 *     bzw. links ausserhalb des Bildes). Port: derselbe Cull (main.c npc_region_culled), kein Sonderweg (Nachbesserung 1;
 *     der Root-Skip-Haken des Zwischenstands c3823fa4 war am Byte falsch).
 */
#ifndef RE15_GEGNER_R39_H
#define RE15_GEGNER_R39_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_R39_AFFEN_TYP           0x27u     /* Gorilla: Wurzel 0x80116db8 -> Dispatch 0x80072c48 (@0x8011e900-0c); 0x29 = kleiner Affe, unberuehrt */
#define RE15_R39_AFFEN_HP_TABELLE    180       /* @0x8011f514 (Zeile 0x27, 16 Spalten gleich), Saat @0x80117104 */
#define RE15_R39_AFFEN_HP_FAKTOR     2         /* NUTZER-VORGABE "doppelt so viel Energie" */
#define RE15_R39_PISTOLE_SCHADEN     12        /* @0x8006ee44 (Zeile 0x27 @0x8006ee38, Waffe 3 = Pistole) */
#define RE15_R39_ZUCK_TREFFER        3         /* NUTZER-VORGABE "3x mit der Handfeuerwaffe" */
#define RE15_R39_ZUCK_SCHWELLE       (RE15_R39_ZUCK_TREFFER * RE15_R39_PISTOLE_SCHADEN)   /* 36 Energie */
#define RE15_R39_KRIECH_WECK_DIST    0xbb8u    /* sltiu 0xbb8 @0x80104718 */
#define RE15_R39_KRIECH_WECK_ARC     0x200     /* arc_test a3 = 0x200 @0x8010472C-34 */

/* NUR Sonden/Tests (Gegenprobe "vorher" im selben Lauf): 0 = alle Haken dieser Spur sind durchsichtig. */
void    re15_r39_gegner_enable(int an);

/* (1) Gorilla 0x27 (alle Raeume, beide Szenarien; Nachbesserung 2) */
int     re15_r39_affen_regel(const re15_actor_t *e);                      /* 1 = Typ 0x27 und Haken an */
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
int     re15_r39_schatten_y(const re15_actor_t *npc, int32_t *y);        /* 0x27/0x29 (auch grid&0x20): Quad-Y = +0x1ba */

/* Mess-Schiene (kein Spielverhalten): RE15_R39_LOG=1 -> gegner_r39.log im Arbeitsordner. Messhaken (nur Messlauf, Nachbesserung 2):
 * RE15_R39_MESS_SPIELER_HP=<n> hebt die Spieler-HP je KI-Bild auf >= n (gegner_r39.c r39_mess_spieler_hp; Begruendung im Dossier). */
void    re15_r39_log(const char *fmt, ...);

#endif /* RE15_GEGNER_R39_H */
