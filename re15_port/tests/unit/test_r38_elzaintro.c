/* test_r38_elzaintro.c — Runde 38 Spur E "elzaintro": Elzas Vorspann. Dossier analysis/befunde_runde38/
 * E_elzaintro.md, Regel include/re15_elzaintro_r38.h, Tabellen engine/src/elzaintro_r38.c.
 *
 *   stimme   die Zuordnung ROOM1241 msg 0..5 -> Aufnahmen room1240 gegen die ausgelieferten Daten: Nachrichten-
 *            Offsettabelle (RDT+0x3C) gleich, msg 0..5 byte-gleich bis 0x01-END, Message_on `2b 0N 00 00` an den
 *            Offsets beider sub02 (ROOM1240 @0x057E.. / ROOM1241 @0x0582..); die Abbildung trifft genau (1241, 0..5),
 *            der Ersatzpfad ist synchro/STAGE1/room1240/main0N.wav, und diese sechs Dateien liegen im Repo
 *   schwarz  die Erzaehler-Tabelle: je Zeile steht am Datei-Offset `29 cut`, es ist das EINZIGE Cut_chg auf den
 *            Schnitt im ganzen Raum-SCD (Opcode-Walk mit scd_opcode_size_at ueber alle main-/sub-Regionen), keine
 *            RVD-Zone (RDT+0x28) fuehrt hinein, die Kamera (RDT+0x24, 0x20 je Schnitt) ist byte-gleich mit Leons
 *            ROOM1170 Cut 7, ROOM1031 Cut 13 = Lobby-Kamera Cut 0, das Standbild BG13 = ROOM1170 BG07; das Gate
 *            schaltet nur genau diese zwei (Raum, Cut)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_elzaintro_r38.h"
#include "re15_scd.h"

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
#ifndef RE15_SYNCHRO_DIR
#error RE15_SYNCHRO_DIR fehlt
#endif

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz + 1);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(b); return NULL; }
    fclose(f);
    if (n) *n = sz;
    return b;
}

static uint8_t *rdt(unsigned id, long *n)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, id >> 12, id);
    uint8_t *b = slurp(p, n);
    if (!b) { printf("FAIL: %s fehlt\n", p); fails++; }
    return b;
}

static unsigned u16(const uint8_t *b, long o) { return (unsigned)b[o] | ((unsigned)b[o + 1] << 8); }
static unsigned long u32(const uint8_t *b, long o)
{
    return (unsigned long)b[o] | ((unsigned long)b[o + 1] << 8) | ((unsigned long)b[o + 2] << 16) |
           ((unsigned long)b[o + 3] << 24);
}

/* Laenge einer Nachricht bis einschliesslich 0x01-END (Steuerzeichen mit Parameter: 02 04 05 06 09 0A 0B). */
static long msg_len(const uint8_t *b, long st, long n)
{
    long i = st;
    while (i < n) {
        uint8_t c = b[i];
        if (c == 0x01) return i + 1 - st;
        i += (c == 0x02 || c == 0x04 || c == 0x05 || c == 0x06 || c == 0x09 || c == 0x0A || c == 0x0B) ? 2 : 1;
    }
    return -1;
}

