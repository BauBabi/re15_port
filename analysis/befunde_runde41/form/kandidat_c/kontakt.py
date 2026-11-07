from bau import *
T = pickle.load(open('teile.pkl', 'rb'))
kn = knochen_tafel(T)
form = FE.lese_emd(open('FORM.EMD', 'rb').read())
for ci, c in enumerate(form['clips']):
    b = c['bilder'][0]
    R, Tr = FE.pose_aus_bild(form, b)
    z = []
    for i, k in enumerate(form['knochen']):
        v = form['meshes'][i]['ecken']
        if v:
            p = np.array(v, float) @ (R[i].astype(float).T / 4096.0) + Tr[i]
            z.append((k['name'], round(float(p[:, 1].max()))))
    print('Clip', ci, 'Wurzel', form['pool'][b['kf']]['wurzel'] if 'kf' in b else b['wurzel'], [q for q in z if q[0] in ('k3', 'k4', 'k7', 'k8', 'k12', 'k13', 'k18', 'k19')])
print([ (i, k['name'], k['eltern']) for i, k in enumerate(form['knochen'])])
print({m: len(form['meshes'][i]['vierecke']) + len(form['meshes'][i]['dreiecke']) for i, m in enumerate(k['name'] for k in form['knochen'])})
t = FE.tim_lesen(open('FORM.TIM', 'rb').read())
print('CLUT-Farben', len(set(t['cluts'][0])), 'Index max', t['pixel'].max())
