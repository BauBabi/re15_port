/* test_r30_titel_blinken.c - RIEGEL Runde 30 / Thema D
 * "Im Titelbild ... blinkt der ausgewaehlte Bereich in der Frequenz im Vergleich zum
 *  Original zu schnell."
 * Dossier: analysis/befunde_runde30/titel-blinken.md
 *
 * ⛔ REICHWEITE: dieser Riegel bindet NUR re15_engine (engine/src/title_pulse.c). Er prueft die
 * Rechenbausteine (Pulsfolge, Tick-Rechnung, Uhr, Einblende) gegen das Original — NICHT, ob
 * platform/pc/main.c sie auch aufruft. Nimmt man die Verdrahtung in main.c zurueck, bleibt er
 * gruen. Das Symptom selbst (Periode am laufenden Spiel, Puls im Bestaetigungs-Fade) sichert der
 * Integrationstest integration_r30_titel_puls mit der echten re15_pc.exe
 * (tests/integration/test_r30_titel_puls.cmake).
 *
 * Teil A  PULSFOLGE: fuehrt den ORIGINAL-Puls-Handler FUN_801028ec aus den ausgelieferten
 *         Bytes von BIN/TITLE.BIN in einem Mini-R3000 aus (derselbe wie in der Messsonde
 *         probe_r30_titel-blinken_puls.c) und stellt JEDEM der 180 Aufrufe den Port-Schritt
 *         re15_title_pulse_step() gegenueber. Soll: 180 von 180 Wertepaaren (Zaehler, Wert)
 *         gleich. Der Port-Startzustand wird gegen die Datenworte der Datei geprueft.
 * Teil B  FALTUNG: re15_title_pulse_advance(n) == n Einzelschritte (n ueber die Periode hinaus).
 * Teil C  TICK-RECHNUNG: Durchgaenge aus Mikrosekunden, T_TICK = 2 VBlanks / 59,826 Hz.
 * Teil D  UHR: Unabhaengigkeit von der Abtastrate (dieselbe Laufzeit, abgetastet mit
 *         20/30/60/144/1000 Hz, ergibt dieselbe Schrittzahl und denselben Pulswert); neu
 *         aufsetzen nach Fade/Unterbildschirm zaehlt die Zwischenzeit nicht; Stillstand.
 * Teil E  EINBLENDE gegen die AUSGEFUEHRTEN Original-Bytes: derselbe Mini-R3000 faehrt die
 *         Titel-Init-Befehle TITLE.BIN 0x80102054-0x8010207c (darin `ori a1,zero,0xfc00`
 *         @0x80102058 und die Aufrufe FUN_800217b0 / FUN_800216ec aus PSX.EXE) und danach je
 *         Durchgang FUN_80021880 aus PSX.EXE; verglichen wird die Farbe, die das Original in das
 *         Rechteck von Kanal 0 schreibt, mit re15_title_fadein_level(Durchgang).
 *
 * (Ein frueherer Teil F "Gegenprobe" zaehlte nur eine for-Schleife und bewies nichts ueber den
 *  alten Stand — gestrichen. Die Gegenprobe am alten Stand fuehrt der Integrationstest.)
 *
 * Rueckgabe 0 = alles gruen, 1 = Abweichung, 2 = Datei nicht lesbar / Befehl unbekannt.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_title_pulse.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_R30_PSX_EXE
#define RE15_R30_PSX_EXE "../info/Re1.5/PSX.EXE"
#endif

#define TITLE_BASE 0x80100000u   /* TITLE.BIN laedt OHNE Kopf @0x80100000 */
#define EXE_BASE   0x80010000u   /* PSX.EXE: t_addr 0x80010000, Abbild ab Datei-Offset 0x800 */
#define STACK_BASE 0x801f0000u   /* Stapel fuer den Mini-R3000 (liegt hinter TITLE.BIN)       */
#define STACK_SIZE 0x10000u

static int s_fail;

#define CHECK(cond, ...) do { if (!(cond)) { printf("  ROT: "); printf(__VA_ARGS__); printf("\n"); s_fail = 1; } } while (0)

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

/* ---------------------------------------------------------------- Speicher ----------- */
typedef struct { uint32_t base; uint8_t *d; long n; } seg_t;
enum { SEG_TITLE, SEG_EXE, SEG_STACK, SEG_N };
static seg_t s_seg[SEG_N];
static int   s_memfault;

