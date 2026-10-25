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
 *   lauer     Nachbesserung 3 (Abnahme 2 M1, Entscheid B9): bis zum Durchbruch untergetaucht wie RE2 (Sce_em_set y=+3000
 *             @RDT 0x25A0): nicht zeichnen, keine Trefferbox/kein Ziel, keine Animation; Auftauchen im Intro-Start-Takt
 *             auf der Pos_set-Lage (Clip 6 Bild 0), danach kein Schritt groesser als der groesste authored Wurzelschritt
 *   schleppen Nachbesserung 3 (M2): Fenstertreffer -> Stufe 1 + erster Stufe-2-Schritt im selben Takt (@0x8010183c-5c ->
 *             @0x80101860), bis zum Anhaengen (t = 13 @0x801018c4-cc) keine Koerperlenkung, Kanister-Weg <= Gator-Weg,
 *             Schub nur laengs (@0x801018a4-c0); danach lenkt die Verfolgung wieder
 *   anschwimmen Nachbesserung 3 (M3): Zielmodus +0x1C0 & 4 (@0x80100a34-b4) sperrt den Biss (T1 @0x8010134c-58), solange der
 *             liegende Kanister >= 8880 x Skala voraus in der Zone des Gators liegt; < 8880, hinter dem Gator, in einer
 *             anderen Zone oder ohne Kanister beisst er
 *
 *   maul      Nachbesserung 5 (Abnahme 4 M1): nach JEDEM toedlichen Biss (CHASE-Bezug + vier GUARD-Hochbiss-Lagen mit Leon oben)
 *             liegt Leon ueber P1-P3 im GEZEICHNETEN Maul wie im Bezug (Maulraum von unit_gator_fress mit Haken, Kopf-Rahmen,
 *             Abstand zum gezeichneten Kiefer), alles im Wasser, Kopf + Leon im Bild der Finisher-Kamera
 *   buehne    Nachbesserung 5: Finisher-Buehne (RE2-P0-Form @0x80101048-4c) fuer jede Gator-Lage gefunden (Raster 1000 x 8 Gier)
 *
 * Seit Nachbesserung 3 ist der Gator im NATUERLICHEN Lauern untergetaucht (kein Kanister-Fenster, RE2 Sub-0-Pre leer):
 * Riegel, die einen ruhenden Gator den Kanister aufnehmen lassen, laufen mit RE15_GB_TEST=5 (Marker-Repro: aufgetaucht
 * wartend an der Marker-Lage, ohne Aggro) - setup(5).
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
#include "re15_math.h"       /* Nachbesserung 5: re15_squareroot0 (Riegel buehne) */
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

/* Nachbesserung 6 (Abnahme 5 M3): die Raum-Objekte von ROOM2090 (Obj_model_set aus main00: obj 0 Kleinteil Rampe, obj 1 Typ-3-Raster =
 * Wasseroberflaeche, obj 2/3 treibende Leichen) - der Riegel braucht sie, weil die Buehne verdeckende Objekte meidet. Die SCD laeuft wie
 * im Raumlader (scd_vm_init + main00, re15-room-probe-Muster); setup() legt danach Aktoren/AOTs/Gegner neu an, die Objekte bleiben in
 * g_scd.props. */
static int s_mit_objekten;
extern void scd_register_current_rdt(const re15_rdt_t *rdt);
extern int  scd_thread_start(int slot, const uint8_t *pc);
static void objekte_installieren(void)
{
    g_current_room_id = 0x2090;
    scd_vm_init();
    scd_register_current_rdt(&g_room_rdt);
    if (g_room_rdt.main_scd) scd_thread_start(0, g_room_rdt.main_scd);
    for (int i = 0; i < 60; i++) scd_vm_tick();
}
static int setup(int gb_test)
{
#ifdef _WIN32
    _putenv(gb_test == 5 ? "RE15_GB_TEST=5" : gb_test ? "RE15_GB_TEST=1" : "RE15_GB_TEST=");
    if (!getenv("R41B_LOG")) _putenv("RE15_GB_STUMM=1");   /* R41B_LOG=1: gator_boss.log schreiben (Diagnose) */
    _putenv("RE15_GATOR_KANISTER=");
#else
    if (gb_test) setenv("RE15_GB_TEST", gb_test == 5 ? "5" : "1", 1); else unsetenv("RE15_GB_TEST");
    if (!getenv("R41B_LOG")) setenv("RE15_GB_STUMM", "1", 1);
    unsetenv("RE15_GATOR_KANISTER");
#endif
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf) { printf("FAIL: ROOM2090.RDT nicht lesbar\n"); return 0; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 0; }
    g_room_rdt_ok = 1;
    if (s_mit_objekten) objekte_installieren();   /* Nachbesserung 6 (M3) */
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
    if (!setup(5)) return 1;                    /* NB3: GB_TEST=5 = aufgetaucht wartend (das natuerliche Lauern ist untergetaucht) */
    leon_statue(-6000, 2000);                   /* kein Aggro: LURK (aufgetaucht), Kanister-Vorpass laeuft dort */
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
    /* Nachbesserung 3 (M2): Stufe 1 laeuft im Fenster-Takt ohne Sprung in den ersten Stufe-2-Schritt (t = 0, Tafel @0x80100024:
     * [1] 0x8010183c -> [2] 0x80101860), Anhaengen beim Schritt t = 12 -> 13 (@0x801018c4-cc): Stufe 2 steht nach 12 Takten. */
    CHECK(stufe_bilder[1] == 0 && stufe_bilder[2] == 12, "Stufe 1/2 %d/%d Bilder statt 0/12 (@0x8010183c-5c -> @0x80101860, "
          "t = 13 @0x801018c4)", stufe_bilder[1], stufe_bilder[2]);
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
    if (!setup(5)) return 1;                    /* NB3: aufgetaucht wartend (s. teil_kanister) */
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
/* Runde 41 Spur G (Nachbesserung 3, M3 / Nachtrag 28): der Kanister ZERPLATZT. R1 P0 ruft FUN_80036eb4 auf obj[1] (`jal 0x80036eb4`
 * @0x80102348): +0x14C |= 0x4A (`ori v0,v0,0x4a` @0x80036ee4), +0x1EC = 0x8000 (@0x80036eb8-bc), +0x1BC = 0x404040 (@0x80036ecc-d0).
 * Der Objekt-Zeichner FUN_800363f0 zeichnet dann bei Bit 0x08 (@0x80036558-64) die Scherben (FUN_8002d3c8/FUN_8002d718: Skala =
 * Tafel[+0x1EC >> 15][+0x1EC & 0x7fff] @0x8009DC28/@0x8009DC64, Farbe flach +0x70) und zaehlt +0x1EC je gezeichnetes Bild
 * (@0x800365bc-c8); R1 P1 nimmt obj[1] nach neun Bildern weg (+0x16B @0x8010255c-78). Der Riegel ruft Abfrage und Weiterzaehlen
 * so, wie der PC-Zeichner (gator_re2_pc.c) es je Bild nach dem Takt tut: zehn Scherben-Bilder, Skala 180..1530 (Zeile 1, Index
 * 0..9), Farbe 0x404040; vor dem Treffer das intakte Mesh. */
