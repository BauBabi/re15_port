/*
 * probe_r34_re2fx_raum — Runde 34 Spur D, Nachbesserung N2 (Gegenpruefung M1/M2): die RE1.5-Raumabbildung
 * von RE2 FUN_8004fba0 (re2fx_boden in engine/src/re2_fx.c) mit ECHT GELADENEN RAEUMEN, ohne Boden-Haken.
 *
 * Abbildung (Dossier bau_d.md §N1): Zelle Band b = Form mit oben -1800*(b+1) (FUN_8001c6e8 @0x8001c868-88c)
 * und unten -1800*b (Spieler-y aus +0x82 @0x8001d7b8-cc), Zellfilter der RE1.5-ESP-Routine 12 (Maske 0x100
 * @0x800177d0), Rechteck um r erweitert; RE2-Vergleich je Form: P.y > unten nichts (@0x8004ffdc-e0), innen
 * Kontakt |= 1 (@0x8004ffe4/@0x80050000), P.y <= oben Rueckgabe = min(oben) (@0x80050028-44), P.y == oben oder
 * innen und P.y < unten Kontakt |= 2 (@0x8005005c-8c); Objekte: P.y <= oben -> oben, innen -> Kontakt |= 1 und
 * P.y - 1 (@0x8004fcbc-fd54), a3 != 0 -> keine Objekte (@0x8004fc5c).
 *
 * Rueckgabe 0 = gruen, sonst die Nummer der ersten verletzten Pruefung.
 *  1xx Vorbedingung: Punkte je Raum per FUN_8001c6e8-Zwilling (re15_collision_room_coll, Routine-12-Argumente
 *      r 0 / Band 8 / Maske 0x100) klassifiziert: Wand = -1800 im Umkreis 1000, frei = 0 im Umkreis 2500,
 *      Band-2-Zelle (ROOM1170) = -5400 im Umkreis 1000.
 *  2xx re2fx_boden_sonde (Rueckgabe, Kontakt) je Punkt fuer P.y ueber/auf/in/unter der Zelle.
 *  3xx Flammen an der WAND (ROOM1140/1150/1170): +0x14 nach Op 27 = 0 (nicht 0xFFFFF8F8), erster Applier-Ruf
 *      erst in X+19 (+0x16 = 16), jede Flamme geht im ersten Gleitbild nach Op 50 (step[2] 64) und steht dann.
 *  4xx Flammen FREI (dieselben Raeume) und unter der Band-2-Zelle (44x): +0x14 = 0, erster Applier-Ruf X+19,
 *      Flammen gleiten (Weg > 1000 in 12 Bildern), kein Op 50.
 *  5xx Objekt (Prop am Freipunkt): Sonde ueber/auf/in/unter dem Kasten, a3 = 1 ohne Objekt, kein Band-Tor,
 *      Rand r; eine Flamme im Prop-Kasten geht nach Op 50.
 *  6xx Negativ-Kontrollen: derselbe Wandpunkt OHNE Raum (g_room_rdt_ok = 0) und mit Boden-Haken gleitet wie
 *      frei; Gegenprobe mit Raum haelt.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re2_fx.h"
#include "re15_ai_flavor.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_scd.h"
#include "re15_collision.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

static int fail(int n, const char *what) { printf("FAIL %d: %s\n", n, what); return n; }

static uint16_t u16(const uint8_t *b, int o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); }
static int16_t  s16(const uint8_t *b, int o) { return (int16_t)u16(b, o); }
static uint32_t u32(const uint8_t *b, int o)
{ return (uint32_t)b[o] | ((uint32_t)b[o + 1] << 8) | ((uint32_t)b[o + 2] << 16) | ((uint32_t)b[o + 3] << 24); }

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (sz > 0) ? (uint8_t *)malloc((size_t)sz) : NULL;
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = (size_t)sz;
    return b;
}

static uint8_t *s_raum_buf;
static int raum_laden(const char *name)
{
    char p[1024]; size_t n = 0;
    snprintf(p, sizeof p, "%s/STAGE1/%s", RE15_XSTR(RE15_ASSETS_PATH), name);
    g_room_rdt_ok = 0;
    free(s_raum_buf);
    s_raum_buf = slurp(p, &n);
    if (!s_raum_buf || re15_rdt_parse(s_raum_buf, n, &g_room_rdt) != 0) { printf("kann %s nicht laden\n", p); return -1; }
    g_room_rdt_ok = 1;
    memset(&g_scd.props, 0, sizeof g_scd.props);
    g_scd.prop_count = 0;
    return 0;
}

/* ---- Spione ------------------------------------------------------------------------------ */
static int s_tick;              /* Bildnummer: 0 = Aufschlagbild X */
static int s_app_n, s_app_erst;
static int app_spion(const int32_t p[3], int16_t gier, const int16_t box[4], uint32_t hitcode)
{
    (void)p; (void)gier; (void)box; (void)hitcode;
    if (s_app_n++ == 0) s_app_erst = s_tick;
    return 0;
}

