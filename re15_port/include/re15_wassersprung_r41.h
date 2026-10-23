/*
 * re15_wassersprung_r41.h — Runde 41 Spur A "wasser20b0", Punkt 2: der Wassereffekt beim Sprung von der Kante
 * ins Wasser (Spieler-Landung) und die Wasser-Schritte der NPCs.
 *
 * Nutzer (AUFTRAG Runde 41): "In STAGE2 fehlt noch der Wassereffekt vom rein springen von der Kante in das
 * Wasser. Auch die NPCs teilen die gleichen Effekte wenn sie durch das Wasser gehen."
 * Dossier analysis/befunde_runde41/A_wasser20b0.md (Punkt 2).
 *
 * ORIGINAL (info/Re1.5/PSX.EXE, selbst disassembliert):
 *
 *  (a) LANDUNG der Kletter-/Sprung-Maschine, Phase 4 @0x800386d4 (climb_common.c case 4):
 *        +0x38 := +0x1ba (Bodenhoehe)                           `lh v0,442(v1)` / `sw v0,56(v1)` @0x80038724-2c
 *        v1 = Spieler+0x88 (Wasserstempel, s16)                 `lh v1,-13604(v1)` @0x8003873c (0x800acadc)
 *        a0 = Spieler+0x38 (y)                                  `lw a0,-13684(a0)` @0x80038744 (0x800aca8c)
 *        v1 = (Stempel < y)                                     `slt v1,v1,a0`    @0x8003874c
 *        beq v1,zero -> keine Partikel                          @0x80038758
 *        Schleife s1 = 4..1 (@0x8003875c `ori s1,zero,0x4`, @0x800387c4-d0 Zaehler), je Durchlauf ZWEI Spawns:
 *          a0 = 0x06033000 (`lui a0,0x603` / `ori a0,a0,0x3000` @0x8003876c-70) = Effekt 6, Sub 3, Skala 0x3000
 *          a1 = +0x6a (Gier) + ((s1-2) << 8)                   @0x8003877c-98 (s0 = (s1&0xff)-2 << 8)
 *          a2 = Spieler+0x20 (Entity-Matrix: T = Lage)         `addiu s3,a2,25` @0x80038760 (a2 = 0x800aca5b -> 0x800aca74)
 *          a3 = 0x8007428c = (0,0,0,0) (Nullversatz)           @0x80038764-68 (`read 0x8007428c 4 --w 4`: 0,0,0,0)
 *          jal 0x80019700                                      @0x80038794
 *          zweiter Spawn gleich, a1 += 2048                     `addiu a1,a1,2048` @0x800387c0, jal @0x800387bc
 *        => 8 Plaetze, Gier rot_y + {512, 2560, 256, 2304, 0, 2048, -256, 1792}, nur wenn der Boden unter
 *           dem Wasserspiegel liegt (Stempel < y; PSX y nach unten). Der Stempel des Vorbild-Scans steht in der
 *           Landung noch (Loeschung erst @0x80031da4 NACH dem Executor-jalr @0x80031cb4 in FUN_80031c44).
 *
 *  (b) NPC-SCHRITTE: die NPC-Geh-Subs 4/5 (FUN_800512xx / 0x800515xx) lesen das Bildwort *(+0x168)
 *        `lw v0,360(v0)` / `lw v1,0(v0)` / `andi v0,v1,0x4000` @0x80051278-88 (Sub 4) bzw. @0x800515e4-f4 (Sub 5)
 *        und rufen bei gesetztem Bit FUN_80045630(a0, 7 - 3*((Wort>>12)&1)): a0 = 0 (Sub 4 @0x80051290) bzw.
 *        a0 = 1 (Sub 5 @0x800515fc), jal @0x800512a8 / @0x80051614; Phase 2 @0x800512d4-308 / @0x800519dc-a10 /
 *        @0x80051bd4-c08 analog. FUN_80045630 fuehrt VOR der Typweiche denselben Effekt-Zweig wie beim Spieler
 *        (re15_wasser_r38.h: Bodentyp FUN_800437d4, Bit 0x80 -> 0x06050D00 an Part[fuss]+0x40 mit Gier +0x6a,
 *        sonst Stempel +0x88 != 0 -> 0x06032000 an Part[fuss-1]+0x40 mit Gier 0x800, Versatz (0,100,0)) und nimmt
 *        fuer Typ >= 0x40 (`sltiu v0,v0,0x40` @0x80045730) den NPC-Zweig: Tonindex = a0 + (Boden & 0x7f) - 8
 *        (`addiu s3,a0,-8` @0x8004575c), ohne das Objekt-Override (@0x80045770-90).
 *        Der Stempel +0x88 wird je Entity im Entity-Takt NACH dem Dispatch geloescht (`sh zero,136(v0)`
 *        @0x8001a59c hinter `jalr` @0x8001a588) und vom AUTO-Pass (FUN_800436a8, Pool 2 @0x80043720-28) neu
 *        gestempelt — der Schritt sieht den Stempel des Vorbilds, wie beim Spieler.
 */
