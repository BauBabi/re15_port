/* test_r41_kanister2080.c — Runde 41 Spur G "kanister2080": Riegel fuer die Bausteine der 2080-Zombies, des Gaskanisters an
 * der Reling und der Marvin/John-Szene. Belege: analysis/befunde_runde41/G_kanister2080.md, include/re15_kanister2080_r41.h.
 *
 *   lage       Kanister (KAN_X, -14400, KAN_Z) in ROOM2080 Cut 5 (RID @0x100) projiziert -> Fuss in der roten Marke von
 *              Gas_Can.bmp (x 159..175, y 149..163), Kopf darueber; Rueckprojektion der Markenmitte (167,156) auf y -14400
 *              trifft die Konstante (+-60). Zombies: alle 8 in Cut 2 und Cut 7 mit Fuss und Kopf im Bild, paarweise >= 800,
 *              keiner in einer Band-8-Wandzelle (SCA @0x4BC, floor-Byte 0x88).
 *   spawn      Programm 0 = 8 Sce_em_set (Typ 0x16, grid 0x02, Etage 8, Tot-Bit 236+i, y -14400, Lage/Gier der Tabelle) +
 *              Evt_end; Form ROOM2020 sub00 @0x008F6 (Bytes 0..7 bis auf Slot/Etage/Tot-Bit/Typ verglichen).
 *   szene      Programm 1: Sce_em_set NPC (0x40 Leon / 0x49 Elza, grid 0x40, Etage 8, Tot-Bit 0xff), Anlegen ROOM1021-Form
 *              (Plc_motion(2,6) + Sleep 110), je lebendem Zombie Plc_motion(2,7) + Signal + Sleep 30, Senken (2,6)+Plc_flg 0x80
 *              + Sleep 20, Schnitte 2 -> 5 -> 7 in dieser Reihenfolge, RE2-Wurf ROOM7020 sub12 (Cut 7, Plc_motion, Message_on
 *              42, Sleep 30, Plc_motion, Sleep 20, Signal WURF, Poll GELANDET, Sleep 15), Rueckkehr-Tuer Raum-Byte 0x09 + Aot_on.
 *              Mit 3 schon toten Zombies (Zone-7-Bits gesetzt) nur 5 Schuss-Bloecke.
 *   start      Programm 2 (Riegel + Tuer -> 0x08 Cut 2 Etage 5 Parkplatz) und 3 (RE2 sub18-Form: Plc_ret, Riegel los, Cut_auto).
 *   bahn       RE2 sub13: unabhaengig nachgerechnete Bahn (Lage += v, v += a; Stop y >= Wasser - 800) == Modul; Landepunkt aus
 *              der Nennlage im 2090-Becken (Aussenwaende x[-8900..7200] z[-27000..-5400], ausserhalb Plattform-/Rampenblock).
 *   texte      2080/2081 Id 42 in der Form ROOM2000 msg 1 @0x1BFD / ROOM2001 msg 0 @0x1E0A (Kopf 04 00 05 07 + Sprecher +
 *              05 00, Ende 04 01 01 63), Wortlaut dekodiert.
 *   ebene      Nachtrag 20: Treppen-Aot @0x0BE2 (sce 12, Band 0x80, x -22700..-16400, side 0, Bandsprung 3) und Absatzwand
 *              @0x05C4 = Konstanten; Flutfuellung der Band-8-Freiflaeche (SCA) ab Zombie 0: geschlossen, alle 8 darin, nichts
 *              westlich x -16650 / auf der Treppe, oberer Absatz erreichbar, Treppenfuss (Band 5) nicht.
 */
#include "re15_kanister2080_r41.h"
#include "re15_camera.h"
#include "re15_scd.h"
#include "re15_gameflow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern uint8_t re15_em_status_zone(void);
extern unsigned g_current_room_id;

static int fails = 0;
#define PRUEFE(c, ...) do { if (!(c)) { fails++; printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *rel, long *n)
{
    char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (n) *n = sz; return b;
}
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static int16_t rd16(const uint8_t *p) { return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8)); }

