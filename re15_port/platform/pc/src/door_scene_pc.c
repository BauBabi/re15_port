/* door_scene_pc.c — die RE2-TUERSEQUENZ auf dem PC: Bildschleife, Zeichnen, Blende, Ton, Takt.
 *
 * ⛔ RE2-ERGAENZUNG (Beta -> Retail), Begruendung im Kopf von include/re15_door_seq.h.
 * Die Rechnung (Skripte, Objekte, Matrizen, Blendenpegel) liegt plattformfrei in
 * engine/src/door_seq_common.c; hier nur, was die RE2-Hauptschleife um Door_move herum tut.
 * Belege: analysis/tor_1170/08_re_bildtakt.md, 08_re_blende.md, 08_re_ton.md, 08_re_zeichnen.md.
 *
 * Ablauf wie RE2 (FUN_80026b7c + Hauptschleife):
 *   1. Abdunkeln des stehenden Bildes: Ladezweig der Hauptschleife, Kanal 0 Pegel 31, je
 *      Durchlauf (8,8,8) subtraktiv auf das NICHT neu gezeichnete Bild (@0x8002b3a4..b0,
 *      @0x8002b3c4..3fc), bis der Pegel unter 0 faellt = 32 Bilder, VSync(0) (@0x8002b424).
 *   2. Door_move (FUN_80013eb4) je Durchlauf: Skripte, Objekte, Bildzaehler; danach
 *      Blenden-Takt (@0x8002b448) und Bildwechsel mit VSync(0) (@0x8002b450, @0x8002b998).
 *   3. Door_exit (0x8001417c): warten, bis Kanal 0 fertig ist (je Durchlauf nur der schwarze
 *      Hintergrund), dann Pegel 0x7fff (schwarz) und - wenn ein Objekt Flag 0x800 trug - Ton 1.
 *   Danach uebernimmt die RE1.5-Einblendung des Raums (re15_room_transition_present, 255 ...),
 *   die nahtlos an Schwarz anschliesst. Sie bleibt RE1.5: das System ist dort vollstaendig.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

#include "re15_door_seq.h"
#include "re15_engine.h"
#include "re15_light.h"
#include "re15_math.h"
#include "re15_audio.h"
#include "re15_tim.h"
#include "re15_fade.h"
#include "asset_root_pc.h"   /* re15_pc_read_re2: shared_assets/RE2/DOOR (Runde 31) */

extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
extern void re15_render_pc_bind_tim_slot(int slot);
extern void re15_render_pc_clear_scene_overlays(void);
extern void re15_render_pc_title_fade_sub(int b);
extern int  re15_render_pc_standbild_wiederholen(void);
extern void re15_render_pc_request_readback(const char *path);

/* TIM-Platz der Tuertextur: 24 ist frei (platform/pc/main.c "RESERVED/unused",
 * analysis/tor_1170/05_port_anschluss.md 2.1). */
#define TUER_TIM_SLOT 24

/* Tuerlicht RE2 (08_re_zeichnen.md 3): Lichtmatrix @0x8009a470, Farbmatrix @0x8009a490
 * (neunmal 0x0640 = 1600), Hintergrundfarbe 68 bzw. 136 bei Flag 0x1000 (@0x800142ac..308). */
static const int16_t L_TUER[9] = { 400, 800, -500,  -1800, -1000, -2700,  3500, 6700, 1200 };
#define LCM_TUER 1600

/* ===========================================================================
 * Takt: virtueller Vcount mit NTSC 59,826 Hz (psx-spx graphicsprocessingunitgpu.md:1264).
 * VSync(0) = auf die naechste Austastung nach dem Aufruf warten (@0x80085efc..f58,
 * 08_re_bildtakt.md 1). Ein SDL_GetTicks-Deckel trifft die Rate nicht (16 ms = 62,5 fps).
 * ======================================================================== */
static uint64_t s_t0, s_frq;

