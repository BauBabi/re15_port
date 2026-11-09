# Variante K5c (Spiel-Asset E30 bis E32, abgeloest durch K5e_f1 am 2026-11-09)

Gesichert beim Einbau der Riesenform K5e ruhige Haut mit F1-Hautrand (Nutzerentscheid E32, `form/NUTZER_ENTSCHEIDE.md`). Dateien byte-gleich
mit dem Stand von Commit a005b357a (`re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`, Einbau E30, FARBE.md Abschnitt "Einbau v0.8.30").

| Datei | Herkunft | sha256 |
|---|---|---|
| `BAHNSTEIG_FORM.EMD` | Spiel-Asset K5c (28 Clips, 32 Meshes, 32 Knochen, 293 516 Byte) = `stufe3/asset` seit E30 | 1ab56076b20ef022c45a4517d944c0829ff93120124b6c7bb3601c7373736041 |
| `BAHNSTEIG_FORM.TIM` | K5c V3 Runde 5, 512 x 256, 16 CLUT-Zeilen (139 296 Byte) | 7187ff097f42427dc40fc77264f43cc5860f6b28de70fb1549c626ef92af75e3 |
| `FORM.EMD` | = `stufe3/basis_k5c/FORM.EMD` (3 Posen-Clips, Rig-Basis der Kette) | a2e87fb84e73c7d7bd173ea11d56482df731175c1e12af26cebd48063eb09adf |
| `FORM.TIM` | = `stufe3/basis_k5c/FORM.TIM` (byte-gleich dem Asset-TIM) | 7187ff097f42427dc40fc77264f43cc5860f6b28de70fb1549c626ef92af75e3 |

Diese Kopie ist seit E32 der Eingang "Spiel-Asset K5c" fuer `generator/kf_k5e.py` (EDD/EMR/Morph/28 Clips; nur MD1 wird ersetzt) und fuer die
Riegel `riegel_k5e.py` (H1/T4) und `riegel_k5d.py` (G0 Asset): Sie lasen bis E32 `re15_port/shared_assets/RE2/BAHNSTEIG_FORM`, dort liegt jetzt K5e_f1.
Nachbau: `python stufe3/gen/s3_clips.py --aus <dir>` mit Rig-Basis `stufe3/basis_k5c` (Default von s3_rig.py).
