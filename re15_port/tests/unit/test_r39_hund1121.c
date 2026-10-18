/*
 * test_r39_hund1121.c — Runde 39 Spur R "hund1121": Riegel des Hunde-Sprungs durch das Fenster in ROOM1121 (Elza,
 * Panikphase). Dossier analysis/befunde_runde39/R_hund1121.md, Code engine/src/hund1121_r39.c,
 * include/re15_hund1121_r39.h, Startmaschine engine/src/enemy_ai_re2_dog.c (re2d_windowjump_launch).
 *
 *   ereignis      Tor (9,97) und Einmal-Bit (7,0xAE), Raum 1121 only; Slot 4 AUTO-Ereignis 30 auf Leons Band (sat 0x41);
 *                 Ausloeser ueber den AOT-Scan; das Programm ueber die echte VM: 5 x Sca_id_set (Band-1-Rueckwand u0 0),
 *                 Hund (5400,-1827,12100) dir 0x418 Kill 0xAE, (7,0xAE) = 1, Slot 4 -> sce 0. Leon ROOM1120 unberuehrt.
 *   sprung        RE2-Hirn mit der echten RE2-Bank (CDEMD0.EMS): ROOM21A0-sub07-Schritte (+5 = 14 / Evt_next / +7 = 1),
 *                 Warten ohne Wandschub, Anlauf 8 Bilder x 65 (Phase 1 faellt in den Rumpf), Absprung SE 3 mit 280,
 *                 Bogen -320/+40 gegen eine UNABHAENGIGE Rechnung, Clip 20 -> 21, Glas im Bild des Durchgangs durch
 *                 z 11200 (13 Splitter, Knall +3/+8 Satz 0x21), Wand wieder zu, Landung y 0 mit SE 10, danach Sub 2 RUN;
 *                 kein Schatten jenseits der Glasebene.
 *   re15          RE1.5-KI-Modus: Spawn grid 0x40 -> Zustand 4 Sub 0 (wartet), Freigabe grid 0x43, Sprung, Landung,
 *                 Ausgang Zustand 1; Glas im Durchgang.
 *   einmal        nach dem Sprung: Wiedereintritt ohne Ereignis; ohne (9,97) kein Ereignis; Leon ROOM1120 nicht.
 *   treffer       Treffer auf der Bruestung vor dem Absprung: Luft-Bit, RE2-Luft-Treffer bringt den Hund auf den Boden,
 *                 kein Glas, Wand wieder fest.
 *   startmaschine RE2 Sub 14 direkt: Phase 0 wartet 60 Bilder (kein +7-Schreiber @0x80102CD4-F4), Phase 1 bewegt im
 *                 selben Bild (@0x80102D20 -> @0x80102D24), Absprung nach 8 Anlaufbildern (+0x158 = 8 @0x80101398).
 * Werkzeuge (kein Riegel, fuer den Integrationshaken):
 *   karte <datei> <raum-hex> <x> <z> <gier> [bank:bit ...]   Spielstand (Figur aus der Raumvariante) fuer CONTINUE
 *   mitschnitt <cap.raw> <exit-bild> <bild> <laenge> ...      Spitze/Median je Bildfenster (RE15_AUDIO_CAP_SYNC)
 *   bild <a.ppm> <b.ppm> <x0> <y0> <x1> <y1>                  Zahl der verschiedenen Pixel im Rechteck
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_ai_flavor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_fenster1120.h"
#include "re15_hund1121_r39.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_gameflow.h"
#include "re2_fx.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void scd_register_current_rdt(const re15_rdt_t *rdt);
extern uint8_t re15_em_status_zone(void);
extern int re15_re2dog_tick(int slot);

static int fail(int code, const char *m) { printf("FAIL %d: %s\n", code, m); return 1; }

static uint8_t *datei(const char *pfad, long *n)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) { printf("kann %s nicht oeffnen\n", pfad); return NULL; }
    fseek(f, 0, SEEK_END); long l = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)l);
    if (b && fread(b, 1, (size_t)l, f) != (size_t)l) { free(b); b = NULL; }
    fclose(f);
    if (b && n) *n = l;
    return b;
}

/* ---- Raum, Banken, Spione ------------------------------------------------------------------------ */
static uint8_t *s_rdt_buf;
static int raum_laden(unsigned raum)
{
    char p[512];
    snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE1/ROOM%04X.RDT", raum);
    long n = 0;
    free(s_rdt_buf);
    s_rdt_buf = datei(p, &n);
    if (!s_rdt_buf) return -1;
    if (re15_rdt_parse(s_rdt_buf, (size_t)n, &g_room_rdt) != 0) return -2;
    g_room_rdt_ok = 1;
    scd_register_current_rdt(&g_room_rdt);
    g_current_room_id = raum;
    return 0;
}

