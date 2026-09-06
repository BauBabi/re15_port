"""Auswahl anwenden: aus Flaechennummern werden Maskendaten.

Gegenstueck zu blatt.py. Die Nummern kommen von dort — angesehen und benannt, nicht
geraten. Alles Weitere ist gemessene Mechanik: Rechteckzerlegung, Tiefe aus dem
Bodenkontakt je Bildspalte, Atlas aus dem Hintergrund gepackt, Ausgabe im
Original-Sektionsformat.

    --vergleich   prueft die Auswahl gegen die KUENSTLER-Masken desselben Cuts.
                  Nur fuer Cuts, die welche haben — das ist die Abnahmepruefung,
                  bevor man der Auswahl auf unbekannten Cuts glaubt.
    sonst         schreibt MASKS/<ROOM>.MSK + MASKS/<ROOM>_PRI<cc>.TIM

Aufruf:
    python re15_port/tools/maske/anwenden.py ROOM1150 1 --ids 37,38,43,46 --vergleich
    python re15_port/tools/maske/anwenden.py ROOM1140 0 --ids 12,13,14
"""
import argparse
import json
import os
import struct
import sys

import numpy as np
from scipy import ndimage

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import atlas as atlasmod
import geom
from geom import load_bg, load_rdt


def kaesten(text):
    """"x0,y0,x1,y1;x0,y0,x1,y1" -> [(x0,y0,x1,y1)] in Bildkoordinaten."""
    out = []
    for teil in text.split(";"):
        teil = teil.strip()
        if not teil:
            continue
        v = [int(x) for x in teil.split(",")]
        out.append((max(0, v[0]), max(0, v[1]), min(320, v[2]), min(240, v[3])))
    return out


def spalten_fuellen(region, art):
    """Kerben je Bildspalte schliessen — je Zusammenhangskomponente (Standard) oder global."""
    if art == "global":
        for x in range(region.shape[1]):
            ys = np.nonzero(region[:, x])[0]
            if len(ys):
                region[ys.min():ys.max() + 1, x] = True
        return region
    lab, n = ndimage.label(region, structure=np.ones((3, 3)))
    out = region.copy()
    for k in range(1, n + 1):
        m = lab == k
        for x in np.nonzero(m.any(0))[0]:
            ys = np.nonzero(m[:, x])[0]
            out[ys.min():ys.max() + 1, x] = True
    return out


def lade_seg(blattdir, room, cut, segments, ppm, rid):
    """Segmentierung laden oder erzeugen. Der Dateiname traegt die Feinheit — eine
    Auswahl ist nur zusammen mit IHRER Feinheit gueltig (die alten Dateien ohne
    Feinheit im Namen liessen sich nicht mehr reproduzieren)."""
    p = os.path.join(blattdir, "%s_%02d_s%d_seg.npy" % (room, cut, segments))
    if os.path.exists(p):
        return np.load(p)
    alt = os.path.join(blattdir, "%s_%02d_seg.npy" % (room, cut))
    if os.path.exists(alt):
        return np.load(alt)
    from blatt import segment
    seg = segment(load_bg(ppm, rid, cut), segments)
    os.makedirs(blattdir, exist_ok=True)
    np.save(p, seg)
    return seg


def artist_region(rdt, cam, cut):
    """Vereinigung der Original-Maskenrechtecke, oder None."""
    po = struct.unpack_from("<I", rdt, cam + cut * 32 + 0x1C)[0]
    if po + 4 > len(rdt):
        return None
    gc, mc = struct.unpack_from("<HH", rdt, po)
    if gc == 0xFFFF or gc == 0 or mc == 0 or gc > 256:
        return None
    p = po + 4
    gn, gdx, gdy = [], [], []
    for _ in range(gc):
        n_, base, dx, dy = struct.unpack_from("<HHhh", rdt, p)
        gn.append(n_); gdx.append(dx); gdy.append(dy); p += 8
    reg = np.zeros((240, 320), bool)
    gi = used = 0
    for _ in range(sum(gn)):
        sx, sy, dx, dy = rdt[p], rdt[p + 1], rdt[p + 2], rdt[p + 3]
        dep, size = struct.unpack_from("<HH", rdt, p + 4); p += 8
        if (size & 0xf000) == 0:
            w, h = struct.unpack_from("<HH", rdt, p); p += 4
        else:
            w = h = (size >> 12) * 8
        while gi < gc and used >= gn[gi]:
            gi += 1; used = 0
        ax, ay = (gdx[gi], gdy[gi]) if gi < gc else (0, 0)
        used += 1
        X = ((dx + ax + 0x8000) & 0xffff) - 0x8000
        Y = ((dy + ay + 0x8000) & 0xffff) - 0x8000
        x0, x1 = max(0, X), min(320, X + w)
        y0, y1 = max(0, Y), min(240, Y + h)
        if x1 > x0 and y1 > y0:
            reg[y0:y1, x0:x1] = True
    return reg


