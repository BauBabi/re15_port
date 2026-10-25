/*
 * test_r41_gatorszene.c — Runde 41 Spur F "gatorszene": Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit allen
 * Installern, SCD-VM, re15_game_step mit dem Boss-Takt des Alligators), Harness-Muster test_r38_dining1141.c.
 *
 * Dossier analysis/befunde_runde41/F_gatorszene.md, Konstanten include/re15_gatorszene_r41.h, Code engine/src/gatorszene_r41.c.
 *
 * Teile (Aufruf: test_r41_gatorszene <teil>):
 *   spinnen     ROOM2090/2091: keine Spinne 0x25; Gegenprobe mit abgeschaltetem Verwerfen: beide RDT-Spinnen da
 *   programme   Opcode-Laengen der drei Programme; P1 Cut 1 + Knall (5,29) + Sleep 45 vor der Tuer; P2 acht Zombies Typ 0x16
 *               Band 8 y -14400 grid 0x0d + Cut 2; P3 Poll Bit 30 + Set(9,110)
 *   leiche      Raumaufbau 2090/2091 vor der Szene: Leiche obj 2 an der Szenen-Lage, Schluessel obj 4 als Kind der Leiche
 *               (Lage + Neigung), Blinken an
 *   texte       Tor-Texte = ROOM20A0.RDT msg 1 @0x203C / msg 2 @0x204C bytegleich; Name "Water Key"
 *   tor         Nachtrag 21: 2090 Slot 1 zu ohne Schluessel (msg 42), mit Schluessel msg 43 + (9,113) + Wegwerf-Abfrage, dann
 *               Durchgang nach 20A0; 20A0 Slot 0 zu (Original-msg 1) bis (9,113), danach Durchgang nach 2090
 *   schluessel  Alligator tot -> (9,111) + Schluessel bei der Leiche (Zone Slot 15); Aufnahme -> Item 0x34 + (9,112); danach
 *               kein Schluessel mehr
 *   szene       die ganze Szene mit dem echten Boss-Takt (RE2-EM23-Bank): Ankunft ueber die Leiter -> Knall -> 2080 (8 Zombies
 *               OBEN, Band 8, nie westlich der Absatzwand) -> zurueck -> Gator-Ablauf G1..G7 -> Kampf, (9,110) = 1, Riegel auf,
 *               Musik-Eintrag 0xFF3C
 *   szene_elza  dasselbe mit Elza (2091 / 2081)
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
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_door_seq.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_item_discard.h"
#include "re15_gameflow.h"
#include "re15_blink_r41.h"
#include "re15_boss_gator.h"
#include "re15_gator_re2_r41.h"
#include "re2_ems.h"
#include "re15_gatorszene_r41.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
#ifndef RE15_ASSET_RE2_DIR
#error RE15_ASSET_RE2_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);
extern unsigned g_re15_gsz_tor_zu, g_re15_gsz_tor_auf;
int re15_gatorszene_r41_bgm_eintrag(int stage, int room);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ---- Harness (Muster test_r38_dining1141.c) --------------------------------------------------------------- */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static long               s_bild = 0;

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
    while (scd_audio_queue_pop(&e)) { }
    s_bild++;
}
static void frame(uint16_t held, uint16_t edge)
{
    if (!re15_discard_frozen()) spielbild(held, edge);
    else s_bild++;
    if (re15_discard_active()) re15_discard_tick((edge & RE15_PAD_BIT_SQUARE) ? (uint16_t)0x4000u : 0, 0);
}

/* RE2-EM23-Bank (Muster test_r41_re2gator.c load_re2_bank): der Lader der PC-Plattform gibt sie dem Boss im Spiel. */
static int gator_bank(void)
{
    static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
    if (!s_ems) s_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_ems_sz);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x23u);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(0x23u);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, s_ems_sz, 0x23, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}
/* Der Alligator wie im Spiel (platform/pc/main.c ALLIGATOR-POOL: Slot rueckwaerts, (-5300,0,-16300), grid 0, INIT, 0xFF). */
static re15_actor_t *gator_setzen(void)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == 0x23u) return &g_actors[i];
    re15_actor_t *a = NULL;
    for (int i = RE15_ACTOR_MAX - 1; i >= 1; i--) if (!g_actors[i].active) { a = &g_actors[i]; break; }
    if (!a) return NULL;
    memset(a, 0, sizeof *a);
    a->active = 1; a->type = 0x23u;
    a->x = -5300; a->y = 0; a->z = -16300;
    a->grid_id = 0; a->state = 0; a->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(a, 0x23u);
    return a;
}

