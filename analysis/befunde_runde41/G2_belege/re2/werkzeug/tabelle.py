import re, sys
# tabelle.py <riegel-ausgabe re2> - Markdown-Zeilen je Lage: nb5 | neu
rx = re.compile(r"^    (nb5|neu) (\S+)\s+\d+ B\.: ohne Fortschritt max\s+([\d.]+) s, 2-s-Fenster\s+(\d+), Drehen\s+(\d+) %, rueckw\.\s+(\d+) % \(\+Rueckzug\s+(\d+) %\), Tauch (\d+) \(([\d.]+)/min\) unter\s+(\d+) %, Block\s+(\d+) %, Kam<2500\s+(\d+) %, hinter\s+(\d+) %, Kopf\s+(\d+), Rammen (\d+)/Querung (\d+).*Takt (\d+)/(\d+)/(\d+) us")
d = {}
for l in open(sys.argv[1], errors="replace"):
    m = rx.match(l)
    if m: d.setdefault(m.group(2), {})[m.group(1)] = m.groups()
print("| Lage | ohne Fortschritt s | 2-s-Fenster | Drehen % | rueckw. % (+Rueckzug) | Tauchg./min, unter % | Kopf/Kan. in Rampe/Plattform % | < 2500 / hinter Kamera % | Rammen | Takt us Mittel/95%/max |")
print("|---|---|---|---|---|---|---|---|---|---|")
for k, v in d.items():
    a, n = v.get("nb5"), v.get("neu")
    if not a or not n: continue
    f = lambda i: "%s / %s" % (a[i], n[i])
    print("| %s | %s | %s | %s | %s / %s (+%s) | %s/%s, %s/%s | %s | %s/%s / %s/%s | %s / %s | %s/%s/%s / %s/%s/%s |" % (k, f(2), f(3), f(4), a[5], n[5], n[6], a[8], n[8], a[9], n[9], f(10), a[11], n[11], a[12], n[12], a[14], n[14], a[16], a[17], a[18], n[16], n[17], n[18]))
