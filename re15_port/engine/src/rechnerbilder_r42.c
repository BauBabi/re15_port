/*
 * RE1.5 Rebuilt — Runde 42 Strang "gegenstaende", Stufe 2: Rechner-Vollbilder (305 Cut 10, 505 Cut 5, 2080 Bild 9).
 * Belege, Stellen und Standbild-Weg: include/re15_rechnerbilder_r42.h.
 */
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "re15_rechnerbilder_r42.h"
#include "re15_rdt.h"
#include "re15_room.h"     /* g_room_rdt */

extern unsigned g_current_room_id;

unsigned g_re15_rb42_zaehler[4];

#ifdef RE15_PLATFORM_PC
#define RB_LOG(...) fprintf(stderr, "[rechnerbilder42] " __VA_ARGS__)
#else
#define RB_LOG(...) ((void)0)
#endif

static int basis(uint16_t room_id, unsigned b) { return (room_id & 0xFFFEu) == b; }

/* ---- Standbild ------------------------------------------------------------------------------------------------------------ */
static const struct { uint16_t raum; uint8_t bild; } k_standbilder[] = {
    { RE15_RB42_RAUM_208, RE15_RB42_BILD_208 },   /* ROOM208.BSS +0x90000, ROOM2080/2081 nCut 9 */
};

static int      s_bild = -1;
static unsigned s_raum = 0;

const uint8_t re15_rb42_satz_208[2] = { 0x29, RE15_RB42_BILD_208 };   /* Cut_chg-Form wie @0x00CC2 `29 07` */

int re15_rb42_standbild(void)
{
    if (s_bild >= 0 && s_raum != g_current_room_id) {   /* Raumwechsel beendet das Standbild */
        RB_LOG("ROOM%04X: Standbild %d endet (Raumwechsel nach ROOM%04X)\n", s_raum, s_bild, g_current_room_id);
        s_bild = -1;
        g_re15_rb42_zaehler[1]++;
    }
    return s_bild;
}

int re15_rb42_cut_chg(uint8_t cut)
{
    for (unsigned i = 0; i < sizeof k_standbilder / sizeof k_standbilder[0]; i++) {
        if (basis((uint16_t)g_current_room_id, k_standbilder[i].raum) && cut == k_standbilder[i].bild &&
            (int)cut >= g_room_rdt.cut_count) {
            s_bild = cut;
            s_raum = g_current_room_id;
            g_re15_rb42_zaehler[0]++;
            RB_LOG("ROOM%04X: Cut_chg(%u) >= nCut %d -> Standbild (Kamera bleibt)\n", g_current_room_id, (unsigned)cut,
                   g_room_rdt.cut_count);
            return 1;
        }
    }
    if (s_bild >= 0) {
        RB_LOG("ROOM%04X: Standbild %d endet (Cut_chg %u)\n", g_current_room_id, s_bild, (unsigned)cut);
        s_bild = -1;
        g_re15_rb42_zaehler[1]++;
    }
    return 0;
}

/* ---- Programme ------------------------------------------------------------------------------------------------------------ */
static const uint8_t *rdt_bytes(int off, int n)
{
    const re15_rdt_t *r = &g_room_rdt;
    if (!r->raw || off < 0 || off + n > (int)r->raw_size) return NULL;
    return r->raw + off;
}

/* Kopie von [off, off+n) mit eingefuegten Saetzen; ins[k] = (Dateiadresse, 2-Byte-Satz). Die Quelle wird gegen den Soll-Kopf
 * geprueft (die ersten 4 Byte des sub), sonst NULL = ausgelieferter sub. */
typedef struct { int vor; uint8_t satz[2]; } einfueg_t;
static int bauen(uint8_t *ziel, int off, int n, const uint8_t kopf[4], const einfueg_t *ins, int nins)
{
    const uint8_t *q = rdt_bytes(off, n);
    if (!q || memcmp(q, kopf, 4) != 0) return 0;
    int z = 0, k = 0;
    for (int a = off; a < off + n; a++) {
        while (k < nins && ins[k].vor == a) { ziel[z++] = ins[k].satz[0]; ziel[z++] = ins[k].satz[1]; k++; }
        ziel[z++] = q[a - off];
    }
    return z;
}

/* 3050/3051 sub15 @0x025E4..@0x0262F */
#define S305_OFF  0x025E4
#define S305_N    0x4C
static uint8_t s_p305[S305_N + 2];
static int     s_p305_n;
/* 5050/5051 sub02 @0x0086E..@0x008B3, sub03 @0x008B4..@0x008FF */
#define S505E_OFF 0x0086E
#define S505E_N   0x46
#define S505A_OFF 0x008B4
#define S505A_N   0x4C
static uint8_t s_p505e[S505E_N + 4];
static int     s_p505e_n;
static uint8_t s_p505a[S505A_N + 2];
static int     s_p505a_n;