static re15_camera_cut_t s_cut[16];
static uint8_t *s_rdt; static long s_rdt_n;
static int rdt_laden(void)
{
    if (s_rdt) return 1;
    s_rdt = slurp("STAGE2/ROOM2080.RDT", &s_rdt_n);
    if (!s_rdt) { printf("  ROOM2080.RDT fehlt (%s)\n", RE15_ASSET_PSX_DIR); return 0; }
    uint32_t cam = rd32(s_rdt + 8 + 7 * 4);
    for (int c = 0; c < s_rdt[1] && c < 16; c++) memcpy(&s_cut[c], s_rdt + cam + c * 32, 32);
    return 1;
}
static int proj(int c, double x, double y, double z, double *sx, double *sy)
{
    re15_camera_view_t v; if (re15_camera_build_view(&s_cut[c], &v) != 0) return 0;
    double vx = (v.rot[0]*x + v.rot[1]*y + v.rot[2]*z) / 4096.0 + v.trans[0];
    double vy = (v.rot[3]*x + v.rot[4]*y + v.rot[5]*z) / 4096.0 + v.trans[1];
    double vz = (v.rot[6]*x + v.rot[7]*y + v.rot[8]*z) / 4096.0 + v.trans[2];
    if (vz <= 0) return 0;
    *sx = 160.0 + vx * v.fov_screen_dist / vz; *sy = 120.0 + vy * v.fov_screen_dist / vz;
    return 1;
}
static int rueck(int c, double u, double w, double Y, double out[3])
{
    re15_camera_view_t v; if (re15_camera_build_view(&s_cut[c], &v) != 0) return 0;
    double R[9]; for (int i = 0; i < 9; i++) R[i] = v.rot[i] / 4096.0;
    double cpos[3]; for (int i = 0; i < 3; i++) cpos[i] = -(R[0*3+i]*v.trans[0] + R[1*3+i]*v.trans[1] + R[2*3+i]*v.trans[2]);
    double dv[3] = { (u - 160.0) / v.fov_screen_dist, (w - 120.0) / v.fov_screen_dist, 1.0 };
    double dw[3]; for (int i = 0; i < 3; i++) dw[i] = R[0*3+i]*dv[0] + R[1*3+i]*dv[1] + R[2*3+i]*dv[2];
    if (fabs(dw[1]) < 1e-9) return 0;
    double t = (Y - cpos[1]) / dw[1]; if (t <= 0) return 0;
    for (int i = 0; i < 3; i++) out[i] = cpos[i] + t * dw[i];
    return 1;
}
static int im_bild(double x, double y) { return x >= 0 && x < 320 && y >= 0 && y < 240; }

/* ---- lage --------------------------------------------------------------------------------------------------------- */
static int teil_lage(void)
{
    if (!rdt_laden()) return 1;
    double fx, fy, kx, ky;
    PRUEFE(proj(RE15_K2080_CUT_KANISTER, RE15_K2080_KAN_X, RE15_K2080_Y, RE15_K2080_KAN_Z, &fx, &fy), "Kanister-Fuss nicht in Cut 5");
    PRUEFE(proj(RE15_K2080_CUT_KANISTER, RE15_K2080_KAN_X, RE15_K2080_Y - RE15_K2080_KAN_LAENGE, RE15_K2080_KAN_Z, &kx, &ky),
           "Kanister-Kopf nicht in Cut 5");
    printf("  Cut 5: Kanister-Fuss (%.1f,%.1f), Kopf (%.1f,%.1f); Marke x 159..175 y 149..163\n", fx, fy, kx, ky);
    PRUEFE(fx >= 159 && fx <= 175 && fy >= 149 && fy <= 163, "Fuss (%.1f,%.1f) nicht in der Marke", fx, fy);
    PRUEFE(ky < fy - 40 && im_bild(kx, ky), "Kopf (%.1f,%.1f) nicht ueber dem Fuss im Bild", kx, ky);
    double w[3];
    PRUEFE(rueck(RE15_K2080_CUT_KANISTER, 167.0, 156.0, RE15_K2080_Y, w), "Rueckprojektion fehlgeschlagen");
    printf("  Rueckprojektion (167,156) -> (%.0f,%.0f,%.0f)\n", w[0], w[1], w[2]);
    PRUEFE(fabs(w[0] - RE15_K2080_KAN_X) <= 60 && fabs(w[2] - RE15_K2080_KAN_Z) <= 60, "Konstante weicht von der Rueckprojektion ab");
    /* Zombies */
    uint32_t sca = rd32(s_rdt + 8 + 6 * 4); int n0 = (int)rd32(s_rdt + sca + 4);
    for (int i = 0; i < RE15_K2080_N_ZOMBIES; i++) {
        int32_t x, z; int16_t g;
        re15_k2080_zombie_lage(i, &x, &z, &g);
        for (int c = 0; c < 2; c++) {
            const int cut = c ? RE15_K2080_CUT_WURF : RE15_K2080_CUT_SCHUSS;
            double a, b, d, e;
            int ok = proj(cut, x, RE15_K2080_Y, z, &a, &b) && proj(cut, x, RE15_K2080_Y - 1800, z, &d, &e);
            PRUEFE(ok && im_bild(a, b) && im_bild(d, e), "Zombie %d (%d,%d) nicht ganz in Cut %d", i, (int)x, (int)z, cut);
        }
        for (int j = 0; j < i; j++) {
            int32_t x2, z2; re15_k2080_zombie_lage(j, &x2, &z2, NULL);
            double dd = hypot((double)(x - x2), (double)(z - z2));
            PRUEFE(dd >= 800.0, "Zombies %d/%d nur %.0f auseinander", i, j, dd);
        }
        for (int k = 0; k < n0; k++) {
            const uint8_t *e = s_rdt + sca + 24 + k * 12;
            if ((e[11] >> 4) != 8) continue;
            int16_t ex = rd16(e + 4), ez = rd16(e + 6); uint16_t ew = (uint16_t)rd16(e), ed = (uint16_t)rd16(e + 2);
            PRUEFE(!(x > ex - 400 && x < ex + ew + 400 && z > ez - 400 && z < ez + ed + 400),
                   "Zombie %d (%d,%d) im Radius 400 der Band-8-Wandzelle @0x%X", i, (int)x, (int)z, (unsigned)(sca + 24 + k * 12));
        }
        const double soll = atan2(-(double)(RE15_K2080_NPC_Z - z), (double)(RE15_K2080_NPC_X - x)) * 4096.0 / 6.283185307179586;
        int sg = (int)lround(soll) & 0xfff;
        int dg = abs(((sg - g + 0x800) & 0xfff) - 0x800);
        PRUEFE(dg <= 2, "Zombie %d Gier %d statt %d (Blick auf den Treppenabsatz)", i, (int)g, sg);
    }
    return fails != 0;
}

