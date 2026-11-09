"""SF V8 Runde 5 (P5, P2): Rueckenecken von Mesh 8 so legen, dass zwischen Ruecken und Kopf keine Luecke bleibt -> ruecken_v8.json.

    python ruecken_r5.py --kette KETTE_JSON --start RUECKEN_JSON --aus AUS_JSON [--basis EMD_V7] [--jobs N] [--runden N] [--nur-start]

Nutzer 2026-11-09 (woertlich): "Das einzige was mir noch auffällt, es gibt immer noch eine Lücke zwischen Rücken und Kopf beim 2 beiner
Birkin. Das ist in der Vorlage so wie es aussieht nicht so." Darstellung.png (weiss): "smooth straight to head without further hills....".
Zahlen-Pruefer r4 (P2): "S2c 8,0 px bei x 376 ... der Schwanz taucht zwischen x 361 und 377 unter die Rumpf-Oberkante, robust in 7
Halbpixel-Varianten".
Mesh 9 (Kopf) bleibt bytegleich V7 (Nutzer: Gesicht "jetzt schon gut"); geschlossen wird nur ueber Ecken von Mesh 8 (Flaechen, Normalen,
UV, Texel bleiben; Normalen werden im Bau nicht neu gerechnet - wie Runde 2-4).
Verfahren (deterministisch): Koordinatensuche wie ruecken_r4 (Schritte 40/20/10/5 E. entlang x/y/z des Knochenraums 8), je Durchgang alle
Einzelschritte parallel bewertet; angenommen wird der beste Einzelschritt, dann gierig je weitere Ecke ihr bester Schritt, wenn die Summe
besser wird (Zwischenstand nach jedem Durchgang in --aus; --start nimmt ihn wieder auf). Kosten = ruecken_r4.Ruecken4.kosten (PV S2/S3, Vorderansicht, Schlitz, Bedeckung, S4) und neu (PORT-WAHL):
  + W_LUECKE je Luecken-Bildpunkt (luecke_r5.luecke, Ansichten SUCH_ANSICHTEN, Halbpixel-Variante 0; Riegel S16 prueft danach alle Ansichten
    und alle 7 Varianten)
  + W_GESICHT je Gesichts-Bildpunkt (Knochen 9, lok x >= GESICHT_X), der gegen den Start verloren geht (Mesh 8 darf das Gesicht nicht
    verdecken; "ohne Durchstossen")
  + W_S2C (S2c mit Culling, Variante 0 - 2,5)+ (Riegel S2c Runde 5 <= 3 robust)
Aktive Menge (--zusatz): nach jeder Suche misst luecke_r5.py alle 177 Ansichten; Ansichten mit Luecke kommen fuer die naechste Suche dazu.
Kandidaten: ruecken_r4.kandidaten (x0 330) und jede Ecke von Mesh 8 mit Ort y < -800 im Umkreis 700 E. um das Kopfgelenk (111, -971, -133).
Kein RE-Wert.
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V        # noqa: E402
import kopf_orig_bau as KB   # noqa: E402
import ruecken_r4 as RR4     # noqa: E402
import luecke_r5 as L        # noqa: E402

SCHRITTE = (80.0, 40.0, 20.0, 10.0, 5.0)  # wie ruecken_r3/r4 (2,6 / 1,3 / 0,65 / 0,33 px in PV), dazu 80 E. (5,2 px): die Kerben in der Szene
                                    # sind bis 12 px breit (Takt F278, gemessen luecke_r5)
W_LUECKE = 1.0                      # je Luecken-Bildpunkt: 1 px Kerbe so teuer wie 1 px Ueberschreitung der Vorderkontur (ruecken_r3 vorn 1,0)
W_GESICHT = 0.5                     # je verlorenem Gesichts-Bildpunkt (halb so teuer wie ein Luecken-Bildpunkt: hinter dem Kopf darf die Haut
                                    # den Hinterkopf decken, das Gesicht nicht; Riegel S16 (c) prueft das Gesicht danach streng)
GESICHT_X = 150                     # Mesh 9 lok x >= 150 = vordere Haelfte des Kopfes (Mesh 9 x -156..353, Gesicht vorn; gemessen geo.py)
W_S2C = 3.0                         # wie ruecken_r3 (3 x (S2c - 2,5))
MERGE_MAX = 12                      # je Durchgang hoechstens 12 Ecken nacheinander zusammengefuehrt (Rechenzeit; PORT-WAHL)
KOPF_GELENK = np.array([111.0, -971.0, -133.0])   # EMR relpos Knochen 9 im Knochenraum 8
SUCH_ANSICHTEN = ('PV', 'P1', 'P2', 'F262', 'F270', 'F278', 'F290', 'F300', 'F310', 'F312', 'F316', 'F320', 'PV+512', 'PV+1024',
                  'PV+1536', 'PV+1792', 'PV+3584', 'PV+3840', 'P2+1024', 'P2+1536', 'P2+1792', 'P2+2048', 'P2+3840')


def gesicht_px(A):
    b = A['bone'][:480]
    m = b == 9
    return int((A['lok'][:480][m][:, 0] >= GESICHT_X).sum())


class Ruecken5(RR4.Ruecken4):
    def __init__(self, basis, kette, start_delta=None, zusatz=()):
        super().__init__(basis, kette)
        av = dict(L.ansichten('alle'))
        self.such = [(n, av[n]) for n in list(SUCH_ANSICHTEN) + [z for z in zusatz if z not in SUCH_ANSICHTEN]]
        self.gesicht0 = None
        if start_delta is not None:
            mo = V.modell(self.blob_mit(start_delta))
            self.gesicht0 = [gesicht_px(L.zeichne(mo, p, L.VARIANTEN[0])) for _n, p in self.such]

    def kosten(self, delta):
        c, m = super().kosten(delta)
        blob = KB.packen(self.blob, self.d, self._ms, self.tim)
        mo = V.modell(blob)
        lu, ge = [], []
        for n, p in self.such:
            A = L.zeichne(mo, p, L.VARIANTEN[0])
            r, _G = L.luecke(A)
            lu.append((n, r['kerbe'] + r['spalt']))
            ge.append(gesicht_px(A))
        verlust = sum(max(0, g0 - g) for g0, g in zip(self.gesicht0, ge)) if self.gesicht0 else 0
        import riegel_v8 as RG
        s2c = RG.s2c_r5_eine(mo)
        m['luecke'] = lu
        m['gesicht_verlust'] = verlust
        m['s2c_r5'] = s2c
        return (c + W_LUECKE * sum(v for _n, v in lu) + W_GESICHT * verlust + W_S2C * max(0.0, s2c - 2.5)), m


_R = None


def _init(basis_pfad, kette_pfad, start, zusatz=()):
    global _R
    _R = Ruecken5(open(basis_pfad, 'rb').read(), json.load(open(kette_pfad)), start, zusatz)


def _eval(delta):
    c, m = _R.kosten(delta)
    return c, json.loads(json.dumps(m, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o)))


def kandidaten(R, delta):
    out = set(RR4.kandidaten(R, delta, 330))
    P = np.array([v[:3] for v in R._ms[8]['v']], float)
    for i, p in enumerate(P):
        if p[1] < -800 and np.linalg.norm(p - KOPF_GELENK) <= 700:
            out.add(i)
    return sorted(out)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--kette', required=True); ap.add_argument('--start', required=True); ap.add_argument('--aus', required=True)
    ap.add_argument('--basis', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--runden', type=int, default=4)
    ap.add_argument('--jobs', type=int, default=12)
    ap.add_argument('--schritt-max', type=float, default=80.0)
    ap.add_argument('--schritt-min', type=float, default=5.0)
    ap.add_argument('--nur-start', action='store_true')
    ap.add_argument('--gesicht-ref', default=os.path.join(HIER, 'ruecken_v8.json'), help='Rueckenecken des Bezugs fuer den Gesichtsverlust')
    ap.add_argument('--zusatz', default='', help='weitere Ansichten (luecke_r5.ansichten-Namen, Komma): aktive Menge nach der Vollmessung')
    ap.add_argument('--kand', default='', help='feste Kandidatenliste (Komma), sonst kandidaten()')
    a = ap.parse_args()
    basis = open(a.basis, 'rb').read()
    assert V.sha(basis) == V.V7_SHA, 'Basis muss V7 sein'
    st = json.load(open(a.start))
    delta = {str(k): [float(c) for c in v] for k, v in st['ecken'].items()}
    ref = {str(k): [float(c) for c in v] for k, v in json.load(open(a.gesicht_ref))['ecken'].items()}
    zusatz = [z for z in a.zusatz.split(',') if z]
    R = Ruecken5(basis, json.load(open(a.kette)), ref, zusatz)
    c0, m0 = R.kosten(delta)
    print('Start Kosten %.3f' % c0, json.dumps(m0, default=str), flush=True)
    if a.nur_start:
        return
    kand = [int(k) for k in a.kand.split(',')] if a.kand else kandidaten(R, delta)
    print('Kandidaten', kand, flush=True)
    from multiprocessing import Pool
    cb, mb = c0, m0
    with Pool(a.jobs, initializer=_init, initargs=(a.basis, a.kette, ref, zusatz)) as pool:
        for s in [t for t in SCHRITTE if a.schritt_min <= t <= a.schritt_max]:
            for rnd in range(a.runden * 3):
                zuege = []
                for i in kand:
                    for ax in range(3):
                        for sg in (1, -1):
                            dn = {k: list(v) for k, v in delta.items()}
                            cur = list(dn.get(str(i), [0.0, 0.0, 0.0]))
                            cur[ax] += sg * s
                            dn[str(i)] = cur
                            zuege.append((i, ax, sg, dn))
                res = pool.map(_eval, [z[3] for z in zuege])
                bessere = sorted([(r[0], z) for r, z in zip(res, zuege) if r[0] < cb - 1e-9], key=lambda t: t[0])
                if not bessere:
                    print('Schritt %g Durchgang %d: keine Verbesserung (Kosten %.3f)' % (s, rnd, cb), flush=True)
                    break
                # gierig zusammenfuehren: bester Einzelschritt, dann je weitere Ecke (nach Guete) ihr bester Schritt, wenn die Summe besser wird
                je_ecke = {}
                for c, z in bessere:
                    je_ecke.setdefault(z[0], (c, z))
                c1, z1 = bessere[0]
                delta, cb = z1[3], c1
                genommen = [z1[0]]
                for i, (c, z) in sorted(je_ecke.items(), key=lambda kv: kv[1][0])[1:MERGE_MAX]:
                    ver = {k: list(v) for k, v in delta.items()}
                    cur = list(ver.get(str(i), [0.0, 0.0, 0.0]))
                    cur[z[1]] += z[2] * s
                    ver[str(i)] = cur
                    cv, _mv = R.kosten(ver)
                    if cv < cb - 1e-9:
                        delta, cb = ver, cv
                        genommen.append(i)
                cb, mb = R.kosten(delta)
                art = 'Ecken %s' % genommen
                schreiben(a, delta, st, c0, cb, mb, kand)
                print('Schritt %g Durchgang %d Kosten %.3f (%s)' % (s, rnd, cb, art), json.dumps(dict(S2=mb['S2'], S3=mb['S3'],
                      luecke=sum(v for _n, v in mb['luecke']), gesicht=mb['gesicht_verlust'], s2c=mb['s2c_r5'], vorn=mb['vorn'],
                      schlitz=sum(v for _n, v in mb['schlitz']), s4w=mb['s4_wurzel']), default=str), flush=True)
    schreiben(a, delta, st, c0, cb, mb, kand)


def schreiben(a, delta, st, c0, cb, mb, kand):
    out = dict(quelle='ruecken_r5.py (Runde 5, P5 Luecke Ruecken/Kopf + P2 S2c; Basis V7 %s, Kette %s, Start %s)' % (
        V.V7_SHA[:8], os.path.basename(a.kette), os.path.basename(a.start)),
        ecken={k: [int(round(c)) for c in v] for k, v in sorted(delta.items(), key=lambda kv: int(kv[0])) if any(abs(c) >= 0.5 for c in v)},
        start_quelle=st.get('quelle'), kosten_start=round(c0, 4), kosten=round(cb, 4), masse=mb,
        verfahren='Koordinatensuche parallel (bester Einzelschritt + gierig weitere Ecken, je hoechstens %d), Schritt %g..%g, Kandidaten %s'
                  % (MERGE_MAX, a.schritt_min, a.schritt_max, kand))
    json.dump(out, open(a.aus, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))


if __name__ == '__main__':
    main()
