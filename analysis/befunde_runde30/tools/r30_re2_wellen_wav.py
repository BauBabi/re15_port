#!/usr/bin/env python3
"""Runde 30 / tuer-verschlossen: die RE2-'verschlossen'-Wellen als WAV abziehen + vermessen.

Quelle: info/re2leon/PL0/RDT/ROOMxxxx.RDT, Raumbank snd0 (RDT-Kopf +0x08/+0x0C/+0x10).
Tonhoehe: exakt der Port-Weg se_play_layers (platform/pc/src/audio_pc.c):
    pitch = re15_vab_note2pitch2(tone.min_note, tone.shift, tone.center, tone.shift)
    Rate  = 44100 * pitch / 4096
mit der 12x16-Tabelle DAT_80077520 (RE1.5 PSX.EXE, hier byte-gelesen, nicht abgetippt).
ADPCM-Dekoder: PSX-SPU-Standard (5 Filterpaare, psx-spx 'SPU ADPCM Samples').

Ausgabe: build/r30_tuer-verschlossen/wav/<name>.wav + eine Messzeile je Welle
(Dauer, Spitzenpegel, Anteil der Energie im staerksten Frequenzband = Tonalitaet).
Das sind MESSWERTE zur Beschreibung, KEIN Hoerbefund - hoeren muss der Nutzer.
"""
import struct, sys, os, wave, math, hashlib
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, HERE)
from r30_re2_roombank import load_bank, resolve

EXE15 = os.path.join(REPO, 'info', 'Re1.5', 'PSX.EXE')
_d = open(EXE15, 'rb').read()
_T = struct.unpack_from('<I', _d, 0x18)[0]
LUT = struct.unpack_from('<192H', _d, 0x800 + 0x80077520 - _T)

def note2pitch2(note, fine, center, shift):
    s = ((fine & 0xffff) + (shift & 0xff)) >> 3
    frac, carry = s, 0
    if s > 15: frac, carry = s - 16, 1
    sem = carry + note + 0x3c - center
    octv = int(sem / 12) - 5
    idx = (sem % 12) * 16 + frac if sem >= 0 else 0
    idx = max(0, min(191, idx))
    p = LUT[idx]
    if octv < 0: p >>= (-octv) & 0x1f
    elif octv >= 1: p = 0x3FFF
    return p & 0xffff

F0 = (0, 60, 115, 98, 122)
F1 = (0, 0, -52, -55, -60)
def vag_decode(buf):
    out = []; s1 = s2 = 0
    for o in range(0, len(buf) - 15, 16):
        hdr = buf[o]; flags = buf[o + 1]
        shift = hdr & 0xf; filt = (hdr >> 4) & 0xf
        if filt > 4: filt = 4
        if shift > 12: shift = 9
        for i in range(28):
            b = buf[o + 2 + (i >> 1)]
            n = (b >> 4) if (i & 1) else (b & 0xf)
            if n >= 8: n -= 16
            s = (n << 12) >> shift
            s += (s1 * F0[filt] + s2 * F1[filt] + 32) >> 6
            s = max(-32768, min(32767, s))
            out.append(s); s2 = s1; s1 = s
        if flags & 1:
            break
    return out

