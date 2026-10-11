/* test_r36_szene1150.c — RIEGEL Spur K (Runde 36): Irons-Todesszene / Montage.
 *
 * Dossier: analysis/befunde_runde36/K_szene1150.md. Code: engine/src/irons_tod_1150.c, include/re15_irons_tod.h,
 * engine/src/gen/knall_bank.inc (tools/r35_l/knall_cut.py).
 *
 * Teile (je ein ctest-Eintrag):
 *   knall_re2   K1: Satz 0 der Knall-Bank = RE2 ROOM1060 Se_on 0x0a (sub05 @0x00F48), Welle bytegleich aus der RDT
 *               (EDT @0x03F80 `00 00 85 20`, Tone 8/9 -> VAG 5 @Datei 0x066B8, 8768 B), zwei Lagen, Direkt-Stimme;
 *               NICHT mehr DOOR04 Door_exit.
 *   folge       K3: die ganze Kette ueber den ECHTEN Weg (Szene 1150 -> Tuer -> 1130 -> 1040 -> 1030 -> 11C0 -> 1150),
 *               jeder Raum aufgebaut wie im Spiel (scd_room_reenter mit Ziel/Ort/Cut der Tuer-Anforderung):
 *               je Montage-Schritt KNALL < SPAWN < KAMERA (Ereignisfolge re15_irons_tod_folge), die Zombies stehen
 *               nach dem Raumaufbau schon da (vor dem ersten Bild), im Schritt selbst erscheint KEIN Zombie auf der
 *               Kamera, die ihn zeigt; Varianten 1140/1070 leer und Tor schon aufgebrochen.
 *   dialog11c0  K2: Ada und Marvin wenden sich bei ihren Zeilen einander zu (Koerper: Ausrichtungs-Test der Engine
 *               re15_ai_arc_test mit dem Kegel des Drehens, Typ-Tabelle @0x80076c41; Kopf: Plc_neck Modus 1 auf den
 *               Partner); Beats der Nutzer-Vorgabe r35 (zum Gebaeude, "Oh, no, Leon!" zum Gebaeude, Ada sieht
 *               Marvin nach). */
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
#include "re15_vab.h"
#include "re15_irons_tod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);
int  re15_ai_arc_test(const re15_actor_t *e, int32_t px, int32_t pz, int cone);

static int g_fail = 0;
extern int g_test_tuer_laden_count, g_test_tuer_se_last, g_test_tuer_se_count;
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
    if (s_raw && s_rdt_raum == room) return 0;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (wie test_r35_cut1150.c). */
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
}

static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static uint8_t  s_bank_buf[4][0x60000];
static void bank_laden(uint8_t type, int buf)
{
    if (!s_ems) {
        char p2[600]; snprintf(p2, sizeof p2, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
        s_ems = slurp(p2, &s_ems_sz);
    }
    if (!s_ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, s_ems_sz, idx, &off, &len) != 0) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb || len > sizeof s_bank_buf[buf]) return;
    memcpy(s_bank_buf[buf], s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank_buf[buf], len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank_buf[buf], len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank_buf[buf], len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank_buf[buf], len, &eb->skel_own, &eb->anim_own) == 0);
}

/* Raum aufbauen wie das Spiel (scd_room_reenter = Init-Lauf + Installer); KEIN Spielbild. */
static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    bank_laden(0x45, 0); bank_laden(0x42, 1); bank_laden(0x40, 2); bank_laden(0x16, 3);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static int aktive_gegner(void)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type != 0) n++;
    return n;
}

/* Bis zur Tuer-Anforderung fahren; je Bild: Kamera, Gegnerzahl (Zuwachs auf der Kamera, die ihn zeigt = "aus dem
 * Nichts"). Liefert die Bildzahl, 0 = keine Tuer. */
