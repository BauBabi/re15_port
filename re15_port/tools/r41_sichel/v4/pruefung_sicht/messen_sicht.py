import sys, os, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
def er(m, n=2):
    return cv2.erode(m.astype(np.uint8), np.ones((2*n+1, 2*n+1), np.uint8)).astype(bool)
def stat(V, m):
    if m.sum() == 0: return None
    lab = G.lab(V[m]); px = V[m]
    return dict(n=int(m.sum()), rgb=G.r1(px.mean(0)), lab=G.r1(lab.mean(0)), chroma=round(float(np.hypot(lab[:,1], lab[:,2]).mean()),1),
                rot_anteil=round(float(((px[:,0]-px[:,1]) >= 15).mean()),3), hell30=round(float((lab[:,0] > 30).mean()),3),
                L_p10_50_90=G.r1(np.percentile(lab[:,0], [10,50,90])), a_p10_50_90=G.r1(np.percentile(lab[:,1], [10,50,90])))
def box(shape, b):
    m = np.zeros(shape[:2], bool); x0,y0,x1,y1 = b; m[y0:y1+1, x0:x1+1] = True; return m
out = {}
# ---- vorn: Original n1485 entblendet (Videoraum roh) vs Port im Videoraum (Vergleichsregel des Messblatts)
o1 = L('vorlage1_entbl.png')
BOX1 = {'Kopf': (368,139,416,191), 'Rumpf': (352,196,448,292), 'Oberarm_B_links': (300,186,345,250), 'Oberarm_B_rechts': (455,196,500,262),
        'Unterarm_B_rechts': (468,268,508,330), 'Jeans': (362,298,428,344), 'Unterschenkel_B_links': (356,352,392,430),
        'Maulzone': (370,183,412,206), 'Schulter_B_links': (318,160,360,190), 'Schulter_B_rechts': (425,165,470,195)}
out['orig1'] = {k: stat(o1, er(box(o1.shape, b))) for k, b in BOX1.items()}
IDS = {'Kopf': [9], 'Rumpf': [8], 'Oberarm_B_links': [13], 'Oberarm_B_rechts': [10], 'Unterarm_B_rechts': [11], 'Unterarm_B_links': [14],
       'Jeans': [0,1,4], 'Unterschenkel': [2,3,5,6], 'Klaue_B_rechts': [12], 'Sichel_B_links': [15], 'Schwanz': list(range(16,23))}
for name in ('V4', 'REF2', 'V3r2'):
    img = L('%s_135.png' % name); ids = np.load('%s_135_ids.npy' % name)[:480]
    V = G.videoraum(img)
    d = {k: stat(V, er(np.isin(ids, v))) for k, v in IDS.items()}
    # Maulzone Port: unteres Viertel der Kopfbox + 14 px darunter, nur Kopf/Rumpf-Ids
    ys, xs = np.nonzero(ids == 9); kb = (xs.min(), ys.min(), xs.max(), ys.max())
    zb = (kb[0]+6, int(kb[1] + 0.75*(kb[3]-kb[1])), kb[2]-6, kb[3]+14)
    d['Maulzone'] = stat(V, box(V.shape, zb) & np.isin(ids, (8, 9)))
    d['Kopfbox'] = [int(v) for v in kb]
    out[name] = d
# per-Id Tabelle V4 vs REF2 (alle Ids)
tab = {}
for name in ('V4', 'REF2'):
    img = L('%s_135.png' % name); ids = np.load('%s_135_ids.npy' % name)[:480]; V = G.videoraum(img)
    tab[name] = {int(i): G.r1(G.lab(V[ids == i]).mean(0)) + [int((ids == i).sum())] for i in np.unique(ids) if i >= 0}
out['per_id_135'] = tab
# ---- seitlich: Original n1435 roh vs Port Pose 85
o2 = L('vorlage2_entbl.png')
BOX2 = {'Kopf': (412,190,467,263), 'Buckel': (372,186,414,215), 'Oberarm_nah': (395,250,440,300), 'Jeans': (405,300,440,340)}
out['orig2'] = {k: stat(o2, er(box(o2.shape, b))) for k, b in BOX2.items()}
# Schwanz entlang der Maskenachse (Leser A), 5x5-Fenster
ACHSE = [(330,236),(310,243),(292,262),(289,280),(288,300),(287,318),(290,335)]
def fenster5(V, p): x,y = p; return V[y-2:y+3, x-2:x+3].reshape(-1,3)
out['schwanz_orig2'] = {str(p): G.r1(G.lab(fenster5(o2, p)).mean(0)) for p in ACHSE}
for name in ('V4', 'REF2', 'V3r2'):
    img = L('%s_85.png' % name); ids = np.load('%s_85_ids.npy' % name)[:480]; V = G.videoraum(img)
    ys, xs = np.nonzero(ids == 9); kb = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))
    d = {'Kopf': stat(V, er(ids == 9)), 'Kopfbox': kb, 'Kopf_BxH_px': (kb[2]-kb[0]+1, kb[3]-kb[1]+1),
         'Schwanz_gesamt': stat(V, er(np.isin(ids, range(16,23)), 1)), 'Rumpf': stat(V, er(ids == 8))}
    sw = np.isin(ids, range(16,23)); sy, sx = np.nonzero(sw)
    pts = {}
    for p in ACHSE:
        # naechster Schwanzpixel zum Achsenpunkt (Port-Schwanz liegt nach T2-T5 an derselben Stelle)
        j = np.argmin((sx-p[0])**2 + (sy-p[1])**2); q = (int(sx[j]), int(sy[j]))
        w = fenster5(V, q); mm = sw[q[1]-2:q[1]+3, q[0]-2:q[0]+3].reshape(-1)
        pts[str(p)] = dict(port_pt=q, abstand=round(float(np.hypot(q[0]-p[0], q[1]-p[1])),1), lab=G.r1(G.lab(w[mm]).mean(0)) if mm.any() else None)
    d['Schwanz_achse'] = pts
    out[name + '_85'] = d
out['H'] = dict(H1_orig=302, H2_orig=253, H_port_135=364, H_port_85=264)
json.dump(out, open('messung_sicht.json', 'w'), indent=1, default=str)
# Ausgabe
def z(d): return '%s n=%d Lab %s C %.1f rot %.2f hell30 %.2f L10/50/90 %s' % (d['rgb'], d['n'], d['lab'], d['chroma'], d['rot_anteil'], d['hell30'], d['L_p10_50_90']) if d else 'None'
print('== VORN Original (Boxen, 2px erodiert, entblendet roh)')
for k, v in out['orig1'].items(): print('  %-22s %s' % (k, z(v)))
for name in ('V4', 'REF2', 'V3r2'):
    print('== VORN %s (Ids, Videoraum)  Kopfbox %s' % (name, out[name]['Kopfbox']))
    for k, v in out[name].items():
        if k != 'Kopfbox': print('  %-22s %s' % (k, z(v)))
print('== SEITE Original'); 
for k, v in out['orig2'].items(): print('  %-12s %s' % (k, z(v)))
print('  Schwanz-Achse orig:', out['schwanz_orig2'])
for name in ('V4', 'REF2', 'V3r2'):
    d = out[name+'_85']; print('== SEITE %s Kopfbox %s BxH %s' % (name, d['Kopfbox'], d['Kopf_BxH_px']))
    print('  Kopf', z(d['Kopf'])); print('  Rumpf', z(d['Rumpf'])); print('  Schwanz', z(d['Schwanz_gesamt']))
    for p, q in d['Schwanz_achse'].items(): print('   ', p, q)
