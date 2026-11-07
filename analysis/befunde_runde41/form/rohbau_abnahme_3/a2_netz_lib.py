import numpy as np
NAMEN = {0:'huefte',1:'s1',2:'s2',3:'s3',4:'s4',5:'s5',6:'s6',7:'wirbel',8:'oschenkel_l',9:'oschenkel_r',10:'brust',11:'oarm_l',12:'uarm_l',13:'hand_l',14:'f14',15:'f15',16:'oarm_r',17:'uarm_r',18:'hand_r',19:'f19',20:'f20',21:'hals',22:'kopf',23:'kiefer',24:'uschenkel_l',25:'fuss_l',26:'uschenkel_r',27:'fuss_r',28:'f28',29:'f29',30:'k30',31:'k31'}
def netz(k):
    aus = []
    for b in range(min(k.mo.skel.bone_count, len(k.mo.meshes))):
        m = k.mo.meshes[b]
        vl, tl = [], []
        off = 0
        if m.tf:
            v = np.array([x[:3] for x in m.tv], float); vl.append(v)
            for g in m.tf:
                ie = g[1::2]; tl.append((ie[0], ie[1], ie[2]))
            off = len(v)
        if m.qf:
            v = np.array([x[:3] for x in m.qv], float); vl.append(v)
            for g in m.qf:
                ie = [i + off for i in g[1::2]]
                tl.append((ie[0], ie[1], ie[3])); tl.append((ie[0], ie[3], ie[2]))
        aus.append((np.vstack(vl) if vl else np.zeros((0, 3)), np.array(tl, int).reshape(-1, 3)))
    return aus