static uint8_t *mem(uint32_t a, unsigned w)
{
    for (int i = 0; i < SEG_N; i++) {
        const seg_t *s = &s_seg[i];
        if (s->d && a >= s->base && (long) (a - s->base) + (long) w <= s->n) return s->d + (a - s->base);
    }
    if (!s_memfault) printf("  Speicherzugriff ausserhalb der geladenen Bereiche: %08x\n", (unsigned) a);
    s_memfault = 1;
    return NULL;
}
static uint32_t m8(uint32_t a)  { uint8_t *p = mem(a, 1); return p ? p[0] : 0; }
static uint32_t m16(uint32_t a) { uint8_t *p = mem(a, 2); return p ? (uint32_t) (p[0] | (p[1] << 8)) : 0; }
static uint32_t m32(uint32_t a)
{
    uint8_t *p = mem(a, 4);
    return p ? ((uint32_t) p[0] | ((uint32_t) p[1] << 8) | ((uint32_t) p[2] << 16) | ((uint32_t) p[3] << 24)) : 0;
}
static void w8(uint32_t a, uint32_t v)  { uint8_t *p = mem(a, 1); if (p) p[0] = (uint8_t) v; }
static void w16(uint32_t a, uint32_t v) { uint8_t *p = mem(a, 2); if (p) { p[0] = (uint8_t) v; p[1] = (uint8_t) (v >> 8); } }
static void w32(uint32_t a, uint32_t v)
{
    uint8_t *p = mem(a, 4);
    if (p) { p[0] = (uint8_t) v; p[1] = (uint8_t) (v >> 8); p[2] = (uint8_t) (v >> 16); p[3] = (uint8_t) (v >> 24); }
}

static int load_title(const char *tp)
{
    free(s_seg[SEG_TITLE].d);
    s_seg[SEG_TITLE].d = slurp(tp, &s_seg[SEG_TITLE].n);
    s_seg[SEG_TITLE].base = TITLE_BASE;
    if (!s_seg[SEG_TITLE].d) { printf("FEHLER: %s nicht lesbar\n", tp); return 2; }
    if (s_seg[SEG_TITLE].n < 0x2948) { printf("FEHLER: TITLE.BIN zu kurz (%ld)\n", s_seg[SEG_TITLE].n); return 2; }
    return 0;
}
static int load_exe(const char *ep)
{
    long n = 0;
    uint8_t *d = slurp(ep, &n);
    if (!d) { printf("FEHLER: %s nicht lesbar\n", ep); return 2; }
    if (n < 0x800 || memcmp(d, "PS-X EXE", 8) != 0) { printf("FEHLER: %s ist keine PS-X EXE\n", ep); free(d); return 2; }
    uint32_t t_addr = (uint32_t) d[0x18] | ((uint32_t) d[0x19] << 8) | ((uint32_t) d[0x1a] << 16) | ((uint32_t) d[0x1b] << 24);
    if (t_addr != EXE_BASE) { printf("FEHLER: t_addr 0x%08x statt 0x80010000\n", (unsigned) t_addr); free(d); return 2; }
    free(s_seg[SEG_EXE].d);
    s_seg[SEG_EXE].d = (uint8_t *) malloc((size_t) (n - 0x800));
    memcpy(s_seg[SEG_EXE].d, d + 0x800, (size_t) (n - 0x800));
    s_seg[SEG_EXE].n = n - 0x800;
    s_seg[SEG_EXE].base = EXE_BASE;
    free(d);
    return 0;
}

/* ---------------------------------------------------------------- Mini-R3000 --------- */
/* Befehlsumfang = das, was FUN_801028ec (TITLE.BIN), die Titel-Init 0x80102054-7c,
 * FUN_800217b0, FUN_800216ec, FUN_80021880 und AddPrim FUN_8006b538 (PSX.EXE) benutzen.
 * Die Ladeverzoegerung des R3000 wird nicht nachgebildet: der Compiler hat in allen diesen
 * Funktionen den Platz hinter jedem Laden mit einem Befehl gefuellt, der das Ziel nicht liest
 * (nachgesehen fuer jede Ladeoperation). Ein unbekannter Befehl bricht ab (Rueckgabe -1). */
static uint32_t R[32];

/* NICHT ausgefuehrte Aufrufe: FUN_80069858 (SetDrawMode, von FUN_800217b0 zweimal gerufen)
 * schreibt nur das DR_MODE-Primitiv an a0 = Kanal+44 / Kanal+56 und ruft seinerseits
 * FUN_80069b54 / FUN_80069d90. FUN_80021880 liest diese Bytes nicht (es liest Kanal+0/+2
 * Pegel/Schritt, +5/+6/+7 Farbmasken, +8 OT-Ebene) — fuer die Farbe des Rechtecks ohne Belang. */
static const uint32_t s_stub[] = { 0x80069858u };
/* BEOBACHTETE Aufrufe (werden ausgefuehrt und mitgeschrieben): AddPrim FUN_8006b538. */
#define ADDPRIM 0x8006b538u
static uint32_t s_addprim_a1[64];
static int      s_addprim_n;

