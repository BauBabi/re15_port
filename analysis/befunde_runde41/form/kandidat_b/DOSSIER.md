# Kandidat B (Rumpfnaht) - Dossier

Stand 2026-10-31, Abgabe. Deutung B: kein eigener Kopf, die Figur ist die Kuppel; Stapel Z1-Z4 ist ein senkrechtes Maul (Naht) auf der Rumpffront; Haken ist die Schulter S.
Kennung wie im Formblatt: gemessen / frei. Bilder selbst angesehen: `vergleich_bild1.png`, `vergleich_bild2.png`, `ausschnitt_bild*.png`, `rundum.png`, `spiel_*.png`.

## 1. Ergebnis

| Mass (form_pruefung) | Bild 1 | Bild 2 | Ziel |
|---|---|---|---|
| IoU auf sicheren Pixeln | 0,996 | 0,999 | Bild 1 >= 0,90 |
| Weiss gedeckt | 0,996 | 1,000 | |
| Schwarz getroffen | 17 px | 12 px | |
| Pruefpunkte | 33/33 | 7/7 | alle |
| Boden, Verdeckung | 2/3 | 9/9 | alle |
| Zeichnung | 9/9 | 7/7 | alle |
| Verbote | 1/1 | 1/1 | alle |
| NCC im Weiss, Filter gegen Video | 0,751 | 0,860 | Anzeige |

**Nicht bestanden:** L2 in Bild 1, 2 von 633 px, 1,0 px tief, Stellen (312,349) und (314,350). Das ist die Spitze von L2 in der Taille von Bein NR. Der Pruefpunkt `nr_y346_links` verlangt dort die Kante bei x<=311, L2 verlangt sie 3 Zeilen tiefer rechts von x=314. Mit rund 50 Flaechen je Glied gelang beides nicht zugleich (Feinrunden pendelten, Lage und Gier in Bild 1 ohne Besserung abgesucht, `zw/lage1.py`).

Rahmen: 1890 Flaechen (1702 Vierecke, 188 Dreiecke), 21 Knochen, Textur 384x256 8 bpp, 1 CLUT, 3 Clips je 1 Bild, tiefster Punkt je Pose 0,0 / 0,4 / 0,0. `FE.pruefe`: 0 Fehler, 1 Warnung (Tick-Clips fehlen, gewollt, kommt in der naechsten Stufe).

## 2. Sichtfrage: gemessen, Sicht Ende

Figur starr wie Pose 1, nur Bein FL wie Clip 1, Lage und Gier per Suche am fertigen Netz (`zw/end_sicht.py`, `zw/end_sicht.json`).

| Sicht | Gier | Weiss fehlt | Schwarz | Haken gedeckt | Naht in Bild 2 sichtbar | Linien Q in Bild 1 sichtbar |
|---|---|---|---|---|---|---|
| Flanke | 996 | 1729 | 708 | 0,70 | 10/10, dort Video Y 18-39 | 113/146, dort Video Y 57 |
| Ende | 2065 | 532 | 1030 | 0,92 | 0/10 | 0/150 |
| Ende (zweiter Start) | 2130 | 878 | 490 | 0,87 | 0/10 | 0/150 |
| Gegenflanke | 3000 | 2097 | 406 | 1,00 | 0/10 | 0/154 |