static int esp_registrieren(void)
{
    static uint8_t *core, *raum; static long nc, nr;
    if (!core) core = datei(RE15_ASSET_PSX_DIR "/../RE2/CORE00.ESP", &nc);
    if (!raum) raum = datei(RE15_ASSET_PSX_DIR "/../RE2/GLAS1090.ESP", &nr);
    if (!core || !raum) return -1;
    if (re2fx_register_core(core, (size_t)nc) != 0) return -2;
    return re2fx_register_raum(raum, (size_t)nr);
}

static int hundebank_laden(void)
{
    static uint8_t *ems; static long ems_sz;
    if (!ems) ems = datei(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_sz);
    if (!ems) return -1;
    re15_enemy_bank_t *eb = re15_enemy_find(0x20);
    if (!eb) eb = re15_enemy_alloc(0x20);
    if (!eb || re2_ems_load_bank(ems, (size_t)ems_sz, 0x20, eb, NULL) != 0) return -2;
    eb->buf = NULL; eb->ok = 1;
    return 0;
}

static int s_knall_n, s_knall_satz;
static void knall_spion(int satz) { s_knall_n++; s_knall_satz = satz; }
static int s_se[64], s_se_n;
static void se_spion(int id, int f2000) { (void)f2000; if (s_se_n < 64) s_se[s_se_n++] = id; }
static void bank_spion(int bank) { (void)bank; }

static void grund(int flavor)
{
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_game_state_init();
    re15_ai_flavor_set(flavor);
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    re2fx_reset();
    s_knall_n = 0; s_knall_satz = -1; s_se_n = 0;
    re15_fenster1120_se_hook = knall_spion;
    re15_re2dog_audio_hook(se_spion, bank_spion);
    g_gameflow.character = 4;                           /* Elza (Raumvariante xxx1) */
}

static re15_actor_t *spieler(int32_t x, int32_t z)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = x; pl->y = 0; pl->z = z; pl->rot_y = 0x400; pl->floor = 0;
    return pl;
}

static int wand_u0(int region)
{
    int base = 0;
    for (int i = 0; i < region; i++) base += g_room_rdt.sca_rgn[i];
    return g_room_rdt.sca[base + RE15_R39R_WAND_INDEX].u0;
}

/* Ausloeser wie im Spiel: Installer, Spieler ins Band, AOT-Scan, scd_event_fire, VM-Takt. */
static int ausloesen(void)
{
    re15_hund1121_r39_install(RE15_R39R_RAUM);
    if (re15_hund1121_r39_phase() != RE15_R39R_SCHARF) return -1;
    re15_actor_t *pl = spieler(4986, 7000);
    re15_aot_scan(pl->x, pl->z, 1);                     /* ausserhalb: kein Ereignis */
    if (g_aot.fired_event_id_this_frame == RE15_R39R_EREIGNIS) return -2;
    pl->z = 6000;
    re15_aot_scan(pl->x, pl->z, 1);
    if (g_aot.fired_event_id_this_frame != RE15_R39R_EREIGNIS) return -3;
    scd_event_fire(g_aot.fired_event_id_this_frame);
    scd_vm_tick();
    return 0;
}

static void bild(void)
{
    re15_hund1121_r39_tick();                           /* game_step: vor re15_enemy_ai_run_all */
    re15_enemy_ai_run_all(1);
    re15_actors_anim_advance();
    re2fx_tick();
}

