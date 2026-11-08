"""SF V6 (Bauer): Kopf der Sichel-Form = ORIGINAL RE1.5-Birkin-Kopf (EM30 = CDEMD0.EMS Blob 16, Mesh 9) mit seiner Original-Textur
und -Palette; nur die Kopftextur erweitert (grauer Fleck, Hellgrau ueber dem Mund).

    python kopf_orig_bau.py [--aus DIR] [--json PROT] [--stufe 0|1]   -> DIR/EM30_V5k.EMD (+ EM30_V4.EMD), Protokoll
        --stufe 0: Original-Kopf unveraendert (Spalte "ORIGINAL" der Leisten, Datei EM30_V6orig.EMD, EM30_V4.EMD bleibt)

Nutzer 2026-11-08 (woertlich, bindend, geht allen frueheren Kopf-Vorgaben vor): "Für die Textur anders - das was haare sind, sind
offenbar keine Haare. Orientiere dich an den Original Birkin in Resident Evil 1.5. Der hat auch schon links in seinen Gesicht diesen
grauen Fleck. Der scheint bei den mutierten Modell einfach nur erweitert zu sein. Das Hellgrau in der Mitte auch, so dass der Mund eher
geschlossen ist. Also nutze das Original Resident Evil 1.5 Birkin Modell als Ausgangsgrundlage, damit solltest du nicht viel falsch
machen." und "Immerhin ist dieses Modell ja eine Mutation/Weiterentwicklung des anderen Models! Dementsprechend muss ein Teil des
Vormodells erkenntbar drin sein."

Herkunft (byte-true, gemessen sfv6/p1.py-p3.py):
  * Original EM30 = CDEMD0.EMS Eintrag 16 (226404 Byte, sha256 569e3138...). Mesh 9: 61 Ecken, 61 Normalen, 73 Dreiecke, 18 Vierecke,
    TPage 128, CLUT-Zeile 480 (= Zeile 0 der EM30-TIM), Texel u 56-127 v 136-254 (6501 Texel; 4 davon teilen die Meshes 10 und 13).
  * Referenz 2 (EM30_HANGAR.EMD, cfd1ac42) ist NICHT bytegleich zum Original: Ecken = round(1,05 x Original) (FORMEN skal 9), UVs auf das
    Wundfleisch u 66-126 v 4-90 umgelegt, CLUT-Zeile 485 (Kopf-Zeile aus Zeile 480, Haut-Transformation); Zeilen 480-482 sind die
    Haut-Transformation der Referenz 2 (nicht Original). Die Original-Kopftexel u 56-127 v 136-254 sind in Referenz 2 unveraendert
    (6501 von 6501 gleich), in V5 Runde 4 zu 85 % vom G2-Kopf ueberschrieben.
Bau (Basis V5 Runde 4 = EM30_V5r4.EMD, sha256 38562ced; Koerper 0-8/10-15 = Referenz 2, Schwanz 16-22 = V4):
  1. Mesh 9 := Original-Mesh 9 bytegleich (Ecken, Normalen, Flaechen, UVs); nur das CLUT-Feld jeder Flaeche zeigt auf die neue Zeile 486.
  2. TIM-Texel: die V5-Einsaetze (G2-Kopf TPage 128 x 0-125 y 160-249, G3-Zahnkachel TPage 130 x 256-267 y 200-235) auf Referenz 2
     zurueck; Schwanztexel (TPage 130 x 320-383) bleiben V4.
  3. CLUT: Zeilen 480-485 bytegleich Referenz 2; 486 = Original-Zeile 0 (y 480) bytegleich; die V5-Zeilen 486/487/488 entfallen
     (7 Zeilen, Port-Texturhoehe 256 x 7 = 1792 <= 4096).
  4. (Stufe 1) Kopftextur erweitert (texel_erweitern): nur Texel, die ausschliesslich Mesh 9 nutzt; Indizes aus der Original-Textur
     (Fortsetzen der Original-Texel des grauen Flecks bzw. des Hellgrau), keine neue Farbe.
  5. (Stufe 1) Ruecken -> Kopf fliessend (Nutzer 4): Pose P2 (Foto 2) gerendert, obere Kontur von der Buckelkuppe zur Kopf-Vorderkante
     (v6_gemein.kontur). Steigt die Kontur nach dem tiefsten Punkt vor dem Kopf wieder an (Original-Kopf: 3 px, Ecke 60 = Hinterkopf
     oben), werden NUR die Kopfecken, die im Spaltenfenster [Tal - 2, Tal + 10] hoeher als (Tal - 1) liegen, auf der Geraden zum
     Mesh-Schwerpunkt so weit eingezogen, dass ihr Bildpunkt die Grenze erreicht (Halbierungssuche). Normalen, Flaechen, UV, Texel und
     Palette bleiben (Liste im Protokoll 'glaettung').
Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante.
"""
import os, sys, json, struct, argparse, hashlib, copy
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, HIER); sys.path.insert(0, TOOLS)
import emd_ansichtsblatt as EA
import birkin_formen as BF

SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
BASIS = os.environ.get('RE15_V6_BASIS', os.path.join(SP, 'sfv4', 'emd', 'EM30_V5r4.EMD'))     # sha256 38562ced (V5 Runde 4)
REF2 = os.path.join(EA.REPO, 're15_port', 'shared_assets', 'PSX', 'EMD', 'EM30_HANGAR.EMD')    # sha256 cfd1ac42 (Pin)
AUS = os.path.join(SP, 'sfv4', 'emd')
KOPF = 9
ZEILE_KOPF = 486                      # neue CLUT-Zeile = Original-Zeile 0 bytegleich
V5_EINSAETZE = [(128, 0, 160, 126, 90), (130, 0, 200, 12, 36)]   # (TPage, u0, v0, Breite, Hoehe) der V5-Kopftexel (G2-Kopf, G3-Kachel)


def sha(b):
    return hashlib.sha256(b).hexdigest()


def teile(blob):
    d = EA.emd_dir(blob); dir_off = EA.u32(blob, 0)
    return d, dir_off, BF.md1_lesen(blob[d[7]:d[8]]), blob[d[8]:dir_off]


def tim_teile(tim):
    cs, cx, cy, cw, ch = struct.unpack_from('<I4H', tim, 8)
    clut = np.frombuffer(bytes(tim[20:20 + 2 * cw * ch]), '<u2').reshape(ch, cw).copy()
    ende = 8 + cs
    iw, ih = struct.unpack_from('<HH', tim, ende + 8)
    pix = np.frombuffer(bytes(tim[ende + 12:ende + 12 + 2 * iw * ih]), np.uint8).reshape(ih, 2 * iw).copy()
    return dict(cx=cx, cy=cy, cw=cw, clut=clut, ende=ende, iw=iw, ih=ih, pix=pix, rest=bytes(tim[ende + 12 + 2 * iw * ih:]),
                kopf=bytes(tim[:8]), bild_kopf=bytes(tim[ende:ende + 12]))


def tim_packen(t):
    c = t['clut'].astype('<u2')
    blk = struct.pack('<I4H', 12 + c.size * 2, t['cx'], t['cy'], t['cw'], c.shape[0]) + c.tobytes()
    neu = t['kopf'] + blk + t['bild_kopf'] + t['pix'].tobytes() + t['rest']
    if len(neu) % 4:
        neu += bytes(4 - len(neu) % 4)
    return neu


def raster(uv, n, po, shape=(256, 384)):
    """Texelmaske einer Flaeche (UV-Polygon inkl. Rand, wie sfv6/p2/p3)."""
    pts = np.array([[uv[3 * k] + po, uv[3 * k + 1]] for k in range(n)], np.int32)
    if n == 4:
        pts = pts[[0, 1, 3, 2]]
    m = np.zeros(shape, np.uint8)
    cv2.fillPoly(m, [pts], 1); cv2.polylines(m, [pts], True, 1)
    return m.astype(bool)


def texel_nutzung(meshes, shape=(256, 384)):
    own = {}
    for i, m in enumerate(meshes):
        mk = np.zeros(shape, bool)
        for uv in m['tuv']:
            mk |= raster(uv, 3, (uv[5] & 0xF) * 128, shape)
        for uv in m['quv']:
            mk |= raster(uv, 4, (uv[5] & 0xF) * 128, shape)
        own[i] = mk
    return own


