/*
 * probe_r37_fahrstuhl.c — Runde 37 Spur C "fahrstuhl" (analysis/befunde_runde37/C_fahrstuhl.md).
 *
 * Teile (je ein ctest, registriert in tests/unit/probes/r37_fahrstuhl.cmake):
 *   archiv    Auftrag #6 "Fahrstuhl Tuer in der Tuer Animation noch nicht korrekt ... sonst bauen": die Port-
 *             Archive P27F (Flurseite) / P27C (Kabinenseite) stehen in der Tabelle (Basis DOOR27, md1_eigen 2),
 *             Datei = Tabelle (Groesse/FNV), Tonteil + SCD BYTEGLEICH der RE2-Fahrstuhltuer DOOR27, MD1 = zwei
 *             GLATTE Fluegel mit GERADER Fuge (alle Fugen-Ecken auf z = +-3000 lokal = Welt 0), 1860 breit,
 *             6600 hoch, Vorderseite eben; Gegenprobe DOOR27: Fugenkante verzahnt (z-Spanne der Fugenecken
 *             > 400). Maschine P27F V0 = DOOR27 V0 Bild fuer Bild (gleiche Objektlagen: Fahrstuhl-Mechanik RE2,
 *             zwei Fluegel gegenlaeufig 2240). Licht der Vorderseite = 73 (NCCT, BK 68 @0x800142e8), Textur grau
 *             (kein Oliv) und gezeigt (Texel * 73 / 128) = gemalt (Zeilenprofil der entzerrten RE1.5-Fluegel,
 *             Werte aus analysis/befunde_runde37/C_belege/feld/profil.json, +-6).
 *   einstieg  echter Spielschritt: ROOM1040 / ROOM10C0 / ROOM1120 (Strom-Flag (4,0xF3), main00 Ck @0x0107A /
 *             @0x00E66 / @0x00C9A) -> Kabine ROOM1080 stellt GENAU eine Anfrage P27F V0 (S013/S040/S057).
 *   ausstieg  Auftrag #7 "Beim Aussteigen aus dem Fahrstuhl fehlt noch die Tuersequenz": ECHTER Skriptweg der
 *             Kabine (scd_event_fire 7/8/9 = Knopf 1F/2F/3F, Fahrt, Plc_dest zur Tuer, Aot_on) mit dem
 *             player_mode-Proxy des Spiels (main.c: flag(1,27)||flag(2,7) -> player_mode 2): ROOM1080/1081 ->
 *             P27C V0 S027/S028/S029, ROOM4020/4021 -> RE2-Datei DOOR27 V0 S221/S222/S223 (RE2 ROOM6010 sub02
 *             @0x00740/@0x0075A); Vorbedingung gemessen: beim Feuern ist player_mode 2. Gegenprobe: eine
 *             normale Tuer (ROOM10C0 -> 1080) in player_mode 2 bleibt ohne Sequenz (Sperre der Runde 31 gilt
 *             weiter, nur die Kabinen-Ausstiege gehen vorbei).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_skeleton.h"
#include "re15_door_seq.h"
#include "re15_fahrstuhl_r37.h"
#include "re15_md1.h"
#include "re15_tim.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fehler = 0;
#define PRUEF(c, ...) do { if (!(c)) { g_fehler++; printf("FEHLER: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

static uint32_t fnv1a(const uint8_t *d, size_t n)
{
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < n; i++) { h ^= d[i]; h *= 16777619u; }
    return h;
}

/* ---------------------------------------------------------------------------------------------
 * Spielschritt-Geruest (wie probe_r36_tueren.c)
 * ------------------------------------------------------------------------------------------ */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;
