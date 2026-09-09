"""Ganzen Raum aus der Auswahldatei erzeugen — und das ERGEBNIS zum Ansehen rendern.

⛔ WARUM ES DAS GIBT (Nutzer-Befund 2026-09-03, error01/02/03.png): die Auswahl wurde
bis dahin per Kommandozeile Cut fuer Cut angewendet und nur die AUSWAHL angesehen
(pruefblatt.py). Drei Fehler blieben dadurch unentdeckt, weil sie erst in der
NACHBEARBEITUNG entstehen: eine Wandflaeche, die mit dem Sofa in einer Superpixel-
Flaeche lag; eine Spaltenfuellung, die den Gang zwischen den Stuehlen zumauerte; ein
Objekt, das schlicht fehlte. Alle drei sind im ERGEBNISBILD (maskenbild.py) sofort zu
sehen. Deshalb erzeugt dieses Werkzeug beides in einem Lauf: Maske schreiben und
Ergebnisbild rendern.

Auswahlformat (analysis/esp_masken_2026-09-03/auswahl.json):
    "ROOM1140": {
      "2": {"segments": 90, "ids": [30, 38, ...],
            "minus": "200,50,320,112",      Kaesten abziehen (Wand/Boden wegschneiden)
            "plus":  "",                    Kaesten hinzufuegen
            "oben": 4, "grow": 1, "fuellen": "komponente"}
    }
Eine blosse Liste statt des Objekts wird als {"ids": [...]} mit den Standardwerten
gelesen (altes Format).

Aufruf:
    python re15_port/tools/maske/raum.py ROOM1140            (alle Cuts der Datei)
    python re15_port/tools/maske/raum.py ROOM1140 2          (nur Cut 2)
"""
import argparse
import json
import os
import struct
import subprocess
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import anwenden
import geom
import maskenbild
from geom import load_bg, load_rdt

AUSWAHL = "analysis/esp_masken_2026-09-03/auswahl.json"
CD = "re15_port/shared_assets/PSX"
# ⛔ SPALTENFUELLUNG STANDARDMAESSIG AUS (Nutzer-Befund 2026-09-03, Screenshot 233104:
# "immer noch sehr viel ueberdeckende Transparenz, bei beiden Fahnen und beim Pult").
# Die Fuellung schliesst je Bildspalte alles zwischen oberstem und unterstem Punkt EINER
# Komponente. Bei einem DUENNEN Gegenstand ist das verheerend: die US-Fahne ist rund 20
# Bildpunkte breit, ihr Fussteller sitzt 110 Zeilen tiefer — gefuellt wurde daraus ein
# Block von rund 55 x 117 Punkten, also das Vier- bis Fuenffache des Objekts, samt
# Teppich, Sockelwand und Schrank. Alles, was dahinter steht, bekommt diese Pixel
# uebergemalt; im Spiel sieht das aus wie ein halbdurchsichtiger Fleck.
# Gemessen gegen die Kuenstler-Silhouetten (kalib.py, seg 220, oben 4, Saum 1):
#     Fuellung aus         Praezision 72.1 %  Ausbeute 88.2 %
#     Fuellung Komponente  Praezision 70.6 %  Ausbeute 89.8 %
# Sie war also ohnehin die schlechtere Wahl bei der Praezision. Wer sie fuer einen
# massiven Gegenstand mit Kerben braucht, setzt sie je Objekt.
STD = {"segments": 220, "oben": 4, "grow": 1, "fuellen": "aus",
       "minus": "", "plus": "", "ids": []}


def eintrag(v):
    e = dict(STD)
    if isinstance(v, list):
        e["ids"] = v
        e["segments"] = 90          # altes Format entstand mit 90
        e["oben"] = 6
        e["fuellen"] = "global"
    else:
        e.update(v)
    return e


def polygon_region(spec, loch=None):
    """Polygonzug(e) -> Region. "x,y x,y ..." je Teil, Teile mit ";" getrennt."""
    from PIL import Image as _I, ImageDraw as _D
    def zeichne(text, wert):
        bild = _I.new("1", (320, 240), 0)
        d = _D.Draw(bild)
        for teil in text.split(";"):
            teil = teil.strip()
            if not teil:
                continue
            pts = []
            for pp in teil.replace(",", " ").split():
                pts.append(float(pp))
            if len(pts) < 6:
                continue
            d.polygon([(pts[i], pts[i + 1]) for i in range(0, len(pts) - 1, 2)], fill=1)
        return np.array(bild, bool)
    r = zeichne(spec, 1)
    if loch:
        r &= ~zeichne(loch, 1)
    return r


