/* probe_save_room_name.c — DIAGNOSE (kein ctest): Nutzer-Report 2026-08-05
 * "Egal in welchem Raum ich speichere, es steht immer Irons Office."
 *
 * ORIGINAL (byte-verifiziert, PSX.EXE + DEBUG.BIN, siehe analysis/bug_save_room_name.md):
 *
 *   FUN_80026658 (Memory-Card-Screen-Zeichner) baut die Slot-Zeile so:
 *     @0x800267f4 lbu (blk+2)&3     -> sysmes-Index 0x18 (Leon) / 0x19 (Elza)
 *     @0x80026800 memcpy(DAT_80026aa4, sysmes(idx), 0xc)
 *     @0x80026810 jal FUN_80026e4c(blk)        <-- ORTSNAMEN-INDEX
 *     @0x8002681c memcpy(DAT_80026ab0, sysmes(0x1a + idx), 0x10)
 *     @0x8002683c DAT_80026aad += cnt/10 ; DAT_80026aae += cnt%10
 *
 *   FUN_80026e4c @0x80026e4c ist ein STUB:
 *     80026e4c: 08 00 e0 03   jr   ra
 *     80026e50: 21 10 00 00   addu v0,zero,zero      -> RETURN 0, IMMER
 *   (Datei-Offset PSX.EXE 0x1764c; identisch im Savestate-RAM.)
 *   => das Original zeigt selbst IMMER sysmes 0x1a = "Irons' Office".
 *
 *   sysmes-Getter FUN_800c00e4 (DEBUG.BIN @0x800c00e4):
 *     t0 = 0x800c0000 + 24470 = 0x800c5f96 ; return t0 + u16[t0 + idx*2]
 *   => Offset-Tabelle DEBUG.BIN-Datei 0x5f96, Strings relativ dazu.
 *
 * Diese Probe LIEST die Tabelle aus der echten DEBUG.BIN, baut die
 * ORIGINAL-Bytefolge der Slot-Zeile nach und vergleicht sie mit der
 * Port-Bytefolge (pc_slot_title_codes, main.c:521-539, hier 1:1 repliziert,
 * weil sie static in main.c liegt). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

/* DEBUG.BIN laedt RAW @0x800c0000 (kein 0x800-Header) */
#define DBG_LOAD   0x800c0000u
#define SYSMES_TBL 0x5f96u          /* = 0x800c5f96 - 0x800c0000, aus FUN_800c00e4 */

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fails++; printf("  FAIL: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *read_file(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (b && fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)s; return b;
}

/* FUN_800c00e4 byte-true */
static const uint8_t *sysmes(const uint8_t *dbg, unsigned idx)
{
    const uint8_t *t = dbg + SYSMES_TBL;
    unsigned off = (unsigned)t[idx * 2] | ((unsigned)t[idx * 2 + 1] << 8);
    return t + off;
}

/* Atlas-Code -> ASCII (Umkehr von re15_msgfont_code, render_pc.c:2396-2409) */
static char atlas_to_ascii(uint8_t c)
{
    if (c >= 0x1d && c <= 0x36) return (char)('A' + (c - 0x1d));
    if (c >= 0x3d && c <= 0x56) return (char)('a' + (c - 0x3d));
    if (c >= 0x0c && c <= 0x15) return (char)('0' + (c - 0x0c));
    switch (c) {
        case 0x00: return ' ';  case 0x57: return '.';  case 0x18: return ',';
        case 0x38: return '/';  case 0x37: return '(';  case 0x39: return ')';
        case 0x3a: return '\''; case 0x3b: return '-';  case 0x16: return ':';
        case 0x17: return ';';  case 0x1a: return '!';  case 0x1b: return '?';
    }
    return '?';
}

