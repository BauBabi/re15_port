# K5g3 Geometrie-Stufe (Runde 1 und 2) - Arbeitsnotiz

**Runde 2 (Abschnitt 9) hat die Geometrie ersetzt:** heute gilt k5g/K5g3/geo/BAHNSTEIG_FORM.EMD sha256
db9404a18368f420050cddbc45f06fca11c24c80a0d315082668280576590e00 (FORM.EMD d84d4762...), Stufen posen -> krallen -> spalt -> uvfein.
Die Tabellen und sha in den Abschnitten 1-8 sind der Stand von Runde 1 (eadfac31...).

Stand 2026-11-14. Grundlage: Orchestrator-Entscheide G1 (Spalt B4, vom Nutzer am 14.11. abgenommen), G2 (rechte Krallen aus K5c,
Posen vorerst K5c, Posen-Datei optional), G3 (Dreiecks-Liste 16384 + Ueberlauf-Zaehler), G5 (Spiel-Asset bleibt K5e_f1).
Alle Pfade relativ zu `analysis/befunde_runde41/form/varianten/K5/`, wenn nicht anders gesagt.

## 1 Aufruf und Stand

```
python generator/kf_k5g3_geo.py bauen      # -> k5g/K5g3/geo/BAHNSTEIG_FORM.EMD, FORM.EMD, bau.json
python generator/kf_k5g3_geo.py pruef      # zwei Laeufe + Datei byte-gleich, Byte-Probe gegen KAND.EMD (Exit 0/1)
python generator/kf_k5g3_geo.py bauen --posen <krallen_posen.json>   # spaeter, mit der Posen-Datei der rechten Hand
```

| Datei | Byte | sha256 |
|---|---|---|
| k5g/K5g3/geo/BAHNSTEIG_FORM.EMD (Geometrie) | 586012 | eadfac31aef1864644376c9952641ff584334c3e8293702df918d3a0e3758409 |
| k5g/K5g3/geo/FORM.EMD (Werkzeug, 3 Keys, MD1 der Geometrie) | 462920 | 3f1f70b3fcc6dda6e54500b7ee5cc249f63005f54d7af9c86eeb0e4450aaaca4 |
| TIM dazu = K5e_f1 (ruhe/K5e_f1/BAHNSTEIG_FORM.TIM = shared_assets/RE2) | 136224 | 31a35e66428bac1e8783587f2336ea0bec7aab6e34876b0be87a144c576445b8 |

Laden im Spiel: `RE15_RE2_ASSET_ROOT=<...>/K5/k5g/K5g3/geo`. Die TIM liegt dort nicht; das Spiel nimmt sie aus
`re15_port/shared_assets/RE2` (asset_root_pc.c `re15_pc_read_re2`: erst der Ordner aus der Variable, dann shared_assets) - das ist
genau die K5e_f1-TIM.

Eingaenge (alle mit sha256 geprueft, sonst Abbruch):

| Eingang | Datei | sha256 |
|---|---|---|
| Basis K5e_f1 | ruhe/K5e_f1/BAHNSTEIG_FORM.EMD (= Spiel-Asset) | f277916d7d6518b5... |
| G1 Spalt B4 | generator/k5g3_spalt_b4_tabelle.json (Kopie von scratchpad k5g2/spalt/kand_b4/TABELLE.json) | 99a4f142c2d01c5eb8e70dd299c0cd98c3e34268adc8473454dcfb42b4cd6d07 |
| G2 Krallen | generator/k5g3_krallen_k5c.json (Ecken+Normalen 19/20/28/29 aus einbau_k5c/BAHNSTEIG_FORM.EMD) | 677aa8cf522e1e9b...; Quelle K5c 1ab56076b20ef022... |
| Posen | keine Datei (Vorgabe generator/krallen_posen.json fehlt) | - |

Stufen: posen -> krallen -> spalt. Der Spalt laeuft ueber `kf_k5g2_geo.geometrie_form(stufen=('mulde',), tabelle=...)` - derselbe
Weg, mit dem die Messbank (scratchpad k5g2/spalt/bank.py `emd`) KAND.EMD gebaut hat. `kf_k5g2_geo.py` ist NICHT geaendert (die
Messbank importiert ihn live; MULDE.EMD sha 7726e46f bleibt reproduzierbar).

