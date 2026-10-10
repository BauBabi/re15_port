/*
 * Runde 36 Spur F "zombieki" — Punkt 1: Sicht-Bit des RE2-Zombies (unten); Punkt 3: Gorilla (Typ 0x27, ROOM11C0)
 * Schatten-Record und Blutlache der Leiche. Dossier: analysis/befunde_runde36/F_zombieki.md.
 *
 * Nutzer: "Bei den Monkeys in ROOM 11C0 laeuft das Blut nicht aus wenn sie tot sind."
 *
 * ORIGINAL (STAGE1.BIN, selbst disassembliert mit re15_disasm.py):
 *   INIT FUN_80116f50: Schatten-Record FUN_8001af5c(a0=0, a1=0, a2 = rec+6 + 100, a3 = rec+6 + 200,
 *     sp+16 = entity+0xB0, sp+20 = 0x80808080) @0x801171cc-f0; rec = *(entity+0x78) =
 *     Tabelle 0x80121368[+0x1e2 = 4 (@0x80117098-9c)] = 0x80121350 -> u16 @0x80121356 = 1600
 *     (Hitbox-Halbbreite). FUN_8001af5c legt a2/a3 als `sh s3,12(s0)` @0x8001b030 / `sh s4,14(s0)`
 *     @0x8001b038 auf entity+0xBC / +0xBE (s0 = sp+16 = entity+0xB0). => +0xBC = 1700, +0xBE = 1800.
 *     (Der Zombie uebergibt an derselben Stelle 0x258/0x2bc = 600/700 @0x80100c2c/c38 — die Port-
 *     Konvention nhx/nhz der Halbmasse ist damit dieselbe.)
 *   LEICHE FUN_8011be54 (Zustand 7): +0x7 == 0: +0x9e = 0x5a @0x8011be80-84, +0x7 = 1 @0x8011be90-94,
 *     Wort0 |= 2 @0x8011beac, |= 0x40 @0x8011bec8; Durchfall; +0x7 == 1: Tinte +0xC4 / +0xEC =
 *     (alt & 0xff000000) | 0x00ffff38 @0x8011bed0-f00, **+0xBC += 8 / +0xBE += 8 @0x8011bf08-14**,
 *     +0x9e-- (alt == 0 -> +0x7 = 2) @0x8011bf24-44; +0x7 >= 2: `jr ra` @0x8011bf48.
 *   Der Port fuehrte die Leiche bisher nur mit dem Zaehler (+0x9e) — Tinte und Wachstum standen als
 *   Kommentar, wurden aber nie geschrieben, und der Renderer zeichnete fuer 0x27 den 500x600-Standard-
 *   Charakterschatten (dieselbe Fehlerklasse wie der Hund in Runde 34).
 */
#ifndef RE15_ZOMBIEKI_R36_H
#define RE15_ZOMBIEKI_R36_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_R36_AFFEN_HITBOX_HALB   1600   /* u16 @0x80121356 (Record 0x80121350[+6], Tabelle 0x80121368[4]) */
#define RE15_R36_AFFEN_SCHATTEN_X    (RE15_R36_AFFEN_HITBOX_HALB + 100)   /* `addiu a2,a3,100` @0x801171e8 */
#define RE15_R36_AFFEN_SCHATTEN_Z    (RE15_R36_AFFEN_HITBOX_HALB + 200)   /* `addiu a3,a3,200` @0x801171f0 */
#define RE15_R36_AFFEN_LACHE_SCHRITT 8      /* `addiu v0,v0,8` / `addiu v1,v1,8` @0x8011bf08-0c */
#define RE15_R36_AFFEN_LACHE_TAKTE   0x5a   /* `ori v0,zero,0x5a` / `sb v0,158(a0)` @0x8011be80-84 */

/* INIT-Haken (re15_maggot_ai_tick Zustand 0): Schatten-Record +0xBC/+0xBE wie FUN_8001af5c @0x801171cc-f0. */
void re15_r36_affen_lache_init(re15_actor_t *e);

/* Leichen-Haken (re15_maggot_ai_tick Zustand 7, waehrend +0x7 == 1): Tinte + Wachstum @0x8011bed0-bf14.
 * Der Zaehler +0x9e bleibt im Aufrufer (unveraendert). */
void re15_r36_affen_lache_tick(re15_actor_t *e);

/* Renderer-Haken (platform/pc/main.c Schattenquad): liefert fuer Typ 0x27 Halbmasse und Lachenfarbe.
 * Rueckgabe 1 = uebernommen (nhx/nhz gesetzt; *pool = 1 mit Farbe 0x00ffff38 -> r 0x38 g 0xff b 0xff
 * wie der RE1.5-Hund @0x801117F0), 0 = nicht zustaendig. */
int re15_r36_affen_lache_render(const re15_actor_t *npc, int32_t *nhx, int32_t *nhz, int *pool,
                                uint8_t *sh_r, uint8_t *sh_g, uint8_t *sh_b);

/* Nur fuer Sonden: Leichen-Haken abschalten (Gegenprobe). */
void re15_r36_affen_lache_enable(int an);

