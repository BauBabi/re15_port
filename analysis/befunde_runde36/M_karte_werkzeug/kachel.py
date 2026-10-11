# Kachel eines RE1.5-Kartenrechtecks als Index-Text (DATA/MAP0<p+1>.PIX, 256x256 4bpp, 128 B/Zeile)
import sys, struct
pix = open(sys.argv[1], 'rb').read(); u, v, w, h = map(int, sys.argv[2:6])
for y in range(h):
    row = ''
    for x in range(w):
        b = pix[(v + y) * 128 + (u + x) // 2]
        k = (b >> 4) if ((u + x) & 1) else (b & 15)
        row += '.' if k == 0 else ('%X' % k)
    print('%3d %s' % (v + y, row))
