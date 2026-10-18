/*
 * test_r39_parkplatz11c0.c — Runde 39 Spur O "parkplatz11c0": ROOM11C0 Tuersperre waehrend des Gorilla-Kampfes.
 * Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit allen Installern, SCD-VM mit dem Evt_exec-Haken, Spieler-/
 * NPC-Walker, Nachrichten-FSM, AOT-Scan, re15_game_step), Muster test_r38_dining1141.c / test_r35_affen.c.
 *
 * Nutzer (AUFTRAG.md Nachtrag 9): "bei room11C0, waehrend der Kampf gegen die Gorillas noch laeuft, soll bei Leon der
 * Text kommen "I cannot leave Ada alone here with those monsters!". Wenn Leon versucht durch die Tuer aus dem Raum raus
 * zu gehen." Dossier analysis/befunde_runde39/O_parkplatz11c0.md, Code include/re15_parkplatz_r39.h +
 * engine/src/parkplatz_r39.c, Programme engine/src/gen/parkplatz_r39_programme.inc (tools/r39_parkplatz/programme_bauen.py).
 *
 * Teile (Aufruf: test_r39_parkplatz11c0 <teil>):
 *   texte          msg 40 dekodiert = NUTZER-VORGABE woertlich; Erzaehlform `04 02 .. 08 .. 01 00` (kein Sprecherkopf),
 *                  Zeilen <= 271 px
 *   programme      SZENE = Aot_reset Slot 0 -> MESSAGE 40 + Gosub sub02; ENDE = Aot_reset Slot 0 -> TUER mit der Nutzlast
 *                  aus main00 @0x01712+14 + Gosub sub03; Kopf-Konstanten = Bytes des ausgelieferten RDT
 *   kampf          ECHTE Szene ab dem Tuer-Spawn (-25279,17268): ab dem ersten Szenenbild ist Slot 0 der Text, bei der
 *                  Rueckgabe der Steuerung (sub02 @0x01ACE) ebenso; Gorillas leben; Druck auf die Tuer -> msg 40 + Stimme,
 *                  KEIN Raumwechsel; danach ist Leon steuerbar; Gorillas tot (ihre Kill-Bits) -> sub03 = Programm ENDE ->
 *                  Slot 0 wieder die Tuer nach ROOM11B0, msg 7, Aot_on(0) @0x01C08 -> Raumwechsel ROOM11B0
 *   wiedereintritt (4,64)=1, (3,67)=0 (alter Spielstand / Sprung): Slot 0 VOR dem ersten Bild = Text; Druck -> msg 40;
 *                  Gorillas tot -> Tuer zurueck, Ende-Szene, Raumwechsel
 *   nachher        (4,64)=1, (3,67)=1 (Kampf vorbei): Slot 0 = Tuer, Druck -> Raumwechsel ROOM11B0, kein Text
 *   elza           ROOM11C1 (Elza, Hunde, kein Begleiter) mit denselben Flags: Slot 0 = Tuer, Druck -> Raumwechsel
 *   fremd          ein geaendertes RDT (1 Byte in sub02): Anker greifen nicht -> Original-Ablauf (keine Sperre)
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
#include "re15_door_seq.h"
#include "re15_parkplatz_r39.h"
#include "font_width.h"

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

/* ---- Harness (Muster test_r38_dining1141.c / test_r35_affen.c) ----------------------------------------------- */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static int                s_n_voice40 = 0;     /* SCD_AUDIO_VOICE_ON mit Id 40 aus der Warteschlange */
static int                s_patch_off = -1;    /* Teil fremd: dieses Datei-Byte nach dem Laden aendern */

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
    if (s_patch_off >= 0 && (size_t)s_patch_off < s_rawsz) s_raw[s_patch_off] ^= 0x01;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;   /* der Spielweg liest den GELADENEN Raum (Anker parkplatz_r39.c) */
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
        if (e.kind == SCD_AUDIO_VOICE_ON && e.sample_id == RE15_R39O_MSG_SPERRE) s_n_voice40++;
}

/* Gegner-Baenke wie die exe (Muster test_r35_affen.c bank_laden_27): ohne Bank laufen Fuss-Sperre und Walker nicht. */
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
          re15_emd_parse_skeleton(emr, rsz, &s_pl00_skel) == 0) ? 1 : 0;   /* Puffer bleiben (Zeiger in die Daten) */
    return ok;
}