/* ---------------------------------------------------------------------------------------------------------
 * Punkt 1 — "sie bleiben hier ewig Beteiligungslos stehen, obwohl ich ziemlich in Ihrer Naehe vor ihnen bin"
 *
 * MESSUNG (test_r36_zombieki_sicht, Stand v0.8.23): 45 stehende Zombies, 2653 Leon-Plaetze im Vorderkegel bis
 * 4000; der Zell-Rechteck-Strahl re15_re2_los_cells_blocked (Slab-Test gegen JEDE u0&4-Zelle, Port-Mapping der
 * Kraehen-Flugkopplung 3486be93, 2026-09-08, vom Zombie mitgelesen) nahm an 306 Plaetzen (11,5 %) die Sicht, an
 * denen der RE1.5-Sichtstrahl (Nibble 3) frei ist; an 68 davon (ROOM10B0, Terrassenkante #9 Nibble 1) blieb der
 * Zombie 480 Bilder stehen. Bei freier Sicht stand KEINER.
 *
 * ORIGINAL RE2 (Ziel, Beta -> Retail): der Zombie-Navigator FUN_8004A808 setzt +0x154&0x800 aus dem Strahl
 *   FUN_80050858(selfPart, playerPart, a2 = 0x2000 `li a2,0x2000` @0x8004a868, a3 = 1 `li a3,1` @0x8004a86c,
 *   jal @0x8004a8d8), `andi v0,v0,0xf7ff` @0x8004a87c, `ori v0,v0,0x800` @0x8004a8f0 bei Rueckgabe 0.
 *   FUN_80050858 nimmt NUR Saetze mit Attribut & 0x2000 (`and`/`beq` @0x800508bc-c8) und ueberspringt die Formen
 *   {1..5,13} (Tabelle @0x800A73B4 = [0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0], `lbu` @0x800508d0-e4); mit a3 = 1 prueft er
 *   zusaetzlich die HOEHE des Satzes (-(w>>11)*100 - ((w>>6)&0x1f)*1800 @0x80050b34-74) gegen den Strahl zwischen
 *   den Koerperteilen -> niedrige Saetze (Tische, Tresen, Kanten) nehmen keine Sicht.
 * ORIGINAL RE1.5 (Datenseite der Raeume): der RE1.5-Sensor FUN_8001b84c ruft FUN_8003dcc4 mit `ori a2,zero,0xf00`
 *   @0x8001b8d4 / `ori a3,zero,0x300` @0x8001b8ec (Delay-Slot des jal @0x8001b8e8): es blocken NUR Zellen, deren
 *   Boden-Nibble (w5 & 0xf00) == 0x300 ist (`and v1,t5,a0` @0x8003de7c, `bne v1,v0,LAB_8003e0a8` @0x8003de94;
 *   Band-Test `lbu v1,0x82(v1)` / `sra v0,v0,0x1c` / `bne` @0x8003de60-6c). Das Nibble ist der von Capcom je Zelle
 *   gesetzte Sicht-Blocker (Waende 3; Tresen ROOM1030 #18 = 0, Terrassenkante ROOM10B0 #4/#9 = 1) — das RE1.5-
 *   Gegenstueck zum RE2-Attribut 0x2000 + Hoehenpruefung.
 * PORT (Mapping, benannt): das Sicht-Bit des RE2-Zombies kommt NUR aus dem RE1.5-Strahl FUN_8003dcc4 (4 Regionen,
 *   Aktor-Maske wie Runde 35 Spur H); der Slab-Test bleibt der Kraehe (Flugkopplung) und dem Zellenarm.
 * --------------------------------------------------------------------------------------------------------- */

/* Sicht-Bit +0x154&0x800 des RE2-Zombies (Haken in enemy_ai_re2_zombie.c): 1 = Sicht frei. */
int re15_r36_zombie_sicht_frei(re15_actor_t *e, re15_actor_t *pl);

/* Nur fuer Sonden: 0 = alter Weg (re15_re2_los_clear mit Slab-Test) fuer die Gegenprobe im selben Lauf. */
void re15_r36_zombie_sicht_enable(int an);

/* enemy_ai_common.c: nur der RE1.5-Sichtstrahl FUN_8003dcc4 (Zellen mit Boden-Nibble 3, 4 Regionen,
 * re15_los_ray_blocked, Aktor-Maske) von e nach (px,pz); 1 = verdeckt. */
int re15_r36_los_ray_nib3(const re15_actor_t *e, int32_t px, int32_t pz);

