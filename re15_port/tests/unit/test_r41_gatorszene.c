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
 *   tod         Nachbesserung 1 (M2): Todeslagen der Abnahme 0 (Todesrolle Cut 0, R1 Cut 1, R1 mit Sprung-Cut 0, Elza) -
 *               Schluessel im Bild der Kamera, nicht verdeckt, im Wasser, zur Kamera gekippt, Blink-Spitze <= 0x80;
 *               Nachbesserung 2 (Abnahme 1, M2): Kamera = Zonenkamera mit dem GROESSTEN Bild (Quadrat >= Wert der Nachbesserung 1
 *               in der laufenden Kamera), Einstellung auf den Schluessel: Schnitt im Bild des Todes, Riegel, nach dem Halt
 *               Cut_old auf den Cut davor + Zonenkamera an + Riegel auf (Folge E / O)
 *   tod_explosion  Nachbesserung 3 (Abnahme 2, M1): R1 MIT Explosionsschnitt der Spur B (Lage des exe-Laufs KW) - angezeigter Cut
 *               je Bild: Explosions-Cut, dann sofort der Schluessel-Cut, kein Bild des Zonen-Cuts 0 bis zum Ende der Einstellung;
 *               Gegenprobe mit (9,111) vorab: B's Rueckschnitt auf Cut 0 unberuehrt
 *   bilddiff    Auswerter des exe-Riegels integration_r41_gatorszene_tod*: Bilder mit/ohne Schluessel (s. bilddiff())
 *   kamfolge    Auswerter des exe-Riegels integration_r41_gatorszene_tod_explosion*: Kamera je Bild ueber drei Referenzbilder
 *               (Explosions-, Schluessel-, Zonen-Cut), verlangt die Folge E+K+ ohne ein Bild des Zonen-Cuts (s. kamfolge())
 * Nachbesserung 1 in bestehenden Teilen: leiche (M5 Puls-Kappung + Gegenprobe der ersten Neigung), tor (M3 genau ein
 * Verschlossen-Ton in 2090 und 20A0), szene/szene_elza (M1 Kamera-Uebergabe auf Cut 0, Spielfigur im Bild, Sperre frei).
 *   szene       die ganze Szene mit dem echten Boss-Takt (RE2-EM23-Bank): Ankunft ueber die Leiter -> Knall -> 2080 (8 Zombies
 *               OBEN, Band 8, nie westlich der Absatzwand) -> zurueck -> Gator-Ablauf G1..G7 -> Kampf, (9,110) = 1, Riegel auf,
 *               Musik-Eintrag 0xFF3C
 *   szene_elza  dasselbe mit Elza (2091 / 2081)
 */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE 1   /* setenv/unsetenv (Teil tod_explosion), VERTRAG 2 */
