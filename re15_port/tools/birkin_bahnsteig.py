#!/usr/bin/env python3
"""Bahnsteig-Birkin (Beta-Form, Nachtrag 38): RE2-G4 als Vorlage, nach den Video-Standbildern umgeformt.

    python re15_port/tools/birkin_bahnsteig.py               # Asset schreiben
    python re15_port/tools/birkin_bahnsteig.py --blatt DIR   # + Ansichtsblatt

Ausgabe (RE2-Format: 8 Verzeichnis-Eintraege, TIM getrennt; geladen vom PC-Port, platform/pc/main.c):
    re15_port/shared_assets/RE2/BAHNSTEIG_G4.EMD
    re15_port/shared_assets/RE2/BAHNSTEIG_G4.TIM

⛔ KEIN BYTE-TRUE-ASSET. Die Form gibt es auf keiner Disc, nur als zwei Standbilder in
"Biohazard 1.5 Final Build.mp4" (1:54.8 Bahnsteig, 1:56.3 roter Gang = ROOM5090 Cut 11).

Byte-true geerbt aus RE2 CDEMD0.EMS kind 0x34 (TOC-Index (kind-0x10)*4+rec, Zeile 0x8009adf4; rec 2 TIM,
rec 3 EMD, re2_ems.c): Skelett-Hierarchie (27 Knochen), EDD aller 29 Clips samt Treffer-/Ton-Flags,
Dreieckstopologie, UVs, Textur. Opfer-Bank (Paar 3) und Morph-Block bleiben unberuehrt.

GESTALTUNG (jede Zahl in FORM unten ist Gestaltung nach den Bildern, kein RE-Wert, und traegt deshalb
absichtlich keine @0x-Adresse; die Messung dazu steht in analysis/befunde_runde41/R_bahnsteig.md):
    skala      Massstab auf alle Laengen (Vertices, Bindoffsets, Wurzelhoehe, Wurzelvortrieb)
    dicke      Glieder im Mesh-Raum quer zur Knochenachse (Y laeuft bei RE2-Gliedern entlang des Knochens)
    wulst      Dickenprofil je Glied: Mitte stark, Gelenk eingeschnuert
    spreizung  Konstante Drehung der Gliedwurzeln um die Rumpf-Laengsachse: enger Stand, hoeherer Bauch
    buckel     Rueckenwoelbung der Rumpfmeshes
Der Boden wird je Keyframe nachgefuehrt (boden_nachfuehren): der tiefste Punkt der fertigen Pose liegt bei
y = 0. RE2 braucht das nicht, weil G4 dort auf einer eigenen Bodenlage steht; in Clip 2 und in den Hieben
reichen die Vorderglieder ohne Nachfuehrung bis 1282 Einheiten unter die Sohle.
"""
import argparse, os, re, struct, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import emd_ansichtsblatt as EA          # noqa: E402  (Leser: MD1, EMR, EDD, Pose)
import birkin_formen as BF              # noqa: E402  (MD1-Skalierung, Keyframe-Bits, Euler-Zerlegung)

KIND     = 0x34
RE2_DIR  = os.path.join(EA.REPO, 're15_port', 'shared_assets', 'RE2')
EMS_PFAD = os.path.join(RE2_DIR, 'CDEMD0.EMS')
TOC_PFAD = os.path.join(EA.REPO, 're15_port', 'engine', 'src', 'gen', 're2_ems_toc.inc')
EMD_DATEI, TIM_DATEI = 'BAHNSTEIG_G4.EMD', 'BAHNSTEIG_G4.TIM'

# Knochen RE2-G4 (aus dem EMR gelesen): 0 Becken, 1-4 Bein rechts, 5-8 Bein links, 9 Taille, 10 Rumpf,
# 11-14 Arm links, 15-19 Arm rechts, 20-22 / 23-25 kleine Arme, 26 Kiefer.
BEIN_R, BEIN_L = (1, 2, 3), (5, 6, 7)
ARM_L,  ARM_R  = (11, 12, 13), (15, 16, 17, 18)

