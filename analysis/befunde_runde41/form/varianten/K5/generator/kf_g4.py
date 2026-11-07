#!/usr/bin/env python3
"""Farbe Versuch 3 der Riesenform K5c: G4-Uebertragung (MESSBLATT_FARBE_V3.md, Bauer Runde 1).

Nutzer 2026-11-07: "Ich koennte mir vorstellen, das er so schwarz rot ist in seiner 4 beider Form, weil er verbrannt ist, wenn der generator
explodiert. ... das Gesicht vorne irgendwie in flammenfarben leuchtet. Bei der vorderansicht ja auch vielleicht hat er mit der Mutatione die
feuer Farbe beim Mund nach der explosion angenommen."

Was der Bau tut (Messblatt V3 2.2/2.3/3):
- FORM.EMD = K5c (varianten/K5/K5c/FORM.EMD, sha256 a71f8d08...): Ecken, Normalen, Flaechenindizes, Knochen, Clips unveraendert (Riegel G0).
  Neu sind nur je Flaeche u/v, CLUT-Wort und TPage (Messblatt V3 2.2 "Verfahren (b2)").
- FORM.TIM: Seiten 0-2 = G4-Atlas (RE2 CDEMD0.EMS Typ 0x34, TIM @0x65e800) bytegleich, CLUT-Zeilen 0-2 = G4-CLUT bytegleich.
  Seite 3 traegt nur die Sonderfelder (F1-Stelle, Maul-Band), zusammengesetzt aus G4-Indizes.
- Jede K5c-Flaeche bekommt die UV des G4-Dreiecks, das ihren Schwerpunkt in (theta, T) enthaelt (Zylinder um die Teil-Laengsachse,
  T entlang der Kette). Die affine Abbildung dieses Dreiecks gilt fuer alle Ecken (Pruefer-Verfahren stich.py). Liegt das UV-Dreieck
  nicht ganz in der Insel des Quellfelds, wird es um seinen Schwerpunkt verkleinert, bis es ganz drin liegt (Riegel S1).
- Ausgeschlossene G4-Felder (Messblatt 2.3 "nicht uebertragen", F3 Rippen hell, Augen F16, Fleisch F7/F8/F9) fehlen in der Quelle; die
  Winkelluecke, die sie lassen, wird je Quellmesh zusammengeschoben (theta-Verdichtung), damit keine Flaeche auf eine Lochkante faellt.
- CLUT-Zeilen (Messblatt 5 H2): 0-2 G4 bytegleich; 3-5 "Bild-1-Aderrot" = G4-Zeile 0-2, rote Eintraege im Farbton auf das Bild-1-Rot
  gedreht und in C* auf dessen Buntheit gesetzt, L* unveraendert (Rangfolge haelt); 6 = F1-Stelle, 7 = Maul-Band, 8 = Zaehne.

Aufruf: python kf_g4.py [--aus <Ordner>]   (Default: farbe/K5c_farbe_v3; dazu farbe/K5C_FARBE_V3.EMD/.TIM und Scratchpad formfarbe3/emd)
"""
import argparse
import copy
import hashlib
import json
import math
import os
import re
import shutil
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.normpath(os.path.join(HIER, '..'))
WT = os.path.normpath(os.path.join(K5, '..', '..', '..', '..', '..'))
TOOLS = os.path.join(WT, 're15_port', 'tools')
sys.path.insert(0, TOOLS)
import emd_ansichtsblatt as EA  # noqa: E402
import form_emd as FE  # noqa: E402

DATEN = os.path.join(HIER, 'v3_daten')
SCRATCH = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3'
K5C_EMD_SHA = 'a71f8d08e8d45b2f00346096dd8fbc7fa3a7dfc078c6e6bc9c80013750edbad2'   # Auftrag / Messblatt V3 Kopf

# ---------------------------------------------------------------- Konstanten (jede mit Quelle)
G4_KIND = 0x34                   # RE2 CDEMD0.EMS Typ 0x34 = G4 (Messblatt V3 1.1; FARBE_V3_LESER_A.md 1: EMD @0x677000, TIM @0x65e800)
CLUT_Y = 480                     # G4-TIM und K5c-TIM: CLUT bei y 480, x 0 (gemessen: FE.tim_lesen beider TIM, 2026-11-07)
Z_G4 = (0, 1, 2)                 # G4-Seite p nutzt CLUT-Zeile p (Leser A 1: 1427/1427 Flaechen; gemessen 0x7800/0x7840/0x7880 = Seite 0/1/2)
Z_B1 = (3, 4, 5)                 # Bild-1-Aderrot-Zeilen zu Seite 0/1/2 (Messblatt V3 5 H2 "eigene Zeilen fuer ... Bild-1-Aderrot")
Z_F1, Z_BAND, Z_ZAHN = 6, 7, 8   # Sonderfeld-Zeilen (Messblatt V3 5 H2)
KAPPE_NA = 0.8                   # Quelldreiecke mit |N.a| > 0,8 sind Kappen und fehlen in der Quelle (Leser A uebertrag.py koord, Messblatt 2.2)
VERSCHIEBE_N = 300               # PORT-WAHL Bauer: Kandidaten (naechste Inseltexel) fuer das Verschieben in voller Groesse
SCHRUMPF = (1.0, 0.85, 0.7, 0.55, 0.4, 0.25, 0.1, 0.0)   # PORT-WAHL Bauer: Stufen fuer das Einpassen in die Insel (S1: 0 fremde Texel)

# Bild-1-Aderrot (Messblatt V3 4, Tabelle "Koerper schwarz-rot"): Bild 1 Figur Rot Lab (39,1 / 21,7 / -1,2), G4-Atlas Rot Lab (38,0 / 35,5 / 20,8).
B1_ROT_LAB = (39.1, 21.7, -1.2)
G4_ROT_LAB = (38.0, 35.5, 20.8)
B1_TON = math.degrees(math.atan2(B1_ROT_LAB[2], B1_ROT_LAB[1]))           # -3,2 Grad
G4_TON = math.degrees(math.atan2(G4_ROT_LAB[2], G4_ROT_LAB[1]))           # +30,4 Grad
B1_C_FAKTOR = math.hypot(*B1_ROT_LAB[1:]) / math.hypot(*G4_ROT_LAB[1:])   # 21,7 / 41,1 = 0,53 (Versuch 3 Lauf 1, verworfen, s. u.)
# Buntheit je Seite (PORT-WAHL Bauer nach Messung, Dossier FARBE.md "Versuch 3", Laeufe 1 und 2):
#  Lauf 1 (alle Seiten x 0,53): K12 gruen, aber K1v2 Bild-1-Chroma 5,7 < 6,6, K2v2 5,7 < 6,8, K4b Ruecken-Rot 5,2 % < 7,1 % -> ROT.
#  Lauf 2 (alle Seiten x 1,0): K1v2/K2v2/K4b gruen, aber K12 Schwanz 17,1 % > 8,9 % -> ROT.
#  Seite 1 traegt Schwanz (F17/F19) und Vorderglied r: dort die Schwanz-Buntheit aus Messblatt V3 4 "1 Schwanz" Rot Lab (39,7 / 23,6 / -0,4):
#  C* 23,6 / 41,1 = 0,574. Seiten 0 und 2 (Becken, Lende, Hinterbeine) behalten die G4-Buntheit, nur der Farbton wird gedreht.
B1_C_JE_SEITE = (1.0, math.hypot(23.6, -0.4) / math.hypot(*G4_ROT_LAB[1:]), 1.0)
# Rot-Gewicht eines CLUT-Eintrags fuer die Bild-1-Zeilen: Rotklasse wie V1-A3 (a* > 15, C* > 20, Farbton -40..70 Grad, Leser A 1 "Farbklassen")
# als weiche Rampe: w = clip((C* - 10) / 10, 0, 1) im Farbtonfenster. PORT-WAHL Bauer: unter C* 10 (Familie dunkle Haut C* 2-7, Leser A F10-F13)
# bleibt der Eintrag unberuehrt, ab C* 20 (Grenze A3) voll gedreht.
ROT_C0, ROT_C1, ROT_H0, ROT_H1 = 10.0, 20.0, -40.0, 70.0

# Zielfarben der Sonderfelder (Messblatt V3 3.1, 3.2, 5 F1t/F2t; Vorwaerts-Gegenprobe gegenprobe.json)
ZIEL_F1 = (149, 103, 122)        # Lab (48,9 / 21,4 / -3,1), -8,3 Grad
ZIEL_BAND = (184, 90, 85)        # Lab (49,6 / 37,5 / 20,8), 29,0 Grad
# Bauziel Band (PORT-WAHL Bauer nach Messung): mit ZIEL_BAND selbst laeuft die Bild-2-Kette am Band zu rot (K15 Lab 47,7/38,7/21,1 gegen
# Rundlauf 49,9/28,2/16,1, dE 11,8; K7 a* 34,7 gegen 24,9). Das Bauziel nutzt die F2t-Toleranz (dE <= 6) zur Kette hin:
# Lab (51,5 / 33,1 / 19,2) = RGB (183, 99, 92), dE 5,1 zu ZIEL_BAND. Ergebnis K15 dE 8,9, K7 12,2 (weiter ROT, Dossier offen).
ZIEL_BAND_BAU = (183, 99, 92)
ZIEL_ZAHN = (199, 191, 183)      # Leser B (Lab 77,7 / 2,1 / 4,8), Messblatt 3.2 "Zaehne: L* >= 70, C* <= 12"

