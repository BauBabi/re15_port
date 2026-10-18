/*
 * test_r38_dining1141.c — Runde 38 Spur F "dining1141": Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit allen
 * Installern, SCD-VM, Spieler-/NPC-Walker, Nachrichten-FSM, re15_game_step), Muster test_r37_sperre1050.c.
 *
 * Dossier analysis/befunde_runde38/F_dining1141.md, Code include/re15_dining_r38.h + engine/src/dining_r38.c, Programme
 * engine/src/gen/dining_r38_programme.inc (tools/r38_dining/programme_bauen.py).
 *
 * Teile (Aufruf: test_r38_dining1141 <teil>):
 *   texte       die fuenf Port-Texte dekodiert = Wortlaut (Hinweis an Roys Leiche, Tuertexte, "Are you okay? ...", Sperre);
 *               kein Port-Text und kein Programm zeigt "Help" als Text (Punkt 1: Help + Klopfen nur als Ton)
 *   programme   Programm-Bytes: HAEMMERN ohne Message_on; BEFREIUNG ohne msg 0/1/2, mit msg 40, Lagen = Kopf-Konstanten
 *   leiche1021  ROOM1021 nach der Szene: Roys Leiche (Ereignis 6) -> msg 40 mit dem Hinweis
 *   schluessel1011  ROOM1011: Schluessel liegt NUR nach der 1021-Szene und bis zur Aufnahme; Aufnahme per Aktionstaste
 *   tuer1131    ROOM1131: Tuer zu ohne Schluessel (msg 40, kein Raumwechsel), mit Schluessel msg 41 + (9,66), dann Tuer
 *   phase_h     ROOM1141 Nachtrag 2: John ab dem ERSTEN Bild hinter dem Rolladen, Klopfen/Help-Takt als Ton, keine
 *               Nachricht; Gemaelde "ja" -> Schleife aus, Rolladen-Ton, John kommt heraus (keine Spruenge), Szene ohne
 *               msg 0/1, Sperre msg 41 an beiden Tueren, Gorillas tot -> Abgang ausserhalb des Bildes (Cut 7), Riegel offen,
 *               Dachtuer -> ROOM1171; Wiederbetreten danach: kein John
 *   leon1140    ROOM1140 (Leon): Gemaelde -> Rolladen-Ton 0x3B 0x0C/0x0A/0x0B (Form ROOM1131 sub02), Rolladen faehrt
 *   Nachbesserung 1 (Abnahme 0):
 *   szene1021   M1: die ECHTE 1021-Szene laeuft (Begehzone Slot 5 -> Ereignis 2 -> Programm SZENE1021), danach zeigt Roys
 *               Leiche im SELBEN Besuch msg 40 (Leichen-Zone Slot 6 = main00 @0x01E2C)
 *   abgang      M3: Johns Abgang mit Elza an sechs Lagen (die vier der Abnahme E10/E5/E6/E7 + knapp neben der Bahn + neben
 *               John): Elza wird nie verschoben, John im Bild nie naeher als 900, in seiner sichtbaren Bahn wartet der Abgang
 *   weg1131     M3: Elza geht waehrend des Wartens durch die Doppeltuer -> 1131: John ist fort, Dachtuer offen
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
#include "re15_door_seq.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_item_discard.h"
#include "re15_gameflow.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_dining_r38.h"

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

/* ---- Harness (Muster test_r37_sperre1050.c) --------------------------------------------------------------- */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static long               s_bild = 0;

typedef struct { long bild; uint8_t bank, satz; } se_t;
static se_t s_se[4096];
static int  s_n_se = 0;

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
    g_room_rdt = s_rdt;   /* der Spielweg liest den GELADENEN Raum (Anker dining_r38.c, Gegner-Wandclamp) */
    return 0;
}

static void spielbild(uint16_t held, uint16_t edge);

/* Ein Bild wie platform/pc/main.c: Wegwerf-Abfrage sichtbar -> KEIN Spielbild (re15_discard_frozen, RE2 @0x80051850),
 * aktiv -> re15_discard_tick NACH dem Spielbild (Muster test_r36_schluessel.c bf_bild). Quadrat = Bestaetigen (0x4000). */
static void frame(uint16_t held, uint16_t edge)
{
    if (!re15_discard_frozen()) spielbild(held, edge);
    else s_bild++;
    if (re15_discard_active()) re15_discard_tick((edge & RE15_PAD_BIT_SQUARE) ? (uint16_t)0x4000u : 0, 0);
}

/* John (Typ 0x49) braucht seine Bank, sonst bewegt Plc_dest ihn nicht (Muster test_r36_schluessel.c bf_bank_john /
 * test_r35_cut1150.c). Elza-Raeume laden ihn aus CDEMD1.EMS (Messlauf m1: "[elzaems] Typ 0x49 aus EMD/CDEMD1.EMS"). */
static uint8_t *s_ems = NULL;
static int      s_ems_sz = 0;
static uint8_t  s_bank[2][0x60000];
static void bank_laden(uint8_t typ, int platz)
{
    if (!s_ems) {
        char p[600]; snprintf(p, sizeof p, "%s/EMD/CDEMD1.EMS", RE15_ASSET_PSX_DIR);
        size_t n = 0; s_ems = slurp(p, &n); s_ems_sz = (int)n;
    }
    if (!s_ems) { printf("  FEHLER: CDEMD1.EMS fehlt\n"); g_fail++; return; }
    int idx = re15_ems_index_for_type(typ);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, (size_t)s_ems_sz, idx, &off, &len) != 0 || len > sizeof s_bank[0]) {
        printf("  FEHLER: Bank 0x%02X nicht im Archiv\n", typ); g_fail++; return; }
    re15_enemy_bank_t *eb = re15_enemy_alloc(typ);
    if (!eb) return;
    uint8_t *bank = s_bank[platz & 1];
    memcpy(bank, s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(bank, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(bank, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(bank, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(bank, len, &eb->skel_own, &eb->anim_own) == 0);
}
static void bank_john(void) { bank_laden(0x49, 0); }

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
        }
    }
    s_bild++;
}

/* Raumaufbau wie der Tuer-Weg (room_common.c: Schritt 7 scd_room_reenter, Schritt 11 msg-Block); die Flags bleiben
 * (ein Raumwechsel ruft scd_vm_init nicht). KEIN Bild vor der Rueckgabe — der Aufrufer prueft den Zustand, den das
 * ERSTE Bild zeigt. */
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
    if (room == RE15_R38F_RAUM_1021) { bank_john(); bank_laden(0x47, 1); }   /* Szene 1021: John + Roy (Nachbesserung 1 M1) */
    re15_discard_reset();
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_room_rdt_ok = 1;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;   /* Elza-RDT = Leon-RDT + 1 (@0x800397e4); 4 = Elza (Muster r36) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0; s_bild = 0; s_n_se = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);   /* Szenen-Clips des Raums (Plc_motion) */
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);   /* laedt nur die Ids 0..n-1, Port-Ids 40+ bleiben */
    return 0;
}

/* Spieler hinstellen (Vorwaerts-620-Punkt im Rechteck) und EINMAL Quadrat; danach bis `bilder` beobachten, Nachrichten
 * ab Bild 40 alle 15 Bilder weiterschalten. Liefert die erste gezeigte Nachricht (-1 = keine) und den Raumwechsel. */
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

static int text_dekodiert(int nr, char *out, size_t n)
{
    int len = 0;
    const uint8_t *b = re15_dining_r38_text(nr, &len);
    if (!b || len <= 0) { out[0] = 0; return 0; }
    return re15_msg_decode_text(b, (size_t)len, out, n);
}

/* Rohbytes: oeffnendes Anfuehrungszeichen 0x61, "Dining Room", schliessendes 0x19 (Form ROOM1011 msg 18 @0x1231). */
static int anfz_ok(int nr)
{
    static const uint8_t m[13] = { 0x61, 0x20, 0x45, 0x4A, 0x45, 0x4A, 0x43, 0x00, 0x2E, 0x4B, 0x4B, 0x49, 0x19 };
    int len = 0;
    const uint8_t *b = re15_dining_r38_text(nr, &len);
    for (int i = 0; b && i + 13 <= len; i++) if (!memcmp(b + i, m, 13)) return 1;
    return 0;
}

static int enthaelt_help(const char *s)
{
    for (const char *p = s; *p; p++)
        if ((p[0] == 'H' || p[0] == 'h') && !strncmp(p + 1, "elp", 3)) return 1;
    return 0;
}

/* ---- Teil: texte ------------------------------------------------------------------------------------------ */
static void teil_texte(void)
{
    printf("== texte ==\n");
    static const char *const soll[5] = {
        /* 1021/40 = msg 9 @0x024FB + Seite + NUTZER-VORGABE. Der Log-Dekoder (msg_common.c re15_msg_glyph) fuegt
         * Seiten ohne Leerzeichen an und kennt das oeffnende Anfuehrungszeichen 0x61 nicht (Font-Glyphe, ROOM1011
         * msg 18 @0x1231) — die Rohbytes prueft unten anfz_ok. */
        "He's dead.Poor Roy...There's an empty key tag on his belt. It reads Dining Room\".",
        /* Nachbesserung 1 M2: RE2 Text 0x0F @0x8009F230 (Herz-Schluessel 0x5B - 0x4C @0x800516b4) statt "The door is
         * marked "Dining Room"." — ueber der Tuer haengt im Hintergrundbild das Schild "Briefing Room" (Abnahme E2). */
        "It's locked.A heart is etched under the key hole.",
        "You've used the Dining Room Key.",
        "Elza: Are you okay? what's happened?",
        "I cannot leave him alone with those monsters.",
    };
    for (int i = 0; i < 5; i++) {
        char t[512];
        text_dekodiert(i, t, sizeof t);
        printf("    Text %d: \"%s\"\n", i, t);
        PRUEF(!strcmp(t, soll[i]), "Text %d = \"%s\"", i, soll[i]);
        PRUEF(!enthaelt_help(t), "Text %d zeigt kein \"Help\"", i);
        if (i == 0) PRUEF(anfz_ok(i), "Text %d: \"Dining Room\" in Anfuehrungszeichen (Rohbytes 61 .. 19)", i);
        if (i == 1) PRUEF(!anfz_ok(i), "Text %d: nennt kein Schild \"Dining Room\" (Nachbesserung 1 M2)", i);
    }
    {   /* Nachbesserung 1 M2: die Zeilen 2/3 des Tuertexts tragen die Glyphen von RE2 Text 0x0F @0x8009F230
         * (`.. fd 00 1d 00 44 41 3d 4e 50 .. 4e fc 50 44 41 00 47 41 55 00 44 4b 48 41 01 fe`). */
        static const uint8_t z2[23] = { 0x1D, 0x00, 0x44, 0x41, 0x3D, 0x4E, 0x50, 0x00, 0x45, 0x4F, 0x00, 0x41, 0x50, 0x3F, 0x44,
                                        0x41, 0x40, 0x00, 0x51, 0x4A, 0x40, 0x41, 0x4E };
        static const uint8_t z3[12] = { 0x50, 0x44, 0x41, 0x00, 0x47, 0x41, 0x55, 0x00, 0x44, 0x4B, 0x48, 0x41 };
        int len = 0, a = 0, c = 0;
        const uint8_t *t1 = re15_dining_r38_text(1, &len);
        for (int i = 0; t1 && i + 23 <= len; i++) if (!memcmp(t1 + i, z2, 23)) a = 1;
        for (int i = 0; t1 && i + 12 <= len; i++) if (!memcmp(t1 + i, z3, 12)) c = 1;
        PRUEF(a && c, "Text 1 = RE2 Text 0x0F @0x8009F230 (Herz-Schluessel): \"A heart is etched under\" / \"the key hole\" "
                      "glyphengleich");
    }
}