static int teil_scherben(void)
{
    if (!setup(5)) return 1;
    leon_statue(-6000, 2000);
    tick();
    e->x = -6000; e->z = -22000; e->y = -1200; e->rot_y = 0xC00;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    re15_gr2_kanister_setzen(-6000, -267, -22000 + (int32_t)(((int64_t)8800 * s) >> 12));
    for (int f = 0; f < 60 && re15_gr2_kanister_stufe() != 7; f++) {
        leon_statue(-6000, 2000); e->x = -6000; e->z = -22000; e->rot_y = 0xC00; tick();
    }
    CHECK(re15_gr2_kanister_stufe() == 7, "Kanister nicht getragen (Stufe %d)", re15_gr2_kanister_stufe());
    int32_t sk = -1; uint32_t fb = 0; int bi = -1;
    CHECK(re15_gr2_kanister_scherben(&sk, &fb, &bi) == 0, "Scherben schon VOR dem Treffer (getragen = intaktes Mesh)");
    (void)re15_enemy_take_damage(e, 0);
    tick();                                                     /* -> R1 P0: FUN_80036eb4 @0x80102348 */
    CHECK(re15_gr2_kanister_stufe() == 13, "Kanister nach R1 in Stufe %d statt 13", re15_gr2_kanister_stufe());
    static const int soll[10] = { 180, 354, 522, 684, 840, 990, 1134, 1272, 1404, 1530 };   /* @0x8009DC64 [0..9] */
    int bilder = 0, folge_ok = 1, farbe_ok = 1, weg_ab = -1;
    for (int k = 0; k < 14; k++) {
        sk = -1; fb = 0; bi = -1;
        if (re15_gr2_kanister_scherben(&sk, &fb, &bi)) {        /* PC-Zeichner: Scherben dieses Bildes */
            if (bilder < 10 && (sk != soll[bilder] || bi != bilder)) {
                folge_ok = 0; printf("  Bild %d: Index %d Skala %d (soll %d)\n", bilder, bi, (int)sk, soll[bilder]);
            }
            if (fb != 0x00404040u) farbe_ok = 0;
            bilder++;
            re15_gr2_kanister_scherben_weiter();               /* +0x1EC += 1 @0x800365bc-c8 */
        } else if (weg_ab < 0) weg_ab = k;
        tick();
    }
    CHECK(bilder == 10, "Scherben-Bilder %d != 10 (P0 + neun P1-Bilder, +0x16B = 9 @0x80102394, @0x8010255c-78)", bilder);
    CHECK(folge_ok, "Scherben-Skala nicht 180..1530 in Index 0..9 (Zeile 1 @0x8009DC64, Bit 15 von 0x8000)");
    CHECK(farbe_ok, "Scherben-Farbe nicht 0x404040 (+0x1BC @0x80036ecc-d0)");
    CHECK(weg_ab == 10 && re15_gr2_kanister_stufe() == -1, "Kanister weg ab Bild %d (soll 10), Stufe danach %d", weg_ab,
          re15_gr2_kanister_stufe());
    printf("scherben: %d Bilder Skala 180..1530 Farbe 0x404040, weg ab Bild %d\n", bilder, weg_ab);
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
    /* Fortsetzung 3: WIRKSAME Lage der lebenden Putz-Teilchen (Pool-Platz +0x34/+0x36/+0x38 nach FUN_8001d894,
     * Bank 4 in +0x1C) - vorher lagen sie bei (67,-2000,2) .. (-634,-4000,-722) um den Ursprung, weil die Lage nur in
     * einer Stack-Matrix stand (haengender Zeiger +0x6C, @0x8001cd6c) statt als Versatz zur Einheitsmatrix 0x8009db44
     * (Sce_espr_on @0x800565a4-0x80056618). */
    int lebend_max = 0, weit_max = 0, gesehen = 0;
    for (int f = 0; f < 120; f++) {
        leon_statue(-6500, -8000); tick(); re2fx_tick();
        int lebend = 0;
        for (int i = 0; i < RE2FX_PLAETZE; i++) {
            const uint8_t *pb = re2fx_platz(i);
            if (!pb || !(pb[0x18] | pb[0x19]) || (pb[0x1C] | (pb[0x1D] << 8)) != 4) continue;
            int wx = (int16_t)(pb[0x34] | (pb[0x35] << 8)), wz = (int16_t)(pb[0x38] | (pb[0x39] << 8));
            int dx = wx - (-6500), dz = wz - (-8000), d = (int)sqrt((double)dx * dx + (double)dz * dz);
            if (d > weit_max) weit_max = d;
            lebend++; gesehen++;
        }
        if (lebend > lebend_max) lebend_max = lebend;
    }
    CHECK(gesehen > 0 && weit_max <= 3000,
          "Putz-Teilchen nicht um Leon: %d Beobachtungen, groesster xz-Abstand %d (Soll <= 3000; RE2-Versaetze <= 1122)", gesehen, weit_max);
    printf("putz: lebende Teilchen max %d/Bild, groesster xz-Abstand zu Leon %d\n", lebend_max, weit_max);
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
 * Nachbesserung 2 (Abnahme 1, M2 + Orchestrator-Entscheid B7): die RE2-Wende (Clip 3 Root-ry 438 -> 1023, danach
 * `yaw += 1024` @0x80100938-48 / @0x80100eb0-c0) darf Kopf und Rumpf NICHT in die Beckenwand drehen.
 * (a) Intro, Leon in der Tuernische (-8200,-25550): gr2_intro_lage waehlt Startlage/-gier (Pos_set-Zwilling, RE2 sub11
 *     @SCD 0x0736) - in JEDEM der 275 Intro-Bilder liegen Rumpf (Bones 0/1), Hals 5, Kopf 6 und Kiefer 7 im Wasser des
 *     Beckens (Raumgeometrie ROOM2090-SCA [0]-[5]: Becken x[-8900,7200] z[-27000,-5400], Plattform x[-1700,1850]
 *     z[-20000,-12450], Rampe x[1850,7200] z[-18100,-14500]); Clip 6 (125) -> Clip 3 (150) -> yaw += 1024; die End-Gier
 *     zeigt auf Leon (Peilung, Toleranz 128); Biss-Sperre 110 (@0x8010091c-20). Vorher (Lauerplatz, Gier 1089): Kopf am
 *     Clip-3-Ende 4988 in der Westwand (probe_wende.c, = Abnahme-1-Lauf A).
 * (b) Dash am Suedausgang des Westkanals, Gator auf der Klemmlinie (-7434,-17900) Blick Sued, Leon Zone 0: Zonenwechsel
 *     2 -> 0 -> gr2_dash_planen dreht VOR Clip 2 auf die kleinste Gier mit freier Wende (vorher Kopf 4474 in der
 *     Westwand), dann SE 4 + Clip 2/3 je 150 (@0x80100dac-e28) entlang der authored Deltas (x UND z, um die Gier gedreht),
 *     am Ende yaw += 1024 (@0x80100eb0-c0); Rumpf/Hals/Kopf/Kiefer in jedem Bild im Wasser. */
static int32_t wand_draussen(int32_t x, int32_t z)
{
    int32_t d = 0;
    if (x < -8900) d = -8900 - x;
    if (x > 7200 && x - 7200 > d) d = x - 7200;
    if (z < -27000 && -27000 - z > d) d = -27000 - z;
    if (z > -5400 && z + 5400 > d) d = z + 5400;
    static const int32_t blk[2][4] = { { -1700, 1850, -20000, -12450 }, { 1850, 7200, -18100, -14500 } };
    for (int k = 0; k < 2; k++)
        if (x > blk[k][0] && x < blk[k][1] && z > blk[k][2] && z < blk[k][3]) {
            int32_t t = x - blk[k][0];
            if (blk[k][1] - x < t) t = blk[k][1] - x;
            if (z - blk[k][2] < t) t = z - blk[k][2];
            if (blk[k][3] - z < t) t = blk[k][3] - z;
            if (t > d) d = t;
        }
    return d;
}
/* groesster Aussenabstand der Rumpf-/Kopf-Bones dieses Bildes; *kopf_x = Kopf (Bone 6) x */
static int32_t wand_bild(int32_t *kopf_x)
{
    static const int bones[5] = { 0, 1, 5, 6, 7 };
    int32_t m = 0;
    for (int i = 0; i < 5; i++) {
        int32_t p[3] = { e->x, e->y, e->z }; re15_gr2_bone_welt_pub(e, bones[i], p);   /* gezeichnete Lage (Skala 2731) */
        int32_t d = wand_draussen(p[0], p[2]);
        if (d > m) m = d;
        if (bones[i] == 6 && kopf_x) *kopf_x = p[0];
    }
    return m;
}
static int teil_wand(void)
{
    if (!setup(1)) return 1;
    leon_statue(-8200, -25550);
    tick();                                   /* INIT: Lauerplatz -> aggro (GB_TEST=1) -> Intro-Lage + Intro-Start */
    int yaw0 = (int)e->rot_y & 0xfff;
    int32_t sx = e->x, sz = e->z;
    int32_t kx = 0, raus_intro = wand_bild(&kx), kx_min = kx;
    int n6 = (e->motion == 6) ? 1 : 0, n3 = 0, f;
    for (f = 0; f < 400; f++) {
        leon_statue(-8200, -25550); tick();
        if (!re15_gr2_im_intro(slot) && f > 2) break;
        if (e->motion == 6) n6++; else if (e->motion == 3) n3++;
        int32_t d = wand_bild(&kx);
        if (d > raus_intro) raus_intro = d;
        if (kx < kx_min) kx_min = kx;
    }
    int yaw1 = (int)e->rot_y & 0xfff;
    int peil = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff;
    int abw = ((yaw1 - peil + 0x800) & 0xfff) - 0x800; if (abw < 0) abw = -abw;
    CHECK(sx != -7200 || sz != -9200, "Intro: keine Startlage gewaehlt (Lauerplatz (-7200,-9200) mit Gier 1089 dreht in die Westwand)");
    CHECK(n6 == 125 && n3 == 150, "Intro Clip 6 %d / Clip 3 %d Bilder (RE2: 125/150, EDD)", n6, n3);
    CHECK(raus_intro == 0, "Intro: Rumpf/Kopf %d ausserhalb des Wassers (Kopf x min %d, Westwand -8900)", raus_intro, kx_min);
    CHECK(((yaw1 - yaw0) & 0xfff) == 1024, "Intro-Ende: yaw %d -> %d, erwartet +1024 (@0x80100938-48)", yaw0, yaw1);
    CHECK(abw <= 128, "Intro-Ende zeigt nicht auf Leon: Gier %d, Peilung %d (Abw. %d > 128)", yaw1, peil, abw);
    CHECK(re15_gr2_biss_cd_rest(slot) == RE15_GR2_BISS_CD_INTRO - 1,
          "Biss-Sperre im ersten CHASE-Takt %d != 109 (110 im Ende-Takt @0x8010091c-20, ein Takt abgezogen)", re15_gr2_biss_cd_rest(slot));
    /* (b) Dash: Klemmlinie im Westkanal, Blick Sued, Leon Zone 0 */
    pl->hp = 100;
    e->x = -7434; e->z = -17900; e->y = -1200; e->rot_y = 1024;
    int f_start = -1, n2 = 0, yaw_d = 0, vor = 0; n3 = 0;
    int32_t raus_dash = 0, kx_dmin = 0x7fffffff;
    int32_t dx_soll_sum = 0, dz_soll_sum = 0, dx_ist_sum = 0, dz_ist_sum = 0; int abw_n = 0;
    int se_vor = s_se_n;
    for (f = 0; f < 600; f++) {
        int clip_b = (int)e->motion, f_b = (int)e->anim_frame, yaw_b = (int)e->rot_y & 0xfff;
        int32_t x_b = e->x, z_b = e->z;
        int im_dash_b = re15_gr2_im_dash(slot);
        leon_statue(-8200, -25550); tick();
        if (im_dash_b && (clip_b == 2 || clip_b == 3)) {
            /* Soll = Schritt des NEUEN Bildes f_b+1 (FUN_8001a330: Vorschub, dann e7c+152c8); das letzte Bild
             * (f = n-1) wird ohne Schritt gezeigt (`bne v0,zero,0x8001a3cc` @0x8001a3b0). */
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
            int32_t dx_soll = (int32_t)(((int64_t)re15_cos_q12(yaw_b) * vx + (int64_t)re15_sin_q12(yaw_b) * vz) >> 12);
            int32_t dz_soll = (int32_t)((-(int64_t)re15_sin_q12(yaw_b) * vx + (int64_t)re15_cos_q12(yaw_b) * vz) >> 12);
            dx_soll_sum += dx_soll; dz_soll_sum += dz_soll;
            dx_ist_sum += e->x - x_b; dz_ist_sum += e->z - z_b;
            if ((e->x - x_b != dx_soll || e->z - z_b != dz_soll) && abw_n++ < 6)
                printf("  wand: Bild %d clip %d f %d yaw %d d ist (%d,%d) soll (%d,%d)\n", f, clip_b, f_b, yaw_b,
                       e->x - x_b, e->z - z_b, dx_soll, dz_soll);
        }
        if (re15_gr2_dash_vordrehung(slot)) vor++;                 /* Vordrehung auf die Plan-Gier (vor Clip 2) */
        if (re15_gr2_im_dash(slot) || re15_gr2_dash_vordrehung(slot)) {
            int32_t d = wand_bild(&kx);
            if (d > raus_dash) raus_dash = d;
            if (kx < kx_dmin) kx_dmin = kx;
        }
        if (re15_gr2_im_dash(slot)) {
            if (f_start < 0) { f_start = f; yaw_d = (int)e->rot_y & 0xfff; }
            if (e->motion == 2) n2++; else if (e->motion == 3) n3++;
        } else if (f_start >= 0) break;
    }
    int yaw2 = (int)e->rot_y & 0xfff;
    CHECK(f_start >= 0 && f_start < 60, "kein Dash beim Zonenwechsel 2 -> 0 in Leons Zone (f_start %d)", f_start);
    CHECK(vor > 0 && yaw_d != 1024, "keine Vordrehung vor dem Dash (Start-Gier %d, %d Drehbilder) - Gier 1024 dreht in die Westwand", yaw_d, vor);
    CHECK(se_count_since(4, se_vor) >= 1, "kein SE 4 am Dash-Start (@0x80100d64/84)");
    CHECK(n2 == 150 && n3 == 150, "Dash Clip 2 %d / Clip 3 %d Bilder (150/150)", n2, n3);
    CHECK(dx_ist_sum == dx_soll_sum && dz_ist_sum == dz_soll_sum,
          "Dash-Bahn ist (%d,%d) != soll (%d,%d) aus den authored Deltas (%d Bilder weichen ab)", dx_ist_sum, dz_ist_sum,
          dx_soll_sum, dz_soll_sum, abw_n);
    CHECK(raus_dash == 0, "Dash: Rumpf/Kopf %d ausserhalb des Wassers (Kopf x min %d)", raus_dash, kx_dmin);
    CHECK(((yaw2 - yaw_d) & 0xfff) == 1024, "Dash-Ende: yaw %d -> %d, erwartet +1024 (@0x80100eb0-c0)", yaw_d, yaw2);
    printf("wand: intro Start (%d,%d) yaw %d Clip6 %d Clip3 %d -> yaw %d (Peilung Leon %d, Abw %d), draussen %d, Kopf x min %d | "
           "dash Vordrehung %d Bilder 1024 -> %d, ab f%d Clip2 %d Clip3 %d, Bahn (%d,%d), draussen %d, Kopf x min %d, yaw %d\n",
           sx, sz, yaw0, n6, n3, yaw1, peil, abw, raus_intro, kx_min, vor, yaw_d, f_start, n2, n3, dx_ist_sum, dz_ist_sum,
           raus_dash, kx_dmin, yaw2);
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
        if (!setup(5)) return 1;                /* NB3: aufgetaucht wartend (s. teil_kanister) */
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
        /* Runde 41 Spur G, Nachbesserung 3 (Abnahme G 2 M1, Orchestrator-Entscheid G-E1): die R1-Kamera waehlt jetzt G-E1 (Kopf,
         * Kiefer und Spritzring-Punkte mit Sprite-Rand im Bild, im Regions-Viereck, unverdeckt, meiste sichtbare Proben des
         * Todes-Clips, Kopf naechst RE2 (170,111); gator_re2_r41.c gr2_r1_wahl) statt "Kopf im laufenden Cut -> nur Sperre, sonst
         * bester Frontalwinkel". Die alte Erwartung (aus Cut 7 kein Schnitt, aus Cut 2 Winkelwahl) ist durch G-E1 widerlegt;
         * geprueft wird die G-E1-Eigenschaft: der gezeigte Cut hat den Kopf im Bild, die Folge Sperre [-> Schnitt -> zurueck] -> frei. */
        if (n > n0) {
            const int gezeigt = cut[n0] >= 0 ? cut[n0] : start;
            int32_t kk[3]; kopf_welt(6, kk);
            int kx = 0, ky = 0; const int im = bild(gezeigt, kk, &kx, &ky);
            printf("  R1-Fall %d (aus Cut %d): G-E1 zeigt Cut %d%s, Kopf (%d,%d)\n", fall, start, gezeigt, cut[n0] >= 0 ? " (Schnitt)" : "", kx, ky);
            CHECK(im, "R1-Fall %d: G-E1-Cut %d zeigt den Kopf nicht (%d,%d)", fall, gezeigt, kx, ky);
            CHECK(cut[n0] < 0 ? g_scd.cam_id == (uint8_t)start : g_scd.cam_id == (uint8_t)cut[n0], "R1-Fall %d: cam_id %d passt nicht zur Wahl",
                  fall, (int)g_scd.cam_id);
        }
        const int geschnitten = n > n0 && cut[n0] >= 0;
        for (int f = 0; f < 80 && !re15_gr2_tot(); f++) tick();
        n = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
        for (int i = n0; i < n; i++) printf("  R1-Fall %d Folge %d: blick %d cut %d clip %d bild %d\n", fall, i, blick[i], cut[i], clip[i], bildn[i]);
        CHECK(re15_gr2_tot() && !re15_gr2_kamera_gesperrt() && g_scd.cut_auto_enabled == 1 && g_scd.cam_id == (uint8_t)start,
              "R1-Fall %d: nach P3 nicht frei/zurueck (tot %d gesperrt %d auto %d cam %d, Start %d)", fall, re15_gr2_tot(),
              re15_gr2_kamera_gesperrt(), (int)g_scd.cut_auto_enabled, (int)g_scd.cam_id, start);
        if (geschnitten)
            CHECK(n == n0 + 3 && cut[n0+1] == start && blick[n0+1] == 0 && clip[n0+1] == 9 && bildn[n0+1] == 36 && cut[n0+2] == -2,
                  "R1 aus Cut %d: Folge Schnitt -> zurueck (Clip 9 Bild 36, @0x801024a4-ec) -> frei erwartet (n %d)", start, n - n0);
        else
            CHECK(n == n0 + 2 && cut[n0+1] == -2, "R1 aus Cut %d: Folge Sperre -> frei erwartet (n %d)", start, n - n0);
    }

    /* (b2) Fortsetzung 3 - die Lage des exe-Laufs kam_k (GB_TEST=5: Gator parkt bei (4997,-20318) Richtung Sueden,
     * Kanister davor, Leons Zonenkamera Cut 0 zeigt den Kopf nicht): die reine Winkelwahl naehme die Kamera mit dem
     * Kopf 3201 vor der Linse (exe: Cut 2, m 2839 * 2^-16 = 2,5-facher RE2-Cut-5-Massstab); die RE2-Grenze (groesster
     * RE2-Schnittmassstab, Cut 0 H 257 / Tiefe 8898) muss sie verwerfen und eine Kamera unter der Grenze waehlen. */
    {
        if (!setup(5)) return 1;                /* NB3: aufgetaucht wartend = die Lage des exe-Laufs kam_k (GB_TEST=5) */
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
            /* Runde 41 Spur G, G-E1 (s.o.): statt der Winkelwahl die G-E1-Eigenschaft - Kopf im gewaehlten Cut im Bild */
            int32_t kk[3]; kopf_welt(6, kk);
            int kx = 0, ky = 0;
            CHECK(bild(cut[n0], kk, &kx, &ky), "R1-Fall 2: G-E1-Cut %d zeigt den Kopf nicht (%d,%d)", cut[n0], kx, ky);
            printf("  R1-Fall 2: G-E1 Cut %d, Kopf (%d,%d)\n", cut[n0], kx, ky);
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
    int fin = -1, vor_angezeigt = -1, vor_angefordert = -1;
    for (int f = 0; f < 2500 && fin < 0; f++) {
        leon_statue(-8200, -25550);
        if (pl->hp < 1 && fin < 0) { /* Leon stirbt: Statue nicht mehr erzwingen (Opfer-Modus) */ }
        vor_angezeigt = (int)g_scd.work_vars[0x0A]; vor_angefordert = (int)g_scd.cam_id;   /* NB6 (M2): Kamera des Biss-Bildes */
        tick();
        n = re15_gr2_kamera_folge(24, cut, blick, clip, bildn);
        for (int i = 0; i < n; i++) if (blick[i] == RE15_GR2_BLICK_FINISHER) { fin = i; break; }
    }
    CHECK(fin >= 0, "kein FINISHER-Schnitt nach dem toedlichen Biss (FRESSEN P0)");
    if (fin >= 0) {
        printf("  FINISHER: Folge %d cut %d clip %d bild %d, Leon (%d,%d), Kamera im Biss-Bild %d/%d\n", fin, cut[fin], clip[fin], bildn[fin],
               pl->x, pl->z, vor_angezeigt, vor_angefordert);
        CHECK(clip[fin] == 4 && bildn[fin] == 0, "FINISHER nicht bei Clip 4 Bild 0 (FRESSEN P0): clip %d bild %d", clip[fin], bildn[fin]);
        /* Nachbesserung 6 (Abnahme 5 M2): nie die laufende Kamera - das Umsetzen faellt immer in einen Schnitt */
        CHECK(cut[fin] != vor_angezeigt && cut[fin] != vor_angefordert, "FINISHER: Cut %d = laufende Kamera %d/%d (kein Schnitt)",
              cut[fin], vor_angezeigt, vor_angefordert);
        /* die Wahl wird an der BUEHNE nachgerechnet (P0 setzt den Gator dorthin, @0x80101048-4c) - ein Takt weiter */
        tick();
        wahl_pruefen("FINISHER (FRESSEN P0, Buehne)", RE15_GR2_BLICK_FINISHER, cut[fin], pl);
        CHECK(re15_gr2_kamera_gesperrt() && g_scd.cut_auto_enabled == 0, "FINISHER: keine Sperre (@0x80100fcc)");
        /* Nachbesserung 2 (Abnahme 1, M1 b) + Entscheid B6: der Finisher-Schnitt steht bis zum Game Over (RE2 Sub 4 hat
         * keinen Freigabe-Pfad; `addiu v1,zero,-257` nur @0x80100910 / @0x80102688 / @0x801028ac / @0x80102b1c). Die RE1.5-Todes-Praesentation
         * (game_step_common.c FSM, laeuft im Spiel VOR dem Boss-Takt) setzt Weissblende / Schwarzgrund / Orbit - hier
         * je Takt so gesetzt, wie die FSM es taete; der Boss-Takt muss sie zuruecknehmen und die Sperre halten. */
        int cam_fin = cut[fin] >= 0 ? cut[fin] : (int)g_scd.cam_id, halt = 0, pl_hp = pl->hp;
        for (int f = 0; f < 400; f++) {
            g_death_cam = 1; g_death_white = 200; g_death_blackbg = 1; g_death_glow = 255;
            g_scd.cut_auto_enabled = 1;                  /* auch ein fremdes Freigeben haelt der Takt nicht aus */
            tick();
            if (!g_death_cam && !g_death_white && !g_death_blackbg && !g_death_glow && g_scd.cut_auto_enabled == 0
                && (int)g_scd.cam_id == cam_fin && re15_gr2_kamera_gesperrt()) halt++;
        }
        printf("  FINISHER-Halt: %d/400 Takte Cut %d gesperrt, RE1.5-Todesbild (Orbit/Weiss/Schwarz) aus, Leon-HP %d\n",
               halt, cam_fin, pl_hp);
        CHECK(halt == 400, "FINISHER-Schnitt nicht bis zum Ende gehalten / RE1.5-Todesbild nicht unterdrueckt: %d/400 (B6)", halt);
    }
    return fails ? 1 : 0;
}

/* ================================================================================================ */
/* Nachbesserung 2 (Abnahme 1, M3): Kanister-Aufnahme IN DER VERFOLGUNG wie RE2 (EM23_OVL_0000.BIN, Sub-1/2-Pre
 * @0x80100964): (1) Aufnahmebeginn schaltet PADDELN (Sub 2, Clip 1) auf SCHWIMMEN (Sub 1, Clip 0, Bild 0, Blend 32):
 * @0x80100b04-18 -> `sw 0x101,4(s0)` @0x80100c04, Sub-1-Main FUN_8001a330(em,0,32,0) @0x80100c34-40 im selben Tick;
 * (2) Stufe 1..6: keine Paddel-Hysterese (@0x80100b20-30) und kein Biss (T1 nur bei Stufe 0/7, FUN_801012fc
 * @0x80101360-70); (3) in der Zone des Kanisters (RE2-Zone 1 + Raumflag Bank 5 Bit 0 @0x80100ab8-c8, nie geloescht)
 * bleibt der Sub auch beim Tragen (Stufe 7) stehen (@0x80100aec / @0x80100afc -> 0x80100c08); (4) ausserhalb dieser
 * Zone laeuft die Hysterese bei Stufe 7 wieder (@0x80100b20-30 laesst 7 durch) und der Biss ist wieder erlaubt. */
static int teil_aufnahme(void)
{
    if (!setup(1)) return 1;
    leon_statue(-6000, -18200);                /* Zone 0 wie teil_clips: Paddeln, kein Dash, kein Biss (6800) */
    tick();
    intro_abwarten();
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    int paddeln = 0;
    for (int f = 0; f < 12; f++) {
        leon_statue(-6000, -18200); e->x = -6000; e->z = -25000; e->y = -1200; e->rot_y = 0xC00;
        tick();
        if (e->motion == 1) paddeln++;
    }
    CHECK(e->motion == 1 && paddeln >= 10, "vor dem Kanister kein Paddeln (motion=%d, %d/12)", (int)e->motion, paddeln);
    /* Kanister 8800 x Scale vor der Schnauze, Liegestelle in Zone 0 (z -19133 <= -18100) = Zone des Gators */
    int32_t laengs = (int32_t)(((int64_t)8800 * s) >> 12);
    re15_gr2_kanister_setzen(-6000, -1200, -25000 + laengs);
    leon_statue(-6000, -18200); e->x = -6000; e->z = -25000; e->rot_y = 0xC00;
    tick();
    /* Nachbesserung 3 (M2): Fenster (Pre @0x80100ae4-b08) und T2 Stufe 1 + erster Stufe-2-Schritt (@0x8010183c-5c -> @0x80101860)
     * im selben Takt -> nach dem Takt Stufe 2 */
    CHECK(re15_gr2_kanister_stufe() == 2, "Fenster nicht getroffen bzw. Stufe 1 nicht im selben Takt (Stufe %d, @0x80100ae4-ec, "
          "@0x8010183c-5c)", re15_gr2_kanister_stufe());
    CHECK(e->motion == 0 && e->anim_frame == 0 && e->anim_frac == 32,
          "Aufnahmebeginn nicht Sub 2 -> Sub 1 (Clip 0, Bild 0, Blend 32 @0x80100b04-18/@0x80100c04/@0x80100c34-40): "
          "motion=%d af=%u blend=%u", (int)e->motion, (unsigned)e->anim_frame, (unsigned)e->anim_frac);
    CHECK(re15_gr2_hysterese_ruht(slot, e), "Hysterese ruht nicht bei Stufe 1 (@0x80100b20-30)");
    CHECK(!re15_gr2_biss_erlaubt(slot), "Biss erlaubt bei Stufe 1 (FUN_801012fc @0x80101360-70)");
    /* Stufen 1..6: Schwimmen ohne Hysterese, Leon direkt vor der Schnauze -> kein Biss */
    int n16 = 0, clip0 = 0, biss = 0;
    for (int f = 0; f < 80 && re15_gr2_kanister_stufe() != 7; f++) {
        int st = re15_gr2_kanister_stufe();
        /* Stufe 2..4: Leon direkt vor der Schnauze (Maulpunkt ~ -22267); Stufe 5/6 (14 Bilder > 10 Bilder Abbau des
         * Schnauzenkontakts g->maul_kontakt im Boss-Modul) wieder fern, damit der in Stufe 7 wieder erlaubte Biss
         * (unten eigens geprueft) die Schwimm-Messung danach nicht ueberdeckt */
        if (st >= 2 && st <= 4) leon_statue(-6000, -21600); else leon_statue(-6000, -18200);
        e->x = -6000; e->z = -25000; e->rot_y = 0xC00;
        tick();
        if (re15_gr2_kanister_stufe() == 7) break;
        n16++;
        if (e->motion == 0) clip0++;
        if (e->motion == 4 || e->motion == 2) biss++;
        CHECK(re15_gr2_hysterese_ruht(slot, e), "Hysterese laeuft in Stufe %d (@0x80100b20-30)", re15_gr2_kanister_stufe());
    }
    CHECK(re15_gr2_kanister_stufe() == 7, "Kanister nicht getragen (Stufe %d)", re15_gr2_kanister_stufe());
    /* nach dem Fenster-Takt: Stufe 2 t = 2..12 (11), Stufe 3 (6), 4 (6), 5 (7), 6 (7) = 37 Takte (NB3: Stufe 1 + t = 0 im
     * Fenster-Takt, @0x8010183c-5c -> @0x80101860) */
    CHECK(n16 == 37 && clip0 == n16, "Stufe 1..6: %d/%d Bilder Clip 0 (Schwimmen, soll 37/37) - RE2 ohne Hysterese", clip0, n16);
    CHECK(biss == 0, "Biss waehrend der Aufnahme (%d Bilder Clip 4/2) - T1 nur bei Stufe 0/7 @0x80101360-70", biss);
    /* Stufe 7 in der Kanister-Zone (Zone 0): Sub bleibt Schwimmen (Bank 5 Bit 0 @0x80100ab8-c8 -> 0x80100c08) */
    int halt0 = 0;
    for (int f = 0; f < 30; f++) {
        leon_statue(-6000, -18200); e->x = -6000; e->z = -25000; e->rot_y = 0xC00;
        tick();
        if (e->motion == 0) halt0++;
    }
    CHECK(halt0 == 30, "Stufe 7 in der Kanister-Zone: nur %d/30 Bilder Clip 0 - RE2-Zone 1 + Bit 0 laesst keine "
          "Hysterese zu (@0x80100aec/@0x80100afc)", halt0);
    /* ausserhalb der Kanister-Zone (Westkanal-Streifen, Zone 2; Leon im Nordbecken = Zone 1 -> kein Dash):
     * Hysterese wieder an (Stufe 7 @0x80100b20-30) -> Paddeln */
    int pad2 = 0;
    for (int f = 0; f < 12; f++) {
        leon_statue(-6000, -8000); e->x = -6000; e->z = -16000; e->rot_y = 0xC00;
        tick();
        if (e->motion == 1) pad2++;
    }
    CHECK(pad2 >= 10, "ausserhalb der Kanister-Zone kein Paddeln bei Stufe 7 (%d/12) - Hysterese @0x80100b20", pad2);
    CHECK(re15_gr2_biss_erlaubt(slot), "Biss bei Stufe 7 nicht erlaubt (FUN_801012fc laesst 7 durch @0x8010136c-70)");
    /* Stufe 7 + Leon vor der Schnauze: der Biss startet wieder (RE2 T1 bei Stufe 7 scharf). Erst zurueck in Zone 0 mit
     * Leon noch im Nordbecken (Zonenwechsel 2 -> 0 nicht in Leons Zone = kein Dash), dann Leon vor die Schnauze. */
    leon_statue(-6000, -8000); e->x = -6000; e->z = -25000; e->rot_y = 0xC00; tick();
    int biss7 = -1;
    for (int f = 0; f < 10 && biss7 < 0; f++) {
        leon_statue(-6000, -21600); e->x = -6000; e->z = -25000; e->rot_y = 0xC00;
        tick();
        if (e->motion == 4 || e->motion == 2) biss7 = f;
    }
    CHECK(biss7 >= 0, "Stufe 7: kein Biss-Start mit Leon vor der Schnauze (T1 bei Stufe 7 @0x8010136c-70)");
    printf("aufnahme: Paddeln %d/12 -> Stufe 1 Clip 0 Blend 32, Stufe 1..6 %d/%d Clip 0, Biss %d, Stufe 7 Zone 0 %d/30 "
           "Clip 0, Zone 2 Paddeln %d/12, Biss Stufe 7 nach %d Takten\n", paddeln, clip0, n16, biss, halt0, pad2, biss7);
    return fails ? 1 : 0;
}

/* ================================================================================================ */
/* Nachbesserung 3 (Abnahme 2 M1, Orchestrator-Entscheid B9): UNTERGETAUCHT bis zum Durchbruch wie RE2. RE2: Sce_em_set
 * `44 00 00 23 00 00 01 16 00 00 4a 16 b8 0b 3c eb ...` @RDT 0x25A0 (ROOM40A0) = y +3000 unter dem Boden, Sub 0 P0
 * @0x80100854-68 ohne FUN_8001a330, Pre leer (@0x8010463c[0] = 0x801007fc `jr ra`); Pos_set @SCD 0x0736 erst mit dem
 * Intro (sub11 Member_set2 @0x0730 -> P1 Clip 6 @0x801008ac-b8). Natuerlicher Zugang wie Abnahme-2-Lauf N: Leon geht aus
 * der Tuernische im Westkanal nach Norden, Aggro bei dist < 9000 zum Lauerplatz (Nutzer-Design GB_AGGRO). */
extern int re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz);   /* re15_damage.c (FUN_8003703c) */
static int32_t max_schritt(int clip, int32_t s)
{
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    const re15_emd_clip_t *c = &bk->anim.clips[clip];
    int32_t m = 0;
    for (int f = 1; f < c->frame_count; f++) {
        int16_t a[3] = {0,0,0}, b[3] = {0,0,0};
        re15_emd_get_keyframe_speed(&bk->skel, (int)(bk->anim.frames[c->first_frame + f] & 0xfffu), &b[0], &b[1], &b[2]);
        re15_emd_get_keyframe_speed(&bk->skel, (int)(bk->anim.frames[c->first_frame + f - 1] & 0xfffu), &a[0], &a[1], &a[2]);
        double dx = (double)(b[0] - a[0]) * s / 4096.0, dz = (double)(b[2] - a[2]) * s / 4096.0;
        int32_t d = (int32_t)(sqrt(dx * dx + dz * dz) + 0.999);
        if (d > m) m = d;
    }
    return m;
}
static int teil_lauer(void)
{
    if (!setup(0)) return 1;                    /* kein GB_TEST: natuerliches Lauern */
    CHECK(re15_gator_boss_untergetaucht(e) == 1, "Spawn-Bild (state 0, vor dem Boss-INIT) nicht untergetaucht");
    leon_statue(-8200, -25550);                 /* Tuer-A-Spawn in der Nische (16 600 vom Lauerplatz) */
    tick();                                     /* INIT -> LURK */
    int32_t box_soll = (int32_t)((2200u * 2731u) >> 12);   /* byte-true Box @0x80118b98 x GB_SCALE_Q12 */
    CHECK(re15_gator_boss_untergetaucht(e) == 1 && e->hit_radius_min == 0 && e->hit_radius_max == 0,
          "nach dem INIT nicht untergetaucht (u %d, Box %d/%d)", re15_gator_boss_untergetaucht(e), (int)e->hit_radius_min,
          (int)e->hit_radius_max);
    int32_t lx = e->x, lz = e->z; int lyaw = (int)e->rot_y & 0xfff;
    uint16_t af0 = e->anim_frame; uint8_t mo0 = e->motion;
    int32_t tx = 0, tz = 0;
    int ziel = re15_player_aim_target(30000, &tx, &tz);
    CHECK(ziel == 0, "Zielhilfe (FUN_8003703c, Radius 30000) waehlt den untergetauchten Gator (%d,%d)", tx, tz);
    /* Leon geht im Westkanal (x -8440) nach Norden, 60 je Bild; je Takt: Lage, Sichtbarkeit, Box */
    int f_auf = -1, unter_n = 0, sprung_max = 0, f_sprung = -1, schritte = 0;
    int32_t ax = 0, az = 0, vx = 0, vz = 0;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    int32_t m6 = max_schritt(6, s), m3 = max_schritt(3, s), m0 = max_schritt(0, s), m1 = max_schritt(1, s);
    int32_t grenze = m6; if (m3 > grenze) grenze = m3; if (m0 > grenze) grenze = m0; if (m1 > grenze) grenze = m1;
    int32_t z = -25550;
    for (int f = 0; f < 700; f++) {
        if (z < -5900) z += 60;
        leon_statue(-8440, z);
        int32_t ox = e->x, oz = e->z;
        tick();
        int u = re15_gator_boss_untergetaucht(e);
        if (u) {
            unter_n++;
            CHECK(e->x == lx && e->z == lz && ((int)e->rot_y & 0xfff) == lyaw && e->motion == mo0 && e->anim_frame == af0,
                  "untergetaucht bewegt/animiert (Takt %d: (%d,%d) yaw %d mo %d af %u)", f, e->x, e->z, (int)e->rot_y & 0xfff,
                  (int)e->motion, (unsigned)e->anim_frame);
            CHECK(e->hit_radius_min == 0, "untergetaucht mit Trefferbox %d (Takt %d)", (int)e->hit_radius_min, f);
            continue;
        }
        if (f_auf < 0) {
            f_auf = f; ax = e->x; az = e->z;
            CHECK(re15_gr2_im_intro(slot) && e->motion == 6 && e->anim_frame == 0,
                  "Auftauchen nicht im Intro-Start-Takt (im_intro %d, Clip %d Bild %u) - RE2 P1 Clip 6 @0x801008ac-b8",
                  re15_gr2_im_intro(slot), (int)e->motion, (unsigned)e->anim_frame);
            CHECK(e->hit_radius_min == box_soll, "Trefferbox nach dem Auftauchen %d statt %d", (int)e->hit_radius_min, (int)box_soll);
            vx = ox; vz = oz;                   /* letzte untergetauchte Lage (Lauerplatz) */
            continue;
        }
        if (!re15_gr2_im_intro(slot)) break;    /* nur der Intro (Clip 6 + Clip 3), danach Verfolgung */
        int64_t dx = (int64_t)e->x - ox, dz = (int64_t)e->z - oz;
        int32_t d = (int32_t)(sqrt((double)(dx * dx + dz * dz)) + 0.5);
        schritte++;
        if (d > sprung_max) { sprung_max = d; f_sprung = f; }
    }
    CHECK(f_auf > 0, "kein Durchbruch im natuerlichen Zugang (Aggro dist < 9000)");
    {   int64_t dx = (int64_t)ax - vx, dz = (int64_t)az - vz;
        printf("lauer: untergetaucht %d Takte am Lauerplatz (%d,%d), Auftauchen Takt %d auf der Intro-Lage (%d,%d) - Pos_set-"
               "Sprung %d im untergetauchten Takt; Intro %d Schritte, groesster %d (Takt %d), Grenze %d (authored Clip 6/3/0/1 "
               "%d/%d/%d/%d x Skala)\n", unter_n, lx, lz, f_auf, ax, az, (int)sqrt((double)(dx * dx + dz * dz)), schritte,
               sprung_max, f_sprung, grenze, m6, m3, m0, m1); }
    CHECK(schritte >= 250 && sprung_max <= grenze, "sichtbarer Lagesprung im Intro: %d > groesster authored Schritt %d (Takt %d, "
          "%d Schritte)", sprung_max, grenze, f_sprung, schritte);
    return fails ? 1 : 0;
}

