"""zensus_abe.py - Runde 38 Spur W, Nachbesserung 1 (M1): alle halbtransparenten Raum-Objekte (Attribut & 2).

Original-Regel (PSX.EXE, selbst disassembliert, Dossier W §10.2):
  ABE  = Attribut & 2           FUN_8002c18c @0x8002c2b4 `andi v0,v0,0x2` / @0x8002c2b8 `sltu s2,zero,v0`
         -> Parameter 4 der vier Zeichner, Code |= ABE << 1 (@0x80022f18/1c, @0x800230ac/b0, @0x800254ac/b0,
            @0x800256bc/c0); Typ 3 ueber FUN_8003e64c @0x8002c2cc (a1 = s2) -> @0x8003e66c/@0x8003e6b8 (a3).
  ABR  = Texpage-Bits 5-6 des Primitivs = (MD1-Seite | (Attribut & 0xC) << 3)
         FUN_80025940 (GT3) @0x800259bc `lw v0,0(a0)` (Attribut) / @0x800259c4 `andi v0,v0,0xc` /
         @0x800259cc `sll t7,v0,19` / @0x80025a14 `or v0,t7,v0` / @0x80025a18 `sw v0,-8(a3)` (Wort 6 = uv1+tpage);
         FUN_80025a98 (GT4) @0x80025b1c/@0x80025b24/@0x80025b2c/@0x80025b70/@0x80025b74.
  STP  = Bit 15 des CLUT-Eintrags je Texel (psx-spx GPU "Semi-transparency": nur Texel mit STP = 1 werden gemischt,
         0000h ist ganz durchsichtig).
Ausgabe je Obj_model_set mit Attribut & 2 (inhaltsgleiche RDTs einmal): Raum, Objekt, Typ, Attribut, Satz-Offset,
Seitenwoerter der Polygone, ABR (Seite / Attribut / wirksam), Polygone, und die Texel unter den UV-Dreiecken aller
Polygone (Viereck = Dreiecke (0,1,2)+(1,2,3) wie die GPU): STP=1 / STP=0 / 0000h.
Aufruf: python zensus_abe.py <PSX-Wurzel> [ausgabe.txt]"""
import os, sys, struct, hashlib

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))
import scd_walk_lib as L  # noqa: E402


def tim_lesen(d, off):
    t = d[off:]
    magic, flags = struct.unpack_from('<II', t, 0)
    bpp = flags & 3
    o = 8
    clut = None
    cx = cy = cw = ch = 0
    if flags & 8:
        clut_len, cx, cy, cw, ch = struct.unpack_from('<IHHHH', t, o)
        clut = list(struct.unpack_from('<%dH' % (cw * ch), t, o + 12))
        o += clut_len
    img_len, ix, iy, iw, ih = struct.unpack_from('<IHHHH', t, o)
    pix = t[o + 12:o + img_len]
    return dict(bpp=bpp, clut=clut, cx=cx, cy=cy, cw=cw, ch=ch, ix=ix, iy=iy, iw=iw, ih=ih, pix=pix)