/* ============================================================================================ */
static int pruef_ereignis(void)
{
    grund(RE15_AI_FLAVOR_RE2);
    if (raum_laden(RE15_R39R_RAUM) != 0) return fail(1, "ROOM1121.RDT");
    /* Tor: ohne (9,97) nichts */
    re15_hund1121_r39_install(RE15_R39R_RAUM);
    if (re15_hund1121_r39_phase() != RE15_R39R_AUS || g_aot.slots[RE15_R39R_SLOT].active) return fail(2, "ohne (9,97) kein Ereignis");
    re15_game_flag_set(RE15_R39R_PANIK_BANK, RE15_R39R_PANIK_BIT, 1);
    /* Einmal-Bit gesetzt -> nichts */
    re15_game_flag_set(re15_em_status_zone(), RE15_R39R_KILL, 1);
    re15_hund1121_r39_install(RE15_R39R_RAUM);
    if (re15_hund1121_r39_phase() != RE15_R39R_AUS) return fail(3, "(7,0xAE) = 1 -> kein Ereignis");
    re15_game_flag_set(re15_em_status_zone(), RE15_R39R_KILL, 0);
    if (re15_em_status_zone() != 7) return fail(4, "Stage 1 -> Zone 7 (@0x800420ec)");
    /* Leon ROOM1120 bleibt bei Runde 35: unser Installer tut dort nichts, das Ereignis 30 gibt es dort nicht */
    g_current_room_id = 0x1120;
    re15_hund1121_r39_install(0x1120);
    if (re15_hund1121_r39_phase() != RE15_R39R_AUS || re15_hund1121_r39_ereignis(0x1120, RE15_R39R_EREIGNIS))
        return fail(5, "ROOM1120 (Leon): kein Hund");
    g_current_room_id = RE15_R39R_RAUM;

    re15_hund1121_r39_install(RE15_R39R_RAUM);
    const re15_aot_t *a = &g_aot.slots[RE15_R39R_SLOT];
    if (re15_hund1121_r39_phase() != RE15_R39R_SCHARF) return fail(6, "scharf");
    if (!a->active || a->type != RE15_AOT_TYPE_AUTO_EVENT || a->event_id != RE15_R39R_EREIGNIS ||
        a->sce_flags != RE15_FENSTER_SAT)
        return fail(7, "Slot 4 = AUTO-Ereignis 30, sat 0x41");
    if (a->x - a->half_w != RE15_FENSTER_ZONE_X0 || a->x + a->half_w != RE15_FENSTER_ZONE_X1 ||
        a->z - a->half_h != RE15_FENSTER_ZONE_Z0 || a->z + a->half_h != RE15_FENSTER_ZONE_Z1)
        return fail(8, "Band = Leons Band x 3500..6650 z 4300..6400");
    for (int r = 0; r < RE15_R39R_WAND_REGIONEN; r++)
        if (wand_u0(r) != 0xFF) return fail(9, "Band-1-Rueckwand vor dem Ereignis fest (u0 0xFF, ROOM1121.RDT @0x003D1 ..)");
    /* Programm-Bytes: Satzformen ROOM1130 @0x00AC8 `37 0r 0a 00`, ROOM1120 @0x00D0C `44 ..`, `22 07 ae 01`, `46 04 ..` */
    int n = 0;
    const uint8_t *p = re15_hund1121_r39_programm(&n);
    printf("Programm %d B:", n);
    for (int i = 0; i < n; i++) printf(" %02x", p[i]);
    printf("\n");
    static const uint8_t k_soll[56] = {
        0x37,0,15,0, 0x37,1,15,0, 0x37,2,15,0, 0x37,3,15,0, 0x37,4,15,0,
        0x44,4,0x20,0x00,0,0,0,0xAE, 0x18,0x15, 0xDD,0xF8, 0x44,0x2F, 0,0, 0x18,0x04, 0,0,
        0x22,7,0xAE,1,
        0x46,4,0,0,0,0,0,0,0,0,
        0x01,0x00 };
    if (n != 56 || memcmp(p, k_soll, 56) != 0) return fail(10, "Programm-Bytes");

    if (ausloesen() != 0) return fail(11, "Ausloeser ueber den AOT-Scan");
    for (int r = 0; r < RE15_R39R_WAND_REGIONEN; r++)
        if (wand_u0(r) != 0x00) return fail(12, "Sca_id_set gibt die Band-1-Rueckwand frei (LAB_8004175c: u0 := 0)");
    const int hs = RE15_R39R_EM + 1;
    re15_actor_t *h = &g_actors[hs];
    printf("Hund Slot %d: aktiv %d Typ 0x%02x grid 0x%02x Lage (%d,%d,%d) rot 0x%x Kill 0x%02x\n", hs, h->active, h->type,
           h->grid_id, (int)h->x, (int)h->y, (int)h->z, (unsigned)h->rot_y, h->em_flag_id);
    if (!h->active || h->type != 0x20 || h->grid_id != 0 || h->x != RE15_R39R_X || h->y != RE15_R39R_Y ||
        h->z != RE15_R39R_Z || h->rot_y != RE15_R39R_DIR || h->em_flag_id != RE15_R39R_KILL)
        return fail(13, "Hund (5400,-1827,12100) dir 0x418, Verhalten 0, Kill 0xAE");
    if (!re15_game_flag_get(re15_em_status_zone(), RE15_R39R_KILL)) return fail(14, "(7,0xAE) = 1 nach dem Spawn");
    if (g_aot.slots[RE15_R39R_SLOT].type != RE15_AOT_TYPE_NONE) return fail(15, "Aot_reset(4) -> sce 0");
    if (re15_hund1121_r39_ereignis(RE15_R39R_RAUM, RE15_R39R_EREIGNIS)) return fail(16, "zweites Programm");
    printf("OK ereignis\n");
    return 0;
}

