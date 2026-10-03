/* granate_r35.c — Runde 35 Spur A: Handgranate gegen Waende und Gegner im Flug, RE2-Reichweite der
 * Explosion, RE2-Explosionston. Belege und Einordnung: include/re15_granate_r35.h und
 * analysis/befunde_runde35/A_granate.md (§3, Nachbesserung 1 N1.1). */
#include "re15_granate_r35.h"
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_collision.h"
#include "re15_room.h"
#include "re2_fx.h"
#include <string.h>
#include <stdio.h>

static unsigned s_n_wand = 0, s_n_kontakt = 0, s_n_expl = 0;   /* s_n_kontakt: nur Sonden-Zaehler */

/* ==== Wand ==========================================================================================
 * RE2 FUN_8001ED9C: `jal 0x8004fba0(&Lage, 2, 0x2000, 0)` @0x8001ee60-a0 setzt DAT_800dcbc8;
 * `lw v0,-13368(v0)` / `beq v0,zero,0x8001f10c` @0x8001ef84-8c = frei -> weiterfliegen.
 * FUN_8004fba0 prueft je Zelle Quadrant (@0x8004fd74-80), Rechteck-Vortest (@0x8004fd88-b8),
 * Klassenmaske (@0x8004fdc0-d0), FORM (`jr` ueber Tabelle 0x80011104 @0x8004fe34-54) und das
 * Hoehenfenster der Zelle (@0x8004ffd4-0x8005000c). Port auf den RE1.5-Zellen (12 B, SCA):
 *   - Quadrant: Liste des Quadranten der Lage (FUN_8003b068, wie FUN_8003b0a4).
 *   - Klasse:   u0 & RE15_GRANATE_R35_WAND_MASKE (PORT-WAHL, Header).
 *   - Form:     die soliden Flaechen der RE1.5-Handler 0x800b2858[typ] (FUN_8003aea0 @0x8003af04-84:
 *               1 FUN_8003bca8, 2 LAB_8003d00c, 3 FUN_8003d6a8, 4 LAB_8003beb0, 5 LAB_8003c734,
 *               6 LAB_8003cb9c, 7 LAB_8003c2cc, 8 LAB_8003d7e8, 9 LAB_8003d930) bei Radius 0 —
 *               Port-Zwillinge push_rect/push_diag2/push_circle/push_diag4..7/push_caps8/9
 *               (re15_collision.c); Herleitung der Flaechen im Dossier N1.1.
 *   - Hoehe:    die RE1.5-Zelle traegt KEINE Hoehe (Dossier N1.1 "Hoehe", OFFEN N1): jede Zelle
 *               haelt in jeder Flughoehe. ABWEICHUNG von RE2 @0x8004ffd4-0x8005000c, gekennzeichnet.
 *   - Strecke:  RE2 tastet EINEN Punkt je Bild. NUTZER-VORGABE "nicht durch die Wand": die Granate
 *               legt 380 (HOCH @0x80018494) / 280 (MITTE @0x800184bc) je Bild zurueck und uebersprang
 *               Zellen, die duenner sind (gemessen Abnahme 0, ROOM1220 x[-21825..-21550]). PORT-WAHL:
 *               getestet wird die STRECKE von der vorigen zur neuen Weltlage; im Wurfbild (noch keine
 *               vorige Lage) die Strecke vom Werfer zur Hand.
 * Rechnung ganzzahlig (s64-Kreuzprodukte), kein Gleitkomma (Engine laeuft auch auf der PSX). */

typedef struct { int32_t x, z; } p2_t;

static int64_t kreuz(int64_t ax, int64_t az, int64_t bx, int64_t bz) { return ax * bz - az * bx; }

/* Strecke a-b gegen ein konvexes Vieleck (Trennachsen: Kanten des Vielecks + die Strecke selbst).
 * Beruehren zaehlt als Treffer. */
