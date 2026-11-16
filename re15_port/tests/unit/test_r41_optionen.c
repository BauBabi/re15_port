/* test_r41_optionen.c — Runde 41 Optionen: "AI MIXED" raus, neue Option "ULTIMATE 1.5" (include/re15_ultimate15.h).
 *
 * Ohne Argument (unit_optionen_r41):
 *   A  KI: ein gesetzter MIXED-Wert (Env RE15_AI_FLAVOR=mixed) kommt als RE2 an (Nutzer: "AI Mixed herausnehmen ...
 *      RE 2 AI ... bleibt unser Standard"). Auf master rot: dort wird "mixed" zu RE15_AI_FLAVOR_MIXED.
 *   B  Schalter + Options-Datei: Standard AUS, Parser, Rundreise ueber re15_options.cfg (RE15_OPTIONS_FILE), bei AUS
 *      ist die Datei Byte fuer Byte die bisherige.
 *   C  Inventar: Regel-Pruefung der Zuordnung an den echten Dateien (Platzhalter = Browning-Kopie / BOSS-Karte), Puffer
 *      bauen, Leser re15_itemall_tile_raw / re15_itps_pixel: AN = Disc-Bild, AUS = heutiges Bild (bitgleich).
 *   D  Welt: Prop-Zensus der Mod-RDT (ROOM1020 obj 4/5 = 0x15, ROOM11B0 obj 1 = 0x30, ROOM4010 obj 2/3 = 0x22/0x16),
 *      Disc-Weltmodelle lesbar (re15_rdt_parse + re15_md1_parse + re15_tim_parse) und vom heutigen verschieden.
 * Werkzeug (fuer integration_optionen_r41_menue):
 *   bmp_gleich <a.bmp> <b.bmp> <x0> <y0> <x1> <y1>    Rechteck in beiden 24-bpp-BMP pixelgleich? (exit 0 = ja)
 *   bmp_anders <a.bmp> <b.bmp> <x0> <y0> <x1> <y1>    Rechteck verschieden?                       (exit 0 = ja)
 */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_ultimate15.h"
#include "re15_lautstaerke_r41.h"
#include "re15_ai_flavor.h"
#include "re15_item_icon.h"
#include "re15_itps.h"
#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_tim.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int s_fails = 0, s_checks = 0;
#define CHECK(cond, ...) do { s_checks++; if (!(cond)) { s_fails++; printf("FAIL: "); } else printf("ok:   "); \
                              printf(__VA_ARGS__); printf("\n"); } while (0)

static void env_setzen(const char *name, const char *wert)
{
#ifdef _WIN32
    char buf[1024]; snprintf(buf, sizeof buf, "%s=%s", name, wert ? wert : ""); _putenv(buf);
#else
    if (wert) setenv(name, wert, 1); else unsetenv(name);
#endif
}

static uint8_t *lesen(const char *rel, int *n)
{
    char p[1024]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(p, "rb"); if (!f) { *n = 0; return NULL; }
    fseek(f, 0, SEEK_END); long g = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)g + 1);
    *n = (int)fread(b, 1, (size_t)g, f); fclose(f);
    return b;
}

/* ---- A KI ------------------------------------------------------------------------------------------------------ */
static void teil_a(void)
{
    printf("\n== A KI ohne MIXED\n");
    /* Muss der ERSTE re15_ai_flavor()-Aufruf des Prozesses sein (das Env wird einmal gelesen). */
    env_setzen("RE15_AI_FLAVOR", "mixed");
    re15_ai_flavor_t f = re15_ai_flavor();
    CHECK(f == RE15_AI_FLAVOR_RE2, "RE15_AI_FLAVOR=mixed -> RE2 (bekommen %d, RE2 = %d)", (int)f, (int)RE15_AI_FLAVOR_RE2);
    env_setzen("RE15_AI_FLAVOR", NULL);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
}

/* ---- B Schalter + Datei ---------------------------------------------------------------------------------------- */
static int datei_text(const char *pfad, char *buf, int n)
{
    FILE *f = fopen(pfad, "rb"); if (!f) return -1;
    int g = (int)fread(buf, 1, (size_t)n - 1, f); fclose(f); buf[g] = 0; return g;
}

