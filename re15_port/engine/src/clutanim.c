/* clutanim.c — Paletten-Animation der Typ-3-Raumobjekte (FUN_8002b968 / FUN_8002bc04).
 * Beleg-Block, Adressen und Ueberlauf: include/re15_clutanim.h. */
#include <string.h>

#include "re15_clutanim.h"
#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_scd.h"

/* Typ-Tabelle @0x80073c6c (PSX.EXE Datei 0x800 + 0x63c6c), gelesen mit re15_disasm.py `bytes 0x80073c6c`:
 * 03 04 07 05 06 08 ff. Die Anmeldung liest den Typ bei 3*k, a bei 0x80073c6d + k, b bei 0x80073c6e + k. */
static const uint8_t k_tab[] = { 0x03, 0x04, 0x07, 0x05, 0x06, 0x08, 0xFF };

/* BYTE-SPEICHER ab 0x800b267c: Eintrag k bei k*0x34 (@0x8002bb14-28 `*13*4`), der Zaehler 0x800b281e liegt
 * bei +0x1a2 (= 0x800b281e - 0x800b267c), also MITTEN in Eintrag 8 (+2 = Takt a). Mit einem Byte-Zaehler
 * kommt man hoechstens bis Eintrag 255; das Feld deckt das ab. */
#define E_STRIDE   0x34
#define E_ZAEHLER  0x1A2
#define DM_OFF(j)  (4 + (j) * 0x18)        /* DR_MOVE j ab e+4, je 0x18 (@0x8002bb64/@0x8002bb70) */
#define DM_SX      12                      /* `sh zero,12(s0)` @0x8002bb7c, Tick `sh a0,16(v0)` = e+4+12 */
#define DM_SY      14                      /* `sh s4,14(s0)`  @0x8002bb80 */
#define DM_X0      16                      /* `sh t0,16(s0)`  @0x8002bb84 */
#define DM_Y0      18                      /* `sh v1,18(s0)`  @0x8002bba8 */
#define MEM_SIZE   (256 * E_STRIDE + E_STRIDE)

static uint8_t s_mem[MEM_SIZE];
/* PORT: die Quelle eines Eintrags ist im Original eine VRAM-Zeile der (verlegten) TIM des anmeldenden
 * Objekts. Ohne VRAM fuehrt der Port dazu das anmeldende Objekt mit; geschrieben genau dann, wenn das
 * Original sy schreibt. */
static int16_t s_quell_obj[256];
static int16_t s_obj_ziel[RE15_CLUTANIM_MAX_OBJ];   /* GetClut-Zeile - 0x1e0 je obj_id, -1 = keine */
static uint8_t s_puffer;                             /* DAT_800aca34 (Bildpuffer), wechselt je Bild */
static uint8_t s_letzter_puffer;

static unsigned zaehler(void) { return s_mem[E_ZAEHLER]; }
static void     sh(unsigned off, uint16_t v) { s_mem[off] = (uint8_t)v; s_mem[off + 1] = (uint8_t)(v >> 8); }
static uint16_t lh(unsigned off) { return (uint16_t)(s_mem[off] | (s_mem[off + 1] << 8)); }

void re15_clutanim_reset(void)
{
    memset(s_mem, 0, sizeof s_mem);                  /* nur der Zaehler ist im Original belegt (@0x8003ed94) */
    for (int i = 0; i < 256; i++) s_quell_obj[i] = -1;
    for (int i = 0; i < RE15_CLUTANIM_MAX_OBJ; i++) s_obj_ziel[i] = -1;
    s_puffer = 0;
    s_letzter_puffer = 0;
}

