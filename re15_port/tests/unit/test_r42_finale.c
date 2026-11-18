/* test_r42_finale.c — Runde 42 Strang finale, Stufe 1 (include/re15_finale_r42.h, include/re2_buehne.h, include/re2_fx.h).
 *
 *   buehne   S3: Schuettel-Tabellen bytegleich mit info/re2leon/PSX.EXE (Datei = Adresse - 0x80010000 + 0x800): Modus 1
 *            @0x8009daf4 = 0x8E2F4, Modus 2 @0x8009db14 = 0x8E314 (je 32 Byte); Takt (idx + 1) & 0xf @0x8002aa90-a4; VSync.
 *   besitzer S2: FUN_8001c8c4 schreibt +0x1D (Besitzer) und +0x22 (Gier) in alle Plaetze des Skripts; FUN_8001cfbc loescht genau
 *            die Plaetze eines Besitzers; FUN_8001cefc(2,0x0D,0) loescht alle Raketen und sonst nichts (Negativ-Kontrollen).
 *   wurzel   S4: Start-HP 20200 in 5090/5091 (Bahnsteig 5080 bleibt), Merker bei hp < 20000, Untergrenze 10000, "passend" nur im
 *            Gehzustand ab Abstand 2801, Uhr genau 1800 Bilder, Wurf-Ausloeser wie sub11 (einmal, nur mit "passend").
 *   programm S6: das Port-Programm = ROOM7020 sub15 (Sleep 15, Zeile, 33, Signal FREI, 32 + 5, Signal W1, Ende), Zeile Id 2
 *            "Leon: Game over." mit Halt 33.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_room.h"
#include "re15_scd.h"
#include "re15_finale_r42.h"
#include "re2_buehne.h"
#include "re2_fx.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

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

static void t_buehne(void)
{
    size_t n = 0;
    uint8_t *exe = slurp(RE15_REPO_ROOT "/info/re2leon/PSX.EXE", &n);
    CHECK(exe && n > 0x8E334, "info/re2leon/PSX.EXE fehlt");
    if (exe && n > 0x8E334) {
        CHECK(memcmp(re2b_schuettel_tabelle(1), exe + 0x8E2F4, 32) == 0, "Modus-1-Tabelle != PSX.EXE @0x8E2F4 (0x8009daf4)");
        CHECK(memcmp(re2b_schuettel_tabelle(2), exe + 0x8E314, 32) == 0, "Modus-2-Tabelle != PSX.EXE @0x8E314 (0x8009db14)");
    }
    free(exe);
    re2b_reset();
    CHECK(re2b_vsync() == 2, "Raumstart VSync %d statt 2", re2b_vsync());
    re2b_vsync_setzen(5);
    CHECK(re2b_vsync() == 5, "VSync 5 nicht gesetzt");
    const uint8_t *t2 = re2b_schuettel_tabelle(2);
    int dx, dy, i;
    /* 16 Schritte Modus 2 laufen die Tabelle einmal durch (Index-Start egal, Folge ist zyklisch) */
    re2b_schuettel_takt(RE2B_SCHUETTEL_LEICHT);
    re2b_schuettel_offset(&dx, &dy);
    int start = -1;
    for (i = 0; i < 16; i++) if (t2[2 * i] == dx && t2[2 * i + 1] == dy && start < 0) start = i;
    CHECK(start >= 0 && re2b_schuettel_modus() == 2, "Modus 2: (%d,%d) nicht in der Tabelle", dx, dy);
    re2b_schuettel_takt(0);
    re2b_schuettel_offset(&dx, &dy);
    CHECK(dx == 0 && dy == 0 && re2b_schuettel_modus() == 0, "ohne Bit kein Schuetteln");
    re2b_reset();
}

static int platz_besitzer_zahl(uint8_t besitzer, int *lebend)
{
    int z = 0, l = 0;
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        const uint8_t *b = re2fx_platz(i);
        const int st = b[0x18] | (b[0x19] << 8);
        if (st) l++;
        if (st && b[0x1D] == besitzer) z++;
    }
    if (lebend) *lebend = l;
    return z;
}

