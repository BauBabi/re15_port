# -*- coding: utf-8 -*-
"""Gegenpruefung R4-2 (Linse Umgehung): NEUE Ein-Zeilen-Mutanten gegen den Gate-Selbsttest (258 Faelle + 132 innere Proben).
Basis: Klassen der R2-Kampagne (mutanten_teil_r2.py = Kopie von nachbesserung_r2_belege/mutanten_teil.py, UNVERAENDERT
importiert), aber auf den seit R2 NEUEN/GEAENDERTEN Pruefcode gerichtet (N1: manifest_lesen, _pfad_fehler, manifest_pruefen
v2, pruefen apk_sha; NB R4-1: ASCII-Regel, quellpfade_pruefen) + zusaetzliche Klassen, die die R2-Klassen nicht erzeugen:
  A2  fehler.append(...) -> pass              (manifest_lesen meldet ueber fehler.append, nicht ueber befund)
  C2  return "<Grund>" / return (...) in _pfad_fehler -> pass (Regel faellt weg)
  K   continue -> pass in manifest_lesen (fehlerhafte Zeile wird trotzdem weiterverarbeitet)
  D   Hand-Mutanten (Konstanten auf Modulebene, strip/rstrip/split/fullmatch-Varianten) aus u2_hand.py
Ueberlebender = Selbsttest Rueckgabe 0 UND letzte Zeile SELBSTTEST-OK; danach Urteil wie apk_pruefen.sh gate_urteil
(u2_urteil.sh) auf dem vollen Log. Schnellmodus RE15_GATE_SELBSTTEST_SCHNELL=1 wie R2 (erster falscher Fall -> Ende).
Aufruf: u2_mutanten.py <gate.py> <ordner> [--parallel N] [--liste] [--klassen A,A2,...] [--nur-namen DATEI]
"""
import argparse
import ast
import concurrent.futures
import hashlib
import os
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mutanten_teil_r2 as r2  # noqa: E402

FUNKS = {"_pfad_fehler", "manifest_lesen", "manifest_pruefen", "quellpfade_pruefen", "quelldateien", "pruefen",
         "paket_pruefen", "nur_quellbaum", "_widerspruch_pruefen", "_befunde_ausgeben", "_manifest_grenze",
         "quellbaum_pruefen", "wurzel_pruefen", "_tuer_zeilen_drucken"}
r2.PRUEFCODE = FUNKS          # die R2-Klassen A/B/C/E/F/G/H/I laufen damit NUR ueber diese Funktionen


def klasse(name):
    return name[:2] if name[:2] in ("A2", "C2") else name[0]


def extra(text):
    q = r2.Quelle(text)
    baum = ast.parse(text)
    aus = []
    for fn in ast.walk(baum):
        if not isinstance(fn, ast.FunctionDef) or fn.name not in FUNKS:
            continue
        for k in ast.walk(fn):
            z = getattr(k, "lineno", 0)
            kurz = " ".join(q.stueck(k).split())[:100] if hasattr(k, "end_lineno") else ""
            if (isinstance(k, ast.Expr) and isinstance(k.value, ast.Call) and isinstance(k.value.func, ast.Attribute)
                    and k.value.func.attr == "append" and isinstance(k.value.func.value, ast.Name)
                    and k.value.func.value.id == "fehler"):
                aus.append(("A2_%s_Z%d" % (fn.name, z), kurz, r2._zeilen_pass(q, k)))
            if fn.name == "_pfad_fehler" and isinstance(k, ast.Return) and k.value is not None and not (
                    isinstance(k.value, ast.Constant) and k.value.value is None):
                aus.append(("C2_%s_Z%d" % (fn.name, z), kurz, r2._zeilen_pass(q, k)))
            if fn.name == "manifest_lesen" and isinstance(k, ast.Continue):
                aus.append(("K_%s_Z%d" % (fn.name, z), "continue -> pass", r2._zeilen_pass(q, k)))
    return aus


def alle(text, hand):
    liste = r2.mutanten(text, hand)
    namen = {m[0] for m in liste}
    gesehen = {m[2] for m in liste}
    for name, beschr, t in extra(text):
        if t == text or t in gesehen:
            continue
        try:
            compile(t, name, "exec")
        except SyntaxError:
            continue
        b, k = name, 2
        while name in namen:
            name = "%s_%d" % (b, k)
            k += 1
        namen.add(name)
        gesehen.add(t)
        liste.append((name, beschr, t))
    return liste


URTEIL = os.path.join(os.path.dirname(os.path.abspath(__file__)), "u2_urteil.sh")
GITBASH = r"C:\Program Files\Git\bin\bash.exe"