static void teil_b(void)
{
    printf("\n== B Schalter + re15_options.cfg\n");
    CHECK(re15_ultimate15() == 0, "Standard AUS");
    CHECK(re15_ultimate15_parse("volume=50\nsound=stereo\nultimate=on\n") == 1, "Parser: ultimate=on -> 1");
    CHECK(re15_ultimate15_parse("ultimate=off\r\nvolume=50\r\n") == 0, "Parser: ultimate=off (CRLF) -> 0");
    CHECK(re15_ultimate15_parse("volume=50\nsound=stereo\n") == -1, "Parser: kein Schluessel -> -1");
    unsigned g0 = re15_ultimate15_generation();
    re15_ultimate15_set(1);
    CHECK(re15_ultimate15() == 1 && re15_ultimate15_generation() == g0 + 1, "set(1): AN, Generation +1");
    re15_ultimate15_set(1);
    CHECK(re15_ultimate15_generation() == g0 + 1, "set(1) erneut: Generation unveraendert");
    re15_ultimate15_set(0);

    char pfad[256], txt[256];
    snprintf(pfad, sizeof pfad, "r41_optionen_%ld.cfg", (long)rand());
    remove(pfad);
    env_setzen(RE15_LAUT_R41_ENV, pfad);
    env_setzen(RE15_U15_ENV, NULL);
    re15_lautstaerke_r41_laden();                               /* keine Datei */
    CHECK(re15_ultimate15() == 0, "Laden ohne Datei: AUS");
    re15_ultimate15_set(1);
    CHECK(re15_lautstaerke_r41_speichern() == 0, "Speichern bei AN");
    datei_text(pfad, txt, sizeof txt);
    CHECK(!strcmp(txt, "volume=50\nsound=stereo\nultimate=on\n"), "Datei bei AN: '%s'", txt);
    re15_lautstaerke_r41_reset();
    CHECK(re15_ultimate15() == 0, "reset: AUS");
    re15_lautstaerke_r41_laden();
    CHECK(re15_ultimate15() == 1, "Rundreise: Laden -> AN");
    re15_ultimate15_set(0);
    re15_lautstaerke_r41_speichern();
    datei_text(pfad, txt, sizeof txt);
    CHECK(!strcmp(txt, "volume=50\nsound=stereo\n"), "Datei bei AUS = bisheriges Format, bitgleich: '%s'", txt);
    re15_lautstaerke_r41_laden();
    CHECK(re15_ultimate15() == 0, "Rundreise: Laden -> AUS");
    env_setzen(RE15_U15_ENV, "1");
    re15_lautstaerke_r41_laden();
    CHECK(re15_ultimate15() == 1, "Messhaken RE15_ULTIMATE15=1 uebersteuert die Datei");
    env_setzen(RE15_U15_ENV, NULL);
    remove(pfad);
    env_setzen(RE15_LAUT_R41_ENV, NULL);
    re15_ultimate15_set(0);
}

/* ---- C Inventar -------------------------------------------------------------------------------------------------- */
static int kachel_gleich(const uint8_t *a, int i, const uint8_t *b, int j)
{
    return !memcmp(a + i * RE15_U15_KACHEL_B, b + j * RE15_U15_KACHEL_B, RE15_U15_KACHEL_B);
}