/* Raumaufbau wie der Tuer-Weg (room_common.c: scd_room_reenter, Nachrichtenblock); die Flags bleiben. KEIN Bild vor der
 * Rueckgabe — der Aufrufer prueft den Zustand, den das ERSTE Bild zeigt. */
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
    s_shown = cut; s_cine_was = 0; s_n_voice40 = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);   /* Szenen-Clips des Raums (Plc_motion) */
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);   /* laedt nur die Ids 0..n-1, Port-Ids 40+ bleiben */
    if (bank_laden(0x27, 0) && bank_laden(0x42, 1) && pl00_laden()) {
        s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    } else { printf("  FEHLER: Baenke 0x27/0x42/PL00 nicht ladbar\n"); g_fail++; }
    return 0;
}

static int n_typ(uint8_t typ)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == typ) n++;
    return n;
}
static void gorillas_starr(void)   /* grid_id | 0x20 = Member_set 0x0C=0x30 (@0x800410b8), wie 11C0 sub02 @0x01A58 */
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R39O_GORILLA_TYP) g_actors[s].grid_id |= 0x20;
}
static int gorillas_tot(void)      /* herausnehmen + GENAU ihre Kill-Bits (STAGE1.BIN @0x8011b8e8-f4, Bank 7) */
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R39O_GORILLA_TYP) { re15_actor_free(s); n++; }
    re15_game_flag_set(RE15_R39O_KILL_BANK, RE15_R39O_KILL_1, 1);
    re15_game_flag_set(RE15_R39O_KILL_BANK, RE15_R39O_KILL_2, 1);
    return n;
}
static int slot0_text(void)
{
    const re15_aot_t *a = &g_aot.slots[RE15_R39O_TUER_SLOT];
    return a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_R39O_MSG_SPERRE;
}
static int slot0_tuer(void)
{
    const re15_aot_t *a = &g_aot.slots[RE15_R39O_TUER_SLOT];
    const re15_aot_door_params_t *d = &g_aot.door_params[RE15_R39O_TUER_SLOT];
    return a->active && a->type == RE15_AOT_TYPE_DOOR && d->dest_room == RE15_R39O_ZIEL_RAUM &&
           d->spawn_x == RE15_R39O_ZIEL_X && d->spawn_y == RE15_R39O_ZIEL_Y && d->spawn_z == RE15_R39O_ZIEL_Z;
}

/* Vor der Tuer (Rechteck x -27100..-23100, z 15900..18600, main00 @0x01712): (-25100,16600) Blick +Z (3072) — der
 * Vorwaerts-Punkt (-25100,17220) liegt im Rechteck. EINMAL Quadrat; danach `bilder` beobachten, Nachrichten ab Bild 40
 * alle 15 Bilder weiterschalten (Muster test_r38_dining1141.c druecke). */
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
#define TUER_X   (-25100)
#define TUER_Z   16600
#define TUER_ROT 3072

static int laeuft_steuerbar(void)  /* Blick -Z (1024), 30 Bilder UP: kommt Leon vom Fleck? */
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 30 && (g_scd.message_active || g_scd.player_mode != 0); f++) frame(0, 0);
    pl->rot_y = 1024;
    const int32_t x0 = pl->x, z0 = pl->z;
    for (int f = 0; f < 30; f++) frame(RE15_PAD_BIT_UP, f == 0 ? RE15_PAD_BIT_UP : 0);
    const double d = sqrt((double)(pl->x - x0) * (pl->x - x0) + (double)(pl->z - z0) * (pl->z - z0));
    printf("    Leon nach 30 Bildern UP: (%d,%d) -> (%d,%d), %.0f Einheiten\n", (int)x0, (int)z0, (int)pl->x, (int)pl->z, d);
    return d > 200.0 && g_scd.player_mode == 0 && !g_room_change.pending;
}

