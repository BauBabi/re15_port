/* test_r38_werkstatt11b0.c — RIEGEL der Runde 38, Spur C "werkstatt11b0" (Dossier
 * analysis/befunde_runde38/C_werkstatt11b0.md). Gemessen wird die MECHANIK in der echten VM + Spielschritt (Verfahren
 * und Bildfolge wie test_r36_szenenleon.c: scd_vm_tick -> Walker -> Letterbox -> re15_msg_tick -> re15_game_step).
 *
 * Teile (je ein ctest-Eintrag, tests/unit/probes/r38_werkstatt11b0.cmake):
 *   text        Punkt 1: msg 20 = "Marvin: But, hold on..." — Bytes in der Form der Original-Zeilen (Sprecherkopf
 *               ROOM11B0 msg 12 @0x1BD0, Komma+Leerzeichen und Auslassung `57 57 57` wie msg 4 @0x19CD), Programm
 *               unveraendert (+0x0008 Message_on 20), installiert und dekodiert (re15_msg_get_text)
 *   takt        Punkt 2: Raumbank 11B0 Satz 7 leer (@0xE4A0), Quelle ROOM1171 Satz 7 @0x6EF8 `00 00 45 10` -> VAG 4
 *               6688 B == ROOM1090 (Leon allein) Byte fuer Byte; Zustaendigkeit der Ergaenzung (nur 11B0 Satz 7);
 *               Szene: Satz 7 wird im Original-Takt angefordert (Bild 1 jedes Clip-0x12-Umlaufs, 25 Bilder Abstand,
 *               je Schieber, + Phase 7 = 8 Aufrufe)
 *   uebergabe   Punkt 3: Marvins Bank je Bild wie der Renderer (main.c) sie waehlt: im Uebergabebild Phase 7 -> Sub 6
 *               Loco Clip 0x11 Bild 0 = die Pose von Phase 6 (gleicher Keyframe), im Uebergabebild Phase 1 -> 2 Bank 1
 *               Clip 5, NIE ein Rueckfall auf die Containerbank (Clip >= Klipzahl); Gegenprobe: die alte Regel haette
 *               im Uebergabebild Containerbank Clip 0x11 Bild 9 (tiefe Schiebehaltung) gezeigt
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
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_fade.h"
#include "re15_vab.h"
#include "re15_anim_select.h"
#include "re15_skeleton.h"
#include "re15_cut10f0.h"
#include "re15_szenenleon_r36.h"
#include "re15_werkstatt11b0_r38.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);
extern int g_test_snd0_se_count, g_test_snd0_se_last;      /* tests/test_support.c */

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

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

static uint8_t *rdt_datei(uint16_t room, size_t *n)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    return slurp(rp, n);
}

static int rdt_laden(uint16_t room)
{
    free(s_raw); s_raw = rdt_datei(room, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    memset(&s_rdt, 0, sizeof s_rdt);
    return re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0 ? -1 : 0;
}

static int s_bild = 0;
static void frame(void)
{
    const unsigned char *raw; int len, id;
    s_bild++;
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
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_cut10f0_tick();
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
}

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
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0; s_bild = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* ---- Baenke: PL00 (Leon, COMMON) und EM040 (Marvin) — wie test_r36_szenenleon.c ---------------------------- */
static re15_emd_skeleton_t  s_pl00_skel;
static re15_emd_animation_t s_pl00_anim;
static int pl00_laden(void)
{
    static int ok = -1;
    static uint8_t *edd = NULL, *emr = NULL;
    if (ok < 0) {
        size_t esz = 0, rsz = 0;
        edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
        emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
        ok = edd && emr && re15_emd_parse_animation(edd, esz, &s_pl00_anim) == 0 &&
             re15_emd_parse_skeleton(emr, rsz, &s_pl00_skel) == 0;
    }
    if (ok) {
        re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);
        s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    }
    return ok;
}

