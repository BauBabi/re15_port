import sys, os
from PIL import Image
src, dst = sys.argv[1], sys.argv[2]
for spec in sys.argv[3:]:
    f, name = spec.split('=')
    Image.open(os.path.join(src, 'fd_%06d.ppm' % int(f))).save(os.path.join(dst, name))
print('ok')
