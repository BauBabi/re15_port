# Textabgleich STAGE 1 (Räume 1xx): Port-Englisch gegen 40%-Disc

Spur ST80-P. Grundlage: `texte_paare.md` (gleicher Ordner), Disc-Texte aus `analysis/story_begegnungen/TEXTE_40.md`, Spätstand-Fotos aus `analysis/story_begegnungen/pruefung/video_a.md`, Mod-Änderungen aus `info/Re1.5/README.TXT` (Zeilennummern als „README Z.“). Erzeugt mit einem Einmal-Skript aus `texte_paare.md`, die englischen Texte sind unverändert übernommen (`|` -> `/`).

**Regeln dieser Auswertung**

- Dialoge werden gegen Disc-Zeilen in jedem Raum geprüft. Einzelne Ausrufe („John, wait!“, „Say what!?“) in einer anderen Szene gelten als Floskel und bleiben MOD-TEXT, die Bemerkung nennt die Disc-Stelle.
- System-Bausteine (Schalter-Frage, „abgeschlossen“, „entriegelt“, „verriegelt, Karte nötig“) werden gegen gleichlautende Disc-Systemzeilen anderer Räume geurteilt. Die Vorlage trägt dann den Zusatz „(Baustein)“: Belegt ist der Wortlaut, nicht der Platz.
- Zeilen zu Gegenständen und Rätseln der Mod (Feuerlöscher, Kartencode, Minidisc, Zange, Zeitbombe, Notstrom, Weste, Speichern) sind MOD-TEXT, auch wenn die Frageform einer Disc-Zeile ähnelt.
- Spätstand-Fotos (80 %, Video A) sind kein Disc-Beleg. Wo eines dieselbe Aussage zeigt, bleibt das Urteil MOD-TEXT, die Bemerkung nennt das Foto.
- Eine auf mehrere Port-Zeilen verteilte Disc-Zeile ist nach Vorgabe ABGEWANDELT, auch wenn der Inhalt treu ist. Die Bemerkung sagt das dann.
- Aufruf: LEBT / TOT / kein aus `texte_paare.md`. „LEBT (C-AOT)“: Zeile aus `story_r36`, ausgelöst über einen im Port angelegten Untersuchen-Platz (`re15_port/engine/src/gen/story_r36_texte.inc`).
- Disc-Sprecher sind in TEXTE_40.md erschlossen, nicht im Text vermerkt. Sprecherwechsel beziehen sich auf diese Zuordnung.

**Zählung T1** (533 englische Port-Zeilen): WOERTLICH 59, SINNGEMAESS 20, ABGEWANDELT 31, MOD-TEXT 421, PORT-TEXT 2.

## T1 — Englische Port-Zeilen der Räume 1xx