static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static re15_enemy_bank_t *npc_bank_laden(uint8_t typ)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems_sz);
    if (!s_ems) return NULL;
    int idx = re15_ems_index_for_type(typ);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, s_ems_sz, idx, &off, &len) != 0) return NULL;
    re15_enemy_bank_t *eb = re15_enemy_find(typ);
    if (!eb) eb = re15_enemy_alloc(typ);
    if (!eb) return NULL;
    uint8_t *buf = (uint8_t *)malloc(len);
    memcpy(buf, s_ems + off, len);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(buf, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { free(buf); return NULL; }
    eb->loco_ok = (re15_emd_parse_loco_bank(buf, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->own_ok  = (re15_emd_parse_own_bank(buf, len, &eb->skel_own, &eb->anim_own) == 0);
    eb->type = typ; eb->ok = 1;
    return eb;
}

static void szene_start(void)
{
    re15_game_state_init();
    re15_game_flag_set(3, 0x43, 1); re15_game_flag_set(3, 0x82, 1); re15_game_flag_set(4, 0xF3, 1);
    room_boot(RE15_SZL_RAUM, -26130, -18455, 1024, 0);
    pl00_laden();
}

/* ================================================================================================ */
/*  Punkt 1                                                                                          */
/* ================================================================================================ */
static void teil_text(void)
{
    static const uint8_t k_soll[33] = {
        0x04, 0x00, 0x05, 0x07, 0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x16, 0x05, 0x00, 0x00,   /* Kopf @0x1BD0 */
        0x1e, 0x51, 0x50, 0x18, 0x00, 0x44, 0x4b, 0x48, 0x40, 0x00, 0x4b, 0x4a, 0x57, 0x57, 0x57,   /* B u t , _ h o l d _ o n . . . */
        0x04, 0x01, 0x01, 0x5f };                                                             /* Ende @0x1BFA */
    int n = 0; const uint8_t *m = re15_szl_meldung(RE15_SZL_MSG_HOLD, &n);
    printf("  msg 20 (%d B):", n);
    for (int i = 0; i < n; i++) printf(" %02x", m[i]);
    printf("\n");
    PRUEF(m && n == 33 && memcmp(m, k_soll, 33) == 0, "msg 20 = `04 00 05 07` Marvin `16 05 00 00` B u t , _ h o l d _ o n . . . `04 01 01 5f`");
    size_t rsz = 0; uint8_t *r = rdt_datei(RE15_SZL_RAUM, &rsz);
    PRUEF(r && rsz > 0x1C00 && memcmp(m, r + 0x1BD0, 14) == 0 && memcmp(m + 29, r + 0x1BFA, 4) == 0,
          "Sprecherkopf und Ende = ROOM11B0 msg 12 @0x1BD0 / @0x1BFA");
    /* Form-Beleg Komma + Auslassung: msg 4 @0x19CD "Marvin: Oh, I see... Damn it!" */
    PRUEF(r && memcmp(r + 0x19CD, k_soll, 14) == 0 && r[0x19CD + 14] == 0x2b && r[0x19CD + 15] == 0x44 &&
          r[0x19CD + 16] == 0x18 && r[0x19CD + 17] == 0x00 && r[0x19CD + 23] == 0x57 && r[0x19CD + 24] == 0x57 &&
          r[0x19CD + 25] == 0x57 && r[0x19CD + 26] == 0x00,
          "Form: msg 4 @0x19CD \"Oh,\" = `2b 44 18` + `00`, \"...\" = `57 57 57` (drei Punkte, kein eigenes Zeichen)");
    free(r);
    int pl = 0; const uint8_t *p = re15_szl_programm(&pl);
    PRUEF(p && pl == 294 && p[8] == 0x2B && p[9] == RE15_SZL_MSG_HOLD && p[10] == 0 && p[11] == 0,
          "Programm unveraendert (294 B), +0x0008 Message_on 20");
    /* installiert + dekodiert wie im Spiel (scd_room_setup.c -> re15_szl_install) */
    szene_start();
    re15_szl_install(RE15_SZL_RAUM);
    const char *t = re15_msg_get_text(RE15_SZL_MSG_HOLD);
    printf("  re15_msg_get_text(20) = \"%s\"\n", t ? t : "(null)");
    PRUEF(t && strcmp(t, "Marvin: But, hold on...") == 0, "Text im Spiel: \"Marvin: But, hold on...\"");
}

/* ================================================================================================ */
/*  Punkt 2                                                                                          */
/* ================================================================================================ */
/* snd0-Teil einer RDT: EDT/VH/VB ueber die Kopfworte +0x08/+0x0C/+0x10 (rdt_common.c parse_audio). */
typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; re15_vab_t vab; int vab_ok; } snd0_t;
static void snd0_laden(uint16_t raum, snd0_t *s)
{
    memset(s, 0, sizeof *s);
    s->buf = rdt_datei(raum, &s->n);
    if (!s->buf || re15_rdt_parse(s->buf, s->n, &s->rdt) < 0) return;
    if (s->rdt.snd_vh[0]) s->vab_ok = (re15_vab_parse(s->rdt.snd_vh[0], (size_t)s->rdt.snd_vh_size[0], &s->vab) == 0);
}

static void teil_takt(void)
{
    static snd0_t r11b0, r11b1, r1090, r1171;
    snd0_laden(0x11B0, &r11b0); snd0_laden(0x11B1, &r11b1); snd0_laden(0x1090, &r1090); snd0_laden(0x1171, &r1171);
    const uint8_t *e0 = r11b0.rdt.snd_edt[0];
    PRUEF(e0 && (e0 - r11b0.buf) == 0xE484 && !memcmp(e0 + 28, "\0\0\0\0", 4),
          "ROOM11B0 snd0-EDT @0x%X, Satz 7 @0x%X = 00 00 00 00 (leer: Original stumm)",
          e0 ? (unsigned)(e0 - r11b0.buf) : 0, e0 ? (unsigned)(e0 - r11b0.buf) + 28 : 0);
    const uint8_t *q = r1171.rdt.snd_edt[0];
    static const uint8_t k7[4] = { 0x00, 0x00, 0x45, 0x10 };
    PRUEF(q && (uint32_t)(q - r1171.buf) == RE15_W11B0_QUELLE_EDT_OFF &&
          (uint32_t)(r1171.rdt.snd_vh[0] - r1171.buf) == RE15_W11B0_QUELLE_VH_OFF &&
          (uint32_t)(r1171.rdt.snd_vb[0] - r1171.buf) == RE15_W11B0_QUELLE_VB_OFF && !memcmp(q + 28, k7, 4),
          "Quelle ROOM1171: EDT @0x6EDC / VH @0x6F5C / VB @0x87FC, Satz 7 @0x6EF8 = 00 00 45 10");
    const uint8_t *l = r1090.rdt.snd_edt[0];
    PRUEF(l && (l - r1090.buf) == 0x118D0 && !memcmp(l + 28, k7, 4), "ROOM1090 (Leon allein) Satz 7 @0x118EC = 00 00 45 10");
    /* Ton 4 (Prog 0) und VAG 4 Byte fuer Byte gleich (1171 = 1090) */
    int ton_gleich = r1171.vab_ok && r1090.vab_ok && !memcmp(r1171.rdt.snd_vh[0] + 0x820 + 4 * 32, r1090.rdt.snd_vh[0] + 0x820 + 4 * 32, 32) &&
                     (r1171.rdt.snd_vh[0] - r1171.buf) + 0x820 + 4 * 32 == 0x77FC;
    const re15_vab_sample_t *v71 = r1171.vab_ok ? &r1171.vab.samples[RE15_W11B0_QUELLE_VAG - 1] : NULL;
    const re15_vab_sample_t *v90 = r1090.vab_ok ? &r1090.vab.samples[RE15_W11B0_QUELLE_VAG - 1] : NULL;
    int vag_gleich = v71 && v90 && (int)v71->size == RE15_W11B0_QUELLE_VAG_LEN && v71->size == v90->size &&
                     !memcmp(r1171.rdt.snd_vb[0] + v71->offset, r1090.rdt.snd_vb[0] + v90->offset, v71->size);
    printf("  VAG 4: ROOM1171 VB+0x%X %u B @0x%X, ROOM1090 VB+0x%X %u B\n", v71 ? v71->offset : 0, v71 ? v71->size : 0,
           v71 ? (unsigned)(r1171.rdt.snd_vb[0] - r1171.buf) + v71->offset : 0, v90 ? v90->offset : 0, v90 ? v90->size : 0);
    PRUEF(ton_gleich && vag_gleich, "Ton 4 @0x77FC (32 B) und VAG 4 (6688 B) von ROOM1171 == ROOM1090: derselbe Ton wie Leons Kiste");
    /* Zustaendigkeit */
    PRUEF(re15_w11b0_ergaenzung_noetig(0x11B0, e0, r11b0.rdt.snd_edt_size[0], 7) == 1, "11B0 Satz 7 leer -> Ergaenzung");
    PRUEF(re15_w11b0_ergaenzung_noetig(0x11B0, e0, r11b0.rdt.snd_edt_size[0], 0) == 0, "11B0 Satz 0 (Schritt) -> keine Ergaenzung");
    PRUEF(re15_w11b0_ergaenzung_noetig(0x11B1, r11b1.rdt.snd_edt[0], r11b1.rdt.snd_edt_size[0], 7) == 0,
          "11B1 (Elza, keine Schub-Szene) -> keine Ergaenzung");
    PRUEF(re15_w11b0_ergaenzung_noetig(0x1090, l, r1090.rdt.snd_edt_size[0], 7) == 0 &&
          re15_w11b0_ergaenzung_noetig(0x11B0, l, r1090.rdt.snd_edt_size[0], 7) == 0,
          "eigener Satz vorhanden (1090) -> Raumbank wie bisher");

    /* Takt in der Szene: re15_audio_room_se_snd0(7) (Stub zaehlt) = FUN_80045024(0x02070000) */
    szene_start();
    re15_enemy_bank_t *eb = npc_bank_laden(0x40);
    PRUEF(eb && eb->loco_ok && eb->own_ok, "EM040 (Marvin) geladen: Loco %d Clips, Bank 1 %d Clips",
          eb ? eb->anim_loco.clip_count : -1, eb ? eb->anim_own.clip_count : -1);
    re15_actor_t *mv = &g_actors[RE15_SZL_MARVIN_SLOT];
    int bilder[16], mv_mo[16], mv_af[16], n = 0, vorher = g_test_snd0_se_count, falsch = 0;
    for (int f = 1; f <= 2400 && !g_room_change.pending; f++) {
        int c0 = g_test_snd0_se_count;
        frame();
        for (int k = c0; k < g_test_snd0_se_count && n < 16; k++) {
            if (g_test_snd0_se_last != RE15_W11B0_SCHUB_SATZ) falsch++;
            bilder[n] = f; mv_mo[n] = mv->motion; mv_af[n] = mv->anim_frame; n++;
        }
        if (re15_szl_programm_laeuft()) break;    /* Erweiterung erreicht: Schub vorbei */
    }
    printf("  Satz-7-Aufrufe (%d):", g_test_snd0_se_count - vorher);
    for (int i = 0; i < n; i++) printf(" B%d(Marvin Clip %d Bild %d)", bilder[i], mv_mo[i], mv_af[i]);
    printf("\n");
    PRUEF(n == 8 && falsch == 0, "8 Aufrufe Satz 7: 3 Umlaeufe x 2 Schieber + Phase 7 je Schieber (%d)", n);
    PRUEF(n == 8 && bilder[0] == bilder[1] && bilder[2] == bilder[3] && bilder[4] == bilder[5] &&
          bilder[2] - bilder[0] == 25 && bilder[4] - bilder[2] == 25,
          "Takt: Leon + Marvin im selben Bild, 25 Bilder Abstand (Clip 0x12 = 25 Bilder, +0x95 == 1 @0x80052ac0)");
    PRUEF(n == 8 && mv_mo[0] == RE15_SZL_CLIP_SCHUB && mv_af[0] == RE15_SZL_BILD_SE,
          "erster Ton bei Clip 0x12 Bild 1 (@0x80052ac0-c8)");
    PRUEF(n == 8 && bilder[6] > bilder[4] && bilder[7] >= bilder[6] && bilder[7] - bilder[6] <= 2,
          "Phase 7 (@0x80052bc8): Leon B%d, Marvin B%d", n == 8 ? bilder[6] : -1, n == 8 ? bilder[7] : -1);
}

/* ================================================================================================ */
/*  Punkt 3                                                                                          */
/* ================================================================================================ */
/* Bank-Kette des Renderers fuer einen NPC (platform/pc/main.c, Block "re15_actor_uses_loco_bank" ... "uses_own_bank"):
 * 1 = Loco, 2 = Bank 1, 3 = RBJ/Victim (hier nicht), 0 = Rueckfall Containerbank (eb->anim). *an/*sk = gewaehlte Bank. */
static int render_bank(const re15_actor_t *n, const re15_enemy_bank_t *b,
                       const re15_emd_animation_t **an, const re15_emd_skeleton_t **sk)
{
    int bank = 0;
    *an = &b->anim; *sk = &b->skel;
    if (re15_actor_uses_loco_bank(n) && b->loco_ok && (int)n->motion < b->anim_loco.clip_count) {
        bank = 1; *an = &b->anim_loco; *sk = &b->skel_loco;
    }
    if (n->state == 4 && n->sub_state_1 == 0 && !n->walk_active) return 3;
    if (n->state == 4 && !n->walk_active && (n->sub_state_1 == 1 || n->sub_state_1 == 7 || n->sub_state_1 == 8)) {
        if (b->loco_ok && (int)n->motion < b->anim_loco.clip_count) { *an = &b->anim_loco; *sk = &b->skel_loco; return 1; }
        return bank;
    }
    if (n->state == 4 && !n->walk_active && n->sub_state_1 == 3) return 3;
    if (re15_actor_uses_own_bank(n)) {
        if (b->own_ok && (int)n->motion < b->anim_own.clip_count) { *an = &b->anim_own; *sk = &b->skel_own; return 2; }
        return bank;                                  /* main.c: Bedingung scheitert -> bisherige Bank bleibt */
    }
    return bank;
}

static void teil_uebergabe(void)
{
    szene_start();
    re15_enemy_bank_t *eb = npc_bank_laden(0x40);
    if (!eb || !eb->loco_ok || !eb->own_ok) { PRUEF(0, "EM040 nicht ladbar"); return; }
    re15_actor_t *mv = &g_actors[RE15_SZL_MARVIN_SLOT];
    int war13 = 0, kf_vor = -1, b_vor = -1;
    int f12 = -1, bank12 = -1, clip12 = -1;
    int f7 = -1, bank7 = -1, kf7 = -1, kf6 = -1, mo7 = -1, af7 = -1, rev7 = -1, halte7 = 0, kf_alt = -1;
    int f6 = -1, bank6 = -1, mo6 = -1, af6 = -1, halte_nach = -1;
    int rueckfall = 0, rueckfall_bild = -1, im13 = 0;
    for (int f = 1; f <= 2400 && !g_room_change.pending; f++) {
        frame();
        const re15_emd_animation_t *an; const re15_emd_skeleton_t *sk;
        int bank = render_bank(mv, eb, &an, &sk);
        int kf = (bank == 1 || bank == 2) ? re15_compute_actor_kf(an, sk, mv, (int)mv->motion, mv->anim_frame) : -1;
        int ist13 = (mv->state == 4 && mv->sub_state_1 == RE15_SZL_MODUS_SCHUB);
        if (ist13) im13++;
        if (ist13 && mv->sub_state_2 >= 2 && mv->motion == RE15_SZL_CLIP_DREH && f12 < 0) {
            f12 = f; bank12 = bank; clip12 = mv->motion;
        }
        if (war13 && !ist13 && f7 < 0) {             /* Phase 7 lief in diesem Bild: +0x5 = 6 */
            f7 = f; bank7 = bank; kf7 = kf; kf6 = kf_vor; mo7 = mv->motion; af7 = mv->anim_frame;
            rev7 = (mv->anim_flags & 0x80) ? 1 : 0; halte7 = re15_w11b0_halte_aktiv(RE15_SZL_MARVIN_SLOT);
            (void)b_vor;
            /* Gegenprobe alte Regel: Sub 6 -> Bank 1, Clip 0x11 >= 14 -> Containerbank, Clip 0x11 VORWAERTS Bild 9 */
            re15_actor_t alt = *mv; alt.anim_frame = 9;
            kf_alt = re15_compute_actor_kf(&eb->anim, &eb->skel, &alt, RE15_SZL_CLIP_ANSATZ, 9);
        }
        if (f7 > 0 && f == f7 + 1) { f6 = f; bank6 = bank; mo6 = mv->motion; af6 = mv->anim_frame;
                                     halte_nach = re15_w11b0_halte_aktiv(RE15_SZL_MARVIN_SLOT); }
        if (mv->state == 4 && (bank == 0) && (ist13 || mv->sub_state_1 == 6 || mv->sub_state_1 == 9) && rueckfall_bild < 0) {
            rueckfall++; rueckfall_bild = f;
        }
        war13 = ist13; kf_vor = kf; b_vor = bank;
        if (re15_szl_programm_laeuft()) break;
    }
    printf("  Sub 0x13 %d Bilder | Phase 1->2 B%d Bank %d Clip %d | Phase 7 B%d Bank %d Clip %d Bild %d rueck %d halte %d "
           "kf %d (Phase 6 zuletzt kf %d; alte Regel Containerbank kf %d) | danach B%d Bank %d Clip %d Bild %d | Rueckfall %d (B%d)\n",
           im13, f12, bank12, clip12, f7, bank7, mo7, af7, rev7, halte7, kf7, kf6, kf_alt, f6, bank6, mo6, af6,
           rueckfall, rueckfall_bild);
    PRUEF(im13 > 60 && f7 > 0, "Marvin durchlaeuft Sub 0x13 (%d Bilder) und geht in Sub 6 ueber (B%d)", im13, f7);
    PRUEF(f12 > 0 && bank12 == 2 && clip12 == RE15_SZL_CLIP_DREH,
          "Uebergabebild Phase 1 -> 2: Bank 1 Clip 5 (posiert in Phase 1 @0x8005294c), nicht die Loco-Bank");
    PRUEF(bank7 == 1 && mo7 == RE15_SZL_CLIP_ANSATZ && af7 == 0 && rev7 == 0 && halte7 == 1,
          "Uebergabebild Phase 7 (@0x80052bc0, kein anim_set): Loco Clip 0x11 Bild 0, +0x95 = 0 (@0x8001f63c)");
    PRUEF(kf7 >= 0 && kf7 == kf6, "gehaltene Pose: Keyframe %d == letzter Keyframe von Phase 6 (%d)", kf7, kf6);
    PRUEF(kf_alt >= 0 && kf_alt != kf6,
          "Gegenprobe: die alte Bankregel zeigte Containerbank Clip 0x11 Bild 9 (kf %d) = tiefe Schiebehaltung", kf_alt);
    PRUEF(bank6 == 2 && mo6 == 1 && af6 == 0, "Folgebild: Sub 6 [0]/[1] posiert Bank 1 Clip 1 Bild 0 (@0x80051844/@0x80051878)");
    PRUEF(rueckfall == 0, "kein Bild mit Rueckfall auf die Containerbank (Sub 0x13/6/9)");
    PRUEF(halte_nach == 0, "Halte-Merker im Folgebild verbraucht (Sub 6 hat posiert)");
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "text";
    printf("test_r38_werkstatt11b0 %s\n", teil);
    if      (!strcmp(teil, "text"))      teil_text();
    else if (!strcmp(teil, "takt"))      teil_takt();
    else if (!strcmp(teil, "uebergabe")) teil_uebergabe();
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf("%s: %s (%d Fehler)\n", teil, g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
