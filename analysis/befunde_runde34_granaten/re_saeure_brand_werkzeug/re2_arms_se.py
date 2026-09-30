#!/usr/bin/env python3
"""Runde 34 / re_saeure_brand: RE2-SE-Code -> Sample aufloesen (ARMS-Banken der GL-Runden).

SE-Spieler FUN_8005ba28 (RE2 PSX.EXE, RE2_Quellcode_V2/FUN_8005ba28.c, Kern gegen Disasm geprueft):
  bank = code >> 24, record = (code >> 16) & 0xFF
  desc = *(u32*)(DAT_800dbb78[bank] + record*4); desc == 0xFFFFFFFF -> KEIN Ton (return)
  prog = desc.b1 & 0x7F, tone = desc.b2 >> 4, (desc.b3 & 0x1F) = Stimme/Kanal
  Tonattribut = DAT_800d75a0[bank] + prog*0x200 + tone*0x20 + 0x820
Die EDH-Datei traegt vorne die Deskriptor-Tabelle (Laenge = erstes Byte des 8-Byte-Trailers,
hier 0x80 = 32 Records), danach den Sony-VAB-Kopf ('pBAV', RE15_KNOWLEDGE.md §1.9).
Dieses Skript liest je Datei die Records und, fuer die gesuchten Records, prog/tone und den
VAG-Index des Tonattributs (VagAtr +22 = vag, 1-basiert) -> <BANK>_<vag-1>.wav.

Aufruf: re2_arms_se.py [EDH ...]   (Default: ARMS09/0A/0B)
"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
SND = os.path.join(REPO, 'info', 're2leon', 'COMMON', 'SOUND')


def parse(path):
    d = open(path, 'rb').read()
    pre = d[-8]
    recs = [struct.unpack_from('<I', d, 4 * i)[0] for i in range(pre // 4)]
    vab = pre
    magic = d[vab:vab + 4]
    ps, ts, vs = struct.unpack_from('<HHH', d, vab + 0x12)
    prog_base = vab + 0x20
    tone_base = prog_base + 128 * 16
    # VAG-Groessentabelle (256 x u16, Einheit 8 Byte) hinter den Tonattributen (ps*16*32 Byte).
    sz_off = tone_base + ps * 16 * 32
    sizes = [struct.unpack_from('<H', d, sz_off + 2 * i)[0] * 8 for i in range(vs + 1)]
    return d, pre, recs, magic, ps, ts, vs, prog_base, tone_base, sizes


def tone_attr(d, prog_base, tone_base, prog, tone):
    # ProgAtr: +0 tones(u8)... Die Tonattribute liegen dicht je benutztem Programm (16 je Programm).
    idx = prog * 16 + tone
    o = tone_base + idx * 32
    if o + 32 > len(d):
        return None
    vag = struct.unpack_from('<h', d, o + 22)[0]
    center = d[o + 4]
    shift = d[o + 5]
    vol = d[o + 2]
    return dict(off=o, vag=vag, center=center, shift=shift, vol=vol, raw=d[o:o + 32].hex())


def main():
    files = sys.argv[1:] or ['ARMS09.EDH', 'ARMS0A.EDH', 'ARMS0B.EDH']
    for f in files:
        p = f if os.path.isabs(f) else os.path.join(SND, f)
        d, pre, recs, magic, ps, ts, vs, pb, tb, sizes = parse(p)
        print('%s: Deskriptor-Tabelle %d Records, VAB %s ps=%d ts=%d vs=%d, VAG-Groessen %s'
              % (os.path.basename(p), pre // 4, magic, ps, ts, vs, sizes))
        # Die extrahierten *_NNNNN.wav ueberspringen die 48-Byte-Stumm-VAG 1 (Groessenabgleich:
        # wav-Samples*16/28 + 16 == VAG-Bytes) -> wav-Index = Zahl der VAGs < vag mit Groesse > 48.
        def wav_of(v):
            if v <= 0 or v >= len(sizes) or sizes[v] <= 48:
                return None
            return sum(1 for j in range(1, v) if sizes[j] > 48)
        for i, r in enumerate(recs):
            if r == 0xFFFFFFFF:
                continue
            b = struct.pack('<I', r)
            prog = b[1] & 0x7F
            tone = b[2] >> 4
            ta = tone_attr(d, pb, tb, prog, tone)
            code = 0x01000001 | (i << 16)
            wi = wav_of(ta['vag']) if ta else None
            wav = ('%s_%05d.wav (VAG %d B)' % (os.path.basename(p)[:-4], wi, sizes[ta['vag']])) if wi is not None else '?'
            print('  Record %2d (SE 0x%08X): desc %s  prog %d tone %d  -> VagAtr @0x%X vag=%s  %s'
                  % (i, code, b.hex(), prog, tone, ta['off'] if ta else 0, ta['vag'] if ta else '?', wav))


if __name__ == '__main__':
    main()
