#!/usr/bin/env python3
"""Runde 30 / tuer-verschlossen: steckt eine der RE2-Verschlossen-Wellen schon in einer
RE1.5-Raumbank?  Huellkurven-Vergleich JEDER RE1.5-Welle gegen die RE2-Familien A/B/E/P."""
import sys, os, glob
sys.path.insert(0,'analysis/befunde_runde30/tools')
from r30_re2_roombank import load_bank, resolve
from r30_re2_wellen_wav import vag_decode, note2pitch2
from r30_re2_wellen_familien import huelle, pearson
def wave(path,sid):
    b=load_bank(path); r=resolve(b,sid)
    if r is None or r['empty']: return None
    L=r['layers'][0]
    if 'sha1' not in L: return None
    raw=b['d'][L['vag_off']:L['vag_off']+L['vag_size']]
    pcm=vag_decode(raw); pitch=note2pitch2(L['mn'],L['shift'],L['center'],L['shift']); rate=44100*pitch//4096
    return dict(sha=L['sha1'][:12],size=L['vag_size'],rate=rate,dauer=len(pcm)/rate,env=huelle(pcm,rate),raw=r['raw'].hex(' '),vol=L['vol'])
re2={'A':wave('info/re2leon/PL0/RDT/ROOM1140.RDT',0x16),'B':wave('info/re2leon/PL0/RDT/ROOM1050.RDT',0x16),'E':wave('info/re2leon/PL0/RDT/ROOM2110.RDT',0x16),'P':wave('info/re2leon/PL0/RDT/ROOM7020.RDT',0x16)}
seen={}
for f in sorted(glob.glob('re15_port/shared_assets/PSX/STAGE*/ROOM*0.RDT')):
    if os.path.getsize(f)<0x100: continue
    room=os.path.basename(f)[4:8]
    b=load_bank(f)
    if b is None: continue
    for sid in range(b['n']):
        raw=b['d'][b['edt']+sid*4:b['edt']+sid*4+4]
        if raw in (b'\0\0\0\0',b'\xff\xff\xff\xff'): continue
        w=wave(f,sid)
        if not w: continue
        seen.setdefault(w['sha'],dict(w=w,wo=[]))['wo'].append('%s/0x%02x'%(room,sid))
print('# RE1.5-Raumbaenke (Leon-Varianten): %d verschiedene Wellen ueber ALLE Saetze 0x00..0x1F' % len(seen))
print('# Huellkurven-Korrelation jeder RE1.5-Welle gegen die RE2-Verschlossen-Wellen A/B/E/P')
best=[]
for sha,v in seen.items():
    w=v['w']
    for k in 'ABEP':
        r=pearson(w['env'],re2[k]['env'])
        gl=abs(w['dauer']-re2[k]['dauer'])<=0.05*max(w['dauer'],re2[k]['dauer'])
        best.append((r,gl,k,sha,w['size'],w['dauer'],v['wo'][:6]))
best.sort(reverse=True)
print('# Treffer (gleiche Dauer +-5 %% UND r >= 0.90): %d' % sum(1 for b in best if b[1] and b[0]>=0.90))
print('# die 12 hoechsten Korrelationen:')
for r,gl,k,sha,size,dauer,wo in best[:12]:
    print('  r=%.2f gegen %s  gleicheDauer=%s  RE1.5 %s %d B %.3f s  %s' % (r,k,gl,sha,size,dauer,' '.join(wo)))
