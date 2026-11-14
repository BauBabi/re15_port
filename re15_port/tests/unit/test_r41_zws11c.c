/*
 * test_r41_zws11c.c — Runde 41, Zwischensequenz Raum 11C (ROOM11C0, Leon und Ada): Riegel fuer den neuen Ablauf
 *   ... msg 3 "You didn't run into him?" -> Gorilla-Schrei (ROOM11C0 snd1 Satz RE15_ZWS_SCHREI_SATZ, Anker vor Sleep 20
 *   @0x01920) -> Tonende + Sleep 20 -> msg 4 "Leon: What!?" @0x01928 -> Umdrehen zum Anhaenger + Sleep 10 @0x0197E -> Film
 *   R11CGORI.STR (Anker vor Sce_bgm_control @0x01982) -> Musikstart @0x01982, Kamera 12 @0x01988 ... msg 5 "Leon: Ada, you
 *   hide inside that patrol car." @0x01A5E.
 * Herleitung: include/re15_zws_r41.h. Harness (echte Engine-Wege: scd_room_reenter mit allen Installern, SCD-VM mit
 * Dispatcher-Anker, Walker, Nachrichten-FSM, re15_game_step, Baenke 0x27/0x42/PL00) wie tests/unit/test_r39_parkplatz11c0.c.
 *
 * Teile (Aufruf: test_r41_zws11c <teil> [Ordner]):
 *   datei     MOVIE/R11CGORI.STR: Sektorform wie MOVIE/CAPCOM.STR, 362 Bilder 320x240 mit den Port-Dekodern, alle
 *             dekodierbar, Abblende am Ende; CD-XA 37800 Hz Stereo, Laenge = Filmlaenge (+ < 1 Sektor); Herkunft .TXT.
 *             Mit [Ordner]: jedes 30. Bild als RGB24 (b_NNN.rgb, 320x240) fuer den Bildvergleich gegen die mp4.
 *   anker     Tabelle: 114 = Port-Zeilen 61/60, 11C = RDT-Anker @0x01920 `09 0a 14 00` / @0x01982 `54 00 01 00 00 00`
 *             (Bytes = ausgelieferte ROOM11C0); armiert nur in ROOM11C0 (nicht 11C1/1141/11B0, nicht bei geaendertem
 *             Byte); Port-Zeilen 60/61 in 11C0 und fremde Zeiger mit gleichen Bytes vom Haken unberuehrt
 *   ablauf    ROOM11C0 am VM (Tuer-Spawn, Programm SZENE = Gosub sub02): msg 3 -> Schrei -> msg 4 genau Tonlaenge + 20 Takte
 *             danach; Film erst nach msg 4 + Drehen; waehrend des Films keine Musik, keine Kamera 12, keine msg 5; nach dem
 *             Film Musikstart MAIN Play und Kamera 12 (Original-Auftritt) - alle Bilder bis vor Cut_chg 5 @0x01A5C unsichtbar
 *             vorgespult, ohne Blendkanal; das erste gezeigte Bild ist Kamera 5 mit msg 5, ab dort Einblenden 240 ... 16, 0
 *   elza      ROOM11C1 (Elzas Szenario): kein Nachrichtenblock, keine Anker, 600 Bilder ohne Schrei/Film
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
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_tim.h"
#include "re15_parkplatz_r39.h"
#include "re15_zws_r41.h"
#include "re15_str.h"
#include "re15_xa.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

static uint8_t *asset(const char *rel, size_t *n)
{
    char p[600];
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    return slurp(p, n);
}

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }

/* ---- Teil: datei --------------------------------------------------------------------------------------------- */
static double helligkeit(const uint32_t *rgba, int n)
{
    double s = 0;
    for (int i = 0; i < n; i++) s += ((rgba[i] >> 24) & 0xFF) + ((rgba[i] >> 16) & 0xFF) + ((rgba[i] >> 8) & 0xFF);
    return s / (3.0 * n);
}

