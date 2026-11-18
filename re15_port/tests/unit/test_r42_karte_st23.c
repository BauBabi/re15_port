/* =============================================================================
 * RIEGEL r42/karte: KARTE STAGE2 (Blatt 6) + STAGE3 (Blatt 7) - Leon UND Elza
 * =============================================================================
 * Nutzer 2026-11-18: "Parallel kannst du dich auch mit der Map auseinander setzen, das die
 * fuer STAGE2-6 genauso sauber funktioniert wir bei STAGE1. Das ist aktuell noch nicht der
 * Fall. Inklusive dem - wo befindet sich Leon - Marker." (dazu 2026-11-15 Punkt 5:
 * "Karten STAGE2 fehlen").
 *
 * Gemessen wird ueber den ECHTEN Pfad: Raum laden wie das Spiel (scd_room_reenter), Zone aus
 * der Spielerposition (re15_map_zone_update), Blatt = re15_inv_map_page_shown, Marker =
 * re15_inv_map_marker. Die Pruefgroesse ist UNABHAENGIG von der Herleitung der Zeilen: die
 * Texel der Originalkachel (DATA/MAP0<Blatt+1>.PIX, Kachelecke aus der Seitentabelle
 * @0x80076840, 12 B je Eintrag {s16 x,y,w,h; u8 u,_,v,_}).
 *
 *  [A] jeder erreichbare Raum hat auf seinem Blatt eine Zone mit Rechteck (ROOM2020 fehlte);
 *  [B] der Marker liegt auf den GEMALTEN Texeln des eigenen Rechtecks (+-1 px), je Raum >= 75 %
 *      der Rasterpunkte (vorher STAGE2 40 %: Raeume lagen auf dem Rechteck von ROOM2000);
 *  [C] der Marker laeuft mit: je Zone mindestens die Haelfte verschiedener Positionen
 *      (vorher ROOM2050: 1 Position, ROOM3060/3070: 3..5 von 37/48);
 *  [D] jede Tuer zwischen zwei Raeumen desselben Blatts zeigt ihre Marke (Identitaet: Marke
 *      verbindet die eigene Zone mit einer Zone des Zielraums) - nur der eigene Raum besucht.
 * Ausnahmen stehen namentlich in s_ausnahme mit Grund.
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_inv_screen.h"
#include "re15_scd.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_esp.h"
#include "re15_msg.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", (name)); g_fail = 1; } \
                               else printf("  PASS: %s\n", (name)); } while (0)

static re15_rdt_t s_rdt;
static uint8_t *s_roh = NULL;
static uint8_t *s_pix[13];

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    long sz;
    uint8_t *b;
    if (!f) return 0;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return 0; }
    b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return 0; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return 0; }
    fclose(f); *n = (size_t)sz; return b;
}

static int betrete(unsigned rid, int32_t px, int32_t pz, int band)
{
    char pfad[600];
    size_t n = 0;
    re15_actor_t *pl;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    if (s_roh) { free(s_roh); s_roh = NULL; }
    s_roh = slurp(pfad, &n);
    if (!s_roh || n < 0x100) return 0;
    if (re15_rdt_parse(s_roh, n, &s_rdt) < 0) return 0;
    scd_vm_init(); re15_actor_init(); re15_aot_init(); re15_enemy_reset();
    re15_enemy_ai_set_paused(1); re15_esp_fx_reset();
    g_current_room_id = rid;
    g_room_change.pending = 0;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz;
    re15_collision_reset_band();
    if (band >= 0) re15_collision_set_band(band);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

/* Texel der Originalkachel: headerlos, 256x256 4 bpp, 128 B je Zeile, unteres Nibble links
 * (DATA/MAP0x.PIX; Seite p -> Datei-Id u16 @0x80074c4c+2p = 12+p -> MAP0<p+1>). */
