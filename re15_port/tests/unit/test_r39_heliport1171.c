/* test_r39_heliport1171.c — RIEGEL Runde 39 Spur K "heliport1171": Johns Animationen in Elzas Dachszene ROOM1171.
 *
 * NUTZER: "Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran
 * wink Animation, die er im Original nicht macht. Auch sonst scheinen die Animationen hier noch nicht ganz zu passen."
 *
 * Dossier: analysis/befunde_runde39/K_heliport1171.md. Ursache: der lazy RBJ-Kanal-Cache des Ports
 * (engine/src/enemy_common.c, 2 Plaetze) wurde von den Ereignis-Kraehen (state 4 / Sub 0 ab Bild 0, vom Renderer
 * abgefragt) belegt; John (rec1 Marker 0x2) bekam keinen Platz und posierte seine Sub-0-Gesten aus Bank 0 — Clip 2 dort
 * ist der Kletter-Clip (+0x94 = 2 @0x800526a0, Kanal +0x84/+0x16c). Original: Marker-Binder FUN_8001b3f8 (@0x80039a08)
 * bindet JEDES Marker-Bit ohne Grenze (@0x8001b470-c4) an +0x180/+0x184; Sub 0 spielt +0x180/+0x184 (@0x80050d40/48).
 *
 * Verfahren wie test_r38_heliport1171.c (ausgelieferte RDT, scd_room_reenter, Bilder in main.c-Reihenfolge), dazu die
 * RBJ-Abfrage des Renderers (platform/pc/main.c Executor-Kanal-Render: JEDER Aktor in state 4 / Sub 0 ohne Walk, in
 * Slot-Reihenfolge) — ohne sie zeigt der Unit-Lauf den Fehler nicht.
 *
 * Teile (je ein ctest-Eintrag):
 *   banken    Johns Baenke (CDEMD1 Typ 0x49): Bank 0 Clip 2 = 50 Bilder (Kletter-Clip), eigene Bank 14 Clips, Clip 6 = 10;
 *             ROOM1171-RBJ: rec0 Marker 0x1 {20,30}, rec1 Marker 0x2 {20,30,96}
 *   binder    Binder-Eigenschaft: Slots OHNE Record (Kraehen, beliebig viele) verdraengen Johns Record nicht;
 *             Gegenprobe ROOM10D0 (Marvin rec1) und ROOM1170 (Elliot bleibt ausgenommen)
 *   szene     ganze Szene (Elza, Standard-KI): Johns Kanal + Clip-Laenge je Plc_motion-Schritt = Original
 *             (Sub 0 -> RBJ rec1, Sub 2 -> eigene Bank), "Elza, hurry up!" = RBJ rec1 Clip 2 (96 Bilder), nicht 50
 *   leon1170  Gegenprobe Leon ROOM1170: kein Aktor bindet einen RBJ-Record ueber den Cache (Elliot ausgenommen, Kraehen
 *             ohne Record) — vorher wie nachher dieselbe Bankwahl
 * "Auch sonst" (Dossier Abschnitt 6, jede Szenen-Animation Johns gegen das Original):
 *   initpose  K2: bis zum ersten Plc_motion INIT-Pose = Loco-Paar Clip 2 Bild 0 (f314(+0x84,+0x16c) @0x8011debc-c4)
 *   eskorte   K2-Gegenprobe: ohne Gegner posiert die Eskorte-EXEC ab dem 1. Takt die eigene Bank (@0x8004f384-8c)
 *   saatbild  K3: jeder Plc_motion-Saattakt posiert Bild 0 bzw. fc-1 (@0x80050cec-d4c), Halten ab Bild fc-1
 *   schub     K4: Schub-Bit-Takt posiert noch Clip 0x12 (@0x80052af4 vor @0x80052b44), dann Clip 0x11 rueckw. 9..0
 * Nachbesserung 1 (Abnahme 0, Dossier Abschnitt 12):
 *   elza      M1/M3a/M2: Elzas Sub 6 (Clip 1 Bild 0..15, kein Einzelbild, Ankunftsbild = Gang-Pose), Spieler-Schub-Ausgang
 *   kraehen   M3b: die Ereignis-Kraehen zeichnen das zuletzt posierte Bild (ORIENT ab 0, Landung, FINISH 0..11, Leiche 11) */
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
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_gameflow.h"
#include "re15_ai_flavor.h"
#include "re15_roy_r37.h"
#include "re15_elzaems_r37.h"
#include "re15_npckante_r38.h"
#include "re15_heliport1171_r39.h"   /* K2 Merker, RE15_R39K_INIT_CLIP/_BILD */
#include "re15_szenenleon_r36.h"     /* RE15_SZL_MODUS_SCHUB, Clips 0x11/0x12 */
#include "re15_anim_select.h"        /* re15_actor_anim_reverse */
#include "re15_md1.h"                /* re15_pld_part (Nachbesserung 1: Elzas W-Bank-Laengen) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ---- Belege (Dossier Abschnitt 2) ------------------------------------------------------------------------------ */
#define K_JOHN_TYP          0x49u   /* sub07 Sce_em_set @0x016C8 `44 00 49 00 ...`                                       */
#define K_KRAEHE_TYP        0x21u   /* sub07 @0x016DC / @0x016F0 `44 0x 21 40 ...`                                       */
#define K_REC1_CLIPS        3       /* ROOM1171 RBJ rec1 (Marker 0x2) EDD @RBJ+0x3E34: {20,30,96}                         */
#define K_RBJ_C0            20      /* Plc_motion(0,0,0) @0x01416/@0x0141E/@0x01526/@0x0152E                              */
#define K_RBJ_C2            96      /* Plc_motion(0,2,0) @0x014CE = "Elza, hurry up!" (Heran-Winken)                      */
#define K_OWN_C6            10      /* Plc_motion(2,6,0) @0x0142A/@0x014C2, Sub 2 = +0x170/+0x174 (@0x80050f88/90)        */
#define K_LOCO_C2_KLETTERN  50      /* Bank 0 Clip 2 = RE15_NPK_KANTE_CLIP_HOCH (+0x94 = 2 @0x800526a0, Kanal +0x84/+0x16c) */

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;
static int                s_bild = 0;

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

/* Blob `type` aus dem Archiv nach TOC (re15_ems_toc — dieselbe Funktion wie der PC-Lader). */
static uint8_t s_blob[0x40000];
static size_t blob_lesen(int archiv, uint8_t type)
{
    uint32_t sek = 0, len = 0;
    if (!re15_ems_toc(archiv, type, &sek, &len) || len > sizeof s_blob) return 0;
    char p[600]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(archiv));
    FILE *f = fopen(p, "rb");
    if (!f) return 0;
    size_t n = 0;
    if (fseek(f, (long)sek * 2048L, SEEK_SET) == 0) n = fread(s_blob, 1, len, f);
    fclose(f);
    return n == len ? len : 0;
}

