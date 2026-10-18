/* probe_r26_inventar.c — RIEGEL fuer die drei Inventar-Aenderungen der Runde 26.
 *
 * TEIL A  BESTAETIGUNGSTON beim Aufnehmen-Ja (belegte RE2-Angleichung).
 *         RE1.5-Satznummern: Ja `lui a0,0x406` @0x8004a51c, Nein `lui a0,0x405`
 *         @0x8004a660 (beide `jal 0x80045024` = Se_on). DASS eine Abfrage-Bestaetigung
 *         toent, ist RE2s Regel FUN_80030844 (Index 0 -> `lui a0,0x406` @0x80030950 /
 *         `jal 0x8005ba28` @0x80030954, Index 1 -> `lui a0,0x405` @0x8003093c), und die
 *         Freigabe traegt RE2s AUFNAHME-Abfrage (SET-B Idx 0, `ori a1,zero,0xe400`
 *         @0x80071ff8 -> `jal 0x8002fe38` @0x80072000).
 *         GEGENPROBEN: der can't-carry-Zweig bleibt stumm (RE2s Gegenstueck SET-B Idx 1
 *         hat gar keine Auswahlbox) und waehrend der Schreibmaschine toent nichts.
 *
 * TEIL B  WEGWERF-TEXT "item" statt "key" fuer Nicht-Schluessel.
 *         ⛔ NUTZER-ENTSCHEIDUNG, nicht byte-true: weder RE1.5 noch RE2 haben eine zweite
 *         Wortlaut-Variante. Geprueft wird gegen die DATEN, nicht gegen eine getippte
 *         Zeichenkette: ein Referenz-Walker im Test spielt die AUSGELIEFERTEN Skripte
 *         (gen/item_prompt_data.inc == DEBUG.BIN @0x800c4fc6) unveraendert ab.
 *
 * TEIL C  MUNITION GLEICHER SORTE STAPELN.
 *         ⛔ NUTZER-ENTSCHEIDUNG. Arithmetik byte-true aus FUN_8004e054 geborgt
 *         (Summe @0x8004e160, cap `lbu` @0x8004e338, `sltu` @0x8004e340,
 *         Ziel = cap @0x8004e3f4 / Rest @0x8004e410, Ziel = Summe @0x8004e398-3a8).
 *         ⛔ DIE DUPLIKATIONS-SUCHE ist der Kern: JEDER Zweig wird mit der Gesamtmenge
 *         VOR und NACH der Aufnahme gemessen (voller Stapel, Teilstapel mit Ueberlauf,
 *         kein vorhandener Platz, kein freier Platz).
 *
 * Aufruf: probe_r26_inventar
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_item_prompt.h"
#include "re15_inv_ui.h"
#include "re15_files.h"      /* Runde 30: Dokument-Ids ab 0x48 (Ausnahme in B4, Pruefung B5) */
#include "re15_schluessel_r36.h" /* Runde 36 Spur Q: Port-Namen der Ids 0x42/0x45 (Ausnahme in B4, Pruefung B4c) */
#include "re15_dining_r38.h"     /* Runde 38 Spur F: Port-Name der Id 0x43 "Dining Room Key" (B3 + B4c wie 0x42/0x45) */

/* Die AUSGELIEFERTEN Prompt-Tabellen als Vergleichsquelle (dieselbe Datei, die die Engine
 * einbindet; erzeugt von tools/gen_item_prompt_data.py aus DEBUG.BIN @0x800c4fc6/@0x800c495c).
 * Damit vergleicht der Riegel gegen die Daten, nicht gegen eine getippte Zeichenkette. */
#include "gen/item_prompt_data.inc"

extern re15_inventory_t g_inv;
extern int g_test_core_se_last, g_test_core_se_count;   /* tests/test_support.c */

static int g_fail = 0;
#define PRUEFE(name, cond) do {                                            \
        int _c = (cond);                                                   \
        printf("  [%s] %s\n", _c ? "OK  " : "FAIL", (name));               \
        if (!_c) g_fail++;                                                 \
    } while (0)

#define PAD_CONFIRM 0x4000u   /* VIRTUELLES Bestaetigen (<- roh SQUARE, @0x80073dbc[14]) */
#define PAD_LR      0x1000u   /* VIRTUELLES Menue-Links/Rechts (Ja/Nein umschalten)      */