static int strecke_vieleck(const p2_t *v, int n, p2_t a, p2_t b)
{
    int64_t fl = 0;
    for (int i = 0; i < n; i++) { const int j = (i + 1) % n; fl += kreuz(v[i].x, v[i].z, v[j].x, v[j].z); }
    if (fl == 0) return 0;                                   /* entartete Zelle (w oder d = 0) */
    const int64_t sg = (fl > 0) ? 1 : -1;
    for (int i = 0; i < n; i++) {
        const int j = (i + 1) % n;
        const int64_t ex = (int64_t)v[j].x - v[i].x, ez = (int64_t)v[j].z - v[i].z;
        const int64_t sa = sg * kreuz(ex, ez, (int64_t)a.x - v[i].x, (int64_t)a.z - v[i].z);
        const int64_t sb = sg * kreuz(ex, ez, (int64_t)b.x - v[i].x, (int64_t)b.z - v[i].z);
        if (sa < 0 && sb < 0) return 0;                      /* beide Enden ausserhalb dieser Kante */
    }
    const int64_t dx = (int64_t)b.x - a.x, dz = (int64_t)b.z - a.z;
    if (dx || dz) {
        int pos = 0, neg = 0;
        for (int i = 0; i < n; i++) {
            const int64_t s = kreuz(dx, dz, (int64_t)v[i].x - a.x, (int64_t)v[i].z - a.z);
            if (s > 0) pos++; else if (s < 0) neg++;
        }
        if (pos == n || neg == n) return 0;                  /* Vieleck ganz auf einer Seite der Strecke */
    }
    return 1;
}

/* Strecke a-b gegen einen Kreis (Abstand Mitte-Strecke < r). Lange Strecken werden halbiert, damit
 * kr*kr in s64 bleibt (Granatenschritt <= 380, Wurfbild <= ~1300; nur Sonden rufen laenger). */
static int strecke_kreis(int32_t cx, int32_t cz, int32_t r, p2_t a, p2_t b)
{
    if (r <= 0) return 0;
    const int64_t dx = (int64_t)b.x - a.x, dz = (int64_t)b.z - a.z;
    if (dx > 4096 || dx < -4096 || dz > 4096 || dz < -4096) {
        const p2_t m = { (int32_t)(a.x + dx / 2), (int32_t)(a.z + dz / 2) };
        return strecke_kreis(cx, cz, r, a, m) || strecke_kreis(cx, cz, r, m, b);
    }
    const int64_t fx = (int64_t)cx - a.x, fz = (int64_t)cz - a.z;
    const int64_t l2 = dx * dx + dz * dz, r2 = (int64_t)r * r;
    const int64_t dot = fx * dx + fz * dz;
    if (l2 == 0 || dot <= 0) return fx * fx + fz * fz < r2;
    if (dot >= l2) { const int64_t gx = (int64_t)cx - b.x, gz = (int64_t)cz - b.z; return gx * gx + gz * gz < r2; }
    const int64_t kr = kreuz(fx, fz, dx, dz);
    return kr * kr < r2 * l2;
}

