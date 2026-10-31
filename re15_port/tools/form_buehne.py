#!/usr/bin/env python3
"""Offline-Buehne der Bahnsteig-Form: EMD + TIM in beliebiger Pose durch eine Kamera, ueber dem Hintergrund, mit Vergleichsblatt.

    python re15_port/tools/form_buehne.py --emd X.EMD --tim X.TIM --clip 5 --bild 0 --lage 1500,0,-23000,0 \
        --raum 5090 --schnitt 11 --original analysis/befunde_runde41/form/bild2.png --aus blatt.png
    python re15_port/tools/form_buehne.py --emd X.EMD --tim X.TIM --clip 5 --bild 0 --lage 1500,0,-23000,0 \
        --kamera analysis/befunde_runde41/form/kamera_bild2.json --aus blatt.png

    python re15_port/tools/form_buehne.py ... --rundum 8 --aus rundum.png        # acht Gierwinkel
    python re15_port/tools/form_buehne.py ... --clip 5 --streifen 6 --aus film.png   # jedes sechste Bild des Clips

Als Bibliothek: buehne(emd, tim, clip, bild, lage, gier, kam, licht, hintergrund) -> (Bild, Maske), Kamera.aus_rid(), Raum(), deckung().

Blatt: Original | Render | Render durch video_filter (form_video.py, falls vorhanden) | Original mit Umriss.
Hintergrund: --hintergrund PNG oder, mit --raum, das BSS-Bild des Schnitts (Werkzeug probe_bg_dump, Zwischenablage build/form_bg). Fuer eine
Vorlage ohne Raum im Spiel (Bahnsteig) dient die Vorlage selbst als Hintergrund, Licht dann mit --licht-raum/--licht-schnitt oder --kein-licht.
--spiel BILD.png legt ein Fensterbild aus form_spiel.sh daneben und misst die Deckung (bester Versatz, Farbabstand).

Nachbau des Port-Zeichenwegs, Stelle fuer Stelle (nur fuer Typ 0x30 ohne Teilschicht, Tinte, Gore, Skalierung):
    Pose             EA.Modell.pose = re15_skel_compute_pose
    Welt             Y(gier) = [c 0 s; 0 1 0; -s 0 c] (platform/pc/main.c nyaw), T = Y*T_knochen + Lage
    Blick            V, t aus RID pos/target (camera_common.c re15_camera_build_view, ganzzahlig; isqrt statt BIOS-SquareRoot0)
    Projektion       sx = 160 + (x * n) >> 16, n = GTE-Division H/z, H = fov >> 7, Eckpunkte ganzzahlig; z < 64 verwirft die Flaeche
    Rueckseiten      re15_nclip_r41_weg: Dreieck MAC0 <= 0, Viereck MAC0 < 0
    Vierecke         zwei Dreiecke a,b,d und a,d,c
    Reihenfolge      nach mittlerer Tiefe, fern zuerst (render_pc.c Tiefensortierung)
    Licht            re15_light_setup_actor an der Aktorlage + NCCT je Ecke, Gouraud; psx_prim_to_sdl_vert, Textur * Farbe / 255
    Textur           Texel-x = u + (page & 0xF) * 128, CLUT-Zeile aus dem CLUT-Wort, 0x0000 transparent, affin wie SDL_RenderGeometry
    Abtastphase      +0.375 logische Pixel (include/re15_abtastphase.h)
NICHT nachgebaut: Vordergrundmasken (sprite.pri) und alle anderen Figuren; die Buehne zeigt nur die Form.
Fuer die Bildvorlagen gilt: die Kameradatei bestimmt, wie Port-Pixel auf 640x480 liegen (abbildung).
"""
import argparse, json, math, os, struct, subprocess, sys
import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import emd_ansichtsblatt as EA          # noqa: E402
import form_emd as FE                   # noqa: E402

REPO = EA.REPO
PSX = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')
BG_CACHE = os.path.join(REPO, 're15_port', 'build', 'form_bg')
BG_EXE = os.path.join(REPO, 're15_port', 'build', 'tests', 'unit', 'probe_bg_dump.exe')
OFX, OFY = 160, 120          # main.c:2184 OFX/OFY
PHASE = 0.375                # include/re15_abtastphase.h RE15_UV_ABTASTPHASE


