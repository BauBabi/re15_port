/* probe_dog_devour.c — MESSUNG + REGRESSION (ctest): der DOG-FINISHER (Cerberus 0x20 frisst Leon).
 *
 * SOLL (STAGE1.BIN roh disassembliert, 2026-08-08):
 *  Kette: Dog-Grab ACTIVE sub 9/10 (FUN_8010f80c/FUN_8010fc60) schreibt cmd 5 (@0x8010f910,
 *  dir @0x8010f928) -> Hook A LAB_80111944 (Tabelle 0x800ac758+0x20*4, FUN_8011e864) = Maschine A
 *  @0x80111984 (Phasen @0x801002ac): Intro-Clip 3*aca59 (@0x80111a0c), Hold-Clip 1 (@0x80111abc).
 *  Feed-Countdown +0x9e=50 (@0x8010fa18) ablaufen / hp<0 (@0x8010fa84) -> Dog sub 0xb =
 *  FUN_801100b4 (DEVOUR-START):
 *    - Dog-Clip 0x1b (@0x801100f4), +0x8f=7 (@0x80110124), spielt einmal, dann Freeze (+0x6>=2
 *      -> return @0x801100cc).
 *    - aca58:=6 (@0x80110138), aca59:=0 (@0x80110140), aca5a:=0 (@0x80110148); acbfc:=dog
 *      (@0x80110150); acbcc:=dog+0x178 (@0x80110160), acbd0:=dog+0x17c (@0x80110170) = EM020
 *      BANK 2 (victim). KEIN Blut, KEIN SE, KEIN HP-Write am Einstieg (voll-disasm
 *      0x801100b4-0x801101e0).
 *  -> Hook B LAB_80111cb0 (0x800ac858+0x20*4) -> 0x801210f0[aca59]={0x80111cf0,0x80111cf0} ->
 *  Maschine B @0x80111cf0 (Phasen @0x801002d4 = {d28,d6c,e74,eac,efc}):
 *    [0] @0x80111d28: aca5a:=1 (@0x80111d38); LEON-Clip +0x94:=4 (@0x80111d44) aus der DOG-BANK2
 *        (acbcc/acbd0); +0x95:=0 (@0x80111d4c); +0x96:=0 (@0x80111d54); +0x8f:=0 (@0x80111d5c,
 *        harter Schnitt); aca3c|=0xc0 (@0x80111d68); faellt in [1].
 *    [1] @0x80111d6c: ad68(player, acbcc, acbd0) Root-Motion (@0x80111d84);
 *        Frame 0x29: Blut FUN_80019700(0x2000, yaw+0x6a, BONE 8 = acbdc+0x5A0, 0-Block
 *          0x801210e0) (@0x80111e30);
 *        Frame 0x3a: dito Blut (@0x80111ddc) + Se_on(0x4030001)=CORE Rec 3 (@0x80111df4);
 *        Frame 0x4f: FUN_80045630(2,0) = Body-Fall-SE auf dem Boden-Material (@0x80111da4);
 *        f314(acbcc,acbd0,0,0x200) (@0x80111e4c); aca5a += done (@0x80111e68).
 *    [2] @0x80111e74: WUNDEN FUN_80037edc(0,0xa) (@0x80111e78), (5,0x32) (@0x80111e84),
 *        (7,0x32) (@0x80111e90); dann `sw 7 -> aca58` (@0x80111ea0) = cmd 7 LEICHE.
 *        KEIN HP-Write in der ganzen Maschine (voll-disasm 0x80111cb0-0x80111f08).
 *  ABGRENZUNG Release (Maschine A [4..8]): Blut 0x1500 + Wunde (0,0xa) (@0x80111b58/@0x80111b64),
 *  Release-Clip 2 (@0x80111b1c), danach Leons EIGENER Clip 0xB via anim_set(DAT_800acad8=player+0x84,
 *  DAT_800acbc0=player+0x16c, 1, 0x200) (@0x80111bf4/@0x80111c10-28), Exit cmd 1 (@0x80111c60/68).
 *  Clip 0xB gehoert also zur RELEASE-Kette und stammt NICHT aus der Dog-Bank2 — der Kollaps ist Clip 4.
 *
 * FEHLERBILD vorher (IST 2026-08-08): der Port spielte als Dog-Kollaps Bank2-Clip 0x0B (falsche
 * Zuordnung des Hook-A-Recover-Clips), spawnte Zombie-Einstiegsblut 0x1500 + CORE-SE 1 (nur
 * FUN_8010a6f8, Zombie) und nutzte die Zombie-Frames 0x23 (hp=-1) / 0x37 (Blut) statt der
 * Dog-Kadenz 0x29/0x3a/0x4f + Wundstempel + cmd 7 am Clip-Ende.
 *
 * Messung bank-echt: EM020 aus CDEMD0.EMS (Haupt- + VICTIM-Bank). */
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
    eb->victim_ok = (re15_emd_parse_victim_bank(s_em20, len, &eb->skel_victim, &eb->anim_victim) == 0);
    printf("EM020 geladen: bones=%d clips=%d | VICTIM ok=%d clips=%d bones=%d fc[0]=%d fc[1]=%d "
           "fc[2]=%d fc[4]=%d fc[0xb]=%d\n",
           eb->skel.bone_count, eb->anim.clip_count,
           eb->victim_ok, eb->anim_victim.clip_count, eb->skel_victim.bone_count,
           eb->anim_victim.clip_count > 0 ? eb->anim_victim.clips[0].frame_count : -1,
           eb->anim_victim.clip_count > 1 ? eb->anim_victim.clips[1].frame_count : -1,
           eb->anim_victim.clip_count > 2 ? eb->anim_victim.clips[2].frame_count : -1,
           eb->anim_victim.clip_count > 4 ? eb->anim_victim.clips[4].frame_count : -1,
           eb->anim_victim.clip_count > 0xb ? eb->anim_victim.clips[0xb].frame_count : -1);
    return eb;
}

