"""Baut aus einer args-Datei einen Workflow-Wrapper, der r41-runde.js mit diesen args als Kind-Workflow startet.

Zweck: die args (Stand-Texte, Bau-/Abnahme-Ergebnisse, ~40 KB) muessen nicht von Hand in den Workflow-Aufruf kopiert
werden. Aufruf: python -I mk_lauf.py <args.json> <wrapper.js> [name]
Danach: Workflow({scriptPath: "<wrapper.js>"}). Der Kind-Lauf teilt Semaphor, Agentenzaehler und Abbruch des Wrappers.
"""
import json
import sys

ORCH = 'C:/workspace/git/reAi_v2/analysis/befunde_runde41/orchestrator/r41-runde.js'

quelle, ziel = sys.argv[1], sys.argv[2]
name = sys.argv[3] if len(sys.argv) > 3 else 'r41-rest'
args = json.load(open(quelle, encoding='utf-8'))
welle = ' '.join(args.get('welle', []))
meta = {
    'name': name,
    'description': 'Runde 41 ' + name + ': Spuren ' + welle + ' (r41-runde.js als Kind-Lauf, hoechstens '
                   + str(args.get('slots', 4)) + ' gleichzeitig)',
    'phases': [{'title': 'Ermittlung+Bau'}, {'title': 'Abnahme'}, {'title': 'Nachbesserung'}],
}
js = ('export const meta = ' + json.dumps(meta, ensure_ascii=False, indent=2) + '\n\n'
      + 'const ARGS = ' + json.dumps(args, ensure_ascii=False) + '\n\n'
      + 'return await workflow({ scriptPath: ' + json.dumps(ORCH) + ' }, ARGS)\n')
open(ziel, 'w', encoding='utf-8').write(js)
print(ziel, len(js), 'Bytes, welle', welle)
