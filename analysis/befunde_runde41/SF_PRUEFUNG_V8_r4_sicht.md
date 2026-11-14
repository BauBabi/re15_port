# SF V8 Runde 4: Pruefung durch den Sicht-Pruefer (2026-11-09)

Gepruefter Stand: Zweig r41/sichel_v8, HEAD 282e4d5946be519e5c1e7e2956d4db71b1105663, Spiel-Asset EM30_HANGAR.EMD
sha256 10d021738731e7be2bf6ac79de7da77637cc16f6c08f640073dd4d1242d1843f (nachgerechnet, gleich dem Dossier). Vergleich: V7 319bbd17
(sfv8/emd/EM30_V7.EMD), Original-EM30 (CDEMD0.EMS Typ 0x30 ueber birkin_formen.lade_em30), freigegebene Szene sfv4/r/V7_szene.

**Ergebnis: NICHT BESTANDEN.** Frage (3) bekommt die dritte Klasse: Von vorn liegt direkt rechts neben dem gelobten Gesicht eine graue
Platte mit gerader harter Kante gegen die neue rosa Haut, darin helle weisse Keile. Alle anderen Fragen sind "wie gewuenscht" oder
"Abweichung klein". Die Punkte der dritten Klasse aus r3 sind behoben: die rote Kappe, die roten Kacheln, das Unterarmband und der
Wurzelkeil.

## Eigene Laeufe (roh, kein AUTOSHOT, kein SOFTWARE_RENDER)
Alle Dateien liegen unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr4s/

- **gdigrab, echtes Fenster** (gdi_p.sh = gdi.sh des Bauers, aber das Fenster wird ueber die EIGENE PID gesucht, Prozessname
  re15_sfv4_<eigene PID>; fremde re15-Prozesse wurden nicht angefasst): gdi/gdi_pruef4.mp4 (50 s, 960 x 720). Der Lauf endete mit EXIT 0.
  szene_gdi/state.log hat 1131 Zeilen und ist **bytegleich** zu sfv4/r/V7_szene/state.log. debug.log meldet "ROOM50A0 Szene Ende: Cut 4".
  Kacheln: k/uebersicht.png (t 7-17 s), k/hinten.png (t 11,50-12,75 s), k/vorn.png (t 13,00-14,50 s und 10,50 s).
- **Framedumps F250-F380** (lauf_v4.sh, EXIT 0, 131 Bilder), dazu V7 | V8r4 bei gleicher Bildnummer: k/k_kopf.png (F346/354/362/370, x2,5),
  k/k_oben.png (Oberkante hinter dem Kopf, F354/F362, x4), k/k_hinten.png (F320/326/332/338), k/k_seite.png (F262/276/290/304),
  k/folge.png (F252-F344, jedes 4. Bild), k/schwanz_nah.png (F274-F308).
