"""Spielbild (gdigrab, eigenes Fenster 640 x 480) gegen Vorhersage: Maler-Ordnung (= Spiel-Regel) und Tiefenpuffer, beide mit Disc-Licht
des Schnitts, Spiel-Asset, Kamera = RID-Schnitt des Raums, Fenster = 2 x Port-Pixel. Misst, welche der beiden Vorhersagen das Spielbild
in den Streifen-Bildpunkten trifft.
Aufruf: python s_spielvergleich.py <name> <raum> <x,z,gier,clip,bild,schnitt> <aufnahme.png>"""
import json
import cv2
from s_basis import *  # noqa: F401,F403

KOPF, KIEFER, HALS, BRUST = 22, 23, 21, 10


def vorhersage(raum, pose, ordner=ASSET):
    x, z, gier, clip, bild, schnitt = (int(v) for v in pose.split(','))
    R_ = FB.Raum(raum)
    s = R_.schnitt(schnitt)
    kam = FB.Kamera.aus_rid(s['pos'], s['ziel'], fov=s['fov'])
    k = kand(ordner)
    tris = dreiecke_3d(k.mo, k.kf(clip, bild), (x, 0, z), gier, kam, R_.licht(schnitt))
    Mw = np.diag([2.0, 2.0, 1.0])
    R = rastern_beide(tris, k.tex, 480, 640, Mw)
    return R, tris


def vergleich(name, raum, pose, aufnahme):
    R, tris = vorhersage(raum, pose)
    tm = tri_mesh(tris)
    ma = np.where(R['maler_i'] >= 0, tm[np.maximum(R['maler_i'], 0)], -1)
    mb = np.where(R['puffer_i'] >= 0, tm[np.maximum(R['puffer_i'], 0)], -1)
    g = cv2.cvtColor(cv2.imread(aufnahme), cv2.COLOR_BGR2RGB).astype(float)
    fig = R['maler_i'] >= 0
    kern = cv2.erode(fig.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
    # innen liegende Dreiecksgrenzen ausnehmen: nur Bildpunkte, deren 3x3-Nachbarn dasselbe Dreieck (Maler) bzw. dasselbe (Puffer) tragen
    def gleich(idx):
        m = np.ones(idx.shape, bool)
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                m &= np.roll(np.roll(idx, dy, 0), dx, 1) == idx
        return m
    ruhig = kern & gleich(R['maler_i']) & gleich(R['puffer_i'])
    agree = ruhig & (R['maler_i'] == R['puffer_i'])
    keil = ruhig & np.isin(ma, (KOPF, KIEFER)) & np.isin(mb, (BRUST, HALS))
    gegen = ruhig & np.isin(ma, (BRUST, HALS)) & np.isin(mb, (KOPF, KIEFER))
    allediff = ruhig & (R['maler_i'] != R['puffer_i'])
    pm = R['rgb_maler'].astype(float); pp = R['rgb_puffer'].astype(float)

    def mae(a, msk):
        return float(np.abs(g[msk] - a[msk]).mean()) if msk.any() else None
    erg = dict(name=name, raum=raum, pose=pose, aufnahme=aufnahme, figur_px=int(fig.sum()))
    for nm, msk in (('uebereinstimmend', agree), ('keil_kopf_ueber_brust_hals', keil), ('brust_hals_ueber_kopf', gegen), ('alle_ordnungsfehler', allediff)):
        # Anteil Bildpunkte, bei denen das Spiel naeher an der Maler-Vorhersage liegt als am Tiefenpuffer
        if msk.any():
            dm = np.abs(g[msk] - pm[msk]).sum(1); dp = np.abs(g[msk] - pp[msk]).sum(1)
            unterscheidbar = np.abs(pm[msk] - pp[msk]).sum(1) > 12
            naeher = float(((dm < dp) & unterscheidbar).sum() / max(unterscheidbar.sum(), 1))
        else:
            naeher, unterscheidbar = None, np.zeros(0, bool)
        erg[nm] = dict(px=int(msk.sum()), abweichung_spiel_zu_maler=mae(pm, msk), abweichung_spiel_zu_tiefenpuffer=mae(pp, msk),
                       unterscheidbare_px=int(unterscheidbar.sum()), anteil_spiel_naeher_an_maler=naeher)
    np.savez_compressed(AUS + '/spiel_%s.npz' % name, maler_i=R['maler_i'], puffer_i=R['puffer_i'], rgb_maler=R['rgb_maler'],
                        rgb_puffer=R['rgb_puffer'], tm=tm, maler_z=R['maler_z'], puffer_z=R['puffer_z'])
    return erg


if __name__ == '__main__':
    e = vergleich(*sys.argv[1:5])
    print(json.dumps(e, ensure_ascii=False, indent=1))
    json.dump(e, open(AUS + '/spielvergleich_%s.json' % sys.argv[1], 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
