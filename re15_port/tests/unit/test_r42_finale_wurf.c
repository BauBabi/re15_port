/* test_r42_finale_wurf.c — Runde 42 Strang finale, Stufe 3 (include/re15_finale_wurf_r42.h): die Wurfszene.
 *
 *   quelle    Jede Konstante gegen die Original-Bytes: RE2 ROOM7020.RDT (sub12 @0x0181A-0x019AE, sub13 @0x019B2-0x01A02,
 *             Item_aot_set @0x01202, RBJ @0x2E34), RE2 STAGE7.BIN (Xa 0 @0x144, Xa 1 @0x14A), ROOM5090/5091.RDT (Tuer Slot 2 @0x0108E).
 *   programm  Das Port-Programm = sub12 Befehl fuer Befehl (Reihenfolge, Sleeps 1/30/20/15/1/87, Schnitte 14/15/zurueck, John Typ 0x49
 *             an die Ankunft, Plc_motion(1,24) zweimal, Zuruf Id 3, Item_aot_set Slot 6 Gegenstand 0x12 Anzahl 2 Taken-Bit 137 ohne
 *             Modell, Ende wie sub18).
 *   bahn      Die Flugschleife (Add_speed, Add_aspeed, Ende y >= -800): mit Adas Werten vom Steg genau 28 Bilder (geschlossene Form
 *             10 n^2 + 10 n >= 7850), aus Johns Standhoehe mit dem gemessenen Handweg 8 Bilder.
 *   luecken   Aufnahme des Werfers mit belegten Zellen 8/9 und Luecken davor: nichts geht verloren, Werfer in 0/1 mit Anzahl 2;
 *             Negativ-Kontrolle: derselbe Einfuegeweg OHNE das Schliessen verliert 8/9 (byte-true FUN_8004dc4c); andere Aufnahmen
 *             (anderes Taken-Bit) bleiben unberuehrt.
 *   modelle   JOHN_49_WURF.EMD: Paar 1 hat 25 Clips, Clip 24 = 55 Bilder mit den Keyframes des RBJ-Clips 0x12 (bytegleich),
 *             Clips 0..23 bytegleich mit dem 0x49-Block; R702_WERFER.MD1/.TIM = ROOM7020 @0x2044..0x2788 / @0x24F9C..Ende.
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
        CHECK(r[0x0187E] == RE15_FW_SCHLAF_1 && r[0x01894] == RE15_FW_SCHLAF_30 && r[0x018B0] == RE15_FW_SCHLAF_20 &&
              r[0x018D2] == RE15_FW_SCHLAF_15 && r[0x01900] == RE15_FW_SCHLAF_1, "Sleeps 1/30/20/15/1 @0x0187C/92/AE/D0/FE");
        CHECK(hex_gleich(r, n, 0x01854, "4800818a 0000 5802 ceff 80fc 0000 0004"), "Super_set @0x01854");
        CHECK(r[0x01857] == (0x80 | RE15_FW_TEIL) && s16(r + 0x0185A) == RE15_FW_VERSATZ_Y && s16(r + 0x0185C) == RE15_FW_VERSATZ_Z &&
              s16(r + 0x0185E) == RE15_FW_HAND_RX && s16(r + 0x01862) == RE15_FW_HAND_RZ, "Super_set-Werte");
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
        CHECK((x1 * 30 + 149) / 150 == RE15_FW_REAKTION_BILDER, "Xa 1: %d Sektoren -> %d Bilder", x1, (x1 * 30 + 149) / 150);
        CHECK(memcmp(a + 0x0108E, b + 0x0108E, 32) == 0, "Tuer Slot 2 5090 != 5091");
        CHECK(a[0x0108E] == 0x3b && a[0x0108F] == 2 && s16(a + 0x0109C) == RE15_FW_JOHN_X && s16(a + 0x0109E) == 0 &&
              s16(a + 0x010A0) == RE15_FW_JOHN_Z && (s16(a + 0x010A2) & 0xfff) == RE15_FW_JOHN_GIER && a[0x010A6] == RE15_FW_CUT_WURF,
              "Ankunft Wagen C @0x0108E: (%d,%d,%d) %d Schnitt %d", s16(a + 0x0109C), s16(a + 0x0109E), s16(a + 0x010A0),
              s16(a + 0x010A2), a[0x010A6]);
        /* AOT-Slot 6 frei: main00 belegt 0..5 (Door_aot_set @0x0104E + 32 i) */
        for (int i = 0; i < 6; i++) CHECK(a[0x0104E + 32 * i] == 0x3b && a[0x0104F + 32 * i] == i, "5090 main00 Slot %d", i);
    }
    free(r); free(st); free(a); free(b);
}

