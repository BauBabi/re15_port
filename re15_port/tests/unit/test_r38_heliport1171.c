/* test_r38_heliport1171.c — RIEGEL Runde 38 Spur I "heliport1171": Elza haengt in ihrer Szene ROOM1171.
 *
 * Dossier: analysis/befunde_runde38/I_heliport1171.md. Belege: include/re15_heliport1171_r38.h,
 * engine/src/heliport1171_r38.c. Verfahren wie test_r37_roy.c: ausgelieferte RDT laden, Raum ueber
 * scd_room_reenter aufbauen, Bilder in der Reihenfolge von platform/pc/main.c fahren, MECHANIK messen.
 *
 * Teile (je ein ctest-Eintrag):
 *   banken     EM021-Opferbank (Paar C, dir[5]) aus CDEMD0 (Leon) und CDEMD1 (Elza): Clip-Laengen; die
 *              eingebetteten Ersatzwerte = CDEMD1 (die Szene gibt es nur bei Elza)
 *   zensus     RDT-Bytes: Ereignis-Kraehen NUR ROOM1171 sub07 @0x016DC/@0x016F0, Warten @0x013F4, kein
 *              Skript setzt (5,0x1C)=1 (der einzige Setzer ist die Kraehe @0x801152c4)
 *   flagwort   Kraehen-Flagwort = Spiel-Bank 5: Griff setzt (5,0x1C), (5,0x1D) -> SPIN, (5,0x1F) -> Tod + Loeschen
 *   szene_re2  ROOM1171 mit Elza, Standard-KI (RE2): Griff, Opfer-Clips, Wurf, Modus 6, beide Kraehen tot,
 *              msg 0/1/3, Aot_on 1 -> ROOM10B1; keine HP; Szene laeuft durch
 *   szene_re15 dasselbe mit RE15_AI_FLAVOR=re15 (identisch: dasselbe RE1.5-Hirn)
 *   danach     Wiedereintritt nach der Szene ((3,0xC2)=0, (4,0xE9)=1): 7 normale Kraehen, keine Ereignis-Kraehe,
 *              Abstieg Slot 2 -> ROOM10B1 (@0x0126C) installiert
 *   leon1170   Gegenprobe Leon ROOM1170: 7 Kraehen (sub15 @0x01886..), keine Ereignis-Kraehe, RE2-Hirn unberuehrt */
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
#include "re15_heliport1171_r38.h"
#include "re15_damage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);
int  re15_player_event_reach_clip(void);
int  re15_player_victim_state(void);
extern int g_test_room_se_log[2048];
extern int g_test_room_se_tickof[2048];
extern int g_test_room_se_n;
extern int g_test_room_se_tick;

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

/* Registry-Bank wie pc_enemy_load (RE1.5-Weg): Archiv nach Figur (Elza = CDEMD1, FUN_80022300 @0x800223ac). */
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

