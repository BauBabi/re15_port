/* probe_re2_dog_hitwindow.c — MESSUNG (kein Pin): wie lange ist der RE2-Hund im PORT nach
 * einem Treffer kein gueltiges Ziel mehr, und wie lange dauert seine Hurt-Kette?
 *
 * Hintergrund (Original-Belege, alle selbst disassembliert):
 *   RE2 (info/re2leon): der Applier FUN_800470C0 hat GENAU VIER Kandidaten-Gates
 *     @0x8004712C `andi v0,v0,0x1` (word0 aktiv), @0x80047138 `lbu v0,467(s0)` (+0x1D3 != 0),
 *     @0x80047148 `lh v0,342(s0)` (HP < 0), @0x80047158-64 `andi v0,v0,0xc000` (+0x10E).
 *     KEIN +0x93. Das Zeit-Gate +0x1D3 wird beim Treffer aus der Waffenzeile gesetzt:
 *       80047338 lw v0,4(a1) / 80047340 srl v0,v0,9 / 80047344 andi v0,v0,0x7f /
 *       80047348 or a0,a0,v0 / 8004734C sb a0,467(s1)    (2. Zweig @0x80047588-9C)
 *     Hunde-Zeile (Typ 0x20 -> Tabelle 0x800A4424 + (waffe-1)*0x14): +4 = 0x078F1E0A
 *       -> (>>9)&0x7f = 15 Frames. Root-Tick dekrementiert die Low-7 @0x80100028-3C.
 *   RE1.5 (STAGE1.BIN): der Hund loescht +0x93 erst am Kettenende (@0x80110B88 Boden,
 *     @0x80110DAC Luft) — Kette dort ist aber EIN Clip (Boden Clip 6 = 18 Frames,
 *     Luft Clip 7 = 15 Frames, EM020-EDD).
 *
 * Diese Sonde misst den PORT: Dauerfeuer auf einen RE2-Hund, echte Pfade
 * (re15_player_weapon_fire + re15_re2dog_tick), mit ECHTER EM020-Bank (Clip-Laengen).
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

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

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
    printf("EM020 geladen: %d Clips  [7]=%d  [17]=%d  [18]=%d Frames\n",
           eb->anim.clip_count,
           eb->anim.clips[7].frame_count,
           eb->anim.clips[17].frame_count,
           eb->anim.clips[18].frame_count);
    return 1;
}

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    re15_actor_init();
    if (!load_em020(base)) { printf("SKIP (keine Bank)\n"); return 0; }

    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_t *PL  = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *DOG = &g_actors[1];
    memset(PL, 0, sizeof *PL);
    memset(DOG, 0, sizeof *DOG);
    PL->active = 1; PL->hp = 100; PL->x = 0; PL->z = 0; PL->floor = 0;
    PL->rot_y = 0;
    DOG->active = 1; DOG->type = 0x20; DOG->state = 0; DOG->hp = 0; DOG->floor = 0;
    DOG->x = 1200; DOG->z = 0;

    /* INIT + ein paar Ticks, damit der Hund normal laeuft */
    for (int i = 0; i < 3; i++) {
        int32_t dx = PL->x - DOG->x, dz = PL->z - DOG->z;
        int64_t d2 = (int64_t)dx*dx + (int64_t)dz*dz;
        uint32_t r = 0; while ((int64_t)(r+1)*(r+1) <= d2) r++;
        DOG->ai_dist = r;
        re15_re2dog_tick(1);
    }
    DOG->hp = 5000;   /* nicht sterben lassen */
    re15_enemy_apply_hitbox(DOG, 0x20);   /* Trefferbox wie beim echten Spawn */

    printf("Start: state=%d sub=%d/%d/%d motion=%d hp=%d radius=%d hit_react=0x%02x\n",
           DOG->state, DOG->sub_state_1, DOG->sub_state_2, DOG->sub_state_3,
           DOG->motion, DOG->hp, DOG->hit_radius_min, DOG->hit_react);

    int last_hp = DOG->hp;
    int hit_frames[64]; int nhits = 0;
    int clear_frame = -1, active_frame = -1;

    for (int f = 0; f < 300; f++) {
        /* Position fixieren: die Messung soll nicht an Reichweite/Winkel scheitern */
        DOG->x = 1200; DOG->z = 0; DOG->y = 0;
        PL->x = 0; PL->z = 0; PL->y = 0; PL->rot_y = 0;
        /* Dauerfeuer Handfeuerwaffe (id 2) */
        re15_player_weapon_fire(2);
        if (DOG->hp != last_hp) {
            if (nhits < 64) hit_frames[nhits] = f;
            nhits++;
            printf("  f=%3d TREFFER  hp %d->%d  state=%d sub=%d/%d/%d motion=%d frame=%u hit_react=0x%02x\n",
                   f, last_hp, DOG->hp, DOG->state, DOG->sub_state_1, DOG->sub_state_2,
                   DOG->sub_state_3, DOG->motion, (unsigned)DOG->anim_frame, DOG->hit_react);
            last_hp = DOG->hp;
        }
        int was_latched = (DOG->hit_react & 1);
        int32_t dx = PL->x - DOG->x, dz = PL->z - DOG->z;
        int64_t d2 = (int64_t)dx*dx + (int64_t)dz*dz;
        uint32_t r = 0; while ((int64_t)(r+1)*(r+1) <= d2) r++;
        DOG->ai_dist = r;
        re15_re2dog_tick(1);
        if (was_latched && !(DOG->hit_react & 1) && clear_frame < 0) {
            clear_frame = f;
            printf("  f=%3d +0x93 Bit0 GELOESCHT  state=%d sub=%d/%d/%d motion=%d\n",
                   f, DOG->state, DOG->sub_state_1, DOG->sub_state_2, DOG->sub_state_3, DOG->motion);
        }
        if (nhits >= 1 && DOG->state == 1 && active_frame < 0 && f > hit_frames[0]) {
            active_frame = f;
            printf("  f=%3d ZURUECK IN ACTIVE (state 1) sub=%d/%d/%d motion=%d\n",
                   f, DOG->sub_state_1, DOG->sub_state_2, DOG->sub_state_3, DOG->motion);
        }
    }

    printf("\nERGEBNIS: %d Treffer in 300 Frames Dauerfeuer.\n", nhits);
    if (nhits >= 2)
        printf("  Abstand Treffer1->Treffer2 = %d Frames (%.2f s @30Hz)\n",
               hit_frames[1] - hit_frames[0], (hit_frames[1]-hit_frames[0]) / 30.0);
    if (clear_frame >= 0 && nhits >= 1)
        printf("  +0x93-Bit0 nach %d Frames frei (%.2f s)\n",
               clear_frame - hit_frames[0], (clear_frame - hit_frames[0]) / 30.0);
    if (active_frame >= 0 && nhits >= 1)
        printf("  wieder state=ACTIVE nach %d Frames (%.2f s)\n",
               active_frame - hit_frames[0], (active_frame - hit_frames[0]) / 30.0);
    /* ---- GEGENPROBE: derselbe Versuch mit dem RE1.5-Hund (byte-true portiert) ---------- */
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    PL  = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    DOG = &g_actors[1];
    memset(PL, 0, sizeof *PL);
    memset(DOG, 0, sizeof *DOG);
    PL->active = 1; PL->hp = 100; PL->x = 0; PL->z = 0; PL->rot_y = 0;
    DOG->active = 1; DOG->type = 0x20; DOG->state = 0; DOG->hp = 0;
    DOG->x = 1200; DOG->z = 0;
    for (int i = 0; i < 3; i++) re15_enemy_ai_run_all(1);
    DOG->hp = 5000;
    re15_enemy_apply_hitbox(DOG, 0x20);
    printf("\n[RE1.5-Hund] Start: state=%d sub=%d/%d/%d motion=%d radius=%d\n",
           DOG->state, DOG->sub_state_1, DOG->sub_state_2, DOG->sub_state_3,
           DOG->motion, DOG->hit_radius_min);
    last_hp = DOG->hp; int n15 = 0; int hf15[64];
    for (int f = 0; f < 300; f++) {
        DOG->x = 1200; DOG->z = 0; DOG->y = 0;
        PL->x = 0; PL->z = 0; PL->y = 0; PL->rot_y = 0;
        DOG->aim_band = 2;   /* LEVEL — der ACTIVE-Tail-Stempel (@0x8010dd00-44) kann in
                              * dieser kuenstlichen Stellung nicht laufen; das Hoehenband
                              * ist NICHT Gegenstand der Messung. */
        re15_player_weapon_fire(2);
        if (DOG->hp != last_hp) {
            if (n15 < 64) hf15[n15] = f;
            n15++;
            printf("  f=%3d TREFFER  hp %d->%d  state=%d sub=%d/%d/%d motion=%d\n",
                   f, last_hp, DOG->hp, DOG->state, DOG->sub_state_1, DOG->sub_state_2,
                   DOG->sub_state_3, DOG->motion);
            last_hp = DOG->hp;
        }
        re15_enemy_ai_run_all(1);
    }
    printf("[RE1.5-Hund] %d Treffer in 300 Frames", n15);
    if (n15 >= 2) printf(", Abstand T1->T2 = %d Frames (%.2f s)", hf15[1]-hf15[0], (hf15[1]-hf15[0])/30.0);
    printf("\n\n");

    /* ---- PASS 3: RE2-Hund im IDLE (sub 0) getroffen — HURT-Zeilenwahl ----------------- */
    /* Original: die HURT-Zeile @0x80105538 wird mit +0x5 = WAFFEN-ID indiziert
     * (`lbu v0,5(a0)` @0x801032C4; +0x5 kommt aus `sb s5,5(s1)` @0x80047324, nachdem
     *  `sw 2,4(s1)` @0x80047288 +0x5..7 genullt hat). Der Port indiziert stattdessen mit
     * re2z_prev_sub (enemy_ai_re2_dog.c:1888) -> ein Hund, der im IDLE (sub 0) getroffen
     * wird, nimmt Zeile [0] = 0x8010321C (Landungs-Gate) statt der generischen Zeile. */
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    PL  = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    DOG = &g_actors[1];
    memset(PL, 0, sizeof *PL);
    memset(DOG, 0, sizeof *DOG);
    PL->active = 1; PL->hp = 100; PL->x = 0; PL->z = 0; PL->rot_y = 0;
    DOG->active = 1; DOG->type = 0x20; DOG->state = 0; DOG->hp = 0;
    DOG->x = 1200; DOG->z = 0;
    for (int i = 0; i < 2; i++) { DOG->x = 1200; DOG->z = 0; re15_re2dog_tick(1); }
    DOG->hp = 5000;
    re15_enemy_apply_hitbox(DOG, 0x20);
    DOG->state = 1; DOG->sub_state_1 = 0; DOG->sub_state_2 = 1; DOG->sub_state_3 = 0;  /* IDLE */
    DOG->x = 1200; DOG->z = 0; DOG->y = 0;
    re15_re2dog_tick(1);            /* setzt re2z_prev_sub = 0 (Schnappschuss bei state==1) */
    DOG->x = 1200; DOG->z = 0; DOG->y = 0;
    printf("\n[RE2-Hund IDLE] vor dem Schuss: state=%d sub=%d/%d/%d motion=%d frame=%u\n",
           DOG->state, DOG->sub_state_1, DOG->sub_state_2, DOG->sub_state_3,
           DOG->motion, (unsigned)DOG->anim_frame);
    re15_player_weapon_fire(2);
    printf("[RE2-Hund IDLE] nach dem Schuss: state=%d sub=%d/%d/%d hp=%d\n",
           DOG->state, DOG->sub_state_1, DOG->sub_state_2, DOG->sub_state_3, DOG->hp);
    int flinch = -1;
    for (int f = 0; f < 120; f++) {
        DOG->x = 1200; DOG->z = 0; DOG->y = 0;
        PL->x = 0; PL->z = 0; PL->y = 0; PL->rot_y = 0;
        re15_re2dog_tick(1);
        if (DOG->motion == 17 && flinch < 0) { flinch = f + 1; break; }
    }
    if (flinch >= 0)
        printf("[RE2-Hund IDLE] Zuck-Clip 17 erst nach %d Frames (%.2f s)\n",
               flinch, flinch / 30.0);
    else
        printf("[RE2-Hund IDLE] Zuck-Clip 17 in 120 Frames GAR NICHT erreicht\n");
    printf("\n");

    printf("  ORIGINAL RE2: +0x1D3 = 15 Frames (0.50 s) — Zeile 0x800A4424+(w-1)*0x14, +4>>9&0x7f\n");
    printf("  ORIGINAL RE1.5: Kette = 1 Clip (Boden 18 / Luft 15 Frames) -> ~0.6 s\n");
    printf(fails ? "PROBE-FAILS %d\n" : "PROBE-OK\n", fails);
    return 0;   /* Messsonde: nie rot */
}