/* Kampf-Ende: Gorillas tot -> sub01 @0x01842 startet sub03 (Programm ENDE) -> Tuer zurueck, Szene, Aot_on(0). */
static void kampfende_pruefen(const char *wo)
{
    const unsigned e0 = g_re15_r39o_ende_zaehler;
    int n = gorillas_tot();
    PRUEF(n == 2, "%s: zwei Gorillas herausgenommen, Kill-Bits (7,0x60)/(7,0x61) gesetzt", wo);
    int f_ende = -1, f_tuer = -1, msg7 = 0, f_wechsel = -1, ziel = -1;
    g_room_change.pending = 0;
    for (int f = 0; f < 4000; f++) {
        frame(0, 0);
        if (f_ende < 0 && g_re15_r39o_ende_zaehler == e0 + 1) {
            f_ende = f;
            PRUEF(slot0_tuer(), "%s: im Bild des Evt_exec sub03 (F%d) ist Slot 0 wieder die Tuer -> 0x%02X Ziel (%d,%d,%d)", wo,
                  f, (unsigned)g_aot.door_params[0].dest_room, g_aot.door_params[0].spawn_x, g_aot.door_params[0].spawn_y,
                  g_aot.door_params[0].spawn_z);
            PRUEF(re15_game_flag_get(RE15_R39O_ENDE_BANK, RE15_R39O_ENDE_BIT) == 1, "%s: sub03 @0x01B02 Set(3,67)=1", wo);
        }
        if (f_tuer < 0 && f_ende >= 0 && slot0_tuer()) f_tuer = f;
        if (g_scd.message_active && g_scd.message_id == 7) msg7 = 1;
        if (g_scd.message_active && (f % 15) == 0) { frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE); }
        if (g_room_change.pending) { f_wechsel = f; ziel = (int)g_room_change.room_id; break; }
    }
    PRUEF(f_ende >= 0, "%s: Programm ENDE gestartet (F%d nach dem Tod)", wo, f_ende);
    PRUEF(msg7, "%s: msg 7 \"Ada: Okay, it's over now.\" lief (sub03 @0x01B68)", wo);
    PRUEF(f_wechsel >= 0 && ziel == 0x11B0, "%s: Ende-Szene geht durch die Tuer: Raumwechsel ROOM%04X (F%d; sub03 @0x01C08 Aot_on(0))",
          wo, (unsigned)ziel, f_wechsel);
}

/* ---- Teil: texte ------------------------------------------------------------------------------------------ */
static void teil_texte(void)
{
    printf("== texte ==\n");
    int n = 0;
    const uint8_t *b = re15_parkplatz_r39_text(&n);
    char t[256];
    re15_msg_decode_text(b, (size_t)n, t, sizeof t);
    printf("    Text: \"%s\"\n", t);
    PRUEF(!strcmp(t, "I cannot leave Ada alone here with those monsters!"),
          "msg 40 = NUTZER-VORGABE \"I cannot leave Ada alone here with those monsters!\"");
    PRUEF(n > 4 && b[0] == 0x04 && b[1] == 0x02 && b[n - 2] == 0x01 && b[n - 1] == 0x00 && b[n - 3] == 0x1A,
          "Erzaehlform `04 02 .. 1a 01 00` wie ROOM1090 msg 7 @0x28E6 / 1141 msg 41");
    int umbrueche = 0, kopf = 0;
    for (int i = 0; i < n; i++) { if (b[i] == 0x08) umbrueche++; if (b[i] == 0x05 || b[i] == 0x16) kopf++; }
    PRUEF(umbrueche == 1 && kopf == 0, "eine Zeile umgebrochen (0x08), kein Sprecherkopf (kein 0x05-Farbwechsel, kein ':')");
    int w = 0, wmax = 0;
    for (int i = 2; i < n - 2; i++) {
        if (b[i] == 0x08) { if (w > wmax) wmax = w; w = 0; continue; }
        w += re15_font_width[b[i]];
    }
    if (w > wmax) wmax = w;
    PRUEF(wmax <= 271, "breiteste Zeile %d px <= 271 (DEBUG.BIN @0x4416 Breiten)", wmax);
    re15_msg_install_text((unsigned char)RE15_R39O_MSG_SPERRE, b, (size_t)n);   /* der Port-Platz nimmt den Text an */
    PRUEF(re15_msg_is_choice(RE15_R39O_MSG_SPERRE) == 0, "msg 40 ist keine Ja/Nein-Frage (kein 0x03)");
}

