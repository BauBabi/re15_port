/*
 * Runde 41 Spur J "stage3szenen_a": Riegel der drei Mechaniken hinter den Nutzer-Punkten
 *   (1) "die Animation von Marvin, am Ende seine Abschnitten in STAGE3 ... ist falsch in ROOM 3060"
 *   (2) "in ROOM 3061 in der cutscene klappt das rein krabbeln von Sherry in den Schacht nicht. Am Ende von dieser
 *       Cutscene hat Elza auch erneut eine falsche Animation."
 * Dossier analysis/befunde_runde41/J_stage3szenen_a.md, Belege include/re15_wclip_r41.h, re15_fussanker_r41.h,
 * re15_superobj_r41.h.
 *
 *   zensus    alle 240 RDTs (Work_set-Verfolgung je Sub): Spieler-Plc_motion(2,c) NUR ROOM3060 @0x010BE und ROOM3061
 *             @0x011DE (beide c = 6); Plc_flg mit Bit 1/2 NUR ROOM3061 sub04 (5 Stellen); Super_set NUR ROOM3061 @0x0127E
 *             und ROOM4030/4031 — der Umfang, fuer den die neuen Wege gelten; kein anderer Schreiber von +0x1c4
 *             (Member_set 0x34, Plc_motion pc[3], Member_set2 0x35 der Kriechtor-Maschine) setzt Bit 1/2
 *   anker     Glied-Anker FUN_800369f8 an Sherrys echter Bank (CDEMD1 Typ 0x4B + ROOM3061 RBJ rec1): Kriech-Takte aus sub04
 *             (SET 4/6/5/4) ab einem Original-Stand, Ziel = der naechste Original-Stand der dichten Reihe o3061_s3 —
 *             bytegleich (Abweichung 0)
 *   superobj  Super_set/Super_reset am echten Opcode-Bytestrom (ROOM3061 @0x0127E / @0x0129E): Gitter an Sherrys Teil 14,
 *             Loslass-Lage gegen die Original-Lage (-18756,-266,-26788); Vater ausserhalb State 4 -> uebersprungen
 *   wclip     Plc_motion(2,c) des Spielers -> W-Bank-Clip c, Halten auf dem letzten Bild (@0x80050fec), Ende bei anderem
 *             Befehl und beim Raumwechsel; Sub 0/1 und ROOM1141 (Runde 39) unberuehrt
 */
