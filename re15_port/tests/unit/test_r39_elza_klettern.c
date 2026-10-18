/* test_r39_elza_klettern.c — Runde 39 Spur H "elza", Punkt 3: "Elza kann in ROOM 1090 bei dem Feuerloescher, bei dem
 * ausgebrannten Fahrzeug hochklettern ... das ist falsch".
 *
 * MECHANIK (Dossier analysis/befunde_runde39/H_elza.md, Belege include/re15_elza_r39.h): die Aktions-Sonde FUN_8002d474
 * nimmt ueber FUN_8002d2c0 jedes installierte Objekt mit Band == Spielerband (@0x8002d364-70) in Reichweite (FUN_8002d1e8,
 * box-unabhaengig) als Kletterobjekt, wenn FUN_8002da4c(., ., -112, Band) trifft — und das trifft bei einer NULL-BOX
 * (Halbbreite 0, `lhu v0,6(a2)` @0x8002da8c vorzeichenlos + Rand @0x8002da94, `sltu` @0x8002daa0) genau aus schraeger
 * Lage (|dx| >= 112 UND |dz| >= 112). Der Port-Loescher (Runde 37 Spur I) trug fest Band 1 = Spielerband in ROOM1090/1091
 * -> Aufstieg (gemessen rot 1024/3072: sub=9 kind=0x81). Das Original haelt Null-Box-Requisiten vom Spielerband fern
 * (ROOM1091 obj 2 @0x02202 Band 2 auf Spielerband 1; ROOM1010 @0x00930 Band 1 auf Spielerband 0).
 *
 *   zensus    alle Obj_model_set-Records (Opcode 0x2D) der 240 RDTs: Band pc[4] gegen Spielerband(y) = -(y/0x708)
 *             (re15_collision_band_from_y). GEMESSEN (nicht wie zuerst angenommen "nie auf dem Spielerband"): von 558
 *             Null-Box-Records liegen 291 auf Spielerband + 1 (haeufigster Wert), 51 AUF dem Spielerband (z.B.
 *             ROOM1040 obj 1-3, ROOM1190 obj 7-11, ROOM4050 obj 4/5 — Eigenheit des Originals, climb_common.c bildet
 *             sie unveraendert ab). Riegel: die drei Vorlagen-Records ROOM1091 obj 2 @0x02202 / ROOM1090 @0x0237E
 *             (Null-Box am selben Wagen) und ROOM1010 @0x00930 (Item-Prop, Vorlage des Port-Loeschers) tragen
 *             Spielerband + 1, und +1 ist der haeufigste Abstand — die Form der Regel re15_elza_r39_prop_band_ueber_boden
 *   loescher  echter Raumstart mit allen Port-Installern (scd_room_reenter), Elza ROOM1091 und Leon ROOM1090 (Feuer aus,
 *             Loescher abgelegt): der Loescher obj 5 steht auf Band 2 (Spielerband 1 + 1); die Aktions-Sonde startet
 *             an der gemessenen Stelle (1880,-1300) in KEINER der vier Blickrichtungen den Aufstieg; die Original-Kiste
 *             obj 0 (Box 900^3, Band 1) ist weiter kletterbar (Gegenprobe); der Untersuchen-Platz des Loeschers bleibt.
 */
#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_climb.h"
#include "re15_collision.h"
#include "re15_gameflow.h"
#include "re15_elza_r39.h"
#include "re15_feuerloescher_r37.h"

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static int g_fail = 0, g_checks = 0;
#define PRUEF(c, ...) do { g_checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static unsigned u16(const uint8_t *b, long o) { return (unsigned)b[o] | ((unsigned)b[o + 1] << 8); }
static unsigned u32(const uint8_t *b, long o) { return u16(b, o) | (u16(b, o + 2) << 16); }
static int      s16(const uint8_t *b, long o) { return (int)(int16_t)(uint16_t)u16(b, o); }

/* Ende einer SCD-Region: naechster Header-Zeiger (RDT+0x20..0x5C) hinter dem Start, sonst Dateiende
 * (Muster tests/unit/test_r38_elzaintro.c). */
static long block_ende(const uint8_t *b, long n, long start)
{
    long e = n;
    for (long o = 0x20; o <= 0x5C; o += 4) {
        long v = (long)u32(b, o);
        if (v > start && v < e) e = v;
    }
    return e;
}

typedef struct { unsigned off; int obj, type, band, flags, x, y, z, hx, hy, hz; } rec_t;

/* Alle Obj_model_set-Records (0x2D, 34 Byte) des Raum-SCD (main RDT+0x40, sub RDT+0x44; je Block u16-Offsettabelle),
 * Opcode-Walk mit scd_opcode_size_at. Rueckgabe = Zahl; *abbrueche zaehlt desynchronisierte Regionen. */
