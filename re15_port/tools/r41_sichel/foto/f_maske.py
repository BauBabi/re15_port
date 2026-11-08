"""SF Messblatt Fotovorlage, Schritt 1: Umriss der Figur in Foto 1 (n1485, entblendet) und Foto 2 (n1435) als TRIMAP.

Die Figur hebt sich im dunklen Tuerraum kaum vom Hintergrund ab (dE76 gegen BSS 50A0/12 im Videoraum 3-6 innen wie aussen,
auto_*_t*.png): ein reiner Schwellen-Umriss ist unbrauchbar. Darum halbautomatisch: Handspur als Polygon (abgelesen an
g_*.png = Ausschnitt x4, Gamma 0,6, Gitter 10 px), dazu fuer den Schwanz in Foto 2 die Bewegungs-/Chroma-Maske der
Leser V4 (sfv4/schwanz_maske.npy, B_L3_masken.npz 'Mc', Vereinigung) und UNBEKANNT-Zonen, wo kein Rand ablesbar ist
(verdeckt oder kontrastlos). Bezeichnung: 1 = Figur sicher, 0 = Hintergrund sicher, 2 = unbekannt (zaehlt in keiner IoU).
"""
import os, sys, json, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
import v4_gemein as G

# Figur-Polygone (Videopixel 640x480)
POLY = {
 1: [[(296, 182), (310, 172), (330, 167), (345, 160), (358, 150), (368, 142), (380, 139), (398, 139), (410, 142),
      (420, 150), (430, 161), (445, 170), (458, 176), (472, 180), (490, 182), (502, 188), (508, 200), (511, 215),
      (512, 240), (512, 270), (511, 300), (511, 330), (508, 350), (504, 368), (494, 372), (484, 368), (478, 352),
      (480, 335), (482, 310), (478, 290), (470, 270), (460, 258), (450, 255), (445, 265), (442, 300), (440, 330),
      (430, 360), (415, 372), (395, 372), (375, 375), (358, 372), (352, 350), (352, 320), (355, 290), (350, 265),
      (335, 252), (320, 248), (305, 246), (297, 238), (295, 215), (295, 195)]],
 2: [[(332, 197), (345, 192), (360, 189), (380, 186), (400, 186), (420, 189), (440, 195), (452, 202), (460, 212),
      (465, 225), (466, 240), (465, 252), (458, 262), (448, 268), (440, 280), (438, 300), (436, 320), (436, 346),
      (380, 346), (372, 330), (365, 315), (362, 300), (360, 285), (362, 272), (360, 262), (357, 252), (340, 258),
      (320, 262), (305, 268), (300, 280), (300, 295), (298, 305), (296, 315), (298, 330), (300, 346), (284, 346),
      (282, 330), (280, 315), (279, 300), (281, 285), (283, 270), (284, 255), (282, 248), (288, 238), (296, 228),
      (305, 220), (318, 214), (325, 205)]],
}
# Unbekannt (kein Rand ablesbar)
UNBEK = {
 1: [[(282, 245), (355, 245), (355, 425), (282, 425)],          # Sichel links (B): dunkel vor dunklem Tuerraum
     [(440, 255), (485, 255), (485, 350), (440, 350)],          # Spalt Rumpf / Arm rechts (B)
     [(340, 370), (480, 370), (480, 450), (340, 450)]],         # Fuesse im Bodenschatten (Sohle in n1485 verdeckt)
 2: [[(0, 346), (639, 346), (639, 479), (0, 479)],              # gruener Vordergrund (Spieler) verdeckt Beine
     [(515, 0), (639, 0), (639, 479), (515, 479)],              # oranger Vordergrund rechts
     [(302, 268), (360, 262), (375, 346), (302, 346)],          # Spalt Schwanz / naher Arm: dunkel-rosa, kein Rand
     [(436, 265), (462, 265), (462, 346), (436, 346)]],         # hinter dem Rumpf vor dem Pfosten: dunkel, kein Rand
}
WEG = {1: [], 2: []}


def trimap(b):
    t = np.zeros((480, 640), np.uint8)
    f = np.zeros((480, 640), np.uint8)
    for p in POLY[b]:
        cv2.fillPoly(f, [np.array(p, np.int32)], 1)
    if b == 2:
        S = np.load(os.path.join(G.SFV4, 'schwanz_maske.npy')) | np.load(os.path.join(G.SFV4, 'B_L3_masken.npz'))['Mc']
        S[:, 270:] &= True; S[:, :270] = False            # Streuung links von x 270 (helle Bodenkante) nicht
        f |= S.astype(np.uint8)
    u = np.zeros((480, 640), np.uint8)
    for p in UNBEK[b]:
        cv2.fillPoly(u, [np.array(p, np.int32)], 1)
    t[(u > 0) & (f == 0)] = 2
    t[f > 0] = 1
    if b == 2:
        t[346:] = 2; t[:, 515:] = 2
    return t


if __name__ == '__main__':
    S = os.path.join(G.SFV4, 'foto')
    from f_gitter import gitter
    R = {}
    for b in (1, 2):
        t = trimap(b)
        np.save(os.path.join(S, 'trimap_%d.npy' % b), t)
        ys = np.nonzero((t == 1).any(1))[0]
        R[b] = dict(figur_px=int((t == 1).sum()), unbekannt_px=int((t == 2).sum()), oberkante=int(ys.min()))
        g = gitter(G.vorlage(b), 260, 120, 530, 450, 3, masken=[(t == 1, (0, 255, 0)), (t == 2, (0, 0, 255))])
        cv2.imwrite(os.path.join(S, 'trimap_%d.png' % b), g)
    print(json.dumps(R))
