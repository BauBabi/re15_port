"""SF V8 Runde 3 (O3): Rueckenecken von Mesh 8 so legen, dass der Huegel in PV bleibt (Riegel S3) UND von vorn (P1 = Foto 1, Szene
Takt F344-F364) keine Platte und keine neue Ecke ueber dem Kopf entsteht -> ruecken_v8.json.

    python ruecken_r3.py --kette KETTE_JSON --start RUECKEN_JSON --aus AUS_JSON [--basis EMD_V7] [--runden N] [--nur-start]

Anlass (Sicht-Pruefer V8 r2 Punkt 2): "von vorn steht hinter dem Kopf ein kantiger Kamm ... eine grosse flache Platte mit geraden
Polygonkanten, einer spitzen Ecke ueber dem Kopf"; Ursache ruecken_ecken.py (Runde 2) setzte die Ecken 46/51/30/53/50/49/47/48 nur fuer
PV auf die Vorlage-Kuppel. Nutzer: "smooth straight to head without further hills....", frueher "Auch der obere Ruecken geht im original
screenshot direkt mehr oder weniger in den Kopf ueber. wir haben davor wieder so einen Hueckelhoven."
Verfahren (deterministisch, Koordinatensuche): Kandidaten = die schon verschobenen Ecken (Start-JSON) und jede Ecke von Mesh 8, deren
Bildpunkt in PV bei x 380-452 hoechstens 16 px unter der Oberkontur liegt. Je Ecke Schritte +-s entlang x/y/z des Knochenraums 8
(s = 40, 20, 10, 5 E., je bis --runden Runden), beste Wahl, feste Reihenfolge. Kosten (Huerden mit Abstand zur Riegel-Schwelle; PORT-WAHL):
  PV (Riegel S2/S3 auf der Silhouette mit Schwanz): 4 (Kuppe - 3)+ + 4 (6 - H)+ + 4 (H - 9)+ + 6 [H nicht bei x 430-443] + 6 je weitere
     Erhebung + 0,3 |x(H) - 436| + 8 (Sattel Kopf - 1,2)+ + 4 (S2a-Mittel - 3)+ + 2 (S2a-Max - 5)+ + 4 (S2b - 1,5)+ + 3 (S2c - 2,5)+
  vorn (Knochen 0-15, x 395-515, je Ansicht P1 und Szene Takt F344/F354/F364): 1,0 x Summe (V7-Oberkontur - neue Oberkontur - 1)+ je
     Spalte + 6 je neue konvexe Ecke (r3_gemein.spitzen, nicht in V7 bei +-3 Spalten)
  + 1e-5 x Summe |Verschiebung|^2 (E.^2)
Jede Probe mit Stufe e (v8_bau.einbetten) wie im Bau. Ergebnis = ganzzahlige Verschiebung je Ecke gegen V7 (Modelleinheiten). Flaechen, Normalen, UV, Texel bleiben. Kein RE-Wert, PORT-WAHL.
"""
import os, sys, json, argparse, copy
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V        # noqa: E402
import kopf_orig_bau as KB   # noqa: E402
import v8_bau as VB          # noqa: E402
import riegel_v8 as RG       # noqa: E402
import r3_gemein as R3       # noqa: E402

X_VORN = (395, 515)
SCHRITTE = (40.0, 20.0, 10.0, 5.0)
HUEGEL_X = 436     # Messblatt V8 3.2: hoechste Stelle des Huegels der Vorlage bei x 437 (Sehne 405 -> 447); Mitte des Riegelfensters 428-445
HUEGEL_RAND = 2    # Abstand zum Riegelfenster 428-445 (Zahlen-Pruefer r2: S3a kippte bei 0,5 px Verschiebung - kein Wert auf der Schwelle)


