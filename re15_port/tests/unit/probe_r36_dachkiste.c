/*
 * probe_r36_dachkiste — Runde 36 Spur R: kann eine Typ-4-Kiste DURCH SCHIEBEN MIT DEM SPIELER eine
 * Absprungzelle (Objekt-Pass @0x8002c120-134: FUN_8003b7f0(&obj+0x34, 0x1c2, obj+0x82) & 2) erreichen?
 *
 * Die Freiraum-Zensus der Spur A (probe_r36_objfall.c) flutet nur die KISTE (FUN_8003b558-Sperre Klasse 2,
 * @0x8003b64c-660 u0 & 2, Rechteck um r = hx @0x8003b68c-690). Sie laesst den Spieler weg — fuer ROOM1090
 * obj 1 meldete sie deshalb "8 Absprungzellen erreichbar". Diese Sonde nimmt den Spieler hinzu, mit den
 * Praedikaten der Schubkette FUN_8002bd44 (selbst disassembliert, Dossier R_dachkiste.md B2):
 *   - Schub nur im Spielerband (@0x8002bf38), nur bei Reichweite FUN_8002d1e8 (Punkt 563 voraus, |dx|,|dz|
 *     <= 900 zur Kistenmitte, @0x8002d204/@0x8002d224), Kiste = Spieler +- (hx + 450) auf der Achse mit dem
 *     groesseren |d| (FUN_8002cabc a2=1, @0x8002cce8-d10, @0x8002cd30-44 / @0x8002cd98-ac) -> die Kiste
 *     wandert immer VOM SPIELER WEG; der Spieler muss auf der Gegenseite stehen.
 *   - Der Spieler selbst: Wandklemme Radius 450 mit Maske 1 (u0 & 1, collision_constrain_impl), und er wird
 *     jedes Bild aus der Kiste geschoben (@0x8002c0d8-e8, Summe hx + 450 je Achse).
 * Die Sonde ueberschaetzt die Freiheit des Spielers absichtlich (notwendige Bedingung): Wandradius 432 statt
 * 450 (+18 Haut), Beruehrung erlaubt, Schub-Stellung axial 1350..1463 (Reichweite 900 + 563) und seitlich bis
 * +-1238 (Blick bis 37 Grad schraeg), Kisten-Hindernis fuer den Spieler um die halbe Zellbreite verkleinert.
 * Findet sie KEINE erreichbare Absprungzelle, ist der Fall durch Schieben ausgeschlossen (a fortiori).
 *
 * Verfahren: Koordinaten-Kompression fuer die Kiste (Zellkanten +-450 und +-r, Raster 450) -> Elementarzellen,
 * auf denen Sperre und Ausloeser konstant sind. Je Kistenzelle wird der Freiraum des Spielers (Wandzellen
 * +-432, Kistenquadrat) exakt komprimiert geflutet und die vier Schub-Seiten (N/S/O/W) beschriftet; Zustand
 * der Suche = (Kistenzelle, Seite), Uebergang = Schub von dieser Seite (Kiste eine Zelle weg vom Spieler,
 * Spieler bleibt auf seiner Seite) oder Seitenwechsel (gleiche Beschriftung = verbunden).
 *
 * Aufruf: probe_r36_dachkiste diag | pin
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_climb.h"
#include "re15_collision.h"

extern scd_vm_t g_scd;

static int g_fail = 0;
#define PRUEF(cond, ...) do { if (!(cond)) { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); g_fail++; } } while (0)

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

static re15_rdt_t    s_rdt;
static re15_actor_t *s_pl;

static int raum(const char *stage, const char *room, uint16_t room_id)
{
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/%s/%s.RDT", RE15_ASSET_PSX_DIR, stage, room);
    uint8_t *raw = read_file(path, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", path); return 77; }
    if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("SKIP: RDT-Parse %s\n", room); return 77; }
    re15_actor_init();
    scd_vm_init();
    re15_climb_reset();
    g_current_room_id = room_id;
    g_room_change.pending = 0;
    s_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_pl->active = 1; s_pl->type = 0; s_pl->hp = 100;
    s_pl->x = 0; s_pl->y = 0; s_pl->z = 0; s_pl->rot_y = 0;
    scd_room_reenter(&s_rdt, s_pl->x, s_pl->z, 0);
    re15_collision_set_band(0);
    return 0;
}

/* ---- Konstanten der Schubkette (Belege im Kopf) ------------------------------------------------------- */
#define PR_SPIELER   450      /* FUN_8003b0a4 Spielerradius (Wandklemme)                                  */
#define HAUT          18      /* push_rect +0x12                                                          */
#define REICH        900      /* FUN_8002d1e8 @0x8002d224 `ori a3,zero,0x384`                              */
#define VORAUS       563      /* FUN_8002d1e8 @0x8002d204 `ori v0,zero,0x233`                              */
#define RAST         450      /* Zusatzraster der Kisten-Kompression (Feinheit, kein Verhaltenswert)        */

