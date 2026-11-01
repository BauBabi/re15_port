"""wa_tabelle.py - Abnahme W1: Tabelle ueber alle Laeufe. Blitz-Ereignisse (wa_blitz.py) werden der Figur zugeordnet, wenn
sie im Figurbereich liegen: befund.log-Kasten des Laufs (Huefte..Fuss) erweitert um x +-12, y -45..+8 (Kopf/Arme)."""
import os, re, sys, glob
SP = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
zeilen = []
for d in sorted(glob.glob(os.path.join(SP, "runs", "*"))):
    name = os.path.basename(d)
    if not os.path.exists(os.path.join(d, "blitz.txt")): continue
    bl = open(os.path.join(d, "blitz.txt")).read().splitlines()
    bef = open(os.path.join(d, "befund_lauf.log"), encoding="utf-8", errors="replace").read()
    dbg = open(os.path.join(d, "debug.log"), errors="replace").read() if os.path.exists(os.path.join(d, "debug.log")) else ""
    raum = (re.findall(r"loaded room(\w+)\.rdt", dbg) or ["?"])[-1].upper()
    # Kaesten nur aus Zeilen des Zielraums
    ks = [tuple(map(int, m.groups())) for m in re.finditer(r"R%s C\d+ .*?kasten=x(-?\d+)\.\.(-?\d+),y(-?\d+)\.\.(-?\d+)" % raum, bef)]
    cut = (re.findall(r"R%s C(\d+)" % raum, bef) or ["?"])[-1]
    fam = (re.findall(r"Spieler-Familie (PL\d+)", dbg) or ["?"])[-1]
    wl = "ja" if "[wasserlauf41] ROOM" in dbg else "nein"
    figur, rest = 0, []
    for t in bl[1:]:
        m = re.search(r"x(\d+)\.\.(\d+) y(\d+)\.\.(\d+)", t)
        x0, x1, y0, y1 = map(int, m.groups())
        if any(not (x1 < k[0] - 12 or x0 > k[1] + 12 or y1 < k[2] - 45 or y0 > k[3] + 8) for k in ks): figur += 1
        else: rest.append(t.strip())
    kopf = re.match(r"bilder=(\d+) ab=([\d.]+)s wechsel_gt8=(\d+) von (\d+) blitz_ereignisse=(\d+)", bl[0]).groups()
    zeilen.append((name, raum, cut, fam, kopf[0], "%s/%s" % (kopf[2], kopf[3]), kopf[4], figur, len(rest), wl, rest))
print("| Lauf | Raum | Cut (befund.log) | Figur | Bilder | Wechsel>8 ganzes Bild | Blitz-Ereignisse | davon an der Figur | **sonstige** | wasserlauf41 |")
print("|---|---|---|---|---|---|---|---|---|---|")
for z in zeilen:
    print("| %s | %s | %s | %s | %s | %s | %s | %d | **%d** | %s |" % z[:10])
print()
for z in zeilen:
    if z[10]:
        print("%s sonstige:" % z[0])
        for t in z[10][:8]: print("   ", t)