/* Umkreis-Pruefung mit dem FUN_8001c6e8-Zwilling (Routine-12-Argumente). */
static int umkreis(int32_t x, int32_t z, int R, int16_t soll)
{
    for (int dx = -R; dx <= R; dx += 250)
        for (int dz = -R; dz <= R; dz += 250) {
            if (dx * dx + dz * dz > R * R) continue;
            if (re15_collision_room_coll(&g_room_rdt, x + dx, z + dz, 0, 8, 0x100u) != soll) return 0;
        }
    return 1;
}

typedef struct {
    uint32_t b14[3];            /* +0x14..+0x17 der Flammen 92/91/90 nach Op 27 (Bild X+1) */
    int      op50[3];           /* Bild, in dem die Flamme Op 50 durchlief (step[2] == 64), -1 = nie */
    int      gelandet[3];       /* Bild der Landung (Op A 19) */
    int32_t  weg[3];            /* |Weltlage(Landung + 12) - Weltlage(Landung)| (x/z Manhattan) */
    int32_t  nach_halt[3];      /* |Weltlage(Op50 + 10) - Weltlage(Op50)|, -1 ohne Op 50 */
    int      app_erst, app_n;   /* erstes Bild eines Applier-Rufs (X = 0), Zahl der Rufe in 60 Bildern */
} lauf_t;

static void lauf(const int32_t q[3], int16_t gier, lauf_t *L)
{
    re2fx_reset(); re15_re2z_rng_reset();
    re2fx_wasser_hook = NULL; re2fx_se_hook = NULL; re2fx_applier = app_spion;
    s_app_n = 0; s_app_erst = -1;
    memset(L, 0, sizeof *L);
    int32_t lage0[3][2] = {{0}}, halt0[3][2] = {{0}};
    for (int k = 0; k < 3; k++) { L->op50[k] = -1; L->gelandet[k] = -1; L->weg[k] = -1; L->nach_halt[k] = -1; }
    re2fx_aufschlag(1, q, gier);
    for (s_tick = 0; s_tick < 60; s_tick++) {          /* s_tick 0 = Aufschlagbild X */
        re2fx_tick();
        for (int k = 0; k < 3; k++) {
            const uint8_t *f = re2fx_platz(92 - k);
            int32_t x = s16(f, 0x34), z = s16(f, 0x38);
            if (s_tick == 1) L->b14[k] = u32(f, 0x14);
            if (L->gelandet[k] < 0 && f[0] == 19) { L->gelandet[k] = s_tick; lage0[k][0] = x; lage0[k][1] = z; }
            if (L->op50[k] < 0 && L->gelandet[k] >= 0 && f[0x02] == 64) { L->op50[k] = s_tick; halt0[k][0] = x; halt0[k][1] = z; }
            if (L->gelandet[k] >= 0 && s_tick == L->gelandet[k] + 12) {
                int32_t dx = x - lage0[k][0], dz = z - lage0[k][1];
                L->weg[k] = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            }
            if (L->op50[k] >= 0 && s_tick == L->op50[k] + 10) {
                int32_t dx = x - halt0[k][0], dz = z - halt0[k][1];
                L->nach_halt[k] = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            }
        }
    }
    L->app_erst = s_app_erst; L->app_n = s_app_n;
}

