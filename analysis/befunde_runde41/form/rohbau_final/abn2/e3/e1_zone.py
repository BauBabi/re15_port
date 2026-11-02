import os, sys
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
RF = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, os.path.normpath(os.path.join(RF, '..', '..', '..', '..', 're15_port', 'tools')))
import form_pruefung as P
Y = cv2.GaussianBlur(P.original(1).astype(np.float32) @ P.YK, (0, 0), .7)
ra = P.teil_roh('right_arm')
print('     ' + ' '.join('%3d' % x for x in range(396, 436, 2)))
for y in range(240, 304, 2):
    print('%3d  ' % y + ' '.join(('%3d' % Y[y, x]) + ('*' if ra[y, x] else ' ') for x in range(396, 436, 2)))