FORM = dict(
    skala=1.0,                  # engine/src/birkin_bahnsteig.c BB_L() muss dazu passen
    # Quer zur Knochenachse. Bei den Gliedern traegt wulst das Profil: Mitte des Glieds dicker, Gelenk eingeschnuert
    # (Bild 1: knotige Glieder mit hellen Baendern an den Gelenken).
    dicke={**{b: 1.35 for b in BEIN_R + BEIN_L + ARM_L + ARM_R},
           3: 1.05, 7: 1.05, 13: 1.05, 18: 1.05,   # Unterstes Gliedstueck: laeuft sonst wie ein Hosenbein aus
           4: 1.2, 8: 1.2, 14: 1.2, 19: 1.2,    # Pfoten
           0: 1.3, 9: 1.5,                      # Becken, Taille: von hinten kein Einschnitt in der Kuppel
           10: (1.0, 1.0, 1.15)},               # Rumpf breiter (Z), Kuppel rund
    wulst=dict(knochen=BEIN_R + BEIN_L + ARM_L + ARM_R, staerke=0.30),
    # Grad um die Rumpf-Laengsachse: Gliedenden zur Mitte, enger Stand.
    spreizung={1: 10.0, 5: -10.0, 11: -14.0, 15: 14.0},
    buckel=None,                # Rueckenwoelbung abschaltbar: dict(mesh=10, hub, x_mitte, x_breite, z_breite)
)

u16, u32 = EA.u16, EA.u32


def lade_rohdaten():
    """rec 3 (EMD) und rec 2 (TIM) aus CDEMD0.EMS; TOC-Tabelle @0x8009adf4 (re2_ems_toc.inc)."""
    inc = open(TOC_PFAD, encoding='utf-8', errors='replace').read()
    m = re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S)
    toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', m.group(1))]
    ems = open(EMS_PFAD, 'rb').read()

    def rec(r):
        i = ((KIND - 0x10) * 4 + r) * 2
        return ems[toc[i] * 0x800:toc[i] * 0x800 + toc[i + 1]]
    return rec(3), rec(2)


def verzeichnis(emd):
    return EA.emd_dir(emd)


def modell(emd, tim):
    """EA.Modell aus RE2-Daten: TIM getrennt, mehrere CLUT-Zeilen, 384 Texel breit."""
    mo = EA.Modell.__new__(EA.Modell)
    d = verzeichnis(emd)
    mo.blob, mo.typ, mo.blob_index, mo.blob_len = emd, KIND, 0, len(emd)
    mo.meshes = EA.md1_parse(emd[d[7]:])
    mo.skel = EA.emr_parse(emd[d[2]:d[3]])
    mo.clips, mo.frames = EA.edd_parse(emd[d[1]:d[2]])
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(tim)
    mo.texh, mo.texw = mo.tex.shape
    return mo


def _rx(grad):
    a = np.radians(grad)
    return np.array([[1, 0, 0], [0, np.cos(a), -np.sin(a)], [0, np.sin(a), np.cos(a)]])


def woelbe_buckel(md1, b):
    """Rueckenwoelbung: Vertices oberhalb der Rumpfmitte (y < 0, Y zeigt nach unten) je nach Lage anheben."""
    if not b:
        return md1
    md1 = bytearray(md1)
    bl = BF.md1_bloecke(md1)[b['mesh']][0]
    gesehen = set()
    for off, n in bl:
        if (off, n) in gesehen:
            continue
        gesehen.add((off, n))
        for k in range(n):
            x, y, z, w = struct.unpack_from('<4h', md1, off + k * 8)
            f = np.exp(-((x - b['x_mitte']) / b['x_breite']) ** 2 - (z / b['z_breite']) ** 2)
            oben = np.clip((200.0 - y) / 700.0, 0.0, 1.0)
            struct.pack_into('<4h', md1, off + k * 8, x, int(round(y - b['hub'] * f * oben)), z, w)
    return bytes(md1)


def wulste_glieder(md1, w):
    """Dickenprofil der Glieder: je Mesh quer zur Y-Achse (Knochenachse) f(u) = 1 + staerke*(2*sin^2(pi*u) - 1), u = Lage
    entlang des Glieds. Die Normalen folgen wie in BF.skaliere_md1 nicht dem Profil, die Abweichung ist auf dem
    grob aufgeloesten Mesh nicht sichtbar."""
    if not w:
        return md1
    md1 = bytearray(md1)
    bl = BF.md1_bloecke(md1)
    gesehen = set()
    for b in w['knochen']:
        for off, n in bl[b][0]:
            if (off, n) in gesehen:
                continue
            gesehen.add((off, n))
            vs = [struct.unpack_from('<4h', md1, off + k * 8) for k in range(n)]
            y0, y1 = min(v[1] for v in vs), max(v[1] for v in vs)
            for k, (x, y, z, q) in enumerate(vs):
                u = (y - y0) / max(1, y1 - y0)
                f = 1.0 + w['staerke'] * (2.0 * np.sin(np.pi * u) ** 2 - 1.0)
                struct.pack_into('<4h', md1, off + k * 8, int(round(x * f)), y, int(round(z * f)), q)
    return bytes(md1)


