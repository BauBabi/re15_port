"""Abnahme 2 Spur PF - eigener Messlauf im echten Fenster.
Beschleunigter Renderer, KEIN RE15_AUTOSHOT, KEIN RE15_SOFTWARE_RENDER, KEIN RE15_FRAMEDUMP.
Fenster per EnumWindows der EIGENEN PID (parallele Agenten haben Fenster gleichen Titels), gdigrab per hwnd,
verlustfrei als Video (libx264rgb -qp 0: spart Platte gegenueber Einzelbildern), dazu Zeitachse der debug.log-Zeilen.

lauf.py <name> <exe> <sekunden> <fps> KEY=VAL ...   (KARTE=<raum;x;y;z;rot;flags> -> re15_card.mcr ueber test_r41_nachtrag30 karte)
"""
import os, sys, subprocess, time, shutil, random, re, json, ctypes
from ctypes import wintypes

FF = r'C:/ProgramData/chocolatey/bin/ffmpeg.exe'
HIER = os.path.dirname(os.path.abspath(__file__))
WERKZEUG = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_panelfix/re15_port/build_abn2/tests/unit/test_r41_nachtrag30.exe'

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


def client_groesse(h):
    r = wintypes.RECT()
    user32.GetClientRect(h, ctypes.byref(r))
    return r.right - r.left, r.bottom - r.top


def main():
    name, exe, dauer, fps = sys.argv[1], sys.argv[2], float(sys.argv[3]), sys.argv[4]
    env = dict(os.environ)
    env['PATH'] = r'C:\msys64\mingw64\bin;' + env['PATH']
    env.update({'SDL_ASSERT': 'always_ignore', 'RE15_NO_INTRO': '1', 'RE15_NOAUDIO': '1', 'SDL_AUDIODRIVER': 'dummy', 'RE15_MSG_LOG': '1'})
    for k in ('RE15_AUTOSHOT', 'RE15_SOFTWARE_RENDER', 'RE15_FRAMEDUMP'):
        env.pop(k, None)
    karte = None
    for kv in sys.argv[5:]:
        k, v = kv.split('=', 1)
        if k == 'KARTE':
            karte = v
        else:
            env[k] = v
    wd = os.path.join(HIER, 'runs', name)
    shutil.rmtree(wd, ignore_errors=True)
    os.makedirs(wd)
    if karte:
        r = subprocess.run([WERKZEUG, 'karte', 're15_card.mcr'] + karte.split(';'), cwd=wd, capture_output=True, text=True)
        print('karte:', r.stdout.strip(), r.returncode)
        if r.returncode:
            sys.exit(1)
    kopie = os.path.join(os.path.dirname(exe), 're15_pc_pfabn2_%s_%06d.exe' % (re.sub(r'\W', '', name), random.randrange(10**6)))
    shutil.copy(exe, kopie)
    log = open(os.path.join(wd, 'stdout.log'), 'w')
    p = subprocess.Popen([kopie], cwd=wd, env=env, stdout=log, stderr=subprocess.STDOUT)
    t0 = time.time()
    zeitachse = []
    gelesen = {}
    ff = None
    hw = 0
    groesse = None
    try:
        while time.time() - t0 < 20 and not hw and p.poll() is None:
            hw = fenster_von(p.pid)
            time.sleep(0.05)
        if not hw:
            raise SystemExit('kein Fenster')
        time.sleep(0.3)
        groesse = client_groesse(hw)
        print('pid', p.pid, 'hwnd', hw, 'client', groesse, '%.2fs' % (time.time() - t0))
        tff = time.time()
        ff = subprocess.Popen([FF, '-hide_banner', '-loglevel', 'error', '-f', 'gdigrab', '-draw_mouse', '0', '-framerate', fps,
                               '-i', 'hwnd=%d' % hw, '-t', str(dauer), '-c:v', 'libx264rgb', '-qp', '0', '-preset', 'ultrafast',
                               os.path.join(wd, 'fenster.mkv')], stdin=subprocess.DEVNULL)
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
                ff.wait(timeout=dauer + 30)
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
    json.dump({'fps': float(fps), 'client': groesse, 'exe': exe, 'zeitachse': zeitachse}, open(os.path.join(wd, 'zeitachse.json'), 'w'))
    print('Video', os.path.getsize(os.path.join(wd, 'fenster.mkv')) if os.path.exists(os.path.join(wd, 'fenster.mkv')) else 0,
          'Bytes, Zeilen', len(zeitachse))


main()