/* ---- Programme ---------------------------------------------------------------------------------------------------- */
static int teil_spawn(void)
{
    int n = 0; const uint8_t *p = re15_k2080_programm(0, &n);
    PRUEFE(p && n == RE15_K2080_N_ZOMBIES * 20 + 2, "Programm 0 Laenge %d", n);
    if (!p) return 1;
    long rn = 0; uint8_t *r2020 = slurp("STAGE2/ROOM2020.RDT", &rn);
    PRUEFE(r2020 && rn > 0x0090A, "ROOM2020.RDT fehlt");
    for (int i = 0; i < RE15_K2080_N_ZOMBIES && p; i++) {
        const uint8_t *s = p + i * 20;
        int32_t x, z; int16_t g; re15_k2080_zombie_lage(i, &x, &z, &g);
        PRUEFE(s[0] == 0x44 && s[1] == i && s[2] == 0x16 && s[3] == 0x02 && s[4] == 8 && s[7] == 236 + i,
               "Satz %d Kopf %02x %02x %02x %02x %02x .. %02x", i, s[0], s[1], s[2], s[3], s[4], s[7]);
        PRUEFE(rd16(s + 8) == x && rd16(s + 10) == -14400 && rd16(s + 12) == z && rd16(s + 16) == g, "Satz %d Lage/Gier", i);
        if (r2020 && i > 0) {   /* Form ROOM2020 sub00 @0x0090A `44 01 16 02 01 00 01 bd ..` */
            const uint8_t *o = r2020 + 0x0090A;
            PRUEFE(o[0] == 0x44 && o[2] == s[2] && o[3] == s[3] && o[5] == s[5] && o[6] == s[6] && o[14] == s[14] && o[15] == s[15],
                   "Satz %d weicht von der Form ROOM2020 @0x0090A ab", i);
        }
    }
    PRUEFE(p[n - 2] == 0x01 && p[n - 1] == 0x00, "Programm 0 ohne Evt_end");
    free(r2020);
    return fails != 0;
}

static const uint8_t *suche(const uint8_t *p, int n, int ab, const uint8_t *muster, int m)
{
    for (int i = ab; i + m <= n; i++) if (!memcmp(p + i, muster, (size_t)m)) return p + i;
    return NULL;
}
static int zaehle(const uint8_t *p, int n, const uint8_t *muster, int m)
{
    int k = 0;
    for (int i = 0; i + m <= n; i++) if (!memcmp(p + i, muster, (size_t)m)) k++;
    return k;
}

