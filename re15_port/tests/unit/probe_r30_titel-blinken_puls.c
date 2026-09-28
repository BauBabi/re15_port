/* probe_r30_titel-blinken_puls.c - MESSSONDE Runde 30 / Thema D
 * "Im Titelbild ... blinkt der ausgewaehlte Bereich in der Frequenz im Vergleich zum
 *  Original zu schnell."
 * Dossier: analysis/befunde_runde30/titel-blinken.md
 *
 * Die Sonde BEHAUPTET nichts ueber den Port. Sie fuehrt den ORIGINAL-Puls-Handler
 * FUN_801028ec aus den ausgelieferten Bytes von BIN/TITLE.BIN in einem Mini-R3000 aus
 * (nur die Befehlsarten, die der Handler benutzt) und liest daraus:
 *   - Aufrufe je Periode, Minimum/Maximum des Pulswerts, Lage des Maximums
 * und prueft die Befehlsworte an JEDER im Dossier zitierten Adresse gegen die Datei
 * (TITLE.BIN laedt @0x80100000 ohne Kopf: Datei-Offset = Adresse - 0x80100000;
 *  PSX.EXE: Datei-Offset = 0x800 + Adresse - 0x80010000).
 *
 * Aufruf: probe_r30_titel_blinken_puls [<TITLE.BIN> [<PSX.EXE>]]
 * Rueckgabe 0 = jede zitierte Stelle steht so in der Datei; 1 = Abweichung.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_R30_PSX_EXE
#define RE15_R30_PSX_EXE "../info/Re1.5/PSX.EXE"
#endif

#define TITLE_BASE 0x80100000u
#define EXE_BASE   0x80010000u
#define EXE_HDR    0x800u

static uint8_t *s_title; static long s_title_n;
static uint8_t *s_exe;   static long s_exe_n;
static int s_fail;

static uint8_t *slurp(const char *path, long *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *d = (uint8_t *) malloc((size_t) *n + 4);
    if (d && fread(d, 1, (size_t) *n, f) != (size_t) *n) { free(d); d = NULL; }
    fclose(f);
    return d;
}
static uint32_t rd32(const uint8_t *d, long off)
{ return (uint32_t) d[off] | ((uint32_t) d[off + 1] << 8) | ((uint32_t) d[off + 2] << 16) | ((uint32_t) d[off + 3] << 24); }
static uint32_t tw(uint32_t a) { return rd32(s_title, (long) (a - TITLE_BASE)); }
static uint32_t ew(uint32_t a) { return rd32(s_exe, (long) (EXE_HDR + a - EXE_BASE)); }

typedef struct { uint32_t addr, word; const char *text; } cite_t;

static void check(const char *file, const cite_t *c, int n, uint32_t (*rd)(uint32_t))
{
    for (int i = 0; i < n; i++) {
        uint32_t got = rd(c[i].addr);
        int ok = (got == c[i].word);
        printf("  %s @0x%08x  %08x  %-46s %s\n", file, (unsigned) c[i].addr, (unsigned) got, c[i].text,
               ok ? "BELEGT" : "ABWEICHUNG");
        if (!ok) { printf("      erwartet %08x\n", (unsigned) c[i].word); s_fail = 1; }
    }
}

/* ---------------------------------------------------------------- Mini-R3000 --------- */
static uint32_t R[32];
static uint16_t m16(uint32_t a) { long o = (long) (a - TITLE_BASE); return (uint16_t) (s_title[o] | (s_title[o + 1] << 8)); }
static void     w16(uint32_t a, uint16_t v) { long o = (long) (a - TITLE_BASE); s_title[o] = (uint8_t) v; s_title[o + 1] = (uint8_t) (v >> 8); }

