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

## (c) Textliste englisch → japanische Vorlage

Jede der **1231** englischen Port-Zeilen steht mit Datei, Id, Aufruf (lebt/tot/kein), Sprecher, Wortlaut, Disc-Vorlage (TEXTE_40-Nr.)
und Urteil in `zensus/textabgleich_stage1.md` (533 Zeilen), `textabgleich_stage23.md` (315) und `textabgleich_stage456.md` (383),
jeweils mit Tabelle T2 „welche Port-Zeile trägt welche Disc-Zeile“. Japanischer Wortlaut und deutsche Übersetzung der Vorlagen:
`analysis/story_begegnungen/TEXTE_40.md`. Urteile:

| Urteil | Bedeutung | STAGE 1 | STAGE 2–3 | STAGE 4–6 | Summe |
|---|---|---|---|---|---|
| WÖRTLICH | dieselbe Aussage, nur Übersetzungsfreiheit | 59 | 88 | 139 | **286** |
| SINNGEMÄSS | gekürzt/umformuliert, keine neuen Fakten | 20 | 26 | 17 | **63** |
| ABGEWANDELT | gleicher Anlass, aber andere Fakten/Sprecher oder Disc-Zeile verteilt | 31 | 45 | 43 | **119** |
| MOD-TEXT | keine Disc-Vorlage | 421 | 153 | 175 | **749** |
| PORT-TEXT | story_r36 (Umschreibung oder neu) | 2 | 3 | 9 | **14** |

