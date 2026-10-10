/*
 * Runde 36 Spur F "zombieki" — Gorilla-Schattenrecord und Blutlache der Leiche (Belege: re15_zombieki_r36.h).
 */
#include "re15_zombieki_r36.h"
#include "re15_room.h"
#include "re15_ai_flavor.h"
#include "re15_collision.h"
#include "re15_skeleton.h"   /* re15_cos_q12 / re15_sin_q12 */
#include <string.h>

/* ----- Punkt 1: Sicht-Bit des RE2-Zombies (Belege: re15_zombieki_r36.h) ----- */
static int s_r36_zombie_sicht_an = 1;   /* NUR Sonden: 0 = alter Weg re15_re2_los_clear (Slab-Test) */
void re15_r36_zombie_sicht_enable(int an) { s_r36_zombie_sicht_an = an ? 1 : 0; }

int re15_r36_zombie_sicht_frei(re15_actor_t *e, re15_actor_t *pl)
{
    if (!s_r36_zombie_sicht_an) return re15_re2_los_clear(e, pl) ? 1 : 0;
    if (!g_room_rdt_ok) return 1;                                   /* keine Zellen -> frei (wie bisher) */
    return re15_r36_los_ray_nib3(e, pl->x, pl->z) ? 0 : 1;          /* FUN_8003dcc4(0xf00, 0x300) @0x8001b8d4/8ec */
}

/* NUR FUER SONDEN (Gegenprobe "vorher" im selben Lauf): 0 = die Leichen-Haken schreiben nichts. Das Spiel ruft das nie. */
static int s_r36_affen_lache_an = 1;
void re15_r36_affen_lache_enable(int an) { s_r36_affen_lache_an = an ? 1 : 0; }

static void r36_padsuche_reset(const re15_actor_t *e);   /* Punkt 2, unten: Pad-Suche des Slots beim INIT loeschen */
void re15_r36_affen_lache_init(re15_actor_t *e)
{
    if (!e) return;
    r36_padsuche_reset(e);
    e->crow_shadow_w = (uint16_t)RE15_R36_AFFEN_SCHATTEN_X;   /* +0xBC = 1600 + 100 @0x801171e8 -> sh @0x8001b030 */
    e->crow_shadow_h = (uint16_t)RE15_R36_AFFEN_SCHATTEN_Z;   /* +0xBE = 1600 + 200 @0x801171f0 -> sh @0x8001b038 */
    e->crow_pool     = 0;                                     /* Farbe des Records: 0x80808080 @0x801171bc (grau) */
}

void re15_r36_affen_lache_tick(re15_actor_t *e)
{
    if (!e || !s_r36_affen_lache_an) return;
    e->crow_pool     = 1;                                                              /* +0xC4/+0xEC = (alt & 0xff000000) | 0x00ffff38 @0x8011bed0-f00 */
    e->crow_shadow_w = (uint16_t)(e->crow_shadow_w + RE15_R36_AFFEN_LACHE_SCHRITT);   /* +0xBC += 8 @0x8011bf08/bf10 */
    e->crow_shadow_h = (uint16_t)(e->crow_shadow_h + RE15_R36_AFFEN_LACHE_SCHRITT);   /* +0xBE += 8 @0x8011bf0c/bf14 */
}

int re15_r36_affen_lache_render(const re15_actor_t *npc, int32_t *nhx, int32_t *nhz, int *pool,
                                uint8_t *sh_r, uint8_t *sh_g, uint8_t *sh_b)
{
    if (!npc || npc->type != 0x27u || npc->crow_shadow_w == 0) return 0;
    *nhx = npc->crow_shadow_w; *nhz = npc->crow_shadow_h;
    if (npc->crow_pool) {
        *pool = 1;
        *sh_r = 0x38; *sh_g = 0xff; *sh_b = 0xff;   /* 0x00ffff38 (B,G,R-Bytes) @0x8011bed0 `ori a2,a2,0xff38` + `lui a2,0xff` @0x8011be6c */
    }
    return 1;
}

