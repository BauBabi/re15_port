# Stapel 2 - Gegenpruefung (ROOM1100..ROOM11B0)

Runde 31, T3. Gegenpruefer zu `stapel_2.json` (Zuordner). Kein Spielcode, kein Commit. Daten: `stapel_2.pruefer.json` (je Seite `seite/vorher/urteil/nachher/begruendung/bilder`; `nachher` enthaelt nur geaenderte Felder).

Vorgehen: jedes `gleich` am RE1.5-Einzelbild (aus + entz, bei dunklen Bildern aufgehellt bzw. Vollbild-Ausschnitt 4..8-fach) neben die DOORxx-Textur UND die von RE2 gemalten Ausschnitte gelegt (entzerrte Blaetter nebeneinander, Feldhoehen verglichen); Griffseite/-form im Bild selbst abgelesen. Alle `aehnlich`/`keine` (alle haben Top-5 >= 0.50 oder Sicherheit <= 2) gegen die Uebersicht der 55 geprueft; Querabgleich mit Stapel 1/3/4 und Pruefer 1 bei Gegenseiten und gleichen Tuertypen.

## Ergebnis

| | Anzahl |
|---|---|
| geprueft | 43 (alle 43 Seiten) |
| bestaetigt | 34 |
| widerlegt | 4 (S088, S089, S090, S091) |
| korrigiert | 5 (S058, S060, S061, S065 Griff-Spender; S095 aehnlich -> gleich DOOR16 V5) |

Seiten vorher gleich 30 / aehnlich 11 / keine 2 -> **nachher gleich 27 / aehnlich 14 / keine 2**.
Tueren (24) vorher 15 gleich / 8 aehnlich / 1 Tor -> **nachher 13 gleich / 10 aehnlich / 1 Tor (eigene Sequenz)**.
- neu nicht abgedeckt: T047 (Leiter S088), T049 (S089+S090), T050 (S091); neu abgedeckt: T052 (Kanaldeckel S095, Gegenseite S128 in Stapel 3 schon gleich DOOR16).
- gleich bleiben: T024 (DOOR16) T030 T031 T032 T044 (DOOR19) T033 T034 (DOOR09) T036 T037 T043 T051 (DOOR1A) T042 (DOOR13) T052 (DOOR16).
- nicht abgedeckt (aehnlich): T015, T018, T022, T026, T035, T045, T047, T048, T049, T050.

## Was sich aendert (fuer den Leiter)

1. **DOOR22 faellt fuer die Kanaltueren (S089/S090/S091, T049/T050).** Entzerrt neben DOOR22 gelegt: kein breiter Nietrand (9 % je Seite, bei ~50 px Blattbreite als Tonstufe sichtbar), kein aufgemalter D-Riegel, stattdessen duenner Druecker rechts auf schlichtem braunem Blatt. Der DOOR22-Riegel ist TEXTUR (kein Griff-Mesh) - der vorgeschlagene Tausch (DOOR07-Druecker) haette zwei Griffe gezeigt. Stapel 4 lehnt DOOR22 fuer den gleichen Tuertyp (S143) aus demselben Grund ab. Fuer T050 ist die Gegenseite S167 (Stapel 4) aehnlich DOOR23 - gleich DOOR22 haette die Tuer gespalten.
2. **Leiter S088 (T047) nur aehnlich DOOR16:** dunkle rost-/graubraune Leiter mit dicken Flachholmen und Vierkant-Sprossen im gruenen Licht; DOOR16 und alle RE2-DOOR16-Leitern (ROOM3090 c0, ROOM4090 c3, ROOM4100 c7) sind duenne Silberrohr-Leitern. Gegenseite S173 (Stapel 4) ist aus demselben Grund aehnlich.
3. **Kanaldeckel S095 (T052) gleich DOOR16 V5** statt aehnlich DOOR28: offener runder Bodenschacht mit Deckel daneben = RE2 ROOM21A0 c3 (Rundschacht -> DOOR16 V5, `t2/hg/paar217_DOOR16_21A0_3020.png`); identischer Fall S038 von Pruefer 1 bestaetigt. DOOR28 ist die eckige Riffelblech-Luke. Automatisches Mass Top-1 = DOOR16 0.68.
4. **Griff-Spender der 3F-Kassettentueren (T033/T034) DOOR04.m1 statt DOOR2F.m1:** RE1.5-Griff = silbrige Stange zwischen zwei Messing-Haltern, 33..35 % der Blatthoehe (S058 c03 143/436, S061 c06 152/436). DOOR04.m1 "Stangengriff lang" 1755/6602 = 27 % mit Messing-Haltern; DOOR2F.m1 schlichter Kupferstab 1369/6602 = 21 %. Der Familienvorteil 06/0B/0C/2F greift fuer DOOR09 nicht (DOOR09.m1 ist Knauf B) - jeder Spender wird mit seinem TIM gezeichnet (T2 §3).

