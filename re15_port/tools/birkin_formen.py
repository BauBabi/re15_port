#!/usr/bin/env python3
"""Zwei Beta-Weiterentwicklungen des Birkin-Modells EM30 (Typ 0x30) erzeugen.

    python re15_port/tools/birkin_formen.py                 # beide EMDs schreiben
    python re15_port/tools/birkin_formen.py --blatt DIR     # + Ansichtsblaetter nach DIR

Ausgabe (vom PC-Port ueber RE15_BIRKIN_FORM geladen, platform/pc/main.c):
    re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD     RE15_BIRKIN_FORM=hangar
    re15_port/shared_assets/PSX/EMD/EM30_BAHNSTEIG.EMD  RE15_BIRKIN_FORM=bahnsteig

⛔ KEIN BYTE-TRUE-ASSET. Diese beiden Formen existieren auf der ausgelieferten Disc nicht.
Sie sind NUR aus Beta-Screenshots bekannt (Hangar C-2: Birkin packt Elza und schleudert sie,
Bahnsteig: dunkle Form mit Riesenarm vor dem Zugwaggon, bioflames.com). Jede Zahl in FORMEN
unten ist eine GESTALTUNGSENTSCHEIDUNG nach diesen Bildern, kein RE-Wert, und hat deshalb
absichtlich KEINE @0x-Adresse. Sie darf nirgends als Original-Konstante zitiert werden.

Byte-true geerbt (unveraendert aus CDEMD0.EMS Blob 16 = EM30):
    EDD (alle 21 Clips), EMR-Hierarchie, Keyframes (Hangar), Dreieckstopologie, UVs,
    TPages, CLUT-Zuordnung, TIM-Pixel. Nur diese Dinge aendern sich:
      1. MD1-Vertices je Knochen  - skaliert um den Knochenursprung (Mesh-lokaler Raum)
      2. MD1-Normalen             - inverse-transponiert mitgefuehrt, Laenge beibehalten
      3. EMR-Bindoffsets          - Kindgelenk mit der Skalierung des Elternmeshes mitgezogen,
                                    damit die Glieder verbunden bleiben
      4. (faerbe_tim: CLUT-Einfaerbung, derzeit von keiner Form genutzt)
      5. Bahnsteig: alle Keyframes umgehalten (Vierbein, umhalten()) - Clip-Tabelle, Timing,
         Wurzel-X/Z und die Knochen 0/3/6/8 bleiben unveraendert (Ellbogen 11/14
         knicken im Vierbein-Gang der Laufclips 1/0x0E)
      6. Hangar: Schwanz als ANGEHAENGTE Geometrie an Mesh 8 (MD1 waechst, TIM-Offset und
         Verzeichnis verschieben sich; die EM30-Faces bleiben ein unveraendertes Praefix)
    Der MD1-Serializer schreibt unveraendertes EM30 byte-gleich zurueck (Assert in main), und
    re15_emd_parse_container liest beide Dateien ohne Aenderung (tests/unit/test_birkin_formen.c).

Knochen von EM30 (aus dem EMR gelesen, Parent in Klammern):
    0 Becken (Wurzel)   1-3 Bein rechts  4-6 Bein links   7 Bauch (0)   8 Brust (7)
    9 Kopf (8)          10-12 Arm links (8)               13-15 Arm rechts mit Klaue (8)
"""
import argparse, os, struct, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import emd_ansichtsblatt as EA          # noqa: E402  (Leser: EMS-Kette, MD1, EMR, TIM)

AUS_DIR = os.path.join(EA.REPO, 're15_port', 'shared_assets', 'PSX', 'EMD')
TYP     = 0x30

