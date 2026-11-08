# merge_pruef.py <repo> <merge> : prueft je Datei, ob jede Abweichung Spurspitze -> Merge-Ergebnis von der Integrationsseite stammt.
# P1 = Integrationsseite, P2 = Spur, B = merge-base. Fuer jede Datei F (geaendert in B..P2):
#   '+'-Zeilen von diff(P2, M) muessen '+'-Zeilen von diff(B, P1) sein (die Integration hat sie gebracht),
#   '-'-Zeilen von diff(P2, M) muessen '-'-Zeilen von diff(B, P1) sein (die Integration hat sie entfernt).
# Rest = "unerklaert" (Konfliktloesung oder verlorene Spurzeile) -> ausgeben.
import subprocess, sys, collections
repo, M = sys.argv[1], sys.argv[2]
def git(*a, binary=False):
    r = subprocess.run(["git", "-C", repo] + list(a), capture_output=True)
    return r.stdout if binary else r.stdout.decode("utf-8", "replace")
P1, P2 = git("rev-parse", M + "^1").strip(), git("rev-parse", M + "^2").strip()
B = git("merge-base", P1, P2).strip()
def zeilen(a, b, f):
    plus, minus = collections.Counter(), collections.Counter()
    for z in git("diff", "-U0", "--no-color", "--no-ext-diff", a, b, "--", f).splitlines():
        if z.startswith("+++") or z.startswith("---"): continue
        if z.startswith("+"): plus[z[1:].rstrip("\r")] += 1
        elif z.startswith("-"): minus[z[1:].rstrip("\r")] += 1
    return plus, minus
spur = [f for f in git("diff", "--name-only", B, P2).splitlines() if f]
integ = set(f for f in git("diff", "--name-only", B, P1).splitlines() if f)
print("Merge %s: P1 %s (Integration), P2 %s (Spur), Basis %s" % (M[:9], P1[:9], P2[:9], B[:9]))
print("Dateien der Spur: %d, davon auch auf der Integrationsseite geaendert: %d" % (len(spur), len(integ & set(spur))))
gesamt_unerkl = 0; spur_hunks = 0
for f in spur:
    # Spur-Hunks zaehlen
    spur_hunks += sum(1 for z in git("diff", "-U0", B, P2, "--", f).splitlines() if z.startswith("@@"))
    nt = git("diff", "--numstat", P2, M, "--", f).strip()
    if not nt:
        continue                      # Ergebnis = Spurstand
    if nt.startswith("-\t-\t"):       # Binaerdatei
        same_int = git("rev-parse", "%s:%s" % (M, f)).strip() == git("rev-parse", "%s:%s" % (P1, f)).strip()
        print("BINAER %s: Ergebnis != Spur, = Integrationsseite: %s" % (f, same_int)); gesamt_unerkl += (0 if same_int else 1); continue
    p, m = zeilen(P2, M, f)
    ip, im = zeilen(B, P1, f)
    up = p - ip; um = m - im
    if up or um:
        gesamt_unerkl += sum(up.values()) + sum(um.values())
        print("== %s: unerklaert +%d / -%d (Abweichung Spur->Ergebnis +%d/-%d)" % (f, sum(up.values()), sum(um.values()), sum(p.values()), sum(m.values())))
        for z, n in list(up.items())[:12]: print("   + (%dx) %s" % (n, z[:170]))
        for z, n in list(um.items())[:12]: print("   - (%dx) %s" % (n, z[:170]))
    else:
        print("ok %s: Abweichung Spur->Ergebnis +%d/-%d, alles von der Integrationsseite" % (f, sum(p.values()), sum(m.values())))
print("Spur-Hunks (B..P2): %d; unerklaerte Zeilen gesamt: %d" % (spur_hunks, gesamt_unerkl))
