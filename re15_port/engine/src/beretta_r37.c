/*
 * beretta_r37.c — Runde 37 Spur G: der 3-Schuss-Feuerstoss der Beretta M93R (5) / Glock 18 (6)
 * trifft JE KUGEL. Belege und Abbildung: include/re15_beretta_r37.h, Dossier
 * analysis/befunde_runde37/G_beretta.md.
 *
 * Ablauf je Feuerstoss:
 *   Eintrittsbild  game_step ruft wie bisher den Resolver (Kugel 1, @0x8003396c) und bucht die drei
 *                  Patronen ab (@0x80033974/7c/84); danach re15_beretta_r37_feuerstoss() mit der Zahl
 *                  der wirklich abgebuchten Patronen.
 *   Bild 3 / 6     re15_beretta_r37_tick() loest Kugel 2 / 3 ueber denselben Resolver auf — derselbe
 *                  3-Bild-Abstand wie die RE1.5-FX-Kette (CORE00.ESP Effekt 2 Sub 2, Routine 15, Warte
 *                  0/2/2 @Datei 0x1188/0x11B0/0x11D8 -> Kinder T/T+3/T+6, am Original gemessen B41/B44/
 *                  B47) und die RE2-Kugelbilder 0/3/6 der Custom Handgun (@0x80048910-20).
 */
#include <string.h>
#include "re15_beretta_r37.h"
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"

extern int re15_player_aim_phase_debug(void);   /* player_common.c: Phase | 0x10 waehrend des Rueckstosses */

/* Kugelabstand in Bildern: RE1.5 Routine 15 wartet +0x0E = 2 Takte und spawnt im dritten
 * (@0x80017aec-b0c / @0x80017b38; Zeilen @CORE00.ESP 0x11B0/0x11D8 tragen 2) = 3 Bilder je Kugel;
 * RE2 Bild 3 / 6 (`addiu v0,zero,3` @0x80048914, `addiu v0,zero,6` @0x8004891c). */
#define BERETTA_KUGEL_ABSTAND 3
/* Kugeln je Feuerstoss: drei FX-Kinder (drei Zeilen Routine 15 @0x1188/0x11B0/0x11D8, +0x26 = 1/1/0)
 * und drei Patronen (`jal 0x8004eae4` @0x80033974/7c/84). */
#define BERETTA_KUGELN 3

static int      s_waffe;          /* 5 / 6 des laufenden Feuerstosses */
static int      s_offen;          /* noch aufzuloesende Kugeln (0..2) */
static int      s_bild;           /* Bilder seit dem Eintritt */
static int      s_naechste;       /* Index der naechsten Kugel (1 = Kugel 2) */
static uint8_t  s_getroffen[RE15_ACTOR_MAX];   /* Plaetze, die DIESER Feuerstoss getroffen hat */
static re15_beretta_r37_mess_t s_mess;

int re15_beretta_r37_ist_feuerstoss(int waffe)
{
    return waffe == 5 || waffe == 6;   /* Entlade-Tabelle @0x80074114/18 -> 0x800338A8 */
}

void re15_beretta_r37_reset(void)
{
    s_waffe = 0; s_offen = 0; s_bild = 0; s_naechste = 0;
    memset(s_getroffen, 0, sizeof s_getroffen);
    memset(&s_mess, 0, sizeof s_mess);
}

void re15_beretta_r37_mess(re15_beretta_r37_mess_t *out)
{
    if (out) *out = s_mess;
}

int re15_beretta_r37_offen(void) { return s_offen; }

static void merke_treffer(int r)
{
    if (r > 0 && r - 1 < RE15_ACTOR_MAX) {
        s_getroffen[r - 1] = 1;
        s_mess.treffer++;
    }
}

void re15_beretta_r37_feuerstoss(int waffe, int patronen, int treffer1)
{
    if (!re15_beretta_r37_ist_feuerstoss(waffe)) return;
    memset(s_getroffen, 0, sizeof s_getroffen);
    s_waffe    = waffe;
    s_bild     = 0;
    s_naechste = 1;
    /* Kugel k braucht ihre eigene Patrone (RE2 `jal 0x80069f54` / `bne v0,zero,0x80048958`
     * @0x80048938-40); Kugel 1 hat sie immer (Feuer-Tor FUN_8004ea6c @0x80033314). */
    if (patronen > BERETTA_KUGELN) patronen = BERETTA_KUGELN;
    s_offen = (patronen > 1) ? patronen - 1 : 0;
    s_mess.feuerstoesse++;
    s_mess.kugeln++;
    merke_treffer(treffer1);
}