/* Registry-Bank wie pc_enemy_load (RE1.5-Weg; Elza = CDEMD1, FUN_80022300 @0x800223ac). */
static uint8_t s_bank_buf[4][0x40000];
static int bank_laden(uint8_t type, int archiv, int buf)
{
    size_t len = blob_lesen(archiv, type);
    if (!len) return -1;
    memcpy(s_bank_buf[buf], s_blob, len);
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return -1;
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank_buf[buf], len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return -1;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank_buf[buf], len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank_buf[buf], len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank_buf[buf], len, &eb->skel_own, &eb->anim_own) == 0);
    return 0;
}

static int rdt_laden(uint16_t room)
{
    if (s_raw && s_rdt_raum == room) return 0;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

/* Die RBJ-Abfrage des Renderers (platform/pc/main.c, Executor-Kanal-Render): Aktor-Schleife npc_i = 1.. in
 * Slot-Reihenfolge, Bedingung `npc->state == 4 && npc->sub_state_1 == 0 && !npc->walk_active` OHNE Typ-Tor. */
static void render_abfrage(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active) continue;
        if (a->state == 4 && a->sub_state_1 == 0 && !a->walk_active) {
            (void)re15_actor_rbj_skel(s);
            (void)re15_actor_rbj_anim(s);
        }
    }
}

/* Ein Bild in der Reihenfolge von platform/pc/main.c (Muster test_r38_heliport1171.c), danach der Render-Teil. */
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
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
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
    render_abfrage();
    s_bild++;
}

static int room_boot(uint16_t room, int32_t px, int32_t py, int32_t pz, int16_t rot, uint8_t cut,
                     const uint8_t *typen, int n, int archiv)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    for (int i = 0; i < n; i++)
        if (bank_laden(typen[i], archiv, i) != 0) { printf("  FEHLER: Bank 0x%02X\n", typen[i]); g_fail++; }
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = py;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(re15_collision_band_from_y(py));
    s_shown = cut; s_cine_was = 0; s_bild = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static int fc(const re15_emd_animation_t *a, int c) { return (a && c >= 0 && c < a->clip_count) ? a->clips[c].frame_count : -1; }

/* RBJ-Record r des geladenen Raums: Marker (u32 @EMR-Praefix) und Clip-Laengen (eigene EDD). */
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static int rbj_record(int r, uint32_t *marker, int *laengen, int max)
{
    const uint8_t *rbj = s_rdt.animation; size_t n = (size_t)s_rdt.animation_size;
    if (!rbj || n < 0x20) return -1;
    uint32_t tl = rd32(rbj), nrec = rbj[4];
    if ((uint32_t)r >= nrec || tl + nrec * 8u > n) return -1;
    uint32_t pre = rd32(rbj + tl + (uint32_t)r * 8u), edd = rd32(rbj + tl + (uint32_t)r * 8u + 4u);
    if (pre + 4u > n || edd + 4u > n) return -1;
    if (marker) *marker = rd32(rbj + pre);
    static re15_emd_animation_t an;
    if (re15_emd_parse_animation(rbj + edd, n - edd, &an) != 0) return -1;
    for (int c = 0; c < an.clip_count && c < max; c++) laengen[c] = an.clips[c].frame_count;
    return an.clip_count;
}

/* ---- banken ---------------------------------------------------------------------------------------------------- */
static void teil_banken(void)
{
    printf("== banken ==\n");
    size_t n = blob_lesen(RE15_EMS_ARCHIV_ELZA, (uint8_t)K_JOHN_TYP);
    PRUEF(n > 0, "CDEMD1 Typ 0x49 (John) ueber TOC @0x80073178 lesbar (%u B)", (unsigned)n);
    if (!n) return;
    static re15_emd_skeleton_t sk; static re15_emd_animation_t loco, own;
    int ok_l = re15_emd_parse_loco_bank(s_blob, n, &sk, &loco) == 0;
    int ok_o = re15_emd_parse_own_bank(s_blob, n, &sk, &own) == 0;
    PRUEF(ok_l && loco.clip_count == 24 && fc(&loco, RE15_NPK_KANTE_CLIP_HOCH) == K_LOCO_C2_KLETTERN,
          "Bank 0 (dir[1], Loco +0x16c/+0x84): %d Clips, Clip %d (Kletter-Clip 'Kante hoch' @0x800526a0) = %d Bilder",
          loco.clip_count, RE15_NPK_KANTE_CLIP_HOCH, fc(&loco, RE15_NPK_KANTE_CLIP_HOCH));
    PRUEF(ok_o && own.clip_count == 14 && fc(&own, 6) == K_OWN_C6 && fc(&own, 2) == 52,
          "eigene Bank (dir[3], +0x174/+0x170 @0x800224b8/c8): %d Clips, Clip 6 = %d, Clip 2 = %d Bilder",
          own.clip_count, fc(&own, 6), fc(&own, 2));
    if (rdt_laden(0x1171) != 0) { PRUEF(0, "ROOM1171.RDT ladbar"); return; }
    uint32_t m0 = 0, m1 = 0; int l0[8] = {0}, l1[8] = {0};
    int n0 = rbj_record(0, &m0, l0, 8), n1 = rbj_record(1, &m1, l1, 8);
    PRUEF(n0 == 2 && m0 == 0x1u && l0[0] == 20 && l0[1] == 30,
          "ROOM1171 RBJ rec0: Marker 0x%X (Spieler), %d Clips {%d,%d}", (unsigned)m0, n0, l0[0], l0[1]);
    PRUEF(n1 == K_REC1_CLIPS && m1 == 0x2u && l1[0] == K_RBJ_C0 && l1[1] == 30 && l1[2] == K_RBJ_C2,
          "ROOM1171 RBJ rec1: Marker 0x%X (Gegner 0 = John, Binder @0x8001b4a0), %d Clips {%d,%d,%d}",
          (unsigned)m1, n1, l1[0], l1[1], l1[2]);
}