static uint64_t vcount(void)
{
    uint64_t d = SDL_GetPerformanceCounter() - s_t0;
    return (d * 59826u) / (s_frq * 1000u);
}

static void vsync0(void)
{
    uint64_t ziel = vcount() + 1;
    for (;;) {
        uint64_t jetzt = vcount();
        if (jetzt >= ziel) break;
        /* Restzeit bis zur naechsten Austastung; grob schlafen, fein warten */
        uint64_t rest_ms = ((ziel - jetzt) * 1000u) / 60u;
        if (rest_ms > 2) SDL_Delay(1);
    }
}

/* ===========================================================================
 * Zeichnen eines Tuerobjekts (FUN_80014234 Schritte e/f + FUN_8001468c), Rezept
 * 08_re_zeichnen.md 4.2.
 * ======================================================================== */
static int32_t sat16(int32_t v) { return v < -0x8000 ? -0x8000 : (v > 0x7fff ? 0x7fff : v); }

typedef struct { int sx, sy; uint16_t sz; } ecke_t;

/* RTPT (sf=1) mit RT = W, TR = T: IR = sat((TR<<12 + R*V) >> 12), SZ3 = sat(MAC3>>12, 0..0xffff),
 * SX = (OFX + IR1 * n) >> 16 mit n = Division(H, SZ3), OFX/OFY = 160/120 (03 3.2 geerbt). */
static ecke_t projizieren(const re15_door_mat_t *w, const re15_md1_vertex_t *v)
{
    ecke_t e;
    int32_t ir[3], mac3 = 0;
    for (int i = 0; i < 3; i++) {
        int32_t s = (int32_t)w->m[i * 3 + 0] * v->x + (int32_t)w->m[i * 3 + 1] * v->y
                  + (int32_t)w->m[i * 3 + 2] * v->z;
        int32_t mac = (s >> 12) + w->t[i];
        ir[i] = sat16(mac);
        if (i == 2) mac3 = mac;
    }
    uint32_t sz = mac3 < 0 ? 0u : (mac3 > 0xffff ? 0xffffu : (uint32_t)mac3);
    uint32_t n = re15_gte_divide(RE15_DOOR_H, sz);
    int64_t sx = ((int64_t)160 << 16) + (int64_t)ir[0] * n;
    int64_t sy = ((int64_t)120 << 16) + (int64_t)ir[1] * n;
    e.sx = (int)(sx >> 16);
    e.sy = (int)(sy >> 16);
    if (e.sx < -0x400) e.sx = -0x400; if (e.sx > 0x3ff) e.sx = 0x3ff;
    if (e.sy < -0x400) e.sy = -0x400; if (e.sy > 0x3ff) e.sy = 0x3ff;
    e.sz = (uint16_t)sz;
    return e;
}