static int cmp_i32(const void *a, const void *b)
{
    const int32_t x = *(const int32_t *)a, y = *(const int32_t *)b;
    return (x > y) - (x < y);
}
static int uniq(int32_t *v, int n)
{
    qsort(v, (size_t)n, sizeof v[0], cmp_i32);
    int m = 0;
    for (int i = 0; i < n; i++) if (m == 0 || v[i] != v[m - 1]) v[m++] = v[i];
    return m;
}
static int idx_of(const int32_t *v, int n, int32_t x)
{
    int lo = 0, hi = n - 2;
    if (x < v[0] || x >= v[n - 1]) return -1;
    while (lo < hi) { int mid = (lo + hi + 1) / 2; if (v[mid] <= x) lo = mid; else hi = mid - 1; }
    return lo;
}

/* ---- Spieler-Freiraum bei gegebener Kistenlage: Seiten beschriften ------------------------------------
 * Seiten: 0 = Spieler WESTLICH (schiebt nach +X), 1 = OESTLICH (-X), 2 = NOERDLICH (schiebt nach +Z),
 * 3 = SUEDLICH (-Z). Rueckgabe label[4] (-1 = keine freie Schubstellung), label[4] = Komponente der
 * Startlage (nur beim Start gebraucht, sonst -1 bei px == INT32_MIN). */
typedef struct { int32_t cx, cz, hx, hz, halb; int band; } kiste_t;

static int32_t s_px_buf[4096], s_pz_buf[4096];

