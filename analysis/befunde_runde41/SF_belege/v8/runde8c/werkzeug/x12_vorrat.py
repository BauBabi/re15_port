"""Runde 8c: Vorrat heller / gedeckter Original-Texel (nur AUSWAHL, keine Farbverschiebung). Original-EM30 (Disc), Zeilen 480/481/482 mit ihrer
Original-Nutzung (adern_r4.nutzung). Lichtfaktor je Kanal = Spielbild-Median des heutigen Bauchs / Texel-Median des heutigen Fleischfelds (Messung
des Spiellichts, keine Eichung der Textur); Vorhersage je Kandidat = Texel-Median x Lichtfaktor."""
import sys, colorsys, json
import numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import kopf_orig_bau as KB, birkin_formen as BF, adern_r4 as A4, schwanz_r8 as Q
import v4_gemein as G
o = BF.lade_em30(); _d, _oo, _mso, timo = KB.teile(o); to = KB.tim_teile(timo)


def rgb_zeile(z):
    c = to['clut'][z - 480].astype(int)
    return np.stack([(c & 31) * 255 / 31., ((c >> 5) & 31) * 255 / 31., ((c >> 10) & 31) * 255 / 31.], -1)


def sv(rgb):
    h, s, v = colorsys.rgb_to_hsv(*(np.asarray(rgb, float) / 255.0)); return h * 360, s, v


# heutiges Fleischfeld (schwanz_r8.json felder.fleisch, Zeile 482 Seite 2)
F_ = Q.felder_laden()['fleisch']
rgb482 = rgb_zeile(482)
tx = []
for v in range(F_['v0'], F_['v1'] + 1):
    u0 = int(round(F_['u_v0'] + (F_['u_v1'] - F_['u_v0']) * (v - F_['v0']) / (F_['v1'] - F_['v0'])))
    for u in range(u0, u0 + F_['breite']):
        tx.append(rgb482[to['pix'][v, 256 + u]])
tx = np.array(tx)
med_t = np.median(tx, axis=0)
spiel = np.array([float(x) for x in sys.argv[1].split(',')])   # Spielbild-Median des Bauchs (x11_spiel_hsv)
faktor = spiel / med_t
print('heutiges Feld: Texel-Median RGB %s HSV %s; Spielbild %s -> Lichtfaktor %s' % (np.round(med_t, 1), np.round(sv(med_t), 2), spiel, np.round(faktor, 3)))
ziel = np.array([98.5, 58.5, 59.5])                               # Pruefer gruenblut: Video h22/h24/h64 RGB (89-108, 51-66, 52-67), Mitte
lab_ziel = G.lab(ziel[None, None])[0, 0]
print('Video-Mitte RGB %s HSV %s Lab %s' % (ziel, np.round(sv(ziel), 2), np.round(lab_ziel, 1)))
erg = []
for z in (480, 481, 482):
    rgb = rgb_zeile(z)
    nutz = A4.nutzung(None, z)
    for seite in range(3):
        nz = nutz[:, 128 * seite:128 * seite + 128]
        P = to['pix'][:, 128 * seite:128 * seite + 128]
        # alle Rechtecke 12x8 .. 32x24 (Schritt 2) voll in der Original-Nutzung
        for h in (8, 10, 12, 16, 20):
            for w in (12, 16, 20, 24, 32):
                for v0 in range(0, 256 - h + 1, 2):
                    for u0 in range(0, 128 - w + 1, 2):
                        if not nz[v0:v0 + h, u0:u0 + w].all():
                            continue
                        c = rgb[P[v0:v0 + h, u0:u0 + w]].reshape(-1, 3)
                        med = np.median(c, axis=0)
                        pr = np.clip(med * faktor, 0, 255)
                        lab = G.lab(pr[None, None])[0, 0]
                        dE = float(np.linalg.norm(lab - lab_ziel))
                        Lt = G.lab(c[None])[0][:, 0]
                        erg.append(dict(zeile=z, seite=seite, u=[u0, u0 + w - 1], v=[v0, v0 + h - 1], n=w * h, texel_med=[round(x, 1) for x in med],
                                        vorhersage=[round(x, 1) for x in pr], hsv=[round(x, 2) for x in sv(pr)], dE=round(dE, 2), L_streu=round(float(Lt.std()), 1)))
erg.sort(key=lambda e: (e['dE'], -e['n']))
# je Gegend nur das groesste beste Feld (nicht ueberlappend)
gew = []
for e in erg:
    if any(g_['zeile'] == e['zeile'] and g_['seite'] == e['seite'] and not (e['u'][1] < g_['u'][0] or e['u'][0] > g_['u'][1] or e['v'][1] < g_['v'][0] or e['v'][0] > g_['v'][1]) for g_ in gew):
        continue
    gew.append(e)
    if len(gew) >= 25:
        break
for e in gew:
    print(e)
json.dump(dict(faktor=list(faktor), ziel=list(ziel), kandidaten=gew), open(sys.argv[2], 'w'), indent=1)
