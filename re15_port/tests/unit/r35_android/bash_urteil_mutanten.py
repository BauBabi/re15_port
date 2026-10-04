# =============================================================================================
# Runde 35 Spur N "android", Nachbesserung 2 (Abnahme 1, M3) - MUTANTEN des bash-Urteils in release/apk_pruefen.sh.
# Nutzer: "Kuenftige Aenderungen am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten."
# Dossier: analysis/befunde_runde35/N_android.md, Abschnitt "Nachbesserung 2". ctest unit_r35_android_bash_mutanten.
#
# Das Python-Urteil (release/gate_urteil.py) mutiert sich in seinem --selbsttest selbst. Das bash-Urteil (die Funktionen
# FUNKTIONEN unten) hatte bis Nachbesserung 2 nur feste Kontrollen, und eine einseitige Lockerung einer Pruefzeile blieb
# unbemerkt (Abnahme 1: F1/F3/F4/F6/F8). Dieses Werkzeug erzeugt je Lauf genau EINE Aenderung an diesen Funktionen und
# laesst urteil_kontrollen.sh (Attrappen, --schnell) dagegen laufen: jeder Mutant muss mindestens eine Kontrolle rot
# machen - sonst ist er in AEQUIVALENT begruendet, oder der Lauf scheitert. Wer eine Pruefzeile aendert oder neu
# schreibt und keine Kontrolle dazu, bekommt hier einen ueberlebenden Mutanten.
#
# OPERATOREN (nur im Code, nie in Kommentaren oder in Zeichenketten in Anfuehrungszeichen, ausser C):
#   A  (( ... )): jeder Vergleich == / != -> jeder andere der sechs (darunter die einseitigen Lockerungen in beide
#      Richtungen), < <-> <=, > <-> >=, < <-> >, <= <-> >= (dieselbe Tabelle wie gate_urteil.py TAUSCH); && <-> ||
#   B  [[ ... ]] ohne =~: == <-> !=; jede [[ ]]-Pruefung verneint bzw. ent-neint (! davor / weg)
#   C  grep -q "<muster>": Rest ab Stueck k -> .*, Wort -> .*, Einzelzeichen weg (auch ^ und Leerzeichen)
#   D  [[ ... =~ <muster> ]]: Rest ab Stueck k -> .*, Wort -> .*, [0-9]+ -> [0-9]* und -> .*, Einzelzeichen/Escape weg,
#      Zeichenklasse -> ., {n} -> +; dazu die Verneinung (B)
#   E  jeder Aufruf von die -> true (die Pruefung bricht nicht mehr ab)
#   F  Zuweisung name=<Zahl> -> 0 bzw. +-1
#   G  Aufruf einer der Pruef-Funktionen als eigene Zeile -> weg
#   H  return <x> -> return 0 (bzw. 1, wenn x = 0)
# NICHT mutiert: Meldungstexte, Ausgaben (echo, grep -v fuer die Anzeige), Pfade/Dateinamen, die Mindestzahlen
# (GATE_URTEIL_MIN_*: deren Senkung ist eine sichtbare Zeile im Diff, Kopf von apk_pruefen.sh).
#
# Aufruf: python bash_urteil_mutanten.py <apk_pruefen.sh> <urteil_kontrollen.sh> <arbeit> <bash> [parallel]
# Schluss "== BASH-URTEIL-MUTANTEN-OK: n Mutanten, k erkannt, g als gleichwertig begruendet ==" (Rueckgabe 0) bzw.
# "== BASH-URTEIL-MUTANTEN-FEHLER: ... ==" (Rueckgabe 1).
# =============================================================================================
import concurrent.futures as cf
import os
import re
import shutil
import subprocess
import sys
import time

FUNKTIONEN = ["gate_pin_pruefen", "gate_urteil_pin_pruefen", "gate_urteil_selbsttest", "gate_festhalten",
              "gate_urteil", "gate_laufen"]
TAUSCH = {"==": ("!=", "<=", ">=", "<", ">"), "!=": ("==", "<", ">", "<=", ">="), "<": ("<=", ">"), "<=": ("<", ">="),
          ">": (">=", "<"), ">=": (">", "<=")}