def zauberstab_region(bg, spec):
    """Flutfuellung von Saatpunkten aus — die Grenze ist die echte Farbkante im Bild."""
    from scipy import ndimage
    a = bg.astype(np.int32)
    saat = []
    for teil in str(spec.get("saat", "")).split(";"):
        teil = teil.strip()
        if not teil:
            continue
        v = [int(x) for x in teil.replace(",", " ").split()]
        saat.append((v[0], v[1]))
    if not saat:
        return np.zeros((240, 320), bool)
    tol = float(spec.get("toleranz", 30))
    erlaubt = np.ones((240, 320), bool)
    if spec.get("box"):
        erlaubt[:] = False
        for k in anwenden.kaesten(spec["box"]):
            erlaubt[k[1]:k[3], k[0]:k[2]] = True
    if spec.get("minus"):
        for k in anwenden.kaesten(spec["minus"]):
            erlaubt[k[1]:k[3], k[0]:k[2]] = False
    # Referenzfarbe = Mittel der Saatpunkte (mit 1 Punkt Umfeld)
    ref = []
    for (x, y) in saat:
        ref.append(a[max(0, y - 1):y + 2, max(0, x - 1):x + 2].reshape(-1, 3))
    ref = np.concatenate(ref, 0).mean(0)
    # ⛔ FARBMASS: der reine Kanalabstand trennt hier NICHTS. Gemessen in ROOM1140
    # Cut 3: Pult R=14.8 G=7.7 B=3.9, Teppich R=15.8 G=24.6 B=21.7 — die Helligkeiten
    # liegen uebereinander, der Unterschied steckt im FARBTON: Pult R-B=+11 / G-R=-7,
    # Teppich R-B=-6 / G-R=+9. In dieser Ebene ist der Abstand 23, also gut trennbar,
    # waehrend der Kanalabstand die Flut ueber den ganzen Kasten laufen liess.
    # "metrik": "ton" (Standard) misst in (R-B, G-R) plus 1/3 Helligkeit;
    # "kanal" ist das alte Verhalten fuer Faelle mit klarem Helligkeitsunterschied.
    if str(spec.get("metrik", "ton")) == "ton":
        def merkmal(v):
            r_, g_, b_ = v[..., 0], v[..., 1], v[..., 2]
            return np.stack([r_ - b_, g_ - r_, (r_ + g_ + b_) / 3.0 / 3.0], -1)
        d = np.abs(merkmal(a.astype(np.float64)) - merkmal(ref.reshape(1, 1, 3))).max(2)
    else:
        d = np.abs(a - ref).max(2)
    aehnlich = (d <= tol) & erlaubt
    lab, n = ndimage.label(aehnlich, np.ones((3, 3)))
    r = np.zeros((240, 320), bool)
    for (x, y) in saat:
        if 0 <= y < 240 and 0 <= x < 320 and lab[y, x]:
            r |= (lab == lab[y, x])
    k = int(spec.get("schliessen", 2))
    if k > 0:
        r = ndimage.binary_closing(r, np.ones((2 * k + 1, 2 * k + 1)))
    if spec.get("loecher", 1):
        r = ndimage.binary_fill_holes(r)
    m = int(spec.get("mindest", 40))
    if m > 0:
        lab2, n2 = ndimage.label(r, np.ones((3, 3)))
        for i in range(1, n2 + 1):
            sel = lab2 == i
            if sel.sum() < m:
                r[sel] = False
    return r


def kontrast_region(bg, spec):
    """Silhouette eines DUENNEN Gegenstands aus dem Bild selbst schneiden.

    ⛔ WARUM (Nutzer-Befund 2026-09-03, Screenshot 233104: "immer noch sehr viel
    ueberdeckende Transparenz, bei beiden Fahnen und beim Pult"): Superpixel sind
    FLAECHIG. Eine Flaeche von rund 700 Bildpunkten kann eine 20 Punkte breite Fahne
    nicht umschliessen, ohne Fenster, Sockelwand und Teppich mitzunehmen — und genau
    diese Fremdpixel werden im Spiel ueber alles gemalt, was dahinter steht. Feiner
    segmentieren hilft nicht: bei 900 Flaechen zerfaellt SLIC auf diesem kontrastarmen
    Bild in ein 4x4-Raster, das den Kanten gar nicht mehr folgt.

    Stattdessen wird die Silhouette aus dem KONTRAST zum oertlichen Hintergrund
    geschnitten: je Bildzeile ist der Hintergrund der Median der Punkte unmittelbar
    LINKS und RECHTS des Kastens; behalten wird, was sich davon deutlich abhebt. Fuer
    einen Gegenstand vor einer ruhigen Flaeche (Fahne vor Fenster und Teppich,
    Mikrofon vor dunklem Holz) ist das die Objektkante selbst und kein Raster.

    spec: {"box": "x0,y0,x1,y1", "rand": 6, "schwelle": 26, "schliessen": 1,
           "mindest": 12}
    """
    from scipy import ndimage
    x0, y0, x1, y1 = [int(v) for v in spec["box"].split(",")]
    rand = int(spec.get("rand", 6))
    schwelle = float(spec.get("schwelle", 26))
    a = bg.astype(np.int32)
    r = np.zeros((240, 320), bool)
    for y in range(max(0, y0), min(240, y1)):
        # Seite waehlbar: liegt auf EINER Seite des Kastens etwas anderes als der
        # ruhige Hintergrund (bei der blauen Fahne rechts der helle Wandpfeiler), zieht
        # dessen Farbe den Median weg und der Schnitt holt sich das Fenster mit.
        seite = spec.get("seite", "beide")
        links = a[y, max(0, x0 - rand):x0] if seite in ("beide", "links") else a[y, 0:0]
        rechts = a[y, x1:min(320, x1 + rand)] if seite in ("beide", "rechts") else a[y, 0:0]
        umfeld = np.concatenate([links, rechts], 0) if len(links) or len(rechts) else None
        if umfeld is None or len(umfeld) == 0:
            continue
        hg = np.median(umfeld, 0)
        d = np.abs(a[y, x0:x1] - hg).max(1)
        r[y, x0:x1] = d > schwelle
    k = int(spec.get("schliessen", 1))
    if k > 0:
        r = ndimage.binary_closing(r, np.ones((2 * k + 1, 2 * k + 1)))
    m = int(spec.get("mindest", 12))
    if m > 0:
        lab, n = ndimage.label(r, np.ones((3, 3)))
        for i in range(1, n + 1):
            sel = lab == i
            if sel.sum() < m:
                r[sel] = False
    return r


