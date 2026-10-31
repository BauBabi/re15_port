/* test_birkin_szenen.c — Raumprobe der Beta-Birkin-Auftritte (include/re15_birkin_formen.h).
 *
 * 1. HANGAR ROOM50A0: Raumladen setzt Birkin (0x30) + Spider-Man (0x2B); sobald Cut 12 angezeigt wird,
 *    laeuft die Wurfszene Sprung -> Griff -> Sturz -> liegt -> Birkin frei. Geprueft: der Spider-Man haengt
 *    im Griff an Birkins Klaue (Knochen 15), wird erst ab Bild 0x2C losgelassen, liegt am Ende am Boden,
 *    und Birkin geht danach in seine KI (Zustand 0 -> 1).
 * 2. BAHNSTEIG ROOM5090: der Auslieferungsspawn sub00 @0x0124A bleibt Typ 0x30 (keine Umtypung mehr);
 *    nach Todesphase 2 wird er zum G5 (0x36, grid 0x13), ohne den Birkin-Schalter z5:0x1C zu setzen.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_room.h"
#include "re15_birkin_formen.h"
#include "re2_ems.h"

extern int re15_g5_boss_zustand(int *sub, int *ph, int *timer);
extern int re15_g5_blut_zaehler(int idx);
extern int re15_g5_biss_blut_zahl(void);
extern void re15_g5_boss_spawn(int slot);

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}

static uint8_t s_ems_buf[2][0x80000];
static int bank_laden(uint8_t type, int idx, const char *ersatz)
{
    size_t len = 0;
    uint8_t *src = NULL;
    if (ersatz) {
        char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", ersatz);
        src = slurp(p, &len);
    } else {
        size_t esz = 0, off = 0;
        uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
        if (ems && re15_ems_get_entry(ems, esz, re15_ems_index_for_type(type), &off, &len) == 0) {
            src = (uint8_t *)malloc(len); memcpy(src, ems + off, len);
        }
        free(ems);
    }
    if (!src || len > sizeof s_ems_buf[idx]) { free(src); return 0; }
    memcpy(s_ems_buf[idx], src, len); free(src);
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re15_emd_parse_container(s_ems_buf[idx], len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return 1;
}

/* Bank der Bahnsteig-Form wie pc_enemy_load_bahnsteig (RE2-Format, Opfer-Bank EM30). */
static uint8_t *s_form_emd = NULL;
static int bank_form_laden(void)
{
    size_t len = 0;
    if (!s_form_emd) s_form_emd = slurp(RE15_ASSET_PSX_DIR "/../RE2/BAHNSTEIG_G4.EMD", &len);
    else return 0;
    if (!s_form_emd) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    if (!eb) return 0;
    uint8_t typ = eb->type;
    memset(eb, 0, sizeof *eb); eb->type = typ;
    if (re2_emd_parse_bank(s_form_emd, len, eb) != 0) return 0;
    eb->ok = 1; eb->re2_rig = 1; eb->buf = NULL;
    return 1;
}

static re15_rdt_t s_rdt;
static uint8_t *raum_laden(const char *rel, unsigned id)
{
    size_t sz = 0;
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rel);
    uint8_t *buf = slurp(p, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) return NULL;
    extern void scd_register_current_rdt(const re15_rdt_t *);
    scd_register_current_rdt(NULL);                      /* Vorraum abmelden (sonst SUB01-RESEED ab Tick 1) */
    g_current_room_id = id;
    re15_enemy_reset();
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    /* RDT erst NACH dem Spawn registrieren: sonst setzt scd_vm_tick Slot 1 jeden Tick auf sub01
     * zurueck (SUB01-RESEED @0x8003f064), bevor sub00 laeuft - im Spiel laeuft sub00 im Raum-Init. */
    for (int i = 0; i < 60; i++) scd_vm_tick();
    scd_register_current_rdt(&s_rdt);
    return buf;
}

