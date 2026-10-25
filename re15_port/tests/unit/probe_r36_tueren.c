/*
 * probe_r36_tueren.c — Runde 36 Spur G "tueren" (analysis/befunde_runde36/G_tueren.md).
 *
 * Teile (je ein ctest, registriert in tests/unit/probes/r36_tueren.cmake):
 *   fahrstuhl     Punkt 1a: die Einstiege in die Fahrstuhlkabine ROOM1080 (S013/S040/S057) spielen die
 *                 RE2-Fahrstuhltuer DOOR27 V0 (RE2-Datei, eigen 0). Mechanik: im echten Spielschritt stellt
 *                 der Durchgang ROOM10C0 -> ROOM1080 genau diese Anfrage; die Maschine schiebt ZWEI Fluegel
 *                 aus der Mitte auseinander (obj 0 / obj 1 laufen in z gegenlaeufig, keine Drehung) - die
 *                 RE1.5-Kabinentuer (ROOM10800) ist zweifluegelig mittig. Gegenprobe: DOOR25 (vorher P25G)
 *                 hebt EIN Blatt (Lage y).
 *                 Runde 37 Spur C (analysis/befunde_runde37/C_fahrstuhl.md): S013/S040/S057 spielen jetzt das Port-
 *                 Archiv P27F (Basis DOOR27, Tonteil + SCD bytegleich, glatte Fluegel) - geprueft wird die Kennung.
 *   schott        Punkt 1b: grosse Tuer ROOM2090 <-> ROOM20A0 (S164/S165) = DOOR26 V0: zwei Fluegel
 *                 (DOOR26 Skript 3 @Datei 0x05914: obj 0 mesh 0 + obj 1 mesh 1); Gegenprobe V3 (vorher) = ein
 *                 Fluegel. Echter Durchgang ROOM2090 -> ROOM20A0 (Band 1) stellt die Anfrage DOOR26 V0.
 *   aufschliessen Punkt 3: Mini-Bank KARTSE.VBS = RE2-Wellen bytegleich (ROOM2110 VAG @0x064B4/0x09874/0x0AA94,
 *                 ROOM20A0 VAG @0x07D8C), RE2-Belegbytes (ROOM2110 sub08 Se_on/Sleep, EXE @0x800515f4/f8),
 *                 Folgen je Art, und der ECHTE Skriptweg: ROOM10D0 Ziffernfeld-Erfolg (Bank-5-Bits 13..16 ->
 *                 sub01 -> sub19 Message_on 5) = Piep, 15 Bilder spaeter Aufschliessen; ROOM3010 Green Keycard
 *                 (Platz 6 -> sub02 Message_on 1) = Karte ein / +15 Piep / +30 Aufschliessen; ROOM1140 Riegel
 *                 (Platz 2 -> sub02 Message_on 0) = Satz AUF_HIER im selben Bild; ohne Ausloeser: kein Ton.
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
#include "re15_aufschliessen_r36.h"
#include "re15_kistenregel_r37.h"
#include "re15_tim.h"
#include <math.h>

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

/* ---------------------------------------------------------------------------------------------
 * Spielschritt-Geruest (wie probe_r33_tueren.c)
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

static int s_flags[8][2], s_n_flags = 0;   /* Story-Flags nach scd_vm_init (spielweite Baenke) */

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
    /* Runde 37 Spur Q: ein Besitz-Bit der Kistenregel zaehlt nur mit dem Gegenstand im INVENTAR (kistenregel_r37.c,
     * RE2 Keep_Item_ck @0x8005850c). Im Original heisst "Bit gesetzt, noch nicht benutzt" = er liegt im Inventar (die
     * Item-Box ist eine Port-Zutat) -> denselben Gegenstand aus der Q-Tabelle dazulegen (ROOM10D0 (9,0x34) Blue Keycard
     * 0x38, ROOM3010 (3,0x3B) Green Keycard 0x36). */
    for (int k = 0; k < s_n_flags; k++)
        for (int j = 0; j < re15_kistenregel_r37_anzahl(); j++) {
            const re15_kr37_fall_t *q = re15_kistenregel_r37_zeile(j);
            if (q->raum == room && q->bank == s_flags[k][0] && q->bit == s_flags[k][1]) { re15_inv_grant(q->item, 1); break; }
        }
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

static re15_door_seq_anfrage_t s_gefangen;
static int s_n_gefangen = 0;
static void fang(const re15_door_seq_anfrage_t *a) { s_gefangen = *a; s_n_gefangen++; }

/* Raum booten, Tuer zum Ziel suchen, per QUADRAT durchgehen: genau eine Anfrage RE2-Datei DOORre2 V var.
 * Runde 37 Spur C: kennung != NULL = Port-Archiv mit dieser Kennung (Basis re2), sonst RE2-Datei (eigen 0). */
