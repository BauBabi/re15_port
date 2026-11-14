# K5g3 Geometrie-Stufe (Runde 1) - Arbeitsnotiz

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
