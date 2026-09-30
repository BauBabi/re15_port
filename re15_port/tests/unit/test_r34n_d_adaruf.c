/* test_r34n_d_adaruf.c — RIEGEL: Ada-Ruf an der Tuer ROOM1050 -> ROOM10A0, Sperre bis zur Ada-Rettung.
 *
 * Runde 34 Nacht, Spur D (PORT-WAHL auf Nutzerwunsch). Dossier: analysis/befunde_runde34_nacht/D_adaruf.md
 * (§4 Zeitlinie, §5 Bauplan, §9 Umsetzung), Konstanten und Belege: include/re15_adaruf.h,
 * engine/src/adaruf_1050.c. Gegenpruefung: D_adaruf.gegenpruefung.md (Auflagen 5 und 6 hier als Teile).
 *
 * Verfahren wie test_r33_tuer1120.c: die ausgelieferte RDT wird geladen, der Raum ueber scd_room_reenter
 * aufgebaut (derselbe Einhaengepunkt wie im Spiel: Tuer, Debug-Sprung — Installation re15_adaruf_install),
 * der Spieler vor die Tuer gestellt (Vorwaerts-620-Punkt im Rechteck, FUN_80042bac @0x80042bd0) und
 * Quadrat gedrueckt: der echte Aktions-Scan im Spielschritt meldet Ereignis 13, scd_event_fire startet
 * ueber die ECHTE Weiche (re15_adaruf_ereignis) den Faden. Jedes Bild laeuft in der Reihenfolge von
 * platform/pc/main.c (scd_vm_tick :5378 -> re15_actor_step_all_walkers :5419 -> re15_letterbox_tick
 * :5440 -> Szenen-Uebergabe :5445-5468 -> re15_msg_tick -> re15_game_step).
 *
 * Teile (je ein ctest-Eintrag):
 *   szene    Erster Druck, Ada nicht gerettet: Szene statt Raumwechsel; Nachrichtenfolge 22 -> 23 -> 24;
 *            Ruf VOR dem Rueckschritt; Rueckschritt <= 12 Bilder, Weg 600..760, z unveraendert, Blick
 *            bleibt zur Tuer; Clip 19 vor, Clip 19 rueckwaerts (Plc_flg 0x80), Clip 17; Balken voll und
 *            wieder weg; Flags danach (9,65)=1 (2,7)=0 (1,27)=0; Slot 4 = Text-Platz msg 25; kein
 *            Raumwechsel, keine Tuersequenz.
 *   doppel   (Auflage 5) Quadrat alle 10 Bilder waehrend der Szene + direkter zweiter scd_event_fire(13)
 *            nach dem ersten VM-Takt -> genau EIN Faden, Ziel-Operanden unveraendert.
 *   sperre   Szene gesehen, ECHTER Raumwechsel 1050 -> 1000 -> 1050: Slot 4 = Text-Platz; Druck ->
 *            "I have to help the Survivor first!", Freeze 0xffff0000, 150 Bilder kein Raumwechsel,
 *            keine Tuersequenz; zweiter Druck ebenso.
 *   frei     (3,0xBB)=1 (mit und ohne (9,65)): Original-Tuer, Druck -> ROOM10A0 + genau eine
 *            Tuersequenz S021 DOOR07 V0 (gen/re15_tuer_eigen.inc Zeile ROOM1050@0xB5A).
 *   rettung  ECHTE Freigabe: ROOM1090 sub03 setzt (3,0xBB) im Port-VM selbst (@0x024D2); danach ROOM1050:
 *            die "Hey, wait!"-Szene loescht (3,0x6E) (@0x00D88) — die Tuer bleibt trotzdem frei, auch
 *            beim Wiederbetreten (Gegenprobe zur Freigabe ueber (3,0x6E)).
 *   elza     ROOM1051: keine Sperre, Druck -> ROOM10A1.
 *   raster   (Auflage 6) alle begehbaren Druckstellen im Raster 100 x 100, Gierung je 256: ueberall
 *            Ereignis 13, Faden endet, Rueckschritt <= 12 Bilder, Weg 600..760, |dz| < 100. Die
 *            Begehbarkeit misst der Teil selbst (Lauf nach Osten mit Pad UP je Rasterzeile) — Stellen
 *            IN der Wand (im Spiel unerreichbar) fallen heraus.
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
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_door_seq.h"
#include "re15_adaruf.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* Standplatz vor Slot 4 (probe_r33_tueren standplatz 1050 10A0): Punkt 620 voraus = (17200,-13700) =
 * Mitte des Tuer-Rechtecks (16700..17700, -14700..-12700) aus Door_aot_set @0x00B5A. */
#define STAND_X  16580
#define STAND_Z  (-13700)
/* Eintritts-Cut 4: ROOM1000 Door_aot_set Slot 0 @0x00BBE Byte 24 = 4 (die Tuer in den 1050-Suedteil). */
#define EINTRITT_CUT_1050  4

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;

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