def laufen(py, pfad, timeout):
    rc, rot, aus, dauer = r2.laufen(py, pfad, timeout)
    urteil = "-"
    if rc == 0:
        log = pfad[:-3] + ".voll.log"
        with open(log, "w", encoding="utf-8", newline="\n") as f:
            f.write(aus)
        # Git-Bash ausdruecklich: "bash" aus Python loest unter Windows den WSL-Starter System32\bash.exe auf
        # (erster Lauf k1: "WSL ... CreateProcessCommon" statt Urteil - Urteile danach mit u2_urteil_nach.sh nachgerechnet)
        u = subprocess.run([GITBASH, URTEIL, "selbsttest", log.replace("\\", "/"), "0"], stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT)
        urteil = "%d %s" % (u.returncode, u.stdout.decode("utf-8", "replace").strip().replace("\n", " | ")[:200])
    return rc, rot, aus, dauer, urteil


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("gate")
    ap.add_argument("ordner")
    ap.add_argument("--parallel", type=int, default=3)
    ap.add_argument("--python", default=sys.executable)
    ap.add_argument("--klassen", default="A,A2,B,C,C2,D,E,F,G,H,I,K")
    ap.add_argument("--liste", action="store_true")
    ap.add_argument("--timeout", type=int, default=400)
    ap.add_argument("--hand", default=os.path.join(os.path.dirname(os.path.abspath(__file__)), "u2_hand.py"))
    ap.add_argument("--nur-namen", default=None)
    a = ap.parse_args()
    text = open(a.gate, encoding="utf-8").read().replace("\r\n", "\n")
    ns = {}
    exec(open(a.hand, encoding="utf-8").read(), ns)
    erlaubt = set(a.klassen.split(","))
    liste = [m for m in alle(text, ns["HAND"]) if klasse(m[0]) in erlaubt]
    if a.nur_namen:
        namen = set(z.split()[0] for z in open(a.nur_namen, encoding="utf-8") if z.strip())
        liste = [m for m in liste if m[0] in namen]
    zahl = {}
    for n, _b, _t in liste:
        zahl[klasse(n)] = zahl.get(klasse(n), 0) + 1
    print("# %d Mutanten %s aus %s" % (len(liste), dict(sorted(zahl.items())), a.gate), flush=True)
    if a.liste:
        for n, b, _t in liste:
            print("%-48s %s" % (n, b))
        return 0
    os.makedirs(a.ordner, exist_ok=True)
    erg = os.path.join(a.ordner, "ergebnis.tsv")
    fertig = set()
    if os.path.exists(erg):
        for z in open(erg, encoding="utf-8"):
            t = z.rstrip("\n").split("\t")
            if len(t) >= 6:
                fertig.add((t[0], t[5]))
    offen = []
    for n, b, t in liste:
        kenn = hashlib.sha256(t.encode("utf-8")).hexdigest()[:12]
        if (n, kenn) in fertig:
            continue
        p = os.path.join(a.ordner, n + ".py")
        with open(p, "w", encoding="utf-8", newline="\n") as f:
            f.write(t)
        offen.append((n, b, p, kenn))
    print("# zu laufen: %d (schon erledigt: %d)" % (len(offen), len(liste) - len(offen)), flush=True)
    t0 = time.monotonic()
    with concurrent.futures.ThreadPoolExecutor(max_workers=a.parallel) as pool:
        laeufe = {pool.submit(laufen, a.python, p, a.timeout): (n, b, p, k) for n, b, p, k in offen}
        i = 0
        for lauf in concurrent.futures.as_completed(laeufe):
            n, b, p, k = laeufe[lauf]
            try:
                rc, rot, aus, dauer, urteil = lauf.result()
            except Exception as ex:
                rc, rot, aus, dauer, urteil = 99, [], "WERKZEUGFEHLER %r" % (ex,), 0.0, "-"
            with open(os.path.join(a.ordner, n + ".log"), "w", encoding="utf-8") as f:
                f.write(aus)
            with open(erg, "a", encoding="utf-8") as f:
                f.write("%s\t%d\t%s\t%.1f\t%s\t%s\t%s\n" % (n, rc, ",".join(rot) if rot else "-", dauer, urteil, k,
                                                          b.replace("\t", " ")[:150]))
            if rc not in (0, 99):
                try:
                    os.remove(p)
                except OSError:
                    pass
            i += 1
            st = "UEBERLEBT" if rc == 0 else ("WERKZEUG" if rc == 99 else "ERKANNT")
            print("%-9s rc=%-3d %4.0fs %-46s urteil=%s | %s" % (st, rc, dauer, n, urteil[:60], b[:70]), flush=True)
            if i % 25 == 0:
                print("# ... %d/%d (%.0f s)" % (i, len(offen), time.monotonic() - t0), flush=True)
    print("# fertig %.0f s" % (time.monotonic() - t0))
    return 0


if __name__ == "__main__":
    sys.exit(main())
