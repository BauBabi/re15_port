"""Pruefer V3: Lage, Groesse, Knochen der Leuchtstellen Bild 1 (F1) und Bild 2 (Unterkiefer), eigene Regeln."""
from szene import *
import json
R = {}
LB = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/leserb/'


def mk(nm):
    return cv2.imread(FORM + '/nutzer_teile/maske_%s.png' % nm, 0) > 0


LOGO = np.zeros((480, 640), bool); LOGO[428:, :262] = True


def form(msk):
    ys, xs = np.nonzero(msk)
    c = np.cov(np.stack([xs, ys]).astype(float)); w, v = np.linalg.eigh(c)
    ach = float(np.degrees(np.arctan2(v[1, 1], v[0, 1]))) % 180
    return dict(px=int(msk.sum()), bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                schwerpunkt=[round(float(xs.mean()), 1), round(float(ys.mean()), 1)],
                laenge_breite_4sigma=[round(4 * float(np.sqrt(w[1])), 1), round(4 * float(np.sqrt(w[0])), 1)], achse_grad=round(ach, 0))


def knochen(msk, kn):
    b, c = np.unique(kn[msk], return_counts=True)
    return {int(x): int(y) for x, y in sorted(zip(b, c), key=lambda t: -t[1])}


def r1(v):
    return [round(float(x), 1) for x in v]


# ---------------- Bild 1
V1 = P.original(1); e1 = FV.video_umkehr(V1, 1)
LV1 = lab(V1)
kn1 = knochen_karte(1); m1 = np.load('m1.npy'); L1 = np.load('L1.npy')
box = np.zeros((480, 640), bool); box[225:266, 300:346] = True        # Auftrag: grob x 300-345, y 225-265
seed = np.unravel_index(np.argmax(np.where(box, LV1[..., 1], -99)), box.shape)
R['bild1_seed_va_max'] = [int(seed[1]), int(seed[0]), r1(LV1[seed])]
tab = {}
for la, aa in ((25, 20), (30, 15), (30, 20), (30, 25), (30, 30), (35, 20), (35, 25), (40, 25), (45, 25), (50, 25)):
    q = ((LV1[..., 0] >= la) & (LV1[..., 1] >= aa)).astype(np.uint8)
    nl, lb = cv2.connectedComponents(q, connectivity=8)
    tab['VL>=%d,Va>=%d' % (la, aa)] = int((lb == lb[seed]).sum())
R['bild1_flaeche_je_schwelle_video'] = tab
q = ((LV1[..., 0] >= 30) & (LV1[..., 1] >= 20)).astype(np.uint8)
nl, lb = cv2.connectedComponents(q, connectivity=8)
F1 = lb == lb[seed]
np.save('F1_pruef.npy', F1)
R['bild1_F1_regel'] = 'Video-Lab L*>=30 und a*>=20 (L*>=35 flutet ueber den Hintergrund ins ganze Bild, gemessen), 8-zusammenhaengend mit dem a*-Maximum in der Auftragsbox (x 300-345, y 225-265); PORT-WAHL Pruefer'
R['bild1_F1_form'] = form(F1)
if os.path.exists(LB + 'F1.npy'):
    B_F1 = np.load(LB + 'F1.npy').astype(bool)
    R['bild1_F1_gegen_LeserB'] = dict(B_px=int(B_F1.sum()), schnitt=int((B_F1 & F1).sum()), IoU=round(float((B_F1 & F1).sum() / (B_F1 | F1).sum()), 3))
R['bild1_F1_im_K5c_render_px'] = int((F1 & m1).sum())
R['bild1_F1_knochen'] = knochen(F1 & m1, kn1)
aK, LK, mK, _ = render(1, [21, 22, 23])
R['bild1_kopf_unverdeckt'] = form(mK)
R['bild1_kopf_sichtbar_px'] = {b: int(((kn1 == b) & m1).sum()) for b in (21, 22, 23)}
R['bild1_F1_auf_kopfprojektion_px'] = int((F1 & mK).sum())
a22, L22, m22, _ = render(1, [22, 23])
R['bild1_F1_auf_kopf22_23_unverdeckt_px'] = int((F1 & m22).sum())
R['bild1_kopf22_23_unverdeckt'] = form(m22)
ys, xs = np.nonzero(m22); fy, fx = np.nonzero(F1)
R['bild1_abstand_F1_schwerpunkt_zu_kopf22_23_px'] = round(float(np.hypot(xs - fx.mean(), ys - fy.mean()).min()), 1)
a0, L0, m0, _ = render(1, [0])
R['bild1_F1_auf_becken_unverdeckt_px'] = int((F1 & m0).sum())
R['bild1_F1_licht_K5c_median'] = [round(float(v), 3) for v in np.median(L1[F1 & m1], 0)]
R['bild1_F1_licht_K5c_p10_p90'] = [[round(float(v), 3) for v in np.percentile(L1[F1 & m1], p, 0)] for p in (10, 90)]
R['bild1_F1_V'] = r1(V1[F1].mean(0))
R['bild1_F1_V_lab'] = r1(lab(V1[F1].mean(0)))
R['bild1_F1_e'] = r1(e1[F1].mean(0))
R['bild1_F1_e_lab'] = r1(lab(np.clip(e1[F1].mean(0), 0, 255)))