static int is_stub(uint32_t pc)
{
    for (unsigned i = 0; i < sizeof s_stub / sizeof s_stub[0]; i++) if (s_stub[i] == pc) return 1;
    return 0;
}

/* fuehrt ab `entry` aus bis zum Ruecksprung (bzw. bis pc == stop, falls stop != 0);
 * 0 = ok, -1 = unbekannter Befehl / Speicherfehler */
static int run_until(uint32_t entry, uint32_t stop)
{
    const uint32_t SENT = 0xdead0000u;
    uint32_t pc = entry, npc = entry + 4;
    R[31] = SENT;
    R[29] = STACK_BASE + STACK_SIZE - 0x100u;
    for (long guard = 0; guard < 200000; guard++) {
        if (pc == SENT || (stop && pc == stop)) return 0;
        if (pc == ADDPRIM && s_addprim_n < (int) (sizeof s_addprim_a1 / sizeof s_addprim_a1[0]))
            s_addprim_a1[s_addprim_n++] = R[5];
        if (is_stub(pc)) { R[2] = 0; pc = R[31]; npc = pc + 4; continue; }
        uint8_t *ip = mem(pc, 4);
        if (!ip) { printf("PC ausserhalb der geladenen Bereiche: %08x\n", (unsigned) pc); return -1; }
        uint32_t ins = m32(pc), nn = npc + 4;
        uint32_t op = ins >> 26, rs = (ins >> 21) & 31, rt = (ins >> 16) & 31, rd = (ins >> 11) & 31;
        uint32_t sa = (ins >> 6) & 31;
        uint32_t imm = ins & 0xffffu; int32_t simm = (int16_t) imm;
        uint32_t ea = R[rs] + (uint32_t) simm;
        switch (op) {
        case 0x00:
            switch (ins & 0x3f) {
            case 0x00: R[rd] = R[rt] << sa; break;                                      /* sll / nop */
            case 0x02: R[rd] = R[rt] >> sa; break;                                      /* srl   */
            case 0x03: R[rd] = (uint32_t) ((int32_t) R[rt] >> sa); break;               /* sra   */
            case 0x08: nn = R[rs]; break;                                               /* jr    */
            case 0x21: R[rd] = R[rs] + R[rt]; break;                                    /* addu  */
            case 0x23: R[rd] = R[rs] - R[rt]; break;                                    /* subu  */
            case 0x24: R[rd] = R[rs] & R[rt]; break;                                    /* and   */
            case 0x25: R[rd] = R[rs] | R[rt]; break;                                    /* or    */
            case 0x2a: R[rd] = ((int32_t) R[rs] < (int32_t) R[rt]) ? 1u : 0u; break;    /* slt   */
            case 0x2b: R[rd] = (R[rs] < R[rt]) ? 1u : 0u; break;                        /* sltu  */
            default: printf("unbekannter SPECIAL-Befehl %08x @%08x\n", (unsigned) ins, (unsigned) pc); return -1;
            }
            break;
        case 0x01:
            if (rt == 0)      { if ((int32_t) R[rs] <  0) nn = npc + ((uint32_t) simm << 2); }     /* bltz */
            else if (rt == 1) { if ((int32_t) R[rs] >= 0) nn = npc + ((uint32_t) simm << 2); }     /* bgez */
            else { printf("unbekannter REGIMM-Befehl %08x @%08x\n", (unsigned) ins, (unsigned) pc); return -1; }
            break;
        case 0x02: nn = (npc & 0xf0000000u) | ((ins & 0x03ffffffu) << 2); break;                 /* j     */
        case 0x03: R[31] = pc + 8; nn = (npc & 0xf0000000u) | ((ins & 0x03ffffffu) << 2); break; /* jal   */
        case 0x04: if (R[rs] == R[rt]) nn = npc + ((uint32_t) simm << 2); break;                 /* beq   */
        case 0x05: if (R[rs] != R[rt]) nn = npc + ((uint32_t) simm << 2); break;                 /* bne   */
        case 0x06: if ((int32_t) R[rs] <= 0) nn = npc + ((uint32_t) simm << 2); break;           /* blez  */
        case 0x07: if ((int32_t) R[rs] >  0) nn = npc + ((uint32_t) simm << 2); break;           /* bgtz  */
        case 0x09: R[rt] = R[rs] + (uint32_t) simm; break;                                       /* addiu */
        case 0x0a: R[rt] = ((int32_t) R[rs] < simm) ? 1u : 0u; break;                            /* slti  */
        case 0x0b: R[rt] = (R[rs] < (uint32_t) simm) ? 1u : 0u; break;                           /* sltiu */
        case 0x0c: R[rt] = R[rs] & imm; break;                                                   /* andi  */
        case 0x0d: R[rt] = R[rs] | imm; break;                                                   /* ori   */
        case 0x0f: R[rt] = imm << 16; break;                                                     /* lui   */
        case 0x20: R[rt] = (uint32_t) (int32_t) (int8_t) m8(ea); break;                          /* lb    */
        case 0x21: R[rt] = (uint32_t) (int32_t) (int16_t) m16(ea); break;                        /* lh    */
        case 0x23: R[rt] = m32(ea); break;                                                       /* lw    */
        case 0x24: R[rt] = m8(ea); break;                                                        /* lbu   */
        case 0x25: R[rt] = m16(ea); break;                                                       /* lhu   */
        case 0x28: w8(ea, R[rt]); break;                                                         /* sb    */
        case 0x29: w16(ea, R[rt]); break;                                                        /* sh    */
        case 0x2b: w32(ea, R[rt]); break;                                                        /* sw    */
        default: printf("unbekannter Befehl %08x @%08x\n", (unsigned) ins, (unsigned) pc); return -1;
        }
        R[0] = 0;
        if (s_memfault) { printf("  beim Befehl %08x @%08x\n", (unsigned) ins, (unsigned) pc); return -1; }
        pc = npc; npc = nn;
    }
    printf("Laufzeitwaechter: kein Ruecksprung\n");
    return -1;
}
static int run(uint32_t entry) { return run_until(entry, 0); }

