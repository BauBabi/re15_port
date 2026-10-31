"""Runde 41 Spur Q (Abnahme M 5 Mangel 2): Teile-Bericht aus dem Lauf, der ausgeliefert ist.

Dossier: analysis/befunde_runde41/Q_restpunkte.md, Punkt 2. Die Datei analysis/befunde_runde41/M_bilder/NB5/teile_nb5.txt stammte aus dem Lauf
pl_a2 (alte Kamera geom.cut_view), MASKS/ aus pl_a3 (Kamera des Spiels, ansicht_spiel): 21 Teile mit anderem Ergebnis. Dieses Werkzeug schreibt den
Bericht aus platzieren_r41.json (--inc-Lauf von platzieren_r41.py) und gibt die Tabelle der Teile ohne Koerper fuer die Freistellungs-Liste aus.

Format wie der Bericht der NB5: je Sektion mit einem Teil die Kopfzeile, je Teil Lage, RMS, Punkte und entweder Koerper oder OHNE + Grund. Teile, die
keinen neuen Punkt bringen (schon von alten Rechtecken gedeckt), stehen nicht im Bericht.

Grund-Kuerzel der Liste: Z keine Zelle traegt die Mehrheit der Bodenkontakte, H kein Bodenkontakt, B Zelle auf mehreren Baendern, N Kamera ueber dem
Grundriss, S Saeule mit schraeger Zelle/kein Punkt getroffen.

Aufruf:
  python re15_port/tools/maske/teile_bericht_r41.py <platzieren_r41.json> <Lauf-Name> <aus.txt> [--liste <aus.md>] [--nachtrag <Text je Teil-Schluessel ROOM:Cut:Teil=Text>]
"""
import argparse
import json


def kuerzel(g):
    """Grund-Text von koerper_teil -> Kuerzel der Liste "Fuer den Nutzer"."""
    if g.startswith("Zelle auf mehreren"):
        return "B"
    if g.startswith("Kamera ueber dem Grundriss"):
        return "N"
    if "kein Bodenkontakt" in g and "keine Zelle" not in g:
        return "H"
    if "Zelltyp" in g or "Saeule" in g or "kein Sehstrahl" in g or "kein Punkt" in g:
        return "S"
    return "Z"


def zeile_teil(e, nachtrag):
    kopf = "  T%-3d (%4d,%4d) RMS %5.2f/%s px %5d | " % (e["komp"], e["shift"][0], e["shift"][1], e["rms"], e["rms2"],
                                                       e["neu_px"] if e.get("koerper") else e["ohne_koerper_px"])
    if e.get("koerper"):
        k = e["koerper"]
        s = "Zelle %s Typ %d Band %d Hoehe %d @0x%05X [%s] %s | Tiefe %d..%d, geerbt %d" % (
            k["zelle"], k["typ"], k["band"], k["hoehe"], k["off"], k["sca"], k["quelle"], e["tiefe"][0], e["tiefe"][1], e["geerbt"])
    else:
        s = "OHNE: %s" % e["ohne_koerper"]
    return kopf + s + (nachtrag and (" | " + nachtrag) or "")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("json")
    ap.add_argument("lauf")
    ap.add_argument("aus")
    ap.add_argument("--liste", default=None)
    ap.add_argument("--nachtrag", action="append", default=[])
    a = ap.parse_args()
    b = json.load(open(a.json))
    nach = {}
    for n in a.nachtrag:
        k, t = n.split("=", 1)
        nach[k] = t
    aus = ["# Runde 41 Spur M, Nachbesserung 5 (Entscheid M-E2): Teile des Original-Vordergrundatlas je Sektion (platzieren_r41.py, Lauf %s)." % a.lauf,
           "# Lage: SSD-Verschiebung (dx,dy) Atlas->Bild, RMS / zweitbeste RMS (Regel 1/2). Koerper: Zelle x z breite tiefe, Typ, Band, Hoehe",
           "# (Oberkante auf der Eintrittsflaeche), RDT-Offset + 12 SCA-Bytes; Tiefe min..max der Bildpunkte (bucket(Eintritt)); px = neue Punkte.",
           "# OHNE = kein Koerper (Grund) -> nicht eingesetzt (Liste 'Fuer den Nutzer: Freistellungen').",
           "# Geschrieben von tools/maske/teile_bericht_r41.py (Runde 41 Spur Q) aus platzieren_r41.json dieses Laufs; dieser Lauf ist der ausgelieferte Stand von MASKS/."]
    liste = []
    n_mit = n_ohne = px_mit = px_ohne = 0
    for room in sorted(b):
        for cut in sorted(b[room], key=int):
            c = b[room][cut]
            mit = [e for e in c.get("komponenten", []) if e.get("angenommen") and e.get("koerper") and e.get("neu_px", 0) > 0]
            ohne = [e for e in c.get("komponenten", []) if e.get("angenommen") and e.get("ohne_koerper")]
            if not mit and not ohne:
                continue
            aus.append("%s Cut %s: neue Rechtecke %d (erstes %s), neue Punkte %d" % (room, cut, c.get("neu", 0), c.get("erstes_neues", "-"), c.get("neu_px", 0)))
            for e in sorted(mit + ohne, key=lambda e: e["komp"]):
                aus.append(zeile_teil(e, nach.get("%s:%s:%d" % (room, cut, e["komp"]), "")))
            n_mit += len(mit)
            n_ohne += len(ohne)
            px_mit += sum(e["neu_px"] for e in mit)
            px_ohne += sum(e["ohne_koerper_px"] for e in ohne)
            if ohne:
                gr = {}
                for e in ohne:
                    k = kuerzel(e["ohne_koerper"])
                    gr[k] = gr.get(k, 0) + e["ohne_koerper_px"]
                rid = int(room[4:], 16)
                liste.append((room, rid | 1, int(cut), sum(e["ohne_koerper_px"] for e in ohne), sorted(e["komp"] for e in ohne),
                              sorted(gr.items(), key=lambda kv: -kv[1])))
    aus.append("# Summe: %d Teile mit Koerper (%d Punkte), %d ohne Koerper (%d Punkte)" % (n_mit, px_mit, n_ohne, px_ohne))
    open(a.aus, "w", encoding="utf-8", newline="\n").write("\n".join(aus) + "\n")
    print("Bericht %s: %d mit (%d Punkte), %d ohne (%d Punkte), %d Sektionen in der Liste" % (a.aus, n_mit, px_mit, n_ohne, px_ohne, len(liste)))
    if a.liste:
        with open(a.liste, "w", encoding="utf-8", newline="\n") as f:
            for room, e, cut, px, teile, gr in liste:
                f.write("  | %s/%04X | %d | %d | %s | %s |  |\n" % (room, e, cut, px, " ".join("T%d" % t for t in teile), ", ".join("%s %d" % g for g in gr)))


if __name__ == "__main__":
    main()