/* ---- binder ---------------------------------------------------------------------------------------------------- */
/* Aktor s als "Zuschauer" ohne Record (Typ t) in state 4 / Sub 0 — so, wie der Renderer ihn abfragt. */
static void zuschauer(int s, uint8_t t)
{
    re15_actor_t *a = &g_actors[s];
    memset(a, 0, sizeof *a);
    a->active = 1; a->type = t; a->state = 4; a->sub_state_1 = 0; a->hp = 10;
}
static void teil_binder(void)
{
    printf("== binder ==\n");
    /* ROOM1171: John Aktor 1 (= Gegner 0, Marker-Bit 1), alle anderen Slots Kraehen ohne Record. */
    if (rdt_laden(0x1171) != 0) { PRUEF(0, "ROOM1171.RDT ladbar"); return; }
    re15_actor_init();
    re15_enemy_reset();
    if (bank_laden((uint8_t)K_JOHN_TYP, RE15_EMS_ARCHIV_ELZA, 0) != 0 || bank_laden((uint8_t)K_KRAEHE_TYP, RE15_EMS_ARCHIV_ELZA, 1) != 0) {
        PRUEF(0, "Baenke 0x49/0x21 (CDEMD1)"); return;
    }
    re15_rbj_bind_room(s_rdt.animation, (size_t)s_rdt.animation_size);
    zuschauer(1, (uint8_t)K_JOHN_TYP);
    zuschauer(2, (uint8_t)K_KRAEHE_TYP);
    zuschauer(3, (uint8_t)K_KRAEHE_TYP);
    /* Reihenfolge wie im Spiel: ab Bild 0 fragt der Renderer die Kraehen ab, John (state 1) noch nicht. */
    g_actors[1].state = 1;
    render_abfrage(); render_abfrage();
    PRUEF(re15_actor_rbj_anim(2) == NULL && re15_actor_rbj_anim(3) == NULL,
          "Kraehen (Marker-Bits 0x4/0x8 in keinem Record) haben keinen RBJ-Kanal");
    g_actors[1].state = 4;                                  /* Plc_motion @0x80041bb0 */
    const re15_emd_animation_t *a = re15_actor_rbj_anim(1);
    PRUEF(a && a->clip_count == K_REC1_CLIPS && fc(a, 2) == K_RBJ_C2 && fc(a, 0) == K_RBJ_C0,
          "John nach den Kraehen: RBJ rec1 gebunden (%d Clips, Clip 0 = %d, Clip 2 = %d) = FUN_8001b3f8 ohne Grenze",
          a ? a->clip_count : -1, fc(a, 0), fc(a, 2));
    /* Belastung: ALLE uebrigen Slots ohne Record zuerst abgefragt — der Record darf nicht verdraengt werden. */
    re15_rbj_bind_room(s_rdt.animation, (size_t)s_rdt.animation_size);
    for (int s = 2; s < RE15_ACTOR_MAX; s++) zuschauer(s, (uint8_t)K_KRAEHE_TYP);
    g_actors[1].state = 1;
    for (int k = 0; k < 3; k++) render_abfrage();
    g_actors[1].state = 4;
    a = re15_actor_rbj_anim(1);
    PRUEF(a && a->clip_count == K_REC1_CLIPS, "John nach %d abgefragten Slots ohne Record: RBJ rec1 (%d Clips)",
          RE15_ACTOR_MAX - 2, a ? a->clip_count : -1);
    PRUEF(re15_actor_rbj_skel(1) != NULL, "Skelett-Kanal +0x180 (EMR rec1) ebenfalls gebunden");

    /* Gegenprobe ROOM10D0 (Leon): Marvin (0x40) Gegner 0 <- rec1 (Marker 0x2) neben Zombies ohne Record. */
    if (rdt_laden(0x10D0) != 0) { PRUEF(0, "ROOM10D0.RDT ladbar"); return; }
    uint32_t m1 = 0; int l1[16] = {0};
    int n1 = rbj_record(1, &m1, l1, 16);
    re15_actor_init();
    re15_enemy_reset();
    if (bank_laden(0x40, RE15_EMS_ARCHIV_LEON, 0) != 0) { PRUEF(0, "Bank 0x40 (CDEMD0)"); return; }
    re15_rbj_bind_room(s_rdt.animation, (size_t)s_rdt.animation_size);
    zuschauer(1, 0x40);
    for (int s = 2; s < RE15_ACTOR_MAX; s++) zuschauer(s, 0x10);
    g_actors[1].state = 1;
    render_abfrage();
    g_actors[1].state = 4;
    a = re15_actor_rbj_anim(1);
    PRUEF(m1 == 0x2u && a && a->clip_count == n1 && n1 > 0 && fc(a, 0) == l1[0],
          "ROOM10D0: Marvin (Aktor 1) nach den Zombies an rec1 gebunden (%d Clips, Marker 0x%X)", a ? a->clip_count : -1,
          (unsigned)m1);

    /* Gegenprobe ROOM1170 (Leon): Elliot (0x47) bleibt ausgenommen (O3, re15_typ47_ist_elliot) — auch neben Kraehen. */
    if (rdt_laden(0x1170) != 0) { PRUEF(0, "ROOM1170.RDT ladbar"); return; }
    re15_actor_init();
    re15_enemy_reset();
    re15_elzaems_r37_reset();
    if (bank_laden((uint8_t)K_KRAEHE_TYP, RE15_EMS_ARCHIV_LEON, 1) != 0) { PRUEF(0, "Bank 0x21 (CDEMD0)"); return; }
    re15_rbj_bind_room(s_rdt.animation, (size_t)s_rdt.animation_size);
    zuschauer(1, 0x47);
    for (int s = 2; s < 9; s++) zuschauer(s, (uint8_t)K_KRAEHE_TYP);
    render_abfrage();
    int gebunden = 0;
    for (int s = 1; s < 9; s++) if (re15_actor_rbj_anim(s)) gebunden++;
    PRUEF(gebunden == 0, "ROOM1170: kein Aktor bindet ueber den Cache (Elliot ausgenommen, Kraehen ohne Record): %d", gebunden);
}

/* ---- szene ----------------------------------------------------------------------------------------------------- */
static int stelle_von(int slot)
{
    const scd_thread_t *t = &g_scd.threads[slot];
    return (t->active && t->pc) ? (int)(t->pc - s_rdt.raw) : -1;
}

/* Johns Plc_motion-Schritte in sub02 (Fenster = die Sleep-Zeile hinter dem Befehl, PC steht dort waehrend des Wartens). */
typedef struct {
    int lo, hi;             /* Thread-PC-Fenster (Datei-Offset)                                             */
    int sub, motion, len;   /* Original: Sub (+0x5 = pc[1]), Clip (+0x94 = pc[2]), Bildzahl des KANALS      */
    int rueck;              /* Plc_flg(0,0x80,0) im selben Bild (@0x80050d44/50: a2 = +0x1C4 >> 7)          */
    const char *was;
    int gesehen, abweichung, ist_len, ist_motion, ist_sub, ist_rueck;
} schritt_t;

