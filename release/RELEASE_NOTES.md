# RE1.5 Port — v0.2.9 (Early Preview)

## Neu in v0.2.9 — die RE2-Spinne

Der RE2-KI-Modus ist damit fuer alle vier gemeinsamen Gegnertypen verfuegbar:
Zombie, Hund, Kraehe — und jetzt die **Spinne**.

Portiert ist der Boden-Modus vollstaendig: Fortbewegung mit drei
Geschwindigkeitsstufen, der neunphasige Sprungangriff, Biss (20 Schaden),
Trefferreaktion, Tod und Leiche. Nachgemessen im echten Raum: beide Spinnen
laufen auf dem RE2-Verhalten, bewegen sich ueber 600 Bilder mit 19
Zustandswechseln ohne Haenger, der Biss zieht korrekt ab.

Nebenbei belegt und damit **nicht** eingebaut: die RE2-Spinne hat KEIN Gift —
der einzige Schadensaufruf des Moduls beruehrt den Vergiftungs-Zustand des
Spielers nachweislich nicht.

**Spinnen an Decke und Wand behalten vorerst das RE1.5-Verhalten.** Das Original
hat dafuer eigene Modi (inklusive Abseilen am Faden); sie sind untersucht, aber
noch nicht umgesetzt. Die Zustaendigkeit ist so gesetzt, dass eine
Decken-Spinne ueber ihre ganze Lebenszeit beim alten Verhalten bleibt — kein
Bruch mitten im Kampf. Ebenfalls noch offen: die Baby-Spinne.

Testsuite 166 Pruefungen; der Raum-Test schlaegt fehl (statt zu ueberspringen),
wenn kein Raum eine Spinne aufstellt.

---

# RE1.5 Port — v0.2.8 (Early Preview)

## Neu in v0.2.8 — RE2-Gore ist sichtbar

Der Zerleger-Zweig der RE2-Zombies war in v0.2.7 zwar berechnet, aber im Bild
nicht zu sehen. Jetzt schon:

- **Abgerissene Gliedmassen verschwinden wirklich.** Das Original laeuft die
  Koerperteile flach ab — ein abgerissener Oberschenkel nimmt Unterschenkel und
  Fuss NICHT automatisch mit; stattdessen gibt es eine Weitergabe-Regel, die
  dasselbe Ergebnis erzeugt. Beides ist nachgebaut.
- **Verkohlung und Saeure-Faerbung** liegen auf den richtigen Koerperteilen. Die
  Faerbung multipliziert im Original die Beleuchtung (neutral ist nicht null) —
  mit dem urspruenglich gesetzten Startwert waere jeder unversehrte Zombie
  schwarz gewesen; das ist abgefangen.
- **Stumpf-Modell**: der Zerleger tauscht jetzt das dafuer vorgesehene
  Reserve-Modell ein (der bisherige Behelf schrieb stattdessen eine Faerbung,
  die das Original an dieser Stelle gar nicht setzt).
- **Trefferrichtung wirkt**: Treffer von hinten loesen andere Taumel- und
  Sturz-Varianten aus als Treffer von vorn (anderer Clip, anderer Rueckstoss),
  und die Zuck-Richtung spiegelt sich entsprechend.

Testsuite 164 Pruefungen; sieben Gegenproben, jede einzeln nachgewiesen.
Der RE1.5-Modus ist dreifach abgesichert unveraendert.

Noch offen: abgerissene Teile fliegen nicht weg, sondern bleiben am Koerper
(der Port hat keine Physik fuer einzelne Koerperteile) — mit Adressen
dokumentiert.

---

# RE1.5 Port — v0.2.7 (Early Preview)

## Neu in v0.2.7 — RE2-Gegner: Zucken, Ragdoll, und JEDE Waffe wirkt

