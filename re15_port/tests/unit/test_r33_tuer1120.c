/* test_r33_tuer1120.c — RIEGEL: Tuer ROOM1130 -> ROOM1120 erst nach der ersten Irons-Szene.
 *
 * Runde 33, Thema R (PORT-WAHL auf Nutzerwunsch). Dossier: analysis/befunde_runde33/tuer_1130_1120.md,
 * Konstanten und Belege: include/re15_tuer1120.h, engine/src/tuer1120_1130.c.
 *
 * Verfahren wie test_r30_tuer.c / probe_r31_tueren.c: die ausgelieferte RDT wird geladen, der Raum
 * ueber scd_room_reenter aufgebaut (derselbe Einhaengepunkt wie im Spiel: Tuer, Debug-Sprung, Laden),
 * der Spieler vor die Tuer gestellt (Vorwaerts-620-Punkt im Rechteck, FUN_80042bac @0x80042bd0) und
 * EINMAL Quadrat gedrueckt — der echte Aktions-Scan re15_aot_scan im Spielschritt.
 *
 * Teile (je ein ctest-Eintrag):
 *   gesperrt  ROOM1130, Flag (3,94) = 0: Slot 1 ist Text-Platz (sce 1, flags 0x31, Maske 0xffff,
 *             msg 6, Rechteck + Band der Tuer), die Nachricht lautet woertlich "I have to report the
 *             situation to the chief first...", Druck -> Nachricht 6 steht, Freeze 0xffff0000,
 *             150 Bilder lang KEIN Raumwechsel, KEINE Tuersequenz-Anfrage; die sechs Raum-
 *             Nachrichten 0..5 sind unveraendert.
 *   frei      ROOM1130, Flag (3,94) = 1: Slot 1 ist die Original-Tuer (Door_aot_set @0x008AE),
 *             Druck -> Raumwechsel nach ROOM1120 und genau eine Tuersequenz (S060 DOOR09 V0).
 *   szene     ECHTE Freigabe: ROOM1150 mit Flag 0, Spieler in den AUTO-Platz Slot 6 (main00
 *             @0x00DEA) -> sub08 laeuft im Port-VM und setzt (3,94) selbst (@0x01110); danach
 *             ROOM1130 -> Tuer frei. Gegenprobe vorher: ohne Szene gesperrt.
 *   elza      ROOM1131, Flag 0: Slot 1 bleibt Tuer, Druck -> ROOM1121 (Elza hat keine Irons-Szene).
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
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_door_seq.h"
#include "re15_tuer1120.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;

static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR,
             (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    return 0;
}

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
}

/* Raum aufbauen OHNE die Flags anzufassen (die gehoeren dem Spiel, nicht dem Raum). */
static int room_boot(uint16_t room, int32_t px, int32_t pz, int warm_druck)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    /* scd_vm_init ist die NEUES-SPIEL-Initialisierung und nullt die Flags (scd_vm.c:508
     * re15_game_state_init). Ein Raumwechsel im Spiel ruft sie nicht (scd_room_setup.c:112
     * "= scd_vm_init MINUS re15_game_state_init (keep flags)") — also hier die Flags retten. */
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = 0;
    re15_collision_set_band(0);
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    /* Reihenfolge wie room_common.c: der Nachrichtenblock kommt NACH scd_room_reenter (:354/:392). */
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int f = 0; f < 30; f++) frame(0, 0);
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

typedef struct { int stand, raumwechsel, ziel, msg_id, freeze, seq; } druck_t;

/* Vor Slot `slot` stellen (Vorwaerts-620-Punkt in dessen Rechteck, Spieler bleibt stehen) und
 * einmal Quadrat druecken. Danach bis zu 150 Bilder beobachten, offene Nachrichten blaettern. */
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

static const char k_text[] = "I have to report the situation to the chief first...";