/* Nachbesserung 3 (Abnahme 2 M2): Kanister-Aufnahme ohne Koerperlenkung. RE2 hat in Sub 1/2 und T2 keinen Gier-Schreiber
 * (`sh ...,118(...)` nur @0x80100280/2a0/948/ec0/2404/2440/27c8/2a34); Stufe 2 schiebt den Kanister nur laengs
 * (`obj[1].z = z + 8100` @0x801018a4-c0) bis zum Anhaengen (t = 13 @0x801018c4-cc). Lage wie die Pruefer-Sonde 'schleppen'
 * der Abnahme 2: Gator (-3000,-24000) Blick Ost, Kanister im Fenster, Leon im Norden (die Verfolgung wuerde nach Norden
 * drehen). Fall 2 schiebt den Gator zusaetzlich 300 je Takt laengs der Gier (schneller als jeder Clip), damit der Stufe-2-
 * Schub selbst greift: er muss auf der Blickachse bleiben (quer unveraendert). */
static int lauf_schleppen(int schub, int *ok_dreh, int32_t *kweg, int32_t *gweg, int *takte, int32_t *quer_d)
{
    if (!setup(1)) return 0;
    leon_statue(-3000, -19000);
    tick();
    intro_abwarten();
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    e->x = -3000; e->z = -24000; e->y = -1200; e->rot_y = 0;
    int32_t laengs = (int32_t)(((int64_t)8800 * s) >> 12);
    re15_gr2_kanister_setzen(-3000 + laengs, -1200, -24000);
    int32_t kp[3], kr[9], kalt[3]; re15_gr2_kanister_welt(kalt, kr);
    int32_t q0 = kalt[2] - e->z;                /* Gier 0 = Ost: quer = z-Abstand */
    int fenster = -1, anh = -1; int16_t yaw_f = 0;
    *kweg = 0; *gweg = 0; *ok_dreh = 1; *quer_d = 0;
    for (int f = 0; f < 60; f++) {
        leon_statue(-3000, -19000);
        int st_vor = re15_gr2_kanister_stufe();
        int32_t gx = e->x, gz = e->z;
        if (schub && st_vor == 2) {             /* Fall 2: schneller voran als jeder Clip (Schub muss greifen) */
            e->x += (int32_t)(((int64_t)re15_cos_q12((int)e->rot_y) * 300) >> 12);
            e->z -= (int32_t)(((int64_t)re15_sin_q12((int)e->rot_y) * 300) >> 12);
        }
        tick();
        int st = re15_gr2_kanister_stufe();
        if (fenster < 0 && st > 0) {
            fenster = f; yaw_f = e->rot_y;
            CHECK(st == 2, "Fenster-Takt endet in Stufe %d statt 2 (Stufe 1 faellt in Stufe 2 @0x8010183c-5c -> @0x80101860)", st);
            re15_gr2_kanister_welt(kalt, kr);
            continue;
        }
        if (fenster < 0) continue;
        if (st == 2) {                          /* bis zum Anhaengen: keine Lenkung, Wege messen */
            if (e->rot_y != yaw_f) *ok_dreh = 0;
            CHECK(re15_gr2_aufnahme_lenksperre(slot), "Lenksperre aus in Stufe 2 vor dem Anhaengen (Takt %d)", f);
            re15_gr2_kanister_welt(kp, kr);
            int64_t kx = (int64_t)kp[0] - kalt[0], kz = (int64_t)kp[2] - kalt[2];
            int64_t dgx = (int64_t)e->x - gx, dgz = (int64_t)e->z - gz;
            *kweg += (int32_t)(sqrt((double)(kx * kx + kz * kz)) + 0.5);
            *gweg += (int32_t)(sqrt((double)(dgx * dgx + dgz * dgz)) + 0.5);
            int32_t qd = (kp[2] - e->z) - q0; if (qd < 0) qd = -qd; if (qd > *quer_d) *quer_d = qd;
            memcpy(kalt, kp, sizeof kalt);
            continue;
        }
        if (st >= 3 && anh < 0) { anh = f; break; }
    }
    *takte = (fenster >= 0 && anh >= 0) ? anh - fenster : -1;
    CHECK(fenster >= 0 && anh > fenster, "Fenster %d / Anhaengen %d nicht erreicht", fenster, anh);
    CHECK(!re15_gr2_aufnahme_lenksperre(slot), "Lenksperre nach dem Anhaengen noch an");
    /* danach lenkt die Verfolgung wieder (Leon im Norden, Gier 0 = Ost) */
    int dreht = 0;
    for (int f = 0; f < 40 && !dreht; f++) { int16_t y0 = e->rot_y; leon_statue(-3000, -19000); tick(); if (e->rot_y != y0) dreht = 1; }
    CHECK(dreht, "nach dem Anhaengen keine Lenkung mehr (Verfolgung tot)");
    return 1;
}
static int teil_schleppen(void)
{
    int ok_dreh = 0, takte = 0; int32_t kweg = 0, gweg = 0, quer_d = 0;
    if (!lauf_schleppen(0, &ok_dreh, &kweg, &gweg, &takte, &quer_d)) return 1;
    CHECK(ok_dreh, "Gier aendert sich zwischen Fenster und Anhaengen (RE2 ohne Gier-Schreiber in Sub 1/2/T2)");
    CHECK(kweg <= gweg && gweg > 0, "Fall 1: Kanister-Weg %d > Gator-Weg %d in Stufe 2 (@0x801018a4-c0)", kweg, gweg);
    CHECK(takte == 12, "Fall 1: Anhaengen %d Takte nach dem Fenster-Takt statt 12 (t = 1..12 -> 13 @0x801018c4-cc)", takte);
    printf("schleppen: Fall 1 (Clip 0): Gier fest %d, Gator-Weg %d, Kanister-Weg %d, Anhaengen %d Takte nach dem Fenster\n",
           ok_dreh, gweg, kweg, takte);
    int ok2 = 0, t2 = 0; int32_t kw2 = 0, gw2 = 0, q2 = 0;
    if (!lauf_schleppen(1, &ok2, &kw2, &gw2, &t2, &q2)) return 1;
    CHECK(ok2, "Fall 2: Gier aendert sich vor dem Anhaengen");
    CHECK(kw2 > 0 && kw2 <= gw2, "Fall 2: Schub greift nicht oder Kanister-Weg %d > Gator-Weg %d (@0x801018a4-c0)", kw2, gw2);
    CHECK(q2 <= 2, "Fall 2: Kanister verlaesst die Blickachse (quer %d) - RE2 schiebt nur laengs (x unberuehrt)", q2);
    printf("schleppen: Fall 2 (+300 je Takt): Gier fest %d, Gator-Weg %d, Kanister-Weg %d (laengs, quer-Abweichung %d)\n",
           ok2, gw2, kw2, q2);
    /* Fall 3 - Kanister SEITLICH im Fenster (quer 1200 < Koerperradius 1466 + 267): RE2 fuehrt nach dem Anhaengen nur x/y heran
     * (Stufen 3-7 @0x801019b4-0x80101b8c), im Korridor liegt der Kanister 262 neben der Achse (sub05 Pos_set x -22816 @SCD 0x0484
     * gegen Bein 2 x -23078 @0x80101048); der Port-Quer-Ausgleich fuehrt z mit den x-Schritten (70/145/100/100) auf 0 - am Ende
     * liegt der Kanister wie in RE2 auf der Kopfachse (1750, -210, 0). */
    {
        if (!setup(1)) return 1;
        leon_statue(-3000, -19000); tick(); intro_abwarten();
        int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
        e->x = -3000; e->z = -24000; e->y = -1200; e->rot_y = 0;
        re15_gr2_kanister_setzen(-3000 + (int32_t)(((int64_t)8800 * s) >> 12), -1200, -24000 + 1200);
        int32_t l_anh[3] = {0, 0, 0}, l[3] = {0, 0, 0}; int anh = 0, nach7 = 0;
        for (int f = 0; f < 200 && nach7 < 40; f++) {
            leon_statue(-3000, -19000);
            tick();
            if (!anh && re15_gr2_kanister_lokal(l_anh)) anh = 1;
            if (re15_gr2_kanister_stufe() == 7) nach7++;
        }
        int ok = re15_gr2_kanister_lokal(l);
        /* Nachbesserung 4 (M1): die Lokallage beim Anhaengen bezieht sich auf die GEZEICHNETE Part-6-Matrix (FUN_8002d030
         * @0x801018f0 gegen die Part-6-Weltmatrix @0x801018d0, samt Kopf-az +0x22A @0x8010037c-90 = nach Stufe 2 ~+204): die
         * Querlage ist darin kleiner als in der Keyframe-Matrix ohne +0x22A (vorher > 1000), aber nicht 0 - der Quer-Ausgleich
         * hat weiter etwas zu tun. */
        CHECK(anh && l_anh[2] != 0, "Fall 3: beim Anhaengen keine Querlage (lokal z %d)", l_anh[2]);
        CHECK(ok && l[0] == RE15_GR2_KAN_LOKAL_X && l[1] == RE15_GR2_KAN_LOKAL_Y && l[2] == 0,
              "Fall 3: Lokallage nach Stufe 7 + 40 Takte (%d,%d,%d) statt (1750,-210,0) - x/y @0x801019c4-0x80101b8c, z Quer-Ausgleich",
              l[0], l[1], l[2]);
        printf("schleppen: Fall 3 (quer 1200): Lokallage beim Anhaengen (%d,%d,%d) -> nach Stufe 7 + 40 Takte (%d,%d,%d)\n",
               l_anh[0], l_anh[1], l_anh[2], l[0], l[1], l[2]);
    }
    return fails ? 1 : 0;
}