/* ============================================================================================ */
typedef struct { int takt, phase; int32_t x, y, z; int st, s1, s2, s3, mo, spd; int se0, se1; int schatten; } zeile_t;

static int pruef_sprung(void)
{
    grund(RE15_AI_FLAVOR_RE2);
    if (raum_laden(RE15_R39R_RAUM) != 0) return fail(20, "ROOM1121.RDT");
    if (esp_registrieren() != 0) return fail(21, "RE2-ESP CORE00 + GLAS1090");
    if (hundebank_laden() != 0) return fail(22, "RE2-Hundebank EM020 aus CDEMD0.EMS");
    re15_game_flag_set(RE15_R39R_PANIK_BANK, RE15_R39R_PANIK_BIT, 1);
    if (ausloesen() != 0) return fail(23, "Ausloeser");
    re15_actor_t *pl = spieler(4986, 2000);              /* Elza ist weitergelaufen (Messlauf: z 6269 -> ~1600) */
    (void)pl;
    const int hs = RE15_R39R_EM + 1;
    re15_actor_t *h = &g_actors[hs];
    static zeile_t z[160];
    int nz = 0;
    for (int f = 0; f < 160; f++) {
        const int se_vor = s_se_n;
        const int schatten = re15_hund1121_r39_schatten_aus(h);
        bild();
        zeile_t *q = &z[nz++];
        q->takt = re15_hund1121_r39_takt(); q->phase = re15_hund1121_r39_phase();
        q->x = h->x; q->y = h->y; q->z = h->z; q->st = h->state; q->s1 = h->sub_state_1; q->s2 = h->sub_state_2;
        q->s3 = h->sub_state_3; q->mo = h->motion; q->spd = h->speed_h; q->schatten = schatten;
        q->se0 = (s_se_n > se_vor) ? s_se[se_vor] : -1; q->se1 = (s_se_n > se_vor + 1) ? s_se[se_vor + 1] : -1;
        if (q->phase == RE15_R39R_FERTIG) break;
    }
    for (int i = 0; i < nz; i++)
        printf("T%3d ph%d (%5d,%6d,%6d) st%d/%2d/%d/%d clip%2d v%3d SE %d %d%s\n", z[i].takt, z[i].phase, (int)z[i].x,
               (int)z[i].y, (int)z[i].z, z[i].st, z[i].s1, z[i].s2, z[i].s3, z[i].mo, z[i].spd, z[i].se0, z[i].se1,
               z[i].schatten ? " schatten-aus" : "");
    /* (a) Spawn-Bild: INIT; T1: Sub 14 Phase 0 (ROOM21A0 sub07 @0x020FE..0x0210A); keine Bewegung, kein Wandschub. */
    if (z[0].takt != 0 || z[0].st != 1) return fail(24, "T0: INIT -> ACTIVE (sw 1,4 @0x8010012C)");
    if (z[1].takt != 1 || z[1].s1 != RE15_R39R_RE2_SUB || z[1].s3 != 0) return fail(25, "T1: Sub 14 Phase 0");
    for (int i = 0; i <= 1; i++)
        if (z[i].x != RE15_R39R_X || z[i].z != RE15_R39R_Z || z[i].y != RE15_R39R_Y) return fail(26, "Warten: Lage unveraendert (Wand frei)");
    /* (b) T2 = Freigabe (@0x02122 +7 = 1): Phase 1 Start + Rumpf im selben Bild: Clip 20, Speed 65, ein Schritt. */
    if (z[2].takt != 2 || z[2].s3 != 2 || z[2].mo != 20 || z[2].spd != 65) return fail(27, "T2: Phase 1 -> 2, Clip 20, 65");
    const int32_t dz1 = RE15_R39R_Z - z[2].z;
    if (dz1 < 60 || dz1 > 65) return fail(28, "T2: Anlauf-Schritt im Freigabe-Bild (@0x80102D20 -> @0x80102D24)");
    /* (c) Absprung: 8 Anlaufbilder (+0x158 = 8 @0x80101398) -> T9 sub_state_2 = 1, Speed 280, SE 3. */
    int t_ab = -1;
    for (int i = 0; i < nz; i++) if (z[i].se0 == 3 || z[i].se1 == 3) { t_ab = z[i].takt; break; }
    printf("Absprung T%d\n", t_ab);
    if (t_ab != 9) return fail(29, "Absprung T9 = 8 Anlaufbilder ab T2, SE 3 (@0x80102D74)");
    if (z[9].s2 != 1 || z[9].spd != 280) return fail(30, "T9: Flug, Speed 280 (@0x80102D68)");
    if (RE15_R39R_Z - z[9].z < 8 * 60 || RE15_R39R_Z - z[9].z > 8 * 65) return fail(31, "Anlaufweg 8 x 65");
    /* (d) Bogen gegen eine UNABHAENGIGE Rechnung: vy -320, +40 je Bild (@0x801014B8-C8), Ende bei vy >= 0. Der Flug
     *     beginnt im Bild nach dem Absprung (T10); der Scheitel ist y0 - (320+280+...+40) = y0 - 1440. */
    {
        int32_t y = RE15_R39R_Y, vy = -320;
        int i = 10, n_flug = 0;
        for (; i < nz && z[i].s2 == 1; i++, n_flug++) {
            y += vy; vy += 40;
            if (z[i].y != y) { printf("Bild T%d y %d soll %d\n", z[i].takt, (int)z[i].y, (int)y); return fail(32, "Steigflug"); }
        }
        int scheitel = 0;
        for (int k = 0; k < nz; k++) if (z[k].y < scheitel) scheitel = z[k].y;
        printf("Steigflug %d Bilder geprueft, Scheitel y %d (soll %d)\n", n_flug, scheitel, RE15_R39R_Y - 1440);
        if (n_flug != 7 || scheitel != RE15_R39R_Y - 1440) return fail(33, "Scheitel = y0 - (320+280+...+40) nach 8 Steigbildern");
    }
    int t21 = -1;
    for (int i = 0; i < nz; i++) if (z[i].mo == 21) { t21 = z[i].takt; break; }
    if (t21 < 0) return fail(34, "Landeclip 21 (0xF0015 @0x80102DC8)");
    /* (e) Glas im Bild des Durchgangs: erstes Bild, das den Hund (vor der KI) diesseits von z 11200 sieht. */
    int t_glas = re15_hund1121_r39_glas_takt(), t_durch = -1;
    for (int i = 0; i < nz; i++) if (z[i].z <= RE15_R39R_GLAS_Z) { t_durch = z[i].takt + 1; break; }
    printf("Glas T%d (Durchgang nach T%d), Splitter %d, Knalle %d Satz 0x%02x\n", t_glas, t_durch - 1,
           re15_hund1121_r39_splitter(), s_knall_n, s_knall_satz);
    if (t_glas != t_durch) return fail(35, "Glas im Bild nach dem Durchgang durch die Glasebene");
    if (re15_hund1121_r39_splitter() != RE15_FENSTER_SPLITTER_N) return fail(36, "13 Splitter (sub15 @0x0054..0x01D4)");
    if (s_knall_n != 2 || s_knall_satz != RE15_FENSTER_SE_SATZ) return fail(37, "2 Knalle Satz 0x21");
    for (int i = 0; i < nz; i++) {
        const int sollschatten = (i == 0) ? 0 : (z[i - 1].z > RE15_R39R_GLAS_Z);
        if (z[i].schatten != sollschatten) return fail(38, "Schatten aus genau jenseits der Glasebene");
    }
    for (int r = 0; r < RE15_R39R_WAND_REGIONEN; r++)
        if (wand_u0(r) != 0xFF) return fail(39, "Band-1-Rueckwand nach dem Durchsprung wieder fest");
    /* (f) Landung y 0 mit SE 10 (@0x801015FC), danach Sub 2 = RUN (@0x80102E2C). */
    int t_land = -1;
    for (int i = 1; i < nz; i++) if (z[i].y == 0 && z[i - 1].y < 0) { t_land = i; break; }
    if (t_land < 0 || !(z[t_land].se0 == 10 || z[t_land].se1 == 10)) return fail(40, "Landung y 0 mit SE 10");
    if (re15_hund1121_r39_phase() != RE15_R39R_FERTIG || h->state != 1 || h->sub_state_1 != 2)
        return fail(41, "danach normaler Gegner: Zustand 1 Sub 2 (RUN)");
    printf("Landung T%d bei (%d,%d), Ende Sub %d\n", z[t_land].takt, (int)z[t_land].x, (int)z[t_land].z, h->sub_state_1);
    printf("OK sprung\n");
    return 0;
}