/* ------------------------------------------------------------------------------------------- */
static void teil_gesperrt(void)
{
    printf("[gesperrt] ROOM1130, Flag (3,94) = 0\n");
    re15_game_state_init();
    if (room_boot(0x1130, -3050, 0, 1) != 0) return;
    const re15_aot_t *a = &g_aot.slots[RE15_TUER1120_SLOT];
    PRUEF(re15_tuer1120_gesperrt() == 1, "Sperre gesetzt");
    PRUEF(a->active && a->type == RE15_AOT_TYPE_MESSAGE, "Slot 1 ist Text-Platz (Typ %d)", a->type);
    PRUEF(a->event_id == RE15_TUER1120_MSG_ID && a->pause_mask16 == 0xffff && a->sce_flags == 0x31,
          "Nutzlast msg %d Maske 0x%04x flags 0x%02x (Soll 6 / 0xffff / 0x31)", a->event_id,
          a->pause_mask16, a->sce_flags);
    /* Rechteck der Tuer @0x008AE: (-3550,-3150,1000,2000) -> Mitte (-3050,-2150), halb (500,1000). */
    PRUEF(!a->has_quad && a->x == -3050 && a->z == -2150 && a->half_w == 500 && a->half_h == 1000 && a->band == 0,
          "Rechteck/Band der Tuer (%d,%d) halb (%d,%d) Band %d", (int)a->x, (int)a->z, (int)a->half_w,
          (int)a->half_h, a->band);
    const char *t = re15_msg_get_text(RE15_TUER1120_MSG_ID);
    PRUEF(t && strcmp(t, k_text) == 0, "Text \"%s\"", t ? t : "(keiner)");
    int n = 0; const uint8_t *m = re15_tuer1120_meldung(&n);
    int rn = 0; const unsigned char *raw = re15_msg_get_raw(RE15_TUER1120_MSG_ID, &rn);
    PRUEF(raw && rn == n && memcmp(raw, m, (size_t)n) == 0 && m[0] == 0x04 && m[1] == 0x02 &&
          m[n - 2] == 0x01 && m[n - 1] == 0x00, "Rohbytes %d B, Kopf 04 02, Ende 01 00", rn);
    PRUEF(re15_msg_is_choice(RE15_TUER1120_MSG_ID) == 0, "keine Ja/Nein-Abfrage");
    const char *t1 = re15_msg_get_text(1);
    const char *t5 = re15_msg_get_text(5);
    PRUEF(t1 && strcmp(t1, "It's not necessary to go back.") == 0 && t5 && strncmp(t5, "A shelf with trophies.", 22) == 0,
          "Raum-Nachrichten 1/5 unveraendert");
    druck_t r = drueck_vor(RE15_TUER1120_SLOT);
    printf("  Druck: Standplatz %d, Nachricht %d, Freeze %d, Raumwechsel %d, Sequenzen %d\n",
           r.stand, r.msg_id, r.freeze, r.raumwechsel, r.seq);
    PRUEF(r.stand, "Standplatz vor der Tuer gefunden");
    PRUEF(r.msg_id == RE15_TUER1120_MSG_ID, "Quadrat oeffnet Nachricht 6");
    PRUEF(r.freeze, "Text-Freeze 0xffff0000 wie jeder sce-1-Text (@0x80043098)");
    PRUEF(!r.raumwechsel, "kein Raumwechsel in 150 Bildern");
    PRUEF(r.seq == 0, "keine Tuersequenz angefragt");
    PRUEF(g_current_room_id == 0x1130, "Raum bleibt ROOM1130");
    /* zweiter Druck nach dem Schliessen: wieder die Nachricht, wieder kein Wechsel */
    druck_t r2 = drueck_vor(RE15_TUER1120_SLOT);
    PRUEF(r2.msg_id == RE15_TUER1120_MSG_ID && !r2.raumwechsel && r2.seq == 0, "zweiter Druck ebenso");
}

static void teil_frei(void)
{
    printf("[frei] ROOM1130, Flag (3,94) = 1\n");
    re15_game_state_init();
    re15_game_flag_set(RE15_TUER1120_FLAG_BANK, RE15_TUER1120_FLAG_BIT, 1);
    if (room_boot(0x1130, -3050, 0, 1) != 0) return;
    const re15_aot_t *a = &g_aot.slots[RE15_TUER1120_SLOT];
    const re15_aot_door_params_t *d = &g_aot.door_params[RE15_TUER1120_SLOT];
    PRUEF(re15_tuer1120_gesperrt() == 0, "keine Sperre");
    PRUEF(a->active && a->type == RE15_AOT_TYPE_DOOR && d->dest_room == 0x12 && d->target_cut == 3,
          "Slot 1 ist die Original-Tuer (Typ %d, Ziel 0x%02x, Cut %d)", a->type, d->dest_room, d->target_cut);
    druck_t r = drueck_vor(RE15_TUER1120_SLOT);
    printf("  Druck: Standplatz %d, Nachricht %d, Raumwechsel %d -> ROOM%04X, Sequenzen %d (S%03u DOOR%02X V%d)\n",
           r.stand, r.msg_id, r.raumwechsel, (unsigned)r.ziel, r.seq, s_seq.seite, s_seq.re2_nr, s_seq.variante);
    PRUEF(r.stand && r.raumwechsel && r.ziel == 0x1120, "Quadrat -> Raumwechsel nach ROOM1120");
    PRUEF(r.msg_id < 0, "keine Nachricht");
    PRUEF(r.seq == 1 && s_seq.seite == 60 && s_seq.re2_nr == 0x09 && s_seq.variante == 0,
          "genau eine Tuersequenz S060 DOOR09 V0 (tuer_zuordnung.inc:84)");
    g_room_change.pending = 0;
}