static void t_besitzer(void)
{
    size_t n = 0;
    uint8_t *esp = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/../RE2/CORE00.ESP", &n);
    CHECK(esp != NULL, "CORE00.ESP fehlt");
    if (!esp) return;
    CHECK(re2fx_register_core(esp, n) == 0, "CORE00.ESP nicht registriert");
    const int16_t ofs[4] = { 0, 1100, 0, 0 };
    const int a = re2fx_spawn_besitzer(0x020D1000u, 0x80, 1234, re2fx_einheitsmatrix, ofs);   /* Rakete, Besitzer 0x80 */
    const int b = re2fx_spawn_sofort(0x020D1000u, 1234, re2fx_einheitsmatrix, ofs);           /* Rakete ohne Besitzer */
    const int c = re2fx_spawn_sofort(0x030A1A00u, 0, re2fx_einheitsmatrix, ofs);              /* Rauch 3/0x0A */
    CHECK(a >= 0 && a < RE2FX_PLAETZE && b >= 0 && b < RE2FX_PLAETZE && c >= 0 && c < RE2FX_PLAETZE, "Spawns %d %d %d", a, b, c);
    if (a < 0 || a >= RE2FX_PLAETZE || b < 0 || b >= RE2FX_PLAETZE || c < 0 || c >= RE2FX_PLAETZE) { free(esp); return; }
    const uint8_t *pa = re2fx_platz(a), *pb = re2fx_platz(b);
    CHECK(pa[0x1C] == 2 && pa[0x1D] == 0x80 && pa[0x1E] == 0x0D, "Besitzer-Platz Packwort %02x %02x %02x (Soll 02 80 0d, @0x8001c964-80)",
          pa[0x1C], pa[0x1D], pa[0x1E]);
    CHECK((int16_t)(pa[0x22] | (pa[0x23] << 8)) == 1234, "Gier +0x22 (@0x8001c988-8c)");
    CHECK(pb[0x1D] == 0, "ohne Besitzer +0x1D = %02x", pb[0x1D]);
    int lebend0 = 0, lebend1 = 0;
    const int z80 = platz_besitzer_zahl(0x80, &lebend0);
    CHECK(z80 >= 1, "keine Plaetze mit Besitzer 0x80");
    re2fx_loesche_besitzer(0x81);                                         /* fremder Besitzer: nichts */
    (void)platz_besitzer_zahl(0x80, &lebend1);
    CHECK(lebend1 == lebend0, "FUN_8001cfbc(0x81) loeschte %d Plaetze", lebend0 - lebend1);
    re2fx_loesche_besitzer(0x80);
    CHECK(platz_besitzer_zahl(0x80, &lebend1) == 0, "Besitzer 0x80 nicht geloescht");
    CHECK(lebend1 == lebend0 - z80, "FUN_8001cfbc(0x80) loeschte %d statt %d", lebend0 - lebend1, z80);
    CHECK((pb[0x18] | (pb[0x19] << 8)) != 0, "Rakete ohne Besitzer mitgeloescht");
    re2fx_loesche_bank_sub(2, 0x0D, NULL);
    CHECK((pb[0x18] | (pb[0x19] << 8)) == 0, "FUN_8001cefc(2,0x0D,0) liess die Rakete stehen");
    const uint8_t *pc = re2fx_platz(c);
    CHECK((pc[0x18] | (pc[0x19] << 8)) != 0, "FUN_8001cefc(2,0x0D,0) loeschte den Rauch 3/0x0A");
    re2fx_loesche_bank_sub(3, 0x0A, NULL);
    CHECK((pc[0x18] | (pc[0x19] << 8)) == 0, "FUN_8001cefc(3,0x0A,0) liess den Rauch stehen");
    re2fx_reset();
    free(esp);
}

