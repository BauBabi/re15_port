"""7a: schnelle Bewertung einer hand_v8-Variante (S17 a,c,d,e ohne g; S20 b,c) aus der Basis nach Stufe srwaun."""
import os, sys, json, copy, hashlib
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W)
import v8_gemein as V
import v8_bau as VB
import hand_szene as SZ
import hand_mass as HM
import riegel_hand as RH
HIER = os.path.dirname(os.path.abspath(__file__))
V7P = os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD')
BASIS_P = os.path.join(HIER, 'emd', 'basis_srwaun.EMD')
if not os.path.exists(BASIS_P):
    b, _ = VB.bauen(V7P, 'srwaun')
    open(BASIS_P, 'wb').write(b)
BASIS = open(BASIS_P, 'rb').read()
REF = open(V7P, 'rb').read()
CFG0 = json.load(open(os.path.join(HIER, 'cfg_6d.json'), encoding='utf-8'))


def blob(cfg):
    return SZ.probe_blob(BASIS, cfg)


def bewerten(cfg, s20=False):
    b = blob(cfg)
    s = RH.s17(b, REF, mit_g=False)
    a, c, d, e = s['a'], s['c'], s['d'], s['e']
    out = dict(sha=hashlib.sha256(b).hexdigest()[:8],
               spitzen=[a['posen'][pn]['spitzen'] for pn in HM.PAARE], kw=[a['posen'][pn]['krallenwurzel'] for pn in HM.PAARE], km=[a['posen'][pn]['krallen_mitte'] for pn in HM.PAARE], a_aus=a['ausser'], a_ok=a['ok'], eng=a['eng'], eng_min=a['eng_min'],
               c_ok=c['ok'], hl=list(c['hand_laenge'].values()), bl=list(c['breite_durch_laenge'].values()), hb=list(c['hand_breit'].values()),
               krallen=d['krallen'], laenge_aus=d['laenge_ausser'], duenn=d['duenner_als_v7'], daumen=d['daumen'], d_aus=d['daumen_ausser'], d_ok=d['ok'],
               kreuzen={k: v for k, v in e['kreuzen'].items() if v}, e_ok=e['ok'], f_ok=s['f']['ok'])
    if s20:
        t = RH.s20(b, REF)
        out['s20'] = dict(ok=t['ok'], rot=t['teile_rot'], winkel=t['b']['winkel'], breit=t['b']['breit'], hg=t['c']['handgelenk'], sp=t['c']['spitze'])
    return out, b


def zeile(name, r):
    return ('%-14s %s eng %s/%s KW %s KM %s spitzen %s aus %s a%s c%s d%s e%s f%s | HL %s BL %s HB %s | Krallen %s | duenn %s lang_aus %s | Daumen %s aus %d | kreuz %s'
            % (name, r['sha'], r['eng'], r['eng_min'], r['kw'], r['km'], r['spitzen'], r['a_aus'], 'G' if r['a_ok'] else 'R', 'G' if r['c_ok'] else 'R', 'G' if r['d_ok'] else 'R', 'G' if r['e_ok'] else 'R',
               'G' if r['f_ok'] else 'R', r['hl'], r['bl'], r['hb'], {k: v[0] for k, v in r['krallen'].items()}, r['duenn'], r['laenge_aus'], r['daumen'], len(r['d_aus']), r['kreuzen']))


if __name__ == '__main__':
    r, b = bewerten(CFG0, s20=True)
    print(hashlib.sha256(b).hexdigest())
    print(zeile('6d', r)); print(r.get('s20'))