static void teil_c(void)
{
    printf("\n== C Inventar\n");
    int mn = 0, dn = 0, imn = 0, idn = 0, xmn = 0, xdn = 0;
    uint8_t *mod  = lesen("DATA/ITEMALL.PIX", &mn);
    uint8_t *disc = lesen(RE15_U15_DISC_DIR "/DATA/ITEMALL.PIX", &dn);
    uint8_t *imod = lesen("ITEM/ITPS.ITP", &imn);
    uint8_t *idsc = lesen(RE15_U15_DISC_DIR "/ITEM/ITPS.ITP", &idn);
    uint8_t *xmod = lesen("DATA/MIXITEM.PIX", &xmn);
    uint8_t *xdsc = lesen(RE15_U15_DISC_DIR "/DATA/MIXITEM.PIX", &xdn);
    CHECK(mod && disc && dn == 86400 && imod && idsc && idn == 872448 && xdsc && xdn == 16800,
          "Dateien da: Mod ITEMALL %d, Disc ITEMALL %d, Disc ITPS %d, Disc MIXITEM %d", mn, dn, idn, xdn);
    if (!mod || !disc || !imod || !idsc || !xmod || !xdsc) return;

    /* Regel-Pruefung ITEMALL: jede ausgeschlossene Id ist entweder Platzhalter (Browning-Kopie) oder heute anders
     * belegt; jede eingesetzte Quelle ist KEINE Browning-Kopie. */
    int platz = 0, quellen = 0, quell_platz = 0;
    for (unsigned id = 0; id < RE15_U15_KACHELN; id++) {
        int q = re15_ultimate15_icon_quelle(id);
        if (q < 0) { if (id != 3 && kachel_gleich(disc, (int)id, disc, 3)) platz++; continue; }
        quellen++;
        if (q != 3 && kachel_gleich(disc, q, disc, 3)) quell_platz++;
    }
    CHECK(platz == 6, "ausgeschlossene Ids mit Browning-Kopie auf der Disc: %d (0x2F 0x30 0x32 0x35 0x42 0x45 = 6; "
          "0x31 holt sein Bild von Disc 0x14)", platz);
    CHECK(quell_platz == 0, "keine eingesetzte Disc-Kachel ist ein Platzhalter (%d von %d)", quell_platz, quellen);

    uint8_t *u15 = (uint8_t *)malloc((size_t)mn);
    int k = re15_ultimate15_itemall_bauen(u15, mod, mn, disc, dn);
    CHECK(k == quellen, "ITEMALL: %d Kacheln eingesetzt", k);
    CHECK(kachel_gleich(u15, 0x15, disc, 0x15) && !kachel_gleich(mod, 0x15, disc, 0x15),
          "0x15 H. Gun Bullets: Disc-Kachel, heute anders");
    CHECK(kachel_gleich(u15, 0x31, disc, 0x14), "0x31 Fire Extinguisher <- Disc 0x14 (Feuerloescher)");
    CHECK(kachel_gleich(u15, 0x14, mod, 0x14), "0x14 Colt Python bleibt (Disc 0x14 ist der Feuerloescher)");
    CHECK(kachel_gleich(u15, 0x2F, mod, 0x2F), "0x2F Nut bleibt (Disc = Browning-Kopie)");
    CHECK(kachel_gleich(u15, 0x33, mod, 0x33) && kachel_gleich(u15, 0x43, mod, 0x43),
          "0x33/0x43 (Port-Belegung) bleiben");
    /* Runde 2 (Pruefbericht r1, rot): Disc-Kachel 0x46 zeigt einen Schluessel (Disc-Name 0x46 "IDカードーM", Bild aber
     * kein Kartenbild; einziger Schluessel-Name der Disc = 0x45 "水キー" = Water Key, Port-Id 0x34). Nach Bedeutung:
     * die Rote Master-Keycard behaelt ihre Karte, der Water Key bekommt den Disc-Schluessel. */
    CHECK(re15_ultimate15_icon_quelle(0x46) == -1 && kachel_gleich(u15, 0x46, mod, 0x46) && !kachel_gleich(u15, 0x46, disc, 0x46),
          "0x46 Rote Master-Keycard: bleibt Karte, NICHT der Disc-Schluessel (Disc-Kachel 0x46)");
    CHECK(re15_ultimate15_icon_quelle(0x34) == 0x46 && kachel_gleich(u15, 0x34, disc, 0x46),
          "0x34 Water Key <- Disc-Kachel 0x46 (Schluessel)");
    CHECK(re15_ultimate15_icon_quelle(0x47) == 0x47 && kachel_gleich(u15, 0x47, disc, 0x47),
          "0x47 Blaue Master-Keycard <- Disc-Kachel 0x47 (Karte)");
    {   /* keine zwei heutigen Ids teilen sich eine Disc-Kachel (jede Disc-Kachel hoechstens einmal vergeben) */
        int doppelt = 0;
        for (unsigned a = 0; a < RE15_U15_KACHELN; a++) for (unsigned b = a + 1; b < RE15_U15_KACHELN; b++) {
            int qa = re15_ultimate15_icon_quelle(a), qb = re15_ultimate15_icon_quelle(b);
            if (qa >= 0 && qa == qb) doppelt++;
        }
        CHECK(doppelt == 0, "jede Disc-Kachel hoechstens einmal vergeben (%d doppelt)", doppelt);
    }

    /* Leser: AUS bitgleich, AN Disc. */
    re15_itemall_set_pix(mod, mn);
    re15_itemall_set_pix_u15(u15, mn);
    re15_ultimate15_set(0);
    const uint8_t *r0 = re15_itemall_tile_raw(0x15);
    CHECK(r0 && !memcmp(r0, mod + 0x15 * 1200, 1200), "AUS: re15_itemall_tile_raw(0x15) == heutige Kachel");
    re15_ultimate15_set(1);
    const uint8_t *r1 = re15_itemall_tile_raw(0x15);
    CHECK(r1 && !memcmp(r1, disc + 0x15 * 1200, 1200), "AN: re15_itemall_tile_raw(0x15) == Disc-Kachel");
    re15_ultimate15_set(0);

    /* ITPS: Regel-Pruefung (BOSS-Karte = Bild von Disc-Block 0), Puffer, Leser. */
    int boss_quelle = 0, echt_aus = 0;
    for (unsigned id = 0; id < 0x47; id++) {
        int istboss = !memcmp(idsc + id * 0x3000 + RE15_U15_ITPS_BILD, idsc + RE15_U15_ITPS_BILD, RE15_U15_ITPS_BILD_N);
        int benutzt = 0;
        for (unsigned j = 0; j < 0x48; j++) if (re15_ultimate15_bild_quelle(j) == (int)id) benutzt = 1;
        if (benutzt && istboss) boss_quelle++;
        if (!benutzt && !istboss) echt_aus++;
    }
    CHECK(boss_quelle == 0 && echt_aus == 0, "ITPS: jede Disc-Nicht-BOSS-Seite benutzt, keine BOSS-Seite (%d/%d)", boss_quelle, echt_aus);
    uint8_t *iu = (uint8_t *)malloc((size_t)imn);
    k = re15_ultimate15_itps_bauen(iu, imod, imn, idsc, idn);
    CHECK(k == 6 + 6, "ITPS: 6 Bilder + 6 Breitbilder eingesetzt (%d)", k);
    CHECK(!memcmp(iu + 0x15 * 0x3000, imod + 0x15 * 0x3000, RE15_U15_ITPS_CLUT) &&
          !memcmp(iu + 0x15 * 0x3000 + RE15_U15_ITPS_BILD, idsc + 0x15 * 0x3000 + RE15_U15_ITPS_BILD, RE15_U15_ITPS_BILD_N),
          "ITPS 0x15: Mod-Kopf (Rechtecke) + Disc-Bild");
    CHECK(!memcmp(iu + 0x3F * 0x3000 + RE15_U15_ITPS_BILD, idsc + 0x3E * 0x3000 + RE15_U15_ITPS_BILD, RE15_U15_ITPS_BILD_N),
          "ITPS 0x3F Pocket Watch <- Disc-Block 0x3E (zeigt die Uhr)");
    re15_itps_set_data(imod, imn);
    re15_itps_set_data_u15(iu, imn);
    int an_disc = 0, aus_mod = 0, n = 0;
    for (int v = 0; v < RE15_ITPS_H; v += 7) for (int u = 0; u < RE15_ITPS_W; u += 5) {
        uint8_t r, g, b, r2, g2, b2; n++;
        re15_ultimate15_set(0);
        int o0 = re15_itps_pixel(0x15, u, v, &r, &g, &b);
        uint8_t idx = imod[0x15 * 0x3000 + RE15_U15_ITPS_BILD + v * 112 + u];
        uint16_t c = (uint16_t)(imod[0x15 * 0x3000 + 0x14 + idx * 2] | (imod[0x15 * 0x3000 + 0x15 + idx * 2] << 8));
        if ((!c && !o0) || (o0 && r == ((c & 31) << 3))) aus_mod++;
        re15_ultimate15_set(1);
        int o1 = re15_itps_pixel(0x15, u, v, &r2, &g2, &b2);
        idx = idsc[0x15 * 0x3000 + RE15_U15_ITPS_BILD + v * 112 + u];
        c = (uint16_t)(idsc[0x15 * 0x3000 + 0x14 + idx * 2] | (idsc[0x15 * 0x3000 + 0x15 + idx * 2] << 8));
        if ((!c && !o1) || (o1 && r2 == ((c & 31) << 3))) an_disc++;
    }
    re15_ultimate15_set(0);
    CHECK(aus_mod == n && an_disc == n, "re15_itps_pixel(0x15): AUS = Mod %d/%d, AN = Disc %d/%d", aus_mod, n, an_disc, n);

    /* MIXITEM: Bild k == ITEMALL-Kachel des Ergebnisses (Disc), Puffer = Disc. */
    int mix_ok = 0;
    for (int i = 0; i < 14; i++) for (int t = 0; t < 72; t++) if (kachel_gleich(xdsc, i, disc, t)) { mix_ok++; break; }
    uint8_t *xu = (uint8_t *)malloc((size_t)xmn);
    k = re15_ultimate15_mixitem_bauen(xu, xmod, xmn, xdsc, xdn);
    CHECK(mix_ok == 14 && k == 14 && !memcmp(xu, xdsc, 16800), "MIXITEM: 14 Disc-Bilder (alle = Disc-ITEMALL-Kachel, %d)", mix_ok);
    free(u15); free(iu); free(xu);
}

