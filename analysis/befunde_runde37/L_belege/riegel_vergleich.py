"""Runde 37 Spur L: Riegel-Vergleich alt/neu aus zwei `ctest -V`-Mitschnitten (gleiche -R-Auswahl).
Je Test: Ergebnis (Passed/Failed) und die Ausgabezeilen; gemeldet wird jeder Test, dessen Ausgabe sich aendert (Pfade,
Zeiten und Scd-Spam ausgefiltert). Aufruf: python riegel_vergleich.py <alt_V.txt> <neu_V.txt> [max_zeilen_je_test]"""
import re, sys
from collections import defaultdict

RX_KOPF = re.compile(r"^\s*(\d+): Test command: .*?(test_[A-Za-z0-9_]+|probe_[A-Za-z0-9_]+)(?:\.exe)?\"?\s*(.*)$")
RX_NAME = re.compile(r"^\s*\d+/\d+ Test\s+#(\d+): (\S+) \.+.*?(Passed|Failed|\*\*\*Failed|Exception|Timeout)")
RX_ZEILE = re.compile(r"^(\d+): (.*)$")
RAUSCH = re.compile(r"\[scd\]|WARN|Test time|Test command|Working Directory|Environment variables|^\s*$|C:/|C:\\\\|Temp|scratchpad|"
                     r"Testing/|exe\b|seconds|sec\b|ms\b|\[re2z\]|thread start|Obj_model_set")


def lese(pfad):
    aus = defaultdict(list)
    erg = {}
    namen = {}
    for z in open(pfad, errors="replace"):
        z = z.rstrip("\n")
        m = RX_NAME.match(z)
        if m:
            namen[m.group(1)] = m.group(2)
            erg[m.group(2)] = "Passed" if m.group(3) == "Passed" else "Failed"
            continue
        m = RX_ZEILE.match(z)
        if m and not RAUSCH.search(m.group(2)):
            aus[m.group(1)].append(m.group(2))
    nach_name = {}
    for nr, name in namen.items():
        nach_name[name] = aus.get(nr, [])
    return nach_name, erg


def main():
    alt, ealt = lese(sys.argv[1])
    neu, eneu = lese(sys.argv[2])
    nmax = int(sys.argv[3]) if len(sys.argv) > 3 else 6
    gleich = 0
    for name in sorted(set(alt) | set(neu)):
        a, n = alt.get(name), neu.get(name)
        if a is None or n is None:
            print("== %s: nur %s" % (name, "neu" if a is None else "alt"))
            continue
        if a == n and ealt.get(name) == eneu.get(name):
            gleich += 1
            continue
        print("== %s: %s -> %s" % (name, ealt.get(name), eneu.get(name)))
        sa, sn = set(a), set(n)
        nur_a = [z for z in a if z not in sn][:nmax]
        nur_n = [z for z in n if z not in sa][:nmax]
        for z in nur_a:
            print("   alt: " + z[:220])
        for z in nur_n:
            print("   neu: " + z[:220])
    print("unveraendert: %d Tests" % gleich)


if __name__ == "__main__":
    main()
