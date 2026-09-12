#!/usr/bin/env python3
"""ENEMSE-Baenke 17/67/68 dumpen, erste Haelften vergleichen, SEs als WAV rendern."""
import struct, os, math, hashlib, wave

REPO = "c:/workspace/git/reAi_v2"
EXE  = open(os.path.join(REPO, "info", "re2leon", "PSX.EXE"), "rb").read()
TADDR = struct.unpack_from("<I", EXE, 0x18)[0]
def fo(a): return 0x800 + (a - TADDR)
VBS = open(os.path.join(REPO, "re15_port", "shared_assets", "RE2", "ENEMSE.VBS"), "rb").read()
OUT = os.path.join(REPO, "analysis", "befunde_runde4_2026-09-12", "gator_se_wavs")
os.makedirs(OUT, exist_ok=True)

TOC = fo(0x800A7B1C)   # 73 Baenke x 0x10
def toc_entry(bank):
    o = TOC + bank * 0x10
    edt_size = struct.unpack_from("<I", EXE, o)[0]
    edt_sec  = struct.unpack_from("<H", EXE, o+4)[0] | (EXE[o+6] << 16)
    vbd_size = struct.unpack_from("<I", EXE, o+8)[0]
    vbd_sec  = struct.unpack_from("<H", EXE, o+12)[0] | (EXE[o+14] << 16)
    return edt_size, edt_sec*0x800, vbd_size, vbd_sec*0x800

# ---- SPU-ADPCM ----
F1 = [0, 60, 115, 98, 122]
F2 = [0, 0, -52, -55, -60]
def adpcm_decode(data):
    out = []
    h0 = h1 = 0
    for b in range(0, len(data) - 15, 16):
        blk = data[b:b+16]
        shift = blk[0] & 0x0F
        filt  = (blk[0] >> 4) & 0x0F
        flags = blk[1]
        if filt > 4: filt = 0
        for i in range(14):
            byte = blk[2+i]
            for nib in (byte & 0x0F, byte >> 4):
                s = nib << 12
                if s & 0x8000: s -= 0x10000
                s >>= shift
                s += (h0 * F1[filt] + h1 * F2[filt] + 32) >> 6
                s = max(-32768, min(32767, s))
                out.append(s)
                h1, h0 = h0, s
        if flags & 1:  # end flag
            break
    return out