typedef struct { int bilder; unsigned ziel; int cut; int32_t x, z; int16_t yaw; int zuwachs_bild; int zuwachs_cut; } schritt_t;
static int bis_tuer(schritt_t *S, int max)
{
    memset(S, 0, sizeof *S); S->zuwachs_bild = -1;
    int n0 = aktive_gegner();
    for (int f = 0; f < max; f++) {
        frame();
        int n = aktive_gegner();
        if (n > n0 && S->zuwachs_bild < 0) { S->zuwachs_bild = f; S->zuwachs_cut = (int)g_scd.cam_id; }
        n0 = n;
        if (g_room_change.pending) {
            S->bilder = f + 1; S->ziel = g_room_change.room_id; S->cut = g_room_change.target_cut;
            S->x = g_room_change.x; S->z = g_room_change.z; S->yaw = (int16_t)g_room_change.yaw_4096;
            return S->bilder;
        }
    }
    return 0;
}

/* ---- Teil knall_re2 (K1) -------------------------------------------------------------------------------------- */
#include "../../engine/src/gen/knall_bank.inc"
static void teil_knall_re2(void)
{
    printf("== knall_re2 ==\n");
    size_t n = 0; char p[700];
    snprintf(p, sizeof p, "%s/ROOM1060.RDT", RE15_RE2_RDT_DIR);
    uint8_t *r = slurp(p, &n);
    PRUEF(r != NULL && n > 0x066B8 + 8768, "RE2 ROOM1060.RDT gelesen (%u B)", (unsigned)n);
    if (!r) return;
    static const uint8_t se_on[12] = { 0x36,0x02,0x0a,0x01,0x00,0x00,0xdc,0x88,0xf8,0xf8,0x3d,0x05 };
    static const uint8_t edt[4] = { 0x00,0x00,0x85,0x20 };
    PRUEF(!memcmp(r + 0x0F48, se_on, 12), "RE2 ROOM1060 sub05 @0x00F48 = Se_on Bank 2 Satz 0x0a (der Schlag gegen die Tuer)");
    PRUEF(!memcmp(r + 0x03F80, edt, 4), "RE2 ROOM1060 EDT[0x0a] @0x03F80 = 00 00 85 20 (Tone 8, +1 Lage, Direkt-Stimme)");
    const uint8_t *b = k_knall_bank;
    PRUEF(!memcmp(b + 0xC38, r + 0x066B8, 8768), "Knall-Bank VAG 1 = RE2 ROOM1060 VAG 5 @Datei 0x066B8 bytegleich (8768 B)");
    re15_vab_t vab; int rc = re15_vab_parse(b + 0x10, 0xC28, &vab);
    int vags[8], tones[8];
    int nl = (rc == 0) ? re15_edt_resolve_layers_ex(b, &vab, KNALL_SE_TUER, vags, tones, 8) : 0;
    PRUEF(nl == 2 && vags[0] == 0 && vags[1] == 0 && tones[0] == 0 && tones[1] == 1,
          "Satz 0 (Tuerknall) spielt ZWEI Lagen auf VAG 1 (Tone 0/1 = RE2 Tone 8/9): %d Lagen", nl);
    int nl1 = (rc == 0) ? re15_edt_resolve_layers_ex(b, &vab, KNALL_SE_1030, vags, tones, 8) : 0;
    PRUEF(nl1 == 1 && vags[0] == 1 && tones[0] == 2, "Satz 1 (Knall wie ROOM1030) = Tone 2 -> VAG 2 (%d Lage)", nl1);
    /* nicht mehr der Door_exit-Satz von DOOR04: dessen VAG 3 waere 9216 B */
    PRUEF(vab.samples[0].size != 9216, "kein DOOR04 Door_exit mehr (VAG 1 = %u B)", (unsigned)vab.samples[0].size);
    free(r);
}

/* ---- Teil folge (K3) ----------------------------------------------------------------------------------------- */
static void szene_flags(void) { re15_game_state_init(); re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1); }

