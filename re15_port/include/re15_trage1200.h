/*
 * RE1.5 Rebuilt — ROOM1200: der Zombie von der Trage (Runde 35 Spur H, Punkt 3).
 *
 * Nutzer (AUFTRAG.md Runde 35): "Bei ROOM 1200 nach dem aufnehmen des Minidisc players steht der
 * Zombie von der Trage auf und laeuft durch die Luft, statt wie im Original danach auf die Spieler
 * Ebene runter zu kommen."  Dossier: analysis/befunde_runde35/H_raeume.md, Punkt 3.
 *
 * GEMESSEN (Basisstand, echte exe, Minidisc-Player per Aktion aufgenommen, RE15_GEGNER_Y_LOG):
 * der Liege-Zombie auf der Bahre (Sce_em_set id 1 @RDT 0x0086A: Typ 0x10, grid 0x87, Band pc[4]=1,
 * y=-1800 — sub03 weckt ihn mit grid 0x89) steht auf und laeuft in BEIDEN KI-Geschmaeckern auf
 * y=-1800 / Band 1 durch den Raum, nie herunter; der Schubladen-Zombie (id 2, grid 0xA1 -> 0x81 in
 * sub02) kriecht unter RE1.5 ebenso auf y=-1800.
 *
 * ORIGINAL-MECHANISMUS = die Engine-Schwerkraft FUN_8001bd60 (PSX.EXE, selbst disassembliert).
 * Die Zombie-Wurzel FUN_80100424 ruft sie JEDES Bild vor dem Zustands-Dispatch:
 *     801004dc  addiu a0,zero,-10      (Delay-Slot)   a0 = Grundschritt -10
 *     80100514  jal   0x8001bd60
 *     80100518  ori   a1,zero,0x14                    a1 = Beschleunigung 20
 * (weitere Aufrufer 0x8010a9b8 Zombie-Maedchen, 0x8011c5ec/0x8011cbbc/0x8011d1e4/0x8011d778/
 *  0x8011dcc4/0x8011e29c NPC-/Typ-0x47-Wurzeln — der Port fuehrte sie als "look helper, deferred").
 * FUN_8001bd60(a0, a1):
 *   8001bd64-80  DAT_800aca3c & 0x4000 (Spieler steht auf einem Objekt) -> nur der Fall-Teil
 *   8001bd94-a8  r = FUN_8003b7f0(&+0x34, -(*(+0x78)+6), +0x82)   Zellen-Attributwort am Platz,
 *                eigenes Band, Rechteck um den Radius VERKLEINERT (`subu a1,zero,a1` @0x8001bda8)
 *   8001bdb0-b4  r & 2 == 0 -> kein Absturz
 *   8001bdc8-ec  nur wenn y == -(+0x82 * 1800) (steht genau auf seinem Band)
 *   8001bdf0-14  +0x1c0 = 0x8000 | ((r & 0xc) >> 2) << 13
 *   8001be18-54  do { +0x1ba += 1800; +0x82 -= 1; } while (n-- != 0)   (n+1 Baender)
 *   8001be64-70  +0x1c0 & 0x8000 (faellt):
 *   8001be7c-a4  y += a0 + a1 * (+0x1c0 & 0x1fff); +0x1c0 += 1
 *   8001beb4-e8  wenn +0x1ba < y: y = +0x1ba, +0x1c0 &= 0x7fff (gelandet)
 * Die Zellen (ROOM1200 SCA, eigener Dump): Band-1-Zellen 11/12/13/17 tragen das Attribut 0x02
 * (Wort 0x1302 = Absturzkante, n = 0 -> ein Band), die Tische/Schubladen sind Band-1-Bloecke 0xFF.
 * Der Zombie faellt also, sobald er von der Bahre in eine Kantenzelle laeuft, mit -10, +10, +30 ...
 * auf +0x1ba = 0 = die Spieler-Ebene.
 *
 * +0x1ba seedet Sce_em_set FUN_800420a0 mit -(pc[4]*1800) (Faktorfolge @0x800421f8-0x8004220c,
 * `sh v0,442(s0)` @0x80042210); der Port fuehrt +0x1ba als e->dog_floor_y (re15_actor.h) und
 * +0x1c0 als e->fall_1c0. Radius *(+0x78)+6 = 400 fuer Typ 0x10 (+0x78 = 0x8011f778 @0x80100770-78,
 * Wort +6 = 0x0190) = e->hit_radius_min.
 */
#ifndef RE15_TRAGE1200_H
#define RE15_TRAGE1200_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_SCHWERKRAFT_ZOMBIE_A0   (-10)   /* `addiu a0,zero,-10` @0x801004dc */
#define RE15_SCHWERKRAFT_ZOMBIE_A1   0x14    /* `ori a1,zero,0x14`  @0x80100518 */
#define RE15_SCHWERKRAFT_BAND        1800    /* `addiu v0,v0,1800` @0x8001be2c / Faktorfolge @0x8001bdd0-e8 */
#define RE15_SCHWERKRAFT_FAELLT      0x8000u /* @0x8001bdf0 / `andi 0x8000` @0x8001be6c */

/* FUN_8001bd60(a0, a1) fuer die Entity e — byte-true, s. o. `radius` = *(+0x78)+6. Liefert 1, wenn
 * die Entity in diesem Bild faellt oder gelandet ist (nur fuer Messung/Test). */
int re15_schwerkraft_8001bd60(re15_actor_t *e, int32_t a0, int32_t a1, int32_t radius);

/* +0x1ba-Seed aus Sce_em_set FUN_800420a0: -(pc[4]*1800) (`lbu v1,2(s2)` @0x800421d4, Faktorfolge
 * @0x800421f8-0x8004220c, `sh v0,442(s0)` @0x80042210). Das Original seedet jede Entity; der Port-Platz
 * dog_floor_y dient bei der RE2-Spinne/-Kraehe aber als +0x1C2 (andere Adresse, enemy_ai_re2_spider.c /
 * enemy_ai_re2_crow.c), der Hund seedet ihn im INIT selbst. Deshalb NUR die Typen der Zombie-Wurzel
 * FUN_80100424 (0x10/0x11/0x12/0x16/0x18, enemy_ai_common.c run_all), deren Wurzel die Schwerkraft ruft. */
void re15_schwerkraft_seed(re15_actor_t *e);

/* Messschiene RE15_GEGNER_Y_LOG=<datei> (env-gegatet, kein Spielverhalten): je Spielbild eine Zeile
 * je aktivem Gegner — Typ, Zustand, grid, Clip/Bild, x/y/z, Band (+0x82). Gerufen aus der
 * PC-Hauptschleife hinter re15_game_step. Auf PSX ein No-op. */
void re15_trage1200_mess(void);

#endif /* RE15_TRAGE1200_H */