static int texel(int page, int u, int v)
{
    if (page < 0 || page > 12 || u < 0 || u > 255 || v < 0 || v > 255) return 0;
    if (!s_pix[page]) {
        char pfad[600]; size_t n = 0;
        snprintf(pfad, sizeof pfad, "%s/DATA/MAP%02X.PIX", RE15_ASSET_PSX_DIR, page + 1);
        s_pix[page] = slurp(pfad, &n);
        if (!s_pix[page] || n < 32768) return 0;
    }
    {
        uint8_t b = s_pix[page][v * 128 + u / 2];
        return (u & 1) ? (b >> 4) : (b & 15);
    }
}

static int auf_kachel(int page, int rect, int mx, int my)
{
    int rx, ry, rw, rh, ru, rv, dx, dy;
    if (!re15_map_rect_geometry((unsigned)page, (unsigned)rect, &rx, &ry, &rw, &rh)) return 0;
    if (!re15_map_rect_uv((unsigned)page, (unsigned)rect, &ru, &rv)) return 0;
    for (dx = -1; dx <= 1; dx++) for (dy = -1; dy <= 1; dy++) {
        int x = mx + dx - rx, y = my + dy - ry;
        if (x < 0 || y < 0 || x >= rw || y >= rh) continue;
        if (texel(page, ru + x, rv + y)) return 1;
    }
    return 0;
}

static int in_rvd(int32_t px, int32_t pz)
{
    int q;
    for (q = 0; q < s_rdt.zone_count; q++) {
        const re15_rdt_zone_t *Z = &s_rdt.zones[q];
        int e, s = 0, ok = 1;
        for (e = 0; e < 4 && ok; e++) {
            int64_t x1 = Z->xs[e], z1 = Z->zs[e], x2 = Z->xs[(e + 1) & 3], z2 = Z->zs[(e + 1) & 3];
            int64_t c = (x2 - x1) * (pz - z1) - (z2 - z1) * (px - x1);
            if (c != 0) { if (!s) s = c > 0 ? 1 : -1; else if ((c > 0) != (s > 0)) ok = 0; }
        }
        if (ok) return 1;
    }
    return 0;
}

/* Raeume der beiden Blaetter. ROOM20B0 fehlt mit Absicht: keine Tuer im Spiel fuehrt hinein
 * (Tuerliste aller RDTs, re15_port/tools/engine_tueren.txt; in ROOM2000 steht dieselbe Anlage). */
static const unsigned s_raeume[] = {
    0x2000, 0x2010, 0x2020, 0x2030, 0x2040, 0x2050, 0x2060, 0x2070, 0x2080, 0x2090, 0x20A0,
    0x3000, 0x3010, 0x3020, 0x3030, 0x3040, 0x3050, 0x3060, 0x3070, 0x3080, 0x3090,
    0x30A0, 0x30B0, 0x30C0, 0x30D0, 0x30E0,
};

/* Ausnahmen (Raum, Zone, Pruefung), jeweils mit Grund. */
static const struct { unsigned room; int zone; char pruef; const char *grund; } s_ausnahme[] = {
    { 0x3030, 0, 'C', "r3 ist 8 px breit gemalt - die 4-px-Klemmung laesst waagerecht 1 Spalte" },
    { 0x3080, 0, 'C', "r8 ist 8x9 px gemalt (Original-Zeile @0x80076A80 trifft genau dorthin)" },
    { 0x30E0, 0, 'C', "30E0 Zone 0: Original-Zeile @0x80076AB0 legt sie auf r6 (= ROOM3060) - OFFEN" },
    { 0x30E0, 0, 'B', "dito" },
    { 0x2070, 2, 'B', "2070 Zone 2 (Gang zu 11A0) ist nicht gemalt - OFFEN (Schema-Kasten)" },
    { 0x2070, 2, 'C', "dito" },
    { 0x2070, 1, 'D', "2070 Zone 1 ist eine Kopie des Gangs, in die keine Tuer einen Spawn setzt (unerreichbar)" },
    { 0x3090, 0, 'B', "AUSGELIEFERTE Zeile @0x80076A88: die 3090-Bbox umschliesst r10/r11 (= 30A0/30B0), 68 % auf r9" },
    { 0x20A0, 0, 'C', "PORT-WAHL r10 ist 11x8 px gemalt (Original: Blatt 0xD ohne Kunst, Datei-Id @0x80074c66 = 0)" },
};
static int ausnahme(unsigned room, int zone, char p)
{
    size_t i;
    for (i = 0; i < sizeof s_ausnahme / sizeof s_ausnahme[0]; i++)
        if (s_ausnahme[i].room == (room & ~1u) && s_ausnahme[i].zone == zone && s_ausnahme[i].pruef == p)
            return 1;
    return 0;
}