void re15_clutanim_anmelden(int n_obj, const uint8_t *aktiv, const uint8_t *typ, const int16_t *quellzeile)
{
    if (n_obj > RE15_CLUTANIM_MAX_OBJ) n_obj = RE15_CLUTANIM_MAX_OBJ;
    for (int i = 0; i < n_obj; i++) {
        if (!aktiv[i]) continue;                                 /* @0x8002b9e0 andi 1 */
        int k = -1;
        for (int t = 0; k_tab[3 * t] != 0xFF; t++)               /* @0x8002ba10-ba2c */
            if (k_tab[3 * t] == typ[i]) { k = t; break; }
        if (k < 0) continue;
        const unsigned n   = zaehler();
        s_obj_ziel[i] = (int16_t)n;                              /* GetClut(0x130, 0x1e0 + n) @0x8002ba48-58 */
        const unsigned e   = n * E_STRIDE;                       /* @0x8002bb08-28 */
        s_mem[e + 0] = 0;                                        /* @0x8002bb2c */
        s_mem[e + 2] = k_tab[1 + k];                             /* @0x8002bb34-44: 0x80073c6d + s0 */
        const uint8_t b = k_tab[2 + k];                          /* @0x8002bb4c-54: 0x80073c6e + s0 */
        s_mem[e + 1] = 0;                                        /* @0x8002bb5c */
        s_mem[e + 3] = b;                                        /* @0x8002bb60 */
        for (int j = 0; j < 2; j++) {                            /* @0x8002bb64-bba8 */
            const unsigned dm = e + DM_OFF(j);
            sh(dm + DM_SX, 0);
            sh(dm + DM_SY, (uint16_t)quellzeile[i]);
            sh(dm + DM_X0, 0x130);
            sh(dm + DM_Y0, (uint16_t)zaehler());                 /* NEU gelesen @0x8002bb8c: nach dem Ueberlauf
                                                                  * steht hier schon a */
        }
        s_quell_obj[n & 0xFF] = (int16_t)i;
        s_mem[E_ZAEHLER] = (uint8_t)(zaehler() + 1);             /* @0x8002bbac-b8 */
    }
}

void re15_clutanim_raum_anmelden(const void *rdt_v)
{
    const re15_rdt_t *rdt = (const re15_rdt_t *)rdt_v;
    re15_clutanim_reset();                                       /* FUN_8003ecec im selben FUN_8003ef6c */
    if (!rdt) return;
    int n = rdt->prop_count;                                     /* nOmodel = RDT[2] (@0x8002b99c) */
    if (n <= 0) return;
    if (n > RE15_CLUTANIM_MAX_OBJ) n = RE15_CLUTANIM_MAX_OBJ;
    uint8_t aktiv[RE15_CLUTANIM_MAX_OBJ] = {0}, typ[RE15_CLUTANIM_MAX_OBJ] = {0};
    int16_t zeile[RE15_CLUTANIM_MAX_OBJ];
    for (int i = 0; i < n; i++) zeile[i] = -1;
    /* Pool-Index = obj_id (LAB_80040914 @0x8004093c-58). Der Port fuehrt die Props in Installations-
     * Reihenfolge; die obj_id stellt den Index her. Flag-Bit 0 = pool+0x00 (@0x80040990-a4 bzw. 0 bei der
     * Zweit-Installation @0x8004098c). */
    for (int p = 0; p < (int)g_scd.prop_count && p < RE15_SCD_MAX_PROPS; p++) {
        int id = g_scd.props[p].obj_id;
        if (id < 0 || id >= n) continue;
        aktiv[id] = (uint8_t)((g_scd.props[p].flags & 1u) ? 1 : 0);
        typ[id]   = g_scd.props[p].obj_type;
    }
    for (int i = 0; i < n && i < RE15_RDT_MAX_PROPS; i++) {
        if (!aktiv[i]) continue;
        re15_md1_t md;
        re15_tim_t tm;
        if (!rdt->prop_md1[i] || !rdt->prop_tim[i]) continue;
        if (re15_md1_parse(rdt->prop_md1[i], rdt->prop_md1_size[i], &md) != 0 || md.mesh_count < 1) continue;
        if (re15_tim_parse(rdt->prop_tim[i], rdt->prop_tim_size[i], &tm) != 0 || !tm.has_clut) continue;
        /* Pool+0x10/+0x18 = erstes Mesh (FUN_8002b898: param_2+0xc / +0x28); +0x14 = Dreiecks-, Vierecks-
         * Zahl. Erst Dreieck, dann Viereck (@0x8002ba78-80 / @0x8002bacc-d4), clut>>6 = y. */
        const re15_md1_mesh_t *m = &md.meshes[0];
        int cl = -1;
        if (m->triangle_count > 0 && m->triangle_uvs) cl = m->triangle_uvs[0].clut;
        if (m->quad_count > 0 && m->quad_uvs)         cl = m->quad_uvs[0].clut;
        if (cl >= 0) zeile[i] = (int16_t)(((cl >> 6) & 0x1FF) - tm.clut_y);
    }
    re15_clutanim_anmelden(n, aktiv, typ, zeile);
}