/* ----- Punkt 2: Umweg des Gorillas (PORT-WAHL, Nachbesserung 1; Belege: re15_zombieki_r36.h) ----- */
static int     s_r36_padsuche_an = 1;                  /* NUR Sonden: 0 = Original-Verhalten (Gegenprobe) */
static uint8_t s_r36_padsuche_aktiv[RE15_ACTOR_MAX];   /* 0 = aus, 1 = Umweg zu Fuss, 2 = Anlauf auf ein Pad (Sprung = Original) */
static int32_t s_r36_ziel_x[RE15_ACTOR_MAX], s_r36_ziel_z[RE15_ACTOR_MAX];
static int16_t s_r36_pad_idx[RE15_ACTOR_MAX];
void re15_r36_affen_padsuche_enable(int an) { s_r36_padsuche_an = an ? 1 : 0; memset(s_r36_padsuche_aktiv, 0, sizeof s_r36_padsuche_aktiv); }
int  re15_r36_affen_padsuche_aktiv(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_r36_padsuche_aktiv[slot] : 0; }
static void r36_padsuche_reset(const re15_actor_t *e)
{
    long slot = e - g_actors;
    if (slot >= 0 && slot < RE15_ACTOR_MAX) s_r36_padsuche_aktiv[slot] = 0;
}

/* ---- Raster: Begehbarkeit aus der ORIGINAL-Klemme (FUN_8003b0a4-Zwilling re15_collision_constrain_contact_band
 * mit Radius, Band und Maske des Aktors): ein Knoten ist frei, wenn die Klemme ihn nicht verschiebt. Schritt = Radius/2:
 * zwei freie Nachbarknoten liegen je >= Radius von jeder Zelle entfernt, eine Zelle kann also nicht zwischen ihnen
 * liegen (keine Abkuerzung durch Waende). Komponenten = 8er-Nachbarschaft (diagonal nur mit beiden Kanten frei). */
#define R36_UW_MAX 12288
typedef struct {
    const void *rdt_id; int band; int32_t r; uint32_t mask; int ok;
    int32_t x0, z0, st; int nx, nz;
    uint8_t frei[R36_UW_MAX];
    uint16_t komp[R36_UW_MAX];
    int16_t dist[R36_UW_MAX]; int dist_ziel;
} r36_uw_t;
static r36_uw_t s_uw;

static int r36_uw_frei_punkt(int32_t x, int32_t z, int32_t r, int band, uint32_t mask)
{
    int32_t nx = x, nz = z; uint8_t ct = 0; uint16_t at = 0;
    re15_collision_constrain_contact_band(&g_room_rdt, x, z, &nx, &nz, r, band, mask, &ct, &at);
    return nx == x && nz == z;
}

static int r36_uw_bauen(int band, int32_t r, uint32_t mask)
{
    if (!g_room_rdt_ok || !g_room_rdt.sca || g_room_rdt.sca_count <= 0 || r <= 0) return 0;
    if (s_uw.ok && s_uw.rdt_id == (const void *)g_room_rdt.sca && s_uw.band == band && s_uw.r == r && s_uw.mask == mask) return 1;
    int32_t xa = 0x7fffffff, xb = -0x7fffffff, za = 0x7fffffff, zb = -0x7fffffff;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if ((int32_t)c->x < xa) xa = c->x;
        if ((int32_t)c->z < za) za = c->z;
        if ((int32_t)c->x + (int32_t)c->width > xb) xb = (int32_t)c->x + (int32_t)c->width;
        if ((int32_t)c->z + (int32_t)c->density > zb) zb = (int32_t)c->z + (int32_t)c->density;
    }
    int32_t st = r / 2;
    int nx = (xb - xa) / st + 1, nz = (zb - za) / st + 1;
    s_uw.ok = 0;
    if (st <= 0 || nx <= 2 || nz <= 2 || nx * nz > R36_UW_MAX) return 0;
    s_uw.rdt_id = (const void *)g_room_rdt.sca; s_uw.band = band; s_uw.r = r; s_uw.mask = mask;
    s_uw.x0 = xa; s_uw.z0 = za; s_uw.st = st; s_uw.nx = nx; s_uw.nz = nz; s_uw.dist_ziel = -1;
    for (int k = 0; k < nx * nz; k++)
        s_uw.frei[k] = (uint8_t)r36_uw_frei_punkt(xa + (k % nx) * st, za + (k / nx) * st, r, band, mask);
    static int q[R36_UW_MAX];
    memset(s_uw.komp, 0, sizeof s_uw.komp);
    uint16_t id = 0;
    for (int k = 0; k < nx * nz; k++) {
        if (!s_uw.frei[k] || s_uw.komp[k]) continue;
        id++; int h = 0, t = 0; q[t++] = k; s_uw.komp[k] = id;
        while (h < t) {
            int n = q[h++], ix = n % nx, iz = n / nx;
            for (int dz = -1; dz <= 1; dz++) for (int dx = -1; dx <= 1; dx++) {
                if (!dx && !dz) continue;
                int jx = ix + dx, jz = iz + dz;
                if (jx < 0 || jz < 0 || jx >= nx || jz >= nz) continue;
                int m = jz * nx + jx;
                if (!s_uw.frei[m] || s_uw.komp[m]) continue;
                if (dx && dz && (!s_uw.frei[iz * nx + jx] || !s_uw.frei[jz * nx + ix])) continue;
                s_uw.komp[m] = id; q[t++] = m;
            }
        }
    }
    s_uw.ok = 1;
    return 1;
}