# Mutanten, die keine Kontrolle erkennen KANN - je Eintrag die Begruendung. Schluessel = Beschreibung ohne Zeilennummer.
AEQUIVALENT = {
}


def masken(zeilen, a, e):
    """Je Zeile a..e-1: (maske, kommentar_ab) - maske[i] True = Zeichen i ist Code (nicht in Anfuehrungszeichen, nicht
    Kommentar). Der Zustand der Anfuehrungszeichen geht ueber Zeilen hinweg (mehrzeilige Meldungstexte von die)."""
    out, q = [], None
    for z in zeilen[a:e]:
        mk, ab, i = [False] * len(z), len(z), 0
        while i < len(z):
            c = z[i]
            if q:
                if c == "\\" and q == '"':
                    i += 2
                    continue
                if c == q:
                    q = None
            elif c in "'\"":
                q = c
            elif c == "#" and (i == 0 or z[i - 1] in " \t"):
                ab = i
                break
            else:
                mk[i] = True
            i += 1
        out.append((mk, ab))
    return out


def stuecke(p, ere=True):
    """Muster in Stuecke: (anfang, ende, art) mit art wort / zeichen / esc / klasse / gruppe (+q bei Quantor)."""
    out, i, n = [], 0, len(p)
    while i < n:
        a, c = i, p[i]
        if c == "\\":
            i, art = i + 2, "esc"
        elif c == "[":
            j = i + 1
            if j < n and p[j] == "^":
                j += 1
            if j < n and p[j] == "]":
                j += 1
            while j < n and p[j] != "]":
                if p[j] == "[" and j + 1 < n and p[j + 1] == ":":
                    j = p.index(":]", j + 2) + 2
                    continue
                j += 1
            i, art = j + 1, "klasse"
        elif c == "(" and ere:
            tiefe, j = 0, i
            while j < n:
                if p[j] == "\\":
                    j += 2
                    continue
                if p[j] == "(":
                    tiefe += 1
                elif p[j] == ")":
                    tiefe -= 1
                    if tiefe == 0:
                        break
                j += 1
            i, art = j + 1, "gruppe"
        elif c.isalnum() or c in "_$":
            while i < n and (p[i].isalnum() or p[i] in "_$"):
                i += 1
            art = "wort" if i - a > 1 else "zeichen"
        else:
            i, art = i + 1, "zeichen"
        q = re.match(r"(?:[*+?]|\{\d*(?:,\d*)?\})", p[i:]) if ere else None
        if q:
            i += q.end()
            art += "+q"
        out.append((a, i, art))
    return out


def lockerungen(p, ere):
    """-> [(beschreibung, neues_muster)] (Operatoren C/D)."""
    out, gesehen = [], {p}

    def dazu(was, neu):
        if neu not in gesehen:
            gesehen.add(neu)
            out.append((was, neu))
    st = stuecke(p, ere)
    for k, (a, _e, _art) in enumerate(st):
        dazu("Rest ab Stueck %d %r -> .*" % (k, p[a:a + 20]), p[:a] + ".*")
    for k, (a, e, art) in enumerate(st):
        if art == "wort":
            dazu("Wort %r -> .*" % p[a:e], p[:a] + ".*" + p[e:])
    for k, (a, e, art) in enumerate(st):
        if art in ("zeichen", "esc"):
            dazu("Zeichen %d %r weg" % (k, p[a:e]), p[:a] + p[e:])
    if ere:
        for m in re.finditer(r"\[0-9\]\+", p):
            dazu("[0-9]+ bei %d -> [0-9]*" % m.start(), p[:m.start()] + "[0-9]*" + p[m.end():])
            dazu("[0-9]+ bei %d -> .*" % m.start(), p[:m.start()] + ".*" + p[m.end():])
        for k, (a, e, art) in enumerate(st):
            if art.startswith("klasse"):
                kl = p[a:e]
                q = re.search(r"(?:[*+?]|\{\d*(?:,\d*)?\})$", kl)
                rumpf = kl[:q.start()] if q else kl
                dazu("Klasse %r -> ." % rumpf, p[:a] + "." + kl[len(rumpf):] + p[e:])
                if q and q.group(0).startswith("{"):
                    dazu("Quantor %r -> +" % q.group(0), p[:a] + rumpf + "+" + p[e:])
    return out