#include "re15_wclip_r41.h"
#include "re15_fussanker_r41.h"
#include "re15_superobj_r41.h"
#include "re15_dining_r39.h"
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_room_list.h"
#include "re15_roy_r37.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);   /* player_common.c */

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
                              else { printf("  ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *datei(const char *pfad, long *n)
{
    FILE *f = fopen(pfad, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)*n + 1);
    if (b && fread(b, 1, (size_t)*n, f) != (size_t)*n) { free(b); b = NULL; }
    fclose(f); return b;
}
static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static uint16_t le16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static const char *stage_dir(unsigned room)
{
    static char b[16]; snprintf(b, sizeof b, "STAGE%u", (room >> 12) & 0xF); return b;
}

/* ---------- zensus ---------- */
typedef struct { unsigned room; long off; int wert; } fund_t;
/* Andere Schreiber von +0x1c4 (Member 16, actor_common.c re15_actor_set_member case 16): Member_set 0x34 mit Bit 1/2,
 * Plc_motion pc[3] (@0x80041bc8 `sh a1,452(v0)` = SET), Member_set2 0x35 aus einer Variable (Calc 0x26 op = pc[2],
 * dst = pc[3], imm = pc[4..5]; Member_get 0x3D var = pc[1] <- Member pc[2], scd_vm.c op_member_get @0x80041238). */
static int s_ms_10, s_pm_flags, s_ms2_10, s_ms2_10_unsicher;
static int16_t les16(const uint8_t *p) { return (int16_t)le16(p); }
/* Sprungziel vorwaerts wie tools/scd_dump_room.py fwd_target (Ifel 0x06, Else 0x07, For/For2 0x0D/0x1B, While/Do/Switch
 * 0x0F/0x11/0x13, 0x14, 0x17) — ein Evt_end beendet den Sub nur, wenn kein Sprung dahinter zeigt. */
static long vorwaerts(const uint8_t *q)
{
    switch (q[0]) {
    case 0x06: return 4 + le16(q + 2);
    case 0x07: return les16(q + 2);
    case 0x0D: case 0x1B: return 6 + les16(q + 2);
    case 0x0F: case 0x11: case 0x13: return 4 + les16(q + 2);
    case 0x14: return 6 + le16(q + 2);
    case 0x17: { int o = les16(q + 4); return o > 0 ? o : 0; }
    default: return 0;
    }
}
static void zensus_sub(unsigned room, const re15_rdt_t *rdt, const uint8_t *p, long grenze,
                       fund_t *pm, int *npm, fund_t *fl, int *nfl, fund_t *ss, int *nss)
{
    int ws_kind = -1;
    long maxf = 0;
    uint8_t vstat[256];                       /* 1 = aus Member 16 geladen und nur bit-erhaltend veraendert, 2 = unsicher */
    memset(vstat, 0, sizeof vstat);
    for (long o = 0; o < grenze; ) {
        const uint8_t *q = p + o;
        if (q[0] >= 0x5F || q[0] == 0x1F) break;                                         /* unbekannt -> Ende */
        int sz = scd_opcode_size_at(q);
        if (sz <= 0 || o + sz > grenze) break;
        long t = vorwaerts(q);
        if (t > 0 && o + t > maxf) maxf = o + t;
        long datei_off = (long)(q - rdt->raw);
        if (q[0] == 0x2E) ws_kind = q[1];                                                /* Work_set kind */
        if (q[0] == 0x3F && q[1] == 2 && ws_kind == 1 && *npm < 16) {                    /* Plc_motion(2,c) Spieler */
            pm[*npm].room = room; pm[*npm].off = datei_off; pm[*npm].wert = q[2]; (*npm)++; }
        if (q[0] == 0x43 && (le16(q + 2) & 3u) && *nfl < 16) {                          /* Plc_flg Bit 1/2 */
            fl[*nfl].room = room; fl[*nfl].off = datei_off; fl[*nfl].wert = le16(q + 2); (*nfl)++; }
        if (q[0] == 0x48 && *nss < 16) {                                                 /* Super_set */
            ss[*nss].room = room; ss[*nss].off = datei_off; ss[*nss].wert = q[2] | (q[3] << 8); (*nss)++; }
        if (q[0] == 0x34 && q[1] == 0x10 && (le16(q + 2) & 3u)) s_ms_10++;              /* Member_set(16, Bit 1/2) */
        if (q[0] == 0x3F && (q[3] & 3u)) s_pm_flags++;                                   /* Plc_motion pc[3] Bit 1/2 */
        if (q[0] == 0x3D) vstat[q[1]] = (uint8_t)(q[2] == 0x10 ? 1 : 0);                 /* Member_get var <- Member */
        if (q[0] == 0x24) vstat[q[1]] = 0;                                                /* Save var = Wert */
        if (q[0] == 0x25) vstat[q[1]] = vstat[q[2]];                                      /* Copy */
        if (q[0] == 0x26 && vstat[q[3]] == 1 && !(q[2] == 6 || (q[2] == 5 && !(le16(q + 4) & 3u))))
            vstat[q[3]] = 2;                                                              /* nur AND / OR ohne Bit 1/2 */
        if (q[0] == 0x27 && vstat[q[2]] == 1) vstat[q[2]] = 2;                            /* Calc2 */
        if (q[0] == 0x35 && q[1] == 0x10) { s_ms2_10++; if (vstat[q[2]] != 1) s_ms2_10_unsicher++; }
        if (q[0] == 0x01 && o + 2 > maxf) break;                                         /* Evt_end ohne Sprung dahinter */
        o += sz;
    }
}
static void t_zensus(void)
{
    fund_t pm[16], fl[16], ss[16]; int npm = 0, nfl = 0, nss = 0, n_raum = 0, n_stub = 0;
    for (int i = 0; i < RE15_ROOM_COUNT; i++) {
        unsigned room = re15_room_ids[i];
        char p[600]; snprintf(p, sizeof p, "%s/%s/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, stage_dir(room), room);
        long n = 0; uint8_t *b = datei(p, &n);
        if (!b) { printf("        %s nicht lesbar\n", p); continue; }
        if (n < 0x60) { n_stub++; free(b); continue; }                 /* 4-Byte-Platzhalter ohne SCD (z.B. ROOM1270, 20C0) */
        re15_rdt_t rdt;
        int prc = re15_rdt_parse(b, (size_t)n, &rdt);
        if (prc < 0) { printf("        ROOM%04X: re15_rdt_parse %d\n", room, prc); free(b); continue; }
        n_raum++;
        if (rdt.main_scd) zensus_sub(room, &rdt, rdt.main_scd, (long)rdt.main_scd_size, pm, &npm, fl, &nfl, ss, &nss);
        for (int s = 0; s < rdt.sub_scd_count && s < RE15_RDT_MAX_SUB_SCD; s++) {
            const uint8_t *q = rdt.sub_scd[s];
            if (!q) continue;
            long grenze = 4096;                                   /* bis zum naechsten Sub (Muster probe_besitz_zensus.c) */
            for (int s2 = 0; s2 < rdt.sub_scd_count && s2 < RE15_RDT_MAX_SUB_SCD; s2++)
                if (rdt.sub_scd[s2] > q && rdt.sub_scd[s2] - q < grenze) grenze = (long)(rdt.sub_scd[s2] - q);
            if ((long)(rdt.raw + rdt.raw_size - q) < grenze) grenze = (long)(rdt.raw + rdt.raw_size - q);
            zensus_sub(room, &rdt, q, grenze, pm, &npm, fl, &nfl, ss, &nss);
        }
        free(b);
    }
    CHECK(n_raum + n_stub == 240 && n_stub == 34, "%d Raeume mit SCD + %d Platzhalter (4 Bytes) = 240", n_raum, n_stub);
    CHECK(npm == 2 && pm[0].room == 0x3060 && pm[0].off == 0x010BE && pm[0].wert == 6 &&
          pm[1].room == 0x3061 && pm[1].off == 0x011DE && pm[1].wert == 6,
          "Spieler-Plc_motion(2,c): %d Stellen (Soll 2: ROOM3060 @0x010BE, ROOM3061 @0x011DE, c = 6)%s", npm,
          npm >= 1 ? "" : "");
    for (int i = 0; i < npm; i++) printf("        ROOM%04X @0x%05lX Clip %d\n", pm[i].room, pm[i].off, pm[i].wert);
    static const long soll_fl[5] = { 0x012EC, 0x012F4, 0x012FC, 0x01316, 0x0131E };
    int fl_ok = (nfl == 5);
    for (int i = 0; fl_ok && i < 5; i++) fl_ok = (fl[i].room == 0x3061 && fl[i].off == soll_fl[i]);
    CHECK(fl_ok, "Plc_flg mit Bit 1/2: %d Stellen (Soll 5, alle ROOM3061 sub04 @0x012EC/F4/FC/0x01316/1E)", nfl);
    for (int i = 0; i < nfl; i++) printf("        ROOM%04X @0x%05lX Wert 0x%04X\n", fl[i].room, fl[i].off, fl[i].wert);
    int ss3061 = 0, ss4030 = 0;
    for (int i = 0; i < nss; i++) {
        printf("        Super_set ROOM%04X @0x%05lX Art/Teil 0x%04X\n", ss[i].room, ss[i].off, ss[i].wert);
        if (ss[i].room == 0x3061 && ss[i].off == 0x0127E && ss[i].wert == 0x8E80) ss3061++;
        if ((ss[i].room == 0x4030 || ss[i].room == 0x4031) && ss[i].wert == 0x8180) ss4030++;
    }
    CHECK(nss == 3 && ss3061 == 1 && ss4030 == 2,
          "Super_set: %d Stellen (Soll 3: ROOM3061 @0x0127E Gegner 0 Teil 14, ROOM4030/4031 Gegner 0 Teil 1)", nss);
    /* Damit wirkt der Executor-Wechsel (enemy_ai_common.c @0x80050c6c/@0x80050c94) nur in ROOM3061 sub04. */
    CHECK(s_ms_10 == 0 && s_pm_flags == 0 && s_ms2_10 == 86 && s_ms2_10_unsicher == 0,
          "andere Schreiber von +0x1c4 Bit 1/2: Member_set(16) %d, Plc_motion pc[3] %d, Member_set2(16) %d Stellen, davon %d "
          "nicht bit-erhaltend (Soll 0/0/86/0: Kriechtor 1030/1031/1040/1041 = Member_get + AND 0x0FFF / OR 0x2000 / "
          "OR 0x1000)", s_ms_10, s_pm_flags, s_ms2_10, s_ms2_10_unsicher);
}

/* ---------- gemeinsame Sherry-Bank ---------- */
static uint8_t *s_ems, *s_rdt3061;
static long s_rdt3061_n;
static re15_emd_skeleton_t s_basis;
static re15_emd_animation_t s_basis_anim;
static re15_md1_t s_basis_md1;
static re15_tim_t s_basis_tim;
static const uint8_t *s_rbj; static size_t s_rbj_n;

static int sherry_laden(void)
{
    if (!s_ems) {
        char p[600]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(RE15_EMS_ARCHIV_ELZA));
        long n = 0; s_ems = datei(p, &n);
        if (!s_ems) { CHECK(0, "%s lesbar", p); return 0; }
        uint32_t sek = 0, len = 0;
        if (!re15_ems_toc(RE15_EMS_ARCHIV_ELZA, 0x4B, &sek, &len) || (long)sek * 2048 + (long)len > n) {
            CHECK(0, "CDEMD1-TOC Typ 0x4B"); return 0; }
        if (re15_emd_parse_container(s_ems + (size_t)sek * 2048u, len, &s_basis_md1, &s_basis, &s_basis_anim, &s_basis_tim) != 0) {
            CHECK(0, "Sherry-EMD (CDEMD1 Sektor %u, %u B) parsebar", sek, len); return 0; }
        printf("        Sherry Typ 0x4B: CDEMD1 Sektor %u, %u B, %d Knochen, Eltern:", sek, len, s_basis.bone_count);
        for (int b = 0; b < s_basis.bone_count; b++) printf(" %d", (int)s_basis.bone_parent[b]);
        printf("\n");
        snprintf(p, sizeof p, "%s/STAGE3/ROOM3061.RDT", RE15_ASSET_PSX_DIR);
        s_rdt3061 = datei(p, &s_rdt3061_n);
        if (!s_rdt3061) { CHECK(0, "ROOM3061.RDT lesbar"); return 0; }
        uint32_t s = le32(s_rdt3061 + 0x5C);
        s_rbj = s_rdt3061 + s; s_rbj_n = (size_t)(s_rdt3061_n - (long)s);
    }
    g_current_room_id = 0x3061;
    re15_enemy_reset();
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x4B);
    if (!eb) { CHECK(0, "Gegner-Bank frei"); return 0; }
    eb->skel = s_basis; eb->anim = s_basis_anim; eb->ok = 1;
    memset(g_actors, 0, sizeof(g_actors));
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x4B; e->state = 4; e->sub_state_1 = 0;
    re15_rbj_bind_room(s_rbj, s_rbj_n);
    const re15_emd_animation_t *an = re15_actor_rbj_anim(1);
    if (!an || an->clip_count != 3 || an->clips[0].frame_count != 90 || an->clips[1].frame_count != 75 ||
        an->clips[2].frame_count != 60) {
        CHECK(0, "ROOM3061 RBJ rec1 an Sherry gebunden (3 Clips 90/75/60)"); return 0; }
    return 1;
}

