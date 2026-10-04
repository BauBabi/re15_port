# Runde 35 Spur N, Nachbesserung 3 (Abnahme 2, M2/M3): faehrt die simulierten "kuenftigen Aenderungen" der Abnahme 2
# (N_abnahme_2.md 3.3, H-Reihe, so woertlich wie die Tabelle sie angibt) und eigene weitere (X-Reihe) am Urteil
# release/gate_urteil.py nach. Je Eintrag genau EINE Aenderung in einer Kopie, dann "<kopie> --selbsttest";
# BEMERKT = Rueckgabe != 0. Dazu die FOLGE je Eintrag (wenn angegeben): die Kopie urteilt ueber eine feste Gate-Ausgabe,
# die das Original ablehnt (Rueckgabe 2) - "Variante 0" heisst, die Aenderung laesst sie durch.
# Aufruf: python nb3_urteil_aenderungen.py <gate_urteil.py> <arbeitsordner> [parallel=4]
import concurrent.futures as cf
import os
import subprocess
import sys

TUER = ["   Tuer-Soll: Port-Tuerarchiv re15_port/shared_assets/RE15DOOR   30/30 wie die Engine-Tabelle (Groesse, Aufbau, FNV-1a)",
        "   Tuer-Soll: RE2-Tuerarchiv  re15_port/shared_assets/RE2/DOOR   27/27 wie die Engine-Tabelle (Groesse, Aufbau)"]


def st_log(nummern, n):
    z = ["== APK-Asset-Gate: Selbsttest ==", "   Innere Proben: 3/3 (x)"]
    z += ["   [ok] %02d Fall %d%s rc=1 (soll 1)" % (i, i, " " * 20) for i in nummern]
    return z + ["== SELBSTTEST-OK: %d/%d Faelle (x) ==" % (n, n)]


def pk_log(zeilen, n=12, k=2):
    return ["== APK-Asset-Gate: PC-Paket =="] + zeilen + TUER + [
        "== APK-ASSET-GATE-PAKET-OK: %d Dateien in %d Baeumen bytegleich, nichts zusaetzlich ==" % (n, k)]


def qb_log(zeilen, n=12, k=2):
    return ["== APK-Asset-Gate: nur Quellbaum =="] + zeilen + TUER + [
        "== APK-ASSET-GATE-QUELLBAUM-OK: %d Dateien in %d Baeumen, Tuer-Soll erfuellt ==" % (n, k)]


