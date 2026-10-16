/*
 * RE1.5 Rebuilt — Runde 38 Spur S "affen3": die kleinen Affen (Typ 0x29, EM029) treffen wie im Original.
 *
 * Nutzer (AUFTRAG.md Runde 38): "In Stage 3 die kleinen Affen koennen einen nicht treffen, es passiert nichts".
 * Dossier mit Messungen und allen Belegen: analysis/befunde_runde38/S_affen3.md.
 *
 * WER: Typ 0x29 = der kleine Affe. Gegnermodell-Lader PSX.EXE FUN_80022300: Typ-0x10 @0x80022338 ->
 *      Tabelle @0x80072f38, Eintrag 25 @0x80073000 = Sektor 994 / 0x364d8 Byte = CDEMD0.EMS Blob 13 (ein
 *      Menschenaffe; "Cockroach" im Port ist nur der RE2-Name). Handler STAGE3.BIN 0x80110b00 (Registrierung
 *      @0x8011cf38), wortgleich in STAGE4.BIN 0x8010c1a0 (@0x801183e8) und STAGE5.BIN 0x8010c320 (@0x8011dd48).
 *      Der Code ist die Gorilla-Familie 0x27 (STAGE1 0x80116db8, re15_affen.h) mit anderen Konstanten.
 *
 * WAS FEHLTE (gemessen, ROOM3040): der Biss (Sub 5) pruefte im Port den MITTEN-Abstand < 1000 (der schwere
 *      Angriff < 800); die Koerperschiebung haelt Affe (Kasten 1100 @0x8011ec14) und Leon (450 @0x80073e94)
 *      auf >= 1550 -> nie ein Treffer. Das Original prueft ein achsenparalleles QUADRAT um einen gezeichneten
 *      Knochenpunkt (FUN_8001bff8 @0x8001c080-c0c0) und schiebt den Affen per Fuss-Sperre (FUN_80115b68 /
 *      FUN_80115c3c) in den Sprung hinein.
 *
 * DIESES MODUL faehrt das ACTIVE-Gehirn 0x8011101c fuer die Subs 0..6 (A[+0x5] und B[+0x5] im SELBEN Tick,
 *      `jalr` @0x8011110c / @0x80111140). Subs 7..15 (Sprung/Flug) bleiben beim bisherigen Code in
 *      enemy_ai_common.c re15_cockroach_ai_tick. Die Knochenkette, der Pool und der Zeichenstand kommen aus
 *      affen_11c0.c (re15_affen_pool_anim / _zeichen_merk / _fusssperre / _trefferpunkt), die seit Runde 38
 *      die Bank des Aktor-Typs nehmen (vorher fest 0x27).
 */
#ifndef RE15_AFFEN3_R38_H
#define RE15_AFFEN3_R38_H

#include <stdint.h>
#include "re15_actor.h"

#define RE15_AFFEN3_TYP             0x29u
#define RE15_AFFEN3_MASSSTAB        0x14cc   /* +0x166 `ori v0,zero,0x14cc` @0x80110f54-58, Flag +0x0|=0x800 @0x80110f44-48 */

/* B[5] BISS 0x80111d68 */
#define RE15_AFFEN3_BISS_CLIP       0x12     /* `ori v0,zero,0x12` / `sb v0,148` @0x80111dd8-ddc */
#define RE15_AFFEN3_BISS_RECORD     9        /* `addiu a0,s2,1612` @0x80111eb8: 1612 = 9*0xac + 0x40 */
#define RE15_AFFEN3_BISS_VERSATZ    0x64     /* a1.vx `ori v0,zero,0x64` / `sw v0,16(sp)` @0x80111e78-7c */
#define RE15_AFFEN3_BISS_R          0x3e8    /* `ori a2,zero,0x3e8` @0x80111ec0 */
#define RE15_AFFEN3_BISS_SCHADEN    5        /* `addiu v0,v0,-5` @0x80111f60 */
#define RE15_AFFEN3_BISS_SE         5        /* `ori a0,zero,0x5` @0x80111f4c -> jal 0x800453d0 @0x80111f6c */
#define RE15_AFFEN3_BISS_SE_AUSHOL  9        /* bei +0x95 == 0xb `ori a0,zero,0x9` @0x80111e90-a0 */
/* B[6] SCHWERER ANGRIFF 0x80112044 */
#define RE15_AFFEN3_SCHWER_CLIP     0x13     /* @0x801120b8-bc */
#define RE15_AFFEN3_SCHWER_REC_A    6        /* `addiu a0,s0,1096` @0x801121ec: 6*0xac + 0x40 */
#define RE15_AFFEN3_SCHWER_REC_B    10       /* `addiu a0,s0,1784` @0x801121f0: 10*0xac + 0x40 */
#define RE15_AFFEN3_SCHWER_R        0x320    /* `ori a2,zero,0x320` @0x801121a4 / @0x801121f8 */
#define RE15_AFFEN3_SCHWER_SCHADEN  10       /* `addiu v0,v0,-10` @0x801122b4 */
#define RE15_AFFEN3_SCHWER_SE       4        /* `ori a0,zero,0x4` @0x80112290 -> jal 0x800453d0 @0x801122c0 */
#define RE15_AFFEN3_SCHWER_SE_AUSHOL 9       /* bei +0x95 == 0x14 @0x80112170-84 */
/* Sperre +0x1dc */
#define RE15_AFFEN3_SPERRE_TREFFER  0x2d     /* @0x80111f68 / @0x801122bc */
#define RE15_AFFEN3_SPERRE_ENDE     0x14     /* @0x80111fe4 / @0x80112398 */

/* INIT-Haken (Zustand 0, 0x80110c98): Massstab 1,3 und der erste Pool-Stand (anim_set @0x80110ed0). */
void re15_affen3_init(re15_actor_t *e);

/* ACTIVE-Haken (Zustand 1, 0x8011101c). dist = +0x1d4 (SquareRoot0 @0x801110c8), sicht = +0x1d0 & 1 (Latch
 * @0x8011105c-88). Rueckgabe 1 = A und B dieses Ticks liefen hier; 0 = Sub >= 7, der bisherige Code faehrt. */
int  re15_affen3_aktiv(int slot, int32_t dist, int sicht);

/* FUN_8001bff8(Pool + record*0xac + 0x40, a1, r, &Spieler+0x34): 1 = Treffer (Quadrat |dX|,|dZ| <= r um den
 * gezeichneten Knochenpunkt). 0 auch, wenn noch kein Zeichenstand existiert. */
int  re15_affen3_quadrat(const re15_actor_t *e, const re15_actor_t *pl, int record, int32_t r);

/* Mess-/Riegel-Zugang: Welt-XZ des Trefferpunkts (Record + a1) wie FUN_8001bff8 ihn bildet; 0 = kein Stand. */
int  re15_affen3_trefferpunkt(const re15_actor_t *e, int record, int32_t out[3]);

/* Mess-/Riegel-Zugang: Treffer-Ereignisse (je Treffer ein Eintrag), zuruecksetzbar. */
typedef struct {
    uint8_t  slot, sub, clip, bild;   /* Affe, +0x5 (5/6), +0x94, +0x95 nach dem Vorschub */
    int16_t  schaden, hp_nach;        /* abgezogene HP, Spieler-HP danach */
    int16_t  abstand;                 /* Mitten-Abstand Affe-Spieler im Treffer-Tick */
} re15_affen3_treffer_t;
#define RE15_AFFEN3_LOG_N 32
void re15_affen3_log_reset(void);
int  re15_affen3_log(re15_affen3_treffer_t *out, int max);

#endif /* RE15_AFFEN3_R38_H */
