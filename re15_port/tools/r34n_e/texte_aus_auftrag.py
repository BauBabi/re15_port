# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: die vier Dokumenttexte WOERTLICH aus AUFTRAG.md ziehen.

Kein Abtippen: jede Zeile kommt aus analysis/befunde_runde34_nacht/AUFTRAG.md (Nutzerwortlaut).
Regeln (keine Messung, nur Entpacken des Zitats):
  * Zitatpraefix '>' und genau EIN folgendes Leerzeichen entfernen, danach fuehrende Tabs
    (die Einrueckung der Aufzaehlung im Chat) entfernen.
  * 'Überschrift: ' und 'Text: ' sind Beschriftungen des Nutzers, nicht Teil des Texts.
  * Eine Zeile '>' allein = Leerzeile (Absatzgrenze) im Text.
  * Der Text endet vor der ersten Zeile, die nicht mehr zum Dokument gehoert (Zeilennummern
    unten, jeweils gegen den Inhalt geprueft).

Aufruf:  python re15_port/tools/r34n_e/texte_aus_auftrag.py
Ausgabe: analysis/befunde_runde34_nacht/E_texte/dok{1..4}_*.txt  +  texte_bericht.txt
"""
import hashlib
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
AUFTRAG = os.path.join(REPO, "analysis", "befunde_runde34_nacht", "AUFTRAG.md")
AUS = os.path.join(REPO, "analysis", "befunde_runde34_nacht", "E_texte")

# (Datei, Titelzeile, erste Textzeile, letzte Textzeile) — 1-basierte Zeilennummern in AUFTRAG.md
DOKS = [
    ("dok1_police_officer", 22, 23, 27),
    ("dok2_elliot",         29, 30, 38),
    ("dok3_marvin",         41, 42, 48),
    ("dok4_armory",         55, 56, 58),
]


def entpacken(z):
    if not z.startswith(">"):
        raise SystemExit("keine Zitatzeile: %r" % z)
    z = z[1:]
    if z.startswith(" "):
        z = z[1:]
    return z.lstrip("\t")


def main():
    zeilen = open(AUFTRAG, encoding="utf-8").read().split("\n")
    os.makedirs(AUS, exist_ok=True)
    bericht = ["Quelle: %s (md5 %s)" % (os.path.relpath(AUFTRAG, REPO).replace("\\", "/"),
                                       hashlib.md5(open(AUFTRAG, "rb").read()).hexdigest()), ""]
    for name, zt, z0, z1 in DOKS:
        titel = entpacken(zeilen[zt - 1])
        if not titel.startswith("Überschrift: "):
            raise SystemExit("Zeile %d ist keine Ueberschrift: %r" % (zt, titel))
        titel = titel[len("Überschrift: "):]
        text = [entpacken(z) for z in zeilen[z0 - 1:z1]]
        if not text[0].startswith("Text: "):
            raise SystemExit("Zeile %d beginnt nicht mit 'Text: ': %r" % (z0, text[0]))
        text[0] = text[0][len("Text: "):]
        # die Zeile NACH dem Text darf nicht mehr zum Dokument gehoeren
        nach = zeilen[z1] if z1 < len(zeilen) else ""
        inhalt = "\n".join(text) + "\n"
        nicht_ascii = sorted(set(c for c in titel + inhalt if ord(c) > 126 or (ord(c) < 32 and c != "\n")))
        pfad = os.path.join(AUS, name + ".txt")
        open(pfad, "w", encoding="utf-8", newline="\n").write(inhalt)
        open(os.path.join(AUS, name + "_titel.txt"), "w", encoding="utf-8", newline="\n").write(titel + "\n")
        absaetze = [a for a in inhalt.split("\n\n")]
        bericht.append("%s: Titel %r (AUFTRAG.md Z. %d), Text Z. %d..%d" % (name, titel, zt, z0, z1))
        bericht.append("   %d Zeichen, %d Zeilen (davon leer %d), %d Woerter, md5 %s"
                       % (len(inhalt), len(text), sum(1 for t in text if t == ""),
                          len(inhalt.split()), hashlib.md5(inhalt.encode("utf-8")).hexdigest()))
        bericht.append("   Zeichen ausserhalb ASCII 32..126: %r" % nicht_ascii)
        bericht.append("   Zeile danach (gehoert NICHT dazu): %r" % nach[:70])
        bericht.append("   verschiedene Zeichen: %s" % "".join(sorted(set(inhalt.replace("\n", "")))))
        bericht.append("")
    open(os.path.join(AUS, "texte_bericht.txt"), "w", encoding="utf-8", newline="\n").write(
        "\n".join(bericht) + "\n")
    print("\n".join(bericht))


if __name__ == "__main__":
    main()