static int                s_kino = 0;      /* player_mode-Proxy wie main.c (cine_active -> 2) */
static int                s_kino_war = 0;

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_audio_event_t e;
    scd_vm_tick();
    if (s_kino) {
        /* Reihenfolge des PC-Hauptlaufs (platform/pc/main.c): SCD-Takt -> re15_actor_step_all_walkers (der
         * Plc_dest-Gang des Skripts) -> player_mode-Proxy (Block "cine_active": flag(1,27)||flag(2,7) ->
         * player_mode 2, Balken -1; fallende Flanke -> 15 Bilder Schliessen, dann 0) -> Spielschritt */
        re15_actor_step_all_walkers();
        int ka = re15_cine_active();
        if (ka) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_kino_war) g_scd.letterbox_countdown = 15;
        s_kino_war = ka;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    while (scd_audio_queue_pop(&e)) { }
}

static int s_flags[8][2], s_n_flags = 0;

static int room_boot(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) { printf("FEHLER: %s fehlt\n", rp); g_fehler++; return -1; }
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("FEHLER: RDT-Parse %s\n", rp); g_fehler++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    for (int k = 0; k < s_n_flags; k++) re15_game_flag_set((uint8_t)s_flags[k][0], (uint8_t)s_flags[k][1], 1);
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 0; pl->z = 0; pl->rot_y = 0;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    s_shown = 0; s_kino_war = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) frame(0, 0);
    int warm = 0;
    while (warm < 3000 &&
           (g_scd.player_mode == 2 || g_scd.letterbox_countdown != 0 || g_scd.message_active ||
            g_scd.message_display_frames > 0 || g_scd.message_query)) {
        uint16_t e = ((warm % 10) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        warm++;
    }
    g_room_change.pending = 0;
    return 0;
}

static re15_door_seq_anfrage_t s_gefangen;
static int s_n_gefangen = 0;
static int s_pmode_beim_fang = -1;
static void fang(const re15_door_seq_anfrage_t *a) { s_gefangen = *a; s_n_gefangen++; s_pmode_beim_fang = g_scd.player_mode; }

static const re15_tuer_eigen_t *eigen_nach_kennung(const char *k, int *idx)
{
    for (int i = 1; i <= re15_door_seq_eigen_anzahl(); i++) {
        const re15_tuer_eigen_t *e = re15_door_seq_eigen(i);
        if (e && !strcmp(e->kennung, k)) { if (idx) *idx = i; return e; }
    }
    return NULL;
}

/* ---------------------------------------------------------------------------------------------
 * Teil "archiv"
 * ------------------------------------------------------------------------------------------ */
typedef struct { int bilder; int32_t pos[400][2][3]; } bahn_t;

static int maschine(const uint8_t *modell, int n_modell, int var, bahn_t *b)
{
    static re15_door_seq_t s;
    memset(b, 0, sizeof *b);
    if (re15_door_seq_start(&s, modell, n_modell, var, 0, 0x27) != 0) return -1;
    int bild = 0;
    while (bild < 4000 && re15_door_seq_bild(&s, 1)) {
        if (bild < 400)
            for (int o = 0; o < 2; o++) for (int a = 0; a < 3; a++) b->pos[bild][o][a] = s.obj[o].on ? s.obj[o].pos[a] : 0;
        bild++;
    }
    b->bilder = bild;
    int ok = s.md1_ok && s.tim_ok && s.notizen == 0;
    re15_door_seq_ende(&s);
    return ok ? 0 : -2;
}

/* Fugenkante eines Fluegel-Mesh: Ecken mit |z| >= 2000 (Fluegel 0 lokal 1140..3000, Fluegel 1 -3000..-1140;
 * DOOR27 Mesh 0 z 0..3236 / Mesh 1 -3186..0) -> z-Spanne der Ecken an der Fugenseite (gerade = 0). */