# ═══════════════════════════════════════════════════ Raum: RID + Licht
class Raum:
    """RDT-Kopf: Byte 1 = nCut, @0x24 RID (32 Byte je Cut), @0x2C Licht (40 Byte je Cut, re15_light.h)."""

    def __init__(self, raum):
        self.raum = raum
        self.pfad = os.path.join(PSX, 'STAGE%s' % raum[0], 'ROOM%s.RDT' % raum)
        self.b = open(self.pfad, 'rb').read()
        self.n = self.b[1]
        self.rid, self.licht_off = EA.u32(self.b, 0x24), EA.u32(self.b, 0x2C)

    def schnitt(self, i):
        fl, fov, px, py, pz, tx, ty, tz, pri = struct.unpack_from('<HH6iI', self.b, self.rid + i * 32)
        return dict(fov=fov, pos=(px, py, pz), ziel=(tx, ty, tz))

    def licht(self, i):
        p = self.licht_off + i * 40
        b = self.b
        return dict(skala=b[p], typ=tuple(b[p + 1:p + 4]),
                    farbe=[tuple(b[p + 4 + k * 3:p + 7 + k * 3]) for k in range(3)], umgebung=tuple(b[p + 13:p + 16]),
                    lage=[struct.unpack_from('<3h', b, p + 0x10 + k * 6) for k in range(3)],
                    hell=struct.unpack_from('<3H', b, p + 0x22))


