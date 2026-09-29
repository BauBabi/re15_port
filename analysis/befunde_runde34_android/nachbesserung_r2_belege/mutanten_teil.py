# -*- coding: utf-8 -*-
"""Nachbesserung R2 (Befund B1 der Gegenpruefung R2): Mutanten-Probe gegen TEILWEISE Abschwaechungen.

Die Probe der Runde 1 (mutanten_voll.py) schaltete ganze Pruefungen ab (befund -> pass, raise -> pass,
Namensregel weg, 18 Hand-Mutanten). Die Gegenpruefung R2 fand fuenf Ein-Zeilen-Mutanten, die noch
pruefen, aber nur eine Richtung/einen Teil (M1 '!=' -> '<', M9 'a or b' -> 'a', M5 '!=' -> '<',
M14 Flag aus dem falschen Kopf, M16 Filter nur Stored) - alle bestanden den Selbsttest 72/72.
Hier werden solche Abschwaechungen SYSTEMATISCH aus dem Quelltext erzeugt (ast, Textstellen nach
Knotenposition ersetzt), fuer jede Pruefstelle des Gates:
  A  befund(...)            -> pass                       (wie Runde 1)
  B  raise Bedienfehler/_Lesefehler(...) -> pass          (wie Runde 1)
  C  return "<Grund>" in _name_fehler -> pass             (wie Runde 1)
  E  Vergleich:  '!=' -> '<' und '>' (nur eine Richtung), '==' -> '<=' und '>=',
                 '<' <-> '<=', '>' <-> '>=' (Grenze um eins), 'in' <-> 'not in', 'is' <-> 'is not'
  F  'a or b (or c)' / 'a and b (and c)': je ein Operand weg (Teilbedingung faellt)
  G  Tupelvergleich '(a, b, c) != (x, y, z)': je ein Paar weg (nur 2 von 3 Feldern verglichen)
  H  'not X' -> 'X'
  I  ganze Zahl in einem Vergleich: +1 und -1 (Schranke verschoben)
  D  Hand-Mutanten (die fuenf der Gegenpruefung R2, soweit ihr Muster im Code noch vorkommt, + eigene)
Je Mutant ein eigener Prozess '<python> <mutant> --selbsttest'; er MUSS mit rc != 0 enden.
Nur Pruefcode wird mutiert (nicht Selbsttest/Fixture/main).

Aufruf: mutanten_teil.py <gate.py> <ausgabeordner> [--parallel N] [--python <exe>] [--nur KLASSEN]
        [--liste]  (nur erzeugen und zaehlen, nichts laufen lassen)
"""
import argparse
import ast
import concurrent.futures
import os
import re
import subprocess
import sys
import time

PRUEFCODE = {
    # Runde 1
    "_zeichenkette_ende", "_ohne_kommentare", "_klammer_ende", "_anweisungen", "gradle_baeume",
    "gradle_baeume_pruefen", "_ant_passt", "_auslass_hinweis", "quelldateien", "zip_verzeichnis",
    "_name_fehler", "struktur_pruefen", "_eintrag_lesen", "_eintrag_pruefen", "_sha_datei",
    "manifest_pruefen", "pruefen",
    # Runde 2 (Tuer-Soll aus den Engine-Tabellen, Paketvergleich, Quellbaum-Wurzel)
    "_c_ohne_kommentare", "c_struktur", "_c_werte", "c_tabelle", "_fnv1a32", "_c_lesen", "tuer_soll",
    "tueren_pruefen", "wurzel_pruefen", "paket_pruefen", "quellbaum_pruefen", "nur_quellbaum", "_befunde_ausgeben",
    "_sammler", "_manifest_grenze", "_tuer_zeilen_drucken",
}

ROR = {ast.NotEq: ("<", ">"), ast.Eq: ("<=", ">="), ast.Lt: ("<=",), ast.LtE: ("<",), ast.Gt: (">=",),
       ast.GtE: (">",), ast.In: ("not in",), ast.NotIn: ("in",), ast.Is: ("is not",), ast.IsNot: ("is",)}
OPTEXT = {ast.NotEq: "!=", ast.Eq: "==", ast.Lt: "<", ast.LtE: "<=", ast.Gt: ">", ast.GtE: ">=",
          ast.In: "in", ast.NotIn: "not in", ast.Is: "is", ast.IsNot: "is not"}


def _abs(zeilen_start, lineno, col):
    return zeilen_start[lineno - 1] + col


