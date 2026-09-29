# T3 — Zuordnung RE1.5-Tueren -> RE2-Tuerarchive (Zusammenfuehrung)

Runde 31, Teil T3 (Leiter der Zuordnung). Zweig `r31/tueren`, Arbeitsbaum `.claude/worktrees/r31_tueren`.
Kein Spielcode, kein Commit. Stand: 2026-09-29, **fertig**.

Ergebnisdatei: `analysis/befunde_runde31/tueren_03/zuordnung.json` (je physischer Tuer: Status, Grund, Archiv,
je Seite Variante + Herkunft, Griff, Griff-Tausch, Tonfamilie, Port-Schluessel). Werkzeuge (nur lesend; sie schreiben
nach `build/r31_tueren/t3/` bzw. in die zwei Ergebnisdateien): `build/r31_tueren/t3/t3_zuordnung.py`,
`build/r31_tueren/t3/t3_boegen.py`, `build/r31_tueren/t3/t3_md_tabellen.py`, `build/r31_tueren/t3/t3_dossier.py`.
Alle Wahlen hier sind PORT-WAHL aus dem Bildvergleich, keine Original-Adresse.

## Kurz — die Antwort an den Nutzer

**144 physische Tueren. 83 davon haben eine GLEICHE RE2-Tuer und bekommen die RE2-Animation mit Ton.
61 Tueren lassen sich so nicht abdecken:**

| Grund | Tueren | Tuerseiten |
|---|---|---|
| aehnlich (gleiche Klasse, mindestens ein Merkmal weicht ab; naechstes Archiv genannt) | **45** | 97 |
| einseitig (eine Seite gleich, die Gegenseite ist anders gemalt) | **3** | 3 (die 3 gleichen Seiten sind baubar und bei "abgedeckt" mitgezaehlt) |
| Aufzug (Einstiegstuer ohne gleiches Archiv; die Fahrt ist eigene Skript-Szene) | **5** | 5 |
| keine Tuer (Durchgang ohne Blatt, Nische zum Hochklettern) | **3** | 4 |
| Tor ROOM1170 (eigene Sequenz, schon gebaut) | **1** | 2 |
| inert / unsichtbar (sce 0 bzw. nichts gemalt) | **4** | 4 |
| keine Entsprechung (Tuerblatt, zu dem RE2 gar nichts Vergleichbares hat) | 0 | 0 |
| **Summe nicht abgedeckt** | **61** | **115** |

Tuerseiten: **299 begehbar, 184 abgedeckt** (181 in den 83 abgedeckten Tueren + die 3 gleichen Seiten der
einseitigen Tueren). Dazu 6 Skript-Seiten (Flaeche 0, Aufzugkabinen ROOM1080/4020), die nie eine Tueranimation haben.

Die 45 "aehnlich" nach naechstem Archiv: DOOR07 12 (die glatten dunklen Polizeirevier-Tueren STAGE1), DOOR23 7,
DOOR08 6 (Fabrik-Stahltueren STAGE3), DOOR1B 5 (Doppeltueren), DOOR16 4 (Messing-/Rostleitern), DOOR13 3, DOOR1E 2
(Lueftungen), je 1: DOOR04, DOOR1D, DOOR22, DOOR24, DOOR26, DOOR27. Davon sind 39 echte Tuerblaetter, 4 Leitern,
2 Lueftungen.

Abgedeckt je Archiv: DOOR25 12, DOOR29 11, DOOR26 9 (+1 DOOR26/DOOR31), DOOR1A 6, DOOR13 5, DOOR16 5, DOOR19 5,
DOOR1C 5, DOOR15 4, DOOR2A 4, DOOR27 3, DOOR06 2, DOOR09 2, DOOR24 2, DOOR2E 2, DOOR0A 1, DOOR1B 1, DOOR1D 1,
DOOR23 1, DOOR2D 1. Griff-Tausch an 3 Tueren (5 Seiten): T027 (DOOR13, Spender DOOR07), T033/T034 (DOOR09,
Spender DOOR04).

## 0. Eingaben

- `build/r31_tueren/t1/zensus.json` (T1: 163 Tueren, 144 physisch, 324 Seiten, davon 299 begehbar).
- `analysis/befunde_runde31/tueren_03/stapel_<1..7>.json` (Zuordner, 299 Seiten) und `stapel_<1..7>.pruefer.json`
  (Gegenpruefer: 299 geprueft, 277 bestaetigt, 9 widerlegt, 13 korrigiert). Das Pruefer-Urteil gilt (Felder aus
  `nachher` ueberschreiben den Zuordner), ausser wo ich in Abschnitt 3 selbst anders entschieden habe.
- T2-Regeln `analysis/befunde_runde31/tueren_02_re2.md` (Variante 2.4, Formfamilien 3, Tonfamilien 4, Anhang A1/A2).
- Tuerseiten, die es nur als Zwilling gibt (28, z. B. ROOM20B0 = geflutetes ROOM2000, ROOM1230 = ROOM1180 mit Licht),
  haengen ueber `seiten[].tuer` an ihrer Tuer und stehen dort als eigene Seiten (eigener Port-Schluessel).

## 1. Regeln der Zusammenfuehrung

1. **Seite:** Urteil nach Pruefer (gleich / aehnlich / keine), Griffseite + Griffform wie gesehen.
2. **Tuer abgedeckt** = alle Seiten MIT brauchbarer Ansicht sind gleich demselben Archiv. Eine Seite ohne brauchbare
   Ansicht zaehlt nicht dagegen und bekommt das Archiv der Tuer (S161, 3.3; S150 zeigt die Leiter nicht, 3.1).
3. **Einseitig** = eine Seite gleich, die Gegenseite hat eine brauchbare Ansicht und ist ANDERS gemalt. Nach RE2
   (238/238 Paare mit gleichem Archiv) gibt es dann kein Archiv fuer beide Seiten -> zaehlt als nicht abgedeckt; die
   gleiche Seite ist fuer sich baubar (Schluessel je Seite), steht in `einseitig_gleich`.
4. **Variante** (T2 2.4): Standardgruppe (26 Archive) und DOOR19 (Anhang A1: V0 Griff links, V1 rechts):
   Griff links -> V0, rechts -> V1. Griff unsichtbar -> Komplement der Gegenseite (RE2-Paar 0/1), Zwilling -> wie
   seine Hauptseite, auf keiner Seite sichtbar -> Seite A V0 / Seite B V1 (nur T059). Sonderarchive je Seite explizit:
   Schott 26/31 V0..V3 nach Teilung + Radseite, Leiter 16 V4 hinauf / V5 hinab, 25 V0, 27 V0 (V0..V3 gleich, RE2 0/0),
   2A V0 Riegel links / V1 rechts / zweifluegelig V2 (RE2-Paar 2/2), 1B Doppeltuer Seite A weg V3 / Seite B hin V2
   (RE2 nutzt 1B nur als 2/3), 1D Doppeltuer Seite A V2 / B V3, 1E V0, 2D V5 hinab / V4 hinauf. Die Herkunft steht je
   Seite in `variante_herkunft`; abgeleitete (nicht abgelesene) sind in Tabelle 4 mit `*` markiert.
