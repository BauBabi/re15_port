/* ultimate15.c — Runde 41 Optionen: Port-Option "ULTIMATE 1.5" (Zustand, Zuordnung, Puffer, Prop-Zensus).
 * Belege und Zuordnungsregeln: include/re15_ultimate15.h. Kein SDL, keine Datei-Ein-/Ausgabe ausser getenv. */
#include "re15_ultimate15.h"
#include "re15_scd.h"   /* scd_opcode_size_at — dieselbe Laengentabelle wie der VM-Vorschub */

#include <stdlib.h>
#include <string.h>

static int      s_an  = 0;   /* Standard AUS (Nutzer-Vorgabe) */
static unsigned s_gen = 0;

int      re15_ultimate15(void)            { return s_an; }
unsigned re15_ultimate15_generation(void) { return s_gen; }
void     re15_ultimate15_set(int an)
{
    an = an ? 1 : 0;
    if (an != s_an) { s_an = an; s_gen++; }
}

int re15_ultimate15_parse(const char *text)
{
    int wert = -1;
    const char *p = text;
    if (!p) return -1;
    while (*p) {
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (!*p) break;
        if (!strncmp(p, RE15_U15_SCHLUESSEL, sizeof RE15_U15_SCHLUESSEL - 1)) {
            const char *w = p + sizeof RE15_U15_SCHLUESSEL - 1;
            if (!strncmp(w, "on", 2) || *w == '1') wert = 1;
            else if (!strncmp(w, "off", 3) || *w == '0') wert = 0;
        }
        while (*p && *p != '\n') p++;
    }
    return wert;
}

void re15_ultimate15_aus_text(const char *text)
{
    int w = re15_ultimate15_parse(text);
    const char *e = getenv(RE15_U15_ENV);
    if (e && *e) w = (!strcmp(e, "1") || !strcmp(e, "on") || !strcmp(e, "ON")) ? 1 : 0;
    re15_ultimate15_set(w == 1);
}

/* ---- Zuordnung ------------------------------------------------------------------------------------------------
 * ICON (ITEMALL): gleiche Bedeutung 0x01..0x13, 0x15..0x2E, 0x36..0x41, 0x44, 0x46, 0x47; Umzug 0x31 <- 0x14.
 * Ausgenommen (Begruendung re15_ultimate15.h): 0x14, 0x2F 0x30 0x32 0x35 0x42 0x45 (Disc = Browning-Kopie),
 * 0x33 0x34 0x43 (Id heute anders belegt). 0x00 = leere Zelle, bitgleich. */
int re15_ultimate15_icon_quelle(unsigned id)
{
    if (id == 0x31u) return 0x14;                              /* Feuerloescher: Disc-Id 0x14 */
    if (id >= 0x01u && id <= 0x13u) return (int)id;
    if (id >= 0x15u && id <= 0x2Eu) return (int)id;
    if (id >= 0x36u && id <= 0x41u) return (int)id;
    if (id == 0x44u || id == 0x46u || id == 0x47u) return (int)id;
    return -1;
}

/* BILD (ITPS 112x72): die Disc hat nur sechs gegenstandseigene Bilder, der Rest ist die BOSS-Karte von Block 0. */
int re15_ultimate15_bild_quelle(unsigned id)
{
    switch (id) {
        case 0x08u: case 0x09u: case 0x15u: case 0x16u: case 0x36u: return (int)id;
        case 0x3Fu: return 0x3E;                               /* Taschenuhr: Disc-Block 0x3E */
        default:    return -1;
    }
}

/* BREITBILD (ITPS +0x21A0, 80x30): nur die 2-Feld-Waffen 0x0e..0x13 benutzen es (re15_kiste2feld_r37.h). */
int re15_ultimate15_breit_quelle(unsigned id)
{
    return (id >= 0x0Eu && id <= 0x13u) ? (int)id : -1;
}

/* WELT: die einzigen Gegenstaende, fuer die die Disc ein eigenes Weltmodell an einem Item_aot_set hat UND das sich
 * vom heutigen unterscheidet (Zensus ueber alle 96 Disc-RDT, BERICHT_r1.md). Prop-Index = Disc-tk_prop. */
