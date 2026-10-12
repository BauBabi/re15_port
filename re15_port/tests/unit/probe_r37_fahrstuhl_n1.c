/*
 * probe_r37_fahrstuhl_n1.c — Runde 37 Spur C Nachbesserung 1 (analysis/befunde_runde37/C_fahrstuhl.md "Nachbesserung 1",
 * AUFTRAG.md Nachtrag 5). Teile (je ein ctest, registriert in tests/unit/probes/r37_fahrstuhl.cmake):
 *
 *   lastenaufzug  Nachtrag 5a "ja, dann mache den noch" = Ausstieg Lastenaufzug ROOM3080 -> ROOM4000 mit Tuersequenz.
 *                 Daten: ROOM3080/3081 @0x0072A Null-Rechteck -> Stage 3 Raum 00 Cut 1, RE1.5-Archiv-Byte 0, Aot_on
 *                 `47 00` @0x00A22/@0x00BFA bzw. @0x0081E/@0x00862. Mechanik der Wahl: P2DS-Maschine Variante 5
 *                 (DOOR2D Skript 2: 451 Bilder, Schacht obj 2 y FAELLT = Buehne faehrt HINAB) gegen Variante 4 (Skript 1:
 *                 301 Bilder, y steigt = HINAUF, die Gegenrichtung S218). ECHTER Skriptweg (Raum betreten -> sub00 startet
 *                 die Fahrt -> Aot_on) mit dem player_mode-Proxy von main.c: erste Fahrt (Ada-Szene, (4,96)=0, sub02),
 *                 weitere Fahrt ((4,96)=1, sub09), Elza ROOM3081 ((4,0xFD)=0 sub02 / =1 sub03) -> genau EINE Anfrage
 *                 P2DS V5 (RE2-Basis 0x2D, Tuer 112), beim Feuern player_mode 2, Ziel ROOM4000/4001. Gegenprobe: die
 *                 Gegenrichtung S218 (ROOM4000 -> ROOM3070) bleibt P2DS V4.
 *   lift2030      Nachtrag 5b "in den Stage2 kleineres aufzügen fehlt noch das sound": Anker ROOM2030/2031 (Bytes,
 *                 Schleifenlaengen 45/90 x 2 Evt_next), Mini-Bank LIFT6030.VBS (Groesse/FNV, Ton-Bytes -> Tonhoehe 0x2C4/0x400,
 *                 FAHRT_KURZ = FAHRT ohne die Bloecke [794,1611), Stopp-Schlag beider Wellen 20 Bilder nach ihrem Fahrt-Ende
 *                 = RE2), ECHTER Weg: Generator (3,126)=1, auf der Plattform Aktionstaste -> KLICK + FAHRT_KURZ (Lift A)
 *                 bzw. KLICK + FAHRT (Lift B) im selben Takt, Fahrt 90 bzw. 180 Bilder bis zum Etagen-Merker (3,222)/(3,220),
 *                 hinauf und hinab, Leon (2030) und Elza (2031). Gegenprobe: ohne Generator kein Ton (msg 1).
 *   kapsel <mit.raw> <ohne.raw> <satz>  Auswertung fuer integration_r37_fahrstuhl_n1: Differenz zweier RE15_AUDIO_CAP_SYNC-
 *                 Aufnahmen (mit / ohne Aktionstaste) = der Lift-Ton; Beginn, Stopp-Schlag (Fahrt + 20 Bilder) und Ende
 *                 (Wellenlaenge) im Spieltakt (1470 Stereo-Bilder).
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
#include "re15_vab.h"
#include "re15_lastenaufzug_r37.h"
#include "re15_lift2030_r37.h"
#include "../../engine/src/gen/re2_lift6030_bank.inc"

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
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static uint32_t fnv1a(const uint8_t *d, size_t n)
{
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < n; i++) { h ^= d[i]; h *= 16777619u; }
    return h;
}

/* ---------------------------------------------------------------------------------------------
 * Spielschritt-Geruest (wie probe_r37_fahrstuhl.c)
 * ------------------------------------------------------------------------------------------ */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;