def licht_kontext(c, aktor):
    """re15_light_setup_actor mit rot = NULL: Weltrichtungen L (Q12 * Skala), Farben C (<< 4), Umgebung. Normierung als
    Gleitkomma statt re15_vector_normal (Abweichung unter 1/1000)."""
    L = np.zeros((3, 3), dtype=np.int64)
    C = np.zeros((3, 3), dtype=np.int64)
    for i in range(3):
        if c['typ'][i] == 0:
            d = [(int(aktor[k]) - int(c['lage'][i][k])) >> 4 for k in range(3)]
            rng = c['hell'][i] >> 4
            if rng <= 0 or c['hell'][i] == 0:
                continue
            ab = rng - math.isqrt(d[0] * d[0] + d[2] * d[2])
            if ab <= 0:
                continue
            f = [c['farbe'][i][k] * ab // rng for k in range(3)]
        else:
            d = list(c['lage'][i])
            f = list(c['farbe'][i])
        if not any(f) or not any(d):
            continue
        n = np.array(d, float)
        n = np.rint(n / np.linalg.norm(n) * 4096.0).astype(np.int64)
        L[i] = -c['skala'] * n
        C[i] = np.array(f, dtype=np.int64) << 4
    return L, C, np.array(c['umgebung'], dtype=np.int64)


def ncct(L_lokal, C, umgebung, N):
    """re15_light_shade_vertex fuer ein Normalenfeld (n x 3): Ergebnis im PSX-Primitivraum (0x80 = neutral)."""
    d = np.clip((L_lokal @ N.T) >> 12, 0, 0x7FFF)                     # 3 x n
    akk = (umgebung[:, None] << 16) + (C.T @ d)                       # 3 x n
    ir = np.clip(akk >> 12, 0, 0x7FFF)
    aus = np.clip((((128 * ir) << 4) >> 12) >> 4, 0, 255)
    return aus.T                                                      # n x 3


def prim_zu_sdl(v):
    """render_pc.c psx_prim_to_sdl_vert."""
    return np.minimum((v * 0xFF + 0x40) // 0x80, 0xFF)


# ═══════════════════════════════════════════════════ Kamera
class Kamera:
    def __init__(self, V, t, h, abbildung=None, name=''):
        self.V = np.asarray(V, dtype=np.int64).reshape(3, 3)
        self.t = np.asarray(t, dtype=np.int64)
        self.h = int(h)
        self.abbildung = abbildung or dict(ax=2.0, bx=0.0, ay=2.0, by=0.0)
        self.name = name

    @classmethod
    def aus_rid(cls, pos, ziel, fov=None, h=None, abbildung=None, name=''):
        """re15_camera_build_view: abschneidende Ganzzahlrechnung (FUN_80053ca4)."""
        px, py, pz = (int(c) for c in pos)
        dx, dy, dz = int(ziel[0]) - px, int(ziel[1]) - py, int(ziel[2]) - pz
        dist = math.isqrt(dx * dx + dy * dy + dz * dz)
        hz = math.isqrt(dx * dx + dz * dz)

        def tdiv(a, b):
            q = abs(a) // abs(b)
            return q if (a < 0) == (b < 0) else -q

        sp, cp = tdiv(-dy * 4096, dist), tdiv(hz * 4096, dist)
        if hz:
            sy, cy = tdiv(dx * 4096, hz), tdiv(dz * 4096, hz)
            V = [cy, 0, -sy, (sp * sy) >> 12, cp, (sp * cy) >> 12, (cp * sy) >> 12, -sp, (cp * cy) >> 12]
        else:
            V = [4096, 0, 0, 0, cp, sp, 0, -sp, cp]
        v = np.array(V, dtype=np.int64).reshape(3, 3)
        t = (v @ np.array([-px, -py, -pz], dtype=np.int64)) >> 12
        return cls(v, t, h if h is not None else (int(fov) >> 7), abbildung, name)


def kamera_laden(pfad):
    """kamera_bildN.json aus form_kamera.py. Erwartet pos/ziel und fov oder h, oder V/t/h, flach oder unter `kamera`; optional abbildung."""
    with open(pfad, encoding='utf-8') as f:
        j = json.load(f)
    ab = j.get('abbildung')
    nm = j.get('name') or os.path.basename(pfad)
    j = j.get('kamera', j)
    if 'V' in j:
        return Kamera(j['V'], j['t'], j['h'], ab, nm)
    return Kamera.aus_rid(j['pos'], j.get('ziel', j.get('target')), j.get('fov'), j.get('h'), ab, nm)


# ═══════════════════════════════════════════════════ Textur
class Textur:
    """Je CLUT-Zeile ein RGBA-Abbild der 256 Eintraege (re15_tim_rgb555_to_argb8888: << 3, 0x0000 transparent)."""

    def __init__(self, tim):
        t = FE.tim_lesen(tim)
        self.px, self.clut_y = t['pixel'], t['clut_y']
        self.h, self.w = self.px.shape
        c = np.array(t['cluts'], dtype=np.int64)
        self.lut = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3, (c != 0) * 255], axis=-1).astype(np.uint8)