# K5c-Flaechen der Sonderfelder (Messblatt V3 1.2 / Leser B 1.3; Zaehlung je Mesh: tf, danach qf)
F1_FLAECHEN = {0: [('q', 7), ('q', 8), ('q', 19), ('q', 20), ('q', 31), ('q', 32)]}
K5C_ZAHNRECHT = (384, 180, 24, 14)   # K5c-Zahnfeld Seite 3 (Messblatt V3 2.3 "Zaehne (Seite 3, 384,180,24,14)")
K5C_AUGENRECHT = (384, 196, 24, 10)  # K5c-Augenfeld (p_riegel.atlas_zonen 'auge', k_form.py:11)

# G4-Felder (Leser A g4_felder.json / felder.py: Mesh -> Inseln)
FELD = {
    'F1': {26: [0, 102]}, 'F2': {26: [5]}, 'F3': {10: [200]}, 'F4': {10: [41]}, 'F5': {10: [90]}, 'F6': {10: [223]}, 'F7': {10: [84]},
    'F8': {10: [229]}, 'F9': {10: [130, 36]}, 'F10': {9: [0, 5, 9]}, 'F11': {0: [0, 3, 5, 20]}, 'F12': {1: [2], 5: [9, 10, 13]},
    'F13': {2: [0, 22]}, 'F14': {3: [1, 8, 10, 12]}, 'F15': {4: [2, 3, 7, 10, 15], 8: [3, 7, 10, 15]},
    'F16': {5: [0], 10: [47, 58], 16: [132]}, 'F17': {11: [7], 15: [0]}, 'F18': {16: [1, 12, 19, 72]},
    'F19': {12: [2, 14, 15], 17: [0, 14, 15]}, 'F20': {13: [14, 16]}, 'F21': {14: [7, 10, 15, 16], 19: [2, 7, 10, 15], 13: [1]},
}

# Teile-Zuordnung (Messblatt V3 2.3, verbindlich). (Name, K5c-Meshes, G4-Meshes, Art, Optionen)
#  art 'kette': durchgehendes T ueber die Meshes (E9), je Mesh eigener Rahmen; 'gruppe': ein Rahmen fuer alle (Kopf); 'einzeln': je Ziel-Mesh dieselbe Quelle.
#  nur = erlaubte G4-Felder (sonst alle Inseln der Quellmeshes ausser 'ohne'); zeilen = 'g4' oder 'b1' (Bild-1-Aderrot, Messblatt V3 4 "Bild-1-sichtbare Teile").
OHNE_BRUST = ['F3', 'F7', 'F8', 'F9', 'F16']      # Messblatt 2.3 Rumpf: "ohne F3 (Rippen, hell), F7, F8, F9, F16"
ZUORDNUNG = [
    ('Rumpf', [0, 7, 10], [0, 9, 10], 'kette', dict(achse=(1, 0, 0), ohne=OHNE_BRUST, zeilen={0: 'b1', 7: 'b1', 10: 'g4'})),
    ('Schwanz', [1, 2, 3, 4, 5, 6], [11, 12], 'kette', dict(zeilen='b1')),
    ('Hals', [21], [10], 'kette', dict(achse=(1, 0, 0), nur=['F4', 'F5'], zeilen='g4')),
    ('Hinterbein l (W) Oberschenkel', [8], [5], 'kette', dict(ohne=['F16'], ohne_inseln={5: [13]}, zeilen='b1')),
    ('Hinterbein l (W) Unterschenkel', [24], [6], 'kette', dict(zeilen='b1')),
    ('Hinterbein l (W) Fuss', [25], [7], 'kette', dict(zeilen='b1')),
    ('Hinterbein r (W) Oberschenkel', [9], [1], 'kette', dict(zeilen='b1')),
    ('Hinterbein r (W) Unterschenkel', [26], [2], 'kette', dict(zeilen='b1')),
    ('Hinterbein r (W) Fuss', [27], [3], 'kette', dict(zeilen='b1')),
    ('Vorderbein l (W) Oberarm', [11], [11], 'kette', dict(zeilen='g4')),
    ('Vorderbein l (W) Unterarm', [12], [12], 'kette', dict(zeilen='g4')),
    ('Vorderbein l (W) Hand', [13], [13], 'kette', dict(nur=['F20'], zeilen='g4')),
    ('Klauen l (W)', [14, 15, 28, 30], [14], 'einzeln', dict(zeilen='g4')),
    ('Vorderbein r (W) Oberarm', [16], [15, 16], 'gruppe16', dict(ohne=['F16'], zeilen='b1')),
    ('Vorderbein r (W) Unterarm', [17], [17], 'kette', dict(zeilen='b1')),
    ('Vorderbein r (W) Hand', [18], [18], 'kette', dict(nur=['F20'], zeilen='b1')),
    ('Klauen r (W)', [19, 20, 29, 31], [19], 'einzeln', dict(zeilen='g4')),
    # Kopf aussen aus F18 (Messblatt V3 2.3 "dunkle Hautfelder F18 (Oberarm r: schwarz mit Laengsadern) oder F4/F5"). Gewaehlt nach Messung
    # (Dossier FARBE.md Versuch 3, Lauf 2/3): mit F4/F5/F6 K13 Bild-2-Dunkel 31,2 % < 46,4 %, K6 dE 9,1; mit F18 46,9 % und dE 4,3.
    # Ohne das rosa Wundoval (G4 16 Insel 72, L* 21,4, 38 % rot, gemessen t7.py): es laege sonst am rechten Auge (K10 L* 35,1 > 30).
    ('Kopf + Kiefer aussen', [22, 23], [16], 'kopf', dict(achse=(1, 0, 0), nur=['F18'], ohne_inseln={16: [72]}, zeilen='g4')),
]
TEILE_H1 = {   # Messblatt 5 H1 "je Teil (Rumpf, Schwanz, Hals, Beine, Arme, Klauen, Kopf/Kiefer aussen)"
    'Rumpf': [0, 7, 10], 'Schwanz': [1, 2, 3, 4, 5, 6], 'Hals': [21], 'Beine': [8, 24, 25, 9, 26, 27],
    'Arme': [11, 12, 13, 16, 17, 18], 'Klauen': [14, 15, 28, 30, 19, 20, 29, 31], 'Kopf/Kiefer aussen': [22, 23],
}


# ================================================================ Laden
def g4_roh():
    """(emd, tim, Quelle) aus RE2 CDEMD0.EMS ueber die TOC des Ports (wie Leser A lade.re2)."""
    inc = open(os.path.join(WT, 're15_port', 'engine', 'src', 'gen', 're2_ems_toc.inc'), encoding='utf-8', errors='replace').read()
    toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S).group(1))]
    ems = open(os.path.join(WT, 're15_port', 'shared_assets', 'RE2', 'CDEMD0.EMS'), 'rb').read()

    def rec(r):
        i = ((G4_KIND - 0x10) * 4 + r) * 2
        return ems[toc[i] * 0x800:toc[i] * 0x800 + toc[i + 1]], toc[i] * 0x800
    e, eo = rec(3)
    t, to = rec(2)
    return e, t, dict(ems='re15_port/shared_assets/RE2/CDEMD0.EMS', kind=hex(G4_KIND), emd_off=hex(eo), tim_off=hex(to))


def g4_modell():
    import birkin_bahnsteig as BB
    e, t, q = g4_roh()
    BB.KIND = G4_KIND
    return BB.modell(e, t), t, q


def k5c_bytes():
    emd = open(os.path.join(K5, 'K5c', 'FORM.EMD'), 'rb').read()
    tim = open(os.path.join(K5, 'K5c', 'FORM.TIM'), 'rb').read()
    sha = hashlib.sha256(emd).hexdigest()
    if sha != K5C_EMD_SHA:
        raise SystemExit('K5c/FORM.EMD sha256 %s, erwartet %s' % (sha, K5C_EMD_SHA))
    return emd, tim


def lab(rgb):
    a = np.asarray(rgb, np.float32).reshape(-1, 3) / 255.0
    return cv2.cvtColor(a[None], cv2.COLOR_RGB2LAB)[0].astype(np.float64)


def rgb_aus_lab(L):
    """Lab -> RGB 0..255. Ausserhalb des sRGB-Raums wird die Buntheit bei festem L* und Farbton verkleinert (statt Kanal-Kappen, das L* verschiebt)."""
    L = np.asarray(L, np.float64).reshape(-1, 3)
    out = np.zeros((len(L), 3), int)
    for i, (l, a, b) in enumerate(L):
        lo, hi = 0.0, 1.0
        x = cv2.cvtColor(np.array([[[l, a, b]]], np.float32), cv2.COLOR_LAB2RGB)[0, 0]
        if x.min() < -1e-4 or x.max() > 1 + 1e-4:
            for _ in range(20):
                k = (lo + hi) / 2
                x = cv2.cvtColor(np.array([[[l, a * k, b * k]]], np.float32), cv2.COLOR_LAB2RGB)[0, 0]
                if x.min() < -1e-4 or x.max() > 1 + 1e-4:
                    hi = k
                else:
                    lo = k
            x = cv2.cvtColor(np.array([[[l, a * lo, b * lo]]], np.float32), cv2.COLOR_LAB2RGB)[0, 0]
        out[i] = np.clip(np.round(x * 255), 0, 255).astype(int)
    return out