Byte-Probe (Auftrag Punkt 1): Geometrie mit K5e_f1-Posen und K5e_f1-Krallen (nur Stufe spalt) ergibt sha256
f78e598336872111de14a7b06ec7cb3610c8777897d30fc3b5bc9f12945c1a66 = scratchpad k5g2/spalt/kand_b4/KAND.EMD. Zweiter Lauf byte-gleich.

## 2 Was gegen K5e_f1 anders ist

| Teil | K5e_f1 Ecken / Dreiecke / Vierecke | K5g3 | Herkunft |
|---|---|---|---|
| 10 Buckel/Brust | 212 / 72 / 168 | 804 / 1483 / 0 | B4-Tabelle |
| 16 rechter Oberarm/Schulter | 134 / 48 / 104 | 406 / 640 / 80 | B4-Tabelle |
| 21 Hals | 102 / 48 / 72 | 1688 / 2981 / 0 | B4-Tabelle (Kopfkugel entfernt, KK_WEG) |
| 22 Kopf | 402 / 182 / 218 | 2768 / 5073 / 0 | B4-Tabelle |
| 19, 20, 28, 29 rechte Krallen | je 26 Ecken | Ecken aus K5c, Normalen gleich (in K5c und K5e_f1 identisch), Flaechen/UV/CLUT/Seite K5e_f1 | G2 |
| 31 rechte Kralle | | unveraendert (K5c = K5e_f1, geprueft) | G2 |
| alle uebrigen 24 Netze, Knochen | | byte-gleich K5e_f1 | |
| Sektionen 0-6 (Morph, EDD/EMR aller drei Baenke, 123976 Byte) | | byte-gleich K5e_f1 UND K5c | G2 |

Gemessen: Die Posen von K5c sind Byte fuer Byte die von K5e_f1 (alle 771 Keys, Clips, Knochen). "Posen = K5c" heisst also: keine
Posen-Aenderung. Port-Dreiecke des Modells: 15249 (K5e_f1 6386).

Posen-Datei (G2, fuer die Entscheidung des Nutzers zur rechten Hand in Bild 2): Format wie kr4 (`keys/<key>/alt|neu/<knochen>` =
[x, y, z], Winkel 0..4095); nur Keys 398-425 und 542-556, nur Knochen des rechten Arms (16 und alles darunter: 16, 17, 18, 19, 20,
29, 31); 'alt' muss der K5c-Wert sein (sonst Abbruch "Posen-Datei passt nicht zur Form"). Die kleine FORM.EMD zieht Key 1 (= Key
542, Bild 2) automatisch nach (nur Winkel; Key 0 = Key 11 liegt ausserhalb der erlaubten Keys). Riegel R7b prueft den Weg mit
einer Probe-Datei (Key 542 Knochen 17 + 64).

## 3 Dreiecks-Liste (G3)

`re15_port/platform/pc/src/render_pc.c:226` TEXTRI_QUEUE_MAX 8192 -> 16384, mitgezogen: s_textri_queue/_depth/_slot/_blend,
s_sb_* (Standbild-Kopie, gleiches Makro), s_textri_flush_buf, order[] in end_frame. Port-Puffer, kein Spielwert.
Speicher: 196 Byte je Eintrag (Liste 66, Standbild 66, Flush 60, order 4) -> 3,06 MiB statt 1,53 MiB.

Zaehler: jedes wegen voller Liste nicht eingereihte Dreieck. debug.log `[tri-liste] UEBERLAUF ...` (sofort), Stand alle 120 Bilder,
Schlusszeile `[tri-liste] ENDE hoechststand=.. verworfen=..` (re15_testhaken_ende in main.c). Abfrage
`re15_render_pc_textri_verworfen()/_hoechststand()/_grenze()`. Messhaken `RE15_TRILISTE_GRENZE=<n>` (nur kleiner).

Test `integration_r41_form_dreiecke` (re15_port/tests/integration/test_r41_form_dreiecke.cmake), K5g3-Geometrie per
RE15_RE2_ASSET_ROOT, SOFTWARE_RENDER wie die anderen Kampf-Tests; "1/1 Passed", 375 s:

| Lauf | Aufbau | hoechststand K5g3 | K5e_f1 (gleicher Lauf, RE15_FORM_ASSET_ROOT=-) | verworfen |
|---|---|---|---|---|
| kampf | 5090 wie integration_r41_birkin_pistole: Form kommt, 96 Schuesse, Form getroffen (HP bis 0), Kamera auch Schnitt 11 | 7415 | 4801 | 0 |
| s11 | 5090 Schnitt 11 (RE15_FORCE_CUT=11), Form Clip 12 abgespielt an der Bild-2-Lage, Spielerin im Bild, 53 Schuesse | 7718 | 4438 | 0 |
| v5080 | 5080 von vorn (Schnitt 1), Form Clip 12, Spielerin im Bild, 53 Schuesse | 7184 | 3646 | 0 |
| Gegenprobe | s11 mit RE15_TRILISTE_GRENZE=2000 | 2000 | 2000 | 6528562 (K5e_f1 3049908) - rot wie verlangt |

