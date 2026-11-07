#!/usr/bin/env python3
"""vergleich5.py - Abnahme 5 Spur AN: zwei state.log Zeile fuer Zeile vergleichen (nur die F-Zeilen, in Reihenfolge).
Ausgabe: Zeilen gesamt/gleich/anders, erste und letzte abweichende Zeile, Bildspannen der Abweichungen und welche Teile abweichen
(PL = Spielerteil, [n t=..] = Akteur n).
  python vergleich5.py <alt/state.log> <neu/state.log> [max_beispiele=4]"""
import sys, re
a = [l.rstrip("\n") for l in open(sys.argv[1], errors="replace") if l.startswith("F")]
b = [l.rstrip("\n") for l in open(sys.argv[2], errors="replace") if l.startswith("F")]
nb = int(sys.argv[3]) if len(sys.argv) > 3 else 4
n = min(len(a), len(b))
akt = re.compile(r"\[(\d+) t=([0-9a-f]+) [^\]]*\][^\[]*")
def teile(l):
    d = {"PL": l.split(" [", 1)[0]}
    for m in akt.finditer(l): d["A%s/t%s" % (m.group(1), m.group(2))] = m.group(0).strip()
    return d
anders, spannen, wer = [], [], {}
abschnitt = 0; last = -1; absch_a = []
for i in range(n):
    f = int(a[i].split(" ", 1)[0][1:])
    if f < last: abschnitt += 1
    last = f
    if a[i] != b[i]:
        anders.append((i, abschnitt, f))
        ta, tb = teile(a[i]), teile(b[i])
        for k in set(ta) | set(tb):
            if ta.get(k) != tb.get(k): wer[k] = wer.get(k, 0) + 1
print("alt %d Zeilen, neu %d Zeilen, verglichen %d: gleich %d, anders %d" % (len(a), len(b), n, n - len(anders), len(anders)))
if anders:
    # Spannen je Abschnitt
    cur = None
    for i, s, f in anders:
        if cur and cur[0] == s and f == cur[2] + 1: cur[2] = f
        else:
            if cur: spannen.append(cur)
            cur = [s, f, f]
    spannen.append(cur)
    print("abweichende Spannen (Abschnitt: F von-bis):", ", ".join("%d:F%d-F%d" % tuple(x) for x in spannen[:20]), "..." if len(spannen) > 20 else "")
    print("abweichende Teile (Anzahl Zeilen):", ", ".join("%s %d" % kv for kv in sorted(wer.items(), key=lambda kv: -kv[1])))
    for i, s, f in anders[:nb]:
        print("  alt:", a[i][:400]); print("  neu:", b[i][:400])
