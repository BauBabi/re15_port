/*
 * Runde 41 Spur J (Orchestrator-Entscheid A-E5): Riegel fuer den RBJ-Marker-Binder byte-true FUN_8001b3f8.
 * Code engine/src/rbj_binder_r41.c (+ Haken enemy_common.c rbj_resolve_slot / re15_apply_room_cinematic),
 * Belege include/re15_rbj_binder_r41.h, Dossier analysis/befunde_runde41/J_stage3szenen_a.md §0.
 *
 *   mechanik  synthetische Marker: a3 kumulativ je Bitposition (@0x8001b4c4), spaeterer Record ueberschreibt
 *             (@0x8001b4a0), Spieler = letzter Record mit Bit 0 (@0x8001b450), kein Bit 0 -> -1
 *   zensus    alle ausgelieferten RDTs: 47 mit RBJ, 12 weichen von der alten Port-Regel ab; Sollwerte je Raum
 *             (aus der Disasm-Nachbildung tools/r41_j/rbj_binder_zensus.py)
 *   kanal     der Port-Weg selbst: re15_rbj_bind_room + Aktor-Slots -> re15_actor_rbj_anim (welcher Record spielt
 *             den Sub-0-Kanal) und re15_apply_room_cinematic (Spieler-Overlay) an echten RDTs 3060/3091/2000/1040/3061
 */
#include "re15_rbj_binder_r41.h"
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_room.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

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

/* RBJ-Block einer RDT (u32 @RDT+0x5C = Datei-Offset, wie tools/r34n_d/rbj_zensus.py rbj_block). */
static const uint8_t *rbj_von(const uint8_t *d, long n, size_t *sz)
{
    if (n < 0x60) return NULL;
    uint32_t s = le32(d + 0x5C);
    if (s == 0 || (long)s + 8 > n) return NULL;
    uint32_t total = le32(d + s), nrec = le32(d + s + 4);
    if (total == 0 || nrec == 0 || nrec > 8 || (long)(s + total + nrec * 8) > n) return NULL;
    *sz = (size_t)(n - (long)s);
    return d + s;
}

/* ---------- mechanik ---------- */
static void baue(uint8_t *buf, size_t cap, const uint32_t *mk, int nrec, size_t *len)
{
    /* Minimal-RBJ: [u32 total][u8 nrec] .. [Records: je u32 Marker + 12 Bytes] [Trailer: je (prefix, edd)] */
    memset(buf, 0, cap);
    size_t o = 0x20;
    uint32_t pre[8];
    for (int r = 0; r < nrec; r++) { pre[r] = (uint32_t)o; memcpy(buf + o, &mk[r], 4); o += 16; }
    uint32_t total = (uint32_t)o;
    memcpy(buf, &total, 4); buf[4] = (uint8_t)nrec;
    for (int r = 0; r < nrec; r++) {
        memcpy(buf + o, &pre[r], 4); uint32_t e = pre[r] + 4; memcpy(buf + o + 4, &e, 4); o += 8;
    }
    *len = o;
}
static void t_mechanik(void)
{
    uint8_t b[512]; size_t n; re15_rbj_bindung_t r;
    { uint32_t mk[] = { 0x1, 0x2, 0x4 }; baue(b, sizeof b, mk, 3, &n); re15_rbj_binder(b, n, &r);
      CHECK(r.spieler == 0 && r.gegner[0] == 1 && r.gegner[1] == -1 && r.gegner[2] == 2,
            "Marker 1/2/4 (ROOM3060-Form): Spieler rec%d, Gegner 0:rec%d 1:rec%d 2:rec%d (Soll 0 / 1 / -1 / 2: a3 zaehlt das "
            "0-Bit von rec2 mit, @0x8001b4c4)", r.spieler, r.gegner[0], r.gegner[1], r.gegner[2]);
      CHECK(re15_rbj_binder_alt_gegner(b, n, 1) == 2 && re15_rbj_binder_alt_gegner(b, n, 2) == -1,
            "alte Port-Regel dort: Gegner 1 = rec2, Gegner 2 ungebunden (der Unterschied)"); }
    { uint32_t mk[] = { 0x1, 0x2, 0x3 }; baue(b, sizeof b, mk, 3, &n); re15_rbj_binder(b, n, &r);
      CHECK(r.spieler == 2 && r.gegner[0] == 1 && r.gegner[1] == 2,
            "Marker 1/2/3 (ROOM3091-Form): Spieler rec%d (Soll 2, letzter mit Bit 0 @0x8001b450), Gegner 0:rec%d 1:rec%d (Soll 1/2)",
            r.spieler, r.gegner[0], r.gegner[1]); }
    { uint32_t mk[] = { 0x7e }; baue(b, sizeof b, mk, 1, &n); re15_rbj_binder(b, n, &r);
      int alle = 1; for (int i = 0; i < 6; i++) alle &= (r.gegner[i] == 0);
      CHECK(r.spieler == -1 && alle && r.gegner[6] == -1,
            "Marker 0x7e (ROOM1040-Form): Spieler -1 (Kanal nicht geschrieben), Gegner 0..5 = rec0, 6 frei"); }
    { uint32_t mk[] = { 0x2, 0x2 }; baue(b, sizeof b, mk, 2, &n); re15_rbj_binder(b, n, &r);
      CHECK(r.spieler == -1 && r.gegner[0] == 0 && r.gegner[1] == 1,
            "Marker 2/2: Gegner 0:rec%d 1:rec%d (Soll 0/1 - a3 wird je Record NICHT zurueckgesetzt, @0x8001b410 nur einmal)",
            r.gegner[0], r.gegner[1]); }
    { uint32_t mk[] = { 0x5, 0x9 }; baue(b, sizeof b, mk, 2, &n); re15_rbj_binder(b, n, &r);
      /* rec0: >>1 = 0b10 -> Bitpos 0 leer (a3 0->1), Bitpos 1 gesetzt -> Gegner 1 = rec0, a3 = 2;
       * rec1: Bit 0 -> Spieler = rec1; >>1 = 0b100 -> Bitpos 0,1 leer (a3 2,3), Bitpos 2 -> Gegner 4 = rec1 */
      CHECK(r.spieler == 1 && r.gegner[0] == -1 && r.gegner[1] == 0 && r.gegner[4] == 1 && r.gegner[2] == -1,
            "Marker 5/9: Spieler rec%d (Soll 1), Gegner 1:rec%d 4:rec%d (Soll 0 / 1)", r.spieler, r.gegner[1], r.gegner[4]); }
    CHECK(re15_rbj_binder(NULL, 0, &r) == -1 && r.spieler == -1 && r.gegner[0] == -1,
          "kein RBJ -> -1, alles ungebunden (@0x8001b40c)");
}