#endif
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
#include "re15_light.h"
#include "re15_lock_se.h"
#include "re15_math.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */

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
extern int g_test_door_se_count;   /* tests/test_support.c: Spion re15_audio_re2_door_se (Tuer-/Schloss-Ton) */
extern int g_test_door_se_last;

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
    /* Lichtsatz des Raums wie room_common.c Schritt 10 (Nachbesserung 1: Puls-Kappung M5 / Schluessel-Neigung M2) */
    g_re15_room_lights_ok = (s_rdt.lights && re15_light_parse(s_rdt.lights, (size_t)s_rdt.lights_size, &g_re15_room_lights) == 0);
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
        /* Nachbesserung 1 (M5): der Puls laeuft nicht in die Kappung des PC-Zeichners (Primitivfarbe 0x80 = Texel x 1,0,
         * render_pc.c psx_prim_to_sdl_vert). Gemessen mit dem Zeichner-Licht (Lichtsatz Cut 1 @0x490, Elternkette obj 2). */
        int ruhe = -1, spitze = -1;
        const int ok = re15_gatorszene_r41_key_prim_jetzt(1, &ruhe, &spitze);
        PRUEF(ok && spitze <= 0x80 && ruhe > 0 && spitze * 128 >= ruhe * 0xBE - 128,
              "Cut 1: Primitivfarbe Ruhe %d -> Blink-Spitze %d (<= 0x80, Verhaeltnis %d/%d ~ 0xBE/0x80 = voller Dreieckpuls)",
              ruhe, spitze, spitze, ruhe);
        if (r == 0 && ks >= 0) {                     /* Gegenprobe: die alte Neigung rx 608 / rz -160 laeuft in die Kappung */
            const int16_t ax = g_scd.props[ks].rot_x, az = g_scd.props[ks].rot_z;
            g_scd.props[ks].rot_x = 608; g_scd.props[ks].rot_z = -160;
            int r2 = -1, s2 = -1;
            re15_gatorszene_r41_key_prim_jetzt(1, &r2, &s2);
            g_scd.props[ks].rot_x = ax; g_scd.props[ks].rot_z = az;
            PRUEF(s2 > 0x80, "Gegenprobe erste Fassung (rx 608 / rz -160): Ruhe %d -> Spitze %d > 0x80 (gekappt = Plateau, Abnahme 0 M5)",
                  r2, s2);
        }
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
    int se0 = g_test_door_se_count;
    druck_t r = druecke(6630, -16350, 0, 200);
    PRUEF(r.msg_id == RE15_GSZ_MSG_ZU && !r.raumwechsel && g_re15_gsz_tor_zu == zu0 + 1,
          "ohne Schluessel: msg %d (soll 42 \"It's locked.\"), kein Raumwechsel", r.msg_id);
    /* Nachbesserung 1 (M3): RE2 LAB_80051514 spielt im Nicht-Treffer-Zweig GENAU EINEN Ton (@0x800516a0 lui a0,0x216 /
     * @0x800516a4 jal 0x8005ba28), Satz der Art M (re15_lock_se_satz_fuer_art). msg 42 ist Port-Text (keine Tabellen-Stelle). */
    PRUEF(g_test_door_se_count - se0 == 1 && g_test_door_se_last == re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M),
          "2090 ohne Schluessel: %d Verschlossen-Ton(e), Satz %d (soll 1 x %d)", g_test_door_se_count - se0, g_test_door_se_last,
          re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M));
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
    se0 = g_test_door_se_count;
    r = druecke(4950, 4100, 3072, 200);
    PRUEF(r.msg_id == RE15_GSZ_MSG20A0_ZU && !r.raumwechsel && g_re15_gsz_tor_zu == zu0 + 1,
          "20A0: msg %d (soll Original-msg 1 \"It's locked.\"), kein Raumwechsel", r.msg_id);
    /* M3: msg 1 steht als Text-Platz-Stelle in gen/lock_se_sites.inc (Zeile 49) - re15_lock_se_notice spielt den Ton, der
     * Tor-Handler nicht noch einmal (Abnahme 0: 2 Aufrufe). */
    PRUEF(g_test_door_se_count - se0 == 1 && g_test_door_se_last == re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M),
          "20A0 ohne (9,113): %d Verschlossen-Ton(e), Satz %d (soll 1 x %d)", g_test_door_se_count - se0, g_test_door_se_last,
          re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M));
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
    /* Nachbesserung 1 (M2): die Mitte liegt ueber der Wasseroberflaeche (-1620 = ROOM2090 @0x009D6 p0), die tiefste Ecke des
     * zur Kamera gekippten Quadrats darauf - die Mitte also hoechstens eine halbe Diagonale (sqrt(2) x 90 = 127, Ecken +-90 der
     * MD1 @0x354C) darueber (Nachbesserung 2: die Kippung zur Kamera mit dem groessten Bild dreht um beide Achsen). */
    PRUEF(ks >= 0 && g_scd.props[ks].active && g_scd.props[ks].parent_obj == -1 && g_scd.props[ks].y <= -1620 &&
          g_scd.props[ks].y >= -1620 - 127 && re15_aot_water_at(g_scd.props[ks].x, g_scd.props[ks].z) != 0,
          "Schluessel liegt im Wasser bei der Leiche (%d,%d,%d)", ks >= 0 ? g_scd.props[ks].x : 0, ks >= 0 ? g_scd.props[ks].y : 0,
          ks >= 0 ? g_scd.props[ks].z : 0);
    PRUEF(a->active && a->type == RE15_AOT_TYPE_ITEM, "Zone Slot 15 = Item");
    PRUEF(re15_blink_r41_aktiv(0x2090, RE15_GSZ_KEY_OBJ), "Schluessel blinkt");
    if (ks < 0) return;
    /* Aufnahme: Vorwaerts-620-Punkt auf dem Schluessel (Blick -X), Quadrat, Abfrage "ja" (Muster test_r38 schluessel1011) */
    const int32_t kx = g_scd.props[ks].x, kz = g_scd.props[ks].z;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Nachbesserung 2 (M2): erst endet die Einstellung auf den Schluessel (Programm 4, Halt 90 mit Riegel (2,7) = Pad-Sperre),
     * dann der Letterbox-Abbau (15 Bilder, FUN_80021a0c) - vorher nimmt die Pad-Sperre den Druck nicht an. */
    {
        int f = 0;
        while (f < 200 && re15_gatorszene_r41_key_einstellung(NULL, NULL, NULL)) { frame(0, 0); f++; }
        PRUEF(f < 200 && !re15_gatorszene_r41_key_einstellung(NULL, NULL, NULL) && !re15_game_flag_get(2, 7),
              "Einstellung auf den Schluessel %d Bilder nach den 5 Takten zu Ende, Riegel auf (Dauer misst Teil tod)", f);
    }
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

static int folge_hat(char art, int wert);   /* Teil szene */
/* ---- Teil: tod (Nachbesserung 1, M2; Nachbesserung 2, M2) -------------------------------------------------------------
 * Nachbesserung 2: die Kamera der Lage ist die Zonenkamera mit dem GROESSTEN Schluessel-Bild (nicht mehr die laufende), die
 * Verdeckung je Knochen-Kugel (halbe Kastenbreite 800 @0x80118b98) statt "vor jedem Knochen", und im Bild des Todes beginnt die
 * Einstellung auf den Schluessel (Programm 4: Riegel, Halt 90, Cut_old auf den Cut davor, Plc_ret, Cut_auto, Riegel auf; Form
 * ROOM2010 sub03 @0x008EE-0x008F8). alt_px10 = Quadrat-Breite x10 der Nachbesserung 1 in der laufenden Kamera (dieser Riegel,
 * Stand a9472f02): die neue Lage muss mindestens so gross sein.
 * Die zwei Todeslagen der Abnahme 0 mit der RE2-EM23-Bank (alle Knochen der Leichen-Pose), je laufender Kamera:
 *   todesrolle   Leiche (-5416,-22578) (GB_TEST=3, Abnahme 0 Lauf T), Clip 7 letztes Bild, laufende Kamera Cut 0
 *                (state.log Abnahme 0 F470-F519 cam=0);
 *   r1           Leiche (3413,-25001) (gator_boss.log "R1 ende (Leiche) pos=(3413,-25001)"), Clip 9 letztes Bild, laufende
 *                Kamera Cut 1 (RVD 1 -> 0 erst bei x -1500..-500: die Zonenkamera des Suedostbeckens);
 *   r1_sprung    dieselbe Leiche mit Cut 0 (Messrezept K der Abnahme: Sprung-Cut 0 + Teleport nach Osten - die Leiche liegt
 *                hinter der Kamera Cut 0 (2754,-3132,-26190)) -> Ersatz: Zonenkamera mit dem groessten Schluessel-Bild.
 * Verlangt: Schluessel im Bild der gewaehlten Kamera (Bildrand 24..296 / 40..205), vor JEDEM Leichen-Knochen (Tiefe - 800 =
 * Alligator-Kasten z @0x80118b98), Mitte und vier Ecken im Wasser, Kippung <= 60 Grad zur Kamera hin, Blink-Spitze <= 0x80
 * (render_pc.c psx_prim_to_sdl_vert, kein Plateau), Zone Slot 15 am Schluessel. Gegenprobe der ersten Fassung: flach (rx 0 /
 * rz 0) am Kopf -> kein Bildpunkt hoeher als 1 (Bildhoehe 180 x sin(Blickhoehe) * H / Tiefe). */