static void teil_szene(void)
{
    printf("== szene ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_elzaems_r37_reset();
    g_gameflow.character = 4;
    re15_game_flag_set(3, 0xC2, 1);                         /* ROOM1141 sub02 @0x00C20 */
    static const uint8_t typen[] = { K_KRAEHE_TYP, K_JOHN_TYP };
    if (room_boot(0x1171, 2300, -7200, 14365, 1024, 0, typen, 2, RE15_EMS_ARCHIV_ELZA) != 0) return;
    int john = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == K_JOHN_TYP) john = s;
    PRUEF(john == 1, "John = Aktor %d (Sce_em_set Slot 0 @0x016C8 -> Marker-Bit 1)", john);
    if (john < 0) return;
    schritt_t st[] = {
        { 0x0141A, 0x0141E, 0, 0, K_RBJ_C0, 0, "Plc_motion(0,0,0) @0x01416 (RBJ rec1 Clip 0)" },
        { 0x01426, 0x0142A, 0, 0, K_RBJ_C0, 1, "Plc_motion(0,0,0) @0x0141E + Plc_flg 0x80 @0x01422 (rueckwaerts)" },
        { 0x0142E, 0x01432, 2, 6, K_OWN_C6, 0, "Plc_motion(2,6,0) @0x0142A (eigene Bank Clip 6)" },
        { 0x014CA, 0x014CE, 2, 6, K_OWN_C6, 1, "msg 0: Plc_motion(2,6,0) @0x014C2 + Plc_flg 0x80 @0x014C6" },
        { 0x014D2, 0x014D6, 0, 2, K_RBJ_C2, 0, "msg 0: Plc_motion(0,2,0) @0x014CE = Heran-Winken (RBJ rec1 Clip 2)" },
        { 0x0152A, 0x0152E, 0, 0, K_RBJ_C0, 0, "msg 1: Plc_motion(0,0,0) @0x01526" },
        { 0x01536, 0x0153A, 0, 0, K_RBJ_C0, 1, "msg 1: Plc_motion(0,0,0) @0x0152E + Plc_flg 0x80 @0x01532" },
    };
    const int nst = (int)(sizeof st / sizeof st[0]);
    for (int i = 0; i < nst; i++) { st[i].ist_len = st[i].ist_motion = st[i].ist_sub = st[i].ist_rueck = -1; }
    int kletter = 0, tuer = -1;
    re15_actor_t *j = &g_actors[john];
    for (int f = 0; f < 2400 && tuer < 0; f++) {
        frame();
        for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {
            int o = stelle_von(s);
            if (o < 0) continue;
            for (int i = 0; i < nst; i++) {
                if (o < st[i].lo || o >= st[i].hi) continue;
                int len  = re15_actor_clip_len(j);                       /* die EINE Uhr = Renderer-Bank */
                int rueck = (j->anim_flags & 0x80) ? 1 : 0;
                int ok = j->state == 4 && j->sub_state_1 == st[i].sub && j->motion == st[i].motion &&
                         len == st[i].len && rueck == st[i].rueck;
                if (st[i].sub == 0) {                                    /* Sub 0: Kanal +0x180 = RBJ rec1 */
                    const re15_emd_animation_t *rb = re15_actor_rbj_anim(john);
                    if (!rb || rb->clip_count != K_REC1_CLIPS) ok = 0;
                }
                st[i].gesehen++;
                if (!ok) st[i].abweichung++;
                st[i].ist_len = len; st[i].ist_motion = j->motion; st[i].ist_sub = j->sub_state_1; st[i].ist_rueck = rueck;
            }
        }
        if (j->state == 4 && j->sub_state_1 == 0 && j->motion == RE15_NPK_KANTE_CLIP_HOCH &&
            re15_actor_clip_len(j) == K_LOCO_C2_KLETTERN) kletter++;
        if (g_room_change.pending) tuer = s_bild;
    }
    for (int i = 0; i < nst; i++)
        PRUEF(st[i].gesehen > 0 && st[i].abweichung == 0,
              "%s: Sub %d Clip %d, %d Bilder%s (%d Bilder gemessen, %d abweichend; zuletzt Sub %d Clip %d Laenge %d rueck %d)",
              st[i].was, st[i].sub, st[i].motion, st[i].len, st[i].rueck ? " rueckwaerts" : "", st[i].gesehen,
              st[i].abweichung, st[i].ist_sub, st[i].ist_motion, st[i].ist_len, st[i].ist_rueck);
    PRUEF(kletter == 0, "kein Bild mit Sub-0-Clip 2 an der Kletter-Laenge %d (Bank 0, +0x94 = 2 @0x800526a0): %d",
          K_LOCO_C2_KLETTERN, kletter);
    PRUEF(tuer > 0 && g_room_change.room_id == 0x10B1, "Szene laeuft durch: Aot_on 01 @0x0158E -> ROOM%04X (Bild %d)",
          g_room_change.room_id, tuer);
}

/* ---- leon1170 -------------------------------------------------------------------------------------------------- */
static void teil_leon1170(void)
{
    printf("== leon1170 ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_elzaems_r37_reset();
    g_gameflow.character = 0;
    re15_game_flag_set(3, 0x7D, 1);                          /* sub15 Else-Zweig @0x01882 (wie test_r38 leon1170) */
    static const uint8_t typen[] = { K_KRAEHE_TYP };
    if (room_boot(0x1170, 2300, -7200, 14365, 1024, 0, typen, 1, RE15_EMS_ARCHIV_LEON) != 0) return;
    int gebunden = 0, abgefragt = 0, kraehen = 0;
    for (int f = 0; f < 240; f++) {
        frame();
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *a = &g_actors[s];
            if (!a->active) continue;
            if (f == 0 && a->type == K_KRAEHE_TYP) kraehen++;
            if (a->state == 4 && a->sub_state_1 == 0 && !a->walk_active) {
                abgefragt++;
                if (re15_actor_rbj_anim(s)) gebunden++;
            }
        }
    }
    PRUEF(kraehen == 7, "Leon ROOM1170: %d Kraehen (sub15 @0x01886..0x018FE)", kraehen);
    PRUEF(gebunden == 0, "kein Aktor bindet einen RBJ-Record ueber den Cache (%d Abfragen, %d gebunden) — wie vorher",
          abgefragt, gebunden);
    PRUEF(re15_actor_rbj_anim(1) == NULL, "Slot 1 ohne RBJ-Kanal ueber den Cache (rec1 Marker 0x2 = Elliot, ausgenommen O3)");
}

/* ==== "Auch sonst" (Dossier Abschnitt 6): K2 INIT-Pose, K3 Plc_motion-Saatbild, K4 Schub Phase 5 -> 6 ============ */

/* Szene starten wie teil_szene (Elza, Standard-KI, (3,0xC2)=1 wie ROOM1141 sub02 @0x00C20); Rueckgabe Johns Slot. */
static int szene_starten(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_elzaems_r37_reset();
    g_gameflow.character = 4;
    re15_game_flag_set(3, 0xC2, 1);
    static const uint8_t typen[] = { K_KRAEHE_TYP, K_JOHN_TYP };
    if (room_boot(0x1171, 2300, -7200, 14365, 1024, 0, typen, 2, RE15_EMS_ARCHIV_ELZA) != 0) return -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == K_JOHN_TYP) return s;
    return -1;
}

/* Das GERENDERTE Bild (anim_select_common.c: Slot = anim_frame, rueckwaerts fc-1-anim_frame). */
static int gerendert(const re15_actor_t *a)
{
    int len = re15_actor_clip_len(a);
    if (len < 1) return -1;
    int m = (int)a->anim_frame % len;
    return re15_actor_anim_reverse(a) ? (len - 1 - m) : m;
}

/* initpose (K2): bis zum ersten Plc_motion @0x01416 zeigt John die INIT-Pose = Loco-Paar Clip 2 Bild 0
 * (+0x94 = 2 @0x8011de7c, +0x95 = 0 @0x8011de8c, f314(+0x84,+0x16c) @0x8011debc-c4); die Eskorte bleibt bei Kraehen
 * < 15000 in der DECIDE 0x8004f100 ohne anim_set. Danach (state 4) gilt der Executor-Kanal. */