def build(rdt, cam, rid, cut, region, bg, out_dir, room):
    dep = geom.depth_map(rdt, cam, cut, region)
    if dep is None:
        return None
    # Das Zerlegen uebergrosser Kaesten (atlas.split_oversize) kann die Zahl erhoehen —
    # ein Bildschirmrechteck ist bis 320 breit, das Atlasblatt nur 256, also bis zu
    # zwei Stuecke je Kasten. Deshalb wird die Anforderung so lange gesenkt, bis das
    # ZERLEGTE Ergebnis in das Budget passt. NICHT abschneiden: ein weggelassener
    # Kasten ist fehlende Verdeckung, und genau so entstand der Befund
    # "die Fuesse stehen auf dem Tisch".
    budget = geom.MAX_MASKS_PER_CUT
    while budget >= 4:
        boxes = geom.rects_from_mask(region, budget)
        if not boxes:
            return None
        if len(atlasmod.split_oversize(boxes)[0]) <= geom.MAX_MASKS_PER_CUT:
            break
        budget -= 4
    tim, place, boxes = atlasmod.build(bg, region, boxes)
    if tim is None:
        return None
    groups, masks = [], []
    for i, (x, y, w, h) in enumerate(boxes):
        if i not in place:
            continue
        ax, ay = place[i]
        win = dep[y:y + h, x:x + w]
        if not (win > 0).any():
            continue
        groups.append((1, x - ax, y - ay))
        masks.append((ax, ay, x, y, w, h, int(np.median(win[win > 0]))))
    if not masks:
        return None
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)
        open(os.path.join(out_dir, "%s_PRI%02d.TIM" % (room, cut)), "wb").write(tim)
    return geom.pack_section(groups, masks), len(masks)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room")
    ap.add_argument("cut", type=int)
    ap.add_argument("--ids", required=True, help="Flaechennummern, z.B. 37,38,43")
    ap.add_argument("--cd", default="re15_port/shared_assets/PSX")
    ap.add_argument("--ppm", default="build/bg_ppm")
    ap.add_argument("--blatt", default="build/blaetter")
    ap.add_argument("--out", default="re15_port/shared_assets/PSX/MASKS")
    ap.add_argument("--vergleich", action="store_true")
    # ⛔ WARUM NACHBEARBEITET WIRD (Nutzer-Befund 2026-09-03, zweimal gemeldet):
    # erst "Leons Fuesse stehen auf dem Tisch", dann "ein kleines bisschen Zombie und
    # ein kleines bisschen Leon-Fuss schaut noch durch". Gemessen an ROOM1140 Cut 0:
    # die Superpixelkante liegt im Mittel 1.8 px (stellenweise 4 px) UNTER der echten
    # Tischkante — ein Saum, der der Zerlegung inhaerent ist. Genau dort schaut der
    # Schuh durch.
    # Die Luecke sitzt OBEN (an der Objektoberkante), nicht unten oder seitlich.
    # Deshalb wird gezielt nach oben aufgeweitet; rundum aufweiten kostet nur
    # Praezision. Gemessen gegen die Kuenstler-Wahrheit (ROOM1150 Cut 1):
    #     roh                          99.7 % Praezision / 67.8 % Ausbeute
    #     Saum 2 rundum                99.3 % / 71.6 %
    #     Saum 4 rundum                98.7 % / 74.4 %
    #     oben 4 + Saum 1 + Spalten    98.8 % / 74.3 %
    #     oben 6 + Saum 1 + Spalten    98.4 % / 75.4 %   <- gewaehlt
    # 4 px reichten NICHT: der Nutzer meldete danach noch "fast minimal noch da", und
    # die Nachmessung gab ihm recht — die schlechteste Bildspalte lag weiter 2 px unter
    # der Tischkante. Mit 6 px ist die schlechteste Spalte bei +0 px, also lueckenlos.
    # Die Spaltenfuellung schliesst Kerben INNERHALB des Objekts (ein Tisch ist in
    # jeder Bildspalte durchgehend), ohne die Aussenkante zu verschieben.
    ap.add_argument("--oben", type=int, default=6, help="Aufweitung nach oben (Standard 6)")
    ap.add_argument("--grow", type=int, default=1, help="Saum rundum (Standard 1)")
    # ⛔ WARUM JE KOMPONENTE (Nutzer-Befund 2026-09-03, error02.png): die globale
    # Spaltenfuellung fuellte in ROOM1140 Cut 1 den Gang ZWISCHEN den Stuehlen mit
    # Maske — sie fuellt je Bildspalte alles zwischen oberstem und unterstem
    # markierten Pixel, und in einer Spalte liegen Stuhl (oben, fern) und Stuhl
    # (unten, nah) mit Teppich dazwischen. Der Teppich bekam damit die NAHE Tiefe
    # der unteren Kante und verdeckte Leon, der genau dort geht.
    # Gemessen gegen die Kuenstler-Silhouetten (build/kalib.py, ROOM1150 Cut 1/2/3,
    # Auswahl als perfekt angenommen, seg 220, oben 4, Saum 1):
    #     Fuellung global      Praezision 65.9 %  Ausbeute 93.0 %
    #     Fuellung Komponente  Praezision 70.6 %  Ausbeute 89.8 %
    #     Fuellung aus         Praezision 72.1 %  Ausbeute 88.2 %
    # Gewaehlt: Komponente — schliesst Kerben INNERHALB eines Objekts (dafuer war
    # sie da) und kann keine zwei Objekte mehr ueberbruecken.
    ap.add_argument("--fuellen", choices=("aus", "komponente", "global"),
                    default="komponente", help="Spaltenfuellung (Standard: komponente)")
    # Feinheit der Superpixel. 90 war zu grob: in ROOM1140 Cut 2 lag das Wandbild
    # MIT der Wand in EINER Flaeche, die dadurch nicht abwaehlbar war (error01.png).
    # Gemessen (dieselbe Reihe): seg 90 -> 67.2 % / 89.0 %, seg 220 -> 70.6 % / 89.8 %.
    ap.add_argument("--segments", type=int, default=220)
    # ⛔ WARUM ES KAESTEN GIBT (Nutzer-Befund 2026-09-03, error01.png): eine Superpixel-
    # Flaeche laesst sich nur GANZ oder GAR NICHT waehlen. In ROOM1140 Cut 2 lag das
    # Wandbild in derselben Flaeche wie Sofa und Stativ — gewaehlt hiess: die WAND
    # verdeckt Leon (ein Wandstreifen lag ueber seiner Schulter). Mit --minus wird
    # der Wandanteil weggeschnitten, ohne die Flaeche ganz aufzugeben.
    # Kaesten sind Bildkoordinaten x0,y0,x1,y1 (320x240), mehrere mit ";" getrennt.
    ap.add_argument("--minus", default="", help="Kaesten abziehen: x0,y0,x1,y1;...")
    ap.add_argument("--plus", default="", help="Kaesten hinzufuegen: x0,y0,x1,y1;...")
    a = ap.parse_args()

    room = a.room.upper()
    rid = int(room[4:], 16)
    rdt, _ = load_rdt(a.cd, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    seg = lade_seg(a.blatt, room, a.cut, a.segments, a.ppm, rid)
    bg = load_bg(a.ppm, rid, a.cut)
    ids = [int(x) for x in a.ids.replace(" ", "").split(",") if x]
    region = np.isin(seg, ids)
    for k in kaesten(a.plus):
        region[k[1]:k[3], k[0]:k[2]] = True
    for k in kaesten(a.minus):
        region[k[1]:k[3], k[0]:k[2]] = False
    if a.oben > 0:
        oben = region.copy()
        for k in range(1, a.oben + 1):
            oben[:-k] |= region[k:]
        region = oben
    if a.grow > 0:
        region = ndimage.binary_dilation(region, iterations=a.grow)
    if a.fuellen != "aus":
        region = spalten_fuellen(region, a.fuellen)
    # Nach der Nachbearbeitung erneut abziehen: Aufweitung und Fuellung schieben sonst
    # genau das wieder hinein, was der Kasten entfernen sollte.
    for k in kaesten(a.minus):
        region[k[1]:k[3], k[0]:k[2]] = False
    print("  Auswahl: %d Flaechen (oben %d, Saum %d, Fuellung %s) -> %.1f %% des Bildes"
          % (len(ids), a.oben, a.grow, a.fuellen, 100 * region.mean()))

    # ⛔ EIN VERSUCH, DER NICHT TRUG: eine Warnung, die gewaehlte Flaechen mit der
    # Bodenfarbe am unteren Bildrand vergleicht. In ROOM1140 Cut 3 meldete sie 13 von
    # 20 Flaechen — darunter fast alle RICHTIGEN (Fahnen, Podium, Mikrofone). In den
    # dunklen Raeumen liegen Teppich und dunkles Holz farblich zu nah beieinander.
    # Als Filter unbrauchbar, als Warnung nur Rauschen; wieder entfernt.
    # Was TRAEGT, ist die Zaehlung im laufenden Spiel (RE15_POCC_SCAN,
    # "figur_verschluckt"): Cuts mit KUENSTLER-Masken kommen dort auf 0 %, meine
    # fehlerhaften auf 29-43 %. Das ist der Abnahmeriegel je Raum.

    if a.vergleich:
        truth = artist_region(rdt, cam, a.cut)
        if truth is None:
            print("  Cut hat keine Kuenstler-Masken — Vergleich nicht moeglich")
            return 1
        hit = int((region & truth).sum())
        print("  PRAEZISION %.1f %%  (Anteil unserer Flaeche, der auch Kuenstler-Flaeche ist)"
              % (100.0 * hit / max(1, region.sum())))
        print("  AUSBEUTE   %.1f %%  (Anteil ihrer Flaeche, den wir treffen)"
              % (100.0 * hit / max(1, truth.sum())))
        print("  unsere Flaeche %d px, ihre %d px" % (int(region.sum()), int(truth.sum())))
        return 0

    r = build(rdt, cam, rid, a.cut, region, bg, a.out, room)
    if not r:
        print("  nichts erzeugt")
        return 1
    sec, n = r
    path = os.path.join(a.out, "%s.MSK" % room)
    secs = {}
    if os.path.exists(path):
        b = open(path, "rb").read()
        if b[:4] == b"R15M":
            _, nc = struct.unpack_from("<II", b, 4)
            offs = struct.unpack_from("<%dI" % nc, b, 12)
            ends = sorted([o for o in offs if o] + [len(b)])
            for c in range(nc):
                if offs[c]:
                    e = min(x for x in ends if x > offs[c])
                    secs[c] = b[offs[c]:e]
    secs[a.cut] = sec
    open(path, "wb").write(geom.pack_container(secs, rdt[1]))
    print("  %s: Cut %d mit %d Masken geschrieben (%d Cuts im Container)"
          % (os.path.basename(path), a.cut, n, len(secs)))
    return 0


if __name__ == "__main__":
    sys.exit(main())


# ---------------------------------------------------------------------------
# OBJEKTWEISE Maske — der zweite Anlauf nach den Nutzerbefunden vom 2026-09-03
# ---------------------------------------------------------------------------
def bau_objektweise(rdt, cam, cut, objekte, bg, out_dir, room, budget=None):
    """Maske aus EINZELNEN OBJEKTEN statt aus einer Blob-Region.

    ⛔ WARUM (zwei gemessene Fehlerklassen, Nutzer-Screenshots vom 2026-09-03):

    1. TIEFE AUS DEM FALSCHEN BODENPUNKT. Die Tiefe kam je Bildspalte aus dem
       untersten Punkt der ZUSAMMENHANGSKOMPONENTE. Wo zwei Gegenstaende in einer
       Spalte uebereinander liegen (Sitzbank hinten, Stativbein davor), erbt der
       hintere die nahe Tiefe des vorderen. Und wo ein Gegenstand auf einer duennen
       Stange steht, die in der Auswahl fehlt (Fahne, Stativ, Mikrofonstaender),
       nimmt die Regel die Unterkante des TUCHS als Bodenkontakt.
       Gemessen an ROOM1140 Cut 3, Spalte x=215: unsere Maske endet bei y=143 und
       bekommt Tiefe 126/127 (Bodenkontakt vz~9000). Der Fahnenteller steht aber bei
       y=178 (vz 7401, richtige Tiefe ~104). Leons Fuesse lagen bei y=165 (vz 7981);
       verdeckt haette ihn nur eine Tiefe < 124.7. Ergebnis im Spiel: Leon wurde VOR
       die Fahne gezeichnet (Screenshot 220043).
       -> Jedes Objekt bekommt seine Tiefe aus SEINEM eigenen Fuss. Das Feld "fuss"
          gibt die Bildzeile des Bodenkontakts an, wenn die Silhouette ihn nicht
          selbst zeigt; dann ist die Tiefe fuer das ganze Objekt die dieses Punktes.

    2. RECHTECKE UEBER ZWEI OBJEKTE. Ein Rechteck traegt genau EINE Tiefe. Lief die
       Zerlegung ueber die vereinigte Region, konnte ein Rechteck zwei Gegenstaende
       mit verschiedenem Abstand ueberdecken und bekam den Median.
       -> Die Zerlegung laeuft jetzt JE OBJEKT.

    objekte: [(name, region bool240x320, fuss oder None, ebene, bodenkante)]
    """
    dep_all = np.zeros((240, 320), np.int32)
    stuecke = []
    rest = budget or geom.MAX_MASKS_PER_CUT
    for eintrag in objekte:
        name, reg, fuss = eintrag[0], eintrag[1], eintrag[2]
        # None = Bodenebene aus den Kollisionsdaten waehlen (geom.boden_ebenen);
        # ein ausdruecklicher Wert (z.B. -700 fuer eine Tischplatte) hat Vorrang.
        ebene = eintrag[3] if len(eintrag) > 3 else None
        # ⛔ "bodenkante": die Bildspalten, in denen die untere Silhouettenkante
        # wirklich der Bodenkontakt ist — s. geom.depth_map_objekt.
        bodenkante = eintrag[4] if len(eintrag) > 4 else None
        if not reg.any():
            continue
        _ber = []
        d = geom.depth_map_objekt(rdt, cam, cut, reg, fuss, ebene, bodenkante, _ber)
        for _z in _ber:
            print("     %s: %s%s" % (name, _z,
                  "" if bodenkante is None else
                  " (Bodenkante x %d..%d, uebrige Spalten erben)" % bodenkante))
        if d is None:
            continue
        dep_all = np.where((d > 0) & ((dep_all == 0) | (d < dep_all)), d, dep_all)
        stuecke.append((name, reg, d))
    if not stuecke:
        return None
    # Budget nach Flaeche verteilen, mindestens 4 Rechtecke je Objekt
    flaechen = [float(r.sum()) for (_, r, _) in stuecke]
    gesamt = sum(flaechen) or 1.0
    region_all = np.zeros((240, 320), bool)
    for (_, r, _) in stuecke:
        region_all |= r
    tim = None
    # ⛔ GITTER statt gieriger Vollrechtecke (Nutzer-Befund 2026-09-04): ein Rechteck,
    # das GANZ in der Region liegen muss, laesst an jeder schraegen Kante einen
    # ungedeckten Saum — im Spiel als grobe Treppe sichtbar. Ein Rechteck darf aber
    # ueber die Region hinausragen, die Feinmaskierung macht der Atlas.
    while True:
        boxes, herkunft = [], []
        kap = 256 * 256
        for i, ((name, reg, d), fl) in enumerate(zip(stuecke, flaechen)):
            b = max(4, int(rest * fl / gesamt))
            for r_ in geom.rects_gitter(reg, b, int(kap * fl / gesamt)):
                boxes.append(r_); herkunft.append(i)
        if len(atlasmod.split_oversize(boxes)[0]) <= geom.MAX_MASKS_PER_CUT or rest <= 8:
            break
        rest -= 4
    tim, place, boxes2 = atlasmod.build(bg, region_all, boxes)
    if tim is None:
        return None
    # split_oversize kann Kaesten zerlegt haben -> Herkunft mitziehen
    _, herk2 = atlasmod.split_oversize(boxes)
    groups, masks = [], []
    for i, (x, y, w, h) in enumerate(boxes2):
        if i not in place:
            continue
        ax, ay = place[i]
        src = herkunft[herk2[i]] if i < len(herk2) else 0
        win = stuecke[src][2][y:y + h, x:x + w]
        win = win[win > 0]
        if len(win) == 0:
            continue
        groups.append((1, x - ax, y - ay))
        masks.append((ax, ay, x, y, w, h, int(np.median(win))))
    if not masks:
        return None
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)
        open(os.path.join(out_dir, "%s_PRI%02d.TIM" % (room, cut)), "wb").write(tim)
    return geom.pack_section(groups, masks), len(masks)