static void fuge_mass(const re15_md1_mesh_t *m, int fluegel, int *spanne, int *zmin, int *zmax, int *xmax, int *ymin, int *ymax)
{
    int lo = 1 << 30, hi = -(1 << 30), xm = -(1 << 30), y0 = 1 << 30, y1 = -(1 << 30), zlo = 1 << 30, zhi = -(1 << 30);
    for (int i = 0; i < m->tri_vertex_count; i++) {
        const re15_md1_vertex_t *v = &m->tri_vertices[i];
        int z = v->z;
        if (z < zlo) zlo = z;
        if (z > zhi) zhi = z;
        if (v->x > xm) xm = v->x;
        if (v->y < y0) y0 = v->y;
        if (v->y > y1) y1 = v->y;
    }
    /* Fugenseite = die der Mitte (Welt z 0) zugewandte Haelfte: Fluegel 0 obere 20 % von z, Fluegel 1 untere 20 % */
    int grenze = fluegel == 0 ? zhi - (zhi - zlo) / 5 : zlo + (zhi - zlo) / 5;
    for (int i = 0; i < m->tri_vertex_count; i++) {
        int z = m->tri_vertices[i].z;
        if (fluegel == 0 ? z >= grenze : z <= grenze) { if (z < lo) lo = z; if (z > hi) hi = z; }
    }
    *spanne = hi - lo; *zmin = zlo; *zmax = zhi; *xmax = xm; *ymin = y0; *ymax = y1;
}

/* gemaltes Zeilenprofil L (Rec.601) der entzerrten RE1.5-Fluegel (tools/fahrstuhl_r37/gemalt_fluegel.py,
 * analysis/befunde_runde37/C_belege/feld/profil.json "zeilen"[4 + 16 k], k = 0..7; Feldzeile r = Fluegel v 2r ->
 * v = 8, 40, 72, ..., 232): Flurseite = ROOM10C00 + ROOM11200 (je zwei Fluegel), Kabinenseite = ROOM10800 Cut 0 */
static const double k_gemalt_flur[8]   = { 39.7, 38.7, 34.8, 31.5, 26.8, 21.0, 13.8, 6.4 };
static const double k_gemalt_kabine[8] = { 67.3, 85.3, 82.3, 64.0, 47.7, 36.1, 25.9, 16.4 };
#define C_VORDERSEITE 73     /* NCCT der Vorderseite (BK 68 @0x800142e8, L @0x8009a470, LCM 1600 @0x8009a490) */

/* Licht + Farbe: Bild 20 der Maschine (Tuer zu, vor dem Oeffnen bei Bild 70), Fluegel 0/1 durch den Zeichner
 * der Tuerszene (door_seq_zeichnen.c = RE2 FUN_80014234/FUN_8001468c): jede Vorderseiten-Ecke (u <= 119) muss
 * die Farbe 73 tragen; dann ist gezeigt = Texel * 73 / 128 (psx-spx GPU, Modulation) und wird gegen das gemalte
 * Zeilenprofil gelegt. */
typedef struct { int vorn, vorn73, andere; } licht_t;
static void licht_fang(void *ctx, const re15_door_dreieck_t *d)
{
    licht_t *l = (licht_t *)ctx;
    if (d->u[0] <= 119 && d->u[1] <= 119 && d->u[2] <= 119) {
        l->vorn++;
        int ok = 1;
        for (int k = 0; k < 3; k++) for (int c = 0; c < 3; c++) ok &= d->rgb[k][c] == C_VORDERSEITE;
        l->vorn73 += ok;
    } else {
        l->andere++;
    }
}

