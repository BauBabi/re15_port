/*
 * test_r41_zws114.c — Runde 41, Zwischensequenz Raum 114 (ROOM1141, John und Elza): Riegel fuer den neuen Ablauf
 *   ... msg 5 -> Gorilla-Schrei (ROOM11C0 snd1 Satz RE15_ZWS114_SCHREI_SATZ) -> Tonende + Sleep 20 -> msg 6 "What was that?!"
 *   -> Drehen + Sleep 86 -> Film R114GORI.STR -> msg 7 "Shit! that is what I was talking about!".
 * Herleitung: include/re15_zws114_r41.h. Harness (echte Engine-Wege: scd_room_reenter mit allen Installern, SCD-VM,
 * Walker, Nachrichten-FSM, re15_game_step) wie tests/unit/test_r39_dining1141.c.
 *
 * Teile (Aufruf: test_r41_zws114 <teil>):
 *   datei     MOVIE/R114GORI.STR: Sektorform wie MOVIE/CAPCOM.STR, 367 Bilder 320x240 mit den Port-Dekodern (re15_str.c /
 *             bss_vlc.c / bss_mdec.c), alle dekodierbar, Abblende am Ende; CD-XA 37800 Hz Stereo, Laenge = Filmlaenge
 *   schrei    ROOM11C0 snd1: Satz 4 `00 00 59 15` = 37 Takte, Satz 2 `00 00 32 14` = 16 Takte (re15_zws114_r41_satz_takte)
 *   programm  ENTRIEGELN39: msg 58, msg 61, Sleep 20, msg 6 und Sleep 86, msg 60, msg 7 hintereinander, kein Glasbruch;
 *             Haken: Schrei parkt n Takte, Film parkt bis "gespielt", andere Raeume/Nachrichten unberuehrt
 *   ablauf    ROOM1141 Phase H am VM: Gemaelde, Riegel -> Szene; Schrei-Takt, msg 6 genau Tonlaenge + 20 Takte danach,
 *             Film erst nach Drehen + Sleep 86, waehrend des Films keine msg 7, msg 7 im Takt nach dem Film, dann msg 8;
 *             vor dem Film kein Abblenden (harter Schnitt), danach Einblenden 240, 224 ... 16, 0 (RE2 ROOM1080 sub14)
 *   blende    RE2-Einblenden nach dem Film: Kanal 2, ABR 2, RGB 7, Schritt -0x800, Pegel 0x7800 -> 15 Bilder bis 16,
 *             dann 0 und Halt; ohne gezeigten Film (re15_zws114_r41_film_fehlt) kein Einblenden
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
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_item_discard.h"
#include "re15_gameflow.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_dining_r38.h"
#include "re15_dining_r39.h"
#include "re15_zws114_r41.h"
#include "re15_str.h"
#include "re15_xa.h"
#include "re15_fade.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);

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

static void teil_datei(void)
{
    printf("== datei ==\n");
    size_t n = 0, nc = 0;
    uint8_t *d = asset(RE15_ZWS114_FILM, &n);
    uint8_t *c = asset("MOVIE/CAPCOM.STR", &nc);
    PRUEF(d && n > 0 && n % 2336 == 0, "%s vorhanden, %zu B = %zu Sektoren zu 2336 B", RE15_ZWS114_FILM, n, n / 2336);
    if (!d || !c) { free(d); free(c); return; }
    /* Form von CAPCOM.STR: erster Video-Sektor (Sektor 0) Subheader 00 01 48 00 (x2), STR-Kopf 0x0160/0x8001, 320x240,
     * BS-Kopf-Kopie (0x3800, Version 2); erster Ton-Sektor (Sektor 7) Subheader 01 01 64 01 (x2). Die Reihenfolge im Strom
     * ist frei (re15_str.c ueberspringt Ton, re15_xa.c Video); hier steht der Ton knapp vor seinem Bild. */
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
    PRUEF(re15_str_open(&s, d, n) == 0 && s.num_frames == RE15_ZWS114_FILM_BILDER && s.width == 320 && s.height == 240,
          "re15_str_open: %d Bilder %dx%d (Quelle %d Bilder bei 30/s)", s.num_frames, s.width, s.height, RE15_ZWS114_FILM_BILDER);
    uint32_t *rgba = (uint32_t *)malloc(320 * 240 * 4);
    int fehler = 0;
    double h_mitte = -1, h_letzt = -1, h_erst = -1;
    for (int f = 0; f < s.num_frames && rgba; f++) {
        if (re15_str_decode(&s, f, rgba) != 0) { fehler++; continue; }
        if (f == 0) h_erst = helligkeit(rgba, 320 * 240);
        if (f == s.num_frames / 2) h_mitte = helligkeit(rgba, 320 * 240);
        if (f == s.num_frames - 1) h_letzt = helligkeit(rgba, 320 * 240);
    }
    PRUEF(fehler == 0, "alle %d Bilder mit den Port-Dekodern dekodiert (%d Fehler)", s.num_frames, fehler);
    printf("    Helligkeit Bild 1 %.1f, Bild %d %.1f, Bild %d %.1f\n", h_erst, s.num_frames / 2 + 1, h_mitte, s.num_frames, h_letzt);
    PRUEF(h_mitte > 10.0 && h_letzt < 4.0, "Bildinhalt: Mitte nicht schwarz, letztes Bild abgeblendet (Abblende Bild 357-367)");
    int16_t *pcm = NULL;
    int fr = 0, rate = 0, ch = 0;
    const int rc = re15_xa_decode_movie(d, n, &pcm, &fr, &rate, &ch);
    const double dauer = rate ? (double)fr / rate : 0.0, film = RE15_ZWS114_FILM_BILDER / 30.0;
    PRUEF(rc == 0 && rate == 37800 && ch == 2, "CD-XA-Ton: 37800 Hz Stereo 4 Bit (re15_xa_decode_movie rc %d)", rc);
    PRUEF(dauer >= film && dauer < film + 2016.0 / 37800.0, "Tonlaenge %.4f s = Filmlaenge %.4f s + < 1 Sektor (2016 Proben)",
          dauer, film);
    long laut = 0;
    for (int i = 0; i < fr * 2; i++) if (abs(pcm[i]) > 1000) laut++;
    PRUEF(laut > 1000, "Ton nicht stumm (%ld Proben ueber 1000)", laut);
    size_t nt = 0;
    uint8_t *t = asset("MOVIE/R114GORI.TXT", &nt);
    PRUEF(t && nt > 0 && strstr((const char *)t, "film_bauen.py") && strstr((const char *)t, "Raum114_Gorillas_v3_Ton.mp4"),
          "Herkunft MOVIE/R114GORI.TXT (Quelle + Werkzeug)");
    free(t); free(pcm); free(rgba); free(d); free(c);
}

