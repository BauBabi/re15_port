/* probe_gegenprobe_hund_luftzeile.c — GEGENPROBE (Messung, kein Pin):
 * Wie lange dauert die RE1.5-Hunde-Hurt-Kette JE HURT-ZEILE (+0x5 = WAFFEN-ID)?
 *
 * Original-Belege (selbst disassembliert, STAGE1.BIN ohne 0x800-Header):
 *   @0x800124BC `sb fp,5(s1)`            -> +0x5 = WAFFEN-ID (fp), danach
 *   @0x80012528/38 `sb v0,4(s1)`         -> +0x4 = 2 (HURT) bzw. 3 (DEATH)
 *   @0x80012428 `sb zero,7(s1)`          -> +0x7 = 0 (Phase 0 laeuft)
 *   @0x801109A8 `lbu v0,5(v0)` + Tabelle @0x80121018 -> Zeile = Waffen-ID
 *     Zeilen 0-6,12,19,20 -> 0x801109E0 (BODEN), 7-11,13-18,21 -> 0x80110B9C (LUFT)
 *   BODEN: Clip 6 (18 Bilder) + 1 Tick Phase 2 -> 19 Ticks, +0x93=0 @0x80110B88
 *   LUFT : Clip 7 (15 Bilder), dann Phase 2 mit ZAEHLER +0x9E, geseedet
 *          @0x80110C48-68 `jal rng` / `addiu v0,v0,45` / `andi v0,v0,0x3f`
 *          / `sb v0,158(v1)`, heruntergezaehlt @0x80110D4C-5C -> +0x93=0 @0x80110DAC
 *          erst wenn der Zaehler VOR dem Dekrement 0 war => 16 + k Ticks, k=0..63.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_ems.h"

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return NULL; }
    size_t rd = fread(b, 1, (size_t)sz, f); fclose(f);
    if (rd != (size_t)sz) { free(b); return NULL; }
    *out_size = (size_t)sz; return b;
}
static uint8_t *s_em20 = NULL;
static int load_em020(const char *base)
{
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(p, &n); if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x20); size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) { free(ems); return 0; }
    s_em20 = (uint8_t *)malloc(len); memcpy(s_em20, ems + off, len); free(ems);
    re15_enemy_bank_t *eb = re15_enemy_find(0x20); if (!eb) eb = re15_enemy_alloc(0x20);
    if (!eb) return 0;
    if (re15_emd_parse_container(s_em20, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    printf("EM020: %d Clips, clip6=%d clip7=%d\n", eb->anim.clip_count,
           eb->anim.clips[6].frame_count, eb->anim.clips[7].frame_count);
    return 1;
}
#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

/* Original-Zeilenklassifikation, roh aus @0x80121018 gelesen */
static int row_air(int r) { return (r>=7&&r<=11)||(r>=13&&r<=18)||r==21; }

static int measure_row(int weapon)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_t *PL = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *DOG = &g_actors[1];
    memset(PL, 0, sizeof *PL); memset(DOG, 0, sizeof *DOG);
    PL->active = 1; PL->hp = 100; PL->x = 0; PL->z = 0; PL->rot_y = 0;
    DOG->active = 1; DOG->type = 0x20; DOG->state = 0; DOG->hp = 0;
    DOG->x = 1200; DOG->z = 0;
    for (int i = 0; i < 3; i++) re15_enemy_ai_run_all(1);
    DOG->hp = 20000;
    re15_enemy_apply_hitbox(DOG, 0x20);

    int last_hp = DOG->hp, first = -1, second = -1, nh = 0;
    for (int f = 0; f < 400; f++) {
        DOG->x = 1200; DOG->z = 0; DOG->y = 0;
        PL->x = 0; PL->z = 0; PL->y = 0; PL->rot_y = 0;
        DOG->aim_band = 2;
        re15_player_weapon_fire(weapon);
        if (DOG->hp != last_hp) {
            if (nh == 0) { first = f;
                printf("  w=%2d Treffer f=%d -> +0x4=%d +0x5=%d (%s) motion=%d\n",
                       weapon, f, DOG->state, DOG->sub_state_1,
                       row_air(DOG->sub_state_1) ? "LUFT" : "BODEN", DOG->motion);
            } else if (nh == 1) { second = f; }
            nh++; last_hp = DOG->hp;
        }
        re15_enemy_ai_run_all(1);
        if (second >= 0) break;
    }
    return (first >= 0 && second >= 0) ? (second - first) : -1;
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    re15_actor_init();
    if (!load_em020(base)) { printf("SKIP\n"); return 0; }

    printf("\n--- Kettenlaenge je Waffen-/Hurt-Zeile (RE1.5-Hund) ---\n");
    for (int w = 1; w <= 21; w++) {
        int d = measure_row(w);
        if (d > 0) printf("  ZEILE %2d (%s): %d Ticks (%.2f s)\n",
                          w, row_air(w) ? "LUFT " : "BODEN", d, d / 30.0);
    }

    /* Streuung der LUFT-Zeile ueber viele Anlaeufe: seed variiert mit dem RNG */
    printf("\n--- LUFT-Zeile: Streuung ueber 40 Anlaeufe (Zeile 8) ---\n");
    int mn = 9999, mx = -1; long sum = 0; int cnt = 0;
    for (int i = 0; i < 40; i++) {
        int d = measure_row(8);
        if (d > 0) { if (d < mn) mn = d; if (d > mx) mx = d; sum += d; cnt++; }
    }
    if (cnt) printf("  n=%d  min=%d  max=%d  mittel=%.1f Ticks (%.2f s)\n",
                    cnt, mn, mx, (double)sum / cnt, ((double)sum / cnt) / 30.0);
    printf("PROBE-OK\n");
    return 0;
}