static void teil_szene(void)
{
    printf("[szene] ROOM1150 sub08 setzt (3,94) selbst, danach ROOM1130\n");
    re15_game_state_init();
    /* Gegenprobe: ohne Szene gesperrt */
    if (room_boot(0x1130, -3050, 0, 1) != 0) return;
    PRUEF(re15_tuer1120_gesperrt() == 1 && g_aot.slots[RE15_TUER1120_SLOT].type == RE15_AOT_TYPE_MESSAGE,
          "vor der Szene gesperrt");
    /* ROOM1150: in den AUTO-Platz Slot 6 (main00 @0x00DEA, Rechteck (-27300,-23800,11500,2000) ->
     * Mitte (-21550,-22800)). Kein Aufwaermen mit Tastendruck — die Szene soll selbst anlaufen. */
    if (room_boot(0x1150, -20500, -22800, 0) != 0) return;
    /* Der Spieler steht schon beim Aufbau im AUTO-Platz: sub08 startet in den 30 Aufbau-Bildern
     * und schaltet Slot 6 mit seinem ERSTEN Opcode ab (@0x01106 Aot_reset 6 -> sce 0). Belegt wird
     * deshalb, dass Slot 6 jetzt abgeschaltet ist UND das Flag steht — nicht der Typ davor. */
    const re15_aot_t *z = &g_aot.slots[6];
    int gesetzt_bei = re15_game_flag_get(3, 94) ? 0 : -1;
    PRUEF(z->type == RE15_AOT_TYPE_NONE && gesetzt_bei == 0,
          "ROOM1150: AUTO-Platz 6 hat sub08 gestartet (Slot 6 Typ %d = abgeschaltet, (3,94) = %d)",
          z->type, re15_game_flag_get(3, 94));
    /* Die Szene laeuft: sub08 hat seine Szenen-Klammer gesetzt (@0x01114 `22 02 07 01` Set(2,7)),
     * derselbe Thread, der zwei Opcodes davor (3,94) gesetzt hat. Bis zum Ende laeuft sie in diesem
     * Geruest nicht (player_mode leitet erst main.c aus (2,7) ab) — das zeigt die Abnahme im Spiel. */
    PRUEF(re15_game_flag_get(2, 7) == 1, "sub08 laeuft (Szenen-Klammer (2,7) = 1, @0x01114)");
    g_room_change.pending = 0;
    if (room_boot(0x1130, -3050, 0, 1) != 0) return;
    PRUEF(re15_tuer1120_gesperrt() == 0 && g_aot.slots[RE15_TUER1120_SLOT].type == RE15_AOT_TYPE_DOOR,
          "nach der Szene: Slot 1 wieder Tuer");
    druck_t r = drueck_vor(RE15_TUER1120_SLOT);
    PRUEF(r.stand && r.raumwechsel && r.ziel == 0x1120 && r.msg_id < 0, "Quadrat -> ROOM1120");
    g_room_change.pending = 0;
}

static void teil_elza(void)
{
    printf("[elza] ROOM1131, Flag (3,94) = 0\n");
    re15_game_state_init();
    if (room_boot(0x1131, -3050, 0, 1) != 0) return;
    const re15_aot_t *a = &g_aot.slots[RE15_TUER1120_SLOT];
    const re15_aot_door_params_t *d = &g_aot.door_params[RE15_TUER1120_SLOT];
    PRUEF(re15_tuer1120_gesperrt() == 0, "keine Sperre fuer Elza");
    PRUEF(a->active && a->type == RE15_AOT_TYPE_DOOR && d->dest_room == 0x12,
          "Slot 1 bleibt Tuer (Typ %d, Ziel 0x%02x)", a->type, d->dest_room);
    druck_t r = drueck_vor(RE15_TUER1120_SLOT);
    printf("  Druck: Standplatz %d, Nachricht %d, Raumwechsel %d -> ROOM%04X\n",
           r.stand, r.msg_id, r.raumwechsel, (unsigned)r.ziel);
    PRUEF(r.stand && r.raumwechsel && r.ziel == 0x1121 && r.msg_id < 0, "Quadrat -> ROOM1121");
    g_room_change.pending = 0;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    re15_door_seq_setze_laeufer(fang);
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "gesperrt")) teil_gesperrt();
    if (alle || !strcmp(teil, "frei"))     teil_frei();
    if (alle || !strcmp(teil, "szene"))    teil_szene();
    if (alle || !strcmp(teil, "elza"))     teil_elza();
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
