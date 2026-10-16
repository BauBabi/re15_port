"""raum_scan.py - Runde 38 Spur A: Nebenwirkungs-Scan des Rueckseiten-Verwurfs ueber alle Leon-Raeume.
Je Raum zwei Laeufe (exe ALT = Stand 1bcbd0ff ohne Verwurf, exe NEU), Sprung per Debug-Menue (RE15_DEBUG_JUMP=<raum>@60),
Framedump Bild 200 (Skala 1), dann Pixel-Differenz (max. Kanal-Abstand > 20). Ausgabe: je Raum Anzahl geaenderter Pixel
und Huellrechteck. Aufruf: python raum_scan.py <ausgabeordner> [raum ...]"""
import os, subprocess, sys, glob
from concurrent.futures import ThreadPoolExecutor
from PIL import Image, ImageChops

WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_cursor'
PCDIR = WT + '/re15_port/build/platform/pc'
EXE = {'alt': PCDIR + '/re15_pc_r38a_alt.exe', 'neu': PCDIR + '/re15_pc_r38a.exe'}


def raeume():
    r = []
    for st in range(1, 7):
        for p in sorted(glob.glob(WT + '/re15_port/shared_assets/PSX/STAGE%d/ROOM*0.RDT' % st)):
            r.append(os.path.basename(p)[4:8])
    return r


def lauf(aus, raum, art):
    wd = os.path.join(aus, raum, art)
    os.makedirs(wd, exist_ok=True)
    ziel = os.path.join(wd, 'f.ppm')
    if os.path.exists(ziel):
        return ziel
    env = dict(os.environ)
    env.update({'RE15_NOAUDIO': '1', 'RE15_NO_INTRO': '1', 'RE15_TITLE_SHOT': 'title.bmp', 'RE15_WINDOW_SCALE': '1',
                'RE15_DEBUG_JUMP': raum + '@60', 'RE15_FRAMEDUMP': '200:f.ppm', 'RE15_EXIT_AT': '203'})
    try:
        subprocess.run([EXE[art]], cwd=wd, env=env, timeout=90, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except subprocess.TimeoutExpired:
        pass
    return ziel if os.path.exists(ziel) else None


def vergleich(aus, raum):
    a = lauf(aus, raum, 'alt')
    n = lauf(aus, raum, 'neu')
    if not a or not n:
        return raum, None, None
    d = ImageChops.difference(Image.open(a).convert('RGB'), Image.open(n).convert('RGB')).convert('L')
    px = d.load()
    pts = [(x, y) for y in range(d.size[1]) for x in range(d.size[0]) if px[x, y] > 20]
    if not pts:
        return raum, 0, None
    xs = [p[0] for p in pts]; ys = [p[1] for p in pts]
    return raum, len(pts), (min(xs), min(ys), max(xs), max(ys))


def main():
    aus = os.path.abspath(sys.argv[1])
    liste = sys.argv[2:] or raeume()
    with ThreadPoolExecutor(max_workers=4) as ex:
        erg = list(ex.map(lambda r: vergleich(aus, r), liste))
    with open(os.path.join(aus, 'scan.txt'), 'w') as f:
        for raum, n, bb in erg:
            z = 'ROOM%s %s %s' % (raum, 'kein Bild' if n is None else '%d Pixel geaendert' % n, bb or '')
            print(z)
            f.write(z + '\n')


if __name__ == '__main__':
    main()