/* Nachbesserung 3 (Abnahme 2 M3): RE2-Anschwimm-Sperre. Gator (-6000,-25000) Blick Nord (Zone 0), Leon 2000 voraus (er
 * wuerde sofort beissen), Kanister liegend voraus: >= 8880 x Skala (und ausserhalb des Fensters 8601..8999) -> Zielmodus
 * +0x1C0 & 4 (@0x80100a34-5c) -> kein T1 (@0x8010134c-58); < 8880 (@0x80100a74-b4), hinter dem Gator, in einer anderen Zone
 * (PORT-WAHL) oder ohne Kanister -> Biss. Lage wie die Pruefer-Sonde 'anschwimmen' der Abnahme 2. */
static int lauf_ansch(int32_t gz, int32_t kan_x, int32_t kan_z, int mit, int *bit)
{
    if (!setup(1)) return -2;
    leon_statue(-6000, -18200);
    tick();
    intro_abwarten();
    if (mit) re15_gr2_kanister_setzen(kan_x, -1200, kan_z);
    int biss = -1; *bit = -1;
    for (int f = 0; f < 40 && biss < 0; f++) {
        leon_statue(-6000, gz + 2000);
        e->x = -6000; e->z = gz; e->y = -1200; e->rot_y = 0xC00;
        tick();
        if (f == 0) *bit = re15_gr2_zielmodus(slot);
        if (e->motion == 4 || e->motion == 2) biss = f;
    }
    return biss;
}
static int teil_anschwimmen(void)
{
    int32_t s = 2731;                           /* GB_SCALE_Q12 (Boss-INIT) */
    /* Zone 0 (Suedbecken) = z <= -18100 (gb_zone, GB_RAMP_Z0): mit dem Gator bei -25000 reicht sie 6900 voraus (RE2 10349),
     * bei -26500 8400 (RE2 12594). 14000 RE2 voraus (z -15666) laege im Westkanal-Streifen (Zone 2) = andere Zone. */
    struct { int32_t gz; int32_t re2; int mit; int sperre; const char *was; } k[] = {
        { -25000, 10000, 1, 1, "Kanister 10000 voraus (>= 8880), gleiche Zone" },
        { -26500, 12000, 1, 1, "Kanister 12000 voraus (>= 8880), gleiche Zone" },
        { -25000,  8000, 1, 0, "Kanister 8000 voraus (< 8880, vor dem Fenster 8601)" },
        { -25000, -1500, 1, 0, "Kanister hinter dem Gator" },
        { -25000, 14000, 1, 0, "Kanister 14000 voraus im Westkanal (andere Zone)" },
        { -25000, 10000, 0, 0, "ohne Kanister" },
    };
    int n = (int)(sizeof k / sizeof k[0]);
    for (int i = 0; i < n; i++) {
        int32_t kz = k[i].gz + (int32_t)(((int64_t)k[i].re2 * s) >> 12);
        int bit = -1;
        int b = lauf_ansch(k[i].gz, -6000, kz, k[i].mit, &bit);
        printf("anschwimmen: %-52s laengs %6d: Zielmodus %d, Biss-Start %s%d\n", k[i].was, (int)(kz - k[i].gz), bit,
               b < 0 ? "nie (40 Takte) " : "nach ", b < 0 ? 40 : b);
        if (k[i].sperre) CHECK(b < 0 && bit == 1, "%s: Biss nach %d Takten / Zielmodus %d (RE2 +0x1C0 & 4 @0x80100a34-5c sperrt T1 "
                               "@0x8010134c-58)", k[i].was, b, bit);
        else             CHECK(b >= 0 && bit == 0, "%s: kein Biss (%d) / Zielmodus %d", k[i].was, b, bit);
    }
    return fails ? 1 : 0;
}

/* ===== Nachbesserung 4 (Abnahme 3) =============================================================================================
 * Zeichner-Nachbau wie der NPC-Pass in main.c (nach dem Takt): clip_override = (anim_flags & 4) ? motion : -1
 * (anim_select_common.c, Gegner-Zweig), re15_compute_actor_kf(.., anim_frame), re15_skel_compute_pose mit g_anim_pose_actor =
 * Aktor (Ueberblendung + Kopf/Kiefer-az +0x22A/+0x228 ueber re15_gator_spine_arc_vz; schreibt den Ueberblend-Schnappschuss wie
 * der echte Zeichner). Part-6-Weltmatrix: Rotation = Yaw x Part (ohne Skala), Lage = Wurzel + Yaw x Skala x Part (main.c nyaw x
 * render_scale_q12). */
