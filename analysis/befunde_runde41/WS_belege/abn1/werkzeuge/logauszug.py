# Log-Auszug der Abnahme WS Runde 1: je Lauf Umgebung, Harness-/Ausloeser-Zeilen und HP-Folge der Ziel-Slots.
import re, os, sys, subprocess
S = sys.argv[1]; out = sys.argv[2]
laeufe = [l.split() for l in sys.argv[3].split(';') if l.strip()]   # "name slot,slot"
hp = os.path.join(S, 'hpfolge.py')
with open(out, 'w', encoding='utf-8') as f:
    f.write('Spur WS - Abnahme 1 - Log-Auszuege (eigene Laeufe, Messkopie re15_pc_wsabn.exe = Kopie von re15_port/build/platform/pc/re15_pc.exe,\n'
            'Bau 12:45:37 auf ac72cd3cb, ninja: no work to do). HP-Folgen: nur Wechsel von Typ/Zustand/HP je Slot aus RE15_STATE_LOG,\n'
            'Bildnummern F = Spielbilder nach dem Debug-Sprung (seg1). Gemeinsame Umgebung: SDL_ASSERT=always_ignore RE15_NO_INTRO=1\n'
            'RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1 RE15_STATE_LOG=state.log, beschleunigter Renderer.\n\n')
    for name, slots in laeufe:
        d = os.path.join(S, 'laeufe', name)
        f.write('== %s\n' % name)
        if os.path.exists(os.path.join(d, 'lauf.txt')):
            for l in open(os.path.join(d, 'lauf.txt')): f.write('   ' + l)
        for fn in ('debug.log', 'gr.log'):
            p = os.path.join(d, fn)
            if not os.path.exists(p): continue
            for l in open(p, errors='replace'):
                if re.search(r'Spieler-Familie|AUTO-JUMP|FORCE_EXPLOSION F|AUSLOESER|EV resolver|\[gr\] RE2|ZERFETZT|EXIT_AT|PLAYER_POS|setflag', l):
                    f.write('   ' + l.rstrip()[:220] + '\n')
        for s in slots.split(','):
            r = subprocess.run([sys.executable, hp, os.path.join(d, 'state.log'), s], capture_output=True, text=True).stdout
            for l in r.splitlines():
                if 'fehlt' in l: continue
                f.write('   ' + l + '\n')
        f.write('\n')
print(out)