static void t_wurzel(void)
{
    re15_actor_t *e = &g_actors[3];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x30;
    g_current_room_id = 0x5080u;
    CHECK(re15_finale_r42_start_hp(1000) == 1000, "5080: Start-HP %d statt 1000", re15_finale_r42_start_hp(1000));
    g_current_room_id = 0x5090u;
    re15_finale_r42_install(0x5090u);
    CHECK(re15_finale_r42_start_hp(1000) == 20200, "5090: Start-HP %d statt 20200", re15_finale_r42_start_hp(1000));
    e->hp = 20200;
    re15_finale_r42_form_takt(3, 1, 2800);
    CHECK(!re15_finale_r42_genug() && !re15_finale_r42_passend(), "20200 / 2800: genug %d passend %d",
          re15_finale_r42_genug(), re15_finale_r42_passend());
    re15_finale_r42_form_takt(3, 1, 2801);
    CHECK(re15_finale_r42_passend(), "Gehen + 2801 nicht passend");
    re15_finale_r42_form_takt(3, 0, 9000);
    CHECK(!re15_finale_r42_passend(), "ohne Gehzustand passend");
    e->hp = 20200 - 201;
    re15_finale_r42_form_takt(3, 0, 9000);
    CHECK(re15_finale_r42_genug(), "Treffer 201 setzt den Merker nicht");
    e->hp = 9699;
    re15_finale_r42_form_takt(3, 0, 9000);
    CHECK(e->hp == 10000, "Untergrenze: hp %d statt 10000", e->hp);
    CHECK(re15_finale_r42_uhr() == -1 && !re15_finale_r42_wurf_faellig(), "Uhr vor dem Kampf");
    re15_finale_r42_kampf_start_test();
    for (int i = 0; i < 1799; i++) re15_finale_r42_form_takt(3, 0, 9000);
    CHECK(re15_finale_r42_uhr() == 1, "Uhr nach 1799 Bildern %d statt 1", re15_finale_r42_uhr());
    CHECK(!re15_finale_r42_wurf_faellig(), "Wurf ohne passend");
    re15_finale_r42_form_takt(3, 0, 9000);
    CHECK(re15_finale_r42_uhr() == 0, "Uhr nach 1800 Bildern nicht abgelaufen");
    re15_finale_r42_form_takt(3, 1, 3000);
    CHECK(re15_finale_r42_wurf_faellig(), "Wurf faellig fehlt (Uhr ab + passend)");
    g_current_room_id = 0x5080u;
    re15_finale_r42_install(0x5080u);
    CHECK(re15_finale_r42_uhr() == -1 && !re15_finale_r42_wurf_faellig(), "Raumwechsel loescht den Zustand nicht");
}

/* S1: R702_XA03_LEON.XAS = die Kanal-5-Sektoren von VOICE07.XAS im Lesefenster [5, 5 + 1026) Schritt 16 (STAGE7.BIN @0x156
 * `05 00 00 00 42 00`, tools/r42_finale/re2_xa_finale.py), Subheader Datei 1 / Kanal 5 / Submode 0x64. */
static void t_xa03(void)
{
    size_t nx = 0, na = 0;
    uint8_t *x = slurp(RE15_REPO_ROOT "/info/re2leon/PL0/VOICE/VOICE07.XAS", &nx);
    uint8_t *a = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/../RE2/" RE15_FI_XA03_DATEI, &na);
    CHECK(x && a, "VOICE07.XAS oder %s fehlt", RE15_FI_XA03_DATEI);
    if (x && a) {
        const size_t sek = 2336, n = (RE15_FI_XA03_LESE + 15) / 16;     /* Sektoren 5, 21, .., 5 + 16 * 64 */
        CHECK(na == n * sek, "%s: %u Byte statt %u", RE15_FI_XA03_DATEI, (unsigned)na, (unsigned)(n * sek));
        for (size_t i = 0; i < n && na == n * sek; i++) {
            const uint8_t *q = x + (5 + 16 * i) * sek;
            CHECK(memcmp(a + i * sek, q, sek) == 0 && q[1] == 5 && q[2] == 0x64, "Sektor %u nicht bytegleich", (unsigned)i);
        }
    }
    free(x); free(a);
}