static void t_programm(void)
{
    g_scd.work_vars[0x0A] = 12;
    re15_finale_wurf_reset();
    int n = 0;
    const uint8_t *p = re15_finale_wurf_programm(&n);
    /* erwartete Folge, je Satz die Bytes (Kopf re15_finale_wurf_r42.h) */
    static const uint8_t soll_kopf[] = {
        0x22, 0x02, 0x07, 0x01, 0x22, 0x01, 0x1b, 0x01, 0x29, RE15_FW_CUT_WURF,
        0x44, RE15_FW_JOHN_SLOT, RE15_FW_JOHN_TYP, RE15_FW_JOHN_GRID, 0x00, 0x00, 0x00, 0xff,
        0xe0, 0x60, 0x00, 0x00, 0x02, 0xa4, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
        0x2e, 0x02, RE15_FW_JOHN_SLOT, 0x00, 0x3f, 0x01, RE15_FW_CLIP_WURF, 0x00, 0x09, 0x0a, 0x01, 0x00,
        0x2b, RE15_FW_MSG_ZURUF, 0x00, 0x00, 0x09, 0x0a, 0x1e, 0x00, 0x3f, 0x01, RE15_FW_CLIP_WURF, 0x00, 0x09, 0x0a, 0x14, 0x00,
        0x22, 0x05, RE15_FW_SIG_WURF, 0x01 };
    CHECK(n > (int)sizeof soll_kopf && memcmp(p, soll_kopf, sizeof soll_kopf) == 0, "Programmkopf (sub12 @0x0181A-0x018BE) weicht ab");
    /* Rest: Poll Wsleep, Poll Landung, Sleep 15, Reaktion, Cut 15, Sleep 1, Sleep 87, Cut 12 (gemerkt), Item_aot_set, Ende */
    static const uint8_t soll_rest[] = {
        0x22, 0x05, RE15_FW_SIG_WSLEEP, 0x00, 0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_FW_SIG_WSLEEP, 0x00,
        0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_FW_SIG_GELANDET, 0x00,
        0x09, 0x0a, 0x0f, 0x00, 0x22, 0x05, RE15_FW_SIG_REAKTION, 0x01, 0x29, RE15_FW_CUT_REAKTION, 0x09, 0x0a, 0x01, 0x00,
        0x09, 0x0a, RE15_FW_REAKTION_BILDER, 0x00, 0x29, 12 };
    const int o = (int)sizeof soll_kopf;
    CHECK(n > o + (int)sizeof soll_rest && memcmp(p + o, soll_rest, sizeof soll_rest) == 0, "Programm-Mitte (@0x018C2-0x01948) weicht ab");
    const uint8_t *it = p + o + sizeof soll_rest;
    CHECK(it[0] == 0x50 && it[1] == RE15_FW_AOT_SLOT && it[2] == 0x09 && it[3] == 0x31 && s16(it + 10) == RE15_FW_AOT_W &&
          s16(it + 12) == RE15_FW_AOT_D && s16(it + 14) == RE15_FW_ITEM && s16(it + 16) == RE15_FW_ANZAHL &&
          s16(it + 18) == RE15_R42_BIT_WERFER_FINALE && it[20] == 0xff, "Item_aot_set (Form ROOM40A1 @0x00ED0) weicht ab");
    static const uint8_t soll_ende[] = { 0x22, 0x05, RE15_FW_SIG_ENDE, 0x01, 0x09, 0x0a, 0x01, 0x00, 0x2e, 0x01, 0x00, 0x00, 0x42, 0x00,
                                         0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x3c, 0x01, 0x01, 0x00 };
    CHECK(n == o + (int)sizeof soll_rest + 22 + (int)sizeof soll_ende && memcmp(it + 22, soll_ende, sizeof soll_ende) == 0,
          "Programm-Ende (@0x01954-0x019AE, sub18 @0x01B54-0x01B60) weicht ab (Laenge %d)", n);
    printf("programm: %d Bytes, sub12 in RE1.5-Opcodes\n", n);
}