static int kam_tiefe(int cut, const int32_t p[3], int *sx, int *sy, int32_t *vz)
{
    re15_camera_view_t v;
    if (cut < 0 || cut >= g_room_rdt.cut_count || re15_camera_build_view(&g_room_rdt.cuts[cut], &v) != 0) return 0;
    const int64_t px = p[0], py = p[1], pz = p[2];
    const int32_t vx = (int32_t)(((int64_t)v.rot[0]*px + (int64_t)v.rot[1]*py + (int64_t)v.rot[2]*pz) >> 12) + v.trans[0];
    const int32_t vy = (int32_t)(((int64_t)v.rot[3]*px + (int64_t)v.rot[4]*py + (int64_t)v.rot[5]*pz) >> 12) + v.trans[1];
    *vz = (int32_t)(((int64_t)v.rot[6]*px + (int64_t)v.rot[7]*py + (int64_t)v.rot[8]*pz) >> 12) + v.trans[2];
    if (*vz <= 0) return 0;
    *sx = 160 + (int)(((int64_t)vx * v.fov_screen_dist) / *vz);
    *sy = 120 + (int)(((int64_t)vy * v.fov_screen_dist) / *vz);
    return (int)v.fov_screen_dist;
}
static void tod_fall(const char *name, uint16_t raum, int32_t gx, int32_t gz, int16_t gier, int clip, int cam, int alt_px10)
{
    printf("  -- %s (ROOM%04X, Leiche (%d,%d) Clip %d, laufende Kamera Cut %d)\n", name, raum, gx, gz, clip, cam);
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    if (room_boot(raum, -8200, 0, -25550, 0, (uint8_t)cam) != 0) return;
    if (!gator_bank()) { printf("  FEHLER: RE2-EM23-Bank nicht ladbar (CDEMD0.EMS)\n"); g_fail++; return; }
    re15_enemy_ai_set_paused(1);                              /* nur der Szenen-Takt: die Leiche liegt (Zustand 7) */
    re15_actor_t *g = gator_setzen();
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    if (!g || !bk || !bk->ok || clip >= bk->anim.clip_count) { printf("  FEHLER: Alligator/Bank fehlt\n"); g_fail++; return; }
    g->x = gx; g->z = gz; g->y = -1200; g->rot_y = gier; g->state = 7;
    g->render_scale_q12 = 2731;                               /* GB_SCALE_Q12 = 2/3 (enemy_ai_boss_gator.c INIT) */
    /* Pose: Clip 0 Bild 0 (Schwimmen) - die Wurzelbahn der Todes-Clips 7/9 ist im Spiel ueber die Lage aufgeloest (Anker), hier
     * zaehlt nur die Lage der Knochen um die gemessene Leichen-Lage; die echten Todes-Posen misst der exe-Lauf. */
    (void)clip;
    g->motion = 0; g->anim_frame = 0; g->anim_frac = 0; g->anim_prev_valid = 0;
    {   /* gezeichneten Wurzelknochen auf die gemessene Leichen-Lage legen (Wurzelversatz des Bildes ausgleichen) */
        int32_t p0[3];
        if (re15_gr2_bone_welt_pub(g, 0, p0)) { g->x += gx - p0[0]; g->z += gz - p0[2]; }
    }
    g_scd.cam_id = (uint8_t)cam; g_scd.work_vars[0x0A] = (int16_t)cam; s_shown = cam;
    /* Knochen der Leiche VOR dem Bild: der Schluessel wird im SCD-Takt dieses Bildes gelegt (vor dem Boss-Takt, der den
     * Harness-Alligator ohne Boss-Phase GBP_DEAD weiterbewegen wuerde; im Spiel steht die Leiche in GBP_DEAD still). */
    static int32_t kn[64][3]; int nb = 0;
    for (int b = 0; b < bk->skel.bone_count && nb < 64; b++) if (re15_gr2_bone_welt_pub(g, b, kn[nb])) nb++;
    int32_t kopf[3] = { g->x, -1620, g->z };
    re15_gr2_bone_welt_pub(g, 6, kopf);
    frame(0, 0);
    const int kc = re15_gatorszene_r41_key_kamera(), ks = prop_von(RE15_GSZ_KEY_OBJ);
    PRUEF(re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT) == 1 && ks >= 0 && g_scd.props[ks].active,
          "Leiche -> (9,111) = 1, Schluessel obj 4 aktiv");
    if (ks < 0) return;
    {   /* Zonenkamera = Cut mit mindestens einem Wechselsatz ausser dem Such-Anker (RVD ROOM2090 Cut 0..7 je 3 Saetze) */
        int nz = 0;
        for (int i = 0; i < g_room_rdt.zone_count; i++) if ((int)g_room_rdt.zones[i].cam_from == kc) nz++;
        PRUEF(kc >= 0 && nz >= 2, "Kamera der Lage: Zonenkamera Cut %d (%d RVD-Saetze; laufende Cut %d)", kc, nz, cam);
    }
    const int32_t kp[3] = { g_scd.props[ks].x, g_scd.props[ks].y, g_scd.props[ks].z };
    int sx = -1, sy = -1; int32_t vz = 0;
    const int fd = kam_tiefe(kc, kp, &sx, &sy, &vz);
    const int px10 = fd > 0 && vz > 0 ? (int)(1800 * fd / vz) : 0;
    PRUEF(fd > 0 && sx >= 24 && sx <= 296 && sy >= 40 && sy <= 205,
          "Schluessel (%d,%d,%d) im Bild von Cut %d bei (%d,%d), Tiefe %d = %d.%d Bildpunkte breit", kp[0], kp[1], kp[2], kc, sx, sy,
          (int)vz, px10 / 10, px10 % 10);
    PRUEF(px10 >= alt_px10, "Quadrat %d.%d Bildpunkte >= Nachbesserung 1 in der laufenden Kamera (%d.%d)", px10 / 10, px10 % 10,
          alt_px10 / 10, alt_px10 % 10);
    /* Verdeckung (Nachbesserung 2): je Knochen eine Kugel mit Radius 800 (Alligator-Kasten z @0x80118b98); verdeckt, wenn sie nicht
     * ganz hinter dem Schluessel liegt und ihr Bildkreis den Schluessel (+ halbe Quadrat-Bildgroesse) trifft. */
    int verdeckt = 0;
    int32_t d_min = 0x7fffffff;
    for (int b = 0; b < nb; b++) {
        const int32_t *p = kn[b]; int bx = 0, by = 0; int32_t bz = 0;
        const int bf = kam_tiefe(kc, p, &bx, &by, &bz);
        if (bf > 0 && bz - 800 < vz) {
            if (bz <= 800) verdeckt = 1;
            else {
                const int64_t r = (int64_t)800 * bf / bz + (int64_t)90 * bf / vz;
                const int64_t ddx = sx - bx, ddy = sy - by;
                if (ddx * ddx + ddy * ddy <= r * r) verdeckt = 1;
            }
        }
        const int32_t d = (int32_t)re15_squareroot0((uint32_t)((int64_t)(p[0]-kp[0])*(p[0]-kp[0]) + (int64_t)(p[2]-kp[2])*(p[2]-kp[2])));
        if (d < d_min) d_min = d;
    }
    const int32_t r_nah = nb > 6 ? (int32_t)re15_squareroot0((uint32_t)((int64_t)(kn[6][0]-kn[0][0])*(kn[6][0]-kn[0][0]) +
                                                                    (int64_t)(kn[6][2]-kn[0][2])*(kn[6][2]-kn[0][2]))) : 3055;
    PRUEF(nb >= 10 && !verdeckt && d_min <= r_nah, "von der Leiche nicht verdeckt (%d Knochen-Kugeln 800), naechster Knochen %d "
          "entfernt (<= Kopf-Wurzel-Abstand der Leiche %d)", nb, (int)d_min, (int)r_nah);
    int wasser = re15_aot_water_at(kp[0], kp[2]) != 0;
    for (int e = 0; e < 4; e++) wasser &= re15_aot_water_at(kp[0] + ((e & 1) ? 90 : -90), kp[2] + ((e & 2) ? 90 : -90)) != 0;
    PRUEF(wasser, "Mitte und vier Ecken (+-90) im Wasser");
    /* Kippung: Weltnormale der MD1-Normalen (0,-4096,0) unter pc_prop_rot_q12(rx, 0, rz): n = (sin rz, -cos rz cos rx, -cos rz sin rx) */
    const int32_t sxr = re15_sin_q12(g_scd.props[ks].rot_x), cxr = re15_cos_q12(g_scd.props[ks].rot_x);
    const int32_t szr = re15_sin_q12(g_scd.props[ks].rot_z), czr = re15_cos_q12(g_scd.props[ks].rot_z);
    const int32_t n[3] = { szr, -(int32_t)(((int64_t)czr * cxr) >> 12), -(int32_t)(((int64_t)czr * sxr) >> 12) };
    const re15_camera_cut_t *c = &g_room_rdt.cuts[kc];
    const int64_t cv = (int64_t)n[0] * (c->pos_x - kp[0]) + (int64_t)n[1] * (c->pos_y - kp[1]) + (int64_t)n[2] * (c->pos_z - kp[2]);
    PRUEF(-n[1] >= 2048 && cv > 0, "Neigung rx %d rz %d: Kippung <= 60 Grad (n_y %d), zur Kamera hin", (int)g_scd.props[ks].rot_x,
          (int)g_scd.props[ks].rot_z, (int)n[1]);
    int ruhe = -1, spitze = -1;
    re15_gatorszene_r41_key_prim_jetzt(kc, &ruhe, &spitze);
    PRUEF(ruhe > 0 && spitze <= 0x80 && spitze * 128 >= ruhe * 0xBE - 256,
          "Primitivfarbe Ruhe %d -> Blink-Spitze %d (<= 0x80: voller Dreieckpuls; NCCT-Rundung <= 2)", ruhe, spitze);
    const re15_aot_t *a = &g_aot.slots[15];
    PRUEF(a->active && a->type == RE15_AOT_TYPE_ITEM && a->x == kp[0] && a->z == kp[2], "Zone Slot 15 = Item am Schluessel");
    /* Einstellung auf den Schluessel (Nachbesserung 2): im Bild des Todes Schnitt auf kc, Riegel (2,7)/(1,27), Programm 4 laeuft;
     * nach dem Halt (90 Bilder = GSZ_LEICHE_BILDER) Cut_old auf den Cut davor (FUN_8004032c @0x8004033c/@0x80040364), Zonenkamera
     * an (@0x80040374-84), Riegel auf (ROOM1150 sub08 @0x012E4/@0x012E8). */
    {
        int ec = -1, ev = -1; unsigned eb = 0;
        const int laeuft = re15_gatorszene_r41_key_einstellung(&ec, &ev, &eb);
        PRUEF(laeuft && ec == kc && ev == cam && (int)g_scd.cam_id == kc && !g_scd.cut_auto_enabled &&
              re15_game_flag_get(2, 7) && re15_game_flag_get(1, 27),
              "Einstellung: Cut %d (vorher %d), Zonenkamera aus, Riegel (2,7)=%d (1,27)=%d, Programm 4 laeuft (%d)", (int)g_scd.cam_id,
              ev, re15_game_flag_get(2, 7), re15_game_flag_get(1, 27), laeuft);
        int f = 0;
        while (f < 120 && re15_gatorszene_r41_key_einstellung(NULL, NULL, NULL)) { frame(0, 0); f++; }
        frame(0, 0);
        PRUEF(f >= 90 && f <= 92 && (int)g_scd.cam_id == cam && g_scd.cut_auto_enabled == 1 && !re15_game_flag_get(2, 7) &&
              !re15_game_flag_get(1, 27), "nach %d Bildern (Halt 90): Cut %d (soll %d = Cut_old), Zonenkamera an, Riegel auf", f,
              (int)g_scd.cam_id, cam);
        PRUEF(folge_hat('E', kc) >= 0 && folge_hat('O', cam) >= 0, "Folge E %d (Schnitt) und O %d (zurueck)", kc, cam);
    }
    /* Gegenprobe erste Fassung: flach am Kopf -> Bildhoehe unter 1 Bildpunkt */
    {
        kopf[1] = -1620;
        int hx, hy; int32_t hz;
        const int hf = kam_tiefe(kc, kopf, &hx, &hy, &hz);
        int64_t hh = 0;
        if (hf > 0 && hz > 0) {
            const int64_t ddx = c->pos_x - kopf[0], ddy = c->pos_y - kopf[1], ddz = c->pos_z - kopf[2];
            const uint32_t dl = re15_squareroot0((uint32_t)(ddx * ddx + ddy * ddy + ddz * ddz));
            hh = dl ? (int64_t)180 * (-ddy) * hf * 10 / ((int64_t)dl * hz) : 0;   /* 180 x sin(Blickhoehe) x H / Tiefe, x10 */
        }
        printf("    Gegenprobe flach am Kopf (%d,%d): %s, Bildhoehe %d.%d Bildpunkte\n", kopf[0], kopf[2],
               hf > 0 ? "vor der Kamera" : "hinter der Kamera", (int)(hh / 10), (int)(hh % 10 < 0 ? -(hh % 10) : hh % 10));
    }
}
static void teil_tod(void)
{
    printf("== tod ==\n");
    /* alt_px10 = Nachbesserung 1 (Dossier N1.M2): Todesrolle Cut 0 9,7 / R1 Cut 1 7,5 / R1 Sprung-Cut 0 -> Ersatz Cut 1 7,5 */
    tod_fall("todesrolle", 0x2090, -5416, -22578, 1024, 7, 0, 97);
    tod_fall("r1", 0x2090, 3413, -25001, 2048, 9, 1, 75);
    tod_fall("r1_sprung", 0x2090, 3413, -25001, 2048, 9, 0, 75);
    tod_fall("todesrolle_elza", 0x2091, -5416, -22578, 1024, 7, 0, 97);
}