/* Raumaufbau wie der Tuer-Weg (room_common.c: scd_room_reenter mit allen Installern); Flags bleiben. */
static int room_boot(uint16_t room, int32_t px, int32_t py, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_discard_reset();
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_room_rdt_ok = 1;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = py;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band((uint8_t)(py < 0 ? (-py) / 1800 : 0));
    s_shown = cut; s_cine_was = 0; s_bild = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

typedef struct { int msg_id, raumwechsel, ziel; } druck_t;
static druck_t druecke(int32_t x, int32_t z, int16_t rot, int bilder)
{
    druck_t r = { -1, 0, -1 };
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = x; pl->z = z; pl->rot_y = rot;
    frame(0, 0);
    g_room_change.pending = 0;
    for (int f = 0; f < bilder; f++) {
        uint16_t e = 0, h = 0;
        if (f == 0) e = RE15_PAD_BIT_SQUARE;
        if (f < 12) h = RE15_PAD_BIT_SQUARE;   /* Tuer: 9 Bilder Halten (aot_common.c door_hold, FUN_8002bd44) */
        if (f >= 40 && (f % 15) == 0 && g_scd.message_active) e = h = RE15_PAD_BIT_SQUARE;
        frame(h, e);
        if (g_scd.message_active && r.msg_id < 0) r.msg_id = (int)g_scd.message_id;
        if (g_room_change.pending) { r.raumwechsel = 1; r.ziel = (int)g_room_change.room_id; break; }
    }
    return r;
}
static void nachricht_weg(int bilder)
{
    for (int f = 0; f < bilder; f++) { uint16_t e = (g_scd.message_active && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
}
static int inv_geben(uint8_t id)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == 0) { g_inv.slots[i].id = id; g_inv.slots[i].qty = 1; return i; }
    return -1;
}
static int typ_zahl(uint8_t typ)
{
    int n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == typ) n++;
    return n;
}
static int prop_von(int obj)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == obj) return k;
    return -1;
}
/* Tuersequenz-Laeufer: faengt jede Anfrage (im Spiel spielt sie die Plattform; Muster test_r38_dining1141.c). */
static void fang(const re15_door_seq_anfrage_t *a) { (void)a; }

/* ---- Teil: spinnen ---------------------------------------------------------------------------------------- */
static void teil_spinnen(void)
{
    printf("== spinnen ==\n");
    const uint16_t raeume[2] = { 0x2090, 0x2091 };
    for (int r = 0; r < 2; r++) {
        re15_game_state_init();
        re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);   /* Szene vorbei: nur der Raumaufbau */
        const int v0 = re15_gatorszene_r41_spinnen_verworfen();
        if (room_boot(raeume[r], -8200, 0, -25550, 0, 0) != 0) return;
        for (int f = 0; f < 10; f++) frame(0, 0);
        PRUEF(typ_zahl(0x25) == 0 && re15_gatorszene_r41_spinnen_verworfen() == v0 + 2,
              "ROOM%04X: keine Spinne 0x25 (Saetze @0x00AB0/@0x00AC4 verworfen: %d)", raeume[r],
              re15_gatorszene_r41_spinnen_verworfen() - v0);
    }
    const int alt = re15_gatorszene_r41_spinnen_weg_setzen(0);
    re15_game_state_init();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    if (room_boot(0x2090, -8200, 0, -25550, 0, 0) == 0) {
        for (int f = 0; f < 10; f++) frame(0, 0);
        PRUEF(typ_zahl(0x25) == 2, "Gegenprobe (Verwerfen aus): %d Spinnen 0x25 (soll 2 = die RDT-Saetze)", typ_zahl(0x25));
    }
    re15_gatorszene_r41_spinnen_weg_setzen(alt);
}