/* ============================================================================================ */
static int pruef_re15(void)
{
    grund(RE15_AI_FLAVOR_RE15);
    if (raum_laden(RE15_R39R_RAUM) != 0) return fail(50, "ROOM1121.RDT");
    if (esp_registrieren() != 0) return fail(51, "RE2-ESP");
    re15_game_flag_set(RE15_R39R_PANIK_BANK, RE15_R39R_PANIK_BIT, 1);
    if (ausloesen() != 0) return fail(52, "Ausloeser");
    const int hs = RE15_R39R_EM + 1;
    re15_actor_t *h = &g_actors[hs];
    if (h->grid_id != RE15_R39R_RE15_GRID) return fail(53, "RE1.5-KI: Spawn mit grid 0x40");
    spieler(4986, 2000);
    int war4 = 0, los = -1, raus = -1;
    for (int f = 0; f < 200 && re15_hund1121_r39_phase() != RE15_R39R_FERTIG; f++) {
        bild();
        if (h->state == 4) war4 = 1;
        if (los < 0 && h->grid_id == RE15_R39R_RE15_LOS) los = re15_hund1121_r39_takt();
        if (raus < 0 && war4 && h->state == 1) raus = re15_hund1121_r39_takt();
    }
    printf("RE1.5: Zustand 4 %d, Freigabe T%d, Ausgang T%d, Glas T%d, Knalle %d, Lage (%d,%d,%d) Zustand %d Sub %d\n",
           war4, los, raus, re15_hund1121_r39_glas_takt(), s_knall_n, (int)h->x, (int)h->y, (int)h->z, h->state,
           h->sub_state_1);
    if (!war4) return fail(54, "INIT @0x8010db88-98: grid 0x40 -> Zustand 4 (wartet)");
    if (los != 2) return fail(55, "Freigabe grid 0x43 in T2 (Evt_next nach dem Warten)");
    if (raus < 0 || h->y != 0) return fail(56, "Sprung, Landung y 0, Ausgang Zustand 1 (Wort 0x201 @0x8011162c)");
    if (re15_hund1121_r39_glas_takt() < 0 || s_knall_n != 2) return fail(57, "Glas + 2 Knalle auch im RE1.5-Modus");
    printf("OK re15\n");
    return 0;
}

