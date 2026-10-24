/* test_r41_re2gator.c — Runde 41 Spur B "re2gator": Riegel fuer die RE2-Bausteine des ROOM2090-Bosses.
 *
 * Treibt den ECHTEN Boss-Tick (re15_gator_boss_tick + Dispatcher-Klemme wie probe_gator_fress.c, kein
 * Nachbau) mit der RE2-EM23-Bank. Belege je Zahl: analysis/befunde_runde41/B_re2gator.md §2 und die
 * @0x-Kommentare in engine/src/gator_re2_r41.c.
 *
 *   clips     Clip 1 (Paddeln) bei dist >= 0x65 ohne Biss-Sperre, Clip 0 in der Sperre, authored Vortrieb
 *   dash      Zonenwechsel -> SE 4, Clip 2 (150) -> Clip 3 (150), Stroke-Ereignis -> Schuetteln 5 x Bit 4 + 10 x Bit 8
 *   intro     Aggro -> Clip 6 (125) -> Clip 3 (150) -> Biss-Sperre 110 -> Clip 0, dann Clip 1
 *   sockel    HP 25300, 200 Pistolentreffer toeten nicht, geschwaecht < 25000, SE 0 xor SE 5, R4-Sperre 120..183
 *   kanister  Fenster -> Stufen 1..7 (Kopf/Kiefer-az), Anhaengen an Part 6, Treffer -> R1 (SE 4, Raum-Satz 15,
 *             Rumble, Clip 9, Kanister nach 9 Bildern weg, Leiche)
 *   putz      Dash-Start -> 54 Putz-Partikel (RE2 CORE00.ESP id 4) ohne unbekannte re2fx-Ops, Lage um Leon
 *   wand      Nachbesserung 1 (M1): Intro ohne Bahn-Gate + Dash auf der Westwand-Klemmlinie nach Sueden (Klemme
 *             nimmt den RE2-Korridor-Seitversatz), yaw += 1024 nach Clip 3 (@0x80100938-48 / @0x80100eb0-c0)
 *
 * Seit Nachbesserung 1 zuendet der RE2-Intro bei JEDEM Aggro (RE2 Sub 0; vorher nur bei freier Bahn, was am
 * Lauerplatz nie galt): Riegel, die das Verhalten DANACH messen, warten ihn mit intro_abwarten() ab.
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
#include "re15_boss_gator.h"
#include "re15_gator_re2_r41.h"
#include "re15_emd.h"
#include "re15_skeleton.h"      /* re15_sin_q12 / re15_cos_q12 (Soll-Schritt im Riegel wand) */
#include "re15_camera.h"        /* Nachtrag 4: re15_camera_build_view (Nachrechnung der Kamerawahl) */
#include "re15_anim_select.h"   /* Nachtrag 4: re15_compute_actor_kf */
#include <math.h>
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_rumble.h"
#include "re2_fx.h"
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

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz;
    return b;
}
static int load_re2_bank(uint8_t type)
{
    static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
    if (!s_ems) { size_t n = 0; s_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &n); s_ems_sz = (long)n; }
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

/* ---- SE-Rekorder (ENEMSE Bank 17 ueber den Boss-Haken, Raum-Satz ueber den R41-Haken) ---- */
static int s_se_log[512], s_se_n;
static int s_raum_se_log[16], s_raum_se_n;
static int s_se_tick_mark;   /* Zahl der SEs beim letzten Marker */
static void se_rec(int id, int flag2000) { (void)flag2000; if (s_se_n < 512) s_se_log[s_se_n++] = id; }
static void bank_rec(int bank) { (void)bank; }
static void raum_rec(int satz) { if (s_raum_se_n < 16) s_raum_se_log[s_raum_se_n++] = satz; }
static int se_count_since(int id, int from) { int n = 0; for (int i = from; i < s_se_n; i++) if (s_se_log[i] == id) n++; return n; }

static int slot;
static re15_actor_t *e, *pl;

static void tick(void)
{
    int32_t ox = e->x, oz = e->z;
    re15_gator_boss_tick(slot);
    if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
        int32_t nx = e->x, nz = e->z;
        re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz, e->hit_radius_min, e->y, 4u);
        e->x = nx; e->z = nz;
    }
}
static void leon_statue(int32_t x, int32_t z)
{
    pl->x = x; pl->z = z; pl->y = 0; pl->floor = 0;
    pl->state = 0; pl->motion = 0; pl->hit_react = 0;
}
/* Intro (Clip 6 + Clip 3, 275 Bilder) zu Ende ticken und die Intro-Biss-Sperre (110) loeschen - fuer die
 * Riegel, die die Verfolgung/Treffer DANACH messen (Leon bleibt, wo der Aufrufer ihn hingestellt hat). */
static void intro_abwarten(void)
{
    int32_t lx = pl->x, lz = pl->z;
    for (int f = 0; f < 400 && (re15_gr2_im_intro(slot) || f < 2); f++) { leon_statue(lx, lz); tick(); }
    re15_gr2_biss_cd(slot, 0);
}

static int setup(int gb_test)
{
#ifdef _WIN32
    _putenv(gb_test ? "RE15_GB_TEST=1" : "RE15_GB_TEST=");
    _putenv("RE15_GB_STUMM=1");
    _putenv("RE15_GATOR_KANISTER=");
#else
    if (gb_test) setenv("RE15_GB_TEST", "1", 1); else unsetenv("RE15_GB_TEST");
    setenv("RE15_GB_STUMM", "1", 1);
    unsetenv("RE15_GATOR_KANISTER");
#endif
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf) { printf("FAIL: ROOM2090.RDT nicht lesbar\n"); return 0; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 0; }
    g_room_rdt_ok = 1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;
    if (!load_re2_bank(0x23u)) { printf("FAIL: RE2-EM23-Bank nicht ladbar (CDEMD0.EMS)\n"); return 0; }
    re15_enemy_bank_t *eb = re15_enemy_find(0x23u);
    if (!eb || eb->skel.bone_count != 22) { printf("FAIL: erwartet 22 Bones\n"); return 0; }
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    slot = RE15_ACTOR_MAX - 1;
    e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x23u;
    e->x = -6000; e->y = 0; e->z = -22000;
    e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x23u);
    pl->active = 1; pl->type = 0; pl->hp = 100;
    s_se_n = 0; s_raum_se_n = 0;
    re15_gator_audio_hook(se_rec, bank_rec);
    re15_gr2_raum_se_hook = raum_rec;
    re15_rumble_reset();
    return 1;
}

/* Authored Wurzelversatz-Differenz (FUN_80015e7c) eines Bildes, skaliert. */
static int32_t authored_vortrieb(int clip, int frame, int32_t s)
{
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    const re15_emd_clip_t *c = &bk->anim.clips[clip];
    int16_t a[3] = {0,0,0}, b[3] = {0,0,0};
    int kfb = (int)(bk->anim.frames[c->first_frame + frame] & 0xfffu);
    re15_emd_get_keyframe_speed(&bk->skel, kfb, &b[0], &b[1], &b[2]);
    if (frame > 0) {
        int kfa = (int)(bk->anim.frames[c->first_frame + frame - 1] & 0xfffu);
        re15_emd_get_keyframe_speed(&bk->skel, kfa, &a[0], &a[1], &a[2]);
    }
    return (int32_t)(((int64_t)(b[0] - a[0]) * s) >> 12);
}

