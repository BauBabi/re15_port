"""elza_props_vergleich.py - Runde 38 Spur A: welche Elza-Raeume (ROOMxxx1) tragen andere Raum-Objekte als die
Leon-Variante (ROOMxxx0)? Der Rueckseiten-Verwurf wirkt nur auf Raum-Objekte (Obj_model_set + Prop-Tabelle RDT+0x30);
wo Modelle (MD1-Bytes) und Obj_model_set-Saetze (Opcode 0x2D, 34 Byte, Byte-Muster) gleich sind, ist die Wirkung
dieselbe wie im Leon-Scan (raum_scan_120.txt). Ausgabe: je Raum gleich/verschieden.
Aufruf: python elza_props_vergleich.py"""
import glob, os, struct

WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_cursor/re15_port/shared_assets/PSX'


def props(p):
    d = open(p, 'rb').read()
    if len(d) < 0x100:
        return None
    n_om = d[2]
    tab = struct.unpack_from('<I', d, 0x30)[0]
    md1s = []
    for oid in range(n_om):
        tim, md1 = struct.unpack_from('<II', d, tab + 8 * oid)
        ln = struct.unpack_from('<I', d, md1)[0] if md1 and md1 + 4 <= len(d) else 0
        md1s.append(d[md1:md1 + 12 + 56 * 2 + 4096] if md1 else b'')   # Kopf + Anfang reicht zum Vergleich
    saetze = sorted(d[o:o + 34] for o in range(len(d) - 34) if d[o] == 0x2D and d[o + 1] < n_om and d[o + 2] in (0, 1, 2, 3, 4))
    return md1s, saetze


def main():
    gleich, anders, fehlt = [], [], []
    for st in range(1, 7):
        for p in sorted(glob.glob(WT + '/STAGE%d/ROOM*0.RDT' % st)):
            q = p[:-5] + '1.RDT'
            r = os.path.basename(p)[4:8]
            if not os.path.exists(q):
                fehlt.append(r)
                continue
            a, b = props(p), props(q)
            if a is None or b is None:
                fehlt.append(r)
            elif a == b:
                gleich.append(r)
            else:
                anders.append(r + ('(Modelle)' if a[0] != b[0] else '') + ('(Saetze)' if a[1] != b[1] else ''))
    print('gleich (%d): %s' % (len(gleich), ' '.join(gleich)))
    print('verschieden (%d): %s' % (len(anders), ' '.join(anders)))
    print('ohne Elza-Variante/leer (%d): %s' % (len(fehlt), ' '.join(fehlt)))


if __name__ == '__main__':
    main()