static void seiten_labels(const re15_rdt_t *rdt, const kiste_t *k, int32_t sx, int32_t sz, int label[5])
{
    const int32_t rp = PR_SPIELER - HAUT;                    /* grosszuegig: 432                            */
    const int32_t hind_x = k->hx + PR_SPIELER - 1 - k->halb;  /* Kistenquadrat fuer den Spieler, verkleinert */
    const int32_t hind_z = k->hz + PR_SPIELER - 1 - k->halb;
    const int32_t ax0 = k->hx + PR_SPIELER - k->halb;          /* Schubstellung axial: Beruehrung ...        */
    const int32_t ax1 = k->hx + PR_SPIELER + (REICH - VORAUS) + k->halb;   /* ... bis Reichweite 900 + 563  */
    const int32_t az0 = k->hz + PR_SPIELER - k->halb, az1 = k->hz + PR_SPIELER + (REICH - VORAUS) + k->halb;
    const int32_t lat = 1238 + k->halb;                        /* seitlich: 900 + 563*sin(37 Grad)          */
    int nx = 0, nz = 0;
    s_px_buf[nx++] = -40000; s_px_buf[nx++] = 40000; s_pz_buf[nz++] = -40000; s_pz_buf[nz++] = 40000;
    for (int i = 0; i < rdt->sca_count && nx < 4000; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        if ((e->floor >> 4) != k->band || !(e->u0 & 1)) continue;
        s_px_buf[nx++] = e->x - rp; s_px_buf[nx++] = e->x + e->width + rp;
        s_pz_buf[nz++] = e->z - rp; s_pz_buf[nz++] = e->z + e->density + rp;
    }
    const int32_t kx[8] = { k->cx - hind_x, k->cx + hind_x + 1, k->cx - ax1, k->cx - ax0 + 1, k->cx + ax0, k->cx + ax1 + 1,
                            k->cx - lat, k->cx + lat + 1 };
    const int32_t kz[8] = { k->cz - hind_z, k->cz + hind_z + 1, k->cz - az1, k->cz - az0 + 1, k->cz + az0, k->cz + az1 + 1,
                            k->cz - lat, k->cz + lat + 1 };
    for (int i = 0; i < 8; i++) { s_px_buf[nx++] = kx[i]; s_pz_buf[nz++] = kz[i]; }
    if (sx != INT32_MIN) { s_px_buf[nx++] = sx; s_px_buf[nx++] = sx + 1; s_pz_buf[nz++] = sz; s_pz_buf[nz++] = sz + 1; }
    nx = uniq(s_px_buf, nx); nz = uniq(s_pz_buf, nz);
    const int W = nx - 1, H = nz - 1;
    int16_t *comp = (int16_t *)malloc(sizeof(int16_t) * (size_t)W * (size_t)H);
    for (int j = 0; j < H; j++)
        for (int i = 0; i < W; i++) {
            const int32_t x = s_px_buf[i], z = s_pz_buf[j];
            int frei = 1;
            if (x > k->cx - hind_x - 1 && x < k->cx + hind_x + 1 && z > k->cz - hind_z - 1 && z < k->cz + hind_z + 1) frei = 0;
            else {
                /* Wandklemme des Spielers: Zellen des Bands mit u0 & 1, um rp erweitert (halboffen). */
                for (int c = 0; c < rdt->sca_count; c++) {
                    const re15_sca_entry_t *e = &rdt->sca[c];
                    if ((e->floor >> 4) != k->band || !(e->u0 & 1)) continue;
                    if ((uint32_t)(x - ((int32_t)e->x - rp)) < (uint32_t)((int32_t)e->width + 2 * rp) &&
                        (uint32_t)(z - ((int32_t)e->z - rp)) < (uint32_t)((int32_t)e->density + 2 * rp)) { frei = 0; break; }
                }
            }
            comp[(size_t)j * W + i] = frei ? 0 : -1;      /* 0 = frei, unbeschriftet; -1 = gesperrt */
        }
    /* Komponenten beschriften (Flutfuellung, 4er-Nachbarschaft). */
    int *q = (int *)malloc(sizeof(int) * (size_t)W * (size_t)H);
    int16_t next = 1;
    for (int s0 = 0; s0 < W * H; s0++) {
        if (comp[s0] != 0) continue;
        int qh = 0, qt = 0; q[qt++] = s0; comp[s0] = next;
        while (qh < qt) {
            const int c = q[qh++], i = c % W, j = c / W;
            static const int di[4] = { 1, -1, 0, 0 }, dj[4] = { 0, 0, 1, -1 };
            for (int d = 0; d < 4; d++) {
                const int ni = i + di[d], nj = j + dj[d];
                if (ni < 0 || nj < 0 || ni >= W || nj >= H) continue;
                int16_t *s2 = &comp[(size_t)nj * W + ni];
                if (*s2 != 0) continue;
                *s2 = next; q[qt++] = nj * W + ni;
            }
        }
        next++;
    }
    /* Seitenregionen: Komponente irgendeiner freien Elementarzelle darin (kleinste Beschriftung, falls mehrere
     * — eine Region kann in mehrere Komponenten zerfallen; dann zaehlt jede, s. seite_komp_alle). */
    for (int s = 0; s < 5; s++) label[s] = -1;
    int32_t rx0[4], rx1[4], rz0[4], rz1[4];
    rx0[0] = k->cx - ax1; rx1[0] = k->cx - ax0 + 1; rz0[0] = k->cz - lat; rz1[0] = k->cz + lat + 1;   /* westlich  */
    rx0[1] = k->cx + ax0; rx1[1] = k->cx + ax1 + 1; rz0[1] = k->cz - lat; rz1[1] = k->cz + lat + 1;   /* oestlich  */
    rx0[2] = k->cx - lat; rx1[2] = k->cx + lat + 1; rz0[2] = k->cz - az1; rz1[2] = k->cz - az0 + 1;   /* noerdlich */
    rx0[3] = k->cx - lat; rx1[3] = k->cx + lat + 1; rz0[3] = k->cz + az0; rz1[3] = k->cz + az1 + 1;   /* suedlich  */
    for (int s = 0; s < 4; s++) {
        for (int j = 0; j < H && label[s] < 0; j++) {
            if (s_pz_buf[j] < rz0[s] || s_pz_buf[j] >= rz1[s]) continue;
            for (int i = 0; i < W; i++) {
                if (s_px_buf[i] < rx0[s] || s_px_buf[i] >= rx1[s]) continue;
                const int16_t c = comp[(size_t)j * W + i];
                if (c > 0) { label[s] = c; break; }
            }
        }
    }
    if (sx != INT32_MIN) {
        const int i = idx_of(s_px_buf, nx, sx), j = idx_of(s_pz_buf, nz, sz);
        if (i >= 0 && j >= 0 && comp[(size_t)j * W + i] > 0) label[4] = comp[(size_t)j * W + i];
    }
    /* Vereinigung: Seiten, die ueber eine andere Komponente derselben Region verbunden sind, werden hier nicht
     * zusammengelegt — das waere nur eine zusaetzliche Verbindung (grosszuegiger). Darum: jede Region auf ALLE
     * Komponenten pruefen und mit der kleinsten gemeinsamen beschriften. */
    for (int s = 0; s < 4; s++) {
        if (label[s] < 0) continue;
        for (int t = s + 1; t < 4; t++) {
            if (label[t] < 0 || label[t] == label[s]) continue;
            /* gemeinsame Komponente? Region t enthaelt label[s] oder Region s enthaelt label[t] */
            int gem = 0;
            for (int j = 0; j < H && !gem; j++) {
                for (int i = 0; i < W && !gem; i++) {
                    const int16_t c = comp[(size_t)j * W + i];
                    if (c <= 0) continue;
                    const int in_s = s_px_buf[i] >= rx0[s] && s_px_buf[i] < rx1[s] && s_pz_buf[j] >= rz0[s] && s_pz_buf[j] < rz1[s];
                    const int in_t = s_px_buf[i] >= rx0[t] && s_px_buf[i] < rx1[t] && s_pz_buf[j] >= rz0[t] && s_pz_buf[j] < rz1[t];
                    if ((in_s && c == label[t]) || (in_t && c == label[s])) gem = 1;
                }
            }
            if (gem) label[t] = label[s];
        }
    }
    free(comp); free(q);
}