def rang_reparatur(neu, basis, eintraege, soll_lab=None):
    """Messblatt V3 H2: L*-Rangfolge der genutzten Eintraege wie im G4-Eintrag (keine Vertauschung bei dL >= 2). Die 15-Bit-Rundung kann
    knappe Paare vertauschen; dann wird der Eintrag in der 5-Bit-Nachbarschaft (+-2 je Kanal) auf die naechste Farbe gesetzt, die die Rangfolge
    haelt. Ein Durchgang aufsteigend nach G4-L*."""
    neu = list(neu)
    e = [i for i in eintraege if basis[i] != 0]
    Lb = {i: float(lab(wort_rgb([basis[i]]))[0, 0]) for i in e}
    ordn = sorted(e, key=lambda i: Lb[i])
    Ln = {i: float(lab(wort_rgb([neu[i]]))[0, 0]) for i in e}
    geaendert = 0
    for i in ordn:
        unten = [Ln[j] for j in ordn if Lb[j] <= Lb[i] - 2]
        oben = [Ln[j] for j in ordn if Lb[j] >= Lb[i] + 2]
        lo = max(unten) if unten else -1
        hi = min(oben) if oben else 101
        if lo <= Ln[i] <= hi:
            continue
        w = neu[i]
        r0, g0, b0 = w & 31, (w >> 5) & 31, (w >> 10) & 31
        ziel = lab(wort_rgb([w]))[0] if soll_lab is None else soll_lab[i]
        best = None
        for dr in range(-2, 3):
            for dg in range(-2, 3):
                for db in range(-2, 3):
                    r, g, b = r0 + dr, g0 + dg, b0 + db
                    if min(r, g, b) < 0 or max(r, g, b) > 31:
                        continue
                    ww = r | (g << 5) | (b << 10)
                    if ww == 0:
                        continue
                    X = lab(wort_rgb([ww]))[0]
                    if lo <= X[0] <= hi:
                        d = float(np.linalg.norm(X - ziel))
                        if best is None or d < best[0]:
                            best = (d, ww, X[0])
        if best is not None:
            neu[i] = best[1] | (w & 0x8000)
            Ln[i] = best[2]
            geaendert += 1
    return neu, geaendert


def wort_rgb(w):
    """PSX-15-Bit -> RGB 0..255 (wie EA.tim_decode: c5 * 255 / 31)."""
    w = np.asarray(w, np.int64)
    r, g, b = w & 31, (w >> 5) & 31, (w >> 10) & 31
    return np.stack([r, g, b], -1) * 255.0 / 31.0


# ================================================================ Flaechen, Inseln (wie Leser A kern3.py, gleiche Inselnummern)
def flaechen(mo, mi):
    """Dreiecke eines Meshes: (V lokal 3x3, UV Atlas 3x2 mit Seitenversatz, CLUT-Wort, (vier, fi))."""
    m = mo.meshes[mi]
    aus = []
    for (verts, faces, uvs, vier) in ((m.tv, m.tf, m.tuv, 0), (m.qv, m.qf, m.quv, 1)):
        if not faces:
            continue
        V = np.array([x[:3] for x in verts], float)
        for fi, (g, uv) in enumerate(zip(faces, uvs)):
            ie = g[1::2]
            po = (uv[5] & 0xF) * 128
            if vier:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                for a, b, c in ((0, 1, 3), (0, 3, 2)):
                    aus.append((V[[ie[a], ie[b], ie[c]]], np.array([tx[a], tx[b], tx[c]], float), uv[2], (vier, fi)))
            else:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                aus.append((V[list(ie[:3])], np.array(tx, float), uv[2], (vier, fi)))
    return aus


def insel_je_flaeche(fl):
    par = list(range(len(fl)))

    def f(i):
        while par[i] != i:
            par[i] = par[par[i]]
            i = par[i]
        return i
    ecke = {}
    for k, (V, T, cw, q) in enumerate(fl):
        for p in T:
            key = (int(p[0]), int(p[1]))
            if key in ecke:
                par[f(k)] = f(ecke[key])
            else:
                ecke[key] = k
    return np.array([f(k) for k in range(len(fl))])


def texel_im_dreieck(T, tol=1e-6):
    """Texel (x, y), deren Mitte im geschlossenen UV-Dreieck liegt (wie p_lade.sammeln / Leser A texel_punkte)."""
    u, v = T[:, 0], T[:, 1]
    A2 = (u[1] - u[0]) * (v[2] - v[0]) - (u[2] - u[0]) * (v[1] - v[0])
    if abs(A2) < 1e-9:
        return np.zeros((0, 2), int)
    x0, x1 = int(np.floor(u.min())), int(np.ceil(u.max()))
    y0, y1 = int(np.floor(v.min())), int(np.ceil(v.max()))
    X, Y = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
    w1 = ((X - u[0]) * (v[2] - v[0]) - (u[2] - u[0]) * (Y - v[0])) / A2
    w2 = ((u[1] - u[0]) * (Y - v[0]) - (X - u[0]) * (v[1] - v[0])) / A2
    w0 = 1 - w1 - w2
    inn = (w0 >= -tol) & (w1 >= -tol) & (w2 >= -tol)
    return np.stack([np.floor(X[inn]).astype(int), np.floor(Y[inn]).astype(int)], 1)


class G4Quelle:
    """G4-Dreiecke je Mesh mit Insel, Feld, Texelmasken je Insel und je Feld."""

    def __init__(self, mo):
        self.mo = mo
        self.fl = {mi: flaechen(mo, mi) for mi in range(len(mo.meshes))}
        self.ins = {mi: insel_je_flaeche(self.fl[mi]) if self.fl[mi] else np.zeros(0, int) for mi in self.fl}
        self.feld_von = {}
        for fn, spec in FELD.items():
            for me, isl in spec.items():
                for i in isl:
                    self.feld_von[(me, i)] = fn
        H, W = mo.tex.shape
        self.H, self.W = H, W
        self.inselmaske = {}          # (mesh, insel) -> bool HxW
        self.global_maske = np.zeros((H, W), bool)
        self.texel_flaeche = np.zeros((H, W), float)   # 3D-Flaeche je Texel (fuer D1)
        for mi, fl in self.fl.items():
            for k, (V, T, cw, q) in enumerate(fl):
                xy = texel_im_dreieck(T)
                if not len(xy):
                    continue
                key = (mi, int(self.ins[mi][k]))
                mk = self.inselmaske.setdefault(key, np.zeros((H, W), bool))
                xy = xy[(xy[:, 0] >= 0) & (xy[:, 0] < W) & (xy[:, 1] >= 0) & (xy[:, 1] < H)]
                mk[xy[:, 1], xy[:, 0]] = True
                self.global_maske[xy[:, 1], xy[:, 0]] = True
                a3 = 0.5 * np.linalg.norm(np.cross(V[1] - V[0], V[2] - V[0]))
                if len(xy):
                    self.texel_flaeche[xy[:, 1], xy[:, 0]] = np.maximum(self.texel_flaeche[xy[:, 1], xy[:, 0]], a3 / len(xy))

        # Inseln anderer Meshes, die dieselben Texel nutzen (G4 6 = 2, 7 = 3, 18 = 13, ...), bekommen das Feld mit der groessten Ueberdeckung
        fm = {fn: self.feldmaske(fn) for fn in FELD}
        for key, mk in self.inselmaske.items():
            if key in self.feld_von:
                continue
            n = mk.sum()
            best = max(fm, key=lambda fn: (fm[fn] & mk).sum())
            if n and (fm[best] & mk).sum() >= 0.5 * n:
                self.feld_von[key] = best

    def feld(self, mi, insel):
        return self.feld_von.get((mi, int(insel)))

    def feldmaske(self, name):
        m = np.zeros((self.H, self.W), bool)
        for me, isl in FELD[name].items():
            for i in isl:
                if (me, i) in self.inselmaske:
                    m |= self.inselmaske[(me, i)]
        return m


# ================================================================ Parameter (theta, T): ein Rahmen je Teil (Gruppe der Meshes in Ruhelage)
# Grundlage Leser A uebertrag.py (rahmen, mitte_t, koord). Geaendert nach Messung t6.py (2026-11-07): Die Hauptachse je Mesh kippt bei breiten
# Teilen zur Seite (K5c-Brust 10: PCA-Achse (0,04/0,02/-1,00), G4-Brust (0,92/0,32/-0,22)). Darum je Teil EIN Rahmen ueber alle seine Meshes
# (Versatz aus dem Gerippe in Ruhelage, ohne Bindedrehung, Leser A 2), Rumpf/Hals/Kopf mit fester Achse x (vorn, beide Modelle),
# und die G4-Winkelreferenz wird aus der K5c-Referenz durch die kleinste Drehung Achse G4 -> Achse K5c gewonnen (gleiche Seiten in beiden).
UP = np.array([0., -1., 0.])
VOR = np.array([1., 0., 0.])
T_BINS = 12                      # wie Leser A mitte_t (nb = 12)
T_FEIN = 48                      # PORT-WAHL Bauer: Aufloesung der T-Verdichtung (4 je Mitte-Abschnitt)


def ruhe_lage(mo):
    """Knochenursprung in Ruhelage: Summe der Versaetze entlang der Elternkette (EMR rel, keine Bindedrehung)."""
    n = mo.skel.bone_count
    pos = [None] * n
    for b in range(n):
        p = mo.skel.parent[b]
        r = np.array(mo.skel.rel[b], float)
        pos[b] = r if (p < 0 or p >= b) else pos[p] + r
    return pos


def mesh_ecken(mo, mi):
    m = mo.meshes[mi]
    return np.array([v[:3] for v in m.tv + m.qv], float)


