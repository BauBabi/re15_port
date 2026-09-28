/* test_r30_titel_blinken.c - RIEGEL Runde 30 / Thema D
 * "Im Titelbild ... blinkt der ausgewaehlte Bereich in der Frequenz im Vergleich zum
 *  Original zu schnell."
 * Dossier: analysis/befunde_runde30/titel-blinken.md
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
 * Teil E  EINBLENDE: re15_title_fadein_level() gegen den nachgebauten Integrator
 *         FUN_80021880 (Pegel 0x7fff, Schritt 0xfc00).
 * Teil F  GEGENPROBE: das ALTE Verhalten (ein Schritt je Schleifendurchgang) faellt an
 *         Teil D — der Riegel ist also nicht von selbst gruen.
 *
 * Rueckgabe 0 = alles gruen, 1 = Abweichung, 2 = Datei nicht lesbar.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_title_pulse.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

#define TITLE_BASE 0x80100000u

static uint8_t *s_title; static long s_title_n;
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
static uint32_t tw(uint32_t a)
{
    long o = (long) (a - TITLE_BASE);
    return (uint32_t) s_title[o] | ((uint32_t) s_title[o + 1] << 8) | ((uint32_t) s_title[o + 2] << 16) | ((uint32_t) s_title[o + 3] << 24);
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
        if (pc < TITLE_BASE || (long) (pc - TITLE_BASE) + 4 > s_title_n) { printf("PC ausserhalb der Datei: %08x\n", (unsigned) pc); return -1; }
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

/* ---------------------------------------------------------------- Teil A ------------- */
static int part_a(const char *tp)
{
    printf("[A] Pulsfolge: Port gegen FUN_801028ec aus %s\n", tp);
    s_title = slurp(tp, &s_title_n);
    if (!s_title) { printf("FEHLER: %s nicht lesbar\n", tp); return 2; }
    if (s_title_n < 0x2948) { printf("FEHLER: Datei zu kurz (%ld)\n", s_title_n); return 2; }

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
    free(s_title); s_title = NULL;
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
static void part_e(void)
{
    printf("[E] Titel-Einblende gegen den Integrator FUN_80021880\n");
    /* Nachbau: Pegel u16 @+0, Schritt s16 @+2. Anstoss FUN_800216ec @0x80021710-20: 0x7fff.
     * Je Durchgang: negativ (Bit 15) -> nichts; sonst Farbe = (s16) Pegel >> 7, dann
     * Pegel += Schritt (0xfc00 @0x80102058). */
    uint16_t level = 0x7fff; const uint16_t step = 0xfc00;
    int same = 0, n = 0, first_zero = -1;
    for (uint64_t tick = 0; tick < 40; tick++, n++) {
        int orig;
        if (level & 0x8000u) orig = 0;
        else { orig = (int) ((int32_t) ((uint32_t) level << 16) >> 23); level = (uint16_t) (level + step); }
        int port = re15_title_fadein_level(tick);
        if (orig == port) same++;
        else printf("  Durchgang %llu: Original %d, Port %d\n", (unsigned long long) tick, orig, port);
        if (port == 0 && first_zero < 0) first_zero = (int) tick;
    }
    printf("  %d von %d Durchgaengen gleich; Einblende fertig ab Durchgang %d = %.1f ms\n", same, n, first_zero,
           first_zero * 2000.0 * 1000.0 / (double) RE15_TITLE_VBLANK_MILLIHZ);
    CHECK(same == n, "Einblende: %d von %d gleich", same, n);
    CHECK(first_zero == 32, "Einblende fertig ab Durchgang %d statt 32", first_zero);
    CHECK(re15_title_fadein_level(0) == 255 && re15_title_fadein_level(1) == 247 && re15_title_fadein_level(31) == 7,
          "Einblende-Stufen 0/1/31 = %d/%d/%d statt 255/247/7", re15_title_fadein_level(0),
          re15_title_fadein_level(1), re15_title_fadein_level(31));
    CHECK(re15_title_fadein_level(1ull << 40) == 0, "Einblende bei grossem Tick nicht 0");
}

/* ---------------------------------------------------------------- Teil F ------------- */
static void part_f(void)
{
    printf("[F] Gegenprobe: das ALTE Verhalten (1 Schritt je Schleifendurchgang) an Teil D\n");
    /* alter Stand: render_pc.c zaehlte je Aufruf -> bei 144 Hz 1440 Schritte in 10 s */
    const uint64_t dauer = 10000000ull, soll = re15_title_tick_count(dauer);
    uint64_t alt = 0;
    for (uint64_t t = 0; t <= dauer; t += 6944ull) alt++;
    printf("  alt bei 144 Hz: %llu Schritte in 10 s, Soll %llu -> der Riegel %s\n", (unsigned long long) alt,
           (unsigned long long) soll, (alt != soll) ? "FAELLT am alten Stand (gut)" : "ist BLIND");
    CHECK(alt != soll, "Gegenprobe: der Riegel unterscheidet alt und neu nicht");
}

int main(int argc, char **argv)
{
    const char *tp = (argc > 1) ? argv[1] : RE15_ASSET_PSX_DIR "/BIN/TITLE.BIN";
    int rc = part_a(tp);
    if (rc) return rc;
    part_b();
    part_c();
    part_d();
    part_e();
    part_f();
    printf("\nERGEBNIS: %s\n", s_fail ? "ROT" : "GRUEN");
    return s_fail ? 1 : 0;
}