/* Gibt fuer diesen Typ ein RE2-PAUSENFILTER den Riegel +0x93 Bit 0 nach der RE2-Trefferpause +0x1D3 frei?
 * Nachbesserung 1 (Abnahme 0 M1): das Kriterium ist der Filter, der wirklich laeuft, NICHT der KI-Modus.
 * Spiegel der Weiche im Bild-Nachlauf game_step_common.c (Block "MIXED (2026-08-23)"): `re15_ai_re2_for_type`
 * UND entweder die Zombie-Familie (`re15_re2z_owns_type` -> re15_re2z_hit_filter_apply, Gate +0x1D3
 * @0x80047138-40) oder Hund/Kraehe/Spinne 0x20/0x21/0x25/0x26 (re15_re2_pause_filter_apply, dessen Typ-Tor in
 * re15_damage.c genau diese vier annimmt). Alle anderen Gegner — im RE2-Flavor die RE1.5-Gehirne 0x23/0x27/
 * 0x29/0x2b/0x30 — bekommen weder Stempel (re15_re2_stun_frames `return 0`) noch Filter; ihren Riegel gibt nur
 * das Ende der RE1.5-Trefferreaktion frei (z.B. Kakerlake `sb zero,147(a0)` STAGE3.BIN @0x801149ac). */
static int re2_pausenfilter_laeuft(unsigned t)
{
    if (!re15_ai_re2_for_type(t)) return 0;
    if (re15_re2z_owns_type(t)) return 1;
    return t == 0x20u || t == 0x21u || t == 0x25u || t == 0x26u;
}

int re15_beretta_r37_re2_pause_regelt(unsigned type) { return re2_pausenfilter_laeuft(type); }

/* Den RE1.5-Riegel +0x93, den DIESER Feuerstoss gesetzt hat, vor Kugel 2/3 loesen — bei lebenden Gegnern,
 * fuer die KEIN RE2-Pausenfilter laeuft (dort entscheidet die RE2-Zeile 4: Zombie 1 Bild, Hund/Spinne 0,
 * Kraehe 15 — @0x800A4168 / @0x800A4460 / @0x800A4BCC / @0x800A45DC). */
static void riegel_loesen(void)
{
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!s_getroffen[s] || !e->active) continue;
        if (e->hp < 0) continue;                                   /* sterbend: Riegel bleibt (@0x80012520-2c) */
        if (re2_pausenfilter_laeuft((unsigned)e->type)) continue;  /* RE2-Pause (+0x1D3) + Filter regeln selbst */
        if (e->hit_react & 0x3u) {
            e->hit_react = (uint8_t)(e->hit_react & ~0x3u);        /* Bits 0|1 (`ori 0x1` @0x800124F0, `|= 2` @0x8001240c) */
            s_mess.riegel_geloest++;
        }
    }
}

int re15_beretta_r37_tick_ex(int noch_im_feuer)
{
    if (s_offen <= 0) return 0;
    if (!noch_im_feuer) {             /* Rueckstoss vorbei / Treffer / Griff / Raumwechsel / Waffenwechsel */
        s_offen = 0;
        s_mess.abgebrochen++;
        return 0;
    }
    s_bild++;
    if (s_bild != BERETTA_KUGEL_ABSTAND * s_naechste) return 0;
    riegel_loesen();
    int r = re15_player_weapon_fire(s_waffe);
    s_mess.kugeln++;
    merke_treffer(r);
    s_naechste++;
    s_offen--;
    return r;
}

int re15_beretta_r37_tick(void)
{
    if (s_offen <= 0) return 0;
    int im_feuer = (re15_player_aim_phase_debug() & 0x10) != 0          /* Rueckstoss laeuft (s_aim_recoil) */
                && re15_player_equipped_weapon() == s_waffe;
    return re15_beretta_r37_tick_ex(im_feuer);
}