/* ---- Teil: programme -------------------------------------------------------------------------------------- */
static int programm_hat_msg(const uint8_t *p, int n, int id)
{
    for (int pc = 0; pc < n; ) {
        int sz = scd_opcode_size_at(p + pc);
        if (sz <= 0) { printf("    Opcode 0x%02X @+%04X ohne Laenge\n", p[pc], pc); return -2; }
        if (p[pc] == 0x2B && (id < 0 || p[pc + 1] == id)) return 1;
        pc += sz;
    }
    return 0;
}

static int programm_hat(const uint8_t *p, int n, const uint8_t *m, int mn)
{
    for (int i = 0; i + mn <= n; i++) if (!memcmp(p + i, m, (size_t)mn)) return 1;
    return 0;
}

static void teil_programme(void)
{
    printf("== programme ==\n");
    int nh = 0, nb = 0;
    const uint8_t *h = re15_dining_r38_programm(3, &nh);
    const uint8_t *b = re15_dining_r38_programm(4, &nb);
    PRUEF(h && nh > 0 && programm_hat_msg(h, nh, -1) == 0, "HAEMMERN (%d B) hat KEIN Message_on (Help/Klopfen nur Ton)", nh);
    PRUEF(b && nb > 0 && programm_hat_msg(b, nb, 0) == 0 && programm_hat_msg(b, nb, 1) == 0 &&
          programm_hat_msg(b, nb, 2) == 0, "BEFREIUNG (%d B) ohne msg 0 \"John, wait!\" / msg 1 \"...sorry\" / msg 2", nb);
    PRUEF(programm_hat_msg(b, nb, RE15_R38F_MSG_1141_FRAGE) == 1, "BEFREIUNG mit Port-Nachricht 40");
    {   /* Plc_dest RUN John -> JOHN_PLATZ (Form sub02 @0x00F8E `40 00 05 01`) und Elza mode 4 -> ELZA_PLATZ (@0x00CD8) */
        const int16_t jx = RE15_R38F_JOHN_PLATZ_X, jz = RE15_R38F_JOHN_PLATZ_Z;
        const int16_t ex = RE15_R38F_ELZA_PLATZ_X, ez = RE15_R38F_ELZA_PLATZ_Z;
        const uint8_t run[8]  = { 0x40, 0x00, 0x05, 0x01, (uint8_t)jx, (uint8_t)((uint16_t)jx >> 8), (uint8_t)jz, (uint8_t)((uint16_t)jz >> 8) };
        const uint8_t walk[8] = { 0x40, 0x00, 0x04, 0x00, (uint8_t)ex, (uint8_t)((uint16_t)ex >> 8), (uint8_t)ez, (uint8_t)((uint16_t)ez >> 8) };
        PRUEF(programm_hat(b, nb, run, 8), "BEFREIUNG: John RUN -> (%d,%d) = re15_dining_r38.h", jx, jz);
        PRUEF(programm_hat(b, nb, walk, 8), "BEFREIUNG: Elza WALK -> (%d,%d) = re15_dining_r38.h", ex, ez);
    }
    {   /* Tuersperre: Aot_reset Slot 0 und 2 -> Text msg 41 */
        const uint8_t s0[10] = { 0x46, 0x00, 0x01, 0x31, RE15_R38F_MSG_1141_SPERRE, 0x00, 0xFF, 0xFF, 0x00, 0x00 };
        const uint8_t s2[10] = { 0x46, 0x02, 0x01, 0x31, RE15_R38F_MSG_1141_SPERRE, 0x00, 0xFF, 0xFF, 0x00, 0x00 };
        PRUEF(programm_hat(b, nb, s0, 10) && programm_hat(b, nb, s2, 10), "BEFREIUNG: Tuersperre Slot 0 + Slot 2 -> msg 41");
    }
    PRUEF(re15_dining_r38_anker_ok() >= 0, "Anker-Abfrage laeuft");
    {   /* Nachbesserung 1 M1: SZENE1021 = `18 02` Gosub sub02 + Aot_set main00 @0x01E2C bytegleich + `01 00` */
        char rp[600]; size_t rn = 0;
        snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1021.RDT", RE15_ASSET_PSX_DIR);
        uint8_t *raw = slurp(rp, &rn);
        int ns = 0;
        const uint8_t *sz = re15_dining_r38_programm(5, &ns);
        PRUEF(raw && rn > 0x01E40 && sz && ns == 24 && sz[0] == 0x18 && sz[1] == 0x02 && !memcmp(sz + 2, raw + 0x01E2C, 20) &&
              sz[2] == 0x2C && sz[3] == 0x06 && sz[22] == 0x01 && sz[23] == 0x00,
              "SZENE1021 (%d B) = Gosub sub02 + Aot_set Slot 6 = ROOM1021 main00 @0x01E2C bytegleich + Evt_end", ns);
        free(raw);
    }
    {   /* Nachbesserung 1 M3: Abgang — Doppeltuer frei, While (Elza in Johns sichtbarer Bahn), Pad-Sperre, Cut 7,
         * Work_set(2,0) + Member_set(6, JOHN_WORT0), dann erst Johns RUN. */
        const int16_t g[4] = { RE15_R38F_BAHN_X_MIN, RE15_R38F_BAHN_X_MAX, RE15_R38F_BAHN_Z_MIN, RE15_R38F_BAHN_Z_MAX };
        const uint8_t id[4] = { 0, 0, 2, 2 }, opr[4] = { 2, 4, 2, 4 };
        uint8_t w[4 + 30 + 4]; int k = 0;
        w[k++] = 0x0F; w[k++] = 30; w[k++] = 34; w[k++] = 0;
        for (int i = 0; i < 4; i++) {
            if (i) { w[k++] = 0; w[k++] = 0; }
            w[k++] = 0x3E; w[k++] = 0; w[k++] = id[i]; w[k++] = opr[i];
            w[k++] = (uint8_t)((uint16_t)g[i] & 0xFF); w[k++] = (uint8_t)((uint16_t)g[i] >> 8);
        }
        w[k++] = 0x02; w[k++] = 0x00; w[k++] = 0x10; w[k++] = 0x00;
        const uint8_t ms[8] = { 0x2E, 0x02, 0x00, 0x00, 0x34, 0x06, (uint8_t)(RE15_R38F_JOHN_WORT0 & 0xFF),
                                (uint8_t)(RE15_R38F_JOHN_WORT0 >> 8) };
        const uint8_t tuer0[4] = { 0x46, 0x00, 0x02, 0x31 }, sperre[4] = { 0x22, 0x02, 0x07, 0x01 }, cut7[2] = { 0x29, 0x07 };
        const uint8_t warte_f2[4] = { 0x21, 0x04, 0xF2, 0x01 };
        int iw = -1, im = -1, it = -1, isp = -1, ic = -1, if2 = -1, irun = -1;
        for (int i = 0; i + (int)sizeof w <= nb && iw < 0; i++) if (!memcmp(b + i, w, sizeof w)) iw = i;
        for (int i = 0; i + 4 <= nb; i++) if (!memcmp(b + i, warte_f2, 4)) if2 = i;
        for (int i = (if2 > 0 ? if2 : 0); i + 4 <= nb && it < 0; i++) if (!memcmp(b + i, tuer0, 4)) it = i;
        for (int i = (iw > 0 ? iw : 0); i + 4 <= nb && isp < 0; i++) if (!memcmp(b + i, sperre, 4)) isp = i;
        for (int i = (isp > 0 ? isp : 0); i + 2 <= nb && ic < 0; i++) if (!memcmp(b + i, cut7, 2)) ic = i;
        for (int i = (ic > 0 ? ic : 0); i + 8 <= nb && im < 0; i++) if (!memcmp(b + i, ms, 8)) im = i;
        for (int i = (im > 0 ? im : 0); i + 4 <= nb && irun < 0; i++) if (b[i] == 0x40 && b[i + 1] == 0 && b[i + 2] == 5) irun = i;
        printf("    BEFREIUNG Abgang: Gorillas-Warten @+%04X, Tuer 0 frei @+%04X, While @+%04X, Pad-Sperre @+%04X, Cut 7 @+%04X, "
               "Member_set 6 @+%04X, RUN @+%04X\n", if2, it, iw, isp, ic, im, irun);
        PRUEF(if2 > 0 && it > if2 && iw > it, "Doppeltuer -> ROOM1131 frei NACH dem Tod der Gorillas, VOR dem Warten auf Johns Bahn");
        PRUEF(iw > 0, "While (Elza x %d..%d, z %d..%d) mit vier Member_cmp (UND) = re15_dining_r38.h", g[0], g[1], g[2], g[3]);
        PRUEF(isp > iw && ic > isp && im > ic && irun > im,
              "danach Pad-Sperre, Cut 7, John Member_set(6, 0x%04X) (Bit 0x2: kein Koerper-Schub), erst dann RUN",
              RE15_R38F_JOHN_WORT0);
        PRUEF(RE15_R38F_BAHN_X_MIN == -7550 - RE15_R38F_KOERPER_R && RE15_R38F_BAHN_X_MAX == RE15_R38F_GANG_X + RE15_R38F_KOERPER_R &&
              RE15_R38F_BAHN_Z_MIN == -21500 - RE15_R38F_KOERPER_R &&
              RE15_R38F_BAHN_Z_MAX == RE15_R38F_CUT7_AUS_Z + RE15_R38F_KOERPER_R && RE15_R38F_KOERPER_R == 450 + 450,
              "Bahn = Ecke sub02 @0x00F8E (-7550,-21500) .. Gang-Ausgang x %d, +-%d (450 + 450), Nordrand Cut-7-Bildgrenze %d + %d",
              RE15_R38F_GANG_X, RE15_R38F_KOERPER_R, RE15_R38F_CUT7_AUS_Z, RE15_R38F_KOERPER_R);
    }
}

/* ---- Teil: leiche1021 ------------------------------------------------------------------------------------- */
static void teil_leiche1021(void)
{
    printf("== leiche1021 ==\n");
    re15_game_state_init();
    re15_game_flag_set(RE15_R38F_ROY_BANK, RE15_R38F_ROY_BIT, 1);   /* (3,0x5F): Szene 1021 gelaufen, Leiche liegt */
    if (room_boot(RE15_R38F_RAUM_1021, -3000, -9000, 0, 0) != 0) return;
    for (int f = 0; f < 30; f++) frame(0, 0);
    PRUEF((re15_dining_r38_anker_ok() & 4) != 0, "ROOM1021 sub06 @0x021EC bytegleich (Anker)");
    scd_event_fire(RE15_R38F_1021_EREIGNIS);   /* = Druck auf Roys Leiche (Aot Slot 6 sce 3, main00 @0x01E2C) */
    int gezeigt = -1;
    for (int f = 0; f < 200 && gezeigt < 0; f++) { frame(0, 0); if (g_scd.message_active) gezeigt = (int)g_scd.message_id; }
    PRUEF(gezeigt == RE15_R38F_MSG_1021_LEICHE, "Leiche -> msg %d (soll 40)", gezeigt);
    const char *t = re15_msg_get_text(RE15_R38F_MSG_1021_LEICHE);
    printf("    msg 40: \"%s\"\n", t ? t : "(keiner)");
    PRUEF(t && strstr(t, "There's an empty key tag on his belt.") && strstr(t, "It reads Dining Room\"."),
          "msg 40 traegt den Hinweis (NUTZER-VORGABE)");
}

/* ---- Teil: schluessel1011 --------------------------------------------------------------------------------- */
static int prop_da(int obj_id, int32_t *x, int32_t *z)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_id == obj_id) { if (x) *x = g_scd.props[k].x; if (z) *z = g_scd.props[k].z; return 1; }
    return 0;
}