static int zeichnen_nachbau(int32_t r6[9], int32_t p6[3])
{
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    if (!bk) return 0;
    int ov = (e->anim_flags & 0x04u) ? (int)e->motion : -1;
    int kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, ov, (uint32_t)e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = e;
    int rv = re15_skel_compute_pose(&bk->skel, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    int32_t tr[3] = { (int32_t)(((int64_t)poses[6].trans[0] * s) >> 12), (int32_t)(((int64_t)poses[6].trans[1] * s) >> 12),
                      (int32_t)(((int64_t)poses[6].trans[2] * s) >> 12) };
    re15_skel_bone_to_world(tr, e->rot_y, e->x, e->y, e->z, p6);
    int32_t ry[9]; re15_skel_euler_matrix(0, (int)e->rot_y, 0, ry);
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            r6[r*3 + c] = (int32_t)(((int64_t)ry[r*3+0] * poses[6].rot[0*3+c] + (int64_t)ry[r*3+1] * poses[6].rot[1*3+c]
                                    + (int64_t)ry[r*3+2] * poses[6].rot[2*3+c]) >> 12);
    return 1;
}
/* |gezeichnete Kanister-Lage - Part6(Bild) x Skala x Lokallage| (groesste Komponente); *fx_ok = Effekt-Quelle Part 6 = dieselbe
 * Matrix (32-Byte-Abbild: 9 x s16 Rotation, 3 x s32 Lage ab Byte 20). -1 = nicht angehaengt. */
static int32_t sitz_fehler(const int32_t r6[9], const int32_t p6[3], int *fx_ok)
{
    int32_t kw[3], kr[9], l[3];
    if (!re15_gr2_kanister_welt(kw, kr) || !re15_gr2_kanister_lokal(l)) return -1;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    int32_t ls[3] = { (int32_t)(((int64_t)l[0] * s) >> 12), (int32_t)(((int64_t)l[1] * s) >> 12), (int32_t)(((int64_t)l[2] * s) >> 12) };
    int32_t m = 0;
    for (int r = 0; r < 3; r++) {
        int32_t soll = p6[r] + (int32_t)(((int64_t)r6[r*3] * ls[0] + (int64_t)r6[r*3+1] * ls[1] + (int64_t)r6[r*3+2] * ls[2]) >> 12);
        int32_t d = soll - kw[r]; if (d < 0) d = -d; if (d > m) m = d;
    }
    if (fx_ok) {
        uint8_t q[32], soll[32]; memset(soll, 0, sizeof soll);
        for (int i = 0; i < 9; i++) { int16_t v = (int16_t)r6[i]; soll[i*2] = (uint8_t)v; soll[i*2+1] = (uint8_t)(v >> 8); }
        for (int i = 0; i < 3; i++) { uint32_t v = (uint32_t)p6[i];
            soll[20+i*4] = (uint8_t)v; soll[21+i*4] = (uint8_t)(v >> 8); soll[22+i*4] = (uint8_t)(v >> 16); soll[23+i*4] = (uint8_t)(v >> 24); }
        *fx_ok = re15_gr2_fx_quelle(0, q) && memcmp(q, soll, 32) == 0;
    }
    return m;
}
/* sitz - Abnahme 3 M1: der getragene Kanister sitzt in JEDEM gezeichneten Bild an Part 6 x Lokallage. RE2 haengt ihn als Kind an
 * die Part-6-Weltmatrix (`addiu a0,s0,1104` @0x801018d0, `sw a0,128(s2)` @0x801018e0; Kopf-az +0x22A in die lokale Part-6-Matrix
 * @0x8010037c-90), der Port rechnete die Lage am Takt-Anfang aus der Pose des Vorbilds (Pruefer-Sonde 'nachlauf', gleiche Lage:
 * Wende 619-637 je Bild, Biss bis 409, > 300 in 51 von 240 Bildern). Gemessen wird nach jedem Takt gegen den Zeichner-Nachbau,
 * (a) Aufnahme bis Stufe 7 + 40, (b) 240 Takte Wende (Leon seitlich) / Biss (Leon vor der Schnauze) / Wende zurueck wie die
 * Sonde, (c) Befreiung aus einem Gefaengnis im Plattform-Slab (Schub, Gier fest) mit dem Kanister im Maul. */
static int teil_sitz(void)
{
    if (!setup(1)) return 1;
    leon_statue(-3000, -19000); tick(); intro_abwarten();
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    e->x = -3000; e->z = -24000; e->y = -1200; e->rot_y = 0;
    re15_gr2_kanister_setzen(-3000 + (int32_t)(((int64_t)8800 * s) >> 12), -1200, -24000);
    int32_t r6[9], p6[3]; int fx_ok = 0, fx_falsch = 0;
    int f7 = 0, n_auf = 0; int32_t m_auf = 0;
    for (int f = 0; f < 200 && f7 < 40; f++) {
        leon_statue(-3000, -19000); tick();
        if (!zeichnen_nachbau(r6, p6)) { CHECK(0, "Zeichner-Nachbau ohne Pose"); return 1; }
        int32_t d = sitz_fehler(r6, p6, &fx_ok);
        if (d >= 0) { n_auf++; if (d > m_auf) m_auf = d; if (!fx_ok) fx_falsch++; }
        if (re15_gr2_kanister_stufe() == 7) f7++;
    }
    CHECK(f7 >= 40, "Stufe 7 nicht erreicht (%d Takte)", f7);
    CHECK(m_auf == 0, "Aufnahme: Kanister %d neben Part6(Bild) x lokal (%d angehaengte Bilder)", m_auf, n_auf);
    printf("sitz: (a) Aufnahme Stufe 3..7 + 40: %d angehaengte Bilder, groesster Abstand zu Part6(Bild) x lokal %d\n", n_auf, m_auf);
    int n_wende = 0, n_biss = 0, n_blend = 0, n = 0; int32_t m_wende = 0, m_biss = 0, m = 0;
    for (int f = 0; f < 240; f++) {
        if (f < 80)       leon_statue(e->x - 6000, e->z + 6000);
        else if (f < 160) leon_statue(e->x + (int32_t)(((int64_t)re15_cos_q12((int)e->rot_y) * 2600) >> 12),
                                      e->z - (int32_t)(((int64_t)re15_sin_q12((int)e->rot_y) * 2600) >> 12));
        else              leon_statue(e->x + 6000, e->z + 6000);
        pl->hp = 100;                           /* kein Fress-Finisher: Wende und Biss messen (wie die Sonde) */
        int16_t y0 = e->rot_y;
        tick();
        if (!zeichnen_nachbau(r6, p6)) { CHECK(0, "Zeichner-Nachbau ohne Pose (Takt %d)", f); return 1; }
        int32_t d = sitz_fehler(r6, p6, &fx_ok);
        if (d < 0) { CHECK(0, "Kanister nicht mehr angehaengt (Takt %d, Stufe %d)", f, re15_gr2_kanister_stufe()); return 1; }
        n++; if (d > m) m = d; if (!fx_ok) fx_falsch++;
        if (e->motion == 0 && e->rot_y != y0) { n_wende++; if (d > m_wende) m_wende = d; }
        if (e->motion == 4) { n_biss++; if (d > m_biss) m_biss = d; }
        if (e->anim_frac > 0) n_blend++;
    }
    CHECK(n_wende >= 20 && n_biss >= 20, "Faelle nicht abgedeckt: Wende %d / Biss %d Bilder (je >= 20)", n_wende, n_biss);
    CHECK(m == 0, "Wende/Biss: Kanister %d neben Part6(Bild) x lokal (Wende max %d, Biss max %d)", m, m_wende, m_biss);
    printf("sitz: (b) %d Bilder, Wende %d (max %d), Biss %d (max %d), mit Ueberblendung %d, groesster Abstand %d\n",
           n, n_wende, m_wende, n_biss, m_biss, n_blend, m);
    /* (c) Gefaengnis: Zentrum 600 innerhalb der um den Koerperradius aufgeblasenen Plattform-Westkante (x -1700 - 1466), Leon im
     *     Westen - die Verfolgung will hinaus, push_rect Fall 0 verwirft den Schritt (re15_collision.c) -> Befreiung (Schub). */
    int b0 = re15_gator_boss_befreiungen(), n_schub = 0, gestartet = 0; int32_t m_schub = 0; int gier_fest = 1;
    const int32_t ausgang = -1700 - ((int32_t)e->hit_radius_min + 60) - 120;   /* Slab-Ausgang bx (enemy_ai_boss_gator.c) */
    e->x = -1700 - (int32_t)e->hit_radius_min + 600; e->z = -16000; e->rot_y = 0x800;
    for (int f = 0; f < 80; f++) {
        leon_statue(-7000, -16000); pl->hp = 100;
        int16_t y0 = e->rot_y; int32_t x0 = e->x;
        tick();
        if (!zeichnen_nachbau(r6, p6)) { CHECK(0, "Zeichner-Nachbau ohne Pose (Befreiung %d)", f); return 1; }
        int32_t d = sitz_fehler(r6, p6, &fx_ok);
        if (d < 0) { CHECK(0, "Kanister bei der Befreiung nicht angehaengt"); return 1; }
        if (!fx_ok) fx_falsch++;
        if (!gestartet && re15_gator_boss_befreiungen() > b0) gestartet = 1;
        if (gestartet && x0 > ausgang) {        /* Schub-Bild: Start bis zum Erreichen des Ausgangs */
            n_schub++;
            if (e->rot_y != y0) gier_fest = 0;
            if (d > m_schub) m_schub = d;
        }
        if (gestartet && e->x <= ausgang) break;
    }
    CHECK(re15_gator_boss_befreiungen() == b0 + 1, "Gefaengnis: %d Befreiungen statt 1", re15_gator_boss_befreiungen() - b0);
    CHECK(n_schub >= 1 && gier_fest, "Befreiung: %d Schub-Bilder, Gier fest %d (NB4 M2: Schub ohne Drehung)", n_schub, gier_fest);
    CHECK(m_schub == 0, "Befreiung: Kanister %d neben Part6(Bild) x lokal", m_schub);
    printf("sitz: (c) Befreiung %d Schub-Bilder, Gier fest %d, groesster Abstand %d\n", n_schub, gier_fest, m_schub);
    /* (d) mit UEBERBLENDUNG: der tragende Gator wechselt ins Nordbecken, in dem Leon ist -> RE2-Zonen-Dash mit Clip 2 und
     *     Blend 16 (@0x80100dac-b4); RE2-Gewicht 4096/(n+1) @0x8001a37c-ac, Abbau -1 je Bild @0x800299c0-cc - 16 Bilder, in
     *     denen die gezeichnete Pose eine Mischung aus Vorbild und Keyframe ist. */
    e->x = 3000; e->z = -9000; e->rot_y = 0x800;
    int n_bl = 0, frac_max = 0; int32_t m_bl = 0;
    for (int f = 0; f < 70; f++) {
        leon_statue(-6000, -7000); pl->hp = 100;
        tick();
        if (!zeichnen_nachbau(r6, p6)) { CHECK(0, "Zeichner-Nachbau ohne Pose (Ueberblendung %d)", f); return 1; }
        int32_t d = sitz_fehler(r6, p6, &fx_ok);
        if (d < 0) { CHECK(0, "Kanister bei der Ueberblendung nicht angehaengt"); return 1; }
        if (!fx_ok) fx_falsch++;
        if (e->anim_frac > 0) { n_bl++; if (d > m_bl) m_bl = d; if (e->anim_frac > frac_max) frac_max = e->anim_frac; }
    }
    CHECK(n_bl >= 15 && frac_max == 16, "Ueberblendung nicht abgedeckt: %d Bilder, groesster Zaehler %d (Dash-Blend 16 erwartet)", n_bl, frac_max);
    CHECK(m_bl == 0, "Ueberblendung: Kanister %d neben Part6(Bild) x lokal", m_bl);
    CHECK(fx_falsch == 0, "Effekt-Quelle Part 6 in %d Bildern nicht die gezeichnete Part-6-Matrix", fx_falsch);
    printf("sitz: (d) Ueberblendung %d Bilder (Zaehler bis %d), groesster Abstand %d; Effekt-Quelle Part 6 falsch in %d Bildern\n",
           n_bl, frac_max, m_bl, fx_falsch);
    return fails ? 1 : 0;
}

/* gier - Abnahme 3 M2: in Verfolgung (CHASE) und Belagerung (GUARD) dreht der Gator hoechstens mit der Port-Wenderate 0x60 je
 * Bild (re15_enemy_steer_point(.., 0x60), "zackige Wende"); einzige Ausnahme der RE2-Knick +1024 am Clip-3-Ende (@0x80100938-48 /
 * @0x80100eb0-c0). RE2 dreht den Koerper in Sub 1/2 nie (Gier-Schreiber nur @0x80100280/2a0/948/ec0/2404/2440/27c8/2a34). Lagen
 * der Abnahme 3 (je >= 300 Bildpaare mit beiden Takten in CHASE/GUARD):
 *   E1  Plattform-Belagerung: Leon OBEN auf der Rampe (6732,-16250, y -1800), GUARD-Ziel (6500,-12600) vor der Ostwand - vorher
 *       Befreiungs-Snap +-1944..1984 alle 32 Bilder an der Ostwand (x 5716 > 7200 - 1526).
 *   C3  Westwand vor der Tuernische: Leon in der Nische (-8200,-25550) - vorher 1300..1650 (C3 F1097/1207/1327, A2 F379, D2 F1075).
 * Beide sind Wand-DRUCK (der Schritt hinaus ist frei) -> keine Befreiung mehr. 800 Takte je Lage, Leon haelt seine Lage. */
static int lauf_gier(int32_t lx, int32_t lz, int32_t ly, int takte, int *paare, int *maxd, int *spr, int *bef, int *guard, int *wand)
{
    if (!setup(1)) return 0;
    leon_statue(lx, lz); tick(); intro_abwarten();
    int b0 = re15_gator_boss_befreiungen();
    int v_alt = re15_gator_boss_verfolgt(e); int16_t y_alt = e->rot_y;
    *paare = 0; *maxd = 0; *spr = 0; *guard = 0; *wand = 0;
    for (int f = 0; f < takte; f++) {
        leon_statue(lx, lz); pl->y = ly; pl->hp = 100;   /* Leon haelt die Lage (ohne Fress-Finisher) */
        tick();
        int v = re15_gator_boss_verfolgt(e);
        if (v && v_alt) {
            int d = (((int)e->rot_y - (int)y_alt + 0x800) & 0xfff) - 0x800, ad = d < 0 ? -d : d;
            (*paare)++;
            if (d != 1024 && ad > *maxd) *maxd = ad;
            if (d != 1024 && ad > 0x60) { (*spr)++; if (*spr <= 5) printf("  Sprung Takt %d: %d (Phase %d, (%d,%d))\n", f, d,
                                                                       re15_gator_boss_phase(e), e->x, e->z); }
        }
        if (re15_gator_boss_phase(e) == 5) (*guard)++;
        if (e->x > 7200 - 1466 - 100 || e->x < -8900 + 1466 + 100) (*wand)++;   /* an der Ost-/Westwand-Klemmlinie */
        v_alt = v; y_alt = e->rot_y;
    }
    *bef = re15_gator_boss_befreiungen() - b0;
    int gp = 0, gm = 0, gs = 0; re15_gator_boss_gier_wache(slot, &gp, &gm, &gs);
    CHECK(gp >= *paare && gs == *spr, "Messzeile GIER-WACHE (%d Paare, %d Spruenge) weicht vom Riegel ab (%d / %d)", gp, gs, *paare, *spr);
    return 1;
}
static int teil_gier(void)
{
    struct { const char *was; int32_t x, z, y; } k[2] = {
        { "E1 Plattform-Belagerung (Leon oben auf der Rampe)", 6732, -16250, -1800 },
        { "C3 Westwand vor der Tuernische (Leon in der Nische)", -8400, -26130, 0 },   /* C3 F1096: Leon (-8389,-26134) */
    };
    for (int i = 0; i < 2; i++) {
        int pa = 0, md = 0, sp = 0, bf = 0, gu = 0, wa = 0;
        if (!lauf_gier(k[i].x, k[i].z, k[i].y, 800, &pa, &md, &sp, &bf, &gu, &wa)) return 1;
        printf("gier: %-52s %d Bildpaare CHASE/GUARD (GUARD %d, an der Wand %d), max |Gier-Delta| %d (ohne +1024), Spruenge > 0x60 %d, "
               "Befreiungen %d\n", k[i].was, pa, gu, wa, md, sp, bf);
        CHECK(pa >= 300, "%s: nur %d Bildpaare CHASE/GUARD (>= 300 verlangt)", k[i].was, pa);
        CHECK(sp == 0 && md <= 0x60, "%s: %d Gier-Spruenge, max |Delta| %d > 0x60 (RE2 ohne Gier-Schreiber in Sub 1/2)", k[i].was, sp, md);
        CHECK(bf == 0, "%s: %d Befreiungen an einer Wand (Wand-Druck ist kein Gefaengnis)", k[i].was, bf);
        if (i == 0) CHECK(gu >= 300, "%s: nur %d GUARD-Bilder (Belagerung nicht erreicht)", k[i].was, gu);
        CHECK(wa >= 100, "%s: nur %d Bilder an der Wand-Klemmlinie (Lage nicht erreicht)", k[i].was, wa);
    }
    return fails ? 1 : 0;
}

/* ===== Nachbesserung 5 (Abnahme 4 M1) ============================================================================================
 * maul - nach JEDEM toedlichen Biss liegt das Opfer ueber P1-P3 im GEZEICHNETEN Maul wie im CHASE-Bezug (A2 / unit_gator_fress).
 * Maulraum-Messung von unit_gator_fress (Leons PL00-Koerper mit den Opfer-Keyframes und dem Fress-Massstab im AABB von Mesh 6+7,
 * gator-lokal), hier mit der GEZEICHNETEN Pose (g_anim_pose_actor = Gator: Bone-0-Neigung pitch_vz, arc/jaw-Haken, RE2-Kopf/Kiefer-az
 * +0x22A/+0x228) in JEDEM Bild ab P0, fuer den CHASE-Bezug (Leon im offenen Wasser, Biss frontal wie unit_gator_fress) und vier
 * GUARD-Hochbiss-Lagen mit Leon OBEN (y -1800): Rampe Ost (6300,-16250) = Ankunft der 20A0-Tuer, E1-Lage (6732,-16250), Rampenmitte
 * (4000,-15000), Plattformblock (0,-16250). Je Lage und Bild der Finisher-Bahn gegen den Bezug im selben Bild:
 *   - Leons Wurzel im gezeichneten Kopf-Rahmen (Bone 6) gleich (<= 32 Modelleinheiten je Achse = ~21 Welteinheiten bei Skala 2/3:
 *     Q12-Rundung der anderen Buehnen-Gier, gemessen bis 10; der Defekt vorher bis 5649),
 *   - Anteil von Leons Koerper im Maulraum gleich (<= 2 Prozentpunkte),
 *   - Abstand Leon <-> gezeichneter Kiefer gegen die authored Bahn (re15_gr2_fress_sitz, Messzeile der exe) <= 8,
 *   - Gator-Wurzel, Hals/Kopf/Kiefer (Bones 1/5/6/7) und Leons Wurzel im Wasser (Beckenquelle des Boss-Moduls),
 *   - Kopf im Bild der Finisher-Kamera, Leon im Bild oder oben hinaus (Wurf P2 Bild 60-75, in RE2 Cut 7 ebenso).
 * RE2: P0 setzt den Gator um (@0x80101048-4c) und nullt die Zusatzwinkel (@0x80101050-54), P1-P3 setzen Gator und Opfer vom
 * gemeinsamen Anker ohne Wand (FUN_80015cb8 @0x80101174/@0x801011cc). Vorher (Stand 7831a6d5, Sonde B41nb5/probe_maul):
 * GUARD-Lagen Kopf-lokal bis 5649 daneben, Klemm-Versatz bis 1129, Bilder mit >= 50 % im Maul P2/P3 0/0 bzw. 17/82. */
/* ---- Nachbesserung 6 (Abnahme 5 M3): unabhaengige Sicht-Pruefung gegen die Raum-Objekte ---------------------------------------------
 * Eigene Rechnung (Gleitkomma), nicht die des Moduls: Huellquader = MD1-Huelle aller Meshes (re15_md1_parse = nur der Parser), Drehung
 * R = Rx * Ry * Rz (die Formel von main.c pc_prop_rot_q12 ausmultipliziert), Welt = R x v + (x, y, z) (Typ 4 -900, Objekt-Zeichner
 * FUN_8002c18c @0x8002c234-50), gezeichnet nur im Regions-Viereck des Cuts (FUN_80014368 @0x8002c2dc). Typ 3 = Raster-Sprite
 * (@0x8002c2bc-c8) = in ROOM2090 die Wasseroberflaeche: kein Verdecker, ihre Hoehe ist die Wasserlinie der Sicht-Pruefung. */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
typedef struct { double lo[3], hi[3], r[9], t[3]; int typ; } v_obj_t;
static v_obj_t    s_vobj[RE15_SCD_MAX_PROPS];
static int        s_vobj_n, s_wasser_ok;
static int32_t    s_wasser_y;
static re15_md1_t s_vmd1;
static void v_objekte(void)
{
    s_vobj_n = 0; s_wasser_ok = 0;
    for (int pi = 0; pi < (int)g_scd.prop_count && pi < RE15_SCD_MAX_PROPS; pi++) {
        if (!g_scd.props[pi].active || g_scd.props[pi].parent_obj >= 0) continue;
        if (g_scd.props[pi].obj_type == 3) { s_wasser_y = g_scd.props[pi].y; s_wasser_ok = 1; continue; }
        int oid = g_scd.props[pi].obj_id;
        if (oid >= RE15_RDT_MAX_PROPS || !g_room_rdt.prop_md1[oid]) continue;
        if (re15_md1_parse(g_room_rdt.prop_md1[oid], g_room_rdt.prop_md1_size[oid], &s_vmd1) != 0) continue;
        v_obj_t *o = &s_vobj[s_vobj_n];
        int nv = 0;
        for (int i = 0; i < 3; i++) { o->lo[i] = 1e9; o->hi[i] = -1e9; }
        for (int m = 0; m < s_vmd1.mesh_count; m++)
            for (int pass = 0; pass < 2; pass++) {
                const re15_md1_vertex_t *V = pass ? s_vmd1.meshes[m].quad_vertices : s_vmd1.meshes[m].tri_vertices;
                int N = pass ? s_vmd1.meshes[m].quad_vertex_count : s_vmd1.meshes[m].tri_vertex_count;
                for (int v = 0; V && v < N; v++) {
                    double q[3] = { V[v].x, V[v].y, V[v].z };
                    for (int i = 0; i < 3; i++) { if (q[i] < o->lo[i]) o->lo[i] = q[i]; if (q[i] > o->hi[i]) o->hi[i] = q[i]; }
                    nv++;
                }
            }
        if (!nv) continue;
        double ax = g_scd.props[pi].rot_x * 2.0 * M_PI / 4096.0, ay = g_scd.props[pi].rot_y * 2.0 * M_PI / 4096.0,
               az = g_scd.props[pi].rot_z * 2.0 * M_PI / 4096.0;
        double cx = cos(ax), sx = sin(ax), cy = cos(ay), sy = sin(ay), cz = cos(az), sz = sin(az);
        double rx[9] = { 1, 0, 0, 0, cx, -sx, 0, sx, cx }, ry[9] = { cy, 0, sy, 0, 1, 0, -sy, 0, cy }, rz[9] = { cz, -sz, 0, sz, cz, 0, 0, 0, 1 };
        double t1[9];
        for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++) { t1[r*3+c] = 0; for (int k = 0; k < 3; k++) t1[r*3+c] += rx[r*3+k] * ry[k*3+c]; }
        for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++) { o->r[r*3+c] = 0; for (int k = 0; k < 3; k++) o->r[r*3+c] += t1[r*3+k] * rz[k*3+c]; }
        o->t[0] = g_scd.props[pi].x; o->t[1] = g_scd.props[pi].y - (g_scd.props[pi].obj_type == 4 ? 900 : 0); o->t[2] = g_scd.props[pi].z;
        o->typ = g_scd.props[pi].obj_type;
        s_vobj_n++;
    }
}
/* Strecke Kamera(cut) -> p durch einen in cut gezeichneten Objekt-Quader? */
static int v_verdeckt(int cut, const int32_t p[3])
{
    int16_t xs[4], zs[4];
    int reg = re15_rdt_get_region_quad(&g_room_rdt, cut, xs, zs);
    const re15_camera_cut_t *c = &g_room_rdt.cuts[cut];
    double a[3] = { c->pos_x, c->pos_y, c->pos_z }, q[3] = { p[0], p[1], p[2] };
    for (int i = 0; i < s_vobj_n; i++) {
        const v_obj_t *o = &s_vobj[i];
        if (reg && !re15_aot_point_in_quad((int32_t)o->t[0], (int32_t)o->t[2], xs, zs)) continue;   /* im Cut nicht gezeichnet */
        double al[3], ql[3];
        for (int k = 0; k < 3; k++) {
            al[k] = ql[k] = 0;
            for (int j = 0; j < 3; j++) { al[k] += o->r[j*3+k] * (a[j] - o->t[j]); ql[k] += o->r[j*3+k] * (q[j] - o->t[j]); }
        }
        double t0 = 0, t1 = 1; int schnitt = 1;
        for (int k = 0; k < 3 && schnitt; k++) {
            double d = ql[k] - al[k];
            if (fabs(d) < 1e-9) { if (al[k] < o->lo[k] || al[k] > o->hi[k]) schnitt = 0; continue; }
            double ta = (o->lo[k] - al[k]) / d, tb = (o->hi[k] - al[k]) / d;
            if (ta > tb) { double x = ta; ta = tb; tb = x; }
            if (ta > t0) t0 = ta;
            if (tb < t1) t1 = tb;
            if (t0 > t1) schnitt = 0;
        }
        if (schnitt) return 1;
    }
    return 0;
}
/* Sicht-Zaehler eines m_maulraum-Aufrufs (s_mv_cut >= 0 schaltet sie ein): Koerper-Vertices im Bild, davon von Raum-Objekten verdeckt,
 * davon ueber der Wasseroberflaeche (y < Wasserlinie; PSX: -y = oben). */
