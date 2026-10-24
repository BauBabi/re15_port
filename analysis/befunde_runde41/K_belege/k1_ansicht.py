#!/usr/bin/env python3
"""Spur K (Runde 41): Ansichtsblatt eines Typs aus CDEMD0.EMS ODER CDEMD1.EMS.
Wrapper um re15_port/tools/emd_ansichtsblatt.py (Leser = Port der Engine-Leser); nur EMS_PFAD wird umgestellt.
    python k1_ansicht.py <0|1> <typ> <clip> <out.png>
"""
import os, sys, importlib.util
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
spec = importlib.util.spec_from_file_location('eab', os.path.join(REPO, 're15_port', 'tools', 'emd_ansichtsblatt.py'))
eab = importlib.util.module_from_spec(spec); spec.loader.exec_module(eab)
arch, typ, clip, out = int(sys.argv[1]), int(sys.argv[2], 0), int(sys.argv[3], 0), sys.argv[4]
eab.EMS_PFAD = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'EMD', 'CDEMD%d.EMS' % arch)
mo = eab.Modell(typ)
print('CDEMD%d typ 0x%02x: blob_index %d len %d meshes %d bones %d clips %d tex %dx%d'
      % (arch, typ, mo.blob_index, mo.blob_len, len(mo.meshes), mo.skel.bone_count, len(mo.clips), mo.texw, mo.texh))
eab.blatt(mo, clip, out, False)