/* ================================================================================================ */
static int teil_clips(void)
{
    if (!setup(1)) return 1;
    /* Gator UND Leon in Zone 0 (z <= -18100): kein Zonenwechsel, also kein Dash - nur die Hysterese.
     * Abstand 6800 > Biss-Commit (Maulpunkt = Zentrum + 2600, Gate 2400) und ausserhalb des Wand-Slabs. */
    leon_statue(-6000, -18200);
    tick();                                    /* INIT (Lauerplatz) + LURK/aggro -> INTRO -> CHASE */
    intro_abwarten();
    e->x = -6000; e->z = -25000; e->y = -1200; e->rot_y = 0xC00;   /* nach Norden */
    CHECK(e->hp == RE15_GR2_HP_INIT, "HP nach INIT %d != %d (RE2 @0x80100518-580)", (int)e->hp, RE15_GR2_HP_INIT);
    int clip1 = 0, wrap = 0, vortrieb_ok = 0, vortrieb_n = 0;
    uint32_t af_alt = 0;
    for (int f = 0; f < 60; f++) {
        leon_statue(-6000, -18200); e->x = -6000; e->z = -25000; e->rot_y = 0xC00;
        tick();
        if (e->motion == 1) {
            clip1++;
            if (af_alt == 25 && e->anim_frame == 0) wrap++;     /* 26 Bilder, Selbst-Reset @0x80100d08-10 */
            af_alt = e->anim_frame;
        }
    }
    CHECK(clip1 >= 50, "Paddeln (Clip 1) nur in %d/60 Bildern - RE2 Sub 2 bei +0x1F0 >= 0x65 (@0x80100b88)", clip1);
    CHECK(wrap >= 1, "Clip 1 wrappt nicht nach 26 Bildern (wrap=%d)", wrap);
    /* Vortrieb je Bild = off[f]-off[f-1] x Scale: Gator wieder weit weg stellen, Positionen vergleichen */
    for (int f = 0; f < 30; f++) {
        leon_statue(-6000, -18200);
        e->x = -6000; e->z = -25000; e->rot_y = 0xC00;   /* Statue, nur der Vortrieb dieses Bildes zaehlt */
        int32_t z0 = e->z;
        tick();
        if (e->motion == 1) {
            int32_t soll = authored_vortrieb(1, (int)e->anim_frame, e->render_scale_q12 ? e->render_scale_q12 : 4096);
            int32_t ist = e->z - z0;              /* nach Norden = +z */
            vortrieb_n++;
            if (ist >= soll - 2 && ist <= soll + 2) vortrieb_ok++;
            else printf("  vortrieb f=%u ist=%d soll=%d\n", (unsigned)e->anim_frame, ist, soll);
        }
    }
    CHECK(vortrieb_n > 0 && vortrieb_ok >= vortrieb_n - 2,
          "Vortrieb nicht der authored Wurzelversatz (FUN_80015e7c): %d/%d Bilder ok", vortrieb_ok, vortrieb_n);
    /* Biss-Sperre +0x238 -> Schwimmen Clip 0 (@0x80100bf4-c04), danach wieder Paddeln */
    re15_gr2_biss_cd(slot, RE15_GR2_BISS_CD_BISS);
    int clip0 = 0;
    for (int f = 0; f < RE15_GR2_BISS_CD_BISS; f++) {
        leon_statue(-6000, -18200); e->x = -6000; e->z = -25000; e->rot_y = 0xC00;
        tick();
        if (e->motion == 0) clip0++;
    }
    CHECK(clip0 >= RE15_GR2_BISS_CD_BISS - 1, "in der Biss-Sperre nur %d/%d Bilder Clip 0", clip0, RE15_GR2_BISS_CD_BISS);
    for (int f = 0; f < 3; f++) { leon_statue(-6000, -18200); e->x = -6000; e->z = -25000; e->rot_y = 0xC00; tick(); }
    CHECK(e->motion == 1, "nach der Sperre nicht wieder Clip 1 (motion=%d)", (int)e->motion);
    printf("clips: Clip1 %d/60, wrap %d, Vortrieb %d/%d, Sperre Clip0 %d/%d\n", clip1, wrap, vortrieb_ok, vortrieb_n, clip0, RE15_GR2_BISS_CD_BISS);
    return fails ? 1 : 0;
}

/* ================================================================================================ */
static int dash_fahren(int max, int *f_start, int *n2, int *n3, int modus_log[])
{
    *f_start = -1; *n2 = *n3 = 0;
    int seen = 0;
    for (int f = 0; f < max; f++) {
        leon_statue(-6500, -8000);
        int se_vor = s_se_n;
        tick();
        if (modus_log) modus_log[f] = re15_gr2_schuettel_modus();
        if (re15_gr2_im_dash(slot)) {
            if (*f_start < 0) { *f_start = f; CHECK(se_count_since(4, se_vor) >= 1, "kein SE 4 am Dash-Start (@0x80100d64/84)"); }
            if (e->motion == 2) (*n2)++; else if (e->motion == 3) (*n3)++;
            seen = 1;
        } else if (seen) return f;             /* Dash zu Ende */
    }
    return -1;
}
static int teil_dash(void)
{
    if (!setup(1)) return 1;
    leon_statue(-6500, -8000);
    tick();
    intro_abwarten();
    e->x = -6500; e->z = -22000; e->y = -1200; e->rot_y = 0xC00;
    static int modus_log[1200];
    int f_start, n2, n3;
    int f_ende = dash_fahren(1200, &f_start, &n2, &n3, modus_log);
    CHECK(f_start >= 0, "kein Dash beim Zonenwechsel in Leons Zone (RE2 Zonenlinie -> 0x301 @0x801009ac)");
    CHECK(n2 == 150 && n3 == 150, "Dash-Clips: Clip 2 %d / Clip 3 %d Bilder (RE2: 150/150, EDD)", n2, n3);
    CHECK(f_ende > 0, "Dash endet nicht");
    /* Schuetteln: Clip 2 f50 (Wort 0x280000e2). Der Frame-Wort-Block des Boss-Ticks liest das Bild des
     * VORIGEN Ticks (Port-Struktur, gilt auch fuer alle Frame-Flag-SEs): Wort f50 im Bild f_start+51 ->
     * Ereignis 1 + Rumble (10,120,50) + Zaehler 2 -> +2 Bilder Ereignis 3 -> Bit 4 (Modus 2) 5 Bilder
     * (+0x16A = 5 @0x80101dcc) -> Bit 8 (Modus 1) 10 Bilder (+0x16A = 10 @0x80101e24) -> frei. Relativ
     * zu t = f_start+50: Bit 4 in +4..+8, Bit 8 in +9..+18. */
    if (f_start >= 0) {
        int t = f_start + 50;
        int m2 = 0, m1 = 0, erstes2 = -1, erstes1 = -1, letztes1 = -1;
        for (int f = t; f < t + 40 && f < 1200; f++) {
            if (modus_log[f] == 2) { m2++; if (erstes2 < 0) erstes2 = f; }
            if (modus_log[f] == 1) { m1++; if (erstes1 < 0) erstes1 = f; letztes1 = f; }
        }
        CHECK(m2 == 5, "Modus 2 (Bit 4, Tabelle @0x8009db14) in %d Bildern statt 5 (+0x16A = 5 @0x80101dcc)", m2);
        CHECK(m1 == 10, "Modus 1 (Bit 8, Tabelle @0x8009daf4) in %d Bildern statt 10 (+0x16A = 10 @0x80101e24)", m1);
        CHECK(erstes2 == t + 4, "Bit 4 ab Bild +%d statt +4 (Wort im Folgebild, Zaehler 2, dann Ereignis 3)", erstes2 - t);
        CHECK(erstes1 == t + 9 && letztes1 == t + 18, "Bit 8 Bilder +%d..+%d statt +9..+18", erstes1 - t, letztes1 - t);
        CHECK(re15_rumble_belegt(1) > 0 || re15_rumble_belegt(0) > 0, "kein Rumble-Eintrag (FUN_800395b8(10,120,50))");
        /* Versatzwerte liegen in der Tabelle (0..3) */
        int dx, dy; re15_gr2_schuettel_offset(&dx, &dy);
        CHECK(dx >= 0 && dx <= 3 && dy >= 0 && dy <= 3, "Versatz (%d,%d) ausserhalb der Tabellenwerte", dx, dy);
    }
    printf("dash: start %d, Clip2 %d, Clip3 %d, ende %d\n", f_start, n2, n3, f_ende);
    return fails ? 1 : 0;
}

/* ================================================================================================ */
static int teil_intro(void)
{
    if (!setup(0)) return 1;                    /* kein GB_TEST: Aggro ueber die Distanz (GB_AGGRO 9000) */
    leon_statue(-6000, 2000);                   /* weit weg, noch kein Aggro */
    tick();                                     /* INIT -> LURK */
    e->x = -6000; e->z = -24000; e->y = -1200; e->rot_y = 0xC00;   /* Bahn nach Norden frei (Westkanal) */
    leon_statue(-6000, -16000);                 /* dist 8000 < 9000 -> Aggro */
    int n6 = 0, n3 = 0, f_cd = -1, n0 = 0, n1 = 0;
    for (int f = 0; f < 600; f++) {
        if (f_cd >= 0) {
            /* nach dem Intro: Gator-Statue in Zone 2 (kein Zonenwechsel/Dash), Leon fern (kein Biss) */
            e->x = -6000; e->z = -16300; e->rot_y = 0xC00; leon_statue(-6000, -8000);
        } else leon_statue(-6000, -16000);
        tick();
        if (e->motion == 6) n6++;
        else if (e->motion == 3 && n6 > 0 && f_cd < 0) n3++;
        else if (n3 > 0) {
            /* +0x238 = 110 wird im Tick des LETZTEN Clip-3-Bildes gesetzt (@0x8010091c-20, FUN_8001a330 == 1,
             * Nachbesserung 1); der Umschalt-Tick (Clip 0) hat den Takt schon einmal abgezogen -> 109 hier. */
            if (f_cd < 0) { f_cd = f; CHECK(re15_gr2_biss_cd_rest(slot) == RE15_GR2_BISS_CD_INTRO - 1, "Biss-Sperre im ersten Clip-0-Tick %d != 109 (110 @0x8010091c im Ende-Tick, ein Takt weniger)", re15_gr2_biss_cd_rest(slot)); }
            if (f < f_cd + RE15_GR2_BISS_CD_INTRO) { if (e->motion == 0) n0++; }
            else if (f < f_cd + RE15_GR2_BISS_CD_INTRO + 10) { if (e->motion == 1) n1++; }
        }
    }
    CHECK(n6 == 125, "Clip 6 (Durchbruch) %d Bilder statt 125 (EDD)", n6);
    CHECK(n3 == 150, "Clip 3 (Schnapper) %d Bilder statt 150 (EDD)", n3);
    CHECK(n0 >= RE15_GR2_BISS_CD_INTRO - 2, "in der 110er-Sperre nur %d Bilder Clip 0", n0);
    CHECK(n1 >= 8, "nach der Sperre nicht Clip 1 (%d/10)", n1);
    printf("intro: Clip6 %d, Clip3 %d, Sperre ab %d, Clip0 %d, danach Clip1 %d\n", n6, n3, f_cd, n0, n1);
    return fails ? 1 : 0;
}

