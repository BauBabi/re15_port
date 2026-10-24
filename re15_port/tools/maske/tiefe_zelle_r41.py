"""Runde 41 Spur M, Nachbesserung 1 (Abnahme 0, Mangel 1): AUSGESCHLOSSENE Generator-Rechtecke der Stages 3..6 mit der
Tiefe ihrer KOLLISIONSZELLE statt der Tiefe des Generators.

Dossier: analysis/befunde_runde41/M_masken36.md, Abschnitt "Nachbesserung 1 / Mangel 1".

Ausgangslage: der Ausschluss der Spur P (R37, engine/src/masken36_r37.c + gen/masken36_ausschluss_r37.inc) nimmt jedes
Rechteck heraus, das an einem erreichbaren Stehplatz einen Figurpunkt ohne solide Zelle davor verdecken wuerde. Ursache
ist die Tiefe des Generators vom 2026-09-03 (gen_masken.depth_map_for_cut: Bodenkontakt der UNTERSTEN Regionszeile je
Spalte auf y = 0, mal DEPTH_SAFETY 0.90 = 10 % naeher). Die Abnahme verlangt die Tiefe je Objekt aus der Kollisionszelle /
Geometrie (wie raum.py "zelle"), NICHT aus der Erreichbarkeit (Weg 1.5, gemessen und verworfen).

Regel (dieselbe wie fuer die Nutzer-Freistellungen STAGE1/2, geometrie.py Phase 2 "(i) Zelle"):
  1. Objekt = 8-Komponente der verifizierten Generator-Region einer Verschiebung (schliessen_r41.gewachsen).
  2. Ebene: das einzige begehbare Band des Raums (geom.begehbare_baender); bei mehreren Baendern jedes Band einzeln -
     gilt nur, wenn GENAU EIN Band eine Zelle liefert (sonst bleibt das Rechteck aus).
  3. Zelle = geometrie.zelle_zuordnen (Mehrheit der Bodenkontakte des Objekts in einer soliden Sperrzelle des Bands),
     Hoehe = geometrie.hoehe_messen (IoU-Maximum der Quader-Silhouette gegen das Objekt; "Saeule" wenn die IoU bis zum
     Rand des Messbereichs waechst), Kamera-z je Bildpunkt = Sehstrahl gegen diese Zelle (geometrie.quader_auf_band /
     geom.kollisionstiefe_schnell) - OHNE Faktor, OHNE Erben: nur Bildpunkte, die der Sehstrahl DIREKT trifft.
  4. Ein ausgeschlossenes Rechteck bekommt die neue Tiefe nur, wenn JEDER seiner deckenden Punkte direkt getroffen ist;
     Tiefe = max bucket(vz) ueber seine Punkte (Statistik "max" wie raum.py --p2 --statistik max; bucket(vz) =
     (1023*vz)>>16, Figur-OT @0x8002565c sra v1,v1,4 mit ZSF3 = 341 @0x80066c70; Maske verdeckt gdw. depth < bucket,
     Masken-OT = depth @0x80039650). Warum das sicher ist: ein Figurpunkt wird nur verdeckt, wenn er auf dem Sehstrahl
     HINTER dem Eintritt in die Zelle liegt; Figuren stehen ausserhalb solider Zellen (Spielerradius 450,
     DAT_80073e94[6]) - also hinter ihr.
  4b. Schritt 2 (--erben, Nachbesserung 1 nach der exe-Pruefung von Schritt 1: 39 DAVOR-Szenen ohne neu uebermalten
     Leon-Punkt): Rechtecke, deren Punkte die Zelle NICHT alle direkt trifft, bekommen die Tiefe wie raum.py "zelle" fuer
     die Nutzer-Freistellungen (geometrie.tiefe_zelle): Punkte ohne Treffer erben den naechsten Treffer desselben Objekts
     (geometrie._erben), Rechteck-Tiefe = max bucket. Das Netz der Spur P (Regel 6) entscheidet danach wie in Schritt 1.
  5. Die Profil-Tiefe (geometrie "(ii)+(iii)", ohne Zelle) wird NICHT benutzt: gemessen ROOM4080 Cut 2, rechte Gangwand:
     Profil-Tiefe max 88, Leon VOR der Wand bei (10475,-13710) bucket 134..148 -> die Wand haette ihn uebermalt, und der
     Ausschluss sieht es nicht (die Grundriss-Strecke Kamera -> Leon kreuzt die schraegen Typ-2/6-Zellen, in deren Huelle
     die Kamera steht; test_r37_masken36 punkt: "ohne Zelle davor verdeckt: 0").
  6. Danach rechnet test_r37_masken36 "erzeuge" den Ausschluss NEU (das Netz der Spur P bleibt): was weiterhin eine Figur
     ohne Zelle davor verdecken wuerde, bleibt aus. Rechtecke, die trotz Zellen-Tiefe aus bleiben, bekommen ihre alte
     Tiefe zurueck (--zurueck <inc>), damit die Datei nur dort abweicht, wo es wirkt.
Nicht ausgeschlossene Rechtecke bleiben bytegleich; Atlas (_PRIcc.TIM), Lage, Groesse, Gruppen unveraendert - nur das
Tiefenfeld (+4 im 12-Byte-Satz, pack_section) der betroffenen Rechtecke. Leon- und Elza-Datei bleiben bytegleich.

Aufruf:
  python re15_port/tools/maske/tiefe_zelle_r41.py --ppm <probe_bg_dump> --cache <Regionen-Cache> --aus <inc> --out <Ordner>
         [--zurueck <inc nach erzeuge>] [ROOMxxxx ...]
Schreibt <out>/ROOMxxxx.MSK (beide Varianten) und <out>/tiefe_zelle_r41.json (je Rechteck alt/neu/Zelle/Hoehe/IoU).
"""
import argparse
import json
import os
import re
import struct
import sys