- Sicht Flanke scheidet fuer Deutung B aus: Ohne Vorsprung stuende die Naht mit Z1 sichtbar auf der Kuppel, das Video ist dort dunkel. Q1 fiele in Bild 1 auf dunkle Haut oberhalb von Z1.
- Sicht Ende mit starrer Figur: Bein FR liegt auf U, Schulter S auf dem Haken, die Haende NL und NR liegen hinten unter dem Bauch und treffen V3 und die Kante G, ab y=380 bleibt Boden frei. Breite gerechnet: Pfeiler 600 Einheiten bei Tiefe rund 5000 ergeben 54 px, V2 misst 53 px.
- **Damit gilt 6 F, zweite Zeile: Haken = Schulter S.** Clip 1 ist Clip 0 bis auf Bein FL und kleine Winkel an Bein FR (rund 12 Grad).
- Einwand aus dem Formblatt bleibt offen: Bein FL stuende mit der Pose aus Bild 1 auf hellem Boden. Es ist deshalb in Clip 1 angehoben und schwingt nach vorn. Das ist gesetzt, nicht gemessen.
- Zweiter Einwand bleibt: Bein FR kreuzt im Render das helle Band (210-250,376-384). Im Video ist es dort hell. Die Flammen des Raums liegen im Spiel an dieser Stelle davor, das Band selbst ist nicht erklaert.

Die Silhouette allein trennt die Sichten nicht (Bestaetigung von FORMBLATT 6B): Mit freien Gliedern erreichen alle Gierwerte IoU >= 0,995 (`zw/p2a_erg.json`, Stand D2). Starr ohne Glieder-Freiheit: `zw/sichten.json`.

## 3. Bau

Lage: `lage.json`. Vorderseite +X = Ende N. Bindpose = Pose 1, Clip 0 ohne Gliederwinkel.

| Knochen | Name | Eltern | Bild 1 |
|---|---|---|---|
| 0, 9, 10 | rumpf_f, rumpf_m, rumpf | Wurzel, 0, 9 | Kuppel R, ein Netz, nach dem Remesh in drei Stuecke mit Ueberlappung geteilt |
| 15, 20 | maul_l, maul_r | 10 | Lippen, dazu Querwulst Z2 (15), Z3 und Z4 (20) |
| 11, 12, 13, 14 | nl_ober, nl_unter, nl_faust, nl_innen | 9, 11, 12, 12 | B1, Handwurzel, Lappen B2/B3, Innenglied B4 |
| 16, 17, 18, 19 | nr_ober, nr_unter, nr_faust, nr_innen | 9, 16, 17, 17 | C1, C3, C4, innerer Finger |
| 5, 6, 7, 8 | fr_ober, fr_unter, fr_fuss, fr_zeh | 0, 5, 6, 7 | Schulter S, Pfeiler A1, Fuss A2 |
| 1, 2, 3, 4 | fl_ober, fl_unter, fl_fuss, fl_zeh | 0, 1, 2, 3 | verdeckt |

Pflichten: 10 = Rumpf, 13 und 18 = Enden der Schlagglieder, Eltern vor Kindern. Glieder sind geschlossene Netze mit runden Enden am gemeinsamen Gelenkknoten, Beugen reisst dort keine Loecher.
Knochen 10, 15, 20 haben einen um 14,8 Grad gedrehten Netzrahmen: Lokal +X liegt auf der Naht, der Biss-Versatz (1400,0,0) trifft sie. Ihr Winkel ist daher in allen Clips -168 (Engine-Einheiten), nicht 0.
**Gemessen:** Z1-Z4 liegen auf der Kuppel bei Azimut 15 Grad rechts der Beinachse, zur Mitte (-420,-100). Die Naht ist also keine Mittellinie des Fussrahmens, sondern steht zur Kamera von Bild 1. Das Ueberlappen der Kuppelstuecke (170 Einheiten) traegt rund +-9 Grad Drehung von Knochen 10, mehr reisst auf.

Textur: `textur.png`. Haut in sechs harten Stufen aus der Palette. Dunkle Flaeche, Video gegen Filter: Bild 1 (32,24,40) gegen (26,20,35), Bild 2 (39,6,16) gegen (28,5,10), dort also rund 11 Stufen Rot zu wenig. Zeichnung aus den Sollflaechen, Ton je Stelle am Bericht nachgezogen; Mitteltoene aus Bild 1 als Albedo (Spielbild durch Licht, `video_umkehr`) in harte Stufen gesetzt, nur innen und oberhalb y=395.