/* ------------------------------------------------------------------ Hilfen */

/* Modal bis zum Ende fahren. wahl 0 = Ja, 1 = Nein. */
static void modal_fahren(uint8_t id, uint8_t menge, int wahl)
{
    re15_item_modal_start(id, menge, 0, -1, 0xFF);
    int wache = 0;
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && wache++ < 900)
        re15_item_modal_tick(0, 0);
    if (wahl) re15_item_modal_tick((uint16_t)PAD_LR, 0);     /* auf "Nein" schalten */
    wache = 0;
    while (re15_item_modal_active() && wache++ < 900)
        re15_item_modal_tick((uint16_t)PAD_CONFIRM, 0);
}

/* Gesamtmenge dieser Sorte im Beutel, ueber ALLE Plaetze (nicht nur den ersten). */
static int gesamt(uint8_t id)
{
    int s = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == id) s += (int)g_inv.slots[i].qty;
    return s;
}
static int plaetze(uint8_t id)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) n++;
    return n;
}

/* Obergrenze aus der AUSGELIEFERTEN Tabelle, genau wie das Original sie liest:
 * `lbu v1,0(at)` @0x8004e338 auf 0x80074da8 + Id*12. Keine Zahl im Test. */
static int cap_von(uint8_t id) { return (int)*RE15_INV_PTR(0x80074DA8u + (uint32_t)id * 12u); }

/* Inventar bis auf `frei` freie Plaetze mit Heilmitteln (0x24, nicht stapelbar) fuellen. */
static void gitter_fuellen(int frei)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == 0) {
            if (frei > 0) { frei--; continue; }
            g_inv.slots[i].id = 0x24; g_inv.slots[i].qty = 1; g_inv.slots[i].flags = 0;
        }
}

/* ---------------------------------------------------- TEIL B: Referenz-Walker */
/* Spielt ein AUSGELIEFERTES Prompt-Skript unveraendert ab (dieselben Steuer-Ops wie
 * re15_item_prompt_walk, aber OHNE die Nutzer-Ersetzung). */
static int ref_lauf(int start, uint8_t id, unsigned char *out, int cap)
{
    int n = 0;
    for (int i = start; ; i++) {
        unsigned char c = re15_item_prompt_script_blob[i];
        if (c == 0x01 || c == 0x03) break;
        if (c == 0x02) { i += 2; continue; }
        if (c == 0x05) { i++; continue; }
        if (c == 0x08) continue;                       /* Zeilenumbruch traegt keine Glyphe */
        if (c == 0x06) {                               /* Item-Namen einsetzen */
            i++;
            if ((int)id < RE15_ITEM_PROMPT_NIDS) {
                const unsigned char *nm =
                    re15_item_prompt_name_blob + re15_item_prompt_name_off[id];
                for (; *nm != 0x07; nm++) { if (n < cap) out[n] = *nm; n++; }
            }
            continue;
        }
        if (n < cap) out[n] = c;
        n++;
    }
    return n;
}

typedef struct { unsigned char g[512]; int n; } lauf_t;
static void sammel_cb(void *ctx, unsigned char code, int attr, int nl)
{
    lauf_t *l = (lauf_t *)ctx; (void)attr;
    if (nl) return;
    if (l->n < (int)sizeof l->g) l->g[l->n] = code;
    l->n++;
}
static int lauf_holen(int prompt_key, uint8_t id, lauf_t *out)
{
    out->n = 0;
    return re15_item_prompt_walk(prompt_key, id, (int)sizeof out->g, sammel_cb, out);
}

