#!/usr/bin/env python3
"""Gemeinsame Leser: Fassungen des Birkin-Modells aus Disc, Mod und RE2. Nutzt die Port-Leser aus
re15_port/tools/emd_ansichtsblatt.py (MD1, EMR, EDD, TIM) und liest die Blobs ueber Sektor/Laenge (blobs.py)."""
import hashlib, re, struct, sys
import numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/re15_port/tools')
import emd_ansichtsblatt as EA        # noqa
REPO = 'C:/workspace/git/reAi_v2/'
DISC = REPO + 'Version_40%/PSX/EMD/'
MOD  = REPO + 'info/Re1.5/PSX/EMD/'
RE2E = REPO + 're15_port/shared_assets/RE2/CDEMD0.EMS'
TOCI = REPO + 're15_port/engine/src/gen/re2_ems_toc.inc'
u16, u32 = EA.u16, EA.u32

def lies(pfad, sektor, laenge):
    with open(pfad, 'rb') as f:
        f.seek(sektor*2048)
        return f.read(laenge)

def re2_rec(kind, r):
    inc = open(TOCI, encoding='utf-8', errors='replace').read()
    m = re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S)
    toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', m.group(1))]
    i = ((kind - 0x10) * 4 + r) * 2
    s, l = toc[i], toc[i+1]
    if not l:
        return None, s, l
    return lies(RE2E, s, l), s, l

FASSUNGEN = {
    # Name: (Pfad, Sektor, Laenge, Text)
    'D0':   (DISC+'CDEMD0.EMS', 1263, 226404, 'Disc CDEMD0 Typ 0x30 (Leon-TOC)'),
    'D1':   (DISC+'CDEMD1.EMS', 1266, 226404, 'Disc CDEMD1 Typ 0x30 (Elza-TOC)'),
    'M0':   (MOD+'CDEMD0.EMS',  1261, 226404, 'Mod CDEMD0 Typ 0x30 (Leon-TOC)'),
    'M1k':  (MOD+'CDEMD1.EMS',  1265, 226404, 'Mod CDEMD1 Sektor 1265, ohne TOC-Eintrag (alter Platz 0x30)'),
    'M1':   (MOD+'CDEMD1.EMS',  2015, 228460, 'Mod CDEMD1 Typ 0x30 (Elza-TOC) Sektor 2015'),
    'M0x':  (MOD+'CDEMD0.EMS',  2012, 228460, 'Mod CDEMD0 Sektor 2012, ohne TOC-Eintrag'),
    'D36':  (DISC+'CDEMD0.EMS', 1374, 128600, 'Disc Typ 0x36'),
    'M36':  (MOD+'CDEMD0.EMS',  1372, 128600, 'Mod Typ 0x36'),
    'Z0a':  (MOD+'CDEMD0.EMS',  1922, 183328, 'Mod CDEMD0 Sektor 1922, ohne TOC'),
    'Z1a':  (MOD+'CDEMD1.EMS',  1924, 184676, 'Mod CDEMD1 Sektor 1924, ohne TOC'),
    'Z0c':  (MOD+'CDEMD0.EMS',  2124, 166592, 'Mod CDEMD0 Sektor 2124, ohne TOC'),
    'Z1c':  (MOD+'CDEMD1.EMS',  2127, 166592, 'Mod CDEMD1 Sektor 2127, ohne TOC'),
    'Z0d':  (MOD+'CDEMD0.EMS',  2206, 213576, 'Mod CDEMD0 Sektor 2206, ohne TOC'),
    'Z1d':  (MOD+'CDEMD1.EMS',  2209, 213896, 'Mod CDEMD1 Sektor 2209, ohne TOC'),
}

def blob(name):
    p, s, l, _ = FASSUNGEN[name]
    return lies(p, s, l)

class M:
    pass

def zerlege(b, re2_tim=None):
    """Sektionen eines EMD. RE1.5: n=9, dir[8]=TIM. RE2: n=8, dir[7]=MD1 bis Verzeichnis, TIM getrennt."""
    d = EA.emd_dir(b)
    dir_off = u32(b, 0)
    n = len(d)
    grenzen = d + [dir_off]
    sek = {}
    for i in range(n):
        sek[i] = b[grenzen[i]:grenzen[i+1]]
    mo = M()
    mo.dir = d
    mo.n = n
    mo.sek = sek
    mo.md1 = sek[7]
    mo.meshes = EA.md1_parse(mo.md1)
    mo.tim = sek[8] if n >= 9 else re2_tim
    mo.tex = mo.pal = None
    if mo.tim:
        try:
            mo.tex, mo.pal, mo.clut_y = EA.tim_decode(mo.tim)
        except Exception as e:
            mo.fehler = str(e)
    try:
        mo.skel = EA.emr_parse(sek[2])
    except Exception as e:
        mo.skel = None
    try:
        mo.clips, mo.frames = EA.edd_parse(sek[1])
    except Exception as e:
        mo.clips, mo.frames = [], []
    return mo

def md5(b):
    return hashlib.md5(b).hexdigest()[:8]

def kennzahlen(mo):
    tris = sum(len(m.tf) for m in mo.meshes)
    quads = sum(len(m.qf) for m in mo.meshes)
    verts = sum(len(m.tv) + (len(m.qv) if m.qv != m.tv else 0) for m in mo.meshes)
    return dict(meshes=len(mo.meshes), tris=tris, quads=quads, verts=verts, knochen=mo.skel.bone_count if mo.skel else -1,
                kf=mo.skel.kf_count if mo.skel else -1, kfgr=mo.skel.kf_size if mo.skel else -1, clips=len(mo.clips))