static void teil_datei(const char *ordner)
{
    printf("== datei ==\n");
    size_t n = 0, nc = 0;
    uint8_t *d = asset(RE15_ZWS11C_FILM, &n);
    uint8_t *c = asset("MOVIE/CAPCOM.STR", &nc);
    PRUEF(d && n > 0 && n % 2336 == 0, "%s vorhanden, %zu B = %zu Sektoren zu 2336 B", RE15_ZWS11C_FILM, n, n / 2336);
    if (!d || !c) { free(d); free(c); return; }
    const uint8_t *dv = NULL, *da = NULL, *cv = NULL, *ca = NULL;
    for (size_t o = 0; o + 2336 <= n && (!dv || !da); o += 2336) {
        if (d[o + 2] == 0x48 && !dv) dv = d + o;
        if (d[o + 2] == 0x64 && !da) da = d + o;
    }
    for (size_t o = 0; o + 2336 <= nc && (!cv || !ca); o += 2336) {
        if (c[o + 2] == 0x48 && !cv) cv = c + o;
        if (c[o + 2] == 0x64 && !ca) ca = c + o;
    }
    PRUEF(dv && da && cv && ca && !memcmp(dv, cv, 8) && !memcmp(da, ca, 8),
          "Subheader wie CAPCOM.STR: Video %02x %02x %02x %02x, Ton %02x %02x %02x %02x", dv ? dv[0] : 0, dv ? dv[1] : 0,
          dv ? dv[2] : 0, dv ? dv[3] : 0, da ? da[0] : 0, da ? da[1] : 0, da ? da[2] : 0, da ? da[3] : 0);
    PRUEF(dv && rd16(dv + 8) == 0x0160 && rd16(dv + 10) == 0x8001 && rd16(dv + 8 + 0x10) == 320 && rd16(dv + 8 + 0x12) == 240 &&
          rd16(dv + 8 + 0x16) == 0x3800 && rd16(dv + 8 + 0x1A) == 2 && cv && rd16(cv + 8 + 0x1A) == 2,
          "STR-Kopf 0x0160/0x8001, 320x240, BS-Kopf-Kopie id 0x3800 Version 2 (wie CAPCOM.STR)");
    re15_str_t s;
    PRUEF(re15_str_open(&s, d, n) == 0 && s.num_frames == RE15_ZWS11C_FILM_BILDER && s.width == 320 && s.height == 240,
          "re15_str_open: %d Bilder %dx%d (Quelle %d Bilder bei 30/s)", s.num_frames, s.width, s.height, RE15_ZWS11C_FILM_BILDER);
    uint32_t *rgba = (uint32_t *)malloc(320 * 240 * 4);
    uint8_t *rgb = (uint8_t *)malloc(320 * 240 * 3);
    int fehler = 0, geschrieben = 0;
    double h_mitte = -1, h_letzt = -1, h_erst = -1;
    for (int f = 0; f < s.num_frames && rgba && rgb; f++) {
        if (re15_str_decode(&s, f, rgba) != 0) { fehler++; continue; }
        if (f == 0) h_erst = helligkeit(rgba, 320 * 240);
        if (f == s.num_frames / 2) h_mitte = helligkeit(rgba, 320 * 240);
        if (f == s.num_frames - 1) h_letzt = helligkeit(rgba, 320 * 240);
        if (ordner && (f % 30) == 0) {
            for (int i = 0; i < 320 * 240; i++) {
                rgb[i * 3 + 0] = (uint8_t)(rgba[i] >> 24); rgb[i * 3 + 1] = (uint8_t)(rgba[i] >> 16);
                rgb[i * 3 + 2] = (uint8_t)(rgba[i] >> 8);
            }
            char p[700];
            snprintf(p, sizeof p, "%s/b_%03d.rgb", ordner, f);
            FILE *o = fopen(p, "wb");
            if (o) { fwrite(rgb, 1, 320 * 240 * 3, o); fclose(o); geschrieben++; }
        }
    }
    PRUEF(fehler == 0, "alle %d Bilder mit den Port-Dekodern dekodiert (%d Fehler)", s.num_frames, fehler);
    if (ordner) printf("    %d Bilder (jedes 30.) nach %s geschrieben\n", geschrieben, ordner);
    printf("    Helligkeit Bild 1 %.1f, Bild %d %.1f, Bild %d %.1f\n", h_erst, s.num_frames / 2 + 1, h_mitte, s.num_frames, h_letzt);
    PRUEF(h_mitte > 10.0 && h_letzt < 4.0, "Bildinhalt: Mitte nicht schwarz, letztes Bild abgeblendet");
    int16_t *pcm = NULL;
    int fr = 0, rate = 0, ch = 0;
    const int rc = re15_xa_decode_movie(d, n, &pcm, &fr, &rate, &ch);
    const double dauer = rate ? (double)fr / rate : 0.0, film = RE15_ZWS11C_FILM_BILDER / 30.0;
    PRUEF(rc == 0 && rate == 37800 && ch == 2, "CD-XA-Ton: 37800 Hz Stereo 4 Bit (re15_xa_decode_movie rc %d)", rc);
    PRUEF(dauer >= film && dauer < film + 2016.0 / 37800.0, "Tonlaenge %.4f s = Filmlaenge %.4f s + < 1 Sektor (2016 Proben)",
          dauer, film);
    long laut = 0;
    for (int i = 0; i < fr * 2; i++) if (abs(pcm[i]) > 1000) laut++;
    PRUEF(laut > 1000, "Ton nicht stumm (%ld Proben ueber 1000)", laut);
    if (ordner && pcm) {   /* Ton fuer den Vergleich gegen die mp4 (s16le Stereo 37800 Hz) */
        char p[700];
        snprintf(p, sizeof p, "%s/ton.raw", ordner);
        FILE *o = fopen(p, "wb");
        if (o) { fwrite(pcm, sizeof(int16_t), (size_t)fr * 2, o); fclose(o); }
    }
    size_t nt = 0;
    uint8_t *t = asset("MOVIE/R11CGORI.TXT", &nt);
    PRUEF(t && nt > 0 && strstr((const char *)t, "film_bauen.py --film 11C") && strstr((const char *)t, "Raum11C_Gorillas_v9_Ton.mp4"),
          "Herkunft MOVIE/R11CGORI.TXT (Quelle + Werkzeug)");
    free(t); free(pcm); free(rgb); free(rgba); free(d); free(c);
}

