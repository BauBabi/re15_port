# Prüfung K5g2, Runde 1: Zahlen (2026-11-09, ca. 22:15)

**Ergebnis: NICHT BESTANDEN.** Es gibt noch nichts, was man abnehmen könnte. Die Variante ist nicht fertig gebaut, und es gibt keinen Commit.

## Was vorliegt (selbst nachgesehen)

| Punkt | Soll | Ist (gemessen) | Bewertung |
|---|---|---|---|
| Commit auf r41/form | Variante K5g2 ist committet | HEAD = a353ec98b (Tafeln der Grundlage v2, Runde 4); kein K5g2-Commit | ROT |
| Abgabe-Ordner k5g/K5g2/ | BAHNSTEIG_FORM.EMD/.TIM, FORM.EMD/.TIM, bau.json, tafeln/ | nur k5g2_par.json, traeger_texel.json, spiel/ (3 Bilder vom heutigen Stand: heute_pose1_r5080.png, heute_vorn_r5080.png, heute_pose2_r5090_s11.png); tafeln/ ist leer | ROT |
| Spiel-Asset unverändert | K5e_f1, wie in Commit 62bd0001b | git status sauber; EMD sha256 f277916d…a5e1, TIM sha256 31a35e66…45b8 | GRÜN |
| Entwicklungsbau | – | scratchpad/k5g2/T_k5g2 (22:05): EMD 4f0e3e15…, TIM 7d5bed31…; bau.json liegt dort NICHT (der Bauer behauptet es) | nur Zwischenstand |
| Helligkeits-Bisektion | abgeschlossen | f_bis.log: erst 2 Schritte (f 1,8 -> 20,637; f 1,2 -> 16,902; Vorlage 15,370); etwa 10 python-Prozesse laufen seit 22:01 | läuft noch |

## Punkte E1 bis V6

- **E1 (Streifen):** Der Bauer meldet selbst ROT. Laut seinen eigenen Zahlen bleiben Keil-Pixel in allen Ansichten übrig. Clip 10 und 12 lassen sich mit Weg (a) nicht lösen. Weg (b) ist weder begründet noch gebaut. Es gibt kein Asset, darum habe ich keine eigene Keil-Messung gemacht. **ROT.**
- **E2 (Krallen), E3/V3 (Zonen, eigene Texel), V1/V2 (Zielband, Ruhe, Hautton), V4 (nichts schwarz, nichts rot am Hals), V5 (Krallen), V6 (F1):** Es gibt kein abgegebenes Asset, also kann ich nichts prüfen. **ROT (nicht prüfbar).**
- **Zweiter Generatorlauf byte-gleich, ctest mit RE15_RE2_ASSET_ROOT:** Kein Generator ist fertig, also auch nicht prüfbar. Der Bauer meldet, dass ctest nicht gelaufen ist. **ROT.**

## Nebenbefund am Entwicklungsbau (nur Hinweis, keine Abnahme)

Ich habe den TIM-Kopf von T_k5g2/BAHNSTEIG_FORM.TIM mit eigenem Code gelesen:
- CLUT 256 x 16: Das sind genau 16 Zeilen, also an der Grenze von höchstens 16.
- **847 CLUT-Einträge sind reinweiß (0x7FFF).** Im Spiel-Asset K5e_f1 sind es nur 3.
- 16 Einträge sind 0x8000 (K5e_f1: 208).
- Das Bild ist 256 x 256.

Ob Flächen diese weißen Einträge wirklich benutzen, habe ich nicht gezählt, weil der Bau nur ein Zwischenstand ist. Bei der Abgabe muss das gezählt werden. V5 verlangt Krallen ohne reinweiße Texel, und K5f Runde 1 ist genau daran gescheitert.

## Was für Runde 2 nötig ist

1. Ein vollständiger Bau unter k5g/K5g2/ mit EMD, TIM, bau.json und den Tafeln.
2. Ein Commit auf r41/form.
3. Ein zweiter Generatorlauf, der byte-gleich ist.
4. Ein ctest-Lauf mit der Umleitung.
5. Für E1 eine Entscheidung zu Clip 10/12: entweder Weg (b) mit Begründung oder den Rest zeigen und begründen, wie es E1 verlangt. Das ist ein Grenzfall, den der Orchestrator entscheiden muss.