static int szene_pruefen(int elza, int tote)
{
    g_gameflow.character = elza ? 4 : 0;
    for (int i = 0; i < RE15_K2080_N_ZOMBIES; i++) re15_game_flag_set(re15_em_status_zone(), (uint8_t)(RE15_K2080_TOT0 + i), i < tote);
    int n = 0; const uint8_t *p = re15_k2080_programm(1, &n);
    PRUEFE(p && n > 100, "Programm 1 leer");
    if (!p) return 1;
    PRUEFE(p[0] == 0x44 && p[1] == RE15_K2080_NPC_SLOT && p[2] == (elza ? 0x49 : 0x40) && p[3] == 0x40 && p[4] == 8 && p[7] == 0xff,
           "NPC-Satz %02x %02x %02x %02x %02x .. %02x", p[0], p[1], p[2], p[3], p[4], p[7]);
    PRUEFE(rd16(p + 8) == RE15_K2080_NPC_X && rd16(p + 10) == -14400 && rd16(p + 12) == RE15_K2080_NPC_Z, "NPC-Lage");
    const uint8_t anlegen[] = { 0x3f, 0x02, 0x06, 0x00, 0x09, 0x0a, 110, 0x00 };
    const uint8_t feuer[] = { 0x3f, 0x02, 0x07, 0x00, 0x22, 0x05, RE15_K2080_SIG_SCHUSS, 0x01, 0x09, 0x0a, 30, 0x00 };
    const uint8_t senken[] = { 0x3f, 0x02, 0x06, 0x00, 0x43, 0x00, 0x80, 0x00, 0x09, 0x0a, 20, 0x00 };
    const int soll = RE15_K2080_N_ZOMBIES - tote;
    PRUEFE(zaehle(p, n, feuer, sizeof feuer) == soll, "%d Schuss-Bloecke statt %d", zaehle(p, n, feuer, sizeof feuer), soll);
    const uint8_t *a = suche(p, n, 0, anlegen, sizeof anlegen), *sk = suche(p, n, 0, senken, sizeof senken);
    PRUEFE(a && sk && a < sk, "Anlegen (ROOM1021 @0x020C6/@0x020CA) / Senken (@0x020EA-F2) fehlt oder vertauscht");
    /* Schnitte mit ihrem Folge-Opcode (ein nacktes `29 cc` koennte in Lage-Bytes stehen): Cut 2 + Tuer-Ton-Signal, Cut 5 +
     * Ankunftsflag loeschen, Cut 7 + Plc_motion(2,6) */
    const uint8_t c2[] = { 0x29, 2, 0x22, 0x05, RE15_K2080_SIG_TUERTON, 0x01 };
    const uint8_t c5[] = { 0x29, 5, 0x22, 0x05, RE15_K2080_SIG_ANKUNFT, 0x00 };
    const uint8_t c7[] = { 0x29, 7, 0x3f, 0x02, 0x06, 0x00 };
    const uint8_t *p2 = suche(p, n, 0, c2, sizeof c2), *p5 = suche(p, n, 0, c5, sizeof c5), *p7 = suche(p, n, 0, c7, sizeof c7);
    PRUEFE(p2 && p5 && p7 && p2 < p5 && p5 < p7, "Schnittfolge 2 -> 5 -> 7");
    const uint8_t wurf[] = { 0x29, 0x07, 0x3f, 0x02, 0x06, 0x00, 0x2b, 42, 0x00, 0x00, 0x09, 0x0a, 30, 0x00, 0x3f, 0x02, 0x06, 0x00,
                             0x09, 0x0a, 20, 0x00, 0x22, 0x05, RE15_K2080_SIG_WURF, 0x01,
                             0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_K2080_SIG_GELANDET, 0x00,
                             0x09, 0x0a, 15, 0x00, 0x3b, RE15_K2080_TUER_SLOT };
    const uint8_t *pw = suche(p, n, 0, wurf, sizeof wurf);
    PRUEFE(pw != NULL, "RE2-Wurfform ROOM7020 sub12 (Cut 7, Plc_motion, Message_on 42, Sleep 30/20, Loslassen, Poll, Sleep 15, Tuer)");
    if (pw) {
        const uint8_t *t = pw + sizeof wurf - 2;   /* Door_aot_set */
        PRUEFE(t[22] == RE15_K2080_STAGE_BYTE && t[23] == RE15_K2080_RAUM_BYTE_2090, "Rueckkehr-Tuer Stage/Raum %02x %02x", t[22], t[23]);
        PRUEFE(t[32] == 0x47 && t[33] == RE15_K2080_TUER_SLOT && t[34] == 0x01, "Aot_on + Evt_end nach der Tuer");
    }
    return 0;
}
static int teil_szene(void)
{
    szene_pruefen(0, 0);
    szene_pruefen(1, 0);
    szene_pruefen(0, 3);
    for (int i = 0; i < RE15_K2080_N_ZOMBIES; i++) re15_game_flag_set(re15_em_status_zone(), (uint8_t)(RE15_K2080_TOT0 + i), 0);
    g_gameflow.character = 0;
    return fails != 0;
}
static int teil_start(void)
{
    int n = 0; const uint8_t *p = re15_k2080_programm(2, &n);
    const uint8_t riegel[] = { 0x22, 0x02, 0x07, 0x01, 0x22, 0x01, 0x1b, 0x01 };
    PRUEFE(p && n == 8 + 32 + 2 + 2 && !memcmp(p, riegel, 8), "Programm 2 Form (Riegel ROOM2080 sub02 @0x0CB6/@0x0CBA)");
    if (p && n >= 44) {
        const uint8_t *t = p + 8;
        PRUEFE(t[0] == 0x3b && t[1] == RE15_K2080_TUER_SLOT && t[23] == RE15_K2080_RAUM_BYTE_2080 && t[24] == RE15_K2080_CUT_SCHUSS &&
               t[25] == RE15_K2080_PARK_ETAGE && rd16(t + 14) == RE15_K2080_PARK_X && rd16(t + 16) == RE15_K2080_PARK_Y &&
               rd16(t + 18) == RE15_K2080_PARK_Z, "Tuer nach 2080 (Raum 0x08, Cut 2, Etage 5, Parkplatz)");
        PRUEFE(p[40] == 0x47 && p[41] == RE15_K2080_TUER_SLOT, "Aot_on");
    }
    p = re15_k2080_programm(3, &n);
    const uint8_t rueck[] = { 0x09, 0x0a, 0x01, 0x00, 0x22, 0x02, 0x07, 0x01, 0x22, 0x01, 0x1b, 0x01, 0x09, 0x0a, 30, 0x00,
                              0x2e, 0x01, 0x00, 0x00, 0x42, 0x00, 0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x3c, 0x01, 0x01, 0x00 };
    PRUEFE(p && n == (int)sizeof rueck && !memcmp(p, rueck, sizeof rueck), "Programm 3 = RE2 sub18-Form (@0x0A2C-0A38)");
    return fails != 0;
}