/* ---- die gemeinsame Suche ------------------------------------------------------------------------------ */
typedef struct {
    const char *stage, *room; uint16_t id; int obj_id;
    int32_t sx, sz;                      /* Spieler-Startlage (Tuer-Ankunft) */
    int n_patch; struct { uint8_t region, index, u0; } patch[8];   /* Sca_id_set-Vorgriff (ROOM2050 sub02) */
    int soll;                            /* 1 = Ausloeser erreichbar erwartet, 0 = unerreichbar erwartet, -1 = Diagnose */
} fall_t;

static int zensus(const fall_t *F, int32_t *erst_x, int32_t *erst_z, int32_t bbox[4])
{
    if (raum(F->stage, F->room, F->id) != 0) { g_fail++; return -1; }
    for (int i = 0; i < F->n_patch; i++) {
        int base = 0;
        for (int r = 0; r < F->patch[i].region; r++) base += s_rdt.sca_rgn[r];
        const int fi = base + F->patch[i].index;
        if (fi >= 0 && fi < s_rdt.sca_count) ((re15_sca_entry_t *)&s_rdt.sca[fi])->u0 = F->patch[i].u0;   /* op_sca_id_set */
    }
    int p = -1;
    for (int k = 0; k < (int)g_scd.prop_count && k < RE15_SCD_MAX_PROPS; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_type == 4 && g_scd.props[k].obj_id == F->obj_id) p = k;
    if (p < 0) { printf("--- %s obj %d nicht installiert\n", F->room, F->obj_id); return -2; }
    kiste_t k;
    k.band = (int)g_scd.props[p].band;
    k.hx = (int32_t)(uint16_t)g_scd.props[p].box_hx; k.hz = (int32_t)(uint16_t)g_scd.props[p].box_hz;
    const int32_t r = k.hx;                                   /* @0x8003b68c-690 (hx == hz) */
    const int32_t bcx = g_scd.props[p].box_cx, bcz = g_scd.props[p].box_cz;
    const int32_t sx0 = g_scd.props[p].x, sz0 = g_scd.props[p].z;   /* Objektlage (obj+0x34/+0x3C) */
    /* Kisten-Kompression ueber die OBJEKTLAGE: Sperre prueft Lage + Boxmitte, Ausloeser die Lage selbst. */
    static int32_t xs[16384], zs[16384];
    int nx = 0, nz = 0;
    xs[nx++] = (int16_t)s_rdt.ceiling_x; zs[nz++] = (int16_t)s_rdt.ceiling_z;
    xs[nx++] = -40000; xs[nx++] = 40000; zs[nz++] = -40000; zs[nz++] = 40000;
    xs[nx++] = sx0; xs[nx++] = sx0 + 1; zs[nz++] = sz0; zs[nz++] = sz0 + 1;
    for (int i = 0; i < s_rdt.sca_count && nx < 16000; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        const int32_t x0 = e->x, z0 = e->z, w = e->width, d = e->density;
        xs[nx++] = x0 - 450; xs[nx++] = x0 + w + 450; xs[nx++] = x0 - r - bcx; xs[nx++] = x0 + w + r - bcx;
        zs[nz++] = z0 - 450; zs[nz++] = z0 + d + 450; zs[nz++] = z0 - r - bcz; zs[nz++] = z0 + d + r - bcz;
    }
    for (int32_t g = -30000; g <= 30000 && nx < 16000; g += RAST) { xs[nx++] = g; zs[nz++] = g; }
    nx = uniq(xs, nx); nz = uniq(zs, nz);
    const int W = nx - 1, H = nz - 1;
    /* Zustand (Zelle, Seite): 0 neu, 1 besucht; Kistenzelle gesperrt/frei einmal je Zelle. */
    uint8_t *frei = (uint8_t *)calloc((size_t)W * (size_t)H, 1);
    uint8_t *aus  = (uint8_t *)calloc((size_t)W * (size_t)H, 1);
    uint8_t *vis  = (uint8_t *)calloc((size_t)W * (size_t)H * 4, 1);
    int32_t *lab  = (int32_t *)malloc(sizeof(int32_t) * 5 * (size_t)W * (size_t)H);   /* Labels je Zelle (Cache) */
    uint8_t *labok = (uint8_t *)calloc((size_t)W * (size_t)H, 1);
    int *par = (int *)malloc(sizeof(int) * (size_t)W * (size_t)H * 4);   /* Vorgaenger-Zustand (Pfadausgabe) */
    for (size_t t = 0; t < (size_t)W * (size_t)H * 4; t++) par[t] = -1;
    for (int j = 0; j < H; j++)
        for (int i = 0; i < W; i++) {
            frei[(size_t)j * W + i] = !re15_collision_box_blocked(&s_rdt, xs[i] + bcx, zs[j] + bcz, k.band, r, 2u);
            aus[(size_t)j * W + i]  = (re15_collision_floor_typeword(&s_rdt, xs[i], zs[j], k.band, 450) & 2) ? 1 : 0;
        }
    const int i0 = idx_of(xs, nx, sx0), j0 = idx_of(zs, nz, sz0);
    int *q = (int *)malloc(sizeof(int) * (size_t)W * (size_t)H * 4);
    int qh = 0, qt = 0, n_aus = 0, n_zell = 0, n_lab = 0;
    bbox[0] = bbox[2] = 0x7fffffff; bbox[1] = bbox[3] = -0x7fffffff;
    *erst_x = 0; *erst_z = 0;
    /* Start: Spieler-Startlage -> Komponente -> alle Seiten mit dieser Beschriftung. */
    {
        k.cx = xs[i0] + bcx; k.cz = zs[j0] + bcz; k.halb = 0;
        int L[5]; seiten_labels(&s_rdt, &k, F->sx, F->sz, L);
        printf("--- %s obj %d Band %d Start Kiste (%d,%d) Spieler (%d,%d): Seitenlabels W/O/N/S = %d/%d/%d/%d, Spieler %d\n",
               F->room, F->obj_id, k.band, sx0, sz0, F->sx, F->sz, L[0], L[1], L[2], L[3], L[4]);
        if (L[4] < 0) { printf("    Spieler-Startlage gesperrt -> keine Suche\n"); return -3; }
        for (int s = 0; s < 4; s++)
            if (L[s] == L[4]) { vis[((size_t)j0 * W + i0) * 4 + s] = 1; q[qt++] = ((j0 * W + i0) << 2) | s; }
        if (qt == 0) printf("    keine Schubseite von der Startlage aus erreichbar\n");
    }
    while (qh < qt) {
        const int st = q[qh++], c = st >> 2, s = st & 3, i = c % W, j = c / W;
        if (!labok[c]) {
            k.cx = xs[i] + bcx; k.cz = zs[j] + bcz;
            k.halb = (xs[i + 1] - xs[i] > zs[j + 1] - zs[j] ? xs[i + 1] - xs[i] : zs[j + 1] - zs[j]) / 2;
            seiten_labels(&s_rdt, &k, INT32_MIN, 0, &lab[(size_t)c * 5]); labok[c] = 1; n_lab++;
        }
        const int32_t *L = &lab[(size_t)c * 5];
        if (xs[i] < bbox[0]) bbox[0] = xs[i]; if (xs[i + 1] > bbox[1]) bbox[1] = xs[i + 1];
        if (zs[j] < bbox[2]) bbox[2] = zs[j]; if (zs[j + 1] > bbox[3]) bbox[3] = zs[j + 1];
        n_zell++;
        for (int s2 = 0; s2 < 4; s2++) {
            if (L[s2] < 0 || L[s2] != L[s]) continue;           /* Seite s2 nicht aus der Lage des Spielers erreichbar */
            if (s2 != s && !vis[(size_t)c * 4 + s2]) {          /* Seitenwechsel auf derselben Zelle (Pfadausgabe) */
                vis[(size_t)c * 4 + s2] = 1; par[(c << 2) | s2] = st; q[qt++] = (c << 2) | s2;
            }
            /* Schub von Seite s2: Kiste wandert vom Spieler weg (0 W -> +X, 1 O -> -X, 2 N -> +Z, 3 S -> -Z). */
            static const int di[4] = { 1, -1, 0, 0 }, dj[4] = { 0, 0, 1, -1 };
            const int ni = i + di[s2], nj = j + dj[s2];
            if (ni < 0 || nj < 0 || ni >= W || nj >= H) continue;
            const int nc = nj * W + ni;
            if (!frei[nc]) continue;                             /* FUN_8003b558 != 0 -> Ruecksetzung @0x8002c0b8-d4 */
            if (aus[nc]) {                                       /* Objekt-Pass: Ausloeser @0x8002c130 -> Fall */
                if (n_aus++ == 0) { *erst_x = xs[ni]; *erst_z = zs[nj];
                    printf("    AUSLOESER erreichbar: Kiste (%d,%d) von Seite %c geschoben (Spieler bleibt dort)\n",
                           xs[ni], zs[nj], "WONS"[s2]);
                    /* Pfad (Diagnose, fuer den Messlauf an der exe): Schub-Segmente von der Startlage bis hierher,
                     * gleiche Seite zusammengefasst; Seitenwechsel = Spieler geht um die Kiste herum. */
                    static int chain[65536]; int n = 0; int cur = st;
                    while (cur >= 0 && n < 65536) { chain[n++] = cur; cur = par[cur]; }
                    int seg_s = -1, seg_x0 = 0, seg_z0 = 0, last_x = 0, last_z = 0;
                    for (int t = n - 1; t >= 0; t--) {
                        const int cc = chain[t] >> 2, ss = chain[t] & 3, ci = cc % W, cj = cc / W;
                        const int32_t x = xs[ci] + bcx, z = zs[cj] + bcz;    /* Kistenmitte (Box) */
                        if (t == n - 1) { seg_s = ss; seg_x0 = x; seg_z0 = z; last_x = x; last_z = z; continue; }
                        if (ss != seg_s) {
                            if (last_x != seg_x0 || last_z != seg_z0)
                                printf("      Schub von %c: Kistenmitte (%d,%d) -> (%d,%d)\n", "WONS"[seg_s], seg_x0, seg_z0, last_x, last_z);
                            printf("      Seitenwechsel %c -> %c bei Kistenmitte (%d,%d)\n", "WONS"[seg_s], "WONS"[ss], x, z);
                            seg_s = ss; seg_x0 = x; seg_z0 = z;
                        }
                        last_x = x; last_z = z;
                    }
                    if (last_x != seg_x0 || last_z != seg_z0)
                        printf("      Schub von %c: Kistenmitte (%d,%d) -> (%d,%d)\n", "WONS"[seg_s], seg_x0, seg_z0, last_x, last_z);
                    printf("      Schub von %c: Kistenmitte (%d,%d) -> (%d,%d) = AUSLOESER-Zelle (Objektlage x [%d,%d) z [%d,%d))\n",
                           "WONS"[s2], last_x, last_z, xs[ni] + bcx, zs[nj] + bcz, xs[ni], xs[ni + 1], zs[nj], zs[nj + 1]);
                }
                continue;
            }
            if (vis[(size_t)nc * 4 + s2]) continue;
            /* Spieler folgt auf seiner Seite: dort muss eine freie Schubstellung existieren. */
            if (!labok[nc]) {
                k.cx = xs[ni] + bcx; k.cz = zs[nj] + bcz;
                k.halb = (xs[ni + 1] - xs[ni] > zs[nj + 1] - zs[nj] ? xs[ni + 1] - xs[ni] : zs[nj + 1] - zs[nj]) / 2;
                seiten_labels(&s_rdt, &k, INT32_MIN, 0, &lab[(size_t)nc * 5]); labok[nc] = 1; n_lab++;
            }
            if (lab[(size_t)nc * 5 + s2] < 0) continue;
            vis[(size_t)nc * 4 + s2] = 1; par[(nc << 2) | s2] = st; q[qt++] = (nc << 2) | s2;
        }
    }
    printf("    erreichbare Kistenzellen (mit Spieler): %d, davon Ausloeser-Uebergaenge: %d; Kistenlage x [%d, %d) z [%d, %d)\n",
           n_zell, n_aus, bbox[0], bbox[1], bbox[2], bbox[3]);
    free(frei); free(aus); free(vis); free(lab); free(labok); free(q); free(par);
    return n_aus;
}

