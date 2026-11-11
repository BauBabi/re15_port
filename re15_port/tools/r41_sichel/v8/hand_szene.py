#!/usr/bin/env python3
"""R41 Spur SF, V8 Runde 6c (Befund P7): linke Hand und linker Arm in den ECHTEN Szenenposen - Messgrundlage, Ellbogen-Stufe, Handstellung.

Nutzer 2026-11-10 abends (woertlich): "Bei dem 2 beinigen Birkin in der Vorschau sieht es so aus als waere die linke Hand verkehrt herum
gedreht, Kann das sein=?"  Ursache (SF_sichel.md "V8 Runde 6c"): die Hand wurde in Runde 6a an den ERSATZ-Posen P1/P2/PV ausgerichtet
(Verdrehung 118 Grad), nicht an den Posen, die das Spiel in der Szene wirklich zeigt; die Ellbogen-Beuge 60 Grad stammt aus NB3.

PAARE: jedes Vorlagenbild des Nutzers (Version_80%/birkin20-24.png = Videobilder f1476/f1477/f1479/f1481/f1483) gegen die Modell-Pose
SEINES Spielbilds. Zuordnung f = h + 1351 (Videobild), F = h + 243 (Spielbild), ab h90 laeuft Clip 1 mit Bild = h - 90. Clip, Bild,
Gier und Ort (x, z) sind die Takt-Zeilen "[hangar-takt] h.. F.. B clip .. bild .. gier .. (x,z)" des eigenen Szenenlaufs vom 2026-11-11
(RE15_HANGAR_LOG, v4/lauf_v4.sh; Abschrift unten, Beleg analysis/befunde_runde41/SF_belege/v8/runde6c/takt.txt).
KONTROLLE: drei weitere Bilder desselben Gangs ohne Vorlagenbild (h100, h110, h135).

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. PORT-WAHL, GESTALTUNG nach dem Video.
"""
import os, sys, math, struct
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
sys.path.insert(0, os.path.join(V.R41, 'v7hand'))
import kopf_orig_bau as KB            # noqa: E402
import birkin_formen as BF            # noqa: E402
import birkin_buehne as BB            # noqa: E402
import emd_ansichtsblatt as EA        # noqa: E402

# (Vorlagenbild, Takt h, Pose) - Takt-Zeilen des Szenenlaufs 2026-11-11 (Spiel-Asset 95a783a6; die Zeilen haengen nicht vom Modell ab)
PAARE = ((20, 125, dict(clip=1, bild=35, gier=1515, x=-9065, z=-22223)),
         (22, 126, dict(clip=1, bild=36, gier=1513, x=-9080, z=-22239)),
         (23, 128, dict(clip=1, bild=38, gier=1513, x=-9110, z=-22271)),
         (21, 130, dict(clip=1, bild=40, gier=1513, x=-9140, z=-22303)),
         (24, 132, dict(clip=1, bild=42, gier=1513, x=-9170, z=-22335)))
KONTROLLE = ((100, dict(clip=1, bild=10, gier=1515, x=-8665, z=-21823)),
             (110, dict(clip=1, bild=20, gier=1515, x=-8825, z=-21983)),
             (135, dict(clip=1, bild=45, gier=1513, x=-9215, z=-22383)))
POSEN = dict([('h%d' % h, p) for _b, h, p in PAARE] + [('h%d' % h, p) for h, p in KONTROLLE])
PAAR_VON = {'h%d' % h: b for b, h, _p in PAARE}       # Posenname -> Vorlagenbild
PAAR_NAMEN = tuple('h%d' % h for _b, h, _p in PAARE)
KONTROLL_NAMEN = tuple('h%d' % h for h, _p in KONTROLLE)


def spielbild(h):
    return h + 243                    # F = h + 243


def videobild(h):
    return h + 1351                   # f = h + 1351