def drehung(a, b):
    """Kleinste Drehung, die Einheitsvektor a auf b dreht (Rodrigues)."""
    a = a / np.linalg.norm(a)
    b = b / np.linalg.norm(b)
    v = np.cross(a, b)
    c = float(a @ b)
    if np.linalg.norm(v) < 1e-9:
        if c > 0:
            return np.eye(3)
        o = np.array([1., 0, 0]) if abs(a[0]) < 0.9 else np.array([0, 1., 0])
        v = np.cross(a, o)
        v /= np.linalg.norm(v)
        return 2 * np.outer(v, v) - np.eye(3)
    vx = np.array([[0, -v[2], v[1]], [v[2], 0, -v[0]], [-v[1], v[0], 0]])
    return np.eye(3) + vx + vx @ vx * (1 / (1 + c))


class Rahmen:
    """Zylinderrahmen eines Teils: Achse a (distal positiv), Referenz r (theta = 0), b = a x r; T = 0..1 entlang a; Mitte je T-Abschnitt."""

    def __init__(self, mo, meshes, achse=None, ref=None, offs=None):
        lage = ruhe_lage(mo)
        o0 = lage[meshes[0]]
        self.offs = {mi: (np.asarray(offs[k], float) if offs is not None else lage[mi] - o0) for k, mi in enumerate(meshes)}
        V = np.vstack([mesh_ecken(mo, mi) + self.offs[mi] for mi in meshes])
        if achse is None:
            c = V.mean(0)
            _w, E = np.linalg.eigh(np.cov((V - c).T))
            a = E[:, -1]
            if (V.mean(0) @ a) < 0:
                a = -a                                   # distal positiv (Wurzel am Ursprung)
        else:
            a = np.asarray(achse, float)
            a = a / np.linalg.norm(a)
        if ref is None:
            r = UP if abs(a @ UP) < 0.7 else VOR
        else:
            r = np.asarray(ref, float)
        r = r - (r @ a) * a
        r /= np.linalg.norm(r)
        self.a, self.r, self.b = a, r, np.cross(a, r)
        s = V @ a
        self.s0, self.s1 = float(s.min()), float(s.max())
        self.L = self.s1 - self.s0
        t = (s - self.s0) / max(self.L, 1e-9)
        q = V - np.outer(s, a)
        kn = np.linspace(0, 1, T_BINS + 1)
        mt = []
        for i in range(T_BINS):
            sel = (t >= kn[i] - 0.05) & (t <= kn[i + 1] + 0.05)
            mt.append(q[sel].mean(0) if sel.any() else q.mean(0))
        self.tc, self.M = (kn[:-1] + kn[1:]) / 2, np.array(mt)
        self.rad = float(np.median(self._rad(V)))

    def _rad(self, P):
        T, th, rad = self._p(P)
        return rad

    def _p(self, P):
        s = P @ self.a
        t = (s - self.s0) / max(self.L, 1e-9)
        cq = np.stack([np.interp(np.clip(t, 0, 1), self.tc, self.M[:, k]) for k in range(3)], 1)
        q = P - np.outer(s, self.a) - cq
        return np.clip(t, 0, 1), np.arctan2(q @ self.b, q @ self.r), np.linalg.norm(q, axis=1)

    def p(self, mi, P):
        T, th, _r = self._p(np.asarray(P, float) + self.offs[mi])
        return T, th

    def achse(self, mi):
        return self.a


def rahmen_paar(k5, zm, g4, qm, achse=None, offs_z=None, offs_q=None):
    """K5c-Rahmen und G4-Rahmen mit gleicher Winkelreferenz (G4-Referenz = K5c-Referenz, zurueckgedreht auf die G4-Achse)."""
    Rz = Rahmen(k5, zm, achse=achse, offs=offs_z)
    Rq0 = Rahmen(g4, qm, achse=achse, offs=offs_q)
    D = drehung(Rq0.a, Rz.a)
    Rq = Rahmen(g4, qm, achse=Rq0.a, ref=D.T @ Rz.r, offs=offs_q)
    return Rz, Rq


# ================================================================ Projektion (b2)
class Theta:
    """theta-Verdichtung je T-Abschnitt: Zielwinkel gleichmaessig auf die Winkel, die erlaubte Quelldreiecke decken (Luecken = ausgeschlossene
    Felder werden uebersprungen, die Naht liegt dort, wo die Luecke war)."""

    def __init__(self, P):
        # T-Verdichtung: T-Abschnitte ohne erlaubte Quelle (z. B. Brust vorn = F7/F8 ausgeschlossen) werden uebersprungen
        tb = np.zeros(T_FEIN, bool)
        for tri in P:
            tb[int(np.clip(tri[:, 1].min() * T_FEIN, 0, T_FEIN - 1)):int(np.clip(tri[:, 1].max() * T_FEIN, 0, T_FEIN - 1)) + 1] = True
        self.t_ged = np.nonzero(tb)[0]
        self.b = np.zeros((T_BINS, 360), bool)
        for tri in P:                                   # tri: 3 x (theta unwrapped, T)
            t0, t1 = tri[:, 1].min(), tri[:, 1].max()
            h0, h1 = np.degrees(tri[:, 0].min()), np.degrees(tri[:, 0].max())
            i0, i1 = int(np.clip(t0 * T_BINS, 0, T_BINS - 1)), int(np.clip(t1 * T_BINS, 0, T_BINS - 1))
            hs = (np.arange(int(np.floor(h0)), int(np.ceil(h1)) + 1) + 180) % 360
            self.b[i0:i1 + 1, hs] = True
        # leere T-Abschnitte erben vom naechsten gefuellten
        voll = np.nonzero(self.b.any(1))[0]
        for i in range(T_BINS):
            if not self.b[i].any() and len(voll):
                self.b[i] = self.b[voll[np.argmin(np.abs(voll - i))]]

    def t_abbilden(self, T):
        T = np.asarray(T, float)
        if len(self.t_ged) == T_FEIN or not len(self.t_ged):
            return T
        f = np.clip(T, 0, 1) * len(self.t_ged)
        k = np.clip(np.floor(f).astype(int), 0, len(self.t_ged) - 1)
        return (self.t_ged[k] + (f - k)) / T_FEIN

    def abbilden(self, T, th):
        T = np.asarray(T, float)
        out = np.asarray(th, float).copy()
        for j in range(len(T)):
            b = self.b[int(np.clip(T[j] * T_BINS, 0, T_BINS - 1))]
            if b.all() or not b.any():
                continue
            ged = np.nonzero(b)[0]
            f = ((np.degrees(out[j]) + 180.0) % 360.0) / 360.0 * len(ged)
            k = int(np.clip(np.floor(f), 0, len(ged) - 1))
            out[j] = np.radians(ged[k] + (f - k) - 180.0)
        return out