static int fx_scan_last(int *scale16, int32_t *fy)
{
    const re15_esp_fx_t *fx = NULL;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *g = re15_esp_fx_get(i);
        if (g) fx = g;
    }
    if (!fx) return 0;
    if (scale16) *scale16 = fx->scale16;
    if (fy) *fy = fx->y;
    return 1;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    re15_actor_init();
    re15_esp_fx_reset();
    re15_wound_reset();
    re15_enemy_bank_t *eb = load_em020(base);
    if (!eb || !eb->victim_ok) { printf("WARN: EM020-VICTIM-Bank fehlt — Messung uebersprungen\n"); return 0; }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 700; pl->hp = 100;
    pl->rot_y = 0;

    const int DS = 1;
    re15_actor_t *d = &g_actors[DS];
    memset(d, 0, sizeof *d);
    d->active = 1; d->type = 0x20; d->state = 0; d->x = 0; d->y = 0; d->z = 0;
    d->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(d, 0x20);
    d->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - d->z, pl->x - d->x) - 0x400) & 0xfff);

    printf("=== DOG-FINISHER (probe_dog_devour) ===\n");
    re15_enemy_ai_run_all(1);                  /* INIT -> ACTIVE */
    CHECK(d->state == 1, "(setup) INIT->ACTIVE, state=%d", d->state);
    d->hp = 100;

    /* GRAB erzwingen: ACTIVE sub 9 (Front-Grab FUN_8010f80c). Kein Mash -> Feed-Countdown
     * +0x9e=50 (@0x8010fa18) laeuft ab -> sub 0xb DEVOUR (@0x8010fa70). */
    d->state = 1; d->sub_state_1 = 9; d->sub_state_2 = 0; d->sub_state_3 = 0;

    int   t_grab = -1, t_devour = -1, t_state7 = -1;
    int   intro_motion = -1, hold_motion = -1, collapse_motion = -1;
    int   dog_devour_clip = -1;
    int   fx_at_entry = 0;                     /* FX-Spawns im Devour-Einstiegs-Tick */
    int   n_fx_collapse = 0;                   /* FX-Spawns waehrend des Kollapses */
    int   fx_frames[8]; int fx_scales[8];      /* Leon-Frame + scale16 je Spawn */
    int32_t fx_y[8];
    int   hp_at_devour = 0, hp_after = 0;
    int   frame_at_state7 = -1;
    uint8_t wounds0[8][2], wounds1[8][2];
    re15_wound_save(wounds0);

    int fx_prev = re15_esp_fx_count();
    int32_t plx_dev = 0, plz_dev = 0;
    for (int t = 0; t < 400; t++) {
        re15_enemy_ai_run_all(1);
        re15_player_victim_tick();
        int vs = re15_player_victim_state();
        if (vs == 1 && t_grab < 0) t_grab = t;
        if (vs == 1) {
            if (intro_motion < 0) intro_motion = pl->motion;
            if (pl->motion != intro_motion) hold_motion = pl->motion;
        }
        if (d->sub_state_1 == 0x0b && t_devour < 0) {
            t_devour = t; hp_at_devour = pl->hp;
            plx_dev = pl->x; plz_dev = pl->z;
        }
        if (vs == 2 && collapse_motion < 0) collapse_motion = pl->motion;
        int fx_now = re15_esp_fx_count();
        if (fx_now != fx_prev) {
            int nnew = fx_now - fx_prev; if (nnew < 0) nnew = 1;
            if (t == t_devour) fx_at_entry += nnew;
            if (vs == 2 || t == t_devour) {
                for (int k = 0; k < nnew && n_fx_collapse < 8; k++) {
                    fx_frames[n_fx_collapse] = pl->anim_frame;
                    fx_scan_last(&fx_scales[n_fx_collapse], &fx_y[n_fx_collapse]);
                    n_fx_collapse++;
                }
            }
            fx_prev = fx_now;
        }
        if (pl->state == 7 && t_state7 < 0) { t_state7 = t; frame_at_state7 = pl->anim_frame; hp_after = pl->hp; }
        if (t_state7 >= 0 && t > t_state7 + 12) break;
        if (d->sub_state_1 == 0x0b) dog_devour_clip = d->motion;   /* letzter Wert = Fress-Clip
                                                                    * (Clip-Set laeuft 1 Tick nach
                                                                    * dem sub-Wechsel, FUN_801100b4
                                                                    * +0x6==0-Ast @0x801100f4) */
    }
    re15_wound_save(wounds1);

    printf("  Grab: t=%d Leon intro-motion=%d hold-motion=%d (Soll: 0 dann 1, Maschine A "
           "@0x80111a0c/@0x80111abc)\n", t_grab, intro_motion, hold_motion);
    printf("  Devour: t=%d dog sub=0x0b clip=%d (Soll 0x1b @0x801100f4) hp_at=%d\n",
           t_devour, dog_devour_clip, hp_at_devour);
    printf("  Kollaps: Leon motion=%d (SOLL 4 @0x80111d44, Dog-BANK2) frame@state7=%d "
           "(SOLL fc[4]-1=%d, cmd7 @0x80111ea0 nach Clip-Ende)\n",
           collapse_motion, frame_at_state7,
           eb->anim_victim.clip_count > 4 ? eb->anim_victim.clips[4].frame_count - 1 : -1);
    printf("  Blut: entry=%d (SOLL 0, kein 19700 in FUN_801100b4) | Kollaps-Spawns=%d:",
           fx_at_entry, n_fx_collapse);
    for (int i = 0; i < n_fx_collapse; i++)
        printf(" (frame=0x%02x scale=0x%x y=%d)", fx_frames[i], fx_scales[i], (int)fx_y[i]);
    printf("\n  (SOLL: genau 2, Frames 0x29 @0x80111e30 + 0x3a @0x80111ddc, scale 0x2000, Bone 8)\n");
    printf("  Wunden-AKKU vor/nach: [0]=%d/%d [5]=%d/%d [7]=%d/%d "
           "(SOLL nach: +0xa/+0x32/+0x32 @0x80111e78/84/90; save[i][1]=akku)\n",
           wounds0[0][1], wounds1[0][1], wounds0[5][1], wounds1[5][1], wounds0[7][1], wounds1[7][1]);
    printf("  Tod: t_state7=%d hp_after=%d | Leon pos devour (%d,%d) -> end (%d,%d) drag=(%d,%d)\n",
           t_state7, hp_after, (int)plx_dev, (int)plz_dev, (int)pl->x, (int)pl->z,
           (int)(pl->x - plx_dev), (int)(pl->z - plz_dev));

    /* ======================= REGRESSIONS-PINS (byte-true SOLL) ======================= */
    CHECK(t_grab >= 0,   "Grab muss latchen (victim state 1)");
    CHECK(t_devour >= 0, "Feed-Countdown 50 (@0x8010fa18) muss in sub 0xb DEVOUR muenden (@0x8010fa70)");
    CHECK(intro_motion == 0, "Maschine A Intro-Clip = 3*aca59 = 0 (front, @0x80111a0c), ist %d", intro_motion);
    CHECK(hold_motion == 1,  "Maschine A Hold-Clip = 1 (@0x80111abc), ist %d", hold_motion);
    CHECK(dog_devour_clip == 0x1b, "Dog-Fress-Clip 0x1b (@0x801100f4), ist %d", dog_devour_clip);
    CHECK(collapse_motion == 4,
          "LEON-KOLLAPS muss Dog-BANK2-Clip 4 sein (+0x94:=4 @0x80111d44) — ist %d "
          "(0x0b = falsch: das ist der Hook-A-Recover aus Leons EIGENER Bank @0x80111bf4)",
          collapse_motion);
    CHECK(fx_at_entry == 0,
          "KEIN Blut am Devour-Einstieg (FUN_801100b4 voll-disasm ohne 19700) — ist %d", fx_at_entry);
    CHECK(n_fx_collapse == 2, "genau 2 Kollaps-Blutspawns (0x29/0x3a), ist %d", n_fx_collapse);
    if (n_fx_collapse == 2) {
        CHECK(fx_frames[0] == 0x29, "Blut 1 bei Leon-Frame 0x29 (@0x80111e30), ist 0x%02x", fx_frames[0]);
        CHECK(fx_frames[1] == 0x3a, "Blut 2 bei Leon-Frame 0x3a (@0x80111ddc), ist 0x%02x", fx_frames[1]);
        CHECK(fx_scales[0] == 0x2000 && fx_scales[1] == 0x2000,
              "Blut-scale 0x2000 (a0 @0x80111dbc), ist 0x%x/0x%x", fx_scales[0], fx_scales[1]);
        CHECK(fx_y[0] != pl->y || fx_y[1] != pl->y,
              "Blut-Anker BONE 8 (acbdc+0x5A0 @0x80111dd8, Port re15_player_victim_bone_pos(8)) — "
              "darf nicht der Root sein (y=%d/%d, root=%d)",
              (int)fx_y[0], (int)fx_y[1], (int)pl->y);
    }
    {
        int fc4 = eb->anim_victim.clip_count > 4 ? eb->anim_victim.clips[4].frame_count : -1;
        CHECK(t_state7 >= 0, "cmd 7 (state 7) muss kommen (@0x80111ea0)");
        CHECK(frame_at_state7 == fc4 - 1,
              "cmd 7 exakt am CLIP-ENDE von Clip 4 (Phase [1] aca5a+=done @0x80111e68 -> Phase [2] "
              "@0x80111e74) — frame=%d, fc-1=%d", frame_at_state7, fc4 - 1);
    }
    CHECK((int)wounds1[0][1] - (int)wounds0[0][1] == 0x0a,
          "Wunde Panel 0 += 0x0a (@0x80111e78), delta=%d", (int)wounds1[0][1] - (int)wounds0[0][1]);
    CHECK((int)wounds1[5][1] - (int)wounds0[5][1] == 0x32,
          "Wunde Panel 5 += 0x32 (@0x80111e84), delta=%d", (int)wounds1[5][1] - (int)wounds0[5][1]);
    CHECK((int)wounds1[7][1] - (int)wounds0[7][1] == 0x32,
          "Wunde Panel 7 += 0x32 (@0x80111e90), delta=%d", (int)wounds1[7][1] - (int)wounds0[7][1]);
    CHECK(hp_at_devour >= 0,
          "Feed-Timeout-Kill kommt mit hp>=0 an (kein HP-Write in FUN_801100b4/Maschine B), ist %d",
          hp_at_devour);

    /* =================== ABGRENZUNG: RELEASE (Break-free, Maschine A [4..8]) ===================
     * Mash (Press-Edge 0xf0f0, FUN_80037024) drainiert +0x9c um 1+100*mash (@0x8010fa9c) ->
     * step 4 (@0x8010faec) = aca5a:=4-Push. SOLL dort: EINMAL Blut 0x1500 (Spieler-Bone 8,
     * @0x80111b58) + EINMAL Wunde (0,0xa) (@0x80111b64) — NICHT das Kill-Triple 0/5/7. Leon:
     * Release-Clip 2 (@0x80111b1c), danach frei (cmd 1 @0x80111c60/68), KEIN cmd 7, hp>=0. */
    printf("=== RELEASE-Fall (Mash-Break-free) ===\n");
    re15_actor_init();
    re15_esp_fx_reset();
    re15_player_victim_reset();
    re15_wound_reset();
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 700; pl->hp = 100; pl->rot_y = 0;
    d = &g_actors[DS];
    memset(d, 0, sizeof *d);
    d->active = 1; d->type = 0x20; d->state = 0; d->x = 0; d->y = 0; d->z = 0;
    d->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(d, 0x20);
    d->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - d->z, pl->x - d->x) - 0x400) & 0xfff);
    re15_enemy_ai_run_all(1);
    d->hp = 100;
    d->state = 1; d->sub_state_1 = 9; d->sub_state_2 = 0; d->sub_state_3 = 0;

    uint8_t wr0[8][2], wr1[8][2];
    re15_wound_save(wr0);
    int t_free = -1, saw_release_clip = 0, saw_dog_recover = 0, freed = 0;
    int n_fx_rel = 0; int rel_scales[4];
    fx_prev = re15_esp_fx_count();
    for (int t = 0; t < 400; t++) {
        re15_enemy_ai_set_pad_pressed(0x0010);          /* Mash: jeder Tick ein Press-Edge (0xf0f0-Maske) */
        re15_enemy_ai_run_all(1);
        re15_player_victim_tick();
        if (d->sub_state_1 == 9 && d->sub_state_2 == 4 && t_free < 0) t_free = t;
        if (d->sub_state_1 == 9 && d->sub_state_2 >= 4 && t_free < 0) t_free = t;
        if (d->sub_state_1 == 0x0c) { saw_dog_recover = 1; d->x = 20000; }   /* raus aus der Re-Grab-Reichweite */
        if (re15_player_victim_state() == 3 && pl->motion == 2) saw_release_clip = 1;
        int fx_now = re15_esp_fx_count();
        if (fx_now != fx_prev) {
            int nnew = fx_now - fx_prev; if (nnew < 0) nnew = 1;
            for (int k = 0; k < nnew && n_fx_rel < 4; k++)
                fx_scan_last(&rel_scales[n_fx_rel], NULL), n_fx_rel++;
            fx_prev = fx_now;
        }
        if (re15_player_victim_state() == 0 && t_free >= 0) { freed = 1; break; }
    }
    re15_enemy_ai_set_pad_pressed(0);
    re15_wound_save(wr1);
    printf("  Break-free: t=%d dog-recover(0x0c)=%d Leon-Release-Clip2=%d freed=%d hp=%d state=%d\n",
           t_free, saw_dog_recover, saw_release_clip, freed, pl->hp, pl->state);
    printf("  Release-Blut: n=%d scales:", n_fx_rel);
    for (int i = 0; i < n_fx_rel; i++) printf(" 0x%x", rel_scales[i]);
    printf(" (SOLL: 1x 0x1500 @0x80111b58)\n");
    printf("  Wunden-AKKU-Delta: [0]=%d [5]=%d [7]=%d (SOLL 0x0a/0/0 — nur Panel 0 @0x80111b64)\n",
           (int)wr1[0][1] - (int)wr0[0][1], (int)wr1[5][1] - (int)wr0[5][1],
           (int)wr1[7][1] - (int)wr0[7][1]);
    CHECK(t_free >= 0, "(REL) Mash muss den Break-free ausloesen (@0x8010faec)");
    CHECK(saw_release_clip, "(REL) Leon muss Release-Clip 2 spielen (@0x80111b1c)");
    CHECK(freed, "(REL) Leon muss frei werden (cmd 1 @0x80111c60/68), victim=%d", re15_player_victim_state());
    CHECK(pl->state != 7 && pl->hp >= 0, "(REL) KEIN Tod im Release (state=%d hp=%d)", pl->state, pl->hp);
    CHECK(n_fx_rel == 1 && rel_scales[0] == 0x1500,
          "(REL) genau 1 Blut-Spawn 0x1500 (@0x80111b58), ist n=%d scale=0x%x",
          n_fx_rel, n_fx_rel ? rel_scales[0] : 0);
    CHECK((int)wr1[0][1] - (int)wr0[0][1] == 0x0a, "(REL) Wunde Panel 0 += 0x0a (@0x80111b64), delta=%d",
          (int)wr1[0][1] - (int)wr0[0][1]);
    CHECK((int)wr1[5][1] - (int)wr0[5][1] == 0 && (int)wr1[7][1] - (int)wr0[7][1] == 0,
          "(REL) KEIN Kill-Triple im Release (Panels 5/7 unveraendert), delta=%d/%d",
          (int)wr1[5][1] - (int)wr0[5][1], (int)wr1[7][1] - (int)wr0[7][1]);

    if (fails == 0) printf("OK: alle Dog-Finisher-Checks gruen\n");
    else            printf("FAILS: %d\n", fails);
    return fails ? 1 : 0;
}