MASKEN = {}          # letzter Bau: ersetzte Texel Zone A / B, freie Kopftexel (fuer Riegel und Leisten)


def original_kopf():
    o = BF.lade_em30()
    d, dir_off, ms, tim = teile(o)
    return o, ms[KOPF], tim_teile(tim)


def bauen(stufe=1, basis=BASIS, param=None):
    blob = open(basis, 'rb').read(); ref2 = open(REF2, 'rb').read()
    o, m9, to = original_kopf()
    prot = dict(basis=basis, basis_sha256=sha(blob), ref2=REF2, ref2_sha256=sha(ref2), original=EA.EMS_PFAD,
                original_ems_index=EA.EMS_ORDER.index(0x30), original_sha256=sha(o), stufe=stufe)
    d, dir_off, meshes, tim = teile(blob)
    dr, _, mr, timr = teile(ref2)
    # 1. Mesh 9 = Original, CLUT-Feld -> 486
    k = copy.deepcopy(m9)
    for uv in k['tuv'] + k['quv']:
        assert uv[2] >> 6 == 480, uv
        uv[2] = (ZEILE_KOPF << 6) | (uv[2] & 0x3F)
    prot['mesh9_alt'] = dict(ecken=len(meshes[KOPF]['v']), dreiecke=len(meshes[KOPF]['tf']), vierecke=len(meshes[KOPF]['qf']))
    meshes[KOPF] = k
    # 2. TIM: V5-Einsaetze auf Referenz 2
    t = tim_teile(tim); tr = tim_teile(timr)
    zur = 0
    for page, u0, v0, w, h in V5_EINSAETZE:
        x0 = 128 * (page & 0xF) + u0
        zur += int((t['pix'][v0:v0 + h, x0:x0 + w] != tr['pix'][v0:v0 + h, x0:x0 + w]).sum())
        t['pix'][v0:v0 + h, x0:x0 + w] = tr['pix'][v0:v0 + h, x0:x0 + w]
    # 3. CLUT: 480-485 = Referenz 2, 486 = Original-Zeile 0
    assert t['cy'] == 480 and tr['clut'].shape[0] == 6
    t['clut'] = np.vstack([tr['clut'], to['clut'][0][None]])
    prot['tim'] = dict(v5_einsaetze_zurueck=zur, clut_zeilen=int(t['clut'].shape[0]), port_texturhoehe=int(t['ih'] * t['clut'].shape[0]),
                       zeile_486='Original EM30 CLUT-Zeile 0 (y 480) bytegleich')
    # 4. Kopftextur erweitern (nur Texel exklusiv fuer Mesh 9)
    if stufe >= 1:
        import texel_erweitern as TE
        nutz = texel_nutzung(meshes)
        fremd = np.zeros_like(nutz[KOPF])
        for i, mk in nutz.items():
            if i != KOPF:
                fremd |= mk
        t['pix'], prot['textur'] = TE.erweitern(t['pix'], k, nutz[KOPF] & ~fremd, to['clut'][0], param)
        MASKEN['a'] = prot['textur'].pop('maske_a'); MASKEN['b'] = prot['textur'].pop('maske_b'); MASKEN['frei'] = nutz[KOPF] & ~fremd
    tim_neu = tim_packen(t)
    neu = packen(blob, d, meshes, tim_neu)
    if stufe >= 1:
        neu, prot['glaettung'] = glaetten(neu, blob, d, meshes, tim_neu)
    prot['emd'] = dict(bytes=len(neu), sha256=sha(neu), sektionen_gleich=list(range(7)))
    return neu, prot


def packen(blob, d, meshes, tim_neu):
    md1 = BF.md1_schreiben(meshes)
    sek = [blob[d[i]:d[i + 1]] for i in range(7)] + [md1, tim_neu]
    neu_dir, o_ = [], 8
    for x in sek:
        assert len(x) % 4 == 0
        neu_dir.append(o_); o_ += len(x)
    n = EA.u32(blob, 4)
    return struct.pack('<II', 8 + sum(len(x) for x in sek), n) + b''.join(sek) + struct.pack('<%dI' % n, *neu_dir)