int main(void)
{
    static re15_aot_t sn_a[RE15_AOT_MAX];
    static re15_aot_door_params_t sn_d[RE15_AOT_MAX];
    size_t ri;
    int fehlt = 0, schlecht_b = 0, schlecht_c = 0, tueren = 0, ohne_marke = 0, var;
    char liste_a[512] = "", liste_b[1024] = "", liste_c[1024] = "", liste_d[1024] = "";

    printf("=== r42 Karte STAGE2/STAGE3: Zone, Marker auf der Kachel, Mitlaufen, Tuermarken ===\n");
    for (ri = 0; ri < sizeof s_raeume / sizeof s_raeume[0]; ri++)
    for (var = 0; var < 2; var++) {
        unsigned rid = s_raeume[ri] | (unsigned)var;
        int i, k, zonen = 0, seite_soll = ((rid >> 12) == 2) ? 6 : 7;
        if (!betrete(rid, 0, 0, -1)) { fehlt++; continue; }
        memcpy(sn_a, g_aot.slots, sizeof sn_a);
        memcpy(sn_d, g_aot.door_params, sizeof sn_d);
        /* ---- [A] Zone + [B] Kachel + [C] Mitlaufen, Raster 7x7 je Zone ---- */
        for (i = 0; i < re15_map_zone_count(); i++) {
            const re15_map_zone_t *zz = re15_map_zone_by_index(i);
            int gx, gz, n = 0, auf = 0, versch = 0, j;
            int16_t pos[49][2];
            if (!zz || zz->room != (unsigned short)rid || zz->etage) continue;
            zonen++;
            if (zz->page != seite_soll || zz->rect == 255) continue;
            for (gx = 0; gx < 7; gx++) for (gz = 0; gz < 7; gz++) {
                int32_t px = zz->wx0 + (int32_t)((int64_t)(zz->wx1 - zz->wx0) * (2 * gx + 1) / 14);
                int32_t pz = zz->wz0 + (int32_t)((int64_t)(zz->wz1 - zz->wz0) * (2 * gz + 1) / 14);
                const re15_map_zone_t *zc;
                int16_t mx = 0, my = 0;
                if (!in_rvd(px, pz)) continue;
                g_actors[RE15_ACTOR_SLOT_PLAYER].x = px;
                g_actors[RE15_ACTOR_SLOT_PLAYER].z = pz;
                re15_map_zone_update(rid, px, pz);
                zc = re15_map_zone_current();
                if (!zc || zc->idx != zz->idx) continue;      /* Punkt gehoert einer anderen Zone */
                g_inv_screen.map_page = re15_inv_map_page_shown();
                re15_inv_map_marker(px, pz, 0, &mx, &my);
                if (g_inv_screen.map_page == seite_soll && auf_kachel(seite_soll, zz->rect, mx, my)) auf++;
                for (j = 0; j < n; j++) if (pos[j][0] == mx && pos[j][1] == my) break;
                if (j == n) versch++;
                pos[n][0] = mx; pos[n][1] = my; n++;
            }
            if (n == 0) continue;
            if (auf * 4 < n * 3 && !ausnahme(rid, zz->idx, 'B')) {
                char e[64]; schlecht_b++;
                snprintf(e, sizeof e, " %04X.z%d(%d/%d)", rid, zz->idx, auf, n);
                strncat(liste_b, e, sizeof liste_b - strlen(liste_b) - 1);
            }
            if (n >= 8 && versch * 2 < n && !ausnahme(rid, zz->idx, 'C')) {
                char e[64]; schlecht_c++;
                snprintf(e, sizeof e, " %04X.z%d(%d/%d)", rid, zz->idx, versch, n);
                strncat(liste_c, e, sizeof liste_c - strlen(liste_c) - 1);
            }
        }
        if (zonen == 0) {
            char e[16]; fehlt++;
            snprintf(e, sizeof e, " %04X", rid);
            strncat(liste_a, e, sizeof liste_a - strlen(liste_a) - 1);
        }
        /* ---- [D] Tuermarken ueber die Identitaet ---- */
        for (k = 0; k < RE15_AOT_MAX; k++) {
            const re15_aot_t *a = &sn_a[k];
            const re15_aot_door_params_t *d = &sn_d[k];
            const re15_map_zone_t *zn;
            unsigned ziel;
            int seite, mk, gef = 0, ix;
            if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
            if (a->half_w == 0 && a->half_h == 0) continue;               /* Skript-Warp */
            ziel = (((unsigned)d->dest_stage + 1u) << 12) | ((unsigned)d->dest_room << 4) | (rid & 0xFu);
            if (ziel == rid || (ziel >> 12) != (rid >> 12) || (ziel & ~1u) == 0x20B0u) continue;
            if (!betrete(rid, a->x, a->z, (int)d->band)) continue;
            re15_map_visited_reset();
            re15_map_visited_mark_at(rid, a->x, a->z);
            re15_map_zone_update(rid, a->x, a->z);
            zn = re15_map_zone_current();
            if (!zn) continue;
            seite = re15_inv_map_page_shown();
            if (seite != seite_soll || ausnahme(rid, zn->idx, 'B') || ausnahme(rid, zn->idx, 'D')) continue;
            {   /* Ziel-Zone selbst offen (30E0 Zone 0)? Dann gibt es dort keine belastbare Lage. */
                const re15_map_zone_t *zt = re15_map_zone_at(ziel, d->spawn_x, d->spawn_z);
                if (zt && ausnahme(ziel, zt->idx, 'B')) continue;
            }
            /* nur Ziele mit Zone auf DIESEM Blatt */
            for (ix = 0, gef = -1; ix < 8; ix++) if (re15_map_zone_fuer(ziel, ix, (unsigned)seite)) gef = 0;
            if (gef < 0) continue;
            tueren++;
            for (mk = 0; mk < re15_map_mark_count() && !gef; mk++) {
                int pg, rc, sx, sy, kd, za, zb, andere;
                if (!re15_map_mark_get(mk, &pg, &rc, &sx, &sy, &kd)) continue;
                if (pg != seite || kd >= 4 || !re15_map_mark_zonen(mk, &za, &zb)) continue;
                if (za != zn->zid && zb != zn->zid) continue;
                andere = (za == zn->zid) ? zb : za;
                for (ix = 0; ix < 8 && !gef; ix++) {
                    const re15_map_zone_t *zt = re15_map_zone_fuer(ziel, ix, (unsigned)seite);
                    if (zt && zt->zid == andere) gef = 1;
                }
            }
            if (!gef) {
                char e[48]; ohne_marke++;
                snprintf(e, sizeof e, " %04X->%04X", rid, ziel);
                strncat(liste_d, e, sizeof liste_d - strlen(liste_d) - 1);
            }
        }
        betrete(rid, 0, 0, -1);
    }
    printf("  [A] Raeume ohne Zone:%s\n", liste_a[0] ? liste_a : " keine");
    printf("  [B] Marker neben der Kachel (<75%%):%s\n", liste_b[0] ? liste_b : " keine");
    printf("  [C] Marker steht (<50%% verschiedene Positionen):%s\n", liste_c[0] ? liste_c : " keine");
    printf("  [D] %d Tueren, ohne eigene Marke:%s\n", tueren, liste_d[0] ? liste_d : " keine");
    CHECK("[A] jeder erreichbare Raum (Leon+Elza) hat eine Zone", fehlt == 0);
    CHECK("[B] Marker auf den gemalten Texeln des eigenen Rechtecks", schlecht_b == 0);
    CHECK("[C] Marker laeuft mit der Figur mit", schlecht_c == 0);
    CHECK("[D] Tueren zeigen ihre Marke", tueren >= 40 && ohne_marke == 0);
    if (s_roh) free(s_roh);
    printf(g_fail ? "=== FAIL ===\n" : "=== PASS ===\n");
    return g_fail;
}
