# Textabgleich STAGE 2 und 3: englische Port-Zeilen gegen die 40%-Disc

Stand 2026-11-02, Spur ST80-P. Grundlage: `texte_paare.md` (Teil A und B, Räume 2xx und 3xx), `TEXTE_40.md`, `BERICHT.md`, Prüf-Dossier `analysis/story_begegnungen/pruefung/texte.md` (Lesarten, Sprecher-Regie), `disc_zensus.json` / `port_zensus.json` (Aufrufstellen, NPCs), Mod-README `info/Re1.5/README.TXT`. Erzeugt mit einem Hilfsskript aus `texte_paare.md` (englischer Wortlaut maschinell übernommen, Urteile von Hand).

**Umfang:** 315 englische Zeilen in 47 Dateien (ROOM2000–20A1, ROOM3000–30E1). Ohne Text in Port und Disc: 2090/2091, 20B0/20B1, 20C0–20F1, 3030/3031, 3081, 30F0/30F1. 20B: Disc und Mod ohne Text, auf der Disc steht nur John in 20B1, dorthin führt keine Tür (BERICHT §5).

**Regeln dieser Auswertung:**
- Vorlagen wurden auch raumübergreifend gesucht. Gleiche Aussage in anderem Raum zählt als WOERTLICH/SINNGEMAESS. ABGEWANDELT setzt gleichen Anlass oder Platz voraus. Eine Dialogzeile, deren Wortlaut zufällig einer Disc-Zeile gleicht, die aber in einer neuen Mod-Szene steht (3020 #2), bleibt MOD-TEXT.
- **Standardsätze** („It's locked.", „There's a button here. Push it?", „You've opened the lock.") an Stellen ohne Disc-Text tragen die wortgleiche Disc-Zeile als Vorlage und den Vermerk „Standardsatz, Stelle nur Mod". Belegt ist dann nur der Wortlaut, nicht die Stelle.
- Eine Disc-Zeile, die der Mod auf mehrere Ids verteilt, ist nach Vorgabe ABGEWANDELT. Wo sonst nichts abweicht, steht „nur Aufteilung, Wortlaut treu".
- Disc-Sprecher nach TEXTE_40.md und Prüf-Dossier: 30E (beide Szenarien) und 306 Elza per Regie bestätigt. 300 ohne Regie (nur Register und Inhalt). 307 #4 ohne Regie. 308 #0 „Marvin oder Leon" (Ada sagt nie 〜ぜ). Die Sprecher-Präfixe im Englischen sind Mod-Zusatz (README Z. 268 „Added missing character names").
- Spielbare Begleiter: Mod und Port machen Sherry ab 3061 zur Spielfigur (ROOM3061 sub02 @0x011F6 setzt work_vars[0x10]=0x0F, 3091 sub07 zurück auf Elza, `re15_port/include/re15_sherry_r41.h`), Marvin ist im Mod in der Fabrik spielbar (README Z. 23), der Port-Raumlader kennt den Wechsel Leon↔Marvin (0x0D) ebenfalls. Die Ich-Zeilen 300 #1 und 309 #8 gehören damit wie auf der Disc angelegt den Begleitern.

## Zählung

| Urteil | Zeilen | davon Standardsatz an Mod-Stelle |
|---|---|---|
| WOERTLICH | 88 | 12 |
| SINNGEMAESS | 26 | 10 |
| ABGEWANDELT | 45 | 0 |
| MOD-TEXT | 153 | 0 |
| PORT-TEXT | 3 | 0 |
| **Summe** | **315** | 22 |

Von den 45 ABGEWANDELT-Zeilen sind 20 reine Aufteilungen mit treuem Wortlaut, 25 weichen inhaltlich ab (Sprecher, Fakten, Fehllesung). Aufruf: LEBT 293, kein Aufruf 22, TOT 0.

Disc-Zeilen der Räume 207, 300, 301, 306, 307, 308, 309, 30E: 100. Ohne Port-Gegenstück: 2 (D309 #21, D309 #24). Dazu D30E #4: vorhanden als 30E0 #6, aber fehlgelesen und ohne Aufruf, spielt im Port also nicht. D207 #0 hat ihr Gegenstück nur raumübergreifend in 20A.

## 1. Tabelle T1: englische Port-Zeilen

| Datei | Id | Aufruf | Sprecher (EN) | Englisch (wortgetreu aus texte_paare.md) | Vorlage | Urteil | Bemerkung |
|---|---|---|---|---|---|---|---|
| ROOM2000 | 0 | LEBT | Leon | Leon: Ada, Marvin, hold on. | - | MOD-TEXT | Disc 2000 ohne Text und ohne NPC. Mod-Text von Juvenal (README Z. 199). Ort und Besetzung Leon/Ada/Marvin zeigt das Spätstand-Foto #103–#105 (dort Zeile マーヴィン…, Rest unlesbar) |
| ROOM2000 | 1 | LEBT | Marvin | Marvin: What's up Leon? | - | MOD-TEXT | Mod-Dialog |
| ROOM2000 | 2 | LEBT | Leon | Leon: There is a little girl that / came in here, we have to find her! | - | MOD-TEXT | Neu: Leon will dem Mädchen nach. Disc: Leon ruft Sherry in 11B nur nach (D11B #9), kein Gespräch über sie |
| ROOM2000 | 3 | LEBT | Marvin | Marvin: A little girl, are you sure? | - | MOD-TEXT | Neu: Marvin erfährt von Sherry |
| ROOM2000 | 4 | LEBT | Leon | Leon: Yes, before meeting Ada I saw / her coming in this way, if I hadn't | - | MOD-TEXT | Neu: Sherry habe Leon den Fluchtweg gezeigt. Disc (tot): Kanaldeckel ist Leons eigene Idee (D11B #16) |
| ROOM2000 | 5 | LEBT | (ohne Praefix, vermutlich Leon) | seen her maybe we wouldn't have / found this way out. | - | MOD-TEXT | Fortsetzung von #4 |
| ROOM2000 | 6 | LEBT | Leon | Leon: She must have thought I was / a zombie. | - | MOD-TEXT | Neu |
| ROOM2000 | 7 | LEBT | Ada | Ada: So there is no time to lose, / we do not know what things are in | - | MOD-TEXT | Neu: Ada drängt zur Suche nach dem Mädchen, keine Disc-Zeile von Ada über Sherry |
| ROOM2000 | 8 | LEBT | (ohne Praefix, vermutlich Ada) | the sewers, zombies are not the only / things we should worry about here. | - | MOD-TEXT | Fortsetzung von #7 |
| ROOM2000 | 9 | LEBT | Leon | Leon: Okay, it will be better to / separate and cover more ground. | - | MOD-TEXT | Neu: Gruppe trennt sich. Disc: in 300 ist Ada nur „schon vorgegangen" (D300 #6), wie es dazu kam, steht nicht auf der Disc |
| ROOM2000 | 10 | LEBT | Leon | Leon: Marvin sees Ada up front. | - | MOD-TEXT | Satz unklar (wohl „Marvin, see Ada up front") |
| ROOM2000 | 11 | LEBT | Marvin | Marvin: Understood. If something / happens, this area will be to meet. | - | MOD-TEXT | Neu: Treffpunkt-Absprache |
| ROOM2001 | 0 | LEBT | John | John: Elza, over here! | - | MOD-TEXT | Disc 2001 ohne Text und ohne NPC |
| ROOM2001 | 1 | LEBT | John | John: Through this staircase you can / access the basement garage. | - | MOD-TEXT | Setzt die Mod-Abkürzung aus 10B1 #5 („sewer tunnels") fort. Disc D10B #3 sagt nur „hier entlang ist es kürzer" |
| ROOM2001 | 2 | LEBT | Elza | Elza: Okay, I'll go for her. | - | MOD-TEXT | Neu: Elza sucht Sherry im Kanal (Mod lässt Sherry in 11B1 #10/#11 verschwinden) |
| ROOM2001 | 3 | LEBT | John | John: Meanwhile I will take a look / around, maybe she is down here too. | - | MOD-TEXT | Neu |
| ROOM2001 | 4 | LEBT | Elza | Elza: Good idea, but be careful. | - | MOD-TEXT | Neu |
| ROOM2001 | 5 | LEBT | John | John: You too. | - | MOD-TEXT | Neu |
| ROOM2001 | 6 | LEBT | Elza | Elza: Sherry... thank god you're / fine! I was so worried! | - | MOD-TEXT | Ort Elza + Sherry in 200 belegt durch Spätstand-Foto #036/#037 (Kamera 200 Cut 11), Wortlaut nicht |
| ROOM2001 | 7 | LEBT | Elza | Elza: What happened? why didn't you / wait for me? | - | MOD-TEXT | Rückbezug auf D11B #8 („Warte brav im Wagen"). Dass Sherry wegläuft, steht nicht auf der Disc |
| ROOM2001 | 8 | LEBT | Sherry | Sherry: It was because of my daddy, / I've heard him call me! | - | MOD-TEXT | Neu: Sherry hört ihren Vater rufen. Disc nennt den Vater erst in 309 (D309 #19) und 400 (D400 #13) |
| ROOM2001 | 9 | LEBT | Sherry | Sherry: I went down here to find him, / but I couldn't... | - | MOD-TEXT | Neu |
| ROOM2001 | 10 | LEBT | Elza | Elza: I understand, don't worry honey, / if he's here we'll find him, okay? | - | MOD-TEXT | Neu |
| ROOM2001 | 11 | LEBT | Elza | Elza: My, that's a nice pocket watch! | - | MOD-TEXT | Spätstand-Foto #037 (200 Cut 11): 「…まあ 大きな時計ね！」 = „Oh, was für eine große Uhr!", nicht auf der Disc. Mod-Wortlaut frei („nice pocket watch") |
| ROOM2001 | 12 | LEBT | Sherry | Sherry: My mom gave it to me, she says / it's for good luck. | - | MOD-TEXT | Neu: Uhr von der Mutter, Glücksbringer. Weder Disc noch Foto |
| ROOM2001 | 13 | LEBT | Elza | Elza: I see, I guess your mom is right. | - | MOD-TEXT | Neu |
| ROOM2001 | 14 | LEBT | Elza | Elza: Come on Sherry, let's look for / John and get out of here! | - | MOD-TEXT | Neu |
| ROOM2010 | 0 | LEBT | System/Untersuchen | Will you take the Crank?  | - | MOD-TEXT | Muster wie D119 #1 („Schraubendreher. Nehmen?"), Item Kurbel = Mod (README Z. 283) |
| ROOM2010 | 1 | LEBT | System/Untersuchen | You've taken the Crank. | - | MOD-TEXT | - |
| ROOM2010 | 2 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Preview-Hinweis der Mod |
| ROOM2010 | 3 | LEBT | System/Untersuchen | It's a computer.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Preview-Hinweis der Mod. Raum 201 = Mod-Speicherraum (README Z. 61) |
| ROOM2010 | 4 | LEBT | System/Untersuchen | An old typewriter.Why use it if there are / computers? | - | MOD-TEXT | - |
| ROOM2011 | 0 | LEBT | System/Untersuchen | Will you take the Crank?  | - | MOD-TEXT | Muster wie D119 #1 („Schraubendreher. Nehmen?"), Item Kurbel = Mod (README Z. 283) |
| ROOM2011 | 1 | LEBT | System/Untersuchen | You've taken the Crank. | - | MOD-TEXT | - |
| ROOM2011 | 2 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Preview-Hinweis der Mod |
| ROOM2011 | 3 | LEBT | System/Untersuchen | It's a computer.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Preview-Hinweis der Mod. Raum 201 = Mod-Speicherraum (README Z. 61) |
| ROOM2011 | 4 | LEBT | System/Untersuchen | An old typewriter.Why use it if there are / computers? | - | MOD-TEXT | - |
| ROOM2020 | 0 | LEBT | System/Untersuchen | It's a square hole. | - | MOD-TEXT | Raum 202 nur Mod |
| ROOM2020 | 1 | LEBT | System/Untersuchen | Will you use the Crank?  | - | MOD-TEXT | Muster wie D400 #1 („ID-Karte benutzen?"), anderes Item |
| ROOM2020 | 2 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 | SINNGEMAESS | Standardsatz, Stelle nur Mod. Disc: „Mit der ID-Karte entriegelt", hier ohne Karte |
| ROOM2020 | 3 | LEBT | System/Untersuchen | It's locked. | D306 #0 | WOERTLICH | Standardsatz, Stelle nur Mod |
| ROOM2021 | 0 | LEBT | System/Untersuchen | It's a square hole. | - | MOD-TEXT | Raum 202 nur Mod |
| ROOM2021 | 1 | LEBT | System/Untersuchen | Will you use the Crank?  | - | MOD-TEXT | Muster wie D400 #1 („ID-Karte benutzen?"), anderes Item |
| ROOM2021 | 2 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 | SINNGEMAESS | Standardsatz, Stelle nur Mod. Disc: „Mit der ID-Karte entriegelt", hier ohne Karte |
| ROOM2021 | 3 | LEBT | System/Untersuchen | It's locked. | D306 #0 | WOERTLICH | Standardsatz, Stelle nur Mod |
| ROOM2030 | 0 | LEBT | System/Untersuchen | It's locked from the other side. | - | MOD-TEXT | Standardsatz der Mod. Disc 203 ohne Text, Disc kennt nur „Abgeschlossen" (D306 #0) |
| ROOM2030 | 1 | LEBT | System/Untersuchen | The elevator is not responding...Maybe it should be activated / from somewhere. | - | MOD-TEXT | Vgl. D307 #15 (Aufzug ohne Strom), anderer Raum und Wortlaut |
| ROOM2030 | 2 | LEBT | System/Untersuchen | Will you take the Fuse?  | - | MOD-TEXT | Muster wie D119 #1, Item Mod |
| ROOM2030 | 3 | LEBT | System/Untersuchen | You've taken the Fuse. | - | MOD-TEXT | - |
| ROOM2031 | 0 | LEBT | System/Untersuchen | It's locked from the other side. | - | MOD-TEXT | Standardsatz der Mod. Disc 203 ohne Text, Disc kennt nur „Abgeschlossen" (D306 #0) |
| ROOM2031 | 1 | LEBT | System/Untersuchen | The elevator is not responding...Maybe it should be activated / from somewhere. | - | MOD-TEXT | Vgl. D307 #15 (Aufzug ohne Strom), anderer Raum und Wortlaut |
| ROOM2031 | 2 | LEBT | System/Untersuchen | Will you take the Fuse?  | - | MOD-TEXT | Muster wie D119 #1, Item Mod |
| ROOM2031 | 3 | LEBT | System/Untersuchen | You've taken the Fuse. | - | MOD-TEXT | - |
| ROOM2040 | 0 | LEBT | System/Untersuchen | Notice of damage door:While doing maintenance and / construction work on the Raccoonsewage system we found a problem / with a door, it seems that itrusted due to water leaks, we tried / everything and nothing works!Unless we use explosives or something / heavy it doesn't seem I'm going togive in, I'll have to ask the manager / for options for this problem.Bill Johnson, / Knights Construction | - | MOD-TEXT | Neue Namen „Bill Johnson", „Knights Construction". Raum 204 nur Mod |
| ROOM2040 | 1 | LEBT | System/Untersuchen | An old typewriter, perhaps someone will / find it useful even in these times. | - | MOD-TEXT | - |
| ROOM2040 | 2 | LEBT | System/Untersuchen | This old console is useless. | - | MOD-TEXT | - |
| ROOM2040 | 3 | LEBT | System/Untersuchen | The body of a sewer worker... / He has something in his hands. | - | MOD-TEXT | - |
| ROOM2040 | 4 | LEBT | (ohne Praefix, vermutlich Spielfigur) | Bomb activated, I have 10 seconds / before it detonates! | - | MOD-TEXT | - |
| ROOM2040 | 5 | LEBT | System/Untersuchen | Will you use the Time Bomb?    e         /            4   2  /           /      3  x  x                     | - | MOD-TEXT | Text enthält Füll-/Steuerbytes |
| ROOM2041 | 0 | LEBT | System/Untersuchen | Notice of damage door:While doing maintenance and / construction work on the Raccoonsewage system we found a problem / with a door, it seems that itrusted due to water leaks, we tried / everything and nothing works!Unless we use explosives or something / heavy it doesn't seem I'm going togive in, I'll have to ask the manager / for options for this problem.Bill Johnson, / Knights Construction | - | MOD-TEXT | Neue Namen „Bill Johnson", „Knights Construction". Raum 204 nur Mod |
| ROOM2041 | 1 | LEBT | System/Untersuchen | An old typewriter, perhaps someone will / find it useful even in these times. | - | MOD-TEXT | - |
| ROOM2041 | 2 | LEBT | System/Untersuchen | This old console is useless. | - | MOD-TEXT | - |
| ROOM2041 | 3 | LEBT | System/Untersuchen | The body of a sewer worker... / He has something in his hands. | - | MOD-TEXT | - |
| ROOM2041 | 4 | LEBT | (ohne Praefix, vermutlich Spielfigur) | Bomb activated, I have 10 seconds / before it detonates! | - | MOD-TEXT | - |
| ROOM2041 | 5 | LEBT | System/Untersuchen | Will you use the Time Bomb?    e         /            4   2  /           /      3  x  x                     | - | MOD-TEXT | Text enthält Füll-/Steuerbytes |
| ROOM2050 | 0 | LEBT | System/Untersuchen | A lever to fill and drain water. / Will you move it?  | - | MOD-TEXT | Raum 205 nur Mod |
| ROOM2050 | 1 | LEBT | System/Untersuchen | The water is blocking the / entrance. | - | MOD-TEXT | - |
| ROOM2050 | 2 | LEBT | System/Untersuchen | I have nothing else to do / here. | - | MOD-TEXT | - |
| ROOM2051 | 0 | LEBT | System/Untersuchen | A lever to fill and drain water. / Will you move it?  | - | MOD-TEXT | Raum 205 nur Mod |
| ROOM2051 | 1 | LEBT | System/Untersuchen | The water is blocking the / entrance. | - | MOD-TEXT | - |
| ROOM2051 | 2 | LEBT | System/Untersuchen | I have nothing else to do / here. | - | MOD-TEXT | - |
| ROOM2060 | 0 | LEBT | System/Untersuchen | The generator does not appear to / be on.There are four switches...Will you push them?  | - | MOD-TEXT | Raum 206 nur Mod (Generator-Mechanik, README Z. 82) |
| ROOM2060 | 1 | LEBT | System/Untersuchen | I need to insert the missing / fuse before I can operate this. | - | MOD-TEXT | - |
| ROOM2060 | 2 | LEBT | System/Untersuchen | It seems that the generator is / working now. | - | MOD-TEXT | - |
| ROOM2060 | 3 | LEBT | System/Untersuchen | One of the fuses is missing... | - | MOD-TEXT | - |
| ROOM2060 | 4 | LEBT | System/Untersuchen | Will you use the Fuse?  | - | MOD-TEXT | - |
| ROOM2060 | 5 | LEBT | System/Untersuchen | You've used the Fuse. | - | MOD-TEXT | - |
| ROOM2060 | 6 | LEBT | System/Untersuchen | The fuse is in place. | - | MOD-TEXT | - |
| ROOM2061 | 0 | LEBT | System/Untersuchen | The generator does not appear to / be on.There are four switches...Will you push them?  | - | MOD-TEXT | Raum 206 nur Mod (Generator-Mechanik, README Z. 82) |
| ROOM2061 | 1 | LEBT | System/Untersuchen | I need to insert the missing / fuse before I can operate this. | - | MOD-TEXT | - |
| ROOM2061 | 2 | LEBT | System/Untersuchen | It seems that the generator is / working now. | - | MOD-TEXT | - |
| ROOM2061 | 3 | LEBT | System/Untersuchen | One of the fuses is missing... | - | MOD-TEXT | - |
| ROOM2061 | 4 | LEBT | System/Untersuchen | Will you use the Fuse?  | - | MOD-TEXT | - |
| ROOM2061 | 5 | LEBT | System/Untersuchen | You've used the Fuse. | - | MOD-TEXT | - |
| ROOM2061 | 6 | LEBT | System/Untersuchen | The fuse is in place. | - | MOD-TEXT | - |
| ROOM2070 | 0 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 | SINNGEMAESS | Standardsatz, Stelle nur Mod. Steht in 207 auf Id 0, wo die Disc D207 #0 (Rolltor-Schalter) hat. Die Disc-Zeile wanderte samt sub05/Ck(5,0)=0 nach 20A0 #0 |
| ROOM2071 | 0 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 | SINNGEMAESS | Standardsatz, Stelle nur Mod. Die Datei 2071 gibt es auf der Disc nicht (2070 ist Einzeldatei) |
| ROOM2080 | 0 | LEBT | System/Untersuchen | From here maybe I can control the / water level.Will you operate the computer?  | - | MOD-TEXT | Raum 208 nur Mod |
| ROOM2080 | 1 | LEBT | System/Untersuchen | The water level is very high...I can't get down to the floor below. | - | MOD-TEXT | - |
| ROOM2081 | 0 | LEBT | System/Untersuchen | From here maybe I can control the / water level.Will you operate the computer?  | - | MOD-TEXT | Raum 208 nur Mod |
| ROOM2081 | 1 | LEBT | System/Untersuchen | The water level is very high...I can't get down to the floor below. | - | MOD-TEXT | - |
| ROOM20A0 | 0 | LEBT | System/Untersuchen | There's a shutter switch here. Press it?  | D207 #0 | WOERTLICH | Aus 207 verschoben: gleiches sub05, gleiche Bedingung Ck(5,0)=0 wie Disc 2070. Raum 20A hat keine Disc-Datei (EXE nennt 20A ohne Datei) |
| ROOM20A0 | 1 | LEBT | System/Untersuchen | It's locked. | D306 #0 | WOERTLICH | Standardsatz, Stelle nur Mod |
| ROOM20A0 | 2 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 | SINNGEMAESS | Standardsatz, Stelle nur Mod, ohne Karte |
| ROOM20A1 | 0 | LEBT | System/Untersuchen | There's a shutter switch here. Press it?  | D207 #0 | WOERTLICH | Wie 20A0 #0. Auf der Disc nur Leon (2070 Einzeldatei, Raum ohne Tür zu ihm) |
| ROOM20A1 | 1 | LEBT | System/Untersuchen | It's locked. | D306 #0 | WOERTLICH | Standardsatz, Stelle nur Mod |
| ROOM20A1 | 2 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 | SINNGEMAESS | Standardsatz, Stelle nur Mod, ohne Karte |
| ROOM3000 | 0 | LEBT | System/Untersuchen | A ladder leads down the manhole. / Descend it?  | D300 #0 | WOERTLICH | - |
| ROOM3000 | 1 | LEBT | (ohne Praefix, vermutlich Spielfigur Marvin) | My strength is running out. / I've got to find the others! | D300 #1 | WOERTLICH | Disc tot (Spielfigur 13 nie gesetzt). Mod: Marvin in der Fabrik spielbar (README Z. 23) |
| ROOM3000 | 2 | LEBT | Leon | Leon: Marvin! Aren't these... RPD / officers!? | D300 #2 | WOERTLICH | Disc-NPC in 3000 ist Typ 0x49 (John-Modell, Platzhalter), der Text nennt Marvin |
| ROOM3000 | 3 | LEBT | Marvin | Marvin: Oh, man... They must be from the / squad investigating on the Umbrella / incident. | D300 #3 | WOERTLICH | Sprecher in 300 nur aus Inhalt (keine Regie). Spätstand-Foto #118 hat anderen Wortlaut (みんな ラクー…) |
| ROOM3000 | 4 | LEBT | Marvin | Marvin: We don't know what's up ahead... / Stay alert, Leon! | D300 #4 | SINNGEMAESS | „stillgelegte Fabrik" (廃工場) weggelassen |
| ROOM3000 | 5 | LEBT | Leon | Leon: Marvin, what about that woman? | D300 #5 | WOERTLICH | - |
| ROOM3000 | 6 | LEBT | Marvin | Marvin: Ada...? She went on ahead. | D300 #6 | WOERTLICH | - |
| ROOM3000 | 7 | LEBT | Marvin | Marvin: She didn't seem to be at all / disturbed by all this massacre. | D300 #7 | WOERTLICH | - |
| ROOM3000 | 8 | LEBT | Leon | Leon: There's a ton of things I need / to ask her the next time I see her... | D300 #8 | WOERTLICH | - |
| ROOM3001 | 0 | LEBT | System/Untersuchen | A ladder leads down the manhole. / Descend it?  | D300 #0 | WOERTLICH | - |
| ROOM3001 | 1 | LEBT | (ohne Praefix, vermutlich Spielfigur Marvin) | My strength is running out. / I've got to find the others! | D300 #1 | WOERTLICH | Marvin-Zeile auch im Elza-Block wie auf der Disc. Disc tot |
| ROOM3001 | 2 | LEBT | Elza | Elza: !! | D300 #9 | WOERTLICH | Disc ohne Sprecher, Mod setzt Elza. Ids ab hier um 7 verschoben (Mod trennt die Blöcke je Szenario) |
| ROOM3001 | 3 | LEBT | Elza | Elza: Wh-what is this place...? | D300 #10 | WOERTLICH | - |
| ROOM3001 | 4 | LEBT | John | John: These corpses are definitively / police officers. | D300 #11 | SINNGEMAESS | „Wie du siehst" (見ての通り) → „definitively" |
| ROOM3001 | 5 | LEBT | John | John: This used to be a chemical plant, / but it was supposedly not in use anymore... | D300 #12 | WOERTLICH | Spätstand-Foto #048 stützt (ない statt ねぇ) |
| ROOM3001 | 6 | LEBT | Elza | Elza: What happened here? | D300 #13 | ABGEWANDELT | Sprecherwechsel John → Elza (Disc-Register んだ männlich, Zuordnung ohne Regie) |
| ROOM3001 | 7 | LEBT | Sherry | Sherry: Elza... | D300 #14 | WOERTLICH | - |
| ROOM3001 | 8 | LEBT | Elza | Elza: It'll be okay, just stick close to / me. | D300 #15 | WOERTLICH | - |
| ROOM3001 | 9 | LEBT | John | John: Let's go. We should find / somewhere to hide for now. | D300 #16 | WOERTLICH | - |
| ROOM3010 | 0 | LEBT | System/Untersuchen | It's locked.An ID card is required to open it. | D301 #0 | WOERTLICH | „ID card" statt nur „Karte" (Karte = ID-Karte laut #1). Disc: nicht ausgelöst, Port ruft es (main00 AOT6) |
| ROOM3010 | 1 | LEBT | System/Untersuchen | You've used the Green Keycard. | D301 #1 | ABGEWANDELT | Disc: „Mit der ID-Karte entriegelt". Mod: „Green Keycard", das Entriegeln fehlt |
| ROOM3011 | 0 | LEBT | System/Untersuchen | It's locked.An ID card is required to open it. | D301 #0 | WOERTLICH | „ID card" statt nur „Karte" (Karte = ID-Karte laut #1). Disc: nicht ausgelöst, Port ruft es (main00 AOT6) |
| ROOM3011 | 1 | LEBT | System/Untersuchen | You've used the Green Keycard. | D301 #1 | ABGEWANDELT | Disc: „Mit der ID-Karte entriegelt". Mod: „Green Keycard", das Entriegeln fehlt |
| ROOM3020 | 0 | LEBT | Leon | Leon: Dammit! the door does not open! | - | MOD-TEXT | Disc 302 ohne Text und ohne NPC. Mod-Szene Leon + Marvin vor verschlossener Tür |
| ROOM3020 | 1 | LEBT | Marvin | Marvin: There's an open window, maybe / we could enter through there? | - | MOD-TEXT | Neu: Einstieg durchs Fenster |
| ROOM3020 | 2 | LEBT | Marvin | Marvin: Leon give me an hand! | - | MOD-TEXT | Wortlaut wie D11B #18 („Leon, hilf mit!", dort Kanaldeckel, im Mod 11B0 #9). Hier neue Fenster-Szene, keine verschobene Disc-Zeile |
| ROOM3020 | 3 | kein | System/Untersuchen | It's locked from the other side. | - | MOD-TEXT | Standardsatz der Mod |
| ROOM3020 | 4 | LEBT | System/Untersuchen | The car is locked, I can't get inside. | - | MOD-TEXT | - |
| ROOM3020 | 5 | LEBT | System/Untersuchen | The panel is smashed and doesn't work. | - | MOD-TEXT | - |
| ROOM3020 | 6 | LEBT | System/Untersuchen |  / It's locked. | D306 #0 | WOERTLICH | Standardsatz, Stelle nur Mod |
| ROOM3021 | 0 | kein | System/Untersuchen | It's locked from the other side. | - | MOD-TEXT | Standardsatz der Mod |
| ROOM3021 | 1 | LEBT | System/Untersuchen | The car is locked, I can't get inside. | - | MOD-TEXT | - |
| ROOM3021 | 2 | LEBT | System/Untersuchen | It's an open window. / I could boost Sherry in there.Not a good idea.Too dark, and there may be zombies / inside. | - | MOD-TEXT | Neu: Elza erwägt, Sherry durchs Fenster zu heben. Disc: Sherrys Weg ist das Gitter in 306 (D306 #3–#7) |
| ROOM3021 | 3 | LEBT | System/Untersuchen | The panel is smashed and doesn't work. | - | MOD-TEXT | - |
| ROOM3021 | 4 | LEBT | System/Untersuchen |  / It's locked. | D306 #0 | WOERTLICH | Standardsatz, Stelle nur Mod |
| ROOM3040 | 0 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 | SINNGEMAESS | Standardsatz, Stelle nur Mod, ohne Karte. Disc 304 ohne Text |
| ROOM3040 | 1 | LEBT | System/Untersuchen | There's a button here. Push it?  | D104 #0 | WOERTLICH | Standardsatz, Stelle nur Mod |
| ROOM3040 | 2 | LEBT | System/Untersuchen | The boxes are blocking the way. / I must find a way to move them. | - | MOD-TEXT | Mod-Rätsel (README Z. 185) |
| ROOM3040 | 3 | LEBT | System/Untersuchen | The control panel is not / responding...Maybe it should be activated / from somewhere. | - | MOD-TEXT | - |
| ROOM3041 | 0 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 | SINNGEMAESS | Standardsatz, Stelle nur Mod, ohne Karte. Disc 304 ohne Text |
| ROOM3041 | 1 | LEBT | System/Untersuchen | There's a button here. Push it?  | D104 #0 | WOERTLICH | Standardsatz, Stelle nur Mod |
| ROOM3041 | 2 | LEBT | System/Untersuchen | The boxes are blocking the way. / I must find a way to move them. | - | MOD-TEXT | Mod-Rätsel (README Z. 185) |
| ROOM3041 | 3 | LEBT | System/Untersuchen | The control panel is not / responding...Maybe it should be activated / from somewhere. | - | MOD-TEXT | - |
| ROOM3050 | 0 | LEBT | System/Untersuchen | There's a button here. Push it?  | D104 #0 | WOERTLICH | Standardsatz, Stelle nur Mod. Raum 305 nur Mod |
| ROOM3050 | 1 | kein | System/Untersuchen | The control panel is not / responding...Maybe it should be activated / from somewhere. | - | MOD-TEXT | - |
| ROOM3050 | 2 | kein | System/Untersuchen | The elevator is not responding...Maybe it should be activated / from somewhere. | - | MOD-TEXT | Vgl. D307 #15, anderer Raum und Wortlaut |
| ROOM3050 | 3 | LEBT | System/Untersuchen | The computer seems to be working. / There is a note:In case the machinery stops / working, it will be necessary touse the Red Master Keycard to / restore the system." | - | MOD-TEXT | Mod-Rätsel Super Redhawk (README Z. 257) |
| ROOM3050 | 4 | LEBT | System/Untersuchen | Will you use the Red Master Keycard?  | - | MOD-TEXT | - |
| ROOM3050 | 5 | LEBT | System/Untersuchen | You've used the Red Master Keycard. | - | MOD-TEXT | - |
| ROOM3050 | 6 | LEBT | System/Untersuchen | The system has been successfully / restored. | - | MOD-TEXT | - |
| ROOM3050 | 7 | LEBT | System/Untersuchen | I don't need to use this anymore. | - | MOD-TEXT | - |
| ROOM3051 | 0 | LEBT | System/Untersuchen | There's a button here. Push it?  | D104 #0 | WOERTLICH | Standardsatz, Stelle nur Mod. Raum 305 nur Mod |
| ROOM3051 | 1 | kein | System/Untersuchen | The control panel is not / responding...Maybe it should be activated / from somewhere. | - | MOD-TEXT | - |
| ROOM3051 | 2 | kein | System/Untersuchen | The elevator is not responding...Maybe it should be activated / from somewhere. | - | MOD-TEXT | Vgl. D307 #15, anderer Raum und Wortlaut |
| ROOM3051 | 3 | LEBT | System/Untersuchen | The computer seems to be working. / There is a note:In case the machinery stops / working, it will be necessary touse the Red Master Keycard to / restore the system." | - | MOD-TEXT | Mod-Rätsel Super Redhawk (README Z. 257) |
| ROOM3051 | 4 | LEBT | System/Untersuchen | Will you use the Red Master Keycard?  | - | MOD-TEXT | - |
| ROOM3051 | 5 | LEBT | System/Untersuchen | You've used the Red Master Keycard. | - | MOD-TEXT | - |
| ROOM3051 | 6 | LEBT | System/Untersuchen | The system has been successfully / restored. | - | MOD-TEXT | - |
| ROOM3051 | 7 | LEBT | System/Untersuchen | I don't need to use this anymore. | - | MOD-TEXT | - |
| ROOM3060 | 0 | LEBT | System/Untersuchen |  / It's locked. | D306 #0 | WOERTLICH | - |
| ROOM3060 | 1 | kein | Elza | Elza: Sherry, come back! | D306 #1 | SINNGEMAESS | Elza-Szene im Leon-Block, auf der Disc ebenso ohne Aufruf. „come back" ergänzt, kein neuer Fakt |
| ROOM3060 | 2 | kein | Elza | Elza: What's wrong? Why did you start / running...? | D306 #2 | WOERTLICH | Elza-Szene im Leon-Block, auf der Disc ebenso ohne Aufruf. |
| ROOM3060 | 3 | kein | Sherry | Sherry: Elza... I know a secret / way in! | D306 #3 | WOERTLICH | Elza-Szene im Leon-Block, auf der Disc ebenso ohne Aufruf. Spätstand-Foto #049 und Film B 3:19 gleich |
| ROOM3060 | 4 | kein | Elza | Elza: A secret entrance? | D306 #4 | WOERTLICH | Elza-Szene im Leon-Block, auf der Disc ebenso ohne Aufruf. |
| ROOM3060 | 5 | kein | Sherry | Sherry: It's here, I can get inside / through this hole! | D306 #5 | WOERTLICH | Elza-Szene im Leon-Block, auf der Disc ebenso ohne Aufruf. „this hole" = das Lüftungsgitter |
| ROOM3060 | 6 | kein | Elza | Elza: Sherry, wait! | D306 #6 | WOERTLICH | Elza-Szene im Leon-Block, auf der Disc ebenso ohne Aufruf. |
| ROOM3060 | 7 | kein | Sherry | Sherry: I'll unlock the back door from / the other side!Just wait by the door, okay? | D306 #7 | WOERTLICH | Elza-Szene im Leon-Block, auf der Disc ebenso ohne Aufruf. |
| ROOM3060 | 8 | kein | John | John: H-hey, you brat! That's dangerous, / get back here! | D306 #8 | WOERTLICH | Elza-Szene im Leon-Block, auf der Disc ebenso ohne Aufruf. |
| ROOM3060 | 9 | LEBT | System/Untersuchen | For some reason the door isn't / locked anymore... | D306 #9 | SINNGEMAESS | „For some reason" statt „irgendwann" (いつの間にか). Der Zeitbezug, auf den BERICHT §4 die Reihenfolge Elza vor Leon stützt, geht verloren |
| ROOM3060 | 10 | LEBT | System/Untersuchen | It's locked from the other side. | D306 #0 | ABGEWANDELT | Gleiche AOT 2 (Disc 3060 sub00 Aot_reset2 → #0 „Abgeschlossen"). Mod-Zusatz „from the other side", Bedingung Ck(4,196)=0 |
| ROOM3061 | 0 | LEBT | System/Untersuchen |  / It's locked. | D306 #0 | WOERTLICH | - |
| ROOM3061 | 1 | LEBT | Elza | Elza: Sherry, come back! | D306 #1 | SINNGEMAESS | „come back" ergänzt, kein neuer Fakt |
| ROOM3061 | 2 | LEBT | Elza | Elza: What's wrong? Why did you start / running...? | D306 #2 | WOERTLICH | - |
| ROOM3061 | 3 | LEBT | Sherry | Sherry: Elza... I know a secret / way in! | D306 #3 | WOERTLICH | Spätstand-Foto #049 und Film B 3:19 gleich |
| ROOM3061 | 4 | LEBT | Elza | Elza: A secret entrance? | D306 #4 | WOERTLICH | - |
| ROOM3061 | 5 | LEBT | Sherry | Sherry: It's here, I can get inside / through this hole! | D306 #5 | WOERTLICH | „this hole" = das Lüftungsgitter |
| ROOM3061 | 6 | LEBT | Elza | Elza: Sherry, wait! | D306 #6 | WOERTLICH | - |
| ROOM3061 | 7 | LEBT | Sherry | Sherry: I'll unlock the back door from / the other side! | D306 #7 (1. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu: D306 #7 auf #7 + #8 verteilt. Mod/Port: danach ist Sherry Spielfigur (ROOM3061 @0x011F6, re15_sherry_r41.h) |
| ROOM3061 | 8 | LEBT | Sherry | Sherry: Just wait by the door, okay? | D306 #7 (2. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM3061 | 9 | LEBT | John | John: H-hey, you brat! That's dangerous, / get back here! | D306 #8 | WOERTLICH | Id um 1 verschoben |
| ROOM3061 | 10 | LEBT | System/Untersuchen | It's locked from the other side. | D306 #0 | ABGEWANDELT | Wie 3060 #10 (Disc 3061 sub00 Aot_reset2 → #0) |
| ROOM3070 | 0 | LEBT | Leon | Leon: Get on the lift! It's our only / option... | D307 #0 | ABGEWANDELT | Sprecherwechsel Ada → Leon. Auf der Disc ordnet Ada den Aufzug an |
| ROOM3070 | 1 | LEBT | Marvin | Marvin: This thing...? / But where will it take us? | D307 #1 | ABGEWANDELT | Sprecherwechsel Leon → Marvin |
| ROOM3070 | 2 | LEBT | Ada | Ada: Underground... Though we might / be better off staying here and get / eaten by those things... | D307 #2 | ABGEWANDELT | „Underground" ist Fehllesung: Disc 地仲さ, 地下 scheidet aus (下 liegt im Font), wohl 地獄 „In die Hölle". Rest wörtlich |
| ROOM3070 | 3 | LEBT | Marvin | Marvin: What? | D307 #3 | ABGEWANDELT | Sprecherwechsel Leon → Marvin |
| ROOM3070 | 4 | LEBT | Leon | Leon: Hurry up! They're coming! | D307 #4 | ABGEWANDELT | Sprecher Disc Marvin oder Ada (ohne Regie), Mod Leon |
| ROOM3070 | 5 | LEBT | System/Untersuchen | The power seems to be off... / If the control box worked,the elevator might move, but... | D307 #15 | WOERTLICH | - |
| ROOM3071 | 0 | LEBT | Elza | Elza: I know it has to be somewhere...! | D307 #5 | SINNGEMAESS | „Leise!" (静かにね) weggelassen |
| ROOM3071 | 1 | LEBT | John | John: Okay, Elza! I've got your back! | D307 #6 | WOERTLICH | - |
| ROOM3071 | 2 | LEBT | Elza | Elza: This lift, this is where it / came from. | D307 #7 | WOERTLICH | - |
| ROOM3071 | 3 | LEBT | John | John: ...! That's a damn huge lift... | D307 #8 (1. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu: D307 #8 auf #3–#5 verteilt |
| ROOM3071 | 4 | LEBT | John | John: They probably used it to move / some heavy equipment. | D307 #8 (2. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM3071 | 5 | LEBT | John | John: ...Can we get it to work? | D307 #8 (3. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM3071 | 6 | LEBT | Elza | Elza: No... It looks like it can be / operated only from the inside... | D307 #9 | ABGEWANDELT | Disc: „für Außenstehende unmöglich" (外部の人間). Mod: „nur von innen bedienbar". Der Hinweis, dass nur Insider den Aufzug bedienen (und Sherry es kann), fällt weg |
| ROOM3071 | 7 | LEBT | Sherry | Sherry: I know how to make it work! | D307 #10 | WOERTLICH | - |
| ROOM3071 | 8 | LEBT | Elza | Elza: Sherry? What are you talking / about...? | D307 #11 | SINNGEMAESS | Disc fragt nach Sherry selbst („Wer bist du eigentlich…"), Mod nach ihrer Aussage |
| ROOM3071 | 9 | LEBT | John | John: Save the discussion for later! / Those things are trying to get in here...! | D307 #12 (1.+2. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM3071 | 10 | LEBT | John | John: Let's get this lift moving and go / down there! | D307 #12 (3. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM3071 | 11 | LEBT | Elza | Elza: Are you insane!? | D307 #13 (1. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu, „Are you insane" frei |
| ROOM3071 | 12 | LEBT | Elza | Elza: I told you that weird monster / came from here! | D307 #13 (2. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu, „I told you" ergänzt |
| ROOM3071 | 13 | LEBT | John | John: If we stay here, we'll be eaten / for sure! | D307 #14 (1. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM3071 | 14 | LEBT | John | John: We've got no choice but to take / our chances down there! Let's go! | D307 #14 (2.+3. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu, „take our chances down there" ergänzt |
| ROOM3071 | 15 | LEBT | System/Untersuchen | The power seems to be off... / If the control box worked,the elevator might move, but... | D307 #15 | WOERTLICH | - |
| ROOM3080 | 0 | LEBT | Ada | Ada: You just don't give up... | D308 #0 | ABGEWANDELT | Sprecher: Disc Marvin oder Leon (Ada sagt auf der Disc nie 〜ぜ), Mod Ada. Disc redet über „den Kerl", Mod redet ihn an |
| ROOM3080 | 1 | LEBT | Ada | Ada: That's... got to be... | D308 #1 | WOERTLICH | Sprecher Ada passt (Disc „eher Ada") |
| ROOM3090 | 0 | LEBT | System/Untersuchen |  / The power is already on... | D309 #0 | WOERTLICH | Leons Nachzustand „Licht an" wie auf der Disc |
| ROOM3090 | 1 | LEBT | System/Untersuchen |  / This breaker switch is flipped down... | D309 #2 | WOERTLICH | Mod kürzt den Leon-Block auf 4 Zeilen, Ids verschoben |
| ROOM3090 | 2 | LEBT | System/Untersuchen |  / This breaker switch is flipped down... | D309 #4 | WOERTLICH | - |
| ROOM3090 | 3 | LEBT | System/Untersuchen | The door is latched shut and won't open. | D309 #7 | WOERTLICH | Disc: nicht ausgelöst. Port ruft sie in 3090 (sub00 Aot_reset1, Ck(4,8)=0) |
| ROOM3091 | 0 | LEBT | System/Untersuchen |  / The power is already on... | D309 #0 | WOERTLICH | - |
| ROOM3091 | 1 | LEBT | System/Untersuchen | The power is off... Has a circuit breaker / tripped? | D309 #1 | WOERTLICH | Sherry-Abschnitt (Mod/Port: Sherry Spielfigur) |
| ROOM3091 | 2 | LEBT | System/Untersuchen |  / This breaker switch is flipped down... | D309 #2 | WOERTLICH | - |
| ROOM3091 | 3 | LEBT | System/Untersuchen |  / I can't reach the breaker switch... | D309 #3 | WOERTLICH | Passt zu Sherry als Figur |
| ROOM3091 | 4 | LEBT | System/Untersuchen |  / This breaker switch is flipped down... | D309 #4 | WOERTLICH | - |
| ROOM3091 | 5 | LEBT | System/Untersuchen | This breaker switch is flipped up... Flip / it back down?  | D309 #5 | WOERTLICH | - |
| ROOM3091 | 6 | LEBT | System/Untersuchen | The door is latched shut and won't open.I'd need a key to open this... | D309 #6 | WOERTLICH | - |
| ROOM3091 | 7 | kein | System/Untersuchen | The door is latched shut and won't open. | D309 #7 | WOERTLICH | Ohne Aufruf, wie auf der Disc (nicht ausgelöst) |
| ROOM3091 | 8 | LEBT | (ohne Praefix, vermutlich Spielfigur Sherry) |  / I need to hurry up and save the others... | D309 #8 | SINNGEMAESS | Name エルザ weggelassen („Elza und die anderen" → „the others"). Auf der Disc ist das der Beleg für den Sherry-Abschnitt. Mod/Port: Sherry ist hier Spielfigur (README Z. 84, ROOM3061 @0x011F6), die Aussage bleibt |
| ROOM3091 | 9 | LEBT | John | John: That was a close shave! / I almost got my ass handed to me! | D309 #9 (1.+2. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu. Foto #059 stützt den Disc-Wortlaut |
| ROOM3091 | 10 | LEBT | John | John: Elza, are you okay? | D309 #9 (3. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM3091 | 11 | LEBT | Elza | Elza: Yes, somehow! I thought I was / done for! | D309 #10 | WOERTLICH | - |
| ROOM3091 | 12 | LEBT | John | John: Hey, you really saved my butt / there! | D309 #11 | SINNGEMAESS | Anrede „Kleine" (お嬢ちゃん) fehlt, „das vergesse ich dir nicht" (恩にきる) frei umformuliert |
| ROOM3091 | 13 | LEBT | Elza | Elza: You really did save us, / thank you! | D309 #12 | WOERTLICH | - |
| ROOM3091 | 14 | LEBT | John | John: What's your name, kid? | D309 #13 | WOERTLICH | - |
| ROOM3091 | 15 | LEBT | Sherry | Sherry: It's Sherry... | D309 #14 | WOERTLICH | - |
| ROOM3091 | 16 | LEBT | John | John: Sherry, huh? I'm John, and you / can rest assured that I'll keep you / safe from now on! | D309 #15 | WOERTLICH | - |
| ROOM3091 | 17 | LEBT | Elza | Elza: You seem to have taken a / liking to her. | D309 #16 | WOERTLICH | - |
| ROOM3091 | 18 | LEBT | John | John: Well, of course, she saved my / life, you know? That's why I'll protect / her with my life! | D309 #17 | WOERTLICH | - |
| ROOM3091 | 19 | LEBT | Elza | Elza: How simplistic... Anyway, Sherry, / how did you know about that entrance? | D309 #18 | WOERTLICH | - |
| ROOM3091 | 20 | LEBT | Sherry | Sherry: This is... where my dad used to / be... | D309 #19 | SINNGEMAESS | „used to be" statt „gearbeitet" (Lücke □いていた = 働いていた), „…Deshalb…" fehlt |
| ROOM3091 | 21 | LEBT | Elza | Elza: Your... dad...? | D309 #20 | WOERTLICH | - |
| ROOM3091 | 22 | LEBT | Sherry | Sherry: What was that? | - | MOD-TEXT | Neu: Geräusch, jemand im Nebenraum. Leitet zur Mod-Szene Elza–Marvin in 30C1 über (README Z. 224) |
| ROOM3091 | 23 | LEBT | John | John: I dunno. Sounds like somebody's / in there. | - | MOD-TEXT | Neu |
| ROOM3091 | 24 | LEBT | Sherry | Sherry: There wasn't anybody before. | - | MOD-TEXT | Neu: Sherry war schon einmal hier |
| ROOM3091 | 25 | LEBT | Elza | Elza: Let me deal with it. / John, take Sherry somewhere safe. | D309 #22+#23 | ABGEWANDELT | Disc: Elza schlägt vor, sich aufzuteilen und einen sicheren Ort zu suchen (#22), John nimmt Sherry selbst mit (#23). Mod: Elza geht dem Geräusch allein nach und schickt John mit Sherry weg, neuer Anlass |
| ROOM3091 | 26 | LEBT | John | John: Will do! | D309 #25 | SINNGEMAESS | „Verlass dich drauf" als Antwort auf Elzas Auftrag statt auf ihre Sorge (#24) |
| ROOM3091 | 27 | LEBT | John | John: You stay with me now, you hear? | D309 #23 (2. Teil) | SINNGEMAESS | „Sherry, du kommst mit mir". 1. Teil „Übernimm dich nicht" fehlt |
| ROOM3091 | 28 | LEBT | Sherry | Sherry: Yes sir. | - | MOD-TEXT | Neu |
| ROOM3091 | 29 | LEBT | Sherry | Sherry: Good luck! | - | MOD-TEXT | Neu |
| ROOM3091 | 30 | LEBT | Elza | Elza: Thanks! | - | MOD-TEXT | Neu |
| ROOM3091 | 31 | LEBT | System/Untersuchen | You've taken the Latch Key. | D309 #26 | WOERTLICH | - |
| ROOM3091 | 32 | LEBT | System/Untersuchen | You've used the Latch Key. | D309 #27 | SINNGEMAESS | „Tür geöffnet" fehlt. Disc: nicht ausgelöst, Port ruft es (sub07) |
| ROOM30A0 | 0 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Raum 30A nur Mod (Itembox README Z. 262). Preview-Hinweis |
| ROOM30A0 | 1 | LEBT | System/Untersuchen | It's a computer.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Preview-Hinweis der Mod |
| ROOM30A0 | 2 | LEBT | System/Untersuchen | A detailed map of product / distribution in the city. | - | MOD-TEXT | - |
| ROOM30A0 | 3 | LEBT | System/Untersuchen | It's a disaster. | - | MOD-TEXT | - |
| ROOM30A0 | 4 | kein | System/Untersuchen | Wait, there's something here. | - | MOD-TEXT | - |
| ROOM30A0 | 5 | LEBT | System/Untersuchen | There is nothing useful. | - | MOD-TEXT | - |
| ROOM30A0 | 6 | LEBT | System/Untersuchen | There are only papers lying / around. | - | MOD-TEXT | - |
| ROOM30A0 | 7 | LEBT | System/Untersuchen | It's empty. | - | MOD-TEXT | - |
| ROOM30A0 | 8 | LEBT | System/Untersuchen | There is nothing more than / papers thrown everywhere.I do not see that it is useful. | - | MOD-TEXT | - |
| ROOM30A0 | 9 | LEBT | System/Untersuchen | A calendar and several sheets.It must be where they had part / of the control of the orders. | - | MOD-TEXT | - |
| ROOM30A1 | 0 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Raum 30A nur Mod (Itembox README Z. 262). Preview-Hinweis |
| ROOM30A1 | 1 | LEBT | System/Untersuchen | It's a computer.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Preview-Hinweis der Mod |
| ROOM30A1 | 2 | LEBT | System/Untersuchen | A detailed map of product / distribution in the city. | - | MOD-TEXT | - |
| ROOM30A1 | 3 | LEBT | System/Untersuchen | It's a disaster. | - | MOD-TEXT | - |
| ROOM30A1 | 4 | kein | System/Untersuchen | Wait, there's something here. | - | MOD-TEXT | - |
| ROOM30A1 | 5 | LEBT | System/Untersuchen | There is nothing useful. | - | MOD-TEXT | - |
| ROOM30A1 | 6 | LEBT | System/Untersuchen | There are only papers lying / around. | - | MOD-TEXT | - |
| ROOM30A1 | 7 | LEBT | System/Untersuchen | It's empty. | - | MOD-TEXT | - |
| ROOM30A1 | 8 | LEBT | System/Untersuchen | There is nothing more than / papers thrown everywhere.I do not see that it is useful. | - | MOD-TEXT | - |
| ROOM30A1 | 9 | LEBT | System/Untersuchen | A calendar and several sheets.It must be where they had part / of the control of the orders. | - | MOD-TEXT | - |
| ROOM30B0 | 0 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Raum 30B nur Mod (README Z. 70). Preview-Hinweis |
| ROOM30B0 | 1 | kein | System/Untersuchen | It's a computer.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Preview-Hinweis der Mod |
| ROOM30B1 | 0 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Raum 30B nur Mod (README Z. 70). Preview-Hinweis |
| ROOM30B1 | 1 | kein | System/Untersuchen | It's a computer.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Preview-Hinweis der Mod |
| ROOM30C0 | 0 | kein | System/Untersuchen | You've opened the lock. | D301 #1 | SINNGEMAESS | Standardsatz, Stelle nur Mod, ohne Karte. Raum 30C nicht auf der Disc (Leon dort im Spätstand-Foto #139) |
| ROOM30C0 | 1 | LEBT | System/Untersuchen |  / It's locked. | D306 #0 | WOERTLICH | Standardsatz, Stelle nur Mod |
| ROOM30C1 | 0 | kein | System/Untersuchen | You've opened the lock. | D301 #1 | SINNGEMAESS | Standardsatz, Stelle nur Mod, ohne Karte. Raum 30C nicht auf der Disc |
| ROOM30C1 | 1 | LEBT | Elza | Elza: Who are you? | - | MOD-TEXT | Raum 30C nicht auf der Disc. Spätstand-Foto #058 zeigt Elza und Marvin am Tresen, ohne Text (README Z. 367: „Added Marvin's cutscene in room 30C (Elza) (based on Inflames's screenshot)") |
| ROOM30C1 | 2 | LEBT | Officer | Officer: I'm Marvin Branagh, RPD. / I'm here with Leon Kennedy. | - | MOD-TEXT | Neu: Nachnamen Branagh und Kennedy (RE2-Retail), Elza erfährt Leons Namen. Keine erreichbare Disc-Zeile nennt die Gegenseite, Nachnamen nirgends (BERICHT §1, §9) |
| ROOM30C1 | 3 | LEBT | Elza | Elza: Leon? I don't know him. I never ran into him at the RPD. | - | PORT-TEXT | Ersetzt Mod „Leon?! How is he? We got separated back at the RPD." = MOD-TEXT, widerspricht dem Disc-Befund (kein Treffen Leon–Elza). Auch die Port-Fassung hat keine Disc-Vorlage |
| ROOM30C1 | 4 | LEBT | Marvin | Marvin: He's a rookie, and he's helping me and Ada escape the city. | - | PORT-TEXT | Ersetzt Mod „He's all right and he's helping me and Ada escape the city." = MOD-TEXT. „rookie" ist Port-Zusatz |
| ROOM30C1 | 5 | LEBT | Elza | Elza: Ada? | - | MOD-TEXT | Elza hört Adas Namen, neu |
| ROOM30C1 | 6 | LEBT | Marvin | Marvin: Someone we rescued from the / RPD. | - | MOD-TEXT | Inhaltlich nah an Leons Disc-Szene 109 (D109 #0–#5, Ada befreit), aber Marvin erzählt es Elza, neu |
| ROOM30C1 | 7 | LEBT | Elza | Elza: Will you tell them that me and John and Sherry are here and we're all right? | - | PORT-TEXT | Ersetzt Mod „Will you tell him that me and John and Sherry are here and we're all right?" = MOD-TEXT. Port: „them" statt „him" |
| ROOM30C1 | 8 | LEBT | Marvin | Marvin: Will do. See you soon! | - | MOD-TEXT | - |
| ROOM30C1 | 9 | LEBT | (ohne Praefix, vermutlich Spielfigur Sherry) |  / I need to hurry up and save the others... | D309 #8 | SINNGEMAESS | Türtext des Sherry-Abschnitts aus 309 übernommen, Name エルザ weggelassen. Foto #050–#055 zeigt Sherry allein in 309 und 30C, Port: Sherry ist ab 30C1 Spielfigur |
| ROOM30C1 | 10 | LEBT | System/Untersuchen |  / It's locked. | D306 #0 | WOERTLICH | Standardsatz, Stelle nur Mod |
| ROOM30D0 | 0 | LEBT | System/Untersuchen | The car is locked, I can't get inside. | - | MOD-TEXT | Raum 30D nur Mod |
| ROOM30D0 | 1 | LEBT | System/Untersuchen | The panel is smashed and doesn't work. | - | MOD-TEXT | - |
| ROOM30D1 | 0 | LEBT | System/Untersuchen | The car is locked, I can't get inside. | - | MOD-TEXT | Raum 30D nur Mod |
| ROOM30D1 | 1 | LEBT | System/Untersuchen | The panel is smashed and doesn't work. | - | MOD-TEXT | - |
| ROOM30E0 | 0 | LEBT | Ada | Ada: What was that noise? | D30E #0 | WOERTLICH | - |
| ROOM30E0 | 1 | LEBT | Leon | Leon: Y-you...! | D30E #1 | WOERTLICH | - |
| ROOM30E0 | 2 | LEBT | Ada | Ada: ...? Where's that other guy? / Wasn't he with you? | D30E #2 | WOERTLICH | - |
| ROOM30E0 | 3 | LEBT | Leon | Leon: Y-yeah, Marvin is out in the / hall. He can't get out because of / all the monsters outside... | D30E #3 (1. Teil) | ABGEWANDELT | Verletzung gestrichen: Disc „am Bein (東) erwischt, kann nicht laufen", Mod „kommt wegen der Monster nicht raus". Dazu Aufteilung auf #3–#5 |
| ROOM30E0 | 4 | LEBT | Leon | Leon: Ada... What have you been doing, / anyway!? | D30E #3 (2. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM30E0 | 5 | LEBT | Leon | Leon: And what happened in that / tunnel, what's with all the police / officer corpses? | D30E #3 (3. Teil) | ABGEWANDELT | „that tunnel" statt „diese Fabrik" (東度 = 工場). Aufteilung |
| ROOM30E0 | 6 | kein | System/Untersuchen |  / Manhole...? | D30E #4 | ABGEWANDELT | Fehllesung ホール „Halle" → „Manhole". Im Port ohne Aufruf (Disc ruft #4 in 30E0 sub02) |
| ROOM30E0 | 7 | LEBT | Ada | Ada: That's really bad... Those things / should already be trying to bust in / there. | D30E #5 (1. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu, frei |
| ROOM30E0 | 8 | LEBT | Ada | Ada: That glass won't hold up if they / keep pushing at it! | D30E #5 (2. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM30E0 | 9 | LEBT | Leon | Leon: Marvin!! Are you okay!? | D30E #6 | WOERTLICH | - |
| ROOM30E0 | 10 | LEBT | Leon | Leon: That wound... Did those things / get you? How careless... | D30E #7 | ABGEWANDELT | Schluss „ziemlich schlimm" (相当深い/酷い, Lesart) → „How careless", andere Aussage |
| ROOM30E0 | 11 | LEBT | Marvin | Marvin: The hall is no good...! Those / things broke through the main gate / and got inside! | D30E #8 | WOERTLICH | - |
| ROOM30E0 | 12 | LEBT | Leon | Leon: Say what!? | D30E #9 | WOERTLICH | - |
| ROOM30E0 | 13 | LEBT | Marvin | Marvin: The entire place is overrun! / And if they're already all the way out / here... the surface might just be done for... | D30E #10 | SINNGEMAESS | „mehr als erwartet" → „entire place overrun" |
| ROOM30E0 | 14 | LEBT | System/Untersuchen | It's too dangerous outside...We need to hurry and get that / elevator working. | D30E #23 | WOERTLICH | Disc ruft die Zeile nur in 30E1 (Elza), Port auch in 30E0 (sub03 Aot_reset0) |
| ROOM30E0 | 15 | LEBT | System/Untersuchen | There's a Flamethrower here.This weapon is not available / in this preview. | - | MOD-TEXT | Preview-Hinweis der Mod (Flammenwerfer) |
| ROOM30E1 | 0 | LEBT | Elza | Elza: John! | D30E #11 | WOERTLICH | - |
| ROOM30E1 | 1 | LEBT | John | John: Elza! So this is where you were? | D30E #12 (1. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM30E1 | 2 | LEBT | John | John: This is bad... one of those monsters / bit me as I tried to get away! | D30E #12 (2. Teil) | ABGEWANDELT | Fehllesung 東まれ: Disc 囲まれ „umstellt", Mod „bit me". Dass John gebissen ist, ist neu |
| ROOM30E1 | 3 | LEBT | John | John: ...And there doesn't seem to be a / place with any medical supplies here... | D30E #12 (3. Teil) | ABGEWANDELT | Disc „nirgends ein Versteck" (隠れられそう), Mod „keine Medikamente", Folgefehler aus #2 |
| ROOM30E1 | 4 | LEBT | John | John: Elza, how are things on your end? / Did you find any tasty food or something? | D30E #12 (4. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu, „hinten im Lager" fehlt |
| ROOM30E1 | 5 | LEBT | Elza | Elza: No, nothing like that... | D30E #13 | SINNGEMAESS | - |
| ROOM30E1 | 6 | LEBT | John | John: Wh-what's wrong? Are those / walking corpses over there as well... | D30E #14 | WOERTLICH | - |
| ROOM30E1 | 7 | LEBT | Elza | Elza: No... not at all... That's... | D30E #15 (1.+2. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM30E1 | 8 | LEBT | John | John: What is that...? | D30E #15 (3. Teil) | ABGEWANDELT | Sprecherwechsel Elza → John (Disc かしら, weiblich, Regie bestätigt) |
| ROOM30E1 | 9 | LEBT | John | John: What are you talking about? / Is there something in there? | D30E #16 | WOERTLICH | - |
| ROOM30E1 | 10 | LEBT | Elza | Elza: I'm not sure what it is, but... / That can't be... human, can it...? | D30E #17 | WOERTLICH | - |
| ROOM30E1 | 11 | LEBT | John | John: Elza, is that thing still in there? | D30E #18 | WOERTLICH | - |
| ROOM30E1 | 12 | LEBT | Elza | Elza: Yes, I think so... But I believe / it got scared and hid when it was / attacked... | D30E #19 (1.+2. Teil) | ABGEWANDELT | Fehllesung 怖 (Ersatzzeichen für 姿): Disc „als ich in Panik schoss, verschwand es plötzlich", Mod „es bekam Angst und versteckte sich" |
| ROOM30E1 | 13 | LEBT | Elza | Elza: I don't know where it could've / gone, though. | D30E #19 (3. Teil) | ABGEWANDELT | nur Aufteilung, Wortlaut treu |
| ROOM30E1 | 14 | LEBT | Elza | Elza: ...Anyway, there are too many / walking corpses outside. We can't / really turn back. | D30E #20 (1.+2. Teil) | ABGEWANDELT | Sprecherwechsel John → Elza, Aufteilung |
| ROOM30E1 | 15 | LEBT | Elza | Elza: We have no choice but to go / inside... | D30E #20 (3. Teil) | ABGEWANDELT | Sprecherwechsel John → Elza |
| ROOM30E1 | 16 | LEBT | John | John: But... that beast might still be in / there, right? | D30E #21 | ABGEWANDELT | Sprecherwechsel Elza → John |
| ROOM30E1 | 17 | LEBT | Elza | Elza: We can probably deal with it if / we stay alert... Let's go! | D30E #22 | ABGEWANDELT | Sprecherwechsel John → Elza, „wenn es nur eines ist" (相手が一匹) → „if we stay alert" |
| ROOM30E1 | 18 | LEBT | System/Untersuchen | It's too dangerous outside...We need to hurry and get that / elevator working. | D30E #23 | WOERTLICH | - |

Bei PORT-TEXT steht in der Spalte Englisch die Port-Fassung aus `story_r36_texte.inc`, die ersetzte Mod-Zeile in der Bemerkung.

## 2. Tabelle T2: Disc-Zeilen der Räume 207, 300, 301, 306, 307, 308, 309, 30E

| Disc [Sprecher] | Japanisch | Deutsch | Port-Zeile(n) | Bemerkung |
|---|---|---|---|---|
| D207 #0 [System] | シャッターのスイッチがある。 / 押しますか？ [Ja/Nein] | Ein Schalter für das Rolltor. Drücken? | 20A0 #0, 20A1 #0 | Disc: nur per JUMP erreichbar. Nur raumübergreifend: Mod verlegte die Zeile samt Skript (sub05, Ck(5,0)=0) nach 20A. In 2070/2071 steht auf Id 0 „You've opened the lock." |
| D300 #0 [System] | マンホールの下に ハシゴがのびている / 降りますか？ [Ja/Nein] | Unter dem Kanaldeckel führt eine Leiter hinab. Hinabsteigen? | 3000 #0, 3001 #0 | - |
| D300 #1 [Spielfigur PL0D (Marvin)] | もう体力がもちそうにない / 仲間のもとへ急ごう | Meine Kräfte reichen nicht mehr. Schnell zu den Gefährten. | 3000 #1, 3001 #1 | Disc tot, Mod: Marvin spielbar |
| D300 #2 [Leon] | マーヴィン！ これは・・・ / ラクーン警さつの警かんじゃないか！ | Marvin! Das sind doch… Polizisten aus Raccoon! | 3000 #2 | - |
| D300 #3 [Marvin] | ああ こいつはひどい・・・ / みんな アンブレラ事けんの調査たい員だ | Ja, furchtbar… alles Ermittler im Umbrella-Fall. | 3000 #3 | - |
| D300 #4 [Marvin] | このはいこう度 何かあるぜ・・・ / レオン 気をつけろ！ | In dieser stillgelegten Fabrik ist etwas… Leon, sei vorsichtig! [Lesart] | 3000 #4 | sinngemäß, Fabrik fehlt |
| D300 #5 [Leon] | / マーヴィン あの女は？ | Marvin, wo ist die Frau? | 3000 #5 | - |
| D300 #6 [Marvin] | / エイダか・・・ 奴なら先に行った | Ada? Die ist schon vorgegangen. | 3000 #6 | - |
| D300 #7 [Marvin] | あの女 / この光けいに女いろひとつ変えなかったぜ | Bei diesem Anblick hat sie nicht mal die Miene verzogen. [Lesart] | 3000 #7 | - |
| D300 #8 [Leon] | 今度会ったときは / 聞く事がやまほどありそうだな・・・ | Beim nächsten Treffen werde ich sie einiges fragen müssen… | 3000 #8 | - |
| D300 #9 [—] | / ！！ | !! | 3001 #2 | Mod setzt Sprecherin Elza |
| D300 #10 [Elza] | / な 何 ここは・・・？ | W-was ist das hier…? | 3001 #3 | - |
| D300 #11 [John] | / 見ての通り 警かんの死体さ | Wie du siehst: tote Polizisten. | 3001 #4 | sinngemäß |
| D300 #12 [John] | ここはむかし 化学薬品こうじょうだった / て話だが今じゃ使われてねぇはずだ・・・ | Das hier soll mal eine Chemiefabrik gewesen sein, aber die dürfte längst stillliegen… | 3001 #5 | - |
| D300 #13 [John] | / いったい 何があったんだ？ | Was ist hier bloß passiert? | 3001 #6 | Sprecher John → Elza |
| D300 #14 [Sherry] | / エルザ・・・ | Elza… | 3001 #7 | - |
| D300 #15 [Elza] | / 大丈夫よ しっかりくっついてなさい | Keine Angst, bleib dicht bei mir. | 3001 #8 | - |
| D300 #16 [John] | 行くぜ / とりあえず かくれ度所さがしだ | Gehen wir. Erst mal ein Versteck suchen. [Lesart] | 3001 #9 | - |
| D301 #0 [System] | ロックされている / ドアを開けるには カードが必要だ | Verriegelt. Zum Öffnen braucht man eine Karte. | 3010 #0, 3011 #0 | Disc nicht ausgelöst, Port ruft es |
| D301 #1 [System] | / IDカードを使って ロックを外した | Mit der ID-Karte entriegelt. | 3010 #1, 3011 #1 | abgewandelt: „Green Keycard" |
| D306 #0 [System] | / カギがかかっている | Abgeschlossen. | 3060 #0, 3061 #0; abgewandelt 3060 #10, 3061 #10 | An AOT 2 hängt der Mod „from the other side" an |
| D306 #1 [Elza] | / ちょっと シェリー！ | He, Sherry! | 3060 #1 (kein Aufruf), 3061 #1 | - |
| D306 #2 [Elza] | どうしたの？ / 急にはしり出したりして… | Was ist denn? Rennst einfach los… | 3060 #2 (kein), 3061 #2 | - |
| D306 #3 [Sherry] | …エルザ / 私 ヒミツの入り口を知ってるの | …Elza, ich kenne einen geheimen Eingang. | 3060 #3 (kein), 3061 #3 | - |
| D306 #4 [Elza] | / ヒミツの入り口？ | Einen geheimen Eingang? | 3060 #4 (kein), 3061 #4 | - |
| D306 #5 [Sherry] | / ここよ この中から入れるの | Hier. Da drin kommt man rein. | 3060 #5 (kein), 3061 #5 | - |
| D306 #6 [Elza] | / シェリー！ 待って！ | Sherry! Warte! | 3060 #6 (kein), 3061 #6 | - |
| D306 #7 [Sherry] | 裏口にかかっているカギは / 中から外すわ / トビラのところで待ってて！ | Das Schloss an der Hintertür mache ich von innen auf. Wartet an der Tür! | 3060 #7 (kein), 3061 #7 + #8 | In 3061 auf zwei Zeilen verteilt |
| D306 #8 [John] | お おいガキ！ 危ねえぞ / 戻ってこい | He, Kleine! Das ist gefährlich, komm zurück! | 3060 #8 (kein), 3061 #9 | - |
| D306 #9 [Erzähltext, Leons Sicht] | いつの間にか、 / カギがはずされている・・・ | Irgendwann ist das Schloss aufgemacht worden… | 3060 #9 | sinngemäß: „For some reason" statt „irgendwann" |
| D307 #0 [Ada] | このリフトに東れ！ / …もうそれしかない | Steigt in diesen Aufzug! …Etwas anderes bleibt nicht. [Lesart] | 3070 #0 | Sprecher Ada → Leon |
| D307 #1 [Leon] | これに？ / …一体どこに続いているんだ？ | Hier rein? …Wohin führt der denn? | 3070 #1 | Sprecher Leon → Marvin |
| D307 #2 [Ada] | 地仲さ… / まだここで奴東に食われた方がましかもな… | Vermutlich: In die Hölle… vielleicht wäre es besser, sich hier von denen fressen zu lassen… [Lesart, Disc 地仲さ] | 3070 #2 | „Underground" statt (wohl) „Hölle" |
| D307 #3 [Leon] | / 何だって？ | Was? | 3070 #3 | Sprecher Leon → Marvin |
| D307 #4 [Marvin oder Ada, keine Regie] | / 急げ 奴東が来る！ | Schnell, sie kommen! [Lesart 奴等] | 3070 #4 | Sprecher → Leon |
| D307 #5 [Elza] | / 東かにね きっとどこかにいるわ…！ | Leise! Es ist bestimmt irgendwo hier…! [Lesart 静かにね] | 3071 #0 | „Leise!" fehlt |
| D307 #6 [John] | / OK エルザ 後ろは俺に任せときな | OK Elza, den Rücken decke ich dir. | 3071 #1 | - |
| D307 #7 [Elza] | / このリフトから東れたのよ | Es kam aus diesem Aufzug. [Lesart] | 3071 #2 | - |
| D307 #8 [John] | / ！…でっけえリフトだな… よっぽど大東の東物を / 階んでいたんだろう / …動かせるか？ | …Riesiges Ding. Damit wurden wohl sehr große Lasten transportiert. …Kriegst du ihn in Gang? [Lesart] | 3071 #3 + #4 + #5 | verteilt |
| D307 #9 [Elza] | / だめよ…外部の人間には無理みたいね… | Nein… für Außenstehende ist das wohl unmöglich… | 3071 #6 | abgewandelt: „von innen" statt „Außenstehende" |
| D307 #10 [Sherry] | / 私 動かし方わかるわ！ | Ich weiß, wie man ihn bedient! | 3071 #7 | - |
| D307 #11 [Elza] | / シェリー？…あなたって一体… | Sherry? …Wer bist du eigentlich… | 3071 #8 | sinngemäß, Identitätsfrage abgeschwächt |
| D307 #12 [John] | おい 話は後だ / 奴東この中まで入ってくるつもりだ…！ / リフトが動くなら 下に降りようぜ | He, reden wir später. Die wollen bis hier rein…! Wenn der Aufzug läuft, fahren wir runter. [Lesart] | 3071 #9 + #10 | verteilt |
| D307 #13 [Elza] | / ちょっと 何言ってるのよ！ 得体の知れない怪物がここから / 東れたのよ！？ | Was redest du da! Von hier ist ein unbekanntes Monster gekommen!? [Lesart] | 3071 #11 + #12 | verteilt |
| D307 #14 [John] | / ここにいたら確実に奴東のえさだ！ もう どうする事も出来ないぜ / さあ 行こう | Bleiben wir hier, sind wir deren Futter! Es geht nicht anders. Los. [Lesart] | 3071 #13 + #14 | verteilt |
| D307 #15 [System] | 電東が入っていないようだ・・・ / コントロールBOXさえ動けば ▼ / エレベーターは作動するようだが・・・ | Anscheinend kein Strom… Liefe die Steuerbox, würde der Aufzug wohl funktionieren… [Lesart] | 3070 #5, 3071 #15 | - |
| D308 #0 [Marvin oder Leon] | / …しつこい奴だぜ… | …hartnäckiger Kerl… | 3080 #0 | Sprecher → Ada |
| D308 #1 [Eher Ada, nicht bewiesen] | / …あれは…やはり… | …das ist… also doch… | 3080 #1 | - |
| D309 #0 [System] | / 電気はすでについている・・・ | Das Licht ist bereits an… | 3090 #0, 3091 #0 | - |
| D309 #1 [System] | 電気がつかない・・・ / ブレーカーが上がっているのだろうか？ | Das Licht geht nicht an… Ist der Schutzschalter oben? | 3091 #1 | - |
| D309 #2 [System] | / ブレーカーは下がっているようだ・・・ | Der Schalter scheint unten zu sein… | 3090 #1, 3091 #2 | - |
| D309 #3 [System] | / ブレーカーに手がとどかない・・・ | Ich komme nicht an den Schalter… | 3091 #3 | - |
| D309 #4 [System] | / ブレーカーは下がっているようだ・・・ | Der Schalter scheint unten zu sein… | 3090 #2, 3091 #4 | - |
| D309 #5 [System] | ブレーカーが上がっている・・・ / 下ろしますか？ [Ja/Nein] | Der Schalter ist oben… Umlegen? | 3091 #5 | - |
| D309 #6 [System] | かんぬきがはまっているので / ドアはあかない ▼ / これをはずすには カギが必要だ・・・ | Ein Riegel liegt vor, die Tür geht nicht auf. Zum Lösen braucht man einen Schlüssel… | 3091 #6 | - |
| D309 #7 [System] | かんぬきがはまっているので / ドアはあかない | Ein Riegel liegt vor, die Tür geht nicht auf. | 3090 #3, 3091 #7 (kein Aufruf) | Disc nicht ausgelöst, Port ruft es in 3090 |
| D309 #8 [Sherry-Abschnitt, auf der Disc bleibt die Figur Elza] | / はやくエルザたちを助けなければ・・・ | Ich muss Elza und die anderen schnell retten… | 3091 #8, 30C1 #9 | Name エルザ weggelassen, auch nach 30C übernommen |
| D309 #9 [John] | はあ、はあ…間一 ってヤツだな！ / もう少しでケツに いつかれてた所だぜ / 大丈夫か、エルザ | Puh… das war knapp! Fast hätten sie mir in den Hintern gebissen. Alles klar, Elza? [Lesart] | 3091 #9 + #10 | verteilt |
| D309 #10 [Elza] | はあ、はあ…何とかね！ / でも、もうダメかと思ったわ！ / | Puh… geht so! Ich dachte schon, das war es! | 3091 #11 | - |
| D309 #11 [John] | おい、お ちゃん 助かった、 / にきるぜ！ / | He, Kleine, danke! Das vergesse ich dir nicht! [Lesart] | 3091 #12 | sinngemäß |
| D309 #12 [Elza] | / ホントよ、ありがとうはあ、助かったわ / | Wirklich, danke. Puh, du hast uns gerettet. | 3091 #13 | - |
| D309 #13 [John] | / お ちゃん、 前なんてえんだ？ / | Wie heißt du denn, Kleine? [Lesart] | 3091 #14 | - |
| D309 #14 [Sherry] | / シェリー… / | Sherry… | 3091 #15 | - |
| D309 #15 [John] | シェリーか！俺はジョンだこれから俺が / 何があっても守ってやるから安心しな！ / | Sherry also! Ich bin John. Ab jetzt beschütze ich dich, egal was kommt! | 3091 #16 | - |
| D309 #16 [Elza] | / ずいぶんな気に入り様ね / | Du bist ja ganz vernarrt. | 3091 #17 | - |
| D309 #17 [John] | そりゃそうさ、西の 人だぜ？ / 俺の西に えてもシェリーは俺が守る！ / | Na klar, sie hat mir das Leben gerettet! Sherry beschütze ich, und wenn es mein Leben kostet! [Lesart] | 3091 #18 | - |
| D309 #18 [Elza] | 単純ね…。それよりシェリー、あんた / なぜあんな入り口を知ってるの？ / | Du bist einfach gestrickt… Aber Sherry, woher kennst du so einen Eingang? | 3091 #19 | - |
| D309 #19 [Sherry] | ここは…私のパパが いていた所なの。 / …だから… / | Hier… hat mein Papa gearbeitet. …Deshalb… [Lesart] | 3091 #20 | sinngemäß, „gearbeitet" → „used to be" |
| D309 #20 [Elza] | / あなたの…パパが…？ / | Dein… Papa…? | 3091 #21 | - |
| D309 #21 [John] | おい、そろそろ行こうぜ。 / こんな所にいたら、奴 またゾロゾロ / まってくる。 | He, gehen wir langsam. Wenn wir hierbleiben, kommen die wieder in Scharen. [Lesart] | fehlt im Port | An dieser Stelle setzt die Mod ihre neue Geräusch-Szene (3091 #22–#24) |
| D309 #22 [Elza] | そうね。手分けして、どこか / 安全な度所を しましょうか / | Stimmt. Teilen wir uns auf und suchen einen sicheren Ort. [Lesart] | 3091 #25 (abgewandelt, zusammen mit #23) | Aufteilen wird zu „Elza geht allein dem Geräusch nach" |
| D309 #23 [John] | そうだな、あまり無理するなよ。 / …シェリー、お前は俺と来な！ / | Gut. Übernimm dich nicht. …Sherry, du kommst mit mir! | 3091 #27 (2. Teil), Inhalt teils in #25 | 1. Teil „Übernimm dich nicht" fehlt |
| D309 #24 [Elza] | / ちょっと、大丈夫でしょうね？ / | Moment, geht das auch gut? | fehlt im Port | - |
| D309 #25 [John] | / 任せとけ！ / | Verlass dich drauf! | 3091 #26 | sinngemäß, anderer Anlass |
| D309 #26 [System] | / かんぬきのかぎをひろった | Riegelschlüssel aufgehoben. | 3091 #31 | - |
| D309 #27 [System] | / かんぬきのかぎを使ってドアを開けた | Tür mit dem Riegelschlüssel geöffnet. | 3091 #32 | Disc nicht ausgelöst, Port ruft es |
| D30E #0 [Ada] | / 一体何の騒ぎだ 今のは？ | Was war das für ein Lärm eben? | 30E0 #0 | - |
| D30E #1 [Leon] | / お お前は…！ | D-du bist…! | 30E0 #1 | - |
| D30E #2 [Ada] | …？あの男はどうした？ / 一緒じゃないのか？ | …? Wo ist der Mann? Ist er nicht bei dir? | 30E0 #2 | - |
| D30E #3 [Leon] | …あ ああマーヴィンならホールにいる / 外の化け物どもに東をやられて動けないんだ… / …エイダ …君は今まで何をしていたんだ！？ この東度では一体何があった？ / 外の警東東の死体は何だ！？ | …Ä-äh, Marvin ist in der Halle. Die Bestien draußen haben ihn wohl am Bein erwischt, er kann nicht laufen… …Ada, …was hast du die ganze Zeit gemacht!? Was ist in dieser Fabrik passiert? Und die toten Polizisten draußen!? [Lesart] | 30E0 #3 + #4 + #5 | Verletzung gestrichen, „Fabrik" → „tunnel" |
| D30E #4 [Ada] | / …ホール…？ | …Halle…? | 30E0 #6 (kein Aufruf) | Als „Manhole" fehlgelesen und im Port nicht gerufen, spielt also nicht |
| D30E #5 [Ada] | まずいぞ あんな所にいたら… / 今 外の奴東がだんだん東東し東めている あんなガラス東りでは / あの人東東手には持たないぞ！ | Schlecht… wenn er dort bleibt… Die da draußen sammeln sich allmählich. Hinter so viel Glas hält man dieser Menge nicht stand! [Lesart] | 30E0 #7 + #8 | verteilt |
| D30E #6 [Leon] | / マーヴィン！！ 大丈夫か？ | Marvin!! Alles in Ordnung? | 30E0 #9 | - |
| D30E #7 [Leon] | この傷…奴東にやられたのか？ / …東当東いな… | Diese Wunde… haben die dich erwischt? …ziemlich schlimm… [Lesart, 相当深い oder 相当酷い] | 30E0 #10 | „How careless" |
| D30E #8 [Marvin] | …ホールはまずい！ 外の奴東が / 東面東関を破って中に押し寄せてきた！ | …Die Halle ist verloren! Die da draußen haben den Haupteingang durchbrochen und drängen herein! [Lesart] | 30E0 #11 | - |
| D30E #9 [Leon] | / 何だって？ | Was? | 30E0 #12 | - |
| D30E #10 [Marvin] | 東東東上の東が東まってきてるぜ！ / こんな東外まで…地上はもう ダメだな… | Es sammeln sich mehr, als ich erwartet hatte! Bis hier draußen am Stadtrand… Oben ist es vorbei… [Lesart 予想以上の数が集まって] | 30E0 #13 | sinngemäß |
| D30E #11 [Elza] | / ジョン！ | John! | 30E1 #0 | - |
| D30E #12 [John] | / エルザ こんな所にいたか まずいぜ…東りを化け物どもに / 東まれちまった！ …東生 東れられそうな所なんか / ありゃしねえ… エルザ そっちはどうだ…奥の東東には / うまい食べ物でもあったか？ | Elza, hier bist du! Schlecht… die Bestien haben uns umstellt! …Verdammt, nirgends ein Versteck… Elza, wie sieht es bei dir aus… gab es hinten im Lager was Leckeres zu essen? [Lesart 隠れられそう, 倉庫?] | 30E1 #1 + #2 + #3 + #4 | Fehllesung: John gebissen, sucht Medikamente |
| D30E #13 [Elza] | / それどころじゃないわ… | Das ist jetzt nicht das Problem… | 30E1 #5 | sinngemäß |
| D30E #14 [John] | ど どうした？ / …そっちもあの死体どもが… | W-was ist? …Waren da auch diese Leichen… | 30E1 #6 | - |
| D30E #15 [Elza] | 違うわ…そんなのじゃないわ… / あれは あれは… / 一体何だったのかしら… | Nein… nicht so was… Das… das war… was war das nur… | 30E1 #7 + #8 | Schluss geht an John |
| D30E #16 [John] | 何を言ってる？ / この奥には何がいるんだ？ | Was redest du? Was ist da hinten? | 30E1 #9 | - |
| D30E #17 [Elza] | 分からない…分からないけど / あれは…まさか…人間じゃ… | Ich weiß es nicht… aber das… das war doch kein… Mensch… | 30E1 #10 | - |
| D30E #18 [John] | / エルザ そいつはまだ中にいるのか？ | Elza, ist das Ding noch drin? | 30E1 #11 | - |
| D30E #19 [Elza] | ええ 多分…でも無東東中で / 攻撃してたら急に怖を消して… / それきりどこに行ったかは分からないわ | Ja, wohl schon… Aber als ich in Panik schoss, verschwand es plötzlich… wohin, weiß ich nicht. [Lesart] | 30E1 #12 + #13 | „es bekam Angst" (怖 fehlgelesen) |
| D30E #20 [John] | …とにかく外は死体どもが多すぎて / 戻るのは危ねえ / …中に入るしかないぜ | …Draußen sind jedenfalls zu viele Leichen, zurück ist zu gefährlich. …Wir müssen rein. | 30E1 #14 + #15 | Sprecher John → Elza |
| D30E #21 [Elza] | でも…あの怪物がどこかに / 東んでいるかもしれないのよ？ | Aber… das Monster lauert vielleicht irgendwo! [Lesart] | 30E1 #16 | Sprecher Elza → John |
| D30E #22 [John] | 東手が一東なら まだ何とかなるさ… / …行くぜ！ | Wenn es nur eines ist, schaffen wir das schon… …Los! [Lesart] | 30E1 #17 | Sprecher John → Elza, Bedingung „nur eines" fehlt |
| D30E #23 [System] | 外は危険だ・・・ / 東くエレベーターを動かさなければ | Draußen ist es gefährlich… Ich muss schnell den Aufzug in Gang bringen. [Lesart] | 30E1 #18, zusätzlich 30E0 #14 | Disc nur Elza, Port auch Leon |

## 3. Auffälligste Abweichungen (Story)

Neue Begegnungen und Namen:
1. **30C1 #1–#8 (Elza–Marvin):** Ganze Szene ist Mod-Text, Raum 30C fehlt auf der Disc. Belegt ist nur das Bild Elza + Marvin am Tresen (Foto #058, ohne Text). #2 bringt die Nachnamen „Branagh" und „Kennedy" (RE2-Retail) und lässt Elza Leons Namen hören. Keine erreichbare Disc-Zeile nennt die Gegenseite. Der Mod-Wortlaut von #3/#7 („Leon?! How is he? We got separated…", „tell him…") setzt eine Bekanntschaft Leon–Elza voraus. Der Port überschreibt #3, #4, #7 (PORT-TEXT), hat dafür aber ebenfalls keine Disc-Vorlage.
2. **2000 #0–#11 (Leon, Kanal):** Mod-Szene von Juvenal. Neu: Leon will das Mädchen suchen, Sherry habe ihm den Fluchtweg gezeigt, die Gruppe trennt sich. Disc 2000 hat weder Text noch NPC. Ort und Besetzung zeigt das Spätstand-Foto #103–#105 mit einer unlesbaren Marvin-Zeile.
3. **2001 #0–#14 (Elza, Kanal):** Mod-Szene. Neu: Sherry läuft weg, weil sie ihren Vater rufen hört, und trägt eine Taschenuhr der Mutter als Glücksbringer. Auf der Disc kommt der Vater erst in 309 und 400 vor. Nur die Uhr hat eine Spur im Spätstand: Foto #037 in 200 zeigt 「…まあ 大きな時計ね！」 („Oh, was für eine große Uhr!").
4. **3091 #22–#30:** Neue Szene am Ende von 309 (Geräusch nebenan, Elza geht allein, schickt John mit Sherry weg). Sie ersetzt das gemeinsame Aufteilen der Disc (D309 #21–#25) und leitet zur Mod-Begegnung mit Marvin in 30C1 über. D309 #21 und #24 fehlen im Port.

Neue Fakten durch Fehllesung der Ersatzzeichen:
5. **30E1 #2/#3:** John sei „gebissen" worden und suche Medikamente. Disc: 周りを…囲まれちまった, „sie haben uns umstellt", und „nirgends ein Versteck". Der Biss ist eine Erfindung aus der Lesart 噛まれ.
6. **30E0 #3/#5/#6:** Leon verschweigt Marvins Verletzung („kommt wegen der Monster nicht raus" statt „am Bein erwischt, kann nicht laufen"), fragt nach dem „Tunnel" statt nach der Fabrik (東度 = 工場), Adas „Halle…?" wird „Manhole…?" und ist im Port nicht gerufen.
7. **30E1 #12:** Das Wesen „bekam Angst und versteckte sich". Disc: Elza schoss in Panik, es verschwand plötzlich. 怖 steht auf der Disc als Ersatz für 姿.
8. **3070 #2:** Adas „In die Hölle…" (地仲さ, 地下 scheidet aus, da 下 im Font liegt) wird „Underground…".
9. **3071 #6:** „Nur von innen bedienbar" statt „für Außenstehende unmöglich". Der Disc-Hinweis, dass Sherry als Insiderin den Aufzug bedienen kann, fällt weg. Dazu #8: „What are you talking about" statt „Wer bist du eigentlich".

Sprecherwechsel:
10. **30E1 #14–#17:** Rollentausch. Disc: John drängt hinein (#20, #22), Elza warnt vor dem Monster (#21). Mod: Elza drängt, John warnt. Die Bedingung „wenn es nur eines ist" fehlt. Dazu #8: Elzas „was war das nur…" geht an John.
11. **3070 #0, #1, #3, #4:** Aufzugsszene mit vertauschten Rollen. Disc: Ada befiehlt „Steigt in den Aufzug", Leon fragt „Wohin führt der?". Mod: Leon befiehlt, Marvin fragt, Leon ruft „Schnell, sie kommen".
12. **3080 #0:** „…hartnäckiger Kerl…" wird Ada zugeschrieben, auf der Disc Marvin oder Leon.
13. **3001 #6:** „Was ist hier bloß passiert?" John → Elza.

Kleinere Verschiebungen:
14. **3060 #9:** „For some reason" statt „irgendwann". Der Zeitbezug, auf den BERICHT §4 die Reihenfolge „Elza vor Leon" in der Fabrik stützt, geht verloren. Der Zustand (Tür offen) bleibt.
15. **20A0 #0:** Disc-Zeile D207 #0 (Rolltor-Schalter) samt Skriptbedingung nach 20A verlegt, 207 trägt stattdessen „You've opened the lock.".
16. **3010 #1:** „Green Keycard" statt ID-Karte. **2040 #0:** neue Namen „Bill Johnson", „Knights Construction".
17. **3091 #8, 30C1 #9:** „save the others" statt „Elza und die anderen retten". Weil Mod und Port Sherry hier spielbar machen, bleibt die Aussage gleich, der Name als Disc-Beleg ist im Text aber weg.