# ═══════════════════════════════════════════════════ Geometrie -> Dreiecke
def _gte(vx, vy, vz, h):
    """RTPS: n = H/z (Festkomma 16), x = OFX + (x * n) >> 16; Eckpunkt ganzzahlig wie im Port."""
    z = np.clip(vz, 1, 0xFFFF)
    n = np.minimum(((h * 0x20000) // z + 1) // 2, 0x1FFFF)
    sx = OFX + ((np.clip(vx, -0x8000, 0x7FFF) * n) >> 16)
    sy = OFY + ((np.clip(vy, -0x8000, 0x7FFF) * n) >> 16)
    return sx, sy


def _mac0(p0, p1, p2):
    return p0[0] * p1[1] + p1[0] * p2[1] + p2[0] * p0[1] - p0[0] * p2[1] - p1[0] * p0[1] - p2[0] * p1[1]


def dreiecke(mo, kf, lage, gier, kam, licht=None, teile=None):
    """Alle sichtbaren Dreiecke als (Tiefe, Bild-xy[3], uv[3], CLUT-Zeile, Farbe[3]), ohne Sortierung."""
    s, c = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], dtype=np.int64)
    R, T = mo.pose(kf)
    lage = np.array(lage, dtype=np.int64)
    ctx = licht_kontext(licht, lage) if licht else None
    aus = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        if teile is not None and b not in teile:
            continue
        m = mo.meshes[b]
        W = (Y @ R[b]) >> 12
        t = ((Y @ T[b]) >> 12) + lage
        VW = (kam.V @ W) >> 12
        vT = ((kam.V @ t) >> 12) + kam.t
        for (verts, norms, faces, uvs, viereck) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = np.array([x[:3] for x in verts], dtype=np.int64)
            q = (VW @ v.T) >> 12
            vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
            sx, sy = _gte(vx, vy, vz, kam.h)
            if ctx is not None and norms:
                L = (W.T @ ctx[0].T).T >> 12            # je Zeile W^T * L
                f = prim_zu_sdl(ncct(L, ctx[1], ctx[2], np.array([x[:3] for x in norms], dtype=np.int64)))
            else:
                f = np.full((max(len(norms), 1), 3), 255, dtype=np.int64)
            for g, uv in zip(faces, uvs):
                ie = g[1::2]
                inn = g[0::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = _mac0(p[0], p[1], p[2])
                if (m0 < 0) if viereck else (m0 <= 0):
                    continue
                po = (uv[5] & 0xF) * 128
                zeile = (uv[2] >> 6) & 0x1FF
                if viereck:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    for a, bb, cc in ((0, 1, 3), (0, 3, 2)):
                        tief = (int(vz[ie[a]]) + int(vz[ie[bb]]) + int(vz[ie[cc]])) / 3.0
                        aus.append((tief, (p[a], p[bb], p[cc]), (tx[a], tx[bb], tx[cc]), zeile,
                                    (f[inn[a]], f[inn[bb]], f[inn[cc]])))
                else:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    tief = (int(vz[ie[0]]) + int(vz[ie[1]]) + int(vz[ie[2]])) / 3.0
                    aus.append((tief, tuple(p), tuple(tx), zeile, (f[inn[0]], f[inn[1]], f[inn[2]])))
    return aus


def abbilden(x, y, ab):
    """Port-Pixel -> Zielbild. ab = {ax, bx, ay, by} oder {matrix: 3 x 3} (projektiv, Spaltenvektor x, y, 1)."""
    if 'matrix' in ab:
        m = np.asarray(ab['matrix'], float).reshape(3, 3)
        w = m[2, 0] * x + m[2, 1] * y + m[2, 2]
        return (m[0, 0] * x + m[0, 1] * y + m[0, 2]) / w, (m[1, 0] * x + m[1, 1] * y + m[1, 2]) / w
    return x * ab['ax'] + ab['bx'], y * ab['ay'] + ab['by']


def rastern(tris, tex, ziel, abbildung):
    """Fern zuerst in `ziel` (H x W x 3 uint8) zeichnen; Rueckgabe = Maske der getroffenen Pixel."""
    H, W = ziel.shape[:2]
    maske = np.zeros((H, W), bool)
    for _t, p, uv, zeile, farbe in sorted(tris, key=lambda t: -t[0]):
        x, y = abbilden(np.array([q[0] + PHASE for q in p], float), np.array([q[1] + PHASE for q in p], float), abbildung)
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0 = max(int(np.floor(x.min())), 0); x1 = min(int(np.ceil(x.max())) + 1, W)
        y0 = max(int(np.floor(y.min())), 0); y1 = min(int(np.ceil(y.max())) + 1, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
        w0 = 1.0 - w1 - w2
        innen = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not innen.any():
            continue
        u = np.array([q[0] for q in uv], float)
        v = np.array([q[1] for q in uv], float)
        uu = np.clip((w0 * u[0] + w1 * u[1] + w2 * u[2]).astype(int), 0, tex.w - 1)
        vv = np.clip((w0 * v[0] + w1 * v[1] + w2 * v[2]).astype(int), 0, tex.h - 1)
        z = zeile - tex.clut_y
        z = z if 0 <= z < tex.lut.shape[0] else 0
        rgba = tex.lut[z][tex.px[vv, uu]]
        tref = innen & (rgba[..., 3] > 0)
        if not tref.any():
            continue
        c0, c1, c2 = (np.asarray(k, float) for k in farbe)
        farb = w0[..., None] * c0 + w1[..., None] * c1 + w2[..., None] * c2
        rgb = np.clip(rgba[..., :3].astype(float) * farb / 255.0, 0, 255).astype(np.uint8)
        s = ziel[y0:y1, x0:x1]
        s[tref] = rgb[tref]
        ms = maske[y0:y1, x0:x1]
        ms[tref] = True
    return maske


# ═══════════════════════════════════════════════════ Hintergrund
def bild_laden(pfad, breite=640, hoehe=480):
    b = Image.open(pfad).convert('RGB')
    if b.size != (breite, hoehe):
        b = b.resize((breite, hoehe), Image.NEAREST)
    return np.array(b)


def hintergrund_bss(raum, schnitt):
    """MDEC-Bild des Schnitts ueber das Werkzeug probe_bg_dump (Engine-Dekoder, Scheibe = Schnitt). None = nicht verfuegbar."""
    name = 'ROOM%s%02d.ppm' % (raum[:3], schnitt)
    pfad = os.path.join(BG_CACHE, name)
    if not os.path.exists(pfad) and os.path.exists(BG_EXE):
        os.makedirs(BG_CACHE, exist_ok=True)
        subprocess.run([BG_EXE, PSX, BG_CACHE, raum[0]], check=False, stdout=subprocess.DEVNULL)
    return pfad if os.path.exists(pfad) else None


def video_filter_holen():
    try:
        import form_video
    except ImportError:
        return None
    return getattr(form_video, 'video_filter', None)


# ═══════════════════════════════════════════════════ Blatt
def umriss(maske):
    m = maske
    k = m & ~(np.roll(m, 1, 0) & np.roll(m, -1, 0) & np.roll(m, 1, 1) & np.roll(m, -1, 1))
    return k


def blatt(tafeln, ziel, kopf):
    """tafeln = Liste (Titel, H x W x 3). Eine Zeile, Beschriftung unter jeder Tafel."""
    h, w = tafeln[0][1].shape[:2]
    pad, zeile = 12, 28
    b = Image.new('RGB', (pad + len(tafeln) * (w + pad), pad + 26 + h + zeile), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((pad, pad // 2), kopf, font=EA.font(16), fill=(170, 173, 180))
    for i, (titel, a) in enumerate(tafeln):
        x = pad + i * (w + pad)
        b.paste(Image.fromarray(a), (x, pad + 26))
        d.text((x, pad + 26 + h + 4), titel, font=EA.font(16, True), fill=(214, 138, 74))
    os.makedirs(os.path.dirname(os.path.abspath(ziel)), exist_ok=True)
    b.save(ziel)


def deckung(render, maske, spiel, suche=6):
    """Render gegen das echte Fensterbild: bester ganzzahliger Versatz ueber die Pixel der Figur, mittlerer Kanalabstand.
    Gemessen wird nur innerhalb der Maske; Effekte und andere Figuren im Spielbild liegen ausserhalb oder treiben den Abstand."""
    ys, xs = np.nonzero(maske)
    a = render[ys, xs].astype(int)
    H, W = spiel.shape[:2]
    best = None
    for dy in range(-suche, suche + 1):
        for dx in range(-suche, suche + 1):
            yy, xx = ys + dy, xs + dx
            ok = (yy >= 0) & (yy < H) & (xx >= 0) & (xx < W)
            d = float(np.abs(a[ok] - spiel[yy[ok], xx[ok]].astype(int)).mean())
            if best is None or d < best[0]:
                best = (d, dx, dy)
            if dx == 0 and dy == 0:
                d0 = d
    return dict(dx=best[1], dy=best[2], abstand=best[0], abstand0=d0, pixel=len(xs))


def bild_von_clip(mo, clip, bild):
    f0, n = mo.clips[clip]
    if not 0 <= bild < n:
        raise SystemExit('Clip %d hat %d Bilder' % (clip, n))
    return mo.frames[f0 + bild] & 0xFFF


def buehne(emd, tim, clip, bild, lage, gier, kam, licht=None, hintergrund=None, breite=640, hoehe=480, kf=None):
    """Bibliotheksaufruf: -> (Render H x W x 3, Maske). hintergrund = H x W x 3 oder None (dunkelgrau)."""
    mo = FE.modell(emd, tim)
    tex = Textur(tim)
    k = kf if kf is not None else bild_von_clip(mo, clip, bild)
    ziel = np.array(hintergrund, copy=True) if hintergrund is not None else np.full((hoehe, breite, 3), 40, np.uint8)
    maske = rastern(dreiecke(mo, k, lage, gier, kam, licht), tex, ziel, kam.abbildung)
    return ziel, maske


def montage(bilder, spalten, ziel, kopf):
    """Gleich grosse Bilder (Beschriftung, H x W x 3) als Raster; dient Rundblick und Filmstreifen."""
    h, w = bilder[0][1].shape[:2]
    pad, zeile = 8, 20
    zeilen = (len(bilder) + spalten - 1) // spalten
    b = Image.new('RGB', (pad + spalten * (w + pad), 30 + zeilen * (h + zeile + pad)), (18, 19, 22))
    d = ImageDraw.Draw(b)
    d.text((pad, 6), kopf, font=EA.font(15), fill=(170, 173, 180))
    for i, (t, a) in enumerate(bilder):
        x, y = pad + (i % spalten) * (w + pad), 30 + (i // spalten) * (h + zeile + pad)
        b.paste(Image.fromarray(a), (x, y))
        d.text((x, y + h + 2), t, font=EA.font(13), fill=(214, 138, 74))
    os.makedirs(os.path.dirname(os.path.abspath(ziel)), exist_ok=True)
    b.save(ziel)


def ausschnitt(bilder):
    """Alle Renderbilder auf die gemeinsame Bbox der Figur schneiden, 16 Pixel Rand."""
    m = np.zeros(bilder[0][2].shape, bool)
    for _t, _a, ma in bilder:
        m |= ma
    ys, xs = np.nonzero(m)
    if not len(xs):
        return [(t, a) for t, a, _ in bilder]
    y0, y1 = max(ys.min() - 16, 0), ys.max() + 17
    x0, x1 = max(xs.min() - 16, 0), xs.max() + 17
    return [(t, a[y0:y1, x0:x1]) for t, a, _ in bilder]


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0],
                                 formatter_class=argparse.RawDescriptionHelpFormatter, epilog=__doc__)
    ap.add_argument('--emd', required=True)
    ap.add_argument('--tim', required=True)
    ap.add_argument('--clip', type=int, default=0)
    ap.add_argument('--bild', type=int, default=0)
    ap.add_argument('--kf', type=int, help='Keyframe direkt statt Clip/Bild')
    ap.add_argument('--lage', default='0,0,0,0', help='x,y,z,gier des Aktors')
    ap.add_argument('--pose', help='x,z,gier,clip,bild,schnitt wie form_spiel.sh; ersetzt --lage, --clip, --bild, --schnitt')
    ap.add_argument('--kamera', help='kamera_bildN.json')
    ap.add_argument('--raum', help='z.B. 5090: RID und Licht aus dem RDT')
    ap.add_argument('--schnitt', type=int, default=0)
    ap.add_argument('--licht-raum', help='Licht aus diesem Raum, Standard --raum')
    ap.add_argument('--licht-schnitt', type=int, help='Licht aus diesem Schnitt, Standard --schnitt')
    ap.add_argument('--kein-licht', action='store_true', help='neutral 0x80 statt NCCT')
    ap.add_argument('--hintergrund', help='PNG; ohne Angabe BSS des Schnitts, sonst grau')
    ap.add_argument('--original', help='Vorlage (PNG, 640x480)')
    ap.add_argument('--spiel', help='Fensterbild aus form_spiel.sh; Deckung wird gemessen')
    ap.add_argument('--einzel', help='Render zusaetzlich einzeln (PNG)')
    ap.add_argument('--rundum', type=int, help='statt des Blatts: N Ansichten, Gier in gleichen Schritten um die Figur')
    ap.add_argument('--streifen', type=int, help='statt des Blatts: jedes K-te Bild des Clips')
    ap.add_argument('--aus', required=True, help='Vergleichsblatt (PNG)')
    a = ap.parse_args()

    emd = open(a.emd, 'rb').read()
    tim = open(a.tim, 'rb').read()
    x, y, z, gier = (int(v) for v in a.lage.split(','))
    if a.pose:
        x, z, gier, a.clip, a.bild, a.schnitt = (int(v) for v in a.pose.split(','))
        y = 0                                           # RE15_BB_POSE setzt e->y = 0
    raum = Raum(a.raum) if a.raum else None
    if a.kamera:
        kam = kamera_laden(a.kamera)
    elif raum:
        s = raum.schnitt(a.schnitt)
        kam = Kamera.aus_rid(s['pos'], s['ziel'], s['fov'], name='%s Schnitt %d' % (a.raum, a.schnitt))
    else:
        raise SystemExit('--kamera oder --raum noetig')
    licht = None
    lr = Raum(a.licht_raum) if a.licht_raum else raum
    if lr and not a.kein_licht:
        licht = lr.licht(a.licht_schnitt if a.licht_schnitt is not None else a.schnitt)

    hg_pfad = a.hintergrund or (hintergrund_bss(a.raum, a.schnitt) if a.raum else None)
    hg = bild_laden(hg_pfad) if hg_pfad else None
    if a.rundum or a.streifen:
        reihe = []
        if a.rundum:
            for i in range(a.rundum):
                g = (gier + i * 4096 // a.rundum) & 0xFFF
                r, m = buehne(emd, tim, a.clip, a.bild, (x, y, z), g, kam, licht, hg, kf=a.kf)
                reihe.append(('Gier %d' % g, r, m))
        else:
            n = FE.modell(emd, tim).clips[a.clip][1]
            for b in range(0, n, a.streifen):
                r, m = buehne(emd, tim, a.clip, b, (x, y, z), gier, kam, licht, hg)
                reihe.append(('Clip %d Bild %d' % (a.clip, b), r, m))
        montage(ausschnitt(reihe), 6, a.aus, 'form_buehne  %s  %s' % (os.path.basename(a.emd), kam.name))
        print('Blatt: ' + a.aus)
        return 0
    render, maske = buehne(emd, tim, a.clip, a.bild, (x, y, z), gier, kam, licht, hg, kf=a.kf)
    if a.einzel:
        Image.fromarray(render).save(a.einzel)
    ys, xs = np.nonzero(maske)
    if len(xs):
        print('Render: %d Pixel, Bbox x %d..%d y %d..%d (640x480)' % (len(xs), xs.min(), xs.max(), ys.min(), ys.max()))
    else:
        print('Render: nichts im Bild')

    tafeln = []
    if a.original:
        tafeln.append(('Original ' + os.path.basename(a.original), bild_laden(a.original)))
    tafeln.append(('Render clip %d bild %d, %s' % (a.clip, a.bild, kam.name), render))
    vf = video_filter_holen()
    if vf:
        tafeln.append(('Render durch video_filter', np.asarray(vf(render), dtype=np.uint8)))
    if a.original:
        o = bild_laden(a.original).copy()
        o[umriss(maske)] = (255, 255, 0)
        tafeln.append(('Original mit Umriss des Renders', o))
    if a.spiel:
        sp = bild_laden(a.spiel)
        tafeln.append(('echtes Fenster', sp))
        v = deckung(render, maske, sp)
        print('Deckung Buehne/Spiel: Versatz %d,%d Pixel, mittlerer Abstand %.1f je Kanal (0-255) ueber %d Pixel; ohne Versatz %.1f'
              % (v['dx'], v['dy'], v['abstand'], v['pixel'], v['abstand0']))
    kopf = 'form_buehne  %s  Lage %d,%d,%d Gier %d  H=%d  Licht %s' % (os.path.basename(a.emd), x, y, z, gier, kam.h,
                                                                       'aus' if not licht else '%s/%d' % (lr.raum, a.licht_schnitt if a.licht_schnitt is not None else a.schnitt))
    blatt(tafeln, a.aus, kopf)
    print('Blatt: ' + a.aus)
    return 0


if __name__ == '__main__':
    sys.exit(main())