static void licht_und_farbe(const uint8_t *tm, int n, const char *kennung, const double soll[8])
{
    static re15_door_seq_t s;
    if (re15_door_seq_start(&s, tm, n, 0, 0, 0x27) != 0 || !s.md1_ok || !s.tim_ok) { PRUEF(0, "%s: Start", kennung); return; }
    int bild = 0;
    while (bild < 20 && re15_door_seq_bild(&s, 1)) bild++;
    licht_t l = { 0, 0, 0 };
    int lfd = 0;
    for (int o = 0; o < 2; o++)
        re15_door_mesh_zeichnen(&s.md1.meshes[s.obj[o].mesh], &s.obj[o].welt, &s.obj[o].welt_vor, s.obj[o].flags, &lfd,
                                licht_fang, &l);
    printf("%s Bild %d: %d Vorderseiten-Dreiecke sichtbar, davon %d mit Eckfarbe %d; %d Kantendreiecke\n", kennung, bild,
           l.vorn, l.vorn73, C_VORDERSEITE, l.andere);
    PRUEF(l.vorn >= 100 && l.vorn73 == l.vorn, "%s: Vorderseite nicht ganz sichtbar / Eckfarbe nicht 73", kennung);
    const uint8_t *px = (const uint8_t *)s.tim.pixels;
    double dmax = 0.0, grau = 0.0;
    int ng = 0;
    printf("%s  v   gezeigt L (Texel*73/128)   gemalt L\n", kennung);
    for (int k = 0; k < 8; k++) {
        int v = 8 + 32 * k;
        double sum = 0.0;
        int nn = 0;
        for (int u = 4; u <= 115; u++) {
            uint16_t c = s.tim.clut[px[v * s.tim.width + u]];
            int r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
            double R = (r << 3) | (r >> 2), G = (g << 3) | (g >> 2), B = (b << 3) | (b >> 2);
            sum += (0.299 * R + 0.587 * G + 0.114 * B) * C_VORDERSEITE / 128.0;
            double dg = R > G ? R - G : G - R, db = G > B ? G - B : B - G;
            grau += (dg > db ? dg : db) * C_VORDERSEITE / 128.0;
            ng++; nn++;
        }
        double gez = sum / nn, d = gez > soll[k] ? gez - soll[k] : soll[k] - gez;
        if (d > dmax) dmax = d;
        printf("%s %3d   %6.1f                    %6.1f\n", kennung, v, gez, soll[k]);
    }
    grau /= ng;
    printf("%s: |gezeigt - gemalt| hoechstens %.1f, Farbabstand der Kanaele im Mittel %.1f (gezeigt)\n", kennung, dmax, grau);
    PRUEF(dmax <= 6.0, "%s: gezeigt weicht vom gemalten Zeilenprofil um %.1f ab (> 6)", kennung, dmax);
    PRUEF(grau <= 5.0, "%s: nicht grau (Kanalabstand %.1f)", kennung, grau);
    re15_door_seq_ende(&s);
}

