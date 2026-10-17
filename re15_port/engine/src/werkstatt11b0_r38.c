/* werkstatt11b0_r38.c — Runde 38 Spur C "werkstatt11b0": Schiebe-Ton-Entscheidung und Bank im Uebergabebild des
 * Schub-Subs 0x13. Herleitung mit Adressen: include/re15_werkstatt11b0_r38.h, Dossier
 * analysis/befunde_runde38/C_werkstatt11b0.md §2/§3. Die Szene (Sub 0x13, Erweiterung) liegt in szenenleon_r36.c.
 */
#include "re15_werkstatt11b0_r38.h"
#include "re15_szenenleon_r36.h"   /* RE15_SZL_MODUS_SCHUB, RE15_SZL_CLIP_DREH, RE15_SZL_CLIP_ANSATZ */
#include "re15_actor.h"
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include "re15_engine.h"           /* g_engine.frame_count — nur Messschrieb */
#endif

int (*re15_w11b0_snd0_hook)(int se_id) = NULL;

/* ===================================================================================================== */
/*  (2) Schiebe-Ton: zustaendig nur fuer den leeren Satz 7 in ROOM11B0                                   */
/* ===================================================================================================== */
int re15_w11b0_ergaenzung_noetig(unsigned room_id, const uint8_t *raum_edt, int raum_edt_size, int se_id)
{
    if (se_id != RE15_W11B0_SCHUB_SATZ) return 0;                 /* nur der Schiebe-Satz (@0x80052acc)      */
    if ((uint16_t)room_id != RE15_W11B0_RAUM) return 0;
    if (!raum_edt || raum_edt_size < (se_id + 1) * 4) return 0;
    const uint8_t *e = raum_edt + se_id * 4;
    return (e[2] == 0 && e[3] == 0) ? 1 : 0;                      /* re15_edt_decode: empty = Byte 2/3 == 0  */
}

/* ===================================================================================================== */
/*  (3) Bank im Uebergabebild Sub 0x13 -> Sub 6                                                          */
/* ===================================================================================================== */
static uint8_t s_halte[RE15_ACTOR_MAX];   /* 1 = Phase 7 hat die Pose von Phase 6 stehen gelassen */

static int slot_von(const re15_actor_t *e)
{
    int i = (int)(e - g_actors);
    return (i >= 0 && i < RE15_ACTOR_MAX) ? i : -1;
}

int re15_w11b0_schub_kanal(const re15_actor_t *e)
{
    if (!e || e->type < 0x40 || e->type > 0x4d) return -1;        /* NPC-Familie (Tabelle 0x80076ca0)          */
    if (e->state != 4 || e->walk_active) return -1;
    const int i = slot_von(e);
    if (e->sub_state_1 == RE15_SZL_MODUS_SCHUB) {
        if (i >= 0) s_halte[i] = 0;
        /* Die Bank folgt dem posierten Clip: Clip 5 posiert Phase 1 aus +0x170/+0x174 (@0x8005294c) — auch im
         * Bild, an dessen Ende +0x6 schon 2 ist (@0x800529a0-b0, Phase 2 setzt Clip 0x11 erst im Folgebild);
         * 0x11/0x12 posieren Phasen 2..6 aus +0x84/+0x16c (@0x80052a08, @0x80052af4, @0x80052b98). */
        return (e->motion == RE15_SZL_CLIP_DREH) ? 2 : 1;
    }
    if (i >= 0 && s_halte[i]) {
        /* Uebergabebild: Phase 7 (@0x80052bc0..@0x80052bf4) ruft kein anim_set; Sub 6 [0]/[1] (@0x80051844/
         * @0x80051878) posiert erst im Folgebild. Bis dahin steht die Pose von Phase 6 = Loco Clip 0x11 Bild 0. */
        if (e->sub_state_1 == 6 && e->sub_state_2 == 0 && e->motion == RE15_SZL_CLIP_ANSATZ) return 1;
        s_halte[i] = 0;                                           /* Sub 6 hat posiert: Uebergabe vorbei     */
    }
    return -1;
}

void re15_w11b0_schub_uebergabe(re15_actor_t *e)
{
    if (!e) return;
    const int i = slot_von(e);
    /* +0x95 = 0: FUN_8001f3bc setzt am Clip-Ende von Phase 6 das Bild zurueck (@0x8001f63c `sb zero,149(v1)`).
     * Vorwaerts-Bild 0 von Clip 0x11 = genau das zuletzt (rueckwaerts) posierte Bild (Index (Bildzahl-9)-1 = 0,
     * FUN_8001f314). Das Richtungsbit hat der Aufrufer schon geloescht (Sub 6 spielt vorwaerts). */
    e->anim_frame = 0;
    if (i < 0) return;
    s_halte[i] = 1;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[w11b0-r38] F%u Aktor %d Typ 0x%02X: Schub-Sub Phase 7 (@0x80052bc0, kein anim_set) -> Pose "
                    "Loco Clip 0x11 Bild 0 gehalten bis Sub 6 posiert\n",
            (unsigned)g_engine.frame_count, i, (unsigned)e->type);
#endif
}

int re15_w11b0_halte_aktiv(int slot)
{
    return (slot >= 0 && slot < RE15_ACTOR_MAX) ? (int)s_halte[slot] : 0;
}