static void teil_schluessel1011(void)
{
    printf("== schluessel1011 ==\n");
    int32_t x = 0, z = 0;
    re15_game_state_init();
    if (room_boot(RE15_R38F_RAUM_1011, 0, 0, 0, 0) != 0) return;
    PRUEF(!prop_da(RE15_R38F_1011_OBJ, NULL, NULL) && !g_aot.slots[RE15_R38F_1011_SLOT].active,
          "vor der 1021-Szene ((3,0x5F)=0): kein Schluessel");
    /* Stand nach der 1021-Szene (E13, ablauf_elza.md): erster Besuch 1011 erledigt (4,0xF1)=1 (sub02 @0x00AFE), Roy
     * liegt nicht mehr ((4,0xFA)=0, ROOM1211 sub02 @0x01F7A), Prison key genommen (4,0xEC)=1 / (3,0x78)=1 (sub06
     * @0x00E2C/@0x00E30 — damit setzt sub00 @0x00A78 Slot 5 auf sub07), Szene 1021 gelaufen (3,0x5F)=1 (4,0xF0)=1. */
    re15_game_state_init();
    re15_game_flag_set(4, 0xF1, 1);
    re15_game_flag_set(4, 0xEC, 1);
    re15_game_flag_set(3, 0x78, 1);
    re15_game_flag_set(RE15_R38F_F0_BANK, RE15_R38F_F0_BIT, 1);
    re15_game_flag_set(RE15_R38F_ROY_BANK, RE15_R38F_ROY_BIT, 1);
    if (room_boot(RE15_R38F_RAUM_1011, RE15_R38F_KEY_X - 620, RE15_R38F_KEY_Z, 0, 0) != 0) return;
    {
        const re15_aot_t *s5 = &g_aot.slots[5];
        printf("    Slot 5 (Roy, sub00 @0x00A78) aktiv %d Typ %d Ereignis %d Mitte (%d,%d) halb (%d,%d)\n", s5->active, s5->type,
               s5->event_id, s5->x, s5->z, s5->half_w, s5->half_h);
        int roy = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == 0x47) roy = 1;
        PRUEF(!roy, "Roy liegt nicht mehr in 1011 (kein Spawn 0x47 bei (4,0xFA)=0)");
    }
    PRUEF(prop_da(RE15_R38F_1011_OBJ, &x, &z) && x == RE15_R38F_KEY_X && z == RE15_R38F_KEY_Z,
          "nach der Szene: Prop obj %d bei (%d,%d) = Roys Liegestelle sub00 @0x009F6", RE15_R38F_1011_OBJ, x, z);
    const re15_aot_t *a = &g_aot.slots[RE15_R38F_1011_SLOT];
    PRUEF(a->active && a->type == RE15_AOT_TYPE_ITEM, "Zone Slot %d = Item", RE15_R38F_1011_SLOT);
    /* Aufnahme: Vorwaerts-620-Punkt auf der Liegestelle, Quadrat, Abfrage "ja" (Quadrat) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 20; f++) frame(0, 0);
    pl->x = RE15_R38F_KEY_X + 620; pl->z = RE15_R38F_KEY_Z; pl->rot_y = 2048;   /* von Osten, Blick -X: Vorwaerts-Punkt = Liegestelle */
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    printf("    Spieler (%d,%d) Blick %d st=%d pm=%d; Slot %d aktiv %d Typ %d Mitte (%d,%d) halb (%d,%d) sat 0x%02X inside %d\n",
           pl->x, pl->z, pl->rot_y, pl->state, g_scd.player_mode, RE15_R38F_1011_SLOT, a->active, a->type, a->x, a->z,
           a->half_w, a->half_h, a->sce_flags, a->was_inside);
    {   /* Aufnahme-Modal wie test_r36_schluessel.c (D): bis zur Abfrage, dann "ja" (0x4000) */
        uint8_t typ = 0; int wahl = -1, w = 0;
        const int modal = re15_item_modal_active();
        while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800) re15_item_modal_tick(0, 0);
        re15_item_modal_prompt(&typ, &wahl);
        PRUEF(modal && typ == RE15_R38F_ITEM, "Druck auf die Liegestelle: Aufnahme-Modal Item 0x%02X (ist 0x%02X)", RE15_R38F_ITEM, typ);
        w = 0;
        while (re15_item_modal_active() && w++ < 1600) re15_item_modal_tick((uint16_t)0x4000u, 0);
    }
    PRUEF(re15_inv_find_item(RE15_R38F_ITEM) >= 0, "Item 0x%02X (Dining Room Key) im Inventar", RE15_R38F_ITEM);
    PRUEF(re15_game_flag_get(9, RE15_R38F_BIT_GENOMMEN) == 1, "(9,%d) = 1", RE15_R38F_BIT_GENOMMEN);
    re15_game_state_t g = g_game;
    if (room_boot(RE15_R38F_RAUM_1011, 0, 0, 0, 0) != 0) return;
    g_game = g;
    PRUEF(!prop_da(RE15_R38F_1011_OBJ, NULL, NULL), "Wiederbetreten nach der Aufnahme: kein Schluessel mehr");
}

/* ---- Teil: tuer1131 --------------------------------------------------------------------------------------- */
static int inv_geben(uint8_t id)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == 0) { g_inv.slots[i].id = id; g_inv.slots[i].qty = 1; return i; }
    return -1;
}

