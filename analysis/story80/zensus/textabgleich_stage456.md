# Textabgleich STAGE 4, 5, 6: englische Port-Zeilen gegen die 40%-Disc

Spur ST80-P, Stand 2026-11-02. Nur gelesen, kein Code und keine Assets geändert.

**Quellen.** Englische Zeilen, Aufrufer und Overrides wortgetreu aus `texte_paare.md` (gleiches Verzeichnis; erzeugt aus `port_zensus.json`, `disc_zensus.json` und `re15_port/engine/src/gen/story_r36_texte.inc`). Disc-Zeilen, Sprecher und Übersetzung aus `analysis/story_begegnungen/TEXTE_40.md`. Hintergrund: `analysis/story_begegnungen/BERICHT.md` §4–§6, §9, §12. Mod-Herkunft: `info/Re1.5/README.TXT`. Raumnamen der Disc-EXE über `raumnamen.py` aus `Version_40%/PSX.EXE` (`@0x8006ff36`).

**Umfang.** Alle 383 englischen Zeilen der Dateien ROOM4xxx, ROOM5xxx, ROOM6xxx (Teil A und B von `texte_paare.md`) und alle 131 Disc-Zeilen der Räume 400, 401, 403, 40A, 501, 504, 506, 50A, 512, 514. Leon- und Elza-Datei zählen getrennt, auch wenn der Text gleich ist.

Ohne englischen Text, also nicht in T1:
- **ROOM6040 / ROOM6041** (Maschinenraum 604, laut README „extended cutscene … (by Richard Mandel)“): `texts` leer. Gegenprobe am RDT: die kodierten Wörter Leon, Elza, Sherry, John, Marvin, Annette, the kommen in beiden Dateien 0-mal vor (in ROOM6030 9-, 1-, 1-, 3-, 2-, 6-, 9-mal). Die Szene in 604 hat auch im Port keinen Text. Die Disc hat in 604 ebenfalls keinen.
- ROOM5020, 50E0/50E1, 50F0/50F1, 5100/5101, 5110/5111: Dateien ohne Text.
- 4-Byte-Platzhalter im Port-Baum: 4060/4061, 40C0–40F1, 5150–5171, 6050–6071. Die Mod-Texte für 607 (README „description texts in … 607“) liegen also nicht im Port.
- Keine `PORT-NEU`-Zeile in STAGE 4–6. Overrides gibt es nur für 50B0 #1, 50B1 #0, 6000 #0/#3, 6030 #1/#2/#12, 6031 #0/#1.