/* ---------- anker ---------- */
/* sub04 Clip-2-Takt je Bild der Runde: SET 4 (0..7) @0x0130E, SET 6 (8..33) @0x01316, SET 5 (34..57) @0x0131E, SET 4. */
static uint16_t takt(int f) { return (f < 8) ? 4 : (f < 34) ? 6 : (f < 58) ? 5 : 4; }

static void t_anker(void)
{
    if (!sherry_laden()) return;
    re15_actor_t *e = &g_actors[1];
    /* Original-Staende der dichten Reihe o3061_s3 (J_belege/o3061_s3_dicht.txt): Clip 2, Sherry r1067. Gezeichnetes Bild =
     * +0x95 - 1: FUN_8001f3bc setzt die Pose von +0x95 und zaehlt DANACH hoch (@0x8001f618/@0x8001f61c, Ende -> 0
     * @0x8001f63c); +0x168 zeigt auf den gezeichneten Eintrag (@0x8001f36c). t08 +0x95 19 -> Bild 18 usw. */
    static const struct { const char *name; int r0, f0, x0, z0, r1, f1, x1, z1; } fall[] = {
        { "t08 (Runde 1, +0x95 19 = Bild 18) -> t10 (Runde 2, +0x95 12 = Bild 11)", 0, 18, -18780, -28017, 1, 11, -18783, -28806 },
        { "t09 (Runde 1, +0x95 44 = Bild 43) -> t11 (Runde 2, +0x95 39 = Bild 38)", 0, 43, -18791, -28390, 1, 38, -18810, -29279 },
    };
    for (unsigned k = 0; k < sizeof fall / sizeof fall[0]; k++) {
        e->motion = 2; e->x = fall[k].x0; e->y = 0; e->z = fall[k].z0; e->rot_y = 1067; e->rot_x = 0; e->rot_z = 0;
        e->anim_frame = fall[k].f0; e->anim_flags = takt(fall[k].f0); e->walk_active = 0; e->anim_freeze = 0;
        re15_fussanker_r41_reset();
        re15_fussanker_r41_merken();                       /* Vorbild-Stand = Part 4/7-Welt des Startbilds */
        int32_t w0[3]; int alt_ok = re15_fussanker_r41_alt(1, 0, w0);
        int n_anker = 0, n_ruhe_bewegt = 0;
        for (int t = fall[k].r0 * 60 + fall[k].f0 + 1; t <= fall[k].r1 * 60 + fall[k].f1; t++) {
            int f = t % 60;
            e->anim_frame = f; e->anim_flags = takt(f);
            int32_t x0 = e->x, z0 = e->z;
            if (e->anim_flags & 1) { re15_fussanker_r41_anwenden(e, 0); n_anker++; }   /* @0x80050c6c/78 */
            if (e->anim_flags & 2) { re15_fussanker_r41_anwenden(e, 1); n_anker++; }   /* @0x80050c94/a0 */
            if (!(e->anim_flags & 3) && (e->x != x0 || e->z != z0)) n_ruhe_bewegt++;
            re15_fussanker_r41_merken();
        }
        int dx = e->x - fall[k].x1, dz = e->z - fall[k].z1;
        CHECK(alt_ok && n_ruhe_bewegt == 0 && dx == 0 && dz == 0,
              "Anker %s: Start (%d,%d), Port-Ende (%d,%d), Original (%d,%d), Abweichung (%d,%d) [Soll 0 = bytegleich], "
              "%d Anker-Bilder, %d SET-4-Bilder mit Bewegung (Soll 0)", fall[k].name, fall[k].x0, fall[k].z0, e->x, e->z,
              fall[k].x1, fall[k].z1, dx, dz, n_anker, n_ruhe_bewegt);
    }
    /* Grundeigenschaft Modus 0: Pose unveraendert -> keine Verschiebung (@0x80036ab4..aec x -= neu - alt). */
    e->motion = 2; e->anim_frame = 20; e->anim_flags = 6; e->x = -18800; e->z = -28500;
    re15_fussanker_r41_reset(); re15_fussanker_r41_merken();
    re15_fussanker_r41_anwenden(e, 1);
    CHECK(e->x == -18800 && e->z == -28500, "gleiche Pose, gleiche Lage -> keine Verschiebung (%d,%d)", e->x, e->z);
    /* Ohne gemerkten Vorbild-Stand (PORT-WAHL) keine Verschiebung. */
    re15_fussanker_r41_reset(); e->anim_frame = 21;
    re15_fussanker_r41_anwenden(e, 1);
    CHECK(e->x == -18800 && e->z == -28500, "ohne Vorbild-Stand keine Verschiebung");
    re15_rbj_bind_room(NULL, 0); re15_enemy_reset();
}