/* ---------------------------------------------------------------- Teil A ------------- */
static int part_a(const char *tp)
{
    printf("[A] Pulsfolge: Port gegen FUN_801028ec aus %s\n", tp);
    int rc = load_title(tp);
    if (rc) return rc;

    re15_title_pulse_reset();
    CHECK(re15_title_pulse_value() == (int) m16(0x80102944u),
          "Startwert Port 0x%02x, Datei 0x2944 = 0x%04x", re15_title_pulse_value(), (unsigned) m16(0x80102944u));
    CHECK(re15_title_pulse_counter() == (int) m16(0x80102946u),
          "Startzaehler Port %d, Datei 0x2946 = %u", re15_title_pulse_counter(), (unsigned) m16(0x80102946u));

    int same = 0, period = -1, last_reset = 0, vmin = 0xffff, vmax = 0;
    for (int call = 1; call <= 180; call++) {
        if (run(0x801028ecu) != 0) return 2;
        re15_title_pulse_step();
        int o_val = (int) m16(0x80102944u), o_ctr = (int) m16(0x80102946u);
        int p_val = re15_title_pulse_value(), p_ctr = re15_title_pulse_counter();
        if (o_val == p_val && o_ctr == p_ctr) same++;
        else if (same == call - 1)
            printf("  erste Abweichung bei Aufruf %d: Original (%d, 0x%02x)  Port (%d, 0x%02x)\n",
                   call, o_ctr, (unsigned) o_val, p_ctr, (unsigned) p_val);
        if (p_val < vmin) vmin = p_val;
        if (p_val > vmax) vmax = p_val;
        if (p_ctr == 0) { if (period < 0) period = call - last_reset; last_reset = call; }
    }
    printf("  Wertepaare gleich: %d von 180   Periode %d Aufrufe   Pulswert 0x%02x ... 0x%02x\n",
           same, period, (unsigned) vmin, (unsigned) vmax);
    CHECK(same == 180, "nur %d von 180 Wertepaaren gleich", same);
    CHECK(period == 0x3c, "Periode %d statt 60 (0x3c @0x80102918)", period);
    CHECK(vmin == 0x80 && vmax == 0xbe, "Wertebereich 0x%02x..0x%02x statt 0x80..0xbe", (unsigned) vmin, (unsigned) vmax);
    return 0;
}

/* ---------------------------------------------------------------- Teil B ------------- */
static void part_b(void)
{
    printf("[B] Faltung ueber die Periode: advance(n) gegen n Einzelschritte\n");
    static const uint64_t N[] = { 0, 1, 30, 31, 32, 59, 60, 61, 119, 120, 121, 1000, 86400ull * 30ull };
    int ok = 0, n_cases = (int) (sizeof N / sizeof N[0]);
    for (int start = 0; start < 60; start += 7) {
        for (int i = 0; i < n_cases; i++) {
            re15_title_pulse_reset();
            for (int k = 0; k < start; k++) re15_title_pulse_step();
            for (uint64_t k = 0; k < N[i]; k++) re15_title_pulse_step();
            int e_val = re15_title_pulse_value(), e_ctr = re15_title_pulse_counter();
            re15_title_pulse_reset();
            for (int k = 0; k < start; k++) re15_title_pulse_step();
            re15_title_pulse_advance(N[i]);
            int g_val = re15_title_pulse_value(), g_ctr = re15_title_pulse_counter();
            if (e_val == g_val && e_ctr == g_ctr) ok++;
            else CHECK(0, "Start %d + %llu: Einzelschritte (%d, 0x%02x), advance (%d, 0x%02x)", start,
                       (unsigned long long) N[i], e_ctr, (unsigned) e_val, g_ctr, (unsigned) g_val);
        }
    }
    printf("  %d Faelle gleich\n", ok);
}