static uint16_t s_rdt_raum = 0;
static int rdt_laden(uint16_t room)
{
    if (s_raw && s_rdt_raum == room) return 0;          /* derselbe Raum: geparste Daten wiederverwenden */
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR,
             (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (siehe Kopf). */
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
    while (scd_audio_queue_pop(&e)) { }
}

/* Raum aufbauen OHNE die Flags anzufassen (die gehoeren dem Spiel, nicht dem Raum) — wie
 * test_r33_tuer1120.c room_boot. warm_druck: laufende Szenen/Texte mit Quadrat wegblaettern. */
static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut, int warm_druck,
                     int bilder)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    /* scd_vm_init ist die NEUES-SPIEL-Initialisierung und nullt die Flags (scd_vm.c re15_game_state_init);
     * ein Raumwechsel im Spiel ruft sie nicht (scd_room_setup.c "= scd_vm_init MINUS
     * re15_game_state_init (keep flags)") — also hier die Flags retten. */
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;               /* neuer Raum im Test = neuer Prozessabschnitt: Balken zu */
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    /* Reihenfolge wie room_common.c: der Nachrichtenblock kommt NACH scd_room_reenter (:354/:392). */
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int f = 0; f < bilder; f++) frame(0, 0);
    int warm = 0;
    while (warm_druck && warm < 3000 &&
           (g_scd.player_mode == 2 || g_scd.letterbox_countdown != 0 || g_scd.message_active ||
            g_scd.message_display_frames > 0 || g_scd.message_query)) {
        uint16_t e = ((warm % 10) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        warm++;
    }
    return 0;
}

static int vorwaerts_trifft(const re15_aot_t *a, int32_t px, int32_t pz, int yaw)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    int32_t fx = px + (int32_t)((620 * c) >> 12);
    int32_t fz = pz - (int32_t)((620 * s) >> 12);
    long dx = (long)fx - (long)a->x, dz = (long)fz - (long)a->z;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    return dx <= a->half_w && dz <= a->half_h;
}

/* Tuersequenz-Laeufer: faengt jede Anfrage (im Spiel spielt ihn die Plattform). */
static int                     s_n_seq = 0;
static re15_door_seq_anfrage_t s_seq;
static void fang(const re15_door_seq_anfrage_t *a) { s_n_seq++; s_seq = *a; }

/* Wie viele Faeden fuehren gerade das Port-Programm aus? (Slot des ersten in *slot.) */
static int faeden_im_programm(int *slot)
{
    const uint8_t *p = re15_adaruf_laufprogramm();
    int n = 0;
    if (slot) *slot = -1;
    for (int s = 0; s < SCD_THREAD_COUNT; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && t->pc >= p && t->pc < p + RE15_ADARUF_PROG_LEN) {
            if (slot && *slot < 0) *slot = s;
            n++;
        }
    }
    return n;
}

static int prog_versatz(void)
{
    int slot; if (faeden_im_programm(&slot) < 1) return -1;
    return (int)(g_scd.threads[slot].pc - re15_adaruf_laufprogramm());
}

static int16_t le16(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }

typedef struct { int stand, raumwechsel, ziel, msg_id, freeze, seq; } druck_t;

/* Vor Slot `slot` stellen (Vorwaerts-620-Punkt in dessen Rechteck, Spieler bleibt stehen) und einmal
 * Quadrat druecken. Danach bis zu 150 Bilder beobachten, offene Nachrichten blaettern
 * (test_r33_tuer1120.c drueck_vor). */