static void folge_drucken(void)
{
    int n = re15_irons_tod_folge_n();
    printf("  Ereignisfolge (%d):", n);
    for (int i = 0; i < n; i++) {
        const re15_it_folge_t *f = re15_irons_tod_folge(i);
        printf(" %c%04X/T%d/%d%s", f->art, f->raum, f->takt, f->wert, f->aufbau ? "a" : "");
    }
    printf("\n");
}

/* Erstes Ereignis der Art `art` in Raum `raum` ab Index `ab`; -1 = keins. */
static int finde(char art, uint16_t raum, int ab)
{
    for (int i = ab; i < re15_irons_tod_folge_n(); i++) {
        const re15_it_folge_t *f = re15_irons_tod_folge(i);
        if (f->art == art && f->raum == raum) return i;
    }
    return -1;
}
/* Letzter Knall VOR Index i (egal in welchem Raum). */
static int knall_vor(int i)
{
    for (int k = i - 1; k >= 0; k--) if (re15_irons_tod_folge(k)->art == 'K') return k;
    return -1;
}

/* Die Kette ab 1150 ueber die echten Tuer-Anforderungen fahren; je Raum: Gegner nach dem Raumaufbau (vor dem
 * ersten Bild), Zuwachs auf der zeigenden Kamera. */
typedef struct { uint16_t raum; int gegner_aufbau; int cut_aufbau; int marvin_aufbau; int prog_len; schritt_t s; } raum_t;
static int s_prog_max = 0;   /* laengstes zusammengesetztes Laufprogramm (s_prog[640] in irons_tod_1150.c) */
static int kette(raum_t *R, int maxr)
{
    int nr = 0;
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1150, -17150, -11960, 1600, 0) != 0) return 0;
    PRUEF(re15_irons_tod_zustand() == RE15_IT_SZENE, "Szene gestartet");
    schritt_t S;
    if (!bis_tuer(&S, 4000)) { PRUEF(0, "1150: keine Tuer"); return 0; }
    while (nr < maxr) {
        unsigned ziel = S.ziel; int cut = S.cut;
        if (room_boot((uint16_t)ziel, S.x, S.z, S.yaw, (uint8_t)cut) != 0) break;
        R[nr].raum = (uint16_t)ziel; R[nr].gegner_aufbau = aktive_gegner(); R[nr].cut_aufbau = (int)g_scd.cam_id;
        R[nr].marvin_aufbau = g_actors[4].active && g_actors[4].type == 0x40;
        { int pl = 0; (void)re15_irons_tod_laufprogramm(&pl); R[nr].prog_len = pl; if (pl > s_prog_max) s_prog_max = pl; }
        if (ziel == 0x1150) { for (int f = 0; f < 200; f++) frame(); nr++; break; }   /* Rueckkehr */
        if (!bis_tuer(&R[nr].s, 2500)) { PRUEF(0, "ROOM%04X: keine Tuer", ziel); nr++; break; }
        S = R[nr].s;
        nr++;
    }
    return nr;
}

/* Fuer einen Montage-Raum: Knall (im vorigen Raum oder im selben Raum vor dem Kamerawechsel) < Spawn < Kamera. */
static void schritt_pruefen(uint16_t raum, int erwart_knall_satz, const char *fall)
{
    int s = finde('S', raum, 0), c = finde('C', raum, 0);
    int k = (s >= 0) ? knall_vor(s) : -1;
    const re15_it_folge_t *K = re15_irons_tod_folge(k), *Sf = re15_irons_tod_folge(s), *Cf = re15_irons_tod_folge(c);
    PRUEF(K && Sf && Cf && k < s && s < c && K->takt < Sf->takt && Sf->takt <= Cf->takt,
          "%s ROOM%04X: Knall (#%d T%d) < Spawn (#%d T%d) < Kamera (#%d T%d)", fall, raum, k, K ? K->takt : -1, s,
          Sf ? Sf->takt : -1, c, Cf ? Cf->takt : -1);
    PRUEF(K && K->wert == erwart_knall_satz, "%s ROOM%04X: der Knall davor ist Satz %d (%d)", fall, raum, erwart_knall_satz,
          K ? K->wert : -1);
    PRUEF(Sf && Sf->aufbau && Cf && Cf->aufbau, "%s ROOM%04X: Spawn und erstes Kamerabild im Raumaufbau (vor dem ersten Bild)",
          fall, raum);
}