static void t_bahn(void)
{
    /* Ada vom Steg: Hand im Loslassbild (Clip 0x12 Bild 20, Ada-Gerippe, Gier 3700) ~ y -8650; RE2 v (25,20,10) Gier 0 = Weltachsen */
    const int32_t s[3] = { 0, -8650, 0 }, v[3] = { 25, 20, 10 };
    int32_t e[3]; int nb = 0;
    re15_finale_wurf_bahn(s, v, 0, e, &nb);
    CHECK(nb == 28, "Steg-Fall %d Bilder statt 28 (10n^2+10n >= 7850)", nb);
    CHECK(e[1] == -8650 + 10 * 28 * 28 + 10 * 28 && e[0] == 25 * 28 + 5 * 28 * 27 / 2 && e[2] == 10 * 28,
          "Steg-Fall Ende (%d,%d,%d)", e[0], e[1], e[2]);
    /* Johns Wurf (finale.log Messlauf: Start (24197,-1196,-23097), v (-50,-15,-22), Gier 2048 -> a (-5,20,0)) */
    const int32_t sj[3] = { 24197, -1196, -23097 }, vj[3] = { -50, -15, -22 };
    re15_finale_wurf_bahn(sj, vj, 0x0800, e, &nb);
    CHECK(nb == 8 && e[1] >= -RE15_FW_FLUG_STOP, "John-Wurf %d Bilder, y %d", nb, e[1]);
    CHECK(e[0] == 24197 - 50 * 8 - 5 * 28 && e[2] == -23097 - 22 * 8, "John-Wurf Ende (%d,%d) statt (23657,-23273)", e[0], e[2]);
    printf("bahn: Steg 28 Bilder, John 8 Bilder -> (%d,%d)\n", e[0], e[2]);
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
    uint8_t *m = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/../RE2/R702_WERFER.MD1", &nm);
    uint8_t *t = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/../RE2/R702_WERFER.TIM", &nt);
    CHECK(j && r && m && t, "JOHN_49_WURF.EMD / ROOM7020.RDT / R702_WERFER.* fehlen");
    if (j && r) {
        re15_emd_skeleton_t sk; re15_emd_animation_t an;
        memset(&sk, 0, sizeof sk); memset(&an, 0, sizeof an);
        CHECK(re15_emd_parse_loco_bank(j, n, &sk, &an) == 0, "Paar 1 nicht lesbar");
        CHECK(an.clip_count == 25 && an.clips[RE15_FW_CLIP_WURF].frame_count == 55, "Paar 1: %d Clips, Clip 24 %d Bilder",
              an.clip_count, an.clip_count > 24 ? an.clips[24].frame_count : -1);
        if (an.clip_count == 25 && sk.keyframe_size_bytes == 80) {
            /* RBJ @0x2E34: Trailer @+0x2E54 (Record 0: EMR-Praefix, EDD), Keyframes ab Praefix + 4 + o_frm */
            const uint8_t *rb = r + 0x2E34;
            const uint32_t pre = u32(rb + 0x2E54), edd = u32(rb + 0x2E58);
            const int o_frm = s16(rb + pre + 6), o0 = s16(rb + edd + 2);
            int erst = 0;
            for (int c = 0; c < 0x12; c++) erst += (c == 0) ? s16(rb + edd) : s16(rb + edd + 4 * c);
            int ok = 1;
            for (int f = 0; f < 55; f++) {
                const uint32_t e = u32(rb + edd + o0 + 4 * (erst + f));
                const uint32_t w = re15_emd_get_frame_entry(&an, RE15_FW_CLIP_WURF, f);
                const uint8_t *kr = rb + pre + 4 + o_frm + (e & 0xfff) * 80;
                const uint8_t *kp = sk.keyframe_data + (w & 0xfff) * 80;
                if (memcmp(kr, kp, 80) != 0 || (w & ~0xfffu) != (e & ~0xfffu)) ok = 0;
            }
            CHECK(ok, "Clip 24 != RBJ-Clip 0x12 (Keyframes/Flags)");
        }
    }
    if (r && m && t) {
        CHECK(nm == 0x2788 - 0x2044 && memcmp(m, r + 0x2044, nm) == 0, "R702_WERFER.MD1 != ROOM7020 @0x2044..0x2788");
        CHECK(nt == nr - 0x24F9C && memcmp(t, r + 0x24F9C, nt) == 0, "R702_WERFER.TIM != ROOM7020 @0x24F9C..Ende");
        re15_md1_t md; memset(&md, 0, sizeof md);
        CHECK(re15_md1_parse(m, (int)nm, &md) == 0 && md.mesh_count >= 1, "R702_WERFER.MD1 nicht lesbar");
        re15_tim_t tm; memset(&tm, 0, sizeof tm);
        CHECK(re15_tim_parse(t, (int)nt, &tm) == 0 && tm.bpp == 8, "R702_WERFER.TIM nicht lesbar / nicht 8 bpp");
        printf("modelle: Werfer %d Meshes, TIM %d bpp %dx%d\n", md.mesh_count, tm.bpp, tm.width, tm.height);
    }
    free(j); free(r); free(m); free(t);
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