static void t_programm(void)
{
    int n = 0;
    const uint8_t *p = re15_finale_r42_programm(0, &n);
    static const uint8_t soll[] = {
        0x54, 0x01, 0x02, 0x00, 0x00, 0x00, 0x22, 0x05, 0x14, 0x01,   /* + Set(5,0x14,1) @0x01A28 (Runde 2) */
        0x09, 0x0a, 15, 0x00, 0x2b, RE15_FI_MSG_ZEILE_LEON, 0x00, 0x00, 0x09, 0x0a, 33, 0x00,
        0x22, 0x05, RE15_FI_SIG_FREI, 0x01, 0x09, 0x0a, 32, 0x00, 0x09, 0x0a, 5, 0x00, 0x22, 0x05, RE15_FI_SIG_W1, 0x01, 0x01, 0x00 };
    CHECK(n == (int)sizeof soll && memcmp(p, soll, sizeof soll) == 0, "Programm weicht von ROOM7020 sub15 ab (%d Byte)", n);
    /* RE2-Zahlen: 15 (@0x01A30), 33 = ceil(162 / 150 * 30) (STAGE7.BIN @0x150), 32 + 5 (@0x01A60/@0x01A64) */
    p = re15_finale_r42_programm(1, &n);
    CHECK(n == (int)sizeof soll && p[20] == RE15_FI_ZEILE_BILDER_ELZA && memcmp(p, soll, 20) == 0 &&
          memcmp(p + 21, soll + 21, sizeof soll - 21) == 0, "Elza-Programm weicht von ROOM7021 sub15 ab");
    CHECK(RE15_FI_SIG_FREI != 0x14u && RE15_FI_SIG_W1 != 0x14u, "Port-Signal belegt das RE2-Bit (5,0x14) von sub15 @0x01A28");
    CHECK((354 * 30 + 149) / 150 == RE15_FI_ZEILE_BILDER_ELZA, "Elza-Fenster");
    CHECK(RE15_FI_SCHLAF_ZEILE + RE15_FI_ZEILE_BILDER == 48 && RE15_FI_SCHLAF_W1A + RE15_FI_SCHLAF_W1B == 37, "Takt T->U / U->W1");
    CHECK((162 * 30 + 149) / 150 == RE15_FI_ZEILE_BILDER, "Lesefenster in Bildern");
}

/* R1 (Pruefbericht s1 r1): das Kill-Bild haengt an der ECHTZEIT des Xa-0x03-Fensters (Bit 0x20 @0x80103d64-74, geloescht vom CD-Zweig
 * @0x80012f7c, 150 Sektoren/s), nicht an 30 Bildern/s. Bei VSync 5 (@0x80103b00-08) ist es das 83. Bild (Leon, 6,92 s) bzw. das 99.
 * (Elza); die alte Rechnung (206 / 247 Bilder) gilt nur bei VSync 2. */
static int kill_bild(int vsync, int sektoren)
{
    uint32_t vbl = 0;
    for (int n = 0; n < 2000; n++) {
        if (n > 0) vbl += (uint32_t)vsync;
        if (re15_finale_r42_xa_aus(vbl, sektoren)) return n;
    }
    return -1;
}
static void t_xa_ende(void)
{
    CHECK(kill_bild(5, RE15_FI_XA03_LESE) == 83, "Leon: Kill-Bild %d statt 83 bei VSync 5", kill_bild(5, RE15_FI_XA03_LESE));
    CHECK(kill_bild(5, RE15_FI_XA03_LESE_ELZA) == 99, "Elza: Kill-Bild %d statt 99", kill_bild(5, RE15_FI_XA03_LESE_ELZA));
    CHECK(kill_bild(2, RE15_FI_XA03_LESE) == 206, "Gegenprobe VSync 2: %d statt 206", kill_bild(2, RE15_FI_XA03_LESE));
    CHECK(!re15_finale_r42_xa_aus(410, RE15_FI_XA03_LESE) && re15_finale_r42_xa_aus(411, RE15_FI_XA03_LESE), "Grenze 410,4 Austastungen");
}

/* U3: Schnitte aus Wagen C (Kameras 11-15 von ROOM5090, RID @0x60 + 32*Schnitt) - W1/W2/W3 verschieden (RE2 8/9/10), der Wechsel
 * "Game over" -> W1 ist ein Schnitt (RE2 11 -> 8); 5091 hat dieselben Kameras (bytegleich @0x60-0x25F). */
static void t_schnitte(void)
{
    const int z = RE15_FI_CUT_ZEILE, a = RE15_FI_CUT_W1, b = RE15_FI_CUT_W2, c = RE15_FI_CUT_W3;
    CHECK(z >= 11 && z <= 15 && a >= 11 && a <= 15 && b >= 11 && b <= 15 && c >= 11 && c <= 15, "Schnitt ausserhalb Wagen C");
    CHECK(a != b && b != c && a != c && z != a, "Schnittfolge ohne Wechsel (%d %d %d %d)", z, a, b, c);
    size_t n0 = 0, n1 = 0;
    uint8_t *r0 = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/STAGE5/ROOM5090.RDT", &n0);
    uint8_t *r1 = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/STAGE5/ROOM5091.RDT", &n1);
    CHECK(r0 && r1 && n0 > 0x260 && n1 > 0x260 && memcmp(r0 + 0x60, r1 + 0x60, 0x200) == 0, "5090/5091 Kameras nicht gleich");
    free(r0); free(r1);
}