static void teil_tuer1131(void)
{
    printf("== tuer1131 ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    /* Tuer Slot 0 main00 @0x0088E: Rechteck x -800..200, z -15900..-11900 -> Vorwaerts-Punkt (-300,-13900), Blick +X */
    if (room_boot(RE15_R38F_RAUM_1131, -920, -13900, 0, 0) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    PRUEF(re15_dining_r38_tuer_gesperrt() == 1, "Slot 0 (Tuer -> 1141) gesperrt");
    unsigned zu0 = g_re15_r38f_zu_zaehler;
    druck_t r = druecke(-920, -13900, 0, 200);
    PRUEF(r.msg_id == RE15_R38F_MSG_1131_ZU && !r.raumwechsel && g_re15_r38f_zu_zaehler == zu0 + 1,
          "ohne Schluessel: msg %d (soll 40 \"It's locked...\"), kein Raumwechsel", r.msg_id);
    for (int f = 0; f < 120; f++) { uint16_t e = (g_scd.message_active && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
    inv_geben(RE15_R38F_ITEM);
    r = druecke(-920, -13900, 0, 300);
    PRUEF(r.msg_id == RE15_R38F_MSG_1131_BENUTZT && !r.raumwechsel, "mit Schluessel: msg %d (soll 41 \"You've used...\")",
          r.msg_id);
    for (int f = 0; f < 300; f++) { uint16_t e = (g_scd.message_active && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
    PRUEF(re15_game_flag_get(9, RE15_R38F_BIT_TUER) == 1, "(9,%d) = 1 (Tuer aufgeschlossen)", RE15_R38F_BIT_TUER);
    {   /* Wegwerf-Abfrage wie Runde 36 Spur Q (RE2 @0x80051670 -> Abfrage-FSM): bestaetigen */
        const int frage = re15_discard_active();
        for (int w = 0; w < 900 && re15_discard_active(); w++) {
            uint16_t e = ((w % 10) == 0) ? RE15_PAD_BIT_SQUARE : 0;
            frame(e, e);
        }
        PRUEF(frage && !re15_discard_active(), "Wegwerf-Abfrage nach dem Aufschliessen gestellt und beantwortet");
        PRUEF(re15_inv_find_item(RE15_R38F_ITEM) < 0, "Dining Room Key weggeworfen (Ja)");
        for (int f = 0; f < 30; f++) frame(0, 0);
    }
    {
        const re15_aot_t *t0 = &g_aot.slots[0];
        printf("    Slot 0 aktiv %d Typ %d Mitte (%d,%d) halb (%d,%d) sat 0x%02X inside %d hold %d; pm=%d msg=%d\n", t0->active,
               t0->type, t0->x, t0->z, t0->half_w, t0->half_h, t0->sce_flags, t0->was_inside, t0->door_hold,
               g_scd.player_mode, g_scd.message_active);
        printf("    Tuer-Parameter: Raum 0x%02X Stage %d Band %d (Spieler-Band %d) Spawn (%d,%d,%d) Cut %d; Spieler (%d,%d) st %d\n",
               g_aot.door_params[0].dest_room, g_aot.door_params[0].dest_stage, g_aot.door_params[0].band,
               re15_collision_debug_band(), g_aot.door_params[0].spawn_x, g_aot.door_params[0].spawn_y,
               g_aot.door_params[0].spawn_z, g_aot.door_params[0].target_cut, g_actors[0].x, g_actors[0].z, g_actors[0].state);
        printf("    Nachricht: query %d display_frames %d fsm %d active %d; Wegwerf aktiv %d frozen %d; Modal %d; pf 0x%08X\n",
               (int)g_scd.message_query, (int)g_scd.message_display_frames, (int)g_scd.message_fsm_active,
               (int)g_scd.message_active, re15_discard_active(), re15_discard_frozen(), re15_item_modal_active(),
               (unsigned)g_re15_pauseflags);
    }
    r = druecke(-920, -13900, 0, 200);
    PRUEF(r.raumwechsel && r.ziel == 0x1141, "danach: Raumwechsel nach ROOM%04X (soll 1141)", (unsigned)r.ziel);
}

/* ---- Teil: phase_h ---------------------------------------------------------------------------------------- */
static void projiziere(const re15_camera_view_t *v, int vx, int vy, int vz, int *sx, int *sy, int *z)
{
    const int32_t *r = v->rot, *t = v->trans;
    int32_t x = (int32_t)(((int64_t)vx*r[0] + (int64_t)vy*r[1] + (int64_t)vz*r[2]) >> 12) + t[0];
    int32_t y = (int32_t)(((int64_t)vx*r[3] + (int64_t)vy*r[4] + (int64_t)vz*r[5]) >> 12) + t[1];
    int32_t zz = (int32_t)(((int64_t)vx*r[6] + (int64_t)vy*r[7] + (int64_t)vz*r[8]) >> 12) + t[2];
    *z = zz;
    if (zz <= 0) { *sx = -9999; *sy = -9999; return; }
    *sx = 160 + (int)((int64_t)x * v->fov_screen_dist / zz);
    *sy = 120 + (int)((int64_t)y * v->fov_screen_dist / zz);
}

/* Sichtbar im Bild des Cuts: irgendein Punkt der Figur (Fuss/Huefte/Kopf) projiziert ins 320x240-Bild. */
static int sichtbar(int cut, const re15_actor_t *a)
{
    if (cut < 0 || cut >= (int)s_rdt.cut_count) return 0;
    re15_camera_view_t v;
    if (re15_camera_build_view(&s_rdt.cuts[cut], &v) != 0) return 0;
    static const int hoehe[3] = { 0, -900, -1750 };
    for (int i = 0; i < 3; i++) {
        int sx, sy, z;
        projiziere(&v, a->x, a->y + hoehe[i], a->z, &sx, &sy, &z);
        if (z > 0 && sx >= 0 && sx < 320 && sy >= 0 && sy < 240) return 1;
    }
    return 0;
}

static void teil_phase_h(void)
{
    printf("== phase_h ==\n");
    /* Runde 39 Spur J (AUFTRAG Runde 39, ROOM1141 Punkte 1-8, Nutzer 2026-10-17): John sitzt NICHT mehr im Dachgang,
     * sondern kommt nach dem Rolladen durch die Dachtuer (Kamera vorne auf Elza, Tuer-Toene, Kamera auf die Tuer); statt
     * Krachen/Stoehnen (0x3C) und "Help me out." (0x3D) laufen Klopfen + help01..05, dazu Elzas Reaktion msg 50. Der
     * Ablauf dieses Teils ist damit vom Nutzer ersetzt; den neuen Ablauf samt der weiter gueltigen Pruefungen dieses Teils
     * (Rolladen-Ton, Szene ohne msg 0/1/2, Sperre msg 41 an beiden Tueren, Ecke, Abgang in Cut 7, Riegel, Dachtuer ->
     * 1171, Wiederbetreten) prueft unit_r39_dining1141_phase_h. */
    printf("  ok: ersetzt durch unit_r39_dining1141_phase_h (Runde 39 Spur J, Nutzer-Punkte 1-8)\n");
    if (1) return;
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_R38F_F0_BANK, RE15_R38F_F0_BIT, 1);     /* (4,0xF0): ROOM1021 sub02 @0x01FE0 */
    /* Eintritt durch die Doppeltuer wie aus ROOM1131 (Spawn main00 @0x0088E: (-7600,0,-17600), Cut 0) */
    if (room_boot(RE15_R38F_RAUM_1141, -7600, -17600, 0, 0) != 0) return;
    re15_actor_t *john = &g_actors[1];
    /* VOR dem ersten Bild: John steht schon im Dachgang hinter dem Rolladen, die Gorillas sind nicht da. */
    PRUEF(re15_dining_r38_phase_h() == 1, "Phase H erkannt ((4,0xF0)=1, (3,0x6A)=0, Anker bytegleich)");
    PRUEF(john->active && john->type == RE15_R38F_JOHN_TYP && john->x == RE15_R38F_JOHN_GANG_X &&
          john->z == RE15_R38F_JOHN_GANG_Z && john->rot_y == RE15_R38F_JOHN_GANG_YAW,
          "Bild 0: John (Typ 0x%02X) bei (%d,%d) Blick %d = hinter dem Rolladen", john->type, john->x, john->z, john->rot_y);
    PRUEF(john->z > -6700 && john->x > -8600 && john->x < -5750, "  ... im Dachgang (SCA x -8600..-5750, z -6700..-250)");
    PRUEF(!g_actors[2].active && !g_actors[3].active, "keine Gorillas in Phase H");
    /* 600 Bilder Phase H: Ton-Takt, keine Nachricht, John bleibt stehen */
    int msg_gesehen = 0, john_weg = 0;
    for (int f = 0; f < 600; f++) {
        frame(0, 0);
        if (g_scd.message_active) msg_gesehen++;
        if (abs(john->x - RE15_R38F_JOHN_GANG_X) > 60 || abs(john->z - RE15_R38F_JOHN_GANG_Z) > 60) john_weg++;
    }
    PRUEF(msg_gesehen == 0, "600 Bilder Phase H: keine Nachricht (Help/Klopfen nur Ton)");
    PRUEF(john_weg == 0, "John bleibt hinter dem Rolladen stehen (%d Bilder abgewichen)", john_weg);
    {   /* Sichtbarkeit in Phase H: in KEINEM Cut ausser 5 (dort deckt ihn der geschlossene Rolladen, Bildbeleg m1 F900/F930)
         * und 6 (Kamera IM Dachgang, nur aktiv wenn der Spieler im Gang steht — der Rolladen sperrt ihn aus). */
        int sicht = 0;
        for (int c = 0; c < (int)s_rdt.cut_count; c++)
            if (c != 5 && c != 6 && sichtbar(c, john)) { sicht++; printf("    John im Bild von Cut %d\n", c); }
        PRUEF(sicht == 0, "Phase H: John in keinem Cut ausser 5 (Rolladen davor) und 6 (Dachgang) im Bild");
    }
    {
        static const uint8_t muster[4][2] = { { 0x3C, 0x0A }, { 0x3C, 0x0B }, { 0x3C, 0x0B }, { 0x3D, 0x00 } };
        static const int abstand[4] = { 90, 30, 30, 90 };   /* vor Zeile k: Sleep 90 (@+0x86), 30, 30, 90 */
        int n = 0, form = 1, takt = 1;
        for (int i = 0; i < s_n_se; i++) {
            if (s_se[i].bank != 0x3C && s_se[i].bank != 0x3D) continue;
            const int k = n % 4;
            if (s_se[i].bank != muster[k][0] || s_se[i].satz != muster[k][1]) form = 0;
            if (n > 0) {
                long d = s_se[i].bild - s_se[i - 1].bild;
                /* +-1: die erste Schlafzeile nach dem Raumaufbau zaehlt das Init-Bild mit (gemessen 29 statt 30) */
                if (d < abstand[k] - 1 || d > abstand[k] + 1) { takt = 0; printf("    Abstand vor Ton %d: %ld (soll %d)\n", n, d, abstand[k]); }
            }
            printf("    Bild %4ld Se_on(0x%02X,0x%02X)\n", s_se[i].bild, s_se[i].bank, s_se[i].satz);
            n++;
        }
        PRUEF(n >= 9 && form, "Klopfen 0x3C 0A/0B/0B + Hilferuf 0x3D 00 im Wechsel (%d Toene)", n);
        PRUEF(takt, "Takt 30/30/90/90 Bilder = RE2 ROOM1060 sub05 Sleep 30/30/90/90");
    }
    /* Gemaelde: Vorwaerts-Punkt in Aot slot 3 (x -9000..-8000, z -11000..-9500), Blick -X; Quadrat, Antwort "ja" */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -8300; pl->z = -10250; pl->rot_y = 2048;
    frame(0, 0);
    const int se_vorher = s_n_se;
    long ja_bild = -1;
    for (int f = 0; f < 600 && ja_bild < 0; f++) {
        uint16_t e = (f == 0 || (g_scd.message_active && (f % 20) == 0)) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT)) ja_bild = s_bild;
    }
    PRUEF(ja_bild >= 0, "Gemaelde-Schalter: \"ja\" -> (3,0x6A) = 1 (sub06 @0x00FF6) in Bild %ld", ja_bild);
    /* Szene bis zur Rueckgabe der Steuerung beobachten */
    int msgs[64], n_msg = 0, last = -1, sprung = 0, max_schritt = 0;
    int32_t jx = john->x, jz = john->z;
    int john_heraus = 0, kam_aus_gang = 0, pm2 = 0;
    long frei_bild = -1;
    for (int f = 0; f < 4000; f++) {
        frame(0, 0);
        if (g_scd.message_active && (int)g_scd.message_id != last) { last = (int)g_scd.message_id; if (n_msg < 64) msgs[n_msg++] = last; }
        if (!g_scd.message_active) last = -1;
        const int dx = john->x - jx, dz = john->z - jz;
        const int schritt = abs(dx) + abs(dz);
        if (schritt > max_schritt) max_schritt = schritt;
        if (schritt > 400) { sprung++; printf("    Sprung Johns in Bild %ld: (%d,%d) -> (%d,%d)\n", s_bild, jx, jz, john->x, john->z); }
        if (john->z < -7200 && !john_heraus) { john_heraus = 1; kam_aus_gang = (jz > -7300); }
        jx = john->x; jz = john->z;
        if (g_scd.player_mode == 2) pm2 = 1;
        if (pm2 && g_scd.player_mode == 0 && !g_scd.message_active && frei_bild < 0 && f > 200) { frei_bild = s_bild; break; }
    }
    printf("    Nachrichten:"); for (int i = 0; i < n_msg; i++) printf(" %d", msgs[i]); printf("\n");
    int hat0 = 0, hat1 = 0, hat2 = 0, i40 = -1, i11 = -1;
    for (int i = 0; i < n_msg; i++) { if (msgs[i] == 0) hat0 = 1; if (msgs[i] == 1) hat1 = 1; if (msgs[i] == 2) hat2 = 1;
                                      if (msgs[i] == 40 && i40 < 0) i40 = i; if (msgs[i] == 11) i11 = i; }
    PRUEF(!hat0 && !hat1 && !hat2, "Szene ohne msg 0 \"John, wait!\", msg 1 \"...sorry\", msg 2 \"Same here...\"");
    PRUEF(i40 >= 0 && i11 > i40, "Szene: msg 40 \"Are you okay? What's happened?\" ... msg 11 (Rest wie Original)");
    PRUEF(sprung == 0, "John springt nie (groesster Schritt %d je Bild)", max_schritt);
    PRUEF(john_heraus && kam_aus_gang, "John laeuft aus dem Dachgang in den Raum (kein Auftauchen)");
    PRUEF(frei_bild > 0, "Steuerung nach der Szene zurueck (Bild %ld)", frei_bild);
    {
        int r0 = -1, r1 = -1, r2 = -1, klopf_danach = 0;
        for (int i = se_vorher; i < s_n_se; i++) {
            if (s_se[i].bank == 0x3B && s_se[i].satz == 0x0C && r0 < 0) r0 = i;
            if (s_se[i].bank == 0x3B && s_se[i].satz == 0x0A && r1 < 0) r1 = i;
            if (s_se[i].bank == 0x3B && s_se[i].satz == 0x0B && r2 < 0) r2 = i;
            if ((s_se[i].bank == 0x3C || s_se[i].bank == 0x3D) && s_se[i].bild > ja_bild) klopf_danach++;
        }
        PRUEF(r0 >= 0 && r1 > r0 && r2 > r1, "Rolladen-Ton 0x3B 0x0C, 0x0A, 0x0B in der Folge von ROOM1131 sub02");
        PRUEF(klopf_danach == 0, "nach dem \"ja\" kein Klopfen/Hilferuf mehr (%d)", klopf_danach);
    }
    /* Die Steuerung kommt @0x00F4E zurueck, John laeuft erst danach (@0x00F8E RUN + @0x00F98 TURN) in die Ecke.
     * Die Gorillas fuer den Riegel einfrieren (grid 0x20 wie Member_set 0x0C=0x30, @0x800410b8), damit kein Griff den
     * Spieler aus den folgenden Drucken nimmt. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) g_actors[s].grid_id |= 0x20;
    for (int f = 0; f < 900 && !(abs(john->x - (-7550)) < 400 && abs(john->z - (-21500)) < 400); f++) {
        const int dx = john->x - jx, dz = john->z - jz;
        if (abs(dx) + abs(dz) > 400) sprung++;
        jx = john->x; jz = john->z;
        frame(0, 0);
    }
    PRUEF(sprung == 0, "John laeuft ohne Sprung in die Ecke");
    PRUEF(abs(john->x - (-7550)) < 400 && abs(john->z - (-21500)) < 400, "John in der Ecke (-7550,-21500) (sub02 @0x00F8E): (%d,%d)",
          john->x, john->z);
    /* Tuersperre solange die Gorillas leben */
    PRUEF(re15_game_flag_get(RE15_R38F_F2_BANK, RE15_R38F_F2_BIT) == 1, "(4,0xF2) = 1: Gorillas leben");
    PRUEF(g_aot.slots[0].active && g_aot.slots[0].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[0].event_id == RE15_R38F_MSG_1141_SPERRE &&
          g_aot.slots[2].active && g_aot.slots[2].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[2].event_id == RE15_R38F_MSG_1141_SPERRE,
          "Slot 0 (Doppeltuer) und Slot 2 (Dachtuer/Riegel) zeigen msg 41");
    {   /* Druck auf die Doppeltuer: Rechteck x -9100..-8100, z -19750..-15750 -> Vorwaerts-Punkt (-8600,-17750), Blick -X */
        re15_actor_t *g1 = &g_actors[2], *g2 = &g_actors[3];
        g1->grid_id |= 0x20; g2->grid_id |= 0x20;   /* Gorillas fuer den Druck einfrieren (Member_set 0x0C, @0x800410b8) */
        druck_t r = druecke(-8600 + 620, -17750, 2048, 200);
        PRUEF(r.msg_id == RE15_R38F_MSG_1141_SPERRE && !r.raumwechsel,
              "Doppeltuer waehrend des Kampfs: msg %d \"I cannot leave him alone with those monsters.\", kein Raumwechsel", r.msg_id);
        for (int f = 0; f < 200; f++) { uint16_t e = (g_scd.message_active && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
        r = druecke(-7250, -250 - 620, 3072, 200);   /* Dachtuer: Vorwaerts-Punkt (-7250,-250), Blick +Z */
        PRUEF(r.msg_id == RE15_R38F_MSG_1141_SPERRE && !r.raumwechsel, "Dachtuer waehrend des Kampfs: msg %d, kein Raumwechsel",
              r.msg_id);
        for (int f = 0; f < 200; f++) { uint16_t e = (g_scd.message_active && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
    }
    /* Gorillas tot (wie RE15_R38F_GORILLAS_TOT: Kill-Flags pc[7] 0xCB/0xCC in Bank 7) */
    pl->x = -3000; pl->z = -9000; pl->rot_y = 0;   /* Elza mitten im Raum, nicht in Johns Bahn */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) re15_actor_free(s);
    re15_game_flag_set(7, 0xCB, 1);
    re15_game_flag_set(7, 0xCC, 1);
    int park_bild = -1, park_cut = -1, vorher_sichtbar = 0, war_sichtbar_cut7 = 0;
    int tuer_auf = 0, tuer_zu = 0;
    int cut_vor7 = -1, cut7_gesehen = 0;   /* Fortsetzung 4 (Dossier 4.3): angezeigter Cut unmittelbar vor Cut 7 */
    const int se_abgang = s_n_se;
    jx = john->x; jz = john->z; sprung = 0;
    for (int f = 0; f < 3000; f++) {
        const int sicht = john->active ? sichtbar(s_shown, john) : 0;
        if (!cut7_gesehen) { if (s_shown == 7) cut7_gesehen = 1; else cut_vor7 = s_shown; }
        frame(0, 0);
        if (s_shown == 7 && john->x > -29000 && sichtbar(7, john)) war_sichtbar_cut7 = 1;
        if (john->x <= -29000 && park_bild < 0) { park_bild = (int)s_bild; park_cut = s_shown; vorher_sichtbar = sicht; }
        else if (john->x > -29000) {
            if (abs(john->x - jx) + abs(john->z - jz) > 400) sprung++;
            jx = john->x; jz = john->z;
        }
        if (park_bild >= 0 && g_scd.player_mode == 0 && f > park_bild + 40) break;
    }
    for (int i = se_abgang; i < s_n_se; i++) {
        if (s_se[i].bank == 0x0E && s_se[i].satz == 0) tuer_auf++;
        if (s_se[i].bank == 0x0E && s_se[i].satz == 1) tuer_zu++;
    }
    PRUEF(park_bild > 0, "Abgang: John geparkt in Bild %d", park_bild);
    PRUEF(park_cut == 7, "  ... waehrend Cut 7 (Blick nach Sueden) aktiv ist (Cut %d)", park_cut);
    PRUEF(!vorher_sichtbar, "  ... und im Bild davor NICHT im Bild (kein Verschwinden im Bild)");
    PRUEF(war_sichtbar_cut7, "  ... vorher lief er sichtbar durch Cut 7 (verlaesst das Bild)");
    PRUEF(sprung == 0, "  ... ohne Sprung bis zum Parken");
    PRUEF(tuer_auf >= 1 && tuer_zu >= 1, "Dachtuer-Toene DOOR1A auf (%d) / zu (%d) im Off", tuer_auf, tuer_zu);
    PRUEF(re15_game_flag_get(9, RE15_R38F_BIT_JOHN_WEG) == 1, "(9,%d) = 1", RE15_R38F_BIT_JOHN_WEG);
    PRUEF(re15_game_flag_get(4, 0xC3) == 1, "Riegel offen (4,0xC3) = 1 (John hat ihn beim Hinausgehen geoeffnet)");
    PRUEF(g_aot.slots[1].active && g_aot.slots[1].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[1].dest_room == 0x17,
          "Slot 1 = Dachtuer -> Raum 0x%02X (soll 0x17 = ROOM1171)", g_aot.door_params[1].dest_room);
    PRUEF(g_aot.slots[0].active && g_aot.slots[0].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[0].dest_room == 0x13,
          "Slot 0 = Doppeltuer -> Raum 0x%02X (soll 0x13 = ROOM1131)", g_aot.door_params[0].dest_room);
    PRUEF(g_scd.player_mode == 0, "Steuerung frei");
    /* Fortsetzung 4 (Dossier 4.3): Cut 7 ist eine reine Szenen-Kamera (RVD ROOM1141 Gruppe 7 = nur der Anker, Satz 21) —
     * das Programm muss mit Cut_old + Cut_auto enden (Form ROOM1150 @0x010F2 `2a 00 3c 01`), sonst bleibt die Kamera auf
     * Cut 7 stehen und Elza laeuft aus dem Bild (Lauf int_d3). */
    for (int f = 0; f < 5; f++) frame(0, 0);
    PRUEF(cut7_gesehen && cut_vor7 >= 0 && cut_vor7 != 7 && s_shown == cut_vor7 && g_scd.cut_auto_enabled,
          "Kamera nach dem Abgang zurueck auf Cut %d (vor Cut 7 angezeigt, jetzt Cut %d), Kamera-Automatik an (%d)",
          cut_vor7, s_shown, (int)g_scd.cut_auto_enabled);
    {
        druck_t r = druecke(-7250, -250 - 620, 3072, 200);
        PRUEF(r.raumwechsel && r.ziel == 0x1171, "Dachtuer -> ROOM%04X (soll 1171)", (unsigned)r.ziel);
    }
    /* Wiederbetreten: kein John, Tueren normal */
    re15_game_state_t g = g_game;
    if (room_boot(RE15_R38F_RAUM_1141, -7600, -17600, 0, 0) != 0) return;
    g_game = g;
    for (int f = 0; f < 30; f++) frame(0, 0);
    int john_da = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == RE15_R38F_JOHN_TYP) john_da = 1;
    PRUEF(!john_da && re15_dining_r38_phase_h() == 0, "Wiederbetreten: kein John, keine Phase H");
}

/* ---- Teil: szene1021 (Nachbesserung 1 M1) ------------------------------------------------------------------------
 * Abnahme 0 E1a: nach der ECHTEN 1021-Szene liess sich Roys Leiche nicht untersuchen — die Zone Slot 6 entsteht im Original
 * nur beim Raumaufbau (main00 @0x01E24 Ck(3,0x5F) -> @0x01E2C). Hier laeuft die Szene selbst (Stand vor der Szene, Ausloese-
 * Zone Slot 5 main00 @0x01DE2 -> Ereignis 2); danach muss der Druck auf die Leiche im SELBEN Besuch msg 40 zeigen. */
static void teil_szene1021(void)
{
    printf("== szene1021 ==\n");
    re15_game_state_init();
    /* Stand vor der Szene (ablauf_elza.md E12, wie Abnahme E1a): (4,0x60) Szene offen (main00 @0x01DDE), (3,0xC9) = 1 -> keine
     * Zombies im Raumaufbau (main00 @0x01E68), die Szene setzt sie @0x01FDC zurueck. */
    re15_game_flag_set(4, 0x60, 1);
    re15_game_flag_set(3, 0xC9, 1);
    if (room_boot(RE15_R38F_RAUM_1021, -15500, -7100, 0, 0) != 0) return;
    PRUEF((re15_dining_r38_anker_ok() & 64) != 0, "ROOM1021 sub02..sub05 @0x01FD8 und main00 @0x01DE2..@0x01E40 bytegleich (Anker)");
    PRUEF(!g_aot.slots[6].active, "vor der Szene: keine Leichen-Zone Slot 6 ((3,0x5F) = 0)");
    for (int f = 0; f < 10; f++) frame(0, 0);
    /* in die Begehzone Slot 5 (x -13100..-11000, z -9100..-5100) gehen wie der Spieler: hinstellen, Bilder laufen */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -12050; pl->z = -7100; pl->rot_y = 0;
    int szene = 0, frei = 0, n_msg = 0, last = -1, msg8 = 0;
    for (int f = 0; f < 6000 && !frei; f++) {
        uint16_t e = (g_scd.message_active && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (g_scd.player_mode == 2) szene = 1;
        if (g_scd.message_active && (int)g_scd.message_id != last) { last = (int)g_scd.message_id; n_msg++; if (last == 8) msg8 = 1; }
        if (!g_scd.message_active) last = -1;
        if (szene && g_scd.player_mode == 0 && !g_scd.message_active && re15_game_flag_get(3, 0x5F) &&
            !re15_game_flag_get(2, 7) && f > 300) frei = 1;
    }
    printf("    Szene: %d Nachrichten, msg 8 %d, frei %d in Bild %ld; Spieler (%d,%d)\n", n_msg, msg8, frei, s_bild, pl->x, pl->z);
    PRUEF(szene && frei && msg8, "Szene sub02 gelaufen (msg 0..8, \"I'd like to be alone now...\") und Steuerung zurueck");
    for (int f = 0; f < 30; f++) frame(0, 0);
    const re15_aot_t *a = &g_aot.slots[6];
    printf("    Slot 6 aktiv %d Typ %d Ereignis %d Mitte (%d,%d) halb (%d,%d) sat 0x%02X\n", a->active, a->type, a->event_id, a->x, a->z,
           a->half_w, a->half_h, a->sce_flags);
    PRUEF(a->active && a->event_id == RE15_R38F_1021_EREIGNIS && a->x == -4350 && a->z == -9600 && a->half_w == 500 &&
          a->half_h == 500, "direkt nach der Szene: Leichen-Zone Slot 6 -> Ereignis 6, Rechteck (-4850,-10100) 1000x1000 "
          "(main00 @0x01E2C)");
    int john = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == 0x49) john = s;
    if (john > 0) printf("    John bei (%d,%d)\n", g_actors[john].x, g_actors[john].z);
    /* Druck wie der Spieler: von Osten (Vorwaerts-Punkt (-4350,-9600) in der Zone), Blick -X */
    druck_t r = druecke(-3730, -9600, 2048, 200);
    PRUEF(r.msg_id == RE15_R38F_MSG_1021_LEICHE && !r.raumwechsel,
          "IM SELBEN BESUCH: Druck auf Roys Leiche -> msg %d (soll 40, Hinweis \"There's an empty key tag ...\")", r.msg_id);
}

/* ---- Teil: abgang (Nachbesserung 1 M3) --------------------------------------------------------------------------
 * Abnahme 0 E5/E6/E7/E10: Johns Abgangslauf schob Elza (Koerper-Schub FUN_8002aec4), im Bild von Cut 7 sichtbar (E10).
 * Je Lage: Phase H bis John in der Ecke steht, Elza an die Lage, Gorillas tot. Geprueft: Elza wird NIE verschoben (sie hat
 * keine Eingabe), John ist im Bild nie naeher als KOERPER_R an ihr, steht sie in seiner sichtbaren Bahn (BAHN_*), WARTET der
 * Abgang (Steuerung frei, Doppeltuer frei), bis sie die Bahn verlaesst; danach Abgang, Parken, Kamera zurueck. */
static int bis_zur_ecke(void)
{
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_R38F_F0_BANK, RE15_R38F_F0_BIT, 1);
    re15_game_flag_set(9, RE15_R38F_BIT_TUER, 1);   /* wie im Spiel: Elza kam durch die aufgeschlossene Tuer 1131 ((9,66)) */
    if (room_boot(RE15_R38F_RAUM_1141, -7600, -17600, 0, 0) != 0) return 0;
    if (re15_dining_r38_phase_h() != 1) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *john = &g_actors[1];
    for (int f = 0; f < 60; f++) frame(0, 0);
    pl->x = -8300; pl->z = -10250; pl->rot_y = 2048;   /* Gemaelde wie teil_phase_h */
    frame(0, 0);
    int ja = 0;
    for (int f = 0; f < 600 && !ja; f++) {
        uint16_t e = (f == 0 || (g_scd.message_active && (f % 20) == 0)) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        ja = re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT);
    }
    if (!ja) return 0;
    /* Runde 39 Spur J, Nachbesserung 1 (Nutzer-Nachtrag 4): das Gemaelde oeffnet nur den Rolladen, danach laeuft Elza frei;
     * die Szene mit John startet erst am Riegel der Dachtuer (Ereignis 5, main00 @0x00A9A -> sub05). Erst die Rolladen-Szene
     * abwarten, dann den Riegel druecken (Vorwaerts-Punkt (-7250,-250) im Rechteck @0x00A9A, Blick +Z) und msg 12 schliessen. */
    { int pm = 0, f1 = 0;
      for (int f = 0; f < 1200 && !f1; f++) { frame(0, 0); if (g_scd.player_mode == 2) pm = 1; if (pm && g_scd.player_mode == 0) f1 = 1; }
      if (!f1) return 0; }
    (void)druecke(-7250, -250 - 620, 3072, 1);
    for (int f = 0; f < 300 && !re15_game_flag_get(4, 0xC3); f++) frame(0, 0);
    if (!re15_game_flag_get(4, 0xC3)) return 0;
    int pm2 = 0, frei = 0;
    for (int f = 0; f < 4000 && !frei; f++) {
        { uint16_t e = (g_scd.message_active && g_scd.message_id == 12 && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
        if (g_scd.player_mode == 2) pm2 = 1;
        if (pm2 && g_scd.player_mode == 0 && !g_scd.message_active && f > 200) frei = 1;
    }
    if (!frei) return 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) g_actors[s].grid_id |= 0x20;
    for (int f = 0; f < 900 && !(abs(john->x - (-7550)) < 400 && abs(john->z - (-21500)) < 400); f++) frame(0, 0);
    for (int f = 0; f < 60; f++) frame(0, 0);   /* TURN @0x00F98 + Warten auf (4,0xF2) */
    return abs(john->x - (-7550)) < 400 && abs(john->z - (-21500)) < 400;
}

static int in_bahn(int32_t x, int32_t z)
{
    return x >= RE15_R38F_BAHN_X_MIN && x <= RE15_R38F_BAHN_X_MAX && z >= RE15_R38F_BAHN_Z_MIN && z <= RE15_R38F_BAHN_Z_MAX;
}

static void gorillas_tot(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) re15_actor_free(s);
    re15_game_flag_set(7, 0xCB, 1);   /* Kill-Flags pc[7] 0xCB/0xCC (sub00 @0x00B1A/@0x00B2E), gelesen sub01 @0x00BFA/@0x00C02 */
    re15_game_flag_set(7, 0xCC, 1);
}

typedef struct { const char *name; int32_t x, z; int16_t rot; int32_t weg_x, weg_z; } abgang_lage_t;

static void abgang_lage(const abgang_lage_t *L)
{
    printf("  -- Lage %s (%d,%d)\n", L->name, L->x, L->z);
    if (!bis_zur_ecke()) { printf("  FEHLER: Phase H bis zur Ecke nicht erreicht\n"); g_fail++; return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *john = &g_actors[1];
    pl->x = L->x; pl->z = L->z; pl->rot_y = L->rot;
    for (int f = 0; f < 10; f++) frame(0, 0);
    int32_t ex = pl->x, ez = pl->z;
    const int bahn = in_bahn(ex, ez);
    PRUEF(abs(ex - L->x) + abs(ez - L->z) == 0, "Elza steht bei (%d,%d) %s Johns sichtbarer Bahn", ex, ez, bahn ? "IN" : "ausserhalb");
    gorillas_tot();
    int max_abw = 0, cut7 = -1, park = -1, min_sicht = 1 << 30, warte_ok = 1;
    for (int f = 0; f < 2400; f++) {
        frame(0, 0);
        const int abw = abs(pl->x - ex) + abs(pl->z - ez);
        if (abw > max_abw) { max_abw = abw; if (abw > 0) printf("    Bild %ld: Elza (%d,%d) Abweichung %d, John (%d,%d)\n", s_bild, pl->x,
                                                                   pl->z, abw, john->x, john->z); }
        if (s_shown == 7 && cut7 < 0) cut7 = f;
        if (john->x > -29000 && sichtbar(s_shown, john)) {
            const double dx = john->x - pl->x, dz = john->z - pl->z;
            const int d = (int)sqrt(dx * dx + dz * dz);
            if (d < min_sicht) min_sicht = d;
        }
        if (john->x <= -29000 && park < 0) park = f;
        if (bahn && f == 300) {
            /* 300 Bilder in der Bahn: kein Abgang, Steuerung frei, Doppeltuer frei (keine Sperre mehr) */
            warte_ok = (cut7 < 0 && park < 0 && g_scd.player_mode == 0 && abs(john->x - (-7550)) < 400 &&
                        abs(john->z - (-21500)) < 400);
            PRUEF(warte_ok, "  in der Bahn: 300 Bilder nach dem Tod der Gorillas KEIN Abgang (Cut 7 %d, Park %d, pm %d, John (%d,%d))",
                  cut7, park, (int)g_scd.player_mode, john->x, john->z);
            PRUEF(g_aot.slots[0].active && g_aot.slots[0].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[0].dest_room == 0x13,
                  "  ... Doppeltuer frei: Slot 0 = Tuer -> Raum 0x%02X (soll 0x13 = ROOM1131, keine Sperre msg 41)",
                  g_aot.door_params[0].dest_room);
            pl->x = L->weg_x; pl->z = L->weg_z;   /* Elza geht aus der Bahn (Ortswechsel des Riegels, ohne Gangbild) */
            frame(0, 0);
            ex = pl->x; ez = pl->z;
            PRUEF(!in_bahn(ex, ez), "  Elza verlaesst die Bahn: (%d,%d)", ex, ez);
        }
        if (park >= 0 && f > park + 60 && g_scd.player_mode == 0) break;
    }
    PRUEF(max_abw == 0, "  Elza wird nie verschoben (groesste Abweichung %d)", max_abw);
    PRUEF(park >= 0, "  John geht (geparkt in Bild %d, Cut 7 ab Bild %d)", park, cut7);
    if (bahn) PRUEF(cut7 > 300, "  ... erst NACHDEM Elza die Bahn verlassen hat (Cut 7 ab Bild %d)", cut7);
    else      PRUEF(cut7 >= 0 && cut7 < 60, "  ... sofort (Cut 7 ab Bild %d)", cut7);
    PRUEF(min_sicht >= RE15_R38F_KOERPER_R, "  John im Bild nie naeher als %d an Elza (kleinster Abstand %d)", RE15_R38F_KOERPER_R,
          min_sicht == (1 << 30) ? -1 : min_sicht);
    PRUEF(s_shown != 7 && g_scd.player_mode == 0 && g_scd.cut_auto_enabled, "  danach Kamera %d (nicht mehr Cut 7), Steuerung frei",
          s_shown);
    PRUEF(re15_game_flag_get(9, RE15_R38F_BIT_JOHN_WEG) == 1 && re15_game_flag_get(4, 0xC3) == 1, "  (9,67) = 1, Riegel (4,0xC3) = 1");
    (void)warte_ok;
}

static void teil_abgang(void)
{
    printf("== abgang ==\n");
    /* Runde 39 Spur J (Punkt 8): nach dem Tod der Gorillas steht Elza fuer Johns zwei Zeilen an einer festen Lage
     * (Pos_set wie sub02 @0x00C32, (-6000,-20000), ausserhalb von Johns Bahn BAHN_*); die sechs Lagen dieses Teils kann
     * es nicht mehr geben. Kein Schub und Abstand > KOERPER_R im Bild prueft unit_r39_dining1141_phase_h. */
    printf("  ok: ersetzt durch unit_r39_dining1141_phase_h (Runde 39 Spur J, Punkt 8)\n");
    if (1) return;
    static const abgang_lage_t lagen[] = {
        /* die vier Lagen der Abnahme (E10 in der Bahn, E5/E6/E7 im Dachgang/an der Dachtuer hinter der Kamera von Cut 7) */
        { "E10 Suedteil auf Johns Weg",      -7418, -14952, 3072, -5600, -14952 },
        { "E5 Dachgang Mitte",               -7342,  -4062, 1024,     0,      0 },
        { "E6 vor der Dachtuer",             -7263,  -1030, 3072,     0,      0 },
        { "E7 auf Johns Zielpunkt",          -7332,   -718, 3072,     0,      0 },
        /* dazu: knapp neben der Bahn (kein Warten, Abstand im Bild), neben John in der Ecke (Warten) */
        { "knapp oestlich der Bahn",         -6400, -15000, 2048,     0,      0 },
        { "neben John in der Ecke",          -6500, -20500, 2048, -5600, -20500 },
    };
    for (unsigned i = 0; i < sizeof lagen / sizeof lagen[0]; i++) abgang_lage(&lagen[i]);
}

/* ---- Teil: weg1131 (Nachbesserung 1 M3): Elza geht waehrend des Wartens durch die Doppeltuer hinaus — beim
 * Zurueckkommen ist John fort (er ging aufs Dach, waehrend sie fort war), die Dachtuer offen; kein John taucht auf. */
static void teil_weg1131(void)
{
    printf("== weg1131 ==\n");
    if (!bis_zur_ecke()) { printf("  FEHLER: Phase H bis zur Ecke nicht erreicht\n"); g_fail++; return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -7980; pl->z = -17750; pl->rot_y = 2048;   /* vor der Doppeltuer (Slot 0 x -9100..-8100, z -19750..-15750) */
    for (int f = 0; f < 10; f++) frame(0, 0);
    PRUEF(in_bahn(pl->x, pl->z), "Elza vor der Doppeltuer (%d,%d) steht in Johns sichtbarer Bahn", pl->x, pl->z);
    /* Runde 39 Spur J (Punkt 8): nach dem Tod der Gorillas folgen Johns zwei Zeilen und sein Abgang unter Pad-Sperre — Elza
     * kann waehrend des Wartens nicht mehr hinausgehen (die Doppeltuer wird erst danach wieder Tuer). Geprueft wird der
     * Rueckfall in tuer_1131 (dining_r38.c) direkt am Zustand "Szene gelaufen, Gorillas tot, John noch da", wie ihn ein
     * Spielstand aus dieser Lage liefert, und danach der Endzustand. */
    gorillas_tot();
    re15_game_flag_set(RE15_R38F_F2_BANK, RE15_R38F_F2_BIT, 0);   /* sub01 @0x00C0C */
    PRUEF(re15_game_flag_get(9, RE15_R38F_BIT_JOHN_WEG) == 0 && re15_game_flag_get(3, 0xC2) == 1,
          "Zustand: Szene gelaufen (3,0xC2) = 1, Gorillas tot, John noch nicht fort ((9,67) = 0)");
    /* Raumaufbau 1131 (room_boot behaelt die Flags; die Installer-Regel tuer_1131 laeuft darin) */
    if (room_boot(RE15_R38F_RAUM_1131, -1300, -13950, 0, 0) != 0) return;   /* Ruecksprung-Spawn aus 1141 */
    PRUEF(re15_game_flag_get(9, RE15_R38F_BIT_JOHN_WEG) == 1 && re15_game_flag_get(4, 0xC3) == 1,
          "ROOM1131: John ist aufs Dach gegangen, waehrend Elza fort war: (9,67) = 1, Riegel (4,0xC3) = 1");
    if (room_boot(RE15_R38F_RAUM_1141, -7600, -17600, 0, 0) != 0) return;
    for (int f = 0; f < 30; f++) frame(0, 0);
    int john_da = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == RE15_R38F_JOHN_TYP) john_da = 1;
    PRUEF(!john_da && re15_dining_r38_phase_h() == 0, "zurueck in 1141: kein John, keine Phase H");
    PRUEF(g_aot.slots[1].active && g_aot.slots[1].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[1].dest_room == 0x17 &&
          !g_aot.slots[2].active, "Dachtuer offen: Slot 1 = Tuer -> Raum 0x%02X (soll 0x17), Riegel-Zone Slot 2 aus (aktiv %d)",
          g_aot.door_params[1].dest_room, g_aot.slots[2].active);
}

/* ---- Teil: takt (Messwerkzeug, kein Riegel): wie viele Aktionsdrucke im Abstand von 60 Bildern (RE15_WEGPLAN
 * AKTION + WARTE 60) braucht jede Abfrage? Liefert die Zahlen fuer tests/integration/test_r38_dining1141.cmake. */
static int drucke_bis(int (*offen)(void), int max)
{
    int n = 0;
    for (int k = 0; k < max && offen(); k++) {
        frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE); frame(RE15_PAD_BIT_SQUARE, 0); frame(RE15_PAD_BIT_SQUARE, 0);
        n++;
        for (int w = 0; w < 60; w++) frame(0, 0);
    }
    return n;
}
static int msg_offen(void) { return g_scd.message_active || g_scd.message_query; }
static int wegwerf_offen(void) { return re15_discard_active(); }

static void teil_takt(void)
{
    printf("== takt ==\n");
    /* 1141 Gemaelde */
    re15_game_state_init(); re15_inv_load_briefing();
    re15_game_flag_set(RE15_R38F_F0_BANK, RE15_R38F_F0_BIT, 1);
    if (room_boot(RE15_R38F_RAUM_1141, -8150, -10250, 2048, 5) != 0) return;
    for (int f = 0; f < 30; f++) frame(0, 0);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE); frame(RE15_PAD_BIT_SQUARE, 0); frame(RE15_PAD_BIT_SQUARE, 0);
    for (int w = 0; w < 60; w++) frame(0, 0);
    int n = drucke_bis(msg_offen, 10);
    printf("    1141 Gemaelde: 1 Druck oeffnet, danach %d Drucke bis zu, (3,0x6A) = %d\n", n,
           re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT));
    /* 1131 Tuer mit Schluessel */
    re15_game_state_init(); re15_inv_load_briefing();
    inv_geben(RE15_R38F_ITEM);
    if (room_boot(RE15_R38F_RAUM_1131, -1300, -13900, 0, 0) != 0) return;
    for (int f = 0; f < 30; f++) frame(0, 0);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE); frame(RE15_PAD_BIT_SQUARE, 0); frame(RE15_PAD_BIT_SQUARE, 0);
    for (int w = 0; w < 60; w++) frame(0, 0);
    n = drucke_bis(msg_offen, 10);
    int f0 = 0; while (f0 < 120 && !re15_discard_active()) { frame(0, 0); f0++; }
    int m = drucke_bis(wegwerf_offen, 10);
    printf("    1131 Tuer: 1 Druck oeffnet msg 41, %d Drucke bis zu, Abfrage nach %d Bildern, %d Drucke bis zu, Schluessel %s\n",
           n, f0, m, re15_inv_find_item(RE15_R38F_ITEM) < 0 ? "weg" : "da");
    /* 1021 Leiche */
    re15_game_state_init();
    re15_game_flag_set(RE15_R38F_ROY_BANK, RE15_R38F_ROY_BIT, 1);
    if (room_boot(RE15_R38F_RAUM_1021, -3730, -9600, 2048, 0) != 0) return;
    for (int f = 0; f < 30; f++) frame(0, 0);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE); frame(RE15_PAD_BIT_SQUARE, 0); frame(RE15_PAD_BIT_SQUARE, 0);
    printf("    1021 Leiche: nach dem Druck msg %d aktiv %d\n", (int)g_scd.message_id, (int)g_scd.message_active);
    for (int w = 0; w < 60; w++) frame(0, 0);
    n = drucke_bis(msg_offen, 10);
    printf("    1021 Leiche: danach %d Drucke bis zu\n", n);
    /* 1140 Leon: welches Ereignis feuert der DRUCK aufs Gemaelde (Slot 5)? sub00 @0x00B14 setzt Slot 5 -> sub03,
     * sub01 @0x00C7A setzt ihn -> sub05 ("not responding"). */
    re15_game_state_init(); re15_inv_load_briefing();
    re15_game_flag_set(2, 0, 1);
    if (room_boot(RE15_R38F_RAUM_1140, -8150, -10250, 2048, 5) != 0) return;
    for (int f = 0; f < 30; f++) frame(0, 0);
    printf("    1140 Slot 5: aktiv %d Typ %d Ereignis %d\n", g_aot.slots[5].active, g_aot.slots[5].type, g_aot.slots[5].event_id);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE); frame(RE15_PAD_BIT_SQUARE, 0); frame(RE15_PAD_BIT_SQUARE, 0);
    for (int w = 0; w < 60; w++) frame(0, 0);
    n = drucke_bis(msg_offen, 10);
    printf("    1140 Gemaelde-Druck: %d Drucke, (3,0x6A) = %d, letzte msg %d\n", n,
           re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT), (int)g_scd.message_id);
}