/* ================================================================================================ */
static int teil_sockel(void)
{
    if (!setup(1)) return 1;
    leon_statue(-6000, -8000);
    tick();
    intro_abwarten();
    e->x = -6000; e->z = -22000; e->y = -1200; e->rot_y = 0xC00;
    int hp0 = e->hp, r4 = 0, r0 = 0, beide = 0, geschw_bei = -1, cd_min = 999, cd_max = 0, letzter_r4 = -1000;
    int treffer = 0;
    for (int i = 0; i < 200; i++) {
        leon_statue(-6000, -8000); e->x = -6000; e->z = -22000;
        int hp_vor = e->hp, se_vor = s_se_n;
        (void)re15_enemy_take_damage(e, 0);     /* attack_type 0 = Handfeuerwaffe */
        tick();
        treffer++;
        int se0 = se_count_since(0, se_vor), se5 = se_count_since(5, se_vor);
        if (se0 && se5) beide++;
        if (e->motion == 10 && hp_vor != e->hp) { r4++; if (letzter_r4 >= 0) { int d = i - letzter_r4; if (d < cd_min) cd_min = d; if (d > cd_max) cd_max = d; } letzter_r4 = i; }
        else if (se5) r0++;
        if (geschw_bei < 0 && re15_gr2_geschwaecht()) geschw_bei = i;
        for (int f = 0; f < 3; f++) { leon_statue(-6000, -8000); e->x = -6000; e->z = -22000; tick(); }
    }
    CHECK(hp0 == RE15_GR2_HP_INIT, "HP-Start %d", hp0);
    CHECK(e->hp > 0 && e->hp < hp0, "HP nach 200 Treffern %d (Sockel: lebt, aber getroffen)", (int)e->hp);
    CHECK(!re15_gr2_tot(), "Gator tot durch Beschuss - RE2 kennt keinen Attrition-Tod (Sockel 25000 @0x80100518)");
    CHECK(beide == 0, "SE 0 und SE 5 im selben Treffer (%d x) - RE2 R0/R4 schliessen sich aus", beide);
    CHECK(r4 >= 1, "keine Grossreaktion in 200 Treffern (rnd&3 == 0 @0x80101fb4)");
    CHECK(geschw_bei >= 0, "nie geschwaecht (HP < 25000 @0x80101ec4)");
    CHECK(re15_gr2_treffer() >= 150, "Trefferzaehler %d", re15_gr2_treffer());
    printf("sockel: hp %d -> %d, R4 %d (Abstand %d..%d Treffer), R0 %d, geschwaecht ab Treffer %d\n", hp0, (int)e->hp, r4, cd_min, cd_max, r0, geschw_bei);
    return fails ? 1 : 0;
}

/* ================================================================================================ */
static int teil_kanister(void)
{
    if (!setup(0)) return 1;
    leon_statue(-6000, 2000);                   /* kein Aggro: LURK, Kanister-Vorpass laeuft auch dort */
    tick();
    e->x = -6000; e->z = -22000; e->y = -1200; e->rot_y = 0xC00;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    int32_t laengs = (int32_t)(((int64_t)8800 * s) >> 12);          /* im Fenster [8601, 9000) x Scale */
    re15_gr2_kanister_setzen(-6000, -267, -22000 + laengs);
    CHECK(re15_gr2_kanister_stufe() == 0, "Kanister nicht gesetzt");
    int stufe_bilder[14]; memset(stufe_bilder, 0, sizeof stufe_bilder);
    int kopf2 = 0, kiefer2 = 0, f7 = -1;
    for (int f = 0; f < 120; f++) {
        leon_statue(-6000, 2000); e->x = -6000; e->z = -22000; e->rot_y = 0xC00;
        tick();
        int st = re15_gr2_kanister_stufe();
        if (st >= 0 && st < 14) stufe_bilder[st]++;
        if (st == 3 && kopf2 == 0) { kopf2 = re15_gr2_bone_az(slot, 6); kiefer2 = re15_gr2_bone_az(slot, 7); }
        if (st == 7 && f7 < 0) f7 = f;
    }
    CHECK(f7 >= 0, "Kanister nie getragen (Stufe 7)");
    CHECK(stufe_bilder[2] == 13, "Stufe 2 %d Bilder statt 13 (@0x801018c4)", stufe_bilder[2]);
    CHECK(stufe_bilder[3] == 6 && stufe_bilder[4] == 6, "Stufe 3/4 %d/%d Bilder statt 6/6", stufe_bilder[3], stufe_bilder[4]);
    CHECK(stufe_bilder[5] == 7 && stufe_bilder[6] == 7, "Stufe 5/6 %d/%d Bilder statt 7/7", stufe_bilder[5], stufe_bilder[6]);
    CHECK(kopf2 > 150 && kopf2 <= 240, "Kopf-az nach Stufe 2 = %d (ease-in 13 x 240 @0x80101860)", kopf2);
    CHECK(kiefer2 < -200 && kiefer2 >= -320, "Kiefer-az nach Stufe 2 = %d (ease-in 13 x -320 @0x80101870)", kiefer2);
    int kopf7 = re15_gr2_bone_az(slot, 6), kiefer7 = re15_gr2_bone_az(slot, 7);
    /* Summen der Ease-Deltas wie das Overlay sie rechnet (ease-in t=0..N-1: A*(N-1)^2/N^2, ease-out
     * t=0..N-1: A + A*(N^2-1)/N^2, weil ease-out bei t=0 die VOLLE Amplitude liefert, `addu v0,a2`
     * @0x80101d38): Kopf 204 - 97 - 276 + 55 + 148 = 34, Kiefer -272 - 41 - 118 + 73 + 198 = -160. */
    CHECK(kopf7 >= 30 && kopf7 <= 40, "Kopf-az beim Tragen %d (Ease-Summen = 34)", kopf7);
    CHECK(kiefer7 >= -170 && kiefer7 <= -150, "Kiefer-az beim Tragen %d (Ease-Summen = -160)", kiefer7);
    /* Welt-Lage folgt Part 6 */
    int32_t kp[3], kr[9], b6[3];
    CHECK(re15_gr2_kanister_welt(kp, kr) == 1, "Kanister unsichtbar beim Tragen");
    re15_enemy_bone_world_pos(e, 6, b6);
    {   int64_t d2 = (int64_t)(kp[0]-e->x)*(kp[0]-e->x) + (int64_t)(kp[2]-e->z)*(kp[2]-e->z);
        CHECK(d2 < (int64_t)9000*9000, "Kanister weit weg vom Gator (%d,%d) vs (%d,%d)", kp[0], kp[2], e->x, e->z); }
    /* Treffer beim Tragen -> R1 */
    int se_vor = s_se_n, raum_vor = s_raum_se_n;
    re15_rumble_reset();
    (void)re15_enemy_take_damage(e, 0);
    tick();
    CHECK(re15_gr2_r1_laeuft(slot), "Treffer beim Tragen loest R1 nicht aus (@0x80101ef4-fc)");
    CHECK(se_count_since(4, se_vor) >= 1, "R1 ohne SE 4 (@0x801021c4/ec)");
    CHECK(s_raum_se_n > raum_vor && s_raum_se_log[s_raum_se_n-1] == 15, "R1 ohne Raum-Satz 15 (Se_on 0x020f0001 @0x8010237c-88)");
    CHECK(re15_rumble_belegt(0) > 0 && re15_rumble_belegt(1) > 0, "R1 ohne Rumble (8,3)/(50,255->0) @0x80102328-44");
    CHECK(e->motion == 9, "R1 spielt nicht Clip 9 (motion=%d)", (int)e->motion);
    CHECK(re15_gr2_kanister_stufe() == 13, "Kanister nicht auf Stufe 13 (+0x226 = 13 @0x801022b8)");
    /* Fortsetzung 3: RE2-Bildtakt DAT_800dfc1a = 4 ab R1 P0 (@0x8010235c-64), 2 ab P3 (@0x80102668/7c) */
    CHECK(re15_gr2_vsync() == 4, "R1 P0 ohne VSync 4 (Zeitlupe, sb 0x800dfc1a @0x8010235c-64): %d", re15_gr2_vsync());
    int sichtbar = 0, hp_r1 = e->hp, zeitlupe = 1;
    for (int f = 0; f < 60; f++) {
        if (re15_gr2_kanister_welt(kp, kr)) sichtbar++;
        tick();
        if (re15_gr2_vsync() == 4) { CHECK(!re15_gr2_tot(), "VSync 4 noch im Endzustand P3 (@0x8010267c setzt 2)"); zeitlupe++; }
    }
    CHECK(re15_gr2_vsync() == 2, "nach P3 nicht zurueck auf VSync 2 (@0x80102668/7c): %d", re15_gr2_vsync());
    printf("kanister: Zeitlupe VSync 4 ueber %d Takte (R1 P0 .. vor P3)\n", zeitlupe);
    CHECK(sichtbar == 10, "Kanister nach R1 %d Bilder sichtbar statt 10 (P0 + 9 P1-Bilder, +0x16B = 9 @0x80102394, Abbau @0x8010255c-78)", sichtbar);
    CHECK(re15_gr2_tot(), "Leiche nicht erreicht (P3 @0x80102654)");
    CHECK(e->motion == 9, "Leiche haelt nicht die Clip-9-Pose (motion=%d)", (int)e->motion);
    (void)re15_enemy_take_damage(e, 0); tick();
    CHECK(e->hp == hp_r1, "Leiche nimmt Schaden (%d -> %d) - +0x1D3 |= 0x80 @0x801022ec", hp_r1, (int)e->hp);
    printf("kanister: Stufen %d/%d/%d/%d/%d/%d, kopf2 %d kiefer2 %d, getragen ab %d, kopf7 %d kiefer7 %d, R1 sichtbar %d\n",
           stufe_bilder[2], stufe_bilder[3], stufe_bilder[4], stufe_bilder[5], stufe_bilder[6], stufe_bilder[7],
           kopf2, kiefer2, f7, kopf7, kiefer7, sichtbar);
    return fails ? 1 : 0;
}

