# SF V4 Originaltexturen Runde 2 - Sicht-Pruefung (Pruefer, 2026-11-07 abends)

Gegenstand: Variante C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/EM30_V4.EMD
(sha256 415a921f436fe8e1..., gemessen; = EM30_V4ot2.EMD). Referenz 2 = re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
(sha256 cfd1ac42729e5636..., gemessen, unveraendert; Pin release/em30_hangar.sha256 gleich). Zweig r41/sichel_nb2, HEAD d8c702aa5.

Gelesen: Leisten 01, 02, 03, 04, 05, 07, 08 in C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/,
C:/workspace/git/reAi_v2/Version_80%/Beschreibung1.png und Beschreibung2.png, Riegel SF_belege/v4/ot2/riegel_ref2tex_V4ot2.md (1 rot von 49,
T5), ctest_v4_ot2.txt (4/4), Dossier-Abschnitt "V4 Originaltexturen - Runde 2" in SF_sichel.md, Messblatt-Markierungen (Abschnitt 5 und
K2/K5/K7/K8/K9/S1/S3/S4 als [ueberholt R2]), Generator ref2tex_v4.py (HAAR_F, HAAR_ERO, haar_vorn) und riegel_ref2tex.py (K5/K6/S3, T-K5).

Eigene Gegenprobe (unabhaengig von riegel_ref2tex.py): Renderer v4_gemein.zeichne (b2, Kamera 50A0/12, RDT-Licht und licht=False =
Texelfarbe), Posen posen_v4 "135" und "85", Ref2 und V4 neu je mit Id-Puffer. Skripte und Bilder:
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sicht_ot2/
(render.py, messen.py -> messung.json, messen2.py -> messung2.json, messen3.py -> messung3.json; *_ok.png farbrichtig, koepfe_135_x6.png,
koepfe_85_x6.png, wurzel_neu_85_x6.png, wurzel_ref2_85_x6.png, schwanz_neu_85_x3.png, k03_ref2_v4_x12.png, r08_v4_p120_x3.png u. a.).
Lab = cv2 RGB2Lab (der Renderer liefert RGB; Runde-1-Werte reproduziert: Ref2 Kopf Pose 135 L* P10/50/90 18,0/31,0/52,5).

## Ergebnis: NICHT bestanden (ein Punkt der dritten Klasse: das Haarfeld)

Der Koerper ist die Original-Birkin, der Kopf traegt jetzt die blasse Ref2-Haut, die Schwanzwurzel ist nicht mehr gestreift, Auge/Nase/
Kuppe/Schwanzform sitzen an den beschrifteten Orten. Uebrig ist EIN Fehler, den der Nutzer nach meiner Einschaetzung wieder ablehnt: das
Haarfeld ist kein dunkles Feld in der Haut, sondern ein fast schwarzer, hartkantiger Block in Form des Messfensters, der von der Seite
(+120 Grad) wie ein Loch im Kopf aussieht. Alles andere ist "wie Vorlage/Original" oder "klein".

## (a) Koerper = Original-Birkin der Referenz 2 - wie Vorlage/Original

- Bild (Leisten 01, 07 alle vier Takte, 08 alle sechs Winkel): V4 neu und Referenz 2 sind am Koerper bildgleich; keine Sprenkel, kein
  Ueberrot, keine weissen/gelben Flecken.
- Eigene Messung (messung.json koerper_135/koerper_85): Pixel gleicher Koerper-Id (0-8, 10-15) in beiden Renders: Pose 135 38790 px,
  0 abweichend (max 0); Pose 85 16175 px, 0 abweichend.
- Riegel T-K1a/b/c OK (0 von 34716 Texeln), G1 bytegleich 15 von 15. Spiel-Asset cfd1ac42 unveraendert.
- Widerlegung gescheitert. Bewertung: **wie Vorlage/Original**.

## (b1) Kopf-Grundton und Machart gegen den Original-Koerper

Teil 1, Grundton (Haut ausserhalb von Haarfeld und Augenfleck): **Abweichung klein**
- Texelfarbe (licht=False, Kopf-Id 9, 15 <= L* <= 45; messung3.json *_kopfhaut_neutral): V4 L* 33,4 / a* 20,7 / b* 8,4 gegen Ref2
  32,9 / 24,1 / 10,8 (Pose 135); Pose 85 34,2 / 19,7 / 7,6 gegen 31,9 / 24,6 / 10,9. Das ist die Ref2-Kopfhaut, etwas weniger rosa.
