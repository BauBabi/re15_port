"""Vergleichsvideo: links Video 4, rechts Port (Framedump F = h + 243, auf den Videoraum abgebildet: x = 2 X/3 + 1,97, y = 2,079 Y/3 + 1,3),
h-8 .. h135, erst normale Geschwindigkeit (30 Bilder/s), dann halbe Geschwindigkeit.

    python vergleich_video.py <laufordner> <ausgabe.mp4> <titel rechts>
"""
import sys, os, subprocess, shutil
import numpy as np, cv2
AN = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/schwanz_v8/analyse'
FF = r'C:/ProgramData/chocolatey/bin/ffmpeg.exe'
lauf, aus, titel = sys.argv[1], sys.argv[2], sys.argv[3]
tmp = aus + '_bilder'
shutil.rmtree(tmp, ignore_errors=True); os.makedirs(tmp)
M = np.array([[2.0 / 3.0, 0, 1.97], [0, 2.079 / 3.0, 1.3]], np.float32)


def port(h):
    p = os.path.join(lauf, 'fd', 'f%06d.ppm' % (h + 243))
    im = cv2.imread(p)
    if im is None:
        return np.zeros((480, 640, 3), np.uint8)
    return cv2.warpAffine(im, M, (640, 480), flags=cv2.INTER_AREA)


def schrift(im, s, x, y, sc=0.6, col=(255, 255, 255)):
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(im, s, (x, y), cv2.FONT_HERSHEY_SIMPLEX, sc, col, 1, cv2.LINE_AA)


n = 0
for h in range(-8, 136):
    v = cv2.imread(AN + '/v4/n%04d.png' % (1349 + h))
    if v is None:
        v = np.zeros((480, 640, 3), np.uint8)
    p = port(h)
    im = np.hstack([v, p])
    schrift(im, 'Video 4  h%d' % h, 10, 25)
    schrift(im, '%s  h%d (F%d)' % (titel, h, h + 243), 650, 25)
    cv2.imwrite(os.path.join(tmp, 'b%04d.png' % n), im)
    n += 1
lst = os.path.join(tmp, 'liste.txt')
subprocess.run([FF, '-hide_banner', '-loglevel', 'error', '-y', '-framerate', '30', '-i', os.path.join(tmp, 'b%04d.png'),
                '-c:v', 'libx264', '-pix_fmt', 'yuv420p', '-crf', '18', os.path.join(tmp, 'a.mp4')], check=True)
subprocess.run([FF, '-hide_banner', '-loglevel', 'error', '-y', '-framerate', '15', '-i', os.path.join(tmp, 'b%04d.png'),
                '-r', '30', '-c:v', 'libx264', '-pix_fmt', 'yuv420p', '-crf', '18', os.path.join(tmp, 'b.mp4')], check=True)
open(lst, 'w').write("file 'a.mp4'\nfile 'b.mp4'\n")
subprocess.run([FF, '-hide_banner', '-loglevel', 'error', '-y', '-f', 'concat', '-safe', '0', '-i', lst, '-c', 'copy', aus], check=True)
shutil.rmtree(tmp, ignore_errors=True)
print('ok', aus, n, 'Bilder')
