# Runde 35 Spur N, Nachbesserung 2 (Abnahme 1, M1/M2): faehrt die simulierten "kuenftigen Aenderungen" der Abnahme 1
# (N_abnahme_1.md 3.2, E-Reihe, woertlich mit Zeilennummer) am Urteil release/gate_urteil.py nach.
# Je Eintrag genau EINE Aenderung in einer Kopie, dann "<kopie> --selbsttest"; BEMERKT = Rueckgabe != 0.
# Formen: je Eintrag eine Liste von (alt, neu, regex?) - die ERSTE Form, deren alt genau einmal vorkommt, gilt. Form 1 =
# woertlich die Abnahme (Stand 575c964d). Form 2 nur fuer E24/E24b: Nachbesserung 2 hat die Tuer-Soll-Pruefung aus dem
# Meldungstext in eine eigene Anweisung "tuer_soll()" verlegt - die gleiche kuenftige Aenderung ("Regel weg") ist dann das
# Streichen dieser Anweisung. Die benutzte Form steht in der Ausgabe.
# Aufruf: python nb2_urteil_aenderungen.py <gate_urteil.py> <arbeitsordner> [parallel=4]
import concurrent.futures as cf
import os
import re
import subprocess
import sys

E = [
    ("E1", "qb sum(baum) != n -> <", [("sum(baum) != n", "sum(baum) < n", False)]),
    ("E1b", "qb sum(baum) != n -> >", [("sum(baum) != n", "sum(baum) > n", False)]),
    ("E2", "pk qq != gg -> >", [("qq != gg", "qq > gg", False)]),
    ("E2b", "pk qq != gg -> <", [("qq != gg", "qq < gg", False)]),
    ("E3", "st ok_n != n -> <", [("ok_n != n", "ok_n < n", False)]),
    ("E4", "tuer_soll int(g1) != int(g2) -> <", [("int(m.group(1)) != int(m.group(2))", "int(m.group(1)) < int(m.group(2))", False)]),
    ("E5", "qb len(baum) != k -> <", [("len(baum) != k or sum(baum)", "len(baum) < k or sum(baum)", False)]),
    ("E6", "pk len(baum) != k -> <", [("len(baum) != k or any(", "len(baum) < k or any(", False)]),
    ("E7", "pk sum(gg) != n -> <", [("sum(gg for _qq, gg in baum) != n", "sum(gg for _qq, gg in baum) < n", False)]),
    ("E7b", "pk sum(gg) != n -> >", [("sum(gg for _qq, gg in baum) != n", "sum(gg for _qq, gg in baum) > n", False)]),
    ("E8", "apk int(apk_eintraege) != a + 1 -> <", [("int(apk_eintraege) != a + 1", "int(apk_eintraege) < a + 1", False)]),
    ("E8b", "apk int(apk_eintraege) != a + 1 -> >", [("int(apk_eintraege) != a + 1", "int(apk_eintraege) > a + 1", False)]),
    ("E9", "st a != b -> a < b", [("if a != b or b < min_innen", "if a < b or b < min_innen", False)]),
    ("E10", "if rc == 1 -> if rc >= 1", [("if rc == 1:", "if rc >= 1:", False)]),
    ("E11", "if rc != 0 -> if rc > 0 (Abnahme: in der Praxis gleichwertig)", [("if rc != 0:", "if rc > 0:", False)]),
    ("E12", "genau_eine len(t) != 1 -> < 1", [("if len(t) != 1:", "if len(t) < 1:", False)]),
    ("E12b", "genau_eine len(t) != 1 -> > 1", [("if len(t) != 1:", "if len(t) > 1:", False)]),
    ("E29", "apk mb != b -> mb > b", [("mb != b", "mb > b", False)]),
    ("E30", "apk mb != b -> mb < b", [("mb != b", "mb < b", False)]),
    ("E13", "for f in faelle -> faelle[1:]", [("for f in faelle:", "for f in faelle[1:]:", False)]),
    ("E14", "sorted(...) != list(range) -> set != set",
     [("sorted(int(f.group(2)) for f in faelle) != list(range(1, n + 1))",
       "set(int(f.group(2)) for f in faelle) != set(range(1, n + 1))", False)]),
    ("E15", "m_ok = re.fullmatch -> re.match", [("m_ok = re.fullmatch(", "m_ok = re.match(", False)]),
    ("E16", "m_neg = re.fullmatch -> re.search", [("m_neg = re.fullmatch(", "m_neg = re.search(", False)]),
    ("E20", "Tuerarchiv-Labels ohne RE15DOOR", [('("RE2/DOOR", "RE15DOOR")', '("RE2/DOOR",)', False)]),
    ("E24", "quellbaum ohne Tuer-Soll-Regel",
     [("% (n, k, tuer_soll()))\n    if modus == \"paket\"", "% (n, k, 2))\n    if modus == \"paket\"", False),
      (r"\n        tuer_soll\(\)[^\n]*(\n        ende\(0, \"APK-ASSET-GATE-QUELLBAUM-OK)", r"\1", True)]),
    ("E24b", "apk ohne Tuer-Soll-Regel",
     [('else "", tuer_soll()))', 'else "", 2))', False),
      (r"\n        tuer_soll\(\)[^\n]*(\n        ende\(0, \"APK-ASSET-GATE-OK)", r"\1", True)]),
    ("E26", "st 'or b < min_innen' weg", [("if a != b or b < min_innen:", "if a != b:", False)]),
    ("E44", "apk Kette ohne '== n'", [("not (q == a == g == mz == n)", "not (q == a == g == mz)", False)]),
    ("E45", "tuer_soll 'not t or' weg", [("if not t or any(", "if any(", False)]),
]