static int s_mv_cut = -1, s_mv_n, s_mv_im, s_mv_verd, s_mv_ueber;
static re15_camera_view_t s_mv_v;
static void m_sicht_punkt(const int32_t w[3])
{
    s_mv_n++;
    if (s_wasser_ok && w[1] < s_wasser_y) s_mv_ueber++;
    int64_t px = w[0], py = w[1], pz = w[2];
    int32_t vx = (int32_t)(((int64_t)s_mv_v.rot[0]*px + (int64_t)s_mv_v.rot[1]*py + (int64_t)s_mv_v.rot[2]*pz) >> 12) + s_mv_v.trans[0];
    int32_t vy = (int32_t)(((int64_t)s_mv_v.rot[3]*px + (int64_t)s_mv_v.rot[4]*py + (int64_t)s_mv_v.rot[5]*pz) >> 12) + s_mv_v.trans[1];
    int32_t vz = (int32_t)(((int64_t)s_mv_v.rot[6]*px + (int64_t)s_mv_v.rot[7]*py + (int64_t)s_mv_v.rot[8]*pz) >> 12) + s_mv_v.trans[2];
    if (vz <= 0) return;
    int sx = 160 + (int)(((int64_t)vx * s_mv_v.fov_screen_dist) / vz), sy = 120 + (int)(((int64_t)vy * s_mv_v.fov_screen_dist) / vz);
    if (sx < 0 || sx >= 320 || sy < 0 || sy >= 240) return;
    s_mv_im++;
    if (v_verdeckt(s_mv_cut, w)) s_mv_verd++;
}
/* NB6 (M2): laufende Kamera im Biss-Bild je Riegel-Lage (teil_maul setzt sie vor m_lauf). In ROOM2090 haben alle 13 Cuts dasselbe
 * Regions-Viereck (Sonde B41nb6/probe_region: (-11000..9500) x (-29000..-3500)) - die Kamera an einer Lage folgt aus dem Weg ueber die
 * RVD-Wechselsaetze (Abnahme 5 probe_kamreg: SW-Becken Cut 0, Rampe 2/3, z -12000 Ost 3, Westkanal 6/7). */
static int s_m_laufend;
static re15_emd_skeleton_t s_m_pl00_skel;
static re15_md1_t          s_m_pl00_md1;
static int m_pl00_geladen(void)
{
    static int s_ok = -1;
    size_t rsz = 0, msz = 0;
    if (s_ok >= 0) return s_ok;
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    uint8_t *md1 = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.MD1", &msz);
    s_ok = (emr && md1 && re15_emd_parse_skeleton(emr, rsz, &s_m_pl00_skel) == 0 && re15_md1_parse(md1, (int)msz, &s_m_pl00_md1) == 0);
    return s_ok;
}
static int m_kf(const re15_emd_animation_t *an, int clip, int fr)
{
    const re15_emd_clip_t *c = &an->clips[clip];
    int fi = c->first_frame + (fr % c->frame_count);
    int fend = c->first_frame + c->frame_count - 1;
    while ((an->frames[fi] & 0x8000u) && fi < fend) fi++;
    return (int)(an->frames[fi] & 0xFFFu);
}
/* Weltpunkt -> Gator-Modellrahmen (Umkehr von G + s x RotY x m) */
static void m_welt_zu_modell(const int32_t w[3], int32_t s, int32_t m[3])
{
    int32_t d[3] = { w[0] - e->x, w[1] - e->y, w[2] - e->z };
    int32_t cs = re15_cos_q12((int)e->rot_y), sn = re15_sin_q12((int)e->rot_y);
    int64_t mx = ((int64_t)cs * d[0] - (int64_t)sn * d[2]) >> 12;
    int64_t mz = ((int64_t)sn * d[0] + (int64_t)cs * d[2]) >> 12;
    m[0] = (int32_t)(mx * 4096 / s); m[1] = (int32_t)((int64_t)d[1] * 4096 / s); m[2] = (int32_t)(mz * 4096 / s);
}
/* Modellpunkt -> Bone-Rahmen (Umkehr von trans + rot x b) */
static void m_modell_zu_bone(const int32_t m[3], const re15_skel_pose_t *p, int32_t b[3])
{
    int32_t d[3] = { m[0] - p->trans[0], m[1] - p->trans[1], m[2] - p->trans[2] };
    for (int r = 0; r < 3; r++)
        b[r] = (int32_t)(((int64_t)p->rot[0*3+r] * d[0] + (int64_t)p->rot[1*3+r] * d[1] + (int64_t)p->rot[2*3+r] * d[2]) >> 12);
}
static void m_bone_welt(const re15_skel_pose_t *p, int32_t s, int32_t w[3])
{
    int32_t t[3] = { (int32_t)(((int64_t)p->trans[0] * s) >> 12), (int32_t)(((int64_t)p->trans[1] * s) >> 12),
                     (int32_t)(((int64_t)p->trans[2] * s) >> 12) };
    re15_skel_bone_to_world(t, e->rot_y, e->x, e->y, e->z, w);
}
/* Leons gezeichnete Wurzel (Welt) wie der Spieler-Zeichner im Opfer-Modus: pl + RotY(pl->rot_y) x render_scale x POS */
static void m_leon_wurzel(int32_t w[3])
{
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    int vkf = m_kf(&bk->anim_victim, pl->motion, (int)pl->anim_frame);
    int16_t px = 0, py = 0, pz = 0; re15_emd_get_keyframe_position(&bk->skel_victim, vkf, &px, &py, &pz);
    int32_t sp = pl->render_scale_q12 ? pl->render_scale_q12 : 4096;
    int32_t P[3] = { (int32_t)(((int64_t)px * sp) >> 12), (int32_t)(((int64_t)py * sp) >> 12), (int32_t)(((int64_t)pz * sp) >> 12) };
    re15_skel_bone_to_world(P, (int16_t)pl->rot_y, pl->x, pl->y, pl->z, w);
}
/* Anteil von Leons Koerper (PL00-Knochen + Opfer-Keyframes, wie main.c Spieler-Zweig) im Maulraum der Pose gp, in Promille */
static int m_maulraum(const re15_skel_pose_t *gp, int32_t s)
{
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    int32_t lo[3] = { 0x7fffffff, 0x7fffffff, 0x7fffffff }, hi[3] = { -0x7fffffff, -0x7fffffff, -0x7fffffff };
    for (int mesh = 6; mesh <= 7; mesh++) {
        const re15_md1_mesh_t *m = &bk->md1.meshes[mesh];
        for (int pass = 0; pass < 2; pass++) {
            const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
            int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
            for (int i = 0; i < NV; i++) {
                int32_t v[3] = { V[i].x, V[i].y, V[i].z };
                for (int r = 0; r < 3; r++) {
                    int64_t s2 = 0;
                    for (int k = 0; k < 3; k++) s2 += (int64_t)gp[mesh].rot[r*3+k] * v[k];
                    int32_t w = (int32_t)(s2 >> 12) + gp[mesh].trans[r];
                    if (w < lo[r]) lo[r] = w;
                    if (w > hi[r]) hi[r] = w;
                }
            }
        }
    }
    int vkf = m_kf(&bk->anim_victim, pl->motion, (int)pl->anim_frame);
    re15_emd_skeleton_t vs = s_m_pl00_skel;
    vs.keyframe_data = bk->skel_victim.keyframe_data; vs.keyframe_data_size = bk->skel_victim.keyframe_data_size;
    vs.keyframe_count = bk->skel_victim.keyframe_count; vs.keyframe_size_bytes = bk->skel_victim.keyframe_size_bytes;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *sv = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int r = re15_skel_compute_pose(&vs, vkf, poses);
    g_anim_pose_actor = sv;
    if (r != 0) return -1;
    s_mv_n = s_mv_im = s_mv_verd = s_mv_ueber = 0;
    if (s_mv_cut >= 0 && re15_camera_build_view(&g_room_rdt.cuts[s_mv_cut], &s_mv_v) != 0) s_mv_cut = -1;
    int32_t cs = re15_cos_q12((int)pl->rot_y), sn = re15_sin_q12((int)pl->rot_y);
    int32_t yaw[9] = { cs, 0, sn, 0, 0x1000, 0, -sn, 0, cs };
    int32_t s_pl = pl->render_scale_q12 ? pl->render_scale_q12 : 4096;
    if (s_pl != 4096) for (int k = 0; k < 9; k++) yaw[k] = (yaw[k] * s_pl) >> 12;
    int nb = s_m_pl00_skel.bone_count; if (nb > s_m_pl00_md1.mesh_count) nb = s_m_pl00_md1.mesh_count; if (nb > 15) nb = 15;
    int n = 0, nin = 0;
    for (int bi = 0; bi < nb; bi++) {
        const re15_md1_mesh_t *m = &s_m_pl00_md1.meshes[bi];
        int32_t yrot[9], ytr[3];
        for (int rr = 0; rr < 3; rr++) for (int c = 0; c < 3; c++) { int64_t s2 = 0;
            for (int k = 0; k < 3; k++) s2 += (int64_t)yaw[rr*3+k] * poses[bi].rot[k*3+c];
            yrot[rr*3+c] = (int32_t)(s2 >> 12); }
        for (int rr = 0; rr < 3; rr++) { int64_t s2 = 0;
            for (int k = 0; k < 3; k++) s2 += (int64_t)yaw[rr*3+k] * poses[bi].trans[k];
            ytr[rr] = (int32_t)(s2 >> 12); }
        for (int pass = 0; pass < 2; pass++) {
            const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
            int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
            for (int i = 0; i < NV; i++) {
                int32_t v[3] = { V[i].x, V[i].y, V[i].z }, w[3], mm[3];
                for (int rr = 0; rr < 3; rr++) { int64_t s2 = 0;
                    for (int k = 0; k < 3; k++) s2 += (int64_t)yrot[rr*3+k] * v[k];
                    w[rr] = (int32_t)(s2 >> 12) + ytr[rr] + (rr == 0 ? pl->x : rr == 1 ? pl->y : pl->z); }
                if (s_mv_cut >= 0) m_sicht_punkt(w);   /* Nachbesserung 6 (M3) */
                m_welt_zu_modell(w, s, mm);
                n++;
                if (mm[0] >= lo[0] && mm[0] <= hi[0] && mm[1] >= lo[1] && mm[1] <= hi[1] && mm[2] >= lo[2] && mm[2] <= hi[2]) nin++;
            }
        }
    }
    return n ? (int)((int64_t)nin * 1000 / n) : -1;
}
/* Nachbesserung 6 (Abnahme 5 M1): P3 endet nicht (RE2 @0x801011c4-e4) - gemessen werden P1 14 + P2 120 + P3 M_P3 Bilder. Im Spiel kam die
 * Game-Over-Uebergabe 141 P3-Bilder nach dem P3-Beginn (exe E1: Tod F627, P3 ab F762, Uebergabe F902); M_P3 = 300 deckt das doppelt. */
#define M_P3     300
#define M_FIN    (14 + 120 + M_P3)
#define M_BILDER (M_FIN + 6)
typedef struct { int n, pn[4], ph[M_BILDER]; int32_t kl[M_BILDER][3]; int pm[M_BILDER]; } m_bahn_t;
/* Ein Lauf: Leon (lx,lz,ly) als Statue bis zum Tod, hp 30 (ein Biss = 50, dmg_table[5] @DAT_8006f418), danach fuehrt der
 * Finisher. gx/gz/gyaw >= 0 x: Gator nach dem Intro hinstellen (Bezug wie unit_gator_fress). Rueckgabe 1 = Tod + Finisher gemessen.
 * band = Leons Kollisionsband bis zum Tod (Spieler +0x82 = DAT_800acad6, oben auf Rampe/Plattform 1). Ab dem Bild nach dem Biss
 * laeuft vor jedem Takt die Spieler-Wandklemme wie im Spiel (Dispatcher-Schwanz FUN_80031c44 `jal 0x8003b0a4` @0x80031d70, auch
 * im Tod; Port re15_player_body_and_walls vor der Gegner-KI) - sie darf Leon im Finisher nie verschieben (exe-Lauf BL2: Band 1
 * nach dem Sturz von der Plattform, P0 setzte Leon in die Band-1-Zelle SCA [8] noerdlich der Rampe, die Klemme schob ihn 951
 * in die Rampe: P1 Kiefer-Abstand 949). */
