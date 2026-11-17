"""Schnelltest der Teile von riegel_r7c.py (ohne riegel_v8.alles) mit Zeiten."""
import os, sys, time
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
os.chdir('C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V, riegel_r7c as R
S = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/'
P3 = S + 'p5/pruef_zahlen-r3/laeufe/'
m = S + 'r7c/m/'
kand = sys.argv[1] if len(sys.argv) > 1 else 'KC6'
lm = {'KC6': S + 'r7c/mk/lauf_LM_NEU', 'R3B': S + 'r7c/laeufe/R3B_LM', 'R2': P3 + 'R2_LM', 'V7': P3 + 'V7_LM'}[kand]
pl = {'KC6': S + 'r7c/laeufe/KC6', 'R3B': S + 'r7c/laeufe/R3B', 'R2': P3 + 'R2', 'V7': P3 + 'V7'}[kand]
t = time.time()
blob, v7, r4 = open(m + kand + '.EMD', 'rb').read(), open(m + 'V7.EMD', 'rb').read(), open(m + 'R4.EMD', 'rb').read()
cos = R.cos_je_ansicht(v7); print('cos', round(time.time() - t), 's', {k: round(cos[k], 2) for k in ('F312', 'F313', 'F340', 'PV', 'P2')}, flush=True)
t = time.time(); Kk = R.lauf_klassen(lm, pl, *R.F_SZENE); K7 = R.lauf_klassen(P3 + 'V7_LM', P3 + 'V7', *R.F_SZENE)
Ke = R.lauf_klassen(S + 'r7c/laeufe/N7B_LM', S + 'r7c/laeufe/N7B', *R.F_SZENE); print('klassen', round(time.time() - t), 's', flush=True)
t = time.time()
e = R.e9_echt({f: k for f, k in Kk.items() if 235 <= f <= 378}, {f: k for f, k in K7.items() if 235 <= f <= 378}, pl, cos)
print('e9_echt', round(time.time() - t), 's', {k: e[k] for k in ('vorn', 'vorn_v7', 'artefakte', 'seite', 'seite_e3', 'zacke', 'zacke_folgen', 'ok')}, flush=True)
t = time.time(); g = R.gesicht_echt(Kk, K7, Ke); print('gesicht', round(time.time() - t), 's', g, flush=True)
t = time.time(); print('kippen', R.kippen(Kk), 'ein', R.kippen(Ke), round(time.time() - t), 's', flush=True)
t = time.time(); print('orange', R.orange(pl, S + 'r7c/laeufe/N7B'), round(time.time() - t), 's', flush=True)
t = time.time(); print('hell', R.hell_e12(pl, S + 'p5/pruef_zahlen-r2/laeufe/R4'), round(time.time() - t), 's', flush=True)
t = time.time(); print('texel', R.fremde_texel(blob, [open(m + 'N7B.EMD', 'rb').read(), r4, v7]), round(time.time() - t), 's', flush=True)
t = time.time(); s = R.s18_r7c(blob, v7, r4, frei=R.E10_ECKEN); print('s18', round(time.time() - t), 's', {k: s[k] for k in ('a1_max', 'a1', 'a1_frei_max', 'a3_max', 'a3', 'ergaenzung', 'kragen_px')}, flush=True)