/* fuehrt eine Funktion bis zum Ruecksprung aus; 0 = ok, -1 = unbekannter Befehl */
static int run(uint32_t entry)
{
    const uint32_t SENT = 0xdead0000u;
    uint32_t pc = entry, npc = entry + 4;
    R[31] = SENT;
    for (int guard = 0; guard < 4096; guard++) {
        if (pc == SENT) return 0;
        uint32_t ins = tw(pc), nn = npc + 4;
        uint32_t op = ins >> 26, rs = (ins >> 21) & 31, rt = (ins >> 16) & 31, rd = (ins >> 11) & 31;
        uint32_t imm = ins & 0xffffu; int32_t simm = (int16_t) imm;
        switch (op) {
        case 0x00:
            switch (ins & 0x3f) {
            case 0x00: R[rd] = R[rt] << ((ins >> 6) & 31); break;          /* sll / nop */
            case 0x08: nn = R[rs]; break;                                  /* jr        */
            default: printf("unbekannter SPECIAL-Befehl %08x @%08x\n", (unsigned) ins, (unsigned) pc); return -1;
            }
            break;
        case 0x02: nn = (npc & 0xf0000000u) | ((ins & 0x03ffffffu) << 2); break;   /* j     */
        case 0x04: if (R[rs] == R[rt]) nn = npc + ((uint32_t) simm << 2); break;   /* beq   */
        case 0x05: if (R[rs] != R[rt]) nn = npc + ((uint32_t) simm << 2); break;   /* bne   */
        case 0x09: R[rt] = R[rs] + (uint32_t) simm; break;                         /* addiu */
        case 0x0b: R[rt] = (R[rs] < (uint32_t) simm) ? 1u : 0u; break;             /* sltiu */
        case 0x0d: R[rt] = R[rs] | imm; break;                                     /* ori   */
        case 0x0f: R[rt] = imm << 16; break;                                       /* lui   */
        case 0x25: R[rt] = m16(R[rs] + (uint32_t) simm); break;                    /* lhu   */
        case 0x29: w16(R[rs] + (uint32_t) simm, (uint16_t) R[rt]); break;          /* sh    */
        default: printf("unbekannter Befehl %08x @%08x\n", (unsigned) ins, (unsigned) pc); return -1;
        }
        R[0] = 0;
        pc = npc; npc = nn;
    }
    printf("Laufzeitwaechter: kein Ruecksprung\n");
    return -1;
}

static int count_jal(uint32_t lo, uint32_t hi, uint32_t target)
{
    uint32_t w = 0x0c000000u | ((target >> 2) & 0x03ffffffu);
    int n = 0;
    for (uint32_t a = lo; a < hi; a += 4) if (tw(a) == w) n++;
    return n;
}