/* Strecke a-b gegen die solide Flaeche EINER RE1.5-Zelle (Typ 1..9, Tabelle Dossier N1.1). */
static int strecke_zelle(const re15_sca_entry_t *e, p2_t a, p2_t b)
{
    const int32_t x = e->x, z = e->z, w = e->width, d = e->density;
    const int32_t xm = x + w, zm = z + d;
    p2_t v[4];
    switch (e->type) {
    case 1:                                                  /* FUN_8003bca8: Rechteck */
        v[0] = (p2_t){ x, z }; v[1] = (p2_t){ xm, z }; v[2] = (p2_t){ xm, zm }; v[3] = (p2_t){ x, zm };
        return strecke_vieleck(v, 4, a, b);
    case 2: {                                                /* LAB_8003d00c: Raute um (x+w/2, z+d/2) */
        const int32_t cx = x + (w >> 1), cz = z + (d >> 1);
        v[0] = (p2_t){ cx, z }; v[1] = (p2_t){ xm, cz }; v[2] = (p2_t){ cx, zm }; v[3] = (p2_t){ x, cz };
        return strecke_vieleck(v, 4, a, b);
    }
    case 3:                                                  /* FUN_8003d6a8: Kreis, Radius w/2 */
        return strecke_kreis(x + (w >> 1), z + (w >> 1), w >> 1, a, b);
    case 4:                                                  /* LAB_8003beb0: rechter Winkel (x+w, z+d) */
        v[0] = (p2_t){ x, zm }; v[1] = (p2_t){ xm, z }; v[2] = (p2_t){ xm, zm };
        return strecke_vieleck(v, 3, a, b);
    case 5:                                                  /* LAB_8003c734: rechter Winkel (x, z+d) */
        v[0] = (p2_t){ x, z }; v[1] = (p2_t){ xm, zm }; v[2] = (p2_t){ x, zm };
        return strecke_vieleck(v, 3, a, b);
    case 6:                                                  /* LAB_8003cb9c: rechter Winkel (x+w, z) */
        v[0] = (p2_t){ x, z }; v[1] = (p2_t){ xm, zm }; v[2] = (p2_t){ xm, z };
        return strecke_vieleck(v, 3, a, b);
    case 7:                                                  /* LAB_8003c2cc: rechter Winkel (x, z) */
        v[0] = (p2_t){ x, zm }; v[1] = (p2_t){ xm, z }; v[2] = (p2_t){ x, z };
        return strecke_vieleck(v, 3, a, b);
    case 8: {                                                /* LAB_8003d7e8: Kapsel in x, Kappen-Durchmesser d */
        const int32_t h = d >> 1, x0 = x + h, x1 = xm - h;
        if (x0 < x1) {
            v[0] = (p2_t){ x0, z }; v[1] = (p2_t){ x1, z }; v[2] = (p2_t){ x1, zm }; v[3] = (p2_t){ x0, zm };
            if (strecke_vieleck(v, 4, a, b)) return 1;
        }
        return strecke_kreis(x + h, z + h, h, a, b) || strecke_kreis(xm - d + h, z + h, h, a, b);
    }
    case 9: {                                                /* LAB_8003d930: Kapsel in z, Kappen-Durchmesser w */
        const int32_t h = w >> 1, z0 = z + h, z1 = zm - h;
        if (z0 < z1) {
            v[0] = (p2_t){ x, z0 }; v[1] = (p2_t){ xm, z0 }; v[2] = (p2_t){ xm, z1 }; v[3] = (p2_t){ x, z1 };
            if (strecke_vieleck(v, 4, a, b)) return 1;
        }
        return strecke_kreis(x + h, z + h, h, a, b) || strecke_kreis(x + h, zm - w + h, h, a, b);
    }
    default:
        return 0;                                            /* Tabelle 0x800b2858 kennt nur 1..9 */
    }
}

/* FUN_8003b068 — Quadrant aus dem Vorzeichen (Lage - Deckenpunkt): Bit 1 = z davor, Bit 0 = x davor. */
static int quadrant(const re15_rdt_t *rdt, int32_t x, int32_t z)
{
    const unsigned zb = (unsigned)(z - (int32_t)(int16_t)rdt->ceiling_z) & 0x80000000u;
    const unsigned xb = (unsigned)(x - (int32_t)(int16_t)rdt->ceiling_x) & 0x80000000u;
    return (int)((zb | (xb >> 1)) >> 30);
}

static int strecke_liste(const re15_rdt_t *rdt, int q, p2_t a, p2_t b, int band, unsigned maske)
{
    int start = 0;
    for (int i = 0; i < q; i++) start += rdt->sca_rgn[i];
    int end = start + rdt->sca_rgn[q];
    if (end > rdt->sca_count) end = rdt->sca_count;
    for (int i = start; i < end; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        if (band != (e->floor >> 4)) continue;               /* Band strikt gleich (FUN_8003b0a4) */
        if ((maske & e->u0) == 0) continue;                  /* Klassenmaske */
        if (strecke_zelle(e, a, b)) return 1;
    }
    return 0;
}

