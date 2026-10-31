#!/usr/bin/env python3
"""Bauwerkzeug Bahnsteig-Form: Zwischenbeschreibung <-> RE2-EMD + TIM.

    python re15_port/tools/form_emd.py rundlauf [--datei EMD] [--tim TIM]   Beweis Lesen -> Schreiben
    python re15_port/tools/form_emd.py probe --aus DIR                      Probemodell schreiben
    python re15_port/tools/form_emd.py info DATEI.EMD [--tim DATEI.TIM]     Kennzahlen + Pruefung
    python re15_port/tools/form_emd.py bau form.json --emd X.EMD --tim X.TIM (--tim-roh bild.png)

Als Bibliothek:

    import form_emd as FE
    form = FE.neu()
    b0 = FE.knochen_dazu(form, 'rumpf', -1, (0, 0, 0))
    b1 = FE.knochen_dazu(form, 'kopf', b0, (1100, -300, 0))
    FE.quader(form, b0, (-900, -400, -500), (900, 400, 500), tex=(0, 0, 63, 63))
    FE.clip_dazu(form, 'stehen', [FE.bild(form, wurzel=(0, -1000, 0), winkel={b1: (0, 200, 0)})])
    FE.orientiere(form)
    open('X.EMD', 'wb').write(FE.schreibe_emd(form))
    open('X.TIM', 'wb').write(FE.tim_schreiben(pixel, [clut]))

KEIN BYTE-TRUE-ASSET: dieses Werkzeug schreibt ein neues Modell im Format, das der Port laedt. Belegt ist hier nur das
FORMAT. Jede Formatregel unten nennt die Stelle im Port, die sie verlangt; was der Port darueber hinaus erwartet, steht in
analysis/befunde_runde41/form/werkzeug.md Abschnitt "Was der Port voraussetzt".

ZWISCHENBESCHREIBUNG (reines dict, JSON-faehig; speichern()/laden())
  knochen  [{name, eltern, versatz}]  eltern = Index, -1 = Wurzel, Eltern stehen VOR dem Kind. versatz = (x, y, z) im Raum
           des Elternknochens (EMR-Bindversatz). Y zeigt nach UNTEN, Vorderseite = +X, Weltlage siehe unten.
  meshes   [{ecken, normalen, dreiecke, vierecke}]  Mesh i haengt an Knochen i. ecken/normalen = s16-Tripel im Knochenraum,
           Normalen mit Laenge 4096. Die Flaechen indizieren beide Listen getrennt.
           dreieck {e:[a,b,c], n:[a,b,c], tex:[[x,y]*3], zeile}   viereck {e:[a,b,c,d], n:[...], tex:[[x,y]*4], zeile}
           tex = Texel im TIM (x darf ueber 255 gehen), zeile = CLUT-Zeile. Roher Weg aus dem Leser: uv + clut + page.
           Vierecke sind Z-foermig: a-b und a-c sind Kanten, d liegt a gegenueber (Port zeichnet a,b,d und a,d,c).
           Windung: cross(b-a, c-a) zeigt GEGEN die Aussennormale (orientiere() stellt das her).
  clips    [{name, bilder:[bild]}]  bild = {wurzel, spur, winkel, wort} oder aus dem Leser {kf, wort} mit form['pool'].
           wurzel = Keyframe +0, Position des Wurzelknochens im Aktorraum.
           spur   = Keyframe +6, KUMULIERT ueber den Clip; bb_spur leitet daraus den Vortrieb ab (Differenz je Bild, x vorn).
           winkel = je Knochen (ax, ay, az) in 1/4096 Umdrehung, RotMatrix M = Rx * Ry * Rz, Vektor v -> M v.
           wort   = obere Bits des EDD-Worts: BILD_TREFFER, BILD_TON, ton(id).
  tim      {clut_x, clut_y, seite}  clut_y = VRAM-Zeile der ersten CLUT, seite = obere Bits des TPage-Worts (0x80 = 8 bpp).
  roh      optional, Sektionen ohne Modellbezug als Base64: morph, paar2 [edd, emr], paar3 [edd, emr].

WELTLAGE (platform/pc/main.c nyaw, RotMatrixY): welt = Y(gier) * (R_knochen v + T_knochen) + Aktorlage,
Y(g) = [c 0 s; 0 1 0; -s 0 c]. Bei gier 0 zeigt die Vorderseite +X nach Welt +X.
"""
import argparse, base64, json, math, os, struct, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import emd_ansichtsblatt as EA          # noqa: E402  (Leser EMR/EDD/MD1/TIM, RotMatrix, Pose)
import birkin_formen as BF              # noqa: E402  (md1_lesen, md1_schreiben, Euler-Zerlegung)

# ═══════════════════════════════════════════════════ Grenzen des Ports
MAX_KNOCHEN    = 32     # RE15_EMD_MAX_BONES include/re15_emd.h:47, emd_common.c:149 Rueckgabe -3
MAX_MESHES     = 32     # MD1_MAX_MESHES include/re15_md1.h:41, md1_common.c:53 kappt still
MAX_CLIPS      = 64     # RE15_EMD_MAX_CLIPS, emd_common.c:63 Rueckgabe -4
MAX_BILDER     = 1664   # RE15_EMD_MAX_FRAMES, emd_common.c:84 Rueckgabe -6 (Summe aller Clips)
MAX_KEYFRAMES  = 4096   # EDD-Wort Bits 0..11, emd_common.c:94
MAX_CLIP_BILD  = 256    # re15_birkin_anim enemy_ai_common.c:11898 `(uint8_t)` auf anim_frame
TIM_MAX_HOEHE  = 256    # u8 v im MD1-UV (re15_md1.h)
TIM_MAX_BREITE = 15 * 128 + 256   # u8 u + (page & 0xF) * 128, main.c `page_off`
RE2_VERZ       = 8      # re2_ems.c re2_emd_parse_bank: dir_cnt != 8 -> -2

BILD_TREFFER = 0x00010000   # BB_FL_TREFFER birkin_bahnsteig.c
BILD_TON     = 0x08000000   # BB_FL_TON birkin_bahnsteig.c, Id = Wort >> 28


def ton(ton_id):
    """EDD-Bits eines Bild-Tons (bb_bild_ton: Bit 27, Id = Wort >> 28)."""
    return BILD_TON | ((ton_id & 0xF) << 28)


# Clips und Teile, die der Tick der Form fest benutzt (engine/src/birkin_bahnsteig.c).
TICK_CLIPS = {0: 'BB_C_RENNEN', 3: 'BB_C_HIEB', 4: 'BB_C_HIEB+1', 5: 'BB_C_GEHEN', 8: 'BB_C_ZUCKEN',
              9: 'BB_C_NACHHIEB', 10: 'BB_C_DROHEN', 26: 'BB_C_BISS', 27: 'BB_C_GRIFF'}