import numpy as np
from scipy import ndimage

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)

import geom                      # noqa: E402
import geometrie as G            # noqa: E402
import maskenbild                # noqa: E402
import original                  # noqa: E402
import schliessen_r41 as SR      # noqa: E402

ERBEN = False                    # --erben (Nachbesserung 1, Schritt 2), s. Kopf Regel 4b
ZUORDNUNG = {}                   # Nachbesserung 2 (zellen3d_r41.py --zuordnung): (raum, cut) -> Eintraege von Hand
NAEHER = False                   # Nachbesserung 2 (zellen3d_r41.py): Zellen-Tiefe auch dann, wenn sie NAEHER als die alte
                                 # liegt - die 3D-Regel des Ausschlusses (test_r37_masken36.c) urteilt dann je Figurpunkt

REPO = SR.REPO
PSX = SR.PSX
MASKS = SR.MASKS


def ausschluss(pfad):
    """gen/masken36_ausschluss_r37.inc -> {(raum, cut): set(rechteck)}"""
    t = open(pfad).read()
    out = {}
    for m in re.finditer(r"\{ 0x([0-9A-F]{4}),\s*(\d+),\s*(\d+), \{ 0x([0-9A-F]{8})u, 0x([0-9A-F]{8})u, "
                         r"0x([0-9A-F]{8})u, 0x([0-9A-F]{8})u \} \}", t):
        raum, cut = int(m.group(1), 16), int(m.group(2))
        if raum:
            bits = [int(m.group(i), 16) for i in range(4, 8)]
            out[(raum, cut)] = set(i for i in range(128) if (bits[i >> 5] >> (i & 31)) & 1)
    return out


def satz_offsets(blob, cut):
    """Datei-Offset jedes Rechteck-Satzes der Sektion (Leser wie SR.sektionen / re15_pri_parse_section)."""
    _, nc = struct.unpack_from("<II", blob, 4)
    o = struct.unpack_from("<I", blob, 12 + 4 * cut)[0]
    gc, _mc = struct.unpack_from("<HH", blob, o)
    p = o + 4
    n = 0
    for _ in range(gc):
        n += struct.unpack_from("<H", blob, p)[0]
        p += 8
    offs = []
    for _ in range(n):
        offs.append(p)
        size = struct.unpack_from("<H", blob, p + 6)[0]
        p += 8 + (4 if (size & 0xF000) == 0 else 0)
    return offs


def kameraort(R, t):
    """Kameraort in der Welt = R^-1 * (-t) (wie geom.kollisionstiefe_schnell)."""
    Rm = np.array(R, float).reshape(3, 3) / 4096.0
    return np.linalg.inv(Rm).dot(-np.array(t, float))