def objekt_regionen(room, cut, e, ppm, blattdir):
    """-> [(name, region, fuss, ebene, bodenkante, aufrecht)] — die Gegenstaende.

    Format in der Auswahldatei:
      "objekte": [{"name": "Konferenztisch", "segments": 90, "ids": [...]},
                  {"name": "US-Fahne", "segments": 220, "ids": [...], "fuss": 178,
                   "minus": "..."}]
    "fuss" ist die Bildzeile des Bodenkontakts und wird gebraucht, wenn die Silhouette
    ihn nicht zeigt (duenne Stange/Beine) — siehe geom.depth_map_objekt.
    """
    from scipy import ndimage
    rid = int(room[4:], 16)
    # RDT + Kamerasatz fuer die Bodenebene (s. unten bei "ebene").
    _rdt_pfad = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..",
                             "shared_assets", "PSX", "STAGE%d" % (rid >> 12),
                             "ROOM%04X.RDT" % rid)
    rdt = open(os.path.abspath(_rdt_pfad), "rb").read()
    cam_off = struct.unpack_from("<I", rdt, 0x24)[0]
    aus = []
    # ⛔ ETIKETTEN STATT KUNST-UNION (Nutzer-Marken F697/F1327, 2026-09-09):
    # die blosse VEREINIGUNG aller PNG-Freistellungen liess die Silhouette des NAHEN
    # Stuhls Kunst-Pixel des FERNEN mitnehmen und mit der nahen Tiefe ueber die Figur
    # blitten (Spieler stand bei z=11816 ZWISCHEN Zelle 18 (8200..9700) und Zelle 17
    # (12200..13700); Kunst des fernen Stuhls deckte ihn mit z 5120..6464 statt
    # ~8900). Jede Freistellung gehoert genau EINEM Stuhl - gemessen liegt jede
    # Lasso-Komponente zu >=0.99 in genau EINER Zellen-Silhouette (Cut 4: 04_01->
    # Z17 1.00, 04_02->Z18 1.00, 04_03->Z19 1.00; Cut 5: 05_01->Z20 1.00,
    # 05_02->Z21 1.00, 05_03->Z22 0.99). Deshalb:
    #   _kunst_label: je Kunst-Pixel der Index seiner Moebel-Zelle (Komponente ->
    #                 Zelle mit groesster Silhouetten-Ueberdeckung),
    #   _dunkel_label: Tiefschwarz (<45) -> Zelle, deren Silhouette es enthaelt;
    #                 bei MEHREREN die TIEFSTE (Nutzer-Regel: steht er davor, darf
    #                 nichts decken - zu tief zeigt hoechstens die Figur ueber dem
    #                 Stuhl, zu nah schneidet sie weg); ohne Silhouette KEINE Zelle
    #                 (Bodenschatten blitten sonst Boden ueber die Figur).
    import numpy as _np
    from scipy import ndimage as _nd
    _sperr = geom.sca_sperrzellen(rdt, 0) or []
    _v2 = geom.cut_view(rdt, cam_off, cut)
    _sil = {}
    for _ki, (_zx, _zz, _zw, _zd, _typ) in enumerate(_sperr):
        if _typ != 3:
            continue
        _vzq, _trq = geom.quader_tiefe(_v2[0], _v2[1], _v2[2], _zx, _zx + _zw,
                                       _zz, _zz + _zd, -1950)
        _sil[_ki] = (_vzq, _trq)
    _kunst_label = _np.full((240, 320), -1, int)
    _lasso_teile = {}      # id(objekt-dict) -> [(zellindex, komponenten-region)]
    for _o2 in (e.get("objekte") or []):
        if "png" not in _o2 or not all(k in _o2 for k in ("x", "y")):
            continue
        import maske_aus_png as _map2
        _r2 = _map2.setze(_o2["png"], _o2["x"], _o2["y"], _o2.get("massstab", 1))
        if _r2 is None:
            continue
        _lab2, _n2 = _nd.label(_r2)
        _teile = []
        for _k2 in range(1, _n2 + 1):
            _comp = _lab2 == _k2
            _npx = int(_comp.sum())
            if _npx < 20:
                continue
            _best_f, _best_ki = 0.0, None
            for _ki, (_vzq, _trq) in _sil.items():
                _f = float((_trq & _comp).sum()) / _npx
                if _f > _best_f:
                    _best_f, _best_ki = _f, _ki
            if _best_ki is None:
                print('   ⚠ "%s": Komponente %d (%d px) liegt in KEINER '
                      'Moebel-Silhouette - bleibt ohne Zelle'
                      % (_o2.get("png", "?"), _k2, _npx))
                continue
            if _best_f < 0.90:
                print('   ⚠ "%s": Komponente %d nur %.2f in Zelle %d - '
                      'Zuordnung unsicher, bitte pruefen'
                      % (_o2.get("png", "?"), _k2, _best_f, _best_ki))
            _kunst_label[_comp] = _best_ki
            _teile.append((_best_ki, _comp))
        _lasso_teile[id(_o2)] = _teile
    _bgL = load_bg(ppm, rid, cut)
    _dunkel_label = _np.full((240, 320), -1, int)
    if _bgL is not None and _sil:
        _dk = _bgL.astype(int).sum(2) < 45
        # 1) KUNST-SAUM: ein Dunkel-Pixel im Abstand <= 7 zur Kunst einer Zelle
        #    ist deren Lehnen-Luecke (schwarz auf schwarz im Lasso-Loch).
        #    GEMESSEN (2026-09-09, ROOM10F0): Luecken-Dunkel der F697-Klasse
        #    liegt bei Distanz 1..7 zur eigenen Kunst (60/52/31/19/12/3/2),
        #    Schreibtisch-Dunkel der F1195-Klasse beginnt bei 7 (7..18+).
        #    Beim Konflikt (genau 7) gewinnt die Luecken-Klasse: die Kosten
        #    sind unsichtbare Speckles ueber dunklem Pult statt sichtbarer
        #    Ueber-Deckung auf der Figur.
        #    Bedingung ZUSAETZLICH: das Pixel liegt in der Silhouette DERSELBEN
        #    Zelle - sonst wird Boden-SCHATTEN neben der Kunst etikettiert und
        #    deckt mit Stuhltiefe (gemessen F1451: 0 -> 77 ungerechtfertigt).
        _naechste = _np.full((240, 320), _np.inf)
        for _ki in sorted(set(int(v) for v in _kunst_label[_kunst_label >= 0])):
            if _ki not in _sil:
                continue
            _dist = _nd.distance_transform_edt(~(_kunst_label == _ki))
            _m = _dk & _sil[_ki][1] & (_dist <= 7) & (_dist < _naechste)
            _naechste[_m] = _dist[_m]
            _dunkel_label[_m] = _ki
        # 2) Sonst: die NAECHSTE enthaltende Zelle - der vorgerenderte
        #    Hintergrund zeigt an einem Pixel die vorderste Flaeche; dunkles
        #    Pult-Zeug in der Silhouette eines nahen Stuhls deckte schon in
        #    der dritten (abgenommenen) Runde mit dessen Tiefe. Der Tiefste-
        #    Bias hier war falsch (Schwarz-ungedeckt 960 -> 2888, F1195/F954
        #    je 700 - Pult-Dunkel wanderte in die fernere Silhouette).
        _rest = _dk & (_dunkel_label < 0)
        _naechstes = _np.full((240, 320), _np.inf)
        for _ki, (_vzq, _trq) in _sil.items():
            _m = _rest & _trq & (_vzq < _naechstes)
            _naechstes[_m] = _vzq[_m]
            _dunkel_label[_m] = _ki
    for o in e.get("objekte") or []:
        if "png" in o:
            # ⛔ DER BESTE WEG (Nutzer, 2026-09-04): ein von Hand freigestelltes PNG.
            # Das Maskenformat IST eine Freistellung (Palettenindex 0 wird nicht
            # gezeichnet), ein Alphakanal ist also 1:1 das, was hineingehoert — und ein
            # Mensch mit dem Lasso trifft in drei Minuten, woran Superpixel, Kontrast-
            # schnitt und Kaesten alle gescheitert sind. Die LAGE wird gemessen, nicht
            # angegeben: die freigestellten Pixel sind Hintergrundpixel, es gibt also
            # genau eine Stelle, an der sie passen (s. maske_aus_png.platziere).
            import maske_aus_png
            bgb = load_bg(ppm, rid, cut)
            if all(k in o for k in ("x", "y", "massstab")):
                r = maske_aus_png.setze(o["png"], o["x"], o["y"], o["massstab"])
            else:
                r, info = maske_aus_png.platziere(o["png"], bgb)
                print("     %s: gefunden bei x=%d y=%d Massstab %d, "
                      "Uebereinstimmung %.1f %% — diese drei Werte in die Auswahldatei "
                      "eintragen, dann entfaellt die Suche"
                      % (o.get("name", "?"), info["x"], info["y"], info["massstab"],
                         100 * info["uebereinstimmung"]))
            if r is None:
                continue
        elif "polygon" in o:
            # ⛔ VON HAND GEZOGEN — dasselbe wie das Lasso des Nutzers, nur in
            # Koordinaten statt mit der Maus. Fuer alles, was eine klare Kante hat
            # (Tische, Stuehle, Schraenke, Tafeln) ist das der genaueste und
            # schnellste Weg: Superpixel sind zu flaechig, der Kontrastschnitt
            # verliert Dunkles vor Dunklem, Kaesten treffen keine Schraegen.
            # "polygon": "x,y x,y x,y ..."  (mehrere Teile mit ";" trennen)
            # "loch":    dieselbe Schreibweise, wird abgezogen (offene Gestelle)
            r = polygon_region(o["polygon"], o.get("loch"))
        elif "zauberstab" in o:
            # ⛔ ZAUBERSTAB (Nutzer 2026-09-04: "das ist nicht genau genug"). Von Hand
            # getippte Polygone treffen die Kante nur auf ±4 Bildpunkte genau — bei
            # einem 6 Punkte breiten Tischrand ist das zu grob. Der Zauberstab arbeitet
            # wie in GIMP: von einem Saatpunkt INNERHALB des Gegenstands wird geflutet,
            # solange die Farbe zur Saat passt; die Grenze ist damit die ECHTE
            # Objektkante im Bild, nicht meine Schaetzung.
            # {"saat": "x,y;x,y", "toleranz": 30, "box": "x0,y0,x1,y1", "fuellen": 1}
            r = zauberstab_region(load_bg(ppm, rid, cut), o["zauberstab"])
        elif "kontrast" in o:
            r = kontrast_region(load_bg(ppm, rid, cut), o["kontrast"])
        elif "quader" in o:
            # ⛔ SILHOUETTE AUS DER RAUMGEOMETRIE (Nutzer-Marken 2026-09-08). Zwei
            # Gegenstaende hatten ueberhaupt keine Maske, weil sie farblich nicht vom
            # Hintergrund zu trennen sind: die Holztischplatte in ROOM10D0 (43,30,11 vor
            # dunklem Raum) und die Trennwand rechts in ROOM10E0 (so grau wie der Boden).
            # Ihre KOLLISIONSZELLE steht aber im RDT. Als Quader mit Deckflaeche
            # gezeichnet liefert sie die Silhouette selbst - siehe geom.quader_tiefe.
            # "quader": [x, z, breite, tiefe, hoehe]
            _q = [int(v) for v in o["quader"]]
            _v = geom.cut_view(rdt, cam_off, cut)
            _vz, r = geom.quader_tiefe(_v[0], _v[1], _v[2], _q[0], _q[0] + _q[2],
                                       _q[1], _q[1] + _q[3], _q[4])
            if o.get("nur_kunst"):
                # ⛔ NUR EIGENE Kunst oder EIGENES Tiefschwarz (Etiketten, s.o.).
                # Dritte Runde (F1039..F1451): Lehnen sind schwarz auf schwarz und
                # fehlen im Lasso -> Tiefschwarz (<45) zaehlt als Stuhl. Vierte
                # Runde (F697/F1327): FREMDE Kunst in der eigenen Silhouette deckte
                # mit falscher Tiefe -> nur Pixel, deren Etikett DIESER Zelle
                # gehoert.
                _eig = None
                for _ki2, (_zx2, _zz2, _zw2, _zd2, _typ2) in enumerate(_sperr):
                    if _typ2 == 3 and _zx2 == _q[0] and _zz2 == _q[1]:
                        _eig = _ki2
                        break
                if _eig is None:
                    print('   ⚠ "%s": Quader (%d,%d) entspricht keiner '
                          'Moebel-Zelle - nur Tiefschwarz deckt'
                          % (o.get("name", "?"), _q[0], _q[1]))
                    r = r & (_dunkel_label >= 0)
                else:
                    r = r & ((_kunst_label == _eig) | (_dunkel_label == _eig))
        elif "kaesten" in o:
            # Massiver, nahezu rechteckiger Gegenstand (Pult, Schrank): direkt als
            # Kaesten angeben. Genauer als eine Superpixel-Auswahl, die zwangslaeufig
            # Teppich an den Raendern mitnimmt, und ohne den Kontrastschnitt, der nur
            # bei duennen Gegenstaenden vor ruhigem Hintergrund traegt.
            r = np.zeros((240, 320), bool)
            for k in anwenden.kaesten(o["kaesten"]):
                r[k[1]:k[3], k[0]:k[2]] = True
        else:
            seg = anwenden.lade_seg(blattdir, room, cut, o.get("segments", e["segments"]), ppm, rid)
            r = np.isin(seg, o["ids"])
        for k in anwenden.kaesten(o.get("plus", "")):
            r[k[1]:k[3], k[0]:k[2]] = True
        for k in anwenden.kaesten(o.get("minus", "")):
            r[k[1]:k[3], k[0]:k[2]] = False
        oben = o.get("oben", e["oben"]); grow = o.get("grow", e["grow"])
        if oben > 0:
            g = r.copy()
            for k in range(1, oben + 1):
                g[:-k] |= r[k:]
            r = g
        if grow > 0:
            r = ndimage.binary_dilation(r, iterations=grow)
        fuell = o.get("fuellen", e["fuellen"])
        if fuell != "aus":
            r = anwenden.spalten_fuellen(r, fuell)
        for k in anwenden.kaesten(o.get("minus", "")):
            r[k[1]:k[3], k[0]:k[2]] = False
        for k in anwenden.kaesten(e["minus"]):
            r[k[1]:k[3], k[0]:k[2]] = False
        # ⛔ SCHRAEGE UNTERKANTE. Ein Tisch endet im Bild an einer DIAGONALEN — mit
        # achsenparallelen Kaesten ist die nicht zu treffen, und genau darunter lag in
        # ROOM1140 Cut 4 Maske auf blankem Teppich (gemessen: 649 Bildpunkte, die
        # Tischstruktur endet in Spalte 120 bei y=140, die Maske lief bis y=176..213).
        # "unterkante": "x0,y0,x1,y1" schneidet alles unterhalb der Geraden durch die
        # beiden Punkte weg; die Gerade wird nach links und rechts verlaengert.
        uk = o.get("unterkante")
        if uk:
            x0, y0, x1, y1 = [float(v) for v in uk.split(",")]
            if x1 != x0:
                m = (y1 - y0) / (x1 - x0)
                xs = np.arange(320)
                ys = y0 + m * (xs - x0)
                for x in range(320):
                    yy = int(round(ys[x]))
                    if yy < 239:
                        r[max(0, yy + 1):, x] = False
        if r.any():
            # ⛔ OHNE "fuss" UND OHNE "ebene" GILT DIE SPALTENREGEL — und die ist fuer
            # jedes Objekt falsch, das oben breiter ist als unten. NUTZER-BEFUND
            # 2026-09-04 (fehler/error2.png): ROOM1140 Cut 2, "Kamerastativ rechts" war
            # als einziges Objekt ohne "fuss" eingetragen. geom.depth_map_objekt nimmt
            # dann den Bodenkontakt JE BILDSPALTE aus dem untersten Silhouettenpunkt —
            # richtig fuer die Beinspalten (Tiefe 122), falsch fuer die breiten
            # Gehaeusespalten (176/193/280). Ergebnis: die Beine verdeckten den Spieler,
            # der Kamerakoerper nicht. Keine Messschiene konnte das sehen
            # (integration_pri_masken prueft die Tiefe nicht, verdeckungskarte.py zaehlt
            # nur ZUVIEL Verdeckung), deshalb sagt es das Werkzeug jetzt beim Bauen.
            # ⛔ "spalten": true macht die Spaltenregel zur ABSICHT statt zum Zufall.
            # Sie ist RICHTIG fuer alles, was diagonal von der Kamera weglaeuft (dort
            # hat jede Bildspalte ihre eigene Entfernung) und FALSCH fuer alles, was
            # aufrecht steht und oben breiter ist als unten. Beide Faelle gibt es im
            # Baum: ROOM1130 Cut 3 (diagonale Wandkante, Spannweite 182 px) und
            # ROOM1140 Cut 2 (Kamerastativ, der gemeldete Fehler). Wer keins von
            # beidem angibt, hat die Frage nicht beantwortet - deshalb die Warnung.
            if (o.get("fuss") is None and not o.get("ebene")
                    and not o.get("spalten") and not o.get("aufrecht")
                    and not o.get("flach")
                    and not o.get("quader")
                    and o.get("tiefe") is None):   # kollision/feste Tiefe beantworten sie auch
                print('   ⚠ "%s": weder "fuss" noch "ebene" — die Tiefe kommt aus der '
                      'Spaltenregel. Fuer ein senkrecht stehendes Objekt ist das falsch, '
                      'sobald es oben breiter ist als unten (ROOM1140-Kamera, 2026-09-04).'
                      % o.get("name", "?"))
            # ⛔ KEIN VORGABEWERT 0 MEHR (Nutzer-Befund 2026-09-05,
            # fehler/error03+04.png: "Die Gelaender scheinen mich gleich mal garnicht
            # zu ueberdecken"). ROOM1060 ist ein Treppenhaus; das Podest liegt bei
            # y = -14400, und mit y=0 landeten die Masken zwei- bis viermal zu weit
            # weg - im Spiel gemessen (RE15_PRI_LOG): Figur-Tiefe 4800..6862, Masken
            # 16960..26496, verdeckend 0 von 81.
            # Fehlt "ebene", kommt sie aus dem BLICKZIEL des Kamerasatzes, gerundet
            # auf das naechste BEGEHBARE Band (geom.ebene_aus_kamera). Ist das nicht
            # eindeutig (guete >= GUETE_MAX), bricht der Bau ab und verlangt den
            # gemessenen Wert - lieber keine Maske als eine falsche.
            _eb = o.get("ebene")
            if _eb is None and not o.get("fuss") and not o.get("aufrecht")                     and not o.get("flach") and not o.get("quader")                     and not isinstance(o.get("tiefe"), int):
                _tref = geom.ebene_aus_kamera(rdt, cam_off, cut, rid)
                if _tref is None:
                    raise SystemExit(
                        '   ⛔ "%s": Bodenebene nicht bestimmbar (kein Kamerasatz). '
                        'Bitte "ebene" messen und in auswahl.json eintragen.'
                        % o.get("name", "?"))
                _eb, _guete = _tref
                if _guete >= geom.GUETE_MAX:
                    raise SystemExit(
                        '   ⛔ "%s": Bodenebene NICHT EINDEUTIG (Blickziel liegt %.0f %% '
                        'zwischen zwei begehbaren Baendern; naechstes waere y=%d). '
                        'Bitte im Spiel messen (RE15_PRI_LOG -> "Spieler (x,y,z)") und '
                        'als "ebene" in auswahl.json eintragen.'
                        % (o.get("name", "?"), 100 * _guete, _eb))
                print('     Bodenebene y=%d aus dem Blickziel des Cuts (Guete %.2f, '
                      'begehbare Baender %s)'
                      % (_eb, _guete,
                         sorted(-b * geom.BAND_HOEHE for b in geom.begehbare_baender(rid))))
            _bk = o.get("bodenkante")
            _au = o.get("aufrecht")
            # ⛔ GANZES LASSO ZU SEINER ZELLE (Nutzer-Marken F697/F1327): die
            # fruehere Naechster-Treffer-Zerlegung schnitt ein Lasso an der Kante
            # der NAEHEREN Fantasie-Box (Zellen sind 1500x1500, die Stuehle ~700)
            # und gab Fern-Stuhl-Kunst die nahe Tiefe. Gemessen gehoert jede
            # Lasso-Komponente zu >=0.99 genau EINER Zelle (Tabelle oben) - also
            # bekommt sie deren Quader als Ganzes; das eigene Tiefschwarz kommt
            # dazu (Lehnen-Luecken, dritte Runde).
            if o.get("tiefe") == "szene":
                for _ki, _comp in _lasso_teile.get(id(o), []):
                    _zx, _zz, _zw, _zd, _typ = _sperr[_ki]
                    _teil = _comp | (_dunkel_label == _ki)
                    aus.append(("%s [Stuhl %d,%d]" % (o.get("name", "?")[:20],
                                                      _zx, _zz),
                                _teil, None, None, None, None, None, None, None,
                                [_zx, _zz, _zw, _zd, -1950], None))
                continue
            aus.append((o.get("name", "?"), r, o.get("fuss"),
                        None if _eb is None else int(_eb),
                        None if _bk is None else (int(_bk[0]), int(_bk[1])),
                        _au, o.get("flach"), o.get("tiefe"), o.get("zelle"),
                        o.get("quader"), o.get("tiefenfaktor")))
    return aus