- Der Runde-1-Fehler ist weg: Anteil b* < 0 im Kopf 0,000 (Runde 1: 23 %); Riegel T-K5a 30,9 / 34,2 (Ref2 30,8), T-K5c 0,000.
- Die Ref2-Kopftextur hat helle Wundstreifen (Anteil L* > 45 in Pose 135: Ref2 44,9 %, V4 9,6 %; P90 57,3 gegen 43,9). V4 ist dort
  glatter als Ref2 (Gradient 3,45 gegen 4,87), bleibt aber eine strukturierte rosa Haut (koepfe_135_x6.png).
- Kinn/Maul: kein dunkles Maul, kein Comic-Maul (K8 Albedo 34,8, dE00 5,7 zu Ref2 KV4). Augenfleck: unregelmaessig, nicht rund
  (Fuellgrad gegen Bbox 127/(14 x 16) = 0,57), blass (54,2 / 6,0 / 2,0). In Ordnung.

Teil 2, Haarfeld: **Abweichung, die der Nutzer wieder ablehnen wuerde**
- Bild: Leiste 03 Spalte 4 und k03_ref2_v4_x12.png: ein fast schwarzes Trapez mit gerader linker Kante und gerader Oberkante,
  etwa halbe Kopfbreite, auf der blassen Haut. Leiste 01 (Spielkamera): rosa Kopf, weisser Fleck, schwarzes Viereck. Leiste 08
  (+120 Grad, r08_v4_p120_x3.png): ein schwarzes Oval mit rosa Rand, das wie eine leere Augenhoehle aussieht; Ref2 zeigt dort einen
  rosa Kopf. Leiste 07 h135 (Spiel): schwarzer Fleck neben weissem Fleck.
- Zahlen, Texelfarbe Pose 135 (messung3.json neu_135_haarfeld): groesste zusammenhaengende Flaeche L* < 15 im Kopf = 675 px =
  24,8 % der Kopfpixel; Bbox 25 x 35 px bei einer Kopfbox von 47 x 56 px (K10 0,154 x 0,187 H, H 302) = 53 % der Breite, 63 % der
  Hoehe; Fuellgrad 0,77 (Block); linker/rechter Rand Standardabweichung der x-Lage 2,1 / 2,4 px ueber 35 Zeilen (gerade Kanten);
  L* innen 8,9 (Std 4,0), Ring aussen 30,0 -> Sprung 21 L*; Randgradient 10,4 gegen 3,5 im Kopfmittel.
- Leiste 08 (Texelfarbe, Kopfzone des Tiles, messung2.json leiste08_*): groesste Flaeche L* < 12 bei +120 Grad V4 288 px gegen
  Ref2 35 px; bei +180 Grad 238 gegen 86 px. Bei 0 und +60 Grad sind beide gleich (138/139, 90/101) - der Fleck ist das Haarfeld.
- Gegen Ref2 (dieselbe Messung, licht=False): Ref2-Kopf hat 0 Texel mit L* < 20 (beide Posen). Auf Ref2-HAUT (Rumpf Id 8) ist die
  groesste Fast-Schwarz-Flaeche 149 px, 11 x 36 px, Fuellgrad 0,38, Randgradient 6,0 - schmale Adern/Schattenrillen, keine Bloecke.
  Die grossen dunklen Flaechen des Ref2-Koerpers (3094 px, Ids 0/1/4/11/12; 628 px Id 14) sind Hose und Schuh. Das V4-Haarfeld ist
  4,5-mal groesser als jede dunkle Hautstelle der Referenz, doppelt so kompakt und hat einen 1,7-mal haerteren Rand. Das ist nicht die
  Machart der Original-Textur, sondern ein aufgemalter Block ("Klecks" im Sinn der Vorgabe (3)).
- Gegen das Video (Leiste 03, Kopfbox+8 x4, Fenster KV2; messung2.json leiste03_*): Video KV2 L* 12,9 bei Kopfbox-Median 17,3 ->
  das Haar ist im Video um 25 % dunkler als sein Kopf. V4: KV2 11,4 bei Kopfbox-Median 28,6 -> 60 % dunkler. Der Nutzer beschreibt
  ein "dunkles Feld daneben", nicht ein schwarzes Loch; die Dunkelheit des Videos auf die blasse Ref2-Haut uebertragen ergibt einen
  Kontrast, den weder Video noch Ref2 haben.
