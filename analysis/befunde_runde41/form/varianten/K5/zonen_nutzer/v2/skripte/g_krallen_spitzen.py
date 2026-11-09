"""Krallen: Nutzer-Schleifen/Formen und ihre Spitzen gegen die Modellspitzen (K5c-Geometrie, Pose Bild 2); hinten Nutzermarken gegen Fuss 25."""
import json
import numpy as np, cv2
from scipy import ndimage as ndi
from g_basis import *
import g_bau
from g_krallen import proj_punkt, spitze, ka, kc, ecken

MS = dict(np.load(ARBEIT + '/zonen_v2_masken.npz'))


def spitze_von(maske_g, n=2):
    """spitzeste Ecke des Umrisses (approxPolyDP 2,5 px, kleinster Innenwinkel), in Vorlage-Koordinaten."""
    cnt, _ = cv2.findContours(maske_g.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    c = max(cnt, key=cv2.contourArea)
    a = cv2.approxPolyDP(c, 2.5, True)[:, 0, :].astype(float)
    best = None
    for i in range(len(a)):
        p, q, r = a[i - 1], a[i], a[(i + 1) % len(a)]
        u, v = p - q, r - q
        w = np.degrees(np.arccos(np.clip(np.dot(u, v) / (np.linalg.norm(u) * np.linalg.norm(v) + 1e-9), -1, 1)))
        if best is None or w < best[0]:
            best = (w, q)
    x, y = v_aus_g(n, best[1][0], best[1][1])
    return [round(float(x), 1), round(float(y), 1)], round(float(best[0]), 1)


def spitze_fern(maske_g, bezug_g, n=2):
    """Runde 2 (Pruefer r1 Nr. 11): Spitze = Umrisspunkt der Schleife mit dem groessten Abstand zum Schwerpunkt des "Fußabschnitts"
    (Krallen zeigen vom Fuss weg). Ersetzt die Regel "spitzeste Ecke", die bei Schleife 1 den oberen Ansatz traf."""
    cnt, _ = cv2.findContours(maske_g.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    c = max(cnt, key=cv2.contourArea)[:, 0, :].astype(float)
    ys, xs = np.nonzero(bezug_g); m = np.array([xs.mean(), ys.mean()])
    q = c[np.argmax(np.hypot(*(c - m).T))]
    x, y = v_aus_g(n, q[0], q[1])
    return [round(float(x), 1), round(float(y), 1)]


def modellspitzen():
    out = {}
    for b in (13, 14, 15, 28, 30, 18, 19, 20, 29, 31):
        for name, mo in (('asset', ka.mo), ('k5c', kc.mo)):
            p = spitze(mo.meshes[b])
            out.setdefault(str(b), {})[name] = [round(c, 1) for c in proj_punkt(ka.mo, b, p)]
    return out


def main():
    erg = {}
    MSp = modellspitzen(); erg['modellspitzen_vorlage'] = MSp
    links = {}
    for i in range(1, 5):
        m = MS['kralle_vorn_links_schleife%d' % i]
        sp_alt, w = spitze_von(m)
        sp = spitze_fern(m, MS['fussabschnitt_vorn_links'] & ~MS['krallen_vorn_links'])
        d = {b: round(float(np.hypot(MSp[b]['k5c'][0] - sp[0], MSp[b]['k5c'][1] - sp[1])), 1) for b in ('13', '14', '15', '28', '30')}
        links['schleife%d' % i] = dict(nutzerspitze_vorlage=sp, regel='fernster Umrisspunkt vom Schwerpunkt des Fußabschnitts',
                                       spitze_runde1_vorlage=sp_alt, abstand_zu_modellspitze_k5c_px=d, naechste=min(d, key=d.get))
    erg['vorn_links'] = links
    # vorn rechts: drei Gebiete der Nutzer-Zeichnung einzeln (Saaten wie v1)
    lf = 'vorne_rechte_Hand_und_Kralle.png'; dx = VS[lf]['dx']
    rechts = {}
    for name, saat in (('form1', (440, 545)), ('form3', (312, 592)), ('dazwischen', (360, 600))):
        m, _ = g_bau.flut(lf, ['rot', 'hellblau'], [saat], box=(290, 500, 470, 620), nur_figur=False)
        mg = m[:, dx:dx + grund_rgba(2).shape[1]]
        MS['krallen_vorn_rechts_' + name] = mg
        ys, xs = np.nonzero(mg)
        e = dict(px_vorlage=round(float(mg.sum() * s_t(2)[0] ** 2)), bbox_grundlage=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())])
        if name != 'dazwischen':
            # Enden der Form: oberster und unterster Punkt
            o = np.argmin(ys); u = np.argmax(ys)
            e['oberes_ende_vorlage'] = [round(float(v), 1) for v in v_aus_g(2, xs[o], ys[o])]
            e['unteres_ende_vorlage'] = [round(float(v), 1) for v in v_aus_g(2, xs[u], ys[u])]
        rechts[name] = e
    for name, b in (('form1', '31'), ('form3', '19')):
        p = rechts[name]['unteres_ende_vorlage']
        rechts[name]['modell'] = b
        rechts[name]['abstand_unteres_ende_zu_spitze_k5c_px'] = round(float(np.hypot(MSp[b]['k5c'][0] - p[0], MSp[b]['k5c'][1] - p[1])), 1)
    erg['vorn_rechts'] = rechts
    np.savez_compressed(ARBEIT + '/zonen_v2_masken.npz', **MS)
    json.dump(erg, open(ARBEIT + '/krallen_spitzen.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print(json.dumps(erg, ensure_ascii=False, indent=1))


if __name__ == '__main__':
    main()