/* Ein Mesh mit Objektmatrix welt (Licht mit welt_vor) zeichnen - FUN_80014234 e/f + FUN_8001468c. */
static void mesh_zeichnen(const re15_md1_mesh_t *m, const re15_door_mat_t *welt,
                          const re15_door_mat_t *welt_vor, uint16_t flags, int *lfd)
{
    /* Licht: LLM = L * W des VORIGEN Bildes (Schritt e liest obj+84, bevor Schritt i es neu
     * schreibt - 08_re_zeichnen.md 1.3), BK 68/136, LCM 1600. */
    re15_actor_lightctx_t w, ctx;
    memset(&w, 0, sizeof w);
    for (int i = 0; i < 3; i++)
        for (int k = 0; k < 3; k++) {
            w.L[i][k] = L_TUER[i * 3 + k];
            w.C[i][k] = LCM_TUER;
        }
    uint8_t bk = (flags & 0x1000) ? 136 : 68;
    w.ambient[0] = w.ambient[1] = w.ambient[2] = bk;
    w.active_lights = 3;
    int32_t wv[9];
    for (int i = 0; i < 9; i++) wv[i] = welt_vor->m[i];
    re15_light_ctx_rotate_for_bone(&w, wv, &ctx);

    int platz = 0;   /* Flags & 0xc0 == 0: Platz des vorigen Dreiecks (@0x800149e0) */
    for (int t = 0; t < m->triangle_count; t++) {
        const re15_md1_triangle_t *tr = &m->triangles[t];
        if (tr->v0 >= m->tri_vertex_count || tr->v1 >= m->tri_vertex_count || tr->v2 >= m->tri_vertex_count)
            continue;
        ecke_t e0 = projizieren(welt, &m->tri_vertices[tr->v0]);
        ecke_t e1 = projizieren(welt, &m->tri_vertices[tr->v1]);
        ecke_t e2 = projizieren(welt, &m->tri_vertices[tr->v2]);
        /* NCLIP: gezeichnet bei MAC0 >= 0 (@0x800148d0 / @0x800148f8 bgez) */
        int64_t mac0 = (int64_t)e0.sx * e1.sy + (int64_t)e1.sx * e2.sy + (int64_t)e2.sx * e0.sy
                     - (int64_t)e0.sx * e2.sy - (int64_t)e1.sx * e0.sy - (int64_t)e2.sx * e1.sy;
        if (mac0 < 0) continue;
        /* NCCT mit den drei Eckennormalen (@0x80014958) */
        uint8_t c[3][3];
        const uint16_t ni[3] = { tr->n0, tr->n1, tr->n2 };
        for (int k = 0; k < 3; k++) {
            if (ni[k] < m->tri_normal_count) {
                const re15_md1_vertex_t *nv = &m->tri_normals[ni[k]];
                re15_light_shade_vertex(&ctx, nv->x, nv->y, nv->z, &c[k][0], &c[k][1], &c[k][2]);
            } else {
                c[k][0] = c[k][1] = c[k][2] = 0x80;
            }
        }
        /* AVSZ3 mit ZSF3 = 341 (@0x8008d29c), verworfen bei otz < 64 (@0x800149a8/ac) */
        int32_t otz = (341 * ((int32_t)e0.sz + e1.sz + e2.sz)) >> 12;
        if ((otz >> 6) == 0) continue;
        /* Ordnungstabelle (08_re_zeichnen.md 2.2): 0x80 -> (otz>>7)+511, 0xc0 -> otz>>7,
         * 0x40 -> eigene 16er-Tabelle, vor allem gezeichnet. Port-Schluessel: groesser = frueher
         * gezeichnet; innerhalb eines Platzes das SPAETER eingehaengte zuerst (addPrim vorn). */
        switch (flags & 0xC0) {
        case 0x80: platz = (otz >> 7) + 511; break;
        case 0xC0: platz = otz >> 7; break;
        case 0x40: platz = 1024 + (otz >> 12); break;
        default: break;
        }
        int z = platz * 4096 + ((*lfd)++ & 0xfff);
        const re15_md1_tri_uv_t *uv = &m->triangle_uvs[t];
        int pxo = (int)((uv->page & 0x000F) * 128);
        re15_render_textured_tri_lit(e0.sx, e0.sy, (int)uv->u0 + pxo, (int)uv->v0,
                                     e1.sx, e1.sy, (int)uv->u1 + pxo, (int)uv->v1,
                                     e2.sx, e2.sy, (int)uv->u2 + pxo, (int)uv->v2,
                                     (int)uv->page, (int)uv->clut, z,
                                     c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2],
                                     c[2][0], c[2][1], c[2][2]);
    }
}