5. **Griff-Tausch** nur, wo die gemalte Form nicht die des Archivs ist (T2 3): S042 Druecker auf DOOR13 (Knauf B) ->
   Spender DOOR07 (Druecker flach); S058/S060/S061/S065 langer Stangengriff auf DOOR09 (Knauf B) -> Spender DOOR04
   (Stangengriff lang; Pruefer 2 hat DOOR2F -> DOOR04 korrigiert). Beide Spender liegen in einer anderen Formfamilie
   -> Beschlagstreifen v 219..255 + Farben des Spenders mitnehmen (T2 2.5).
   Schreibweise vereinheitlicht: DOOR29-Bedientafel `unklar` (Stapel 5 schrieb `druecker`), DOOR19 `knauf`
   (Griffmulde mit Drehscheibe; Stapel 3 schrieb `griffmulde`), Handrad/Riegelkasten `unklar`, Doppeltuer-Beschlag an
   der Fuge `unklar`; die rohe Angabe steht in `griff_form_hinweis`. Kein Tausch daraus. DOOR2A hat kein Griff-Mesh
   (Drehriegel gemalt) - der "Knauf" von S321/S323 ist dieser runde Drehriegel, kein Tausch.
6. **Ton** = Tonfamilie des Archivs (T2 4): F2 09, F3 06, F4 15/1A/23, F5 1B, F6 24/29, F7 26/31; eigene Tonteile
   0A 13 16 19 1C 1D 25 27 2A 2D 2E (und 1E fuer die einseitige S146).
7. **Port-Schluessel je Seite** (`seiten[].schluessel`): alle Raumdateien der Seite aus `saetze[].raum` (xxx0/xxx1),
   Band pc[4], Rechteck [x, z, w, d] + Halbmass bzw. vier Punkte, Mitte, Ziel; die Satz-Offsets (`datei`, `off`,
   `slot`, `skript`) nur zur Herkunft - **Schluessel ist Raum + Flaeche + Band, nicht der Slot** (T2 6.2).

## 2. Vergleichsboegen (angesehen)

- `build/r31_tueren/t3/vergleich_abgedeckt.png` (84 Bloecke; T131 zweimal): je Tuer bis zu 2 RE1.5-Seiten
  (gruen beschriftet mit Variante + Griffseite) | RE2 gemalte Tuer desselben Archivs (bester Ausschnitt aus T1) |
  RE2-Textur (Blatt v 0..217). Selbst angesehen, alle fuenf Streifen: die Aufteilung stimmt Block fuer Block
  (DOOR13 Gitterfenster + Feld, DOOR15 Bretter + X + Warnfeld, DOOR19 Wellenkante + Schlitze + Drehscheibe, DOOR1A
  drei Felder, DOOR1C Gitterfenster + Klappe, DOOR25 Mittelband mit Schild, DOOR26 Schott mit Rad, DOOR29 Tafel +
  Schild, DOOR2A Zugtuer mit Fenster + Lamellen, DOOR2E Gitter mit gerundetem Rahmen). Farbabweichungen, die die
  Pruefer bewusst zugelassen haben, weil RE2 dieselbe Tuer selbst so malt: DOOR19 dunkelgrau (RE2 ROOM2140 c0 grau),
  DOOR26 grau/khaki (RE2 ROOM60C0 c4, ROOM6170 c0). Die Leiterboegen zeigen im RE1.5-Ausschnitt oft nur die Wand
  (Umriss an der Leiter vorbei) - die Leitern sind in 3.1 an den Rohbildern verglichen.
- `build/r31_tueren/t3/vergleich_aehnlich.png` (53 Bloecke: 45 aehnlich + 5 Aufzug + 3 einseitig) mit naechstem
  Archiv und der Abweichung in der Kopfzeile. Angesehen: die zwoelf DOOR07-Faelle sind glatte dunkle Blaetter mit
  Druecker (DOOR07 hat Lueftungsschlitze und graue Flecken); die DOOR08-Faelle haben Randnut + gerahmte Felder.

## 3. Entscheidungen bei Widerspruechen (Leiter)

### 3.1 Leiterregel (Stapel 1/3 mild, Pruefer 2/4 streng, Pruefer 3 gemessen)

Frage: sind die RE1.5-Wandleitern "gleich" DOOR16 (Stahlrohrleiter)? Vergleich an den ROHEN Hintergruenden
(`extracted/PSX/.../ROOMxxxNN.bmp`, `info/re2leon/COMMON/BSS/...`), Ausschnitte 4-fach nebeneinander selbst angesehen:

| Leiter | Eindruck | Holme hell R/B (absolut, mein Mass) | Pruefer 3: relativ zur Wand |
|---|---|---|---|
| RE2 ROOM4090 c3 (DOOR16) | silbern, duenn | 1,08 | 1,02 (c1) |
| RE2 ROOM4100 c7 (DOOR16) | oliv im gelben Licht | 1,47 (Wand 1,90) | 0,85 (c4) |
| RE2 ROOM3090 c0 (DOOR16) | grau, leicht rostig | 1,16 | 1,12 |
| RE1.5 S037 ROOM10B0 c0 | grau im Nachtlicht | 1,08 | - |
| RE1.5 S163 ROOM2090 c0 | grau-oliv im gruenen Licht | 1,33 (Wand 2,30) | neutralgrau (Pruefer 4) |
| RE1.5 S133 ROOM2000 c0 | graubraun wie der Tunnel | 1,34 (Wand 1,46) | 0,97 |
| RE1.5 S137 ROOM2000 c10 | braun mit orangen Kanten | 1,43 (Wand 1,51) | 1,04 |
| RE1.5 S128 ROOM1260 c7 | **goldgelb vor grauer Wand** (`ROOM12607.bmp` selbst angesehen) | - | 1,52 |
| RE1.5 S088 ROOM11A0 c0 | **rotbraun vor gruener Wand** (`ROOM11A00.bmp` selbst angesehen) | - | - |

(Mein Mass: Mittel der hellsten 30 % im Leiterbereich gegen die dunkelsten 50 %; nur Stuetze, das Urteil ist der Blick.)

Entscheidung: **RE2-eigene Toleranz (Pruefer 3)** - RE2 nimmt DOOR16 fuer gemalte Leitern von relativ 0,85 bis 1,12,
also fuer silberne, olivgelb beleuchtete und leicht rostige. Gleich sind damit die Leitern, die nicht deutlich
waermer als ihre Umgebung sind: T024 (ROOM10B0/1170), T067/T068/T070 (ROOM2000/20B0 <-> ROOM1260/2050), T083
(ROOM2080/2090). Aehnlich die Messingleitern ROOM1260 (T023, T052, T053: relativ 1,39..1,52) und die rotbraune Leiter
ROOM11A0 (T047). Folgen:
- S168/S169/S172 (ROOM20B0, Stapel 4 streng aehnlich) -> **gleich**: ROOM20B0 ist das geflutete ROOM2000 mit
  demselben Bild (Bogen ROOM20B0 angesehen: c0/c4/c9 = ROOM2000 c0/c5/c10), also dieselben Leitern wie S133/S134/S137.
