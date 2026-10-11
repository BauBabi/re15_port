/* test_r36_szenenleon.c — RIEGEL der Runde 36, Spur J "szenenleon" (Dossier analysis/befunde_runde36/J_szenenleon.md).
 *
 * Verfahren wie test_r35_cut10f0.c: die ausgelieferte RDT wird geladen, der Raum ueber scd_room_reenter aufgebaut
 * (derselbe Einhaengepunkt wie im Spiel), jedes Bild laeuft in der Reihenfolge von platform/pc/main.c
 * (scd_vm_tick -> Walker -> Letterbox -> Szenen-Uebergabe -> re15_msg_tick -> re15_game_step). Gemessen wird die
 * MECHANIK (Werte, Zustaende, Bildfolge).
 *
 * Teile (je ein ctest-Eintrag, tests/unit/probes/r36_szenenleon.cmake):
 *   programm    statisch: Erweiterung/Tuerprogramm schliessen auf Evt_end, Set(9,84) zuerst, Se_on nur Port-Bank 0x3A
 *               Satz 0x0A, Tuer-zu-Opcodes, Schwanz = sub06 @0x017EA `47 05 01 00`, Texte 20/21 + 22 == ROOM11B1 msg 12
 *   schub       Punkt 1: ROOM11B0 sub06 — Plc_dest Modus 0x13 fuer Leon UND Marvin (Sub @0x80052864): Leon PL00 Clip
 *               0x11 -> 0x12 -> 0x11 rueckwaerts, Marvin Sub 0x13 Clip 0x11/0x12 auf dem Loco-Kanal, Container
 *               Objekt-Wort 0x0201 (Bit 0x200 @0x8002bec0) -> wird von Leon um >= 1300 in +z geschoben, am Ende 0x0081,
 *               Schiebe-Ton snd0 Satz 7 (0x02070000 @0x80052acc/@0x80052bcc)
 *   erweiterung Punkt 2: dieselbe Szene weiter: Umleitung @0x017EA, msg 20 vor dem Lauf, Marvin an der Tuer, dort
 *               (Nachbesserung 1) ERST schliessen: Clip 20, Tuerbild 8 -> 2 (Zonen zurueckgetauscht), Tuer-zu-Ton
 *               0x0E/1 — DANN Clip 15 + Se_on 0x3A/0x0A (Bildnummern streng steigend), danach Zelle (3,0x0C) solid +
 *               Text-Platz 8 = msg 22, zurueck, msg 21, dann erst Aot_on 5 (ROOM1260); Kameras 6-10-8-2-4-6
 *   tuer        Punkt 2 danach: ROOM11B0 mit (9,84)=1 -> Tuer zu SCHON NACH DEM INIT-LAUF (Zelle, Text 22 mit Elzas
 *               Bytes, RE2-Ton wie Elza, Zonen/Tuerbild 0/2, Eintritts-Cut 8 -> 2 bzw. 7 -> 0, Ost Bild 2 / West Bild 0,
 *               nie 7/8); ohne (9,84) bleibt die Original-Tuer offen (sub00 @0x0118C, Bild 8); ROOM11B1 unberuehrt
 *   abgang10f0  Punkt 3: ROOM10F0 nach der letzten Zeile kein Cut_chg mehr, Kamera Cut 2 (Leon), Marvin/Ada erst
 *               AUSSERHALB des Bildes geparkt, Tuerton (Bank 0x0E Satz 1) erst danach
 *   irons       Punkt 4: ROOM1150, Irons tot ((9,73)) -> Kopf-FSM +0x1b8 = 0x12, Kopf folgt Leon nicht mehr (zwei
 *               Standpunkte, gleicher Kopf); Gegenprobe lebend: Kopf folgt Leon (Tracking, INIT @0x8011d344)
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
#include "re15_anim_select.h"
#include "re15_skeleton.h"
#include "re15_cut10f0.h"
#include "re15_irons_tod.h"
#include "re15_szenenleon_r36.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void re15_actor_step_all_walkers(void);
extern int g_test_snd0_se_count, g_test_snd0_se_last;      /* tests/test_support.c */
int re15_lock_se_notice(unsigned room_id, uint8_t msg_id, int weg);

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

/* Immer frisch laden: Sca_id_set schreibt in den RDT-Puffer (scd_vm.c sca_entry_at), das Spiel liest jede RDT
 * beim Raumwechsel neu von der Disc. */
static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    memset(&s_rdt, 0, sizeof s_rdt);
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    return 0;
}

/* Se_on-Ereignisse je Bild */
#define SE_MAX 64
static struct { int bild, bank, satz; } s_se[SE_MAX];
static int s_se_n = 0, s_bild = 0;

static void frame(uint16_t held, uint16_t edge)
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
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_cut10f0_tick();
    re15_game_step(&s_ctx);                    /* enthaelt Anim-Vorschub + KI (game_step_common.c Step-Ende) */
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) {
        if (e.kind == SCD_AUDIO_SE_ON && s_se_n < SE_MAX) {
            s_se[s_se_n].bild = s_bild; s_se[s_se_n].bank = e.bank; s_se[s_se_n].satz = e.sample_id; s_se_n++;
        }
    }
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
    s_shown = cut; s_cine_was = 0; s_se_n = 0; s_bild = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* ---- Baenke: PL00 (COMMON, Clips 0x11/0x12) und EM040 (Marvin, Loco-Paar dir[1]/dir[2]) ---------------- */
static re15_emd_skeleton_t  s_pl00_skel;
static re15_emd_animation_t s_pl00_anim;
static uint8_t *s_edd = NULL, *s_emr = NULL;
static int pl00_laden(void)
{
    static int ok = -1;
    if (ok < 0) {
        size_t esz = 0, rsz = 0;
        s_edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
        s_emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
        ok = s_edd && s_emr && re15_emd_parse_animation(s_edd, esz, &s_pl00_anim) == 0 &&
             re15_emd_parse_skeleton(s_emr, rsz, &s_pl00_skel) == 0;
        printf("  PL00: %s, Clip 0x11 %d Bilder, Clip 0x12 %d Bilder\n", ok ? "geladen" : "FEHLT",
               ok && s_pl00_anim.clip_count > 0x12 ? s_pl00_anim.clips[0x11].frame_count : -1,
               ok && s_pl00_anim.clip_count > 0x12 ? s_pl00_anim.clips[0x12].frame_count : -1);
    }
    if (ok) {   /* game_step_common.c:1012 registriert jedes Bild die Baenke aus dem Kontext (main.c) */
        re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);
        s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    }
    return ok;
}