def quell_dreiecke(G, Rq, meshes, erlaubt_fn):
    """G4-Dreiecke in (theta, T) mit UV, Seite, Insel; Kappen und nicht erlaubte Inseln fehlen."""
    P, UV, S, I = [], [], [], []
    for mi in meshes:
        fl = G.fl[mi]
        isl = G.ins[mi]
        for k, (V, T, cw, q) in enumerate(fl):
            if not erlaubt_fn(mi, isl[k]):
                continue
            n = np.cross(V[1] - V[0], V[2] - V[0])
            nn = np.linalg.norm(n)
            if nn < 1e-9 or abs(n @ Rq.a) / nn > KAPPE_NA:
                continue
            t, th = Rq.p(mi, V)
            th = th.copy()
            for i in (1, 2):
                th[i] = th[0] + (th[i] - th[0] + math.pi) % (2 * math.pi) - math.pi
            P.append(np.stack([th, t], 1))
            UV.append(T)
            S.append(int(min(T[:, 0]) // 128))
            I.append((mi, int(isl[k])))
    return np.array(P), np.array(UV), np.array(S), I


def affin(pq, tq, p):
    """UV der Punkte p (n x 2) unter der affinen Abbildung Quelldreieck (theta,T) -> UV."""
    A = np.array([pq[1] - pq[0], pq[2] - pq[0]]).T
    l = np.linalg.pinv(A) @ (p - pq[0]).T
    return tq[0] + np.outer(l[0], tq[1] - tq[0]) + np.outer(l[1], tq[2] - tq[0])


def einpassen(uv, maske, seite, rand=None, quote=0.01):
    """UV-Dreieck/-Viereck (Atlas, float) so in die Inselmaske legen, dass seine Texel darin liegen. -> (uv int, schrumpf, fremd).
    rand = Inselmaske plus 1 Texel G4-Malrand (gemessen: 80 % der Texel im 1-Texel-Ring um die G4-Inseln sind bemalt, L* 18 gegen 21,5 innen;
    t2.py 2026-11-07). Angenommen wird ein Dreieck, dessen Texel alle in rand liegen und hoechstens quote (1 %, Riegel S1) ausserhalb der Maske.
    Reihenfolge: an Ort und Stelle (Groesse 1,0 / 0,85 / 0,7 / 0,55), dann in voller Groesse verschoben auf die naechsten Inseltexel
    (bis VERSCHIEBE_N Kandidaten), erst danach kleiner (Schrumpf bis 0). Ein verschobenes Dreieck behaelt den Texelmassstab; ein auf einen
    Punkt geschrumpftes waere eine einfarbige Flaeche (Versuch 3 Lauf 2: flache Bloecke an Schulter und Hals)."""
    H, W = maske.shape
    if rand is None:
        rand = maske
    c0 = uv.mean(0)

    def probe(c, f):
        q = np.round(c + (uv - c0) * f).astype(int)
        q[:, 0] = np.clip(q[:, 0], seite * 128, seite * 128 + 255)
        q[:, 1] = np.clip(q[:, 1], 0, 255)
        xy = texel_set(q)
        if len(xy) == 0:
            return q, None
        inn = (xy[:, 0] >= 0) & (xy[:, 0] < W) & (xy[:, 1] >= 0) & (xy[:, 1] < H)
        if not inn.all() or not rand[xy[:, 1], xy[:, 0]].all():
            return q, None
        fremd = int((~maske[xy[:, 1], xy[:, 0]]).sum())
        return q, (fremd if fremd <= int(quote * len(xy)) else None)
    q, fr = probe(c0, 1.0)
    if fr is not None:
        return q, 1.0, fr
    if len(texel_set(np.round(uv).astype(int))) == 0:
        q = np.round(uv).astype(int)
        q[:, 0] = np.clip(q[:, 0], seite * 128, seite * 128 + 255)
        q[:, 1] = np.clip(q[:, 1], 0, 255)
        return q, -1.0, 0                                   # Linie (Kappe zwischen Ringen), deckt kein Texel: bleibt, wie sie ist
    ci = (int(np.clip(np.floor(c0[0]), 0, W - 1)), int(np.clip(np.floor(c0[1]), 0, H - 1)))
    if maske[ci[1], ci[0]]:
        for f in SCHRUMPF[1:4]:
            q, fr = probe(c0, f)
            if fr is not None:
                return q, f, fr
    ys, xs = np.nonzero(maske)
    d = (xs + 0.5 - c0[0]) ** 2 + (ys + 0.5 - c0[1]) ** 2
    ordn = np.argsort(d)[:VERSCHIEBE_N]
    for f in SCHRUMPF[:4]:
        for j in ordn:
            q, fr = probe(np.array([xs[j] + 0.5, ys[j] + 0.5]), f)
            if fr is not None:
                return q, (0.999 if f == 1.0 else f), fr
    cn = np.array([xs[ordn[0]] + 0.5, ys[ordn[0]] + 0.5])
    for f in SCHRUMPF[4:]:
        q, fr = probe(cn, f)
        if fr is not None:
            return q, f, fr
    q = np.round(np.repeat(cn[None], len(uv), 0)).astype(int)
    return q, 0.0, 0


def texel_set(q):
    """Texel eines UV-Dreiecks oder -Vierecks (Viereck = Dreiecke 0,1,3 und 0,3,2 in PSX-Reihenfolge 0,1,2,3 -> a,b,d / a,d,c)."""
    q = np.asarray(q, float)
    if len(q) == 3:
        return texel_im_dreieck(q)
    a = texel_im_dreieck(q[[0, 1, 3]])
    b = texel_im_dreieck(q[[0, 3, 2]])
    x = np.vstack([a, b])
    return np.unique(x, axis=0) if len(x) else x


def projektion(k5, G, zm, qm, art, op, k5skel_rel=None, auswahl=None):
    """-> ({(mesh, 't'|'q', fi): (UV-Ecken Atlas int, Seite, (G4-Mesh, Insel), Schrumpf)}, Statistik, Quellmaske).
    auswahl: Menge (mesh, teil, fi), die diese Zuordnung bekommt (None = alle Flaechen der Ziel-Meshes)."""
    ohne = set(op.get('ohne', []))
    nur = op.get('nur')
    ohne_inseln = op.get('ohne_inseln', {})

    def erlaubt(mi, insel):
        fn = G.feld(mi, insel)
        if int(insel) in ohne_inseln.get(mi, []):
            return False
        if nur is not None:
            return fn in nur
        return fn not in ohne

    Rz, Rq = rahmen_paar(k5, zm, G.mo, qm, achse=op.get('achse'))
    P, UV, S, I = quell_dreiecke(G, Rq, qm, erlaubt)
    TH = Theta(P)
    tq = np.concatenate([p[:, 1] for p in P])
    tmin, tmax = (float(np.percentile(tq, 1)), float(np.percentile(tq, 99))) if (nur is not None) else (0.0, 1.0)
    erl_maske = np.zeros((G.H, G.W), bool)
    for key in set(I):
        erl_maske |= G.inselmaske[key]
    erl_rand = cv2.dilate(erl_maske.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool) & \
        (cv2.dilate(G.global_maske.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0)
    k = Rq.rad / max(Rq.L, 1e-9)
    Pc = P.mean(1)
    A_ = P[:, 1] - P[:, 0]
    B_ = P[:, 2] - P[:, 0]
    det = A_[:, 0] * B_[:, 1] - A_[:, 1] * B_[:, 0]
    ok = np.abs(det) > 1e-12
    dd = np.where(ok, det, 1.0)
    neu = {}
    st = dict(flaechen=0, im_dreieck=0, naechstes=0, geschrumpft=0, kollabiert=0, fremd_texel=0, linie=0)
    for mi in zm:
        m = k5.meshes[mi]
        for teil, verts, faces in (('t', m.tv, m.tf), ('q', m.qv, m.qf)):
            V = np.array([x[:3] for x in verts], float)
            for fi, g in enumerate(faces):
                if auswahl is not None and (mi, teil, fi) not in auswahl:
                    continue
                ie = list(g[1::2])
                T, th = Rz.p(mi, V[ie])
                T = TH.t_abbilden(tmin + T * (tmax - tmin))
                th = TH.abbilden(T, th)
                for i in range(1, len(ie)):
                    th[i] = th[0] + (th[i] - th[0] + math.pi) % (2 * math.pi) - math.pi
                p = np.stack([th, T], 1)
                cen = p.mean(0)
                best, bw = None, 0.0
                for sh in (0.0, 2 * math.pi, -2 * math.pi):
                    Cc = cen + np.array([sh, 0.0]) - P[:, 0]
                    l1 = np.where(ok, (Cc[:, 0] * B_[:, 1] - Cc[:, 1] * B_[:, 0]) / dd, -9)
                    l2 = np.where(ok, (A_[:, 0] * Cc[:, 1] - A_[:, 1] * Cc[:, 0]) / dd, -9)
                    inn = ok & (l1 >= -1e-6) & (l2 >= -1e-6) & (l1 + l2 <= 1 + 1e-6)
                    if inn.any():
                        best, bw = int(np.nonzero(inn)[0][0]), sh
                        break
                st['flaechen'] += 1
                if best is None:
                    dth = (Pc[:, 0] - cen[0] + math.pi) % (2 * math.pi) - math.pi
                    d = (k * dth) ** 2 + (Pc[:, 1] - cen[1]) ** 2
                    best = int(np.argmin(d))
                    bw = float(np.round((Pc[best, 0] - cen[0] - dth[best]) / (2 * math.pi)) * 2 * math.pi)
                    st['naechstes'] += 1
                else:
                    st['im_dreieck'] += 1
                uv = affin(P[best], UV[best], p + np.array([bw, 0.0]))
                seite = int(S[best])
                q, f, fremd = einpassen(uv, erl_maske, seite, erl_rand)
                if f == -1.0:
                    st['linie'] += 1
                elif f < 1.0:
                    st['geschrumpft'] += 1
                if f == 0.0:
                    st['kollabiert'] += 1
                st['fremd_texel'] += fremd
                neu[(mi, teil, fi)] = (q, seite, I[best], f)
    return neu, st, erl_maske


# ================================================================ CLUT-Zeilen
def g4_cluts(g4tim):
    t = FE.tim_lesen(g4tim)
    if t['clut_x'] != 0 or t['clut_y'] != CLUT_Y or len(t['cluts']) != 3:
        raise SystemExit('G4-TIM unerwartet: clut %d,%d, %d Zeilen' % (t['clut_x'], t['clut_y'], len(t['cluts'])))
    return t['pixel'], [list(z) for z in t['cluts']]


def wort(rgb):
    return FE.clut_wort(*[int(x) for x in rgb])


def zeile_b1(zeile, c_faktor=B1_C_FAKTOR):
    """Bild-1-Aderrot: rote Eintraege im Farbton von G4_TON auf B1_TON gedreht, C* mal B1_C_FAKTOR, L* unveraendert.
    Gewicht w (Rampe ROT_C0..ROT_C1 im Farbtonfenster) mischt zwischen G4-Eintrag und gedrehtem Eintrag. 0x0000 (transparent) bleibt."""
    neu = list(zeile)
    rgb = wort_rgb(zeile)
    L = lab(rgb)
    C = np.hypot(L[:, 1], L[:, 2])
    h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    w = np.clip((C - ROT_C0) / (ROT_C1 - ROT_C0), 0, 1) * ((h > ROT_H0) & (h < ROT_H1))
    dh = math.radians(B1_TON - G4_TON)
    liste = []
    for i in range(256):
        if zeile[i] == 0 or w[i] <= 0:
            continue
        ww = float(w[i])
        cc = C[i] * (1 + ww * (c_faktor - 1))
        hh = math.radians(h[i]) + ww * dh
        Ln = np.array([[L[i, 0], cc * math.cos(hh), cc * math.sin(hh)]])
        r = rgb_aus_lab(Ln)[0]
        neu[i] = wort(r) | (zeile[i] & 0x8000)
        liste.append(i)
    neu, _n = rang_reparatur(neu, zeile, [i for i in range(256) if zeile[i] != 0])
    return neu, liste


def zeile_ziel(zeile, eintraege, gewichte, ziel_rgb, iterationen=8, fest=()):
    """Eigene Sonderfeld-Zeile: die genutzten Eintraege (Gewicht = Texelzahl) so umfaerben, dass das Feldmittel (Lab, texelgewichtet) das Ziel
    trifft. L* ueber eine Potenz (monoton, Rangfolge haelt), a*b* gedreht und skaliert (Farbton und Buntheit des Mittels). Iteriert gegen die
    15-Bit-Rundung. fest = weitere genutzte Eintraege der Zeile, die ihre G4-Farbe behalten (Haut im F1-Feld): dann bleibt L* bis zum hellsten
    festen Eintrag Lk gleich und wird erst darueber linear gestreckt (L' = Lk + s (L - Lk)), damit die Rangfolge aller genutzten Eintraege haelt (H2)."""
    ziel = lab(np.array([ziel_rgb], float))[0]
    rgb = wort_rgb(zeile)
    L0 = lab(rgb)
    e = np.array(sorted(eintraege))
    w = np.array([gewichte[i] for i in e], float)
    w = w / w.sum()
    gam, rot, k = 1.0, 0.0, 1.0
    fest = [i for i in fest if zeile[i] != 0]
    Lk = float(max(L0[i, 0] for i in fest)) if fest else None
    zl = list(zeile)
    for _ in range(iterationen):
        Ls = L0[e].copy()
        if Lk is None:
            Lp = 100.0 * (np.clip(Ls[:, 0], 0, 100) / 100.0) ** gam
        else:
            Lp = np.where(Ls[:, 0] <= Lk, Ls[:, 0], Lk + gam * (Ls[:, 0] - Lk))
        ab = Ls[:, 1:]
        ca, sa = math.cos(rot), math.sin(rot)
        ab2 = np.stack([ca * ab[:, 0] - sa * ab[:, 1], sa * ab[:, 0] + ca * ab[:, 1]], 1) * k
        X = np.column_stack([Lp, ab2])
        r = rgb_aus_lab(X)
        for j, i in enumerate(e):
            zl[i] = wort(r[j])
        ist = (lab(wort_rgb([zl[i] for i in e])) * w[:, None]).sum(0)
        # Nachfuehren: L ueber gam, ab ueber Drehung/Skalierung des Mittels
        if Lk is not None:
            ob = (Ls[:, 0] > Lk)
            wo = w[ob].sum()
            if wo > 0:
                mo = (w[ob] * (Ls[ob, 0] - Lk)).sum()
                gam = max(0.05, (ziel[0] - (w[~ob] * Ls[~ob, 0]).sum() - wo * Lk) / max(mo, 1e-9))
        elif ist[0] > 0.5 and ziel[0] > 0.5:
            gam *= math.log(max(ziel[0], 1) / 100.0) / math.log(max(ist[0], 1) / 100.0)
        h_ist, h_z = math.atan2(ist[2], ist[1]), math.atan2(ziel[2], ziel[1])
        rot += h_z - h_ist
        c_ist, c_z = math.hypot(ist[1], ist[2]), math.hypot(ziel[1], ziel[2])
        if c_ist > 0.3:
            k *= c_z / c_ist
    zl, rep = rang_reparatur(zl, zeile, list(e) + list(fest))
    ist = (lab(wort_rgb([zl[i] for i in e])) * w[:, None]).sum(0)
    return zl, dict(rang_repariert=rep, L_fest_bis=None if Lk is None else round(Lk, 2), gamma=round(gam, 4), drehung_grad=round(math.degrees(rot), 2), c_faktor=round(k, 4),
                    feldmittel_lab=[round(float(x), 2) for x in ist], ziel_lab=[round(float(x), 2) for x in ziel],
                    dE=round(float(np.linalg.norm(ist - ziel)), 2))


# ================================================================ Messdaten der Sonderfelder (v3_daten, aus kf_g4_daten.py)
def daten():
    d = json.load(open(os.path.join(DATEN, 'sonderfelder.json'), encoding='utf-8'))
    z = np.load(os.path.join(DATEN, 'lokpunkte.npz'))
    return d, z


def schluessel(k5, mi, nr):
    """Flaechen-Nr. (tf, danach qf; Leser B flaechen_id.py) -> (mesh, teil, fi)."""
    nt = len(k5.meshes[mi].tf)
    return (mi, 't', nr) if nr < nt else (mi, 'q', nr - nt)


def alle_flaechen(k5, meshes=None):
    out = []
    for mi, m in enumerate(k5.meshes):
        if meshes is not None and mi not in meshes:
            continue
        out += [(mi, 't', i) for i in range(len(m.tf))] + [(mi, 'q', i) for i in range(len(m.qf))]
    return out


def flaeche_info(k5, key):
    """(Ecken lokal n x 3, mittlere Normale lokal, K5c-UV-Schwerpunkt Atlas)."""
    mi, teil, fi = key
    m = k5.meshes[mi]
    if teil == 't':
        g, uv, V, N = m.tf[fi], m.tuv[fi], m.tv, m.tn
        tx = [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])]
    else:
        g, uv, V, N = m.qf[fi], m.quv[fi], m.qv, m.qn
        tx = [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7]), (uv[9], uv[10])]
    po = (uv[5] & 0xF) * 128
    P = np.array([V[i][:3] for i in g[1::2]], float)
    n = np.array([N[i][:3] for i in g[0::2]], float).mean(0)
    n = n / max(np.linalg.norm(n), 1e-9)
    c = np.array(tx, float).mean(0) + [po, 0]
    return P, n, c