/* ---- bahn ---------------------------------------------------------------------------------------------------------- */
static int teil_bahn(void)
{
    const int32_t start[3] = { RE15_K2080_NPC_WURF_X + RE15_K2080_HAND_VOR, RE15_K2080_Y + RE15_K2080_HAND_HOCH, RE15_K2080_NPC_STAND_Z };
    int32_t ende[3]; int nb = 0;
    re15_k2080_bahn_rechnen(start, 0, RE15_K2080_WASSER_TIEF, ende, &nb);
    /* unabhaengig: RE2 @0x80055ab0 (Lage += v) dann @0x80055b2c (v += a), Pruefung vor jedem Durchlauf (Edwhile) */
    int64_t p[3] = { start[0], start[1], start[2] }, v[3] = { 25, 20, 10 }, a[3] = { 5, 20, 0 };
    int k = 0;
    while (p[1] < RE15_K2080_WASSER_TIEF - 800 && k < 4096) { for (int i = 0; i < 3; i++) p[i] += v[i]; for (int i = 0; i < 3; i++) v[i] += a[i]; k++; }
    printf("  Bahn: Start (%d,%d,%d) -> Ende (%d,%d,%d) nach %d Bildern (unabhaengig (%lld,%lld,%lld) nach %d)\n",
           (int)start[0], (int)start[1], (int)start[2], (int)ende[0], (int)ende[1], (int)ende[2], nb,
           (long long)p[0], (long long)p[1], (long long)p[2], k);
    PRUEFE(nb == k && ende[0] == p[0] && ende[1] == p[1] && ende[2] == p[2], "Modul-Bahn != RE2-Nachrechnung");
    PRUEFE(ende[1] >= RE15_K2080_WASSER_TIEF - 800 && ende[1] - 20 - 20 * (k - 1) < RE15_K2080_WASSER_TIEF - 800, "Stop-Bedingung");
    /* Gier 1024 (Blick -z): Vektor gedreht = (x cos + z sin, -x sin + z cos) -> v (10, ., -25) */
    re15_k2080_bahn_rechnen(start, 1024, RE15_K2080_WASSER_TIEF, ende, &nb);
    PRUEFE(ende[0] > start[0] && ende[2] < start[2] - 2000, "Gier 1024: Bahn nicht nach -z (%d,%d)", (int)ende[0], (int)ende[2]);
    /* Landepunkt der Nennlage im 2090-Becken (gb_becken_draussen: Aussenwaende + Bloecke) */
    re15_k2080_bahn_rechnen(start, 0, RE15_K2080_WASSER_TIEF, ende, &nb);
    const int32_t x = ende[0], z = ende[2];
    const int innen = x > -8900 && x < 7200 && z > -27000 && z < -5400;
    const int plat = x > -1700 && x < 1850 && z > -20000 && z < -12450;
    const int ramp = x > 1850 && x < 7200 && z > -18100 && z < -14500;
    PRUEFE(innen && !plat && !ramp, "Landepunkt (%d,%d) nicht im Wasser des Beckens", (int)x, (int)z);
    return fails != 0;
}