/* Ein Bild in der Reihenfolge von platform/pc/main.c (Muster test_r37_roy.c). */
static void frame(void)
{
    const unsigned char *raw; int len, id;
    g_test_room_se_tick = s_bild;
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
    g_test_room_se_n = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static int fc(const re15_emd_animation_t *a, int c) { return (a && c < a->clip_count) ? a->clips[c].frame_count : -1; }

/* Opfer-Clip-Laengen (Paar C) der Kraehe aus dem Archiv. */
static int opfer_laengen(int archiv, int out[3])
{
    size_t n = blob_lesen(archiv, (uint8_t)RE15_R38I_KRAEHE_TYP);
    if (!n) return -1;
    static re15_emd_skeleton_t sk; static re15_emd_animation_t an;
    if (re15_emd_parse_victim_bank(s_blob, n, &sk, &an) != 0) return -1;
    for (int c = 0; c < 3; c++) out[c] = fc(&an, c);
    return an.clip_count;
}

/* ---- banken --------------------------------------------------------------------------------------------- */
static void teil_banken(void)
{
    printf("== banken ==\n");
    int leon[3] = { -1, -1, -1 }, elza[3] = { -1, -1, -1 };
    int nl = opfer_laengen(RE15_EMS_ARCHIV_LEON, leon);
    int ne = opfer_laengen(RE15_EMS_ARCHIV_ELZA, elza);
    PRUEF(nl >= 3, "CDEMD0 EM021 Opferbank (dir[5]): %d Clips, Laengen %d/%d/%d", nl, leon[0], leon[1], leon[2]);
    PRUEF(ne >= 3, "CDEMD1 EM021 Opferbank (dir[5]): %d Clips, Laengen %d/%d/%d", ne, elza[0], elza[1], elza[2]);
    PRUEF(elza[0] == RE15_R38I_LEN_INTRO && elza[1] == RE15_R38I_LEN_HALTEN && elza[2] == RE15_R38I_LEN_WURF,
          "Ersatzwerte im Header = Elzas Archiv CDEMD1 (%d/%d/%d)", RE15_R38I_LEN_INTRO, RE15_R38I_LEN_HALTEN,
          RE15_R38I_LEN_WURF);
}

/* ---- zensus --------------------------------------------------------------------------------------------- */
static int bytes_bei(const uint8_t *d, size_t n, unsigned off, const uint8_t *muster, size_t m)
{
    return off + m <= n && memcmp(d + off, muster, m) == 0;
}
static void teil_zensus(void)
{
    printf("== zensus ==\n");
    if (rdt_laden(0x1171) != 0) { PRUEF(0, "ROOM1171.RDT ladbar"); return; }
    static const uint8_t k1[] = { 0x44, 0x01, 0x21, 0x40 }, k2[] = { 0x44, 0x02, 0x21, 0x40 };
    static const uint8_t w[]  = { 0x0f, 0x04, 0x08, 0x00, 0x21, 0x05, 0x1c, 0x00 };
    static const uint8_t ck[] = { 0x21, 0x03, 0xc2, 0x01 };
    PRUEF(bytes_bei(s_raw, s_rawsz, 0x016DC, k1, 4) && bytes_bei(s_raw, s_rawsz, 0x016F0, k2, 4),
          "ROOM1171 sub07 @0x016DC/@0x016F0 = Sce_em_set Kraehe 0x21 beh 0x40 (Slots 1/2)");
    PRUEF(bytes_bei(s_raw, s_rawsz, 0x013F4, w, 8), "sub02 @0x013F4 `0f 04 08 00 21 05 1c 00` = While Ck(5,0x1C)==0");
    PRUEF(bytes_bei(s_raw, s_rawsz, 0x016C4, ck, 4), "sub07 @0x016C4 Ck(3,0xC2)==1 (gesetzt ROOM1141 sub02 @0x00C20)");
    /* Opcode-genauer Walk ueber ALLE RDTs (Laengen aus derselben Tabelle wie die VM: scd_opcode_size_at):
     * Ereignis-Kraehen-Spawn = Sce_em_set (0x44) Typ 0x21 mit beh & 0x40; Set(5,0x1C)=1 = `22 05 1c 01`. */
    int kraehen40 = 0, kraehen40_1171 = 0, set1c = 0, raeume = 0;
    char p[600];
    for (int st = 1; st <= 7; st++)
        for (int r = 0; r < 0x40; r++)
            for (int v = 0; v < 2; v++) {
                uint16_t id = (uint16_t)((st << 12) | (r << 4) | v);
                snprintf(p, sizeof p, "%s/STAGE%d/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, st, id);
                size_t n = 0; uint8_t *d = slurp(p, &n);
                if (!d) continue;
                static re15_rdt_t rd;
                if (re15_rdt_parse(d, n, &rd) < 0) { free(d); continue; }
                raeume++;
                const uint8_t *anf[RE15_RDT_MAX_SUB_SCD + 1]; int na = 0;
                if (rd.main_scd) anf[na++] = rd.main_scd;
                for (int k = 0; k < rd.sub_scd_count && k < RE15_RDT_MAX_SUB_SCD; k++)
                    if (rd.sub_scd[k]) anf[na++] = rd.sub_scd[k];
                for (int a = 0; a < na; a++) {
                    const uint8_t *ende = d + n;               /* naechster Abschnittsanfang dahinter */
                    for (int b2 = 0; b2 < na; b2++) if (anf[b2] > anf[a] && anf[b2] < ende) ende = anf[b2];
                    const uint8_t *pc = anf[a];
                    while (pc < ende) {
                        int sz = scd_opcode_size_at(pc);
                        if (sz <= 0 || pc + sz > ende) break;  /* desynchronisiert -> kein Befund */
                        if (pc[0] == 0x44 && pc[2] == 0x21 && (pc[3] & 0x40)) {
                            kraehen40++; if (id == 0x1171) kraehen40_1171++;
                        }
                        if (pc[0] == 0x22 && pc[1] == 0x05 && pc[2] == 0x1c && pc[3] == 0x01) set1c++;
                        pc += sz;
                    }
                }
                free(d);
            }
    printf("  (%d RDTs gewalkt)\n", raeume);
    PRUEF(kraehen40 == 2 && kraehen40_1171 == 2, "Ereignis-Kraehen (Sce_em_set Typ 0x21, beh & 0x40) game-weit %d, davon ROOM1171 %d",
          kraehen40, kraehen40_1171);
    PRUEF(set1c == 0, "kein Skript setzt (5,0x1C)=1 (`22 05 1c 01`: %d Treffer) -> Setzer ist die Kraehe @0x801152c4", set1c);
}

/* ---- flagwort ------------------------------------------------------------------------------------------- */
static void teil_flagwort(void)
{
    printf("== flagwort ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_game_state_init();
    g_gameflow.character = 4;
    static const uint8_t typen[] = { 0x21 };
    if (room_boot(0x1171, 1164, -7200, 7435, 1125, 9, typen, 1, RE15_EMS_ARCHIV_ELZA) != 0) return;
    /* eine Ereignis-Kraehe direkt neben Elza in HOVER: Kontakt im naechsten Takt */
    re15_actor_t *k = &g_actors[3];
    memset(k, 0, sizeof *k);
    k->active = 1; k->type = 0x21; k->grid_id = 0x40; k->hp = 0;
    k->x = 1164 + 100; k->y = -7200 - 600; k->z = 7435;
    re15_enemy_apply_hitbox(k, 0x21);
    k->state = 4; k->sub_state_1 = 1; k->sub_state_2 = 1;   /* HOVER 0x80115130 */
    PRUEF(re15_r38i_ereigniskraehe(k), "Ereignis-Kraehe erkannt (Typ 0x21, +0x9 & 0x40)");
    PRUEF(!re15_game_flag_get(5, 0x1C), "(5,0x1C) vor dem Griff 0");
    int griff = -1;
    for (int f = 0; f < 60 && griff < 0; f++) {
        k->x = g_actors[RE15_ACTOR_SLOT_PLAYER].x + 100; k->z = g_actors[RE15_ACTOR_SLOT_PLAYER].z;
        k->y = g_actors[RE15_ACTOR_SLOT_PLAYER].y - 600;
        frame();
        if (k->sub_state_1 == 2) griff = f;
    }
    PRUEF(griff >= 0 && re15_game_flag_get(5, 0x1C) && (g_game.flags[5][0] & 0x8u),
          "HOVER-Kontakt -> ORIENT (Bild %d), Bank-5-Wort 0x%08X: Bit 0x1C (Maske 0x8) = @0x801152c4", griff,
          (unsigned)g_game.flags[5][0]);
    PRUEF(re15_r38i_griffe() >= 1 && re15_r38i_phase() >= 1, "Rueckgriff latcht (aca58=5/aca59=1), Phase %d", re15_r38i_phase());
    PRUEF(g_actors[RE15_ACTOR_SLOT_PLAYER].hp == 100, "kein HP-Abzug beim Rueckgriff (hp %d; @0x80115248-c8 schreibt keine HP)",
          g_actors[RE15_ACTOR_SLOT_PLAYER].hp);
    re15_game_flag_set(5, 0x1D, 1);
    frame();
    PRUEF(k->sub_state_1 == 3, "(5,0x1D) -> FLIGHT-2-Verteiler @0x80114f3c: SPIN (Sub %d)", k->sub_state_1);
    PRUEF(re15_r38i_phase() == 4, "(5,0x1D) -> Wurzel @0x801120e8: Opfer-Phase 3 -> Wurf-Clip, Phase %d", re15_r38i_phase());
    re15_game_flag_set(5, 0x1F, 1);
    frame();
    PRUEF(k->state == 3 && !re15_game_flag_get(5, 0x1F),
          "(5,0x1F) -> Zustand 3 (@0x80112088) und Bit geloescht (@0x80112098): Zustand %d, Bit %d", k->state,
          re15_game_flag_get(5, 0x1F));
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
}

/* ---- szene ---------------------------------------------------------------------------------------------- */
static int stelle_von(int slot)
{
    const scd_thread_t *t = &g_scd.threads[slot];
    return (t->active && t->pc) ? (int)(t->pc - s_rdt.raw) : -1;
}

static void teil_szene(re15_ai_flavor_t flavor, const char *name)
{
    printf("== %s ==\n", name);
    re15_ai_flavor_set(flavor);
    re15_game_state_init();
    re15_elzaems_r37_reset();
    g_gameflow.character = 4;
    re15_game_flag_set(3, 0xC2, 1);                         /* ROOM1141 sub02 @0x00C20 */
    static const uint8_t typen[] = { 0x21, 0x49 };
    if (room_boot(0x1171, 2300, -7200, 14365, 1024, 0, typen, 2, RE15_EMS_ARCHIV_ELZA) != 0) return;
    int el[2] = { -1, -1 }, ne = 0, john = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active) continue;
        if (re15_r38i_ereigniskraehe(&g_actors[s]) && ne < 2) el[ne++] = s;
        if (g_actors[s].type == 0x49) john = s;
    }
    PRUEF(ne == 2 && john > 0, "sub07 Ereigniszweig: 2 Ereignis-Kraehen (Aktoren %d/%d), John Aktor %d", el[0], el[1], john);
    int griff_bild = -1, warten_vorbei = -1, wurf_bild = -1, modus6_bild = -1, tuer_bild = -1;
    int hp_min = 1000, msg_folge[8], nmsg = 0, msg_prev = -1, tot[2] = { -1, -1 }, leiche[2] = { -1, -1 };
    int clip_intro_bilder = 0, clip_halten_erst = -1, se_griff = 0, se_umlauf = 0;
    int bit_vor_griff = 0, max_victim = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 2400 && tuer_bild < 0; f++) {
        int phase_vor = re15_r38i_phase();
        int bit_vor = re15_game_flag_get(5, 0x1C);
        frame();
        int b = s_bild - 1;
        if (pl->hp < hp_min) hp_min = pl->hp;
        if (griff_bild < 0 && re15_r38i_phase() >= 1) { griff_bild = b; bit_vor_griff = bit_vor; }
        if (warten_vorbei < 0)                              /* sub02 hinter der Warteschleife @0x013F4-0x013FE */
            for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {
                int oo = stelle_von(s);
                if (oo >= 0x01400 && oo < 0x01592) { warten_vorbei = b; break; }
            }
        if (re15_player_victim_state() == 4 && pl->motion == RE15_R38I_CLIP_INTRO) clip_intro_bilder++;
        if (clip_halten_erst < 0 && re15_player_victim_state() == 4 && pl->motion == RE15_R38I_CLIP_HALTEN) clip_halten_erst = b;
        if (wurf_bild < 0 && phase_vor >= 0 && phase_vor < 3 && re15_r38i_phase() == 4) wurf_bild = b;
        if (modus6_bild < 0 && wurf_bild >= 0 && re15_r38i_phase() < 0 && re15_player_event_reach_clip() >= 1) modus6_bild = b;
        if (re15_player_victim_state() > max_victim) max_victim = re15_player_victim_state();
        for (int i = 0; i < 2; i++) {
            if (el[i] < 0) continue;
            if (tot[i] < 0 && g_actors[el[i]].state == 3) tot[i] = b;
            if (leiche[i] < 0 && g_actors[el[i]].state == 7) leiche[i] = b;
        }
        if (g_scd.message_active && g_scd.message_id != msg_prev && nmsg < 8) {
            msg_folge[nmsg++] = g_scd.message_id; msg_prev = g_scd.message_id;
        }
        if (!g_scd.message_active) msg_prev = -1;
        if (g_room_change.pending && tuer_bild < 0) tuer_bild = b;
    }
    for (int i = 0; i < g_test_room_se_n; i++) {
        if (g_test_room_se_log[i] != RE15_R38I_SE_SATZ) continue;
        if (g_test_room_se_tickof[i] == griff_bild) se_griff++;
        if (clip_halten_erst >= 0 && g_test_room_se_tickof[i] == clip_halten_erst) se_umlauf++;
    }
    PRUEF(griff_bild > 0 && !bit_vor_griff && re15_game_flag_get(5, 0x1C) == 0,
          "Rueckgriff in Bild %d; (5,0x1C) war davor 0 (vom Skript spaeter @0x014AC geloescht)", griff_bild);
    PRUEF(warten_vorbei == griff_bild + 1, "Warten @0x013F4 endet im Bild nach dem Griff (Bild %d, Griff %d)",
          warten_vorbei, griff_bild);
    PRUEF(clip_intro_bilder == RE15_R38I_LEN_INTRO && clip_halten_erst == griff_bild + RE15_R38I_LEN_INTRO,
          "Opfer-Clip 0 %d Bilder (@0x80115bec), Clip 1 ab Bild %d (@0x80115c54)", clip_intro_bilder, clip_halten_erst);
    PRUEF(se_griff >= 1 && se_umlauf >= 1, "Se_on 0x03020001 = Raum-SE 2 im Griff-Bild (@0x80115c04) und am Clip-0-Ende (@0x80115c3c)");
    PRUEF(wurf_bild == griff_bild + 1 + 60, "Wurf (Phase 3/4, Clip 2) in Bild %d = Griff + 1 + Sleep 60 (@0x01400) -> Set(5,0x1D) @0x01404",
          wurf_bild);
    PRUEF(modus6_bild == wurf_bild + RE15_R38I_LEN_WURF,
          "Kommando 4 / Modus 6 (PLW-Clip 1 -> 2) in Bild %d = Wurf + %d (Clip 2, @0x80115d04-18)", modus6_bild,
          RE15_R38I_LEN_WURF);
    PRUEF(max_victim == 4 && hp_min == 100, "Elza: Opfer-Maschine nur Modus 4 (Pin, Clips der Kraehenbank), HP bleibt 100 (min %d)", hp_min);
    PRUEF(tot[0] > wurf_bild && tot[1] > tot[0] && leiche[0] > tot[0] && leiche[1] > tot[1],
          "beide Ereignis-Kraehen sterben nacheinander ((5,0x1F) @0x01464/@0x01498): Tod %d/%d, Leiche %d/%d",
          tot[0], tot[1], leiche[0], leiche[1]);
    PRUEF(nmsg >= 3 && msg_folge[0] == 0 && msg_folge[1] == 1 && msg_folge[2] == 3,
          "Texte msg %d/%d/%d (0 'Elza, hurry up!', 1 'Help me out!', 3 'let's get down from here!')",
          nmsg > 0 ? msg_folge[0] : -1, nmsg > 1 ? msg_folge[1] : -1, nmsg > 2 ? msg_folge[2] : -1);
    PRUEF(tuer_bild > 0 && g_room_change.room_id == 0x10B1,
          "Aot_on 01 @0x0158E -> Tuer Slot 1 -> ROOM%04X in Bild %d", g_room_change.room_id, tuer_bild);
    PRUEF(re15_game_flag_get(4, 0xE9) && !re15_game_flag_get(3, 0xC2),
          "(4,0xE9)=1 @0x013A0 (Abstieg frei beim naechsten Betreten), (3,0xC2)=0 @0x01398");
    PRUEF(re15_r38i_wuerfe() >= 1 && re15_r38i_phase() < 0, "Rueckgriff beendet (Wuerfe %d)", re15_r38i_wuerfe());
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
}