/* ---------- superobj ---------- */
static void t_superobj(void)
{
    if (!sherry_laden()) return;
    re15_actor_t *e = &g_actors[1];
    e->motion = 0; e->anim_frame = 33; e->anim_flags = 0; e->x = -18860; e->y = 0; e->z = -26266; e->rot_y = 1067;
    memset(&g_scd, 0, sizeof g_scd);
    g_scd.prop_count = 1;
    g_scd.props[0].active = 1; g_scd.props[0].obj_id = 0; g_scd.props[0].obj_type = 0; g_scd.props[0].parent_obj = -1;
    g_scd.props[0].x = -19095; g_scd.props[0].y = -495; g_scd.props[0].z = -27045;     /* Obj_model_set @0x00F80 */
    g_scd.props[0].rot_x = 0; g_scd.props[0].rot_y = 3072; g_scd.props[0].rot_z = 0;
    re15_superobj_r41_reset();
    const uint8_t *set = s_rdt3061 + 0x0127E, *rst = s_rdt3061 + 0x0129E;
    CHECK(set[0] == 0x48 && set[2] == 0x80 && set[3] == 0x8E && rst[0] == 0x49,
          "Bytes @0x0127E = Super_set 48 .. 80 8e, @0x0129E = Super_reset 49");
    scd_thread_t t; memset(&t, 0, sizeof t);
    t.work_slot = -1; t.work_prop_idx = 0; t.pc = set;
    int rc = re15_superobj_r41_op_set(&t);
    int32_t r[9], w[3];
    int hing = re15_superobj_r41_welt(0, r, w);
    CHECK(rc == 1 && t.pc == set + 16 && g_scd.props[0].x == 252 && g_scd.props[0].y == 192 && g_scd.props[0].z == -153 &&
          g_scd.props[0].rot_y == 1024 && g_scd.props[0].rot_z == 3072 && hing,
          "Super_set: lokal (%d,%d,%d) Rot (%d,%d,%d), angehaengt %d, Welt im Griff (%d,%d,%d)",
          g_scd.props[0].x, g_scd.props[0].y, g_scd.props[0].z, g_scd.props[0].rot_x, g_scd.props[0].rot_y,
          g_scd.props[0].rot_z, hing, w[0], w[1], w[2]);
    /* Super_reset nach Sleep 26 (@0x0128E): das letzte Zusammensetzen zeigt Clip 0 Bild 58. */
    e->anim_frame = 58;
    t.pc = rst;
    rc = re15_superobj_r41_op_reset(&t);
    int dx = g_scd.props[0].x - (-18756), dy = g_scd.props[0].y - (-266), dz = g_scd.props[0].z - (-26788);
    CHECK(rc == 1 && t.pc == rst + 8 && dx == 0 && dy == 0 && dz == 0 &&
          g_scd.props[0].rot_x == 72 && g_scd.props[0].rot_y == 3072 && g_scd.props[0].rot_z == 0 &&
          !re15_superobj_r41_welt(0, r, w),
          "Super_reset: Loslass-Lage (%d,%d,%d) gegen Original (-18756,-266,-26788) = Endlage (-18116,-266,-26788) - 640, "
          "Abweichung (%d,%d,%d) [Soll 0 = bytegleich], Rot (%d,%d,%d) [Soll (72,3072,0)], danach Welt-Vater",
          g_scd.props[0].x, g_scd.props[0].y, g_scd.props[0].z, dx, dy, dz,
          g_scd.props[0].rot_x, g_scd.props[0].rot_y, g_scd.props[0].rot_z);
    /* Vater ausserhalb State 4 (ROOM4030-Fall): Opcode uebersprungen, Prop unveraendert. */
    e->state = 1;
    g_scd.props[0].x = 111; g_scd.props[0].y = 222; g_scd.props[0].z = 333;
    t.pc = set;
    rc = re15_superobj_r41_op_set(&t);
    CHECK(rc == 1 && t.pc == set + 16 && g_scd.props[0].x == 111 && g_scd.props[0].z == 333 && !re15_superobj_r41_welt(0, r, w),
          "Vater nicht in State 4: uebersprungen, Prop unveraendert (%d,%d,%d)",
          g_scd.props[0].x, g_scd.props[0].y, g_scd.props[0].z);
    re15_rbj_bind_room(NULL, 0); re15_enemy_reset();
}