/* ---- Teil: programme -------------------------------------------------------------------------------------- */
static int hat(const uint8_t *p, int n, const uint8_t *m, int mn)
{
    for (int i = 0; i + mn <= n; i++) if (!memcmp(p + i, m, (size_t)mn)) return i;
    return -1;
}
static void teil_programme(void)
{
    printf("== programme ==\n");
    for (int nr = 1; nr <= 3; nr++) {
        int n = 0, pc = 0, ok = 1;
        const uint8_t *p = re15_gatorszene_r41_programm(nr, &n);
        while (p && pc < n) { int sz = scd_opcode_size_at(p + pc); if (sz <= 0) { ok = 0; break; } pc += sz; }
        PRUEF(p && ok && pc == n && p[n - 2] == 0x01, "Programm %d: %d Byte, alle Opcodes mit Laenge, Evt_end am Ende", nr, n);
    }
    int n1 = 0, n2 = 0, n3 = 0;
    const uint8_t *p1 = re15_gatorszene_r41_programm(1, &n1);
    const uint8_t *p2 = re15_gatorszene_r41_programm(2, &n2);
    const uint8_t *p3 = re15_gatorszene_r41_programm(3, &n3);
    {
        static const uint8_t cut1[2] = { 0x29, 0x01 };
        static const uint8_t knall[8] = { 0x22, 0x05, RE15_GSZ_SIG_KNALL, 0x01, 0x09, 0x0a, 45, 0x00 };
        const int ic = hat(p1, n1, cut1, 2), ik = hat(p1, n1, knall, 8), it = hat(p1, n1, (const uint8_t *)"\x3b\x14", 2);
        PRUEF(ic >= 0 && ik > ic && it > ik, "P1: Cut 1 (@+%d), Knall Set(5,29,1) + Sleep 45 (@+%d), dann Door_aot_set (@+%d)",
              ic, ik, it);
    }
    {
        int em = 0, oben = 0;
        for (int pc = 0; pc < n2; ) {
            int sz = scd_opcode_size_at(p2 + pc);
            if (sz <= 0) break;
            if (p2[pc] == 0x44) {
                em++;
                const int16_t y = (int16_t)(p2[pc + 10] | (p2[pc + 11] << 8));
                const int16_t x = (int16_t)(p2[pc + 8] | (p2[pc + 9] << 8));
                if (p2[pc + 2] == 0x16 && p2[pc + 3] == 0x0d && p2[pc + 4] == 8 && y == -14400 && x > -16650) oben++;
            }
            pc += sz;
        }
        PRUEF(em == 8 && oben == 8, "P2: %d Sce_em_set, davon %d Typ 0x16 grid 0x0d Band 8 y -14400 oestlich der Absatzwand x -16650",
              em, oben);
        static const uint8_t cut2[2] = { 0x29, 0x02 };
        PRUEF(hat(p2, n2, cut2, 2) >= 0, "P2: Cut 2 (ROOM2080 RID @0x0A0)");
    }
    {
        static const uint8_t poll[4] = { 0x21, 0x05, RE15_GSZ_SIG_FERTIG, 0x00 };
        static const uint8_t ges[4] = { 0x22, RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 0x01 };
        const int ip = hat(p3, n3, poll, 4), ig = hat(p3, n3, ges, 4);
        PRUEF(ip >= 0 && ig > ip, "P3: Poll (5,30) (@+%d) vor Set(9,110,1) (@+%d)", ip, ig);
    }
}

/* ---- Teil: leiche ----------------------------------------------------------------------------------------- */
static void teil_leiche(void)
{
    printf("== leiche ==\n");
    const uint16_t raeume[2] = { 0x2090, 0x2091 };
    for (int r = 0; r < 2; r++) {
        re15_game_state_init();
        if (room_boot(raeume[r], -8200, 0, -25550, 0, 0) != 0) return;
        const int kl = prop_von(RE15_GSZ_LEICHE_OBJ), ks = prop_von(RE15_GSZ_KEY_OBJ);
        PRUEF(re15_gatorszene_r41_zustand() == RE15_GSZ_ANKUNFT, "ROOM%04X: Szene startet (Zustand ANKUNFT)", raeume[r]);
        PRUEF(kl >= 0 && g_scd.props[kl].x == -5500 && g_scd.props[kl].z == -23700 && g_scd.props[kl].rot_y == 384,
              "Leiche obj 2 an der Szenen-Lage (-5500,-23700) Gier 384 (leiche_lage.py)");
        PRUEF(ks >= 0 && g_scd.props[ks].active && g_scd.props[ks].parent_obj == RE15_GSZ_LEICHE_OBJ &&
              g_scd.props[ks].x == 1090 && g_scd.props[ks].y == -420 && g_scd.props[ks].z == -100 &&
              g_scd.props[ks].rot_x == 544 && g_scd.props[ks].rot_z == -464,
              "Schluessel obj 4 = Kind der Leiche, lokal (1090,-420,-100), Neigung rx 544 rz -464 (leiche_lage.py, Puls-Grenze)");
        PRUEF(re15_blink_r41_aktiv(raeume[r], RE15_GSZ_KEY_OBJ), "Schluessel blinkt (re15_blink_r41)");
    }
}