/* ---- danach --------------------------------------------------------------------------------------------- */
static void teil_danach(void)
{
    printf("== danach ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_elzaems_r37_reset();
    g_gameflow.character = 4;
    re15_game_flag_set(4, 0xE9, 1);                         /* sub02 @0x013A0 */
    static const uint8_t typen[] = { 0x21 };
    if (room_boot(0x1171, 4014, -7200, -7436, 3072, 3, typen, 1, RE15_EMS_ARCHIV_ELZA) != 0) return;  /* 10B1 -> 1171 Cut 3 */
    for (int f = 0; f < 5; f++) frame();
    int n = 0, ereignis = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x21) { n++; if (re15_r38i_ereigniskraehe(&g_actors[s])) ereignis++; }
    PRUEF(n == 7 && ereignis == 0, "sub07 Else-Zweig: %d Kraehen (@0x0170C..0x01784), Ereignis-Kraehen %d", n, ereignis);
    const re15_aot_t *a = &g_aot.slots[2];
    const re15_aot_door_params_t *dp = &g_aot.door_params[2];
    PRUEF(a->active && a->type == RE15_AOT_TYPE_DOOR && dp->dest_room == 0x0B && dp->dest_stage == 0,
          "Abstieg Slot 2 (@0x0126C) installiert: DOOR -> ROOM10B1 (Stage %u Raum 0x%02X)", dp->dest_stage, dp->dest_room);
}