static int m_lauf(const char *was, int32_t lx, int32_t lz, int32_t ly, int band, int setzen, int32_t gx, int32_t gz, int gyaw,
                  int soll_phase, m_bahn_t *b, const m_bahn_t *bezug, char *zeile, size_t zn)
{
    if (!setup(1)) return 0;
    re15_player_set_pl00_banks(&s_m_pl00_skel, NULL);   /* NB6: Leons Knochen fuer die Buehnen-Sicht (im Spiel setzt game_step sie) */
    v_objekte();
    int laufend = s_m_laufend;                            /* NB6 (M2) */
    memset(b, 0, sizeof *b);
    pl->hp = 100;
    pl->x = lx; pl->z = lz; pl->y = ly; re15_collision_set_band(band); pl->state = 0; pl->motion = 0; pl->hit_react = 0;
    tick();
    for (int f = 0; f < 400 && (re15_gr2_im_intro(slot) || f < 2); f++) {
        pl->x = lx; pl->z = lz; pl->y = ly; re15_collision_set_band(band); pl->state = 0; pl->motion = 0; pl->hit_react = 0;
        tick();
    }
    re15_gr2_biss_cd(slot, 0);
    if (setzen) { e->x = gx; e->z = gz; e->y = -1200; e->rot_y = (int16_t)gyaw; }
    pl->hp = 30;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    int tod = -1, ph_tod = -1, bild = 0, cut = -1;
    int32_t kl_max = 0, pm_max = 0, ki_max = 0, klemm_max = 0; int wasser_aus = 0, kopf_aus = 0, leon_aus = 0, oben = 0, ki_fehlt = 0;
    int klemm_n = 0, band_falsch = 0;
    int fress_ende = -1, los = 0, ungeparkt = 0, mass_falsch = 0, verd_bilder = 0, verd_max = 0, ueber_min = 1000, im_min = 1 << 30;
    for (int f = 0; f < 6000; f++) {
        if (tod < 0) { g_scd.work_vars[0x0A] = (int16_t)laufend; g_scd.cam_id = (uint8_t)laufend; }   /* NB6 (M2) */
        if (pl->hp >= 0) { pl->x = lx; pl->z = lz; pl->y = ly; re15_collision_set_band(band); pl->state = 0; pl->motion = 0; pl->hit_react = 0; }
        if (tod >= 0) {                                   /* Spieler-Wandklemme vor der Gegner-KI, wie im Spiel */
            int32_t nx = pl->x, nz = pl->z;
            re15_collision_constrain(&g_room_rdt, pl->x, pl->z, &nx, &nz);
            int32_t d = (nx > pl->x ? nx - pl->x : pl->x - nx) + (nz > pl->z ? nz - pl->z : pl->z - nz);
            if (d && bild > 0) { klemm_n++; if (d > klemm_max) klemm_max = d; }   /* ab P1: Leon steht auf der P0-Lage */
            pl->x = nx; pl->z = nz;
        }
        int ph_vor = re15_gator_boss_phase(e);
        tick();
        int fr = re15_gator_boss_im_fressen(e);
        if (fr && tod < 0) { tod = f; ph_tod = ph_vor; continue; }   /* Biss-Bild: zeigt noch die alte Kamera (Schnitt-Auftrag) */
        if (tod < 0) continue;
        if (!fr) { fress_ende = bild; break; }   /* NB6 (M1): darf nicht vorkommen - RE2 P3 @0x801011c4-e4 schreibt keine Phase */
        if (bild >= M_FIN) break;
        int pidx = e->motion == 4 ? 1 : e->motion == 5 ? 2 : e->motion == 11 ? 3 : 0;
        /* NB6 (M1): Opfer gehalten (Spieler +0x1D3 |= 0x80 @0x80102f24-28 = re15_player_is_grabbed), in P3 geparkt (Opfer-Clip 1 Bild 119,
         * Leon-Handler Phase 3 ohne Animation), Maul-Massstab (render_scale_q12) bleibt */
        if (!re15_player_is_grabbed()) los++;
        if (pidx == 3 && !(pl->motion == 1 && pl->anim_frame == 119)) ungeparkt++;
        if (pl->render_scale_q12 != e->render_scale_q12) mass_falsch++;
        cut = re15_gr2_finisher_cut(slot);
        s_mv_cut = cut;                                   /* NB6 (M3): Sicht-Zaehler im naechsten m_maulraum */
        if (re15_collision_debug_band() != re15_collision_band_from_y(pl->y)) band_falsch++;   /* Band folgt Leons y (P0: Beckenboden) */
        re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
        int ov = (e->anim_flags & 0x04u) ? (int)e->motion : -1;
        int kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, ov, (uint32_t)e->anim_frame);
        re15_skel_pose_t gp[RE15_EMD_MAX_BONES];
        void *save = g_anim_pose_actor; g_anim_pose_actor = e;      /* gezeichnet: mit allen Haken (NPC-Pass) */
        int rv = re15_skel_compute_pose(&bk->skel, kf, gp);
        g_anim_pose_actor = save;
        if (rv != 0 || bild >= M_BILDER) { CHECK(0, "%s: Pose/Bildzahl in FRESSEN-Bild %d", was, bild); return 0; }
        int32_t lw[3], lm[3], kl[3];
        m_leon_wurzel(lw);
        m_welt_zu_modell(lw, s, lm);
        m_modell_zu_bone(lm, &gp[6], kl);
        int pm = m_maulraum(gp, s);
        if (s_mv_cut >= 0) {
            if (s_mv_verd > 0) verd_bilder++;
            if (s_mv_verd > verd_max) verd_max = s_mv_verd;
            if (s_mv_im < im_min) im_min = s_mv_im;
            if (pidx == 1 && s_mv_n > 0 && s_mv_ueber * 1000 / s_mv_n < ueber_min) ueber_min = s_mv_ueber * 1000 / s_mv_n;
        }
        s_mv_cut = -1;
        /* Abstand Leon <-> gezeichneter Kiefer gegen die authored Bahn (Messzeile der exe) */
        int32_t abw = 0, kab = -1, kopf[3], leon[3]; int ph2 = 0;
        if (!re15_gr2_fress_sitz(e, pl, &abw, kopf, leon, &kab, &ph2) || kab < 0) ki_fehlt++;
        else if (kab > ki_max) ki_max = kab;
        /* Wasser: Wurzel, Bones 1/5/6/7, Leons Wurzel */
        static const int kb[4] = { 1, 5, 6, 7 };
        int nass = re15_gr2_becken_draussen(e->x, e->z, 0) == 0 && re15_gr2_becken_draussen(lw[0], lw[2], 0) == 0;
        int32_t k6[3] = { 0, 0, 0 };
        for (int i = 0; i < 4; i++) {
            int32_t w[3]; m_bone_welt(&gp[kb[i]], s, w);
            if (kb[i] == 6) memcpy(k6, w, sizeof w);
            if (re15_gr2_becken_draussen(w[0], w[2], 0)) nass = 0;
        }
        if (!nass) { wasser_aus++; if (wasser_aus <= 3) printf("  %s: Bild %d P%d nicht im Wasser G=(%d,%d) Leon=(%d,%d)\n", was, bild, pidx, e->x, e->z, lw[0], lw[2]); }
        /* Bild der Finisher-Kamera */
        int sx = -1, sy = -1, lx2 = -1, ly2 = -1;
        int kopf_im = cut >= 0 && re15_gr2_im_bild_cut(cut, k6, &sx, &sy);
        int leon_im = cut >= 0 && re15_gr2_im_bild_cut(cut, lw, &lx2, &ly2);
        int leon_oben = !leon_im && cut >= 0 && lx2 >= 0 && lx2 < 320 && ly2 < 0;
        if (!kopf_im) kopf_aus++;
        if (!leon_im && !leon_oben) { leon_aus++; if (leon_aus <= 3) printf("  %s: Bild %d P%d Leon nicht im Bild von Cut %d: Bildlage (%d,%d), Welt (%d,%d,%d)\n", was, bild, pidx, cut, lx2, ly2, lw[0], lw[1], lw[2]); }
        if (leon_oben) oben++;
        b->ph[bild] = pidx; b->pm[bild] = pm; memcpy(b->kl[bild], kl, sizeof kl);
        b->pn[pidx & 3]++;
        if (bezug && bild < bezug->n) {
            for (int i = 0; i < 3; i++) { int32_t d = kl[i] - bezug->kl[bild][i]; if (d < 0) d = -d; if (d > kl_max) kl_max = d; }
            int32_t d = pm - bezug->pm[bild]; if (d < 0) d = -d; if (d > pm_max) pm_max = d;
            if (pidx != bezug->ph[bild]) CHECK(0, "%s: Bild %d Phase P%d, Bezug P%d", was, bild, pidx, bezug->ph[bild]);
        }
        bild++;
    }
    b->n = bild;
    int im_maul[4] = { 0, 0, 0, 0 };
    for (int i = 0; i < b->n; i++) if (b->pm[i] >= 500) im_maul[b->ph[i] & 3]++;
    snprintf(zeile, zn, "%-44s Tod Takt %d aus Phase %d, Finisher %d Bilder (P1 %d P2 %d P3 %d), Cut %d (laufend %d), Maul >= 50 %%: P1 %d "
             "P2 %d P3 %d, Kopf-lokal gegen Bezug max %d, Maulraum gegen Bezug max %d Promille, Kiefer-Abstand max %d (fehlt %d), nicht im "
             "Wasser %d, Kopf nicht im Bild %d, Leon nicht im Bild %d (oben hinaus %d), Wandklemme verschob Leon in %d Bildern (max %d), Band "
             "falsch %d | NB6: FRESSEN-Ende %d, Opfer nicht gehalten %d, P3 nicht geparkt %d, Massstab falsch %d, von Raum-Objekten verdeckt "
             "%d Bilder (max %d Vertices, Koerper im Bild min %d), P1 ueber der Wasserlinie y %d min %d Promille",
             was, tod, ph_tod, b->n, b->pn[1], b->pn[2], b->pn[3], cut, laufend, im_maul[1], im_maul[2], im_maul[3], (int)kl_max, (int)pm_max,
             (int)ki_max, ki_fehlt, wasser_aus, kopf_aus, leon_aus, oben, klemm_n, (int)klemm_max, band_falsch,
             fress_ende, los, ungeparkt, mass_falsch, verd_bilder, verd_max, im_min == (1 << 30) ? -1 : im_min,
             s_wasser_ok ? (int)s_wasser_y : 0, ueber_min);
    CHECK(tod >= 0, "%s: kein toedlicher Biss in 6000 Takten", was);
    if (tod < 0) return 0;
    if (soll_phase >= 0) CHECK(ph_tod == soll_phase, "%s: Tod aus Phase %d statt %d", was, ph_tod, soll_phase);
    CHECK(fress_ende < 0, "%s: FRESSEN endete nach %d Bildern (RE2 P3 @0x801011c4-e4 hat keinen Phasen-Schreiber, Abnahme 5 M1)",
          was, fress_ende);
    CHECK(b->pn[1] == 14 && b->pn[2] == 120 && b->pn[3] == M_P3, "%s: Finisher-Bilder P1 %d P2 %d P3 %d (14/120/%d: Clip 4 Bild 1..14, "
          "Clip 5 sf 1..120, Clip 11 ohne Ende)", was, b->pn[1], b->pn[2], b->pn[3], M_P3);
    CHECK(los == 0 && ungeparkt == 0 && mass_falsch == 0, "%s: Opfer in %d Bildern nicht gehalten (+0x1D3 & 0x80 @0x80102f24-28), in %d "
          "P3-Bildern nicht auf Opfer-Clip 1 Bild 119 geparkt, in %d Bildern ohne Maul-Massstab", was, los, ungeparkt, mass_falsch);
    CHECK(cut >= 0, "%s: keine Finisher-Buehne/Kamera (Rueckfall am Ort)", was);
    CHECK(cut != laufend, "%s: Buehnen-Kamera %d = laufende Kamera (kein Schnitt, Umsetzen sichtbar - Abnahme 5 M2)", was, cut);
    CHECK(verd_bilder == 0, "%s: Leon in %d Finisher-Bildern von Raum-Objekten verdeckt (max %d Vertices im Bild, Abnahme 5 M3)",
          was, verd_bilder, verd_max);
    CHECK(!s_wasser_ok || ueber_min >= 500, "%s: in P1 liegt nur %d Promille von Leons Koerper ueber der Wasseroberflaeche y %d "
          "(Typ-3-Raster obj 1) - Opfer im Wasser verdeckt (Abnahme 5 M3)", was, ueber_min, (int)s_wasser_y);
    CHECK(klemm_n == 0 && band_falsch == 0, "%s: Spieler-Wandklemme verschob Leon in %d Finisher-Bildern (max %d), Band nicht nach y in %d",
          was, klemm_n, (int)klemm_max, band_falsch);
    CHECK(ki_fehlt == 0 && ki_max <= 8, "%s: Abstand Leon <-> gezeichneter Kiefer weicht bis %d von der authored Bahn ab (%d Bilder ohne Wert)",
          was, (int)ki_max, ki_fehlt);
    CHECK(wasser_aus == 0, "%s: %d Bilder mit Gator/Kopf/Kiefer/Leon ausserhalb des Wassers", was, wasser_aus);
    CHECK(kopf_aus == 0 && leon_aus == 0, "%s: Kopf in %d, Leon in %d Bildern nicht im Bild der Finisher-Kamera Cut %d", was, kopf_aus, leon_aus, cut);
    if (bezug) {
        CHECK(b->n == bezug->n, "%s: %d Finisher-Bilder, Bezug %d", was, b->n, bezug->n);
        CHECK(kl_max <= 32, "%s: Leons Wurzel im gezeichneten Kopf-Rahmen bis %d neben dem CHASE-Bezug (<= 32)", was, (int)kl_max);
        CHECK(pm_max <= 20, "%s: Leons Anteil im Maulraum bis %d Promille neben dem CHASE-Bezug (<= 20)", was, (int)pm_max);
    }
    return 1;
}
/* Gegenprobe M3 (Abnahme 5): die NB5-Buehne des Tuerwegs E1 (gator_boss.log NB5: "-> (5638,-12546) Gier 2323 Cut 4") mit Leon in P1
 * (P0-Lage Gator + RotY(Gier) x (10643 x s, -915) @0x8010106c-84, Opfer-Clip 0 Bild 8, Maul-Massstab) - die Raum-Leiche obj 3 muss
 * hier Vertices verdecken, sonst sieht der Riegel den Befund nicht. */
static int m_gegenprobe_nb5(void)
{
    if (!setup(1)) return 0;
    v_objekte();
    leon_statue(-4000, -22000); tick();                     /* Boss-INIT: Zeichen-Skala 2/3 (GB_SCALE_Q12) */
    e->x = 5638; e->z = -12546; e->y = -1200; e->rot_y = 2323; e->motion = 4; e->anim_frame = 0;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    int32_t vx = (int32_t)(((int64_t)10643 * s) >> 12), c = re15_cos_q12(2323), sn = re15_sin_q12(2323);
    pl->x = e->x + (int32_t)(((int64_t)c * vx + (int64_t)sn * (-915)) >> 12);
    pl->z = e->z + (int32_t)((-(int64_t)sn * vx + (int64_t)c * (-915)) >> 12);
    pl->y = e->y; pl->rot_y = 2323; pl->motion = 0; pl->anim_frame = 8; pl->render_scale_q12 = (int16_t)s;
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    re15_skel_pose_t gp[RE15_EMD_MAX_BONES];
    int kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, 4, 0);
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL; re15_skel_compute_pose(&bk->skel, kf, gp); g_anim_pose_actor = save;
    s_mv_cut = 4;
    m_maulraum(gp, s);
    s_mv_cut = -1;
    printf("maul: Gegenprobe NB5-Buehne E1 (5638,-12546) Gier 2323 Cut 4, Leon P1 (%d,%d): Koerper %d Vertices, im Bild %d, von Raum-Objekten "
           "verdeckt %d\n", pl->x, pl->z, s_mv_n, s_mv_im, s_mv_verd);
    CHECK(s_mv_verd > 0, "Gegenprobe: die Raum-Leiche verdeckt in der NB5-Buehne keine Vertices - der Riegel waere blind fuer M3");
    /* Gegenprobe Wasserlinie: NB5 stellte Leon in P1 auf den Beckenboden (pl->y = 0) - mit dem Maul-Massstab liegt er dann unter der
     * Wasseroberflaeche (Typ-3-Raster obj 1 y -1620); die Schranke ">= 500 Promille ueber der Wasserlinie" muss das verwerfen */
    pl->y = 0;
    s_mv_cut = 4;
    m_maulraum(gp, s);
    s_mv_cut = -1;
    int pro = s_mv_n ? s_mv_ueber * 1000 / s_mv_n : 0;
    printf("maul: Gegenprobe NB5-Hoehe (Leon P1 auf y 0): %d Promille des Koerpers ueber der Wasserlinie y %d\n", pro, (int)s_wasser_y);
    CHECK(s_wasser_ok && pro < 500, "Gegenprobe: auf y 0 liegen %d Promille ueber der Wasserlinie - die Schranke waere blind fuer die "
          "Wasser-Verdeckung", pro);
    return 1;
}
static int teil_maul(void)
{
    if (!m_pl00_geladen()) { printf("FAIL: PL00.EMR/MD1 nicht ladbar\n"); return 1; }
    static m_bahn_t bezug, b;
    char z[1024];
    s_mit_objekten = 1;                                   /* NB6 (M3): Raum-Objekte von ROOM2090 */
    m_gegenprobe_nb5();
    /* Bezug = Lage von unit_gator_fress: Leon (-4000,-22000) im Wasser, Gator nach dem Intro auf (-6000,-22000) mit Blick auf Leon;
     * laufende Kamera 0 (SW-Becken, exe A2/G1: cam=0 im Biss-Bild) */
    s_m_laufend = 0;
    if (!m_lauf("CHASE-Bezug (offenes Wasser, unit_gator_fress)", -4000, -22000, 0, 0, 1, -6000, -22000, 0, -1, &bezug, NULL, z, sizeof z))
        return 1;
    printf("maul: %s\n", z);
    int im2 = 0, im3 = 0;
    for (int i = 0; i < bezug.n; i++) { if (bezug.pm[i] >= 500 && bezug.ph[i] == 2) im2++; if (bezug.pm[i] >= 500 && bezug.ph[i] == 3) im3++; }
    CHECK(im2 >= 100 && im3 == M_P3, "Bezug: Leon in P2 %d / P3 %d Bildern zu >= 50 %% im Maulraum (Sonde: 106 / alle %d)", im2, im3, M_P3);
    /* laufende Kamera je Lage: Rampe Ost = Cut 2 (exe E1 state.log cam=2 F587-F627), Rampenmitte = Cut 3 (Rampe 2/3), Plattformblock =
     * Cut 6 = die Kamera, die die Buehne ohne Ausschluss nimmt (erzwingt die Ausweich-Buehne), BL2 (z -11982 Ost) = Cut 3 */
    struct { const char *was; int32_t x, z; int lauf; } k[4] = {
        { "GUARD Rampe Ost (Ankunft 20A0-Tuer)", 6300, -16250, 2 },
        { "GUARD Rampe Ost (E1-Lage)", 6732, -16250, 2 },
        { "GUARD Rampenmitte (MN2)", 4000, -15000, 3 },
        { "GUARD Plattformblock", 0, -16250, 6 },
    };
    for (int i = 0; i < 4; i++) {
        s_m_laufend = k[i].lauf;
        /* laufend 6 = Ausweich-Buehne: eine andere Lage als der Bezug, die Bezugs-Vergleiche (Kopf-lokal, Maulraum) gelten weiter, weil
         * beide die authored Relation zeigen; nur die Buehne/Kamera ist eine andere */
        if (!m_lauf(k[i].was, k[i].x, k[i].z, -1800, 1, 0, 0, 0, 0, 5, &b, &bezug, z, sizeof z)) continue;
        printf("maul: %s\n", z);
    }
    /* exe-Lauf BL2 (Fortsetzung NB5): toedlicher CHASE-Biss, Leon nach dem Sturz von der Plattform im Wasser, aber noch auf Band 1;
     * Gator-Lage beim Biss aus gator_boss.log ("von (4741,-11236) Gier 233"), Leon vor der Schnauze (6732,-11982). */
    s_m_laufend = 3;
    if (m_lauf("CHASE-Biss, Leon noch auf Band 1 (exe BL2)", 6732, -11982, 0, 1, 1, 4741, -11236, 233, -1, &b, &bezug, z, sizeof z))
        printf("maul: %s\n", z);
    return fails ? 1 : 0;
}
/* buehne - Garantie der Finisher-Buehne (PORT-WAHL, Form = RE2-P0 @0x80101048-4c): fuer JEDE Gator-Lage, in der ein toedlicher Biss
 * fallen kann (Wurzel im Wasser ausserhalb der Wand-Klemmzone 1466, dazu Plattform/Rampe = CROSS-Biss), findet
 * re15_gr2_finisher_buehne eine Buehne (Raster 1000, 8 Gierwinkel). Je Buehne unabhaengig nachgeprueft: Wurzel im Wasser ausserhalb
 * der Klemmzone, Kamera ist eine Zonenkamera, Versatz <= 27000 (Suchgrenze = Becken-Diagonale). Sonde B41nb5c/probe_bsweep
 * Raster 250: 24256 Lagen, 0 ohne Buehne, Versatz max 8997. */