class Quelle(object):
    def __init__(self, text):
        self.text = text
        self.start = [0]
        for z in text.split("\n"):
            self.start.append(self.start[-1] + len(z) + 1)

    def spanne(self, k):
        return (_abs(self.start, k.lineno, k.col_offset), _abs(self.start, k.end_lineno, k.end_col_offset))

    def stueck(self, k):
        a, e = self.spanne(k)
        return self.text[a:e]

    def ersetzen(self, a, e, neu):
        return self.text[:a] + neu + self.text[e:]


def _zeilen_pass(q, k, einzug_von=None):
    zeilen = q.text.split("\n")
    a, e = k.lineno - 1, k.end_lineno
    einzug = re.match(r"\s*", zeilen[a]).group(0)
    return "\n".join(zeilen[:a] + [einzug + "pass  # MUTANT"] + zeilen[e:])


def mutanten(text, hand):
    q = Quelle(text)
    baum = ast.parse(text)
    aus = []
    gesehen = set()

    def neu(name, beschr, t):
        if t == text or t in gesehen:
            return
        try:
            compile(t, name, "exec")
        except SyntaxError:
            return
        gesehen.add(t)
        aus.append((name, beschr, t))

    for fn in ast.walk(baum):
        if not isinstance(fn, ast.FunctionDef) or fn.name not in PRUEFCODE:
            continue
        for k in ast.walk(fn):
            z = getattr(k, "lineno", 0)
            kurz = lambda: " ".join(q.stueck(k).split())[:100]
            # --- A/B/C wie Runde 1
            if (isinstance(k, ast.Expr) and isinstance(k.value, ast.Call) and isinstance(k.value.func, ast.Name)
                    and k.value.func.id == "befund"):
                neu("A_%s_Z%d" % (fn.name, z), kurz(), _zeilen_pass(q, k))
            elif (isinstance(k, ast.Raise) and isinstance(k.exc, ast.Call) and isinstance(k.exc.func, ast.Name)
                  and k.exc.func.id in ("Bedienfehler", "_Lesefehler")):
                neu("B_%s_Z%d" % (fn.name, z), kurz(), _zeilen_pass(q, k))
            elif (fn.name == "_name_fehler" and isinstance(k, ast.Return) and isinstance(k.value, ast.Constant)
                  and isinstance(k.value.value, str) and k.value.value):
                neu("C_%s_Z%d" % (fn.name, z), kurz(), _zeilen_pass(q, k))
            # --- E Vergleichsoperatoren (auch in Ketten 'a <= b < c': je Operator einzeln)
            if isinstance(k, ast.Compare):
                operanden = [k.left] + list(k.comparators)
                for oi, op in enumerate(k.ops):
                    if type(op) not in ROR:
                        continue
                    a = q.spanne(operanden[oi])[1]
                    e = q.spanne(operanden[oi + 1])[0]
                    zwischen = q.text[a:e]
                    alt = OPTEXT[type(op)]
                    m = re.search(r"(?<![<>=!])" + re.escape(alt).replace(r"\ ", r"\s+") + r"(?![<>=])", zwischen)
                    if not m:
                        continue
                    for n_op in ROR[type(op)]:
                        t = q.ersetzen(a + m.start(), a + m.end(), n_op)
                        neu("E_%s_Z%d_%s%s" % (fn.name, z, {"<": "lt", ">": "gt", "<=": "le", ">=": "ge", "in": "in",
                                                             "not in": "notin", "is": "is", "is not": "isnot"}[n_op],
                                                "" if len(k.ops) == 1 else "_op%d" % oi),
                            "%s  [%s -> %s]" % (kurz(), alt, n_op), t)
            if isinstance(k, ast.Compare) and len(k.ops) == 1:
                # G Tupelvergleich: je ein Paar weg
                if (isinstance(k.left, ast.Tuple) and isinstance(k.comparators[0], ast.Tuple)
                        and len(k.left.elts) == len(k.comparators[0].elts) >= 2):
                    le, re_ = k.left.elts, k.comparators[0].elts
                    for i in range(len(le)):
                        l_t = ", ".join(q.stueck(x) for j, x in enumerate(le) if j != i)
                        r_t = ", ".join(q.stueck(x) for j, x in enumerate(re_) if j != i)
                        if len(le) == 2:
                            l_t, r_t = "(" + l_t + ",)", "(" + r_t + ",)"
                        else:
                            l_t, r_t = "(" + l_t + ")", "(" + r_t + ")"
                        la, lz = q.spanne(k.left)
                        ra, rz = q.spanne(k.comparators[0])
                        # Klammern der Tupel mitnehmen, falls vorhanden
                        if q.text[la - 1:la] == "(" and q.text[lz:lz + 1] == ")":
                            la, lz = la - 1, lz + 1
                        if q.text[ra - 1:ra] == "(" and q.text[rz:rz + 1] == ")":
                            ra, rz = ra - 1, rz + 1
                        t = q.text[:la] + l_t + q.text[lz:ra] + r_t + q.text[rz:]
                        neu("G_%s_Z%d_ohne%d" % (fn.name, z, i), "%s  [Paar %d weg]" % (kurz(), i), t)
                # I ganze Zahl im Vergleich +-1
                for c in [k.left] + list(k.comparators):
                    if isinstance(c, ast.Constant) and type(c.value) is int and not isinstance(c.value, bool):
                        ca, ce = q.spanne(c)
                        for d, nm in ((1, "plus1"), (-1, "minus1")):
                            neu("I_%s_Z%d_%s" % (fn.name, z, nm), "%s  [%d -> %d]" % (kurz(), c.value, c.value + d),
                                q.ersetzen(ca, ce, str(c.value + d)))
            # --- F BoolOp: je ein Operand weg
            if isinstance(k, ast.BoolOp) and len(k.values) >= 2:
                wort = " or " if isinstance(k.op, ast.Or) else " and "
                a, e = q.spanne(k)
                for i in range(len(k.values)):
                    rest = [q.stueck(v) for j, v in enumerate(k.values) if j != i]
                    rest = ["(" + r + ")" if ("\n" in r or " or " in r or " and " in r) else r for r in rest]
                    t = q.ersetzen(a, e, "(" + wort.join(rest) + ")")
                    neu("F_%s_Z%d_ohne%d" % (fn.name, z, i),
                        "%s  [Operand %d weg: %s]" % (" ".join(q.stueck(k).split())[:80], i,
                                                     " ".join(q.stueck(k.values[i]).split())[:40]), t)
            # --- H not X -> X
            if isinstance(k, ast.UnaryOp) and isinstance(k.op, ast.Not):
                a, e = q.spanne(k)
                neu("H_%s_Z%d" % (fn.name, z), "%s  [not weg]" % kurz(), q.ersetzen(a, e, "(" + q.stueck(k.operand) + ")"))
    for name, ersetzungen in hand:
        t = text
        ok = True
        for alt, neu_t, n_soll in ersetzungen:
            n = t.count(alt)
            if n != n_soll:
                print("# Hand-Mutant %s: Muster %d-mal (erwartet %d) - im Code nicht mehr vorhanden, entfaellt: %r"
                      % (name, n, n_soll, alt[:70]))
                ok = False
                break
            t = t.replace(alt, neu_t)
        if ok:
            neu(name, "Hand-Mutant", t)
    return aus