static int bau_305(void)
{
    static const uint8_t kopf[4] = { 0x22, 0x05, 0x00, 0x00 };                 /* @0x025E4 Set(5,0)=0 */
    static const einfueg_t ins[] = { { 0x02616, { 0x29, RE15_RB42_CUT_305 } } };   /* vor Message_on 6 @0x02616 */
    const uint8_t *m6 = rdt_bytes(0x02616, 4);
    if (!m6 || m6[0] != 0x2b || m6[1] != 0x06) return 0;
    s_p305_n = bauen(s_p305, S305_OFF, S305_N, kopf, ins, 1);
    return s_p305_n;
}

static int bau_505(void)
{
    static const uint8_t kopf_e[4] = { 0x2b, 0x00, 0xff, 0xff };               /* @0x0086E Message_on 0 */
    static const einfueg_t ins_e[] = {
        { 0x00888, { 0x29, RE15_RB42_CUT_505 } },   /* vor msg 2 @0x00888 (msg 1 @0x00882 bleibt auf Cut 4 @0x00880) */
        { 0x0088E, { 0x29, 0x04 } },                /* vor msg 3 @0x0088E: zurueck auf Cut 4 wie @0x00880 (Cursor-Spiel)  */
    };
    static const uint8_t kopf_a[4] = { 0x22, 0x03, 0x7b, 0x01 };               /* @0x008B4 Set(3,0x7B)=1 */
    static const einfueg_t ins_a[] = { { 0x008E0, { 0x29, RE15_RB42_CUT_505 } } };   /* vor msg 5 @0x008E0 */
    const uint8_t *c4 = rdt_bytes(0x00880, 2), *ifel = rdt_bytes(0x00874, 4), *m5 = rdt_bytes(0x008E0, 2);
    if (!c4 || c4[0] != 0x29 || c4[1] != 0x04 || !ifel || ifel[0] != 0x06 || ifel[2] != 0x3a || !m5 || m5[0] != 0x2b || m5[1] != 0x05)
        return 0;
    s_p505e_n = bauen(s_p505e, S505E_OFF, S505E_N, kopf_e, ins_e, 2);
    if (s_p505e_n) s_p505e[0x00876 - S505E_OFF] = 0x3a + 4;   /* Ifel-Laenge @0x00876: zwei Saetze mehr im Ja-Block */
    s_p505a_n = bauen(s_p505a, S505A_OFF, S505A_N, kopf_a, ins_a, 1);
    return s_p505e_n && s_p505a_n;
}

const uint8_t *re15_rb42_ereignis(uint16_t room_id, uint8_t event_id)
{
    /* sub15 kommt im Spiel nur ueber Evt_exec (sub01 @0x0221C); der Wegplan-Befehl EREIGNIS (dachkiste_r36.c, Durchlauf
     * integration_r42_story3a) startet ihn per scd_event_fire - dort dasselbe Programm. */
    if (basis(room_id, RE15_RB42_RAUM_305) && event_id == RE15_RB42_SUB_305 && bau_305()) {
        g_re15_rb42_zaehler[2]++;
        RB_LOG("ROOM%04X: Ereignis 15 = sub15 mit Cut 10 (Bildschirm) vor msg 6\n", (unsigned)room_id);
        return s_p305;
    }
    if (basis(room_id, RE15_RB42_RAUM_505) && event_id == RE15_RB42_SUB_505_EIN && bau_505()) {
        g_re15_rb42_zaehler[3]++;
        RB_LOG("ROOM%04X: Ereignis 2 = sub02 mit Cut 5 (Bildschirm) zu msg 2, Cut 4 ab msg 3\n", (unsigned)room_id);
        return s_p505e;
    }
    return NULL;
}

const uint8_t *re15_rb42_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *pc)
{
    if (basis(room_id, RE15_RB42_RAUM_305) && sub_id == RE15_RB42_SUB_305 && bau_305()) {
        g_re15_rb42_zaehler[2]++;
        RB_LOG("ROOM%04X: sub15 mit Cut 10 (Bildschirm) vor msg 6\n", (unsigned)room_id);
        return s_p305;
    }
    if (basis(room_id, RE15_RB42_RAUM_505) && sub_id == RE15_RB42_SUB_505_AUS && bau_505()) {
        g_re15_rb42_zaehler[3]++;
        RB_LOG("ROOM%04X: sub03 mit Cut 5 (Bildschirm) vor msg 5\n", (unsigned)room_id);
        return s_p505a;
    }
    return pc;
}

const uint8_t *re15_rb42_programm(int welches, int *out_len)
{
    const uint8_t *p = NULL; int n = 0;
    if (welches == 0 && bau_305()) { p = s_p305; n = s_p305_n; }
    else if (welches == 1 && bau_505()) { p = s_p505e; n = s_p505e_n; }
    else if (welches == 2 && bau_505()) { p = s_p505a; n = s_p505a_n; }
    if (out_len) *out_len = n;
    return p;
}
