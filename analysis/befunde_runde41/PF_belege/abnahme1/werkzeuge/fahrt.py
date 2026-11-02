"""Abnahme PF 1 - Messlauf im echten Fenster (beschleunigter Renderer, KEIN AUTOSHOT/SOFTWARE_RENDER/FRAMEDUMP).
gdigrab per hwnd der EIGENEN PID (nie per Titel: parallele Agenten haben Fenster gleichen Titels), Daueraufnahme mit F Bildern/s,
dazu die Ankunftszeit jeder neuen debug.log/stdout-Zeile (Zeitachse fuer die Zuordnung Bild <-> Ereignis).

fahrt.py <name> <sekunden> <fps> KEY=VAL ...      (Sonder: KARTE=<raum;x;y;z;rot;flags...> legt re15_card.mcr im Laufordner an)
"""
import os, sys, subprocess, time, shutil, random, re, json, ctypes
from ctypes import wintypes

FF = r'C:/ProgramData/chocolatey/bin/ffmpeg.exe'
HIER = os.path.dirname(os.path.abspath(__file__))
WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_panelfix/re15_port/build'
EXE = WT + r'/platform/pc/re15_pc.exe'
WERKZEUG = WT + r'/tests/unit/test_r41_nachtrag30.exe'

user32 = ctypes.WinDLL('user32', use_last_error=True)
EnumWindowsProc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)


def fenster_von(pid):
    gef = []

    def cb(h, l):
        p = wintypes.DWORD()
        user32.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and user32.IsWindowVisible(h):
            n = user32.GetWindowTextLengthW(h)
            b = ctypes.create_unicode_buffer(n + 1)
            user32.GetWindowTextW(h, b, n + 1)
            if b.value.startswith('RE1.5'):
                gef.append(h)
        return True
    user32.EnumWindows(EnumWindowsProc(cb), 0)
    return gef[0] if gef else 0


def main():
    name, dauer, fps = sys.argv[1], float(sys.argv[2]), sys.argv[3]
    env = dict(os.environ)
    env['PATH'] = r'C:\msys64\mingw64\bin;' + env['PATH']
    env.update({'SDL_ASSERT': 'always_ignore', 'RE15_NO_INTRO': '1', 'RE15_NOAUDIO': '1', 'SDL_AUDIODRIVER': 'dummy', 'RE15_MSG_LOG': '1'})
    karte = None
    for kv in sys.argv[4:]:
        k, v = kv.split('=', 1)
        if k == 'KARTE':
            karte = v
        else:
            env[k] = v
    wd = os.path.join(HIER, 'runs', name)
    shutil.rmtree(wd, ignore_errors=True)
    os.makedirs(os.path.join(wd, 'bilder'))
    if karte:
        r = subprocess.run([WERKZEUG, 'karte', 're15_card.mcr'] + karte.split(';'), cwd=wd, capture_output=True, text=True)
        print('karte:', r.stdout.strip(), r.returncode)
        if r.returncode:
            sys.exit(1)
    kopie = os.path.join(os.path.dirname(EXE), 're15_pc_pfabn1_%s_%06d.exe' % (re.sub(r'\W', '', name), random.randrange(10**6)))
    shutil.copy(EXE, kopie)
    log = open(os.path.join(wd, 'stdout.log'), 'w')
    p = subprocess.Popen([kopie], cwd=wd, env=env, stdout=log, stderr=subprocess.STDOUT)
    t0 = time.time()
    zeitachse = []
    gelesen = {}
    ff = None
    hw = 0
    try:
        while time.time() - t0 < 20 and not hw and p.poll() is None:
            hw = fenster_von(p.pid)
            time.sleep(0.05)
        print('pid', p.pid, 'hwnd', hw, '%.2fs' % (time.time() - t0))
        if not hw:
            raise SystemExit('kein Fenster')
        tff = time.time()
        ff = subprocess.Popen([FF, '-hide_banner', '-loglevel', 'error', '-f', 'gdigrab', '-draw_mouse', '0', '-framerate', fps,
                               '-i', 'hwnd=%d' % hw, '-t', str(dauer), os.path.join(wd, 'bilder', 'b_%05d.png')],
                              stdin=subprocess.DEVNULL)
        while time.time() - tff < dauer + 1 and p.poll() is None:
            for f in ('debug.log', 'stdout.log'):
                pf = os.path.join(wd, f)
                if not os.path.exists(pf):
                    continue
                with open(pf, errors='replace') as fh:
                    fh.seek(gelesen.get(f, 0))
                    neu = fh.read()
                    if neu and neu.endswith('\n'):
                        gelesen[f] = fh.tell()
                        t = time.time() - tff
                        for z in neu.splitlines():
                            zeitachse.append((round(t, 2), f[0], z))
            time.sleep(0.05)
        if ff:
            try:
                ff.wait(timeout=dauer + 15)
            except subprocess.TimeoutExpired:
                ff.kill()
    finally:
        if p.poll() is None:
            p.kill()
        p.wait()
        log.close()
        time.sleep(0.5)
        try:
            os.remove(kopie)
        except OSError:
            pass
    json.dump({'fps': float(fps), 'zeitachse': zeitachse}, open(os.path.join(wd, 'zeitachse.json'), 'w'))
    n = len(os.listdir(os.path.join(wd, 'bilder')))
    print('Bilder', n, 'Zeilen', len(zeitachse))


main()
