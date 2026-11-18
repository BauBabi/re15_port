/* test_r42_finale_wurf.c — Runde 42 Strang finale, Stufe 3 (include/re15_finale_wurf_r42.h): die Wurfszene (Runde 2: John am
 * Eingang, stehender Wurf, Flug bis neben den Spieler, Abgang durch die Tuer).
 *
 *   quelle    Jede Konstante gegen die Original-Bytes: RE2 ROOM7020.RDT (sub12 @0x0181A-0x019AE, sub13 @0x019B2-0x01A02,
 *             Item_aot_set @0x01202), RE2 STAGE7.BIN (Xa 0 @0x144), ROOM5090/5091.RDT (Tuer Slot 2 @0x0108E = Ankunft, Slot 4
 *             @0x010CE = Tuer-AOT des Abgangs), RE1.5 PSX.EXE (Heben Clip 6 @0x80032f68, Wurf Clip 7 @0x800334b0, Loslassbild 22
 *             @0x800336fc; Version_40% bytegleich, nicht versioniert - im Bericht gemessen).
 *   programm  Das Port-Programm = sub12 Befehl fuer Befehl (Schnitt 13, John Typ 0x49 an der Ankunft mit Gier zum Spieler,
 *             Plc_motion(1,24) Heben, Sleeps 1/30, Plc_motion(1,25) Wurf, Warten aufs Loslassen, Zuruf Id 3, Landung, Sleep 15,
 *             Reaktion Schnitt 13, Sleep 1, Plc_dest Modus 5 (laufen) zur Tuer-Vorderkante, zurueck auf die gemerkte Kamera VOR dem Parken, Item_aot_set Slot 6
 *             Gegenstand 0x12 Anzahl 2 Taken-Bit 137, Ende wie sub18).
 *   bahn      Die Flugschleife (Add_speed, Add_aspeed, Ende y >= -800): mit Adas Werten vom Steg genau 28 Bilder (10 n^2 + 10 n >=
 *             7850); die Startwerte des Ports landen nach genau 28 Bildern an der Landestelle (kurze und lange Wuerfe); die
 *             Landestelle liegt so weit vor dem Spieler (Richtung John), wie das RE2-Aufnahme-Rechteck 3400x2500 ihn mit Rand 100
 *             einschliesst (laengs des Wagens 1600), hoechstens auf halber Strecke.
 *   luecken   Aufnahme des Werfers mit belegten Zellen 8/9 und Luecken davor: nichts geht verloren, Werfer in 0/1 mit Anzahl 2;
 *             Negativ-Kontrolle: derselbe Einfuegeweg OHNE das Schliessen verliert 8/9 (byte-true FUN_8004dc4c); andere Aufnahmen
 *             (anderes Taken-Bit) bleiben unberuehrt.
 *   modelle   JOHN_49_WURF.EMD: Paar 1 hat 26 Clips, Clip 24/25 = PL00W09.PLW Clip 6/7 (Keyframes bytegleich), Clips 0..23 und
 *             ihre Keyframes bytegleich mit dem 0x49-Block; Werfer = PL00W12.PLW dir[2] Waffenband (38 Dreiecke, v >= 224, u 72..127,
 *             Rohr y -744..1027 = RE15_FW_W12_LAENGE), dir[3] 8 bpp 56x32, bei PL04W12 bytegleich (Runde 3).
 *   ersatz    John-Ersatzmodell nur fuer 0x49 in 5090/5091.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_inventory.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_story_r42.h"
#include "re15_finale_wurf_r42.h"
#include "re15_math.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

extern unsigned g_current_room_id;
static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}
static int16_t s16(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }
static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static int hex_gleich(const uint8_t *b, size_t n, size_t off, const char *hex)
{
    size_t k = 0;
    for (const char *h = hex; h[0] && h[1]; h += 2) {
        while (*h == ' ') h++;
        if (!h[0] || !h[1]) break;
        unsigned v; if (sscanf(h, "%2x", &v) != 1) return 0;
        if (off + k >= n || b[off + k] != (uint8_t)v) return 0;
        k++;
    }
    return 1;
}

static void t_quelle(void)
{
    size_t n = 0, ns = 0, n50 = 0, n51 = 0;
    uint8_t *r = slurp(RE15_REPO_ROOT "/info/re2leon/PL0/RDT/ROOM7020.RDT", &n);
    uint8_t *st = slurp(RE15_REPO_ROOT "/info/re2leon/COMMON/BIN/STAGE7.BIN", &ns);
    uint8_t *a = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/STAGE5/ROOM5090.RDT", &n50);
    uint8_t *b = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/STAGE5/ROOM5091.RDT", &n51);
    CHECK(r && st && a && b, "Quellen fehlen (ROOM7020.RDT / STAGE7.BIN / ROOM5090/5091.RDT)");
    if (r && st && a && b) {
        CHECK(hex_gleich(r, n, 0x01878, "3f0a1240") && hex_gleich(r, n, 0x018AA, "3f0a1240"), "Plc_motion @0x01878/@0x018AA");
        CHECK(hex_gleich(r, n, 0x019B6, "2f020a00 2f011400 2f001900") && r[0x019B8] == RE15_FW_FLUG_VZ && r[0x019BC] == RE15_FW_FLUG_VY &&
              r[0x019C0] == RE15_FW_FLUG_VX, "Speed_set v (25,20,10) @0x019B6-BE");
        CHECK(r[0x0187E] == RE15_FW_SCHLAF_1 && r[0x01894] == RE15_FW_SCHLAF_30 && r[0x018B0] == RE15_FW_SCHLAF_20 &&
              r[0x018D2] == RE15_FW_SCHLAF_15 && r[0x01900] == RE15_FW_SCHLAF_1, "Sleeps 1/30/20/15/1 @0x0187C/92/AE/D0/FE");
        CHECK(hex_gleich(r, n, 0x01854, "4800818a 0000 5802 ceff 80fc 0000 0004"), "Super_set @0x01854");
        CHECK(r[0x01857] == (0x80 | RE15_FW_ADA_TEIL) && s16(r + 0x0185A) == RE15_FW_ADA_VERSATZ_Y &&
              s16(r + 0x0185C) == RE15_FW_ADA_VERSATZ_Z && s16(r + 0x0185E) == RE15_FW_ADA_HAND_RX && s16(r + 0x01862) == RE15_FW_ADA_HAND_RZ,
              "Super_set-Werte (RE2-Beleg Adas Teil 10; Port Teil 11 = Leons Griff)");
        CHECK(RE15_FW_TEIL == 11, "Port-Teil = rechte Hand 11 (Waffenteil)");
        /* Runde 3, Abgang: RE2 schneidet ZURUECK (@0x01948), BEVOR Ada geparkt wird (@0x01954) - Reihenfolge an den Bytes */
        CHECK(hex_gleich(r, n, 0x0193A, "40001523 40066000") && hex_gleich(r, n, 0x01942, "0b0c 1d040100 2900") &&
              hex_gleich(r, n, 0x0194A, "46050231 11000200 2800 2e020000 3200a092 c8ce14ba"),
              "Abgang sub12: Plc_dest @0x0193A, Wsleep @0x01942, Work_copy/Cut_chg @0x01944/48, Aot_reset @0x0194A, Pos_set Parken @0x01954");
        CHECK(s16(r + 0x018B8) == RE15_FW_FLUG_RX && s16(r + 0x018BA) == RE15_FW_FLUG_RY && s16(r + 0x018BC) == RE15_FW_FLUG_RZ,
              "Super_reset @0x018B6");
        CHECK(s16(r + 0x019FC) == RE15_FW_LIEG_RX && s16(r + 0x01A00) == RE15_FW_LIEG_RZ, "Dir_set Landung @0x019FA");
        CHECK(s16(r + 0x01874) == RE15_FW_ADA_GIER, "Adas Gier @0x01870");
        CHECK(hex_gleich(r, n, 0x019C2, "2f080000 2f071400 2f060500"), "Speed_set a @0x019C2-CA");
        CHECK(r[0x019CC] == RE15_FW_FLUG_AX && r[0x019C8] == RE15_FW_FLUG_AY && r[0x019C4] == RE15_FW_FLUG_AZ, "a (5,20,0)");
        CHECK(hex_gleich(r, n, 0x019D8, "3e000c03e0fc") && s16(r + 0x019DC) == -RE15_FW_FLUG_STOP, "Member_cmp y < -800 @0x019D8");
        CHECK(r[0x019E2] == 0x36 && r[0x019E4] == RE15_FW_SE_LANDUNG, "Se_on Raum 0x0E @0x019E2");
        CHECK(hex_gleich(r, n, 0x01202, "4e050231 0000 a899 60a5 480d c409 1100 0200 2800 0001"), "Item_aot_set @0x01202");
        CHECK(s16(r + 0x0120C) == RE15_FW_AOT_W && s16(r + 0x0120E) == RE15_FW_AOT_D && s16(r + 0x01212) == RE15_FW_ANZAHL,
              "Rechteck 3400x2500, Anzahl 2 @0x01212");
        /* Xa 0 / Xa 1 -> Bilder: (Laenge*16 - 30) Sektoren / 150 je s * 30 Bilder, aufgerundet */
        CHECK(hex_gleich(st, ns, 0x144, "06005c001800") && hex_gleich(st, ns, 0x14A, "040054001d00"), "STAGE7.BIN @0x144/@0x14A");
        const int x0 = (s16(st + 0x148) * 16 - 30), x1 = (s16(st + 0x14E) * 16 - 30);
        CHECK((x0 * 30 + 149) / 150 == RE15_FW_ZURUF_BILDER, "Xa 0: %d Sektoren -> %d Bilder", x0, (x0 * 30 + 149) / 150);
        (void)x1;
        CHECK(memcmp(a + 0x0108E, b + 0x0108E, 32) == 0, "Tuer Slot 2 5090 != 5091");
        CHECK(a[0x0108E] == 0x3b && a[0x0108F] == 2 && s16(a + 0x0109C) == RE15_FW_JOHN_X && s16(a + 0x0109E) == 0 &&
              s16(a + 0x010A0) == RE15_FW_JOHN_Z && (s16(a + 0x010A2) & 0xfff) == RE15_FW_JOHN_GIER && a[0x010A6] == 14,
              "Ankunft Wagen C @0x0108E: (%d,%d,%d) %d Schnitt %d", s16(a + 0x0109C), s16(a + 0x0109E), s16(a + 0x010A0),
              s16(a + 0x010A2), a[0x010A6]);
        /* Tuer-AOT des Abgangs: Slot 4 @0x010CE (x 25150 w 900, z -25300 d 3950), Ziel Wagen B; 5091 bytegleich */
        CHECK(memcmp(a + 0x010CE, b + 0x010CE, 32) == 0, "Tuer Slot 4 5090 != 5091");
        CHECK(a[0x010CE] == 0x3b && a[0x010CF] == 4 && s16(a + 0x010D4) == RE15_FW_TUER_X0 && s16(a + 0x010D6) == RE15_FW_TUER_Z0 &&
              s16(a + 0x010D8) == RE15_FW_TUER_W && s16(a + 0x010DA) == RE15_FW_TUER_D &&
              RE15_FW_TUER_X == RE15_FW_TUER_X0 && RE15_FW_TUER_Z == RE15_FW_TUER_Z0 + RE15_FW_TUER_D / 2,
              "Tuer-AOT Slot 4 @0x010CE (%d,%d %dx%d)", s16(a + 0x010D4), s16(a + 0x010D6), s16(a + 0x010D8), s16(a + 0x010DA));
        /* AOT-Slot 6 frei: main00 belegt 0..5 (Door_aot_set @0x0104E + 32 i) */
        for (int i = 0; i < 6; i++) CHECK(a[0x0104E + 32 * i] == 0x3b && a[0x0104F + 32 * i] == i, "5090 main00 Slot %d", i);
    }
    free(r); free(st); free(a); free(b);
    /* RE1.5 PSX.EXE (Datei = 0x800 + Adresse - 0x80010000): Heben, Wurf MITTE, Loslassbild 22 - Mod-Baum und echte Disc */
    /* Version_40%/PSX.EXE (echte Disc, nicht versioniert) traegt an allen drei Stellen dieselben Bytes (xxd, Bericht s3 r2) */
    static const char *exes[1] = { RE15_REPO_ROOT "/info/Re1.5/PSX.EXE" };
    for (int k = 0; k < 1; k++) {
        size_t ne = 0;
        uint8_t *e = slurp(exes[k], &ne);
        CHECK(e != NULL, "%s fehlt", exes[k]);
        if (!e) continue;
        CHECK(hex_gleich(e, ne, 0x800 + 0x80032f68 - 0x80010000, "06000234"),
              "%s: Heben `ori v0,zero,0x6` @0x80032f68", exes[k]);
        CHECK(hex_gleich(e, ne, 0x800 + 0x800334b0 - 0x80010000, "07004224"), "%s: Wurf `addiu v0,v0,7` @0x800334b0", exes[k]);
        CHECK(hex_gleich(e, ne, 0x800 + 0x800336fc - 0x80010000, "16000234 13006214") &&
              e[0x800 + 0x800336fc - 0x80010000] == RE15_FW_LOS_BILD, "%s: Loslassbild `ori v0,zero,0x16` @0x800336fc", exes[k]);
        /* Runde 3: NPC-Plc_dest-Subs @0x80076ca0: [4] = 0x80051148 Gehen, [5] = 0x80051484 Laufen (RE15_FW_GEHEN_MODUS = 5) */
        const size_t ot = 0x800 + 0x80076ca0 - 0x80010000;
        CHECK(ot + 24 <= ne && u32(e + ot + 16) == 0x80051148u && u32(e + ot + 4 * RE15_FW_GEHEN_MODUS) == 0x80051484u,
              "%s: NPC-Sub-Tabelle @0x80076ca0 [4]/[5]", exes[k]);
        free(e);
    }
}

