/* probe_dog_hit.c — MESSUNG + REGRESSION (ctest): die Dog-Treffer-Trias (Nutzer-Report 2026-08-08):
 *  (1) BLUT beim Treffer:  Original HURT-Entry spawnt func_0x80019700(0x2000, rot_y, pool+0xEC, &DAT_80120f54)
 *      = Effekt 0, scale 0x2000, Anker BONE 1 (0xEC = 0xAC*1+0x40), Offset (0,0,0)
 *      (grounded @0x80110a84-aa4; airborne @0x80110c6c-8c Bone 0; bark-death @0x8010ebe0-e4 Bone 4;
 *       airborne-death @0x80110f54-70 Bone 0; DAT_80120f54 = 16x 00 gedumpt).
 *  (2) RUECKSTOSS: HURT-Exit -> state 1 sub 7 (@0x80110b48-58); ACT[7]=FUN_8010efbc: Clip 0x13
 *      (@0x8010f040-44), SPEED +0x8c=0x14 (@0x8010f070-74), pro Frame 245d8(0x800) = RUECKWAERTS
 *      (@0x8010f120-124) + Homing a8f8(&player,(rng&0xf)+1) (@0x8010f080-ac) -> Clip-Ende sub 3
 *      (@0x8010f028/f130). Clip 0x13 = 29 Frames (EM020-EDD) -> >= 29*20 = 580 Units rueckwaerts
 *      (zzgl. FUN_80111870-Root-Motion @0x8010f118, port-offen).
 *  (3) TODES-POSE: DEATH grounded (FUN_80111120): Detour ueber HURT (case 0 @0x80111160-9c),
 *      Collapse-Clip 0x0E (@0x80111206 "+0x94=0xe"), f314 bis Ende, dann +0x4=7 CORPSE
 *      (@0x80111284). f314 wrappt +0x95 am Clip-Ende auf 0 (FUN_8001f3bc @Z.89-94 / FUN_8001f8b4
 *      @Z.76-81 Decompile), aber der Part-Pool haelt die ZULETZT GESTELLTE Pose = Keyframe
 *      fc-1 — und der CORPSE-Root FUN_80111774 hat KEINEN f314-Call (voll-disasm 0x80111774-186c)
 *      => die Leiche zeigt den LETZTEN Keyframe des Collapse-Clips (0x0E: Frame 43), nie Frame 0.
 * Messung bank-echt: EM020 aus CDEMD0.EMS (Blob-Index re15_ems_index_for_type(0x20)). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_math.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

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
static re15_enemy_bank_t *load_em020(const char *base)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { printf("WARN: CDEMD0.EMS nicht lesbar (%s) — bank-los\n", emsp); return NULL; }
    int idx = re15_ems_index_for_type(0x20);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, ems_size, idx, &off, &len) != 0) {
        printf("WARN: EM020-Blob nicht gefunden — bank-los\n"); free(ems); return NULL;
    }
    s_em20 = (uint8_t *)malloc(len);
    memcpy(s_em20, ems + off, len);
    free(ems);
    re15_enemy_bank_t *eb = re15_enemy_find(0x20);
    if (!eb) eb = re15_enemy_alloc(0x20);
    if (!eb) return NULL;
    if (re15_emd_parse_container(s_em20, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) {
        printf("WARN: EM020 parse fehlgeschlagen — bank-los\n"); return NULL;
    }
    eb->ok = 1; eb->buf = NULL;
    printf("EM020 geladen: bones=%d clips=%d fc[6]=%d fc[0x13]=%d fc[0x0e]=%d fc[7]=%d\n",
           eb->skel.bone_count, eb->anim.clip_count,
           eb->anim.clips[6].frame_count, eb->anim.clips[0x13].frame_count,
           eb->anim.clips[0x0e].frame_count, eb->anim.clips[7].frame_count);
    return eb;
}

static void face(re15_actor_t *e, const re15_actor_t *t)
{
    e->rot_y = (int16_t)(((int)re15_atan2_q12(t->z - e->z, t->x - e->x) - 0x400) & 0xfff);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    re15_actor_init();
    re15_esp_fx_reset();
    re15_enemy_bank_t *eb = load_em020(base);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 2000; pl->hp = 100;

    const int DS = 1;
    re15_actor_t *d = &g_actors[DS];
    memset(d, 0, sizeof *d);
    d->active = 1; d->type = 0x20; d->state = 0; d->x = 0; d->y = 0; d->z = 0;
    d->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(d, 0x20);
    face(d, pl);            /* Hund schaut zum Spieler (rot_y -> +z) */
    face(pl, d);            /* Spieler schaut zum Hund (Gun-Wedge) */

    printf("=== DOG Treffer-Trias (probe_dog_hit) ===\n");
    re15_enemy_ai_run_all(1);                 /* INIT -> ACTIVE */
    CHECK(d->state == 1, "(setup) INIT->ACTIVE, state=%d", d->state);
    d->hp = 100;                              /* deterministisch fuer die Messung */

    /* ---------- (1)+(2) Handgun-Treffer (weapon 3, Row 3 = grounded @0x80121018[3]) ---------- */
    int fx0 = re15_esp_fx_count();
    int hit = re15_player_weapon_fire(3);
    CHECK(hit == DS + 1, "(1) weapon 3 muss den Hund treffen (ret=%d)", hit);
    CHECK(d->state == 2, "(1) Treffer -> state 2 (HURT), ist %d", d->state);
    CHECK(d->sub_state_1 == 3, "(1) +0x5 = weapon_id 3, ist %d", d->sub_state_1);
    printf("  Treffer: state=%d sub=%d hp=%d fx_before=%d\n", d->state, d->sub_state_1, d->hp, fx0);

    /* Tick 1 = HURT-Entry: genau EIN Blut-Spawn (@0x80110a84-aa4), Anker BONE 1 */
    int32_t bone1[3] = { d->x, d->y, d->z };
    re15_enemy_bone_world_pos(d, 1, bone1);   /* Soll-Anker (pool+0xEC @0x80110aa4) */
    re15_enemy_ai_run_all(1);
    int fx1 = re15_esp_fx_count();
    CHECK(fx1 - fx0 == 1, "(1) HURT-Entry muss GENAU 1 Blut-FX spawnen (@0x80110a84-aa4), delta=%d", fx1 - fx0);
    CHECK(d->motion == 6, "(1) Flinch-Clip 6 (@0x80110a3c), motion=%d", d->motion);
    /* der neue Slot: Position + scale pruefen */
    {
        const re15_esp_fx_t *fx = NULL;
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
            const re15_esp_fx_t *g = re15_esp_fx_get(i);
            if (g) fx = g;                    /* letzter aktiver = der neue (Pool war leer) */
        }
        CHECK(fx != NULL, "(1) FX-Slot muss existieren");
        if (fx) {
            printf("  Blut-FX: pos=(%d,%d,%d) scale16=0x%x | Soll-Anker Bone1=(%d,%d,%d) Dog-Root=(%d,%d,%d)\n",
                   (int)fx->x, (int)fx->y, (int)fx->z, fx->scale16,
                   (int)bone1[0], (int)bone1[1], (int)bone1[2], (int)d->x, (int)d->y, (int)d->z);
            CHECK(fx->scale16 == 0x2000, "(1) scale16 muss 0x2000 sein (a0 @0x80110a84), ist 0x%x", fx->scale16);
            /* BONE-Anker-Gate: der Spawn posiert Bone 1 zur Spawn-Zeit (Flinch-Clip 6 Frame 0,
             * Crossfade laeuft) — der exakte Vektor haengt am Blend-Zustand, aber Bone 1 sitzt
             * IMMER deutlich ueber dem Root (Y-hoch = negativ). Der alte Port spawnte exakt am
             * Root (y == dog.y, Boden) -> das Gate trennt beide eindeutig. */
            CHECK(fx->y < d->y - 100,
                  "(1) Anker muss BONE 1 sein (pool+0xEC @0x80110aa4) — fx.y=%d liegt nicht ueber dem Root y=%d",
                  (int)fx->y, (int)d->y);
        }
    }

    /* (2) Flinch (Clip 6, KEINE Bewegung @FUN_801109e0) -> Recover sub 7 -> Hop rueckwaerts */
    int32_t z_hit = d->z;
    int reached7 = 0, hop_clip_ok = 0, hop_speed_ok = 0, back_at_exit = 0;
    int32_t z_hop_start = 0;
    for (int f = 0; f < 90; f++) {
        re15_enemy_ai_run_all(1);
        if (d->state == 1 && d->sub_state_1 == 7) {
            if (!reached7) { reached7 = 1; z_hop_start = d->z; }
            if (d->motion == 0x13) hop_clip_ok = 1;
            if (d->crow_speed == 0x14) hop_speed_ok = 1;
        }
        if (reached7 && d->state == 1 && d->sub_state_1 == 3) { back_at_exit = 1; break; }
    }
    CHECK(reached7,    "(2) HURT-Recover muss zu sub 7 gehen (@0x80110b58)");
    CHECK(hop_clip_ok, "(2) Hop-Clip 0x13 (@0x8010f040-44), motion=%d", d->motion);
    CHECK(hop_speed_ok,"(2) Hop-Speed +0x8c=0x14 (@0x8010f070-74)");
    CHECK(back_at_exit,"(2) Hop-Ende muss zu sub 3 exiten (@0x8010f130)");
    {
        /* Rueckwaerts = weg vom Spieler (Spieler bei z=+2000, Hund faced +z, Hop = -z).
         * Soll: 29 Frames (Clip 0x13) x 20 = 580 Units (245d8(0x800) @0x8010f120-124).
         * Homing a8f8 dreht minimal — Toleranz auf die z-Achse: >= 500. */
        int32_t back = z_hop_start - d->z;
        printf("  Rueckstoss: z_hit=%d z_hopstart=%d z_end=%d -> rueckwaerts=%d (Soll >= 500, 29f x 20)\n",
               (int)z_hit, (int)z_hop_start, (int)d->z, (int)back);
        CHECK(back >= 500, "(2) Hop muss >= 500 Units rueckwaerts tragen (29x20=580 @0x8010f070/f120), ist %d", (int)back);
    }

    /* ---------- (3) Todes-Sequenz + Corpse-Endpose ---------- */
    d->hp = 3; d->hit_react = 0;              /* naechster Treffer toetet (dmg 10 > 3) */
    /* zurueck in Reichweite/Kegel stellen */
    d->x = 0; d->z = 0; face(d, pl); face(pl, d);
    hit = re15_player_weapon_fire(3);
    CHECK(hit == DS + 1, "(3) Kill-Schuss muss treffen (ret=%d)", hit);
    CHECK(d->state == 3, "(3) hp<0 -> state 3 (DEATH), ist state=%d hp=%d", d->state, d->hp);

    int saw_detour6 = 0, saw_collapse = 0, corpse_frame = -1, corpse_motion = -1;
    for (int f = 0; f < 160 && corpse_frame < 0; f++) {
        re15_enemy_ai_run_all(1);
        if (d->state == 2 && d->motion == 6)    saw_detour6 = 1;   /* Detour @0x80111160-9c */
        if (d->state == 3 && d->motion == 0x0e) saw_collapse = 1;  /* Collapse @0x801111f0-121c */
        if (d->state == 7) { corpse_motion = d->motion; corpse_frame = d->anim_frame; }
    }
    CHECK(saw_detour6,  "(3) DEATH muss ueber den HURT-Flinch (Clip 6) detouren (@0x80111160)");
    CHECK(saw_collapse, "(3) Collapse-Clip 0x0E muss laufen (@0x801111f0-121c)");
    CHECK(corpse_frame >= 0, "(3) Hund muss CORPSE (state 7) erreichen");
    printf("  Corpse: motion=%d anim_frame=%d (Soll: motion=0x0e, frame=LETZTER Keyframe 43 — nicht 0)\n",
           corpse_motion, corpse_frame);
    CHECK(corpse_motion == 0x0e, "(3) Corpse muss den Collapse-Clip 0x0E halten, ist %d", corpse_motion);
    /* Original: f314 posiert zuletzt Keyframe fc-1 (FUN_8001f3bc +0x95-Wrap), Corpse-Root
     * FUN_80111774 ruft KEIN f314 -> Pose = letzter Keyframe. EM020-Clip 0x0E hat 44 Frames. */
    {
        int fc = (eb && eb->ok) ? eb->anim.clips[0x0e].frame_count : 44;
        CHECK(corpse_frame == fc - 1,
              "(3) Corpse-Endpose muss der LETZTE Keyframe sein (fc-1=%d; f314-Wrap FUN_8001f3bc, "
              "kein f314 im Corpse-Root 0x80111774) — ist frame=%d", fc - 1, corpse_frame);
        /* 10 weitere Ticks: die Pose darf sich nicht mehr bewegen */
        int mo = d->motion, fr = d->anim_frame;
        for (int f = 0; f < 10; f++) re15_enemy_ai_run_all(1);
        CHECK(d->motion == mo && d->anim_frame == fr,
              "(3) Corpse-Pose muss stehen bleiben (kein f314 @0x80111774), ist mo=%d fr=%d", d->motion, d->anim_frame);
    }

    /* ---------- (3b) AIRBORNE-Death-Lane (Magnum row 7 @0x80121070[7] = FUN_80110eb0) ---------- */
    {
        re15_actor_t *d2 = &g_actors[2];
        memset(d2, 0, sizeof *d2);
        d2->active = 1; d2->type = 0x20; d2->state = 0; d2->x = 400; d2->y = 0; d2->z = 0;
        d2->em_flag_id = 0xFF;
        re15_enemy_apply_hitbox(d2, 0x20);
        d->active = 0;                                   /* der tote Hund 1 raus aus dem Auto-Aim */
        re15_enemy_ai_run_all(1);                        /* INIT */
        d2->hp = 50; d2->hit_react = 0;
        face(pl, d2);
        int fxb = re15_esp_fx_count();
        int h2 = re15_player_weapon_fire(7);             /* Magnum: dmg 200 -> hp<0, row 7 = AIRBORNE */
        CHECK(h2 == 3, "(3b) Magnum muss Hund 2 treffen (ret=%d)", h2);
        CHECK(d2->state == 3 && d2->sub_state_1 == 7, "(3b) row 7 -> AIRBORNE death, state=%d sub=%d",
              d2->state, d2->sub_state_1);
        int c2_frame = -1, c2_motion = -1, saw7 = 0;
        for (int f = 0; f < 80 && c2_frame < 0; f++) {
            re15_enemy_ai_run_all(1);
            if (d2->state == 3 && d2->motion == 7) saw7 = 1;   /* knock-fly clip 7 @0x80110ef4-f20 */
            if (d2->state == 7) { c2_motion = d2->motion; c2_frame = d2->anim_frame; }
        }
        CHECK(re15_esp_fx_count() - fxb >= 1, "(3b) airborne-death Entry muss Blut spawnen (@0x80110f80-fa0)");
        CHECK(saw7, "(3b) knock-fly Clip 7 muss laufen (@0x80110ef4-f20)");
        CHECK(c2_motion == 7, "(3b) Corpse haelt Clip 7, ist %d", c2_motion);
        {
            int fc7 = (eb && eb->ok) ? eb->anim.clips[7].frame_count : 15;
            CHECK(c2_frame == fc7 - 1,
                  "(3b) Corpse-Endpose = LETZTER Keyframe des Fly-Clips (fc-1=%d), ist %d", fc7 - 1, c2_frame);
        }
        printf("  Airborne-Death: corpse motion=%d frame=%d (Soll 7/%d)\n", c2_motion, c2_frame,
               ((eb && eb->ok) ? eb->anim.clips[7].frame_count : 15) - 1);
    }

    if (fails == 0) printf("OK: alle Dog-Treffer-Checks gruen\n");
    else            printf("FAILS: %d\n", fails);
    return fails ? 1 : 0;
}