/* Punkt der Strecke bei t/65536 (auf 0 gerundet; +-1 Einheit an der Quadrantengrenze). */
static p2_t punkt_bei(p2_t a, p2_t b, int64_t t)
{
    p2_t p;
    p.x = (int32_t)(a.x + (((int64_t)b.x - a.x) * t) / 65536);
    p.z = (int32_t)(a.z + (((int64_t)b.z - a.z) * t) / 65536);
    return p;
}

int re15_granate_r35_strecke(const re15_rdt_t *rdt, int32_t x0, int32_t z0, int32_t x1, int32_t z1,
                             int band, unsigned maske)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0 || band < 0) return 0;
    const p2_t a = { x0, z0 }, b = { x1, z1 };
    /* Die Strecke an den Quadrantengrenzen (x = Deckenpunkt.x, z = Deckenpunkt.z) teilen: jedes Stueck
     * gilt gegen die Liste SEINES Quadranten. */
    int64_t t[4]; int n = 0;
    t[n++] = 0;
    const int32_t cx = (int16_t)rdt->ceiling_x, cz = (int16_t)rdt->ceiling_z;
    if ((x0 < cx) != (x1 < cx)) t[n++] = (((int64_t)cx - x0) * 65536) / ((int64_t)x1 - x0);
    if ((z0 < cz) != (z1 < cz)) {
        const int64_t tz = (((int64_t)cz - z0) * 65536) / ((int64_t)z1 - z0);
        if (n == 2 && tz < t[1]) { t[2] = t[1]; t[1] = tz; n = 3; } else t[n++] = tz;
    }
    t[n++] = 65536;
    for (int i = 0; i + 1 < n; i++) {
        const p2_t s0 = (i == 0) ? a : punkt_bei(a, b, t[i]);
        const p2_t s1 = (i + 2 == n) ? b : punkt_bei(a, b, t[i + 1]);
        const p2_t m  = punkt_bei(a, b, (t[i] + t[i + 1]) / 2);
        if (strecke_liste(rdt, quadrant(rdt, m.x, m.z), s0, s1, band, maske)) return 1;
    }
    return 0;
}

int re15_granate_r35_punkt(const re15_rdt_t *rdt, int32_t x, int32_t z, int band, unsigned maske)
{
    return re15_granate_r35_strecke(rdt, x, z, x, z, band, maske);
}

/* Anfang der Flugstrecke dieses Bildes.
 *   Wurfbild (xlat == 0: die Physik @0x8001a324-388 hat den Platz noch nie bewegt): der Werfer.
 *   sonst: die vorige Weltlage. Physik je Tick `xlat += vel; vel += acc` (@0x8001a324-388) ->
 *   xlat_alt = xlat - (vel - acc) je Achse; Weltlage daraus wie im Tick (@0x8001a118-2a4). Nach einem
 *   Abprall (R29 @0x800183c4-f8) ist das die auf die Ebene korrigierte Abprallstelle. */
static p2_t strecken_anfang(const re15_esp_fx_t *f)
{
    p2_t s;
    if (f->xlat_x == 0 && f->xlat_y == 0 && f->xlat_z == 0) {
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        s.x = pl->x; s.z = pl->z;
        return s;
    }
    re15_esp_fx_t alt = *f;
    alt.xlat_x -= (int32_t)(int16_t)(f->drift_x - f->accel_x);
    alt.xlat_y -= (int32_t)(int16_t)(f->drift_y - f->accel_y);
    alt.xlat_z -= (int32_t)(int16_t)(f->drift_z - f->accel_z);
    re15_esp_r35_weltlage(&alt);
    s.x = alt.wpos[0]; s.z = alt.wpos[2];
    return s;
}

int re15_granate_r35_wand(const re15_esp_fx_t *f)
{
    if (!f || !g_room_rdt_ok) return 0;
    const int band = re15_collision_band_from_y(f->granate_boden);
    const p2_t s = strecken_anfang(f);
    return re15_granate_r35_strecke(&g_room_rdt, s.x, s.z, (int32_t)f->wpos[0], (int32_t)f->wpos[2],
                                    band, RE15_GRANATE_R35_WAND_MASKE);
}