/* ---- Teil: schrei -------------------------------------------------------------------------------------------- */
static int schrei_takte(int satz, re15_rdt_t *r)
{
    const int b = RE15_ZWS114_SCHREI_BANK;
    return re15_zws114_r41_satz_takte(r->snd_edt[b], r->snd_vh[b], r->snd_vh_size[b], r->snd_vb[b], r->snd_vb_size[b], satz);
}

static uint8_t   *s_11c0 = NULL;
static re15_rdt_t s_rdt11c0;
static int bank_11c0(void)
{
    if (s_11c0) return 0;
    size_t n = 0;
    s_11c0 = asset(RE15_ZWS114_SCHREI_RDT, &n);
    if (!s_11c0 || re15_rdt_parse(s_11c0, n, &s_rdt11c0) < 0) return -1;
    return 0;
}

static void teil_schrei(void)
{
    printf("== schrei ==\n");
    PRUEF(bank_11c0() == 0 && s_rdt11c0.snd_edt[1] && s_rdt11c0.snd_vh[1] && s_rdt11c0.snd_vb[1],
          "%s geladen, snd1 vorhanden (RDT-Kopf +0x14/+0x18/+0x1C)", RE15_ZWS114_SCHREI_RDT);
    if (!s_11c0) return;
    const uint8_t *e = s_rdt11c0.snd_edt[1];
    PRUEF(e[16] == 0x00 && e[17] == 0x00 && e[18] == 0x59 && e[19] == 0x15,
          "EDT Satz 4 = 00 00 59 15 (Prog 0, Ton 5, Stimme 5) - Gorilla Se(4) @0x8011795c/@0x80117960");
    PRUEF(e[8] == 0x00 && e[9] == 0x00 && e[10] == 0x32 && e[11] == 0x14,
          "EDT Satz 2 = 00 00 32 14 (Prog 0, Ton 3, Stimme 4) - Gorilla Se(2) @0x80118c80/@0x80118c60");
    /* VAG 5: 13580 Proben bei Pitch 0x400 (11025 Hz) = 1,2317 s = 36,95 Takte -> 37; VAG 3: 5572 Proben = 0,5054 s -> 16 */
    const int t4 = schrei_takte(4, &s_rdt11c0), t2 = schrei_takte(2, &s_rdt11c0);
    PRUEF(t4 == 37, "Satz 4 (Ruf Clip 3) = %d Takte (13580 Proben / 11025 Hz = 1,2317 s, 30 Takte/s, aufgerundet)", t4);
    PRUEF(t2 == 16, "Satz 2 (Landung) = %d Takte (5572 Proben = 0,5054 s)", t2);
    PRUEF(schrei_takte(12, &s_rdt11c0) < 0, "Satz 12 leer (00 00 00 00) -> keine Laenge");
    PRUEF(RE15_ZWS114_SCHREI_SATZ == 4, "Schrei-Satz = %d (vorlaeufig Satz 4, Koordinator 2026-11-13)", RE15_ZWS114_SCHREI_SATZ);
}

