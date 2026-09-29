/* probe_r31_tueren.c — Riegel der Runde 31 "Tueren" (analysis/befunde_runde31/tueren_04_bau.md).
 *
 * Teile (Aufruf mit dem Teilnamen, je ein ctest):
 *   viereck   Die 40-Byte-Tuersaetze ROOM4030/4031 @0x47E / @0x4A6 (sat 0xB1): Punkte pc+6..21,
 *             Nutzlast pc+22 (@0x80042f90 addiu a0,s0,20), Trefftest FUN_80014368. Vorher las der
 *             Port sie mit dem 32-B-Schema und stellte Tueren mit Ziel "BF0A0"/"C5F00" auf
 *             (re15_port/tools/engine_tueren.txt Zeilen 184/185). Geprueft: Installation (Punkte,
 *             Ziel, Lage, Cut), der Trefftest Befehl fuer Befehl, und ein echter Durchgang im
 *             Spielschritt (Standplatz vor der Tuer, QUADRAT) -> Raumwechsel nach ROOM4040 bzw.
 *             ROOM4080 mit der Lage aus der Nutzlast.
 *   maschine  Die Tuermaschine (engine/src/door_seq_common.c) gegen den Katalog-Simulator fuer JEDE
 *             benutzte Archiv-Variante (43 Paare), gelesen aus den Kopien shared_assets/RE2/DOOR:
 *             je Bild Pruefsumme ueber alle 10 Objekte, Se_on-Bilder, Bildzahl, Schliesston-Merker;
 *             ausserdem: Skript 0 verteilt die Variante (Case, nicht Default), keine Notiz
 *             (unbekannter Opcode/Feld). Referenz tests/unit/gen/r31_tuer_referenz.inc
 *             (tools/tueren/tuer_maschine_referenz.py).
 *   zuordnung Die Port-Tabelle (gen/tuer_zuordnung.inc): jede Zeile trifft einen echten Door_aot_set
 *             ihres RDT (Offset in der Zeile, Flaeche/Band so gerechnet wie op_door_aot_set) und
 *             findet ihre Wahl; eine Kreuz-Raum-Tuer mit Griff-Tausch (T033/T034/T027) stellt
 *             im echten Spielschritt die Anfrage (Laeufer-Attrappe) und der Raumwechsel steht danach
 *             noch an; eine nicht abgedeckte Tuer (seit Runde 33 ROOM1050 -> ROOM1090) nicht; das Tor bleibt beim Tor-Archiv; die
 *             Intro-Uebergabe (Null-Rechteck) nicht; Viereck S225 ja, S226 nein.
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
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_door_seq.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fehler = 0;
#define PRUEF(c, ...) do { if (!(c)) { g_fehler++; printf("FEHLER: " __VA_ARGS__); printf("\n"); } } while (0)

/* ---------------------------------------------------------------------------------------------
 * Spielschritt-Geruest (wie probe_r30_tuer_verschlossen.c): Raum booten, Einschwingen, Bilder.
 * ------------------------------------------------------------------------------------------ */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

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
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR,
             (unsigned)(room >> 12), (unsigned)room);
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

/* Vorwaertspunkt 620 vor dem Spieler (FUN_80042bac @0x80042bd0 ori 0x26c) in der Tuerflaeche? */
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

/* Standplatz vor der Tuer suchen (acht Blickrichtungen x fuenf Zielpunkte), den die
 * Kollision nicht verschiebt, und dort QUADRAT druecken. Rueckgabe 1 = Raumwechsel angefragt. */