/* naechster freier Knoten zu (x,z) innerhalb von `ring` Knoten (-1 = keiner) */
static int r36_uw_knoten(int32_t x, int32_t z, int ring)
{
    int ix = (int)((x - s_uw.x0 + s_uw.st / 2) / s_uw.st), iz = (int)((z - s_uw.z0 + s_uw.st / 2) / s_uw.st);
    int best = -1; int64_t bd = 0;
    for (int dz = -ring; dz <= ring; dz++) for (int dx = -ring; dx <= ring; dx++) {
        int jx = ix + dx, jz = iz + dz;
        if (jx < 0 || jz < 0 || jx >= s_uw.nx || jz >= s_uw.nz) continue;
        int m = jz * s_uw.nx + jx;
        if (!s_uw.frei[m]) continue;
        int64_t ex = (int64_t)s_uw.x0 + (int64_t)jx * s_uw.st - x, ez = (int64_t)s_uw.z0 + (int64_t)jz * s_uw.st - z, d = ex * ex + ez * ez;
        if (best < 0 || d < bd) { best = m; bd = d; }
    }
    return best;
}

static void r36_uw_bfs(int ziel)
{
    if (s_uw.dist_ziel == ziel) return;
    static int q[R36_UW_MAX];
    int n = s_uw.nx * s_uw.nz;
    for (int k = 0; k < n; k++) s_uw.dist[k] = -1;
    int h = 0, t = 0; q[t++] = ziel; s_uw.dist[ziel] = 0;
    while (h < t) {
        int a = q[h++], ix = a % s_uw.nx, iz = a / s_uw.nx;
        for (int dz = -1; dz <= 1; dz++) for (int dx = -1; dx <= 1; dx++) {
            if (!dx && !dz) continue;
            int jx = ix + dx, jz = iz + dz;
            if (jx < 0 || jz < 0 || jx >= s_uw.nx || jz >= s_uw.nz) continue;
            int m = jz * s_uw.nx + jx;
            if (!s_uw.frei[m] || s_uw.dist[m] >= 0) continue;
            if (dx && dz && (!s_uw.frei[iz * s_uw.nx + jx] || !s_uw.frei[jz * s_uw.nx + ix])) continue;
            s_uw.dist[m] = (int16_t)(s_uw.dist[a] + 1); q[t++] = m;
        }
    }
    s_uw.dist_ziel = ziel;
}

static int r36_uw_strecke_frei(int a, int b)   /* gerade Strecke Knoten a -> b nur ueber freie Knoten (halber Schritt) */
{
    int32_t ax = s_uw.x0 + (a % s_uw.nx) * s_uw.st, az = s_uw.z0 + (a / s_uw.nx) * s_uw.st;
    int32_t bx = s_uw.x0 + (b % s_uw.nx) * s_uw.st, bz = s_uw.z0 + (b / s_uw.nx) * s_uw.st;
    int32_t dx = bx - ax, dz = bz - az; int64_t len = (int64_t)dx * dx + (int64_t)dz * dz;
    int n = 1; while ((int64_t)n * n * (s_uw.st / 2) * (s_uw.st / 2) < len) n++;
    for (int i = 1; i < n; i++) {
        int k = r36_uw_knoten(ax + (int32_t)((int64_t)dx * i / n), az + (int32_t)((int64_t)dz * i / n), 0);
        if (k < 0) return 0;
    }
    return 1;
}

