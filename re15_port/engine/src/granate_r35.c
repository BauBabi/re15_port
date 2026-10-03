/* granate_r35.c — Runde 35 Spur A: Handgranate gegen Waende und Gegner im Flug, RE2-Reichweite der
 * Explosion, RE2-Explosionston. Belege und Einordnung: include/re15_granate_r35.h und
 * analysis/befunde_runde35/A_granate.md (§3). */
#include "re15_granate_r35.h"
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_collision.h"
#include "re15_room.h"
#include "re2_fx.h"
#include <string.h>

static unsigned s_n_wand = 0, s_n_kontakt = 0, s_n_expl = 0;   /* s_n_kontakt: nur Sonden-Zaehler */

/* ---- Wand ----------------------------------------------------------------------------------------
 * RE2 FUN_8001ED9C: `jal 0x8004fba0(&Lage, 2, 8192, 0)` @0x8001ee60-a0 setzt DAT_800dcbc8 bei
 * Zellkontakt (FUN_8004fba0 loescht es `sw zero,-13368(at)` @0x8004fc34 und schreibt es im Zellscan);
 * `lw v0,-13368(v0)` / `beq v0,zero,0x8001f10c` @0x8001ef84-8c = frei -> weiterfliegen.
 * Port: die Granaten-Weltlage (slot+0x28/+0x2c) gegen die soliden SCA-Zellen des Werfer-Bandes
 * (granate_boden = Standhoehe des Werfers, Band = -(y / 0x708) wie FUN_8001c2dc), Punkttest r = 0,
 * Maske RE15_GRANATE_R35_WAND_MASKE. */
int re15_granate_r35_wand(const re15_esp_fx_t *f)
{
    if (!f || !g_room_rdt_ok) return 0;
    const int band = re15_collision_band_from_y(f->granate_boden);
    return re15_collision_box_blocked(&g_room_rdt, (int32_t)f->wpos[0], (int32_t)f->wpos[2],
                                      band, 0, RE15_GRANATE_R35_WAND_MASKE) ? 1 : 0;
}

/* ---- Gegner-Kontakt --------------------------------------------------------------------------------
 * RE2 FUN_8001ED9C @0x8001ee90-ef14: Hitcode 0x00030009 + Art (`lui s2,0x3 / ori s2,s2,0x9`,
 * `lb a3,27(v1)` / `addu a3,a3,s2`), Box @0x80010900 {-1400,0,350,250}, `jal 0x800470c0` an
 * y + 1000 (@0x8001eec8/d8) und y - 1000 (@0x8001eef8/f08); `bne s0,zero,0x8001ef38` @0x8001ef14 ->
 * Explosion im selben Bild. Der Kontakt ist hier NUR Ausloeser (PORT-WAHL, Dossier §3.2): die
 * Explosion (unten) trifft den beruehrten Gegner mit. Kandidatentest = die Gates 1-4 + Band + Box des
 * Appliers FUN_800470C0 (re15_re2_gl_kandidat, re15_damage.c), ohne Anwendung. Erster Treffer
 * genuegt (Hitcode ohne Bit 0x10000 = "erster", `beq` @0x80047208-10 -> Abbruch nach dem Treffer). */
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