/* ---- texte --------------------------------------------------------------------------------------------------------- */
static void dekodiere(const uint8_t *b, int n, char *out, int cap)
{
    int k = 0;
    for (int i = 0; i < n && k + 1 < cap; i++) {
        uint8_t c = b[i];
        if (c == 0x04 || c == 0x05 || c == 0x01) { i++; continue; }
        if (c == 0x00) out[k++] = ' ';
        else if (c >= 0x1D && c < 0x1D + 26) out[k++] = (char)('A' + c - 0x1D);
        else if (c >= 0x3D && c < 0x3D + 26) out[k++] = (char)('a' + c - 0x3D);
        else if (c == 0x16) out[k++] = ':';
        else if (c == 0x18) out[k++] = ',';
        else if (c == 0x1A) out[k++] = '!';
        else out[k++] = '?';
    }
    out[k] = 0;
}
static int teil_texte(void)
{
    long n0 = 0, n1 = 0; uint8_t *r2000 = slurp("STAGE2/ROOM2000.RDT", &n0), *r2001 = slurp("STAGE2/ROOM2001.RDT", &n1);
    PRUEFE(r2000 && r2001, "ROOM2000/2001.RDT fehlen");
#include "../../engine/src/gen/kanister2080_r41_texte.inc"
    const uint8_t *t[2] = { k_k2080_msg42_leon, k_k2080_msg42_elza };
    const int tn[2] = { (int)sizeof k_k2080_msg42_leon, (int)sizeof k_k2080_msg42_elza };
    const char *soll[2] = { "Marvin: Hey, Leon, try with this!", "John: Hey, Elza, try with this!" };
    const uint8_t *vor[2] = { r2000 ? r2000 + 0x1BFD : NULL, r2001 ? r2001 + 0x1E0A : NULL };
    for (int i = 0; i < 2; i++) {
        char s[96]; dekodiere(t[i], tn[i], s, (int)sizeof s);
        printf("  Id 42 %s: \"%s\"\n", i ? "Elza" : "Leon", s);
        PRUEFE(!strcmp(s, soll[i]), "Wortlaut \"%s\" statt \"%s\"", s, soll[i]);
        PRUEFE(!memcmp(t[i] + tn[i] - 4, "\x04\x01\x01\x63", 4), "Ende 04 01 01 63");
        const int kopf = i ? 4 + 5 + 2 : 4 + 7 + 2;   /* 04 00 05 07 + Sprecher + 05 00 */
        PRUEFE(vor[i] && !memcmp(t[i], vor[i], (size_t)kopf), "Kopf weicht von der Vorbild-Zeile ab");
    }
    g_gameflow.character = 0;
    free(r2000); free(r2001);
    return fails != 0;
}

/* ---- ebene (Nachtrag 20) ------------------------------------------------------------------------------------------- */
/* Raster ueber ROOM2080 (50er Zellen): frei = kein Band-8-Wandkoerper der SCA @RDT+0x20 (typ 1 Rechteck, typ 3 Kreis um die
 * Mitte mit Radius w/2). Flutfuellung ab Zombie 0 mit Punktgroesse (strengste Form: was ein Punkt nicht erreicht, erreicht kein
 * Zombie mit Radius). Erwartet: alle 8 in derselben Flaeche, Flaeche geschlossen (beruehrt den Rasterrand nicht), keine Zelle
 * westlich der Absatzwand @0x05C4 (x -16650) = nichts von der Treppe @0x0BE2 (x -22700..-16400), der Treppenabsatz
 * (-15500,-16500) gehoert dazu, der Treppenfuss an der 2030-Tuer (-23200,-16700, Band 5) nicht. */
