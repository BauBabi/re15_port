/* probe_r33_tueren.c — Riegel der Runde 33 / Thema T "restliche Tueren"
 * (analysis/befunde_runde33/tueren_rest_plan.md, tueren_rest_pilot.md).
 *
 * PORT-EIGENE Tuerarchive im RE2-Aufbau (shared_assets/RE15DOOR, tools/tueren/tuer_archiv_bauen.py,
 * Tabelle gen/re15_tuer_eigen.inc). ⛔ PORT-WAHL (Textur), keine Original-Adresse - gepinnt wird,
 * dass das Archiv NUR in der Textur vom RE2-Basis-Archiv abweicht und genauso laeuft.
 *
 * Teile (Aufruf mit dem Teilnamen, je ein ctest):
 *   archive   Je Port-Archiv: Datei da, Groesse/Sektor/FNV-1a wie die Tabelle; Tonteil und Modellteil
 *             bis zur TIM BYTEGLEICH dem Basis-Archiv (shared_assets/RE2/DOOR), TIM gleich lang;
 *             TIM 8 bit, 128x256, genau eine CLUT mit 256 Eintraegen; die Maschine laeuft jede in
 *             der Tabelle benutzte Variante bis zum Ende (Platz 10 aus, < 4000 Bilder, keine Notiz)
 *             und liefert Bild fuer Bild dieselben Objektlagen wie das Basis-Archiv (Pruefsumme wie
 *             probe_r31_tueren "maschine"), dieselben Se_on-Bilder und denselben Schliesston-Merker.
 *   zuordnung Jede Port-Zeile trifft einen echten Door_aot_set ihres RDT (Flaeche/Band wie
 *             op_door_aot_set) und findet ihre Wahl (Basis, Variante, eigen, Griff-Tausch mit Spender aus
 *             einem Port-Archiv, z. B. P1DG <- P07G); keine Flaeche doppelt
 *             mit anderer Wahl; im echten Spielschritt stellt eine G1-Tuer ROOM1000 -> ROOM1050 die
 *             Anfrage mit dem Port-Archiv P07G, der Raumwechsel steht danach noch an.
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

/* Pruefsumme eines Bildes ueber alle 10 Objekte (wie probe_r31_tueren.c bild_summe). */
static uint32_t fnv(uint32_t h, uint32_t w)
{
    for (int k = 0; k < 4; k++) { h ^= (w >> (8 * k)) & 0xFFu; h *= 16777619u; }
    return h;
}

static uint32_t bild_summe(const re15_door_seq_t *s)
{
    uint32_t h = 2166136261u;
    for (int i = 0; i < RE15_DOOR_OBJEKTE; i++) {
        const re15_door_obj_t *o = &s->obj[i];
        h = fnv(h, (uint32_t)i);
        if (!o->on) { h = fnv(h, 0); continue; }
        h = fnv(h, 1);
        h = fnv(h, o->mesh);
        h = fnv(h, o->flags);
        for (int a = 0; a < 3; a++) h = fnv(h, (uint32_t)o->pos[a]);
        for (int a = 0; a < 3; a++) h = fnv(h, o->rot[a]);
    }
    return h;
}

typedef struct { int bilder, n_ton, ton[16], schliess, notizen, md1_ok, tim_ok; uint32_t summe[4000]; } lauf_t;

static int laufen(const uint8_t *teil, int n, int variante, int tuer_nr, lauf_t *l)
{
    static re15_door_seq_t s;
    memset(l, 0, sizeof *l);
    if (re15_door_seq_start(&s, teil, n, variante, 0, tuer_nr) != 0) return -1;
    int bild = 0;
    while (bild < 4000 && re15_door_seq_bild(&s, 1)) {
        l->summe[bild] = bild_summe(&s);
        for (int t = 0; t < s.n_ton; t++) if (l->n_ton < 16) l->ton[l->n_ton++] = bild;
        bild++;
    }
    l->bilder = bild; l->schliess = s.schliesston; l->notizen = (int)s.notizen;
    l->md1_ok = s.md1_ok; l->tim_ok = s.tim_ok;
    re15_door_seq_ende(&s);
    return bild < 4000 ? 0 : -2;
}

/* ---------------------------------------------------------------------------------------------
 * Teil "archive"
 * ------------------------------------------------------------------------------------------ */