/* ---- Gegner-Kontakt --------------------------------------------------------------------------------
 * RE2 FUN_8001ED9C @0x8001ee90-ef14: Hitcode 0x00030009 + Art (`lui s2,0x3 / ori s2,s2,0x9`,
 * `lb a3,27(v1)` / `addu a3,a3,s2`), Box @0x80010900 {-1400,0,350,250}, `jal 0x800470c0` an
 * y + 1000 (@0x8001eec8/d8) und y - 1000 (@0x8001eef8/f08); `bne s0,zero,0x8001ef38` @0x8001ef14 ->
 * Status |= 0x80 (@0x8001ef4c-50) und Sprung ueber step[2] (`lbu v0,2(v1)` @0x8001ef74 / `j 0x8001f0e4`).
 * Hier NUR Geometrie (Kandidatentest = Gates 1-4 + Band + Box des Appliers FUN_800470C0,
 * re15_re2_gl_kandidat), ohne Anwendung. Erster Treffer genuegt (Hitcode ohne Bit 0x10000). */
int re15_granate_r35_kontakt(const re15_esp_fx_t *f)
{
    if (!f) return 0;
    s_n_kontakt++;
    static const int16_t box0[4] = RE15_GRANATE_R35_BOX_KONTAKT;
    int32_t p[3] = { (int32_t)f->wpos[0], (int32_t)f->wpos[1] + RE15_GRANATE_R35_KONTAKT_DY,
                     (int32_t)f->wpos[2] };
    for (int probe = 0; probe < 2; probe++) {
        int16_t box[4];
        memcpy(box, box0, sizeof box);
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
            if (re15_re2_gl_kandidat(&g_actors[s], p, f->param, box)) return 1;
        }
        p[1] = (int32_t)f->wpos[1] - RE15_GRANATE_R35_KONTAKT_DY;
    }
    return 0;
}

/* ⛔ GEMESSEN (Runde 35, Laeufe n1_mitte/n2_tief, Dossier §4.2): der RE2-Flugkontakt ist fuer die
 * Handgranate NICHT verdrahtet. Die Box @0x80010900 liegt als gefegtes Volumen HINTER dem Geschoss
 * (Ecke -1400, Kante 4*350 = 1400 -> lokal x in [-1400, 0]) und das Band des Appliers ist 3200 hoch
 * (b98 -1500, h9e 1500 @0x8004716c-a4) — passend zur schnellen, flachen RE2-Runde. Auf dem langsamen,
 * hohen Bogen der Handgranate (280/Bild, Scheitel bis ~3000) zuendete sie damit in der LUFT ueber einem
 * schon ueberflogenen Zombie: n1 Kontakt bei y -2614 vier Bilder nach dem Wurf (P 3114 ueber dem Boden),
 * n2 drei Bilder nach dem Wurf neben Leon. Das widerspricht dem Nutzerziel (die Gruppe erreichen); die
 * Granate behaelt RE1.5s Zeitzuender (@0x80018474, 36 Bilder nach dem Liegen). Die Kontaktgeometrie
 * bleibt als Funktion fuer die Sonde (unit_r35_granate 110-112) und das Dossier erhalten. */
int re15_granate_r35_flugtest(const re15_esp_fx_t *f)
{
    if (re15_granate_r35_wand(f)) { s_n_wand++; return 1; }
    return 0;
}

/* ---- Rueckzug aus der Wand -------------------------------------------------------------------------
 * RE2 @0x8001ef90-0x8001f0c8 (je Achse x/y/z = +0xC/+0xE/+0x10 vel, +0x8/+0x9/+0xA acc s8,
 * +0x24/+0x26/+0x28 pos; Idiom 0x55555556 = /3 auf 0 gerundet, Vorzeichenkorrektur `sra 31 / subu`):
 *   8001efd4 subu v0,v0,v1 / sh v0,12(t2)      vel.x -= acc.x
 *   8001eff4 lhu a0,12(t2)                      a0 = vel.x'  (nach dem ersten Abzug)
 *   8001f000 subu v0,v0,a2 / sh v0,12(t2)       vel.x -= acc.x  (zweiter Abzug)
 *   8001f018 mult v0,t1 ... 8001f098 subu a2,t3,a2    a2 = vel.x''/3
 *   8001f020 subu v1,v1,a0 / sh v1,36(t2)       pos.x -= vel.x'
 *   8001f0a4 subu v0,v0,a2 / sh v0,36(t2)       pos.x -= vel.x''/3
 * RE1.5-Slot: vel = drift (+0x10), acc = accel (+0x08), pos = xlat (+0x34, s32). */