**So habe ich geurteilt.**
- WOERTLICH, SINNGEMAESS, ABGEWANDELT, MOD-TEXT, PORT-TEXT wie im Auftrag.
- **Textbox-Teilung:** Teilt der Port eine Disc-Zeile auf aufeinanderfolgende Zeilen **desselben Sprechers**, ohne den Inhalt zu ändern (in 4010: 9 Disc-Zeilen auf 21 Port-Zeilen), urteile ich je Teil nach Inhalt (meist WOERTLICH) und vermerke „Teil n/m“. Geht ein Teil an einen **anderen Sprecher** (D401 #0 → 4010 #0/#1), gilt ABGEWANDELT.
- **Raumübergreifend:** Ein Disc-Satz, den der Mod in einen anderen Raum kopiert oder abgewandelt hat, bekommt die Vorlage mit Zusatz „(raumübergr.)“: 4050 #0 ← D401 #27, 4050 #3 ← D401 #26, 40B0 #2 ← D40A #10 + #11. Allgemeine Formeln, die nicht aus der Disc-Zeile stammen, bleiben MOD-TEXT, mit Hinweis in der Bemerkung. Beispiel 50D0 #0 „There's a button here. Push it?“: inhaltlich gleich D104 #0, aber Port-1040 trägt D104 als „shutter switch“, und die Formel stammt aus den Mod-Räumen 304/305.
- **Leichte Fehlübersetzungen**, die eine Tatsache ändern, zum Beispiel links gegen rechts, „nicht benutzt“ gegen „unbenutzbar“ oder Kratzer gegen Fußspuren, zählen als ABGEWANDELT, Vermerk „leicht“.
- Sprecher: Präfix im Englischen. Fehlt es, steht „System/Untersuchen“, „System/Schild“ oder „(ohne Präfix, Spielfigur …)“. Die Disc-Sprecher in TEXTE_40.md sind aus Regie und Sprachform erschlossen (zum Beispiel weibliches わよ, männliches ぜ). Ein Sprecherwechsel heißt also: der Port widerspricht dieser Zuordnung.
- Aufruf: LEBT, wenn mindestens ein Aufrufer statisch erreichbar ist, auch hinter einer Bedingung. TOT heißt nur tote Aufrufer, „kein“ heißt keine Skriptstelle.

## Zahlen

| Urteil | Zeilen gesamt | Leon (xxx0) | Elza (xxx1) |
|---|---|---|---|
| WOERTLICH | 139 | 68 | 71 |
| SINNGEMAESS | 17 | 10 | 7 |
| ABGEWANDELT | 43 | 16 | 27 |
| MOD-TEXT | 175 | 100 | 75 |
| PORT-TEXT | 9 | 6 | 3 |
| **Summe** | **383** | **200** | **183** |

- **Disc-Zeilen ohne Port-Gegenstück: 0 von 131.** Nur teilweise vorhanden ist der Raumtext 急いでワクチンを持ってこなければ！ (D401 #18 = D501 #43): Auf der Disc steht er in 401 und 501 für beide Figuren, im Port nur in 4010 (Leon) und 5011 (Elza).
- **51 Disc-Zeilen, die auf der Disc nicht laufen, LEBEN im Port:** D400 #5–8, D401 #19–22, D501 #0–37, D50A #0–3, D512 #1. 35 davon stehen in toten Subs, 16 werden auf der Disc nicht aufgerufen. Sie zeigen Capcoms Plan, nicht den Ablauf der Disc.
- Umgekehrt haben die Port-Zeilen zu D40A #9, D501 #40 und D514 #0 keinen Aufruf. D40A #9 und D501 #40 laufen auf der Disc.
- 11 Sprecherwechsel gegen die Disc: 4001 #6, #9, #10; 4010 #1; 4031 #5; 5011 #0, #23, #24, #26, #33, #37.

## T1: englische Port-Zeilen

| Datei | Id | Aufruf | Sprecher (EN) | Englisch (wortgetreu aus texte_paare.md) | Vorlage | Urteil | Bemerkung |
|---|---|---|---|---|---|---|---|
| 4000 | 0 | LEBT | System/Untersuchen | The door is locked. The text on the / panel reads:Door is under emergency lockdown. / An ID card is required to unlock it." | D400 #0 | WOERTLICH | Panel-Text vollständig |
| 4000 | 1 | LEBT | System/Untersuchen | Will you use the Blue Master Keycard?  | D400 #1 | ABGEWANDELT | Disc: „ID-Karte benutzen?“; Port: Mod-Item „Blue Master Keycard“ (README: liegt in 401) |
| 4000 | 2 | LEBT | System/Untersuchen | You've used the Blue Master Keycard. | D400 #2 | ABGEWANDELT | Disc: Panel-Anzeige „ID bestätigt, entriegle“; Port: Item-Meldung mit Mod-Item |
| 4000 | 3 | LEBT | Leon | Leon: Ada, what is this place...? | D400 #3 | WOERTLICH |  |
| 4000 | 4 | LEBT | Ada | Ada: Just hurry up, okay?! / Let's get him to this room here! | D400 #4 | SINNGEMAESS | „him“ (Marvin) ergänzt, aus der Szene ableitbar |
| 4000 | 5 | LEBT | Leon | Leon: Go on ahead! / I'll take care of this! | D400 #5 | WOERTLICH | Disc tot (`ROOM4000 sub16` ohne Aufrufer), Port LEBT (sub09): Leons Aufhalte-Szene läuft erst im Mod |
| 4000 | 6 | LEBT | Ada | Ada: Leon...! | D400 #6 | WOERTLICH | Disc tot (`ROOM4000 sub16` ohne Aufrufer), Port LEBT (sub09) |
| 4000 | 7 | LEBT | Leon | Leon: Just get going already! | D400 #7 | WOERTLICH | Disc tot (`ROOM4000 sub16` ohne Aufrufer), Port LEBT (sub09) |
| 4000 | 8 | LEBT | Leon | Leon: Take care of Marvin! | D400 #8 | WOERTLICH | Disc tot (`ROOM4000 sub16` ohne Aufrufer), Port LEBT (sub09) |
| 4000 | 9 | LEBT | System/Untersuchen | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. | - | MOD-TEXT | Aufzug-Text; Disc 400 hat keinen. Nächste Disc-Zeile D307 #15 (Aufzug ohne Strom) sagt etwas anderes |
| 4001 | 0 | kein | System/Untersuchen | The door is locked. The text on the / panel reads:Door is under emergency lockdown. / An ID card is required to unlock it." | D400 #0 | WOERTLICH | kein Aufruf (Disc: nur Leon löst aus) |
| 4001 | 1 | kein | System/Untersuchen | Will you use the Blue Master Keycard?  | D400 #1 | ABGEWANDELT | kein Aufruf; Mod-Item wie 4000 #1 |
| 4001 | 2 | kein | System/Untersuchen | You've used the Blue Master Keycard. | D400 #2 | ABGEWANDELT | kein Aufruf; Mod-Item wie 4000 #2 |
| 4001 | 3 | LEBT | Elza | Elza: What? What is this place...!? | D400 #9 | WOERTLICH |  |
| 4001 | 4 | LEBT | John | John: Wow, this sure is something... | D400 #10 | WOERTLICH |  |
| 4001 | 5 | LEBT | Elza | Elza: Sherry, what is this place? | D400 #11 | WOERTLICH |  |
| 4001 | 6 | LEBT | John | John: Do you know anything about / this place? | D400 #12 | ABGEWANDELT | Sprecherwechsel Elza → John; Disc „Du kennst sogar diesen Ort?“ wird allgemeine Frage |
| 4001 | 7 | LEBT | Sherry | Sherry: This is dad's research center. / He should be around here somewhere. | D400 #13 | WOERTLICH |  |
| 4001 | 8 | LEBT | Sherry | Sherry: I'll go look for him! | D400 #14 | WOERTLICH |  |
| 4001 | 9 | LEBT | John | John: Sherry, hold on! | D400 #15 | ABGEWANDELT | Sprecherwechsel Elza → John |
| 4001 | 10 | LEBT | John | John: Don't go alone! | D400 #16 | ABGEWANDELT | Sprecherwechsel Elza → John |
| 4001 | 11 | LEBT | Elza | Elza: Sherry! | D400 #17 | WOERTLICH |  |
| 4001 | 12 | LEBT | John | John: Elza, let's go after her! | D400 #18 | WOERTLICH |  |
| 4001 | 13 | LEBT | System/Untersuchen | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. | - | MOD-TEXT | Aufzug-Text wie 4000 #9 |
| 4001 | 14 | LEBT | System/Untersuchen | Security Room"It's locked. | - | MOD-TEXT | Türschild „Security Room“ = EXE-Raumname 401 GUARDHOUSE; die Sperre ist Mod (README: Tür zu 401 bei Elza verschlossen) |
| 4010 | 0 | LEBT | Ada | Ada: Leon, that wound... | D401 #0 (Teil) | ABGEWANDELT | Disc-Zeile auf zwei Sprecher verteilt: Ada sagt nur „diese Wunde…“, die Eile spricht Leon (#1) |
| 4010 | 1 | LEBT | Leon | Leon: Yeah, we need to hurry. | D401 #0 (Teil) | ABGEWANDELT | Sprecherwechsel Ada → Leon für „wenn wir uns nicht beeilen“ |
| 4010 | 2 | LEBT | Leon | Leon: Ada... Just who are you, anyway? | D401 #1 (Teil 1/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 3 | LEBT | Leon | Leon: Don't tell me... | D401 #1 (Teil 2/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 4 | LEBT | Ada | Ada: That's right. / I'm with Umbrella. | D401 #2 | WOERTLICH | Ada bekennt sich zu Umbrella wie auf der Disc |
| 4010 | 5 | LEBT | Ada | Ada: Leon... There's no time to talk / right now. | D401 #3 (Teil 1/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 6 | LEBT | Ada | Ada: If we don't hurry, he's going / to die. | D401 #3 (Teil 2/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 7 | LEBT | Leon | Leon: What should we do? | D401 #4 | WOERTLICH | „ich“ → „wir“ |
| 4010 | 8 | LEBT | Ada | Ada: It's a virus that makes people / turn into those things. If we can / get him the vaccine, it won't break out. | D401 #5 | WOERTLICH |  |
| 4010 | 9 | LEBT | Leon | Leon: And where's this vaccine? | D401 #6 | WOERTLICH |  |
| 4010 | 10 | LEBT | Ada | Ada: The vaccine itself isn't here, / but we can make one from a strain of / the virus. | D401 #7 (Teil 1/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 11 | LEBT | Ada | Ada: However... | D401 #7 (Teil 2/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 12 | LEBT | Leon | Leon: Ada? What's wrong? | D401 #8 | WOERTLICH |  |
| 4010 | 13 | LEBT | Ada | Ada: Leon, I can't help you. | D401 #9 | WOERTLICH |  |
| 4010 | 14 | LEBT | Leon | Leon: Wait, what!? | D401 #10 | WOERTLICH |  |
| 4010 | 15 | LEBT | Ada | Ada: There's something I have to do. | D401 #11 (Teil 1/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 16 | LEBT | Ada | Ada: You've helped me out a lot, but / I have to do this myself!  | D401 #11 (Teil 2/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher; Disc „ihr“ (Plural) → „you“ |
| 4010 | 17 | LEBT | Leon | Leon: I'll... | D401 #12 (Teil 1/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 18 | LEBT | Leon | Leon: ...do it on my own if I have to! | D401 #12 (Teil 2/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 19 | LEBT | Leon | Leon: So, please tell me! / What should I do!? | D401 #13 | WOERTLICH |  |
| 4010 | 20 | LEBT | Ada | Ada: The viral strain should be stored / in this research center's ultra-low / temperature laboratory. | D401 #14 (Teil 1/4) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 21 | LEBT | Ada | Ada: As long as you can start up the / program for it, it should handle the / rest automatically. | D401 #14 (Teil 2/4) | SINNGEMAESS | „Setz ihn ins Gerät“ fehlt |
| 4010 | 22 | LEBT | Ada | Ada: However, the Virus Inhibitor is / very delicate. | D401 #14 (Teil 3/4) | ABGEWANDELT | Disc: der Virusstamm (ウィルスかぶ) ist empfindlich; Port nennt ihn „Virus Inhibitor“ (anderer Stoff, widerspricht #20) |
| 4010 | 23 | LEBT | Ada | Ada: If you don't handle it with / extreme care, it'll all be for naught. | D401 #14 (Teil 4/4) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 24 | LEBT | Leon | Leon: Well, I have to give it a go. | D401 #15 (Teil 1/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 25 | LEBT | Leon | Leon: Marvin... I'll save you! | D401 #15 (Teil 2/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 26 | LEBT | Ada | Ada: Leon. When all this is over, / head to the underground shelter! | D401 #16 (Teil 1/3) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 27 | LEBT | Ada | Ada: If you don't, you probably won't / survive. | D401 #16 (Teil 2/3) | SINNGEMAESS | „kann für dein Leben nicht garantieren“ → „you probably won't survive“ |
| 4010 | 28 | LEBT | Ada | Ada: And keep in mind that this is an / Umbrella research facility. Be careful. | D401 #16 (Teil 3/3) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 29 | LEBT | Leon | Leon: Ada, wait! | D401 #17 (Teil 1/2) | WOERTLICH | Textbox-Teilung, gleicher Sprecher |
| 4010 | 30 | LEBT | Leon | Leon: I probably won't survive...? | D401 #17 (Teil 2/2) | SINNGEMAESS | Echo von #27 statt „nicht garantieren…?“ |
| 4010 | 31 | LEBT | (ohne Präfix, Spielfigur Leon) | I need to hurry up and get that / vaccine! | D401 #18 | WOERTLICH | Raumtext (Disc: 401 und 501, beide Figuren); Port hat ihn in 401 nur bei Leon |
| 4010 | 32 | LEBT | Leon | Leon: What's wrong, Marvin? Open your / eyes!! | D401 #19 | WOERTLICH | Disc tot (`ROOM4010 sub06` ohne Aufrufer), Port LEBT (sub04): Marvin bricht zusammen |
| 4010 | 33 | LEBT | Leon | Leon: Ada! | D401 #20 | WOERTLICH | Disc tot, Port LEBT |
| 4010 | 34 | LEBT | Ada | Ada: Leon! What are you doing!? The / virus is on the loose here as well! | D401 #21 | WOERTLICH | Disc tot, Port LEBT: Ada kehrt zurück |
| 4010 | 35 | LEBT | Ada | Ada: Hurry to the shelter! | D401 #22 | WOERTLICH | Disc tot, Port LEBT |
| 4010 | 36 | LEBT | System/Untersuchen | It's a row of monitors.They're completely blank, maybe / they're not getting any power? | D401 #23 | WOERTLICH |  |
| 4010 | 37 | LEBT | System/Untersuchen |  / The desk is completely empty. | D401 #24 | WOERTLICH |  |
| 4010 | 38 | LEBT | System/Untersuchen | This equipment seems to be unusable. | D401 #25 | ABGEWANDELT | Disc „wurde wohl nicht benutzt“ → Port „unbenutzbar“ (Fehlübersetzung, leicht) |
| 4010 | 39 | LEBT | System/Untersuchen | There's a memo here.To Michael: I left an ID card on the / right desk.Don't lose it again, okay? - Henry | D401 #26 | ABGEWANDELT | Disc: ID-Karte auf dem LINKEN Tisch (左の机); Port: „right desk“ |
| 4010 | 40 | LEBT | System/Untersuchen |  / There's nothing useful in this locker. | D401 #27 | WOERTLICH |  |
| 4010 | 41 | LEBT | System/Untersuchen | This freezer isn't on. It smells pretty / bad. | D401 #28 | SINNGEMAESS | „auch“ und „stechend“ fehlen |
| 4010 | 42 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Itembox-Hinweis, gleicher Text in 1150/2010/30A0; nicht auf der Disc |
| 4010 | 43 | LEBT | System/Untersuchen | It's a computer.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Speicher-PC (README: „Added interaction with PCs in room 401 and room 501“) |
| 4011 | 0 | LEBT | System/Untersuchen | It's a row of monitors.They're completely blank, maybe / they're not getting any power? | D401 #23 | WOERTLICH |  |
| 4011 | 1 | LEBT | System/Untersuchen |  / The desk is completely empty. | D401 #24 | WOERTLICH |  |
| 4011 | 2 | LEBT | System/Untersuchen |  / This equipment seems to be unusable. | D401 #25 | ABGEWANDELT | wie 4010 #38 |
| 4011 | 3 | LEBT | System/Untersuchen | There's a memo here.To Michael: I left an ID card on the / right desk.Don't lose it again, okay? - Henry | D401 #26 | ABGEWANDELT | wie 4010 #39: links → rechts |
| 4011 | 4 | LEBT | System/Untersuchen |  / There's nothing useful in this locker. | D401 #27 | WOERTLICH |  |
| 4011 | 5 | LEBT | System/Untersuchen | This freezer isn't on. It smells pretty / bad. | D401 #28 | SINNGEMAESS | wie 4010 #41 |
| 4011 | 6 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | wie 4010 #42 |
| 4011 | 7 | LEBT | System/Untersuchen | It's a computer.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | wie 4010 #43 |
| 4030 | 0 | LEBT | System/Untersuchen | Main Fuse" / It's a large circuit breaker system.It looks like someone has removed all / the fuses. | D403 #0 | SINNGEMAESS | Disc: jemand hat die Anlage ausgelöst, Sicherungen herausgesprungen; Port: jemand hat sie entfernt |
| 4030 | 1 | LEBT | System/Untersuchen | Will you use the Spark Plug?  | D403 #1 | ABGEWANDELT | Itemname: Disc リペアープラグ (Reparaturstecker), Port „Spark Plug“ (README: Modell in 404 ergänzt) |
| 4030 | 2 | LEBT | System/Untersuchen | The fuses are all connected. I should / be able to get to some new places now. | D403 #2 | WOERTLICH |  |
| 4030 | 3 | LEBT | System/Untersuchen | Thanks to the footlights, I can just / barely see the ground. | D403 #3 | WOERTLICH |  |
| 4030 | 4 | LEBT | System/Untersuchen | The lamps seem to flicker now and / then, but... | D403 #4 | WOERTLICH | Disc-Lesart unsicher |
| 4031 | 0 | LEBT | System/Untersuchen | Main Fuse" / It's a large circuit breaker system.It looks like someone has removed all / the fuses. | D403 #0 | SINNGEMAESS | Disc: jemand hat die Anlage ausgelöst, Sicherungen herausgesprungen; Port: jemand hat sie entfernt |
| 4031 | 1 | LEBT | System/Untersuchen | Will you use the Spark Plug?  | D403 #1 | ABGEWANDELT | Itemname: Disc リペアープラグ (Reparaturstecker), Port „Spark Plug“ (README: Modell in 404 ergänzt) |
| 4031 | 2 | LEBT | System/Untersuchen | The fuses are all connected. I should / be able to get to some new places now. | D403 #2 | WOERTLICH |  |
| 4031 | 3 | LEBT | System/Untersuchen | Thanks to the footlights, I can just / barely see the ground. | D403 #3 | WOERTLICH |  |
| 4031 | 4 | LEBT | System/Untersuchen | The lamps seem to flicker now and / then, but... | D403 #4 | WOERTLICH | Disc-Lesart unsicher |
| 4031 | 5 | LEBT | John | John: Sherry! | D403 #5 | ABGEWANDELT | Sprecherwechsel Elza → John |
| 4031 | 6 | LEBT | John | John: What the? / Where did she go!? | D403 #6 | WOERTLICH |  |
| 4031 | 7 | LEBT | Elza | Elza: Let's split up and look for / her! | D403 #7 | WOERTLICH |  |
| 4031 | 8 | LEBT | John | John: Okay! I'll go this way, you / go that way, Elza! | D403 #8 | WOERTLICH |  |
| 40A0 | 0 | LEBT | System/Untersuchen | There's a switch here... Press it? /   | D40A #0 | WOERTLICH |  |
| 40A0 | 1 | LEBT | System/Untersuchen | There's a Flammable" mark on it. / Looks dangerous. | D40A #1 | WOERTLICH |  |
| 40A0 | 2 | LEBT | System/Untersuchen | The infirmary bed shows no signs of / recent use. | D40A #2 | WOERTLICH | „kaum benutzt“ → „no signs of recent use“ |
| 40A0 | 3 | LEBT | System/Untersuchen | It's securely locked. An ID card is / required to open it. | D40A #3 | WOERTLICH |  |
| 40A0 | 4 | kein | System/Untersuchen | Use the Research Center ID Card?  | D40A #4 | WOERTLICH | kein Aufruf; auf der Disc ebenfalls nicht ausgelöst |
| 40A0 | 5 | LEBT | System/Untersuchen |  / It's a CT Scanner.The researchers here must be rather / unhealthy, because it looks like it's / seen a lot of use. | D40A #5 | ABGEWANDELT | Disc: zur Gesundheitskontrolle der Forscher; Port: „die Forscher müssen recht krank sein“ (Fehlübersetzung) |
| 40A0 | 6 | LEBT | System/Untersuchen |  / It's too dark to see what's on the shelf. | D40A #6 | WOERTLICH |  |
| 40A0 | 7 | LEBT | System/Untersuchen |  / It's too dangerous to dig around with / my bare hands. | D40A #7 | WOERTLICH |  |
| 40A0 | 8 | LEBT | System/Untersuchen |  / It's too dark to tell what it is. | D40A #8 | WOERTLICH |  |
| 40A0 | 9 | kein | System/Untersuchen |  / It's too dark to see anything. | D40A #9 | WOERTLICH | Port: kein Aufruf; Disc: in Elzas Datei ausgelöst |
| 40A0 | 10 | LEBT | System/Untersuchen | An operating table? It's too dark to / tell for sure. | D40A #10 | WOERTLICH |  |
| 40A0 | 11 | LEBT | System/Untersuchen |  / There's what seems to be blood on it... | D40A #11 | WOERTLICH |  |
| 40A0 | 12 | LEBT | System/Untersuchen | There is one U.S.S. armor / that should fit you.Will you equip it?  | - | MOD-TEXT | U.S.S.-Rüstung (README: „equip U.S.S. armors in CT-SCAN“) |
| 40A0 | 13 | LEBT | System/Untersuchen | Will you unequip the armor?   | - | MOD-TEXT | U.S.S.-Rüstung, Mod |
| 40A1 | 0 | LEBT | System/Untersuchen | There's a switch here... Press it? /   | D40A #0 | WOERTLICH |  |
| 40A1 | 1 | LEBT | System/Untersuchen | There's a Flammable" mark on it. / Looks dangerous. | D40A #1 | WOERTLICH |  |
| 40A1 | 2 | LEBT | System/Untersuchen | The infirmary bed shows no signs of / recent use. | D40A #2 | WOERTLICH | „kaum benutzt“ → „no signs of recent use“ |
| 40A1 | 3 | LEBT | System/Untersuchen | It's securely locked. An ID card is / required to open it. | D40A #3 | WOERTLICH |  |
| 40A1 | 4 | kein | System/Untersuchen | Use the Research Center ID Card?  | D40A #4 | WOERTLICH | kein Aufruf; auf der Disc ebenfalls nicht ausgelöst |
| 40A1 | 5 | LEBT | System/Untersuchen |  / It's a CT Scanner.The researchers here must be rather / unhealthy, because it looks like it's / seen a lot of use. | D40A #5 | ABGEWANDELT | Disc: zur Gesundheitskontrolle der Forscher; Port: „die Forscher müssen recht krank sein“ (Fehlübersetzung) |
| 40A1 | 6 | LEBT | System/Untersuchen |  / It's too dark to see what's on the shelf. | D40A #6 | WOERTLICH |  |
| 40A1 | 7 | LEBT | System/Untersuchen |  / It's too dangerous to dig around with / my bare hands. | D40A #7 | WOERTLICH |  |
| 40A1 | 8 | LEBT | System/Untersuchen |  / It's too dark to tell what it is. | D40A #8 | WOERTLICH |  |
| 40A1 | 9 | kein | System/Untersuchen |  / It's too dark to see anything. | D40A #9 | WOERTLICH | Port: kein Aufruf; Disc: in Elzas Datei ausgelöst |
| 40A1 | 10 | LEBT | System/Untersuchen | An operating table? It's too dark to / tell for sure. | D40A #10 | WOERTLICH |  |
| 40A1 | 11 | LEBT | System/Untersuchen |  / There's what seems to be blood on it... | D40A #11 | WOERTLICH |  |
| 40A1 | 12 | LEBT | System/Untersuchen | There is one U.S.S. armor / that should fit you.Will you equip it?  | - | MOD-TEXT | U.S.S.-Rüstung (README: „equip U.S.S. armors in CT-SCAN“) |
| 40A1 | 13 | LEBT | System/Untersuchen | Will you unequip the armor?   | - | MOD-TEXT | U.S.S.-Rüstung, Mod |
| 5010 | 0 | LEBT | System/Untersuchen | There's a bunch of footprints going / in circles.There doesn't seem to be anything / useful here. | D501 #38 | ABGEWANDELT | Disc: hastig durchwühlt (ひっかき回した跡); Port: „Fußspuren im Kreis“ (Fehlübersetzung) |
| 5010 | 1 | LEBT | System/Untersuchen |  / This desk is very dusty.There's nothing useful on it. | D501 #39 | WOERTLICH |  |
| 5010 | 2 | kein | System/Untersuchen | It looks like this was used a little / while ago. | D501 #40 | WOERTLICH | Port: kein Aufruf; Disc: ausgelöst |
| 5010 | 3 | LEBT | System/Untersuchen | There's a memo board behind a bunch / of machinery.It seems to be mostly notes written / down by the research staff. | D501 #41 | WOERTLICH |  |
| 5010 | 4 | LEBT | System/Untersuchen | The trashcan is full of crumpled / up papers. | D501 #42 | WOERTLICH |  |
| 5010 | 5 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | wie 4010 #42 |
| 5010 | 6 | LEBT | System/Untersuchen | It's a computer.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Speicher-PC (README: PCs in 401 und 501) |
| 5011 | 0 | LEBT | Elza | Elza: Sherry! Wake up! | D501 #0 | ABGEWANDELT | Sprecherwechsel John → Elza; „halt durch“ → „wach auf“. Disc tot (`ROOM5011 sub02` ohne Aufrufer), Port LEBT |
| 5011 | 1 | LEBT | John | John: It's no good... / She's passed out. | D501 #1 | WOERTLICH | Disc tot (`ROOM5011 sub02` ohne Aufrufer), Port LEBT |
| 5011 | 2 | LEBT | Elza | Elza: Ahh... Sherry... | D501 #2 | WOERTLICH | Disc tot (`ROOM5011 sub02` ohne Aufrufer), Port LEBT |
| 5011 | 3 | LEBT | Elza | Elza: John. I read some files in this / research center, and I found out / some terrifying things. | D501 #3 | WOERTLICH | Disc tot (`ROOM5011 sub02` ohne Aufrufer), Port LEBT |
| 5011 | 4 | LEBT | Elza | Elza: She's been infected by what / they were working on here. | D501 #4 | SINNGEMAESS | „vom Virus zerfressen“ → „infiziert mit dem, woran hier gearbeitet wurde“. Disc tot (`ROOM5011 sub02` ohne Aufrufer), Port LEBT |
| 5011 | 5 | LEBT | John | John: Elza, have a look at this. | D501 #5 | WOERTLICH | Disc tot (`ROOM5011 sub02` ohne Aufrufer), Port LEBT |
| 5011 | 6 | LEBT | Elza | Elza: What is this...? | D501 #6 | ABGEWANDELT | Ausruf „Wie furchtbar…“ → Frage „What is this…?“ (leicht). Disc nicht ausgelöst, Port LEBT |
| 5011 | 7 | LEBT | John | John: If we can't get the vaccine, / Sherry will... | D501 #7 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 5011 | 8 | LEBT | Elza | Elza: Okay. I'll go get it. | D501 #8 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 5011 | 9 | LEBT | John | John: What? | D501 #9 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 5011 | 10 | LEBT | Elza | Elza: While we're talking, the virus / is spreading inside Sherry's body. | D501 #10 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 5011 | 11 | LEBT | Elza | Elza: Even if it's just a chance, / I have to take it. | D501 #11 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 5011 | 12 | LEBT | John | John: Elza... | D501 #12 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 5011 | 13 | LEBT | Elza | Elza: John, keep a close eye on her. | D501 #13 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 5011 | 14 | LEBT | Elza | Elza: Sherry needs you right now. | D501 #14 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 5011 | 15 | LEBT | John | John: Elza, please... | D501 #15 | WOERTLICH | 頼むぜ = „ich verlasse mich auf dich“/„please“. Disc nicht ausgelöst, Port LEBT |
| 5011 | 16 | LEBT | John | John: ...Please save this girl, even / if it's just her. | D501 #16 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 5011 | 17 | LEBT | Elza | Elza: John, I've got the vaccine! | D501 #17 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 18 | LEBT | Elza | Elza: How's Sherry? | D501 #18 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 19 | LEBT | John | John: Quick! She doesn't have long! | D501 #19 | ABGEWANDELT | Disc: „sie stöhnt schon lange im Schlaf“; Port: „sie hat nicht mehr lange“ (Fehlübersetzung, dramatisiert). Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 20 | LEBT | Elza | Elza: Sherry! Wake up! | D501 #20 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 21 | LEBT | John | John: Sherry, what's wrong!? / Talk to me! | D501 #21 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 22 | LEBT | Elza | Elza: Sherry, please... Wake up...! | D501 #22 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 23 | LEBT | John | John: What's wrong...? | D501 #23 | ABGEWANDELT | Sprecherwechsel Sherry → John: Sherrys erstes Wort beim Erwachen fehlt. Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 24 | LEBT | Elza | Elza: Sherry! You're awake! | D501 #24 | ABGEWANDELT | Sprecherwechsel John → Elza. Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 25 | LEBT | Elza | Elza: What a relief... | D501 #25 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 26 | LEBT | John | John: Yes, I was really worried! | D501 #26 | ABGEWANDELT | Sprecherwechsel Elza → John. Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 27 | LEBT | Elza | Elza: John, did you hear that / announcement? | D501 #27 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 28 | LEBT | John | John: Yeah. What's going on!? | D501 #28 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 29 | LEBT | Elza | Elza: It seems like that beast broke / the place's power core. | D501 #29 | WOERTLICH | „It seems“ ergänzt; Monster zerstört die Energie wie auf der Disc. Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 30 | LEBT | Elza | Elza: And fire's starting to spread / all over the place. | D501 #30 | ABGEWANDELT | Disc: Explosion setzt das Virus frei; Port: Feuer breitet sich aus. Die Virusgefahr fällt weg. Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 31 | LEBT | John | John: What!? That means we're also / in danger! | D501 #31 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 32 | LEBT | Elza | Elza: That's right! We can't stay / here, we need to hurry to the shelter! | D501 #32 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 33 | LEBT | John | John: Sherry, do you know where the / shelter is? | D501 #33 | ABGEWANDELT | Sprecherwechsel Elza → John. Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 34 | LEBT | Sherry | Sherry: Y-yes. | D501 #34 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 35 | LEBT | Sherry | Sherry: The second shelter should be / open. | D501 #35 | ABGEWANDELT | Disc: Sherry KANN Schutzraum Nr. 2 öffnen; Port: „sollte offen sein“. Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 36 | LEBT | John | John: Alright, let's get going! | D501 #36 | WOERTLICH | Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 37 | LEBT | Elza | Elza: Lead the way, Sherry! | D501 #37 | ABGEWANDELT | Sprecherwechsel John → Elza; „ich verlasse mich auf dich“ → „geh voran“. Disc tot (`ROOM5011 sub06` ohne Aufrufer), Port LEBT |
| 5011 | 38 | LEBT | System/Untersuchen | There's a bunch of footprints going / in circles.There doesn't seem to be anything / useful here. | D501 #38 | ABGEWANDELT | wie 5010 #0 |
| 5011 | 39 | LEBT | System/Untersuchen |  / This desk is very dusty.There's nothing useful on it. | D501 #39 | WOERTLICH |  |
| 5011 | 40 | kein | System/Untersuchen | It looks like this was used a little / while ago. | D501 #40 | WOERTLICH | Port: kein Aufruf; Disc: ausgelöst |
| 5011 | 41 | LEBT | System/Untersuchen | There's a memo board behind a bunch / of machinery.It seems to be mostly notes written / down by the research staff. | D501 #41 | WOERTLICH |  |
| 5011 | 42 | LEBT | System/Untersuchen | The trashcan is full of crumpled / up papers. | D501 #42 | WOERTLICH |  |
| 5011 | 43 | LEBT | (ohne Präfix, Spielfigur Elza) |  / I need to hurry up and get that vaccine! | D501 #43 | WOERTLICH | Raumtext (Disc: 401 und 501, beide Figuren); Port hat ihn in 501 nur bei Elza |
| 5011 | 44 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | wie 4010 #42 |
| 5011 | 45 | LEBT | System/Untersuchen | It's a computer.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | wie 5010 #6 |
| 5040 | 0 | LEBT | System/Schild | Left: P-4 Level Laboratory / Right: Computer Room | D504 #0 | WOERTLICH |  |
| 5041 | 0 | LEBT | System/Schild | Left: P-4 Level Laboratory / Right: Computer Room | D504 #0 | WOERTLICH |  |
| 5060 | 0 | LEBT | System/Untersuchen | A locker used to hold containment suits.It's broken, and can't be opened. | D506 #0 | WOERTLICH | 作業用 („Arbeits-“) → „containment suits“ |
| 5060 | 1 | LEBT | System/Untersuchen | There's nothing inside.It looks like it was emptied in a hurry. | D506 #1 | SINNGEMAESS | „hastig offen gelassen“ → „hastig ausgeräumt“ |
| 5060 | 2 | LEBT | System/Untersuchen | Current Biohazard Level:It seems to be at a safe level right now. | D506 #2 | WOERTLICH |  |
| 5060 | 3 | LEBT | System/Untersuchen | Security CabinetIt's locked and airtight. | D506 #3 | WOERTLICH |  |
| 5060 | 4 | LEBT | System/Untersuchen | The security bolt has been removed.I hope nothing dangerous was inside. | D506 #4 | ABGEWANDELT | Disc: Schrauben lösen sich, Inhalt hoffentlich ungefährlich; Port: Bolzen entfernt, „was inside“ = Inhalt fort (leicht) |
| 5060 | 5 | LEBT | System/Untersuchen | Something must've happened inside / this building. These footprints look / like those of an injured person. | D506 #5 | ABGEWANDELT | Disc: Kratzspuren von einem Arbeitsunfall; Port: Fußspuren eines Verletzten |
| 5060 | 6 | LEBT | System/Untersuchen | The sink is very clean. | D506 #6 | WOERTLICH |  |
| 5060 | 7 | LEBT | System/Untersuchen | It's a draft chamber, for use if a gas / outbreak occurs.It's very clean. I can't tell if it's ever / been used. | D506 #7 | WOERTLICH |  |
| 5060 | 8 | LEBT | System/Untersuchen | It looks like a virus injector. I don't / really need to use it. | D506 #8 | SINNGEMAESS | „Gerät für Virusarbeiten“ → „virus injector“ |
| 5060 | 9 | LEBT | System (Entwicklervermerk) | I can not investigate" this ROM yet. | D506 #9 | WOERTLICH | Entwicklervermerk der Disc; im Port als Ich-Satz, LEBT solange Flag (3,79)=0 |
| 5060 | 10 | LEBT | System/Untersuchen | A vaccine synthesis machine.A sample of the virus must / go inside.Will you push the switch?  | - | MOD-TEXT | Impfstoff-Synthesegerät. Vorgang folgt D401 #14 (Stamm ins Gerät, Programm starten), Wortlaut Mod (README: vaccine machine in 506) |
| 5060 | 11 | LEBT | System/Untersuchen | Processing... | - | MOD-TEXT | Mod |
| 5060 | 12 | LEBT | System/Untersuchen | Processed! | - | MOD-TEXT | Mod |
| 5060 | 13 | LEBT | System/Untersuchen | You've taken the T-Vaccine. | - | MOD-TEXT | Leon erhält „T-Vaccine“. T- und G-Impfstoff sind auf der Disc getrennte Items (BERICHT §4), die Zeile ist Mod |
| 5060 | 14 | LEBT | System/Untersuchen | There is nothing left for me to do / here. | - | MOD-TEXT | Mod |
| 5061 | 0 | LEBT | System/Untersuchen | A locker used to hold containment suits.It's broken, and can't be opened. | D506 #0 | WOERTLICH | 作業用 („Arbeits-“) → „containment suits“ |
| 5061 | 1 | LEBT | System/Untersuchen | There's nothing inside.It looks like it was emptied in a hurry. | D506 #1 | SINNGEMAESS | „hastig offen gelassen“ → „hastig ausgeräumt“ |
| 5061 | 2 | LEBT | System/Untersuchen | Current Biohazard Level:It seems to be at a safe level right now. | D506 #2 | WOERTLICH |  |
| 5061 | 3 | LEBT | System/Untersuchen | Security CabinetIt's locked and airtight. | D506 #3 | WOERTLICH |  |
| 5061 | 4 | LEBT | System/Untersuchen | The security bolt has been removed.I hope nothing dangerous was inside. | D506 #4 | ABGEWANDELT | Disc: Schrauben lösen sich, Inhalt hoffentlich ungefährlich; Port: Bolzen entfernt, „was inside“ = Inhalt fort (leicht) |
| 5061 | 5 | LEBT | System/Untersuchen | Something must've happened inside / this building. These footprints look / like those of an injured person. | D506 #5 | ABGEWANDELT | Disc: Kratzspuren von einem Arbeitsunfall; Port: Fußspuren eines Verletzten |
| 5061 | 6 | LEBT | System/Untersuchen | The sink is very clean. | D506 #6 | WOERTLICH |  |
| 5061 | 7 | LEBT | System/Untersuchen | It's a draft chamber, for use if a gas / outbreak occurs.It's very clean. I can't tell if it's ever / been used. | D506 #7 | WOERTLICH |  |
| 5061 | 8 | LEBT | System/Untersuchen | It looks like a virus injector. I don't / really need to use it. | D506 #8 | SINNGEMAESS | „Gerät für Virusarbeiten“ → „virus injector“ |
| 5061 | 9 | LEBT | System (Entwicklervermerk) | I can not investigate" this ROM yet. | D506 #9 | WOERTLICH | Entwicklervermerk der Disc; im Port als Ich-Satz, LEBT solange Flag (3,79)=0 |
| 5061 | 10 | LEBT | System/Untersuchen | A vaccine synthesis machine.A sample of the virus must / go inside.Will you push the switch?  | - | MOD-TEXT | Impfstoff-Synthesegerät. Vorgang folgt D401 #14 (Stamm ins Gerät, Programm starten), Wortlaut Mod (README: vaccine machine in 506) |
| 5061 | 11 | LEBT | System/Untersuchen | Processing... | - | MOD-TEXT | Mod |
| 5061 | 12 | LEBT | System/Untersuchen | Processed! | - | MOD-TEXT | Mod |
| 5061 | 13 | LEBT | System/Untersuchen | You've taken the G-Vaccine. | - | MOD-TEXT | Elza erhält „G-Vaccine“; sonst wie 5060 #13 |
| 5061 | 14 | LEBT | System/Untersuchen | There is nothing left for me to do / here. | - | MOD-TEXT | Mod |
| 50A0 | 0 | LEBT | System/Untersuchen | It's the shutter control panel.I can't use it without an ID Card. | D50A #0 | SINNGEMAESS | „besondere ID-Karte“ und „steht geschrieben“ fehlen. Disc nicht ausgelöst, Port LEBT |
| 50A0 | 1 | LEBT | System/Untersuchen | It looks the shutter on the right / leads to a hangar. | D50A #1 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 50A0 | 2 | LEBT | System/Untersuchen | There's a maintenance panel here, but / it doesn't look like it has seen any use. | D50A #2 | SINNGEMAESS | „wenig benutzt“ → „not any use“. Disc nicht ausgelöst, Port LEBT |
| 50A0 | 3 | LEBT | System/Untersuchen | Emergency Staircase"They probably wrote this so that / the researchers wouldn't get lost. | D50A #3 | ABGEWANDELT | Mod-Zusatz: Schildtext „Emergency Staircase“ (verweist auf Mod-Raum 407). Disc nicht ausgelöst, Port LEBT |
| 50A1 | 0 | LEBT | System/Untersuchen | It's the shutter control panel.I can't use it without an ID Card. | D50A #0 | SINNGEMAESS | „besondere ID-Karte“ und „steht geschrieben“ fehlen. Disc nicht ausgelöst, Port LEBT |
| 50A1 | 1 | LEBT | System/Untersuchen | It looks the shutter on the right / leads to a hangar. | D50A #1 | WOERTLICH | Disc nicht ausgelöst, Port LEBT |
| 50A1 | 2 | LEBT | System/Untersuchen | There's a maintenance panel here, but / it doesn't look like it has seen any use. | D50A #2 | SINNGEMAESS | „wenig benutzt“ → „not any use“. Disc nicht ausgelöst, Port LEBT |
| 50A1 | 3 | LEBT | System/Untersuchen | Emergency Staircase"They probably wrote this so that / the researchers wouldn't get lost. | D50A #3 | ABGEWANDELT | Mod-Zusatz: Schildtext „Emergency Staircase“ (verweist auf Mod-Raum 407). Disc nicht ausgelöst, Port LEBT |
| 5120 | 0 | LEBT | System/Schild | Left: P-4 Level Laboratory / Right: Computer Room | D512 #0 | WOERTLICH |  |
| 5120 | 1 | LEBT | System/Untersuchen | The door is locked because of / the fire! | D512 #1 | WOERTLICH | Disc nicht ausgelöst, Port LEBT (README: Türen in 512 wegen Feuer gesperrt) |
| 5121 | 0 | LEBT | System/Schild | Left: P-4 Level Laboratory / Right: Computer Room | D512 #0 | WOERTLICH |  |
| 5121 | 1 | LEBT | System/Untersuchen | The door is locked because of / the fire! | D512 #1 | WOERTLICH | Disc nicht ausgelöst, Port LEBT (README: Türen in 512 wegen Feuer gesperrt) |
| 5140 | 0 | kein | System/Untersuchen |  / It's open on this end. | D514 #0 | ABGEWANDELT | Disc: Frauenstimme „Es ist offen, da hinten“; Port: Systemtext „offen auf dieser Seite“, Sprecherin entfällt; beidseitig kein Aufruf |
| 5141 | 0 | kein | System/Untersuchen |  / It's open on this end. | D514 #0 | ABGEWANDELT | Disc: Frauenstimme „Es ist offen, da hinten“; Port: Systemtext „offen auf dieser Seite“, Sprecherin entfällt; beidseitig kein Aufruf |
| 4020 | 0 | LEBT | System/Untersuchen |  / There's a map of the entire area. | - | MOD-TEXT | Raum 402 nur im Mod (README: Lab Elevator, by Juvenal) |
| 4020 | 1 | LEBT | System/Untersuchen |  / The button doesn't respond... | - | MOD-TEXT | Raum 402 nur im Mod; einzige Disc-Knopfzeile D104 #0 ist eine Frage, keine Aussage |
| 4021 | 0 | LEBT | System/Untersuchen |  / There's a map of the entire area. | - | MOD-TEXT | Raum 402 nur im Mod (README: Lab Elevator, by Juvenal) |
| 4021 | 1 | LEBT | System/Untersuchen |  / The button doesn't respond... | - | MOD-TEXT | Raum 402 nur im Mod; einzige Disc-Knopfzeile D104 #0 ist eine Frage, keine Aussage |
| 4040 | 0 | LEBT | System/Schild | Left: Emergency Staircase / Right: Private Rooms | - | MOD-TEXT | Schild; 404 = EXE MIGITSUURO, keine Disc-Datei. „Emergency Staircase“/„Private Rooms“ = Mod-Räume 407/405 |
| 4041 | 0 | LEBT | System/Schild | Left: Emergency Staircase / Right: Private Rooms | - | MOD-TEXT | Schild; 404 = EXE MIGITSUURO, keine Disc-Datei. „Emergency Staircase“/„Private Rooms“ = Mod-Räume 407/405 |
| 4050 | 0 | LEBT | System/Untersuchen | There's nothing useful in this / locker. | D401 #27 (raumübergr.) | WOERTLICH | Wortlaut = 4010 #40; im Mod-Raum 405 an fünf Spinden wiederverwendet, Ort nicht belegt |
| 4050 | 1 | LEBT | System/Untersuchen | A researcher's bed. | - | MOD-TEXT | Raum 405 nur im Mod (README: Private Rooms) |
| 4050 | 2 | LEBT | System/Untersuchen | A cabinet full of books and / reference materials.Most appear to be about / chemistry and biology. | - | MOD-TEXT | Raum 405 nur im Mod |
| 4050 | 3 | kein | System/Untersuchen | There's a memo here.To David: I found your ID card / in the hallway.Please don't lose it again. / - Eric | D401 #26 (raumübergr.) | ABGEWANDELT | Variante der Henry-Notiz aus 401: andere Namen (David/Eric), Karte im Flur gefunden; kein Aufruf |
| 4051 | 0 | LEBT | System/Untersuchen | There's nothing useful in this / locker. | D401 #27 (raumübergr.) | WOERTLICH | Wortlaut = 4010 #40; im Mod-Raum 405 an fünf Spinden wiederverwendet, Ort nicht belegt |
| 4051 | 1 | LEBT | System/Untersuchen | A researcher's bed. | - | MOD-TEXT | Raum 405 nur im Mod (README: Private Rooms) |
| 4051 | 2 | LEBT | System/Untersuchen | A cabinet full of books and / reference materials.Most appear to be about / chemistry and biology. | - | MOD-TEXT | Raum 405 nur im Mod |
| 4051 | 3 | kein | System/Untersuchen | There's a memo here.To David: I found your ID card / in the hallway.Please don't lose it again. / - Eric | D401 #26 (raumübergr.) | ABGEWANDELT | Variante der Henry-Notiz aus 401: andere Namen (David/Eric), Karte im Flur gefunden; kein Aufruf |
| 4070 | 0 | LEBT | System/Untersuchen | The door won't open! | - | MOD-TEXT | Raum 407 nur im Mod (README: Emergency Stairs) |
| 4071 | 0 | LEBT | System/Untersuchen | The door won't open! | - | MOD-TEXT | Raum 407 nur im Mod (README: Emergency Stairs) |
| 4080 | 0 | LEBT | System/Untersuchen | Cell Cultivation Room"It's electronically locked. | - | MOD-TEXT | Schild „Cell Cultivation Room“ = EXE-Raumname 40B BAIYOUSHITSU (培養室); Text und Sperre Mod |
| 4081 | 0 | LEBT | System/Untersuchen | Cell Cultivation Room"It's electronically locked. | - | MOD-TEXT | Schild „Cell Cultivation Room“ = EXE-Raumname 40B BAIYOUSHITSU (培養室); Text und Sperre Mod |
| 4090 | 0 | LEBT | System/Untersuchen | An ultra low temperature freezer / used to store viral samples.There is a hole to set the Enzyme  / in. | - | MOD-TEXT | Ort passt zu D401 #14 (超低温実験室; 409 = EXE JIKKENSHITSU), Wortlaut und Enzym-Mechanik sind Mod. Disc-4090 hat keinen Text |
| 4090 | 1 | kein | System/Untersuchen | The enzyme is already in place. / I need to operate the machine tosynthesize a sample of the T-Virus. | - | MOD-TEXT | kein Aufruf; nennt „T-Virus“, die Disc sagt nur ウィルス |
| 4090 | 2 | kein | System/Untersuchen | A control box for operating the / robotic arm.Will you use it?  | - | MOD-TEXT | kein Aufruf |
| 4090 | 3 | kein | System/Untersuchen | There's a slot for swiping a keycard. | - | MOD-TEXT | kein Aufruf |
| 4090 | 4 | kein | System/Untersuchen | The keypanel has already been / activated. | - | MOD-TEXT | kein Aufruf |
| 4090 | 5 | LEBT | System/Untersuchen | A numerical keypanel.An ID card is required to operate / it. | - | MOD-TEXT |  |
| 4090 | 6 | kein | System/Untersuchen | A numerical keypanel.If I had the codes, I could use the / machine, but... | - | MOD-TEXT | kein Aufruf |
| 4090 | 7 | LEBT | System/Untersuchen | It looks like a computer for organizing / data files. | - | MOD-TEXT |  |
| 4091 | 0 | LEBT | System/Untersuchen | An ultra low temperature freezer / used to store viral samples.There is a hole to set the Enzyme  / in. | - | MOD-TEXT | Ort passt zu D401 #14 (超低温実験室; 409 = EXE JIKKENSHITSU), Wortlaut und Enzym-Mechanik sind Mod. Disc-4090 hat keinen Text |
| 4091 | 1 | kein | System/Untersuchen | The enzyme is already in place. / I need to operate the machine tosynthesize a sample of the T-Virus. | - | MOD-TEXT | kein Aufruf; nennt „T-Virus“, die Disc sagt nur ウィルス |
| 4091 | 2 | kein | System/Untersuchen | A control box for operating the / robotic arm.Will you use it?  | - | MOD-TEXT | kein Aufruf |
| 4091 | 3 | kein | System/Untersuchen | There's a slot for swiping a keycard. | - | MOD-TEXT | kein Aufruf |
| 4091 | 4 | kein | System/Untersuchen | The keypanel has already been / activated. | - | MOD-TEXT | kein Aufruf |
| 4091 | 5 | LEBT | System/Untersuchen | A numerical keypanel.An ID card is required to operate / it. | - | MOD-TEXT |  |
| 4091 | 6 | kein | System/Untersuchen | A numerical keypanel.If I had the codes, I could use the / machine, but... | - | MOD-TEXT | kein Aufruf |
| 4091 | 7 | LEBT | System/Untersuchen | It looks like a computer for organizing / data files. | - | MOD-TEXT |  |
| 40B0 | 0 | LEBT | System/Untersuchen | It looks like a large vat used to / store research specimens butthere's nothing inside... | - | MOD-TEXT | 40B = EXE BAIYOUSHITSU, Datei nicht auf der Disc |
| 40B0 | 1 | LEBT | System/Untersuchen | It looks like the remains of / a failed experiment... | - | MOD-TEXT | gescheitertes Experiment, Mod |
| 40B0 | 2 | LEBT | System/Untersuchen | It's an operating table.There's what seems to be blood / on it... | D40A #10 + D40A #11 (raumübergr.) | ABGEWANDELT | Zwei Disc-Zeilen aus 40A (OP-Tisch, Blut) zusammengezogen und in den Mod-Raum 40B versetzt, hier ohne „zu dunkel“ |
| 40B0 | 3 | LEBT | System/Untersuchen | It's been broken from the inside. | - | MOD-TEXT | „von innen aufgebrochen“: neue Andeutung, Mod |
| 40B1 | 0 | LEBT | System/Untersuchen | It looks like a large vat used to / store research specimens butthere's nothing inside... | - | MOD-TEXT | 40B = EXE BAIYOUSHITSU, Datei nicht auf der Disc |
| 40B1 | 1 | LEBT | System/Untersuchen | It looks like the remains of / a failed experiment... | - | MOD-TEXT | gescheitertes Experiment, Mod |
| 40B1 | 2 | LEBT | System/Untersuchen | It's an operating table.There's what seems to be blood / on it... | D40A #10 + D40A #11 (raumübergr.) | ABGEWANDELT | Zwei Disc-Zeilen aus 40A (OP-Tisch, Blut) zusammengezogen und in den Mod-Raum 40B versetzt, hier ohne „zu dunkel“ |
| 40B1 | 3 | LEBT | System/Untersuchen | It's been broken from the inside. | - | MOD-TEXT | „von innen aufgebrochen“: neue Andeutung, Mod |
| 5000 | 0 | LEBT | System/Untersuchen | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. | - | MOD-TEXT | Raum 500 nur im Mod (README: A-2 Entrance); Aufzug-Text wie 4000 #9 |
| 5001 | 0 | LEBT | System/Untersuchen | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. | - | MOD-TEXT | Raum 500 nur im Mod (README: A-2 Entrance); Aufzug-Text wie 4000 #9 |
| 5021 | 0 | LEBT | System/Untersuchen | Sherry must have crawled through here, / but the hole is too small for me.There must be another way to / get back there. | - | MOD-TEXT | Untersuchen (README: description texts in 502); Sherry kriecht durch ein Loch, neue Handlung |
| 5021 | 1 | LEBT | Elza | Elza: Sherry... | - | MOD-TEXT | README: „Added a small cutscene in room 502 (Elza)“ |
| 5030 | 0 | LEBT | System/Untersuchen | A map of the lab is available here.Will you file the laboratory map?  | - | MOD-TEXT | Laborkarte; 503 = EXE KANSEISHITSU, Disc-Datei ohne Text |
| 5030 | 1 | LEBT | System/Untersuchen | You've taken the laboratory map. | - | MOD-TEXT | Mod |
| 5030 | 2 | LEBT | System/Untersuchen | It's a row of monitors.They appear to be used for showing / security footage, but they're onlydisplaying static... | - | MOD-TEXT | README: Monitor-Effekt in 503 ergänzt |
| 5031 | 0 | LEBT | System/Untersuchen | A map of the lab is available here.Will you file the laboratory map?  | - | MOD-TEXT | Laborkarte; 503 = EXE KANSEISHITSU, Disc-Datei ohne Text |
| 5031 | 1 | LEBT | System/Untersuchen | You've taken the laboratory map. | - | MOD-TEXT | Mod |
| 5031 | 2 | LEBT | System/Untersuchen | It's a row of monitors.They appear to be used for showing / security footage, but they're onlydisplaying static... | - | MOD-TEXT | README: Monitor-Effekt in 503 ergänzt |
| 5031 | 3 | LEBT | Elza | Elza: Sherry! | - | MOD-TEXT | Ausruf wie D400 #17 / D403 #5, aber eigene Mod-Szene (README: „new custcene for Elza in room 503“) |
| 5050 | 0 | LEBT | System/Untersuchen | Will you operate the / computer?  | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5050 | 1 | LEBT | System/Untersuchen | Culture Experiment / RoomStaff Registry. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5050 | 2 | LEBT | System/Untersuchen | Commencing with temporary / registration. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5050 | 3 | LEBT | System/Untersuchen | Register your fingerprint. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5050 | 4 | kein | System/Untersuchen | Will you register?  | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README); kein Aufruf |
| 5050 | 5 | LEBT | System/Untersuchen | Registration complete. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5050 | 6 | LEBT | System/Untersuchen | Guest registration is valid / for 24 hours. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5050 | 7 | LEBT | System/Untersuchen | I don't need to use this anymore. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5051 | 0 | LEBT | System/Untersuchen | Will you operate the / computer?  | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5051 | 1 | LEBT | System/Untersuchen | Culture Experiment / RoomStaff Registry. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5051 | 2 | LEBT | System/Untersuchen | Commencing with temporary / registration. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5051 | 3 | LEBT | System/Untersuchen | Register your fingerprint. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5051 | 4 | kein | System/Untersuchen | Will you register?  | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README); kein Aufruf |
| 5051 | 5 | LEBT | System/Untersuchen | Registration complete. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5051 | 6 | LEBT | System/Untersuchen | Guest registration is valid / for 24 hours. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5051 | 7 | LEBT | System/Untersuchen | I don't need to use this anymore. | - | MOD-TEXT | 505 = EXE DENSANSHITSU, keine Disc-Datei; Registrierung öffnet 40B (README) |
| 5070 | 0 | LEBT | System/Schild | Left: Shelter 02 / Right: Shelter 01 | - | MOD-TEXT | Schild; „Schutzraum Nr. 2“ steht als Begriff in D501 #35, die Schilder selbst sind Mod; 507 ohne Disc-Datei |
| 5070 | 1 | LEBT | System/Untersuchen | Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... | - | MOD-TEXT | Mod; Schutzraum öffnet „nur im Notfall“ = neue Regel |
| 5070 | 2 | LEBT | System/Untersuchen | Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... | - | MOD-TEXT | Mod |
| 5071 | 0 | LEBT | System/Schild | Left: Shelter 02 / Right: Shelter 01 | - | MOD-TEXT | Schild; „Schutzraum Nr. 2“ steht als Begriff in D501 #35, die Schilder selbst sind Mod; 507 ohne Disc-Datei |
| 5071 | 1 | LEBT | System/Untersuchen | Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... | - | MOD-TEXT | Mod; Schutzraum öffnet „nur im Notfall“ = neue Regel |
| 5071 | 2 | LEBT | System/Untersuchen | Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... | - | MOD-TEXT | Mod |
| 5080 | 0 | LEBT | System/Untersuchen | A high yield generator / that supplies power forthe inside facilities / and the train.Will you turn on the / switch?  | - | MOD-TEXT | Generator versorgt Anlage UND Zug: neue Fakten, Raum 508 nur im Mod |
| 5080 | 1 | LEBT | System/Untersuchen | The emergency mode has / been activated.The power supply will be / cut temporarily.The emergency train will / be activated uponrestoration of the power." | - | MOD-TEXT | „Notfall-Zug“: neue Fakten |
| 5080 | 2 | LEBT | System/Untersuchen | The door won't open until / the power is restored! | - | MOD-TEXT | Mod |
| 5080 | 3 | LEBT | System/Untersuchen | The power already has been / supplied to the train! | - | MOD-TEXT | Mod |
| 5081 | 0 | LEBT | System/Untersuchen | A high yield generator / that supplies power forthe inside facilities / and the train.Will you turn on the / switch?  | - | MOD-TEXT | Generator versorgt Anlage UND Zug: neue Fakten, Raum 508 nur im Mod |
| 5081 | 1 | LEBT | System/Untersuchen | The emergency mode has / been activated.The power supply will be / cut temporarily.The emergency train will / be activated uponrestoration of the power." | - | MOD-TEXT | „Notfall-Zug“: neue Fakten |
| 5081 | 2 | LEBT | System/Untersuchen | The door won't open until / the power is restored! | - | MOD-TEXT | Mod |
| 5081 | 3 | LEBT | System/Untersuchen | The power already has been / supplied to the train! | - | MOD-TEXT | Mod |
| 5090 | 0 | LEBT | System (Abspann) | THANK YOU FOR PLAYING !!! | - | MOD-TEXT | Spielende-Text des Mod (README: „Added ending“); 509 ohne Disc-Datei |
| 5090 | 1 | LEBT | (ohne Präfix, Spielfigur) | I can't put my friends / in danger.I have no choice but to / stop that creature here! | - | MOD-TEXT | Gedanke der Spielfigur: Freunde schützen, die Kreatur hier aufhalten. Foto #145 zeigt Riesen-Birkin und Kittel-Frau in 509, ein Text dazu fehlt |
| 5091 | 0 | LEBT | System (Abspann) | THANK YOU FOR PLAYING !!! | - | MOD-TEXT | Spielende-Text des Mod (README: „Added ending“); 509 ohne Disc-Datei |
| 5091 | 1 | LEBT | (ohne Präfix, Spielfigur) | I can't put my friends / in danger.I have no choice but to / stop that creature here! | - | MOD-TEXT | Gedanke der Spielfigur: Freunde schützen, die Kreatur hier aufhalten. Foto #145 zeigt Riesen-Birkin und Kittel-Frau in 509, ein Text dazu fehlt |
| 50B0 | 0 | LEBT | Ada | Ada: We're all set. Annette is waiting / for us below, and I left Marvin with her. | - | MOD-TEXT | Neu: Annette namentlich, wartet unten; Ada hat Marvin bei ihr gelassen. Disc: Ada trennt sich in 401 (D401 #11), Wiedersehen nur tot (D401 #21/#22) |
| 50B0 | 1 | LEBT | Ada | Mod: "Ada: Your other friends are down there / with her too." ⇒ Port: "Ada: Some other survivors are down there with her too." | - | PORT-TEXT | Ersetzte Mod-Zeile: MOD-TEXT (Raum 50B nicht auf der Disc). Override nimmt „deine Freunde“ zurück |
| 50B0 | 2 | LEBT | Leon | Leon: That's good. Who's Annette? | - | MOD-TEXT | Leon fragt nach Annette; der Name steht in keiner Capcom-Zeile |
| 50B0 | 3 | LEBT | Ada | Ada: Dr. Birkin's wife and my boss. / She's going to help us with the train. | - | MOD-TEXT | Neu: Annette = Birkins Frau und Adas Chefin, hilft mit dem Zug |
| 50B0 | 4 | LEBT | Leon | Leon: Who's Dr. Birkin, and where is he? | - | MOD-TEXT | Name „Dr. Birkin“ fällt; die Disc nennt ihn nie (Sherry sagt nur „Papa“) |
| 50B0 | 5 | LEBT | Ada | Ada: He... | - | MOD-TEXT | Mod |
| 50B0 | 6 | LEBT | Ada | Ada: He's dead. Umbrella killed him. / That's why Annette is helping us. | - | MOD-TEXT | Neu: Birkin ist tot, von Umbrella getötet |
| 50B0 | 7 | LEBT | Leon | Leon: What's with those? | - | MOD-TEXT | Mod |
| 50B0 | 8 | LEBT | Ada | Ada: They're part of Umbrella's / experiments here. | - | MOD-TEXT | Neu: Kreaturen sind Umbrella-Experimente dieses Labors |
| 50B0 | 9 | LEBT | Ada | Ada: They were to be shipped out for / use as biological weapons. | - | MOD-TEXT | Neu: Versand als Biowaffen |
| 50B0 | 10 | LEBT | Leon | Leon: Ada, why did you set the reactor / to blow up? | - | MOD-TEXT | Neu: Ada hat den Reaktor zur Sprengung gebracht. Widerspricht D501 #29 (Monster zerstörte die Energie) |
| 50B0 | 11 | LEBT | Ada | Ada: So I can destroy this place. / You've seen what's in here. | - | MOD-TEXT | Adas Auftrag = Labor zerstören; Disc sagt nur „ich habe etwas zu erledigen“ (D401 #11) |
| 50B0 | 12 | LEBT | Ada | Ada: You've seen what they can do. / I had to do it, understand? It's... | - | MOD-TEXT | Mod |
| 50B0 | 13 | LEBT | Ada | Ada: Aaaaaaaagggghhh! | - | MOD-TEXT | Ada wird angegriffen (Schrei) |
| 50B0 | 14 | LEBT | Leon | Leon: Ada... | - | MOD-TEXT | Mod |
| 50B0 | 15 | LEBT | Ada | Ada: I... I'm sorry, Leon. I'm not... / going... to make it... am I? | - | MOD-TEXT | Ada stirbt. Die Disc zeigt Adas Tod nirgends |
| 50B0 | 16 | LEBT | Ada | Ada: I... I so wanted... to escape... / you... will have... to do that... for me... | - | MOD-TEXT | Adas letzte Worte, Mod |
| 50B0 | 17 | LEBT | (ohne Präfix, Spielfigur Leon) | I can't leave Ada alone! | - | MOD-TEXT | Türsperre, Mod |
| 50B0 | 18 | LEBT | System/Untersuchen | She's dead. | - | MOD-TEXT | Untersuchen: Adas Leiche, Mod |
| 50B1 | 0 | LEBT | System/Untersuchen | Mod: "It's Ada. She's dead.It looks like one of those / gorillas killed her." ⇒ Port: "It's a woman. She's dead.It looks like one of those gorillas killed her." | - | PORT-TEXT | Ersetzte Mod-Zeile „It's Ada…“: MOD-TEXT (Elza findet Adas Leiche; Disc: Elza–Ada nur als Zweig in 603). Port: unbekannte Frau |
| 50C0 | 0 | LEBT | System/Untersuchen | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. | - | MOD-TEXT | 50C = EXE 3-A TSUURO, Disc-Datei ohne Text; Aufzug-Text wie 4000 #9 |
| 50C1 | 0 | LEBT | System/Untersuchen | An elevator. Pressing the / button has no effect.It hasn't been used for a / long time. | - | MOD-TEXT | 50C = EXE 3-A TSUURO, Disc-Datei ohne Text; Aufzug-Text wie 4000 #9 |
| 50D0 | 0 | LEBT | System/Untersuchen | There's a button here. Push it?  e         /            4   2  /           /      3  x  x                     | - | MOD-TEXT | Formel inhaltsgleich mit D104 #0 (ボタンがある 押しますか), aber nicht daraus abgeleitet: Port-1040 trägt D104 als „shutter switch“, die Formel stammt aus den Mod-Räumen 304/305. 50D = EXE „TEST“, Aufzug laut README Mod. Datenmüll nach dem Fragezeichen |
| 50D1 | 0 | LEBT | System/Untersuchen | There's a button here. Push it?  e         /            4   2  /           /      3  x  x                     | - | MOD-TEXT | Formel inhaltsgleich mit D104 #0 (ボタンがある 押しますか), aber nicht daraus abgeleitet: Port-1040 trägt D104 als „shutter switch“, die Formel stammt aus den Mod-Räumen 304/305. 50D = EXE „TEST“, Aufzug laut README Mod. Datenmüll nach dem Fragezeichen |
| 5130 | 0 | kein | System/Schild | Left: Shelter 02 / Right: Shelter 01 | - | MOD-TEXT | kein Aufruf; Kopie der 507-Schilder |
| 5130 | 1 | kein | System/Untersuchen | Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... | - | MOD-TEXT | kein Aufruf |
| 5130 | 2 | kein | System/Untersuchen | Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... | - | MOD-TEXT | kein Aufruf |
| 5131 | 0 | kein | System/Schild | Left: Shelter 02 / Right: Shelter 01 | - | MOD-TEXT | kein Aufruf; Kopie der 507-Schilder |
| 5131 | 1 | kein | System/Untersuchen | Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... | - | MOD-TEXT | kein Aufruf |
| 5131 | 2 | kein | System/Untersuchen | Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency... | - | MOD-TEXT | kein Aufruf |
| 6000 | 0 | LEBT | Annette | Mod: "Annette: You must be that cop. / Your friends are already here." ⇒ Port: "Annette: You must be that cop. Your friend is already here." | - | PORT-TEXT | Ersetzte Mod-Zeile: MOD-TEXT (600 nicht auf der Disc). Annette (Typ 0x4D) am Bahnsteig; Foto #152/#154 zeigt Leon mit der Kittel-Frau am Bahnsteig, ohne Text |
| 6000 | 1 | LEBT | Annette | Annette: Where's Ada? | - | MOD-TEXT | Annette fragt nach Ada (setzt Adas Tod in 50B voraus) |
| 6000 | 2 | LEBT | Annette | Annette: I see... | - | MOD-TEXT | Mod |
| 6000 | 3 | LEBT | Annette | Mod: "Annette: Come on. Let's join your / friends and get out of here." ⇒ Port: "Annette: Come on. Let's join the others and get out of here." | - | PORT-TEXT | Ersetzte Mod-Zeile: MOD-TEXT |
| 6000 | 4 | LEBT | Annette | Annette: Let's see if we can get / this thing moving. | - | MOD-TEXT | Annette will den Zug starten |
| 6000 | 5 | LEBT | System/Untersuchen | A device to open the / tunnel gate.Will you turn on the / switch?  | - | MOD-TEXT | Tunneltor-Schalter, Mod |
| 6000 | 6 | LEBT | System/Untersuchen | It's locked from the other side. | - | MOD-TEXT | README: „Added door locked from the other side in room 600“; Formel auch in 1170/11A0/2030 |
| 6000 | 7 | LEBT | System/Untersuchen | A device to open the / tunnel gate.The power is not supplied. | - | MOD-TEXT | Mod |
| 6001 | 0 | LEBT | System/Untersuchen | A device to open the / tunnel gate.Will you turn on the / switch?  | - | MOD-TEXT | Tunneltor-Schalter, Mod |
| 6001 | 1 | LEBT | System/Untersuchen | It's locked from the other side. | - | MOD-TEXT | README: „Added door locked from the other side in room 600“; Formel auch in 1170/11A0/2030 |
| 6001 | 2 | LEBT | System/Untersuchen | A device to open the / tunnel gate.The power is not supplied. | - | MOD-TEXT | Mod |
| 6010 | 0 | LEBT | System/Untersuchen | You've opened the lock. | - | MOD-TEXT | Formel auch in 10D0/1100/1140 (Mod); ähnlich nur D301 #1 (dort mit ID-Karte) |
| 6011 | 0 | LEBT | System/Untersuchen | You've opened the lock. | - | MOD-TEXT | Formel auch in 10D0/1100/1140 (Mod); ähnlich nur D301 #1 (dort mit ID-Karte) |
| 6020 | 0 | TOT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | TOT; wie 4010 #42 |
| 6021 | 0 | TOT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | TOT; wie 4010 #42 |
| 6030 | 0 | LEBT | Leon | Leon: Hang on, Marvin. / We're getting out of here. | - | MOD-TEXT | Disc: Marvin, John, Sherry stehen in 604 (nicht 603) mit Leon im Raum, ohne Text. Port-6030 setzt sie an die Disc-604-Plätze (John −26055/−25784) |
| 6030 | 1 | LEBT | Leon | Mod: "Leon: Where's Elza?" ⇒ Port: "Leon: Is anyone else with you?" | - | PORT-TEXT | Ersetzte Mod-Zeile „Where's Elza?“: MOD-TEXT (Leon kennt Elza) |
| 6030 | 2 | LEBT | Sherry | Mod: "Sherry: She's out there fighting / the monsters!" ⇒ Port: "Sherry: Elza is out there fighting the monsters!" | - | PORT-TEXT | Ersetzte Mod-Zeile: MOD-TEXT. Override nennt Elza jetzt beim Namen; „Elza kämpft draußen“ bleibt Erfindung |
| 6030 | 3 | LEBT | John | John: Yeah! She's trying to keep / the outside clear so we can escape! | - | MOD-TEXT | Elza hält draußen den Weg frei: Erfindung (BERICHT §6) |
| 6030 | 4 | LEBT | Leon | Leon: Why aren't you out there? | - | MOD-TEXT | Mod |
| 6030 | 5 | LEBT | John | John: She told me to stay and keep / an eye on Marvin here. | - | MOD-TEXT | John bewacht Marvin; auf der Disc nur gemeinsame Aufstellung in 604 |
| 6030 | 6 | LEBT | Leon | Leon: Where's Annette? | - | MOD-TEXT | Leon kennt den Namen Annette (aus 50B0) |
| 6030 | 7 | LEBT | John | John: Up front, trying to start / the train. | - | MOD-TEXT | Annette vorne am Zug, Mod |
| 6030 | 8 | LEBT | Leon | Leon: I see. Thanks. | - | MOD-TEXT | Mod |
| 6030 | 9 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Itembox |
| 6030 | 10 | LEBT | (ohne Präfix, Spielfigur Leon) | I have to get this thing moving / first! | - | MOD-TEXT | Mod |
| 6030 | 11 | LEBT | Annette | Annette: Where's that biker girl? | - | MOD-TEXT | Annette kennt Elza („biker girl“) = Begegnung Annette–Elza vorausgesetzt |
| 6030 | 12 | LEBT | Leon | Mod: "Leon: Keeping the monsters at bay. / Can you start this thing?" ⇒ Port: "Leon: I've never met her. Can you start this thing?" | - | PORT-TEXT | Ersetzte Mod-Zeile „Keeping the monsters at bay“: MOD-TEXT (Leon wusste, was Elza tut) |
| 6030 | 13 | LEBT | Annette | Annette: Certainly. | - | MOD-TEXT | Annette (Typ 0x4D) im Führerstand. Disc-6030: Figur 0x4D steht dort (Kittel-Frau, Foto #146/#147), Name und Text fehlen |
| 6030 | 14 | LEBT | Annette | Annette: If I remember the startup / sequence correctly... | - | MOD-TEXT | Mod |
| 6030 | 15 | LEBT | Leon | Leon: What's wrong? | - | MOD-TEXT | Mod |
| 6030 | 16 | LEBT | Annette | Annette: Not enough power! / The train's batteries are too low! | - | MOD-TEXT | Zug ohne Strom. Disc-603: WARNING-Konsole blinkt (BERICHT §5), ein Text dazu fehlt |
| 6030 | 17 | LEBT | Leon | Leon: I'll take care of it! | - | MOD-TEXT | Mod |
| 6030 | 18 | LEBT | Annette | Annette: I'll be ready to start the / train when you do! | - | MOD-TEXT | Mod |
| 6030 | 19 | LEBT | Leon | Leon: Good luck to both of us! | - | MOD-TEXT | Mod |
| 6030 | 20 | LEBT | System/Untersuchen | You can't get off the / train right now. | - | MOD-TEXT | Mod |
| 6031 | 0 | LEBT | Elza | Mod: "Elza: Where's Leon?" ⇒ Port: "Elza: Sherry! Is anyone else here?" | - | PORT-TEXT | Ersetzte Mod-Zeile „Where's Leon?“: MOD-TEXT (Elza kennt Leon) |
| 6031 | 1 | LEBT | Sherry | Mod: "Sherry: He's fighting off monsters. / He wants you to start the train." ⇒ Port: "Sherry: A policeman! He's fighting off monsters. He wants us to start the train." | - | PORT-TEXT | Ersetzte Mod-Zeile: MOD-TEXT. Auch im Override kämpft ein (unbenannter) Polizist draußen: Erfindung |
| 6031 | 2 | LEBT | Elza | Elza: I'll do it! | - | MOD-TEXT | Mod |
| 6031 | 3 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Itembox |
| 6031 | 4 | LEBT | (ohne Präfix, Spielfigur Elza) | I have to get this thing moving / first! | - | MOD-TEXT | Mod |
| 6031 | 5 | LEBT | Sherry | Sherry: I know how to start this! / I watched Mom and Dad do it. | - | MOD-TEXT | Sherry im Führerstand: Disc-6031 stellt sie dorthin (sub08 läuft), Text fehlt. Satz 1 erinnert an D307 #10 (Aufzug); neu: „Mama und Papa zugesehen“ |
| 6031 | 6 | LEBT | Sherry | Sherry: Let's see, you push this / here and throw this switch there. | - | MOD-TEXT | Mod |
| 6031 | 7 | LEBT | Elza | Elza: What's wrong? | - | MOD-TEXT | Mod |
| 6031 | 8 | LEBT | Sherry | Sherry: It's doesn't have enough power! | - | MOD-TEXT | Mod |
| 6031 | 9 | LEBT | Sherry | Sherry: This says the train's main / batteries need to be fueled! | - | MOD-TEXT | Mod |
| 6031 | 10 | LEBT | Elza | Elza: Let me take care of that. / You start the train once it's ready. | - | MOD-TEXT | Elza kümmert sich um die Batterien, Mod |
| 6031 | 11 | LEBT | System/Untersuchen | You can't get off the / train right now. | - | MOD-TEXT | Mod |

## T2: Disc-Zeilen der Räume 4xx und 5xx

Die Disc hat in STAGE 6 keinen Text (603 und 604 ohne Textblock, 600–602 und 605–607 ohne Datei).

| Disc | Japanisch | Deutsch | Port-Zeile(n) | Bemerkung |
|---|---|---|---|---|
| D400 #0 | ドアはロックされている / パネルにひょうじが出ている ▼ ”エマージェンシー発令によりへいさ / ロックかい除にはIDカードが必要です” | Die Tür ist verriegelt. Auf dem Panel steht: „Wegen Notfall-Ausrufung gesperrt. Zum Entriegeln ist eine ID-Karte nötig.“ | 4000 #0 [WOERTLICH]; 4001 #0 [WOERTLICH] | Disc: Leon / Sprecher System |
| D400 #1 | IDカードを使いますか？ [Ja/Nein] | ID-Karte benutzen? | 4000 #1 [ABGEWANDELT]; 4001 #1 [ABGEWANDELT] | Disc: Leon / Sprecher System. Abweichung: Disc: „ID-Karte benutzen?“; Port: Mod-Item „Blue Master Keycard“ (README: liegt in 401) |
| D400 #2 | ”IDかく認 ロックかい除します” | „ID bestätigt, entriegle.“ | 4000 #2 [ABGEWANDELT]; 4001 #2 [ABGEWANDELT] | Disc: Leon / Sprecher System. Abweichung: Disc: Panel-Anzeige „ID bestätigt, entriegle“; Port: Item-Meldung mit Mod-Item |
| D400 #3 | / エイダ ここは…？ | Ada, was ist das hier…? | 4000 #3 [WOERTLICH] | Disc: Leon / Sprecher Leon |
| D400 #4 | / いいから急げ！ こっちの部屋だ！ | Egal, beeil dich! Hier in den Raum! | 4000 #4 [SINNGEMAESS] | Disc: Leon / Sprecher Ada. Abweichung: „him“ (Marvin) ergänzt, aus der Szene ableitbar |
| D400 #5 | / 先に行け！ 俺がかたづける！ | Geht vor! Ich erledige das! | 4000 #5 [WOERTLICH] | Disc: Leon, tot (`ROOM4000 sub16 @0x18D8` ohne Aufrufer) / Sprecher Leon. Läuft auf der Disc nicht, im Port LEBT |
| D400 #6 | / レオン…！ | Leon…! | 4000 #6 [WOERTLICH] | Disc: Leon, tot (`ROOM4000 sub16 @0x18D8` ohne Aufrufer) / Sprecher Ada. Läuft auf der Disc nicht, im Port LEBT |
| D400 #7 | / いいからはやく！ | Egal, schnell! | 4000 #7 [WOERTLICH] | Disc: Leon, tot (`ROOM4000 sub16 @0x18D8` ohne Aufrufer) / Sprecher Leon. Läuft auf der Disc nicht, im Port LEBT |
| D400 #8 | / マーヴィンを頼む！ | Kümmere dich um Marvin! | 4000 #8 [WOERTLICH] | Disc: Leon, tot (`ROOM4000 sub16 @0x18D8` ohne Aufrufer) / Sprecher Leon. Läuft auf der Disc nicht, im Port LEBT |
| D400 #9 | / 何？ ここは…!? | Was? Was ist das hier…!? | 4001 #3 [WOERTLICH] | Disc: Elza / Sprecher Elza |
| D400 #10 | / こいつはすげぇな… | Nicht schlecht… | 4001 #4 [WOERTLICH] | Disc: Elza / Sprecher John |
| D400 #11 | / シェリー ここはどこなの？ | Sherry, wo sind wir hier? | 4001 #5 [WOERTLICH] | Disc: Elza / Sprecher Elza |
| D400 #12 | / あなたこんな所まで知っているの？ | Du kennst sogar diesen Ort? | 4001 #6 [ABGEWANDELT] | Disc: Elza / Sprecher Elza. Abweichung: Sprecherwechsel Elza → John; Disc „Du kennst sogar diesen Ort?“ wird allgemeine Frage |
| D400 #13 | ここはパパの研究所よ / どこかにパパがいるはずだわ | Das ist Papas Forschungslabor. Irgendwo muss Papa sein. | 4001 #7 [WOERTLICH] | Disc: Elza / Sprecher Sherry |
| D400 #14 | / 私 捜してくる！ | Ich suche ihn! | 4001 #8 [WOERTLICH] | Disc: Elza / Sprecher Sherry |
| D400 #15 | / ちょっとシェリー！ | He, Sherry! | 4001 #9 [ABGEWANDELT] | Disc: Elza / Sprecher Elza. Abweichung: Sprecherwechsel Elza → John |
| D400 #16 | / ダメよ一人じゃ！ | Nicht allein! | 4001 #10 [ABGEWANDELT] | Disc: Elza / Sprecher Elza. Abweichung: Sprecherwechsel Elza → John |
| D400 #17 | / シェリー！ | Sherry! | 4001 #11 [WOERTLICH] | Disc: Elza / Sprecher Elza |
| D400 #18 | / エルザ おうぞ！ | Elza, hinterher! | 4001 #12 [WOERTLICH] | Disc: Elza / Sprecher John |
| D401 #0 | / レオン この傷… / 急がないとまずい | Leon, diese Wunde… wenn wir uns nicht beeilen, wird es schlimm. | 4010 #0 [ABGEWANDELT]; 4010 #1 [ABGEWANDELT] | Disc: Leon / Sprecher Ada. Abweichung: Disc-Zeile auf zwei Sprecher verteilt: Ada sagt nur „diese Wunde…“, die Eile spricht Leon (#1) |
| D401 #1 | / エイダ…君は一体何者だ？ / まさか… | Ada… wer bist du eigentlich? Etwa… | 4010 #2 [WOERTLICH]; 4010 #3 [WOERTLICH] | Disc: Leon / Sprecher Leon |
| D401 #2 | / そうだ 私はアンブレラの人間だ | Ja. Ich gehöre zu Umbrella. | 4010 #4 [WOERTLICH] | Disc: Leon / Sprecher Ada |
| D401 #3 | / レオン…今は話している時間は無い / 急がないとこいつは死んでしまうぞ | Leon… jetzt ist keine Zeit zum Reden. Wenn wir uns nicht beeilen, stirbt er. | 4010 #5 [WOERTLICH]; 4010 #6 [WOERTLICH] | Disc: Leon / Sprecher Ada |
| D401 #4 | / どうすればいいんだ？ | Was soll ich tun? | 4010 #7 [WOERTLICH] | Disc: Leon / Sprecher Leon |
| D401 #5 | やつらのすがたはウィルスによるものだ / ワクチンさえうてば 発びょうはしない | Deren Zustand kommt vom Virus. Mit einer Impfung bricht es nicht aus. | 4010 #8 [WOERTLICH] | Disc: Leon / Sprecher Ada |
| D401 #6 | / そのワクチンはどこに？ | Wo ist der Impfstoff? | 4010 #9 [WOERTLICH] | Disc: Leon / Sprecher Leon |
| D401 #7 | ワクチン自体はここにはないが / ”ウィルスかぶ”から作ることは出来る / しかし… | Der Impfstoff selbst ist nicht hier, aber man kann ihn aus dem „Virusstamm“ herstellen. Aber… | 4010 #10 [WOERTLICH]; 4010 #11 [WOERTLICH] | Disc: Leon / Sprecher Ada |
| D401 #8 | / どうした？ エイダ？ | Was ist, Ada? | 4010 #12 [WOERTLICH] | Disc: Leon / Sprecher Leon |
| D401 #9 | / レオン 私はきょう力出来ない | Leon, ich kann nicht helfen. | 4010 #13 [WOERTLICH] | Disc: Leon / Sprecher Ada |
| D401 #10 | / 何だって！？ | Was!? | 4010 #14 [WOERTLICH] | Disc: Leon / Sprecher Leon |
| D401 #11 | / やらなければならない事がある お前たちにはいろいろと助けられたが / これだけは私の手で！ | Ich habe etwas zu erledigen. Ihr habt mir viel geholfen, aber das muss ich selbst tun! | 4010 #15 [WOERTLICH]; 4010 #16 [WOERTLICH] | Disc: Leon / Sprecher Ada |
| D401 #12 | / 俺は… / 一人でもやるぜ！ | Ich… mache es notfalls allein! | 4010 #17 [WOERTLICH]; 4010 #18 [WOERTLICH] | Disc: Leon / Sprecher Leon |
| D401 #13 | / おしえてくれ！ どうすればいい？ | Sag mir, wie! | 4010 #19 [WOERTLICH] | Disc: Leon / Sprecher Leon |
| D401 #14 | この研究所のちょうていおん実験室に / ウィルスかぶが保存されているはずだ それを装置にセットしてプログラムを / はしらせば後は自動でやってくれる だが ウィルスかぶは / ひどくデリケートだ 取りあつかいにちゅう意しなくては / 全てがダメになるぞ | Im Ultratiefkühl-Labor hier muss der Virusstamm lagern. Setz ihn ins Gerät, starte das Programm, der Rest läuft automatisch. Aber der Stamm ist äußerst empfindlich. Geh falsch damit um, und alles ist hin. | 4010 #20 [WOERTLICH]; 4010 #21 [SINNGEMAESS]; 4010 #22 [ABGEWANDELT]; 4010 #23 [WOERTLICH] | Disc: Leon / Sprecher Ada. Abweichung: „Setz ihn ins Gerät“ fehlt |
| D401 #15 | / やるしかない / マーヴィン… 必ず助けてやる | Es bleibt nichts anderes. Marvin… ich rette dich. | 4010 #24 [WOERTLICH]; 4010 #25 [WOERTLICH] | Disc: Leon / Sprecher Leon |
| D401 #16 | レオン 全てがおわったら / 地下のシェルターに行け！ / でないといのちの保証は出来ない ここはアンブレラの研究所だ / くれぐれも気をつけろよ | Leon, wenn alles vorbei ist, geh zum unterirdischen Schutzraum! Sonst kann ich für dein Leben nicht garantieren. Das hier ist ein Umbrella-Labor, sei bloß vorsichtig. | 4010 #26 [WOERTLICH]; 4010 #27 [SINNGEMAESS]; 4010 #28 [WOERTLICH] | Disc: Leon / Sprecher Ada. Abweichung: „kann für dein Leben nicht garantieren“ → „you probably won't survive“ |
| D401 #17 | / おい エイダ！ / いのちの保証ができない…？ | He, Ada! Nicht für mein Leben garantieren…? | 4010 #29 [WOERTLICH]; 4010 #30 [SINNGEMAESS] | Disc: Leon / Sprecher Leon. Abweichung: Echo von #27 statt „nicht garantieren…?“ |
| D401 #18 | / 急いでワクチンを持ってこなければ！ | Ich muss den Impfstoff schnell holen! | 4010 #31 [WOERTLICH] | Disc: Leon + Elza / Sprecher Spielfigur, Raumtext in 401 und 501. Port nur 4010 #31 (Leon in 401); in 4011 fehlt der Raumtext, Elza bekommt ihn in 501 (5011 #43) |
| D401 #19 | / どうした？ マーヴィン目をさませ！！ | Was ist? Marvin, wach auf!! | 4010 #32 [WOERTLICH] | Disc: Leon, tot (`ROOM4010 sub06 @0x908` ohne Aufrufer) / Sprecher Leon. Läuft auf der Disc nicht, im Port LEBT |
| D401 #20 | / エイダ！ | Ada! | 4010 #33 [WOERTLICH] | Disc: Leon, tot (`ROOM4010 sub06 @0x908` ohne Aufrufer) / Sprecher Leon. Läuft auf der Disc nicht, im Port LEBT |
| D401 #21 | レオン！ 何をしている！ / ここもウィルスにやられるぞ！ | Leon! Was machst du! Das Virus erreicht auch diesen Ort! | 4010 #34 [WOERTLICH] | Disc: Leon, tot (`ROOM4010 sub06 @0x908` ohne Aufrufer) / Sprecher Ada. Läuft auf der Disc nicht, im Port LEBT |
| D401 #22 | / シェルターへ急ぐぞ！ | Schnell zum Schutzraum! | 4010 #35 [WOERTLICH] | Disc: Leon, tot (`ROOM4010 sub06 @0x908` ohne Aufrufer) / Sprecher Ada. Läuft auf der Disc nicht, im Port LEBT |
| D401 #23 | / モニターが並んでいる ▼ 電力が来ていないのか / 完全に死んでしまっている | Eine Reihe Monitore. Kein Strom offenbar, völlig tot. | 4010 #36 [WOERTLICH]; 4011 #0 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D401 #24 | / 机の上には何も置かれていない | Auf dem Tisch liegt nichts. | 4010 #37 [WOERTLICH]; 4011 #1 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D401 #25 | / この装置は使われていなかったようだ | Dieses Gerät wurde wohl nicht benutzt. | 4010 #38 [ABGEWANDELT]; 4011 #2 [ABGEWANDELT] | Disc: Leon + Elza / Sprecher System. Abweichung: Disc „wurde wohl nicht benutzt“ → Port „unbenutzbar“ (Fehlübersetzung, leicht) |
| D401 #26 | / メモがはられている ▼ マイケルへ / 左の机の上にIDカードを置いておく ▼ 今度こそ無くすなよ / ヘンリー | Eine Notiz: „An Michael: Ich lege die ID-Karte auf den linken Tisch. Verlier sie diesmal nicht. Henry“ | 4010 #39 [ABGEWANDELT]; 4011 #3 [ABGEWANDELT]; 4050 #3 [ABGEWANDELT]; 4051 #3 [ABGEWANDELT] | Disc: Leon + Elza / Sprecher System. Abweichung: Disc: ID-Karte auf dem LINKEN Tisch (左の机); Port: „right desk“. Variante zusätzlich in 4050 #3 (Mod-Raum 405) |
| D401 #27 | / このロッカーには別に何も無い | In diesem Spind ist nichts Besonderes. | 4010 #40 [WOERTLICH]; 4011 #4 [WOERTLICH]; 4050 #0 [WOERTLICH]; 4051 #0 [WOERTLICH] | Disc: Leon + Elza / Sprecher System. Wortlaut zusätzlich im Mod-Raum 405 (4050 #0) |
| D401 #28 | このフリーザーも電力が来ていない / 少しつんとしたイヤなにおいがする | Auch dieser Gefrierschrank hat keinen Strom. Es riecht unangenehm stechend. | 4010 #41 [SINNGEMAESS]; 4011 #5 [SINNGEMAESS] | Disc: Leon + Elza / Sprecher System. Abweichung: „auch“ und „stechend“ fehlen |
| D403 #0 | ”メインヒューズ” / 大がかりなブレーカーシステムだ / ▼ 誰かが作動させたらしく / ヒューズ部分が全て外れている / | „Hauptsicherung“. Eine große Schalteranlage. Jemand hat sie wohl ausgelöst, alle Sicherungen sind herausgesprungen. | 4030 #0 [SINNGEMAESS]; 4031 #0 [SINNGEMAESS] | Disc: Leon + Elza / Sprecher System. Abweichung: Disc: jemand hat die Anlage ausgelöst, Sicherungen herausgesprungen; Port: jemand hat sie entfernt |
| D403 #1 | リペアープラグを使いますか？ / [Ja/Nein] | Reparaturstecker benutzen? | 4030 #1 [ABGEWANDELT]; 4031 #1 [ABGEWANDELT] | Disc: Leon + Elza / Sprecher System. Abweichung: Itemname: Disc リペアープラグ (Reparaturstecker), Port „Spark Plug“ (README: Modell in 404 ergänzt) |
| D403 #2 | ヒューズはすべて接続されている / 今まで行けなかった所にも行けるはずだ / | Alle Sicherungen sind verbunden. Jetzt müsste man auch dorthin kommen, wo es bisher nicht ging. | 4030 #2 [WOERTLICH]; 4031 #2 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D403 #3 | このフットランプのおかげで / 下の方だけはかろうじて見える / | Dank dieser Bodenlampen sieht man unten gerade noch etwas. | 4030 #3 [WOERTLICH]; 4031 #3 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D403 #4 | ときおりランプの光が / 品まっているようだが・・・ / | Das Lampenlicht scheint zeitweise schwächer zu werden… [Lesart] | 4030 #4 [WOERTLICH]; 4031 #4 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D403 #5 | シェリー！ / | Sherry! | 4031 #5 [ABGEWANDELT] | Disc: Elza. In 4030 tot (`@0x52C`) / Sprecher Elza. Abweichung: Sprecherwechsel Elza → John |
| D403 #6 | 何てこった！ どこへ行ったんだ？ / | Verdammt! Wo ist sie hin? | 4031 #6 [WOERTLICH] | Disc: Elza. In 4030 tot (`@0x52C`) / Sprecher John |
| D403 #7 | 手分けして捜しましょう！ / | Teilen wir uns auf und suchen! | 4031 #7 [WOERTLICH] | Disc: Elza. In 4030 tot (`@0x52C`) / Sprecher Elza |
| D403 #8 | OK！ 俺はこっちだ / エルザは向こうを頼むぜ / | OK! Ich gehe hier lang, Elza, du dort! | 4031 #8 [WOERTLICH] | Disc: Elza. In 4030 tot (`@0x52C`) / Sprecher John |
| D40A #0 | スイッチがある・・・押しますか？ / [Ja/Nein] | Da ist ein Schalter… Drücken? | 40A0 #0 [WOERTLICH]; 40A1 #0 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D40A #1 | “火気げんきん”のマーキングがある / 危険なもののようだ | Markierung „Feuer verboten“. Offenbar etwas Gefährliches. | 40A0 #1 [WOERTLICH]; 40A1 #1 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D40A #2 | 救急用のベッドだ / あまり使った跡が無い | Ein Notfallbett, kaum benutzt. | 40A0 #2 [WOERTLICH]; 40A1 #2 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D40A #3 | げん重にロックされている / 開けるにはIDカードが必要だ | Streng verriegelt. Zum Öffnen braucht man eine ID-Karte. | 40A0 #3 [WOERTLICH]; 40A1 #3 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D40A #4 | [研究所のIDカード]を使いますか [Ja/Nein] | [ID-Karte des Labors] benutzen? | 40A0 #4 [WOERTLICH]; 40A1 #4 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher System |
| D40A #5 | / CTスキャンのたぐいだ ▼ 研究員の体調をかんりするためだろう / ひんぱんに作動していたようだ | Eine Art CT-Scanner. Wohl zur Gesundheitskontrolle der Forscher, wurde häufig benutzt. | 40A0 #5 [ABGEWANDELT]; 40A1 #5 [ABGEWANDELT] | Disc: Leon + Elza / Sprecher System. Abweichung: Disc: zur Gesundheitskontrolle der Forscher; Port: „die Forscher müssen recht krank sein“ (Fehlübersetzung) |
| D40A #6 | / 棚があるが中は暗くて分からない | Ein Regal, innen zu dunkel, um etwas zu erkennen. | 40A0 #6 [WOERTLICH]; 40A1 #6 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D40A #7 | / 手さぐりで調べるのは危険だ | Blind herumzutasten wäre gefährlich. | 40A0 #7 [WOERTLICH]; 40A1 #7 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D40A #8 | / 暗くてよく分からない | Zu dunkel, nichts zu erkennen. | 40A0 #8 [WOERTLICH]; 40A1 #8 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D40A #9 | / 暗すぎてよく見えない | Viel zu dunkel. | 40A0 #9 [WOERTLICH]; 40A1 #9 [WOERTLICH] | Disc: Elza / Sprecher System. Port-Zeilen ohne Aufruf |
| D40A #10 | / 手術台のようだが暗くてよく分からない | Offenbar ein OP-Tisch, aber zu dunkel, um Genaues zu sehen. | 40A0 #10 [WOERTLICH]; 40A1 #10 [WOERTLICH]; 40B0 #2 [ABGEWANDELT]; 40B1 #2 [ABGEWANDELT] | Disc: Leon + Elza / Sprecher System. Abweichung: Zwei Disc-Zeilen aus 40A (OP-Tisch, Blut) zusammengezogen und in den Mod-Raum 40B versetzt, hier ohne „zu dunkel“. mit #11 zusätzlich in 40B0 #2 (Mod-Raum 40B) zusammengezogen |
| D40A #11 | / 血のようなものがこびりついている | Etwas wie Blut klebt daran. | 40A0 #11 [WOERTLICH]; 40A1 #11 [WOERTLICH]; 40B0 #2 [ABGEWANDELT]; 40B1 #2 [ABGEWANDELT] | Disc: Leon + Elza / Sprecher System. Abweichung: Zwei Disc-Zeilen aus 40A (OP-Tisch, Blut) zusammengezogen und in den Mod-Raum 40B versetzt, hier ohne „zu dunkel“. mit #10 zusätzlich in 40B0 #2 (Mod-Raum 40B) zusammengezogen |
| D501 #0 | / シェリー しっかりしな！ | Sherry, halt durch! | 5011 #0 [ABGEWANDELT] | Disc: Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) / Sprecher John. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Sprecherwechsel John → Elza; „halt durch“ → „wach auf“ |
| D501 #1 | / だめだ… 気ぃ失なってるぜ | Nichts zu machen… sie ist bewusstlos. | 5011 #1 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #2 | / あぁ… シェリー | Ach… Sherry. | 5011 #2 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #3 | ジョン この研究所のファイルを見てて / おそろしい事が分かったの | John, in den Akten dieses Labors habe ich etwas Furchtbares gefunden. | 5011 #3 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #4 | この子の体は ウィルスに / むしばまれているのよ… | Der Körper dieses Kindes wird vom Virus zerfressen… | 5011 #4 [SINNGEMAESS] | Disc: Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT. Abweichung: „vom Virus zerfressen“ → „infiziert mit dem, woran hier gearbeitet wurde“ |
| D501 #5 | / エルザ こいつを見てみな | Elza, sieh dir das an. | 5011 #5 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #6 | / 何て事… | Wie furchtbar… | 5011 #6 [ABGEWANDELT] | Disc: nicht ausgelöst / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Ausruf „Wie furchtbar…“ → Frage „What is this…?“ (leicht) |
| D501 #7 | / ワクチンをうたねぇとシェリーは… | Ohne Impfung wird Sherry… | 5011 #7 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #8 | / いいわ 私が行く | Gut, ich gehe. | 5011 #8 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #9 | / 何？ | Was? | 5011 #9 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #10 | こうしている間にもシェリーの体は / ウィルスにむしばまれているわ | Auch jetzt gerade wird Sherrys Körper vom Virus zerfressen. | 5011 #10 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #11 | / イチかバチか やってみるしかないわ | Alles oder nichts, ich muss es versuchen. | 5011 #11 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #12 | / エルザ… | Elza… | 5011 #12 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #13 | / ジョン しっかりみててね | John, pass gut auf sie auf. | 5011 #13 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #14 | / 今のシェリーにはあなたが必要よ | Sherry braucht dich jetzt. | 5011 #14 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #15 | / 頼むぜエルザ | Ich verlasse mich auf dich, Elza. | 5011 #15 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #16 | / この子だけはぜっ対助けてやりてぇ | Dieses Kind will ich unbedingt retten. | 5011 #16 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #17 | / ジョン ワクチンよ！ | John, der Impfstoff! | 5011 #17 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #18 | / シェリーは？ | Was ist mit Sherry? | 5011 #18 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #19 | / はやく！ もう長いことうなされてる | Schnell! Sie stöhnt schon lange im Schlaf. | 5011 #19 [ABGEWANDELT] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher John. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Disc: „sie stöhnt schon lange im Schlaf“; Port: „sie hat nicht mehr lange“ (Fehlübersetzung, dramatisiert) |
| D501 #20 | / シェリー！ 目を開けて！ | Sherry! Mach die Augen auf! | 5011 #20 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #21 | シェリー！ さぁどうした / へん事をしてくれ！ | Sherry! Komm schon, antworte! | 5011 #21 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #22 | / お願い… シェリー 起きて… | Bitte… Sherry, wach auf… | 5011 #22 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #23 | / どうしたの？ | Was ist denn? | 5011 #23 [ABGEWANDELT] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Sherry. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Sprecherwechsel Sherry → John: Sherrys erstes Wort beim Erwachen fehlt |
| D501 #24 | / シェリー！ 気がついたか！ | Sherry! Du bist wach! | 5011 #24 [ABGEWANDELT] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher John. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Sprecherwechsel John → Elza |
| D501 #25 | / よかった… | Gott sei Dank… | 5011 #25 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #26 | / 本当 心配したわよ | Du hast uns wirklich Sorgen gemacht. | 5011 #26 [ABGEWANDELT] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Sprecherwechsel Elza → John |
| D501 #27 | / ジョン アナウンスは聞いた？ | John, hast du die Durchsage gehört? | 5011 #27 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #28 | / あぁ 一体何が起こってんだ？ | Ja. Was ist hier eigentlich los? | 5011 #28 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #29 | / 怪物がこのしせつの動力を破壊したのよ | Das Monster hat die Energieversorgung der Anlage zerstört. | 5011 #29 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #30 | / ばく発でウィルスがばらまかれるわ | Bei der Explosion wird das Virus freigesetzt. | 5011 #30 [ABGEWANDELT] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Disc: Explosion setzt das Virus frei; Port: Feuer breitet sich aus |
| D501 #31 | / 何だって!?それじゃぁ俺たちもやばいぜ | Was!? Dann sind wir auch dran! | 5011 #31 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #32 | そうよ ここはまずいわ / はやくシェルターへ行きましょう！ | Genau, hier ist es gefährlich. Schnell zum Schutzraum! | 5011 #32 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT |
| D501 #33 | / シェリー シェルターは分かる？ | Sherry, kennst du den Schutzraum? | 5011 #33 [ABGEWANDELT] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Elza. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Sprecherwechsel Elza → John |
| D501 #34 | / う、うん | J-ja. | 5011 #34 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Sherry. Läuft auf der Disc nicht, im Port LEBT |
| D501 #35 | / 2号シェルターなら開けられるわ | Schutzraum Nr. 2 kann ich öffnen. | 5011 #35 [ABGEWANDELT] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher Sherry. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Disc: Sherry KANN Schutzraum Nr. 2 öffnen; Port: „sollte offen sein“ |
| D501 #36 | / よし じゃぁ行こうぜ | Gut, dann los. | 5011 #36 [WOERTLICH] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher John. Läuft auf der Disc nicht, im Port LEBT |
| D501 #37 | / シェリー よろしく頼むぜ | Sherry, ich verlasse mich auf dich. | 5011 #37 [ABGEWANDELT] | Disc: Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) / Sprecher John. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Sprecherwechsel John → Elza; „ich verlasse mich auf dich“ → „geh voran“ |
| D501 #38 | / あわてて中をひっかき回した跡がある ▼ / 役に立ちそうなものは無い | Hier wurde hastig alles durchwühlt. Nichts Brauchbares. | 5010 #0 [ABGEWANDELT]; 5011 #38 [ABGEWANDELT] | Disc: Leon + Elza / Sprecher System. Abweichung: Disc: hastig durchwühlt (ひっかき回した跡); Port: „Fußspuren im Kreis“ (Fehlübersetzung) |
| D501 #39 | / 机の上はひどくほこりっぽい ▼ / 特に何も無い | Der Tisch ist staubig. Nichts Besonderes. | 5010 #1 [WOERTLICH]; 5011 #39 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D501 #40 | / 少し前まで使われていたようだ | Bis vor Kurzem wurde das noch benutzt. | 5010 #2 [WOERTLICH]; 5011 #40 [WOERTLICH] | Disc: Leon + Elza / Sprecher System. Port-Zeilen ohne Aufruf |
| D501 #41 | さまざまな機器の後ろに / メモなどがはられたボードがある ▼ / 研究所内での仕事のおぼえ書きのようだ | Hinter den Geräten hängt eine Tafel mit Notizen. Offenbar Arbeitsvermerke des Labors. | 5010 #3 [WOERTLICH]; 5011 #41 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D501 #42 | ゴミばこの中にはくしゃくしゃの / 紙くずが無造作に放りこまれている | Im Papierkorb liegen achtlos zerknüllte Zettel. | 5010 #4 [WOERTLICH]; 5011 #42 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D501 #43 | / 急いでワクチンを持ってこなければ！ | Ich muss den Impfstoff schnell holen! | 5011 #43 [WOERTLICH] | Disc: Leon + Elza / Sprecher Spielfigur, Raumtext in 401 und 501. Port nur 5011 #43 (Elza in 501); in 5010 fehlt er, Leon bekommt ihn in 401 (4010 #31) |
| D504 #0 | ”左:Pー4レベル実験室” / ”右:コンピュータールーム” | „Links: P-4-Labor“, „Rechts: Computerraum“ | 5040 #0 [WOERTLICH]; 5041 #0 [WOERTLICH] | Disc: Leon + Elza / Sprecher Schild |
| D506 #0 | 作 用のスーツの入ったロッカーだ / こわれてしまっているのか開かない | Ein Spind mit Arbeitsanzügen. Offenbar kaputt, er geht nicht auf. [Lesart 作業用] | 5060 #0 [WOERTLICH]; 5061 #0 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D506 #1 | 中には何も無い ▼ あわてて開け放しにしたように見える | Nichts drin. Sieht aus, als sei er hastig offen gelassen worden. | 5060 #1 [SINNGEMAESS]; 5061 #1 [SINNGEMAESS] | Disc: Leon + Elza / Sprecher System. Abweichung: „hastig offen gelassen“ → „hastig ausgeräumt“ |
| D506 #2 | バイオハザードレベルを している ▼ 今は安全レベルのようだ | Zeigt die Biohazard-Stufe an. Im Moment offenbar eine sichere Stufe. [Lesart] | 5060 #2 [WOERTLICH]; 5061 #2 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D506 #3 | 安全キャビネットだ ▼ しっかりとみっぺいされている | Ein Sicherheitsschrank, fest versiegelt. | 5060 #3 [WOERTLICH]; 5061 #3 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D506 #4 | 固定ボルトが外れかかっているようだ ▼ 中のものが危険なもので / 無ければいいのだが | Die Halteschrauben lösen sich. Hoffentlich ist der Inhalt nicht gefährlich. | 5060 #4 [ABGEWANDELT]; 5061 #4 [ABGEWANDELT] | Disc: Leon + Elza / Sprecher System. Abweichung: Disc: Schrauben lösen sich, Inhalt hoffentlich ungefährlich; Port: Bolzen entfernt, „was inside“ = Inhalt fort (leicht) |
| D506 #5 | 作 中のハプニングによるものか / キズのような跡がある | Spuren wie Kratzer, wohl von einem Zwischenfall bei der Arbeit. [Lesart] | 5060 #5 [ABGEWANDELT]; 5061 #5 [ABGEWANDELT] | Disc: Leon + Elza / Sprecher System. Abweichung: Disc: Kratzspuren von einem Arbeitsunfall; Port: Fußspuren eines Verletzten |
| D506 #6 | この流し台もクリーンに保たれている | Auch dieses Spülbecken wird sauber gehalten. | 5060 #6 [WOERTLICH]; 5061 #6 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D506 #7 | 室内にガスが発生したとき / 動作するドラフトチャンバーだ ▼ これもクリーンに保たれていて / 作動した事があったのかはわからない | Ein Abzug, der bei Gas im Raum anspringt. Ebenfalls sauber, ob er je lief, ist nicht zu erkennen. | 5060 #7 [WOERTLICH]; 5061 #7 [WOERTLICH] | Disc: Leon + Elza / Sprecher System |
| D506 #8 | ウィルスにかかわる機 だろう / 特に必要とするものはない | Wohl ein Gerät für Virusarbeiten. Nichts Nötiges. [Lesart] | 5060 #8 [SINNGEMAESS]; 5061 #8 [SINNGEMAESS] | Disc: Leon + Elza / Sprecher System. Abweichung: „Gerät für Virusarbeiten“ → „virus injector“ |
| D506 #9 | このROMではまだ / ”調べる”ができません | In dieser ROM ist „Untersuchen“ noch nicht möglich. | 5060 #9 [WOERTLICH]; 5061 #9 [WOERTLICH] | Disc: Leon + Elza / Sprecher Entwicklervermerk |
| D50A #0 | / シャッターのパネルだ ▼ 特別なIDカードが無ければ / 開かない様な事が書かれている | Ein Panel für das Rolltor. Offenbar öffnet es nur mit einer besonderen ID-Karte. | 50A0 #0 [SINNGEMAESS]; 50A1 #0 [SINNGEMAESS] | Disc: nicht ausgelöst / Sprecher System. Läuft auf der Disc nicht, im Port LEBT. Abweichung: „besondere ID-Karte“ und „steht geschrieben“ fehlen |
| D50A #1 | / 右のシャッターがハンガーのようだ | Das rechte Rolltor führt offenbar zum Hangar. | 50A0 #1 [WOERTLICH]; 50A1 #1 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher System. Läuft auf der Disc nicht, im Port LEBT |
| D50A #2 | メンテナンス用のパネルがあるが / あまり使用していないように見える | Ein Wartungspanel, sieht wenig benutzt aus. | 50A0 #2 [SINNGEMAESS]; 50A1 #2 [SINNGEMAESS] | Disc: nicht ausgelöst / Sprecher System. Läuft auf der Disc nicht, im Port LEBT. Abweichung: „wenig benutzt“ → „not any use“ |
| D50A #3 | 研究員が迷わないように / 書かれているのだろう | Steht wohl da, damit sich die Forscher nicht verlaufen. | 50A0 #3 [ABGEWANDELT]; 50A1 #3 [ABGEWANDELT] | Disc: nicht ausgelöst / Sprecher System. Läuft auf der Disc nicht, im Port LEBT. Abweichung: Mod-Zusatz: Schildtext „Emergency Staircase“ (verweist auf Mod-Raum 407) |
| D512 #0 | ”左:Pー4レベル実験室” / ”右:コンピュータールーム” | „Links: P-4-Labor“, „Rechts: Computerraum“ | 5120 #0 [WOERTLICH]; 5121 #0 [WOERTLICH] | Disc: Leon + Elza / Sprecher Schild |
| D512 #1 | / 火事でロックされてしまっている！ | Durch das Feuer verriegelt! | 5120 #1 [WOERTLICH]; 5121 #1 [WOERTLICH] | Disc: nicht ausgelöst / Sprecher System. Läuft auf der Disc nicht, im Port LEBT |
| D514 #0 | / 開いたわ この奥よ | Es ist offen. Da hinten. | 5140 #0 [ABGEWANDELT]; 5141 #0 [ABGEWANDELT] | Disc: nicht ausgelöst / Sprecher Frauenstimme, vom Ton her nicht Ada, Sherry denkbar, offen. Abweichung: Disc: Frauenstimme „Es ist offen, da hinten“; Port: Systemtext „offen auf dieser Seite“, Sprecherin entfällt; beidseitig kein Aufruf |

## Auffälligste Abweichungen (Story)

**Das Ende ab 50B ist ganz Mod-Text.** Die Disc hat in 50B, 508, 509, 600–602 keine Datei und in 603/604 keinen Text. Belegt ist dort nur, wer wo steht.

1. **50B0 #0–#18, Adas Tod (Leon):** eine Szene, die nur im Mod steht. Neu sind: Annette mit Namen, Annette ist Birkins Frau und Adas Chefin (#3), Birkin ist tot, von Umbrella getötet (#6), die Kreaturen sollten als Biowaffen verschickt werden (#8/#9), Ada stirbt (#13–#16). Auf der Disc trennt sich Ada in 401 („ich habe etwas zu erledigen“, D401 #11). Ihre Rückkehr (D401 #21/#22, „Schnell zum Schutzraum!“) liegt in totem Code. Einen Tod Adas zeigt die Disc nirgends.
2. **50B0 #10/#11:** Ada habe den Reaktor zur Sprengung gebracht. Das widerspricht D501 #29 und 5011 #29, dort hat das Monster die Energie zerstört.
3. **5011 #30:** Disc „Bei der Explosion wird das Virus freigesetzt“, Port „fire's starting to spread“. Die Virusgefahr als Grund für den Schutzraum fällt weg.
4. **Annette als sprechende Figur (6000 #0–#4, 6030 #6/#7/#11–#19, 50B0):** Die Disc kennt den Namen nicht. Belegt ist nur Figur 0x4D, die Kittel-Frau, in Leons 603 (BERICHT §6) und auf Fotos am Bahnsteig (#152/#154). Dass sie den Zug startet, ist Mod-Text. Die blinkende WARNING-Konsole in 603 steht auf der Disc, Text dazu nicht.
5. **6030 #0–#8, Leon spricht mit Sherry und John:** Auf der Disc stehen Marvin, John und Sherry in **604** mit Leon im Raum, ohne Text. Leons Blick zu ihnen (`sub04`) hat keinen Aufrufer. Der Mod verlegt die Gruppe nach 603, an die Disc-604-Plätze (John −26055/−25784), und lässt sie reden. Leon–Sherry und Leon–John sind damit Gespräche, die die Disc nicht hat.
6. **6030 #2/#3, 6031 #1, die andere Hauptfigur kämpft draußen:** Erfindung (BERICHT §6). Die Overrides nehmen nur die Bekanntschaft zurück. Bei Leon nennt Sherry jetzt Elza beim Namen, bei Elza kämpft „a policeman“. Das Draußen-Kämpfen bleibt.
7. **6030 #11, „Where's that biker girl?“:** Annette kennt Elza. Das setzt eine Begegnung Annette–Elza voraus, die nirgends belegt ist. Der Override (#12) ändert nur Leons Antwort.
8. **50B1 #0:** Im Mod findet Elza Adas Leiche („It's Ada“). Der Port macht daraus eine unbekannte Frau. Auf der Disc gibt es Elza–Ada nur als Zweig in 603, an Flag (4,22), das nie gesetzt wird.
9. **5011, Sprecher getauscht in der Impfstoff-Szene:** #23 nimmt Sherry ihr erstes Wort beim Erwachen („Was ist denn?“) und gibt es John. #24 und #37 wandern von John zu Elza, #26 und #33 von Elza zu John, #0 von John zu Elza.
10. **5011 #35:** Disc „Schutzraum Nr. 2 kann ich öffnen“ (Sherry hat Zugang), Port „The second shelter should be open“.
11. **400/403, Elzas Zeilen gehen an John:** 4001 #6, #9, #10 („Nicht allein!“) und 4031 #5. Im Port hält John Sherry zurück, auf der Disc tut das Elza.
12. **5011 #19:** Disc „sie stöhnt schon lange im Schlaf“, Port „She doesn't have long!“. Der Port dramatisiert.
13. **4010 #22:** Der Virusstamm heißt „Virus Inhibitor“. **4010 #39 / 4011 #3:** Henrys Notiz legt die ID-Karte auf den rechten statt den linken Tisch. **4050 #3:** Mod-Variante derselben Notiz mit neuen Namen (David, Eric).
14. **Neue Begriffe und Mechanik:** „T-Virus“ (4090 #1), T- und G-Vaccine je Figur (5060/5061 #13), „Blue Master Keycard“ statt ID-Karte (4000 #1/#2), „Spark Plug“ statt Reparaturstecker (4030 #1), Generator für den Notfall-Zug (5080), Schutzraum öffnet „nur im Notfall“ (5070), Spielende-Text „THANK YOU FOR PLAYING !!!“ in 509 (5090 #0).
15. **Szenen, die auf der Disc nicht laufen, laufen im Port:** Leon hält in 400 auf (D400 #5–8), Marvin bricht in 401 zusammen und Ada kehrt zurück (D401 #19–22), dazu die ganze Sherry-Szene in 501 (D501 #0–37). Der Wortlaut ist meist treu. Belegt ist damit Capcoms Plan, nicht der Ablauf der Disc.