/* ---- Teil: programme -------------------------------------------------------------------------------------- */
static void teil_programme(void)
{
    printf("== programme ==\n");
    if (rdt_laden(RE15_R39O_RAUM) != 0) { PRUEF(0, "ROOM11C0 ladbar"); return; }
    const uint8_t *d = s_raw;
    int n0 = 0, n1 = 0;
    const uint8_t *p0 = re15_parkplatz_r39_programm(0, &n0), *p1 = re15_parkplatz_r39_programm(1, &n1);
    static const uint8_t szene[14] = { 0x46, 0x00, 0x01, 0x31, 0x28, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x18, 0x02, 0x01, 0x00 };
    PRUEF(n0 == 14 && !memcmp(p0, szene, 14), "SZENE = `46 00 01 31 28 00 ff ff 00 00` Aot_reset Slot 0 -> MESSAGE 40, `18 02` "
          "Gosub sub02, `01 00`");
    {   /* RE1.5-Form der Zeile: ROOM11B0 sub01 @0x01216 `46 01 01 31 0f 00 ff ff 00 00` (sce/Flags/Maske gleich) */
        char rp[600]; size_t nb = 0;
        snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11B0.RDT", RE15_ASSET_PSX_DIR);
        uint8_t *b = slurp(rp, &nb);
        PRUEF(b && nb > 0x01220 && b[0x01216] == 0x46 && b[0x01218] == p0[2] && b[0x01219] == p0[3] && b[0x0121C] == p0[6] &&
              b[0x0121D] == p0[7], "SZENE-Zeile in der Form ROOM11B0 sub01 @0x01216 (sce 1, Flags 0x31, Maske 0xffff)");
        free(b);
    }
    PRUEF(n1 == 14 && p1[0] == 0x46 && p1[1] == 0 && p1[2] == 0x02 && p1[3] == d[0x01715] && !memcmp(p1 + 4, d + 0x01712 + 14, 6) &&
          p1[10] == 0x18 && p1[11] == 0x03 && p1[12] == 0x01, "ENDE = Aot_reset Slot 0 -> TUER, Flags = main00 @0x01715, Nutzlast = "
          "main00 @0x01712+14 (%02x %02x %02x %02x %02x %02x), Gosub sub03", p1[4], p1[5], p1[6], p1[7], p1[8], p1[9]);
    /* Kopf-Konstanten gegen das ausgelieferte RDT */
    PRUEF(d[0x01712] == 0x3B && d[0x01713] == RE15_R39O_TUER_SLOT && d[0x01714] == RE15_R39O_SCE_TUER && d[0x01715] == RE15_R39O_SAT &&
          d[0x01712 + 23] == RE15_R39O_ZIEL_RAUM, "main00 @0x01712 Door_aot_set Slot 0 sce 2 Flags 0x31 -> Raum 0x1B");
    PRUEF((int16_t)(d[0x01720] | d[0x01721] << 8) == RE15_R39O_ZIEL_X && (int16_t)(d[0x01722] | d[0x01723] << 8) == RE15_R39O_ZIEL_Y &&
          (int16_t)(d[0x01724] | d[0x01725] << 8) == RE15_R39O_ZIEL_Z, "Ziel (%d,%d,%d) = main00 @0x01712+14..+19",
          RE15_R39O_ZIEL_X, RE15_R39O_ZIEL_Y, RE15_R39O_ZIEL_Z);
    PRUEF(d[0x01820] == 0x21 && d[0x01821] == RE15_R39O_ADA_BANK && d[0x01822] == RE15_R39O_ADA_BIT && d[0x01823] == 0 &&
          d[0x01824] == 0x04 && d[0x01826] == 0x18 && d[0x01827] == RE15_R39O_SUB_SZENE,
          "sub01 @0x01820 Ck(4,64)==0 -> @0x01824 Evt_exec sub02");
    PRUEF(d[0x0182E] == 0x21 && d[0x0182F] == RE15_R39O_ENDE_BANK && d[0x01830] == RE15_R39O_ENDE_BIT &&
          d[0x01836] == 0x21 && d[0x01837] == RE15_R39O_KILL_BANK && d[0x01838] == RE15_R39O_KILL_1 && d[0x01839] == 1 &&
          d[0x0183E] == 0x21 && d[0x0183F] == RE15_R39O_KILL_BANK && d[0x01840] == RE15_R39O_KILL_2 && d[0x01841] == 1 &&
          d[0x01842] == 0x04 && d[0x01845] == RE15_R39O_SUB_ENDE,
          "sub01 @0x0182E Ck(3,67)==0, @0x01836 Ck(7,96)==1, @0x0183E Ck(7,97)==1 -> @0x01842 Evt_exec sub03");
    PRUEF(d[0x0184E] == 0x22 && d[0x0184F] == RE15_R39O_ADA_BANK && d[0x01850] == RE15_R39O_ADA_BIT && d[0x01851] == 1 &&
          d[0x01856] == 0x22 && d[0x01857] == 2 && d[0x01858] == 7 && d[0x01859] == 1 &&
          d[0x01ACE] == 0x22 && d[0x01ACF] == 2 && d[0x01AD0] == 7 && d[0x01AD1] == 0,
          "sub02 @0x0184E Set(4,64)=1, @0x01856 Pad-Sperre, @0x01ACE Steuerung zurueck");
    PRUEF(d[0x01B02] == 0x22 && d[0x01B03] == RE15_R39O_ENDE_BANK && d[0x01B04] == RE15_R39O_ENDE_BIT && d[0x01B05] == 1 &&
          d[0x01C08] == 0x47 && d[0x01C09] == RE15_R39O_TUER_SLOT, "sub03 @0x01B02 Set(3,67)=1 ... @0x01C08 Aot_on(0)");
    PRUEF(d[0x01784 + 2] == RE15_R39O_GORILLA_TYP && d[0x01784 + 7] == RE15_R39O_KILL_1 && d[0x01798 + 2] == RE15_R39O_GORILLA_TYP &&
          d[0x01798 + 7] == RE15_R39O_KILL_2, "sub00 Gorillas Typ 0x27 mit Kill-Flag 0x60/0x61 (pc[7])");
    g_current_room_id = RE15_R39O_RAUM;
    PRUEF(re15_parkplatz_r39_anker_ok(), "Anker (FNV main00-Tuer + sub00..sub03) im ausgelieferten RDT");
    PRUEF(re15_parkplatz_r39_evt_exec(RE15_R39O_RAUM, 2, s_rdt.sub_scd[2]) == p0 &&
          re15_parkplatz_r39_evt_exec(RE15_R39O_RAUM, 3, s_rdt.sub_scd[3]) == p1 &&
          re15_parkplatz_r39_evt_exec(RE15_R39O_RAUM, 7, s_rdt.sub_scd[7]) == s_rdt.sub_scd[7] &&
          re15_parkplatz_r39_evt_exec(0x11C1, 2, s_rdt.sub_scd[2]) == s_rdt.sub_scd[2],
          "Evt_exec-Haken: 11C0 sub02 -> SZENE, sub03 -> ENDE, sub07 und andere Raeume unveraendert");
}