static void rueckzug_achse(int32_t *xlat, int16_t *drift, int16_t accel)
{
    const int16_t v1 = (int16_t)(*drift - accel);
    const int16_t v2 = (int16_t)(v1 - accel);
    *xlat -= (int32_t)v1;
    *xlat -= (int32_t)v2 / 3;              /* C-Division = auf 0 gerundet (0x55555556-Idiom) */
    *drift = v2;
}
void re15_granate_r35_rueckzug(re15_esp_fx_t *f)
{
    if (!f) return;
    rueckzug_achse(&f->xlat_x, &f->drift_x, f->accel_x);
    rueckzug_achse(&f->xlat_y, &f->drift_y, f->accel_y);
    rueckzug_achse(&f->xlat_z, &f->drift_z, f->accel_z);
}

/* ---- Flug-Haken der Routine 29 (EIN Aufruf am Kopf von esp_fx_dispatch_b_29, re15_esp.c) -----------
 * Wand (oben) -> Rueckzug (RE2 @0x8001ef90-0x8001f0c8) -> Weltlage neu (RE2 `jal 0x8001d894`
 * @0x8001f0cc) -> Explosion SOFORT: RE2 springt im selben Bild in die Op aus step[3] + Art
 * (`lbu v0,3(v1)` @0x8001f0e0 / `lb v1,27(v1)` / Tabelle 0x8009d868 / `jalr v0` @0x8001f104). Port:
 * der Platz nimmt den Liegezustand der Routine 29 (`ori v0,zero,0x63 / sb v0,108` @0x80018368-6c,
 * `ori v0,zero,0x1f / sh v0,0` @0x80018378-7c, `sh zero,2` @0x80018384) mit Zuender 7 (`ori v0,zero,0x7`
 * @0x8001856c) und Routine 31 laeuft noch in diesem Tick (Zuender 7 -> 6, danach 2 = Nachbrand, 0 = frei).
 *
 * WURFBILD (PORT-WAHL zur NUTZER-VORGABE "nicht durch die Wand"): liegt schon zwischen Werfer und Hand
 * eine Zelle, gibt es keine freie vorige Lage, auf die der RE2-Rueckzug zurueckfuehren koennte. Bleibt die
 * Strecke Werfer -> Rueckzugspunkt blockiert, zuendet die Granate ueber dem Standpunkt des Werfers
 * (Anker x/z um die Differenz verschoben; keine Konstante). */
static void row_setze16(re15_esp_fx_t *f, int off, uint16_t v)
{
    f->row[off] = (uint8_t)v; f->row[off + 1] = (uint8_t)(v >> 8);
}
int re15_granate_r35_flug(re15_esp_fx_t *f)
{
    if (!re15_granate_r35_flugtest(f)) return 0;
    const int wurfbild = (f->xlat_x == 0 && f->xlat_y == 0 && f->xlat_z == 0);
    const p2_t von = strecken_anfang(f);
    const int16_t vor[3] = { f->wpos[0], f->wpos[1], f->wpos[2] };
    re15_granate_r35_rueckzug(f);
    re15_esp_r35_weltlage(f);
    if (wurfbild && re15_granate_r35_strecke(&g_room_rdt, von.x, von.z, (int32_t)f->wpos[0], (int32_t)f->wpos[2],
                                             re15_collision_band_from_y(f->granate_boden),
                                             RE15_GRANATE_R35_WAND_MASKE)) {
        f->x += von.x - (int32_t)f->wpos[0];
        f->z += von.z - (int32_t)f->wpos[2];
        re15_esp_r35_weltlage(f);
    }
    unsigned tick = 0;
    FILE *gl = re15_esp_r35_log(&tick);
    if (gl) fprintf(gl, "T=%u EV wand wpos=(%d,%d,%d) -> rueckzug (%d,%d,%d) -> explosion sofort von=(%d,%d)%s\n",
                    tick, (int)vor[0], (int)vor[1], (int)vor[2],
                    (int)f->wpos[0], (int)f->wpos[1], (int)f->wpos[2], (int)von.x, (int)von.z,
                    wurfbild ? " wurfbild" : "");
    f->flags = 0x63;
    row_setze16(f, 0x00, 31);
    row_setze16(f, 0x02, 0);
    row_setze16(f, 0x1e, 7);
    re15_esp_r35_routine_a(f);
    return 1;
}