static void teil_archive(void)
{
    int n_eig = re15_door_seq_eigen_anzahl();
    PRUEF(n_eig >= 3, "nur %d Port-Archive (Pilot: P07G, P1DG, P16M)", n_eig);
    static lauf_t la, lb;
    int varianten_geprueft = 0;
    for (int e = 1; e <= n_eig; e++) {
        const re15_tuer_eigen_t *t = re15_door_seq_eigen(e);
        char pfad[600], pb[600];
        snprintf(pfad, sizeof pfad, "%s/RE15DOOR/%s.DO2", RE15_ASSET_SHARED_DIR, t->kennung);
        snprintf(pb, sizeof pb, "%s/DOOR/DOOR%02X.DO2", RE15_ASSET_RE2_DIR, t->basis);
        size_t n = 0, nb = 0;
        uint8_t *d = slurp(pfad, &n), *b = slurp(pb, &nb);
        PRUEF(d != NULL, "%s fehlt", pfad);
        PRUEF(b != NULL, "Basis %s fehlt", pb);
        if (!d || !b) { free(d); free(b); continue; }
        int ton = 0, modell = 0, sektor = 0, groesse = 0;
        PRUEF(re15_door_seq_re2_archiv(t->basis, &ton, &modell, &sektor, &groesse) == 0, "%s: Basis DOOR%02X ohne Tabelle",
              t->kennung, t->basis);
        PRUEF((int)n == (int)t->datei && t->sektor * 0x800u + t->modell == t->datei && fnv1a(d, n) == t->fnv,
              "%s: %u B / Sektor %u / Modell %u / FNV passt nicht zur Tabelle", t->kennung, (unsigned)n,
              (unsigned)t->sektor, (unsigned)t->modell);
        /* Basis gegen @0x8009a520: das Port-Archiv hat dieselbe Aufteilung wie sein Basis-Archiv */
        PRUEF(n == nb && t->ton == ton && t->modell == modell && (int)t->sektor == sektor && groesse == (int)nb,
              "%s: Groessen weichen vom Basis-Archiv DOOR%02X ab", t->kennung, t->basis);
        if (n != nb || (int)n != groesse) { free(d); free(b); continue; }
        const uint8_t *tm = d + t->sektor * 0x800, *bm = b + t->sektor * 0x800;
        uint32_t tim_rel = rd32(tm + 4);
        PRUEF(rd32(bm + 4) == tim_rel && rd32(bm) == rd32(tm), "%s: MD1-/TIM-Versatz anders", t->kennung);
        PRUEF(memcmp(d, b, t->ton) == 0, "%s: Tonteil nicht bytegleich dem Basis-Archiv", t->kennung);
        PRUEF(memcmp(tm, bm, tim_rel) == 0, "%s: Kopf/SCD/MD1 nicht bytegleich dem Basis-Archiv", t->kennung);
        size_t diff = 0;
        for (size_t k = t->sektor * 0x800 + tim_rel; k < n; k++) diff += d[k] != b[k];
        re15_tim_t tim;
        int tok = re15_tim_parse(tm + tim_rel, (int)(t->modell - tim_rel), &tim) == 0;
        PRUEF(tok && tim.bpp == 8 && tim.width == 128 && tim.height == 256 && tim.has_clut && tim.clut_entries == 256,
              "%s: TIM nicht 8 bit / 128x256 / 1 CLUT (bpp %d %dx%d CLUT %d)", t->kennung, tim.bpp, tim.width,
              tim.height, tim.clut_entries);
        printf("%s: Basis DOOR%02X, %u B, Tonteil+Kopf+SCD+MD1 bytegleich, TIM %d bit %dx%d CLUT %d, "
               "%u TIM-Bytes neu, FNV %08X\n", t->kennung, t->basis, (unsigned)n, tim.bpp, tim.width, tim.height,
               tim.clut_entries, (unsigned)diff, (unsigned)t->fnv);
        PRUEF(diff > 0, "%s: TIM gleich dem Basis-Archiv (keine neue Textur)", t->kennung);
        /* jede benutzte Variante: Port gegen Basis, Bild fuer Bild */
        int gesehen[16] = {0};
        for (int i = 0; i < re15_door_seq_zeilen(); i++) {
            const re15_tuer_zeile_t *z = re15_door_seq_zeile(i);
            if (z->eigen != e || z->variante >= 16 || gesehen[z->variante]) continue;
            gesehen[z->variante] = 1;
            int ra = laufen(tm, t->modell, z->variante, t->basis, &la);
            int rb = laufen(bm, t->modell, z->variante, t->basis, &lb);
            int gleich = ra == 0 && rb == 0 && la.bilder == lb.bilder && la.n_ton == lb.n_ton
                         && !memcmp(la.ton, lb.ton, sizeof la.ton) && la.schliess == lb.schliess
                         && !memcmp(la.summe, lb.summe, sizeof la.summe[0] * (size_t)la.bilder);
            printf("  %s V%d: %d Bilder (Basis %d), %d Se_on, Schliesston %d, Notizen 0x%x, md1 %d tim %d -> %s\n",
                   t->kennung, z->variante, la.bilder, lb.bilder, la.n_ton, la.schliess, la.notizen, la.md1_ok,
                   la.tim_ok, gleich ? "gleich" : "ANDERS");
            PRUEF(gleich, "%s V%d: laeuft anders als DOOR%02X", t->kennung, z->variante, t->basis);
            PRUEF(la.notizen == 0 && la.md1_ok && la.tim_ok && la.bilder > 100, "%s V%d: Notizen/Format", t->kennung,
                  z->variante);
            varianten_geprueft++;
        }
        free(d); free(b);
    }
    PRUEF(varianten_geprueft >= 6, "nur %d Archiv-Varianten geprueft", varianten_geprueft);
    printf("Port-Archive: %d, Varianten gegen das Basis-Archiv: %d\n", n_eig, varianten_geprueft);
}