Die Pruefung "hoechststand >= 6000" belegt, dass wirklich das grosse Modell gezeichnet wurde (K5e_f1 bleibt in allen Laeufen
unter 4900). Dazu ein Budget-Lauf an der laut Messbank schwersten Stelle (5080 von vorn, Clip 11 abgespielt, Spielerin im Bild,
eigenes Fenster): K5g3 7596, K5e_f1 3591, verworfen 0.

Ehrlich: In keinem Spiellauf kam die Liste ueber 8192 (hoechstens 7718). Die Messbank rechnet fuer die schwerste Ansicht 7318
Form-Dreiecke + 773 + 86 = 8177 - mit Effekten haette 8192 also nicht sicher gereicht; 16384 laesst ueber 8600 Platz.

## 4 Riegel

`python generator/riegel_k5g3.py`, Protokoll k5g/K5g3/geo/riegel_k5g3.txt (+ .json). Summenzeile: siehe Abschnitt 4.1.

| Riegel | prueft | Gegenprobe (muss rot sein) |
|---|---|---|
| R1 Generator | Datei == Generator, zweiter Lauf gleich | ohne Krallen-Stufe |
| R2 Byte-Probe | K5e_f1-Posen/-Krallen + B4 == KAND.EMD f78e5983 | volle Geometrie |
| R3 Spalt | Netze 10/16/21/22 == B4-Tabelle, Tabelle sha 99a4f142 | K5e_f1 |
| R4 Krallen | 19/20/28/29 Ecken+Normalen == K5c, Flaechen/UV/CLUT/Seite == K5e_f1; 31 == K5e_f1 == K5c | K5e_f1-Krallen; K5c-Netze ganz (K5c-UV/CLUT) |
| R5 Rest | 24 Netze und Knochen == K5e_f1 | Netz 31 eine Ecke +1 |
| R6 EDD/EMR | Sektionen 0-6 byte-gleich K5e_f1 und K5c | ein Bit in emr1 |
| R7 Posen | Pool == K5c; FORM.EMD Key 0/1 == Key 11/542, MD1 == Geometrie | Probe-Posen-Datei; Posen-Datei mit Key 300 |
| R7b Posen-Datei | Probe-Datei wirkt genau auf ihren Tripel, FORM.EMD zieht nach | Probe-Datei mit falschem 'alt' |
| R8 B4-Grenzen | Messbank-Urteil (urteil4, voller Lauf 3995 Ansichten) des Gesamtmodells nicht schlechter als B4 allein | Urteil von B3 statt Gesamtmodell |
| R9 Dreiecke | Messbank max je Bild + 773 + 86 + 1000 Effekte <= 16384 - 2048; Spiel hoechststand <= 14336, verworfen 0 | alte Grenze 8192 |

### 4.1 Ergebnis

Summenzeile (riegel_k5g3.txt): **"RIEGEL K5g3 GEOMETRIE: 10 von 11 gruen, 13 von 13 Gegenproben rot wie verlangt"**, Exit 1.
ROT ist R8 - aus einem Grund, der in G2 liegt, nicht im Bau (R8b belegt das, gruen).

Messbank voll (scratchpad k5g2/spalt/bank.py `lauf`, 3995 Ansichten in 10 Teilen wie bei B4, ohne Abbruch; Urteil mit
spalt/kand_b4/urteil4.py; Kopien messung/urteil_gk.json/.txt, Bezug messung/urteil_b4.json):