def messen(pcm, rate):
    n = len(pcm)
    if n == 0: return dict(dauer=0)
    peak = max(abs(x) for x in pcm)
    # wirksame Dauer: bis zum letzten Sample ueber 2 % der Spitze
    last = n - 1
    while last > 0 and abs(pcm[last]) < peak * 0.02: last -= 1
    # grobes Spektrum (Goertzel auf 48 Baendern 100..8000 Hz) ueber das ganze Signal
    step = max(1, n // 4096)
    x = pcm[::step][:4096]; r = rate / step
    tot = sum(v * v for v in x) or 1
    best = (0, 0)
    bands = []
    for k in range(48):
        f = 100 * (80 ** (k / 47.0))
        if f > r / 2: break
        w = 2 * math.pi * f / r; c = 2 * math.cos(w); q0 = q1 = q2 = 0.0
        for v in x:
            q0 = c * q1 - q2 + v; q2 = q1; q1 = q0
        p = q1 * q1 + q2 * q2 - c * q1 * q2
        bands.append((f, p))
        if p > best[1]: best = (f, p)
    ps = sum(p for _, p in bands) or 1
    return dict(dauer=n / rate, wirk=last / rate, peak=peak, fmax=best[0], anteil=best[1] / ps)

WELLEN = [
    # (name, raum, satz, wozu)
    ('re2_2110_16_kartenleser_zu',   '2110', 0x16, 'Kartenleser-Tuer verschlossen (sub09 @0x01BBC)'),
    ('re2_2110_0c_karte_angenommen', '2110', 0x0c, 'Karte benutzt, 1. Ton (sub08 @0x01B4C)'),
    ('re2_2110_0d_karte_2',          '2110', 0x0d, 'Karte benutzt, 2. Ton (sub08 @0x01B5C)'),
    ('re2_2110_0b_karte_3',          '2110', 0x0b, 'Karte benutzt, 3. Ton (sub08 @0x01B90)'),
    ('re2_1140_16_schluessel_fehlt', '1140', 0x16, 'EXE-Tuer-Handler: Schluessel fehlt (Pik, aot 0)'),
    ('re2_1140_25_schluessel_benutzt','1140', 0x25, 'EXE-Tuer-Handler: Schluessel benutzt'),
    ('re2_1050_16_schluessel_fehlt', '1050', 0x16, 'EXE-Tuer-Handler: Schluessel fehlt (aot 0, key 0x58)'),
    ('re2_1050_25_schluessel_benutzt','1050', 0x25, 'EXE/Skript: Schluessel benutzt'),
    ('re2_2040_25_schluessel_benutzt','2040', 0x25, 'EXE-Tuer-Handler: Schluessel benutzt'),
    ('re2_2010_25_schluessel_benutzt','2010', 0x25, 'Schluessel benutzt (Variante 42d10c97)'),
    ('re2_20A0_26_entriegelt',       '20A0', 0x26, 'EXE-Tuer-Handler: von dieser Seite entriegelt'),
    ('re2_6090_26_entriegelt',       '6090', 0x26, 'EXE-Tuer-Handler: entriegelt (Labor)'),
    ('re2_2000_16_elektronisch',     '2000', 0x16, 'Skript: "It is electronically locked" (sub09/sub10)'),
    ('re2_40A0_16_elektronisch',     '40A0', 0x16, 'Skript: "electronically locked and will not open" (sub30)'),
    ('re2_60C0_16_kartentuer_labor', '60C0', 0x16, 'EXE-Tuer-Handler: Labor-Kartentuer key_type 0x61'),
    ('re2_60C0_25_karte_benutzt',    '60C0', 0x25, 'EXE-Tuer-Handler: Labor-Karte benutzt'),
    ('re2_6030_16_labor',            '6030', 0x16, 'EXE-Tuer-Handler: Labor key_type 0x5f'),
    ('re2_6030_25_labor',            '6030', 0x25, 'EXE-Tuer-Handler: Labor Schluessel benutzt'),
    ('re2_5090_16_von_innen',        '5090', 0x16, 'Skript: "locked from the inside" (sub19)'),
    ('re2_6170_16_versiegelt',       '6170', 0x16, 'Skript: "completely sealed" (sub23)'),
    ('re2_7020_16_strom',            '7020', 0x16, 'Skript: "won t open until the power is restored" (sub06)'),
    ('re2_2080_27_spezialschloss',   '2080', 0x27, 'Skript: "A special kind of key is required" (sub13)'),
]

def main():
    out = os.path.join(REPO, 'build', 'r30_tuer-verschlossen', 'wav')
    os.makedirs(out, exist_ok=True)
    print('%-34s %-5s %-4s %-13s %6s %6s %5s %4s %5s %6s %6s %6s %5s  %s' % (
        'name', 'raum', 'satz', 'sha1', 'bytes', 'rate', 'vol', 'cen', 'note', 'dauer', 'wirk', 'fmax', 'ant', 'wozu'))
    for name, room, sid, wozu in WELLEN:
        p = os.path.join(REPO, 'info', 're2leon', 'PL0', 'RDT', 'ROOM%s.RDT' % room)
        b = load_bank(p)
        r = resolve(b, sid)
        if r is None or r['empty']:
            print('%-34s %-5s 0x%02x LEER' % (name, room, sid)); continue
        for li, L in enumerate(r['layers']):
            raw = b['d'][L['vag_off']:L['vag_off'] + L['vag_size']]
            pcm = vag_decode(raw)
            pitch = note2pitch2(L['mn'], L['shift'], L['center'], L['shift'])
            rate = 44100 * pitch // 4096
            m = messen(pcm, rate)
            fn = name + ('' if li == 0 else '_lage%d' % li) + '.wav'
            with wave.open(os.path.join(out, fn), 'wb') as w:
                w.setnchannels(1); w.setsampwidth(2); w.setframerate(max(1000, rate))
                w.writeframes(struct.pack('<%dh' % len(pcm), *pcm))
            print('%-34s %-5s 0x%02x %-13s %6d %6d %5d %4d %5d %6.3f %6.3f %6.0f %5.2f  %s' % (
                name, room, sid, L['sha1'][:12], L['vag_size'], rate, L['vol'], L['center'], L['mn'],
                m['dauer'], m['wirk'], m['fmax'], m['anteil'], wozu))

if __name__ == '__main__':
    main()
