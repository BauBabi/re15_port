/* fahrstuhl_r37.c — Runde 37 Spur C "fahrstuhl": Tuersequenz beim AUSSTEIGEN aus den Fahrstuhlkabinen
 * (analysis/befunde_runde37/C_fahrstuhl.md, Auftrag #7 "Beim Aussteigen aus dem Fahrstuhl fehlt noch die
 * Tuersequenz").
 *
 * ⛔ RE2-ERGAENZUNG (Beta -> Retail), Belege (Bytes selbst gelesen):
 *   RE1.5 ROOM1080.RDT (= ROOM1081): main00 @0x00482/@0x004A2/@0x004C2 Door_aot_set Platz 0/1/2, Rechteck
 *         (0,0,0,0) -> ROOM1040 / ROOM10C0 / ROOM1120; sub07..09 (Knopf 1F/2F/3F): Set(2,7)=1 @0x007BE,
 *         Set(1,0x1B)=1 @0x007C2, Plc_dest (-13650,-600) @0x007F8, Edwhile Ck(5,0x20) @0x00806, Aot_on 0/1/2
 *         @0x0077C/@0x0080E/@0x008A0 (sub10 @0x008B4/C4/D4: Tuer-auf-Knopf).
 *   RE1.5 ROOM4020.RDT (= ROOM4021): Platz 0/1/2 @0x0063A/@0x0065A/@0x0067A (0,0,0,0) -> ROOM4000 / ROOM5000 /
 *         ROOM50C0; Aot_on @0x00990/@0x00A36/@0x00ACC, sub10 @0x00AE0/AF0/B00.
 *   RE1.5 PSX.EXE Aot_on @0x800407bc: Satz 0x800ac9b0[Platz], `jalr v0` @0x8004082c auf 0x8007469c[sce] = der
 *         sce-2-Tuer-Handler wie beim Betreten; die Tuermaschine spielt DOOR00 = Evt_end (keine Sequenz).
 *   RE2   ROOM6010.RDT (A-2-Kabine): main00 @0x004C2/@0x004E2/@0x00502 Door_aot_set aot 0/1/2, Nutzlast +12/+13 =
 *         27 00 (DOOR27 Variante 0); sub02 @0x006F6 Set(2,7)=1, @0x006FA Set(1,0x1B)=1 (dieselben Merker),
 *         @0x00740 Aot_reset aot 0 sce 1 sat 0x21 (Ziel neu, Archiv/Variante bleiben), @0x0075A Plc_dest in das
 *         Tuerrechteck -> die Tuer feuert und RE2 spielt DOOR27 V0 beim AUSSTIEG.
 *
 * Was hier entschieden wird:
 *   * ROOM4020/4021 (Labor-Kabine "A-2 ELEVATOR", gemalt wie RE2 - die Einstiege S215/S260/S290 spielen seit
 *     Runde 31 DOOR27 V0): Ausstieg = RE2-Datei DOOR27 V0 unveraendert, wie ROOM6010.
 *   * ROOM1080/1081 (Revier-Kabine): dieselbe Tuer wie beim Einstieg (RE2: beide Seiten eines Paares tragen
 *     dasselbe Archiv, 238/238, Runde 31 tueren_02_re2.md 2.1) = Port-Archiv der Revier-Fahrstuhltuer, Kabinenseite
 *     P27C (gemalt ROOM10800 Cut 0; Flurseite P27F fuer S013/S040/S057, tools/fahrstuhl_r37/fahrstuhl_archiv_bauen.py).
 *   * Variante 0 = RE2 ROOM6010 @0x004C2 Byte 27 (00); DOOR27 Skript 0 verteilt V0..V3 alle auf Skript 1.
 *   * Seiten-/Tuernummern = Runde 31 (zuordnung.json): S027/S028/S029 (T011/T017/T018), S221/S222/S223
 *     (T109/T115/T114) - nur Protokoll/Pruefhaken.
 *   * Die Zwischensequenz-Sperre der Runde 31 (aot_common.c tuer_sequenz_anfragen: player_mode 2 oder Balken)
 *     gilt hier NICHT: player_mode 2 kommt aus flag(1,27)||flag(2,7), die RE1.5 sub07..09 und RE2 ROOM6010 sub02
 *     gleich setzen - und RE2 spielt die Tuer in genau diesem Zustand.
 * Der Schluessel ist Raum + Ziel (Stage/Raum des Satzes), nicht die Flaeche: die drei Saetze haben dasselbe
 * Null-Rechteck, die Port-Tabelle (Raum + Flaeche + Band) truege sie nicht auseinander.
 */
#include "re15_fahrstuhl_r37.h"

#include <stddef.h>
#include <string.h>