- **Offline mit gleicher Spielkamera** (f_rast, Werkzeuge des Bauers, nur gelesen): Original-EM30 | V7 | V8r4 in P1, P2, F328, F332,
  F338 und F354 (off/*_o_v7_v8.png), dazu off/PV_nah_V8r4.png, off/huegel_V8r4.png und off/kopf_umfeld_F354_orig_v7_v8r4.png.
  Messskripte: off/farbe.py, off/hell.py, off/huegel.py, off/streifen.py, weiss.py.
- Angesehen: alle Tafeln 01-12, Darstellung.png, Darstellung_Vorlage (in Tafel 01), Beschreibung1.png, Beschreibung2.png, image_1.png
  (KI-Bild, nur als Hilfe fuer die Raumform).

## Seite
"Die Rechte Seite" ist die (W)-rechte Seite. In PV/Foto 2 ist sie die zur Kamera gewandte Flanke, mit dem grossen Oberarm unter dem Buckel
(Knochen 13). Von vorn (P1, Szene F344-F372) liegt sie im Bild links. Das habe ich an Tafel 01/02/05 und off/P2_o_v7_v8.png geprueft.
Es stimmt mit Messblatt und Dossier ueberein.

## Antworten

### (1) Koerperbau wie die Vorlage: Abweichung klein
- In PV (Tafel 01, off/PV_nah_V8r4.png) kommt der Schwanz breit aus dem oberen Ruecken. Er zieht im glatten Bogen entlang der gruenen Linie
  bis zur roten Linie am Boden und ist unten nicht duenn. Riegel S1 2,2/6,0 px, S4 s 140-160 31,3/30,9 px. Dasselbe zeigen P2 (Tafel 02)
  und die Szene von der Seite (k/k_seite.png F262, k/schwanz_nah.png F274-F278): ein breites Horn, das zur Spitze schmal wird.
- Rest: Farbkante an der Wurzel. Der Ruecken ist jetzt hell rosa, der Schwanz olivgrau. Gemessen in PV, Lab, erodierte Masken:
  - **V8r4:** Rumpf L 42 / a* 10,5, Schwanz L 17 / a* -0,3. Der Helligkeitssprung betraegt 25.
  - **Vorlage, gleiche Pixel:** Rumpf L 31 / a* 9,7, Schwanz L 15 / a* 6,1. Der Sprung betraegt 16, und beide Teile sind mauve.
  - **V7:** Sprung 12.

  An der Wurzel sitzt deshalb eine sichtbare Kante mit einem dunkelroten Saum (Tafel 01 rechts etwa x 1330-1345). Der Schwanz wirkt wie
  ein angesetztes Teil und nicht als "geschlossen ein zusammenhaengendes Stueck" (Nutzer). Die Form selbst ist erreicht.

### (2) Ruecken mit kleinem roten Huegel, glatte Linie zum Kopf, von vorn kein Kamm: Abweichung klein
- Huegel: In den obersten 8 px bei x 428-445 in PV liegt das Modell bei a* im Mittel 12,5, davon 74 % >= 10. Die Vorlage hat an
  denselben Pixeln a* 9,2 und 43 % >= 10. Fuer V7 waren es 5,2 und 6 %. Der Huegel ist also roetlich wie in der Vorlage, aber eher
  rosa-rot als ein klar roter Fleck (off/huegel_V8r4.png).
- Von vorn gibt es keine rote Kappe mehr: a* >= 30 ueber dem Kopf ergibt P1 0 und F354 0 (Riegel S11a). Die Kontur liegt hoechstens 2 px
  ueber V7 (S9).
- Rest: Hinter und ueber dem Kopf zeigt sich jetzt eine kantige Giebelform im Inneren der Silhouette. Ein Grat laeuft vom hoechsten Punkt
  ueber dem Kopf schraeg nach rechts zur Schulter, mit geraden, hell-dunkel abgesetzten Flaechen (k/k_oben.png F354/F362). In V7 ist dieselbe
  Stelle rund und gleichmaessig grau. Das ist kein Kamm ueber der Kontur, wirkt aber eckiger als V7. Gegen "smooth straight to head" ist
  das eine kleine Abweichung.

### (3) Rechte Seite: mehr rote Adern, sauber wie am Original-Birkin: Abweichung, die der Nutzer ablehnen wuerde
Was erreicht ist:
- In PV und P2 traegt die (W)-rechte Flanke die blassrosa Haut des Original-Birkin mit roten Adern (Tafel 05/10, off/P2_o_v7_v8.png). Der
  Rotanteil in PV liegt fuer den Rumpf bei a* 10,5 (Vorlage 9,7, V7 4,2) und fuer den Oberarm bei 12,0 (Vorlage 9,4). Die Farbrichtung
  stimmt also.
- Das gerade rote Unterarmband ist weg. In P2 liegen 0 rote px auf Knochen 14 (S12), und auch in P2/P1/F354 sehe ich kein Band mehr. Die
  violetten Adern am Unterarm stammen vom Original (off/F354_o_v7_v8.png, Original links).
- Die Rueckennaht rosa/grau aus r3 ist weg (S13).

**Was neu bzw. verschoben ist und sofort ins Auge faellt:** Von vorn liegt rechts neben dem Gesicht, also an der (W)-linken oberen Brust,
eine graue Platte (Brust-Zeile 488, V7 nach O1). Sie grenzt jetzt mit gerader harter Kante an die neue rosa Haut darueber und links vom Kopf.
Darin stehen helle weisse Keile.
- Gemessen offline in F354 (5x5-Mittel, Lab):
  - **V8r4:** Platte L 32 / a* 5,6, Haut darueber L 51 / a* 14,8. Der Sprung betraegt dL 19 und da* 9.
  - **V7:** dieselben Stellen 32/5,4 gegen 36/4,0, also ein Sprung von dL 4.
  - **Keil:** V8r4 L 70 gegen V7 L 50.
- Gemessen in den echten Framedumps F344-F372 im Fenster x 560-690 / y 260-330: fast weisse Pixel (L > 75, Chroma < 12) **V8r4 2097
  gegen V7 5**. In jedem Bild sind es 76-221.
- Das Original-EM30 ist an dieser Stelle durchgehend rosa mit Adern und hat weder Platte noch Naht (off/kopf_umfeld_F354_orig_v7_v8r4.png).
- Sichtbar ist das in der laengsten Frontalphase der Szene: k/k_kopf.png F346-F370, k/vorn.png (gdigrab t 13,00-14,00), Tafel 09 rechts
  und Tafel 11 t 13,25/14,00. Bei t 11,75 (Tafel 11) steht dort ein heller senkrechter Streifen neben dem Kopf.
- Das Dossier fuehrt den Punkt selbst als "Offen 2".

Die Naht rosa/grau ist also nicht verschwunden, sondern von der linken auf die rechte Seite des Gesichts gewandert. Dazu kommen
weisse Keile. Genau diese Art von "aufgelegt/fleckig" hat r3 als dritte Klasse gewertet, und hier sitzt sie direkt am gelobten Gesicht.
Hinweis: Die Ursache liegt im Zusammenspiel der Vorgaben. O1 laesst die Brust wie V7, waehrend der restliche Rumpf auf die Original-Zeilen
geht. Gemessen wird hier, was der Nutzer sieht.

### (4) Gesicht und linke Hand wie V7: wie gewuenscht
Pixelvergleich V7 | V8r4 bei gleicher Pose und Kamera (offline):
- Knochen 9: 0 ungleiche Pixel in P1 (1267 px), P2 (902), F354 (1291), F328 (1396), F332 (1074) und F338 (1016).
- Knochen 10-12: 0 ungleiche Pixel (P1 4330, F354 4488 px usw.).
- Beine 0-7 und Knochen 15: 0 ungleiche Pixel.

S5 ist gruen. Das Gesicht selbst ist unveraendert. Sein Umfeld wirkt aber anders, siehe (3).

### (5) Szene in Bewegung: Abweichung klein
- Der Ablauf ist bytegleich zu V7: state.log ist gleich, "Szene Ende: Cut 4". Der Schwanz bewegt sich ruhig als breites Horn, ohne Balken,
  Knick oder Zickzack (k/folge.png F252-F344, k/schwanz_nah.png). Ein Durchstossen durch Beine oder Rumpf habe ich in F250-F380 und im
  gdigrab t 9-15 s nicht gefunden.
- Von hinten gibt es keinen Keil und keine Luecke an der Wurzel mehr (k/k_hinten.png F332/F338, k/hinten.png t 12,25-12,75, Tafel 12).
  Das ist gegenueber r3 behoben.
- Den Eindruck eines Schlauchs auf dem Ruecken habe ich von direkt hinten nicht. Der Schwanz waechst oben aus dem Ruecken und hebt sich
  im Bogen ab.
- Rest: Um die Wurzel liegt von hinten ein auffaelliges diagonales rot-weisses Streifenfeld (Original-Textur auf der angehobenen Haut,
  Dossier "Offen 4"). Gemessen (Rumpf, lokale Streuung a* und L > 12):
  - **F332:** V8r4 364 px (2,6 % des Rumpfs) gegen Original-EM30 254 px (3,8 %).
  - **F328:** V8r4 319 px (2,6 %) gegen Original 188 px (2,8 %).

  Anteilig entspricht das dem Original, absolut ist das Feld groesser. Weil es genau an der Wurzel sitzt, sieht die Wurzel von hinten
  aus wie ein Horn, das in einer gestreiften Manschette steckt. Dazu kommt mitten im Ruecken ein kleines scharfes rotes Dreieck, das am
  Original an derselben Stelle ebenfalls vorhanden ist.

### (6) Insgesamt klarer Fortschritt gegenueber V7: Abweichung klein
Der Koerperbau ist ein deutlicher Fortschritt gegenueber V7: breiter Schwanz aus dem oberen Ruecken, Bogen bis zum Boden, unten nicht duenn.
Auch die rechte Flanke mit Original-Adern und die ruhige Szene ohne Keil, Kappe und Band sind Fortschritte. Der Nutzer wuerde den
Fortschritt erkennen. Er wuerde aber in der Frontalphase sofort die graue Platte mit weissen Keilen neben dem Gesicht ansprechen, ebenso
die Farbkante olivgrau/rosa an der Wurzel. Wegen (3) gilt der Stand als nicht bestanden.

## Punkte fuer die naechste Runde (aus den Messungen, keine Gestaltung)
1. Die Uebergangsflaechen zwischen der Brust (Zeile 488, V7) und der Original-Haut von Mesh 8 neben dem Kopf, also die (W)-linke obere Brust
   von vorn, so behandeln, dass von vorn keine gerade Kante und keine weissen Keile entstehen. Vorschlag fuer einen Riegel: weisse Pixel
   (L > 75, C < 12) in F344-F372 im Kopfumfeld <= V7 + kleiner Rest (heute 2097 gegen 5) und dL ueber die Brustkante <= V7 + 5. Ob die Brust
   selbst bleiben muss (O1), entscheidet der Orchestrator.
2. Farbkante an der Wurzel in PV: Helligkeitssprung Rumpf/Schwanz heute 25 L, Vorlage 16 L.
3. Die Giebelkante ueber dem Kopf von vorn (k/k_oben.png) beobachten. Sie ist heute klein.

Testbelege (ctest 8/8, 1013/1013, V8-KETTE-OK) und die Riegel-Liste habe ich aus den Dateien des Bauers uebernommen. Selbst geprueft
habe ich: Asset-sha256, beide Szenenlaeufe (state.log bytegleich), gdigrab, die Pixelvergleiche zu (4) und die Messungen zu (1), (2),
(3) und (5).