# Keyframes (Clip -> Bilder), die der Hangar-Auftritt ab dem ersten Videobild (Spielbild F243 = h0) zeigt; aus dem Takt-Protokoll des
# Szenenlaufs (analysis/befunde_runde41/SF_belege/v8/runde6c/takt.txt): Clip 0 Stand, Clip 11 Auftritt bis h89, Clip 1 Gang ab h90.
SZENE_KF = {0: tuple(range(9, 16)), 1: tuple(range(45)),
            11: tuple(range(30)) + (32, 35, 38, 41, 44, 46, 49, 52) + tuple(range(55, 69))}

V7_BEUGE = 60.0                       # birkin_formen.FORMEN['hangar']['ducken']['ellbogen_links'] (NB3), in der V7-Basis 319bbd17 enthalten
ARM = (10, 11, 12)                    # linker Oberarm, Unterarm, Hand
SPITZE = {'T': 20, 'A': 47, 'B': 29, 'C': 42}
RING = {'T': [18, 19, 21], 'A': [45, 46, 48], 'B': [27, 28, 30], 'C': [40, 41, 43]}


def einheit(v):
    v = np.asarray(v, float)
    return v / max(float(np.linalg.norm(v)), 1e-12)


# ------------------------------------------------------------------ Ellbogen-Stufe (Stufe b der V8-Kette)
_ORIG = {}


def original_emr():
    """EMR Bank 0 des Original-EM30 (CDEMD0.EMS Typ 0x30) und das Duck-Gewicht w je Keyframe - dieselbe Formel wie
    birkin_formen.ducken(): w = clip((R7[1,1] - 0,35) / 0,45, 0, 1) aus den Original-Lokaldrehungen (Aufrichtung des Rumpfs)."""
    if not _ORIG:
        blob = BF.lade_em30()
        d = EA.emd_dir(blob)
        emr = bytes(blob[d[2]:d[3]])
        s = EA.emr_parse(emr)
        rel = [np.array(r, float) for r in s.rel]
        w, lok11 = [], []
        for k in range(s.kf_count):
            lokal = [EA.mat_euler(*EA.kf_angles(s, k, b)).astype(float) / 4096 for b in range(s.bone_count)]
            R0, _T0 = BF._welt(s, rel, lokal, np.array(EA.kf_pos(s, k), float))
            w.append(float(np.clip((R0[7][1, 1] - 0.35) / 0.45, 0.0, 1.0)))
            lok11.append(lokal[11])
        _ORIG.update(w=np.array(w), lok11=lok11, kf=s.kf_count)
    return _ORIG


def beuge_emr(emr, grad):
    """EMR (Bank 0 der Sichel-Form, 23 Knochen) -> EMR mit der Ellbogen-Beuge <grad> statt 60: Knochen 11 je Keyframe neu aus dem
    ORIGINAL-Keyframe, lokal = Rz(-grad * w) @ Original (ducken() rechnet Rz(-60 * w) @ Original); das nimmt die Drehung von Knochen 11
    um (60 - grad) * w zurueck, ohne die Quantisierung der V7-Basis ein zweites Mal zu runden. Keyframes mit w = 0 bleiben wie sie sind.
    -> (EMR, Zahl der geaenderten Keyframes)."""
    o0 = original_emr()
    emr = bytearray(emr)
    s = EA.emr_parse(bytes(emr))
    assert s.kf_count == o0['kf'], 'Keyframe-Zahl %d ungleich Original %d' % (s.kf_count, o0['kf'])
    kf_off, ks = struct.unpack_from('<H', emr, 2)[0], s.kf_size
    n = 0
    for k in range(s.kf_count):
        w = o0['w'][k]
        if w <= 0.0:
            continue
        o = kf_off + k * ks
        vor = bytes(emr[o:o + ks])
        for i, wi in enumerate(BF._zerlege(BF._rz(-grad * w) @ o0['lok11'][k])):
            BF._bits_schreiben(emr, o + 12, 11 * 36 + i * 12, wi)
        n += vor != bytes(emr[o:o + ks])
    return bytes(emr), n