**Jede Waffe loest jetzt eine Trefferreaktion aus.** Im RE2-Modus trafen sieben
von 22 Angriffsarten eine leere Stelle der Reaktionstabelle — der Zombie steckte
den Treffer wortlos weg. Ursache war eine falsche Zuordnung im Port: Die
Reaktionszeile wird im Original ueber die **Gegenstands-Nummer der gefuehrten
Waffe** gewaehlt, der Port stempelte stattdessen seine eigene Waffen-Nummer und
zeigte damit auf Felder, die es im RE2-Sinn gar nicht gibt (Magnum und
Raketenwerfer machen dort 900 Schaden und toeten sofort — die Reaktion kommt
also nie dran). Jetzt ist jede RE1.5-Waffe ihrer RE2-Entsprechung zugeordnet,
zwoelf davon als harte Identitaet (Browning, Ingram und Flammenwerfer sind in
beiden Spielen dieselbe Waffe).

**Ingram, SPAS-12 und MC51 trafen auf Distanz ueberhaupt nicht.** Ihr
Trefferpunkt-Test lief durch den kurzen Nahkampf-Kegel statt durch den
Schuss-Streifen — ein Fehler in einer Waffenliste im Schadenscode, der BEIDE
KI-Modi betraf.

**Zombies zucken beim Treffer.** Das Original dreht dafuer zwei Knochen direkt:
der Oberkoerper bekommt exakt den Gegenwinkel des Rumpfes, wodurch sich die
Drehung oben aufhebt und nur ein Versatz bleibt — das sichtbare Zucken. Vorher
bewegte sich dabei kein einziges Bild, jetzt 19 von 40.

**Zwei neue Reaktionen:** ein Ragdoll-Sturz mit echter Aufprall-Physik (der
Zombie prallt beim Aufschlag ab und steht als Kriecher wieder auf) und eine
Rutsch-Reaktion. Beide kommen mit staerkeren Waffen im Spiel tatsaechlich vor.

Testsuite 162 Pruefungen; ein Zensus faehrt alle 22 Waffen durch einen echten
Schuss und verlangt je eine tatsaechlich gefahrene Reaktion (Animationswechsel,
Zucken, Laut) — kein blosser Tabellen-Check. Im RE1.5-Modus bleibt die
Original-Logik unveraendert; das ist zusaetzlich abgesichert.

---

# RE1.5 Port — v0.2.6 (Early Preview)

## Neu in v0.2.6 — Zombie stoehnt beim Aufstehen wieder, korrekte Liegezeit

Nutzer-Report zu v0.2.5: "Das schrille Moaning kommt aktuell nur beim Sterben.
Im Original macht er es auch beim Aufstehen, nachdem man ihn niedergeschossen
hat."

**Die Ursache war grundsaetzlicher als der Laut selbst: an dieser Stelle wird
im Original gar nicht gewuerfelt.** Der Zufallsgenerator des Spiels ist eine
reine Rechenfunktion seines Eingabewerts — der gespeicherte "Zustand" wird
zwar gelesen, aber nie verwendet. Im Niedergeschossen-Zustand ist dieser
Eingabewert konstant (die komplette Aufrufkette wurde nachverfolgt: niemand
veraendert ihn unterwegs). Damit fallen beide "Wuerfe" beim Aufstehen immer
gleich aus: **der Stoehn-Laut kommt jedes Mal.** Der Port hatte daraus eine
echte Wahrscheinlichkeit von 1:16 gemacht — in 15 von 16 Faellen blieb es
still. Gemessen ueber acht Zufalls-Startwerte: vorher in fuenf von acht
Durchlaeufen gar kein Laut, jetzt in allen acht.

Aus derselben Herleitung mitkorrigiert:
- **Liegezeit nach dem Niederschuss**: immer 360 Ticks (der Port wuerfelte
  zwischen 30 und 750 — mal eine Sekunde, mal 25).
- **Standfestigkeit nach dem Aufstehen**: fester Wert statt Wurf.

Geprueft und ausgeschlossen: die in v0.2.5 eingefuehrte Stimmen-/Vorrang-
Verwaltung verwirft den Laut NICHT (im Test mitgesichert); sie blieb
unangetastet.

Nicht geaendert, weil ohne Beleg: der Grunzer beim Hinfallen bleibt bei 1/4 —
dort laeuft der Code ueber einen anderen Pfad, dessen Eingabewert nachweislich
nicht konstant ist.

Testsuite 160 Pruefungen; der neue Pin faehrt acht Durchlaeufe mit
verschiedenen Zufalls-Startwerten.

---

# RE1.5 Port — v0.2.5 (Early Preview)