/* ---- Teil: programm ------------------------------------------------------------------------------------------ */
static int finde(const uint8_t *p, int n, const uint8_t *m, int mn, int ab)
{
    for (int i = ab; i + mn <= n; i++) if (!memcmp(p + i, m, (size_t)mn)) return i;
    return -1;
}

static int ist_zeilenanfang(const uint8_t *p, int n, int pos)
{
    for (int pc = 0; pc < n; ) {
        if (pc == pos) return 1;
        const int sz = scd_opcode_size_at(p + pc);
        if (sz <= 0) return 0;
        pc += sz;
    }
    return 0;
}

static int s_fake_takte = 3, s_fake_satz = -1, s_fake_n = 0;
static int fake_schrei(int satz) { s_fake_satz = satz; s_fake_n++; return s_fake_takte; }

static void teil_programm(void)
{
    printf("== programm ==\n");
    int nb = 0;
    const uint8_t *b = re15_dining_r39_programm(1, &nb);
    const uint8_t vor6[16] = { 0x2B, RE15_R39J_MSG_STIMME_WARTEN, 0, 0, 0x2B, RE15_ZWS114_MSG_SCHREI, 0, 0,
                               0x09, 0x0A, 0x14, 0x00, 0x2B, 0x06, 0x00, 0x00 };
    const uint8_t vor7[12] = { 0x09, 0x0A, 0x56, 0x00, 0x2B, RE15_ZWS114_MSG_FILM, 0, 0, 0x2B, 0x07, 0x00, 0x00 };
    const uint8_t glas[3] = { 0x36, RE15_R39J_SE_BANK, RE15_R39J_SATZ_GLAS };
    const int i6 = b ? finde(b, nb, vor6, 16, 0) : -1, i7 = b ? finde(b, nb, vor7, 12, 0) : -1;
    PRUEF(i6 > 0 && ist_zeilenanfang(b, nb, i6) && finde(b, nb, vor6, 16, i6 + 1) < 0,
          "ENTRIEGELN39 @+%04X: Message_on 58, Message_on 61 (Schrei), Sleep 20 (@0x00DC6), Message_on 6 (@0x00DCA)", i6);
    PRUEF(i7 > i6 && ist_zeilenanfang(b, nb, i7) && finde(b, nb, vor7, 12, i7 + 1) < 0,
          "ENTRIEGELN39 @+%04X: Sleep 86 (@0x00E10), Message_on 60 (Film), Message_on 7 (@0x00E14)", i7);
    PRUEF(b && finde(b, nb, glas, 3, 0) < 0, "kein Glasbruch Se_on(0x3E,0x21) mehr im Programm (der Film zeigt die Scheiben)");

    /* Haken */
    const unsigned alt = g_current_room_id;
    static const uint8_t z61[4] = { 0x2B, RE15_ZWS114_MSG_SCHREI, 0, 0 }, z60[4] = { 0x2B, RE15_ZWS114_MSG_FILM, 0, 0 },
                         z6[4] = { 0x2B, 0x06, 0, 0 }, z7[4] = { 0x2B, 0x07, 0, 0 };
    g_current_room_id = 0x1140u;
    PRUEF(re15_zws114_r41_message_on(z61) == 0 && re15_zws114_r41_message_on(z60) == 0, "ROOM1140: Zeilen 60/61 nicht vom Haken");
    g_current_room_id = RE15_ZWS114_RAUM;
    re15_zws114_r41_install(RE15_ZWS114_RAUM);
    PRUEF(re15_zws114_r41_message_on(z6) == 0 && re15_zws114_r41_message_on(z7) == 0, "ROOM1141: msg 6/7 selbst unberuehrt");
    re15_zws114_r41_schrei_hook = NULL;
    PRUEF(re15_zws114_r41_message_on(z61) == 1, "Schrei ohne Plattform: Zeile laeuft sofort durch (PSX-Fassung)");
    re15_zws114_r41_schrei_hook = fake_schrei;
    s_fake_takte = 3; s_fake_n = 0;
    int folge[4], k;
    for (k = 0; k < 4; k++) folge[k] = re15_zws114_r41_message_on(z61);   /* danach ruekt der pc weiter (pc += 4) */
    PRUEF(s_fake_n == 1 && s_fake_satz == RE15_ZWS114_SCHREI_SATZ && folge[0] == 2 && folge[1] == 2 && folge[2] == 2 &&
          folge[3] == 1, "Schrei 3 Takte: Start parkt, 2 x parken, im 3. Takt danach weiter (%d %d %d %d), Satz %d einmal",
          folge[0], folge[1], folge[2], folge[3], s_fake_satz);
    re15_zws114_r41_film_spieler = 0;
    PRUEF(re15_zws114_r41_message_on(z60) == 1, "Film ohne Plattform: Zeile laeuft sofort durch");
    re15_zws114_r41_film_spieler = 1;
    const int f1 = re15_zws114_r41_message_on(z60), f2 = re15_zws114_r41_message_on(z60), offen = re15_zws114_r41_film_offen();
    re15_zws114_r41_film_gespielt();
    const int f3 = re15_zws114_r41_message_on(z60);
    PRUEF(f1 == 2 && f2 == 2 && offen && f3 == 1 && re15_zws114_r41_film_zustand() == RE15_ZWS114_FILM_AUS,
          "Film: parkt (%d %d), angefordert, nach 'gespielt' weiter (%d), Zustand zurueck", f1, f2, f3);
    re15_zws114_r41_film_spieler = 0;
    re15_zws114_r41_schrei_hook = NULL;
    g_current_room_id = alt;
}

