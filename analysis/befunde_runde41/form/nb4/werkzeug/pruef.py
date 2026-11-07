# Riegel-Kriterien 6a-6d (wie test_r41_form_wagenweg.cmake) auf einen lauf.sh-Lauf: Taumelbilder aus state.log, Slotdeckung aus deck.log,
# fast schwarz aus fd/f*.ppm (nur vorhandene Bilder). Ausgabe: Zusammenfassung + Verstoesse + je Bild eine Zeile (Datei pruef.txt).
#   python pruef.py <laufdir>
import sys, os, re, glob
import numpy as np
from PIL import Image
d = sys.argv[1]
taumel = []
for line in open(os.path.join(d, 'state.log'), errors='replace'):
    m = re.match(r'^F(\d+) pad=', line)
    if m and re.search(r't=30 st=[23] ss1=\d+ ss2=\d+ ss3=\d+ g=[0-9a-f]+ mo=13 [^]]*\] hp=1( |$)', line):
        taumel.append(int(m.group(1)))
deck = {}
for line in open(os.path.join(d, 'deck.log')):
    m = re.match(r'F(\d+) cam=(-?\d+)(.*)', line)
    if m:
        deck[int(m.group(1))] = (int(m.group(2)), {int(a): (int(b), int(c)) for a, b, c in re.findall(r' s(\d+)=(\d+)/(\d+)', m.group(3))})
summe = {}
for t in taumel:
    if t in deck:
        for s, (v, b) in deck[t][1].items():
            if 1 <= s <= 18: summe[s] = summe.get(s, 0) + b
fs = max(summe, key=summe.get) if summe else -1
dn = {}
for p in glob.glob(os.path.join(d, 'fd', 'f*.ppm')):
    f = int(os.path.basename(p)[1:7])
    if taumel and taumel[0] <= f <= taumel[-1]:
        a = np.asarray(Image.open(p).convert('RGB'), dtype=np.int64)
        l = (299 * a[:, :, 0] + 587 * a[:, :, 1] + 114 * a[:, :, 2]) // 1000
        dn[f] = int((l < 24).sum() * 1000 // l.size)
verst = []
zeilen = []
mn_form, mx_pl, mx_deck = 10**9, 0, 0
kams = []
for t in taumel:
    if t not in deck: continue
    cam, sl = deck[t]
    if cam not in kams: kams.append(cam)
    fv, fb = sl.get(fs, (0, 0)); pv = sl.get(0, (0, 0))[0]
    dk = (fb - fv) * 1000 // fb if fb else 0
    mn_form = min(mn_form, fv); mx_pl = max(mx_pl, pv); mx_deck = max(mx_deck, dk)
    g = []
    if fv < 768: g.append('Form %d<768' % fv)
    if pv > 25600: g.append('Spieler %d>25600' % pv)
    if dk > 500: g.append('verdeckt %d>500' % dk)
    if t in dn and dn[t] > 600: g.append('schwarz %d>600' % dn[t])
    if g: verst.append('F%d Cut %d: %s' % (t, cam, ', '.join(g)))
    zeilen.append('F%d cam=%d form=%d/%d spieler=%d verdeckt=%d dunkel=%s' % (t, cam, fv, fb, pv, dk, dn.get(t, '-')))
open(os.path.join(d, 'pruef.txt'), 'w').write('\n'.join(zeilen) + '\n')
print('%s: Taumeln F%d-F%d (%d Bilder, deck %d), Schnitte %s, Form-Slot %d min sichtbar %d, Spieler max %d, Form max %d Promille verdeckt, max %s Promille schwarz (%d Bilder); %d Verstoesse'
      % (os.path.basename(d.rstrip('/')), taumel[0], taumel[-1], len(taumel), sum(1 for t in taumel if t in deck), kams, fs, mn_form, mx_pl, mx_deck,
         max(dn.values()) if dn else '-', len(dn), len(verst)))
for v in verst[:12]: print('  ' + v)
