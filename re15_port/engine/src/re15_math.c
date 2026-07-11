/*
 * re15_math.c — byte-true PSX math primitives.
 *
 * re15_squareroot0: exact replica of the PsyQ BIOS SquareRoot0 (0x80065f60).
 *
 * The BIOS routine (disassembled 0x80065f60-0x80065fd0):
 *   v0 = LZCR(a0)                       ; mtc2 a0,$30 / mfc2 v0,$31  (GTE leading-zero count)
 *   if (v0 == 32) return 0              ; a0 == 0
 *   t2 = v0 & ~1                        ; round the count DOWN to even
 *   t1 = (31 - t2) >> 1                 ; denormalize shift
 *   t4 = (t2 >= 24) ? a0 << (t2-24)     ; normalize a0 into [64,256)
 *                   : a0 >> (24-t2)     ;   (drops the low mantissa bits — the source of the error)
 *   t5 = table[t4 - 64] << t1           ; lh @0x8007d984, sllv
 *   v0 = t5 >> 12                       ; srl  (table is Q12: table[i] = round(sqrt(64+i) * 512))
 *
 * The 192-entry table @0x8007d984 (PSX.EXE) — extracted verbatim. table[0]=4096=sqrt(64)*512,
 * table[64]=5792=sqrt(128)*512, table[191]=8175=sqrt(255)*512.
 */
#include "re15_math.h"

/* SquareRoot0 mantissa table @0x8007d984 (192 x u16, Q12: sqrt(64+i)*512). */
static const uint16_t re15_sqrt0_tab[192] = {
     4096,  4127,  4159,  4190,  4222,  4252,  4283,  4314,  4344,  4374,  4404,  4434,
     4463,  4492,  4521,  4550,  4579,  4608,  4636,  4664,  4692,  4720,  4748,  4775,
     4802,  4830,  4857,  4884,  4910,  4937,  4964,  4990,  5016,  5042,  5068,  5094,
     5120,  5145,  5170,  5196,  5221,  5246,  5271,  5296,  5320,  5345,  5369,  5394,
     5418,  5442,  5466,  5490,  5514,  5538,  5561,  5585,  5608,  5632,  5655,  5678,
     5701,  5724,  5747,  5769,  5792,  5815,  5837,  5860,  5882,  5904,  5926,  5948,
     5970,  5992,  6014,  6036,  6058,  6079,  6101,  6122,  6144,  6165,  6186,  6207,
     6228,  6249,  6270,  6291,  6312,  6333,  6353,  6374,  6394,  6415,  6435,  6456,
     6476,  6496,  6516,  6536,  6556,  6576,  6596,  6616,  6636,  6656,  6675,  6695,
     6714,  6734,  6753,  6773,  6792,  6811,  6830,  6850,  6869,  6888,  6907,  6926,
     6945,  6963,  6982,  7001,  7020,  7038,  7057,  7075,  7094,  7112,  7131,  7149,
     7168,  7186,  7204,  7222,  7240,  7258,  7276,  7294,  7312,  7330,  7348,  7366,
     7384,  7401,  7419,  7437,  7454,  7472,  7489,  7507,  7524,  7542,  7559,  7576,
     7594,  7611,  7628,  7645,  7662,  7680,  7697,  7714,  7731,  7747,  7764,  7781,
     7798,  7815,  7832,  7848,  7865,  7882,  7898,  7915,  7931,  7948,  7964,  7981,
     7997,  8014,  8030,  8046,  8062,  8079,  8095,  8111,  8127,  8143,  8159,  8175,
};

/* GTE LZCR: count of leading bits equal to bit31 (leading zeros for a positive
 * value, leading ones for a negative one). Squared distances are always >= 0 and
 * < 2^31 in reach, so this is a plain leading-zero count there. */
static int re15_lzcr(uint32_t x)
{
    uint32_t y = (x & 0x80000000u) ? ~x : x;
    if (y == 0) return 32;
    int n = 0;
    while ((y & 0x80000000u) == 0) { y <<= 1; n++; }
    return n;
}

uint32_t re15_squareroot0(uint32_t x)
{
    if (x == 0) return 0;
    int v0 = re15_lzcr(x);
    if (v0 == 32) return 0;                         /* x == 0 (or all-ones) */
    int t2 = v0 & ~1;                               /* even leading-zero count */
    int t1 = (31 - t2) >> 1;                        /* denormalize shift, in [0,15] */
    uint32_t t4 = (t2 >= 24) ? (x << (t2 - 24))     /* normalize into [64,256) */
                             : (x >> (24 - t2));
    uint32_t idx = t4 - 64;                         /* table index, guaranteed [0,191] */
    uint32_t t5 = (uint32_t)re15_sqrt0_tab[idx] << t1;
    return t5 >> 12;
}