/* U5: Zuordnung gegen die Bytes des Tyrant-Overlays (CDEMD0.EMS, EM2B ai1 ab Datei 0x48A800 = 0x80100000): Tabelle @0x8010476c
 * (Teil +0x00, Marke +0x12, 20 Byte je Eintrag) und die Brocken-Teile = a1 der sechs `jal 0x8010158c` (`move a1,zero` @0x80103c14,
 * `addiu a1,zero,n` @0x80103c20/2c/38/44/50). Jeder RE2-Brocken hat mindestens einen Form-Teil, die Form-Teile sind < 32
 * (BAHNSTEIG_FORM.EMD) und paarweise verschieden; alle 21 RE2-Teile (Gerippe EM2B) kommen vor. */
static void t_zerplatzen(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_XSTR(RE15_ASSETS_PATH) "/../RE2/CDEMD0.EMS", &n);
    const size_t ov = 0x48A800u;
    CHECK(ems && n > ov + 0x4D00, "CDEMD0.EMS fehlt");
    int benutzt[32] = { 0 }, re2_da[32] = { 0 }, i, brocken_re2[6];
    if (ems && n > ov + 0x4D00) {
        static const uint32_t stelle[6] = { 0x3c14, 0x3c20, 0x3c2c, 0x3c38, 0x3c44, 0x3c50 };
        for (int j = 0; j < 6; j++) {
            const uint8_t *w = ems + ov + stelle[j];
            const uint32_t ins = (uint32_t)w[0] | (uint32_t)w[1] << 8 | (uint32_t)w[2] << 16 | (uint32_t)w[3] << 24;
            brocken_re2[j] = (ins == 0x00002821u) ? 0 : ((ins >> 16) == 0x2405u ? (int)(ins & 0xffff) : -1);   /* move a1,zero / addiu a1,zero */
        }
    } else for (int j = 0; j < 6; j++) brocken_re2[j] = -2;
    for (i = 0; re15_finale_r42_teil(i, NULL, NULL, NULL); i++) {
        int re2 = -1, form = -1, marke = -1;
        (void)re15_finale_r42_teil(i, &re2, &form, &marke);
        CHECK(form >= 0 && form < 32 && !benutzt[form], "Form-Teil %d doppelt/ungueltig", form);
        if (form >= 0 && form < 32) benutzt[form] = 1;
        if (re2 >= 0 && re2 < 32) re2_da[re2]++;
        if (!ems || n <= ov + 0x4D00) continue;
        if (i < RE15_FI_TEILE_FLUG) {
            const uint8_t *e = ems + ov + 0x476C + 20 * i;
            CHECK(re2 == (e[0] | e[1] << 8) && marke == (e[0x12] | e[0x13] << 8), "Tabelle @0x8010476c Eintrag %d: %d/%d", i, re2, marke);
        } else {
            int ok = 0;
            for (int j = 0; j < 6; j++) if (brocken_re2[j] == re2) ok = 1;
            CHECK(ok, "Brocken-Eintrag %d: RE2-Teil %d ist kein a1 von FUN_8010158c", i, re2);
        }
    }
    for (int j = 0; j < 6; j++) CHECK(brocken_re2[j] >= 0 && re2_da[brocken_re2[j]], "RE2-Brocken %d ohne Form-Teil", brocken_re2[j]);
    for (int r = 0; r < 21; r++) CHECK(re2_da[r] > 0, "RE2-Teil %d ohne Zuordnung", r);
    CHECK(i >= RE15_FI_TEILE_FLUG + RE15_FI_TEILE_BROCKEN, "zu wenige Eintraege (%d)", i);
    free(ems);
}

int main(void)
{
    t_buehne();
    t_besitzer();
    t_wurzel();
    t_programm();
    t_xa03();
    t_xa_ende();
    t_schnitte();
    t_zerplatzen();
    if (fails) { fprintf(stderr, "test_r42_finale: %d Fehler\n", fails); return 1; }
    printf("test_r42_finale: alles gruen\n");
    return 0;
}
