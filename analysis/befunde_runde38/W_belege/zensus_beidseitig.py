"""zensus_beidseitig.py - Runde 38 Spur W, Nachbesserung 1 (Abnahme-Mangel M2): wo zeichnet der Port mit dem
Original-Polygon-Weg (objpoly_r38.c) ANDERS als bis v0.8.25 — in BEIDE Richtungen, mit Typ 3 und Eltern-Objekten.

Gegenueber zensus_objpoly.py (Bau-Agent, nur WENIGER, ohne Typ 3 / Eltern):
  * MEHR:    NEU bedeckt > ALT + 300 px (Polygone, die der alte Port-Ausschnitt `_vz < 64` ganz verwarf und die der
             Original-Weg mit gesaettigten SXY zeichnet, wenn NCLIP >= 0, OTZ >= 64 und ein GPU-Dreieck besteht).
  * WENIGER: ALT - NEU > 1536 px oder NEU < 50 % von ALT bei ALT > 1000 px (wie bisher).
  * Typ 3 (FUN_8003e64c, Kachel-Objekte): der Port zeichnet sie ohne Objekt-Ausschnitt (re15_prop_culled Typ 3 -> 0)
    in JEDEM Cut; der Zensus rechnet sie darum fuer alle Cuts.
  * Eltern-Objekte (Obj_model_set pc[5] & 0xC0 == 0xC0, ROOM1150/1151 obj 1/2 auf obj 0): Weltmatrix = Elternmatrix x
    lokale Matrix (LAB_80040914 @0x80040a84, FUN_8002c18c FUN_80022da0 @0x8002c22c), Lage aus den Saetzen.
    pc[5] = 0x80 (Figuren-Anhang, ROOM4030/4031 obj 0) setzt der Port nicht um (re15_scd.h) -> gemeldet, nicht gerechnet.
  * Zusaetzlich: jedes HALBTRANSPARENTE Objekt (Attribut & 2) mit NEU-Bedeckung > 0 — dort zeigt der Port seit
    Nachbesserung 1 (objabe_r38) die Original-Mischung statt der deckenden Flaeche.
Aufruf: python zensus_beidseitig.py <PSX-Wurzel> [ausgabe.txt]"""
import os, sys, struct, hashlib

HERE = os.path.dirname(os.path.abspath(__file__))
_src = open(os.path.join(HERE, 'zensus_objpoly.py'), encoding='utf-8').read()
_src = _src[:_src.rfind('main()')]
Z = {'__name__': 'zensus_objpoly', '__file__': os.path.join(HERE, 'zensus_objpoly.py')}
exec(compile(_src, 'zensus_objpoly.py', 'exec'), Z)
L = Z['L']


def mat_mul(A, B):
    return [sum(A[i * 3 + k] * B[k * 3 + j] for k in range(3)) >> 12 for i in range(3) for j in range(3)]


