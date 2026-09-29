/* door_seq_zuordnung.c — welche RE1.5-Tuerseite spielt welche RE2-Tuersequenz (Runde 31).
 *
 * ⛔ RE2-ERGAENZUNG (Beta -> Retail), Begruendung im Kopf von include/re15_door_seq.h: RE1.5 hat
 * die Tuermaschine, aber nur DOOR00 mit dem einzigen Skript `Evt_end`, und alle echten Tuersaetze
 * tragen Archiv 0 / Variante 0 (Payload+12/+13). Ziel ist RE2 Retail.
 *
 * ⛔ PORT-WAHL: die Zuordnung Seite -> Archiv/Variante/Griff-Tausch stammt aus dem Bildvergleich
 * der Stufe 3 (analysis/befunde_runde31/tueren_03_zuordnung.md, zuordnung.json), nicht aus einer
 * Original-Adresse. Die Tabelle erzeugt tools/tueren/tuer_zuordnung_gen.py (je Zeile Seite, Tuer,
 * RDT-Offset des Door_aot_set und Herkunft der Variante im Kommentar).
 *
 * Schluessel = Raum (volle Id, beide Spielervarianten) + Flaeche + Band - NICHT der Slot
 * (analysis/befunde_runde31/tueren_02_re2.md 6.2). Das Tor ROOM1170/1171 bleibt in
 * door_seq_tor1170.c und hat Vorrang.
 *
 * Die RE2-Archivtabelle @0x8009a520 (Tonteil, Modellteil, Sektor) liegt in gen/re2_tuer_tabelle.inc
 * (tools/tueren/re2_tuer_tabelle.py, 55/55 gegen die Dateien geprueft).
 *
 * Runde 33 (analysis/befunde_runde33/tueren_rest_plan.md): dazu PORT-EIGENE Archive im RE2-Aufbau
 * (gen/re15_tuer_eigen.inc, tools/tueren/tuer_archiv_bauen.py) mit ihren Seitenzeilen; eine Zeile
 * mit eigen != 0 spielt das Port-Archiv, re2_nr ist dann dessen Basis-Archiv (var 15, Griff-Tausch).
 *
 * PSX: die Szene laeuft dort nicht (kein Laeufer, die Anfrage verfiele) - die Tabellen werden dort
 * nicht gelinkt, re15_door_seq_zuordnen_flaeche liefert fuer RE2-Seiten nichts.
 */
#include "re15_door_seq.h"

#include <stddef.h>
#include <string.h>

#ifndef RE15_PLATFORM_PSX
#include "gen/tuer_zuordnung.inc"   /* re15_tuer_zeilen[], re15_griff_tausche[] */
#include "gen/re2_tuer_tabelle.inc" /* re2_tuer_arch[55] */
#include "gen/re15_tuer_eigen.inc"  /* Runde 33: re15_tuer_eigen[], re15_tuer_zeilen_eigen[] */
#define N_ZEILEN_RE2 ((int)(sizeof re15_tuer_zeilen / sizeof re15_tuer_zeilen[0]))
#define N_ZEILEN_EIG ((int)(sizeof re15_tuer_zeilen_eigen / sizeof re15_tuer_zeilen_eigen[0]))
#define N_ZEILEN (N_ZEILEN_RE2 + N_ZEILEN_EIG)
#define N_EIGEN  ((int)(sizeof re15_tuer_eigen / sizeof re15_tuer_eigen[0]))
#define N_TAUSCH ((int)(sizeof re15_griff_tausche / sizeof re15_griff_tausche[0]))
#define N_TAUSCH_EIG ((int)(sizeof re15_griff_tausche_eigen / sizeof re15_griff_tausche_eigen[0]))
#define N_ARCH   ((int)(sizeof re2_tuer_arch / sizeof re2_tuer_arch[0]))
#else
#define N_ZEILEN 0
#define N_EIGEN  0
#endif

int re15_door_seq_zeilen(void) { return N_ZEILEN; }

const re15_tuer_zeile_t *re15_door_seq_zeile(int i)
{
#ifndef RE15_PLATFORM_PSX
    if (i >= 0 && i < N_ZEILEN_RE2) return &re15_tuer_zeilen[i];
    if (i >= N_ZEILEN_RE2 && i < N_ZEILEN) return &re15_tuer_zeilen_eigen[i - N_ZEILEN_RE2];
#else
    (void)i;
#endif
    return NULL;
}

int re15_door_seq_eigen_anzahl(void) { return N_EIGEN; }