/* ---- Harness (wie test_r39_dining1141.c) --------------------------------------------------------------------- */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static long               s_bild = 0;

typedef struct { long bild; uint8_t bank, satz; } se_t;
static se_t s_se[8192];
static int  s_n_se = 0;

static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt;
    return 0;
}

static void spielbild(uint16_t held, uint16_t edge)
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
    while (scd_audio_queue_pop(&e)) {
        if (e.kind == SCD_AUDIO_SE_ON && s_n_se < (int)(sizeof s_se / sizeof s_se[0])) {
            s_se[s_n_se].bild = s_bild; s_se[s_n_se].bank = e.bank; s_se[s_n_se].satz = e.sample_id; s_n_se++;
            (void)re15_dining_r38_se_on(e.bank, e.sample_id);
        }
    }
    s_bild++;
}

static void frame(uint16_t held, uint16_t edge)
{
    if (!re15_discard_frozen()) spielbild(held, edge);
    else s_bild++;
    if (re15_discard_active()) re15_discard_tick((edge & RE15_PAD_BIT_SQUARE) ? (uint16_t)0x4000u : 0, 0);
}

static uint8_t *s_ems = NULL;
static int      s_ems_sz = 0;
static uint8_t  s_bank[0x60000];
static void bank_john(void)   /* John (Typ 0x49) aus CDEMD1.EMS (Muster test_r39_dining1141.c) */
{
    if (!s_ems) { size_t n = 0; s_ems = asset("EMD/CDEMD1.EMS", &n); s_ems_sz = (int)n; }
    if (!s_ems) { printf("  FEHLER: CDEMD1.EMS fehlt\n"); g_fail++; return; }
    int idx = re15_ems_index_for_type(0x49);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, (size_t)s_ems_sz, idx, &off, &len) != 0 || len > sizeof s_bank) {
        printf("  FEHLER: Bank 0x49 nicht im Archiv\n"); g_fail++; return; }
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x49);
    if (!eb) return;
    memcpy(s_bank, s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank, len, &eb->skel_own, &eb->anim_own) == 0);
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
    if (room == RE15_R38F_RAUM_1141) bank_john();
    re15_discard_reset();
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_room_rdt_ok = 1;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0; s_bild = 0; s_n_se = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* ---- Teil: ablauf -------------------------------------------------------------------------------------------- */
static int  s_echt_takte = -1;
static long s_bild_schrei = -1;
static int  s_n_schrei = 0, s_satz_schrei = -1;
static int echter_schrei(int satz)   /* Plattform-Ersatz: Laenge aus der echten Bank, Spielbild merken */
{
    s_bild_schrei = s_bild + 1;   /* Zaehlweise wie die Messungen nach frame(): s_bild nach diesem Takt */
    s_n_schrei++; s_satz_schrei = satz;
    return schrei_takte(satz, &s_rdt11c0);
}