def _baum_beenden(proc):
    """Prozess UND alle Kinder beenden: ein Mutant, der das Gate endlos laufen laesst, hinterliesse sonst
    verwaiste Gate-Prozesse (der Selbsttest setzt deren Zeitgrenze selbst durch - stirbt er, tut es keiner)."""
    if os.name == "nt":
        subprocess.run(["taskkill", "/T", "/F", "/PID", str(proc.pid)], stdout=subprocess.DEVNULL,
                       stderr=subprocess.DEVNULL)
    else:
        try:
            os.killpg(proc.pid, 9)
        except OSError:
            pass
    try:
        proc.kill()
    except OSError:
        pass


def laufen(py, pfad, timeout):
    t0 = time.monotonic()
    extra = {} if os.name == "nt" else {"start_new_session": True}
    umgebung = dict(os.environ, RE15_GATE_SELBSTTEST_SCHNELL="1")   # Selbsttest bricht beim ersten falschen Fall ab
    proc = subprocess.Popen([py, pfad, "--selbsttest"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            stdin=subprocess.DEVNULL, env=umgebung, **extra)
    try:
        out, _ = proc.communicate(timeout=timeout)
        aus = out.decode("utf-8", "replace")
        rc = proc.returncode
    except subprocess.TimeoutExpired:
        _baum_beenden(proc)
        out, _ = proc.communicate()
        aus = (out or b"").decode("utf-8", "replace") + "\n[ZEITGRENZE %d s - Prozessbaum beendet]" % timeout
        rc = -9
    rot = re.findall(r"\[FEHLER\] (\d+) ", aus)
    return rc, rot, aus, time.monotonic() - t0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("gate")
    ap.add_argument("ordner")
    ap.add_argument("--parallel", type=int, default=3)
    ap.add_argument("--python", default=sys.executable)
    ap.add_argument("--nur", default="ABCDEFGHI")
    ap.add_argument("--liste", action="store_true")
    ap.add_argument("--timeout", type=int, default=300,
                    help="je Mutant; ein Gate, das haengt, ist erkannt (der Selbsttest faellt damit ebenfalls)")
    ap.add_argument("--hand", default=None, help="Python-Datei mit HAND = [(name, [(alt, neu, n)])]")
    a = ap.parse_args()
    text = open(a.gate, encoding="utf-8").read().replace("\r\n", "\n")
    hand = []
    if a.hand:
        ns = {}
        exec(open(a.hand, encoding="utf-8").read(), ns)
        hand = ns["HAND"]
    liste = [m for m in mutanten(text, hand) if m[0][0] in a.nur]
    zahl = {}
    for name, _b, _t in liste:
        zahl[name[0]] = zahl.get(name[0], 0) + 1
    print("# %d Mutanten %s aus %s, Python %s" % (len(liste), dict(sorted(zahl.items())), a.gate, a.python), flush=True)
    if a.liste:
        for name, beschr, _t in liste:
            print("%-44s %s" % (name, beschr))
        return 0
    os.makedirs(a.ordner, exist_ok=True)
    # Ergebnisse laufend in <ordner>/ergebnis.tsv (Name, rc, rote Faelle, Dauer) - ein Neustart ueberspringt,
    # was dort schon steht (Mutanten mit gleichem Namen UND gleichem Text: Pruefsumme im Namen der Kopie)
    ergebnis_datei = os.path.join(a.ordner, "ergebnis.tsv")
    fertig_vorher = {}
    if os.path.exists(ergebnis_datei):
        for z in open(ergebnis_datei, encoding="utf-8"):
            teile = z.rstrip("\n").split("\t")
            if len(teile) >= 5:
                fertig_vorher[(teile[0], teile[4])] = (int(teile[1]), teile[2].split(",") if teile[2] != "-" else [],
                                                      float(teile[3]))
    import hashlib
    pfade, ergebnis = [], {}
    for name, beschr, t in liste:
        kennung = hashlib.sha256(t.encode("utf-8")).hexdigest()[:12]
        if (name, kennung) in fertig_vorher:
            rc, rot, dauer = fertig_vorher[(name, kennung)]
            ergebnis[name] = (rc, rot, "(aus ergebnis.tsv)", dauer)
            pfade.append((name, beschr, None, kennung))
            continue
        p = os.path.join(a.ordner, name + ".py")
        with open(p, "w", encoding="utf-8", newline="\n") as f:
            f.write(t)
        pfade.append((name, beschr, p, kennung))
    offen = [(n, b, p, k) for n, b, p, k in pfade if p is not None]
    print("# davon schon erledigt (ergebnis.tsv): %d, zu laufen: %d" % (len(pfade) - len(offen), len(offen)), flush=True)
    t0 = time.monotonic()
    with concurrent.futures.ThreadPoolExecutor(max_workers=a.parallel) as pool:
        laeufe = {pool.submit(laufen, a.python, p, a.timeout): (n, b, p, k) for n, b, p, k in offen}
        fertig = 0
        for lauf in concurrent.futures.as_completed(laeufe):
            n, b, p, k = laeufe[lauf]
            rc, rot, aus, dauer = lauf.result()
            ergebnis[n] = (rc, rot, aus, dauer)
            with open(os.path.join(a.ordner, n + ".log"), "w", encoding="utf-8") as f:
                f.write(aus)
            with open(ergebnis_datei, "a", encoding="utf-8") as f:
                f.write("%s\t%d\t%s\t%.1f\t%s\n" % (n, rc, ",".join(rot) if rot else "-", dauer, k))
            if rc != 0:
                os.remove(p)
            fertig += 1
            print("%-9s rc=%-3d %5.0fs %-44s %s" % ("ERKANNT" if rc != 0 else "UEBERLEBT", rc, dauer, n, b[:90]), flush=True)
            if fertig % 25 == 0:
                print("# ... %d/%d (%.0f s)" % (fertig, len(offen), time.monotonic() - t0), flush=True)
    ueberlebt = [(n, b) for n, b, _p, _k in pfade if ergebnis[n][0] == 0]
    print("# erkannt: %d / %d, ueberlebt: %d, Laufzeit %.0f s" % (len(pfade) - len(ueberlebt), len(pfade), len(ueberlebt),
                                                               time.monotonic() - t0))
    for n, b in ueberlebt:
        print("#   UEBERLEBT %s: %s" % (n, b))
    return 0


if __name__ == "__main__":
    sys.exit(main())
