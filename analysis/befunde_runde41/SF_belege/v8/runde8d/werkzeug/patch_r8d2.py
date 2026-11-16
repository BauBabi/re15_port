"""Patch Runde 8d Teil 2 (nur falls die Flanken-Messung x22_fugen.py Wechsel an den Fugen zeigt): ein Grau-Feld fuer alle Glieder
(schwanz_r8.json felder.grau_alle), laengs je Glied gespiegelt wie der Bauch -> an jedem Gelenk dieselbe Feldzeile auf beiden Seiten."""
import os, json
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
p = os.path.join(W, 'schwanz_r8.py')
s = open(p, encoding='utf-8').read()


def ers(alt, neu):
    global s
    assert s.count(alt) == 1, alt[:60]
    s = s.replace(alt, neu)


ers("""    feld_grau = felder['grau'][str(16 + g)]""",
    """    grau_alle = felder.get('grau_alle')                    # Runde 8d: ein Grau-Feld fuer alle Glieder, laengs je Glied gespiegelt (sonst je Glied)
    feld_grau = grau_alle or felder['grau'][str(16 + g)]""")
ers("""            if kl == 'grau':
                s_um = float(np.clip((th - tb) / max(360.0 - 2 * tb, 1.0), 0.0, 1.0))
                uv_e.append(_uv_feld(feld_grau, s_um, s_lang))""",
    """            if kl == 'grau':
                s_um = float(np.clip((th - tb) / max(360.0 - 2 * tb, 1.0), 0.0, 1.0))
                uv_e.append(_uv_feld(feld_grau, s_um, (s_lang if g % 2 == 0 else 1.0 - s_lang) if grau_alle else s_lang))""")
open(p, 'w', encoding='utf-8').write(s)
pj = os.path.join(W, 'schwanz_r8.json')
j = json.load(open(pj))
j['felder']['grau_alle'] = j['felder']['grau']['16']
j['quelle'] = j['quelle'] + ('; Runde 8d: grau_alle = Grau-Feld von Glied 16 (u 64-87 v 76-131, Oberseiten-Texel des Eingangs) fuer alle Glieder, '
                             'laengs je Glied gespiegelt (Messung der Flanke an den Fugen SF_belege/v8/runde8d/)')
json.dump(j, open(pj, 'w'), indent=1)
print('ok')