/* ===========================================================================
 * Griff-Tausch (Runde 31, analysis/befunde_runde31/tueren_02_re2.md 2.5 und 3).
 * ⛔ PORT-WAHL: RE2 tauscht Griffe nur archivintern per Bit 7 (DOOR01/DOOR05, selber
 * Anhaengepunkt, selbe Textur). Wo RE1.5 eine andere Griff-FORM malt als das Archiv (T027 DOOR13
 * mit Druecker, T033/T034 DOOR09 mit langem Stangengriff), zeichnet der Port das Griff-Objekt des
 * Archivs mit dem Griff-Mesh des Spenders (DOOR07 Druecker flach, DOOR04 Stangengriff lang):
 *   - am Anhaengepunkt des ARCHIVS (Lage des Objekts aus dessen Skript),
 *   - mit Textur und CLUT des SPENDERS (Beschlagstreifen v 219..255 liegt in dessen TIM, eigene
 *     256er-CLUT - der Spender-TIM liegt dafuer in TIM-Platz 25),
 *   - Grund-Drehung des Spendergriffs (vorn / hinten), Zeitverlauf der Griffbewegung des Archivs,
 *     Ausschlag des Spenders (Knauf 1500 -> Druecker -702, Stange 0; Werte [SIM] in
 *     gen/tuer_zuordnung.inc). So kippt der Druecker, statt sich wie ein Knauf zu drehen.
 * ======================================================================== */
#define SPENDER_TIM_SLOT 25   /* frei: platform/pc/main.c RE15_TIM_SLOT_WPN_GUN "RESERVED/unused" */

typedef struct {
    const re15_griff_tausch_t *gt;
    re15_md1_t md1;
    int ok;
    re15_door_mat_t vor[RE15_DOOR_OBJEKTE];   /* Matrix des vorigen Bildes (Licht) */
} griff_tausch_t;

static int griff_getauscht(const griff_tausch_t *g, const re15_door_obj_t *o)
{
    return g && g->ok && o->eltern >= 0 && o->mesh == g->gt->mesh_archiv;
}

static void griff_zeichnen(const re15_door_seq_t *s, griff_tausch_t *g, int oi, int *lfd)
{
    const re15_door_obj_t *o = &s->obj[oi];
    if (g->gt->mesh_spender >= (unsigned)g->md1.mesh_count) return;
    int hinten = o->pos[0] < 0;                                  /* x -130 = Rueckseite */
    const uint16_t *basis = hinten ? g->gt->rot_hinten : g->gt->rot_vorn;
    int32_t delta = (int16_t)(uint16_t)(o->rot[0] - o->rot0[0]);  /* Griffbewegung des Archivs */
    int32_t d2 = g->gt->aus_archiv ? delta * g->gt->aus_spender / g->gt->aus_archiv : 0;
    uint16_t rot[3] = { (uint16_t)(basis[0] + d2), basis[1], basis[2] };
    re15_door_mat_t welt;
    re15_door_seq_objmatrix(&s->obj[o->eltern].welt, rot, o->pos, &welt);
    re15_render_pc_bind_tim_slot(SPENDER_TIM_SLOT);
    mesh_zeichnen(&g->md1.meshes[g->gt->mesh_spender], &welt, &g->vor[oi], o->flags, lfd);
    re15_render_pc_bind_tim_slot(TUER_TIM_SLOT);
    g->vor[oi] = welt;
}

static void objekt_zeichnen(const re15_door_seq_t *s, int oi, int *lfd, griff_tausch_t *g)
{
    const re15_door_obj_t *o = &s->obj[oi];
    if (!o->on) return;
    if (griff_getauscht(g, o)) { griff_zeichnen(s, g, oi, lfd); return; }
    if (!s->md1_ok || o->mesh >= (unsigned)s->md1.mesh_count) return;
    mesh_zeichnen(&s->md1.meshes[o->mesh], &o->welt, &o->welt_vor, o->flags, lfd);
}

/* Serienabzug zur Abnahme: RE15_TUER_SERIE=<Verzeichnis> legt jedes Bild als PPM ab. Der
 * Rueckleser liest VOR dem Present (render_pc.c, s_readback_pending) - danach ist der
 * Backbuffer undefiniert. Also vor re15_render_end_frame() anfordern. */