/* ---- Explosion -------------------------------------------------------------------------------------
 * RE2 Op 47 @0x80020c3c: Box @0x80010918 an P (`jal 0x800470c0` @0x80020d78) und an P + 900
 * (`addiu v0,v0,900` @0x80020d98, jal @0x80020db0), Hitcode 0x10020009 (`lui a3,0x1002 / ori a3,a3,0x9`
 * @0x80020d54-58; Bit 0x10000 = ALLE Kandidaten, `andi v0,v0,0x10000`-Gate @0x80047208-10). Die
 * zweite Hoehe trifft nur Kandidaten, die der ersten entgangen sind (Sperre +0x1D3 nach dem Treffer
 * @0x8004731c-4c = Gate 2 beim zweiten Lauf) -> hier: jeder Platz hoechstens einmal gesammelt.
 * Anwendung: der RE1.5-Gegnerzweig von FUN_80012d60 (@0x80012f54-0x80013024: Gate B, Richtungsbit,
 * Schaden E4, +0x4/+0x5/+0x93, RE2-Stempel) in umgekehrter Sammelreihenfolge wie das Original
 * (`do-while` @0x80012f12-30). Kein Spielerzweig: RE2 FUN_800470C0 iteriert nur die Gegnerliste
 * (0x800CFBF3 @0x800470c4-0x8004710c). */
int re15_granate_r35_explosion(const int32_t p[3], int16_t gier, uint8_t art)
{
    if (!p) return 0;
    static const int16_t box0[4] = RE15_GRANATE_R35_BOX_EXPLOSION;
    int   slots[RE15_ACTOR_MAX];
    int32_t punkt[RE15_ACTOR_MAX][3];
    int   n = 0;
    int32_t q[3] = { p[0], p[1], p[2] };
    s_n_expl++;
    for (int probe = 0; probe < 2; probe++) {
        int16_t box[4];
        memcpy(box, box0, sizeof box);
        for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
            int schon = 0;
            for (int k = 0; k < n; k++) if (slots[k] == s) { schon = 1; break; }
            if (schon) continue;
            if (re15_re2_gl_kandidat(&g_actors[s], q, gier, box)) {
                slots[n] = s;
                punkt[n][0] = q[0]; punkt[n][1] = q[1]; punkt[n][2] = q[2];
                n++;
            }
        }
        q[1] = p[1] + RE15_GRANATE_R35_EXPLOSION_DY;
    }
    int hits = 0;
    for (int k = n - 1; k >= 0; k--) {
        if (re15_resolver_gegnerzweig(&g_actors[slots[k]], art, punkt[k], NULL) >= 0) hits++;
    }
    return hits;
}

void re15_granate_r35_explosion_se(const int32_t p[3])
{
    if (re2fx_se_hook) re2fx_se_hook(RE15_GRANATE_R35_SE_EXPLOSION, p);
}

void re15_granate_r35_zaehler(unsigned *wand, unsigned *kontakt, unsigned *explosionen)
{
    if (wand)        *wand        = s_n_wand;
    if (kontakt)     *kontakt     = s_n_kontakt;
    if (explosionen) *explosionen = s_n_expl;
}
void re15_granate_r35_zaehler_reset(void) { s_n_wand = s_n_kontakt = s_n_expl = 0; }