/* ---- Teil: anker --------------------------------------------------------------------------------------------- */
static void teil_anker(void)
{
    printf("== anker ==\n");
    const re15_zws_film_t *f114 = re15_zws_r41_film_fuer_raum(RE15_ZWS114_RAUM), *f11c = re15_zws_r41_film_fuer_raum(0x11C0);
    PRUEF(re15_zws_r41_anzahl() == 2 && f114 && f11c && f114 != f11c, "Tabelle: zwei Filme (114, 11C)");
    if (!f114 || !f11c) return;
    PRUEF(f114->schrei.msg == RE15_ZWS114_MSG_SCHREI && f114->film.msg == RE15_ZWS114_MSG_FILM &&
          !strcmp(f114->film_datei, RE15_ZWS114_FILM) && f114->film_bilder == RE15_ZWS114_FILM_BILDER &&
          !strcmp(f114->kennung, "zws114"), "114: Port-Zeilen %d/%d, %s, %d Bilder", f114->schrei.msg, f114->film.msg,
          f114->film_datei, f114->film_bilder);
    PRUEF(f11c->schrei.msg < 0 && f11c->schrei.off == 0x01920u && f11c->film.msg < 0 && f11c->film.off == 0x01982u &&
          !strcmp(f11c->film_datei, "MOVIE/R11CGORI.STR") && f11c->film_bilder == 362 && !strcmp(f11c->kennung, "zws11c"),
          "11C: Anker @0x%05X / @0x%05X, %s, %d Bilder", (unsigned)f11c->schrei.off, (unsigned)f11c->film.off,
          f11c->film_datei, f11c->film_bilder);
    PRUEF(f114->schrei_satz == RE15_ZWS_SCHREI_SATZ && f11c->schrei_satz == RE15_ZWS_SCHREI_SATZ,
          "beide Filme: derselbe Schrei ROOM11C0 snd1 Satz %d (eine Stelle RE15_ZWS_SCHREI_SATZ)", RE15_ZWS_SCHREI_SATZ);
    size_t n = 0;
    uint8_t *r = asset("STAGE1/ROOM11C0.RDT", &n);
    PRUEF(r && n > 0x01A62, "ROOM11C0.RDT geladen (%zu B)", n);
    if (!r) return;
    /* Bytes am Ort im ausgelieferten RDT (Kopf re15_zws_r41.h): Sleep 20 @0x01920 vor Work_set Leon @0x01924 + msg 4 @0x01928;
     * Sce_bgm_control @0x01982 nach Sleep 10 @0x0197E, vor Cut_chg 12 @0x01988; msg 5 @0x01A5E nach Cut_chg 5 @0x01A5C. */
    static const uint8_t vor4[12] = { 0x09, 0x0A, 0x14, 0x00, 0x2E, 0x01, 0x00, 0x00, 0x2B, 0x04, 0x00, 0x00 };
    static const uint8_t film[12] = { 0x09, 0x0A, 0x0A, 0x00, 0x54, 0x00, 0x01, 0x00, 0x00, 0x00, 0x29, 0x0C };
    static const uint8_t vor5[6]  = { 0x29, 0x05, 0x2B, 0x05, 0x00, 0x00 };
    PRUEF(!memcmp(r + 0x01920, vor4, 12), "@0x01920 `09 0a 14 00` Sleep 20, @0x01924 `2e 01 00 00` Work_set Leon, "
                                          "@0x01928 `2b 04 00 00` msg 4 \"What!?\"");
    PRUEF(!memcmp(r + 0x0197E, film, 12), "@0x0197E `09 0a 0a 00` Sleep 10, @0x01982 `54 00 01 00 00 00` Sce_bgm_control "
                                          "MAIN Play, @0x01988 `29 0c` Cut_chg 12");
    PRUEF(!memcmp(r + 0x01A5C, vor5, 6), "@0x01A5C `29 05` Cut_chg 5, @0x01A5E `2b 05 00 00` msg 5 \"Ada, you hide ...\"");
    PRUEF(!memcmp(r + f11c->schrei.off, f11c->schrei.bytes, f11c->schrei.n) &&
          !memcmp(r + f11c->film.off, f11c->film.bytes, f11c->film.n) && f11c->sichtbar.off == 0x01A5Cu &&
          !memcmp(r + f11c->sichtbar.off, f11c->sichtbar.bytes, f11c->sichtbar.n) && f114->sichtbar.n == 0,
          "Tabellen-Bytes = RDT-Bytes an allen drei Ankern (Cut_chg 5 @0x01A5C `29 05` = Ende des Vorspulens; 114 ohne)");

    const unsigned alt = g_current_room_id;
    re15_zws_r41_raum_scan(r, (long)n, 0x11C0);
    PRUEF(g_re15_zws_anker_n == 3, "ROOM11C0: 3 Anker armiert");
    re15_zws_r41_raum_scan(r, (long)n, 0x11C1);
    PRUEF(g_re15_zws_anker_n == 0, "dieselben Bytes als ROOM11C1 (Elza): kein Anker");
    {
        size_t n2 = 0; uint8_t *r2 = asset("STAGE1/ROOM1141.RDT", &n2);
        re15_zws_r41_raum_scan(r2, (long)n2, RE15_ZWS114_RAUM);
        PRUEF(g_re15_zws_anker_n == 0, "ROOM1141: kein Anker (114 = Port-Zeilen in ENTRIEGELN39)");
        free(r2);
        r2 = asset("STAGE1/ROOM11B0.RDT", &n2);
        re15_zws_r41_raum_scan(r2, (long)n2, 0x11B0);
        PRUEF(g_re15_zws_anker_n == 0, "ROOM11B0: kein Anker");
        free(r2);
    }
    uint8_t *k = (uint8_t *)malloc(n);
    memcpy(k, r, n); k[0x01922] ^= 0x01;   /* Sleep-Laenge geaendert */
    re15_zws_r41_raum_scan(k, (long)n, 0x11C0);
    PRUEF(g_re15_zws_anker_n == 0, "geaendertes Byte @0x01922: kein Anker (Original-Ablauf)");
    memcpy(k, r, n);
    re15_zws_r41_raum_scan(r, (long)n, 0x11C0);
    g_current_room_id = 0x11C0;
    re15_zws_r41_install(0x11C0);
    re15_zws_r41_schrei_hook = NULL; re15_zws_r41_film_spieler = 0;
    PRUEF(re15_zws_r41_pc(k + 0x01920) == 0 && re15_zws_r41_pc(k + 0x01982) == 0 && g_re15_zws_schreie == 0 &&
          g_re15_zws_filme == 0, "fremder Puffer mit denselben Bytes: Haken greift nicht (nur der armierte Zeiger)");
    static const uint8_t z61[4] = { 0x2B, RE15_ZWS114_MSG_SCHREI, 0, 0 }, z60[4] = { 0x2B, RE15_ZWS114_MSG_FILM, 0, 0 },
                         z4[4] = { 0x2B, 0x04, 0, 0 }, z5[4] = { 0x2B, 0x05, 0, 0 };
    PRUEF(re15_zws_r41_message_on(z61) == 0 && re15_zws_r41_message_on(z60) == 0 && re15_zws_r41_message_on(z4) == 0 &&
          re15_zws_r41_message_on(z5) == 0, "ROOM11C0: Message_on 60/61/4/5 nicht vom Zeilen-Haken (11C hat nur Anker)");
    PRUEF(re15_zws_r41_pc(r + 0x01928) == 0 && re15_zws_r41_pc(r + 0x01A5E) == 0, "msg 4 / msg 5 selbst ohne Haken");
    PRUEF(re15_zws_r41_pc(r + 0x01920) == 0 && g_re15_zws_schreie == 1, "Schrei ohne Plattform: Anker laeuft sofort durch");
    PRUEF(re15_zws_r41_pc(r + 0x01982) == 0 && g_re15_zws_filme == 1, "Film ohne Plattform: Anker laeuft sofort durch");
    g_re15_zws_schreie = 0; g_re15_zws_filme = 0;
    free(k); free(r);
    re15_zws_r41_raum_scan(NULL, 0, 0);
    g_current_room_id = alt;
}