/* ============================================================================================ */
static int pruef_einmal(void)
{
    grund(RE15_AI_FLAVOR_RE2);
    if (raum_laden(RE15_R39R_RAUM) != 0) return fail(60, "ROOM1121.RDT");
    if (hundebank_laden() != 0) return fail(61, "Hundebank");
    re15_game_flag_set(RE15_R39R_PANIK_BANK, RE15_R39R_PANIK_BIT, 1);
    if (ausloesen() != 0) return fail(62, "Ausloeser");
    for (int f = 0; f < 120; f++) bild();
    /* Wiedereintritt: Raum neu (SCA frisch), Gegner weg wie beim Raumwechsel. */
    re15_actor_init(); re15_aot_init();
    if (raum_laden(RE15_R39R_RAUM) != 0) return fail(63, "ROOM1121.RDT neu");
    re15_hund1121_r39_install(RE15_R39R_RAUM);
    if (re15_hund1121_r39_phase() != RE15_R39R_AUS || g_aot.slots[RE15_R39R_SLOT].active) return fail(64, "Wiedereintritt: kein zweites Ereignis");
    /* Rettung (Spur P loescht 97): kein Ereignis, auch wenn 0xAE noch 0 waere. */
    re15_game_flag_set(re15_em_status_zone(), RE15_R39R_KILL, 0);
    re15_game_flag_set(RE15_R39R_PANIK_BANK, RE15_R39R_PANIK_BIT, 0);
    re15_hund1121_r39_install(RE15_R39R_RAUM);
    if (re15_hund1121_r39_phase() != RE15_R39R_AUS) return fail(65, "ohne (9,97) kein Ereignis");
    printf("OK einmal\n");
    return 0;
}

/* ============================================================================================ */
/* Treffer auf der Bruestung (vor dem Absprung): der RE2-Applier schreibt das ganze Wort +4..+7 = 2/0/0/0 und dann +5 =
 * Waffen-Zeile (`sw v0,4(s1)` @0x80047288, `sb s5,5(s1)` @0x80047324). Erwartet: Luft-Bit (Port-Wahl, Header des Takts),
 * RE2-Luft-Treffer P1 (@0x801034C8) bringt den Hund auf den Boden, kein Glasbruch, Wand wieder fest. */