/* ================================================================================================ */
/* Geplatzter Kiefer: FUN_80104318 (@0x80104318-58) stempelt Part-Record 7 des RE2-Modellblocks
 * ([em+0x198] + 7 x 172, `addiu a1,s3,1204` @0x80102358): Flagwort |= 0x4A, +0x70 = 0x202038, +0x98 = yaw
 * + 0x800, +0x9A = -10, +0x9C/+0x9E/+0xA4 = 0, +0xA0 = 0x8000. Danach uebernimmt der Zeichner FUN_80027434:
 * eigene Matrix (0x40 @0x80027498), Drift FUN_80028DAC (@0x80027B98: y += -10 je Bild @0x80028E04-14,
 * Lebensdauer `andi 0x7fff / sltiu 0x1d` @0x80028DC8-CC, dann Flagwort 0 @0x80028DDC) und Scherben-Zeichnung
 * FUN_8002D3C8 (Skala = Tabelle[life>>15][life&0x7fff] @0x8009DC28/@0x8009DC64, Farbe flach +0x70). Der Riegel
 * treibt die Port-Bruecke so, wie main.c sie je Bild ruft (re15_re2z_gore_part_matrix, dann _part_burst). */
static int teil_kiefer(void)
{
    if (!setup(0)) return 1;
    leon_statue(-6000, 2000);
    tick();
    e->x = -6000; e->z = -22000; e->y = -1200; e->rot_y = 0xC00;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    re15_gr2_kanister_setzen(-6000, -267, -22000 + (int32_t)(((int64_t)8800 * s) >> 12));
    for (int f = 0; f < 60 && re15_gr2_kanister_stufe() != 7; f++) {
        leon_statue(-6000, 2000); e->x = -6000; e->z = -22000; e->rot_y = 0xC00; tick();
    }
    CHECK(re15_gr2_kanister_stufe() == 7, "Kanister nicht getragen (Stufe %d)", re15_gr2_kanister_stufe());
    CHECK(re15_re2z_gore_active(e) == 0, "Part-Bruecke VOR dem Explosionstod aktiv");
    int16_t yaw_vor = e->rot_y;
    (void)re15_enemy_take_damage(e, 0);
    tick();                                                     /* -> R1 P0: FUN_80104318 @0x80102354-58 */
    CHECK(re15_gr2_kiefer_geplatzt(slot), "Kiefer nicht gestempelt (FUN_80104318)");
    CHECK(re15_re2z_gore_active(e) == 1, "Part-Bruecke nach R1 nicht aktiv (Haken re15_re2z_gore_active, Typ 0x23)");
    CHECK(e->re2z_part_flags[7] == 0x4Bu, "Part 7 Flagwort 0x%x != 0x4B (Bit 0 + `ori 0x4a` @0x80104324)", e->re2z_part_flags[7]);
    CHECK(e->re2z_part_tint[7] == 0x00202038u, "Part 7 Tinte 0x%x != 0x202038 (@0x8010434c)", e->re2z_part_tint[7]);
    CHECK(e->re2z_part_life[7] == 0x8000u, "Part 7 +0xA0 = 0x%x != 0x8000 (@0x80104334)", e->re2z_part_life[7]);
    CHECK(e->re2z_part_w9a[7] == -10, "Part 7 +0x9A = %d != -10 (@0x80104340)", e->re2z_part_w9a[7]);
    CHECK(e->re2z_part_w9c[7] == 0 && e->re2z_part_w9e[7] == 0 && e->re2z_part_wa4[7] == 0, "Part 7 +0x9C/+0x9E/+0xA4 nicht 0");
    CHECK(((e->re2z_part_yaw98[7] - yaw_vor) & 0xfff) == 0x800, "Part 7 +0x98 = %d, erwartet yaw %d + 0x800 (@0x80104350)", e->re2z_part_yaw98[7], yaw_vor);
    for (int i = 0; i < 16; i++) if (i != 7)
        CHECK(e->re2z_part_flags[i] == 1u && e->re2z_part_tint[i] == 0x00808080u, "Record %d nicht neutral geseedet", i);
    CHECK(re15_gr2_bone_az(slot, 7) == 0, "Kiefer-az nach R1 %d != 0 (+0x228 = 0 @0x801022dc; -10 ist vy, kein az)", re15_gr2_bone_az(slot, 7));
    /* Elternkette: kein Kind haengt am Kiefer (Kaskade @0x80027480-94 traefe sonst mit) */
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    int kinder = 0;
    for (int i = 0; i < bk->skel.bone_count; i++) if (bk->skel.bone_parent[i] == 7) kinder++;
    CHECK(kinder == 0, "%d Bones haengen am Kiefer-Part 7", kinder);
    /* Zeichner-Bruecke je Bild: Matrix (seed + Physik) und dann Burst-Skala/Tinte wie main.c */
    int32_t rot[9] = { 4096,0,0, 0,4096,0, 0,0,4096 }, trans[3], y0 = 0;
    re15_enemy_bone_world_pos(e, 7, trans); y0 = trans[1];
    int burst_bilder = 0, m_ok = 0, erste_skala = -1, letzte_skala = -1; uint32_t tint = 0; int y_ok = 0, weg_ab = -1;
    for (int k = 1; k <= 34; k++) {
        int32_t r2[9]; memcpy(r2, rot, sizeof r2);
        int32_t t2[3]; memcpy(t2, trans, sizeof t2);
        int m = re15_re2z_gore_part_matrix(e, 7, (uint32_t)k, r2, t2);
        int32_t sk = 0; uint32_t tn = 0;
        int b = re15_re2z_gore_part_burst(e, 7, &sk, &tn);
        if (m) { m_ok++; if (t2[1] == y0 - 10 * k) y_ok++; }     /* vy -10 je Bild (@0x80028E04-14) */
        if (b) { burst_bilder++; if (erste_skala < 0) erste_skala = sk; letzte_skala = sk; tint = tn; }
        if (!m && weg_ab < 0) weg_ab = k;
        tick();
    }
    CHECK(burst_bilder == 30, "Scherben-Bilder %d != 30 (life 0x8000..0x801d, `sltiu 0x1d` @0x80028DCC)", burst_bilder);
    CHECK(m_ok == 30 && y_ok == 30, "eigene Matrix in %d Bildern, Aufstieg -10/Bild in %d (soll 30/30)", m_ok, y_ok);
    CHECK(erste_skala == 180 && letzte_skala == 2790, "Burst-Skala %d..%d, soll 180..2790 (Zeile 1 @0x8009DC64, Bit 15 von 0x8000)", erste_skala, letzte_skala);
    CHECK(tint == 0x00202038u, "Scherben-Tinte 0x%x != 0x202038", tint);
    CHECK(weg_ab == 31, "Kiefer weg ab Bild %d statt 31 (Flagwort 0 @0x80028DDC nach 29 Zaehlschritten)", weg_ab);
    CHECK(e->re2z_part_flags[7] == 0u, "Part 7 Flagwort %u != 0 nach Ablauf", e->re2z_part_flags[7]);
    {   uint8_t dr[RE15_EMD_MAX_BONES]; uint32_t tn[RE15_EMD_MAX_BONES]; uint8_t ms[RE15_EMD_MAX_BONES];
        for (int i = 0; i < RE15_EMD_MAX_BONES; i++) { dr[i] = 1; tn[i] = 0x00808080u; ms[i] = (uint8_t)i; }
        int on = re15_re2z_gore_resolve(e, bk->skel.bone_parent, bk->skel.bone_count, dr, tn, ms);
        int gezeichnet = 0; for (int i = 0; i < bk->skel.bone_count; i++) gezeichnet += dr[i];
        CHECK(on == 1 && dr[7] == 0 && gezeichnet == bk->skel.bone_count - 1,
              "Resolver: on=%d Kiefer gezeichnet=%d, uebrige %d/%d (Bit 0 @0x8002737C)", on, dr[7], gezeichnet, bk->skel.bone_count); }
    printf("kiefer: Flag 0x%x Tinte 0x%x life 0x%x vy %d Kurs %d; Scherben %d Bilder Skala %d..%d, weg ab %d, Kinder %d\n",
           0x4B, 0x202038, 0x8000, -10, (int)e->re2z_part_yaw98[7], burst_bilder, erste_skala, letzte_skala, weg_ab, kinder);
    return fails ? 1 : 0;
}

