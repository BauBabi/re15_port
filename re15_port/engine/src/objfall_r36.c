/*
 * RE1.5 Rebuilt — OBJEKT-FALL der Typ-4-Objekte (Kisten/Tritte). Runde 36 Spur A, Nachbesserung 3.
 *
 * Herleitung der Haken und Felder: include/re15_objfall_r36.h. Jede Konstante unten traegt die
 * Adresse ihrer Instruktion in info/Re1.5/PSX.EXE (Auslieferungsstand), selbst disassembliert
 * (re15_disasm.py dis 0x8002bd44 / 0x8002c40c / 0x8002c444 / 0x8002d3d8 / 0x8002dacc /
 * 0x80068098 / 0x800684b4 / 0x80065d90 / 0x800661c0 / 0x80066120 / 0x8001c6e8 / 0x8003b7f0 /
 * 0x80042bac / 0x800436a8). Das Decompilat RE_15_Quellcode_V2/FUN_8002c444.c wurde gegen die
 * Bytes gelesen und stimmt im Ablauf; massgeblich sind die Instruktionen.
 *
 * FUN_8002c444 (a0 = s1 = OBJEKT), Zustand obj+5:
 *   0 @0x8002c49c-4f8  Init der Fall-Globalen (DAT_8008f67c.. = eine Fallbewegung zur Zeit, wie im
 *                      Original GLOBAL, nicht je Objekt), obj+5 = 1, obj+6 = 0, faellt in 1 durch.
 *   1 @0x8002c4fc-968  Vorschub x/z += Tempo * Richtung, Kippen bis 1024 (90 Grad), Absinken; ab
 *                      Bild 13 freier Fall (+160 je Bild), bei Bild 13 Bodenhoehe FUN_8001c6e8 und
 *                      NEUES BAND obj+0x82 = -(Boden/1800) (@0x8002c6c4), Wasser-Spritzer, Landung.
 *   2 @0x8002c96c-a0c  Aufsetzen (Landeton ohne Wasser), 1 Bild hoch, obj+5 = 3.
 *   3 @0x8002ca10-a88  Nachfedern, obj+4 = 0 (Fall vorbei).
 *   immer @0x8002ca8c  FUN_8002dacc(obj, &DAT_8008f698) — Drehung um den Bild-Winkelschritt.
 *
 * NICHT PORTIERT (benannte Reste, kein Ersatzwert):
 *   - FUN_8002d3d8 @0x8002d454-468 schreibt obj+0x68/+0x6c nach DAT_800b282c/DAT_800b2838 — ohne
 *     Leser: Scan aller lh/lhu/lw/sh/sw mit Offset 0x282c/0x2838 in PSX.EXE + STAGE1-6.BIN findet
 *     nur diese beiden Stores (Ghidra-XREF ebenso nur W).
 *   - @0x8002c6ac-b0 obj+0x40/+0x44 = (u16)x/z (Vorlage) — der Port fuehrt keine Vorlage-Felder
 *     fuer Props; Leser ist die Eingrenzung FUN_8002cfd4 (@0x8002d074/@0x8002d088), die nur fuer
 *     ein Objekt laeuft, AUF dem der Spieler steht — nie fuer ein fallendes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_objfall_r36.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_audio.h"       /* re15_audio_room_se_snd0 = FUN_80045024 Bank 2 */
#include "re15_collision.h"   /* floor_typeword = FUN_8003b7f0, room_coll = FUN_8001c6e8 */
#include "re15_engine.h"      /* g_engine.frame_count — nur Messschrieb */
#include "re15_esp.h"         /* re15_esp_fx_spawn_rows = FUN_80019700 */
#include "re15_math.h"        /* re15_ratan2 = 0x80065de0, re15_squareroot0 = 0x80065f60 */
#include "re15_room.h"        /* g_current_room_id — nur Messschrieb */
#include "re15_skeleton.h"    /* re15_sin_q12/re15_cos_q12 = Tabelle 0x800794c4 */

extern scd_vm_t g_scd;

/* ---- Zustand je Pool-Eintrag (obj+4/+5/+6/+0x88) ---------------------------------------- */
static uint8_t s_fall[RE15_SCD_MAX_PROPS];     /* obj+0x04: 1 = faellt (@0x8002c148)        */
static uint8_t s_zust[RE15_SCD_MAX_PROPS];     /* obj+0x05: 0..3                            */
static uint8_t s_zaehl[RE15_SCD_MAX_PROPS];    /* obj+0x06: Bildzaehler des Falls           */
static int16_t s_wasser[RE15_SCD_MAX_PROPS];   /* obj+0x88: Wasserspiegel (0 = keins)       */

/* ---- die GLOBALEN Fallgroessen des Originals (eine Fallbewegung zur Zeit) ----------------- */
static int16_t s_richt_x, s_richt_z;   /* DAT_800b526c / DAT_800b527c (FUN_8002d3d8)            */
static int16_t s_67c;                  /* DAT_8008f67c Vorschub je Bild                         */
static int16_t s_680;                  /* DAT_8008f680 Vorschub-Zuwachs                         */
static int16_t s_684;                  /* DAT_8008f684 Kipp-Winkelschritt                       */
static int16_t s_688;                  /* DAT_8008f688 Kipp-Zuwachs                             */
static int16_t s_68c;                  /* DAT_8008f68c Fallgeschwindigkeit                      */
static int16_t s_690;                  /* DAT_8008f690 aufsummierte Kippung                     */
static int16_t s_694;                  /* DAT_8008f694 Bodenhoehe (FUN_8001c6e8)               */
static int16_t s_drehung[3];           /* DAT_8008f698/69a/69c = SVECTOR fuer FUN_8002dacc      */
static uint8_t s_6a0;                  /* DAT_8008f6a0 Spritzer schon gefallen                  */

