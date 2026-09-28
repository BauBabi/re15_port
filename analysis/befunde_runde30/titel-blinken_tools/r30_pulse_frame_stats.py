#!/usr/bin/env python3
"""r30_pulse_frame_stats.py - Runde 30 / Thema D, ABNAHME nach dem Umbau.

Wertet die Messschiene RE15_TITLE_PULSE_LOG der UNVERAENDERTEN re15_pc.exe aus
(platform/pc/main.c, pc_title_pulse_log). Zeile je BILD des Titelmenues:
    <zeit_us> <zaehler> <pulswert> <faellige_durchgaenge> <phase> <einblende_tick> <einblende_B>
phase 0 = Titel-Schleife, 1 = Bestaetigungs-Fade.

Anders als r30_pulse_log_stats.py (eine Zeile je PULSSCHRITT, alter Stand) traegt hier jedes
Bild eine Zeile; ein Pulsschritt ist ein Bild mit faellige_durchgaenge > 0.

Ausgabe:
  - Bildrate der Titel-Schleife (= Anzeige mit VSync, frei ohne)
  - Pulsschritte: Zahl, Abstand, hoechste Zahl je Bild
  - PULSPERIODE: Abstand der Ruecksetzungen (Zaehler wird 0), je Periode die Schrittzahl
  - mittlere Periode aus der Schrittrate (unabhaengig von der Bild-Quantisierung)
  - EINBLENDE: Dauer vom ersten Bild bis zum ersten Bild mit B = 0, Stufen, Haltedauer
  - Fade: Bilder, Dauer, Zahl der Aenderungen des Pulswerts

Aufruf: r30_pulse_frame_stats.py <log> [soll_periode_ms]
        r30_pulse_frame_stats.py --alt <log>      alter Stand (Messkopie r30_build_instr.sh,
                                                   eine Zeile je Aufruf des Zeichners =
                                                   je Bild): Dauer der Einblende, die dort an
                                                   tblink hing (B = 255 - (tblink>>1)*8,
                                                   B = 0 ab tblink = 64)
Rueckgabe 0 = Periode im Soll (+/- 1 Bild), 60 Schritte je Periode, Einblende im Soll,
Fade ohne Pulsaenderung.
"""
import sys, statistics

T_TICK_MS = 2.0 * 1000.0 / 59.826          # 2 VBlanks @0x8002130c-14, 59,826 Hz psx-spx
FADEIN_TICKS = 32                          # 0x7fff / 0x400 + 1: Schritt 0xfc00 @0x80102058

def alt(path):
    t = [int(l.split()[0]) for l in open(path) if l.strip()]
    if len(t) < 66:
        print("zu wenig Bilder"); return 2
    d = (t[64] - t[0]) / 1000.0
    print("ALTER STAND, Titel-Einblende (B = 255 - (tblink>>1)*8):")
    print("  Bild 0 -> Bild 64 (erstes Bild mit B = 0): %.1f ms" % d)
    print("  Bildabstand Bilder 1..64: Mittel %.3f ms" % ((t[64] - t[1]) / 63000.0))
    print("  Original: %d Durchgaenge x %.3f ms = %.1f ms" % (FADEIN_TICKS, T_TICK_MS, FADEIN_TICKS * T_TICK_MS))
    return 0