static int pruef_treffer(void)
{
    grund(RE15_AI_FLAVOR_RE2);
    if (raum_laden(RE15_R39R_RAUM) != 0) return fail(80, "ROOM1121.RDT");
    if (esp_registrieren() != 0) return fail(81, "RE2-ESP");
    if (hundebank_laden() != 0) return fail(82, "Hundebank");
    re15_game_flag_set(RE15_R39R_PANIK_BANK, RE15_R39R_PANIK_BIT, 1);
    if (ausloesen() != 0) return fail(83, "Ausloeser");
    spieler(4986, 4000);
    const int hs = RE15_R39R_EM + 1;
    re15_actor_t *h = &g_actors[hs];
    for (int f = 0; f < 5; f++) bild();                 /* T0..T4: Freigabe in T2, Anlauf laeuft */
    if (h->sub_state_1 != 14 || h->sub_state_2 != 0 || h->y != RE15_R39R_Y) return fail(84, "Anlauf auf der Bruestung");
    h->state = 2; h->sub_state_1 = 2; h->sub_state_2 = 0; h->sub_state_3 = 0; h->hp -= 10;
    int boden = -1;
    for (int f = 0; f < 120 && boden < 0; f++) { bild(); if (h->y == h->dog_floor_y) boden = f; }
    printf("Treffer in T4: Boden nach %d Bildern bei (%d,%d,%d), Zustand %d Sub %d, Phase %d, Glas %d, Knalle %d\n", boden,
           (int)h->x, (int)h->y, (int)h->z, h->state, h->sub_state_1, re15_hund1121_r39_phase(),
           re15_hund1121_r39_glas_takt(), s_knall_n);
    if (boden < 0) return fail(85, "Hund faellt nach dem Treffer auf seinen Boden (RE2 HURT P1 @0x801034C8)");
    if (re15_hund1121_r39_phase() != RE15_R39R_FERTIG || re15_hund1121_r39_glas_takt() >= 0 || s_knall_n != 0)
        return fail(86, "Sprung abgebrochen: kein Glas, kein Knall");
    for (int r = 0; r < RE15_R39R_WAND_REGIONEN; r++)
        if (wand_u0(r) != 0xFF) return fail(87, "Band-1-Rueckwand wieder fest");
    printf("OK treffer\n");
    return 0;
}

/* ============================================================================================ */
static int pruef_startmaschine(void)
{
    grund(RE15_AI_FLAVOR_RE2);
    if (hundebank_laden() != 0) return fail(70, "Hundebank");
    g_current_room_id = RE15_R39R_RAUM;
    const int hs = 6;
    re15_actor_t *h = &g_actors[hs];
    h->active = 1; h->type = 0x20; h->state = 0; h->x = 0; h->y = -1800; h->z = 0; h->rot_y = 0x400;
    re15_re2dog_tick(hs);                               /* INIT */
    h->state = 1; h->sub_state_1 = 14; h->sub_state_2 = 0; h->sub_state_3 = 0;   /* ROOM21A0 sub07 @0x020FE..0x0210A */
    for (int f = 0; f < 60; f++) re15_re2dog_tick(hs);
    if (h->sub_state_3 != 0 || h->x != 0 || h->z != 0) return fail(71, "Phase 0 wartet (kein +7-Schreiber @0x80102CD4-F4)");
    if (!(h->re2z_self1d3 & 0x80u)) return fail(72, "+0x1D3 |= 0x80 @0x80102CE4-EC");
    h->sub_state_3 = 1;                                 /* @0x02122 */
    re15_re2dog_tick(hs);
    printf("Freigabe-Bild: +7 %d Clip %d Speed %d +0x158 %d z %d\n", h->sub_state_3, h->motion, h->speed_h,
           (int)h->re2z_t158, (int)h->z);
    if (h->sub_state_3 != 2 || h->motion != 20 || h->speed_h != 65) return fail(73, "0x80101380(65,-320): Clip 20, +7 = 2");
    if (h->re2z_t158 != 7 || h->z != -65) return fail(74, "Phase 1 faellt in den Rumpf: Schritt 65 und +0x158 8 -> 7 im selben Bild");
    int n = 0;
    s_se_n = 0;
    int se_vor = 0;
    while (h->sub_state_2 == 0 && n < 20) { se_vor = s_se_n; re15_re2dog_tick(hs); n++; }
    /* SE 3 im Absprung-Bild (@0x80102D74); davor darf der Clip-20-Bild-Ton SE 8 kommen (ACTIVE-Schwanz @0x80100510-30). */
    int se3 = 0;
    for (int k = se_vor; k < s_se_n; k++) if (s_se[k] == 3) se3 = 1;
    printf("Absprung nach %d weiteren Bildern, Speed %d, SE im Absprung-Bild %s\n", n, h->speed_h, se3 ? "3" : "-");
    if (n != 7 || h->speed_h != 280 || !se3) return fail(75, "Absprung nach weiteren 7 Bildern (8 Anlauf), Speed 280, SE 3");
    printf("OK startmaschine\n");
    return 0;
}