static void teil_initpose(void)
{
    printf("== initpose ==\n");
    int john = szene_starten();
    PRUEF(john == 1, "John = Aktor %d", john);
    if (john < 0) return;
    re15_actor_t *j = &g_actors[john];
    int vor = 0, falsch = 0, erste4 = -1, nach_loco = -1, f;
    for (f = 0; f < 600 && erste4 < 0; f++) {
        frame();
        if (j->state == 4) { erste4 = f; nach_loco = re15_actor_uses_loco_bank(j); break; }
        int ok = (j->state == 1 && re15_actor_uses_loco_bank(j) && !re15_actor_uses_own_bank(j) &&
                  j->motion == RE15_R39K_INIT_CLIP && re15_actor_clip_len(j) == K_LOCO_C2_KLETTERN &&
                  gerendert(j) == RE15_R39K_INIT_BILD && re15_r39k_init_pose(j));
        vor++;
        if (!ok) {
            if (falsch++ < 3)
                printf("    Bild %d: st %d mo %d af %d loco %d own %d len %d Bild %d\n", f, j->state, j->motion,
                       j->anim_frame, re15_actor_uses_loco_bank(j), re15_actor_uses_own_bank(j), re15_actor_clip_len(j),
                       gerendert(j));
        }
    }
    PRUEF(vor >= 200 && falsch == 0,
          "vor dem ersten Plc_motion %d Bilder INIT-Pose Loco Clip 2 (%d Bilder) Bild 0, eigene Bank nie (%d abweichend)",
          vor, K_LOCO_C2_KLETTERN, falsch);
    PRUEF(erste4 > 0 && nach_loco == 0 && !re15_r39k_init_pose(j) && j->sub_state_1 == 0 &&
          re15_actor_clip_len(j) == K_RBJ_C0,
          "Plc_motion(0,0,0) @0x01416 (Bild %d): Merker geloest, Sub 0 posiert RBJ rec1 Clip 0 (%d Bilder)",
          erste4, re15_actor_clip_len(j));
}

/* eskorte (K2, Gegenprobe): ohne Gegner in 15000 laeuft die Eskorte-EXEC (+0x9 = 0, Tabelle @0x8012181c[0]) und
 * posiert aus der EIGENEN Bank (Stehen @0x8004f384-8c) — der Merker loest sich im ersten Eskorte-Takt. */
static void teil_eskorte(void)
{
    printf("== eskorte ==\n");
    int john = szene_starten();
    if (john < 0) { PRUEF(0, "Szene"); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)                  /* Kraehen weg: 0x800509e4 zaehlt nur aktive (@0x80050a4c) */
        if (g_actors[s].active && g_actors[s].type == K_KRAEHE_TYP) g_actors[s].active = 0;
    re15_actor_t *j = &g_actors[john];
    frame();                                                  /* INIT 0x8011dd98 */
    int init_loco = re15_actor_uses_loco_bank(j), init_bild = gerendert(j);
    PRUEF(j->state == 1 && init_loco && init_bild == 0 && re15_actor_clip_len(j) == K_LOCO_C2_KLETTERN,
          "INIT-Bild: Loco Clip 2 Bild 0 (st %d, loco %d, Bild %d)", j->state, init_loco, init_bild);
    int own = 0, loco = 0;
    for (int f = 1; f < 30; f++) {
        frame();
        if (re15_actor_uses_own_bank(j) && !re15_r39k_init_pose(j)) own++;
        if (re15_actor_uses_loco_bank(j)) loco++;
    }
    PRUEF(j->state == 1 && (j->grid_id & 0x0f) == 0 && own == 29 && loco == 0,
          "ohne Gegner: Eskorte-EXEC posiert ab dem 1. Takt die eigene Bank (%d/29), nie mehr Loco (%d)", own, loco);
}

/* Johns Plc_motion-Fenster (PC auf der Sleep-Zeile hinter dem Befehl; das erste Bild im Fenster ist der Saattakt). */
typedef struct { int lo, hi, rueck, fc; const char *was; int erstes, bilder, abw, halte_bild; } saat_t;

/* saatbild (K3): der Saattakt POSIERT Bild 0 (rueckwaerts fc-1) — Phase 0 @0x80050cec-d0c faellt in f314 @0x80050d4c,
 * f314 posiert +0x95 (@0x8001f35c-36c) und schiebt erst danach vor (@0x8001f618-1c); +0x6 = 2 im Takt, der fc-1
 * posiert (@0x80050da4). Erwartet je Fenster-Bild k: Bild min(k, fc-1) (rueckwaerts gespiegelt), Halten ab k = fc-1. */
static void teil_saatbild(void)
{
    printf("== saatbild ==\n");
    int john = szene_starten();
    if (john < 0) { PRUEF(0, "Szene"); return; }
    saat_t st[] = {
        { 0x0141A, 0x0141E, 0, K_RBJ_C0, "Plc_motion(0,0,0) @0x01416" },
        { 0x01426, 0x0142A, 1, K_RBJ_C0, "Plc_motion(0,0,0) @0x0141E + Plc_flg 0x80" },
        { 0x0142E, 0x01432, 0, K_OWN_C6, "Plc_motion(2,6,0) @0x0142A" },
        { 0x014CA, 0x014CE, 1, K_OWN_C6, "Plc_motion(2,6,0) @0x014C2 + Plc_flg 0x80" },
        { 0x014D2, 0x014D6, 0, K_RBJ_C2, "Plc_motion(0,2,0) @0x014CE Heran-Winken" },
        { 0x0152A, 0x0152E, 0, K_RBJ_C0, "Plc_motion(0,0,0) @0x01526" },
        { 0x01536, 0x0153A, 1, K_RBJ_C0, "Plc_motion(0,0,0) @0x0152E + Plc_flg 0x80" },
    };
    const int n = (int)(sizeof st / sizeof st[0]);
    for (int i = 0; i < n; i++) { st[i].erstes = -1; st[i].bilder = 0; st[i].abw = 0; st[i].halte_bild = -1; }
    re15_actor_t *j = &g_actors[john];
    for (int f = 0; f < 2400 && !g_room_change.pending; f++) {
        frame();
        int pc = -1;
        for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST && pc < 0; s++) {
            int o = stelle_von(s);
            for (int i = 0; i < n && o >= 0; i++) if (o >= st[i].lo && o < st[i].hi) pc = i;
        }
        if (pc < 0) continue;
        saat_t *w = &st[pc];
        int k = w->bilder++;
        int soll = (k < w->fc - 1) ? k : w->fc - 1;
        if (w->rueck) soll = w->fc - 1 - soll;
        int ist = gerendert(j);
        if (k == 0) w->erstes = ist;
        if (ist != soll || re15_actor_clip_len(j) != w->fc) w->abw++;
        if (j->sub_state_2 == 2 && w->halte_bild < 0) w->halte_bild = k;
    }
    for (int i = 0; i < n; i++) {
        int halte_soll = (st[i].bilder >= st[i].fc) ? st[i].fc - 1 : -1;
        PRUEF(st[i].bilder > 0 && st[i].erstes == (st[i].rueck ? st[i].fc - 1 : 0) && st[i].abw == 0 &&
              st[i].halte_bild == halte_soll,
              "%s: Saattakt Bild %d (soll %d), %d Fensterbilder, %d abweichend, Halten ab Fensterbild %d (soll %d)",
              st[i].was, st[i].erstes, st[i].rueck ? st[i].fc - 1 : 0, st[i].bilder, st[i].abw, st[i].halte_bild,
              halte_soll);
    }
}