static void test_hangar(void)
{
    uint8_t *buf = raum_laden("STAGE5/ROOM50A0.RDT", 0x50A0u);
    CHECK(buf != NULL, "ROOM50A0 nicht ladbar");
    if (!buf) return;
    CHECK(bank_laden(0x30u, 0, re15_bf_emd_datei(0x30u)), "EM30_HANGAR-Bank");
    CHECK(re15_bf_emd_datei(0x30u) && strstr(re15_bf_emd_datei(0x30u), "HANGAR"), "Modellwahl 50A0 != HANGAR");
    CHECK(bank_laden(0x2Bu, 1, NULL), "EM2B-Bank");
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -14000; pl->z = -26000; pl->hp = 100;
    g_scd.work_vars[0x0A] = 0;                            /* anderer Cut: Szene wartet */
    for (int f = 0; f < 5; f++) re15_enemy_ai_run_all(0);
    int bs = re15_bf_hangar_slot(1), ms = re15_bf_hangar_slot(0);
    CHECK(bs > 0 && g_actors[bs].type == 0x30u, "kein Birkin gesetzt (slot %d)", bs);
    CHECK(ms > 0 && g_actors[ms].type == 0x2Bu, "kein Spider-Man gesetzt (slot %d)", ms);
    CHECK(re15_bf_hangar_phase() == 0, "Szene laeuft ohne Cut 12 (phase %d)", re15_bf_hangar_phase());
    if (bs <= 0 || ms <= 0) { free(buf); return; }
    re15_actor_t *b = &g_actors[bs], *m = &g_actors[ms];

    g_scd.work_vars[0x0A] = 12;
    int max_hoehe = 0, griff_gesehen = 0, frueh_los = 0, ende = -1;
    for (int f = 0; f < 400; f++) {
        int ph_vor = re15_bf_hangar_phase();
        re15_enemy_ai_run_all(0);
        int ph = re15_bf_hangar_phase();
        if (-m->y > max_hoehe) max_hoehe = -m->y;
        if (ph == 2) {
            griff_gesehen = 1;
            int32_t k[3]; re15_enemy_bone_world_pos(b, 15, k);
            CHECK(m->x == k[0] && m->z == k[2], "Griff: Spider-Man nicht an der Klaue (f%d)", f);
        }
        if (ph_vor == 2 && ph == 3 && b->anim_frame < 0x2C) frueh_los = 1;
        if (ph == 5 && ende < 0) ende = f;
    }
    CHECK(griff_gesehen, "Griff-Phase nie erreicht");
    CHECK(!frueh_los, "Spider-Man vor Wurfbild 0x2C losgelassen");
    CHECK(max_hoehe > 2500, "Spider-Man nie hochgestemmt (max %d)", max_hoehe);
    CHECK(ende > 0, "Szene endet nicht (phase %d)", re15_bf_hangar_phase());
    CHECK(m->y == 0 && m->motion == 0x0A, "Spider-Man liegt nicht am Boden (y %d clip %d)", m->y, m->motion);
    CHECK(b->state == 1, "Birkin nicht in der KI (state %d)", b->state);
    CHECK(re15_bf_szene_besitzt(ms) && !re15_bf_szene_besitzt(bs), "Besitz nach Szenenende falsch");
    fprintf(stderr, "Hangar: Birkin slot %d, SM slot %d, Szene Ende Tick %d, SM max Hoehe %d\n",
            bs, ms, ende, max_hoehe);
    free(buf);
}