- S150 (ROOM2050, Ausstieg oben) -> **gleich**, Ansicht "ohne Leiter": das eigene Bild zeigt nur das Gelaender und ein
  grau gebogenes Holmende (Pruefer 4); die Leiter von T070 zeigen S137/S172.
- **Schachtseiten folgen ihrer Leiter:** S038 (ROOM10B0, Rundschacht) und S095 (ROOM11B0, Kanaldeckel) waren gleich
  DOOR16 V5 nach der Art (RE2 ROOM21A0 c3). Die DOOR16-Sequenz zeigt aber die Leiter gross, und die Leiter dieser
  Tueren ist die ROOM1260-Messingleiter (S130 bzw. S128) -> beide **aehnlich**, wie S173 (ROOM3000, Pruefer 4) zur
  Rostleiter S088.
- Waere die milde Regel gewollt (jede einfache Wandleiter = DOOR16): +4 Tueren (T023, T047, T052, T053) -> 87.
  Waere die strenge Farbregel gewollt (nur graue Leitern): T067/T068/T070 fallen -> 80.

### 3.2 T131 ROOM4080 <-> ROOM4090 (FREEZING ROOM): zwei Archive

Aussen S251: graues EINTEILIGES Schott, Baender links, Rad rechts = DOOR26 V2. Innen S252: zweiteiliges, weiss
ueberfrorenes Schott, Rad links, TYPE-P = DOOR31 V0 (Bogen ROOM4080 und DOOR31.png angesehen). RE1.5 malt die Seiten
also verschieden; jede Seite ist fuer sich gleich. DOOR26 und DOOR31 sind Geschwister (gleiche 5 Meshes
344/48/48/48/2, 18 Skripte, Tonfamilie F7, nur die Textur ist vereist). Entscheidung: **abgedeckt, je Seite ihr
Archiv** (aussen DOOR26 V2, innen DOOR31 V0). Das geht gegen die RE2-Regel "ein Archiv je Tuer" - RE2 nimmt fuer seinen
Kuehlraum 60C0/60D0 DOOR31 auf beiden Seiten; mit DOOR31 aussen saehe man dort ein vereistes zweiteiliges Schott vor
einem grauen einteiligen. Wer die RE2-Regel ueber das Bild stellt: DOOR31 V0 auf beiden Seiten (dann ist die
Aussenseite nicht gleich, Tuer einseitig).

### 3.3 Seite ohne brauchbare Ansicht: S161 (T075)

S161 (ROOM2080 -> ROOM2030): Umriss 31 px klein, hinten im dunklen Gang HINTER der offenen orangen Zarge (Vollbild c02
angesehen) - dort ist nichts gemalt, weder Blatt noch Gangende. Das ist keine anders gemalte Seite, sondern keine
Ansicht. Pruefer 4 hat sie aus Gleichbehandlung mit S165 auf "keine" gesetzt; S165 (ROOM20A0) zeigt dagegen frontal
eine schwarze Oeffnung ohne Blatt (T084 ist ohnehin aehnlich DOOR26). **T075 abgedeckt DOOR06** (S142 V1 nach Griff,
S161 V0 per Komplement).

### 3.4 Einseitig (RE1.5 malt die Seiten verschieden)

| Tuer | gleiche Seite | andere Seite (Ansicht brauchbar) |
|---|---|---|
| T048 ROOM11A0 <-> ROOM2070 | S159 ROOM2070: Staebe + Querband in gruener Bogenzarge = DOOR0A V1 | S087 ROOM11A0: Holzrahmen, Maschendraht oben, Druecker (naechstes DOOR14, Schiebetor) |
| T078 ROOM2040 <-> ROOM2060 | S146 ROOM2040: Lueftung mit senkrechten Staeben = DOOR1E V0 | S154 ROOM2060: Lueftung mit waagerechten Lamellen |
| T161 ROOM6000 <-> ROOM6030 | S314 ROOM6000: Zug-Aussentuer, Fenster + Lamellen = DOOR2A (V0 wie RE2s Zug ROOM7000 c5) | S322 ROOM6030: Wageninnentuer, zwei Felder + Leiste (naechstes DOOR08) |

Alle drei nebeneinander angesehen (Montage aus den `_aus.png` der sechs Seiten, dazu `vergleich_aehnlich.png`).
Zaehlen als nicht abgedeckt; die gleiche Seite kann gebaut werden (dann Animation nur aus dieser Richtung).

### 3.5 Kleinere Punkte

- T050 (S091 schlichte braune Tuer ~DOOR22 / S167 Profiltuer ~DOOR23): beide aehnlich, verschieden gemalt; naechstes
  DOOR23 (S167 ist die bessere Ansicht, S091 Sicherheit 1).
- T094 (S184 ~DOOR13, S206 ~DOOR08): beide aehnlich, S206 dunkel/steil; beide Archive genannt.
- T110 (S216/S220 beide Tafel links): V0/V0 (Pruefer 5 hat S220 korrigiert; RE2 kennt 0/0).
- T148 Hubbuehne (Kategorie aufzug, aber RE2 hat genau diese Art: DOOR2D): abgedeckt. T109/T114/T115 (Aufzugtuer
  der A-2-Kabine = DOOR27, von RE2 in ROOM6010/6020 so gemalt): abgedeckt auf der begehbaren Seite; die
  Kabinen-Seiten S221/S222/S223 sind Skript-Uebergaenge nach der Fahrt.
- "Aufzug" als Grund: T011/T017/T018 (ROOM1040/10C0/1120 -> Kabine ROOM1080: glatte graue Tuer in tiefer Nische,
  ~DOOR25 ohne Schild/Rippen), T102 (ROOM3070 -> Lastenaufzug ROOM3080, Rautengitter, ~DOOR14), T112 (ROOM4000 ->
  ROOM3070, offener Schacht mit Fachwerk, ~DOOR2D). Die Fahrt selbst ist Skript in der Kabine (RE2-Fahrton ueber
  `engine/src/scd_elev_se.c`), keine Tueranimation.

## 4. Abgedeckte Tueren (83)

`*` = Variante abgeleitet (Griff auf dieser Seite nicht sichtbar). Seiten in `[..]` mit abweichendem Archiv.