static void abzug(const char *serie, const char *art, int n)
{
    if (!serie || !*serie) return;
    char p[512];
    snprintf(p, sizeof p, "%s/%s_%03d.ppm", serie, art, n);
    re15_render_pc_request_readback(p);
}

/* ===========================================================================
 * RE2-Archiv aus shared_assets/RE2/DOOR (Runde 31): Datei UNVERAENDERT, Aufteilung nach der
 * EXE-Tabelle @0x8009a520 (gen/re2_tuer_tabelle.inc): Tonteil = Datei[0 .. Tonteil) (Tonlader
 * FUN_80014cd0 @0x80014d94), Modellteil = Datei + Sektor * 0x800 (Lader FUN_80015064
 * @0x800150b0 lw t2,4(v1) / @0x800150f4 lhu v1,2(v1)).
 * ======================================================================== */
typedef struct {
    uint8_t       *datei;
    int            n;
    const uint8_t *modell; int n_modell;
    const uint8_t *ton;    int n_ton;
} re2_archiv_t;

static int re2_archiv_lesen(int nr, re2_archiv_t *a)
{
    memset(a, 0, sizeof *a);
    int ton = 0, modell = 0, sektor = 0, groesse = 0;
    if (re15_door_seq_re2_archiv(nr, &ton, &modell, &sektor, &groesse) != 0) return -1;
    char rel[32];
    snprintf(rel, sizeof rel, "DOOR/DOOR%02X.DO2", nr);
    int n = 0;
    uint8_t *d = re15_pc_read_re2(rel, &n);
    if (!d) {
        fprintf(stderr, "[tuer] shared_assets/RE2/%s fehlt\n", rel);
        return -1;
    }
    if (n != groesse || sektor * 0x800 + modell != n || ton > sektor * 0x800) {
        fprintf(stderr, "[tuer] %s: %d B passt nicht zur Tabelle @0x8009a520 (%d B)\n", rel, n, groesse);
        free(d);
        return -1;
    }
    a->datei = d; a->n = n;
    a->modell = d + sektor * 0x800; a->n_modell = modell;
    a->ton = d; a->n_ton = ton;
    return 0;
}

static uint32_t rd32le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Pruefhaken RE15_TUER_BOGEN=<Verzeichnis> (Runde 31): statt der ganzen Serie nur zwei Bilder
 * je Sequenz, <Sxxx>_anfang.ppm (Tuer zu, nach dem Einblenden) und <Sxxx>_mitte.ppm (Oeffnen).
 * Welche Bilder, bestimmt ein Trockenlauf derselben Maschine an Objekt 0 (Blatt bzw. Leiter,
 * Klappe, Plattform): Mitte = das Bild, in dem die aufsummierte Drehung von Objekt 0 die Haelfte
 * ihrer Summe erreicht (Drehung unter 256 oder keine: Lage y/z; Lage x ist die Kamerafahrt zur
 * Tuer hin). Anfang =
 * vor der ersten Bewegung irgendeines Objekts nach dem Einblenden (Bild 16..20). */
static int32_t absdiff16(uint16_t a, uint16_t b) { int32_t d = (int16_t)(uint16_t)(a - b); return d < 0 ? -d : d; }