static druck_t drueck_vor(int slot)
{
    druck_t r; memset(&r, 0, sizeof r); r.msg_id = -1; r.ziel = -1;
    re15_aot_t *a = &g_aot.slots[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    static const int dxf[5] = { 0, 1, -1, 0, 0 }, dzf[5] = { 0, 0, 0, 1, -1 };
    for (int k = 0; k < 5 && !r.stand; k++) {
        long zx = a->x + dxf[k] * (long)(a->half_w / 2);
        long zz = a->z + dzf[k] * (long)(a->half_h / 2);
        for (int d = 0; d < 16 && !r.stand; d++) {
            int yaw = d * 256;
            int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
            int32_t px = (int32_t)zx - (int32_t)((620 * c) >> 12);
            int32_t pz = (int32_t)zz + (int32_t)((620 * s) >> 12);
            pl->rot_y = (int16_t)yaw; pl->x = px; pl->z = pz;
            frame(0, 0);
            g_room_change.pending = 0;
            if (pl->x == px && pl->z == pz && (int)pl->rot_y == yaw && vorwaerts_trifft(a, px, pz, yaw))
                r.stand = 1;
        }
    }
    if (!r.stand) return r;
    printf("  Standplatz (%d,%d) Blick %d\n", (int)pl->x, (int)pl->z, (int)pl->rot_y);
    s_n_seq = 0;
    g_room_change.pending = 0;
    for (int f = 0; f < 150; f++) {
        uint16_t e = 0;
        if (f == 0) e = RE15_PAD_BIT_SQUARE;
        else if (f > 20 && g_scd.message_active && (f % 10) == 0) e = RE15_PAD_BIT_SQUARE;
        frame(e, e);
        if (g_scd.message_active && r.msg_id < 0) {
            r.msg_id = (int)g_scd.message_id;
            r.freeze = ((g_re15_pauseflags & 0xffff0000u) == 0xffff0000u);
        }
        if (g_room_change.pending) { r.raumwechsel = 1; r.ziel = (int)g_room_change.room_id; break; }
    }
    r.seq = s_n_seq;
    return r;
}

/* ---- Die Szene fahren und vermessen -------------------------------------------------------------- */
typedef struct {
    int gestartet;               /* Faden mit dem Port-Programm im Druckbild gestartet            */
    int ereignis;                /* g_aot.fired_event_id_this_frame im Druckbild                  */
    int msg_bild[3];             /* erstes Bild, in dem msg 22/23/24 aktiv ist                     */
    int msg_folge_ok;            /* nur 22,23,24 in dieser Reihenfolge, keine andere Nachricht     */
    int schritt_start, ankunft;  /* Bild mit PC > +0x38 bzw. > +0x48                               */
    double weg; int dz, gierung_ende;
    int clip19_vor, clip19_rueck, clip17;   /* Bild, in dem der Clip (mit Flag) zuerst steht      */
    int ende;                    /* Bild, in dem der Faden weg ist                                 */
    int lb_voll, lb_weg;         /* Letterbox 0xF0 erreicht / nach dem Ende wieder 0               */
    int pm_frei;                 /* player_mode nach dem Ende wieder 0                             */
    int raumwechsel, seq;
    int max_faeden;
    int ziel_geaendert;
} szene_t;

static szene_t szene_fahren(int quadrat_waehrend, int zweiter_fire)
{
    szene_t r; memset(&r, 0, sizeof r);
    r.msg_bild[0] = r.msg_bild[1] = r.msg_bild[2] = -1;
    r.schritt_start = r.ankunft = r.ende = r.lb_voll = r.lb_weg = -1;
    r.clip19_vor = r.clip19_rueck = r.clip17 = -1;
    r.msg_folge_ok = 1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_n_seq = 0; g_room_change.pending = 0;

    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    r.ereignis = g_aot.fired_event_id_this_frame;
    r.gestartet = faeden_im_programm(NULL);
    if (!r.gestartet) return r;
    int32_t sx = pl->x, sz = pl->z;
    uint8_t ziele[8];
    memcpy(ziele, re15_adaruf_laufprogramm() + RE15_ADARUF_OFF_DREH, 4);
    memcpy(ziele + 4, re15_adaruf_laufprogramm() + RE15_ADARUF_OFF_ZIEL, 4);
    int zuletzt = -1;
    for (int f = 1; f <= 700; f++) {
        uint16_t e = (quadrat_waehrend && (f % 10) == 0 && r.ende < 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (zweiter_fire && f == 1) {
            int s2 = scd_event_fire(RE15_ADARUF_EREIGNIS);
            printf("  direkter zweiter scd_event_fire(13) nach dem ersten VM-Takt -> %d\n", s2);
        }
        int nf = faeden_im_programm(NULL);
        if (nf > r.max_faeden) r.max_faeden = nf;
        if (memcmp(ziele, re15_adaruf_laufprogramm() + RE15_ADARUF_OFF_DREH, 4) != 0 ||
            memcmp(ziele + 4, re15_adaruf_laufprogramm() + RE15_ADARUF_OFF_ZIEL, 4) != 0)
            r.ziel_geaendert = 1;
        int off = prog_versatz();
        if (g_scd.message_active) {
            int id = (int)g_scd.message_id;
            if (id != zuletzt) {
                int k = id - RE15_ADARUF_MSG_RUF;
                if (k < 0 || k > 2) r.msg_folge_ok = 0;
                else {
                    if (r.msg_bild[k] < 0) r.msg_bild[k] = f;
                    if (k > 0 && r.msg_bild[k - 1] < 0) r.msg_folge_ok = 0;
                }
                zuletzt = id;
            }
        }
        if (pl->motion == 19 && !(pl->anim_flags & 0x80) && r.clip19_vor < 0)  r.clip19_vor = f;
        if (pl->motion == 19 &&  (pl->anim_flags & 0x80) && r.clip19_rueck < 0) r.clip19_rueck = f;
        if (pl->motion == 17 && r.clip17 < 0) r.clip17 = f;
        if (r.schritt_start < 0 && off > 0x38) r.schritt_start = f;
        if (r.ankunft < 0 && off > 0x48) {
            r.ankunft = f;
            r.weg = sqrt((double)(pl->x - sx) * (pl->x - sx) + (double)(pl->z - sz) * (pl->z - sz));
            r.dz = (int)(pl->z - sz);
            r.gierung_ende = pl->rot_y & 0xfff;
        }
        if (r.lb_voll < 0 && g_letterbox_level >= 0xF0) r.lb_voll = f;
        if (r.ende < 0 && off < 0) r.ende = f;
        if (r.ende > 0 && r.lb_weg < 0 && g_letterbox_level == 0) r.lb_weg = f;
        if (g_room_change.pending) r.raumwechsel = 1;
        if (r.ende > 0 && f > r.ende + 30) break;
    }
    r.pm_frei = (g_scd.player_mode == 0);
    r.seq = s_n_seq;
    return r;
}

static int text_ist(int id, const char *soll)
{
    const char *t = re15_msg_get_text(id);
    printf("    msg %d Text \"%s\"\n", id, t ? t : "(keiner)");
    return t && strcmp(t, soll) == 0;
}

static void szene_pruefen(const szene_t *r)
{
    printf("  Druck: Ereignis %d, Faden %d | msg 22/23/24 ab B%d/B%d/B%d | Schritt B%d..B%d Weg %.0f dz %d "
           "Gierung %d | Clip19 B%d, Clip19 rueckw. B%d, Clip17 B%d | Ende B%d | Balken voll B%d weg B%d\n",
           r->ereignis, r->gestartet, r->msg_bild[0], r->msg_bild[1], r->msg_bild[2], r->schritt_start,
           r->ankunft, r->weg, r->dz, r->gierung_ende, r->clip19_vor, r->clip19_rueck, r->clip17, r->ende,
           r->lb_voll, r->lb_weg);
    PRUEF(r->ereignis == RE15_ADARUF_EREIGNIS && r->gestartet == 1,
          "Quadrat -> Aktions-Scan meldet Ereignis 13, scd_event_fire startet das Port-Programm");
    PRUEF(r->msg_folge_ok && r->msg_bild[0] > 0 && r->msg_bild[1] > r->msg_bild[0] && r->msg_bild[2] > r->msg_bild[1],
          "Nachrichtenfolge 22 -> 23 -> 24, keine andere");
    PRUEF(r->msg_bild[0] > 0 && r->schritt_start > r->msg_bild[0] + 90,
          "Rueckschritt NACH dem Ruf (Ruf ab B%d, Schritt ab B%d; Sleep 100 dazwischen)", r->msg_bild[0],
          r->schritt_start);
    PRUEF(r->ankunft > 0 && r->ankunft - r->schritt_start <= 12,
          "Rueckschritt endet (%d Bilder, Modus 8 Ankunft < 100 @0x800312fc)", r->ankunft - r->schritt_start);
    PRUEF(r->weg >= 600.0 && r->weg <= 760.0, "Weg %.0f in 600..760 (Soll ~700 wie ROOM1090 sub02)", r->weg);
    PRUEF(abs(r->dz) < 100, "z aendert sich nicht (dz %d) — keine Kamerazone gekreuzt", r->dz);
    PRUEF(r->gierung_ende <= 128 || r->gierung_ende >= 4096 - 128,
          "Blick bleibt zur Tuer (+X): Gierung %d", r->gierung_ende);
    PRUEF(r->msg_bild[1] > 0 && r->clip19_vor >= r->msg_bild[1] && r->clip19_rueck > r->clip19_vor &&
          r->clip19_rueck < r->msg_bild[2],
          "\"Another civilian survivor.\": Clip 19 vor (B%d), dann rueckwaerts (B%d), vor msg 24",
          r->clip19_vor, r->clip19_rueck);
    PRUEF(r->clip17 >= r->msg_bild[2] && r->msg_bild[2] > 0, "\"I have to help her!\": Clip 17 (B%d)", r->clip17);
    PRUEF(r->ende > 0 && r->ende <= 330, "Faden endet (B%d <= 330)", r->ende);
    PRUEF(r->lb_voll > 0 && r->lb_voll < r->msg_bild[0] && r->lb_weg > r->ende,
          "Balken wie jede Original-Szene: voll ab B%d, nach dem Ende weg (B%d)", r->lb_voll, r->lb_weg);
    PRUEF(r->pm_frei, "Steuerung danach frei (player_mode 0)");
    PRUEF(!r->raumwechsel && r->seq == 0, "kein Raumwechsel, keine Tuersequenz");
}

/* ------------------------------------------------------------------------------------------------- */
static void grundzustand(void)
{
    re15_game_state_init();
    /* Rolltor offen: im Spiel ist der Suedteil (Tuer 10A0, Umkleide 1000, Hof 1090) NUR durch das Rolltor
     * erreichbar (Dossier §2.3, A_rolltor.md §7); ROOM1050 sub02 @0x0CBA `22 03 79 01`. */
    re15_game_flag_set(3, 121, 1);
}

static void teil_szene(void)
{
    printf("[szene] ROOM1050, (3,0xBB)=0, (9,65)=0\n");
    grundzustand();
    if (room_boot(0x1050, STAND_X, STAND_Z, 0, EINTRITT_CUT_1050, 1, 30) != 0) return;
    const re15_aot_t *a = &g_aot.slots[RE15_ADARUF_SLOT];
    PRUEF(re15_adaruf_zustand() == RE15_ADARUF_SZENE, "Installation: Szene scharf");
    PRUEF(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == RE15_ADARUF_EREIGNIS &&
          a->sce_flags == RE15_ADARUF_FLAGS,
          "Slot 4 = Ereignis-Platz (Typ %d, Ereignis %d, flags 0x%02x)", a->type, a->event_id, a->sce_flags);
    /* Rechteck der Tuer @0x00B5A: (16700,-14700,1000,2000) -> Mitte (17200,-13700), halb (500,1000). */
    PRUEF(!a->has_quad && a->x == 17200 && a->z == -13700 && a->half_w == 500 && a->half_h == 1000 && a->band == 0,
          "Rechteck/Band der Tuer (%d,%d) halb (%d,%d) Band %d", (int)a->x, (int)a->z, (int)a->half_w,
          (int)a->half_h, a->band);
    /* Nachrichten 22..25: Rohbytes = Modul, Texte woertlich. */
    int ok_roh = 1;
    for (int id = RE15_ADARUF_MSG_RUF; id <= RE15_ADARUF_MSG_SPERRE; id++) {
        int n = 0, rn = 0;
        const uint8_t *m = re15_adaruf_meldung(id, &n);
        const unsigned char *raw = re15_msg_get_raw(id, &rn);
        if (!m || !raw || rn != n || memcmp(raw, m, (size_t)n) != 0) ok_roh = 0;
    }
    PRUEF(ok_roh, "Nachrichten 22..25 eingesetzt (Rohbytes = adaruf_1050.c)");
    PRUEF(text_ist(RE15_ADARUF_MSG_SPERRE, "I have to help the Survivor first!"), "msg 25 woertlich");
    PRUEF(re15_msg_is_choice(RE15_ADARUF_MSG_RUF) == 0 && re15_msg_is_choice(RE15_ADARUF_MSG_SPERRE) == 0,
          "keine Ja/Nein-Abfrage");
    {   /* Raum-Nachrichten unveraendert: msg 2 = Spur As Original-Satz (ROOM1050 @0x0ED2). */
        const char *t2 = re15_msg_get_text(2);
        PRUEF(t2 && strstr(t2, "I need a fuse to run the shutter.") != NULL, "Raum-Nachricht 2 unveraendert");
    }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = STAND_X; pl->z = STAND_Z; pl->rot_y = 0;
    frame(0, 0);
    PRUEF(pl->x == STAND_X && pl->z == STAND_Z, "Standplatz (%d,%d) Blick 0 haelt", (int)pl->x, (int)pl->z);
    szene_t r = szene_fahren(0, 0);
    szene_pruefen(&r);
    PRUEF(re15_game_flag_get(RE15_ADARUF_GESEHEN_BANK, RE15_ADARUF_GESEHEN_BIT) == 1 &&
          re15_game_flag_get(2, 7) == 0 && re15_game_flag_get(1, 27) == 0 &&
          re15_game_flag_get(RE15_ADARUF_FREI_BANK, RE15_ADARUF_FREI_BIT) == 0,
          "Flags danach: (9,65)=1, (2,7)=0, (1,27)=0, (3,0xBB) unberuehrt");
    PRUEF(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_ADARUF_MSG_SPERRE &&
          a->pause_mask16 == 0xffff && a->x == 17200 && a->z == -13700,
          "Slot 4 danach = Text-Platz msg %d Maske 0x%04x (Aot_reset +0x82)", a->event_id, a->pause_mask16);
    /* Folgedruck im selben Raum -> Sperrtext, kein Wechsel */
    druck_t d = drueck_vor(RE15_ADARUF_SLOT);
    PRUEF(d.stand && d.msg_id == RE15_ADARUF_MSG_SPERRE && d.freeze && !d.raumwechsel && d.seq == 0,
          "Folgedruck: msg %d, Freeze %d, kein Raumwechsel, keine Tuersequenz", d.msg_id, d.freeze);
}

static void teil_doppel(void)
{
    printf("[doppel] Quadrat alle 10 Bilder waehrend der Szene + direkter zweiter scd_event_fire(13)\n");
    grundzustand();
    if (room_boot(0x1050, STAND_X, STAND_Z, 0, EINTRITT_CUT_1050, 1, 30) != 0) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = STAND_X; pl->z = STAND_Z; pl->rot_y = 0;
    frame(0, 0);
    szene_t r = szene_fahren(1, 1);
    printf("  Faeden im Programm max %d, Ziel-Operanden geaendert %d, Ende B%d, Rueckschritt B%d..B%d\n",
           r.max_faeden, r.ziel_geaendert, r.ende, r.schritt_start, r.ankunft);
    PRUEF(r.gestartet == 1 && r.max_faeden == 1, "genau EIN Faden (max %d)", r.max_faeden);
    PRUEF(!r.ziel_geaendert, "Ziel-Operanden +0x28/+0x3C unveraendert");
    PRUEF(r.ende > 0 && r.ende <= 330 && !r.raumwechsel && r.seq == 0,
          "Szene endet normal (B%d), kein Raumwechsel, keine Tuersequenz", r.ende);
    PRUEF(g_aot.slots[RE15_ADARUF_SLOT].type == RE15_AOT_TYPE_MESSAGE, "Slot 4 danach Text-Platz");
}

static void teil_sperre(void)
{
    printf("[sperre] Szene gesehen, echter Raumwechsel 1050 -> 1000 -> 1050\n");
    grundzustand();
    if (room_boot(0x1050, STAND_X, STAND_Z, 0, EINTRITT_CUT_1050, 1, 30) != 0) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = STAND_X; pl->z = STAND_Z; pl->rot_y = 0;
    frame(0, 0);
    szene_t r = szene_fahren(0, 0);
    PRUEF(r.ende > 0 && re15_game_flag_get(RE15_ADARUF_GESEHEN_BANK, RE15_ADARUF_GESEHEN_BIT) == 1,
          "Szene gelaufen, (9,65)=1");
    /* ROOM1050 Slot 3 @0x00B3A -> ROOM1000 (Umkleide): Spawn (21850,0,-13400), dann zurueck durch
     * ROOM1000 Slot 0 @0x00BBE -> ROOM1050 (Cut 4). Hier als zwei Raumaufbauten (Flags bleiben). */
    if (room_boot(0x1000, 21850, -13400, 2048, 0, 1, 30) != 0) return;
    PRUEF(re15_adaruf_zustand() == RE15_ADARUF_AUS, "ROOM1000: nichts umgewidmet");
    if (room_boot(0x1050, 14700, -13500, 0, EINTRITT_CUT_1050, 1, 30) != 0) return;
    const re15_aot_t *a = &g_aot.slots[RE15_ADARUF_SLOT];
    PRUEF(re15_adaruf_zustand() == RE15_ADARUF_SPERRE, "Wiederbetreten: Sperre");
    PRUEF(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_ADARUF_MSG_SPERRE &&
          a->pause_mask16 == 0xffff && a->sce_flags == RE15_ADARUF_FLAGS && a->band == 0,
          "Slot 4 = Text-Platz msg %d Maske 0x%04x flags 0x%02x", a->event_id, a->pause_mask16, a->sce_flags);
    PRUEF(text_ist(RE15_ADARUF_MSG_SPERRE, "I have to help the Survivor first!"), "Sperrtext woertlich");
    druck_t d = drueck_vor(RE15_ADARUF_SLOT);
    printf("  Druck: Standplatz %d, Nachricht %d, Freeze %d, Raumwechsel %d, Sequenzen %d\n",
           d.stand, d.msg_id, d.freeze, d.raumwechsel, d.seq);
    PRUEF(d.stand && d.msg_id == RE15_ADARUF_MSG_SPERRE, "Quadrat oeffnet Nachricht 25");
    PRUEF(d.freeze, "Text-Freeze 0xffff0000 wie jeder sce-1-Text (@0x80043098)");
    PRUEF(!d.raumwechsel && d.seq == 0 && g_current_room_id == 0x1050,
          "kein Raumwechsel in 150 Bildern, keine Tuersequenz");
    PRUEF(faeden_im_programm(NULL) == 0, "keine zweite Szene");
    druck_t d2 = drueck_vor(RE15_ADARUF_SLOT);
    PRUEF(d2.msg_id == RE15_ADARUF_MSG_SPERRE && !d2.raumwechsel && d2.seq == 0, "zweiter Druck ebenso");
}

static void frei_fall(int b65)
{
    grundzustand();
    re15_game_flag_set(RE15_ADARUF_FREI_BANK, RE15_ADARUF_FREI_BIT, 1);
    if (b65) re15_game_flag_set(RE15_ADARUF_GESEHEN_BANK, RE15_ADARUF_GESEHEN_BIT, 1);
    if (room_boot(0x1050, STAND_X, STAND_Z, 0, EINTRITT_CUT_1050, 1, 30) != 0) return;
    const re15_aot_t *a = &g_aot.slots[RE15_ADARUF_SLOT];
    const re15_aot_door_params_t *dp = &g_aot.door_params[RE15_ADARUF_SLOT];
    PRUEF(re15_adaruf_zustand() == RE15_ADARUF_AUS, "keine Umwidmung ((9,65)=%d)", b65);
    PRUEF(a->active && a->type == RE15_AOT_TYPE_DOOR && dp->dest_room == 0x0A,
          "Slot 4 = Original-Tuer (Typ %d, Ziel 0x%02x @0x00B71)", a->type, dp->dest_room);
    druck_t d = drueck_vor(RE15_ADARUF_SLOT);
    printf("  Druck: Standplatz %d, Nachricht %d, Raumwechsel %d -> ROOM%04X, Sequenzen %d (S%03u DOOR%02X V%d)\n",
           d.stand, d.msg_id, d.raumwechsel, (unsigned)d.ziel, d.seq, s_seq.seite, s_seq.re2_nr, s_seq.variante);
    PRUEF(d.stand && d.raumwechsel && d.ziel == 0x10A0 && d.msg_id < 0, "Quadrat -> Raumwechsel nach ROOM10A0");
    PRUEF(d.seq == 1 && s_seq.seite == 21 && s_seq.re2_nr == 0x07 && s_seq.variante == 0,
          "genau eine Tuersequenz S021 DOOR07 V0 (gen/re15_tuer_eigen.inc ROOM1050@0xB5A)");
    PRUEF(faeden_im_programm(NULL) == 0, "keine Szene");
    g_room_change.pending = 0;
}

static void teil_frei(void)
{
    printf("[frei] ROOM1050, (3,0xBB)=1, (9,65)=0\n");
    frei_fall(0);
    printf("[frei] ROOM1050, (3,0xBB)=1, (9,65)=1\n");
    frei_fall(1);
}

static void teil_rettung(void)
{
    printf("[rettung] ROOM1090 sub03 setzt (3,0xBB) selbst, danach ROOM1050 zweimal\n");
    grundzustand();
    /* Vorbedingungen der Rettung (Zensus §3.2): Loescher (3,133), Feuer-Szene (3,128), Feuer geloescht
     * (3,129) und das Rettungs-Flag (3,132) aus sub06 @0x02722 -> ROOM1090 sub00 @0x022A6 Ck(3,132)==1 ->
     * Sce_em_set Ada + Evt_exec sub03 @0x022E0. */
    re15_game_flag_set(3, 133, 1); re15_game_flag_set(3, 128, 1);
    re15_game_flag_set(3, 129, 1); re15_game_flag_set(3, 132, 1);
    PRUEF(re15_game_flag_get(RE15_ADARUF_FREI_BANK, RE15_ADARUF_FREI_BIT) == 0, "vorher (3,0xBB)=0");
    if (room_boot(0x1090, -130, -1988, 0, 0, 0, 5) != 0) return;
    PRUEF(re15_game_flag_get(RE15_ADARUF_FREI_BANK, RE15_ADARUF_FREI_BIT) == 1 &&
          re15_game_flag_get(3, 110) == 1 && re15_game_flag_get(3, 132) == 0,
          "ROOM1090 sub03 im Port-VM: (3,0xBB)=1 (@0x024D2), (3,0x6E)=1 (@0x024D6), (3,132)=0 (@0x024CE)");
    /* Mit Ada durch ROOM1090 Slot 0 (@0x0211A, Ziel ROOM1050) — ROOM1050 sub00 @0x00C8A Ck(3,110)==1 ->
     * sub03 "Hey, wait!" loescht (3,110) als erstes Opcode @0x00D88. */
    if (room_boot(0x1050, 19850, -22300, 3072, 6, 1, 30) != 0) return;
    PRUEF(re15_game_flag_get(3, 110) == 0, "ROOM1050 sub03 hat (3,0x6E) geloescht (@0x00D88) — die Falle ist echt");
    PRUEF(re15_adaruf_zustand() == RE15_ADARUF_AUS && g_aot.slots[RE15_ADARUF_SLOT].type == RE15_AOT_TYPE_DOOR,
          "erstes Betreten nach der Rettung: Slot 4 = Tuer");
    if (room_boot(0x1000, 21850, -13400, 2048, 0, 1, 30) != 0) return;
    if (room_boot(0x1050, 14700, -13500, 0, EINTRITT_CUT_1050, 1, 30) != 0) return;
    PRUEF(re15_adaruf_zustand() == RE15_ADARUF_AUS && g_aot.slots[RE15_ADARUF_SLOT].type == RE15_AOT_TYPE_DOOR,
          "Wiederbetreten ((3,0x6E)=0): Slot 4 bleibt Tuer — Freigabe ueber (3,0xBB)");
    druck_t d = drueck_vor(RE15_ADARUF_SLOT);
    PRUEF(d.stand && d.raumwechsel && d.ziel == 0x10A0 && d.seq == 1, "Quadrat -> ROOM10A0 mit Tuersequenz");
    g_room_change.pending = 0;
}

static void teil_elza(void)
{
    printf("[elza] ROOM1051, (3,0xBB)=0\n");
    grundzustand();
    if (room_boot(0x1051, STAND_X, STAND_Z, 0, EINTRITT_CUT_1050, 1, 30) != 0) return;
    const re15_aot_t *a = &g_aot.slots[RE15_ADARUF_SLOT];
    const re15_aot_door_params_t *dp = &g_aot.door_params[RE15_ADARUF_SLOT];
    PRUEF(re15_adaruf_zustand() == RE15_ADARUF_AUS, "keine Sperre fuer Elza");
    PRUEF(a->active && a->type == RE15_AOT_TYPE_DOOR && dp->dest_room == 0x0A,
          "Slot 4 bleibt Tuer (Typ %d, Ziel 0x%02x)", a->type, dp->dest_room);
    druck_t d = drueck_vor(RE15_ADARUF_SLOT);
    printf("  Druck: Standplatz %d, Nachricht %d, Raumwechsel %d -> ROOM%04X\n",
           d.stand, d.msg_id, d.raumwechsel, (unsigned)d.ziel);
    PRUEF(d.stand && d.raumwechsel && d.ziel == 0x10A1 && d.msg_id < 0, "Quadrat -> ROOM10A1");
    g_room_change.pending = 0;
}

/* Auflage 6: Raster ueber alle begehbaren Druckstellen. */
static int vorwaerts_im_tuerrechteck(int32_t px, int32_t pz, int yaw)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    int32_t fx = px + (int32_t)((620 * c) >> 12);
    int32_t fz = pz - (int32_t)((620 * s) >> 12);
    return fx >= 16700 && fx <= 17700 && fz >= -14700 && fz <= -12700;   /* @0x00B5A */
}

static void teil_raster(void)
{
    printf("[raster] Druckstellen 100 x 100, Gierung je 256, nur begehbar\n");
    grundzustand();
    re15_game_state_t basis = g_game;
    /* Begehbarkeit je Rasterzeile MESSEN: von x=15700 mit Pad UP nach Osten laufen (60 Bilder), die
     * Endposition ist die Ostgrenze des Flurs in dieser Zeile (Wand + Koerperradius). */
    int32_t ost[64]; int nz = 0;
    for (int32_t z = -15700; z <= -11700 && nz < 64; z += 100, nz++) {
        g_game = basis;
        if (room_boot(0x1050, 15700, z, 0, EINTRITT_CUT_1050, 0, 3) != 0) return;
        for (int f = 0; f < 60; f++) frame(RE15_PAD_BIT_UP, 0);
        ost[nz] = g_actors[RE15_ACTOR_SLOT_PLAYER].x;
    }
    printf("  gemessene Ostgrenze: z=-15700 -> %d, z=-15000 -> %d, z=-13700 -> %d, z=-11700 -> %d\n",
           (int)ost[0], (int)ost[7], (int)ost[20], (int)ost[40]);
    int kandidaten = 0, gueltig = 0, ereignis = 0, haengt = 0, schlecht = 0;
    int max_schritt = 0, max_ende = 0; double min_weg = 1e9, max_weg = 0; int max_dz = 0;
    for (int32_t x = 15700; x <= 17500; x += 100) {
        int iz = 0;
        for (int32_t z = -15700; z <= -11700; z += 100, iz++)
        for (int yaw = 0; yaw < 4096; yaw += 256) {
            if (!vorwaerts_im_tuerrechteck(x, z, yaw)) continue;
            if (x > ost[iz]) continue;                     /* in der Wand: im Spiel unerreichbar */
            kandidaten++;
            g_game = basis;
            if (room_boot(0x1050, x, z, (int16_t)yaw, EINTRITT_CUT_1050, 0, 30) != 0) return;
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            if (labs((long)pl->x - x) > 4 || labs((long)pl->z - z) > 4 || (pl->rot_y & 0xfff) != yaw) continue;
            gueltig++;
            szene_t r = szene_fahren(0, 0);
            if (r.gestartet && r.ereignis == RE15_ADARUF_EREIGNIS) ereignis++;
            else { printf("  KEIN EREIGNIS  (%d,%d) Gierung %d\n", (int)x, (int)z, yaw); schlecht++; continue; }
            if (r.ende < 0) { haengt++; printf("  HAENGT  (%d,%d) Gierung %d\n", (int)x, (int)z, yaw); continue; }
            int schritt = r.ankunft - r.schritt_start;
            if (schritt > max_schritt) max_schritt = schritt;
            if (r.ende > max_ende) max_ende = r.ende;
            if (r.weg < min_weg) min_weg = r.weg;
            if (r.weg > max_weg) max_weg = r.weg;
            if (abs(r.dz) > max_dz) max_dz = abs(r.dz);
            if (schritt > 12 || r.weg < 600.0 || r.weg > 760.0 || abs(r.dz) >= 100 || r.ende > 330) {
                schlecht++;
                printf("  AUSSERHALB  (%d,%d) Gierung %d: Schritt %d Bilder, Weg %.0f, dz %d, Ende B%d\n",
                       (int)x, (int)z, yaw, schritt, r.weg, r.dz, r.ende);
            }
        }
    }
    printf("  Kandidaten %d, gueltig %d, Ereignis 13 %d, haengt %d, ausserhalb %d | Schritt max %d Bilder, "
           "Weg %.0f..%.0f, |dz| max %d, Szene max %d Bilder\n", kandidaten, gueltig, ereignis, haengt, schlecht,
           max_schritt, min_weg, max_weg, max_dz, max_ende);
    PRUEF(gueltig >= 100, "genug begehbare Druckstellen (%d)", gueltig);
    PRUEF(ereignis == gueltig && haengt == 0 && schlecht == 0,
          "ueberall Ereignis 13, Faden endet, Schritt <= 12, Weg 600..760, |dz| < 100, Ende <= 330");
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    re15_door_seq_setze_laeufer(fang);
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "szene"))   teil_szene();
    if (alle || !strcmp(teil, "doppel"))  teil_doppel();
    if (alle || !strcmp(teil, "sperre"))  teil_sperre();
    if (alle || !strcmp(teil, "frei"))    teil_frei();
    if (alle || !strcmp(teil, "rettung")) teil_rettung();
    if (alle || !strcmp(teil, "elza"))    teil_elza();
    if (alle || !strcmp(teil, "raster"))  teil_raster();
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