/* ---------------------------------------------------------------- Teil C ------------- */
static void part_c(void)
{
    printf("[C] Tick-Rechnung (2 VBlanks @0x8002130c-14 bei 59,826 Hz -> T_TICK = 33430,28 us)\n");
    static const struct { uint64_t us, ticks; const char *why; } T[] = {
        {           0ull,    0ull, "Start" },
        {       33429ull,    0ull, "1 us vor T_TICK (abgerundet 33430)" },
        {       33430ull,    0ull, "33430 us < 33430,28 us" },
        {       33431ull,    1ull, "erster Durchgang" },
        {       66860ull,    1ull, "2 x 33430 us < 2 x T_TICK" },
        {       66861ull,    2ull, "zweiter Durchgang" },
        {     2005800ull,   59ull, "60 x 33430 us = 2005800 us liegt 17 us VOR dem 60. Durchgang" },
        {     2005816ull,   59ull, "1 us vor 60 x T_TICK = 2005816,9 us" },
        {     2005817ull,   60ull, "eine volle Pulsperiode (60 Durchgaenge)" },
        {  1000000000ull, 29913ull, "1000 s: 1000 x 59,826 / 2 = 29913" },
        { 3600000000ull, 107686ull, "1 h: 3600 x 29,913 = 107686,8" },
    };
    for (unsigned i = 0; i < sizeof T / sizeof T[0]; i++) {
        uint64_t got = re15_title_tick_count(T[i].us);
        printf("  %12llu us -> %6llu Durchgaenge (Soll %6llu)  %s\n", (unsigned long long) T[i].us,
               (unsigned long long) got, (unsigned long long) T[i].ticks, T[i].why);
        CHECK(got == T[i].ticks, "%llu us: %llu statt %llu", (unsigned long long) T[i].us,
              (unsigned long long) got, (unsigned long long) T[i].ticks);
    }
    /* Periode in Zeit: kleinste Laufzeit mit 60 Durchgaengen */
    uint64_t us = 2000000ull;
    while (re15_title_tick_count(us) < (uint64_t) RE15_TITLE_PULSE_PERIOD) us++;
    printf("  kleinste Laufzeit mit 60 Durchgaengen: %llu us (Soll 2005817 = aufgerundet 60 x 2 / 59,826 s)\n",
           (unsigned long long) us);
    CHECK(us == 2005817ull, "Periode %llu us statt 2005817", (unsigned long long) us);
    /* monoton und nie mehr als 1 Durchgang je Mikrosekunde */
    uint64_t prev = 0; int mono = 1;
    for (uint64_t t = 0; t < 400000ull; t++) {
        uint64_t k = re15_title_tick_count(t);
        if (k < prev || k > prev + 1) mono = 0;
        prev = k;
    }
    CHECK(mono, "Tick-Rechnung nicht monoton");
}

/* ---------------------------------------------------------------- Teil D ------------- */
/* faehrt `dauer_us` Laufzeit, abgetastet alle `schritt_us`; liefert Zahl der Pulsschritte */
static uint64_t fahre(uint64_t dauer_us, uint64_t schritt_us, int *val, int *ctr, uint64_t *max_due)
{
    re15_title_clock_t c;
    uint64_t now = 123456789ull, steps = 0;
    re15_title_pulse_reset();
    re15_title_clock_start(&c, now);
    *max_due = 0;
    for (uint64_t t = 0; t <= dauer_us; t += schritt_us) {
        uint64_t due = re15_title_clock_poll(&c, now + t);
        if (due > *max_due) *max_due = due;
        re15_title_pulse_advance(due);
        steps += due;
    }
    /* Rest bis genau dauer_us */
    { uint64_t due = re15_title_clock_poll(&c, now + dauer_us); re15_title_pulse_advance(due); steps += due; }
    *val = re15_title_pulse_value(); *ctr = re15_title_pulse_counter();
    return steps;
}