## Neu in v0.2.5 — Sound-Stimmen, RE2-Trefferreaktion, RE2-Soundbank

**Gegner klangen falsch, weil die Stimmen-Verwaltung fehlte.** Im Original ist
jeder Laut fest einer von acht Klang-Stimmen zugeordnet, und auf jeder Stimme
klingt immer nur EIN Sample: ein neuer Laut schneidet den laufenden ab — oder
wird komplett verworfen, wenn der laufende Vorrang hat. Der Port legte jeden
Laut auf einem freien Kanal obendrauf. Beim Zombie (Treffer -> Sturz ->
Aufprall) lagen dadurch vier Samples uebereinander statt zwei; der Fall-Laut
faellt im Original weg, weil die Stimme noch den Treffer-Laut fuehrt. Weil die
Aufnahmen je Raum unterschiedlich lang sind, fiel das im 2F-Korridor auf und im
Dinner-Raum nicht.

**RE2-Modus: Gegner reagieren jetzt auf Treffer.** Das Original waehlt die
Reaktion ueber eine Tabelle mit zwei Eingaengen (Trefferart x Zone/Entfernung);
im Port waren BEIDE Achsen falsch belegt — die Spalte kam aus der Zielhoehe und
landete systematisch in einem leeren Tabellenfeld, und das Byte, das die
Reaktions-Phase fuehrt, wurde ueberschrieben. Damit war die Reaktion
strukturell tot. Jetzt portiert: Grunzer beim Treffer, vierphasige Reaktion,
Zurricktaumeln, leichtes Zucken, Rueckstoss, Knockdown bei Schnellfeuer, und
der Widerstandswert sinkt korrekt pro Trefferart.

**RE2-Modus: Soundbank wechselt wieder.** Sie wurde nur beim ersten Mal geladen
— nach einem Zombie-Raum spielte der Hund Zombie-Samples.

**Kleiner Bildfehler beim Sturz behoben:** im Uebergabemoment blitzte fuer ein
einzelnes Bild eine fremde Pose auf.

Als korrekt belegt und bewusst NICHT geaendert: dass der Zombie nach dem
Zuruecktaumeln seine Lauf-Animation neu startet (Original-Verhalten, vorher
durch die falsche Animationsbank verdeckt), und dass die Raeume verschiedene
Zombie-Stimmsaetze haben.

Noch offen im RE2-Modus (mit Adressen dokumentiert): das feine Zucken des
Oberkoerpers (direkt manipulierte Knochen-Matrizen), zwei von sechs
Reaktions-Routinen (Ragdoll-Sturz, Rutsch-Variante) und der Gore-/Zerleger-Zweig.

Testsuite 154 -> 159 Pruefungen, jede Korrektur mit Gegenprobe.

---

# RE1.5 Port — v0.2.4 (Early Preview)

## Neu in v0.2.4 (gegenueber v0.2.3) — drei Animations-/Sound-Fehler