static void show(const char *tag, const uint8_t *c, int n)
{
    printf("  %-10s bytes:", tag);
    for (int i = 0; i < n; i++) printf(" %02x", c[i]);
    printf("\n  %-10s text :\"", tag);
    for (int i = 0; i < n; i++) {
        if (c[i] == 0x05) { i++; continue; }        /* Farb-Op frisst das naechste Byte */
        if (c[i] == 0x01) break;                    /* Terminator */
        putchar(atlas_to_ascii(c[i]));
    }
    printf("\"\n");
}

/* --- PORT: main.c pc_slot_title_codes, 1:1 kopiert (Stand 2026-08-08: Ortsname
 * datengetrieben sysmes(0x1a+loc), Kopie bis exkl. Terminator 0x01, max 0x10 —
 * der Original-memcpy nimmt 0x10 Bytes @0x80026830, der Drucker stoppt am 0x01). */
static int port_slot_title_codes(const uint8_t *dbg, uint8_t *tc, int character, int count, int loc)
{
    static const uint8_t leon[] = { 0x28, 0x41, 0x4b, 0x4a };
    static const uint8_t elza[] = { 0x21, 0x48, 0x56, 0x3d };
    int n = 0;
    const uint8_t *nm = character ? elza : leon;
    tc[n++] = 0x05; tc[n++] = (uint8_t)(character ? 0x06 : 0x07);
    for (int k = 0; k < 4; k++)  tc[n++] = nm[k];
    tc[n++] = 0x05; tc[n++] = 0x00;
    tc[n++] = 0x00;
    tc[n++] = 0x38;
    tc[n++] = (uint8_t)(0x0c + (count / 10) % 10);
    tc[n++] = (uint8_t)(0x0c + count % 10);
    tc[n++] = 0x38;
    tc[n++] = 0x00; tc[n++] = 0x00;
    const uint8_t *loc_nm = sysmes(dbg, (unsigned)(0x1a + loc));
    for (int k = 0; k < 0x10 && loc_nm[k] != 0x01; k++) tc[n++] = loc_nm[k];
    return n;
}

/* --- ORIGINAL: FUN_80026658 @0x800267f4-0x80026848 --------------------------- */
static int orig_slot_title_codes(const uint8_t *dbg, uint8_t *buf, int character, int count, int room_idx)
{
    memcpy(buf, sysmes(dbg, character ? 0x19u : 0x18u), 0xc);   /* @0x80026800 */
    memcpy(buf + 0xc, sysmes(dbg, (unsigned)(0x1a + room_idx)), 0x10);  /* @0x8002681c */
    buf[9]  = (uint8_t)(buf[9]  + count / 10);                  /* @0x8002683c */
    buf[10] = (uint8_t)(buf[10] + count % 10);                  /* @0x80026848 */
    return 0x1c;
}