/* ---- Teil: tod_explosion (Nachbesserung 3, Abnahme 2 M1) -----------------------------------------------------------------
 * R1 MIT Explosionsschnitt der Spur B in der Lage des exe-Laufs KW (= test_r41_re2gator Fall (b2), GB_TEST=5): Gator (4997,-20318)
 * Gier 1024 traegt den Kanister, Leon an der Leiter (-8200,-25550), Zonen-Cut 0 zeigt den Kopf nicht -> P0 schneidet auf die
 * Explosions-Kamera E (RE2 +0x218 |= 0x40 @0x80102248). Am Clip-9-Ende schnitte B auf Cut 0 zurueck (RE2 `jal 0x8002c7ac`
 * @0x801024ec), das Tot-Flag kaeme zwei Takte spaeter (P2 @0x80102638-50, P3 @0x8010265c-64, Sperre frei @0x80102688-94).
 * Verlangt Bild fuer Bild (angezeigter Cut = re15_cam_present_tick am Bildanfang, wie der Zeichner): erst E, dann der Schluessel-
 * Cut K, KEIN Bild mit Cut 0 bis zum Ende der Einstellung; Folge R E/0 und E K; Einstellung im Takt des Rueckschnitts (vorher Cut 0,
 * Clip 9 letztes Bild); nach dem Tot-Flag haelt die Einstellung K mit gesperrter Zonenkamera (B's P3 gibt nichts frei); nach dem
 * Halt Cut_old auf Cut 0 und Zonenkamera an. Gegenprobe mit (9,111) vorab (Schluessel liegt schon): B's Ablauf unberuehrt - zwei
 * Bilder Cut 0 vor dem Tot-Flag wie RE2, keine Einstellung. */