/* ---- Teil: kampf (echter Weg ab dem Tuer-Spawn) --------------------------------------------------------------- */
static void teil_kampf(void)
{
    printf("== kampf ==\n");
    re15_game_state_init();
    /* Spawn aus ROOM11B0 main00 @0x00FAA (Slot 1 -> 0x1C): (-25279,0,17268) Blick 0x11FB, Cut 0 — gemessen exe vor2 */
    if (room_boot(RE15_R39O_RAUM, -25279, 17268, 0x11FB, 0) != 0) return;
    const unsigned s0 = g_re15_r39o_szene_zaehler;
    PRUEF(slot0_tuer(), "vor dem ersten Bild: Slot 0 = Tuer (erster Besuch, (4,64)=0 — die Szene sperrt selbst)");
    frame(0, 0);
    PRUEF(g_re15_r39o_szene_zaehler == s0 + 1, "erstes Bild: sub01 @0x01824 Evt_exec sub02 -> Programm SZENE");
    PRUEF(slot0_text(), "erstes Bild: Slot 0 = Text msg 40");
    PRUEF(re15_game_flag_get(RE15_R39O_ADA_BANK, RE15_R39O_ADA_BIT) == 1 && re15_game_flag_get(2, 7) == 1,
          "erstes Bild: (4,64)=1 (sub02 @0x0184E) und Pad-Sperre (2,7)=1 (@0x01856) in derselben Runde");
    int f_frei = -1, war_gesperrt = 1, text_ok = 1;
    for (int f = 1; f < 4000; f++) {
        frame(0, 0);
        if (!slot0_text()) text_ok = 0;
        if (g_scd.message_active && (f % 15) == 0) frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
        if (war_gesperrt && re15_game_flag_get(2, 7) == 0) { f_frei = f; break; }
    }
    PRUEF(f_frei > 0, "Steuerung zurueck (sub02 @0x01ACE Set(2,7)=0) in Bild %d", f_frei);
    PRUEF(text_ok && slot0_text(), "von der Szene bis zur Rueckgabe der Steuerung durchgehend Slot 0 = Text msg 40");
    PRUEF(n_typ(RE15_R39O_GORILLA_TYP) == 2 && re15_parkplatz_r39_kampf(), "der Kampf laeuft: zwei Gorillas, (4,64)=1, (3,67)=0");
    for (int f = 0; f < 60 && g_scd.player_mode != 0; f++) frame(0, 0);
    gorillas_starr();
    const unsigned v0 = g_re15_r39o_stimme_zaehler;
    druck_t r = druecke(TUER_X, TUER_Z, TUER_ROT, 240);
    PRUEF(r.msg_id == RE15_R39O_MSG_SPERRE && !r.raumwechsel,
          "Tuer im Kampf: msg %d \"I cannot leave Ada alone here with those monsters!\", kein Raumwechsel", r.msg_id);
    PRUEF(g_re15_r39o_stimme_zaehler == v0 + 1 && s_n_voice40 == 1, "Stimme msg 40 einmal eingereiht (Ordnerregel main40.wav)");
    PRUEF(slot0_text(), "nach dem Text bleibt Slot 0 der Text");
    PRUEF(laeuft_steuerbar(), "nach dem Text ist Leon steuerbar (laeuft weg, kein Raumwechsel)");
    r = druecke(TUER_X, TUER_Z, TUER_ROT, 240);
    PRUEF(r.msg_id == RE15_R39O_MSG_SPERRE && !r.raumwechsel, "zweiter Versuch: wieder msg %d, kein Raumwechsel", r.msg_id);
    kampfende_pruefen("kampf");
}

