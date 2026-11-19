/*
 * RE1.5 Port — Lautstaerke-Gesetz der libsnd (Strang "ton", Stufe 1). Herleitung, Adressen und Messung: re15_svm_vol.h.
 *
 * Alle Teilungen des Originals sind Ganzzahl-Teilungen nicht-negativer Werte (die Magic-Multiplikationen
 * 0x82061029 / 0x040c2051 = / 0x3f01, 0x02040811 = / 127, 0x04104105 = / 63, 0x00040011 = / 0x3fff). Mit Eingaben
 * 0..127 (alle 6254 Geraeusch-Schichten und alle Musik-Toene der Disc: Zensus scratchpad ton/s1/zensus_vol0.py) ist der
 * groesste Zwischenwert 127 * 127 * 0x3fff = 264241407 < 2^31, die Rechnung also exakt. Gerechnet wird in uint32_t,
 * damit ein Byte ausserhalb 0..127 (kommt in den Daten nicht vor) wie in den 32-Bit-Registern der PSX umlaeuft statt
 * in C undefiniert zu sein; das Ergebnis ist das 16-Bit-Register (`sh`).
 */
#include "re15_svm_vol.h"
#include "re15_actor.h"               /* g_actors[0] = Spieler (Original: Entity @0x800aca54, Lage +0x34 = 0x800aca88) */
#include "re15_room.h"                /* g_room_rdt / g_room_rdt_ok                                                   */
#include "re15_wassersprung_r41.h"    /* re15_wassersprung_r41_lage = FUN_80045a64 (Runde 41 Spur A)                  */

#define U_VOLL   ((uint32_t)RE15_SVM_VOLL)        /* 0x7f   */
#define U_MITTE  ((uint32_t)RE15_SVM_PAN_MITTE)   /* 0x40   */
#define U_REG    ((uint32_t)RE15_SVM_REG_VOLL)    /* 0x3fff */

/* Eine der drei Pan-Stufen (@0x80055e68-d8 Ton, @0x80055edc-0x80055f48 Programm, @0x80055f4c-b8 Taste). */
static void pan_stufe(uint32_t pan, uint32_t *l, uint32_t *r)
{
    if (pan < U_MITTE) *r = (*r * pan) / 0x3fu;                      /* `sltiu v0,v1,0x40`; rechts * Pan / 63 */
    else                          *l = (*l * (U_VOLL - pan)) / 0x3fu;     /* links * (0x7f - Pan) / 63             */
}

/* Produkt + Pan-Stufen + Mono, vor dem Quadrat (@0x80055d10-0x80055fe4). */
static void produkt(const re15_svm_vol_t *in, uint32_t *l, uint32_t *r)
{
    const uint32_t key = (uint32_t)in->key & 0xffu, bank = (uint32_t)in->vab_mvol & 0xffu;      /* `lbu` @0x80055d1c / @0x80055d24 */
    const uint32_t prog = (uint32_t)in->prog_mvol & 0xffu, ton = (uint32_t)in->tone_vol & 0xffu; /* `lbu` @0x80055d48 / @0x80055d6c */
    uint32_t a = (key * (bank * U_REG)) / 0x3f01u;                   /* @0x80055d1c-5c */
    a = (a * prog * ton) / 0x3f01u;                                              /* @0x80055d60-ac */
    *l = *r = a;                                                                 /* `addu a2,a3,zero` @0x80055e04 */
    if (in->ist_seq) {                                                           /* `beq a1,v0` @0x80055e00 (0x21 = keine Sequenz) */
        *l = (a * ((uint32_t)in->seq_l & 0xffffu)) / U_VOLL;              /* `lhu v0,116(v1)` @0x80055e08-50 */
        *r = (a * ((uint32_t)in->seq_r & 0xffffu)) / U_VOLL;              /* `lhu v0,118(v1)` @0x80055e28-64 */
    }
    pan_stufe((uint32_t)in->tone_pan & 0xffu, l, r);
    pan_stufe((uint32_t)in->prog_pan & 0xffu, l, r);
    pan_stufe((uint32_t)in->key_pan  & 0xffu, l, r);
    if (in->mono == 1) {                                                         /* `bne v1,v0` @0x80055fc8 */
        if (*l < *r) *l = *r; else *r = *l;                                      /* `sltu v0,a2,a1` @0x80055fd0-e4: der groessere */
    }
}

void re15_svm_keyon_reg(const re15_svm_vol_t *in, int *reg_l, int *reg_r)
{
    uint32_t l, r;
    produkt(in, &l, &r);
    *reg_l = (int)(((l * l) / U_REG) & 0xffffu);                     /* `mult a2,a2` @0x80055fe8, / 0x3fff, `sh a2` @0x80056058 */
    *reg_r = (int)(((r * r) / U_REG) & 0xffffu);                     /* `mult a1,a1` @0x80056008, / 0x3fff, `sh a1` @0x80056070 */
}

void re15_svm_keyon_reg_re2(const re15_svm_vol_t *in, int *reg_l, int *reg_r)
{
    uint32_t l, r;
    produkt(in, &l, &r);                                                         /* RE2 @0x8008376c-0x80083a28: dieselbe Kette */
    if (in->ist_seq) {                                                           /* RE2 `beq v1,v0,0x80083a98` @0x80083a38: Geraeusch ueberspringt */
        l = (l * l) / U_REG;                                         /* RE2 `mult a2,a2` @0x80083a40 */
        r = (r * r) / U_REG;                                         /* RE2 `mult a1,a1` @0x80083a50 */
    }
    *reg_l = (int)(l & 0xffffu);                                                 /* RE2 `sh a2,0(v0)` @0x80083ac4 */
    *reg_r = (int)(r & 0xffffu);                                                 /* RE2 `sh a1,0(v1)` @0x80083ac8 */
}

void re15_svm_utkeyon_split(int voll, int volr, int *key, int *key_pan)
{
    if (voll == volr)     { *key = voll; *key_pan = RE15_SVM_PAN_MITTE; }                  /* @0x80059e08-20 */
    else if (volr < voll) { *key = voll; *key_pan = (volr << 6) / voll; }                  /* @0x80059e2c-6c */
    else                  { *key = volr; *key_pan = RE15_SVM_VOLL - (voll << 6) / volr; }  /* @0x80059e78-bc */
}

int re15_svm_seq_key(int anschlag, int kanal_vol)
{
    return (anschlag * kanal_vol) / RE15_SVM_VOLL;                               /* `mult s5,v0` @0x80058508, 0x81020409 sra 6 */
}

int re15_svm_lage_spieler(int lage[2])
{
    if (!g_room_rdt_ok) return 0;
    const int32_t pos[3] = { g_actors[0].x, g_actors[0].y, g_actors[0].z };
    return re15_wassersprung_r41_lage(&g_room_rdt, pos, lage, 0);
}