/* ================================================================================================ */
static int teil_putz(void)
{
    if (!setup(1)) return 1;
    size_t n = 0;
    uint8_t *esp = slurp(RE15_ASSET_RE2_DIR "/CORE00.ESP", &n);
    if (!esp) { printf("FAIL: shared_assets/RE2/CORE00.ESP fehlt\n"); return 1; }
    re2fx_reset();
    CHECK(re2fx_register_core(esp, n) == 0, "re2fx_register_core schlug fehl");
    leon_statue(-6500, -8000);
    tick();
    intro_abwarten();
    e->x = -6500; e->z = -22000; e->y = -1200; e->rot_y = 0xC00;
    int f_start, n2, n3;
    int spawns0 = re15_gr2_putz_spawns();
    /* bis zum Dash-Start fahren, dann 120 Bilder re2fx mitticken */
    int gestartet = 0;
    for (int f = 0; f < 1200 && !gestartet; f++) {
        leon_statue(-6500, -8000); tick(); re2fx_tick();
        if (re15_gr2_im_dash(slot)) gestartet = 1;
    }
    (void)f_start; (void)n2; (void)n3;
    CHECK(gestartet, "kein Dash");
    for (int f = 0; f < 120; f++) { leon_statue(-6500, -8000); tick(); re2fx_tick(); }
    int spawns = re15_gr2_putz_spawns() - spawns0;
    CHECK(spawns == 54, "Putz-Spawns %d statt 54 (6 x 9, ROOM40A0 sub13/sub14)", spawns);
    CHECK(re2fx_op_unbekannt() == 0, "re2fx meldet %u unbekannte Ops fuer Effekt 4", re2fx_op_unbekannt());
    {   /* Nachbesserung 1 (M2): Lage um LEON (putz_x/y/z war vorher ungesetzt = (0,0,0) ausserhalb des Beckens).
         * Letzter Spawn jeder Runde = k_putz[8] @0x0900: Versatz (-622, -2400, -50) zu Leons Stand (-6500, 0, -8000). */
        int32_t lp[3]; re15_gr2_putz_letzte(lp);
        CHECK(lp[0] == -6500 - 622 && lp[1] == -2400 && lp[2] == -8000 - 50,
              "letzter Putz-Spawn bei (%d,%d,%d) statt (-7122,-2400,-8050) um Leon", lp[0], lp[1], lp[2]);
    }
    printf("putz: spawns %d, unbekannte Ops %u; Op-Zaehler:", spawns, re2fx_op_unbekannt());
    for (int op = 0; op < 96; op++) if (re2fx_op_zaehler(op)) printf(" %d:%u", op, re2fx_op_zaehler(op));
    printf("\n");
    return fails ? 1 : 0;
}

/* ================================================================================================ */
/* Kanister-Modell: ROOM40A0 Modell 1 = MD1 @RDT 0x4024 .. 0x5388 (4964 B; das len-Feld 3444 endet GENAU
 * am Quad-UV-Block @0xC+0xD68, der bis zum naechsten Modell reicht) + TIM @0x2fcfc (8 Bit, 2 CLUTs). */
static int teil_modell(void)
{
    size_t n = 0, tn = 0;
    uint8_t *md1 = slurp(RE15_ASSET_RE2_DIR "/KANISTER40A0.MD1", &n);
    uint8_t *tim = slurp(RE15_ASSET_RE2_DIR "/KANISTER40A0.TIM", &tn);
    CHECK(md1 && n == 4964, "KANISTER40A0.MD1 fehlt oder %u B statt 4964", (unsigned)n);
    CHECK(tim && tn == 33824, "KANISTER40A0.TIM fehlt oder %u B statt 33824", (unsigned)tn);
    if (!md1 || !tim) return 1;
    re15_md1_t m; memset(&m, 0, sizeof m);
    int rc = re15_md1_parse(md1, (int)n, &m);
    CHECK(rc == 0, "re15_md1_parse = %d", rc);
    CHECK(m.mesh_count == 1, "mesh_count %d != 1", m.mesh_count);
    if (m.mesh_count >= 1) {
        CHECK(m.meshes[0].quad_count == 95 && m.meshes[0].triangle_count == 0,
              "Quads %d / Dreiecke %d (RDT: 95 / 0)", m.meshes[0].quad_count, m.meshes[0].triangle_count);
        CHECK(m.meshes[0].quad_vertex_count == 104, "Vertices %d != 104", m.meshes[0].quad_vertex_count);
        if (m.meshes[0].quad_count >= 1)
            CHECK(m.meshes[0].quad_uvs[0].clut == 0x7840, "CLUT des ersten Quads 0x%x != 0x7840 (CLUT-Zeile 481 = Warnstreifen)", m.meshes[0].quad_uvs[0].clut);
    }
    re15_tim_t t; memset(&t, 0, sizeof t);
    CHECK(re15_tim_parse(tim, (int)tn, &t) == 0, "re15_tim_parse fehlgeschlagen");
    CHECK(t.bpp == 8 && t.width == 128 && t.height == 256 && t.has_clut && t.clut_entries == 512,
          "TIM %d bpp %dx%d clut %d/%d (soll 8 bpp 128x256, 2 CLUTs = 512)", t.bpp, t.width, t.height, t.has_clut, t.clut_entries);
    printf("modell: MD1 %u B, Quads %d, Vertices %d; TIM %d bpp %dx%d, CLUTs %d\n", (unsigned)n,
           m.mesh_count ? m.meshes[0].quad_count : -1, m.mesh_count ? m.meshes[0].quad_vertex_count : -1,
           t.bpp, t.width, t.height, t.clut_entries / 256);
    return fails ? 1 : 0;
}

/* ================================================================================================ */
/* Nachbesserung 1 (Abnahme 0, M1): Dash und Intro muessen im NATUERLICHEN Ablauf zuenden.
 * (a) Intro vom Lauerplatz (-7200,-9200), Blick zur Leiter (INIT-Yaw ~1087), Leon in der Tuernische
 *     (-8200,-25550): RE2 Sub 0 ohne Bahn-Gate - Clip 6 (125) -> Clip 3 (150) -> yaw += 1024 (@0x80100938-48),
 *     Biss-Sperre 110 (@0x8010091c-20); der Durchbruch (Clip 6 netto 6281 laengs) kommt nach Sueden voran, die
 *     Dispatcher-Klemme (tick()) haelt ihn an der Westwand (x >= -8900 + 1466).
 * (b) Dash im Westkanal nach SUEDEN auf der Klemmlinie x = -7434 (Abnahme-Lauf A: Route bei x -7289..-7300),
 *     Leon Zone 0: Zonenwechsel 2 -> 0 bei z = -18100 -> SE 4 + Clip 2/3 je 150 (@0x80100dac-e28); die Klemme
 *     nimmt den RE2-Korridor-Seitversatz von Clip 3 (-2775 quer = West), laengs ~4188 (2307 + 1881) voran, am
 *     Ende yaw += 1024 (@0x80100eb0-c0). Vorher (gb_bahn_frei, Ende-x -7309 < -7300) zuendete hier nichts. */
