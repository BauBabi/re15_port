# Einbau K5e (E32), Sicht-Pruefung Runde 1

Pruefer: Sicht (Auge des Nutzers), 2026-11-09. Gegenstand: Spiel-Asset K5e_f1 in `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`
(Worktree r41_form, Stand 2b9a57ec1). Massstab: Vorlage 19 (`C:/workspace/git/reAi_v2_entscheidung_2026-11-06/19_riesenform_k5e_ruhige_haut`,
Bilder 01-07), Nutzer E32: "ansonsten passt, baue das ruhig einfach mal alles so ein", einzige Beanstandung: schwarzer Ring um den roten Fleck am Becken.

**Ergebnis: BESTANDEN.** Keine Abweichung, die der Nutzer ablehnen wuerde. Drei kleine Abweichungen (nur bei 3-facher Aufhellung sichtbar).

## Eigene Kacheln und Laeufe (alle roh, Ordner C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5e_einbau/sicht/)

- Eigener gdigrab-Lauf `lauf.sh` / `lauf.log`: re15_port/tools/form_spiel.sh, eigene exe-Kopien, Asset OHNE RE15_RE2_ASSET_ROOT (re2-Ordner im
  Laufverzeichnis leer), RE15_TIM_LOG=1, kein AUTOSHOT/SOFTWARE_RENDER. 8 Laeufe, alle rc=0, "Verfahren: gdigrab (Versuch 1)",
  "[enemy] Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips", 0 Zeilen FAILED/not loaded in jedem debug.log:
  p_5080_bild1 (Gier 756 Clip 0 Bild 11), p_5080_g2400 (Kopf/Krallen vorn), p_5090_bild2 (Clip 12), p_5090_g2850/g2500/g3250/g1800 (Clip 2),
  p_5090_anim10_1..3 (Clip 10 abgespielt).
- Weisses Licht: `weiss.py` -> `weiss_k5e_vs_f1.png` (rl.weiss_id, Clip 2, 7 Ansichten, oben K5e, unten K5e_f1), Nahkacheln `weiss_f1_nah.png`.
- Nahkacheln Spielfenster: `crop_5080_f1.png` und `crop_5080_f1_x3hell.png` (K5c | K5e | Asset, x4), `c_5090_f1_h.png` (Gier 2500 | 2850, x4, x2,5 hell),
  `crop_bauer_seite.png` (Bauer-Fensterbilder Seite K5c | K5e | Asset), `crop_5080_kopf_hell.png`, `crop_rs_hals.png`.

## Belege, dass im Spiel genau K5e mit nur geaenderter F1-Zeile laeuft

- sha256 Asset = `ruhe/K5e_f1`: EMD f277916d... (= K5e, EMD unveraendert), TIM 31a35e66... K5c-Sicherung `einbau_k5c` 1ab56076.../7187ff09...
- TIM K5e gegen K5e_f1 (eigener Parser C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/timdiff.py): Pixel bytegleich, 10 CLUT-Zeilen, einzig Zeile 8 verschieden, 24 Eintraege
  (1-6, 15, 17, 19, 67, 75, 78, 84, 86, 190, 191, 194, 198-200, 232, 245, 246, 255). bau.json flaechen_je_zeile: Zeile 8 = 6 Flaechen (nur F1).
- RE15_TIM_LOG in allen 8 eigenen Laeufen: "[tim] slot 11/12: 512x2560 8bpp cluts=10" (K5c haette 16 Zeilen / 512x4096).
- Mein Fensterbild 5080 Bild-1-Lage gegen das Fensterbild der abgenommenen Vorlage (`ruhe/spiel/k5e_pose1_nachstellung_r5080.png`, Quelle von Bild 04):
  **1526 Pixel verschieden, alle in EINEM Fleck y143-217 x307-365 (F1)**, der Rest des Bildes bytegleich. Gegen das Bauer-Bild
  `einbau_spiel/f1_pose1_nachstellung_r5080.png`: 0 Pixel verschieden. Gegen K5c (`heute_pose1...`): 34768 Pixel verschieden (ganze Figur).
- Bauer-Framedumps Seite (`seite_dump_k5e.ppm` / `seite_dump_f1.ppm`): 1349 verschiedene Pixel, eine Komponente y192-261 x224-286 (F1).
- Weisses Licht K5e gegen K5e_f1, verschiedene Pixel je Ansicht: 321 / 0 / 0 / 0 / 18 / 534 / 297, jeweils nur am Becken.

