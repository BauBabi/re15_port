"""Pruefer P5 R3: E9-Aufteilung. cos Kopf-Vorwaertsachse (Knochen 9 lok +x) zur Kamera, Pose aus eigenem V7-Lauf (vorn.cosvorn, V7-Knochen;
Knochen/Keyframes in allen Staenden bytegleich, struktur_R3.txt). vorn = cos >= 0,5 (E9). Offline: off/luecke_<N>.json; echt: laeufe/<N>_luecke.json.
Aufruf: python split.py N1 N2 ... -> off/split.json + Ausgabe"""
import sys, os, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pl, luecke_eigen as L, vorn as VO
R = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/pruef_zahlen-r3/'
V7 = VO.lade('V7')
COS = {nm: VO.cosvorn(V7, p) for nm, p in L.ansichten()}
FOTO = ('PV', 'P1', 'P2')
AUS = set(range(316, 334))   # Vordergrund-Bilder (in allen Staenden zahlengleich)
out = {'cos': COS}
for n in sys.argv[1:]:
    off = json.load(open(R + 'off/luecke_%s.json' % n))
    ue = []
    for nm, zl in off:
        if not all(L.soll(m) for m in zl):
            ue.append((nm, max(m['summe'] for m in zl), max(m['tief'] for m in zl), max(m['maxtiefe'] for m in zl), round(COS[nm], 2)))
    vo = [u for u in ue if COS[u[0]] >= 0.5 and u[0] not in FOTO]; se = [u for u in ue if COS[u[0]] < 0.5 and u[0] not in FOTO]; fo = [u for u in ue if u[0] in FOTO]
    fotos = {nm: (max(m['summe'] for m in zl), max(m['tief'] for m in zl), max(m['maxtiefe'] for m in zl)) for nm, zl in off if nm in FOTO}
    ech = json.load(open(R + 'laeufe/%s_luecke.json' % n))
    eu = [(m['f'], m['summe'], m['tief'], m['maxtiefe'], round(COS.get('F%d' % m['f'], 9), 2)) for m in ech if not m['ok']]
    ev = [e for e in eu if e[4] >= 0.5 and e[0] not in AUS]; es = [e for e in eu if e[4] < 0.5 and e[0] not in AUS]
    ea = [e for e in eu if e[0] in AUS]
    out[n] = dict(off_vorn=vo, off_seite=se, off_foto=fo, fotos=fotos, echt_vorn=ev, echt_seite=es, echt_aus=ea)
    S = lambda l: sum(x[1] for x in l)
    print('%s OFFLINE ueber %d | vorn %d Summe %d | seite/hinten %d Summe %d %s | Foto ueber %s | Fotoposen %s' % (n, len(ue), len(vo), S(vo), len(se), S(se), se, fo, fotos))
    print('%s ECHT (ohne F316-F333) vorn %d Summe %d | seite/hinten %d Summe %d %s | F316-F333 ueber %d' % (n, len(ev), S(ev), len(es), S(es), es, len(ea)))
    print('     echt vorn:', ev)
json.dump(out, open(R + 'off/split.json', 'w'))