# Gestaltung je Form (siehe Docstring: Nachbildung nach Screenshots, keine RE-Werte).
#   skal[b]  = (sx, sy, sz) im Mesh-lokalen Raum von Knochen b. Y laeuft bei den
#              Gliedmassen entlang des Knochens (Laenge), X/Z ist die Dicke.
#   rel[b]   = (dx, dy, dz) Zusatzverschiebung des Bindoffsets von b NACH dem Mitziehen.
#   tint     = None oder (Matrix 3x3 auf RGB, Helligkeit) fuer alle CLUT-Eintraege.
FORMEN = {
    # Bilder 2-10 (Hangar C-2): der vorhandene Koerper, aber oben massiger. Buckel und
    # Schultern breiter und hoeher, Kopf tiefer zwischen die Schultern gesunken, beide
    # Arme deutlich dicker und etwas laenger (Haende auf Kniehoehe), Beine unveraendert.
    'hangar': dict(
        datei='EM30_HANGAR.EMD',
        titel='Birkin Hangar-Form',
        skal={
            7:  (1.15, 1.10, 1.15),
            8:  (1.30, 1.25, 1.30),
            9:  (1.05, 1.05, 1.05),
            10: (1.35, 1.12, 1.35), 11: (1.40, 1.12, 1.40), 12: (1.30, 1.15, 1.30),
            13: (1.35, 1.12, 1.35), 14: (1.40, 1.12, 1.40), 15: (1.30, 1.15, 1.30),
        },
        rel={9: (60, 170, 0)},
        tint=None,
        # Nutzerhinweis 2026-10-30: "Die Hangar-Form hat auch direkt einen Tail am Ruecken."
        # EM30 hat keinen freien Knochen (EMR-Keyframe = 12 + 16*36 Bit = 84 Byte, ein 17.
        # Knochen passt nicht ohne alle 912 Keyframes umzuschreiben) -> der Schwanz wird als
        # Zusatzgeometrie an Mesh 8 (Brust) gehaengt und bewegt sich starr mit dem Oberkoerper.
        # Mittellinie im Mesh-8-Raum (X vorn, Y unten), Radius je Stuetzpunkt.
        schwanz=dict(
            knochen=8,
            punkte=[(-380, 120, 0), (-760, 420, 60), (-1150, 860, 120), (-1500, 1380, 90),
                    (-1760, 1880, -20), (-1900, 2280, -120), (-1960, 2520, -160)],
            radien=[300, 260, 215, 170, 125, 80, 34],
            seiten=7, unterteilung=3,
            # Fleischtextur links oben (TPage 128, CLUT-Zeile 0 wie der Rumpf, u<128)
            uv=dict(clut=30720, page=128, u=(4, 60), v=(4, 124)),
        ),
    ),
    # Bild 1 (Bahnsteig): die naechste Stufe. Insgesamt wuchtiger,
    # der rechte Arm riesig mit grosser Klaue, der linke dicker, Brust/Buckel stark
    # vergroessert, Kopf fast im Rumpf, Beine kraeftiger.
    'bahnsteig': dict(
        datei='EM30_BAHNSTEIG.EMD',
        titel='Birkin Bahnsteig-Form',
        skal={
            0:  (1.20, 1.00, 1.20),
            1:  (1.30, 1.00, 1.30), 2: (1.30, 1.00, 1.30), 3: (1.20, 1.00, 1.15),
            4:  (1.30, 1.00, 1.30), 5: (1.30, 1.00, 1.30), 6: (1.20, 1.00, 1.15),
            7:  (1.30, 1.15, 1.30),
            8:  (1.50, 1.35, 1.45),
            9:  (1.05, 1.05, 1.05),
            10: (1.35, 1.10, 1.35), 11: (1.40, 1.10, 1.40), 12: (1.30, 1.10, 1.30),
            13: (1.85, 1.10, 1.85), 14: (1.95, 1.10, 1.95), 15: (1.70, 1.20, 1.70),
        },
        rel={9: (90, 260, 0)},
        # Nutzerbefund 2026-10-30: "eher liegend, 4-beinig". Auf der Disc gibt es keine
        # Vierbein-Haltung (EM36-Clips = Knien mit vorgestrecktem Arm), also werden ALLE
        # Keyframes umgehalten (umhalten()): Rumpf vorgekippt, Arme behalten ihre Welt-
        # orientierung (haengen vom vorgeschobenen Schultergurt senkrecht = Vorderbeine),
        # Beine gehockt, Kopf gehoben; danach Bodenkontakt je Keyframe nachgefuehrt.
        # Abgestimmt (Clip 0, Keyframe 0): Fussgelenk 285 / Handgelenke 266 + 393 ueber dem
        # Boden (EM30 stehend: Fussgelenk 264) -> alle vier Gliedmassen tragen; Huefte 1324.
        # Winkel in Grad um die Z-Achse (Seitenachse) im Elternraum: + = Oberseite nach vorn.
        haltung=dict(
            kippen={7: 80, 1: -80, 4: -80, 2: 80, 5: 80},
            welt_halten=(10, 13),
            nachkippen={9: -64, 10: -40, 13: -40, 12: -75, 15: -75},
            # VIERBEIN-GANG (Nutzer 2026-10-30: "Bewegungsanimationen ausdenken, die dazu
            # passen"). Erfunden, aber aus dem EM30-Takt abgeleitet: der menschliche Armschwung
            # laeuft im Original schon diagonal zum Bein (linker Arm mit rechtem Bein) = Trab.
            # In den Laufclips wird er zum Vorderbein-Schritt verstaerkt (Faktor), und in der
            # Vorschwung-Phase knickt der Ellbogen ein (Vorderbein hebt ab, Klaue setzt auf).
            # Clips (Bank 0, EDD dir[1]): 1 = WALK/REPOSITION/CHARGE, 0x0E = RUN-OFF
            # (enemy_ai_common.c re15_birkin_ai_tick, Clip-Zuweisungen @0x80117220/@0x80119b78).
            gang=dict(clips=(0x01, 0x0E), arme={10: 11, 13: 14}, faktor=1.8, anheben=40,
                      fuesse=(3, 6)),
        ),
        # Farbe: wie EM30. Das Violett im Screenshot ist der Videoqualitaet geschuldet
        # (Nutzerbefund 2026-10-30), die Original-Textur bleibt unveraendert.
        tint=None,
    ),
}