static int durchgehen(int slot, int *gefunden_out)
{
    re15_aot_t *a = &g_aot.slots[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int band = g_aot.door_params[slot].band;
    long ax = a->x, az = a->z;
    if (a->has_quad) {
        ax = ((long)a->xs[0] + a->xs[1] + a->xs[2] + a->xs[3]) / 4;
        az = ((long)a->zs[0] + a->zs[1] + a->zs[2] + a->zs[3]) / 4;
    }
    static const int dxf[5] = { 0, 1, -1, 0, 0 }, dzf[5] = { 0, 0, 0, 1, -1 };
    int gefunden = 0;
    for (int k = 0; k < 5 && !gefunden; k++) {
        long zx = ax + dxf[k] * (long)(a->has_quad ? 150 : a->half_w / 2);
        long zz = az + dzf[k] * (long)(a->has_quad ? 150 : a->half_h / 2);
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
 * Teil "viereck"
 * ------------------------------------------------------------------------------------------ */
typedef struct {
    int slot;
    int16_t xs[4], zs[4];
    uint8_t stage, raum, cut;
    int16_t x, y, z, dir;
} viereck_soll_t;

/* Satzbytes selbst gelesen (ROOM4030.RDT == ROOM4031.RDT an beiden Stellen):
 * @0x47E 3b 01 02 b1 00 00 | 82 a1 6e 9c ae 9d 40 98 fa 97 7c 9d 5e 9d 86 a2 |
 *        be 0a 00 00 86 24 00 06 | 03 04 0c 00 | 00 00 ...
 * @0x4A6 3b 02 02 b1 00 00 | f6 af f0 a1 38 b4 12 9e fc ae a4 98 b0 aa 68 9d |
 *        c4 f0 00 00 34 08 00 02 | 03 08 08 00 | 00 00 ... */
static const viereck_soll_t s_viereck[2] = {
    { 1, { -24190, -25170, -26630, -25250 }, { -25490, -26560, -25220, -23930 },
      3, 0x04, 12, 2750, 0, 9350, 1536 },
    { 2, { -20490, -19400, -20740, -21840 }, { -24080, -25070, -26460, -25240 },
      3, 0x08, 8, -3900, 0, 2100, 512 },
};

static void teil_viereck(void)
{
    /* 1. Trefftest Befehl fuer Befehl (FUN_80014368) */
    {
        const viereck_soll_t *v = &s_viereck[0];
        PRUEF(re15_aot_point_in_quad_fun80014368(-25310, -25300, v->xs, v->zs) == 1, "Viereck 1: Mitte muss treffen");
        PRUEF(re15_aot_point_in_quad_fun80014368(-23000, -25300, v->xs, v->zs) == 0, "Viereck 1: ausserhalb darf nicht");
        /* feste Umlaufrichtung: dasselbe Viereck andersherum umlaufen trifft im Original nie */
        int16_t rx[4] = { v->xs[0], v->xs[3], v->xs[2], v->xs[1] }, rz[4] = { v->zs[0], v->zs[3], v->zs[2], v->zs[1] };
        PRUEF(re15_aot_point_in_quad_fun80014368(-25310, -25300, rx, rz) == 0,
              "Viereck 1 umgekehrt: FUN_80014368 prueft feste Umlaufrichtung");
        /* 32-Bit-Produkte (mult/mflo): ein weit entfernter Punkt ueberlaeuft nicht ins Treffen */
        PRUEF(re15_aot_point_in_quad_fun80014368(30000, 30000, v->xs, v->zs) == 0, "Viereck 1: ferner Punkt");
        const viereck_soll_t *w = &s_viereck[1];
        PRUEF(re15_aot_point_in_quad_fun80014368(-20617, -25212, w->xs, w->zs) == 1, "Viereck 2: Mitte muss treffen");
    }

    /* 2. Installation + 3. Durchgang, beide Varianten */
    const uint16_t raeume[2] = { 0x4030, 0x4031 };
    for (int r = 0; r < 2; r++) {
        for (int i = 0; i < 2; i++) {
            if (room_boot(raeume[r]) != 0) return;
            const viereck_soll_t *v = &s_viereck[i];
            const re15_aot_t *a = &g_aot.slots[v->slot];
            const re15_aot_door_params_t *d = &g_aot.door_params[v->slot];
            int pts_ok = 1;
            for (int k = 0; k < 4; k++) if (a->xs[k] != v->xs[k] || a->zs[k] != v->zs[k]) pts_ok = 0;
            unsigned ziel = (((unsigned)d->dest_stage + 1u) << 12) | ((unsigned)d->dest_room << 4)
                          | (raeume[r] & 0xFu);
            unsigned ziel_soll = (((unsigned)v->stage + 1u) << 12) | ((unsigned)v->raum << 4) | (raeume[r] & 0xFu);
            printf("ROOM%04X Slot %d: Typ %d Viereck %d Punkte %s Ziel ROOM%04X Lage (%d,%d,%d) Richtung %d Cut %u\n",
                   raeume[r], v->slot, a->type, a->has_quad, pts_ok ? "gleich" : "ANDERS", ziel,
                   (int)d->spawn_x, (int)d->spawn_y, (int)d->spawn_z, d->spawn_yaw_4096, d->target_cut);
            PRUEF(a->active && a->type == RE15_AOT_TYPE_DOOR, "ROOM%04X Slot %d keine Tuer", raeume[r], v->slot);
            PRUEF(a->has_quad == 1 && pts_ok, "ROOM%04X Slot %d: Viereck-Punkte", raeume[r], v->slot);
            PRUEF(ziel == ziel_soll, "ROOM%04X Slot %d: Ziel ROOM%04X, soll ROOM%04X", raeume[r], v->slot, ziel, ziel_soll);
            PRUEF(d->spawn_x == v->x && d->spawn_y == v->y && d->spawn_z == v->z && d->spawn_yaw_4096 == v->dir
                  && d->target_cut == v->cut, "ROOM%04X Slot %d: Nutzlast (Lage/Richtung/Cut)", raeume[r], v->slot);
            PRUEF(d->band == 0, "ROOM%04X Slot %d: Band %d", raeume[r], v->slot, d->band);

            int gefunden = 0;
            int wechsel = durchgehen(v->slot, &gefunden);
            printf("  Durchgang: Standplatz %s, Raumwechsel %d -> ROOM%04X Lage (%d,%d,%d) Cut %d\n",
                   gefunden ? "gefunden" : "KEINER", wechsel, g_room_change.room_id,
                   (int)g_room_change.x, (int)g_room_change.y, (int)g_room_change.z, g_room_change.target_cut);
            PRUEF(gefunden, "ROOM%04X Slot %d: kein Standplatz vor der Tuer", raeume[r], v->slot);
            PRUEF(wechsel && g_room_change.room_id == ziel_soll && g_room_change.x == v->x
                  && g_room_change.z == v->z && g_room_change.target_cut == v->cut,
                  "ROOM%04X Slot %d: Durchgang fuehrt nicht nach ROOM%04X", raeume[r], v->slot, ziel_soll);
            g_room_change.pending = 0;
        }
    }
}


/* ---------------------------------------------------------------------------------------------
 * Teil "maschine"
 * ------------------------------------------------------------------------------------------ */
#include "gen/r31_tuer_referenz.inc"

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

static uint8_t *datei(const char *pfad, int *n)
{
    size_t z = 0;
    uint8_t *b = slurp(pfad, &z);
    *n = (int)z;
    return b;
}

static void teil_maschine(void)
{
    int n_ref = (int)(sizeof r31_refs / sizeof r31_refs[0]);
    int gut = 0;
    for (int k = 0; k < n_ref; k++) {
        const r31_ref_t *r = &r31_refs[k];
        char pfad[600];
        snprintf(pfad, sizeof pfad, "%s/DOOR/DOOR%02X.DO2", RE15_ASSET_RE2_DIR, r->archiv);
        int n = 0, ton = 0, modell = 0, sektor = 0, groesse = 0;
        uint8_t *d = datei(pfad, &n);
        PRUEF(d != NULL, "%s fehlt", pfad);
        if (!d) continue;
        PRUEF(re15_door_seq_re2_archiv(r->archiv, &ton, &modell, &sektor, &groesse) == 0
              && groesse == n && sektor * 0x800 + modell == n,
              "DOOR%02X: Datei %d B passt nicht zur Tabelle @0x8009a520", r->archiv, n);
        PRUEF(r->verteilt, "DOOR%02X V%d: Skript 0 verteilt die Variante nicht", r->archiv, r->variante);
        re15_door_seq_t s;
        PRUEF(re15_door_seq_start(&s, d + sektor * 0x800, modell, r->variante, 0, r->archiv) == 0,
              "DOOR%02X V%d: Start", r->archiv, r->variante);
        int bild = 0, abw = 0, ton_i = 0;
        while (bild < 4000 && re15_door_seq_bild(&s, 1)) {
            if (bild < r->n_bilder && bild_summe(&s) != r->summen[bild]) {
                if (abw++ < 3) printf("  DOOR%02X V%d Bild %d: Summe weicht ab\n", r->archiv, r->variante, bild);
            }
            for (int t = 0; t < s.n_ton; t++) {
                PRUEF(ton_i < r->n_tone && r->tone[ton_i] == bild, "DOOR%02X V%d: Se_on in Bild %d unerwartet",
                      r->archiv, r->variante, bild);
                ton_i++;
            }
            bild++;
        }
        int ok = abw == 0 && bild == r->n_bilder && ton_i == r->n_tone && s.schliesston == r->schliesston
                 && s.notizen == 0 && s.md1_ok && s.tim_ok;
        printf("DOOR%02X V%d: %3d Bilder (Ref %3d), %d Se_on (Ref %d), Schliesston %d (Ref %d), Notizen 0x%x, "
               "Abweichungen %d -> %s\n", r->archiv, r->variante, bild, r->n_bilder, ton_i, r->n_tone,
               s.schliesston, r->schliesston, s.notizen, abw, ok ? "gleich" : "ANDERS");
        PRUEF(ok, "DOOR%02X V%d: Maschine weicht vom Simulator ab", r->archiv, r->variante);
        gut += ok;
        re15_door_seq_ende(&s);
        free(d);
    }
    printf("Maschine gegen Simulator: %d von %d Archiv-Varianten gleich\n", gut, n_ref);
}

/* ---------------------------------------------------------------------------------------------
 * Teil "zuordnung"
 * ------------------------------------------------------------------------------------------ */
static re15_door_seq_anfrage_t s_gefangen;
static int s_n_gefangen = 0;
static void fang(const re15_door_seq_anfrage_t *a) { s_gefangen = *a; s_n_gefangen++; }

static int16_t le16(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }

/* Schluessel aus den Satzbytes, gerechnet wie op_door_aot_set (scd_vm.c). */
static int satz_schluessel(const uint8_t *pc, int *form, int32_t *x, int32_t *z, int32_t *hw, int32_t *hh,
                           int16_t qx[4], int16_t qz[4], int *band)
{
    if (pc[0] != 0x3B) return -1;
    *band = pc[4];
    if (pc[3] & 0x80) {
        *form = 1;
        for (int k = 0; k < 4; k++) { qx[k] = le16(pc + 6 + 4 * k); qz[k] = le16(pc + 8 + 4 * k); }
        *x = *z = *hw = *hh = 0;
    } else {
        int16_t rx = le16(pc + 6), rz = le16(pc + 8), rw = le16(pc + 10), rd = le16(pc + 12);
        *form = 0;
        *x = (int32_t)rx + (int32_t)rw / 2; *z = (int32_t)rz + (int32_t)rd / 2;
        *hw = (int32_t)(rw < 0 ? -rw : rw) / 2; *hh = (int32_t)(rd < 0 ? -rd : rd) / 2;
        for (int k = 0; k < 4; k++) { qx[k] = 0; qz[k] = 0; }
    }
    return 0;
}

static void teil_zuordnung(void)
{
    /* 1. Tabelle gegen die echten Saetze */
    /* Runde 33: hinter den RE2-Zeilen stehen die Port-Archiv-Zeilen (eigen != 0, probe_r33_tueren) -
     * hier nur die 368 Zeilen der Runde 31. */
    int n_alle = re15_door_seq_zeilen(), n = 0, treffer = 0, seiten = 0, geteilt = 0;
    for (int i = 0; i < n_alle; i++) n += re15_door_seq_zeile(i)->eigen == 0;
    PRUEF(n == 368, "Tabelle hat %d RE2-Zeilen, erwartet 368 (184 Seiten x 2 Raumdateien)", n);
    for (int i = 0; i < n_alle; i++) {
        const re15_tuer_zeile_t *t = re15_door_seq_zeile(i);
        if (t->eigen) continue;
        char rp[600];
        snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(t->raum >> 12), t->raum);
        size_t sz = 0;
        uint8_t *b = slurp(rp, &sz);
        int form = -1, band = -1; int32_t x = 0, z = 0, hw = 0, hh = 0; int16_t qx[4], qz[4];
        int ok = b && t->off + 40 <= sz && satz_schluessel(b + t->off, &form, &x, &z, &hw, &hh, qx, qz, &band) == 0
                 && form == t->form && band == t->band
                 && (form == 1 ? (!memcmp(qx, t->qx, sizeof qx) && !memcmp(qz, t->qz, sizeof qz))
                               : (x == t->x && z == t->z && hw == t->hw && hh == t->hh));
        PRUEF(ok, "Zeile %d S%03u ROOM%04X @0x%X trifft keinen passenden Door_aot_set", i, t->seite, t->raum,
              (unsigned)t->off);
        free(b);
        re15_door_seq_anfrage_t q;
        int r = re15_door_seq_zuordnen_flaeche(t->raum, t->form, t->x, t->z, t->hw, t->hh, t->qx, t->qz, t->band, &q);
        /* Die Wahl muss stimmen; die Seitennummer darf eine andere sein, wo zwei Seiten dieselbe
         * Flaeche mit verschiedenen Zielen teilen (Szenario-Saetze, z. B. S117/S118) - die
         * Tabelle hat dann fuer beide dieselbe Wahl (tuer_zuordnung_gen.py prueft das). */
        PRUEF(r == RE15_DOOR_ARCHIV_RE2 && q.re2_nr == t->re2_nr && q.variante == t->variante
              && q.tuer_nr == t->re2_nr && q.spender == t->spender && q.bit7 == t->bit7 && q.eigen == 0,
              "Zeile %d S%03u: findet ihre Wahl nicht", i, t->seite);
        if (q.seite != t->seite) geteilt++;
        int ar = 0, mo = 0, se = 0, gr = 0;
        PRUEF(re15_door_seq_re2_archiv(t->re2_nr, &ar, &mo, &se, &gr) == 0, "S%03u: DOOR%02X ohne Tabelle",
              t->seite, t->re2_nr);
        if (t->spender != RE15_DOOR_KEIN_SPENDER)
            PRUEF(re15_door_seq_griff_tausch(t->re2_nr, t->spender) != NULL, "S%03u: Griff-Tausch fehlt", t->seite);
        treffer += ok;
        int neu = 1;
        for (int k = 0; k < i; k++)
            if (re15_door_seq_zeile(k)->seite == t->seite && !re15_door_seq_zeile(k)->eigen) { neu = 0; break; }
        seiten += neu;
    }
    printf("Tabelle: %d Zeilen, %d treffen ihren Satz, %d Tuerseiten, %d Zeilen teilen die Flaeche mit einer "
           "anderen Seite (gleiche Wahl)\n", n, treffer, seiten, geteilt);
    PRUEF(seiten == 184, "%d Tuerseiten, erwartet 184", seiten);

    /* 2. Tor unveraendert, Intro-Uebergabe nicht, Viereck S225 ja, S226 nein */
    {
        re15_door_seq_anfrage_t q;
        PRUEF(re15_door_seq_zuordnen_flaeche(0x1170, 0, 2550, 15250, 1050, 850, NULL, NULL, 4, &q) == RE15_DOOR_ARCHIV_TOR1170
              && q.variante == 0, "Tor ROOM1170 Slot 0");
        PRUEF(re15_door_seq_zuordnen_flaeche(0x1171, 0, -11065, -27850, 875, 600, NULL, NULL, 4, &q) == RE15_DOOR_ARCHIV_TOR1170
              && q.variante == 1, "Tor ROOM1171 Slot 5");
        PRUEF(re15_door_seq_zuordnen_flaeche(0x1170, 0, 0, 0, 0, 0, NULL, NULL, 0, &q) == RE15_DOOR_ARCHIV_KEINS,
              "Intro-Uebergabe (Null-Rechteck) darf keine Sequenz bekommen");
        const int16_t qx[4] = { -24190, -25170, -26630, -25250 }, qz[4] = { -25490, -26560, -25220, -23930 };
        PRUEF(re15_door_seq_zuordnen_flaeche(0x4030, 1, 0, 0, 0, 0, qx, qz, 0, &q) == RE15_DOOR_ARCHIV_RE2
              && q.re2_nr == 0x25 && q.variante == 0 && q.seite == 225, "Viereck S225 -> DOOR25 V0");
        const int16_t px[4] = { -20490, -19400, -20740, -21840 }, pz[4] = { -24080, -25070, -26460, -25240 };
        PRUEF(re15_door_seq_zuordnen_flaeche(0x4030, 1, 0, 0, 0, 0, px, pz, 0, &q) == RE15_DOOR_ARCHIV_KEINS,
              "Viereck S226 (keine Tuer, nicht abgedeckt) darf keine Sequenz bekommen");
    }

    /* 3. echter Spielschritt: Kreuz-Raum-Tuer mit Anfrage, nicht abgedeckte ohne */
    re15_door_seq_setze_laeufer(fang);
    {
        /* eine beim Betreten aktive Kreuz-Raum-Tuer MIT Griff-Tausch (T033/T034: DOOR09 <- DOOR04,
         * T027: DOOR13 <- DOOR07). ROOM10D0 S042 ist beim Betreten inert (sce 0, @0x1052). */
        static const uint16_t raeume[] = { 0x1130, 0x1120, 0x1150, 0x10F0, 0x10D0 };
        int erledigt = 0;
        for (size_t r = 0; r < sizeof raeume / sizeof raeume[0] && !erledigt; r++) {
            if (room_boot(raeume[r]) != 0) continue;
            for (int i = 0; i < RE15_AOT_MAX && !erledigt; i++) {
                const re15_aot_t *a = &g_aot.slots[i];
                const re15_aot_door_params_t *d = &g_aot.door_params[i];
                if (!a->active || a->type != RE15_AOT_TYPE_DOOR || !(a->half_w || a->half_h || a->has_quad)) continue;
                re15_door_seq_anfrage_t soll;
                if (re15_door_seq_zuordnen_flaeche(raeume[r], a->has_quad, a->x, a->z, a->half_w, a->half_h,
                                                   a->xs, a->zs, d->band, &soll) != RE15_DOOR_ARCHIV_RE2) continue;
                if (soll.spender == RE15_DOOR_KEIN_SPENDER) continue;
                s_n_gefangen = 0; memset(&s_gefangen, 0, sizeof s_gefangen);
                int gefunden = 0;
                int wechsel = durchgehen(i, &gefunden);
                printf("ROOM%04X Slot %d (S%03u): Standplatz %d, Raumwechsel %d -> ROOM%04X, Laeufer %d x: "
                       "Archiv %d DOOR%02X V%d Tuer %u Spender %02X Seite S%03u\n", raeume[r], i, soll.seite,
                       gefunden, wechsel, g_room_change.room_id, s_n_gefangen, s_gefangen.archiv, s_gefangen.re2_nr,
                       s_gefangen.variante, s_gefangen.tuer_nr, s_gefangen.spender, s_gefangen.seite);
                if (!gefunden) continue;                     /* kein Standplatz: naechste Tuer */
                erledigt = 1;
                PRUEF(wechsel && g_room_change.room_id != raeume[r], "S%03u: kein Kreuz-Raum-Durchgang", soll.seite);
                PRUEF(s_n_gefangen == 1 && s_gefangen.archiv == RE15_DOOR_ARCHIV_RE2 && s_gefangen.re2_nr == soll.re2_nr
                      && s_gefangen.variante == soll.variante && s_gefangen.tuer_nr == soll.re2_nr
                      && s_gefangen.spender == soll.spender,
                      "S%03u: Anfrage DOOR%02X V%d Spender %02X fehlt/falsch", soll.seite, soll.re2_nr, soll.variante,
                      soll.spender);
                /* die Sequenz lief im Spielschritt VOR dem Raumwechsel: der Wechsel steht noch an */
                PRUEF(g_room_change.pending, "S%03u: Raumwechsel muss nach der Sequenz noch anstehen", soll.seite);
                g_room_change.pending = 0;
            }
        }
        PRUEF(erledigt, "keine begehbare Kreuz-Raum-Tuer mit Griff-Tausch gefunden");
    }
    if (room_boot(0x1050) == 0) {
        /* eine Tuer ROOM1050 -> ROOM1090 (T014, Doppeltuer "aehnlich DOOR1B"): nicht abgedeckt.
         * (Bis Runde 32 stand hier ROOM1000 -> ROOM1050; T000..T002 spielen seit Runde 33 das
         * Port-Archiv P07G, gepinnt in probe_r33_tueren "zuordnung".) */
        int slot = -1;
        for (int i = 0; i < RE15_AOT_MAX && slot < 0; i++) {
            const re15_aot_t *a = &g_aot.slots[i];
            const re15_aot_door_params_t *d = &g_aot.door_params[i];
            if (a->active && a->type == RE15_AOT_TYPE_DOOR && d->dest_room == 0x09 && d->dest_stage == 0
                && (a->half_w || a->half_h)) {
                re15_door_seq_anfrage_t q;
                if (re15_door_seq_zuordnen_flaeche(0x1050, a->has_quad, a->x, a->z, a->half_w, a->half_h,
                                                   a->xs, a->zs, d->band, &q) == RE15_DOOR_ARCHIV_KEINS)
                    slot = i;
            }
        }
        PRUEF(slot >= 0, "ROOM1050: keine nicht abgedeckte Tuer nach ROOM1090 gefunden");
        if (slot >= 0) {
            s_n_gefangen = 0;
            int gefunden = 0;
            int wechsel = durchgehen(slot, &gefunden);
            printf("ROOM1050 Slot %d (nicht abgedeckt): Standplatz %d, Raumwechsel %d -> ROOM%04X, Laeufer %d x\n",
                   slot, gefunden, wechsel, g_room_change.room_id, s_n_gefangen);
            PRUEF(gefunden && wechsel && g_room_change.room_id == 0x1090, "ROOM1050: kein Durchgang nach ROOM1090");
            PRUEF(s_n_gefangen == 0, "nicht abgedeckte Tuer darf keine Sequenz anfragen");
            g_room_change.pending = 0;
        }
    }
    re15_door_seq_setze_laeufer(NULL);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "viereck";
    if (!strcmp(teil, "viereck")) teil_viereck();
    else if (!strcmp(teil, "maschine")) teil_maschine();
    else if (!strcmp(teil, "zuordnung")) teil_zuordnung();
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf(g_fehler ? "probe_r31_tueren %s: %d FEHLER\n" : "probe_r31_tueren %s: OK\n", teil, g_fehler);
    return g_fehler ? 1 : 0;
}