| Datei | Id | Aufruf | Sprecher (EN) | Englisch (wortgetreu aus texte_paare.md) | Vorlage | Urteil | Bemerkung |
|---|---|---|---|---|---|---|---|
| 1020 | 0 | LEBT | System/Untersuchen | It's the RPD officer, Roy.He appears to have died recently. | - | MOD-TEXT | Leon findet Roys Leiche und kennt seinen Namen. Disc 1020 ohne Text und ohne Roy (Roys Leiche: README Z. 265; Port zeigt sie erst nach Strom, story_r36 P1) |
| 1020 | 1 | LEBT | System/Untersuchen | There are only reports among / other office objects.Nothing that can be of help. | - | MOD-TEXT | Disc 102 hat keinen Untersuchen-Text |
| 1020 | 2 | LEBT | System/Untersuchen | It's empty. | - | MOD-TEXT | Disc 102 hat keinen Untersuchen-Text |
| 1020 | 3 | LEBT | System/Untersuchen | It's Lieutenant Branagh's desk. | - | MOD-TEXT | neuer Name „Branagh“ (Marvins RE2-Nachname), Disc nennt ihn nirgends; Disc 102 ohne Untersuchen-Text |
| 1020 | 4 | kein | System/Untersuchen | There is something saved here. | - | MOD-TEXT | Disc 102 ohne Untersuchen-Text |
| 1020 | 5 | LEBT | System/Untersuchen | There are only reports of various / cases. | - | MOD-TEXT | Disc 102 ohne Untersuchen-Text |
| 1021 | 0 | LEBT | John | John: No way, this can't be... Roy, this / is a joke, right? | D102 #0 | WOERTLICH |  |
| 1021 | 1 | LEBT | Roy | Roy: John... Help me... Please kill me. | D102 #1 | WOERTLICH |  |
| 1021 | 2 | LEBT | Elza | Elza: John, shoot! That thing isn't / Roy anymore! | D102 #2 | WOERTLICH |  |
| 1021 | 3 | LEBT | John | John: Roy, it's me... You do recognize / me, right? | D102 #3 | WOERTLICH |  |
| 1021 | 4 | LEBT | Elza | Elza: John, I couldn't call for help / using the radio. | D102 #4 | ABGEWANDELT | D102 #4 auf #4 und #5 verteilt; dieser Teil (Funk) treu |
| 1021 | 5 | LEBT | Elza | Elza: The building's electric power / is starting to fail... | D102 #4 | ABGEWANDELT | zweite Hälfte von D102 #4; Disc: Strom „ist ausgefallen“, Mod: „starting to fail“ |
| 1021 | 6 | LEBT | John | John: What am I supposed to do alone? / I don't get it! | D102 #5 | ABGEWANDELT | SPRECHERWECHSEL Elza -> John (Disc-Endung のよ = Elza); „I don't get it“ statt „weiß nicht, was tun“ |
| 1021 | 7 | LEBT | Elza | Elza: We're the only ones left. If we / don't do anything, we'll also be... | D102 #6 | SINNGEMAESS | „the only ones left“ etwas stärker als 残った私たち („wir, die übrig sind“) |
| 1021 | 8 | LEBT | John | John: Please... I... I'd like to be / alone now... | D102 #7 | WOERTLICH |  |
| 1021 | 9 | LEBT | System/Untersuchen | He's dead.Poor Roy... | - | MOD-TEXT | Untersuchen an Roys Leiche (README Z. 265), Disc ohne |
| 1021 | 10 | LEBT | System/Untersuchen | There are only reports among / other office objects.Nothing that can be of help. | - | MOD-TEXT | Disc 102 hat keinen Untersuchen-Text |
| 1021 | 11 | LEBT | System/Untersuchen | It's empty. | - | MOD-TEXT | Disc 102 hat keinen Untersuchen-Text |
| 1021 | 12 | LEBT | System/Untersuchen | It's Lieutenant Branagh's desk. | - | MOD-TEXT | neuer Name „Branagh“ (Marvins RE2-Nachname), Disc nennt ihn nirgends; Disc 102 ohne Untersuchen-Text |
| 1021 | 13 | kein | System/Untersuchen | There is something saved here. | - | MOD-TEXT | Disc 102 ohne Untersuchen-Text |
| 1021 | 14 | LEBT | System/Untersuchen | There are only reports of various / cases. | - | MOD-TEXT | Disc 102 ohne Untersuchen-Text |
| 1040 | 0 | LEBT | System/Untersuchen | It's a shutter switch. / Will you push it?  | D104 #0 | ABGEWANDELT | gleiche Aufrufstellen sub02/07/08; Disc sagt nur „Knopf“, „Rolltor“ ist Mod-Präzisierung (README Z. 201), Wortlaut = D207 #0 |
| 1040 | 1 | LEBT | System/Untersuchen | It's an elevator.Pressing the button has no effect.It seems the power isn't turned on. | - | MOD-TEXT | Strom-Rätsel der Mod (Bedingung Ck(4,243)); Anklang D307 #15 (Fabrik-Aufzug ohne Strom) |
| 1041 | 0 | LEBT | System/Untersuchen | It's a shutter switch. / Will you push it?  | D104 #0 | ABGEWANDELT | gleiche Aufrufstellen sub02/07/08; Disc sagt nur „Knopf“, „Rolltor“ ist Mod-Präzisierung (README Z. 201), Wortlaut = D207 #0 |
| 1041 | 1 | LEBT | System/Untersuchen | It's an elevator.Pressing the button has no effect.It seems the power isn't turned on. | - | MOD-TEXT | Strom-Rätsel der Mod (Bedingung Ck(4,243)); Anklang D307 #15 (Fabrik-Aufzug ohne Strom) |
| 1041 | 2 | LEBT | Elza | Elza: What was that!? | - | MOD-TEXT | Mod-Szene (README Z. 276): Schuss aus dem Off, Schütze ungenannt |
| 1041 | 3 | LEBT | (ohne Präfix, Geräuschtext) | gunshot | - | MOD-TEXT | Geräuschtext der Mod-Szene (README Z. 276) |
| 1090 | 0 | LEBT | Woman | Woman: Anyone! Can someone please / get me out of here!? | D109 #0 | SINNGEMAESS | „aufmachen“ -> „get me out“; Präfix „Woman“ (Disc ohne Sprecher, erschlossen Ada) |
| 1090 | 1 | LEBT | Leon | Leon: Hey! Who's that?! | D109 #1 | SINNGEMAESS | „Ist da drin jemand?“ -> „Who's that?!“ |
| 1090 | 2 | LEBT | Woman | Woman: Someone, anyone! If out there, / open the door! | D109 #2 | SINNGEMAESS | „Ist denn niemand da!“ fehlt |
| 1090 | 3 | LEBT | Leon | Leon: There! Are you okay? | D109 #3 | WOERTLICH |  |
| 1090 | 4 | LEBT | Woman | Woman: You saved me... Thank you, / you have my gratitude. | D109 #4 | WOERTLICH |  |
| 1090 | 5 | LEBT | Leon | Leon: We can talk later... It's not safe / here. | D109 #5 | ABGEWANDELT | D109 #5 auf #5 und #6 verteilt, Inhalt treu |
| 1090 | 6 | LEBT | Leon | Leon: Besides, you need to get that / wound treated, right? Let's go. | D109 #5 | ABGEWANDELT | zweite Hälfte von D109 #5, Inhalt treu |
| 1090 | 7 | LEBT | System/Untersuchen | I must hurry up and get something to / put out this fire to save that woman! | - | MOD-TEXT | Feuerlöscher-Auftrag Mod (README Z. 333); Disc löscht ohne Text |
| 1090 | 8 | LEBT | System/Untersuchen | Will you use the Fire Extinguisher?  | - | MOD-TEXT | Feuerlöscher Mod (README Z. 333); Frageform wie D403 #1 |
| 1090 | 9 | LEBT | System/Untersuchen | You've used the Fire Extinguisher. | - | MOD-TEXT | Feuerlöscher Mod (README Z. 333) |
| 1091 | 20 | LEBT (C-AOT) | System/Untersuchen | It's a burnt-out shed.Someone must have put out the fire. | - | PORT-TEXT | PORT-NEU (story_r36 P3, Nutzer-Stichwort), ersetzt keine Mod-Zeile. Disc 1091 ohne Text; stützt sich auf den Disc-Zustand (Elza findet den Transporter ohne Feuer, BERICHT §4). „shed“ statt Transporter |
| 10B1 | 0 | LEBT | Elza | Elza: John, wait! | D10B #0 | ABGEWANDELT | D10B #0 auf #0 und #1 verteilt |
| 10B1 | 1 | LEBT | Elza | Elza: There's still a little girl / trapped in there! | D10B #0 | ABGEWANDELT | zweite Hälfte von D10B #0; „zurückgeblieben“ -> „trapped“. Spätstand #032 hat anderen Wortlaut (内に女の子がいるの / 戻らなきゃ) |
| 10B1 | 2 | LEBT | John | John: Say what!? | D10B #1 | WOERTLICH |  |
| 10B1 | 3 | LEBT | Elza | Elza: I found her in the basement / garage! We can't just leave her behind! | D10B #2 | SINNGEMAESS | „eingesperrt“ -> „I found her“, „ich“ -> „wir“ |
| 10B1 | 4 | LEBT | John | John: The garage in the basement, / huh...? | D10B #3 | ABGEWANDELT | erste Hälfte von D10B #3 |
| 10B1 | 5 | LEBT | John | John: If that's the case, we can go / through the sewer tunnels. Follow me. | D10B #3 | ABGEWANDELT | NEUER FAKT: Kanaltunnel als Weg; Disc sagt nur „hier entlang ist es kürzer, komm schnell“ |
| 1150 | 0 | LEBT | System/Untersuchen | There's a small sign in one corner. / It reads:Raccoon City 21st Century, / Hillman-Grayson Architects LLC." | - | MOD-TEXT | Schild am Stadtmodell, Disc 115 hat nur die Chef-Zeile |
| 1150 | 1 | LEBT | System/Untersuchen | It's a phone.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Speicher-Hinweis der Mod-Basis („preview“); Disc kann nicht speichern |
| 1150 | 2 | LEBT | Irons | Irons: I'll be fine. Just worry about / yourself for now. | D115 #0 | WOERTLICH | Präfix „Irons“ nur Mod (README Z. 268); Disc nennt den Chef nicht beim Namen. Spätstand #016 (Elza-Lauf, 「……レオン」) fehlt in Mod und Disc |
| 1150 | 3 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Hinweis der Mod-Basis („preview“) |
| 1150 | 4 | LEBT | Leon | Leon: Sir... the helicopter is / gone... it was our only way out... | - | MOD-TEXT | Irons-Szene Mod (README Z. 192): Hubschrauber weg als einziger Ausweg. Foto #089 zeigt Leon am Sofa ohne Text |
| 1150 | 5 | LEBT | Irons | Irons: Calm down Rookie, there / must be a way. | - | MOD-TEXT | Irons-Szene Mod (README Z. 192); „Rookie“ |
| 1150 | 6 | LEBT | Leon | Leon: The building is besieged by / those things, I don't see any option. | - | MOD-TEXT | Irons-Szene Mod (README Z. 192) |
| 1150 | 7 | LEBT | Irons | Irons: Maybe using the communication / system... you can ask for outside help. | - | MOD-TEXT | Irons-Szene Mod: Auftrag Funkanlage (neu) |
| 1150 | 8 | LEBT | Leon | Leon: But the system is damaged. | - | MOD-TEXT | Irons-Szene Mod: Anlage beschädigt (neu) |
| 1150 | 9 | LEBT | Irons | Irons: I know, I know... but some / officers were trying to repair it, / you may be luckier than them. | - | MOD-TEXT | Irons-Szene Mod: andere Polizisten wollten sie reparieren (neu) |
| 1150 | 10 | LEBT | Leon | Leon: Ok, I'll go see if I can / achieve something with that. | - | MOD-TEXT | Irons-Szene Mod |
| 1150 | 11 | kein | Irons | Irons: Take this card... it will allow / you to enter the communications room... / now go... | - | MOD-TEXT | Irons übergibt Karte (tot); Kamiya nennt für RE2 Marvin als Kartengeber |
| 1150 | 12 | LEBT | Irons | Irons: And remember this Leon, never / let your guard down... uniform or not, | - | MOD-TEXT | Irons-Szene Mod; deutet an, dass der Chef gebissen ist (Disc: nur „Mir geht es gut“) |
| 1150 | 13 | LEBT | (ohne Präfix, Fortsetzung Irons) | those things... are no longer what / they were, don't make my mistake... | - | MOD-TEXT | Fortsetzung von #12; Motiv wie D102 #2 („nicht mehr Roy“), keine Vorlage |
| 1150 | 14 | LEBT | Leon | Leon: Yes sir... | - | MOD-TEXT | Irons-Szene Mod |
| 1151 | 0 | LEBT | System/Untersuchen | There's a small sign in one corner. / It reads:Raccoon City 21st Century, / Hillman-Grayson Architects LLC." | - | MOD-TEXT | Schild am Stadtmodell, Disc 115 hat nur die Chef-Zeile |
| 1151 | 1 | LEBT | System/Untersuchen | It's a phone.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Speicher-Hinweis der Mod-Basis („preview“); Disc kann nicht speichern |
| 1151 | 2 | LEBT | Irons | Irons: I'll be fine. Just worry about / yourself for now. | D115 #0 | WOERTLICH | Präfix „Irons“ nur Mod (README Z. 268); Disc nennt den Chef nicht beim Namen. Spätstand #016 (Elza-Lauf, 「……レオン」) fehlt in Mod und Disc |
| 1151 | 3 | LEBT | System/Untersuchen | Itembox is not available in / this preview. | - | MOD-TEXT | Hinweis der Mod-Basis („preview“) |
| 1170 | 0 | LEBT | Elliot | Elliot: We must go! There is no / time to lose! | - | MOD-TEXT | Hubschrauber-Szene Mod (README Z. 160, Texte Juvenal); neue Figur „Elliot“ |
| 1170 | 1 | LEBT | Pilot | Pilot: Is there no one else left / inside? | - | MOD-TEXT | Hubschrauber-Szene Mod (README Z. 160); neue Figuren Elliot und Pilot |
| 1170 | 2 | LEBT | Elliot | Elliot: There is no one left, we / must go! | - | MOD-TEXT | Hubschrauber-Szene Mod (README Z. 160); neue Figuren Elliot und Pilot |
| 1170 | 3 | LEBT | Elliot | Elliot: Those things are already / close, they almost caught me! | - | MOD-TEXT | Hubschrauber-Szene Mod (README Z. 160); neue Figuren Elliot und Pilot |
| 1170 | 4 | LEBT | Pilot | Pilot: Alright get on board we're / off. | - | MOD-TEXT | Hubschrauber-Szene Mod (README Z. 160); neue Figuren Elliot und Pilot |
| 1170 | 5 | LEBT | Leon | Leon: Hey! Hey wait! Down here! | - | MOD-TEXT | Hubschrauber-Szene Mod; Spätstand B 0:19 zeigt Leon auf dem Dach mit おい 冗談だろ？ (ähnlicher Anlass, anderer Wortlaut) |
| 1170 | 6 | LEBT | Leon | Leon: Oh, that's just freaking great... | - | MOD-TEXT | Mod; Spätstand B 0:19–0:23.6 …クソッなんてこった！ sinnähnlich, kein Disc-Text |
| 1170 | 7 | LEBT | Leon | Leon: Now what am I gonna do? | - | MOD-TEXT | Mod |
| 1170 | 8 | LEBT | (ohne Präfix, vermutlich Erzähler) | We barricaded ourselves / inside the police station... | - | MOD-TEXT | Erzähltext, Disc ohne; Motiv „たてこもる“ nur im Complete-Disc-Werbetitel (BERICHT §8) |
| 1170 | 9 | LEBT | (ohne Präfix, vermutlich Erzähler) | Where we thought it would / be safe. | - | MOD-TEXT | Erzähltext, Disc ohne |
| 1170 | 10 | LEBT | (ohne Präfix, vermutlich Erzähler) | It was no use, and so began a / life and death struggle... | - | MOD-TEXT | Erzähltext, Disc ohne |
| 1170 | 11 | LEBT | (ohne Präfix, vermutlich Erzähler) | To escape. | - | MOD-TEXT | Erzähltext, Disc ohne |
| 1170 | 12 | LEBT | System/Untersuchen | It's locked from the other side. | D306 #0 (Baustein) | ABGEWANDELT | Floskel + „von der anderen Seite“ (RE2-Formel, nicht auf der Disc) |
| 1170 | 13 | LEBT | Leon | Leon: Damn it, there must be another / way out of here. | - | MOD-TEXT | Mod-Monolog (README Z. 160) |
| 1170 | 14 | LEBT | Leon | Leon: If only I could contact the / outside. | - | MOD-TEXT | Mod-Monolog; Motiv „keine Verbindung nach draußen“ wie D102 #4 |
| 1170 | 15 | LEBT | Leon | Leon: ... | - | MOD-TEXT | Mod-Monolog |
| 1170 | 16 | LEBT | Leon | Leon: That's right! I have to report / this to the chief. | - | MOD-TEXT | kein Disc-Text, aber SPÄTSTAND-BELEG: Foto #083 (117 Cut 5) そうだ！[..]にこの[..]報告しないと, gleicher Raum, gleiche Aussage |
| 1171 | 0 | LEBT | John | John: Elza, hurry up! | D117 #0 | WOERTLICH |  |
| 1171 | 1 | LEBT | John | John: We need to get this thing / off of here! Help me out! | D117 #1 | WOERTLICH |  |
| 1171 | 2 | kein | John | John: C'mon, give me a hand! | D117 #2 | WOERTLICH | auf der Disc ebenfalls nicht ausgelöst |
| 1171 | 3 | LEBT | John | John: Alright, let's get down from here! | D117 #3 | WOERTLICH |  |
| 1171 | 4 | LEBT | System/Untersuchen | It's locked from the other side. | D306 #0 (Baustein) | ABGEWANDELT | Floskel + „von der anderen Seite“ (RE2-Formel, nicht auf der Disc) |
| 1190 | 0 | LEBT | System/Untersuchen | There's a switch here. Push it?  | D119 #0 | WOERTLICH | gleicher Raum; Port ruft zusätzlich aus sub12 auf |
| 1190 | 1 | kein | System/Untersuchen | There's a driver here. Take it?  | D119 #1 | WOERTLICH | Disc ruft auf (sub10 @0x1CE8), Port nicht: Schraubendreher-Fund im Mod tot |
| 1190 | 2 | LEBT | System/Untersuchen | I have nothing else to do / here. | - | MOD-TEXT | Mod-Rätsel Schießstand (README Z. 253) |
| 1190 | 3 | LEBT | System/Untersuchen | No response...The power is not supplied. | - | MOD-TEXT | Folgetext zum Schalter D119 #0 fehlt auf der Disc; Strom-Rätsel Mod; Anklang D307 #15 |
| 1190 | 4 | LEBT | System/Untersuchen | There is one R.P.D. armor / that should fit you.Will you equip it?  | - | MOD-TEXT | Weste/Rüstung Mod |
| 1190 | 5 | LEBT | System/Untersuchen | Will you unequip the armor? e         /   4      4   / | - | MOD-TEXT | Weste/Rüstung Mod; Restbytes im Text |
| 1191 | 0 | LEBT | System/Untersuchen | There's a switch here. Push it?  | D119 #0 | WOERTLICH | gleicher Raum; Port ruft zusätzlich aus sub12 auf |
| 1191 | 1 | kein | System/Untersuchen | There's a driver here. Take it?  | D119 #1 | WOERTLICH | Disc ruft auf (sub10 @0x1CE8), Port nicht: Schraubendreher-Fund im Mod tot |
| 1191 | 2 | LEBT | System/Untersuchen | I have nothing else to do / here. | - | MOD-TEXT | Mod-Rätsel Schießstand (README Z. 253) |
| 1191 | 3 | LEBT | System/Untersuchen | No response...The power is not supplied. | - | MOD-TEXT | Folgetext zum Schalter D119 #0 fehlt auf der Disc; Strom-Rätsel Mod; Anklang D307 #15 |
| 1191 | 4 | LEBT | System/Untersuchen | There is one R.P.D. armor / that should fit you.Will you equip it?  | - | MOD-TEXT | Weste/Rüstung Mod |
| 1191 | 5 | LEBT | System/Untersuchen | Will you unequip the armor? e         /   4      4   / | - | MOD-TEXT | Weste/Rüstung Mod; Restbytes im Text |
| 11B0 | 0 | LEBT | Leon | Leon: H-hey! | D11B #9 | WOERTLICH | Leon sieht Sherry; läuft auf Disc und im Port |
| 11B0 | 1 | LEBT | Marvin | Marvin: Leon, you're okay! What / was the commotion up there? | D11B #10 | WOERTLICH | Disc tot (11C0 fragt Bank 8 ab), Port LEBT. Spätstand #169: nur 上のさわぎは何だ？ |
| 11B0 | 2 | LEBT | Leon | Leon: Those things broke through the / shutters and started getting inside. | D11B #11 | WOERTLICH | Disc tot, Port LEBT |
| 11B0 | 3 | LEBT | Marvin | Marvin: What about the chief? | D11B #12 | WOERTLICH | Disc tot, Port LEBT |
| 11B0 | 4 | LEBT | Marvin | Marvin: Oh, I see... Damn it! | D11B #13 | WOERTLICH | Disc tot, Port LEBT |
| 11B0 | 5 | LEBT | Marvin | Marvin: Well you saw it, right? / We can't get out using the patrol / cars anymore. What should we do? | D11B #14 | WOERTLICH | Disc tot, Port LEBT |
| 11B0 | 6 | LEBT | Leon | Leon: Marvin, please come here! | D11B #15 | WOERTLICH | Disc tot, Port LEBT |
| 11B0 | 7 | LEBT | Leon | Leon: There's a manhole here. How / about an underground escape route? | D11B #16 | WOERTLICH | Disc tot, Port LEBT. Spätstand #170 anderer Wortlaut |
| 11B0 | 8 | LEBT | Marvin | Marvin: We don't really have any other / option than to try. Okay, let's see if / we can move this! | D11B #17 | SINNGEMAESS | „Einen Versuch wert“ -> „keine andere Wahl als es zu versuchen“; Disc tot, Port LEBT. Spätstand #172 ganz andere Aussage, nicht im Mod |
| 11B0 | 9 | LEBT | Marvin | Marvin: Leon! give me a hand! | D11B #18 | WOERTLICH | Disc tot, Port LEBT |
| 11B0 | 10 | LEBT | Marvin | Marvin: Okay, let's hurry! | D11B #19 | WOERTLICH | Disc tot, Port LEBT; Sprecher Marvin wie Disc-Vermutung |
| 11B0 | 11 | LEBT | Leon | Leon: Marvin, I'll climb down first, / you help her get down! | D11B #20 | WOERTLICH | Disc tot, Port LEBT |
| 11B0 | 12 | LEBT | Marvin | Marvin: Okay! Be careful down there! | D11B #21 | WOERTLICH | Disc tot, Port LEBT |
| 11B0 | 13 | LEBT | System/Untersuchen | It doesn't seem like you could move / this on your own. | D11B #22 | WOERTLICH | gleicher Raum, gleiche Aussage |
| 11B0 | 14 | LEBT | System/Untersuchen | The shutter is tightly sealed in place. | - | MOD-TEXT | Disc 11B ohne Untersuchen-Text; Anklang D40A #3 (げん重にロック) |
| 11B0 | 15 | LEBT | System/Untersuchen | It's too dark to see anything...I must find way to activate the energy. | - | MOD-TEXT | dunkle 11B-Fassung der Mod (README Z. 193); Anklang D40A #9 „Viel zu dunkel.“; Strom-Rätsel Mod |
| 11B1 | 0 | LEBT | Elza | Elza: Hey! What are you doing in a / place like this? | D11B #0 | WOERTLICH |  |
| 11B1 | 1 | LEBT | Elza | Elza: You got trapped in here, right? | D11B #1 | SINNGEMAESS | „Komm her…“ fehlt |
| 11B1 | 2 | LEBT | Elza | Elza: What's your name? | D11B #2 | WOERTLICH |  |
| 11B1 | 3 | LEBT | Girl | Girl: ...Sherry. | D11B #3 | WOERTLICH | Präfix „Girl“ (Name noch unbekannt) |
| 11B1 | 4 | LEBT | Elza | Elza: Sherry, huh? That's a cute name. / I'm Elza. | D11B #4 | WOERTLICH |  |
| 11B1 | 5 | LEBT | Sherry | Sherry: Elza... | D11B #5 | WOERTLICH |  |
| 11B1 | 6 | LEBT | Elza | Elza: That's right! Okay Sherry, listen / up... | D11B #6 | WOERTLICH |  |
| 11B1 | 7 | LEBT | Elza | Elza: Just wait there. / I'll come rescue you in a bit. | D11B #7 | WOERTLICH | „ganz sicher“ (きっと) abgeschwächt |
| 11B1 | 8 | LEBT | Elza | Elza: So just hold tight and wait for / me quietly in that car over there. Okay? | D11B #8 | WOERTLICH |  |
| 11B1 | 9 | LEBT | System/Untersuchen | It doesn't seem like you could move / this on your own. | D11B #22 | WOERTLICH | gleicher Raum, gleiche Aussage |
| 11B1 | 10 | LEBT | Elza | Elza: Sherry!? | - | MOD-TEXT | neue Mod-Szene (README Z. 362): Sherry ist aus dem Wagen verschwunden; Motiv wie D403 #5 (Labor, später) |
| 11B1 | 11 | LEBT | Elza | Elza: Where has she gone!? | - | MOD-TEXT | neue Mod-Szene (README Z. 362); Motiv wie D403 #6, dort spricht John |
| 11B1 | 12 | LEBT | System/Untersuchen | The door is locked. | D306 #0 (Baustein) | WOERTLICH | Floskel „Abgeschlossen.“; D11B hat an dieser Stelle keine Zeile, Platz Mod |
| 11B1 | 13 | LEBT | System/Untersuchen | The shutter is tightly sealed in place. | - | MOD-TEXT | Disc 11B ohne Untersuchen-Text; Anklang D40A #3 (げん重にロック) |
| 11B1 | 14 | LEBT | System/Untersuchen | It's too dark to see anything...I must find way to activate the energy. | - | MOD-TEXT | dunkle 11B-Fassung der Mod (README Z. 193); Anklang D40A #9 „Viel zu dunkel.“; Strom-Rätsel Mod |
| 11C0 | 0 | LEBT | Leon | Leon: Ada! Where's Marvin? Wasn't / he with you? | D11C #0 | WOERTLICH |  |
| 11C0 | 1 | LEBT | Ada | Ada: He turned around all of a sudden / to go look for you. | D11C #1 | WOERTLICH |  |
| 11C0 | 2 | LEBT | Ada | Ada: There was this really loud noise / from upstairs earlier. | D11C #2 | WOERTLICH |  |
| 11C0 | 3 | LEBT | Ada | Ada: You didn't run into him? | D11C #3 | WOERTLICH | Spätstand #127 gleicher Wortlaut |
| 11C0 | 4 | LEBT | Leon | Leon: What!? | D11C #4 | WOERTLICH |  |
| 11C0 | 5 | LEBT | Leon | Leon: Ada, you hide inside that patrol / car. | D11C #5 | WOERTLICH |  |
| 11C0 | 6 | LEBT | Ada | Ada: O-okay, I'll do that... | D11C #6 | WOERTLICH |  |
| 11C0 | 7 | LEBT | Ada | Ada: Okay, it's over now. | D11C #7 | ABGEWANDELT | SPRECHERWECHSEL Leon -> Ada (Disc-Sprecher nur erschlossen; ぜ passt zu Leon wie zu Adas männlicher Sprechweise); Disc tot, Port LEBT (README Z. 208) |
| 11C0 | 8 | LEBT | Ada | Ada: ...There are some outrageous / monsters out there. | D11C #8 | SINNGEMAESS | „hier“ -> „out there“, Mehrzahl; Disc tot, Port LEBT |
| 11C0 | 9 | LEBT | Leon | Leon: Yeah... Anyway, let's get / out of here. | D11C #9 | WOERTLICH | Disc tot, Port LEBT; Dublette D11C #10 hat kein Gegenstück |
| 1000 | 0 | LEBT | System/Untersuchen | Nothing unusual. | - | MOD-TEXT | Floskel; Raum nur im Mod-Baum |
| 1001 | 0 | LEBT | System/Untersuchen | Nothing unusual. | - | MOD-TEXT | Floskel; Raum nur im Mod-Baum |
| 1010 | 0 | LEBT | System/Untersuchen | Several books of testimonies / and cases.Nothing useful. | - | MOD-TEXT | Mod-Raum 101 |
| 1010 | 1 | LEBT | System/Untersuchen | An old typewriter.Why use it if there are / computers? | - | MOD-TEXT | Mod-Text |
| 1010 | 2 | LEBT | System/Untersuchen | A camera to record the / interrogations. | - | MOD-TEXT | Mod-Raum 101 |
| 1011 | 0 | LEBT | Officer | Officer: groans | - | MOD-TEXT | Roy-Szene Elza in Mod-Raum 101 (README Z. 277). Spätstand #027/#028: Elza bei verletztem Polizisten, ohne Text |
| 1011 | 1 | LEBT | Elza | Elza: Hang in there! Are you the / only officer left in the building? | - | MOD-TEXT | Roy-Szene Mod: fragt nach weiteren Polizisten |
| 1011 | 2 | LEBT | Officer | Officer: Who... who are you? | - | MOD-TEXT | Roy-Szene Mod |
| 1011 | 3 | LEBT | Elza | Elza: My name's Elza. / What's yours? | - | MOD-TEXT | Roy-Szene Mod; Motiv wie D11B #2/#4 (Namen tauschen) |
| 1011 | 4 | LEBT | Officer | Officer: Elza, huh? I'm Roy. | - | MOD-TEXT | Roy-Szene Mod; Disc nennt Roy nur in 102 |
| 1011 | 5 | LEBT | Elza | Elza: That wound... did one of / those things get you? How horrible... | - | MOD-TEXT | Roy-Szene Mod; Roy verletzt vor seiner Verwandlung (Disc: nur 102) |
| 1011 | 6 | LEBT | Roy | Roy: This precinct isn't safe anymore. | - | MOD-TEXT | Roy-Szene Mod |
| 1011 | 7 | LEBT | Roy | Roy: Those things broke through the / rear shutters and got inside. | D11B #11 | ABGEWANDELT | aus Leons Zeile übernommen (fast wortgleich mit 11B0 #2): SPRECHER Roy statt Leon, Raum 101 statt 11B, „rear“ neu |
| 1011 | 8 | LEBT | Roy | Roy: Those bastards at Umbrella / caused all of this... | - | MOD-TEXT | NEUER FAKT: Roy gibt Umbrella die Schuld. Disc nennt Umbrella erst in 300/401 |
| 1011 | 9 | LEBT | Elza | Elza: ...What do you mean? | - | MOD-TEXT | Roy-Szene Mod |
| 1011 | 10 | LEBT | Roy | Roy: groans | - | MOD-TEXT | Roy-Szene Mod |
| 1011 | 11 | LEBT | Elza | Elza: Are you okay? | - | MOD-TEXT | Roy-Szene Mod |
| 1011 | 12 | LEBT | Roy | Roy: We don't have time to talk / right now. | - | MOD-TEXT | Floskel wie D401 #3 („keine Zeit zum Reden“), andere Szene |
| 1011 | 13 | LEBT | Roy | Roy: You have to rescue the survivors / in the other rooms. | - | MOD-TEXT | Roy-Szene Mod: Auftrag, Überlebende zu retten |
| 1011 | 14 | LEBT | Elza | Elza: But... | - | MOD-TEXT | Roy-Szene Mod |
| 1011 | 15 | LEBT | Roy | Roy: My friend John should still be / in the underground cell block. | - | MOD-TEXT | NEUER FAKT: John ist Roys Freund und sitzt im Zellentrakt. Disc: John kennt Roy (D102), steht aber nicht in 121 |
| 1011 | 16 | LEBT | Roy | Roy: Please, you have to help him... | - | MOD-TEXT | Roy-Szene Mod |
| 1011 | 17 | LEBT | Elza | Elza: Roy, wake up! Please wake up! | - | MOD-TEXT | Roy-Szene Mod; Floskel wie D501 #22 |
| 1011 | 18 | LEBT | System/Untersuchen | He has passed out from his injuries.He is holding a slip.The numbers 0513" are / printed on the slip. | - | MOD-TEXT | Code 0513 für Zahlenschloss (README Z. 164) |
| 1011 | 19 | LEBT | System/Untersuchen | He is holding something... | - | MOD-TEXT | Mod-Gegenstand |
| 1011 | 20 | LEBT | System/Untersuchen | You've taken the Prison key. | - | MOD-TEXT | Zellenschlüssel Mod; Form wie D309 #26 „aufgehoben“ |
| 1011 | 21 | LEBT | System/Untersuchen | Several books of testimonies / and cases.Nothing useful. | - | MOD-TEXT | Mod-Raum 101 |
| 1011 | 22 | LEBT | System/Untersuchen | An old typewriter.Why use it if there are / computers? | - | MOD-TEXT | Mod-Text |
| 1011 | 23 | LEBT | System/Untersuchen | A camera to record the / interrogations. | - | MOD-TEXT | Mod-Raum 101 |
| 1030 | 0 | LEBT | System/Untersuchen | This should keep those things out...For now... | - | MOD-TEXT | Rolltor/Gitter 103; Disc-Datei ohne Text (Durchbruch ROOM1030 sub08 ohne Text) |
| 1030 | 1 | LEBT | System/Untersuchen | Guess it was only a matter of time... | - | MOD-TEXT | Gitter gibt nach; Disc-Datei ohne Text |
| 1030 | 2 | LEBT | System/Untersuchen | The emergency shutters have / already been activated. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1030 | 3 | LEBT | System/Untersuchen | A payphone.The line appears to be down... | - | MOD-TEXT | Telefon tot; passt zu D102 #4 (kein Funk), keine Vorlage |
| 1030 | 4 | LEBT | System/Untersuchen | Some sort of reference chart for / identifying emergency severity." | - | MOD-TEXT | Disc-Datei ohne Text |
| 1030 | 5 | kein | System/Untersuchen | This emergency severity" chart / gives more confusion than it prevents. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1030 | 6 | LEBT | System/Untersuchen | There's nothing useful on the counter. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1030 | 7 | kein | System/Untersuchen | Seems like the crash broke the axle.Poor thing... | - | MOD-TEXT | Motorrad-Text; in Leons Datei ohne Aufruf |
| 1030 | 8 | LEBT | System/Untersuchen | Seems like whoever rode this crashed / it through the front gate.Wonder if they're ok... | - | MOD-TEXT | Leon untersucht ein Motorrad („whoever rode this… Wonder if they're ok“): indirekte Spur auf Elza, Mod-Text (Motorrad README Z. 332) |
| 1030 | 9 | kein | System/Untersuchen | It's too dark to see if there's / anything inside.Would you like to reach inside? | - | MOD-TEXT | ohne Aufruf; Anklang D40A #6/#7 (Regal zu dunkel, Hineintasten gefährlich), anderes Objekt |
| 1030 | 10 | kein | System/Untersuchen | Found Green Medicine. / Take it? | - | MOD-TEXT | ohne Aufruf; Frageform wie D119 #1 |
| 1030 | 11 | kein | System/Untersuchen | Better not dig through other's trash... | - | MOD-TEXT | Disc-Datei ohne Text |
| 1030 | 12 | LEBT | System/Untersuchen | There's glass from the front door / all over the floor. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1030 | 13 | LEBT | System/Untersuchen | Desks for filling out inquiry forms. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1030 | 14 | LEBT | System/Untersuchen | Seems like even the emergency / broadcast has gone offline... | - | MOD-TEXT | Notfunk tot; passt zu D102 #4, keine Vorlage |
| 1030 | 15 | LEBT | System/Untersuchen | A map of dowtown Raccoon City. / Population est. 100,000 or more... | - | MOD-TEXT | Disc-Datei ohne Text |
| 1030 | 16 | LEBT | System/Untersuchen | An emergency alarm.The red light works, but the alarm / bell appears to be malfunctioning. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1030 | 17 | kein | System/Untersuchen | There are some words carved here... / I    C  A  N  '  T    E  S  C  A  P  E  !  !  !"The door is locked, I can't open it. | - | MOD-TEXT | ohne Aufruf; eingeritzte Schrift plus angehängter Satz „The door is locked…“ (Floskel D306 #0) |
| 1031 | 0 | LEBT | System/Untersuchen | This should keep those things out...For now... | - | MOD-TEXT | Rolltor/Gitter 103; Disc-Datei ohne Text (Durchbruch ROOM1030 sub08 ohne Text) |
| 1031 | 1 | LEBT | System/Untersuchen | Guess it was only a matter of time... | - | MOD-TEXT | Gitter gibt nach; Disc-Datei ohne Text |
| 1031 | 2 | LEBT | System/Untersuchen | The emergency shutters have / already been activated. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1031 | 3 | LEBT | System/Untersuchen | A payphone.The line appears to be down... | - | MOD-TEXT | Telefon tot; passt zu D102 #4 (kein Funk), keine Vorlage |
| 1031 | 4 | LEBT | System/Untersuchen | Some sort of reference chart for / identifying emergency severity." | - | MOD-TEXT | Disc-Datei ohne Text |
| 1031 | 5 | kein | System/Untersuchen | This emergency severity" chart / gives more confusion than it prevents. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1031 | 6 | LEBT | System/Untersuchen | There's nothing useful on the counter. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1031 | 7 | LEBT | System/Untersuchen | Seems like the crash broke the axle.Poor thing... | - | MOD-TEXT | Elza an ihrem eigenen Motorrad (README Z. 332) |
| 1031 | 8 | kein | System/Untersuchen | Seems like whoever rode this crashed / it through the front gate.Wonder if they're ok... | - | MOD-TEXT | Leons Motorrad-Text, in Elzas Datei ohne Aufruf |
| 1031 | 9 | kein | System/Untersuchen | It's too dark to see if there's / anything inside.Would you like to reach inside? | - | MOD-TEXT | ohne Aufruf; Anklang D40A #6/#7 (Regal zu dunkel, Hineintasten gefährlich), anderes Objekt |
| 1031 | 10 | kein | System/Untersuchen | Found Green Medicine. / Take it? | - | MOD-TEXT | ohne Aufruf; Frageform wie D119 #1 |
| 1031 | 11 | kein | System/Untersuchen | Better not dig through other's trash... | - | MOD-TEXT | Disc-Datei ohne Text |
| 1031 | 12 | LEBT | System/Untersuchen | There's glass from the front door / all over the floor. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1031 | 13 | LEBT | System/Untersuchen | Desks for filling out inquiry forms. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1031 | 14 | LEBT | System/Untersuchen | Seems like even the emergency / broadcast has gone offline... | - | MOD-TEXT | Notfunk tot; passt zu D102 #4, keine Vorlage |
| 1031 | 15 | LEBT | System/Untersuchen | A map of dowtown Raccoon City. / Population est. 100,000 or more... | - | MOD-TEXT | Disc-Datei ohne Text |
| 1031 | 16 | LEBT | System/Untersuchen | An emergency alarm.The red light works, but the alarm / bell appears to be malfunctioning. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1031 | 17 | kein | System/Untersuchen | There are some words carved here... / I    C  A  N  '  T    E  S  C  A  P  E  !  !  !"The door is locked, I can't open it. | - | MOD-TEXT | ohne Aufruf; eingeritzte Schrift plus angehängter Satz „The door is locked…“ (Floskel D306 #0) |
| 1031 | 18 | LEBT | Elza | Elza: That was close, this must / contain them for some time... | - | MOD-TEXT | neue Elza-Szene in 103 (README Z. 161, Texte Juvenal) |
| 1031 | 19 | LEBT | Elza | Elza: Hey, is anyone here?! | - | MOD-TEXT | neue Elza-Szene in 103 (README Z. 161): ruft nach Überlebenden |
| 1031 | 20 | LEBT | (ohne Präfix, vermutlich Erzähler) | We barricaded ourselves / inside the police station... | - | MOD-TEXT | Erzähltext, Disc ohne; Motiv „たてこもる“ nur im Complete-Disc-Werbetitel (BERICHT §8) |
| 1031 | 21 | LEBT | (ohne Präfix, vermutlich Erzähler) | Where we thought it would / be safe. | - | MOD-TEXT | Erzähltext, Disc ohne |
| 1031 | 22 | LEBT | (ohne Präfix, vermutlich Erzähler) | It was no use, and so began a / life and death struggle... | - | MOD-TEXT | Erzähltext, Disc ohne |
| 1031 | 23 | LEBT | (ohne Präfix, vermutlich Erzähler) | To escape. | - | MOD-TEXT | Erzähltext, Disc ohne |
| 1050 | 0 | LEBT | System/Untersuchen | It's a shutter switch. / Will you push it?  | D207 #0 (Baustein) | WOERTLICH | Wortlaut = D207 #0 „Ein Schalter für das Rolltor. Drücken?“; Platz auf der Disc nicht belegt |
| 1050 | 1 | LEBT | System/Untersuchen | Various posters of missing / persons.It is better not to think about / it... | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1050 | 2 | kein | System/Untersuchen | I need a fuse to run the shutter. | - | MOD-TEXT | ohne Aufruf; Sicherungs-Motiv wie D403 #0, anderer Raum |
| 1050 | 3 | LEBT | System/Untersuchen | Just scattered papers and books.Nothing relevant. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1050 | 4 | LEBT | System/Untersuchen | A blackboard full of reports of riots / and cannibalism at points in the city. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1050 | 5 | kein | System/Untersuchen | He's got a big bite on his neck, he / didn't give him a chance to avoid it.The weapon can be useful.Sorry, I'll just borrow this... | - | MOD-TEXT | toter Polizist mit Waffe, Mod-Raum |
| 1050 | 6 | kein | Ada | Ada: Give me a moment, I need a / break. | - | MOD-TEXT | Ada-Szene (README Z. 105), ohne Aufruf; Wunde wie D109 #5, „Sanitätsraum“ neu |
| 1050 | 7 | kein | Leon | Leon: Relax, I'll take you to the / medical room. | - | MOD-TEXT | Ada-Szene (README Z. 105), ohne Aufruf |
| 1050 | 8 | LEBT | Leon | Leon: Hey, wait! | - | MOD-TEXT | Ada-Szene (README Z. 105) |
| 1051 | 0 | LEBT | System/Untersuchen | It's a shutter switch. / Will you push it?  | D207 #0 (Baustein) | WOERTLICH | Wortlaut = D207 #0 „Ein Schalter für das Rolltor. Drücken?“; Platz auf der Disc nicht belegt |
| 1051 | 1 | LEBT | System/Untersuchen | Various posters of missing / persons.It is better not to think about / it... | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1051 | 2 | kein | System/Untersuchen | I need a fuse to run the shutter. | - | MOD-TEXT | ohne Aufruf; Sicherungs-Motiv wie D403 #0, anderer Raum |
| 1051 | 3 | LEBT | System/Untersuchen | Just scattered papers and books.Nothing relevant. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1051 | 4 | LEBT | System/Untersuchen | A blackboard full of reports of riots / and cannibalism at points in the city. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1051 | 5 | LEBT | System/Untersuchen | He's got a big bite on his neck, he / didn't give him a chance to avoid it.The weapon can be useful.Sorry, I'll just borrow this... | - | MOD-TEXT | toter Polizist mit Waffe, Mod-Raum |
| 1060 | 0 | LEBT | System/Untersuchen | Only the lights of the / buildings are visible. | - | MOD-TEXT | Fensterblick, Mod |
| 1061 | 0 | LEBT | System/Untersuchen | Only the lights of the / buildings are visible. | - | MOD-TEXT | Fensterblick, Mod |
| 1070 | 0 | LEBT | System/Untersuchen | The number of desks far outnumbers / the amount of office chairs...Were these their solution? | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 1 | kein | System/Untersuchen | The seats contra chairs almost makes  / musical chairs seem like the most fairmethod of ensuring everyone had / an even chance getting good seat for sits. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 2 | LEBT | System/Untersuchen | A cabinet full of binders containing / police reports.Nothing useful. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 3 | LEBT | System/Untersuchen | These boxes contain nothing of use. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 4 | LEBT | System/Untersuchen | The lockers all appear to be locked. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 5 | kein | System/Untersuchen | A work schedule of some sort. / Guess "L.S.K." lucked out this week. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 6 | kein | System/Untersuchen | The work schedule for this week. / This really wasn't my week. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 7 | LEBT | System/Untersuchen | A lot of notes and reminders regarding / ongoing cases.Most appears to be missing people / or random assault cases. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 8 | LEBT | System/Untersuchen | A messy desk. Nothing useful here. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 9 | LEBT | System/Untersuchen | I'd hate sitting this close to the door, / even if I had that computer at my desk. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 10 | kein | System/Untersuchen | During times like these, I almost miss / filling out paperwork at a desk like this. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 11 | LEBT | System/Untersuchen | A tidy desk with an electric fan. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 12 | LEBT | System/Untersuchen | These desks are tidy, but the fan on / the other desk is clearly a neusance. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 13 | kein | System/Untersuchen | Going by the shelf, whoever used this / desk probably enjoyed privacy. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 14 | kein | System/Untersuchen | Like the other, this desk too contains / documents for ongoing investigations. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 15 | kein | System/Untersuchen | These desks all appear quite messy / and seem to contain nothing of use. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 16 | kein | System/Untersuchen | The desks on this row all seem to / be filled with paperwork. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 17 | kein | System/Untersuchen | These desks look strangely empty. / Guess they were unoccupied. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 18 | LEBT | System/Untersuchen | You can get a good overview of the / lobby area from here. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 19 | kein | System/Untersuchen | A police officer. He's dead... | - | MOD-TEXT | Disc-Datei ohne Text |
| 1070 | 20 | LEBT | System/Untersuchen | It's an phone.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Speicher-Hinweis der Mod-Basis („preview“); Disc kann nicht speichern |
| 1071 | 0 | LEBT | System/Untersuchen | The number of desks far outnumbers / the amount of office chairs...Were these their solution? | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 1 | kein | System/Untersuchen | The seats contra chairs almost makes  / musical chairs seem like the most fairmethod of ensuring everyone had / an even chance getting good seat for sits. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 2 | LEBT | System/Untersuchen | A cabinet full of binders containing / police reports.Nothing useful. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 3 | LEBT | System/Untersuchen | These boxes contain nothing of use. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 4 | LEBT | System/Untersuchen | The lockers all appear to be locked. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 5 | kein | System/Untersuchen | A work schedule of some sort. / Guess "L.S.K." lucked out this week. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 6 | kein | System/Untersuchen | The work schedule for this week. / This really wasn't my week. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 7 | LEBT | System/Untersuchen | A lot of notes and reminders regarding / ongoing cases.Most appears to be missing people / or random assault cases. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 8 | LEBT | System/Untersuchen | A messy desk. Nothing useful here. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 9 | LEBT | System/Untersuchen | I'd hate sitting this close to the door, / even if I had that computer at my desk. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 10 | kein | System/Untersuchen | During times like these, I almost miss / filling out paperwork at a desk like this. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 11 | LEBT | System/Untersuchen | A tidy desk with an electric fan. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 12 | LEBT | System/Untersuchen | These desks are tidy, but the fan on / the other desk is clearly a neusance. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 13 | kein | System/Untersuchen | Going by the shelf, whoever used this / desk probably enjoyed privacy. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 14 | kein | System/Untersuchen | Like the other, this desk too contains / documents for ongoing investigations. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 15 | kein | System/Untersuchen | These desks all appear quite messy / and seem to contain nothing of use. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 16 | kein | System/Untersuchen | The desks on this row all seem to / be filled with paperwork. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 17 | kein | System/Untersuchen | These desks look strangely empty. / Guess they were unoccupied. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 18 | LEBT | System/Untersuchen | You can get a good overview of the / lobby area from here. | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 19 | kein | System/Untersuchen | A police officer. He's dead... | - | MOD-TEXT | Disc-Datei ohne Text |
| 1071 | 20 | LEBT | System/Untersuchen | It's an phone.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Speicher-Hinweis der Mod-Basis („preview“); Disc kann nicht speichern |
| 10C0 | 0 | LEBT | System/Untersuchen | It's an elevator.Pressing the button has no effect.It seems the power isn't turned on. | - | MOD-TEXT | Strom-Rätsel der Mod (Bedingung Ck(4,243)); Anklang D307 #15 (Fabrik-Aufzug ohne Strom) |
| 10C0 | 1 | LEBT | System/Untersuchen | Only the lights of the / buildings are visible. | - | MOD-TEXT | Fensterblick, Mod |
| 10C0 | 2 | LEBT | System/Untersuchen | Wanted posters and reports, / nothing relevant. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10C0 | 3 | LEBT | System/Untersuchen | The emergency channel is / down... | - | MOD-TEXT | Notkanal tot; passt zu D102 #4, keine Vorlage |
| 10C0 | 4 | LEBT | System/Untersuchen | Nothing useful. | - | MOD-TEXT | Floskel; gleichlautend D501 #38 „Nichts Brauchbares“ (Labor), kein Ortsbezug |
| 10C0 | 5 | LEBT | System/Untersuchen | Best Midwest Winner K-9 Unit / for R.P.D. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10C0 | 6 | LEBT | System/Untersuchen | Papers of various cases and / forms, nothing important. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10C0 | 7 | LEBT | (ohne Präfix, unklar) |  / ... | - | MOD-TEXT | nur Auslassungspunkte |
| 10C1 | 0 | LEBT | System/Untersuchen | It's an elevator.Pressing the button has no effect.It seems the power isn't turned on. | - | MOD-TEXT | Strom-Rätsel der Mod (Bedingung Ck(4,243)); Anklang D307 #15 (Fabrik-Aufzug ohne Strom) |
| 10C1 | 1 | LEBT | System/Untersuchen | Only the lights of the / buildings are visible. | - | MOD-TEXT | Fensterblick, Mod |
| 10C1 | 2 | LEBT | System/Untersuchen | Wanted posters and reports, / nothing relevant. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10C1 | 3 | LEBT | System/Untersuchen | The emergency channel is / down... | - | MOD-TEXT | Notkanal tot; passt zu D102 #4, keine Vorlage |
| 10C1 | 4 | LEBT | System/Untersuchen | Nothing useful. | - | MOD-TEXT | Floskel; gleichlautend D501 #38 „Nichts Brauchbares“ (Labor), kein Ortsbezug |
| 10C1 | 5 | LEBT | System/Untersuchen | Best Midwest Winner K-9 Unit / for R.P.D. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10C1 | 6 | LEBT | System/Untersuchen | Papers of various cases and / forms, nothing important. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10C1 | 7 | LEBT | (ohne Präfix, unklar) |  / ... | - | MOD-TEXT | nur Auslassungspunkte |
| 10D0 | 0 | LEBT | System/Untersuchen | Enter the first number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 10D0 | 1 | LEBT | System/Untersuchen | Enter the second number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 10D0 | 2 | LEBT | System/Untersuchen | Enter the third number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 10D0 | 3 | LEBT | System/Untersuchen | Enter the fourth number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 10D0 | 4 | LEBT | System/Untersuchen | Wrong code, try again. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 10D0 | 5 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 (Baustein) | SINNGEMAESS | Disc: „Mit der ID-Karte entriegelt“; hier ohne Mittel |
| 10D0 | 6 | LEBT | System/Untersuchen | Communication Room"It's electronically / locked.There's a card reader / on the right. | D400 #0 (Baustein) | ABGEWANDELT | Kern „verriegelt, Karte nötig“; Raumschild und Kartenleser rechts sind Mod (Kartenrätsel README Z. 163-165) |
| 10D0 | 7 | LEBT | System/Untersuchen | A card reader.You have to use the Blue Keycard / and insert four digits tounlock the door.Will you operate the / card reader?  | - | MOD-TEXT | Kartencode-Rätsel Mod (README Z. 163-165); nur die Frageform ähnelt D400 #1 „ID-Karte benutzen?“ |
| 10D0 | 8 | LEBT | System/Untersuchen | You have not the Blue Keycard / to operate it. | - | MOD-TEXT | Mod-Karte; Aussage ähnelt D40A #3 „zum Öffnen ID-Karte nötig“ |
| 10D0 | 9 | LEBT | System/Untersuchen | You've used the Blue Keycard. | - | MOD-TEXT | Mod-Karte; Form wie D301 #1 |
| 10D0 | 10 | LEBT | System/Untersuchen | Left: First Aid Room, Communication / Room. Right: Archive Room. | - | MOD-TEXT | Wegweiser; Schildform wie D504 #0 („Links: … Rechts: …“), Inhalt Mod |
| 10D0 | 11 | LEBT | System/Untersuchen | It's a shutter switch. / Will you push it?  | D207 #0 (Baustein) | WOERTLICH | Wortlaut = D207 #0 „Ein Schalter für das Rolltor. Drücken?“; Platz auf der Disc nicht belegt |
| 10D0 | 12 | LEBT | ??? | ???: Freeze! | - | MOD-TEXT | Marvin-Szene 10D (README Z. 198, Texte Juvenal): ERSTES Treffen Leon–Marvin in einem Mod-Raum |
| 10D0 | 13 | LEBT | Leon | Leon: Wait don't shoot! | - | MOD-TEXT | Marvin-Szene Mod |
| 10D0 | 14 | LEBT | Marvin | Marvin: Leon... You're still alive! | D11B #10 | ABGEWANDELT | Begrüßung wie D11B #10 („Leon! Du lebst!“), hier in Mod-Raum 10D und früher; ob abgeleitet, offen |
| 10D0 | 15 | LEBT | Leon | Leon: I'm glad to see that you're / still alive Marvin. | - | MOD-TEXT | Marvin-Szene Mod |
| 10D0 | 16 | LEBT | Leon | Leon: Where are the rest of the / officers? | - | MOD-TEXT | Marvin-Szene Mod |
| 10D0 | 17 | LEBT | Leon | Leon: Has everyone been eaten by / zombies? or have they turned into / zombies? | - | MOD-TEXT | kein Disc-Text; SPÄTSTAND #084 gleiche Aussage (みんなゾンビに[..] / それとも、ゾンビになっているかだよ。), dort Aussagesatz eines hockenden Mannes (wohl Marvin), Mod macht daraus Leons Frage |
| 10D0 | 18 | LEBT | Marvin | Marvin: You've already seen him, / I thought he was the only one left at | - | MOD-TEXT | Marvin-Szene Mod |
| 10D0 | 19 | LEBT | (ohne Präfix, Fortsetzung Marvin) | the station, tell me, are there other / survivors? | - | MOD-TEXT | Fortsetzung #18 |
| 10D0 | 20 | LEBT | Leon | Leon: You are the only one who I found, / the chief is injured, one of those / things bit him. | - | MOD-TEXT | NEUER FAKT: Chef wurde gebissen (Disc 115 nur „Mir geht es gut“) |
| 10D0 | 21 | LEBT | Marvin | Marvin: Damn, the situation is terrible, / we have to find a way out of here. | - | MOD-TEXT | Marvin-Szene Mod |
| 10D0 | 22 | LEBT | Leon | Leon: The chief told me about using the / communication system and asking for / outside help. | - | MOD-TEXT | Marvin-Szene Mod: Funkanlage-Plan (wie 1150 #7) |
| 10D0 | 23 | LEBT | Marvin | Marvin: But the system is damaged, if / the other officers could not repair it / as only we can? | - | MOD-TEXT | Marvin-Szene Mod |
| 10D0 | 24 | LEBT | Leon | Leon: I don't know, it remains to try, / the helicopter abandoned us, the phones | - | MOD-TEXT | Hubschrauber weg (vgl. 1170), Mod |
| 10D0 | 25 | LEBT | (ohne Präfix, Fortsetzung Leon) | are down and with the building besieged / by those things, I don't see any more / options. | - | MOD-TEXT | Fortsetzung #24 |
| 10D0 | 26 | LEBT | Marvin | Marvin: Okay, I'll meet with the chief / first and we'll start looking for a way / to fix it. | - | MOD-TEXT | Marvin geht zum Chef, Mod |
| 10D0 | 27 | LEBT | Leon | Leon: I agree, he's in his office. | - | MOD-TEXT | Marvin-Szene Mod |
| 10D1 | 0 | LEBT | System/Untersuchen | Enter the first number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 10D1 | 1 | LEBT | System/Untersuchen | Enter the second number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 10D1 | 2 | LEBT | System/Untersuchen | Enter the third number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 10D1 | 3 | LEBT | System/Untersuchen | Enter the fourth number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 10D1 | 4 | LEBT | System/Untersuchen | Wrong code, try again. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 10D1 | 5 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 (Baustein) | SINNGEMAESS | Disc: „Mit der ID-Karte entriegelt“; hier ohne Mittel |
| 10D1 | 6 | LEBT | System/Untersuchen | Communication Room"It's electronically / locked.There's a card reader / on the right. | D400 #0 (Baustein) | ABGEWANDELT | Kern „verriegelt, Karte nötig“; Raumschild und Kartenleser rechts sind Mod (Kartenrätsel README Z. 163-165) |
| 10D1 | 7 | LEBT | System/Untersuchen | A card reader.You have to use the Blue Keycard / and insert four digits tounlock the door.Will you operate the / card reader?  | - | MOD-TEXT | Kartencode-Rätsel Mod (README Z. 163-165); nur die Frageform ähnelt D400 #1 „ID-Karte benutzen?“ |
| 10D1 | 8 | LEBT | System/Untersuchen | You have not the Blue Keycard / to operate it. | - | MOD-TEXT | Mod-Karte; Aussage ähnelt D40A #3 „zum Öffnen ID-Karte nötig“ |
| 10D1 | 9 | LEBT | System/Untersuchen | You've used the Blue Keycard. | - | MOD-TEXT | Mod-Karte; Form wie D301 #1 |
| 10D1 | 10 | LEBT | System/Untersuchen | Left: First Aid Room, Communication / Room. Right: Archive Room. | - | MOD-TEXT | Wegweiser; Schildform wie D504 #0 („Links: … Rechts: …“), Inhalt Mod |
| 10D1 | 11 | LEBT | System/Untersuchen | It's a shutter switch. / Will you push it?         /   ,      3    | D207 #0 (Baustein) | WOERTLICH | wie D207 #0; im Mod-Text hängen Restbytes an |
| 10E0 | 0 | LEBT | System/Untersuchen | It's the body of a doctor.He has no vital signs. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10E0 | 1 | LEBT | System/Untersuchen | An old typewriter.Why use it if there are / computers? | - | MOD-TEXT | Mod-Text |
| 10E0 | 2 | LEBT | System/Untersuchen | September 1998. | - | MOD-TEXT | Datum September 1998 (RE2), Disc ohne |
| 10E0 | 3 | LEBT | System/Untersuchen | Various medicines and bandages. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10E0 | 4 | LEBT | System/Untersuchen | It's a photo of the Arklay / Mountains. | - | MOD-TEXT | Arklay-Anspielung (RE1), Disc ohne |
| 10E1 | 0 | LEBT | System/Untersuchen | It's the body of a doctor.He has no vital signs. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10E1 | 1 | LEBT | System/Untersuchen | An old typewriter.Why use it if there are / computers? | - | MOD-TEXT | Mod-Text |
| 10E1 | 2 | LEBT | System/Untersuchen | September 1998. | - | MOD-TEXT | Datum September 1998 (RE2), Disc ohne |
| 10E1 | 3 | LEBT | System/Untersuchen | Various medicines and bandages. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10E1 | 4 | LEBT | System/Untersuchen | It's a photo of the Arklay / Mountains. | - | MOD-TEXT | Arklay-Anspielung (RE1), Disc ohne |
| 10F0 | 0 | LEBT | System/Untersuchen | A large communications device.It's out of commission.I can't send any messages from / here. | - | MOD-TEXT | Funkanlage außer Betrieb; passt zu D102 #4, keine Vorlage |
| 10F0 | 1 | LEBT | System/Untersuchen | Nothing suspicious nor / out of the ordinary. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10F0 | 2 | LEBT | System/Untersuchen | There isnt anything / useful on this shelf. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10F0 | 3 | LEBT | System/Untersuchen | There are various devices.This must be where they / gathered information. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10F0 | 4 | LEBT | System/Untersuchen | Several files from past / cases.Nothing useful here. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10F0 | 5 | LEBT | System/Untersuchen | It's a small office.It's locked. | D306 #0 (Baustein) | ABGEWANDELT | Floskel „abgeschlossen“ + kleines Büro; Raum nur im Mod-Baum |
| 10F1 | 0 | LEBT | System/Untersuchen | A large communications device.It's out of commission.I can't send any messages from / here. | - | MOD-TEXT | Funkanlage außer Betrieb; passt zu D102 #4, keine Vorlage |
| 10F1 | 1 | LEBT | System/Untersuchen | Nothing suspicious nor / out of the ordinary. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10F1 | 2 | LEBT | System/Untersuchen | There isnt anything / useful on this shelf. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10F1 | 3 | LEBT | System/Untersuchen | There are various devices.This must be where they / gathered information. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10F1 | 4 | LEBT | System/Untersuchen | Several files from past / cases.Nothing useful here. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 10F1 | 5 | LEBT | System/Untersuchen | It's a small office.It's locked. | D306 #0 (Baustein) | ABGEWANDELT | Floskel „abgeschlossen“ + kleines Büro; Raum nur im Mod-Baum |
| 1100 | 0 | LEBT | System/Untersuchen | It's electronically locked. | D40A #3 (Baustein) | SINNGEMAESS | „げん重にロック“ = streng verriegelt; Kartenhinweis fehlt |
| 1100 | 1 | LEBT | System/Untersuchen | It's a control panel.A type of voice device is required / to open the lock. | - | MOD-TEXT | Sprachschloss Mod (README Z. 267) |
| 1100 | 2 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 (Baustein) | SINNGEMAESS | Disc: „Mit der ID-Karte entriegelt“; hier ohne Mittel |
| 1100 | 3 | LEBT | System/Untersuchen | Will you use the Minidisc Player w/ Disc?  | - | MOD-TEXT | Minidisc Mod (README Z. 267); Frageform wie D403 #1 |
| 1100 | 4 | LEBT | System/Untersuchen | You've used the Minidisc Player w/ Disc. | - | MOD-TEXT | Minidisc Mod (README Z. 267) |
| 1100 | 5 | LEBT | System/Untersuchen | I don't need to use this anymore. | - | MOD-TEXT | Mod |
| 1101 | 0 | LEBT | System/Untersuchen | It's electronically locked. | D40A #3 (Baustein) | SINNGEMAESS | „げん重にロック“ = streng verriegelt; Kartenhinweis fehlt |
| 1101 | 1 | LEBT | System/Untersuchen | It's a control panel.A type of voice device is required / to open the lock. | - | MOD-TEXT | Sprachschloss Mod (README Z. 267) |
| 1101 | 2 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 (Baustein) | SINNGEMAESS | Disc: „Mit der ID-Karte entriegelt“; hier ohne Mittel |
| 1101 | 3 | LEBT | System/Untersuchen | Will you use the Minidisc Player w/ Disc?  | - | MOD-TEXT | Minidisc Mod (README Z. 267); Frageform wie D403 #1 |
| 1101 | 4 | LEBT | System/Untersuchen | You've used the Minidisc Player w/ Disc. | - | MOD-TEXT | Minidisc Mod (README Z. 267) |
| 1101 | 5 | LEBT | System/Untersuchen | I don't need to use this anymore. | - | MOD-TEXT | Mod |
| 1110 | 0 | LEBT | System/Untersuchen | It's a police officer, he's dead.He is holding a slip.The numbers 4312" are / printed on the slip. | - | MOD-TEXT | Code 4312 (README Z. 165) |
| 1110 | 1 | LEBT | System/Untersuchen | Files on various cases.None appear to be useful. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1110 | 2 | LEBT | System/Untersuchen | Visual materials. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1110 | 3 | LEBT | System/Untersuchen | A pile of investigation reports. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1110 | 4 | LEBT | System/Untersuchen | Weapons seized, those that remain / are unusable. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1110 | 5 | LEBT | System/Untersuchen | An unarmed carbine, I have no idea / how to fix it. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1110 | 6 | LEBT | System/Untersuchen | A rocket launcher, has something / written on it:Evidence of the Arklay incident, / do not manipulate. | - | MOD-TEXT | Arklay-Anspielung (RE1), Disc ohne |
| 1110 | 7 | LEBT | System/Untersuchen | Ammunition boxes seized, they've / all taken it. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1110 | 8 | LEBT | System/Untersuchen | Will you take the Time Bomb?  | - | MOD-TEXT | Mod-Gegenstand „Time Bomb“; Frageform wie D119 #1 |
| 1110 | 9 | LEBT | System/Untersuchen | You've taken the Time Bomb. | - | MOD-TEXT | Mod-Gegenstand |
| 1111 | 0 | LEBT | System/Untersuchen | It's a police officer, he's dead.He is holding a slip.The numbers 4312" are / printed on the slip. | - | MOD-TEXT | Code 4312 (README Z. 165) |
| 1111 | 1 | LEBT | System/Untersuchen | Files on various cases.None appear to be useful. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1111 | 2 | LEBT | System/Untersuchen | Visual materials. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1111 | 3 | LEBT | System/Untersuchen | A pile of investigation reports. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1111 | 4 | LEBT | System/Untersuchen | Weapons seized, those that remain / are unusable. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1111 | 5 | LEBT | System/Untersuchen | An unarmed carbine, I have no idea / how to fix it. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1111 | 6 | LEBT | System/Untersuchen | A rocket launcher, has something / written on it:Evidence of the Arklay incident, / do not manipulate. | - | MOD-TEXT | Arklay-Anspielung (RE1), Disc ohne |
| 1111 | 7 | LEBT | System/Untersuchen | Ammunition boxes seized, they've / all taken it. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1111 | 8 | LEBT | System/Untersuchen | Will you take the Time Bomb?  | - | MOD-TEXT | Mod-Gegenstand „Time Bomb“; Frageform wie D119 #1 |
| 1111 | 9 | LEBT | System/Untersuchen | You've taken the Time Bomb. | - | MOD-TEXT | Mod-Gegenstand |
| 1120 | 0 | LEBT | System/Untersuchen | It's an elevator.Pressing the button has no effect.It seems the power isn't turned on. | - | MOD-TEXT | Strom-Rätsel der Mod (Bedingung Ck(4,243)); Anklang D307 #15 (Fabrik-Aufzug ohne Strom) |
| 1120 | 1 | kein | System/Untersuchen | A painting, titled: / The Music Lesson" | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1120 | 2 | kein | System/Untersuchen | A painting of 5 women's. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1120 | 3 | kein | System/Untersuchen | Only the lights of the / buildings are visible. | - | MOD-TEXT | Fensterblick, Mod |
| 1120 | 4 | kein | System/Untersuchen | Various reports and case / information.Nothing useful. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1120 | 5 | kein | System/Untersuchen | A strange Christmas toy. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1120 | 6 | LEBT | System/Untersuchen | It's a phone.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Speicher-Hinweis der Mod-Basis („preview“); Disc kann nicht speichern |
| 1120 | 7 | kein | (ohne Präfix, unklar) | ... | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1120 | 8 | kein | System/Untersuchen | An advertisement for an arm / wrestling contest.The winner will be named / Chief for a Day of theRaccoon Police Department!" | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1121 | 0 | LEBT | System/Untersuchen | It's an elevator.Pressing the button has no effect.It seems the power isn't turned on. | - | MOD-TEXT | Strom-Rätsel der Mod (Bedingung Ck(4,243)); Anklang D307 #15 (Fabrik-Aufzug ohne Strom) |
| 1121 | 1 | kein | System/Untersuchen | A painting, titled: / The Music Lesson" | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1121 | 2 | kein | System/Untersuchen | A painting of 5 women's. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1121 | 3 | kein | System/Untersuchen | Only the lights of the / buildings are visible. | - | MOD-TEXT | Fensterblick, Mod |
| 1121 | 4 | kein | System/Untersuchen | Various reports and case / information.Nothing useful. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1121 | 5 | kein | System/Untersuchen | A strange Christmas toy. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1121 | 6 | LEBT | System/Untersuchen | It's a phone.You can save your progress / with this.Save is not available in / this preview. | - | MOD-TEXT | Speicher-Hinweis der Mod-Basis („preview“); Disc kann nicht speichern |
| 1121 | 7 | kein | (ohne Präfix, unklar) | ... | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1121 | 8 | kein | System/Untersuchen | An advertisement for an arm / wrestling contest.The winner will be named / Chief for a Day of theRaccoon Police Department!" | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1130 | 0 | LEBT | System/Untersuchen | It's a shutter switch. / Will you push it?  | D207 #0 (Baustein) | WOERTLICH | Wortlaut = D207 #0 „Ein Schalter für das Rolltor. Drücken?“; Platz auf der Disc nicht belegt |
| 1130 | 1 | LEBT | System/Untersuchen | It's not necessary to go back. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1130 | 2 | LEBT | System/Untersuchen | A photo of the members of / S.T.A.R.S. | - | MOD-TEXT | S.T.A.R.S.-Anspielung (RE1), Disc ohne |
| 1130 | 3 | LEBT | System/Untersuchen | An emblem of the Raccoon / S.W.A.T Team. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1130 | 4 | LEBT | System/Untersuchen | A painting of a man with a hook, / seeing it makes me uncomfortable. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1130 | 5 | LEBT | System/Untersuchen | A shelf with trophies.Among them is the award for best / shooter, Chris Redfield. | - | MOD-TEXT | nennt Chris Redfield (RE1), Disc ohne |
| 1131 | 0 | LEBT | System/Untersuchen | It's a shutter switch. / Will you push it?  | D207 #0 (Baustein) | WOERTLICH | Wortlaut = D207 #0 „Ein Schalter für das Rolltor. Drücken?“; Platz auf der Disc nicht belegt |
| 1131 | 1 | kein | System/Untersuchen | It's not necessary to go back. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1131 | 2 | LEBT | System/Untersuchen | A photo of the members of / S.T.A.R.S. | - | MOD-TEXT | S.T.A.R.S.-Anspielung (RE1), Disc ohne |
| 1131 | 3 | LEBT | System/Untersuchen | An emblem of the Raccoon / S.W.A.T Team. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1131 | 4 | LEBT | System/Untersuchen | A painting of a man with a hook, / seeing it makes me uncomfortable. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1131 | 5 | LEBT | System/Untersuchen | A shelf with trophies.Among them is the award for best / shooter, Chris Redfield. | - | MOD-TEXT | nennt Chris Redfield (RE1), Disc ohne |
| 1140 | 0 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 (Baustein) | SINNGEMAESS | Disc: „Mit der ID-Karte entriegelt“; hier ohne Mittel |
| 1140 | 1 | LEBT | System/Untersuchen | A beautiful painting.There's a switch below. / Will you push it?  | D119 #0 (Baustein) | ABGEWANDELT | Schalter-Frage + Gemälde; Gemälde-Schalter ist Mod-Rätsel |
| 1140 | 2 | LEBT | System/Untersuchen | It's a big table. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1140 | 3 | kein | System/Untersuchen | The streets are infested with / zombies! | - | MOD-TEXT | Fensterblick, ohne Aufruf |
| 1140 | 4 | LEBT | System/Untersuchen | The streets are infested with / zombies! | - | MOD-TEXT | Fensterblick, Mod-Raum |
| 1140 | 5 | LEBT | System/Untersuchen | The switch is not responding... | - | MOD-TEXT | Gemälde-Schalter Mod |
| 1141 | 0 | LEBT | Elza | Elza: John, wait! | - | MOD-TEXT | John-Szene in Mod-Raum 114 (README Z. 338); Floskel wie 10B1 #0 / D10B #0 |
| 1141 | 1 | LEBT | John | John: Elza! sorry, I'm glad to see / you're still alive... | - | MOD-TEXT | John-Szene Mod: Elza und John waren getrennt |
| 1141 | 2 | LEBT | Elza | Elza: Same here... are you okay? / what's happened? | - | MOD-TEXT | John-Szene Mod |
| 1141 | 3 | LEBT | John | John: Somehow... Elza, listen to me! | - | MOD-TEXT | John-Szene Mod |
| 1141 | 4 | LEBT | Elza | Elza: What is wrong? | - | MOD-TEXT | John-Szene Mod |
| 1141 | 5 | LEBT | John | John: Don't go to the rooftop! / ...there's something...a somewhat / dangerous monster! | - | MOD-TEXT | kein Disc-Text; SPÄTSTAND #012 (Raum 114) エルザ！屋上に行っちゃダメだ！ / [ヤ]バイ怪物がいやがる, gleiche Aussage im gleichen Raum. Disc 117: John ruft Elza aufs Dach |
| 1141 | 6 | LEBT | Elza | Elza: What was that?! | - | MOD-TEXT | John-Szene Mod |
| 1141 | 7 | LEBT | John | John: Shit! that is what I was / talking about! | - | MOD-TEXT | John-Szene Mod |
| 1141 | 8 | LEBT | Elza | Elza: what the hell is that?! | - | MOD-TEXT | John-Szene Mod; Spätstand #013/#018 zeigen ein rotes Riesenwesen in 114, ohne Text |
| 1141 | 9 | LEBT | John | John: Looks like some kind of gorilla, / that would explain how they managed / to enter through the windows! | - | MOD-TEXT | NEU: „Gorilla“ als Erklärung für Einbruch durch Fenster |
| 1141 | 10 | LEBT | Elza | Elza: John! what will we do now?! | - | MOD-TEXT | John-Szene Mod |
| 1141 | 11 | LEBT | John | John: Elza, I'll try to distract them, / while you try to get out of here, hurry! | - | MOD-TEXT | NEU: John lenkt ab, Elza flieht allein; Motiv wie D400 #5 (Leon, tot) |
| 1141 | 12 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 (Baustein) | SINNGEMAESS | Disc: „Mit der ID-Karte entriegelt“; hier ohne Mittel |
| 1141 | 13 | LEBT | System/Untersuchen | A beautiful painting.There's a switch below. / Will you push it?  | D119 #0 (Baustein) | ABGEWANDELT | Schalter-Frage + Gemälde; Gemälde-Schalter ist Mod-Rätsel |
| 1141 | 14 | LEBT | System/Untersuchen | It's a big table. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1180 | 0 | LEBT | System/Untersuchen | Weapon Storage"It's electronically / locked.There's a card reader / on the right. | D400 #0 (Baustein) | ABGEWANDELT | Kern „verriegelt, Karte nötig“; Raumschild und Kartenleser rechts sind Mod (Kartenrätsel README Z. 163-165) |
| 1180 | 1 | LEBT | System/Untersuchen | A card reader.It seems that it will not / work if power is not restored. | - | MOD-TEXT | Strom-Rätsel Mod; Anklang D307 #15 |
| 1180 | 2 | LEBT | (ohne Präfix, unklar) |  / ... | - | MOD-TEXT | Disc-Datei ohne Text |
| 1181 | 0 | LEBT | System/Untersuchen | Weapon Storage"It's electronically / locked.There's a card reader / on the right. | D400 #0 (Baustein) | ABGEWANDELT | Kern „verriegelt, Karte nötig“; Raumschild und Kartenleser rechts sind Mod (Kartenrätsel README Z. 163-165) |
| 1181 | 1 | LEBT | System/Untersuchen | A card reader.It seems that it will not / work if power is not restored. | - | MOD-TEXT | Strom-Rätsel Mod; Anklang D307 #15 |
| 1181 | 2 | LEBT | (ohne Präfix, unklar) |  / ... | - | MOD-TEXT | Disc-Datei ohne Text |
| 11A0 | 0 | LEBT | System/Untersuchen | There's a shutter switch here. Press it?  | D207 #0 (Baustein) | WOERTLICH | Wortlaut = D207 #0; Raum-Datei auf der Disc ohne Text |
| 11A0 | 1 | LEBT | System/Untersuchen | It's locked. | D306 #0 (Baustein) | WOERTLICH | Floskel „Abgeschlossen.“; Platz nicht belegt |
| 11A0 | 2 | LEBT | System/Untersuchen | It's locked from the other side. | D306 #0 (Baustein) | ABGEWANDELT | Floskel + „von der anderen Seite“ (RE2-Formel, nicht auf der Disc) |
| 11A1 | 0 | LEBT | System/Untersuchen | There's a shutter switch here. Press it?  | D207 #0 (Baustein) | WOERTLICH | Wortlaut = D207 #0; Raum-Datei auf der Disc ohne Text |
| 11A1 | 1 | LEBT | System/Untersuchen | It's locked. | D306 #0 (Baustein) | WOERTLICH | Floskel „Abgeschlossen.“; Platz nicht belegt |
| 11A1 | 2 | LEBT | System/Untersuchen | It's locked from the other side. | D306 #0 (Baustein) | ABGEWANDELT | Floskel + „von der anderen Seite“ (RE2-Formel, nicht auf der Disc) |
| 11D0 | 0 | LEBT | System/Untersuchen | A panel to control the cells of / the dogs.It does not work. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 11D0 | 1 | LEBT | System/Untersuchen | The road is blocked and the knob / is broken. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 11D0 | 2 | LEBT | System/Untersuchen | Boxes and dog food.Nothing useful. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 11D0 | 3 | LEBT | System/Untersuchen | Raccoon Police wins the award for / the best K-9 unit in the Midwest. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 11D0 | 4 | LEBT | System/Untersuchen | -Warning-Sewer access area. / Authorized personal only. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 11D1 | 0 | LEBT | System/Untersuchen | A panel to control the cells of / the dogs.It does not work. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 11D1 | 1 | LEBT | System/Untersuchen | The road is blocked and the knob / is broken. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 11D1 | 2 | LEBT | System/Untersuchen | Boxes and dog food.Nothing useful. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 11D1 | 3 | LEBT | System/Untersuchen | Raccoon Police wins the award for / the best K-9 unit in the Midwest. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 11D1 | 4 | LEBT | System/Untersuchen | -Warning-Sewer access area. / Authorized personal only. | - | MOD-TEXT | Raum nur im Mod-Baum |
| 11E0 | 0 | LEBT | System/Untersuchen | Enter the first number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 11E0 | 1 | LEBT | System/Untersuchen | Enter the second number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 11E0 | 2 | LEBT | System/Untersuchen | Enter the third number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 11E0 | 3 | LEBT | System/Untersuchen | Enter the fourth number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 11E0 | 4 | LEBT | System/Untersuchen | Wrong code, try again. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 11E0 | 5 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 (Baustein) | SINNGEMAESS | Disc: „Mit der ID-Karte entriegelt“; hier ohne Mittel |
| 11E0 | 6 | LEBT | System/Untersuchen | Prisons"It's electronically / locked.There's a card reader / on the right. | D400 #0 (Baustein) | ABGEWANDELT | Kern „verriegelt, Karte nötig“; Raumschild und Kartenleser rechts sind Mod (Kartenrätsel README Z. 163-165) |
| 11E0 | 7 | LEBT | System/Untersuchen | A card reader.You have to use the Yellow Keycard / and insert four digits tounlock the door.Will you operate the / card reader?  | - | MOD-TEXT | Kartencode-Rätsel Mod (README Z. 163-165); nur die Frageform ähnelt D400 #1 „ID-Karte benutzen?“ |
| 11E0 | 8 | LEBT | System/Untersuchen | You have not the Yellow Keycard / to operate it. | - | MOD-TEXT | Mod-Karte; Aussage ähnelt D40A #3 „zum Öffnen ID-Karte nötig“ |
| 11E0 | 9 | LEBT | System/Untersuchen | You've used the Yellow Keycard. | - | MOD-TEXT | Mod-Karte; Form wie D301 #1 |
| 11E0 | 10 | LEBT | System/Untersuchen | If I had some kind of pliers maybe I / could cut that wire and disable thesafety lock. | - | MOD-TEXT | Zange/Rolltor-Rätsel Mod (README Z. 334) |
| 11E0 | 11 | LEBT | System/Untersuchen | Will you use the Pliers?  | - | MOD-TEXT | Zange Mod (README Z. 334) |
| 11E0 | 12 | LEBT | System/Untersuchen | You've used the Pliers. | - | MOD-TEXT | Zange Mod (README Z. 334) |
| 11E1 | 0 | LEBT | System/Untersuchen | Enter the first number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 11E1 | 1 | LEBT | System/Untersuchen | Enter the second number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 11E1 | 2 | LEBT | System/Untersuchen | Enter the third number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 11E1 | 3 | LEBT | System/Untersuchen | Enter the fourth number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 11E1 | 4 | LEBT | System/Untersuchen | Wrong code, try again. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 11E1 | 5 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 (Baustein) | SINNGEMAESS | Disc: „Mit der ID-Karte entriegelt“; hier ohne Mittel |
| 11E1 | 6 | LEBT | System/Untersuchen | Prisons"It's electronically / locked.There's a card reader / on the right. | D400 #0 (Baustein) | ABGEWANDELT | Kern „verriegelt, Karte nötig“; Raumschild und Kartenleser rechts sind Mod (Kartenrätsel README Z. 163-165) |
| 11E1 | 7 | LEBT | System/Untersuchen | A card reader.You have to use the Yellow Keycard / and insert four digits tounlock the door.Will you operate the / card reader?  | - | MOD-TEXT | Kartencode-Rätsel Mod (README Z. 163-165); nur die Frageform ähnelt D400 #1 „ID-Karte benutzen?“ |
| 11E1 | 8 | LEBT | System/Untersuchen | You have not the Yellow Keycard / to operate it. | - | MOD-TEXT | Mod-Karte; Aussage ähnelt D40A #3 „zum Öffnen ID-Karte nötig“ |
| 11E1 | 9 | LEBT | System/Untersuchen | You've used the Yellow Keycard. | - | MOD-TEXT | Mod-Karte; Form wie D301 #1 |
| 11E1 | 10 | LEBT | System/Untersuchen | If I had some kind of pliers maybe I / could cut that wire and disable thesafety lock. | - | MOD-TEXT | Zange/Rolltor-Rätsel Mod (README Z. 334) |
| 11E1 | 11 | LEBT | System/Untersuchen | Will you use the Pliers?  | - | MOD-TEXT | Zange Mod (README Z. 334) |
| 11E1 | 12 | LEBT | System/Untersuchen | You've used the Pliers. | - | MOD-TEXT | Zange Mod (README Z. 334) |
| 11F0 | 0 | LEBT | System/Untersuchen | -Reserve Power / Control Panel-Power can be routed to / the areas withinsufficient power using / these ten switches.Using all the power may / cause a short andcut the power supply. | - | MOD-TEXT | Notstrom-Rätsel Mod (Raum 11F README Z. 58) |
| 11F0 | 1 | LEBT | System/Untersuchen | Will you push them?  | - | MOD-TEXT | Notstrom-Rätsel Mod (Raum 11F README Z. 58); Frageform „Drücken?“ |
| 11F0 | 2 | LEBT | System/Untersuchen | Power supply OK. | - | MOD-TEXT | Notstrom-Rätsel Mod (Raum 11F README Z. 58) |
| 11F0 | 3 | kein | System/Untersuchen | Power supply incorrect. | - | MOD-TEXT | Notstrom-Rätsel Mod (Raum 11F README Z. 58) |
| 11F1 | 0 | LEBT | System/Untersuchen | -Reserve Power / Control Panel-Power can be routed to / the areas withinsufficient power using / these ten switches.Using all the power may / cause a short andcut the power supply. | - | MOD-TEXT | Notstrom-Rätsel Mod (Raum 11F README Z. 58) |
| 11F1 | 1 | LEBT | System/Untersuchen | Will you push them?  | - | MOD-TEXT | Notstrom-Rätsel Mod (Raum 11F README Z. 58); Frageform „Drücken?“ |
| 11F1 | 2 | LEBT | System/Untersuchen | Power supply OK. | - | MOD-TEXT | Notstrom-Rätsel Mod (Raum 11F README Z. 58) |
| 11F1 | 3 | kein | System/Untersuchen | Power supply incorrect. | - | MOD-TEXT | Notstrom-Rätsel Mod (Raum 11F README Z. 58) |
| 1210 | 20 | LEBT (C-AOT) | System/Untersuchen | The cell door is open...Whoever was locked up here is gone. | - | PORT-TEXT | PORT-NEU (story_r36 P2, Nutzer-Zitat), ersetzt keine Mod-Zeile. Stützt sich auf den Disc-Zustand (Zellentür bei Leon offen, BERICHT §4); „Whoever was locked up here“ setzt einen Gefangenen voraus, den die Disc nicht nennt |
| 1211 | 0 | LEBT | Elza | Elza: Hey! You must be John right? / Get up now! | - | MOD-TEXT | John-Szene in 121 (README Z. 278); Disc: John steht in 121 nicht im Skript |
| 1211 | 1 | LEBT | John | John: What do you want? / I'm trying to sleep here! | - | MOD-TEXT | John-Szene Mod: John schläft in der Zelle |
| 1211 | 2 | LEBT | John | John: Ughh... my head is killing me. | - | MOD-TEXT | John-Szene Mod |
| 1211 | 3 | LEBT | John | John: Who the hell are you anyway? | - | MOD-TEXT | John-Szene Mod |
| 1211 | 4 | LEBT | Elza | Elza: My name's Elza. / Roy sent me to get you out of here. | - | MOD-TEXT | NEU: Roy schickt Elza zu John |
| 1211 | 5 | LEBT | Elza | Elza: Listen. We're in serious trouble. / The whole station is crawling with / zombies! | - | MOD-TEXT | John-Szene Mod |
| 1211 | 6 | LEBT | John | John: Say what!? | - | MOD-TEXT | Floskel wie 10B1 #2 / D10B #1 |
| 1211 | 7 | LEBT | Elza | Elza: Roy was injured by one of them. / I left him in the Interrogation Room, but... | - | MOD-TEXT | NEU: Elza ließ Roy im Verhörraum zurück (Mod-Raum 101) |
| 1211 | 8 | LEBT | John | John: The interrogation room? / Alright. I'm going there. | - | MOD-TEXT | John-Szene Mod; erklärt Johns Weg nach 102 |
| 1211 | 9 | LEBT | Elza | Elza: John. Wait! | - | MOD-TEXT | Floskel wie D10B #0 („John, warte!“) |
| 1211 | 10 | LEBT | System/Untersuchen | It's locked.It seems that there is someone / inside... | D306 #0 (Baustein) | ABGEWANDELT | „Abgeschlossen“ + NEU „jemand ist drin“ (John); Disc: Zellentür bei Elza zu, ohne Text |
| 1230 | 0 | LEBT | System/Untersuchen | Enter the first number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 1230 | 1 | LEBT | System/Untersuchen | Enter the second number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 1230 | 2 | LEBT | System/Untersuchen | Enter the third number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 1230 | 3 | LEBT | System/Untersuchen | Enter the fourth number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 1230 | 4 | LEBT | System/Untersuchen | Wrong code, try again. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 1230 | 5 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 (Baustein) | SINNGEMAESS | Disc: „Mit der ID-Karte entriegelt“; hier ohne Mittel |
| 1230 | 6 | LEBT | System/Untersuchen | Weapon Storage"It's electronically / locked.There's a card reader / on the right. | D400 #0 (Baustein) | ABGEWANDELT | Kern „verriegelt, Karte nötig“; Raumschild und Kartenleser rechts sind Mod (Kartenrätsel README Z. 163-165) |
| 1230 | 7 | LEBT | System/Untersuchen | A card reader.You have to use the Red Keycard / and insert four digits tounlock the door.Will you operate the / card reader?  | - | MOD-TEXT | Kartencode-Rätsel Mod (README Z. 163-165); nur die Frageform ähnelt D400 #1 „ID-Karte benutzen?“ |
| 1230 | 8 | LEBT | System/Untersuchen | You have not the Red Keycard / to operate it. | - | MOD-TEXT | Mod-Karte; Aussage ähnelt D40A #3 „zum Öffnen ID-Karte nötig“ |
| 1230 | 9 | LEBT | System/Untersuchen | You've used the Red Keycard. | - | MOD-TEXT | Mod-Karte; Form wie D301 #1 |
| 1230 | 10 | LEBT | System/Untersuchen | A miserable death...He is holding a slip.The numbers 5632" are / printed on the slip. | - | MOD-TEXT | Code 5632 (README Z. 163) |
| 1230 | 11 | LEBT | (ohne Präfix, unklar) |  / ... | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1231 | 0 | LEBT | System/Untersuchen | Enter the first number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 1231 | 1 | LEBT | System/Untersuchen | Enter the second number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 1231 | 2 | LEBT | System/Untersuchen | Enter the third number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 1231 | 3 | LEBT | System/Untersuchen | Enter the fourth number. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 1231 | 4 | LEBT | System/Untersuchen | Wrong code, try again. | - | MOD-TEXT | Zahlenschloss Mod (README Z. 163-165) |
| 1231 | 5 | LEBT | System/Untersuchen | You've opened the lock. | D301 #1 (Baustein) | SINNGEMAESS | Disc: „Mit der ID-Karte entriegelt“; hier ohne Mittel |
| 1231 | 6 | LEBT | System/Untersuchen | Weapon Storage"It's electronically / locked.There's a card reader / on the right. | D400 #0 (Baustein) | ABGEWANDELT | Kern „verriegelt, Karte nötig“; Raumschild und Kartenleser rechts sind Mod (Kartenrätsel README Z. 163-165) |
| 1231 | 7 | LEBT | System/Untersuchen | A card reader.You have to use the Red Keycard / and insert four digits tounlock the door.Will you operate the / card reader?  | - | MOD-TEXT | Kartencode-Rätsel Mod (README Z. 163-165); nur die Frageform ähnelt D400 #1 „ID-Karte benutzen?“ |
| 1231 | 8 | LEBT | System/Untersuchen | You have not the Red Keycard / to operate it. | - | MOD-TEXT | Mod-Karte; Aussage ähnelt D40A #3 „zum Öffnen ID-Karte nötig“ |
| 1231 | 9 | LEBT | System/Untersuchen | You've used the Red Keycard. | - | MOD-TEXT | Mod-Karte; Form wie D301 #1 |
| 1231 | 10 | LEBT | System/Untersuchen | A miserable death...He is holding a slip.The numbers 5632" are / printed on the slip. | - | MOD-TEXT | Code 5632 (README Z. 163) |
| 1231 | 11 | LEBT | (ohne Präfix, unklar) |  / ... | - | MOD-TEXT | Raum nur im Mod-Baum |
| 1240 | 0 | LEBT | (ohne Präfix, Erzähler) | A bizarre incident occurred in the / outskirts of an American suburb called / Raccoon City. | - | MOD-TEXT | Vorspann aus RE2 bzw. RE2-Trial (README Z. 22, 355), Disc ohne; Port-Startraum |
| 1240 | 1 | LEBT | (ohne Präfix, Erzähler) | It was later revealed that the terrible / disaster had been caused by the T-Virus, | - | MOD-TEXT | Vorspann RE2 (README Z. 22, 355); nennt T-Virus, Umbrella, S.T.A.R.S., Chris und Jill — nichts davon auf der Disc |
| 1240 | 2 | LEBT | (ohne Präfix, Erzähler) | a mutagenic toxin created by the / international corporation Umbrella / incorporated. | - | MOD-TEXT | Vorspann RE2 (README Z. 22, 355); nennt T-Virus, Umbrella, S.T.A.R.S., Chris und Jill — nichts davon auf der Disc |
| 1240 | 3 | LEBT | (ohne Präfix, Erzähler) | The Raccoon police department special / S.T.A.R.S Unit began an immediate / investigation. | - | MOD-TEXT | Vorspann RE2 (README Z. 22, 355) |
| 1240 | 4 | LEBT | (ohne Präfix, Erzähler) | The case was apparently closed thanks / to the efforts of S.T.A.R.S members / Chris Redfield and Jill Valentine. | - | MOD-TEXT | Vorspann RE2 (README Z. 22, 355); nennt T-Virus, Umbrella, S.T.A.R.S., Chris und Jill — nichts davon auf der Disc |
| 1240 | 5 | LEBT | (ohne Präfix, Erzähler) | But the umbrella corporation's / experiments were far from finished. | - | MOD-TEXT | Vorspann RE2 (README Z. 22, 355) |
| 1241 | 0 | LEBT | (ohne Präfix, Erzähler) | A bizarre incident occurred in the / outskirts of an American suburb called / Raccoon City. | - | MOD-TEXT | Vorspann aus RE2 bzw. RE2-Trial (README Z. 22, 355), Disc ohne; Port-Startraum |
| 1241 | 1 | LEBT | (ohne Präfix, Erzähler) | It was later revealed that the terrible / disaster had been caused by the T-Virus, | - | MOD-TEXT | Vorspann RE2 (README Z. 22, 355); nennt T-Virus, Umbrella, S.T.A.R.S., Chris und Jill — nichts davon auf der Disc |
| 1241 | 2 | LEBT | (ohne Präfix, Erzähler) | a mutagenic toxin created by the / international corporation Umbrella / incorporated. | - | MOD-TEXT | Vorspann RE2 (README Z. 22, 355); nennt T-Virus, Umbrella, S.T.A.R.S., Chris und Jill — nichts davon auf der Disc |
| 1241 | 3 | LEBT | (ohne Präfix, Erzähler) | The Raccoon police department special / S.T.A.R.S Unit began an immediate / investigation. | - | MOD-TEXT | Vorspann RE2 (README Z. 22, 355) |
| 1241 | 4 | LEBT | (ohne Präfix, Erzähler) | The case was apparently closed thanks / to the efforts of S.T.A.R.S members / Chris Redfield and Jill Valentine. | - | MOD-TEXT | Vorspann RE2 (README Z. 22, 355); nennt T-Virus, Umbrella, S.T.A.R.S., Chris und Jill — nichts davon auf der Disc |
| 1241 | 5 | LEBT | (ohne Präfix, Erzähler) | But the umbrella corporation's / experiments were far from finished. | - | MOD-TEXT | Vorspann RE2 (README Z. 22, 355) |

## T2 — Disc-Zeilen der Räume 1xx

60 Disc-Zeilen in 102, 104, 109, 10B, 115, 117, 119, 11B, 11C. Alle übrigen 1xx-Räume haben auf der Disc keinen Text oder keine Datei.

| Disc | Japanisch | Deutsch | Port-Zeile(n) | Bemerkung |
|---|---|---|---|---|
| D102 #0 | / そんな まさか…ロイ ウソだろ？ | Nein, das kann nicht… Roy, das ist doch nicht wahr? | 1021 #0 | wörtlich |
| D102 #1 | ジョン…助けて… / 俺を 殺してくれ | John… hilf mir… töte mich. | 1021 #1 | wörtlich |
| D102 #2 | ジョン はやく撃って / それはもう ロイじゃないのよ！！ | John, schieß schnell! Das ist nicht mehr Roy!! | 1021 #2 | wörtlich |
| D102 #3 | / ロイ 俺だ… 分かるだろ？ | Roy, ich bin es… du erkennst mich doch? | 1021 #3 | wörtlich |
| D102 #4 | ジョン 通信で助けはよべなかったわ / ビルの電ゲンが落ちてしまって… | John, über Funk konnte ich keine Hilfe rufen. Der Strom im Gebäude ist ausgefallen… | 1021 #4, #5 | auf zwei Zeilen verteilt; „ausgefallen“ -> „starting to fail“ |
| D102 #5 | 私一人じゃどうすればいいのか / 分からないのよ | Allein weiß ich nicht, was ich tun soll. | 1021 #6 | Sprecher Elza -> John |
| D102 #6 | 残った私たちで何とかしないと / このままじゃ私たちだって… | Wir, die noch übrig sind, müssen etwas unternehmen. Sonst werden wir auch noch… | 1021 #7 | sinngemäß |
| D102 #7 | 頼む… 一人に… / …してくれ… | Bitte… lass mich… allein… | 1021 #8 | wörtlich |
| D104 #0 | ボタンがある / 押しますか？ [Ja/Nein] | Da ist ein Knopf. Drücken? | 1040 #0, 1041 #0 | Mod: „shutter switch“ (Wortlaut D207 #0) |
| D109 #0 | 誰か！ 誰かここを開けてくれ！ | Jemand! Jemand soll hier aufmachen! | 1090 #0 | sinngemäß, Präfix „Woman“ |
| D109 #1 | おい！ 中に誰かいるのか？ | He! Ist da drin jemand? | 1090 #1 | sinngemäß |
| D109 #2 | 誰か！ 誰かいないのか！ / いたら開けてくれ！ | Jemand! Ist denn niemand da! Falls doch, macht auf! | 1090 #2 | sinngemäß, gekürzt |
| D109 #3 | さあ！ 大丈夫か？ | So! Alles in Ordnung? | 1090 #3 | wörtlich |
| D109 #4 | 助かった… / ありがとう を言う | Gerettet… Danke, ich danke dir. [Lesart, Lücke 礼] | 1090 #4 | wörtlich |
| D109 #5 | 話は後だな… ここはまずい / それに傷の手当ても必要だろう / 行こう | Reden wir später… hier ist es gefährlich. Und die Wunde muss versorgt werden. Gehen wir. | 1090 #5, #6 | verteilt, Inhalt treu |
| D10B #0 | ジョン 待って まだ 内に女の子が一人 / 取り残されているのよ | John, warte! Im Revier ist noch ein Mädchen zurückgeblieben. [Lesart] | 10B1 #0, #1 | verteilt |
| D10B #1 | 何だって？ | Was? | 10B1 #2 | wörtlich |
| D10B #2 | 地下の 度に じ められていたのよ / 置いては行けないわ | Sie war in der Tiefgarage eingesperrt. Ich kann sie nicht zurücklassen. [Lesart, Lücken 駐車場, 閉, 込] | 10B1 #3 | sinngemäß („found her“) |
| D10B #3 | 地下 度か… だったらこっちの方が社 だぜ / く来な | Die Tiefgarage… dann ist es hier entlang kürzer. Komm schnell. [Lesart] | 10B1 #4, #5 | verteilt; „kürzer“ -> Kanaltunnel (neu) |
| D115 #0 | 私なら大丈夫だ / 今は自分の安全をかんがえろ | Mir geht es gut. Denk jetzt an deine eigene Sicherheit. | 1150 #2, 1151 #2 | wörtlich, Präfix „Irons“ neu |
| D117 #0 | / エルザ はやくしろ！ | Elza, beeil dich! | 1171 #0 | wörtlich |
| D117 #1 | こいつをここからおとすんだ / てつだってくれ！ | Wir müssen das Ding hier runterstoßen! Hilf mir! | 1171 #1 | wörtlich |
| D117 #2 | / さあ てつだってくれ！ | Los, hilf mir! | 1171 #2 | wörtlich; Disc und Port ohne Aufruf |
| D117 #3 | / さあ ここをおりるんだ！ | Los, hier runter! | 1171 #3 | wörtlich |
| D119 #0 | スイッチがある おしますか？ / [Ja/Nein] | Da ist ein Schalter. Drücken? | 1190 #0, 1191 #0 | wörtlich |
| D119 #1 | ドライバーがある とりますか？ / [Ja/Nein] | Da liegt ein Schraubendreher. Nehmen? | 1190 #1, 1191 #1 | wörtlich, aber im Port OHNE Aufruf (Disc ruft auf) |
| D11B #0 | あなた！ こんな所でどうしたの？ | Du! Was machst du denn an so einem Ort? | 11B1 #0 | wörtlich |
| D11B #1 | ここに じこめられているのね？ / こっちへおいで… | Du bist hier eingesperrt, nicht wahr? Komm her… [Lesart] | 11B1 #1 | sinngemäß, „Komm her“ fehlt |
| D11B #2 | あなた なまえは？ | Wie heißt du? | 11B1 #2 | wörtlich |
| D11B #3 | …シェリー | …Sherry. | 11B1 #3 | wörtlich |
| D11B #4 | シェリーっていうの？カワイイなまえね / …私はエルザよ | Sherry heißt du? Ein süßer Name. …Ich bin Elza. | 11B1 #4 | wörtlich |
| D11B #5 | エルザ… | Elza… | 11B1 #5 | wörtlich |
| D11B #6 | そう！…いい？シェリー | Genau! …Hör zu, Sherry. | 11B1 #6 | wörtlich |
| D11B #7 | ここで待ってなさい / 後できっと助けてあげるから | Warte hier. Ich hole dich später ganz sicher raus. | 11B1 #7 | wörtlich |
| D11B #8 | くるまの中でおとなしく待ってるのよ / いいわね？ | Warte brav im Wagen, ja? | 11B1 #8 | wörtlich |
| D11B #9 | お おい！ | H-he! (ruft der vorbeilaufenden Sherry nach) | 11B0 #0 | wörtlich |
| D11B #10 | レオン！無事だったか / あの上の騒ぎはなんだ？ | Leon! Du lebst! Was war das für ein Lärm oben? | 11B0 #1 (Begrüßung auch 10D0 #14) | Disc tot, Port lebt |
| D11B #11 | シャッターが破られた / 奴らつぎつぎと入ってきやがる | Das Rolltor wurde durchbrochen. Sie kommen einer nach dem anderen rein. | 11B0 #2; 1011 #7 (Roy) | Disc tot, Port lebt; zweite Kopie bei Roy in Mod-Raum 101 |
| D11B #12 | 長は？ | Und der Chef? [Lesart, Lücke 署] | 11B0 #3 | Disc tot, Port lebt |
| D11B #13 | そうか…チクショウ！ | Verstehe… verdammt! | 11B0 #4 | Disc tot, Port lebt |
| D11B #14 | 見ただろ？パトじゃもう出られない / どうする？ | Hast du gesehen? Mit dem Streifenwagen kommen wir nicht mehr raus. Was nun? | 11B0 #5 | Disc tot, Port lebt |
| D11B #15 | マーヴイン 来てくれ！ | Marvin, komm her! | 11B0 #6 | Disc tot, Port lebt |
| D11B #16 | この下にマンホールがあるんだ / そこから地下に逃げるってのはどうだ？ | Hier drunter ist ein Kanaldeckel. Wie wäre es, wenn wir dort in den Untergrund fliehen? | 11B0 #7 | Disc tot, Port lebt |
| D11B #17 | やってみる はあるな / よし こいつをどかそう | Einen Versuch ist es wert. Gut, schieben wir das Ding weg. [Lesart, Lücke 価値] | 11B0 #8 | sinngemäß; Disc tot, Port lebt |
| D11B #18 | レオン 手根え！ | Leon, hilf mit! [Lesart] | 11B0 #9 | Disc tot, Port lebt |
| D11B #19 | さぁ 急げ！ | Los, schnell! | 11B0 #10 | Disc tot, Port lebt; Sprecher Marvin |
| D11B #20 | マーヴィン 俺が先に降りる / つぎに女を降ろせ！ | Marvin, ich gehe zuerst runter. Danach lass die Frau runter! | 11B0 #11 | Disc tot, Port lebt |
| D11B #21 | OK / 気をつけろよ！ | OK. Pass auf dich auf! | 11B0 #12 | Disc tot, Port lebt |
| D11B #22 | 1人では押せそうにない | Allein lässt sich das nicht schieben. | 11B0 #13, 11B1 #9 | wörtlich |
| D11C #0 | エイダ！マーヴィンは？ / 一緒じゃなかったのか？ / | Ada! Wo ist Marvin? War er nicht bei dir? | 11C0 #0 | wörtlich |
| D11C #1 | かれならと中でひきかえして / お前を捜しに行った / | Er ist unterwegs umgekehrt und wollte dich suchen. | 11C0 #1 | wörtlich |
| D11C #2 | さっき上の階で / すごいおとがしたときにな / | Vorhin, als es oben so laut krachte. | 11C0 #2 | wörtlich |
| D11C #3 | 会わなかったのか？ / | Bist du ihm nicht begegnet? | 11C0 #3 | wörtlich |
| D11C #4 | なんだって！ / | Was! | 11C0 #4 | wörtlich |
| D11C #5 | エイダ君はパトカーの中にかくれていろ / | Ada, versteck dich im Streifenwagen. | 11C0 #5 | wörtlich |
| D11C #6 | そ・そうしよう・・・ / | J-ja, das tue ich… | 11C0 #6 | wörtlich |
| D11C #7 | さぁ もうおわったぜ / | So, das war es. | 11C0 #7 | Sprecher Leon -> Ada; Disc tot, Port lebt |
| D11C #8 | ・・・とんでもない奴がいるな / | …was für ein Ungeheuer hier herumläuft. | 11C0 #8 | sinngemäß; Disc tot, Port lebt |
| D11C #9 | あぁ とにかくここはもう出よう / | Ja. Hauptsache, wir kommen hier raus. | 11C0 #9 | wörtlich; Disc tot, Port lebt |
| D11C #10 | あぁ とにかくここはもう出よう / | Ja. Hauptsache, wir kommen hier raus. (Dublette) | fehlt im Port | Dublette von #9, auf der Disc nicht ausgelöst |

Ohne Port-Gegenstück: 1 (D11C #10). Im Port vorhanden, aber ohne Aufruf, obwohl die Disc sie aufruft: D119 #1 (Schraubendreher). D117 #2 ist auf Disc und Port gleichermaßen ohne Aufruf. Zwölf auf der Disc tote Zeilen (D11B #10–#21) und drei (D11C #7–#9) laufen im Port.

## 3. Auffälligste Abweichungen (Story)

**Neue Begegnungen und Szenen, Disc ohne Text**
1. **1011 #0–#17:** Elza trifft Roy lebend und verletzt im Mod-Raum 101. Er schickt sie zu seinem Freund John in den Zellentrakt. Auf der Disc kommt Roy nur in 102 vor, dort verwandelt er sich gerade. Spätstand #027/#028 zeigt Elza bei einem verletzten Polizisten, ohne Text.
2. **1211 #0–#9:** Elza weckt John in einer Zelle in 121 („Roy sent me“). Auf der Disc steht John in 121 nicht im Skript.
3. **1141 #0–#11:** Elza und John im Mod-Raum 114. Ein „Gorilla“ bricht durchs Fenster, John lenkt ihn ab, Elza flieht allein. #5 („Don't go to the rooftop!“) deckt sich mit Spätstand-Foto #012 aus demselben Raum. Auf der Disc ruft John Elza dagegen in 117 aufs Dach.
4. **10D0 #12–#27:** Leon trifft Marvin zum ersten Mal, im Mod-Raum 10D. Die Begrüßung #14 entspricht D11B #10. Neu sind der gebissene Chef (#20), der Funkanlagen-Plan und der verschwundene Hubschrauber. #17 entspricht Spätstand #084, dort als Aussage, vermutlich von Marvin, nicht als Leons Frage.
5. **1150 #4–#14:** Irons spricht lange mit Leon: Hubschrauber als einziger Ausweg, Funkanlage defekt, Karte für den Funkraum (#11 ohne Aufruf), Andeutung, er sei gebissen worden. Disc 115 hat nur „Mir geht es gut…“.
6. **1170 #0–#16:** Der Hubschrauber startet mit den neuen Figuren Elliot und Pilot, Leon bleibt zurück. Auf der Disc ist 1170 ohne Text. Nur #16 („report this to the chief“) ist durch Spätstand #083 im selben Raum belegt.
7. **1240 #0–#5:** RE2-Vorspann mit T-Virus, Umbrella, S.T.A.R.S., Chris und Jill. Nichts davon steht auf der Disc. ROOM1240 ist der Startraum des Ports.

**Neue Fakten in Disc-Szenen**

8. **10B1 #5:** John schlägt die Kanaltunnel vor. Die Disc (D10B #3) sagt nur „hier entlang ist es kürzer, komm schnell“.
9. **1011 #8:** Roy gibt Umbrella die Schuld. Auf der Disc fällt der Name Umbrella erst in 300 und 401.
10. **1020 #0, #3:** Leon erkennt Roys Leiche und nennt ihn beim Namen. Disc 1020 hat weder Text noch Roy. Auch „Lieutenant Branagh“ (Marvins Nachname aus RE2) steht nirgends auf der Disc.
11. **1030 #8:** Leon untersucht ein Motorrad („Wonder if they're ok…“). Das ist eine indirekte Spur auf Elza, aber Mod-Text.
12. **11B1 #10/#11:** Sherry verschwindet aus dem Wagen. Das ist eine neue Mod-Szene (README Z. 362), das Motiv stammt aus D403 #5/#6.
13. **1041 #2/#3:** Elza hört einen Schuss. Mod-Szene, der Schütze bleibt ungenannt.

**Sprecherwechsel**

14. **1021 #6:** D102 #5 („Allein weiß ich nicht, was ich tun soll“) spricht im Port John. Die Satzendung のよ weist auf Elza.
15. **11C0 #7:** D11C #7 („So, das war es“) spricht im Port Ada, laut TEXTE_40 Leon. Diese Disc-Zuordnung ist nur erschlossen.
16. **1011 #7:** Leons Zeile D11B #11 („Rolltor durchbrochen, sie kommen rein“) spricht in der Mod zusätzlich Roy, in Raum 101. Der Wortlaut ist fast gleich wie 11B0 #2.

**Namen:** „Irons“ ist nur ein Präfix der Mod (1150/1151 #2), auf der Disc bleibt der Chef namenlos. „Branagh“ (1020 #3, 1021 #12) und „Elliot“ (1170) sind Mod.

**Ablauf ohne Textänderung**

17. D11B #10–#21 und D11C #7–#9 sind auf der Disc tot, weil 11C0 Bank 8 abfragt. Im Port laufen sie (README Z. 208), damit auch der Kanaldeckel-Plan mit Marvin und Ada.
18. D119 #1 (Schraubendreher nehmen) ruft die Disc auf, der Port nicht.

**Port-Zeilen (story_r36)**

19. **1091 #20 und 1210 #20** sind Port-eigene Spuren und ersetzen keine Mod-Zeile. Sie stützen sich auf Zustände der Disc (Transporter ohne Feuer, offene Zelle), nicht auf Disc-Text. „Whoever was locked up here“ (1210) setzt einen Gefangenen voraus, den nur die Mod hat: John in 1211. „shed“ (1091) passt nicht zum Transporter.