/* ---- Teil: texte ------------------------------------------------------------------------------------------ */
static void teil_texte(void)
{
    printf("== texte ==\n");
    size_t n = 0;
    uint8_t *d = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM20A0.RDT", &n);
    int l0 = 0, l1 = 0;
    const uint8_t *t0 = re15_gatorszene_r41_tor_text(0, &l0), *t1 = re15_gatorszene_r41_tor_text(1, &l1);
    PRUEF(d && n > 0x2070 && t0 && l0 == 16 && !memcmp(d + 0x203C, t0, 16), "msg 42 = ROOM20A0 msg 1 @0x203C \"It's locked.\"");
    PRUEF(d && t1 && l1 == 27 && !memcmp(d + 0x204C, t1, 27), "msg 43 = ROOM20A0 msg 2 @0x204C \"You've opened the lock.\"");
    free(d);
    static const uint8_t name[10] = { 0x33, 0x3d, 0x50, 0x41, 0x4e, 0x00, 0x27, 0x41, 0x55, 0x07 };   /* "Water Key" 07 */
    const uint8_t *nm = re15_gatorszene_r41_name(RE15_GSZ_KEY_ITEM);
    PRUEF(nm && !memcmp(nm, name, sizeof name), "Name Item 0x34 = \"Water Key\" (Namensblob-Form, 0x07)");
}