static void bogen_bilder(const uint8_t *teil, int n, const re15_door_seq_anfrage_t *a, int *anfang, int *mitte)
{
    static re15_door_seq_t t;
    static int32_t kum_rot[4000], kum_pos[4000];
    *anfang = 20; *mitte = 120;
    if (re15_door_seq_start(&t, teil, n, a->variante, a->bit7 ? 0x80 : 0, a->tuer_nr) != 0) return;
    int32_t alt[RE15_DOOR_OBJEKTE][7];
    memset(alt, 0, sizeof alt);
    uint16_t r_alt[3] = {0, 0, 0};
    int32_t y_alt = 0, z_alt = 0, sr = 0, sp = 0;
    int bild = 0, ea = -1, la = -1;
    while (bild < 4000 && re15_door_seq_bild(&t, 1)) {
        const re15_door_obj_t *o = &t.obj[0];
        if (bild > 16 && o->on) {
            sr += absdiff16(o->rot[0], r_alt[0]) + absdiff16(o->rot[1], r_alt[1]) + absdiff16(o->rot[2], r_alt[2]);
            sp += (o->pos[1] > y_alt ? o->pos[1] - y_alt : y_alt - o->pos[1])
                + (o->pos[2] > z_alt ? o->pos[2] - z_alt : z_alt - o->pos[2]);
        }
        memcpy(r_alt, o->rot, sizeof r_alt); y_alt = o->pos[1]; z_alt = o->pos[2];
        kum_rot[bild] = sr; kum_pos[bild] = sp;
        for (int i = 0; i < RE15_DOOR_OBJEKTE; i++) {
            const re15_door_obj_t *q = &t.obj[i];
            int32_t w[7] = { q->on, q->pos[0], q->pos[1], q->pos[2], q->rot[0], q->rot[1], q->rot[2] };
            if (bild > 16 && memcmp(w, alt[i], sizeof w) != 0) { if (ea < 0) ea = bild; la = bild; }
            memcpy(alt[i], w, sizeof w);
        }
        bild++;
    }
    re15_door_seq_ende(&t);
    /* kleine Drehungen (Wackeln einer Schiebetuer) zaehlen nicht als Oeffnen: ab 256 = 22,5 Grad */
    int drehen = sr >= 256;
    const int32_t *kum = drehen ? kum_rot : (sp > 0 ? kum_pos : (sr > 0 ? kum_rot : NULL));
    int32_t summe = drehen ? sr : (sp > 0 ? sp : sr);
    if (kum) {
        for (int b = 0; b < bild; b++) if (kum[b] * 2 >= summe) { *mitte = b; break; }
    } else if (ea > 0) {
        *mitte = (ea + la) / 2;
    }
    if (ea > 0) *anfang = ea - 1 < 20 ? ea - 1 : 20;
    if (*anfang < 16) *anfang = 16;
}