static int records_sammeln(const uint8_t *b, long n, rec_t *out, int max, int *abbrueche)
{
    int k = 0;
    for (int blk = 0; blk < 2; blk++) {
        long st = (long)u32(b, blk ? 0x44 : 0x40);
        if (st <= 0 || st + 2 > n) continue;
        long en = block_ende(b, n, st);
        unsigned regs = u16(b, st) / 2;
        for (unsigned r = 0; r < regs; r++) {
            long rs = st + (long)u16(b, st + 2 * (long)r);
            long re = (r + 1 < regs) ? st + (long)u16(b, st + 2 * (long)(r + 1)) : en;
            long pc = rs;
            while (pc < re && pc + 1 < n) {
                int sz = scd_opcode_size_at(b + pc);
                if (sz <= 0) { (*abbrueche)++; break; }
                if (b[pc] == 0x2D && pc + 34 <= n && k < max) {
                    rec_t *q = &out[k++];
                    q->off = (unsigned)pc; q->obj = b[pc + 1]; q->type = b[pc + 2]; q->band = b[pc + 4];
                    q->flags = (int)(u16(b, pc + 6) | 1u);           /* ori v0,v0,0x1 @0x80040998 */
                    q->x = s16(b, pc + 10); q->y = s16(b, pc + 12); q->z = s16(b, pc + 14);
                    q->hx = s16(b, pc + 28); q->hy = s16(b, pc + 30); q->hz = s16(b, pc + 32);
                }
                pc += sz;
            }
        }
    }
    return k;
}

/* ---------------------------------------------------------------------------------------------- */
static int teil_zensus(void)
{
    int raeume = 0, recs = 0, abbrueche = 0;
    int null_n = 0, box_n = 0;
    int null_hist[16] = {0}, box_hist[16] = {0};      /* Index = diff + 8, diff = Band - Spielerband(y) */
    int null_auf_spielerband = 0;
    int v1091 = -99, v1090 = -99, v1010 = -99;   /* Band - Spielerband der drei Vorlagen-Records */
    for (int stage = 1; stage <= 6; stage++) {
        char dir[512];
        snprintf(dir, sizeof dir, "%s/STAGE%d", RE15_ASSET_PSX_DIR, stage);
        DIR *d = opendir(dir);
        if (!d) { printf("FAIL: Verzeichnis %s nicht lesbar\n", dir); g_fail++; continue; }
        struct dirent *e;
        while ((e = readdir(d)) != NULL) {
            const char *nm = e->d_name;
            size_t ln = strlen(nm);
            if (ln != 12 || strncmp(nm, "ROOM", 4) != 0 || strcmp(nm + 8, ".RDT") != 0) continue;
            char path[600]; size_t sz = 0;
            snprintf(path, sizeof path, "%s/%s", dir, nm);
            uint8_t *b = read_file(path, &sz);
            if (!b) { printf("FAIL: %s nicht lesbar\n", path); g_fail++; continue; }
            raeume++;
            rec_t R[64];
            int k = records_sammeln(b, (long)sz, R, 64, &abbrueche);
            for (int i = 0; i < k; i++) {
                const rec_t *q = &R[i];
                const int pband = re15_collision_band_from_y(q->y);
                int diff = q->band - pband;
                if (diff < -8) diff = -8; if (diff > 7) diff = 7;
                const int nullbox = (q->hx == 0 && q->hy == 0 && q->hz == 0);
                /* Vorlagen (RDT-Bytes selbst gelesen): ROOM1091/1090 obj 2 am Wagen y -2653 (Spielerband 1) Band 2;
                 * ROOM1010 obj 0 First Aid Spray y -1600 (Spielerband 0) Band 1 — jeweils Spielerband(y) + 1 */
                if (strcmp(nm, "ROOM1091.RDT") == 0 && q->off == 0x02202 && nullbox && q->y == -2653) v1091 = diff;
                if (strcmp(nm, "ROOM1090.RDT") == 0 && q->off == 0x0237E && nullbox && q->y == -2653) v1090 = diff;
                if (strcmp(nm, "ROOM1010.RDT") == 0 && q->off == 0x00930 && nullbox && q->y == -1600) v1010 = diff;
                if (nullbox) {
                    null_n++; null_hist[diff + 8]++;
                    if (diff == 0) {
                        null_auf_spielerband++;
                        printf("  NULL-BOX AUF SPIELERBAND: %s @0x%05X obj %d typ %d band %d y %d (Spielerband %d) fl 0x%04X\n",
                               nm, q->off, q->obj, q->type, q->band, q->y, pband, (unsigned)q->flags);
                    }
                } else {
                    box_n++; box_hist[diff + 8]++;
                }
                recs++;
            }
            free(b);
        }
        closedir(d);
    }
    printf("  Zensus: %d RDT, %d Obj_model_set-Records (%d Walk-Abbrueche), Null-Box %d, mit Box %d\n",
           raeume, recs, abbrueche, null_n, box_n);
    printf("  Band - Spielerband(y)   Null-Box:");
    for (int i = 0; i < 16; i++) if (null_hist[i]) printf("  %+d:%d", i - 8, null_hist[i]);
    printf("\n  Band - Spielerband(y)   mit Box :");
    for (int i = 0; i < 16; i++) if (box_hist[i]) printf("  %+d:%d", i - 8, box_hist[i]);
    printf("\n");
    PRUEF(raeume == 240 && recs > 100, "%d RDT gelesen (soll 240), %d Records", raeume, recs);
    printf("  %d Null-Box-Records AUF dem Spielerband (Eigenheit des Originals: FUN_8002da4c @0x8002da8c-a0 traefe sie "
           "schraeg; der Port bildet Original-Records unveraendert ab)\n", null_auf_spielerband);
    PRUEF(v1091 == 1 && v1090 == 1 && v1010 == 1,
          "Vorlagen: ROOM1091 obj 2 @0x02202 (%+d), ROOM1090 @0x0237E (%+d), ROOM1010 @0x00930 (%+d) = Spielerband + 1",
          v1091, v1090, v1010);
    {
        int mx = 0;
        for (int i = 0; i < 16; i++) if (i != 9 && null_hist[i] > mx) mx = null_hist[i];
        PRUEF(null_n > 0 && null_hist[9] > mx && null_hist[9] > null_hist[8],
              "Spielerband + 1 ist der haeufigste Abstand der Null-Box-Records (%d von %d; auf dem Spielerband %d)",
              null_hist[9], null_n, null_hist[8]);
    }
    PRUEF(re15_elza_r39_prop_band_ueber_boden(-1800) == 2 && re15_elza_r39_prop_band_ueber_boden(0) == 1 &&
          re15_elza_r39_prop_band_ueber_boden(-2122) == 2,
          "Regel: Band(y=-1800) = 2 (ROOM1091 obj 2 @0x02202), Band(y=0) = 1 (ROOM1010 @0x00930), Band(y=-2122) = 2");
    return g_fail ? 1 : 0;
}

