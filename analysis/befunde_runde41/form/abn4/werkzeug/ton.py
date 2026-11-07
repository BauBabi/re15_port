# Abnahme 3 (eigenes Werkzeug): Raum-Ton 10 (Gebruell) gespielt ([se] Stimme: se=10) / vom SE-Tor verworfen ([se] GATE: se=10 ... VERWORFEN) / SILENT.
import sys, re, os
d = sys.argv[1]
dbg = open(os.path.join(d, 'debug.log'), errors='replace').read() if os.path.exists(os.path.join(d, 'debug.log')) else ''
r2 = open(os.path.join(d, 're2se.log'), errors='replace').read() if os.path.exists(os.path.join(d, 're2se.log')) else ''
ges = len(re.findall(r'^\[se\] Stimme: se=10 ', dbg, re.M))
verw = len(re.findall(r'^\[se\] GATE: se=10 .*VERWORFEN', dbg, re.M))
sil = len(re.findall(r'SILENT', dbg)) + len(re.findall(r'SILENT', r2))
silz = [l.strip()[:90] for l in (dbg + r2).splitlines() if 'SILENT' in l][:3]
print('%s: Raum-Ton 10 gespielt %d, verworfen %d, SILENT %d %s' % (os.path.basename(d.rstrip('/')), ges, verw, sil, silz))