# (id, Beschreibung, alt, neu, Folge (modus, zeilen) oder None)
H = [
    ("H1", "qb 'or 0 in baum' gestrichen", "sum(baum) != n or 0 in baum:", "sum(baum) != n:", None),
    ("H2", "pk 'or qq == 0' gestrichen", "any(qq != gg or qq == 0 for qq, gg in baum)", "any(qq != gg for qq, gg in baum)", None),
    ("H3", "qb 'n <= 0 or' gestrichen", "if n <= 0 or len(baum) != k or sum(baum)", "if len(baum) != k or sum(baum)", None),
    ("H4", "apk 'or mb != b' gestrichen", "or not (q == a == g == mz == n) or mb != b:", "or not (q == a == g == mz == n):", None),
    ("H5", "Tuerarchiv '> 0' gestrichen", "t[0] == t[1] == t[2] == t[3] > 0", "t[0] == t[1] == t[2] == t[3]", None),
    ("H6", "'or f.group(6).strip()' gestrichen", " or f.group(6).strip():", ":", None),
    ("H7", "startswith('ABBRUCH') -> 'ABBRUCH:'", 'z.startswith("ABBRUCH")', 'z.startswith("ABBRUCH:")', None),
    ("H8", "letzte = letzte Zeile, die mit '== ' beginnt", "    letzte = zeilen[-1]\n",
     "    letzte = ([z for z in zeilen if z.startswith(\"== \")] or zeilen)[-1]\n", None),
    ("H9", "n < min_faelle -> n < min_faelle - 1", "n < min_faelle:", "n < min_faelle - 1:", None),
    ("H10", "Fallzeilen != range(1, n+1) -> range(1, len(faelle)+1)", "!= list(range(1, n + 1))",
     "!= list(range(1, len(faelle) + 1))", ("selbsttest", st_log([1, 2, 3, 4], 5))),
    ("H11", "genau_eine len(t) != 1 -> not t", "        if len(t) != 1:", "        if not t:", None),
    ("H12", "tuer_soll any( -> all(", "if not t or any(", "if not t or all(", None),
    ("H13", "if rc == 1 -> if rc in (1, 2)", "if rc == 1:", "if rc in (1, 2):", None),
    ("H14", "qb Baumzeile fullmatch -> search", 're.fullmatch(r"   \\S+\\s+(\\d+) Dateien", z)',
     're.search(r"   \\S+\\s+(\\d+) Dateien", z)', None),
    ("H15", "'or n < min_faelle' gestrichen", "if ok_n != n or n < min_faelle:", "if ok_n != n:", None),
    ("H16", "apk 'n <= 0 or' gestrichen", "if n <= 0 or not (q == a", "if not (q == a", None),
    ("H17", "pk Quelle = gleich nur noch als Summe", "any(qq != gg or qq == 0 for qq, gg in baum)",
     "any(qq == 0 for qq, gg in baum) or sum(qq for qq, _g in baum) != sum(gg for _q, gg in baum)",
     ("paket", pk_log(["   shared_assets/PSX   9   10", "   synchro   3   2"]))),
    ("H18", "pk Baumzeile laesst eine Zusatzspalte zu", 'r"   \\S+\\s+(\\d+)\\s+(\\d+)"',
     'r"   \\S+\\s+(\\d+)\\s+(?:\\S*\\s+)?(\\d+)"', ("paket", pk_log(["   shared_assets/PSX   10 9 10", "   synchro   2   2"]))),
    ("H19", "tuer_soll wertet nur die erste Tuer-Soll-Zeile", "        if not t or any(int(m.group(1))",
     "        t = t[:1]\n        if not t or any(int(m.group(1))", None),
    ("H20", "unzip-Gegenprobe nur bei weniger", "if int(apk_eintraege) != a + 1:", "if int(apk_eintraege) < a + 1:", None),
    ("H21", "Fallnummern -> nur len(faelle) != n", "if sorted(int(f.group(2)) for f in faelle) != list(range(1, n + 1)):",
     "if len(faelle) != n:", None),
    ("H22", "qb Toleranz abs(sum - n) > 1", "sum(baum) != n", "abs(sum(baum) - n) > 1", None),
    # ---- eigene (Nachbesserung 3): Klassen, die weder H- noch E-Reihe abdecken
    ("X1", "genau_eine fullmatch -> match (Text hinter der Zeile erlaubt)",
     "t = [m for m in (re.fullmatch(muster, z) for z in zeilen) if m]", "t = [m for m in (re.match(muster, z) for z in zeilen) if m]", None),
    ("X2", "pk Baumzeile fullmatch -> match", 're.fullmatch(r"   \\S+\\s+(\\d+)\\s+(\\d+)", z)', 're.match(r"   \\S+\\s+(\\d+)\\s+(\\d+)", z)',
     ("paket", pk_log(["   shared_assets/PSX   10   10   7", "   synchro   2   2"]))),
    ("X3", "qb Baumzeile mit Zusatzzahl", 'r"   \\S+\\s+(\\d+) Dateien"', 'r"   \\S+\\s+(?:\\d+\\s+)?(\\d+) Dateien"',
     ("quellbaum", qb_log(["   shared_assets/PSX   10 9 Dateien", "   synchro   2 Dateien"]))),
    ("X4", "Fallzeilen: nur Nummern 1..n als Menge (Doppelte erlaubt)",
     "if sorted(int(f.group(2)) for f in faelle) != list(range(1, n + 1)):",
     "if set(int(f.group(2)) for f in faelle) != set(range(1, n + 1)):", ("selbsttest", st_log([1, 2, 3, 4, 5, 5], 5))),
    ("X5", "Fallzeilen-Schleife nur bis zur vorletzten", "        for f in faelle:\n", "        for f in faelle[:-1]:\n", None),
    ("X6", "tuer_soll nur die letzte Tuer-Soll-Zeile", "        if not t or any(int(m.group(1))",
     "        t = t[-1:]\n        if not t or any(int(m.group(1))", None),
    ("X7", "apk Kette ohne Manifest-Zeilen", "not (q == a == g == mz == n)", "not (q == a == g == n)", None),
    ("X8", "pk Summe ueber Quelle statt gleich", "sum(gg for _qq, gg in baum) != n", "sum(qq for qq, _gg in baum) != n", None),
    ("X9", "Fehler-Zeilen-Schleife ohne die letzte Zeile", "    for z in zeilen:\n        if z.startswith(\"ABBRUCH\")",
     "    for z in zeilen[:-1]:\n        if z.startswith(\"ABBRUCH\")", None),
    ("X10", "qb Baum mit 0 Dateien nur im ERSTEN Baum geprueft", "or 0 in baum:", "or baum[:1] == [0]:",
     ("quellbaum", qb_log(["   shared_assets/PSX   12 Dateien", "   synchro   0 Dateien"]))),
]