static void part_d(void)
{
    printf("[D] Uhr: Unabhaengigkeit von der Abtastrate, neu aufsetzen, Stillstand\n");
    static const struct { const char *name; uint64_t schritt_us; uint64_t max_due_soll; } A[] = {
        { "1000 Hz (ohne VSync)", 1000ull,  1ull },
        { " 144 Hz",              6944ull,  1ull },
        { "  60 Hz",             16667ull,  1ull },
        { "  30 Hz",             33333ull,  1ull },
        { "  20 Hz",             50000ull,  2ull },
    };
    const uint64_t dauer = 10000000ull;      /* 10 s */
    const uint64_t soll  = re15_title_tick_count(dauer);
    int val0 = -1, ctr0 = -1;
    for (unsigned i = 0; i < sizeof A / sizeof A[0]; i++) {
        int val, ctr; uint64_t max_due;
        uint64_t steps = fahre(dauer, A[i].schritt_us, &val, &ctr, &max_due);
        printf("  %-22s: %llu Pulsschritte in 10 s (Soll %llu), hoechstens %llu je Abtastung, Zustand (%d, 0x%02x)\n",
               A[i].name, (unsigned long long) steps, (unsigned long long) soll,
               (unsigned long long) max_due, ctr, (unsigned) val);
        CHECK(steps == soll, "%s: %llu Schritte statt %llu", A[i].name, (unsigned long long) steps, (unsigned long long) soll);
        CHECK(max_due <= A[i].max_due_soll, "%s: %llu Schritte in EINER Abtastung", A[i].name, (unsigned long long) max_due);
        if (i == 0) { val0 = val; ctr0 = ctr; }
        CHECK(val == val0 && ctr == ctr0, "%s: Zustand (%d, 0x%02x) weicht von 1000 Hz ab (%d, 0x%02x)",
              A[i].name, ctr, (unsigned) val, ctr0, (unsigned) val0);
    }
    CHECK(soll == 299ull, "10 s = %llu Durchgaenge statt 299 (10 x 59,826 / 2 = 299,13)", (unsigned long long) soll);

    /* Rueckkehr aus Bestaetigungs-Fade / Unterbildschirm: 1 s laufen, 5 s NICHT abfragen, dann
     * neu aufsetzen. Die 5 s zaehlen nicht, und der erste Durchgang nach dem Aufsetzen wird genau
     * einen Tick spaeter faellig (nicht frueher — sonst verkuerzt sich die Einblende). */
    {
        re15_title_clock_t c; uint64_t now = 5000ull, steps = 0;
        re15_title_clock_start(&c, now);
        steps += re15_title_clock_poll(&c, now + 1000000ull);
        uint64_t t_back = now + 6000000ull;
        re15_title_clock_start(&c, t_back);
        uint64_t at_restart = re15_title_clock_poll(&c, t_back);
        uint64_t before_tick = re15_title_clock_poll(&c, t_back + 33430ull);
        uint64_t at_tick = re15_title_clock_poll(&c, t_back + 33431ull);
        steps += at_restart + before_tick + at_tick;
        steps += re15_title_clock_poll(&c, t_back + 1000000ull);
        printf("  Rueckkehr: %llu Durchgaenge beim Aufsetzen, %llu nach 33430 us, %llu nach 33431 us, %llu gesamt"
               " (Soll 0 / 0 / 1 / %llu)\n",
               (unsigned long long) at_restart, (unsigned long long) before_tick, (unsigned long long) at_tick,
               (unsigned long long) steps, (unsigned long long) (2ull * re15_title_tick_count(1000000ull)));
        CHECK(at_restart == 0 && before_tick == 0 && at_tick == 1, "erster Durchgang nach dem Aufsetzen liegt falsch");
        CHECK(steps == 2ull * re15_title_tick_count(1000000ull), "Rueckkehr: %llu statt %llu", (unsigned long long) steps,
              (unsigned long long) (2ull * re15_title_tick_count(1000000ull)));
    }
    /* Zeitquelle laeuft rueckwaerts: nichts faellig, kein Unterlauf */
    {
        re15_title_clock_t c;
        re15_title_clock_start(&c, 1000000ull);
        uint64_t a = re15_title_clock_poll(&c, 1100000ull);
        uint64_t b = re15_title_clock_poll(&c, 1050000ull);
        uint64_t d = re15_title_clock_poll(&c, 900000ull);
        printf("  Zeit rueckwaerts: %llu, dann %llu und %llu Durchgaenge (Soll 2 / 0 / 0)\n",
               (unsigned long long) a, (unsigned long long) b, (unsigned long long) d);
        CHECK(a == 2 && b == 0 && d == 0, "Zeit rueckwaerts: %llu / %llu / %llu", (unsigned long long) a,
              (unsigned long long) b, (unsigned long long) d);
    }
    /* Stillstand (Fenster gezogen): 100 s in EINER Abtastung -> Pulszustand wie nach 100 s */
    {
        re15_title_clock_t c; uint64_t now = 77ull;
        re15_title_pulse_reset(); re15_title_clock_start(&c, now);
        uint64_t due = re15_title_clock_poll(&c, now + 100000000ull);
        re15_title_pulse_advance(due);
        int val = re15_title_pulse_value(), ctr = re15_title_pulse_counter();
        re15_title_pulse_reset();
        for (uint64_t k = 0; k < due; k++) re15_title_pulse_step();
        printf("  Stillstand 100 s: %llu Durchgaenge faellig, Zustand (%d, 0x%02x), Einzelschritte (%d, 0x%02x)\n",
               (unsigned long long) due, ctr, (unsigned) val, re15_title_pulse_counter(), (unsigned) re15_title_pulse_value());
        CHECK(due == 2991ull, "100 s = %llu Durchgaenge statt 2991", (unsigned long long) due);
        CHECK(val == re15_title_pulse_value() && ctr == re15_title_pulse_counter(), "Stillstand: Zustand weicht ab");
    }
}

