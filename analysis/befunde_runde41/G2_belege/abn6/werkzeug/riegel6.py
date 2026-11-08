"""riegel6.py <ctest_exe.txt>: je exe-Riegel Ergebnis + Zeit und die Kernzahlen seiner Statuszeile (aussenlog, obenlog, haenglog, tragvergleich) - fuer Abschnitt 2."""
import re, sys
t = open(sys.argv[1], errors="replace").read()
res = {}
for m in re.finditer(r"^ *\d+/20 Test +#\d+: (integration_\S+) \.+\s+(\*{0,3}\w+)\s+([\d.]+) sec", t, re.M):
    res[m.group(1)] = (m.group(2), m.group(3))
stat = {}
for m in re.finditer(r"^\d+: -- (r41_gatorfix \S+?|r41_re2gator)[: ](.*)$", t, re.M):
    stat.setdefault(m.group(1).replace("r41_gatorfix ", "").replace("r41_re2gator", "re2gator"), []).append(m.group(2))
for name, (r, s) in sorted(res.items(), key=lambda kv: kv[0]):
    kurz = name.replace("integration_r41_gatorfix_", "").replace("integration_r41_", "")
    zeilen = " ".join(stat.get(kurz, []))
    teile = []
    for pat in (r"aussenlog: \d+ Bilder[^;]*; [^;]*aussen \d+, Deck verlassen \d+, im Block \d+",
                r"erster Treffer oben nach \d+ Bildern[^;]*; laengste Pause ohne Treffer oben \d+ Bilder",
                r"Haenge-Fenster \d+[^;]*; ohne Fortschritt max \d+ Bilder[^;]*; Treffer \d+; Zeilen HAENGT \d+, RUECKWAERTS an \d+, TAUCHEN an \d+",
                r"tragvergleich: [^;]*abweichend \d+", r"Kanister-Sitz \d+ Bilder, Abweichung max \d+", r"^ ?ok\b",
                r"kanister: \d+ angehaengte Bilder, Abweichung max \d+", r"belagerung: GIER-WACHE bis F\d+: \d+ Bildpaare"):
        f = re.findall(pat, zeilen)
        if f:
            teile.append(f[0] if len(set(f)) == 1 else " / ".join(sorted(set(f))))
    print("%-34s %-8s %8s s | %s" % (kurz, r, s, " | ".join(teile)[:900]))
s = re.findall(r"^\d+% tests passed.*$|^Total Test time.*$|^The following tests FAILED.*$|^\s+\d+ - integration\S+ \(\w+\)$", t, re.M)
print("\n".join(s))
