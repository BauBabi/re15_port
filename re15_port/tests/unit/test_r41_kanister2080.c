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
 *              Mit 3 schon toten Zombies (Zone-7-Bits gesetzt) nur 5 Schuss-Bloecke. Nachbesserung 1: zwischen zwei Zielen
 *              Poll auf AUSGERICHTET (kein Sleep 10), kein Sleep nach dem Tuer-Ton; Aufnehmen = Marvins Hinknien/Aufstehen
 *              ROOM11B0 sub06 @0x01650..0x01663 Byte fuer Byte (Plc_motion(1,11), Sleep 60 geteilt am Griff-Bild 24,
 *              Plc_motion(1,11)+Plc_flg 0x80, Sleep 30); kein Loco c2 (PL00-Klettern) und kein Eigen c0 (RUN-Gait) im Programm.
 *   start      Programm 2 (Riegel + Tuer -> 0x08 Cut 2 Etage 5 Parkplatz) und 3 (RE2 sub18-Form: Plc_ret, Riegel los, Cut_auto;
 *              Nachbesserung 1 ohne Sleep 30 - RE2 Wsleeping @0x80053ac0 wartet dort nur auf Leons Zeile).
 *   bahn       RE2 sub13: unabhaengig nachgerechnete Bahn (Lage += v, v += a; Stop y >= Wasser - 800) == Modul; Landepunkt aus
 *              der Nennlage im 2090-Becken (Aussenwaende x[-8900..7200] z[-27000..-5400], ausserhalb Plattform-/Rampenblock).
 *   landung    Nachbesserung 2 (N3): Nord- UND Suedwurf enden im 2090-Wasser, >= 10 Fall-Bilder in Cut 12, Wahl des weiteren
 *              Landepunkts nach der Lage des Alligators (>= 8880 x 2731/4096 entfernt), Bit 117 im Raumaufbau.
 *              Nachbesserung 1 (FG-E3/FG-E4): Nennbahn mit der Wurf-Gier endet 800 ueber dem 2090-Wasser (y -1620) im Becken,
 *              mindestens 10 Fall-Bilder in ROOM2090 Cut 12 sichtbar; Spielstand mit Bit 116: der Installer legt den Kanister an
 *              diese Aufschlagstelle (Spur-B-API re15_gr2_kanister_welt, Stufe 0) - in 2090 UND 2091.
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
#include "re15_gator_re2_r41.h"
#include "re15_skeleton.h"   /* re15_cos_q12 / re15_sin_q12 (Nennbahn, Teil landung) */
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
    const uint8_t c7[] = { 0x29, 7, 0x33, 0x00 };   /* Nachbesserung 2 (N2): Cut 7 + Dir_set (RE2 @0x0726 -> @0x0748) */
    const uint8_t *p2 = suche(p, n, 0, c2, sizeof c2), *p5 = suche(p, n, 0, c5, sizeof c5), *p7 = suche(p, n, 0, c7, sizeof c7);
    PRUEFE(p2 && p5 && p7 && p2 < p5 && p5 < p7, "Schnittfolge 2 -> 5 -> 7");
    /* FG-E4: mit dem Loslassen der Raumwechsel nach 2090 Cut 12 (der Rest der RE2-Folge laeuft dort, Programm 3).
     * Nachbesserung 2: N2 Dir_set DIREKT nach Cut_chg 7 (RE2 @0x0726 -> @0x0748, kein Bild dazwischen); N1 Wsleep/Wsleeping (RE2
     * @0x079A/@0x079B) als Poll auf RE15_K2080_SIG_WSLEEP zwischen Sleep 20 (@0x0786) und dem Loslassen.
     * Nachbesserung 3 (Abnahme 2 M2, G-E2): der Dir_set-WERT ist die der Kamera zugewandte Gier wie Adas 0x0e74 (Teil blick),
     * unabhaengig von der Wurfrichtung (Nord/Sued, Bit 117). */
    const int wg = re15_k2080_werfer_gier();
    uint8_t wurf[] = { 0x29, 0x07, 0x33, 0x00, 0x00, 0x00, (uint8_t)(wg & 0xff), (uint8_t)((wg >> 8) & 0xff), 0x00, 0x00,
                             0x3f, 0x02, 0x06, 0x00, 0x2b, 42, 0x00, 0x00, 0x09, 0x0a, 30, 0x00, 0x3f, 0x02, 0x06, 0x00,
                             0x09, 0x0a, 20, 0x00,
                             0x22, 0x05, RE15_K2080_SIG_WSLEEP, 0x00,
                             0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_K2080_SIG_WSLEEP, 0x00,
                             0x22, 0x05, RE15_K2080_SIG_WURF, 0x01, 0x09, 0x0a, 0x01, 0x00,
                             0x3b, RE15_K2080_TUER_SLOT };
    PRUEFE(wg == RE15_K2080_WERFER_GIER_NENN, "Werfer-Gier %d ohne geladenen Raum statt des Nennwerts %d", wg, RE15_K2080_WERFER_GIER_NENN);

    /* Nachbesserung 1 (M1): Aufnehmen = ROOM11B0 sub06 @0x01650..0x01663 (Marvin kniet am Gully und steht auf) - dieselben
     * Opcodes, das Sleep 60 @0x01654 am Griff-Bild 24 (letztes Bild von Loco c11, 25 Bilder) in 24 + Signal + 36 geteilt. */
    {
        long n11 = 0; uint8_t *r11b0 = slurp("STAGE1/ROOM11B0.RDT", &n11);
        const uint8_t form[] = { 0x3f, 0x01, 0x0b, 0x00, 0x09, 0x0a, 0x3c, 0x00, 0x3f, 0x01, 0x0b, 0x00, 0x43, 0x00, 0x80, 0x00,
                                 0x09, 0x0a, 0x1e, 0x00 };
        PRUEFE(r11b0 && n11 > 0x01664 && !memcmp(r11b0 + 0x01650, form, sizeof form), "ROOM11B0 sub06 @0x01650..0x01663 != Vorbild");
        free(r11b0);
        const uint8_t knien[] = { 0x3f, 0x01, 0x0b, 0x00, 0x09, 0x0a, 24, 0x00, 0x22, 0x05, RE15_K2080_SIG_AUFHEBEN, 0x01,
                                  0x09, 0x0a, 36, 0x00, 0x3f, 0x01, 0x0b, 0x00, 0x43, 0x00, 0x80, 0x00, 0x09, 0x0a, 0x1e, 0x00 };
        const uint8_t *pk = suche(p, n, 0, knien, sizeof knien);
        PRUEFE(pk != NULL, "Aufnehmen nicht in der Form ROOM11B0 sub06 (Hinknien, Griff Bild 24, Aufstehen rueckwaerts, Sleep 30)");
        PRUEFE(pk && p5 && p7 && pk > p5 && pk < p7, "Aufnehmen nicht zwischen Lauf (Cut 5) und Wurf (Cut 7)");
        const uint8_t klettern[] = { 0x3f, 0x01, 0x02, 0x00 }, laufgait[] = { 0x3f, 0x02, 0x00, 0x00 };
        PRUEFE(zaehle(p, n, klettern, 4) == 0, "Plc_motion(1,2) = Loco c2 = PL00-Klettern (@0x80038134) im Programm");
        PRUEFE(zaehle(p, n, laufgait, 4) == 0, "Plc_motion(2,0) = Eigen c0 = RUN-Gait (@0x8005157c) im Programm");
        /* M5: zwischen zwei Zielen Poll auf AUSGERICHTET statt Sleep 10; kein Sleep direkt nach dem Tuer-Ton-Signal */
        const uint8_t naechst[] = { 0x22, 0x05, RE15_K2080_SIG_AUSGERICHTET, 0x00, 0x22, 0x05, RE15_K2080_SIG_ZIEL, 0x01,
                                    0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_K2080_SIG_AUSGERICHTET, 0x00 };
        PRUEFE(zaehle(p, n, naechst, sizeof naechst) == (soll > 0 ? soll - 1 : 0), "%d Ausricht-Polls statt %d",
               zaehle(p, n, naechst, sizeof naechst), soll > 0 ? soll - 1 : 0);
        PRUEFE(p2 && p2[6] == 0x2e, "nach dem Tuer-Ton-Signal folgt nicht direkt Work_set (Anlegen ROOM1021 @0x020C2)");
    }
    const uint8_t *pw = suche(p, n, 0, wurf, sizeof wurf);
    PRUEFE(pw != NULL, "RE2-Wurfform ROOM7020 sub12 (Cut 7, Plc_motion, Message_on 42, Sleep 30/20, Loslassen, Poll, Sleep 15, Tuer)");
    if (pw) {
        const uint8_t *t = pw + sizeof wurf - 2;   /* Door_aot_set */
        PRUEFE(t[22] == RE15_K2080_STAGE_BYTE && t[23] == RE15_K2080_RAUM_BYTE_2090, "Rueckkehr-Tuer Stage/Raum %02x %02x", t[22], t[23]);
        PRUEFE(t[24] == RE15_K2080_CUT_FALL, "Rueckkehr-Tuer Cut %d statt %d (FG-E4: Fall in 2090 Cut 12)", t[24], RE15_K2080_CUT_FALL);
        PRUEFE(t[32] == 0x47 && t[33] == RE15_K2080_TUER_SLOT && t[34] == 0x01, "Aot_on + Evt_end nach der Tuer");
    }
    return 0;
}
static int teil_szene(void)
{
    /* Nachbesserung 3 (G-E2): Nord- und Suedwurf bauen denselben Dir_set (die Wurfrichtung dreht nur die Bahn) */
    for (int sued = 0; sued < 2; sued++) {
        re15_game_flag_set(9, RE15_K2080_BIT_SUEDWURF, sued);
        re15_k2080_reset();
        int n = 0; const uint8_t *p = re15_k2080_programm(1, &n);
        const uint8_t c7[] = { 0x29, 7, 0x33, 0x00 };
        const uint8_t *q = p ? suche(p, n, 0, c7, sizeof c7) : NULL;
        PRUEFE(q && rd16(q + 6) == re15_k2080_werfer_gier(), "%swurf: Dir_set %d statt der Werfer-Gier %d", sued ? "Sued" : "Nord",
               q ? (int)rd16(q + 6) : -1, re15_k2080_werfer_gier());
    }
    re15_game_flag_set(9, RE15_K2080_BIT_SUEDWURF, 0);
    re15_k2080_reset();
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
    /* FG-E4: in 2090 warten auf den Aufschlag (@0x079C-07A4), Sleep 15 (@0x07A8), Schnitt auf den Fangenden (@0x07D4, hier Cut 0 =
     * die Lage beim Ausloesen im Test), dann die RE2 sub18-Form (@0x0A2C-0A38) */
    const uint8_t rueck[] = { 0x09, 0x0a, 0x01, 0x00, 0x22, 0x02, 0x07, 0x01, 0x22, 0x01, 0x1b, 0x01,
                              0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_K2080_SIG_GELANDET, 0x00,
                              0x09, 0x0a, 15, 0x00, 0x29, 0x00,
                              0x2e, 0x01, 0x00, 0x00, 0x42, 0x00, 0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x3c, 0x01, 0x01, 0x00 };
    PRUEFE(p && n == (int)sizeof rueck && !memcmp(p, rueck, sizeof rueck), "Programm 3 = Aufschlag/Sleep 15/Schnitt + RE2 sub18-Form");
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

/* ---- landung (Nachbesserung 1, FG-E3/FG-E4; Nachbesserung 2 N3: zwei Wurfrichtungen + Wahl nach der Lage des Alligators) ---- */
/* Nennbahn einer Wurfrichtung g aus der Wurf-Lage (Werfer bei RE15_K2080_NPC_WURF_X, Haende Eigen c6 Ende); sichtbare Fall-Bilder
 * in ROOM2090 Cut 12 (Projektion wie der Zeichner, re15_camera_build_view). Rueckgabe 0 = Aufschlag nicht im Wasser. */
static int nennbahn(int g, const uint8_t *r90, int32_t ende[3], int *sicht_out, int *nb_out)
{
    const int32_t c = re15_cos_q12(g), sn = re15_sin_q12(g);
    /* Nachbesserung 3 (G-E2): die Haende stehen vor dem der Kamera zugewandten Werfer, die Bahn laeuft in der Wurfrichtung g */
    const int wg = re15_k2080_werfer_gier();
    const int32_t cw = re15_cos_q12(wg), sw = re15_sin_q12(wg);
    const int32_t start[3] = { RE15_K2080_NPC_WURF_X + (int32_t)(((int64_t)RE15_K2080_HAND_VOR * cw) >> 12), RE15_K2080_Y + RE15_K2080_HAND_HOCH,
                               RE15_K2080_NPC_STAND_Z + (int32_t)((-(int64_t)RE15_K2080_HAND_VOR * sw) >> 12) };   /* RotMatrixY wie vektor_drehen */
    int nb = 0;
    re15_k2080_bahn_rechnen(start, (int16_t)g, RE15_K2080_WASSER_2090, ende, &nb);
    const int32_t x = ende[0], z = ende[2];
    const int innen = x > -8900 && x < 7200 && z > -27000 && z < -5400;
    const int plat = x > -1700 && x < 1850 && z > -20000 && z < -12450;
    const int ramp = x > 1850 && x < 7200 && z > -18100 && z < -14500;
    int sicht = 0;
    if (r90) {
        re15_camera_cut_t c12; memcpy(&c12, r90 + rd32(r90 + 8 + 28) + 12 * 32, 32);
        re15_camera_view_t v;
        if (re15_camera_build_view(&c12, &v) == 0) {
            int32_t q[3] = { start[0], start[1], start[2] }, vel[3], acc[3];
            vel[0] = (int32_t)((25 * (int64_t)c + 10 * (int64_t)sn) >> 12); vel[2] = (int32_t)((-25 * (int64_t)sn + 10 * (int64_t)c) >> 12); vel[1] = 20;
            acc[0] = (int32_t)((5 * (int64_t)c) >> 12); acc[2] = (int32_t)((-5 * (int64_t)sn) >> 12); acc[1] = 20;
            for (int k = 0; k <= nb; k++) {
                const double vx = (v.rot[0] * (double)q[0] + v.rot[1] * (double)q[1] + v.rot[2] * (double)q[2]) / 4096.0 + v.trans[0];
                const double vy = (v.rot[3] * (double)q[0] + v.rot[4] * (double)q[1] + v.rot[5] * (double)q[2]) / 4096.0 + v.trans[1];
                const double vz = (v.rot[6] * (double)q[0] + v.rot[7] * (double)q[1] + v.rot[8] * (double)q[2]) / 4096.0 + v.trans[2];
                if (vz > 0) {
                    const double sx = 160 + vx * v.fov_screen_dist / vz, sy = 120 + vy * v.fov_screen_dist / vz;
                    if (sx >= 0 && sx < 320 && sy >= 0 && sy < 240) sicht++;
                }
                for (int i = 0; i < 3; i++) q[i] += vel[i];
                for (int i = 0; i < 3; i++) vel[i] += acc[i];
            }
        }
    }
    printf("  Nennbahn Gier %d: Start (%d,%d,%d) -> Aufschlag (%d,%d,%d) nach %d Bildern, %d von %d im Bild von 2090 Cut 12\n", g,
           (int)start[0], (int)start[1], (int)start[2], (int)x, (int)ende[1], (int)z, nb, sicht, nb + 1);
    if (sicht_out) *sicht_out = sicht;
    if (nb_out) *nb_out = nb;
    return innen && !plat && !ramp && ende[1] >= RE15_K2080_WASSER_2090 - 800;
}
static int teil_landung(void)
{
    long n90 = 0; uint8_t *r90 = slurp("STAGE2/ROOM2090.RDT", &n90);
    PRUEFE(r90 && n90 > 0x60 + 13 * 32, "ROOM2090.RDT fehlt");
    int32_t lnord[3], lsued[3]; int sicht = 0;
    PRUEFE(nennbahn(RE15_K2080_WURF_GIER_NORD, r90, lnord, &sicht, NULL), "Nordwurf: Aufschlag (%d,%d) nicht im Wasser",
           (int)lnord[0], (int)lnord[2]);
    PRUEFE(sicht >= 10, "Nordwurf: nur %d Fall-Bilder in 2090 Cut 12", sicht);
    PRUEFE(nennbahn(RE15_K2080_WURF_GIER_SUED, r90, lsued, &sicht, NULL), "Suedwurf: Aufschlag (%d,%d) nicht im Wasser",
           (int)lsued[0], (int)lsued[2]);
    PRUEFE(sicht >= 10, "Suedwurf: nur %d Fall-Bilder in 2090 Cut 12", sicht);
    free(r90);
    /* Nachbesserung 2 (N3): Wahl nach der Lage des Alligators beim Ausloesen = der WEITERE Landepunkt; Lagen aus den Laeufen der
     * Abnahme 1 (Dossier 9.3) - der gewaehlte Punkt liegt mindestens 8880 x Zeichen-Skala 2731/4096 = 5920 entfernt (RE2-
     * Zielmodus @0x80100a88-8c), sonst koennte der Alligator ihn nicht aus dem Anlauf holen. */
    const struct { int32_t x, z; int soll; const char *was; } k_gl[] = {
        { -4889,  -7442, RE15_K2080_WURF_GIER_SUED, "kampf/k_leon (Nordbecken West)" },
        {  5782, -26532, RE15_K2080_WURF_GIER_NORD, "v1 natuerlich (Suedost-Ecke)" },
        { -4940,  -9716, RE15_K2080_WURF_GIER_SUED, "v3/v5 Start West (Nordbecken)" },
        {  5716,  -7090, RE15_K2080_WURF_GIER_SUED, "v2/v4 Start (0,-7000) (Nordost)" },
        { -6500, -23500, RE15_K2080_WURF_GIER_NORD, "Kampf im Suedwestbecken" },
    };
    for (unsigned i = 0; i < sizeof k_gl / sizeof k_gl[0]; i++) {
        const int g = re15_k2080_wurf_gier_wahl(35, k_gl[i].x, k_gl[i].z);
        const int32_t *l = (g == RE15_K2080_WURF_GIER_SUED) ? lsued : lnord;
        const int64_t dx = (int64_t)l[0] - k_gl[i].x, dz = (int64_t)l[2] - k_gl[i].z;
        const double d = sqrt((double)(dx * dx + dz * dz));
        printf("  Alligator (%d,%d) %s: Wurf-Gier %d -> Landung (%d,%d), Abstand %.0f\n", (int)k_gl[i].x, (int)k_gl[i].z,
               k_gl[i].was, g, (int)l[0], (int)l[2], d);
        PRUEFE(g == k_gl[i].soll, "%s: Gier %d statt %d", k_gl[i].was, g, k_gl[i].soll);
        PRUEFE(d >= (RE15_K2080_ANSCHWIMM_AB * 2731) / 4096, "%s: Landung nur %.0f vom Alligator", k_gl[i].was, d);
    }
    PRUEFE(re15_k2080_wurf_gier_wahl(-1, 0, 0) == RE15_K2080_WURF_GIER_NORD, "ohne gesicherten Alligator nicht der Nordwurf");
    /* Spielstand mit Bit 116 ohne Laufzeit-Bahn: der Installer legt den Kanister an die Aufschlagstelle der Nennbahn, Bit 117 waehlt
     * Nord/Sued (2090 und 2091) */
    for (int r = 0; r < 4; r++) {
        const int elza_ = r & 1, sued = r >> 1;
        g_gameflow.character = elza_ ? 4 : 0;
        re15_k2080_reset();
        re15_gr2_kanister_entfernen();
        re15_game_flag_set(9, RE15_K2080_BIT_F_SZENE, 1);
        re15_game_flag_set(9, RE15_K2080_BIT_SZENE, 1);
        re15_game_flag_set(9, RE15_K2080_BIT_KANISTER, 1);
        re15_game_flag_set(9, RE15_K2080_BIT_SUEDWURF, sued);
        g_current_room_id = elza_ ? RE15_K2080_RAUM_2091 : RE15_K2080_RAUM_2090;
        re15_kanister2080_r41_install((uint16_t)g_current_room_id);
        int32_t pos[3] = { 0, 0, 0 }, rot[9];
        const int da = re15_gr2_kanister_welt(pos, rot);
        const int32_t *l = sued ? lsued : lnord;
        printf("  ROOM%04X Bit 117 = %d: Kanister %s bei (%d,%d,%d), Stufe %d\n", (unsigned)g_current_room_id, sued,
               da ? "liegt" : "FEHLT", (int)pos[0], (int)pos[1], (int)pos[2], re15_gr2_kanister_stufe());
        PRUEFE(da && pos[0] == l[0] && pos[1] == RE15_K2080_GATOR_WASSER_Y && pos[2] == l[2] && re15_gr2_kanister_stufe() == 0,
               "ROOM%04X Bit 117 = %d: Kanister nicht an der Aufschlagstelle", (unsigned)g_current_room_id, sued);
    }
    re15_gr2_kanister_entfernen();
    for (int b = 110; b <= 117; b++) re15_game_flag_set(9, (uint8_t)b, 0);
    g_gameflow.character = 0;
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
/* ---- kippen (Nachtrag 26, RE2-Kippen beim Greifen) ----------------------------------------------------------------- */
static int teil_kippen(void)
{
    /* unabhaengig: RE2 ROOM40A0 sub10 - Work_set loescht v/a (@0x80055904), a[5] = -4 (@0x066E), For 23 { Add_speed (Winkel += v)
     * @0x067C, Add_aspeed (v += a) @0x067D }, danach Member_set rz = 0xfc00 (@0x0682) */
    int w = 0, v = 0;
    int alt = -1;
    for (int k = 1; k <= 24; k++) {
        if (k <= 23) { w += v; v += 4; }
        const int soll = (k <= 23) ? w * 4096 / 1024 : 4096;
        const int ist = re15_k2080_kipp_bruch(k);
        PRUEFE(ist == soll, "Kippen Bild %d: Bruch %d statt %d (RE2 Winkel %d)", k, ist, soll, w);
        PRUEFE(ist >= alt, "Kippen Bild %d: Bruch faellt (%d < %d)", k, ist, alt);
        alt = ist;
    }
    printf("  Kippen: Bild 1 %d, Bild 12 %d, Bild 23 %d (RE2-Winkel 1012/1024), Bild 24 %d (Member_set -1024)\n",
           re15_k2080_kipp_bruch(1), re15_k2080_kipp_bruch(12), re15_k2080_kipp_bruch(23), re15_k2080_kipp_bruch(24));
    PRUEFE(re15_k2080_kipp_bruch(0) == 0 && re15_k2080_kipp_bruch(1) == 0, "Kippen beginnt nicht bei 0 (hochkant)");
    PRUEFE(re15_k2080_kipp_bruch(30) == 4096, "Kippen endet nicht quer");
    return fails != 0;
}

/* ---- hals (Nachtrag 26, RE2-Halsfuehrung FUN_800177c0) ---------------------------------------------------------------- */
static int teil_hals(void)
{
    re15_k2080_reset();
    /* Slew: hoechstens 20 je Bild (+0x9C/+0x9E @0x801005a0-ac), im Fenster genau das Ziel */
    int16_t y = 0, n = 0;
    for (int f = 1; f <= 40; f++) {
        re15_k2080_hals_schritt(560, -312);
        re15_k2080_hals_lesen(&y, &n);
        const int sy = f * 20 < 560 ? f * 20 : 560, sn = f * 20 < 312 ? -f * 20 : -312;
        PRUEFE(y == sy && n == sn, "Bild %d: Akku (%d,%d) statt (%d,%d)", f, (int)y, (int)n, sy, sn);
    }
    for (int f = 1; f <= 40; f++) re15_k2080_hals_schritt(0, 0);       /* SELBST-Zweig: Akku -> 0 */
    re15_k2080_hals_lesen(&y, &n);
    PRUEFE(y == 0 && n == 0, "Akku nach dem Zielmodus nicht 0 (%d,%d)", (int)y, (int)n);
    /* Ziel: geradeaus (Welt-Gier = Gier des Aktors) -> Gier-Residual 0; 90 Grad daneben -> Klemme 560; tief unten -> Klemme 312 */
    int fehler_y = 0, fehler_k = 0;
    for (int r = 0; r < 4096; r += 512) {
        /* Richtung, deren Welt-Gier r ist (Gier-Konvention des Ports: (re15_atan2_q12(dz,dx) - 1024) & 0xfff, re15_actor.h:1196;
         * re15_atan2_q12 = FUN_8001a6d4 + 0x400 = 0x400 - catan(dz/dx) -> Richtung (cos(-r), sin(-r))) */
        const int32_t dx = (int32_t)((int64_t)re15_cos_q12((-r) & 0xfff) * 6000 >> 12);
        const int32_t dz = (int32_t)((int64_t)re15_sin_q12((-r) & 0xfff) * 6000 >> 12);
        {   const int gi = ((int)re15_atan2_q12(dz, dx) - 1024) & 0xfff, d = ((gi - r + 0x800) & 0xfff) - 0x800;
            PRUEFE(d >= -8 && d <= 8, "Richtungs-Konvention: Gier %d statt %d", gi, r); }
        int zy = 99, zp = 99;
        re15_k2080_hals_ziel(dx, 0, dz, r, 0, 0, 0, &zy, &zp);
        if (zy < -8 || zy > 8) fehler_y++;
        re15_k2080_hals_ziel(dx, 0, dz, (r + 1024) & 0xfff, 0, 0, 0, &zy, &zp);   /* Ziel 90 Grad zur Seite */
        if (zy != 560 && zy != -560) fehler_k++;
    }
    PRUEFE(fehler_y == 0, "%d von 8 Richtungen: Gier-Residual geradeaus nicht ~0", fehler_y);
    PRUEFE(fehler_k == 0, "%d von 8 Richtungen: 90 Grad daneben nicht auf die Klemme 560 begrenzt", fehler_k);
    int zy = 0, zp = 0;
    re15_k2080_hals_ziel(0, 20000, 100, 0, 0, 0, 0, &zy, &zp);           /* Ziel 20000 unter dem Hals, fast senkrecht */
    PRUEFE(zp == 312 || zp == -312, "steiles Ziel: Neigung %d nicht auf der Klemme 312", zp);
    re15_k2080_hals_ziel(0, 0, 6000, 0, 0, 100, -50, &zy, &zp);           /* Keyframe-Winkel werden abgezogen */
    {   int zy0 = 0, zp0 = 0; re15_k2080_hals_ziel(0, 0, 6000, 0, 0, 0, 0, &zy0, &zp0);
        PRUEFE(zy == zy0 - 100 && zp == zp0 + 50, "Akku-Ziel zieht die Keyframe-Winkel nicht ab (%d,%d) vs (%d,%d)", zy, zp, zy0, zp0); }
    printf("  Hals: Slew 20/Bild bis (560,-312), zurueck auf 0; Ziel geradeaus Residual 0, seitlich Klemme 560, steil Klemme 312\n");
    re15_k2080_reset();
    return fails != 0;
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

/* ---- blick (Nachbesserung 3, Abnahme 2 M2, Orchestrator-Entscheid G-E2) -------------------------------------------------
 * RE2 ROOM7020 Cut 7 (info/re2leon/PL0/RDT/ROOM7020.RDT, selbst gelesen): Ada Pos_set `32 00 a0 92 b4 e2 14 ba` @SCD 0x0740 =
 * (-28000,-7500,-17900), Dir_set `33 00 00 00 74 0e 00 00` @0x0748 = 3700, Kamera Cut 7 RID @RDT 0x144 = (-22158,-3852,-15174).
 * Unabhaengig nachgerechnet: Gier Ada -> Kamera, Versatz = 3700 - Gier (Mesh-Gier zur Richtung (dx,dz) = atan2(dx,dz) - 1024, 4096
 * je Umlauf) muss RE15_K2080_ADA_BLICK_VERSATZ ergeben; der Port-Werfer am Wurf-Stand blickt zur RID-Kamera Cut 7 von ROOM2080
 * und ROOM2081 (aus der Datei) mit demselben Versatz = RE15_K2080_WERFER_GIER_NENN, frontal = cos(Versatz) wie Ada. */
static double gier_zu(double dx, double dz) { double g = atan2(dx, dz) * 4096.0 / (2.0 * 3.14159265358979) - 1024.0; while (g < 0) g += 4096; return fmod(g, 4096.0); }
static int teil_blick(void)
{
    const double ada = gier_zu(-22158.0 - -28000.0, -15174.0 - -17900.0);
    double vers = 0x0e74 - ada; while (vers > 2048) vers -= 4096; while (vers < -2048) vers += 4096;
    printf("  RE2 Ada: Gier zur Kamera %.1f, Dir_set 3700, Versatz %.1f (frontal %.3f)\n", ada, vers, cos(vers * 2 * 3.14159265358979 / 4096));
    PRUEFE((int)lround(vers) == RE15_K2080_ADA_BLICK_VERSATZ, "Ada-Versatz %.1f statt %d", vers, RE15_K2080_ADA_BLICK_VERSATZ);
    for (int r = 0; r < 2; r++) {
        long n = 0; uint8_t *d = slurp(r ? "STAGE2/ROOM2081.RDT" : "STAGE2/ROOM2080.RDT", &n);
        PRUEFE(d && n > 0x200, "ROOM208%d.RDT fehlt", r);
        if (!d) continue;
        const uint8_t *k = d + rd32(d + 8 + 7 * 4) + RE15_K2080_CUT_WURF * 32;
        const double kx = (double)(int32_t)rd32(k + 4), kz = (double)(int32_t)rd32(k + 12);
        const double zur = gier_zu(kx - RE15_K2080_NPC_WURF_X, kz - RE15_K2080_NPC_STAND_Z);
        int soll = (int)lround(zur) + RE15_K2080_ADA_BLICK_VERSATZ; soll &= 0xfff;
        double fr = cos((RE15_K2080_WERFER_GIER_NENN - zur) * 2 * 3.14159265358979 / 4096);
        printf("  ROOM208%d Cut %d Kamera (%.0f,%.0f): Gier zur Kamera %.1f -> Werfer %d (Nenn %d), frontal %.3f\n", r,
               RE15_K2080_CUT_WURF, kx, kz, zur, soll, RE15_K2080_WERFER_GIER_NENN, fr);
        PRUEFE(abs(soll - RE15_K2080_WERFER_GIER_NENN) <= 1, "ROOM208%d: Werfer-Gier %d statt Nenn %d", r, soll, RE15_K2080_WERFER_GIER_NENN);
        PRUEFE(fr >= 0.98, "ROOM208%d: frontal %.3f < Ada 0,985", r, fr);
        free(d);
    }
    return fails != 0;
}

/* Nachbesserung 4 (Abnahme G 3 M1): Rangfolge der R1-Kamerawahl (gator_re2_r41.c re15_gr2_r1_rang, Entscheid G-E1 "Kopf und
 * Kiefer ... im Bild ... unverdeckt ... am naechsten an der RE2-Lage"). Fall A = die Kandidatentabelle des Abnahme-Laufs
 * plat_leon (gator_boss.log F1278, Leon auf der Plattform): der laufende Cut 4 (Start 8/8, Todes-Clip nur 6/19 - ab F1290 liegt der
 * Kopf hinter dem Maschinenblock) gegen Cut 3 (8/8, 19/19, Massstab 2158 > Grenze 1892). Vorher waehlte die Rangfolge (gueltiger
 * Start vor den Proben, Grenze in Stufe 0) den Cut 4; jetzt muss Cut 3 kommen (Stufe 1: voller Todes-Clip vor der Grenze).
 * Faelle B-D: zwei volle Cuts -> der naeher an RE2 (170,111); voll unter der Grenze vor voll darueber (Entscheid B8);
 * kein voller Cut -> die meisten Proben. */
static int teil_r1wahl(void)
{
    const int32_t grenze = 1892;                              /* gr2_massstab_grenze() (Messzeile plat_leon "grenze=1892") */
    /* cut, start_ok, im, bilder, kopf_ok, kiefer_ok, kx, ky, m - Abnahme 3 plat_leon F1278 */
    const re15_gr2_r1_kand_t a[8] = {
        { 0, 0, 0,  0, 0, 0,   967,   138, 2218 }, { 1, 0, 0,  0, 0, 0,    27,    92,  523 },
        { 2, 0, 2, 16, 1, 1,   163,   104,  561 }, { 3, 1, 8, 19, 1, 1,    43,   156, 2158 },
        { 4, 1, 8,  6, 1, 1,   128,    99,  669 }, { 5, 0, 0,  0, 0, 0, -9999, -9999,    0 },
        { 6, 0, 0,  0, 0, 0,  -271,   135, 1516 }, { 7, 0, 0,  0, 0, 0,   278,   102,  508 } };
    int st = -1;
    int i = re15_gr2_r1_rang(a, 8, 4, grenze, 19, &st);
    printf("r1wahl A (plat_leon F1278, laufender Cut 4 mit 6/19): Wahl Cut %d Stufe %d\n", i >= 0 ? a[i].cut : -1, st);
    PRUEFE(i >= 0 && a[i].cut == 3 && st == 1, "Fall A: Cut %d Stufe %d statt Cut 3 Stufe 1 (voller Todes-Clip)", i >= 0 ? a[i].cut : -1, st);
    /* B: zwei volle Cuts unter der Grenze -> naeher an (170,111) */
    const re15_gr2_r1_kand_t b[2] = { { 0, 1, 8, 19, 1, 1, 60, 150, 900 }, { 1, 1, 8, 19, 1, 1, 175, 105, 1200 } };
    i = re15_gr2_r1_rang(b, 2, 0, grenze, 19, &st);
    PRUEFE(i == 1 && st == 0, "Fall B: Index %d Stufe %d statt 1/0 (naeher an RE2)", i, st);
    /* C: voll unter der Grenze vor voll darueber, auch wenn dieser naeher an RE2 liegt (B8) */
    const re15_gr2_r1_kand_t c[2] = { { 0, 1, 8, 19, 1, 1, 60, 150, 900 }, { 1, 1, 8, 19, 1, 1, 170, 111, 2500 } };
    i = re15_gr2_r1_rang(c, 2, 5, grenze, 19, &st);
    PRUEFE(i == 0 && st == 0, "Fall C: Index %d Stufe %d statt 0/0 (Grenze bevorzugt)", i, st);
    /* D: kein voller Cut -> die meisten Proben (vor dem gueltigen Start) */
    const re15_gr2_r1_kand_t d[2] = { { 0, 1, 8, 10, 1, 1, 170, 111, 900 }, { 1, 0, 3, 17, 1, 1, 60, 150, 900 } };
    i = re15_gr2_r1_rang(d, 2, 0, grenze, 19, &st);
    PRUEFE(i == 1 && st == 2, "Fall D: Index %d Stufe %d statt 1/2 (meiste Proben)", i, st);
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
    else if (!strcmp(t, "landung")) rc = teil_landung();
    else if (!strcmp(t, "kippen")) rc = teil_kippen();
    else if (!strcmp(t, "hals")) rc = teil_hals();
    else if (!strcmp(t, "ebene")) rc = teil_ebene();
    else if (!strcmp(t, "blick")) rc = teil_blick();
    else if (!strcmp(t, "r1wahl")) rc = teil_r1wahl();   /* Nachbesserung 4 (M1) */
    else printf("unbekannter Teil %s\n", t);
    printf("%s: %s (%d Fehler)\n", t, rc ? "FAIL" : "OK", fails);
    return rc;
}