static void t_programm(void)
{
    g_scd.work_vars[0x0A] = 12;
    re15_finale_wurf_reset();
    int n = 0;
    const uint8_t *p = re15_finale_wurf_programm(&n);
    /* erwartete Folge, je Satz die Bytes (Kopf re15_finale_wurf_r42.h); Gier im Riegel = Ankunfts-Gier 0x0800 */
    static const uint8_t soll_kopf[] = {
        0x22, 0x02, 0x07, 0x01, 0x22, 0x01, 0x1b, 0x01, 0x29, RE15_FW_CUT_WURF,
        0x44, RE15_FW_JOHN_SLOT, RE15_FW_JOHN_TYP, RE15_FW_JOHN_GRID, 0x00, 0x00, 0x00, 0xff,
        0xe0, 0x60, 0x00, 0x00, 0x02, 0xa4, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
        0x2e, 0x02, RE15_FW_JOHN_SLOT, 0x00, 0x3f, 0x01, RE15_FW_CLIP_HEBEN, 0x00, 0x09, 0x0a, 0x01, 0x00,
        0x09, 0x0a, 0x1e, 0x00, 0x3f, 0x01, RE15_FW_CLIP_WURF, 0x00,
        0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_FW_SIG_WURF, 0x00,
        0x2b, RE15_FW_MSG_ZURUF, 0x00, 0x00,
        0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_FW_SIG_GELANDET, 0x00,
        0x09, 0x0a, 0x0f, 0x00, 0x22, 0x05, RE15_FW_SIG_REAKTION, 0x01, 0x29, RE15_FW_CUT_REAKTION, 0x09, 0x0a, 0x01, 0x00,
        0x40, 0x00, RE15_FW_GEHEN_MODUS, RE15_FW_SIG_TUER, 0x3e, 0x62, 0xe3, 0xa4,   /* (25150, -23325) Vorderkante Tuer-AOT */
        0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_FW_SIG_TUER, 0x00,
        0x29, 12 };
    CHECK(n > (int)sizeof soll_kopf && memcmp(p, soll_kopf, sizeof soll_kopf) == 0, "Programm (sub12 @0x0181A-0x01948) weicht ab");
    if (n > (int)sizeof soll_kopf)
        for (int i = 0; i < (int)sizeof soll_kopf; i++)
            if (p[i] != soll_kopf[i]) { fprintf(stderr, "  erste Abweichung Byte %d: %02x statt %02x\n", i, p[i], soll_kopf[i]); break; }
    const int o = (int)sizeof soll_kopf;
    const uint8_t *it = p + o;
    CHECK(it[0] == 0x50 && it[1] == RE15_FW_AOT_SLOT && it[2] == 0x09 && it[3] == 0x31 && s16(it + 10) == RE15_FW_AOT_W &&
          s16(it + 12) == RE15_FW_AOT_D && s16(it + 14) == RE15_FW_ITEM && s16(it + 16) == RE15_FW_ANZAHL &&
          s16(it + 18) == RE15_R42_BIT_WERFER_FINALE && it[20] == 0xff, "Item_aot_set (Form ROOM40A1 @0x00ED0) weicht ab");
    static const uint8_t soll_ende[] = { 0x22, 0x05, RE15_FW_SIG_ENDE, 0x01, 0x09, 0x0a, 0x01, 0x00, 0x2e, 0x01, 0x00, 0x00, 0x42, 0x00,
                                         0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x3c, 0x01, 0x01, 0x00 };
    CHECK(n == o + 22 + (int)sizeof soll_ende && memcmp(it + 22, soll_ende, sizeof soll_ende) == 0,
          "Programm-Ende (@0x01968-0x019AE, sub18 @0x01B54-0x01B60) weicht ab (Laenge %d)", n);
    printf("programm: %d Bytes, sub12 in RE1.5-Opcodes\n", n);
}

