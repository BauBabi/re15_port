/*
 * treffer_r39.c - Runde 39 Spur D "treffer": Sichtlinie des Spieler-Schusses (Moebel durchlaessig) und der
 * Zielhilfe (verdeckte Gegner werden nicht angevisiert). Belege und Einordnung: include/re15_treffer_r39.h,
 * Dossier analysis/befunde_runde39/D_treffer.md.
 */
#include "re15_treffer_r39.h"
#include "re15_room.h"       /* g_room_rdt, g_room_rdt_ok, g_current_room_id */
#include "re15_engine.h"     /* g_engine.frame_count (Messschiene) */
#include <stdio.h>
#include <stdlib.h>

#include "re15_zombieki_r36.h" /* re15_r36_los_zelle_kreuzt: der Kreuzungstest von FUN_8003dcc4 fuer EINE Zelle (Runde 36 F) */

static const re15_treffer_r39_moebel_t s_moebel[] = {
#include "gen/treffer_r39_moebel.inc"
};

static int      s_moebel_aus = 0, s_ziel_aus = 0;
static int      s_letzte_zelle = -1;
static unsigned s_zaehler[3];
static int      s_alt_init = 0;

/* Mess-Sonde RE15_TREFFER_ALT=1 (nur Umgebungsvariable, kein Spielverhalten): alter Stand (Moebel sperren, Zielhilfe
 * ohne Sichtlinie) in derselben exe, fuer die Vorher/Nachher-Messung am Echtlauf. Einmal je Prozess gelesen. */
static void alt_sonde_init(void)
{
    if (s_alt_init) return;
    s_alt_init = 1;
    const char *a = getenv("RE15_TREFFER_ALT");
    if (a && *a && *a != '0') { s_moebel_aus = 1; s_ziel_aus = 1; }
}

/* Messschiene RE15_TREFFER_LOG=<datei> (env-gegatet, reine Diagnose): eine Zeile je Sperre/Uebersprung. */
static FILE *s_log = NULL;
static int   s_log_init = 0;
static void log_zeile(const char *art, const re15_actor_t *von, const re15_actor_t *nach, int zelle, int moebel)
{
    if (!s_log_init) {
        const char *p = getenv("RE15_TREFFER_LOG");
        s_log_init = 1;
        if (p && *p) s_log = fopen(p, "a");
    }
    if (!s_log) return;
    fprintf(s_log, "F%u R%04X %s von=(%d,%d) nach=(%d,%d) slot=%d zelle=%d moebel_uebersprungen=%d\n",
            (unsigned)g_engine.frame_count, (unsigned)g_current_room_id, art,
            von->x, von->z, nach->x, nach->z, (int)(nach - g_actors), zelle, moebel);
    fflush(s_log);
}

const re15_treffer_r39_moebel_t *re15_treffer_r39_moebel_tabelle(int *n)
{
    if (n) *n = (int)(sizeof s_moebel / sizeof s_moebel[0]);
    return s_moebel;
}

int re15_treffer_r39_zelle_moebel(unsigned room, const re15_sca_entry_t *c)
{
    alt_sonde_init();
    if (s_moebel_aus || !c) return 0;
    for (size_t i = 0; i < sizeof s_moebel / sizeof s_moebel[0]; i++) {
        const re15_treffer_r39_moebel_t *m = &s_moebel[i];
        if (m->room != (uint16_t)room) continue;
        if (m->x != c->x || m->z != c->z || m->w != c->width || m->d != c->density) continue;
        return 1;
    }
    return 0;
}

/* FUN_8001b9b4 @0x8001b9b4-ba50: Regionen 3..0 (`ori s0,zero,0x3` @0x8001ba08, `addiu s0,s0,-1` @0x8001ba38), je
 * Region FUN_8003dcc4(delta, region, 0xf00, 0x300) @0x8001ba1c-24: Band == Ursprung+0x82, Art-Nibble == 3, Strahl
 * gegen beide Diagonalen (/0x12). ODER der Regionen (`or s1,s1,v0` @0x8001ba28) = die erste sperrende Zelle
 * entscheidet. Runde 39 Spur D: Moebel-Zellen (gen/treffer_r39_moebel.inc) werden uebersprungen - RE2
 * FUN_80050858 ueberspringt Zellen ohne Flagtreffer genauso (`beq v0,zero,0x800508b0` @0x800508c8). */