static int teil_stimme(void)
{
    long na = 0, nb = 0;
    uint8_t *a = rdt(0x1240, &na), *b = rdt(0x1241, &nb);
    if (!a || !b) return 1;
    long ma = (long)u32(a, 0x3C), mb = (long)u32(b, 0x3C);
    CHECK(ma == 0x62C && mb == 0x630, "Nachrichtenblock ROOM1240 @0x%lX, ROOM1241 @0x%lX (soll 0x62C / 0x630)", ma, mb);
    unsigned cnt_a = u16(a, ma) / 2, cnt_b = u16(b, mb) / 2;
    CHECK(cnt_a == 6 && cnt_b == 6, "je 6 Nachrichten (1240: %u, 1241: %u)", cnt_a, cnt_b);
    static const unsigned k_tab[6] = { 12, 105, 191, 278, 370, 484 };
    for (unsigned i = 0; i < 6 && i < cnt_a && i < cnt_b; i++) {
        unsigned oa = u16(a, ma + 2 * (long)i), ob = u16(b, mb + 2 * (long)i);
        long la = msg_len(a, ma + oa, na), lb = msg_len(b, mb + ob, nb);
        CHECK(oa == k_tab[i] && ob == k_tab[i] && la > 0 && la == lb && !memcmp(a + ma + oa, b + mb + ob, (size_t)la),
              "msg %u byte-gleich: 1240 @0x%lX / 1241 @0x%lX, %ld Byte", i, ma + (long)oa, mb + (long)ob, la);
    }
    /* Message_on-Folge beider sub02 (scd_dump_room.py): `2b id 00 00`, Leon @0x057E.., Elza je +4 */
    static const long k_mon[6] = { 0x057E, 0x0598, 0x05A0, 0x05BA, 0x05DA, 0x05FA };
    for (int i = 0; i < 6; i++) {
        const uint8_t s[4] = { 0x2B, (uint8_t)i, 0x00, 0x00 };
        CHECK(!memcmp(a + k_mon[i], s, 4) && !memcmp(b + k_mon[i] + 4, s, 4),
              "Message_on(%d): ROOM1240 @0x%04lX, ROOM1241 @0x%04lX", i, k_mon[i], k_mon[i] + 4);
    }
    CHECK(!memcmp(a + 0x055A, "\x22\x03\x8b\x01", 4) && !memcmp(b + 0x055A, "\x22\x03\x6f\x01\x22\x03\x70\x01", 8),
          "sub02-Kopf: 1240 Set(3,0x8B) / 1241 Set(3,0x6F)+Set(3,0x70) -> die 4 Byte Versatz");
    free(a); free(b);

    /* Abbildung: genau (1241, 0..5) -> 1240 */
    int ok = 1;
    for (int i = 0; i < 6; i++) ok &= re15_elzaintro_r38_stimme_raum(0x1241, i) == 0x1240;
    CHECK(ok, "(0x1241, 0..5) -> 0x1240");
    CHECK(re15_elzaintro_r38_stimme_raum(0x1241, 6) == 0x1241 && re15_elzaintro_r38_stimme_raum(0x1241, -1) == 0x1241,
          "(0x1241, 6) / (0x1241, -1) bleiben (ROOM1241 hat nur 6 Nachrichten)");
    CHECK(re15_elzaintro_r38_stimme_raum(0x1240, 0) == 0x1240 && re15_elzaintro_r38_stimme_raum(0x1031, 20) == 0x1031 &&
          re15_elzaintro_r38_stimme_raum(0x1171, 0) == 0x1171 && re15_elzaintro_r38_stimme_raum(0x1170, 8) == 0x1170,
          "Leon 1240, Elzas Erzaehler 1031/20 (Leons Erzaehler 1170 msg 8..11 hat keine Aufnahme), 1171 unberuehrt");
    char p[256];
    CHECK(re15_elzaintro_r38_stimme_pfad(0x1241, 3, "main03", p, sizeof p) &&
          !strcmp(p, "synchro/STAGE1/room1240/main03.wav"), "Ersatzpfad (1241, 3) = %s", p);
    CHECK(!re15_elzaintro_r38_stimme_pfad(0x1240, 3, "main03", p, sizeof p) &&
          !re15_elzaintro_r38_stimme_pfad(0x1241, 6, "main06", p, sizeof p), "kein Ersatz fuer Leon / fuer msg 6");
    for (int i = 0; i < 6; i++) {
        char q[600];
        snprintf(q, sizeof q, "%s/STAGE1/room1240/main%02d.wav", RE15_SYNCHRO_DIR, i);
        long n = 0; uint8_t *w = slurp(q, &n);
        CHECK(w && n > 44 && !memcmp(w, "RIFF", 4) && !memcmp(w + 8, "WAVE", 4), "Aufnahme %s (%ld Byte)", q, n);
        free(w);
    }
    return 0;
}

/* Ende einer SCD-Region: naechster Header-Zeiger (RDT+0x20..0x5C) hinter dem Start, sonst Dateiende. */
static long block_ende(const uint8_t *b, long n, long start)
{
    long e = n;
    for (long o = 0x20; o <= 0x5C; o += 4) {
        long v = (long)u32(b, o);
        if (v > start && v < e) e = v;
    }
    return e;
}

/* Zaehlt Cut_chg(cut) im ganzen Raum-SCD (main RDT+0x40, sub RDT+0x44; je Block u16-Offsettabelle). -1 = Walk
 * desynchronisiert. *off = Offset des letzten Treffers. */
static int cut_chg_zaehlen(const uint8_t *b, long n, int cut, long *off)
{
    int treffer = 0;
    for (int blk = 0; blk < 2; blk++) {
        long st = (long)u32(b, blk ? 0x44 : 0x40), en = block_ende(b, n, st);
        unsigned regs = u16(b, st) / 2;
        for (unsigned r = 0; r < regs; r++) {
            long rs = st + (long)u16(b, st + 2 * (long)r);
            long re = (r + 1 < regs) ? st + (long)u16(b, st + 2 * (long)(r + 1)) : en;
            long pc = rs;
            while (pc < re) {
                int sz = scd_opcode_size_at(b + pc);
                if (sz <= 0) { printf("  Walk-Abbruch @0x%05lX op 0x%02X\n", pc, b[pc]); return -1; }
                if (b[pc] == 0x29 && b[pc + 1] == (uint8_t)cut) { treffer++; *off = pc; }
                pc += sz;   /* ganze Region wie scd_dump_room.py (auch hinter einem Evt_end) */
            }
        }
    }
    return treffer;
}

