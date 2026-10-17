"""wicklung_props.py - Runde 38 Spur A: Wicklungs-Konvention der Objekt-Modelle (Nebenwirkung des Rueckseiten-Verwurfs).

Der Verwurf (NCLIP MAC0 < 0 auf v0,v1,v2, FUN_800254a0 @0x800255a8 / FUN_800256b0 @0x800257c0) zeichnet eine Flaeche nur,
wenn ihre Ecken auf dem Bildschirm in der Original-Richtung umlaufen. Bei einem Modell mit gleichmaessiger Wicklung ist
das genau dann der Fall, wenn die Flaeche zur Kamera zeigt. Ein Modell mit UMGEKEHRTER Wicklung wuerde nach dem Verwurf
"von innen" gezeichnet (Vorderseiten weg, Innenseiten sichtbar).

Mass je Flaeche (Modellraum, kameraunabhaengig): w = sign( dot( (v1-v0) x (v2-v0), n0+n1+n2[+n3] ) ) mit den
gespeicherten Normalen (Licht-Normalen, nach aussen). Ausgabe je Modell: Anzahl w>0 / w<0 / w=0.
  1. alle Original-Props aller RDTs (shared_assets/PSX/STAGE*/ROOM*.RDT, Prop-Tabelle RDT+0x30) -> Konvention
  2. alle Port-Modelle aus engine/src/gen/*.inc (C-Felder *_md1*), darunter die zwei NEU GEBAUTEN
     (Sicherung gen/sicherung_prop.inc, Feuerloescher gen/feuerloescher_r37.inc)
Aufruf: python wicklung_props.py"""
import glob, os, re, struct
from collections import Counter

WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_cursor/re15_port'


def faces(d, m):
    """liefert je Flaeche (art, index, w) fuer das MD1 an Offset m."""
    ln, unk, nobj = struct.unpack_from('<III', d, m)
    b = m + 12
    out = []
    for mi in range(nobj // 2):
        tv, tvn, tn, tnn, tf, tfn, tuv, qv, qvn, qn, qnn, qf, qfn, quv = struct.unpack_from('<14I', d, b + mi * 56)
        V = [struct.unpack_from('<hhh', d, b + tv + 8 * i) for i in range(tvn)]
        N = [struct.unpack_from('<hhh', d, b + tn + 8 * i) for i in range(tnn)]
        QV = [struct.unpack_from('<hhh', d, b + qv + 8 * i) for i in range(qvn)]
        QN = [struct.unpack_from('<hhh', d, b + qn + 8 * i) for i in range(qnn)]

        def w(P, NN):
            a = [P[1][k] - P[0][k] for k in range(3)]
            c = [P[2][k] - P[0][k] for k in range(3)]
            g = (a[1] * c[2] - a[2] * c[1], a[2] * c[0] - a[0] * c[2], a[0] * c[1] - a[1] * c[0])
            n = [sum(x[k] for x in NN) for k in range(3)]
            s = g[0] * n[0] + g[1] * n[1] + g[2] * n[2]
            return (s > 0) - (s < 0)

        for i in range(tfn):
            n0, v0, n1, v1, n2, v2 = struct.unpack_from('<6H', d, b + tf + 12 * i)
            if max(v0, v1, v2) >= tvn or max(n0, n1, n2) >= tnn:
                continue
            out.append(('D', mi, i, w([V[v0], V[v1], V[v2]], [N[n0], N[n1], N[n2]])))
        for i in range(qfn):
            n0, v0, n1, v1, n2, v2, n3, v3 = struct.unpack_from('<8H', d, b + qf + 16 * i)
            if max(v0, v1, v2, v3) >= qvn or max(n0, n1, n2, n3) >= qnn:
                continue
            out.append(('V', mi, i, w([QV[v0], QV[v1], QV[v2]], [QN[n0], QN[n1], QN[n2], QN[n3]])))
    return out


def zaehle(fl):
    c = Counter(x[3] for x in fl)
    return c[1], c[-1], c[0]


def original():
    gesamt = Counter()
    modelle = Counter()
    auffaellig = []
    gesehen = set()
    for p in sorted(glob.glob(WT + '/shared_assets/PSX/STAGE*/ROOM*.RDT')):
        d = open(p, 'rb').read()
        if len(d) < 0x100:
            continue
        n_om = d[2]
        tab = struct.unpack_from('<I', d, 0x30)[0]
        for oid in range(n_om):
            try:
                tim, md1 = struct.unpack_from('<II', d, tab + 8 * oid)
                if not md1 or md1 >= len(d):
                    continue
                key = d[md1:md1 + struct.unpack_from('<I', d, md1)[0] + 12]
                if key in gesehen:
                    continue
                gesehen.add(key)
                fl = faces(d, md1)
            except struct.error:
                continue
            pos, neg, null = zaehle(fl)
            gesamt.update({'+': pos, '-': neg, '0': null})
            if pos and neg:
                modelle['gemischt'] += 1
                auffaellig.append('%s obj %d: +%d -%d 0:%d' % (os.path.basename(p), oid, pos, neg, null))
            elif pos:
                modelle['nur +'] += 1
            elif neg:
                modelle['nur -'] += 1
            else:
                modelle['leer/0'] += 1
    return gesamt, modelle, auffaellig


def inc_felder():
    erg = []
    for p in sorted(glob.glob(WT + '/engine/src/gen/*.inc')):
        s = open(p, encoding='utf-8', errors='replace').read()
        for m in re.finditer(r'static const unsigned char (\w*md1\w*)\[(\d*)\]\s*=\s*\{([^}]*)\}', s):
            name = m.group(1)
            b = bytes(int(x, 0) for x in m.group(3).replace('\n', ' ').split(',') if x.strip())
            erg.append((os.path.basename(p), name, b))
    return erg


def main():
    g, mo, auff = original()
    print('ORIGINAL (alle RDT-Props, je Modell einmal): Flaechen w>0 %d, w<0 %d, w=0 %d' % (g['+'], g['-'], g['0']))
    print('  Modelle: %s' % dict(mo))
    for z in auff[:40]:
        print('  gemischt: ' + z)
    print('PORT-MODELLE (engine/src/gen/*.inc):')
    for datei, name, b in inc_felder():
        try:
            fl = faces(b, 0)
        except struct.error as e:
            print('  %-24s %-26s nicht lesbar (%s)' % (datei, name, e))
            continue
        pos, neg, null = zaehle(fl)
        print('  %-24s %-26s w>0 %4d  w<0 %4d  w=0 %4d' % (datei, name, pos, neg, null))


if __name__ == '__main__':
    main()