/* ---- Teil: leon1140 --------------------------------------------------------------------------------------- */
static void teil_leon1140(void)
{
    printf("== leon1140 ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(2, 0, 1);   /* Strom an (scd_vm.c s_power_gates: sonst leitet Ereignis 3 auf 5) */
    if (room_boot(RE15_R38F_RAUM_1140, -8300, -10250, 2048, 5) != 0) return;
    for (int f = 0; f < 20; f++) frame(0, 0);
    PRUEF((re15_dining_r38_anker_ok() & 1) != 0, "ROOM1140 sub03 @0x00CA6 bytegleich (Anker)");
    int prop = -1;
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == 0) prop = k;
    const int32_t y0 = prop >= 0 ? g_scd.props[prop].y : 0;
    const int se0 = s_n_se;
    scd_event_fire(RE15_R38F_1140_EREIGNIS);
    int32_t ymin = y0;
    for (int f = 0; f < 600; f++) {
        uint16_t e = (g_scd.message_active && (f % 20) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (prop >= 0 && g_scd.props[prop].y < ymin) ymin = g_scd.props[prop].y;
    }
    int r0 = -1, r1 = -1, r2 = -1;
    for (int i = se0; i < s_n_se; i++) {
        printf("    Bild %4ld Se_on(0x%02X,0x%02X)\n", s_se[i].bild, s_se[i].bank, s_se[i].satz);
        if (s_se[i].bank == 0x3B && s_se[i].satz == 0x0C && r0 < 0) r0 = i;
        if (s_se[i].bank == 0x3B && s_se[i].satz == 0x0A && r1 < 0) r1 = i;
        if (s_se[i].bank == 0x3B && s_se[i].satz == 0x0B && r2 < 0) r2 = i;
    }
    PRUEF(r0 >= 0 && r1 > r0 && r2 > r1, "Leon 1140: Rolladen-Ton 0x3B 0x0C, 0x0A, 0x0B (ROOM1131 sub02 @0x00A48/0x00A58/0x00AB2)");
    if (r0 >= 0 && r1 > r0 && r2 > r1)
        PRUEF(s_se[r1].bild - s_se[r0].bild >= 15 && s_se[r1].bild - s_se[r0].bild <= 17,
              "  Abstand 0x0C -> 0x0A = Sleep 15 (@0x00A54): %ld Bilder", s_se[r1].bild - s_se[r0].bild);
    PRUEF(prop >= 0 && ymin < y0 - 1000, "Rolladen (Prop obj 0) faehrt hoch: y %d -> %d", y0, ymin);
}

/* ---- Teil: sicht (Werkzeug des Riegels integration_r38_dining1141_s; Dossier 4.4) ------------------------------------
 * Aufruf: test_r38_dining1141 sicht <laufA> <laufB> <praefix>
 * A = Lauf mit John, B = derselbe Lauf mit RE15_R38F_JOHN_AUS=1 (John weder Mesh noch Schatten, Spiel unveraendert). Beide
 * mit RE15_R38F_SICHT=<praefix>: je Bild von ROOM1141 das fertig komponierte Bild <praefix>NNNNNN.ppm (Ruecklese-Weg von
 * RE15_FRAMEDUMP) und eine Zeile "[r38f-sicht] ..." in debug.log (platform/pc/src/dining_r38_pc.c). Johns sichtbare Pixel
 * je Bild = die Pixel, in denen A und B sich unterscheiden. Geprueft (Nachtrag 2: "keine plötzlich rein poppenden
 * Charaktere"; Auftrag: in keinem Bild erscheint John ohne vorherige Bewegung ins Bild):
 *   R0  A und B tragen dieselben Messzeilen (das Spiel lief ohne Johns Bild gleich, sonst waere der Vergleich ungueltig),
 *       jedes Bild beider Laeufe ist da.
 *   R1  solange der Notrolladen nicht faehrt, ist John in keinem Bild sichtbar (verdeckt im Dachgang).
 *   R2  innerhalb derselben Kamera wird John nur sichtbar (0 -> >0 Pixel), wenn er sich bewegt (Schritt 1..400 Einheiten
 *       je Bild) oder der Rolladen faehrt.
 *   R3  innerhalb derselben Kamera verschwindet John nur (>0 -> 0 Pixel), wenn er sich bewegt (Schritt 1..400); ein Parken
 *       (x <= -29000) oder Sprung (> 400) waehrend er sichtbar ist, ist ein Verstoss.
 * Kamerawechsel sind Schnitte: dort darf sich alles aendern. Erkannt am BILD des Laufs B (ohne John): mittlere
 * Abweichung je Kanal zum Vorbild > SICHT_SCHNITT (gemessen Lauf e7035a6f, Grauwert: an den 9 Kamerawechseln 25,5..40,3,
 * sonst <= 3,0 — Letterbox-Balken 3,0, Textfenster 1,2). Die Kamera-Nummern des Logs taugen dafuer NICHT: ein
 * Skript-Cut_chg steht ein Bild VOR dem Bildwechsel im Log (F2119 anz=2, Bild erst F2120), ein Zonenwechsel im selben Bild
 * (F801) — gemessen, erste Fassung des Werkzeugs meldete deshalb an F2120/F2222 zwei Schein-Verstoesse.
 * Schreibt <laufA>/sicht.txt (je Bild F, Kamera, Pixel, Kasten, Lagen, Bildwechsel). */
#define SICHT_SCHNITT 10.0
typedef struct { int da, cam, anz, jda, jx, jz, rda, ry, ex, ez; } sicht_t;
#define SICHT_MAX 20000
static sicht_t s_sicht_a[SICHT_MAX], s_sicht_b[SICHT_MAX];

static int sicht_lesen(const char *lauf, sicht_t *out)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/debug.log", lauf);
    FILE *f = fopen(p, "rb");
    if (!f) return -1;
    char z[4096];
    int n = 0;
    while (fgets(z, sizeof z, f)) {
        if (strncmp(z, "[r38f-sicht] F", 14) != 0) continue;
        unsigned fr; sicht_t s;
        if (sscanf(z, "[r38f-sicht] F%u cam=%d anz=%d john=%d (%d,%d) rolladen=%d y=%d elza=(%d,%d)", &fr, &s.cam, &s.anz,
                   &s.jda, &s.jx, &s.jz, &s.rda, &s.ry, &s.ex, &s.ez) != 10 || fr >= SICHT_MAX || out[fr].da) continue;
        s.da = 1;
        out[fr] = s;
        n++;
    }
    fclose(f);
    return n;
}