static void durchgang_pruefen_k(uint16_t raum, uint16_t ziel, int re2, int var, const char *kennung)
{
    re15_door_seq_setze_laeufer(fang);
    if (room_boot(raum) != 0) return;
    int erledigt = 0;
    for (int i = 0; i < RE15_AOT_MAX && !erledigt; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        const re15_aot_door_params_t *d = &g_aot.door_params[i];
        if (!a->active || a->type != RE15_AOT_TYPE_DOOR || d->dest_room != (ziel & 0xFF) >> 4
            || d->dest_stage != (ziel >> 12) - 1 || !(a->half_w || a->half_h || a->has_quad)) continue;
        s_n_gefangen = 0; memset(&s_gefangen, 0, sizeof s_gefangen);
        int gefunden = 0;
        int wechsel = durchgehen(i, &gefunden);
        printf("ROOM%04X Slot %d: Standplatz %d, Raumwechsel %d -> ROOM%04X, Laeufer %d x: DOOR%02X V%d eigen %d S%03u\n",
               raum, i, gefunden, wechsel, g_room_change.room_id, s_n_gefangen, s_gefangen.re2_nr, s_gefangen.variante,
               s_gefangen.eigen, s_gefangen.seite);
        if (!gefunden) continue;
        erledigt = 1;
        PRUEF(wechsel && g_room_change.room_id == ziel, "ROOM%04X: kein Durchgang nach ROOM%04X", raum, ziel);
        const re15_tuer_eigen_t *ek = s_gefangen.eigen ? re15_door_seq_eigen(s_gefangen.eigen) : NULL;
        PRUEF(s_n_gefangen == 1 && s_gefangen.archiv == RE15_DOOR_ARCHIV_RE2 && s_gefangen.re2_nr == re2
              && s_gefangen.tuer_nr == re2 && s_gefangen.variante == var
              && (kennung ? (ek && !strcmp(ek->kennung, kennung)) : s_gefangen.eigen == 0),
              "ROOM%04X -> ROOM%04X: Anfrage DOOR%02X V%d (%s) fehlt/falsch", raum, ziel, re2, var,
              kennung ? kennung : "RE2-Datei");
        g_room_change.pending = 0;
    }
    PRUEF(erledigt, "ROOM%04X: keine begehbare Tuer nach ROOM%04X", raum, ziel);
    re15_door_seq_setze_laeufer(NULL);
}
static void durchgang_pruefen(uint16_t raum, uint16_t ziel, int re2, int var)
{
    durchgang_pruefen_k(raum, ziel, re2, var, NULL);
}

/* Zeilen einer Seite: alle (xxx0 und xxx1) tragen RE2-Datei DOORre2 Vvar.
 * Runde 37 Spur C: kennung != NULL = Port-Archiv dieser Kennung mit Basis DOORre2 (sonst RE2-Datei, eigen 0). */
static int eigen_ist(int eigen, const char *kennung)
{
    const re15_tuer_eigen_t *e = eigen ? re15_door_seq_eigen(eigen) : NULL;
    return kennung ? (e && !strcmp(e->kennung, kennung)) : eigen == 0;
}
static void zeilen_pruefen_k(int seite, int re2, int var, const char *kennung)
{
    int n = 0;
    for (int i = 0; i < re15_door_seq_zeilen(); i++) {
        const re15_tuer_zeile_t *z = re15_door_seq_zeile(i);
        if ((int)z->seite != seite) continue;
        n++;
        PRUEF(z->re2_nr == re2 && z->variante == var && eigen_ist(z->eigen, kennung) && z->spender == RE15_DOOR_KEIN_SPENDER,
              "S%03d ROOM%04X: DOOR%02X V%d eigen %d statt DOOR%02X V%d %s", seite, z->raum, z->re2_nr,
              z->variante, z->eigen, re2, var, kennung ? kennung : "RE2-Datei");
    }
    PRUEF(n == 2, "S%03d: %d Zeilen statt 2 (xxx0/xxx1)", seite, n);
    re15_door_seq_anfrage_t q;
    unsigned raum = 0;
    PRUEF(re15_door_seq_anfrage_fuer_seite(seite, &q, &raum) && q.re2_nr == re2 && q.variante == var
          && eigen_ist(q.eigen, kennung),
          "S%03d: Pruefhaken RE15_TUER_SEITE faende nicht DOOR%02X V%d", seite, re2, var);
}
static void zeilen_pruefen(int seite, int re2, int var)
{
    zeilen_pruefen_k(seite, re2, var, NULL);
}

/* Maschinenlauf eines RE2-Archivs aus shared_assets/RE2/DOOR: Lage/Drehung/an je Objekt und Bild. */
typedef struct { int bilder; int an[RE15_DOOR_OBJEKTE]; int32_t pos0[RE15_DOOR_OBJEKTE][3], posmin[RE15_DOOR_OBJEKTE][3],
                 posmax[RE15_DOOR_OBJEKTE][3]; int drehung[RE15_DOOR_OBJEKTE]; uint16_t mesh[RE15_DOOR_OBJEKTE]; } spur_t;