def halte_keyframes_um(vor, d, form):
    """Alle Keyframes von Paar 1 (dir[2]): Wurzelhoehe/-vortrieb skalieren, Gliedwurzeln drehen."""
    a = d[2]
    struktur = EA.emr_parse(bytes(vor[a:d[3]]))
    ks, kf_off, n = struktur.kf_size, u16(vor, a + 2), struktur.kf_count
    s = form['skala']
    for k in range(n):
        o = a + kf_off + k * ks
        px, py, pz, sx, sy, sz = struct.unpack_from('<6h', vor, o)
        struct.pack_into('<6h', vor, o, *(int(round(c * s)) for c in (px, py, pz, sx, sy, sz)))
        sk = EA.Skel(); sk.kf_size, sk.kf_data = ks, bytes(vor[o:o + ks])
        for b, grad in form['spreizung'].items():
            lokal = EA.mat_euler(*EA.kf_angles(sk, 0, b)).astype(float) / 4096
            for i, w in enumerate(BF._zerlege(_rx(grad) @ lokal)):
                BF._bits_schreiben(vor, o + 12, b * 36 + i * 12, w)


def boden_nachfuehren(emd, tim):
    """Tiefster Punkt je Keyframe auf y = 0 (Y zeigt nach unten): Wurzelhoehe im Keyframe nachziehen."""
    emd = bytearray(emd)
    d = verzeichnis(emd)
    mo = modell(bytes(emd), tim)
    a, ks = d[2], mo.skel.kf_size
    kf_off = u16(emd, a + 2)
    v = {b: np.array([x[:3] for x in m.tv], float) for b, m in enumerate(mo.meshes[:mo.skel.bone_count]) if m.tv}
    for k in range(mo.skel.kf_count):
        R, T = mo.pose(k)
        tief = max(float((p @ (R[b].astype(float).T / 4096.0) + T[b])[:, 1].max()) for b, p in v.items())
        o = a + kf_off + k * ks
        y = struct.unpack_from('<h', emd, o + 2)[0]
        struct.pack_into('<h', emd, o + 2, max(-32768, min(32767, int(round(y - tief)))))
    return bytes(emd)


def baue(emd, tim, form=FORM):
    """Gleiches Layout wie rec 3: nur MD1 (Vertices/Normalen), EMR-Bindoffsets und Keyframes aendern sich."""
    d = verzeichnis(emd)
    dir_off, n = u32(emd, 0), u32(emd, 4)
    assert n == 8 and d[0] == 8, 'kein RE2-EMD'
    s = form['skala']
    skal = {}
    for b in range(27):
        t = form['dicke'].get(b, 1.0)
        tx, ty, tz = t if isinstance(t, tuple) else (t, 1.0, t)
        skal[b] = (s * tx, s * ty, s * tz)
    neu = bytearray(emd)
    neu[d[7]:dir_off] = BF.skaliere_md1(wulste_glieder(woelbe_buckel(emd[d[7]:dir_off], form.get('buckel')), form.get('wulst')), skal)
    neu[d[2]:d[3]] = BF.passe_emr_an(emd[d[2]:d[3]], skal, {})
    halte_keyframes_um(neu, d, form)
    return boden_nachfuehren(bytes(neu), tim)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--blatt', help='Verzeichnis fuer das Ansichtsblatt (PNG)')
    ap.add_argument('--clip', type=lambda s: int(s, 0), default=5)
    ap.add_argument('--aus', default=RE2_DIR, help='Zielverzeichnis')
    a = ap.parse_args()
    emd, tim = lade_rohdaten()
    assert len(emd) == 273812 and len(tim) == 99872, 'CDEMD0.EMS kind 0x34 unerwartet'
    fertig = baue(emd, tim)
    assert len(fertig) == len(emd)
    os.makedirs(a.aus, exist_ok=True)
    for name, daten in ((EMD_DATEI, fertig), (TIM_DATEI, tim)):
        with open(os.path.join(a.aus, name), 'wb') as f:
            f.write(daten)
        print('geschrieben: %s  (%d Byte)' % (os.path.join(a.aus, name), len(daten)))
    if a.blatt:
        mo = modell(fertig, tim)
        mo.titel = 'Bahnsteig-Birkin - Form nach Video, Basis RE2-G4 (kind 0x34)'
        mo.untertitel = 'NACHBILDUNG nach V2 1:54.8 + 1:56.3 - ' + EMD_DATEI + ' - Blob %d'
        mo.hinweis = ('NICHT byte-true: Massstab, Gliederdicke, Spreizung, Buckel und Bodennachfuehrung sind Gestaltung '
                      '(birkin_bahnsteig.py FORM). Unveraendert aus G4: Hierarchie, Clips, Topologie, UVs, Textur.')
        os.makedirs(a.blatt, exist_ok=True)
        EA.blatt(mo, a.clip, os.path.join(a.blatt, 'birkin_bahnsteig_g4.png'))


if __name__ == '__main__':
    main()