const re15_tuer_eigen_t *re15_door_seq_eigen(int eigen)
{
#ifndef RE15_PLATFORM_PSX
    if (eigen >= 1 && eigen <= N_EIGEN) return &re15_tuer_eigen[eigen - 1];
#else
    (void)eigen;
#endif
    return NULL;
}

static void anfrage_aus_zeile(const re15_tuer_zeile_t *t, re15_door_seq_anfrage_t *out)
{
    memset(out, 0, sizeof *out);
    out->archiv   = RE15_DOOR_ARCHIV_RE2;
    out->re2_nr   = t->re2_nr;
    out->variante = t->variante;
    out->bit7     = t->bit7;
    /* var 15 = Payload+12 = Tuertyp = Archivnummer (Lader @0x80015088 lbu v1,12(v0) waehlt damit
     * die Zeile @0x8009a520; Door_init @0x80013e90/98 legt dasselbe Byte in var 15) */
    out->tuer_nr  = t->re2_nr;
    out->spender  = t->spender;
    out->seite    = t->seite;
    out->tuer     = t->tuer;
    out->eigen    = t->eigen;   /* Runde 33: Port-Archiv (0 = RE2-Datei) */
}

/* RE2-Tabelle (ohne Tor). Rueckgabe RE15_DOOR_ARCHIV_RE2 oder _KEINS. */
int re15_door_seq_zuordnen_re2(unsigned room_id, int viereck, int32_t x, int32_t z,
                               int32_t half_w, int32_t half_h,
                               const int16_t qx[4], const int16_t qz[4], int band,
                               re15_door_seq_anfrage_t *out)
{
    for (int i = 0; i < N_ZEILEN; i++) {
        const re15_tuer_zeile_t *t = re15_door_seq_zeile(i);
        if (t->raum != room_id || t->band != band) continue;
        if (viereck) {
            if (t->form != 1 || !qx || !qz) continue;
            if (memcmp(t->qx, qx, sizeof t->qx) != 0 || memcmp(t->qz, qz, sizeof t->qz) != 0) continue;
        } else {
            if (t->form != 0 || t->x != x || t->z != z || t->hw != half_w || t->hh != half_h) continue;
        }
        if (out) anfrage_aus_zeile(t, out);
        return RE15_DOOR_ARCHIV_RE2;
    }
    return RE15_DOOR_ARCHIV_KEINS;
}

int re15_door_seq_anfrage_fuer_seite(int seite, re15_door_seq_anfrage_t *out, unsigned *raum)
{
    for (int i = 0; i < N_ZEILEN; i++) {
        const re15_tuer_zeile_t *t = re15_door_seq_zeile(i);
        if ((int)t->seite != seite) continue;
        if (out) anfrage_aus_zeile(t, out);
        if (raum) *raum = t->raum;
        return 1;
    }
    return 0;
}

const re15_griff_tausch_t *re15_door_seq_griff_tausch(int archiv, int spender)
{
#ifndef RE15_PLATFORM_PSX
    for (int i = 0; i < N_TAUSCH; i++)
        if (re15_griff_tausche[i].archiv == archiv && re15_griff_tausche[i].spender == spender)
            return &re15_griff_tausche[i];
    /* Runde 33: Griff-Tausch der Port-Archive (gen/re15_tuer_eigen.inc) */
    for (int i = 0; i < N_TAUSCH_EIG; i++)
        if (re15_griff_tausche_eigen[i].archiv == archiv && re15_griff_tausche_eigen[i].spender == spender)
            return &re15_griff_tausche_eigen[i];
#else
    (void)archiv; (void)spender;
#endif
    return NULL;
}

int re15_door_seq_re2_archiv(int nr, int *ton, int *modell, int *sektor, int *datei)
{
#ifndef RE15_PLATFORM_PSX
    if (nr < 0 || nr >= N_ARCH) return -1;
    const re2_tuer_arch_t *a = &re2_tuer_arch[nr];
    if (ton)    *ton    = a->ton;       /* FUN_80014cd0 @0x80014d94 lhu s4,0(s1)           */
    if (modell) *modell = a->modell;    /* FUN_80015064 @0x800150f4 lhu v1,2(v1)           */
    if (sektor) *sektor = (int)a->sektor;   /* @0x800150b0 lw t2,4(v1): Modellteil = Sektor*0x800 */
    if (datei)  *datei  = (int)a->datei;
    return 0;
#else
    (void)nr; (void)ton; (void)modell; (void)sektor; (void)datei;
    return -1;
#endif
}