/* ---- Harness (Muster test_r39_parkplatz11c0.c) ---------------------------------------------------------------- */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static long               s_bild = 0;
static long               s_bild_musik = -1;    /* erstes SEQ_CTL MAIN Play (Sce_bgm_control 00 01) */
static int                s_n_musik = 0;

static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    return 0;
}

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e))
        if (e.kind == SCD_AUDIO_SEQ_CTL && e.bank == 0 && e.volume == 1) {
            s_n_musik++;
            if (s_bild_musik < 0) s_bild_musik = s_bild + 1;
        }
    s_bild++;
}

static uint8_t s_blob[2][0x80000];
static int bank_laden(uint8_t typ, int platz)
{
    re15_enemy_bank_t *eb = re15_enemy_find(typ);
    if (eb && eb->ok) return 1;
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(typ);
    size_t off = 0, len = 0;
    int ok = (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob[0]);
    if (ok) { memcpy(s_blob[platz & 1], ems + off, len); if (!eb) eb = re15_enemy_alloc(typ); ok = (eb != NULL); }
    if (ok) {
        re15_tim_t tim = (re15_tim_t){0};
        ok = (re15_emd_parse_container(s_blob[platz & 1], len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0);
        if (ok) { eb->ok = 1; eb->buf = NULL;
                  eb->loco_ok   = (re15_emd_parse_loco_bank(s_blob[platz & 1], len, &eb->skel_loco, &eb->anim_loco) == 0);
                  eb->victim_ok = (re15_emd_parse_victim_bank(s_blob[platz & 1], len, &eb->skel_victim, &eb->anim_victim) == 0);
                  eb->own_ok    = (re15_emd_parse_own_bank(s_blob[platz & 1], len, &eb->skel_own, &eb->anim_own) == 0); }
        else eb->type = 0;
    }
    free(ems);
    return ok;
}

static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;
static int pl00_laden(void)
{
    static int ok = -1;
    if (ok >= 0) return ok;
    char p[600]; size_t esz = 0, rsz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", RE15_ASSET_PSX_DIR); uint8_t *edd = slurp(p, &esz);
    snprintf(p, sizeof p, "%s/PLD/PL00.EMR", RE15_ASSET_PSX_DIR); uint8_t *emr = slurp(p, &rsz);
    ok = (edd && emr && re15_emd_parse_animation(edd, esz, &s_pl00_anim) == 0 &&
          re15_emd_parse_skeleton(emr, rsz, &s_pl00_skel) == 0) ? 1 : 0;
    return ok;
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0; s_bild = 0; s_bild_musik = -1; s_n_musik = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    if (bank_laden(0x27, 0) && bank_laden(0x42, 1) && pl00_laden()) {
        s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    } else { printf("  FEHLER: Baenke 0x27/0x42/PL00 nicht ladbar\n"); g_fail++; }
    return 0;
}

/* ---- Teil: ablauf -------------------------------------------------------------------------------------------- */
static int        s_echt_takte = -1;
static long       s_bild_schrei = -1;
static int        s_n_schrei = 0, s_satz_schrei = -1;
static uint8_t   *s_11c0 = NULL;
static re15_rdt_t s_rdt11c0;
static int schrei_takte(int satz)
{
    if (!s_11c0) {
        size_t n = 0;
        s_11c0 = asset(RE15_ZWS_SCHREI_RDT, &n);
        if (!s_11c0 || re15_rdt_parse(s_11c0, n, &s_rdt11c0) < 0) return -1;
    }
    const int b = RE15_ZWS_SCHREI_BANK;
    return re15_zws_r41_satz_takte(s_rdt11c0.snd_edt[b], s_rdt11c0.snd_vh[b], s_rdt11c0.snd_vh_size[b], s_rdt11c0.snd_vb[b],
                                   s_rdt11c0.snd_vb_size[b], satz);
}
static int echter_schrei(int satz)   /* Plattform-Ersatz: Laenge aus der echten Bank, Spielbild merken */
{
    s_bild_schrei = s_bild + 1;      /* Zaehlweise wie die Messungen nach frame(): s_bild nach diesem Takt */
    s_n_schrei++; s_satz_schrei = satz;
    return schrei_takte(satz);
}

#define FILM_HALTEN 60   /* so viele Spielbilder "laeuft" der Film im Riegel - das Skript muss so lange parken */

static void teil_ablauf(void)
{
    printf("== ablauf ==\n");
    s_echt_takte = schrei_takte(RE15_ZWS_SCHREI_SATZ);
    re15_game_state_init();
    re15_fade_init();
    /* Tuer-Spawn aus ROOM11B0 main00 @0x00FAA: (-25279,0,17268) Blick 0x11FB, Cut 0 (wie test_r39_parkplatz11c0 kampf) */
    if (room_boot(0x11C0, -25279, 17268, 0x11FB, 0) != 0) return;
    PRUEF(g_re15_zws_anker_n == 3, "Raumladen ROOM11C0: 3 Anker armiert (scd_register_current_rdt)");
    re15_zws_r41_schrei_hook = echter_schrei;
    re15_zws_r41_film_spieler = 1;
    const unsigned s0 = g_re15_r39o_szene_zaehler;
    long b_msg[64]; int msgs[64], n_msg = 0, last = -1;
    long b_msg3 = -1, b_msg4 = -1, b_msg5 = -1, b_schrei_ende = -1, b_film_an = -1, b_film_aus = -1, b_cam12 = -1;
    int msg5_im_film = 0, cam12_im_film = 0, musik_im_film = 0, halte = 0;
    int blende[32], n_blende = 0, vorher_blende = 0;
    long b_sichtbar = -1;                     /* erstes Bild nach dem Film, das wieder gezeigt wird (Vorspulen aus) */
    int unsichtbar = 0, blende_unsichtbar = 0, cam_sichtbar = -1;
    re15_fade_kill(RE15_ZWS_BLENDE_KANAL);
    for (int f = 0; f < 6000 && (b_msg5 < 0 || s_bild < b_msg5 + 20); f++) {
        frame(0, 0);
        /* Plattform-Ersatz Vorspulen (zws_r41_pc.c re15_zws_r41_pc_vorspul_bild am Bildende): Bild nicht zeigen, zaehlen */
        if (b_film_aus >= 0 && b_sichtbar < 0) {
            if (re15_zws_r41_vorspulen()) { unsichtbar++; re15_zws_r41_vorspul_bild(); }
            else { b_sichtbar = s_bild; cam_sichtbar = (int)g_scd.cam_id; }
        }
        if (s_bild_schrei >= 0 && b_schrei_ende < 0 && re15_zws_r41_schrei_rest() == 0) b_schrei_ende = s_bild;
        if (g_scd.message_active && (int)g_scd.message_id != last) {
            last = (int)g_scd.message_id;
            if (n_msg < 64) { msgs[n_msg] = last; b_msg[n_msg] = s_bild; n_msg++; }
            if (last == 3 && b_msg3 < 0) b_msg3 = s_bild;
            if (last == 4 && b_msg4 < 0) b_msg4 = s_bild;
            if (last == 5 && b_msg5 < 0) b_msg5 = s_bild;
        }
        if (!g_scd.message_active) last = -1;
        if (b_cam12 < 0 && g_scd.cam_id == 12) b_cam12 = s_bild;
        re15_fade_tick();   /* wie re15_render_end_frame (render_pc.c) je gezeichnetem Bild */
        if (b_film_an < 0 && g_fade_ch[RE15_ZWS_BLENDE_KANAL].drawn) vorher_blende++;
        if (b_film_aus >= 0 && b_sichtbar < 0 && g_fade_ch[RE15_ZWS_BLENDE_KANAL].drawn) blende_unsichtbar++;
        if (b_sichtbar >= 0 && n_blende < 32)   /* ab dem ersten wieder gezeigten Bild */
            blende[n_blende++] = g_fade_ch[RE15_ZWS_BLENDE_KANAL].drawn ? g_fade_ch[RE15_ZWS_BLENDE_KANAL].out_r : -1;
        /* Plattform-Ersatz Film: angefordert -> FILM_HALTEN Bilder "laufen lassen" (die echte Plattform spielt ihn
         * zwischen zwei Spielbildern; hier tickt das Spiel weiter, damit das Parken sichtbar wird), dann gespielt. */
        if (re15_zws_r41_film_offen()) {
            if (b_film_an < 0) b_film_an = s_bild;
            if (g_scd.message_active && g_scd.message_id == 5) msg5_im_film = 1;
            if (g_scd.cam_id == 12) cam12_im_film = 1;
            if (s_n_musik > 0) musik_im_film = 1;
            if (++halte >= FILM_HALTEN) {
                PRUEF(re15_zws_r41_aktiv() && !strcmp(re15_zws_r41_aktiv()->film_datei, RE15_ZWS11C_FILM),
                      "Plattform spielt %s", re15_zws_r41_aktiv() ? re15_zws_r41_aktiv()->film_datei : "-");
                re15_zws_r41_film_gespielt(); b_film_aus = s_bild;
                PRUEF(re15_zws_r41_vorspulen() && re15_fade_done(RE15_ZWS_BLENDE_KANAL),
                      "nach dem Film: Vorspulen an, noch kein Einblenden (erst am Anker @0x01A5C)");
            }
        }
    }
    printf("    Nachrichten:"); for (int i = 0; i < n_msg; i++) printf(" %d@%ld", msgs[i], b_msg[i]); printf("\n");
    printf("    msg 3 Bild %ld, Schrei Bild %ld (Satz %d, %d Takte laut Bank), Tonende Bild %ld, msg 4 Bild %ld, Film an %ld "
           "aus %ld, Musik MAIN Play Bild %ld, Kamera 12 Bild %ld, msg 5 Bild %ld\n", b_msg3, s_bild_schrei, s_satz_schrei,
           s_echt_takte, b_schrei_ende, b_msg4, b_film_an, b_film_aus, s_bild_musik, b_cam12, b_msg5);
    PRUEF(g_re15_r39o_szene_zaehler == s0 + 1, "Szene = Programm SZENE (Gosub sub02 aus dem RDT, parkplatz_r39.c)");
    int i3 = -1, i4 = -1, i5 = -1;
    for (int i = 0; i < n_msg; i++) {
        if (msgs[i] == 3 && i3 < 0) i3 = i;
        if (msgs[i] == 4 && i4 < 0) i4 = i;
        if (msgs[i] == 5 && i5 < 0) i5 = i;
    }
    PRUEF(i3 >= 0 && i4 == i3 + 1 && i5 == i4 + 1, "Zeilenfolge msg 3, 4, 5 ohne Zwischenzeile");
    PRUEF(s_n_schrei == 1 && s_satz_schrei == RE15_ZWS_SCHREI_SATZ && s_echt_takte == 37,
          "Schrei genau einmal, ROOM11C0 snd1 Satz %d, %d Takte", s_satz_schrei, s_echt_takte);
    PRUEF(b_msg3 >= 0 && s_bild_schrei > b_msg3 && b_msg4 > s_bild_schrei,
          "Reihenfolge: msg 3 (Bild %ld) -> Schrei (Bild %ld) -> msg 4 \"What!?\" (Bild %ld)", b_msg3, s_bild_schrei, b_msg4);
    PRUEF(b_schrei_ende - s_bild_schrei == s_echt_takte,
          "Skript parkt genau die Tonlaenge: %ld Takte (Soll %d)", b_schrei_ende - s_bild_schrei, s_echt_takte);
    PRUEF(b_msg4 - s_bild_schrei == s_echt_takte + 20,
          "Pause: msg 4 %ld Takte nach dem Schrei = Tonlaenge %d + Sleep 20 @0x01920 (%.2f s)", b_msg4 - s_bild_schrei,
          s_echt_takte, (b_msg4 - s_bild_schrei) / 30.0);
    PRUEF(b_film_an > b_msg4 && b_film_an - b_msg4 >= 40 + 50 + 10 + 10 + 10 + 10,
          "Film erst nach msg 4 + Leon/Ada drehen sich zum Anhaenger (Sleep 40/50 @0x01930/@0x0193C, Sleep 10 x3, Gosub sub06, "
          "Sleep 10 @0x0197E): %ld Takte (%.2f s)", b_film_an - b_msg4, (b_film_an - b_msg4) / 30.0);
    PRUEF(!musik_im_film && !cam12_im_film && !msg5_im_film,
          "waehrend der Film laeuft (%d Bilder): keine Musik, keine Kamera 12, keine msg 5", FILM_HALTEN);
    PRUEF(s_bild_musik > b_film_aus && s_bild_musik - b_film_aus <= 2 && s_n_musik == 1,
          "Musik MAIN Play (Sce_bgm_control @0x01982) direkt nach dem Film (%ld Takte), einmal", s_bild_musik - b_film_aus);
    PRUEF(b_cam12 >= s_bild_musik && b_cam12 - b_film_aus <= 2, "Kamera 12 (@0x01988) direkt nach dem Film (Bild %ld)", b_cam12);
    PRUEF(b_msg5 > b_cam12 && b_msg5 - b_cam12 >= 36 + 80,
          "msg 5 \"Ada, you hide inside that patrol car.\" nach dem Original-Auftritt (36 Takte Schwingen + Sleep 80 @0x01A50): "
          "%ld Takte nach Kamera 12", b_msg5 - b_cam12);
    /* Orchestrator-Entscheid 2026-11-14: der Auftritt @0x01988..@0x01A5B laeuft mit allen Takten, aber unsichtbar */
    printf("    Vorspulen: Film aus Bild %ld, %d Bilder unsichtbar (Zaehler %u), erstes gezeigtes Bild %ld (Kamera %d), "
           "msg 5 Bild %ld\n", b_film_aus, unsichtbar, g_re15_zws_vorspul_bilder, b_sichtbar, cam_sichtbar, b_msg5);
    PRUEF(b_sichtbar == b_msg5 && cam_sichtbar == 5 && unsichtbar == (int)(b_sichtbar - b_film_aus - 1) &&
          g_re15_zws_vorspul_bilder == (unsigned)unsichtbar && unsichtbar >= 36 + 80,
          "unsichtbar vorgespult: alle %d Bilder von Kamera 12 bis vor Cut_chg 5 @0x01A5C; erstes gezeigtes Bild = Kamera 5 "
          "mit msg 5 (Bild %ld)", unsichtbar, b_sichtbar);
    PRUEF(blende_unsichtbar == 0, "waehrend des Vorspulens kein Blendkanal aktiv (%d Bilder)", blende_unsichtbar);
    {
        int ok = n_blende >= 17 && vorher_blende == 0;
        for (int i = 0; ok && i < 16; i++) if (blende[i] != 240 - 16 * i) ok = 0;
        if (ok && blende[16] != -1) ok = 0;
        printf("    Blende ab Kamera 5:"); for (int i = 0; i < n_blende && i < 18; i++) printf(" %d", blende[i]); printf("\n");
        PRUEF(ok, "Ueberblendung wie RE2: vor dem Film kein Abblenden (%d Bilder), ab dem ersten gezeigten Bild (Kamera 5, "
                  "msg 5) %d, %d ... %d, %d, Halt (15 Bilder aus Schwarz, ROOM1080 sub14 @0x0252A)", vorher_blende,
              n_blende > 0 ? blende[0] : -9, n_blende > 1 ? blende[1] : -9, n_blende > 14 ? blende[14] : -9,
              n_blende > 15 ? blende[15] : -9);
    }
    PRUEF(!re15_zws_r41_vorspulen(), "Vorspulen danach aus");
    re15_zws_r41_schrei_hook = NULL;
    re15_zws_r41_film_spieler = 0;
}

/* ---- Teil: elza ---------------------------------------------------------------------------------------------- */
static void teil_elza(void)
{
    printf("== elza ==\n");
    re15_game_state_init();
    if (room_boot(0x11C1, -25279, 17268, 0x11FB, 0) != 0) return;
    PRUEF(s_rdt.messages == NULL || s_rdt.messages_size == 0, "ROOM11C1 hat keinen Nachrichtenblock (RDT-Kopf +0x3C = 0): "
                                                              "der Leon/Ada-Dialog gibt es nur in ROOM11C0 (Leons Szenario)");
    PRUEF(g_re15_zws_anker_n == 0, "ROOM11C1: keine Anker");
    re15_zws_r41_schrei_hook = echter_schrei;
    re15_zws_r41_film_spieler = 1;
    const unsigned sch0 = g_re15_zws_schreie, fi0 = g_re15_zws_filme;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) g_actors[s].grid_id |= 0x20;   /* Hunde ruhig */
    for (int f = 0; f < 600; f++) frame(0, 0);
    PRUEF(g_re15_zws_schreie == sch0 && g_re15_zws_filme == fi0 && !re15_zws_r41_film_offen(),
          "ROOM11C1: 600 Bilder ohne Schrei/Film");
    re15_zws_r41_schrei_hook = NULL;
    re15_zws_r41_film_spieler = 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) { printf("Aufruf: test_r41_zws11c <datei [Ordner]|anker|ablauf|elza>\n"); return 2; }
    if (!strcmp(argv[1], "datei")) teil_datei(argc > 2 ? argv[2] : NULL);
    else if (!strcmp(argv[1], "anker")) teil_anker();
    else if (!strcmp(argv[1], "ablauf")) teil_ablauf();
    else if (!strcmp(argv[1], "elza")) teil_elza();
    else { printf("unbekannter Teil %s\n", argv[1]); return 2; }
    printf("%s: %d Fehler\n", argv[1], g_fail);
    return g_fail ? 1 : 0;
}
