# SF V4 Originaltexturen Runde 1 - Sicht-Pruefung (Pruefer, 2026-11-07)

Gegenstand: Variante C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/EM30_V4.EMD
(sha256 a9da924fee71348d..., gemessen), Referenz 2 = re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD (sha256 cfd1ac42729e5636..., gemessen, unveraendert).
Zweig r41/sichel_nb2, HEAD 4bf4542e0.

Gelesen: Leisten 01, 02, 03, 04, 05, 07, 08 in C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/,
C:/workspace/git/reAi_v2/Version_80%/Beschreibung1.png und Beschreibung2.png, Dossier-Abschnitt "V4 Originaltexturen" in SF_sichel.md.

Eigene Gegenprobe (unabhaengig von riegel_ref2tex.py): Renderer v4_gemein.zeichne (b2, Kamera 50A0/12, RDT-Licht), Posen posen_v4 "135"
und "85", fuer Ref2 und V4 neu MIT Id-Puffer. Skript und Bilder:
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sicht_ot1/
(idprobe.py, ref2_135_ok.png, neu_135_ok.png, ref2_85_ok.png, neu_85_ok.png, *_ids.npy, koepfe.png, wurzel16.png, neu85_ids.png).

## Ergebnis: NICHT bestanden

Der Koerper ist jetzt wirklich die Original-Birkin. Der Kopf aber ist ein fast schwarzer, violettstichiger Klumpen auf dem rosa Koerper,
und an der Schwanzwurzel sitzt ein rot-weiss gestreifter Block. Beides faellt genau unter den Satz des Nutzers ("Die Farben koennen doch
dann im Prinzip garnicht so scheisse aussehen"). Ich rechne damit, dass er beides wieder ablehnt.

## (a) Koerper = Original-Birkin der Referenz 2 - wie Vorlage/Original

- Bild: In den Leisten 01, 07 (alle vier Takte) und 08 (alle sechs Winkel) ist der Koerper in V4 neu bildgleich zu Referenz 2. Es gibt
  keine Sprenkel, kein Ueberrot und keine weissen oder gelben Flecken mehr; die V4-r5-Fehler sind weg.
- Eigene Messung: Pixel mit gleicher Koerper-Id (0-8, 10-15) in beiden Renders: Pose 135 38790 Pixel, davon 0 abweichend (max 0);
  Pose 85 16175 Pixel, davon 0 abweichend.
- Riegel des Bauers (Leiste 01): T-K1a/b/c OK, 0 von 34716 Koerpertexeln abweichend. Das Spiel-Asset hat sha256 cfd1ac42, unveraendert.
- Widerlegungsversuch gescheitert. Bewertung: **wie Vorlage/Original**.

## (b1) Kopf passt farblich und in der Machart zum Original-Koerper - Abweichung, die der Nutzer wieder ablehnen wuerde

Am Bild (Leisten 01, 03, 04; koepfe.png): Der Kopf ist eine fast schwarze bis dunkelgraue Masse mit dunkler Weinrot-Kappe und einem
blassen Fleck am linken Rand. Daneben liegt der helle, rosa-graue Ref2-Rumpf. Der Kopf liest sich wie eine Kapuze oder ein Bluterguss,
nicht wie dieselbe Haut. Die Ref2-Machart hat feine rosa/rote Wundtupfen auf heller Haut. Davon ist am Kopf nur die Kappe uebrig, die
Gesichtsflaeche ist grossflaechig glatt dunkel.

Zahlen (eigene Renders, Lab je Id-Maske, gleiches Licht):

| | Kopf Id 9: L* P10/P50/P90 | Anteil L* < 12 | Anteil L* < 20 | Anteil b* < 0 (violett) |
|---|---|---|---|---|
| Ref2 Pose 135 | 18,0 / 31,0 / 52,5 | 5 % | 12 % | 0 % |
| V4 neu Pose 135 | 3,3 / 11,4 / 23,5 | **51 %** | **80 %** | **23 %** |
| Rumpf Id 8 (Ref2 = V4) Pose 135 | 9,4 / 26,3 / 42,7 | 14 % | 34 % | 1 % |
| Ref2 Pose 85 | 12,8 / 31,0 / 34,1 | 9 % | 16 % | 0 % |
| V4 neu Pose 85 | 2,7 / 18,0 / 34,1 | 36 % | 52 % | 19 % |

- Gegen den Koerper: Kopf zu Rumpf ist im Mittel L* 13,5 zu 26,0 = 0,52 (Pose 135). Bei Ref2 sind es 33,9 zu 26,4 = 1,28.
- Gegen das Video: Auch dort ist der Kopf dunkler als die Schultern. In Leiste 01/03 gemessen: Kopfkern L* 18,8, Schultern 28,4/23,7,
  Verhaeltnis 0,72. Die Vorlage ist also nicht der Grund fuer einen so dunklen Kopf. V4 ist mit 0,54 (gleiche Messfenster) noch
  dunkler als das Video.
- Violettstich: 23 % der Kopfpixel haben b* < 0. Auf dem Original-Rumpf sind es 1 %, im Video gar keine (Nutzerpunkte Haare
  13,3/7,4/1,5, Nase 18,2/7,1/1,3, Leiste 03). T-K2 ("0 von 14008 Texeln ausserhalb der Ref2-CLUT") stimmt zwar, aber die verwendeten
  Eintraege sind auf dem Original-Koerper Randfarben, hier fuellen sie den halben Kopf. Palettengleich heisst nicht bildgleich.
- Ursache, aus dem Dossier belegt: Die "Zonenhelligkeit wird nach der Vergleichsregel nachgefuehrt" (12 Runden), gegen die
  Messblatt-Ziele K5/K7/K8 mit L* 13-18. Das sind Videowerte. Damit steckt die Video-Farbeichung, die die neue Vorgabe abschaffen
  sollte, ueber die Helligkeit wieder im Kopf. Der Renderer legt Szenenlicht und Schatten ein zweites Mal darauf. Das Dossier nennt
  es selbst unter "Offen 3": "Er ist damit dunkler als Kopf und Rumpf von Referenz 2".
- Ein aufgemaltes rundes Auge oder ein Comic-Maul gibt es nicht; der Augenfleck ist unregelmaessig und blass. Das ist in Ordnung.
- Bewertung: **Abweichung, die der Nutzer wieder ablehnen wuerde.**

## (b2) Schwanz passt farblich und in der Machart zum Original-Koerper

Teil 1, Glieder 17-22, Haut und Adern: **Abweichung klein**
- Bild (Leisten 02, 05, 08): Die Glieder tragen die Oberarmhaut mit roten Adern und passen zum Rumpf. Ich sehe keine einfarbigen Bloecke.
- Eigene Messung Pose 85, L*/a*/b* je Id: Rumpf 8 29,3/12,2/5,9; Glied 18 27,2/9,1/4,0; 20 29,1/10,3/4,9; 22 31,7/9,7/4,0.
- Zwischen Glied 20 und 21 liegt ein sichtbarer Lichtwechsel. Der Bauer hat ihn gemessen (T-K3a 13,0 gegen 13,5) und als
  Gouraud-Wechsel je Glied erklaert.

Teil 2, Wurzel Glied 16: **Abweichung, die der Nutzer wieder ablehnen wuerde**
- Bild: In der Seitenansicht (Leiste 02 und 05, Spalte V4 neu, oben auf dem Buckel) sitzt ein rosa-rot-weiss SENKRECHT GESTREIFTER
  Block, schon in normaler Groesse sichtbar.
- Zuordnung per Id-Puffer: Er gehoert zu Glied 16, der Schwanzwurzel (wurzel16.png, gelbe Umrandung = Id 16; neu85_ids.png).
- Gemessen in den oberen 14 Zeilen von Id 16 (Pose 85, 315 Pixel):
  - mittlerer Helligkeitssprung waagerecht 7,45, senkrecht 4,36; das Verhaeltnis 1,7 bedeutet Streifen;
  - zum Vergleich: Rumpf 8 3,79/4,60, Glied 18 3,86/3,99, Rest von Id 16 4,43/5,40;
  - a* 25,9 gegen 12,2 auf dem Rumpf, also ein gesaettigter roter Streifen.
- Das ist genau der Wurzel-Querstreifen, den das Dossier als durch die Entzerrung behoben meldet. Er ist auf der Oberseite der Wurzel
  stehen geblieben. Vorgabe (2) verlangt, dass die Wurzel ohne Kante in den Ruecken uebergeht und keine Bloecke hat; hier ist ein Block.
- Die Riegel T-K3c (Sprung 5,2) und T-K3b messen gliedweise im Mittel. Ein Streifen auf einer Teilflaeche faellt durch sie durch.

## (c) Orte der Merkmale gegen die Nutzer-Beschriftung

- Auge, vorn: **Abweichung klein.** Der blasse Fleck liegt (B)-links, also beim Wesen rechts der Kopfmitte (W), wie Beschreibung1.
  Nutzerpunkt und Messpunkt liegen in Leiste 03, Zeile 2, an derselben Stelle; Riegel K4 0,052 H links / 0,096 H unter.
- Auge, seitlich: **Abweichung klein.** Es sitzt vorn am Kopf wie in Beschreibung2. Riegel S2 0,046 / 0,817 H. Der Fleck ist dunkler
  als im Video (S1 L* 42,7 gegen 48,4).
- Haare: **Abweichung, die der Nutzer wieder ablehnen wuerde** (Folge von b1). Das dunkle Feld liegt nicht "daneben", es nimmt 80 %
  des Kopfes ein (L* < 20).
- Nase: **Abweichung, die der Nutzer wieder ablehnen wuerde** (Folge von b1). Sie ist im Dunkel nicht als eigenes Merkmal erkennbar.
- Rote Kopfoberseite: **Abweichung klein.** Sie ist vorhanden, vorn als dunkles Weinrot, seitlich als roter Saum.
- Schwanzform, Beginn - Knick - Ende: **Abweichung klein.** Der Verlauf deckt sich mit dem Polygonzug der Vorlage (Leiste 05, Zeile 2;
  T2-T4 OK). Ab dem Knick wird der Schwanz nach oben breiter, wie Beschreibung2. T5 bleibt mit 0,098 gegen 0,100 knapp rot; die
  Geometrie ist unveraendert seit Runde 5.

## Was der Bauer in Runde 2 tun muesste

1. Kopf: die Zonenhelligkeit NICHT an Video-L* (K5/K7/K8) nachfuehren.
   - Grundton = blasse Ref2-Kopf- und Rumpfhaut, wie der Ref2-Kopf (L* P50 um 31 im Render).
   - Dunkel nur im Haarfeld daneben, nicht ueber das ganze Gesicht.
   - Keine Eintraege mit violettem Stich (b* < 0) auf Flaechen.
   - Abnahme: Kopf-zu-Rumpf-Verhaeltnis und Dunkelanteil gegen Ref2, nicht gegen das Video.
2. Schwanzwurzel Glied 16: die gestreifte Oberseite entzerren oder anders belegen, Streifen-Verhaeltnis waagerecht/senkrecht wie am
   Rumpf (etwa 0,8-1,0). Den Riegel um ein Anisotropie-Mass je Teilflaeche ergaenzen.
