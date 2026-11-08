/*
 * Integration Runde 41 (Nutzer-Marke 2026-11-08, ROOM6031 F164 "gestreckte Sherry"; Story-Liste: ROOM6030 Annette ebenso):
 * Riegel fuer die Gliederzusammenhaenge eines NPC, der im Sub-0-Kanal (RBJ-Record) spielt.
 *
 * ORIGINAL: Sce_em_set -> FUN_80022300 (jal @0x80042328) schreibt fuer JEDEN Spawn @0x800224a8 "sw v0,0x84(s2)" =
 * dir[2] des neuen Modells (Hierarchie + relpos). Der RBJ-Kanal +0x180/+0x184 (Sub 0 @0x80050d40/48 -> FUN_8001f314)
 * liefert nur EDD + Keyframes: die RBJ-Records haben keine Hierarchie (EMR-Kopf o_arm = 0, o_frm = 8; ROOM6031.RDT
 * RBJ @0x13C8 rec1 @+200, ROOM6030.RDT RBJ @0x17BC rec1 @+200). Die Knochenlagen kommen also immer vom Modell, das
 * gerade im Slot steht.
 *
 * SZENEN (beide ohne Raumladen: die Tuer Slot 1 main00 @0x00C52 fuehrt in denselben Raum):
 *   6031  Wagen sub06 @0x00E42 Aktor 1 = 0x40 (CDEMD1), Fuehrerstand sub07 @0x00EFA Aktor 1 = 0x4B Sherry (CDEMD1)
 *   6030  Wagen sub06 @0x0101C Aktor 1 = 0x49 John (CDEMD0, die Wagen-Szene sub02 bindet ihn), Fuehrerstand
 *         sub07 @0x010D4 Aktor 1 = 0x4D Annette (Port: Ersatzmodell RE2/ANNETTE_4D.EMD, Spur AN)
 * PRUEFUNG: nach dem Typwechsel im selben Slot muss der Abstand jedes Kindknochens zu seinem Elternknochen im
 * gebundenen Kanal-Skelett (= |relpos|; Drehungen aendern Laengen nicht) gleich dem des gezeichneten Modells sein.
 * Toleranz 0: das Original zeigt die relpos des Modells unveraendert (@0x800224a8), es gibt nichts zu runden;
 * verglichen werden die ganzzahligen relpos selbst, der Abstand wird nur zur Anzeige gerechnet. Zusaetzlich: die
 * Keyframes kommen weiter aus dem RBJ-Record (Zeiger in den RBJ-Block), der Kanal ist also nicht abgefallen.
 * GEGENPROBE: Stand b27d96891 (Cache je Slot ohne Typ) -> rot.
 */
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_room.h"
#include "re15_roy_r37.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
                              else { printf("  ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *datei_teil(const char *pfad, long off, long n_soll, long *n)
{
    FILE *f = fopen(pfad, "rb"); if (!f) return NULL;
    if (n_soll < 0) { fseek(f, 0, SEEK_END); n_soll = ftell(f) - off; }
    fseek(f, off, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n_soll + 1);
    if (b && fread(b, 1, (size_t)n_soll, f) != (size_t)n_soll) { free(b); b = NULL; }
    fclose(f); *n = n_soll; return b;
}
static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Modell eines Typs aus dem Archiv (TOC @0x80072f38 Leon / @0x80073178 Elza, re15_ems_toc) oder einer RE2-Datei. */
static uint8_t *modell(int archiv, uint8_t typ, const char *re2_datei, long *n)
{
    char p[512];
    if (re2_datei) {
        snprintf(p, sizeof p, "%s/../RE2/%s", RE15_ASSET_PSX_DIR, re2_datei);
        return datei_teil(p, 0, -1, n);
    }
    uint32_t sektor = 0, laenge = 0;
    if (!re15_ems_toc(archiv, typ, &sektor, &laenge)) return NULL;
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(archiv));
    return datei_teil(p, (long)sektor * 2048, (long)laenge, n);
}

static re15_emd_animation_t s_anim_tmp;
static int bank_fuellen(uint8_t typ, const uint8_t *emd, long n, re15_emd_skeleton_t *out)
{
    memset(out, 0, sizeof *out);
    (void)re15_emd_parse_container(emd, (size_t)n, NULL, out, &s_anim_tmp, NULL);   /* ohne MD1: Rueckgabe -1, Skelett gefuellt */
    if (out->bone_count != 15) return 0;
    re15_enemy_bank_t *eb = re15_enemy_alloc(typ);
    if (!eb) return 0;
    eb->skel = *out; eb->anim = s_anim_tmp; eb->ok = 1;
    return 1;
}

static double laenge(const int16_t *v) { return sqrt((double)v[0] * v[0] + (double)v[1] * v[1] + (double)v[2] * v[2]); }

static void szene(const char *name, const char *stage, unsigned raum,
                  int archiv_a, uint8_t typ_a, const char *re2_a,
                  int archiv_b, uint8_t typ_b, const char *re2_b)
{
    printf("== %s (ROOM%04X): Aktor 1 erst Typ 0x%02X, dann Typ 0x%02X im selben Raum\n", name, raum, typ_a, typ_b);
    char p[512]; snprintf(p, sizeof p, "%s/%s/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, stage, raum);
    long n = 0; uint8_t *rdt = datei_teil(p, 0, -1, &n);
    if (!rdt || n < 0x60) { CHECK(0, "%s ladbar", p); free(rdt); return; }
    uint32_t s = le32(rdt + 0x5C);
    const uint8_t *rbj = rdt + s; size_t rbj_n = (size_t)(n - (long)s);
    long na = 0, nb = 0;
    uint8_t *ea = modell(archiv_a, typ_a, re2_a, &na), *ebuf = modell(archiv_b, typ_b, re2_b, &nb);
    re15_emd_skeleton_t ska, skb;
    g_current_room_id = raum;
    re15_enemy_reset();
    if (!ea || !ebuf || !bank_fuellen(typ_a, ea, na, &ska) || !bank_fuellen(typ_b, ebuf, nb, &skb)) {
        CHECK(0, "%s: Modelle 0x%02X/0x%02X ladbar (15 Knochen)", name, typ_a, typ_b);
        free(rdt); free(ea); free(ebuf); return;
    }
    memset(g_actors, 0, sizeof(g_actors));
    g_actors[1].active = 1; g_actors[1].type = typ_a;
    re15_rbj_bind_room(rbj, rbj_n);
    const re15_emd_skeleton_t *k1 = re15_actor_rbj_skel(1);
    CHECK(k1 != NULL, "%s Wagen: Aktor 1 (0x%02X) im Sub-0-Kanal gebunden", name, typ_a);
    if (k1) CHECK(memcmp(k1->bone_relative_pos, ska.bone_relative_pos, sizeof ska.bone_relative_pos) == 0,
                  "%s Wagen: Kanal-Skelett = Hierarchie von 0x%02X", name, typ_a);
    /* Spawn im selben Slot ohne Raumladen (Fuehrerstand sub07) */
    g_actors[1].type = typ_b;
    const re15_emd_skeleton_t *k2 = re15_actor_rbj_skel(1);
    CHECK(k2 != NULL, "%s Fuehrerstand: Aktor 1 (0x%02X) im Sub-0-Kanal gebunden", name, typ_b);
    if (k2) {
        int max_abw = 0; double max_d = 0;
        for (int b = 1; b < 15; b++) {
            int abw = 0;
            for (int c = 0; c < 3; c++) {
                int d = abs((int)k2->bone_relative_pos[b][c] - (int)skb.bone_relative_pos[b][c]);
                if (d > abw) abw = d;
            }
            double dl = fabs(laenge(k2->bone_relative_pos[b]) - laenge(skb.bone_relative_pos[b]));
            if (abw) printf("    Knochen %2d (Eltern %d): Kanal (%d,%d,%d) |%.0f| gegen Modell (%d,%d,%d) |%.0f|\n", b,
                            skb.bone_parent[b], k2->bone_relative_pos[b][0], k2->bone_relative_pos[b][1],
                            k2->bone_relative_pos[b][2], laenge(k2->bone_relative_pos[b]), skb.bone_relative_pos[b][0],
                            skb.bone_relative_pos[b][1], skb.bone_relative_pos[b][2], laenge(skb.bone_relative_pos[b]));
            if (abw > max_abw) max_abw = abw;
            if (dl > max_d) max_d = dl;
        }
        CHECK(max_abw == 0 && memcmp(k2->bone_parent, skb.bone_parent, 15) == 0,
              "%s Fuehrerstand: Glieder von 0x%02X zusammen - groesste relpos-Abweichung %d, Abstand Kind-Eltern %.0f "
              "(Toleranz 0, @0x800224a8)", name, typ_b, max_abw, max_d);
        CHECK(k2->keyframe_data >= rbj && k2->keyframe_data < rbj + rbj_n,
              "%s Fuehrerstand: Keyframes weiter aus dem RBJ-Record (Kanal +0x184)", name);
    }
    re15_rbj_bind_room(NULL, 0);
    re15_enemy_reset();
    free(rdt); free(ea); free(ebuf);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "sherry6031") || !strcmp(teil, "alle"))
        szene("Sherry 6031", "STAGE6", 0x6031, RE15_EMS_ARCHIV_ELZA, 0x40, NULL, RE15_EMS_ARCHIV_ELZA, 0x4B, NULL);
    if (!strcmp(teil, "annette6030") || !strcmp(teil, "alle"))
        szene("Annette 6030", "STAGE6", 0x6030, RE15_EMS_ARCHIV_LEON, 0x49, NULL, RE15_EMS_ARCHIV_LEON, 0x4D, "ANNETTE_4D.EMD");
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
