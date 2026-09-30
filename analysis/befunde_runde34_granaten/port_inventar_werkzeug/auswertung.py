#!/usr/bin/env python3
"""Runde 34 (Granaten) - Auswertung eines lauf_baseline.sh-Laufs (build/r34g_baseline/<marke>/).

Liest wf.log (RE15_WAFFEN_LOG, Schreiber engine/src/game_step_common.c:1678-1691 + SPAWN-Zeilen
re15_esp.c:772-777 + SE-Zeilen audio_pc.c:1054-1058), state.log (RE15_STATE_LOG, platform/pc/main.c:
7404-7450) und optional fx.log (RE15_FX_LOG, main.c:302-314). Nur der Teil NACH dem Raumeintritt
in ROOM1140 wird ausgewertet (Spieler steht dort zuerst auf PL(-7600,-17600); frame_count startet
beim Eintritt bei 0, main.c:7798).

Je Wurf (Flanke rec 0->1 im wf.log):
  Abzugsbild F0, Clip, fc, Zielhoehe (aus Clip 7/9/11), Bild der SPAWN-Zeile (anim_frame), fx vor/nach,
  Magazin (state.log mg) vor/nach, Spieler-Pos/rot, je Gegner Abstand d beim Abzug und alle Zustands-
  wechsel (st/ss1) im Fenster [F0-2, F0+90], Spieler-hp-Aenderungen.
Aufruf: auswertung.py <marke> [fenster]
"""
import os
import re
import sys

BASE = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'build', 'r34g_baseline')

RX_WF = re.compile(r'^F(\d+) pad=([0-9a-f]+) w=(\d+) clip=(\d+) fc=(\d+) frame=(\d+) ph=(\d+) rec=(\d+) '
                   r'auto=(\d+)/(\d+) takt=(\d+) mag=(\d+) fx=(\d+)')
RX_PL = re.compile(r'^F(\d+) pad=([0-9a-f]+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) ps1=(\d+) '
                   r'ps2=(\d+) mo=(\d+) ac=(\d+) fx=(\d+) mg=(-?\d+)')
RX_EN = re.compile(r'\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) '
                   r'af=(\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\]')


def post_entry(lines, rx_pos):
    """Schneidet die Zeilen ab dem Raumeintritt (letzter Rueckfall der Bildnummer auf 0/1)."""
    start = 0
    prev = -1
    for i, ln in enumerate(lines):
        m = re.match(r'^F(\d+) ', ln)
        if not m:
            continue
        f = int(m.group(1))
        if f < prev:
            start = i
        prev = f
    return lines[start:]


