# Messer r34g-Integration: echte Tastatur-Eingabe an GENAU EIN Fenster (PostMessage WM_KEYDOWN/WM_KEYUP an das Fenster
# der eigenen exe-Kopie; kein SendInput, kein Fokuswechsel -> keine fremden Fenster/Sitzungen betroffen).
# Nur fuer Tasten, die RE15_INPUT_SCRIPT / RE15_PRESS nicht kennen (L2 = '1', R2 = '3'); ohne RE15_INPUT_SCRIPT, weil das
# Skript die Tastatur-Bits ersetzt (input_pc.c "SCRIPTED-INPUT OVERRIDE").
#   python tasten_inject.py <exe> <laufordner> <tasten> [env...]
#   <tasten> = "370:3,385:3,400:1" (Spielbild nach dem Debug-Sprung : Taste)
import ctypes, ctypes.wintypes as wt, os, subprocess, sys, time

exe, zdir, tasten = sys.argv[1], sys.argv[2], sys.argv[3]
extra = sys.argv[4:]
os.makedirs(zdir, exist_ok=True)
env = dict(os.environ)
for kv in extra:
    k, v = kv.split('=', 1)
    env[k] = v
plan = []
for t in tasten.split(','):
    f, k = t.split(':')
    plan.append((int(f), k))
plan.sort()

VK = {'1': (0x31, 0x02), '3': (0x33, 0x04), 'e': (0x45, 0x12), 'q': (0x51, 0x10), 'l': (0x4C, 0x26), 'i': (0x49, 0x17)}
u32 = ctypes.WinDLL('user32', use_last_error=True)
WNDENUMPROC = ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)
u32.EnumWindows.argtypes = [WNDENUMPROC, wt.LPARAM]
u32.GetWindowThreadProcessId.argtypes = [wt.HWND, ctypes.POINTER(wt.DWORD)]
u32.IsWindowVisible.argtypes = [wt.HWND]
u32.PostMessageW.argtypes = [wt.HWND, wt.UINT, wt.WPARAM, wt.LPARAM]

def fenster(pid):
    res = []
    def cb(h, l):
        p = wt.DWORD()
        u32.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and u32.IsWindowVisible(h):
            res.append(h)
        return True
    u32.EnumWindows(WNDENUMPROC(cb), 0)
    return res[0] if res else None

def taste(h, k, halt=0.15):
    vk, sc = VK[k]
    down = 1 | (sc << 16)
    up = 1 | (sc << 16) | (1 << 30) | (1 << 31)
    u32.PostMessageW(h, 0x0100, vk, down)
    time.sleep(halt)
    u32.PostMessageW(h, 0x0101, vk, up)

log = open(os.path.join(zdir, 'inject.log'), 'w')
p = subprocess.Popen([exe], cwd=zdir, env=env, stdout=open(os.path.join(zdir, 'stdout.txt'), 'w'),
                     stderr=open(os.path.join(zdir, 'stderr.txt'), 'w'))
t0 = time.time()
h = None
state = os.path.join(zdir, 'state.log')
prev = -1; sprung = False; i = 0
while p.poll() is None and time.time() - t0 < 300:
    if h is None:
        h = fenster(p.pid)
        if h: log.write('Fenster %s pid %d nach %.1f s\n' % (hex(h), p.pid, time.time() - t0)); log.flush()
    f = None
    try:
        with open(state, 'rb') as fh:
            fh.seek(0, 2); n = fh.tell(); fh.seek(max(0, n - 4000))
            zeilen = fh.read().decode('latin1').splitlines()
        for z in reversed(zeilen):
            if z.startswith('F') and ' pad=' in z:
                f = int(z[1:z.index(' ')]); break
    except OSError:
        pass
    if f is not None:
        if f < prev: sprung = True
        prev = f
        while sprung and h and i < len(plan) and f >= plan[i][0]:
            taste(h, plan[i][1])
            log.write('F%d (Soll %d) Taste %s gesendet\n' % (f, plan[i][0], plan[i][1])); log.flush()
            i += 1
    time.sleep(0.01)
rc = p.wait(timeout=60)
log.write('rc=%d dauer=%.1f gesendet=%d/%d\n' % (rc, time.time() - t0, i, len(plan)))
log.close()
print('rc=%d gesendet=%d/%d' % (rc, i, len(plan)))
