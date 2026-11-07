import struct, json, numpy as np, hashlib
W='C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
P={'asset':W+'/re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD','basis':W+'/analysis/befunde_runde41/form/stufe3/basis_k5c/FORM.EMD',
   'variante':W+'/analysis/befunde_runde41/form/varianten/K5/farbe/K5c_farbe_v3/FORM.EMD','k4':W+'/analysis/befunde_runde41/form/varianten/K4/BAHNSTEIG_FORM.EMD'}
F={k:open(p,'rb').read() for k,p in P.items()}
u16=lambda b,o:struct.unpack_from('<H',b,o)[0]; u32=lambda b,o:struct.unpack_from('<I',b,o)[0]
def D(b): o=u32(b,0); return list(struct.unpack_from('<8I',b,o))
R={}
def md1_proper(b):
    d=D(b); s=d[7]; e=len(b)-32   # Schwanz = Verzeichniskopie (8 x u32), gemessen per xxd
    assert list(struct.unpack_from('<8I',b,e))==d, 'Schwanz ist keine Verzeichniskopie'
    return b[s:e], s
def clut_offsets(md1):
    n=u32(md1,8)//2; offs=set(); words=[]
    for i in range(n):
        h=12+i*56; f=struct.unpack_from('<14I',md1,h)
        tf_cnt,tu_off,qf_cnt,qu_off=f[5],f[6],f[12],f[13]
        for k in range(tf_cnt): o=12+tu_off+12*k+2; offs.update((o,o+1)); words.append(o)
        for k in range(qf_cnt): o=12+qu_off+16*k+2; offs.update((o,o+1)); words.append(o)
    return offs,words
MA,sA=md1_proper(F['asset']); MB,_=md1_proper(F['basis']); MV,_=md1_proper(F['variante']); MK,_=md1_proper(F['k4'])
R['md1_len']={'asset':len(MA),'basis':len(MB),'variante':len(MV),'k4':len(MK)}
R['md1_asset_eq_basis']=MA==MB
offs,words=clut_offsets(MA)
a=np.frombuffer(MA,'u1'); v=np.frombuffer(MV,'u1')
diff=np.nonzero(a!=v)[0]
R['md1_asset_vs_variante']={'diff_bytes':int(len(diff)),'diff_ausserhalb_clut':int(sum(1 for i in diff if int(i) not in offs)),
    'clut_worte_gesamt':len(words),'clut_worte_geaendert':sum(1 for o in words if MA[o:o+2]!=MV[o:o+2])}
# crc32 wie der Test (zlib crc32?) - Pin FORM_J_MD1_CRC 0x2456b5cc, LEN 169500
import zlib
R['md1_crc_asset']={'len':len(MA),'crc32':hex(zlib.crc32(MA)&0xffffffff)}
T=open(W+'/re15_port/shared_assets/RE2/BAHNSTEIG_FORM.TIM','rb').read()
R['tim_crc_asset']={'len':len(T),'crc32':hex(zlib.crc32(T)&0xffffffff)}
# EMR-Struktur
def emr(b):
    d=D(b); o=d[2]; bt,kf,nb,ks=u16(b,o),u16(b,o+2),u16(b,o+4),u16(b,o+6)
    ende=d[3] if d[3]>o else len(b)-32
    return {'o':o,'bt':bt,'kf':kf,'nb':nb,'ks':ks,'struct':b[o+4:o+kf],'pool':b[o+kf:ende],'ende':ende}
EA,EV,EB,EK=emr(F['asset']),emr(F['variante']),emr(F['basis']),emr(F['k4'])
R['emr']={k:{kk:vv for kk,vv in e.items() if kk not in('struct','pool')} for k,e in (('asset',EA),('variante',EV),('basis',EB),('k4',EK))}
R['emr_struct_asset_eq_variante']=EA['struct']==EV['struct']
R['emr_struct_asset_eq_basis']=EA['struct']==EB['struct']
R['emr_struct_asset_eq_k4']=EA['struct']==EK['struct']
# rel-Pos-Unterschiede zu K4
relA=[struct.unpack_from('<3h',F['asset'],EA['o']+8+6*k) for k in range(EA['nb'])]
relK=[struct.unpack_from('<3h',F['k4'],EK['o']+8+6*k) for k in range(EK['nb'])]
R['relpos_diff_zu_k4']={k:{'asset':relA[k],'k4':relK[k]} for k in range(min(len(relA),len(relK))) if relA[k]!=relK[k]}
R['bonetable_asset_eq_k4']=F['asset'][EA['o']+EA['bt']:EA['o']+EA['kf']]==F['k4'][EK['o']+EK['bt']:EK['o']+EK['kf']]
# Keyframes
kfA=[EA['pool'][i*EA['ks']:(i+1)*EA['ks']] for i in range(len(EA['pool'])//EA['ks'])]
kfV=[EV['pool'][i*EV['ks']:(i+1)*EV['ks']] for i in range(len(EV['pool'])//EV['ks'])]
kfB=[EB['pool'][i*EB['ks']:(i+1)*EB['ks']] for i in range(len(EB['pool'])//EB['ks'])]
R['keyframes']={'asset':len(kfA),'variante':len(kfV),'basis':len(kfB),'pool_rest_asset':len(EA['pool'])%EA['ks']}
sa=set(kfA)
R['variante_kf_im_asset']=[kf in sa for kf in kfV]
R['variante_kf_index_im_asset']=[kfA.index(kf) if kf in sa else -1 for kf in kfV]
R['basis_kf_eq_variante']=kfB==kfV
# EDD
def edd(b):
    d=D(b); o=d[1]; e=d[2]; return b[o:e]
R['edd_len']={'asset':len(edd(F['asset'])),'variante':len(edd(F['variante'])),'k4':len(edd(F['k4']))}
R['edd_asset_eq_k4']=edd(F['asset'])==edd(F['k4'])
R['edd_clips']={k:u16(F[k],D(F[k])[1]+2)//4 for k in F}
# Clip-Tabelle: je Clip (count u16, offset u16) -> Bilder
def clips(b):
    o=D(b)[1]; n=u16(b,o+2)//4; out=[]
    for i in range(n):
        cnt,off=u16(b,o+4*i),u16(b,o+4*i+2); out.append(cnt)
    return out
R['clip_bilder_asset']=clips(F['asset']); R['clip_bilder_k4']=clips(F['k4']); R['clip_bilder_variante']=clips(F['variante'])
R['clip_bilder_summe_asset']=sum(R['clip_bilder_asset'])
# Bild-Eintraege (u32 je Bild: kf-Index & 0xfff) -> alle Indizes < keyframe_count?
def frames(b):
    o=D(b)[1]; n=u16(b,o+2)//4; idx=[]
    for i in range(n):
        cnt,off=u16(b,o+4*i),u16(b,o+4*i+2)
        for k in range(cnt): idx.append(u32(b,o+off+4*k)&0xfff)
    return idx
fa=frames(F['asset']); R['frame_idx_max_asset']=max(fa); R['frame_idx_alle_gueltig']=max(fa)<len(kfA)
print(json.dumps(R,indent=1,default=str))
json.dump(R,open('aus/pruef_p2.json','w'),indent=1,default=str)