## Befunde je Punkt

| Punkt | Soll (abgenommen) | Ist | Bewertung |
|---|---|---|---|
| Form wie Bild 04 | ruhige grau-mauve Haut, im 5080-Fenster dunkel schiefergrau | 5080 bytegleich zur Vorlage ausser F1-Fleck (1526 px); 5090 Bild-2/Ruhe/Seite wie Vorlage | wie abgenommen |
| Helle Gelenke, fleischroetliche Schwanzspitze | wie Bild 03/06/07 | weiss_k5e_vs_f1.png: Gelenke heller, Schwanzspitze rosa, in beiden Zeilen gleich (0 px Unterschied ausser Becken) | wie abgenommen |
| Lippe/Unterkiefer grau, Krallen in Handfarbe | wie Bild 05 | p_5080_g2400: Lippe grau, Krallen dunkel wie Hand; feine rote Linie am Maulspalt wie in Vorlage 05 "K5e Clip 2 (Ruhe) 0/8" | wie abgenommen |
| Schwarzer Ring um F1 weg | kein schwarzer Keil um den roten Kern | weg in allen Ansichten: Spielfenster 5080 (crop_5080_f1), 5090 Gier 2500/2850, weisses Licht 2 Ansichten. Eigene Messung 5080 Fenster (OpenCV-L*, Feld = 1526 geaenderte px gegen 4-px-Umkreis auf der Figur): K5e -5,72 -> Asset -1,25 | wie abgenommen (Korrektur erfuellt) |
| Roter Kern noch da | rosa Kern wie Vorlage | Kern-Pixel unveraendert (nicht unter den geaenderten Pixeln), in allen Kacheln rosa sichtbar | wie abgenommen |
| Feld leicht dunkler / Polygonkante | - | Feld im Fenster 1,25 L* (eigene Messung) bzw. 2,6-2,8 L* (Bauer EB.6) dunkler als der Umkreis; Kante nur bei x3-Aufhellung andeutungsweise sichtbar | Abweichung klein |
| Dunkler Saum direkt am Kern | - | feiner dunkler Saum am Kernrand (28 Fleisch-Randtexel bleiben K5c, FARBE.md EB.7 Pkt. 5); war auch in K5c/K5e da | Abweichung klein |
| Helle Texel an der Kernspitze | - | 2-3 blassrosa Pixel am oberen Kernende (Rand-Eintraege 78/245 L* 59,9/63,9, EB.7 Pkt. 4); nur bei Aufhellung erkennbar | Abweichung klein |
| Neue Artefakte | kein Mosaik, keine falsche Zeile, keine schwarzen/fehlenden Flaechen, keine Loecher an den Krallen | keine gefunden; Aenderung beschraenkt auf 6 F1-Flaechen (bau.json), EMD bytegleich K5e; Krallen vorn (g2400, Bild 2) geschlossen | wie abgenommen |
| K5c nirgends mehr | - | alle eigenen Laeufe cluts=10; einzige Fundstelle alter Hashes ist ein Kommentar test_birkin_bahnsteig.c:116; shared_assets/RE2 hat nur BAHNSTEIG_FORM.* | wie abgenommen |
| Griff/Rueckstoss | wirkt wie vorher | Bauer-Bilder griff_asset_bild_041, rs_asset_bild_073: Form texturiert, grau, Szene laeuft (szenen_lauf2.log rc=0 alle 4); rote Partie unter dem Kopf im Rueckstoss liegt nicht in Zeile 8 und ist damit K5e-Bestand | wie abgenommen |
| Animation | - | p_5090_anim10_1..3 (Clip 10 laeuft, t751 bild 11): keine Flackerflaechen | wie abgenommen |

## Widerlegungsversuche ohne Treffer

1. Schwarze Ringreste aus anderen Blickwinkeln (Gier 1800/2500/2850/3250, weisses Licht 0-300 Grad): keine.
2. Wirkt die neue Zeile auf andere Flaechen: Nein, Zeile 8 = 6 Flaechen; Pixeldiffs in Spiel und Offline nur am Becken.
3. Laedt das Spiel doch eine Umleitung oder K5c: Nein, re2-Ordner leer, cluts=10, Fensterbild 0 px gegen Bauer, 34768 px gegen K5c.