int re15_ultimate15_modell_quelle(unsigned item, const char **rdt_rel, int *prop)
{
    const char *r = NULL; int p = -1;
    switch (item) {
        case 0x15u: r = RE15_U15_DISC_DIR "/STAGE1/ROOM1020.RDT"; p = 0; break;   /* H. Gun Bullets   */
        case 0x16u: r = RE15_U15_DISC_DIR "/STAGE4/ROOM4010.RDT"; p = 3; break;   /* Shotgun Shells   */
        case 0x22u: r = RE15_U15_DISC_DIR "/STAGE4/ROOM4010.RDT"; p = 2; break;   /* First Aid Spray  */
        case 0x30u: r = RE15_U15_DISC_DIR "/STAGE1/ROOM11B0.RDT"; p = 1; break;   /* Pliers           */
        default: return 0;
    }
    if (rdt_rel) *rdt_rel = r;
    if (prop) *prop = p;
    return 1;
}

/* ---- Puffer ---------------------------------------------------------------------------------------------------- */
int re15_ultimate15_itemall_bauen(uint8_t *dst, const uint8_t *basis, int n, const uint8_t *disc, int dn)
{
    int k = 0;
    if (!dst || !basis || n < RE15_U15_KACHELN * RE15_U15_KACHEL_B) return -1;
    memcpy(dst, basis, (size_t)n);
    if (!disc) return 0;
    for (unsigned id = 0; id < RE15_U15_KACHELN; id++) {
        int q = re15_ultimate15_icon_quelle(id);
        if (q < 0 || (q + 1) * RE15_U15_KACHEL_B > dn) continue;
        memcpy(dst + id * RE15_U15_KACHEL_B, disc + q * RE15_U15_KACHEL_B, RE15_U15_KACHEL_B);
        k++;
    }
    return k;
}

int re15_ultimate15_mixitem_bauen(uint8_t *dst, const uint8_t *basis, int n, const uint8_t *disc, int dn)
{
    /* Bild k == ITEMALL-Kachel des Misch-Ergebnisses (0x27..0x2E, 0x09..0x0B, 0x19..0x1B) in Disc UND Mod — alle
     * Ergebnis-Ids stehen in der Icon-Zuordnung, also alle 14 Bilder von der Disc. */
    int k = 0;
    if (!dst || !basis || n <= 0) return -1;
    memcpy(dst, basis, (size_t)n);
    if (!disc) return 0;
    for (int i = 0; i < RE15_U15_MIX_KACHELN; i++) {
        if ((i + 1) * RE15_U15_KACHEL_B > n || (i + 1) * RE15_U15_KACHEL_B > dn) break;
        memcpy(dst + i * RE15_U15_KACHEL_B, disc + i * RE15_U15_KACHEL_B, RE15_U15_KACHEL_B);
        k++;
    }
    return k;
}

int re15_ultimate15_itps_bauen(uint8_t *dst, const uint8_t *basis, int n, const uint8_t *disc, int dn)
{
    int k = 0;
    if (!dst || !basis || n < RE15_U15_ITPS_BLOCK) return -1;
    memcpy(dst, basis, (size_t)n);
    if (!disc) return 0;
    for (unsigned id = 0; (int)((id + 1) * RE15_U15_ITPS_BLOCK) <= n; id++) {
        uint8_t *b = dst + id * RE15_U15_ITPS_BLOCK;
        int q = re15_ultimate15_bild_quelle(id);
        if (q >= 0 && (q + 1) * RE15_U15_ITPS_BLOCK <= dn) {
            const uint8_t *s = disc + q * RE15_U15_ITPS_BLOCK;
            memcpy(b + RE15_U15_ITPS_CLUT, s + RE15_U15_ITPS_CLUT, RE15_U15_ITPS_CLUT_N);
            memcpy(b + RE15_U15_ITPS_BILD, s + RE15_U15_ITPS_BILD, RE15_U15_ITPS_BILD_N);
            k++;
        }
        q = re15_ultimate15_breit_quelle(id);
        if (q >= 0 && (q + 1) * RE15_U15_ITPS_BLOCK <= dn) {
            const uint8_t *s = disc + q * RE15_U15_ITPS_BLOCK;
            memcpy(b + RE15_U15_ITPS_BREIT, s + RE15_U15_ITPS_BREIT, RE15_U15_ITPS_BREIT_N);
            k++;
        }
    }
    return k;
}