static void zeige(const char *was, const lauf_t *L)
{
    printf("  %-13s +0x14 %08X/%08X/%08X Landung %d/%d/%d Op50 %d/%d/%d Weg %d/%d/%d nach Halt %d/%d/%d Applier ab X+%d (%d Rufe)\n",
           was, L->b14[0], L->b14[1], L->b14[2], L->gelandet[0], L->gelandet[1], L->gelandet[2],
           L->op50[0], L->op50[1], L->op50[2], L->weg[0], L->weg[1], L->weg[2],
           L->nach_halt[0], L->nach_halt[1], L->nach_halt[2], L->app_erst, L->app_n);
}

/* Zeitplan (gemessen, Dossier §N2): Flamme landet in X+2 (Op 28 -> Op 46), Op A 19 zaehlt ab X+3 +0x16 hoch
 * (@0x8001f31c-28), das Tor `sltiu v0,v0,0x10` @0x8001f2e8 oeffnet mit +0x16 = 16 in X+19.
 * Ein Bild Gleiten vor dem Halt: Op 46 setzt vel.x 180 (@0x80020ba4) vor der Physik desselben Bilds
 * (@0x8001d70c-798), Op 29 im naechsten Bild findet den Kontakt (@0x8001fe84) -> Op 50 (vel.x := 0 @0x8002199c). */
#define LANDUNG      2
#define APP_ERST    19

/* Flammen an einer Stelle, die fuer die Flamme wie freier Boden wirkt (kein Kontakt beim Gleiten). */
static int pruef_gleitet(int basis, const lauf_t *L)
{
    for (int k = 0; k < 3; k++) {
        if (L->b14[k] != 0u)            return fail(basis + 1, "+0x14 nach Op 27 != 0 (RE2: P.y > unten jeder Zelle)");
        if (L->gelandet[k] != LANDUNG)  return fail(basis + 2, "Landung nicht in X+2");
        if (L->op50[k] != -1)           return fail(basis + 3, "Op 50 ohne Kontakt");
        if (L->weg[k] < 1000)           return fail(basis + 4, "Flamme gleitet nicht (Weg < 1000 in 12 Bildern)");
    }
    if (L->app_erst != APP_ERST) return fail(basis + 5, "erster Applier-Ruf nicht in X+19 (+0x16 = 16)");
    return 0;
}
/* Flammen in einer Zelle/einem Objekt: +0x14 = 0, Op 50 im ersten Gleitbild, danach Stillstand. */
static int pruef_haelt(int basis, const lauf_t *L)
{
    for (int k = 0; k < 3; k++) {
        if (L->b14[k] != 0u)                return fail(basis + 1, "+0x14 nach Op 27 != 0 (vorher 0xFFFFF8F8 -> +0x16 = 0xFFFF)");
        if (L->gelandet[k] != LANDUNG)      return fail(basis + 2, "Landung nicht in X+2");
        if (L->op50[k] != LANDUNG + 1)      return fail(basis + 3, "Op 50 nicht im ersten Gleitbild (Kontakt Op 29 @0x8001fe84)");
        if (L->nach_halt[k] != 0)           return fail(basis + 4, "Flamme bewegt sich nach Op 50 weiter");
        if (L->weg[k] > 260)                return fail(basis + 5, "mehr als ein Bild Gleiten vor dem Halt");
    }
    if (L->app_erst != APP_ERST) return fail(basis + 6, "erster Applier-Ruf nicht in X+19 (Schadenstor zu frueh offen)");
    return 0;
}