#define FILM_HALTEN 60   /* so viele Spielbilder "laeuft" der Film im Riegel - das Skript muss so lange parken */

static void teil_ablauf(void)
{
    printf("== ablauf ==\n");
    if (bank_11c0() != 0) { printf("  FEHLER: ROOM11C0 fehlt\n"); g_fail++; return; }
    s_echt_takte = schrei_takte(RE15_ZWS114_SCHREI_SATZ, &s_rdt11c0);
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_R38F_F0_BANK, RE15_R38F_F0_BIT, 1);     /* (4,0xF0): ROOM1021 sub02 @0x01FE0 */
    if (room_boot(RE15_R38F_RAUM_1141, -7600, -17600, 0, 0) != 0) return;
    re15_zws114_r41_schrei_hook = echter_schrei;   /* Installer lief schon (scd_room_reenter) */
    re15_zws114_r41_film_spieler = 1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    PRUEF(re15_dining_r38_phase_h() == 1, "Phase H erkannt ((4,0xF0)=1, Dachtuer verriegelt)");
    /* Gemaelde: "ja" -> Rolladen hoch (wie test_r39_dining1141 phase_h), dann frei */
    pl->x = -8300; pl->z = -10250; pl->rot_y = 2048;
    frame(0, 0);
    long ja = -1;
    for (int f = 0; f < 600 && ja < 0; f++) {
        uint16_t e = (f == 0 || (g_scd.message_active && (f % 20) == 0)) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT)) ja = s_bild;
    }
    for (int f = 0; f < 600; f++) {
        frame(0, 0);
        if (g_scd.player_mode == 0 && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 0x1B) &&
            !re15_game_flag_get(RE15_R39J_MERKER_BANK, RE15_R39J_MERKER_BIT) && s_bild > ja + 30) break;
    }
    PRUEF(ja > 0, "Gemaelde \"ja\": Rolladen offen (Bild %ld)", ja);
    (void)re15_inv_grant(0x03, 15);
    /* Riegel der Dachtuer (Ereignis 5 -> ENTRIEGELN39): Vorwaerts-Punkt (-7250,-250), Blick +Z */
    pl->x = -7250; pl->z = -250 - 620; pl->rot_y = 3072;
    frame(0, 0);
    g_room_change.pending = 0;
    const int se0 = s_n_se;
    long b_msg[64]; int msgs[64], n_msg = 0, last = -1;
    long b_film_an = -1, b_film_aus = -1, b_msg6 = -1, b_msg7 = -1, b_msg8 = -1, b_schrei_ende = -1;
    int msg7_im_film = 0, halte = 0;
    int blende[32], n_blende = 0, vorher_blende = 0;
    re15_fade_kill(RE15_ZWS114_BLENDE_KANAL);
    for (int f = 0; f < 6000 && b_msg8 < 0; f++) {
        uint16_t h = 0, e = 0;
        if (f == 0) e = RE15_PAD_BIT_SQUARE;
        if (f < 12) h = RE15_PAD_BIT_SQUARE;
        if (g_scd.message_active && g_scd.message_id == 12 && (f % 15) == 0) e = h = RE15_PAD_BIT_SQUARE;
        frame(h, e);
        if (s_bild_schrei >= 0 && b_schrei_ende < 0 && re15_zws114_r41_schrei_rest() == 0) b_schrei_ende = s_bild;
        if (g_scd.message_active && (int)g_scd.message_id != last) {
            last = (int)g_scd.message_id;
            if (n_msg < 64) { msgs[n_msg] = last; b_msg[n_msg] = s_bild; n_msg++; }
            if (last == 6 && b_msg6 < 0) b_msg6 = s_bild;
            if (last == 7 && b_msg7 < 0) b_msg7 = s_bild;
            if (last == 8 && b_msg8 < 0) b_msg8 = s_bild;
        }
        if (!g_scd.message_active) last = -1;
        re15_fade_tick();   /* wie re15_render_end_frame (render_pc.c) je gezeichnetem Bild */
        if (b_film_an < 0 && g_fade_ch[RE15_ZWS114_BLENDE_KANAL].drawn) vorher_blende++;
        if (b_film_aus >= 0 && n_blende < 32 && s_bild > b_film_aus)
            blende[n_blende++] = g_fade_ch[RE15_ZWS114_BLENDE_KANAL].drawn ? g_fade_ch[RE15_ZWS114_BLENDE_KANAL].out_r : -1;
        /* Plattform-Ersatz Film: angefordert -> FILM_HALTEN Bilder "laufen lassen" (die echte Plattform spielt ihn
         * zwischen zwei Spielbildern; hier tickt das Spiel weiter, damit das Parken sichtbar wird), dann gespielt. */
        if (re15_zws114_r41_film_offen()) {
            if (b_film_an < 0) b_film_an = s_bild;
            if (g_scd.message_active && g_scd.message_id == 7) msg7_im_film = 1;
            if (++halte >= FILM_HALTEN) { re15_zws114_r41_film_gespielt(); b_film_aus = s_bild; }
        }
    }
    printf("    Nachrichten:"); for (int i = 0; i < n_msg; i++) printf(" %d@%ld", msgs[i], b_msg[i]); printf("\n");
    printf("    Schrei Bild %ld (Satz %d, %d Takte laut Bank), Tonende Bild %ld, msg 6 Bild %ld, Film an %ld aus %ld, "
           "msg 7 Bild %ld, msg 8 Bild %ld\n", s_bild_schrei, s_satz_schrei, s_echt_takte, b_schrei_ende, b_msg6, b_film_an,
           b_film_aus, b_msg7, b_msg8);
    int glas = 0;
    for (int i = se0; i < s_n_se; i++) if (s_se[i].bank == RE15_R39J_SE_BANK && s_se[i].satz == RE15_R39J_SATZ_GLAS) glas++;
    int i5 = -1, i6 = -1, i7 = -1, i8 = -1;
    for (int i = 0; i < n_msg; i++) {
        if (msgs[i] == 5 && i5 < 0) i5 = i;
        if (msgs[i] == 6 && i6 < 0) i6 = i;
        if (msgs[i] == 7 && i7 < 0) i7 = i;
        if (msgs[i] == 8 && i8 < 0) i8 = i;
    }
    PRUEF(i5 >= 0 && i6 == i5 + 1 && i7 == i6 + 1 && i8 == i7 + 1, "Zeilenfolge msg 5, 6, 7, 8 ohne Zwischenzeile (Port-Zeilen 58/60/61 ohne Text)");
    PRUEF(s_n_schrei == 1 && s_satz_schrei == RE15_ZWS114_SCHREI_SATZ && s_echt_takte == 37,
          "Schrei genau einmal, ROOM11C0 snd1 Satz %d, %d Takte", s_satz_schrei, s_echt_takte);
    PRUEF(i5 >= 0 && s_bild_schrei > b_msg[i5] && b_msg6 > s_bild_schrei,
          "Reihenfolge: msg 5 (Bild %ld) -> Schrei (Bild %ld) -> msg 6 \"What was that?!\" (Bild %ld)",
          i5 >= 0 ? b_msg[i5] : -1L, s_bild_schrei, b_msg6);
    PRUEF(b_schrei_ende - s_bild_schrei == s_echt_takte,
          "Skript parkt genau die Tonlaenge: %ld Takte (Soll %d)", b_schrei_ende - s_bild_schrei, s_echt_takte);
    PRUEF(b_msg6 - s_bild_schrei == s_echt_takte + 20,
          "Pause: msg 6 %ld Takte nach dem Schrei = Tonlaenge %d + Sleep 20 @0x00DC6 (%.2f s)", b_msg6 - s_bild_schrei,
          s_echt_takte, (b_msg6 - s_bild_schrei) / 30.0);
    PRUEF(b_film_an > b_msg6 && b_film_an - b_msg6 >= 5 + 5 + 5 + 86,
          "Film erst nach msg 6 + Drehen (Sleep 5 x3 @0x00DDC/@0x00DEE/@0x00DFE) + Sleep 86 @0x00E10: %ld Takte (%.2f s)",
          b_film_an - b_msg6, (b_film_an - b_msg6) / 30.0);
    PRUEF(!msg7_im_film && b_msg7 > b_film_aus, "waehrend der Film laeuft (%d Bilder) keine msg 7; msg 7 danach", FILM_HALTEN);
    PRUEF(b_msg7 - b_film_aus >= 0 && b_msg7 - b_film_aus <= 2,
          "msg 7 \"Shit! that is what I was talking about!\" direkt nach dem Film (%ld Takte)", b_msg7 - b_film_aus);
    PRUEF(glas == 0, "kein Glasbruch Se_on(0x3E,0x21) in der Szene (%d)", glas);
    {   /* RE2 ROOM1080 sub14: harter Schnitt in den Film, danach Einblenden 240 -> 0 in Schritten von 16 */
        int ok = n_blende >= 17 && vorher_blende == 0;
        for (int i = 0; ok && i < 16; i++) if (blende[i] != 240 - 16 * i) ok = 0;
        if (ok && blende[16] != -1) ok = 0;
        printf("    Blende nach dem Film:"); for (int i = 0; i < n_blende && i < 18; i++) printf(" %d", blende[i]); printf("\n");
        PRUEF(ok, "Ueberblendung wie RE2: vor dem Film kein Abblenden (%d Bilder), danach %d, %d ... %d, %d, Halt (15 Bilder "
                  "aus Schwarz, ROOM1080 sub14 @0x0252A)", vorher_blende, n_blende > 0 ? blende[0] : -9,
              n_blende > 1 ? blende[1] : -9, n_blende > 14 ? blende[14] : -9, n_blende > 15 ? blende[15] : -9);
    }
    PRUEF(b_msg8 > b_msg7, "danach wie bisher: msg 8 \"what the hell is that?!\" (Bild %ld)", b_msg8);
    re15_zws114_r41_schrei_hook = NULL;
    re15_zws114_r41_film_spieler = 0;
}