GLATT = dict(pose='P2', fenster=(-2, 10), rest=1, schritte=3)   # Grenze = Tal - rest (px); hoechstens 3 Durchgaenge


def glaetten(neu, blob, d, meshes, tim_neu):
    import v6_gemein as V
    import birkin_buehne as BB
    pose = V.posen()[GLATT['pose']]
    liste, info = {}, []
    for _ in range(GLATT['schritte']):
        mo = V.modell(neu)
        img, ids = V.render(mo, pose)
        k = V.kontur(ids)
        info.append(dict(wiederanstieg=k['wiederanstieg'], stelle=k['stelle']))
        tal_x, tal_y = k['stelle'][0], k['stelle'][1]
        if k['wiederanstieg'] <= GLATT['rest']:
            break
        grenze = tal_y - GLATT['rest']
        kf = BB.kf_von_clip(mo, pose['clip'], pose['bild']); R, T = mo.pose(kf)
        Rk, Tk = R[KOPF].astype(float) / 4096.0, T[KOPF].astype(float)
        P = np.array([v[:3] for v in meshes[KOPF]['v']], float)
        c = P.mean(0)
        def proj(q):
            return V.b2.projiziere(pose, (np.atleast_2d(q) @ Rk.T) + Tk)
        q = proj(P)
        f0, f1 = GLATT['fenster']
        kand = [i for i in range(len(P)) if tal_x + f0 <= q[i, 0] <= tal_x + f1 and q[i, 1] < grenze + 0.5]
        if not kand:
            break
        for i in kand:
            lo, hi = 0.0, 1.0
            for _b in range(30):
                m = (lo + hi) / 2
                if proj(P[i] + m * (c - P[i]))[0, 1] >= grenze + 0.5:
                    hi = m
                else:
                    lo = m
            neu_p = np.round(P[i] + hi * (c - P[i])).astype(int)
            alt = liste.get(i, dict(alt=[int(x) for x in meshes[KOPF]['v'][i][:3]]))['alt']
            meshes[KOPF]['v'][i][:3] = [int(x) for x in neu_p]
            liste[i] = dict(ecke=int(i), alt=alt, neu=[int(x) for x in neu_p], zum_schwerpunkt=round(float(hi), 4),
                            weg=round(float(np.linalg.norm(np.array(neu_p) - np.array(alt))), 1), bild_alt=[round(float(v), 1) for v in q[i]])
        neu = packen(blob, d, meshes, tim_neu)
    return neu, dict(regel=dict(GLATT), schwerpunkt=[round(float(v), 1) for v in np.array([v[:3] for v in meshes[KOPF]['v']], float).mean(0)],
                     ecken=list(liste.values()), durchgaenge=info)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--aus', default=AUS); ap.add_argument('--basis', default=BASIS); ap.add_argument('--json')
    ap.add_argument('--stufe', type=int, default=1); ap.add_argument('--name')
    a = ap.parse_args()
    neu, prot = bauen(a.stufe, a.basis)
    os.makedirs(a.aus, exist_ok=True)
    name = a.name or ('EM30_V5k.EMD' if a.stufe >= 1 else 'EM30_V6orig.EMD')
    open(os.path.join(a.aus, name), 'wb').write(neu)
    if a.stufe >= 1 and not a.name:
        open(os.path.join(a.aus, 'EM30_V4.EMD'), 'wb').write(neu)
    print('geschrieben %s (%d Byte, sha256 %s)%s' % (os.path.join(a.aus, name), len(neu), prot['emd']['sha256'][:16],
                                                     '; EM30_V4.EMD darauf' if a.stufe >= 1 and not a.name else ''))
    print('TIM', prot['tim'])
    if 'textur' in prot:
        print('Textur', {k: v for k, v in prot['textur'].items() if not isinstance(v, (list, dict)) or len(str(v)) < 400})
    if a.json:
        json.dump(prot, open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))


if __name__ == '__main__':
    main()
