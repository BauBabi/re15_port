# Story-Zensus des Ports gegen die 40%-Disc (Spur ST80-P)

Stand 2026-11-02, Worktree `r41_story80` auf master `789f6e681`. Nur gelesen und gemessen, kein Code und kein Asset geändert.
Gegenstück: der 80%-Katalog der Parallelspur ST80-K (`KATALOG_80.md`, beim Schreiben noch in Arbeit). Die Spalten „80 %“ unten
stützen sich deshalb vorläufig auf `analysis/story_begegnungen/BERICHT.md` §5/§10 und die Bildlisten `pruefung/video_a.md`,
`pruefung/video_b.md`. Beim Abgleich mit KATALOG_80 sind sie zu ersetzen.

**Maßstab (Nutzer, Wortlaut über die Koordination):** „die Räume, auch wenn sie vom Port erfunden wurden bleiben so! Lediglich Story
soll der 80% Version entsprechend angepasst werden.“ Nachtrag: „wenn ein Story snipped nachweislich und gravierend von der
ursprünglich geplanten Story abweicht, dann muss ich den halt noch mal aufnehmen. Aber ich musste in STAGE1 und STAGE2 sowieso mir
viele zusätzliche Dialoge und Cutscenes ausdenken […] Das werde ich auch noch für STAGE3-6 machen müssen.“ Daraus folgt:
- Herkunft MOD oder PORT ohne Beleg ist **kein Fehler an sich**. Port-Elemente auf Nutzerwunsch heißen hier **Nutzer-Lückenfüller**.
- Jedes Element trägt das Feld **Widerspruch** zu Disc/80 %: *nein* (gestützt oder neutral), *möglich* (setzt eine unbelegte Tatsache
  mit Story-Gewicht voraus, keine Quelle dagegen), *ja* (eine belegte Tatsache steht dagegen, mit Beleg).
- **Änderungskandidat** ist nur, was *ja* UND gravierend ist (Begegnung Leon/Elza, Leben/Tod/Rolle einer Figur, Ursache eines
  Hauptereignisses, Figur dort, wo die Quellen sie anders zeigen). Liste in §d.3.

## Datenlage und Methode

| Teil | Werkzeug / Datei (alle unter `analysis/story80/zensus/`) | Art |
|---|---|---|
| SCD-Walk aller RDTs beider Bäume: Subs, statische Erreichbarkeit, NPC-Sätze (Typ, Lage, Bedingung), Textaufrufe | `scd_zensus.py` → `port_zensus.json`, `disc_zensus.json` | gemessen (Bytes). Längentabelle = `re15_port/tools/scd_walk_lib.py` (eine Wahrheit mit `scd_vm.c`), 0 Desync in 240+96 Dateien |
| Raumliste, Raumnamen der EXE (`@0x8006ff36`, 26 B je Raum), Kamera-Vergleich Mod/Disc | `raumliste.py`, `raumnamen.py` → `raumliste.md`, `namen_disc.json` | gemessen |
| Gegenüberstellung je Raum: Disc-Zeilen (TEXTE_40) gegen englische Port-Zeilen mit Aufrufer | `texte_paare.py` → `texte_paare.md` | gemessen |
| Urteil je englischer Zeile (1231 Zeilen) und je Disc-Zeile (291) | `textabgleich_stage1.md`, `textabgleich_stage23.md`, `textabgleich_stage456.md` | gelesen und bewertet (drei Teilprüfer) |
| Story-Eingriffe der Port-C-Module (141 Elemente, mit Datei:Zeile, Commit, AUFTRAG-Fundstelle) | `port_module_teil1.md` (STAGE 1, Szenen/Figuren), `port_module_teil2.md` (Texte, STAGE 2–6, Ende) | Code gelesen |
| Sprachdateien → (Raum, Id) → Text → Herkunft | `stimmen.py` → `stimmen.md` | gemessen (Dateibaum `synchro/` des Hauptbaums + Sonderzuordnungen aus `stimmen_r39.c`, `elza_r36.c:226`, `dining_r39.c:550-555`) |
| Laufzeit | `re15_port/build/tests/unit/probe_synchro_status.exe --alle` (Port-Dekoder, Hauptbaum): 1229 Messages in 170 Räumen, 435 vertont | gemessen; deckt sich mit dem Python-Dekoder |