def in_recht(c, r):
    return r[0] <= c[0] < r[0] + r[2] and r[1] <= c[1] < r[1] + r[3]


def klassen(k5, D):
    """Sonderfeld-Flaechen: F1, Band, Zaehne, Augen, Maulinneres; Rest = Zuordnung."""
    F1 = {(mi, t, i) for mi, l in F1_FLAECHEN.items() for t, i in l}
    zahn, auge, innen = set(), set(), set()
    for key in alle_flaechen(k5, [22, 23]):
        P, n, c = flaeche_info(k5, key)
        if in_recht(c, K5C_ZAHNRECHT):
            zahn.add(key)
        elif in_recht(c, K5C_AUGENRECHT):
            auge.add(key)
    band = set()
    for b, nr, anteil in D['band_flaechen']:
        key = schluessel(k5, b, nr)
        if anteil >= D['band_schwelle'] and key not in zahn and key not in auge:
            band.add(key)
    for key in alle_flaechen(k5, [22, 23]):
        if key in zahn or key in auge or key in band:
            continue
        P, n, c = flaeche_info(k5, key)
        # Leser A voll.py: Kieferboden (Mesh 23, Normale nach oben N.y < -0,3) und Gaumen (Mesh 22, Normale nach unten N.y > 0,6) -> Maulinneres F2
        if (key[0] == 23 and n[1] < -0.3) or (key[0] == 22 and n[1] > 0.6):
            innen.add(key)
    return dict(F1=F1, band=band, zahn=zahn, auge=auge, innen=innen)


# ================================================================ Sonderfelder auf Seite 3
SEITE3_X = 384
SLOT_F1 = (SEITE3_X + 2, 2)          # PORT-WAHL Bauer: Lage der Sonderfelder auf der freien Seite 3 (2 Texel Abstand zum Rand)
SLOT_BAND = (SEITE3_X + 2, 100)


def dichte_g4(G, meshes):
    """sqrt(Texel / 3D-Flaeche) eines G4-Meshes (wie Leser A dichte_q)."""
    A, n = 0.0, set()
    for mi in meshes:
        for V, T, cw, q in G.fl[mi]:
            A += 0.5 * np.linalg.norm(np.cross(V[1] - V[0], V[2] - V[0]))
            for x, y in texel_im_dreieck(T):
                n.add((x, y))
    return math.sqrt(len(n) / A)


def ebene(P):
    c = P.mean(0)
    w, E = np.linalg.eigh(np.cov((P - c).T))
    return c, E[:, 2], E[:, 1]


def kopie_gefuellt(px, maske, x0, y0, w, h):
    """Indizes eines Feldausschnitts; Texel ausserhalb der Feldmaske (Loecher) bekommen den Index des naechsten Feldtexels."""
    a = px[y0:y0 + h, x0:x0 + w].copy()
    m = maske[y0:y0 + h, x0:x0 + w]
    if (~m).any():
        _d, lab_ = cv2.distanceTransformWithLabels((~m).astype(np.uint8), cv2.DIST_L2, 5, labelType=cv2.DIST_LABEL_PIXEL)
        ys, xs = np.nonzero(m)
        lut = np.zeros(lab_.max() + 1, int)
        lut[lab_[ys, xs]] = np.arange(len(ys))
        j = lut[lab_]
        a = a[ys[j], xs[j]].reshape(a.shape) if a.ndim == 2 else a
    return a


def groesstes_rechteck_in(maske, w, h):
    """Linke obere Ecke eines w x h-Rechtecks ganz in der Maske (erstes gefundenes, nach Abstand zur Maskenmitte)."""
    ys, xs = np.nonzero(maske)
    cy, cx = ys.mean(), xs.mean()
    ii = cv2.integral(maske.astype(np.uint8))
    best = None
    for y in range(maske.shape[0] - h + 1):
        for x in range(maske.shape[1] - w + 1):
            s = ii[y + h, x + w] - ii[y, x + w] - ii[y + h, x] + ii[y, x]
            if s == w * h:
                d = (x + w / 2 - cx) ** 2 + (y + h / 2 - cy) ** 2
                if best is None or d < best[0]:
                    best = (d, x, y)
    return None if best is None else (best[1], best[2])


# ================================================================ Bau
FLECK_WEITE = 1  # PORT-WAHL Bauer nach Messung: Fleck = Maskenpunkte F1 (Bild 1) + 1 Texel. Ohne Erweiterung K14-Rotklasse 51,9 % < 58,8 %
                 # (der 1-Texel-Saum WEICH_RAND frisst den Rand), mit Erweiterung 74,5 % (Dossier FARBE.md Versuch 3, Lauf 1 -> 2)