static void gb_test_env(int an)
{
#ifdef _WIN32
    _putenv(an ? "RE15_GB_TEST=5" : "RE15_GB_TEST=");
    _putenv("RE15_GB_STUMM=1");
    _putenv("RE15_GATOR_KANISTER=");
#else
    if (an) setenv("RE15_GB_TEST", "5", 1); else unsetenv("RE15_GB_TEST");
    setenv("RE15_GB_STUMM", "1", 1);
    unsetenv("RE15_GATOR_KANISTER");
#endif
}
static void tod_explosion_fall(uint16_t raum, int gegenprobe)
{
    printf("  -- ROOM%04X R1 mit Explosionsschnitt%s\n", raum, gegenprobe ? " - Gegenprobe (9,111) vorab" : "");
    gb_test_env(1);
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    if (gegenprobe) re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT, 1);
    if (room_boot(raum, -8200, 0, -25550, 0, 0) != 0) { gb_test_env(0); return; }
    if (!gator_bank()) { printf("  FEHLER: RE2-EM23-Bank nicht ladbar (CDEMD0.EMS)\n"); g_fail++; gb_test_env(0); return; }
    re15_actor_t *g = gator_setzen();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!g) { printf("  FEHLER: Alligator nicht gesetzt\n"); g_fail++; gb_test_env(0); return; }
    const int gslot = (int)(g - g_actors);
    frame(0, 0);                                              /* INIT (GB_TEST=5: aufgetaucht wartend) */
    const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
    re15_gr2_kanister_setzen(4997, -1200, -20318 - (int32_t)(((int64_t)8800 * sk) >> 12));
    for (int f = 0; f < 160 && !re15_gr2_traegt(); f++) {
        g->x = 4997; g->z = -20318; g->rot_y = 1024;
        pl->x = -8200; pl->z = -25550; pl->rot_y = 0;
        frame(0, 0);
    }
    PRUEF(re15_gr2_traegt(), "Alligator traegt den Kanister (Stufe 7)");
    if (!re15_gr2_traegt()) { gb_test_env(0); return; }
    g_scd.cam_id = 0; g_scd.work_vars[0x0A] = 0; g_scd.cut_auto_enabled = 1; s_shown = 0;
    const int n_folge0 = re15_gatorszene_r41_folge_n();
    (void)re15_enemy_take_damage(g, 0);                       /* Treffer beim Tragen -> R1 (@0x80101ef4-fc) */
    const int ecut = (int)g_scd.cam_id;
    PRUEF(re15_gr2_r1_laeuft(gslot) && re15_gr2_kamera_gesperrt() && ecut != 0,
          "R1 mit Explosionsschnitt: Cut 0 -> Cut %d, Sperre (RE2 +0x218 |= 0x40 @0x80102248)", ecut);
    if (!re15_gr2_r1_laeuft(gslot) || ecut == 0) { gb_test_env(0); return; }
    char folge[400]; int nf = 0, tot_bild = -1, einst_bild = -1, null_bilder = 0, k_cut = -1;
    int einst_nach_tot = 0, gesperrt_nach_tot = 0;
    for (int f = 0; f < 300 && nf < (int)sizeof folge - 1; f++) {
        frame(0, 0);
        const int angezeigt = s_shown;
        folge[nf++] = (char)(angezeigt < 10 ? '0' + angezeigt : 'a' + angezeigt - 10);
        if (angezeigt == 0 && (einst_bild < 0 || re15_gatorszene_r41_key_einstellung(NULL, NULL, NULL))) null_bilder++;
        if (einst_bild < 0 && re15_gatorszene_r41_key_einstellung(&k_cut, NULL, NULL)) einst_bild = f;
        if (tot_bild < 0 && re15_gr2_tot()) {
            tot_bild = f;
            einst_nach_tot = re15_gatorszene_r41_key_einstellung(NULL, NULL, NULL);
            gesperrt_nach_tot = (g_scd.cut_auto_enabled == 0);
        }
        if (tot_bild >= 0 && (gegenprobe ? f > tot_bild + 20 : !re15_gatorszene_r41_key_einstellung(NULL, NULL, NULL))) break;
    }
    folge[nf] = 0;
    printf("    angezeigter Cut je Bild ab dem Treffer: %s\n", folge);
    if (gegenprobe) {
        PRUEF(einst_bild < 0 && null_bilder >= 2 && tot_bild >= 0,
              "Gegenprobe: keine Einstellung, B schneidet vor dem Tot-Flag (Bild %d) auf Cut 0 zurueck (%d Bilder Cut 0) - RE2-Form",
              tot_bild, null_bilder);
        PRUEF(folge_hat('R', -1) < 0, "Gegenprobe: keine Folge-Zeile R");
        gb_test_env(0);
        return;
    }
    int r_i = -1;
    for (int i = n_folge0; i < re15_gatorszene_r41_folge_n(); i++) {
        const re15_gsz_folge_t *fz = re15_gatorszene_r41_folge(i);
        if (fz && fz->art == 'R') { r_i = i; break; }
    }
    const re15_gsz_folge_t *fr = r_i >= 0 ? re15_gatorszene_r41_folge(r_i) : NULL;
    PRUEF(fr && fr->wert == ecut && fr->wert2 == 0, "Folge R %d/%d: Schluessel-Schnitt statt B's Rueckschnitt (Explosions-Cut %d, "
          "Ausgangs-Cut 0)", fr ? fr->wert : -1, fr ? fr->wert2 : -1, ecut);
    PRUEF(einst_bild >= 0 && tot_bild >= 0 && tot_bild - einst_bild == 2 && k_cut >= 0 && k_cut != ecut && k_cut != 0,
          "Einstellung Cut %d ab Bild %d, Tot-Flag Bild %d (P1-Ende -> P2 -> P3 = zwei Takte, @0x80102638-50 / @0x8010265c-64)", k_cut,
          einst_bild, tot_bild);
    PRUEF(null_bilder == 0, "kein angezeigtes Bild mit Cut 0 bis zum Ende der Einstellung (%d; vorher zwei, Abnahme 2 M1)", null_bilder);
    {   /* Folge E..E K..K 0 (Cut_old): genau ein Wechsel E -> K, danach erst nach der Einstellung Cut 0 */
        int ok = nf > 2 && folge[0] == (char)('0' + ecut), w = 0;
        for (int i = 1; i < nf; i++) if (folge[i] != folge[i - 1]) w++;
        ok = ok && w <= 2 && strchr(folge, (char)('0' + k_cut)) != NULL;
        const char *k0 = strchr(folge, (char)('0' + k_cut));
        ok = ok && k0 && (k0 == folge || k0[-1] == (char)('0' + ecut));
        PRUEF(ok, "Bildfolge Explosion %d -> Schluessel %d direkt (%d Wechsel, letzter = Cut_old auf 0)", ecut, k_cut, w);
    }
    PRUEF(einst_nach_tot && gesperrt_nach_tot && !re15_gr2_kamera_gesperrt(),
          "nach dem Tot-Flag haelt die Einstellung (Zonenkamera gesperrt %d, B's Programm frei %d)", gesperrt_nach_tot,
          !re15_gr2_kamera_gesperrt());
    PRUEF(!re15_gatorszene_r41_key_einstellung(NULL, NULL, NULL) && (int)g_scd.cam_id == 0 && g_scd.cut_auto_enabled == 1 &&
          re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT) == 1,
          "nach dem Halt: Cut_old auf Cut 0 (FUN_8004032c @0x8004033c/@0x80040364), Zonenkamera an (@0x80040374-84), (9,111) = 1");
    gb_test_env(0);
}
static void teil_tod_explosion(void)
{
    printf("== tod_explosion ==\n");
    tod_explosion_fall(0x2090, 0);
    tod_explosion_fall(0x2091, 0);
    tod_explosion_fall(0x2090, 1);
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
    /* Nachbesserung 1 (M1): Szenenende = Cut_chg auf den Eintritts-Cut (Leiter ROOM2080 Slot 1 @0x00BC2 -> Cut 0, Form ROOM1150
     * sub08 `29 05` @0x012C4 vor Plc_ret @0x012E0), im selben Bild wie die Freigabe der Spur-B-Sperre; die Spielfigur im Bild. */
    {
        const int ic = folge_hat('C', 0);
        const re15_gsz_folge_t *q = ic >= 0 ? re15_gatorszene_r41_folge(ic) : NULL;
        PRUEF(q && q->wert2 == 1, "Folge C: Kamera-Uebergabe auf Cut 0, Spielfigur im Bild (wert2 %d)", q ? q->wert2 : -1);
        PRUEF(g_scd.cam_id == 0 && g_scd.work_vars[0x0A] == 0 && re15_gatorszene_r41_spieler_im_bild(0) && !re15_gr2_kamera_gesperrt(),
              "120 Bilder nach dem Kampfbeginn: Cut %d (soll 0), Spielfigur im Bild, Spur-B-Sperre frei", (int)g_scd.cam_id);
        PRUEF(g_scd.cut_auto_enabled == 1, "Zonenkamera an (Cut_auto 1 aus Programm 3, ROOM1150 sub08 @0x012E2)");
    }
}