static void test_bahnsteig(void)
{
    uint8_t *buf = raum_laden("STAGE5/ROOM5090.RDT", 0x5090u);
    CHECK(buf != NULL, "ROOM5090 nicht ladbar");
    if (!buf) return;
    CHECK(re15_bf_emd_datei(0x30u) == NULL, "5090 vor der Mutation muss das Original-EM30 sein");
    static int s_hook_rufe = 0;
    re15_bf_modell_hook(NULL);
    int bs = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && (g_actors[s].type == 0x30u || g_actors[s].type == 0x36u)) bs = s;
    CHECK(bs > 0 && g_actors[bs].type == 0x30u, "5090-Spawn nicht 0x30 (slot %d type 0x%02X)",
          bs, bs > 0 ? g_actors[bs].type : 0);
    if (bs <= 0) { free(buf); return; }
    CHECK(bank_laden(0x30u, 0, NULL), "EM30-Original-Bank");
    re15_actor_t *b = &g_actors[bs];
    re15_game_flag_set(5, 0x1c, 0);
    (void)s_hook_rufe;
    /* 1. ORIGINAL -> BAHNSTEIG-FORM: toedlicher Treffer vor der Mutation fuehrt in die Mutation (sub 6, Clip 0x14) */
    b->x = 1200; b->z = -23350; b->grid_id = 0x13;
    b->state = 3; b->sub_state_3 = 0; b->hp = 0; b->birkin_flags = 0;
    int m, mut_clip = 0;
    for (m = 0; m < 600 && re15_bf_mutation_offen(); m++) {
        re15_enemy_ai_run_all(0);
        if (b->sub_state_1 == 6 && b->motion == 0x14) mut_clip = 1;
    }
    CHECK(mut_clip, "Mutations-Clip 0x14 nicht gespielt");
    CHECK(!re15_bf_mutation_offen(), "Mutation endet nicht (%d Ticks)", m);
    CHECK(b->hp == 150 && b->type == 0x30u, "nach Mutation hp %d type 0x%02X", b->hp, b->type);
    CHECK(re15_bf_emd_datei(0x30u) == NULL && re15_bf_bahnsteig_bank_faellig(0x30u), "nach Mutation keine Bahnsteig-Form");
    fprintf(stderr, "Bahnsteig: Mutation Original->Form nach %d Ticks\n", m);
    CHECK(bank_form_laden(), "Bank der Bahnsteig-Form");
    /* Kampfposition wie nach sub04 (Pos_set(1200,0,-23350) @0x12FE), Spieler an der Kampfmarke (300,-23400) */
    b->x = 1200; b->z = -23350; b->grid_id = 0x13;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 300; pl->z = -23400; pl->hp = 100;
    re15_enemy_ai_run_all(0);                            /* erster Formen-Tick: Ctor-Teil, eigene HP 150 */
    CHECK(b->hp == 150, "Form kommt mit HP %d statt 150", b->hp);
    b->state = 3; b->sub_state_3 = 0; b->hp = 0; b->birkin_flags = 0;
    int f, gefeuert = 0, versteckt = 0, zonen_an = 0;
    int32_t x_min = b->x;
    g_scd.cut_auto_enabled = 1;                          /* ROOM5090 sub04 Cut_auto 01 @0x134C */
    for (f = 0; f < 900 && b->type == 0x30u; f++) {
        re15_enemy_ai_run_all(0);
        if (re15_bf_rueckstoss_phase() != 0 && g_scd.cut_auto_enabled) zonen_an++;
        if (re15_bf_rueckstoss_phase() == 1) {
            uint16_t pr = 0, pad = re15_bf_pad(0, &pr);
            if ((pad & 0x0800u) && (pad & 0x8000u)) gefeuert = 1;
        }
        if (b->no_draw) versteckt = 1;
        if (b->x < x_min) x_min = b->x;
    }
    CHECK(zonen_an == 0, "M9: Kamerazonen in %d Szenen-Bildern an", zonen_an);
    CHECK(gefeuert, "Spieler zielt/feuert in der Rueckstoss-Szene nicht");
    CHECK(x_min < -5683, "Birkin verlaesst den Zug nicht nach Westen (min x %d)", x_min);
    CHECK(versteckt, "Birkin verschwindet hinter dem Zugende nicht");
    CHECK(b->type == 0x36u, "kein Morph zum G5 nach %d Ticks (state %d ph3 %d)", f, b->state, b->sub_state_3);
    CHECK(b->grid_id == 0x13u, "G5 nicht armiert (grid 0x%02X)", b->grid_id);
    CHECK(!b->no_draw, "G5 bleibt unsichtbar");
    CHECK(g_scd.cam_id == 0x0B, "G5-Auftritt nicht auf Cut 11 (Zugende), cam %d", g_scd.cam_id);
    CHECK(re15_game_flag_get(5, 0x1c) == 0, "Birkin-Schalter z5:0x1C schon beim Tod der Form gesetzt");
    /* M9 + Abnahme 2: bis zum Ende von [T2] haelt die Szene Cut 11 mit Zonen aus, danach Zonen an, Schnitt aus der Spielerlage.
     * Leon steht weit im Osten: ein fester Cut 12 liesse ihn ausser Bild und ohne Zone. */
    {
        int zonen_im_auftritt = 0, frei = -1, sub = 2, ph = 0, n, sub_bei_frei = -1;
        for (n = 0; n < 1500 && frei < 0; n++) {
            pl->x = 20748; pl->z = -26416; pl->hp = 100; pl->hit_react = 0;
            re15_enemy_ai_run_all(0);
            if (re15_g5_boss_zustand(&sub, &ph, NULL) && sub == 2 && ph < 3 && g_scd.cut_auto_enabled) zonen_im_auftritt++;
            if (g_scd.cut_auto_enabled) { frei = n; sub_bei_frei = sub; }
        }
        CHECK(zonen_im_auftritt == 0, "M9: Kamerazonen in %d Auftritts-Bildern an", zonen_im_auftritt);
        CHECK(frei >= 0, "M9: Kamerazonen nach dem Hereinziehen nicht wieder an (%d Ticks, Sub %d Ph %d)", n, sub, ph);
        CHECK(sub_bei_frei == 2, "Szenenkamera endet erst mit Sub 2 (Freigabe in Sub %d), sperrt 37,5 s", sub_bei_frei);
        CHECK(g_scd.cam_id == 0x0E, "Leon bei x 20748: Cut %d statt 14 nach dem Szenenende", g_scd.cam_id);
        fprintf(stderr, "Bahnsteig: Kamerazonen %d Ticks nach dem Morph wieder an, Cut %d\n", frei, g_scd.cam_id);
    }
    fprintf(stderr, "Bahnsteig: Rueckstoss + Morph nach %d Ticks (min x %d)\n", f, x_min);
    free(buf);
}

