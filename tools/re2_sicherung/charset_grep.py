#!/usr/bin/env python3
"""charset_grep.py - ein Wort im RE2-Zeichensatz kodieren und BYTEWEISE ueberall suchen.

Warum nicht ueber die extrahierten *.msg: deren Zerschneidung ist
unvollstaendig (room1010/msg/sub00.msg dekodiert zu '<NL>1'), und desynchrone
SCD-Bloecke koennen Text verdecken. Weil der Zeichensatz eine FESTE
Byte-Abbildung ist, hat jedes Wort genau eine Bytefolge; eine Bytesuche ueber
ALLE Dateien kann daher nichts uebersehen.

Zeichensatz (verifiziert an den Item-Namen, re2_items.py):
    0x00 ' '  0x01 '.'  0x13..0x1C '0'..'9'  0x1D..0x36 'A'..'Z'  0x3D..0x56 'a'..'z'

Aufruf:
    charset_grep.py <wort> [<wort> ...]           in info/re2leon + info/Re1.5
    charset_grep.py --root <pfad> <wort> [...]
"""
import sys, os


def enc(word):
    out = bytearray()
    for c in word:
        if c == " ": out.append(0x00)
        elif c == ".": out.append(0x01)
        elif c.isdigit(): out.append(0x0C + int(c))
        elif "A" <= c <= "Z": out.append(0x1D + ord(c) - ord("A"))
        elif "a" <= c <= "z": out.append(0x3D + ord(c) - ord("a"))
        else: raise ValueError("kein Zeichen im Satz: %r" % c)
    return bytes(out)


def variants(word):
    """Wortanfang gross und klein — beide Schreibweisen kommen vor."""
    vs = {word}
    if word:
        vs.add(word[0].upper() + word[1:])
        vs.add(word[0].lower() + word[1:])
        vs.add(word.upper())
        vs.add(word.lower())
    return sorted(vs)


def scan(roots, words, quiet_limit=400):
    pats = []
    for w in words:
        for v in variants(w):
            try:
                pats.append((v, enc(v)))
            except ValueError:
                pass
    nfiles = 0
    nhits = 0
    for root in roots:
        for dp, dn, fn in os.walk(root):
            for f in fn:
                p = os.path.join(dp, f)
                try:
                    d = open(p, "rb").read()
                except OSError:
                    continue
                nfiles += 1
                for v, pat in pats:
                    i = d.find(pat)
                    while i >= 0:
                        nhits += 1
                        if nhits <= quiet_limit:
                            print("%-72s +0x%06X  %-16s  %s" % (
                                p, i, v, " ".join("%02x" % b for b in d[i:i + len(pat)])))
                        i = d.find(pat, i + 1)
    return nfiles, nhits


if __name__ == "__main__":
    a = sys.argv[1:]
    roots = None
    if a and a[0] == "--root":
        roots = [a[1]]; a = a[2:]
    if roots is None:
        HERE = os.path.dirname(os.path.abspath(__file__))
        REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
        roots = [os.path.join(REPO, "info", "re2leon")]
    nf, nh = scan(roots, a)
    print("--- %d Dateien durchsucht, %d Treffer fuer %s ---" % (nf, nh, a))