/* ---- Auswerter fuer den exe-Riegel (integration_r41_gatorszene_tod*, Nachbesserung 1 M2) -------------------------------
 * Aufruf `test_r41_gatorszene bilddiff <ordnerA> <ordnerB> <f0> <f1> <schritt>`: A = Lauf mit Schluessel, B = derselbe Lauf mit
 * (9,112) vorab (kein Schluessel), je Bild fd_NNNNNN.ppm (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1). Je Bild: Zahl der Pixel mit
 * Unterschied und der hellste Unterschieds-Pixel in A (max R/G/B). Verlangt: (1) in JEDEM Bild ein Unterschied (Schluessel im
 * Bild), (2) Puls max/min >= 1,40 (Blink-Folge 0x80..0xBE = x1,484, FUN_801028ec; die gekappte erste Fassung der Ankunft lag
 * bei 1,22), (3) kein Plateau: hoechstens 4 von 30 Bildern auf dem Hoechstwert (gekappt: 34 von 60 Bildern).
 * Nachbesserung 2 (M2): optional `<min_breite> <min_hoehe>` - (4) der Kasten aller Unterschiede ist mindestens so gross
 * (Riegel: 10 x 9 = der Ankunfts-Schluessel ~10,4 x 9,5; Abnahme 1 mass nach dem Tod 4 x 6 bzw. 6 x 4). */
