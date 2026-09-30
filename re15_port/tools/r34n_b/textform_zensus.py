#!/usr/bin/env python3
"""Spur B (Runde 34 Nacht, BAU, Auflage 6 der Gegenpruefung): FORM der RE1.5-Untersuchungstexte.

Die Gegenpruefung haelt fest, dass der Satzschluss-Zensus '.' und '...' nicht trennt und dass das
naechste Vorbild (ROOM4020 msg 1 @0x0BAA "The button doesn't respond...") mit `08` beginnt. Dieses
Werkzeug zaehlt ueber alle Untersuchungstexte (Kopf `04 02`) aller RDTs:
  * fuehrendes 0x08 (Zeilenumbruch direkt nach dem Kopf) ja/nein,
  * Zeilenzahl (0x08 im Text + 1),
  * Schluss: genau ein '.', '...' (3x 0x57 bzw. 0xF2), '!', '?', sonst,
  * getrennt fuer EINZEILIGE Texte (die Form, die "Nothing happened." haette),
und listet die kurzen Misserfolgs-Texte ("respond", "Nothing", "happen", "won't", "doesn't").

    C:/Python310/python.exe re15_port/tools/r34n_b/textform_zensus.py
"""
import collections
import glob
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))


def g(b):
    if b == 0x00: return " "
    if 0x0C <= b <= 0x15: return chr(ord('0') + b - 0x0C)
    if b == 0x16: return ":"
    if b == 0x18: return ","
    if b == 0x19: return '"'
    if b == 0x1A: return "!"
    if b == 0x1B: return "?"
    if 0x1D <= b <= 0x36: return chr(ord('A') + b - 0x1D)
    if b == 0x38: return "/"
    if b == 0x3A: return "'"
    if b == 0x3B: return "-"
    if b == 0x3C: return "."
    if 0x3D <= b <= 0x56: return chr(ord('a') + b - 0x3D)
    if b == 0x57: return "."
    if b == 0xF2: return "..."
    return "?%02x" % b


def glyphen(d, st):
    """Zeichenfolge (nur druckbare Glyphen + 0x08) ab st+2 bis 0x01; Steuercodes mit
    Parameter werden uebersprungen. Liefert (liste_der_bytes, text) oder None bei Auswahl."""
    i = st + 2
    out = []
    while i < len(d) and i < st + 800:
        b = d[i]
        if b == 0x01:
            return out
        if b in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
            i += 2
            continue
        if b in (0x03, 0x07):
            return None
        out.append(b)
        i += 1
    return out


def schluss(bs):
    druck = [b for b in bs if b != 0x08]
    if not druck:
        return "leer"
    if druck[-1] == 0xF2 or druck[-3:] == [0x57, 0x57, 0x57]:
        return "'...'"
    if druck[-1] == 0x57:
        return "'.'"
    if druck[-1] == 0x1A:
        return "'!'"
    if druck[-1] == 0x1B:
        return "'?'"
    return "sonst(%02x)" % druck[-1]


def main():
    alle = collections.Counter()
    ein = collections.Counter()
    fuehr = collections.Counter()
    fuehr_ein = collections.Counter()
    fuehr_misserfolg = []
    misserfolg = []
    n = 0
    for p in sorted(glob.glob(os.path.join(REPO, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
        d = open(p, "rb").read()
        if len(d) < 0x60:
            continue
        ms = struct.unpack_from("<I", d, 0x3C)[0]
        if ms == 0 or ms >= len(d):
            continue
        first = d[ms] | (d[ms + 1] << 8)
        cnt = first // 2
        if cnt == 0 or cnt > 64:
            continue
        tbl = [d[ms + 2 * i] | (d[ms + 2 * i + 1] << 8) for i in range(cnt)]
        for idx, o in enumerate(tbl):
            st = ms + o
            if d[st:st + 2] != b"\x04\x02":
                continue
            bs = glyphen(d, st)
            if bs is None:
                continue
            n += 1
            lead = bool(bs) and bs[0] == 0x08
            inner = bs[1:] if lead else bs
            zeilen = inner.count(0x08) + 1
            s = schluss(bs)
            alle[s] += 1
            fuehr[lead] += 1
            text = "".join(" / " if b == 0x08 else g(b) for b in bs)
            if zeilen == 1:
                ein[s] += 1
                fuehr_ein[lead] += 1
            low = text.lower()
            if any(w in low for w in ("respond", "nothing", "happen", "won't", "doesn't", "no use",
                                      "can't", "locked")):
                eintrag = "%s msg %d @0x%05X lead08=%d zeilen=%d schluss=%s  %r" % (
                    os.path.basename(p), idx, st, lead, zeilen, s, text)
                misserfolg.append(eintrag)
                if lead:
                    fuehr_misserfolg.append(eintrag)
    print("# %d Untersuchungstexte (Kopf 04 02, ohne Auswahl) in allen RDTs" % n)
    print("\n## Schluss (alle)")
    for k, v in alle.most_common():
        print("  %-12s %4d (%.1f %%)" % (k, v, 100.0 * v / n))
    ne = sum(ein.values())
    print("\n## Schluss (nur EINZEILIGE, %d)" % ne)
    for k, v in ein.most_common():
        print("  %-12s %4d (%.1f %%)" % (k, v, 100.0 * v / ne))
    print("\n## fuehrendes 0x08: alle %d von %d, einzeilige %d von %d" % (
        fuehr[True], n, fuehr_ein[True], ne))
    print("\n## Misserfolgs-/Leer-Texte (%d, davon %d mit fuehrendem 08)" % (
        len(misserfolg), len(fuehr_misserfolg)))
    for e in misserfolg:
        print("  " + e)


if __name__ == "__main__":
    main()
