/* probe_skept_kraehe_takt.c - UNABHAENGIGE NACHMESSUNG des Kraehen-Treffertakts (Typ 0x21).
 *
 * Prueft den Befund "Kraehe 39-53 Bilder statt 15" und WELCHES Gate sperrt.
 * Reine Messsonde - veraendert keinen Engine-Code.
 *
 * Original-Soll selbst gelesen aus info/re2leon/PSX.EXE (t_addr 0x80010000 @0x18,
 * Code ab Datei-Offset 0x800):
 *   0x800A6A88[0x21] @0x800A6B0C = 0x800A45A0 ; Stun = (Zeile[+4] >> 9) & 0x7F = 15 fuer alle 22
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
#include "re15_math.h"

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static uint8_t *s_em = NULL;
static int load_bank(const char *base, uint8_t type)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { printf("WARN: CDEMD0.EMS nicht lesbar (%s)\n", emsp); return 0; }
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, ems_size, idx, &off, &len) != 0) {
        printf("WARN: Blob fuer Typ 0x%02X nicht gefunden\n", type); free(ems); return 0;
    }
    s_em = (uint8_t *)malloc(len);
    memcpy(s_em, ems + off, len);
    free(ems);
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) { printf("WARN: keine Bank-Slots\n"); return 0; }
    if (re15_emd_parse_container(s_em, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) {
        printf("WARN: EMD-Parse fehlgeschlagen\n"); return 0;
    }
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_em, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_em, len, &eb->skel_victim, &eb->anim_victim) == 0);
    return 1;
}

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

/* mode 0 = Bestand | 3 = Freigabe im Root (Hunde-Muster, flankengetriggert)
 * 4 = Freigabe im Root (pegelgetriggert) | 2 = zusaetzliches AUSSCHLUSS-Gate */