typedef struct {
    uint16_t    raum;        /* Kabine ohne Spielervariante */
    uint8_t     ziel_stage;  /* Door_aot_set pc[22] (0 = STAGE1) */
    uint8_t     ziel_raum;   /* pc[23] */
    uint16_t    seite, tuer; /* Runde 31 */
    uint32_t    off;         /* Datei-Offset des Door_aot_set (ROOMxxx0 = ROOMxxx1) */
    const char *kennung;     /* Port-Archiv (gen/re15_tuer_eigen.inc) oder NULL = RE2-Datei DOOR27 */
} ausstieg_t;

static const ausstieg_t k_ausstiege[] = {
    { 0x1080, 0, 0x04,  27,  11, 0x00482, "P27C" },   /* -> ROOM1040 (1F) */
    { 0x1080, 0, 0x0C,  28,  17, 0x004A2, "P27C" },   /* -> ROOM10C0 (2F) */
    { 0x1080, 0, 0x12,  29,  18, 0x004C2, "P27C" },   /* -> ROOM1120 (3F) */
    { 0x4020, 3, 0x00, 221, 109, 0x0063A, NULL },     /* -> ROOM4000 */
    { 0x4020, 4, 0x00, 222, 115, 0x0065A, NULL },     /* -> ROOM5000 */
    { 0x4020, 4, 0x0C, 223, 114, 0x0067A, NULL },     /* -> ROOM50C0 */
};
#define N_AUS ((int)(sizeof k_ausstiege / sizeof k_ausstiege[0]))

/* DOOR27 = RE2-Fahrstuhltuer (RE2 ROOM6010 @0x004C2 Byte 26 = 0x27), Variante 0 (Byte 27 = 0x00) */
#define FAHRSTUHL_ARCHIV   0x27
#define FAHRSTUHL_VARIANTE 0

int re15_fahrstuhl_r37_anzahl(void) { return N_AUS; }

int re15_fahrstuhl_r37_satz(int i, uint16_t *raum, uint8_t *ziel_stage, uint8_t *ziel_raum, uint16_t *seite,
                            uint16_t *tuer, uint32_t *off, const char **kennung)
{
    if (i < 0 || i >= N_AUS) return 0;
    const ausstieg_t *s = &k_ausstiege[i];
    if (raum) *raum = s->raum;
    if (ziel_stage) *ziel_stage = s->ziel_stage;
    if (ziel_raum) *ziel_raum = s->ziel_raum;
    if (seite) *seite = s->seite;
    if (tuer) *tuer = s->tuer;
    if (off) *off = s->off;
    if (kennung) *kennung = s->kennung;
    return 1;
}

#ifndef RE15_PLATFORM_PSX
static int eigen_index(const char *kennung)
{
    if (!kennung) return 0;
    for (int i = 1; i <= re15_door_seq_eigen_anzahl(); i++) {
        const re15_tuer_eigen_t *e = re15_door_seq_eigen(i);
        if (e && e->basis == FAHRSTUHL_ARCHIV && strcmp(e->kennung, kennung) == 0) return i;
    }
    return -1;
}
#endif

int re15_fahrstuhl_r37_ausstieg(unsigned room_id, const re15_aot_t *a, const re15_aot_door_params_t *d,
                                re15_door_seq_anfrage_t *out)
{
#ifdef RE15_PLATFORM_PSX
    /* wie door_seq_zuordnung.c: auf der PSX laeuft die Szene nicht (kein Laeufer), keine Anfrage */
    (void)room_id; (void)a; (void)d; (void)out;
    return 0;
#else
    if (!a || !d || !out) return 0;
    if (a->has_quad || a->half_w != 0 || a->half_h != 0) return 0;   /* nur die Null-Rechteck-Saetze */
    for (int i = 0; i < N_AUS; i++) {
        const ausstieg_t *s = &k_ausstiege[i];
        if ((room_id & 0xFFF0u) != s->raum || d->dest_stage != s->ziel_stage || d->dest_room != s->ziel_raum)
            continue;
        int eig = eigen_index(s->kennung);
        if (eig < 0) return 0;              /* Port-Archiv fehlt in der Tabelle -> RE1.5-Uebergang */
        memset(out, 0, sizeof *out);
        out->aktiv    = 1;
        out->archiv   = RE15_DOOR_ARCHIV_RE2;
        out->re2_nr   = FAHRSTUHL_ARCHIV;
        out->tuer_nr  = FAHRSTUHL_ARCHIV;   /* var 15 = Payload+12 = Archivnummer (door_seq_zuordnung.c) */
        out->variante = FAHRSTUHL_VARIANTE;
        out->bit7     = 0;
        out->spender  = RE15_DOOR_KEIN_SPENDER;
        out->seite    = s->seite;
        out->tuer     = s->tuer;
        out->eigen    = (uint8_t)eig;
        return 1;
    }
    return 0;
#endif
}