typedef struct { const char *raum; int32_t wx, wz, fx, fz; } raum_t;
static const raum_t k_raeume[3] = {
    { "ROOM1140.RDT",  -2750,  -5750,  -3000, -20500 },   /* Wand = Zelle 3 x -5750..16700 z -7800..1750 (Band 0) */
    { "ROOM1150.RDT", -20000, -19000, -23750, -13750 },   /* Wand = Zelle 0 x -21782..-17858 z -20864..-15914 */
    { "ROOM1170.RDT", -19250, -18250, -28750, -14250 },   /* Wand = Zellen 16/17 (Ecke x -20000..-17876 z -18272..-17120) */
};
/* ROOM1170 Band-2-Zellen 23/25 (floor 0x23): x -28900..-22300 z -28100..-26800 / x -28700..-26900 z -28000..-23000 */
static const int32_t k_b2x = -27750, k_b2z = -27000;

static int sonde(int32_t x, int32_t y, int32_t z, int a3, int *k)
{
    const int32_t p[3] = { x, y, z };
    return (int)re2fx_boden_sonde(p, 2, 0x2000u, a3, k);
}
/* Erwartung je P.y: {P.y, Rueckgabe, Kontakt}. */
typedef struct { int32_t y; int f, k; } soll_t;
static int pruef_sonde(int nr, int32_t x, int32_t z, int a3, const soll_t *s, int n)
{
    for (int i = 0; i < n; i++) {
        int k = -1, f = sonde(x, s[i].y, z, a3, &k);
        if (f != s[i].f || k != s[i].k) {
            printf("  (%d,%d,%d) a3 %d: Rueckgabe %d Kontakt %d, erwartet %d / %d\n", x, s[i].y, z, a3, f, k, s[i].f, s[i].k);
            return fail(nr, "Bodensonde != RE2-Regel");
        }
    }
    return 0;
}

static int32_t boden_flach(const int32_t p[3], int r, uint32_t mask, int a3, int *kontakt)
{ (void)r; (void)mask; (void)a3; *kontakt = (p[1] > 0); return 0; }