/* Fuss-Umweg: Knoten g -> Ziel-Knoten z (gleiche Komponente); Zielpunkt = der fernste Knoten entlang des fallenden
 * BFS-Abstands, der gerade erreichbar ist (hoechstens 8 Schritte voraus). */
static int r36_uw_schritt(int g, int z, int32_t *tx, int32_t *tz)
{
    r36_uw_bfs(z);
    if (s_uw.dist[g] < 0) return 0;
    int cur = g, wahl = g;
    for (int s = 0; s < 8 && s_uw.dist[cur] > 0; s++) {
        int ix = cur % s_uw.nx, iz = cur / s_uw.nx, nxt = -1;
        for (int dz = -1; dz <= 1 && nxt < 0; dz++) for (int dx = -1; dx <= 1; dx++) {
            int jx = ix + dx, jz = iz + dz;
            if ((!dx && !dz) || jx < 0 || jz < 0 || jx >= s_uw.nx || jz >= s_uw.nz) continue;
            int m = jz * s_uw.nx + jx;
            if (s_uw.dist[m] == s_uw.dist[cur] - 1) { nxt = m; break; }
        }
        if (nxt < 0) break;
        cur = nxt;
        if (r36_uw_strecke_frei(g, cur)) wahl = cur;
    }
    if (wahl == g) wahl = cur;
    *tx = s_uw.x0 + (wahl % s_uw.nx) * s_uw.st; *tz = s_uw.z0 + (wahl / s_uw.nx) * s_uw.st;
    return 1;
}

/* Pad zwischen den Komponenten kg (Gorilla) und kl (Leon): SCA-Zelle u1 == 0x10 auf dem Band (die Zellen, die
 * FUN_8003b93c(attr 0x10) in A[4] @0x80117ee0-f00 abfragt), an deren einer Seite kg und an der GEGENUEBERLIEGENDEN
 * kl liegt. Der Sprung des Originals geht von der Kontaktseite UEBER das Pad (coll_contact_dir @0x8003ba8c: westlich
 * 0x000, suedlich 0xc00, noerdlich 0x400, oestlich 0x800). *start = freier Knoten der kg-Seite (Anlaufpunkt). */
static int r36_uw_pad(int kg, int kl, int32_t gx, int32_t gz, int *start)
{
    int best = -1; int64_t bd = 0;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if ((int)(c->floor >> 4) != s_uw.band || c->u1 != 0x10u) continue;
        int32_t mx = (int32_t)c->x + (int32_t)c->width / 2, mz = (int32_t)c->z + (int32_t)c->density / 2;
        int32_t a = s_uw.r + s_uw.st;                 /* knapp ausserhalb der Pad-Kante */
        int32_t sx[4] = { mx, mx, (int32_t)c->x - a, (int32_t)c->x + (int32_t)c->width + a };
        int32_t sz[4] = { (int32_t)c->z - a, (int32_t)c->z + (int32_t)c->density + a, mz, mz };
        int ks[4];
        for (int s = 0; s < 4; s++) { int k = r36_uw_knoten(sx[s], sz[s], 3); ks[s] = (k >= 0) ? k : -1; }
        for (int s = 0; s < 4; s++) {
            int o = s ^ 1;                             /* Gegenseite: 0<->1 (Sued/Nord), 2<->3 (West/Ost) */
            if (ks[s] < 0 || ks[o] < 0 || s_uw.komp[ks[s]] != kg || s_uw.komp[ks[o]] != kl) continue;
            int64_t ex = (int64_t)sx[s] - gx, ez = (int64_t)sz[s] - gz, d = ex * ex + ez * ez;
            if (best < 0 || d < bd) { best = i; bd = d; *start = ks[s]; }
        }
    }
    return best;
}

