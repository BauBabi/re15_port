"""Schluessel-Assets fuer die Figurmasken im echten Spiel: jede CLUT-Farbe != 0 wird 0x7FFF (STP-Bit bleibt), Pixel/UV/Geometrie bleiben.
Normal- und Schluessel-Lauf sind sonst gleich (gleicher Ladeweg: Split-Datei im Ueberlagerungsordner)."""
import os, sys, re, struct, shutil
import numpy as np
WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
sys.path.insert(0, WT + '/re15_port/tools')
import emd_ansichtsblatt as EA
S = sys.argv[1]
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]

def tim_key(b, o):
    """CLUT des TIM an Offset o (Magie 0x10) in bytearray b auf Weiss setzen; Rueckgabe Zahl geaenderter Woerter."""
    assert u32(b, o) == 0x10, hex(u32(b, o))
    fl = u32(b, o + 4); assert fl & 8, 'TIM ohne CLUT'
    cs = u32(b, o + 8); n = (cs - 12) // 2; k = 0
    for i in range(n):
        p = o + 20 + 2 * i
        w = u16(b, p)
        if w != 0:
            struct.pack_into('<H', b, p, 0x7FFF | (w & 0x8000)); k += 1
    return k

def schreibe(p, b):
    os.makedirs(os.path.dirname(p), exist_ok=True); open(p, 'wb').write(bytes(b))

ems = open(EA.EMS_PFAD, 'rb').read()
# RE1.5-Typen: Split-Datei EMD/EM%02X.EMD (Lader pc_enemy_read_re15_emd sucht sie vor dem EMS-Blob)
for name, typen in (('zombie', (0x10, 0x11, 0x16)), ('gorilla', (0x27,)), ('g1', (0x30,)), ('tyrant', (0x2B,)), ('hund15', (0x20,))):
    for art in ('normal', 'key'):
        for t in typen:
            off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(t))
            b = bytearray(ems[off:off + ln])
            d = EA.emd_dir(b)
            k = tim_key(b, d[8]) if art == 'key' else 0
            schreibe('%s/ueber/%s_%s/EMD/EM%02X.EMD' % (S, name, art, t), b)
            print(name, art, hex(t), 'blob', ln, 'clut geaendert', k)
# EM30_HANGAR.EMD (50A0)
b0 = open(WT + '/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD', 'rb').read()
for art in ('normal', 'key'):
    b = bytearray(b0); d = EA.emd_dir(b)
    k = tim_key(b, d[8]) if art == 'key' else 0
    schreibe('%s/ueber/hangar_%s/EMD/EM30_HANGAR.EMD' % (S, art), b); print('hangar', art, k)
# Tyrant im Hangar (Spider-Man EM2B) im selben Ueberlagerungsordner wie EM30, eigener Schluessel-Lauf
for art in ('key',):
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(0x2B))
    b = bytearray(ems[off:off + ln]); d = EA.emd_dir(b); k = tim_key(b, d[8])
    schreibe('%s/ueber/hangar_key2b/EMD/EM2B.EMD' % S, b)
    schreibe('%s/ueber/hangar_key2b/EMD/EM30_HANGAR.EMD' % S, b0); print('hangar key2b', k)
# RE2 EM36 (G5) im RE2-CDEMD0.EMS: eigene Kopie mit Schluessel-CLUT (RE15_RE2_ASSET_ROOT)
inc = open(WT + '/re15_port/engine/src/gen/re2_ems_toc.inc', encoding='utf-8', errors='replace').read()
toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S).group(1))]
e2 = bytearray(open(WT + '/re15_port/shared_assets/RE2/CDEMD0.EMS', 'rb').read())
for kind in (0x36,):
    for r in range(4):
        i = ((kind - 0x10) * 4 + r) * 2
        o, ln = toc[i] * 0x800, toc[i + 1]
        if ln and u32(e2, o) == 0x10:
            print('RE2 kind', hex(kind), 'rec', r, 'TIM @', hex(o), 'geaendert', tim_key(e2, o))
schreibe('%s/ueber/g5_key/CDEMD0.EMS' % S, e2)
# Bahnsteig-Form: EMD + Schluessel-TIM je Variante
K5 = WT + '/analysis/befunde_runde41/form/varianten/K5'
for name, d in (('k5e_f1', K5 + '/ruhe/K5e_f1'), ('k5c', K5 + '/einbau_k5c')):
    for art in ('normal', 'key'):
        t = bytearray(open(d + '/BAHNSTEIG_FORM.TIM', 'rb').read())
        k = tim_key(t, 0) if art == 'key' else 0
        schreibe('%s/ueber/form_%s_%s/BAHNSTEIG_FORM.TIM' % (S, name, art), t)
        shutil.copy(d + '/BAHNSTEIG_FORM.EMD', '%s/ueber/form_%s_%s/BAHNSTEIG_FORM.EMD' % (S, name, art))
        print('form', name, art, k)