/* ---- Teil: tor -------------------------------------------------------------------------------------------- */
static void teil_tor(void)
{
    printf("== tor ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    /* 2090 Slot 1 @0x009B6: Rechteck x 6750..7750, z -18100..-14600, Band 1 -> Vorwaerts-Punkt (7250,-16350), Blick +X */
    if (room_boot(0x2090, 6630, -1800, -16350, 0, 2) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    PRUEF(re15_gatorszene_r41_tor_gesperrt(), "2090 Slot 1 (Tor -> 20A0) gesperrt (Ereignis %d)", RE15_GSZ_TOR_EREIGNIS);
    unsigned zu0 = g_re15_gsz_tor_zu;
    druck_t r = druecke(6630, -16350, 0, 200);
    PRUEF(r.msg_id == RE15_GSZ_MSG_ZU && !r.raumwechsel && g_re15_gsz_tor_zu == zu0 + 1,
          "ohne Schluessel: msg %d (soll 42 \"It's locked.\"), kein Raumwechsel", r.msg_id);
    nachricht_weg(120);
    inv_geben(RE15_GSZ_KEY_ITEM);
    r = druecke(6630, -16350, 0, 300);
    PRUEF(r.msg_id == RE15_GSZ_MSG_AUF && !r.raumwechsel, "mit Schluessel: msg %d (soll 43 \"You've opened the lock.\")",
          r.msg_id);
    for (int f = 0; f < 300 && !re15_discard_active(); f++) {
        uint16_t e = (g_scd.message_active && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
    PRUEF(re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_TOR) == 1, "(9,%d) = 1 (Tor aufgeschlossen)", RE15_GSZ_BIT_TOR);
    {
        const int frage = re15_discard_active();
        for (int w = 0; w < 900 && re15_discard_active(); w++) { uint16_t e = ((w % 10) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
        PRUEF(frage && !re15_discard_active() && re15_inv_find_item(RE15_GSZ_KEY_ITEM) < 0,
              "Wegwerf-Abfrage gestellt und mit Ja beantwortet: Water Key weg");
        for (int f = 0; f < 30; f++) frame(0, 0);
    }
    r = druecke(6630, -16350, 0, 200);
    PRUEF(r.raumwechsel && (r.ziel & 0xFFFE) == 0x20A0, "danach: Raumwechsel nach ROOM%04X (soll 20A0)", (unsigned)r.ziel);

    /* 20A0 Slot 0 @0x01746: Rechteck x 3450..6450, z 4500..5500, Band 2 -> Spieler (4950,4100) Blick +Z (Gier 3072) */
    re15_game_state_init();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    if (room_boot(0x20A0, 4950, -3600, 4100, 3072, 13) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    PRUEF(re15_gatorszene_r41_tor_gesperrt(), "20A0 Slot 0 (Tor -> 2090) gesperrt, solange (9,113) = 0");
    zu0 = g_re15_gsz_tor_zu;
    r = druecke(4950, 4100, 3072, 200);
    PRUEF(r.msg_id == RE15_GSZ_MSG20A0_ZU && !r.raumwechsel && g_re15_gsz_tor_zu == zu0 + 1,
          "20A0: msg %d (soll Original-msg 1 \"It's locked.\"), kein Raumwechsel", r.msg_id);
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_TOR, 1);
    if (room_boot(0x20A0, 4950, -3600, 4100, 3072, 13) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    PRUEF(!re15_gatorszene_r41_tor_gesperrt(), "20A0 nach (9,113): Tor nicht mehr gesperrt");
    r = druecke(4950, 4100, 3072, 200);
    PRUEF(r.raumwechsel && (r.ziel & 0xFFFE) == 0x2090, "20A0 nach (9,113): Raumwechsel nach ROOM%04X (soll 2090)", (unsigned)r.ziel);
}

/* ---- Teil: schluessel ------------------------------------------------------------------------------------- */
static void teil_schluessel(void)
{
    printf("== schluessel ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    if (room_boot(0x2090, -8200, 0, -25550, 0, 0) != 0) return;
    re15_enemy_ai_set_paused(1);                       /* nur der Szenen-Takt: der Alligator liegt als Leiche (Zustand 7) */
    re15_actor_t *g = gator_setzen();
    if (g) { g->x = -3000; g->z = -22000; g->y = -1200; g->state = 7; }
    for (int f = 0; f < 5; f++) frame(0, 0);
    const re15_aot_t *a = &g_aot.slots[15];
    const int ks = prop_von(RE15_GSZ_KEY_OBJ);
    PRUEF(re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT) == 1, "Leiche (Zustand 7) -> (9,%d) = 1", RE15_GSZ_BIT_LIEGT);
    PRUEF(ks >= 0 && g_scd.props[ks].active && g_scd.props[ks].parent_obj == -1 && g_scd.props[ks].y == -1620 &&
          re15_aot_water_at(g_scd.props[ks].x, g_scd.props[ks].z) != 0,
          "Schluessel liegt im Wasser bei der Leiche (%d,%d,%d)", ks >= 0 ? g_scd.props[ks].x : 0, ks >= 0 ? g_scd.props[ks].y : 0,
          ks >= 0 ? g_scd.props[ks].z : 0);
    PRUEF(a->active && a->type == RE15_AOT_TYPE_ITEM, "Zone Slot 15 = Item");
    PRUEF(re15_blink_r41_aktiv(0x2090, RE15_GSZ_KEY_OBJ), "Schluessel blinkt");
    if (ks < 0) return;
    /* Aufnahme: Vorwaerts-620-Punkt auf dem Schluessel (Blick -X), Quadrat, Abfrage "ja" (Muster test_r38 schluessel1011) */
    const int32_t kx = g_scd.props[ks].x, kz = g_scd.props[ks].z;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 20; f++) frame(0, 0);
    pl->x = kx + 620; pl->z = kz; pl->rot_y = 2048; pl->y = 0;
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    {
        uint8_t typ = 0; int wahl = -1, w = 0;
        const int modal = re15_item_modal_active();
        while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800) re15_item_modal_tick(0, 0);
        re15_item_modal_prompt(&typ, &wahl);
        PRUEF(modal && typ == RE15_GSZ_KEY_ITEM, "Druck auf den Schluessel: Aufnahme-Modal Item 0x%02X (ist 0x%02X)",
              RE15_GSZ_KEY_ITEM, typ);
        w = 0;
        while (re15_item_modal_active() && w++ < 1600) re15_item_modal_tick((uint16_t)0x4000u, 0);
    }
    PRUEF(re15_inv_find_item(RE15_GSZ_KEY_ITEM) >= 0, "Water Key (0x34) im Inventar");
    PRUEF(re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_GENOMMEN) == 1, "(9,%d) = 1", RE15_GSZ_BIT_GENOMMEN);
    re15_game_state_t gs = g_game;
    if (room_boot(0x2090, -8200, 0, -25550, 0, 0) != 0) return;
    g_game = gs;
    const int k2 = prop_von(RE15_GSZ_KEY_OBJ);
    PRUEF(k2 < 0 || !g_scd.props[k2].active, "Wiederbetreten nach der Aufnahme: kein Schluessel mehr");
}

/* ---- Teil: szene ------------------------------------------------------------------------------------------ */
static int folge_hat(char art, int wert)
{
    for (int i = 0; i < re15_gatorszene_r41_folge_n(); i++) {
        const re15_gsz_folge_t *f = re15_gatorszene_r41_folge(i);
        if (f && f->art == art && (wert < 0 || f->wert == wert)) return i;
    }
    return -1;
}
static void teil_szene(int elza)
{
    printf("== szene%s ==\n", elza ? "_elza" : "");
    const uint16_t r2090 = (uint16_t)(0x2090 + elza), r2080 = (uint16_t)(0x2080 + elza);
    re15_game_state_init();
    re15_inv_load_briefing();
    /* Ankunft ueber die Leiter = Nutzlast ROOM2080 Slot 1 @0x00BC2 (-8200,0,-25550) Gier 0 Cut 0 */
    if (room_boot(r2090, -8200, 0, -25550, 0, 0) != 0) return;
    PRUEF(re15_gatorszene_r41_zustand() == RE15_GSZ_ANKUNFT && folge_hat('P', 1) >= 0, "ROOM%04X: Programm 1 laeuft", r2090);
    int f = 0;
    for (; f < 600 && !g_room_change.pending; f++) frame(0, 0);
    PRUEF(folge_hat('T', -1) >= 0, "Knall-Signal (5,29) im Port-Takt angekommen (Folge T)");
    PRUEF(g_room_change.pending && (g_room_change.room_id & 0xFFFEu) == 0x2080 && g_room_change.x == -9950 &&
          g_room_change.y == -14400 && g_room_change.z == -25700 && g_room_change.target_cut == 2,
          "nach %d Bildern: Tuer nach ROOM%04X (%d,%d,%d) Cut %d (soll 2080 Leiter-Ankunft (-9950,-14400,-25700) Cut 2)", f,
          g_room_change.room_id, g_room_change.x, g_room_change.y, g_room_change.z, g_room_change.target_cut);

    if (room_boot(r2080, -9950, -14400, -25700, 2048, 2) != 0) return;
    PRUEF(re15_gatorszene_r41_zustand() == RE15_GSZ_MONTAGE, "ROOM%04X: Montage (Programm 2)", r2080);
    int n16 = 0, oben = 1, westlich = 0, bewegt = 0;
    int32_t x0[RE15_ACTOR_MAX], z0[RE15_ACTOR_MAX];
    for (int i = 0; i < RE15_ACTOR_MAX; i++) { x0[i] = g_actors[i].x; z0[i] = g_actors[i].z; }
    for (f = 0; f < 600 && !g_room_change.pending; f++) {
        frame(0, 0);
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            const re15_actor_t *a = &g_actors[i];
            if (!a->active || a->type != 0x16) continue;
            if (a->y != -14400) oben = 0;
            if (a->x <= -16650) westlich++;
        }
    }
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->type != 0x16) continue;
        n16++;
        if (abs(a->x - x0[i]) + abs(a->z - z0[i]) > 300) bewegt++;
    }
    PRUEF(n16 == 8, "8 Zombies Typ 0x16 in der Einstellung (ist %d)", n16);
    PRUEF(oben && westlich == 0, "alle Bilder: Zombies auf y -14400 (Band 8), keiner westlich der Absatzwand x -16650 (%d)", westlich);
    /* Gehen braucht die Zombie-Bank (Clips + Wurzelbahn), die nur der Lader der Plattform haengt - das Laufen misst
     * integration_r41_gatorszene_* am state.log der exe (Lauf lad4: 8 von 8 bewegt, z.B. (-9900,-14700) -> (-10732,-18661)). */
    printf("    (ohne Zombie-Bank im Harness: %d von 8 bewegt - Laufen misst der exe-Riegel)\n", bewegt);
    PRUEF(g_room_change.pending && (g_room_change.room_id & 0xFFFEu) == 0x2090 && g_room_change.x == -8200 &&
          g_room_change.z == -25550 && g_room_change.target_cut == 1,
          "nach %d Bildern: zurueck nach ROOM%04X (%d,%d) Cut %d (soll Eintrittslage (-8200,-25550), Cut 1)", f,
          g_room_change.room_id, g_room_change.x, g_room_change.z, g_room_change.target_cut);

    if (room_boot(r2090, -8200, 0, -25550, 0, 1) != 0) return;
    PRUEF(re15_gatorszene_r41_zustand() == RE15_GSZ_RUECKKEHR, "ROOM%04X: Rueckkehr (Programm 3)", r2090);
    if (!gator_bank()) { printf("  FEHLER: RE2-EM23-Bank nicht ladbar (CDEMD0.EMS)\n"); g_fail++; return; }
    re15_actor_t *g = gator_setzen();
    PRUEF(g != NULL, "Alligator gesetzt (main.c-Form)");
    int unter_vor = 0;
    for (f = 0; f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
        frame(0, 0);
        if (re15_gatorszene_r41_zustand() == RE15_GSZ_RUECKKEHR && g && re15_gator_boss_untergetaucht(g)) unter_vor++;
    }
    PRUEF(unter_vor > 0, "vor dem Signal untergetaucht (%d Bilder)", unter_vor);
    static const char arten[] = "SGGKGGVGGG";
    static const int  werte[] = { -1, 1, 2, 13, 3, 4, -1, 5, 6, 7 };
    int vor = -1, reihe = 1;
    for (int i = 0; i < 10; i++) {
        int j = -1;
        for (int k = vor + 1; k < re15_gatorszene_r41_folge_n(); k++) {
            const re15_gsz_folge_t *q = re15_gatorszene_r41_folge(k);
            if (q->art == arten[i] && (werte[i] < 0 || q->wert == werte[i])) { j = k; break; }
        }
        if (j < 0) { reihe = 0; printf("    fehlt: %c %d\n", arten[i], werte[i]); break; }
        vor = j;
    }
    PRUEF(reihe, "Ablauf S, G1 Durchbruch, G2 Biss, K13 Koppelbild, G3 Schuetteln, G4 Kauen, V verschlungen, G5 Wende, G6 Schrei, G7 Ende");
    PRUEF(re15_gatorszene_r41_zustand() == RE15_GSZ_KAMPF, "Zustand KAMPF nach %d Bildern", f);
    for (int k = 0; k < 120; k++) frame(0, 0);
    PRUEF(re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN) == 1, "(9,110) = 1 (Szene gesehen)");
    PRUEF(re15_game_flag_get(2, 7) == 0 && re15_game_flag_get(1, 27) == 0, "Riegel auf: (2,7) = 0, (1,27) = 0");
    PRUEF(prop_von(RE15_GSZ_LEICHE_OBJ) < 0 || !g_scd.props[prop_von(RE15_GSZ_LEICHE_OBJ)].active, "Leiche verschlungen (obj 2 aus)");
    PRUEF(prop_von(RE15_GSZ_KEY_OBJ) < 0 || !g_scd.props[prop_von(RE15_GSZ_KEY_OBJ)].active, "Schluessel mitgefressen (obj 4 aus)");
    PRUEF(re15_gatorszene_r41_bgm_eintrag(1, 9) == 0xFF3C, "Musik-Eintrag 0xFF3C (RE2 MAIN1A) im Kampf");
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    const int alle = !strcmp(teil, "alle");
    re15_door_seq_setze_laeufer(fang);
    if (alle || !strcmp(teil, "spinnen"))    teil_spinnen();
    if (alle || !strcmp(teil, "programme"))  teil_programme();
    if (alle || !strcmp(teil, "leiche"))     teil_leiche();
    if (alle || !strcmp(teil, "texte"))      teil_texte();
    if (alle || !strcmp(teil, "tor"))        teil_tor();
    if (alle || !strcmp(teil, "schluessel")) teil_schluessel();
    if (alle || !strcmp(teil, "szene"))      teil_szene(0);
    if (alle || !strcmp(teil, "szene_elza")) teil_szene(1);
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
