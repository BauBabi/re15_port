"""Pruefung V4 r4 (Sicht): Spielbilder (RE15_FRAMEDUMP, registriert) der Laeufe sfv4/r/V4r4 und V4r3 ueber c6.port."""
import sys, os, numpy as np, cv2
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel'
sys.path.insert(0, W + '/nb2neu'); sys.path.insert(0, W + '/v4')
import c6
OUT = sys.argv[1]; os.makedirs(OUT, exist_ok=True); os.chdir(OUT)
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
runs = {'V4r4': S + '/sfv4/r/V4r4', 'V4r3': S + '/sfv4/r/V4r3'}
for k, r in runs.items():
    try: print(k, 'off', c6.off(r))
    except Exception as e: print(k, 'off FEHLER', e); continue
    for h in (85, 90, 100, 110, 118, 125, 135):
        try: p = c6.port(r, h)
        except Exception as e: print(k, h, 'FEHLER', e); continue
        if p is None: print(k, h, 'None'); continue
        cv2.imwrite('spiel_%s_h%d.png' % (k, h), p)
    for h in (85, 100, 118, 135):
        v = c6.vid(h)
        if v is not None: cv2.imwrite('video_h%d.png' % h, v)
print('ok')
