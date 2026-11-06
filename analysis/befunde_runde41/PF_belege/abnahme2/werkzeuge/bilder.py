"""Bilder aus dem verlustfreien Fenster-Video (ffmpeg rawvideo-Pipe, keine Einzeldateien) + Zeitachse."""
import subprocess, json, os
import numpy as np

FF = r'C:/ProgramData/chocolatey/bin/ffmpeg.exe'
HIER = os.path.dirname(os.path.abspath(__file__))


def lauf(name):
    wd = os.path.join(HIER, 'runs', name)
    z = json.load(open(os.path.join(wd, 'zeitachse.json')))
    return wd, z


def bilder(name, schritt=1, ab=0):
    """liefert (i, t, bild HxWx3 uint8) fuer jedes schritt-te Bild."""
    wd, z = lauf(name)
    W, H = z['client']
    fps = z['fps']
    p = subprocess.Popen([FF, '-hide_banner', '-loglevel', 'error', '-i', os.path.join(wd, 'fenster.mkv'), '-f', 'rawvideo', '-pix_fmt', 'rgb24', '-'],
                         stdout=subprocess.PIPE)
    n = W * H * 3
    i = 0
    while True:
        b = p.stdout.read(n)
        if len(b) < n:
            break
        if i >= ab and (i - ab) % schritt == 0:
            yield i, i / fps, np.frombuffer(b, np.uint8).reshape(H, W, 3)
        i += 1
    p.stdout.close()
    p.wait()


def logisch(b):
    """960x720 -> 320x240: Mitte jedes 3x3-Blocks; dazu Anteil einfarbiger Bloecke."""
    H, W, _ = b.shape
    f = W // 320
    l = b[f // 2::f, f // 2::f][:240, :320]
    return l


def einfarbig(b, x0=0, y0=0, x1=320, y1=240):
    H, W, _ = b.shape
    f = W // 320
    t = b[y0 * f:y1 * f, x0 * f:x1 * f].reshape(y1 - y0, f, x1 - x0, f, 3).astype(int)
    return float(np.mean((t.max((1, 3)) == t.min((1, 3))).all(-1)))


def ereignisse(name, muster=('[pri] cut=', '[schrank2010]', '[msgtext]', '[panel2030]', 'wegplan', 'GELOEST')):
    wd, z = lauf(name)
    return [(t, s) for t, k, s in z['zeitachse'] if any(m in s for m in muster)]