# ---------------- Bild 2
V2 = P.original(2); e2 = FV.video_umkehr(V2, 2)
LV2 = lab(V2); Le2 = lab(np.clip(e2, 0, 255))
kn2 = knochen_karte(2); m2 = np.load('m2.npy'); L2 = np.load('L2.npy')
uk = mk('unterkiefer') & ~LOGO; ok = mk('oberkiefer') & ~LOGO
R['bild2_unterkiefer_form'] = form(uk)
R['bild2_unterkiefer_K5c_px'] = int((uk & m2).sum())
R['bild2_unterkiefer_knochen'] = knochen(uk & m2, kn2)
C = np.hypot(Le2[..., 1], Le2[..., 2]); h = np.degrees(np.arctan2(Le2[..., 2], Le2[..., 1]))
clip2 = V2.min(2) >= 200
rot = (Le2[..., 1] >= 10) & (h > -10) & (h <= 60) & ~clip2
neu = (C < 10) & (Le2[..., 0] >= 20) & ~clip2
gelb = (C >= 10) & (h > 60) & (h <= 100) & ~clip2
dun = (Le2[..., 0] < 10) & ~rot & ~clip2
kl = {}
for nm, q in (('rot', rot), ('neutral_hell', neu & ~rot), ('gelb', gelb & ~rot), ('dunkel', dun), ('geclippt', clip2)):
    kl[nm] = round(100 * float((q & uk).sum() / uk.sum()), 1)
kl['rest'] = round(100 - sum(kl.values()), 1)
R['bild2_klassenregel'] = 'e-Lab: rot a*>=10 und Farbton -10..60; neutral-hell C*<10 und L*>=20; gelb C*>=10 und Farbton 60..100; dunkel L*<10; geclippt Video min>=200 (PORT-WAHL Pruefer, andere Grenzen als Leser B)'
R['bild2_unterkiefer_klassen_e_prozent'] = kl
band = uk & rot; zahn = uk & neu & ~rot
np.save('band2_pruef.npy', band); np.save('zahn2_pruef.npy', zahn)
for nm, q in (('band', band), ('zaehne', zahn)):
    R['bild2_%s_form' % nm] = form(q)
    R['bild2_%s_knochen' % nm] = knochen(q & m2, kn2)
    R['bild2_%s_V' % nm] = r1(V2[q].mean(0))
    R['bild2_%s_e_lab' % nm] = r1(lab(np.clip(e2[q].mean(0), 0, 255)))
    R['bild2_%s_licht_K5c_median' % nm] = [round(float(v), 3) for v in np.median(L2[q & m2], 0)]
bxA = np.zeros((480, 640), bool); bxA[310:421, 255:346] = True
bxA &= cv2.imread(FORM + '/maske_bild2.png', 0) > 0
thr = np.percentile(LV2[..., 0][bxA], 90)
selA = bxA & (LV2[..., 0] >= thr)
np.save('selA2.npy', selA)
R['bild2_auswahlA'] = dict(px=int(selA.sum()), im_unterkiefer=round(100 * float((selA & uk).sum() / selA.sum()), 1),
                           davon_zaehne=round(100 * float((selA & zahn).sum() / selA.sum()), 1), davon_band=round(100 * float((selA & band).sum() / selA.sum()), 1),
                           geclippt=round(100 * float((selA & clip2).sum() / selA.sum()), 1), e_lab=r1(lab(np.clip(e2[selA].mean(0), 0, 255))))
wf = clip2.copy(); wf[:, :150] = False; wf[:, 230:] = False; wf[:320] = False; wf[395:] = False
R['bild2_weisse_flecken'] = dict(form(wf), K5c_px=int((wf & m2).sum()), knochen=knochen(wf & m2, kn2),
                                 in_nutzerteilen=int(sum((wf & mk(t)).sum() for t in ('unterkiefer', 'oberkiefer', 'face_and_upper_body', 'right_arm_front', 'left_arm_front'))))
R['bild2_oberkiefer_e_lab'] = r1(lab(np.clip(e2[ok].mean(0), 0, 255)))
R['bild2_oberkiefer_anteil_eL_unter_10'] = round(100 * float((Le2[..., 0][ok] < 10).mean()), 1)
R['bild2_oberkiefer_knochen'] = knochen(ok & m2, kn2)
for nm, (x, y) in (('auge_r_W', (217, 275)), ('auge_l_W', (280, 275))):
    R['bild2_%s_e_lab' % nm] = r1(lab(np.clip(e2[y - 1:y + 2, x - 1:x + 2].reshape(-1, 3).mean(0), 0, 255)))
    R['bild2_%s_knochen' % nm] = int(kn2[y, x])
R['bild2_band_oberkante_y'] = int(np.nonzero(band)[0].min()); R['bild2_zaehne_oberkante_y'] = int(np.nonzero(zahn)[0].min())
json.dump(R, open('orte.json', 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
for k, v in R.items():
    print(k, v)