static void teil_archiv(void)
{
    int i_f = 0, i_c = 0;
    const re15_tuer_eigen_t *pf = eigen_nach_kennung("P27F", &i_f), *pc = eigen_nach_kennung("P27C", &i_c);
    PRUEF(pf && pc, "P27F/P27C fehlen in gen/re15_tuer_eigen.inc");
    if (!pf || !pc) return;
    printf("Tabelle: P27F = eigen %d, P27C = eigen %d (von %d Port-Archiven)\n", i_f, i_c, re15_door_seq_eigen_anzahl());
    char pb[600];
    snprintf(pb, sizeof pb, "%s/DOOR/DOOR27.DO2", RE15_ASSET_RE2_DIR);
    size_t nb = 0;
    uint8_t *b = slurp(pb, &nb);
    PRUEF(b != NULL, "%s fehlt", pb);
    int ton = 0, modell = 0, sektor = 0, groesse = 0;
    PRUEF(re15_door_seq_re2_archiv(0x27, &ton, &modell, &sektor, &groesse) == 0 && groesse == (int)nb,
          "DOOR27 passt nicht zur RE2-Tabelle @0x8009a520");
    if (!b) return;
    const uint8_t *bm = b + sektor * 0x800;
    static re15_md1_t md27;
    uint32_t bmd1 = rd32(bm), btim = rd32(bm + 4);
    PRUEF(re15_md1_parse(bm + bmd1, (int)(btim - bmd1), &md27) == 0 && md27.mesh_count == 3, "DOOR27-MD1");

    /* Gegenprobe DOOR27: Fugenkante VERZAHNT */
    for (int f = 0; f < 2; f++) {
        int sp, z0, z1, xm, y0, y1;
        fuge_mass(&md27.meshes[f], f, &sp, &z0, &z1, &xm, &y0, &y1);
        printf("DOOR27 Mesh %d (vorher): z %d..%d, Fugenkante z-Spanne %d (Zaehne), x bis %d, y %d..%d, %d Dreiecke\n",
               f, z0, z1, sp, xm, y0, y1, md27.meshes[f].triangle_count);
        PRUEF(sp > 400, "Gegenprobe DOOR27 Mesh %d: Fugenkante sollte verzahnt sein (Spanne %d)", f, sp);
    }
    static bahn_t b27, bp;
    PRUEF(maschine(bm, modell, 0, &b27) == 0 && b27.bilder > 200, "DOOR27 V0 laeuft nicht");

    const re15_tuer_eigen_t *arch[2] = { pf, pc };
    for (int k = 0; k < 2; k++) {
        const re15_tuer_eigen_t *t = arch[k];
        char p[600];
        snprintf(p, sizeof p, "%s/RE15DOOR/%s.DO2", RE15_ASSET_SHARED_DIR, t->kennung);
        size_t n = 0;
        uint8_t *d = slurp(p, &n);
        PRUEF(d != NULL, "%s fehlt", p);
        if (!d) continue;
        PRUEF(t->basis == 0x27 && t->md1_eigen == 2, "%s: Basis %02X md1_eigen %d", t->kennung, t->basis, t->md1_eigen);
        PRUEF((size_t)t->datei == n && t->sektor * 0x800u + t->modell == t->datei && fnv1a(d, n) == t->fnv
              && (int)t->ton == ton && (int)t->sektor == sektor, "%s: Datei passt nicht zur Tabelle", t->kennung);
        PRUEF(memcmp(d, b, (size_t)ton) == 0, "%s: Tonteil nicht bytegleich DOOR27", t->kennung);
        const uint8_t *tm = d + t->sektor * 0x800;
        uint32_t md1_rel = rd32(tm), tim_rel = rd32(tm + 4);
        PRUEF(md1_rel == bmd1 && memcmp(tm + 8, bm + 8, md1_rel - 8) == 0, "%s: SCD nicht bytegleich DOOR27", t->kennung);
        static re15_md1_t md;
        int ok = re15_md1_parse(tm + md1_rel, (int)(tim_rel - md1_rel), &md) == 0 && md.mesh_count == 3;
        PRUEF(ok, "%s: MD1 nicht lesbar / nicht 3 Meshes", t->kennung);
        if (ok) {
            for (int f = 0; f < 2; f++) {
                int sp, z0, z1, xm, y0, y1;
                fuge_mass(&md.meshes[f], f, &sp, &z0, &z1, &xm, &y0, &y1);
                printf("%s Mesh %d: z %d..%d (Breite %d), Fugenkante z-Spanne %d, x bis %d, y %d..%d, %d Dreiecke\n",
                       t->kennung, f, z0, z1, z1 - z0, sp, xm, y0, y1, md.meshes[f].triangle_count);
                PRUEF(sp == 0, "%s Mesh %d: Fugenkante nicht gerade (Spanne %d)", t->kennung, f, sp);
                PRUEF(z1 - z0 == 1860 && (f == 0 ? z1 == 3000 : z0 == -3000), "%s Mesh %d: Fluegel z %d..%d statt 1860 an der Fuge 3000",
                      t->kennung, f, z0, z1);
                PRUEF(y0 == -6600 && y1 == 0 && xm == 100, "%s Mesh %d: Hoehe/Dicke (y %d..%d, x %d)", t->kennung, f, y0, y1, xm);
            }
            PRUEF(md.meshes[2].tri_vertex_count == md27.meshes[2].tri_vertex_count
                  && !memcmp(md.meshes[2].tri_vertices, md27.meshes[2].tri_vertices,
                             sizeof *md27.meshes[2].tri_vertices * (size_t)md27.meshes[2].tri_vertex_count),
                  "%s: Mesh 2 nicht DOOR27", t->kennung);
        }
        /* Maschine Bild fuer Bild = DOOR27 (Skripte bytegleich): Lagen obj 0/1 */
        if (maschine(tm, (int)t->modell, 0, &bp) == 0) {
            int gleich = bp.bilder == b27.bilder && !memcmp(bp.pos, b27.pos, sizeof bp.pos);
            int32_t w0 = 0, w1 = 0;
            for (int f = 0; f < bp.bilder && f < 400; f++) {
                int32_t d0 = bp.pos[f][0][2] - bp.pos[0][0][2], d1 = bp.pos[f][1][2] - bp.pos[0][1][2];
                if (d0 < w0) w0 = d0;
                if (d1 > w1) w1 = d1;
            }
            printf("%s V0: %d Bilder (DOOR27 %d), Lagen obj 0/1 je Bild %s DOOR27, Fluegel z-Weg %d / %+d\n", t->kennung,
                   bp.bilder, b27.bilder, gleich ? "gleich" : "ANDERS als", w0, w1);
            PRUEF(gleich, "%s V0 laeuft anders als DOOR27 V0", t->kennung);
            PRUEF(w0 == -2240 && w1 == 2240, "%s V0: Fluegel schieben nicht je 2240 auseinander (%d / %d)", t->kennung, w0, w1);
        } else {
            PRUEF(0, "%s V0: Maschine/Format", t->kennung);
        }
        /* Licht der Vorderseite + Farbe: Tuerlicht wie door_seq_zeichnen.c (BK 68 ohne Flag 0x1000) */
        re15_tim_t tim;
        PRUEF(re15_tim_parse(tm + tim_rel, (int)(t->modell - tim_rel), &tim) == 0 && tim.bpp == 8 && tim.width == 128
              && tim.height == 256 && tim.clut_entries == 256, "%s: TIM", t->kennung);
        licht_und_farbe(tm, (int)t->modell, t->kennung, k == 0 ? k_gemalt_flur : k_gemalt_kabine);
        free(d);
    }
    free(b);
}