/* ---- Prop-Zensus ------------------------------------------------------------------------------------------------ */
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static unsigned rd16(const uint8_t *p) { return (unsigned)p[0] | ((unsigned)p[1] << 8); }

/* Ende einer SCD-Sektion: kleinster Kopfzeiger 0x40..0x5C hinter dem groessten Tabellen-Offset (scd_dump_room.py
 * rdt_section_end). */
static int sektion_ende(const uint8_t *d, int size, uint32_t s)
{
    unsigned first = rd16(d + s), mx = 0;
    uint32_t best = (uint32_t)size;
    if (first >= 2 && !(first & 1) && s + first <= (uint32_t)size)
        for (unsigned i = 0; i < first / 2; i++) {
            unsigned o = rd16(d + s + 2 * i);
            if (o && s + o < (uint32_t)size && o > mx) mx = o;
        }
    for (uint32_t h = 0x40; h < 0x60; h += 4) {
        uint32_t v = rd32(d + h);
        if (v > s + mx && v <= (uint32_t)size && v < best) best = v;
    }
    return (int)best;
}

static int region_ablaufen(const uint8_t *d, int a, int e, uint8_t *out, int n)
{
    int k = 0, pc = a;
    while (pc < e) {
        int sz = scd_opcode_size_at(d + pc);
        if (sz <= 0 || pc + sz > e) break;            /* unbekannter Opcode / Ueberlauf: Walk endet (kein Befund) */
        if (d[pc] == 0x50) {
            int lang = (d[pc + 3] & 0x80) != 0;
            uint8_t item = lang ? d[pc + 22] : d[pc + 14];
            uint8_t prop = lang ? d[pc + 28] : d[pc + 20];
            if (prop < n) out[prop] = item;
            k++;
        }
        pc += sz;
    }
    return k;
}

int re15_ultimate15_prop_items(const uint8_t *d, int size, uint8_t *out, int n)
{
    int k = 0;
    if (out) memset(out, 0xFF, (size_t)n);
    if (!d || !out || size < 0x60) return 0;
    const uint32_t sek[2] = { rd32(d + 0x40), rd32(d + 0x44) };
    for (int si = 0; si < 2; si++) {
        uint32_t s = sek[si];
        if (s == 0 || s + 2 > (uint32_t)size) continue;
        int se = sektion_ende(d, size, s);
        unsigned first = rd16(d + s);
        if (first < 2 || (first & 1) || s + first > (uint32_t)size) continue;
        unsigned cnt = first / 2;
        /* Regionen = sortierte, eindeutige Tabellen-Offsets; Region i endet am naechsten Offset bzw. am Sektionsende. */
        unsigned off[256]; unsigned m = 0;
        for (unsigned i = 0; i < cnt && m < 256; i++) {
            unsigned o = rd16(d + s + 2 * i);
            if (!o || s + o >= (uint32_t)size) continue;
            unsigned j = 0; while (j < m && off[j] != o) j++;
            if (j == m) off[m++] = o;
        }
        for (unsigned i = 1; i < m; i++) { unsigned v = off[i], j = i; while (j > 0 && off[j - 1] > v) { off[j] = off[j - 1]; j--; } off[j] = v; }
        for (unsigned i = 0; i < m; i++) {
            int a = (int)(s + off[i]);
            int e = (i + 1 < m) ? (int)(s + off[i + 1]) : se;
            if (e > se) e = se;
            if (e > a) k += region_ablaufen(d, a, e, out, n);
        }
    }
    return k;
}