static int      s_handler_lief;        /* PORT: Typ-Handler in diesem Bild schon gelaufen       */
static unsigned s_n_ausloeser, s_n_landungen, s_n_spritzer;

/* ---- Messschrieb RE15_OBJFALL_LOG=<datei> (Diagnose, kein Verhalten) --------------------- */
static FILE *objfall_log(void)
{
    static int init = 0;
    static FILE *f = NULL;
    if (!init) {
        init = 1;
        const char *p = getenv("RE15_OBJFALL_LOG");
        if (p && *p) f = fopen(p, "w");
    }
    return f;
}

/* ================================================================================================
 *  libgte-Zwillinge (nur fuer FUN_8002dacc)
 * ================================================================================================ */

/* RotMatrix @0x80068098 (Tabelle 0x800794c4: lo16 = sin, hi16 = cos; re15_sin_q12/re15_cos_q12).
 * Winkel `lh` (s16): a >= 0 -> Index a & 0xfff (`bgez t7` / Delay `andi t9,t7,0xfff`
 * @0x800680a0-a4); a < 0 -> Index (-a) & 0xfff, sin NEGIERT, cos wie gelesen (`subu t7,zero,t7`
 * @0x800680a8, `subu t3,zero,t8` @0x800680d0). Produkte `multu` + `mflo` (untere 32 Bit, also
 * vorzeichenneutral), `sra 12`. Eintraege:
 *   m02 = sy @0x8006816c ; m12 = -(cy*sx)>>12 @0x80068180 ; m22 = (cy*cx)>>12 @0x80068194/1d4
 *   m00 = (cz*cy)>>12 @0x8006820c ; m01 = -(sz*cy)>>12 @0x8006822c
 *   A = (cz*-sy)>>12, B = (sz*-sy)>>12
 *   m10 = ((sz*cx)>>12) - ((A*sx)>>12) ; m20 = ((sz*sx)>>12) + ((A*cx)>>12)
 *   m11 = ((cz*cx)>>12) + ((B*sx)>>12) ; m21 = ((cz*sx)>>12) - ((B*cx)>>12)
 * (dieselbe Herleitung wie der Zwilling esp_rotmatrix in re15_esp.c, dort static). */
static void gte_trig(int16_t a, int32_t *s, int32_t *c)
{
    if (a >= 0) {
        *s = re15_sin_q12(a & 0xfff);
        *c = re15_cos_q12(a & 0xfff);
    } else {
        int n = (-(int)a) & 0xfff;
        *s = -re15_sin_q12(n);
        *c = re15_cos_q12(n);
    }
}

static void gte_rotmatrix(int16_t rx, int16_t ry, int16_t rz, int16_t m[9])
{
    int32_t sx, cx, sy, cy, sz, cz;
    gte_trig(rx, &sx, &cx);
    gte_trig(ry, &sy, &cy);
    gte_trig(rz, &sz, &cz);
    int32_t nsy = -sy;
    int32_t A = (int32_t)((uint32_t)cz * (uint32_t)nsy) >> 12;
    int32_t B = (int32_t)((uint32_t)sz * (uint32_t)nsy) >> 12;
    m[2] = (int16_t)sy;
    m[5] = (int16_t)((int32_t)(0u - (uint32_t)cy * (uint32_t)sx) >> 12);
    m[8] = (int16_t)((int32_t)((uint32_t)cy * (uint32_t)cx) >> 12);
    m[0] = (int16_t)((int32_t)((uint32_t)cz * (uint32_t)cy) >> 12);
    m[1] = (int16_t)((int32_t)(0u - (uint32_t)sz * (uint32_t)cy) >> 12);
    m[3] = (int16_t)(((int32_t)((uint32_t)sz * (uint32_t)cx) >> 12) -
                     ((int32_t)((uint32_t)A * (uint32_t)sx) >> 12));
    m[6] = (int16_t)(((int32_t)((uint32_t)sz * (uint32_t)sx) >> 12) +
                     ((int32_t)((uint32_t)A * (uint32_t)cx) >> 12));
    m[4] = (int16_t)(((int32_t)((uint32_t)cz * (uint32_t)cx) >> 12) +
                     ((int32_t)((uint32_t)B * (uint32_t)sx) >> 12));
    m[7] = (int16_t)(((int32_t)((uint32_t)cz * (uint32_t)sx) >> 12) -
                     ((int32_t)((uint32_t)B * (uint32_t)cx) >> 12));
}

/* GTE-Ergebnis mit sf = 1: MAC = Summe >> 12 (arithmetisch). */
static int32_t gte_mac(const int16_t *row, int32_t v0, int32_t v1, int32_t v2)
{
    int64_t s = (int64_t)row[0] * v0 + (int64_t)row[1] * v1 + (int64_t)row[2] * v2;
    return (int32_t)(s >> 12);
}

/* MulMatrix2 @0x800684b4: m1 = m0 * m1. m0 -> RT11..RT33 (ctc2 0x48c80000..0x48cc2000
 * @0x800684c8-d8), je Spalte j von m1 V0 = (m1[0][j], m1[1][j], m1[2][j]) (@0x800684dc-f8 /
 * @0x80068504-28 / @0x80068534-5c — alle drei Spalten werden VOR dem ersten Store gelesen),
 * MVMVA 0x4a486012 (sf = 1, mx = RT, v = V0, cv = keine, lm = 0) -> IR1..3 (mfc2 $9..$11),
 * IR mit lm = 0 auf [-0x8000, 0x7fff] gesaettigt, Rueckschreiben @0x80068568ff. */
