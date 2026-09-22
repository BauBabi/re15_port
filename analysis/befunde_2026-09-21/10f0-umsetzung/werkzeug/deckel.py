"""Welche Flaeche des Quaders liefert die schaedlichen Maskenpunkte — Seite oder DECKEL?

geom.quader_tiefe setzt jede Kollisionszelle als Quader MIT Deckflaeche an. Der Deckel
ist bei einem Tisch oder einer Trennwand richtig (die Kamera schaut darauf). Bei einem
Buerostuhl gibt es ihn nicht: eine 1500x1500 grosse waagerechte Platte 1950 Einheiten
ueber dem Boden existiert im Bild nirgends. Jeder Bildpunkt, dessen naechste Quader-
flaeche der DECKEL ist, steht also nur wegen dieser Fiktion in der Silhouette.

Gemessen wird: wieviele der an der Nutzer-Marke faelschlich verdeckten Figurpunkte auf
Deckel-Bildpunkte zurueckgehen.
"""
import json
import os
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
sys.path.insert(0, HIER)
os.chdir(WURZEL)

import geom
import maske_aus_png
import raum
import variante
from geom import load_bg, load_rdt

CD = "re15_port/shared_assets/PSX"


def quader_flaechen(R, t, H, X0, X1, Z0, Z1, hoehe):
    """Wie geom.quader_tiefe, gibt aber zusaetzlich zurueck, ob der DECKEL der
    naechste Treffer war.  -> (vz, treffer, deckel)"""
    _R = np.array(R, float).reshape(3, 3) / 4096.0
    _Ri = np.linalg.inv(_R)
    _c = _Ri.dot(-np.array(t, float))
    _sx, _sy = np.meshgrid(np.arange(320) - 160.0, np.arange(240) - 120.0)
    _d = np.stack([_sx, _sy, np.full_like(_sx, float(H))], -1) @ _Ri.T
    best = np.full((240, 320), np.inf)
    deck = np.zeros((240, 320), bool)

    def eintragen(s, q, ist_deckel):
        vz = (q[..., 0] * R[6] + q[..., 1] * R[7] + q[..., 2] * R[8]) / 4096.0 + t[2]
        ok = (s > 0) & np.isfinite(vz) & (vz > 64) & (vz < best)
        best[ok] = vz[ok]
        deck[ok] = ist_deckel

    for achse, wert, lo, hi, oa in ((0, X0, Z0, Z1, 2), (0, X1, Z0, Z1, 2),
                                    (2, Z0, X0, X1, 0), (2, Z1, X0, X1, 0)):
        dd = _d[..., achse]
        with np.errstate(divide="ignore", invalid="ignore"):
            s = (wert - _c[achse]) / dd
            q = _c + s[..., None] * _d
        gut = (np.isfinite(s) & (np.abs(dd) > 1e-9) & (q[..., oa] >= lo) & (q[..., oa] <= hi)
               & (q[..., 1] <= 0) & (q[..., 1] >= hoehe))
        eintragen(np.where(gut, s, -1.0), np.nan_to_num(q), False)
    with np.errstate(divide="ignore", invalid="ignore"):
        s = (hoehe - _c[1]) / _d[..., 1]
        q = _c + s[..., None] * _d
    gut = (np.isfinite(s) & (np.abs(_d[..., 1]) > 1e-9)
           & (q[..., 0] >= X0) & (q[..., 0] <= X1) & (q[..., 2] >= Z0) & (q[..., 2] <= Z1))
    eintragen(np.where(gut, s, -1.0), np.nan_to_num(q), True)
    tr = np.isfinite(best)
    return np.where(tr, best, 0.0), tr, (deck & tr)


def main():
    aus = json.load(open(raum.AUSWAHL, encoding="utf-8"))["ROOM10F0"]
    rdt, _ = load_rdt(CD, "ROOM10F0")
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    sp = geom.sca_sperrzellen(rdt, 0) or []
    # Eichung: quader_flaechen muss dieselbe Silhouette liefern wie geom.quader_tiefe
    v = geom.cut_view(rdt, cam, 4)
    for ki, (zx, zz, zw, zd, typ) in enumerate(sp):
        if typ != 3:
            continue
        a_vz, a_tr = geom.quader_tiefe(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, -1950)
        b_vz, b_tr, b_dk = quader_flaechen(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, -1950)
        assert (a_tr == b_tr).all() and np.allclose(a_vz, b_vz), "Eichung Zelle %d" % ki
    print("EICHUNG quader_flaechen == geom.quader_tiefe: bestanden (alle Typ-3-Zellen)")

    for cut in (4, 5):
        v = geom.cut_view(rdt, cam, cut)
        bg = load_bg("build/bg_ppm", 0x10F0, cut)
        dk = bg.astype(int).sum(2) < 45
        kunst = np.zeros((240, 320), bool)
        for o in aus[str(cut)]["objekte"]:
            if "png" in o:
                r = maske_aus_png.setze(o["png"], o["x"], o["y"], o.get("massstab", 1))
                if r is not None:
                    kunst |= r
        ges_sil = np.zeros((240, 320), bool)
        ges_deck = np.zeros((240, 320), bool)
        for ki, (zx, zz, zw, zd, typ) in enumerate(sp):
            if typ != 3:
                continue
            _vz, tr, dcl = quader_flaechen(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, -1950)
            ges_sil |= tr
            ges_deck |= dcl
        print("CUT %d: Tiefschwarz in irgendeiner Stuhl-Silhouette %5d px, "
              "davon auf dem DECKEL %5d (%.1f %%) | Kunst auf dem Deckel %d"
              % (cut, int((dk & ges_sil).sum()), int((dk & ges_sil & ges_deck).sum()),
                 100.0 * (dk & ges_sil & ges_deck).sum() / max(1, (dk & ges_sil).sum()),
                 int((kunst & ges_deck).sum())))
        np.save("build/r22/deckel_c%d.npy" % cut, ges_deck)

    # Wirkung an der Marke
    fz = np.fromfile("build/r22/fig_f335.bin", np.int32).reshape(240, 320)
    gez = fz > 0
    bk = (1023 * np.where(gez, fz, 1).astype(np.int64)) >> 16
    deck_map = np.load("build/r22/deckel_c4.npy")
    d0, t0, _ = variante.deckung_tiefe("build/r22/v0_auslieferung", 4)
    verd = gez & d0 & (t0 < bk)
    kunst = np.zeros((240, 320), bool)
    for o in aus["4"]["objekte"]:
        if "png" in o:
            r = maske_aus_png.setze(o["png"], o["x"], o["y"], o.get("massstab", 1))
            if r is not None:
                kunst |= r
    zugabe = verd & ~kunst
    print("\nMARKE F335: verdeckt %d, davon Zugabe %d, davon auf DECKEL-Bildpunkten %d (%.0f %%)"
          % (int(verd.sum()), int(zugabe.sum()), int((zugabe & deck_map).sum()),
             100.0 * (zugabe & deck_map).sum() / max(1, zugabe.sum())))


if __name__ == "__main__":
    main()