def mutanten(text):
    """-> Liste (zeilennummer, beschreibung, neuer_text)"""
    zeilen = text.split("\n")
    bereiche = []
    for name in FUNKTIONEN:
        a = next(i for i, z in enumerate(zeilen) if re.match(r"%s\(\) *\{" % re.escape(name), z))
        e = next(i for i in range(a + 1, len(zeilen)) if zeilen[i].startswith("}"))
        bereiche.append((a + 1, e))
    out = []

    def m(i, was, neu_zeile):
        z = list(zeilen)
        z[i] = neu_zeile
        out.append((i + 1, was, "\n".join(z)))

    for a, e in bereiche:
        for i, (mk, ce) in zip(range(a, e), masken(zeilen, a, e)):
            z = zeilen[i]
            if not any(mk[:ce]):
                continue
            imfrei = lambda pos, mk=mk: pos < len(mk) and mk[pos]   # noqa: E731
            # A: (( ... ))
            for mm in re.finditer(r"\(\((.*?)\)\)", z[:ce]):
                if not imfrei(mm.start()):
                    continue
                inhalt, base = mm.group(1), mm.start(1)
                for om in re.finditer(r"==|!=|<=|>=|&&|\|\||<|>", inhalt):
                    op = om.group(0)
                    neue = ("||",) if op == "&&" else ("&&",) if op == "||" else TAUSCH[op]
                    for neu in neue:
                        nz = z[:base + om.start()] + neu + z[base + om.end():]
                        m(i, "A (( %s )): %s -> %s" % (inhalt.strip(), op, neu), nz)
            # B/D: [[ ... ]]
            for mm in re.finditer(r"\[\[ (.*?) \]\]", z[:ce]):
                if not imfrei(mm.start()):
                    continue
                inhalt, base = mm.group(1), mm.start(1)
                if inhalt.startswith("! "):
                    m(i, "B [[ %s ]]: ! weg" % inhalt, z[:base] + inhalt[2:] + z[mm.end(1):])
                else:
                    m(i, "B [[ %s ]]: ! davor" % inhalt, z[:base] + "! " + inhalt + z[mm.end(1):])
                rx = re.match(r"(.*? =~ )(.*)$", inhalt)
                if rx:
                    p, pa = rx.group(2), base + rx.end(1)
                    for was, neu in lockerungen(p, True):
                        m(i, "D =~ %s: %s" % (p[:40], was), z[:pa] + neu + z[pa + len(p):])
                    continue
                for om in re.finditer(r" (==|!=) ", inhalt):
                    neu = "!=" if om.group(1) == "==" else "=="
                    m(i, "B [[ %s ]]: %s -> %s" % (inhalt, om.group(1), neu),
                      z[:base + om.start(1)] + neu + z[base + om.end(1):])
            # C: grep -q "<muster>"
            for mm in re.finditer(r'grep -q "((?:[^"\\]|\\.)*)"', z[:ce]):
                if not imfrei(mm.start()):
                    continue
                p, pa = mm.group(1), mm.start(1)
                for was, neu in lockerungen(p, False):
                    m(i, "C grep -q %s: %s" % (p[:40], was), z[:pa] + neu + z[pa + len(p):])
            # E: die -> true
            for mm in re.finditer(r"(?<![\w-])die(?= )", z[:ce]):
                if imfrei(mm.start()):
                    m(i, "E die -> true: %s" % z[mm.start():ce].strip()[:70], z[:mm.start()] + "true" + z[mm.end():])
            # F: name=<Zahl>
            for mm in re.finditer(r"(?<![\w$])([a-z_]+)=(\d+)\b", z[:ce]):
                if not imfrei(mm.start()):
                    continue
                w = int(mm.group(2))
                for neu in sorted({0, w + 1, w - 1} - {w, -1}):
                    m(i, "F %s=%d -> %d" % (mm.group(1), w, neu), z[:mm.start(2)] + str(neu) + z[mm.end(2):])
            # G: Aufruf einer Pruef-Funktion als eigene Zeile
            g = re.match(r"(\s*)(%s)\b" % "|".join(FUNKTIONEN), z[:ce])
            if g and z[:ce].strip().split()[0] in FUNKTIONEN:
                m(i, "G Aufruf weg: %s" % z[:ce].strip()[:70], g.group(1) + ":")
            # H: return <x>
            for mm in re.finditer(r"\breturn (\S+)", z[:ce]):
                if imfrei(mm.start()):
                    neu = "1" if mm.group(1) == "0" else "0"
                    m(i, "H return %s -> %s" % (mm.group(1), neu), z[:mm.start(1)] + neu + z[mm.end(1):])
    return out