| Tuer | Raeume | Archiv | Varianten je Seite (Griffseite) | Griff-Tausch | Ton | Kat. |
|---|---|---|---|---|---|---|
| T075 | ROOM2030 <-> ROOM2080 | DOOR06 | S142 2030 V1 (rechts); S161 2080 V0 * | - | F3 | normal |
| T079 | ROOM2040 | DOOR06 | S147 2040 V1 (rechts); S148 2040 V0 (links) | - | F3 | selbst |
| T033 | ROOM1120 <-> ROOM1130 | DOOR09 | S058 1120 V1 (rechts); S060 1130 V0 (links) | DOOR04 | F2 | normal |
| T034 | ROOM1130 <-> ROOM1150 | DOOR09 | S061 1130 V0 (links); S065 1150 V0 (links) | DOOR04 | F2 | normal |
| T077 | ROOM2040 <-> ROOM2070 | DOOR0A | S149 2040 V0 (links); S160 2070 V1 (rechts) | - | eigen (DOOR0A) | normal |
| T005 | ROOM1020 <-> ROOM1040 | DOOR13 | S007 1020 V0 (links); S016 1040 V1 (rechts) | - | eigen (DOOR13) | normal |
| T007 | ROOM1030 <-> ROOM1070 | DOOR13 | S009 1030 V1 (rechts); S026 1070 V0 (links) | - | eigen (DOOR13) | normal |
| T010 | ROOM1030 <-> ROOM1050 | DOOR13 | S008 1030 V1 (rechts); S017 1050 V0 (links) | - | eigen (DOOR13) | normal |
| T027 | ROOM10D0 <-> ROOM10F0 | DOOR13 | S042 10D0 V1 (rechts); S047 10F0 V0 (links) | DOOR07 | eigen (DOOR13) | normal |
| T042 | ROOM1180 <-> ROOM1190 <-> ROOM1230 | DOOR13 | S079 1180 V0 (links); S082 1190 V1 (rechts); S083 1190 V1 (rechts); S120 1230 V0 (links) | - | eigen (DOOR13) | normal |
| T087 | ROOM3000 <-> ROOM3010 | DOOR15 | S174 3000 V1 (rechts); S175 3010 V0 (links) | - | F4 | normal |
| T089 | ROOM3010 <-> ROOM3060 | DOOR15 | S176 3010 V1 (rechts); S190 3060 V0 (links) | - | F4 | normal |
| T090 | ROOM3010 <-> ROOM3020 | DOOR15 | S178 3010 V0 (links); S181 3020 V1 (rechts) | - | F4 | normal |
| T091 | ROOM3010 <-> ROOM30D0 | DOOR15 | S179 3010 V0 (links); S207 30D0 V1 (rechts) | - | F4 | normal |
| T024 | ROOM10B0 <-> ROOM1170 | DOOR16 | S037 10B0 V4; S069 1170 V5 | - | eigen (DOOR16) | leiter |
| T067 | ROOM1260 <-> ROOM2000 <-> ROOM20B0 | DOOR16 | S129 1260 V5; S133 2000 V4; S168 20B0 V4 | - | eigen (DOOR16) | normal |
| T068 | ROOM1260 <-> ROOM2000 <-> ROOM20B0 | DOOR16 | S132 1260 V5; S134 2000 V4; S169 20B0 V4 | - | eigen (DOOR16) | normal |
| T070 | ROOM2000 <-> ROOM2050 <-> ROOM20B0 | DOOR16 | S137 2000 V4; S150 2050 V5; S172 20B0 V4 | - | eigen (DOOR16) | normal |
| T083 | ROOM2080 <-> ROOM2090 | DOOR16 | S162 2080 V5; S163 2090 V4 | - | eigen (DOOR16) | normal |
| T030 | ROOM1100 <-> ROOM1110 | DOOR19 | S050 1100 V0 (links); S051 1110 V1 (rechts) | - | eigen (DOOR19) | normal |
| T031 | ROOM1110 | DOOR19 | S052 1110 V0 (links); S053 1110 V1 (rechts) | - | eigen (DOOR19) | selbst |
| T032 | ROOM1110 | DOOR19 | S054 1110 V0 (links); S055 1110 V1 (rechts) | - | eigen (DOOR19) | selbst |
| T044 | ROOM1180 <-> ROOM1190 <-> ROOM1230 | DOOR19 | S081 1180 V1 (rechts); S084 1190 V1 (rechts); S085 1190 V1 (rechts); S122 1230 V1 (rechts) | - | eigen (DOOR19) | normal |
| T056 | ROOM11E0 <-> ROOM1210 | DOOR19 | S103 11E0 V1 (rechts); S106 1210 V0 (links) | - | eigen (DOOR19) | normal |
| T036 | ROOM1130 <-> ROOM1170 | DOOR1A | S062 1130 V1 (rechts); S072 1170 V0 (links) | - | F4 | normal |
| T037 | ROOM1140 <-> ROOM1170 | DOOR1A | S064 1140 V1 (rechts); S073 1170 V0 (links) | - | F4 | normal |
| T043 | ROOM1160 <-> ROOM1180 <-> ROOM11D0 <-> ROOM1230 | DOOR1A | S066 1160 V1 (rechts); S067 1160 V1 (rechts); S076 1180 V0 (links); S077 1180 V0 (links); S097 11D0 V1 (rechts); S098 11D0 V1 (rechts); S117 1230 V0 (links); S118 1230 V0 (links) | - | F4 | normal |
| T051 | ROOM11B0 <-> ROOM11C0 | DOOR1A | S094 11B0 V0 (links); S096 11C0 V0 (links) | - | F4 | normal |
| T055 | ROOM11E0 <-> ROOM1200 | DOOR1A | S101 11E0 V0 (links); S105 1200 V1 (rechts) | - | F4 | normal |
| T143 | ROOM5080 <-> ROOM6010 | DOOR1A | S278 5080 V0 (links); S317 6010 V1 (rechts) | - | F4 | normal |
| T028 | ROOM10D0 <-> ROOM10E0 | DOOR1B | S043 10D0 V3 (mitte); S046 10E0 V2 (mitte) | - | F5 | normal |
| T057 | ROOM1210 <-> ROOM1220 | DOOR1C | S109 1210 V0 (links); S114 1220 V1 (rechts) | - | eigen (DOOR1C) | normal |
| T058 | ROOM1210 <-> ROOM1220 | DOOR1C | S107 1210 V0 (links); S112 1220 V1 (rechts) | - | eigen (DOOR1C) | normal |
| T059 | ROOM1210 <-> ROOM1220 | DOOR1C | S111 1210 V0 *; S116 1220 V1 * | - | eigen (DOOR1C) | normal |
| T060 | ROOM1210 <-> ROOM1220 | DOOR1C | S110 1210 V0 (links); S115 1220 V1 * | - | eigen (DOOR1C) | normal |
| T061 | ROOM1210 <-> ROOM1220 | DOOR1C | S108 1210 V0 (links); S113 1220 V1 (rechts) | - | eigen (DOOR1C) | normal |
| T097 | ROOM3040 <-> ROOM3060 | DOOR1D | S186 3040 V2 (beide); S192 3060 V3 (beide) | - | eigen (DOOR1D) | normal |
| T069 | ROOM2000 <-> ROOM2010 <-> ROOM20B0 | DOOR23 | S135 2000 V0 (links); S138 2010 V1 (rechts); S170 20B0 V0 (links) | - | F4 | normal |
| T088 | ROOM3010 <-> ROOM30E0 | DOOR24 | S180 3010 V1 (rechts); S210 30E0 V0 (links) | - | F6 | normal |
| T099 | ROOM3060 <-> ROOM3090 | DOOR24 | S191 3060 V0 (links); S200 3090 V1 (rechts) | - | F6 | normal |
| T111 | ROOM4000 <-> ROOM4030 | DOOR25 | S217 4000 V0; S224 4030 V0 | - | eigen (DOOR25) | normal |
| T116 | ROOM4030 <-> ROOM4040 | DOOR25 | S225 4030 V0; S228 4040 V0 (links) | - | eigen (DOOR25) | normal |
| T128 | ROOM4070 <-> ROOM50A0 | DOOR25 | S246 4070 V0; S285 50A0 V0 | - | eigen (DOOR25) | normal |
| T135 | ROOM5000 <-> ROOM5030 | DOOR25 | S256 5000 V0; S265 5030 V0 | - | eigen (DOOR25) | normal |
| T136 | ROOM5000 <-> ROOM5110 | DOOR25 | S257 5000 V0; S300 5110 V0 | - | eigen (DOOR25) | normal |
| T138 | ROOM5030 <-> ROOM5040 | DOOR25 | S264 5030 V0; S268 5040 V0 | - | eigen (DOOR25) | normal |
| T139 | ROOM5030 <-> ROOM50A0 | DOOR25 | S267 5030 V0; S286 50A0 V0 | - | eigen (DOOR25) | normal |
| T140 | ROOM5030 <-> ROOM5070 | DOOR25 | S266 5030 V0; S275 5070 V0 | - | eigen (DOOR25) | normal |
| T141 | ROOM5040 <-> ROOM5050 <-> ROOM5120 | DOOR25 | S270 5040 V0; S271 5050 V0; S272 5050 V0; S304 5120 V0 | - | eigen (DOOR25) | normal |
| T154 | ROOM5110 <-> ROOM5120 | DOOR25 | S299 5110 V0; S303 5120 V0 | - | eigen (DOOR25) | normal |
| T155 | ROOM5110 <-> ROOM5140 | DOOR25 | S302 5110 V0; S310 5140 V0 | - | eigen (DOOR25) | normal |
| T156 | ROOM5110 <-> ROOM5130 | DOOR25 | S301 5110 V0; S306 5130 V0 | - | eigen (DOOR25) | normal |
| T129 | ROOM4080 <-> ROOM40B0 | DOOR26 | S249 4080 V0 (links); S255 40B0 V0 * | - | F7 | normal |
| T133 | ROOM5000 <-> ROOM5020 | DOOR26 | S259 5000 V0 (links); S262 5020 V0 (links) | - | F7 | normal |
| T149 | ROOM50B0 <-> ROOM5140 | DOOR26 | S287 50B0 V0 (links); S309 5140 V1 (rechts) | - | F7 | normal |
| T150 | ROOM50C0 <-> ROOM50D0 | DOOR26 | S289 50C0 V0 (links); S291 50D0 V0 * | - | F7 | normal |
| T151 | ROOM50D0 <-> ROOM5100 | DOOR26 | S294 50D0 V0 (links); S298 5100 V0 (links) | - | F7 | normal |
| T152 | ROOM50D0 <-> ROOM50F0 | DOOR26 | S292 50D0 V0 (links); S296 50F0 V1 (rechts) | - | F7 | normal |
| T153 | ROOM50D0 <-> ROOM50E0 | DOOR26 | S293 50D0 V0 (links); S295 50E0 V0 (links) | - | F7 | normal |
| T157 | ROOM5070 <-> ROOM5130 <-> ROOM6020 | DOOR26 | S277 5070 V2 (rechts); S308 5130 V2 (rechts); S319 6020 V3 (links) | - | F7 | normal |
| T158 | ROOM5070 <-> ROOM5130 <-> ROOM6020 | DOOR26 | S276 5070 V3 (links); S307 5130 V3 (links); S318 6020 V2 (rechts) | - | F7 | normal |
| T131 | ROOM4080 <-> ROOM4090 | DOOR26/DOOR31 | S251 4080 V2 (rechts) [DOOR26]; S252 4090 V0 (links) [DOOR31] | - | F7 | normal |
| T109 | ROOM4000 <-> ROOM4020 | DOOR27 | S215 4000 V0; S221 Skript | - | eigen (DOOR27) | aufzug |
| T114 | ROOM4020 <-> ROOM50C0 | DOOR27 | S290 50C0 V0; S223 Skript | - | eigen (DOOR27) | aufzug |
| T115 | ROOM4020 <-> ROOM5000 | DOOR27 | S260 5000 V0; S222 Skript | - | eigen (DOOR27) | aufzug |
| T110 | ROOM4000 <-> ROOM4010 | DOOR29 | S216 4000 V0 (links); S220 4010 V0 (links) | - | F6 | normal |
| T118 | ROOM4040 <-> ROOM4050 | DOOR29 | S229 4040 V0 (links); S231 4050 V1 (rechts) | - | F6 | normal |
| T119 | ROOM4040 <-> ROOM4050 | DOOR29 | S230 4040 V0 (links); S235 4050 V0 (links) | - | F6 | normal |
| T120 | ROOM4040 <-> ROOM4070 | DOOR29 | S227 4040 V1 *; S245 4070 V0 (links) | - | F6 | normal |
| T121 | ROOM4050 | DOOR29 | S238 4050 V1 (rechts); S243 4050 V0 (links) | - | F6 | selbst |
| T122 | ROOM4050 | DOOR29 | S237 4050 V0 (links); S241 4050 V1 (rechts) | - | F6 | selbst |
| T124 | ROOM4050 | DOOR29 | S234 4050 V1 (rechts); S244 4050 V0 (links) | - | F6 | selbst |
| T125 | ROOM4050 | DOOR29 | S233 4050 V0 (links); S242 4050 V1 * | - | F6 | selbst |
| T127 | ROOM4070 <-> ROOM5100 | DOOR29 | S247 4070 V0 (links); S297 5100 V1 (rechts) | - | F6 | normal |
| T130 | ROOM4080 <-> ROOM40A0 | DOOR29 | S250 4080 V1 (rechts); S253 40A0 V0 (links) | - | F6 | normal |
| T134 | ROOM5000 <-> ROOM5010 | DOOR29 | S258 5000 V1 (rechts); S261 5010 V0 (links) | - | F6 | normal |
| T144 | ROOM5090 | DOOR2A | S281 5090 V2 (beide); S283 5090 V2 (beide) | - | eigen (DOOR2A) | selbst |
| T145 | ROOM5090 | DOOR2A | S279 5090 V2 (beide); S282 5090 V2 (beide) | - | eigen (DOOR2A) | selbst |
| T147 | ROOM5090 <-> ROOM6030 | DOOR2A | S280 5090 V0 (links); S323 6030 V1 (rechts) | - | eigen (DOOR2A) | normal |
| T162 | ROOM6030 | DOOR2A | S320 6030 V0 *; S321 6030 V1 (rechts) | - | eigen (DOOR2A) | selbst |
| T148 | ROOM50B0 <-> ROOM6000 | DOOR2D | S288 50B0 V5; S311 6000 V4 | - | eigen (DOOR2D) | aufzug |
| T159 | ROOM6000 <-> ROOM6010 | DOOR2E | S313 6000 V1 (rechts); S316 6010 V0 (links) | - | eigen (DOOR2E) | normal |
| T160 | ROOM6000 <-> ROOM6010 | DOOR2E | S312 6000 V1 (rechts); S315 6010 V0 (links) | - | eigen (DOOR2E) | normal |

