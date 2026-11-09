"""SF V8 (Runde 2): Auswahl der GANZEN V7-UV-Inseln fuer die Ader-Zeilen X1/X2/X3 -> adern_v8.json (Messblatt V8 6c).

    python adern_suche.py [--basis EMD] [--kette JSON] [--ruecken JSON] [--aus JSON]

Nutzer 2026-11-08 (woertlich): "die Rechte Seite auch mehr rottöne ... also quasi mehr adern an verschiedenen Stellen, am Arm und Körper,
nicht nur grau. Übernehme die die Textur Sachen die du dafür brauchst vom originalen Birkin von Resident Evil 1.5".
Pruefung V8 r1 (Sicht-Pruefer, Punkt 3): Runde 1 schob 9 Teil-Inseln je fuer sich auf aderreiche Fenster - im Spiel von hinten ein
"Flickenteppich" (Streifen enden an Dreieckskanten, gleiche V-Adern nebeneinander, Naehte zwischen den Inseln); Oberarm-Wulst und
untere Flanke blieben grau. Runde 2:
  Insel = alle Flaechen eines Meshes mit derselben CLUT-Zeile und Textur-Seite, die eine UV-Ecke teilen (zusammenhaengend) - die
  UV-Inseln von V7 (= Original-EM30-UV, Texel = Original-Texel, Riegel S7a). Eine Insel wechselt GANZ die Zeile, die UV bleibt: die
  Original-Textur liegt durchgehend wie beim RE1.5-Birkin, keine neue Naht.
  Gewaehlt (Seite (W)-rechts = Knochenraum-8 z < 0, v8_gemein Kopf; "am Arm und Koerper"):
    Mesh 8  (Rumpf):           Inseln der Zeilen 480/481 mit Schwerpunkt z < 0, die in PV >= 10 Bildpunkte zeigen
    Mesh 13 (Oberarm (W)-r.):  Inseln der Zeile 480, die in PV >= 10 Bildpunkte zeigen
    Mesh 14 (Unterarm (W)-r.): Inseln der Zeile 482 (heller Wulst unter dem Buckel, sfv8/r2 diag1.py: 14/482 x 346-395 y 293-363),
                               die in PV >= 10 Bildpunkte zeigen
  Probe sfv8/r2 adp2.py / adp3.py / adp4.py: dazu ein starrer Schub je ganzer Insel (19 gueltige Schuebe fuer die groesste Rumpfinsel)
  hob den roten Anteil in PV (Riegel S7c) nur von 5,7 auf 14,3 % und die roten Flecken (S7d) auf 30 > 27 (Original) - verworfen.
Kein RE-Wert; das Ergebnis ist PORT-WAHL.
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import adern_v8 as AV                 # noqa: E402
import f_rast as FR                   # noqa: E402

WAHL = {8: (480, 481), 13: (480,), 14: (482,)}
MIN_PX = 10


def ecken_uv(uv):
    n = 3 if len(uv) == 9 else 4
    po = (uv[5] & 0xF) * 128
    return [(uv[3 * k] + po, uv[3 * k + 1]) for k in range(n)]


def inseln(m, zeilen):
    fs = [('d%d' % i, u) for i, u in enumerate(m['tuv'])] + [('v%d' % i, u) for i, u in enumerate(m['quv'])]
    fs = [f for f in fs if (f[1][2] >> 6) & 0x1FF in zeilen]
    par = {k: k for k, _u in fs}

    def w(k):
        while par[k] != k:
            k = par[k]
        return k
    for i, (ka, ua) in enumerate(fs):
        for kb, ub in fs[i + 1:]:
            if (ua[2] >> 6) == (ub[2] >> 6) and (ua[5] & 0xF) == (ub[5] & 0xF) and set(ecken_uv(ua)) & set(ecken_uv(ub)):
                par[w(ka)] = w(kb)
    out = {}
    for k, _u in fs:
        out.setdefault(w(k), []).append(k)
    return [sorted(v, key=lambda q: (q[0], int(q[1:]))) for _w, v in sorted(out.items(), key=lambda kv: (kv[0][0], int(kv[0][1:])))]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=V.ASSET)
    ap.add_argument('--kette', default=os.path.join(HIER, 'kette_v8.json'))
    ap.add_argument('--ruecken', default=os.path.join(HIER, 'ruecken_v8.json'))
    ap.add_argument('--aus', default=os.path.join(HIER, 'adern_v8.json'))
    a = ap.parse_args()
    import v8_bau as VB
    basis = open(a.basis, 'rb').read()
    blob, _p = VB.bauen(a.basis, 'sr', kette=json.load(open(a.kette)), ruecken=json.load(open(a.ruecken)))
    _d, _o, ms, _tim = KB.teile(blob)
    mo = V.modell(blob)
    A = FR.mit_bg(mo, V.PV)
    bone, fl = A['bone'][:480], A['flaeche'][:480]
    flaechen, liste = {}, []
    for b, zeilen in sorted(WAHL.items()):
        m = ms[b]
        P = np.array([v[:3] for v in m['v']], float)
        for keys in inseln(m, zeilen):
            fz = [m['qf'][int(k[1:])] if k[0] == 'v' else m['tf'][int(k[1:])] for k in keys]
            zs = float(np.mean([P[i][2] for f in fz for i in f[1::2]]))
            px = int(((bone == b) & np.isin(fl, keys)).sum())
            zeile = (AV.flaeche(m, keys[0])[2] >> 6) & 0x1FF
            ok = px >= MIN_PX and (b != 8 or zs < 0)
            liste.append(dict(mesh=b, zeile=zeile, flaechen=len(keys), erste=keys[0], z_schwer=round(zs, 1), pv_px=px, gewaehlt=ok))
            if ok:
                flaechen.setdefault(str(b), []).extend(keys)
            print('Mesh %d Zeile %d Insel %s (%d Flaechen): z %.0f, PV %d px -> %s' % (b, zeile, keys[0], len(keys), zs, px, 'X' if ok else '-'))
    res = dict(quelle='adern_suche.py Runde 2 (Basis %s sha256 %s, Kette %s, Ruecken %s): ganze V7-UV-Inseln, UV unveraendert' % (
                   os.path.basename(a.basis), V.sha(basis), os.path.basename(a.kette), os.path.basename(a.ruecken)),
               zeilen=[0, 1, 2], flaechen=flaechen, inseln=liste)
    json.dump(res, open(a.aus, 'w'), indent=1)
    print('%d Flaechen gewaehlt' % sum(len(v) for v in flaechen.values()))


if __name__ == '__main__':
    main()
