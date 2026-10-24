/*
 * RE1.5 Rebuilt — Runde 41 Spur J: Spieler-Plc_motion(2,c) = W-Bank-Clip c (allgemeine Regel, Belege im Header
 * include/re15_wclip_r41.h, Dossier analysis/befunde_runde41/J_stage3szenen_a.md §1).
 */
#include <stdio.h>
#include "re15_wclip_r41.h"
#include "re15_actor.h"
#include "re15_engine.h"   /* g_engine.frame_count (Messzeile) */
#include "re15_room.h"     /* g_current_room_id (Messzeile) */

extern int re15_player_wbank_clip_fc(int clip);   /* player_common.c: Bildzahlen der geladenen W-Bank (PLW-EDD) */
extern int re15_player_equipped_weapon(void);     /* re15_damage.h: DAT_800aca5d */

static int s_clip = -1;
static unsigned s_raum = 0;   /* Raum der Phase: der Raumwechsel setzt den Spieler neu auf (cmd 0), die W-Phase endet dort */

void re15_wclip_r41_beginn(int entity, int clip)
{
    if (entity != RE15_WCLIP_R41_SUB || clip < 0) { s_clip = -1; return; }
    s_clip = clip;
    s_raum = g_current_room_id;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[wclip41] ROOM%04X F%u Spieler Plc_motion(2,%d) -> W-Bank-Clip %d (0x80073e30[2] = 0x80050f00, "
                    "Kanal +0x170/+0x174, Waffe %d, %d Bilder)\n", g_current_room_id, (unsigned)g_engine.frame_count,
            clip, clip, re15_player_equipped_weapon(), re15_player_wbank_clip_fc(clip));
#endif
}

int re15_wclip_r41_clip(void)
{
    /* Gemessen (Lauf m3060_tuer, RE15_ANIM_TRACE): ohne diese Pruefung zeichnete das ERSTE Bild in ROOM3020 Leon noch mit
     * W-Clip 6 (Halten laeuft erst im Spielschritt danach). Original nach dem Wechsel (Savestate o3060_s2_t04, ROOM3020):
     * Spieler cmd 0 / Sub 0 / Phase 0 — der Plc-Executor 0x80030660 (cmd 4) und damit Sub 2 (0x80050f00) laeuft nicht mehr. */
    if (s_clip >= 0 && g_current_room_id != s_raum) s_clip = -1;
    return s_clip;
}

void re15_wclip_r41_halten(void)
{
    if (s_clip < 0) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (pl->state != 4 || (int)pl->motion != s_clip) {   /* anderer Plc-Befehl: W-Bank-Phase vorbei */
        s_clip = -1;
        return;
    }
    if (pl->anim_flags & 0x04) return;                     /* Schleife: Phase 1 erneut (@0x80051004/@0x80051010) */
    const int fc = re15_player_wbank_clip_fc(s_clip);
    if (fc >= 1 && (int)pl->anim_frame > fc - 1) pl->anim_frame = (uint16_t)(fc - 1);   /* Phase 2 halten @0x80050fec */
}