static int16_t gte_ir(int32_t mac)
{
    if (mac < -0x8000) return (int16_t)-0x8000;
    if (mac > 0x7fff)  return (int16_t)0x7fff;
    return (int16_t)mac;
}
static void gte_mulmatrix2(const int16_t m0[9], int16_t m1[9])
{
    int16_t r[9];
    for (int j = 0; j < 3; j++) {
        const int32_t v0 = m1[0 * 3 + j], v1 = m1[1 * 3 + j], v2 = m1[2 * 3 + j];
        for (int i = 0; i < 3; i++)
            r[i * 3 + j] = gte_ir(gte_mac(&m0[i * 3], v0, v1, v2));
    }
    memcpy(m1, r, sizeof r);
}

/* TransposeMatrix @0x80065d90: `lh`/`sh` je Element (@0x80065d90-ddc). */
static void gte_transpose(const int16_t m[9], int16_t t[9])
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            t[j * 3 + i] = m[i * 3 + j];
}

/* ApplyMatrix @0x800661c0: MVMVA sf = 1, Ergebnis = MAC1..3 (swc2, 32 Bit, KEINE Saettigung). */
static void gte_applymatrix(const int16_t m[9], const int16_t v[3], int32_t r[3])
{
    for (int i = 0; i < 3; i++) r[i] = gte_mac(&m[i * 3], v[0], v[1], v[2]);
}

/* SquareRoot12 @0x80066120: wie SquareRoot0 @0x80065f60 (LZCR, gerade Zaehlung, Tabelle
 * 0x8007d984), nur Denormierung t1 = (19 - t2) >> 1 (`addiu t1,zero,19` @0x80066148) und KEIN
 * `srl 12` — bei t1 >= 0 `sllv` (@0x80066194), sonst `srlv` um -t1 (@0x800661a0-a8).
 * Fuer 0 < a < 2^19 ist das GENAU SquareRoot0(a << 12): LZCR sinkt um 12 (gerade -> t2 - 12),
 * t1' = (43 - t2) >> 1 = t1 + 12, der normierte Index ist derselbe (die unteren 12 Bit sind 0),
 * und (tab << (t1 + 12)) >> 12 = tab << t1 bzw. tab >> -t1 (t1 >= -6, tab < 2^13, kein Ueberlauf).
 * a == 0 -> 0 (@0x80066134 `beq v0,at` bei LZCR 32). Andere Eingaben kommen hier nicht vor:
 * a = 4096 - (y*y >> 12) mit |y| <= 4096 fuer die Spalte einer Drehmatrix; alle fallfaehigen
 * Objekte der Auslieferung haben rot_y 0 oder 2048 (ROOM2050 obj 6), also m02 ~ 0 -> a ~ 4096. */
static int32_t gte_squareroot12(int32_t a)
{
    if (a > 0 && a < (1 << 19)) return (int32_t)re15_squareroot0((uint32_t)a << 12);
    if (a != 0) {
        FILE *lf = objfall_log();
        if (lf) fprintf(lf, "F%u SquareRoot12(%d) ausserhalb 0..2^19 -> nicht nachgebildet\n",
                        g_engine.frame_count, (int)a);
    }
    return 0;
}

/* FUN_8002dacc(obj, &delta) — Drehung um den Bild-Winkelschritt, Euler neu ableiten:
 *   @0x8002db00  RotMatrix(delta, R)                     (`jal 0x80068098`)
 *   @0x8002db0c  MulMatrix2(R, obj+0x20)  -> M = R * M     (`jal 0x800684b4`)
 *   obj+0x20 ist die Matrix, die der Zeichner FUN_8002c18c JEDES Bild aus obj+0x68 baut
 *   (@0x8002c210-218 `addiu a0,s1,104` / `jal 0x80068098` / a1 = s1+32) -> M = RotMatrix(rot).
 *   @0x8002db14-5c  len = (m02^2 + m12^2 + m22^2) >> 12 (`bgez` / `addiu 4095` / `sra 12`)
 *   @0x8002db40/60  y = (m02 << 12) / len                   (`div`, gekappt Richtung 0)
 *   @0x8002db8c-98  rot_x = ratan2(-m12, m22)               (`sh v0,104(s1)`)
 *   @0x8002db94-ac  q = (y*y) >> 12 (`bgez`/`addiu 4095`/`sra 12`), SquareRoot12(4096 - q)
 *   @0x8002dbbc-cc  rot_y = ratan2(y, sqrt)                 (`sh v0,106(s1)`)
 *   @0x8002dbd0     rot_z = 0, @0x8002dbdc RotMatrix(rot) -> T, @0x8002dbec Transpose(T)
 *   @0x8002dbf4-0c  v = (m00, m10, m20) (`lhu 32/38/44`), @0x8002dc14 ApplyMatrix(T^t, v)
 *   @0x8002dc1c-2c  rot_z = ratan2(r.y, r.x)                (`sh v0,108(s1)`) */