static int                s_kino = 1;      /* player_mode-Proxy wie main.c (cine_active -> 2) */
static int                s_kino_war = 0;

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_audio_event_t e;
    scd_vm_tick();
    if (s_kino) {
        /* Reihenfolge des PC-Hauptlaufs (platform/pc/main.c): SCD-Takt -> Plc_dest-Gang -> player_mode-Proxy
         * (flag(1,27)||flag(2,7) -> player_mode 2, Balken -1; fallende Flanke -> 15 Bilder, dann 0) -> Spielschritt */
        re15_actor_step_all_walkers();
        int ka = re15_cine_active();
        if (ka) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_kino_war) g_scd.letterbox_countdown = 15;
        s_kino_war = ka;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    while (scd_audio_queue_pop(&e)) { }
}

static int s_flags[8][2], s_n_flags = 0;

static int room_laden(uint16_t room, int warm)
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
    s_shown = 0; s_kino_war = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    if (!warm) return 0;
    for (int f = 0; f < 30; f++) frame(0, 0);
    int w = 0;
    while (w < 3000 &&
           (g_scd.player_mode == 2 || g_scd.letterbox_countdown != 0 || g_scd.message_active ||
            g_scd.message_display_frames > 0 || g_scd.message_query)) {
        uint16_t e = ((w % 10) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        w++;
    }
    g_room_change.pending = 0;
    return 0;
}

static re15_door_seq_anfrage_t s_gefangen;
static int s_n_gefangen = 0;
static int s_pmode_beim_fang = -1;
static void fang(const re15_door_seq_anfrage_t *a) { s_gefangen = *a; s_n_gefangen++; s_pmode_beim_fang = g_scd.player_mode; }

static const re15_tuer_eigen_t *eigen_nach_kennung(const char *k, int *idx)
{
    for (int i = 1; i <= re15_door_seq_eigen_anzahl(); i++) {
        const re15_tuer_eigen_t *e = re15_door_seq_eigen(i);
        if (e && !strcmp(e->kennung, k)) { if (idx) *idx = i; return e; }
    }
    return NULL;
}

/* =============================================================================================
 * Teil "lastenaufzug" (Nachtrag 5a)
 * ========================================================================================== */

/* P2DS-Maschine: Bilder bis Platz 10 aus, y von obj 2 (Schacht-Fachwerk, DOOR2D Mesh 2) je Bild. */
static int maschine_2d(const uint8_t *modell, int n, int var, int *bilder, int32_t *y2, int max_y)
{
    static re15_door_seq_t s;
    if (re15_door_seq_start(&s, modell, n, var, 0, 0x2D) != 0) return -1;
    int bild = 0;
    while (bild < 4000 && re15_door_seq_bild(&s, 1)) {
        if (bild < max_y) y2[bild] = s.obj[2].on ? s.obj[2].pos[1] : 0;
        bild++;
    }
    *bilder = bild;
    int ok = s.md1_ok && s.tim_ok && s.notizen == 0;
    re15_door_seq_ende(&s);
    return ok ? 0 : -2;
}

/* Richtung der Schacht-Bewegung: Summe der Bild-zu-Bild-Aenderungen von obj 2 y (nur bewegte Bilder). */
static void richtung(const int32_t *y, int n, int *steigt, int *faellt)
{
    *steigt = *faellt = 0;
    for (int i = 1; i < n; i++) {
        int32_t d = y[i] - y[i - 1];
        if (d > 0 && d <= 200) (*steigt)++;     /* je Bild +125 (Skript 4 @0x6300); Ruecksprung -7500 zaehlt nicht */
        if (d < 0 && d >= -200) (*faellt)++;    /* je Bild -125 (Skript 5 @0x6344); Ruecksprung +7500 zaehlt nicht */
    }
}