Y Video / gefilterter Render: Z1 142/134, Z2 130/106, Z3 172/197, Z4 141/129, Z5 75/70, Z6a 134/143, Z8 96/103, Q1 58/54, Q2 32/31, Q3 50/51, Q4 96/80, Q5 34/41, G 113/106, P 116/96.

## 4. Frei gesetzt (nicht aus den Bildern)

Bein FL ganz: Lage, Haltung, Hub in Clip 1, gespiegelte Zeichnung von Bein FR (nur was Bild 2 nicht sieht). Seite L und Ende F der Kuppel ausser Zug Q. Unterseite. Lippenform, Rinnentiefe, Querwuelste als Form (die Stellen selbst sind gemessen). Zwei Finger je Hand: Der Bild-Lesung nach zwei Massen (B2/B3 und B4), an Bein NR ist der innere Finger nur Annahme. Fussform mit Ferse und drei Zehen. Ganzer Ruhe-Clip 2: Bein NR und FL auf die Spiegellage von NL und FR gezogen. Hautmuster: Straenge, Querfalten, Knoten, Adern, gemalte Nahverschattung, alle Stufen frei verteilt. Kuppelmass: Radius 1050, Mitte X -420 ist gewaehlt, damit die Pose ohne Fussversatz in Bild 2 passt; Spanne im Formblatt 1750-2300 Breite.
Nicht gebaut: Gegenstand X, Block C5, V-Form F, Bodenschatten.

## 5. Was im Bild noch abweicht

Bild 1:
- Untere Massen B2/B3, C3, C4 sind im Render flacher und dunkler. Das Video hat dort mehr kleine helle Flecken und blaue Kanten, darum NCC nur 0,75.
- Kuppelfront: Im Video zwei Massen mit Furche und Z1 darin, im Render liest sich die Furche schwaecher.
- Z3 zu hell (197/172), Z2 zu dunkel (106/130). Z7 steht mit 136/101 im Bericht, das Polygon liegt zu 38 % neben der Figur auf Boden, Ton von Hand gesetzt.
- Z6b 117/72 zu hell, die Stelle ist offen und zaehlt nicht.
- L2 siehe oben.

Bild 2:
- V-Form F und die Fortsetzung von G nach unten fehlen, dort zeigt der Render Boden.
- G ist glatter als im Video, die Segmente sind nicht gemalt.
- Bein FR ist unten breiter und dunkler als die Flaeche Y, ragt bis unter das Logo.
- Haken: Rand und Inneres gemalt, im Video heller.

Spiel:
- `spiel_pose2_room5090_cut11.png`: Figur und Zug Q sitzen wie im Video, Flammen stehen links, rechts und unten aehnlich. Der Schatten ist noch der alte Festwert 2300 x 1300 und zu gross.
- `spiel_pose1_room5080_cut1.png`: Bild 1 hat keinen Schnitt. Schnitt 1 ist der einzige, der die Lage sieht, Abstand rund 9400. L0 hellt die Oberseite stark auf, die Hautstufen treten dort klarer hervor als im Video.
- Beide Bilder mit `--env SDL_RENDER_DRIVER=software`, Verfahren PrintWindow: Die Sitzung war getrennt (`qwinsta`), mit direct3d11 kamen sechs von sechs Versuchen schwarz. Der Schatten erscheint dadurch als Rechteck mit Verlauf (ABR2 fehlt dem Software-Treiber). Der Beschleuniger-Pfad ist mit diesem Stand **nicht** gesehen, nur Laden und Pose im Log: `Bahnsteig-Form: 21 Meshes, 21 Knochen, 3 Clips`, `typ 0x30 ... clip 1 bild 0`.
- Sortierfehler (Malerreihenfolge) sehe ich in diesen zwei Bildern nicht. Andere Blickwinkel sind nicht im Spiel geprueft.

## 6. Durchgaenge