static void objfall_8002dacc(int p, const int16_t d[3])
{
    int16_t m[9], r[9];
    gte_rotmatrix(g_scd.props[p].rot_x, g_scd.props[p].rot_y, g_scd.props[p].rot_z, m);
    gte_rotmatrix(d[0], d[1], d[2], r);
    gte_mulmatrix2(r, m);
    const int32_t m02 = m[2], m12 = m[5], m22 = m[8];
    int32_t s0 = m02 * m02 + m12 * m12 + m22 * m22;
    int32_t len = ((s0 < 0) ? (s0 + 4095) : s0) >> 12;
    if (len == 0) return;                                   /* `break 7` @0x8002db6c — unerreicht */
    int32_t y = (int32_t)((uint32_t)m02 << 12) / len;
    int16_t rx = (int16_t)re15_ratan2(-m12, m22);
    int32_t yy = y * y;
    yy = ((yy < 0) ? (yy + 4095) : yy) >> 12;
    int16_t ry = (int16_t)re15_ratan2(y, gte_squareroot12(4096 - yy));
    int16_t t[9], tt[9];
    gte_rotmatrix(rx, ry, 0, t);
    gte_transpose(t, tt);
    const int16_t v[3] = { m[0], m[3], m[6] };
    int32_t o[3];
    gte_applymatrix(tt, v, o);
    g_scd.props[p].rot_x = rx;
    g_scd.props[p].rot_y = ry;
    g_scd.props[p].rot_z = (int16_t)re15_ratan2(o[1], o[0]);
}

void re15_objfall_drehe(int prop_idx, const int16_t delta[3])
{
    if (prop_idx < 0 || prop_idx >= (int)g_scd.prop_count) return;
    objfall_8002dacc(prop_idx, delta);
}

/* ================================================================================================
 *  FUN_8002d3d8 — Fallrichtung VOM SPIELER WEG (@0x8002d3d8-46c)
 *   dx = (s16)(obj.x - player.x) (`lhu 52(a2)` - `lhu 0x800aca88`, `sll/sra 16` @0x8002d3fc-400)
 *   |dx| < 900 (`slti v0,v0,900` @0x8002d410) -> Z-Richtung: richt_z = (dz16 >> 31) | 1, richt_x = 0
 *   sonst                                       -> X-Richtung: richt_x = (dx16 >> 31) | 1, richt_z = 0
 * ================================================================================================ */
static void objfall_8002d3d8(int p)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int32_t dx = (int32_t)(int16_t)(uint16_t)((uint32_t)g_scd.props[p].x - (uint32_t)pl->x);
    const int32_t dz = (int32_t)(int16_t)(uint16_t)((uint32_t)g_scd.props[p].z - (uint32_t)pl->z);
    const int32_t adx = (dx < 0) ? -dx : dx;
    if (adx < 900) {                                        /* @0x8002d410-14 */
        s_richt_z = (int16_t)((dz >> 31) | 1);              /* @0x8002d41c-20, sh @0x8002d430 */
        s_richt_x = 0;                                      /* sh zero @0x8002d428 */
    } else {
        s_richt_x = (int16_t)((dx >> 31) | 1);              /* @0x8002d43c-40, sh @0x8002d450 */
        s_richt_z = 0;                                      /* sh zero @0x8002d448 */
    }
}

/* ================================================================================================
 *  FUN_8002c444 — der Fall (a0 = s1 = Objekt)
 * ================================================================================================ */
static void objfall_spritzer(int p)
{
    /* @0x8002c700-708 DAT_8008f6a0 = 1; @0x8002c6fc/@0x8002c70c FUN_80045024(0x02080000, obj+0x34)
     * = Bank 2 (RDT-snd0) Satz 8. Dann 12x FUN_80019700(0x06022000 = Effekt 6, sub 2, Skala
     * 0x2000 (`lui a0,0x602` / `ori a0,a0,0x2000` @0x8002c714-18 ...), Gier a1, a2 = Einheitsmatrix
     * 0x80072d4c (@0x8002c724-28), a3 = &P (sp+16)). P wird zwischen den Aufrufen nur teilweise
     * neu geschrieben (Rest bleibt stehen); P.y = obj.y (`sw v0,20(sp)` @0x8002c72c). */
    static const struct { int16_t dx, dz, gier; uint8_t setzt_x; } k[12] = {
        {  900,  900, 0x200, 1 },   /* @0x8002c730-750: x+900 z+900, a1 0x200 @0x8002c71c */
        {  900,  225, 0x400, 0 },   /* @0x8002c764-770: z+225 (0xe1),  a1 0x400 @0x8002c75c */
        {  900, -225, 0x400, 0 },   /* @0x8002c788-794: z-225,         a1 0x400 @0x8002c780 */
        {  900, -900, 0x600, 0 },   /* @0x8002c7ac-7b8: z-900,         a1 0x600 @0x8002c7a4 */
        {  225,  900, 0x000, 1 },   /* @0x8002c7cc-7e8: x+225 z+900,   a1 0     @0x8002c7c8 */
        {  225, -900, 0x800, 0 },   /* @0x8002c800-80c: z-900,         a1 0x800 @0x8002c7f8 */
        { -225,  900, 0x000, 1 },   /* @0x8002c820-83c: x-225 z+900,   a1 0     @0x8002c81c */
        { -225, -900, 0x800, 0 },   /* @0x8002c854-860: z-900,         a1 0x800 @0x8002c84c */
        { -900,  900, 0xe18, 1 },   /* @0x8002c874-890: x-900 z+900,   a1 0xe18 @0x8002c870 */
        { -900,  225, 0xc18, 0 },   /* @0x8002c8a8-8b4: z+225,         a1 0xc18 @0x8002c8a0 */
        { -900, -225, 0xc18, 0 },   /* @0x8002c8cc-8d8: z-225,         a1 0xc18 @0x8002c8c4 */
        { -900, -900, 0xa00, 0 },   /* @0x8002c8f0-8fc: z-900,         a1 0xa00 @0x8002c8e8 */
    };
    s_6a0 = 1;
    re15_audio_room_se_snd0(8);
    const int32_t ox = g_scd.props[p].x, oy = g_scd.props[p].y, oz = g_scd.props[p].z;
    int32_t px = ox + 900, pz = oz + 900;
    int n = 0;
    for (int i = 0; i < 12; i++) {
        if (k[i].setzt_x) px = ox + k[i].dx;
        pz = oz + k[i].dz;
        /* Boden-Ebene der Port-Sammelklemme (re15_esp_fx_tick, Routine B 12): das Original fragt je
         * Takt FUN_8001c6e8 ab; hier dieselbe Abfrage, die der Fall bei Bild 13 schon an der
         * Objektlage gemacht hat (DAT_8008f694). */
        n += re15_esp_fx_spawn_rows(re15_esp_room_bank(), 0x06, 0x02, 0x2000,
                                    px, oy, pz, (int32_t)s_694, k[i].gier);
    }
    s_n_spritzer++;
    FILE *lf = objfall_log();
    if (lf) fprintf(lf, "F%u room=%04X p=%d SPRITZER wasser=%d y=%d se=snd0:8 plaetze=%d\n",
                    g_engine.frame_count, g_current_room_id, p, (int)s_wasser[p], (int)oy, n);
}