static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static int npc_bank_laden(uint8_t typ)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems_sz);
    if (!s_ems) return 0;
    int idx = re15_ems_index_for_type(typ);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, s_ems_sz, idx, &off, &len) != 0) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(typ);
    if (!eb) eb = re15_enemy_alloc(typ);
    if (!eb) return 0;
    uint8_t *buf = (uint8_t *)malloc(len);
    memcpy(buf, s_ems + off, len);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(buf, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { free(buf); return 0; }
    eb->loco_ok = (re15_emd_parse_loco_bank(buf, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->own_ok  = (re15_emd_parse_own_bank(buf, len, &eb->skel_own, &eb->anim_own) == 0);
    eb->type = typ; eb->ok = 1;
    return eb->loco_ok;
}

/* ---- Opcode-Laengen (scd_vm.c s_opcode_sizes; test_r35_cut10f0.c) ------------------------------------- */
static int op_len(const uint8_t *pc)
{
    static const uint8_t k[0x5F] = {
        1,2,1,4,4,2,4,4, 2,4,3,1,1,6,2,4, 2,4,2,4,6,4,2,6, 2,2,2,6,1,1,1,1,
        1,4,4,6,4,3,6,4, 1,2,1,4,20,34,3,4, 1,1,8,8,4,3,12,4, 12,4,16,32,2,3,6,4,
        8,10,1,4,20,3,10,2, 16,8,2,3,18,10,5,22, 22,4,4,3,6,6,6,4, 4,4,6,4,4,4,4 };
    uint8_t op = pc[0];
    if (op >= 0x5F) return -1;
    if (op == 0x2C) return (pc[3] & 0x80) ? 28 : 20;
    if (op == 0x3B) return (pc[3] & 0x80) ? 40 : 32;
    if (op == 0x50) return (pc[3] & 0x80) ? 30 : 22;
    return (int)k[op];
}

static const re15_sca_entry_t *zelle(int region, int index)
{
    int base = 0;
    for (int i = 0; i < region; i++) base += s_rdt.sca_rgn[i];
    int fi = base + index;
    return (fi >= 0 && fi < s_rdt.sca_count) ? &s_rdt.sca[fi] : NULL;
}

/* ================================================================================================ */
static void teil_programm(void)
{
    int plen = 0; const uint8_t *p = re15_szl_programm(&plen);
    int off = 0, ende = -1, ok = 1, se_n = 0, se_ok = 1, msgs[4], msg_n = 0, sca_zu = 0, aot8 = 0, plc13 = 0;
    int se_tuer = -1, se_schloss = -1;   /* Nachbesserung 1: erst der Tuer-zu-Ton, dann der Abschliess-Ton */
    while (off < plen) {
        int l = op_len(p + off);
        if (l <= 0 || off + l > plen) { ok = 0; break; }
        if (p[off] == 0x36) {
            se_n++;
            if (p[off + 1] == RE15_SZL_SE_TUERBANK && p[off + 2] == RE15_SZL_SE_TUER_ZU && se_tuer < 0) se_tuer = off;
            else if (p[off + 1] == RE15_SZL_SE_BANK && p[off + 2] == RE15_SZL_SE_SCHLOSS && se_schloss < 0) se_schloss = off;
            else se_ok = 0;
        }
        if (p[off] == 0x2B && msg_n < 4) msgs[msg_n++] = p[off + 1];
        if (p[off] == 0x37 && p[off + 1] == 3 && p[off + 2] == 0x0C && p[off + 3] == 0xFF) sca_zu++;
        if (p[off] == 0x2C && p[off + 1] == 8 && p[off + 14] == RE15_SZL_MSG_ZU) aot8++;
        if (p[off] == 0x40 && p[off + 2] == 0x13) plc13++;
        if (p[off] == 0x01) { ende = off; break; }
        off += l;
    }
    printf("  Erweiterung %d B, Ende +%04X\n", plen, ende);
    PRUEF(ok && ende == plen - 2, "Opcode-Laengen schliessen genau auf das Evt_end am Ende");
    PRUEF(plen >= 4 && p[0] == 0x22 && p[1] == RE15_SZL_BANK_ZU && p[2] == RE15_SZL_BIT_ZU && p[3] == 1,
          "ERSTES Opcode = Set(9,84)=1 (Einmal-Riegel, Form ROOM11B0 sub06 @0x01478)");
    PRUEF(se_n == 2 && se_ok && se_tuer >= 0 && se_schloss > se_tuer,
          "zwei Se_on: ERST Tuer-zu-Ton Port-Bank 0x0E Satz 1 (RE2 Door_exit @0x800141f4) +0x%04X, DANN Abschliess-Ton "
          "Port-Bank 0x3A Satz 0x0A (RE2 ROOM1010 sub09 @0x01C20) +0x%04X", se_tuer, se_schloss);
    PRUEF(msg_n == 2 && msgs[0] == RE15_SZL_MSG_HOLD && msgs[1] == RE15_SZL_MSG_FOLLOW, "Message_on 20, dann 21");
    PRUEF(sca_zu == 1 && aot8 == 1, "Tuer zu: Sca_id_set(3,0x0C)=0xff + Aot_set Platz 8 Text 22");
    PRUEF(plc13 == 0, "kein Schub-Modus in der Erweiterung");
    PRUEF(plen >= 4 && p[plen - 4] == 0x47 && p[plen - 3] == 0x05 && p[plen - 2] == 0x01 && p[plen - 1] == 0x00,
          "Schwanz = sub06 @0x017EA `47 05 01 00` (Aot_on 5 = Gully, Evt_end)");
    {
        int si = se_schloss;
        int clip15_davor = si >= 8 && p[si - 8] == 0x3F && p[si - 6] == 0x0F && p[si - 4] == 0x09 && p[si - 2] == 0x14;
        PRUEF(clip15_davor, "Kendo-Form: Plc_motion Clip 15 (= RE2 1010 RBJ Clip 0), Sleep 20, dann Se_on (@0x01C18..@0x01C20)");
    }
    /* Nachbesserung 1 (M2): an der Tuer ERST schliessen — Clip 20, Sleep bis zum Hand-Maximum, Rueck-Tausch der
     * Tuerbilder mit GENAU den Saetzen aus Leons sub00 @0x01184..@0x0118B, Cut_chg 2, Tuer-zu-Ton; Abschliessen danach */
    size_t lsz = 0; char lp[600];
    snprintf(lp, sizeof lp, "%s/STAGE1/ROOM11B0.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *l0 = slurp(lp, &lsz);
    {
        int m[RE15_SZL_M_ANZAHL];
        for (int i = 0; i < RE15_SZL_M_ANZAHL; i++) m[i] = re15_szl_marke(i);
        const uint8_t *s = p + m[RE15_SZL_M_SCHLIESSEN];
        int form = s[0] == 0x3F && s[2] == RE15_SZL_CLIP_SCHLIESS && s[4] == 0x09 && s[6] == 15 &&
                   l0 && memcmp(s + 8, l0 + 0x01184, 8) == 0 && s[16] == 0x29 && s[17] == RE15_SZL_CUT_TUER_ZU &&
                   s + 18 == p + se_tuer && m[RE15_SZL_M_TUER_BILD_ZU] == m[RE15_SZL_M_SCHLIESSEN] + 8 &&
                   m[RE15_SZL_M_TUER_TON] == se_tuer && m[RE15_SZL_M_SCHLOSS_TON] == se_schloss;
        printf("  Schliessen +0x%04X: %02X %02X %02X %02X | %02X %02X %02X %02X | Cut_replace %02X %02X %02X %02X %02X %02X "
               "%02X %02X | %02X %02X | Se_on +0x%04X\n", m[RE15_SZL_M_SCHLIESSEN], s[0], s[1], s[2], s[3], s[4], s[5],
               s[6], s[7], s[8], s[9], s[10], s[11], s[12], s[13], s[14], s[15], s[16], s[17], se_tuer);
        PRUEF(form, "Schliessen: Plc_motion(0,20), Sleep 15, Cut_replace == ROOM11B0 sub00 @0x01184..@0x0118B (Rueck-"
                    "Tausch, LAB_80040414), Cut_chg 2, Tuer-zu-Ton");
        PRUEF(m[RE15_SZL_M_SCHLIESSEN] < m[RE15_SZL_M_TUER_TON] && m[RE15_SZL_M_TUER_TON] < m[RE15_SZL_M_ABSCHLIESSEN] &&
              m[RE15_SZL_M_ABSCHLIESSEN] < m[RE15_SZL_M_SCHLOSS_TON] && m[RE15_SZL_M_SCHLOSS_TON] < m[RE15_SZL_M_TUER_ZU],
              "Programmfolge Schliessen < Tuer-zu-Ton < Abschliessen < Abschliess-Ton < Zelle zu");
    }
    int tl = 0; const uint8_t *t = re15_szl_tuerprogramm(&tl);
    PRUEF(tl == 80 && t[0] == 0x37 && t[1] == 3 && t[2] == 0x0C && t[3] == 0xFF && t[4] == 0x39 && t[7] == 0x03 &&
          t[8] == 0x2C && t[9] == 8 && t[22] == RE15_SZL_MSG_ZU && t[78] == 0x01,
          "Tuerprogramm: Zelle (3,0x0C) u0=0xff fl=0x03 (RDT @0x00EC8), Text-Platz 8 msg 22, ..., Evt_end (%d B)", tl);
    /* Nachbesserung 1 (M1): Strom-Zweig = Kopf von sub00 @0x0116C/@0x01170 (Laenge eigen), dieselben Cut_replace, dann
     * die Ansicht 7 -> 0 und 8 -> 2 in der Form von sub00 @0x01174..@0x01183 (nur Vergleichswert/Ziel getauscht) */
    {
        static const uint8_t k_ansicht_70[16] = { 0x06,0x00,0x0c,0x00, 0x23,0x00,0x0a,0x00,0x07,0x00, 0x29,0x00, 0x3c,0x01, 0x08,0x00 };
        static const uint8_t k_ansicht_82[16] = { 0x06,0x00,0x0c,0x00, 0x23,0x00,0x0a,0x00,0x08,0x00, 0x29,0x02, 0x3c,0x01, 0x08,0x00 };
        int kopf = l0 && t[28] == 0x06 && t[29] == 0x00 && memcmp(t + 32, l0 + 0x01170, 4) == 0;
        int tausch = l0 && memcmp(t + 36, l0 + 0x01184, 8) == 0;
        int form_sub00 = l0 && l0[0x01174] == 0x06 && l0[0x01178] == 0x23 && l0[0x0117A] == 0x0A && l0[0x0117E] == 0x29 &&
                         l0[0x01180] == 0x3C && l0[0x01182] == 0x08;
        int ansicht = memcmp(t + 44, k_ansicht_70, 16) == 0 && memcmp(t + 60, k_ansicht_82, 16) == 0 && t[76] == 0x08;
        PRUEF(kopf && tausch && form_sub00 && ansicht,
              "Ereignis 23: Ifel Ck(4,6)==1 (sub00 @0x01170) { Cut_replace == sub00 @0x01184..@0x0118B; Cut 7 -> 0; "
              "Cut 8 -> 2 (Form sub00 @0x01174..@0x01183) }");
    }
    free(l0);
    {
        int n = 0; const uint8_t *b = re15_szl_tuerton(&n);
        PRUEF(b && n == 0x5338 && b[2] == 0x14 && b[3] == 0x16 && b[6] == 0x24 && b[7] == 0x17 && !memcmp(b + 0x10, "pBAV", 4),
              "Tuer-zu-Ton: Tonteil DOOR1A 0x%X B (RE2 @0x8009A658), Tonkopf Satz 0/1 `14 16`/`24 17`, VH pBAV", n);
    }
    /* Text 22 = Elzas Nachricht 12 Byte fuer Byte */
    size_t esz = 0; char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11B1.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *e = slurp(rp, &esz);
    int n22 = 0; const uint8_t *m22 = re15_szl_meldung(RE15_SZL_MSG_ZU, &n22);
    PRUEF(e && n22 == 23 && memcmp(m22, e + 0x179F, 23) == 0, "msg 22 == ROOM11B1 msg 12 @0x179F (23 B)");
    PRUEF(e && memcmp(e + 0x112A, t + 8, 14) == 0 && e[0x112A + 14] == 0x0C,
          "Text-Platz = ROOM11B1 sub00 @0x0112A bis auf die Nachricht (12 -> 22)");
    free(e);
}

typedef struct {
    int schub_leon_11, schub_leon_12, schub_leon_rueck, schub_leon_ende;
    int schub_mv_11, schub_mv_12, schub_mv_ende;
    int32_t box_z0, box_z1, leon_z0, leon_z1; int box_flags_schub, box_flags_ende;
    int umleitung, msg20, msg21, aot_on_bild, tuer_bild, se_bild, se_bank, se_satz, mv_tuer_dist, mv_motion_se;
    int zelle_zu_bild, aot8_bild, mv_heim_x, mv_heim_z, msg21_mv_x, msg21_mv_z, raumwechsel;
    int cuts[24], n_cuts, snd0_vorher, snd0_nachher;
    /* Nachbesserung 1: Schliessen vor dem Abschliessen + Tuerbild */
    int clip20_bild, tuerton_bild, cut2_bild, clip15_bild, zonen_offen_an_tuer, zonen_zu_nach, zonen_zu_ende;
} lauf11b0_t;

/* ---- Zonen-Zustand (Nachbesserung 1, M1/M4): RVD der ausgelieferten RDT (= Tuerbilder 0/2, geschlossen) gegen den
 * laufenden Stand. Cut_replace LAB_80040414 tauscht cam_from/cam_to (@0x80040458..@0x80040494). ----------------- */
static uint8_t s_zon_from[RE15_RDT_MAX_ZONES], s_zon_to[RE15_RDT_MAX_ZONES];
static int     s_zon_n = -1;
static int zonen_original_laden(void)
{
    if (s_zon_n >= 0) return s_zon_n;
    char rp[600]; size_t n = 0;
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11B0.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *b = slurp(rp, &n);
    static re15_rdt_t r;
    memset(&r, 0, sizeof r);
    if (!b || re15_rdt_parse(b, n, &r) < 0) { free(b); return s_zon_n = 0; }
    for (int i = 0; i < r.zone_count; i++) { s_zon_from[i] = r.zones[i].cam_from; s_zon_to[i] = r.zones[i].cam_to; }
    s_zon_n = r.zone_count;
    return s_zon_n;   /* b bleibt: re15_rdt_t zeigt hinein */
}
static uint8_t tausch_0782(uint8_t c) { return c == 0 ? 7 : c == 7 ? 0 : c == 2 ? 8 : c == 8 ? 2 : c; }
/* 1 = Zonen wie ausgeliefert (Tuer zu: Bild 0/2), 2 = um 0<->7 + 2<->8 getauscht (Tuer offen, sub00 @0x01184/@0x01188),
 * 0 = weder noch */
static int zonen_zustand(void)
{
    int n = zonen_original_laden();
    if (n <= 0 || n != s_rdt.zone_count) return 0;
    int gleich = 1, getauscht = 1;
    for (int i = 0; i < n; i++) {
        if (s_rdt.zones[i].cam_from != s_zon_from[i] || s_rdt.zones[i].cam_to != s_zon_to[i]) gleich = 0;
        if (s_rdt.zones[i].cam_from != tausch_0782(s_zon_from[i]) || s_rdt.zones[i].cam_to != tausch_0782(s_zon_to[i])) getauscht = 0;
    }
    return gleich ? 1 : getauscht ? 2 : 0;
}

#define LAUF_MAX 4200
static lauf11b0_t lauf_11b0(void)
{
    lauf11b0_t r; memset(&r, 0, sizeof r);
    r.msg20 = r.msg21 = r.aot_on_bild = r.tuer_bild = r.se_bild = r.zelle_zu_bild = r.aot8_bild = -1;
    r.schub_leon_ende = r.schub_mv_ende = -1; r.mv_tuer_dist = 1 << 30;
    r.clip20_bild = r.tuerton_bild = r.cut2_bild = r.clip15_bild = -1;
    re15_game_state_init();
    re15_game_flag_set(3, 0x43, 1); re15_game_flag_set(3, 0x82, 1); re15_game_flag_set(4, 0xF3, 1);
    if (room_boot(RE15_SZL_RAUM, -26130, -18455, 1024, 0) != 0) return r;
    pl00_laden(); npc_bank_laden(0x40);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *mv = &g_actors[RE15_SZL_MARVIN_SLOT];
    int letzter_cut = -1, schub_vorher = -1;
    r.snd0_vorher = g_test_snd0_se_count;
    for (int f = 1; f <= LAUF_MAX; f++) {
        frame(0, 0);
        if (s_shown != letzter_cut) { if (r.n_cuts < 24) r.cuts[r.n_cuts] = s_shown; r.n_cuts++; letzter_cut = s_shown; }
        int ph = re15_szl_schub_spieler_phase();
        if (getenv("R36J_DBG") && (ph >= 0 || schub_vorher >= 0))
            printf("    B%d ph=%d mo=%d af=%d fl=0x%x st=%d pm=%d z=%ld box=%ld\n", f, ph, pl->motion, pl->anim_frame,
                   pl->anim_flags, pl->state, g_scd.player_mode, (long)pl->z, (long)g_scd.props[0].z);
        if (ph >= 0 && schub_vorher < 0) { r.box_z0 = g_scd.props[0].z; r.leon_z0 = pl->z; }
        if (ph >= 0) r.box_flags_schub = g_scd.props[0].flags;
        if (ph == 3 && pl->motion == 0x11 && re15_szl_schub_spieler_pl00() == 0x11) r.schub_leon_11 = 1;
        if (ph == 5 && pl->motion == 0x12 && re15_szl_schub_spieler_pl00() == 0x12) r.schub_leon_12 = 1;
        if (ph == 6 && pl->motion == 0x11 && (pl->anim_flags & 0x80)) r.schub_leon_rueck = 1;
        if (ph < 0 && schub_vorher >= 0 && r.schub_leon_ende < 0) { r.schub_leon_ende = f; r.leon_z1 = pl->z; r.box_z1 = g_scd.props[0].z; }
        schub_vorher = ph;
        if (mv->state == 4 && mv->sub_state_1 == RE15_SZL_MODUS_SCHUB) {
            if (mv->sub_state_2 == 3 && mv->motion == 0x11 && re15_szl_schub_npc_kanal(mv) == 1) r.schub_mv_11 = 1;
            if (mv->sub_state_2 == 5 && mv->motion == 0x12 && re15_szl_schub_npc_kanal(mv) == 1) r.schub_mv_12 = 1;
        } else if (r.schub_mv_12 && r.schub_mv_ende < 0) r.schub_mv_ende = f;
        if (!r.umleitung && re15_szl_programm_laeuft()) r.umleitung = f;
        if (g_scd.message_active) {
            if ((int)g_scd.message_id == RE15_SZL_MSG_HOLD && r.msg20 < 0) r.msg20 = f;
            if ((int)g_scd.message_id == RE15_SZL_MSG_FOLLOW && r.msg21 < 0) { r.msg21 = f; r.msg21_mv_x = mv->x; r.msg21_mv_z = mv->z; }
        }
        if (r.umleitung) {
            int32_t dx = mv->x - (-20860), dz = mv->z - (-16230);
            int d = (int)sqrt((double)dx * dx + (double)dz * dz);
            if (d < r.mv_tuer_dist) r.mv_tuer_dist = d;
            if (d < 300 && r.tuer_bild < 0) r.tuer_bild = f;
        }
        for (int i = 0; i < s_se_n; i++) {
            if (s_se[i].bank == RE15_SZL_SE_BANK && r.se_bild < 0) {
                r.se_bild = s_se[i].bild; r.se_bank = s_se[i].bank; r.se_satz = s_se[i].satz; r.mv_motion_se = mv->motion;
            }
            if (s_se[i].bank == RE15_SZL_SE_TUERBANK && s_se[i].satz == RE15_SZL_SE_TUER_ZU && r.umleitung &&
                r.tuerton_bild < 0) r.tuerton_bild = s_se[i].bild;
        }
        /* Nachbesserung 1: Folge an der Tuer + Tuerbild (gezeigter Cut, Zonen) */
        if (r.umleitung && r.tuer_bild > 0) {
            if (mv->motion == RE15_SZL_CLIP_SCHLIESS && r.clip20_bild < 0) {
                r.clip20_bild = f;
                r.zonen_offen_an_tuer = (zonen_zustand() == 2 && s_shown == RE15_SZL_CUT_TUER_OFFEN);
            }
            if (r.clip20_bild > 0 && s_shown == RE15_SZL_CUT_TUER_ZU && r.cut2_bild < 0) {
                r.cut2_bild = f; r.zonen_zu_nach = (zonen_zustand() == 1);
            }
            if (r.clip20_bild > 0 && mv->motion == 15 && r.clip15_bild < 0) r.clip15_bild = f;
        }
        const re15_sca_entry_t *z = zelle(3, 0x0C);
        if (r.umleitung && z && z->u0 == 0xFF && r.zelle_zu_bild < 0) r.zelle_zu_bild = f;
        if (r.umleitung && g_aot.slots[8].active && g_aot.slots[8].type == RE15_AOT_TYPE_MESSAGE &&
            g_aot.slots[8].event_id == RE15_SZL_MSG_ZU && r.aot8_bild < 0) r.aot8_bild = f;
        if (g_room_change.pending && r.aot_on_bild < 0) {
            r.aot_on_bild = f; r.raumwechsel = 1; r.mv_heim_x = mv->x; r.mv_heim_z = mv->z;
            r.box_flags_ende = g_scd.props[0].flags;
            r.zonen_zu_ende = (zonen_zustand() == 1);
            break;
        }
    }
    r.snd0_nachher = g_test_snd0_se_count;
    return r;
}

static void teil_schub(void)
{
    lauf11b0_t r = lauf_11b0();
    printf("  Leon 0x11=%d 0x12=%d rueck=%d Ende B%d z %d -> %d | Marvin 0x11=%d 0x12=%d Ende B%d | Container z %d -> %d "
           "Flags waehrend 0x%04X Ende 0x%04X | snd0 %d -> %d\n",
           r.schub_leon_11, r.schub_leon_12, r.schub_leon_rueck, r.schub_leon_ende, r.leon_z0, r.leon_z1,
           r.schub_mv_11, r.schub_mv_12, r.schub_mv_ende, r.box_z0, r.box_z1, r.box_flags_schub, r.box_flags_ende,
           r.snd0_vorher, r.snd0_nachher);
    PRUEF(r.schub_leon_11 && r.schub_leon_12 && r.schub_leon_rueck,
          "Leon: Sub 0x13 spielt PL00 Clip 0x11 (@0x800529cc), 0x12 (@0x80052a8c), 0x11 rueckwaerts (@0x80052b98)");
    PRUEF(r.schub_leon_ende > 0, "Leon: Sub 0x13 endet (Bank-5-Bit 20 @0x80052b44 -> Phase 7 -> Sub 6) bei B%d", r.schub_leon_ende);
    PRUEF(r.schub_mv_11 && r.schub_mv_12, "Marvin: Sub 0x13 (0x80076ca0[19]) Clip 0x11/0x12 auf dem Loco-Kanal +0x84/+0x16c");
    PRUEF(r.schub_mv_ende > 0, "Marvin: Sub 0x13 endet (-> Sub 6 @0x80052be0) bei B%d", r.schub_mv_ende);
    PRUEF(r.box_flags_schub == 0x0201, "Container-Objektwort waehrend des Schubs 0x0201 (Member_set 6 @0x01706, Bit 0x200)");
    PRUEF(r.box_z1 - r.box_z0 >= 1300 && r.box_z1 - r.box_z0 <= 1700,
          "Container von Leon in +z geschoben: %d -> %d (+%d; Raum-Records +1550 @0x010C4/@0x010EA)",
          r.box_z0, r.box_z1, r.box_z1 - r.box_z0);
    PRUEF(r.leon_z1 - r.leon_z0 >= 1300, "Leon schiebt mit: z %d -> %d", r.leon_z0, r.leon_z1);
    PRUEF(r.box_flags_ende == 0x0081, "am Szenenende Objektwort 0x0081 (Member_set 6 @0x017D6), ist 0x%04X", r.box_flags_ende);
    PRUEF(r.snd0_nachher - r.snd0_vorher >= 3,
          "Schiebe-Ton snd0 Satz 7: Bild 1 von Clip 0x12 + Phase 7, je Schieber (%d Aufrufe)", r.snd0_nachher - r.snd0_vorher);
}

static void teil_erweiterung(void)
{
    lauf11b0_t r = lauf_11b0();
    printf("  Umleitung B%d msg20 B%d Tuer B%d (min %d) Se_on B%d bank 0x%02X satz %d (Marvin Clip %d) Zelle zu B%d "
           "Platz 8 B%d msg21 B%d (Marvin %d,%d) Aot_on B%d | Cuts:", r.umleitung, r.msg20, r.tuer_bild, r.mv_tuer_dist,
           r.se_bild, r.se_bank, r.se_satz, r.mv_motion_se, r.zelle_zu_bild, r.aot8_bild, r.msg21, r.msg21_mv_x,
           r.msg21_mv_z, r.aot_on_bild);
    for (int i = 0; i < r.n_cuts && i < 24; i++) printf(" %d", r.cuts[i]);
    printf("\n");
    PRUEF(r.umleitung > 0 && r.schub_leon_ende > 0 && r.umleitung > r.schub_leon_ende,
          "Erweiterung beginnt NACH dem Schieben (B%d) an sub06 @0x017EA (B%d)", r.schub_leon_ende, r.umleitung);
    PRUEF(re15_game_flag_get(RE15_SZL_BANK_ZU, RE15_SZL_BIT_ZU) == 1, "(9,84)=1");
    PRUEF(r.msg20 >= r.umleitung && r.msg20 < r.tuer_bild, "\"Hold on.\" (msg 20) vor dem Lauf zur Tuer");
    PRUEF(r.tuer_bild > 0 && r.mv_tuer_dist < 300, "Marvin erreicht die Innenseite der Werkstatt-Tuer (Abstand %d < 300)",
          r.mv_tuer_dist);
    PRUEF(r.se_bild >= r.tuer_bild && r.se_bank == RE15_SZL_SE_BANK && r.se_satz == RE15_SZL_SE_SCHLOSS &&
          r.mv_motion_se == 15, "an der Tuer: Clip 15 (Kendo Clip 0) und Zuschliess-Ton Port-Bank 0x3A Satz 0x0A");
    /* Nachbesserung 1 (M2, Nachtrag abends): Bildnummern Schliessen < Tuer-zu-Ton < Abschliessen < Abschliess-Ton */
    printf("  Folge an der Tuer: Schliessen (Clip 20) B%d < Tuer-zu-Ton (0x0E/1) B%d, Bild 2 gezeigt B%d < Abschliessen "
           "(Clip 15) B%d < Abschliess-Ton (0x3A/0x0A) B%d | Zonen: an der Tuer offen %d, nach dem Schliessen zu %d, "
           "am Ende zu %d\n", r.clip20_bild, r.tuerton_bild, r.cut2_bild, r.clip15_bild, r.se_bild,
           r.zonen_offen_an_tuer, r.zonen_zu_nach, r.zonen_zu_ende);
    PRUEF(r.tuer_bild > 0 && r.clip20_bild >= r.tuer_bild && r.tuerton_bild > r.clip20_bild &&
          r.clip15_bild > r.tuerton_bild && r.se_bild > r.clip15_bild,
          "Schliessen B%d < Tuer-zu-Ton B%d < Abschliessen B%d < Abschliess-Ton B%d", r.clip20_bild, r.tuerton_bild,
          r.clip15_bild, r.se_bild);
    PRUEF(r.zonen_offen_an_tuer, "an der Tuer vor dem Schliessen: Bild 8 gezeigt, Zonen getauscht (Tuer offen, sub00 "
                                 "@0x01184/@0x01188)");
    PRUEF(r.cut2_bild > r.clip20_bild && r.cut2_bild <= r.tuerton_bild + 1 && r.cut2_bild < r.clip15_bild && r.zonen_zu_nach,
          "Tuer geht im Bild zu: Bild 2 (geschlossen gemalt) ab B%d, im Bild des Tuer-zu-Tons; Zonen zurueck wie ausgeliefert",
          r.cut2_bild);
    PRUEF(r.zelle_zu_bild > r.se_bild && r.aot8_bild > r.se_bild,
          "danach Tuer zu: Zelle (3,0x0C) u0=0xff (B%d), Text-Platz 8 = msg 22 (B%d)", r.zelle_zu_bild, r.aot8_bild);
    PRUEF(r.msg21 > r.aot8_bild && abs(r.msg21_mv_x - 1100) < 400 && abs(r.msg21_mv_z - (-19999)) < 400,
          "Marvin zurueck bei Leon/Ada (%d,%d), dann \"I just want to make sure ...\" (msg 21)", r.msg21_mv_x, r.msg21_mv_z);
    PRUEF(r.raumwechsel && r.aot_on_bild > r.msg21 && g_room_change.room_id == 0x1260,
          "erst danach Aot_on 5 -> Raumwechsel ROOM1260 (Ziel ROOM%04X)", (unsigned)g_room_change.room_id);
    PRUEF(r.zonen_zu_ende, "am Szenenende bleiben die Zonen auf Bild 0/2 (Tuer zu)");
    {
        int i6 = -1, i10 = -1, i8 = -1, i2 = -1, j4 = -1, j6 = -1;
        for (int i = 0; i < r.n_cuts && i < 24; i++) {
            if (r.cuts[i] == 6 && i10 < 0) i6 = i;
            if (r.cuts[i] == 10 && i6 >= 0 && i10 < 0) i10 = i;
            if (r.cuts[i] == 8 && i10 >= 0 && i8 < 0) i8 = i;
            if (r.cuts[i] == 2 && i8 >= 0 && i2 < 0) i2 = i;
            if (r.cuts[i] == 4 && i2 >= 0 && j4 < 0) j4 = i;
            if (r.cuts[i] == 6 && j4 >= 0 && j6 < 0) j6 = i;
        }
        PRUEF(i6 >= 0 && i10 > i6 && i8 > i10 && i2 == i8 + 1 && j4 == i2 + 1 && j6 > j4,
              "Kameras 6 -> 10 -> 8 (Tuer offen) -> 2 (Tuer zu, gleiche Kamera) -> 4 (Rueckweg, Tuer zu) -> 6");
    }
}

static void teil_tuer(void)
{
    /* mit (9,84): Tuer zu — Nachbesserung 1: schon NACH DEM INIT-LAUF (Ereignis 23 laeuft hinter sub00), vor dem
     * ersten Bild; Tuerbilder zurueck auf 0/2 (M1/M4) */
    re15_game_state_init();
    re15_game_flag_set(4, 0xF3, 1); re15_game_flag_set(3, 0x82, 1); re15_game_flag_set(3, 0x83, 1);
    re15_game_flag_set(4, 0xCD, 1); re15_game_flag_set(RE15_SZL_BANK_ZU, RE15_SZL_BIT_ZU, 1);
    if (room_boot(RE15_SZL_RAUM, -20900, -16230, 2048, 8) != 0) return;
    const re15_sca_entry_t *z = zelle(3, 0x0C);
    int u0_init = z ? z->u0 : -1, fl_init = z ? z->floor : -1;
    int zon_init = zonen_zustand(), cam_init = (int)g_scd.cam_id, wv_init = (int)g_scd.work_vars[0x0A];
    for (int f = 0; f < 5; f++) frame(0, 0);
    z = zelle(3, 0x0C);
    printf("  (9,84)=1, Eintritt Cut 8: nach dem Init-Lauf Zelle u0 0x%02X fl 0x%02X, Zonen %d, Cut %d (work_vars[0x0A] %d); "
           "nach 5 Bildern gezeigt Cut %d, Zonen %d\n", u0_init, fl_init, zon_init, cam_init, wv_init, s_shown, zonen_zustand());
    PRUEF(u0_init == 0xFF && fl_init == 0x03, "(9,84)=1: Zelle (3,0x0C) schon nach dem Init-Lauf zu (u0 0x%02X fl 0x%02X)",
          u0_init, fl_init);
    PRUEF(zon_init == 1 && cam_init == RE15_SZL_CUT_TUER_ZU && wv_init == RE15_SZL_CUT_TUER_ZU,
          "(9,84)=1: Zonen nach dem Init-Lauf wie ausgeliefert (Bild 0/2), Eintritts-Cut 8 -> 2 (Ereignis 23, Form sub00 "
          "@0x01174) VOR dem ersten Bild");
    PRUEF(z && z->u0 == 0xFF && z->floor == 0x03 && s_shown == RE15_SZL_CUT_TUER_ZU && zonen_zustand() == 1,
          "(9,84)=1: nach 5 Bildern Zelle zu, gezeigt Bild 2 (Tuer geschlossen gemalt)");
    const re15_aot_t *a = &g_aot.slots[8];
    PRUEF(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_SZL_MSG_ZU &&
          a->x == -22400 + 600 && a->z == -17200 + 850,
          "Text-Platz 8 = msg 22 auf Elzas Rechteck (-22400,-17200,1200,1700), Mitte (%ld,%ld)", (long)a->x, (long)a->z);
    int n = 0; const uint8_t *m = re15_msg_get_raw(RE15_SZL_MSG_ZU, &n);
    const char *txt = re15_msg_get_text(RE15_SZL_MSG_ZU);
    PRUEF(m && n == 23 && txt && strstr(txt, "The door is locked.") != NULL, "Nachricht 22 = \"%s\"", txt ? txt : "-");
    unsigned r1 = 0x11B0; uint8_t m1 = RE15_SZL_MSG_ZU;
    PRUEF(re15_szl_lock_stelle(&r1, &m1) == 1 && r1 == 0x11B1 && m1 == 12,
          "Schloss-Ton: (0x11B0,22) laeuft als Elzas (0x11B1,12) (gen/lock_se_sites.inc { 0x11B1, 12, M, 1 })");
    PRUEF(re15_lock_se_notice(0x11B0, RE15_SZL_MSG_ZU, 1) >= 0 && re15_lock_se_notice(0x11B0, RE15_SZL_MSG_ZU, 1) ==
          re15_lock_se_notice(0x11B1, 12, 1), "re15_lock_se_notice: gleicher RE2-Satz wie Elzas Tuer");
    /* Leon laeuft gegen die Tuer (von Osten, Blick nach Westen) — die Zelle haelt ihn */
    g_scd.player_mode = 0; re15_game_flag_set(2, 7, 0); re15_game_flag_set(1, 27, 0);
    for (int f = 0; f < 45; f++) frame(RE15_PAD_BIT_UP, 0);
    PRUEF(g_actors[RE15_ACTOR_SLOT_PLAYER].x >= -21410, "Leon kommt nicht durch (x %ld >= Zellen-Ostkante -21410)",
          (long)g_actors[RE15_ACTOR_SLOT_PLAYER].x);
    /* Eintritt mit Cut 0 (West-Tuer / Debug-Sprung): sub00 @0x0117E stellt 0 -> 7, Ereignis 23 zurueck 7 -> 0; die
     * Zonen-Maschine fuehrt Leon dann nach dem ausgelieferten Netz: oestlich der Trennwand Bild 2, westlich Bild 0 —
     * nie die OFFEN-Bilder 8/7 */
    {
        static const struct { int32_t x, z; int16_t rot; int soll; const char *wo; } k_platz[2] = {
            { -20500, -16300, 2048, RE15_SZL_CUT_TUER_ZU,  "Ost"  },
            { -23300, -16300, 0,    RE15_SZL_CUT_WEST_ZU,  "West" } };
        for (int k = 0; k < 2; k++) {
            re15_game_state_init();
            re15_game_flag_set(4, 0xF3, 1); re15_game_flag_set(3, 0x82, 1); re15_game_flag_set(3, 0x83, 1);
            re15_game_flag_set(4, 0xCD, 1); re15_game_flag_set(RE15_SZL_BANK_ZU, RE15_SZL_BIT_ZU, 1);
            if (room_boot(RE15_SZL_RAUM, k_platz[k].x, k_platz[k].z, k_platz[k].rot, 0) != 0) return;
            int c0 = (int)g_scd.cam_id, offen = 0;
            for (int f = 0; f < 30; f++) {
                frame(0, 0);
                if (s_shown == RE15_SZL_CUT_TUER_OFFEN || s_shown == RE15_SZL_CUT_WEST_OFFEN) offen++;
            }
            printf("  (9,84)=1, Eintritt Cut 0, Leon %s (%d,%d): nach dem Init-Lauf Cut %d, nach 30 Bildern gezeigt %d, "
                   "Bilder mit 7/8: %d\n", k_platz[k].wo, k_platz[k].x, k_platz[k].z, c0, s_shown, offen);
            PRUEF(c0 == RE15_SZL_CUT_WEST_ZU && s_shown == k_platz[k].soll && offen == 0 && zonen_zustand() == 1,
                  "(9,84)=1 %s: Init-Cut 0 bleibt 0 (sub00 7, Ereignis 23 zurueck), gezeigt Bild %d, nie Bild 7/8",
                  k_platz[k].wo, k_platz[k].soll);
        }
    }
    /* ohne (9,84): Original — offen, kein Platz 8, Tuerbilder 7/8 (sub00 @0x01184/@0x01188) */
    re15_game_state_init();
    re15_game_flag_set(4, 0xF3, 1); re15_game_flag_set(3, 0x82, 1); re15_game_flag_set(3, 0x83, 1); re15_game_flag_set(4, 0xCD, 1);
    if (room_boot(RE15_SZL_RAUM, -20900, -16230, 2048, 8) != 0) return;
    {
        const re15_sca_entry_t *z0 = zelle(3, 0x0C);
        PRUEF(z0 && z0->u0 == 0x00 && z0->floor == 0x40,
              "(9,84)=0 Init-Lauf: Leon sub00 @0x0118C/@0x01190 oeffnet die Zelle (u0 0x%02X fl 0x%02X)",
              z0 ? z0->u0 : -1, z0 ? z0->floor : -1);
    }
    for (int f = 0; f < 5; f++) frame(0, 0);
    z = zelle(3, 0x0C);
    PRUEF(z && z->u0 == 0x00 && !g_aot.slots[8].active, "(9,84)=0: Tuer offen wie im Original, kein Text-Platz 8");
    PRUEF(zonen_zustand() == 2 && s_shown == RE15_SZL_CUT_TUER_OFFEN,
          "(9,84)=0: Zonen getauscht (sub00 @0x01184/@0x01188), gezeigt Bild 8 = Tuer offen wie im Original");
    PRUEF(re15_szl_ereignis(RE15_SZL_RAUM, RE15_SZL_EREIGNIS_TUER) == NULL, "Ereignis 23 nur mit (9,84)");
    /* Elza: ROOM11B1 bleibt Original (Platz 8 = msg 12 solange (3,0x83)==0) */
    re15_game_state_init();
    re15_game_flag_set(4, 0xF3, 1); re15_game_flag_set(RE15_SZL_BANK_ZU, RE15_SZL_BIT_ZU, 1);
    if (room_boot(0x11B1, -22800, -16230, 0, 1) != 0) return;
    for (int f = 0; f < 5; f++) frame(0, 0);
    PRUEF(g_aot.slots[8].active && g_aot.slots[8].event_id == 12 && re15_szl_ereignis(0x11B1, RE15_SZL_EREIGNIS_TUER) == NULL,
          "ROOM11B1 (Elza): Original-Platz 8 = msg 12, kein Port-Programm");
}

/* ---- Punkt 3: ROOM10F0-Abgang ---------------------------------------------------------------------- */
static int im_bild(const re15_camera_cut_t *cut, int32_t x, int32_t y, int32_t z)
{
    re15_camera_view_t v;
    if (re15_camera_build_view(cut, &v) != 0) return 0;
    int64_t cx = ((int64_t)v.rot[0] * x + (int64_t)v.rot[1] * y + (int64_t)v.rot[2] * z) / 4096 + v.trans[0];
    int64_t cy = ((int64_t)v.rot[3] * x + (int64_t)v.rot[4] * y + (int64_t)v.rot[5] * z) / 4096 + v.trans[1];
    int64_t cz = ((int64_t)v.rot[6] * x + (int64_t)v.rot[7] * y + (int64_t)v.rot[8] * z) / 4096 + v.trans[2];
    if (cz <= 0) return 0;
    int64_t sx = 160 + (int64_t)v.fov_screen_dist * cx / cz, sy = 120 + (int64_t)v.fov_screen_dist * cy / cz;
    return sx >= -40 && sx < 360 && sy >= -40 && sy < 280;   /* Bildfeld + halbe Figurenbreite Rand */
}

static void teil_abgang10f0(void)
{
    re15_game_state_init();
    if (room_boot(RE15_CUT10F0_RAUM, RE15_CUT10F0_SPAWN_X, RE15_CUT10F0_SPAWN_Z, RE15_CUT10F0_SPAWN_DIR, 0) != 0) return;
    re15_actor_t *ada = &g_actors[RE15_CUT10F0_AKTOR_ADA];
    re15_actor_t *mv  = &g_actors[RE15_CUT10F0_AKTOR_MARVIN];
    int msg23 = -1, cuts_nach = 0, cut_nach = -1, ende = -1, mv_park = -1, ada_park = -1, mv_sichtbar = -1, ada_sichtbar = -1;
    int ton = -1, ton_n = 0, letzter = -1, mv_war = 0, ada_war = 0;
    int32_t mvx = 0, mvz = 0, ax = 0, az = 0;
    for (int f = 1; f <= 3600; f++) {
        frame(0, 0);
        if (g_scd.message_active && (int)g_scd.message_id == RE15_CUT10F0_MSG_LETZTE && msg23 < 0) { msg23 = f; letzter = s_shown; }
        if (msg23 > 0 && ende < 0) {
            if (s_shown != letzter) { cuts_nach++; letzter = s_shown; }
            cut_nach = s_shown;
            int mv_weg = mv->x <= -20000 && mv->z <= -20000, ada_weg = ada->x <= -20000 && ada->z <= -20000;
            if (mv_weg && mv_park < 0 && mv_war) { mv_park = f; mv_sichtbar = im_bild(&s_rdt.cuts[s_shown], mvx, -900, mvz); }
            if (ada_weg && ada_park < 0 && ada_war) { ada_park = f; ada_sichtbar = im_bild(&s_rdt.cuts[s_shown], ax, -900, az); }
            if (!mv_weg) { mvx = mv->x; mvz = mv->z; mv_war = 1; }
            if (!ada_weg) { ax = ada->x; az = ada->z; ada_war = 1; }
            for (int i = 0; i < s_se_n; i++)
                if (s_se[i].bild == s_bild && s_se[i].bank == RE15_CUT10F0_SE_BANK && s_se[i].satz == RE15_CUT10F0_SE_TUERKNALL) {
                    ton_n++; if (ton < 0) ton = f;
                }
            if (re15_cut10f0_zustand() == RE15_CUT10F0_FERTIG) ende = f;
        }
        if (ende > 0) break;
    }
    printf("  Zeile 23 B%d | Cut danach %d, Wechsel %d | Marvin geparkt B%d (vorher im Bild %d, (%d,%d)) | Ada geparkt B%d "
           "(vorher im Bild %d, (%d,%d)) | Tuerton B%d (%dx) | Ende B%d\n", msg23, cut_nach, cuts_nach, mv_park, mv_sichtbar,
           mvx, mvz, ada_park, ada_sichtbar, ax, az, ton, ton_n, ende);
    PRUEF(msg23 > 0 && ende > msg23, "Szene laeuft bis zum Ende (Zeile 23 B%d, Ende B%d)", msg23, ende);
    PRUEF(cuts_nach == 0 && cut_nach == 2, "nach der letzten Zeile KEIN Kamerawechsel mehr, Cut 2 (Leon im Bild) bis zum Ende");
    PRUEF(mv_park > 0 && mv_sichtbar == 0, "Marvin wird erst AUSSERHALB des Bildes von Cut 2 geparkt (kein Ploppen)");
    PRUEF(ada_park > 0 && ada_sichtbar == 0, "Ada wird erst AUSSERHALB des Bildes von Cut 2 geparkt (kein Ploppen)");
    PRUEF(ton_n == 1 && ton >= mv_park && ton >= ada_park && ton > msg23,
          "genau EIN Tuer-zu-Ton (Bank 0x0E Satz 1), zum Schluss: im Bild des letzten Parkens oder danach (B%d)", ton);
}

/* ---- Punkt 4: Irons ---------------------------------------------------------------------------------- */
static re15_emd_skeleton_t s_sk45; static re15_emd_animation_t s_an45;
static uint8_t s_em45[0x80000];
static int em045(void)
{
    static int ok = -1;
    if (ok >= 0) return ok;
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems_sz);
    int idx = re15_ems_index_for_type(0x45);
    size_t off = 0, len = 0;
    ok = s_ems && idx >= 0 && re15_ems_get_entry(s_ems, s_ems_sz, idx, &off, &len) == 0 && len <= sizeof s_em45;
    if (ok) { memcpy(s_em45, s_ems + off, len); ok = re15_emd_parse_own_bank(s_em45, len, &s_sk45, &s_an45) == 0; }
    return ok;
}

/* ROOM1150 aufbauen (inkl. re15_irons_tod_install), Leon hinstellen, N Bilder mit Pose (Neck-FSM im Pose-Builder). */
static re15_actor_t *irons_lauf_raum(uint16_t raum, int tot, int32_t lx, int32_t lz, int bilder)
{
    re15_game_state_init();
    if (tot && raum == RE15_IT_RAUM_1150) re15_game_flag_set(RE15_IT_BANK, RE15_IT_BIT_GESEHEN, 1);
    if (room_boot(raum, lx, lz, 0x05dc, 0) != 0) return NULL;
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    re15_actor_t *a = &g_actors[RE15_IT_IRONS_SLOT];
    for (int f = 0; f < bilder; f++) {
        if (tot && raum != RE15_IT_RAUM_1150 && a->active && a->type == RE15_IT_IRONS_TYP) {
            /* Stellvertreter fuer Spur L (Elza: Irons "von Anfang an tot"): dieselbe gehaltene Todespose wie
             * irons_tod_1150.c irons_pose(RE15_IT_IRONS_CLIP_TOT, RE15_IT_IRONS_BILD_TOT, 1). */
            re15_actor_set_motion(a, (int16_t)RE15_IT_IRONS_CLIP_TOT);
            a->state = 4; a->sub_state_1 = 0; a->sub_state_2 = 2;
            a->anim_frame = (uint8_t)RE15_IT_IRONS_BILD_TOT; a->anim_flags = 0; a->anim_frac = 0;
            a->motion_init_delay = 0;
        }
        frame(0, 0);
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = lx; g_actors[RE15_ACTOR_SLOT_PLAYER].z = lz;
        if (a->active && a->type == RE15_IT_IRONS_TYP) {
            int kf = re15_compute_actor_kf(&s_an45, &s_sk45, a, a->motion, a->anim_frame);
            if (kf < 0) kf = 0;
            g_anim_pose_actor = a;
            re15_skel_compute_pose(&s_sk45, kf, poses);
            g_anim_pose_actor = NULL;
        }
    }
    return (a->active && a->type == RE15_IT_IRONS_TYP) ? a : NULL;
}
static re15_actor_t *irons_lauf(int tot, int32_t lx, int32_t lz, int bilder)
{
    return irons_lauf_raum(RE15_IT_RAUM_1150, tot, lx, lz, bilder);
}

static void teil_irons(void)
{
    if (!em045()) { printf("  FEHLER: EM045\n"); g_fail++; return; }
    /* Standpunkte: Skript-Platz sub03 @0x0050 (0xafc6,0x9dc5) und ein zweiter, 3000 weiter */
    const int32_t ax_ = (int16_t)0xafc6, az_ = (int16_t)0x9dc5, bx_ = (int16_t)0xafc6 + 3000, bz_ = (int16_t)0x9dc5 + 2500;
    re15_actor_t *a = irons_lauf(0, ax_, az_, 150);
    int lebend_a = a ? a->neck_yaw : 0, lebend_fl = a ? a->neck_flags : -1;
    a = irons_lauf(0, bx_, bz_, 150);
    int lebend_b = a ? a->neck_yaw : 0;
    printf("  lebend: Kopf-Akku A %d / B %d (Flags 0x%02X)\n", lebend_a, lebend_b, lebend_fl);
    PRUEF(lebend_fl == 0x00 && lebend_a != lebend_b, "Gegenprobe LEBEND (= Stand vor der Spur fuer den Toten): +0x1b8 = 0, "
          "der Kopf folgt Leon (A %d != B %d)", lebend_a, lebend_b);
    a = irons_lauf(1, ax_, az_, 150);
    int tot_a = a ? a->neck_yaw : 99, tot_ap = a ? a->neck_pitch : 99, tot_fl = a ? a->neck_flags : -1;
    int tot_mo = a ? a->motion : -1, tot_st = a ? a->state : -1;
    a = irons_lauf(1, bx_, bz_, 150);
    int tot_b = a ? a->neck_yaw : 99, tot_bp = a ? a->neck_pitch : 99;
    printf("  tot: Clip %d state %d Flags 0x%02X | Kopf-Akku A (%d,%d) / B (%d,%d)\n", tot_mo, tot_st, tot_fl, tot_a, tot_ap, tot_b, tot_bp);
    PRUEF(tot_mo == RE15_IT_IRONS_CLIP_TOT && tot_st == 4, "Irons liegt tot (Clip 2 gehalten, re15_irons_tod_install)");
    PRUEF(tot_fl == RE15_SZL_HALS_FREI, "tot: +0x1b8 = 0x12 (kein Blickziel, Form `sb v0,440(s0)` @0x80042618)");
    /* 0x02 Release-Yaw @0x80037528-48: Ziel = Kopf-Keyframe-Yaw -> Akku 0; 0x10 Pitch-Null @0x80037554-60:
     * Ziel = GESAMT-Neigung 0 -> Akku = -Keyframe-Pitch (skeleton_common.c). Beides konstant, Leon egal. */
    PRUEF(tot_a == tot_b && tot_ap == tot_bp && tot_a == 0,
          "tot: Kopf auf der Clip-Pose (Yaw-Akku 0, Pitch-Akku %d = -Keyframe-Neigung), an beiden Standpunkten gleich",
          tot_ap);
    /* Elza ROOM1151: Eigentuemer Spur L (elza_r36.c: Irons immer tot, Kopf fest in Plc_neck-Form 0x80|0x12).
     * Integration Runde 36: dieser Haken greift dort nicht mehr ein; geprueft wird nur, dass der Kopf fest bleibt
     * (kein Blickziel, Akku an beiden Standpunkten gleich). Den frueheren Fall "lebend" gibt es seit Spur L nicht. */
    a = irons_lauf_raum(RE15_SZL_IRONS_ELZA_RAUM, 1, ax_, az_, 150);
    int e_a = a ? a->neck_yaw : 99, e_ap = a ? a->neck_pitch : 99, e_fl = a ? a->neck_flags : -1;
    a = irons_lauf_raum(RE15_SZL_IRONS_ELZA_RAUM, 1, bx_, bz_, 150);
    int e_b = a ? a->neck_yaw : 99, e_bp = a ? a->neck_pitch : 99;
    printf("  ROOM1151 tot: Flags 0x%02X Akku A (%d,%d) / B (%d,%d)\n", e_fl, e_a, e_ap, e_b, e_bp);
    PRUEF((e_fl & 0x7F) == RE15_SZL_HALS_FREI && e_a == e_b && e_ap == e_bp && e_a == 0,
          "ROOM1151 (Elza, Spur L): kein Blickziel (+0x1b8 & 0x7F = 0x12), Kopf an beiden Standpunkten gleich");
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "programm";
    printf("test_r36_szenenleon %s\n", teil);
    if      (!strcmp(teil, "programm"))    teil_programm();
    else if (!strcmp(teil, "schub"))       teil_schub();
    else if (!strcmp(teil, "erweiterung")) teil_erweiterung();
    else if (!strcmp(teil, "tuer"))        teil_tuer();
    else if (!strcmp(teil, "abgang10f0"))  teil_abgang10f0();
    else if (!strcmp(teil, "irons"))       teil_irons();
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf("%s: %s (%d Fehler)\n", teil, g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