/* ---- D Welt ---------------------------------------------------------------------------------------------------- */
static void teil_d(void)
{
    printf("\n== D Welt\n");
    static const struct { const char *rdt; int prop; unsigned item; } F[] = {
        { "STAGE1/ROOM1020.RDT", 4, 0x15 }, { "STAGE1/ROOM1020.RDT", 5, 0x15 },
        { "STAGE1/ROOM11B0.RDT", 1, 0x30 }, { "STAGE4/ROOM4010.RDT", 2, 0x22 }, { "STAGE4/ROOM4010.RDT", 3, 0x16 },
    };
    for (size_t i = 0; i < sizeof F / sizeof F[0]; i++) {
        int n = 0; uint8_t *d = lesen(F[i].rdt, &n);
        uint8_t out[RE15_RDT_MAX_PROPS];
        int k = re15_ultimate15_prop_items(d, n, out, RE15_RDT_MAX_PROPS);
        CHECK(d && k > 0 && out[F[i].prop] == F[i].item, "Mod %s obj %d -> Item 0x%02X (gefunden 0x%02X, %d Item_aot_set)",
              F[i].rdt, F[i].prop, F[i].item, d ? out[F[i].prop] : 0, k);
        free(d);
    }
    static const unsigned IT[4] = { 0x15, 0x16, 0x22, 0x30 };
    static const struct { const char *rdt; int prop; } HEUTE[4] = {
        { "STAGE1/ROOM1020.RDT", 4 }, { "STAGE4/ROOM4010.RDT", 3 }, { "STAGE4/ROOM4010.RDT", 2 }, { "STAGE1/ROOM11B0.RDT", 1 } };
    for (int i = 0; i < 4; i++) {
        const char *rel = NULL; int prop = -1;
        int hat = re15_ultimate15_modell_quelle(IT[i], &rel, &prop);
        int n = 0, hn = 0; uint8_t *d = hat ? lesen(rel, &n) : NULL, *h = lesen(HEUTE[i].rdt, &hn);
        static re15_rdt_t r, rh; memset(&r, 0, sizeof r); memset(&rh, 0, sizeof rh);
        int ok = d && h && re15_rdt_parse(d, (size_t)n, &r) == 0 && re15_rdt_parse(h, (size_t)hn, &rh) == 0 &&
                 prop < r.prop_count && r.prop_md1[prop] && r.prop_tim[prop];
        re15_md1_t m; re15_tim_t t; int md1ok = 0, timok = 0, anders = 0;
        if (ok) {
            md1ok = re15_md1_parse(r.prop_md1[prop], r.prop_md1_size[prop], &m) == 0 && m.mesh_count > 0;
            timok = re15_tim_parse(r.prop_tim[prop], r.prop_tim_size[prop], &t) == 0;
            const uint8_t *hm = rh.prop_md1[HEUTE[i].prop], *ht = rh.prop_tim[HEUTE[i].prop];
            anders = !(hm && rh.prop_md1_size[HEUTE[i].prop] == r.prop_md1_size[prop] &&
                       !memcmp(hm, r.prop_md1[prop], (size_t)r.prop_md1_size[prop]) &&
                       ht && rh.prop_tim_size[HEUTE[i].prop] == r.prop_tim_size[prop] &&
                       !memcmp(ht, r.prop_tim[prop], (size_t)r.prop_tim_size[prop]));
        }
        CHECK(hat && ok && md1ok && timok && anders, "Item 0x%02X: Disc-Modell %s obj %d lesbar (MD1 %d Teile, TIM %s), "
              "vom heutigen verschieden", IT[i], rel ? rel : "-", prop, md1ok ? m.mesh_count : 0, timok ? "ok" : "FEHLER");
        free(d); free(h);
    }
    CHECK(!re15_ultimate15_modell_quelle(0x17, NULL, NULL) && !re15_ultimate15_modell_quelle(0x21, NULL, NULL),
          "Magnum Bullets 0x17 (Disc-Modell == heute) und Memory Card 0x21 (kein Disc-Modell): kein Tausch");
    uint8_t out[RE15_RDT_MAX_PROPS];
    CHECK(re15_ultimate15_prop_items(NULL, 0, out, RE15_RDT_MAX_PROPS) == 0 && out[0] == 0xFF, "leere RDT: kein Fund");
}

