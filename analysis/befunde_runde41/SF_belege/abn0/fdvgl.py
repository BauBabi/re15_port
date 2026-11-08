import sys, os, hashlib
a, b = sys.argv[1], sys.argv[2]
lo, hi = int(sys.argv[3]), int(sys.argv[4])
gleich = ungleich = fehlt = 0; erste = []
for f in range(lo, hi + 1):
    n = 'f%06d.ppm' % f
    pa, pb = os.path.join(a, n), os.path.join(b, n)
    if not (os.path.exists(pa) and os.path.exists(pb)):
        fehlt += 1; continue
    if open(pa, 'rb').read() == open(pb, 'rb').read(): gleich += 1
    else:
        ungleich += 1
        if len(erste) < 5: erste.append(f)
print('Bilder %d-%d: gleich %d, ungleich %d, fehlend %d, erste ungleiche %s' % (lo, hi, gleich, ungleich, fehlt, erste))