Aufgefallen bei der Untersuchung einer Nutzer-Meldung ("der liegende Zombie im
2F-Korridor klingt beim Getroffenwerden/Hinfallen/Aufstehen anders als die im
Dinner-Raum"). Die Meldung selbst ist **byte-true** — die Raeume liefern im
Original verschiedene Zombie-Stimmsaetze (andere Aufnahmen, andere Tonhoehe:
11 kHz statt 8 kHz), und der Zufallsgenerator laesst die Grunzer je nach
Blickrichtung unterschiedlich fallen. Auf dem Weg dorthin kamen aber drei echte
Fehler ans Licht:

- **Rueckwaerts abgespielte Animationen spielten ihre Geraeusche zum falschen
  Zeitpunkt.** Im Original haengen Pose und Ton zwingend am selben (gespiegelten)
  Bild-Index; der Port zeichnete gespiegelt, las die Toene aber vorwaerts.
  Gemessen: ein Laut feuerte bei Zaehler 42 statt 17.
- **Ein Richtungs-Merker blieb haengen**: Ein Zombie, den man beim Hinknien zum
  Fressen unterbricht, stand danach **rueckwaerts** auf (auch sichtbar).
- **Sichtbares Stocken beim Aufstehen**: ein unbelegter Ueberblendungswert liess
  die ersten acht von 59 Bildern komplett in der alten Pose einfrieren.

Zwei falsche Faehrten sind im Quelltext dokumentiert, damit sie niemand erneut
"repariert": Der Wert, der wie eine Abspielgeschwindigkeit aussieht, steuert die
Ueberblendungsdauer (der Bildvorschub ist immer ein Bild pro Tick); und die
Vorne/Hinten-Auswahl beim Zu-Boden-Gehen gilt nur fuer den Todes-Clip, nicht fuer
den Knockdown.

Testsuite von 150 auf 154 Pruefungen; jede Korrektur mit Gegenprobe abgesichert.

---

# RE1.5 Port — v0.2.3 (Early Preview)

## Neu in v0.2.3 (gegenueber v0.2.2) — vier gemeldete Punkte

**Zombies im Dinner-Raum nach Wiedereintritt** und **Zombie-Sound beim
Hinfallen/Aufstehen** waren derselbe Fehler: Die STAGE1-Zombies beziehen ihre
Posen aus zwei Animationssaetzen, und der Port ordnete drei Zustaenden den
falschen zu. Folgen: Der Schlaefer, der im Dinner-Raum erst auftaucht, nachdem
man in die Menge gelaufen ist (der Raum schaltet dann per Ausloesebereich auf
eine andere Aufstellung um), spielte eine 14-Bilder-Aktionsanimation statt der
75-Bilder-Steh-Animation — der fruehere Anlauf liess lediglich den falschen Clip
sauber wiederholen. Und weil die Schritt- und Aufprallgeraeusche in den
Einzelbildern des richtigen Satzes stecken, war derselbe Zombie rund um den
Sturz stumm. Die Fall-/Aufsteh-Logik selbst war bereits byte-true.

**Irons schaut jetzt "diagonaler"**: Im Original folgt er Leon dauerhaft mit dem
Kopf, in beiden Achsen zugleich. Im Port lief seine Kopfsteuerung ueberhaupt
nicht an, weil ihre Initialisierung an einen Zustand geknuepft war, den das
Raumskript schon im Ladeframe ueberschreibt — im Original ruft das
Spawn-Kommando den NPC-Code dagegen sofort mit auf. Gemessen dreht sein Kopf
jetzt 54 Grad zu Leon statt starr geradeaus. Betrifft game-weit 22 weitere
NPC-Platzierungen, die im Original ebenso mit dem Kopf folgen.

**Speicher-Nummer zaehlt jetzt durch** — das war *kein* Portfehler: Das Original
zaehlt die Speicherungen des Spielstrangs (der Zaehler steckt im Spielstand und
wird beim Laden zurueckgesetzt), nicht die Karten-Slots. Auf Nutzer-Entscheid
weicht der Port hier bewusst ab und zaehlt monoton hoch; alles andere bleibt
byte-true. Die Abweichung ist im Code und in `analysis/save_counter.md`
dokumentiert.

Testsuite von 144 auf 150 Pruefungen gewachsen; jede Korrektur ist mit einer
Gegenprobe abgesichert (Fix zurueckgebaut -> Pruefung schlaegt fehl).

---

# RE1.5 Port — v0.2.2 (Early Preview)

## Neu in v0.2.2 (gegenueber v0.2.1) — START-FIX: Spiel liess sich nicht mehr starten

**Wer v0.2.1 nicht starten konnte, braucht dieses Paket.** Symptom: Doppelklick auf
`re15_pc.exe` (oder `Start_RE15_Port.bat`) erzeugte einen Prozess im Hintergrund, aber
KEIN Fenster — und nicht einmal eine `debug.log`.

Ursache (per Debugger am haengenden Prozess belegt): Die exe war eine **Konsolen**-
Anwendung (PE-Subsystem 3). Ohne bereits offene Konsole muss der Windows-Loader erst
eine erzeugen, und genau dort blockierte er — **bevor** die erste Zeile des Programms
lief:

```
#0  ntdll!ZwCreateFile
#1  KERNELBASE!AttachConsole
#14 ntdll!LdrLoadDll
#19 ntdll!LdrInitializeThunk      <- noch im Loader, vor main()
```

Aus einer bereits offenen Konsole gestartet lief dieselbe exe normal durch (sie erbt
die Konsole) — daher war der Fehler so verwirrend. `start ""` in der .bat legte
ebenfalls eine neue Konsole an und half deshalb nicht.

Fix: Die exe ist jetzt eine **GUI-Anwendung** (Subsystem 2) — ein SDL-Spiel braucht
keine Konsole und umgeht den Loader-Pfad komplett. Damit Kommandozeilen-Laeufe und die
Testsuite ihre Ausgabe behalten, haengt sich das Programm beim Start an eine *bereits
vorhandene* Eltern-Konsole an (erzeugt keine neue). Zusaetzlich: `start ""` aus der
.bat entfernt. Nebeneffekt: beim Doppelklick oeffnet sich kein schwarzes Konsolen-
fenster mehr.

Verifikation ueber genau den Startweg, der vorher hing:
| | v0.2.1 | v0.2.2 |
|---|---|---|
| Threads | 1 | 14 |
| CPU | 0 % | 0,47 % |
| Fenster | keins | sichtbar, 976x759 |
| `debug.log` | wird nicht geschrieben | wird geschrieben |

Neu im Repo: `tools/diag_window.ps1` — listet ALLE Fenster eines Prozesses (auch
unsichtbare oder ausserhalb des Bildschirms), die Bildschirme und die `debug.log`.
Damit laesst sich "kein Fenster erzeugt" von "Fenster unsichtbar" und "Fenster
ausserhalb des Desktops" unterscheiden.

Inhaltlich identisch zu v0.2.1 (alle 15 dort behobenen Abweichungen sind enthalten);
Testsuite 144/144 auf beiden Plattformen.

---

# RE1.5 Port — v0.2.1 (Early Preview)

## Neu in v0.2.1 — 15 gemeldete Abweichungen behoben

Aus einem Spieltest von v0.2 kamen 17 Beobachtungen. 15 davon waren echte
Abweichungen vom Original und sind behoben; 2 waren korrektes Verhalten und
bleiben unveraendert (unten begruendet). Jede Korrektur traegt ihre
Disassembly-Adresse im Quelltext; die Testsuite ist von 132 auf 144 Pruefungen
gewachsen.

**Die Wurzel gleich zweier Meldungen:** Bei Nicht-Spieler-Figuren lief eine
Posen-Ueberblendung nie aus — der Port mischte dauerhaft 7/8 der vorherigen Pose
bei, wodurch die Beine nur ~39 % ihrer Schwingung erreichten. Das war das
"Gleiten/Schlurfen" von Elliot (Intro) und Marvin (2F-Korridor). In allen
Original-Savestates steht der zugehoerige Zaehler auf 0.

Behoben:
- **Elliot-Intro**: Laufanimation (s.o.) und der harte Szenenschnitt — der
  Uebergang laeuft jetzt wie im Original ueber Schwarzblende und Einblendung.
- **Kurz sichtbarer Helipad-Hintergrund** direkt nach der Charakterwahl: der
  Boot-Notbehelf zeichnete eine Datei, die byte-identisch mit dem
  Helipad-Hintergrund ist; jetzt bleibt das Bild schwarz wie im Original.
- **Helikopter-Rotor im Intro**: zwei portseitige Erfindungen (Lautstaerke nach
  Kameraposition, Stummschaltung nach Spielerzustand) entfernt. Die eine hob
  zusaetzlich das Skript-Stop des Sounds jeden Frame wieder auf. Nachgemessen am
  Tonausgang: Rotor jetzt hoerbar mit korrekter Rotorblatt-Frequenz, und still,
  sobald das Skript ihn stoppt.
- **Leons Kopfhaltung** im und nach dem Intro (er verfolgte durch einen
  Rechenueberlauf eine weit entfernte Figur) sowie seine **Blickrichtung** in der
  Marvin-Szene.
- **Leon schaut liegenden Gegnern nach** — diese Automatik fehlte komplett.
- **Texte frieren das Spiel ein**: Beim Untersuchen steht jetzt alles still
  (Gegner, Animationen, Skript) bis der Text weg ist — wie im Original. Die
  Untertitel der Zwischensequenzen frieren weiterhin nicht ein; diese
  Unterscheidung steckt in den Spieldaten selbst.
- **Modelle nach Tod und NEW GAME** sowie **Messer-Animationen trotz Pistole nach
  dem Laden**: beides waren Zustaende, die einen Neustart ueberlebten.
- **Item-Aufnahme-Text** laesst sich jetzt beschleunigen (Taste halten).
- **Zombie-Menge im Dinner-Raum** nach erneutem Betreten (standen als Statue).
- **Hintere Tuer im 2F-Korridor** zeigte die Hintergruende des Helipads: 13
  Hintergrund-Dateien des Evidence-Korridors waren aus dem falschen Quell-Archiv
  geschnitten. Ein neuer Datentest prueft jetzt alle 156 Raeume.
- **Zombie-Positionen im Evidence-Korridor** und **Zombie-Sounds beim
  Hinfallen/Aufstehen** (RE2-Modus: Sounds spielten in falschen Zustaenden und
  dreimal zu haeufig).

Unveraendert, weil im Original genauso:
- **Verletzt-Idle nach dem Laden**: Das Original startet die Idle-Animation nach
  jedem Laden neutral und wechselt erst nach einigen Sekunden in die verletzte
  Variante — und nur unter halber Gesundheit. Die HUD-Warnung "CAUTION" erscheint
  aber schon deutlich frueher. Ein Spielstand in diesem Fenster zeigt also
  CAUTION ohne Verletzt-Animation, im Original wie hier.
- **Irons' Kopfdrehung**: war Teil desselben Klemmen-Fehlers wie Leons
  Blickrichtung und ist damit miterledigt.

---

## Neu in v0.2 (gegenueber v0.1.2) — RE2-KI-Modus fuer Zombies, Hunde, Kraehen

Im OPTIONS-Menue schaltet der Eintrag **AI: RE1.5 / RE2** die drei Gegnertypen,
die es auch im fertigen Retail-RE2 gibt, auf deren **vollstaendiges
RE2-Original-Verhalten** um — Gehirn, Animationen, Sounds und Effekt-Ablaeufe
byte-genau aus den RE2-Overlays re-engineert (EMZ0 / EMD0G_MOD0 / EMOVL21,
jede Konstante mit Disassembly-Zitat im Code, Master-Doku `RE15_RE2_AI.md`):

- **Zombie** (Wellen A+B): RE2-Gangarten-Maschine, Angriffs-Leiter, 10-Phasen-
  Grab mit Biss-Frames/Schaden aus dem Original-Parameterblock, Partner-
  Aufweck-Domino, dreistufige Trefferreaktion, RE2-Modelle/Clips + ENEMSE-SEs.
- **Hund/Cerberus** (Welle C): 17 Zustaende — Stalking mit Aggro-Meter,
  4-Phasen-Verfolgung mit Ermuedung, Sprungattacke mit echter Flugbahn,
  **Kehlen-Latch als Finisher** (nur bei toedlichem Biss; per Tasten-Hammern
  abschuettelbar), Rudel-Heul-Koordination.
- **Kraehe** (Welle D): 15 Zustaende — Schwarm-Koordination (es greift immer
  nur EINE an), Sichtpruefung, Sturzflug-Hieb, Festkrallen mit Pick-Schaden,
  Spiralsturz mit Erholung, zuckende Leiche mit wachsender Blutlache.
- Die **Spinne** folgt in einer spaeteren Version.

Der RE1.5-Modus (Default) bleibt byte-identisch unberuehrt. Qualitaetssicherung:
je Welle ein adversarieller Multi-Agent-Review (alle bestaetigten Divergenzen
vor dem Commit gefixt), 132 Engine-Tests inkl. A/B-Proben RE1.5-vs-RE2 in
ROOM1140/1190/10C0 (Biss exakt 20 HP, Peck exakt 5 HP, keine Doppel-Claims).

Paketierung: `shared_assets/RE2/` (CDEMD0.EMS + ENEMSE.VBS, ~18 MB) ist neu im
Paket; die Startskripte exportieren `RE15_RE2_ASSET_ROOT`, und
`release/make_package.sh` bricht ab, wenn die RE2-Assets fehlen.
Die Dateinamen unten heissen entsprechend `re15_port_v0.2_*`.

---

## Neu in v0.1.2 (gegenueber v0.1.1) — Linux-/Steam-Deck-Paket repariert
Das v0.1.1-Deck-Paket war in vier Punkten falsch geschnuert. Alle vier sind
belegt und behoben; `release/make_package.sh` bricht jetzt ab, statt sie
noch einmal auszuliefern:

| Defekt in v0.1.1 | Wirkung auf dem Deck | Fix |
|---|---|---|
| Binary enthielt `%sBSS/%s/BG%02d.BSS` (kleingeschriebener Raumordner), der Baum heisst `BSS/ROOM1170/` | ext4 ist case-sensitiv -> **kein Raumhintergrund**, alles schwarz | `bg_pc.c` schreibt den Pfad gross (wie der PRI-Lader daneben); Gate `check_binary_paths` |
| `shared_assets/extracted_fx/` fehlte im Paket (0 Dateien) | **Blut, Muendungsfeuer, Rauch, Huelsen rendern nicht** | wird mitpaketiert; Gate `check_tree` |
| `run.sh` war 53 Bytes (`cd` + `exec`) — kein `RE15_CD_ROOT`, kein Render-Backend | Effekt-Texturen unauffindbar; Fades/Cutscene-Balken **weiss statt schwarz** (Mesa kann den subtraktiven Blend nur unter `opengles2`) | `release/pkg_files/linux/run.sh`, jetzt im Repo versioniert |
| Build gegen `ubuntu:22.04` -> `GLIBC_2.34` | startet auf SteamOS 3.4 (glibc 2.33) **gar nicht** | Build auf Debian-11-/sniper-Basis (`GLIBC_2.29`); Gate `check_glibc` |

Ursache dahinter: `pkg-linux/` ist gitignoriert und wurde von Hand befuellt —
Startskript und Paketinhalt hatten keine versionierte Quelle. Jetzt:
`release/pkg_files/` (Startskripte + README-Vorlagen) + `release/make_package.sh`.

Ausserdem enthalten:
- 1-Frame-ROOM1170-BG-Leck zwischen Player-Select und Intro beseitigt.
- `test_em_status_persist` repariert (nur Test, keine Engine-Aenderung). Er stuerzte in
  ~1 von 150 Laeufen ab und blockierte damit das Release-Gate. Ursache (ASan:
  stack-buffer-underflow in `scd_vm_tick`, `scd_vm.c:594`): der Re-Entry-Teil baute den
  Raum-Wiedereintritt mit `re15_actor_init()` nach, liess Slot 1 aber mit dem geparkten
  Thread des vorigen Spawns belegt. `scd_thread_start` verweigert einen belegten Slot
  (`scd_vm.c:433`), also lief Pruefung (5) NIE — sie hielt aus dem falschen Grund —, und
  der geparkte `pc` zeigte auf einen toten Stack-Puffer. Jetzt benutzt der Test die echte
  Kette `scd_room_reenter` (byte-true FUN_8001d600 -> FUN_800396fc -> FUN_8003ef6c); die
  Pruefung laeuft wirklich und haelt (400/400 sauber, ASan-frei).

## Neu in v0.1.1 (gegenueber v0.1)
- **Elliot-Intro (ROOM1170) byte-true:** Der Renn-Glide ist behoben, und Elliot laeuft
  jetzt vollstaendig ueber die NPC-Sub-VM mit seiner EIGENEN EM047-Animationsbank
  (Roadmap 7b komplett; inkl. eines nachgezogenen Original-Details: das
  SCD-Ankunfts-Flag des Turn-Subs @0x80051dd8).
- **Debug-Menue (SELECT) byte-true:** Original-Navigationsgefuehl (Auto-Repeat-Kadenz
  0/6/8/10), Kreuz/SELECT schliessen per Tasten-EDGE (gehaltene Renn-Taste schliesst
  nicht mehr sofort), 8x8-Original-Debugfont aus TEX.TIM statt der Spielschrift,
  subtraktive Hintergrund-Box, Raumnummern mit fuehrender Null. Bestaetigung = ▢.
- Test-Suite-Fix (Use-after-free, nur Tests; Linux-Gate deckte ihn auf).
Die Dateinamen unten heissen entsprechend `re15_port_v0.1.1_*`.

---

# v0.1 (Basis)

Erster eigenstaendiger PC-Release des Resident-Evil-1.5-Ports (MZD-Build-Basis).
Zwei Pakete (je ~135 MB gezippt, ~290 MB entpackt, Assets enthalten).
**Die ZIPs liegen als Standard-SPLIT-ZIPs direkt in diesem Repo-Ordner**
(Volumes `.z01` + `.zip`, je < 100 MB): einfach die `.zip` mit 7-Zip, WinRAR
oder (Steam Deck) Ark oeffnen — die Volumes werden automatisch als EIN Archiv
gelesen. Hinweis: der Windows-Explorer-eigene ZIP-Handler kann KEINE
Split-Zips — 7-Zip/WinRAR verwenden. Pruefsummen: `SHA256SUMS.txt`.

| Paket | Plattform | Start |
|---|---|---|
| `re15_port_v0.2.9_win64.zip` | Windows x64 | `Start_RE15_Port.bat` doppelklicken |
| `re15_port_v0.2.9_linux_steamdeck_x64.zip` | Linux x64 / Steam Deck (SteamOS 3.x) | `./run.sh` (Deck: als Non-Steam-Game hinzufuegen) |

Beide Pakete sind selbst-enthalten: SDL2 statisch, Assets unter `shared_assets/PSX`
(CD-Baum) plus `shared_assets/extracted_fx` (Effekt-Texturen), Savegames als
PSX-Memory-Card-Image `re15_card.mcr` neben der Anwendung. Gamepads via
SDL-GameController (Steam Deck nativ).
**Startskript benutzen, nicht das Binary direkt** — nur `run.sh` bzw.
`Start_RE15_Port.bat` setzen `RE15_ASSET_ROOT`/`RE15_CD_ROOT`, ohne die findet
der Port `extracted_fx/` nicht (Effekte fehlen dann still).

## Steuerung (Tastatur, DuckStation-Layout)
Pfeile/WASD laufen · K/Shift rennen (✕) · J Aktion (▢, haelt Texte schneller) ·
L Abbrechen (◯) · I Inventar (△) · E zielen (R1) + J Schuss · Enter Start ·
Backspace Select · 1/3 = L2/R2.

## Stand v0.1 (Auszug)
- Byte-true re-engineerte Kernsysteme: Spieler/Kampf/Gegner-KI (alle STAGE1-Typen
  inkl. ROOM1030-Kriechtor mit Hand-Lock-Root-Motion), AOT/SCD-VM, Kamera-Zonen,
  Tuersystem (inkl. Fahrstuhl), Save/Load mit Ortsnamen, Inventar/Item-Box,
  Effekt-Pipeline (additive/subtraktive Blends), Blut-Decals, BGM/SE.
- Verifikation: 123 Engine-Tests (ctest) + Hardware-Gegenmessungen (PCSX-Redux)
  + Savestate-Vergleiche; Details in `analysis/` und `ROADMAP_ROOMCHAIN.md`.

## Bekannte Luecken (ehrlich)
- STAGE-2-6-Inhalte in unterschiedlicher Tiefe; diverse dokumentierte OFFEN-Punkte
  (siehe ROADMAP_ROOMCHAIN.md §5 / UNTESTED_IMPLEMENTATIONS.md).
- Steam Deck: Tastatur-Layout ist Default — fuer Gamepad ggf. Steam-Input-Profil.
- v0.1 = Preview: Abstuerze/Regressionen bitte mit Raum + Schrittfolge melden.

## Build-Reproduktion
Windows: `cmake -S re15_port -B build -G Ninja -DRE15_BUILD_PC=ON -DCMAKE_BUILD_TYPE=Release`
(Binary nach `release/win_out/re15_pc.exe`).

Linux/Deck — **Debian-11-Basis (glibc 2.31), nicht ubuntu:22.04**:
```bash
release/build_linux_deck.sh                    # docker/podman, Image debian:11
release/build_linux_deck.sh --distrobox re15-build   # auf dem Deck selbst
```
Paketieren (beide Plattformen, mit den Gates oben):
```bash
release/make_package.sh --version v0.2.9               # --only linux | --only win
```
Ergebnis: `re15_port_v0.2.9_{linux_steamdeck_x64,win64}.{z01,zip}` + `SHA256SUMS.txt`.