#define EB_X0     (-30000)
#define EB_Z0     (-32000)
#define EB_RASTER 50
#define EB_NX     600
#define EB_NZ     640
static uint8_t s_eb_frei[EB_NX * EB_NZ];
static uint8_t s_eb_flut[EB_NX * EB_NZ];
static int     s_eb_q[EB_NX * EB_NZ];
static int eb_index(int32_t x, int32_t z)
{
    int ix = (int)((x - EB_X0) / EB_RASTER), iz = (int)((z - EB_Z0) / EB_RASTER);
    if (ix < 0 || ix >= EB_NX || iz < 0 || iz >= EB_NZ) return -1;
    return iz * EB_NX + ix;
}
static int teil_ebene(void)
{
    if (!rdt_laden()) return 1;
    const uint8_t *a = s_rdt + 0x0BE2;   /* ROOM2080 main00 Aot_set Slot 2 (Treppe) */
    PRUEFE(a[0] == 0x2c && a[1] == 2 && a[2] == 12 && a[4] == 0x80, "Treppen-Aot @0x0BE2 Kopf %02x %02x %02x %02x %02x",
           a[0], a[1], a[2], a[3], a[4]);
    PRUEFE(rd16(a + 6) == RE15_K2080_TREPPE_X0 && rd16(a + 8) == RE15_K2080_TREPPE_Z0 &&
           rd16(a + 6) + (int)(uint16_t)rd16(a + 10) == RE15_K2080_TREPPE_X1 &&
           rd16(a + 8) + (int)(uint16_t)rd16(a + 12) == RE15_K2080_TREPPE_Z1, "Treppen-Rechteck @0x0BE2 != Konstanten");
    PRUEFE(rd16(a + 14) == 0 && rd16(a + 16) == 3, "Treppe side %d / Bandsprung %d statt 0 / 3", rd16(a + 14), rd16(a + 16));
    const uint8_t *w = s_rdt + 0x05C4;   /* Band-8-Absatzwand */
    PRUEFE(w[8] == 1 && (w[11] >> 4) == RE15_K2080_ETAGE && rd16(w + 4) + (int)(uint16_t)rd16(w) == RE15_K2080_ABSATZ_WAND_X,
           "Absatzwand @0x05C4: typ %d Band %d Ostkante %d", w[8], w[11] >> 4, rd16(w + 4) + (int)(uint16_t)rd16(w));
    const uint32_t sca = rd32(s_rdt + 0x20);
    int n = 0;
    for (int g = 0; g < 5; g++) n += (int)rd32(s_rdt + sca + 4 + 4 * g);
    int n8 = 0;
    for (int i = 0; i < EB_NX * EB_NZ; i++) s_eb_frei[i] = 1;
    for (int k = 0; k < n; k++) {
        const uint8_t *e = s_rdt + sca + 24 + k * 12;
        if ((e[11] >> 4) != RE15_K2080_ETAGE) continue;
        n8++;
        const int32_t ex = rd16(e + 4), ez = rd16(e + 6), ew = (uint16_t)rd16(e), ed = (uint16_t)rd16(e + 2);
        PRUEFE(e[8] == 1 || e[8] == 3, "Band-8-Zelle %d typ %d (erwartet 1/3)", k, e[8]);
        for (int iz = 0; iz < EB_NZ; iz++)
            for (int ix = 0; ix < EB_NX; ix++) {
                const int32_t x = EB_X0 + ix * EB_RASTER + EB_RASTER / 2, z = EB_Z0 + iz * EB_RASTER + EB_RASTER / 2;
                int drin;
                if (e[8] == 3) {
                    const double dx = x - (ex + ew / 2.0), dz = z - (ez + ed / 2.0);
                    drin = dx * dx + dz * dz < (ew / 2.0) * (ew / 2.0);
                } else {
                    drin = x >= ex && x < ex + ew && z >= ez && z < ez + ed;
                }
                if (drin) s_eb_frei[iz * EB_NX + ix] = 0;
            }
    }
    int32_t zx, zz; re15_k2080_zombie_lage(0, &zx, &zz, NULL);
    const int start = eb_index(zx, zz);
    PRUEFE(start >= 0 && s_eb_frei[start], "Zombie 0 nicht auf freier Band-8-Flaeche");
    if (start < 0 || !s_eb_frei[start]) return 1;
    memset(s_eb_flut, 0, sizeof s_eb_flut);
    int kopf = 0, ende = 0, rand = 0, west = 0, treppe = 0, zellen = 0;
    int32_t bx0 = 0x7fffffff, bx1 = -0x7fffffff, bz0 = 0x7fffffff, bz1 = -0x7fffffff;
    s_eb_flut[start] = 1; s_eb_q[ende++] = start;
    while (kopf < ende) {
        const int c = s_eb_q[kopf++], ix = c % EB_NX, iz = c / EB_NX;
        const int32_t x = EB_X0 + ix * EB_RASTER + EB_RASTER / 2, z = EB_Z0 + iz * EB_RASTER + EB_RASTER / 2;
        zellen++;
        if (x < bx0) bx0 = x; if (x > bx1) bx1 = x; if (z < bz0) bz0 = z; if (z > bz1) bz1 = z;
        if (ix == 0 || iz == 0 || ix == EB_NX - 1 || iz == EB_NZ - 1) rand++;
        if (x < RE15_K2080_ABSATZ_WAND_X) west++;
        if (x >= RE15_K2080_TREPPE_X0 && x < RE15_K2080_TREPPE_X1 - 300 && z >= RE15_K2080_TREPPE_Z0 && z < RE15_K2080_TREPPE_Z1)
            treppe++;   /* Treppen-Rechteck ohne den 300 breiten Streifen ueber dem Absatz (x -16700..-16400 liegt im Absatz) */
        const int nb[4] = { ix > 0 ? c - 1 : -1, ix < EB_NX - 1 ? c + 1 : -1, iz > 0 ? c - EB_NX : -1, iz < EB_NZ - 1 ? c + EB_NX : -1 };
        for (int q = 0; q < 4; q++)
            if (nb[q] >= 0 && s_eb_frei[nb[q]] && !s_eb_flut[nb[q]]) { s_eb_flut[nb[q]] = 1; s_eb_q[ende++] = nb[q]; }
    }
    printf("  SCA @0x%X: %d Zellen, davon %d auf Band %d; Flaeche ab Zombie 0: %d Raster, x %d..%d z %d..%d\n", (unsigned)sca, n, n8,
           RE15_K2080_ETAGE, zellen, (int)bx0, (int)bx1, (int)bz0, (int)bz1);
    printf("  Rasterrand %d, westlich der Absatzwand %d, auf der Treppe %d\n", rand, west, treppe);
    PRUEFE(rand == 0, "Band-8-Flaeche nicht geschlossen (%d Randzellen)", rand);
    PRUEFE(west == 0 && treppe == 0, "Band-8-Flaeche reicht auf die Treppe (%d westlich, %d im Rechteck)", west, treppe);
    for (int i = 0; i < RE15_K2080_N_ZOMBIES; i++) {
        int32_t x, z; re15_k2080_zombie_lage(i, &x, &z, NULL);
        const int c = eb_index(x, z);
        PRUEFE(c >= 0 && s_eb_flut[c], "Zombie %d (%d,%d) nicht in der Flaeche von Zombie 0", i, (int)x, (int)z);
    }
    const int absatz = eb_index(-15500, -16500), fuss = eb_index(RE15_K2080_PARK_X, RE15_K2080_PARK_Z);
    PRUEFE(absatz >= 0 && s_eb_flut[absatz], "oberer Treppenabsatz (-15500,-16500) nicht erreichbar");
    PRUEFE(fuss >= 0 && !s_eb_flut[fuss], "Treppenfuss (%d,%d) Band 5 von oben erreichbar", RE15_K2080_PARK_X, RE15_K2080_PARK_Z);
    return fails != 0;
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "lage";
    int rc = 1;
    if (!strcmp(t, "lage")) rc = teil_lage();
    else if (!strcmp(t, "spawn")) rc = teil_spawn();
    else if (!strcmp(t, "szene")) rc = teil_szene();
    else if (!strcmp(t, "start")) rc = teil_start();
    else if (!strcmp(t, "bahn")) rc = teil_bahn();
    else if (!strcmp(t, "texte")) rc = teil_texte();
    else if (!strcmp(t, "ebene")) rc = teil_ebene();
    else printf("unbekannter Teil %s\n", t);
    printf("%s: %s (%d Fehler)\n", t, rc ? "FAIL" : "OK", fails);
    return rc;
}
