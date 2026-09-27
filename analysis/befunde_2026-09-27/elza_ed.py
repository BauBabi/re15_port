"""Kleiner CRLF-treuer Textersetzer fuer die Elza-Zweig-Runde.

Die Quelldateien des Ports sind durchgehend CRLF; Python liest sie universal
(-> \n) und schreibt sie hier wieder mit \r\n zurueck, damit der Diff nur die
wirklich geaenderten Zeilen zeigt.
"""
import io


def rd(p):
    return io.open(p, encoding="utf-8", newline=None).read()


def wr(p, s):
    io.open(p, "w", encoding="utf-8", newline="\r\n").write(s)


def rep(p, pairs):
    s = rd(p)
    for old, new in pairs:
        n = s.count(old)
        if n != 1:
            raise SystemExit("count=%d in %s:\n%s" % (n, p, old[:300]))
        s = s.replace(old, new)
    wr(p, s)
    print("patched", p)