WEICH_RAND = 1   # PORT-WAHL Bauer (Auftrag "weiche Uebergaenge in PSX-Machart, keine harten Kleckse"): im aeussersten Texelring des F1-Flecks
                 # bleibt nur die hellere Haelfte der Fleisch-Texel stehen (helle Strahlen laufen in die Haut aus), sonst Haut


def zaehler():
    import collections
    return collections.Counter()


def kopf_uv(k5, key):
    """K5c-UV-Ecken einer Flaeche im Atlas (mit Seitenversatz)."""
    mi, teil, fi = key
    m = k5.meshes[mi]
    uv = m.tuv[fi] if teil == 't' else m.quv[fi]
    po = (uv[5] & 0xF) * 128
    tx = [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])] + ([(uv[9], uv[10])] if teil == 'q' else [])
    return np.array(tx, float) + [po, 0]


def bau(aus_dir):
    g4, g4tim, g4q = g4_modell()
    emd, tim = k5c_bytes()
    k5 = FE.modell(emd, tim)
    G = G4Quelle(g4)
    D, Z = daten()
    px_g4, cl_g4 = g4_cluts(g4tim)
    H, W = 256, 512
    px = np.zeros((H, W), np.uint8)
    px[:, :384] = px_g4                                         # Seiten 0-2 bytegleich (Messblatt V3 2.2 b2)
    cl = [list(cl_g4[0]), list(cl_g4[1]), list(cl_g4[2])]       # Zeilen 0-2 bytegleich
    b1 = [zeile_b1(cl_g4[p], B1_C_JE_SEITE[p]) for p in range(3)]
    cl += [b1[p][0] for p in range(3)]                          # Zeilen 3-5 Bild-1-Aderrot
    cl += [None, None, None]                                    # Zeilen 6-8 Sonderfelder (unten)
    bericht = dict(g4=g4q, k5c_emd_sha256=K5C_EMD_SHA, zeilen={}, teile={}, sonder={})
    bericht['zeilen']['b1_umgefaerbt'] = {str(3 + p): len(b1[p][1]) for p in range(3)}
    K = klassen(k5, D)
    belegt = set().union(*K.values())
    uv = {}

    # ---- Zuordnung Messblatt 2.3 (b2)
    for nm, zm, qm, art, op in ZUORDNUNG:
        aw = {k for k in alle_flaechen(k5, zm) if k not in belegt}
        st_sum = {}
        if art == 'einzeln':
            for z in zm:
                neu, st, _m = projektion(k5, G, [z], qm, 'kette', op, k5.skel.rel, {k for k in aw if k[0] == z})
                for k, v in neu.items():
                    uv[k] = (v[0], v[1], v[2], v[3], nm, op)
                for a_, b_ in st.items():
                    st_sum[a_] = st_sum.get(a_, 0) + b_
        else:
            neu, st, _m = projektion(k5, G, zm, qm, art, op, k5.skel.rel, aw)
            for k, v in neu.items():
                uv[k] = (v[0], v[1], v[2], v[3], nm, op)
            st_sum = st
        bericht['teile'][nm] = dict(k5c=zm, g4=qm, art=art, **st_sum)

    # ---- Maulinneres F2 (Leser A voll.py: Kieferboden/Gaumen -> F2), Projektion im Kopfrahmen auf G4-Kopf 26
    neu, st, _m = projektion(k5, G, [22, 23], [26], 'kopf', dict(achse=(1, 0, 0), nur=['F2']), k5.skel.rel, K['innen'])
    for k, v in neu.items():
        uv[k] = (v[0], v[1], v[2], v[3], 'Maulinneres F2', dict(zeilen='g4'))
    bericht['teile']['Maulinneres F2'] = dict(k5c=[22, 23], g4=[26], art='kopf', **st)

    # ---- Zeilenfamilie je Flaeche
    fertig = {}
    for k, (q, seite, herk, f, nm, op) in uv.items():
        z = op.get('zeilen', 'g4')
        if isinstance(z, dict):
            z = z.get(k[0], 'g4')
        zeile = (Z_B1 if z == 'b1' else Z_G4)[seite]
        fertig[k] = (q, seite, zeile, nm)

    # ---- Augen: K5c-Augenfeld (24 x 10) -> dunkles Rechteck in G4-F10 (Lende, Lab 5,0/1,7/-2,7, Leser A 1), Zeile der Lende (Bild-1-Familie)
    m10 = G.feldmaske('F10')
    ecke = groesstes_rechteck_in(m10, K5C_AUGENRECHT[2], K5C_AUGENRECHT[3])
    for k in K['auge']:
        q = kopf_uv(k5, k) - np.array(K5C_AUGENRECHT[:2]) + np.array(ecke)
        fertig[k] = (np.round(q).astype(int), 0, Z_B1[0], 'Augen F10')
    bericht['sonder']['Augen'] = dict(flaechen=len(K['auge']), ziel_rechteck=[int(ecke[0]), int(ecke[1]), K5C_AUGENRECHT[2], K5C_AUGENRECHT[3]])

    # ---- Zaehne: K5c-Zahnfeld (24 x 14) -> G4-F8 (Stachelkranz, oben hellgelb-weiss, Leser A 1), quer gestellt, Zeile 8
    m8 = G.feldmaske('F8')
    r8 = cv2.dilate(m8.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    ys, xs = np.nonzero(m8)
    x8, y8, w8 = xs.min(), ys.min(), xs.max() - xs.min() + 1
    zahn_texel = zaehler()
    for k in K['zahn']:
        r = kopf_uv(k5, k) - np.array(K5C_ZAHNRECHT[:2])         # 0..24 x 0..14
        q = np.stack([x8 + r[:, 1] * (w8 - 1) / K5C_ZAHNRECHT[3], y8 + r[:, 0]], 1)
        q, f, fremd = einpassen(q, m8, 0, r8)
        fertig[k] = (q, 0, Z_ZAHN, 'Zaehne F8')
        for x, y in texel_set(q):
            zahn_texel[int(px[y, x])] += 1

    # ---- F1-Stelle: Becken-Vierecke (Messblatt 1.2) -> eigenes Feld auf Seite 3: Fleisch aus G4-F16 (Strahlen ohne Iris), Haut aus G4-F11
    #      (Becken, gespiegelt), beide Seite-2-Indizes; Zeile 6 = G4-Zeile 2 mit umgefaerbten Fleischeintraegen (Ziel F1t)
    rho_b = dichte_g4(G, [0])                                   # G4-Becken: Texel je Einheit (gemessen, t4.py: 0,0339)
    Pall = np.vstack([flaeche_info(k5, k)[0] for k in K['F1']])
    c0, e1, e2 = ebene(Pall)
    st_ = np.stack([(Pall - c0) @ e1, (Pall - c0) @ e2], 1) * rho_b
    lo = st_.min(0)
    wf, hf = int(np.ceil(st_[:, 0].max() - lo[0])) + 3, int(np.ceil(st_[:, 1].max() - lo[1])) + 3
    ox, oy = SLOT_F1

    def f1_uv(P):
        s = np.stack([(P - c0) @ e1, (P - c0) @ e2], 1) * rho_b - lo + 1
        return s + np.array([ox, oy])
    for k in K['F1']:
        fertig[k] = (np.round(f1_uv(flaeche_info(k5, k)[0])).astype(int), 3, Z_F1, 'F1-Stelle')
    lp = f1_uv(Z['F1_lok'].astype(float))
    fleck = np.zeros((H, W), bool)
    fleck[np.floor(lp[:, 1]).astype(int), np.floor(lp[:, 0]).astype(int)] = True
    fleck = cv2.morphologyEx(fleck.astype(np.uint8), cv2.MORPH_CLOSE, np.ones((3, 3), np.uint8)).astype(bool)
    for _ in range(FLECK_WEITE):
        fleck = cv2.dilate(fleck.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    feld = np.zeros((H, W), bool)
    feld[oy:oy + hf, ox:ox + wf] = True
    fleck &= feld
    m11 = G.feldmaske('F11')
    ys11, xs11 = np.nonzero(m11)
    e11 = (int(xs11.min()), int(ys11.min()))
    hw, hh = int(xs11.max() - e11[0] + 1), int(ys11.max() - e11[1] + 1)
    haut = kopie_gefuellt(px_g4, m11, e11[0], e11[1], hw, hh)[:, ::-1]     # ganzes F11-Feld, Loecher gefuellt, gespiegelt
    haut = np.tile(haut, (int(np.ceil(hf / hh)), int(np.ceil(wf / hw))))[:hf, :wf]
    px[oy:oy + hf, ox:ox + wf] = haut
    m16 = G.feldmaske('F16')
    rgb16 = wort_rgb(np.array(cl_g4[2])[px_g4])
    L16 = lab(rgb16.reshape(-1, 3)).reshape(H, 384, 3)
    h16 = np.degrees(np.arctan2(L16[..., 2], L16[..., 1]))
    iris = m16 & (h16 > 60) & (h16 < 100) & (L16[..., 0] > 30)          # Leser A 1 "gelb-olive Iris"
    iris = cv2.dilate(iris.astype(np.uint8), np.ones((9, 9), np.uint8)).astype(bool)
    ys_f, xs_f = np.nonzero(fleck)
    fw, fh = int(xs_f.max() - xs_f.min() + 1), int(ys_f.max() - ys_f.min() + 1)
    e16 = groesstes_rechteck_in(m16 & ~iris, fw, fh)
    if e16 is None:
        raise SystemExit('F16 ohne Iris: kein %dx%d-Rechteck' % (fw, fh))
    fl_idx = px_g4[e16[1]:e16[1] + fh, e16[0]:e16[0] + fw]
    fl_L = L16[e16[1]:e16[1] + fh, e16[0]:e16[0] + fw, 0]
    dist = cv2.distanceTransform(fleck.astype(np.uint8), cv2.DIST_L1, 3)
    med = float(np.median(fl_L))
    haut_eintraege = set(np.unique(px[feld & ~fleck]).tolist())
    fleisch_texel = zaehler()
    for y, x in zip(ys_f, xs_f):
        j, i = y - ys_f.min(), x - xs_f.min()
        if dist[y, x] <= WEICH_RAND and fl_L[j, i] < med:
            continue
        px[y, x] = fl_idx[j, i]
        fleisch_texel[int(fl_idx[j, i])] += 1
    eintr = [i for i in fleisch_texel if i not in haut_eintraege and cl_g4[2][i] != 0]
    z6, info6 = zeile_ziel(cl_g4[2], eintr, fleisch_texel, ZIEL_F1, fest=sorted(haut_eintraege))
    cl[Z_F1] = z6
    bericht['sonder']['F1'] = dict(flaechen=sorted([list(k) for k in K['F1']]), dichte=round(rho_b, 4), feld=[ox, oy, wf, hf], fleck_texel=int(fleck.sum()),
                                   fleisch_texel=int(sum(fleisch_texel.values())), quelle_fleisch_F16=[int(e16[0]), int(e16[1]), fw, fh],
                                   quelle_haut_F11=[int(e11[0]), int(e11[1]), hw, hh], umgefaerbt=len(eintr), zeile=info6)

    # ---- Maul-Band: Bandflaechen (Bild 2) -> eigenes Feld auf Seite 3: G4-F7 (strahlenfoermiges Fleisch, 175 Farben) mit gefuellten Loechern;
    #      Zeile 7 = G4-Zeile 0 mit umgefaerbten F7-Eintraegen (Ziel F2t)
    m7 = G.feldmaske('F7')
    ys7, xs7 = np.nonzero(m7)
    x7, y7 = int(xs7.min()), int(ys7.min())
    w7, h7 = int(xs7.max() - x7 + 1), int(ys7.max() - y7 + 1)
    bx, by = SLOT_BAND
    px[by:by + h7, bx:bx + w7] = kopie_gefuellt(px_g4, m7, x7, y7, w7, h7)
    bandfeld = np.zeros((H, W), bool)
    bandfeld[by:by + h7, bx:bx + w7] = True
    Kk = Rahmen(k5, [22, 23], achse=(1, 0, 0))
    rho_r = dichte_g4(G, [10])                                  # G4-Brust (Herkunft F7): Texel je Einheit (gemessen, t4.py: 0,0290)
    pts = {k: Kk.p(k[0], flaeche_info(k5, k)[0]) for k in K['band']}
    th_all = np.concatenate([v[1] for v in pts.values()])
    th0 = math.atan2(np.sin(th_all).mean(), np.cos(th_all).mean())

    def band_st(T, th, s):
        d = (th - th0 + math.pi) % (2 * math.pi) - math.pi
        return np.stack([d * Kk.rad * s, T * Kk.L * s], 1)
    allst = np.vstack([band_st(T, th, rho_r) for T, th in pts.values()])
    ext = allst.max(0) - allst.min(0)
    s_band = rho_r * min(1.0, (w7 - 3) / max(ext[0], 1e-9), (h7 - 3) / max(ext[1], 1e-9))
    allst = np.vstack([band_st(T, th, s_band) for T, th in pts.values()])
    lo_b = allst.min(0)
    band_texel = zaehler()
    for k, (T, th) in pts.items():
        q = band_st(T, th, s_band) - lo_b + 1 + np.array([bx, by])
        q, f, fremd = einpassen(q, bandfeld, 3)
        fertig[k] = (q, 3, Z_BAND, 'Maul-Band')
        for x, y in texel_set(q):
            band_texel[int(px[y, x])] += 1
    eintr7 = [i for i in band_texel if cl_g4[0][i] != 0]
    z7, info7 = zeile_ziel(cl_g4[0], eintr7, band_texel, ZIEL_BAND_BAU)
    cl[Z_BAND] = z7
    bericht['sonder']['Band'] = dict(flaechen=len(K['band']), dichte_soll=round(rho_r, 4), dichte=round(s_band, 4), feld=[bx, by, w7, h7],
                                     quelle_F7=[x7, y7, w7, h7], umgefaerbt=len(eintr7), zeile=info7)
    eintr8 = [i for i in zahn_texel if cl_g4[0][i] != 0]
    z8, info8 = zeile_ziel(cl_g4[0], eintr8, zahn_texel, ZIEL_ZAHN)
    cl[Z_ZAHN] = z8
    bericht['sonder']['Zaehne'] = dict(flaechen=len(K['zahn']), umgefaerbt=len(eintr8), zeile=info8)

    # ---- Vollstaendigkeit
    fehlt = [k for k in alle_flaechen(k5) if k not in fertig]
    if fehlt:
        raise SystemExit('%d Flaechen ohne UV, z. B. %s' % (len(fehlt), fehlt[:5]))

    # ---- EMD (nur uv/clut/page) und TIM schreiben
    form = FE.lese_emd(emd)
    for (mi, teil, fi), (q, seite, zeile, nm) in fertig.items():
        f = form['meshes'][mi]['dreiecke' if teil == 't' else 'vierecke'][fi]
        u = [int(x) - 128 * seite for x in q[:, 0]]
        v = [int(y) for y in q[:, 1]]
        if min(u) < 0 or max(u) > 255 or min(v) < 0 or max(v) > 255:
            raise SystemExit('UV ausserhalb u8: %s %s %s' % ((mi, teil, fi), u, v))
        f['uv'] = [[a_, b_] for a_, b_ in zip(u, v)]
        f['clut'] = ((CLUT_Y + zeile) << 6) | 0
        f['page'] = (int(f['page']) & ~0xF) | seite
    emd_neu = FE.schreibe_emd(form, pruefen=True)
    tim_neu = FE.tim_schreiben(px, cl, clut_x=0, clut_y=CLUT_Y)
    os.makedirs(aus_dir, exist_ok=True)
    open(os.path.join(aus_dir, 'FORM.EMD'), 'wb').write(emd_neu)
    open(os.path.join(aus_dir, 'FORM.TIM'), 'wb').write(tim_neu)
    fj = json.load(open(os.path.join(K5, 'K5c', 'form.json'), encoding='utf-8'))
    fj['name'] = 'K5c_farbe_v3'
    json.dump(fj, open(os.path.join(aus_dir, 'form.json'), 'w', encoding='utf-8'), indent=1)
    bericht['flaechen_herkunft'] = {'%d|%s|%d' % k: [nm, int(zeile)] for k, (q, s, zeile, nm) in fertig.items()}
    bericht['zeilen']['anzahl'] = len(cl)
    bericht['sha256'] = dict(emd=hashlib.sha256(emd_neu).hexdigest(), tim=hashlib.sha256(tim_neu).hexdigest())
    bericht['basis'] = 'K5c (varianten/K5/K5c): Geometrie, Knochen, Clips unveraendert; UV/CLUT/TPage je Flaeche und FORM.TIM neu (Versuch 3, kf_g4.py)'
    bericht['konstanten'] = dict(G4_KIND=hex(G4_KIND), B1_ROT_LAB=B1_ROT_LAB, G4_ROT_LAB=G4_ROT_LAB, B1_TON=round(B1_TON, 2), G4_TON=round(G4_TON, 2),
                                 B1_C_JE_SEITE=[round(x, 4) for x in B1_C_JE_SEITE], ROT_RAMPE=(ROT_C0, ROT_C1, ROT_H0, ROT_H1), ZIEL_F1=ZIEL_F1, ZIEL_BAND=ZIEL_BAND,
                                 ZIEL_ZAHN=ZIEL_ZAHN, ZIEL_BAND_BAU=ZIEL_BAND_BAU, FLECK_WEITE=FLECK_WEITE, KAPPE_NA=KAPPE_NA, SCHRUMPF=SCHRUMPF, WEICH_RAND=WEICH_RAND, BAND_SCHWELLE=D['band_schwelle'])
    json.dump(bericht, open(os.path.join(aus_dir, 'bau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False, default=str)
    return bericht


def main():
    a = argparse.ArgumentParser()
    a.add_argument('--aus', default=os.path.join(K5, 'farbe', 'K5c_farbe_v3'))
    x = a.parse_args()
    b = bau(x.aus)
    shutil.copyfile(os.path.join(x.aus, 'FORM.EMD'), os.path.join(K5, 'farbe', 'K5C_FARBE_V3.EMD'))
    shutil.copyfile(os.path.join(x.aus, 'FORM.TIM'), os.path.join(K5, 'farbe', 'K5C_FARBE_V3.TIM'))
    os.makedirs(os.path.join(SCRATCH, 'emd'), exist_ok=True)
    for f in ('FORM.EMD', 'FORM.TIM', 'form.json', 'bau.json'):
        shutil.copyfile(os.path.join(x.aus, f), os.path.join(SCRATCH, 'emd', f))
    for nm, t in b['teile'].items():
        print('%-32s %s' % (nm, {k: v for k, v in t.items() if k not in ('k5c', 'g4', 'art')}))
    for nm, t in b['sonder'].items():
        print('%-12s %s' % (nm, t))
    print('sha256', b['sha256'])


if __name__ == '__main__':
    main()