V7_INNEN = 20.0                       # birkin_formen.FORMEN['hangar']['ducken']['arm_innen'], in der V7-Basis 319bbd17 enthalten


def oberarm_emr(emr, innen):
    """EMR -> EMR mit dem linken Oberarm (Knochen 10) um <innen> statt 20 Grad zur Koerpermitte gedreht. ducken() rechnet fuer Knochen 10
    lokal = Rx(-arm_innen * w) @ M (M = Brust-Gegendrehung mal gehaltener Arm); der Wechsel von 20 auf <innen> ist also
    lokal_neu = Rx((20 - innen) * w) @ lokal_basis. Gerechnet wird aus der gespeicherten (12-Bit-)Drehung der Basis: ein zweites Runden
    auf 12 Bit (0,088 Grad) kommt hinzu, M selbst liegt nicht vor. Bei <innen> = 20 und bei w = 0 bleibt der Keyframe unberuehrt.
    -> (EMR, Zahl der geaenderten Keyframes)."""
    o0 = original_emr()
    emr = bytearray(emr)
    s = EA.emr_parse(bytes(emr))
    assert s.kf_count == o0['kf']
    kf_off, ks = struct.unpack_from('<H', emr, 2)[0], s.kf_size
    n = 0
    for k in range(s.kf_count):
        w = o0['w'][k]
        if w <= 0.0 or float(innen) == V7_INNEN:
            continue
        o = kf_off + k * ks
        vor = bytes(emr[o:o + ks])
        lok = EA.mat_euler(*EA.kf_angles(s, k, 10)).astype(float) / 4096
        for i, wi in enumerate(BF._zerlege(BF._rx((V7_INNEN - float(innen)) * w) @ lok)):
            BF._bits_schreiben(emr, o + 12, 10 * 36 + i * 12, wi)
        n += vor != bytes(emr[o:o + ks])
    return bytes(emr), n


def stufe_b(blob, cfg, prot=None):
    """Stufe b: Ellbogen-Beuge des linken Arms nach hand_v8.json "ellbogen". Selbstprobe: dieselbe Rechnung mit 60 Grad ergibt die EMR
    der Basis bytegleich (also ist Knochen 11 der Basis genau der Wert von ducken() und die Stufe aendert nichts ausser der Beuge)."""
    d = EA.emd_dir(blob)
    emr = bytes(blob[d[2]:d[3]])
    probe, n60 = beuge_emr(emr, V7_BEUGE)
    assert probe == emr and n60 == 0, 'Selbstprobe: Beuge 60 ergibt nicht die Basis (%d Keyframes anders)' % n60
    neu, n = beuge_emr(emr, float(cfg['beuge_grad']))
    neu, n10 = oberarm_emr(neu, float(cfg.get('oberarm_innen_grad', V7_INNEN)))
    assert len(neu) == len(emr)
    if prot is not None:
        prot['ellbogen'] = dict(cfg=cfg, keyframes=int(original_emr()['kf']), keyframes_geaendert=int(n), keyframes_oberarm_geaendert=int(n10),
                                keyframes_w0=int((original_emr()['w'] <= 0).sum()), selbstprobe_60_bytegleich=True)
    return bytes(blob[:d[2]]) + neu + bytes(blob[d[3]:])


def probe_blob(basis, cfg):
    """Probe-Stand fuer Reihen: Basis-EMD (ein Stand der V8-Kette nach Stufe h, Beuge 60) mit Hand/Unterarm nach cfg (hand_r6) und Beuge
    nach cfg['ellbogen'] - dieselben Funktionen wie die Kette (v8_bau Stufen h und b)."""
    import hand_r6 as H6
    d, _do, ms, tim = KB.teile(basis)
    H6.anwenden(ms, None, cfg, {})
    neu = KB.packen(basis, d, ms, tim)
    return stufe_b(neu, cfg['ellbogen']) if cfg.get('ellbogen') else neu