/* ---- Werkzeug ---------------------------------------------------------------------------------------------------- */
static uint8_t *bmp_lesen(const char *p, int *w, int *h, int *off)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long g = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)g); if (fread(b, 1, (size_t)g, f) != (size_t)g) { fclose(f); free(b); return NULL; }
    fclose(f);
    *off = b[10] | (b[11] << 8) | (b[12] << 16); *w = b[18] | (b[19] << 8); *h = b[22] | (b[23] << 8);
    return b;
}
static int bmp_vergleich(int argc, char **argv, int gleich)
{
    if (argc < 8) return 2;
    int wa, ha, oa, wb, hb, ob;
    uint8_t *a = bmp_lesen(argv[2], &wa, &ha, &oa), *b = bmp_lesen(argv[3], &wb, &hb, &ob);
    if (!a || !b || wa != wb || ha != hb) { printf("BMP fehlt/ungleich gross\n"); return 2; }
    int x0 = atoi(argv[4]), y0 = atoi(argv[5]), x1 = atoi(argv[6]), y1 = atoi(argv[7]), diff = 0, n = 0;
    int pitch = (wa * 3 + 3) & ~3;
    for (int y = y0; y <= y1; y++) for (int x = x0; x <= x1; x++) {
        const uint8_t *pa = a + oa + (ha - 1 - y) * pitch + x * 3, *pb = b + ob + (hb - 1 - y) * pitch + x * 3;
        n++; if (memcmp(pa, pb, 3)) diff++;
    }
    printf("Rechteck (%d,%d)-(%d,%d): %d von %d Pixeln verschieden\n", x0, y0, x1, y1, diff, n);
    free(a); free(b);
    return gleich ? (diff == 0 ? 0 : 1) : (diff > 0 ? 0 : 1);
}

int main(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "bmp_gleich")) return bmp_vergleich(argc, argv, 1);
    if (argc > 1 && !strcmp(argv[1], "bmp_anders")) return bmp_vergleich(argc, argv, 0);
    teil_a();
    teil_b();
    teil_c();
    teil_d();
    printf("\nunit_optionen_r41: %d Pruefungen, %d Fehler\n", s_checks, s_fails);
    return s_fails ? 1 : 0;
}