- Ursache, aus Code und Dossier belegt:
  1. Form = Messfenster: ref2tex_v4.py HAAR_ERO = 0 "das ganze Fenster (K5 KV2 L* <= 22,8 braucht es)"; haar_vorn = Texel, die Pose 135
     im Fenster KV2 und in der K6-Box zeigt. Das Fenster KV2 ist im Messblatt ein Mess-Ort fuer den Mittelwert, nicht die Form der
     Haare; mit HAAR_SIGMA 1,5 / HAAR_STRUKTUR 0,8 bleiben die Fensterkanten gerade (gemessen oben).
  2. Dunkelheit = Videowert: HAAR_F = 0,5 x haar_l (16,1 = Median der dunklen Ref2-Eintraege) -> Texel L* 8-9, Kommentar "mit 1,0:
     K6 L* 18,4-20,8 > 18". Der Riegel K6 verlangt "L* <= 18 (Original 12,9)" - die 18 ist aus dem Video-Messwert abgeleitet und im
     Messblatt NICHT als ueberholt markiert (nur K5/K7/K8). Die Halbierung gegenueber den dunkelsten Ref2-Eintraegen fuehrt also die
     Video-Dunkelheit durch die Hintertuer K6 nach - genau das, was Orchestrator (a) ausschliesst.
  3. Folge im ganzen Kopf (Pose 135, Licht, messung.json kopf_neu_135): L* P50 23,9, Kopf/Rumpf 0,86, Anteil L* < 12 34 %, L* < 20
     46 % (Ref2 31,0 / 1,28 / 5,5 % / 11,8 %). Der Riegel T-K5 blendet das Haarfeld aus ("ausserhalb des Haarfelds") und fuehrt den
     ganzen Kopf nur als INFO; die Orchestrator-Vorgabe (a) nennt "Kopf-Render L* P50 um 31, Kopf/Rumpf um 1,28" fuer den Kopf.
- Riegel, die es haetten fangen muessen: T-K5d misst "dunkle Textur ausserhalb des Haarfelds" und ist per Definition blind fuer das
  Haarfeld selbst; es gibt kein Mass fuer Groesse, Kantenhaerte oder Fuellgrad des Haarfelds gegen Ref2-Haut.

## (b2) Schwanz gegen den Original-Koerper

Teil 1, Wurzel Glied 16: **Abweichung klein**
- Bild (Leiste 02/05, wurzel_neu_85_x6.png): der rosa-rot-weiss gestreifte Block der Runde 1 ist weg. Oben auf der Wurzel liegt ein
  glattes, helles, laengs verschmiertes Band.
- Zahlen Pose 85, obere 14 Zeilen von Id 16 (315 px): Helligkeitssprung waagerecht/senkrecht 1,96 / 1,51 = 1,30 (Runde 1: 7,45 /
  4,36 = 1,7); Rumpf Id 8 4,32 / 4,76 = 0,91, Oberarm 13 0,97. a* 17,6 (Runde 1 25,9; Rumpf 11,6). Das Verhaeltnis liegt ueber dem
  Richtwert 0,8-1,0, aber die Spruenge sind so klein, dass keine Streifen mehr sichtbar sind.
- Dafuer ist das Band flacher und heller als die Haut daneben: Std L* 4,35 gegen Rumpf 10,6, Glied 18 8,6, Oberarm 10,0 (jeweils obere
  14 Zeilen, Texelfarbe); L* 42,8 gegen 31,5-32,3. Es liest sich als ruhige, helle Kuppe auf dem Buckel, nicht als Fremdkoerper.
  Kante zum Ruecken: dE76 5,1 (Ids 8|16, je 2-px-Band), keine sichtbare Kante.

Teil 2, Glieder 17-22: **Abweichung klein** (unveraendert seit Runde 1, Vorgabe (c))
- L*/a*/b* je Glied 21-32 / 9-13 / 4-6 gegen Rumpf 29 / 12 / 6 (Pose 85). Spruenge an den Gelenken dE76 1,7-6,5 (17|18 dL -6,5,
  20|21 +6,1 = Gouraud je Glied). Anisotropie Glied 20 1,46 und 22 1,35 auf 276 / 206 px; am Bild (schwanz_neu_85_x3.png) Oberarmhaut
  mit Adern, keine Quernaehte, keine einfarbigen Bloecke. T-K3a/b OK.

## (c) Orte der Merkmale gegen die Nutzer-Beschriftung