def main():
    marke = sys.argv[1]
    win = int(sys.argv[2]) if len(sys.argv) > 2 else 90
    d = os.path.join(BASE, marke)
    wf = post_entry(open(os.path.join(d, 'wf.log'), encoding='latin-1').read().splitlines(), RX_WF)
    st = post_entry(open(os.path.join(d, 'state.log'), encoding='latin-1').read().splitlines(), RX_PL)
    fxp = os.path.join(d, 'fx.log')
    # state je Bild
    S = {}
    for ln in st:
        m = RX_PL.match(ln)
        if not m:
            continue
        f = int(m.group(1))
        en = {int(e[0]): dict(t=e[1], st=int(e[2]), ss1=int(e[3]), ss2=int(e[4]), mo=int(e[7]), af=int(e[8]),
                               stun=int(e[9]), d=int(e[10]), x=int(e[11]), z=int(e[12]))
              for e in RX_EN.findall(ln)}
        S[f] = dict(x=int(m.group(3)), z=int(m.group(4)), rot=int(m.group(5)), hp=int(m.group(6)),
                    pst=int(m.group(7)), mo=int(m.group(10)), ac=int(m.group(11)), fx=int(m.group(12)),
                    mg=int(m.group(13)), en=en)
    # wf je Bild + Anhaenge (SPAWN/SE-Zeilen gehoeren zum Bild der Zeile davor)
    W = {}
    extra = {}
    last = None
    for ln in wf:
        m = RX_WF.match(ln)
        if m:
            f = int(m.group(1))
            W[f] = dict(pad=int(m.group(2), 16), w=int(m.group(3)), clip=int(m.group(4)), fc=int(m.group(5)),
                        frame=int(m.group(6)), ph=int(m.group(7)), rec=int(m.group(8)), mag=int(m.group(12)),
                        fx=int(m.group(13)))
            last = f
        elif last is not None and ln.strip():
            extra.setdefault(last, []).append(ln.strip())
    frames = sorted(W)
    throws = []
    for i, f in enumerate(frames):
        if i and W[f]['rec'] == 1 and W[frames[i - 1]]['rec'] == 0:
            throws.append(f)
    print(f'Lauf {marke}: {len(frames)} Bilder nach Eintritt (F{frames[0]}..F{frames[-1]}), '
          f'{len(throws)} Abzuege, Extra-Zeilen: {sum(len(v) for v in extra.values())}')
    elev = {7: 'MITTE', 9: 'HOCH', 11: 'TIEF'}
    for n, f0 in enumerate(throws, 1):
        # das Log-Bild F0 zeigt rec=1 -> der Abzug fiel im game_step von F0-1 (Log steht VOR dem Feuer-Pfad)
        fa = f0 - 1
        clip = W[f0]['clip']
        fc = W[f0]['fc']
        # Ende des Wurfs
        fe = f0
        while fe + 1 in W and W[fe + 1]['rec'] == 1:
            fe += 1
        spawns = [(g, W[g]['frame'], x) for g in range(fa, fe + 2) for x in extra.get(g, []) if 'SPAWN' in x]
        ses = [(g, x) for g in range(fa - 1, fe + win) for x in extra.get(g, []) if x.startswith('SE')]
        s0 = S.get(fa, {})
        s1 = S.get(fe + 1, {})
        print(f'\n== Wurf {n}: Abzug im Bild F{fa} (Log F{f0} frame={W[f0]["frame"]}), Clip {clip} '
              f'({elev.get(clip, "?")}), fc={fc}, rec bis F{fe} (letztes frame={W[fe]["frame"]}), '
              f'Pad beim Abzug 0x{W[fa]["pad"]:04x}' if fa in W else '')
        if s0:
            print(f'   Spieler F{fa}: ({s0["x"]},{s0["z"]}) rot={s0["rot"]} hp={s0["hp"]} Magazin mg={s0["mg"]} fx={s0["fx"]}')
        for g, fr, x in spawns:
            print(f'   SPAWN in Bild F{g} bei anim_frame={fr}: {x}')
        if not spawns:
            print('   KEIN SPAWN')
        for g, x in ses:
            print(f'   SE in Bild F{g}: {x}')
        mg_after = [S[g]['mg'] for g in range(fa, fa + 5) if g in S]
        print(f'   Magazin F{fa}..F{fa+4}: {mg_after}; fx F{fa}..F{fe+1}: '
              f'{[S[g]["fx"] for g in range(fa, fe + 2) if g in S]}')
        if s0:
            for sl, e in sorted(s0['en'].items()):
                print(f'   Gegner {sl} t=0x{e["t"]} beim Abzug: d={e["d"]} st={e["st"]} ss1={e["ss1"]} mo={e["mo"]}')
        # Zustandswechsel im Fenster
        prev = {}
        for g in range(fa - 2, fa + win):
            if g not in S:
                continue
            for sl, e in S[g]['en'].items():
                key = (e['st'], e['ss1'])
                if sl in prev and prev[sl] != key:
                    print(f'   F{g} (Abzug+{g-fa}): Gegner {sl} st/ss1 {prev[sl]} -> {key} mo={e["mo"]} d={e["d"]}')
                prev[sl] = key
        hp = [(g, S[g]['hp']) for g in range(fa - 2, fa + win) if g in S]
        ch = [(g, h) for i, (g, h) in enumerate(hp) if i and h != hp[i - 1][1]]
        if ch:
            print(f'   Spieler-hp-Wechsel: {ch}')
    # fx-Verlauf gesamt
    fxs = [(f, S[f]['fx']) for f in sorted(S)]
    ch = [(f, v) for i, (f, v) in enumerate(fxs) if i and v != fxs[i - 1][1]]
    print(f'\nfx-Wechsel (Bild, Anzahl) ueber den ganzen Lauf: {ch}')
    if os.path.exists(fxp):
        fl = open(fxp, encoding='latin-1').read().splitlines()
        g = [l for l in fl if l.startswith('id=4 sub=13')]
        print(f'fx.log: {len(fl)} Zeilen, davon Granate (id=4 sub=13): {len(g)}')
        for l in (g[:3] + g[-3:]):
            print('   ' + l)


if __name__ == '__main__':
    main()