static void objfall_8002c444(const re15_rdt_t *rdt, int p)
{
    FILE *lf = objfall_log();
    const uint8_t z0 = s_zust[p];
    switch (z0) {
    case 0:                                                  /* @0x8002c49c-4f8 */
        s_680 = 0xd;                                         /* sh -> 0x8008f680 @0x8002c4a0 */
        s_67c = 0xd;                                         /* sh -> 0x8008f67c @0x8002c4a8 */
        s_688 = 8;                                           /* sh -> 0x8008f688 @0x8002c4b4 */
        s_690 = 0;                                           /* @0x8002c4c0 */
        s_684 = 0;                                           /* @0x8002c4c8 */
        s_drehung[0] = s_drehung[1] = s_drehung[2] = 0;      /* @0x8002c4d0/d8/e0 */
        s_68c = 0;                                           /* @0x8002c4e8 */
        s_6a0 = 0;                                           /* sb @0x8002c4f0 */
        s_zust[p]  = 1;                                      /* sb v0(=1),5(s1) @0x8002c4f4 */
        s_zaehl[p] = 0;                                      /* sb zero,6(s1)   @0x8002c4f8 */
        /* fallthrough: Zustand 1 im selben Bild (@0x8002c4fc) */
    case 1: {
        const int32_t rx = s_richt_x, rz = s_richt_z;        /* lh 0x800b526c / 0x800b527c */
        const int32_t sp = s_67c;                            /* lh 0x8008f67c @0x8002c500 */
        g_scd.props[p].x += sp * rx;                         /* mult/sw 52 @0x8002c510-534 */
        g_scd.props[p].z += sp * rz;                         /* mult/sw 60 @0x8002c524-544 */
        const int32_t w = s_690;                             /* lh 0x8008f690 @0x8002c54c */
        if (w < 1024) {                                      /* slti 1024 @0x8002c558 */
            const uint32_t a1 = (uint16_t)s_684;             /* lhu @0x8002c568 */
            const int32_t v1 = w + (int32_t)a1;              /* addu @0x8002c570 */
            s_690 = (int16_t)v1;                             /* sh @0x8002c584 */
            if (!((int16_t)v1 < 1025))                       /* slti 1025 @0x8002c57c */
                s_684 = (int16_t)((int32_t)a1 + 1024 - v1);  /* @0x8002c58c-598 */
            const int32_t vel = s_684;                       /* lh @0x8002c5a0 */
            s_drehung[0] = (int16_t)(vel * (-rz));           /* @0x8002c5a4-b8 -> 0x8008f698 */
            s_drehung[2] = (int16_t)(rx * vel);              /* @0x8002c5b0-c4 -> 0x8008f69c */
        } else {
            s_drehung[2] = 0;                                /* @0x8002c5d4 */
            s_drehung[0] = 0;                                /* @0x8002c5dc */
        }
        s_684 = (int16_t)((uint16_t)s_684 + (uint16_t)s_688);   /* @0x8002c5e0-60c */
        s_67c = (int16_t)((uint16_t)s_67c + (uint16_t)s_680);   /* @0x8002c5f4-614 */
        s_zaehl[p] = (uint8_t)(s_zaehl[p] + 1);              /* @0x8002c618-624 */
        if (s_zaehl[p] < 0xd) {                              /* sltiu 0xd @0x8002c630 */
            /* @0x8002c92c-968: y += (s16)(Vorschub / 3) (0x55555556-Idiom, Richtung 0 gekappt) */
            const int32_t v = s_67c;
            g_scd.props[p].y += (int32_t)(int16_t)(v / 3);
            break;
        }
        {
            const int32_t f = s_68c;                         /* lh 0x8008f68c @0x8002c640 */
            s_680 = 0;                                       /* sh zero @0x8002c64c */
            g_scd.props[p].y += f;                           /* @0x8002c650 / sw @0x8002c664 */
            s_68c = (int16_t)(f + 160);                      /* addiu 160 @0x8002c654 */
        }
        if (s_zaehl[p] == 0xd) {                             /* `ori v0,zero,0xd` / bne @0x8002c668-6c */
            /* FUN_8001c6e8(&obj+0x34, 0, 8, 0x200) @0x8002c674-680: Bodenhoehe; das Objekt selbst
             * ueberspringt die Funktion (`beq s5,v0` @0x8001c800, a0 == eigener +0x34). Im Port:
             * den eigenen Platz fuer die Dauer der Abfrage aus der Objekt-Stufe nehmen. */
            const uint8_t war_aktiv = g_scd.props[p].active;
            g_scd.props[p].active = 0;
            const int16_t boden = re15_collision_room_coll(rdt, g_scd.props[p].x, g_scd.props[p].z,
                                                           0, 8, 0x200u);
            g_scd.props[p].active = war_aktiv;
            s_694 = boden;                                   /* sh -> 0x8008f694 @0x8002c6a8 */
            /* @0x8002c684-6c4: 0x91a2b3c5-Idiom = Division durch 1800 (gekappt), negiert. */
            const int32_t a2 = boden;
            const uint8_t band_neu = (uint8_t)(-(a2 / 1800));
            if (lf) fprintf(lf, "F%u room=%04X p=%d BODEN FUN_8001c6e8=%d band %d -> %d pos=(%d,%d,%d)\n",
                            g_engine.frame_count, g_current_room_id, p, (int)boden,
                            (int)g_scd.props[p].band, (int)band_neu,
                            g_scd.props[p].x, g_scd.props[p].y, g_scd.props[p].z);
            g_scd.props[p].band = band_neu;                  /* sb v1,130(s1) @0x8002c6c4 */
        }
        /* @0x8002c6c8-6f8: Spritzer, einmal je Fall, wenn ein Wasserspiegel gestempelt ist und
         * die Objekt-Unterkante darunter liegt (`slt v0,v1,v0` = Spiegel < obj.y). */
        if (s_6a0 == 0 && s_wasser[p] != 0 && (int32_t)s_wasser[p] < g_scd.props[p].y)
            objfall_spritzer(p);
        /* @0x8002c904-928: unter dem Boden -> aufsetzen, Zustand 2. */
        if ((int32_t)s_694 < g_scd.props[p].y) {
            g_scd.props[p].y = s_694;                        /* sw v1,56(s1) @0x8002c920 */
            s_zust[p] = 2;                                   /* sb v0,5(s1)  @0x8002c928 */
            s_n_landungen++;
            if (lf) fprintf(lf, "F%u room=%04X p=%d LANDUNG y=%d band=%d pos=(%d,%d)\n",
                            g_engine.frame_count, g_current_room_id, p, g_scd.props[p].y,
                            (int)g_scd.props[p].band, g_scd.props[p].x, g_scd.props[p].z);
        }
        break;
    }
    case 2: {                                                /* @0x8002c96c-a0c */
        if (s_wasser[p] == 0) re15_audio_room_se_snd0(6);    /* lh 136 / bne; FUN_80045024(0x02060000) @0x8002c97c */
        const int32_t a1 = s_richt_x, a0 = s_richt_z;
        g_scd.props[p].y -= 30;                              /* addiu -30 @0x8002c99c */
        g_scd.props[p].x += a1 * 20;                         /* ((a1<<2)+a1)<<2 @0x8002c9a4-b4 */
        g_scd.props[p].z += a0 * 20;                         /* @0x8002c9b8-cc */
        s_drehung[0] = (int16_t)(-(a0 * 30));                /* ((a0<<4)-a0)<<1, negiert @0x8002c9d0-e4 */
        s_drehung[2] = (int16_t)(a1 * 30);                   /* @0x8002c9e8-f8 */
        s_zust[p] = (uint8_t)(s_zust[p] + 1);                /* @0x8002c9fc-a0c */
        if (lf) fprintf(lf, "F%u room=%04X p=%d AUFSETZEN se=%s\n", g_engine.frame_count,
                        g_current_room_id, p, s_wasser[p] == 0 ? "snd0:6" : "-");
        break;
    }
    case 3: {                                                /* @0x8002ca10-a88 */
        const int32_t a1 = s_richt_x, a0 = s_richt_z;
        g_scd.props[p].y += 30;                              /* addiu 30 @0x8002ca28 */
        g_scd.props[p].x += a1 * 20;                         /* @0x8002ca30-40 */
        g_scd.props[p].z += a0 * 20;                         /* @0x8002ca44-58 */
        s_drehung[0] = (int16_t)(a0 * 30);                   /* @0x8002ca5c-6c */
        s_drehung[2] = (int16_t)(-(a1 * 30));                /* @0x8002ca70-84 */
        s_fall[p] = 0;                                       /* sb zero,4(s1) @0x8002ca88 */
        break;
    }
    default:
        break;                                               /* -> @0x8002ca8c */
    }
    objfall_8002dacc(p, s_drehung);                          /* @0x8002ca8c-94 */
    if (lf) fprintf(lf, "F%u room=%04X p=%d zust %d->%d n=%d pos=(%d,%d,%d) band=%d rot=(%d,%d,%d) "
                        "dreh=(%d,%d,%d) 67c=%d 684=%d 690=%d 68c=%d wasser=%d\n",
                    g_engine.frame_count, g_current_room_id, p, (int)z0, (int)s_zust[p],
                    (int)s_zaehl[p], g_scd.props[p].x, g_scd.props[p].y, g_scd.props[p].z,
                    (int)g_scd.props[p].band, (int)g_scd.props[p].rot_x, (int)g_scd.props[p].rot_y,
                    (int)g_scd.props[p].rot_z, (int)s_drehung[0], (int)s_drehung[1],
                    (int)s_drehung[2], (int)s_67c, (int)s_684, (int)s_690, (int)s_68c,
                    (int)s_wasser[p]);
}

