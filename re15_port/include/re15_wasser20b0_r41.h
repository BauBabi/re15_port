/*
 * re15_wasser20b0_r41.h — Runde 41 Spur A "wasser20b0": der Wasserschwall der Flut-Szene ROOM20B0/20B1.
 *
 * Nutzer (AUFTRAG Runde 41): "in 20B0 laeuft ja das Wasser mit grossen Druck aus der Tuer heraus.
 * Uebrigens ist das in unseren Port noch kaputt!" Dossier analysis/befunde_runde41/A_wasser20b0.md.
 *
 * ORIGINAL (STAGE2.BIN, selbst disassembliert, re15_disasm.py --bin STAGE2.BIN; Original-RAM aus
 * DuckStation-Savestates ts_10/12/14/flut_a.sav, Dossier 1.3):
 *
 *   ROOM20B0 sub00 @0x1AA6 spawnt bei Ck(3,0xEB)==0 35 Entities Typ 0x24 (EM024) in einer Reihe
 *   x=7200 y=-1800 z=-8150..-4750 (Schritt 100) — grid 05 (32x), grid 01 (Satz 08/1A), grid 00 (Satz 11).
 *   EM024 ist KEIN Gegner, sondern ein unsichtbarer Partikel-Traeger:
 *     INIT 0x8010ef1c: *(+0x188) &= ~1 @0x8010efe4 = Bit 0 des Part-Worts (ZEICHENMASKE, re15_actor.h
 *       no_draw) geloescht -> das Mesh wird nie gezeichnet (Savestate: Part-Wort 0 = 0); +0x8c = 0x78;
 *       +0x5 := +0x9 (grid) @0x8010f00c; +0x1d0..+0x1ec = 0 @0x8010efa8; anim_set(.., 0, 0x200) @0x8010ef84
 *       (Original-RAM: +0x94 = 0, +0x95 = 1, KONSTANT ueber alle Messpunkte).
 *     ACTIVE 0x8010f020: jalr Tabelle @0x80118d58[+0x5] @0x8010f0a8 VOR der Drift; dann +0x8c++ @0x8010f0c8,
 *       x -= +0x8c @0x8010f0e8, x < -25000 -> x = 20000, +0x8c = 225 @0x8010f100-11c.
 *       Tabelle @0x80118d58: [0]=0x8010f130 [1]=0x8010f3fc [2]=0x8010f910 [3]=0x8010fe24 [4]=0x80110338
 *       [5]=[6]=[7]=0x8011084c (`jr ra`). 20B0/20B1 spawnen nur grid 0/1/5 (RDT-Bytes, beide identisch).
 *   grid 0 (0x8010f130, Phasen @0x80118d88[+0x6]): Phase 0 0x8010f178 / Phase 1 0x8010f2c0 — je 11 Takte
 *     (`sltiu 0xb` @0x8010f284/@0x8010f3cc), bei Takt 0 sechs Spawns (Tabelle unten), Phase 1 -> 0.
 *   grid 1 (0x8010f3fc, Phasen @0x80118d90[+0x6]): Phase 0 0x8010f444 (26 Takte, `sltiu 0x1a` @0x8010f610,
 *     Spawns bei Takt 0/3/6/9/12), Phase 1 0x8010f64c (11 Takte, leer), Phase 2 0x8010f6b0 (26 Takte, Spawns
 *     Takt 0/3/6/9/12), Phase 3 0x8010f8b8 (11 Takte, leer) -> 0.
 *   Spawn = 0x801108cc(x,y,z) (Versatz nach 0x8011931c/20/24) + 0x80110988(skala, a1, sub):
 *     a0 = 0x0f000000 | (sub<<16) | skala (`lui v0,0xf00` @0x801109b8) = Effekt 0x0f der Raum-Bank,
 *     a1 = s16 (slot+0x2e, Gier-Zuschlag der Zeilen-Euler-Y), a2 = *(+0x188)+0x40 (Teil-0-Weltmatrix:
 *     Rotation = Einheit, T = Entity-Lage + Knochen-0-Pose (0,-3329,0)), a3 = Versatz; `jal 0x80019700`
 *     @0x801109cc. Die Zufallsstreuung 0x80110904 rufen grid 0/1 NICHT.
 *   Effekt 0x0f (ROOM20B0.RDT idh @0x3450, eff_start 0x4bac, rowblk 0x4d18): je sub EIN Strom, 2 Zeilen —
 *     Zeile 0 Routine A=10 (Flags/TPAGE/CLUT/Anim-Start), Zeile 1 A=0 mit vel/acc (sub 0: (120,-80,0)/(0,10,0),
 *     sub 7: (70,0,0)/0). Die Zeilen-VM des Ports (re15_esp_fx_spawn_rows) fuehrt beides aus.
 *
 * AOT-Mechanik derselben Szene (Dossier 1.3): FUN_800436a8 (AUTO-Pass, a2 = 0, Spieler Pool 1
 * @0x800436bc-cc) laeuft UNBEDINGT je Bild aus der Hauptschleife (`jal 0x800436a8` @0x8001ce1c) — der
 * AUTO-Satz 7 (0x41 -> sub05: Westtuer fliegt, Set (3,0xEB)) feuert auch waehrend der Pad-Sperre (2,7).
 * Der Port blockierte in_cinematic (player_mode 2) jeden Nicht-RVD-Satz -> Szene haengt (Messung 1.1).
 */