def texel(tim, page, clutw, u, v):
    """16-Bit-Farbe des Texels (u,v) der Seite `page` (Seite relativ zur TIM, wie im MD1; FUN_80022150 verschiebt
    nur Seite/CLUT als Ganzes) ueber den CLUT-Eintrag `clutw`."""
    bpp = tim['bpp']
    tpx = page & 0xF
    if bpp == 1:            # 8 Bit: 2 Texel je Halbwort, Seite = 64 Halbworte = 128 Texel
        x = tpx * 128 + u
        w = tim['iw'] * 2
        if v >= tim['ih'] or x >= w:
            return None
        idx = tim['pix'][v * w + x]
    elif bpp == 0:          # 4 Bit: 4 Texel je Halbwort, Seite = 256 Texel
        x = tpx * 256 + u
        w = tim['iw'] * 4
        if v >= tim['ih'] or x >= w:
            return None
        b = tim['pix'][(v * w + x) // 2]
        idx = (b >> 4) if (x & 1) else (b & 0xF)
    else:
        return None
    crow = (clutw >> 6) - tim['cy']
    ccol = (clutw & 0x3F) * 16 - tim['cx']
    k = crow * tim['cw'] + ccol + idx
    if tim['clut'] is None or k < 0 or k >= len(tim['clut']):
        return None
    return tim['clut'][k]


def in_dreieck(px, py, a, b, c):
    def s(p, q, r):
        return (q[0] - p[0]) * (r[1] - p[1]) - (q[1] - p[1]) * (r[0] - p[0])
    d1, d2, d3 = s(a, b, (px, py)), s(b, c, (px, py)), s(c, a, (px, py))
    return (d1 >= 0 and d2 >= 0 and d3 >= 0) or (d1 <= 0 and d2 <= 0 and d3 <= 0)


def md1_uvs(d, off):
    """[(page, clut, [(u,v) ...]), ...] aller Polygone (Dreiecke 3, Vierecke 4 Scheitel)."""
    m = d[off:]
    objc = struct.unpack_from('<I', m, 8)[0]
    out = []
    for i in range(objc // 2):
        h = struct.unpack_from('<14I', m, 12 + i * 56)
        tv, tvc, tn, tnc, tf, tfc, tu, qv, qvc, qn, qnc, qf, qfc, qu = h
        for k in range(tfc):
            u0, v0, clutw, u1, v1, page, u2, v2 = struct.unpack_from('<BBHBBHBB', m, 12 + tu + k * 12)
            out.append((page, clutw, [(u0, v0), (u1, v1), (u2, v2)]))
        for k in range(qfc):
            u0, v0, clutw, u1, v1, page, u2, v2, _, u3, v3 = struct.unpack_from('<BBHBBHBBHBB', m, 12 + qu + k * 16)
            out.append((page, clutw, [(u0, v0), (u1, v1), (u2, v2), (u3, v3)]))
    return out


def stp_zaehlen(tim, polys):
    n = {'stp1': 0, 'stp0': 0, 'null': 0, 'aus': 0}
    for page, clutw, uv in polys:
        tris = [uv] if len(uv) == 3 else [uv[0:3], uv[1:4]]
        gesehen = set()
        for tri in tris:
            xs = [p[0] for p in tri]; ys = [p[1] for p in tri]
            for vy in range(min(ys), max(ys) + 1):
                for vx in range(min(xs), max(xs) + 1):
                    if (vx, vy) in gesehen or not in_dreieck(vx, vy, *tri):
                        continue
                    gesehen.add((vx, vy))
                    c = texel(tim, page, clutw, vx, vy)
                    if c is None:
                        n['aus'] += 1
                    elif c == 0:
                        n['null'] += 1
                    elif c & 0x8000:
                        n['stp1'] += 1
                    else:
                        n['stp0'] += 1
    return n


def main():
    root = sys.argv[1]
    out = open(sys.argv[2], 'w', encoding='utf-8') if len(sys.argv) > 2 else sys.stdout
    seen = {}
    zeilen = []
    n_rec = n_abe = 0
    abr_zahl = {}
    stp_summe = {'stp1': 0, 'stp0': 0, 'null': 0, 'aus': 0}
    attr_zahl = {}
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
            tbl = struct.unpack_from('<I', d, 0x30)[0]
            nO = d[2]
            recs = {}
            for (tag, idx), ops in L.regionen(d).items():
                for (pc, op, sz) in ops:
                    if op == 0x2D and pc + 34 <= len(d):
                        recs.setdefault(d[pc:pc + 34], pc)
            for rec, pc in sorted(recs.items(), key=lambda kv: kv[1]):
                n_rec += 1
                oid, typ, par = rec[1], rec[2], rec[5]
                attr = rec[8] | (rec[9] << 8)
                attr_zahl[attr] = attr_zahl.get(attr, 0) + 1
                if not (attr & 2):
                    continue
                n_abe += 1
                if oid >= nO:
                    zeilen.append((fn[4:8], oid, typ, attr, pc, 'Objekt-Nr >= nOmodel', None, None, None))
                    continue
                tim_off, md1_off = struct.unpack_from('<II', d, tbl + oid * 8)
                polys = md1_uvs(d, md1_off) if md1_off else []
                tim = tim_lesen(d, tim_off) if tim_off else None
                seiten = sorted(set(p[0] for p in polys))
                abr_seite = sorted(set((p >> 5) & 3 for p in seiten))
                abr_attr = (attr >> 2) & 3
                abr_eff = sorted(set(((p | ((attr & 0xC) << 3)) >> 5) & 3 for p in seiten))
                for a in abr_eff:
                    abr_zahl[a] = abr_zahl.get(a, 0) + 1
                n = stp_zaehlen(tim, polys) if tim else None
                if n:
                    for k in stp_summe:
                        stp_summe[k] += n[k]
                zeilen.append((fn[4:8], oid, typ, attr, pc, seiten, (abr_seite, abr_attr, abr_eff), len(polys),
                               (n, tim['bpp'] if tim else None)))
    out.write('Inhaltsverschiedene RDTs: %d, Obj_model_set-Saetze (je RDT verschieden): %d, davon Attribut & 2: %d\n'
              % (len(seen), n_rec, n_abe))
    out.write('Attribut-Werte (Saetze): %s\n' % ', '.join('0x%04x x%d' % (a, n) for a, n in sorted(attr_zahl.items())))
    out.write('wirksame ABR der ABE-Saetze (Saetze je Wert): %s\n'
              % ', '.join('ABR%d x%d' % (a, n) for a, n in sorted(abr_zahl.items())))
    out.write('Texel unter allen ABE-Polygonen: STP=1 %d, STP=0 %d, 0000h %d, ausserhalb der TIM %d\n'
              % (stp_summe['stp1'], stp_summe['stp0'], stp_summe['null'], stp_summe['aus']))
    for z in zeilen:
        rid, oid, typ, attr, pc, seiten, abr, npoly, stp = z
        varianten = [r for hh, rr in seen.items() for r in rr if rr[0] == rid]
        if abr is None:
            out.write('  ROOM%s obj %2d typ %d attr 0x%04x @0x%05X: %s\n' % (rid, oid, typ, attr, pc, seiten))
            continue
        n, bpp = stp
        out.write('  ROOM%s (%s) obj %2d typ %d attr 0x%04x @0x%05X: Seiten %s ABR Seite %s | Attribut %d -> wirksam %s,'
                  ' %d Polygone, %d-Bit-TIM, Texel STP=1 %d STP=0 %d 0000h %d aus %d\n'
                  % (rid, '/'.join(varianten), oid, typ, attr, pc, ','.join('0x%04x' % s for s in seiten), abr[0],
                     abr[1], abr[2], npoly, 8 if bpp == 1 else 4, n['stp1'], n['stp0'], n['null'], n['aus']))
    out.flush()


main()