def zelle_geometrie(rdt, R, t, H, o, y0, band, hull):
    """Phase-2-Regel (i) fuer EIN Objekt -> dict mit "vz"/"hit" (nur DIREKT getroffene Punkte) oder {"grund": ...}."""
    zellen = geom.sca_sperrzellen(rdt, band) or []
    if not zellen:
        return {"grund": "keine Sperrzelle auf Band %d" % band}
    stand, eigene, ohne = G.standpunkte(o, R, t, H, y0, hull)
    if not stand:
        return {"grund": "kein Bodenkontakt"}
    z, anteil = G.zelle_zuordnen(o, eigene, R, t, H, y0, zellen, hull)
    if z is None:
        return {"grund": "keine Zelle traegt die Mehrheit der Bodenkontakte (%.2f)" % anteil}
    return koerper_geometrie(R, t, H, o, y0, band, z, anteil)


def zelle_von_hand(rdt, e):
    """Nachbesserung 2: Zelle einer Zuordnung von Hand (zuordnung_r41.json, Feld "zelle" = RDT-Offset des SCA-Eintrags) ->
    ((x, z, breite, tiefe, typ), band). Der Offset muss ein Eintrag der SCA-Tabelle (RDT +0x20) und solide sein."""
    s = struct.unpack_from("<I", rdt, 0x20)[0]
    n = sum(struct.unpack_from("<5I", rdt, s + 4))
    off = int(e["zelle"], 16)
    if not (s + 24 <= off < s + 24 + 12 * n) or (off - s - 24) % 12:
        raise SystemExit("Zuordnung %s: 0x%X ist kein SCA-Eintrag" % (e.get("objekt"), off))
    w, d, x, z = struct.unpack_from("<HHhh", rdt, off)
    t_, u0, _u1, f = rdt[off + 8:off + 12]
    if not (u0 & 1):
        raise SystemExit("Zuordnung %s: Zelle 0x%X nicht solide" % (e.get("objekt"), off))
    return (x, z, w, d, t_ & 15), f >> 4


def koerper_geometrie(R, t, H, o, y0, band, z, anteil):
    """Koerper einer zugeordneten Zelle: Hoehe gemessen, Sehstrahl je Punkt (Rest von zelle_geometrie)."""
    hm = G.hoehe_messen(o, R, t, H, z, y0)
    if hm is None:
        return {"grund": "Zelle trifft das Objekt bei keiner Hoehe"}
    hoehe, iou, kante, saeule = hm
    typ = int(z[4])
    # Ausschluesse des Modells (keine Zahl gewaehlt - beides sind Faelle, in denen der Sehstrahl KEINE Eintrittsflaeche hat):
    #  * Saeule mit nicht-rechteckiger Zelle: der Saeulen-Weg (geom.kollisionstiefe_schnell) kennt nur das Huellrechteck
    #    (geometrie.tiefe_zelle "OFFEN UND LAUT"); eine Diagonale als Rechteck geraycastet liegt falsch (geom.py Kopf
    #    "FORM einer SCA-Zelle", ROOM10E0 Cut 7: 1739 Einheiten).
    #  * Kamera IN der Zelle (Grundriss in der Form, bei einem Quader auch unter dessen Oberkante): der Strahl beginnt im
    #    Festkoerper, getroffen wird nur die AUSTRITTSflaeche - keine Oberflaeche, die im Bild zu sehen waere.
    if saeule and typ != 1:
        return {"grund": "Saeule mit Zellentyp %d (nur Huellrechteck modelliert)" % typ}
    c = kameraort(R, t)
    drin = (z[0] <= c[0] <= z[0] + z[2]) and (z[1] <= c[2] <= z[1] + z[3])
    if drin:
        form = geom.zellen_solid_test(typ, z[0], z[1], z[2], z[3])
        if form is not None:
            drin = bool(form(c[0], c[2]))
    if drin and (saeule or (y0 + hoehe) <= c[1] <= y0):
        return {"grund": "Kamera in der Zelle"}
    if saeule:
        vz = geom.kollisionstiefe_schnell(R, t, H, [(z[0], z[1], z[2], z[3])]) * 64.0
        tr = vz > 0
    else:
        vz, tr = G.quader_auf_band(R, t, H, z[0], z[0] + z[2], z[1], z[1] + z[3], hoehe, y0, typ)
    hit = o & tr
    if not hit.any():
        return {"grund": "kein Punkt direkt getroffen"}
    return {"vz": np.where(hit, vz, 0.0), "hit": hit, "zelle": [int(v) for v in z[:4]], "typ": typ,
            "hoehe": int(hoehe), "iou": round(float(iou), 3), "saeule": bool(saeule),
            "anteil": round(float(anteil), 3) if anteil is not None else None,
            "band": int(band), "treffer": int(hit.sum()), "px": int(o.sum())}