def main():
    root = sys.argv[1]
    out = open(sys.argv[2], 'w', encoding='utf-8') if len(sys.argv) > 2 else sys.stdout
    seen = {}
    mehr, weniger, abe_sicht, nicht = [], [], [], []
    summe = dict(kombis=0, alt=0, neu=0, typ3=0, eltern=0)
    for st in range(1, 7):
        sd = os.path.join(root, 'STAGE%d' % st)
        if not os.path.isdir(sd):
            continue
        for fn in sorted(os.listdir(sd)):
            if not (fn.upper().startswith('ROOM') and fn.upper().endswith('.RDT')):
                continue
            d = open(os.path.join(sd, fn), 'rb').read()
            if len(d) < 0x60:
                continue
            h = hashlib.md5(d).hexdigest()
            if h in seen:
                seen[h].append(fn[4:8])
                continue
            seen[h] = [fn[4:8]]
            rid = fn[4:8]
            ncut = d[1]
            camoff = struct.unpack_from('<I', d, 0x24)[0]
            tbl = struct.unpack_from('<I', d, 0x30)[0]
            nO = d[2]
            regs = Z['regions'](d)
            recs = {}
            for (tag, idx), ops in L.regionen(d).items():
                for (pc, op, sz) in ops:
                    if op == 0x2D and pc + 34 <= len(d):
                        recs.setdefault(d[pc:pc + 34], pc)
            # erste Lage je Objekt (fuer Eltern-Verkettung)
            lage = {}
            for rec, pc in sorted(recs.items(), key=lambda kv: kv[1]):
                if rec[1] not in lage:
                    lage[rec[1]] = rec
            modelle = {}
            for rec, pc in sorted(recs.items(), key=lambda kv: kv[1]):
                oid, typ, par = rec[1], rec[2], rec[5]
                attr = rec[8] | (rec[9] << 8)
                if oid >= nO:
                    continue
                if (par & 0xC0) in (0x40, 0x80):
                    nicht.append('ROOM%s obj %d @0x%05X pc[5]=0x%02x (Figuren-/Anker-Anhang, im Port nicht umgesetzt)'
                                 % (rid, oid, pc, par))
                    continue
                x, y, z, rx, ry, rz = struct.unpack_from('<6h', rec, 10)
                W = Z['prop_rot'](rx, ry, rz)
                pos = [x, y, z]
                eltern = (par & 0xC0) == 0xC0
                if eltern:
                    p = lage.get(par - 0xC0)
                    if p is None:
                        nicht.append('ROOM%s obj %d @0x%05X: Elternobjekt %d ohne Satz' % (rid, oid, pc, par - 0xC0))
                        continue
                    px, py, pz, prx, pry, prz = struct.unpack_from('<6h', p, 10)
                    PW = Z['prop_rot'](prx, pry, prz)
                    pos = [px + ((PW[0] * x + PW[1] * y + PW[2] * z) >> 12),
                           py + ((PW[3] * x + PW[4] * y + PW[5] * z) >> 12),
                           pz + ((PW[6] * x + PW[7] * y + PW[8] * z) >> 12)]
                    W = mat_mul(PW, W)
                    summe['eltern'] += 1
                if typ == 4:
                    pos[1] -= 900
                if oid not in modelle:
                    try:
                        mo = struct.unpack_from('<I', d, tbl + oid * 8 + 4)[0]
                        modelle[oid] = Z['md1_polys'](d, mo) if mo else []
                    except Exception:
                        modelle[oid] = []
                polys = modelle[oid]
                if not polys:
                    continue
                if typ == 3:
                    summe['typ3'] += 1
                for cut in range(ncut):
                    if typ != 3 and cut in regs and not Z['inside_quad'](pos[0], pos[2], regs[cut]):
                        continue
                    cam = struct.unpack_from('<HH6i', d, camoff + cut * 0x20)
                    v = Z['view'](cam)
                    if v is None:
                        continue
                    R, T, H = v
                    C, Ct = Z['compose'](R, T, W, pos)
                    ia, na = Z['bedeckung'](polys, C, Ct, H, False)
                    ib, nb = Z['bedeckung'](polys, C, Ct, H, True)
                    pa = sum(ia.getdata()); pb = sum(ib.getdata())
                    summe['kombis'] += 1; summe['alt'] += pa; summe['neu'] += pb
                    zeile = (rid, '/'.join(seen[h]), oid, typ, attr, par, cut, pos, pa, na, pb, nb)
                    if pb - pa > 300:
                        mehr.append(zeile)
                    if pa - pb > 1536 or (pa > 1000 and pb < pa * 0.5):
                        weniger.append(zeile)
                    if (attr & 2) and pb > 0:
                        abe_sicht.append(zeile)

    def fmt(z):
        rid, var, oid, typ, attr, par, cut, pos, pa, na, pb, nb = z
        return ('ROOM%s obj %2d Typ %d attr 0x%04x%s Cut %2d @(%d,%d,%d): ALT %5d px (%3d Poly) -> NEU %5d px (%3d Poly)'
                % (rid, oid, typ, attr, ' Eltern %d' % (par - 0xC0) if (par & 0xC0) == 0xC0 else '', cut,
                   pos[0], pos[1], pos[2], pa, na, pb, nb))

    out.write('Inhaltsverschiedene RDTs: %d; Kombinationen Objekt x Cut: %d (davon Typ-3-Saetze %d, Eltern-Saetze %d)\n'
              % (len(seen), summe['kombis'], summe['typ3'], summe['eltern']))
    out.write('Summe Bildbedeckung ALT %d px, NEU %d px\n' % (summe['alt'], summe['neu']))
    out.write('\nMEHR (NEU > ALT + 300 px): %d\n' % len(mehr))
    for z in sorted(mehr, key=lambda z: -(z[10] - z[8])):
        out.write('  ' + fmt(z) + ', mehr %d px\n' % (z[10] - z[8]))
    out.write('\nWENIGER (ALT - NEU > 1536 px oder NEU < 50 %% von ALT > 1000 px): %d\n' % len(weniger))
    for z in sorted(weniger, key=lambda z: -(z[8] - z[10])):
        out.write('  ' + fmt(z) + ', weniger %d px\n' % (z[8] - z[10]))
    out.write('\nHALBTRANSPARENT sichtbar (Attribut & 2, NEU > 0 px) - dort mischt der Port seit Nachbesserung 1: %d\n'
              % len(abe_sicht))
    for z in sorted(abe_sicht):
        out.write('  ' + fmt(z) + '\n')
    out.write('\nNicht gerechnet: %d\n' % len(nicht))
    for n in nicht:
        out.write('  ' + n + '\n')
    out.flush()


main()