| Grenze | B4 allein | Gesamtmodell K5g3 | |
|---|---|---|---|
| H1a Streifen Kopf/Kiefer gegen Brust/Hals, Summe | 3578 | 3578 | gleich |
| H1b groesstes Streifenstueck | 23 | 23 | gleich |
| H1c je Ansicht ausserhalb Clip 10/12 | 20 | 20 | gleich |
| H2 mit Schulter 16: Summe / Ansichten > 20 | 12543 / 162 | 12543 / 162 | gleich |
| H3b Schulter gegen Buckel, rote Ansichten / Summe | 0 / 384302 | 0 / 383432 | besser |
| H4 meiste gezeichnete Dreiecke | 7318 (spiel5080_vorn 11:30) | 7276 (dieselbe Ansicht) | besser |
| H9-S14 Stirnlicht | gruen | gruen | gleich |
| H8 Tabelle | gruen | gruen | gleich |
| H3a Paare mit Kopf/Kiefer | gruen | **ROT**: 19<->23 535, 20<->22 424, 19<->22 104 (Grenze je 50; K5e_f1 und B4: 0) | Krallen |
| H7 Umriss Bild 2 gegen die Nutzer-Masken | 0,65316 | **0,61248** (Grenze 0,65244) | Krallen |
| H7 Umriss Bild 1 | 0,72330 | 0,72328 | gleich |
| H6b fremde Netze | keine | 19, 20, 28, 29 | gewollt (G2) |
| Zielmarken neu / hoch48 (Formabstand zu K5e_f1) | erreicht | groesste 10966 (bild2 3:25) / 24054 (spiel5080_vorn 8:12) | Krallen-Form anders als K5e_f1 |
| H9 (S9 Kanten) / S18 | rot (30 Zeilen) / rot | rot (30 Zeilen, dieselben) / rot | wie B4, vom Nutzer angenommen |

Alles, was der Spalt misst, ist mit den neuen Krallen gleich oder besser. Die Verschlechterungen haengen alle an den K5c-Krallen:
- Umriss Bild 2: K5c ALLEIN (einbau_k5c, vom Nutzer am 07.11. abgenommen) hat 0,61228, K5g3 0,61248 (messung/umriss.json). Die
  K5c-Krallen ragen in der Bild-2-Haltung (Key 542) ueber die Nutzer-Maske hinaus - genau die Haltung, ueber die der Nutzer gerade
  am Vergleichsblatt entscheidet (Keys 398-425, 542-556 -> Posen-Datei).
- H3a: Maler-Streifen zwischen rechten Krallen und Kopf/Kiefer, fast nur in Clip 3 Bild 12-16 und Clip 9 (Kralle 19 gegen
  Kiefer 23: groesste Ansicht 52 px, spiel5080_vorn 3:13; 20 gegen 22: 43 px, bild2 3:14; 31 gegen 22: 49 px, bild2 9:5 - 31 ist
  unveraendert K5e_f1 und war dort unter der Grenze). Die langen K5c-Krallen kommen in diesen Bildern dem Kopf nahe.

## 5 Spiellauf

Eigene exe-Kopien (`generator/kf_k5g3_spiel.sh` bzw. der Test), Modell per RE15_RE2_ASSET_ROOT, gdigrab nur auf das eigene
Fenster (Handle ueber die eigene PID), kein AUTOSHOT/SOFTWARE_RENDER bei den Fensterbildern, Ende ueber RE15_EXIT_AT. Kein Absturz
in 19 Laeufen (14 mit K5g3, 5 mit K5e_f1 als Vergleich); in jedem "Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips". Verworfene Dreiecke: 0 in allen Laeufen ohne
Messhaken (Zahlen in Abschnitt 3; messung/spiel_dreiecke.json).

Selbst angesehen (geo/spiel/):
- k5g3_r5080_vorn_clip12_schiessen.png: 5080 von vorn, Leon zielt auf die Form, Form in Clip 12 vollstaendig, Kopf und Buckel
  ohne Loecher, die rechten Krallen stehen als lange dunkle Klingen nach oben rechts.
- k5g3_r5090_s11_clip12_a.png / _schiessen.png: 5090 Schnitt 11 mit Feuer im Wagen, Annette links, Form ueber Leon. Leon zielt
  in diesem Aufbau auf Annette (0x4D), nicht auf die Form - die Form wird im Messhaken RE15_BB_POSE gehalten und nicht getroffen.
  Den Kampf MIT Treffern liefert der Lauf "kampf" (Form-HP 150 -> 0, Rueckstoss, Sturz).
Die Figur ist im Fenster dunkel (Textur K5e_f1 im Raumlicht); Einzelheiten der Kanten sieht man dort nicht, dafuer die Tafel.

Falle (gemessen): `RE15_BB_POSE` mit Schnitt >= 0 setzt jedes Bild `cam_change_pending`; seit der Kamerahalte-Regel
(engine/src/kamerahalt_r41.c, Spur E) wird dann JEDES Bild gehalten - es wird nichts gezeichnet (hoechststand 0, Fenster zeigt
den alten Schirm). `re15_port/tools/form_spiel.sh` und `generator/kf_k5g2_fenster.sh` laufen deshalb heute ins Leere.
`RE15_FORCE_CUT=<n>` schaltet die Halteregel ab; so arbeiten kf_k5g3_spiel.sh und der Test.