| Nr | Stand | Bild 1 | Bild 2 | Abweichung, die den naechsten Schritt ausloeste |
|---|---|---|---|---|
| D1 | `zw/v0`, Kette steht | IoU 0,999, 26/33 | | Klumpen, Glieder ohne Zusammenhang, Atlas 0,024 Texel je Einheit |
| D2 | `zw/v2`, Knoten geteilt, Sicht Flanke, NL erhoben | 21/33 | alles bestanden | Z1 sichtbar bei (263,252), Video dunkel. Daraus die Sichtmessung |
| D3 | `zw/v3`, `v4`, Sicht Ende, Pose starr, FL angehoben | IoU 0,994, 23/33 | 6/7 | Kuppel Mitte X -420. Falle: Netz am Gitterrand offen, Voxel-Remesh lieferte 6 Flaechen |
| D4 | `zw/g5`, Naht, Lippen, Eckenzug | 0,995, 29/33 | 3/7 | Einzelecken ergaben Dornen, ersetzt durch weiche Verschiebung |
| D5 | `zw/g6`, Schultern in die Kuppel, Furche | 0,981, 29/33 | alles bestanden | Taille NR, Fuesse zu klein |
| D6 | `zw/g7`, Gliederbau mit Mindestradien, Fuesse, erste Textur | 0,995, 31/33 | 7/7, 7/9 | Risszellen wirkten wie Stein, Ringe zu regelmaessig |
| D7 | `zw/g7`, Muskelstraenge, Feinrunden am echten Bericht | 0,997, 33/33 | alles bestanden | L2 65 px, Haken ohne Inneres |
| D8 | `zw/g9`, Nahverschattung, Hakeninneres, Zug Q, Ruhe gerade | 33/33, NCC 0,79 | NCC 0,89 | L2 3 px |
| D9 | `zw/g10`, Abgabe, Bein FL hinter C1 geschoben, 1890 Flaechen | 0,996, 33/33, L2 2 px | alles bestanden | siehe Abschnitt 5 |

D9 ist in Bild 1 bei NCC schlechter als D8 (0,75 gegen 0,79). Gemessen in den Feinrunden: 0,777 vor den Zuegen an der L2-Spitze, 0,744-0,754 danach. Die 12 px weniger Treffer in L2 kosteten das, abwaegen, falls L2 nicht zwingend ist.

## 7. Wiederholen

```bash
cd analysis/befunde_runde41/form/kandidat_b
python b_fit.py --aus zw/fit6 --iter 1200 --gier 2100 --x 2700 --z -23850   # Volumen, Pose 2
python b_alles.py --fit zw/fit6.pt --tag 10                                  # Netze, Blender, Eckenzug, Malen, Bericht, Feinrunden
```

`zw/fit6.pt` ist der abgegebene Fit. Er entstand vor der Gewichtung 3 auf Schwarz in `lade_masken`, ein neuer Lauf von `b_fit.py` trifft ihn nicht genau. QuadriFlow und Feinrunden liefern je Lauf andere Netze, das Abgabenetz liegt in `zw/g10/quad` + `zw/g10/snap_fein.pkl`. Letzte Textur: `python b_alles.py --fit zw/fit6.pt --tag 10 --ab fein --runden 1`.

Dateien: `b_teile.py` Knochen, Knoten; `b_fit.py` Strahlen-Fit; `b_naht.py` Naht; `b_mesh.py` Marching Cubes; `b_blender.py` Voxel-Remesh, QuadriFlow, Smart-UV; `b_snap.py`, `b_fein.py`, `b_lage2.py` Umriss; `b_pack.py` Atlas; `b_mal.py`, `b_mal2.py` Textur; `b_ruhe.py` Clip 2; `b_bau.py`, `b_lauf.py`, `b_alles.py` Bau.

Keine Datei ausserhalb von `kandidat_b/` geaendert, ausser Laufverzeichnissen unter `re15_port/build/form_spiel/`. Kein Commit.