/* ---------------------------------------------------------------------------------------------- */
static re15_rdt_t s_rdt;
static uint8_t   *s_raw = NULL;

static int raum_laden(int stage, unsigned room)
{
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE%d/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, stage, room);
    if (s_raw) { free(s_raw); s_raw = NULL; }
    s_raw = read_file(path, &sz);
    if (!s_raw) { printf("FAIL: %s nicht lesbar\n", path); g_fail++; return -1; }
    if (re15_rdt_parse(s_raw, sz, &s_rdt) < 0) { printf("FAIL: RDT-Parse %s\n", path); g_fail++; return -1; }
    return 0;
}

/* Raumstart wie probe_r36_klettern_zensus: VM + Pools frisch, scd_room_reenter laeuft main00/sub00 UND den Port-
 * Installer-Block (scd_room_setup.c, darunter re15_feuerloescher_r37_install). */
static void raum_start(unsigned room, int elza)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();            /* memset(&g_scd) — wischt auch die Bankbits, daher die Flags ERST jetzt */
    re15_climb_reset();
    if (!elza) {
        /* Leon ROOM1090: Feuer aus (3,0x81), Loescher genommen (9,0x86) + benutzt (3,0x85), nicht im Inventar -> liegt */
        re15_game_flag_set(9, 0x86, 1); re15_game_flag_set(3, 0x85, 1); re15_game_flag_set(3, 0x81, 1);
    }
    g_gameflow.character = elza ? 4 : 0;
    g_current_room_id = room;
    g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 1880; pl->y = -1800; pl->z = -1300; pl->rot_y = 0;
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_collision_set_band(1);                       /* Boden y -1800 = Spielerband 1 (-(y/0x708)) */
}

static int prop_mit_obj(int obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count && k < 16; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

/* Aktions-Sonde an einer Stelle: Spieler setzen, Viereck -> re15_climb_try_start. Rueckgabe Substate (0 = nichts). */
static int sonde(int32_t x, int32_t z, int yaw, int *kind, int *obj)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_climb_reset();
    re15_collision_set_band(1);
    pl->x = x; pl->y = -1800; pl->z = z; pl->rot_y = (int16_t)yaw;
    (void)re15_climb_try_start(&s_rdt, 1);
    *kind = re15_climb_dbg_kind(); *obj = re15_climb_dbg_obj();
    return re15_climb_dbg_sub();
}