static void lift_fahrt(uint16_t raum, int bank, int bit, uint16_t ziel, const char *was)
{
    re15_door_seq_setze_laeufer(fang);
    s_n_flags = 0;
    if (bank >= 0) { s_n_flags = 1; s_flags[0][0] = bank; s_flags[0][1] = bit; }
    s_kino = 1;
    if (room_laden(raum, 0) != 0) { re15_door_seq_setze_laeufer(NULL); return; }
    s_n_gefangen = 0; memset(&s_gefangen, 0, sizeof s_gefangen); s_pmode_beim_fang = -1;
    int bilder = 0, kino = 0;
    while (bilder < 6000 && !g_room_change.pending) {
        /* Ada-Zeilen (msg 0/1) wie ein Spieler wegdruecken; sonst keine Eingabe */
        uint16_t e = ((g_scd.message_active || g_scd.message_query) && (bilder % 10) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (g_scd.player_mode == 2) kino++;
        bilder++;
    }
    const re15_tuer_eigen_t *e = s_gefangen.eigen ? re15_door_seq_eigen(s_gefangen.eigen) : NULL;
    printf("Lastenaufzug ROOM%04X %s: %d Bilder (%d mit player_mode 2), Raumwechsel -> ROOM%04X, Laeufer %d x: DOOR%02X V%d "
           "%s Tuer %u, player_mode beim Feuern %d\n", raum, was, bilder, kino, g_room_change.pending ? g_room_change.room_id : 0,
           s_n_gefangen, s_gefangen.re2_nr, s_gefangen.variante, e ? e->kennung : "(RE2-Datei)", s_gefangen.tuer,
           s_pmode_beim_fang);
    PRUEF(g_room_change.pending && g_room_change.room_id == ziel, "ROOM%04X %s: kein Ausstieg nach ROOM%04X", raum, was, ziel);
    PRUEF(s_n_gefangen == 1 && s_gefangen.archiv == RE15_DOOR_ARCHIV_RE2 && s_gefangen.re2_nr == 0x2D
          && s_gefangen.tuer_nr == 0x2D && s_gefangen.variante == 5 && s_gefangen.tuer == 112 && e
          && !strcmp(e->kennung, "P2DS"), "ROOM%04X %s: Ausstiegs-Sequenz P2DS V5 fehlt/falsch", raum, was);
    PRUEF(s_pmode_beim_fang == 2, "ROOM%04X %s: player_mode beim Feuern %d statt 2", raum, was, s_pmode_beim_fang);
    g_room_change.pending = 0;
    s_n_flags = 0;
    re15_door_seq_setze_laeufer(NULL);
}

static void teil_lastenaufzug(void)
{
    /* --- Daten der RDTs (Satz @0x0072A, Aot_on) --- */
    static const struct { uint16_t raum; uint32_t aot_on[2]; } k_raeume[] = {
        { 0x3080, { 0x00A22u, 0x00BFAu } },   /* sub02 (erste Fahrt) / sub09 */
        { 0x3081, { 0x0081Eu, 0x00862u } },   /* sub02 / sub03 (Elza) */
    };
    uint16_t raum; uint8_t zs, zr, basis, var; uint32_t off; const char *kenn;
    re15_lastenaufzug_r37_satz(&raum, &zs, &zr, &off, &kenn, &basis, &var);
    PRUEF(raum == 0x3080 && zs == 3 && zr == 0x00 && off == 0x0072A && basis == 0x2D && var == 5 && !strcmp(kenn, "P2DS"),
          "lastenaufzug_r37: Satz-Daten weichen ab");
    for (int r = 0; r < 2; r++) {
        char p[600]; size_t n = 0;
        snprintf(p, sizeof p, "%s/STAGE3/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, k_raeume[r].raum);
        uint8_t *d = slurp(p, &n);
        PRUEF(d != NULL && n > 0x1000, "%s fehlt", p);
        if (!d) continue;
        const uint8_t *s = d + off;
        int null_rechteck = 1;
        for (int k = 6; k < 14; k++) if (s[k]) null_rechteck = 0;
        printf("ROOM%04X @0x%05X: %02x %02x %02x %02x .. Rechteck %s, Ziel Stage %u Raum %02X Cut %u, RE1.5-Archiv-Byte %u; "
               "Aot_on @0x%05X %02x %02x / @0x%05X %02x %02x\n", k_raeume[r].raum, off, s[0], s[1], s[2], s[3],
               null_rechteck ? "(0,0,0,0)" : "!=0", s[22], s[23], s[24], s[26],
               k_raeume[r].aot_on[0], d[k_raeume[r].aot_on[0]], d[k_raeume[r].aot_on[0] + 1],
               k_raeume[r].aot_on[1], d[k_raeume[r].aot_on[1]], d[k_raeume[r].aot_on[1] + 1]);
        PRUEF(s[0] == 0x3B && s[1] == 0x00 && s[2] == 0x02 && null_rechteck && s[22] == 3 && s[23] == 0x00 && s[24] == 1
              && s[26] == 0x00, "ROOM%04X: Door_aot_set @0x%05X weicht ab", k_raeume[r].raum, off);
        for (int k = 0; k < 2; k++)
            PRUEF(d[k_raeume[r].aot_on[k]] == 0x47 && d[k_raeume[r].aot_on[k] + 1] == 0x00,
                  "ROOM%04X: Aot_on 0 @0x%05X fehlt", k_raeume[r].raum, k_raeume[r].aot_on[k]);
        free(d);
    }

    /* --- Mechanik: P2DS V5 = hinab (DOOR2D Skript 2/5), V4 = hinauf (Skript 1/4) --- */
    int i_p = 0;
    const re15_tuer_eigen_t *p2 = eigen_nach_kennung("P2DS", &i_p);
    PRUEF(p2 && p2->basis == 0x2D, "P2DS fehlt in gen/re15_tuer_eigen.inc");
    if (p2) {
        char p[600]; size_t n = 0;
        snprintf(p, sizeof p, "%s/RE15DOOR/P2DS.DO2", RE15_ASSET_SHARED_DIR);
        uint8_t *d = slurp(p, &n);
        PRUEF(d && n == p2->datei && fnv1a(d, n) == p2->fnv, "P2DS.DO2 passt nicht zur Tabelle");
        if (d && n == p2->datei) {
            const uint8_t *m = d + p2->sektor * 0x800u;
            static int32_t y5[600], y4[600];
            int b5 = 0, b4 = 0, st5, fa5, st4, fa4;
            PRUEF(maschine_2d(m, (int)p2->modell, 5, &b5, y5, 600) == 0, "P2DS V5: Maschine meldet Notizen/Formatfehler");
            PRUEF(maschine_2d(m, (int)p2->modell, 4, &b4, y4, 600) == 0, "P2DS V4: Maschine meldet Notizen/Formatfehler");
            richtung(y5, b5 < 600 ? b5 : 600, &st5, &fa5);
            richtung(y4, b4 < 600 ? b4 : 600, &st4, &fa4);
            printf("P2DS V5: %d Bilder, Schacht obj 2 y: %d Bilder fallend / %d steigend (= Buehne HINAB)\n", b5, fa5, st5);
            printf("P2DS V4: %d Bilder, Schacht obj 2 y: %d Bilder steigend / %d fallend (= Buehne HINAUF, S218)\n", b4, st4, fa4);
            /* je 60 Bilder Fahrt + Pause, bis zu 4 Runden (Skript 5 @0x6344 / Skript 4 @0x6300); nur eine Richtung */
            PRUEF(b5 == 451 && fa5 >= 100 && st5 == 0, "P2DS V5: nicht die Abwaertsfahrt (451 Bilder, y faellt)");
            PRUEF(b4 == 301 && st4 >= 100 && fa4 == 0, "P2DS V4: nicht die Aufwaertsfahrt (301 Bilder, y steigt)");
        }
        free(d);
    }

    /* --- echter Skriptweg --- */
    lift_fahrt(0x3080, -1, 0, 0x4000, "erste Fahrt (4,96)=0 sub02 Ada");
    lift_fahrt(0x3080, 4, 96, 0x4000, "weitere Fahrt (4,96)=1 sub09");
    lift_fahrt(0x3081, -1, 0, 0x4001, "Elza (4,0xFD)=0 sub02");
    lift_fahrt(0x3081, 4, 0xFD, 0x4001, "Elza (4,0xFD)=1 sub03");

    /* --- Gegenprobe: die Gegenrichtung S218 (ROOM4000 -> ROOM3070) bleibt P2DS V4 --- */
    re15_door_seq_anfrage_t q; unsigned r218 = 0;
    PRUEF(re15_door_seq_anfrage_fuer_seite(218, &q, &r218) && q.variante == 4 && q.eigen == i_p,
          "S218 ist nicht mehr P2DS V4");
    printf("Gegenprobe S218 ROOM%04X: DOOR%02X V%d eigen %d (P2DS = %d)\n", r218, q.re2_nr, q.variante, q.eigen, i_p);
}

/* =============================================================================================
 * Teil "lift2030" (Nachtrag 5b)
 * ========================================================================================== */
static int s_ton[16], s_ton_takt[16], s_ton_bild[16], s_n_ton = 0, s_bild = 0;
static void ton_spion(int satz)
{
    if (s_n_ton < 16) { s_ton[s_n_ton] = satz; s_ton_takt[s_n_ton] = (int)g_scd.tick_count; s_ton_bild[s_n_ton] = s_bild; s_n_ton++; }
}

/* Huellkurve (RMS je Spielbild) einer dekodierten Welle bei Tonhoehe p. */
static int huelle(const int16_t *pcm, int n, uint16_t p, double *rms, int max)
{
    double je_bild = 44100.0 * p / 4096.0 / 30.0;
    int k = 0;
    for (; k < max; k++) {
        int a = (int)(k * je_bild), b = (int)((k + 1) * je_bild);
        if (b > n) break;
        double s = 0;
        for (int i = a; i < b; i++) s += (double)pcm[i] * pcm[i];
        rms[k] = sqrt(s / (b - a));
    }
    return k;
}
static int cmp_d(const void *a, const void *b) { double x = *(const double *)a, y = *(const double *)b; return (x > y) - (x < y); }
static double median(const double *v, int n)
{
    static double t[512];
    if (n > 512) n = 512;
    memcpy(t, v, sizeof(double) * (size_t)n);
    qsort(t, (size_t)n, sizeof(double), cmp_d);
    return t[n / 2];
}
/* Stopp-Schlag = erstes Bild ab 'ab' mit RMS > 2 * Median der Fahrt (Bilder 30..80) — Regel des Werkzeugs. */
static int stopp_schlag(const double *rms, int n, int ab)
{
    double m = median(rms + 30, 50);
    for (int k = ab; k < n; k++) if (rms[k] > 2.0 * m) return k;
    return -1;
}

static void teil_bank(void)
{
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/LIFT6030.VBS", RE15_ASSET_RE2_DIR);
    uint8_t *d = slurp(p, &n);
    PRUEF(d && n == RE2_LIFT_DATEI_GROESSE && fnv1a(d, n) == RE2_LIFT_DATEI_FNV,
          "LIFT6030.VBS fehlt/weicht ab (Groesse/FNV aus gen/re2_lift6030_bank.inc)");
    if (!d || n != RE2_LIFT_DATEI_GROESSE) { free(d); return; }
    uint32_t vh_off = rd32(d + RE2_LIFT_EDT_SIZE - 8);
    static re15_vab_t vab;
    PRUEF(re15_vab_parse(d + vh_off, RE2_LIFT_EDT_SIZE - vh_off, &vab) == 0 && vab.vag_count == 3, "LIFT6030.VBS: VH");
    int vags[3][2], tones[3][2];
    static int16_t pcm[3][70000];
    int np[3] = { 0, 0, 0 };
    uint16_t pit[3] = { 0, 0, 0 };
    for (int s = 0; s < 3; s++) {
        PRUEF(re15_edt_resolve_layers_ex(d, &vab, s, vags[s], tones[s], 2) == 1, "Satz %d: nicht genau eine Lage", s);
        const re15_vab_tone_t *t = &vab.tones[tones[s][0]];
        pit[s] = re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift);
        np[s] = re15_vag_adpcm_decode(d + RE2_LIFT_VBD_OFF + vab.samples[vags[s][0]].offset, vab.samples[vags[s][0]].size,
                                      pcm[s], 70000);
        printf("Satz %d: Ton note %u center %u shift %u vol %u -> pitch 0x%X, VAG %d %u B = %d Abtastwerte = %.2f s\n", s,
               t->min_note, t->center_note, t->pitch_shift, t->vol, pit[s], vags[s][0], vab.samples[vags[s][0]].size, np[s],
               np[s] / (44100.0 * pit[s] / 4096.0));
    }
    /* Ton-Bytes = RE2 ROOM6030 Prog 1 Ton 10 (@0x0ED48: center 93, shift 0, note 69) / Ton 1 (@0x0EC28: 91/42/60) */
    PRUEF(pit[0] == 0x400 && pit[1] == RE2_LIFT_FAHRT_PITCH && pit[2] == RE2_LIFT_FAHRT_PITCH, "Tonhoehen weichen ab");
    /* FAHRT_KURZ = FAHRT ohne die Bloecke [794,1611) (Werkzeug, bytegleich davor/danach) */
    const uint8_t *w1 = d + RE2_LIFT_VBD_OFF + vab.samples[vags[1][0]].offset;
    const uint8_t *w2 = d + RE2_LIFT_VBD_OFF + vab.samples[vags[2][0]].offset;
    uint32_t n1 = vab.samples[vags[1][0]].size, n2 = vab.samples[vags[2][0]].size;
    PRUEF(n1 == 34320 && n2 == n1 - 817u * 16u && !memcmp(w2, w1, 794 * 16) && !memcmp(w2 + 794 * 16, w1 + 1611 * 16, n1 - 1611 * 16),
          "FAHRT_KURZ ist nicht FAHRT ohne die Bloecke [794,1611)");
    /* Stopp-Schlag: 20 Bilder nach dem Fahrt-Ende - FAHRT (RE2 For 180 @0x02FDC) und FAHRT_KURZ (Lift A 90 Bilder) */
    static double r1[400], r2[400];
    int k1 = huelle(pcm[1], np[1], pit[1], r1, 400), k2 = huelle(pcm[2], np[2], pit[2], r2, 400);
    int s1 = stopp_schlag(r1, k1, 150), s2 = stopp_schlag(r2, k2, 95);
    printf("Stopp-Schlag FAHRT Bild %d (Fahrt 180 + %d), FAHRT_KURZ Bild %d (Fahrt 90 + %d); Wellenende %d / %d Bilder\n",
           s1, s1 - 180, s2, s2 - 90, k1, k2);
    PRUEF(s1 - 180 == RE2_LIFT_STOPP_NACH_FAHRT && s2 - 90 == RE2_LIFT_STOPP_NACH_FAHRT,
          "Stopp-Schlag nicht %d Bilder nach dem Fahrt-Ende", RE2_LIFT_STOPP_NACH_FAHRT);
    free(d);
}

/* Plattform-Platz (Aot_set sce 3) des Lifts mit Mitte (cx,cz) auf Band 'band' suchen. */
static int lift_platz(int32_t cx, int32_t cz, int band)
{
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->has_quad || a->half_w != 750 || a->half_h != 750) continue;
        if (a->x == cx && a->z == cz && a->band == band) return i;
    }
    return -1;
}

