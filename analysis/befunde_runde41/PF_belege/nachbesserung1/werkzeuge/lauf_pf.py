"""Lauf pf des Riegels _schrank_* von Hand (Muster test_r41_nachtrag30.cmake Lauf pf): F720 Cut 3, Tuer offen, Figur weg -> f.ppm.
lauf_pf.py <name> <exe> [leon|elza]"""
import os, sys, subprocess, shutil, random, re
name, exe = sys.argv[1], sys.argv[2]
figur = sys.argv[3] if len(sys.argv) > 3 else 'leon'
S = '1' if figur == 'elza' else '0'
wd = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'runs_pf', name)
shutil.rmtree(wd, ignore_errors=True); os.makedirs(wd)
env = dict(os.environ)
env['PATH'] = r'C:\msys64\mingw64\bin;' + env['PATH']
env.update({'SDL_ASSERT': 'always_ignore', 'RE15_NO_INTRO': '1', 'RE15_WINDOW_SCALE': '1', 'RE15_MSG_LOG': '1', 'RE15_NOAUDIO': '1',
            'RE15_SOFTWARE_RENDER': '1', 'RE15_DEBUG_JUMP': '2010@250', 'RE15_PLAYER_POS': '4300,-2770,0,0', 'RE15_FORCE_CUT': '3',
            'RE15_INPUT_SCRIPT_BASIS': 'spiel', 'RE15_INPUT_SCRIPT_START': '330',
            'RE15_INPUT_SCRIPT': 'A0.1,W2,A0.1,W1.5,A0.1,W1.5,A0.1,W1.5,A0.1,W1,D2.5,W8', 'RE15_FRAMEDUMP': '720:f.ppm', 'RE15_EXIT_AT': '740#201' + S})
if figur == 'elza':
    env.update({'RE15_TITLE_CONFIRM_MS': '1500', 'RE15_PSELECT_AUTO': '1', 'RE15_PSELECT_AUTO_SWITCH': '1'})
else:
    env.update({'RE15_TITLE_SHOT': 'title.bmp', 'RE15_TITLE_SHOT_AF': '2'})
kopie = os.path.join(os.path.dirname(exe), 're15_pc_nb1pf_%s_%06d.exe' % (re.sub(r'\W', '', name), random.randrange(10**6)))
shutil.copy(exe, kopie)
try:
    r = subprocess.run([kopie], cwd=wd, env=env, timeout=300, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print(name, 'exit', r.returncode, 'f.ppm', os.path.exists(os.path.join(wd, 'f.ppm')))
finally:
    try: os.remove(kopie)
    except OSError: pass