static void tuer_laeufer(const re15_door_seq_anfrage_t *a)
{
    static re15_door_seq_t s;
    static griff_tausch_t g;
    re2_archiv_t arch, spend;
    memset(&arch, 0, sizeof arch);
    memset(&spend, 0, sizeof spend);
    int re2 = (a->archiv == RE15_DOOR_ARCHIV_RE2);
    int n = 0;
    const uint8_t *teil = NULL;
    if (re2) {
        if (re2_archiv_lesen(a->re2_nr, &arch) == 0) { teil = arch.modell; n = arch.n_modell; }
    } else {
        teil = re15_door_seq_archiv(a->archiv, &n);
    }
    /* var 12 = Variante, var 14 = Bit 7, var 15 = Tuernummer (Door_init @0x80013e5c..98) */
    if (!teil || re15_door_seq_start(&s, teil, n, a->variante, a->bit7 ? 0x80 : 0, a->tuer_nr) != 0) {
        fprintf(stderr, "[tuer] Archiv %d/%02X nicht lesbar -> Tuerwechsel ohne Sequenz\n", a->archiv, a->re2_nr);
        free(arch.datei);
        return;
    }
    const char *serie = getenv("RE15_TUER_SERIE");
    const char *bogen = getenv("RE15_TUER_BOGEN");
    int schnell = getenv("RE15_TUER_SCHNELL") != NULL;   /* Pruefhaken: ohne VSync-Takt */
    fprintf(stderr, "[tuer] Sequenz Archiv %d DOOR%02X Variante %d Bit7 %d Tuer %u Seite S%03u T%03u "
                    "Spender %02X (%d Skripte)\n",
            a->archiv, re2 ? a->re2_nr : 0x2E, a->variante, a->bit7, a->tuer_nr, a->seite, a->tuer,
            a->spender, s.n_skripte);
    if (s.tim_ok) re15_render_pc_upload_tim_slot(&s.tim, TUER_TIM_SLOT);

    /* Griff-Tausch (PORT-WAHL, s.o.): Spender-Archiv lesen, MD1 + TIM daraus */
    memset(&g, 0, sizeof g);
    if (re2 && a->spender != RE15_DOOR_KEIN_SPENDER) {
        g.gt = re15_door_seq_griff_tausch(a->re2_nr, a->spender);
        if (g.gt && re2_archiv_lesen(a->spender, &spend) == 0 && spend.n_modell > 8) {
            uint32_t md1_rel = rd32le(spend.modell), tim_rel = rd32le(spend.modell + 4);
            re15_tim_t tim;
            if (md1_rel < tim_rel && (int)tim_rel < spend.n_modell
                && re15_md1_parse(spend.modell + md1_rel, (int)(tim_rel - md1_rel), &g.md1) == 0
                && re15_tim_parse(spend.modell + tim_rel, spend.n_modell - (int)tim_rel, &tim) == 0) {
                re15_render_pc_upload_tim_slot(&tim, SPENDER_TIM_SLOT);
                g.ok = 1;
            }
        }
        fprintf(stderr, "[tuer] Griff-Tausch DOOR%02X <- DOOR%02X: %s\n", a->re2_nr, a->spender,
                g.ok ? "Spender-Griff geladen" : "Spender NICHT lesbar -> Archiv-Griff");
    }

    int b_anfang = -1, b_mitte = -1;
    char bogen_pfad[512];
    if (bogen && *bogen) {
        bogen_bilder(teil, n, a, &b_anfang, &b_mitte);
        fprintf(stderr, "[tuer] Bogen-Bilder S%03u: Anfang Bild %d, Mitte Bild %d\n", a->seite, b_anfang, b_mitte);
    }

    uint8_t balken = g_letterbox_level;
    /* Tonteil laden ("DOOR SOUND", FUN_80014cd0) - im Port synchron, also vor dem ersten
     * Door_move-Durchlauf FERTIG. var 13 heisst in RE2 "Laden laeuft" und faellt, sobald das
     * Ladebit faellt (@0x80013f54..68) - unabhaengig davon, ob der Ton spielbar ist. Darum wird
     * der Maschine immer "geladen" gemeldet; ohne Ton (kein Audiogeraet, RE15_NOAUDIO, Datei
     * fehlt) laeuft die Sequenz stumm statt im Warteskript (Skript 4) haengenzubleiben.
     * Runde 31: RE2-Archive spielen den Tonteil IHRER Datei (Tonfamilie des Archivs, tueren_02_re2.md
     * 4), das Tor weiter TORSE.VBS. */
    int ton_ok = re2 ? re15_audio_re2_tuer_laden(arch.ton, arch.n_ton) : re15_audio_re2_tor_laden();
    int takt = 0;   /* re15_audio_tick nur jedes zweite Bild = 30-Hz-Spieltakt, s.u. */

    s_frq = SDL_GetPerformanceFrequency();
    s_t0 = SDL_GetPerformanceCounter();

    /* 1. Abdunkeln des stehenden Bildes: Pegel 31..0 = 32 Bilder, je Bild 8 mehr abgezogen */
    for (int k = 1; k <= 32; k++) {
        re15_render_begin_frame();
        re15_render_pc_standbild_wiederholen();
        re15_render_pc_title_fade_sub(k * 8 > 255 ? 255 : k * 8);
        abzug(serie, "a_abdunkeln", k);
        re15_render_end_frame();
        re15_audio_se_pumpe();
        if (takt++ & 1) re15_audio_tick();
        if (!schnell) vsync0();
    }
    /* Balken sind waehrend der Tuer gesperrt (0x800cfb74 & 0x4000 sperrt den Takt 0x8002c378,
     * @0x8002c588; 08_re_blende 5.3). Das Abdunkeln davor zeigt den stehenden Puffer samt
     * etwaiger Balken - deshalb erst hier auf 0. */
    g_letterbox_level = 0;

    /* 2. Door_move: je Durchlauf Skripte + Objekte, dann Blenden-Takt, dann Bildwechsel */
    int bild = 0, n_ton0 = 0;
    /* Deckel: die laengste RE2-Sequenz hat 451 Bilder (03 K19); 4000 faengt nur Datenfehler */
    while (bild < 4000 && re15_door_seq_bild(&s, 1)) {
        for (int i = 0; i < s.n_ton; i++)
            if (ton_ok && s.ton[i].vab == 0) {                         /* Se_on vab 0 = Tuerbank */
                if (re2) re15_audio_re2_tuer_se(s.ton[i].se);
                else     re15_audio_re2_tor_se(s.ton[i].se);
                fprintf(stderr, "[tuer] Bild %d Se_on Tonkopf-Eintrag %d\n", bild, s.ton[i].se);
                n_ton0++;
            }
        re15_render_begin_frame();
        re15_render_pc_clear_scene_overlays();       /* keine Raumdreiecke, keine Raummasken */
        re15_render_pc_bind_tim_slot(TUER_TIM_SLOT);
        int lfd = 0;
        for (int oi = 0; oi < RE15_DOOR_OBJEKTE; oi++) objekt_zeichnen(&s, oi, &lfd, &g);
        int h = re15_door_seq_blende_takt(&s);
        re15_render_pc_title_fade_sub(h < 0 ? 0 : h);
        abzug(serie, "b_tuer", bild);
        if (bogen && *bogen && (bild == b_anfang || bild == b_mitte)) {
            snprintf(bogen_pfad, sizeof bogen_pfad, "%s/S%03u_%s.ppm", bogen, a->seite,
                     bild == b_anfang ? "anfang" : "mitte");
            re15_render_pc_request_readback(bogen_pfad);
        }
        re15_render_end_frame();
        re15_audio_se_pumpe();
        if (takt++ & 1) re15_audio_tick();
        if (!schnell) vsync0();
        bild++;
    }

    /* 3. Door_exit: warten, bis Kanal 0 fertig ist; in diesen Durchlaeufen keine Tuerobjekte */
    int warte = 0;
    while (!re15_door_seq_blende_fertig(&s) && warte < 600) {
        re15_render_begin_frame();
        re15_render_pc_clear_scene_overlays();
        int h = re15_door_seq_blende_takt(&s);
        re15_render_pc_title_fade_sub(h < 0 ? 0 : h);
        abzug(serie, "c_ende", warte);
        re15_render_end_frame();
        re15_audio_se_pumpe();
        if (takt++ & 1) re15_audio_tick();
        if (!schnell) vsync0();
        warte++;
    }
    re15_door_seq_blende_schwarz(&s);                          /* @0x800141a4..c4: Pegel 0x7fff */
    if (s.schliesston && ton_ok) {                             /* @0x800141d8..f4: Ton 1 */
        if (re2) re15_audio_re2_tuer_se(1);
        else     re15_audio_re2_tor_se(1);
    }
    re15_audio_se_pumpe();
    fprintf(stderr, "[tuer] Sequenz fertig: %d Bilder + %d Warten, %d Se_on, Schliesston %d, Ton %s, "
                    "Notizen 0x%x%s\n", bild, warte, n_ton0, s.schliesston, ton_ok ? "geladen" : "STUMM",
            s.notizen, (b_anfang >= 0) ? " (Bogen-Bilder geschrieben)" : "");

    re15_render_pc_title_fade_sub(0);
    g_letterbox_level = balken;
    re15_door_seq_ende(&s);
    free(arch.datei);
    free(spend.datei);
    memset(&g, 0, sizeof g);
}

void re15_pc_tuerszene_anmelden(void)
{
    re15_door_seq_setze_laeufer(tuer_laeufer);
}