## Offene Regelfragen (stapeluebergreifend einheitlich entscheiden)

- **Leiter-Farbe/Profil:** S037 (Stapel 1, Pruefer 1) und S128 (Stapel 3) gleich trotz Flachprofil bzw. Rost-/Messingfarbe; S150/S168/S169/S172/S173 (Stapel 4) und hier S088 aehnlich. Haengt direkt an T024 (S069 folgt S037) und T052 (S095 + S128). Faellt die Regel streng, werden T024 und T052 aehnlich (dann 11 gleich / 12 aehnlich in diesem Stapel).
- **Zwei verschieden gemalte Seiten einer Tuer:** T048 (S087 Maschendraht-Drehtuer, aehnlich DOOR14; S159 Staebe, Stapel 4 gleich DOOR0A). RE2 nimmt je Tuer EIN Archiv fuer beide Seiten - T048 hier als nicht abgedeckt gezaehlt.
- **Griff beidseitig gleich gemalt:** T034 (beide links) und T044 (beide rechts) im Bild bestaetigt (S065 ueber 4-fach-Ausschnitt: Griff an der fernen = linken Kante). Je Seite eigene Variante (T2 2.4).

## Je Seite

| Seite | vorher | Urteil | nachher | Kurzbegruendung |
|---|---|---|---|---|
| S049 | aehnlich (naechstes DOOR1B, s=3) | **bestaetigt** | - | c00: zweifluegelige glatte graue Stahltuer ohne Felder/Fenster, zwei Druecker an der Mittelfuge. Doppeltuer-Archive 0C (gruenes Holz+Glas), 1B (braunes Blech mit Sichtfenster + Druckstange), 30 (Glasrahmen) und Einzelfluegel-Archive 01/04/06/15/1D V2/V3, 11... |
| S050 | gleich DOOR19 | **bestaetigt** | - | c06 frontal: Wellen-Oberkante mit drei Boegen, zwei waagerechte Schlitze rechts, Drehscheibe (Schlitzscheibe) auf erhabener Platte links = DOOR19-Textur + Beschlag (Scheibe mit Schlitz) + RE2 ROOM5070 c0 gemalt (Scheibe auf Platte links). Grundton dunkelgra... |
| S051 | gleich DOOR19 | **bestaetigt** | - | c00: Rueckseite von S050, Wellenkante, zwei Schlitze links, Drehscheibe auf Platte rechts; c05/c06 zeigen nur Regal (Umriss verdeckt). Griff rechts abgelesen, stimmt. |
| S052 | gleich DOOR19 | **bestaetigt** | - | c00/c06: Wellenkante, Schlitze rechts, Drehscheibe auf Platte links, rote Lampe ueber der Zarge (Wand). Griff links abgelesen, stimmt. |
| S053 | gleich DOOR19 | **bestaetigt** | - | c07 frontal: drei Wellenboegen oben, zwei Schlitze links, Drehscheibe auf Platte rechts = DOOR19 gespiegelt (V1). Griff rechts abgelesen, stimmt. |
| S054 | gleich DOOR19 | **bestaetigt** | - | c05/c06: Wellenkante, Schlitze rechts, Drehscheibe links, rote Lampe darueber. Griff links abgelesen, stimmt. |
| S055 | gleich DOOR19 | **bestaetigt** | - | c10 frontal: Wellenkante, Schlitze links, Drehscheibe auf Platte rechts. Griff rechts abgelesen, stimmt. |
| S056 | aehnlich (naechstes DOOR07, s=3) | **bestaetigt** | - | c00: glattes graues Stahlblatt ohne Felder/Fenster/Lueftung, Druecker links, Tuerschliesser oben. DOOR07 hat Lueftungsschlitze unten; Top-5 (05, 00, 08, 11, 12) Holz/genietet/Lamellen/Treppe. Kein merkmalloses Blatt unter den 55 (wie Pruefer 1 fuer S000..S0... |
| S057 | aehnlich (naechstes DOOR25, s=2) | **bestaetigt** | - | c00: Aufzugtuer in tiefer Nische, glattes graues Blechfeld, Tasterfeld rechts in der Nische. RE2 malt DOOR25 selbst mit gut sichtbarem Schild 'SHAFT TYPE-L' und Warnaufkleber (ROOM6140/6080/6110 c0), DOOR29 hat Bedientafel am Blatt. Top-5 (00, 16, 08, 06, 2... |
| S058 | gleich DOOR09 (Griff-Tausch DOOR2F) | **korrigiert** | griff_tausch=DOOR04 | Gestaltung bestaetigt: c03 entzerrt neben DOOR09 gelegt -> sechs Kassetten 2 x 3 mit Mittelfries, Reihenhoehen deckungsgleich (je ca. 25 % mit gleichen Friesen), Holz, Olivton (DOOR18 olivgruen hat nur 4 Felder, DOOR03 Rautenfuellung, DOOR04 blau). Griff re... |
| S059 | aehnlich (naechstes DOOR04, s=2) | **bestaetigt** | - | Nur steil sichtbar; Gestalt ueber S063 (Doppeltuer, braunes Holz, je Fluegel 2 x 3 Kassetten, lange Messing-Stangen an der Fuge). DOOR04 hat Aufteilung + Stangengriff + Doppelvarianten V2/V3, ist aber blau gestrichen; DOOR09 hat den braun/oliven 2 x 3-Flueg... |
| S060 | gleich DOOR09 (Griff-Tausch DOOR2F) | **korrigiert** | griff_tausch=DOOR04 | c02 (steil, linke Wand): 2 x 3 Kassetten entzerrt erkennbar, Stangengriff an der nahen Kante; linke Umrisskante laenger = nah, Griff im Bild links am Blatt -> vor der Tuer links. Gestaltung + Griff links/stange bestaetigt; Spender wie S058 auf DOOR04.m1 (la... |
| S061 | gleich DOOR09 (Griff-Tausch DOOR2F) | **korrigiert** | griff_tausch=DOOR04 | c06 frontal + c09: 2 x 3 Kassetten, Messing-Halter oben/unten mit Stange an der linken Kante (152/436 = 35 % Blatthoehe), 'Chief Office'-Schild (c05). Gestaltung + Griff links/stange bestaetigt; Spender DOOR04.m1 statt DOOR2F (Begruendung S058). |
| S062 | gleich DOOR1A | **bestaetigt** | - | c07 (schraeg) und entzerrt neben die DOOR1A-Textur gelegt: Stahlrahmentuer mit drei liegenden Feldern in nahezu gleichen Hoehen (ca. 35/33/30 % gegen DOOR1A 33/33/30 %), Druecker auf schmaler senkrechter Platte mit rundem Schloss darueber im Mittelfeld = DO... |
| S063 | aehnlich (naechstes DOOR04, s=2) | **bestaetigt** | - | c00 Vollbild: zweifluegelige braune Holztuer, je Fluegel 2 x 3 Kassetten, zwei lange Messing-Stangen an der Fuge, Zierrahmen. Doppel gegen Doppel: nur DOOR04 (V2/V3) hat sechs Kassetten + lange Stange, aber BLAU (unter warmem Raumlicht ist das RE1.5-Holz br... |
| S064 | gleich DOOR1A | **bestaetigt** | - | c06 frontal: drei liegende Felder, Druecker + rundes Schloss auf Platte rechts im Mittelfeld, Notausgang-Schild darueber; Feldhoehen entzerrt deckungsgleich mit DOOR1A. Griff rechts/druecker stimmt. |
| S065 | gleich DOOR09 (Griff-Tausch DOOR2F) | **korrigiert** | griff_tausch=DOOR04 | c00 Vollbild 4-fach: Tuer in der rechten Wand hinter der Wandlampe, extrem steil; Blatt von x~175 (fern) bis x~330 (nah, an der Lampe). Griff = silbrige Stange mit zwei gebogenen Messing-Haltern im fernen = linken Teil des Blatts -> vor der Tuer links (Zuor... |
| S066 | gleich DOOR1A | **bestaetigt** | - | c00 (dunkel): drei liegende Felder (oberes groesstes), Druecker + rundes Schloss auf Platte rechts; c03 zeigt nur das Bulletin-Brett (Umriss daneben). Griff rechts/druecker stimmt. |
| S067 | gleich DOOR1A | **bestaetigt** | - | Bilder identisch zu S066 (dieselbe Flaeche, anderes Ziel). Befund wie S066. |
| S068 | keine (Tor, eigene Sequenz, s=3) | **bestaetigt** | - | c00/c01: niedriges Rohrrahmen-Tor im Dachgelaender mit gelb-schwarz umrandetem Warnschild. Top-5 (1A 0.51, 25 0.51, 09 0.51, 2B 0.50) sind Tueren/Klappen; kein RE2-Archiv hat ein Gelaendertor (2D Hubbuehne hat nur den Warnrand). Eigene Sequenz aus analysis/... |
| S069 | gleich DOOR16 (Leiter, V5 hinab) | **bestaetigt** | - | Eigene Seite zeigt nur die Luecke im Dachgelaender (c02/c03/c04, Nacht, Leiter nicht sichtbar); Gestalt ueber die Gegenseite S037 (ROOM10B0 c00, Stapel 1, vom Pruefer 1 bestaetigt): einfache Steigleiter an der Hauswand, zwei gerade Holme, gerade Sprossen, S... |
| S072 | gleich DOOR1A | **bestaetigt** | - | c08 (steil von oben, Nacht): Stahlrahmentuer mit drei liegenden Feldern (Querriegel hell angeleuchtet), Druecker + Schloss an der linken Blattkante im Mittelfeld; c06 nur Silhouette. Griff links/druecker stimmt; komplementaer zu S062 (rechts). |
| S073 | gleich DOOR1A | **bestaetigt** | - | c09 (klein, dunkel): drei liegende Felder, kleine helle Beschlagmarken an der linken Kante im Mittelfeld; c06/c10 ohne Blatt (Umriss auf Treppe/Silhouette). Griff links/druecker stimmt; komplementaer zu S064 (rechts). |
| S074 | keine (Tor, eigene Sequenz, s=3) | **bestaetigt** | - | c11/c12: Gegenseite desselben Gelaendertors mit Warnschild. Top-5 alle <= 0.50. Wie S068. |
| S076 | gleich DOOR1A | **bestaetigt** | - | c00 frontal (tuerkises Licht): drei liegende Felder, Druecker + rundes Schloss auf Platte links im Mittelfeld, Leuchte ueber der Zarge; entzerrt Feldhoehen wie DOOR1A. Griff links/druecker stimmt (V0-Lage, Platte links wie in der DOOR1A-Textur). |
| S077 | gleich DOOR1A | **bestaetigt** | - | Bilder identisch zu S076 (dieselbe Flaeche, anderes Ziel). Befund wie S076. |
| S078 | aehnlich (naechstes DOOR07, s=2) | **bestaetigt** | - | c02: glattes dunkles Stahlblatt, Druecker links, Tuerschliesser, Notausgang-Leuchte. Wie S056 (kein glattes Archiv; DOOR07 mit Lueftung). Top-5 (06 X-Kreuz, 28 Luke, 05 Holz, 23 Achteckprofil, 12 Treppe) abweichend. aehnlich bleibt. |
| S079 | gleich DOOR13 | **bestaetigt** | - | c07 frontal: dunkle Stahltuer, oben grosses dunkles Fenster (Rahmen abgesetzt) mit kleinem hellem Schild mittig oben, unten glattes Feld, runder Knauf an der linken Kante genau an der Teilung = DOOR13 (Fenster + Schildleiste auf der Textur; Knauf B Anhaenge... |
| S080 | aehnlich (naechstes DOOR1B, s=2) | **bestaetigt** | - | c08 mit Gamma 0.45 aufgehellt, 8-fach: Doppeltuer, zwei Druecker gegenlaeufig an der Mittelfuge, Fluegel mit Stahlrahmen und Querriegel (Feldaufteilung wie DOOR1A, aber zweifluegelig). Kein Doppeltuer-Archiv mit fensterlosen Stahlrahmen-Feldern (1B Sichtfen... |
| S081 | gleich DOOR19 | **bestaetigt** | - | c06 (linke Flurwand, steil, sehr dunkel; mit Helligkeit x2.5 geprueft): zwei waagerechte Schlitze an der linken Kante, Drehscheibe auf erhabener Platte rechts, Leuchte ueber der Zarge, 'Fire Shutter'-Schild links an der Wand = DOOR19 in V1-Lage. Linke Umris... |
| S082 | gleich DOOR13 | **bestaetigt** | - | c00/c01: gleiche Aufteilung (Fenster oben mit Schildleiste, unteres Feld mit abgesetzter Oberkante), Knauf rechts an der Teilung. Blatt hellgrau unter dem hellen, blaeulichen Raumlicht von ROOM1190 (Waende ebenso hell); Gegenseite S079 dunkel unter tuerkise... |
| S083 | gleich DOOR13 | **bestaetigt** | - | Bilder identisch zu S082 (dieselbe Flaeche, Ziel ROOM1230). Befund wie S082. |
| S084 | gleich DOOR19 | **bestaetigt** | - | c12 (klein, blaues Licht; Vollbild-Ausschnitt 8-fach mit Kontrast geprueft): zwei waagerechte Schlitze von der linken Kante bis gut zur Mitte (bei ca. 40 % und 73 % Hoehe), dunkle kleine Marke rechts bei ca. 65 % Hoehe (Scheibe/Platte), punktierte Oberkante... |
| S085 | gleich DOOR19 | **bestaetigt** | - | Bild identisch zu S084 (dieselbe Flaeche, Ziel ROOM1230); Befund wie S084. |
| S087 | aehnlich (naechstes DOOR14, s=2) | **bestaetigt** | - | c07: Tuer im braunen Rahmen, oberes Feld Rautengitter, Mittelriegel mit Druecker rechts, unteres Feld dunkel. Aufteilung wie DOOR14 (zwei Rautengitterfelder + Mittelriegel), aber DOOR14 ist ein SCHIEBE-Tor (Schieben z -3875, Griffplatte am Rand), die RE1.5-... |
| S088 | gleich DOOR16 (Leiter, V4 hinauf) | **widerlegt** | urteil=aehnlich, archiv=None, naechstes=DOOR16 | c00 neben die RE2-Leiterausschnitte gelegt (Profil, Staerke und Farbe weichen gleichzeitig ab; bei S037/S069 nur das Profil). Die Gegenseite derselben Tuer T047, S173 (Stapel 4), ist bereits aehnlich mit genau diesem Argument ('Leiter der Gegenseite dunkel/... |
| S089 | gleich DOOR22 (Griff-Tausch DOOR07) | **widerlegt** | urteil=aehnlich, archiv=None, naechstes=DOOR22, griff_tausch=None | c09 (gross, linke Wand) entzerrt und aufgehellt neben DOOR22/DOOR23 gelegt: kein breiter Rand, keine Nieten, kein D-Riegel; nur ein duenner dunkler Druecker rechts. Bei ca. 50 px Blattbreite waere der 9-%-Rand als Tonstufe sichtbar. Stapel 4 lehnt DOOR22 fu... |
| S090 | gleich DOOR22 (Griff-Tausch DOOR07) | **widerlegt** | urteil=aehnlich, archiv=None, naechstes=DOOR22, griff_tausch=None | c12: dieselbe schlichte braune Tuer wie S089 (Druecker rechts auf halber Hoehe, kein Rand/Nieten sichtbar); c05 nur Streifen. Zweite Seite derselben Selbst-Verbindung T049 -> gleiches Urteil wie S089. Griff rechts/druecker bestaetigt. |
| S091 | gleich DOOR22 (Griff-Tausch DOOR07) | **widerlegt** | urteil=aehnlich, archiv=None, naechstes=DOOR22, griff_tausch=None, sicherheit=1 | c09 frontal, nur ca. 16 px breit: braunes Blatt mit Randlinien, Griff rechts auf halber Hoehe; c11 hinter dem Regal verdeckt. Auf dieser Aufloesung ist gleich nicht belegbar (Sicherheit 1), und die gut sichtbare Gegenseite S167 ist ein anderer Typ (abgeschr... |
| S092 | aehnlich (naechstes DOOR1B, s=2) | **bestaetigt** | - | c00 Vollbild (Gamma 0.6): an der linken Wand sehr steil eine Stahlrahmentuer mit hellen Drueckern in der Mitte, T1-Umriss liegt davor auf der Wand. Gestalt ueber S080. Top-5 (18 0.58 Holz 4 Felder, 26 0.51 Schott) abweichend. aehnlich bleibt. |
| S093 | aehnlich (naechstes DOOR1B, s=2) | **bestaetigt** | - | Bilder identisch zu S092 (dieselbe Flaeche, Ziel ROOM1230). Wie S092. |
| S094 | gleich DOOR1A | **bestaetigt** | - | c01 Vollbild: T1-Umriss nur als duenner Streifen an der linken Tuerkante, die Tuer selbst steht frontal in der Stirnwand hinter den Pylonen: Stahlrahmentuer, drei liegende Felder, Druecker + Schloss auf Platte links im Mittelfeld = DOOR1A. Griff links/druec... |
| S095 | aehnlich (naechstes DOOR28, s=2) | **korrigiert** | urteil=gleich, archiv=DOOR16, naechstes=None, sicherheit=2 | c06 Vollbild: offener runder Schacht im Garagenboden, Deckel liegt daneben (c05/c11 Umriss an der Wand bzw. hinter dem Gitter). Identische Lage wie S038 (Stapel 1, Pruefer 1: 'offener runder Schacht im Boden ... Deckel daneben - identisch mit RE2 ROOM21A0 c... |

Vollstaendige Begruendungen, Abweichungstexte und Bildpfade: `stapel_2.pruefer.json`. Hilfsbilder (nicht im Repo) im Scratchpad-Unterordner `pr2_stapel2/`.