/* ---------------------------------------------------------------------------------------------
 * Teil "einstieg"
 * ------------------------------------------------------------------------------------------ */
static int vorwaerts_trifft(const re15_aot_t *a, int32_t px, int32_t pz, int yaw)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    int32_t fx = px + (int32_t)((620 * c) >> 12);
    int32_t fz = pz - (int32_t)((620 * s) >> 12);
    if (a->has_quad) return re15_aot_point_in_quad_fun80014368(fx, fz, a->xs, a->zs);
    long dx = (long)fx - (long)a->x, dz = (long)fz - (long)a->z;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    return dx <= a->half_w && dz <= a->half_h;
}

static int durchgehen(int slot, int *gefunden_out)
{
    re15_aot_t *a = &g_aot.slots[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int band = g_aot.door_params[slot].band;
    long ax = a->x, az = a->z;
    static const int dxf[5] = { 0, 1, -1, 0, 0 }, dzf[5] = { 0, 0, 0, 1, -1 };
    int gefunden = 0;
    for (int k = 0; k < 5 && !gefunden; k++) {
        long zx = ax + dxf[k] * (long)(a->half_w / 2);
        long zz = az + dzf[k] * (long)(a->half_h / 2);
        for (int d = 0; d < 16 && !gefunden; d++) {
            int yaw = d * 256;
            int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
            int32_t px = (int32_t)zx - (int32_t)((620 * c) >> 12);
            int32_t pz = (int32_t)zz + (int32_t)((620 * s) >> 12);
            pl->rot_y = (int16_t)yaw; pl->x = px; pl->z = pz;
            if (!(band & 0x80)) { re15_collision_set_band(band); pl->floor = (uint8_t)band; }
            frame(0, 0);
            g_room_change.pending = 0;
            if (pl->x == px && pl->z == pz && (int)pl->rot_y == yaw && vorwaerts_trifft(a, px, pz, yaw))
                gefunden = 1;
        }
    }
    if (gefunden_out) *gefunden_out = gefunden;
    if (!gefunden) return 0;
    for (int f = 0; f < 30; f++) {
        uint16_t e = (f == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (g_room_change.pending) return 1;
    }
    return 0;
}

static void einstieg(uint16_t raum, int seite)
{
    re15_door_seq_setze_laeufer(fang);
    s_n_flags = 1; s_flags[0][0] = 4; s_flags[0][1] = 0xF3;   /* Strom: ROOM1040 @0x0107A / 10C0 @0x00E66 / 1120 @0x00C9A */
    s_kino = 0;
    if (room_boot(raum) == 0) {
        int erledigt = 0;
        for (int i = 0; i < RE15_AOT_MAX && !erledigt; i++) {
            const re15_aot_t *a = &g_aot.slots[i];
            const re15_aot_door_params_t *d = &g_aot.door_params[i];
            if (!a->active || a->type != RE15_AOT_TYPE_DOOR || d->dest_stage != 0 || d->dest_room != 0x08
                || !(a->half_w || a->half_h)) continue;
            s_n_gefangen = 0; memset(&s_gefangen, 0, sizeof s_gefangen);
            int gefunden = 0;
            int wechsel = durchgehen(i, &gefunden);
            const re15_tuer_eigen_t *e = s_gefangen.eigen ? re15_door_seq_eigen(s_gefangen.eigen) : NULL;
            printf("Einstieg ROOM%04X Slot %d: Standplatz %d, Raumwechsel %d -> ROOM%04X, Laeufer %d x: DOOR%02X V%d %s S%03u\n",
                   raum, i, gefunden, wechsel, g_room_change.room_id, s_n_gefangen, s_gefangen.re2_nr, s_gefangen.variante,
                   e ? e->kennung : "(RE2-Datei)", s_gefangen.seite);
            if (!gefunden) continue;
            erledigt = 1;
            PRUEF(wechsel && g_room_change.room_id == 0x1080, "ROOM%04X: kein Durchgang in die Kabine", raum);
            PRUEF(s_n_gefangen == 1 && s_gefangen.re2_nr == 0x27 && s_gefangen.variante == 0 && e
                  && !strcmp(e->kennung, "P27F") && (int)s_gefangen.seite == seite,
                  "ROOM%04X -> ROOM1080: Anfrage P27F V0 S%03d fehlt/falsch", raum, seite);
            g_room_change.pending = 0;
        }
        PRUEF(erledigt, "ROOM%04X: keine begehbare Fahrstuhltuer", raum);
    }
    s_n_flags = 0;
    re15_door_seq_setze_laeufer(NULL);
}

static void teil_einstieg(void)
{
    einstieg(0x1040, 13);
    einstieg(0x10C0, 40);
    einstieg(0x1120, 57);
}

/* ---------------------------------------------------------------------------------------------
 * Teil "ausstieg"
 * ------------------------------------------------------------------------------------------ */
static void ausstieg(uint16_t kabine, int knopf, uint16_t ziel, int seite, const char *kennung)
{
    re15_door_seq_setze_laeufer(fang);
    s_kino = 1;
    if (room_boot(kabine) != 0) { s_kino = 0; return; }
    s_n_gefangen = 0; memset(&s_gefangen, 0, sizeof s_gefangen); s_pmode_beim_fang = -1;
    scd_event_fire((uint8_t)knopf);                           /* Knopf = Evt_exec sub07/08/09 (sub01 @0x00600..) */
    int bilder = 0, kino_bilder = 0;
    while (bilder < 2000 && !g_room_change.pending) {
        frame(0, 0);
        if (g_scd.player_mode == 2) kino_bilder++;
        bilder++;
    }
    const re15_tuer_eigen_t *e = s_gefangen.eigen ? re15_door_seq_eigen(s_gefangen.eigen) : NULL;
    printf("Ausstieg ROOM%04X Knopf sub%02d: %d Bilder (%d mit player_mode 2), Raumwechsel -> ROOM%04X, Laeufer %d x: "
           "DOOR%02X V%d %s S%03u T%03u, player_mode beim Feuern %d\n", kabine, knopf, bilder, kino_bilder,
           g_room_change.pending ? g_room_change.room_id : 0, s_n_gefangen, s_gefangen.re2_nr, s_gefangen.variante,
           e ? e->kennung : "(RE2-Datei)", s_gefangen.seite, s_gefangen.tuer, s_pmode_beim_fang);
    PRUEF(g_room_change.pending && g_room_change.room_id == ziel, "ROOM%04X sub%02d: kein Ausstieg nach ROOM%04X",
          kabine, knopf, ziel);
    PRUEF(s_n_gefangen == 1 && s_gefangen.archiv == RE15_DOOR_ARCHIV_RE2 && s_gefangen.re2_nr == 0x27
          && s_gefangen.tuer_nr == 0x27 && s_gefangen.variante == 0 && (int)s_gefangen.seite == seite
          && (kennung ? (e && !strcmp(e->kennung, kennung)) : s_gefangen.eigen == 0),
          "ROOM%04X sub%02d: Ausstiegs-Sequenz %s V0 S%03d fehlt/falsch", kabine, knopf, kennung ? kennung : "DOOR27", seite);
    PRUEF(s_pmode_beim_fang == 2, "ROOM%04X: Vorbedingung player_mode 2 beim Feuern nicht gemessen (%d)", kabine,
          s_pmode_beim_fang);
    g_room_change.pending = 0;
    s_kino = 0;
    re15_door_seq_setze_laeufer(NULL);
}

static void teil_ausstieg(void)
{
    /* Revier-Kabine: Knopf 1F/2F/3F = sub07/08/09 -> Aot_on 0/1/2 @0x0077C/@0x0080E/@0x008A0 */
    ausstieg(0x1080, 7, 0x1040, 27, "P27C");
    ausstieg(0x1080, 8, 0x10C0, 28, "P27C");
    ausstieg(0x1080, 9, 0x1120, 29, "P27C");
    ausstieg(0x1081, 8, 0x10C1, 28, "P27C");        /* Elza */
    /* Labor-Kabine "A-2 ELEVATOR": Aot_on 0/1/2 @0x00990/@0x00A36/@0x00ACC */
    ausstieg(0x4020, 7, 0x4000, 221, NULL);
    ausstieg(0x4020, 8, 0x5000, 222, NULL);
    ausstieg(0x4020, 9, 0x50C0, 223, NULL);
    ausstieg(0x4021, 9, 0x50C1, 223, NULL);        /* Elza */

    /* Tabelle des Moduls = die sechs Saetze (Datei-Offsets der Door_aot_set) */
    PRUEF(re15_fahrstuhl_r37_anzahl() == 6, "fahrstuhl_r37: %d Saetze statt 6", re15_fahrstuhl_r37_anzahl());

    /* Gegenprobe: eine NORMALE Tuer in player_mode 2 bleibt ohne Sequenz (Sperre der Runde 31) */
    re15_door_seq_setze_laeufer(fang);
    s_n_flags = 1; s_flags[0][0] = 4; s_flags[0][1] = 0xF3;
    s_kino = 0;
    if (room_boot(0x10C0) == 0) {
        int slot = -1;
        for (int i = 0; i < RE15_AOT_MAX; i++)
            if (g_aot.slots[i].active && g_aot.slots[i].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[i].dest_room == 0x08
                && g_aot.slots[i].half_w) slot = i;
        PRUEF(slot >= 0, "ROOM10C0: Fahrstuhltuer-Platz fehlt");
        if (slot >= 0) {
            s_n_gefangen = 0;
            g_scd.player_mode = 2; g_scd.letterbox_countdown = -1;
            re15_aot_fire_slot(slot);
            re15_door_seq_ausfuehren();
            printf("Gegenprobe ROOM10C0 Platz %d in player_mode 2: Raumwechsel %d, Laeufer %d x (Sperre der Runde 31)\n",
                   slot, g_room_change.pending, s_n_gefangen);
            PRUEF(g_room_change.pending && s_n_gefangen == 0, "Gegenprobe: normale Tuer spielt in player_mode 2 eine Sequenz");
            g_room_change.pending = 0;
            g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
        }
    }
    s_n_flags = 0;
    re15_door_seq_setze_laeufer(NULL);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "archiv";
    if (!strcmp(teil, "archiv")) teil_archiv();
    else if (!strcmp(teil, "einstieg")) teil_einstieg();
    else if (!strcmp(teil, "ausstieg")) teil_ausstieg();
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf("%s: %s (%d Fehler)\n", teil, g_fehler ? "ROT" : "GRUEN", g_fehler);
    return g_fehler ? 1 : 0;
}