static int teil_wand(void)
{
    if (!setup(1)) return 1;
    leon_statue(-8200, -25550);
    tick();                                   /* INIT: Lauerplatz, Blick zur Leiter, aggro (GB_TEST=1) -> der
                                               * INIT-Tick laeuft schon in LURK -> Intro-Start (Clip 6, Bild 0) */
    CHECK(e->x == -7200 && e->z == -9200, "INIT-Lage (%d,%d) statt Lauerplatz (-7200,-9200)", e->x, e->z);
    int yaw0 = (int)e->rot_y & 0xfff;
    int n6 = (e->motion == 6) ? 1 : 0, n3 = 0, f;
    int32_t x_min = e->x, z_min = e->z;
    for (f = 0; f < 400; f++) {
        leon_statue(-8200, -25550); tick();
        if (!re15_gr2_im_intro(slot) && f > 2) break;
        if (e->motion == 6) n6++; else if (e->motion == 3) n3++;
        if (e->x < x_min) x_min = e->x;
        if (e->z < z_min) z_min = e->z;
    }
    int yaw1 = (int)e->rot_y & 0xfff;
    CHECK(n6 == 125 && n3 == 150, "Intro Clip 6 %d / Clip 3 %d Bilder (RE2: 125/150, EDD)", n6, n3);
    CHECK(x_min >= -8900 + 1400, "Intro: Gator in der Westwand (x_min %d, Klemmlinie -7434)", x_min);
    CHECK(z_min <= -9200 - 5000, "Intro: Durchbruch kam nicht voran (z_min %d; Clip 6 netto 6281 laengs)", z_min);
    CHECK(((yaw1 - yaw0) & 0xfff) == 1024, "Intro-Ende: yaw %d -> %d, erwartet +1024 (@0x80100938-48)", yaw0, yaw1);
    CHECK(re15_gr2_biss_cd_rest(slot) == RE15_GR2_BISS_CD_INTRO - 1,
          "Biss-Sperre im ersten CHASE-Tick %d != 109 (110 im Ende-Tick @0x8010091c-20, ein Takt abgezogen)", re15_gr2_biss_cd_rest(slot));
    /* (b) Dash: Klemmlinie im Westkanal, Blick Sued, Leon Zone 0 */
    e->x = -7434; e->z = -17900; e->y = -1200; e->rot_y = 1024;
    int f_start = -1, n2 = 0, yaw_d = 0; n3 = 0;
    int32_t z_start = 0, x_dmin = e->x, x_dmax = e->x;
    int32_t dz_soll_sum = 0, dz_ist_sum = 0; int abw_n = 0;
    int se_vor = s_se_n;
    for (f = 0; f < 600; f++) {
        /* Soll-Schritt dieses Bildes (FUN_80015e7c-Delta, um den Yaw gedreht wie gr2_rig_welt), nur z */
        int clip_b = (int)e->motion, f_b = (int)e->anim_frame, yaw_b = (int)e->rot_y & 0xfff;
        int32_t z_b = e->z;
        int im_dash_b = re15_gr2_im_dash(slot);
        leon_statue(-8200, -25550); tick();
        if (im_dash_b && (clip_b == 2 || clip_b == 3)) {
            /* Soll = Schritt des NEUEN Bildes f_b+1 (FUN_8001a330: Vorschub, dann e7c+152c8); das letzte Bild
             * (f = n-1) wird ohne Schritt gezeigt (`bne v0,zero,0x8001a3cc` @0x8001a3b0), der Umschalt-Tick
             * (Bild 0 des Folgeclips / Clip 0) ebenfalls ohne Schritt. */
            re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
            int16_t a[3] = {0,0,0}, b[3] = {0,0,0};
            const re15_emd_clip_t *c = &bk->anim.clips[clip_b];
            int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
            int f_n = f_b + 1;
            if (f_n < c->frame_count - 1 && f_b < c->frame_count - 1) {
                re15_emd_get_keyframe_speed(&bk->skel, (int)(bk->anim.frames[c->first_frame + f_n] & 0xfffu), &b[0], &b[1], &b[2]);
                re15_emd_get_keyframe_speed(&bk->skel, (int)(bk->anim.frames[c->first_frame + f_n - 1] & 0xfffu), &a[0], &a[1], &a[2]);
            }
            int32_t vx = (int32_t)(((int64_t)(b[0] - a[0]) * s) >> 12), vz = (int32_t)(((int64_t)(b[2] - a[2]) * s) >> 12);
            int32_t dz_soll = (int32_t)((-(int64_t)re15_sin_q12(yaw_b) * vx + (int64_t)re15_cos_q12(yaw_b) * vz) >> 12);
            int32_t dz_ist = e->z - z_b;
            dz_soll_sum += dz_soll; dz_ist_sum += dz_ist;
            if (dz_ist != dz_soll && abw_n++ < 6)
                printf("  wand: Bild %d clip %d f %d yaw %d dz ist %d soll %d\n", f, clip_b, f_b, yaw_b, dz_ist, dz_soll);
        }
        if (re15_gr2_im_dash(slot)) {
            if (f_start < 0) { f_start = f; z_start = e->z; yaw_d = (int)e->rot_y & 0xfff; }
            if (e->motion == 2) n2++; else if (e->motion == 3) n3++;
            if (e->x < x_dmin) x_dmin = e->x;
            if (e->x > x_dmax) x_dmax = e->x;
        } else if (f_start >= 0) break;
    }
    int yaw2 = (int)e->rot_y & 0xfff;
    int32_t weg = z_start - e->z;
    CHECK(dz_ist_sum == dz_soll_sum, "Dash z-Summe ist %d != soll %d (authored Delta um den Yaw gedreht; %d Bilder weichen ab)",
          dz_ist_sum, dz_soll_sum, abw_n);
    CHECK(f_start >= 0 && f_start < 60, "kein Dash beim Zonenwechsel 2 -> 0 in Leons Zone (f_start %d)", f_start);
    CHECK(se_count_since(4, se_vor) >= 1, "kein SE 4 am Dash-Start (@0x80100d64/84)");
    CHECK(n2 == 150 && n3 == 150, "Dash Clip 2 %d / Clip 3 %d Bilder (150/150)", n2, n3);
    CHECK(x_dmin >= -8900 + 1400, "Dash: Gator in der Westwand (x_min %d, Klemmlinie -7434)", x_dmin);
    /* laengs: roh 3535 + 2884 = 6419 x 2731/4096 = 4279, je Bild dreimal floor (Skalierung x/z, Rotation) ->
     * gemessen 4389 bei Yaw 1007; exakt = -dz_soll_sum (dieselbe Integer-Arithmetik wie gr2_rig_welt) */
    CHECK(weg == -dz_soll_sum && weg >= 4000 && weg <= 4500,
          "Dash laengs %d (soll %d aus den authored Deltas; Klemme nur quer)", weg, -dz_soll_sum);
    CHECK(((yaw2 - yaw_d) & 0xfff) == 1024, "Dash-Ende: yaw %d -> %d, erwartet +1024 (@0x80100eb0-c0)", yaw_d, yaw2);
    printf("wand: intro Clip6 %d Clip3 %d x_min %d z_min %d yaw %d->%d | dash ab f%d yaw %d Clip2 %d Clip3 %d x[%d..%d] laengs %d yaw %d\n",
           n6, n3, x_min, z_min, yaw0, yaw1, f_start, yaw_d, n2, n3, x_dmin, x_dmax, weg, yaw2);
    return fails ? 1 : 0;
}

/* ================================================================================================ */
/* Nachtrag 4: RE2-Kameraschnitte. Unabhaengige Nachrechnung der Wahl (Projektion re15_camera_build_view,
 * Kopf = Bone 6 mit Entity-Scale, Blickachse Bone 1 -> Bone 6, frontal = cos in xz, gleitkomma) gegen das
 * Modul: der gewaehlte Cut ist eine Zonenkamera mit dem Kopf im Bild und einem Bildmassstab H * Skala / Tiefe
 * nicht groesser als der groesste RE2-Schnittmassstab (Cut 0, Clip-6-Bild 70: H 257 / Tiefe 8898, Fortsetzung 3),
 * und keine andere solche Kamera liegt naeher am Soll (Toleranz 8/4096 fuer Integer-gegen-Gleitkomma). Soll je
 * Blick = RE2-Messung (Dossier §9.2, probe_kamera_re2.c). */
