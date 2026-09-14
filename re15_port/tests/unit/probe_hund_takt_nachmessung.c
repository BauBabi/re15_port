/* probe_hund_takt_nachmessung.c — UNABHAENGIGE NACHMESSUNG des Hunde-Treffertakts.
 *
 * Prueft den Befund "Trefferabstand ist 15 Bilder fuer w2 und w3".
 * Misst je Waffe die Bildnummern ALLER Treffer im Dauerfeuer, fuer RE2- und RE1.5-Flavour.
 * Reine Messsonde, veraendert keinen Engine-Code.
 *
 * Original-Werte selbst gelesen aus info/re2leon/PSX.EXE (t_addr 0x80010000 @0x18,
 * Code ab Datei-Offset 0x800):
 *   Zeile = 0x800A6A88[typ] + (row-1)*0x14 ; Stun = (Zeile[+4] >> 9) & 0x7F
 *   Hund 0x20 -> 0x800A4424 ; row1 @0x800A4424 +4=0x078F1E0A -> 15
 *                             row3 @0x800A444C +4=0x078F1E0A -> 15
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

static uint8_t *s_em20 = NULL;
static int load_em020(const char *base)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { printf("WARN: CDEMD0.EMS nicht lesbar (%s)\n", emsp); return 0; }
    int idx = re15_ems_index_for_type(0x20);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, ems_size, idx, &off, &len) != 0) {
        printf("WARN: EM020-Blob nicht gefunden\n"); free(ems); return 0;
    }
    s_em20 = (uint8_t *)malloc(len);
    memcpy(s_em20, ems + off, len);
    free(ems);
    re15_enemy_bank_t *eb = re15_enemy_find(0x20);
    if (!eb) eb = re15_enemy_alloc(0x20);
    if (!eb) { printf("WARN: keine Bank-Slots\n"); return 0; }
    if (re15_emd_parse_container(s_em20, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) {
        printf("WARN: EMD-Parse fehlgeschlagen\n"); return 0;
    }
    eb->ok = 1; eb->buf = NULL;
    return 1;
}

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

/* Ein Dauerfeuer-Lauf. flavor: RE2 oder RE15. Gibt die Trefferbilder aus. */
static int g_force_release = 0;
static void lauf(int flavor, int weapon, int frames)
{
    re15_actor_init();
    re15_ai_flavor_set(flavor);
    re15_actor_t *PL  = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *DOG = &g_actors[1];
    memset(PL, 0, sizeof *PL);
    memset(DOG, 0, sizeof *DOG);
    PL->active = 1; PL->hp = 100; PL->x = 0; PL->z = 0; PL->floor = 0; PL->rot_y = 0;
    DOG->active = 1; DOG->type = 0x20; DOG->state = 0; DOG->hp = 0; DOG->floor = 0;
    DOG->x = 1200; DOG->z = 0;

    for (int i = 0; i < 3; i++) {
        DOG->x = 1200; DOG->z = 0;
        int32_t dx = PL->x - DOG->x, dz = PL->z - DOG->z;
        int64_t d2 = (int64_t)dx*dx + (int64_t)dz*dz;
        uint32_t r = 0; while ((int64_t)(r+1)*(r+1) <= d2) r++;
        DOG->ai_dist = r;
        if (flavor == RE15_AI_FLAVOR_RE2) re15_re2dog_tick(1);
        else                              re15_enemy_ai_run_all(1);
    }
    DOG->hp = 5000;                  /* nicht sterben lassen */
    re15_enemy_apply_hitbox(DOG, 0x20);

    int last_hp = DOG->hp;
    int hf[256]; int n = 0;
    for (int f = 0; f < frames; f++) {
        DOG->x = 1200; DOG->z = 0; DOG->y = 0;
        PL->x = 0; PL->z = 0; PL->y = 0; PL->rot_y = 0;
        DOG->aim_band = 2;
        if (DOG->hp < 1000) DOG->hp = 5000;     /* HP auffuellen, Takt bleibt unberuehrt */
        last_hp = DOG->hp;
        re15_player_weapon_fire((uint8_t)weapon);
        if (DOG->hp != last_hp) {
            if (n < 256) hf[n] = f;
            n++;
            last_hp = DOG->hp;
        }
        int32_t dx = PL->x - DOG->x, dz = PL->z - DOG->z;
        int64_t d2 = (int64_t)dx*dx + (int64_t)dz*dz;
        uint32_t r = 0; while ((int64_t)(r+1)*(r+1) <= d2) r++;
        DOG->ai_dist = r;
        if (flavor == RE15_AI_FLAVOR_RE2) re15_re2dog_tick(1);
        else                              re15_enemy_ai_run_all(1);
        if (g_force_release) DOG->hit_react &= (uint8_t)~1u;   /* Gegenprobe: Riegel manuell frei */
    }
    printf("  [%s w%-2d] %3d Treffer in %d Bildern | Bilder:",
           flavor == RE15_AI_FLAVOR_RE2 ? "RE2 " : "RE15", weapon, n, frames);
    int lim = n < 12 ? n : 12;
    for (int i = 0; i < lim; i++) printf(" %d", hf[i]);
    if (n > lim) printf(" ...");
    printf("  | Abstaende:");
    for (int i = 1; i < lim; i++) printf(" %d", hf[i] - hf[i-1]);
    printf("  hit_react=0x%02x c1d3=0x%02x\n", DOG->hit_react, DOG->re2z_self1d3);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    re15_actor_init();
    if (!load_em020(base)) { printf("SKIP (keine Bank)\n"); return 0; }

    printf("=== NACHMESSUNG Hunde-Treffertakt (Typ 0x20) ===\n");
    printf("-- RE2-Flavour --\n");
    for (int w = 0; w <= 21; w++) lauf(RE15_AI_FLAVOR_RE2, w, 200);
    printf("-- RE1.5-Flavour --\n");
    for (int w = 0; w <= 21; w++) lauf(RE15_AI_FLAVOR_RE15, w, 200);
    printf("-- GEGENPROBE RE2: +0x93 Bit0 jedes Bild manuell frei (w0..w6) --\n");
    g_force_release = 1;
    for (int w = 0; w <= 6; w++) lauf(RE15_AI_FLAVOR_RE2, w, 200);
    g_force_release = 0;
    printf("PROBE-OK\n");
    return 0;
}