/* schub (K4): Plc_dest(0,0x13,0x14) @0x01572. Im Bit-Takt der Phase 5 (Bit (5,0x14) von sub03 @0x015EE) posiert John
 * noch Clip 0x12 (anim_set @0x80052af4 vor der Bit-Abfrage @0x80052b44); Clip 0x11 rueckwaerts beginnt im naechsten
 * Takt mit Bild 9 (Phase 6 @0x80052b84-98) und endet nach 10 Takten mit Bild 0 (+0x6 = 7). */
static void teil_schub(void)
{
    printf("== schub ==\n");
    int john = szene_starten();
    if (john < 0) { PRUEF(0, "Szene"); return; }
    re15_actor_t *j = &g_actors[john];
    int vor5 = 0, t = -1, bit_mo = -1, bit_rueck = -1, folge_ok = 1, folge_n = 0, ende7 = -1;
    for (int f = 0; f < 2400 && !g_room_change.pending; f++) {
        frame();
        if (j->state != 4 || j->sub_state_1 != RE15_SZL_MODUS_SCHUB) { if (t >= 0 && ende7 < 0) ende7 = -2; continue; }
        if (t < 0) {
            if (j->sub_state_2 == 5) vor5 = 1;
            else if (vor5 && j->sub_state_2 == 6) { t = f; bit_mo = j->motion; bit_rueck = re15_actor_anim_reverse(j); }
            continue;
        }
        int k = f - t;                                        /* 1..10 = Phase-6-Takte */
        if (k >= 1 && k <= 10) {
            folge_n++;
            if (j->motion != RE15_SZL_CLIP_ANSATZ || !re15_actor_anim_reverse(j) || gerendert(j) != 10 - k) folge_ok = 0;
            if (k == 10 && j->sub_state_2 == 7) ende7 = k;
        }
    }
    PRUEF(t > 0 && bit_mo == RE15_SZL_CLIP_SCHUB && bit_rueck == 0,
          "Bit-Takt (Bild %d): posiert noch Clip 0x%X vorwaerts (Clip %d, rueckw. %d)", t, RE15_SZL_CLIP_SCHUB, bit_mo, bit_rueck);
    PRUEF(folge_n == 10 && folge_ok && ende7 == 10,
          "Phase 6: Clip 0x11 rueckwaerts Bild 9..0 in 10 Takten (%d Takte, Folge %s), +0x6 = 7 im 10. Takt (%d)",
          folge_n, folge_ok ? "ok" : "FALSCH", ende7);
}

/* ==== NACHBESSERUNG 1 (Dossier Abschnitt 12): Elzas Spieler-Pfad (M1/M2/M3a) und die Ereignis-Kraehen (M3b) ========== */

int re15_player_event_reach_clip(void);   /* game_step_common.c: Sub-6-Clip 1/2, -1 = aus bzw. Dispatcher-Bild */
void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);   /* player_common.c: W-Bank-Laengen */

/* Pose-Protokoll je Bild: Elza (Aktor 0), John, die beiden Ereignis-Kraehen. */
#define K_BILDER 1300
typedef struct {
    int16_t mo; int32_t af; uint8_t frac, rev, walk; int8_t erc, ph;         /* Elza */
    int16_t j_mo; int j_bild, j_ss1, j_ss2;                                   /* John */
    int16_t k_mo[2]; int32_t k_af[2]; uint8_t k_st[2], k_ss1[2], k_ss3[2];    /* Kraehen */
} k_zeile_t;
static k_zeile_t s_z[K_BILDER];
static int s_nz = 0;

/* Elzas W-Bank-Laengen wie platform/pc/main.c beim Start (Default-Ausruestung Messer -> Bank W01, "%sW01" mit pl_fam
 * PL04 fuer Elza): EDD aus PLD/PL04W01.PLW (re15_pld_part), Laengen an re15_player_set_aim_clip_lens — Sub 6 spielt
 * Clip 1/2 dieses Paars (+0x170/+0x174 @0x80051884/88). Ohne sie kennt der Unit-Lauf keine Clip-Laenge. */
static uint8_t s_plw[0x80000];
static int elza_wbank_laengen(void)
{
    char pp[600]; snprintf(pp, sizeof pp, "%s/PLD/PL04W01.PLW", RE15_ASSET_PSX_DIR);
    FILE *f = fopen(pp, "rb"); if (!f) return -1;
    size_t n = fread(s_plw, 1, sizeof s_plw, f); fclose(f);
    unsigned long off = 0, len = 0;
    if (!re15_pld_part(s_plw, (long)n, RE15_PLD_EDD, &off, &len) || len == 0) return -1;
    static re15_emd_animation_t an; memset(&an, 0, sizeof an);
    if (re15_emd_parse_animation(s_plw + off, (int)len, &an) != 0 || an.clip_count < 3) return -1;
    uint16_t fcs[16]; int k = an.clip_count < 16 ? an.clip_count : 16;
    for (int i = 0; i < k; i++) fcs[i] = (uint16_t)an.clips[i].frame_count;
    re15_player_set_aim_clip_lens(fcs, k);
    return (int)fcs[1] * 1000 + (int)fcs[2];
}

/* Elzas COMMON-Paar (Spieler +0x84/+0x16c = PLD-Teil EDD/EMR, einzige Schreiber @0x8003154c/@0x80031578; Elza = PL04,
 * wie platform/pc/main.c pc_read_pl_part("PL04")): der Spieler-Schub 0x80052864 spielt dort Clip 0x11/0x12. Registriert
 * wie test_r36_szenenleon.c pl00_laden (re15_player_set_pl00_banks + Kontext, game_step registriert sie jedes Bild). */
static uint8_t s_pld[0x80000];
static re15_emd_animation_t s_pl04_anim;
static re15_emd_skeleton_t  s_pl04_skel;
static int elza_common_laden(void)
{
    char pp[600]; snprintf(pp, sizeof pp, "%s/PLD/PL04.PLD", RE15_ASSET_PSX_DIR);
    FILE *f = fopen(pp, "rb"); if (!f) return -1;
    size_t n = fread(s_pld, 1, sizeof s_pld, f); fclose(f);
    unsigned long eo = 0, el = 0, ro = 0, rl = 0;
    if (!re15_pld_part(s_pld, (long)n, RE15_PLD_EDD, &eo, &el) || !re15_pld_part(s_pld, (long)n, RE15_PLD_EMR, &ro, &rl))
        return -1;
    memset(&s_pl04_anim, 0, sizeof s_pl04_anim); memset(&s_pl04_skel, 0, sizeof s_pl04_skel);
    if (re15_emd_parse_animation(s_pld + eo, (int)el, &s_pl04_anim) != 0 ||
        re15_emd_parse_skeleton(s_pld + ro, (int)rl, &s_pl04_skel) != 0 || s_pl04_anim.clip_count <= RE15_SZL_CLIP_SCHUB)
        return -1;
    re15_player_set_pl00_banks(&s_pl04_skel, &s_pl04_anim);
    s_ctx.pl00_skel = &s_pl04_skel; s_ctx.pl00_anim = &s_pl04_anim;
    return s_pl04_anim.clips[RE15_SZL_CLIP_ANSATZ].frame_count * 1000 + s_pl04_anim.clips[RE15_SZL_CLIP_SCHUB].frame_count;
}