static int teil_buehne(void)
{
    s_mit_objekten = 1;                                   /* NB6 (M3): die Buehne meidet verdeckende Raum-Objekte */
    if (!setup(1)) return 1;
    if (!m_pl00_geladen()) { printf("FAIL: PL00.EMR/MD1 nicht ladbar\n"); return 1; }
    re15_player_set_pl00_banks(&s_m_pl00_skel, NULL);
    leon_statue(-4000, -22000); tick();
    /* Nachbesserung 6 (Abnahme 5 M2): fuer JEDE Bisslage und JEDE Zonenkamera als laufende Kamera (angezeigt work_vars[0x0A] =
     * DAT_800b0fe4 @0x80021bfc und angefordert cam_id = DAT_800afbb5 @0x80014300) nimmt die Buehne eine ANDERE Kamera - das Umsetzen
     * faellt damit immer in einen Schnitt (RE2 P0 Umsetzen @0x80101048-4c im Takt des Schnitts @0x80100ff4). Erst ohne Ausschluss
     * (laufend = Cut 12, keine Zonenkamera) die freie Wahl c0, dann jede Zonenkamera als laufende, dann angezeigt c0 / angefordert eine
     * andere (beide gesperrt). Raster 2000 x 4 Gierwinkel (Wasser + Plattform/Rampe; die NB6-Suche laeuft ueber das ganze Becken). */
    int n = 0, ohne = 0, falsch = 0, gleich = 0, frei_c0 = 0, ausschluss_wirkt = 0, paar_falsch = 0, lagen = 0; int32_t vmax = 0;
    int nutz[16] = {0};
    for (int32_t x = -8900 + 1466; x <= 7200 - 1466; x += 2000)
        for (int32_t z = -27000 + 1466; z <= -5400 - 1466; z += 2000)
            for (int yaw = 0; yaw < 4096; yaw += 1024) {
                e->x = x; e->z = z; e->y = -1200; e->rot_y = (int16_t)yaw;
                lagen++;
                g_scd.work_vars[0x0A] = 12; g_scd.cam_id = 12;
                if (!re15_gr2_finisher_buehne(slot, e)) { ohne++; if (ohne <= 5) printf("  keine Buehne (frei): G=(%d,%d) Gier %d\n", x, z, yaw); continue; }
                int c0 = re15_gr2_finisher_cut(slot);
                frei_c0++;
                for (int rc = 0; rc < g_room_rdt.cut_count; rc++) {
                    if (!zonenkamera(rc)) continue;
                    g_scd.work_vars[0x0A] = (int16_t)rc; g_scd.cam_id = (uint8_t)rc;
                    n++;
                    if (!re15_gr2_finisher_buehne(slot, e)) { ohne++; if (ohne <= 5) printf("  keine Buehne: G=(%d,%d) Gier %d laufend %d\n", x, z, yaw, rc); continue; }
                    re15_actor_t bb = *e;
                    re15_gr2_finisher_setzen(slot, &bb);
                    int c = re15_gr2_finisher_cut(slot);
                    if (c >= 0 && c < 16) nutz[c]++;
                    if (c == rc) { gleich++; if (gleich <= 5) printf("  Buehnen-Kamera = laufende Kamera %d: G=(%d,%d) Gier %d\n", rc, x, z, yaw); }
                    if (rc == c0 && c != c0) ausschluss_wirkt++;
                    int zk = 0;
                    for (int i = 0; i < g_room_rdt.zone_count; i++) if ((int)g_room_rdt.zones[i].cam_from == c) zk++;
                    int64_t dx = bb.x - x, dz = bb.z - z;
                    int32_t v = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
                    if (v > vmax) vmax = v;
                    if (re15_gr2_becken_draussen(bb.x, bb.z, 1466) || c < 0 || zk < 2 || v > 27000) {
                        falsch++;
                        if (falsch <= 5) printf("  Buehne falsch: G=(%d,%d) Gier %d -> (%d,%d) Cut %d (Saetze %d) Versatz %d\n", x, z, yaw, bb.x, bb.z, c, zk, v);
                    }
                }
                /* angezeigt c0, angefordert (c0 + 1) mod 8: beide gesperrt */
                g_scd.work_vars[0x0A] = (int16_t)c0; g_scd.cam_id = (uint8_t)((c0 + 1) % 8);
                if (re15_gr2_finisher_buehne(slot, e)) {
                    int c = re15_gr2_finisher_cut(slot);
                    if (c == c0 || c == (c0 + 1) % 8) paar_falsch++;
                } else ohne++;
            }
    printf("buehne: %d Bisslagen (Raster 2000 x 4 Gier) x laufende Zonenkamera = %d Suchen, ohne Buehne %d, Buehne = laufende Kamera %d, "
           "Buehne falsch %d, angezeigt/angefordert-Paar falsch %d, Versatz max %d; freie Wahl c0 in %d Lagen, Ausschluss von c0 wirkte %d mal; "
           "Buehnen-Kameras 0..7: %d %d %d %d %d %d %d %d\n", lagen, n, ohne, gleich, falsch, paar_falsch, (int)vmax, frei_c0, ausschluss_wirkt,
           nutz[0], nutz[1], nutz[2], nutz[3], nutz[4], nutz[5], nutz[6], nutz[7]);
    CHECK(lagen >= 200 && n >= 1000, "nur %d Lagen / %d Suchen geprueft", lagen, n);
    CHECK(ohne == 0, "%d Suchen ohne Finisher-Buehne (Rueckfall am Ort = Maul leer, Abnahme 4 M1)", ohne);
    CHECK(gleich == 0 && paar_falsch == 0, "%d Buehnen auf der laufenden Kamera, %d auf angezeigt/angefordert (Abnahme 5 M2)", gleich, paar_falsch);
    CHECK(ausschluss_wirkt == frei_c0, "Ausschluss der frei gewaehlten Kamera wirkte nur %d von %d mal", ausschluss_wirkt, frei_c0);
    CHECK(falsch == 0, "%d Buehnen ausserhalb des Wassers / ohne Zonenkamera / weiter als die Becken-Diagonale 27000", falsch);
    return fails ? 1 : 0;
}

/* ================================================================================================ */
/* Runde 41 Spur G (Nachbesserung 4, Abnahme G 3 M3): KOPF-KLEMME (gator_re2_r41.c re15_gr2_kopf_klemme, Haken re15_gator_boss_tick
 * (2b)). Abnahme 3 (v1_leon ohne Testhaken): der Alligator trug den Kanister mit dem Kopf in der Suedwand - die Wand-Klemme haelt nur
 * die Wurzel (hit_radius_min), Kanister beim R1 (5996,-2041,-29290), Wand z -27000, Regions-Viereck z >= -29000 -> alle sechs
 * Spritzring-Sprites verworfen (FUN_8002c820 @0x80077a30). RE2 hat keine Wandklemme fuer den Alligator, Kopf und Kanister bleiben
 * auf den Korridor-Beinen im Raum (Bein 2 x -23078 @0x80101048). Riegel: Kanister im Maul, Leon an der Suedwand (5800,-26500) wie
 * im Abnahme-Lauf, der Alligator mit der Wurzel auf der Steuerziel-Linie z -25400 (Kante -27000 + 1600) Richtung Sueden (Gier
 * 0x400): VOR dem Takt meldet die Klemme den Ueberstand (Kopf/Kanister jenseits der Wand); NACH jedem Takt liegt der Kanister
 * (Weltlage der gezeichneten Pose, re15_gr2_kanister_welt) innerhalb x[-8900..7200] z[-27000..-5400]; nach dem Schuss (R1) bleiben
 * Hals/Kopf/Kiefer (Bones 5/6/7) in jedem Bild des Todes-Clips innerhalb (Rohpose ohne Ueberblendung, Toleranz 300 = die
 * Ueberblend-Differenz zur gezeichneten Pose, nach der die Klemme rechnet), und die sechs Spritzring-Punkte (Kanister-Matrix x Tafel
 * @0x801000a8) liegen im Regions-Viereck des Raums (RVD-Anker, re15_rdt_get_region_quad). Gegenprobe: ohne Kanister greift die
 * Klemme nicht. */
static int teil_kopfklemme(void)
{
    if (!setup(5)) return 1;
    leon_statue(-6000, 2000);
    tick();
    int32_t dx = 0, dz = 0;
    e->x = 2000; e->z = -25400; e->y = -1200; e->rot_y = 0x400;
    CHECK(re15_gr2_kopf_klemme(slot, e, &dx, &dz) == 0, "Gegenprobe: Klemme ohne Kanister aktiv (%d,%d)", (int)dx, (int)dz);
    /* Aufnahme wie teil_scherben (Wurzel fest, Gier Nord), dann zur Suedwand */
    e->x = -6000; e->z = -22000; e->rot_y = 0xC00;
    int32_t sc = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    re15_gr2_kanister_setzen(-6000, -267, -22000 + (int32_t)(((int64_t)8800 * sc) >> 12));
    for (int f = 0; f < 60 && re15_gr2_kanister_stufe() != 7; f++) {
        leon_statue(-6000, 2000); e->x = -6000; e->z = -22000; e->rot_y = 0xC00; tick();
    }
    CHECK(re15_gr2_kanister_stufe() == 7, "Kanister nicht getragen (Stufe %d)", re15_gr2_kanister_stufe());
    e->x = 2000; e->z = -25400; e->rot_y = 0x400;
    const int vorher = re15_gr2_kopf_klemme(slot, e, &dx, &dz);
    printf("kopfklemme: Wurzel (2000,-25400) Gier 0x400 mit Kanister -> Ueberstand vor dem Takt (%d,%d)\n", (int)dx, (int)dz);
    CHECK(vorher && dz > 0, "Lage ohne Ueberstand - der Riegel prueft nichts (dz %d)", (int)dz);
    int32_t kmin = 0x7fffffff, kp[3], kr[9];
    for (int f = 0; f < 30; f++) {
        leon_statue(5800, -26500);
        tick();
        CHECK(re15_gr2_kanister_welt(kp, kr), "Bild %d: kein Kanister", f);
        if (kp[2] < kmin) kmin = kp[2];
        CHECK(kp[0] >= -8900 && kp[0] <= 7200 && kp[2] >= -27000 && kp[2] <= -5400, "Bild %d: Kanister (%d,%d) ausserhalb der Aussenwaende",
              f, kp[0], kp[2]);
    }
    printf("kopfklemme: 30 Bilder mit Kanister an der Suedwand, Kanister z min %d (Wand -27000)\n", (int)kmin);
    /* Schuss -> R1 (wie teil_scherben): Kanister beim R1-Start + Spritzring im Viereck, Bones 5/6/7 im Todes-Clip */
    (void)re15_enemy_take_damage(e, 0);
    tick();
    CHECK(re15_gr2_kanister_stufe() == 13, "Kanister nach dem Schuss in Stufe %d statt 13", re15_gr2_kanister_stufe());
    if (re15_gr2_kanister_welt(kp, kr)) {
        static const int16_t ring[6][3] = { { 0, -2800, 0 }, { 300, -2100, 0 }, { 0, -700, 0 }, { 0, 0, 300 }, { 0, -1400, -200 },
                                            { -500, -400, 200 } };   /* @0x801000a8 */
        int16_t rx[4], rz[4];
        const int reg = re15_rdt_get_region_quad(&g_room_rdt, 0, rx, rz);
        int drin = 0;
        for (int k = 0; k < 6; k++) {
            int32_t q[3];
            for (int i = 0; i < 3; i++)
                q[i] = kp[i] + (int32_t)(((int64_t)kr[i*3] * ring[k][0] + (int64_t)kr[i*3+1] * ring[k][1] + (int64_t)kr[i*3+2] * ring[k][2]) >> 12);
            if (!reg || re15_aot_point_in_quad(q[0], q[2], rx, rz)) drin++;
        }
        printf("kopfklemme: R1-Start Kanister (%d,%d,%d), Spritzring-Punkte im Regions-Viereck %d/6\n", kp[0], kp[1], kp[2], drin);
        CHECK(kp[2] >= -27000 && kp[0] >= -8900 && kp[0] <= 7200, "R1-Start: Kanister (%d,%d) ausserhalb", kp[0], kp[2]);
        CHECK(reg && drin == 6, "R1-Start: nur %d/6 Spritzring-Punkte im Regions-Viereck", drin);
    }
    int bilder = 0, raus = 0; int32_t zmin = 0x7fffffff;
    for (int f = 0; f < 60 && !re15_gr2_tot(); f++) {
        leon_statue(5800, -26500);
        tick();
        for (int b = 5; b <= 7; b++) {
            int32_t k[3]; kopf_welt(b, k);
            if (k[2] < zmin) zmin = k[2];
            if (k[0] < -8900 - 300 || k[0] > 7200 + 300 || k[2] < -27000 - 300 || k[2] > -5400 + 300) raus++;
        }
        bilder++;
    }
    printf("kopfklemme: R1-Todes-Clip %d Bilder, Hals/Kopf/Kiefer z min %d, ausserhalb (Toleranz 300) %d\n", bilder, (int)zmin, raus);
    CHECK(re15_gr2_tot(), "R1 nicht zu Ende");
    CHECK(raus == 0, "R1: %d Bone-Lagen ausserhalb der Aussenwaende", raus);
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
    else if (!strcmp(t, "scherben")) rc = teil_scherben();    /* Runde 41 Spur G (Nachbesserung 3, M3) */
    else if (!strcmp(t, "kopfklemme")) rc = teil_kopfklemme();   /* Runde 41 Spur G (Nachbesserung 4, M3) */
    else if (!strcmp(t, "putz")) rc = teil_putz();
    else if (!strcmp(t, "wand")) rc = teil_wand();
    else if (!strcmp(t, "kamera")) rc = teil_kamera();
    else if (!strcmp(t, "aufnahme")) rc = teil_aufnahme();
    else if (!strcmp(t, "lauer")) rc = teil_lauer();
    else if (!strcmp(t, "schleppen")) rc = teil_schleppen();
    else if (!strcmp(t, "anschwimmen")) rc = teil_anschwimmen();
    else if (!strcmp(t, "sitz")) rc = teil_sitz();
    else if (!strcmp(t, "gier")) rc = teil_gier();
    else if (!strcmp(t, "maul")) rc = teil_maul();
    else if (!strcmp(t, "buehne")) rc = teil_buehne();
    else printf("unbekannter Teil %s\n", t);
    printf("%s: %s (%d Fehler)\n", t, rc ? "FAIL" : "OK", fails);
    return rc;
}