/* ---- Teil: wiedereintritt ----------------------------------------------------------------------------------- */
static void teil_wiedereintritt(void)
{
    printf("== wiedereintritt ==\n");
    re15_game_state_init();
    re15_game_flag_set(RE15_R39O_ADA_BANK, RE15_R39O_ADA_BIT, 1);   /* Ada-Szene gelaufen, Kampf nicht beendet */
    const unsigned e0 = g_re15_r39o_eintritt_zaehler;
    if (room_boot(RE15_R39O_RAUM, -25279, 17268, 0x11FB, 0) != 0) return;
    PRUEF(g_re15_r39o_eintritt_zaehler == e0 + 1 && slot0_text(), "VOR dem ersten Bild: Slot 0 = Text msg 40 (Installer)");
    PRUEF(n_typ(RE15_R39O_GORILLA_TYP) == 2, "sub00 @0x017E8/@0x017FC: zwei Gorillas neu gesetzt (Kampf laeuft weiter)");
    const unsigned s0 = g_re15_r39o_szene_zaehler;
    frame(0, 0);
    PRUEF(g_re15_r39o_szene_zaehler == s0, "keine Szene (sub01 @0x01820 Ck(4,64)==0 falsch)");
    gorillas_starr();
    druck_t r = druecke(TUER_X, TUER_Z, TUER_ROT, 240);
    PRUEF(r.msg_id == RE15_R39O_MSG_SPERRE && !r.raumwechsel, "Tuer: msg %d, kein Raumwechsel", r.msg_id);
    kampfende_pruefen("wiedereintritt");
}

/* ---- Teil: nachher ------------------------------------------------------------------------------------------ */
static void teil_nachher(void)
{
    printf("== nachher ==\n");
    re15_game_state_init();
    re15_game_flag_set(RE15_R39O_ADA_BANK, RE15_R39O_ADA_BIT, 1);
    re15_game_flag_set(RE15_R39O_ENDE_BANK, RE15_R39O_ENDE_BIT, 1);   /* Kampf vorbei (sub03 @0x01B02) */
    const unsigned e0 = g_re15_r39o_eintritt_zaehler;
    if (room_boot(RE15_R39O_RAUM, -25279, 17268, 0x11FB, 0) != 0) return;
    PRUEF(g_re15_r39o_eintritt_zaehler == e0 && slot0_tuer(), "Slot 0 = Tuer nach ROOM11B0");
    PRUEF(n_typ(RE15_R39O_GORILLA_TYP) == 0, "keine Gorillas (sub00 @0x017CC Ck(3,67)==0 falsch)");
    for (int f = 0; f < 30; f++) frame(0, 0);
    druck_t r = druecke(TUER_X, TUER_Z, TUER_ROT, 240);
    PRUEF(r.raumwechsel && r.ziel == 0x11B0 && r.msg_id != RE15_R39O_MSG_SPERRE,
          "Tuer nach dem Kampf: Raumwechsel ROOM%04X, kein Text", (unsigned)r.ziel);
}