/* ---------------------------------------------------------------------------------------------
 * Spielschritt-Geruest (wie probe_r31_tueren.c)
 * ------------------------------------------------------------------------------------------ */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_audio_event_t e;
    scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    while (scd_audio_queue_pop(&e)) { }
}

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
    s_shown = 0;
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

/* ---------------------------------------------------------------------------------------------
 * Teil "zuordnung"
 * ------------------------------------------------------------------------------------------ */
static re15_door_seq_anfrage_t s_gefangen;
static int s_n_gefangen = 0;
static void fang(const re15_door_seq_anfrage_t *a) { s_gefangen = *a; s_n_gefangen++; }

static int16_t le16(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }

static void teil_zuordnung(void)
{
    int n = re15_door_seq_zeilen(), eig = 0, treffer = 0, seiten = 0, tueren = 0;
    for (int i = 0; i < n; i++) {
        const re15_tuer_zeile_t *t = re15_door_seq_zeile(i);
        if (!t->eigen) continue;
        eig++;
        const re15_tuer_eigen_t *e = re15_door_seq_eigen(t->eigen);
        PRUEF(e && e->basis == t->re2_nr, "Zeile %d S%03u: eigen %d ohne Archiv/Basis", i, t->seite, t->eigen);
        char rp[600];
        snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(t->raum >> 12), t->raum);
        size_t sz = 0;
        uint8_t *b = slurp(rp, &sz);
        int ok = 0;
        if (b && t->off + 40 <= sz && b[t->off] == 0x3B) {
            const uint8_t *pc = b + t->off;
            if (t->form == 0 && !(pc[3] & 0x80)) {
                int16_t rx = le16(pc + 6), rz = le16(pc + 8), rw = le16(pc + 10), rd = le16(pc + 12);
                ok = pc[4] == t->band && (int32_t)rx + (int32_t)rw / 2 == t->x && (int32_t)rz + (int32_t)rd / 2 == t->z
                     && (int32_t)(rw < 0 ? -rw : rw) / 2 == t->hw && (int32_t)(rd < 0 ? -rd : rd) / 2 == t->hh;
            } else if (t->form == 1 && (pc[3] & 0x80)) {
                ok = pc[4] == t->band;
                for (int k = 0; k < 4; k++) ok = ok && le16(pc + 6 + 4 * k) == t->qx[k] && le16(pc + 8 + 4 * k) == t->qz[k];
            }
        }
        free(b);
        PRUEF(ok, "Zeile %d S%03u ROOM%04X @0x%X trifft keinen passenden Door_aot_set", i, t->seite, t->raum, (unsigned)t->off);
        treffer += ok;
        re15_door_seq_anfrage_t q;
        int r = re15_door_seq_zuordnen_flaeche(t->raum, t->form, t->x, t->z, t->hw, t->hh, t->qx, t->qz, t->band, &q);
        PRUEF(r == RE15_DOOR_ARCHIV_RE2 && q.eigen == t->eigen && q.re2_nr == t->re2_nr && q.tuer_nr == t->re2_nr
              && q.variante == t->variante && q.spender == t->spender,
              "Zeile %d S%03u: findet ihre Wahl nicht (eigen %d/%d)", i, t->seite, q.eigen, t->eigen);
        if (t->spender != RE15_DOOR_KEIN_SPENDER) {
            /* Griff-Tausch eines Port-Archivs: Satz vorhanden, Spender-Archiv (Port oder RE2) mit Basis = Spender */
            const re15_griff_tausch_t *g = re15_door_seq_griff_tausch(t->re2_nr, t->spender);
            const re15_tuer_eigen_t *se = g ? re15_door_seq_eigen(g->spender_eigen) : NULL;
            PRUEF(g && (g->spender_eigen == 0 || (se && se->basis == t->spender)),
                  "S%03u: Griff-Tausch DOOR%02X <- %02X fehlt/Spender-Archiv falsch", t->seite, t->re2_nr, t->spender);
        }
        int neu = 1, neu_t = 1;
        for (int k = 0; k < i; k++) {
            const re15_tuer_zeile_t *u = re15_door_seq_zeile(k);
            if (u->seite == t->seite) neu = 0;
            if (u->tuer == t->tuer && u->eigen) neu_t = 0;
            /* gleiche Flaeche in einer anderen Zeile: gleiche Wahl (sonst gewinnt die erste) */
            if (u->raum == t->raum && u->form == t->form && u->band == t->band && u->x == t->x && u->z == t->z
                && u->hw == t->hw && u->hh == t->hh && !memcmp(u->qx, t->qx, sizeof u->qx) && !memcmp(u->qz, t->qz, sizeof u->qz))
                PRUEF(u->eigen == t->eigen && u->variante == t->variante && u->re2_nr == t->re2_nr,
                      "S%03u: Flaeche schon in Zeile %d (S%03u) mit anderer Wahl", t->seite, k, u->seite);
        }
        seiten += neu; tueren += neu_t;
    }
    printf("Port-Zeilen: %d, %d treffen ihren Satz, %d Tuerseiten, %d Tueren\n", eig, treffer, seiten, tueren);
    PRUEF(eig >= 72 && seiten >= 36 && tueren >= 17, "Pilot: erwartet >= 72 Zeilen / 36 Seiten / 17 Tueren");

    /* echter Spielschritt: G1-Tuer ROOM1000 -> ROOM1050 (T000..T002) stellt die Anfrage mit P07G */
    re15_door_seq_setze_laeufer(fang);
    if (room_boot(0x1000) == 0) {
        int erledigt = 0;
        for (int i = 0; i < RE15_AOT_MAX && !erledigt; i++) {
            const re15_aot_t *a = &g_aot.slots[i];
            const re15_aot_door_params_t *d = &g_aot.door_params[i];
            if (!a->active || a->type != RE15_AOT_TYPE_DOOR || d->dest_room != 0x05 || d->dest_stage != 0
                || !(a->half_w || a->half_h)) continue;
            re15_door_seq_anfrage_t soll;
            if (re15_door_seq_zuordnen_flaeche(0x1000, a->has_quad, a->x, a->z, a->half_w, a->half_h, a->xs, a->zs,
                                               d->band, &soll) != RE15_DOOR_ARCHIV_RE2) continue;
            s_n_gefangen = 0; memset(&s_gefangen, 0, sizeof s_gefangen);
            int gefunden = 0;
            int wechsel = durchgehen(i, &gefunden);
            const re15_tuer_eigen_t *e = re15_door_seq_eigen(s_gefangen.eigen);
            printf("ROOM1000 Slot %d (S%03u): Standplatz %d, Raumwechsel %d -> ROOM%04X, Laeufer %d x: DOOR%02X "
                   "V%d eigen %d (%s)\n", i, soll.seite, gefunden, wechsel, g_room_change.room_id, s_n_gefangen,
                   s_gefangen.re2_nr, s_gefangen.variante, s_gefangen.eigen, e ? e->kennung : "-");
            if (!gefunden) continue;
            erledigt = 1;
            PRUEF(wechsel && g_room_change.room_id == 0x1050, "ROOM1000: kein Durchgang nach ROOM1050");
            PRUEF(s_n_gefangen == 1 && s_gefangen.archiv == RE15_DOOR_ARCHIV_RE2 && s_gefangen.re2_nr == 0x07
                  && s_gefangen.variante == soll.variante && e && !strcmp(e->kennung, "P07G"),
                  "ROOM1000: Anfrage mit Port-Archiv P07G fehlt/falsch");
            PRUEF(g_room_change.pending, "ROOM1000: Raumwechsel muss nach der Sequenz noch anstehen");
            g_room_change.pending = 0;
        }
        PRUEF(erledigt, "ROOM1000: keine begehbare G1-Tuer nach ROOM1050");
    }
    re15_door_seq_setze_laeufer(NULL);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "archive";
    if (!strcmp(teil, "archive")) teil_archive();
    else if (!strcmp(teil, "zuordnung")) teil_zuordnung();
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf(g_fehler ? "probe_r33_tueren %s: %d FEHLER\n" : "probe_r33_tueren %s: OK\n", teil, g_fehler);
    return g_fehler ? 1 : 0;
}