void re15_clutanim_tick(void)
{
    const unsigned n = zaehler();                                /* @0x8002bc10 */
    for (unsigned k = 0; k < n; k++) {                           /* @0x8002bd2c sltu */
        const unsigned e = k * E_STRIDE;
        const uint8_t a = s_mem[e + 2];
        if (a == 0) continue;                                    /* @0x8002bc58 break 7 (Division durch 0) —
                                                                  * kommt mit der Tabelle nicht vor */
        sh(e + DM_OFF(s_puffer) + DM_SX, (uint16_t)((s_mem[e + 1] / a) << 4));   /* @0x8002bc4c-7c */
        const unsigned ende = (unsigned)s_mem[e + 3] * a;        /* @0x8002bc80-8c mult */
        s_mem[e + 1] = (uint8_t)(s_mem[e + 1] + 1);              /* @0x8002bc98-9c */
        if (s_mem[e + 1] == ende) s_mem[e + 1] = 0;              /* @0x8002bca8-b0 (lbu: Byte-Vergleich) */
    }
    s_letzter_puffer = s_puffer;
    s_puffer ^= 1u;                                              /* DAT_800aca34 wechselt je Bild */
}

/* Welcher getickte Eintrag beschreibt die Zeile? addPrim haengt vorn ein -> die OT laeuft vom zuletzt
 * angehaengten (groesste Nummer) zum ersten; der kleinste Index schreibt zuletzt und gewinnt. */
static int schreiber(int ziel)
{
    const unsigned n = zaehler();
    for (unsigned k = 0; k < n; k++)
        if ((int)lh(k * E_STRIDE + DM_OFF(s_letzter_puffer) + DM_Y0) == ziel) return (int)k;
    return -1;
}

int re15_clutanim_objekt(int obj_id, int *quell_obj, int *quell_zeile, int *unter)
{
    if (obj_id < 0 || obj_id >= RE15_CLUTANIM_MAX_OBJ || s_obj_ziel[obj_id] < 0) return 0;
    const int k = schreiber(s_obj_ziel[obj_id]);
    if (k < 0) return 2;
    const unsigned dm = (unsigned)k * E_STRIDE + DM_OFF(s_letzter_puffer);
    if (quell_obj)   *quell_obj   = s_quell_obj[k & 0xFF];
    if (quell_zeile) *quell_zeile = (int16_t)lh(dm + DM_SY);
    if (unter)       *unter       = lh(dm + DM_SX) >> 4;
    return 1;
}

int re15_clutanim_max_unter(int obj_id)
{
    if (obj_id < 0 || obj_id >= RE15_CLUTANIM_MAX_OBJ || s_obj_ziel[obj_id] < 0) return -1;
    const int k = schreiber(s_obj_ziel[obj_id]);
    if (k < 0) return -1;
    const unsigned e = (unsigned)k * E_STRIDE;
    const unsigned a = s_mem[e + 2], b = s_mem[e + 3];
    if (a == 0) return 0;
    unsigned m = b ? (a * b - 1u) / a : 255u / a;               /* Zaehler laeuft 0..b*a-1 (b=0: Byte-Umlauf) */
    return m > 15u ? 15 : (int)m;
}

int re15_clutanim_anzahl(void) { return (int)zaehler(); }

int re15_clutanim_eintrag(int k, int *z, int *a, int *b, int *ziel, int *quell_obj, int *quell_zeile, int *sx)
{
    if (k < 0 || k > 255) return -1;
    const unsigned e = (unsigned)k * E_STRIDE, dm = e + DM_OFF(s_letzter_puffer);
    if (z)           *z = s_mem[e + 1];
    if (a)           *a = s_mem[e + 2];
    if (b)           *b = s_mem[e + 3];
    if (ziel)        *ziel = lh(dm + DM_Y0);
    if (quell_obj)   *quell_obj = s_quell_obj[k];
    if (quell_zeile) *quell_zeile = (int16_t)lh(dm + DM_SY);
    if (sx)          *sx = lh(dm + DM_SX);
    return 0;
}

int re15_clutanim_zielzeile(int obj_id)
{
    if (obj_id < 0 || obj_id >= RE15_CLUTANIM_MAX_OBJ) return -1;
    return s_obj_ziel[obj_id];
}