def kandidaten(room, cut, blob, sek, aus, rdt, cam, cache):
    """-> Liste dict je ausgeschlossenem Rechteck (neu = None: keine Zellen-Tiefe)."""
    rid = int(room[4:], 16)
    v = geom.cut_view(rdt, cam, cut)
    if not v or v[2] <= 0:
        return []
    R, t, H = v
    own = maskenbild.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))[0]
    ms = maskenbild.masken(blob, cut) or []
    grown = SR.gewachsen(None, None, None, cache)
    per, _alle = SR.region_im_bild(grown)
    labs = {sh: ndimage.label(f, structure=np.ones((3, 3)))[0] for sh, f in per.items()}
    hull = G.huelle(rdt)
    baender = sorted(b for b in geom.begehbare_baender(rid) if 0 <= b < 16)
    geo = {}

    def objekt(sh, j):
        if (sh, j) not in geo:
            o = labs[sh] == j
            for e in ZUORDNUNG.get((room, cut), []):      # Nachbesserung 2: Zelle von Hand (Beleg im Eintrag)
                if tuple(e["verschiebung"]) == tuple(sh) and labs[sh][e["punkt"][1], e["punkt"][0]] == j:
                    z, b = zelle_von_hand(rdt, e)
                    g = koerper_geometrie(R, t, H, o, -b * geom.BAND_HOEHE, b, z, None)
                    if "grund" not in g:
                        g["von_hand"] = e["zelle"]
                    geo[(sh, j)] = g
                    return g
            treffer, gruende = [], []
            for b in baender:
                g = zelle_geometrie(rdt, R, t, H, o, -b * geom.BAND_HOEHE, b, hull)
                if "grund" in g:
                    gruende.append("Band %d: %s" % (b, g["grund"]))
                else:
                    treffer.append(g)
            if len(treffer) == 1:
                geo[(sh, j)] = treffer[0]
            elif treffer:
                geo[(sh, j)] = {"grund": "Zelle auf mehreren Baendern %s" % [g["band"] for g in treffer]}
            else:
                geo[(sh, j)] = {"grund": "; ".join(gruende) or "kein begehbares Band"}
        return geo[(sh, j)]

    out = []
    for i, (sx, sy, X, Y, w, h, dep) in enumerate(ms):
        if i not in aus:
            continue
        d = np.zeros((240, 320), bool)
        x0, x1 = max(0, X), min(320, X + w)
        y0_, y1 = max(0, Y), min(240, Y + h)
        if x1 > x0 and y1 > y0_:
            sub = own[sy + (y0_ - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
            if sub.shape == (y1 - y0_, x1 - x0):
                d[y0_:y1, x0:x1] = sub != 0
        r = {"i": i, "alt": int(dep), "neu": None, "px": int(d.sum())}
        out.append(r)
        if not d.any():
            r["grund"] = "keine deckenden Punkte"
            continue
        shs = [sh for sh, f in per.items() if f[d].all()]
        if not shs:
            r["grund"] = "nicht in der Region einer Verschiebung"
            continue
        sh = shs[0]
        comps = sorted(set(int(c) for c in labs[sh][d]))
        vz = np.zeros((240, 320))
        hit = np.zeros((240, 320), bool)
        info = []
        for c in comps:
            g = objekt(sh, c)
            if "grund" in g:
                r["grund"] = "Objekt: " + g["grund"]
                break
            o = labs[sh] == c
            vz[o] = g["vz"][o]
            hit[o] = g["hit"][o]
            info.append(dict({k: g[k] for k in ("zelle", "typ", "hoehe", "iou", "saeule", "anteil", "band", "treffer", "px")},
                             von_hand=g.get("von_hand")))
        else:
            if not hit[d].all():
                if not ERBEN:
                    r["grund"] = "nicht jeder Punkt direkt getroffen (%d von %d)" % (int(hit[d].sum()), int(d.sum()))
                    r["zellen"] = info
                    continue
                # --erben (Schritt 2): wie raum.py "zelle" (geometrie.tiefe_zelle): Punkte des Objekts ohne Treffer erben den
                # naechsten Treffer (geometrie._erben, Distanztransformation); die Rechteck-Tiefe bleibt das Maximum.
                obj = np.zeros((240, 320), bool)
                for c in comps:
                    obj |= labs[sh] == c
                vz = G._erben(np.where(hit, vz, 0.0), obj | d)
                r["geerbt"] = int((d & ~hit).sum())
            neu = int(G.rastern(vz[d]).max())
            r["zellen"] = info
            r["vz_min"] = int(G.rastern(vz[d]).min())
            if neu <= dep:
                r["grund"] = "Zellen-Tiefe %d nicht ferner als %d" % (neu, dep)
                if not NAEHER:
                    continue
                r["naeher"] = True
            r["neu"] = neu
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("raeume", nargs="*")
    ap.add_argument("--cache", required=True, help="Regionen-Cache (schliessen_r41 --cache)")
    ap.add_argument("--aus", required=True, help="Ausschluss-Tabelle, gegen die gerechnet wird")
    ap.add_argument("--zurueck", default=None, help="Tabelle nach erzeuge: dort weiter ausgeschlossene -> alte Tiefe")
    ap.add_argument("--quelle", default=MASKS, help="MSK-Ordner (Stand vor diesem Schritt)")
    ap.add_argument("--out", required=True)
    ap.add_argument("--erben", action="store_true",
                    help="Schritt 2: Rechtecke mit nicht direkt getroffenen Punkten wie raum.py 'zelle' (Fehltreffer erben)")
    a = ap.parse_args()
    global ERBEN
    ERBEN = a.erben
    os.makedirs(a.out, exist_ok=True)
    aus = ausschluss(a.aus)
    zur = ausschluss(a.zurueck) if a.zurueck else None
    bericht = {}
    for f in sorted(os.listdir(a.quelle)):
        if not re.match(r"ROOM[3-6][0-9A-F]{2}0\.MSK$", f):
            continue
        room = f[:8]
        rid = int(room[4:], 16)
        if a.raeume and room not in a.raeume:
            continue
        blob = bytearray(open(os.path.join(a.quelle, f), "rb").read())
        zw = open(os.path.join(a.quelle, "ROOM%04X.MSK" % (rid | 1)), "rb").read()
        if zw != bytes(blob):
            raise SystemExit("%s: Elza-Datei nicht bytegleich" % room)
        rdt = open(os.path.join(PSX, "STAGE%d" % (rid >> 12), "%s.RDT" % room), "rb").read()
        cam = struct.unpack_from("<I", rdt, 0x24)[0]
        secs, n_cut = SR.sektionen(bytes(blob))
        b_raum = {}
        for cut, sek in sorted(secs.items()):
            al = aus.get((rid, cut), set())
            ae = aus.get((rid | 1, cut), set())
            if al != ae:
                raise SystemExit("%s Cut %d: Ausschluss Leon/Elza verschieden" % (room, cut))
            if not al:
                continue
            cache = os.path.join(a.cache, "%s_%02d.npz" % (room, cut))
            if not os.path.exists(cache):
                raise SystemExit("Regionen-Cache fehlt: %s (schliessen_r41.py --cache)" % cache)
            ks = kandidaten(room, cut, bytes(blob), sek, al, rdt, cam, cache)
            offs = satz_offsets(bytes(blob), cut)
            for k in ks:
                if k["neu"] is None:
                    continue
                if zur is not None and k["i"] in zur.get((rid, cut), set()):
                    k["zurueck"] = True        # trotz Zellen-Tiefe weiter aus -> alte Tiefe bleibt
                    continue
                struct.pack_into("<H", blob, offs[k["i"]] + 4, k["neu"])
            b_raum[cut] = ks
            n_neu = sum(1 for k in ks if k["neu"] is not None and not k.get("zurueck"))
            print("%s C%-2d ausgeschlossen %3d, Zellen-Tiefe %3d" % (room, cut, len(al), n_neu), flush=True)
        for r in (rid, rid | 1):
            open(os.path.join(a.out, "ROOM%04X.MSK" % r), "wb").write(bytes(blob))
        bericht[room] = b_raum
    json.dump(bericht, open(os.path.join(a.out, "tiefe_zelle_r41.json"), "w"), indent=1)


if __name__ == "__main__":
    main()