class Ruecken:
    def __init__(self, basis, kette):
        self.blob, _ = VB.bauen(None, 's', kette=kette, blob=basis)
        self.d, self.o, self.ms, self.tim = KB.teile(self.blob)
        self.P0 = np.array([v[:3] for v in self.ms[8]['v']], float)
        sp = R3.szene_posen()
        self.vorn = [R3.P1] + [sp[f] for f in R3.VORN_TAKTE]
        mo7 = V.modell(basis)
        self.ref = []
        for pose in self.vorn:
            o = R3.oben(V.b2.umriss_schnell(mo7, pose, knochen=list(range(16))), *X_VORN)
            self.ref.append((o, R3.spitzen(o)))

    def blob_mit(self, delta):
        ms = copy.deepcopy(self.ms)
        for k, dl in delta.items():
            ms[8]['v'][int(k)][:3] = [int(round(self.P0[int(k)][j] + dl[j])) for j in range(3)]
        VB.einbetten(KB.packen(self.blob, self.d, ms, self.tim), ms, {})     # Stufe e wie im Bau (Wurzelrohr unter der neuen Haut)
        return KB.packen(self.blob, self.d, ms, self.tim)

    def vorn_mass(self, mo):
        out = []
        for pose, (o7, sp7) in zip(self.vorn, self.ref):
            o = R3.oben(V.b2.umriss_schnell(mo, pose, knochen=list(range(16))), *X_VORN)
            ueber = [o7[x] - o[x] for x in o if x in o7]
            neu = [x for x in R3.spitzen(o) if not any(abs(x - y) <= 3 for y in sp7)]
            out.append(dict(ueber_max=max(ueber + [0]), ueber_summe=float(sum(max(0.0, u - 1.0) for u in ueber)), neue_ecken=neu))
        return out

    def messen(self, delta):
        mo = V.modell(self.blob_mit(delta))
        r = V.b2.umriss_schnell(mo, V.PV, knochen=list(range(23)))
        s2, s3, _om = RG.s2_s3(mo, np.where(r, 0, -1))
        return dict(S2=s2, S3=s3, vorn=self.vorn_mass(mo))

    def kosten(self, delta):
        m = self.messen(delta)
        h = lambda v: max(0.0, v)
        s2, s3 = m['S2'], m['S3']
        H, Hx = s3['b_huegel']
        c = (4 * h(s3['a_kuppe'] - 3.0) + 4 * h(6.0 - H) + 4 * h(H - 9.0) + (0 if 428 + HUEGEL_RAND <= Hx <= 445 - HUEGEL_RAND else 6) + 6 * len(s3['c_weitere'])
             + 8 * h(s3['d_sattel_kopf'][0] - 1.2) + 4 * h(s2['a']['mittel'] - 3.0) + 2 * h(s2['a']['max'] - 5) + 4 * h(s2['b'][0] - 1.5)
             + 3 * h(s2['c']['huckel_px'] - 2.5) + 0.3 * abs(Hx - HUEGEL_X))
        for v in m['vorn']:
            c += 1.0 * v['ueber_summe'] + 6 * len(v['neue_ecken'])
        c += 1e-5 * sum(float(np.sum(np.square(dl))) for dl in delta.values())
        return c, m


def kandidaten(R, delta):
    blob = R.blob_mit(delta)
    mo = V.modell(blob)
    Rr, T = V.pose_r_t(mo)
    R8, T8 = Rr[8].astype(float) / 4096.0, T[8].astype(float)
    _d, _o, ms, _t = KB.teile(blob)
    P = np.array([v[:3] for v in ms[8]['v']], float)
    q = V.proj(V.PV, P @ R8.T + T8)
    top = R3.oben(V.b2.umriss_schnell(mo, V.PV, knochen=list(range(23))), 300, 470)
    out = set(int(k) for k in delta)
    for i, (x, y) in enumerate(q):
        xi = int(round(x))
        if 380 <= xi <= 452 and xi in top and y - top[xi] <= 16:
            out.add(i)
    return sorted(out)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--kette', required=True); ap.add_argument('--start', required=True); ap.add_argument('--aus', required=True)
    ap.add_argument('--basis', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--runden', type=int, default=3)
    ap.add_argument('--schritt-max', type=float, default=40.0)
    ap.add_argument('--nur-start', action='store_true', help='nur messen (Start-JSON), keine Suche')
    a = ap.parse_args()
    basis = open(a.basis, 'rb').read()
    assert V.sha(basis) == V.V7_SHA, 'Basis muss V7 sein'
    R = Ruecken(basis, json.load(open(a.kette)))
    st = json.load(open(a.start))
    delta = {str(k): [float(c) for c in v] for k, v in st['ecken'].items()}
    c0, m0 = R.kosten(delta)
    print('Start Kosten %.3f' % c0, json.dumps(m0, default=str))
    if a.nur_start:
        return
    kand = kandidaten(R, delta)
    print('Kandidaten', kand)
    cb, mb = c0, m0
    for s in [t for t in SCHRITTE if t <= a.schritt_max]:
        for rnd in range(a.runden):
            besser = False
            for i in kand:
                for ax in range(3):
                    for sg in (1, -1):
                        dn = {k: list(v) for k, v in delta.items()}
                        cur = list(dn.get(str(i), [0.0, 0.0, 0.0]))
                        cur[ax] += sg * s
                        dn[str(i)] = cur
                        c, m = R.kosten(dn)
                        if c < cb - 1e-9:
                            cb, mb, delta, besser = c, m, dn, True
            print('Schritt %g Runde %d Kosten %.3f' % (s, rnd, cb), flush=True)
            if not besser:
                break
    print('Ende Kosten %.3f' % cb, json.dumps(mb, default=str))
    out = dict(quelle='ruecken_r3.py (Runde 3, O3; Basis V7 %s, Kette %s, Start %s): Koordinatensuche PV-Huegel + Vorderansicht' % (
        V.V7_SHA[:8], os.path.basename(a.kette), os.path.basename(a.start)),
        ecken={k: [int(round(c)) for c in v] for k, v in sorted(delta.items(), key=lambda kv: int(kv[0])) if any(abs(c) >= 0.5 for c in v)},
        start_quelle=st.get('quelle'), kosten_start=round(c0, 4), kosten=round(cb, 4), masse=mb)
    json.dump(out, open(a.aus, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))


if __name__ == '__main__':
    main()