static void t_bahn(void)
{
    /* Ada vom Steg: Hand im Loslassbild (Clip 0x12 Bild 20, Ada-Gerippe, Gier 3700) ~ y -8650; RE2 v (25,20,10) a (5,20,0) Weltachsen */
    const int32_t s[3] = { 0, -8650, 0 }, v[3] = { RE15_FW_FLUG_VX, RE15_FW_FLUG_VY, RE15_FW_FLUG_VZ };
    int32_t e[3]; int nb = 0;
    const int32_t a2[3] = { RE15_FW_FLUG_AX, RE15_FW_FLUG_AY, RE15_FW_FLUG_AZ };
    re15_finale_wurf_bahn(s, v, a2, e, &nb);
    CHECK(nb == RE15_FW_FLUG_BILDER, "Steg-Fall %d Bilder statt 28 (10n^2+10n >= 7850)", nb);
    CHECK(e[1] == -8650 + 10 * 28 * 28 + 10 * 28 && e[0] == 25 * 28 + 5 * 28 * 27 / 2 && e[2] == 10 * 28,
          "Steg-Fall Ende (%d,%d,%d)", e[0], e[1], e[2]);
    /* Port: von der Hand (Bild 22 des Wurfs, ~2 m hoch) zur Landestelle neben dem Spieler - nach genau 28 Bildern, <= 30 daneben */
    static const int32_t faelle[][5] = {   /* Start x, y, z, Spieler x, z */
        { 24120, -2010, -23120,  8150, -23550 },     /* Integrationslauf Leon (Spieler im Schnitt 12) */
        { 24120, -2010, -23120, 21000, -23000 },     /* kurz: Spieler 3,8 m vor John */
        { 24120, -2010, -23120, 23300, -24200 },     /* sehr kurz: halbe Strecke */
    };
    for (unsigned k = 0; k < sizeof faelle / sizeof faelle[0]; k++) {
        const int32_t *f = faelle[k];
        int32_t lx, lz, vv[3], aa[3]; int16_t g;
        re15_finale_wurf_landestelle(f[3], f[4], RE15_FW_JOHN_X, RE15_FW_JOHN_Z, &lx, &lz);
        const int32_t ddx = lx - f[3], ddz = lz - f[4], djx = RE15_FW_JOHN_X - f[3], djz = RE15_FW_JOHN_Z - f[4];
        const int32_t dl = (int32_t)re15_squareroot0((uint32_t)(ddx * ddx + ddz * ddz));
        const int32_t dj = (int32_t)re15_squareroot0((uint32_t)((int64_t)djx * djx + (int64_t)djz * djz));
        /* Spieler im Aufnahme-Rechteck um die Landestelle, mit Rand; hoechstens halbe Strecke; laengs des Wagens (Fall 0) = 1600 */
        CHECK(abs(ddx) <= RE15_FW_AOT_W / 2 - RE15_FW_LANDUNG_RAND + 2 && abs(ddz) <= RE15_FW_AOT_D / 2 - RE15_FW_LANDUNG_RAND + 2 &&
              dl <= dj / 2 + 2 && (k != 0 || (dl >= 1598 && dl <= 1602)), "Fall %u: Landestelle (%d,%d) %d vom Spieler", k, lx, lz, dl);
        re15_finale_wurf_startwerte(f, lx, lz, vv, aa, &g);
        re15_finale_wurf_bahn(f, vv, aa, e, &nb);
        const int32_t fx = e[0] - lx, fz = e[2] - lz;
        CHECK(nb == RE15_FW_FLUG_BILDER && fx * fx + fz * fz <= 30 * 30 && e[1] >= -RE15_FW_FLUG_STOP,
              "Fall %u: %d Bilder, Ende (%d,%d,%d) statt (%d,%d)", k, nb, e[0], e[1], e[2], lx, lz);
        printf("bahn: Fall %u Spieler (%d,%d) -> Landung (%d,%d) nach %d Bildern, v (%d,%d,%d) a (%d,%d,%d)\n", k, f[3], f[4], e[0], e[2],
               nb, vv[0], vv[1], vv[2], aa[0], aa[1], aa[2]);
    }
    CHECK(re15_finale_wurf_vy(-2010) < 0, "v_y aus Handhoehe muss nach oben zeigen");
}

