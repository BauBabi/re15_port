#!/usr/bin/env python3
"""Runde 30 / tuer-verschlossen: RE2-Raumbank (RDT snd0 = Se_on-Bank 2) lesen.
RDT-Kopf +0x08/+0x0C/+0x10 = EDT/VH/VB (Lader FUN_80059e54: DAT_800dbb80 = [RDT+8],
DAT_800d75a8 = [RDT+0xC], SsVabTransBody([RDT+0x10])).
EDT-Satz (Se_on FUN_8005ba28): b0 bit7 = VAB-Override, b1&0x7f = prog, b2>>4 = tone,
b2&0xf = prio, b3&0x1f = Stimme, b3>>5 = Zusatzlagen. 0xFFFFFFFF = leer (Se_on kehrt um).
Tone-Satz @VH+0x820+prog*0x200+tone*0x20, VAG-Index (1-basiert) = u16 @+0x16.
"""
import struct, hashlib, os, sys, glob
def load_bank(path):
    d = open(path, 'rb').read()
    edt, vh, vb = struct.unpack_from('<3I', d, 8)
    if vh == 0 or edt == 0 or d[vh:vh+4] != b'pBAV':
        return None
    nprog, ntone, nvag = struct.unpack_from('<3H', d, vh + 0x12)
    n = (vh - edt) // 4
    sizetab = vh + 0x820 + nprog * 0x200
    sizes = [struct.unpack_from('<H', d, sizetab + i*2)[0] << 3 for i in range(256)]
    offs = [0]*257
    acc = vb
    # Eintrag 0 ist der Vorspann (Groesse 0); VAG k (1-basiert) beginnt nach sum(sizes[0..k-1])
    pos = vb
    vag = {}
    for k in range(0, nvag + 1):
        if k >= 1:
            vag[k] = (pos, sizes[k])
        pos += sizes[k]
    return dict(d=d, edt=edt, vh=vh, vb=vb, n=n, nprog=nprog, ntone=ntone, nvag=nvag, vag=vag)
def rec(b, i):
    if i >= b['n']: return None
    r = b['d'][b['edt']+i*4: b['edt']+i*4+4]
    return r
def resolve(b, i):
    r = rec(b, i)
    if r is None: return None
    if r == b'\xff\xff\xff\xff': return dict(raw=r, empty=True)
    prog = r[1] & 0x7f; tone = r[2] >> 4; prio = r[2] & 0xf; voice = r[3] & 0x1f; extra = r[3] >> 5
    out = dict(raw=r, empty=False, prog=prog, tone=tone, prio=prio, voice=voice, extra=extra,
               override=(r[0] & 0x7f) if (r[0] & 0x80) else None, layers=[])
    for L in range(extra + 1):
        ta = b['vh'] + 0x820 + prog*0x200 + (tone+L)*0x20
        t = b['d'][ta:ta+32]
        vagi = struct.unpack_from('<H', t, 0x16)[0]
        lay = dict(tone_off=ta, vol=t[2], pan=t[3], center=t[4], shift=t[5], mn=t[6], mx=t[7],
                   adsr1=struct.unpack_from('<H', t, 0x10)[0], adsr2=struct.unpack_from('<H', t, 0x12)[0], vag=vagi)
        if vagi in b['vag']:
            o, s = b['vag'][vagi]
            w = b['d'][o:o+s]
            lay.update(vag_off=o, vag_size=s, sha1=hashlib.sha1(w).hexdigest())
        out['layers'].append(lay)
    return out
if __name__ == '__main__':
    p = sys.argv[1]
    b = load_bank(p)
    print('%s: EDT=0x%X n=%d VH=0x%X VB=0x%X nprog=%d ntone=%d nvag=%d' % (os.path.basename(p), b['edt'], b['n'], b['vh'], b['vb'], b['nprog'], b['ntone'], b['nvag']))
    for i in range(b['n']):
        r = resolve(b, i)
        if r['empty']: continue
        for L in r['layers']:
            print('  se 0x%02x raw %s prog%d tone%d prio%d voice%d extra%d  tone@0x%X vol%d pan%d center%d shift%d min%d max%d vag%d @0x%X %dB sha1 %s' % (
                i, r['raw'].hex(' '), r['prog'], r['tone'], r['prio'], r['voice'], r['extra'], L['tone_off'], L['vol'], L['pan'], L['center'], L['shift'], L['mn'], L['mx'], L['vag'], L.get('vag_off', -1), L.get('vag_size', -1), L.get('sha1', '?')[:12]))