/* ---------- zensus ---------- */
typedef struct { const char *raum; int spieler; int g[4]; } soll_t;   /* g[i] = Gegner i, -1 = frei */
static const soll_t k_soll[] = {   /* Disasm-Nachbildung tools/r41_j/rbj_binder_zensus.py (Dossier §0.2) */
    { "ROOM1040", -1, { 0, 0, 0, 0 } },   { "ROOM1041", -1, { 0, 0, 0, 0 } },
    { "ROOM2000",  2, { 1, 2, -1, -1 } }, { "ROOM3060",  0, { 1, -1, 2, -1 } },
    { "ROOM3091",  2, { 1, 2, -1, -1 } }, { "ROOM30E0",  2, { 1, 2, -1, -1 } },
    { "ROOM4000",  2, { 1, 2, -1, -1 } }, { "ROOM4001",  2, { 1, 2, -1, -1 } },
    { "ROOM4010",  2, { 1, 2, -1, -1 } }, { "ROOM5011",  2, { 1, 2, -1, -1 } },
    { "ROOM6030",  2, { 1, 2, -1, -1 } }, { "ROOM6031",  2, { 1, 2, -1, -1 } },
};
static void t_zensus(void)
{
    int n_rdt = 0, n_rbj = 0, n_ab = 0, n_soll = 0;
    for (int st = 1; st <= 6; st++) {
        char dir[512]; snprintf(dir, sizeof dir, "%s/STAGE%d", RE15_ASSET_PSX_DIR, st);
        DIR *d = opendir(dir); if (!d) continue;
        struct dirent *e;
        while ((e = readdir(d)) != NULL) {
            const char *nm = e->d_name; size_t l = strlen(nm);
            if (l != 12 || strncmp(nm, "ROOM", 4) != 0 || strcmp(nm + 8, ".RDT") != 0) continue;
            char p[700]; snprintf(p, sizeof p, "%s/%s", dir, nm);
            long n = 0; uint8_t *b = datei(p, &n); if (!b) continue;
            n_rdt++;
            size_t sz = 0; const uint8_t *rbj = rbj_von(b, n, &sz);
            if (rbj) {
                n_rbj++;
                re15_rbj_bindung_t r; re15_rbj_binder(rbj, sz, &r);
                int ab = (r.spieler != 0);
                for (int i = 0; i < 31; i++) if (r.gegner[i] != re15_rbj_binder_alt_gegner(rbj, sz, i)) ab = 1;
                n_ab += ab;
                for (size_t k = 0; k < sizeof k_soll / sizeof k_soll[0]; k++) {
                    if (strncmp(nm, k_soll[k].raum, 8) != 0) continue;
                    n_soll++;
                    int g_ok = 1; for (int i = 0; i < 4; i++) g_ok &= (r.gegner[i] == k_soll[k].g[i]);
                    CHECK(ab && r.spieler == k_soll[k].spieler && g_ok,
                          "%.8s: Spieler rec%d (Soll %d), Gegner 0..3 = %d %d %d %d (Soll %d %d %d %d), weicht von alter Regel ab",
                          nm, r.spieler, k_soll[k].spieler, r.gegner[0], r.gegner[1], r.gegner[2], r.gegner[3],
                          k_soll[k].g[0], k_soll[k].g[1], k_soll[k].g[2], k_soll[k].g[3]);
                }
            }
            free(b);
        }
        closedir(d);
    }
    CHECK(n_rdt == 240 && n_rbj == 47 && n_ab == 12 && n_soll == 12,
          "Zensus: %d RDTs, %d mit RBJ, %d abweichend, %d Sollzeilen geprueft (Soll 240 / 47 / 12 / 12)",
          n_rdt, n_rbj, n_ab, n_soll);
}