#ifndef RE15_WASSERSPRUNG_R41_H
#define RE15_WASSERSPRUNG_R41_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_WASSERSPRUNG_R41_CODE      0x06033000u   /* `lui a0,0x603` / `ori a0,a0,0x3000` @0x8003876c-70 */
#define RE15_WASSERSPRUNG_R41_PLAETZE   8             /* s1 = 4..1 x 2 Spawns @0x8003875c / @0x800387c4-d0   */
#define RE15_WASSERSPRUNG_R41_NPC_TON   8             /* `addiu s3,a0,-8` @0x8004575c                          */

/* (a) Landung: Spieler p auf Bodenhoehe (p->y = +0x1ba), stempel = Spieler+0x88 des Vorbild-Scans, t_y = y der
 * Entity-Matrix +0x20 (zuletzt gezeichnete Lage, vor dem Bodenschreiben @0x8003872c). Spawnt die 8 Plaetze bei
 * (p->x, t_y, p->z), wenn stempel < p->y. Liefert die Zahl der Spawn-Aufrufe (0 oder 8). */
int re15_wassersprung_r41_landung(const re15_actor_t *p, int16_t stempel, int32_t t_y);

/* (b) NPC-Schritt: FUN_80045630(a0, fuss) fuer einen Aktor vom Typ >= 0x40 (Effekt-Zweig + Ton -8).
 * rdt = Raum (Bodentyp), fuss = 7/4 (Part-Index). Liefert den Tonindex (oder -1 ohne Ton). */
int re15_wassersprung_r41_npc_schritt(re15_actor_t *e, int a0, int fuss, const void *rdt);

/* Haken im NPC-Geh-Sub (Phase 1 und 2): Bildwort-Test + Schritt wie @0x80051278-a8. Liefert 1, wenn ein
 * Schritt lag. Muss VOR dem Bildvorschub dieses Takts laufen (Bildwort des gezeichneten Bilds). */
int re15_wassersprung_r41_npc_takt(re15_actor_t *e, int a0);

/* Je Bild (Bildanfang): Folge-Plaetze der NPC-Schritte nachfuehren (Flags-Bit 0x04 @0x80019f54-a4). */
void re15_wassersprung_r41_bildanfang(void);

typedef struct {
    unsigned landungen;        /* Landungen mit Spawns                      */
    unsigned landung_trocken;  /* Landungen ohne (Stempel >= y)             */
    unsigned npc_schritte;     /* NPC-Schritte gesamt                       */
    unsigned npc_boden;        /* davon Zweig Bit 0x80 (0x06050D00)         */
    unsigned npc_stempel;      /* davon Zweig Stempel (0x06032000)          */
    unsigned plaetze;          /* gespawnte Plaetze gesamt                  */
    int32_t  letzter_anker[3];
    int16_t  letzte_gier[RE15_WASSERSPRUNG_R41_PLAETZE];
    int      letzter_ton;
} re15_wassersprung_r41_mess_t;
const re15_wassersprung_r41_mess_t *re15_wassersprung_r41_mess(void);
void re15_wassersprung_r41_reset(void);

#endif /* RE15_WASSERSPRUNG_R41_H */