int main(void)
{
    char p[1024]; size_t n = 0;
    snprintf(p, sizeof p, "%s/../RE2/CORE00.ESP", RE15_XSTR(RE15_ASSETS_PATH));
    uint8_t *esp = slurp(p, &n);
    if (!esp || re2fx_register_core(esp, n) != 0) return fail(1, "CORE00.ESP");
    re2fx_boden_hook = NULL;
    int rc;

    /* Band-0-Zelle: oben -1800, unten 0. RE2-Regel (Kopf). */
    static const soll_t k_wand[7] = {
        { -2500, -1800, 0 },    /* ueber der Oberkante: Rueckgabe oben, kein Kontakt (@0x80050028-54) */
        { -1800, -1800, 2 },    /* auf der Oberkante: oben, Kontakt 2 (@0x8005005c-8c) */
        { -1799,     0, 3 },    /* innen: Kontakt 1|2, Rueckgabe bleibt (@0x8004ffe4-5000c) */
        {  -900,     0, 3 },
        {     0,     0, 1 },    /* P.y == unten: innen, nicht ueber unten */
        {    10,     0, 1 },    /* unter der Zelle: nur Grundebene (@0x8004fc48-58) */
        {  2000,     0, 1 } };
    static const soll_t k_frei[4] = { { -2500, 0, 0 }, { -900, 0, 0 }, { 0, 0, 0 }, { 10, 0, 1 } };

    for (int ri = 0; ri < 3; ri++) {
        const raum_t *R = &k_raeume[ri];
        if (raum_laden(R->raum) != 0) return fail(2, "Raum laden");
        printf("%s: Wand (%d,%d) frei (%d,%d)\n", R->raum, R->wx, R->wz, R->fx, R->fz);
        /* 1xx Vorbedingungen */
        if (!umkreis(R->wx, R->wz, 1000, -1800)) return fail(101 + ri, "Wandpunkt nicht im Umkreis 1000 Band-0-Zelle");
        if (!umkreis(R->fx, R->fz, 2500, 0))     return fail(104 + ri, "Freipunkt nicht im Umkreis 2500 frei");
        /* 2xx Bodensonde */
        if ((rc = pruef_sonde(201 + ri, R->wx, R->wz, 0, k_wand, 7))) return rc;
        if ((rc = pruef_sonde(204 + ri, R->fx, R->fz, 0, k_frei, 4))) return rc;
        /* 3xx / 4xx Flammen */
        lauf_t L;
        const int32_t qw[3] = { R->wx, 10, R->wz }, qf[3] = { R->fx, 10, R->fz };
        lauf(qw, 0, &L); zeige("Wand", &L);
        if ((rc = pruef_haelt(300 + 10 * ri, &L))) return rc;
        lauf(qf, 0, &L); zeige("frei", &L);
        if ((rc = pruef_gleitet(400 + 10 * ri, &L))) return rc;
    }

    /* Zellrand: RE2 ERWEITERT das Formrechteck um r = 2 (`sll a0,s4,1 / addu v0,v0,a0 / sltu` @0x8004fd84-98);
     * ROOM1150 Zelle 0 beginnt bei x = -21782: x - 2 = -21784 zaehlt, x - 3 = -21785 nicht. Vorbedingung:
     * ohne Rand (FUN_8001c6e8 r 0) liegt keiner der beiden Punkte in einer Zelle. */
    if (raum_laden("ROOM1150.RDT") != 0) return fail(2, "Raum laden");
    if (re15_collision_room_coll(&g_room_rdt, -21784, -19000, 0, 8, 0x100u) != 0 ||
        re15_collision_room_coll(&g_room_rdt, -21782, -19000, 0, 8, 0x100u) != -1800)
        return fail(108, "Randpunkte ROOM1150 Zelle 0 nicht wie erwartet");
    {
        static const soll_t k_rand_in[2]  = { { -900, 0, 3 }, { -2500, -1800, 0 } };
        static const soll_t k_rand_aus[2] = { { -900, 0, 0 }, { -2500,     0, 0 } };
        if ((rc = pruef_sonde(208, -21784, -19000, 0, k_rand_in, 2)))  return rc;
        if ((rc = pruef_sonde(209, -21785, -19000, 0, k_rand_aus, 2))) return rc;
    }

    /* ROOM1170: Band-2-Zelle UEBER der Flamme (oben -5400, unten -3600) - wirkt wie freier Boden. */
    if (raum_laden("ROOM1170.RDT") != 0) return fail(2, "Raum laden");
    if (!umkreis(k_b2x, k_b2z, 1000, -5400)) return fail(107, "Band-2-Punkt nicht im Umkreis 1000 -5400");
    {
        static const soll_t k_b2[6] = { { -6000, -5400, 0 }, { -5400, -5400, 2 }, { -4000, 0, 3 },
                                        { -3600, 0, 1 }, { -900, 0, 0 }, { 10, 0, 1 } };
        if ((rc = pruef_sonde(207, k_b2x, k_b2z, 0, k_b2, 6))) return rc;
        lauf_t L; const int32_t q[3] = { k_b2x, 10, k_b2z };
        lauf(q, 0, &L); zeige("Band-2-Zelle", &L);
        if ((rc = pruef_gleitet(440, &L))) return rc;
    }

    /* 5xx Objekt: Prop (Obj_model_set) am Freipunkt von ROOM1140, Kasten 2000 x 600 (oben -600, unten 0). */
    if (raum_laden("ROOM1140.RDT") != 0) return fail(2, "Raum laden");
    {
        const raum_t *R = &k_raeume[0];
        g_scd.prop_count = 1;
        g_scd.props[0].active = 1; g_scd.props[0].band = 0;
        g_scd.props[0].x = R->fx; g_scd.props[0].y = 0; g_scd.props[0].z = R->fz;
        g_scd.props[0].box_cx = 0; g_scd.props[0].box_cy = -300; g_scd.props[0].box_cz = 0;
        g_scd.props[0].box_hx = 1000; g_scd.props[0].box_hy = 300; g_scd.props[0].box_hz = 1000;
        static const soll_t k_prop[6] = {
            { -900, -600, 0 },      /* ueber dem Objekt: Kandidat oben (@0x8004fccc-dc), kein Kontakt */
            { -600, -600, 0 },      /* auf der Oberkante: oben, kein Kontakt (Objekte kennen kein Bit 2) */
            { -300, -301, 1 },      /* im Objekt: Kontakt 1, Kandidat P.y - 1 (@0x8004fd0c/@0x8004fd34) */
            {    0,   -1, 1 },      /* P.y == unten: im Objekt */
            {   10,    0, 1 },      /* unter dem Objekt: nichts (@0x8004fce0-e4), Grundebene */
            { -2500, -600, 0 } };
        if ((rc = pruef_sonde(501, R->fx, R->fz, 0, k_prop, 6))) return rc;
        /* a3 != 0 -> Objekt-Schleife aus (@0x8004fc5c) */
        static const soll_t k_prop_a3[2] = { { -300, 0, 0 }, { -900, 0, 0 } };
        if ((rc = pruef_sonde(502, R->fx, R->fz, 1, k_prop_a3, 2))) return rc;
        /* RE2 kennt kein Band-Tor fuer Objekte (FUN_80038950 a3 = 0 @0x800389a4): anderes Prop-Band, gleiches Ergebnis. */
        g_scd.props[0].band = 1;
        if ((rc = pruef_sonde(503, R->fx, R->fz, 0, k_prop, 6))) return rc;
        g_scd.props[0].band = 0;
        /* Rand r = 2 (FUN_80038950 `addu v0,v0,a2` @0x80038978): |dx| <= hx + 2 drin, hx + 3 draussen. */
        { int k = -1; if (sonde(R->fx + 1002, -300, R->fz, 0, &k) != -301 || k != 1) return fail(504, "Rand r = 2 am Objekt"); }
        { int k = -1; if (sonde(R->fx + 1003, -300, R->fz, 0, &k) != 0 || k != 0)    return fail(505, "ausserhalb hx + r noch Objekt"); }
        lauf_t L; const int32_t q[3] = { R->fx, 10, R->fz };
        lauf(q, 0, &L); zeige("Objekt", &L);
        if ((rc = pruef_haelt(510, &L))) return rc;
        g_scd.prop_count = 0; g_scd.props[0].active = 0;
    }

    /* 6xx Negativ-Kontrollen am ROOM1140-Wandpunkt. */
    {
        const raum_t *R = &k_raeume[0];
        const int32_t qw[3] = { R->wx, 10, R->wz };
        lauf_t L;
        g_room_rdt_ok = 0;                                  /* ohne Raum: nur Grundebene */
        { int k = -1; if (sonde(R->wx, -900, R->wz, 0, &k) != 0 || k != 0) return fail(601, "ohne Raum trotzdem Zellkontakt"); }
        lauf(qw, 0, &L); zeige("ohne Raum", &L);
        if ((rc = pruef_gleitet(610, &L))) return rc;
        g_room_rdt_ok = 1;
        re2fx_boden_hook = boden_flach;                     /* Haken: Raum unbeachtet */
        lauf(qw, 0, &L); zeige("Haken", &L);
        re2fx_boden_hook = NULL;
        if ((rc = pruef_gleitet(620, &L))) return rc;
        /* Gegenprobe mit Raum am selben Punkt (muss halten, sonst prueften 61x/62x nichts). */
        lauf(qw, 0, &L);
        if ((rc = pruef_haelt(630, &L))) return rc;
    }
    if (re2fx_op_unbekannt() != 0) return fail(700, "nicht umgesetzter Op erreicht");
    printf("probe_r34_re2fx_raum: alle Pruefungen gruen\n");
    return 0;
}
