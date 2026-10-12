/* lift2030_r37.c — Runde 37 Spur C Nachbesserung 1 (Nachtrag 5b): RE2-Ton der zwei STAGE2-Hubplattformen
 * ROOM2030/2031. Herleitung und Belege: include/re15_lift2030_r37.h, analysis/befunde_runde37/C_fahrstuhl.md N1.
 *
 * Anker (ROOM2030.RDT = ROOM2031.RDT, Bytes selbst gelesen, tools/scd_dump_room.py):
 *   sub02 @0x01E30 `22 02 07 01` Set(2,7)=1 (Ereignisbeginn Lift A)            -> KLICK   (RE2 @0x02F8C/@0x02F90)
 *   sub02 @0x01E54 `0d 00 06 00 2d 00` For 45 x (Add_speed, 2x Evt_next) = 90 Bilder hinauf -> FAHRT_KURZ
 *   sub02 @0x01E98 dieselbe Schleife hinab (Else-Zweig @0x01E7C)                 -> FAHRT_KURZ
 *   sub04 @0x01F0A `22 02 07 01` Set(2,7)=1 (Ereignisbeginn Lift B)            -> KLICK
 *   sub04 @0x01F2E `0d 00 06 00 5a 00` For 90 x 2 = 180 Bilder hinauf          -> FAHRT  (= RE2-Laenge @0x02FDC)
 *   sub04 @0x01F72 dieselbe Schleife hinab (Else-Zweig @0x01F56)               -> FAHRT
 * Lage wie RE2 ROOM6030 sub10: KLICK direkt hinter Set(2,7)=1, FAHRT direkt vor der Fahrt-Schleife. RE1.5 hat
 * zwischen beiden kein Sleep (RE2: Sleep 10 @0x02F9C) - beide Toene fallen ins selbe Bild; die RE1.5-Zeitachse
 * bleibt unangetastet.
 * For laeuft im Port nur EINMAL ueber den Anker (Next springt auf den Koerper pc+6, scd_vm.c op_for/op_next) ->
 * je Fahrt genau ein FAHRT-Ton.
 */
#include "re15_lift2030_r37.h"
#include "re15_scd.h"

#include <stdio.h>
#include <string.h>

void (*re15_lift2030_ton_hook)(int satz) = NULL;
int g_re15_lift2030_anker_n = 0;

typedef struct {
    uint32_t off;        /* Datei-Offset (ROOM2030 = ROOM2031) */
    uint8_t  sig[8];     /* Rohbytes ab off */
    uint8_t  satz;       /* RE15_LIFT_TON_* */
} anker_t;

static const anker_t k_anker[] = {
    { 0x01E30u, { 0x22,0x02,0x07,0x01, 0x3c,0x00,0x06,0x00 }, RE15_LIFT_TON_KLICK      },  /* sub02 Set(2,7)=1, Cut_auto 0 */
    { 0x01E54u, { 0x0d,0x00,0x06,0x00, 0x2d,0x00,0x30,0x02 }, RE15_LIFT_TON_FAHRT_KURZ },  /* sub02 For 45 hinauf        */
    { 0x01E98u, { 0x0d,0x00,0x06,0x00, 0x2d,0x00,0x30,0x02 }, RE15_LIFT_TON_FAHRT_KURZ },  /* sub02 For 45 hinab         */
    { 0x01F0Au, { 0x22,0x02,0x07,0x01, 0x3c,0x00,0x06,0x00 }, RE15_LIFT_TON_KLICK      },  /* sub04 Set(2,7)=1, Cut_auto 0 */
    { 0x01F2Eu, { 0x0d,0x00,0x06,0x00, 0x5a,0x00,0x30,0x02 }, RE15_LIFT_TON_FAHRT      },  /* sub04 For 90 hinauf        */
    { 0x01F72u, { 0x0d,0x00,0x06,0x00, 0x5a,0x00,0x30,0x02 }, RE15_LIFT_TON_FAHRT      },  /* sub04 For 90 hinab         */
};
#define ANKER_N ((int)(sizeof k_anker / sizeof k_anker[0]))
#define LIFT_RAUM 0x2030u   /* Raum ohne Spielervariante (ROOM2030 Leon / ROOM2031 Elza) */

static const unsigned char *s_anker_pc[ANKER_N];
static int                  s_anker_idx[ANKER_N];
static int                  s_zaehler[RE15_LIFT_TON_N];
static int                  s_takt[RE15_LIFT_TON_N];
static unsigned             s_letzter_anker = 0;

void re15_lift2030_r37_reset(void)
{
    memset(s_zaehler, 0, sizeof s_zaehler);
    for (int i = 0; i < RE15_LIFT_TON_N; i++) s_takt[i] = -1;
    s_letzter_anker = 0;
}

int re15_lift2030_r37_zaehler(int satz)      { return (satz >= 0 && satz < RE15_LIFT_TON_N) ? s_zaehler[satz] : 0; }
int re15_lift2030_r37_letzter_takt(int satz) { return (satz >= 0 && satz < RE15_LIFT_TON_N) ? s_takt[satz] : -1; }
unsigned re15_lift2030_r37_letzter_anker(void) { return s_letzter_anker; }
int re15_lift2030_r37_anzahl(void) { return ANKER_N; }

int re15_lift2030_r37_anker(int i, uint16_t *raum, uint32_t *off, int *satz)
{
    if (i < 0 || i >= ANKER_N) return 0;
    if (raum) *raum = (uint16_t)LIFT_RAUM;
    if (off) *off = k_anker[i].off;
    if (satz) *satz = k_anker[i].satz;
    return 1;
}

void re15_lift2030_r37_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id)
{
    g_re15_lift2030_anker_n = 0;
    if (!raw || raw_size <= 0 || (room_id & 0xFFF0u) != LIFT_RAUM || (room_id & 0x000Fu) > 1u) return;
    for (int i = 0; i < ANKER_N; i++) {
        const anker_t *a = &k_anker[i];
        if ((long)a->off + 8 > (long)raw_size) continue;
        if (memcmp(raw + a->off, a->sig, 8) != 0) continue;   /* falscher Stand: lieber stumm als falsch */
        s_anker_pc[g_re15_lift2030_anker_n]  = raw + a->off;
        s_anker_idx[g_re15_lift2030_anker_n] = i;
        g_re15_lift2030_anker_n++;
    }
}

void re15_lift2030_r37_pc(const unsigned char *pc)
{
    for (int k = 0; k < g_re15_lift2030_anker_n; k++) {
        if (s_anker_pc[k] != pc) continue;
        const anker_t *a = &k_anker[s_anker_idx[k]];
        s_zaehler[a->satz]++;
        s_takt[a->satz] = (int)g_scd.tick_count;
        s_letzter_anker = a->off;
#ifndef RE15_PLATFORM_PSX
        fprintf(stderr, "[lift2030] Anker @0x%05X -> Satz %d (Takt %u)\n", (unsigned)a->off, (int)a->satz,
                (unsigned)g_scd.tick_count);
#endif
        if (re15_lift2030_ton_hook) re15_lift2030_ton_hook((int)a->satz);
        return;
    }
}