- Auge vorn: **Abweichung klein.** Heller Fleck (B)-links = beim Wesen rechts der Kopfmitte wie Beschreibung1; K4 0,054 H links /
  0,095 H unter OK, Leiste 03 Zeile 2 Nutzerpunkt und Messpunkt an derselben Stelle. Blass-grau statt rosa-weiss (a* 6 gegen 14,7 der
  hellen Ref2-Texel), unregelmaessig, nicht rund.
- Auge seitlich: **Abweichung klein.** Vorn am Kopf wie Beschreibung2, S2 0,050 / 0,818 H, 1 Komponente.
- Haare: Ort stimmt (dunkles Feld (B)-rechts neben dem Auge, auch seitlich S3 Abstand 0,000 H) - Ausfuehrung siehe (b1) Teil 2:
  **Abweichung, die der Nutzer wieder ablehnen wuerde.**
- Nase: **Abweichung klein.** Im Fenster KV3 L* 21,7 zwischen Haar 12,9 und Auge 54,5; am Bild ein kaum lesbarer grauer Schimmer
  am unteren Rand des schwarzen Blocks (k03_ref2_v4_x12.png). Lage richtig, Lesbarkeit leidet unter (b1).
- Rote Kopfoberseite: **Abweichung klein.** Vorn 38,0 / 22,5 / 7,7 (Ref2 41,8 / 18,2 / 8,8), seitlich 36,8 / 16,0 / 3,9; am Bild
  (Leiste 04, 07 h85/h100) eine glatte lachsrote Kuppe, etwas flaecher als die Ref2-Kuppe, aber dieselbe Farbe.
- Schwanzform Beginn - Knick - Ende: **Abweichung klein.** Geometrie unveraendert seit Runde 5; T1-T4, TA, T6, TK OK; T5 0,098 H gegen
  0,100 (T5q quer OK). Beginn im Ruecken, Knick, Verbreiterung nach oben wie Beschreibung2 (Leiste 05 Zeile 2).

## Was in Runde 1 gut war und geblieben ist (Vorgabe (c))
Koerper bytegleich (0 px), Auge-/Nase-Lage, rote Kopfoberseite, Glieder 17-22 ab Texelzeile 56 texelgleich (Riegel T-K3d INFO),
Schwanzform. Zusaetzlich behoben: Kopf-Grundton (Runde 1 L* P50 11,4 -> Haut 33,4), Violettstich (23 % -> 0), Wurzelstreifen
(7,45/4,36 -> 1,96/1,51).

## Was der Bauer fuer eine Abnahme aendern muesste (ein Punkt)
1. Haarfeld in der Machart der Ref2-Haut:
   - Dunkelheit aus der Ref2-Palette, nicht halbiert: HAAR_F / HAAR_F_V = 1,0 (haar_l 16,1 = Median der dunklen Ref2-Eintraege) oder
     relativ zum Kopf wie im Video (Haar um 25 % dunkler als der Kopf-Median -> auf Ref2-Haut L* um 23; das erfuellt K5 <= 22,8 knapp
     und S3 <= 22,8).
   - K6 "L* <= 18 (Original 12,9)" ist ein Video-Wert und wie K5/K7/K8 als ueberholt zu markieren (Ersatz gegen Ref2: dunkler als die
     Kopfhaut, z. B. <= Ref2-Kopf P50 - 8 wie K5).
   - Form nicht = Fenster KV2 + K6-Box: Haarmaske aus dem dunklen Bereich der Vorlage (Leiste 03 Spalte 1, L* < 15 = 71 % des Fensters,
     aber mit weichem Rand) oder erodiert (HAAR_ERO >= 2) mit Rand, der der Ref2-Kopfstruktur folgt; Ziel: Randgradient <= Ref2-Haut
     (um 6), Sprung Innen/Ring <= 12 L*, Fuellgrad < 0,6, gerade Kanten (Std x < 2 px) weg.
   - Riegel ergaenzen: Groesse, Fuellgrad, Randgradient und Innen/Ring-Sprung des Haarfelds gegen die groesste dunkle Hautstelle von
     Ref2 (Rumpf Id 8: 149 px, Fuellgrad 0,38, Gradient 6,0); ganzer Kopf Pose 135 L* P50 >= Ref2 - 8 und Kopf/Rumpf >= 1,0 als
     Pflicht statt INFO.
2. Alles andere bleibt (Koerper, Grundton, Auge, Kuppe, Nase, Wurzel, Glieder, Form).