static void loescher_raum(int stage, unsigned room, int elza, int aot_slot)
{
    printf("  --- ROOM%04X (%s) ---\n", room, elza ? "Elza" : "Leon");
    if (raum_laden(stage, room) != 0) return;
    raum_start(room, elza);
    const int p = prop_mit_obj(RE15_FL37_OBJ);
    PRUEF(p >= 0, "Loescher obj %d installiert (Pool-Index %d)", RE15_FL37_OBJ, p);
    if (p < 0) return;
    PRUEF((int)g_scd.props[p].band == re15_elza_r39_prop_band_ueber_boden(g_scd.props[p].y) &&
          (int)g_scd.props[p].band == 2 && (int)g_scd.props[p].band != re15_collision_debug_band(),
          "Loescher Band %d = Spielerband(y=%ld) + 1 = 2, nicht das Spielerband %d (Band-Gate @0x8002d364-70)",
          (int)g_scd.props[p].band, (long)g_scd.props[p].y, re15_collision_debug_band());
    PRUEF(g_scd.props[p].box_hx == 0 && g_scd.props[p].box_hz == 0 && (g_scd.props[p].flags & 0x101u) != 0u,
          "Loescher bleibt Null-Box (h %d/%d) mit Flags 0x%04X (Bit 0 = Installer @0x80040998) — nur das Band schuetzt",
          (int)g_scd.props[p].box_hx, (int)g_scd.props[p].box_hz, (unsigned)g_scd.props[p].flags);
    /* die gemessene Stelle (1880,-1300), vier Blickrichtungen (Sondenpunkt 800 voraus, FUN_8002d474 @0x8002d518-28) */
    static const int yaws[4] = { 0, 1024, 2048, 3072 };
    int starts = 0;
    for (int i = 0; i < 4; i++) {
        int kind = 0, obj = -1;
        const int sub = sonde(1880, -1300, yaws[i], &kind, &obj);
        printf("    rot %4d: sub=%d kind=0x%02X obj=%d\n", yaws[i], sub, (unsigned)kind, obj);
        if (sub != 0) starts++;
    }
    PRUEF(starts == 0, "Aktionstaste neben dem Loescher startet in keiner Blickrichtung den Aufstieg (%d/4)", starts);
    /* Gegenprobe: die Original-Kiste obj 0 (@0x021BE / @0x0216E, Box h 900, Band 1) ist weiter kletterbar */
    const int kiste = prop_mit_obj(0);
    PRUEF(kiste >= 0 && g_scd.props[kiste].box_hx == 900 && (int)g_scd.props[kiste].band == 1,
          "Kiste obj 0 im Pool (Index %d, h %d, Band %d)", kiste, kiste >= 0 ? (int)g_scd.props[kiste].box_hx : -1,
          kiste >= 0 ? (int)g_scd.props[kiste].band : -1);
    if (kiste >= 0) {
        int kind = 0, obj = -1;
        const int32_t kx = g_scd.props[kiste].x, kz = g_scd.props[kiste].z;
        const int sub = sonde(kx - 900 - 450, kz, 0, &kind, &obj);
        PRUEF(sub == 9 && kind == 0x81 && obj == kiste,
              "Kiste: Aufstieg startet (sub=%d kind=0x%02X obj=%d; Substate 9 @0x8002d604, KIND 0x81 @0x8002d614/18)",
              sub, (unsigned)kind, obj);
    }
    PRUEF(aot_slot >= 0 && aot_slot < RE15_AOT_MAX && g_aot.slots[aot_slot].active &&
          g_aot.slots[aot_slot].type == RE15_AOT_TYPE_MESSAGE,
          "Untersuchen-Platz des Loeschers Slot %d steht (MESSAGE, msg 29)", aot_slot);
}

static int teil_loescher(void)
{
    /* Elza ROOM1091: Loescher liegt immer (Runde 37 I 3.4), Untersuchen Slot 3 */
    loescher_raum(1, 0x1091, 1, 3);
    /* Leon ROOM1090 (Flags in raum_start): Loescher liegt, Untersuchen Slot 6 */
    loescher_raum(1, 0x1090, 0, 6);
    return g_fail ? 1 : 0;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "zensus";
    int rc;
    if      (strcmp(teil, "zensus")   == 0) rc = teil_zensus();
    else if (strcmp(teil, "loescher") == 0) rc = teil_loescher();
    else { printf("Aufruf: %s zensus|loescher\n", argv[0]); return 2; }
    printf("%s: %d Pruefungen, %d FAIL\n", teil, g_checks, g_fail);
    return rc;
}