static const int k_soll[6] = { 0, 4092, 1445, 3386, 4046, 4094 };
static const double k_re2_grenze = 257.0 / 8898.0;   /* RE2 RID 0 fov 32946 >> 7 = 257, Kopf-Tiefe 8898 */
static void kopf_welt(int bone, int32_t out[3])
{
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    int kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, -1, (uint32_t)e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    re15_skel_compute_pose(&bk->skel, kf, poses);
    g_anim_pose_actor = save;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    int32_t tr[3] = { (int32_t)(((int64_t)poses[bone].trans[0] * s) >> 12), (int32_t)(((int64_t)poses[bone].trans[1] * s) >> 12),
                      (int32_t)(((int64_t)poses[bone].trans[2] * s) >> 12) };
    re15_skel_bone_to_world(tr, e->rot_y, e->x, e->y, e->z, out);
}
static int bild(int cut, const int32_t p[3], int *sx, int *sy)
{
    re15_camera_view_t v; re15_camera_build_view(&g_room_rdt.cuts[cut], &v);
    int64_t px = p[0], py = p[1], pz = p[2];
    int32_t vx = (int32_t)(((int64_t)v.rot[0]*px + (int64_t)v.rot[1]*py + (int64_t)v.rot[2]*pz) >> 12) + v.trans[0];
    int32_t vy = (int32_t)(((int64_t)v.rot[3]*px + (int64_t)v.rot[4]*py + (int64_t)v.rot[5]*pz) >> 12) + v.trans[1];
    int32_t vz = (int32_t)(((int64_t)v.rot[6]*px + (int64_t)v.rot[7]*py + (int64_t)v.rot[8]*pz) >> 12) + v.trans[2];
    if (vz <= 0) { *sx = *sy = -9999; return 0; }
    *sx = 160 + (int)(((int64_t)vx * v.fov_screen_dist) / vz); *sy = 120 + (int)(((int64_t)vy * v.fov_screen_dist) / vz);
    return *sx >= 0 && *sx < 320 && *sy >= 0 && *sy < 240;
}
static int zonenkamera(int cut) { int n = 0; for (int i = 0; i < g_room_rdt.zone_count; i++) if (g_room_rdt.zones[i].cam_from == cut) n++; return n >= 2; }
/* Prueft die Wahl im aktuellen Zustand; druckt die Kandidatentabelle (Dossier §9.3). */
static void wahl_pruefen(const char *was, int blick, int gewaehlt, const re15_actor_t *opfer_unbenutzt)
{
    (void)opfer_unbenutzt;
    int32_t k6[3], k1[3], k7[3]; kopf_welt(6, k6); kopf_welt(1, k1); kopf_welt(7, k7);
    /* FINISHER: Kiefer (Part 7) muss mit ins Bild (Opfer im Maul ab P1, gator_re2_r41.c k_blick) */
    const int32_t *opfer = (blick == RE15_GR2_BLICK_FINISHER) ? k7 : NULL;
    double ax = k6[0] - k1[0], az = k6[2] - k1[2], la = sqrt(ax*ax + az*az);
    int best = -1; double best_d = 1e9;
    printf("  %s: Kopf (%d,%d,%d) Achse (%.0f,%.0f), gewaehlt Cut %d\n", was, k6[0], k6[1], k6[2], ax, az, gewaehlt);
    double skala = (e->render_scale_q12 ? e->render_scale_q12 : 4096) / 4096.0;
    for (int stufe = (opfer ? 0 : 2); stufe < 4 && best < 0; stufe++) {   /* wie das Modul: (Opfer, Grenze) abwaerts */
        int mit_o = stufe < 2, mit_g = !(stufe & 1);
        for (int k = 0; k < g_room_rdt.cut_count; k++) {
            if (!zonenkamera(k)) continue;
            int sx, sy, ox = 0, oy = 0;
            int im = bild(k, k6, &sx, &sy);
            int oim = (opfer && mit_o) ? bild(k, opfer, &ox, &oy) : 1;
            const re15_camera_cut_t *c = &g_room_rdt.cuts[k];
            re15_camera_view_t v; re15_camera_build_view(c, &v);
            double vz = (v.rot[6] * (double)k6[0] + v.rot[7] * (double)k6[1] + v.rot[8] * (double)k6[2]) / 4096.0 + v.trans[2];
            double m = vz > 0 ? v.fov_screen_dist * skala / vz : 1e9;
            int gok = !mit_g || m <= k_re2_grenze + 1.0 / 65536.0;
            double cx = c->pos_x - k6[0], cz = c->pos_z - k6[2], lc = sqrt(cx*cx + cz*cz);
            double f = (la > 0 && lc > 0) ? (ax*cx + az*cz) / (la*lc) * 4096.0 : 0;
            if (stufe == (opfer ? 0 : 2))
                printf("    Cut %d: Kopf-Bild (%5d,%5d) %s%s frontal %+5.0f |f-soll| %4.0f dist %5.0f massstab %.4f%s\n", k, sx, sy,
                       im ? "im Bild" : "draussen", opfer ? (oim ? " +Kiefer" : " -Kiefer") : "", f, fabs(f - k_soll[blick]), lc,
                       m, gok ? "" : " >Grenze");
            if (!im || !oim || !gok) continue;
            double d = fabs(f - k_soll[blick]);
            if (d < best_d - 8.0) { best = k; best_d = d; }
            else if (fabs(d - best_d) <= 8.0 && best >= 0) { /* Gleichstand im Rauschen: Modulwahl zulassen */
                if (k == gewaehlt) best = k; }
        }
    }
    CHECK(best == gewaehlt, "%s: Wahl Cut %d, Nachrechnung Cut %d (Blick %d, soll %d)", was, gewaehlt, best, blick, k_soll[blick]);
}
/* Reine Winkelwahl OHNE die Massstab-Grenze (die Wahl vor Fortsetzung 3): bester |frontal - soll| unter den
 * Zonenkameras mit dem Kopf im Bild; *m_out = deren Bildmassstab H * Skala / Tiefe. */