TICK_TEILE = {10: 'BB_TEIL_RUMPF', 13: 'Hieb Seite 1', 18: 'Hieb Seite 0'}
# Bildnummern, die der Tick anspringt: Doppelschlag k_doppel (Clip 3 Bild 30, Clip 4 Bild 19), Zucken ab Bild 5, Griff bei Bild 8.
TICK_CLIP_BILD = {3: 31, 4: 20, 8: 6, 27: 9}
TICK_TREFFERCLIPS = (3, 4, 26)      # BB_FL_TREFFER liest bb_hieb und bb_biss aus dem EDD-Wort


class FormFehler(Exception):
    pass


u16 = EA.u16
u32 = EA.u32


def _b64(b):
    return base64.b64encode(bytes(b)).decode('ascii')


def _unb64(s):
    return base64.b64decode(s)


def _s12(w):
    w &= 0xFFF
    return w - 0x1000 if w & 0x800 else w


def _liste(v):
    return [int(x) for x in v]


# ═══════════════════════════════════════════════════ Formen bauen
def neu(clut_x=0, clut_y=480, seite=0x80):
    return dict(knochen=[], meshes=[], clips=[], tim=dict(clut_x=clut_x, clut_y=clut_y, seite=seite))


def knochen_dazu(form, name, eltern, versatz):
    """Knochen + leeres Mesh gleichen Index anhaengen; Rueckgabe = Index."""
    i = len(form['knochen'])
    if eltern >= i:
        raise FormFehler('Knochen %s: Eltern %d stehen nicht vor dem Kind %d' % (name, eltern, i))
    form['knochen'].append(dict(name=name, eltern=int(eltern), versatz=_liste(versatz)))
    if len(form['meshes']) <= i:
        form['meshes'].append(dict(ecken=[], normalen=[], dreiecke=[], vierecke=[]))
    return i


def ecke(mesh, p):
    mesh['ecken'].append([int(round(c)) for c in p])
    return len(mesh['ecken']) - 1


def normale(mesh, n):
    """Normale auf Laenge 4096 bringen (Laenge der Originale, gemessen 4096/4097)."""
    v = np.asarray(n, float)
    v = v / (np.linalg.norm(v) or 1.0) * 4096.0
    mesh['normalen'].append([int(round(c)) for c in v])
    return len(mesh['normalen']) - 1


def dreieck(mesh, e, n, tex, zeile=0):
    mesh['dreiecke'].append(dict(e=_liste(e), n=_liste(n), tex=[_liste(t) for t in tex], zeile=int(zeile)))


def viereck(mesh, e, n, tex, zeile=0):
    """Z-Reihenfolge: a-b und a-c Kanten, d gegenueber von a."""
    mesh['vierecke'].append(dict(e=_liste(e), n=_liste(n), tex=[_liste(t) for t in tex], zeile=int(zeile)))


def quader(form, knochen, lo, hi, tex, zeile=0):
    """Achsparalleler Quader im Raum von `knochen`. tex = (x0, y0, x1, y1) fuer alle Seiten oder Liste von sechs
    Rechtecken in der Reihenfolge -X +X -Y +Y -Z +Z. Flache Normalen, Windung nach der Regel im Dateikopf."""
    m = form['meshes'][knochen]
    x = (lo[0], hi[0]); y = (lo[1], hi[1]); z = (lo[2], hi[2])
    c = {}
    for i in range(2):
        for j in range(2):
            for k in range(2):
                c[i, j, k] = ecke(m, (x[i], y[j], z[k]))
    seiten = [((-1, 0, 0), (0, 0, 0), (0, 1, 0), (0, 0, 1)),   # (Normale, Anfang, Kante 1, Kante 2)
              ((1, 0, 0),  (1, 0, 0), (0, 1, 0), (0, 0, 1)),
              ((0, -1, 0), (0, 0, 0), (1, 0, 0), (0, 0, 1)),
              ((0, 1, 0),  (0, 1, 0), (1, 0, 0), (0, 0, 1)),
              ((0, 0, -1), (0, 0, 0), (1, 0, 0), (0, 1, 0)),
              ((0, 0, 1),  (0, 0, 1), (1, 0, 0), (0, 1, 0))]
    rechtecke = [tex] * 6 if isinstance(tex[0], (int, np.integer)) else list(tex)
    for (nv, a, k1, k2), r in zip(seiten, rechtecke):
        ia = tuple(a)
        ib = tuple(p + q for p, q in zip(a, k1))
        ic = tuple(p + q for p, q in zip(a, k2))
        id_ = tuple(p + q + s for p, q, s in zip(a, k1, k2))
        n = normale(m, nv)
        viereck(m, (c[ia], c[ib], c[ic], c[id_]), (n, n, n, n),
                ((r[0], r[1]), (r[2], r[1]), (r[0], r[3]), (r[2], r[3])), zeile)


def netz(form, knochen, punkte, flaechen, tex, zeile=0, normalen=None):
    """Fertiges Netz in den Raum von `knochen` uebernehmen. punkte n x 3, flaechen = Index-Tripel oder -Quadrupel als RING um die
    Flaeche (wird hier in die Z-Reihenfolge des Formats gebracht), tex n x 2 Texel je Punkt. Ohne normalen: glatte Normalen,
    flaechengewichtet, Aussenseite aus der Ringrichtung, von aussen gesehen im Uhrzeigersinn. Rueckgabe: Index der ersten Ecke."""
    m = form['meshes'][knochen]
    p = np.asarray(punkte, float)
    if normalen is None:
        nsum = np.zeros_like(p)
        for f in flaechen:
            for i in range(1, len(f) - 1):
                k = np.cross(p[f[i + 1]] - p[f[0]], p[f[i]] - p[f[0]])     # Regel des Formats: Windung gegen die Normale
                for j in (f[0], f[i], f[i + 1]):
                    nsum[j] += k
        normalen = nsum
    v0, n0 = len(m['ecken']), len(m['normalen'])
    for q in p:
        ecke(m, q)
    for q in np.asarray(normalen, float):
        normale(m, q if np.any(q) else (0, -1, 0))
    t = [[int(round(c)) for c in x] for x in tex]
    for f in flaechen:
        if len(f) == 3:
            dreieck(m, [v0 + i for i in f], [n0 + i for i in f], [t[i] for i in f], zeile)
        else:
            z = (f[0], f[1], f[3], f[2])
            viereck(m, [v0 + i for i in z], [n0 + i for i in z], [t[i] for i in z], zeile)
    return v0


