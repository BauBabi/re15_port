# Variante K5e_f1 (Spiel-Asset E32 bis 15.11.2026, abgeloest durch K5g3 + R9 "Variante 1")

Gesichert beim Einbau der Riesenform K5g3 mit der rechten Hand "Variante 1" (Krallen-Runde R9, Nutzerentscheid 15.11.2026 am Blatt
`31_riesenform_rechte_hand/rechte_hand_variante1_variante2.png`; Bericht `k5g/K5g3/EINBAU_R9.md`). Dateien byte-gleich mit dem Stand von
Commit 62bd0001b (`re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`) und mit `ruhe/K5e_f1/` (Basis aller K5g-Generatoren).

| Datei | Herkunft | sha256 |
|---|---|---|
| `BAHNSTEIG_FORM.EMD` | Spiel-Asset K5e_f1 (28 Clips, 32 Meshes, 32 Knochen, 293 516 Byte) = `ruhe/K5e_f1/BAHNSTEIG_FORM.EMD` | f277916d7d6518b5fee621dc3b1a50ebc8cf5861ba6cc290818139449da9a5e1 |
| `BAHNSTEIG_FORM.TIM` | = `ruhe/K5e_f1/BAHNSTEIG_FORM.TIM` (10 Zeilen, 136 224 Byte) | 31a35e66428bac1e8783587f2336ea0bec7aab6e34876b0be87a144c576445b8 |

FORM.EMD/.TIM lagen nie in `shared_assets` (nur BAHNSTEIG_FORM.EMD/.TIM, wie beim Einbau 62bd0001b); die kleine FORM.EMD von K5e_f1 liegt in
`ruhe/K5e_f1/FORM.EMD`.

Diese Kopie ist seit dem R9-Einbau der Eingang "Spiel-Asset K5e_f1" fuer `generator/kf_k5g3.py` (Konstante ASSET;
kf_k5g3 prueft die Basis `ruhe/K5e_f1` jetzt per sha256). Sie lasen bis dahin `re15_port/shared_assets/RE2/BAHNSTEIG_FORM`, dort liegt jetzt K5g3 + R9
(EMD 88c49ce1..., TIM c00823b3...).

Hinweis git: `*.EMD`/`*.TIM` sind unter `analysis/befunde_runde41/form/` ignoriert (wie bei `einbau_k5c/`); die beiden Dateien liegen lokal in diesem
Ordner und sind jederzeit byte-gleich aus git holbar: `git show 62bd0001b:re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` (bzw. `.TIM`).
