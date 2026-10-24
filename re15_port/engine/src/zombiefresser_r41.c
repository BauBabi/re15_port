/*
 * Runde 41 Spur I "zombiefresser" — RE1.5-Deskriptor 0x84/0x85 = Liegender mit Naehe-Wecker (nicht Fresser).
 * Belege und Abbildung: include/re15_zombiefresser_r41.h, Dossier analysis/befunde_runde41/I_zombiefresser.md.
 */
#include "re15_zombiefresser_r41.h"

#include "re15_damage.h"      /* re15_engine_rand8 = FUN_8001af20 (RE1.5-Zufall) */
#include "re15_engine.h"      /* g_engine.frame_count (Messschiene) */
#include <stddef.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

/* RE1.5-snd1-Raum-SE (FUN_800453d0-Pfad) — extern wie in enemy_ai_re2_zombie.c / wasserzombie_r38.c. */
extern void re15_audio_room_se(int idx);

static int s_an = 1;
void re15_r41_liegewecker_enable(int an) { s_an = an ? 1 : 0; }
int  re15_r41_liegewecker_enabled(void)  { return s_an; }

/* ---- Messschiene RE15_ZOMBIEFRESSER_LOG=<datei> (env-gegatet, kein Verhalten) ------------------------------ */
#ifdef RE15_PLATFORM_PC
static FILE *zf_log(void)
{
    static int s_init = 0; static FILE *s_f = NULL;
    if (!s_init) {
        s_init = 1;
        const char *p = getenv("RE15_ZOMBIEFRESSER_LOG");
        if (p && *p) s_f = fopen(p, "w");
    }
    return s_f;
}
#define ZF_LOG(...) do { FILE *zf_ = zf_log(); if (zf_) { fprintf(zf_, __VA_ARGS__); fflush(zf_); } } while (0)
#else
#define ZF_LOG(...) do { } while (0)
#endif

static int aktor_slot(const re15_actor_t *e)
{
    ptrdiff_t i = e - g_actors;
    return (i >= 0 && i < RE15_ACTOR_MAX) ? (int)i : -1;
}

int re15_r41_liegewecker_deskriptor(uint8_t beh)
{
    uint8_t sel = (uint8_t)(beh & 0x1fu);                      /* andi v1,v1,0x1f @0x80100b48 (STAGE2) */
    if (!s_an) return 0;
    return (beh & 0x80u) != 0u && (sel == 4u || sel == 5u);    /* Tor @0x80100b40-44, sel 4 @0x80100b4c-50,
                                                                * sel 5 @0x80100b90-94 */
}

int re15_r41_liegewecker_liegt(const re15_actor_t *e)
{
    if (!e) return 0;
    return re15_r41_liegewecker_deskriptor(e->grid_id);       /* P3 @0x801039d0 / RE2-P4 loeschen +0x9 */
}

void re15_r41_liegewecker_init(re15_actor_t *e)
{
    if (!re15_r41_liegewecker_liegt(e)) return;
    e->ai_timer = (int16_t)RE15_R41_UNSCHARF;
    ZF_LOG("[r41-wecker] F%u slot=%d INIT grid=%02x Wort=%d/%d/%d f10e=%04x\n",
           (unsigned)g_engine.frame_count, aktor_slot(e), e->grid_id, e->state, e->sub_state_1,
           e->sub_state_2, e->re2z_f10e);
}

int re15_r41_liegewecker_re2_tick(re15_actor_t *e, const re15_actor_t *pl)
{
    (void)pl;                                                   /* der Wecker liest nur +0x1d0 (kein HP-Test,
                                                                 * @0x80103814-84 hat keinen Spieler-Zugriff) */
    if (!re15_r41_liegewecker_liegt(e)) return 0;
    if (!(e->re2z_f10e & 0x4000u)) return 1;                    /* Latch frei: die RE2-Kette (P2..P4) laeuft */
    /* DECIDE = Wecker 0x80103814 (STAGE2) — einmalig (Original: +0x6 == 0 @0x80103838-40). */
    if (e->ai_timer == (int16_t)RE15_R41_UNSCHARF && e->ai_dist < RE15_R41_WECK_DIST) {
        e->ai_timer = (int16_t)(re15_engine_rand8() & RE15_R41_WARTE_MASKE);   /* @0x80103848-60 */
        ZF_LOG("[r41-wecker] F%u slot=%d WECKT d=%u warte=%d\n", (unsigned)g_engine.frame_count,
               aktor_slot(e), (unsigned)e->ai_dist, (int)e->ai_timer);
    }
    /* ANIMATE P1 @0x80103964-98: im selben Takt wie der Wecker (DECIDE dann ANIMATE, Dispatcher 0x80101710). */
    if (e->ai_timer != (int16_t)RE15_R41_UNSCHARF) {
        int16_t was = e->ai_timer;                              /* lhu v1,156 @0x80103964 */
        e->ai_timer = (int16_t)(was - 1);                       /* sh v0,156 (Delay-Slot) @0x80103974 */
        if (was == 0) {                                         /* bne v1,zero @0x80103970 -> P2 @0x80103984 */
            e->re2z_f10e &= (uint16_t)~0x4000u;                 /* RE2: der Latch-Clear `andi 0xbfff` @0x80104F0C
                                                                 * (EXEC[7] P1 @0x8010381C-28 geht dann nach P2) */
            ZF_LOG("[r41-wecker] F%u slot=%d STEHT AUF (Latch frei) d=%u\n", (unsigned)g_engine.frame_count,
                   aktor_slot(e), (unsigned)e->ai_dist);
        }
    }
    return 1;
}

int re15_r41_liegewecker_re15_decide(re15_actor_t *e)
{
    if (!s_an || !e) return 0;
    {   uint8_t nib = (uint8_t)(e->grid_id & 0x0fu);
        if (nib < 4u || nib > 6u) return 0;                     /* Dispatcher @0x80117910[4..6] (STAGE2) */
    }
    if (e->sub_state_1 != 0) return 0;                          /* nur Zeile 0 ist belegt (Tabelle @0x80117acc) */
    if (e->ai_dist < RE15_R41_WECK_DIST && e->sub_state_2 == 0) {   /* @0x80103824-40 */
        e->sub_state_2 = 1;                                     /* sb v0,6 @0x8010384c */
        e->ai_timer = (int16_t)(re15_engine_rand8() & RE15_R41_WARTE_MASKE);   /* @0x80103848-60 */
        if ((re15_engine_rand8() & RE15_R41_MOAN_MASKE) == 0u)  /* @0x8010385c-68 */
            re15_audio_room_se(RE15_R41_MOAN_SE);               /* jal 0x800453d0 a0=5 @0x80103870-74 */
        ZF_LOG("[r41-wecker] F%u slot=%d WECKT(RE1.5) d=%u warte=%d\n", (unsigned)g_engine.frame_count,
               aktor_slot(e), (unsigned)e->ai_dist, (int)e->ai_timer);
    }
    return 1;
}