static void lift_echt(uint16_t raum, char lift, int oben, int strom)
{
    /* Lift A: obj 1, Platz 4 (Ebene 6) / 5 (Ebene 3), Mitte (6600,12100) = Aot_set (5850,11350,1500,1500); Merker (3,222)
     * Lift B: obj 2, Platz 6 (Ebene 9) / 7 (Ebene 3), Mitte (-9800,3600)  = Aot_set (-10550,2850,1500,1500); Merker (3,220) */
    int32_t cx = lift == 'A' ? 6600 : -9800, cz = lift == 'A' ? 12100 : 3600;
    int merker = lift == 'A' ? 222 : 220;
    int band = oben ? (lift == 'A' ? 6 : 9) : 3;
    int bilder_soll = lift == 'A' ? 90 : 180;
    int satz_soll = lift == 'A' ? RE15_LIFT_TON_FAHRT_KURZ : RE15_LIFT_TON_FAHRT;
    s_n_flags = 0;
    if (strom) { s_flags[s_n_flags][0] = 3; s_flags[s_n_flags][1] = 126; s_n_flags++; }
    if (oben)  { s_flags[s_n_flags][0] = 3; s_flags[s_n_flags][1] = merker; s_n_flags++; }
    s_kino = 1;
    if (room_laden(raum, 1) != 0) return;
    re15_lift2030_r37_reset();
    s_n_ton = 0; s_bild = 0;
    re15_lift2030_ton_hook = ton_spion;
    int platz = lift_platz(cx, cz, band);
    PRUEF(platz >= 0, "ROOM%04X Lift %c: Plattform-Platz Band %d fehlt", raum, lift, band);
    if (platz < 0) { re15_lift2030_ton_hook = NULL; s_n_flags = 0; return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Standplatz 620 vor der Mitte, Blick auf die Mitte (Vorwaerts-Probe wie probe_r37_fahrstuhl.c) */
    pl->rot_y = 0; pl->x = cx - 620; pl->z = cz;
    pl->floor = (uint8_t)band; pl->y = -band * 1800; re15_collision_set_band(band);
    frame(0, 0); frame(0, 0);
    int merker_vor = re15_game_flag_get(3, (uint8_t)merker);
    int fahrt_bild = -1, ende_bild = -1;
    for (s_bild = 0; s_bild < 400; s_bild++) {
        uint16_t e = (s_bild == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (fahrt_bild < 0) for (int k = 0; k < s_n_ton; k++) if (s_ton[k] != RE15_LIFT_TON_KLICK) fahrt_bild = s_ton_bild[k];
        if (ende_bild < 0 && re15_game_flag_get(3, (uint8_t)merker) != merker_vor) ende_bild = s_bild;
    }
    printf("ROOM%04X Lift %c %s%s: Platz %d, Toene", raum, lift, oben ? "hinab" : "hinauf", strom ? "" : " OHNE Strom", platz);
    for (int k = 0; k < s_n_ton; k++) printf(" %d@Takt%d/Bild%d", s_ton[k], s_ton_takt[k], s_ton_bild[k]);
    printf(", Anker @0x%05X, Merker (3,%d) %d -> %d nach %d Bildern ab dem Fahrt-Ton, Spieler y %d\n",
           re15_lift2030_r37_letzter_anker(), merker, merker_vor, re15_game_flag_get(3, (uint8_t)merker),
           (fahrt_bild >= 0 && ende_bild >= 0) ? ende_bild - fahrt_bild : -1, pl->y);
    if (!strom) {
        PRUEF(s_n_ton == 0, "ROOM%04X Lift %c ohne Strom: %d Toene (soll 0, msg 1)", raum, lift, s_n_ton);
    } else {
        PRUEF(s_n_ton == 2 && s_ton[0] == RE15_LIFT_TON_KLICK && s_ton[1] == satz_soll && s_ton_takt[0] == s_ton_takt[1],
              "ROOM%04X Lift %c: Toene nicht KLICK + %s im selben Takt", raum, lift, lift == 'A' ? "FAHRT_KURZ" : "FAHRT");
        PRUEF(ende_bild >= 0 && fahrt_bild >= 0 && ende_bild - fahrt_bild == bilder_soll,
              "ROOM%04X Lift %c: Fahrt %d Bilder statt %d", raum, lift, ende_bild - fahrt_bild, bilder_soll);
    }
    re15_lift2030_ton_hook = NULL;
    s_n_flags = 0;
}

static void teil_lift2030(void)
{
    /* --- Anker: Bytes in ROOM2030 und ROOM2031, Schleifenlaenge passt zum Satz --- */
    PRUEF(re15_lift2030_r37_anzahl() == 6, "lift2030_r37: %d Anker statt 6", re15_lift2030_r37_anzahl());
    for (int r = 0; r < 2; r++) {
        char p[600]; size_t n = 0;
        uint16_t raum = (uint16_t)(0x2030 + r);
        snprintf(p, sizeof p, "%s/STAGE2/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, raum);
        uint8_t *d = slurp(p, &n);
        PRUEF(d != NULL, "%s fehlt", p);
        if (!d) continue;
        for (int i = 0; i < re15_lift2030_r37_anzahl(); i++) {
            uint16_t rr; uint32_t off; int satz;
            re15_lift2030_r37_anker(i, &rr, &off, &satz);
            const uint8_t *s = d + off;
            if (satz == RE15_LIFT_TON_KLICK) {
                PRUEF(s[0] == 0x22 && s[1] == 0x02 && s[2] == 0x07 && s[3] == 0x01, "ROOM%04X @0x%05X: nicht Set(2,7)=1", raum, off);
            } else {
                /* For: 0d 00 06 00 <Anzahl> 00, Koerper 30 02 02 00 0e 00 = Add_speed, 2x Evt_next, Nop, Next */
                int anzahl = s[4] | (s[5] << 8);
                int ok = s[0] == 0x0D && s[2] == 0x06 && s[6] == 0x30 && s[7] == 0x02 && s[8] == 0x02 && s[10] == 0x0E;
                PRUEF(ok && anzahl * 2 == (satz == RE15_LIFT_TON_FAHRT ? 180 : 90),
                      "ROOM%04X @0x%05X: Schleife %d x 2 passt nicht zu Satz %d", raum, off, anzahl, satz);
                if (r == 0) printf("Anker @0x%05X: For %d x (Add_speed, 2x Evt_next) = %d Bilder -> Satz %d\n", off, anzahl,
                                   anzahl * 2, satz);
            }
        }
        free(d);
    }
    teil_bank();
    /* --- echter Weg: Generator an, auf der Plattform Aktionstaste --- */
    for (int r = 0; r < 2; r++) {
        uint16_t raum = (uint16_t)(0x2030 + r);
        lift_echt(raum, 'A', 0, 1);
        lift_echt(raum, 'A', 1, 1);
        lift_echt(raum, 'B', 0, 1);
        lift_echt(raum, 'B', 1, 1);
    }
    lift_echt(0x2030, 'A', 0, 0);   /* Gegenprobe: ohne Strom (3,126)=0 -> msg 1, kein Ton */
}

/* =============================================================================================
 * Teil "kapsel" (Auswertung der exe-Aufnahmen fuer integration_r37_fahrstuhl_n1)
 * ========================================================================================== */
static int teil_kapsel(const char *mit, const char *ohne, int satz)
{
    size_t na = 0, nb = 0;
    uint8_t *a = slurp(mit, &na), *b = slurp(ohne, &nb);
    PRUEF(a && b, "Aufnahmen fehlen (%s / %s)", mit, ohne);
    if (!a || !b) { free(a); free(b); return 1; }
    size_t n = (na < nb ? na : nb) / 4;          /* Stereo-Bilder s16 */
    const int T = 1470;                          /* RE15_AUDIO_RATE / 30 (audio_pc.c RE15_AUDIO_CAP_SYNC) */
    int takte = (int)(n / T);
    static double r[20000];
    if (takte > 20000) takte = 20000;
    int start = -1, ende = -1;
    for (int k = 0; k < takte; k++) {
        double s = 0;
        for (int i = 0; i < T; i++) {
            size_t o = ((size_t)k * T + i) * 4;
            int16_t la = (int16_t)(a[o] | (a[o + 1] << 8)), ra = (int16_t)(a[o + 2] | (a[o + 3] << 8));
            int16_t lb = (int16_t)(b[o] | (b[o + 1] << 8)), rb = (int16_t)(b[o + 2] | (b[o + 3] << 8));
            double dl = (double)la - lb, dr = (double)ra - rb;
            s += dl * dl + dr * dr;
        }
        r[k] = sqrt(s / (2.0 * T));
        if (r[k] > 0) { if (start < 0) start = k; ende = k; }
    }
    int fahrt = satz == RE15_LIFT_TON_FAHRT_KURZ ? 90 : 180;
    int stopp = start >= 0 ? stopp_schlag(r + start, takte - start, fahrt - 10) : -1;
    /* Wellenende im Takt: Abtastwerte / (44100 * 0x2C4 / 4096 / 30) */
    int laenge_soll = satz == RE15_LIFT_TON_FAHRT_KURZ ? (int)(1328 * 28 / (44100.0 * 0x2C4 / 4096.0 / 30.0))
                                                     : (int)(2145 * 28 / (44100.0 * 0x2C4 / 4096.0 / 30.0));
    printf("Kapsel: %d Takte, Differenz von Takt %d bis %d (%d Takte, Welle %d), Stopp-Schlag +%d (Fahrt %d + %d)\n",
           takte, start, ende, ende - start, laenge_soll, stopp, fahrt, stopp - fahrt);
    if (start >= 0) {
        printf("Huelle der Differenz ab Takt %d:", start);
        for (int k = 0; k < ende - start + 2 && k < 260; k++) printf("%s%d", (k % 15) ? " " : "\n  ", (int)r[start + k]);
        printf("\n");
    }
    PRUEF(start >= 0, "keine Differenz: kein Lift-Ton");
    PRUEF(stopp - fahrt >= RE2_LIFT_STOPP_NACH_FAHRT - 2 && stopp - fahrt <= RE2_LIFT_STOPP_NACH_FAHRT + 2,
          "Stopp-Schlag +%d statt Fahrt %d + %d", stopp, fahrt, RE2_LIFT_STOPP_NACH_FAHRT);
    PRUEF(ende - start >= laenge_soll - 2 && ende - start <= laenge_soll + 2, "Ton dauert %d Takte statt %d", ende - start,
          laenge_soll);
    free(a); free(b);
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "lastenaufzug";
    if (!strcmp(teil, "lastenaufzug")) teil_lastenaufzug();
    else if (!strcmp(teil, "lift2030")) teil_lift2030();
    else if (!strcmp(teil, "kapsel") && argc > 4) teil_kapsel(argv[2], argv[3], atoi(argv[4]));
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf("%s: %s (%d Fehler)\n", teil, g_fehler ? "ROT" : "GRUEN", g_fehler);
    return g_fehler ? 1 : 0;
}