/* ================================================================================================
 *  Haken
 * ================================================================================================ */
void re15_objfall_install(uint16_t room_id)
{
    (void)room_id;
    /* Der Port legt g_scd.props[] beim Raumaufbau neu an (memset in scd_room_reenter); die Felder
     * obj+4/+5/+6/+0x88 des Originals schreibt der Installer LAB_80040914 NICHT (Stores nur +0x00,
     * +0x08, +0x09, +0x0C, +0x34..+0x3C, +0x68..+0x6C, +0x74, +0x78, +0x82 — @0x8004095c-80040aa0),
     * und der Raumlader nullt nur +0x00 (FUN_8003ea7c @0x8003eab0-cc). Ein Fall, der beim
     * Raumwechsel laeuft, wuerde im Original im selben Pool-Platz weiterlaufen — das braucht einen
     * Raumwechsel waehrend der ~25 Fallbilder vor dem Spieler (kein Spielweg). Der Port beginnt den
     * Raum mit ruhenden Objekten. */
    memset(s_fall, 0, sizeof s_fall);
    memset(s_zust, 0, sizeof s_zust);
    memset(s_zaehl, 0, sizeof s_zaehl);
    memset(s_wasser, 0, sizeof s_wasser);
    s_handler_lief = 0;
}

static void objfall_handler_pass(const re15_rdt_t *rdt)
{
    /* @0x8002bdb0-c8 rueckwaerts, @0x8002bdcc-d8 nur aktive, @0x8002bde0-be08 Tabelle
     * @0x80073c70[Typ]; nur [4] = FUN_8002c40c tut etwas: obj+4 == 1 -> FUN_8002c444
     * (@0x8002c414-42c; obj+4 == 0 -> `beq v1,zero` @0x8002c41c). */
    for (int p = (int)g_scd.prop_count - 1; p >= 0; p--) {
        if (p >= RE15_SCD_MAX_PROPS || !g_scd.props[p].active) continue;
        if (g_scd.props[p].obj_type != 4) continue;
        if (s_fall[p] != 1) continue;
        objfall_8002c444(rdt, p);
    }
    s_handler_lief = 1;
}