static int maschine(int nr, int var, spur_t *sp)
{
    char p[600];
    snprintf(p, sizeof p, "%s/DOOR/DOOR%02X.DO2", RE15_ASSET_RE2_DIR, nr);
    size_t n = 0;
    uint8_t *d = slurp(p, &n);
    int ton = 0, modell = 0, sektor = 0, groesse = 0;
    if (!d || re15_door_seq_re2_archiv(nr, &ton, &modell, &sektor, &groesse) != 0 || groesse != (int)n) {
        printf("FEHLER: %s fehlt/passt nicht\n", p); g_fehler++; free(d); return -1;
    }
    static re15_door_seq_t s;
    memset(sp, 0, sizeof *sp);
    if (re15_door_seq_start(&s, d + sektor * 0x800, modell, var, 0, nr) != 0) { free(d); return -1; }
    int bild = 0;
    uint16_t rot0[RE15_DOOR_OBJEKTE][3];
    while (bild < 4000 && re15_door_seq_bild(&s, 1)) {
        for (int i = 0; i < RE15_DOOR_OBJEKTE; i++) {
            const re15_door_obj_t *o = &s.obj[i];
            if (!o->on) continue;
            if (!sp->an[i]) {
                sp->an[i] = 1; sp->mesh[i] = o->mesh;
                for (int a = 0; a < 3; a++) { sp->pos0[i][a] = sp->posmin[i][a] = sp->posmax[i][a] = o->pos[a]; rot0[i][a] = o->rot[a]; }
            }
            for (int a = 0; a < 3; a++) {
                if (o->pos[a] < sp->posmin[i][a]) sp->posmin[i][a] = o->pos[a];
                if (o->pos[a] > sp->posmax[i][a]) sp->posmax[i][a] = o->pos[a];
                int dr = (int16_t)(uint16_t)(o->rot[a] - rot0[i][a]);
                if (dr < 0) dr = -dr;
                if (dr > sp->drehung[i]) sp->drehung[i] = dr;
            }
        }
        bild++;
    }
    sp->bilder = bild;
    re15_door_seq_ende(&s);
    free(d);
    return 0;
}

/* ---------------------------------------------------------------------------------------------
 * Teil "fahrstuhl"
 * ------------------------------------------------------------------------------------------ */
static void teil_fahrstuhl(void)
{
    /* Runde 37 Spur C (analysis/befunde_runde37/C_fahrstuhl.md): die Seiten spielen jetzt das Port-Archiv P27F =
     * DOOR27 (Tonteil + SCD bytegleich, dieselbe Maschine V0) mit glatten Fluegeln statt der verzahnten Labortuer */
    zeilen_pruefen_k(13, 0x27, 0, "P27F");   /* ROOM1040/1041 -> ROOM1080 */
    zeilen_pruefen_k(40, 0x27, 0, "P27F");   /* ROOM10C0/10C1 -> ROOM1080 */
    zeilen_pruefen_k(57, 0x27, 0, "P27F");   /* ROOM1120/1121 -> ROOM1080 */

    /* Maschine DOOR27 V0: zwei Fluegel schieben aus der Mitte auseinander (z gegenlaeufig), keiner dreht */
    static spur_t sp;
    if (maschine(0x27, 0, &sp) == 0) {
        int f0 = -1, f1 = -1;
        for (int i = 0; i < RE15_DOOR_OBJEKTE; i++) {
            if (!sp.an[i] || sp.mesh[i] > 1) continue;
            if (sp.mesh[i] == 0 && f0 < 0) f0 = i;
            if (sp.mesh[i] == 1 && f1 < 0) f1 = i;
        }
        PRUEF(f0 >= 0 && f1 >= 0, "DOOR27 V0: zwei Fluegel (Mesh 0 und 1) erwartet");
        if (f0 >= 0 && f1 >= 0) {
            int32_t w0 = sp.posmax[f0][2] - sp.posmin[f0][2], w1 = sp.posmax[f1][2] - sp.posmin[f1][2];
            int32_t r0 = (sp.posmax[f0][2] - sp.pos0[f0][2]) - (sp.pos0[f0][2] - sp.posmin[f0][2]);
            int32_t r1 = (sp.posmax[f1][2] - sp.pos0[f1][2]) - (sp.pos0[f1][2] - sp.posmin[f1][2]);
            printf("DOOR27 V0 (%d Bilder): Fluegel obj %d z-Weg %d (Richtung %+d), obj %d z-Weg %d (Richtung %+d), "
                   "Drehung %d/%d, y-Weg %d/%d\n", sp.bilder, f0, w0, r0, f1, w1, r1, sp.drehung[f0], sp.drehung[f1],
                   sp.posmax[f0][1] - sp.posmin[f0][1], sp.posmax[f1][1] - sp.posmin[f1][1]);
            PRUEF(w0 > 1000 && w1 > 1000 && ((r0 > 0) != (r1 > 0)),
                  "DOOR27 V0: Fluegel laufen nicht gegenlaeufig in z (%d/%d, %+d/%+d)", w0, w1, r0, r1);
            PRUEF(sp.drehung[f0] < 64 && sp.drehung[f1] < 64, "DOOR27 V0: Fluegel drehen (Schiebetuer erwartet)");
            PRUEF(sp.posmax[f0][1] - sp.posmin[f0][1] < 500, "DOOR27 V0: Fluegel hebt sich (Hubtuer?)");
        }
    }
    /* Gegenprobe vorher (P25G = DOOR25 V0): EIN Blatt, das nach oben faehrt */
    static spur_t sv;
    if (maschine(0x25, 0, &sv) == 0) {
        int blatt = 0, hoch = 0;
        for (int i = 0; i < RE15_DOOR_OBJEKTE; i++)
            if (sv.an[i]) { blatt++; if (sv.posmax[i][1] - sv.posmin[i][1] > 3000) hoch = 1; }
        printf("DOOR25 V0 (vorher P25G): %d Objekt(e), Hub nach oben %d\n", blatt, hoch);
        PRUEF(blatt == 1 && hoch, "Gegenprobe DOOR25 V0: ein hochfahrendes Blatt erwartet");
    }
    /* echter Durchgang 10C0 -> Kabine 1080: die Kabinentuer ist erst mit Strom eine Tuer (ROOM10C0 main00
     * @0x00E66 Ck(4,0xF3)==0 -> Text "It's an elevator...", sonst @0x00E82 Door_aot_set Platz 1) */
    s_n_flags = 1; s_flags[0][0] = 4; s_flags[0][1] = 0xF3;
    durchgang_pruefen_k(0x10C0, 0x1080, 0x27, 0, "P27F");   /* Runde 37 Spur C: Port-Archiv P27F (Basis DOOR27) */
    s_n_flags = 0;
}

