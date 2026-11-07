# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: RE2-Modelle aus CDEMD0.EMS per TOC in den Scratchpad legen (nur Eichmass fuers Handwerk, keine Formvorlage)."""
import re, os, sys
WT = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
sys.path.insert(0, WT + '/re15_port/tools')
import form_emd as FE
ZIEL = sys.argv[1]
txt = open(WT + '/re15_port/engine/src/gen/re2_ems_toc.inc', encoding='utf-8').read()
blk = txt[txt.index('s_re2_cdemd0_toc[600]'):]
blk = blk[blk.index('{') + 1:blk.index('};')]
w = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', blk)]
assert len(w) == 600, len(w)
ems = open(WT + '/re15_port/shared_assets/RE2/CDEMD0.EMS', 'rb').read()
for k in range(0x10, 0x10 + 75):
    i = (k - 0x10) * 4
    te = (w[(i + 2) * 2], w[(i + 2) * 2 + 1])
    ee = (w[(i + 3) * 2], w[(i + 3) * 2 + 1])
    if ee[1] == 0:
        continue
    emd = ems[ee[0] * 0x800:ee[0] * 0x800 + ee[1]]
    tim = ems[te[0] * 0x800:te[0] * 0x800 + te[1]] if te[1] else b''
    try:
        f = FE.lese_emd(emd)
    except Exception as e:
        print('kind %02X Fehler %r' % (k, e))
        continue
    nb = len(f['knochen'])
    nf = sum(len(m['dreiecke']) + len(m['vierecke']) for m in f['meshes'][:nb])
    nq = sum(len(m['vierecke']) for m in f['meshes'][:nb])
    print('kind %02X  Knochen %2d  Meshes %2d  Flaechen %4d  davon Vier %4d  Clips %3d  EMD %6d TIM %6d' % (k, nb, len(f['meshes']), nf, nq, len(f['clips']), len(emd), len(tim)))
    open(ZIEL + '/K%02X.EMD' % k, 'wb').write(emd)
    if tim:
        open(ZIEL + '/K%02X.TIM' % k, 'wb').write(tim)