/* ============================================================================================ */
static int werkzeug_karte(int argc, char **argv)
{
    if (argc < 7) { printf("Aufruf: karte <datei> <raum-hex> <x> <z> <gier> [bank:bit ...]\n"); return 2; }
    unsigned room = (unsigned)strtoul(argv[3], NULL, 16);
    scd_vm_init(); re15_actor_init(); re15_aot_init(); re15_game_state_init();
    g_current_room_id = room;
    g_gameflow.character = (room & 1u) ? 4 : 0;         /* Variante = niedrigste Hex-Ziffer (@0x800397ec) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = atoi(argv[4]); pl->z = atoi(argv[5]); pl->rot_y = (int16_t)atoi(argv[6]); pl->y = 0;
    for (int a = 7; a < argc; a++) {
        char *c = strchr(argv[a], ':');
        if (!c) { printf("FAIL: '%s'\n", argv[a]); return 2; }
        re15_game_flag_set((uint8_t)strtol(argv[a], NULL, 0), (uint8_t)strtol(c + 1, NULL, 0), 1);
    }
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(argv[2], 0, &sd, "R39 HUND    ") != 0) { printf("FAIL: Karte\n"); return 1; }
    printf("Karte %s: Raum 0x%04X Figur %d Lage (%d,%d)\n", argv[2], room, (int)g_gameflow.character, (int)pl->x, (int)pl->z);
    return 0;
}

static int cmp_int(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

static int werkzeug_mitschnitt(int argc, char **argv)
{
    if (argc < 6) return 2;
    long sz = 0;
    uint8_t *raw = datei(argv[2], &sz);
    if (!raw) return 1;
    const int16_t *s = (const int16_t *)(const void *)raw;
    const long T = 1470, ticks = sz / 4 / T;           /* 44100/30 Stereo-Frames je Spielbild (audio_pc.c) */
    const long off = ticks - 1 - atol(argv[3]);
    if (off < 0) { printf("FEHLER: Mitschnitt kuerzer als der Raum\n"); return 1; }
    int *pk = (int *)malloc(sizeof(int) * (size_t)ticks);
    for (long t = 0; t < ticks; t++) {
        int m = 0;
        for (long i = 0; i < T; i++) {
            int v = (s[(t * T + i) * 2] + s[(t * T + i) * 2 + 1]) / 2;
            if (v < 0) v = -v;
            if (v > m) m = v;
        }
        pk[t] = m;
    }
    const int nr = (int)(ticks - off);
    int *srt = (int *)malloc(sizeof(int) * (size_t)nr);
    memcpy(srt, pk + off, sizeof(int) * (size_t)nr);
    qsort(srt, (size_t)nr, sizeof(int), cmp_int);
    const int median = srt[nr / 2];
    for (int a = 4; a + 1 < argc; a += 2) {
        const int b0 = atoi(argv[a]), l = atoi(argv[a + 1]);
        int mx = 0, at = -1;
        for (int k = 0; k < l; k++) { long t = off + b0 + k; if (t >= 0 && t < ticks && pk[t] > mx) { mx = pk[t]; at = b0 + k; } }
        printf("fenster %d..%d spitze %d bei %d median %d\n", b0, b0 + l - 1, mx, at, median);
    }
    return 0;
}

static int werkzeug_bild(int argc, char **argv)
{
    if (argc < 8) return 2;
    long na = 0, nb = 0;
    uint8_t *a = datei(argv[2], &na), *b = datei(argv[3], &nb);
    if (!a || !b || na != nb || na < 15 + 320 * 240 * 3) { printf("FEHLER: Bilder\n"); return 1; }
    const int x0 = atoi(argv[4]), y0 = atoi(argv[5]), x1 = atoi(argv[6]), y1 = atoi(argv[7]);
    int versch = 0, dunkler = 0;
    for (int y = y0; y <= y1; y++)
        for (int x = x0; x <= x1; x++) {
            const uint8_t *pa = a + 15 + (y * 320 + x) * 3, *pb = b + 15 + (y * 320 + x) * 3;
            const int d = abs(pa[0] - pb[0]) + abs(pa[1] - pb[1]) + abs(pa[2] - pb[2]);
            if (d > 30) versch++;
        }
    (void)dunkler;
    printf("rechteck %d,%d..%d,%d verschieden %d\n", x0, y0, x1, y1, versch);
    return 0;
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "ereignis"))      return pruef_ereignis();
    if (!strcmp(t, "sprung"))        return pruef_sprung();
    if (!strcmp(t, "re15"))          return pruef_re15();
    if (!strcmp(t, "einmal"))        return pruef_einmal();
    if (!strcmp(t, "startmaschine")) return pruef_startmaschine();
    if (!strcmp(t, "treffer"))       return pruef_treffer();
    if (!strcmp(t, "karte"))         return werkzeug_karte(argc, argv);
    if (!strcmp(t, "mitschnitt"))    return werkzeug_mitschnitt(argc, argv);
    if (!strcmp(t, "bild"))          return werkzeug_bild(argc, argv);
    printf("Aufruf: ereignis | sprung | re15 | einmal | treffer | startmaschine | karte ... | mitschnitt ... | bild ...\n");
    return 2;
}