static int teil_schwarz(void)
{
    CHECK(re15_elzaintro_r38_erzaehler_anzahl() == 2, "Erzaehler-Tabelle: %d Zeilen (soll 2)",
          re15_elzaintro_r38_erzaehler_anzahl());
    long nl = 0; uint8_t *leon = rdt(0x1170, &nl);
    if (!leon) return 1;
    long cam_l = (long)u32(leon, 0x24) + 7 * 0x20;
    for (int i = 0; i < re15_elzaintro_r38_erzaehler_anzahl(); i++) {
        unsigned raum = 0, off = 0; int cut = -1;
        re15_elzaintro_r38_erzaehler_zeile(i, &raum, &cut, &off);
        long n = 0; uint8_t *b = rdt(raum, &n);
        if (!b) continue;
        CHECK(b[off] == 0x29 && b[off + 1] == (uint8_t)cut, "ROOM%04X @0x%05X = `29 %02x` (Cut_chg(%d))",
              raum, off, (unsigned)cut, cut);
        long letzt = -1; int z = cut_chg_zaehlen(b, n, cut, &letzt);
        CHECK(z == 1 && letzt == (long)off, "ROOM%04X: %d x Cut_chg(%d) im ganzen SCD (soll 1, @0x%05lX)",
              raum, z, cut, letzt);
        /* RVD-Zonen: 20 Byte je Satz bis 0xFFFFFFFF, Byte 3 = Ziel-Schnitt */
        long zs = (long)u32(b, 0x28), o = zs; int hinein = 0, saetze = 0;
        while (o + 20 <= n && u32(b, o) != 0xFFFFFFFFul) { if (b[o + 3] == (uint8_t)cut) hinein++; saetze++; o += 20; }
        CHECK(saetze > 0 && hinein == 0, "ROOM%04X: %d RVD-Zonen ab 0x%lX, %d fuehren in Cut %d (soll 0)",
              raum, saetze, zs, hinein, cut);
        long cam = (long)u32(b, 0x24) + (long)cut * 0x20;
        CHECK(!memcmp(b + cam + 4, leon + cam_l + 4, 24), "ROOM%04X Kamera Cut %d @0x%lX = ROOM1170 Cut 7 @0x%lX (pos+tgt)",
              raum, cut, cam, cam_l);
        if (raum == 0x1031) {
            long cam0 = (long)u32(b, 0x24);
            CHECK(!memcmp(b + cam + 4, b + cam0 + 4, 24), "ROOM1031 Cut 13 = Lobby-Kamera Cut 0 @0x%lX", cam0);
        }
        free(b);
    }
    free(leon);
    /* Standbild: ROOM1031 BG13 = ROOM1170 BG07 (byte-gleich, das schwarze MDEC-Standbild) */
    { char p1[600], p2[600]; long n1 = 0, n2 = 0;
      snprintf(p1, sizeof p1, "%s/BSS/ROOM1031/BG13.BSS", RE15_ASSET_PSX_DIR);
      snprintf(p2, sizeof p2, "%s/BSS/ROOM1170/BG07.BSS", RE15_ASSET_PSX_DIR);
      uint8_t *x = slurp(p1, &n1), *y = slurp(p2, &n2);
      CHECK(x && y && n1 == n2 && !memcmp(x, y, (size_t)n1), "BSS/ROOM1031/BG13 = BSS/ROOM1170/BG07 (%ld Byte)", n1);
      free(x); free(y); }
    /* Gate: genau diese zwei (Raum, Cut) */
    CHECK(re15_elzaintro_r38_erzaehler_schwarz(0x1170, 7) && re15_elzaintro_r38_erzaehler_schwarz(0x1031, 13),
          "Gate an: ROOM1170 Cut 7, ROOM1031 Cut 13");
    CHECK(!re15_elzaintro_r38_erzaehler_schwarz(0x1031, 0) && !re15_elzaintro_r38_erzaehler_schwarz(0x1031, 12) &&
          !re15_elzaintro_r38_erzaehler_schwarz(0x1030, 13) && !re15_elzaintro_r38_erzaehler_schwarz(0x1170, 0) &&
          !re15_elzaintro_r38_erzaehler_schwarz(0x1171, 7) && !re15_elzaintro_r38_erzaehler_schwarz(0x1241, 0) &&
          !re15_elzaintro_r38_erzaehler_schwarz(0x1240, 0), "Gate aus: Lobby Cut 0/12, 1030, Helipad, 1171, Montage");
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "";
    if (!strcmp(teil, "stimme")) teil_stimme();
    else if (!strcmp(teil, "schwarz")) teil_schwarz();
    else { printf("Aufruf: test_r38_elzaintro stimme|schwarz\n"); return 2; }
    printf("%s: %d Pruefungen, %d Fehler\n", teil, checks, fails);
    return fails ? 1 : 0;
}