Grenzen: Erreichbarkeit ist **statisch** (Aufruferkette main/sub00/sub01 → Evt_exec/Evt_chain/Gosub/AOT-sce-3, wie
`story_begegnungen/werkzeuge/k12_subs.py`); Flags, die nie gesetzt werden, sind nicht ausgewertet. Die exe wurde nicht gestartet.
Port-C-Auslöser (scd_event_fire usw.) stehen in `port_module_teil*.md`, nicht im JSON.

## 0. Zahlen

| Größe | Wert |
|---|---|
| Port-Baum (= Mod „Magic Zombie Door“, bytegleich mit `info/Re1.5/PSX`) | 240 RDT-Dateien: **206 echte**, **34 Platzhalter** (4 Byte `00000000`) |
| 40%-Disc | 96 RDT zu 51 Raumnummern; **keine** Disc-RDT ist bytegleich mit der Port-RDT gleichen Namens |
| Raumnummern mit echter Datei im Port | 103 = **51 auch auf der Disc** + **52 nur in der Mod** |
| Raumnummern ohne Datei in beiden Bäumen (nur Platzhalter oder nur EXE-Name) | **17** |
| Englische Port-Zeilen (beide Szenarien, inkl. Untersuchen) | **1231**: WÖRTLICH 286, SINNGEMÄSS 63, ABGEWANDELT 119, MOD-TEXT 749, PORT-TEXT 14 |
| Disc-Zeilen | 291; ohne Port-Gegenstück **3** (D11C #10 Dublette, D309 #21, D309 #24), dazu D30E #4 verlesen und nie gerufen, D119 #1 vorhanden aber nie gerufen |
| Disc-Zeilen, die auf der Disc **nicht** laufen, im Port aber schon | 66: D11B #10–21, D11C #7–9, D400 #5–8, D401 #19–22, D501 #0–37, D50A #0–3, D512 #1 (Capcoms Plan, nicht der Disc-Ablauf) |
| Story-Eingriffe der Port-C-Module | 141 Zeilen (Teil 1: 71, Teil 2: 70, teils dasselbe Element aus zwei Blickwinkeln). Herkunft: PORT-WAHL 114, MOD 15, RE2 12, DISC als Hauptquelle 0. Fast alle mit Nutzerwunsch-Fundstelle |
| Sprachdateien `synchro/` (ohne `unused/`) | **489**. Zeile dazu: WÖRTLICH 152, SINNGEMÄSS 24, ABGEWANDELT 69, **MOD-TEXT 177**, **PORT 52**, **PORT-TEXT 11** (story_r36-Umschreibung), ohne Text 4 |

## (a) Raumliste Port / Disc / nur Mod

Volle Tabelle (je Raumnummer: EXE-Name, Dateien, Kamera-Vergleich, Textzahl, Figuren): [`zensus/raumliste.md`](zensus/raumliste.md).

**a.1 Nur in der Mod (52 Raumnummern, je Leon- und Elza-Datei):**
100, 101, 105, 108, 10A, 10C, 10D, 10E, 10F, 111, 112, 113, 114, 11D, 11F, 122, 123, 124, 125, 126 ·
201, 202, 204, 205, 206, 208, 20A · 305, 30A, 30B, 30C, 30D · 402, 404, 405, 407, 408, 40B ·
500, 502, 505, 507, 508, 509, 50B, 50D, 50E, 50F, 513 · 600, 601, 602.
Davon trägt die Disc-EXE einen Namen für 10A (KAIDAN 2), 11F (BOIRA SHITSU), 122–126 (DOKUBOU 1–5), 201 (ZONBIE TSUURO),
208 (WANI BOSS ENT), 20A (WANI BOSS BEYA), 404 (MIGITSUURO), 40B (BAIYOUSHITSU), 505 (DENSANSHITSU), 50D (TEST) — geplant, ohne
Datei auf der Disc. Im 80%-Material zu sehen (BERICHT §10, video_a/b): **30C** (#053, #058, #139), **305** (#060, #061, #065),
**509** (#144, #145), **600** (#152–#154), **601** (#149), **50B** (#155, B 12:25), **50E** (B 12:29), **408** (B 12:38, Disc nur BSS).

**a.2 Gänzlich fehlende Räume (17, weder Disc- noch echte Mod-Datei):** 127 (EXE: L B1 ROUKA), 20C, 20D, 20E, 20F, 30F, 406,
40C, 40D, 40E, 40F, 515, 516, 517, **605, 606, 607** (EXE: DOURYOKUSHITSU = Maschinenraum; Mod-BSS 4 Byte). Das Finale nach 604
existiert also nirgends. Aus video_b.md zusätzlich Orte im 80%-Material **ohne Port-Raum**: „Rippen-Tunnel“, „Raum mit rotem Rad“,
drei Trailer-Orte (Prüfung und Liste: ST80-K, `katalog/orte_ohne_port.jpg`).

**a.3 Gleiche Nummer, anderer Raum (Falle für jeden Abgleich).** Kamera-Lagen (je Kamera 28 von 32 Byte ab RDT+0x24) Mod gegen Disc:
- **0 von n Mod-Kameras auf der Disc:** 106, 116, 118, 11A, 203, 207, 409, **603 (Mod 14 / Disc 6)**, **604 (Mod 9 / Disc 7)**.
- Laut video_b.md/video_a.md (Bildvergleich): **Mod-603 Cut 6–12 = Disc-604 Cut 0–6** (die Mod hat Führerstand und Maschinenraum zu
  einem Raum 603 zusammengelegt); **Mod-604 = Abspann-Diashow** (9 Cuts, `aot_common.c:717-726`); Disc 201 = Mod 11A; Disc 203
  Cut 0–4 = Mod 207 Cut 4–8; Disc 20B Cut 11–14 = Mod 207 Cut 0–3; die Moos-Räume der Disc (207) fehlen in der Mod (README Z. 228).
- Teilweise gleich (Mod hat Kameras ergänzt): 103/1031, 110, 115, 117, 119, 11B, 11C, 11E, 209, 302 (2 von 9), 304, 309, 30E (1 von 10),
  401, 501, 503, 506, 511. Bildgleich (alle Kameras): 102, 104, 107, 109, 10B, 120, 121, 200, 20B, 300, 301, 303, 306, 307, 308, 400,
  403, 40A, 504, 50A, 50C, 510, 512, 514.
- Disc-Räume nur mit Leon-Datei (Mod hat beide): 106, 118, 203, 207, 209, 302.

## (b) Zensus je Raum und Szenario

**Figuren-Modelle im Port** = Modelle der Mod (alle NPC-Blobs der Mod-EMS sind ersetzt, `story_begegnungen/werkzeuge/ems.py`):
0x40 Marvin (README Z. 180), 0x42 Ada (Z. 96/274) — **in Elzas 3091 ist 0x42 Elza als NPC** (CDEMD1 Sektor 1505, Sherry ist dort
Spielfigur; `befunde_runde41/K_stage3szenen_b.md` K1.3), 0x45 Irons (Z. 179), 0x47 bei Leon Elliot (PLD/ELLIOT.*), bei Elza Roy
(CDEMD1, `re15_roy_r37.h`), 0x49 John (Z. 95), 0x4B Sherry, 0x4D „Annette“ (Z. 275). Auf der Disc sind 0x40/0x45/0x47 ein
Platzhalter-Blob und 0x4D Elzas Körper mit Adas Clips (§f).

Legende Herkunft: **D** = Inhalt auf der Disc (Text/Figur/Zustand), **M** = nur Mod (README-Zeile), **P** = Port-C-Code
(Modul, Runde; *NL* = Nutzer-Lückenfüller, AUFTRAG-Fundstelle und Commit in `port_module_teil*.md`). Widerspruch: nein / möglich / **ja**.
„x Z.“ = englische Zeilen der Datei. Volle Zeilen mit Aufrufer und Urteil: `zensus/textabgleich_stage*.md`.

### b.1 STAGE 1

| Datei | Figuren im Port | Was der Port zeigt | Herkunft | Disc | 80 % (vorläufig) | Widerspruch |
|---|---|---|---|---|---|---|
| 1240 / 1241 | – | RE2-Vorspann (T-Virus, S.T.A.R.S., Chris, Jill), 6 Z.; Start von NEW GAME | M (README Z. 22); P: Elza-Stimme (elzaintro_r38, R38) | nur `ROOM124.BSS` | Figurenwahl B 0:10.5 | möglich (RE2-Text statt eines 1.5-Vorspanns, nichts belegt) |
| 1170 Leon | Elliot (0x47), Pilot (nur Text) | Hubschrauber startet ohne Leon, 17 Z.; #16 „report to the chief“ | M (Z. 103, 160, Juvenal) | ohne Text, ohne NPC | B 0:19–0:23.6 Leon allein: おい 冗談だろ？/…クソッなんてこった！; #083 = 1170 #16 | möglich (Elliot/Pilot unbelegt; Leon allein passt) |
| 1171 Elza | John (sub07) | John: beeil dich, Kisten hinunterstoßen, „let's get down“ | D (D117 #0–3) + P: Krähen/Pose (heliport1171_r38/r39), Stimme (1171,3)←main02 | John sub00 | B 1:08.5 Elza bei den Kisten, Krähen | nein |
| 1011 Elza (Mod-Raum) | Roy (0x47, CDEMD1) | Roy lebt verletzt, schickt Elza zu John in den Zellentrakt, 18 Z.; Stimme main16.5 (Untertitel 1011,24); „Roys Diary“ statt Code-Text | M (Z. 277) + P NL (elza_r36 R36, roy_r37 R37, R38) | Roy nur in 102 | #027/#028 Elza bei verletztem Polizisten, wohl Roy | nein |
| 1020 Leon | – (Roys Leiche als Objekt) | Leiche erst nach Strom (4,0xF3) | M (Z. 265) + P NL (story_r36 P1, R36 :113) | 1020 ohne Roy, ohne Text | – | möglich (Leon sieht Roy auf der Disc nie) |
| 1021 Elza | John, Roy | Roy verwandelt sich, John erschießt ihn (D102 #0–7), Schuss-/Sterbe-Ton, Leichen-Text (1021,40) | D + P NL (elza_r36, elzatoene_r38) | dieselbe Szene | – | **ja, gering**: 1021 #6 spricht John statt Elza (D102 #5, Endung のよ) |
| 1050 Leon (Mod-Raum) | Ada | Ada ruft hinter der Tür (1050 #22–26), Mod-Kurzszene nach der Rettung | M (Z. 105) + P NL (adaruf_1050, R34n/R35) | Ada ruft in 109 aus dem Transporter | – | nein (Ruf passt zu D109 #0/#2) |
| 1090 Leon | Ada (geparkt, nur Stimme) | Transporter brennt, Rettung D109 #0–5, Feuerlöscher | D + M (Löscher Z. 333) + P (feuerloescher_r37) | gleich | #096 Feuer | nein |
| 1091 Elza | – | (1091,20) „It's a burnt-out shed…“ | P NL (story_r36 P3, R36 :127) | Transporter offen, ohne Feuer | #035 ohne Feuer | nein |
| 10B1 Elza | John (geparkt) | D10B #0–3; #5 John schlägt die Kanaltunnel vor | D + M-Zusatz | D10B #3 „hier entlang kürzer“ | #032 anderer Wortlaut | möglich, gering |
| 10D0 Leon (Mod-Raum) | Marvin | Erstes Treffen Leon–Marvin, gebissener Chef, Funk-Plan, 28 Z. | M (Z. 198, Juvenal) | Wiedersehen in 11B (D11B #10 „Du lebst!“ setzt eine frühere Trennung voraus) | #084 Satz eines anderen Mannes | möglich |
| 10F0 Leon (Mod-Raum) | Ada (neu gesetzt), Marvin | Neue Szene an den Monitoren, 18 Z. (10F0 #6–23), Karte → 11C0 | P NL (cut_10f0, R35 :40-67) | – | – | nein (keine Quelle dagegen) |
| 10F1 Elza (Mod-Raum) | – | Funkszene, 3 Z. (10F1 #40–42) | P NL (funk10f1_r39, R39 :40-43) | **gestützt**: D102 #4 „über Funk konnte ich keine Hilfe rufen“ | – | nein |
| 1141 Elza (Mod-Raum) | John | Gorilla bricht ein, John lenkt ab („Don't go to the rooftop!“); Port: Klopf-Schleife, help01–05, John durch die Dachtür, Z. 41/51/52 | M (Z. 338) + P NL (dining_r38/r39, R37–R39) | – | **#012** John: „Elza! Geh nicht aufs Dach!“ in 114 | nein |
| 1041/1031/1061/1131/1051 Elza | Zombies | Panikphase, Johns Ruf, Türsperren, Durchbruch-Montage | P NL (johnruf_r39, elzazombies_r39, R39) | – | – | nein |
| 1150 Leon | Irons | Mod-Gespräch 1150 #4–14 (Hubschrauber, Funk, Karte); Port: zweite Szene, Irons stirbt, 8 Z. (#22–29, Stimme main15–22), danach tot auf der Couch; Knall-Montage 1130/1060/1040/1030 | M (Z. 192, Juvenal) + P NL (irons_tod_1150 R35, szenenleon_r36) | D115 #0 „Mir geht es gut…“; **D11B #12/#13** „Und der Chef?“ – „Verstehe… verdammt!“ | #089 Leon am Sofa | nein (Tod gestützt durch D11B #12/#13) |
| 1151 Elza | Irons (tot) | Irons liegt von Anfang an tot da, (1151,20) „He has already passed…“ | P NL (elza_r36, R36 :36, bestätigt :123) | **Irons lebt** und sagt D115 #0 (`ROOM1151 @0xE5E`, `@0xE66`) | **#016** Elza am Sofa, Untertitel 「……レオン」, Sprecher wahrscheinlich der Chef | **ja** (bewusste Nutzerentscheidung) |
| 11A0 / 11A1 (Mod-11A) | Marvin / John, Sherry | Waffenkammer der Mod (Disc-201-Bild = Mod-11A) | M (Z. 56, 253, 281) | 11A ohne NPC | – | nein |
| 11B0 Leon | Sherry, Ada, Marvin (main00 geparkt, Szenen setzen sie) | Sherry läuft vorbei („H-he!“), Marvin „Leon! Du lebst!“, Kanaldeckel-Plan D11B #10–21; Port: Marvin schließt die Werkstatt ab (#20/21, Stimme main13/14), Parking Lot Key | D (#10–21 auf der Disc tot) + P NL (szenenleon_r36 J, schluessel_r36, R36 :37-45) | gleich, #10–21 tot | **#169–#172** Leon, Marvin, Ada, Kanaldeckel-Plan | nein |
| 11B1 Elza | Sherry; Ada, Marvin geparkt | Sherry „warte im Wagen“; Mod: Sherry verschwindet (#10/#11); Werkstatt-Tür bleibt zu | D + M (Z. 362) + P NL (werkstatt11b1_r41, R41) | Sherry wartet | – | möglich, gering |
| 11C0 Leon | Ada (2×), Marvin (Port) | Ada: „Marvin ist umgekehrt…“, Gorilla-Kampf; Port-Montage: Ada und Marvin hören den Knall (#10–14, #40) | D + P NL (szene11c0_r37, parkplatz_r39) | D11C #0–9 (#7–9 tot) | #127 = D11C #3 | **ja, gering**: 11C0 #7 spricht Ada statt Leon (Disc-Sprecher nur erschlossen) |
| 1210 Leon | – | (1210,20) „The cell door is open…“ | P NL (story_r36 P2, R36 :125) | Tür offen (`ROOM1210 @0x1E30`) | – | nein |
| 1211 Elza | John | Elza befreit John aus der Zelle („Roy sent me“), 10 Z., Unlock-Ton | M (Z. 278) + P (Ton, R36 :46) | John steht nicht im Skript, Tür bei Elza zu (`ROOM1211 @0x1E0E`) | – | möglich |
| 1260 / 1261 (Mod-126) | Ada, Marvin / John | Begleiter gehen durch den Kanalgang, ohne Text | M (Z. 60, Juvenal) | – | – | nein |

### b.2 STAGE 2

| Datei | Figuren | Was der Port zeigt | Herkunft | Disc | 80 % | Widerspruch |
|---|---|---|---|---|---|---|
| 2000 Leon | Ada, Marvin (bei (4,244)) | Leon will dem Mädchen nach, Gruppe trennt sich, 12 Z.; Port: Leitersperre (2000,40) | M (Z. 199, Juvenal) + P NL (ventilkette_r41) | ohne Text, ohne NPC | **#103–#105** Leon, Ada, Marvin in 200 (マーヴィン００ ２); **B 5:04.5** im Kanal | nein (Besetzung gestützt, Wortlaut unbelegt) |
| 2001 Elza | John (2 Plätze), Sherry | Elza und John trennen sich, Sherry hört ihren Vater, Taschenuhr der Mutter, 15 Z. | M (Z. 363-364, „nach Inflames' Fotos“) | ohne Text | **#037** Elza kniet vor Sherry 「…まあ 大きな時計ね！」 | nein |
| 2011 Elza (Mod-201) | Sherry | Mod-Szene | M | – | – | nein |
| 2030/2050/2010 | – | Ventilkette, Warnung (2030,46) mit Stimme, Ertrinken, Dokument „Technician's Note“, Plakat | P NL (ventilkette_r41, R41) | 203 nur Leon-Datei, ohne Text | – | nein (Mechanik) |
| 2080/2081, 2090/2091 | Alligator, Zombies, Marvin bzw. John | Ankunftsszene, Leiche mit Schlüssel gefressen; Kanister-Szene (2080,42)/(2081,42) mit Stimme | P NL (gatorszene_r41, kanister2080_r41, R41) | 209 nur Leon-Datei, ohne Text | – | nein |
| 20B0 / 20B1 | – | Flutszene macht 20B0 spielbar; Johns Disc-Platz in 20B1 fehlt | P NL (ventilkette_r41) | 20B1: John, aber keine Tür dorthin | B 5:38.6 weiße Wolke an der Tür (20B Cut 7) | nein |

### b.3 STAGE 3–6 (Kurzform; je Raum ausführlich in §g)

| Datei | Figuren | Was der Port zeigt | Herkunft | Widerspruch |
|---|---|---|---|---|
| 3000 / 3001 | Marvin / John, Sherry | D300 #2–8 bzw. #9–16 fast wörtlich; 3000 #1 (Marvin als Spielfigur) lebt (Cmp V16=13 `@0x1648`) | D | 3001 #6 John→Elza (gering) |
| 3020 Leon → 3060 | Marvin | **spielbarer Marvin**: Save V16=13 `ROOM3020 sub02 @0xD7A`, zurück `ROOM3060 sub03 @0x10D2/@0x10DE`; Mod-Dialog 3020 #0–2 | M (README Z. 23) | nein (Disc plant PL0D + D300 #1, BERICHT §7) |
| 3061 Elza → 30C1 → 3091 | Sherry, John; in 3091 Elza als NPC (0x42) | **spielbare Sherry**: Save V16=15 `ROOM3061 sub02 @0x011F6`, zurück `ROOM3091 sub07 @0x1D9C/@0x1DA8`; ohne Waffe | M (Z. 84, 258) + P NL (sherry_r41, R41 :85-87) | nein (#050–#055 Sherry allein in 309/30C) |
| 3070 / 3071 | Ada, Marvin / John, Sherry | D307-Aufzugsszene mit vertauschten Rollen | D, ABGEWANDELT | **ja, gering–mittel** (D307 #0–4: Ada befiehlt; D307 #9/#11: Sherrys Insiderwissen fällt weg) |
| 3091 | Elza-NPC, John | D309 #1–20 wörtlich, Mod-Schluss #22–30 statt D309 #21–25 | D + M (Z. 304/366) | nein (gleicher Ausgang: Elza allein, John mit Sherry) |
| 30C1 | Marvin | Elza trifft Marvin; Port überschreibt #3/#4/#7 | M (Z. 367) + P NL (story_r36 P4) | möglich (Marvin nennt „Leon Kennedy“; #058 ohne Text) |
| 30E0 / 30E1 | Ada, Marvin / John, Sherry | D30E-Szenen mit Fehllesungen | D, ABGEWANDELT | **ja, mittel** (30E0 #3/#5 Marvins Beinwunde verschwiegen; 30E1 #2/#3 John „gebissen“; #14–17 Rollen vertauscht) |
| 4000 / 4001 | Ada, Marvin / John, Sherry | D400; Leon hält auf (D400 #5–8, auf der Disc tot) | D | 4001 #6/#9/#10 Elza→John (gering) |
| 4010 | Ada, Marvin | D401: „Ich gehöre zu Umbrella“, Impfstoff, Ada geht; #19–22 (Disc tot) laufen | D | gering (#22 „Virus Inhibitor“, #39 rechter Tisch) |
| 4031 | John | D403 #5–8 | D | #5 Elza→John (gering) |
| 4080/4081, 50A0/50A1 | roter Affe; Birkin + Spider-Man | nach dem 80%-Video | P NL (birkin_formen, R41) | nein (B 12:38, B 13:03) |
| 5011 | John, Sherry | D501-Sherry-Szene komplett (#0–37 auf der Disc tot) | D, ABGEWANDELT | **ja, mittel**: #30 Feuer statt Virusfreisetzung, #35 Schutzraum, Sprecher #0/#23/#24/#26/#33/#37 |
| 5021, 5031 | Sherry | Mod-Kurzszenen | M (Z. 378) | nein |
| 50B0 Leon | Ada | **Adas Tod**, „Annette“ (Birkins Frau, Adas Chefin), Birkin tot, Ada sprengte den Reaktor, 19 Z. | M (Z. 328, Richard Mandel) + P (story_r36 #1) | **ja** (#10/#11 gegen D501 #29); Adas Tod möglich |
| 50B1 Elza | – | (50B1,0) „It's a woman. She's dead.“ | M + P NL (story_r36, R37 :32/:156) | möglich |
| 5080/5081 | Mod-Birkin 0x30 | Generator, Kampf | M | nein |
| 5090 / 5091 | 0x4D (nur Leon); Mod-Birkin → Bahnsteig-Form → RE2-G5 + 4 Tentakel | Endkampf, danach kein Abspann | M + P NL (birkin_formen/bahnsteig R41, enemy_ai_boss_g5, Nutzer 2026-09-12/13) | möglich (#145 Kittel-Frau + Riesenwesen in 509 gestützt, G5-Gestalt unbelegt) |
| 6000 Leon | 0x4D | Bahnsteig: „Annette“ (5 Z.), Gerät öffnet das Tunneltor, der Zug fährt | M (Z. 305, 329, 382) + P (story_r36 #0/#3) | nein (#152/#154 Leon + Frau in Weiß); Name unbelegt |
| 6020 / 6021 (Mod-602) | Marvin, Ada / John, Sherry | Schutzraum mit Begleitern, Item-Box | M (Z. 75) + P (Box-Platz) | nein (D401 #16, D501 #35 Schutzraum) |
| 6030 Leon | 0x4D am Pult (sub07), John, Marvin, Sherry im Wagen (sub06, an Disc-604-Plätzen) | Leon redet mit Sherry und John, „Annette“ startet den Zug, „biker girl“, 21 Z. | M (Z. 288, 383, Richard Mandel) + P (story_r36 #1/#2/#12) | Besetzung nein; Gespräch, „Elza kämpft draußen“, „biker girl“ möglich |
| 6031 Elza | Sherry am Pult (sub07), Marvin, John, Sherry im Wagen (sub06) | Elza und Sherry, 12 Z. | M (Z. 385) + P (story_r36 #0/#1) | Besetzung nein; Ada-Zweig gestrichen (möglich) |
| 6040 / 6041 | – | **im Port nie zu sehen** (Abspann-Riegel `aot_common.c:727`); Mod: Diashow, „extended cutscene“, „Leon and Elza models“ (Z. 195, 288) | M; P NL (Nutzer 2026-09-12) | nein (Disc-604 lebt in Mod-6030 weiter) |

### b.4 Besonders genau: das Ende

| Ort | Disc (6.11.1996) | 80 % (vorläufig) | Mod | Port heute |
|---|---|---|---|---|
| 5080 Generator | keine Datei | – | Mod-Raum: Birkin-Kampf, Strom für den Zug (`sub00 @0x0072A`, msg 3) | wie Mod, Treffer-Reaktion angepasst (birkin_opfer.c) |
| 600 Bahnsteig | keine Datei | #152, #154 Leon + blonde Frau in Weiß; #143 Riesen-Birkin am Bahnsteig | 6000: 0x4D „Annette“ spricht, Gerät öffnet das Tunneltor (`sub07 @0x011A6`, Set(3,0x59)) | wie Mod, msg 0/3 umgeschrieben (story_r36). Die Bahnsteig-Form kämpft **nicht hier**, sondern in 5090 |
| 603 Führerstand | Leon: 0x4D am Pult (Ada nur bei (4,22)=1 & (4,23)=0, nie gesetzt), WARNING; Elza: Sherry (Ada bei (4,22)=1). `ROOM6030 @0x388-0x3A8` | #146, #147 Leon + Frau im Kittel am Pult | Mod-603 = Disc-603 **+ Disc-604** (Cut 6–12); 0x4D/Sherry am Pult ohne Ada-Zweig (`sub07`, Bed. (4,23)=0) | wie Mod; Tür 3 → 5090, nach Abfahrt kein Ausstieg (msg 10/20) |
| 604 Maschinenraum | Leon: Marvin (Tür aus 514), John, Sherry; `sub06` Gang, Blick `sub04` ohne Aufrufer. Elza: Marvin an der Klappliege, John, Sherry, `sub11` Blicke | #148, #151 Leon allein (nach Debug-Sprung) | Disc-604-Gruppe nach 6030/6031 verlegt (John −26055/−25784 wie Disc; Marvin an Elzas Klappliegen-Platz auch bei Leon); Mod-604 = Abspann | Mod-604 unerreichbar |
| 509 Endkampf | keine Datei | #144 Leon in 509; #145 Kittel-Frau + dunkles Riesenwesen, Feuer | Mod-Birkin, danach „THANK YOU FOR PLAYING !!!“ (`sub02 @0x12CE`) und Diashow 6040 | Mod-Birkin → Bahnsteig-Form → RE2-G5 + Tentakel; sub02 gestrichen (`scd_vm.c:1236-1250`), Tür nach 6040 verriegelt (`aot_common.c:713-731`) |
| nach dem Kampf | 605–607 ohne Datei | Finale nicht zu sehen | Titel | **kein Ende**: G5-Kadaver sinkt, freie Steuerung, Wege nur 5090 ↔ 6030 (`port_module_teil2.md` „Spielende“) |

Folge: Das ganze Ende ab 50B ist Mod-Text; belegt ist nur die Besetzung (Frau im Kittel am Pult und am Bahnsteig, Marvin/John/Sherry im
Zug, ein Riesenwesen in 509 und am Bahnsteig). Der Port hat **kein Ende**, die Disc auch nicht (605–607 fehlen).