static void test_modellwahl(void)
{
    g_current_room_id = 0x4080u;
    CHECK(re15_bf_emd_datei(0x29u) && strstr(re15_bf_emd_datei(0x29u), "EM29_ROT"), "4080: kein roter Affe");
    g_current_room_id = 0x4081u;
    CHECK(re15_bf_emd_datei(0x29u) != NULL, "4081: kein roter Affe");
    g_current_room_id = 0x50A0u;
    CHECK(re15_bf_emd_datei(0x29u) == NULL, "50A0: Affe darf Auslieferung bleiben");
    g_current_room_id = 0x3070u;
    CHECK(re15_bf_emd_datei(0x30u) == NULL, "3070: Birkin muss EM30 bleiben");
    CHECK(!re15_bf_bahnsteig_aktiv() && !re15_bf_hangar_aktiv(), "3070: Formen aktiv");
}

/* Abnahme 2: Schnitt aus der Spielerlage gegen die RVD-Zonen des Raums (Datei @0x490, 20 Byte je Zone, Ende 0xFFFF): kein Streifen des
 * gewaehlten Schnitts darf an dieser Lage sofort weiterschalten, sonst pendelt die Kamera, und aus jeder Lage fuehrt der Schnitt zu Bild. */
static void test_schnitt_aus_lage(const char *rdt)
{
    size_t n = 0;
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rdt);
    uint8_t *b = slurp(p, &n);
    CHECK(b != NULL, "%s nicht ladbar", rdt);
    if (!b) return;
    static const struct { int32_t x; int cut; } soll[] = {
        { -400, 11 }, { 2749, 11 }, { 2750, 12 }, { 13999, 12 }, { 14000, 13 }, { 20249, 13 }, { 20250, 14 }, { 25000, 14 },
    };
    for (unsigned i = 0; i < sizeof soll / sizeof soll[0]; i++)
        CHECK(re15_bf_schnitt_aus_lage(soll[i].x) == soll[i].cut, "x %d: Cut %d statt %d", soll[i].x, re15_bf_schnitt_aus_lage(soll[i].x), soll[i].cut);
    for (int32_t x = -400; x <= 25000; x += 50)
        for (int32_t z = -26400; z <= -22168; z += 400) {
            int c = re15_bf_schnitt_aus_lage(x), treffer = 0;
            for (size_t o = 0x490; o + 20 <= n && !(b[o] == 0xff && b[o + 1] == 0xff); o += 20) {
                int16_t q[8];
                memcpy(q, b + o + 4, sizeof q);
                if (b[o + 2] != c || b[o + 3] == 0) continue;        /* Zone dieses Schnitts, Ziel 0 = Ankerzone ohne Wechsel */
                /* konvexes Viereck: alle Kanten auf derselben Seite */
                {
                    int pos = 0, neg = 0;
                    for (int k = 0; k < 4; k++) {
                        int64_t ax = q[k * 2], az = q[k * 2 + 1], bx = q[(k * 2 + 2) & 7], bz = q[(k * 2 + 3) & 7];
                        int64_t kreuz = (bx - ax) * ((int64_t)z - az) - (bz - az) * ((int64_t)x - ax);
                        if (kreuz > 0) pos++; else if (kreuz < 0) neg++;
                    }
                    if (!(pos && neg)) treffer++;
                }
            }
            CHECK(treffer == 0, "%s: Lage (%d,%d) Cut %d liegt in einem Wechselstreifen", rdt, x, z, c);
        }
    free(b);
}