int main(int argc, char **argv)
{
    const char *mode = (argc > 1) ? argv[1] : "diag";
    const int pin = !strcmp(mode, "pin");
    setvbuf(stdout, NULL, _IONBF, 0);
    static const fall_t F[] = {
        /* ROOM1090/1091 obj 1 (Dach, Band 5): Ankunft aus ROOM10F0/10F1 (Door slot 1 @0x00F52 -> (-13600,1300)). */
        { "STAGE1", "ROOM1090", 0x1090, 1, -13600, 1300, 0, {{0,0,0}}, 0 },
        { "STAGE1", "ROOM1091", 0x1091, 1, -13600, 1300, 0, {{0,0,0}}, 0 },
        /* ROOM3050/3051 obj 1 (Band 4): Ankunft aus ROOM30E0 (Door slot 6 @0x00ABA -> (-3596,-7200,1523) floor 4).
         * PIN (Nachbesserung 1, M3): der Fall ist der VORGESEHENE Weg (Fall-Lage @0x01F40 unter (3,93), Kerbe slot 7
         * @0x01F06, Latch sub01 @0x02184-2196, Super Redhawk slot 6 @0x01EC6 floor 1) -> der Zensus MUSS einen
         * Ausloeser finden, und die erste Lage liegt in der um 450 erweiterten Absprungzelle r*[46] @0x00AD4
         * (x -25000..-22950 z -18050..-16800, Wort 0x4002; FUN_8003b7f0-Radius 450 @0x8002c128). An der exe
         * gemessen (R_belege/nb1_3050fall_*): Schub West, dann Sued -> AUSLOESER -> Fall auf Band 0. */
        { "STAGE3", "ROOM3050", 0x3050, 1, -3596, 1523, 0, {{0,0,0}}, 1 },
        { "STAGE3", "ROOM3051", 0x3051, 1, -3596, 1523, 0, {{0,0,0}}, 1 },
        /* ROOM2050/2051 obj 6 (Band 3): Ankunft aus ROOM2000/20B0 (Door slot 4 -> (9100,-5400,4100) floor 3); einmal
         * Auslieferung, einmal nach sub02 (Sca_id_set r*[0x33] u0 = 0 @0x02294-22A4, r*[0x32] u0 = 0xff @0x022A8-22B8).
         * PIN (soll = 1, Nachbesserung 2 / N3): an der exe GEMESSEN (R_belege/nb2_2050fall_c, Wegplan-Treiber):
         * Sued bis Kistenmitte z -7037, Ost aus der Tasche bis x 7571 (Fenster [7550,7650) zwischen r*[44] und r*[40]),
         * Nord von Sueden -> F1624 AUSLOESER wort=0x3202 pos=(7571,-5400,-11706) -> Fall -> Landung Band 2. Der Zensus
         * MUSS einen Ausloeser finden, und die erste Lage liegt in der um 450 (@0x8002c12c) erweiterten Absprungzelle
         * r*[49] @0x00B5C x 6350..8750 z -13950..-12150 (Wort 0x3202, u0 0x01 sperrt nur den Spieler). */
        { "STAGE2", "ROOM2050", 0x2050, 6, 9100, 4100, 0, {{0,0,0}}, 1 },
        { "STAGE2", "ROOM2051", 0x2051, 6, 9100, 4100, 0, {{0,0,0}}, 1 },
        { "STAGE2", "ROOM2050", 0x2050, 6, 9100, 4100, 4, {{0,0x33,0},{1,0x33,0},{2,0x33,0},{3,0x33,0}}, 1 },
        { "STAGE2", "ROOM2050", 0x2050, 6, -6800, -5650, 4, {{0,0x33,0},{1,0x33,0},{2,0x33,0},{3,0x33,0}}, 1 },
    };
    for (size_t i = 0; i < sizeof F / sizeof F[0]; i++) {
        int32_t ex, ez, bb[4];
        fall_t f = F[i];
        if (f.n_patch == 4) {   /* Vorgriff: zusaetzlich r*[0x32] u0 = 0xff (Regionen 0..3; Region 4 hat dieselben Zellen) */
            printf("--- (nach sub02: Zelle 0x33 frei, Zelle 0x32 gesperrt)\n");
        }
        const int n = zensus(&f, &ex, &ez, bb);
        if (f.n_patch == 4) {
            /* zweiter Teil des Vorgriffs: 0x32 sperren */
            fall_t g = f; g.n_patch = 0;
            (void)g;
        }
        if (n < 0 && f.soll >= 0) PRUEF(0, "%s: Zensus nicht moeglich (%d)", f.room, n);
        if (n >= 0 && f.soll == 0) PRUEF(n == 0, "%s obj %d: Ausloeser MIT Spieler erreichbar (%d), erste Lage (%d,%d)", f.room, f.obj_id, n, ex, ez);
        if (n >= 0 && f.soll == 1) PRUEF(n > 0, "%s obj %d: Ausloeser mit Spieler NICHT erreichbar", f.room, f.obj_id);
        if (n > 0 && f.soll == 1 && f.id >= 0x2050 && f.id <= 0x2051) {
            /* Absprungzelle r*[49] @0x00B5C x 6350..8750 z -13950..-12150, Objektlage mit Radius 450 (@0x8002c12c). */
            PRUEF(ex >= 6350 - 450 && ex <= 8750 + 450 && ez >= -13950 - 450 && ez <= -12150 + 450,
                  "%s obj %d: erste Ausloeser-Lage (%d,%d) nicht in der Absprungzelle r*[49] (+-450)", f.room, f.obj_id, ex, ez);
        }
        if (n > 0 && f.soll == 1 && f.id >= 0x3050 && f.id <= 0x3051) {
            /* Absprungzelle r*[46] @0x00AD4 x -25000..-22950 z -18050..-16800, Objektlage mit Radius 450 (@0x8002c128). */
            PRUEF(ex >= -25000 - 450 && ex <= -22950 + 450 && ez >= -18050 - 450 && ez <= -16800 + 450,
                  "%s obj %d: erste Ausloeser-Lage (%d,%d) nicht in der Absprungzelle r*[46] (+-450)", f.room, f.obj_id, ex, ez);
        }
        if (n >= 0 && f.soll == 0) {
            /* ROOM1090/1091: die Kiste bleibt an der Bruestung: Mitte z >= -15503 nie groesser als der Record
             * (kein Schub nach Sueden moeglich), x in [-5978, -4300). */
            PRUEF(bb[3] <= -15482 + 1 + RAST && bb[2] >= -15503 - 1, "%s: Kisten-z-Bereich [%d, %d) ausserhalb [-15503, -15482]", f.room, bb[2], bb[3]);
        }
    }
    if (!pin) return 0;
    printf(g_fail ? "FAIL %d\n" : "PASS\n", g_fail);
    return g_fail ? 1 : 0;
}