/* ---------------------------------------------------------------- Teil E ------------- */
/* Einblende gegen die AUSGEFUEHRTEN Bytes. Fade-Kanal 0 liegt @0x800b5458 (Kanal-Tabelle,
 * 0x44 Byte je Kanal: `addiu v1,v1,0x5458` @0x800216fc, `addiu s0,s0,21592` @0x8002188c,
 * Schrittweite `addiu s0,s0,68` @0x800219e0). Layout, wie es die drei Funktionen benutzen:
 *   +0 Pegel (u16)  +2 Schritt (s16, `sh a1,2(s0)` @0x800217e4)  +4 Halbtransparenz-Art
 *   (`sb a2,4(s0)` @0x800217e8, a2 = a0 >> 8)  +5/+6/+7 Farbmasken R/G/B  +8 OT-Ebene
 *   +12 + puf*16 Rechteck (Farbe R/G/B bei +16/+17/+18, `sb` @0x800218e8/00/18),
 *   puf = Doppelpuffer-Index DAT_800aca34 (`lbu v1,0(s2)` @0x800218d4). */
#define FADE_CH0   0x800b5458u
#define DBUF_IDX   0x800aca34u

static int part_e(const char *tp, const char *ep)
{
    printf("[E] Titel-Einblende gegen die ausgefuehrten Bytes (TITLE.BIN 0x80102054-7c, PSX.EXE)\n");
    int rc = load_title(tp);       /* frisch: Teil A hat die Datenworte 0x2944/46 veraendert */
    if (rc) return rc;
    rc = load_exe(ep);
    if (rc) return rc;
    static uint8_t stack[STACK_SIZE];
    memset(stack, 0, sizeof stack);
    s_seg[SEG_STACK].base = STACK_BASE; s_seg[SEG_STACK].d = stack; s_seg[SEG_STACK].n = STACK_SIZE;

    /* Die Befehlsworte, auf die sich Kommentar und Code stuetzen, genau so in den Dateien? */
    static const struct { uint32_t a, w; const char *s; } W[] = {
        { 0x80102054u, 0x34040200u, "ori a0,zero,0x200    (Kanal 0, Art 2 = subtraktiv)" },
        { 0x80102058u, 0x3405fc00u, "ori a1,zero,0xfc00   (Schritt -0x400)" },
        { 0x8010205cu, 0x34060007u, "ori a2,zero,0x7      (Masken R|G|B)" },
        { 0x80102060u, 0x0c0085ecu, "jal 0x800217b0       (Aufbau)" },
        { 0x80102064u, 0x34070003u, "ori a3,zero,0x3" },
        { 0x80102078u, 0x0c0085bbu, "jal 0x800216ec       (Anstoss)" },
        { 0x800217e4u, 0xa6050002u, "sh a1,2(s0)          (Schritt speichern)" },
        { 0x80021710u, 0x28420001u, "slti v0,v0,1         (Schritt < 1 ?)" },
        { 0x80021714u, 0x00021023u, "subu v0,zero,v0" },
        { 0x80021718u, 0x30427fffu, "andi v0,v0,0x7fff    (-> Pegel 0x7fff)" },
        { 0x80021720u, 0xa4620000u, "sh v0,0(v1)          (Pegel speichern)" },
        { 0x800218c8u, 0x00021400u, "sll v0,v0,16" },
        { 0x800218ccu, 0x04400041u, "bltz v0,0x800219d4   (Pegel negativ -> nichts zeichnen)" },
        { 0x800218d0u, 0x000225c3u, "sra a0,v0,23         (Farbe = Pegel >> 7)" },
        { 0x80021928u, 0x00431021u, "addu v0,v0,v1        (Pegel += Schritt, NACH der Farbe)" },
        { 0x80020f44u, 0x0c008620u, "jal 0x80021880       (Hauptschleife: 1 Takt je Durchgang)" },
    };
    int words_ok = 0;
    for (unsigned i = 0; i < sizeof W / sizeof W[0]; i++) {
        uint32_t got = m32(W[i].a);
        if (got == W[i].w) words_ok++;
        else CHECK(0, "@0x%08x = %08x, erwartet %08x (%s)", (unsigned) W[i].a, (unsigned) got, (unsigned) W[i].w, W[i].s);
    }
    printf("  %d von %u zitierten Befehlsworten stehen so in TITLE.BIN / PSX.EXE\n", words_ok,
           (unsigned) (sizeof W / sizeof W[0]));

    /* Titel-Init ausfuehren: 0x80102054 ... bis vor `jal 0x80029ac8` @0x80102080. */
    memset(R, 0, sizeof R);
    if (run_until(0x80102054u, 0x80102080u) != 0) return 2;
    int16_t step0  = (int16_t) m16(FADE_CH0 + 2);
    uint32_t lvl0  = m16(FADE_CH0);
    uint32_t mode0 = m8(FADE_CH0 + 4);
    uint32_t mk_r = m8(FADE_CH0 + 5), mk_g = m8(FADE_CH0 + 6), mk_b = m8(FADE_CH0 + 7);
    printf("  nach der Titel-Init: Kanal 0 Pegel 0x%04x, Schritt %d, Art %u, Masken %02x/%02x/%02x\n",
           (unsigned) lvl0, (int) step0, (unsigned) mode0, (unsigned) mk_r, (unsigned) mk_g, (unsigned) mk_b);
    CHECK(lvl0 == 0x7fffu, "Pegel nach dem Anstoss 0x%04x statt 0x7fff", (unsigned) lvl0);
    CHECK(step0 == -0x400, "Schritt %d statt -0x400", (int) step0);
    CHECK(mode0 == 2, "Halbtransparenz-Art %u statt 2 (subtraktiv = re15_render_pc_title_fade_sub)", (unsigned) mode0);
    CHECK(mk_r == 0xff && mk_g == 0xff && mk_b == 0xff, "Farbmasken nicht 0xff");

    /* Je Durchgang FUN_80021880 ausfuehren und die Farbe des Rechtecks von Kanal 0 lesen. */
    int same = 0, n = 0, first_zero = -1, rgb_ok = 1;
    for (uint64_t tick = 0; tick < 40; tick++, n++) {
        memset(R, 0, sizeof R);
        s_addprim_n = 0;
        if (run(0x80021880u) != 0) return 2;
        uint32_t buf  = m8(DBUF_IDX);
        uint32_t prim = FADE_CH0 + 12u + buf * 16u;          /* AddPrim(ot, a1 = Kanal+12+puf*16) */
        int drawn = 0;
        for (int k = 0; k < s_addprim_n; k++) if (s_addprim_a1[k] == prim) drawn = 1;
        int orig = 0;
        if (drawn) {
            uint32_t r = m8(prim + 4), g = m8(prim + 5), b = m8(prim + 6);
            if (r != b || g != b) rgb_ok = 0;
            orig = (int) b;
        }
        int port = re15_title_fadein_level(tick);
        if (orig == port) same++;
        else printf("  Durchgang %llu: Original %d (%s), Port %d\n", (unsigned long long) tick, orig,
                    drawn ? "gezeichnet" : "nicht gezeichnet", port);
        if (port == 0 && first_zero < 0) first_zero = (int) tick;
    }
    printf("  %d von %d Durchgaengen gleich (Original ausgefuehrt); Einblende fertig ab Durchgang %d = %.1f ms\n",
           same, n, first_zero, first_zero * 2000.0 * 1000.0 / (double) RE15_TITLE_VBLANK_MILLIHZ);
    CHECK(same == n, "Einblende: %d von %d gleich", same, n);
    CHECK(rgb_ok, "Original schreibt R, G, B verschieden");
    CHECK(first_zero == 32, "Einblende fertig ab Durchgang %d statt 32", first_zero);
    CHECK(re15_title_fadein_level(1ull << 40) == 0, "Einblende bei grossem Tick nicht 0");
    return 0;
}

int main(int argc, char **argv)
{
    const char *tp = (argc > 1) ? argv[1] : RE15_ASSET_PSX_DIR "/BIN/TITLE.BIN";
    const char *ep = (argc > 2) ? argv[2] : RE15_R30_PSX_EXE;
    int rc = part_a(tp);
    if (rc) return rc;
    part_b();
    part_c();
    part_d();
    rc = part_e(tp, ep);
    if (rc) return rc;
    printf("\nERGEBNIS: %s\n", s_fail ? "ROT" : "GRUEN");
    return s_fail ? 1 : 0;
}