/* Abnahme 2, G5-Biss: FUN_800401d4(40, 1) @0x80104118 / @0x80104170 toetet nie, klemmt auf HP 0 @0x800402b0. Devour verlangt HP <= 0
 * (`bgtz` @0x801008e0, @0x80103e80). Der Todes-Latch @0x800402d8 startet die Game-Over-FSM mitten im Fressen; der Kau-Zyklus
 * @0x80104b0c-6c senkt das Gewicht, sonst bleibt das Ende-Gate @0x80101d14 zu. */
extern void re15_g5_boss_tick(int slot);
extern int  re15_g5_fress_todeslatch(void);
extern int  re15_death_presentation_active(void);
extern int  re15_g5_blob_zustand(int *blob, int *ph, int32_t *gewicht0);
static void test_g5_fressen(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    CHECK(ems != NULL, "RE2/CDEMD0.EMS fehlt");
    if (!ems) return;
    re15_enemy_reset();
    re15_actor_init();
    g_current_room_id = 0x5090u;
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x36);
    CHECK(eb && re2_ems_load_bank(ems, n, 0x36, eb, NULL) == 0, "EM036 laedt nicht");
    if (!eb) { free(ems); return; }
    eb->buf = NULL; eb->ok = 1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->x = 24000; pl->z = -23400; pl->y = 0;
    re15_actor_t *e = &g_actors[2];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x36; e->flags = 1; e->hp = 600; e->grid_id = 0x13;
    {   /* Auftritt bis zum Kampf */
        int sub = 2, t;
        for (t = 0; t < 1400 && (sub == 2 || t == 0); t++) { re15_g5_boss_tick(2); (void)re15_g5_boss_zustand(&sub, NULL, NULL); }
        CHECK(sub != 2, "G5-Auftritt endet nicht");
    }
    {   /* Spieler stirbt nicht am Biss, nie bei HP > 0 gefressen, HP faellt ueber 0 */
        int biss = 0, null_gesehen = 0, frass_bei = 999, sub = 0, ph = 0, reaktion = 0;
        for (int t = 0; t < 12000 && !re15_g5_fress_todeslatch(); t++) {
            int vor = pl->hp;
            e->x = 10000; e->z = -23400;                      /* u >= 9001 @0x801008a0 */
            pl->x = e->x + 4500; pl->z = e->z;
            if (reaktion > 0) reaktion--; else pl->hit_react = 0;   /* Treffer-Reaktion des Spielers, je Biss ein Treffer */
            e->hp = 600;
            re15_g5_boss_tick(2);
            if (vor - pl->hp == 20) biss++;                   /* 40 @0x80104118, halb auf Port-HP; Tentakel 15 @0x801016d0 zaehlt nicht */
            if (pl->hp < vor) reaktion = 30;
            if (pl->hp == 0) null_gesehen = 1;
            if (pl->hp < 0) { frass_bei = vor; break; }
        }
        (void)re15_g5_boss_zustand(&sub, &ph, NULL);
        CHECK(biss >= 2, "nur %d Bisse vor dem Fressen, Sperre +0x229 laeuft nicht ab (@0x80100188-98)", biss);
        CHECK(null_gesehen, "HP faellt nicht auf 0 (Biss a1 = 1 @0x80104170 klemmt auf 0 @0x800402b0)");
        CHECK(frass_bei == 0, "gefressen bei HP %d statt 0 (`bgtz` @0x801008e0 / @0x80103e80)", frass_bei);
        CHECK(sub == 4, "kein Devour (Sub %d)", sub);
    }
    CHECK(re15_g5_fress_todeslatch(), "Todes-Latch nicht gesetzt (@0x800402d8)");
    CHECK(re15_death_presentation_active(), "Game-Over-FSM startet nicht im Fressen (@0x80015014-30)");
    CHECK(!re15_player_is_dead(), "Opfer-Modus 4 besitzt den Spieler, is_dead muss 0 bleiben");
    {   /* Kauen: Gewicht pendelt, nach 150 Bildern Clip 2 faellt das Ende-Gate */
        int blob = 4, ph = 0, p2 = 0, p3 = 0, ende = -1;
        int32_t g0 = 0, g_max = -99999, g_min = 99999;
        for (int t = 0; t < 2400 && ende < 0; t++) {
            re15_g5_boss_tick(2);
            (void)re15_g5_blob_zustand(&blob, &ph, &g0);
            if (blob == 4 && ph == 2) p2++;
            if (blob == 4 && ph == 3) {
                p3++;
                if (g0 > g_max) g_max = g0;
                if (g0 < g_min) g_min = g0;
            }
            if (blob != 4) ende = t;
        }
        CHECK(p2 > 0 && p3 > 0, "Kau-Zyklus laeuft nicht (Phase 2 %d, Phase 3 %d)", p2, p3);
        CHECK(g_min < 6000 && g_max >= 10001, "Kauen: Gewicht %d..%d, Ende-Gate 6000 @0x80101d14 kann nicht fallen", g_min, g_max);
        CHECK(ende >= 0, "Ende-Gate faellt nie, Fressen laeuft endlos");
        fprintf(stderr, "G5: Fressen Blob zurueck nach %d Ticks, Kauen %d..%d\n", ende, g_min, g_max);
    }
    CHECK(re15_g5_fress_todeslatch(), "Latch faellt mit dem Blob, FSM wuerde zurueckgesetzt (kein Routine-Exit @0x80101d20-24)");
    pl->hp = 100;                                             /* Neues Spiel ueber den Titel: s_g5 steht noch */
    CHECK(!re15_g5_fress_todeslatch(), "Latch ueberlebt den Spielerzustand");
    free(ems);
}