# ------------------------------------------------------------------ Hand-Achsen und Stellung
def mesh_punkte(me):
    """Ecken eines Meshes im Knochenraum; me = b2-Mesh (tv) oder KB-Mesh (dict v)."""
    return np.array([x[:3] for x in (me['v'] if isinstance(me, dict) else me.tv)], float)


def achsen(P):
    """Achsen der Hand aus dem Netz (Knochenraum, Ursprung = Handgelenk): f = Finger (Handgelenk -> Mitte der Krallenwurzeln A, B, C),
    d = Daumen (Mitte der Krallenwurzeln -> Wurzel der Daumenkralle, quer zu f), r = Handruecken (quer zu beiden; die Krallen
    kruemmen sich von r weg). -> f, d, r (Einheitsvektoren), Kruemmung der Fingerkrallen gegen r (alle < 0 = eindeutig)."""
    wurzeln = np.array([P[RING[k]].mean(0) for k in 'ABC'])
    f = einheit(wurzeln.mean(0))
    dv = P[RING['T']].mean(0) - wurzeln.mean(0)
    d = einheit(dv - (dv @ f) * f)
    n = np.cross(f, d)
    kr = [float((P[SPITZE[k]] - P[RING[k]].mean(0)) @ n) for k in 'ABC']
    vz = -1.0 if sum(kr) > 0 else 1.0
    return f, d, vz * n, [round(k * vz, 1) for k in kr]


def verdrehung(mo, mo0):
    """S20 (a): Verdrehung der Hand gegen den Unterarm, verglichen mit Capcoms Original, in ALLEN Keyframes: der Daumen (Richtung d) wird
    in den Raum des Unterarms gebracht (lokale Drehung von Knochen 12 des jeweiligen Keyframes) und der Winkel zwischen Modell und
    Original um die Fingerachse des Originals gemessen.
    -> dict(min, median, max, lokal_ungleich = Keyframes, in denen die lokale Drehung von Knochen 12 vom Original abweicht)."""
    _f, d, _r, _k = achsen(mesh_punkte(mo.meshes[12]))
    f0, d0, _r0, _k0 = achsen(mesh_punkte(mo0.meshes[12]))
    w, ungl = [], 0
    for kf in range(mo.skel.kf_count):
        L = EA.mat_euler(*EA.kf_angles(mo.skel, kf, 12)).astype(float) / 4096
        L0 = EA.mat_euler(*EA.kf_angles(mo0.skel, kf, 12)).astype(float) / 4096
        ungl += int(np.abs(L - L0).max() > 1e-9)
        a, b, fa = L @ d, L0 @ d0, einheit(L0 @ f0)
        a, b = a - (a @ fa) * fa, b - (b @ fa) * fa
        w.append(math.degrees(math.atan2(float(np.cross(b, a) @ fa), float(a @ b))))
    w = np.array(w)
    return dict(min=float(w.min()), median=float(np.median(w)), max=float(w.max()), lokal_ungleich=ungl, keyframes=len(w))


def haltung(mo, achs=None):
    """Je Keyframe im Modellraum (x vorn, y unten, z links): Handruecken nach vorn (r . x > 0)? Daumen zur Koerpermitte (d . z < 0)?
    und der Winkel phi des Daumens um die Unterarm-Achse, gemessen von der Beugeseite des Ellbogens (0 = Daumen zur Beugeseite,
    +90 = Daumen zur Koerpermitte, +-180 = Daumen zur Streckseite; nur bei Ellbogen-Beugung >= 10 Grad bestimmt)."""
    f, d, r, _k = achs or achsen(mesh_punkte(mo.meshes[12]))
    aus = []
    for kf in range(mo.skel.kf_count):
        R, T = mo.pose(kf)
        R12 = R[12].astype(float) / 4096
        a = einheit(T[11].astype(float) - T[10].astype(float))
        u = einheit(T[12].astype(float) - T[11].astype(float))
        beuge = math.degrees(math.acos(float(np.clip(a @ u, -1, 1))))
        dm, rm = R12 @ d, R12 @ r
        phi = None
        if beuge >= 10.0:
            ef = einheit(-a + (a @ u) * u)
            em = np.cross(u, ef)
            dp = dm - (dm @ u) * u
            phi = math.degrees(math.atan2(float(dp @ em), float(dp @ ef)))
        aus.append(dict(kf=kf, ruecken_vorn=float(rm[0]), daumen_mitte=float(-dm[2]), beuge=beuge, phi=phi, d=dm, r=rm, f=R12 @ f))
    return aus