void re15_objfall_vor_schub(const re15_rdt_t *rdt)
{
    objfall_handler_pass(rdt);
}

/* AUTO-Scan-Stempel der WASSER-Zonen auf die Objekte (FUN_800436a8 @0x80043768-b4, je Pool-Eintrag
 * 0..n-1 FUN_80042bac(obj, 4, 0)). Je AOT-Record: @0x80042c8c Pool-Maske & 4, @0x80042ca4 Bit 0x10
 * (nur ACTION) muss 0 sein, @0x80042cac-ccc Band-Gate (Bit 0x80 = jedes Band, sonst rec[2] ==
 * obj+0x82), @0x80042ea8-ed8 CENTRE (0x40) auf obj+0x34/+0x3C, sonst @0x80042ef8-f2c FORWARD (0x20)
 * 620 voraus; Treffer -> Handler @0x8007469c[sce] (@0x80042fc8-43010), fuer sce 8 = 0x8004330c
 * `sh v0,136(entity)` = Nutzlast u16@0. Die Schleife laeuft weiter (@0x8004301c) -> der letzte
 * Treffer gewinnt. Dieselbe Geometrie wie re15_object_notch_update (aot_common.c). */
static void objfall_wasser_stempel(void)
{
    for (int p = 0; p < (int)g_scd.prop_count && p < RE15_SCD_MAX_PROPS; p++) {
        for (int i = 0; i < RE15_AOT_MAX; i++) {
            const re15_aot_t *a = &g_aot.slots[i];
            if (!a->active || a->type != RE15_AOT_TYPE_WATER) continue;
            if (!(a->sce_flags & 0x04)) continue;
            if (a->sce_flags & 0x10) continue;
            if (!(a->band & 0x80) && g_scd.props[p].band != a->band) continue;
            int hit = 0;
            if (a->sce_flags & 0x40) {
                const int32_t ex = g_scd.props[p].x, ez = g_scd.props[p].z;
                hit = a->has_quad ? re15_aot_point_in_quad(ex, ez, a->xs, a->zs)
                                  : ((((ex - a->x) < 0 ? -(ex - a->x) : (ex - a->x)) <= a->half_w) &&
                                     (((ez - a->z) < 0 ? -(ez - a->z) : (ez - a->z)) <= a->half_h));
            }
            if (!hit && (a->sce_flags & 0x20)) {
                const int ry = (int)g_scd.props[p].rot_y;
                const int32_t c = re15_cos_q12(ry), s = re15_sin_q12(ry);
                const int32_t fx = g_scd.props[p].x + (int32_t)((620 * c) >> 12);
                const int32_t fz = g_scd.props[p].z - (int32_t)((620 * s) >> 12);
                hit = a->has_quad ? re15_aot_point_in_quad(fx, fz, a->xs, a->zs)
                                  : ((((fx - a->x) < 0 ? -(fx - a->x) : (fx - a->x)) <= a->half_w) &&
                                     (((fz - a->z) < 0 ? -(fz - a->z) : (fz - a->z)) <= a->half_h));
            }
            if (hit) s_wasser[p] = g_aot.env_params[i].p0;
        }
    }
}

/* Messschrieb: Lage-Flags der Raeume mit Fall-Lage im RDT (Ck vor dem Band-0-Record) und die
 * Kerbe obj+0x0B (Member 15) der Typ-4-Objekte, je bei Aenderung. Nur Diagnose. */
