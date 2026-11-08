import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools')
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v2')
import birkin_formen as BF, b2
P = json.load(open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv2/posen_vorn.json'))
aend = json.loads(sys.argv[1])
for k, v in aend.items():
    if isinstance(v, dict) and isinstance(BF.SCHWANZ_V3.get(k), dict): BF.SCHWANZ_V3[k] = dict(BF.SCHWANZ_V3[k], **v)
    else: BF.SCHWANZ_V3[k] = v
blob = BF.baue(BF.lade_em30(), BF._variante_v3()); open('C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/bau/vprobe.emd','wb').write(blob)
mo = b2.modell('C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/bau/vprobe.emd')
for h in ('118','125'):
    im, ids = b2.zeichne(mo, P[h], None)
    m = (ids>=16)&(ids<=22); ys,xs=np.nonzero(m)
    print(h, int(m.sum()), (xs.min(),xs.max(),ys.min(),ys.max()) if m.any() else None, 'Spitze ue. Sohle', round(BF.BAU_INFO['schwanz_v2']['spitze_ueber_sohle_H'],3))
    if h=='118': cv2.imwrite('C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/bau/vprobe.png', cv2.cvtColor(im[150:450,200:500], cv2.COLOR_RGB2BGR))