def lauf(py, kopie, folge, d):
    p = subprocess.run([py, kopie, "--selbsttest"], capture_output=True, text=True, encoding="utf-8", errors="replace")
    letzte = [z for z in p.stdout.splitlines() if z.strip()][-1:] or [""]
    falsch = [z.strip()[:110] for z in p.stdout.splitlines() if z.startswith("   [FEHLER]")][:3]
    f_txt = ""
    if folge:
        modus, zeilen = folge
        log = os.path.join(d, "folge.txt")
        open(log, "w", encoding="utf-8", newline="").write("\n".join(zeilen) + "\n")
        q = subprocess.run([py, kopie, modus, log, "0", "5", "3"], capture_output=True, text=True, encoding="utf-8", errors="replace")
        f_txt = "Folge: Variante %d (%s)" % (q.returncode, q.stdout.strip()[:120])
    return p.returncode, letzte[0][:150], falsch, f_txt


def main():
    quelle, ziel = sys.argv[1], sys.argv[2]
    par = int(sys.argv[3]) if len(sys.argv) > 3 else 4
    py = sys.executable
    text = open(quelle, encoding="utf-8", newline="").read()
    os.makedirs(ziel, exist_ok=True)
    jobs = [("K0", "keine Aenderung (Kontrolle)", None, None, None)] + H
    with cf.ThreadPoolExecutor(par) as ex:
        futs = []
        for hid, besch, alt, neu, folge in jobs:
            d = os.path.join(ziel, hid)
            os.makedirs(d, exist_ok=True)
            k = os.path.join(d, "gate_urteil.py")
            if alt is None:
                t = text
            else:
                if text.count(alt) != 1:
                    print("%-4s NICHT ANWENDBAR (%d Treffer): %s" % (hid, text.count(alt), alt[:80]))
                    continue
                t = text.replace(alt, neu)
            open(k, "w", encoding="utf-8", newline="").write(t)
            for f in ("python_finden.sh",):
                pass
            futs.append((hid, besch, alt, ex.submit(lauf, py, k, folge, d)))
        bemerkt = n = 0
        for hid, besch, alt, fut in futs:
            rc, letzte, falsch, f_txt = fut.result()
            if alt is None:
                print("%-4s %s: %s" % (hid, "KONTROLLE-OK" if rc == 0 else "KONTROLLE-FALSCH", letzte))
                continue
            n += 1
            bemerkt += rc != 0
            print("%-4s %-8s %s" % (hid, "BEMERKT" if rc != 0 else "NICHT", besch))
            print("       %s" % letzte)
            for z in falsch:
                print("       %s" % z)
            if f_txt:
                print("       %s" % f_txt)
    print("SUMME: %d von %d Aenderungen bemerkt" % (bemerkt, n))


if __name__ == "__main__":
    main()