static void inv_setzen(const uint8_t ids[10])
{
    re15_inv_init();
    for (int i = 0; i < 10; i++) { g_inv.slots[i].id = ids[i]; g_inv.slots[i].qty = ids[i] ? (uint8_t)(i + 1) : 0; }
    re15_inv_set_equipped_slot(0x80);
}
static void t_luecken(void)
{
    /* Zellen: 0 Pistole, 1 leer, 2 Munition, 3 leer, 4..7 belegt, 8/9 belegt (Schluessel/Kraut) */
    static const uint8_t ids[10] = { 0x03, 0x00, 0x15, 0x00, 0x30, 0x31, 0x32, 0x33, 0x40, 0x41 };
    inv_setzen(ids);
    re15_finale_wurf_vor_aufnahme(RE15_FW_ITEM, RE15_R42_BIT_WERFER_FINALE);
    CHECK(re15_inv_grant(RE15_FW_ITEM, RE15_FW_ANZAHL) == 0, "Aufnahme abgelehnt");
    CHECK(g_inv.slots[0].id == RE15_FW_ITEM && g_inv.slots[1].id == RE15_FW_ITEM && g_inv.slots[0].qty == 2 && g_inv.slots[1].qty == 2,
          "Werfer nicht in 0/1 mit Anzahl 2 (%02x/%02x %d)", g_inv.slots[0].id, g_inv.slots[1].id, g_inv.slots[0].qty);
    for (int i = 0; i < 10; i++) {
        if (!ids[i]) continue;
        int da = 0;
        for (int k = 0; k < 10; k++) if (g_inv.slots[k].id == ids[i] && g_inv.slots[k].qty == i + 1) da = 1;
        CHECK(da, "Gegenstand 0x%02x (Zelle %d) verloren", ids[i], i);
    }
    /* Negativ-Kontrolle: ohne Schliessen verliert der byte-true Schub die Zellen 8/9 */
    inv_setzen(ids);
    re15_inv_grant(RE15_FW_ITEM, RE15_FW_ANZAHL);
    int verloren = 0;
    for (int i = 8; i < 10; i++) { int da = 0; for (int k = 0; k < 10; k++) if (g_inv.slots[k].id == ids[i]) da = 1; verloren += !da; }
    CHECK(verloren == 2, "Negativ-Kontrolle: ohne Schliessen gingen %d statt 2 Zellen verloren (Riegel waere blind)", verloren);
    /* andere Aufnahme (anderes Taken-Bit): unberuehrt */
    inv_setzen(ids);
    re15_finale_wurf_vor_aufnahme(RE15_FW_ITEM, 45);
    CHECK(g_inv.slots[1].id == 0 && g_inv.slots[3].id == 0 && g_inv.slots[8].id == 0x40, "fremde Aufnahme hat das Inventar veraendert");
    printf("luecken: 8/9 bleiben, Negativ-Kontrolle verliert %d\n", verloren);
}