/* ---------- kanal (Port-Weg) ---------- */
static re15_emd_skeleton_t  s_base, s_scr_skel, s_leon_skel, s_ell_skel, s_tmp_skel;
static re15_emd_animation_t s_scr_anim, s_leon_anim, s_ell_anim, s_tmp_anim;
static uint8_t *s_emr;
static int basis_laden(void)
{
    char p[512]; snprintf(p, sizeof p, "%s/PLD/PL00.EMR", RE15_ASSET_PSX_DIR);
    long n = 0; s_emr = datei(p, &n);
    return s_emr && re15_emd_parse_skeleton(s_emr, (size_t)n, &s_base) == 0;
}
static void noop(uint8_t t) { (void)t; }
static int clips_rec(const uint8_t *rbj, size_t sz, int rec)
{
    return re15_emd_parse_rbj_record(rbj, sz, rec, &s_base, &s_tmp_skel, &s_tmp_anim) == 0 ? s_tmp_anim.clip_count : -1;
}
static void raum_kanal(const char *stage, unsigned id, int n_akt)
{
    char p[512]; snprintf(p, sizeof p, "%s/%s/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, stage, id);
    long n = 0; uint8_t *b = datei(p, &n); size_t sz = 0;
    const uint8_t *rbj = b ? rbj_von(b, n, &sz) : NULL;
    if (!rbj) { CHECK(0, "ROOM%04X ladbar", id); free(b); return; }
    g_current_room_id = id;
    re15_enemy_reset();
    /* Gegner-Bank fuer einen Hilfstyp: Basis-Hierarchie = PL00-Skelett (15 Bones wie die Records). */
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x40);
    if (!eb) { CHECK(0, "Gegner-Bank frei"); free(b); return; }
    eb->skel = s_base; eb->ok = 1;
    memset(g_actors, 0, sizeof(g_actors));
    for (int s = 1; s <= n_akt; s++) { g_actors[s].active = 1; g_actors[s].type = 0x40; }
    re15_rbj_bind_room(rbj, sz);
    re15_rbj_bindung_t r; re15_rbj_binder(rbj, sz, &r);
    for (int s = 1; s <= n_akt; s++) {
        const re15_emd_animation_t *a = re15_actor_rbj_anim(s);
        int ist = a ? a->clip_count : -1;
        int soll = r.gegner[s - 1] >= 0 ? clips_rec(rbj, sz, r.gegner[s - 1]) : -1;
        CHECK(ist == soll,
              "ROOM%04X Aktor %d: Sub-0-Kanal %s (%d Clips), Soll rec%d (%d Clips), alte Regel rec%d",
              id, s, a ? "gebunden" : "ungebunden", ist, r.gegner[s - 1], soll,
              re15_rbj_binder_alt_gegner(rbj, sz, s - 1));
    }
    /* Spieler-Overlay: def-Bank vorher auf eine Kennung setzen, dann re15_apply_room_cinematic. */
    memset(&s_leon_anim, 0, sizeof s_leon_anim); s_leon_anim.clip_count = -7;
    re15_apply_room_cinematic(rbj, sz, id, &s_base, &s_leon_skel, &s_leon_anim, NULL, 0, &s_ell_skel, &s_ell_anim,
                              &s_scr_skel, &s_scr_anim, noop);
    int soll_pl = r.spieler >= 0 ? clips_rec(rbj, sz, r.spieler) : -7;
    CHECK(s_leon_anim.clip_count == soll_pl,
          "ROOM%04X Spieler-Overlay: %d Clips (Soll rec%d = %d Clips; -7 = Overlay nicht geschrieben)",
          id, s_leon_anim.clip_count, r.spieler, soll_pl);
    re15_rbj_bind_room(NULL, 0);
    re15_enemy_reset();
    free(b);
}
static void t_kanal(void)
{
    if (!basis_laden()) { CHECK(0, "PL00.EMR ladbar"); return; }
    raum_kanal("STAGE3", 0x3060, 3);   /* Gegner 1 jetzt ungebunden, Gegner 2 = rec2 */
    raum_kanal("STAGE3", 0x3091, 2);   /* Spieler rec2 (14 Clips) statt rec0 (1 Clip), Gegner 1 = rec2 */
    raum_kanal("STAGE2", 0x2000, 2);
    raum_kanal("STAGE1", 0x1040, 2);   /* Spieler-Kanal nicht geschrieben */
    raum_kanal("STAGE3", 0x3061, 2);   /* unveraendert: Spieler rec0, Gegner 0 = rec1 */
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "mechanik") || !strcmp(teil, "alle")) t_mechanik();
    if (!strcmp(teil, "zensus")   || !strcmp(teil, "alle")) t_zensus();
    if (!strcmp(teil, "kanal")    || !strcmp(teil, "alle")) t_kanal();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
