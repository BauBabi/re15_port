/* =============================================================================
 * SONDE r42/karte: BESTANDSAUFNAHME DER KARTE, JEDER RAUM (Leon UND Elza)
 * =============================================================================
 * Nutzer 2026-11-18: "das die [Map] fuer STAGE2-6 genauso sauber funktioniert wie bei
 * STAGE1 ... Inklusive dem - wo befindet sich Leon - Marker."
 *
 * Reine MESS-Sonde (kein Pin). Sie faehrt denselben Weg wie integration_map_raum_live
 * (Raum laden wie das Spiel, scd_room_reenter, Zone aus der Spielerposition, echte
 * Op-Liste mit re15_inv_screen_build, Marker mit re15_inv_map_marker), schreibt aber
 * ROHZAHLEN je Raum/Kamera/Tuer als Zeilen auf stdout. Die Bewertung (Marker auf den
 * gemalten Texeln? Tuer auf ihrem Symbol? Sprung beim Durchgang?) macht das Python-
 * Auswertewerkzeug, weil es die Kachel-Texel aus DATA/MAP0x.PIX liest.
 *
 * Anders als integration_map_raum_live:
 *   - JEDER Raum, auch die ohne Zone (die zaehlt der Pin nicht und versteckt sie damit);
 *   - beide Szenario-Varianten xxx0 (Leon) und xxx1 (Elza);
 *   - je KAMERA eine Stichprobe: Mitte der Kamera-Region (re15_rdt_get_region_quad,
 *     PSX FUN_80014324) - "Marker in jedem Raum an der richtigen Stelle";
 *   - nur der aktuelle Raum ist besucht (Schleier-Pruefung), kein Plan im Besitz.
 *
 * Zeilen:
 *   ROOM rid st room origpage ncut ndoor nsca nzone
 *   CAM  rid cut x z zpage zrect zidx zid synth shownpage mx my rot n_akt n_bes n_unb n_fill
 *   DOOR rid slot dest x z sx sz band | page mx my | mark mpage mx my kind ident | dpage dmx dmy drot
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

static re15_rdt_t s_rdt;
static uint8_t *s_roh = NULL;

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

/* Dieselbe Kette wie integration_map_raum_live (= Debug-JUMP). */
static int betrete(unsigned rid, int32_t px, int32_t pz, int band)
{
    char pfad[600];
    size_t n = 0;
    re15_actor_t *pl;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT",
             RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    if (s_roh) { free(s_roh); s_roh = NULL; }
    s_roh = slurp(pfad, &n);
    if (!s_roh) return 0;
    if (re15_rdt_parse(s_roh, n, &s_rdt) < 0) return 0;
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(1);
    re15_esp_fx_reset();
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

static int ist_kachel(const re15_inv_op_t *o)
{
    return o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_MAP4 && o->w >= 4 && o->h >= 4;
}
static int ist_flaeche(const re15_inv_op_t *o)
{
    if (o->w < 4 || o->h < 4) return 0;
    if (o->kind == RE15_INV_OP_FILL || o->kind == RE15_INV_OP_FILLMASK) return 1;
    return ist_kachel(o);
}
static int ist_rot(const re15_inv_op_t *o)
{
    if (!ist_flaeche(o)) return 0;
    if (o->kind == RE15_INV_OP_FILL || o->kind == RE15_INV_OP_FILLMASK) {
        if (o->abe && o->r == 200 && o->g == 16 && o->b == 16) return 1;
        if (o->r == 104 && o->g == 8 && o->b == 8) return 1;
        return o->r == 74 && o->g == 20 && o->b == 20;
    }
    return o->clut == RE15_INV_CLUT_MAP_AKTUELL;
}
static int oben_an(const re15_inv_op_t *ops, int n, int x, int y)
{
    int i;
    for (i = 0; i < n; i++) {
        if (!ist_flaeche(&ops[i])) continue;
        if (x < ops[i].x || x >= ops[i].x + ops[i].w) continue;
        if (y < ops[i].y || y >= ops[i].y + ops[i].h) continue;
        return i;
    }
    return -1;
}

static const int s_basis[6] = { 0, 38, 50, 65, 77, 98 };      /* @0x8004b568.. Seiten-Setzer */
static const int s_grenze[6] = { 0x26, 0xc, 0x20, 0xc, 0x15, 0x8 };  /* sltiu je Stage-Init */

/* Stellt den Spieler an (x,z) in Raum rid, nur dieser Ort besucht, Karte offen. */
static int schau(unsigned rid, int32_t x, int32_t z, int band,
                 re15_inv_op_t *ops, int max_ops, int *seite, int16_t *mx, int16_t *my,
                 int *rot, int *n_akt, int *n_bes, int *n_unb, int *n_fill,
                 const re15_map_zone_t **zone)
{
    int nops, k;
    unsigned st = (rid >> 12) & 0xF, r = (rid >> 4) & 0xFF;
    if (!betrete(rid, x, z, band)) return 0;
    re15_map_visited_reset();
    re15_map_visited_mark_at(rid, x, z);
    re15_map_zone_update(rid, x, z);
    *zone = re15_map_zone_current();
    if (st >= 1 && st <= 6) re15_inv_map_stage_init((int)st - 1, (int)r);
    re15_inv_screen_open();
    g_inv_screen.substate = 1;
    g_inv_screen.item_state = 1;
    g_inv_screen.map_page = re15_inv_map_page_shown();
    *seite = g_inv_screen.map_page;
    nops = re15_inv_screen_build(&g_inv_screen, ops, max_ops);
    *rot = *n_akt = *n_bes = *n_unb = *n_fill = 0;
    for (k = 0; k < nops; k++) {
        if (ist_kachel(&ops[k])) {
            if (ops[k].clut == RE15_INV_CLUT_MAP_AKTUELL) (*n_akt)++;
            else if (ops[k].clut == RE15_INV_CLUT_MAP_BESUCHT) (*n_bes)++;
            else if (ops[k].clut == RE15_INV_CLUT_MAP_UNBESUCHT) (*n_unb)++;
        } else if (ist_flaeche(&ops[k])) (*n_fill)++;
        if (!ist_rot(&ops[k])) continue;
        if (oben_an(ops, nops, ops[k].x + ops[k].w / 2, ops[k].y + ops[k].h / 2) == k) *rot = 1;
    }
    {
        unsigned slot = 0;
        if (st >= 1 && st <= 6) slot = (unsigned)(s_basis[st - 1] + (int)r);
        re15_inv_map_marker(x, z, (uint8_t)slot, mx, my);
    }
    return 1;
}

int main(void)
{
    static re15_inv_op_t ops[1024];
    static re15_aot_t sn_a[RE15_AOT_MAX];
    static re15_aot_door_params_t sn_d[RE15_AOT_MAX];
    unsigned st, r, v;
    const char *nur = getenv("RE15_BESTAND_STAGE");   /* z.B. "23" = nur STAGE2+3 */
    const char *punkte = getenv("RE15_BESTAND_PUNKTE");

    /* Stufe 3: Erwartung je Abzug an der TATSAECHLICHEN Lage aus dem Spiel-Log.
     * Datei mit Zeilen "<tag> <hexraum> <x> <z>" -> "PUNKT tag raum x z seite mx my zid rot". */
    if (punkte && *punkte) {
        FILE *f = fopen(punkte, "r");
        char tag[64];
        unsigned rid;
        int x, z;
        if (!f) { printf("FAIL: %s\n", punkte); return 1; }
        while (fscanf(f, "%63s %x %d %d", tag, &rid, &x, &z) == 4) {
            int16_t mx = 0, my = 0;
            int seite = -1, rot = 0, na, nb, nu, nf;
            const re15_map_zone_t *zn = NULL;
            if (!schau(rid, x, z, -1, ops, 1024, &seite, &mx, &my, &rot, &na, &nb, &nu, &nf, &zn)) {
                printf("PUNKT %s %04X %d %d -1 0 0 -1 0\n", tag, rid, x, z);
                continue;
            }
            printf("PUNKT %s %04X %d %d %d %d %d %d %d\n", tag, rid, x, z, seite, (int)mx, (int)my,
                   zn ? (int)zn->zid : -1, rot);
        }
        fclose(f);
        return 0;
    }

    printf("# r42 karte bestand\n");
    for (st = 1; st <= 6; st++) {
        if (nur && !strchr(nur, (int)('0' + st))) continue;
        for (r = 0; r < 0x40; r++) {
            for (v = 0; v < 2; v++) {
                unsigned rid = (st << 12) | (r << 4) | v;
                int ncut, ndoor = 0, k, origpage = -1, nzone = 0, i;
                int cut;
                if (!betrete(rid, 0, 0, -1)) continue;
                memcpy(sn_a, g_aot.slots, sizeof sn_a);
                memcpy(sn_d, g_aot.door_params, sizeof sn_d);
                for (k = 0; k < RE15_AOT_MAX; k++)
                    if (sn_a[k].active && sn_a[k].type == RE15_AOT_TYPE_DOOR) ndoor++;
                if ((int)r < s_grenze[st - 1]) {
                    re15_inv_map_stage_init((int)st - 1, (int)r);
                    origpage = re15_inv_map_page();
                }
                for (i = 0; i < re15_map_zone_count(); i++) {
                    const re15_map_zone_t *zz = re15_map_zone_by_index(i);
                    if (zz && zz->room == (unsigned short)rid && !zz->etage) nzone++;
                }
                ncut = s_rdt.cut_count;
                printf("ROOM %04X %u %02X %d %d %d %d %d\n", rid, st, r, origpage, ncut, ndoor,
                       (int)s_rdt.sca_count, nzone);
                /* ---- je Kamera eine Stichprobe ---- */
                for (cut = 0; cut < ncut; cut++) {
                    int16_t xs[4], zs[4], mx = 0, my = 0;
                    int32_t cx, cz;
                    int seite, rot, na, nb, nu, nf;
                    const re15_map_zone_t *zn = NULL;
                    if (!re15_rdt_get_region_quad(&s_rdt, cut, xs, zs)) continue;
                    cx = ((int32_t)xs[0] + xs[1] + xs[2] + xs[3]) / 4;
                    cz = ((int32_t)zs[0] + zs[1] + zs[2] + zs[3]) / 4;
                    if (!schau(rid, cx, cz, -1, ops, 1024, &seite, &mx, &my, &rot, &na, &nb, &nu, &nf, &zn))
                        continue;
                    printf("CAM %04X %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                           rid, cut, (int)cx, (int)cz,
                           zn ? (int)zn->page : -1, zn ? (int)zn->rect : -1,
                           zn ? (int)zn->idx : -1, zn ? (int)zn->zid : -1, zn ? (int)zn->synth : -1,
                           seite, (int)mx, (int)my, rot, na, nb, nu, nf);
                    /* zurueck in den Raum (schau hat ihn neu geladen) */
                    if (!betrete(rid, 0, 0, -1)) break;
                }
                /* ---- MITLAUFEN: Raster 7x7 je Zone, nur Punkte in einem RVD-Viereck
                 * (= dort steht die Kamera-Logik, also kann der Spieler dort sein) ---- */
                if (betrete(rid, 0, 0, -1)) {
                    for (i = 0; i < re15_map_zone_count(); i++) {
                        const re15_map_zone_t *zz = re15_map_zone_by_index(i);
                        int gx, gz;
                        if (!zz || zz->room != (unsigned short)rid || zz->etage) continue;
                        for (gx = 0; gx < 7; gx++) for (gz = 0; gz < 7; gz++) {
                            int32_t px = zz->wx0 + (int32_t)((int64_t)(zz->wx1 - zz->wx0) * (2 * gx + 1) / 14);
                            int32_t pz = zz->wz0 + (int32_t)((int64_t)(zz->wz1 - zz->wz0) * (2 * gz + 1) / 14);
                            int q, drin = 0, seite;
                            int16_t mx = 0, my = 0;
                            const re15_map_zone_t *zc;
                            for (q = 0; q < s_rdt.zone_count && !drin; q++) {
                                const re15_rdt_zone_t *Z = &s_rdt.zones[q];
                                int e, s = 0, ok = 1;
                                for (e = 0; e < 4 && ok; e++) {
                                    int64_t x1 = Z->xs[e], z1 = Z->zs[e], x2 = Z->xs[(e + 1) & 3], z2 = Z->zs[(e + 1) & 3];
                                    int64_t c = (x2 - x1) * (pz - z1) - (z2 - z1) * (px - x1);
                                    if (c != 0) { if (!s) s = c > 0 ? 1 : -1; else if ((c > 0) != (s > 0)) ok = 0; }
                                }
                                if (ok) drin = 1;
                            }
                            if (!drin) continue;
                            g_actors[RE15_ACTOR_SLOT_PLAYER].x = px;
                            g_actors[RE15_ACTOR_SLOT_PLAYER].z = pz;
                            re15_map_zone_update(rid, px, pz);
                            zc = re15_map_zone_current();
                            seite = re15_inv_map_page_shown();
                            g_inv_screen.map_page = (uint8_t)seite;
                            re15_inv_map_marker(px, pz, (uint8_t)(s_basis[st - 1] + (int)r), &mx, &my);
                            printf("FOLGE %04X %d %d %d %d %d %d %d\n", rid, (int)zz->idx, (int)px, (int)pz,
                                   zc ? (int)zc->idx : -1, seite, (int)mx, (int)my);
                        }
                    }
                }
                /* ---- je Tuer: Marker am Trigger, Marke, Ankunft im Zielraum ---- */
                for (k = 0; k < RE15_AOT_MAX; k++) {
                    const re15_aot_t *a = &sn_a[k];
                    const re15_aot_door_params_t *d = &sn_d[k];
                    unsigned ziel;
                    int seite, rot, na, nb, nu, nf, dseite = -1, drot = 0;
                    int16_t mx = 0, my = 0, dmx = -999, dmy = -999;
                    const re15_map_zone_t *zn = NULL, *dz = NULL;
                    int mk, mn, beste = -1, ident = 0, mpg = -1, mrc, mmx = -999, mmy = -999, kind = -1;
                    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
                    ziel = (((unsigned)d->dest_stage + 1u) << 12) | ((unsigned)d->dest_room << 4) | (rid & 0xFu);
                    if (!schau(rid, a->x, a->z, (int)d->band, ops, 1024, &seite, &mx, &my, &rot, &na, &nb, &nu, &nf, &zn))
                        continue;
                    /* Marke ueber die IDENTITAET (wie tuer_anziehen): meine Zone + eine Zone des Ziels */
                    mn = re15_map_mark_count();
                    if (zn) {
                        int einzeln = -1;
                        for (mk = 0; mk < mn; mk++) {
                            int pg, rc, sx2, sy2, kd, za, zb, andere, ix, gef = 0;
                            if (!re15_map_mark_get(mk, &pg, &rc, &sx2, &sy2, &kd)) continue;
                            if (pg != seite || kd >= 4) continue;
                            if (!re15_map_mark_zonen(mk, &za, &zb)) continue;
                            if (za != zn->zid && zb != zn->zid) continue;
                            andere = (za == zn->zid) ? zb : za;
                            if (andere == 255) { if (einzeln < 0) einzeln = mk; continue; }
                            for (ix = 0; ix < 8 && !gef; ix++) {
                                const re15_map_zone_t *zt = re15_map_zone_fuer(ziel, ix, (unsigned)seite);
                                if (zt && zt->zid == andere) gef = 1;
                            }
                            if (gef) { beste = mk; ident = 2; break; }
                        }
                        if (beste < 0 && einzeln >= 0) { beste = einzeln; ident = 1; }
                        if (beste >= 0) re15_map_mark_get(beste, &mpg, &mrc, &mmx, &mmy, &kind);
                    }
                    if (ziel != rid) {
                        if (!schau(ziel, d->spawn_x, d->spawn_z, (int)d->band, ops, 1024, &dseite, &dmx, &dmy,
                                   &drot, &na, &nb, &nu, &nf, &dz))
                            dseite = -2;
                    }
                    printf("DOOR %04X %d %04X %d %d %d %d %d | %d %d %d %d | %d %d %d %d %d %d | %d %d %d %d\n",
                           rid, k, ziel, (int)a->x, (int)a->z, (int)d->spawn_x, (int)d->spawn_z, (int)d->band,
                           seite, (int)mx, (int)my, rot,
                           beste, mpg, mmx, mmy, kind, ident,
                           dseite, (int)dmx, (int)dmy, drot);
                }
            }
        }
    }
    if (s_roh) free(s_roh);
    return 0;
}