def bild(form, wurzel=(0, 0, 0), spur=(0, 0, 0), winkel=None, wort=0):
    """Ein Keyframe. winkel = dict {Knochen: (ax, ay, az)} oder volle Liste; fehlende Knochen = 0."""
    n = len(form['knochen'])
    w = [[0, 0, 0] for _ in range(n)]
    if isinstance(winkel, dict):
        for b, a in winkel.items():
            w[int(b)] = _liste(a)
    elif winkel is not None:
        w = [_liste(a) for a in winkel]
    return dict(wurzel=_liste(wurzel), spur=_liste(spur), winkel=w, wort=int(wort))


def clip_dazu(form, name, bilder):
    form['clips'].append(dict(name=name, bilder=list(bilder)))
    return len(form['clips']) - 1


def grad(g):
    """Grad -> Engine-Winkel (1/4096 Umdrehung)."""
    return int(round(g * 4096.0 / 360.0))


def winkel_aus_matrix(m):
    """Drehmatrix (3x3, float) -> (ax, ay, az) der Engine, Umkehrung von EA.mat_euler; vorzeichenrichtig."""
    return tuple(_s12(w) for w in BF._zerlege(np.asarray(m, float)))


# ═══════════════════════════════════════════════════ Windung
def _kreuz(mesh, e):
    p = [np.array(mesh['ecken'][i], float) for i in e[:3]]
    return np.cross(p[1] - p[0], p[2] - p[0])


def _normalensumme(mesh, nn):
    return sum(np.array(mesh['normalen'][i], float) for i in nn)


def _falsch_gewunden(mesh, f):
    return float(_kreuz(mesh, f['e']) @ _normalensumme(mesh, f['n'])) > 0


def _tausche_12(f):
    for k in ('e', 'n', 'tex', 'uv'):
        if k in f:
            f[k][1], f[k][2] = f[k][2], f[k][1]


def orientiere(form):
    """Windung jeder Flaeche an ihren Normalen ausrichten: cross(b-a, c-a) gegen die Normale. Grund: der Zeichner verwirft
    fuer Typ 0x30 Rueckseiten (re15_nclip_r41_weg: Dreieck bei MAC0 <= 0, Viereck bei MAC0 < 0, Bildschirm-Y nach unten).
    Rueckgabe = Zahl der gedrehten Flaechen."""
    n = 0
    for m in form['meshes']:
        for f in m['dreiecke'] + m['vierecke']:
            if _falsch_gewunden(m, f):
                _tausche_12(f)
                n += 1
    return n


# ═══════════════════════════════════════════════════ Pose der Beschreibung
def pose_aus_bild(form, b):
    """re15_skel_compute_pose: R = R_eltern * lokal, T = R_eltern * versatz + T_eltern; Wurzel T = Keyframe."""
    ks = form['knochen']
    R, T = [None] * len(ks), [None] * len(ks)
    k = form['pool'][b['kf']] if 'kf' in b else b
    wurzel = np.array(k['wurzel'], dtype=np.int64)
    for i, kn in enumerate(ks):
        loc = EA.mat_euler(*k['winkel'][i])
        p = kn['eltern']
        if p < 0 or p >= i:
            R[i], T[i] = loc, wurzel
        else:
            R[i] = (R[p] @ loc) >> 12
            T[i] = ((R[p] @ np.array(kn['versatz'], dtype=np.int64)) >> 12) + T[p]
    return R, T


def tiefster_punkt(form, b):
    """Groesstes Y (nach unten) ueber alle Ecken in dieser Pose, im Aktorraum."""
    R, T = pose_aus_bild(form, b)
    tief = None
    for i in range(len(form['knochen'])):
        v = form['meshes'][i]['ecken']
        if not v:
            continue
        p = np.array(v, float) @ (R[i].astype(float).T / 4096.0) + T[i]
        y = float(p[:, 1].max())
        tief = y if tief is None else max(tief, y)
    return tief


def boden_nachfuehren(form, ziel_y=0):
    """Wurzelhoehe je Bild so, dass der tiefste Punkt auf ziel_y liegt (Aktorlage y = 0 ist der Boden)."""
    for c in form['clips']:
        for b in c['bilder']:
            t = tiefster_punkt(form, b)
            if t is not None:
                b['wurzel'][1] = int(round(b['wurzel'][1] - (t - ziel_y)))


# ═══════════════════════════════════════════════════ EDD
def edd_lesen(s):
    """Liste je Clip -> Worte. Tabelle je Clip (u16 Bilder, u16 Byte-Offset), dann u32-Worte, am Ende u32 = Gesamtlaenge
    ohne dieses Wort (gemessen an BAHNSTEIG_G4.EMD: 0x16f8 = 5880 Paar 1, 0x354 = 852 Paar 3, 0 im leeren Paar 2)."""
    if len(s) < 4 or u16(s, 2) == 0:
        return []
    n = u16(s, 2) // 4
    return [list(struct.unpack_from('<%dI' % u16(s, i * 4), s, u16(s, i * 4 + 2))) for i in range(n)]


def edd_schreiben(clips):
    if not clips:
        return b'\0\0\0\0'                     # leeres Paar wie dir[3] im Muster (emd_common.c:58 -> -3, Paar unbenutzt)
    cur = 4 * len(clips)
    kopf, leib = bytearray(), bytearray()
    for c in clips:
        kopf += struct.pack('<HH', len(c), cur)
        leib += struct.pack('<%dI' % len(c), *c)
        cur += 4 * len(c)
    return bytes(kopf) + bytes(leib) + struct.pack('<I', cur)