int main(void)
{
    char path[512];
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    snprintf(path, sizeof path, "%s/BIN/DEBUG.BIN", base);
    size_t sz = 0;
    uint8_t *dbg = read_file(path, &sz);
    if (!dbg) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    printf("DEBUG.BIN: %s (%zu Bytes, laedt @0x%08x)\n\n", path, sz, DBG_LOAD);

    /* ---- M1: die 8 Ortsnamen der Save-Maske, sysmes 0x1a..0x21 ---- */
    printf("[M1] sysmes-Ortsnamen (FUN_80026658 nutzt 0x1a + FUN_80026e4c()):\n");
    for (unsigned i = 0x1a; i <= 0x21; i++) {
        const uint8_t *s = sysmes(dbg, i);
        printf("  idx 0x%02x (= room_idx %u) @DEBUG.BIN 0x%04x: \"", i, i - 0x1a, (unsigned)(s - dbg));
        for (int k = 0; k < 24 && s[k] != 0x01; k++) {
            if (s[k] == 0x05) { k++; continue; }
            putchar(atlas_to_ascii(s[k]));
        }
        printf("\"\n");
    }

    /* ---- M2: ORIGINAL-Zeile vs PORT-Zeile (Leon, Zaehler 7) ---- */
    printf("\n[M2] Slot-Zeile Leon, Zaehler 7:\n");
    uint8_t o[64], p[64];
    int on = orig_slot_title_codes(dbg, o, 0, 7, 0);   /* Resolver == 0 (Schreibmaschine/Stub) */
    int pn = port_slot_title_codes(dbg, p, 0, 7, 0);
    show("ORIGINAL", o, on);
    show("PORT", p, pn);
    CHECK(pn == on, "Laenge: Port %d Bytes, Original %d Bytes", pn, on);
    CHECK(pn == on && memcmp(p, o, (size_t)on) == 0, "Bytefolge weicht ab (siehe oben)");

    /* ---- M2b: loc=1 (Telefon ROOM1070/1071 — Patch-Uebernahme) ---- */
    printf("\n[M2b] Slot-Zeile Leon, Zaehler 7, loc=1 (Telefon -> sysmes 0x1b Medical Room):\n");
    on = orig_slot_title_codes(dbg, o, 0, 7, 1);       /* Resolver == 1 (Patch AOT_TYPE1_HOOK) */
    pn = port_slot_title_codes(dbg, p, 0, 7, 1);
    show("ORIGINAL", o, on);
    show("PORT", p, pn);

    /* ---- M3: dasselbe fuer Elza ---- */
    printf("\n[M3] Slot-Zeile Elza, Zaehler 12:\n");
    on = orig_slot_title_codes(dbg, o, 1, 12, 0);
    pn = port_slot_title_codes(dbg, p, 1, 12, 0);
    show("ORIGINAL", o, on);
    show("PORT", p, pn);
    CHECK(pn == on && memcmp(p, o, (size_t)on) == 0, "Bytefolge Elza weicht ab");

    /* ---- M4: Ortsname des Ports ist byte-true == sysmes 0x1a? ---- */
    {
        const uint8_t *s = sysmes(dbg, 0x1a);
        static const uint8_t port_room[] = { 0x25,0x4e,0x4b,0x4a,0x4f,0x3a,0x00,0x2b,0x42,0x42,0x45,0x3f,0x41 };
        printf("\n[M4] Port-Ortsname vs sysmes 0x1a: %s\n",
               memcmp(port_room, s, sizeof port_room) == 0 ? "IDENTISCH" : "ABWEICHUNG");
        CHECK(memcmp(port_room, s, sizeof port_room) == 0, "Port-room[] != sysmes 0x1a");
    }

    /* ---- M5: Debug-Menue-Raumnamen der Save-Point-Raeume (DEBUG.BIN 0x2642) ---- */
    {
        const unsigned BASE = 0x2642, STAGE_HW = 637, REC_HW = 13;
        struct { unsigned room; int stage, idx; } sp[] = {
            { 0x1070, 0, 0x07 }, { 0x1120, 0, 0x12 }, { 0x1150, 0, 0x15 },
            { 0x2010, 1, 0x01 }, { 0x30a0, 2, 0x0a }, { 0x30b0, 2, 0x0b },
            { 0x4010, 3, 0x01 }, { 0x5010, 4, 0x01 },
        };
        printf("\n[M5] Debug-Menue-Namen der 8 Save-Point-Raeume (DEBUG.BIN 0x2642, 26 B/Satz):\n");
        for (unsigned i = 0; i < sizeof sp / sizeof sp[0]; i++) {
            unsigned off = BASE + (STAGE_HW * (unsigned)sp[i].stage + REC_HW * (unsigned)sp[i].idx) * 2;
            char nm[15]; memcpy(nm, dbg + off, 14); nm[14] = 0;
            printf("  ROOM%04X (stage %d idx 0x%02x) @0x%04x: \"%s\"\n",
                   sp[i].room, sp[i].stage, sp[i].idx, off, nm);
        }
    }

    free(dbg);
    if (fails) { printf("\nprobe_save_room_name: %d Abweichung(en) gemessen\n", fails); return 1; }
    printf("\nprobe_save_room_name: Port == Original\n");
    return 0;
}
