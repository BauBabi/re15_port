# parity_net — gelernter Frame-Matcher für re15-parity-verify

Kleines PyTorch-CNN-Embedding (SupCon-kontrastiv, GPU-trainiert), das Port-Frames und
PSX-Original-Frames **inhaltsbasiert** matcht — timing-unabhängig. Es adressiert die
DuckStation-Variable-fps-Falle (SKILL.md §4): PSX-Captures und die deterministische
30-fps-Port-Serie können NICHT über die Zeitachse aligniert werden, also braucht der
Screen-by-Screen-Vergleich einen Match über den Bildinhalt, robust gegen den
Renderer-Gap (RGB555 + Bayer-Dither + MDEC vs. sauberer 24-bit-Software-Render).

**⚠️ RE-Disziplin:** ein hoher Cosine-Score ist „sieht richtig aus"-Evidenz und damit
KEIN byte-true Beleg (CLAUDE.md). Das Tool **aligniert und triagiert** Frames;
geflaggte Divergenzen müssen weiterhin über Disassembly/RAM belegt werden.

## Quickstart

```bash
cd .claude/skills/re15-parity-verify/parity_net
python parity_net.py build-data     # data/{port,psx}/ aus shots/ + stage_saves/
python parity_net.py train          # ~2 min auf GPU -> parity_net.pt
python parity_net.py eval           # Selbsttest
python parity_net.py match --queries "psx_frames_dir_oder_glob" \
                           --gallery "shots/series_f*.bmp" --sheet --name myrun
# -> out/myrun.csv + out/myrun_sheet.png (Side-by-Side, Score, Verdict)
```

Verdicts: `cos >= 0.70` → `MATCH` (gleicher Spielzustand auf beiden Seiten gefunden),
sonst `CHECK-DIVERGENCE` (im Gallery-Set existiert kein Gegenstück — Kandidat für einen
Port-Gap oder ein anderes Kamera-/Ereignis-Verhalten). `--align` erzwingt eine monotone
Serie-zu-Serie-Zuordnung (DTW), wenn Queries UND Gallery zeitlich geordnete Läufe sind.

## Warum SupCon-Gruppen (die zentrale Design-Erkenntnis)

Reines Instance-Discrimination (SimCLR/NT-Xent) **scheitert** hier messbar
(Cross-Domain-Match ~0 %, alle cos < 0.55): das echte Paar (PSX-Spawn-Frame ↔
Port-Spawn-Frame) sind zwei verschiedene Instanzen im Batch und werden vom Loss aktiv
AUSEINANDERgedrückt — die synthetische PSX-ifizierung (RGB555-Quantisierung +
4×4-Bayer-Dither + Pixelation + Noise) allein überbrückt das nicht.

Fix: `GROUPS` in `parity_net.py` — inhaltsverifizierte Cross-Domain-Paare als
Supervised-Contrastive-Positives (Khosla et al. 2020). Quellen der Paare:
- Intro-Montage: `contact_ORIGINAL.png` (oNN, PSX) ↔ `port_full_sequence.png`
  (Frame NN·100, Port) — per Slide-Text/Inhalt gepaart.
- ROOM1140-Spawn: VRAM-Framebuffer-Crops (x=440, Double-Buffer y=0/240, vermessen an
  `mzd_stage1_briefing.png`) ↔ `series_f00100+`.

## Ergebnisse (2026-07-03, Seed 1337, 134 Frames, 1500 Epochen)

- Augmented-Retrieval (PSX-ifizierte Query → Gallery, top-1, Gruppe/±1-Nachbar): **89.3 %**
- Cross-Domain-Anker (PSX → Port-only-Gallery, gleiche Gruppe): **19/19**
- Ungepaarte Wake-Sheet-Tiles (Generalisierung): Door-Cam-Tiles → `series_f00100/110`
  mit cos 0.86–0.97; Desk-Cam-/Death-Cam-/YOU-DIED-Tiles → cos ≤ 0.40 geflaggt —
  KORREKT, denn die Port-Serie enthält diese Szenen nicht (Game-Over/Death-Kamera ist
  ein bekannter offener Port-Gap).

## Erweitern

Neue verifizierte Paar-Captures (z. B. weitere Räume via re15-room-capture + Port-
AUTOSHOT) → Dateien in `data/` legen bzw. `cmd_build_data` ergänzen, Paare als neue
`GROUPS`-Zeile eintragen, `train` neu laufen lassen. Score-Lücke prüfen: Matches
sollten ≥0.85, Nicht-Matches ≤0.5 liegen, sonst Threshold (`--threshold`) anpassen.