int main(int argc, char **argv)
{
    const char *tp = (argc > 1) ? argv[1] : RE15_ASSET_PSX_DIR "/BIN/TITLE.BIN";
    const char *ep = (argc > 2) ? argv[2] : RE15_R30_PSX_EXE;
    s_title = slurp(tp, &s_title_n);
    if (!s_title) { printf("FEHLER: %s nicht lesbar\n", tp); return 2; }
    printf("TITLE.BIN: %s (%ld Byte)\n", tp, s_title_n);

    /* ---- 1. der Puls-Handler, aus den Bytes ausgefuehrt ---------------------------- */
    printf("\n[1] FUN_801028ec aus den Datei-Bytes ausgefuehrt (Startwerte aus der Datei)\n");
    printf("    Startwert  @0x80102944 (Datei 0x2944) = 0x%04x   Zaehler @0x80102946 (Datei 0x2946) = %u\n",
           (unsigned) m16(0x80102944u), (unsigned) m16(0x80102946u));
    int period = -1, last_reset = 0, vmax = 0, vmin = 0xffff, at_max = -1, up = 0, down = 0;
    uint16_t prev = m16(0x80102944u);
    for (int call = 1; call <= 180; call++) {
        if (run(0x801028ecu) != 0) return 2;
        uint16_t val = m16(0x80102944u), ctr = m16(0x80102946u);
        if (call <= 60) {
            if (val > prev) up++; else if (val < prev && ctr != 0) down++;
            if ((int) val > vmax) { vmax = val; at_max = call; }
            if ((int) val < vmin) vmin = val;
        }
        if (call <= 3 || (call >= 30 && call <= 33) || (call >= 58 && call <= 62) || call == 120 || call == 180)
            printf("    Aufruf %3d: Zaehler %2u  Pulswert 0x%02x\n", call, (unsigned) ctr, (unsigned) val);
        if (ctr == 0) {
            if (period < 0) period = call - last_reset;
            else if (call - last_reset != period) { printf("    Periode schwankt\n"); s_fail = 1; }
            last_reset = call;
        }
        prev = val;
    }
    printf("    => Aufrufe je Periode: %d   Pulswert min 0x%02x max 0x%02x (Maximum nach Aufruf %d)\n",
           period, (unsigned) vmin, (unsigned) vmax, at_max);
    printf("    => %d Schritte aufwaerts (+2), %d abwaerts (-2), dann Ruecksetzen auf 0x80\n", up, down);
    if (period != 0x3c || vmax != 0xbe || vmin != 0x80 || at_max != 0x1f) { printf("    ABWEICHUNG vom Dossier\n"); s_fail = 1; }

    /* frische Kopie fuer die Wortpruefung (der Lauf oben hat die zwei Datenworte beschrieben) */
    free(s_title); s_title = slurp(tp, &s_title_n);
    if (!s_title) return 2;

    /* ---- 2. die zitierten Stellen in TITLE.BIN --------------------------------------- */
    printf("\n[2] zitierte Befehlsworte in TITLE.BIN\n");
    static const cite_t T[] = {
        { 0x801028fcu, 0x2c62001fu, "sltiu v0,v1,0x1f   (Zaehler < 31 -> aufwaerts)" },
        { 0x8010290cu, 0x24840002u, "addiu a0,a0,2      (Schritt +2)" },
        { 0x80102910u, 0x2484fffeu, "addiu a0,a0,-2     (Schritt -2)" },
        { 0x80102914u, 0x24630001u, "addiu v1,v1,1      (Zaehler +1)" },
        { 0x80102918u, 0x3402003cu, "ori v0,zero,0x3c   (Periode 60 Aufrufe)" },
        { 0x80102924u, 0x34030000u, "ori v1,zero,0      (Zaehler := 0)" },
        { 0x80102928u, 0x34040080u, "ori a0,zero,0x80   (Pulswert := 0x80)" },
        { 0x80102944u, 0x00000080u, "Daten: Pulswert 0x0080, Zaehler 0x0000" },
        { 0x80101fb4u, 0x0040f809u, "jalr v0            (Titel-Task ruft State-Handler)" },
        { 0x80101fbcu, 0x0c00a6b2u, "jal 0x80029ac8     (Task gibt ab)" },
        { 0x80101fc0u, 0x34040001u, "ori a0,zero,1      (... fuer 1 Durchgang)" },
        { 0x80101fc4u, 0x080407e6u, "j 0x80101f98       (Schleife)" },
        { 0x801020c4u, 0x34080001u, "ori t0,zero,1      (Sub-State := 1 = Menue)" },
        { 0x801020ccu, 0xa02826c5u, "sb t0,0x26c5(at)   (... nach 0x801026c5)" },
        { 0x80102ba0u, 0x0c040a3bu, "jal 0x801028ec     (Menue: 1 Pulsschritt)" },
        { 0x80102d10u, 0x0c040a84u, "jal 0x80102a10     (Fade A: Neuzeichnen OHNE Puls)" },
        { 0x80102d60u, 0x0c040a84u, "jal 0x80102a10     (Fade B: Neuzeichnen OHNE Puls)" },
        { 0x80102800u, 0x3c08e100u, "lui t0,0xe100      (Texpage-Befehl ...)" },
        { 0x80102804u, 0x25080095u, "addiu t0,t0,0x95   (... 0xe1000095)" },
        { 0x80102824u, 0x35080020u, "ori t0,t0,0x20     (Durchgang 0: ABR 1 additiv)" },
        { 0x8010281cu, 0x35080040u, "ori t0,t0,0x40     (Durchgang 1: ABR 2 subtraktiv)" },
        { 0x80102810u, 0x3c0b0001u, "lui t3,0x1         (Durchgang 1: y+1)" },
        { 0x80102830u, 0x3c086681u, "lui t0,0x6681      (Rechteck-Befehl ...)" },
        { 0x80102834u, 0x25088080u, "addiu t0,t0,-32640 (... 0x66808080)" },
        { 0x80102848u, 0x95082944u, "lhu t0,0x2944(t0)  (aktive Zeile: Pulswert)" },
        { 0x80102850u, 0xa2080008u, "sb t0,8(s0)        (... als R)" },
        { 0x80102854u, 0xa2080009u, "sb t0,9(s0)        (... als G)" },
        { 0x80102858u, 0xa208000au, "sb t0,10(s0)       (... als B)" },
    };
    check("TITLE", T, (int) (sizeof T / sizeof T[0]), tw);
    {
        int in_menu = count_jal(0x80102b00u, 0x80102cccu, 0x801028ecu);
        int in_draw = count_jal(0x80102a10u, 0x80102a8cu, 0x801028ecu);
        int in_fade = count_jal(0x80102cccu, 0x80102db8u, 0x801028ecu);
        printf("  Aufrufe von FUN_801028ec: Menue FUN_80102b00 = %d, Neuzeichner FUN_80102a10 = %d, Fade FUN_80102ccc = %d\n",
               in_menu, in_draw, in_fade);
        if (in_menu != 1 || in_draw != 0 || in_fade != 0) { printf("  ABWEICHUNG vom Dossier\n"); s_fail = 1; }
    }

    /* ---- 3. die zitierten Stellen in PSX.EXE ------------------------------------------ */
    s_exe = slurp(ep, &s_exe_n);
    if (!s_exe) {
        printf("\n[3] PSX.EXE nicht lesbar (%s) - EXE-Teil NICHT GEPRUEFT\n", ep);
        s_fail = 1;
    } else {
        printf("\n[3] zitierte Befehlsworte in PSX.EXE (%s, %ld Byte)\n", ep, s_exe_n);
        static const cite_t E[] = {
            { 0x8002130cu, 0x34020002u, "ori v0,zero,2      (Boot-Task: VSync-Argument 2)" },
            { 0x80021314u, 0xa0225456u, "sb v0,0x5456(at)   (... nach DAT_800b5456)" },
            { 0x80021318u, 0x0c00a68au, "jal 0x80029a28     (Overlay 0 = TITLE.BIN starten)" },
            { 0x8002131cu, 0x00002021u, "addu a0,zero,zero  (... Tabelleneintrag 0)" },
            { 0x80021478u, 0x26105456u, "addiu s0,s0,0x5456 (Flip: &DAT_800b5456)" },
            { 0x8002147cu, 0x92040000u, "lbu a0,0(s0)       (Flip: a0 = DAT_800b5456)" },
            { 0x80021480u, 0x0c0187f0u, "jal 0x80061fc0     (Flip: VSync(a0))" },
            { 0x80020de4u, 0x0c00a62cu, "jal 0x800298b0     (Hauptschleife: Tasks)" },
            { 0x80020f4cu, 0x0c0084dfu, "jal 0x8002137c     (Hauptschleife: Flip)" },
            { 0x80029adcu, 0xa4640002u, "sh a0,2(v1)        (Abgeben: Wartezaehler)" },
            { 0x80029ae8u, 0xa4620000u, "sh v0,0(v1)        (Abgeben: Zustand 1)" },
            { 0x8002994cu, 0x2442ffffu, "addiu v0,v0,-1     (Scheduler: Wartezaehler -1)" },
            { 0x80029958u, 0x1c400005u, "bgtz v0,...        (>0 -> Task ruht weiter)" },
            { 0x80062028u, 0x8c4287f0u, "lw v0,-30736(v0)   (VSync: letzter Vcount)" },
            { 0x80062030u, 0x2442ffffu, "addiu v0,v0,-1     (VSync: Ziel = letzter - 1 ...)" },
            { 0x80062038u, 0x00441021u, "addu v0,v0,a0      (... + n)" },
            { 0x80062078u, 0x24840001u, "addiu a0,a0,1      (VSync: danach Vcount + 1)" },
        };
        check("EXE  ", E, (int) (sizeof E / sizeof E[0]), ew);
        uint32_t t0 = ew(0x80073bdcu), t1 = ew(0x80073be0u);
        printf("  Overlay-Tabelle @0x80073bdc[0]: Datei-Id %u, Einsprung 0x%08x  %s\n", (unsigned) t0, (unsigned) t1,
               (t0 == 6u && t1 == 0x80101f7cu) ? "BELEGT" : "ABWEICHUNG");
        if (!(t0 == 6u && t1 == 0x80101f7cu)) s_fail = 1;
        printf("  DAT_800b5456 im EXE-Abbild (Datei 0x%05x): %u\n",
               (unsigned) (EXE_HDR + 0x800b5456u - EXE_BASE), (unsigned) s_exe[EXE_HDR + 0x800b5456u - EXE_BASE]);
    }

    /* ---- 4. Rechnung ------------------------------------------------------------------ */
    {
        /* Bildrate: psx-spx graphicsprocessingunitgpu.md "Vertical Refresh Rates":
         * NTSC non-interlaced 59.826 Hz (DISPENV.isinter = 0 im Titel, Savestate-Messung). */
        const double hz = 59.826;
        const int vsync_arg = 2, calls = 0x3c;
        double tick_ms = 1000.0 * vsync_arg / hz;
        printf("\n[4] Rechnung: %d Aufrufe x %d VBlanks = %d VBlanks;  bei %.3f Hz: Tick %.3f ms, Periode %.1f ms\n",
               calls, vsync_arg, calls * vsync_arg, hz, tick_ms, tick_ms * calls);
    }
    printf("\nERGEBNIS: %s\n", s_fail ? "ABWEICHUNG - siehe oben" : "alle zitierten Stellen BELEGT");
    return s_fail ? 1 : 0;
}