static int winkel_bester(int blick, double *m_out)
{
    int32_t k6[3], k1[3]; kopf_welt(6, k6); kopf_welt(1, k1);
    double ax = k6[0] - k1[0], az = k6[2] - k1[2], la = sqrt(ax*ax + az*az);
    double skala = (e->render_scale_q12 ? e->render_scale_q12 : 4096) / 4096.0, best_d = 1e9;
    int best = -1; *m_out = 0;
    for (int k = 0; k < g_room_rdt.cut_count; k++) {
        if (!zonenkamera(k)) continue;
        int sx, sy; if (!bild(k, k6, &sx, &sy)) continue;
        const re15_camera_cut_t *c = &g_room_rdt.cuts[k];
        re15_camera_view_t v; re15_camera_build_view(c, &v);
        double vz = (v.rot[6] * (double)k6[0] + v.rot[7] * (double)k6[1] + v.rot[8] * (double)k6[2]) / 4096.0 + v.trans[2];
        double cx = c->pos_x - k6[0], cz = c->pos_z - k6[2], lc = sqrt(cx*cx + cz*cz);
        double f = (la > 0 && lc > 0) ? (ax*cx + az*cz) / (la*lc) * 4096.0 : 0;
        if (fabs(f - k_soll[blick]) < best_d) { best_d = fabs(f - k_soll[blick]); best = k; *m_out = vz > 0 ? v.fov_screen_dist * skala / vz : 1e9; }
    }
    return best;
}
static int teil_kamera(void)
{
    if (!setup(1)) return 1;
    /* (a) INTRO vom Lauerplatz, Leon in der Tuernische, Zonenkamera vorher Cut 0 (Leons Eintrittskamera) */
    g_scd.cam_id = 0; g_scd.work_vars[0x0A] = 0; g_scd.cut_auto_enabled = 1;
    leon_statue(-8200, -25550);
    tick();                                    /* INIT + LURK/aggro -> Intro-Start: Programm DURCHBRUCH */
    int cut[24], blick[24], clip[24], bildn[24];
    int n = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
    CHECK(n == 1 && blick[0] == RE15_GR2_BLICK_DURCHBRUCH && clip[0] == 6 && bildn[0] == 0,
          "Intro-Start: erwartet DURCHBRUCH bei Clip 6 Bild 0 (n=%d blick=%d clip=%d bild=%d)", n, n ? blick[0] : -1, n ? clip[0] : -1, n ? bildn[0] : -1);
    if (n >= 1) wahl_pruefen("DURCHBRUCH (Clip 6 Bild 0)", RE15_GR2_BLICK_DURCHBRUCH, cut[0], NULL);
    CHECK(re15_gr2_kamera_gesperrt() && g_scd.cut_auto_enabled == 0, "Intro: Zonenkamera nicht gesperrt (RE2 0x800cfbd8 |= 0x110 @0x8010087c)");
    CHECK(n >= 1 && (int)g_scd.cam_id == cut[0] && g_scd.cam_change_pending, "Intro: Cut_chg-Zwilling nicht angefordert (cam_id %d)", (int)g_scd.cam_id);
    int gesehen_seite = 0, gesehen_angriff = 0, gesperrt_bis = -1;
    for (int f = 0; f < 400; f++) {
        leon_statue(-8200, -25550); tick();
        int m = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
        if (m >= 2 && !gesehen_seite) { gesehen_seite = 1; wahl_pruefen("SEITE (Clip 6 Bild 70)", RE15_GR2_BLICK_SEITE, cut[1], NULL); }
        if (m >= 3 && !gesehen_angriff) { gesehen_angriff = 1; wahl_pruefen("ANGRIFF (Clip-6-Ende)", RE15_GR2_BLICK_ANGRIFF, cut[2], NULL); }
        if (re15_gr2_kamera_gesperrt()) gesperrt_bis = f;
        if (!re15_gr2_im_intro(slot) && f > 2) break;
    }
    n = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
    for (int i = 0; i < n; i++) printf("  Intro-Folge %d: blick %d cut %d clip %d bild %d\n", i, blick[i], cut[i], clip[i], bildn[i]);
    CHECK(n >= 4, "Intro-Folge zu kurz (%d)", n);
    if (n >= 4) {
        CHECK(blick[1] == RE15_GR2_BLICK_SEITE && clip[1] == 6 && bildn[1] == 70, "SEITE nicht bei Clip 6 Bild 70 (@0x80100894-a4): blick %d clip %d bild %d", blick[1], clip[1], bildn[1]);
        CHECK(blick[2] == RE15_GR2_BLICK_ANGRIFF && clip[2] == 6 && bildn[2] == 124, "ANGRIFF nicht am Clip-6-Ende Bild 124 (@0x801008bc-c8): blick %d clip %d bild %d", blick[2], clip[2], bildn[2]);
        CHECK(cut[n-1] == -2 && clip[n-1] == 3 && bildn[n-1] == 149, "Sperre nicht am Clip-3-Ende frei (@0x801008f8-918): cut %d clip %d bild %d", cut[n-1], clip[n-1], bildn[n-1]);
    }
    CHECK(!re15_gr2_kamera_gesperrt() && g_scd.cut_auto_enabled == 1 && g_scd.cam_id == 0,
          "nach dem Intro: Zonenkamera an + zurueck auf Cut 0 (Cut_old-Zwilling) - gesperrt %d auto %d cam %d",
          re15_gr2_kamera_gesperrt(), (int)g_scd.cut_auto_enabled, (int)g_scd.cam_id);
    printf("kamera intro: Sperre bis Takt %d, Schnitte %d\n", gesperrt_bis, n);

    /* (b) R1 Kanister-Explosion aus zwei Ausgangs-Cuts: 7 zeigt den Kopf (RE2: Cut 3/4/5 -> nur Sperre),
     * 2 zeigt ihn nicht (RE2: Cut 6/13, Kopf bei Bild-x -2 -> Cut 5 @0x80102218-6c, zurueck @0x801024a4-ec). */
    for (int fall = 0; fall < 2; fall++) {
        if (!setup(0)) return 1;
        leon_statue(-6000, 2000); tick();
        e->x = -6000; e->z = -22000; e->y = -1200; e->rot_y = 0xC00;
        int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
        re15_gr2_kanister_setzen(-6000, -1200, -22000 + (int32_t)(((int64_t)8800 * s) >> 12));
        for (int f = 0; f < 120 && !re15_gr2_traegt(); f++) { leon_statue(-6000, 2000); e->x = -6000; e->z = -22000; e->rot_y = 0xC00; tick(); }
        CHECK(re15_gr2_traegt(), "R1-Fall %d: Kanister nicht getragen", fall);
        int start = fall == 0 ? 7 : 2;
        g_scd.cam_id = (uint8_t)start; g_scd.work_vars[0x0A] = (int16_t)start; g_scd.cut_auto_enabled = 1;
        int n0 = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
        (void)re15_enemy_take_damage(e, 0);
        tick();
        n = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
        CHECK(re15_gr2_r1_laeuft(slot) && n == n0 + 1 && blick[n0] == RE15_GR2_BLICK_EXPLOSION,
              "R1-Fall %d: kein EXPLOSION-Eintrag beim R1-Start (n %d -> %d)", fall, n0, n);
        CHECK(re15_gr2_kamera_gesperrt() && g_scd.cut_auto_enabled == 0, "R1-Fall %d: keine Sperre (RE2 `ori 0x100` @0x8010220c)", fall);
        if (fall == 0) {
            CHECK(n > n0 && cut[n0] == -1 && g_scd.cam_id == 7, "R1 aus Cut 7 (Kopf im Bild): geschnitten auf %d statt nur Sperre", n > n0 ? cut[n0] : -9);
        } else if (n > n0) {
            CHECK(cut[n0] >= 0 && g_scd.cam_id == (uint8_t)cut[n0], "R1 aus Cut 2 (Kopf draussen): kein Schnitt (cut %d)", cut[n0]);
            wahl_pruefen("EXPLOSION (R1 P0, aus Cut 2)", RE15_GR2_BLICK_EXPLOSION, cut[n0], NULL);
        }
        for (int f = 0; f < 80 && !re15_gr2_tot(); f++) tick();
        n = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
        for (int i = n0; i < n; i++) printf("  R1-Fall %d Folge %d: blick %d cut %d clip %d bild %d\n", fall, i, blick[i], cut[i], clip[i], bildn[i]);
        CHECK(re15_gr2_tot() && !re15_gr2_kamera_gesperrt() && g_scd.cut_auto_enabled == 1 && g_scd.cam_id == (uint8_t)start,
              "R1-Fall %d: nach P3 nicht frei/zurueck (tot %d gesperrt %d auto %d cam %d, Start %d)", fall, re15_gr2_tot(),
              re15_gr2_kamera_gesperrt(), (int)g_scd.cut_auto_enabled, (int)g_scd.cam_id, start);
        if (fall == 1)
            CHECK(n == n0 + 3 && cut[n0+1] == start && blick[n0+1] == 0 && clip[n0+1] == 9 && bildn[n0+1] == 36 && cut[n0+2] == -2,
                  "R1 aus Cut 2: Folge Schnitt -> zurueck (Clip 9 Bild 36, @0x801024a4-ec) -> frei erwartet (n %d)", n - n0);
        else
            CHECK(n == n0 + 2 && cut[n0+1] == -2, "R1 aus Cut 7: Folge Sperre -> frei erwartet (n %d)", n - n0);
    }

    /* (b2) Fortsetzung 3 - die Lage des exe-Laufs kam_k (GB_TEST=5: Gator parkt bei (4997,-20318) Richtung Sueden,
     * Kanister davor, Leons Zonenkamera Cut 0 zeigt den Kopf nicht): die reine Winkelwahl naehme die Kamera mit dem
     * Kopf 3201 vor der Linse (exe: Cut 2, m 2839 * 2^-16 = 2,5-facher RE2-Cut-5-Massstab); die RE2-Grenze (groesster
     * RE2-Schnittmassstab, Cut 0 H 257 / Tiefe 8898) muss sie verwerfen und eine Kamera unter der Grenze waehlen. */
    {
        if (!setup(0)) return 1;
        leon_statue(-6000, 2000); tick();
        e->x = 4997; e->z = -20318; e->y = -1200; e->rot_y = 1024;
        int32_t s2 = e->render_scale_q12 ? e->render_scale_q12 : 4096;
        re15_gr2_kanister_setzen(4997, -1200, -20318 - (int32_t)(((int64_t)8800 * s2) >> 12));
        for (int f = 0; f < 120 && !re15_gr2_traegt(); f++) { leon_statue(-6000, 2000); e->x = 4997; e->z = -20318; e->rot_y = 1024; tick(); }
        CHECK(re15_gr2_traegt(), "R1-Fall 2: Kanister nicht getragen");
        g_scd.cam_id = 0; g_scd.work_vars[0x0A] = 0; g_scd.cut_auto_enabled = 1;
        int n0 = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
        (void)re15_enemy_take_damage(e, 0);
        tick();
        n = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
        CHECK(n == n0 + 1 && blick[n0] == RE15_GR2_BLICK_EXPLOSION && cut[n0] >= 0,
              "R1-Fall 2: kein EXPLOSION-Schnitt aus Cut 0 (n %d -> %d, cut %d)", n0, n, n > n0 ? cut[n0] : -9);
        if (n > n0 && cut[n0] >= 0) {
            wahl_pruefen("EXPLOSION (R1 P0, Lage kam_k, aus Cut 0)", RE15_GR2_BLICK_EXPLOSION, cut[n0], NULL);
            double m_w = 0; int w = winkel_bester(RE15_GR2_BLICK_EXPLOSION, &m_w);
            printf("  R1-Fall 2: Winkelwahl ohne Grenze Cut %d (massstab %.4f, Grenze %.4f), gewaehlt Cut %d\n", w, m_w, k_re2_grenze, cut[n0]);
            CHECK(w >= 0 && w != cut[n0] && m_w > k_re2_grenze,
                  "R1-Fall 2: die Grenze war nicht entscheidend (Winkelwahl %d massstab %.4f, gewaehlt %d)", w, m_w, cut[n0]);
        }
    }

    /* (c) FINISHER: toedlicher Biss in der Tuernische -> FRESSEN P0 (Haken in enemy_ai_boss_gator.c nach der
     * Opfer-Platzierung) -> Sperre + Cut mit Kopf UND Leon im Bild (RE2 Cut 7 @0x80100fbc-ff4), kein Ende. */
    if (!setup(1)) return 1;
    g_scd.cam_id = 0; g_scd.work_vars[0x0A] = 0; g_scd.cut_auto_enabled = 1;
    leon_statue(-8200, -25550); pl->hp = 1;
    int fin = -1;
    for (int f = 0; f < 2500 && fin < 0; f++) {
        leon_statue(-8200, -25550);
        if (pl->hp < 1 && fin < 0) { /* Leon stirbt: Statue nicht mehr erzwingen (Opfer-Modus) */ }
        tick();
        n = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
        for (int i = 0; i < n; i++) if (blick[i] == RE15_GR2_BLICK_FINISHER) { fin = i; break; }
    }
    CHECK(fin >= 0, "kein FINISHER-Schnitt nach dem toedlichen Biss (FRESSEN P0)");
    if (fin >= 0) {
        printf("  FINISHER: Folge %d cut %d clip %d bild %d, Leon (%d,%d)\n", fin, cut[fin], clip[fin], bildn[fin], pl->x, pl->z);
        CHECK(clip[fin] == 4 && bildn[fin] == 0, "FINISHER nicht bei Clip 4 Bild 0 (FRESSEN P0): clip %d bild %d", clip[fin], bildn[fin]);
        wahl_pruefen("FINISHER (FRESSEN P0)", RE15_GR2_BLICK_FINISHER, cut[fin], pl);
        CHECK(re15_gr2_kamera_gesperrt() && g_scd.cut_auto_enabled == 0, "FINISHER: keine Sperre (@0x80100fcc)");
    }
    return fails ? 1 : 0;
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "clips";
    int rc = 1;
    if (!strcmp(t, "modell")) rc = teil_modell();
    else if (!strcmp(t, "clips")) rc = teil_clips();
    else if (!strcmp(t, "dash")) rc = teil_dash();
    else if (!strcmp(t, "intro")) rc = teil_intro();
    else if (!strcmp(t, "sockel")) rc = teil_sockel();
    else if (!strcmp(t, "kanister")) rc = teil_kanister();
    else if (!strcmp(t, "kiefer")) rc = teil_kiefer();
    else if (!strcmp(t, "putz")) rc = teil_putz();
    else if (!strcmp(t, "wand")) rc = teil_wand();
    else if (!strcmp(t, "kamera")) rc = teil_kamera();
    else printf("unbekannter Teil %s\n", t);
    printf("%s: %s (%d Fehler)\n", t, rc ? "FAIL" : "OK", fails);
    return rc;
}