/* ---- leon1170 -------------------------------------------------------------------------------------------- */
static void teil_leon1170(void)
{
    printf("== leon1170 ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_elzaems_r37_reset();
    g_gameflow.character = 0;
    re15_game_flag_set(3, 0x7D, 1);                          /* sub15 Else-Zweig @0x01882 */
    static const uint8_t typen[] = { 0x21 };
    if (room_boot(0x1170, 2300, -7200, 14365, 1024, 0, typen, 1, RE15_EMS_ARCHIV_LEON) != 0) return;
    for (int f = 0; f < 120; f++) frame();
    int n = 0, ereignis = 0, re2zust = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x21) continue;
        n++;
        if (re15_r38i_ereigniskraehe(e)) ereignis++;
        if (e->state != 4) re2zust++;
    }
    PRUEF(n == 7 && ereignis == 0, "Leon ROOM1170: %d Kraehen (sub15 @0x01886..0x018FE), Ereignis-Kraehen %d", n, ereignis);
    PRUEF(re2zust == 7, "keine Kraehe im RE1.5-FLIGHT-2 (Zustand 4): %d/7 nicht in Zustand 4", re2zust);
    PRUEF((g_game.flags[5][0] & 0xFu) == 0, "Bank 5 Bits 0x1C..0x1F unberuehrt (Wort 0x%08X)", (unsigned)g_game.flags[5][0]);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "banken")     || !strcmp(teil, "alle")) teil_banken();
    if (!strcmp(teil, "zensus")     || !strcmp(teil, "alle")) teil_zensus();
    if (!strcmp(teil, "flagwort")   || !strcmp(teil, "alle")) teil_flagwort();
    if (!strcmp(teil, "szene_re2")  || !strcmp(teil, "alle")) teil_szene(RE15_AI_FLAVOR_RE2, "szene_re2");
    if (!strcmp(teil, "szene_re15") || !strcmp(teil, "alle")) teil_szene(RE15_AI_FLAVOR_RE15, "szene_re15");
    if (!strcmp(teil, "danach")     || !strcmp(teil, "alle")) teil_danach();
    if (!strcmp(teil, "leon1170")   || !strcmp(teil, "alle")) teil_leon1170();
    printf("%s: %s (%d Fehler)\n", "test_r38_heliport1171", g_fail ? "FEHLER" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