/* ---- Teil: blende ---------------------------------------------------------------------------------------------- */
static void teil_blende(void)
{
    printf("== blende ==\n");
    const unsigned alt = g_current_room_id;
    static const uint8_t z60[4] = { 0x2B, RE15_ZWS114_MSG_FILM, 0, 0 };
    g_current_room_id = RE15_ZWS114_RAUM;
    re15_fade_init();
    re15_zws114_r41_install(RE15_ZWS114_RAUM);
    re15_zws114_r41_film_spieler = 1;
    (void)re15_zws114_r41_message_on(z60);                       /* Film angefordert */
    PRUEF(re15_fade_done(RE15_ZWS114_BLENDE_KANAL), "vor/waehrend des Films kein Blendkanal aktiv (harter Schnitt wie RE2 "
                                                     "@0x8002ba28 vor DrawOTag)");
    re15_zws114_r41_film_gespielt();
    const re15_fade_ch_t *c = &g_fade_ch[RE15_ZWS114_BLENDE_KANAL];
    PRUEF(c->abr == 2 && c->mask_r == 0xFF && c->mask_g == 0xFF && c->mask_b == 0xFF && c->step == -0x800 &&
          c->ot_bucket == 7 && c->level == 0x7800,
          "nach dem Film: Kanal %d ABR %d RGB %02x%02x%02x Schritt %d Ebene %d Pegel 0x%04x (RE2 Sce_fade_set 53 02 02 07 00 f8)",
          RE15_ZWS114_BLENDE_KANAL, c->abr, c->mask_r, c->mask_g, c->mask_b, (int)c->step, c->ot_bucket, (unsigned)c->level);
    int folge[20], n = 0, ok = 1;
    for (int i = 0; i < 20; i++) { re15_fade_tick(); folge[n++] = c->drawn ? c->out_r : -1; }
    for (int i = 0; i < 16; i++) if (folge[i] != 240 - 16 * i) ok = 0;
    for (int i = 16; i < 20; i++) if (folge[i] != -1) ok = 0;
    printf("    Folge:"); for (int i = 0; i < n; i++) printf(" %d", folge[i]); printf("\n");
    PRUEF(ok, "Helligkeit 240, 224 ... 16 (15 Bilder), 0, dann Halt - FUN_8002c378 sra 0x17 @0x8002c3b8 / bltz @0x8002c3b4");
    PRUEF(re15_zws114_r41_message_on(z60) == 1, "Skript laeuft nach dem Film weiter");
    /* ohne gezeigten Film: kein Einblenden */
    re15_fade_init();
    (void)re15_zws114_r41_message_on(z60);
    re15_zws114_r41_film_fehlt();
    PRUEF(re15_fade_done(RE15_ZWS114_BLENDE_KANAL) && re15_zws114_r41_message_on(z60) == 1,
          "Film fehlt: kein Einblenden, Skript laeuft weiter");
    re15_zws114_r41_film_spieler = 0;
    g_current_room_id = alt;
}

int main(int argc, char **argv)
{
    if (argc < 2) { printf("Aufruf: test_r41_zws114 <datei|schrei|programm|ablauf|blende>\n"); return 2; }
    if (!strcmp(argv[1], "datei")) teil_datei();
    else if (!strcmp(argv[1], "schrei")) teil_schrei();
    else if (!strcmp(argv[1], "programm")) teil_programm();
    else if (!strcmp(argv[1], "ablauf")) teil_ablauf();
    else if (!strcmp(argv[1], "blende")) teil_blende();
    else { printf("unbekannter Teil %s\n", argv[1]); return 2; }
    printf("%s: %d Fehler\n", argv[1], g_fail);
    return g_fail ? 1 : 0;
}