/* ---------- wclip ---------- */
static void t_wclip(void)
{
    /* W-Bank-Bildzahlen wie der Plattform-Lader: PL00W01.PLW (Marvin, Modell 0x0D -> 76 = PL00Wxx @0x800741e8). */
    char p[600]; snprintf(p, sizeof p, "%s/PLD/PL00W01.PLW", RE15_ASSET_PSX_DIR);
    long n = 0; uint8_t *b = datei(p, &n);
    if (!b) { CHECK(0, "PL00W01.PLW lesbar"); return; }
    uint32_t dir = le32(b), edd = le32(b + dir);
    int nclip = le16(b + edd + 2) / 4;
    uint16_t fcs[32]; int nc = nclip > 32 ? 32 : nclip;
    for (int c = 0; c < nc; c++) fcs[c] = le16(b + edd + 4 * c);
    free(b);
    CHECK(nc >= 7 && fcs[6] == 10, "PL00W01.PLW EDD @+%u: %d Clips, Clip 6 = %d Bilder (Soll 10)", edd, nc, nc > 6 ? fcs[6] : -1);
    re15_player_set_aim_clip_lens(fcs, nc);

    memset(g_actors, 0, sizeof(g_actors));
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1;
    g_current_room_id = 0x3060;
    re15_dining_r39_w_beginn(2, 6);                       /* scd_vm.c op_plc_motion, Spieler-Zweig, pc[1] = 2 */
    CHECK(re15_wclip_r41_clip() == 6 && re15_dining_r39_w_clip() == 6,
          "ROOM3060 Plc_motion(2,6): Zeichner-Clip %d (Soll W-Bank-Clip 6, 0x80073e30[2] = 0x80050f00)", re15_dining_r39_w_clip());
    pl->state = 4; pl->motion = 6; pl->anim_flags = 0;
    pl->anim_frame = 7;  re15_wclip_r41_halten();
    int f7 = (int)pl->anim_frame;
    pl->anim_frame = 14; re15_wclip_r41_halten();
    CHECK(f7 == 7 && pl->anim_frame == 9, "Phase 1 laeuft (Bild 7 bleibt %d), Phase 2 haelt das letzte Bild: 14 -> %d (Soll 9, "
          "@0x80050fec)", f7, (int)pl->anim_frame);
    pl->anim_flags = 4; pl->anim_frame = 14; re15_wclip_r41_halten();
    CHECK(pl->anim_frame == 14, "+0x1c4 & 4 (Schleife @0x80051004): kein Halten (%d)", (int)pl->anim_frame);
    pl->anim_flags = 0; pl->motion = 0; re15_wclip_r41_halten();
    CHECK(re15_wclip_r41_clip() == -1, "anderer Plc-Befehl (Clip 0): W-Phase vorbei");
    re15_dining_r39_w_beginn(2, 6);
    g_current_room_id = 0x3020;
    CHECK(re15_wclip_r41_clip() == -1, "Raumwechsel 3060 -> 3020: W-Phase vorbei (Original o3060_s2_t04: cmd 0)");
    g_current_room_id = 0x3061;
    re15_dining_r39_w_beginn(0, 6);
    CHECK(re15_wclip_r41_clip() == -1, "Plc_motion(0,6) = Sub 0 (+0x180 RBJ): keine W-Bank");
    re15_dining_r39_w_beginn(1, 6);
    CHECK(re15_wclip_r41_clip() == -1, "Plc_motion(1,6) = Sub 1 (+0x16c PLD): keine W-Bank");
    re15_dining_r39_w_beginn(2, 6);
    CHECK(re15_wclip_r41_clip() == 6, "ROOM3061 (Elza) Plc_motion(2,6): W-Bank-Clip 6");
    g_current_room_id = 0x1141;
    re15_dining_r39_w_beginn(2, 6);
    CHECK(re15_wclip_r41_clip() == -1, "ROOM1141: Regel der Runde 39 (dining_r39.c), allgemeine Regel aus");
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    printf("test_r41_stage3szenen_a [%s]\n", teil);
    re15_game_state_init();
    if (!strcmp(teil, "zensus")   || !strcmp(teil, "alle")) t_zensus();
    if (!strcmp(teil, "anker")    || !strcmp(teil, "alle")) t_anker();
    if (!strcmp(teil, "superobj") || !strcmp(teil, "alle")) t_superobj();
    if (!strcmp(teil, "wclip")    || !strcmp(teil, "alle")) t_wclip();
    printf("%s (%d Fehler)\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