/* ---------------------------------------------------------------------------------------------
 * Teil "schott"
 * ------------------------------------------------------------------------------------------ */
static void teil_schott(void)
{
    zeilen_pruefen(164, 0x26, 0);   /* ROOM2090/2091 -> ROOM20A0 */
    zeilen_pruefen(165, 0x26, 0);   /* ROOM20A0/20A1 -> ROOM2090 */
    static spur_t s0, s3;
    if (maschine(0x26, 0, &s0) == 0 && maschine(0x26, 3, &s3) == 0) {
        int b0 = 0, b3 = 0;
        for (int i = 0; i < RE15_DOOR_OBJEKTE; i++) {
            if (s0.an[i] && s0.mesh[i] <= 2) b0++;
            if (s3.an[i] && s3.mesh[i] <= 2) b3++;
        }
        printf("DOOR26 V0: %d Fluegel-Objekte (Mesh 0..2), V3 (vorher): %d\n", b0, b3);
        PRUEF(b0 == 2, "DOOR26 V0: zwei Fluegel erwartet (Skript 3 @Datei 0x05914 obj 0 + obj 1), gefunden %d", b0);
        PRUEF(b3 == 1, "DOOR26 V3: ein Fluegel erwartet, gefunden %d", b3);
    }
    /* echter Durchgang 2090 -> 20A0 (Band 1, Podest).
     * Querbezug Runde 41 Spur F (Nachtrag 21 "Das Tor soll verschlossen sein, und dann mit den Schluessel den man vom
     * Aligator bekommt geoeffnet werden!"): 2090 Slot 1 @0x009B6 ist bis Bank 9 Bit 113 (Tor aufgeschlossen) ein sce-3-Ereignis
     * (gatorszene_r41.c tor_sperren), Bit 110 (Szene gesehen) haelt die Ankunftsszene fern. Gemessen wird hier weiter die Tuer
     * DOOR26 V0 dieses Durchgangs - also im Zustand NACH dem Aufschliessen. Das Gesperrt-Sein misst unit_r41_gatorszene_tor. */
    s_n_flags = 2; s_flags[0][0] = 9; s_flags[0][1] = 110; s_flags[1][0] = 9; s_flags[1][1] = 113;
    durchgang_pruefen(0x2090, 0x20A0, 0x26, 0);
    s_n_flags = 0;
}

/* ---------------------------------------------------------------------------------------------
 * Teil "aufschliessen"
 * ------------------------------------------------------------------------------------------ */
static int s_spion_n = 0, s_spion_satz[32], s_spion_bild[32], s_bild = 0;
static void spion(int satz) { if (s_spion_n < 32) { s_spion_satz[s_spion_n] = satz; s_spion_bild[s_spion_n] = s_bild; } s_spion_n++; }

static int re2_bytes(const char *rel, long off, const char *hex)
{
    char p[600];
    snprintf(p, sizeof p, "%s/%s", RE15_RE2_INFO_DIR, rel);
    size_t n = 0;
    uint8_t *d = slurp(p, &n);
    int ok = d != NULL;
    for (int i = 0; ok && hex[2 * i]; i++) {
        unsigned b = 0;
        sscanf(hex + 2 * i, "%2x", &b);
        ok = (size_t)(off + i) < n && d[off + i] == b;
    }
    free(d);
    return ok;
}