def region_of(room, cut, e, ppm, blattdir):
    """Auswahl -> fertige Region.

    MEHRERE FEINHEITEN je Cut sind erlaubt ("gruppen"): eine Auswahl gilt immer nur
    zusammen mit IHRER Segmentierung, und eine grobe Auswahl (Tisch) laesst sich nicht
    in eine feine umrechnen. Statt alles neu zu waehlen, wenn ein Detail feiner
    gefasst werden muss, werden die Regionen vereinigt.
    """
    from scipy import ndimage
    rid = int(room[4:], 16)
    r = np.zeros((240, 320), bool)
    for g in e.get("gruppen") or [{"segments": e["segments"], "ids": e["ids"]}]:
        seg = anwenden.lade_seg(blattdir, room, cut, g["segments"], ppm, rid)
        r |= np.isin(seg, g["ids"])
    for k in anwenden.kaesten(e["plus"]):
        r[k[1]:k[3], k[0]:k[2]] = True
    for k in anwenden.kaesten(e["minus"]):
        r[k[1]:k[3], k[0]:k[2]] = False
    if e["oben"] > 0:
        o = r.copy()
        for k in range(1, e["oben"] + 1):
            o[:-k] |= r[k:]
        r = o
    if e["grow"] > 0:
        r = ndimage.binary_dilation(r, iterations=e["grow"])
    if e["fuellen"] != "aus":
        r = anwenden.spalten_fuellen(r, e["fuellen"])
    for k in anwenden.kaesten(e["minus"]):
        r[k[1]:k[3], k[0]:k[2]] = False
    return r


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room")
    ap.add_argument("cut", type=int, nargs="?")
    ap.add_argument("--ppm", default="build/bg_ppm")
    ap.add_argument("--blatt", default="build/blaetter")
    ap.add_argument("--out", default=os.path.join(CD, "MASKS"))
    ap.add_argument("--bild", default="build/maskenbild")
    a = ap.parse_args()

    room = a.room.upper()
    rid = int(room[4:], 16)
    aus = json.load(open(AUSWAHL, encoding="utf-8"))
    if room not in aus:
        raise SystemExit("%s steht nicht in %s" % (room, AUSWAHL))
    rdt, _ = load_rdt(CD, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]

    cuts = sorted(int(c) for c in aus[room] if not c.startswith("_"))
    if a.cut is not None:
        cuts = [a.cut]
    secs = {}
    for cut in cuts:
        e = eintrag(aus[room][str(cut)])
        bg = load_bg(a.ppm, rid, cut)
        if bg is None:
            print("  Cut %d: Hintergrund fehlt" % cut)
            continue
        objekte = objekt_regionen(room, cut, e, a.ppm, a.blatt)
        if objekte:
            res = anwenden.bau_objektweise(rdt, cam, cut, objekte, bg, a.out, room)
            flaeche = np.zeros((240, 320), bool)
            for e_ in objekte:
                r_ = e_[1]
                flaeche |= r_
            r = flaeche
        else:
            r = region_of(room, cut, e, a.ppm, a.blatt)
            res = anwenden.build(rdt, cam, rid, cut, r, bg, a.out, room)
        if not res:
            print("  Cut %d: nichts erzeugt" % cut)
            continue
        secs[cut], n = res
        # ⛔ TREUEPRUEFUNG (Nutzer-Befund 2026-09-04, fehler/error.png). Die gierige
        # Rechteckzerlegung liess an schraegen Kanten einen ungedeckten Saum stehen —
        # im Spiel eine grobe Treppe statt der glatten Kante der Freistellung. Seitdem
        # wird nach dem Bauen nachgezaehlt, wieviel der GEWOLLTEN Flaeche die fertige
        # Maske wirklich deckt. Alles ausser 0 fehlenden Punkten ist ein Fehler.
        try:
            import maskenbild as _MB
            _blob = geom.pack_container({cut: secs[cut]}, rdt[1])
            _ms = _MB.masken(_blob, cut) or []
            _t = _MB.lies_tim(os.path.join(a.out, "%s_PRI%02d.TIM" % (room, cut)))
            _idx = _t[0] if _t else None
            _deck = np.zeros((240, 320), bool)
            for (_sx, _sy, _X, _Y, _w, _h, _dep) in _ms:
                _x0, _x1 = max(0, _X), min(320, _X + _w)
                _y0, _y1 = max(0, _Y), min(240, _Y + _h)
                if _x1 <= _x0 or _y1 <= _y0:
                    continue
                _sub = _idx[_sy + (_y0 - _Y):_sy + (_y1 - _Y), _sx + (_x0 - _X):_sx + (_x1 - _X)]
                if _sub.shape == (_y1 - _y0, _x1 - _x0):
                    _deck[_y0:_y1, _x0:_x1] |= (_sub != 0)
            _fehlt = int((r & ~_deck).sum())
            _zuviel = int((_deck & ~r).sum())
            if _fehlt or _zuviel:
                print("     ⛔ TREUE: %d Punkte der gewollten Flaeche FEHLEN, %d zuviel"
                      % (_fehlt, _zuviel))
            else:
                print("     Treue: die Maske deckt die gewollte Flaeche punktgenau (%d px)"
                      % int(r.sum()))
        except Exception as _e:
            print("     (Treuepruefung nicht moeglich: %s)" % _e)
        print("  Cut %d: %5.1f %% Bildflaeche, %3d Rechtecke%s"
              % (cut, 100 * r.mean(), n,
                 ("  [%s]" % ", ".join("%s%s%s" % (e[0], "" if e[2] is None else " Fuss y=%d" % e[2],
                                        "" if len(e) < 4 or not e[3] else " Ebene %d" % e[3])
                                        for e in objekte)) if objekte else ""))

    # Container schreiben: bestehende Sektionen anderer Cuts erhalten
    path = os.path.join(a.out, "%s.MSK" % room)
    alt = {}
    if os.path.exists(path):
        b = open(path, "rb").read()
        if b[:4] == b"R15M":
            _, nc = struct.unpack_from("<II", b, 4)
            offs = struct.unpack_from("<%dI" % nc, b, 12)
            ends = sorted([o for o in offs if o] + [len(b)])
            for c in range(nc):
                if offs[c]:
                    alt[c] = b[offs[c]:min(x for x in ends if x > offs[c])]
    if a.cut is None:
        alt = {}                     # ganzer Raum wird neu gebaut
    alt.update(secs)
    open(path, "wb").write(geom.pack_container(alt, rdt[1]))
    print("  %s: %d Cuts im Container" % (os.path.basename(path), len(alt)))

    for cut in secs:
        r = maskenbild.bild(room, cut, a.ppm, a.bild)
        if r:
            print("  %s" % r[0])


if __name__ == "__main__":
    main()