/* Ganze Szene im Lade-Weg-Aufbau (szene_starten) protokollieren, bis ROOM10B1 geladen wird. */
static int szene_protokoll(int *john_out, int kraehe[2])
{
    int john = szene_starten();
    if (john < 0) return -1;
    { int l = elza_common_laden(); printf("  Elza COMMON PL04: Clip 0x11 = %d, Clip 0x12 = %d Bilder\n", l / 1000, l % 1000);
      if (l < 0) return -1; }
    { int l = elza_wbank_laengen(); printf("  Elza W-Bank PL04W01: Clip 1 = %d, Clip 2 = %d Bilder\n", l / 1000, l % 1000);
      if (l < 0) return -1; }
    int nk = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && nk < 2; s++)
        if (g_actors[s].active && g_actors[s].type == K_KRAEHE_TYP) kraehe[nk++] = s;
    if (nk != 2) return -1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *j = &g_actors[john];
    s_nz = 0;
    for (int f = 0; f < K_BILDER && !g_room_change.pending; f++) {
        frame();
        k_zeile_t *z = &s_z[s_nz++];
        z->mo = pl->motion; z->af = pl->anim_frame; z->frac = pl->anim_frac;
        z->rev = (pl->anim_flags & 0x80u) ? 1 : 0; z->walk = pl->walk_active ? 1 : 0;
        z->erc = (int8_t)re15_player_event_reach_clip(); z->ph = (int8_t)re15_szl_schub_spieler_phase();
        z->j_mo = j->motion; z->j_bild = gerendert(j); z->j_ss1 = j->sub_state_1; z->j_ss2 = j->sub_state_2;
        for (int i = 0; i < 2; i++) {
            const re15_actor_t *k = &g_actors[kraehe[i]];
            z->k_mo[i] = k->motion; z->k_af[i] = k->anim_frame; z->k_st[i] = k->state;
            z->k_ss1[i] = k->sub_state_1; z->k_ss3[i] = k->sub_state_3;
        }
    }
    *john_out = john;
    return 0;
}

/* elza (M1/M3a/M2): Sub 6 0x800517f0 — Phase 0 (+0x94 = 1 @0x80051854, +0x95 = 0 @0x80051864, +0x8f = 7 @0x80051874)
 * faellt in f314 @0x8005188c: Clip 1 = Bild 0..15 (16 Takte), Clip 2 ab Bild 0 (@0x800518c8/@0x800518f0). +0x5 = 6 aus
 * dem Dispatcher (Ankunft @0x80030cf0 hinter f314 @0x80030c78, Drehen @0x800313ec / f314 @0x80031494, Schub Phase 7
 * @0x80052be0): Sub 6 laeuft erst im Folgebild — kommt dort per SCD (@0x8001cdec vor @0x8001ce0c) der naechste Plc_dest
 * (@0x80041c14-1c), gibt es KEIN Sub-6-Bild. Schub: Bit-Takt posiert Clip 0x12 (@0x80052af4 vor @0x80052b44), Phase 6
 * Clip 0x11 rueckwaerts 9..0 (@0x80052b98), Phase 7 ohne anim_set (Bild 0 steht), dann Sub 6 Bild 0. */
static void teil_elza(void)
{
    printf("== elza ==\n");
    int john = -1, kraehe[2];
    if (szene_protokoll(&john, kraehe) != 0) { PRUEF(0, "Szene"); return; }
    /* M1: Sub-6-Laeufe */
    int laeufe = 0, start_ok = 0, clip1_16 = 0, clip2_0 = 0, einzel = 0;
    for (int f = 0; f < s_nz; f++) {
        if (s_z[f].erc < 0 || (f > 0 && s_z[f - 1].erc >= 0)) continue;
        laeufe++;
        if (s_z[f].erc == 1 && s_z[f].mo == 1 && s_z[f].af == 0 && s_z[f].frac == 7) start_ok++;
        int n1 = 0, folge = 1;
        while (f + n1 < s_nz && s_z[f + n1].erc == 1) { if (s_z[f + n1].af != n1) folge = 0; n1++; }
        if (n1 == 16 && folge) clip1_16++;
        if (f + n1 < s_nz && s_z[f + n1].erc == 2 && s_z[f + n1].af == 0) clip2_0++;
        if (f + 1 < s_nz && s_z[f + 1].erc < 0) einzel++;
        printf("  Sub-6-Lauf ab Bild %d: Clip 1 %d Bilder (Folge %s), danach Clip %d Bild %d\n", f, n1, folge ? "0.." : "FALSCH",
               f + n1 < s_nz ? s_z[f + n1].erc : -1, f + n1 < s_nz ? (int)s_z[f + n1].af : -1);
    }
    PRUEF(laeufe >= 5 && start_ok == laeufe && clip1_16 == laeufe && clip2_0 == laeufe,
          "M1: jeder Sub-6-Lauf beginnt mit Clip 1 Bild 0 / Blende 7 und zeigt 16 Bilder 0..15, dann Clip 2 Bild 0 "
          "(%d Laeufe: Start %d, 16 Bilder %d, Clip 2 ab 0 %d)", laeufe, start_ok, clip1_16, clip2_0);
    PRUEF(einzel == 0, "M3a: kein einzelnes Sub-6-Bild zwischen zwei Plc_dest (%d)", einzel);
    /* M3a: im Ankunftsbild (Walker endet) steht noch die Gang-/Dreh-Pose (Sentinel 100/105), nie Clip 1 */
    int ank = 0, ank_ok = 0;
    for (int f = 1; f < s_nz; f++) {
        if (!(s_z[f - 1].walk && !s_z[f].walk) || s_z[f].ph >= 0) continue;
        ank++;
        if ((s_z[f].mo == 100 || s_z[f].mo == 105) && s_z[f].erc < 0) ank_ok++;
    }
    PRUEF(ank >= 5 && ank_ok == ank, "M3a: Ankunftsbilder zeigen die Gang-/Dreh-Pose (%d von %d; f314 @0x80030c78/@0x80031494 "
          "vor +0x5 = 6)", ank_ok, ank);
    /* M2: Elzas Schub-Ausgang, Takt fuer Takt — und im selben Bild wie Johns */
    int bit = -1, j_bit = -1;
    for (int f = 1; f < s_nz; f++) {
        if (bit < 0 && s_z[f - 1].ph == 5 && s_z[f].ph == 6) bit = f;
        if (j_bit < 0 && s_z[f - 1].j_ss1 == RE15_SZL_MODUS_SCHUB && s_z[f - 1].j_ss2 == 5 &&
            s_z[f].j_ss1 == RE15_SZL_MODUS_SCHUB && s_z[f].j_ss2 == 6) j_bit = f;
    }
    int ok6 = 1;
    if (bit < 0 || bit + 12 >= s_nz) ok6 = 0;
    else {
        if (s_z[bit].mo != RE15_SZL_CLIP_SCHUB || s_z[bit].rev) ok6 = 0;
        for (int k = 1; k <= 10; k++) {
            int b = s_z[bit + k].rev ? (9 - (int)(s_z[bit + k].af % 10)) : -1;
            if (s_z[bit + k].mo != RE15_SZL_CLIP_ANSATZ || b != 10 - k) ok6 = 0;
        }
        { int b = s_z[bit + 11].rev ? (9 - (int)(s_z[bit + 11].af % 10)) : -1;
          if (s_z[bit + 11].mo != RE15_SZL_CLIP_ANSATZ || b != 0 || s_z[bit + 11].erc >= 0) ok6 = 0; }   /* Phase 7 */
        if (s_z[bit + 12].erc != 1 || s_z[bit + 12].af != 0) ok6 = 0;                                     /* Sub 6 */
    }
    printf("  Schub: Bit-Takt Elza Bild %d, John Bild %d\n", bit, j_bit);
    PRUEF(ok6 && bit == j_bit,
          "M2: Elza Bit-Takt Clip 0x12, dann Clip 0x11 rueckwaerts 9..0, Phase 7 haelt Bild 0, danach Sub 6 Bild 0 — im "
          "selben Bild wie John (Elza %d, John %d, Folge %s)", bit, j_bit, ok6 ? "ok" : "FALSCH");
}