/* Abnahme 3 N5: der G5 blutet. Wache `lbu v0,6(s3)` @0x801025e8 ist die Phase der Treffer-Routine (im ersten Tick 0), nicht die KI-Phase; Blick
 * `slti v0,a1,-64` @0x80102640, Zeilen 16 und 9..12 ohne Blut @0x80102658-64. Biss-Serie @0x801042d8: 3 Spawns je Bild, 10 Bilder (@0x801041a8). */
static void test_g5_blut(void)
{
    size_t ems_n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_n);
    CHECK(ems != NULL, "RE2 CDEMD0.EMS fehlt");
    if (!ems) return;
    re15_enemy_reset(); re15_actor_init();
    {   re15_enemy_bank_t *eb = re15_enemy_alloc(0x36u);
        CHECK(eb && re2_ems_load_bank(ems, ems_n, 0x36u, eb, NULL) == 0, "G5-Bank");
        if (eb) { eb->buf = NULL; eb->ok = 1; }
        eb = re15_enemy_alloc(0x37u);
        CHECK(eb && re2_ems_load_bank(ems, ems_n, 0x37u, eb, NULL) == 0, "Arm-Bank");
        if (eb) { eb->buf = NULL; eb->ok = 1; }
    }
    g_current_room_id = 0x5090u;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *e = &g_actors[2];
    pl->active = 1; pl->hp = 100; pl->x = 14000; pl->z = -23400; pl->rot_y = 2048;   /* Blick nach Westen: dirx = -4096 */
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x36u; e->flags = 1; e->grid_id = 0x13u; e->hp = 600; e->x = 8000; e->z = -23400;
    re15_g5_boss_spawn(2);
    re15_g5_boss_tick(2);
    {   int sub = -1, ph = -1, n = 0;                         /* bis in die Lauf-Phase: KI-Phase != 0 */
        for (; n < 2000; n++) { re15_g5_boss_tick(2); (void)re15_g5_boss_zustand(&sub, &ph, NULL); if (sub <= 1 && ph != 0) break; pl->hp = 100; pl->hit_react = 0; }
        CHECK(n < 2000 && ph != 0, "G5: keine KI-Phase != 0 (Sub %d Phase %d)", sub, ph);
    }
    #define TREFFER(waffe) do { e->hit_react |= 1u; e->sub_state_1 = (waffe); e->re2z_self1d3 = 0; re15_g5_boss_tick(2); e->hit_react = 0; e->re2z_self1d3 = 0; re15_g5_boss_tick(2); } while (0)
    {   int a = re15_g5_blut_zaehler(3), sub = -1, ph = -1;
        (void)re15_g5_boss_zustand(&sub, &ph, NULL);
        TREFFER(3);
        CHECK(re15_g5_blut_zaehler(3) >= a + 2, "N5: Pistole, KI-Phase %d: %d Spawns statt mindestens 2 (@0x801027ec, @0x80102848)", ph, re15_g5_blut_zaehler(3) - a);
        fprintf(stderr, "G5: Pistole in KI-Phase %d: %d Blut-Spawns\n", ph, re15_g5_blut_zaehler(3) - a);
    }
    for (int i = 0; i < 30; i++) { pl->hp = 100; pl->hit_react = 0; re15_g5_boss_tick(2); }
    {   int a = re15_g5_blut_zaehler(3);
        pl->rot_y = 0;                                        /* Blick nach Osten: dirx = +4096, `slti v0,a1,-64` @0x80102640 */
        TREFFER(3);
        CHECK(re15_g5_blut_zaehler(3) == a, "N5: Blick nach Osten, %d Spawns statt 0", re15_g5_blut_zaehler(3) - a);
        pl->rot_y = 2048;
    }
    for (int i = 0; i < 30; i++) { pl->hp = 100; pl->hit_react = 0; re15_g5_boss_tick(2); }
    {   int a = re15_g5_blut_zaehler(3);                      /* Waffe 15 -> Zeile 9 (Tabelle s_g5_zeile_von_waffe) */
        TREFFER(15);
        CHECK(re15_g5_blut_zaehler(3) == a, "N5: Zeile 9 %d Spawns statt 0 (@0x80102660-64)", re15_g5_blut_zaehler(3) - a);
    }
    {   /* a1-Regel FUN_800401d4: a1 = 0 @0x801016d4 Gnade -14..-1 -> HP 0 (`slti v0,v0,-14` @0x80040288), bei HP 0 tot; a1 = 1 toetet nie (`bltz` @0x80040264) */
        extern void re15_g5_tent_treffer_test(re15_actor_t *p, int schaden, int a1);
        pl->hp = 5;  re15_g5_tent_treffer_test(pl, 7, 0);
        CHECK(pl->hp == 0, "Arm a1 = 0 bei HP 5: HP %d statt 0 (Gnade)", pl->hp);
        pl->hp = 0;  re15_g5_tent_treffer_test(pl, 7, 0);
        CHECK(pl->hp < 0, "Arm a1 = 0 bei HP 0: HP %d, muss toeten", pl->hp);
        pl->hp = 0;  re15_g5_tent_treffer_test(pl, 7, 1);
        CHECK(pl->hp == 0, "Arm a1 = 1 bei HP 0: HP %d, toetet nie", pl->hp);
        pl->hp = 3;  re15_g5_tent_treffer_test(pl, 7, 1);
        CHECK(pl->hp == 0, "Arm a1 = 1 bei HP 3: HP %d", pl->hp);
        pl->hp = 100; pl->hit_react = 0;
    }
    {   /* Biss: Lunge trifft den stehenden Spieler vor der Masse, danach 10 Bilder je 3 Spawns */
        int a = re15_g5_biss_blut_zahl(), hp0, gebissen = 0;
        pl->x = e->x + 4500; pl->z = e->z; pl->rot_y = 2048;
        for (int i = 0; i < 2400 && !gebissen; i++) {
            pl->hit_react = 0;
            hp0 = pl->hp = 100;
            pl->x = e->x + 4500; pl->z = e->z;
            re15_g5_boss_tick(2);
            if (pl->hp < hp0) gebissen = 1;
        }
        CHECK(gebissen, "G5 beisst nicht");
        for (int i = 0; i < 40; i++) { pl->hp = 100; re15_g5_boss_tick(2); }
        CHECK(re15_g5_biss_blut_zahl() - a == 30, "N5: Biss-Serie %d Spawns statt 30 (@0x801042d8, +0x21B = 10 @0x801041a8)", re15_g5_biss_blut_zahl() - a);
    }
    free(ems);
}

int main(void)
{
    test_modellwahl();
    test_hangar();
    test_bahnsteig();
    test_schnitt_aus_lage("STAGE5/ROOM5090.RDT");
    test_schnitt_aus_lage("STAGE5/ROOM5091.RDT");
    test_g5_fressen();
    test_g5_blut();
    if (fails) { fprintf(stderr, "test_birkin_szenen: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_birkin_szenen: OK\n");
    return 0;
}
