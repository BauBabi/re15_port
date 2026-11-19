/* test_r42_ton_gesetz.c — Strang "ton", Stufe 1 (2026-11-19): das Lautstaerke-Gesetz der libsnd.
 *
 * Nutzer 19.11.2026: "Bei der PC Version klingen die Sounds irgendwie dumpfer/anders als in der PSX Version."
 * Messbericht U1: der Port rechnete die Stimmen-Lautstaerke linear mit pauschalem Abzug; das Original rechnet in
 * SpuVmKeyOnNow @0x80055d10-0x80056070 ein Produkt, drei Pan-Stufen und das QUADRAT (re15_svm_vol.h).
 *
 * Der Riegel rechnet mit den BANKDATEN des ausgelieferten Baums (shared_assets/PSX) und der belegten Formel und
 * vergleicht mit den SPU-Lautstaerke-Registern, die im LAUFENDEN ORIGINAL gemessen wurden (DuckStation-Savestates
 * des Messberichts, Werkzeug ss_spu.py; die Register stehen als Sollwerte hier im Quelltext):
 *   Rolltor-Rumpeln  ROOM1130 snd0 Satz 10 (Ton 7, vol 127), Zweig ohne Lage      16383/16383
 *   Rolltor-Stopp    ROOM1130 snd0 Satz 11 (Ton 8, vol 113), Vormerk-Satz 113/113  10268/10268 (Rechnung; Satz gemessen)
 *   Schalter-Klick   Ton-vol 110, Vormerk-Satz 110/110                              9219/ 9219
 *   Pistolenschuss   ARMS03 Satz 0 (Ton 1 + 2, vol 90), Lage 106/95                 5731/ 4691 je Schicht
 *   Zombie-Laut      ROOM1140 snd1 Ton 4 (vol 127), Lage 99/62 und 102/65            9955/ 4012 und 10567/4259
 *   Schritt (Rotor-Platz) ROOM1170 snd0 Ton 1 (vol 60), Lage 80/90                   1450/ 1836
 *   Rotor            SUB_15 Programm 0 (mvol 80) Ton 0/1 (vol 127), Anschlag 127     6500/ 6500
 *   Musik MAIN17     Programm 0 Ton 0 (Bank 100, Programm 110, Ton 50): Anschlag 115/90/125 bei Kanal 90 -> 480/290/566;
 *                    Programm 3 (Ton 80), Anschlag 127, Kanal 127 -> 3023; Programm 5 (Ton 80), Anschlag 64/54, Kanal 60
 *                    -> 168/117; Programm 2 Ton 0/1 nach Controller 7 (SpuVmSetVol @0x8005926c) -> 2372/2372 und 4723/0
 *   Lage der Schritte: ROOM1140, Cut 0, Spieler -5118/0/-15817 -> FUN_80045a64 = 93/103 (Vormerk-Saetze 2 und 3)
 *
 * GEGENPROBE: auf r42/story 71caea668 gibt es weder re15_svm_vol.h noch svm_vol_common.c — der Riegel baut dort nicht.
 * Die Pegel-Gegenprobe am echten Mischer steht in test_r42_ton_mischer.c (rot auf 71caea668 mit dem alten audio_pc.c).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_svm_vol.h"
#include "re15_vab.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_wassersprung_r41.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *lesen(const char *rel, int *n)
{
    char p[600];
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(p, "rb");
    if (!f) { printf("FAIL: %s nicht lesbar\n", p); fails++; return NULL; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (int)sz;
    return b;
}
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* Register eines Bank-Tons als Geraeusch (SsUtKeyOnV): voll/volr < 0 = Zweig ohne Lage (Tastenstaerke = Ton-vol). */
static void se_reg(const re15_vab_t *vab, int prog, int tone, int voll, int volr, int re2, int *l, int *r)
{
    const re15_vab_tone_t *t = &vab->tones[prog * RE15_VAB_TONES_PER_PROGRAM + tone];
    re15_svm_vol_t in; memset(&in, 0, sizeof in);
    in.vab_mvol = vab->master_volume; in.prog_mvol = vab->prog_mvol[prog]; in.prog_pan = vab->prog_mpan[prog];
    in.tone_vol = t->vol; in.tone_pan = t->pan;
    if (voll < 0) { in.key = t->vol; in.key_pan = RE15_SVM_PAN_MITTE; }
    else re15_svm_utkeyon_split(voll, volr, &in.key, &in.key_pan);
    if (re2) re15_svm_keyon_reg_re2(&in, l, r); else re15_svm_keyon_reg(&in, l, r);
}