static int ppm_lesen(const char *p, int *w, int *h, unsigned char **px)
{
    FILE *f = fopen(p, "rb");
    if (!f) return 0;
    char m[3] = { 0 }; int mx = 0;
    if (fscanf(f, "%2s %d %d %d", m, w, h, &mx) != 4 || m[0] != 'P' || m[1] != '6' || *w <= 0 || *h <= 0) { fclose(f); return 0; }
    fgetc(f);
    *px = (unsigned char *)malloc((size_t)(*w) * (size_t)(*h) * 3u);
    const size_t n = *px ? fread(*px, 1, (size_t)(*w) * (size_t)(*h) * 3u, f) : 0;
    fclose(f);
    if (n != (size_t)(*w) * (size_t)(*h) * 3u) { free(*px); *px = NULL; return 0; }
    return 1;
}
static int bilddiff(const char *da, const char *db, int f0, int f1, int st, int min_b, int min_h)
{
    int bilder = 0, mit = 0, hmin = 256, hmax = -1, am_max = 0, werte[512], nw = 0;
    int kx0 = 9999, ky0 = 9999, kx1 = -1, ky1 = -1;
    for (int f = f0; f <= f1 && nw < 512; f += (st > 0 ? st : 1)) {
        char pa[700], pb[700];
        snprintf(pa, sizeof pa, "%s/fd_%06d.ppm", da, f); snprintf(pb, sizeof pb, "%s/fd_%06d.ppm", db, f);
        int wa, ha, wb, hb; unsigned char *a = NULL, *b = NULL;
        if (!ppm_lesen(pa, &wa, &ha, &a) || !ppm_lesen(pb, &wb, &hb, &b) || wa != wb || ha != hb) {
            printf("  F%d: Bild fehlt (%s / %s)\n", f, pa, pb); free(a); free(b); continue;
        }
        bilder++;
        int n = 0, hell = 0;
        for (int y = 0; y < ha; y++)
            for (int x = 0; x < wa; x++) {
                const unsigned char *qa = a + ((size_t)y * wa + x) * 3, *qb = b + ((size_t)y * wa + x) * 3;
                if (qa[0] == qb[0] && qa[1] == qb[1] && qa[2] == qb[2]) continue;
                n++;
                const int v = qa[0] > qa[1] ? (qa[0] > qa[2] ? qa[0] : qa[2]) : (qa[1] > qa[2] ? qa[1] : qa[2]);
                if (v > hell) hell = v;
                if (x < kx0) kx0 = x;
                if (y < ky0) ky0 = y;
                if (x > kx1) kx1 = x;
                if (y > ky1) ky1 = y;
            }
        free(a); free(b);
        if (n > 0) { mit++; werte[nw++] = hell; if (hell < hmin) hmin = hell; if (hell > hmax) hmax = hell; }
        printf("  F%d: %d Pixel Unterschied, hellster %d\n", f, n, hell);
    }
    for (int i = 0; i < nw; i++) if (werte[i] == hmax) am_max++;
    const int kb = kx1 >= kx0 ? kx1 - kx0 + 1 : 0, kh = ky1 >= ky0 ? ky1 - ky0 + 1 : 0;
    const int ok = bilder > 0 && mit == bilder && hmin > 0 && hmax * 100 >= hmin * 140 && am_max * 30 <= 4 * nw &&
                   kb >= min_b && kh >= min_h;
    printf("bilddiff: %d Bilder, %d mit Schluessel-Unterschied, Kasten (%d,%d)-(%d,%d) = %d x %d (mindestens %d x %d), hellster "
           "min %d max %d (x%d.%02d), %d Bilder am Hoechstwert -> %s\n", bilder, mit, kx0, ky0, kx1, ky1, kb, kh, min_b, min_h,
           hmin, hmax, hmin > 0 ? hmax / hmin : 0, hmin > 0 ? (hmax * 100 / hmin) % 100 : 0, am_max, ok ? "OK" : "FEHLER");
    return ok ? 0 : 1;
}