# ═══════════════════════════════════════════════════ EMR
def keyframe_groesse(n):
    """12 Byte Kopf, 36 Bit je Knochen, auf 4 Byte aufgerundet. Gemessen: 13 -> 72, 15 -> 80, 16 -> 84, 27 -> 136."""
    return (12 + (n * 36 + 7) // 8 + 3) & ~3


def keyframe_lesen(s, o, n, ks):
    px, py, pz, sx, sy, sz = struct.unpack_from('<6h', s, o)
    bits = int.from_bytes(s[o + 12:o + ks], 'little')
    winkel = [[_s12(bits >> (b * 36 + i * 12)) for i in range(3)] for b in range(n)]
    return dict(wurzel=[px, py, pz], spur=[sx, sy, sz], winkel=winkel)


def keyframe_schreiben(k, n, ks):
    """Bit 0 von Byte 0 = LSB des ersten Werts (bit_read emd_common.c), je Knochen x, y, z zu 12 Bit ab Keyframe +12."""
    bits = 0
    for b in range(n):
        for i in range(3):
            bits |= (int(k['winkel'][b][i]) & 0xFFF) << (b * 36 + i * 12)
    return struct.pack('<6h', *k['wurzel'], *k['spur']) + bits.to_bytes(ks - 12, 'little')


def emr_lesen(s):
    tab, kfo, n, ks = struct.unpack_from('<4H', s, 0)
    rel = [list(struct.unpack_from('<3h', s, 8 + b * 6)) for b in range(n)]
    eltern = [-1] * n
    for b in range(n):                         # wie emr_assign_parents: der erste Elternteil gewinnt
        anz, off = struct.unpack_from('<HH', s, tab + b * 4)
        for i in range(anz):
            c = s[tab + off + i]
            if c < n and c != b and eltern[c] == -1:
                eltern[c] = b
    pool = [keyframe_lesen(s, kfo + i * ks, n, ks) for i in range((len(s) - kfo) // ks)]
    return rel, eltern, pool, ks


def emr_schreiben(knochen, pool):
    """Kopf (Tabelle, Keyframes, Knochen, Keyframe-Groesse), Bindversaetze, Tabelle je Knochen (Kinderzahl, Listenoffset
    relativ zur Tabelle), Kinderbytes, Keyframes; Tabelle und Keyframes liegen auf 4 Byte."""
    n = len(knochen)
    ks = keyframe_groesse(n)
    kinder = [[] for _ in range(n)]
    for i, k in enumerate(knochen):
        if k['eltern'] >= 0:
            kinder[k['eltern']].append(i)
    tab = (8 + 6 * n + 3) & ~3
    cursor = 4 * n
    tabelle, liste = bytearray(), bytearray()
    for b in range(n):
        tabelle += struct.pack('<HH', len(kinder[b]), cursor)
        liste += bytes(kinder[b])
        cursor += len(kinder[b])
    kfo = (tab + 4 * n + len(liste) + 3) & ~3
    out = bytearray(kfo)
    struct.pack_into('<4H', out, 0, tab, kfo, n, ks)
    for b, k in enumerate(knochen):
        struct.pack_into('<3h', out, 8 + b * 6, *k['versatz'])
    out[tab:tab + len(tabelle)] = tabelle
    out[tab + len(tabelle):tab + len(tabelle) + len(liste)] = liste
    return bytes(out) + b''.join(keyframe_schreiben(k, n, ks) for k in pool)


def _keyframe_schluessel(k):
    return (tuple(k['wurzel']), tuple(k['spur']), tuple(tuple(int(c) & 0xFFF for c in a) for a in k['winkel']))


def pool_und_worte(form):
    """Keyframe-Pool + EDD-Worte. Bilder aus dem Leser tragen schon {kf, wort}; neue Bilder werden nach Inhalt
    zusammengelegt, in der Reihenfolge der ersten Benutzung (so liegt auch das Muster, gemessen: 1296 Keyframes)."""
    pool = list(form.get('pool') or [])
    index = {}
    for i, k in enumerate(pool):
        index.setdefault(_keyframe_schluessel(k), i)
    worte = []
    for c in form['clips']:
        zeile = []
        for b in c['bilder']:
            if 'kf' in b:
                i = int(b['kf'])
            else:
                s = _keyframe_schluessel(b)
                i = index.get(s)
                if i is None:
                    i = index[s] = len(pool)
                    pool.append(dict(wurzel=b['wurzel'], spur=b['spur'], winkel=b['winkel']))
            zeile.append(i | (int(b.get('wort', 0)) & 0xFFFFF000))
        worte.append(zeile)
    return pool, worte


# ═══════════════════════════════════════════════════ MD1
def md1_lesen(md1):
    roh = BF.md1_lesen(md1)
    meshes = []
    for m in roh:
        d = []
        for f, uv in zip(m['tf'], m['tuv']):
            d.append(dict(e=[f[1], f[3], f[5]], n=[f[0], f[2], f[4]],
                          uv=[[uv[0], uv[1]], [uv[3], uv[4]], [uv[6], uv[7]]], clut=uv[2], page=uv[5]))
        q = []
        for f, uv in zip(m['qf'], m['quv']):
            q.append(dict(e=[f[1], f[3], f[5], f[7]], n=[f[0], f[2], f[4], f[6]],
                          uv=[[uv[0], uv[1]], [uv[3], uv[4]], [uv[6], uv[7]], [uv[9], uv[10]]], clut=uv[2], page=uv[5]))
        meshes.append(dict(ecken=[x[:3] for x in m['v']], normalen=[x[:3] for x in m['n']], dreiecke=d, vierecke=q))
    return meshes


def _uv_aufloesen(f, tim):
    """-> (uv, clut, page). Roher Weg aus dem Leser, sonst tex/zeile: TPage = linkes Ende, u = x - 128 * Seite; v = y.
    Port: Texel-x = u + (page & 0xF) * 128 (main.c page_off), Texel-y = v + Zeile * TIM-Hoehe (render_pc.c v_offset)."""
    if 'uv' in f:
        return f['uv'], int(f['clut']), int(f['page'])
    xs = [t[0] for t in f['tex']]
    seite = min(xs) // 128
    uv = [[t[0] - seite * 128, t[1]] for t in f['tex']]
    clut = ((tim['clut_y'] + f.get('zeile', 0)) << 6) | (tim['clut_x'] >> 4)
    return uv, clut, tim['seite'] | seite


def md1_schreiben(form):
    tim = form.get('tim') or dict(clut_x=0, clut_y=480, seite=0x80)
    out = []
    for m in form['meshes']:
        tf, tuv, qf, quv = [], [], [], []
        for f in m['dreiecke']:
            uv, c, p = _uv_aufloesen(f, tim)
            tf.append([f['n'][0], f['e'][0], f['n'][1], f['e'][1], f['n'][2], f['e'][2]])
            tuv.append([uv[0][0], uv[0][1], c, uv[1][0], uv[1][1], p, uv[2][0], uv[2][1], 0])
        for f in m['vierecke']:
            uv, c, p = _uv_aufloesen(f, tim)
            qf.append([f['n'][0], f['e'][0], f['n'][1], f['e'][1], f['n'][2], f['e'][2], f['n'][3], f['e'][3]])
            quv.append([uv[0][0], uv[0][1], c, uv[1][0], uv[1][1], p, uv[2][0], uv[2][1], 0, uv[3][0], uv[3][1], 0])
        out.append(dict(v=[list(x) + [0] for x in m['ecken']], n=[list(x) + [0] for x in m['normalen']],
                        tf=tf, tuv=tuv, qf=qf, quv=quv))
    return BF.md1_schreiben(out)


# ═══════════════════════════════════════════════════ EMD
def verzeichnis(emd):
    dir_off, n = struct.unpack_from('<II', emd, 0)
    if n != RE2_VERZ or dir_off + n * 4 > len(emd):
        raise FormFehler('kein RE2-EMD: %d Verzeichnis-Eintraege bei 0x%x' % (n, dir_off))
    return dir_off, list(struct.unpack_from('<8I', emd, dir_off))


def sektionen(emd):
    dir_off, d = verzeichnis(emd)
    ende = d[1:] + [dir_off]
    namen = ('morph', 'edd1', 'emr1', 'edd2', 'emr2', 'edd3', 'emr3', 'md1')
    return {n: bytes(emd[a:e]) for n, a, e in zip(namen, d, ende)}


def lese_emd(emd):
    """EMD -> Zwischenbeschreibung, verlustfrei: Keyframes als Pool, Bilder als {kf, wort}, Rohsektionen in form['roh']."""
    s = sektionen(emd)
    rel, eltern, pool, _ks = emr_lesen(s['emr1'])
    form = neu()
    form['knochen'] = [dict(name='k%d' % i, eltern=eltern[i], versatz=rel[i]) for i in range(len(rel))]
    form['meshes'] = md1_lesen(s['md1'])
    form['pool'] = pool
    form['clips'] = [dict(name='c%d' % i, bilder=[dict(kf=w & 0xFFF, wort=w & 0xFFFFF000) for w in c])
                     for i, c in enumerate(edd_lesen(s['edd1']))]
    form['roh'] = dict(morph=_b64(s['morph']), paar2=[_b64(s['edd2']), _b64(s['emr2'])],
                       paar3=[_b64(s['edd3']), _b64(s['emr3'])])
    return form


def schreibe_emd(form, pruefen=True):
    """Layout des Musters: u32 dir_off, u32 8, Sektionen dir[0..7], Verzeichnis am Ende.
    Ohne form['roh']: Morphblock Maske 0 (12 Byte, re2_ems.c setzt dann kein morph_ok), Paar 2 und 3 je leer."""
    if pruefen:
        fehler, _ = pruefe(form)
        if fehler:
            raise FormFehler('\n'.join(fehler))
    pool, worte = pool_und_worte(form)
    roh = form.get('roh') or {}
    morph = _unb64(roh['morph']) if 'morph' in roh else b'\0' * 12
    paar2 = [_unb64(x) for x in roh['paar2']] if 'paar2' in roh else [b'\0' * 4, b'\0' * 4]
    paar3 = [_unb64(x) for x in roh['paar3']] if 'paar3' in roh else [b'\0' * 4, b'\0' * 4]
    teile = [morph, edd_schreiben(worte), emr_schreiben(form['knochen'], pool), paar2[0], paar2[1], paar3[0], paar3[1],
             md1_schreiben(form)]
    d, cur = [], 8
    for t in teile:
        if len(t) % 4:
            raise FormFehler('Sektion %d nicht auf 4 Byte: %d' % (len(d), len(t)))
        d.append(cur)
        cur += len(t)
    return struct.pack('<II', cur, RE2_VERZ) + b''.join(teile) + struct.pack('<8I', *d)


# ═══════════════════════════════════════════════════ Pruefung gegen den Port
def pruefe(form):
    """-> (fehler, warnungen). Fehler brechen schreibe_emd ab: der Port liest die Datei dann nicht oder falsch.
    Warnungen laufen, aber etwas im Spiel fehlt (Treffer, Clip, Licht)."""
    F, W = [], []
    ks, ms = form['knochen'], form['meshes']
    n = len(ks)
    if not 1 <= n <= MAX_KNOCHEN:
        F.append('%d Knochen, Port kann 1..%d (RE15_EMD_MAX_BONES)' % (n, MAX_KNOCHEN))
    if len(ms) > MAX_MESHES:
        F.append('%d Meshes, Port kappt bei %d (MD1_MAX_MESHES)' % (len(ms), MAX_MESHES))
    if len(ms) < n:
        F.append('%d Meshes fuer %d Knochen: main.c kappt die Knochenzahl auf die Meshzahl' % (len(ms), n))
    if len(ms) > n:
        W.append('%d Meshes ueber der Knochenzahl: Typ 0x30 zeichnet sie nicht (main.c npc_zeichen_n)' % (len(ms) - n))
    wurzeln = 0
    for i, k in enumerate(ks):
        if k['eltern'] < 0:
            wurzeln += 1
        elif k['eltern'] >= i:
            F.append('Knochen %d: Eltern %d stehen nicht davor; skeleton_common.c macht ihn sonst zur Wurzel' % (i, k['eltern']))
        for c in k['versatz']:
            if not -32768 <= c <= 32767:
                F.append('Knochen %d: Versatz ausserhalb s16' % i)
    if ks and ks[0]['eltern'] >= 0:
        F.append('Knochen 0 muss Wurzel sein')
    if wurzeln > 1:
        W.append('%d Wurzeln: jede sitzt auf der Keyframe-Wurzelposition' % wurzeln)
    tim = form.get('tim') or {}
    for mi, m in enumerate(ms):
        nv, nn = len(m['ecken']), len(m['normalen'])
        for c in m['ecken'] + m['normalen']:
            if len(c) != 3 or any(not -32768 <= x <= 32767 for x in c):
                F.append('Mesh %d: Koordinate ausserhalb s16: %r' % (mi, c))
                break
        for x in m['normalen']:
            lg = math.sqrt(sum(c * c for c in x))
            if abs(lg - 4096) > 41:
                W.append('Mesh %d: Normale mit Laenge %.0f statt 4096 (Licht NCCT rechnet mit Q12-Einheit)' % (mi, lg))
                break
        falsch = 0
        for art, faces, ne in (('Dreieck', m['dreiecke'], 3), ('Viereck', m['vierecke'], 4)):
            for fi, f in enumerate(faces):
                if len(f['e']) != ne or len(f['n']) != ne:
                    F.append('Mesh %d %s %d: %d Eckindizes' % (mi, art, fi, len(f['e'])))
                    continue
                if any(not 0 <= v < nv for v in f['e']) or any(not 0 <= v < nn for v in f['n']):
                    F.append('Mesh %d %s %d: Index ausserhalb (Ecken %d, Normalen %d)' % (mi, art, fi, nv, nn))
                    continue
                if 'tex' in f:
                    xs = [t[0] for t in f['tex']]
                    ys = [t[1] for t in f['tex']]
                    if len(xs) != ne:
                        F.append('Mesh %d %s %d: %d tex-Eintraege' % (mi, art, fi, len(xs)))
                        continue
                    if min(xs) < 0 or max(xs) - (min(xs) // 128) * 128 > 255:
                        F.append('Mesh %d %s %d: tex-x %d..%d passt in keine TPage (u8 + 128 je Seite)' % (mi, art, fi, min(xs), max(xs)))
                    if min(ys) < 0 or max(ys) > 255:
                        F.append('Mesh %d %s %d: tex-y %d..%d ausserhalb u8' % (mi, art, fi, min(ys), max(ys)))
                    if not 0 <= f.get('zeile', 0) < 512 - tim.get('clut_y', 480):
                        F.append('Mesh %d %s %d: CLUT-Zeile %d' % (mi, art, fi, f.get('zeile', 0)))
                if _falsch_gewunden(m, f):
                    falsch += 1
        if falsch:
            W.append('Mesh %d: %d Flaechen falsch gewunden, der Zeichner verwirft sie (orientiere() richtet sie aus)' % (mi, falsch))
    cl = form['clips']
    if not 1 <= len(cl) <= MAX_CLIPS:
        F.append('%d Clips, Port braucht 1..%d' % (len(cl), MAX_CLIPS))
    gesamt = 0
    for ci, c in enumerate(cl):
        b = c['bilder']
        gesamt += len(b)
        if len(b) > MAX_CLIP_BILD:
            F.append('Clip %d: %d Bilder, anim_frame laeuft durch (uint8_t) in re15_birkin_anim' % (ci, len(b)))
        if not b:
            W.append('Clip %d ist leer' % ci)
        for bi, x in enumerate(b):
            k = form['pool'][x['kf']] if 'kf' in x else x
            if len(k['winkel']) != n:
                F.append('Clip %d Bild %d: %d Winkel fuer %d Knochen' % (ci, bi, len(k['winkel']), n))
                break
            if any(not -32768 <= v <= 32767 for v in list(k['wurzel']) + list(k['spur'])):
                F.append('Clip %d Bild %d: Wurzel/Spur ausserhalb s16' % (ci, bi))
            if int(x.get('wort', 0)) & 0xFFF:
                F.append('Clip %d Bild %d: wort belegt die Keyframe-Bits 0..11' % (ci, bi))
    if gesamt > MAX_BILDER:
        F.append('%d Bilder in Summe, Port kann %d (RE15_EMD_MAX_FRAMES, emd_common.c:84)' % (gesamt, MAX_BILDER))
    if not F:
        pool, _ = pool_und_worte(form)
        if len(pool) > MAX_KEYFRAMES:
            F.append('%d Keyframes, EDD-Index hat 12 Bit' % len(pool))
    fehlt = [('%d %s' % (i, nm)) for i, nm in TICK_CLIPS.items() if i >= len(cl)]
    if fehlt:
        W.append('Tick der Form erwartet Clips, die fehlen (Modulo in re15_compute_actor_kf): ' + ', '.join(fehlt))
    for i, mind in TICK_CLIP_BILD.items():
        if i < len(cl) and len(cl[i]['bilder']) < mind:
            W.append('Clip %d hat %d Bilder, der Tick braucht mindestens %d (birkin_bahnsteig.c)' % (i, len(cl[i]['bilder']), mind))
    for i in TICK_TREFFERCLIPS:
        if i < len(cl) and not any(int(b.get('wort', 0)) & BILD_TREFFER for b in cl[i]['bilder']):
            W.append('Clip %d ohne BILD_TREFFER: dieser Angriff trifft nie (BB_FL_TREFFER)' % i)
    fehlt = [('%d %s' % (i, nm)) for i, nm in TICK_TEILE.items() if i >= n]
    if fehlt:
        W.append('Tick der Form erwartet Teile, die fehlen (re15_enemy_part_point -> Treffer gehen ins Leere): ' + ', '.join(fehlt))
    return F, W


# ═══════════════════════════════════════════════════ TIM (8 bpp mit CLUTs)
def clut_wort(r, g, b):
    """8-Bit-RGB -> PSX-15-Bit. 0x0000 ist die transparente Farbe (re15_tim_texel_argb), opakes Schwarz daher 0x8000."""
    w = (int(r) >> 3) | ((int(g) >> 3) << 5) | ((int(b) >> 3) << 10)
    return w if w else 0x8000


def clut_aus_rgb(farben, transparent0=True):
    """Liste von bis zu 256 RGB-Tripeln -> 256 CLUT-Worte. transparent0: Index 0 bleibt 0x0000."""
    c = [0] * 256
    for i, f in enumerate(farben):
        c[i] = clut_wort(*f)
    if transparent0:
        c[0] = 0
    return c


def tim_schreiben(pixel, cluts, clut_x=0, clut_y=480, bild_x=0, bild_y=0):
    """pixel = H x W uint8, cluts = Liste von Zeilen zu je 256 Worten. Flag 9 = 8 bpp + CLUT (re15_tim_parse).
    Der Port nimmt je CLUT-Zeile eine Kopie der Textur untereinander (render_pc.c re15_render_pc_upload_tim_slot)."""
    px = np.asarray(pixel, dtype=np.uint8)
    h, w = px.shape
    if w % 2 or w > TIM_MAX_BREITE or h > TIM_MAX_HOEHE:
        raise FormFehler('TIM %dx%d: Breite gerade und <= %d, Hoehe <= %d' % (w, h, TIM_MAX_BREITE, TIM_MAX_HOEHE))
    for z in cluts:
        if len(z) != 256:
            raise FormFehler('CLUT-Zeile mit %d Worten statt 256' % len(z))
    n = len(cluts)
    clut = b''.join(struct.pack('<256H', *z) for z in cluts)
    return (struct.pack('<II', 0x10, 9) + struct.pack('<I4H', 12 + len(clut), clut_x, clut_y, 256, n) + clut
            + struct.pack('<I4H', 12 + w * h, bild_x, bild_y, w // 2, h) + px.tobytes())


def tim_lesen(t):
    if struct.unpack_from('<II', t, 0) != (0x10, 9):
        raise FormFehler('kein 8-bpp-TIM mit CLUT')
    cs, cx, cy, cw, ch = struct.unpack_from('<I4H', t, 8)
    cluts = [list(struct.unpack_from('<%dH' % cw, t, 20 + 2 * cw * z)) for z in range(ch)]
    o = 8 + cs
    _ds, bx, by, bw, bh = struct.unpack_from('<I4H', t, o)
    px = np.frombuffer(t, dtype=np.uint8, count=bw * 2 * bh, offset=o + 12).reshape(bh, bw * 2).copy()
    return dict(pixel=px, cluts=cluts, clut_x=cx, clut_y=cy, bild_x=bx, bild_y=by)


def tim_aus_rgb(rgb, maske=None, farben=255):
    """RGB-Bild (H x W x 3) -> (pixel, clut-Zeile). Index 0 = transparent, maske False = transparent. Medianschnitt ohne
    Rastern; 8 bpp mit einer CLUT-Zeile."""
    from PIL import Image
    rgb = np.asarray(rgb, dtype=np.uint8)
    q = Image.fromarray(rgb).quantize(colors=farben, method=Image.MEDIANCUT, dither=Image.NONE)
    pal = q.getpalette()
    idx = np.array(q, dtype=np.int32) + 1
    if maske is not None:
        idx[~np.asarray(maske, bool)] = 0
    farbliste = [(0, 0, 0)] + [tuple(pal[i * 3:i * 3 + 3]) for i in range(farben)]
    return idx.astype(np.uint8), clut_aus_rgb(farbliste, transparent0=True)


def modell(emd, tim):
    """EA.Modell aus EMD + TIM fuer Ansichtsblatt und Buehne."""
    mo = EA.Modell.__new__(EA.Modell)
    dir_off, d = verzeichnis(emd)
    mo.blob, mo.typ, mo.blob_index, mo.blob_len = emd, 0x30, 0, len(emd)
    mo.meshes = EA.md1_parse(emd[d[7]:dir_off])
    mo.skel = EA.emr_parse(emd[d[2]:d[3]])
    mo.clips, mo.frames = EA.edd_parse(emd[d[1]:d[2]])
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(tim)
    mo.texh, mo.texw = mo.tex.shape
    return mo


# ═══════════════════════════════════════════════════ JSON
def speichern(form, pfad):
    with open(pfad, 'w', encoding='utf-8') as f:
        json.dump(form, f, ensure_ascii=False)


def laden(pfad):
    with open(pfad, encoding='utf-8') as f:
        return json.load(f)


# ═══════════════════════════════════════════════════ Probemodell (Werkzeugtest, keine Figur)
PROBE_EMD, PROBE_TIM = 'FORM_PROBE.EMD', 'FORM_PROBE.TIM'


def probe_tim():
    """256 x 64, zwei CLUT-Zeilen: Zeile 0 Bloecke links (Rumpf) und ab x 130 (Kopf, TPage 1), Zeile 1 dieselben Indizes in
    anderen Farben (Arm). Jeder Block traegt ein Gitter und einen transparenten Punkt, damit Abtastung und Farbschluessel
    im Bild erkennbar sind."""
    px = np.zeros((64, 256), dtype=np.uint8)
    for bx, i in ((0, 1), (64, 2), (130, 3), (194, 4)):
        px[:, bx:bx + 60] = i
        px[::8, bx:bx + 60] = 5
        px[:, bx:bx + 60:8] = 5
        px[30:34, bx + 28:bx + 32] = 0
    f0 = [(0, 0, 0), (200, 40, 40), (40, 180, 60), (60, 80, 220), (220, 200, 40), (240, 240, 240)]
    f1 = [(0, 0, 0), (40, 200, 200), (200, 40, 200), (120, 120, 120), (250, 140, 30), (20, 20, 20)]
    return tim_schreiben(px, [clut_aus_rgb(f0), clut_aus_rgb(f1)])


def probe_form():
    """Drei Knochen aus Quadern, am Kopf eine Spitze aus Dreiecken, zwei Clips. Masse sind Werkzeugtest, sie bilden nichts ab."""
    f = neu()
    rumpf = knochen_dazu(f, 'rumpf', -1, (0, -1000, 0))
    kopf = knochen_dazu(f, 'kopf', rumpf, (1100, -300, 0))
    arm = knochen_dazu(f, 'arm', rumpf, (600, 300, 700))
    quader(f, rumpf, (-900, -400, -500), (900, 400, 500), tex=[(0, 0, 59, 59), (64, 0, 123, 59)] * 3)
    quader(f, kopf, (-250, -250, -250), (250, 250, 250), tex=(130, 0, 189, 59))
    netz(f, kopf, [(250, -150, -150), (250, -150, 150), (250, 150, 150), (250, 150, -150), (600, 0, 0)],
         [(0, 1, 4), (1, 2, 4), (2, 3, 4), (3, 0, 4)], [(130, 0), (189, 0), (189, 59), (130, 59), (160, 30)])
    quader(f, arm, (-150, 0, -150), (150, 900, 150), tex=(194, 0, 253, 59), zeile=1)
    h = (0, -1000, 0)
    stehen = [bild(f, wurzel=h), bild(f, wurzel=(0, -1000, 0), winkel={kopf: (0, 200, 0)})]
    wippen = []
    for i in range(32):
        s = math.sin(2 * math.pi * i / 32)
        w = BILD_TREFFER if i == 10 else (ton(3) if i == 20 else 0)
        wippen.append(bild(f, wurzel=(0, -1000 + int(round(60 * s)), 0), spur=(i * 7, 0, 0),
                           winkel={kopf: (0, int(round(300 * s)), 0), arm: (0, 0, int(round(-700 * s)))}, wort=w))
    clip_dazu(f, 'stehen', stehen)
    clip_dazu(f, 'wippen', wippen)
    orientiere(f)
    return f


# ═══════════════════════════════════════════════════ Beweise
def _vergleich(name, a, b):
    if a == b:
        print('  %-6s gleich (%d Byte)' % (name, len(a)))
        return True
    print('  %-6s VERSCHIEDEN (%d gegen %d Byte)' % (name, len(a), len(b)))
    for i in range(min(len(a), len(b))):
        if a[i] != b[i]:
            print('         erste Abweichung bei +0x%x: %02x gegen %02x' % (i, a[i], b[i]))
            break
    return False


def rundlauf(emd, tim=None):
    """Drei Stufen. 1 verlustfrei: Leser -> Schreiber, Sektion fuer Sektion byte-gleich. 2 ueber Bilder: Keyframes wieder je
    Bild ausgeschrieben, der Schreiber legt inhaltsgleiche zusammen; verglichen wird je Bild Inhalt und Wort, und der
    EMR-Kopf bis zu den Keyframes. 3 ueber tex/zeile: rohe UV-Werte in Texel umgerechnet, TPage und CLUT-Wort neu bestimmt."""
    ok = True
    orig = sektionen(emd)
    print('Stufe 1: lese_emd -> schreibe_emd')
    form = lese_emd(emd)
    neu_emd = schreibe_emd(form, pruefen=False)
    neu_sek = sektionen(neu_emd)
    for k in ('morph', 'edd1', 'emr1', 'edd2', 'emr2', 'edd3', 'emr3', 'md1'):
        ok &= _vergleich(k, orig[k], neu_sek[k])
    ok &= _vergleich('Datei', bytes(emd), neu_emd)

    print('Stufe 2: Bilder ausgeschrieben, Pool vom Schreiber')
    f2 = lese_emd(emd)
    pool = f2.pop('pool')
    for c in f2['clips']:
        c['bilder'] = [dict(pool[b['kf']], wort=b['wort']) for b in c['bilder']]
    e2 = schreibe_emd(f2, pruefen=False)
    z2 = lese_emd(e2)
    gleich = len(z2['clips']) == len(form['clips'])
    bilder = abweichend = 0
    if gleich:
        for ca, cb in zip(form['clips'], z2['clips']):
            gleich &= len(ca['bilder']) == len(cb['bilder'])
            for a, b in zip(ca['bilder'], cb['bilder']):
                bilder += 1
                ka, kb = form['pool'][a['kf']], z2['pool'][b['kf']]
                if a['wort'] != b['wort'] or _keyframe_schluessel(ka) != _keyframe_schluessel(kb):
                    abweichend += 1
    print('  %d Bilder, %d abweichend; Pool %d -> %d (inhaltsgleiche Keyframes zusammengelegt)'
          % (bilder, abweichend, len(form['pool']), len(z2['pool'])))
    ok &= gleich and abweichend == 0
    ok &= _vergleich('emr-K', orig['emr1'][:u16(orig['emr1'], 2)], sektionen(e2)['emr1'][:u16(orig['emr1'], 2)])

    print('Stufe 3: UV ueber Texel und CLUT-Zeile')
    f3 = lese_emd(emd)
    tim_kopf = tim_lesen(tim) if tim else None
    clut_y = tim_kopf['clut_y'] if tim_kopf else 480
    for m in f3['meshes']:
        for x in m['dreiecke'] + m['vierecke']:
            seite = x['page'] & 0xF
            x['tex'] = [[u + seite * 128, v] for u, v in x['uv']]
            x['zeile'] = (x['clut'] >> 6) - clut_y
            del x['uv'], x['clut'], x['page']
    f3['tim'] = dict(clut_x=0, clut_y=clut_y, seite=0x80)
    s3 = sektionen(schreibe_emd(f3, pruefen=False))
    ok &= _vergleich('md1', orig['md1'], s3['md1'])

    if tim:
        print('TIM: tim_lesen -> tim_schreiben')
        t = tim_lesen(tim)
        ok &= _vergleich('TIM', bytes(tim), tim_schreiben(t['pixel'], t['cluts'], t['clut_x'], t['clut_y'], t['bild_x'], t['bild_y']))
    fehler, warnungen = pruefe(f3)
    print('Pruefung Stufe 3: %d Fehler, %d Warnungen' % (len(fehler), len(warnungen)))
    for x in fehler + warnungen:
        print('  ' + x)
    print('RUNDLAUF ' + ('GLEICH' if ok else 'ABWEICHUNG'))
    return ok


def info(emd, tim=None):
    form = lese_emd(emd)
    fehler, warnungen = pruefe(form)
    n = len(form['knochen'])
    bilder = sum(len(c['bilder']) for c in form['clips'])
    print('Knochen %d, Meshes %d, Clips %d, Bilder %d, Keyframes %d, Keyframe-Groesse %d, Datei %d Byte'
          % (n, len(form['meshes']), len(form['clips']), bilder, len(form['pool']), keyframe_groesse(n), len(emd)))
    print('Eltern ' + ' '.join(str(k['eltern']) for k in form['knochen']))
    print('Clip-Laengen ' + ' '.join(str(len(c['bilder'])) for c in form['clips']))
    print('Dreiecke %d, Vierecke %d' % (sum(len(m['dreiecke']) for m in form['meshes']), sum(len(m['vierecke']) for m in form['meshes'])))
    if tim:
        t = tim_lesen(tim)
        print('TIM %dx%d, %d CLUT-Zeilen ab y %d' % (t['pixel'].shape[1], t['pixel'].shape[0], len(t['cluts']), t['clut_y']))
    for x in fehler:
        print('FEHLER ' + x)
    for x in warnungen:
        print('Warnung ' + x)
    return not fehler


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sp = ap.add_subparsers(dest='befehl', required=True)
    a = sp.add_parser('rundlauf')
    a.add_argument('--datei', default=os.path.join(EA.REPO, 're15_port', 'shared_assets', 'RE2', 'BAHNSTEIG_G4.EMD'))
    a.add_argument('--tim', default=os.path.join(EA.REPO, 're15_port', 'shared_assets', 'RE2', 'BAHNSTEIG_G4.TIM'))
    a = sp.add_parser('probe')
    a.add_argument('--aus', required=True)
    a = sp.add_parser('info')
    a.add_argument('datei')
    a.add_argument('--tim')
    a = sp.add_parser('bau')
    a.add_argument('form')
    a.add_argument('--emd', required=True)
    a.add_argument('--tim', help='Ziel-TIM; nur mit --bild')
    a.add_argument('--bild', help='RGB(A)-Textur (PNG), wird zu 8 bpp mit einer CLUT-Zeile')
    arg = ap.parse_args()
    if arg.befehl == 'rundlauf':
        roh = open(arg.datei, 'rb').read()
        t = open(arg.tim, 'rb').read() if arg.tim else None
        return 0 if rundlauf(roh, t) else 1
    if arg.befehl == 'probe':
        os.makedirs(arg.aus, exist_ok=True)
        f = probe_form()
        emd = schreibe_emd(f)
        tim = probe_tim()
        for name, daten in ((PROBE_EMD, emd), (PROBE_TIM, tim)):
            with open(os.path.join(arg.aus, name), 'wb') as h:
                h.write(daten)
            print('geschrieben: %s (%d Byte)' % (os.path.join(arg.aus, name), len(daten)))
        return 0 if info(emd, tim) else 1
    if arg.befehl == 'info':
        t = open(arg.tim, 'rb').read() if arg.tim else None
        return 0 if info(open(arg.datei, 'rb').read(), t) else 1
    if arg.befehl == 'bau':
        form = laden(arg.form)
        orientiere(form)
        emd = schreibe_emd(form)
        with open(arg.emd, 'wb') as h:
            h.write(emd)
        tim = None
        if arg.bild and arg.tim:
            from PIL import Image
            bild_ = Image.open(arg.bild).convert('RGBA')
            r = np.array(bild_)
            px, clut = tim_aus_rgb(r[..., :3], r[..., 3] > 127)
            tim = tim_schreiben(px, [clut])
            with open(arg.tim, 'wb') as h:
                h.write(tim)
        return 0 if info(emd, tim) else 1
    return 2


if __name__ == '__main__':
    sys.exit(main())
