"""Material je Flaeche aus ruhe/messung/material.json (Teilmengen-Materialien zuletzt, damit das eigentliche Material gewinnt)."""
import json

MAT = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/messung/material.json'


def material_je_flaeche():
    m = json.load(open(MAT, encoding='utf-8'))['material']
    out = {}
    for name in ('Haut', 'Gelenk', 'Fleisch_Schwanzspitze', 'Fleisch_Buckel', 'Zahn', 'Auge', 'Maulinneres', 'F1'):
        for f in m.get(name, {}).get('liste', []):
            out.setdefault(f, name)
    return out