u16, u32 = EA.u16, EA.u32


# ═══════════════════════════════════════════════════ MD1: Vertex-/Normalen-Offsets
def md1_bloecke(md1):
    """Je Mesh die Byte-Offsets (relativ zum MD1-Anfang) der Vertex- und Normalenbloecke.
    Gleiches Layout wie EA.md1_parse; Tri- und Quad-Teil koennen auf DENSELBEN Block
    zeigen, darum wird ueber (Offset, Anzahl) dedupliziert."""
    obj, base, out = u32(md1, 8), 12, []
    for i in range(obj // 2):
        (tvo, tvc, tno, tnc, _tfo, _tfc, _tuo,
         qvo, qvc, qno, qnc, _qfo, _qfc, _quo) = struct.unpack_from('<14I', md1, 12 + i*56)
        vb = {(base + tvo, tvc), (base + qvo, qvc)}
        nb = {(base + tno, tnc), (base + qno, qnc)}
        out.append(([b for b in vb if b[1]], [b for b in nb if b[1]]))
    return out


def skaliere_md1(md1, skal):
    """Vertices v -> S v, Normalen n -> S^-1 n (auf die Ursprungslaenge gebracht)."""
    md1 = bytearray(md1)
    gesehen = set()                       # ein Block, den zwei Meshes teilen, nur einmal
    for b, (vbl, nbl) in enumerate(md1_bloecke(md1)):
        if b not in skal:
            continue
        s = np.array(skal[b], dtype=np.float64)
        for off, n in vbl:
            if (off, n) in gesehen:
                continue
            gesehen.add((off, n))
            for k in range(n):
                x, y, z, w = struct.unpack_from('<4h', md1, off + k*8)
                v = np.rint(np.array([x, y, z]) * s).astype(int)
                v = np.clip(v, -32768, 32767)
                struct.pack_into('<4h', md1, off + k*8, int(v[0]), int(v[1]), int(v[2]), w)
        for off, n in nbl:
            if (off, n) in gesehen:
                continue
            gesehen.add((off, n))
            for k in range(n):
                x, y, z, w = struct.unpack_from('<4h', md1, off + k*8)
                alt = np.array([x, y, z], dtype=np.float64)
                laenge = np.linalg.norm(alt)
                if laenge == 0:
                    continue
                neu = alt / s
                neu = neu / np.linalg.norm(neu) * laenge
                neu = np.clip(np.rint(neu).astype(int), -32768, 32767)
                struct.pack_into('<4h', md1, off + k*8, int(neu[0]), int(neu[1]), int(neu[2]), w)
    return bytes(md1)


# ═══════════════════════════════════════════════════ MD1: neu serialisieren + Schwanz
def md1_lesen(md1):
    """MD1 in bearbeitbare Listen. EM30 teilt je Mesh EINEN Vertex- und EINEN Normalenblock
    zwischen Tri- und Quad-Teil (tvo==qvo, tno==qno) - das wird geprueft und beibehalten."""
    ms = EA.md1_parse(md1)
    out = []
    for i, m in enumerate(ms):
        t = struct.unpack_from('<14I', md1, 12 + i*56)
        assert t[0] == t[7] and t[2] == t[9], 'Mesh %d: getrennte Vertexbloecke' % i
        out.append(dict(v=[list(x) for x in m.tv], n=[list(x) for x in m.tn],
                        tf=[list(x) for x in m.tf], tuv=[list(x) for x in m.tuv],
                        qf=[list(x) for x in m.qf], quv=[list(x) for x in m.quv]))
    return out


def md1_schreiben(meshes):
    """Gleiche Blockreihenfolge wie das Original: Tabelle, alle Vertexbloecke, alle
    Normalenbloecke, je Mesh Tri-/Quad-Faces, je Mesh Tri-/Quad-UVs. Unveraendertes EM30
    ergibt damit byte-gleich die Original-MD1 (siehe main --selbsttest)."""
    n = len(meshes)
    cur = n * 56
    vo, no, tfo, qfo, tuo, quo = [], [], [], [], [], []
    for m in meshes:
        vo.append(cur); cur += 8 * len(m['v'])
    for m in meshes:
        no.append(cur); cur += 8 * len(m['n'])
    for m in meshes:
        tfo.append(cur); cur += 12 * len(m['tf'])
        qfo.append(cur); cur += 16 * len(m['qf'])
    uv_start = cur
    for m in meshes:
        tuo.append(cur); cur += 12 * len(m['tf'])
        quo.append(cur); cur += 16 * len(m['qf'])
    out = bytearray(12 + cur)
    struct.pack_into('<III', out, 0, 12 + uv_start, 0, n * 2)
    for i, m in enumerate(meshes):
        nv, nn = len(m['v']), len(m['n'])
        struct.pack_into('<14I', out, 12 + i*56, vo[i], nv, no[i], nn, tfo[i], len(m['tf']), tuo[i],
                         vo[i], nv, no[i], nn, qfo[i], len(m['qf']), quo[i])
        for k, x in enumerate(m['v']):   struct.pack_into('<4h', out, 12 + vo[i] + k*8, *x)
        for k, x in enumerate(m['n']):   struct.pack_into('<4h', out, 12 + no[i] + k*8, *x)
        for k, x in enumerate(m['tf']):  struct.pack_into('<6H', out, 12 + tfo[i] + k*12, *x)
        for k, x in enumerate(m['qf']):  struct.pack_into('<8H', out, 12 + qfo[i] + k*16, *x)
        for k, x in enumerate(m['tuv']): struct.pack_into('<BBHBBHBBH', out, 12 + tuo[i] + k*12, *x)
        for k, x in enumerate(m['quv']): struct.pack_into('<BBHBBHBBHBBH', out, 12 + quo[i] + k*16, *x)
    return bytes(out)


def _catmull(p, n):
    """Catmull-Rom durch die Stuetzpunkte, n Zwischenschritte je Abschnitt."""
    p = [np.array(x, float) for x in p]
    p = [2*p[0] - p[1]] + p + [2*p[-1] - p[-2]]
    out = []
    for i in range(1, len(p) - 2):
        for k in range(n):
            t = k / n
            a, b, c, d = p[i-1], p[i], p[i+1], p[i+2]
            out.append(0.5 * ((2*b) + (-a + c)*t + (2*a - 5*b + 4*c - d)*t*t
                              + (-a + 3*b - 3*c + d)*t*t*t))
    out.append(p[-2])
    return out


def haenge_schwanz_an(meshes, sw):
    """Verjuengte Roehre entlang der Mittellinie + Spitze, in Mesh sw['knochen'].
    Windung wie im Original: cross(v1-v0, v2-v0) zeigt GEGEN die Normale (alle 347 Quads
    von EM30 erfuellen das). Normalen in der Originallaenge 4096."""
    m = meshes[sw['knochen']]
    mitte = _catmull(sw['punkte'], sw['unterteilung'])
    rad = np.interp(np.linspace(0, len(sw['radien']) - 1, len(mitte)),
                    np.arange(len(sw['radien'])), sw['radien'])
    S = sw['seiten']
    uv = sw['uv']
    v0, n0 = len(m['v']), len(m['n'])
    ringe = []
    oben = np.array([0.0, -1.0, 0.0])
    for i, c in enumerate(mitte):
        t = mitte[min(i+1, len(mitte)-1)] - mitte[max(i-1, 0)]
        t /= np.linalg.norm(t)
        a = np.cross(t, oben); a /= np.linalg.norm(a)
        b = np.cross(t, a)
        ring = []
        for k in range(S):
            w = 2*np.pi*k/S
            r = np.cos(w)*a + np.sin(w)*b
            p = c + r*rad[i]
            ring.append(len(m['v']))
            m['v'].append([int(round(x)) for x in p] + [0])
            m['n'].append([int(round(x)) for x in r*4096] + [0])
        ringe.append(ring)
    spitze_dir = mitte[-1] - mitte[-2]; spitze_dir /= np.linalg.norm(spitze_dir)
    spitze = mitte[-1] + spitze_dir * rad[-1] * 3
    sv = len(m['v'])
    m['v'].append([int(round(x)) for x in spitze] + [0])
    m['n'].append([int(round(x)) for x in spitze_dir*4096] + [0])

    def u_(k):  return int(round(uv['u'][0] + (uv['u'][1]-uv['u'][0]) * k / S))
    def v_(i):  return int(round(uv['v'][0] + (uv['v'][1]-uv['v'][0]) * i / (len(ringe)-1)))

    def orient(ia, ib, ic):
        P = [np.array(m['v'][j][:3], float) for j in (ia, ib, ic)]
        N = sum(np.array(m['n'][j][:3], float) for j in (ia, ib, ic))
        return np.cross(P[1]-P[0], P[2]-P[0]) @ N < 0

    for i in range(len(ringe) - 1):
        for k in range(S):
            k2 = (k + 1) % S
            a, b = ringe[i][k], ringe[i][k2]
            c, d = ringe[i+1][k], ringe[i+1][k2]
            uvs = [(u_(k), v_(i)), (u_(k+1), v_(i)), (u_(k), v_(i+1)), (u_(k+1), v_(i+1))]
            if not orient(a, b, c):           # Quad-Split (v0,v1,v2)+(v1,v3,v2): a<->b tauschen
                a, b, c, d = b, a, d, c
                uvs = [uvs[1], uvs[0], uvs[3], uvs[2]]
            m['qf'].append([a - v0 + n0, a, b - v0 + n0, b, c - v0 + n0, c, d - v0 + n0, d])
            m['quv'].append([uvs[0][0], uvs[0][1], uv['clut'], uvs[1][0], uvs[1][1], uv['page'],
                             uvs[2][0], uvs[2][1], 0, uvs[3][0], uvs[3][1], 0])
    letzte = ringe[-1]
    for k in range(S):
        a, b = letzte[k], letzte[(k + 1) % S]
        if not orient(a, b, sv):
            a, b = b, a
        m['tf'].append([a - v0 + n0, a, b - v0 + n0, b, sv - v0 + n0, sv])
        m['tuv'].append([u_(k), uv['v'][1], uv['clut'], u_(k+1), uv['v'][1], uv['page'],
                         (u_(k)+u_(k+1))//2, uv['v'][1], 0])
    assert v0 == n0
    return meshes


# ═══════════════════════════════════════════════════ EMR: Bindoffsets mitziehen
def passe_emr_an(emr, skal, extra):
    """rel[b] liegt im Raum des Eltern-Knochens -> mit dessen Mesh-Skalierung mitziehen."""
    emr = bytearray(emr)
    s = EA.emr_parse(bytes(emr))
    for b in range(s.bone_count):
        p = s.parent[b]
        x, y, z = s.rel[b]
        if p >= 0 and p in skal:
            sx, sy, sz = skal[p]
            x, y, z = x*sx, y*sy, z*sz
        dx, dy, dz = extra.get(b, (0, 0, 0))
        x, y, z = (int(round(c)) for c in (x + dx, y + dy, z + dz))
        struct.pack_into('<3h', emr, 8 + b*6, x, y, z)
    return bytes(emr)


# ═══════════════════════════════════════════════════ TIM: CLUTs einfaerben
def faerbe_tim(tim, tint):
    """15-Bit-CLUT-Eintraege umfaerben. 0x0000 (PSX: voll transparent) und das STP-Bit
    bleiben erhalten; ein Eintrag, der durch das Umfaerben 0 wuerde, bleibt mindestens 1."""
    if tint is None:
        return tim
    mat, hell = tint
    tim = bytearray(tim)
    cs  = u32(tim, 8)
    cw, ch = u16(tim, 16), u16(tim, 18)
    for k in range(cw * ch):
        o = 20 + k*2
        c = u16(tim, o)
        if c == 0:
            continue
        rgb = np.array([c & 31, (c >> 5) & 31, (c >> 10) & 31], dtype=np.float64)
        neu = np.clip(np.rint(mat @ rgb * hell), 0, 31).astype(int)
        w = int(neu[0]) | (int(neu[1]) << 5) | (int(neu[2]) << 10) | (c & 0x8000)
        if w == 0:
            w = 1
        struct.pack_into('<H', tim, o, w)
    assert 20 + cw*ch*2 <= 8 + cs
    return bytes(tim)


# ═══════════════════════════════════════════════════ Zusammenbau
# ═══════════════════════════════════════════════════ EMR: Keyframes umhalten
def _rz(grad):
    a = np.radians(grad)
    return np.array([[np.cos(a), -np.sin(a), 0], [np.sin(a), np.cos(a), 0], [0, 0, 1.0]])


def _zerlege(m):
    """Umkehrung von EA.mat_euler (RotMatrix @0x80068130): m[0][2]=sy, m[1][2]=-cy*sx,
    m[2][2]=cy*cx, m[0][0]=cz*cy, m[0][1]=-sz*cy. Roundtrip-Fehler <= 1/4096 Umdrehung."""
    k = 4096 / (2*np.pi)
    ay = np.arcsin(np.clip(m[0, 2], -1, 1))
    ax = np.arctan2(-m[1, 2], m[2, 2])
    az = np.arctan2(-m[0, 1], m[0, 0])
    return [int(round(a*k)) & 0xFFF for a in (ax, ay, az)]


def _bits_schreiben(buf, start, bitoff, wert):
    for i in range(12):
        bi = start + ((bitoff + i) >> 3)
        mask = 1 << ((bitoff + i) & 7)
        buf[bi] = (buf[bi] | mask) if (wert >> i) & 1 else (buf[bi] & ~mask)


def _welt(skel, rel, lokal, wurzel):
    n = skel.bone_count
    R, T = [None]*n, [None]*n
    for b in range(n):
        p = skel.parent[b]
        if p < 0:
            R[b], T[b] = lokal[b], np.array(wurzel, float)
        else:
            R[b] = R[p] @ lokal[b]
            T[b] = R[p] @ np.array(rel[b], float) + T[p]
    return R, T


def _tiefster_punkt(R, T, verts):
    """Groesstes Y (Y zeigt nach unten) ueber alle Mesh-Vertices = Bodenkontakt."""
    return max(float((v @ R[b].T + T[b])[:, 1].max()) for b, v in verts.items())


def umhalten(vor, d, md1, haltung):
    """Alle Keyframes aller drei Baenke (EMR dir[2]/[4]/[6]) umhalten. Arbeitet auf den
    bereits angepassten Bindoffsets und der skalierten MD1, damit der Bodenkontakt zur
    fertigen Geometrie passt."""
    struktur = EA.emr_parse(bytes(vor[d[2]:d[3]]))
    rel = struktur.rel
    ks  = struktur.kf_size
    verts = {}
    for b, m in enumerate(EA.md1_parse(md1)[:struktur.bone_count]):
        if m.tv:
            verts[b] = np.array([v[:3] for v in m.tv], float)
    gang = _gang_plan(vor, d, struktur, haltung.get('gang'))
    for a, e in ((d[2], d[3]), (d[4], d[5]), (d[6], d[7])):
        kf_off = u16(vor, a + 2)
        n = (e - a - kf_off) // ks
        for k in range(n):
            o = a + kf_off + k*ks
            sk = EA.Skel(); sk.kf_size, sk.kf_data = ks, bytes(vor[o:o+ks])
            winkel = [EA.kf_angles(sk, 0, b) for b in range(struktur.bone_count)]
            wurzel = list(EA.kf_pos(sk, 0))
            lokal  = [EA.mat_euler(*w).astype(float) / 4096 for w in winkel]
            R0, T0 = _welt(struktur, rel, lokal, wurzel)
            # Anteil der Haltung = Aufrichtung des Rumpfs im Original (Y-Achse von Knochen 7
            # gegen die Senkrechte): aufrecht -> voll, liegend (Niederschlag/Tod) -> nichts,
            # sonst knickt ein schon liegender Rumpf um weitere 80 Grad durch.
            f = float(np.clip(R0[7][1, 1], 0.0, 1.0))
            neu = list(lokal)
            for b, g in haltung['kippen'].items():
                neu[b] = _rz(g * f) @ lokal[b]
            # Arme: Weltorientierung wie vorher -> haengen senkrecht vom gekippten Rumpf
            R1, _ = _welt(struktur, rel, neu, wurzel)
            for b in haltung['welt_halten']:
                p = struktur.parent[b]
                neu[b] = R1[p].T @ R0[p] @ lokal[b]
            for b, g in haltung['nachkippen'].items():
                neu[b] = _rz(g * f) @ neu[b]
            geaendert = set(haltung['kippen']) | set(haltung['welt_halten']) | set(haltung['nachkippen'])
            if a == d[2] and k in gang:
                # Vorderbein-Schritt: Zusatzdrehung um die Welt-Seitenachse Z (Laufrichtung +X),
                # danach Ellbogen-Knick in der Vorschwung-Phase.
                for arm, (zusatz, knick) in gang[k].items():
                    R1, _ = _welt(struktur, rel, neu, wurzel)
                    p = struktur.parent[arm]
                    neu[arm] = R1[p].T @ _rz(zusatz) @ R1[arm]
                    ell = haltung['gang']['arme'][arm]
                    neu[ell] = _rz(knick) @ neu[ell]
                    geaendert |= {arm, ell}
            R1, T1 = _welt(struktur, rel, neu, wurzel)
            # Bodenkontakt: im Lauf an den Hinterfuessen (sonst hoebe eine zurueckschwingende
            # Klaue den ganzen Koerper an), sonst am tiefsten Punkt des Modells.
            boden = verts
            if a == d[2] and k in gang:
                boden = {b: verts[b] for b in haltung['gang']['fuesse'] if b in verts}
            wurzel[1] += int(round(_tiefster_punkt(R0, T0, boden) - _tiefster_punkt(R1, T1, boden)))
            struct.pack_into('<h', vor, o + 2, max(-32768, min(32767, wurzel[1])))
            for b in geaendert:
                for i, w in enumerate(_zerlege(neu[b])):
                    _bits_schreiben(vor, o + 12, b*36 + i*12, w)


def _gang_plan(vor, d, struktur, gang):
    """Je Keyframe der Laufclips (Bank 0) und je Vorderbein: (Zusatzwinkel, Ellbogenknick) in
    Grad. Schwung p = Neigung der Original-Armachse in der Laufebene (atan2(x, y), Y = unten);
    der Zusatz verstaerkt die Abweichung vom Clip-Mittel, der Knick folgt der Schwung-
    Geschwindigkeit (nur vorwaerts, normiert auf das Clip-Maximum)."""
    if not gang:
        return {}
    clips, frames = EA.edd_parse(bytes(vor[d[1]:d[2]]))
    a = d[2]
    kf_off, ks = u16(vor, a + 2), struktur.kf_size
    plan = {}
    for c in gang['clips']:
        if c >= len(clips):
            continue
        f0, n = clips[c]
        kfs = [frames[f0 + i] & 0xFFF for i in range(n)]
        for arm in gang['arme']:
            pw = []
            for kf in kfs:
                o = a + kf_off + kf*ks
                sk = EA.Skel(); sk.kf_size, sk.kf_data = ks, bytes(vor[o:o+ks])
                lokal = [EA.mat_euler(*EA.kf_angles(sk, 0, b)).astype(float) / 4096
                         for b in range(struktur.bone_count)]
                R, _ = _welt(struktur, struktur.rel, lokal, EA.kf_pos(sk, 0))
                v = R[arm] @ np.array([0.0, 1.0, 0.0])
                pw.append(np.degrees(np.arctan2(v[0], v[1])))
            pw = np.array(pw)
            mitte = pw.mean()
            ges = np.roll(pw, -1) - np.roll(pw, 1)
            gmax = max(ges.max(), 1e-6)
            for i, kf in enumerate(kfs):
                zusatz = -(gang['faktor'] - 1.0) * (pw[i] - mitte)
                knick = gang['anheben'] * max(0.0, ges[i]) / gmax
                plan.setdefault(kf, {})[arm] = (zusatz, knick)
    return plan


def lade_em30():
    ems = open(EA.EMS_PFAD, 'rb').read()
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(TYP))
    return ems[off:off+ln]


def baue(blob, form):
    """Blob = [u32 dir_off][u32 n=9][Sektionen dir[0..8]][Verzeichnis n*4]. Nur MD1 darf
    durch den Schwanz wachsen; danach verschieben sich TIM-Offset und Verzeichnis."""
    d = EA.emd_dir(blob)
    dir_off, n = u32(blob, 0), u32(blob, 4)
    assert dir_off + n*4 == len(blob) and d[0] == 8 and n == 9
    # Verzeichnis EM30: dir[2]=EMR Bank 0 (Struktur), dir[7]=MD1, dir[8]=TIM
    emr_a, emr_e = d[2], d[3]
    md1_a, md1_e = d[7], d[8]
    vor = bytearray(blob[:md1_a])
    vor[emr_a:emr_e] = passe_emr_an(blob[emr_a:emr_e], form['skal'], form['rel'])
    md1 = skaliere_md1(blob[md1_a:md1_e], form['skal'])
    if form.get('haltung'):
        umhalten(vor, d, md1, form['haltung'])
    if form.get('schwanz'):
        md1 = md1_schreiben(haenge_schwanz_an(md1_lesen(md1), form['schwanz']))
    tim = faerbe_tim(blob[md1_e:dir_off], form['tint'])
    delta = len(md1) - (md1_e - md1_a)
    assert delta % 4 == 0
    neu_dir = d[:8] + [d[8] + delta]
    neu = bytes(vor[8:]) + md1 + tim
    kopf = struct.pack('<II', 8 + len(neu), n)
    return kopf + neu + struct.pack('<%dI' % n, *neu_dir)


def modell_aus_blob(blob):
    """EA.Modell ohne EMS-Zugriff aus einem fertigen Blob aufbauen (fuer das Ansichtsblatt)."""
    mo = EA.Modell.__new__(EA.Modell)
    mo.blob, mo.typ, mo.blob_index, mo.blob_len = blob, TYP, EA.EMS_ORDER.index(TYP), len(blob)
    d = EA.emd_dir(blob)
    mo.meshes            = EA.md1_parse(blob[d[7]:d[8]])
    mo.skel              = EA.emr_parse(blob[d[2]:d[3]])
    mo.clips, mo.frames  = EA.edd_parse(blob[d[1]:d[2]])
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(blob[d[8]:])
    mo.texh, mo.texw     = mo.tex.shape
    return mo


# ═══════════════════════════════════════════════════ Roter Affe (ROOM4080/4081)
# Video 1 4:11-4:19 (farbtreu): der Affe in 4080 ist tief dunkelrot; gemessen an den roten Bildpunkten
# r/g ~ 2.1 gegen r/g ~ 1.7 der EM29-Textur, deren Fell braun-grau ist (r/g ~ 1.2). Gestaltung: das ganze
# Fell nach dunklem Weinrot ziehen (Zielwert r/g ~ 2.5-3, b ~ g), deutlich abdunkeln. Nur die CLUTs aendern sich.
ROT_AFFE = dict(typ=0x29, datei='EM29_ROT.EMD',
                tint=(np.array([[0.70, 0.40, 0.20],
                                [0.08, 0.20, 0.08],
                                [0.08, 0.14, 0.22]]), 0.62))


def baue_rot_affe():
    ems = open(EA.EMS_PFAD, 'rb').read()
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(ROT_AFFE['typ']))
    blob = bytearray(ems[off:off+ln])
    d = EA.emd_dir(bytes(blob))
    dir_off = u32(blob, 0)
    tim_a = d[-1]                                   # letzte Sektion = TIM (dir[8])
    blob[tim_a:dir_off] = faerbe_tim(bytes(blob[tim_a:dir_off]), ROT_AFFE['tint'])
    return bytes(blob)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--blatt', help='Verzeichnis fuer Ansichtsblaetter (PNG)')
    ap.add_argument('--clip', type=lambda s: int(s, 0), default=0)
    ap.add_argument('--aus', default=AUS_DIR, help='Zielverzeichnis der EMDs')
    a = ap.parse_args()

    affe = baue_rot_affe()
    ziel = os.path.join(a.aus, ROT_AFFE['datei'])
    os.makedirs(a.aus, exist_ok=True)
    with open(ziel, 'wb') as f:
        f.write(affe)
    print('geschrieben: %s  (%d Byte, roter Affe)' % (ziel, len(affe)))
    if a.blatt:
        mo = EA.Modell.__new__(EA.Modell)
        mo.blob, mo.typ, mo.blob_index, mo.blob_len = affe, 0x29, EA.EMS_ORDER.index(0x29), len(affe)
        dd = EA.emd_dir(affe)
        mo.meshes = EA.md1_parse(affe[dd[7]:dd[8]]); mo.skel = EA.emr_parse(affe[dd[2]:dd[3]])
        mo.clips, mo.frames = EA.edd_parse(affe[dd[1]:dd[2]])
        mo.tex, mo.pal, mo.clut_y = EA.tim_decode(affe[dd[8]:]); mo.texh, mo.texw = mo.tex.shape
        mo.titel = 'Roter Affe ROOM4080 - abgeleitet von EM29 (Typ 0x29)'
        mo.untertitel = 'NACHBILDUNG nach Video 1 4:11-4:19 - EM29_ROT.EMD - Basis CDEMD0.EMS Blob %d'
        mo.hinweis = 'NICHT byte-true: nur die CLUTs sind umgefaerbt (birkin_formen.py ROT_AFFE). Alles andere = EM29.'
        os.makedirs(a.blatt, exist_ok=True)
        EA.blatt(mo, 0, os.path.join(a.blatt, 'affe_rot.png'))
    basis = lade_em30()
    d = EA.emd_dir(basis)
    roh = basis[d[7]:d[8]]
    assert md1_schreiben(md1_lesen(roh)) == roh, 'MD1-Serializer nicht byte-gleich zu EM30'
    os.makedirs(a.aus, exist_ok=True)
    for name, form in FORMEN.items():
        blob = baue(basis, form)
        ziel = os.path.join(a.aus, form['datei'])
        with open(ziel, 'wb') as f:
            f.write(blob)
        print('geschrieben: %s  (%d Byte, Form %s)' % (ziel, len(blob), name))
        if a.blatt:
            os.makedirs(a.blatt, exist_ok=True)
            mo = modell_aus_blob(blob)
            mo.titel      = '%s - abgeleitet von EM30 (Typ 0x30)' % form['titel']
            mo.untertitel = 'NACHBILDUNG nach Beta-Screenshots - ' + form['datei'] + ' - Basis CDEMD0.EMS Blob %d'
            mo.hinweis    = ('NICHT byte-true: Vertex-/Normalen-Skalierung, Bindoffsets und Farben sind Gestaltung '
                             '(birkin_formen.py FORMEN)' + (', Schwanz angehaengt an Mesh 8' if form.get('schwanz') else '')
                             + (', Keyframes umgehalten (Vierbein)' if form.get('haltung') else '')
                             + '. Unveraendert aus EM30: Topologie, UVs, Skelett-Hierarchie, Clip-Tabelle.')
            EA.blatt(mo, a.clip,
                     os.path.join(a.blatt, 'birkin_%s.png' % name))


if __name__ == '__main__':
    main()