/* kraehen (M3b): gezeichnet wird das zuletzt POSIERTE Bild. ORIENT: Clip 8 Bild 0..7 ab dem Eintritt (0x80115d94 @0x80115310
 * posiert nicht, f314 @0x8011536c posiert +0x95 vor dem Vorschub); Landung (+0x7 = 2) haelt die Fall-Pose (Clip 0x0a
 * @0x801148f4 ohne f314), FINISH posiert 0..11 (f314 @0x8011493c, Timer 11..0), die Leiche (Zustand 7, 0x801157e8 ohne
 * f314) haelt Bild 11. */
static void teil_kraehen(void)
{
    printf("== kraehen ==\n");
    int john = -1, kraehe[2];
    if (szene_protokoll(&john, kraehe) != 0) { PRUEF(0, "Szene"); return; }
    int orient_ok = 0, land_ok = 0, finish_ok = 0, leiche_ok = 0, start_ok = 0;
    for (int i = 0; i < 2; i++) {
        if (s_z[0].k_mo[i] == 0 && s_z[0].k_af[i] == 0) start_ok++;           /* INIT @0x80112300: Clip 0 Bild 0 */
        int o = -1, l = -1, t7 = -1;
        for (int f = 1; f < s_nz; f++) {
            if (o < 0 && s_z[f].k_st[i] == 4 && s_z[f].k_ss1[i] == 2 && s_z[f].k_mo[i] == 8 &&
                !(s_z[f - 1].k_ss1[i] == 2 && s_z[f - 1].k_mo[i] == 8)) o = f;
            if (l < 0 && s_z[f].k_st[i] == 3 && s_z[f].k_ss3[i] == 2 && s_z[f - 1].k_ss3[i] != 2) l = f;
            if (t7 < 0 && s_z[f].k_st[i] == 7) t7 = f;
        }
        int ofolge = (o >= 0 && o + 9 < s_nz);
        for (int k = 0; ofolge && k < 10; k++) if (s_z[o + k].k_mo[i] != 8 || s_z[o + k].k_af[i] != k % 8) ofolge = 0;
        if (ofolge) orient_ok++;
        if (l >= 0 && s_z[l].k_mo[i] != 0x0a) land_ok++;
        int ffolge = (l >= 0 && l + 12 < s_nz);
        for (int k = 1; ffolge && k <= 12; k++) if (s_z[l + k].k_mo[i] != 0x0a || s_z[l + k].k_af[i] != k - 1) ffolge = 0;
        if (ffolge) finish_ok++;
        int lf = (t7 >= 0 && s_nz - t7 >= 60);
        for (int f = t7; lf && f < s_nz; f++) if (s_z[f].k_mo[i] != 0x0a || s_z[f].k_af[i] != 11) lf = 0;
        if (lf) leiche_ok++;
        printf("  Kraehe Slot %d: Start Clip %d Bild %d, ORIENT ab Bild %d (Folge %s), Landung Bild %d (Clip %d), "
               "Zustand 7 ab Bild %d (Clip %d Bild %d)\n", kraehe[i], s_z[0].k_mo[i], (int)s_z[0].k_af[i], o,
               ofolge ? "0..7,0,1" : "FALSCH", l, l >= 0 ? s_z[l].k_mo[i] : -1, t7, t7 >= 0 ? s_z[t7].k_mo[i] : -1,
               t7 >= 0 ? (int)s_z[t7].k_af[i] : -1);
    }
    PRUEF(start_ok == 2, "INIT posiert Clip 0 Bild 0 (@0x80112300), %d von 2", start_ok);
    PRUEF(orient_ok >= 1, "ORIENT zeigt Clip 8 ab Bild 0 (0..7,0,1), %d Kraehe(n)", orient_ok);
    PRUEF(land_ok == 2 && finish_ok == 2, "Tod: Landebild haelt die Fall-Pose (%d), FINISH zeigt Clip 0x0a Bild 0..11 (%d)",
          land_ok, finish_ok);
    PRUEF(leiche_ok == 2, "Leiche haelt Clip 0x0a Bild 11 (%d von 2)", leiche_ok);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "banken")   || !strcmp(teil, "alle")) teil_banken();
    if (!strcmp(teil, "binder")   || !strcmp(teil, "alle")) teil_binder();
    if (!strcmp(teil, "szene")    || !strcmp(teil, "alle")) teil_szene();
    if (!strcmp(teil, "leon1170") || !strcmp(teil, "alle")) teil_leon1170();
    if (!strcmp(teil, "initpose") || !strcmp(teil, "alle")) teil_initpose();
    if (!strcmp(teil, "eskorte")  || !strcmp(teil, "alle")) teil_eskorte();
    if (!strcmp(teil, "saatbild") || !strcmp(teil, "alle")) teil_saatbild();
    if (!strcmp(teil, "schub")    || !strcmp(teil, "alle")) teil_schub();
    if (!strcmp(teil, "elza")     || !strcmp(teil, "alle")) teil_elza();      /* Nachbesserung 1: M1/M2/M3a */
    if (!strcmp(teil, "kraehen")  || !strcmp(teil, "alle")) teil_kraehen();   /* Nachbesserung 1: M3b */
    printf("%s: %s (%d Fehler)\n", "test_r39_heliport1171", g_fail ? "FEHLER" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