/* ---- Auswerter kamfolge (Nachbesserung 3, Abnahme 2 M1) -----------------------------------------------------------------
 * Bilder fd_<f0..f1>.ppm eines Laufs mit Explosionsschnitt der Spur B (R1 P0, RE2 +0x218 |= 0x40 @0x80102248): je Bild die
 * mittlere Abweichung (Summe |Delta| ueber alle Kanaele / Kanalzahl) zu drei Referenzbildern desselben Laufs - E (mitten in der
 * Explosions-Einstellung), K (mitten in der Schluessel-Einstellung), X (Zonen-Cut nach Cut_old) - und die Klasse = naechste
 * Referenz (keine Schwelle). Verlangt die Folge E+K+: zuerst Explosions-Cut, dann Schluessel-Cut, KEIN Bild des Zonen-Cuts
 * dazwischen und kein Zurueck. Vorher (Abnahme 2 Lauf KW, Nachbesserung 3 Lauf V_KW): fd 197/198 = X (mittlere Abweichung zu X 3,31
 * / 0,19 gegen E 14,34 / K 11,64) -> E+X+K+. */
static int kamfolge(const char *d, int f0, int f1, int fe, int fk, int fx)
{
    const int refn[3] = { fe, fk, fx };
    const char kl[3] = { 'E', 'K', 'X' };
    unsigned char *ref[3] = { NULL, NULL, NULL };
    int w = 0, h = 0, ok = 1;
    for (int r = 0; r < 3; r++) {
        char pr[700]; int wr, hr;
        snprintf(pr, sizeof pr, "%s/fd_%06d.ppm", d, refn[r]);
        if (!ppm_lesen(pr, &wr, &hr, &ref[r]) || (r > 0 && (wr != w || hr != h))) {
            printf("kamfolge: Referenzbild %c fehlt/ungleich gross (%s) -> FEHLER\n", kl[r], pr);
            for (int i = 0; i < 3; i++) free(ref[i]);
            return 1;
        }
        w = wr; h = hr;
    }
    char folge[1024]; int nf = 0, n[3] = { 0, 0, 0 };
    for (int f = f0; f <= f1 && nf < (int)sizeof folge - 1; f++) {
        char pa[700]; int wa, ha; unsigned char *a = NULL;
        snprintf(pa, sizeof pa, "%s/fd_%06d.ppm", d, f);
        if (!ppm_lesen(pa, &wa, &ha, &a) || wa != w || ha != h) { printf("  F%d: Bild fehlt (%s)\n", f, pa); free(a); ok = 0; continue; }
        long long sum[3] = { 0, 0, 0 };
        const size_t nb = (size_t)w * (size_t)h * 3u;
        for (int r = 0; r < 3; r++)
            for (size_t i = 0; i < nb; i++) { const int dv = (int)a[i] - (int)ref[r][i]; sum[r] += dv < 0 ? -dv : dv; }
        free(a);
        int best = 0;
        for (int r = 1; r < 3; r++) if (sum[r] < sum[best]) best = r;
        n[best]++;
        folge[nf++] = kl[best];
        printf("  F%d: mittlere Abweichung E %lld.%02lld K %lld.%02lld X %lld.%02lld -> %c\n", f,
               sum[0] * 100 / (long long)nb / 100, sum[0] * 100 / (long long)nb % 100, sum[1] * 100 / (long long)nb / 100,
               sum[1] * 100 / (long long)nb % 100, sum[2] * 100 / (long long)nb / 100, sum[2] * 100 / (long long)nb % 100, kl[best]);
    }
    folge[nf] = 0;
    for (int i = 0; i < 3; i++) free(ref[i]);
    /* E+K+: erstes Bild E, letztes K, genau ein Wechsel E -> K, kein X */
    int wechsel = 0;
    for (int i = 1; i < nf; i++) if (folge[i] != folge[i - 1]) wechsel++;
    ok = ok && nf > 1 && n[2] == 0 && folge[0] == 'E' && folge[nf - 1] == 'K' && wechsel == 1;
    printf("kamfolge: %d Bilder F%d-F%d (Referenzen E F%d, K F%d, X F%d): E %d, K %d, X %d, %d Wechsel, Folge %s -> %s\n", nf, f0,
           f1, fe, fk, fx, n[0], n[1], n[2], wechsel, folge, ok ? "OK" : "FEHLER");
    return ok ? 0 : 1;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    if (!strcmp(teil, "bilddiff") && argc >= 7)
        return bilddiff(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), argc >= 9 ? atoi(argv[7]) : 0,
                        argc >= 9 ? atoi(argv[8]) : 0);
    if (!strcmp(teil, "kamfolge") && argc >= 8)
        return kamfolge(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
    const int alle = !strcmp(teil, "alle");
    re15_door_seq_setze_laeufer(fang);
    if (alle || !strcmp(teil, "spinnen"))    teil_spinnen();
    if (alle || !strcmp(teil, "programme"))  teil_programme();
    if (alle || !strcmp(teil, "leiche"))     teil_leiche();
    if (alle || !strcmp(teil, "texte"))      teil_texte();
    if (alle || !strcmp(teil, "tor"))        teil_tor();
    if (alle || !strcmp(teil, "schluessel")) teil_schluessel();
    if (alle || !strcmp(teil, "tod"))        teil_tod();
    if (alle || !strcmp(teil, "tod_explosion")) teil_tod_explosion();
    if (alle || !strcmp(teil, "szene"))      teil_szene(0);
    if (alle || !strcmp(teil, "szene_elza")) teil_szene(1);
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