static void t_modelle(void)
{
    size_t n = 0, nr = 0, nm = 0, nt = 0;
    uint8_t *j = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/../RE2/JOHN_49_WURF.EMD", &n);
    uint8_t *r = slurp(RE15_REPO_ROOT "/info/re2leon/PL0/RDT/ROOM7020.RDT", &nr);
    size_t nw = 0;
    uint8_t *w9 = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/PLD/PL00W09.PLW", &nw);
    uint8_t *m = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/" RE15_FW_W12_DATEI, &nm);
    uint8_t *t = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/PLD/PL04W12.PLW", &nt);
    CHECK(j && r && m && t, "JOHN_49_WURF.EMD / ROOM7020.RDT / PL00W12.PLW / PL04W12.PLW fehlen");
    CHECK(w9 != NULL, "PL00W09.PLW fehlt");
    if (j && w9) {
        re15_emd_skeleton_t sk; re15_emd_animation_t an;
        memset(&sk, 0, sizeof sk); memset(&an, 0, sizeof an);
        CHECK(re15_emd_parse_loco_bank(j, n, &sk, &an) == 0, "Paar 1 nicht lesbar");
        CHECK(an.clip_count == 26 && an.clips[RE15_FW_CLIP_HEBEN].frame_count == 10 && an.clips[RE15_FW_CLIP_WURF].frame_count == 35,
              "Paar 1: %d Clips, Clip 24 %d / 25 %d Bilder", an.clip_count, an.clip_count > 24 ? an.clips[24].frame_count : -1,
              an.clip_count > 25 ? an.clips[25].frame_count : -1);
        if (an.clip_count == 26 && sk.keyframe_size_bytes == 80) {
            /* PL00W09.PLW: [u32 dir_off][u32 n] .. dir[0] = EDD, dir[1] = EMR; EDD-Tabelle (u16 Bilder, u16 Offset) je Clip, Bild-Eintraege
             * u32; EMR-Kopf (u16 0, u16 Keyframe-Offset, u16 15, u16 80) */
            const uint32_t dof = u32(w9), d0 = u32(w9 + dof), d1 = u32(w9 + dof + 4);
            const uint8_t *edd = w9 + d0, *emr = w9 + d1;
            const uint8_t *kd = emr + s16(emr + 2);
            static const int quelle[2][2] = { { 6, RE15_FW_CLIP_HEBEN }, { 7, RE15_FW_CLIP_WURF } };
            int ok = 1;
            for (int q = 0; q < 2; q++) {
                const int cnt = s16(edd + 4 * quelle[q][0]), off = s16(edd + 4 * quelle[q][0] + 2);
                if (cnt != an.clips[quelle[q][1]].frame_count) ok = 0;
                for (int f = 0; f < cnt && ok; f++) {
                    const uint32_t e = u32(edd + off + 4 * f);
                    const uint32_t w = re15_emd_get_frame_entry(&an, quelle[q][1], f);
                    if (memcmp(kd + (e & 0xfff) * 80, sk.keyframe_data + (w & 0xfff) * 80, 80) != 0 || (w & ~0xfffu) != (e & ~0xfffu)) ok = 0;
                }
            }
            CHECK(ok, "Clips 24/25 != PL00W09.PLW Clip 6/7 (Keyframes/Flags)");
        }
    }
    if (m && t) {
        /* PLW: u32 dir_off, dir = 4 x u32 (EDD, EMR, MD1, TIM); TIM endet am Verzeichnis */
        const uint32_t dm = u32(m), m2 = u32(m + dm + 8), m3 = u32(m + dm + 12);
        const uint32_t dt = u32(t), t3 = u32(t + dt + 12);
        re15_md1_t md; memset(&md, 0, sizeof md);
        CHECK(m2 < m3 && m3 < dm && re15_md1_parse(m + m2, (int)(m3 - m2), &md) == 0 && md.mesh_count == 1, "PL00W12 dir[2] nicht lesbar");
        if (md.mesh_count == 1) {
            const re15_md1_mesh_t *hm = &md.meshes[0];
            int band = 0, rest_ok = 1, ymin = 99999, ymax = -99999, umin = 999, umax = -1, vmax = -1;
            for (int i = 0; i < hm->triangle_count; i++) {
                const re15_md1_tri_uv_t *uv = &hm->triangle_uvs[i];
                const int ist = uv->v0 >= RE15_FW_W12_V_MIN && uv->v1 >= RE15_FW_W12_V_MIN && uv->v2 >= RE15_FW_W12_V_MIN;
                const int hand = uv->v0 < RE15_FW_W12_V_MIN && uv->v1 < RE15_FW_W12_V_MIN && uv->v2 < RE15_FW_W12_V_MIN;
                if (!ist && !hand) rest_ok = 0;
                if (!ist) continue;
                band++;
                const uint8_t us[3] = { uv->u0, uv->u1, uv->u2 }, vs[3] = { uv->v0, uv->v1, uv->v2 };
                const uint32_t vi[3] = { hm->triangles[i].v0, hm->triangles[i].v1, hm->triangles[i].v2 };
                for (int k = 0; k < 3; k++) {
                    if (us[k] < umin) umin = us[k];
                    if (us[k] > umax) umax = us[k];
                    if (vs[k] > vmax) vmax = vs[k];
                    const int y = hm->tri_vertices[vi[k]].y;
                    if (y < ymin) ymin = y;
                    if (y > ymax) ymax = y;
                }
            }
            CHECK(hm->triangle_count == 72 && band == 38 && rest_ok, "PL00W12: %d Dreiecke, Waffenband %d (soll 72/38, keine gemischten)",
                  hm->triangle_count, band);
            CHECK(umin == RE15_FW_W12_U0 && umax == 127 && vmax == 255, "Waffenband UV u %d..%d v ..%d (soll 72..127, ..255)", umin, umax, vmax);
            CHECK(ymin == -744 && ymax == 1027 && RE15_FW_W12_LAENGE == ymax - ymin, "Rohr y %d..%d", ymin, ymax);
        }
        re15_tim_t tm; memset(&tm, 0, sizeof tm);
        CHECK(re15_tim_parse(m + m3, (int)(dm - m3), &tm) == 0 && tm.bpp == 8 && tm.width == 56 && tm.height == 32,
              "PL00W12 dir[3] nicht 8 bpp 56x32 (%d bpp %dx%d)", tm.bpp, tm.width, tm.height);
        CHECK(dm - m3 == dt - t3 && memcmp(m + m3, t + t3, dm - m3) == 0, "PL04W12 dir[3] != PL00W12 dir[3]");
        printf("modelle: Werfer = PL00W12 Waffenband, TIM %d bpp %dx%d\n", tm.bpp, tm.width, tm.height);
    }
    free(j); free(r); free(m); free(t); free(w9);
}

static void t_ersatz(void)
{
    g_current_room_id = 0x5090u;
    CHECK(re15_finale_wurf_emd_datei(0x49) && strcmp(re15_finale_wurf_emd_datei(0x49), "JOHN_49_WURF.EMD") == 0, "5090 0x49");
    CHECK(re15_finale_wurf_emd_datei(0x40) == NULL, "5090 0x40");
    g_current_room_id = 0x5091u;
    CHECK(re15_finale_wurf_emd_datei(0x49) != NULL, "5091 0x49");
    g_current_room_id = 0x6031u;
    CHECK(re15_finale_wurf_emd_datei(0x49) == NULL, "6031 0x49 (John im Zug bleibt das Auslieferungsmodell)");
    g_current_room_id = 0;
}

int main(void)
{
    t_quelle();
    t_programm();
    t_bahn();
    t_luecken();
    t_modelle();
    t_ersatz();
    if (fails) { fprintf(stderr, "test_r42_finale_wurf: %d Fehler\n", fails); return 1; }
    printf("test_r42_finale_wurf: OK\n");
    return 0;
}
