#!/usr/bin/env python3
"""Kameras der beiden Formvorlagen (Bahnsteig-Form, Runde 41).

    python re15_port/tools/form_kamera.py                # Selbsttest beider Kameras

Kette Welt -> Bild (640x480, Pixelmitte wie in OpenCV, y nach unten):

    v = R * (welt - auge)                                Blickraum, +z nach vorn, Bild-y = Welt-y nach unten
    u = 160 + H * v.x / v.z,  w = 120 + H * v.y / v.z    Port-Bild 320x240 (GTE RTPS, OFX/OFY @0x8001643c/@0x80016440, H = fov>>7)
    (x, y, 1) ~ B * (u, w, 1)                            Video-Bild 640x480, Homographie B aus kamera_bildN.json

R ist das LookAt des Spiels ohne Rollen (camera_common.c, FUN_80053ca4). Die Datei enthaelt zusaetzlich die fertige 3x4-Matrix P.
Bodenebene ist y = 0; Y zeigt nach unten, Hoehen sind negativ.

Bild 1 hat keine Kamera auf der Disc: dort sind Auge, Ziel und B geschaetzt, siehe analysis/befunde_runde41/form/kamera.md.
"""
import json
import os

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
ORDNER = os.path.normpath(os.path.join(HIER, '..', '..', 'analysis', 'befunde_runde41', 'form'))


def blickmatrix(auge, ziel):
    """R wie re15_camera_build_view (FUN_80053ca4), in Gleitkomma: Zeilen = rechts, unten, vorn."""
    auge, ziel = np.asarray(auge, float), np.asarray(ziel, float)
    d = ziel - auge
    dist, horiz = np.linalg.norm(d), np.hypot(d[0], d[2])
    sp, cp = -d[1] / dist, horiz / dist
    sy, cy = d[0] / horiz, d[2] / horiz
    return np.array([[cy, 0.0, -sy],
                     [sp * sy, cp, sp * cy],
                     [cp * sy, -sp, cp * cy]])


def baue_p(auge, ziel, h, b):
    """3x4-Matrix Welt -> Video aus Auge, Ziel, Blickdistanz h und Homographie b."""
    r = blickmatrix(auge, ziel)
    k = np.array([[h, 0.0, 160.0], [0.0, h, 120.0], [0.0, 0.0, 1.0]])
    return np.asarray(b, float) @ k @ np.c_[r, -r @ np.asarray(auge, float)]


def lade_kamera(kamera):
    """kamera = 1, 2 oder Pfad zur JSON-Datei. Liefert ein dict; 'P', 'B', 'R' sind numpy-Arrays."""
    pfad = os.path.join(ORDNER, 'kamera_bild%d.json' % kamera) if isinstance(kamera, int) else kamera
    with open(pfad, encoding='utf-8') as f:
        k = json.load(f)
    k['B'] = np.array(k['abbildung_port_nach_bild']['B'], float)
    k['R'] = blickmatrix(k['auge'], k['ziel'])
    k['P'] = baue_p(k['auge'], k['ziel'], k['H'], k['B'])
    return k


def _nx3(welt):
    w = np.asarray(welt, float)
    return w.reshape(1, 3) if w.ndim == 1 else w


def projiziere(kam, welt):
    """Weltpunkte (.., 3) -> Bildpunkte (.., 2) in 640x480. Punkte hinter dem Auge ergeben nan."""
    w = np.asarray(welt, float)
    x = _nx3(w)
    h = (kam['P'] @ np.c_[x, np.ones(len(x))].T).T
    tiefe = (x - np.asarray(kam['auge'], float)) @ kam['R'][2]
    with np.errstate(divide='ignore', invalid='ignore'):
        b = h[:, :2] / h[:, 2:3]
    b[tiefe <= 1.0] = np.nan
    return b[0] if w.ndim == 1 else b


def tiefe(kam, welt):
    """Abstand entlang der Blickachse."""
    w = np.asarray(welt, float)
    d = (_nx3(w) - np.asarray(kam['auge'], float)) @ kam['R'][2]
    return float(d[0]) if w.ndim == 1 else d


def strahl(kam, bild):
    """Bildpunkte -> (auge, Einheitsrichtung je Punkt)."""
    b = np.asarray(bild, float)
    x = b.reshape(1, 2) if b.ndim == 1 else b
    m = kam['P'][:, :3]
    d = (np.linalg.inv(m) @ np.c_[x, np.ones(len(x))].T).T
    d /= np.linalg.norm(d, axis=1, keepdims=True)
    d[(d @ kam['R'][2]) < 0] *= -1.0
    return np.asarray(kam['auge'], float), (d[0] if b.ndim == 1 else d)


def strahl_auf_boden(kam, bild, y=0.0):
    """Bildpunkte -> Weltpunkte auf der Ebene Y = y. Strahlen, die die Ebene nicht vor dem Auge treffen, ergeben nan."""
    b = np.asarray(bild, float)
    auge, d = strahl(kam, b)
    d2 = d.reshape(1, 3) if d.ndim == 1 else d
    with np.errstate(divide='ignore', invalid='ignore'):
        s = (y - auge[1]) / d2[:, 1]
    p = auge + s[:, None] * d2
    p[~(s > 0)] = np.nan
    return p[0] if b.ndim == 1 else p


def port_nach_bild(kam, port):
    """Port-Pixel (320x240) -> Video-Pixel (640x480)."""
    p = np.asarray(port, float)
    x = p.reshape(1, 2) if p.ndim == 1 else p
    h = (kam['B'] @ np.c_[x, np.ones(len(x))].T).T
    h = h[:, :2] / h[:, 2:3]
    return h[0] if p.ndim == 1 else h


def bild_nach_port(kam, bild):
    """Video-Pixel (640x480) -> Port-Pixel (320x240)."""
    p = np.asarray(bild, float)
    x = p.reshape(1, 2) if p.ndim == 1 else p
    h = (np.linalg.inv(kam['B']) @ np.c_[x, np.ones(len(x))].T).T
    h = h[:, :2] / h[:, 2:3]
    return h[0] if p.ndim == 1 else h


def _selbsttest():
    for n in (1, 2):
        pfad = os.path.join(ORDNER, 'kamera_bild%d.json' % n)
        if not os.path.exists(pfad):
            print('Bild %d: keine Kameradatei' % n)
            continue
        k = lade_kamera(n)
        welt = np.array([[0.0, 0.0, 0.0]]) + np.asarray(k['ziel'], float) * [1, 0, 1]
        b = projiziere(k, welt)
        zurueck = strahl_auf_boden(k, b)
        print('Bild %d: Ziel am Boden %s -> Bild %s -> Boden %s, Abweichung %.3f'
              % (n, np.round(welt[0]).tolist(), np.round(b[0], 1).tolist(), np.round(zurueck[0]).tolist(),
                 np.linalg.norm(welt - zurueck)))


if __name__ == '__main__':
    _selbsttest()
