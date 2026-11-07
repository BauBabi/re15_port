"""fuellen6.py <bericht.md> <ctest_exe.txt> <ctest_exe_zeit.txt>: setzt die Riegel-Platzhalter des Berichts aus der fertigen ctest-Ausgabe (nur wenn alle 20 gruen)."""
import re, sys
p, ct, zt = sys.argv[1:4]
s = open(p, encoding="utf-8").read()
t = open(ct, errors="replace").read()
z = open(zt, errors="replace").read()
summe = re.search(r"^(\d+% tests passed, \d+ tests failed out of \d+)$", t, re.M).group(1)
total = re.search(r"^(Total Test time \(real\) = +[\d.]+ sec)$", t, re.M).group(1)
failed = re.findall(r"^\s+(\d+ - integration\S+ \(\w+\))$", t, re.M)
rwe = re.search(r"Test +#\d+: integration_r41_gatorfix_abn2_rampe_west_elza \.+\s+(\w+)\s+([\d.]+) sec", t)
start = re.search(r"start (\S+)", z).group(1)
ende = re.search(r"ende (\S+)", z)
ende = ende.group(1) if ende else "?"
alle_gruen = summe.startswith("100%")
rep = {
    "{{EXE_ZEIT}}": "%s-%s" % (start, ende),
    "{{EXE_SUMME}}": "**`%s`**, `%s`." % (summe, total) + ("" if not failed else " `The following tests FAILED:` " + "; ".join("`%s`" % f for f in failed)),
    "{{RWE_ZEIT}}": "%s s (%s)" % (rwe.group(2).replace(".", ","), rwe.group(1)) if rwe else "?",
    "{{EXE_ROT}}": ("Kein roter Riegel - nichts einzeln nachzufahren. abn2_rampe_west_elza (in der Gruppe des Bauers rot durch den Auswerter, Kletterbilder als"
                    " \"Deck verlassen\") ist hier gruen; mein Auswerter auf dem damals roten Lauf `lauf_5e76769d` (mit/ohne): nie ausserhalb, 91 Kletterbilder,"
                    " 98 Bilder oben.") if alle_gruen else "ROTE RIEGEL - siehe oben, einzeln nachfahren.",
    "{{EXE_ZEILE}}": "`%s`, `%s`" % (summe, total),
    "{{EXE_BEW}}": "gruen" if alle_gruen else "ROT",
    "{{URTEIL_EXE}}": ("20/20 gruen (`%s`)" % summe) if alle_gruen else ("ROT: `%s`" % summe),
    "{{TESTS_EXE}}": "`ctest --test-dir \"<Baum>/re15_port/build\" -R \"^integration_r41_gatorfix_|^integration_r41_re2gator\" -j 2 --timeout 1800 -V` (%s-%s, `ctest_exe.txt`): **`%s`**, `%s`." % (start, ende, summe, total),
}
for k, v in rep.items():
    s = s.replace(k, v)
open(p, "w", encoding="utf-8").write(s)
print("alle_gruen", alle_gruen, "|", summe, "|", total, "| rest", re.findall(r"\{\{[A-Z_]+\}\}", s))