## 5. Nicht abgedeckte Tueren (61)

Seiten mit `(gleich DOORxx)` sind die baubaren Seiten der einseitigen Tueren.

| Tuer | Raeume | Grund | naechstes | Abweichung (Kurz) | Seiten |
|---|---|---|---|---|---|
| T035 | ROOM1130 <-> ROOM1140 | aehnlich | DOOR04 | DOOR04 (Fluegel einer Doppeltuer V2/V3, sechs Kassetten, Stangengriff lang Messing) ist BLAU gestrichen, RE1.5 braunes Holz unter warmem Licht. DOOR01 V2/V3 braun, abe... | S059, S063 |
| T000 | ROOM1000 <-> ROOM1050 | aehnlich | DOOR07 | DOOR07 hat Lueftungsschlitze unten und ist hellgrau-fleckig; kein RE2-Archiv mit glattem dunklem Blatt | S001, S019 |
| T001 | ROOM1000 <-> ROOM1050 | aehnlich | DOOR07 | DOOR07 hat Lueftungsschlitze unten und ist hellgrau-fleckig; kein RE2-Archiv mit glattem dunklem Blatt | S002, S018 |
| T002 | ROOM1000 <-> ROOM1050 | aehnlich | DOOR07 | DOOR07 = graue fleckige Blechtuer MIT Lueftungsschlitzen unten; hier keine Lueftung und dunkle glatte Farbe. Kein RE2-Archiv hat ein voellig glattes dunkles Blatt | S000, S020 |
| T003 | ROOM1010 <-> ROOM1020 | aehnlich | DOOR07 | DOOR07 hat Lueftungsschlitze unten und graue Fleckentextur; kein RE2-Archiv mit glattem dunklem Blatt | S004, S005 |
| T004 | ROOM1010 <-> ROOM1020 | aehnlich | DOOR07 | DOOR07 hat Lueftungsschlitze unten und graue Fleckentextur; kein RE2-Archiv mit glattem dunklem Blatt | S003, S006 |
| T006 | ROOM1030 <-> ROOM1040 | aehnlich | DOOR07 | DOOR07 hat Lueftungsschlitze unten und graue Fleckentextur; kein RE2-Archiv mit glattem dunklem Blatt | S010, S014 |
| T012 | ROOM1040 <-> ROOM1060 | aehnlich | DOOR07 | DOOR07 hat Lueftungsschlitze unten und graue Fleckentextur; kein RE2-Archiv mit glattem dunklem Blatt | S015, S025 |
| T013 | ROOM1050 <-> ROOM10A0 | aehnlich | DOOR07 | DOOR07 hat Lueftungsschlitze unten und graue Fleckentextur, hier glatt und dunkel | S021, S033 |
| T015 | ROOM1060 <-> ROOM1120 | aehnlich | DOOR07 | DOOR07 (graue fleckige Blechtuer) hat Lueftungsschlitze unten - hier keine; Farbe/Material sonst nah | S023, S056 |
| T016 | ROOM1060 <-> ROOM10C0 | aehnlich | DOOR07 | DOOR07 hat Lueftungsschlitze unten - hier keine | S024, S039 |
| T021 | ROOM10A0 <-> ROOM11E0 | aehnlich | DOOR07 | DOOR07 hat Lueftungsschlitze unten und ist heller grau-fleckig | S036, S102 |
| T022 | ROOM10A0 <-> ROOM1180 <-> ROOM1230 | aehnlich | DOOR07 | DOOR07 hat Lueftungsschlitze unten und ist heller grau-fleckig | S034, S035, S078, S119 |
| T096 | ROOM3040 <-> ROOM3050 | aehnlich | DOOR08 | wie S188: DOOR08 hat flache vertiefte Felder gleicher Hoehe, Nieten, Rost, Knauf; hier gepraegte ungleiche Felder, Stangengriff | S187, S188 |
| T098 | ROOM3050 <-> ROOM30E0 | aehnlich | DOOR08 | DOOR08 (Stahltuer mit zwei vertieften Feldern) ist dieselbe Klasse, aber ohne umlaufende gerundete Randnut und ohne die breit gerahmten Felder, dafuer Eck-Nieten, Rost... | S189, S213 |
| T103 | ROOM3070 <-> ROOM30E0 | aehnlich | DOOR08 | wie S189: DOOR08 ohne gerahmte Felder/Randnut, mit Nieten/Rost/dunklem Unterfeld; Griff Knauf statt Stange | S196, S211 |
| T105 | ROOM3090 <-> ROOM30C0 <-> ROOM30D0 | aehnlich | DOOR08 | wie S189: naechstes DOOR08 ohne gerundete Randnut und gerahmte Felder, mit Nieten/Rost; Griff Knauf statt Stange | S198, S199, S205, S208 |
| T106 | ROOM3090 <-> ROOM30B0 | aehnlich | DOOR08 | wie S189: DOOR08 ohne gerahmte Felder/Randnut, mit Nieten/Rost; Griff Knauf statt Stange | S202, S204 |
| T107 | ROOM3090 <-> ROOM30A0 | aehnlich | DOOR08 | wie S203/S189: DOOR08 ohne gerundete Randnut und gerahmte Felder, mit Nieten/Rost; Griff Knauf statt Stange | S201, S203 |
| T094 | ROOM3030 <-> ROOM30C0 <-> ROOM30D0 | aehnlich | DOOR13, DOOR08 | DOOR13 (dunkle Stahltuer: Gitterfenster oben, Feld unten) hat dieselbe Zweiteilung, ist aber dunkel und das obere Feld ein Gitterfenster mit Schild; hier helles glatte... | S184, S185, S206, S209 |
| T123 | ROOM4050 | aehnlich | DOOR13 | DOOR13 (dunkle Stahltuer) hat oben ein grosses Gitterfenster mit Schild und unten ein Kassettenfeld; hier nur ein kleines dunkles Feld mit abgeschnittener Ecke, Piktog... | S236, S239 |
| T126 | ROOM4050 | aehnlich | DOOR13 | DOOR13 (dunkle Stahltuer) hat oben ein grosses Gitterfenster mit Schild und unten ein Kassettenfeld; hier nur ein kleines dunkles Feld mit abgeschnittener Ecke, Piktog... | S232, S240 |
| T023 | ROOM10B0 <-> ROOM1260 | aehnlich | DOOR16 | Schacht-Art wie RE2 ROOM21A0 (DOOR16 V5), aber die Leiter DIESER Tuer (Gegenseite S130, ROOM1260) ist messinggelb (relativ 1,39 gegen 0,85..1,12 der RE2-DOOR16-Leitern... | S038, S130 |
| T047 | ROOM11A0 <-> ROOM3000 | aehnlich | DOOR16 | Art gleich (Steigleiter zum runden Deckenschacht), Gestalt nicht: dunkle rost-/graubraune Leiter mit dicken Flachholmen und breiten Vierkant-Sprossen (ROOM11A0 c00, im... | S088, S173 |
| T052 | ROOM11B0 <-> ROOM1260 | aehnlich | DOOR16 | Schacht-Art wie RE2 ROOM21A0 (DOOR16 V5), aber die Leiter DIESER Tuer (Gegenseite S128, ROOM1260 c7) ist messinggelb (relativ 1,52) | S095, S128 |
| T053 | ROOM11D0 <-> ROOM1260 | aehnlich | DOOR16 | gleiche Art (Wandleiter/Schacht wie DOOR16), aber Messingleiter: Holme gegen die Wand relativ 1,4-1,5 warm (RE2-DOOR16-Leitern 0,85-1,12, Textur silbernes Rohr) - Farb... | S099, S131 |
| T014 | ROOM1050 <-> ROOM1090 | aehnlich | DOOR1B | Doppeltuer-Archive: DOOR1B = braune Blechtuer mit kleinem Sichtfenster und Druckstange, DOOR0C = gruene Holztuer mit Glas, DOOR30 = Glasrahmen; keines hat Rahmen-Querr... | S022, S030 |
| T025 | ROOM10C0 <-> ROOM10D0 | aehnlich | DOOR1B | DOOR1B (Doppeltuer) = braune Blechtuer mit kleinem quadratischem Sichtfenster und Druckplatte; DOOR0C = dunkelgruen mit breitem Glasfeld; DOOR30 = Glasrahmen. Keines h... | S041, S044 |
| T026 | ROOM10D0 <-> ROOM1100 | aehnlich | DOOR1B | DOOR1B hat je Fluegel Sichtfenster und Druckplatte und ist braun; DOOR0C gruenes Holz mit Glas, DOOR30 Glasrahmen - kein glattes Doppeltuer-Archiv | S045, S049 |
| T045 | ROOM1180 <-> ROOM11B0 <-> ROOM1230 | aehnlich | DOOR1B | DOOR1B (Blech-Doppeltuer) hat kleine quadratische Fenster, Druckstange, braun; RE1.5 hohe Felder ohne Glas, zwei Druecker, grau | S080, S092, S093, S121 |
| T054 | ROOM11E0 <-> ROOM11F0 | aehnlich | DOOR1B | DOOR1B (einzige zweifluegelige Blechtuer) hat je Fluegel ein Sichtfenster oben und eine Druckstange quer, Farbe braun; RE1.5 ohne Fenster, mit zwei Drueckern statt Dru... | S100, S104 |
| T074 | ROOM2030 <-> ROOM2060 | aehnlich | DOOR1D | DOOR1D (rotbraune Blechtuer, Griffkasten, dunkler Sockel) hat oben ein grosses Lueftungsgitter statt des schmalen Schilds, kein Warndreieck; Farbe orange statt rotbraun | S144, S153 |
| T029 | ROOM10F0 | aehnlich | DOOR1E | DOOR1E/33/35 (RE2-Lueftungsklappe) haben SENKRECHTE Gitterstaebe im Rahmen ueber genieteten Stahlplatten; Lamellengitter gibt es als Klappe in RE2 nicht | S048 |
| T080 | ROOM2040 <-> ROOM2050 | aehnlich | DOOR1E | DOOR1E = Lueftungsgitter mit durchgehenden senkrechten Staeben im Rahmen; hier sind Staebe nur unten sichtbar (gelaenderartig), Rahmen nicht erkennbar, sehr dunkel -> ... | S145, S152 |
| T049 | ROOM11A0 | aehnlich | DOOR22 | DOOR22 = rostrote Stahltuer mit breitem genietetem Rand (ca. 9 % der Blattbreite je Seite, Nietreihe rundum) und aufgemaltem D-foermigem Riegelblock mit Schloss; RE1.5... | S089, S090 |
| T050 | ROOM11A0 <-> ROOM20A0 | aehnlich | DOOR23, DOOR22 | wie S089 (kein Nietrand, Druecker statt gemaltem D-Riegel). Ausserdem: Gegenseite S167 (Stapel 4, ROOM20A0 c08) zeigt dieselbe Tuer T050 als braune Stahltuer mit abges... | S091, S167 |
| T071 | ROOM2000 <-> ROOM2070 | aehnlich | DOOR23 | Fluegel-Gestaltung wie DOOR23, aber Doppeltuer: DOOR23 hat nur Einzelfluegel (keine V2/V3 mit festem Zweitfluegel); die RE2-Doppeltueren 0C/1B/30 sind Holz-mit-Glas / ... | S136, S155 |
| T072 | ROOM2020 <-> ROOM2030 | aehnlich | DOOR23 | Wie S139: an der Gegenseite S143 laeuft die Profillinie am Griff gerade, keine DOOR23-Einbuchtung erkennbar; eigene Seite nicht auswertbar (Sicherheit 1) | S140, S143 |
| T073 | ROOM2020 <-> ROOM2050 | aehnlich | DOOR23 | An der Gegenseite S151 (kontrastverstaerkt angesehen) laeuft die linke Profillinie am Griff GERADE durch - die DOOR23-typische Einbuchtung des Profils um den Griff (so... | S139, S151 |
| T076 | ROOM2030 <-> ROOM2070 | aehnlich | DOOR23 | jeder Fluegel = DOOR23-Gestaltung (achteckiges Profil + Ausbuchtung + Riegelstange, gespiegelt; einfluegelig bestaetigt an S170), aber RE2 hat DOOR23 nur EINFLUEGELIG;... | S141, S156 |
| T081 | ROOM2070 | aehnlich | DOOR23 | jeder Fluegel = DOOR23-Gestaltung (achteckiges Profil + Ausbuchtung + Riegelstange, gespiegelt; einfluegelig bestaetigt an S170), aber RE2 hat DOOR23 nur EINFLUEGELIG;... | S157 |
| T082 | ROOM2070 | aehnlich | DOOR23 | jeder Fluegel = DOOR23-Gestaltung (achteckiges Profil + Ausbuchtung + Riegelstange, gespiegelt; einfluegelig bestaetigt an S170), aber RE2 hat DOOR23 nur EINFLUEGELIG;... | S158 |
| T095 | ROOM3030 <-> ROOM30E0 | aehnlich | DOOR24 | DOOR24 (beige Labortuer: Sichtfenster, Aushang, Blechkasten, Trittblech): Fenster hier flacher und schmaler, KEIN Aushang erkennbar (bei DOOR24 V1 waere er links unter... | S183, S214 |
| T084 | ROOM2090 <-> ROOM20A0 | aehnlich | DOOR26 | DOOR26/31 = rostrote zweiteilige Schotttuer TYPE-P mit Beschlagbaendern, grossem Handrad in Blattmitte und Schildern; hier dunkles glattes Blatt mit zwei Warnstreifen,... | S164, S165 |
| T142 | ROOM5040 <-> ROOM5060 <-> ROOM5120 | aehnlich | DOOR27 | DOOR27 (zweiteilige Labor-Schiebetuer mit Kasten oben rechts, Schild, WARNING-Aufkleber) ist dieselbe Klasse; hier aber gelbes Strahlenwarnschild auf dem linken Feld (... | S269, S273, S274, S305 |
| T161 | ROOM6000 <-> ROOM6030 | einseitig | DOOR08 | DOOR08 (Stahltuer mit zwei Feldern und Mittelriegel) hat dieselbe Grundteilung, aber keine senkrechte Rahmenleiste rechts und kein Rechteck oben, dafuer Nieten und Ros... | S314(gleich DOOR2A), S322 |
| T048 | ROOM11A0 <-> ROOM2070 | einseitig | DOOR14 | DOOR14 hat dieselbe Aufteilung (zwei Rautengitterfelder, Mittelriegel), ist aber ein SCHIEBE-Maschendrahttor im Stahlrahmen mit Griffplatte; RE1.5 ist eine Drehtuer mi... | S087, S159(gleich DOOR0A) |
| T078 | ROOM2040 <-> ROOM2060 | einseitig | DOOR1E | DOOR1E ist eine Gitterklappe mit SENKRECHTEN Staeben; hier ein Lamellengitter (waagerecht) - andere Gestalt; die Gegenseite S146 hat dagegen Staebe | S146(gleich DOOR1E), S154 |
| T102 | ROOM3070 | aufzug | DOOR14 | DOOR14 (Maschendraht-Tor im Stahlrahmen mit Querstange, breites Schiebetor 5857) ist dieselbe Klasse; hier groebere Rauten (eher Scherengitter), heller grauer Rahmen s... | S195 |
| T011 | ROOM1040 <-> ROOM1080 | aufzug | DOOR25 | DOOR25 (Aufzugtuer SHAFT TYPE-L) hat Schild, gelb-schwarzen Warnaufkleber und senkrechte Rippen, DOOR29 (TYPE-M) ist eine Drehtuer mit Bedientafel; die RE1.5-Tuer ist ... | S013 |
| T017 | ROOM1080 <-> ROOM10C0 | aufzug | DOOR25 | DOOR25 (SHAFT TYPE-L) hat Schild, Warnaufkleber und Rippen, DOOR29 ist eine Drehtuer mit Bedientafel; die RE1.5-Aufzugtuer ist glatt ohne Beschriftung | S040 |
| T018 | ROOM1080 <-> ROOM1120 | aufzug | DOOR25 | DOOR25 (Aufzugtuer SHAFT TYPE-L) hat Mittelfeld mit Schild und Warnaufkleber, Seitenleisten; RE1.5-Blatt glatt ohne Schild. DOOR29 hat Bedientafel am Blatt | S057 |
| T112 | ROOM4000 | aufzug | DOOR2D | DOOR2D (RE2-Hubbuehne: Streckmetallgitter mit gelb-schwarzem Warnrand, Bedientafel, rote Lampe) ist dieselbe Klasse (Lastenaufzug mit Warnrand), aber hier ist keine Pl... | S218 |
| T085 | ROOM20A0 | keine_tuer | - | kein Blatt; RE2 hat fuer Hochklettern in eine Wandnische kein Archiv (Leiter 16 = stehende Wandleiter, Luke 28 = Bodenluke, Klappe 1E = Gitterklappe) | S166 |
| T086 | ROOM20B0 | keine_tuer | - | kein Blatt; keine RE2-Entsprechung (nur objektlose Blende-Archive 20/21/32/34/36) | S171 |
| T117 | ROOM4030 <-> ROOM4080 | keine_tuer | - | kein Blatt: die Gegenseite S248 (ROOM4080 c07) zeigt eine OFFENE Oeffnung im Rahmen (Seitenpfeiler mit Leuchtknoepfen, Sturz), dahinter den rot beleuchteten Hauptschac... | S226, S248 |
| T041 | ROOM1170 | tor | - | nicht Teil des RE2-Vergleichs - eigenes Archiv im RE2-Aufbau schon gebaut | S068, S074 |
| T019 | ROOM1090 | inert_unsichtbar | - | kein Tuerblatt im Bild; RE2 hat fuer objektlose Uebergaenge nur Blende+Ton (DOOR20/21/32/34/36), das ist keine Tuergestalt | S031 |
| T064 | ROOM1250 | inert_unsichtbar | - | keine Tuer - nichts zu animieren; zaehlt nur formal als nicht abgedeckt | S126 |
| T065 | ROOM1250 | inert_unsichtbar | - | keine Tuer - nichts zu animieren; zaehlt nur formal als nicht abgedeckt | S127 |
| T066 | ROOM1250 | inert_unsichtbar | - | keine Tuer - nichts zu animieren; zaehlt nur formal als nicht abgedeckt | S125 |

## 6. Fuer den Bau (Schritt 1.2 aus T2 6.2)

- Archive, die die Zuordnung braucht (`re2_tuer_tabelle.py --kopiere-nach ... --nur`): **04 06 07 09 0A 13 15 16 19 1A
  1B 1C 1D 1E 23 24 25 26 27 29 2A 2D 2E 31** (21 Archive der abgedeckten Tueren, 1E fuer die einseitige S146,
  04/07 nur als Griff-Spender).
- Tabelle je Seite aus `zuordnung.json`: `tueren[].seiten[]` mit `abgedeckt == true` -> `schluessel` (Raeume, Band,
  Rechteck/Punkte), `archiv`, `variante`, `griff_tausch`, `tonfamilie`. Einseitige Tueren: nur die Seite mit
  `abgedeckt == true`.
- **S225 (T116, ROOM4030/4031 -> ROOM4040) ist ein Viereck-Satz** (40 B, `@0x47E`/`@0x4A6`). Der Port liest ihn
  heute mit dem 32-B-Schema und stellt die Tuer mit falschem Ziel auf (T1 §9.1). Ohne diese Korrektur feuert die
  Seite nie; die Gegenseite S228 ist normal.
- 54 der 184 abgedeckten Seiten stellt die Engine beim Betreten nicht selbst auf (`engine: false` in T1, u. a. ROOM1110,
  ROOM4050, ROOM5090 und Zwillingsraeume) - sie kommen aus Unterskripten. Die Zuordnung per Raum + Flaeche + Band
  trifft sie trotzdem, sobald `aot_fire_door` sie ausloest.
- DOOR2D V4 (S311, hinauf) ist nur das Gegenstueck zu RE2s V5; welches der beiden Skripte (V0/2/4 = Skript 1,
  301 Bilder; V1/3/5 = Skript 2, 451 Bilder) hinauf zeigt, ist nicht per Simulator belegt.

## 7. Offen

1. Leiterregel ist eine Wahl (3.1, Spanne 80..87 abgedeckt). Gewaehlt: RE2-eigene Toleranz.
2. T131: zwei Archive fuer eine Tuer gegen die RE2-Regel (3.2).
3. Einseitige Tueren T048/T078/T161: bauen (nur eine Richtung) oder nicht (3.4).
4. DOOR2D-Richtung der Skripte (6.).
5. S225 Viereck-Satz im Port (6.).
6. 16 Gleich-Seiten mit Sicherheit 1 (S111 S115 S116 S217 S227 S228 S230 S242 S285 S293 S299 S300 S301 S304 S309
   S320): jeweils durch die Gegenseite bzw. baugleiche Schwestertueren gestuetzt, im eigenen Bild kein sicheres
   Einzelmerkmal. Einzige Tuer, deren gleiche Seiten ALLE Sicherheit 1 haben: T059 (S111/S116, DOOR1C; Variante dort
   nach Regel Seite A V0 / B V1, weil auf keiner Seite ein Griff sichtbar ist).
7. Farbe als Merkmal: DOOR19 (grau statt beige) und DOOR26 (grau/khaki statt rostrot) gelten als gleich, weil RE2
   dieselbe Tuer selbst so malt; die Animation zeigt dann die RE2-Texturfarbe vor anders gemalter Wand - wie in RE2.