/* ---------------------------------------------------------------------------------------------------------
 * Punkt 2 — "Die Monkeys scheinen noch oft blockiert zu sein. Das muessen wir irgendwie verbessern."
 *
 * MESSUNG (test_r36_zombieki_affen_block fest ..., Stand nach Punkt 1): ein Gorilla in der JAGD (Sub 3), dessen
 * Sichtlatch +0x1d0 faellt (Wagenblock #2/#7 = Nibble 3 dazwischen) und der an der Wandklemme haengt, bleibt
 * 1200/1200 Bilder in Sub 3 an derselben Stelle (z.B. Leon (-7000,12500), Gorilla (-14000,-3000): Abstand 8415
 * -> 8476, Klemme 1200, Sicht 0, nur Sub 3) — das ist das "Blockiert" des Nutzers.
 * ORIGINAL (STAGE1.BIN, A[3] @0x80117b40-c74, selbst disassembliert): die Entscheidung der Jagd kennt nur die
 *   Ausgaenge Biss (a804(0xbb8,0x180) @0x80117a60-74), Wiedereintritt mit Sicht (`sltiu v0,v0,0xbb8` + `andi 1` +
 *   `lbu 7` @0x80117b60-98 bzw. `sltiu v0,v0,0x1771` @0x80117bc8-c08) und FERN + SICHT -> Auswahl Sub 4
 *   (`sltiu v0,v0,0x1771` / `bne` @0x80117c34-38, `andi v0,v0,0x1` / `beq` @0x80117c48-4c, `ori v0,zero,4` /
 *   `sb v0,5(v1)` @0x80117c50-54). Fuer "keine Sicht" gibt es KEINEN Ausgang; B[3] steuert dabei immer auf
 *   playerX (`addiu a0,a0,-13688` = 0x800aca88 @0x80117d4c, `jal 0x8001a8f8` @0x80117d50) — gegen den Wagen.
 *   Die Wagen ueberquert das Original NUR per Sprungpad aus Sub 4 (A[4] @0x80117ecc-8011802c: FUN_8003b93c attr
 *   0x10, Yaw-Fenster `((zc&0xf0)<<4 - Yaw + 512) & 0xfff < 1024` @0x80117f18-38, +0x7 = 1 @0x80117f50-74).
 *   Der Blind-Zonensprung auf der Stelle ist Original (Runde 35 Spur J, GDB 400/400).
 * PORT-WAHL (NUTZER-VORGABE "irgendwie verbessern", kein Original-Verhalten): in der Jagd OHNE Sicht und MIT
 *   Wandklemme (+0x1d6 != 0, dasselbe Flag, das A[3] @0x80117b18-20 und A[4] @0x80118054-5c lesen) steuert
 *   B[3] statt auf playerX auf die Mitte des naechsten Sprungpads (SCA-Zelle u1 == 0x10, Band des Gorillas — die
 *   Daten, die A[4] abfragt); steht er auf dem Pad (FUN_8003b93c attr 0x10 trifft) und im Yaw-Fenster, fuehrt
 *   er den 0x10-Sprung des Originals aus (+0x9f = Code, +0x5/6/7 = 7/0/1 @0x80117f40-74, Flug = B[7] wie im
 *   Original); steht er falsch gedreht darauf, dreht er in die
 *   Pad-Richtung ((zc&0xf0)<<4, die Richtung, die das Fenster verlangt). Sicht zurueck -> sofort wieder
 *   Original. Keine neue Zahl: Radius box[6] + 100 (@0x80117ee0-f00), Fenster und Impuls bleiben die des
 *   Originals; 3000 ist nur die Laenge des Richtungs-Zielpunkts (beliebig > 0, a8f8 nutzt nur den Winkel).
 *   GEMESSEN UND VERWORFEN (test_r36_zombieki_affen_block riegel, je 3 Szenen + Zufallsgang): (a) die Suche
 *   auch in der AUSWAHL (Sub 4, B[4]) mit den 0x20-Pads (Blind-Sprung @0x80118000-24) -> Endlos-Sprungketten
 *   (19-22 Spruenge je 1500 Bilder, Leon nie erreicht); (b) Sub 4 nur mit 0x10-Pads -> Hin-und-her am selben
 *   Pad (Szene 1: 15 Spruenge, kein Angriff); (c) dazu "zuletzt benutztes Pad meiden" -> Zufallsgang 1116
 *   blockierte Bilder, 1 Griff (vorher 0 / 6-9). Die AUSWAHL (Sub 4) bleibt deshalb Original (A[4] ohne Sicht
 *   ohne Ausgang — OFFEN, s. Dossier).
 * --------------------------------------------------------------------------------------------------------- */

/* Haken in re15_maggot_ai_tick A[3] (nach dem Biss-Test, vor B[3]); sub = 3 (andere Werte: Rueckgabe 0, keine
 * Wirkung). Rueckgabe 1 = der 0x10-Sprung (Sub 7, +0x7 = 1) wurde ausgeloest. */
int re15_r36_affen_padsuche_decide(re15_actor_t *e, int slot, int sub);

/* Haken in B[3]: Steuerziel (playerX oder Pad-Mitte bzw. Punkt in Pad-Richtung); 1 = Pad-Suche aktiv. */
int re15_r36_affen_padsuche_ziel(const re15_actor_t *e, int slot, int sub, int32_t plx, int32_t plz, int32_t *tx, int32_t *tz);

/* Nur fuer Sonden: Pad-Suche abschalten (Gegenprobe = Original-Verhalten). */
void re15_r36_affen_padsuche_enable(int an);
int  re15_r36_affen_padsuche_aktiv(int slot);

#endif /* RE15_ZOMBIEKI_R36_H */
