/* probe_p3_spider_stalk.c — MESS-SONDE Phase 3 (2026-09-19), kein add_test.
 *
 * Offener Punkt 3 des Dossiers (gator-finisher-sounds.md §5 "Offen"):
 *   "Spinnen-Szenario A erreicht Sub 7 in 1500 F nicht — die Stalk->Angriff-Bedingungen
 *    (Sub 5/6 @0x80100B80) sind hier nicht vermessen."
 *
 * WAS DIE SONDE ZEIGT (und was die Formulierung des Dossiers korrigiert): Sub 5/6 ist
 * gar nicht der Starter des Angriffs. Die Anpirsch-Zustaende drehen nur (re2s_m0_stalk,
 * enemy_ai_re2_spider.c:578) und geben mit `re2s_word(e, 1u)` (@0x80100CC0) an SUB 0
 * zurueck. Den Angriff startet allein die Entscheidungsleiter in Sub 0
 * (re2s_m0_sub0 :461, @0x80100784) — und zwar ueber ein ABSTANDS-BAND:
 *
 *   Vor-Gate  @0x80100830-38 : +0x218 (re2s_t218) muss 0 sein, sonst Ende
 *   Zweig 1   @0x80100840-64 : d < 0x1d4c (7500) && arc(128) == 0 && d >= 0xbb9 (3001)
 *                              -> Zustandswort 0x701 = Sub 7 (ANGRIFF)
 *   Zweig 2   @0x8010086C-AC : d < 0x1770 (6000) && arc(128) == 0 && d >= 0x3e9 (1001)
 *                              -> `srav 832,rand&0xF` (@0x80100898): 3 von 16 Ziehungen
 *                                 auf Sub 8, sonst Sub 7
 *   Stalk     @0x801007E4-FC : arc(1024) != 0 && d < 0x1f40 (8000) -> Sub 5
 *
 * Beide Angriffs-Zweige haben also eine MINDEST-Distanz (1001 bzw. 3001). Szenario A des
 * Vorgaengers stellte den Spieler 900 Einheiten vor die Spinne — UNTER beide Schwellen.
 * Deshalb wurde Sub 7 dort nie erreicht; die Spinne hatte keinen Defekt, sie stand zu nah.
 *
 * Die Sonde faehrt denselben natuerlichen Ablauf wie Szenario A (ab INIT, keine erzwungenen
 * Zustaende) ueber ein Abstands-Raster und protokolliert je Abstand: erreichte Sub-Zustaende,
 * Tick des ersten Sub 7, Angriffe, Bisse, SE-Rufe.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_collision.h"
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

extern void re15_re2spider_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int), int baby);
extern int  re15_re2spider_tick(int slot);
extern int  re15_re2spider_owns(const re15_actor_t *e);

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz;
    return b;
}

static int s_se_calls = 0, s_se_attack = 0;
static void spy_bank(int bank) { (void)bank; }
static void spy_se(int id, int flag2000)
{
    s_se_calls++;
    if (id == 1 && !flag2000) s_se_attack++;   /* re2s_attack SE 1 @0x80105B34-38 */
}

/* Die Spinne sitzt wie in probe_r16_spider_se auf der Plattform (Memory
 * reai-v2-gator-bosskampf: Sitz (-700,-17800) y=-1800); der Spieler wird auf derselben
 * Ebene in +X davor gestellt, also genau in Blickrichtung (rot_y = 0). */
#define SP_X (-700)
#define SP_Y (-1800)
#define SP_Z (-17800)

