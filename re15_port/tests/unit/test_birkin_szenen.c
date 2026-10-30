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
    /* 1. ORIGINAL -> VIERBEIN: toedlicher Treffer vor der Mutation fuehrt in die Mutation (sub 6, Clip 0x14) */
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
    CHECK(re15_bf_emd_datei(0x30u) && strstr(re15_bf_emd_datei(0x30u), "BAHNSTEIG"), "nach Mutation kein Vierbein-Modell");
    fprintf(stderr, "Bahnsteig: Mutation Original->Vierbein nach %d Ticks\n", m);
    CHECK(bank_laden(0x30u, 0, re15_bf_emd_datei(0x30u)), "EM30_BAHNSTEIG-Bank");
    /* Kampfposition wie nach sub04 (Pos_set(1200,0,-23350) @0x12FE), Spieler an der Kampfmarke (300,-23400) */
    b->x = 1200; b->z = -23350; b->grid_id = 0x13;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 300; pl->z = -23400; pl->hp = 100;
    b->state = 3; b->sub_state_3 = 0; b->hp = 0; b->birkin_flags = 0;
    int f, gefeuert = 0, versteckt = 0;
    int32_t x_min = b->x;
    for (f = 0; f < 900 && b->type == 0x30u; f++) {
        re15_enemy_ai_run_all(0);
        if (re15_bf_rueckstoss_phase() == 1) {
            uint16_t pr = 0, pad = re15_bf_pad(0, &pr);
            if ((pad & 0x0800u) && (pad & 0x8000u)) gefeuert = 1;
        }
        if (b->no_draw) versteckt = 1;
        if (b->x < x_min) x_min = b->x;
    }
    CHECK(gefeuert, "Spieler zielt/feuert in der Rueckstoss-Szene nicht");
    CHECK(x_min < -5683, "Birkin verlaesst den Zug nicht nach Westen (min x %d)", x_min);
    CHECK(versteckt, "Birkin verschwindet hinter dem Zugende nicht");
    CHECK(b->type == 0x36u, "kein Morph zum G5 nach %d Ticks (state %d ph3 %d)", f, b->state, b->sub_state_3);
    CHECK(b->grid_id == 0x13u, "G5 nicht armiert (grid 0x%02X)", b->grid_id);
    CHECK(!b->no_draw, "G5 bleibt unsichtbar");
    CHECK(g_scd.cam_id == 0x0B, "G5-Auftritt nicht auf Cut 11 (Zugende), cam %d", g_scd.cam_id);
    CHECK(re15_game_flag_get(5, 0x1c) == 0, "Birkin-Schalter z5:0x1C schon beim Vierbein-Tod gesetzt");
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

int main(void)
{
    test_modellwahl();
    test_hangar();
    test_bahnsteig();
    if (fails) { fprintf(stderr, "test_birkin_szenen: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_birkin_szenen: OK\n");
    return 0;
}