static void lauf(uint8_t type, int flavor, int weapon, int frames, int verbose, int mode)
{
    re15_actor_init();
    re15_ai_flavor_set(flavor);
    re15_actor_t *PL = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *E  = &g_actors[1];
    memset(PL, 0, sizeof *PL);
    memset(E,  0, sizeof *E);
    PL->active = 1; PL->hp = 100; PL->x = 0; PL->z = 0; PL->floor = 0; PL->rot_y = 0;
    E->active = 1; E->type = type; E->state = 0; E->hp = 0; E->floor = 0;
    E->x = 1200; E->z = 0;

    for (int i = 0; i < 3; i++) {
        E->x = 1200; E->z = 0;
        int32_t dx = PL->x - E->x, dz = PL->z - E->z;
        int64_t d2 = (int64_t)dx*dx + (int64_t)dz*dz;
        uint32_t r = 0; while ((int64_t)(r+1)*(r+1) <= d2) r++;
        E->ai_dist = r;
        if (type == 0x21) { if (flavor == RE15_AI_FLAVOR_RE2) re15_re2crow_tick(1); else re15_enemy_ai_run_all(1); }
        else              { if (flavor == RE15_AI_FLAVOR_RE2) re15_re2dog_tick(1);  else re15_enemy_ai_run_all(1); }
    }
    E->hp = 30000;
    re15_enemy_apply_hitbox(E, type);

    int last_hp = E->hp;
    int hf[512]; int n = 0;
    for (int f = 0; f < frames; f++) {
        E->x = 1200; E->z = 0; E->y = 0;
        PL->x = 0; PL->z = 0; PL->y = 0; PL->rot_y = 0;
        E->aim_band = 2;
        if (E->hp < 20000) E->hp = 30000;
        last_hp = E->hp;
        uint8_t hr_before = E->hit_react; uint8_t d3_before = E->re2z_self1d3;
        uint8_t st_before = E->state;
        re15_player_weapon_fire((uint8_t)weapon);
        int hit = (E->hp != last_hp);
        if (hit) { if (n < 512) hf[n] = f; n++; last_hp = E->hp; }
        if (verbose && f < 70)
            printf("      f%-3d st=%d sub1=%-2d hr=0x%02X d3=0x%02X %s\n",
                   f, st_before, E->sub_state_1, hr_before, d3_before, hit ? "<== TREFFER" : "");
        int32_t dx = PL->x - E->x, dz = PL->z - E->z;
        int64_t d2 = (int64_t)dx*dx + (int64_t)dz*dz;
        uint32_t r = 0; while ((int64_t)(r+1)*(r+1) <= d2) r++;
        E->ai_dist = r;
        uint8_t d3_pre = E->re2z_self1d3;
        if (type == 0x21) { if (flavor == RE15_AI_FLAVOR_RE2) re15_re2crow_tick(1); else re15_enemy_ai_run_all(1); }
        else              { if (flavor == RE15_AI_FLAVOR_RE2) re15_re2dog_tick(1);  else re15_enemy_ai_run_all(1); }
        /* SIMULATION der Einbauwege - NUR in der Sonde, kein Engine-Code veraendert. */
        if (mode == 3) {   /* Freigabe im Root, flankengetriggert (exakt das Hunde-Muster
                            * aus bfc2c71d: beim Dekrement auf 0 den +0x93-Riegel loesen) */
            if ((d3_pre & 0x7fu) != 0u && (E->re2z_self1d3 & 0x7fu) == 0u)
                E->hit_react &= (uint8_t)~1u;
        } else if (mode == 4) {   /* Freigabe im Root, pegelgetriggert */
            if ((E->re2z_self1d3 & 0x7fu) == 0u)
                E->hit_react &= (uint8_t)~1u;
        }
        (void)d3_pre;
    }
    printf("  [mode %d typ 0x%02X %s w%-2d] %3d Treffer/%d Bilder | Bilder:",
           mode, type, flavor == RE15_AI_FLAVOR_RE2 ? "RE2 " : "RE15", weapon, n, frames);
    int lim = n < 14 ? n : 14;
    for (int i = 0; i < lim; i++) printf(" %d", hf[i]);
    if (n > lim) printf(" ...");
    printf("  | Abstaende:");
    for (int i = 1; i < lim; i++) printf(" %d", hf[i] - hf[i-1]);
    printf("\n");
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    re15_actor_init();
    if (!load_bank(base, 0x21)) { printf("SKIP (keine Kraehen-Bank)\n"); return 0; }

    printf("=== NACHMESSUNG Kraehen-Treffertakt (Typ 0x21) ===\n");
    printf("-- BESTAND (mode 0), RE2-Flavour --\n");
    for (int w = 1; w <= 4; w++) lauf(0x21, RE15_AI_FLAVOR_RE2, w, 300, 0, 0);
    printf("-- FREIGABE IM ROOT, flankengetriggert (mode 3) --\n");
    for (int w = 1; w <= 4; w++) lauf(0x21, RE15_AI_FLAVOR_RE2, w, 300, 0, 3);
    printf("-- FREIGABE IM ROOT, pegelgetriggert (mode 4) --\n");
    for (int w = 1; w <= 4; w++) lauf(0x21, RE15_AI_FLAVOR_RE2, w, 300, 0, 4);
    printf("-- BESTAND RE1.5-Flavour (Gegenprobe) --\n");
    for (int w = 1; w <= 4; w++) lauf(0x21, RE15_AI_FLAVOR_RE15, w, 300, 0, 0);
    printf("-- Gegenprobe HUND 0x20 (seit v0.8.2 gefixt) --\n");
    lauf(0x20, RE15_AI_FLAVOR_RE2, 2, 300, 0, 0);
    printf("-- ALLE 22 WAFFEN: Bestand (mode 0) vs Freigabe (mode 3) --\n");
    for (int w = 0; w < 22; w++) { lauf(0x21, RE15_AI_FLAVOR_RE2, w, 200, 0, 0);
                                   lauf(0x21, RE15_AI_FLAVOR_RE2, w, 200, 0, 3); }
    printf("PROBE-OK\n");
    return 0;
}