/* Register einer Sequenz-Note; ton_pan < 0 = der Ton selbst (Key-On), sonst der uebergebene (SpuVmSetVol). */
static void seq_reg(const re15_vab_t *vab, int prog, int tone, int anschlag, int kanal, int kanal_pan, int ton_pan, int *l, int *r)
{
    const re15_vab_tone_t *t = &vab->tones[prog * RE15_VAB_TONES_PER_PROGRAM + tone];
    re15_svm_vol_t in; memset(&in, 0, sizeof in);
    in.key = re15_svm_seq_key(anschlag, kanal); in.key_pan = kanal_pan;
    in.vab_mvol = vab->master_volume; in.prog_mvol = vab->prog_mvol[prog]; in.prog_pan = vab->prog_mpan[prog];
    in.tone_vol = t->vol; in.tone_pan = ton_pan < 0 ? t->pan : ton_pan;
    in.ist_seq = 1; in.seq_l = in.seq_r = 127;        /* Slot-Halbworte 0x800b52b0/b8/c0 = 127 (gemessen) */
    re15_svm_keyon_reg(&in, l, r);
}

static re15_vab_t s_vab;      /* gross (2048 Tones) -> nicht auf den Stapel */

int main(void)
{
    int l, r, n = 0;

    /* ---- (1) reine Formel: Vollausschlag, Mono, Rundung ---------------------------------------------------- */
    {   re15_svm_vol_t in; memset(&in, 0, sizeof in);
        in.key = in.vab_mvol = in.prog_mvol = in.tone_vol = 127; in.key_pan = in.prog_pan = in.tone_pan = 0x40;
        re15_svm_keyon_reg(&in, &l, &r);
        CHECK(l == 0x3fff && r == 0x3fff, "alles 127, Mitte -> %d/%d (0x3fff/0x3fff)", l, r);
        in.key = in.tone_vol = 110;                                  /* Klick: Ton-vol 110 zweimal */
        re15_svm_keyon_reg(&in, &l, &r);
        CHECK(l == 9219 && r == 9219, "Klick Ton-vol 110 (Vormerk-Satz 110/110) -> %d/%d (gemessen 9219/9219)", l, r);
        in.key = in.tone_vol = 127; in.tone_pan = 0;                 /* Ton hart links: rechts 0 (@0x80055e7c) */
        re15_svm_keyon_reg(&in, &l, &r);
        CHECK(l == 0x3fff && r == 0, "Ton-Pan 0 -> %d/%d (16383/0)", l, r);
        in.mono = 1;                                                 /* Mono-Schalter: beide = der groessere (@0x80055fd0-e4) */
        re15_svm_keyon_reg(&in, &l, &r);
        CHECK(l == 0x3fff && r == 0x3fff, "Ton-Pan 0 mit Mono-Schalter -> %d/%d (16383/16383)", l, r);
        in.mono = 0; in.tone_pan = 0x7f;                             /* hart rechts: links * 0 / 63 (@0x80055ea8) */
        re15_svm_keyon_reg(&in, &l, &r);
        CHECK(l == 0 && r == 0x3fff, "Ton-Pan 127 -> %d/%d (0/16383)", l, r);
        /* RE2: Geraeusch ohne Quadrat (@0x80083a38), Sequenz-Stimme mit Quadrat (@0x80083a40/50) */
        in.tone_pan = 0x40; in.key = in.tone_vol = 100;
        re15_svm_keyon_reg_re2(&in, &l, &r);
        const int x = (((100 * (127 * 0x3fff)) / 0x3f01) * 127 * 100) / 0x3f01;
        CHECK(l == x && r == x && x == 10157, "RE2-Geraeusch Ton-vol 100 -> %d/%d (linear %d = Tuersequenz-Probe 10157)", l, r, x);
        in.ist_seq = 1; in.seq_l = in.seq_r = 127;
        re15_svm_keyon_reg_re2(&in, &l, &r);
        CHECK(l == (x * x) / 0x3fff && r == l, "RE2-Sequenz-Stimme -> %d/%d (Quadrat %d)", l, r, (x * x) / 0x3fff);
        int k, p;
        re15_svm_utkeyon_split(106, 95, &k, &p);
        CHECK(k == 106 && p == 57, "SsUtKeyOnV 106/95 -> Staerke %d Pan %d (106, (95<<6)/106 = 57)", k, p);
        re15_svm_utkeyon_split(99, 62, &k, &p);
        CHECK(k == 99 && p == 40, "SsUtKeyOnV 99/62 -> Staerke %d Pan %d (99, 40)", k, p);
        re15_svm_utkeyon_split(80, 90, &k, &p);
        CHECK(k == 90 && p == 0x7f - (80 << 6) / 90, "SsUtKeyOnV 80/90 -> Staerke %d Pan %d (90, 71)", k, p);
        CHECK(re15_svm_seq_key(115, 90) == 81 && re15_svm_seq_key(127, 127) == 127, "SpuVmKeyOn 115 * 90 / 127 = %d (81)", re15_svm_seq_key(115, 90));
        CHECK(RE15_SPU_MVOL_Q15 == 0x7ffe && RE15_SPU_REG_Q15(0x3fff) == 0x7ffe, "Hauptlautstaerke 0x3fff -> Q15 0x%x (0x7ffe)", RE15_SPU_MVOL_Q15);
    }

    /* ---- (2) ROOM1130 snd0: Rolltor ------------------------------------------------------------------------- */
    uint8_t *rdt = lesen("STAGE1/ROOM1130.RDT", &n);
    if (rdt && re15_vab_parse(rdt + rd32(rdt + 0x0c), (size_t)(rd32(rdt + 0x10) - rd32(rdt + 0x0c)), &s_vab) == 0) {
        const uint8_t *edt = rdt + rd32(rdt + 0x08);
        CHECK(edt[10 * 4 + 2] >> 4 == 7 && edt[11 * 4 + 2] >> 4 == 8, "ROOM1130 snd0 Satz 10/11 -> Ton %d/%d (7/8)", edt[10 * 4 + 2] >> 4, edt[11 * 4 + 2] >> 4);
        se_reg(&s_vab, 0, 7, -1, -1, 0, &l, &r);
        CHECK(s_vab.tones[7].vol == 127 && l == 16383 && r == 16383, "Rolltor-Rumpeln (Ton-vol %d) -> %d/%d (gemessen 16383/16383)", s_vab.tones[7].vol, l, r);
        se_reg(&s_vab, 0, 8, -1, -1, 0, &l, &r);
        CHECK(s_vab.tones[8].vol == 113 && l == 10268 && r == 10268, "Rolltor-Stopp (Ton-vol %d) -> %d/%d (10268/10268; Vormerk-Satz gemessen 113/113)", s_vab.tones[8].vol, l, r);
        /* Schritt Satz 29 (Ton 1, vol 80) mit der gemessenen Lage 93/103 (ROOM1130 Cut 9) */
        se_reg(&s_vab, 0, 1, 93, 103, 0, &l, &r);
        CHECK(s_vab.tones[1].vol == 80 && l == 3498 && r == 4275, "Schritt ROOM1130 (Ton-vol %d, Lage 93/103) -> %d/%d (3498/4275)", s_vab.tones[1].vol, l, r);
    } else { printf("FAIL: ROOM1130 snd0 nicht lesbar\n"); fails++; }
    free(rdt);

    /* ---- (3) ARMS03: Pistolenschuss mit der gemessenen Lage -------------------------------------------------- */
    uint8_t *edh = lesen("SOUND/ARMS03.EDH", &n);
    if (edh && re15_vab_parse(edh + rd32(edh + n - 8), (size_t)(n - 8) - rd32(edh + n - 8), &s_vab) == 0) {
        CHECK(edh[2] >> 4 == 1 && edh[3] >> 5 == 1, "ARMS03 Satz 0: Ton %d, %d Zusatzschicht (1, 1)", edh[2] >> 4, edh[3] >> 5);
        for (int k = 1; k <= 2; k++) {
            se_reg(&s_vab, 0, k, 106, 95, 0, &l, &r);
            CHECK(s_vab.tones[k].vol == 90 && l == 5731 && r == 4691, "Schuss Schicht Ton %d (vol %d, Lage 106/95) -> %d/%d (gemessen 5731/4691)", k, s_vab.tones[k].vol, l, r);
        }
        CHECK((5731 + 4691) / 2 == 5211, "Summe (PORT-WAHL bis Stufe 6): Mittelwert 5211");
    } else { printf("FAIL: ARMS03 nicht lesbar\n"); fails++; }
    free(edh);

    /* ---- (4) ROOM1140: Zombie-Laut (snd1) + Lage des Schusses aus FUN_80045a64 -------------------------------- */
    rdt = lesen("STAGE1/ROOM1140.RDT", &n);
    if (rdt && re15_vab_parse(rdt + rd32(rdt + 0x18), (size_t)(rd32(rdt + 0x1c) - rd32(rdt + 0x18)), &s_vab) == 0) {
        se_reg(&s_vab, 0, 4, 99, 62, 0, &l, &r);
        CHECK(s_vab.tones[4].vol == 127 && l == 9955 && r == 4012, "Zombie-Laut Ton 4 (vol %d, Lage 99/62) -> %d/%d (gemessen 9955/4012)", s_vab.tones[4].vol, l, r);
        se_reg(&s_vab, 0, 4, 102, 65, 0, &l, &r);
        CHECK(l == 10567 && r == 4259, "Zombie-Laut Lage 102/65 -> %d/%d (gemessen 10567/4259)", l, r);
        if (re15_rdt_parse(rdt, (size_t)n, &g_room_rdt) == 0) {
            g_room_rdt_ok = 1;
            g_scd.work_vars[0x0A] = 0;                               /* Cut 0 (DAT_800b0fe4 im Savestate) */
            memset(&g_actors[0], 0, sizeof g_actors[0]);
            /* Schritt-Lage: Spieler -5118/0/-15817 (Savestate pre.sav des Schuss-Laufs) -> Vormerk-Saetze 2 und 3 = 93/103 */
            g_actors[0].x = -5118; g_actors[0].y = 0; g_actors[0].z = -15817;
            int lage[2] = { -1, -1 };
            int da = re15_svm_lage_spieler(lage);
            CHECK(da == 1 && lage[0] == 93 && lage[1] == 103, "Lage ROOM1140 Cut 0 Spieler -5118/0/-15817 -> %d/%d (gemessen: Schritt-Saetze 93/103)", lage[0], lage[1]);
            /* Beim Schuss steht der Spieler auf -5541/0/-16024: seine Lage ist 92/102. Der Schuss selbst traegt im Original
             * 106/95, weil FUN_80017654 die Lage aus dem Angriffs-Arbeitssatz nimmt (`lw v1,0x800b52c4`, `lh v0,40(v1)` /
             * 42 / 44 @0x80017658-0x8001767c), nicht &DAT_800aca88 -> Stufe 6 (Sender durchreichen). */
            g_actors[0].x = -5541; g_actors[0].y = 0; g_actors[0].z = -16024;
            da = re15_svm_lage_spieler(lage);
            CHECK(da == 1 && lage[0] == 92 && lage[1] == 102, "Lage ROOM1140 Cut 0 Spieler -5541/0/-16024 -> %d/%d (92/102)", lage[0], lage[1]);
            g_room_rdt_ok = 0;
            CHECK(re15_svm_lage_spieler(lage) == 0, "ohne Raum keine Lage");
        } else { printf("FAIL: ROOM1140 nicht parsebar\n"); fails++; }
    } else { printf("FAIL: ROOM1140 snd1 nicht lesbar\n"); fails++; }
    /* rdt bleibt geladen: g_room_rdt zeigt hinein */

    /* ---- (5) ROOM1170 snd0: Schritt auf dem Hubschrauberplatz ------------------------------------------------- */
    {   uint8_t *r17 = lesen("STAGE1/ROOM1170.RDT", &n);
        if (r17 && re15_vab_parse(r17 + rd32(r17 + 0x0c), (size_t)(rd32(r17 + 0x10) - rd32(r17 + 0x0c)), &s_vab) == 0) {
            se_reg(&s_vab, 0, 1, 80, 90, 0, &l, &r);
            CHECK(s_vab.tones[1].vol == 60 && l == 1450 && r == 1836, "Schritt ROOM1170 Ton 1 (vol %d, Lage 80/90) -> %d/%d (gemessen 1450/1836)", s_vab.tones[1].vol, l, r);
        } else { printf("FAIL: ROOM1170 snd0 nicht lesbar\n"); fails++; }
        free(r17); }

    /* ---- (6) Musik: SUB_15 (Rotor) und MAIN17 ------------------------------------------------------------------ */
    {   uint8_t *b = lesen("SOUND/SUB_15.BGM", &n);
        if (b && re15_vab_parse(b + rd32(b + n - 12), (size_t)(rd32(b + n - 16) - rd32(b + n - 12)), &s_vab) == 0) {
            seq_reg(&s_vab, 0, 0, 127, 127, 0x40, -1, &l, &r);
            CHECK(s_vab.master_volume == 127 && s_vab.prog_mvol[0] == 80 && s_vab.tones[0].vol == 127 && l == 6500 && r == 6500,
                  "Rotor SUB_15 (Bank %d, Programm %d, Ton %d) -> %d/%d (gemessen 6500/6500)", s_vab.master_volume, s_vab.prog_mvol[0], s_vab.tones[0].vol, l, r);
            seq_reg(&s_vab, 0, 1, 127, 127, 0x40, -1, &l, &r);
            CHECK(l == 6500 && r == 6500, "Rotor SUB_15 Ton 1 -> %d/%d (gemessen 6500/6500)", l, r);
        } else { printf("FAIL: SUB_15 nicht lesbar\n"); fails++; }
        free(b);
        b = lesen("SOUND/MAIN17.BGM", &n);
        if (b && re15_vab_parse(b + rd32(b + n - 8), (size_t)(rd32(b + n - 12) - rd32(b + n - 8)), &s_vab) == 0) {
            CHECK(s_vab.master_volume == 100 && s_vab.prog_mvol[0] == 110 && s_vab.tones[0].vol == 50, "MAIN17 Bank %d Programm 0 %d Ton 0 %d (100, 110, 50)",
                  s_vab.master_volume, s_vab.prog_mvol[0], s_vab.tones[0].vol);
            static const int soll[3][2] = { { 115, 480 }, { 90, 290 }, { 125, 566 } };
            for (int i = 0; i < 3; i++) {
                seq_reg(&s_vab, 0, 0, soll[i][0], 90, 0x40, -1, &l, &r);
                CHECK(l == soll[i][1] && r == soll[i][1], "MAIN17 Programm 0 Anschlag %d Kanal 90 -> %d/%d (gemessen %d)", soll[i][0], l, r, soll[i][1]);
            }
            seq_reg(&s_vab, 3, 1, 127, 127, 0x40, -1, &l, &r);
            CHECK(l == 3023 && r == 3023, "MAIN17 Programm 3 Ton 1 -> %d/%d (gemessen 3023/3023)", l, r);
            seq_reg(&s_vab, 5, 0, 64, 60, 0x40, -1, &l, &r);
            CHECK(l == 168 && r == 168, "MAIN17 Programm 5 Anschlag 64 Kanal 60 -> %d/%d (gemessen 168/168)", l, r);
            seq_reg(&s_vab, 5, 0, 54, 60, 0x40, -1, &l, &r);
            CHECK(l == 117 && r == 117, "MAIN17 Programm 5 Anschlag 54 Kanal 60 -> %d/%d (gemessen 117/117)", l, r);
            /* Programm 2: das Stereo-Paar Ton 0 (Pan 0) / Ton 1 (Pan 127). Key-On: je eine Seite. Nach Controller 7
             * (SpuVmSetVol) liest libsnd den Pan aus Programm 0 (Ton 0 Pan 64, Ton 1 Pan 0) -> gemessen 2372/2372 und 4723/0. */
            const re15_vab_tone_t *p2 = &s_vab.tones[2 * RE15_VAB_TONES_PER_PROGRAM];
            CHECK(p2[0].pan == 0 && p2[1].pan == 127 && s_vab.tones[0].pan == 64 && s_vab.tones[1].pan == 0,
                  "MAIN17 Pan Programm 2 Ton 0/1 = %d/%d (0/127), Programm 0 Ton 0/1 = %d/%d (64/0)", p2[0].pan, p2[1].pan, s_vab.tones[0].pan, s_vab.tones[1].pan);
            seq_reg(&s_vab, 2, 0, 127, 100, 0x40, -1, &l, &r);
            CHECK(l == 2372 && r == 0, "MAIN17 Programm 2 Ton 0 beim Key-On -> %d/%d (2372/0)", l, r);
            seq_reg(&s_vab, 2, 0, 127, 100, 0x40, s_vab.tones[0].pan, &l, &r);
            CHECK(l == 2372 && r == 2372, "MAIN17 Programm 2 Ton 0 nach Controller 7 -> %d/%d (gemessen 2372/2372)", l, r);
            seq_reg(&s_vab, 2, 1, 127, 100, 0x40, -1, &l, &r);
            CHECK(l == 0 && r == 4723, "MAIN17 Programm 2 Ton 1 beim Key-On -> %d/%d (0/4723)", l, r);
            seq_reg(&s_vab, 2, 1, 127, 100, 0x40, s_vab.tones[1].pan, &l, &r);
            CHECK(l == 4723 && r == 0, "MAIN17 Programm 2 Ton 1 nach Controller 7 -> %d/%d (gemessen 4723/0)", l, r);
        } else { printf("FAIL: MAIN17 nicht lesbar\n"); fails++; }
        free(b); }

    if (fails == 0) { printf("test_r42_ton_gesetz: OK (%d Pruefungen)\n", checks); return 0; }
    printf("test_r42_ton_gesetz: %d von %d FEHLGESCHLAGEN\n", fails, checks);
    return 1;
}