int main(void)
{
    /* =============================== TEIL A =============================== */
    printf("=== A: Bestaetigungston beim Aufnehmen (belegte RE2-Angleichung) ===\n");
    re15_inv_init();
    {
        int vor = g_test_core_se_count;
        modal_fahren(0x15, 30, 0);                       /* Ja */
        PRUEFE("A1 Ja spielt GENAU EINEN Ton",       g_test_core_se_count == vor + 1);
        PRUEFE("A2 Ja = Bank4/Satz 6 (@0x8004a51c)", g_test_core_se_last == 6);
    }
    re15_inv_init();
    {
        int vor = g_test_core_se_count;
        modal_fahren(0x15, 30, 1);                       /* Nein */
        PRUEFE("A3 Nein spielt GENAU EINEN Ton",       g_test_core_se_count == vor + 1);
        PRUEFE("A4 Nein = Bank4/Satz 5 (@0x8004a660)", g_test_core_se_last == 5);
        PRUEFE("A5 Nein nimmt nichts auf",             gesamt(0x15) == 0);
    }
    /* A6 GEGENPROBE: der can't-carry-Zweig bleibt STUMM (RE2 SET-B Idx 1 hat kein 0xFB). */
    re15_inv_init();
    gitter_fuellen(0);
    {
        int vor = g_test_core_se_count;
        modal_fahren(0x36, 1, 0);                        /* Schluessel, Inventar voll */
        PRUEFE("A6 can't-carry bleibt stumm", g_test_core_se_count == vor);
    }
    /* A7 GEGENPROBE: waehrend der Schreibmaschine toent nichts. */
    re15_inv_init();
    {
        int vor = g_test_core_se_count, w = 0;
        re15_item_modal_start(0x15, 30, 0, -1, 0xFF);
        while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 900)
            re15_item_modal_tick(0, 0);
        PRUEFE("A7 vor der Bestaetigung kein Ton", g_test_core_se_count == vor);
        w = 0;
        while (re15_item_modal_active() && w++ < 900)
            re15_item_modal_tick((uint16_t)PAD_CONFIRM, 0);
    }

    /* =============================== TEIL B =============================== */
    printf("=== B: Wegwerf-Text 'item' statt 'key' (NUTZER-ENTSCHEIDUNG) ===\n");
    /* B0 ANKER GEGEN DIE DATEN: verschiebt sich die ausgelieferte Tabelle, stimmt der
     * Skript-Index 20 nicht mehr. Skript [6] beginnt bei Blob 0xc5 (== @0x800c508b);
     * "key" = 47 41 55 @0x800c509f..a1 (Blob 0xd9..0xdb),
     * "item" = 45 50 41 49 @0x800c500e (Blob 0x48..0x4b, Skript [1] "...more items."). */
    PRUEFE("B0a Skript [6] beginnt bei Blob 0xc5 (@0x800c508b)",
           re15_item_prompt_script_off[6] == 0xc5);
    PRUEFE("B0b Blob 0xd9..0xdb == 'key'  (@0x800c509f)",
           re15_item_prompt_script_blob[0xd9] == 0x47 &&
           re15_item_prompt_script_blob[0xda] == 0x41 &&
           re15_item_prompt_script_blob[0xdb] == 0x55);
    PRUEFE("B0c Blob 0x48..0x4b == 'item' (@0x800c500e)",
           re15_item_prompt_script_blob[0x48] == 0x45 &&
           re15_item_prompt_script_blob[0x49] == 0x50 &&
           re15_item_prompt_script_blob[0x4a] == 0x41 &&
           re15_item_prompt_script_blob[0x4b] == 0x49);

    /* B1 SCHLUESSEL: Skript [6] bit-gleich zum Auslieferungsstand. */
    {
        static const uint8_t schluessel[] = { 0x36, 0x37, 0x38, 0x46, 0x47 };
        unsigned char ref[512];
        int gleich = 1, geprueft = 0;
        for (unsigned k = 0; k < sizeof schluessel; k++) {
            lauf_t l; int n = lauf_holen(8, schluessel[k], &l);
            int m = ref_lauf((int)re15_item_prompt_script_off[6], schluessel[k],
                             ref, (int)sizeof ref);
            if (n != m || memcmp(l.g, ref, (size_t)m) != 0) gleich = 0;
            geprueft++;
        }
        PRUEFE("B1 fuenf Schluessel: Skript [6] bit-gleich (Vergleich gegen den Blob)",
               gleich && geprueft == 5);
    }
    /* B2 NICHT-SCHLUESSEL: genau ein Glyph laenger, Unterschied ausschliesslich an 20..23. */
    {
        static const uint8_t andere[] = { 0x30, 0x31, 0x44 };
        unsigned char ref[512];
        int nref = ref_lauf((int)re15_item_prompt_script_off[6], 0x30, ref, (int)sizeof ref);
        int ok = 1;
        for (unsigned k = 0; k < sizeof andere; k++) {
            lauf_t l; int n = lauf_holen(8, andere[k], &l);
            if (n != nref + 1) { ok = 0; continue; }
            if (memcmp(l.g, ref, 20) != 0) ok = 0;
            if (!(l.g[20] == 0x45 && l.g[21] == 0x50 && l.g[22] == 0x41 && l.g[23] == 0x49)) ok = 0;
            if (memcmp(l.g + 24, ref + 23, (size_t)(nref - 23)) != 0) ok = 0;
        }
        PRUEFE("B2 drei Nicht-Schluessel tragen 'item' an 20..23, sonst unveraendert", ok);
    }
    /* B3 ZENSUS ueber alle 102 ausgelieferten Namen: genau die acht Namen mit "Key"/"key"
     * behalten die Original-Zeile. Die Menge kommt aus dem VERHALTEN, nicht aus einer Liste. */
    {
        /* Runde 38 Spur F: 0x43 (ausgeliefert "Communications Card") ist der Dining Room Key ->
         * re15_dining_r38_ist_schluessel, item_prompt_common.c re15_prompt_name_ist_schluessel. */
        static const uint8_t soll[] = { 0x36, 0x37, 0x38, 0x39, 0x42, 0x43, 0x45, 0x46, 0x47 };
        unsigned char ref[512];
        int nref = ref_lauf((int)re15_item_prompt_script_off[6], 0x00, ref, (int)sizeof ref);
        int gefunden = 0, falsch = 0;
        for (int id = 0; id < re15_item_prompt_name_count(); id++) {
            lauf_t l; int n = lauf_holen(8, (uint8_t)id, &l);
            int ist_key = (n == nref), soll_key = 0;
            for (unsigned k = 0; k < sizeof soll; k++) if (soll[k] == id) soll_key = 1;
            if (ist_key != soll_key) falsch++;
            if (ist_key) gefunden++;
        }
        PRUEFE("B3a genau 9 von 102 Namen behalten 'key' (Runde 38 Spur F: + 0x43)", gefunden == 9);
        PRUEFE("B3b und es sind genau die neun erwarteten Ids", falsch == 0);
    }
    /* B4 KEIN KOLLATERAL: alle uebrigen Skripte sind fuer alle 102 Namen unveraendert.
     * prompt_key -> Skriptindex (item_prompt_common.c prompt_key_to_script).
     * ⛔ AUSNAHME SEIT RUNDE 30 (Thema irons-diary-dokument, mit Beleg): Ids, die die
     * Dokument-Tabelle des Ports fuehrt (re15_files.c, heute nur 0x48 = Irons Diary),
     * nehmen ihren Namen zur Laufzeit aus der Tabelle statt aus RE1.5s Namensbank - dort
     * steht unter 0x48 der vorinstallierte RE1-Name "Chris' Diary" (@0x800c4e04), den der
     * Nutzer-Auftrag entfernt. Diese Ids prueft B5 gesondert; alle anderen bleiben
     * bit-gleich zum Auslieferungsstand. */
    {
        static const int key2script[10] = { 0, 0, 1, 0, 4, 2, 3, 5, 6, 7 };
        unsigned char ref[512];
        int ok = 1, gefahren = 0, dokumente = 0, port_namen = 0, port_ok = 1, port_einsatz = 0;
        /* Runde 38 Spur G (s. B4 unten): die Dokument-Basis kommt aus den ORIGINAL-Bytes, nicht aus
         * dem Engine-Makro RE15_FILES_FIRST_ITEM_ID, das hier mitgeprueft wird - DEBUG.BIN (Modul
         * @0x800c0000) Datei 0x7370 = u8 @0x800c7370, gelesen @0x800c72dc `addiu v1,v1,0x7370` /
         * @0x800c72f8 `lbu s1,0(v1)`. */
        int basis = -1, gefahren_unter_basis = 0, dokumente_unter_basis = 0;
        {
            FILE *f = fopen(RE15_ASSET_PSX_DIR "/BIN/DEBUG.BIN", "rb");
            if (f) { if (fseek(f, 0x7370, SEEK_SET) == 0) basis = fgetc(f); fclose(f); }
        }
        for (int pk = 0; pk <= 9; pk++) {
            if (key2script[pk] == 6) continue;                 /* Skript [6] = B1/B2 */
            for (int id = 0; id < re15_item_prompt_name_count(); id++) {
                if (re15_files_doc_from_item(id) >= 0) {       /* -> B5 */
                    if (id < basis) dokumente_unter_basis++;
                    dokumente++; continue;
                }
                /* Runde 36 Spur Q: 0x42/0x45 tragen den Port-Namen (Prison Key / Parking Lot Key,
                 * re15_schluessel_r36.h) statt "Key Disc"/"Water Key". B4c: setzt das Skript einen
                 * Namen ein (der Soll-Lauf mit dem AUSGELIEFERTEN Namen enthaelt ihn), steht im Lauf
                 * der Port-Name und nicht mehr der ausgelieferte; sonst ist der Lauf bit-gleich. */
                if (re15_schluessel_r36_name((uint8_t)id) || re15_dining_r38_name((uint8_t)id)) {   /* + 0x43 Runde 38 Spur F */
                    const uint8_t *pn = re15_schluessel_r36_name((uint8_t)id);
                    if (!pn) pn = re15_dining_r38_name((uint8_t)id);
                    const uint8_t *an = re15_item_prompt_name_blob + re15_item_prompt_name_off[id];
                    int pl = 0, al = 0; while (pn[pl] != 0x07) pl++; while (an[al] != 0x07) al++;
                    lauf_t lq; int nq = lauf_holen(pk, (uint8_t)id, &lq);
                    int mq = ref_lauf((int)re15_item_prompt_script_off[key2script[pk]], (uint8_t)id,
                                      ref, (int)sizeof ref);
                    int ref_hat = 0, port_hat = 0, alt_hat = 0;
                    for (int i = 0; i + al <= mq; i++) if (memcmp(ref + i, an, (size_t)al) == 0) ref_hat = 1;
                    for (int i = 0; i + pl <= nq; i++) if (memcmp(lq.g + i, pn, (size_t)pl) == 0) port_hat = 1;
                    for (int i = 0; i + al <= nq; i++) if (memcmp(lq.g + i, an, (size_t)al) == 0) alt_hat = 1;
                    if (ref_hat ? (!port_hat || alt_hat || nq != mq - al + pl)
                                : (nq != mq || memcmp(lq.g, ref, (size_t)mq) != 0)) port_ok = 0;
                    if (ref_hat) port_einsatz++;
                    port_namen++; continue;
                }
                lauf_t l; int n = lauf_holen(pk, (uint8_t)id, &l);
                int m = ref_lauf((int)re15_item_prompt_script_off[key2script[pk]],
                                 (uint8_t)id, ref, (int)sizeof ref);
                if (n != m || memcmp(l.g, ref, (size_t)m) != 0) ok = 0;
                gefahren++;
                if (id < basis) gefahren_unter_basis++;
            }
        }
        printf("  (B4 Vergleiche gefahren: %d, Dokument-Ids ausgenommen: %d)\n",
               gefahren, dokumente);
        printf("  (B4 Dokument-Basis aus DEBUG.BIN @0x800c7370: 0x%02X; Nicht-Dokument-Ids darunter: "
               "%d gefahren, %d als Dokument geschluckt)\n", basis, gefahren_unter_basis,
               dokumente_unter_basis);
        /* Runde 34 Nacht (Spur E): die Dokument-Tabelle fuehrt jetzt 0x48..0x4C (Irons Diary +
         * vier Dokumente, re15_files.c) — ausgenommen werden genau ihre Ids, alle anderen
         * (102 - Tabellengroesse) werden je Schluessel verglichen. */
        /* RUNDE 38 SPUR G (Nachbesserung 1, Mangel 2 der Abnahme 0): die feste Untergrenze
         * "gefahren >= 850" stammt aus Runde 34 Nacht (damals 9 x (102 - 5) = 873, Luft fuer
         * hoechstens 7 Dokumente); seit Runde 36 Q (- 2 fuer 0x42/0x45) liess sie nur noch 5
         * Dokumente zu (9 x 95 = 855). Dokument 5 "Roys Diary" (Item 0x4D = 0x48 + 5,
         * re15_files.c) macht 9 x (102 - 6 - 2) = 846 -> rot, obwohl die Tabelle nur eine
         * RE1.5-Dokument-Id mehr fuehrt. Der Zweck der Grenze — die Tabelle darf keine
         * Nicht-Dokument-Id schlucken — wird jetzt GENAU geprueft statt ueber die Tabellengroesse:
         *   RE1.5s Item-Ids enden bei 0x47: PSX.EXE @0x8004a350 `sltiu v0,v0,0x48`,
         *   @0x8004a358 `ori v0,zero,0x47` (Klemme); die FILE-Zeilen beginnen bei 0x48:
         *   DEBUG.BIN @0x800c7370 (Datei 0x7370) = `48 52 5c` (Basis je FILE-Seite), der
         *   Zeilenmaler @0x800c72f8 `lbu s1,0(v1)` -> @0x800c7320 `jal 0x80028840` (Item-Name,
         *   u16 @0x800c495c) mit a0 = s1, @0x800c7350 `addiu s1,s1,1` je Zeile (10 Zeilen,
         *   @0x800c7344 `ori v0,zero,0xa`); die Namensbank (gen/item_prompt_data.inc) traegt
         *   0x47 "Blue Master Keycard", 0x48 "Chris' Diary", 0x49 "Operation Report",
         *   0x4A..0x4F "File 3".."File 8".
         * Die Basis liest der Riegel aus DEBUG.BIN selbst (oben), nicht aus RE15_FILES_FIRST_ITEM_ID:
         * eine verschobene Basis im Engine-Makro wanderte sonst mit dem Riegel mit. Jede Id UNTER
         * der Basis — ausser den Port-Namen 0x42/0x45 (+ 0x43 Runde 38 Spur F) aus B4c — muss je Schluessel verglichen
         * sein: 9 x (0x48 - 3) = 621, unabhaengig davon, wie viele RE1.5-Dokument-Ids (0x48..) die
         * Tabelle fuehrt. Die Gleichung ueber alle 102 Ids bleibt. */
        const int n_dok = re15_files_doc_count();
        PRUEFE("B4 kein Kollateral in den uebrigen Skripten",
               ok && gefahren == 9 * (re15_item_prompt_name_count() - n_dok - 3) &&
               basis == 0x48 && gefahren_unter_basis == 9 * (basis - 3) && dokumente_unter_basis == 0);
        /* Integration Runde 38: G (genaue Pruefung unter der Basis) + F (dritter Port-Name 0x43 < 0x48). */
        printf("  (B4c Laeufe 0x42/0x43/0x45: %d, davon mit Namens-Einsatz: %d)\n", port_namen, port_einsatz);
        PRUEFE("B4c 0x42/0x45 (Runde 36 Spur Q) + 0x43 (Runde 38 Spur F): Port-Name statt ausgeliefertem Namen, sonst bit-gleich",
               port_ok && port_namen == 9 * 3 && port_einsatz >= 3 * 2);
        PRUEFE("B4b genau die Dokument-Ids der Tabelle ausgenommen (0x48..), je Schluessel einmal",
               dokumente == 9 * n_dok);
    }
    /* B5 DOKUMENT-NAME (Runde 30): Skript [5] "The <name> has been filed." (@0x800c506f,
     * Schluessel 7) traegt fuer 0x48 den Namen "Irons Diary" aus der Dokument-Tabelle; der
     * Rest des Laufs ist bit-gleich zum ausgelieferten Skript. Der Soll-Lauf wird aus den
     * DATEN gebaut: Skript [5] abspielen und am Namens-Opcode 0x06 den Tabellennamen
     * einsetzen (dieselben Steuer-Ops wie ref_lauf). */
    {
        const re15_file_doc_t *d = re15_files_doc(re15_files_doc_from_item(0x48));
        unsigned char soll[512];
        int ns = 0, ok = (d != NULL);
        for (int i = (int)re15_item_prompt_script_off[5]; ok; i++) {
            unsigned char c = re15_item_prompt_script_blob[i];
            if (c == 0x01 || c == 0x03) break;
            if (c == 0x02) { i += 2; continue; }
            if (c == 0x05) { i++; continue; }
            if (c == 0x08) continue;
            if (c == 0x06) {
                i++;
                for (const uint8_t *nm = d->name; *nm != 0x07; nm++) soll[ns++] = *nm;
                continue;
            }
            soll[ns++] = c;
        }
        lauf_t l; int n = lauf_holen(7, 0x48, &l);
        static const uint8_t irons[11] = { 0x25,0x4e,0x4b,0x4a,0x4f,0x00,0x20,0x45,0x3d,0x4e,0x55 };
        int hat_irons = 0, hat_chris = 0;
        for (int i = 0; i + 11 <= n; i++) if (memcmp(l.g + i, irons, 11) == 0) hat_irons = 1;
        for (int i = 0; i + 5 <= n; i++)                        /* "Chris" 1f 44 4e 45 4f */
            if (l.g[i] == 0x1f && l.g[i+1] == 0x44 && l.g[i+2] == 0x4e && l.g[i+3] == 0x45 &&
                l.g[i+4] == 0x4f) hat_chris = 1;
        printf("  (B5 Lauf 0x48 Skript [5]: %d Glyphen, Soll %d)\n", n, ns);
        PRUEFE("B5a 0x48 in \"has been filed\": Lauf == Skript [5] mit Tabellennamen",
               ok && n == ns && memcmp(l.g, soll, (size_t)ns) == 0);
        PRUEFE("B5b der Lauf traegt \"Irons Diary\" und NICHT \"Chris' Diary\"",
               hat_irons && !hat_chris);
    }

    /* =============================== TEIL C =============================== */
    printf("=== C: Munition gleicher Sorte stapeln (NUTZER-ENTSCHEIDUNG) ===\n");
    const int cap15 = cap_von(0x15);          /* @0x80074da8 + 0x15*12, `lbu` @0x8004e338 */
    const int cap18 = cap_von(0x18);          /* FLAME FUEL — andere Obergrenze            */
    printf("  cap(0x15) = %d, cap(0x18) = %d   (aus der ausgelieferten Tabelle @0x80074da8)\n",
           cap15, cap18);
    PRUEFE("C0 die beiden Obergrenzen sind verschieden (die Tabelle wird wirklich gelesen)",
           cap15 != cap18 && cap15 > 0 && cap18 > 0);

    /* C1 VOLLER STAPEL: zweimal dieselbe Sorte -> EIN Platz, Menge genau einmal. */
    re15_inv_init();
    {
        modal_fahren(0x15, 30, 0);
        int vor = gesamt(0x15), p1 = plaetze(0x15);
        modal_fahren(0x15, 30, 0);
        PRUEFE("C1a erster Aufsammler legt einen Platz an", p1 == 1 && vor == 15);
        PRUEFE("C1b zweiter stapelt auf denselben Platz",   plaetze(0x15) == 1);
        PRUEFE("C1c Menge GENAU EINMAL gutgeschrieben",     gesamt(0x15) == vor + 15);
    }
    /* C2 TEILSTAPEL MIT UEBERLAUF: Ziel = Obergrenze, Rest auf einen neuen Platz. */
    re15_inv_init();
    {
        g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = (uint8_t)(cap15 - 5);
        int vor = gesamt(0x15);
        modal_fahren(0x15, 30, 0);                            /* halbiert -> 15 */
        PRUEFE("C2a Zielplatz steht auf der Obergrenze (@0x8004e3f4)",
               (int)g_inv.slots[0].qty == cap15);
        PRUEFE("C2b Rest liegt auf einem ZWEITEN Platz (@0x8004e410)", plaetze(0x15) == 2);
        PRUEFE("C2c Gesamtmenge = vorher + 15, keine Duplikation", gesamt(0x15) == vor + 15);
    }
    /* C3 KEIN VORHANDENER PLATZ: byte-true Insert, neuer Platz, Menge einmal. */
    re15_inv_init();
    {
        modal_fahren(0x16, 12, 0);
        PRUEFE("C3 ohne vorhandenen Platz genau ein neuer Platz mit 6",
               plaetze(0x16) == 1 && gesamt(0x16) == 6);
    }
    /* C4 VOLLES GITTER, aber Luft auf dem vorhandenen Platz -> stapeln statt ablehnen. */
    re15_inv_init();
    {
        g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = 20;
        gitter_fuellen(0);                                    /* kein freier Platz mehr */
        int vor = gesamt(0x15), pv = plaetze(0x15);
        modal_fahren(0x15, 30, 0);
        PRUEFE("C4a volles Gitter: gestapelt statt abgelehnt", gesamt(0x15) == vor + 15);
        PRUEFE("C4b und kein zusaetzlicher Platz",             plaetze(0x15) == pv);
    }
    /* C5 ⛔ DER DUPLIKATIONS-FALL: volles Gitter UND der vorhandene Platz laeuft ueber.
     * Kein Platz fuer den Rest -> die Aufnahme muss VOLLSTAENDIG scheitern; ein
     * Teilbetrag waere unbegrenzte Munition (die Packung bliebe in der Welt liegen). */
    re15_inv_init();
    {
        g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = (uint8_t)(cap15 - 5);
        gitter_fuellen(0);
        int vor = gesamt(0x15);
        modal_fahren(0x15, 30, 0);                            /* 15 > 5 Luft */
        PRUEFE("C5a Gesamtmenge UNVERAENDERT (keine Teilgutschrift)", gesamt(0x15) == vor);
        PRUEFE("C5b Zielplatz unveraendert", (int)g_inv.slots[0].qty == cap15 - 5);
    }
    /* C6 VOLLES GITTER, vorhandener Platz AUF der Obergrenze -> abgelehnt, nichts bewegt. */
    re15_inv_init();
    {
        g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = (uint8_t)cap15;
        gitter_fuellen(0);
        int vor = gesamt(0x15);
        modal_fahren(0x15, 30, 0);
        PRUEFE("C6 voller Stapel + volles Gitter: nichts gutgeschrieben", gesamt(0x15) == vor);
    }
    /* C7 MEMORY CARD (0x21): liegt zwar im Id-Fenster 0x15..0x21, traegt in der
     * ausgelieferten Tabelle aber den Null-Satz @0x80074c88 im Feld +4 — keine Munition.
     * Runde 39 Spur C (NUTZER-VORGABE "alles mit einer Zahl ... muss stapelbar sein"): die
     * Aufnahme stapelt sie trotzdem (re15_inventar_r39_stapelt, Form RE2 Ink Ribbon
     * @0x800A9DB8 `1e 05 1e ff`); der byte-true Helfer re15_pickup_stapelt_nutzer bleibt 0. */
    re15_inv_init();
    {
        modal_fahren(0x21, 2, 0);
        modal_fahren(0x21, 2, 0);
        int n = 0;
        for (int i = 0x15; i <= 0x20; i++) n += re15_pickup_stapelt_nutzer((uint8_t)i);
        PRUEFE("C7a Memory Card stapelt (EIN Platz, 2 + 2 = 4; Runde 39 Spur C)",
               plaetze(0x21) == 1 && gesamt(0x21) == 4);
        PRUEFE("C7b re15_pickup_stapelt_nutzer(0x21) == 0",
               re15_pickup_stapelt_nutzer(0x21) == 0);
        PRUEFE("C7c aber alle zwoelf Sorten 0x15..0x20 stapeln", n == 12);
    }
    /* C8 NICHT-MUNITION stapelt nicht (Heilmittel 0x24). */
    re15_inv_init();
    {
        modal_fahren(0x24, 1, 0);
        modal_fahren(0x24, 1, 0);
        PRUEFE("C8 Heilmittel 0x24 stapelt nicht", plaetze(0x24) == 2);
    }
    /* C9 zweite Sorte mit ANDERER Obergrenze (FLAME FUEL 0x18) — die Grenze kommt aus der
     * Tabelle, nicht aus einer Zahl im Code. */
    re15_inv_init();
    {
        g_inv.slots[0].id = 0x18; g_inv.slots[0].qty = (uint8_t)(cap18 - 3);
        int vor = gesamt(0x18);
        modal_fahren(0x18, 30, 0);                            /* halbiert -> 15 */
        PRUEFE("C9a 0x18 klemmt auf SEINER Obergrenze", (int)g_inv.slots[0].qty == cap18);
        PRUEFE("C9b Gesamtmenge = vorher + 15",         gesamt(0x18) == vor + 15);
    }

    printf("\n=== probe_r26_inventar: %s (%d Fehler) ===\n", g_fail ? "ROT" : "GRUEN", g_fail);
    return g_fail ? 1 : 0;
}