def haltung_zahlen(mo, mo0, achs=None):
    """Zusammenfassung ueber alle Keyframes, Modell gegen Capcoms Original: Anteil Handruecken nach vorn / Daumen zur Koerpermitte und
    die mittlere Abweichung des Daumen-Winkels um die Fingerachse (Daumenrichtung des Modells gegen die des Originals im selben Keyframe,
    beide im Modellraum, quer zur Fingerachse des Modells)."""
    A, B = haltung(mo, achs), haltung(mo0)
    ab = []
    for a, b in zip(A, B):
        fa = einheit(a['f'])
        x, y = a['d'] - (a['d'] @ fa) * fa, b['d'] - (b['d'] @ fa) * fa
        if np.linalg.norm(y) < 0.2:
            continue
        ab.append(abs(math.degrees(math.atan2(float(np.cross(y, x) @ fa), float(x @ y)))))
    phi = np.array([a['phi'] for a in A if a['phi'] is not None])
    phi0 = np.array([b['phi'] for b in B if b['phi'] is not None])
    n = float(len(A))
    return dict(ruecken_vorn=sum(a['ruecken_vorn'] > 0 for a in A) / n, daumen_mitte=sum(a['daumen_mitte'] > 0 for a in A) / n,
                ruecken_vorn_orig=sum(b['ruecken_vorn'] > 0 for b in B) / n, daumen_mitte_orig=sum(b['daumen_mitte'] > 0 for b in B) / n,
                daumen_abw_mittel=float(np.mean(ab)), daumen_abw_median=float(np.median(ab)), gezaehlt=len(ab),
                phi_median=float(np.median(phi)), phi_ausser=float((np.abs(phi) > 100).mean()), phi_n=len(phi),
                phi_median_orig=float(np.median(phi0)), phi_ausser_orig=float((np.abs(phi0) > 100).mean()))


def kamera_winkel(mo, pose, achs=None):
    """S20 (b): aus welcher Richtung sieht die Spielkamera die Hand? Blick Hand -> Kamera im Raum der Hand.
    -> dict(winkel = Drehung der Kamera um die Fingerachse vom Handruecken aus (0 = genau auf den Handruecken, +90 = auf die Daumenkante,
    -90 = auf die daumenabgewandte Kante, +-180 = in die Handflaeche), ruecken = Anteil des Blicks auf den Handruecken (+1 = frontal),
    finger = Anteil entlang der Finger (+1 = Kamera sieht von den Spitzen her))."""
    f, d, r, _k = achs or achsen(mesh_punkte(mo.meshes[12]))
    R, T = V.pose_r_t(mo, pose)
    R12 = R[12].astype(float) / 4096
    b = R12.T @ einheit(V.kamera_modell(pose) - T[12].astype(float))
    return dict(winkel=math.degrees(math.atan2(float(b @ d), float(b @ r))), ruecken=float(b @ r), daumen=float(b @ d), finger=float(b @ f))


def kf_liste(mo):
    """Erstes (Clip, Bild) je Keyframe."""
    erst = {}
    for c, (_f, k) in enumerate(mo.clips):
        for b in range(k):
            erst.setdefault(int(BB.kf_von_clip(mo, c, b)), (c, b))
    return erst