/* n Bilder spielen; liefert, in welchem Bild (relativ) welcher Satz kam */
static void spiel(int bilder, uint16_t druecke_alle)
{
    for (int f = 0; f < bilder; f++, s_bild++) {
        uint16_t e = (druecke_alle && f && (f % druecke_alle) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
    }
}

static void folge_pruefen(const char *wo, const int *satz, const int *bild, int n)
{
    PRUEF(s_spion_n == n, "%s: %d Toene statt %d", wo, s_spion_n, n);
    for (int k = 0; k < n && k < s_spion_n; k++) {
        int db = s_spion_bild[k] - s_spion_bild[0];
        printf("  %s: Ton %d = Satz %d bei Bild %+d\n", wo, k, s_spion_satz[k], db);
        PRUEF(s_spion_satz[k] == satz[k] && db == bild[k], "%s: Ton %d Satz %d Bild %+d statt Satz %d Bild %+d",
              wo, k, s_spion_satz[k], db, satz[k], bild[k]);
    }
}

static void teil_aufschliessen(void)
{
    /* 1. RE2-Belegbytes (Datei-Offsets, info/re2leon) */
    PRUEF(re2_bytes("PL0/RDT/ROOM2110.RDT", 0x01B4C, "36020c00") && re2_bytes("PL0/RDT/ROOM2110.RDT", 0x01B58, "090a0f00")
          && re2_bytes("PL0/RDT/ROOM2110.RDT", 0x01B5C, "36020d00") && re2_bytes("PL0/RDT/ROOM2110.RDT", 0x01B8C, "090a0f00")
          && re2_bytes("PL0/RDT/ROOM2110.RDT", 0x01B90, "36020b00"),
          "RE2 ROOM2110 sub08: Se_on 0x0C / Sleep 15 / Se_on 0x0D / Sleep 15 / Se_on 0x0B nicht an den Offsets");
    /* RE2 PSX.EXE (t_addr 0x80010000, Kopf 0x800): @0x800515f4 lui a0,0x226 = 3c040226, @0x800515f8 jal 0x8005ba28 */
    PRUEF(re2_bytes("PSX.EXE", 0x800 + 0x800515f4 - 0x80010000, "2602043c") &&
          re2_bytes("PSX.EXE", 0x800 + 0x800515f8 - 0x80010000, "8a6e010c"),
          "RE2 PSX.EXE @0x800515f4/f8: lui a0,0x226 / jal 0x8005ba28 nicht gefunden");

    /* 2. Mini-Bank: VBD = die vier RE2-Wellen bytegleich hintereinander */
    {
        char p[600];
        snprintf(p, sizeof p, "%s/KARTSE.VBS", RE15_ASSET_RE2_DIR);
        size_t n = 0; uint8_t *k = slurp(p, &n);
        re15_karte_bank_rec_t rec; re15_karte_bank_rec(&rec);
        PRUEF(k && n == rec.vbd_off + rec.vbd_size && rec.se_count == 4, "KARTSE.VBS fehlt/Groesse %u statt %u",
              (unsigned)n, rec.vbd_off + rec.vbd_size);
        static const struct { const char *raum; long off, len; } w[4] = {
            { "2110", 0x064B4, 2976 }, { "2110", 0x09874, 4640 }, { "2110", 0x0AA94, 11264 }, { "20A0", 0x07D8C, 7568 } };
        long pos = rec.vbd_off;
        for (int i = 0; k && i < 4; i++) {
            char q[600]; snprintf(q, sizeof q, "%s/PL0/RDT/ROOM%s.RDT", RE15_RE2_INFO_DIR, w[i].raum);
            size_t m = 0; uint8_t *r = slurp(q, &m);
            PRUEF(r && (size_t)(w[i].off + w[i].len) <= m && (size_t)(pos + w[i].len) <= n
                  && !memcmp(k + pos, r + w[i].off, (size_t)w[i].len),
                  "KARTSE.VBS Welle %d != RE2 ROOM%s VAG @0x%05lX", i, w[i].raum, w[i].off);
            pos += w[i].len;
            free(r);
        }
        free(k);
    }

    /* 3. Tabelle */
    int je[4] = { 0, 0, 0, 0 };
    for (int i = 0; i < re15_aufschliessen_r36_stellen(); i++) {
        int art = -1; re15_aufschliessen_r36_stelle(i, NULL, NULL, &art, NULL);
        if (art >= 0 && art < 4) je[art]++;
    }
    printf("Stellen: %d (KARTE_EIN %d, KARTE %d, KARTE_AUF %d, RIEGEL %d)\n", re15_aufschliessen_r36_stellen(),
           je[0], je[1], je[2], je[3]);
    PRUEF(je[0] == 6 && je[1] == 3 && je[2] == 8 && je[3] == 12, "Arten-Zensus weicht ab (6/3/8/12 erwartet)");

    /* 4. Folgen (RE2 ROOM2110 sub08: 0 / Sleep 15 / Sleep 15) */
    {
        int sa[3], bi[3];
        PRUEF(re15_aufschliessen_r36_folge(1, sa, bi, 3) == 3 && sa[0] == 0 && sa[1] == 1 && sa[2] == 2
              && bi[0] == 0 && bi[1] == 15 && bi[2] == 30, "Folge KARTE != Klack 0 / Piep 15 / Aufschliessen 30");
        PRUEF(re15_aufschliessen_r36_folge(2, sa, bi, 3) == 2 && sa[0] == 1 && sa[1] == 2 && bi[0] == 0 && bi[1] == 15,
              "Folge KARTE_AUF != Piep 0 / Aufschliessen 15");
        PRUEF(re15_aufschliessen_r36_folge(3, sa, bi, 3) == 1 && sa[0] == 3 && bi[0] == 0, "Folge RIEGEL != Satz 3 bei 0");
        PRUEF(re15_aufschliessen_r36_folge(0, sa, bi, 3) == 1 && sa[0] == 0 && bi[0] == 0, "Folge KARTE_EIN != Satz 0 bei 0");
    }

    re15_aufschliessen_r36_setze_spieler(spion);
    /* 5a. ROOM10D0: ohne Ausloeser 300 Bilder still */
    s_n_flags = 0;
    if (room_boot(0x10D0) == 0) {
        s_spion_n = 0; s_bild = 0;
        spiel(300, 0);
        PRUEF(s_spion_n == 0, "ROOM10D0 ohne Code: %d Toene", s_spion_n);
        /* 5b. Ziffernfeld-Erfolg: die vier Ziffern-Bits (5,13..16) -> sub01 @0x01526 Evt_exec sub19 -> Message_on 5 */
        for (int b = 13; b <= 16; b++) re15_game_flag_set(5, (uint8_t)b, 1);
        s_spion_n = 0; s_bild = 0;
        spiel(120, 0);
        static const int sa[2] = { 1, 2 }, bi[2] = { 0, 15 };
        folge_pruefen("ROOM10D0 Code richtig (KARTE_AUF)", sa, bi, 2);
        PRUEF(re15_game_flag_get(3, 0x32), "ROOM10D0: sub01 hat (3,0x32) nicht gesetzt - Skriptweg nicht gelaufen");
    }
    /* 5b'. ROOM10D0 Kartenleser: Blue Keycard (9,0x34) da, Platz 1 sce 3 -> sub20 @0x019AE Message_on 7 (Ja/Nein),
     * Ja (Ck(12,0x1F)==0) -> @0x019C4 Message_on 9 "You've used the Blue Keycard." = KARTE_EIN (nur Karte ein;
     * Piep + Aufschliessen erst nach dem Code, 5b). Bestaetigt wird mit QUADRAT (Ja ist vorgewaehlt), Text mehrseitig -> alle 8 Bilder druecken. */
    s_n_flags = 1; s_flags[0][0] = 9; s_flags[0][1] = 0x34;
    if (room_boot(0x10D0) == 0) {
        s_spion_n = 0; s_bild = 0;
        re15_aot_fire_slot(1);
        for (int f = 0; f < 900 && s_spion_n == 0; f++, s_bild++) {
            uint16_t e = (f >= 8 && (f % 8) == 0) ? RE15_PAD_BIT_SQUARE : 0;
            frame(e, e);
        }
        static const int sa[1] = { 0 }, bi[1] = { 0 };
        folge_pruefen("ROOM10D0 Kartenleser Ja (KARTE_EIN)", sa, bi, 1);
    }
    /* 5c. ROOM3010 Green Keycard: (3,0x3B) = Karte da -> Platz 6 sce 3 -> sub02 Message_on 1 */
    s_n_flags = 1; s_flags[0][0] = 3; s_flags[0][1] = 0x3B;
    if (room_boot(0x3010) == 0) {
        s_spion_n = 0; s_bild = 0;
        re15_aot_fire_slot(6);
        spiel(90, 0);
        static const int sa[3] = { 0, 1, 2 }, bi[3] = { 0, 15, 30 };
        folge_pruefen("ROOM3010 Green Keycard (KARTE)", sa, bi, 3);
    }
    /* 5d. ROOM1140 Riegel von dieser Seite: Platz 2 sce 3 -> sub02 Message_on 0 */
    s_n_flags = 0;
    if (room_boot(0x1140) == 0) {
        s_spion_n = 0; s_bild = 0;
        re15_aot_fire_slot(2);
        spiel(60, 0);
        static const int sa[1] = { 3 }, bi[1] = { 0 };
        folge_pruefen("ROOM1140 Riegel (RIEGEL)", sa, bi, 1);
    }
    re15_aufschliessen_r36_setze_spieler(NULL);
}


/* ---------------------------------------------------------------------------------------------
 * Teil "textur" (Punkt 2): P0CD (T025 ROOM10C0 <-> ROOM10D0) nicht mehr "herausstechend billig"
 * Masse wie tools/tueren/r36_textur_mass.py (Blatt v 0..217, Helligkeit L Rec.601):
 *   fein = std(L - G1 L)/mean L, fleck = std(G1 L - G4 L)/mean L, form = std(G4 L)/mean L.
 * Vorher (Runde-33-P0CD, git 78a4a748): fein 0.0818 (RE2-Minimum 0.0788), form 0.1817 (RE2-Median 0.3955),
 * hell 114.5 (hellste Holztuer; RE2-Holz 25..70). Faser vorher senkrechte Flecken, gemalt (ROOM10C04) waagerecht.
 * ------------------------------------------------------------------------------------------ */
static void gauss_zeilen(const double *in, double *out, int w, int h, double sg)
{
    int r = (int)(3 * sg + 0.5);
    double k[64]; double su = 0;
    for (int i = -r; i <= r; i++) { k[i + r] = exp(-(double)(i * i) / (2 * sg * sg)); su += k[i + r]; }
    for (int i = 0; i <= 2 * r; i++) k[i] /= su;
    static double tmp[256 * 128];
    for (int y = 0; y < h; y++) for (int x = 0; x < w; x++) {
        double a = 0;
        for (int i = -r; i <= r; i++) { int xx = x + i; if (xx < 0) xx = -xx - 1; if (xx >= w) xx = 2 * w - xx - 1; a += k[i + r] * in[y * w + xx]; }
        tmp[y * w + x] = a;
    }
    for (int y = 0; y < h; y++) for (int x = 0; x < w; x++) {
        double a = 0;
        for (int i = -r; i <= r; i++) { int yy = y + i; if (yy < 0) yy = -yy - 1; if (yy >= h) yy = 2 * h - yy - 1; a += k[i + r] * tmp[yy * w + x]; }
        out[y * w + x] = a;
    }
}

static double sd(const double *a, int n) { double m = 0, q = 0; for (int i = 0; i < n; i++) m += a[i]; m /= n;
    for (int i = 0; i < n; i++) q += (a[i] - m) * (a[i] - m); return sqrt(q / n); }

static const char *s_textur_pfad = NULL;   /* Werkzeug: anderes Archiv messen (probe_r36_tueren textur <DO2>) */

static void teil_textur(void)
{
    const re15_tuer_eigen_t *e = NULL;
    for (int i = 1; i <= re15_door_seq_eigen_anzahl(); i++)
        if (re15_door_seq_eigen(i) && !strcmp(re15_door_seq_eigen(i)->kennung, "P0CD")) e = re15_door_seq_eigen(i);
    PRUEF(e != NULL, "Port-Archiv P0CD fehlt in gen/re15_tuer_eigen.inc");
    if (!e) return;
    char p[600];
    if (s_textur_pfad) snprintf(p, sizeof p, "%s", s_textur_pfad);
    else snprintf(p, sizeof p, "%s/RE15DOOR/P0CD.DO2", RE15_ASSET_SHARED_DIR);
    size_t n = 0; uint8_t *d = slurp(p, &n);
    PRUEF(d && (s_textur_pfad || n == e->datei), "P0CD.DO2 fehlt/Groesse");
    if (!d) return;
    const uint8_t *m = d + e->sektor * 0x800;
    uint32_t tim_rel = (uint32_t)m[4] | ((uint32_t)m[5] << 8) | ((uint32_t)m[6] << 16) | ((uint32_t)m[7] << 24);
    re15_tim_t t;
    PRUEF(re15_tim_parse(m + tim_rel, (int)(n - (size_t)(m - d) - tim_rel), &t) == 0 && t.bpp == 8 && t.width == 128,
          "P0CD: TIM nicht lesbar");
    static double L[218 * 128], g1[218 * 128], g4[218 * 128], fe[218 * 128], fl[218 * 128];
    const uint8_t *px = (const uint8_t *)t.pixels;
    double summe = 0;
    for (int v = 0; v < 218; v++) for (int u = 0; u < 128; u++) {
        uint16_t c = t.clut[px[v * 128 + u]];
        double r = ((c & 31) << 3) | ((c & 31) >> 2), g = (((c >> 5) & 31) << 3) | (((c >> 5) & 31) >> 2),
               b = (((c >> 10) & 31) << 3) | (((c >> 10) & 31) >> 2);
        L[v * 128 + u] = 0.299 * r + 0.587 * g + 0.114 * b; summe += L[v * 128 + u];
    }
    double mittel = summe / (218 * 128);
    gauss_zeilen(L, g1, 128, 218, 1.0);
    gauss_zeilen(L, g4, 128, 218, 4.0);
    for (int i = 0; i < 218 * 128; i++) { fe[i] = L[i] - g1[i]; fl[i] = g1[i] - g4[i]; }
    double fein = sd(fe, 218 * 128) / mittel, fleck = sd(fl, 218 * 128) / mittel, form = sd(g4, 218 * 128) / mittel;
    /* Faserrichtung im Holz rechts vom Fenster (u 62..119, v 20..139): Zeilenwechsel (gy) gegen Spaltenwechsel (gx) */
    double gx = 0, gy = 0;
    for (int v = 20; v < 140; v++) for (int u = 62; u < 120; u++) {
        gx += fabs(L[v * 128 + u + 1] - L[v * 128 + u]);
        gy += fabs(L[(v + 1) * 128 + u] - L[v * 128 + u]);
    }
    /* Fenster dunkler als das Holz daneben (gemalt: Glas 34/36/32 gegen Holz 90/69/44 in ROOM10C04) */
    double glas = 0, holz = 0;
    for (int v = 40; v < 120; v++) { glas += L[v * 128 + 40]; holz += L[v * 128 + 90]; }
    printf("P0CD: fein %.4f fleck %.4f form %.4f scharf %.3f hell %.1f | Faser gy/gx %.2f | Glas/Holz %.2f\n",
           fein, fleck, form, fein / fleck, mittel, gy / gx, glas / holz);
    /* Schranken gegen den Runde-33-Stand (dieselbe Rechnung, `probe_r36_tueren textur <alt.DO2>`):
     * vorher fein 0.0814 fleck 0.0915 form 0.1824 scharf 0.890 hell 114.5 Faser gy/gx 0.44 Glas/Holz 0.48 */
    PRUEF(fein >= 0.095, "P0CD fein %.4f < 0.095 (vorher 0.0814)", fein);
    PRUEF(form >= 0.30, "P0CD form %.4f < 0.30 (vorher 0.1824, RE2-Median 0.3955)", form);
    PRUEF(mittel <= 100.0, "P0CD hell %.1f > 100 (vorher 114.5)", mittel);
    PRUEF(gy > 2.0 * gx, "P0CD Faser nicht waagerecht wie ROOM10C04 (gy/gx %.2f, vorher 0.44)", gy / gx);
    PRUEF(glas < 0.8 * holz, "P0CD Fenster nicht dunkler als das Holz (%.2f)", glas / holz);
    /* gezeigt = gemalt: Texel * 73/128 (Blattflaeche c = 73) gegen das ROOM10C04-Zeilenprofil des linken Fluegels
     * (tools/tueren/r36_rezept_p0cd.py PROFIL_*), Messspalten u 85..103, Median je 5 Zeilen, +-10 je Kanal */
    static const int py[5] = { 70, 86, 102, 118, 138 };
    static const double prgb[5][3] = { {99, 84, 68}, {89, 67, 43}, {78, 56, 29}, {69, 49, 24}, {45, 31, 11} };
    for (int k = 0; k < 5 && !s_textur_pfad; k++) {
        int v = (int)((py[k] + 1.5 - 69) * 218.0 / 95.0 + 0.5);
        double ka[3];
        for (int c = 0; c < 3; c++) {
            double w[5 * 19]; int nw = 0;
            for (int vv = v - 2; vv <= v + 2; vv++) for (int u = 85; u < 104; u++) {
                uint16_t col = t.clut[px[vv * 128 + u]];
                int x5 = (col >> (5 * c)) & 31;
                w[nw++] = (double)((x5 << 3) | (x5 >> 2));
            }
            for (int i = 1; i < nw; i++) for (int j = i; j > 0 && w[j] < w[j - 1]; j--) { double h = w[j]; w[j] = w[j - 1]; w[j - 1] = h; }
            ka[c] = w[nw / 2] * 73.0 / 128.0;
        }
        printf("  y%d v%3d gezeigt (%.0f %.0f %.0f) gemalt (%.0f %.0f %.0f)\n", py[k], v, ka[0], ka[1], ka[2],
               prgb[k][0], prgb[k][1], prgb[k][2]);
        PRUEF(fabs(ka[0] - prgb[k][0]) <= 10 && fabs(ka[1] - prgb[k][1]) <= 10 && fabs(ka[2] - prgb[k][2]) <= 10,
              "P0CD v%d: gezeigt weicht vom gemalten ROOM10C04-Profil ab", v);
    }
    free(d);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "fahrstuhl";
    if (!strcmp(teil, "fahrstuhl")) teil_fahrstuhl();
    else if (!strcmp(teil, "schott")) teil_schott();
    else if (!strcmp(teil, "aufschliessen")) teil_aufschliessen();
    else if (!strcmp(teil, "textur")) { if (argc > 2) s_textur_pfad = argv[2]; teil_textur(); }
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf("%s: %s (%d Fehler)\n", teil, g_fehler ? "ROT" : "GRUEN", g_fehler);
    return g_fehler ? 1 : 0;
}