static void objfall_log_latches(FILE *lf)
{
    static const struct { uint8_t bank, bit; } fl[4] = {
        { 4, 203 },   /* ROOM5020/5021 Ck @0x0122A/@0x0124A, Latch sub01 @0x012F4-302 */
        { 3, 204 },   /* ROOM20A0/20A1 Ck @0x0183C, Latch @0x01AD4-e2 */
        { 3,  93 },   /* ROOM3050/3051 Ck @0x01F02, Latch @0x02188-196 */
        { 5,   6 },   /* ROOM11A0/11A1 Ck @0x01040, Latch @0x012B2-c4 */
    };
    static int alt[4] = { -1, -1, -1, -1 };
    static uint8_t m0b_alt[RE15_SCD_MAX_PROPS];
    for (int i = 0; i < 4; i++) {
        int v = re15_game_flag_get(fl[i].bank, fl[i].bit);
        if (v != alt[i]) {
            fprintf(lf, "F%u room=%04X FLAG (%d,%d)=%d\n", g_engine.frame_count, g_current_room_id,
                    (int)fl[i].bank, (int)fl[i].bit, v);
            alt[i] = v;
        }
    }
    if ((g_engine.frame_count % 15u) == 0u) {
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        fprintf(lf, "F%u room=%04X spieler=(%d,%d,%d) rot=%d band=%d", g_engine.frame_count,
                g_current_room_id, pl->x, pl->y, pl->z, (int)pl->rot_y, re15_collision_debug_band());
        for (int p = 0; p < (int)g_scd.prop_count && p < RE15_SCD_MAX_PROPS; p++)
            if (g_scd.props[p].active && g_scd.props[p].obj_type == 4)
                fprintf(lf, " | p%d (%d,%d,%d) b%d r(%d,%d,%d)", p, g_scd.props[p].x,
                        g_scd.props[p].y, g_scd.props[p].z, (int)g_scd.props[p].band,
                        (int)g_scd.props[p].rot_x, (int)g_scd.props[p].rot_y,
                        (int)g_scd.props[p].rot_z);
        fprintf(lf, "\n");
    }
    for (int p = 0; p < (int)g_scd.prop_count && p < RE15_SCD_MAX_PROPS; p++) {
        if (g_scd.props[p].obj_type != 4) continue;
        if (g_scd.props[p].member_0b != m0b_alt[p]) {
            fprintf(lf, "F%u room=%04X p=%d KERBE m0b=%d pos=(%d,%d,%d) band=%d\n",
                    g_engine.frame_count, g_current_room_id, p, (int)g_scd.props[p].member_0b,
                    g_scd.props[p].x, g_scd.props[p].y, g_scd.props[p].z, (int)g_scd.props[p].band);
            m0b_alt[p] = g_scd.props[p].member_0b;
        }
    }
}

void re15_objfall_nach_pass(const re15_rdt_t *rdt)
{
    if (!s_handler_lief) objfall_handler_pass(rdt);   /* Zweige ohne Objekt-Schub (Klettern, Treppe, ...) */
    s_handler_lief = 0;
    FILE *lf = objfall_log();
    if (lf) objfall_log_latches(lf);
    for (int p = (int)g_scd.prop_count - 1; p >= 0; p--) {
        if (p >= RE15_SCD_MAX_PROPS) continue;
        if (g_scd.props[p].active &&                         /* @0x8002bdcc-d8 (inaktiv -> @0x8002c150) */
            g_scd.props[p].obj_type == 4 &&                  /* @0x8002c0fc-108 */
            s_fall[p] == 0) {                                /* @0x8002c110-118 */
            /* FUN_8003b7f0(&obj+0x34, 0x1c2, obj+0x82) @0x8002c120-12c: Attributwort der ersten
             * Zelle des Objekt-Bands, deren um 450 erweitertes Rechteck die Objektlage enthaelt. */
            const uint16_t w = re15_collision_floor_typeword(rdt, g_scd.props[p].x,
                                                             g_scd.props[p].z,
                                                             (int)g_scd.props[p].band, 450);
            if (w & 2) {                                     /* andi 0x2 / beq @0x8002c130-134 */
                objfall_8002d3d8(p);                         /* jal 0x8002d3d8 @0x8002c13c */
                s_fall[p] = 1;                               /* sb v0(=1),4(s2) @0x8002c148 */
                s_zust[p] = 0;                               /* sb zero,5(s2)   @0x8002c14c */
                s_n_ausloeser++;
                if (lf) fprintf(lf, "F%u room=%04X p=%d AUSLOESER wort=0x%04x band=%d pos=(%d,%d,%d) "
                                    "spieler=(%d,%d) richt=(%d,%d)\n",
                                g_engine.frame_count, g_current_room_id, p, (unsigned)w,
                                (int)g_scd.props[p].band, g_scd.props[p].x, g_scd.props[p].y,
                                g_scd.props[p].z, g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                                g_actors[RE15_ACTOR_SLOT_PLAYER].z, (int)s_richt_x, (int)s_richt_z);
            }
        }
        s_wasser[p] = 0;                                     /* sh zero,136(s2) @0x8002c154 */
    }
    objfall_wasser_stempel();                                /* AUTO-Scan @0x8001ce1c */
}

int re15_objfall_info(int p, re15_objfall_info_t *o)
{
    if (!o || p < 0 || p >= RE15_SCD_MAX_PROPS) return -1;
    o->fall = s_fall[p];  o->zustand = s_zust[p];  o->zaehler = s_zaehl[p];
    o->wasser = s_wasser[p];  o->boden = s_694;  o->winkel = s_690;  o->spritzer = s_6a0;
    o->richt_x = s_richt_x;  o->richt_z = s_richt_z;
    return 0;
}
unsigned re15_objfall_n_ausloeser(void) { return s_n_ausloeser; }
unsigned re15_objfall_n_landungen(void) { return s_n_landungen; }
unsigned re15_objfall_n_spritzer(void)  { return s_n_spritzer; }
