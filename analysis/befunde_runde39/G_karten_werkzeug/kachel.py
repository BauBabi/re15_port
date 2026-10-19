"""Kachel eines Kartenrechtecks als Zeichenbild: kachel.py <seite> <u> <v> <w> <h> <x> <y>
MAP0<seite+1>.PIX (256x256, 4 bpp, unteres Nibble = linkes Pixel). '.' = 0 (durchsichtig), sonst Index (hex)."""
import sys, os
import numpy as np
PSX = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 're15_port', 'shared_assets', 'PSX', 'DATA')
def pix(seite):
    d = open(os.path.join(PSX, f'MAP0{seite+1:X}.PIX'), 'rb').read()
    b = np.frombuffer(d[:128*256], np.uint8).reshape(256, 128)
    t = np.zeros((256, 256), np.uint8); t[:, 0::2] = b & 15; t[:, 1::2] = b >> 4
    return t
if __name__ == '__main__':
    s, u, v, w, h, x, y = map(int, sys.argv[1:8])
    t = pix(s)
    print('      ' + ''.join(str((x + i) // 10 % 10) if (x + i) % 10 == 0 else ' ' for i in range(w)))
    print('      ' + ''.join(str((x + i) % 10) for i in range(w)))
    for j in range(h):
        print('%4d  ' % (y + j) + ''.join('.' if t[v + j, u + i] == 0 else '%x' % t[v + j, u + i] for i in range(w)))