/* ---- Teil: elza -------------------------------------------------------------------------------------------- */
static void teil_elza(void)
{
    printf("== elza ==\n");
    for (int k = 0; k < 2; k++) {
        re15_game_state_init();
        if (k) re15_game_flag_set(RE15_R39O_ADA_BANK, RE15_R39O_ADA_BIT, 1);
        const unsigned e0 = g_re15_r39o_eintritt_zaehler, s0 = g_re15_r39o_szene_zaehler;
        if (room_boot(0x11C1, -25279, 17268, 0x11FB, 0) != 0) return;
        PRUEF(g_aot.slots[0].active && g_aot.slots[0].type == RE15_AOT_TYPE_DOOR && g_re15_r39o_eintritt_zaehler == e0,
              "ROOM11C1 (Elza, (4,64)=%d): Slot 0 = Tuer, kein Installer-Eingriff", k);
        PRUEF(n_typ(RE15_R39O_GORILLA_TYP) == 0 && n_typ(0x42) == 0, "ROOM11C1: keine Gorillas, keine Ada (sub00 = Hunde 0x20)");
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) g_actors[s].grid_id |= 0x20;
        for (int f = 0; f < 30; f++) frame(0, 0);
        PRUEF(g_re15_r39o_szene_zaehler == s0, "ROOM11C1: kein Programm SZENE");
        druck_t r = druecke(TUER_X, TUER_Z, TUER_ROT, 240);
        PRUEF(r.raumwechsel && r.ziel == 0x11B1 && r.msg_id != RE15_R39O_MSG_SPERRE,
              "ROOM11C1: Tuer -> Raumwechsel ROOM%04X, kein Text", (unsigned)r.ziel);
    }
}

/* ---- Teil: fremd -------------------------------------------------------------------------------------------- */
static void teil_fremd(void)
{
    printf("== fremd ==\n");
    re15_game_state_init();
    re15_game_flag_set(RE15_R39O_ADA_BANK, RE15_R39O_ADA_BIT, 1);
    s_patch_off = 0x01A52;   /* sub02 @0x01A50 Sleep 0x50: Laengenbyte geaendert -> sub00..sub03-Anker weicht ab */
    const unsigned e0 = g_re15_r39o_eintritt_zaehler;
    if (room_boot(RE15_R39O_RAUM, -25279, 17268, 0x11FB, 0) != 0) { s_patch_off = -1; return; }
    s_patch_off = -1;
    PRUEF(!re15_parkplatz_r39_anker_ok(), "geaendertes RDT: Anker greift nicht");
    PRUEF(g_re15_r39o_eintritt_zaehler == e0 && slot0_tuer(), "geaendertes RDT: keine Sperre, Slot 0 bleibt die Tuer");
    PRUEF(re15_parkplatz_r39_evt_exec(RE15_R39O_RAUM, 2, s_rdt.sub_scd[2]) == s_rdt.sub_scd[2] &&
          re15_parkplatz_r39_evt_exec(RE15_R39O_RAUM, 3, s_rdt.sub_scd[3]) == s_rdt.sub_scd[3],
          "geaendertes RDT: Evt_exec sub02/sub03 laufen die Original-Subs");
}

static int s_n_seq = 0;
static void fang(const re15_door_seq_anfrage_t *a) { (void)a; s_n_seq++; }

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    const int alle = !strcmp(teil, "alle");
    re15_door_seq_setze_laeufer(fang);
    if (alle || !strcmp(teil, "texte"))          teil_texte();
    if (alle || !strcmp(teil, "programme"))      teil_programme();
    if (alle || !strcmp(teil, "kampf"))          teil_kampf();
    if (alle || !strcmp(teil, "wiedereintritt")) teil_wiedereintritt();
    if (alle || !strcmp(teil, "nachher"))        teil_nachher();
    if (alle || !strcmp(teil, "elza"))           teil_elza();
    if (alle || !strcmp(teil, "fremd"))          teil_fremd();
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler (Tuersequenzen %d)\n", teil, g_fail, s_n_seq);
    return g_fail ? 1 : 0;
}