static void teil_folge(void)
{
    printf("== folge ==\n");
    raum_t R[8]; int nr;
    /* A: alle leben (1140 und 1070), Tor zu, (4,15)=0 */
    szene_flags();
    int knall0 = g_test_tuer_se_count;
    nr = kette(R, 8);
    folge_drucken();
    printf("  Raeume:"); for (int i = 0; i < nr; i++) printf(" %04X(Gegner %d, Cut %d)", R[i].raum, R[i].gegner_aufbau, R[i].cut_aufbau); printf("\n");
    PRUEF(nr == 5 && R[0].raum == 0x1130 && R[1].raum == 0x1040 && R[2].raum == 0x1030 && R[3].raum == 0x11C0 && R[4].raum == 0x1150,
          "A: Kette 1150 -> 1130 -> 1040 -> 1030 -> 11C0 -> 1150 (%d Raeume)", nr);
    /* prog_anhaengen verwirft still, was nicht in s_prog[640] passt: jedes zusammengesetzte Programm muss passen und mit
     * Evt_end enden (sonst fehlte z.B. der Tuersatz) */
    {
        int ok_len = 1; for (int i = 0; i < nr; i++) if (R[i].prog_len <= 0 || R[i].prog_len > 640) ok_len = 0;
        PRUEF(ok_len && s_prog_max < 640, "A: Laufprogramme passen in s_prog[640] (laengstes %d B)", s_prog_max);
    }
    if (nr >= 4) {
        PRUEF(R[0].gegner_aufbau == 5 && R[0].cut_aufbau == 0, "A: ROOM1130 nach dem Raumaufbau 5 Zombies, Cut 0 (%d, %d)",
              R[0].gegner_aufbau, R[0].cut_aufbau);
        PRUEF(R[0].s.zuwachs_bild < 0, "A: ROOM1130 waehrend des Schritts erscheint KEIN Zombie (vorher: Bild 31 auf Cut 0)");
        PRUEF(R[1].gegner_aufbau == 5 && R[1].cut_aufbau == 1, "A: ROOM1040 nach dem Raumaufbau 5 Zombies, Cut 1 (%d, %d)",
              R[1].gegner_aufbau, R[1].cut_aufbau);
        PRUEF(R[1].s.zuwachs_bild < 0, "A: ROOM1040 waehrend des Schritts erscheint KEIN Zombie");
        PRUEF(R[2].cut_aufbau == 7 && R[2].gegner_aufbau >= 11, "A: ROOM1030 nach dem Raumaufbau Cut 7, %d Gegner (6 Raum + 5 Kopien)",
              R[2].gegner_aufbau);
        PRUEF(R[2].s.zuwachs_bild >= 0 && R[2].s.zuwachs_cut == 7,
              "A: ROOM1030 die Kriecher erscheinen auf Cut 7 (Tor nicht im Bild), Bild %d Cut %d", R[2].s.zuwachs_bild, R[2].s.zuwachs_cut);
        PRUEF(R[3].cut_aufbau == 13 && R[3].marvin_aufbau, "A: ROOM11C0 Marvin im Raumaufbau (vor dem ersten Bild), Cut 13");
    }
    schritt_pruefen(0x1130, 0, "A");
    schritt_pruefen(0x1040, 1, "A");
    schritt_pruefen(0x1030, 0, "A (Cut 7)");
    {   /* 1030 Tor-Teil: Original-Knall (Satz-Kennung 2) < Kriecher (+3) < Kamera Cut 12, alles im Spieltakt */
        int k = finde('K', 0x1030, 0), s2 = -1, c2 = -1;
        if (k >= 0) { for (int i = k + 1; i < re15_irons_tod_folge_n(); i++) {
            const re15_it_folge_t *f = re15_irons_tod_folge(i);
            if (f->raum != 0x1030) continue;
            if (s2 < 0 && f->art == 'S' && !f->aufbau) s2 = i;
            if (c2 < 0 && f->art == 'C' && !f->aufbau && f->wert == 12) c2 = i; } }
        const re15_it_folge_t *K = re15_irons_tod_folge(k), *S2 = re15_irons_tod_folge(s2), *C2 = re15_irons_tod_folge(c2);
        PRUEF(K && S2 && C2 && K->wert == 2 && S2->wert == 3 && K->takt < S2->takt && S2->takt < C2->takt,
              "A ROOM1030 Tor: Original-Knall @0x02776 (T%d) < 3 Kriecher (T%d) < Kamera Cut 12 (T%d)",
              K ? K->takt : -1, S2 ? S2->takt : -1, C2 ? C2->takt : -1);
    }
    PRUEF(finde('K', 0x1130, 0) >= 0 && re15_irons_tod_folge(finde('K', 0x1130, 0))->wert == 1 &&
          finde('K', 0x1130, 0) > finde('C', 0x1130, 0),
          "A: in ROOM1130 nur der Knall des naechsten Schritts (Satz 1) NACH dem Kamerabild, keiner davor");
    PRUEF(g_test_tuer_se_count - knall0 == 3, "A: Knall-Bank 3x gespielt (Ende 1150 Satz 0, Ende 1130 Satz 1, Ende 1040 Satz 0; "
          "der Tor-Knall in 1030 ist das Original-Se_on): %d", g_test_tuer_se_count - knall0);

    /* B: 1140 und 1070 leer, Tor zu ((4,15)=0) -> 1150 Satz 1 -> 1040 -> Satz 1 -> 1030 Cut 12 (Bruch im Raumaufbau) */
    szene_flags();
    for (int i = 0; i < 5; i++) { re15_game_flag_set(7, (uint8_t)(0xd3 + i), 1); re15_game_flag_set(7, (uint8_t)(0xc6 + i), 1); }
    knall0 = g_test_tuer_se_count;
    nr = kette(R, 8);
    folge_drucken();
    PRUEF(g_test_tuer_se_count - knall0 == 2, "B: Knall-Bank 2x (Ende 1150 Satz 1 'passiert nichts' in 1140, Ende 1040 Satz 1): %d",
          g_test_tuer_se_count - knall0);
    PRUEF(nr == 4 && R[0].raum == 0x1040 && R[1].raum == 0x1030 && R[2].raum == 0x11C0, "B: Kette 1150 -> 1040 -> 1030 -> 11C0 -> 1150");
    schritt_pruefen(0x1040, 1, "B");
    schritt_pruefen(0x1030, 1, "B (Tor, Cut 12)");
    if (nr >= 2) {
        PRUEF(R[1].cut_aufbau == 12 && re15_game_flag_get(4, 15) == 1,
              "B: ROOM1030 erstes Bild = Cut 12 (aufgebrochenes Tor, Bruch im Raumaufbau): Cut %d, (4,15)=%d", R[1].cut_aufbau,
              re15_game_flag_get(4, 15));
        PRUEF(R[1].s.zuwachs_bild < 0, "B: ROOM1030 waehrend des Schritts erscheint KEIN Zombie (Kriecher schon im Raumaufbau)");
        PRUEF(finde('K', 0x1030, 0) < 0, "B: in ROOM1030 kein weiterer Knall (der Knall kam am Ende von 1040)");
    }

    /* C: Tor schon aufgebrochen ((4,15)=1), alle leben */
    szene_flags(); re15_game_flag_set(4, 15, 1);
    nr = kette(R, 8);
    folge_drucken();
    PRUEF(nr == 5 && R[2].raum == 0x1030, "C: Kette mit 1030");
    schritt_pruefen(0x1030, 0, "C (Cut 7)");
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

/* ---- Teil dialog11c0 (K2) ---------------------------------------------------------------------------------- */
#define ADA_X   (-8965)
#define ADA_Z   (-14347)
#define TUER_X  (-25100)
#define TUER_Z  (17250)
static int zugewandt(const re15_actor_t *a, int32_t x, int32_t z)
{
    return re15_ai_arc_test(a, x, z, 0x60) == 0;   /* Kegel des Drehens fuer Typ 0x40/0x42 (@0x80076c41) */
}
static int kopf_auf(const re15_actor_t *a, int32_t x, int32_t z)
{
    return (a->neck_flags & 0x04) && a->neck_tx == x && a->neck_tz == z;
}
static void teil_dialog11c0(void)
{
    printf("== dialog11c0 ==\n");
    szene_flags(); re15_game_flag_set(9, 73, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_S1030);
    if (room_boot(0x11C0, RE15_IT_PARK_11C0_X, RE15_IT_PARK_11C0_Z, RE15_IT_PARK_11C0_YAW, 13) != 0) return;
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);   /* Reihenfolge der exe (RBJ nach dem Installer) */
    const re15_actor_t *ada = &g_actors[1], *mar = &g_actors[4];
    PRUEF(ada->active && ada->type == 0x42 && mar->active && mar->type == 0x40, "Ada und Marvin nach dem Raumaufbau da (vor dem ersten Bild)");
    /* je Nachricht: Zustand in jedem Bild der ersten 45 Bilder der Zeile */
    int msg_bild[16], n_msg = 0, letzte = -1;
    int ok_ada_zu_marvin_10 = 0, ok_mar_kopf_ada_10 = 0, ok_mar_zu_ada_11 = 0, ok_mar_tuer_12 = 0, ok_ada_kopf_mar_12 = 0,
        ok_mar_zu_ada_13 = 0, ok_ada_tuer_14 = 0, ok_ada_kopf_mar_11 = 0, ok_mar_kopf_ada_13 = 0;
    int bild_mit[16] = {0};
    schritt_t S; memset(&S, 0, sizeof S);
    int f;
    for (f = 0; f < 1500; f++) {
        frame();
        if (g_scd.message_active && (int)g_scd.message_id != letzte) {
            letzte = (int)g_scd.message_id;
            if (n_msg < 16) msg_bild[n_msg++] = f;
        }
        if (!g_scd.message_active) letzte = -1;
        /* gemessen wird in den ersten 45 Bildern (1,5 s) jeder Zeile: danach laeuft schon der naechste Schritt des Programms,
         * waehrend der Untertitel noch steht (Anzeigedauer re15_msg_compute_duration, msg 12 = 72 / msg 13 = 71 Bilder) */
        int id = (g_scd.message_active && n_msg > 0 && f - msg_bild[n_msg - 1] < 45) ? (int)g_scd.message_id : -1;
        int32_t mx = mar->x, mz = mar->z;
        /* gemessen wird in JEDEM Bild der Zeile; gezaehlt wird, wie viele Bilder die Bedingung haelt */
        if (id == 10) { bild_mit[10]++; ok_ada_zu_marvin_10 += zugewandt(ada, mx, mz); ok_mar_kopf_ada_10 += kopf_auf(mar, ADA_X, ADA_Z); }
        if (id == 11) { bild_mit[11]++; ok_mar_zu_ada_11 += zugewandt(mar, ADA_X, ADA_Z); ok_ada_kopf_mar_11 += kopf_auf(ada, -8163, -13748); }
        if (id == 12) { bild_mit[12]++; ok_mar_tuer_12 += kopf_auf(mar, TUER_X, TUER_Z); ok_ada_kopf_mar_12 += kopf_auf(ada, -8163, -13748); }
        if (id == 13) { bild_mit[13]++; ok_mar_zu_ada_13 += zugewandt(mar, ADA_X, ADA_Z); ok_mar_kopf_ada_13 += kopf_auf(mar, ADA_X, ADA_Z); }
        if (id == 14) { bild_mit[14]++; ok_ada_tuer_14 += zugewandt(ada, TUER_X, TUER_Z); }
        if (g_room_change.pending) break;
    }
    printf("  Nachrichten-Beginn:"); for (int i = 0; i < n_msg; i++) printf(" %d", msg_bild[i]); printf(" (Schnitt nach %d Bildern)\n", f + 1);
    PRUEF(n_msg == 5, "fuenf Zeilen 10..14 (%d)", n_msg);
    PRUEF(bild_mit[10] > 0 && ok_ada_zu_marvin_10 * 10 >= bild_mit[10] * 9,
          "msg 10 'Ada: What was this noise?': Ada Marvin zugewandt in %d von %d Bildern", ok_ada_zu_marvin_10, bild_mit[10]);
    PRUEF(bild_mit[10] > 0 && ok_mar_kopf_ada_10 == bild_mit[10],
          "msg 10: Marvin sieht Ada an (Plc_neck Modus 1 auf Ada) in %d von %d Bildern", ok_mar_kopf_ada_10, bild_mit[10]);
    PRUEF(bild_mit[11] > 0 && ok_mar_zu_ada_11 * 10 >= bild_mit[11] * 9,
          "msg 11 'Marvin: Yes!....': Marvin Ada zugewandt in %d von %d Bildern", ok_mar_zu_ada_11, bild_mit[11]);
    PRUEF(bild_mit[11] > 0 && ok_ada_kopf_mar_11 == bild_mit[11], "msg 11: Ada sieht Marvin an in %d von %d Bildern",
          ok_ada_kopf_mar_11, bild_mit[11]);
    PRUEF(bild_mit[12] > 0 && ok_mar_tuer_12 == bild_mit[12] && ok_ada_kopf_mar_12 == bild_mit[12],
          "msg 12 'Oh, no, Leon!': Marvin blickt zum Gebaeude (%d/%d), Ada sieht ihn an (%d/%d)", ok_mar_tuer_12, bild_mit[12],
          ok_ada_kopf_mar_12, bild_mit[12]);
    PRUEF(bild_mit[13] > 0 && ok_mar_zu_ada_13 * 10 >= bild_mit[13] * 9 && ok_mar_kopf_ada_13 * 10 >= bild_mit[13] * 9,
          "msg 13 'I have to help him': Marvin hat sich zu Ada gedreht (Koerper %d/%d, Kopf %d/%d)", ok_mar_zu_ada_13, bild_mit[13],
          ok_mar_kopf_ada_13, bild_mit[13]);
    PRUEF(bild_mit[14] > 0 && ok_ada_tuer_14 * 10 >= bild_mit[14] * 9,
          "msg 14 'Marvin!...': Ada sieht ihm nach (zur Tuer gewandt) in %d von %d Bildern", ok_ada_tuer_14, bild_mit[14]);
    PRUEF(g_room_change.pending && g_room_change.room_id == 0x1150 && g_room_change.target_cut == 7,
          "Schnitt zurueck nach ROOM1150 Cut 7");
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    re15_game_state_init();
    if (!strcmp(teil, "knall_re2") || !strcmp(teil, "alle")) teil_knall_re2();
    if (!strcmp(teil, "folge") || !strcmp(teil, "alle")) teil_folge();
    if (!strcmp(teil, "dialog11c0") || !strcmp(teil, "alle")) teil_dialog11c0();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