def main():
    if len(sys.argv) > 2 and sys.argv[1] == "--alt":
        return alt(sys.argv[2])
    rows = []
    for l in open(sys.argv[1]):
        p = l.split()
        if len(p) >= 5:
            v = [int(x) for x in p[:7]]
            while len(v) < 7: v.append(-1)
            rows.append(tuple(v))
    soll = float(sys.argv[2]) if len(sys.argv) > 2 else 60.0 * T_TICK_MS
    n_title = sum(1 for r in rows if r[4] == 0)
    n_fade = sum(1 for r in rows if r[4] == 1)
    fail = 0
    print("Bilder gesamt          : %d  (Titel-Schleife %d, Bestaetigungs-Fade %d)" % (len(rows), n_title, n_fade))
    if n_title < 3:
        print("zu wenig Bilder"); return 2
    # zusammenhaengende Abschnitte gleicher Phase
    seg, fseg = [], []
    cur, cur_ph = [], None
    for r in rows:
        if cur and r[4] != cur_ph:
            (seg if cur_ph == 0 else fseg).append(cur); cur = []
        cur.append(r); cur_ph = r[4]
    if cur: (seg if cur_ph == 0 else fseg).append(cur)
    print("Titel-Abschnitte       : %d  (Bilder je Abschnitt: %s)" % (len(seg), ", ".join(str(len(s)) for s in seg)))

    dt = []
    for s in seg:
        dt += [(b[0] - a[0]) / 1000.0 for a, b in zip(s, s[1:])]
    print("Bildabstand   Median   : %.3f ms   Mittel %.3f ms   min %.3f   max %.3f   -> %.2f Bilder/s" % (
        statistics.median(dt), statistics.mean(dt), min(dt), max(dt), 1000.0 / statistics.mean(dt)))

    all_per, all_n, all_tol, all_step_dt, max_due, n_steps = [], [], [], [], 0, 0
    rate_num, rate_den = 0.0, 0
    vals = set()
    for s in seg:
        # je Schritt zusaetzlich: Dauer des Bildes, in dem er faellig wurde (= Quantisierung:
        # der Schritt wird erst am Bildanfang ausgefuehrt, faellig war er irgendwann davor)
        steps = [(r[0], r[1], r[2], r[3], (r[0] - s[i - 1][0]) / 1000.0 if i else 0.0)
                 for i, r in enumerate(s) if r[3] > 0]
        for r in s: vals.add(r[2])
        n_steps += sum(x[3] for x in steps)
        if steps: max_due = max(max_due, max(x[3] for x in steps))
        all_step_dt += [(b[0] - a[0]) / 1000.0 for a, b in zip(steps, steps[1:])]
        if len(steps) > 1:
            rate_num += (steps[-1][0] - steps[0][0]) / 1000.0
            rate_den += sum(x[3] for x in steps[1:])
        # Ruecksetzungen: Schritt, nach dem der Zaehler 0 ist
        resets = [i for i, x in enumerate(steps) if x[1] == 0]
        for a, b in zip(resets, resets[1:]):
            all_per.append((steps[b][0] - steps[a][0]) / 1000.0)
            all_n.append(sum(x[3] for x in steps[a + 1:b + 1]))
            all_tol.append(max(steps[a][4], steps[b][4]))
    print("Pulsschritte           : %d   hoechstens %d je Bild" % (n_steps, max_due))
    if all_step_dt:
        print("Schrittabstand Median  : %.3f ms   Mittel %.3f ms   min %.3f   max %.3f" % (
            statistics.median(all_step_dt), statistics.mean(all_step_dt), min(all_step_dt), max(all_step_dt)))
    if rate_den:
        tick = rate_num / rate_den
        print("Schrittdauer (Mittel)  : %.4f ms  = %.4f Schritte/s   -> 60 Schritte = %.2f ms" % (
            tick, 1000.0 / tick, 60.0 * tick))
    print("Pulswerte              : min 0x%02x max 0x%02x, %d verschiedene" % (min(vals), max(vals), len(vals)))
    if not all_per:
        print("KEINE volle Periode im Log")
        if n_fade == 0: fail = 1
    else:
        print("PULSPERIODE            : Median %.1f ms   Mittel %.1f ms   min %.1f   max %.1f   (n=%d)" % (
            statistics.median(all_per), statistics.mean(all_per), min(all_per), max(all_per), len(all_per)))
        print("Schritte je Periode    : %s" % sorted(set(all_n)))
        # Toleranz je Periode = 1 Bild: das laengere der beiden Bilder, in denen die
        # begrenzenden Ruecksetzungen ausgefuehrt wurden (bei VSync = Bildabstand der Anzeige)
        out = [(p, t) for p, t in zip(all_per, all_tol) if abs(p - soll) > t]
        worst = max(abs(p - soll) for p in all_per)
        print("SOLL                   : %.1f ms +/- 1 Bild (Bilddauer an den Periodengrenzen %.2f ... %.2f ms)" % (
            soll, min(all_tol), max(all_tol)))
        print("                         groesste Abweichung %.2f ms; Perioden ausserhalb: %d von %d   -> %s" % (
            worst, len(out), len(all_per), "IM SOLL" if not out else "AUSSERHALB"))
        for p, t in out:
            print("                         Periode %.2f ms, Bilddauer %.2f ms" % (p, t))
        if out: fail = 1
        if sorted(set(all_n)) != [60]: fail = 1
        if rate_den:
            # Mittel ueber alle Schritte: Quantisierung nur an den beiden Enden der Messung,
            # also hoechstens 1 Bild auf rate_den Schritte -> auf 60 Schritte umgerechnet
            mean_per = 60.0 * rate_num / rate_den
            bound = 60.0 * max(dt) / rate_den
            ok = abs(mean_per - soll) <= bound
            print("MITTLERE PERIODE       : %.2f ms aus %d Schritten; Soll %.2f ms, Messgrenze +/- %.2f ms   -> %s" % (
                mean_per, rate_den, soll, bound, "IM SOLL" if ok else "AUSSERHALB"))
            if not ok: fail = 1

    # ---- Titel-Einblende: je Titel-Abschnitt vom ersten Bild bis zum ersten Bild mit B = 0
    if rows[0][6] >= 0:
        for k, s in enumerate(seg):
            if s[0][5] != 0 or s[0][6] != 255:
                continue                      # Abschnitt beginnt nicht mit einer Einblende
            z = next((i for i, r in enumerate(s) if r[6] == 0), None)
            if z is None:
                print("EINBLENDE Abschnitt %d  : endet nicht im Log (letztes B = %d)" % (k, s[-1][6])); continue
            dur = (s[z][0] - s[0][0]) / 1000.0
            tol = (s[z][0] - s[z - 1][0]) / 1000.0
            levels = []
            for r in s[:z + 1]:
                if not levels or levels[-1][0] != r[6]: levels.append([r[6], r[0], 1])
                else: levels[-1][2] += 1
            hold = [(b[1] - a[1]) / 1000.0 for a, b in zip(levels, levels[1:])]
            soll_f = FADEIN_TICKS * T_TICK_MS
            # jede Stufe liegt auf der Leiter 255 - 8*k; uebersprungene Stufen nur dort, wo in
            # EINEM Bild mehrere Durchgaenge faellig waren (Bildrate unter 30/s)
            skipped = sum(r[3] - 1 for r in s[1:z + 1] if r[3] > 1)
            ladder = all(r[6] == max(0, 255 - 8 * r[5]) if r[5] < FADEIN_TICKS else r[6] == 0 for r in s[:z + 1])
            ok = abs(dur - soll_f) <= tol and ladder and len(levels) + skipped >= FADEIN_TICKS + 1
            print("EINBLENDE Abschnitt %d  : %.1f ms bis B = 0 (Soll %d Durchgaenge = %.1f ms +/- 1 Bild %.2f ms)   -> %s" % (
                k, dur, FADEIN_TICKS, soll_f, tol, "IM SOLL" if ok else "AUSSERHALB"))
            print("                         %d Stufen (%d ... %d), Schritt %s, %d uebersprungen, Haltedauer Median %.2f ms (min %.2f, max %.2f)" % (
                len(levels) - 1, levels[0][0], levels[-2][0],
                sorted(set(a[0] - b[0] for a, b in zip(levels, levels[1:-1]))), skipped,
                statistics.median(hold), min(hold), max(hold)))
            if not ok: fail = 1

    # ---- Bestaetigungs-Fade: der Puls ruht
    idx_of = {}
    for i, r in enumerate(rows): idx_of.setdefault(r, i)
    for k, f in enumerate(fseg):
        chg = sum(1 for a, b in zip(f, f[1:]) if (a[1], a[2]) != (b[1], b[2]))
        fdt = [(b[0] - a[0]) / 1000.0 for a, b in zip(f, f[1:])]
        print("Fade %d                 : %d Bilder, Dauer %.2f s, Bildabstand Median %.2f ms" % (
            k, len(f), (f[-1][0] - f[0][0]) / 1e6, statistics.median(fdt) if fdt else 0.0))
        print("Pulswert im Fade       : %d Aenderungen (Original: 0)   Werte %s" % (
            chg, sorted(set("0x%02x" % r[2] for r in f))))
        i0 = idx_of[f[0]]
        if i0 > 0:
            before = rows[i0 - 1]
            same = (before[1], before[2]) == (f[0][1], f[0][2])
            print("Stand beim Bestaetigen : Titel (%d, 0x%02x) -> Fade (%d, 0x%02x)  %s" % (
                before[1], before[2], f[0][1], f[0][2], "gleich" if same else "VERSCHIEDEN"))
            if not same: fail = 1
        if chg: fail = 1
    # ---- Rueckkehr aus dem Unterbildschirm: der Puls setzt dort fort, wo er stand
    for k in range(1, len(seg)):
        prev_last = None
        i0 = idx_of[seg[k][0]]
        if i0 > 0: prev_last = rows[i0 - 1]
        if prev_last is None: continue
        first = seg[k][0]
        # beim ersten Bild nach der Rueckkehr duerfen hoechstens die dort faelligen Schritte dazukommen
        exp_ctr = (prev_last[1] + first[3]) % 60
        ok = first[1] == exp_ctr
        print("Rueckkehr Abschnitt %d  : vorher (%d, 0x%02x), danach (%d, 0x%02x), faellig %d, Pause %.2f s   -> %s" % (
            k, prev_last[1], prev_last[2], first[1], first[2], first[3], (first[0] - prev_last[0]) / 1e6,
            "Puls setzt fort" if ok else "Puls SPRINGT"))
        if not ok: fail = 1
    print("ERGEBNIS: %s" % ("ABWEICHUNG" if fail else "IM SOLL"))
    return 1 if fail else 0

if __name__ == "__main__":
    sys.exit(main())