static uint8_t *ppm_lesen(const char *p, int *w, int *h)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    char mag[3] = { 0 };
    int mx = 0;
    if (fscanf(f, "%2s %d %d %d", mag, w, h, &mx) != 4 || strcmp(mag, "P6") || mx != 255 || *w <= 0 || *h <= 0) {
        fclose(f); return NULL;
    }
    fgetc(f);
    const size_t n = (size_t)(*w) * (size_t)(*h) * 3u;
    uint8_t *b = (uint8_t *)malloc(n);
    if (!b || fread(b, 1, n, f) != n) { free(b); fclose(f); return NULL; }
    fclose(f);
    return b;
}

static int teil_sicht(int argc, char **argv)
{
    printf("== sicht ==\n");
    if (argc < 5) { printf("Aufruf: test_r38_dining1141 sicht <laufA> <laufB> <praefix>\n"); return 2; }
    const char *la = argv[2], *lb = argv[3], *pf = argv[4];
    const int na = sicht_lesen(la, s_sicht_a), nb = sicht_lesen(lb, s_sicht_b);
    PRUEF(na > 0 && nb > 0, "Messzeilen [r38f-sicht]: A %d, B %d", na, nb);
    if (na <= 0 || nb <= 0) return 1;
    int erst = -1, letzt = -1, abweich = -1;
    for (int f = 0; f < SICHT_MAX; f++) {
        const sicht_t *a = &s_sicht_a[f], *b = &s_sicht_b[f];
        if (abweich < 0 && (a->da != b->da || (a->da && memcmp(a, b, sizeof *a) != 0))) abweich = f;
        if (a->da) { if (erst < 0) erst = f; letzt = f; }
    }
    PRUEF(abweich < 0, "R0 A und B mit gleichen Messzeilen (Spiel ohne Johns Bild unveraendert), erste Abweichung %d", abweich);
    char p[1200];
    snprintf(p, sizeof p, "%s/sicht.txt", la);
    FILE *out = fopen(p, "w");
    if (out) fprintf(out, "# Bild cam anz Pixel Kasten(x0,y0,x1,y1) John(da,x,z) Rolladen(da,y) Elza(x,z) Bildwechsel(B)\n");
    int fehlend = 0, bilder = 0, sichtbar_n = 0, erst_sicht = -1, letzt_sicht = -1, verstoss = 0;
    int r1 = 0, r2 = 0, r3 = 0, erscheinen = 0, verschwinden = 0, schnitte = 0;
    int px_vor = -1, f_vor = -1, rolladen_faehrt_seit = -1;
    uint8_t *bb_vor = NULL;   /* Bild B des Vorbilds (Schnitt-Erkennung) */
    double mad_max_ohne = 0.0, mad_min_schnitt = 1e9;
    const int ry0 = s_sicht_a[erst].ry;
    for (int f = erst; f <= letzt; f++) {
        const sicht_t *s = &s_sicht_a[f];
        if (!s->da) { px_vor = -1; continue; }
        int wa = 0, ha = 0, wb = 0, hb = 0;
        char pa[1200], pb[1200];
        snprintf(pa, sizeof pa, "%s/%s%06d.ppm", la, pf, f);
        snprintf(pb, sizeof pb, "%s/%s%06d.ppm", lb, pf, f);
        uint8_t *ba = ppm_lesen(pa, &wa, &ha), *bb = ppm_lesen(pb, &wb, &hb);
        if (!ba || !bb || wa != wb || ha != hb) {
            fehlend++; free(ba); free(bb); free(bb_vor); bb_vor = NULL; px_vor = -1; continue;
        }
        bilder++;
        /* Bildwechsel im Lauf B (ohne John): mittlere Abweichung je Kanal zum Vorbild */
        double mad = -1.0;
        if (bb_vor && f_vor == f - 1) {
            unsigned long long sum = 0;
            const size_t nb3 = (size_t)wb * (size_t)hb * 3u;
            for (size_t i = 0; i < nb3; i++) sum += (unsigned long long)abs((int)bb[i] - (int)bb_vor[i]);
            mad = (double)sum / (double)nb3;
        }
        int px = 0, x0 = wa, y0 = ha, x1 = -1, y1 = -1;
        for (int y = 0; y < ha; y++)
            for (int x = 0; x < wa; x++) {
                const size_t i = ((size_t)y * wa + x) * 3u;
                if (ba[i] != bb[i] || ba[i + 1] != bb[i + 1] || ba[i + 2] != bb[i + 2]) {
                    px++;
                    if (x < x0) x0 = x;
                    if (x > x1) x1 = x;
                    if (y < y0) y0 = y;
                    if (y > y1) y1 = y;
                }
            }
        free(ba); free(bb_vor); bb_vor = bb;
        if (out) fprintf(out, "F%d %d %d %d (%d,%d,%d,%d) (%d,%d,%d) (%d,%d) (%d,%d) %.1f\n", f, s->cam, s->anz, px,
                         px ? x0 : -1, px ? y0 : -1, x1, y1, s->jda, s->jx, s->jz, s->rda, s->ry, s->ex, s->ez, mad);
        if (rolladen_faehrt_seit < 0 && s->rda && s->ry != ry0) rolladen_faehrt_seit = f;
        if (px > 0) { sichtbar_n++; if (erst_sicht < 0) erst_sicht = f; letzt_sicht = f; }
        /* R1: verdeckt, solange der Rolladen nicht faehrt */
        if (rolladen_faehrt_seit < 0 && px > 0) {
            r1++; verstoss++;
            if (r1 <= 5) printf("  VERSTOSS R1 Bild %d: John sichtbar (%d px, Kasten %d,%d-%d,%d), Rolladen steht noch\n",
                               f, px, x0, y0, x1, y1);
        }
        if (px_vor >= 0 && f_vor == f - 1) {
            const sicht_t *v = &s_sicht_a[f - 1];
            const int schnitt = mad > SICHT_SCHNITT;
            if (schnitt) {
                schnitte++;
                if (mad < mad_min_schnitt) mad_min_schnitt = mad;
                printf("  Schnitt Bild %d: Bildwechsel %.1f, Kamera laut Log %d -> %d (angefordert %d -> %d), John %d -> %d px\n",
                       f, mad, v->anz, s->anz, v->cam, s->cam, px_vor, px);
            } else if (mad > mad_max_ohne) mad_max_ohne = mad;
            const int park = s->jda && s->jx <= -29000, park_v = v->jda && v->jx <= -29000;
            int schritt = -1;
            if (s->jda && v->jda && !park && !park_v) schritt = abs(s->jx - v->jx) + abs(s->jz - v->jz);
            const int laeuft = schritt >= 1 && schritt <= 400;
            const int rolladen = s->rda && v->rda && s->ry != v->ry;
            if (!schnitt && px_vor == 0 && px > 0) {
                erscheinen++;
                printf("  erscheint Bild %d (Cut %d): %d px Kasten %d,%d-%d,%d, John-Schritt %d, Rolladen %s\n", f, s->anz,
                       px, x0, y0, x1, y1, schritt, rolladen ? "faehrt" : "steht");
                if (!(laeuft || rolladen)) { r2++; verstoss++; printf("  VERSTOSS R2 Bild %d: ohne Bewegung ins Bild\n", f); }
            }
            if (!schnitt && px_vor > 0 && px == 0) {
                verschwinden++;
                printf("  verschwindet Bild %d (Cut %d): vorher %d px, John-Schritt %d%s\n", f, s->anz, px_vor, schritt,
                       park ? ", GEPARKT" : "");
                if (!laeuft) { r3++; verstoss++; printf("  VERSTOSS R3 Bild %d: verschwindet ohne Bewegung aus dem Bild\n", f); }
            }
            if (!schnitt && (px > 0 || px_vor > 0) && schritt > 400) {
                r3++; verstoss++;
                printf("  VERSTOSS R3 Bild %d: Sprung %d Einheiten, waehrend John sichtbar ist\n", f, schritt);
            }
        }
        px_vor = px; f_vor = f;
    }
    if (out) fclose(out);
    free(bb_vor);
    PRUEF(fehlend == 0 && bilder > 0, "R0 Bilder: %d verglichen (Bild %d..%d), %d fehlen", bilder, erst, letzt, fehlend);
    PRUEF(schnitte > 0, "Schnitte am Bild erkannt: %d (Bildwechsel dort >= %.1f, sonst <= %.1f, Schwelle %.1f)", schnitte,
          mad_min_schnitt, mad_max_ohne, SICHT_SCHNITT);
    PRUEF(rolladen_faehrt_seit > 0, "Rolladen faehrt ab Bild %d", rolladen_faehrt_seit);
    PRUEF(r1 == 0, "R1 vor dem Hochfahren in keinem Bild sichtbar (%d Verstoesse)", r1);
    PRUEF(sichtbar_n > 0 && erst_sicht >= rolladen_faehrt_seit,
          "John sichtbar in %d Bildern, zuerst Bild %d (Rolladen ab %d), zuletzt Bild %d", sichtbar_n, erst_sicht,
          rolladen_faehrt_seit, letzt_sicht);
    PRUEF(r2 == 0, "R2 %d mal ins Bild gekommen, jedes Mal mit Bewegung (%d Verstoesse)", erscheinen, r2);
    PRUEF(r3 == 0, "R3 %d mal aus dem Bild gegangen, jedes Mal mit Bewegung, kein Parken/Sprung im Bild (%d Verstoesse)",
          verschwinden, r3);
    printf("  sicht.txt: %s\n", p);
    return (g_fail || verstoss) ? 1 : 0;
}