int main(void)
{
    static const int ABST[] = { 500, 900, 1000, 1001, 1200, 2000, 3000, 3001,
                                4000, 5000, 5999, 6000, 7000, 7499, 7500, 9000 };
    size_t n = 0, es = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    uint8_t *ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &es);
    re15_enemy_bank_t *eb;
    re15_actor_t *pl, *e;
    int slot, k;

    if (!buf || !ems) { printf("FAIL assets\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL RDT\n"); return 1; }
    g_room_rdt_ok = 1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;
    eb = re15_enemy_alloc(0x25u);
    if (!eb || re2_ems_load_bank(ems, es, 0x25, eb, NULL) != 0) { printf("FAIL Bank 0x25\n"); return 1; }
    eb->buf = NULL; eb->ok = 1;
    re15_re2spider_audio_hook(spy_se, spy_bank, 0);

    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    slot = RE15_ACTOR_MAX - 2; e = &g_actors[slot];

    printf("=== Sub-0-Entscheidungsleiter der RE2-Spinne, Abstands-Raster ===\n");
    printf("Gates: Zweig1 3001 <= d < 7500 (@0x80100840/@0x8010084C-58) | "
           "Zweig2 1001 <= d < 6000 (@0x8010086C-80) | Stalk d < 8000 (@0x801007E4-F4)\n");
    printf("Ablauf je Zeile: natuerlich ab INIT, 1500 Bilder, keine erzwungenen Zustaende "
           "(= Szenario A des Vorgaengers, nur mit anderem Abstand)\n\n");
    printf("  Abstand | Sub7 ab Tick | Ticks in Sub7 | Sub5/6-Ticks | Bisse | Angriffs-SE | "
           "besuchte Sub-Zustaende\n");
    printf("  --------+--------------+---------------+--------------+-------+-------------+"
           "-----------------------\n");

    for (k = 0; k < (int)(sizeof ABST / sizeof ABST[0]); k++) {
        int f, sub7_ab = -1, sub7_n = 0, stalk_n = 0, bisse = 0, lasthp;
        unsigned besucht = 0u;
        int se0 = s_se_calls, sa0 = s_se_attack;
        char liste[64]; int li = 0, b;
        char ab_txt[16];

        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x25u;
        e->x = SP_X; e->y = SP_Y; e->z = SP_Z; e->grid_id = 0;
        e->state = 0; e->em_flag_id = 0xFF; e->rot_y = 0;
        re15_enemy_apply_hitbox(e, 0x25u);
        pl->active = 1; pl->type = 0; pl->hp = 200;
        pl->x = SP_X + ABST[k]; pl->z = SP_Z; pl->y = SP_Y; pl->floor = 1;
        lasthp = pl->hp;
        re15_damage_seed_rng(0x2545f491u);      /* gleiche Ziehung je Zeile */

        for (f = 0; f < 1500; f++) {
            e->hit_react = 0; pl->hit_react = 0; pl->state = 0;
            pl->x = SP_X + ABST[k]; pl->z = SP_Z; pl->y = SP_Y;   /* Spieler steht still */
            e->re2z_f10e = (uint16_t)e->grid_id;
            re15_re2spider_tick(slot);
            if (e->re2s_mode222 == 0 && e->sub_state_1 < 32)
                besucht |= 1u << e->sub_state_1;
            if (e->sub_state_1 == 7 && e->re2s_mode222 == 0) {
                if (sub7_ab < 0) sub7_ab = f;
                sub7_n++;
            }
            if ((e->sub_state_1 == 5 || e->sub_state_1 == 6) && e->re2s_mode222 == 0) stalk_n++;
            if (pl->hp < lasthp) { bisse++; lasthp = pl->hp; }
            if (pl->hp <= 0) { pl->hp = 200; lasthp = 200; }
        }
        for (b = 0; b < 16; b++)
            if (besucht & (1u << b)) li += snprintf(liste + li, sizeof liste - (size_t)li,
                                                    "%s%d", li ? "," : "", b);
        if (!li) snprintf(liste, sizeof liste, "(keine)");
        if (sub7_ab >= 0) snprintf(ab_txt, sizeof ab_txt, "%d", sub7_ab);
        else              snprintf(ab_txt, sizeof ab_txt, "nie");
        printf("  %7d | %12s | %13d | %12d | %5d | %11d | %s   [SE %d]\n",
               ABST[k], ab_txt, sub7_n, stalk_n, bisse, s_se_attack - sa0,
               liste, s_se_calls - se0);
        e->active = 0;
    }

    printf("\nLESART: unterhalb von d = 1001 (@0x8010086C-80 `sltiu`-Paar) kennt die\n"
           "Sub-0-Leiter KEINEN Angriffs-Zweig — die Spinne pirscht (Sub 5/6) und laeuft,\n"
           "beisst aber nicht. Szenario A des Vorgaengers stand bei d = 900.\n");
    return 0;
}