Lesehilfe und Auslegungen der Prüfer (Kopf der drei Dateien): kurze Systemsätze („It's locked.“) mit wortgleicher Disc-Zeile in einem
anderen Raum tragen „(Baustein)“ — belegt ist dann der Wortlaut, nicht der Ort. Die STAGE-4–6-Prüfung urteilt bei auf mehrere
Textboxen desselben Sprechers verteilten Disc-Zeilen nach dem Inhalt (sonst wären in 4010 21 Zeilen ABGEWANDELT). Vom Port per
C-Code eingesetzte Texte (Ids ≥ 20 in STAGE 1/2, z. B. 10F0 #6–23, 1150 #22–29) stehen nicht in den RDTs und sind in
`port_module_teil*.md` mit Wortlaut aufgeführt; sie sind alle Port-Text ohne Disc-Vorlage.

**Wichtigste Befunde aus dem Abgleich**
- Die Disc-Dialoge sind fast vollständig im Port: nur D11C #10 (Dublette), D309 #21 und D309 #24 fehlen; D30E #4 ist zu „Manhole…?“
  verlesen und wird nie gerufen; D119 #1 (Schraubendreher) steht im Port, wird aber nicht gerufen.
- 66 Disc-Zeilen, die auf der Disc **nicht laufen** (tote Subs, Bank-8-Abfrage in 11C0), laufen im Port (Liste §0).
- Der Name der Gegenseite: Keine erreichbare Disc-Zeile nennt sie. Im Port nennen sie 30C1 #2 („I'm here with Leon Kennedy“, Mod)
  und 6030 #2 (Port: „Elza is out there fighting the monsters!“). Mod-Wortlaute mit Bekanntschaft sind vom Port umgeschrieben (P4).
- Sprecherwechsel gegen die Disc (Sprecher dort aus Grammatik/Zusammenhang erschlossen): 1021 #6, 11C0 #7, 3001 #6, 3070 #0–4,
  3080 #0, 30E1 #8/#14–17, 4001 #6/#9/#10, 4031 #5, 5011 #0/#23/#24/#26/#33/#37.
- Neue Fakten durch Fehllesung der Disc-Ersatzzeichen: 30E1 #2/#3 („gebissen“), 30E1 #12, 3070 #2 („Underground“ statt „In die
  Hölle“), 30E0 #3/#5/#6.
- Inhaltlich gegen die Disc: 50B0 #10/#11 (Ada sprengt den Reaktor ↔ D501 #29), 5011 #30 (Feuer ↔ D501 #30 Virus), 5011 #35
  (Schutzraum „sollte offen sein“ ↔ Sherry kann ihn öffnen), 3071 #6/#8 (Sherrys Insiderwissen fällt weg).

### c.1 Urteile je Datei (alle englischen Zeilen, auch ungerufene)

| Datei | Zeilen | wörtl. | sinng. | abgew. | Mod | Port |
|---|---|---|---|---|---|---|
| 1011 | 24 |  |  | 1 | 23 |  |
| 1020 | 6 |  |  |  | 6 |  |
| 1021 | 15 | 5 | 1 | 3 | 6 |  |
| 1030 | 18 |  |  |  | 18 |  |
| 1031 | 24 |  |  |  | 24 |  |
| 1040 | 2 |  |  | 1 | 1 |  |
| 1041 | 4 |  |  | 1 | 3 |  |
| 1050 | 9 | 1 |  |  | 8 |  |
| 1051 | 6 | 1 |  |  | 5 |  |
| 1070 | 21 |  |  |  | 21 |  |
| 1071 | 21 |  |  |  | 21 |  |
| 1090 | 10 | 2 | 3 | 2 | 3 |  |
| 1091 | 1 |  |  |  |  | 1 |
| 10B1 | 6 | 1 | 1 | 4 |  |  |
| 10C0 | 8 |  |  |  | 8 |  |
| 10C1 | 8 |  |  |  | 8 |  |
| 10D0 | 28 | 1 | 1 | 2 | 24 |  |
| 10D1 | 12 | 1 | 1 | 1 | 9 |  |
| 10E0 | 5 |  |  |  | 5 |  |
| 10E1 | 5 |  |  |  | 5 |  |
| 10F0 | 6 |  |  | 1 | 5 |  |
| 10F1 | 6 |  |  | 1 | 5 |  |
| 1100 | 6 |  | 2 |  | 4 |  |
| 1101 | 6 |  | 2 |  | 4 |  |
| 1110 | 10 |  |  |  | 10 |  |
| 1111 | 10 |  |  |  | 10 |  |
| 1120 | 9 |  |  |  | 9 |  |
| 1121 | 9 |  |  |  | 9 |  |
| 1130 | 6 | 1 |  |  | 5 |  |
| 1131 | 6 | 1 |  |  | 5 |  |
| 1140 | 6 |  | 1 | 1 | 4 |  |
| 1141 | 15 |  | 1 | 1 | 13 |  |
| 1150 | 15 | 1 |  |  | 14 |  |
| 1151 | 4 | 1 |  |  | 3 |  |
| 1170 | 17 |  |  | 1 | 16 |  |
| 1171 | 5 | 4 |  | 1 |  |  |
| 1180 | 3 |  |  | 1 | 2 |  |
| 1181 | 3 |  |  | 1 | 2 |  |
| 1190 | 6 | 2 |  |  | 4 |  |
| 1191 | 6 | 2 |  |  | 4 |  |
| 11A0 | 3 | 2 |  | 1 |  |  |
| 11A1 | 3 | 2 |  | 1 |  |  |
| 11B0 | 16 | 13 | 1 |  | 2 |  |
| 11B1 | 15 | 10 | 1 |  | 4 |  |
| 11C0 | 10 | 8 | 1 | 1 |  |  |
| 11D0 | 5 |  |  |  | 5 |  |
| 11D1 | 5 |  |  |  | 5 |  |
| 11E0 | 13 |  | 1 | 1 | 11 |  |
| 11E1 | 13 |  | 1 | 1 | 11 |  |
| 11F0 | 4 |  |  |  | 4 |  |
| 11F1 | 4 |  |  |  | 4 |  |
| 1210 | 1 |  |  |  |  | 1 |
| 1211 | 11 |  |  | 1 | 10 |  |
| 1230 | 12 |  | 1 | 1 | 10 |  |
| 1231 | 12 |  | 1 | 1 | 10 |  |
| 1240 | 6 |  |  |  | 6 |  |
| 1241 | 6 |  |  |  | 6 |  |
| 2000 | 12 |  |  |  | 12 |  |
| 2001 | 15 |  |  |  | 15 |  |
| 2010 | 5 |  |  |  | 5 |  |
| 2011 | 5 |  |  |  | 5 |  |
| 2020 | 4 | 1 | 1 |  | 2 |  |
| 2021 | 4 | 1 | 1 |  | 2 |  |
| 2030 | 4 |  |  |  | 4 |  |
| 2031 | 4 |  |  |  | 4 |  |
| 2040 | 6 |  |  |  | 6 |  |
| 2041 | 6 |  |  |  | 6 |  |
| 2060 | 7 |  |  |  | 7 |  |
| 2061 | 7 |  |  |  | 7 |  |
| 2070 | 1 |  | 1 |  |  |  |
| 2071 | 1 |  | 1 |  |  |  |
| 20A0 | 3 | 2 | 1 |  |  |  |
| 20A1 | 3 | 2 | 1 |  |  |  |
| 3000 | 9 | 8 | 1 |  |  |  |
| 3001 | 10 | 8 | 1 | 1 |  |  |
| 3010 | 2 | 1 |  | 1 |  |  |
| 3011 | 2 | 1 |  | 1 |  |  |
| 3020 | 7 | 1 |  |  | 6 |  |
| 3021 | 5 | 1 |  |  | 4 |  |
| 3040 | 4 | 1 | 1 |  | 2 |  |
| 3041 | 4 | 1 | 1 |  | 2 |  |
| 3050 | 8 | 1 |  |  | 7 |  |
| 3051 | 8 | 1 |  |  | 7 |  |
| 3060 | 11 | 8 | 2 | 1 |  |  |
| 3061 | 11 | 7 | 1 | 3 |  |  |
| 3070 | 6 | 1 |  | 5 |  |  |
| 3071 | 16 | 4 | 2 | 10 |  |  |
| 3080 | 2 | 1 |  | 1 |  |  |
| 3090 | 4 | 4 |  |  |  |  |
| 3091 | 33 | 18 | 6 | 3 | 6 |  |
| 30A0 | 10 |  |  |  | 10 |  |
| 30A1 | 10 |  |  |  | 10 |  |
| 30C0 | 2 | 1 | 1 |  |  |  |
| 30C1 | 11 | 1 | 2 |  | 5 | 3 |
| 30E0 | 16 | 7 | 1 | 7 | 1 |  |
| 30E1 | 19 | 6 | 1 | 12 |  |  |
| 4000 | 10 | 6 | 1 | 2 | 1 |  |
| 4001 | 15 | 8 |  | 5 | 2 |  |
| 4010 | 44 | 33 | 4 | 5 | 2 |  |
| 4011 | 8 | 3 | 1 | 2 | 2 |  |
| 4030 | 5 | 3 | 1 | 1 |  |  |
| 4031 | 9 | 6 | 1 | 2 |  |  |
| 4050 | 4 | 1 |  | 1 | 2 |  |
| 4051 | 4 | 1 |  | 1 | 2 |  |
| 4090 | 8 |  |  |  | 8 |  |
| 4091 | 8 |  |  |  | 8 |  |
| 40A0 | 14 | 11 |  | 1 | 2 |  |
| 40A1 | 14 | 11 |  | 1 | 2 |  |
| 40B0 | 4 |  |  | 1 | 3 |  |
| 40B1 | 4 |  |  | 1 | 3 |  |
| 5010 | 7 | 4 |  | 1 | 2 |  |
| 5011 | 46 | 32 | 1 | 11 | 2 |  |
| 5031 | 4 |  |  |  | 4 |  |
| 5040 | 1 | 1 |  |  |  |  |
| 5041 | 1 | 1 |  |  |  |  |
| 5050 | 8 |  |  |  | 8 |  |
| 5051 | 8 |  |  |  | 8 |  |
| 5060 | 15 | 6 | 2 | 2 | 5 |  |
| 5061 | 15 | 6 | 2 | 2 | 5 |  |
| 5080 | 4 |  |  |  | 4 |  |
| 5081 | 4 |  |  |  | 4 |  |
| 50A0 | 4 | 1 | 2 | 1 |  |  |
| 50A1 | 4 | 1 | 2 | 1 |  |  |
| 50B0 | 19 |  |  |  | 18 | 1 |
| 50B1 | 1 |  |  |  |  | 1 |
| 5120 | 2 | 2 |  |  |  |  |
| 5121 | 2 | 2 |  |  |  |  |
| 5140 | 1 |  |  | 1 |  |  |
| 5141 | 1 |  |  | 1 |  |  |
| 6000 | 8 |  |  |  | 6 | 2 |
| 6030 | 21 |  |  |  | 18 | 3 |
| 6031 | 12 |  |  |  | 10 | 2 |

Nicht aufgeführt: Dateien mit höchstens drei Zeilen, die alle MOD-TEXT sind (Untersuchen-Texte in Mod-Räumen).

## (d) Kandidatenliste: alle MOD- und PORT-Story-Elemente

Felder: **NL** = Nutzer-Lückenfüller (Port-Element auf Nutzerwunsch; Fundstelle AUFTRAG/Commit in `port_module_teil*.md`).
**Widerspruch** zu Disc/80 %: nein / möglich / **ja** (mit Beleg). **Gewicht**: gering / mittel / gravierend (Begegnung Leon–Elza,
Leben/Tod/Rolle, Ursache eines Hauptereignisses, Figur an anderem Ort/in anderer Rolle als in den Quellen).
Stimmen = Ordner unter `synchro/STAGE<n>/` mit Dateizahl der betroffenen Zeilen (alle Dateien: `zensus/stimmen.md`).

### d.1 MOD-Elemente (Fan-Mod „Magic Zombie Door“, im Port über die RDTs/EMS aktiv)

| Nr | Raum / Szenario | Element | Beleg Mod (README `info/Re1.5/README.TXT`) | NL | Widerspruch | Gewicht | Stimmen |
|---|---|---|---|---|---|---|---|
| M1 | 1240/1241 beide | RE2-Vorspann (Chris, Jill, T-Virus) als Spielstart | Z. 22 | nein (Elza-Stimme R38: ja) | möglich: nichts belegt für einen 1.5-Vorspann dieses Inhalts | gering | room1240 6, room1241 6 |
| M2 | 1170 Leon | Hubschrauber startet mit Elliot und Pilot ohne Leon | Z. 103, 160 | nein | möglich: B 0:19 zeigt Leon allein („おい 冗談だろ？“), Elliot/Pilot unbelegt | gering | room1170 11 |
| M3 | 1011 Elza | Roy lebt verletzt, schickt Elza zu John | Z. 277 | teils (Diary, main16.5) | nein: #027/#028 | – | room1011 17 |
| M4 | 1211 Elza | Elza befreit John aus der Zelle | Z. 278 | nein (Ton R36) | möglich: Disc ohne John in 121, Tür bei Elza zu | gering | room1211 10 |
| M5 | 1141 Elza | Gorilla, John lenkt ab, „Don't go to the rooftop!“ | Z. 338 | Port erweitert (R37–R39) | nein: #012 | – | room1141 12 |
| M6 | 10D0 Leon | erstes Treffen Leon–Marvin, Chef gebissen, Funkplan | Z. 198 | nein | möglich (D11B #10 setzt frühere Trennung voraus, passt) | gering | room10D0 15 |
| M7 | 1150 Leon | langes Irons-Gespräch (#4–14) | Z. 192 | nein | möglich: Disc nur D115 #0 | gering | room1150 10 |
| M8 | 1020 Leon | Roys Leiche, Leon kennt Roy | Z. 265 | Port: erst nach Strom (R36) | möglich: Leon begegnet Roy auf der Disc nie | gering | – |
| M9 | 1050 / 1090 Leon | Ada-Kurzszene, Feuerlöscher-Rätsel | Z. 105, 333 | Port erweitert | nein | – | room1050 1 |
| M10 | 10B1 Elza | John schlägt die Kanaltunnel vor (#5) | Z. 359 | nein | möglich: D10B #3 nur „kürzer“ | gering | room10B1 (4 abgew.) |
| M11 | 11B1 Elza | Sherry verschwindet aus dem Wagen (#10/#11) | Z. 362 | nein | möglich (Motiv aus D403 #5/#6) | gering | room11B1 2 |
| M12 | 2000 Leon | Kanal: Leon will dem Mädchen nach, Trennung | Z. 199 | nein | nein: #103–#105, B 5:04.5 (Besetzung) | – | room2000 12 |
| M13 | 2001 Elza | Kanal: Sherry hört den Vater, Taschenuhr | Z. 363–364 | nein | nein: #037 „große Uhr“ | – | room2001 15 |
| M14 | 3020→3060 Leon | spielbarer Marvin, Fenster-Dialog | Z. 23 | nein | nein: Disc plant PL0D + D300 #1 | – | room3020 3 |
| M15 | 3061→30C1→3091 Elza | spielbare Sherry | Z. 84, 258 | Port: ohne Waffe (R41) | nein: #050–#055 | – | – |
| M16 | 3091 Elza | Trennung in 309 (#22–30) statt D309 #21–25 | Z. 304, 366 | nein | nein (gleicher Ausgang) | – | room3091 6 |
| M17 | 30C1 Elza | Elza trifft Marvin, Marvin nennt „Leon Kennedy“, Ada | Z. 367 | Port schreibt #3/#4/#7 um (R36) | möglich: #058 ohne Text; Leons Name fällt im Elza-Lauf sonst nur in #016 | mittel | room30C1 5 (+3 PORT-TEXT) |
| M18 | 30E0 Leon | Marvins Beinwunde verschwiegen (#3/#5), „tunnel“, „Manhole“ | Übersetzung | nein | **ja**: D30E #3 „am Bein erwischt, kann nicht laufen“ | mittel | room30E0 main03, main05 |
| M19 | 30E1 Elza | John „gebissen“, sucht Medikamente (#2/#3); Rollentausch #14–17; #12 | Übersetzung | nein | **ja**: D30E #12 „umstellt“, D30E #20–22 John drängt hinein | mittel | room30E1 main02, 03, 12, 14–17 |
| M20 | 3070 / 3071 | Aufzugsszene: Rollen vertauscht, „Underground“, Insiderwissen fehlt | Übersetzung | nein | **ja**: D307 #0–4, #9, #11 | gering–mittel | room3070 5, room3071 10 |
| M21 | 4001, 4031, 1021, 11C0, 3001, 3080 | einzelne Sprecherwechsel | Übersetzung | nein | **ja** (Sprecher auf der Disc erschlossen) | gering | je 1–3 Dateien |
| M22 | 5011 Elza | Impfstoff-Szene: #30 Feuer statt Virus, #35 Schutzraum, Sprecher #0/#23/#24/#26/#33/#37, #19 | Übersetzung | nein | **ja**: D501 #30, #35, Sprecher | mittel | room5011 main30, 35, 00, 23, 24, 26, 33, 37, 19 |
| M23 | 50B0 Leon | **Adas Tod**; „Annette“ = Birkins Frau, Adas Chefin; Birkin tot; Ada sprengte den Reaktor | Z. 328 | Port: #1 umgeschrieben (R36) | **ja** für #10/#11 (D501 #29: das Monster zerstörte die Energie); Tod möglich (Disc: Ada-Zweig lebend in 603, D401 #21/#22 Ada kehrt zurück) | **gravierend** | room50B0 16 (+1 PORT-TEXT) |
| M24 | 50B1 Elza | Elza findet Adas Leiche | Mod | Port: „a woman“ (R37) | möglich (Kamiya: Ada taucht im Szenario der Heldin kurz auf) | mittel | – |
| M25 | 5021, 5031 Elza | Sherry-Kurzszenen | Z. 378 | nein | nein | – | room5021 1, room5031 1 |
| M26 | 5080 | Generator, Birkin-Kampf, Strom für den Zug | Mod | nein | nein | – | – |
| M27 | 5090 Leon | 0x4D im Kampfwagen, Mod-Birkin, Spielende „THANK YOU“ | Mod | Port ersetzt Kampf, streicht Ende | möglich (#145 Kittel-Frau + Wesen in 509 passt) | gering | – |
| M28 | 6000 Leon | „Annette“ am Bahnsteig, öffnet das Tunneltor | Z. 305, 329, 382 | Port: #0/#3 umgeschrieben | nein (#152/#154); Name „Annette“ unbelegt | gering | room6000 3 (+2 PORT-TEXT) |
| M29 | 6020/6021 | Begleiter im Schutzraum | Z. 75 | nein | nein | – | – |
| M30 | 6030 Leon | Disc-604-Gruppe in den Führerstand-Wagen verlegt; Leon redet mit Sherry/John; „Elza kämpft draußen“; „Annette“ startet den Zug; „Where's that biker girl?“ | Z. 288, 383 | Port: #1/#2/#12 umgeschrieben | Besetzung nein (Disc 604, Kamiya „Marvin bis zum Schluss“); Gespräch möglich; „biker girl“ (Annette kennt Elza) möglich | mittel | room6030 15 (+3 PORT-TEXT) |
| M31 | 6031 Elza | „a policeman fights outside, start the train“ | Z. 385 | Port: #0/#1 umgeschrieben | möglich | gering | room6031 7 (+2 PORT-TEXT) |
| M32 | 6030/6031 | Ada-Zweig im Führerstand entfernt (Mod-sub07 ohne Ada) | Mod | nein | möglich: Disc `ROOM6030/6031 @0x388-0x3A8` (Flag (4,22)/(4,23), nie gesetzt); Kamiya S. 261 | mittel | – |
| M33 | 6040/6041 | Abspann-Diashow, erweiterte 604-Szene, Leon- und Elza-Modell | Z. 195, 288, 321 | Port riegelt ab (Nutzer) | nein (im Port unsichtbar) | – | – |
| M34 | viele | Mod-Namen und -Begriffe: Irons, Branagh, Kennedy, Elliot, Annette, Bill Johnson, Knights Construction, Green/Blue/Red Master Keycard, Virus Inhibitor, T-/G-Vaccine, Spark Plug | Mod | nein | möglich: Disc nennt den Chef nie, Ada heißt schon エイダ, Karte = „ID-Karte“ | gering | – |

### d.2 PORT-Elemente (C-Code)

Alle Zeilen mit Fundstelle, Datei:Zeile und Commit: `port_module_teil1.md` (71) und `port_module_teil2.md` (70). Hier gruppiert.

| Nr | Raum / Szenario | Element | Modul, Anlage-Commit | NL | Widerspruch | Gewicht | Stimmen |
|---|---|---|---|---|---|---|---|
| P1 | 1150 Leon | Irons stirbt (zweite Szene), Knall-Montage 1130/1060/1040/1030, Karte → 10F0 | irons_tod_1150 `059664db1`, szenenleon_r36 `3d394d711` (R35 :69-82, R36) | ja | nein: gestützt durch D11B #12/#13 | – | room1150 main15–22 |
| P2 | 1151 Elza | **Irons von Anfang an tot**, „He has already passed…“ | elza_r36 `3eaf631d5` (R36 :36, bestätigt :123) | ja | **ja**: Disc 1151 Irons lebt mit D115 #0; 80 % #016 | **gravierend** | – |
| P3 | 10F0 Leon | Ada-Szene an den Monitoren (18 Z.), Karte → 11C0/1150 | cut_10f0 `0908b3dfe` (R35 :40-67) | ja | nein | – | room10F0 17 |
| P4 | 1050 Leon | Ada ruft hinter der Tür, Türsperre bis zur Rettung | adaruf_1050 `e6f46443e` (R34n, R35) | ja | nein (D109 #0/#2) | – | room1050 main09–11 |
| P5 | 11C0 Leon | Montage: Ada und Marvin hören den Knall, Marvin rennt los; „I cannot leave Ada alone“ | szene11c0_r37 `914e677a2`, parkplatz_r39 `6bbc46d08` (R35–R39) | ja | nein (D11C #1/#2) | – | room11C0 5 |
| P6 | 11B0 Leon | Marvin schließt die Werkstatt ab, Tür bleibt zu | szenenleon_r36 (Spur J, R36 :37-44) | ja | nein | – | room11B0 main13, main14 |
| P7 | 10F0/10F1/11B0/1011/1131 | Parking Lot Key, Prison Key, Dining Room Key; Roy gibt keinen Schlüssel mehr | schluessel_r36, dining_r38 `6ae7118ba` (R36–R38) | ja | nein (Mechanik) | – | – |
| P8 | 1210 Leon / 1091 Elza | Spur-Texte „cell door is open“, „burnt-out shed“ | story_r36 `7be36ff32` (R36 :125, :127) | ja | nein (Disc-Zustände) | – | – |
| P9 | 1020 Leon | Roys Leiche erst nach Strom | story_r36 (R36 :113) | ja | möglich (wie M8) | gering | – |
| P10 | 30C1, 6030, 6031, 50B0, 6000, 50B1 | Text-Sweep „nie begegnet“ (12 Zeilen) | story_r36 (R36 :121, R37 :32/:156) | ja | nein für die Begegnung (BERICHT §1); 6030 #2 nennt Elza, #11 bleibt | – | 11 Dateien PORT-TEXT (Wortlaut alt/neu nur durch Anhören klärbar) |
| P11 | Elza STAGE 1 | Kette 1021 → 1011 → 1131 → 1141, Phase H (John auf dem Dach, Klopfen, help01–05), Johns Ruf 1041/1131, Panikphase, Durchbruch-Montage 1041/1061/10C1, 1121 Fenster/Krähe/Hund | dining_r39 `46916d791`, johnruf_r39 `024692703`, elzazombies_r39 `cc66567d3`, hund1121_r39 `c8e2b02cb` (R37–R39) | ja | nein (1141 gestützt durch #012) | – | room1141 9, help01–05, door_sound |
| P12 | 10F1 Elza | Funkszene | funk10f1_r39 `fe2c19358` (R39 :40-43) | ja | nein (D102 #4) | – | room10F1 2 |
| P13 | 1011/1021 Elza | Roy-Modell/Animationen, main16.5, Schuss-/Sterbe-Töne | roy_r37 `34b24c8a3`, elza_r36 (R36 :47-50, R37) | ja | nein | – | room1011 main16.5 |
| P14 | mehrere | Dokumente: Irons Diary, Elliot's Diary, Police Officer's Final Diary Entry, Roys Diary, Technician's Note, Plakat | dokumente_r34, tagebuch_r38, ventilkette_r41 (R30, R34n, R38, R41) | ja | möglich (Elliot ist Mod-Figur) | gering | – |
| P15 | STAGE 2 | Ventilkette, Flut 20B0, Warnung mit Stimme | ventilkette_r41 `b64c09dcf` (R41) | ja | nein | – | room2030/2031 main46 |
| P16 | 2080/2090 | Alligator-Ankunft, Kanister-Szene mit Marvin/John | gatorszene_r41 `079507918`, kanister2080_r41 `3d5812dcc` (R41) | ja | nein (Disc-EXE: 208/209 = WANI BOSS) | – | room2080/2081 main42 |
| P17 | 3061–3091 Elza | Sherry spielbar ohne Waffe | sherry_r41 `750bfe363` (R41 :85-87) | ja | nein | – | – |
| P18 | 4080, 50A0 | roter Affe; Birkin wirft Spider-Man (nach 80%-Video) | birkin_formen `a48c4d4f0` (R41) | ja | nein (B 12:38, B 13:03) | – | – |
| P19 | 5090/5091 | Endkampf: Bahnsteig-Form, Rückstoß-Szene, RE2-G5 + 4 Tentakel | birkin_formen, birkin_bahnsteig `4b044fe98`, enemy_ai_boss_g5 `33ecb6193` (Nutzer 2026-09-12/13, R41) | ja | möglich: #143 zeigt das Riesenwesen **am Bahnsteig 600**, der Port lässt es nur in 509 kämpfen; G5-Gestalt unbelegt | mittel | – |
| P20 | 5090 → 6040 | Abspann gestrichen, **kein Ende** | aot_common `5c8ddf4dd`, scd_vm (Nutzer 2026-09-12) | ja | nein (Disc hat auch kein Ende) | Lücke | – |
| P21 | STAGE 1 | Ablauf-Texte: „Chief first“ (1130/1060/1050), Zielscheiben 1190/1191, „Nothing happened.“, Fuse, „rescue John“ (1041/1051), Lagepläne, Speicher/Item-Box | diverse (R33–R41) | ja | nein | – | – |
| P22 | 1241, 1171, 1050, 1150, 11B0, 1011, 1141, 1100 | Stimmen-Sonderzuordnungen (Dateiname ≠ Id) | stimmen_r39, elza_r36, dining_r39, rekorder_r38 | ja | – | – | siehe §e |
| P23 | 11D0/1110/1100, 1150 sub04, 20A0 sub02, 1120 Plätze 5–7, 6020 Box-Platz | tote Mod-Inhalte geweckt (Nahaufnahmen, Gerät, Effekt, Texte) | nahaufnahmen_r40, untersuchen_r40, scd_room_setup (R40, Nutzer 2026-08-30) | ja (6020: unklar) | nein | – | – |

### d.3 Änderungskandidaten (Widerspruch **ja** und gravierend oder mittel)

| Nr | Element | Beleg | Gewicht | Stimmen, die neu aufgenommen werden müssten |
|---|---|---|---|---|
| K1 | 1151: Irons bei Elza von Anfang an tot (P2) | Disc `ROOM1151 @0xE5E/@0xE66` (Irons lebt, D115 #0); 80 % #016 「……レオン」 am Sofa | gravierend | keine (Port-Text). **Nutzer hat das bewusst so entschieden (R36 AUFTRAG :36, :123)** — nur zur Kenntnis |
| K2 | 50B0 #10/#11: Ada sprengte den Reaktor (M23) | D501 #29 „Das Monster hat die Energieversorgung zerstört“ (im Port 5011 #29 ebenso) | gravierend (Ursache der Explosion) | room50B0/main10, main11 |
| K3 | 5011 #30: „fire's starting to spread“ statt Virusfreisetzung (M22) | D501 #30 | mittel (Grund für den Schutzraum) | room5011/main30 |
| K4 | 30E1 #2/#3: John „gebissen“ (M19) | D30E #12 „umstellt“; Fehllesung | mittel | room30E1/main02, main03 |
| K5 | 30E0 #3/#5: Marvins Beinwunde verschwiegen (M18) | D30E #3 | mittel (Anlass für den Impfstoff in 401) | room30E0/main03, main05 |
| K6 | Rollentausch mit Story-Gewicht: 3070 #0–4, 30E1 #14–17, 5011 #23, 4001 #6/#9/#10, 3071 #6/#8 (M19–M22) | Disc-Sprecher aus Grammatik/Zusammenhang (TEXTE_40) | gering–mittel | room3070 main00–04, room30E1 main14–17, room5011 main23, room4001 main06/09/10, room3071 main06/08 |

**Beobachtungsliste (möglich, aber gravierend, kein Beleg dagegen):** Adas Tod in 50B0 (M23); „Annette“ kennt Elza (6030 #11, M30);
„die andere Hauptfigur kämpft draußen“ (6030 #2, 6031 #1); Marvin nennt Elza Leons Namen (30C1 #2, M17); Ada-Zweig im Führerstand
gestrichen (M32); Bahnsteig-Form kämpft nicht am Bahnsteig (P19); John in Elzas Zelle 121 (M4). Hier entscheidet der Abgleich mit
KATALOG_80 (ST80-K).

## (e) Sprachdateien je Element

Regel: `synchro/STAGE<n>/room<RAUM>/main<ID>.wav` (`audio_pc.c re15_voice_load_clip`). Ausnahmen (Dateiname ≠ Message-Id):
(1050, 22–24) ← main09–11; (1150, 22–29) ← main15–22; (11B0, 20/21) ← main13/14; (1171, 3) ← main02 (`stimmen_r39.c` k_tabelle);
(1011, 24) ← main16.5 (`elza_r36.c:226`); (1141, 53–57) ← help01–05 und (1141, 59) ← door_sound, auch für 1041/1131
(`dining_r39.c:550-555`); 1100/1101 ← Irons_Rekorder_Sound(.wav, 2.wav) (rekorder_r38); 1241 nutzt room1240, solange room1241
keine eigene Datei hat (elzaintro_r38; room1241 hat eigene). **Ohne Zeile:** room1141/main42.wav, main50.wav (keine Id 42/50).
Eine neue Aufnahme muss diese Zuordnung beachten: wer z. B. 1150 #27 neu spricht, ersetzt `main20.wav`, nicht `main27.wav`.

Volle Liste (489 Dateien mit Id, Zeile, Urteil, Disc-Vorlage): [`zensus/stimmen.md`](zensus/stimmen.md).

**Sprachdateien an MOD- oder PORT-Zeilen** (nur aufgelistet, keine Bewertung): MOD-TEXT 177 Dateien, PORT 52, PORT-TEXT 11.
Ordner ganz oder überwiegend auf MOD-/PORT-Zeilen: room1011 (17+1), room10D0 (15), room10F0 (17 PORT), room10F1 (2 PORT),
room1141 (12+9), room1150 (10+8), room1170 (11), room1211 (10), room1240 (6), room1241 (6), room2000 (12), room2001 (15), room2030,
room2031, room2080, room2081 (je 1 PORT), room3020 (3), room30C1 (5+3), room50B0 (16+1), room6000 (3+2), room6030 (15+3),
room6031 (7+2), room5021, room5031 (je 1), room1050 (1+3), room1041 (1+1), room11C0 (5 PORT), room11B0 (2 PORT).

**Die 11 Dateien an story_r36-Umschreibungen** (Wortlaut auf der Platte alt oder neu? nur durch Anhören prüfbar; laut Teil 2 alle
vom 2026-10-30, nicht in git): room30C1/main03, main04, main07; room6030/main01, main02, main12; room6031/main00, main01;
room50B0/main01; room6000/main00, main03.

**Dateien an Änderungskandidaten (§d.3):** room50B0/main10, main11 (K2); room5011/main30 (K3); room30E1/main02, main03 (K4);
room30E0/main03, main05 (K5); room3070/main00–04, room30E1/main14–17, room5011/main23, room4001/main06, main09, main10,
room3071/main06, main08 (K6). K1 hat keine Datei.

### e.1 Sprachdateien je Ordner nach Herkunft der Zeile

| Ordner | Dateien | wörtl./sinng. | abgew. | MOD-TEXT | PORT (Modul) | PORT-TEXT (story_r36) | ohne Text |
|---|---|---|---|---|---|---|---|
| room1011 | 19 |  | 1 | 17 | 1 |  |  |
| room1021 | 9 | 6 | 3 |  |  |  |  |
| room1031 | 2 |  |  | 2 |  |  |  |
| room1041 | 2 |  |  | 1 | 1 |  |  |
| room1050 | 4 |  |  | 1 | 3 |  |  |
| room1090 | 7 | 5 | 2 |  |  |  |  |
| room10B1 | 6 | 2 | 4 |  |  |  |  |
| room10D0 | 16 |  | 1 | 15 |  |  |  |
| room10F0 | 17 |  |  |  | 17 |  |  |
| room10F1 | 2 |  |  |  | 2 |  |  |
| room1100 | 2 |  |  |  |  |  | 2 |
| room1141 | 23 |  |  | 12 | 9 |  | 2 |
| room1150 | 19 | 1 |  | 10 | 8 |  |  |
| room1170 | 11 |  |  | 11 |  |  |  |
| room1171 | 3 | 3 |  |  |  |  |  |
| room11B0 | 15 | 13 |  |  | 2 |  |  |
| room11B1 | 11 | 9 |  | 2 |  |  |  |
| room11C0 | 15 | 9 | 1 |  | 5 |  |  |
| room1211 | 10 |  |  | 10 |  |  |  |
| room1240 | 6 |  |  | 6 |  |  |  |
| room1241 | 6 |  |  | 6 |  |  |  |
| room2000 | 12 |  |  | 12 |  |  |  |
| room2001 | 15 |  |  | 15 |  |  |  |
| room2030 | 1 |  |  |  | 1 |  |  |
| room2031 | 1 |  |  |  | 1 |  |  |
| room2080 | 1 |  |  |  | 1 |  |  |
| room2081 | 1 |  |  |  | 1 |  |  |
| room3000 | 7 | 7 |  |  |  |  |  |
| room3001 | 7 | 6 | 1 |  |  |  |  |
| room3020 | 3 |  |  | 3 |  |  |  |
| room3061 | 9 | 7 | 2 |  |  |  |  |
| room3070 | 5 |  | 5 |  |  |  |  |
| room3071 | 15 | 5 | 10 |  |  |  |  |
| room3080 | 2 | 1 | 1 |  |  |  |  |
| room3091 | 22 | 13 | 3 | 6 |  |  |  |
| room30C1 | 8 |  |  | 5 |  | 3 |  |
| room30E0 | 13 | 7 | 6 |  |  |  |  |
| room30E1 | 18 | 6 | 12 |  |  |  |  |
| room4000 | 6 | 6 |  |  |  |  |  |
| room4001 | 10 | 7 | 3 |  |  |  |  |
| room4010 | 35 | 32 | 3 |  |  |  |  |
| room4031 | 4 | 3 | 1 |  |  |  |  |
| room5011 | 38 | 28 | 10 |  |  |  |  |
| room5021 | 1 |  |  | 1 |  |  |  |
| room5031 | 1 |  |  | 1 |  |  |  |
| room50B0 | 17 |  |  | 16 |  | 1 |  |
| room6000 | 5 |  |  | 3 |  | 2 |  |
| room6030 | 18 |  |  | 15 |  | 3 |  |
| room6031 | 9 |  |  | 7 |  | 2 |  |

## (f) Platzhalter-Fallen

| Falle | Was gilt | Beleg |
|---|---|---|
| Disc-Typ 0x40/0x45/0x47 sieht aus wie Leon | ein einziger Blob (Leons Uniform-Körper, Johns Clips) für „Marvin“, Polizeichef und 0x47 — kein Leon-Auftritt | BERICHT §8, `ems.py` |
| Disc-Typ 0x4D in Leons 603 trägt Elzas Körper | Elzas Körper und Skelett mit Adas Clips = Platzhalter; im 80%-Stand steht dort eine blonde Frau im Kittel (#146/#147) | BERICHT §8, `belege/vergleich_4D_gegen_elza.png` |
| Disc-3000 stellt John neben Leon | Platzhalter, der Text nennt Marvin; Mod/Port setzen Marvin (richtig) | D300 #2–8 |
| Parkposition (−30000, −30000) | kein Auftritt: Disc 1090/1091 Ada, 10B0 John, 11B0/11B1 Sherry/Ada/Marvin; im Port bleiben 11B0/11B1/10B1 geparkt, die Szenen setzen die Figuren selbst | `port_zensus.json` npcs.park |
| Subs ohne Aufrufer | laufen nicht: Disc 6030 sub02–10/15–17, 6040 sub02–05/09/10, 6041 viele, 501 #0–37, 11B #10–21 (Bank 8). Der Port spielt die Mod-Fassungen, in denen viele dieser Szenen verdrahtet sind → im Port läuft Capcoms **Plan**, nicht der Disc-Ablauf | `disc_zensus.json` reach; `k12_subs.py` |
| Gleiche Raumnummer = gleicher Raum | falsch für 603/604 (Mod-603 = Disc-603 + Disc-604 Cut 0–6), 604 (Mod-Abspann), 106, 116, 118, 11A, 203, 207, 409 (0 gemeinsame Kameras); Disc 201 = Mod 11A | §a.3, `video_b.md` |
| Typ 0x42 = Ada | in Elzas 3091 ist 0x42 **Elza als NPC** (Mod-CDEMD1 Sektor 1505), weil Sherry dort Spielfigur ist | `befunde_runde41/K_stage3szenen_b.md` K1.3 |
| Typ 0x47 = Roy | bei Leon Elliot (Mod-Figur), bei Elza Roy (CDEMD1) | `re15_roy_r37.h` |
| Name „Annette“ | steht in keiner Capcom-Quelle; Disc-Text kennt nur エイダ, keinen Namen für die Kittel-Frau | BERICHT §8, §11 |
| Spielbare Begleiter | Disc: vorbereitet (PL0D/PL0E/PL0F, D300 #1, D309 #8), aber kein Schreiber für work_vars[0x10]; Mod/Port: verdrahtet (3020→3060 Marvin, 3061→3091 Sherry). John (PL0E) nirgends spielbar | §b.3, BERICHT §7 |
| Englischer Mod-Text als Beleg | nie: auch WÖRTLICH-Zeilen belegen nur die Disc-Vorlage, nicht den englischen Zusatz | Auftrag |
| Ersatzzeichen der Disc (東, 怖, 度 …) | Lesart; daraus entstanden Mod-Fehler (30E1 „gebissen“, 3070 „Underground“) | TEXTE_40 Kopf |
| 80%-Fotos nach Debug-Sprung | #148/#151 (604 Leon allein) belegen keinen Ablauf | BERICHT §8 |
| Disc-Flags ohne Schreiber | (4,22)/(4,23) Ada-Zweig 603, (3,98) Zelle, (3,134) Gitter 103: nichts auf der Disc setzt sie | BERICHT §4, §6 |
| **Port-Fehlerkandidat ROOM1151 msg 20** | `elza_r36.c:272` legt „He has already passed…“ unter Id 20 ab, `hebetisch_cursor_1150.c:314` überschreibt Id 20 bei jedem Fehlklick mit „Nothing happened.“ (`re15_hebetisch_cursor.h:183`). Danach zeigt Irons wohl den falschen Text. **Nur gelesen, nicht gemessen** (messen: 1151, Hebetisch daneben klicken, Irons anklicken, `RE15_MSG_LOG=1`) | beide Teilprüfer unabhängig |
| Ein Raum, zwei Stände | Der Port trägt den Stand master `789f6e681`; parallele Zweige (r41_g5auftritt, r41_stage3szenen_a) sind nicht erfasst | `git worktree list` |

## (g) Lückenliste STAGE 3–6: was belegt ist, was der Port zeigt, wo Story fehlt

Grundlage für eigene Dialoge des Nutzers. „Belegt“ = Disc (TEXTE_40-Nr., Figuren aus `disc_zensus.json`), 80 % (BERICHT §10,
`video_a.md`/`video_b.md`, vorläufig bis KATALOG_80), Kamiya (Guide Book S. 261). „Lücke“ = kein Text, keine Szene, unklarer Anlass
oder ein Beleg, den der Port nicht zeigt.

### g.1 STAGE 3 (Fabrik)

| Raum | Szen. | Belegt | Port heute | Lücke |
|---|---|---|---|---|
| 300 | L | D300 #2–8: Leon + Marvin, tote Polizisten, Umbrella-Ermittler, Ada vorgegangen, „Miene nicht verzogen“; D300 #1 Marvin als Spieler; 80 % #118 Leon + hockender Mann, Untertitel 「みんな ラクー…」 (anders als D300 #3) | Marvin, D300 wörtlich | #118-Wortlaut und Mann offen (ST80-K) |
| 300 | E | D300 #9–16: Elza, John, Sherry, Chemiefabrik, Versteck; 80 % #044, #048 (Variante von #12) | John, Sherry, fast wörtlich | – |
| 301 | L/E | nur Kartenschloss; 80 % #068/#069, B 3:35 Elza + Zombies | Untersuchen | keine Story belegt |
| 302 | L | Disc-Datei ohne Text (anderer Raum, 2/9 Kameras); 80 % #070/#071 Elza + Streifenwagen, #115 Leon + zweite Figur | Marvin, Mod-Dialog am Fenster, **Marvin wird Spielfigur** | Anlass und Dialog des Figurenwechsels unbelegt (Disc nur vorbereitet) |
| 302 | E | keine Disc-Datei | John, Sherry, Mod-Untersuchen | keine Story |
| 303 | L/E | ohne Text; 80 % #057 Elza | – | – |
| 304 | L/E | ohne Text; 80 % #062–#066 Elza, Zombies, Gore; #113/#114 Leon, Gore-Wesen | Kisten-/Panel-Texte (Mod); README Z. 280 nennt eine Sherry-Szene bei Leon, im SCD-Zensus kein Sce_em_set 0x4B in 3040 | Sherry-Szene bei Leon unbelegt und nicht nachgewiesen |
| 305 | E (nur Mod) | 80 % #060/#061/#065: Elza, Rechner „Now Running…“, „Break“, 「システムエラー」 | Rechner-Notiz „Red Master Keycard“ (Mod) | 80%-Rechnertext fehlt im Port |
| 306 | L | D306 #9 „Irgendwann ist das Schloss aufgemacht worden“, Gitter am Boden, 3 Hunde; 80 % #116 Leon + Marvin | Marvin; #9 „For some reason“ | Zeitbezug „irgendwann“ sinngemäß verloren (gering) |
| 306 | E | D306 #1–8 Sherrys geheimer Eingang; 80 % #049, B 3:19.5 (wörtlich gleich) | wörtlich, Sherry wird Spielfigur | – |
| 307 | L | D307 #0–4 Ada: Aufzug, „In die Hölle…“; 80 % #117, #140 (ホール…), #141/#142 Birkin, B 13:09–13:56 Leon + Birkin (Marvin nicht im Bild) | Ada, Marvin, Birkin 0x30; Rollen vertauscht | Rollen (K6); Birkin-Begegnung ohne Text |
| 307 | E | D307 #5–15 Monster kam aus dem Aufzug, Sherry kann ihn bedienen; 80 % #073–#081 Birkin am Tor, Elza an der Kontrollbox, B 3:54 Tor bricht, B 7:10.5 Elza + Birkin | John, Sherry, Birkin; 10 Zeilen abgewandelt | Sherrys Insiderwissen (D307 #9/#11) |
| 308 | L | D308 #0/#1 „…hartnäckiger Kerl…“, „…also doch…“ (Fragmente) | Ada, Marvin, Birkin 0x36; #0 Ada | Anlass der Zeilen unklar |
| 308 | E | keine Elza-Zeile | John, Sherry, kein Text | – |
| 309 | L | D309 #0 „Licht ist bereits an“ | gleich | – |
| 309 | E/Sherry | D309 #1–25 Sherry-Abschnitt; 80 % #050–#055 Sherry allein, #051 「電げんがある」, #059 John-Zeile | spielbare Sherry, D309 wörtlich, Mod-Schluss #22–30 | 80%-Zeile #051 nicht im Port (prüfen, ST80-K); D309 #21/#24 fehlen |
| 30A, 30B, 30D | L/E (nur Mod) | 80 % #067/#072 Elza in 30A | Mod-Untersuchen | keine Story |
| 30C | E (nur Mod) | 80 % #058 Elza + Marvin am Tresen, #053 Sherry allein; Kamiya: Marvin Leons Partner | Marvin-Szene (Mod, Port umgeschrieben) | **Anlass und Dialog Elza–Marvin unbelegt** |
| 30C | L (nur Mod) | 80 % #139 Leon am Tresen | nur Schloss-Text | Leon in 30C ohne Story |
| 30E | L | D30E #0–10 Ada taucht auf, Marvin am Bein verletzt, Halle fällt; 80 % – | Ada, Marvin; Fehllesungen | Marvins Wunde (K5); D30E #4 nie gerufen |
| 30E | E | D30E #11–23 Elza sah etwas, „kein Mensch“, John will hinein; 80 % #056 Elza | John, Sherry; „gebissen“, Rollentausch | K4, K6 |

### g.2 STAGE 4 (Labor)

| Raum | Szen. | Belegt | Port heute | Lücke |
|---|---|---|---|---|
| 400 | L | D400 #0–8 Ausweis-Tür, „Ada, was ist das hier“, Leon hält auf, „Kümmere dich um Marvin“ (#5–8 auf der Disc tot) | Ada, Marvin, Gorilla-Konfrontation (Mod Z. 203), D400 läuft ganz | – |
| 400 | E | D400 #9–18 Sherry: Papas Labor, läuft los | John, Sherry; John statt Elza hält zurück | Sprecher (K6) |
| 401 | L | D401 #0–22: Wunde, „Ich gehöre zu Umbrella“, Impfstoff aus dem Virusstamm, Ada geht, Schutzraum; #19–22 tot (Marvin bricht zusammen, Ada kehrt zurück); Kamiya: Marvin bis zum Schluss | Ada, Marvin; alles läuft, 33 wörtlich | – |
| 401 | E | nur Untersuchen + „Impfstoff schnell holen“ | Untersuchen, Satz nur in 5011 | – |
| 402, 404, 405, 407 | L/E (nur Mod) | – | Mod-Texte (405: Notiz mit David/Eric) | keine Story |
| 403 | L | Untersuchen (Hauptsicherung) | gleich | – |
| 403 | E | D403 #5–8 Sherry verschwunden, Suche | John; #5 John statt Elza | gering |
| 408 | E (nur Mod; Disc nur BSS) | B 12:38 Elza + dunkelrotes Wesen | roter Affe (Port, nach Video) | Anlass ohne Text |
| 409, 40A, 40B | L/E | 40A Untersuchen; 409 anderer Raum | Mod-Texte (T-Virus) | keine Story |

### g.3 STAGE 5 (Tiefes Labor)

| Raum | Szen. | Belegt | Port heute | Lücke |
|---|---|---|---|---|
| 501 | E | D501 #0–37 (auf der Disc tot): Sherry vom Virus zerfressen, Elza holt Impfstoff, Sherry erwacht, Monster zerstörte die Energie, Virus wird frei, Schutzraum Nr. 2 | John, Sherry; Szene läuft, Sprecher getauscht, #30/#35 geändert | K3, K6 |
| 501 | L | Untersuchen + Impfstoff-Satz | Untersuchen | Leons Weg durch 501 ohne Story (Disc ebenso) |
| 502, 503 | E | – (502/503 Disc ohne NPC) | Sherry-Kurzszenen (Mod Z. 378) | Anlass unbelegt |
| 504, 506, 50A, 50C, 510–514 | L/E | Schilder/Untersuchen; 80 %: Elza in 504, 506 (Rotlicht B 10:43), 512, 514; Leon in 50C, 514; D514 #0 Frauenstimme „Es ist offen. Da hinten.“ (nie ausgelöst); D512 #1 Feuer-Sperre (tot) | Untersuchen; 5140 #0 als Systemtext ohne Sprecherin, nie gerufen; 5120 #1 läuft | **Wer ist die Frau in 514?** (Disc-Plan, Anlass offen) |
| 505, 507, 50D, 50F, 513 | (nur Mod) | EXE-Namen DENSANSHITSU, TEST | Mod-Texte | keine Story |
| 508 | L/E (nur Mod) | – | Generator, Birkin-Kampf, Strom für den Zug | Anlass unbelegt |
| 509 | L (nur Mod) | 80 % #144 Leon; #145 Kittel-Frau + dunkles Riesenwesen, Feuer | Endkampf (Bahnsteig-Form → G5), 0x4D steht im Raum | Szene Kittel-Frau + Wesen ohne Text; **kein Ende danach** |
| 509 | E | – | Endkampf ohne 0x4D | Elzas Endkampf unbelegt |
| 50B | L (nur Mod) | 80 % #155 helle Figur; B 12:25 Leon in der Maschinenhalle | Adas Tod (Mod) | Adas Verbleib unbelegt (Beobachtungsliste) |
| 50B | E | – | „It's a woman. She's dead.“ | Kamiya: Ada taucht im Szenario der Heldin kurz auf — wo, ist offen |
| 50E | L (nur Mod) | B 12:29 Leon + braune Kreatur (Film 4) | Mod-Birkin 0x30 | Kreatur ohne Text |

### g.4 STAGE 6 (Zug)

| Raum | Szen. | Belegt | Port heute | Lücke |
|---|---|---|---|---|
| 600 | L (nur Mod) | 80 % #152/#154 Leon + blonde Frau in Weiß; #143 Riesen-Birkin am Bahnsteig | 0x4D „Annette“ mit Mod-Dialog, Tunneltor, Zug fährt | Riesen-Birkin am Bahnsteig fehlt; Dialog unbelegt |
| 600 | E | – | keine Figur, 3 Untersuchen | Elzas Weg zum Zug ohne Story |
| 601 | L/E (nur Mod) | 80 % #149 Leon | Untersuchen | – |
| 602 | L/E (nur Mod) | Disc-Text: Schutzraum (D401 #16, D501 #35) | Begleiter im Schutzraum | Anlass ohne Text |
| 603 | L | 0x4D am Pult, WARNING; Ada-Zweig an (4,22)/(4,23); 80 % #146/#147 | „Annette“ startet den Zug (Mod-Dialog) | Dialog/Anlass unbelegt; Ada-Zweig |
| 603 | E | Sherry am Pult; Ada-Zweig an (4,22) | Sherry am Pult | Dialog unbelegt |
| 604 | L | Marvin (Tür aus 514), John, Sherry; Leon geht vier Wegpunkte (`sub06`), Blick zu Sherry/John und John läuft los (`sub04`, ohne Aufrufer) | Gruppe in 6030 (Mod), Leon redet mit Sherry/John | Was Leon und die Gruppe einander sagen: nicht belegt |
| 604 | E | Marvin an der Klappliege, John, Sherry; Elza blickt umher (`sub11`), Gang zu Marvin (`sub06`/`sub09`, ohne Aufrufer) | Gruppe in 6031 | Anlass Marvin an der Klappliege (Pose, Gespräch) unbelegt |
| 605–607 | L/E | nur EXE-Name DOURYOKUSHITSU, keine Datei | – | **Finale fehlt überall**; Port endet nach dem G5 ohne Abschluss |

## Offene Punkte und nächster Schritt

1. **Abgleich mit KATALOG_80 (ST80-K):** Die 80%-Spalten hier sind vorläufig (BERICHT §10, video_a/b). Zu prüfen besonders: #118
   (Fabrikhof-Wortlaut und Mann), #051 「電げんがある」, #065 Rechnertext, #143 Riesen-Birkin am Bahnsteig, #145 Szene in 509, Orte ohne
   Port-Raum (Rippen-Tunnel, Raum mit rotem Rad, Trailer-Orte), Tonspur der Complete-Disc-Filme.
2. **Messen statt lesen:** ROOM1151 msg 20 (Irons-Text gegen „Nothing happened.“), §f.
3. **Anhören:** die 11 Dateien an story_r36-Umschreibungen (§e) — sprechen sie den alten Mod- oder den neuen Port-Wortlaut?
4. **Nicht erfasst:** Stand anderer Zweige (r41_g5auftritt, r41_stage3szenen_a); Dokumenttexte im Wortlaut (nur Fundorte, §d P14).