def anwenden(text, formen):
    for nr, (alt, neu, rx) in enumerate(formen, 1):
        n = len(re.findall(alt, text)) if rx else text.count(alt)
        if n == 1:
            return (re.sub(alt, neu, text) if rx else text.replace(alt, neu)), nr
    return None, 0


def lauf(py, kopie):
    p = subprocess.run([py, kopie, "--selbsttest"], capture_output=True, text=True, encoding="utf-8", errors="replace")
    zeilen = [z for z in p.stdout.splitlines() if z.strip()]
    fehl = [z.strip() for z in zeilen if z.lstrip().startswith("[FEHLER]")][:2]
    return p.returncode, (zeilen[-1] if zeilen else "(keine Ausgabe)"), fehl


def main():
    quelle, ziel = sys.argv[1], sys.argv[2]
    par = int(sys.argv[3]) if len(sys.argv) > 3 else 4
    os.makedirs(ziel, exist_ok=True)
    text = open(quelle, encoding="utf-8", newline="").read()
    jobs = {}
    with cf.ThreadPoolExecutor(par) as ex:
        jobs["K0"] = ("keine Aenderung (Kontrolle)", 0, ex.submit(lauf, sys.executable, quelle))
        for eid, besch, formen in E:
            neu, form = anwenden(text, formen)
            if neu is None:
                jobs[eid] = (besch, 0, None)
                continue
            d = os.path.join(ziel, eid + ".py")
            open(d, "w", encoding="utf-8", newline="").write(neu)
            jobs[eid] = (besch, form, ex.submit(lauf, sys.executable, d))
        bemerkt = gesamt = 0
        for eid, (besch, form, fut) in jobs.items():
            if fut is None:
                print("%-5s KEINE-FORM  %s (Text nicht gefunden)" % (eid, besch))
                gesamt += 1
                continue
            rc, letzte, fehl = fut.result()
            if eid == "K0":
                print("%-5s %s rc=%d  %s" % (eid, "KONTROLLE-OK" if rc == 0 else "KONTROLLE-ROT", rc, letzte[:150]))
                continue
            gesamt += 1
            bemerkt += rc != 0
            print("%-5s %-7s Form %d rc=%d  %-48s %s" % (eid, "BEMERKT" if rc else "NICHT", form, rc, besch[:48], letzte[:120]))
            for z in fehl:
                print("          " + z[:150])
    print("SUMME: %d von %d Aenderungen bemerkt" % (bemerkt, gesamt))


main()