#ifndef RE15_WASSER20B0_R41_H
#define RE15_WASSER20B0_R41_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_WASSER20B0_R41_EFFEKT      0x0Fu      /* `lui v0,0xf00` @0x801109b8 (a0 >> 24)            */
#define RE15_WASSER20B0_R41_SUB_GRID0   7          /* `ori a2,zero,0x7` @0x8010f1b0 ff. (grid 0)         */
#define RE15_WASSER20B0_R41_SUB_GRID1   0          /* `addu a2,zero,zero` @0x8010f47c ff. (grid 1)       */
#define RE15_WASSER20B0_R41_TAKTE_KURZ  11         /* `sltiu v0,v0,0xb`  @0x8010f284 / 0x8010f3cc / 0x8010f67c / 0x8010f8e8 */
#define RE15_WASSER20B0_R41_TAKTE_LANG  26         /* `sltiu v0,v0,0x1a` @0x8010f610 / 0x8010f87c       */
#define RE15_WASSER20B0_R41_KNOCHEN0_Y  (-3329)    /* Original-RAM flut_a/ts_10..16: Part-Matrix T.y = y - 3329 */

/* Ein Spawn-Satz: Versatz (0x801108cc), Skala + a1 + sub (0x80110988), Takt der Phase. */
typedef struct {
    uint8_t  takt;
    int16_t  x, y, z;
    uint16_t skala;
    int16_t  a1;
} re15_wasser20b0_r41_spawn_t;

/* INIT-Anteil (0x8010ef1c): Zeichenmaske loeschen (@0x8010efe4), Bild 1 (+0x95, Original-RAM). */
void re15_wasser20b0_r41_init(re15_actor_t *e);

/* Der +0x5-Dispatch (jalr @0x8010f0a8) VOR der Drift: grid 0/1 = Phasen-/Takt-Sequencer mit Spawns,
 * grid 2..4 = OFFEN (kein ausgeliefertes RDT spawnt sie), grid >= 5 = `jr ra`. Liefert die Zahl der
 * in diesem Takt gespawnten Plaetze. Felder: +0x5 = sub_state_1 (grid), +0x6 = sub_state_2 (Phase),
 * +0x1d0 = sub_state_3 (Takt). */
int  re15_wasser20b0_r41_sub(re15_actor_t *e);

/* Weltlage eines Versatzes: Teil-0-Weltmatrix (Pose Clip 0 Bild 1, Gier) * off + Lage = a2/a3 von
 * FUN_80019700. Liefert 0 (Pose aus der Gegnerbank) oder -1 (keine Bank: Rueckfall Lage + off). */
int  re15_wasser20b0_r41_anker(const re15_actor_t *e, const int32_t off[3], int32_t out[3]);

/* Haken fuer aot_common.c: AUTO-Ereignis-Saetze in ROOM20B0/20B1 feuern auch unter player_mode 2
 * (FUN_800436a8 @0x8001ce1c ohne Gate). 1 = Sperre uebergehen. */
int  re15_wasser20b0_r41_auto_aot(unsigned room_id, uint8_t aot_type);

/* Spawn-Tabellen (fuer Riegel): grid 0 Phase 0/1 je 6 Saetze, grid 1 Phase 0/2 je 9 Saetze. */
const re15_wasser20b0_r41_spawn_t *re15_wasser20b0_r41_tabelle(int grid, int phase, int *out_n);
int  re15_wasser20b0_r41_phasen_takte(int grid, int phase);   /* 11 / 26, -1 = keine Phase */

/* Mess-Schiene (Sonden/Riegel, kein Verhalten). */
typedef struct {
    unsigned spawns_grid0, spawns_grid1;   /* Aufrufe von re15_esp_fx_spawn_rows je grid            */
    unsigned plaetze;                      /* Summe der gespawnten Plaetze                           */
    unsigned ohne_bank;                    /* Spawns, die mangels Effekt 0x0f ausfielen             */
    int32_t  letzter_anker[3];             /* Weltlage des letzten Spawns                            */
    uint32_t letzter_code;                 /* a0 des letzten Spawns (Id<<24 | sub<<16 | Skala)       */
    int16_t  letztes_a1;
} re15_wasser20b0_r41_mess_t;
const re15_wasser20b0_r41_mess_t *re15_wasser20b0_r41_mess(void);
void re15_wasser20b0_r41_reset(void);

#endif /* RE15_WASSER20B0_R41_H */
