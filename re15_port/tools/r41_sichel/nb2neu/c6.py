"""NB2neu/NB6 HSF: gemeinsame Helfer (Nachfolger von nb5/c5.py; Laufordner NB6_RUNS).
Videobilder: analysis/befunde_runde41/SF_belege/roh/alle/f%04d.png (Video 4, ffmpeg -vsync 0, 1-basiert), h = n - 1350.
Port: RE15_FRAMEDUMP eines Laufs (lauf5.sh), Bild des Videobilds h = Dump F(h-1-Messzeile) + h.
Registrierung Port -> Video der Abnahme 3 (SF_belege/abnahme3/werkzeuge/reg.json, ECC cc 0,996).
Laufordner: Umgebungsvariable NB5_RUNS (Standard: Sitzungs-Scratchpad r/)."""
import cv2, numpy as np, os, re, json, math
S = os.path.dirname(os.path.abspath(__file__))
BAUM = os.environ.get('RE15_BAUM', r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel')
VID = os.path.join(BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle')
RUNS = os.environ.get('NB6_RUNS', os.environ.get('NB5_RUNS', ''))
_REG = os.path.join(BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'abnahme3', 'werkzeuge', 'reg.json')
if not os.path.exists(_REG): _REG = os.path.join(S, 'reg.json')   # NB2neu: Registrierung der Abnahme 3 (cc 0,996) als Kopie
W = np.array(json.load(open(_REG))['M'],
             np.float32)
_off = {}
def rundir(run):
    p = run if os.path.isabs(run) else os.path.join(RUNS, run)
    if not os.path.isdir(p): raise FileNotFoundError(p)
    return p
def off(run):
    """Dump-Bild des Videobilds h0 = erstes Dump-Bild mit schwarzem Balken oben UND unten (Cut-12-Letterbox), am BILD
    gemessen wie Abnahme 4 (k4.f0), damit alte Staende (NB2: andere Messzeile) gleich zugeordnet werden."""
    if run not in _off:
        o = None
        d = os.path.join(rundir(run), 'fd')
        for F in range(200, 320):
            q = os.path.join(d, 'f%06d.ppm' % F)
            if not os.path.exists(q): continue
            im = cv2.imread(q)
            g = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
            g = cv2.resize(g, (960, 720), interpolation=cv2.INTER_NEAREST)   # SF V4 Runde 3: Dumps auch in 320 x 240 (Laeufe V4r3*)
            if g[:50].mean() < 1 and g[-50:].mean() < 1 and g[100:600].mean() > 20:
                o = F; break
        _off[run] = o
    return _off[run]
def vid(h): return cv2.imread(os.path.join(VID, 'f%04d.png' % (1350 + h)))
def vidn(n): return cv2.imread(os.path.join(VID, 'f%04d.png' % n))
def roh(run, F):
    return cv2.imread(os.path.join(rundir(run), 'fd', 'f%06d.ppm' % F))
def port(run, h, reg=True, F=None):
    im = roh(run, off(run) + h if F is None else F)
    if im is None: return None
    im = cv2.resize(im, (640, 480), interpolation=cv2.INTER_AREA)
    if reg: im = cv2.warpAffine(im, W, (640, 480), flags=cv2.INTER_LINEAR | cv2.WARP_INVERSE_MAP)
    return im
_bg = None
def bg():
    global _bg
    if _bg is None:
        _bg = port('BG', 0, F=240)
    return _bg
POS = np.array([-13140, -3258, -26604], float); TGT = np.array([-2808, -1440, -11556], float); HF = 231.0
_f = TGT - POS; _f /= np.linalg.norm(_f); _up = np.array([0, -1, 0], float)
_r = np.cross(_up, _f); _r /= np.linalg.norm(_r); _u = np.cross(_f, _r)
def proj(p):
    """Weltpunkt -> Videopunkt (640x480) durch Cut 12 und die Registrierung."""
    d = np.array(p, float) - POS; x = d @ _r; y = d @ _u; z = d @ _f
    px, py = 160 - HF * x / z, 120 - HF * y / z
    X, Y = 2 * px, 2 * py
    A = np.vstack([W, [0, 0, 1]]); Ai = np.linalg.inv(A)
    v = Ai @ np.array([X, Y, 1.0])
    return (float(v[0]), float(v[1]))
def tiefe(p):
    return float((np.array(p, float) - POS) @ _f)
def boden(vx, vy, y=0.0):
    """Videopunkt -> Weltpunkt auf der Ebene Hoehe y (y+ = unten, Boden 0)."""
    A = np.vstack([W, [0, 0, 1]]); q = A @ np.array([vx, vy, 1.0]); px, py = q[0] / 2, q[1] / 2
    d = _f + _r * (-(px - 160) / HF) + _u * (-(py - 120) / HF)
    t = (y - POS[1]) / d[1]
    return POS + d * t
def log(run):
    R = {}
    for L in open(os.path.join(rundir(run), 'debug.log'), errors='ignore'):
        m = re.search(r'\[hangar-takt\] h(-?\d+) F(\d+) B clip (\d+) bild (\d+) gier (\d+) \((-?\d+),(-?\d+)\)', L)
        if m: R.setdefault(int(m.group(1)), {})['B'] = tuple(int(m.group(i)) for i in range(2, 8)); continue
        m = re.search(r'\[hangar-takt\] h(-?\d+) SM clip (\d+) bild (\d+) \((-?\d+),(-?\d+),(-?\d+)\)', L)
        if m: R.setdefault(int(m.group(1)), {})['SM'] = tuple(int(m.group(i)) for i in range(2, 7)); continue
        m = re.search(r'\[hangar-takt\] h(-?\d+) Schwanzspitze \((-?\d+),(-?\d+),(-?\d+)\)', L)
        if m: R.setdefault(int(m.group(1)), {})['spitze'] = tuple(int(m.group(i)) for i in range(2, 5))
    return R
def Yg(im): return cv2.cvtColor(im, cv2.COLOR_BGR2GRAY).astype(np.float32)
def label(im, t, org=(5, 20), s=0.6):
    cv2.putText(im, t, org, 0, s, (0, 0, 0), 3); cv2.putText(im, t, org, 0, s, (0, 255, 255), 1); return im
def zoom(im, x0, y0, x1, y1, f=3, grid=10, lab=50, title=None):
    c = cv2.resize(im[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_CUBIC)
    for x in range((x0 // grid + 1) * grid, x1, grid):
        X = int((x - x0) * f); col = (0, 255, 255) if x % lab == 0 else (90, 90, 90)
        cv2.line(c, (X, 0), (X, c.shape[0] - 1), col, 1)
        if x % lab == 0: cv2.putText(c, str(x), (X + 2, 12), 0, 0.4, (0, 255, 255), 1)
    for y in range((y0 // grid + 1) * grid, y1, grid):
        Y = int((y - y0) * f); col = (0, 255, 255) if y % lab == 0 else (90, 90, 90)
        cv2.line(c, (0, Y), (c.shape[1] - 1, Y), col, 1)
        if y % lab == 0: cv2.putText(c, str(y), (2, Y - 2), 0, 0.4, (0, 255, 255), 1)
    if title: label(c, title, (5, c.shape[0] - 8), 0.6)
    return c
def sheet(rows, out, scale=1.0, q=88):
    im = np.vstack([np.hstack(r) if isinstance(r, list) else r for r in rows])
    if scale != 1.0: im = cv2.resize(im, None, fx=scale, fy=scale, interpolation=cv2.INTER_AREA)
    cv2.imwrite(out, im, [cv2.IMWRITE_JPEG_QUALITY, q])
def rot(im, x0, x1, y0, y1):
    c = im[y0:y1, x0:x1].astype(int); b, g, r = c[..., 0], c[..., 1], c[..., 2]
    return int(((r > g + 35) & (r > 90) & (r > b + 30)).sum())
def rotw(im, x0, x1, y0, y1):
    c = im[y0:y1, x0:x1].astype(int); b, g, r = c[..., 0], c[..., 1], c[..., 2]
    return int(((r > g + 20) & (r > 70) & (r > b + 15)).sum())
def gelb(im):
    b, g, r = [im[..., i].astype(int) for i in range(3)]
    return (r > 70) & (g > 45) & (r - b > 35) & (g - b > 15) & (r < 1.9 * g)
def gruen(im):
    b, g, r = [im[..., i].astype(int) for i in range(3)]
    return (g > r + 14) & (g > b + 6) & (g > 45)
def P(run, h): return cv2.GaussianBlur(port(run, h), (0, 0), 1.35)
def welt_tiefe(vx, vy, t):
    """Videopunkt -> Weltpunkt in Blicktiefe t (Abstand entlang der Blickachse von Cut 12)."""
    A = np.vstack([W, [0, 0, 1]]); q = A @ np.array([vx, vy, 1.0]); px, py = q[0] / 2, q[1] / 2
    d = _f + _r * (-(px - 160) / HF) + _u * (-(py - 120) / HF)
    return POS + d * t
# NB6: Videoschwarz. Video 4 hat einen angehobenen Schwarzwert (Letterbox-Balken 15-18, Abnahme 4 Abschnitt 0: 17,4-17,7).
# Am Hintergrund selbst gemessen (Lauf BG gegen Video h104/h110/h116, figurfreie Streifen x 10-240 und 500-630, Port Gauss 1,35,
# 3 x 130000 Bildpunkte): Video = 0,987 x Port + 10,65; im dunklen Bereich (Port < 60) Video = 0,954 x Port + 12,65. Fuer
# Helligkeits- und Farbklassenvergleiche wird das Video mit der dunklen Abbildung zurueckgerechnet (Vk), der Port dazu weich
# (Gauss 1,35 wie die Masken der Abnahme, Videounschaerfe).
VSCHWARZ, VGAIN = 12.65, 0.954
def vsw(im):
    return np.clip((im.astype(np.float32) - VSCHWARZ) / VGAIN, 0, 255).astype(np.uint8)