/* Zielwahl fuer Gorilla e mit Leon pl. Rueckgabe 0 = kein Umweg (Original), 1 = zu Fuss, 2 = Pad-Anlauf. */
static int r36_uw_route(const re15_actor_t *e, const re15_actor_t *pl, int slot)
{
    if (!r36_uw_bauen((int)e->floor, (int32_t)e->hit_radius_min, 4u)) return 0;   /* Maske 4 = re15_enemy_sca_clamp_band(..., 4u) des 0x27-Zweigs */
    int g = r36_uw_knoten(e->x, e->z, 3), l = r36_uw_knoten(pl->x, pl->z, 4);
    if (g < 0 || l < 0) return 0;
    int32_t tx, tz;
    if (s_uw.komp[g] == s_uw.komp[l]) {
        if (!r36_uw_schritt(g, l, &tx, &tz)) return 0;
        s_r36_ziel_x[slot] = tx; s_r36_ziel_z[slot] = tz;
        return 1;
    }
    int start = -1, p = r36_uw_pad(s_uw.komp[g], s_uw.komp[l], e->x, e->z, &start);
    if (p < 0) return 0;
    const re15_sca_entry_t *c = &g_room_rdt.sca[p];
    int32_t sx = s_uw.x0 + (start % s_uw.nx) * s_uw.st, sz = s_uw.z0 + (start / s_uw.nx) * s_uw.st;
    int64_t ex = (int64_t)sx - e->x, ez = (int64_t)sz - e->z;
    s_r36_pad_idx[slot] = (int16_t)p;
    if (ex * ex + ez * ez > (int64_t)s_uw.st * s_uw.st * 4) {        /* noch nicht am Anlaufpunkt: zu Fuss dorthin */
        if (!r36_uw_schritt(g, start, &tx, &tz)) return 0;
        s_r36_ziel_x[slot] = tx; s_r36_ziel_z[slot] = tz;
        return 1;
    }
    s_r36_ziel_x[slot] = (int32_t)c->x + (int32_t)c->width / 2;      /* gerader Anlauf auf die Pad-Mitte: die Kontaktseite */
    s_r36_ziel_z[slot] = (int32_t)c->z + (int32_t)c->density / 2;    /* zeigt auf Leons Seite, das Yaw-Fenster passt */
    return 2;
}

/* Mitte des Gorillas in einem 0x10-Pad-Rechteck (Band des Gorillas)? */
static int r36_im_pad10(const re15_actor_t *e)
{
    if (!g_room_rdt_ok || !g_room_rdt.sca) return 0;
    for (int i = 0; i < g_room_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &g_room_rdt.sca[i];
        if ((int)(c->floor >> 4) != (int)e->floor || c->u1 != 0x10u) continue;
        if ((uint32_t)(e->x - (int32_t)c->x) < (uint32_t)c->width && (uint32_t)(e->z - (int32_t)c->z) < (uint32_t)c->density) return 1;
    }
    return 0;
}

/* Gehakt aus A[3] (sub 3, nach dem Biss-Test) und vor B[4] (sub 4). Rueckgabe 1 = der 0x10-Sprung des Originals wurde
 * ausgeloest (nur sub 3; in sub 4 fuehrt A[4] Path A @0x80117ecc-8011802c ihn selbst aus). */