## 6 Tafel

`tafeln/10_geometrie.png` (`python generator/kf_k5g3_tafel.py`, roh, Buehne des Ports, weisses Licht, Textur K5e_f1 fuer beide):
je Bild 1 (Clip 0 Bild 11), Bild 2 (Clip 12 Bild 0) und Spiel 5080 von vorn (Clip 12 Bild 0) drei Kacheln K5e_f1 | K5g3 |
Unterschied (weiss = Bildpunkt anders), darunter ein vergroesserter Ausschnitt.

Was zu sehen ist:
- Bild 1: fast gleich (220 Bildpunkte anders), nur oben am Buckel/Nacken kleine Sprenkel - die fein geteilten Spalt-Netze, die nach
  Dreiecksmitte gemalt werden. Die rechte Hand liegt in Bild 1 verdeckt.
- Bild 2 (7611 anders) und 5080 von vorn (9881 anders): (a) die rechten Krallen sind lang und klingenartig (K5c), zwei grosse
  Keile links im Bild, K5e_f1 hatte dort kurze Stummel; (b) am Buckel und Hinterkopf viele kleine verstreute Unterschiede =
  Spalt B4 (abgenommen); Gesicht, Zaehne und Unterkiefer gleich.
- Die Krallen tragen noch die K5e_f1-Textur (dunkles Grau mit hellen Sprenkeln) - die Farbe macht die Textur-Stufe (V5).

## 7 Fuer den Textur-Bauer

- Geometrie: k5g/K5g3/geo/BAHNSTEIG_FORM.EMD (sha eadfac31...), mit `kf_k5g3_geo.geometrie_form()` als Form zu lesen (Rueckgabe
  Sektions-Bytes, Form, Bericht). Posen = K5e_f1 (byte-gleich).
- Netze 10/16/21/22 kommen aus generator/k5g3_spalt_b4_tabelle.json; jede Flaeche hat in `herkunft` ihre K5e_f1-Mutterflaeche.
  G4: auf den K5e_f1-Flaechen malen (Zonen v2 und traeger_texel gelten), dann `kf_k5g2_geo.mulde_auf_form(form,
  tabelle=kf_k5g3_geo.GEO3_SPALT)` - Kinder erben CLUT, Seite und UV-Verschiebung der Mutter (Lauf ohne Abbruch, Messbank H8).
  Die Kopfkugel in Netz 21 ist weg; Mutterflaechen ohne Kind erscheinen nicht mehr.
- Krallen 19/20/28/29: nur Ecken geaendert, Flaechen/UV/CLUT/Seite = K5e_f1 - die Zonen (19 -> 35.2, 20/29 -> 35.3, 28 -> 39.4)
  gelten auf denselben Flaechen.
- Netz 22 hat 1463 UV-entartete Port-Dreiecke (Messbank H8), Netz 10 45, Netz 21 37 - Farbstreifen statt Texturstueck, im Texelraum
  nicht heilbar.
- FORM.EMD liegt neben der Geometrie (MD1 der Geometrie, Key 0/1 = Bild 1/Bild 2).

## 8 Offen

1. R8 rot wegen der K5c-Krallen (Abschnitt 4.1): Umriss Bild 2 0,612 statt >= 0,652 und Krallen-Kopf-Streifen in Clip 3/9. Der
   Umriss haengt an der Haltung der rechten Hand in Bild 2 - die Posen-Datei (Nutzer-Entscheid) wird ihn veraendern; danach
   `kf_k5g3_geo.py bauen --posen <datei>`, Messbank-Lauf und `riegel_k5g3.py` neu.
2. Posen-Datei der rechten Hand fehlt (G2, Nutzer entscheidet am Vergleichsblatt).
3. form_spiel.sh / kf_k5g2_fenster.sh brauchen RE15_FORCE_CUT (Abschnitt 5), sonst zeigen sie nichts.
4. Bildzeit: die Einfuegesortierung in end_frame (render_pc.c) ist quadratisch; mit bis zu 7718 Dreiecken je Bild nicht gemessen.
5. Unit-Test-Pins (test_birkin_bahnsteig.c MD1-Laenge/CRC) werden rot, sobald K5g3 Spiel-Asset wird (G5, spaeter).
6. Wie bei B4 angenommen: H9-Kanten, S18-Risse (3 px), Sprenkelfleck im linken roten Halsfeld, Schulter-Naht im Licht.