/* Tuersequenz-Laeufer: faengt jede Anfrage (im Spiel spielt ihn die Plattform; Muster test_r37_sperre1050.c). */
static int s_n_seq = 0;
static void fang(const re15_door_seq_anfrage_t *a) { (void)a; s_n_seq++; }

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    const int alle = !strcmp(teil, "alle");
    if (!strcmp(teil, "sicht")) return teil_sicht(argc, argv);   /* Werkzeug des Riegels integration_r38_dining1141_s */
    re15_door_seq_setze_laeufer(fang);
    if (alle || !strcmp(teil, "texte"))          teil_texte();
    if (alle || !strcmp(teil, "programme"))      teil_programme();
    if (alle || !strcmp(teil, "leiche1021"))     teil_leiche1021();
    if (alle || !strcmp(teil, "schluessel1011")) teil_schluessel1011();
    if (alle || !strcmp(teil, "tuer1131"))       teil_tuer1131();
    if (alle || !strcmp(teil, "phase_h"))        teil_phase_h();
    if (alle || !strcmp(teil, "leon1140"))       teil_leon1140();
    if (alle || !strcmp(teil, "szene1021"))      teil_szene1021();   /* Nachbesserung 1 M1 */
    if (alle || !strcmp(teil, "abgang"))         teil_abgang();      /* Nachbesserung 1 M3 */
    if (alle || !strcmp(teil, "weg1131"))        teil_weg1131();     /* Nachbesserung 1 M3 */
    if (!strcmp(teil, "takt"))                       teil_takt();   /* Messwerkzeug, kein Riegel */
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler (Tuersequenzen %d)\n", teil, g_fail, s_n_seq);
    return g_fail ? 1 : 0;
}