def main():
    ap, kontrollen, arbeit, bash = (x.replace("\\", "/") for x in sys.argv[1:5])   # bash bekommt nur C:/...-Pfade
    par = int(sys.argv[5]) if len(sys.argv) > 5 else 6
    text = open(ap, encoding="utf-8", newline="").read()
    if os.path.isdir(arbeit):
        shutil.rmtree(arbeit)
    os.makedirs(arbeit)
    att = arbeit + "/attrappen"
    t0 = time.time()
    p = subprocess.run([bash, kontrollen, "anlegen", ap, att], capture_output=True, text=True)
    if p.returncode != 0:
        print(p.stdout + p.stderr)
        print("== BASH-URTEIL-MUTANTEN-FEHLER: Attrappen nicht angelegt ==")
        return 1

    def lauf(nr, neu):
        d = "%s/m%03d" % (arbeit, nr)
        os.makedirs(d)
        f = d + "/apk_pruefen.sh"
        open(f, "w", encoding="utf-8", newline="").write(neu)
        try:
            q = subprocess.run([bash, kontrollen, "pruefen", f, att, d + "/w", "--schnell"],
                               capture_output=True, text=True, timeout=300)
        except subprocess.TimeoutExpired:
            return 124, "Zeitlimit"
        z = [x for x in q.stdout.splitlines() if x.startswith("FALSCH")]
        return q.returncode, (z[0] if z else q.stdout.strip().splitlines()[-1] if q.stdout.strip() else "")

    # Kontrolle: das unveraenderte Urteil muss ALLE Kontrollen bestehen
    k_rc, k_z = lauf(0, text)
    print("   Kontrolle (unveraendert): Rueckgabe %d %s" % (k_rc, k_z[:150]))
    ms = mutanten(text)
    zaehl = {}
    for _n, was, _t in ms:
        zaehl[was[0]] = zaehl.get(was[0], 0) + 1
    print("   Mutanten je Operator: " + ", ".join("%s %d" % kv for kv in sorted(zaehl.items())))
    erkannt = gleich = 0
    ueberlebt, gesehen = [], set()
    with cf.ThreadPoolExecutor(par) as ex:
        futs = [(zl, was, ex.submit(lauf, nr, neu)) for nr, (zl, was, neu) in enumerate(ms, 1)]
        for zl, was, fut in futs:
            rc, z = fut.result()
            if rc != 0:
                erkannt += 1
                print("   [erkannt] Zeile %d: %s  <- %s" % (zl, was[:110], z[:90]))
            elif was in AEQUIVALENT:
                gleich += 1
                gesehen.add(was)
                print("   [gleichwertig] Zeile %d: %s" % (zl, was[:150]))
            else:
                ueberlebt.append("Zeile %d: %s" % (zl, was))
    for u in ueberlebt:
        print("   [FEHLER] Mutant UEBERLEBT (Kontrolle in urteil_kontrollen.sh ergaenzen oder in AEQUIVALENT begruenden): " + u)
    veraltet = sorted(set(AEQUIVALENT) - gesehen)
    for v in veraltet:
        print("   [FEHLER] AEQUIVALENT-Eintrag passt zu keinem gleichwertigen Mutanten mehr: " + v)
    print("   Laufzeit %.0f s, %d parallel" % (time.time() - t0, par))
    if k_rc != 0 or ueberlebt or veraltet or not ms:
        print("== BASH-URTEIL-MUTANTEN-FEHLER: Kontrolle %d, %d Mutanten, %d ueberlebt, %d veraltete Eintraege =="
              % (k_rc, len(ms), len(ueberlebt), len(veraltet)))
        return 1
    print("== BASH-URTEIL-MUTANTEN-OK: %d Mutanten, %d erkannt, %d als gleichwertig begruendet ==" % (len(ms), erkannt, gleich))
    return 0


if __name__ == "__main__":
    sys.exit(main())