## 9 Runde 2 (2026-11-14): Befunde der Pruefung K5g3 Runde 1, Anteil der Geometrie

### 9.1 Welche Befunde die Geometrie betreffen (gemessen, nicht angenommen)

| Befund (Pruefer) | Anteil Geometrie | Beleg |
|---|---|---|
| Weisse Musterung C* 18-25 (Zahlen, ROT) | keiner | Material-Transformation kf_k5g3_tex.py:356-362 (k_haut), Textur-Stufe |
| Weisser Strich Flaeche 10\|1\|76 (Zahlen, ROT) | keiner (Ursache ist die Kopie-Maske) | siehe 9.3 |
| Haut drei Seitenfaktoren (Zahlen, GELB) | keiner | Textur-Stufe / Orchestrator |
| Braun, Reinweiss auf der Haut (Zahlen, GELB) | keiner | Textur-Stufe |
| Augen verschwunden, schwarzes Loch rechts (Sicht, ablehnen) | keiner | siehe 9.4 |
| Gestreckte Farbstreifen an Kopf/Kiefer aus UV-entarteten Dreiecken (Sicht, klein) | ja, teilweise | siehe 9.2 - behoben mit der Stufe uvfein |
| Leuchtstelle F1 lila-grau (Sicht, klein) | keiner | F1-Flaechen sind nicht geteilt (Netz 0) |

### 9.2 Gestreckte Farbstreifen: Stufe uvfein

Mechanismus (gemessen): B4 (scratchpad k5g2/spalt/kand_b4/bau.py attribute()) rundet die UV jeder neuen Ecke einzeln (rint der
baryzentrischen Mutter-UV). Bei kleinen Kindern faellt das UV-Dreieck dadurch auf eine Linie (Flaeche 0 bei zwei verschiedenen Ecken);
das Dreieck zeigt dann nur die Texel entlang dieser Linie, quer gestreckt. Zaehlung der Render-Dreiecke mit UV-Linie in den Netzen
10/16/21/22: K5e_f1 0/3/0/25 = 28, B4 41/36/36/1461 = 1574. Ausserdem: 2711 von 9961 Kindern haben eine Ecke ausserhalb der UV-Flaeche
der Mutter (Rundung).

Aber auch: der Grossteil der Streifen, die man unter weissem Licht sieht, ist schon auf den UNGETEILTEN Flaechen da (Pruef-Stand
k5g/K5g3/pruef = K5e_f1-Flaechen mit K5g3-Textur): die K5e_f1-UV ist am Kopf stark gestreckt, die kontrastreiche K5g3-Textur macht das
sichtbar. Messung (scratchpad k5g2/geo_r2/m9b.py, rl.weiss_id 760 px, Clips 0/1/2 x 6 Winkel = 18 Ansichten, Textur K5g3):
Pixel auf Linien-Dreiecken der Netze 10/16/21/22 ungeteilt 569, B4 10687, Runde 2 2203 (von 315722); Pixel, die vom ungeteilten
Stand abweichen (Farbabstand > 24): B4 42795, Runde 2 44053 (von 314874; darin stecken auch die Formaenderungen von B4).

Stufe uvfein (generator/kf_k5g3_uv.py, Regel im Dateikopf): nur die UV der Kinder; Ecken, Normalen, Flaechen, CLUT, Seite bleiben B4.
Je Kind ganzzahlige UV im UV-Gebiet der Mutter (Render-Dreiecke (0,1,3)+(0,3,2), main.c npc_qz1/npc_qz2 bzw. form_buehne.py), gewaehlt
nach (keine Linie / kein Kippen, Fehlrate der floor-Abtastung gegen die exakte Abbildung, Abstand). Ist im Gebiet nur eine Linie
moeglich, das B4-Kind aber gut, bleibt B4 (49 Kinder).