int re15_r36_affen_padsuche_decide(re15_actor_t *e, int slot, int sub)
{
    if (!s_r36_padsuche_an || !e || slot < 0 || slot >= RE15_ACTOR_MAX || (sub != 3 && sub != 4)) return 0;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (sub == 3 && (e->dog_flags & 1)) { s_r36_padsuche_aktiv[slot] = 0; return 0; }   /* Jagd mit Sicht: Original (fern+Sicht -> Sub 4
                                                                                       * @0x80117c48-54) */
    if ((uint16_t)e->dog_dist < 0x1771u && !r36_im_pad10(e)) { s_r36_padsuche_aktiv[slot] = 0; return 0; }
                                                                                      /* NAH (< 6001, `sltiu 0x1771` @0x80117bc8 / @0x8011806c):
                                                                                       * Original (Spur-J-Riegel bildgenau) — ausser der Gorilla
                                                                                       * steckt MIT DER MITTE in einem 0x10-Pad (Engstelle, Kontakt-
                                                                                       * code 0xf00 @0x8003ba8c, das Yaw-Fenster passt nie zu Leon) */
    if (!s_r36_padsuche_aktiv[slot] && e->sca_wall_hit == 0) return 0;                  /* Beginn nur an der Wandklemme (+0x1d6, A[3]
                                                                                       * @0x80117b18-20 / A[4] @0x80118054-5c) */
    uint8_t zk = 0;
    if (r36_im_pad10(e) && re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x10u, &zk)) {
        /* MITTE IM PAD (Engstelle, verkeilt): der Kontaktcode ist der Platzhalter 0xf00 (coll_contact_dir @0x8003ba8c);
         * zu Fuss kommt der Gorilla nicht heraus (Klemme hebt jeden Schritt auf). Ausweg = der Sprung des Originals:
         * in die Code-Richtung (zc & 0xf0) << 4 drehen, dann passt das Yaw-Fenster @0x80117f18-38. */
        int d = (((int)(zk & 0xf0u)) << 4) & 0xfff;
        s_r36_ziel_x[slot] = e->x + (int32_t)(((int64_t)re15_cos_q12(d) * (int64_t)e->hit_radius_min) >> 12);   /* Vorwaerts = (cos, -sin) */
        s_r36_ziel_z[slot] = e->z - (int32_t)(((int64_t)re15_sin_q12(d) * (int64_t)e->hit_radius_min) >> 12);   /* (FUN_800152C8); Laenge egal */
        s_r36_padsuche_aktiv[slot] = 2;
    } else {
        s_r36_padsuche_aktiv[slot] = (uint8_t)r36_uw_route(e, pl, slot);
    }
    if (sub != 3 || s_r36_padsuche_aktiv[slot] != 2) return 0;
    uint8_t zc = 0;                                                               /* am Pad? (wie A[4] @0x80117ee0-f00) */
    if (re15_collision_zone_query(&g_room_rdt, e->x, e->z, (int32_t)e->hit_radius_min + 100, (int)e->floor, 0x10u, &zc)) {
        int win = ((((int)(zc & 0xf0u)) << 4) - (int)e->rot_y + 512) & 0xfff;   /* Yaw-Fenster des Originals @0x80117f18-38 */
        if (win < 1024) {                                                         /* der 0x10-Sprung des Originals (A[4] @0x80117f40-74) */
            s_r36_padsuche_aktiv[slot] = 0;
            e->dog_aux9f = (int8_t)zc;                                            /* +0x9f = +0x90 @0x80117f40 */
            e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 1;          /* +0x5 = 7, +0x6 = 0, +0x7 = 1 @0x80117f50-74 */
            return 1;
        }
    }
    return 0;
}

/* Haken in B[3] / B[4]: Steuerziel = Spieler (Original, playerX @0x80117d4c / @0x801181e0) oder das Umweg-Ziel. */
int re15_r36_affen_padsuche_ziel(const re15_actor_t *e, int slot, int sub, int32_t plx, int32_t plz, int32_t *tx, int32_t *tz)
{
    *tx = plx; *tz = plz;
    if (!s_r36_padsuche_an || !e || slot < 0 || slot >= RE15_ACTOR_MAX || (sub != 3 && sub != 4) || !s_r36_padsuche_aktiv[slot]) return 0;
    *tx = s_r36_ziel_x[slot]; *tz = s_r36_ziel_z[slot];
    return 1;
}

/* ----- Nachbesserung 1, Mangel 2: zonenloser RE2-Zombie -> Ziel = Spielerlage (Belege: re15_zombieki_r36.h) ----- */
#include "re15_enemy_ai.h"   /* re15_nav_zone_count / re15_nav_zone_from_pos */
static int s_r36_nav_an = 1;
void re15_r36_nav_zonenlos_enable(int an) { s_r36_nav_an = an ? 1 : 0; }
void re15_r36_nav_zonenlos(re15_actor_t *e, const re15_actor_t *pl)
{
    if (!s_r36_nav_an || !e || !pl) return;
    if (!re15_ai_re2_for_type(e->type) || !re15_re2z_owns_type(e->type)) return;   /* nur das RE2-Zombie-Hirn */
    if (re15_nav_zone_count() <= 0) return;                                      /* ohne Graph steuert der Navigator schon direkt */
    if (re15_nav_zone_from_pos((int16_t)e->x, (int16_t)e->z) != 0xffu) return;   /* in einer Zone: Original (FUN_8004aab8 != 0xff) */
    e->steer_x = (int16_t)pl->x;                                                 /* +0x1c4 := Ziel.x @0x8004a90c-914 (Direkt-Form) */
    e->steer_z = (int16_t)pl->z;                                                 /* +0x1c6 := Ziel.z @0x8004a918-924 */
}

/* NUR Sonden: Umweg-Ziel und Pad des Slots lesen. */
int re15_r36_affen_umweg_lesen(int slot, int32_t *x, int32_t *z, int *pad)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    *x = s_r36_ziel_x[slot]; *z = s_r36_ziel_z[slot]; *pad = s_r36_pad_idx[slot];
    return s_r36_padsuche_aktiv[slot];
}