def parse_bank(bank):
    edt_size, edt_off, vbd_size, vbd_off = toc_entry(bank)
    edt = VBS[edt_off:edt_off+edt_size]
    vh_off = struct.unpack_from("<I", edt, edt_size-8)[0]
    vh = edt[vh_off:]
    assert vh[:4] == b'pBAV', (bank, vh[:4])
    progs = struct.unpack_from("<H", vh, 0x12)[0]
    tones = struct.unpack_from("<H", vh, 0x14)[0]
    vags  = struct.unpack_from("<H", vh, 0x16)[0]
    map_entries = [struct.unpack_from("<I", edt, i*4)[0] for i in range(vh_off // 4)]
    # Tone-Tabelle @0x820, 0x200 je Programm; VAG-Groessen-Tabelle danach (u16, /8... eigentlich *8)
    tone_tab = 0x820
    vag_tab = 0x820 + 0x200 * progs
    sizes = [struct.unpack_from("<H", vh, vag_tab + 2*i)[0] * 8 for i in range(vags + 1)]
    offs = []
    acc = 0
    for i in range(1, vags + 1):
        offs.append(acc); acc += sizes[i]
    tone_list = []
    for p in range(progs):
        for t in range(16):
            o = tone_tab + p*0x200 + t*0x20
            prior, mode, vol, pan, center, shift, mn, mx = vh[o:o+8]
            vag = struct.unpack_from("<H", vh, o+22)[0]
            tone_list.append(dict(prog=p, tone=t, vol=vol, pan=pan, center=center,
                                  shift=shift, min=mn, max=mx, vag=vag))
    return dict(bank=bank, edt_size=edt_size, edt_off=edt_off, vbd_size=vbd_size,
                vbd_off=vbd_off, vh_off=vh_off, progs=progs, tones=tones, vags=vags,
                map=map_entries, sizes=sizes[1:], offs=offs, tone_list=tone_list)

def decode_map(e):
    if e == 0xFFFFFFFF: return None
    b0, b1, b2, b3 = e & 0xFF, (e>>8)&0xFF, (e>>16)&0xFF, (e>>24)&0xFF
    return dict(vab_ov=(b0 & 0x7F) if (b0 & 0x80) else -1, prog=b1 & 0x7F,
                tone=b2 >> 4, prio=b2 & 0xF, chan=b3 & 0x1F, extra=b3 >> 5)

def envelope(pcm, rate):
    n = len(pcm)
    if n == 0: return "leer"
    dur = n / rate
    win = max(1, int(rate * 0.025))
    rms = []
    for i in range(0, n, win):
        seg = pcm[i:i+win]
        rms.append(math.sqrt(sum(s*s for s in seg) / len(seg)))
    peak = max(rms); pi = rms.index(peak)
    att = pi * 0.025
    # Zeit nach Peak bis unter 10% Peak (=-20dB)
    dec = None
    for j in range(pi, len(rms)):
        if rms[j] < peak * 0.1:
            dec = (j - pi) * 0.025; break
    # Anteil der Fenster > 30% Peak (Sustain-Mass)
    loud = sum(1 for r in rms if r > peak * 0.3) / len(rms)
    kind = "PERKUSSIV" if (dur < 0.55 or (dec is not None and dec < 0.25 and loud < 0.35)) else \
           ("ANHALTEND" if loud > 0.5 else "MITTEL")
    return "dauer=%.2fs attack=%.2fs decay20dB=%s loud30=%d%% -> %s" % (
        dur, att, ("%.2fs" % dec) if dec is not None else ">rest", int(loud*100), kind)

for bank in (17, 67, 68):
    B = parse_bank(bank)
    print("\n===== BANK %d: EDT @0x%06X (%d B) VBD @0x%06X (%d B) progs=%d tones=%d vags=%d map=%d"
          % (bank, B['edt_off'], B['edt_size'], B['vbd_off'], B['vbd_size'],
             B['progs'], B['tones'], B['vags'], len(B['map'])))
    for i, e in enumerate(B['map']):
        d = decode_map(e)
        if d is None:
            continue
        ti = d['prog']*16 + d['tone']
        if ti >= len(B['tone_list']):
            print("  SE %2d: 0x%08X chan=%d prio=%d prog=%d tone=%d extra=%d | TONE AUSSERHALB (progs=%d)"
                  % (i, e, d['chan'], d['prio'], d['prog'], d['tone'], d['extra'], B['progs']))
            continue
        t = B['tone_list'][ti]
        vag = t['vag'] - 1
        vsz = B['sizes'][vag] if 0 <= vag < len(B['sizes']) else -1
        print("  SE %2d: 0x%08X chan=%d prio=%d prog=%d tone=%d extra=%d | tone: vol=%d pan=%d center=%d shift=%d min=%d max=%d vag=%d (%d B)"
              % (i, e, d['chan'], d['prio'], d['prog'], d['tone'], d['extra'],
                 t['vol'], t['pan'], t['center'], t['shift'], t['min'], t['max'], t['vag'], vsz))
    print("  EDT md5=%s  VBD md5=%s" % (
        hashlib.md5(VBS[B['edt_off']:B['edt_off']+B['edt_size']]).hexdigest()[:12],
        hashlib.md5(VBS[B['vbd_off']:B['vbd_off']+B['vbd_size']]).hexdigest()[:12]))
    print("  Map roh:", " ".join("%08X" % x for x in B['map']))
    # VAG-Hashes (Vergleich der Baenke)
    for i in range(B['vags']):
        raw = VBS[B['vbd_off']+B['offs'][i] : B['vbd_off']+B['offs'][i]+B['sizes'][i]]
        print("  vag%-2d %6d B  md5=%s" % (i+1, B['sizes'][i], hashlib.md5(raw).hexdigest()[:12]))
    # WAVs der belegten SEs rendern
    for i, e in enumerate(B['map']):
        d = decode_map(e)
        if d is None: continue
        ti = d['prog']*16 + d['tone']
        if ti >= len(B['tone_list']): continue
        t = B['tone_list'][ti]
        vag = t['vag'] - 1
        if not (0 <= vag < len(B['sizes'])): continue
        raw = VBS[B['vbd_off']+B['offs'][vag] : B['vbd_off']+B['offs'][vag]+B['sizes'][vag]]
        pcm = adpcm_decode(raw)
        rate = int(round(44100 * 2 ** ((t['min'] - t['center']) / 12.0)))
        name = "bank%02d_se%02d_vag%d.wav" % (bank, i, t['vag'])
        with wave.open(os.path.join(OUT, name), "wb") as w:
            w.setnchannels(1); w.setsampwidth(2); w.setframerate(rate)
            w.writeframes(b"".join(struct.pack("<h", s) for s in pcm))
        print("  WAV %s rate=%d  %s" % (name, rate, envelope(pcm, rate)))