| | B4 (Runde 1) | uvfein (Runde 2) |
|---|---|---|
| Kinder mit neuer UV | - | 3743 von 9961 (10: 544, 16: 262, 21: 813, 22: 2124) |
| Linien-Dreiecke 10/16/21/22 | 41/36/36/1461 = 1574 | 5/34/0/186 = 225 (K5e_f1 28) |
| Kinder mit Ecke ausserhalb der Mutter-UV | 2711 | 49 (dort waere im Gebiet nur eine Linie moeglich) |
| gekippte Kinder (UV-Umlauf gegen die exakte Abbildung, |Flaeche| > 0,1) | 98 | 3 |
| Fehlrate floor-Abtastung gegen exakte Abbildung, flaechengewichtet | 0,2395 | 0,2478 (Preis der Nicht-Entartung) |
| Linien-Pixel, vier Kopf-Ansichten der Tafel 11 | 3443 | 674 (ungeteilt 111) |
| Augen-Texel (Zone 29) je Mutter 22\|0\|82, 83, 22\|1\|112, 113, 152 | 6/6/6/5/0 | 6/6/6/5/0 (= K5e_f1) |

Ehrlich: Die Fehlrate steigt leicht (+0,008) und die Abweichung vom ungeteilten Stand um 2,9 % der Pixel; ganz heilen laesst sich die
Rundung mit 8-Bit-UV nicht (eine optimierte Wahl je Kind ohne Linien-Bedingung senkt die Fehlrate nur von 0,216 auf 0,180 im Netz 22,
macht aber mehr Linien). 225 Linien-Dreiecke bleiben (186 im Kopf; 16 davon liegen in Muettern, deren UV-Gebiet selbst nur eine Linie
ist).

Aufruf unveraendert (`python generator/kf_k5g3_geo.py bauen | pruef`); `bauen` schreibt zusaetzlich geo/spalt_uv_tabelle.json
(sha cfec25bc..., B4-Tabelle mit den uvfein-UV), `pruef` prueft zusaetzlich diese Datei. Byte-Probe ohne uvfein weiter == KAND.EMD.

### 9.3 Weisser Strich 10|1|76: Ursache ist die Kopie-Maske, nicht die Teilung

Nachgemessen (scratchpad k5g2/geo_r2/idx0.py, m8.py; 16 x 16 Proben im Inneren je Dreieck wie der Zahlen-Pruefer): 26 Zugriffe auf
Index 0 (reinweiss), nur Kinder 10|0|342 (12), 343 (2), 344 (12). Kind 344 ist UNVERAENDERT das Render-Dreieck (0,3,2) der Mutter
(Texel (468,120), (476,169), (476,194)) und liest dieselben 12 Index-0-Texel wie 342. Die Mutter-UV (84,120) (85,172) (92,194) (92,169)
auf Seite 3 ist als Polygon (0,1,3,2) eine "Fliege" (Kanten 1-3 und 2-0 schneiden sich); kf_k5g2_tex.flaeche_maske (fillPoly +
polylines ueber 0,1,3,2) laesst darum einen Teil von Dreieck (0,3,2) aus, die Kopie bleibt dort leer. Die Geometrie-Stufe aendert das
nicht: mit uvfein 27 Zugriffe (343: 3). Erhebung ueber alle Vierecke des Pruef-Stands (bowtie2.py): 73 Vierecke mit sich selbst
schneidendem Polygon (0,1,3,2), 48 Vierecke mit Render-Texeln mehr als 1 Texel ausserhalb der Maske (groesste 10|1|149 268,
10|1|150 201, 22|1|195 133, 10|1|158 130). Behebung gehoert in die Textur-Stufe: Maske = Vereinigung der Render-Dreiecke (0,1,3) und
(0,3,2).

### 9.4 Augen: kein Anteil der Geometrie

R10 misst: die Kinder der fuenf Zone-29-Muetter lesen dieselben Augen-Texel wie K5e_f1 (6/6/6/5/0), in Runde 1 wie in Runde 2; Tafel 10
zeigt die Augen auf der Geometrie mit K5e_f1-Textur. Fuer die Textur-Stufe gemessen: 22|0|82 und 22|0|83 sind schon in K5e_f1
UV-entartet (alle drei Ecken v = 72, UV (109,72) (112,72) (103,72) bzw. (112,72) (115,72) (103,72)) - diese Flaechen koennen nur einen
gestreckten Strich zeigen, nie einen Punkt; 22|1|152 liest keinen Augen-Texel. Ein sichtbarer Augenpunkt braucht dort eigene Texel mit
eigener, nicht entarteter UV (V3 erlaubt "UV dieser Flaechen umlegen").

### 9.5 Messbank, Spiel, Riegel (Runde 2)