int re15_treffer_r39_los_blockiert(const re15_actor_t *von, const re15_actor_t *nach)
{
    int moebel = 0;
    s_letzte_zelle = -1;
    if (!von || !nach) return 0;
    if (!g_room_rdt_ok || !g_room_rdt.sca) return 0;          /* keine SCA -> leere Regionen -> 0 (@0x8003dd94) */
    for (int region = 3; region >= 0; region--) {
        int start = 0;
        for (int g = 0; g < region; g++) start += g_room_rdt.sca_rgn[g];
        int cnt = g_room_rdt.sca_rgn[region];
        if (start + cnt > g_room_rdt.sca_count) continue;
        for (int i = 0; i < cnt; i++) {
            const re15_sca_entry_t *c = &g_room_rdt.sca[start + i];
            if ((c->floor >> 4) != von->floor) continue;         /* w10>>12 == +0x82 (@0x8003de60-68) */
            if ((c->floor & 0x0f) != 3) continue;                /* (w10 & 0xf00) == 0x300 (@0x8003de7c-94) */
            if (!re15_r36_los_zelle_kreuzt(von->x, von->z, nach->x, nach->z, c)) continue;
            if (re15_treffer_r39_zelle_moebel(g_current_room_id, c)) { moebel++; s_zaehler[2]++; continue; }
            s_letzte_zelle = start + i;
            s_zaehler[1]++;
            log_zeile("sperre", von, nach, start + i, moebel);
            return 1;
        }
    }
    if (moebel) log_zeile("frei", von, nach, -1, moebel);
    return 0;
}

/* RE2 FUN_80042144 @0x800422a8-b0: `jal 0x80050858` je Kandidat (Flagmaske 0x80, Hoehentest 1), verdeckt ->
 * `bne v0,zero,0x800422f4` = naechstes Wesen. Hier mit demselben Strahl wie der Spieler-Schuss. */
int re15_treffer_r39_zielhilfe_verdeckt(const re15_actor_t *pl, const re15_actor_t *e)
{
    alt_sonde_init();
    if (s_ziel_aus) return 0;
    if (!re15_treffer_r39_los_blockiert(pl, e)) return 0;
    s_zaehler[0]++;
    log_zeile("zielhilfe_verdeckt", pl, e, s_letzte_zelle, 0);
    return 1;
}

/* Messschiene: das Ergebnis der Zielhilfe (Slot des Gewinners, -1 = keiner) — eine Zeile je Wechsel. */
void re15_treffer_r39_log_ziel(int slot)
{
    static int s_letztes = -2;
    if (slot == s_letztes) return;
    s_letztes = slot;
    if (!s_log_init) { const char *p = getenv("RE15_TREFFER_LOG"); s_log_init = 1; if (p && *p) s_log = fopen(p, "a"); }
    if (!s_log) return;
    if (slot >= 0) {
        const re15_actor_t *e = &g_actors[slot];
        fprintf(s_log, "F%u R%04X ziel slot=%d typ=%02x @(%d,%d) st=%u/%u/%u\n", (unsigned)g_engine.frame_count,
                (unsigned)g_current_room_id, slot, e->type, e->x, e->z, e->state, e->sub_state_1, e->sub_state_2);
    } else {
        fprintf(s_log, "F%u R%04X ziel keiner\n", (unsigned)g_engine.frame_count, (unsigned)g_current_room_id);
    }
    fflush(s_log);
}

int      re15_treffer_r39_los_letzte_zelle(void) { return s_letzte_zelle; }
void     re15_treffer_r39_moebel_aus(int aus)     { s_moebel_aus = aus ? 1 : 0; }
void     re15_treffer_r39_zielhilfe_aus(int aus)  { s_ziel_aus = aus ? 1 : 0; }
unsigned re15_treffer_r39_zaehler(int welcher)    { return (welcher >= 0 && welcher < 3) ? s_zaehler[welcher] : 0u; }
void     re15_treffer_r39_zaehler_reset(void)     { s_zaehler[0] = s_zaehler[1] = s_zaehler[2] = 0u; s_letzte_zelle = -1; }
