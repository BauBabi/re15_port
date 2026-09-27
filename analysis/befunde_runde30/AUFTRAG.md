# Runde 30 (2026-09-27) — Auftrag des Nutzers, wörtlich

Stand vor der Runde: v0.8.15, Suite 360/360, master 8d83a025.
Ergebnis offen der letzten Session: „Elzas Szenario ist nicht durchspielbar. Gefahren ist
genau ein Übergang, 119 der 120 ungeraden Räume sind ungeprüft."

Nutzer: „So, jetzt gehen wir in die letzten Details."

## A — „Tür verschlossen"-Sounds aus RE2

> Room 2190 in Resident Evil 2 hat zum Beispiel Beispiele für den Sound von verschlossenen
> Türen - bei Türen mit Kartenleser oder Pincode. Diesen "Tür verschlossen" Sound würde ich
> gerne für unsere Kartenleser Türen, Pincode Türen übernehmen, wenn sie noch verschlossen
> sind. Des weiteren hat Resident Evil 2 auch noch andere verschlossen Sounds für andere
> Situationen, zum Beispiel für Türen die verschlossen sind, und wir den Schlüssel dafür
> noch nicht besitzen. Ich möchte das du ermittelst, was für Arten von "verschlossen"
> Sound es gibt und die entsprechend auch bei uns sauber einbaust an die entsprechenden
> Stellen.

## B — Karte öffnet sich nach der Irons-Cutscene (RE2 ROOM3010-Mechanismus)

> In ROOM 3010 in Resident Evil 2 gibt es nach der Cutscene eine Stelle wo die Map aufgeht
> und zeigt, wo der Spieler hin gehen muss. Auch mit einen bestimmten Sound. So einen
> ähnlichen Mechanismus möchte ich bitte nach der Irons Cutscene in Irons Office im ROOM
> 1150 ebenfalls haben, der mir dann den communication ROOM markiert. Beim Schließen dieser
> Karte kann der Room, wenn man dann die Karte erneut aufruft auf ähnliche Weise wie bei
> Resident Evil 2 damit umgegangen wird, auch bei uns damit umgegangen werden. Also entweder
> markiert bleiben, oder als besucht markiert bleiben, oder keine Ahnung - ich weiß nicht
> genau wie Resident Evil 2 das macht.

## C — Android: R1 als Umschalter statt Halten

> Beim Android Port will ich eine minimale Änderung. Dort möchte ich, das R1 nicht gedrückt
> gehalten werden muss, um die Waffe zu heben, sondern das man einmal kurz R1 andrückt,
> dann bleibt die Kampfpose vorbereitet, und drückt man erneut R1 geht die Kampfpose wieder
> zurück. Das liegt einfach daran, das auf dem Touchscreen eines Android Phones das Gedrückt
> halten von R1 mit anschließenden Drücken von Viereck einen einen Knoten in die Finger
> macht.

## D — Titelmenü blinkt zu schnell

> Im Titelbild bei der Auswahl New Game, Load Game, Option blinkt der ausgewählte Bereich in
> der Frequenz im Vergleich zum Original zu schnell.

## E — Irons Diary in Irons' Office + Memory-Card-Item

> In Irons Office möchte ich das du hinten auf seinen Tresen ein Dokument hinterlegst: Der
> Hintergrund ist FILE08 aus deinen extracted_re2_dokumente\hintergruende. Wie die Seiten
> auszusehen haben, hast du hoffentlich noch korrekt ermittelt. Das Weltmodell was da liegen
> muss ist wahrscheinlich mesh03_cf9f316d_a.png. Das soll an der Stelle sein wie in "Irons
> item.png" in rot markiert. Der Header soll sein: Irons Diary. Der Text soll sein:

```
18. September 1998:

Es kommen vermehrt Nachrichten über Übergriffe herein. Wir haben unsere polizeikräfte zur verstärkten Patrouille ausgesand. Sie werden die Täter schon zu fassen bekommen!

19. September:

Wir haben Informationen bekommen das die Täter allesamt einige Eigenschaften gemeinsam haben. Sie wirken blass / aschpfahl, ja fast schon tot, und sind auch öffentlich sehr aggressiv. Haben wir es mit einer bande von drogensüchtigen zu tun in meiner Stadt?

20. September:

Es traf mich wie ein donnerschlag. Die drogensüchtige bande fällt sogar Menschen an und frisst sie. Es sind Kannibalen. Sie sind im rausch sogar so Schmerz unempfindlich, daß sie nach Schüssen sogar weiter auf unsere polizeikräfte zutorkeln, als wäre nichts geschehen.

21. September

Jetzt such die Zivilisten schon verstärkt Zuflucht im polizeirevier vor der bande. Einige kamen gerade mit dem Leben davon nachdem sie diese Barbaren gebissen haben. Es scheinen auch immer mehr davon zu werden da draußen. Was ist da bloß los.

22. September

Ich kann es kaum glauben...
Es sind keine drogensüchtigen ...
Es sind ... ich weiß nicht ... irgendeine Art von Monster, die aussehen wie Menschen... und wir haben sie hier in unser polizeirevier rein gelassen.... viele unserer einsatzkräfte sind tot, überall ist Blut und es werden immer mehr.... Sobald die Leute gebissen worden, werden sie einer von ihnen....

26. September

Ich habe meinen Männern den Befehl gegeben auszuharren, und sich um übrige Zivilisten zu kümmern. Mir ist klar, daß jetzt schon eine Weile keine Zivilisten mehr bei uns Zuflucht suchten, aber es ist unsere gottverdammte Pflicht als polizei Offiziere auch den letzten möglichen Zivilisten zu retten.

27. September

Das wars ... jetzt hat eines dieser Dinger mich erwischt.... ich werde also auch eines von ihnen werden. Aber ich werde so lange für die Zivilisten und auch für die Kollegen kämpfen wie ich es kann. Dadurch das diese Dinger immer mehr werden, habe ich angeordnet die Rollläden als Schutz zwischen den Abschnitten runter zu lassen, um eine zusätzliche schutzbarriere zu haben. Die Abschnitte dazwischen sind von den wenig übriggebliebenen Kollegen kontrollierbar.

28. September

Jetzt auch das noch.... irgendjemand scheint uns von innen heraus zu sabotieren. Die Kommunikationsanlage wurde zerschossen und der Strom abgeschaltet... wir sind nicht mehr in der Lage die Zivilisten zu schützen, wenn wir sogar selbst schon opfer von Angriffen sind....
Es ist alles verloren... ich habe die Kollegen gebeten die restliche Ausrüstung die wir haben zu verteilen und an strategischen Punkten im polizeirevier zu hinterlegen, damit andere überlebende die im polizeirevier schutz suchen würden eine Chance haben....

Leon, Marvin, Elliott, Roy.... Ich wünsche euch, das ihr es schafft zu überleben!
```

> Zwischen den einzelnen Daten soll, wie auch in Resident Evil 2 geblättert werden.
> Außerdem möchte ich das du die Vorinstallierten Texte alle entfernst und die richtigen
> Sounds für die Textdokumente aus Resident Evil 2 übernimmst. Nach dem auflesen und
> zumachen, soll es vom Schreibtisch verschwinden. Auf "Irons items.png" in blau markiert,
> soll ein - ich glaube iot-item heißt das - der Memory Card liegen, das man aufnehmen kann.

Bild: `irons items.png` (Repo-Root, 320x240, Irons' Office ROOM1150 Cut auf den Schreibtisch;
ROT = Ablage des Dokuments, BLAU = Ablage des Memory-Card-Items).

## F — Karte: drei Nutzer-Marken + Verlust beim Laden

> 3 Marker gesetzt: Roof ist irgendwie die Wand unten blau... 2F ist jetzt unten eine Tür
> eingezeichnet auf der Karte die es nicht gibt.... und ROOM 1000 ist irgendwie jetzt blau
> eingezeichnet.... Außerdem glaube ich, das wenn das spiel Gespeichert und dann geladen
> wird, Teile der Karte die ich bereits freigeschaltet habe verloren gegangen sind....

(Das `befund.log` mit den drei F9-Marken liegt beim Nutzer neben der exe und wurde noch
nicht übergeben.)

## G — Elza: Intro-Fehler

> Und wenn wir schon bei der ELZA Implementierung waren - wähle ich am Anfang Elza aus kommt
> zum einen im Intro schon kurz ein Bild der Lobby, das sollte nicht kommen. Dann, breche
> ich das intro mit square halten ab, spielt es einfach noch einmal. und dann zum anderen,
> nach dem Intro stehe ich in der Lobby aber mit Leon statt mit Elza, und ihre Cutscene
> spielt nicht ab wie sie soll.

## H — Sicherung: unsichtbar im Hebetisch + falsches Item (Nachtrag)

> Achso, und vergessen unter C:\workspace\git\reAi_v2\build\sicherung hast du ein
> Sicherungsmodell erstellt, aber in ROOM 1170 im Modell das hochgeht ist es nicht sichtbar.
> Außerdem bekommt man nicht genau diese SIcherung als Item im Anschluss, sondern eine
> anderen Sicherung. Ergänze mir das.

(„ROOM 1170" = gemeint ist der Hebetisch in Irons' Office ROOM1150/1151, wo
`sicherung_1150.c` das Prop obj_id 4 anhängt; ROOM1170 ist der Heliport ohne Hebetisch.)

## Arbeitsregeln dieser Runde

- RE-Gate (CLAUDE.md ⛔ STOP-GATE): messen → Original disassemblieren → Adresse posten →
  dann Code. Jede Konstante mit `@0x…`.
- Beta→Retail (Memory `reai-v2-beta-zu-retail`): wo RE1.5 unfertig ist, ist RE2 Retail das
  Ziel; Beleg dann aus RE2 (`ghidra_re2_Leon.txt`, `RE2_Quellcode_*`, `info/re2leon/`,
  `re2_disasm.py`).
- Dossiers: `analysis/befunde_runde30/<thema>.md`; Sonden: `tests/unit/probes/r30_<thema>.cmake`.
- Am Ende: Paket (Windows + Linux + Android), Archiv, Tag, Push — ohne Rückfrage.