- Messbank voll (spalt/bank.py lauf, 3995 Ansichten in 10 Teilen, scratchpad k5g2/geo_r2/lauf, 17:10-18:03; urteil4.py): alle Zahlen
  gleich Runde 1 (H1a 3578, H1b 23, H1c 20, H2 12543/162, H3b 0/383432, H4 7276, H7 0,61248/0,72328, H3a 19<->23 535, 20<->22 424,
  19<->22 104), H5 Lauf == EMD db9404a1. Die Messbank misst Tiefe/Form, nicht UV - erwartet gleich, gemessen gleich.
  Kopien: geo/messung/urteil_gk.json/.txt. Umriss neu gemessen (bank.py umriss): 0,612480 / 0,723282 wie Runde 1 (messung/umriss.json).
- Spiel (eigene exe-Kopien, Modell per RE15_RE2_ASSET_ROOT=k5g/K5g3/geo, TIM = K5e_f1 aus shared_assets): Test-Ablauf
  re15_port/tests/integration/test_r41_form_dreiecke.cmake (kampf 5090 96 Schuesse, Form-HP bis 0; s11; v5080; Gegenprobe Grenze 2000
  rot) "alle Pruefungen gruen"; dazu kf_k5g3_spiel.sh mit gdigrab des eigenen Fensters 5080 von vorn Clip 12, 5090 Schnitt 11 Clip 12
  und 5080 Clip 11 (schwerste Stelle). Hoechststand 7184 / 7718 / 7415 / 7718 / 7184 / 7596, verworfen 0 in allen sechs Laeufen, kein
  Absturz, "Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips". messung/spiel_dreiecke.json. Fensterbilder geo/spiel/r2_*.png (selbst
  angesehen: Form im Bild, Leon schiesst, Huelse sichtbar; 5090 mit Feuer und Annette).
- Riegel `python generator/riegel_k5g3.py` (geo/riegel_k5g3.txt): **"RIEGEL K5g3 GEOMETRIE: 11 von 12 gruen, 15 von 15 Gegenproben rot
  wie verlangt"**. Neu: R3 trennt Geometrie (== B4) und UV (== uvfein(B4)), Gegenprobe B4-UV; R10 UV-Stufe (Kinder in der Mutter-UV,
  Linien-Dreiecke je Netz <= B4 und gesamt <= 1/4 B4, Augen-Texel == K5e_f1), Gegenprobe B4-UV. ROT bleibt R8 aus demselben Grund wie in
  Runde 1 (K5c-Krallen in der Bild-2-Haltung, G2; R8b gruen).

### 9.6 Fuer den Textur-Bauer (Runde 2)

- mulde_auf_form mit `tabelle=kf_k5g3_geo.GEO3_SPALT_UV` (geo/spalt_uv_tabelle.json) statt GEO3_SPALT, sonst bekommen die Kinder wieder
  die B4-UV (kf_k5g3.py:94; geo_schluessel vergleicht keine UV und merkt das nicht). Pruefbar: die Kinder-UV des Ergebnisses minus
  Verschiebung der Mutter == spalt_uv_tabelle.json.
- Kopie-Maske: kf_k5g2_tex.flaeche_maske nimmt das Polygon (0,1,3,2); fuer 73 Vierecke schneidet es sich selbst, bei 48 liegen
  Render-Texel mehr als 1 Texel ausserhalb (9.3). Maske = Vereinigung der Render-Dreiecke (0,1,3) + (0,3,2) behebt den weissen Strich auf
  10|1|76.
- Augen: 22|0|82/83 sind in K5e_f1 UV-entartet (v = 72) - nur ein Strich moeglich; fuer zwei Augenpunkte eigene Texel mit eigener,
  flaechiger UV.

### 9.7 Offen

1. R8 rot (K5c-Krallen, Bild-2-Haltung) wie Runde 1 - wartet auf die Posen-Datei der rechten Hand (G2).
2. 225 Linien-Dreiecke bleiben (K5e_f1 28); 49 Kinder lesen bis zu 8 Texel ausserhalb ihrer Mutter-UV (duenne Muetter, z. B.
   10|1|140; dort ergaebe jede Wahl im Gebiet eine Linie). Beides ist B4-Rest, kein neuer Fehler.
3. urteil4 H8 meldet "uv_entartet 1463" fuer Netz 22, weil es die B4-Tabelle prueft, nicht die EMD (urteil.sh uebergibt die Tabelle);
   die EMD hat 186 (R10).
4. Git schreibt spalt_uv_tabelle.json beim Auschecken ggf. mit CRLF am Schluss; `pruef` vergleicht darum ohne das letzte Zeilenende.
